/*
  Copyright (C) 2006 by Kapoulkine Arseny
                2007 by Marten Svanfeldt

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

#include "csgfx/imagememory.h"
#include "csgfx/renderbuffer.h"
#include "csgfx/rgbpixel.h"
#include "csgfx/shadervar.h"
#include "csgfx/shadervarcontext.h"
#include "cstool/rbuflock.h"
#include "csutil/objreg.h"
#include "csutil/refarr.h"
#include "iengine.h"
#include "imesh/terrain2.h"
#include "ivideo/rendermesh.h"
#include "ivideo/txtmgr.h"

#include "bruteblockrenderer.h"

CS_PLUGIN_NAMESPACE_BEGIN(Terrain2)
{

SCF_IMPLEMENT_FACTORY (csTerrainBruteBlockRenderer)

struct csBruteBlockTerrainRenderData;

/**
* This is one block in the terrain.
*/
struct csTerrBlock : public csRefCount
{
  csTerrBlock (csBruteBlockTerrainRenderData *data);
  ~csTerrBlock ();

  /// Load data from Former
  void LoadData ();

  /// Generate mesh
  void SetupMesh ();

  /// Detach the node from the tree
  void Detach ();

  /// Split block in 4 children
  void Split ();

  /// Merge block
  void Merge ();

  /// Checks if something needs to be merged or split
  void CalcLOD ();

  /// Returns true if this node is a leaf
  bool IsLeaf ()  const
  { return children[0] == 0; }

  void ReplaceChildNeighbours (csTerrBlock *a, csTerrBlock *b);

  void DrawTest (iGraphics3D* g3d, iRenderView *rview, uint32 frustum_mask,
    csReversibleTransform &transform, iMovable *movable,
    csDirtyAccessArray<csRenderMesh*>& meshes);


  CS_LEAKGUARD_DECLARE (csTerrBlock);

  csRef<iRenderBuffer> mesh_vertices;
  csRef<iRenderBuffer> mesh_normals;
  csRef<iRenderBuffer> mesh_texcoords;
  csRef<iRenderBuffer> mesh_heights;
  csRef<csRenderBufferHolder> bufferHolder;

  csVector2 center;
  csVector2 size;
  int res;

  // for this kind of grid:
  // . _ .
  // _ _ _
  // . _ .
  // left = top = 0, right = bottom = 2,
  // step = 2
  int left, top, right, bottom;
  int step, child;


  csBruteBlockTerrainRenderData *rdata;

  //          0
  //      ---------
  //      | 0 | 1 |
  //    2 |-------| 1
  //      | 2 | 3 |
  //      ---------
  //          3

  csTerrBlock* parent;
  csRef<csTerrBlock> children[4];
  csTerrBlock* neighbours[4];

  csBox3 bbox;

  bool detach;
  bool built;
};


struct csBruteBlockTerrainRenderData: public csRefCount
{
  csRef<csTerrBlock> rootblock;

  csRenderMeshHolder* rm_holder;

  csPlane3* planes;

  csRefArray<iMaterialWrapper>* material_palette;
  csArray<csRef<csShaderVariableContext> > sv_context;

  csArray<csRef<iTextureHandle> > alpha_map;

  csRef<csShaderVariableContext> baseContext;

  unsigned int primitive_count;

  int block_res_log2;
  float lod_lcoeff, lod_distance;


  csReversibleTransform tr_o2c;

  bool initialized;

  iTerrainCell* cell;
  csTerrainBruteBlockRenderer* renderer;

  csBruteBlockTerrainRenderData(iTerrainCell* cell, csTerrainBruteBlockRenderer* renderer)
    : cell (cell), renderer (renderer), initialized (false)
  {
    csTerrainBruteBlockCellRenderProperties* prop = (csTerrainBruteBlockCellRenderProperties*)
      cell->GetRenderProperties ();

    block_res_log2 = csLog2 (prop->GetBlockResolution ());

    lod_lcoeff = prop->GetLODLCoeff ();

    lod_distance = 200;

    baseContext.AttachNew (new csShaderVariableContext);

    csRef<csShaderVariable> lod_var = 
      new csShaderVariable (renderer->strings->Request ("texture lod distance"));
    lod_var->SetType (csShaderVariable::VECTOR3);
    lod_var->SetValue (csVector3 (lod_distance, lod_distance, lod_distance));
    baseContext->AddVariable (lod_var);
  }

