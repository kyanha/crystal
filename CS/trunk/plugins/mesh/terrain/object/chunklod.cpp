/*
    Copyright (C) 2003 by Jorrit Tyberghein, Daniel Duhprey

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
#include "qsqrt.h"

#include "iutil/cache.h"
#include "iutil/object.h"
#include "iutil/objreg.h"
#include "iutil/vfs.h"

#include "imesh/terrain.h"

#include "iengine/rview.h"
#include "iengine/camera.h"
#include "iengine/movable.h"
#include "iengine/mesh.h"
#include "iengine/sector.h"

#include "ivideo/graph3d.h"
#include "ivideo/rndbuf.h"
#include "ivideo/txtmgr.h"
#include "ivideo/material.h"
#include "ivideo/texture.h"

#include "igraphic/image.h"
#include "ivaria/reporter.h"

#include "csgfx/rgbpixel.h"
#include "csgfx/memimage.h"

#include "csutil/csendian.h"
#include "csutil/csmd5.h"
#include "csutil/memfile.h"
#include "csutil/util.h"

#include "cstool/rbuflock.h"

#include "chunklod.h"
#include <limits.h>

#define MIN_TERRAIN 129

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE (csChunkLodTerrainType)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectType)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csChunkLodTerrainType::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csChunkLodTerrainType)

csChunkLodTerrainType::csChunkLodTerrainType (iBase* p) : parent(p)
{
  SCF_CONSTRUCT_IBASE (parent)
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent)
}

csChunkLodTerrainType::~csChunkLodTerrainType ()
{
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiComponent)
  SCF_DESTRUCT_IBASE ()
}

csPtr<iMeshObjectFactory> csChunkLodTerrainType::NewFactory ()
{
  return csPtr<iMeshObjectFactory> 
	(new csChunkLodTerrainFactory (this, object_reg));
}

SCF_IMPLEMENT_IBASE (csChunkLodTerrainFactory)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectFactory)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObjectModel)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iTerrainFactoryState)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csChunkLodTerrainFactory::eiObjectModel)
  SCF_IMPLEMENTS_INTERFACE (iObjectModel)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csChunkLodTerrainFactory::eiTerrainFactoryState)
  SCF_IMPLEMENTS_INTERFACE (iTerrainFactoryState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csChunkLodTerrainFactory::csChunkLodTerrainFactory (csChunkLodTerrainType* p, 
	iObjectRegistry* objreg) : parent (p), object_reg (objreg)
{
  SCF_CONSTRUCT_IBASE (p)
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObjectModel)
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiTerrainFactoryState)

  root = 0;
  hm_x = 0; hm_y = 0;

  r3d = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
  shmgr = CS_QUERY_REGISTRY (object_reg, iShaderManager);
  light_mgr = CS_QUERY_REGISTRY (object_reg, iLightManager);
  engine = CS_QUERY_REGISTRY (object_reg, iEngine);

  csRef<iStringSet> strings = 
	CS_QUERY_REGISTRY_TAG_INTERFACE (object_reg,
	"crystalspace.shared.stringset", iStringSet);
  vertex_name = strings->Request ("vertices");
  compressed_vertex_name = strings->Request ("compressed vertices");
  texcors_name = strings->Request ("texture coordinates");
  compressed_texcors_name = strings->Request ("compressed texture coordinates");
  texcoords_norm_name = strings->Request ("texture coordinates normalized");
  normal_name = strings->Request ("normals");
  compressed_normal_name = strings->Request ("compressed normals");
  tangent_name = strings->Request ("tangents");
  compressed_tangent_name = strings->Request ("compressed tangents");
  binormal_name = strings->Request ("binormals");
  compressed_binormal_name = strings->Request ("compressed binormals");
  color_name = strings->Request ("colors");
  compressed_color_name = strings->Request ("compressed colors");
  index_name = strings->Request ("indices");
}

csChunkLodTerrainFactory::~csChunkLodTerrainFactory ()
{
  delete root;

  SCF_DESTRUCT_EMBEDDED_IBASE (scfiObjectModel)
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiTerrainFactoryState)
  SCF_DESTRUCT_IBASE ()
}

csPtr<iMeshObject> csChunkLodTerrainFactory::NewInstance ()
{
  return csPtr<iMeshObject>(new csChunkLodTerrainObject (this));
}

void csChunkLodTerrainFactory::GetObjectBoundingBox (csBox3& bbox, int type)
{
  bbox = root->BBox ();
}

void csChunkLodTerrainFactory::GetRadius (csVector3& rad, csVector3& c)
{
  const csBox3& bbox = root->BBox ();
  c = bbox.GetCenter ();
  rad = (bbox.Max () - bbox.Min ()) * 0.5f;
}

void csChunkLodTerrainFactory::SetScale (const csVector3& s)
{
  scale = s;
}

void csChunkLodTerrainFactory::ComputeError (int i, int j, int di, int dj,
	int n, int w)
{
  Data *b = &datamap[i + j * w];
  Data *l = &datamap[i - di + (j - dj) * w];
  Data *r = &datamap[i + di + (j + dj) * w];
  b->error = (float)fabs(b->pos.y - (l->pos.y + r->pos.y) / 2.0);
  if (n)
  {
    dj = (di + dj) / 2;
    di -= dj;
    for (int k = 0; k < 4; k ++)
    {
      if ((i > 0 || di >= 0) && (i < (w-1) || di <= 0) &&
          (j > 0 || dj >= 0) && (j < (w-1) || dj <= 0))
      {
        Data *cp = &datamap[i + di + (j + dj) * w];
        b->error = (b->error > cp->error) ? b->error : cp->error;
      }
      dj += di;
      di -= dj;
      dj += di;
    }
  }
}

bool csChunkLodTerrainFactory::SetHeightMap (const csArray<float>& data,
	int w, int h)
{
  CS_ASSERT (w == h);
  CS_ASSERT (w >= MIN_TERRAIN);
  datamap.SetLength (w * h);

  hm_x = w; hm_y = h;
  int i, j;
  for (j = 0; j < h; j ++)
  {
    for (i = 0; i < w; i ++)
    {
      int pos = i + j * w;

      datamap[pos].pos.x = (i - (w>>1)) * scale.x;
      datamap[pos].pos.y = data[pos] * scale.y;
      datamap[pos].pos.z = ((h>>1) - j) * scale.z;

      float up = (j-1 < 0) ? data[pos] : data[pos-w];
      float dn = (j+1 >= w) ? data[pos] : data[pos+w];
      float lt = (i-1 < 0) ? data[pos] : data[pos-1];
      float rt = (i+1 >= h) ? data[pos] : data[pos+1];

      datamap[pos].norm = csVector3 (lt - rt,
      	2.0/(float)w + 2.0/(float)h, dn - up);
      datamap[pos].norm.Normalize ();
      datamap[pos].tan = csVector3 (0,0,1) % datamap[pos].norm;
      datamap[pos].tan.Normalize ();
      datamap[pos].bin = datamap[pos].norm % datamap[pos].tan;
      datamap[pos].bin.Normalize ();

      datamap[pos].tex = csVector2 (i, j);

      datamap[pos].col = pos;
    }
  }
  int a, b, c, s;
  for (a = c = 1, b = 2, s = 0; a != w-1; a = c = b, b *= 2, s = w)
  {
    for (j = a; j < w-1; j += b)
    {
      for (i = 0; i < w; i += b)
      {
        ComputeError (i, j, 0, a, s, w);
        ComputeError (j, i, a, 0, s, w);
      }
    }

    for (j = a; j < w-1; c = -c, j += b)
    {
      for (i = a; i < w-1; c = -c, i += b)
      {
        ComputeError (i, j, a, c, w, w);
      }
    }
  }

  int max_error = csLog2 (w - 1) - csLog2 (MIN_TERRAIN);
  float error = pow (2.0, max_error);

  root = new MeshTreeNode (this, 0, 0, w, h, error);
  return true;
}

bool csChunkLodTerrainFactory::SetHeightMap (iImage* map)
{
  csArray<float> image_data;
  image_data.SetLength (map->GetSize());
  if (map->GetFormat () & CS_IMGFMT_PALETTED8)
  {
    csRGBpixel *palette = map->GetPalette ();
    uint8 *data = (uint8 *)map->GetImageData ();
    for (int i = 0; i < map->GetSize (); i ++)
    {
      image_data[i] = ((float)palette[data[i]].Intensity()) / 255.0;
    }
  }
  else
  {
    csRGBpixel *data = (csRGBpixel *)map->GetImageData ();
    for (int i = 0; i < map->GetSize (); i ++)
    {
      image_data[i] = ((float)data[i].Intensity()) / 255.0;
    }
  }
  return SetHeightMap (image_data, map->GetWidth(), map->GetHeight());
}

bool csChunkLodTerrainFactory::SaveState (const char *filename)
{
  return false;
}

bool csChunkLodTerrainFactory::RestoreState (const char *filename)
{
  return false;
}

csVector3 csChunkLodTerrainFactory::CollisionDetect (const csVector3 &p)
{
  int i = (int)(p.x / scale.x + (hm_x >> 1));
  float i_delta = p.x / scale.x + (hm_x >> 1) - i;
  int j = (int)(-p.z / scale.z + (hm_y >> 1));
  float j_delta =  -p.z / scale.z + (hm_y >> 1) - j;
  if (i < 0 || i >= hm_x-1 || j < 0 || j >= hm_y-1) 
    return p;

  float d1 = datamap[i+j*hm_x].pos.y * (1.0-i_delta) + 
             datamap[(i+1)+j*hm_x].pos.y * i_delta;
  float d2 = datamap[i+(j+1)*hm_x].pos.y * (1.0-i_delta) +
             datamap[(i+1)+(j+1)*hm_x].pos.y * i_delta;
  float d = d1 * (1.0 - j_delta) + d2 * j_delta + 2;
  return csVector3 (p.x, (d > p.y) ? d : p.y, p.z);
}


csChunkLodTerrainFactory::MeshTreeNode::MeshTreeNode (
	csChunkLodTerrainFactory* p, int x, int y, int w, int h, float e)
{
  pFactory = p;

  error = (e < 1) ? 0.5 : e;
  int mid_w = w>>1, mid_h = h>>1;
  int c = x+mid_w + (y+mid_w) * p->hm_x;
  center = p->datamap[c].pos;
  int sw = x + (y+h-1) * p->hm_x;
  int se = x+w-1 + (y+h-1) * p->hm_x;
  int ne = x+w-1 + y * p->hm_x;
  int nw = x + y * p->hm_x;

  box.StartBoundingBox ();
  for (int i = x; i < (x+w); i ++)
  {
    for (int j = y; j < (y+h); j ++)
    {
      box.AddBoundingVertex (p->datamap[i+j*p->hm_x].pos);
    }
  }

  csVector3 radiusv = p->datamap[sw].pos - center;
  radiusv.y = (box.MaxY() - box.MinY())/2;
  radius = radiusv.Norm ();

  max_levels = 2 * csLog2(w) - 1;
  InitBuffer (p->datamap[sw], 0);
  ProcessMap (0, c, sw, se);
  AddVertex (p->datamap[se], 1);
  ProcessMap (0, c, se, ne);
  AddVertex (p->datamap[ne], 1);
  ProcessMap (0, c, ne, nw);
  AddVertex (p->datamap[nw], 1);
  ProcessMap (0, c, nw, sw);
  AddVertex (p->datamap[sw], 1);

  const float eps = 0.00001f;
  Data south, east, north, west;
  south.pos = csVector3 (0,-error-1,eps);
  south.norm = csVector3 (0,0,-1);
  south.tan = csVector3 (1,0,0);
  south.bin = csVector3 (0,-1,0);
  south.tex = csVector2 (0,error+1);

  east.pos = csVector3 (-eps,-error-1,0);
  east.norm = csVector3 (1,0,0);
  east.tan = csVector3 (0,0,1);
  east.bin = csVector3 (0,-1,0);
  east.tex = csVector2 (error+1,0);

  north.pos = csVector3 (0,-error-1,-eps);
  north.norm = csVector3 (0,0,-1);
  north.tan = csVector3 (-1,0,0);
  north.bin = csVector3 (0,-1,0);
  north.tex = csVector2 (0,-error-1);

  west.pos = csVector3 (eps,-error-1,0);
  west.norm = csVector3 (-1,0,0);
  west.tan = csVector3 (0,0,-1);
  west.bin = csVector3 (0,-1,0);
  west.tex = csVector2 (-error-1,0);

  Data southwest;
  southwest.pos = csVector3 (eps,-error-1,eps);
  southwest.norm = csVector3 (-0.707f, 0.0f, -0.707f);
  southwest.tan = csVector3 (0.707f, 0.0f, -0.707f);
  southwest.bin = csVector3 (0, -1, 0);
  southwest.tex = csVector2 (-error-1,error+1);
  AddEdgeVertex (p->datamap[sw], southwest);
  AddSkirtVertex (p->datamap[sw], southwest);

  // bottom edge
  ProcessEdge (sw+1, se-1, 1, south);

  Data southeast;
  southeast.pos = csVector3 (-eps,-error-1,eps);
  southeast.norm = csVector3 (0.707f, 0.0f, -0.707f);
  southeast.tan = csVector3 (0.707f, 0.0f, 0.707f);
  southeast.bin = csVector3 (0,-1,0);
  southeast.tex = csVector2 (error+1,error+1);
  AddEdgeVertex (p->datamap[se], southeast);
  AddSkirtVertex (p->datamap[se], southeast);

  // right edge
  ProcessEdge (se-p->hm_x, ne+p->hm_x, -p->hm_x, east);

  Data northeast;
  northeast.pos = csVector3 (-eps,-error-1,-eps);
  northeast.norm = csVector3 (0.707f, 0.0f, 0.707f);
  northeast.tan = csVector3 (-0.707f, 0.0f, 0.707f);
  northeast.bin = csVector3 (0,-1,0);
  northeast.tex = csVector2 (error+1,-error-1);
  AddEdgeVertex (p->datamap[ne], northeast);
  AddSkirtVertex (p->datamap[ne], northeast);

  // top edge
  ProcessEdge (ne-1, nw+1, -1, north);

  Data northwest;
  northwest.pos = csVector3 (eps,-error-1,-eps);
  northwest.norm = csVector3 (-0.707f, 0.0f ,0.707f);
  northwest.tan = csVector3 (-0.707f, 0.0f, -0.707f);
  northwest.bin = csVector3 (0,-1,0);
  northwest.tex = csVector2 (-error-1,-error-1);
  AddEdgeVertex (p->datamap[nw], northwest);
  AddSkirtVertex (p->datamap[nw], northwest);

  // left edge
  ProcessEdge (nw+p->hm_x, sw-p->hm_x, p->hm_x, west);

  AddEdgeVertex (p->datamap[sw], southwest);
  AddSkirtVertex (p->datamap[sw], southwest);

  if (error >= 1)
  {
    float new_error = error / 2.0;
    int new_w = mid_w + 1;
    int new_h = mid_h + 1;
    children[0] = new MeshTreeNode (p, x, y, new_w, new_h, new_error);
    children[1] = new MeshTreeNode (p, x+new_w-1, y, new_w, new_h, new_error);
    children[2] = new MeshTreeNode (p, x, y+new_h-1, new_w, new_h, new_error);
    children[3] = new MeshTreeNode (p, x+new_w-1, y+new_h-1, new_w, new_h,
    	new_error);
  }
  else
  {
    children[0] = 0;
    children[1] = 0;
    children[2] = 0;
    children[3] = 0;
  }
}

csChunkLodTerrainFactory::MeshTreeNode::~MeshTreeNode ()
{
  delete children[0];
  delete children[1];
  delete children[2];
  delete children[3];
}

iRenderBuffer *csChunkLodTerrainFactory::MeshTreeNode::GetRenderBuffer (
	csStringID name)
{
  if (name == pFactory->vertex_name) 
  {
    if (!vertex_buffer) 
    {
      unsigned int len = vertices.Length();
      vertex_buffer = pFactory->r3d->CreateRenderBuffer (
	sizeof (csVector3) * len, CS_BUF_STATIC, 
	CS_BUFCOMP_FLOAT, 3, false);
      csVector3 *vbuf = (csVector3*)vertex_buffer->Lock (CS_BUF_LOCK_NORMAL);
      memcpy (vbuf, &vertices[0], len * sizeof (csVector3));
      vertex_buffer->Release ();
    }
    return vertex_buffer;
  }
  else if (name == pFactory->compressed_vertex_name)
  {
    return 0;
  }
  else if (name == pFactory->normal_name) 
  {
    if (!normal_buffer)
    {
      unsigned int len = normals.Length();
      normal_buffer = pFactory->r3d->CreateRenderBuffer (
	sizeof (csVector3) * len, CS_BUF_STATIC, 
	CS_BUFCOMP_FLOAT, 3, false);
   
      csVector3 *nbuf = (csVector3*)normal_buffer->Lock (CS_BUF_LOCK_NORMAL);
      memcpy (nbuf, &normals[0], len * sizeof (csVector3));
      normal_buffer->Release ();
    }
    return normal_buffer;
  }
  else if (name == pFactory->compressed_normal_name)
  {
    if (!compressed_normal_buffer)
    {
      unsigned int len = normals.Length();
      compressed_normal_buffer = pFactory->r3d->CreateRenderBuffer (
        sizeof (csVector2) * len, CS_BUF_STATIC,
	CS_BUFCOMP_FLOAT, 2, false);
      csVector2 *cnbuf = (csVector2*)compressed_normal_buffer->Lock (
      	CS_BUF_LOCK_NORMAL);
      for (unsigned int i = 0; i < len; i ++) {
        cnbuf[i].x = normals[i].x;
        cnbuf[i].y = normals[i].z;
      }
      compressed_normal_buffer->Release ();
    }
    return compressed_normal_buffer;
  }
  else if (name == pFactory->tangent_name) 
  {
    if (!tangent_buffer)
    {
      unsigned int len = tangents.Length();
      tangent_buffer = pFactory->r3d->CreateRenderBuffer (
	sizeof (csVector3) * len, CS_BUF_STATIC, 
	CS_BUFCOMP_FLOAT, 3, false);
   
      csVector3 *tbuf = (csVector3*)tangent_buffer->Lock (CS_BUF_LOCK_NORMAL);
      memcpy (tbuf, &tangents[0], len * sizeof (csVector3));
      tangent_buffer->Release ();
    }
    return tangent_buffer;
  }
  else if (name == pFactory->compressed_tangent_name)
  {
    return 0;
  }
  else if (name == pFactory->binormal_name) 
  {
    if (!binormal_buffer)
    {
      unsigned int len = binormals.Length();
      binormal_buffer = pFactory->r3d->CreateRenderBuffer (
	sizeof (csVector3) * len, CS_BUF_STATIC, 
	CS_BUFCOMP_FLOAT, 3, false);
   
      csVector3 *bbuf = (csVector3*)binormal_buffer->Lock (CS_BUF_LOCK_NORMAL);
      memcpy (bbuf, &binormals[0], len * sizeof (csVector3));
      binormal_buffer->Release ();
    }
    return binormal_buffer;
  }
  else if (name == pFactory->compressed_binormal_name)
  {
    return 0;
  }
  else if (name == pFactory->texcors_name) 
  {
    if (!texcors_buffer)
    {
      unsigned int len = texcors.Length();
      {
        texcors_buffer = pFactory->r3d->CreateRenderBuffer (
	  sizeof (csVector2) * len, CS_BUF_STATIC, 
	  CS_BUFCOMP_FLOAT, 2, false);
	texcors_buffer->CopyToBuffer (texcors.GetArray(), 
	  len * sizeof (csVector2));
      }	
    }
    return texcors_buffer;
  }
  else if (name == pFactory->compressed_texcors_name)
  {
    return 0;
  }
  else if (name == pFactory->texcoords_norm_name)
  {
    if (!texcoords_norm_buffer)
    {
      uint len = texcors.Length();
      texcoords_norm_buffer = pFactory->r3d->CreateRenderBuffer (
	sizeof (csVector2) * len, CS_BUF_STATIC, 
	CS_BUFCOMP_FLOAT, 2, false);

      float inv_x = 1.0f / (float)pFactory->hm_x;
      float inv_y = 1.0f / (float)pFactory->hm_y;

      csRenderBufferLock<csVector2> normalizedTexCoords (
	texcoords_norm_buffer);

      for (uint i = 0; i < len; i++)
      {
	normalizedTexCoords.Get (i).Set (texcors[i].x * inv_x, 
	  texcors[i].y * inv_y); 
      }
    }
    return texcoords_norm_buffer;
  }
  else if (name == pFactory->compressed_color_name)
  {
    return 0;
  }
  else if (name == pFactory->index_name) 
  {
    if (!index_buffer)
    {
      unsigned int len = vertices.Length();
      if (len < UCHAR_MAX) 
      {
        index_buffer = pFactory->r3d->CreateRenderBuffer (
	  sizeof (unsigned char) * len, CS_BUF_STATIC, 
	  CS_BUFCOMP_UNSIGNED_BYTE, 1, true);
        unsigned char *ibuf = (unsigned char *)index_buffer->Lock (
		CS_BUF_LOCK_NORMAL);
        for (unsigned char i = 0; i < len; i ++)
          ibuf[i] = i;
        index_buffer->Release ();
      } 
      else if (len < USHRT_MAX)  
      {
        index_buffer = pFactory->r3d->CreateRenderBuffer (
	  sizeof (unsigned short) * len, CS_BUF_STATIC, 
	  CS_BUFCOMP_UNSIGNED_SHORT, 1, true);
        unsigned short *ibuf = (unsigned short *)index_buffer->Lock (
		CS_BUF_LOCK_NORMAL);
        for (unsigned short i = 0; i < len; i ++)
          ibuf[i] = i;
        index_buffer->Release ();
      } 
      else
      {
        index_buffer = pFactory->r3d->CreateRenderBuffer (
	  sizeof (unsigned int) * len, CS_BUF_STATIC, 
	  CS_BUFCOMP_UNSIGNED_INT, 1, true);
        unsigned int *ibuf = (unsigned int *)index_buffer->Lock (
		CS_BUF_LOCK_NORMAL);
        for (unsigned int i = 0; i < len; i ++)
          ibuf[i] = i;
        index_buffer->Release ();
      }
    }
    return index_buffer;
  }
  return 0;
}

void csChunkLodTerrainFactory::MeshTreeNode::InitBuffer (const Data& d, int p)
{
  vertices.Push(d.pos); vertices.Push(d.pos);
  normals.Push(d.norm); normals.Push(d.norm);
  tangents.Push(d.tan); tangents.Push(d.tan);
  binormals.Push(d.bin); binormals.Push(d.bin);
  texcors.Push(d.tex); texcors.Push(d.tex);
  colors.Push(d.col); colors.Push(d.col);
  parity = p;
}

void csChunkLodTerrainFactory::MeshTreeNode::AddVertex (const Data& d, int p)
{
  int len = vertices.Length ();
  if (d.pos == vertices[len - 1] || d.pos == vertices[len - 2])
    return;
  if (p == parity)
  {
    vertices.Push (vertices[len - 2]);
    normals.Push (normals[len - 2]);
    tangents.Push (tangents[len - 2]);
    binormals.Push (binormals[len - 2]);
    texcors.Push (texcors[len - 2]);
    colors.Push (colors[len - 2]);
  }
  vertices.Push(d.pos);
  normals.Push(d.norm);
  tangents.Push(d.tan);
  binormals.Push(d.bin);
  texcors.Push(d.tex);
  colors.Push(d.col);
  parity = p;
}

void csChunkLodTerrainFactory::MeshTreeNode::AddEdgeVertex (const Data& d,
	const Data& m)
{
  vertices.Push(d.pos+csVector3 (m.pos.x, 0.0, m.pos.z));
  normals.Push(d.norm);
  tangents.Push(d.tan);
  binormals.Push(d.bin);
  texcors.Push(d.tex);
  colors.Push(d.col);
}

void csChunkLodTerrainFactory::MeshTreeNode::AddSkirtVertex (const Data& d,
	const Data& m)
{
  vertices.Push(d.pos+m.pos);
  normals.Push(d.norm);
  tangents.Push(d.tan);
  binormals.Push(d.bin);
  texcors.Push(d.tex+m.tex);
  colors.Push(d.col);
}


void csChunkLodTerrainFactory::MeshTreeNode::ProcessMap (int l, int i, int j, 
	int k)
{
  int child = (j+k)>>1;
  if (l == 0 || pFactory->datamap[i].error > error)
  {
    if (l < max_levels)
      ProcessMap (l+1, child, j, i);
    AddVertex (pFactory->datamap[i], l & 1);
    if (l < max_levels)
      ProcessMap (l+1, child, i, k);
  }
}

void csChunkLodTerrainFactory::MeshTreeNode::ProcessEdge (int start, int end, 
	int move, const Data& mod)
{
  while (start != end) 
  {
    if (pFactory->datamap[start].error > error)
    {
      AddEdgeVertex (pFactory->datamap[start], mod);
      AddSkirtVertex (pFactory->datamap[start], mod);
    }
    start += move;
  }
}

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csChunkLodTerrainObject)
  SCF_IMPLEMENTS_INTERFACE (iMeshObject)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iTerrainObjectState)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObjectModel)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iLightingInfo)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iShadowReceiver)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csChunkLodTerrainObject::eiTerrainObjectState)
  SCF_IMPLEMENTS_INTERFACE (iTerrainObjectState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csChunkLodTerrainObject::eiObjectModel)
  SCF_IMPLEMENTS_INTERFACE (iObjectModel)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csChunkLodTerrainObject::LightingInfo)
  SCF_IMPLEMENTS_INTERFACE (iLightingInfo)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csChunkLodTerrainObject::ShadowReceiver)
  SCF_IMPLEMENTS_INTERFACE (iShadowReceiver)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csChunkLodTerrainObject::csChunkLodTerrainObject (csChunkLodTerrainFactory* p)
	: logparent (p), pFactory (p), returnMeshesHolder (false)
{
  SCF_CONSTRUCT_IBASE (p)
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiTerrainObjectState)
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObjectModel)
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiLightingInfo)
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiShadowReceiver)

  error_tolerance = 1.0;
  lod_distance = 200.0;

  staticLighting = false;
  colorVersion = 0;
  dynamic_ambient.Set (0.0f, 0.0f, 0.0f);
  staticLights.SetLength (pFactory->hm_x * pFactory->hm_y);

  rootNode.AttachNew (new MeshTreeNodeWrapper (this, p->root));
}

csChunkLodTerrainObject::~csChunkLodTerrainObject ()
{
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiLightingInfo)
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiShadowReceiver)
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiTerrainObjectState)
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiObjectModel)
  SCF_DESTRUCT_IBASE ()
}

int optimized_meshes;

bool csChunkLodTerrainObject::DrawTestQuad (iRenderView* rv, 
	MeshTreeNodeWrapper* nodeWrapper, float kappa,
	uint32 frustum_mask)
{
  csChunkLodTerrainFactory::MeshTreeNode* node = nodeWrapper->factoryNode;

  int clip_portal, clip_plane, clip_z_plane;
  if (!rv->ClipBBox (planes, frustum_mask,
  	node->BBox (), clip_portal, clip_plane, clip_z_plane))
    return false;

  csVector3 camera_origin = tr_o2c * node->Center ();
  float sq_dist = camera_origin.SquaredNorm ();
  float error_projection = node->Error() / kappa + node->Radius();
  error_projection *= error_projection;
  if (error_projection > sq_dist && node->GetChild(0) != 0)
  {
    DrawTestQuad (rv, nodeWrapper->GetChild (0), kappa, frustum_mask);
    DrawTestQuad (rv, nodeWrapper->GetChild (1), kappa, frustum_mask);
    DrawTestQuad (rv, nodeWrapper->GetChild (2), kappa, frustum_mask);
    DrawTestQuad (rv, nodeWrapper->GetChild (3), kappa, frustum_mask);
  } 
  else 
  {
    bool meshCreated;
    csRenderMesh*& rm = rmHolder.GetUnusedMesh (meshCreated);
    rm->object2camera = tr_o2c;
    rm->camera_origin = camera_origin;
    rm->clip_portal = clip_portal;
    rm->clip_plane = clip_plane;
    rm->clip_z_plane = clip_z_plane;
    rm->do_mirror = rv->GetCamera()->IsMirrored();
    matwrap->Visit ();
    rm->material = matwrap;
    rm->z_buf_mode = CS_ZBUF_TEST;
    rm->mixmode = CS_FX_COPY;
    rm->variablecontext = nodeWrapper->svcontext;
    rm->indexstart = 0;
    rm->indexend = node->Count ();
    rm->meshtype = CS_MESHTYPE_TRIANGLESTRIP;
    // meshes[len].meshtype = CS_MESHTYPE_LINESTRIP;
    returnMeshes->Push (rm);

    float texel_error_projection = node->Radius() + lod_distance;
    texel_error_projection *= texel_error_projection;
    if (texel_error_projection > sq_dist) 
    {
      for (int i = 0; i < palette.Length(); i ++)
      {
	csRenderMesh*& rm = rmHolder.GetUnusedMesh (meshCreated);
        rm->object2camera = tr_o2c;
        rm->camera_origin = camera_origin;
        rm->clip_portal = clip_portal;
        rm->clip_plane = clip_plane;
        rm->clip_z_plane = clip_z_plane;
        rm->do_mirror = rv->GetCamera()->IsMirrored();
        palette[i]->Visit ();
        rm->material = palette[i];
        rm->z_buf_mode = CS_ZBUF_TEST;
        rm->mixmode = CS_FX_COPY;
	rm->variablecontext = nodeWrapper->svcontext;
        rm->indexstart = 0;
        rm->indexend = node->Count ();
        rm->meshtype = CS_MESHTYPE_TRIANGLESTRIP;
	returnMeshes->Push (rm);
        // palette_meshes[i][len].meshtype = CS_MESHTYPE_LINESTRIP;
      }
    }
    tricount += node->Count () - 2;
  }
  return true;
}

bool csChunkLodTerrainObject::DrawTest (iRenderView* rview, iMovable* movable)
{
  iCamera* cam = rview->GetCamera ();
  tr_o2c = cam->GetTransform ();
  if (!movable->IsFullTransformIdentity ())
    tr_o2c /= movable->GetFullTransform ();

  float kappa = error_tolerance * cam->GetInvFOV() * 
	2*tan(cam->GetFOVAngle()/180.0*PI/2);
  returnMeshes = &returnMeshesHolder.GetUnusedMeshes ();
  returnMeshes->Empty ();
  tricount = 0;
  uint32 frustum_mask;
  rview->SetupClipPlanes (tr_o2c, planes, frustum_mask);
  if (!DrawTestQuad (rview, rootNode, kappa, frustum_mask)) 
    return false;
  if (returnMeshes->Length () == 0)
    return false;
// printf ("avg triangle per mesh %f\n", (float)tricount/(float)meshes.Length());
// printf ("tricount %d, meshcount %d\n", tricount, meshes.Length());
  return true;
}

csRenderMesh** csChunkLodTerrainObject::GetRenderMeshes (
	int &n, iRenderView* rview, 
	iMovable* movable, uint32 frustum_mask)
{
  if (!DrawTest (rview, movable))
  {
    n = 0;
    return 0;
  }

  n = returnMeshes->Length();
  if (n == 0) 
  {
    // pass back root node as default always
    bool meshCreated;
    csRenderMesh*& rm = rmHolder.GetUnusedMesh (meshCreated);
    rm->z_buf_mode = CS_ZBUF_TEST;
    rm->mixmode = CS_FX_COPY;
    rm->variablecontext = rootNode->svcontext;
    rm->indexstart = 0;
    rm->indexend = rootNode->factoryNode->Count ();
    rm->meshtype = CS_MESHTYPE_TRIANGLESTRIP;
    returnMeshes->Push (rm);
    n ++;
  }

  light_movable = movable;
  /*
    @@@ In theory it can happen that GetRenderMeshes() is consecutively called
    with different movables, causing that for later lighting updates a wrong
    movable is used.
   */

  return returnMeshes->GetArray();;
}

