/*
    Copyright (C) 2001 by Jorrit Tyberghein

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
#include "surf.h"
#include "iengine/movable.h"
#include "iengine/rview.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "ivideo/material.h"
#include "iengine/material.h"
#include "iengine/camera.h"
#include "igeom/clip2d.h"
#include "iengine/engine.h"
#include "iengine/light.h"
#include "qsqrt.h"

//#define SURF_DEBUG

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE (csSurfMeshObject)
  SCF_IMPLEMENTS_INTERFACE (iMeshObject)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iSurfaceState)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSurfMeshObject::SurfaceState)
  SCF_IMPLEMENTS_INTERFACE (iSurfaceState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csSurfMeshObject::csSurfMeshObject (iMeshObjectFactory* factory)
{
  SCF_CONSTRUCT_IBASE (NULL);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSurfaceState);
  csSurfMeshObject::factory = factory;
  initialized = false;
  cur_cameranr = -1;
  cur_movablenr = -1;
  xres = yres = 4;
  topleft.Set (-.5, -.5, 0);
  xscale = yscale = 1;
  material = NULL;
  MixMode = 0;
  vis_cb = NULL;
  mesh.vertices[0] = NULL;
  mesh.vertex_colors[0] = NULL;
  mesh.texels[0] = NULL;
  mesh.triangles = NULL;
  mesh.vertex_fog = NULL;
  corner[0] = corner[1] = corner[2] = corner[3] = csVector3(0,0,0);
  shapenr = 0;
  do_lighting = true;
  color.red = 0;
  color.green = 0;
  color.blue = 0;
  current_lod = 1;
  current_features = ALL_FEATURES;
}

csSurfMeshObject::~csSurfMeshObject ()
{
  if (vis_cb) vis_cb->DecRef ();
  delete[] mesh.vertices[0];
  delete[] mesh.vertex_colors[0];
  delete[] mesh.texels[0];
  delete[] mesh.triangles;
  delete[] mesh.vertex_fog;
}

void csSurfMeshObject::GetTransformedBoundingBox (long cameranr,
	long movablenr, const csReversibleTransform& trans, csBox3& cbox)
{
  if (cur_cameranr == cameranr && cur_movablenr == movablenr)
  {
    cbox = camera_bbox;
    return;
  }
  cur_cameranr = cameranr;
  cur_movablenr = movablenr;
  camera_bbox.StartBoundingBox (trans * corner[0]);
  camera_bbox.AddBoundingVertexSmart (trans * corner[1]);
  camera_bbox.AddBoundingVertexSmart (trans * corner[2]);
  camera_bbox.AddBoundingVertexSmart (trans * corner[3]);
  cbox = camera_bbox;
}

static void Perspective (const csVector3& v, csVector2& p, float fov,
    	float sx, float sy)
{
  float iz = fov / v.z;
  p.x = v.x * iz + sx;
  p.y = v.y * iz + sy;
}

float csSurfMeshObject::GetScreenBoundingBox (long cameranr,
      long movablenr, float fov, float sx, float sy,
      const csReversibleTransform& trans, csBox2& sbox, csBox3& cbox)
{
  csVector2 oneCorner;

  GetTransformedBoundingBox (cameranr, movablenr, trans, cbox);

  // if the entire bounding box is behind the camera, we're done
  if ((cbox.MinZ () < 0) && (cbox.MaxZ () < 0))
  {
    return -1;
  }

  // Transform from camera to screen space.
  if (cbox.MinZ () <= 0)
  {
    // Sprite is very close to camera.
    // Just return a maximum bounding box.
    sbox.Set (-10000, -10000, 10000, 10000);
  }
  else
  {
    Perspective (cbox.Max (), oneCorner, fov, sx, sy);
    sbox.StartBoundingBox (oneCorner);
    csVector3 v (cbox.MinX (), cbox.MinY (), cbox.MaxZ ());
    Perspective (v, oneCorner, fov, sx, sy);
    sbox.AddBoundingVertexSmart (oneCorner);
    Perspective (cbox.Min (), oneCorner, fov, sx, sy);
    sbox.AddBoundingVertexSmart (oneCorner);
    v.Set (cbox.MaxX (), cbox.MaxY (), cbox.MinZ ());
    Perspective (v, oneCorner, fov, sx, sy);
    sbox.AddBoundingVertexSmart (oneCorner);
  }

  return cbox.MaxZ ();
}

void csSurfMeshObject::GenerateSurface (G3DTriangleMesh& mesh)
{
  csVector3* vertices = new csVector3[10000];	// Temporary only
  csVector2* uvverts = new csVector2[10000];
  csTriangle* triangles = new csTriangle[10000];

  float rx, ry, u, v;
  int x, y;
  int num_vertices = 0;
  for (y = 0 ; y <= yres ; y++)
  {
    v = float (y) / float (yres);
    ry = topleft.y + yscale * v;
    for (x = 0 ; x <= xres ; x++)
    {
      u = float (x) / float (xres);
      rx = topleft.x + xscale * u;
      vertices[num_vertices].Set (rx, ry, topleft.z);
      uvverts[num_vertices].Set (u, v);
      num_vertices++;
    }
  }
  // Using the last values used in loop
  corner[0] = topleft;
  corner[1] = csVector3( topleft.x, ry, topleft.z);
  corner[2] = csVector3( rx, ry, topleft.z);
  corner[3] = csVector3( rx, topleft.y, topleft.z);

  int num_triangles = 0;
  int i = 0;
  for (y = 0 ; y < yres ; y++)
  {
    for (x = 0 ; x < xres ; x++, i++)
    {
      triangles[num_triangles].c = i;
      triangles[num_triangles].b = i+1;
      triangles[num_triangles].a = i+xres+1;
      num_triangles++;
      triangles[num_triangles].c = i+1;
      triangles[num_triangles].b = i+xres+1+1;
      triangles[num_triangles].a = i+xres+1;
      num_triangles++;
    }
    i++;
  }

  // Setup the mesh and normal array.
  mesh.num_vertices = num_vertices;
  mesh.vertices[0] = new csVector3[num_vertices];
  memcpy (mesh.vertices[0], vertices, sizeof(csVector3)*num_vertices);
  mesh.texels[0] = new csVector2[num_vertices];
  memcpy (mesh.texels[0], uvverts, sizeof(csVector2)*num_vertices);
  mesh.vertex_colors[0] = new csColor[num_vertices];
  csColor* colors = mesh.vertex_colors[0];
  for (i = 0 ; i < num_vertices ; i++)
    colors[i].Set (1, 1, 1);
  mesh.vertex_fog = new G3DFogInfo[num_vertices];
  mesh.num_triangles = num_triangles;
  mesh.triangles = new csTriangle[num_triangles];
  memcpy (mesh.triangles, triangles, sizeof(csTriangle)*num_triangles);

  delete[] vertices;
  delete[] uvverts;
  delete[] triangles;
}

void csSurfMeshObject::RecalcObjectBBox ()
{
  object_bbox.StartBoundingBox (corner[0]);
  int i;
  for (i = 1 ; i < 3 ; i++)
    object_bbox.AddBoundingVertexSmart (corner[i]);
  float xx = object_bbox.MaxX ()-object_bbox.MinX (); xx /= 2.;
  float yy = object_bbox.MaxY ()-object_bbox.MinY (); yy /= 2.;
  float zz = object_bbox.MaxZ ()-object_bbox.MinZ (); zz /= 2.;
  max_radius.Set (qsqrt (xx*xx+xx*xx), qsqrt (yy*yy+yy*yy),
  	qsqrt (zz*zz+zz*zz));
}

void csSurfMeshObject::RecalcSurfaceNormal ()
{
  csVector3 v1 = mesh.vertices[0][1] - mesh.vertices[0][0];
  csVector3 v2 = mesh.vertices[0][xres+1] - mesh.vertices[0][0];
  surface_normal = v1 % v2;
  surface_normal.Normalize ();
}

void csSurfMeshObject::SetupObject ()
{
  if (!initialized)
  {
    initialized = true;
    delete[] mesh.vertices[0];
    delete[] mesh.vertex_colors[0];
    delete[] mesh.texels[0];
    delete[] mesh.triangles;
    delete[] mesh.vertex_fog;
    mesh.vertices[0] = NULL;
    mesh.vertex_colors[0] = NULL;
    mesh.texels[0] = NULL;
    mesh.triangles = NULL;
    mesh.vertex_fog = NULL;

    GenerateSurface (mesh);
    RecalcObjectBBox ();
    RecalcSurfaceNormal ();
    mesh.morph_factor = 0;
    mesh.num_vertices_pool = 1;
    mesh.do_morph_texels = false;
    mesh.do_morph_colors = false;
    mesh.vertex_mode = G3DTriangleMesh::VM_WORLDSPACE;
  }
}

bool csSurfMeshObject::DrawTest (iRenderView* rview, iMovable* movable)
{
  SetupObject ();
  iGraphics3D* g3d = rview->GetGraphics3D ();
  iCamera* camera = rview->GetCamera ();

  // First create the transformation from object to camera space directly:
  //   W = Mow * O - Vow;
  //   C = Mwc * (W - Vwc)
  // ->
  //   C = Mwc * (Mow * O - Vow - Vwc)
  //   C = Mwc * Mow * O - Mwc * (Vow + Vwc)
  csReversibleTransform tr_o2c = camera->GetTransform ()
    	* movable->GetFullTransform ().GetInverse ();
  float fov = camera->GetFOV ();
  float shiftx = camera->GetShiftX ();
  float shifty = camera->GetShiftY ();

  // Test visibility of entire ball by clipping bounding box against clipper.
  // There are three possibilities:
  //	1. box is not visible -> ball is not visible.
  //	2. box is entirely visible -> ball is visible and need not be clipped.
  //	3. box is partially visible -> ball is visible and needs to be clipped
  //	   if rview has do_clip_plane set to true.
  csBox2 sbox;
  csBox3 cbox;
  if (GetScreenBoundingBox (camera->GetCameraNumber (),
  	movable->GetUpdateNumber (), fov, shiftx, shifty,
  	tr_o2c, sbox, cbox) < 0)
    return false;
  int clip_portal, clip_plane, clip_z_plane;
  if (rview->ClipBBox (sbox, cbox, clip_portal, clip_plane,
  	clip_z_plane) == false)
    return false;

  iClipper2D* clipper; clipper = rview->GetClipper ();
  g3d->SetObjectToCamera (&tr_o2c);
  mesh.clip_portal = clip_portal;
  mesh.clip_plane = clip_plane;
  mesh.clip_z_plane = clip_z_plane;
  mesh.do_mirror = camera->IsMirrored ();
  return true;
}

void csSurfMeshObject::UpdateLighting (iLight** lights, int num_lights,
    iMovable* movable)
{
  SetupObject ();

  int i, l;
  csColor* colors = mesh.vertex_colors[0];

  // Set all colors to ambient light (@@@ NEED TO GET AMBIENT!)
  for (i = 0 ; i < mesh.num_vertices ; i++)
    colors[i] = color;
  if (!do_lighting) return;
    // @@@ it is not effiecient to do this all the time.

  // Do the lighting.
  csVector3 obj_center (0);
  csReversibleTransform trans = movable->GetFullTransform ();
  csVector3 wor_center = trans.This2Other (obj_center);
  csColor color;
  for (l = 0 ; l < num_lights ; l++)
  {
    iLight* li = lights[l];
    // Compute light position in object coordinates
    csVector3 wor_light_pos = li->GetCenter ();
    float wor_sq_dist = csSquaredDist::PointPoint (wor_light_pos, wor_center);
    if (wor_sq_dist >= li->GetSquaredRadius ()) continue;

    csVector3 obj_light_pos = trans.Other2This (wor_light_pos);
    float obj_sq_dist = csSquaredDist::PointPoint (obj_light_pos, obj_center);
    float in_obj_dist = qisqrt (obj_sq_dist);

    csVector3 obj_light_dir = (obj_light_pos - obj_center);

    csColor light_color = li->GetColor () * (256. / NORMAL_LIGHT_LEVEL)
      * li->GetBrightnessAtDistance (qsqrt (wor_sq_dist));

    csVector3 normal = surface_normal;
    float cosinus;
    if (obj_sq_dist < SMALL_EPSILON) cosinus = 1;
    else cosinus = obj_light_dir * normal;
    if (cosinus > 0)
    {
      color = light_color;
      if (obj_sq_dist >= SMALL_EPSILON) cosinus *= in_obj_dist;
      if (cosinus < 1) color *= cosinus;
      for (i = 0 ; i < mesh.num_vertices ; i++)
	colors[i] += color;
    }
  }

  // Clamp all vertex colors to 2.
  for (i = 0 ; i < mesh.num_vertices ; i++)
  {
    colors[i].ClampDown ();
    colors[i].Clamp (2., 2., 2.);
  }
}

bool csSurfMeshObject::Draw (iRenderView* rview, iMovable* /*movable*/,
	csZBufMode mode)
{
// @@@ TODO:
//     - Z fill vs Z use
  if (!material)
  {
    printf ("INTERNAL ERROR: ball used without material!\n");
    return false;
  }
  iMaterialHandle* mat = material->GetMaterialHandle ();
  if (!mat)
  {
    printf ("INTERNAL ERROR: ball used without valid material handle!\n");
    return false;
  }

  if (vis_cb) if (!vis_cb->BeforeDrawing (this, rview)) return false;

  iGraphics3D* g3d = rview->GetGraphics3D ();

  // Prepare for rendering.
  g3d->SetRenderState (G3DRENDERSTATE_ZBUFFERMODE, mode);

  material->Visit ();
  mesh.mat_handle = mat;
  mesh.use_vertex_color = true;
  mesh.fxmode = MixMode | CS_FX_GOURAUD;
  rview->CalculateFogMesh (g3d->GetObjectToCamera (), mesh);
  g3d->DrawTriangleMesh (mesh);

  return true;
}

