/*
Copyright (C) 2005-2005 by Jorrit Tyberghein

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

#include "cssysdef.h"
#include "csgeom/math3d.h"
#include "csgeom/matrix3.h"
#include "csgfx/renderbuffer.h"
#include "csutil/randomgen.h"
#include "iengine/material.h"
#include "igraphic/image.h"

#include "densityfactormap.h"
#include "../engine.h"
#include "meshgen.h"
#include "meshgen_positionmap.h"


CS_PLUGIN_NAMESPACE_BEGIN(Engine)
{

csMeshGeneratorGeometry::csMeshGeneratorGeometry (
  csMeshGenerator* generator) : scfImplementationType (this),
  min_draw_dist (0),
  sq_min_draw_dist (0),
  min_opaque_dist (0),
  max_opaque_dist (CS::Infinity()),
  generator (generator)
{
  colldetID = generator->GetStringSet()->Request ("colldet");
  radius = 0.0f;
  density = 1.0f;
  total_max_dist = 0.0f;
  default_material_factor = 0.0f;
  celldim = 0;
  positions = 0;
  wind_direction = csVector2(0.0f);
  wind_bias = 1.0f;
  wind_speed = 1.0f;
}

csMeshGeneratorGeometry::~csMeshGeneratorGeometry ()
{
}

void csMeshGeneratorGeometry::AddSVToMesh (iMeshWrapper* mesh,  
                                           csShaderVariable* sv) 
{ 
  if (!mesh) return; 
  mesh->GetSVContext()->AddVariable (sv); 
  iSceneNode* sn = mesh->QuerySceneNode (); 
  csRef<iSceneNodeArray> children = sn->GetChildrenArray (); 
  size_t i; 
  for (i = 0 ; i < children->GetSize () ; i++) 
    AddSVToMesh (children->Get (i)->QueryMesh (), sv); 
} 

void csMeshGeneratorGeometry::SetMeshBBox (iMeshWrapper* mesh,  
                                           const csBox3& bbox) 
{ 
  if (!mesh) return; 
  mesh->GetMeshObject()->GetObjectModel()->SetObjectBoundingBox (bbox); 
  // FIXME UGLY UGLY UGLY
  // So the player doesn't collide with the huge mesh bbox. 
 	mesh->GetMeshObject()->GetObjectModel()->SetTriangleData (colldetID, 0); 
  iSceneNode* sn = mesh->QuerySceneNode (); 
  csRef<iSceneNodeArray> children = sn->GetChildrenArray (); 
  size_t i; 
  for (i = 0 ; i < children->GetSize () ; i++) 
    SetMeshBBox (children->Get (i)->QueryMesh (), bbox); 
} 

void csMeshGeneratorGeometry::SetupGeomMeshObject (GeomMeshObject& meshobj,
                                                   const csMGGeom& geom)
{
  meshobj.mesh = generator->engine->CreateMeshWrapper (geom.factory, 0);
  meshobj.mesh->GetFlags ().Set (CS_ENTITY_NOHITBEAM);
  meshobj.mesh->SetZBufModeRecursive (CS_ZBUF_USE);
  meshobj.instancesNumVar.AttachNew (new csShaderVariable (generator->varInstancesNum));
  meshobj.transformVar.AttachNew (new csShaderVariable (generator->varTransform));
  meshobj.instanceExtraVar.AttachNew (new csShaderVariable (generator->varInstanceExtra));
  AddSVToMesh (meshobj.mesh, geom.windDataVar);
  AddSVToMesh (meshobj.mesh, meshobj.instancesNumVar);
  AddSVToMesh (meshobj.mesh, geom.fadeInfoVar);
  AddSVToMesh (meshobj.mesh, meshobj.transformVar);
  AddSVToMesh (meshobj.mesh, meshobj.instanceExtraVar);

  float maxdist (geom.maxdistance);
  csBox3 bbox;
  bbox.SetSize (csVector3 (maxdist, maxdist, maxdist));
  SetMeshBBox (meshobj.mesh, bbox);
}

GeomMeshObjectArrayRefCounted* csMeshGeneratorGeometry::GetMeshObjects (iCamera* cam)
{
  csRef<GeomMeshObjectArrayRefCounted> meshobj_array (
    meshobjs.Get (cam, csRef<GeomMeshObjectArrayRefCounted> ()));
  if (meshobj_array) return meshobj_array;
  meshobj_array.AttachNew (new GeomMeshObjectArrayRefCounted);
  for (size_t f = 0; f < factories.GetSize(); f++)
  {
    GeomMeshObject meshobj;
    SetupGeomMeshObject (meshobj, factories[f]);
    static_cast<csMeshWrapper*> ((iMeshWrapper*)(meshobj.mesh))
      ->SetCameraRestriction (cam);
    meshobj_array->Insert (f, meshobj);
  }
  meshobjs.PutUnique (cam, meshobj_array);
  return meshobj_array;
}

void csMeshGeneratorGeometry::GetDensityMapFactor (float x, float z,
                                                   float &data)
{
  float factorMapScale = 1.0f;
  if (density_map)
  {
    density_map->SampleFloat (density_map_type, x, z, factorMapScale); 
    factorMapScale *= density_map_factor;  
  }
  data = factorMapScale;

  // Sum up all density factor maps
  if (densityFactorMaps.GetSize() > 0)
  {
    csVector3 worldCoord (x, 0, z);
    float factorMapSum = 0;
    for (size_t i = 0; i < densityFactorMaps.GetSize(); i++)
    {
      factorMapSum += densityFactorMaps[i].first->GetDensity (worldCoord)
	* densityFactorMaps[i].second;
    }    
    
    data *= factorMapSum;
  }
}

void csMeshGeneratorGeometry::SetDensityMap (iTerraFormer* map, float factor, 
                                             const csStringID &type)
{
  density_map = map;
  density_map_factor = factor;
  density_map_type = type;
}

bool csMeshGeneratorGeometry::UseDensityFactorMap (const char* factorMapID,
						   float factor)
{
  DensityFactorMap* factorMap = generator->GetDensityFactorMap (factorMapID);
  if (!factorMap) return false;
  densityFactorMaps.Push (DensityFactorMapScalePair (factorMap, factor));
  return true;
}

void csMeshGeneratorGeometry::SetMinimumDrawDistance (float dist)
{
  min_draw_dist = dist;
  sq_min_draw_dist = dist*dist;
}

void csMeshGeneratorGeometry::SetMinimumOpaqueDistance (float dist)
{
  min_opaque_dist = dist;
}

void csMeshGeneratorGeometry::SetMaximumOpaqueDistance (float dist)
{
  max_opaque_dist = dist;
}

void csMeshGeneratorGeometry::AddPositionsFromMap (iTerraFormer* map,
	const csBox2 &region, uint resx, uint resy, float value,
	const csStringID & type)
{
  csRef<iTerraSampler> sampler = map->GetSampler (region, resx, resy);
  float stepx = (region.MaxX () - region.MinX ())/resx;
  float stepy = (region.MaxY () - region.MinY ())/resy;

  const float* map_values = sampler->SampleFloat (type);
  float curx = region.MinX (), cury = region.MinY ();
  for (uint i = 0; i < resx; i++)
  {
    for (uint j = 0; j < resy; j++)
    {
      if (map_values[i*resx + j] == value)
      {
        AddPosition (csVector2 (curx, cury));
      }
      curx += stepx;
    }
    curx = region.MinX ();
    cury += stepy;
  }
}

void csMeshGeneratorGeometry::ResetManualPositions (int new_celldim)
{
  if (celldim == new_celldim)
    return;

  csArray<csVector2> tmp_pos;
  for (int i = 0; i < celldim; i++)
  {
    for (size_t j = 0; j < positions[i].GetSize (); j++)
    {
      tmp_pos.Push (positions[i][j]);
    }
  }

  delete[] positions;

  celldim = new_celldim;
  positions = new csArray<csVector2> [celldim*celldim];

  for (size_t i = 0; i < tmp_pos.GetSize (); i++)
  {
    AddPosition (tmp_pos[i]);
  }
}

void csMeshGeneratorGeometry::AddPosition (const csVector2 &pos)
{
  ResetManualPositions (generator->GetCellCount ());
  positions[generator->GetCellId (pos)].Push (pos);
}

void csMeshGeneratorGeometry::AddDensityMaterialFactor (
  iMaterialWrapper* material, float factor)
{
  csMGDensityMaterialFactor mf;
  mf.material = material;
  mf.factor = factor;
  material_factors.Push (mf);
}

static int CompareGeom (csMGGeom const& m1, csMGGeom const& m2)
{
  if (m1.maxdistance < m2.maxdistance) return -1;
  else if (m1.maxdistance > m2.maxdistance) return 1;
  else return 0;
}

void csMeshGeneratorGeometry::AddFactory (iMeshFactoryWrapper* factory,
                                          float maxdist)
{
  csMGGeom g;
  g.factory = factory;
  g.maxdistance = maxdist;
  g.sqmaxdistance = maxdist * maxdist;
  g.factory->GetMeshObjectFactory ()->SetMixMode (CS_FX_COPY);
  g.windDataVar.AttachNew (new csShaderVariable (generator->varWindData));
  g.windDataVar->SetType (csShaderVariable::VECTOR3);
  g.fadeInfoVar.AttachNew (new csShaderVariable (generator->varFadeInfo));
  g.fadeInfoVar->SetType (csShaderVariable::VECTOR4);
  //GeomMeshObject meshobj;
  //SetupGeomMeshObject (meshobj, g);

  /*size_t factoryIndex = */factories.InsertSorted (g, CompareGeom);
  //meshobjs->Insert (factoryIndex, meshobj);

  if (maxdist > total_max_dist) total_max_dist = maxdist;
}