bool csChunkLodTerrainObject::HitBeamOutline (const csVector3& start, 
	const csVector3& end, csVector3& isect, float* pr)
{
  csReport (pFactory->object_reg, CS_REPORTER_SEVERITY_DEBUG, 
    "crystalspace.mesh.object.terrain.chunklod",
    "HitBeamOutline called, but not implemented");
  return false;
}

bool csChunkLodTerrainObject::HitBeamObject (const csVector3& start,
	const csVector3& end, csVector3& isect, float* pr,
	int* polygon_idx)
{
  if (polygon_idx) *polygon_idx = -1;
  csReport (pFactory->object_reg, CS_REPORTER_SEVERITY_DEBUG, 
    "crystalspace.mesh.object.terrain.chunklod",
    "HitBeamObject called, but not implemented");
  return false;
}

bool csChunkLodTerrainObject::SetMaterialPalette (
	const csArray<iMaterialWrapper*>& pal)
{
  palette.SetLength (pal.Length());
  for (int i = 0; i < pal.Length(); i++)
  {
    palette[i] = pal[i];
  }
  return true;
}

bool csChunkLodTerrainObject::SetMaterialMap (const csArray<char>& data,
	int w, int h)
{
  csRef<iStringSet> strings = 
	CS_QUERY_REGISTRY_TAG_INTERFACE (pFactory->object_reg,
	"crystalspace.shared.stringset", iStringSet);
  csRef<iTextureManager> mgr = pFactory->r3d->GetTextureManager ();
  csRef<csShaderVariable> splat_var = 
    new csShaderVariable (strings->Request ("splat map scale"));
  splat_var->SetType (csShaderVariable::VECTOR2);
  splat_var->SetValue (csVector2 (1.0 / (float)pFactory->hm_x,
  	1.0 / (float)pFactory->hm_y));
  matwrap->GetMaterial()->AddVariable (splat_var);

  csRef<csShaderVariable> lod_var = 
    new csShaderVariable (strings->Request ("texture lod distance"));
  lod_var->SetType (csShaderVariable::VECTOR3);
  lod_var->SetValue (csVector3 (lod_distance, lod_distance, lod_distance));
  matwrap->GetMaterial()->AddVariable (lod_var); 
    // @@@ FIXME: Don't change the material, use the RM SV context

  for (int i = 0; i < palette.Length(); i ++) 
  {
    csRef<iImage> alpha = csPtr<iImage> (new csImageMemory (w, h, 
	CS_IMGFMT_ALPHA | CS_IMGFMT_TRUECOLOR));

    csRGBpixel *map = (csRGBpixel *)alpha->GetImageData ();
    int y, x;
    for (y = 0; y < h; y ++) 
    {
      for (x = 0; x < w; x ++) 
      {
	int v = (data[x + y * w] == i) ? 255 : 0;
	map[x + y * w].Set (v, v, v, v);
      }
    }

    csRef<iTextureHandle> hdl = mgr->RegisterTexture (alpha, CS_TEXTURE_2D);
    csRef<csShaderVariable> var = 
      new csShaderVariable (strings->Request ("splat alpha map"));
    var->SetType (csShaderVariable::TEXTURE);
    var->SetValue (hdl);
    palette[i]->GetMaterial()->AddVariable (var);
    csRef<csShaderVariable> splat_var = 
      new csShaderVariable (strings->Request ("splat map scale"));
    splat_var->SetType (csShaderVariable::VECTOR2);
    splat_var->SetValue (csVector2 (1.0 / (float)pFactory->hm_x,
    	1.0 / (float)pFactory->hm_y));
    matwrap->GetMaterial()->AddVariable (splat_var);
    palette[i]->GetMaterial()->AddVariable (splat_var);

    csRef<csShaderVariable> lod_var = 
      new csShaderVariable (strings->Request ("texture lod distance"));
    lod_var->SetType (csShaderVariable::VECTOR3);
    lod_var->SetValue (csVector3 (lod_distance, lod_distance, lod_distance));
    matwrap->GetMaterial()->AddVariable (lod_var);
    palette[i]->GetMaterial()->AddVariable (lod_var);
  }
  return true;
}

