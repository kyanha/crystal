/*
    Metaballs Demo
    Copyright (C) 1999 by Denis Dmitriev

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

#include <stdarg.h>
#include <string.h>
#include "cssysdef.h"
#include "qsqrt.h"
#include "csutil/scf.h"
#include "csutil/cscolor.h"
#include "iengine/rview.h"
#include "iengine/movable.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "ivideo/material.h"
#include "ivideo/vbufmgr.h"
#include "iengine/engine.h"
#include "iengine/material.h"
#include "iengine/camera.h"
#include "iengine/light.h"
#include "igeom/clip2d.h"
#include "iutil/cfgmgr.h"
#include "iutil/objreg.h"
#include "iutil/strset.h"

#include "meta.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE (csMetaBall)
  SCF_IMPLEMENTS_INTERFACE (iMeshObject)
#ifdef CS_USE_NEW_RENDERER
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iRenderBufferSource)
#endif
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObjectModel)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iMetaBallState)
#ifndef CS_USE_NEW_RENDERER
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iVertexBufferManagerClient)
#endif
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csMetaBall::ObjectModel)
  SCF_IMPLEMENTS_INTERFACE (iObjectModel)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csMetaBall::MetaBallState)
  SCF_IMPLEMENTS_INTERFACE (iMetaBallState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

#ifdef CS_USE_NEW_RENDERER
SCF_IMPLEMENT_EMBEDDED_IBASE (csMetaBall::RenderBufferSource)
  SCF_IMPLEMENTS_INTERFACE (iRenderBufferSource) 
SCF_IMPLEMENT_EMBEDDED_IBASE_END
#endif

#ifndef CS_USE_NEW_RENDERER
SCF_IMPLEMENT_EMBEDDED_IBASE (csMetaBall::eiVertexBufferManagerClient)
  SCF_IMPLEMENTS_INTERFACE (iVertexBufferManagerClient)
SCF_IMPLEMENT_EMBEDDED_IBASE_END
#endif

#define MAP_RESOLUTION  256
static float asin_table[2*MAP_RESOLUTION+1];

csMetaBall::csMetaBall (iMeshObjectFactory *fact)
{
  SCF_CONSTRUCT_IBASE (0);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObjectModel);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiMetaBallState);
#ifndef CS_USE_NEW_RENDERER
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiVertexBufferManagerClient);
#else
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiRenderBufferSource);
#endif



  logparent = 0;
  th = 0;
  alpha = frame = 0;
  meta_balls = 0;
  factory = fact;
  object_reg = 0;
  vis_cb = 0;
  do_lighting = false;
  initialize = false;
  cur_camera_num = 0;
  cur_movable_num = 0;
  MixMode = 0;
  rad = csVector3(1,1,1);
//-------------------------------------- Initialize metaball default values
  num_meta_balls = 3;
  max_vertices = 6000;
  env_map_mult = 1.0;
  env_mapping = TRUE_ENV_MAP;
  mp.iso_level = 1.0;
  mp.charge = 3.5;
  mp.rate = 0.03;
  current_lod = 1;
  current_features = 0;
#ifndef CS_USE_NEW_RENDERER
  vbufmgr = 0;
#endif
  num_mesh_vertices = 0;
}

csMetaBall::~csMetaBall ()
{
#ifndef CS_USE_NEW_RENDERER
  if (vbufmgr) vbufmgr->RemoveClient (&scfiVertexBufferManagerClient);
#endif

  if (vis_cb) vis_cb->DecRef ();
  delete [] meta_balls;
#ifndef CS_USE_NEW_RENDERER
  delete [] mesh.triangles;
  delete [] mesh.vertex_fog;
#endif
  delete [] mesh_vertices;
  delete [] mesh_texels;
  delete [] mesh_colors;
#ifdef CS_USE_NEW_RENDERER
  delete [] mesh_indices; 
#endif
  initialize = false;
}

float csMetaBall::map (float x)
{
  return asin_table[(int)(MAP_RESOLUTION*(1+x))];
}

void csMetaBall::InitTables(void)
{
  int i, j;
  for (i = -MAP_RESOLUTION, j = 0; i <= MAP_RESOLUTION; i++, j++)
  {
    float c = 1.0 * i / MAP_RESOLUTION;
    switch(env_mapping)
    {
      case TRUE_ENV_MAP:
        asin_table[j] = env_map_mult * (0.5f + asin(c) / PI);
        break;
      case FAKE_ENV_MAP:
        asin_table[j] = 0.5f * env_map_mult * (1 + c);
        break;
    }
  }
}

bool csMetaBall::Initialize (iObjectRegistry* object_reg)
{
  if (!initialize)
  {
    csMetaBall::object_reg = object_reg;
    initialize = true;
    meta_balls = new MetaBall[num_meta_balls];
#ifndef CS_USE_NEW_RENDERER
    memset(&mesh,0,sizeof(G3DTriangleMesh));

    SetupVertexBuffer ();
    mesh.num_vertices_pool = 1;
    mesh.triangles = new csTriangle[int(max_vertices/3)];
    mesh.vertex_fog = new G3DFogInfo[max_vertices];
#endif
    mesh_vertices = new csVector3[max_vertices];
    mesh_texels = new csVector2[max_vertices];
    mesh_colors = new csColor[max_vertices];
    mesh_indices = new csTriangle[int(max_vertices/3)];

    int i;
    for (i = 0; i < max_vertices; i++)
    {
      mesh_vertices[i].Set(0,0,0);
      mesh_texels[i].Set(0,0);
      mesh_colors[i].Set(1,1,1);
    }
    InitTables();
    
#ifndef CS_USE_NEW_RENDERER
    mesh.do_mirror = false;
    mesh.do_fog = false;
    mesh.do_morph_texels = false;
    mesh.do_morph_colors = false;
    mesh.vertex_mode = G3DTriangleMesh::VM_WORLDSPACE;
#endif
    NextFrame(0,csVector3(0,0,0));

#ifdef CS_USE_NEW_RENDERER
    g3d = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
    csRef<iStringSet> strings = CS_QUERY_REGISTRY_TAG_INTERFACE (object_reg, 
                      "crystalspace.renderer.stringset", iStringSet);

    vertex_name = strings->Request ("vertices");
    texel_name = strings->Request ("texture coordinates");
    color_name = strings->Request ("colors");
    index_name = strings->Request ("indices");

    bool rndbuf_verts_dirty = true;
    bool rndbuf_texels_dirty = true;
    bool rndbuf_colors_dirty = true;
    bool rndbuf_index_dirty = true;
#endif
  }
  return true;
}

#ifndef CS_USE_NEW_RENDERER
void csMetaBall::SetupVertexBuffer ()
{

 if (!vbuf)
 {
   csRef<iGraphics3D> g3d (CS_QUERY_REGISTRY (object_reg, iGraphics3D));
   // @@@ priority should be a parameter.
   vbufmgr = g3d->GetVertexBufferManager ();
   vbuf = vbufmgr->CreateBuffer (0);
   vbufmgr->AddClient (&scfiVertexBufferManagerClient);
   mesh.buffers[0] = vbuf;
 }


}
#endif


void csMetaBall::SetMetaBallCount (int number)
{
  if (number < 1 || number  == num_meta_balls)
    return;
  num_meta_balls = number;
  delete [] meta_balls;
  meta_balls = new MetaBall [num_meta_balls];

}

void csMetaBall::SetQualityEnvironmentMapping (bool toggle)
{
  env_mapping = toggle ? TRUE_ENV_MAP : FAKE_ENV_MAP;
  InitTables ();
}

void csMetaBall::SetEnvironmentMappingFactor (float env_mult)
{
  env_map_mult = env_mult;
  InitTables ();
}

#if 0
static void LitVertex( const csVector3 &n, csColor &c )
{
  if (n.z > 0)
    c.red = c.green = c.blue = 0;
  else
  {
    float l = n.z*n.z;
    c.red = c.green = c.blue = l;
  }
}
#endif

csRenderMesh** csMetaBall::GetRenderMeshes (int &n)
{
#ifdef CS_USE_NEW_RENDERER
  n = 1;
     
  meshPtr = &mesh;
  return &meshPtr;
#else
  n = 0;
  return 0;
#endif
}


#ifdef CS_USE_NEW_RENDERER
iRenderBuffer *csMetaBall::GetRenderBuffer (csStringID name)
{
  /*
  csRef<iRenderBuffer> rndbuf_verts;
  csRef<iRenderBuffer> rndbuf_texels;
  csRef<iRenderBuffer> rndbuf_colors;
  csRef<iRenderBuffer> rndbuf_index;
  */

  if (name == vertex_name)
  {
    if (!rndbuf_verts.IsValid())
    {
      rndbuf_verts = g3d->CreateRenderBuffer(
        sizeof (csVector3)*max_vertices, 
        CS_BUF_DYNAMIC, 
        CS_BUFCOMP_FLOAT, 3, false);

      rndbuf_verts_dirty = true;
    }

    if(rndbuf_verts_dirty)
    {
      csVector3* vbuf = (csVector3*)rndbuf_verts->Lock(CS_BUF_LOCK_NORMAL);

      if(vbuf)
        memcpy(vbuf, mesh_vertices, sizeof (csVector3)*num_mesh_vertices);

      rndbuf_verts->Release();
      rndbuf_verts_dirty = false;
    }
    return rndbuf_verts;
  }

  if (name == texel_name)
  {
    if (!rndbuf_texels.IsValid())
    {
      rndbuf_texels = g3d->CreateRenderBuffer(
        sizeof (csVector2)*max_vertices, 
        CS_BUF_DYNAMIC,
        CS_BUFCOMP_FLOAT, 2, false);

      rndbuf_texels_dirty = true;

    }

    if(rndbuf_texels_dirty)
    {
      csVector2* vbuf = (csVector2*)rndbuf_texels->Lock(CS_BUF_LOCK_NORMAL);

      if(vbuf)
        memcpy(vbuf, mesh_texels, sizeof (csVector2)*num_mesh_vertices);

      rndbuf_texels->Release();
      rndbuf_texels_dirty = false;
    }
    return rndbuf_texels;
  }

  if (name == color_name)
  {
    if (!rndbuf_colors.IsValid())
    {
      rndbuf_colors = g3d->CreateRenderBuffer(
        sizeof (csColor)*max_vertices, 
        CS_BUF_DYNAMIC,
        CS_BUFCOMP_FLOAT, 3, false);
      
      rndbuf_colors_dirty = true;
      
    }

    if(rndbuf_colors_dirty)
    {
      csColor* vbuf = (csColor*)rndbuf_colors->Lock(CS_BUF_LOCK_NORMAL);

      memset(mesh_colors, 0xff, sizeof (csColor)*num_mesh_vertices);
      if (vbuf)
        memcpy (vbuf, mesh_colors, sizeof (csColor)*num_mesh_vertices);
      
      rndbuf_colors->Release ();
      rndbuf_colors_dirty = false;
    }

    return rndbuf_colors;
  }
  if (name == index_name)
  {
    if (!rndbuf_index.IsValid())
    {
      rndbuf_index = g3d->CreateRenderBuffer(
        sizeof (csTriangle)*int(max_vertices / 3), 
        CS_BUF_DYNAMIC,
        CS_BUFCOMP_UNSIGNED_INT,
        1, true);

      rndbuf_index_dirty = true;
    }

    if(rndbuf_index_dirty)
    {
      unsigned int * vbuf = (unsigned int*)rndbuf_index->Lock(CS_BUF_LOCK_NORMAL);

      if (vbuf)
        memcpy (vbuf, mesh_indices, sizeof (csTriangle)*num_mesh_triangles);

      rndbuf_index->Release ();
      rndbuf_index_dirty = false;
    }
    return rndbuf_index;
  }
  return 0;
}

