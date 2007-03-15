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

#include "csgeom/csrect.h"
#include "csgfx/imagememory.h"
#include "csgfx/renderbuffer.h"
#include "csgfx/rgbpixel.h"
#include "csgfx/shadervar.h"
#include "csgfx/shadervarcontext.h"
#include "cstool/rbuflock.h"
#include "csutil/objreg.h"
#include "csutil/refarr.h"
#include "iengine/rview.h"
#include "iengine/camera.h"
#include "iengine/movable.h"
#include "iengine/material.h"
#include "imesh/terrain2.h"
#include "iutil/strset.h"
#include "ivideo/rendermesh.h"
#include "ivideo/txtmgr.h"

#include "simplerenderer.h"

CS_PLUGIN_NAMESPACE_BEGIN(Terrain2)
{

SCF_IMPLEMENT_FACTORY (csTerrainSimpleRenderer)

struct csSimpleTerrainRenderData: public csRefCount
{
  csRef<iRenderBuffer> vb_pos;
  csRef<iRenderBuffer> vb_texcoord;
  csRef<iRenderBuffer> ib;
  csRef<iRenderBuffer> vb_normals;

  csRef<iMaterialWrapper> material;
  
  csArray<csRef<csShaderVariableContext> > sv_context;
  
  csArray<csRef<iTextureHandle> > alpha_map;

  unsigned int primitive_count;
};

csTerrainSimpleCellRenderProperties::csTerrainSimpleCellRenderProperties ()
  : scfImplementationType (this), visible (true)
{
}

csTerrainSimpleCellRenderProperties::csTerrainSimpleCellRenderProperties (
  csTerrainSimpleCellRenderProperties& other)
  : scfImplementationType (this), visible (other.visible)
{

}

csTerrainSimpleCellRenderProperties::~csTerrainSimpleCellRenderProperties ()
{
}

bool csTerrainSimpleCellRenderProperties::GetVisible() const
{
  return visible;
}

void csTerrainSimpleCellRenderProperties::SetVisible(bool value)
{
  visible = value;
}

void csTerrainSimpleCellRenderProperties::SetParameter (const char* name,
  const char* value)
{
  if (!strcmp (name, "visible"))
    visible = !strcmp(value, "true");
}

csPtr<iTerrainCellRenderProperties> csTerrainSimpleCellRenderProperties::Clone ()
{
  return csPtr<iTerrainCellRenderProperties> (
    new csTerrainSimpleCellRenderProperties (*this));
}

csTerrainSimpleRenderer::csTerrainSimpleRenderer (iBase* parent)
  : scfImplementationType (this, parent)
{
}


csTerrainSimpleRenderer::~csTerrainSimpleRenderer ()
{
}

csPtr<iTerrainCellRenderProperties> csTerrainSimpleRenderer::CreateProperties()
{
  return csPtr<iTerrainCellRenderProperties> (
    new csTerrainSimpleCellRenderProperties);
}

void csTerrainSimpleRenderer::ConnectTerrain (iTerrainSystem* system)
{
  system->AddCellHeightUpdateListener (this);
}

void csTerrainSimpleRenderer::DisconnectTerrain (iTerrainSystem* system)
{
  system->RemoveCellHeightUpdateListener (this);
}

csRenderMesh** csTerrainSimpleRenderer::GetRenderMeshes (int& n, iRenderView* rview,
  iMovable* movable, uint32 frustum_mask, const csArray<iTerrainCell*> cells)
{
  meshes.Empty ();
  
  bool do_mirroring = rview->GetCamera ()->IsMirrored ();
  
  const csReversibleTransform& o2wt = movable->GetFullTransform ();
  const csVector3& wo = o2wt.GetOrigin ();
  
  for (int i = 0; i < cells.GetSize (); ++i)
  {
    csSimpleTerrainRenderData* rdata = (csSimpleTerrainRenderData*)
      cells[i]->GetRenderData ();

    if (!rdata) continue;

    for (unsigned int j = 0; j < material_palette.GetSize (); ++j)
    {
      bool created;
    
      csRenderMesh*& mesh = rm_holder.GetUnusedMesh (created,
        rview->GetCurrentFrameNumber ());

    
      mesh->meshtype = CS_MESHTYPE_TRIANGLES;
    
      mesh->material = material_palette[j];
      
      mesh->indexstart = 0;
      mesh->indexend = rdata->primitive_count * 3;
    
      mesh->do_mirror = do_mirroring;
    
      mesh->variablecontext = rdata->sv_context[j];
    
      mesh->object2world = o2wt;
      mesh->worldspace_origin = wo;
      
      if (created) 
        mesh->buffers.AttachNew (new csRenderBufferHolder);
    
      mesh->buffers->SetRenderBuffer (CS_BUFFER_POSITION, rdata->vb_pos);
      mesh->buffers->SetRenderBuffer (CS_BUFFER_TEXCOORD0, rdata->vb_texcoord);
      mesh->buffers->SetRenderBuffer (CS_BUFFER_NORMAL, rdata->vb_normals);
      mesh->buffers->SetRenderBuffer (CS_BUFFER_INDEX, rdata->ib);
      
      meshes.Push (mesh);
    }
  }
  
  n = (int)meshes.GetSize ();
  
  return meshes.GetArray ();
}

void csTerrainSimpleRenderer::OnMaterialPaletteUpdate (
const csRefArray<iMaterialWrapper>& material_palette)
{
  this->material_palette = material_palette;
}

void csTerrainSimpleRenderer::OnHeightUpdate (iTerrainCell* cell,
  const csRect& rectangle)
{
  csRef<csSimpleTerrainRenderData> rdata = (csSimpleTerrainRenderData*)
    cell->GetRenderData ();

  int grid_width = cell->GetGridWidth ();
  int grid_height = cell->GetGridHeight ();
  
  int mm_width = cell->GetMaterialMapWidth ();
  int mm_height = cell->GetMaterialMapHeight ();

  if (!rdata)
  {
    rdata.AttachNew (new csSimpleTerrainRenderData);

    cell->SetRenderData (rdata);

    rdata->primitive_count = (grid_width-1)*(grid_height-1)*2;

    rdata->vb_pos = csRenderBuffer::CreateRenderBuffer (
      grid_width * grid_height, CS_BUF_STATIC, CS_BUFCOMP_FLOAT, 3);

    rdata->vb_normals = csRenderBuffer::CreateRenderBuffer (
      grid_width * grid_height, CS_BUF_STATIC, CS_BUFCOMP_FLOAT, 3);
    
    rdata->vb_texcoord = csRenderBuffer::CreateRenderBuffer (
      grid_width * grid_height, CS_BUF_STATIC, CS_BUFCOMP_FLOAT, 2);

    rdata->ib = csRenderBuffer::CreateIndexRenderBuffer (
      rdata->primitive_count*3, CS_BUF_STATIC, CS_BUFCOMP_UNSIGNED_INT, 0,
      grid_width * grid_height);

    // fill ib
    csRenderBufferLock<unsigned int> ilocker(rdata->ib);
    unsigned int* iptr = ilocker;

    int y_mul_grid_width_plus_x = 0;

    for (int y = 0; y < grid_height - 1; ++y)
	{
      for (int x = 0; x < grid_width - 1; ++x, ++y_mul_grid_width_plus_x)
      {
        // tl - tr
        //  | / |
        // bl - br

        int tl = y_mul_grid_width_plus_x;
        int tr = y_mul_grid_width_plus_x + 1;
        int bl = y_mul_grid_width_plus_x + grid_width;
        int br = y_mul_grid_width_plus_x + grid_width + 1;

        *iptr++ = tl;
        *iptr++ = bl;
        *iptr++ = tr;

        *iptr++ = tr;
        *iptr++ = bl;
        *iptr++ = br;
      }

      ++y_mul_grid_width_plus_x;
    }
    rdata->ib->Release ();

    // fill tex coords
    csRenderBufferLock<float> vlocker(rdata->vb_texcoord);
    float* vptr = vlocker;

    float u_offset = 1.0f / (grid_width - 1);
    float v_offset = 1.0f / (grid_height - 1);
    
    float u_correct = 0.5f / mm_width;
    float v_correct = 0.5f / mm_height;
    
    for (int y = 0; y < grid_height; ++y)
      for (int x = 0; x < grid_width; ++x)
      {
        *vptr++ = x * u_offset + u_correct;
        *vptr++ = y * v_offset + v_correct;
      }

    rdata->vb_texcoord->Release();
  }

  csRenderBufferLock<float> vlocker(rdata->vb_pos);
  csRenderBufferLock<csVector3> nlocker(rdata->vb_normals);
  float* vptr = vlocker;

  float offset_x = cell->GetPosition ().x;
  float offset_y = cell->GetPosition ().y;

  float scale_x = cell->GetSize ().x / (grid_width - 1);
  float scale_y = cell->GetSize ().y / (grid_height - 1);

  csLockedHeightData data = cell->LockHeightData (rectangle);

  for (int y = rectangle.ymin; y < rectangle.ymax; ++y)
  {
    const float* row = data.data + y * data.pitch + rectangle.xmin;

    for (int x = rectangle.xmin; x < rectangle.xmax; ++x)
    {
      csVector3 v (x * scale_x + offset_x, *row++,y * scale_y + offset_y);
      *vptr++ = v.x;
      *vptr++ = v.y;
      *vptr++ = v.z;
      *nlocker++ = cell->GetNormal (csVector2 (v.x, v.z));
    }
  }
}

void csTerrainSimpleRenderer::OnMaterialMaskUpdate (iTerrainCell* cell,
  size_t material, const csRect& rectangle, const unsigned char* data,
  size_t pitch)
{
  csRef<csSimpleTerrainRenderData> rdata = (csSimpleTerrainRenderData*)
  cell->GetRenderData ();

  if (!rdata)
  {
    rdata.AttachNew (new csSimpleTerrainRenderData);

    cell->SetRenderData (rdata);
  }

  if (rdata->sv_context.GetSize () <= material)
  {
    rdata->sv_context.SetSize (material + 1);
  }
  
  if (!rdata->sv_context[material])
  {
    rdata->sv_context[material].AttachNew (new csShaderVariableContext);
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


bool csTerrainSimpleRenderer::Initialize (iObjectRegistry* object_reg)
{
  this->object_reg = object_reg;
   
  g3d = csQueryRegistry<iGraphics3D> (object_reg);
  
  strings = csQueryRegistryTagInterface<iStringSet> (object_reg,
    "crystalspace.shared.stringset");
    
  return true;
}

}
CS_PLUGIN_NAMESPACE_END(Terrain2)