  void SetupObject()
  {
    if (!initialized)
    {
      initialized = true;     

      rootblock.AttachNew (new csTerrBlock (this));
      csVector2 center = cell->GetPosition () + csVector2 (cell->GetSize ().x,
        cell->GetSize ().z)/2;
      rootblock->center = center;
      rootblock->size = csVector2(cell->GetSize ().x, cell->GetSize ().z);

      rootblock->left = rootblock->top = 0;
      rootblock->right = cell->GetGridWidth () - 1;
      rootblock->bottom = cell->GetGridHeight () - 1;

      // so, we're going to take block_res steps of size step to reach 
      // from left to right, that is
      // left + step * block_res = right
      // beware! block_res and right should be 2^n !
      rootblock->step = rootblock->right >> block_res_log2;

      rootblock->SetupMesh ();
    }
  }
};

csTerrBlock::csTerrBlock (csBruteBlockTerrainRenderData* data)
{
  parent = 0;
  child = 0;
  children[0] = children[1] = children[2] = children[3] = 0;
  neighbours[0] =  neighbours[1] = neighbours[2] = neighbours[3] = 0;

  built = false;

  bufferHolder.AttachNew (new csRenderBufferHolder);

  detach = false;

  rdata = data;
}

csTerrBlock::~csTerrBlock ()
{
}

void csTerrBlock::Detach ()
{
  detach = true;

  if (!IsLeaf ())
  {
    children[0]->Detach ();
    children[1]->Detach ();
    children[2]->Detach ();
    children[3]->Detach ();
    children[0] = 0;
    children[1] = 0;
    children[2] = 0;
    children[3] = 0;
  }

  if (neighbours[0] && ((child==0 || child==1) || !parent))
    neighbours[0]->ReplaceChildNeighbours(this, parent);

  if (neighbours[1] && ((child==1 || child==3) || !parent))
    neighbours[1]->ReplaceChildNeighbours(this, parent);

  if (neighbours[2] && ((child==0 || child==2) || !parent))
    neighbours[2]->ReplaceChildNeighbours(this, parent);

  if (neighbours[3] && ((child==2 || child==3) || !parent))
    neighbours[3]->ReplaceChildNeighbours(this, parent);
}

void csTerrBlock::ReplaceChildNeighbours(csTerrBlock *a, csTerrBlock *b)
{
  for(int i = 0; i < 4; ++i)
    if(neighbours[i] && neighbours[i]==a) neighbours[i]=b;

  if(!IsLeaf())
    for(int i = 0; i < 4; ++i)
      children[i]->ReplaceChildNeighbours(a,b);
}