bool csChunkLodTerrainObject::SetMaterialMap (iImage* map)
{
  csArray<char> image_data;
  image_data.SetLength (map->GetSize());
  if (map->GetFormat () & CS_IMGFMT_PALETTED8)
  {
    uint8 *data = (uint8 *)map->GetImageData ();
    for (int i = 0; i < map->GetSize (); i ++)
    {
      image_data[i] = data[i];
    }
  }
  else
  {
    csRGBpixel *data = (csRGBpixel *)map->GetImageData ();
    for (int i = 0; i < map->GetSize (); i ++)
    {
      image_data[i] = data[i].Intensity();
    }
  }
  return SetMaterialMap (image_data, map->GetWidth(), map->GetHeight());
}

bool csChunkLodTerrainObject::SaveState (const char *filename)
{
  return false;
}

bool csChunkLodTerrainObject::RestoreState (const char *filename)
{
  return false;
}

int csChunkLodTerrainObject::CollisionDetect (iMovable *m, csTransform *t)
{
  csVector3 p = t->GetOrigin() - m->GetPosition ();
  csVector3 np = pFactory->CollisionDetect (p);
  if (np != p) 
  {
    t->SetOrigin (np + m->GetPosition ());
    return 1;
  } 
  else 
  {
    return 0;
  }
}