void csMeshGeneratorGeometry::RemoveFactory (size_t idx)
{
  factories.DeleteIndex (idx);
}

void csMeshGeneratorGeometry::SetRadius (float radius)
{
  csMeshGeneratorGeometry::radius = radius;
}

void csMeshGeneratorGeometry::SetDensity (float density)
{
  csMeshGeneratorGeometry::density = density;
}

bool csMeshGeneratorGeometry::AllocMesh (iCamera* cam,
  int cidx, const csMGCell& cell, float sqdist,
  csMGPosition& pos)
{
  if (sqdist < sq_min_draw_dist) return false;
  size_t lod = GetLODLevel (sqdist);
  if (lod == csArrayItemNotFound) return false;
  pos.lod = lod;

  GeomMeshObject& meshobj = (*GetMeshObjects (cam))[lod];
  size_t i = meshobj.allPositions.GetSize();
  pos.idInGeometry = i;
  csMGPosition*& newPos = meshobj.allPositions.GetExtend (i);
  newPos = &pos;
  
  meshobj.allTransforms.GetExtend (i);
  meshobj.allInstanceExtra.GetExtend (i).Set (rng.Get());

  meshobj.dataDirty = true;
  
  return true;
}

void csMeshGeneratorGeometry::MoveMesh (iCamera* cam,
                                        int cidx,
                                        const csMGPosition& pos,
                                        const csVector3& position,
                                        const csMatrix3& matrix)
{
  GeomMeshObject& meshobj = (*GetMeshObjects (cam))[pos.lod];
  csVector3 meshpos = meshobj.mesh->GetMovable ()->GetFullPosition ();
  csVector3 relPos = position - meshpos;
  GeomMeshObject::Transform& tf = meshobj.allTransforms[pos.idInGeometry];

  /* Based on makeGLMatrix()
     (Transposition 'inherited' from there ...) */
  tf.m[0] = matrix.m11;
  tf.m[4] = matrix.m12;
  tf.m[8] = matrix.m13;

  tf.m[1] = matrix.m21;
  tf.m[5] = matrix.m22;
  tf.m[9] = matrix.m23;

  tf.m[2] = matrix.m31;
  tf.m[6] = matrix.m32;
  tf.m[10] = matrix.m33;

  tf.m[3] = relPos.x;
  tf.m[7] = relPos.y;
  tf.m[11] = relPos.z;

  meshobj.dataDirty = true;
}