void csTerrBlock::LoadData ()
{
  res = (1 << rdata->block_res_log2) + 1;

  mesh_vertices = 
    csRenderBuffer::CreateRenderBuffer (
    res*res, CS_BUF_STATIC, CS_BUFCOMP_FLOAT,
    3);

  mesh_normals = 
    csRenderBuffer::CreateRenderBuffer (
    res*res, CS_BUF_STATIC, CS_BUFCOMP_FLOAT,
    3);

  mesh_texcoords = 
    csRenderBuffer::CreateRenderBuffer (res*res,
    CS_BUF_STATIC, CS_BUFCOMP_FLOAT,
    2);

  bufferHolder->SetRenderBuffer (CS_BUFFER_POSITION, mesh_vertices);
  bufferHolder->SetRenderBuffer (CS_BUFFER_NORMAL, mesh_normals);
  bufferHolder->SetRenderBuffer (CS_BUFFER_TEXCOORD0, mesh_texcoords);

  const csVector2& pos = rdata->cell->GetPosition ();
  const csVector3& cell_size = rdata->cell->GetSize ();

  csLockedHeightData data = rdata->cell->GetHeightData ();

  if (!built)
    bbox.Empty ();

  {
    csRenderBufferLock<csVector3> vertex_data (mesh_vertices);
    csRenderBufferLock<csVector3> normal_data (mesh_normals);

    float min_x = center.x - size.x/2;
    float max_x = center.x + size.x/2;

    float min_z = center.y - size.y/2;
    float max_z = center.y + size.y/2;

    float x_offset = (max_x - min_x) / (float)(res - 1);
    float z_offset = (max_z - min_z) / (float)(res - 1);

    float c_z = min_z;

    float min_height = 1e+20f;
    float max_height = -1e+20f;

    for (int y = 0, grid_y = top; y < res; ++y, grid_y += step)
    {
      float c_x = min_x;
      float* row = data.data + data.pitch * grid_y + left;

      for (int x = 0; x < res; ++x, row += step)
      {
        float height = *row;
        *vertex_data++ = csVector3(c_x, height, c_z);
        csVector3 normal = rdata->cell->GetNormal (x*step,y*step).Unit (); 
        *normal_data++ = normal;       

        if (min_height > height) min_height = height;
        if (max_height < height) max_height = height;

        c_x += x_offset;
      }

      c_z += z_offset;
    }

    if (!built)
    {
      bbox.Set (min_x, min_height, min_z,
        max_x, max_height, max_z);
    }
  }  

  {
    csRenderBufferLock<csVector2> texcoord_data(mesh_texcoords);

    float min_u = (center.x - size.x/2 - pos.x) / cell_size.x;
    float max_u = (center.x + size.x/2 - pos.x) / cell_size.x;

    float min_v = (center.y - size.y/2 - pos.y) / cell_size.z;
    float max_v = (center.y + size.y/2 - pos.y) / cell_size.z;

    float u_offset = (max_u - min_u) / (float)(res - 1);
    float v_offset = (max_v - min_v) / (float)(res - 1);

    float v = min_v;

    for (int y = 0; y < res; ++y)
    {
      float u = min_u;

      for (int x = 0; x < res; ++x)
      {
        (*texcoord_data).x = u;
        (*texcoord_data).y = v;
        ++texcoord_data;

        u += u_offset;
      }

      v += v_offset;
    }
  }
}

void csTerrBlock::SetupMesh ()
{ 
  res = (1 << rdata->block_res_log2) + 1;

  LoadData ();

  built = true;
}