struct QueuedLight
{
  iLight* light;
  bool isPseudo;

  csVector3 obj_light_pos;
  float inflRadSq;
  csColor lightcol;

  float* intensities;
};
CS_IMPLEMENT_STATIC_VAR (GetLightQueue, csArray<QueuedLight>, ())

void csChunkLodTerrainObject::UpdateColors (const csArray<int>& colors,
					    const csVector3* vertices, 
					    const csVector3* normals,
					    const csBox3& box,
					    csColor* staticColors)
{
  const float lightScale = CS_NORMAL_LIGHT_LEVEL / 256.0f;
  csReversibleTransform trans = light_movable->GetFullTransform ();

  csArray<QueuedLight>& lightQueue = *GetLightQueue();
  lightQueue.Empty();

  csSet<iLight*>::GlobalIterator it = affecting_lights.GetIterator ();
  while (it.HasNext ())
  {
    QueuedLight ql;
    ql.light = it.Next ();

    csVector3 wor_light_pos = ql.light->GetCenter ();
    ql.obj_light_pos = trans.Other2This (wor_light_pos);

    if (!csIntersect3::BoxSphere (box, ql.obj_light_pos,
      (ql.inflRadSq = ql.light->GetInfluenceRadiusSq()))) continue;

    ql.isPseudo = false;
    ql.lightcol = ql.light->GetColor () * lightScale;

    lightQueue.Push (ql);
  }
  csHash<csShadowArray*, iLight*>::GlobalIterator pdlIt =
	pseudoDynInfo.GetIterator ();
  while (pdlIt.HasNext ())
  {
    QueuedLight ql;
    csShadowArray* shadowArr = pdlIt.Next (ql.light);

    csVector3 wor_light_pos = ql.light->GetCenter ();
    csVector3 obj_light_pos = trans.Other2This (wor_light_pos);

    if (!csIntersect3::BoxSphere (box, obj_light_pos,
      (ql.light->GetInfluenceRadiusSq()))) continue;

    ql.isPseudo = true;
    ql.intensities = shadowArr->shadowmap;
    ql.lightcol = ql.light->GetColor ();

    lightQueue.Push (ql);
  }

  csColor baseColor (dynamic_ambient);
  if (!staticLighting)
  {
    csColor amb;
    pFactory->engine->GetAmbientLight (amb);
    baseColor += amb;
  }
      
  for (int i = 0; i < colors.Length(); i++)
  {
    csColor col (baseColor);
    if (staticLighting)
    {
      col += staticLights[colors[i]];
    }
    staticColors[i] = col;
  }

  for (int l = 0; l < lightQueue.Length(); l++)
  {
    const QueuedLight& ql = lightQueue[l];

    if (ql.isPseudo)
    {
      for (int i = 0; i < colors.Length(); i++)
      {
	staticColors[i] += ql.lightcol * ql.intensities[colors[i]];
      }
    }
    else
    {
      for (int i = 0; i < colors.Length(); i++)
      {
	float obj_sq_dist = csSquaredDist::PointPoint (ql.obj_light_pos, 
	  vertices[i]);
	if (obj_sq_dist >= ql.inflRadSq) continue;

	csColor light_color = ql.lightcol * 
	  ql.light->GetBrightnessAtDistance (qsqrt (obj_sq_dist));

	float cosinus;
	if (obj_sq_dist < SMALL_EPSILON) cosinus = 1;
	else cosinus = ql.obj_light_pos * normals[i];

	if (cosinus > 0)
	{
	  csColor newcol = light_color;
	  if (obj_sq_dist >= SMALL_EPSILON) 
	    cosinus *= (obj_sq_dist >= SMALL_EPSILON) ? 
	      qisqrt (obj_sq_dist) : 1.0f;
	  if (cosinus < 1) newcol *= cosinus;
	  staticColors[i] += newcol;
	}
      }
    }
  }
}