void csMeshGeneratorGeometry::SetFadeParams (csMGGeom& geom, float opaqueDist, float scale)
{
  float fadeInM, fadeInN, fadeOutM, fadeOutN;
  float max_opaque_dist = csMin (this->max_opaque_dist, opaqueDist);
  float max_draw_dist = csMin (total_max_dist,
                               (fabsf (scale) > EPSILON ? 1.0f/scale : 0) + opaqueDist);
  float min_fade_dist = min_opaque_dist-min_draw_dist;
  if (fabsf (min_fade_dist) > EPSILON)
  {
    fadeInM = 1.0f/(min_fade_dist);
    fadeInN = -min_draw_dist * fadeInM;
  }
  else
  {
    fadeInM = 0;
    fadeInN = 1;
  }
  float max_fade_dist = max_opaque_dist-max_draw_dist;
  if (fabsf (max_fade_dist) > EPSILON)
  {
    fadeOutM = 1.0f/(max_fade_dist);
    fadeOutN = -max_opaque_dist*fadeOutM + 1;
  }
  else
  {
    fadeOutM = 0;
    fadeOutN = 1;
  }

  geom.fadeInfoVar->SetValue (csVector4 (fadeInM, fadeInN, fadeOutM, fadeOutN));
}

void csMeshGeneratorGeometry::SetWindDirection (float x, float z)
{
  wind_direction.x = x;
  wind_direction.y = z;

  for (size_t g = 0; g < factories.GetSize (); ++g)
  {
    csVector2 maxOffset = wind_direction * wind_speed;
    factories[g].windDataVar->SetValue (csVector3(maxOffset.x, maxOffset.y, wind_bias));
  }
}

void csMeshGeneratorGeometry::SetWindBias (float bias)
{
  wind_bias = csClamp (bias, 1.0f, 0.0f);

  for (size_t g = 0; g < factories.GetSize (); ++g)
  {
    csVector2 maxOffset = wind_direction * wind_speed;
    factories[g].windDataVar->SetValue (csVector3(maxOffset.x, maxOffset.y, wind_bias));
  }
}

void csMeshGeneratorGeometry::SetWindSpeed (float speed)
{
  wind_speed = csClamp (speed, FLT_MAX, 0.0f);

  for (size_t g = 0; g < factories.GetSize (); ++g)
  {
    csVector2 maxOffset = wind_direction * wind_speed;
    factories[g].windDataVar->SetValue (csVector3(maxOffset.x, maxOffset.y, wind_bias));
  }
}

void csMeshGeneratorGeometry::FreeMesh (iCamera* cam, csMGPosition& pos)
{
  GeomMeshObject& meshobj = (*GetMeshObjects (cam))[pos.lod];
  
  size_t index = pos.idInGeometry;
  meshobj.allPositions.DeleteIndexFast (index);
  meshobj.allTransforms.DeleteIndexFast (index);
  meshobj.allInstanceExtra.DeleteIndexFast (index);
  meshobj.dataDirty = true;
  if (index < meshobj.allPositions.GetSize())
  {
    csMGPosition*& newPos = meshobj.allPositions[index];
    newPos->idInGeometry = index;
  }
}

template<typename DataArray>
static void UpdateInstancingParams (bool allDataDirty,
                                    const DataArray& array,
                                    csRef<iRenderBuffer>& buffer,
                                    csShaderVariable* sv)
{
  bool updateData = allDataDirty;
  if (!buffer
    || (buffer->GetElementCount() != array.Capacity()))
  {
    buffer = csRenderBuffer::CreateRenderBuffer (array.Capacity(),
                                                 CS_BUF_STREAM, CS_BUFCOMP_FLOAT,
                                                 sizeof (typename DataArray::ValueType) / sizeof(float));
    sv->SetValue (buffer);
    updateData = true;
  }
  if (updateData) buffer->SetData (array.GetArray());
}

void csMeshGeneratorGeometry::FinishUpdate (iCamera* cam)
{
  float fadeOpaqueDist =
    generator->use_alpha_scaling ? generator->alpha_mindist : total_max_dist;
  float fadeDistScale =
    generator->use_alpha_scaling ? generator->alpha_scale : 0;

  size_t lod;
  for (lod = 0 ; lod < factories.GetSize () ; lod++)
  {
    csMGGeom& geom = factories[lod];
    GeomMeshObject& meshobj = (*GetMeshObjects (cam))[lod];
     if (meshobj.allPositions.GetSize() == 0)
    {
      // No instances
      meshobj.mesh->GetMovable()->ClearSectors ();
    }
    else
    {
      if (meshobj.mesh->GetMovable ()->GetSectors()->GetCount() == 0)
      {
	meshobj.mesh->GetMovable ()->SetSector (generator->GetSector ());
      }
      
      SetFadeParams (geom, fadeOpaqueDist, fadeDistScale);
      meshobj.instancesNumVar->SetValue (int (meshobj.allPositions.GetSize()));

      /* Update buffers
         (Compare capacity instead of size so a buffer is kept if the number of
         elements changes only slightly.) */
      UpdateInstancingParams (meshobj.dataDirty, meshobj.allTransforms,
                              meshobj.transformBuffer, meshobj.transformVar);

      UpdateInstancingParams (meshobj.dataDirty, meshobj.allInstanceExtra,
                              meshobj.instanceExtraBuffer, meshobj.instanceExtraVar);

      meshobj.dataDirty = false;
    }
    meshobj.mesh->GetMovable ()->UpdateMove ();
  }

  // @@@ FIXME: This actually something only needed once per frame
  meshobjs.Purge ();
}