void csTerrBlock::Split ()
{
  int i;
  for (i=0; i<4; i++)
  {
    if (neighbours[i] && neighbours[i]->step>step && neighbours[i]->IsLeaf ())
      neighbours[i]->Split ();
  }

  int half_right = left + (right - left) / 2;
  int half_bottom = top + (bottom - top) / 2;

  children[0].AttachNew (new csTerrBlock (rdata));
  children[0]->parent = this;
  children[0]->size = size/2.0;
  children[0]->center = center + csVector2 (-size.x/4.0, -size.y/4.0);
  children[0]->child = 0;
  children[0]->step = step / 2;
  children[0]->left = left;
  children[0]->right = half_right;
  children[0]->top = top;
  children[0]->bottom = half_bottom;

  children[1].AttachNew (new csTerrBlock (rdata));
  children[1]->parent = this;
  children[1]->size = size/2.0;
  children[1]->center = center + csVector2 ( size.x/4.0, -size.y/4.0);
  children[1]->child = 1;
  children[1]->step = step / 2;
  children[1]->left = half_right;
  children[1]->right = right;
  children[1]->top = top;
  children[1]->bottom = half_bottom;

  children[2].AttachNew (new csTerrBlock (rdata));
  children[2]->parent = this;
  children[2]->size = size/2.0;
  children[2]->center = center + csVector2 (-size.x/4.0, size.y/4.0);
  children[2]->child = 2;
  children[2]->step = step / 2;
  children[2]->left = left;
  children[2]->right = half_right;
  children[2]->top = half_bottom;
  children[2]->bottom = bottom;

  children[3].AttachNew (new csTerrBlock (rdata));
  children[3]->parent = this;
  children[3]->size = size/2.0;
  children[3]->center = center + csVector2 ( size.x/4.0, size.y/4.0);
  children[3]->child = 3;
  children[3]->step = step / 2;
  children[3]->left = half_right;
  children[3]->right = right;
  children[3]->top = half_bottom;
  children[3]->bottom = bottom;

  if (neighbours[0])
  {
    if (!neighbours[0]->IsLeaf ())
    {
      children[0]->neighbours[0] = neighbours[0]->children[2];
      children[1]->neighbours[0] = neighbours[0]->children[3];
      children[0]->neighbours[0]->neighbours[3] = children[0];
      children[1]->neighbours[0]->neighbours[3] = children[1];
    }
    else
    {
      children[0]->neighbours[0] = neighbours[0];
      children[1]->neighbours[0] = neighbours[0];
      neighbours[0]->neighbours[3] = this;
    }
  }
  if (neighbours[1])
  {
    if (!neighbours[1]->IsLeaf ())
    {
      children[1]->neighbours[1] = neighbours[1]->children[0];
      children[3]->neighbours[1] = neighbours[1]->children[2];
      children[1]->neighbours[1]->neighbours[2] = children[1];
      children[3]->neighbours[1]->neighbours[2] = children[3];
    }
    else
    {
      children[1]->neighbours[1] = neighbours[1];
      children[3]->neighbours[1] = neighbours[1];
      neighbours[1]->neighbours[2] = this;
    }
  }
  if (neighbours[2])
  {
    if (!neighbours[2]->IsLeaf ())
    {
      children[0]->neighbours[2] = neighbours[2]->children[1];
      children[2]->neighbours[2] = neighbours[2]->children[3];
      children[0]->neighbours[2]->neighbours[1] = children[0];
      children[2]->neighbours[2]->neighbours[1] = children[2];
    }
    else
    {
      children[0]->neighbours[2] = neighbours[2];
      children[2]->neighbours[2] = neighbours[2];
      neighbours[2]->neighbours[1] = this;
    }
  }
  if (neighbours[3])
  {
    if (!neighbours[3]->IsLeaf ())
    {
      children[2]->neighbours[3] = neighbours[3]->children[0];
      children[3]->neighbours[3] = neighbours[3]->children[1];
      children[2]->neighbours[3]->neighbours[0] = children[2];
      children[3]->neighbours[3]->neighbours[0] = children[3];
    }
    else
    {
      children[2]->neighbours[3] = neighbours[3];
      children[3]->neighbours[3] = neighbours[3];
      neighbours[3]->neighbours[0] = this;
    }
  }
  children[0]->neighbours[1] = children[1];
  children[0]->neighbours[3] = children[2];

  children[1]->neighbours[2] = children[0];
  children[1]->neighbours[3] = children[3];

  children[2]->neighbours[0] = children[0];
  children[2]->neighbours[1] = children[3];

  children[3]->neighbours[0] = children[1];
  children[3]->neighbours[2] = children[2];

  children[0]->SetupMesh ();
  children[1]->SetupMesh ();
  children[2]->SetupMesh ();
  children[3]->SetupMesh ();
}

void csTerrBlock::Merge ()
{
  if (IsLeaf ())
    return;

  if(!children[0]->IsLeaf() || !children[1]->IsLeaf()
    || !children[2]->IsLeaf() || !children[3]->IsLeaf())
    return;
  
  if (neighbours[0] && 
      (!neighbours[0]->IsLeaf () && 
       (!neighbours[0]->children[2]->IsLeaf () ||
        !neighbours[0]->children[3]->IsLeaf ())))
    return;
  if (neighbours[1] && 
      (!neighbours[1]->IsLeaf () && 
       (!neighbours[1]->children[0]->IsLeaf () ||
        !neighbours[1]->children[2]->IsLeaf ())))
    return;
  if (neighbours[2] && 
      (!neighbours[2]->IsLeaf () && 
       (!neighbours[2]->children[1]->IsLeaf () ||
        !neighbours[2]->children[3]->IsLeaf ())))
    return;
  if (neighbours[3] && 
      (!neighbours[3]->IsLeaf () && 
       (!neighbours[3]->children[0]->IsLeaf () ||
        !neighbours[3]->children[1]->IsLeaf ())))
    return;

  children[0]->Detach ();
  children[1]->Detach ();
  children[2]->Detach ();
  children[3]->Detach ();
  children[0] = children[1] = children[2] = children[3] = 0;
}