#endif

bool csMetaBall::DrawTest( iRenderView* rview, iMovable* movable)
{
// This is basically a ripped of version of drawtest from the
// ball mesh object.
#ifndef CS_USE_NEW_RENDERER
  iGraphics3D *g3d = rview->GetGraphics3D();
  iCamera *cam = rview->GetCamera();
  csReversibleTransform tr_o2c = cam->GetTransform();
  if (!movable->IsFullTransformIdentity ())
    tr_o2c /= movable->GetFullTransform ();
  float fov = cam->GetFOV();
  float shftx = cam->GetShiftX();
  float shfty = cam->GetShiftY();
  csBox2 sbox;
  csBox3 cbox;
  if ( GetScreenBoundingBox( cam->GetCameraNumber(),
	movable->GetUpdateNumber(), fov, shftx,shfty, tr_o2c, sbox, cbox ) < 0)
	return false;
  int clip_portal, clip_plane, clip_z_plane;
  if (rview->ClipBBox( sbox,cbox, clip_portal, clip_plane,
  	clip_z_plane) == false )
    return false;
  g3d->SetObjectToCamera( &tr_o2c );
  mesh.clip_portal = clip_portal;
  mesh.clip_plane = clip_plane;
  mesh.clip_z_plane = clip_z_plane;
  mesh.do_mirror = cam->IsMirrored();
  return true;

#else

  iGraphics3D * g3d = rview->GetGraphics3D();
  iCamera *cam = rview->GetCamera();

  // Shouldn't this be done in the renderer?
  tr_o2c = cam->GetTransform ();
  if (!movable->IsFullTransformIdentity ())
    tr_o2c /= movable->GetFullTransform ();

  int clip_portal, clip_plane, clip_z_plane;
  csVector3 radius;
  csSphere sphere;
  GetRadius (radius, sphere.GetCenter ());
  float max_radius = radius.x;
  if (max_radius < radius.y) max_radius = radius.y;
  if (max_radius < radius.z) max_radius = radius.z;
  sphere.SetRadius (max_radius);
  if (rview->ClipBSphere (tr_o2c, sphere, clip_portal, clip_plane,
  	clip_z_plane) == false)
    return false;

#ifndef CS_USE_NEW_RENDERER
  g3d->SetObjectToCamera( &tr_o2c );
#endif
 
  mesh.material = th;
  mesh.indexstart = 0;
  mesh.indexend = num_mesh_triangles * 3;
  mesh.meshtype = CS_MESHTYPE_TRIANGLES;
  mesh.buffersource = &scfiRenderBufferSource;
  mesh.object2camera = tr_o2c;

  mesh.clip_portal = clip_portal;
  mesh.clip_plane = clip_plane;
  mesh.clip_z_plane = clip_z_plane;
  mesh.do_mirror = cam->IsMirrored();
 
  return true;


#endif
}