char* csChunkLodTerrainObject::GenerateCacheName ()
{
  csBox3 b;
  pFactory->GetObjectBoundingBox (b);

  csMemFile mf;
  mf.Write ("chunklod", 8);
  uint32 l;
  l = convert_endian ((uint32)pFactory->hm_x);
  mf.Write ((char*)&l, 4);
  l = convert_endian ((uint32)pFactory->hm_y);
  mf.Write ((char*)&l, 4);

  if (logparent)
  {
    csRef<iMeshWrapper> mw (SCF_QUERY_INTERFACE (logparent, iMeshWrapper));
    if (mw)
    {
      if (mw->QueryObject ()->GetName ())
        mf.Write (mw->QueryObject ()->GetName (),
        strlen (mw->QueryObject ()->GetName ()));
      iMovable* movable = mw->GetMovable ();
      iSector* sect = movable->GetSectors ()->Get (0);
      if (sect && sect->QueryObject ()->GetName ())
        mf.Write (sect->QueryObject ()->GetName (),
        strlen (sect->QueryObject ()->GetName ()));
      csVector3 pos = movable->GetFullPosition ();
      l = convert_endian ((int32)QInt ((pos.x * 1000)+.5));
      mf.Write ((char*)&l, 4);
      l = convert_endian ((int32)QInt ((pos.y * 1000)+.5));
      mf.Write ((char*)&l, 4);
      l = convert_endian ((int32)QInt ((pos.z * 1000)+.5));
      mf.Write ((char*)&l, 4);
      csReversibleTransform tr = movable->GetFullTransform ();
      const csMatrix3& o2t = tr.GetO2T ();
      l = convert_endian ((int32)QInt ((o2t.m11 * 1000)+.5));
      mf.Write ((char*)&l, 4);
      l = convert_endian ((int32)QInt ((o2t.m12 * 1000)+.5));
      mf.Write ((char*)&l, 4);
      l = convert_endian ((int32)QInt ((o2t.m13 * 1000)+.5));
      mf.Write ((char*)&l, 4);
      l = convert_endian ((int32)QInt ((o2t.m21 * 1000)+.5));
      mf.Write ((char*)&l, 4);
      l = convert_endian ((int32)QInt ((o2t.m22 * 1000)+.5));
      mf.Write ((char*)&l, 4);
      l = convert_endian ((int32)QInt ((o2t.m23 * 1000)+.5));
      mf.Write ((char*)&l, 4);
      l = convert_endian ((int32)QInt ((o2t.m31 * 1000)+.5));
      mf.Write ((char*)&l, 4);
      l = convert_endian ((int32)QInt ((o2t.m32 * 1000)+.5));
      mf.Write ((char*)&l, 4);
      l = convert_endian ((int32)QInt ((o2t.m33 * 1000)+.5));
      mf.Write ((char*)&l, 4);
    }
  }

  l = convert_endian ((int32)QInt ((b.MinX () * 1000)+.5));
  mf.Write ((char*)&l, 4);
  l = convert_endian ((int32)QInt ((b.MinY () * 1000)+.5));
  mf.Write ((char*)&l, 4);
  l = convert_endian ((int32)QInt ((b.MinZ () * 1000)+.5));
  mf.Write ((char*)&l, 4);
  l = convert_endian ((int32)QInt ((b.MaxX () * 1000)+.5));
  mf.Write ((char*)&l, 4);
  l = convert_endian ((int32)QInt ((b.MaxY () * 1000)+.5));
  mf.Write ((char*)&l, 4);
  l = convert_endian ((int32)QInt ((b.MaxZ () * 1000)+.5));
  mf.Write ((char*)&l, 4);

  csMD5::Digest digest = csMD5::Encode (mf.GetData (), mf.GetSize ());
  csString hex(digest.HexString());
  return hex.Detach();
}