void csSurfMeshObject::GetObjectBoundingBox (csBox3& bbox, int /*type*/)
{
  SetupObject ();
  bbox = object_bbox;
}

void csSurfMeshObject::HardTransform (const csReversibleTransform& t)
{
  SetupObject ();
  int i;
  for (i = 0 ; i < mesh.num_vertices ; i++)
    mesh.vertices[0][i] = t.This2Other (mesh.vertices[0][i]);
  for (i = 0; i < 4; i++ )
	corner[i] = t.This2Other(corner[i]);
  RecalcObjectBBox ();
  RecalcSurfaceNormal ();
  shapenr++;
}

bool csSurfMeshObject::HitBeamObject(const csVector3& start,
  const csVector3& end, csVector3& isect, float *pr)
{
  // The real corners of the surface are cached during generation to
  // improve performance.

  csSegment3 seg (start, end);
#ifdef SURF_DEBUG
  printf("Surface:Hit Beam Object\n");
  printf("Debug:\n");
  printf("Top: (%f,%f,%f) to (%f,%f,%f)\n",
	corner[0].x, corner[0].y, corner[0].z,
	corner[1].x, corner[1].y, corner[1].z);
  printf("Bottom: (%f,%f,%f) to (%f,%f,%f)\n",
	corner[2].x, corner[2].y, corner[2].z,
	corner[3].x, corner[3].y, corner[3].z);
#endif

    if (csIntersect3::IntersectTriangle (corner[0], corner[2], corner[1], seg, isect) 
	  || csIntersect3::IntersectTriangle (corner[0], corner[3], corner[2], seg, isect))
    {
      if (pr)
      {
        *pr = qsqrt (csSquaredDist::PointPoint (start, isect) /
		csSquaredDist::PointPoint (start, end));
      }
#ifdef SURF_DEBUG
      printf("Surface:Hit Beam Object: HIT! intersect : (%f,%f,%f)\n",
        isect.x, isect.y, isect.z);
#endif
      return true;
    }
  return false;
}