void csMetaBall::UpdateLighting( iLight **, int, iMovable * )
{
  //TODO: The lighting code is incomplete...
#if 0
      csVector3 n(0, 0, 0);
	  int k;
      for(k = 0; k < num_meta_balls; k++)
      {
        csVector3 rv(mesh_vertices[m].x - meta_balls[k].center.x,
          mesh_vertices[m].y - meta_balls[k].center.y,
          mesh_vertices[m].z - meta_balls[k].center.z);

        float r = rv.Norm();
        float c = mp.charge / (r*r*r);

        n += rv * c;
      }
      n = n.Unit();
	  LitVertex ( n, mesh.vertex_colors[0][m]);
#endif
}

void csMetaBall::GetObjectBoundingBox(csBox3& bbox, int )
{
  bbox = object_bbox;
}

static void Perspective( const csVector3& v, csVector2& p, float fov,
		float sx, float sy )
{
  float iz = fov / v.z;
  p.x = v.x * iz + sx;
  p.y = v.y * iz + sy;
}

void csMetaBall::CreateBoundingBox()
{
  float minx, maxx, miny, maxy, minz, maxz;
  minx = maxx = meta_balls[0].center.x;
  miny = maxy = meta_balls[0].center.y;
  minz = maxz = meta_balls[0].center.z;
  csBox3 bb;

  int maxxi = 0, maxyi = 0, maxzi = 0, minxi = 0, minyi = 0, minzi = 0;

  int i;
  for (i = 0; i < num_mesh_vertices; i++)
  {
	if ( mesh_vertices[i].x < minx ) {minx = mesh_vertices[i].x; minxi = i; }
	else
	if ( mesh_vertices[i].x > maxx ) {maxx = mesh_vertices[i].x; maxxi = i; }

	if ( mesh_vertices[i].y < miny ) {miny = mesh_vertices[i].y; minyi = i; }
	else
	if ( mesh_vertices[i].y > maxy ) {maxy = mesh_vertices[i].y; maxyi = i; }

	if ( mesh_vertices[i].z < minz ) {minz = mesh_vertices[i].z; minzi = i; }
	else
	if ( mesh_vertices[i].z > maxz ) {maxz = mesh_vertices[i].z; maxzi = i; }
  }
  bb.StartBoundingBox( csVector3(minx,miny,minz));
  bb.AddBoundingVertexSmart( csVector3(maxx,maxy,maxz));
  if ( !object_bbox.Contains(bb))
  {
    scfiObjectModel.ShapeChanged ();
  }
  object_bbox = bb;
  rad.Set(object_bbox.Max() - object_bbox.GetCenter());

#if 0
  printf("Minimum bounding found at %d,%d,%d\n", minxi, minyi, minzi);
  printf("Maximum bounding found at %d,%d,%d\n", maxxi, maxyi, maxzi);
  printf("Minimum bounding vertex at (%f,%f,%f)\n", minx, miny, minz);
  printf("Maximum bounding vertex at (%f,%f,%f)\n", maxx, maxy, maxz);
  printf("Vertex zero : (%f,%f,%f)\n",mesh_vertices[0].x, mesh_vertices[0].y, mesh_vertices[0].z);
  printf("Num verts : %d\n", num_mesh_vertices);
#endif

}