size_t csMeshGeneratorGeometry::GetLODLevel (float sqdist)
{
  if (sqdist >= sq_min_draw_dist)
  {
    for (size_t i = 0 ; i < factories.GetSize () ; i++)
    {
      csMGGeom& geom = factories[i];
      if (sqdist <= geom.sqmaxdistance)
      {
        return i;
      }
    }
  }
  return csArrayItemNotFound;
}

bool csMeshGeneratorGeometry::IsRightLOD (float sqdist, size_t current_lod)
{
  if (sqdist < sq_min_draw_dist) return false;
  if (current_lod == 0)
    return (sqdist <= factories[0].sqmaxdistance);
  else
    return (sqdist <= factories[current_lod].sqmaxdistance) &&
    (sqdist > factories[current_lod-1].sqmaxdistance);
}

void csMeshGeneratorGeometry::UpdatePosition (iCamera* cam, const csVector3& pos)
{ 
  for (size_t f = 0; f < factories.GetSize(); f++) 
  { 
    GeomMeshObject& meshobj = (*GetMeshObjects (cam))[f];
    meshobj.mesh->GetMovable()->SetPosition (pos);
  } 
}

//--------------------------------------------------------------------------

void csMGCell::DisownBlock (csMGPositionBlock* block)
{
  csMGCell::PositionBlocksHash::GlobalIterator blocksIter (blocks.GetIterator());
  while (blocksIter.HasNext())
  {
    csRef<csMGPositionBlock> aBlock (blocksIter.NextNoAdvance ());
    if (aBlock == block)
    {
      blocks.DeleteElement (blocksIter);
      return;
    }
    blocksIter.Advance ();
  }
}

//--------------------------------------------------------------------------

csMeshGenerator::csMeshGenerator (csEngine* engine) : 
  scfImplementationType (this), total_max_dist (-1.0f), minRadius(-1.0f),
  use_alpha_scaling (false),
  last_pos (0, 0, 0), setup_cells (false),
  cell_dim (50), inuse_blocks (0), inuse_blocks_last (0),
  max_blocks (100), engine (engine)
{
  default_density_factor = 1.0f;

  cells = new csMGCell [cell_dim * cell_dim];

  for (size_t i = 0 ; i < size_t (max_blocks) ; i++)
  {
    csRef<csMGPositionBlock> new_block;
    new_block.AttachNew (new csMGPositionBlock ());
    cache_blocks.Push (new_block);
  }

  prev_cells.MakeEmpty ();

  for (size_t i = 0 ; i < CS_GEOM_MAX_ROTATIONS ; i++)
  {
    rotation_matrices[i] = csYRotMatrix3 (TWO_PI * float (i)
      / float (CS_GEOM_MAX_ROTATIONS));
  }

  alpha_priority = engine->GetAlphaRenderPriority ();
  object_priority = engine->GetObjectRenderPriority ();
  
  strings = csQueryRegistryTagInterface<iStringSet> (
    engine->objectRegistry, "crystalspace.shared.stringset");
  SVstrings = csQueryRegistryTagInterface<iShaderVarStringSet> (
    engine->objectRegistry, "crystalspace.shader.variablenameset");
  varInstancesNum = SVstrings->Request ("instances num");
  varTransform = SVstrings->Request ("instancing transforms");
  varFadeInfo = SVstrings->Request ("meshgen fade info");
  varInstanceExtra = SVstrings->Request ("instance extra");
  varWindData = SVstrings->Request ("wind data");
}

csMeshGenerator::~csMeshGenerator ()
{
  delete[] cells;
}

void csMeshGenerator::SelfDestruct ()
{
  if (GetSector ())
  {
    size_t c = GetSector ()->GetMeshGeneratorCount ();
    while (c > 0)
    {
      c--;
      if (GetSector ()->GetMeshGenerator (c) == (iMeshGenerator*)this)
      {
        GetSector ()->RemoveMeshGenerator (c);
        return;
      }
    }
    CS_ASSERT (false);
  }
}

float csMeshGenerator::GetTotalMaxDist ()
{
  if (total_max_dist < 0.0f)
  {
    total_max_dist = 0.0f;
    size_t i;
    for (i = 0 ; i < geometries.GetSize () ; i++)
    {
      float md = geometries[i]->GetTotalMaxDist ();
      if (md > total_max_dist) total_max_dist = md;
    }
    sq_total_max_dist = total_max_dist * total_max_dist;
  }
  return total_max_dist;
}

void csMeshGenerator::SetDensityScale (float mindist, float maxdist,
                                       float maxdensityfactor)
{
}

void csMeshGenerator::SetAlphaScale (float mindist, float maxdist)
{
  use_alpha_scaling = true;
  alpha_mindist = mindist;
  sq_alpha_mindist = alpha_mindist * alpha_mindist;
  alpha_maxdist = maxdist;

  alpha_scale = 1.0f / (alpha_maxdist - alpha_mindist);
}

