/*
  Copyright (C) 2010 Alexandru - Teodor Voicu
      Faculty of Automatic Control and Computer Science of the "Politehnica"
      University of Bucharest
      http://csite.cs.pub.ro/index.php/en/

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <cssysdef.h>

#include "furmesh.h"
#include "hairphysicscontrol.h"
#include "furmeshproperties.h"

#define MAX_GUIDE_FURS 10000
#define MAX_FUR_STRAND_DENSITY 100

CS_PLUGIN_NAMESPACE_BEGIN(FurMesh)
{
  /********************
  *  FurMesh
  ********************/

  CS_LEAKGUARD_IMPLEMENT(FurMesh);

  FurMesh::FurMesh (iEngine* engine, iObjectRegistry* object_reg, 
    iMeshObjectFactory* object_factory) : scfImplementationType (this, engine), 
    materialWrapper(0), object_reg(object_reg), object_factory(object_factory), 
    engine(engine), isEnabled(true), animesh(0), physicsControl(0), 
    hairMeshProperties(0), furStrandShift(0), positionShift(0), rng(0), 
    controlPointsLOD(1), offsetIndex(0), offsetVertex(0), endVertex(0), 
    guideLOD(0), previousGuideLOD(0), strandLOD(1), furStrandsLODSize(0), 
    physicsControlEnabled(false), isReset(false), startFrame(0), 
    meshFactory(0), meshFactorySubMesh(0), indexstart(0), indexend(0)
  {
    svStrings = csQueryRegistryTagInterface<iShaderVarStringSet> (
      object_reg, "crystalspace.shader.variablenameset");

    if (!svStrings) 
      printf ("No SV names string set!");

    if (!engine) printf ("Failed to locate 3D engine!");

    rng = new csRandomGen(csGetTicks());
    
    if (engine)
      SetRenderPriority (engine->GetRenderPriority ("transp"));
  }

  FurMesh::~FurMesh ()
  {
    delete rng;
    
    if (furStrandShift)
      delete furStrandShift;

    if (positionShift)
      delete positionShift;

    // Delete fur geometry data
    for (size_t i = 0 ; i < guideFurs.GetSize() ; i ++)
      guideFurs.Get(i).Clear();

    guideFurs.DeleteAll();

    for (size_t i = 0 ; i < guideFursLOD.GetSize() ; i ++)
      guideFursLOD.Get(i).Clear();

    guideFursLOD.DeleteAll();

    for (size_t i = 0 ; i < furStrands.GetSize() ; i ++)
      furStrands.Get(i).Clear();

    furStrands.DeleteAll();

    guideFursTriangles.DeleteAll();
  }

  iMeshObjectFactory* FurMesh::GetFactory () const
  {
    return object_factory;
  }

  bool FurMesh::HitBeamObject (const csVector3& start, const csVector3& end,
    csVector3& isect, float* pr, int* polygon_idx,
    iMaterialWrapper** material, iMaterialArray* materials)
  {
    return csIntersect3::BoxSegment (GetObjectBoundingBox(), 
      csSegment3 (start, end), isect, pr) != 0;
  }

  void FurMesh::NextFrame (csTicks current_time, const csVector3& pos,
    uint currentFrame)
  {
    if (!isEnabled)
      return;

    Update();
    UpdateObjectBoundingBox();

    if (isReset)
    {
      startFrame = currentFrame + 1;
      isReset = false;
    }

    if (currentFrame == startFrame)
    {
      startFrame = 0;
      RegenerateGeometry();
    }
  }

  bool FurMesh::SetMaterialWrapper (iMaterialWrapper* mat)
  {
    materialWrapper = mat;
    return true;
  }

  iMaterialWrapper* FurMesh::GetMaterialWrapper () const
  {
    return materialWrapper;
  }

  void FurMesh::UpdateObjectBoundingBox ()
  {
    if (!GetVertexCount())
      return;

    boundingbox.StartBoundingBox();
    
    csVector3* vertex_buffer = 
      (csVector3*)GetVertices()->Lock (CS_BUF_LOCK_READ);

    for (size_t i = offsetVertex ; i < endVertex ; i ++)
      boundingbox.AddBoundingVertex(vertex_buffer[i]);

    GetVertices()->Release();
  }

  CS::Graphics::RenderMesh** FurMesh::GetRenderMeshes (int& num, 
    iRenderView* rview, iMovable* movable, uint32 frustum_mask)
  {
    for (size_t i = 0 ; i < renderMeshes.GetSize() ; i ++)
      delete renderMeshes.Get(i);

    renderMeshes.Empty();

    // Boiler-plate stuff...
    iCamera* camera = rview->GetCamera ();

    int clip_portal, clip_plane, clip_z_plane;
    CS::RenderViewClipper::CalculateClipSettings (rview->GetRenderContext (),
      frustum_mask, clip_portal, clip_plane, clip_z_plane);

    const csReversibleTransform o2wt = movable->GetFullTransform ();

    if (indexstart == indexend)
    {
      num = 0;
      return 0;
    }

    if (!materialWrapper)
    {
      csPrintfErr ("INTERNAL ERROR: mesh used without material!\n");
      num = 0;
      return 0;
    }

    if (materialWrapper->IsVisitRequired ()) 
      materialWrapper->Visit ();

    csRenderMesh* meshPtr = new csRenderMesh;

    // Setup the render mesh
    meshPtr->clip_portal = clip_portal;
    meshPtr->clip_plane = clip_plane;
    meshPtr->clip_z_plane = clip_z_plane;
    meshPtr->do_mirror = camera->IsMirrored ();
    meshPtr->meshtype = CS_MESHTYPE_TRIANGLES;
    meshPtr->indexstart = indexstart;
    meshPtr->indexend = indexend;
    meshPtr->material = materialWrapper;

    meshPtr->buffers = bufferholder;

    meshPtr->mixmode = GetMixmode();
    meshPtr->renderPrio = GetRenderPriority();
    meshPtr->z_buf_mode = GetZBufMode();

    meshPtr->object2world = o2wt;
    meshPtr->bbox = GetObjectBoundingBox();
    meshPtr->geometryInstance = object_factory;
    meshPtr->variablecontext = svContext;

    renderMeshes.Push (meshPtr);    

    num = (int)renderMeshes.GetSize ();
    return renderMeshes.GetArray ();
  }

  void FurMesh::SetIndexRange (uint indexstart, uint indexend)
  {
    this->indexstart = indexstart;
    this->indexend = indexend;
  }

  size_t FurMesh::GetControlPointsCount(float controlPointsLOD) const
  {
    size_t controlPointsCount = 0;

    for ( size_t i = 0 ; i < furStrands.GetSize() ; i ++ )
      controlPointsCount += 
      furStrands.Get(i).GetControlPointsCount(controlPointsLOD);

    return controlPointsCount;
  }

  void FurMesh::GenerateGeometry (iView* view, iSector *room)
  {
    if (!animesh)
    {
      csPrintfErr("Please specify base animesh!\n");
      return;
    }

    if (!meshFactory)
    {
      csPrintfErr("Please specify base animesh factory!\n");
      return;
    }

    if (!meshFactorySubMesh)
    {
      csPrintfErr("Please specify base animesh factory sub mesh!\n");
      return;
    }

    if (!GetDensityMap())
    {
      csPrintfErr( "Please specify density map texture!\n" );    
      return;
    }

    densitymap.handle = GetDensityMap()->GetTextureHandle();

    if (!densitymap.handle)
    {
      csPrintfErr( "Please specify density map texture!\n" );    
      return;
    }

    // Density map
    if ( !densitymap.Read() )
      csPrintfErr( "Error reading densitymap texture!\n" );    

    if (!GetHeightMap())
    {
      csPrintfErr( "Please specify height map texture!\n" );    
      return;
    }

    heightmap.handle = GetHeightMap()->GetTextureHandle();

    if (!heightmap.handle)
    {
      csPrintfErr( "Please specify height map texture!\n" );
      return;
    }
    // Height map
    if ( !heightmap.Read() )
      csPrintfErr( "Error reading heightmap texture!\n" );  

    GenerateGuideFurs();
    GenerateGuideFursLOD();
    GenerateFurStrands();

//     SaveUVImage();

    this->view = view;
    size_t numberOfStrains = furStrands.GetSize();
    furStrandsLODSize = furStrands.GetSize();
    
    if( !numberOfStrains ) 
      return;

    size_t controlPointsCount = GetControlPointsCount(1.0f);
    
    endVertex = 2 * controlPointsCount;

    // Generate fur strand deviation
    furStrandShift = new csVector3 [ numberOfStrains ];

    for ( size_t i = 0 ; i < numberOfStrains ; i ++)
      furStrandShift[i] = csVector3(rng->Get() * 2 - 1, rng->Get() * 2 - 1, 
        rng->Get() * 2 - 1) * GetFurStrandDeviation();

    // Generate position deviation
    positionShift = new csVector3 [ controlPointsCount ];

    for ( size_t i = 0 ; i < controlPointsCount ; i ++)
      positionShift[i] = csVector3(rng->Get() * 2 - 1, rng->Get() * 2 - 1, 
        rng->Get() * 2 - 1) * GetControlPointsDeviation();

    controlPointsCount = GetControlPointsCount(1.0f) + 
      GetControlPointsCount(0.5f) + GetControlPointsCount(0.0f);

    // First create the factory
    SetVertexCount ( 2 * controlPointsCount );
    SetTriangleCount( 2 * ( controlPointsCount - 3 * numberOfStrains ) );

    csVector3* vbuf = 
      (csVector3*)GetVertices()->Lock (CS_BUF_LOCK_NORMAL);
    csVector2* uv = 
      (csVector2*)GetTexCoords()->Lock (CS_BUF_LOCK_NORMAL);
    csTriangle* ibuf = 
      (csTriangle*)GetIndices()->Lock (CS_BUF_LOCK_NORMAL);
    csVector3* bin = 
      (csVector3*)GetBinormals()->Lock(CS_BUF_LOCK_NORMAL);

    size_t offsetIndex = 0;
    size_t offsetVertex = 0;

    // Generate all 3 LODs
    for (float controlPointsLOD = 1.0f ; controlPointsLOD >= 0.0f ; 
      controlPointsLOD -= 0.5f)
    {
      for ( size_t x = 0, controlPointSum = 0 ; x < numberOfStrains ; 
        controlPointSum += furStrands.Get(x).GetControlPointsCount(controlPointsLOD), x++ )      
      {
        csFurStrand furStrand = furStrands.Get(x);
        csVector2 strandUV = furStrand.uv;

        for ( size_t y = 0 ; y < furStrand.GetControlPointsCount(controlPointsLOD) ; y ++ )
        {
          vbuf[ offsetVertex + 2 * controlPointSum + 2 * y] = furStrand.controlPoints[y];
          vbuf[ offsetVertex + 2 * controlPointSum + 2 * y + 1] = 
            furStrand.controlPoints[y] + csVector3(-0.01f,0,0);
          uv[ offsetVertex + 2 * controlPointSum + 2 * y] = strandUV;
          uv[ offsetVertex + 2 * controlPointSum + 2 * y + 1] = strandUV;
        }

        for ( size_t y = 0 ; y < 2 * (furStrand.GetControlPointsCount(controlPointsLOD) - 1) ; y ++ )
        {
          if (y % 2 == 0)
            ibuf[ offsetIndex + 2 * (controlPointSum - x) + y ] = 
            csTriangle( offsetVertex + 2 * controlPointSum + y ,  
            offsetVertex + 2 * controlPointSum + y + 1 , 
            offsetVertex + 2 * controlPointSum + y + 2 );
          else
            ibuf[ offsetIndex + 2 * (controlPointSum - x) + y ] = 
            csTriangle( offsetVertex + 2 * controlPointSum + y , 
            offsetVertex + 2 * controlPointSum + y + 2 ,
            offsetVertex + 2 * controlPointSum + y + 1 );
        }

        // Generate color deviation and UV 
        float len = csVector3::Norm(
          furStrand.controlPoints[ furStrand.GetControlPointsCount(controlPointsLOD) - 1 ] - 
          furStrand.controlPoints[ 0 ]);

        for ( size_t y = 0 ; y < furStrand.GetControlPointsCount(controlPointsLOD) ; y ++ )
        {
          float sum = csVector3::Norm( furStrand.controlPoints[ y ] - 
            furStrand.controlPoints[ 0 ]);

          bin[ offsetVertex + 2 * controlPointSum + 2 * y] = 
            csVector3( rng->Get(), sum/len, rng->Get() * GetThicknessVariation());
          bin[ offsetVertex + 2 * controlPointSum + 2 * y + 1] = 
            bin[ offsetVertex + 2 * controlPointSum + 2 * y];
        }
      }

      controlPointsCount = GetControlPointsCount(controlPointsLOD);

      offsetIndex += 2 * (controlPointsCount - numberOfStrains);
      offsetVertex += 2 * controlPointsCount;
    }

    GetBinormals()->Release();
    GetVertices()->Release();
    GetTexCoords()->Release();
    GetIndices()->Release();

    // Default material
    csRef<iMaterialWrapper> materialWrapper = 
      CS::Material::MaterialBuilder::CreateColorMaterial
      (object_reg,"hairDummyMaterial",csColor(1,0,0));

    if (hairMeshProperties && hairMeshProperties->GetMaterial())
      materialWrapper->SetMaterial(hairMeshProperties->GetMaterial());

    SetMaterialWrapper(materialWrapper);

    // Get the buffer holder
    bufferholder.AttachNew (new csRenderBufferHolder);
    bufferholder->SetRenderBuffer (CS_BUFFER_INDEX, GetIndices());
    bufferholder->SetRenderBuffer (CS_BUFFER_POSITION, GetVertices());
    bufferholder->SetRenderBuffer (CS_BUFFER_TEXCOORD0, GetTexCoords());
    bufferholder->SetRenderBuffer (CS_BUFFER_BINORMAL, GetBinormals());
    if (!GetSmallFur())
    {
      bufferholder->SetRenderBuffer (CS_BUFFER_NORMAL, GetNormals());
      bufferholder->SetRenderBuffer (CS_BUFFER_TANGENT, GetTangents());
    }
    else
    {
      bufferholder->SetRenderBuffer (CS_BUFFER_NORMAL, GetTangents());
      bufferholder->SetRenderBuffer (CS_BUFFER_TANGENT, GetNormals());
    }

    svContext.AttachNew (new csShaderVariableContext);
    csShaderVariable* sv;

    // Get the SV names
    sv = svContext->GetVariableAdd (svStrings->Request ("position"));
    sv->SetValue (GetVertices());

    sv = svContext->GetVariableAdd (svStrings->Request ("texture coordinate 0"));
    sv->SetValue (GetTexCoords());

    sv = svContext->GetVariableAdd (svStrings->Request ("normal"));
    if (!GetSmallFur())
      sv->SetValue (GetNormals());
    else
      sv->SetValue (GetTangents());

    sv = svContext->GetVariableAdd (svStrings->Request ("tangent"));
    if (!GetSmallFur())
      sv->SetValue (GetTangents());
    else
      sv->SetValue (GetNormals());

    sv = svContext->GetVariableAdd (svStrings->Request ("binormal"));
    sv->SetValue (GetBinormals());

    SetIndexRange(0, 3 * 2 * (GetControlPointsCount(1.0f) - numberOfStrains) );
  }

  void FurMesh::GenerateGuideFurs()
  {
    size_t indexstart = meshFactorySubMesh->GetIndices(0)->GetRangeStart();
    size_t indexend = meshFactorySubMesh->GetIndices(0)->GetRangeEnd();

    size_t indexCount = 
      meshFactorySubMesh->GetIndices(0)->GetSize() / sizeof(csTriangle);

    // Create a walker for the position buffer of the animesh
    csRenderBufferHolder holder;
    animesh->GetRenderBufferAccessor ()->PreGetBuffer (&holder, CS_BUFFER_POSITION);
    csRenderBufferLock<csVector3> positions (holder.GetRenderBuffer (CS_BUFFER_POSITION));
    animesh->GetRenderBufferAccessor ()->PreGetBuffer (&holder, CS_BUFFER_NORMAL);
    csRenderBufferLock<csVector3> normals (holder.GetRenderBuffer (CS_BUFFER_NORMAL));
    animesh->GetRenderBufferAccessor ()->PreGetBuffer (&holder, CS_BUFFER_TANGENT);
    csRenderBufferLock<csVector3> tangents (holder.GetRenderBuffer (CS_BUFFER_TANGENT));

    // Compute the new position of the anchor
    csRef<iMeshObject> mesh = scfQueryInterface<iMeshObject> (animesh);

    csTriangle* index_buffer = 
      (csTriangle*)meshFactorySubMesh->GetIndices(0)->Lock(CS_BUF_LOCK_READ);
    csVector2* texcoord_buffer = 
      (csVector2*)meshFactory->GetTexCoords()->Lock(CS_BUF_LOCK_READ);

    // Choose unique indices
    for ( size_t i = 0 ; i < indexCount ; i ++)
    {
      csTriangle tri = index_buffer[i];
      csTriangle triangleNew = csTriangle(tri.a - indexstart, 
        tri.b - indexstart, tri.c - indexstart);
      guideFursTriangles.Push(triangleNew);
    }

    float averageHeight = 0.0f;
  
    // From average control points count to control points distance
    for (size_t i = indexstart; i <= indexend; i ++)
    {
      csVector2 uv = texcoord_buffer[i];

      float height = heightmap.Get((int)(uv.x * heightmap.width),  
        (int)(uv.y * heightmap.height), 0) / 255.0f;

      averageHeight += height * GetHeightFactor();
    }

    averageHeight = averageHeight / (indexend - indexstart + 1);

    if (GetAverageControlPointsCount() == 0 || averageHeight < EPSILON)
      SetControlPointsDistance(FLT_MAX);
    else
    {
      if (GetAverageControlPointsCount() == 1)
        SetAverageControlPointsCount(2);

      SetControlPointsDistance( averageHeight / (GetAverageControlPointsCount() - 1) );
    }

    // Generate the guide furs
    for (size_t i = indexstart; i <= indexend; i ++)
    {
      csVector3 position = mesh->GetMeshWrapper ()->GetMovable ()->
        GetTransform ().This2Other (positions[i]);
      csVector3 normal = mesh->GetMeshWrapper ()->GetMovable ()->
        GetTransform ().This2Other (normals[i]);
      normal.Normalize();
      csVector3 tangent = mesh->GetMeshWrapper ()->GetMovable ()->
        GetTransform ().This2Other (tangents[i]);
      tangent.Normalize();

      csVector3 pos = position + GetDisplacement() * normal;

      csGuideFur guideFur;
      guideFur.uv = texcoord_buffer[i];

      float height = heightmap.Get((int)(guideFur.uv.x * heightmap.width),  
        (int)(guideFur.uv.y * heightmap.height), 0) / 255.0f;

      size_t controlPointsCount = (int)( (height * GetHeightFactor()) 
        / GetControlPointsDistance());
      
      if (controlPointsCount == 0 && (height * GetHeightFactor()) > EPSILON)
        controlPointsCount ++;

      if ((height * GetHeightFactor()) > EPSILON)
        controlPointsCount ++;

      float realDistance = (height * GetHeightFactor()) / (controlPointsCount - 1);

      if (GetGrowTangent())
        guideFur.Generate(controlPointsCount, realDistance, pos, tangent);
      else
        guideFur.Generate(controlPointsCount, realDistance, pos, normal);

      guideFurs.Push(guideFur);
    }

    meshFactorySubMesh->GetIndices(0)->Release();
    meshFactory->GetTexCoords()->Release();
  }

  void FurMesh::GenerateGuideFursLOD()
  {
    float density, area;
    csGuideFur A, B, C;

    // Generate guide hairs for LOD
    for (size_t iter = 0 ; iter < guideFursTriangles.GetSize() && iter < MAX_GUIDE_FURS; iter ++)
    {
      csTriangle triangle = guideFursTriangles.Get(iter);
      TriangleAreaDensity(triangle, area, density, A, B, C);

      // If a new guide fur is needed
      if ( (density * area * GetDensityFactorGuideFurs()) < 1)
        continue;

      // Make new guide fur
      csGuideFurLOD guideFurLOD;
      guideFurLOD.SetGuideHairsRefs(triangle, rng);

      guideFurLOD.isActive = false;

      guideFurLOD.SetUV(guideFurs, guideFursLOD);

      // Generate control points
      size_t controlPointsCount = csMin( (csMin( 
        A.GetControlPointsCount(controlPointsLOD),
        B.GetControlPointsCount(controlPointsLOD)) ), 
        C.GetControlPointsCount(controlPointsLOD));

      guideFurLOD.Generate(controlPointsCount, guideFurs, guideFursLOD);
      
      // Add new triangles
      guideFursLOD.Push(guideFurLOD);
      size_t indexD = guideFursLOD.GetSize() - 1 + guideFurs.GetSize();
      csTriangle ADC = csTriangle(triangle.a, indexD, triangle.c);
      csTriangle ADB = csTriangle(triangle.a, indexD, triangle.b);
      csTriangle BDC = csTriangle(triangle.b, indexD, triangle.c);

      guideFursTriangles.Push(ADC);
      guideFursTriangles.Push(ADB);
      guideFursTriangles.Push(BDC);
    }    
  }

  void FurMesh::GenerateFurStrands ()
  {
    float area, density;
    csGuideFur A, B, C;

    // Generated based on density
    for (int den = 0 , change = 1 ; change && den < MAX_FUR_STRAND_DENSITY ; den ++)
    {
      change = 0;
      // For every triangle
      for (size_t iter = 0 ; iter < guideFursTriangles.GetSize(); iter ++)
      {
        csTriangle triangle = guideFursTriangles.Get(iter);
        TriangleAreaDensity(triangle, area, density, A, B, C);

        // How many new guide fur are needed
        if ( den < (density * area * GetDensityFactorFurStrands()))
        {
          change = 1;
          csFurStrand furStrand;
          furStrand.SetGuideHairsRefs(triangle, rng);

          size_t controlPointsCount = csMin( (csMin( 
            A.GetControlPointsCount(controlPointsLOD),
            B.GetControlPointsCount(controlPointsLOD)) ), 
            C.GetControlPointsCount(controlPointsLOD));

          furStrand.Generate(controlPointsCount, guideFurs, guideFursLOD);
          furStrand.SetUV(guideFurs, guideFursLOD);

          furStrands.Push(furStrand);		
        }
      }
    }
  }

  void FurMesh::RegenerateGeometry()
  {
    // Density map
    if ( !densitymap.Read() )
      csPrintfErr( "Error reading densitymap texture!\n" );    

    // Height map
    if ( !heightmap.Read() )
      csPrintfErr( "Error reading heightmap texture!\n" );    

    GenerateGuideFurs();

    // Update guide ropes LOD
    for (size_t i = 0 ; i < guideFursLOD.GetSize(); i ++)
      guideFursLOD.Get(i).Update(guideFurs, guideFursLOD, controlPointsLOD);

    // Update fur strands
    for (size_t i = 0 ; i < furStrandsLODSize; i ++)
      furStrands.Get(i).Update(guideFurs, guideFursLOD, controlPointsLOD);

    StartAnimationControl();

    float oldControlPointsLOD = controlPointsLOD;
    controlPointsLOD = 1.0f;
    SetControlPointsLOD(oldControlPointsLOD);
  }

  void FurMesh::TriangleAreaDensity(const csTriangle& triangle, float &area, 
    float &density, csGuideFur& A, csGuideFur& B, csGuideFur& C)
  {
    size_t indexA = triangle.a;
    size_t indexB = triangle.b;
    size_t indexC = triangle.c;

    if ( indexA < guideFurs.GetSize())
      A = guideFurs.Get(indexA);
    else
      A = guideFursLOD.Get(indexA - guideFurs.GetSize());

    if ( indexB < guideFurs.GetSize())
      B = guideFurs.Get(indexB);
    else
      B = guideFursLOD.Get(indexB - guideFurs.GetSize());

    if ( indexC < guideFurs.GetSize())
      C = guideFurs.Get(indexC);
    else
      C = guideFursLOD.Get(indexC - guideFurs.GetSize());

    // Triangle area
    if ( ( A.GetControlPointsCount(controlPointsLOD) == 0 ) || 
      ( B.GetControlPointsCount(controlPointsLOD) == 0 ) ||
      ( C.GetControlPointsCount(controlPointsLOD) == 0 ) )
    {
      area = density = 0;
      return;
    }

    float bA = csVector3::Norm(B.controlPoints[0] - C.controlPoints[0]);
    float bB = csVector3::Norm(A.controlPoints[0] - C.controlPoints[0]);
    float bC = csVector3::Norm(B.controlPoints[0] - A.controlPoints[0]);    

    float s = (bA + bB + bC) / 2.0f;
    area = sqrt(s * (s - bA) * (s - bB) * (s - bC));

    float area2d;

    csVector2 a = csVector2(A.uv.x * densitymap.width, A.uv.y * densitymap.height);
    csVector2 b = csVector2(B.uv.x * densitymap.width, B.uv.y * densitymap.height);
    csVector2 c = csVector2(C.uv.x * densitymap.width, C.uv.y * densitymap.height);

    float baseA = csVector2::Norm(b - c);
    float baseB = csVector2::Norm(a - c);
    float baseC = csVector2::Norm(b - a);

    if (baseA < EPSILON)
      baseA = EPSILON;

    if (baseB < EPSILON)
      baseB = EPSILON;

    if (baseC < EPSILON)
      baseC = EPSILON;

    s = (baseA + baseB + baseC) / 2.0f;
    area2d = sqrt(s * (s - baseA) * (s - baseB) * (s - baseC));

    if (area2d < EPSILON)
    {
      area = density = 0;
      return;
    }

    float hA = (2 * area2d) / baseA;
    float hB = (2 * area2d) / baseB;
    float hC = (2 * area2d) / baseC;

    density = 0;
    int count = 0;

    // Triangle density
    for( float bA = 0.0; bA <= 1.0; bA += 1 / hA )
      for (float bB = 0.0; bB <= 1.0 - bA; bB += 1 / baseA)
      {
        count++;
        float bC = 1 - bA - bB;
        csVector2 newPoint = a * bA + b * bB + c * bC;
        density += densitymap.Get((int)newPoint.x, (int)newPoint.y, 0);
      }

    for( float bB = 0.0; bB <= 1.0; bB += 1 / hB )
      for (float bA = 0.0; bA <= 1.0 - bB; bA += 1 / baseB)
      {
        count++;
        float bC = 1 - bA - bB;
        csVector2 newPoint = a * bA + b * bB + c * bC;
        density += densitymap.Get((int)newPoint.x, (int)newPoint.y, 0);
      }

    for( float bC = 0.0; bC <= 1.0; bC += 1 / hC )
      for (float bA = 0.0; bA <= 1.0 - bC; bA += 1 / baseC)
      {
        count++;
        float bB = 1 - bA - bC;
        csVector2 newPoint = a * bA + b * bB + c * bC;
        density += densitymap.Get((int)newPoint.x, (int)newPoint.y, 0);
      }

    if (count != 0)
      density /= count;
  }


  void FurMesh::SetGuideLOD(float guideLOD)
  {
    // clamp
    guideLOD = csMin( csMax(guideLOD, 0.0f), 1.0f );

    this->guideLOD = guideLOD;

    if (!physicsControlEnabled) // no physics support
      return;

    // Deactivate all guide furs
    for (size_t i = 0 ; i < guideFursLOD.GetSize(); i ++)
    {
      guideFursLOD.Get(i).isActive = false;
      physicsControl->RemoveStrand(i + guideFurs.GetSize());
    }

    // Use LOD (non-pure) guide ropes
    for (size_t i = 0 ; i < guideFursLOD.GetSize(); i ++)
      if ( rng->Get() < guideLOD )
      {
        guideFursLOD.Get(i).isActive = true;
        physicsControl->InitializeStrand(i + guideFurs.GetSize(), 
          guideFursLOD.Get(i).controlPoints, 
          guideFursLOD.Get(i).GetControlPointsCount(controlPointsLOD));
      }
  }

  void FurMesh::SetStrandLOD(float strandLOD)
  {
    // clamp
    strandLOD = csMin( csMax(strandLOD, 0.0f), 1.0f );

    this->strandLOD = strandLOD;
    size_t totalGuideHairsCount = guideFurs.GetSize() + guideFursLOD.GetSize();

    size_t minFurCount = csMin(furStrands.GetSize(), totalGuideHairsCount);

    furStrandsLODSize = (size_t)(minFurCount + 
      (int)(strandLOD * ((int)furStrands.GetSize() - (int)minFurCount)));

    strandWidthLOD = 1 / ( strandLOD * 0.75f + 0.25f ) * GetStrandWidth();

    size_t controlPointsCount = 0;

    for (size_t i = 0 ; i < furStrandsLODSize ; i ++)
      controlPointsCount += 
      furStrands.Get(i).GetControlPointsCount(controlPointsLOD);

    endVertex = offsetVertex + 2 * controlPointsCount;

//     csPrintf("Fur Strands Displayed: %u\n", furStrandsLODSize);
//     csPrintf("Vertices Displayed: %u\n", endVertex - offsetVertex);
  }

  void FurMesh::SetControlPointsLOD(float controlPointsLOD)
  {
    // clamp
    controlPointsLOD = csMin( csMax(controlPointsLOD, 0.0f), 1.0f );

    // No changes needed
    if (GetControlPointsCount(controlPointsLOD) == 
      GetControlPointsCount(this->controlPointsLOD))
      return;

    float oldControlPointsLOD = this->controlPointsLOD;
    this->controlPointsLOD = controlPointsLOD;

    // Get control points LOD
    offsetIndex = 0;
    offsetVertex = 0;

    size_t controlPointsCount = 0;

    if (0.0f <= controlPointsLOD && controlPointsLOD < 0.67f)
      controlPointsCount += GetControlPointsCount(1.0f);
    if (0.0f <= controlPointsLOD && controlPointsLOD <= 0.33f)
    {
      controlPointsCount += GetControlPointsCount(0.5f);
      offsetIndex -= 2 * furStrands.GetSize();
    }

    if (controlPointsCount)
    {
      offsetIndex += 2 * (controlPointsCount - furStrands.GetSize());
      offsetVertex += 2 * controlPointsCount;
    }

    controlPointsCount = 0;

    for (size_t i = 0 ; i < furStrandsLODSize ; i ++)
      controlPointsCount += 
      furStrands.Get(i).GetControlPointsCount(controlPointsLOD);

    endVertex = offsetVertex + 2 * controlPointsCount;

    // re-init guide furs
    StopAnimationControl();

    size_t averageControlPointsCount = 0;

    // Update guide furs
    for (size_t i = 0 ; i < guideFurs.GetSize() ; i ++)
    {
      csGuideFur guideFur = guideFurs.Get(i);

      size_t controlPointsCountMax = 
        guideFur.GetControlPointsCount(oldControlPointsLOD);
      if (!controlPointsCountMax)
        continue;

      csVector3 direction = guideFur.controlPoints[controlPointsCountMax - 1] - 
        guideFur.controlPoints[0];
      float distance = csVector3::Norm(direction);
      direction.Normalize();

      size_t controlPointsCount = guideFur.GetControlPointsCount(controlPointsLOD);
      averageControlPointsCount += controlPointsCount;

      for (size_t j = 0 ; j < controlPointsCount ; j ++ )
        guideFur.controlPoints[j] = guideFur.controlPoints[0] + direction * 
        (j * (distance / (controlPointsCount - 1)));
    }

//     csPrintf( "Average Control Points Count: %f\n", 
//       (float)averageControlPointsCount / (float)guideFurs.GetSize());

    // Update guide ropes LOD
    for (size_t i = 0 ; i < guideFursLOD.GetSize(); i ++)
      guideFursLOD.Get(i).Update(guideFurs, guideFursLOD, controlPointsLOD);

    // Update fur strands
    for (size_t i = 0 ; i < furStrands.GetSize(); i ++)
      furStrands.Get(i).Update(guideFurs, guideFursLOD, controlPointsLOD);    

    StartAnimationControl();

//     csPrintf("Vertices Displayed: %u\n", endVertex - offsetVertex);
  }

  void FurMesh::SetLOD(float lod)
  {
    SetGuideLOD(lod);
    SetStrandLOD(lod);
    SetControlPointsLOD(lod);

    // Print the number of control furs
    size_t count = guideFurs.GetSize();
    for (size_t i = 0 ; i < guideFursLOD.GetSize(); i ++)
      if (guideFursLOD.Get(i).isActive)
        count++;

//     csPrintf("Guide Strands: %u/%u\n", count, guideFurs.GetSize() + 
//       guideFursLOD.GetSize());
  }

  void FurMesh::SynchronizeGuideHairs ()
  {
    if (!physicsControlEnabled) // no physics support
      return;

    for (size_t i = 0 ; i < guideFurs.GetSize(); i ++)
      physicsControl->InitializeStrand(i,guideFurs.Get(i).controlPoints, 
      guideFurs.Get(i).GetControlPointsCount(controlPointsLOD));
  }

  void FurMesh::SetAnimatedMesh(CS::Mesh::iAnimatedMesh* animesh)
  {
    this->animesh = animesh;
  }
  
  void FurMesh::SetAnimationControl (CS::Animation::iFurAnimationControl* physicsControl)
  {
    this->physicsControl = physicsControl;
  }

  void FurMesh::StartAnimationControl()
  {
    if (!guideFurs.GetSize())
    {
      csPrintfErr("Geometry not generated. Animation not started!\n");
      return;
    }

    if (!physicsControl)
    {
      csPrintfErr("No physics control specified!\n");
      return;
    }

    if (!physicsControlEnabled)
    {
      physicsControlEnabled = true;
      SynchronizeGuideHairs();
      SetGuideLOD(previousGuideLOD);
    }
  }

  void FurMesh::StopAnimationControl()
  {
    if (!physicsControl)
    {
      csPrintfErr("No physics control specified!\n");
      return;
    }

    if (physicsControlEnabled)
    {
      previousGuideLOD = guideLOD;
      SetGuideLOD(0);
      physicsControlEnabled = false;
      physicsControl->RemoveAllStrands();
    }
  }

  void FurMesh::EnableMesh()
  {
    if(isEnabled)
      return;

    isEnabled = true;
    StartAnimationControl();
    ResetMesh();
  }

  void FurMesh::ResetMesh()
  {
    if (!guideFurs.GetSize())
    {
      csPrintfErr("Geometry not generated. Mesh not reset!\n");
      return;
    }

    if(!physicsControlEnabled)
      return;

    StopAnimationControl();

    // Delete fur geometry data
    for (size_t i = 0 ; i < guideFurs.GetSize() ; i ++)
      guideFurs.Get(i).Clear();

    guideFurs.DeleteAll();

    isReset = true;
  }

  void FurMesh::DisableMesh()
  {
    if (!isEnabled)
      return;

    isEnabled = false;
    StopAnimationControl();
    
    if (!furStrands.GetSize())
      SetIndexRange(0, 0);
    else
      SetIndexRange(0, 1);
  }

  void FurMesh::SetMeshFactory ( CS::Mesh::iAnimatedMeshFactory* meshFactory)
  {
    this->meshFactory = meshFactory;
  }

  void FurMesh::SetMeshFactorySubMesh ( CS::Mesh::iAnimatedMeshSubMeshFactory* 
    meshFactorySubMesh )
  { 
    this->meshFactorySubMesh = meshFactorySubMesh;
  }

  void FurMesh::SetFurMeshProperties
    ( CS::Mesh::iFurMeshMaterialProperties* hairMeshProperties)
  {
    this->hairMeshProperties = hairMeshProperties;
  }

  CS::Mesh::iFurMeshMaterialProperties* FurMesh::GetFurMeshProperties( ) const
  {
    return hairMeshProperties;
  }

  void FurMesh::UpdateGuideHairs()
  {
    // Update guide ropes
    for (size_t i = 0 ; i < guideFurs.GetSize(); i ++)
      physicsControl->AnimateStrand(i, guideFurs.Get(i).controlPoints,
        guideFurs.Get(i).GetControlPointsCount(controlPointsLOD));

    // Update guide ropes LOD
    for (size_t i = 0 ; i < guideFursLOD.GetSize(); i ++)
      if ( guideFursLOD.Get(i).isActive )
        physicsControl->AnimateStrand(i + guideFurs.GetSize(),
          guideFursLOD.Get(i).controlPoints, 
          guideFursLOD.Get(i).GetControlPointsCount(controlPointsLOD));        
      else
        guideFursLOD.Get(i).Update(guideFurs, guideFursLOD, controlPointsLOD);
  }

  void FurMesh::Update()
  {
    // Update shader
    if (hairMeshProperties)
      hairMeshProperties->Update();

    // First update the control points
    if (physicsControlEnabled)
      UpdateGuideHairs();

    size_t numberOfStrains = furStrandsLODSize;

    if (!numberOfStrains)
      return;

    // Then update the hair strands
    if (physicsControlEnabled)
      for (size_t i = 0 ; i < numberOfStrains; i ++)
        furStrands.Get(i).Update(guideFurs, guideFursLOD, controlPointsLOD);

    const csOrthoTransform& tc = view -> GetCamera() ->GetTransform ();

    csVector3* vbuf = 
      (csVector3*)GetVertices()->Lock(CS_BUF_LOCK_NORMAL);
    csVector3* normals = 
      (csVector3*)GetNormals()->Lock(CS_BUF_LOCK_NORMAL); 
    csVector3* tangents = 
      (csVector3*)GetTangents()->Lock(CS_BUF_LOCK_NORMAL);
    csVector3* binormals = 
      (csVector3*)GetBinormals()->Lock(CS_BUF_LOCK_READ);

    csVector3 normal, tangent, binormal, cameraOrigin;
    csVector3 strip, firstPoint, secondPoint;

    // In case strand width changes real-time
    size_t controlPointsCount = GetControlPointsCount(1.0f);

    if (controlPointsDeviation != GetControlPointsDeviation())
    {
      for ( size_t i = 0 ; i < controlPointsCount ; i ++)
        positionShift[i] = csVector3(rng->Get() * 2 - 1, rng->Get() * 2 - 1, 
        rng->Get() * 2 - 1) * GetControlPointsDeviation();

      controlPointsDeviation = GetControlPointsDeviation();
    }

    cameraOrigin = tc.GetOrigin();
    csVector3 *furShift = furStrandShift;
    csVector3 *posShift = positionShift;

    vbuf += offsetVertex;
    normals += offsetVertex;
    binormals += offsetVertex;
    tangents += offsetVertex;

    size_t triangleCount = 0;

    // In case strand width changes real-time
    strandWidthLOD = 1 / ( strandLOD * 0.75f + 0.25f ) * GetStrandWidth();

    for ( size_t x = 0 ; x < numberOfStrains ; x ++ , furShift ++)
    {
      size_t y = 0;
      tangent = csVector3(0);
      strip = csVector3(0);

      csVector3 *controlPoints = furStrands.Get(x).controlPoints;
      size_t controlPointsCount = 
        furStrands.Get(x).GetControlPointsCount(controlPointsLOD);
      triangleCount += 2 * controlPointsCount - 2;

      for ( y = 0 ; y < controlPointsCount - 1; y ++, controlPoints ++, 
        vbuf += 2, tangents += 2, binormals += 2, posShift ++, normals += 2 )
      {
        firstPoint = *controlPoints + 
          ( (*posShift) + (*furShift) * (*binormals).y ) * (y != 0);
        secondPoint = *(controlPoints + 1) + 
          ( (*posShift) + (*furShift) * (*binormals).y ) * (y != 0);

        csMath3::CalcNormal(binormal, secondPoint, firstPoint, cameraOrigin);
        binormal.Normalize();
        strip = strandWidthLOD * binormal * ((*binormals).z + 1.0f) * 
          ( GetPointiness() * 2.0f * (1.0f - (*binormals).y) + (1.0f - GetPointiness()) );

        (*vbuf) = firstPoint;
        (*(vbuf + 1)) = firstPoint - strip;

        tangent = secondPoint - firstPoint;
        tangent.Normalize();
        normal.Cross(tangent, binormal);

        (*normals) = normal;
        (*(normals + 1)) = normal;

        (*tangents) = tangent;
        (*(tangents + 1)) = tangent;
      }

      if (controlPointsCount)
      {
        firstPoint = *controlPoints + (*posShift) + (*furShift);
        secondPoint = *(controlPoints + 1) + (*posShift) + (*furShift);

        csMath3::CalcNormal(binormal, secondPoint, firstPoint, cameraOrigin);
        binormal.Normalize();
        strip = strandWidthLOD * binormal * ((*binormals).z + 1.0f) * 
          ( GetPointiness() * 2.0f * (1.0f - (*binormals).y) + (1.0f - GetPointiness()) );

        (*vbuf) = firstPoint;
        (*(vbuf + 1)) = firstPoint + strip;

        (*tangents) = tangent;
        (*(tangents + 1)) = tangent;

        (*normals) = normal;
        (*(normals + 1)) = normal;

        vbuf += 2;
        tangents += 2;
        binormals += 2;
        normals += 2;
        posShift ++;
      }
    }

    GetVertices()->Release();
    GetNormals()->Release();
    GetTangents()->Release();
    GetBinormals()->Release();

    SetIndexRange(3 * offsetIndex, 3 * offsetIndex + 3 * triangleCount );
  }

  void FurMesh::SaveUVImage()
  {
    // From normal guide ropes
    for (size_t i = 0 ; i < guideFurs.GetSize() ; i ++)
    {
      csVector2 uv = guideFurs.Get(i).uv;

      densitymap.Set( (int)(uv.x * densitymap.width), 
        (int)(uv.y * densitymap.height), 1, 255);

      heightmap.Set( (int)(uv.x * heightmap.width),  
        (int)(uv.y * heightmap.height), 1, 255);
    }

    // From LOD guide ropes
    for (size_t i = 0 ; i < guideFursLOD.GetSize() ; i ++)
    {
      csVector2 uv = guideFursLOD.Get(i).uv;

      densitymap.Set( (int)(uv.x * densitymap.width),
        (int)(uv.y * densitymap.height), 0, 255);
    }

    // From fur strands
    for (size_t i = 0 ; i < furStrands.GetSize() ; i ++)
    {
      csVector2 uv = furStrands.Get(i).uv;

      densitymap.Set( (int)(uv.x * densitymap.width),
        (int)(uv.y * densitymap.height), 2, 255);
    }

    densitymap.SaveImage(object_reg, 
      "/data/hairtest/densitymap_debug.png");
    heightmap.SaveImage(object_reg, 
      "/data/hairtest/heightmap_debug.png");
  }
}
CS_PLUGIN_NAMESPACE_END(FurMesh)