float csMetaBall::GetScreenBoundingBox( long cam_num, long mov_num,
		float fov, float sx, float sy, const csReversibleTransform& trans,
		csBox2& sbox, csBox3& cbox )
{
  csVector2 one_corner;
  GetTransformedBoundingBox( cam_num, mov_num, trans, cbox );
  if ((cbox.MinZ() < 0) && (cbox.MaxZ() < 0))
	return -1.0;
  if (cbox.MinZ() <= 0)
	sbox.Set(-10000,-10000,10000,10000);
  else
  {
	Perspective( cbox.Max(), one_corner, fov, sx, sy );
	sbox.StartBoundingBox( one_corner );
	csVector3 v ( cbox.MinX(), cbox.MinY(), cbox.MaxZ());
	Perspective( v, one_corner, fov, sx, sy );
	sbox.AddBoundingVertexSmart( one_corner );
	v.Set( cbox.MaxX(), cbox.MaxY(), cbox.MinZ());
	Perspective( v, one_corner, fov, sx, sy );
	sbox.AddBoundingVertexSmart( one_corner );
  }
  return cbox.MaxZ();
}
void csMetaBall::GetTransformedBoundingBox( long cam_num, long move_num,
		const csReversibleTransform& trans, csBox3& cbox)
{
  if ((cur_camera_num == cam_num) && (cur_movable_num == move_num))
  {
	cbox = camera_bbox;
	return;
  }
  cur_camera_num = cam_num;
  cur_movable_num = move_num;
  camera_bbox.StartBoundingBox( trans * object_bbox.Min());
  camera_bbox.AddBoundingVertexSmart( trans * object_bbox.Max());
  cbox = camera_bbox;
}