void csChunkLodTerrainObject::InitializeDefault (bool clear)
{
  if (!staticLighting) return;

  if (clear)
  {
    csColor amb;
    float lightScale = CS_NORMAL_LIGHT_LEVEL / 256.0f;
    pFactory->engine->GetAmbientLight (amb);
    amb *= lightScale;
    for (int i = 0 ; i < staticLights.Length(); i++)
    {
      staticLights[i] = amb;
    }
  }
  colorVersion++;
}

const char CachedLightingMagic[] = "chunky";
const int CachedLightingMagicSize = sizeof (CachedLightingMagic) - 1;

#define STATIC_LIGHT_SCALE	255.0f

bool csChunkLodTerrainObject::ReadFromCache (iCacheManager* cache_mgr)
{
  if (!staticLighting) return true;

  colorVersion++;
  char* cachename = GenerateCacheName ();
  cache_mgr->SetCurrentScope (cachename);
  delete[] cachename;

  bool rc = false;
  csRef<iDataBuffer> db = cache_mgr->ReadCache ("chunklod_lm", 0, ~0);
  if (db)
  {
    csMemFile mf ((const char*)(db->GetData ()), db->GetSize ());
    char magic[CachedLightingMagicSize + 1];
    if (mf.Read (magic, CachedLightingMagicSize) != CachedLightingMagicSize) 
      goto stop;
    magic[CachedLightingMagicSize] = 0;
    if (strcmp (magic, CachedLightingMagic) == 0)
    {
      int v;
      for (v = 0; v < staticLights.Length(); v++)
      {
	csColor& c = staticLights[v];
	uint8 b;
	if (mf.Read ((char*)&b, sizeof (b)) != sizeof (b)) goto stop;
	c.red = (float)b / STATIC_LIGHT_SCALE;
	if (mf.Read ((char*)&b, sizeof (b)) != sizeof (b)) goto stop;
	c.green = (float)b / STATIC_LIGHT_SCALE;
	if (mf.Read ((char*)&b, sizeof (b)) != sizeof (b)) goto stop;
	c.blue = (float)b / STATIC_LIGHT_SCALE;
      }

      uint8 c;
      if (mf.Read ((char*)&c, sizeof (c)) != sizeof (c)) goto stop;
      while (c != 0)
      {
	char lid[16];
	if (mf.Read (lid, 16) != 16) goto stop;
	iLight *l = pFactory->engine->FindLightID (lid);
	if (!l) goto stop;
	l->AddAffectedLightingInfo (&scfiLightingInfo);

	csShadowArray* shadowArr = new csShadowArray();
	float* intensities = new float[staticLights.Length()];
	shadowArr->shadowmap = intensities;
	for (int n = 0; n < staticLights.Length(); n++)
	{
          uint8 b;
          if (mf.Read ((char*)&b, sizeof (b)) != sizeof (b))
          {
            delete shadowArr;
            goto stop;
          }
          intensities[n] = (float)b / STATIC_LIGHT_SCALE;
	}
	pseudoDynInfo.Put (l, shadowArr);

        if (mf.Read ((char*)&c, sizeof (c)) != sizeof (c)) goto stop;
      }
      rc = true;
    }
  }

stop:
  cache_mgr->SetCurrentScope (0);
  return rc;
}