void csTerrBlock::CalcLOD ()
{
  int res = (1 << rdata->block_res_log2);

  csVector3 cam = rdata->tr_o2c.GetOrigin ();
  csBox3 cambox (bbox.Min ()-cam, bbox.Max ()-cam);

  csVector3 toBB = (bbox.GetCenter () - cam).Unit ();
  csVector3 bbsize = bbox.GetSize ();

  float projArea = 
    (bbsize.x*bbsize.y) * fabsf (toBB.z) +
    (bbsize.x*bbsize.z) * fabsf (toBB.y) + 
    (bbsize.y*bbsize.z) * fabsf (toBB.x);

  const float resFactor = rdata->lod_lcoeff / float (res);
  float targetDistSq = projArea*resFactor*resFactor;

  if (cambox.SquaredOriginDist ()<targetDistSq && 
    step > 1)
  {
    if (IsLeaf ())
      Split ();
  }
  else
  {
    if (!IsLeaf ())
      Merge ();
  }
  if (!IsLeaf ())
    for (int i=0; i<4; i++)
      children[i]->CalcLOD ();
}

void csTerrBlock::DrawTest (iGraphics3D* g3d,
                            iRenderView *rview, uint32 frustum_mask,
                            csReversibleTransform &transform,
                            iMovable *movable,
                            csDirtyAccessArray<csRenderMesh*>& meshes)
{
  int clip_portal, clip_plane, clip_z_plane;
  if (!rview->ClipBBox (rdata->planes, frustum_mask,
    bbox, clip_portal, clip_plane, clip_z_plane))
    return;

  if (!IsLeaf () && children[0]->built &&
    children[1]->built &&
    children[2]->built &&
    children[3]->built)
  {
    children[0]->DrawTest (g3d, rview, frustum_mask, transform, movable,
      meshes);
    children[1]->DrawTest (g3d, rview, frustum_mask, transform, movable,
      meshes);
    children[2]->DrawTest (g3d, rview, frustum_mask, transform, movable,
      meshes);
    children[3]->DrawTest (g3d, rview, frustum_mask, transform, movable,
      meshes);
    return;
  }

  if (!built)
    return;

  //csVector3 cam = rview->GetCamera ()->GetTransform ().GetOrigin ();
  csVector3 cam = transform.GetOrigin ();

  // left & top sides are covered with additional triangles
  /*int idx = ((!neighbours[0] || neighbours[0]->step>step)?1:0)+
    (((!neighbours[1] || neighbours[1]->step>step)?1:0)<<1)+
    (((neighbours[2] && neighbours[2]->step>step)?1:0)<<2)+
    (((neighbours[3] && neighbours[3]->step>step)?1:0)<<3);*/
  int idx = ((neighbours[0] && neighbours[0]->step>step)?1:0)+
    (((neighbours[1] && neighbours[1]->step>step)?1:0)<<1)+
    (((neighbours[2] && neighbours[2]->step>step)?1:0)<<2)+
    (((neighbours[3] && neighbours[3]->step>step)?1:0)<<3);

  int numIndices;
  bufferHolder->SetRenderBuffer (CS_BUFFER_INDEX, 
    rdata->renderer->GetIndexBuffer (rdata->block_res_log2, idx, numIndices));


  const csReversibleTransform o2wt = movable->GetFullTransform ();
  const csVector3& wo = o2wt.GetOrigin ();
  bool isMirrored = rview->GetCamera()->IsMirrored();

  for (int j = -1; j < (int)rdata->material_palette->GetSize (); ++j)
  {
    bool created;

    iMaterialWrapper* mat = 0;
    iShaderVariableContext* svContext;

    if (j < 0)
    {
      mat = rdata->cell->GetBaseMaterial ();
      svContext = rdata->baseContext;
    }
    else
    {
      mat = rdata->material_palette->Get (j);
      svContext = rdata->sv_context[j];
    }

    if (!mat)
      continue;

    csRenderMesh*& mesh = rdata->rm_holder->GetUnusedMesh (created,
      rview->GetCurrentFrameNumber ());

    mesh->meshtype = CS_MESHTYPE_TRIANGLESTRIP;

    mesh->buffers = 0;
    mesh->buffers = bufferHolder;
    mesh->clip_portal = clip_portal;
    mesh->clip_plane = clip_plane;
    mesh->clip_z_plane = clip_z_plane;
    mesh->indexstart = 0;
    mesh->indexend = numIndices;
    mesh->material = mat;
    mesh->variablecontext = svContext;

    mesh->object2world = o2wt;
    mesh->worldspace_origin = wo;
    mesh->do_mirror = isMirrored;

    meshes.Push (mesh);
  }
}