bool csMetaBall::HitBeamOutline (const csVector3& start, const csVector3& end,
  csVector3& isect, float* pr)
{
  csSegment3 seg (start, end);
  int i, max = int(vertices_tesselated/3);
  for (i = 0 ; i < max ; i++)
  {
    if (csIntersect3::IntersectTriangle (mesh_vertices[i+2], mesh_vertices[i+1],
    	mesh_vertices[i], seg, isect))
    {
      if (pr)
      {
        *pr = qsqrt (csSquaredDist::PointPoint (start, isect) /
		csSquaredDist::PointPoint (start, end));
      }
      return true;
    }
  }
  return false;
}

bool csMetaBall::HitBeamObject( const csVector3& start, const csVector3& end,
  csVector3& isect, float *pr, int* polygon_idx)
{
  if (polygon_idx) *polygon_idx = -1;
  int i, max = int(vertices_tesselated/3);
  csSegment3 seg (start, end);
  csVector3 tmp;
  float dist, temp;
  float tot_dist = csSquaredDist::PointPoint(start, end);
  dist = temp = tot_dist;
  float itot_dist = 1. / tot_dist;
  for (i = 0 ; i < max ; i++)
  {
    if (csIntersect3::IntersectTriangle (mesh_vertices[i+2], mesh_vertices[i+1],
    	mesh_vertices[i], seg, tmp))
    {
      temp = csSquaredDist::PointPoint (start, tmp);
      if (temp < dist)
      {
        dist = temp;
	isect = tmp;
      }
    }
  }
  if (pr) *pr = qsqrt (dist * itot_dist);
  if (dist >= tot_dist)
    return false;
  return true;
}