bool csChunkLodTerrainObject::WriteToCache (iCacheManager* cache_mgr)
{
  if (!staticLighting) return true;
  char* cachename = GenerateCacheName ();
  cache_mgr->SetCurrentScope (cachename);
  delete[] cachename;

  bool rc = false;
  csMemFile mf;
  mf.Write (CachedLightingMagic, CachedLightingMagicSize);
  for (int v = 0; v < staticLights.Length(); v++)
  {
    const csColor& c = staticLights[v];
    int i; uint8 b;

    i = QInt (c.red * STATIC_LIGHT_SCALE);
    if (i < 0) i = 0; if (i > 255) i = 255; b = i;
    mf.Write ((char*)&b, sizeof (b));

    i = QInt (c.green * STATIC_LIGHT_SCALE);
    if (i < 0) i = 0; if (i > 255) i = 255; b = i;
    mf.Write ((char*)&b, sizeof (b));

    i = QInt (c.blue * STATIC_LIGHT_SCALE);
    if (i < 0) i = 0; if (i > 255) i = 255; b = i;
    mf.Write ((char*)&b, sizeof (b));
  }
  uint8 c = 1;

  csHash<csShadowArray*, iLight*>::GlobalIterator pdlIt (
    pseudoDynInfo.GetIterator ());
  while (pdlIt.HasNext ())
  {
    mf.Write ((char*)&c, sizeof (c));

    iLight* l;
    csShadowArray* shadowArr = pdlIt.Next (l);
    const char* lid = l->GetLightID ();
    mf.Write ((char*)lid, 16);

    float* intensities = shadowArr->shadowmap;
    for (int n = 0; n < staticLights.Length(); n++)
    {
      int i; uint8 b;
      i = QInt (intensities[n] * STATIC_LIGHT_SCALE);
      if (i < 0) i = 0; if (i > 255) i = 255; b = i;
      mf.Write ((char*)&b, sizeof (b));
    }
  }
  c = 0;
  mf.Write ((char*)&c, sizeof (c));


  rc = cache_mgr->CacheData ((void*)(mf.GetData ()), mf.GetSize (),
    "chunklod_lm", 0, ~0);
  cache_mgr->SetCurrentScope (0);
  return rc;
}

void csChunkLodTerrainObject::PrepareLighting ()
{
  if (!staticLighting && pFactory->light_mgr)
  {
    const csArray<iLight*>& relevant_lights = pFactory->light_mgr
      ->GetRelevantLights (logparent, -1, false);
    for (int i = 0; i < relevant_lights.Length(); i++)
      affecting_lights.Add (relevant_lights[i]);
  }
}

void csChunkLodTerrainObject::SetDynamicAmbientLight (const csColor& color)
{
  dynamic_ambient = color;
  colorVersion++;
}

void csChunkLodTerrainObject::LightChanged (iLight* light)
{
  colorVersion++;
}

void csChunkLodTerrainObject::LightDisconnect (iLight* light)
{
  affecting_lights.Delete (light);
  colorVersion++;
}

#define VERTEX_OFFSET       (10.0f * SMALL_EPSILON)