csTerrainBruteBlockCellRenderProperties::csTerrainBruteBlockCellRenderProperties()
  : scfImplementationType (this), visible (true), block_res (16), lod_lcoeff (16)
{

}

csTerrainBruteBlockCellRenderProperties::csTerrainBruteBlockCellRenderProperties(
  csTerrainBruteBlockCellRenderProperties& other)
  : scfImplementationType (this), visible (other.visible), block_res (other.block_res),
  lod_lcoeff (other.lod_lcoeff)
{

}

csTerrainBruteBlockCellRenderProperties::~csTerrainBruteBlockCellRenderProperties ()
{
}

void csTerrainBruteBlockCellRenderProperties::SetParameter (const char* name,
                                                        const char* value)
{
  if (strcmp (name, "visible") == 0)
    SetVisible (strcmp(value, "true") == 0);
  else if (strcmp (name, "block resolution") == 0)
    SetBlockResolution (atoi (value));
  else if (strcmp (name, "lod lcoeff") == 0)
    SetLODLCoeff (atof (value));
}

csPtr<iTerrainCellRenderProperties> csTerrainBruteBlockCellRenderProperties::Clone ()
{
  return csPtr<iTerrainCellRenderProperties> (new csTerrainBruteBlockCellRenderProperties (*this));
}


csTerrainBruteBlockRenderer::csTerrainBruteBlockRenderer (iBase* parent)
: scfImplementationType (this, parent)
{
}


csTerrainBruteBlockRenderer::~csTerrainBruteBlockRenderer ()
{
}

csPtr<iTerrainCellRenderProperties> csTerrainBruteBlockRenderer::CreateProperties()
{
  return csPtr<iTerrainCellRenderProperties> (new csTerrainBruteBlockCellRenderProperties);
}

void csTerrainBruteBlockRenderer::ConnectTerrain (iTerrainSystem* system)
{
  system->AddCellHeightUpdateListener (this);
}

void csTerrainBruteBlockRenderer::DisconnectTerrain (iTerrainSystem* system)
{
  system->RemoveCellHeightUpdateListener (this);
}


csRenderMesh** csTerrainBruteBlockRenderer::GetRenderMeshes (int& n, iRenderView* rview,
                                                             iMovable* movable, uint32 frustum_mask,
                                                             const csArray<iTerrainCell*> cells)
{
  meshes.Empty ();

  bool do_mirroring = rview->GetCamera ()->IsMirrored ();

  const csReversibleTransform& o2wt = movable->GetFullTransform ();
  const csVector3& wo = o2wt.GetOrigin ();

  iCamera* cam = rview->GetCamera ();
  csReversibleTransform tr_o2c = cam->GetTransform ();
  if (!movable->IsFullTransformIdentity ())
    tr_o2c /= movable->GetFullTransform ();

  csPlane3 planes[10];
  rview->SetupClipPlanes (tr_o2c, planes, frustum_mask);

  for (size_t i = 0; i < cells.GetSize (); ++i)
  {
    csBruteBlockTerrainRenderData* rdata = (csBruteBlockTerrainRenderData*)
      cells[i]->GetRenderData ();

    if (!rdata) continue;

    rdata->material_palette = &material_palette;
    rdata->rm_holder = &rm_holder;
    rdata->planes = planes;

    rdata->rootblock->CalcLOD ();

    rdata->tr_o2c = tr_o2c;

    rdata->rootblock->DrawTest (g3d, rview, frustum_mask, tr_o2c, movable, meshes);
  }

  n = (int)meshes.GetSize ();

  return meshes.GetArray ();
}

void csTerrainBruteBlockRenderer::OnMaterialPaletteUpdate (
  const csRefArray<iMaterialWrapper>& material_palette)
{
  this->material_palette = material_palette;
}