void csMetaBall::NextFrame(csTicks, const csVector3& /*pos*/)
{
  if ((num_mesh_vertices == 0) || (mp.rate != 0))
  {
  int i,j,m;
  float l = ( do_lighting ) ? 1.0 : 0.0;
  alpha += mp.rate;
  num_mesh_vertices = 0;
#ifndef CS_USE_NEW_RENDERER
  mesh.num_triangles = 0;
#endif
//-------------------------------- MetaBall - Set position -------
// Below is the generic position calculation that was supplied with
// metaballs by Denis Dmitriev. In future I will be looking at replacing
// this method with a definable skeletal animation method.

#if 1
  for (i = 0; i < num_meta_balls; i++)
  {
    float m = fmod((i + 1) / 3.0f, 1.5f) + 0.5f;

    csVector3 &c = meta_balls[i].center;
    c.x = 4 * m * sin (m * alpha + i * PI / 4);
    c.y = 4 * m * cos (1.4f * m * alpha + m * PI / 6);
    c.z = 4 * sin (m * alpha * 1.3214f);

//  printf("Ball %d, (%f,%f,%f)\n",i, c.x,c.y,c.z);
  }
#endif

//----------------------------------------------------------------
// Calculate meta balls is the core of the meta ball engine.
// Everything in this module is basically housekeeping routines,
// except for InitTables, which sets up the correct tables for the
// resolution used.

  CalculateMetaBalls();

//----------------------------------------------------------------
// The next routine sets up the triangles and the texturing for
// the mesh...
  int num = 0, trigs = int( vertices_tesselated / 3);
  int k;
  for (i = 0; i < trigs; i++, num += 3)
  {
	// Fill our mesh buffer with the created triangles
#ifndef CS_USE_NEW_RENDERER
	mesh.triangles[i].a = num + 2;
	mesh.triangles[i].b = num + 1;
	mesh.triangles[i].c = num;
#else
        mesh_indices[i].a = num + 2;
        mesh_indices[i].b = num + 1;
        mesh_indices[i].c = num;
#endif

	for (j = 0; j < 3; j++)
	{
	  m = num + j;
      csVector3 n(0, 0, 0);
      for(k = 0; k < num_meta_balls; k++)
      {
      	csVector3 rv(mesh_vertices[m].x - meta_balls[k].center.x,
          mesh_vertices[m].y - meta_balls[k].center.y,
          mesh_vertices[m].z - meta_balls[k].center.z);

        float r = rv.Norm();
        float c = mp.charge / (r*r*r);

        n += rv * c;
      }
      n = n.Unit();
	  mesh_texels[m].x = map (n.x);
	  mesh_texels[m].y = map (n.y);
	  csColor &c = mesh_colors[m];
	  c.red = c.blue = c.green = l; // Default lighting. Not real perty yet.
	}
  }
#ifndef CS_USE_NEW_RENDERER
  mesh.num_triangles = trigs;
#else
  num_mesh_triangles = trigs;
  rndbuf_index_dirty = true;
  rndbuf_verts_dirty = true;

#endif
  num_mesh_vertices = vertices_tesselated;
  CreateBoundingBox();
  } // If Rate != 0
}

void csMetaBall::HardTransform( const csReversibleTransform& )
{
  printf("Hard transform\n");
}