void csMeshGenerator::SetCellCount (int number)
{
  if (setup_cells)
  {
    // We already setup so we need to deallocate all blocks first.
    int x, z;
    for (z = 0 ; z < cell_dim ; z++)
      for (x = 0 ; x < cell_dim ; x++)
      {
        int cidx = z*cell_dim + x;
        csMGCell& cell = cells[cidx];
        FreeMeshesInBlock (cell);
        csMGCell::PositionBlocksHash::GlobalIterator blocksIter (cell.blocks.GetIterator());
        while (blocksIter.HasNext())
        {
          csRef<csMGPositionBlock>& block (blocksIter.Next ());
          if (block)
          {
            block->parent_cell = csArrayItemNotFound;
            cache_blocks.Push (block);
            block->next = block->prev = 0;
            block = 0;
          }
        }
      }
      inuse_blocks = 0;
      inuse_blocks_last = 0;
      setup_cells = false;
  }
  cell_dim = number;
  delete[] cells;
  cells = new csMGCell [cell_dim * cell_dim];
}

void csMeshGenerator::SetBlockCount (int number)
{
  if (setup_cells)
  {
    // We already setup so we need to deallocate all blocks first.
    int x, z;
    for (z = 0 ; z < cell_dim ; z++)
      for (x = 0 ; x < cell_dim ; x++)
      {
        int cidx = z*cell_dim + x;
        csMGCell& cell = cells[cidx];
        FreeMeshesInBlock (cell);
        csMGCell::PositionBlocksHash::GlobalIterator blocksIter (cell.blocks.GetIterator());
        while (blocksIter.HasNext())
        {
          csRef<csMGPositionBlock>& block (blocksIter.Next ());
          if (block)
          {
            block->parent_cell = csArrayItemNotFound;
            cache_blocks.Push (block);
            block->next = block->prev = 0;
            block = 0;
          }
        }
      }
      inuse_blocks = 0;
      inuse_blocks_last = 0;

      setup_cells = false;
  }
  size_t i;
  if (number > max_blocks)
  {
    for (i = max_blocks ; i < size_t (number) ; i++)
    {
      csRef<csMGPositionBlock> new_block;
      new_block.AttachNew (new csMGPositionBlock ());
      cache_blocks.Push (new_block);
    }
  }
  else
  {
    for (i = number ; i < size_t (max_blocks) ; i++)
    {
      cache_blocks.Pop ();
    }
  }
  max_blocks = number;
}

void csMeshGenerator::SetSampleBox (const csBox3& box)
{
  samplebox = box;
  setup_cells = false;
}

void csMeshGenerator::SetupSampleBox ()
{
  if (setup_cells) return;
  setup_cells = true;
  samplefact_x = float (cell_dim) / (samplebox.MaxX () - samplebox.MinX ());
  samplefact_z = float (cell_dim) / (samplebox.MaxZ () - samplebox.MinZ ());
  samplecellwidth_x = 1.0f / samplefact_x;
  samplecellheight_z = 1.0f / samplefact_z;
  int x, z;
  int idx = 0;
  for (z = 0 ; z < cell_dim ; z++)
  {
    float wz = GetWorldZ (z);
    for (x = 0 ; x < cell_dim ; x++)
    {
      float wx = GetWorldX (x);
      cells[idx].box.Set (wx, wz, wx + samplecellwidth_x, wz + samplecellheight_z);
      // Here we need to calculate the meshes relevant for this cell (i.e.
      // meshes that intersect this cell as seen in 2D space).
      // @@@ For now we just copy the list of meshes from csMeshGenerator.
      cells[idx].meshes = meshes;
      idx++;
    }
  }
  for (size_t g = 0 ; g < geometries.GetSize () ; g++)
    geometries[g]->ResetManualPositions (cell_dim);
}

DensityFactorMap* csMeshGenerator::GetDensityFactorMap (const char* id) const
{
  return densityFactorMaps.Get (id, (DensityFactorMap*)nullptr);
}

int csMeshGenerator::GetCellId (const csVector2& pos)
{
  SetupSampleBox ();
  int cellx = GetCellX (pos.x);
  int cellz = GetCellZ (pos.y);
  return cellz*cell_dim + cellx;
}

size_t csMeshGenerator::CountPositions (int cidx, csMGCell& cell)
{
  random.Initialize ((unsigned int)cidx); // @@@ Consider using a better seed?
  size_t counter = 0;

  const csBox2& box = cell.box;
  float box_area = box.Area ();

  size_t i, j, g;
  for (g = 0 ; g < geometries.GetSize () ; g++)
  {
    float density = geometries[g]->GetDensity () * default_density_factor;
    size_t count = size_t (density * box_area);
    for (j = 0 ; j < count ; j++)
    {
      float x = random.Get (box.MinX (), box.MaxX ());
      float z = random.Get (box.MinY (), box.MaxY ());
      csVector3 start (x, samplebox.MaxY (), z);
      csVector3 end = start;
      end.y = samplebox.MinY ();
      bool hit = false;
      for (i = 0 ; i < cell.meshes.GetSize () ; i++)
      {
        csHitBeamResult rc = cell.meshes[i]->HitBeam (start, end);
        if (rc.hit)
        {
          end.y = rc.isect.y + 0.0001;
          hit = true;
          break;
        }
      }
      
      if (hit)
      {
        counter++;
      }
    }
  }
  return counter;
}

size_t csMeshGenerator::CountAllPositions ()
{
  size_t counter = 0;
  int x, z;
  for (z = 0 ; z < cell_dim ; z++)
    for (x = 0 ; x < cell_dim ; x++)
    {
      int cidx = z*cell_dim + x;
      csMGCell& cell = cells[cidx];
      counter += CountPositions (cidx, cell);
    }
  return counter;
}