void csTerrainBruteBlockRenderer::OnHeightUpdate (iTerrainCell* cell,
                                                  const csRect& rectangle)
{
  csRef<csBruteBlockTerrainRenderData> rdata = SetupCellRenderData (cell);

  //@@TODO! Fix "incremental" updates
}

void csTerrainBruteBlockRenderer::OnMaterialMaskUpdate (iTerrainCell* cell,
  size_t material, const csRect& rectangle, const unsigned char* data,
  size_t pitch)
{
  csRef<csBruteBlockTerrainRenderData> rdata = SetupCellRenderData (cell);

  if (rdata->sv_context.GetSize () <= material)
  {
    rdata->sv_context.SetSize (material + 1);
  }

  if (!rdata->sv_context[material])
  {
    rdata->sv_context[material].AttachNew (new csShaderVariableContext);

    float lod_distance = rdata->lod_distance;
    csRef<csShaderVariable> lod_var = 
      new csShaderVariable (strings->Request ("texture lod distance"));
    lod_var->SetType (csShaderVariable::VECTOR3);
    lod_var->SetValue (csVector3 (lod_distance, lod_distance, lod_distance));
    rdata->sv_context[material]->AddVariable (lod_var);
  }

  if (rdata->alpha_map.GetSize () <= material)
  {
    rdata->alpha_map.SetSize (material + 1);
  }

  if (!rdata->alpha_map[material])
  {
    csRef<iImage> image;
    image.AttachNew (new csImageMemory (cell->GetMaterialMapWidth (),
      cell->GetMaterialMapHeight (), CS_IMGFMT_TRUECOLOR));

    rdata->alpha_map[material] = g3d->GetTextureManager ()->RegisterTexture
      (image, CS_TEXTURE_2D | CS_TEXTURE_3D | CS_TEXTURE_CLAMP);

    csRef<csShaderVariable> var;
    var.AttachNew (new csShaderVariable(strings->Request ("splat alpha map")));
    var->SetType (csShaderVariable::TEXTURE);
    var->SetValue (rdata->alpha_map[material]);
    rdata->sv_context[material]->AddVariable (var);
  }

  csDirtyAccessArray<csRGBpixel> image_data;
  image_data.SetSize (rectangle.Width () * rectangle.Height ());

  csRGBpixel* dst_data = image_data.GetArray ();

  for (int y = 0; y < rectangle.Width (); ++y)
  {
    const unsigned char* src_data = data + y * pitch;

    for (int x = 0; x < rectangle.Height (); ++x, ++src_data)
    {
      (*dst_data++).Set (*src_data, *src_data, *src_data, *src_data);
    }
  }

  rdata->alpha_map[material]->Blit (rectangle.xmin, rectangle.ymin,
    rectangle.Width (), rectangle.Height (), (unsigned char*)
    image_data.GetArray (), iTextureHandle::RGBA8888);
}

bool csTerrainBruteBlockRenderer::Initialize (iObjectRegistry* object_reg)
{
  this->object_reg = object_reg;

  g3d = csQueryRegistry<iGraphics3D> (object_reg);

  strings = csQueryRegistryTagInterface<iStringSet> (object_reg,
    "crystalspace.shared.stringset");

  return true;
}

csRef<csBruteBlockTerrainRenderData> 
csTerrainBruteBlockRenderer::SetupCellRenderData (iTerrainCell* cell)
{
  csRef<csBruteBlockTerrainRenderData> rdata = 
    (csBruteBlockTerrainRenderData*)cell->GetRenderData ();

  if (!rdata)
  {
    rdata.AttachNew (new csBruteBlockTerrainRenderData(cell, this));
    cell->SetRenderData (rdata);
    rdata->SetupObject ();
  }

  return rdata;
}

