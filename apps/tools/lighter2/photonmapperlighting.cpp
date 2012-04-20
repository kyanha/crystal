/*
  Copyright (C) 2008 by Greg Hoffman
  Portions Copyright (C) 2009 by Seth Berrier

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "common.h"
#include "photonmapperlighting.h"
#include "material.h"
#include "scene.h"

#if defined(_OPENMP)
#include "omp.h"
#endif

namespace lighter
{
  PhotonmapperLighting::PhotonmapperLighting ()
  {
    // Cache global photon map settings
    searchRadius =         globalConfig.GetIndirectProperties() .sampleDistance;
    numPhotonsPerSector =  globalConfig.GetIndirectProperties() .numPhotons;
    numCausticPhotonsPerSector = globalConfig.GetIndirectProperties() .numCausticPhotons;
    numSamplesForDensity = globalConfig.GetIndirectProperties() .maxDensitySamples;

    // Cache global final gather settings
    finalGather =             globalConfig.GetIndirectProperties() .finalGather;
    numFinalGatherMSubdivs  = globalConfig.GetIndirectProperties() .numFinalGatherRays;
    numFinalGatherNSubdivs  = globalConfig.GetIndirectProperties() .numFinalGatherRays;

    // Determine which bounces should be stored
    directLightEnabled =
      (globalConfig.GetLighterProperties ().directLightEngine ==
          LIGHT_ENGINE_PHOTONMAPPER);

    indirectLightEnabled =
      (globalConfig.GetLighterProperties ().indirectLightEngine ==
          LIGHT_ENGINE_PHOTONMAPPER);

    // Initialize random number generators
    randVect.Initialize();
    randFloat.Initialize();
  }

  PhotonmapperLighting::~PhotonmapperLighting () {}

  void PhotonmapperLighting::SetPhotonStorage(bool storeDirectPhotons,
          bool storeIndirectPhotons)
  {
    directLightEnabled = storeDirectPhotons;
    indirectLightEnabled = storeIndirectPhotons;
  }

  csColor PhotonmapperLighting::ComputeElementLightingComponent(Sector* sector,
      ElementProxy element, SamplerSequence<2>& lightSampler,
      bool recordInfluence)
  {
    // Get the position, normal and containing object for this element
    csVector3 pos = element.primitive.ComputeElementCenter(element.element);
    csVector3 normal = element.primitive.ComputeNormal(pos);
    Object* obj = element.primitive.GetObject();

    // Now compute like we would a vertex
    return ComputePointLightingComponent(sector, obj, pos, normal, lightSampler);
  }

  csColor PhotonmapperLighting::ComputePointLightingComponent(Sector* sector,
        Object* obj, const csVector3& point, const csVector3& normal,
        SamplerSequence<2>& lightSampler)
  {
    // Color to return
    csColor c(0.0f, 0.0f, 0.0f);

    // Are we doing a final gather ?
    if (finalGather)
    {
      // Attempt secondary calculation (cache lookup)
      if(sector->SampleIRCache(point, normal, c))
      {
        globalStats.photonmapping.irCacheSecondary++;
      }

      // Secondary calculation failed, fall-back to primary
      // calculation (single-bounce monte carlo raytracing)
      else
      {
        // Accumulate the energy from the FG rays in 'final'
        csColor final(0.0f, 0.0f, 0.0f);

        // Accumulate the recipricol length of each ray
        float meanDist = 0.0;

        // Count the rays that hit something
        size_t rayCount = 0;

        // Loop through an M by N grid of sample rays
        for (size_t j = 1; j <= numFinalGatherMSubdivs; j++)
        {
          for (size_t i = 1; i <= numFinalGatherNSubdivs; i++)
          {
            // Use stratified sampling to sample the hemisphere above our point
            csVector3 sampleDir = StratifiedSample(normal, i, j,
                  numFinalGatherMSubdivs, numFinalGatherNSubdivs);

            // Build a hit and ray structure to use for Final Gather Rays
            lighter::HitPoint hit;
            hit.distance = FLT_MAX*0.9f;

            lighter::Ray ray;
            ray.type = RAY_TYPE_OTHER2;   // Special type for Final Gather rays
            ray.direction = sampleDir;
            ray.origin = point;
            ray.minLength = 0.01f;

            // Trace the final gather ray
            if (lighter::Raytracer::TraceClosestHit(sector->kdTree, ray, hit)
                && hit.primitive)
            {
              // Compute the direction to the source point
              csVector3 dirToSource = point - hit.hitPoint;
              meanDist += dirToSource.InverseNorm();
              rayCount++;
              dirToSource.Normalize();

              // Calculate the normal at the hit point
              csVector3 hNorm = hit.primitive->ComputeNormal(hit.hitPoint);

              // Make sure normal is facing towards source point
              if(dirToSource*hNorm < 0.0) hNorm -= hNorm;

              // Sample the photon map at the hit point and accumulate the energy
              final += sector->SamplePhoton(hit.hitPoint, hNorm, searchRadius);
            }
          }
        }

        // Normalize the accumulated energy
        c = final * (PI / (numFinalGatherMSubdivs*numFinalGatherNSubdivs));

        // Cache the results if we accumulated some energy
        if(rayCount > 0)
        {
          #pragma omp critical
          sector->AddToIRCache(point, normal, c, rayCount/meanDist);
          globalStats.photonmapping.irCachePrimary++;
        }
      }
    }

    else
    {
      // Just lookup irradiance in the photon map (no raytracing)
      c = sector->SamplePhoton(point, normal, searchRadius);
    }

    return c;
  }

  void PhotonmapperLighting::BalancePhotons(Sector *sect, Statistics::Progress& progress)
  {
    // Balance the PhotonMap KD-Trees
    progress.SetProgress(0.0f);
    Statistics::ProgressState progressState(progress, (int)(0.5*sect->GetPhotonCount()));

    sect->BalancePhotons(progressState);

    progress.SetProgress(0.0f);

    // Save the photon map if requested
    if(globalConfig.GetIndirectProperties ().savePhotonMap)
    {
      static int secCount = 0;
      char filename[30];
      sprintf(filename, "photonmap%d.dat", secCount);
      sect->SavePhotonMap(filename);
      secCount++;
    }
  }

  bool PhotonmapperLighting::ParseSector(Sector *sect)
  {

    bool causticSector = false;
    
    ObjectHash::GlobalIterator objIt = sect->allObjects.GetIterator ();
    while (objIt.HasNext ())
    {

      bool produceCaustic = false;
      csRef<Object> obj = objIt.Next ();
      csString matName = obj->materialName;
      Scene * curScene = sect->scene;
      lighter::RadMaterial * radMat = curScene->radMaterials[matName];
      
      if (radMat)
      {
        if (radMat->produceCaustic)
        {
          produceCaustic = true;
          causticSector = true;
        }
      }
      if (produceCaustic)
      {
        csSphere const boundingSphere = obj->GetBoundingSphere();
        causticList.Push(boundingSphere);
      }
    }
    return causticSector;
  }

  void PhotonmapperLighting::EmitPhotons(Sector *sect, Statistics::Progress& progress)
  {
    progress.SetProgress(0.0f);

    // Sleep Time
	//Sleep(10000);
    // Determine maximum allowed photon recursion
    size_t maxDepth = 0;
    if(indirectLightEnabled)
    {
      maxDepth =
        (size_t)globalConfig.GetIndirectProperties ().maxRecursionDepth;
    }

    // Iterate through all the non 'Pseudo Dynamic' light sources
    const LightRefArray& allNonPDLights = sect->allNonPDLights;
    Statistics::ProgressState progressState(progress, numPhotonsPerSector);

    // Iterate over the lights to determine the total lumen power in the sector
    double sectorLumenPower = 0;
    for(size_t lightIdx = 0; lightIdx < allNonPDLights.GetSize(); ++lightIdx)
    {
      Light* curLight = allNonPDLights[lightIdx];
      csColor pow = curLight->GetColor()*curLight->GetPower()*
        globalConfig.GetLighterProperties ().PMLightScale;
      sectorLumenPower += (pow.red + pow.green + pow.blue)/3.0;
    }

    
    // Check if caustics are enabled
    bool enableCaustics = false;

    if (globalConfig.GetIndirectProperties().caustics)
    {
      enableCaustics = true;
      if (!ParseSector(sect))
      {
        numCausticPhotonsPerSector = 0;
        enableCaustics = false;
      }
    }

    for (size_t lightIdx = 0; lightIdx < allNonPDLights.GetSize(); ++lightIdx)
    {
      // Get the position, color and power for this light source
      Light* curLight = allNonPDLights[lightIdx];
      const csVector3& pos = curLight->GetPosition();
      csVector3 photonOrigin = pos;
      const csColor& color = curLight->GetColor()*
          globalConfig.GetLighterProperties ().PMLightScale;
      const csColor& power = curLight->GetPower();

      // Determine type of light source
      csLightType curLightType = CS_LIGHT_POINTLIGHT;
      if(dynamic_cast<SpotLight*>(curLight) != NULL) curLightType = CS_LIGHT_SPOTLIGHT;
      else if(dynamic_cast<DirectionalLight*>(curLight) != NULL) curLightType = CS_LIGHT_DIRECTIONAL;

      csColor pow = color*power;

      // How many photons does this light get (proportional to the fraction
      // of power this light contributes to the sector)?
      double powerScale = ((pow.red + pow.green + pow.blue)/3.0)/sectorLumenPower;
      size_t photonsForCurLight = floor(powerScale*numPhotonsPerSector + 0.5);

      size_t causticPhotonsForCurLight = 0;
      
      if (enableCaustics)
      {
        causticPhotonsForCurLight = floor(powerScale*numCausticPhotonsPerSector + 0.5);
      }

      bool warning = false, stop = false;

      // Setting one time light properties

      float spRadius, innerFalloff, outterFalloff;
      spRadius = innerFalloff = outterFalloff = 0.0f;
      csVector3 lightDir (0.0f);
      switch (curLightType) 
      {

        // directional light
      case CS_LIGHT_DIRECTIONAL:
        {

          lightDir = ((DirectionalLight*)curLight)->GetDirection();
          spRadius = ((DirectionalLight*)curLight)->GetRadius();
        }
        break;

        // spotlight
      case CS_LIGHT_SPOTLIGHT:
        {
          if(!warning)
          {
            globalLighter->Report (
              "Spotlight falloff is ignored for indirect light calculation");
            warning = true;
          }

          // Get spotlight properties
          
          lightDir = ((SpotLight*)curLight)->GetDirection();
          ((SpotLight*)curLight)->GetFalloff(innerFalloff, outterFalloff);

        }
        break;
	
	// point light
      case CS_LIGHT_POINTLIGHT:
	// Nothing special to do
	break;

      }

      // Loop to generate the requested number of photons for this light source
    #if defined(_OPENMP)
      omp_set_num_threads(omp_get_num_procs()*2);
    #endif

	  if(!stop)
	  {

		  #pragma omp parallel for
		  for (size_t num = 0; num < photonsForCurLight; ++num)
		  {
			// Get direction to emit the photon
			csVector3 dir;

			switch (curLightType) 
			{

			  // directional light
			  case CS_LIGHT_DIRECTIONAL:
				{
				  photonOrigin = DirectionalLightScatter(lightDir,spRadius,pos);
				  dir = lightDir;
				}
				break;

			  // spotlight
			  case CS_LIGHT_SPOTLIGHT:
				{
				  // Generate a random direction within the spotlight cone
				  dir = SpotlightDir(lightDir, outterFalloff);
				}
				break;

			  // Default behavior is to treat a light like a point light
			  case CS_LIGHT_POINTLIGHT:
			  default:
				{
				  // Generate a random direction vector for uniform light source sampling
				  dir = randVect.Get();
				}
				break;
			}

			// Emit a single photon into the sector containing this light
			const PhotonRay newPhoton = { photonOrigin, dir, color, power, curLight, RAY_TYPE_OTHER1, 1.0f };
			EmitPhoton(sect, newPhoton, maxDepth, 0, !directLightEnabled, false);
			progressState.Advance();
		  }
	  }

      if (enableCaustics)
      {

        // Calculate volume ratio for caustic objects

        float totalVolume = 1.0;

        csArray<csSphere>::Iterator itr = causticList.GetIterator();
        for (; itr.HasNext();)
        {
          csSphere sphere = itr.Next();
          float radius = 0;
          radius = sphere.GetRadius();
          totalVolume += radius*radius*radius;
        }

        itr = causticList.GetIterator();

        for (;itr.HasNext();)
        {
          csSphere sphere = itr.Next();
          float radius = 0;
          radius = sphere.GetRadius();
          size_t causticPhotonsForMesh = floor(causticPhotonsForCurLight*(radius*radius*radius/totalVolume) + 0.5);

          // Setting one time light properties
      
          float spRadius, spanAngle;
	  spRadius = spanAngle = 0.0f;
          csVector3 lightDir (0.0f);
	  csVector3 objDir (0.0f);
	  csVector3 pseudoPos (0.0f);

          switch (curLightType)
          {

            // directional light
          case CS_LIGHT_DIRECTIONAL:
            {
              lightDir = ((DirectionalLight*)curLight)->GetDirection();
              spRadius = ((DirectionalLight*)curLight)->GetRadius();

              csVector3 posDir = sphere.GetCenter()-pos;
              csVector3 lightDirVector = posDir*lightDir;
              csVector3 radVector = lightDirVector-posDir;
              float parallelDist = radVector.SquaredNorm();
              pseudoPos = pos;

              if (spRadius >= parallelDist)
              {
                float tempDist = csMax (0.f, parallelDist + radius - spRadius);
                pseudoPos = pos + radVector*(1 - tempDist/(parallelDist*2.0f));
                spRadius = radius - tempDist/2.0f;
              }
              else
              {
                stop = true;
                continue;
              }
            }
            break;

            // spotlight

          case CS_LIGHT_SPOTLIGHT:
            {
              if(!warning)
              {
                globalLighter->Report (
                  "Spotlight falloff is ignored for indirect light calculation");
                warning = true;
              }

              // Get spotlight properties
              float innerFalloff, outterFalloff;
              lightDir = ((SpotLight*)curLight)->GetDirection();
              ((SpotLight*)curLight)->GetFalloff(innerFalloff, outterFalloff);

              objDir = sphere.GetCenter() - pos;
              spanAngle = ABS(atan(radius/objDir.SquaredNorm()));

              objDir.Normalize();
              lightDir.Normalize();
              float angleDir = ABS(lightDir*objDir);

              if ( ((spanAngle/2.0) +angleDir) > ABS(acos(outterFalloff)))
              {
                stop = true;
                continue;
              }
              
            }
            break;

            // point light
          case CS_LIGHT_POINTLIGHT:
          default:
            {
              // Get the direction of the caustic object w.r.t the light
              
              objDir = sphere.GetCenter() - pos;
              spanAngle = atan(radius/objDir.SquaredNorm());
              objDir.Normalize();
              
            }
            break;
          }

          // Emit caustic photons for the current light

		  if(!stop)
		  {
			  #pragma omp parallel for
			  for (size_t cnum = 0; cnum < causticPhotonsForMesh; ++cnum)
			  {
				// Get direction to emit the photon
				csVector3 dir;

				switch (curLightType)
				{

				  // directional light
				  case CS_LIGHT_DIRECTIONAL:
					{

					  csVector3 emitPos = DirectionalLightScatter(lightDir,spRadius,pseudoPos);
					  photonOrigin = emitPos;
					  dir = lightDir;
	                 
					}
					break;

				  // spotlight
	              
				  case CS_LIGHT_SPOTLIGHT:
					{
					  // Generate a random direction within the spotlight cone towards the caustic
					  dir = CausticDir(objDir, spanAngle);
					}
					break;
	              
				  // point light
				  case CS_LIGHT_POINTLIGHT:
				  default:
					{
					  dir = CausticDir(objDir,spanAngle);
					}
					break;
				}

				// Emit a single photon into the sector containing this light
				const PhotonRay newPhoton = { photonOrigin, dir, color, power, curLight, RAY_TYPE_OTHER1, 1.0f };
				EmitPhoton(sect, newPhoton, maxDepth, 0, !directLightEnabled,true);
	            
			  }
		  }
        }
      }

    }

    // Scale the photons for this light
    sect->ScalePhotons(1.0/numPhotonsPerSector);
    if (enableCaustics)
    {
      sect->ScaleCausticPhotons(1.0/numCausticPhotonsPerSector);
    }

    progress.SetProgress(1.0f);
  }

  void PhotonmapperLighting::EmitPhoton(Sector* &sect, const PhotonRay &photon,
    const size_t &maxDepth, const size_t &depth, const bool &ignoreDirect,bool produceCaustic = false)
  {
    // Check recursion depth
    if(depth > maxDepth)
    {
      return;
    }

    
    // Trace this photon using the standard raytracer
    lighter::HitPoint hit;
    hit.distance = FLT_MAX*0.9f;
    lighter::Ray ray = photon.getRay();
    bool hitResult;
    #pragma omp critical
    hitResult = lighter::Raytracer::TraceClosestHit(sect->kdTree, ray, hit); 

    if (hitResult)
    {
      // TODO: Why would a hit be returned and 'hit.primitive' be NULL?
      if (!hit.primitive || !hit.primitive->GetMaterial()) { return; }

      // Get the surface normal and direction from light source
      csVector3 N = hit.primitive->ComputeNormal(hit.hitPoint);
      csVector3 L = photon.direction;
      N.Normalize(); L.Normalize();

      
      // Compute reflected color intensity (lambertian)
      // Compute the UV coordinates for the hit point for the primitive
      csVector2 uvCordOnPrimitive = hit.primitive->ComputeUV(hit.hitPoint);

      // Get the color of the primitive at that point
      const lighter::RadMaterial* hitPtMaterial = hit.primitive->GetMaterial();
      static csColor hitPtColor (1,1,1);
      if(hitPtMaterial->IsTextureValid())
      {
        hitPtColor = hitPtMaterial->GetTextureValue(uvCordOnPrimitive);
      }
      float dot = ABS((-L)*N);
      csColor reflColor = photon.color*photon.power*dot*hitPtColor;
      float refrIndex  =  hitPtMaterial->refractiveIndex;

      // Record photon based on enabled options
      if((depth == 0 && !ignoreDirect) || depth > 0)
      {
        if(!produceCaustic)
        {
	  #pragma omp critical
          sect->AddPhoton(reflColor, hit.hitPoint, L);
        }
        else if (!hitPtMaterial->produceCaustic)
        {
          // Add the photon to the caustic photon map
	  #pragma omp critical
          sect->AddCausticPhoton(reflColor, hit.hitPoint, L);
          return;
        }
      }

      // If indirect lighting is enabled, scatter the photon
      static csColor blackColor (0,0,0);
      if(maxDepth > 0 && reflColor!=blackColor )
      {
        if (!produceCaustic)
        {
          // Russian roulette to cut off recursion depth early
          //float avgRefl = (Pd.red + Pd.green + Pd.blue)/3.0;
          csRandomFloatGen randGen;
          float rand = randGen.Get();

          if(dot > rand)
          {
            // Determine color and power of scattered light
            // Due to russian roulette, no power is lost but color should
            // change according to surface reflectivity.

            csColor newColor = photon.color*hitPtColor*dot;
            csColor newPower = photon.power;

            // Generate a scattering direction in the hemisphere around the normal

            csVector3 scatterDir = DiffuseScatter(N);


            // Emit a new Photon
            const PhotonRay newPhoton = { hit.hitPoint, scatterDir, newColor, newPower, photon.source, RAY_TYPE_REFLECT, refrIndex };
            EmitPhoton(sect, newPhoton, maxDepth, depth+1, ignoreDirect, produceCaustic);
          }
        }
        else
        {
          csColor newColor = photon.color*hitPtColor*dot;
          csColor newPower = photon.power;

          // Compute the refracted ray direction

          float ratio = photon.refrIndex/refrIndex;
          float cos1 = dot;
          float cos2 = sqrt(1.0-((ratio*ratio)*(1-cos1*cos1)));
          csVector3 scatterDir = ratio*L + (ratio*cos1 - cos2)*N;

          // Emit a new Photon
          const PhotonRay newPhoton = { hit.hitPoint, scatterDir, newColor, newPower, photon.source, RAY_TYPE_REFLECT, refrIndex };
          EmitPhoton(sect, newPhoton, maxDepth, depth+1, ignoreDirect, produceCaustic);
        }
      }
    }
  }

  csVector3 PhotonmapperLighting::SpotlightDir(const csVector3 &dir, const float cosTheta)
  {
    // Local, static random number generator
    static csRandomFloatGen randGen;

    // Get two uniformly distributed random numbers between 0 and 1
    double e1 = randGen.Get();
    double e2 = randGen.Get();

    // Compute the angles of rotation around the optical axis
    double theta = acos(cosTheta)*e1;
    double phi = TWO_PI * e2;

    return RotateAroundN(dir, theta, phi);
  }

  csVector3 PhotonmapperLighting::CausticDir(const csVector3 &dir, const float outerFalloffAngle)
  {
    // Local, static random number generator
    static csRandomFloatGen randGen;

    // Get two uniformly distributed random numbers between 0 and 1
    double e1 = randGen.Get();
    double e2 = randGen.Get();

    // Compute the angles of rotation around the optical axis
    double theta = outerFalloffAngle*e1;
    double phi = TWO_PI * e2;

    return RotateAroundN(dir, theta, phi);
  }

  csVector3 PhotonmapperLighting::DirectionalLightScatter(const csVector3 &dir, const float spRadius,const csVector3 pos)
  {

    // Local, static random number generator
    static csRandomFloatGen randGen;

    // Get a uniformly distributed random numbers between 0 and 1
    double e1 = randGen.Get();
    double e2 = randGen.Get();

    // Compute the angle of rotation around the normal
    double phi = TWO_PI * e1;

    csVector3 deflectionDir = RotateAroundN(dir, HALF_PI, phi);

    return pos + deflectionDir*(spRadius*e2);


  }
  csVector3 PhotonmapperLighting::EqualScatter(const csVector3 &n)
  {
    // Local, static random number generator
    static csRandomFloatGen randGen;

    // Get two uniformly distributed random numbers between 0 and 1
    double e1 = randGen.Get();
    double e2 = randGen.Get();

    // Compute the angles of rotation around the normal
    // Up to 90 and 360 respectively.
    double theta = HALF_PI * e1;
    double phi = TWO_PI * e2;

    return RotateAroundN(n, theta, phi);
  }

  csVector3 PhotonmapperLighting::DiffuseScatter(const csVector3 &n)
  {
    // Local, static random number generator
    static csRandomFloatGen randGen;

    // Get two uniformly distributed random numbers between 0 and 1
    double e1 = randGen.Get();
    double e2 = randGen.Get();

    // Compute the angles of rotation around the normal
    // Note: altitude is weighted by cosine just like Lambert's law
    double theta = acos(sqrt(e1));
    double phi = TWO_PI * e2;

    return RotateAroundN(n, theta, phi);
  }

  csVector3 PhotonmapperLighting::StratifiedSample(const csVector3 &n, const size_t i,
                                      const size_t j, const size_t M, const size_t N)
  {
    // Local, static random number generator
    static csRandomFloatGen randGen;

    // Get two uniformly distributed random numbers between 0 and 1
    double e1 = randGen.Get();
    double e2 = randGen.Get();

    // Generate rotation angles around n.  Here we are generating
    // jittered samples in a grid across the hemisphere weighted
    // by the cos function again (lambert's law).
    double theta = acos(sqrt((j-e1)/M));
    double phi = TWO_PI * ( (i-e2)/N );

    return RotateAroundN(n, theta, phi);
  }

  csVector3 PhotonmapperLighting::RotateAroundN(const csVector3 &n,
    const double theta, const double phi)
  {
    // Find orthogonal axis (must avoid the dominant axis)
    csVector3 orthoN, rotAxis;
    switch(n.DominantAxis())
    {
      case CS_AXIS_X: orthoN.Set(0, 1, 0); break;
      case CS_AXIS_Y: orthoN.Set(0, 0, 1); break;
      case CS_AXIS_Z: orthoN.Set(1, 0, 0); break;
    }

    rotAxis.Cross(n, orthoN);
    rotAxis.Normalize();

    // Create a vector to hold the results and a
    // quaternion for rotations
    csVector3 result = n;
    csQuaternion rotater;

    // Rotate n theta radians around 'rotAxis'
    rotater.SetAxisAngle(rotAxis, theta);
    result = rotater.Rotate(result);

    // Rotate again, this time phi radians around 'n'
    rotater.SetAxisAngle(n, phi);
    result = rotater.Rotate(result);

    return result;
  }
}