void csChunkLodTerrainObject::CastShadows (iMovable* movable, 
					   iFrustumView* fview)
{
  iBase* b = (iBase *)fview->GetUserdata ();
  iLightingProcessInfo* lpi = (iLightingProcessInfo*)b;
  CS_ASSERT (lpi != 0);

  iLight* li = lpi->GetLight ();
  bool dyn = lpi->IsDynamic ();

  if (!dyn)
  {
    if (!staticLighting || 
      li->GetDynamicType () == CS_LIGHT_DYNAMICTYPE_PSEUDO)
    {
      li->AddAffectedLightingInfo (&scfiLightingInfo);
      if (li->GetDynamicType () != CS_LIGHT_DYNAMICTYPE_PSEUDO)
        affecting_lights.Add (li);
    }
  }
  else
  {
    if (!affecting_lights.In (li))
    {
      li->AddAffectedLightingInfo (&scfiLightingInfo);
      affecting_lights.Add (li);
    }
    if (staticLighting) return;
  }

  if (!staticLighting) return;

  csReversibleTransform o2w (movable->GetFullTransform ());

  csFrustum *light_frustum = fview->GetFrustumContext ()->GetLightFrustum ();
  iShadowBlockList* shadows = fview->GetFrustumContext ()->GetShadows ();
  iShadowIterator* shadowIt = shadows->GetShadowIterator ();

  // Compute light position in object coordinates
  csVector3 wor_light_pos = li->GetCenter ();
  csVector3 obj_light_pos = o2w.Other2This (wor_light_pos);

  bool pseudoDyn = li->GetDynamicType () == CS_LIGHT_DYNAMICTYPE_PSEUDO;
  csShadowArray* shadowArr;
  if (pseudoDyn)
  {
    shadowArr = new csShadowArray ();
    pseudoDynInfo.Put (li, shadowArr);
    shadowArr->shadowmap = new float[staticLights.Length()];
    memset(shadowArr->shadowmap, 0, staticLights.Length() * sizeof(float));
  }

  float lightScale = CS_NORMAL_LIGHT_LEVEL / 256.0f;
  csColor light_color = li->GetColor () * lightScale /* * (256. / CS_NORMAL_LIGHT_LEVEL)*/;

  csColor col;
  int i;
  for (i = 0 ; i < staticLights.Length() ; i++)
  {
    const csChunkLodTerrainFactory::Data& data = pFactory->datamap[i];
    /*
      A small fraction of the normal is added to prevent unwanted
      self-shadowing (due small inaccuracies, the tri(s) this vertex
      lies on may shadow it.)
     */
    csVector3 v = o2w.This2Other (data.pos + (data.norm * VERTEX_OFFSET)) -
      wor_light_pos;

    if (!light_frustum->Contains (v))
    {
      continue;
    }
    bool inShadow = false;
    shadowIt->Reset ();
    while (shadowIt->HasNext ())
    {
      csFrustum* shadowFrust = shadowIt->Next ();
      if (shadowFrust->Contains (v))
      {
	inShadow = true;
	break;
      }
    }
    if (inShadow) continue;

    float vrt_sq_dist = csSquaredDist::PointPoint (obj_light_pos,
      data.pos);
    if (vrt_sq_dist >= li->GetInfluenceRadiusSq ()) continue;
    float in_vrt_dist =
      (vrt_sq_dist >= SMALL_EPSILON) ? qisqrt (vrt_sq_dist) : 1.0f;

    float cosinus;
    if (vrt_sq_dist < SMALL_EPSILON) cosinus = 1;
    else cosinus = (obj_light_pos - data.pos) * data.norm;
    // because the vector from the object center to the light center
    // in object space is equal to the position of the light

    if (cosinus > 0)
    {
      if (vrt_sq_dist >= SMALL_EPSILON) cosinus *= in_vrt_dist;
      float bright = li->GetBrightnessAtDistance (qsqrt (vrt_sq_dist));
      if (cosinus < 1) bright *= cosinus;
      if (pseudoDyn)
      {
	// Pseudo-dynamic
	bright *= lightScale;
	if (bright > 1.0f) bright = 1.0f; // @@@ clamp here?
	shadowArr->shadowmap[i] = bright;
      }
      else
      {
	col = light_color * bright;
	staticLights[i] += col;
      }
    }
  }
}

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csChunkLodTerrainObject::MeshTreeNodeWrapper)
SCF_IMPLEMENT_IBASE_END

csChunkLodTerrainObject::MeshTreeNodeWrapper::MeshTreeNodeWrapper (
  csChunkLodTerrainObject* obj, csChunkLodTerrainFactory::MeshTreeNode* node)
{
  SCF_CONSTRUCT_IBASE(0);
  MeshTreeNodeWrapper::obj = obj;
  factoryNode = node;

  svcontext.AttachNew (new csShaderVariableContext ());
  csRef<iShaderVariableAccessor> sva;
  sva.AttachNew (new MeshTreeNodeSVA (this));

  csShaderVariable *sv;
  sv = svcontext->GetVariableAdd (obj->pFactory->vertex_name);
  sv->SetAccessor (sva);
  sv = svcontext->GetVariableAdd (obj->pFactory->normal_name);
  sv->SetAccessor (sva);
  sv = svcontext->GetVariableAdd (obj->pFactory->tangent_name);
  sv->SetAccessor (sva);
  sv = svcontext->GetVariableAdd (obj->pFactory->binormal_name);
  sv->SetAccessor (sva);
  sv = svcontext->GetVariableAdd (obj->pFactory->texcors_name);
  sv->SetAccessor (sva);
  sv = svcontext->GetVariableAdd (obj->pFactory->texcoords_norm_name);
  sv->SetAccessor (sva);
  sv = svcontext->GetVariableAdd (obj->pFactory->color_name);
  sv->SetAccessor (sva);
  sv = svcontext->GetVariableAdd (obj->pFactory->index_name);
  sv->SetAccessor (sva);
}

csChunkLodTerrainObject::MeshTreeNodeWrapper::~MeshTreeNodeWrapper()
{
  SCF_DESTRUCT_IBASE();
}

csChunkLodTerrainObject::MeshTreeNodeWrapper* 
csChunkLodTerrainObject::MeshTreeNodeWrapper::GetChild (int n)
{
  if (children[n] == 0)
  {
    children[n].AttachNew (
      new MeshTreeNodeWrapper (obj, factoryNode->GetChild (n)));
  }
  return children[n];
}

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csChunkLodTerrainObject::MeshTreeNodeSVA)
  SCF_IMPLEMENTS_INTERFACE(iShaderVariableAccessor)
SCF_IMPLEMENT_IBASE_END

csChunkLodTerrainObject::MeshTreeNodeSVA::MeshTreeNodeSVA (
  MeshTreeNodeWrapper* wrapper)
{
  SCF_CONSTRUCT_IBASE(0);

  MeshTreeNodeSVA::wrapper = wrapper;
  colorVersion = ~0;
}

csChunkLodTerrainObject::MeshTreeNodeSVA::~MeshTreeNodeSVA ()
{
  SCF_DESTRUCT_IBASE();
}

void csChunkLodTerrainObject::MeshTreeNodeSVA::PreGetValue (
  csShaderVariable *variable)
{
  const csStringID name = variable->GetName ();
  csChunkLodTerrainObject* obj = wrapper->obj;
  csChunkLodTerrainFactory* factory = obj->pFactory;
  if (name == factory->color_name)
  {
    if (!colorBuffer || (colorVersion != obj->colorVersion))
    {
      const csArray<int>& colors = wrapper->factoryNode->colors;
      unsigned int len = colors.Length();
      if (!colorBuffer)
      {
	colorBuffer = factory->r3d->CreateRenderBuffer (
	  sizeof (csColor) * len, CS_BUF_DYNAMIC, 
	  CS_BUFCOMP_FLOAT, 3, false);
      }

      csChunkLodTerrainFactory::MeshTreeNode* node = 
	wrapper->factoryNode;
      CS_ALLOC_STACK_ARRAY(csColor, newColors, len);

      obj->UpdateColors (colors, node->GetVertices(), node->GetNormals(), 
	node->BBox(), newColors);

      colorBuffer->CopyToBuffer (newColors, sizeof (csColor) * len);

      colorVersion = obj->colorVersion;
    }
    variable->SetValue (colorBuffer);
  }
  else
    variable->SetValue (
      wrapper->factoryNode->GetRenderBuffer (name));
}
