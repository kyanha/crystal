/*
  Copyright (C) 2006 by Marten Svanfeldt

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

#include "directlight.h"
#include "lighter.h"
#include "raytracer.h"
#include "scene.h"
#include "statistics.h"

namespace lighter
{
  //-------------------------------------------------------------------------

/*  static const float ElementQuadrantConstants[][2] =
  {
    {-0.25f, -0.25f},
    {-0.25f,  0.25f},
    { 0.25f, -0.25f},
    { 0.25f,  0.25f}
  };*/

  DirectLighting::PVLPointShader DirectLighting::pvlPointShader = 0;
  DirectLighting::LMElementShader DirectLighting::lmElementShader = 0;

  void DirectLighting::Initialize ()
  {
    if (globalLighter->configMgr->GetBool ("lighter2.DirectLightRandom", false))
    {
      pvlPointShader = UniformShadeRndLightNonPD;
      lmElementShader = UniformShadeRndLightNonPD;
    }
    else
    {
      pvlPointShader = UniformShadeAllLightsNonPD;
      lmElementShader = UniformShadeAllLightsNonPD;
    }    
  }

  // Shade a single point in space with direct lighting
  csColor DirectLighting::UniformShadeAllLightsNonPD (Sector* sector, 
    Object* obj, const csVector3& point, const csVector3& normal, 
    SamplerSequence<2>& lightSampler)
  {
    csColor res (0);
    const LightRefArray& allLights = sector->allNonPDLights;

    for (size_t i = 0; i < allLights.GetSize (); ++i)
    {
      float rndValues[2];
      lightSampler.GetNext (rndValues);

      res += ShadeLight (allLights[i], obj, point, normal, lightSampler);
    }

    return res;
  }

  // Shade a single point in space with direct lighting using a single light
  csColor DirectLighting::UniformShadeRndLightNonPD (Sector* sector, 
    Object* obj, const csVector3& point, const csVector3& normal, 
    SamplerSequence<2>& sampler)
  {
    SamplerSequence<3> lightSampler (sampler);

    csColor res (0);
    const LightRefArray& allLights = sector->allNonPDLights;

    // Select random light
    float rndValues[3];
    lightSampler.GetNext (rndValues);

    size_t lightIdx = (size_t) floorf (allLights.GetSize () * rndValues[2]);

    return ShadeLight (allLights[lightIdx], obj, point, normal, sampler) * 
      allLights.GetSize ();
  }

  template<typename T>
  csColor DirectLighting::UniformShadeElement (T& shade, ElementProxy element, 
    SamplerSequence<2>& lightSampler)
  {
    csColor res (0);

    // Get some data
    const csVector3& uVec = element.primitive.GetuFormVector ();
    const csVector3& vVec = element.primitive.GetvFormVector ();
    const csVector3& elementC = element.primitive.ComputeElementCenter (element.element); 
  
    const csVector2 defElementQuadrantConstants[4] = {
      csVector2(-0.25f, -0.25f),
      csVector2(-0.25f,  0.25f),
      csVector2( 0.25f, -0.25f),
      csVector2( 0.25f,  0.25f)
    };
    csVector2 clippedElementQuadrantConstants[4];
    const csVector2* ElementQuadrantConstants = defElementQuadrantConstants;

    if (element.primitive.GetElementType (element.element) == Primitive::ELEMENT_BORDER)
    {
      element.primitive.RecomputeQuadrantOffset (element.element, 
        defElementQuadrantConstants, clippedElementQuadrantConstants);
      ElementQuadrantConstants = clippedElementQuadrantConstants;
    }

    for (size_t qi = 0; qi < 4; ++qi)
    {
      const csVector3 offsetVector = uVec * ElementQuadrantConstants[qi].x +
        vVec * ElementQuadrantConstants[qi].y;

      const csVector3 pos = elementC + offsetVector;
      const csVector3 normal = element.primitive.ComputeNormal (pos);

      res += shade.ShadeLight (element.primitive.GetObject(), 
        pos, normal, lightSampler, &element.primitive);
    }
    
    return res * 0.25f;
  }

  csColor DirectLighting::ShadeAllLightsNonPD::ShadeLight (Object* obj, 
    const csVector3& point, const csVector3& normal, 
    SamplerSequence<2>& lightSampler, 
    const Primitive* shadowIgnorePrimitive, 
    bool fullIgnore)
  {
    csColor res (0);
    for (size_t i = 0; i < allLights.GetSize (); ++i)
    {
      res += DirectLighting::ShadeLight (allLights[i], obj, point,
        normal, lightSampler, shadowIgnorePrimitive, fullIgnore);
    }
    return res;
  }

  // Shade a primitive element with direct lighting
  csColor DirectLighting::UniformShadeAllLightsNonPD (Sector* sector, 
    ElementProxy element, SamplerSequence<2>& lightSampler)
  {
    const LightRefArray& allLights = sector->allNonPDLights;
    ShadeAllLightsNonPD shade (allLights);
    return UniformShadeElement (shade, element, lightSampler);
  }

  csColor DirectLighting::ShadeRndLightNonPD::ShadeLight (Object* obj, 
    const csVector3& point, const csVector3& normal, 
    SamplerSequence<2>& sampler, const Primitive* shadowIgnorePrimitive, 
    bool fullIgnore)
  {
    float rndValues[3];
    
    lightSampler.GetNext (rndValues);
    size_t lightIdx = (size_t) floorf (allLights.GetSize () * rndValues[2]);

    return DirectLighting::ShadeLight (allLights[lightIdx], obj, point,
      normal, sampler, shadowIgnorePrimitive, fullIgnore);
  }

  // Shade a primitive element with direct lighting using a single light
  csColor DirectLighting::UniformShadeRndLightNonPD (Sector* sector, 
    ElementProxy element, SamplerSequence<2>& sampler)
  {
    SamplerSequence<3> lightSampler (sampler);
    const LightRefArray& allLights = sector->allNonPDLights;

    ShadeRndLightNonPD shade (allLights, lightSampler);
    return UniformShadeElement (shade, element, sampler) 
      * allLights.GetSize ();
  }

  csColor DirectLighting::UniformShadeOneLight (Sector* sector, Object* obj, 
    const csVector3& point, const csVector3& normal, Light* light, 
    SamplerSequence<2>& sampler)
  {
    return ShadeLight (light, obj, point, normal, sampler);
  }

  csColor DirectLighting::ShadeOneLight::ShadeLight (Object* obj, 
    const csVector3& point, const csVector3& normal, 
    SamplerSequence<2>& sampler, const Primitive* shadowIgnorePrimitive,
    bool fullIgnore)
  {
    return DirectLighting::ShadeLight (light, obj, point,
      normal, sampler, shadowIgnorePrimitive, fullIgnore);
  }

  csColor DirectLighting::UniformShadeOneLight (Sector* sector, ElementProxy element,
    Light* light, SamplerSequence<2>& sampler)
  {
    ShadeOneLight shade (light);
    return UniformShadeElement (shade, element, sampler);
  }

  class DirectLightingBorderIgnoreCb : public HitIgnoreCallback
  {
  public:
    explicit DirectLightingBorderIgnoreCb (const Primitive* ignorePrim)
      : ignorePrim (ignorePrim)
    {}

    virtual bool IgnoreHit (const Primitive* prim)
    {
      return (prim == ignorePrim)
       || (prim->GetPlane() == ignorePrim->GetPlane());
    }

  private:
    const Primitive* ignorePrim;
  };

  csColor DirectLighting::ShadeLight (Light* light, Object* obj, 
    const csVector3& point, const csVector3& normal, 
    SamplerSequence<2>& lightSampler, const Primitive* shadowIgnorePrimitive,
    bool fullIgnore)
  {
    // Some variables..
    VisibilityTester visTester (light, obj);
    float lightPdf, cosineTerm = 0;
    csVector3 lightVec;

    float lightSamples[2];
    lightSampler.GetNext (lightSamples);

    csColor lightColor = light->SampleLight (point, normal, lightSamples[0],
      lightSamples[1], lightVec, lightPdf, visTester);

    if (lightPdf > 0.0f && !lightColor.IsBlack () &&
      (cosineTerm = normal * lightVec) > 0)
    {
      VisibilityTester::OcclusionState occlusion;
      if (fullIgnore)
      {
        DirectLightingBorderIgnoreCb icb (shadowIgnorePrimitive);
        occlusion = visTester.Occlusion (&icb);
      }
      else
      {
        occlusion = visTester.Occlusion (shadowIgnorePrimitive);
      }
      if (occlusion == VisibilityTester::occlOccluded)
        return csColor (0, 0, 0);
      else if (occlusion == VisibilityTester::occlPartial)
        lightColor *= visTester.GetFilterColor ();

      if (light->IsDeltaLight ())
        return lightColor * fabsf (cosineTerm) / lightPdf;
      else
        // Properly handle area sources! See pbrt page 732
        return lightColor * fabsf (cosineTerm) / lightPdf;
    }

    return csColor (0,0,0);
  }

  //--------------------------------------------------------------------------

  void DirectLighting::ShadeDirectLighting (Sector* sector, 
                                            Statistics::Progress& progress)
  {
    progress.SetProgress (0);
    size_t totalElements = 0;

    // Sum up total amount of elements for progress display purposes
    ObjectHash::GlobalIterator giter = sector->allObjects.GetIterator ();
    while (giter.HasNext ())
    {
      csRef<Object> obj = giter.Next ();

      if (!obj->GetFlags ().Check (OBJECT_FLAG_NOLIGHT))
      {
        if (obj->lightPerVertex)
          totalElements += obj->GetVertexData().positions.GetSize();
        else
        {
          csArray<PrimitiveArray>& submeshArray = obj->GetPrimitives ();
          for (size_t submesh = 0; submesh < submeshArray.GetSize (); ++submesh)
          {
            PrimitiveArray& primArray = submeshArray[submesh];

            for (size_t pidx = 0; pidx < primArray.GetSize (); ++pidx)
            {
              Primitive& prim = primArray[pidx];
              totalElements += prim.GetElementCount();
            }
          }
        }
      }
    }

    // Setup some common stuff
    SamplerSequence<2> masterSampler;
    ProgressState progressState (progress, totalElements);

    giter.Reset();
    while (giter.HasNext ())
    {
      csRef<Object> obj = giter.Next ();

      if (!obj->GetFlags ().Check (OBJECT_FLAG_NOLIGHT))
      {
        if (obj->lightPerVertex)
          ShadePerVertex (sector, obj, masterSampler, progressState);
        else
          ShadeLightmap (sector, obj, masterSampler, progressState);
      }
    }

    progress.SetProgress (1);
    return;
  }
  
  void DirectLighting::ShadeLightmap (Sector* sector, Object* obj, 
    SamplerSequence<2>& masterSampler, ProgressState& progress)
  {
    csArray<PrimitiveArray>& submeshArray = obj->GetPrimitives ();
    const LightRefArray& allPDLights = sector->allPDLights;

    LightRefArray PDLights;
    csArray<Lightmap*> pdLightLMs;

    for (size_t pdli = 0; pdli < allPDLights.GetSize (); ++pdli)
    {
      Light* pdl = allPDLights[pdli];
      if (pdl->GetBoundingSphere().TestIntersect (obj->GetBoundingSphere()))
      {
        PDLights.Push (pdl);
      }
    }

    for (size_t submesh = 0; submesh < submeshArray.GetSize (); ++submesh)
    {
      PrimitiveArray& primArray = submeshArray[submesh];

      float area2pixel = 1.0f;

      for (size_t pidx = 0; pidx < primArray.GetSize (); ++pidx)
      {
        Primitive& prim = primArray[pidx];

        area2pixel = 
          1.0f / (prim.GetuFormVector () % prim.GetvFormVector ()).Norm();

        //const ElementAreas& areas = prim.GetElementAreas ();
        size_t numElements = prim.GetElementCount ();        
        Lightmap* normalLM = sector->scene->GetLightmap (prim.GetGlobalLightmapID (), (Light*)0);

        ScopedSwapLock<Lightmap> lightLock (*normalLM);

        pdLightLMs.Empty ();
        for (size_t pdli = 0; pdli < PDLights.GetSize (); ++pdli)
        {
          Lightmap* lm = sector->scene->GetLightmap (prim.GetGlobalLightmapID (),
            PDLights[pdli]);

          lm->Lock ();
          pdLightLMs.Push (lm);
        }

        csVector2 minUV = prim.GetMinUV ();

        // Iterate all elements
        for (size_t eidx = 0; eidx < numElements; ++eidx)
        {
          //const float elArea = areas.GetElementArea (eidx);
          Primitive::ElementType elemType = prim.GetElementType (eidx);

          ElementProxy ep = prim.GetElement (eidx);
          size_t u, v;
          prim.GetElementUV (eidx, u, v);
          u += size_t (floorf (minUV.x));
          v += size_t (floorf (minUV.y));

          if (elemType == Primitive::ELEMENT_EMPTY)
          {                        
            progress.Advance ();
            continue;
          }

          //float pixelArea = (elArea*area2pixel);
          const float pixelAreaPart = 
            elemType == Primitive::ELEMENT_BORDER ? prim.ComputeElementFraction (eidx) : 
                                                    1.0f;

          // Shade non-PD lights
          csColor c;        
          c = lmElementShader (sector, ep, masterSampler);
          

          normalLM->SetAddPixel (u, v, c * pixelAreaPart);

          // Shade PD lights
          for (size_t pdli = 0; pdli < PDLights.GetSize (); ++pdli)
          {
            csColor c;
    
            Lightmap* lm = pdLightLMs[pdli];
            Light* pdl = PDLights[pdli];

            c = UniformShadeOneLight (sector, ep, pdl, masterSampler);

            lm->SetAddPixel (u, v, c * pixelAreaPart);
          }
          progress.Advance ();
        }
        for (size_t pdli = 0; pdli < PDLights.GetSize (); ++pdli)
        {
          pdLightLMs[pdli]->Unlock();
        }
      }
    }
  }

  void DirectLighting::ShadePerVertex (Sector* sector, Object* obj, 
    SamplerSequence<2>& masterSampler, ProgressState& progress)
  {
    const LightRefArray& allPDLights = sector->allPDLights;
    LightRefArray PDLights;

    Object::LitColorArray* litColors = obj->GetLitColors ();
    const ObjectVertexData& vdata = obj->GetVertexData ();

    for (size_t pdli = 0; pdli < allPDLights.GetSize (); ++pdli)
    {
      Light* pdl = allPDLights[pdli];
      if (pdl->GetBoundingSphere().TestIntersect (obj->GetBoundingSphere()))
      {
        PDLights.Push (pdl);
      }
    }

    for (size_t i = 0; i < vdata.positions.GetSize (); ++i)
    {
      const csVector3& pos = vdata.positions[i];
      const csVector3& normal = vdata.normals[i];

      csColor& c = litColors->Get (i);

      c = pvlPointShader (sector, obj, pos, normal, masterSampler);

      // Shade PD lights
      for (size_t pdli = 0; pdli < PDLights.GetSize (); ++pdli)
      {
        Light* pdl = PDLights[pdli];
        Object::LitColorArray* pdlColors = obj->GetLitColorsPD (pdl);
        pdlColors->Get (i) += UniformShadeOneLight (sector, obj, pos, normal, pdl,
          masterSampler);
      }
      progress.Advance ();
    }

  }

}