bool csMetaBall::Draw( iRenderView* rview, iMovable* /* movable */,
		csZBufMode mode )
{
  if (!th)
  {
	printf("csMetaBall: Draw aborted, no material applied to this object\n");
	return false;
  }
#ifndef CS_USE_NEW_RENDERER
  mesh.mat_handle = th->GetMaterialHandle();
  mesh.use_vertex_color = true;
#else
  mesh.material = th;
#endif

  

#if 0
  int i;
  printf("Dumping Triangles: %d Vertices: %d\n", mesh.num_triangles, num_mesh_vertices );
  for (i = 0; i < num_mesh_vertices; i++)
  {
	csVector3 v = mesh_vertices[i];
	printf("V(%f,%f,%f) ; %d\n", v.x,v.y,v.z, i);
  }
  for (i = 0; i < mesh.num_triangles; i++)
  {
	csTriangle tr = mesh.triangles[i];
	printf("TRIANGLE(%d,%d,%d) ; %d\n",tr.a,tr.b,tr.c,i);
  }
  printf("Finished dump =============================================\n");
#endif

  if (vis_cb) if (!vis_cb->BeforeDrawing ( this, rview )) return false;

#ifndef CS_USE_NEW_RENDERER
  iGraphics3D* G3D = rview->GetGraphics3D();
  G3D->SetRenderState (G3DRENDERSTATE_ZBUFFERMODE, mode);
  SetupVertexBuffer ();
  mesh.mixmode = MixMode | CS_FX_GOURAUD;
  CS_ASSERT (!vbuf->IsLocked ());
  vbufmgr->LockBuffer (vbuf, mesh_vertices, mesh_texels,
  	mesh_colors, num_mesh_vertices, 0, object_bbox);
  rview->CalculateFogMesh (G3D->GetObjectToCamera (), mesh);
  G3D->DrawTriangleMesh(mesh);
  vbufmgr->UnlockBuffer (vbuf);
  return true;
#else
 /* iGraphics3D* r3D = rview->GetGraphics3D();
  csRenderMesh::
  mesh.indexstart = 0;
  mesh.indexend = num_mesh_triangles * 3;
  mesh.material = th;
  mesh.buffersource = &this->scfiRenderBufferSource;
  mesh.tMESHTYPE_TRIANGLES);
  r3D->DrawMesh (&mesh);*/
  return true;
#endif
}

#ifndef CS_USE_NEW_RENDERER
void csMetaBall::eiVertexBufferManagerClient::ManagerClosing ()
{
  if (scfParent->vbuf)
  {
    scfParent->vbuf = 0;
    scfParent->vbufmgr = 0;
  }
}
#endif

SCF_IMPLEMENT_IBASE(csMetaBallFactory)
  SCF_IMPLEMENTS_INTERFACE(iMeshObjectFactory)
SCF_IMPLEMENT_IBASE_END

csMetaBallFactory::csMetaBallFactory( iBase *par, iObjectRegistry* object_reg )
{
  SCF_CONSTRUCT_IBASE(par);
  csMetaBallFactory::object_reg = object_reg;
  logparent = 0;
}

csMetaBallFactory::~csMetaBallFactory()
{
}

csPtr<iMeshObject> csMetaBallFactory::NewInstance()
{
  csMetaBall* cm = new csMetaBall((iMeshObjectFactory *) this);
  cm->Initialize(object_reg);
  csRef<iMeshObject> im (SCF_QUERY_INTERFACE (cm, iMeshObject));
  cm->DecRef ();
  return csPtr<iMeshObject> (im);
}

SCF_IMPLEMENT_IBASE (csMetaBallType)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectType)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csMetaBallType::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csMetaBallType)


csMetaBallType::csMetaBallType( iBase *par )
{
  SCF_CONSTRUCT_IBASE (par);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csMetaBallType::~csMetaBallType()
{
}

csPtr<iMeshObjectFactory> csMetaBallType::NewFactory()
{
  csMetaBallFactory* cm = new csMetaBallFactory(this, object_reg);
  csRef<iMeshObjectFactory> ifact (
  	SCF_QUERY_INTERFACE(cm, iMeshObjectFactory));
  cm->DecRef ();
  return csPtr<iMeshObjectFactory> (ifact);
}