void csMeshGenerator::GeneratePositions (int cidx, csMGCell& cell,
                                         csMGPositionBlock* block)
{
  random.Initialize ((unsigned int)cidx); // @@@ Consider using a better seed?

  block->positions.Empty ();
  block->needPositions = false;

  const csBox2& box = cell.box;
  float box_area = box.Area ();
  
  CS_ALLOC_STACK_ARRAY(float, geoRadii, geometries.GetSize ());
  for (size_t i = 0 ; i < geometries.GetSize () ; i++)
    geoRadii[i] = geometries[i]->GetRadius();
  PositionMap positionMap (geoRadii, geometries.GetSize (),
			   box, cidx);

  if(minRadius < 0.0f)
  {
    for (size_t i = 0 ; i < geometries.GetSize () ; i++)
    {
      if(geometries[i]->GetRadius() < minRadius || minRadius < 0.0f)
      {
        minRadius = geometries[i]->GetRadius();
      }
    }
  }

  size_t i, j;
  for (size_t g = 0 ; g < geometries.GetSize () ; g++)
  {
    csMGPosition pos;
    pos.geom_type = g;
    size_t mpos_count = geometries[g]->GetManualPositionCount (cidx);

    float density = geometries[g]->GetDensity () * default_density_factor;

    size_t count = (mpos_count > 0)? mpos_count : size_t (density * box_area);

    /* Estimation of actual number of generated positions
     * Count how many of the generated positions have actually caused a mesh
     * to be placed (densities < 1 will cause some position to be rejected...).
     * After generating minSamples positions, estimate the total number
     * of actually used positions, and break if that is reached. */
    size_t minSamples = ceil (sqrtf (count));
    size_t positionsUsed = 0;

    const csArray<csMGDensityMaterialFactor>& mftable
      = geometries[g]->GetDensityMaterialFactors ();
    float default_material_factor
      = geometries[g]->GetDefaultDensityMaterialFactor ();
    bool do_material = mftable.GetSize () > 0;
    for (j = 0 ; j < count ; j++)
    {
      float pos_factor;
      float r = geometries[g]->csMeshGeneratorGeometry::GetRadius();
      float x;
      float z;
      PositionMap::AreaID area;
      if (mpos_count == 0)
      {
        if(!positionMap.GetRandomPosition (r, x, z, area))
        {
          // Ran out of room in this cell.
          return;
        }
        geometries[g]->GetDensityMapFactor (x, z, pos_factor);
      }
      else
      {
        csVector2 mpos = geometries[g]->GetManualPosition (cidx,j);
        x = mpos.x; 
        z = mpos.y;
        pos_factor = 1.0f;
      }
      
      if (!((pos_factor < 0.0001) ||
        (pos_factor < 0.9999 && random.Get () > pos_factor)))
      {
        csVector3 start (x, samplebox.MaxY (), z);
        csVector3 end = start;
        end.y = samplebox.MinY ();
        bool hit = false;
        iMaterialWrapper* hit_material = 0;
        for (i = 0 ; i < cell.meshes.GetSize () ; i++)
        {
          csHitBeamResult rc = cell.meshes[i]->HitBeam (start, end,
            do_material);
          if (rc.hit)
          {
            pos.position = rc.isect;
            end.y = rc.isect.y + 0.0001;
            hit_material = rc.material;
            hit = true;
          }
        }
        if (hit)
        {
          if (do_material)
          {
            // We use material density tables.
            float factor = default_material_factor;
	    for (size_t mi = 0 ; mi < mftable.GetSize () ; mi++)
	    {
	      if (mftable[mi].material == hit_material)
	      {
		factor = mftable[mi].factor;
		break;
	      }
	    }

            if (factor < 0.0001 || (factor < 0.9999 && random.Get () > factor))
            {
              hit = false;
            }
          }
          if (hit)
          {
            int rot = int (random.Get (CS_GEOM_MAX_ROTATIONS));
            if (rot < 0) rot = 0;
            else if (rot >= CS_GEOM_MAX_ROTATIONS) rot = CS_GEOM_MAX_ROTATIONS-1;
            pos.rotation = rot;
            block->positions.Push (new csMGPosition (pos));
	    positionsUsed++;
	    
	    if (mpos_count == 0)
	      positionMap.MarkAreaUsed (area, r, x, z);
          }
        }
      }

      /* Estimate how many positions will actually be used based on the
       * running count of used positions. */
      size_t estimatedCount = ceil (count * (float (positionsUsed) / (j+1)));
      // If enough samples have been collected...
      if (j >= minSamples)
      {
	// ... and the estimated count has been hit, exit early.
	if (positionsUsed >= estimatedCount)
	  break;
      }
    }
  }
}

void csMeshGenerator::AllocateBlock (iCamera* cam, int cidx, csMGCell& cell)
{
  csRef<csMGPositionBlock> block (cell.blocks.Get (cam, csRef<csMGPositionBlock>()));
  if (block)
  {
    // Our block is already there. We just push it back to the
    // front of 'inuse_blocks' if it is not already there.
    if (block->prev)
    {
      // Unlink first.
      block->prev->next = block->next;
      if (block->next) block->next->prev = block->prev;
      else inuse_blocks_last = block->prev;

      // Link to front.
      block->next = inuse_blocks;
      block->prev = 0;
      inuse_blocks->prev = block;
      inuse_blocks = block;
    }
    // It is possible that our positions got cleared so we may have to
    // recalculate them here.
    if (block->needPositions)
    {
      GeneratePositions (cidx, cell, block);
    }
  }
  else if (cache_blocks.GetSize () > 0)
  {
    // We need a new block and one is available in the cache.
    block = cache_blocks.Pop ();
    CS_ASSERT (block->parent_cell == csArrayItemNotFound);
    CS_ASSERT (block->next == 0 && block->prev == 0);
    cell.blocks.Put (cam, block);
    block->parent_cell = cidx;
    // Link block to the front.
    block->next = inuse_blocks;
    block->prev = 0;
    if (inuse_blocks) inuse_blocks->prev = block;
    else inuse_blocks_last = block;
    inuse_blocks = block;

    GeneratePositions (cidx, cell, block);
  }
  else
  {
    // We need a new block and the cache is empty.
    // Now we take the last used block from 'inuse_blocks'.
    block = inuse_blocks_last;
    CS_ASSERT (block->parent_cell != csArrayItemNotFound);
    FreeMeshesInBlock (cells[block->parent_cell], block);
    cells[block->parent_cell].DisownBlock (block);
    cell.blocks.Put (cam, block);
    block->parent_cell = cidx;

    // Unlink first.
    block->prev->next = 0;
    inuse_blocks_last = block->prev;

    // Link to front.
    block->next = inuse_blocks;
    block->prev = 0;
    inuse_blocks->prev = block;
    inuse_blocks = block;

    GeneratePositions (cidx, cell, block);
  }
}