//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csSurfMeshObjectFactory)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectFactory)
SCF_IMPLEMENT_IBASE_END

csSurfMeshObjectFactory::csSurfMeshObjectFactory (iBase *pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
}

csSurfMeshObjectFactory::~csSurfMeshObjectFactory ()
{
}

iMeshObject* csSurfMeshObjectFactory::NewInstance ()
{
  csSurfMeshObject* cm = new csSurfMeshObject ((iMeshObjectFactory*)this);
  iMeshObject* im = SCF_QUERY_INTERFACE (cm, iMeshObject);
  im->DecRef ();
  return im;
}

//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csSurfMeshObjectType)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectType)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iPlugin)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSurfMeshObjectType::eiPlugin)
  SCF_IMPLEMENTS_INTERFACE (iPlugin)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csSurfMeshObjectType)

SCF_EXPORT_CLASS_TABLE (surf)
  SCF_EXPORT_CLASS (csSurfMeshObjectType, "crystalspace.mesh.object.surface",
    "Crystal Space Surface Mesh Type")
SCF_EXPORT_CLASS_TABLE_END

csSurfMeshObjectType::csSurfMeshObjectType (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiPlugin);
}

csSurfMeshObjectType::~csSurfMeshObjectType ()
{
}

iMeshObjectFactory* csSurfMeshObjectType::NewFactory ()
{
  csSurfMeshObjectFactory* cm = new csSurfMeshObjectFactory (this);
  iMeshObjectFactory* ifact = SCF_QUERY_INTERFACE (cm, iMeshObjectFactory);
  ifact->DecRef ();
  return ifact;
}