template<typename T>
static void FillEdge (bool halfres, int res, T* indices, int &indexcount,
                      int offs, int xadd, int zadd)
{
  int x;
  // This triangulation scheme could probably be better.
  for (x=0; x<res; x+=2)
  {
    if (x>0)
    {
      indices[indexcount++] = offs+x*xadd+zadd;
      indices[indexcount++] = offs+x*xadd;
    }
    else
    {
      indices[indexcount++] = offs+x*xadd;
      indices[indexcount++] = offs+x*xadd;
      indices[indexcount++] = offs+x*xadd;
    }

    indices[indexcount++] = offs+(x+1)*xadd+zadd;
    if (!halfres)
      indices[indexcount++] = offs+(x+1)*xadd;
    else
      indices[indexcount++] = offs+x*xadd;

    if (x<res-2)
    {
      indices[indexcount++] = offs+(x+2)*xadd+zadd;
      indices[indexcount++] = offs+(x+2)*xadd;
    }
    else
    {
      indices[indexcount++] = offs+(x+2)*xadd;
      indices[indexcount++] = offs+(x+2)*xadd;
      indices[indexcount++] = offs+(x+2)*xadd;
    }
  }
}

template<typename T>
static void FillBlock (T* indices, int &indexcount, int block_res, int t, int r, int l, int b)
{
  indexcount = 0;
  int x, z;
  for (z=1; z<(block_res-1); z++)
  {
    indices[indexcount++] = 1+z*(block_res+1);
    indices[indexcount++] = 1+z*(block_res+1);
    for (x=1; x<(block_res); x++)
    { 
      indices[indexcount++] = x+(z+0)*(block_res+1);
      indices[indexcount++] = x+(z+1)*(block_res+1);
    }
    indices[indexcount++] = x-1+(z+1)*(block_res+1);
    indices[indexcount++] = x-1+(z+1)*(block_res+1);
  }

  FillEdge (t==1,
    block_res, indices, indexcount, 
    0, 1, (block_res+1));

  FillEdge (r==1,
    block_res, indices, indexcount,
    block_res, (block_res+1), -1);

  FillEdge (l==1,
    block_res, indices, indexcount, 
    block_res*(block_res+1), -(block_res+1), 1);

  FillEdge (b==1, 
    block_res, indices, indexcount,
    block_res*(block_res+1)+block_res, -1, -(block_res+1));

}

iRenderBuffer* csTerrainBruteBlockRenderer::GetIndexBuffer (int block_res_log2, int index, int& numIndices)
{
  csRef<IndexBufferSet> set;

  if (size_t (block_res_log2) >= indexBufferList.GetSize () ||
    indexBufferList[block_res_log2] == 0)
  {
    // Add a new one
    indexBufferList.SetSize (block_res_log2 + 1);
    set.AttachNew (new IndexBufferSet);
    indexBufferList.Put (block_res_log2, set);

    int block_res = 1 << block_res_log2;

    int* numindices = set->numindices;

    int maxIndex = (block_res+1) * (block_res+1) - 1;

    // Reset
    for (int t=0; t<=1; t++)
    {
      for (int r=0; r<=1; r++)
      {
        for (int l=0; l<=1; l++)
        {
          for (int b=0; b<=1; b++)
          {
            int idx = t+(r<<1)+(l<<2)+(b<<3);

            if (maxIndex > 0xFFFF)
            {
              set->mesh_indices[idx] = 
                csRenderBuffer::CreateIndexRenderBuffer (
                block_res*block_res*2*3, 
                CS_BUF_STATIC, CS_BUFCOMP_UNSIGNED_INT,
                0, maxIndex);

              uint32 *indices = 
                (uint32*)set->mesh_indices[idx]->Lock (CS_BUF_LOCK_NORMAL);

              FillBlock (indices, numindices[idx], block_res, t, r, l ,b);

              set->mesh_indices[idx]->Release ();
            }
            else
            {
              set->mesh_indices[idx] = 
                csRenderBuffer::CreateIndexRenderBuffer (
                block_res*block_res*2*3, 
                CS_BUF_STATIC, CS_BUFCOMP_UNSIGNED_SHORT,
                0, maxIndex);

              uint16 *indices = 
                (uint16*)set->mesh_indices[idx]->Lock (CS_BUF_LOCK_NORMAL);

              FillBlock (indices, numindices[idx], block_res, t, r, l , b);
              
              set->mesh_indices[idx]->Release ();
            }
          }
        }
      }
    }
  }
  else
  {
    set = indexBufferList[block_res_log2];
  }

  numIndices = set->numindices[index];
  return set->mesh_indices[index];
}

}
CS_PLUGIN_NAMESPACE_END(Terrain2)