void csMeshGenerator::AllocateMeshes (iCamera* cam,
                                      int cidx, csMGCell& cell,
                                      const csVector3& pos,
                                      const csVector3& delta)
{
  csRef<csMGPositionBlock> block (cell.blocks.Get (cam, csRef<csMGPositionBlock> ()));
  CS_ASSERT (block);
  CS_ASSERT (sector != 0);
  csPDelArray<csMGPosition>& positions = block->positions;
  GetTotalMaxDist ();
  size_t i;
  for (i = 0 ; i < positions.GetSize () ; i++)
  {
    csMGPosition& p = *(positions[i]);

    float sqdist = csSquaredDist::PointPoint (pos, p.position);
    if (sqdist < sq_total_max_dist)
    {
      if (p.idInGeometry == csArrayItemNotFound)
      {
        // We didn't have a mesh here so we allocate one.
        if (geometries[p.geom_type]->AllocMesh (cam, cidx, cell, sqdist, p))
        {
          geometries[p.geom_type]->MoveMesh (cam, cidx, p,
                                              p.position, rotation_matrices[p.rotation]);
        }
      }
      else
      {
        // We already have a mesh but we check here if we should switch LOD level.
        if (!geometries[p.geom_type]->IsRightLOD (sqdist, p.lod))
        {
          // We need a different mesh here.
          geometries[p.geom_type]->FreeMesh (cam, p);
          if (geometries[p.geom_type]->AllocMesh (cam, cidx, cell, sqdist, p))
          {
            geometries[p.geom_type]->MoveMesh (cam, cidx, p,
              p.position, rotation_matrices[p.rotation]);
          }
          else
            p.idInGeometry = csArrayItemNotFound;
        }
        else if(!delta.IsZero())
        { 
          // LOD level is fine, adjust for new pos 
          geometries[p.geom_type]->MoveMesh (cam, cidx, p, 
            p.position, rotation_matrices[p.rotation]); 
        } 
      }
    }
    else
    {
      if (p.idInGeometry != csArrayItemNotFound)
      {
        geometries[p.geom_type]->FreeMesh (cam, p);
        p.idInGeometry = csArrayItemNotFound;
      }
    }
  }
}

void csMeshGenerator::SetDefaultDensityFactor (float factor)
{
  default_density_factor = factor;
  ClearAllPositions ();
}

void csMeshGenerator::ClearAllPositions ()
{
  SetupSampleBox ();
  for (int z = 0 ; z < cell_dim ; z++)
    for (int x = 0 ; x < cell_dim ; x++)
    {
      int cidx = z*cell_dim + x;
      csMGCell& cell = cells[cidx];
      FreeMeshesInBlock (cell);
    }
}

void csMeshGenerator::ClearPosition (const csVector3& pos)
{
  SetupSampleBox ();

  int cellx = GetCellX (pos.x);
  int cellz = GetCellZ (pos.z);

  int cidx = cellz*cell_dim + cellx;
  csMGCell& cell = cells[cidx];
  FreeMeshesInBlock (cell);
}

void csMeshGenerator::UpdateForPosition (iCamera* cam, const csVector3& pos)
{
  csVector3 delta = pos - last_pos;

  last_pos = pos;
  SetupSampleBox ();

  for (size_t i = 0 ; i < geometries.GetSize () ; i++) 
  { 
    geometries[i]->UpdatePosition (cam, pos); 
  } 

  int cellx = GetCellX (pos.x);
  int cellz = GetCellZ (pos.z);

  // Total maximum distance for all geometries.
  float md = GetTotalMaxDist ();
  float sqmd = md * md;
  // Same in cell counts:
  int cell_x_md = 1+int (md * samplefact_x);
  int cell_z_md = 1+int (md * samplefact_z);

  // @@@ This can be done more efficiently.
  csVector2 pos2d (pos.x, pos.z);
  int x, z;

  int minz = cellz - cell_z_md;
  if (minz < 0) minz = 0;
  int maxz = cellz + cell_z_md+1;
  if (maxz > cell_dim) maxz = cell_dim;

  int minx = cellx - cell_x_md;
  if (minx < 0) minx = 0;
  int maxx = cellx + cell_x_md+1;
  if (maxx > cell_dim) maxx = cell_dim;

  // Calculate the intersection of minx, ... with prev_minx, ...
  // This intersection represent all cells that are not used this frame
  // but may potentially have gotten meshes the previous frame. We need
  // to free those meshes.
  csRect cur_cells (minx, minz, maxx, maxz);
  if (!prev_cells.IsEmpty ())
  {
    for (z = prev_cells.ymin ; z < prev_cells.ymax ; z++)
      for (x = prev_cells.xmin ; x < prev_cells.xmax ; x++)
        if (!cur_cells.Contains (x, z))
        {
          int cidx = z*cell_dim + x;
          csMGCell& cell = cells[cidx];
          FreeMeshesInBlock (cell);
        }
  }
  prev_cells = cur_cells;

  int total_needed = 0;

  // Now allocate the cells that are close enough.
  for (z = minz ; z < maxz ; z++)
  {
    int cidx = z*cell_dim + minx;
    for (x = minx ; x < maxx ; x++)
    {
      csMGCell& cell = cells[cidx];
      float sqdist = cell.box.SquaredPosDist (pos2d);

      if (sqdist < sqmd)
      {
        total_needed++;
        if (total_needed >= max_blocks)
        {
          engine->Error (
            "The mesh generator needs more blocks than %d!", max_blocks);
          return;	// @@@ What to do here???
        }
        AllocateBlock (cam, cidx, cell);
        AllocateMeshes (cam, cidx, cell, pos, delta);
      }
      else
      {
        // Block is out of range. We keep the block in the cache
        // but free all meshes that are in it.
        FreeMeshesInBlock (cam, cell);
      }
      cidx++;
    }
  }

  // Housekeeping
  size_t i;
  for (i = 0 ; i < geometries.GetSize () ; i++)
  {
    geometries[i]->FinishUpdate (cam);
  }
}

void csMeshGenerator::FreeMeshesInBlock (iCamera* cam, csMGCell& cell)
{
  csRef<csMGPositionBlock> block (cell.blocks.Get (cam, csRef<csMGPositionBlock> ()));
  FreeMeshesInBlock (cam, block);
}

void csMeshGenerator::FreeMeshesInBlock (csMGCell& cell)
{
  csMGCell::PositionBlocksHash::GlobalIterator blocksIter (cell.blocks.GetIterator());
  while (blocksIter.HasNext())
  {
    csWeakRef<iCamera> cam;
    csRef<csMGPositionBlock> block (blocksIter.Next (cam));
    csRef<iCamera> cam_ref;
    cam.Get (cam_ref);
    FreeMeshesInBlock (cam_ref, block);
  }
}

void csMeshGenerator::FreeMeshesInBlock (csMGCell& cell, csMGPositionBlock* block)
{
  csMGCell::PositionBlocksHash::GlobalIterator blocksIter (cell.blocks.GetIterator());
  while (blocksIter.HasNext())
  {
    csWeakRef<iCamera> cam;
    csRef<csMGPositionBlock> aBlock (blocksIter.Next (cam));
    if (aBlock == block)
    {
      csRef<iCamera> cam_ref;
      cam.Get (cam_ref);
      FreeMeshesInBlock (cam_ref, block);
    }
  }
}

void csMeshGenerator::FreeMeshesInBlock (iCamera* cam, csMGPositionBlock* block)
{
  if (block)
  {
    csPDelArray<csMGPosition>& positions = block->positions;
    size_t i;
    for (i = 0 ; i < positions.GetSize () ; i++)
    {
      if (positions[i]->idInGeometry != csArrayItemNotFound)
      {
        CS_ASSERT (positions[i]->geom_type >= 0);
        CS_ASSERT (positions[i]->geom_type < geometries.GetSize ());
        geometries[positions[i]->geom_type]->FreeMesh (cam, *(positions[i]));
        positions[i]->idInGeometry = csArrayItemNotFound;
      }
    }
    block->needPositions = true;
  }
}

iMeshGeneratorGeometry* csMeshGenerator::CreateGeometry ()
{
  csMeshGeneratorGeometry* geom = new csMeshGeneratorGeometry (this);
  geometries.Push (geom);
  minRadius = -1.0f; // Requires a recalculate.
  total_max_dist = -1.0f;	// Requires a recalculate.
  geom->DecRef ();
  return geom;
}

void csMeshGenerator::RemoveGeometry (size_t idx)
{
  geometries.DeleteIndex (idx);
}

void csMeshGenerator::RemoveMesh (size_t idx)
{
  meshes.DeleteIndex (idx);
}

void csMeshGenerator::AddDensityFactorMap (const char* factorMapID,
					   iImage* mapImage,
					   const CS::Math::Matrix4& worldToMap)
{
  csRef<DensityFactorMap> factorMap;
  factorMap.AttachNew (new DensityFactorMap);
  factorMap->SetImage (mapImage);
  factorMap->SetWorldToMapTransform (worldToMap);
  densityFactorMaps.PutUnique (factorMapID, factorMap);
}

void csMeshGenerator::UpdateDensityFactorMap (const char* factorMapID, iImage* mapImage)
{
  DensityFactorMap* factorMap = densityFactorMaps.Get (factorMapID, 0);
  if (factorMap == 0) return;
  factorMap->SetImage (mapImage);
}

bool csMeshGenerator::IsValidDensityFactorMap (const char* factorMapID) const
{
  DensityFactorMap* factorMap = densityFactorMaps.Get (factorMapID, 0);
  return factorMap != 0;
}

const CS::Math::Matrix4& csMeshGenerator::GetWorldToMapTransform (const char* factorMapID) const
{
  DensityFactorMap* factorMap = densityFactorMaps.Get (factorMapID, 0);
  CS_ASSERT (factorMap != 0);
  return factorMap->GetWorldToMapTransform ();
}

int csMeshGenerator::GetDensityFactorMapWidth (const char* factorMapID) const
{
  DensityFactorMap* factorMap = densityFactorMaps.Get (factorMapID, 0);
  if (factorMap)
    return factorMap->GetWidth ();
  else
    return 0;
}

int csMeshGenerator::GetDensityFactorMapHeight (const char* factorMapID) const
{
  DensityFactorMap* factorMap = densityFactorMaps.Get (factorMapID, 0);
  if (factorMap)
    return factorMap->GetHeight ();
  else
    return 0;
}


}
CS_PLUGIN_NAMESPACE_END(Engine)
