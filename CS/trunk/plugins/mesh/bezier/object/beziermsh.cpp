/*
    Copyright (C) 1998-2002 by Jorrit Tyberghein

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
#include "csqint.h"
#include "csqsqrt.h"

#include "csgeom/frustum.h"
#include "csgeom/math3d.h"
#include "csgeom/poly3d.h"
#include "csgeom/polypool.h"
#include "csgeom/sphere.h"
#include "csgfx/renderbuffer.h"
#include "csutil/array.h"
#include "csutil/csendian.h"
#include "csutil/csmd5.h"
#include "csutil/csstring.h"
#include "csutil/debug.h"
#include "csutil/dirtyaccessarray.h"
#include "csutil/memfile.h"
#include "iengine/camera.h"
#include "iengine/engine.h"
#include "iengine/fview.h"
#include "iengine/light.h"
#include "iengine/material.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "iengine/rview.h"
#include "iengine/sector.h"
#include "iengine/shadcast.h"
#include "iengine/shadows.h"
#include "iengine/texture.h"
#include "iutil/cache.h"
#include "iutil/comp.h"
#include "iutil/databuff.h"
#include "iutil/eventh.h"
#include "iutil/verbositymanager.h"
#include "iutil/vfs.h"
#include "ivaria/reporter.h"
#include "ivideo/graph3d.h"
#include "ivideo/rendermesh.h"
#include "ivideo/texture.h"
#include "ivideo/txtmgr.h"

#include "lightpool.h"
#include "curvebase.h"
#include "beziermsh.h"

CS_IMPLEMENT_PLUGIN

//---------------------------------------------------------------------------

CS_PLUGIN_NAMESPACE_BEGIN(Bezier)
{

int csBezierMesh:: last_thing_id = 0;

//----------------------------------------------------------------------------

csBezierMeshStatic::csBezierMeshStatic (
	csBezierMeshObjectType* thing_type,
	iBezierFactoryState* factory_state)
{
  csBezierMeshStatic::thing_type = thing_type;
  csBezierMeshStatic::factory_state = factory_state;

  curves_center.x = curves_center.y = curves_center.z = 0;
  curves_scale = 40;
  curve_vertices = 0;
  curve_texels = 0;
  num_curve_vertices = max_curve_vertices = 0;

  obj_bbox_valid = false;

  static_data_nr = 1;
  prepared = false;
}

csBezierMeshStatic::~csBezierMeshStatic ()
{
  delete[] curve_vertices;
  delete[] curve_texels;
}

void csBezierMeshStatic::Prepare ()
{
  prepared = true;
}

//----------------------------------------------------------------------------

// Option variable: cosinus factor.
float csBezierMesh:: cfg_cosinus_factor = 0;

csStringID csBezierMesh::vertex_name = csInvalidStringID;
csStringID csBezierMesh::texel_name = csInvalidStringID;
csStringID csBezierMesh::color_name = csInvalidStringID;
csStringID csBezierMesh::index_name = csInvalidStringID;

csBezierMesh::csBezierMesh (iBase *parent, csBezierMeshObjectType* thing_type) :
  scfImplementationType (this, parent), curves (4, 16)
{
  static_data = new csBezierMeshStatic (
  	thing_type, (iBezierFactoryState*)this);

  polygonMesh.AttachNew (new BezierPolyMeshHelper ());
  polygonMeshLOD.AttachNew (new BezierPolyMeshHelper ());
  polygonMesh->SetThing (this);
  polygonMeshLOD->SetThing (this);
  SetPolygonMeshBase (polygonMesh);
  SetPolygonMeshColldet (polygonMesh);
  SetPolygonMeshViscull (polygonMeshLOD);
  SetPolygonMeshShadows (polygonMeshLOD);

  last_thing_id++;
  thing_id = last_thing_id;
  logparent = 0;
  logparent_factory = 0;
  beziermsh_type = thing_type;

  light_version = 1;

  ParentTemplate = 0;

  cameranr = -1;
  movablenr = -1;
  wor_bbox_movablenr = -1;
  cached_movable = 0;

  prepared = false;
  static_data_nr = 0;

  current_lod = 1;
  current_features = 0;

  curves_transf_ok = false;

  current_visnr = 1;
  cosinus_factor = -1;

  dynamic_ambient_version = 0;

  csRef<iStringSet> strings;
  strings = CS_QUERY_REGISTRY_TAG_INTERFACE (thing_type->object_reg,
    "crystalspace.shared.stringset", iStringSet);

  if ((vertex_name == csInvalidStringID) ||
    (texel_name == csInvalidStringID) ||
    (color_name == csInvalidStringID) ||
    (index_name == csInvalidStringID))
  {
    vertex_name = strings->Request ("vertices");
    texel_name = strings->Request ("texture coordinates");
    color_name = strings->Request ("colors");
    index_name = strings->Request ("indices");
  }
}

csBezierMesh::~csBezierMesh ()
{
  delete static_data;
}

char* csBezierMesh::GenerateCacheName ()
{
  csMemFile mf;
  int32 l;
  l = csLittleEndian::Convert ((int32)static_data->num_curve_vertices);
  mf.Write ((char*)&l, 4);
  l = csLittleEndian::Convert ((int32)curves.Length ());
  mf.Write ((char*)&l, 4);

  if (logparent)
  {
    iMeshWrapper* mw = logparent;
    if (mw->QueryObject ()->GetName ())
      mf.Write (mw->QueryObject ()->GetName (),
		strlen (mw->QueryObject ()->GetName ()));
    iSector* sect = mw->GetMovable ()->GetSectors ()->Get (0);
    if (sect && sect->QueryObject ()->GetName ())
      mf.Write (sect->QueryObject ()->GetName (),
		strlen (sect->QueryObject ()->GetName ()));
  }

  csMD5::Digest digest = csMD5::Encode (mf.GetData (), mf.GetSize ());
  csString hex(digest.HexString());
  return hex.Detach();
}

void csBezierMesh::MarkLightmapsDirty ()
{
  light_version++;
}

void csBezierMesh::LightChanged (iLight* /*light*/)
{
  MarkLightmapsDirty ();
}

void csBezierMesh::LightDisconnect (iLight* light)
{
  MarkLightmapsDirty ();
  size_t i;
  int dt = light->GetDynamicType ();
  for (i = 0; i < curves.Length (); i++)
  {
    csCurve *c = curves[i];
    if (dt == CS_LIGHT_DYNAMICTYPE_DYNAMIC)
      c->DynamicLightDisconnect (light);
    else
      c->StaticLightDisconnect (light);
  }
}

void csBezierMesh::DisconnectAllLights ()
{
  MarkLightmapsDirty ();
  size_t i;
  for (i = 0; i < curves.Length (); i++)
  {
    csCurve *c = curves[i];
    c->DisconnectAllLights ();
  }
}

void csBezierMesh::WorUpdate ()
{
  if (cached_movable && cached_movable->GetUpdateNumber () != movablenr)
  {
    movablenr = cached_movable->GetUpdateNumber ();

    if (cached_movable->IsFullTransformIdentity ())
    {
      csReversibleTransform movtrans;	// Identity.
      UpdateCurveTransform (movtrans);
    }
    else
    {
      csReversibleTransform movtrans = cached_movable->GetFullTransform ();
      UpdateCurveTransform (movtrans);
    }

    // If the movable changed we invalidate the camera number as well
    // to make sure the camera vertices are recalculated as well.
    cameranr--;
  }
}

void csBezierMesh::Prepare ()
{
  static_data->Prepare ();

  prepared = true;

  polygonMeshLOD->Cleanup ();
  polygonMesh->Cleanup ();

  static_data_nr = static_data->static_data_nr;

  if (cached_movable) movablenr = cached_movable->GetUpdateNumber ()-1;
  else movablenr--;

  ShapeChanged ();
}

size_t csBezierMesh::AddCurveVertex (const csVector3 &v, const csVector2 &t)
{
  if (!static_data->curve_vertices)
  {
    static_data->max_curve_vertices = 10;
    static_data->curve_vertices = new csVector3[static_data->max_curve_vertices];
    static_data->curve_texels = new csVector2[static_data->max_curve_vertices];
  }

  while (static_data->num_curve_vertices >= static_data->max_curve_vertices)
  {
    static_data->max_curve_vertices += 10;

    csVector3 *new_vertices = new csVector3[static_data->max_curve_vertices];
    csVector2 *new_texels = new csVector2[static_data->max_curve_vertices];
    memcpy (
      new_vertices,
      static_data->curve_vertices,
      sizeof (csVector3) * static_data->num_curve_vertices);
    memcpy (
      new_texels,
      static_data->curve_texels,
      sizeof (csVector2) * static_data->num_curve_vertices);
    delete[] static_data->curve_vertices;
    delete[] static_data->curve_texels;
    static_data->curve_vertices = new_vertices;
    static_data->curve_texels = new_texels;
  }

  static_data->curve_vertices[static_data->num_curve_vertices] = v;
  static_data->curve_texels[static_data->num_curve_vertices] = t;
  static_data->num_curve_vertices++;
  return static_data->num_curve_vertices - 1;
}

void csBezierMesh::SetCurveVertex (int idx, const csVector3 &vt)
{
  CS_ASSERT (idx >= 0 && idx < static_data->num_curve_vertices);
  static_data->curve_vertices[idx] = vt;
  static_data->obj_bbox_valid = false;
  curves_transf_ok = false;
}

void csBezierMesh::SetCurveTexel (int idx, const csVector2 &vt)
{
  CS_ASSERT (idx >= 0 && idx < static_data->num_curve_vertices);
  static_data->curve_texels[idx] = vt;
}

void csBezierMesh::ClearCurveVertices ()
{
  delete[] static_data->curve_vertices;
  static_data->curve_vertices = 0;
  delete[] static_data->curve_texels;
  static_data->curve_texels = 0;
  static_data->obj_bbox_valid = false;
  curves_transf_ok = false;
}

void csBezierMesh::InvalidateThing ()
{
  prepared = false;
  static_data->obj_bbox_valid = false;

  polygonMeshLOD->Cleanup ();
  polygonMesh->Cleanup ();
  ShapeChanged ();
}

csCurve *csBezierMesh::GetCurve (char *name) const
{
  size_t i;
  for (i = 0 ; i < curves.Length () ; i++)
  {
    const char* n = curves[i]->GetName ();
    if (n && !strcmp (n, name))
      return curves[i];
  }
  return 0;
}

void csBezierMesh::AddCurve (csCurve *curve)
{
  curve->SetParentThing (this);
  curves.Push (curve);
  curves_transf_ok = false;
  static_data->obj_bbox_valid = false;
}

iCurve *csBezierMesh::CreateCurve ()
{
  csCurve *c = new csBezierCurve (static_data->thing_type);
  c->SetParentThing (this);
  AddCurve (c);
  return c;
}

int csBezierMesh::FindCurveIndex (iCurve *curve) const
{
  return (int)curves.Find (static_cast<csCurve*> (curve));
}

void csBezierMesh::RemoveCurve (int idx)
{
  curves.DeleteIndex (idx);
  curves_transf_ok = false;
  static_data->obj_bbox_valid = false;
}

void csBezierMesh::RemoveCurves ()
{
  curves.DeleteAll ();
  curves_transf_ok = false;
  static_data->obj_bbox_valid = false;
}

void csBezierMesh::HardTransform (const csReversibleTransform &t)
{
  size_t i;

  static_data->curves_center = t.This2Other (static_data->curves_center);
  if (static_data->curve_vertices)
    for (i = 0; i < (size_t)static_data->num_curve_vertices; i++)
      static_data->curve_vertices[i] = t.This2Other (
      	static_data->curve_vertices[i]);

  curves_transf_ok = false;
  for (i = 0; i < curves.Length (); i++)
  {
    csCurve *c = curves[i];
    c->HardTransform (t);
  }
}

void csBezierMesh::GetTransformedBoundingBox (
  const csReversibleTransform &trans,
  csBox3 &cbox)
{
  //@@@@@@@@@@@@@@

  // @@@ Shouldn't we try to cache this depending on camera/movable number?

  // Similar to what happens in csSprite3D.
  const csBox3& box = GetBoundingBox ();
  cbox.StartBoundingBox (trans * box.GetCorner (0));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (1));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (2));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (3));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (4));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (5));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (6));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (7));
}

static void Perspective (
  const csVector3 &v,
  csVector2 &p,
  float fov,
  float sx,
  float sy)
{
  float iz = fov / v.z;
  p.x = v.x * iz + sx;
  p.y = v.y * iz + sy;
}

float csBezierMesh::GetScreenBoundingBox (
  float fov,
  float sx,
  float sy,
  const csReversibleTransform &trans,
  csBox2 &sbox,
  csBox3 &cbox)
{
  csVector2 oneCorner;

  GetTransformedBoundingBox (trans, cbox);

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

void csBezierMesh::AppendShadows (
  iMovable* movable,
  iShadowBlockList *shadows,
  const csVector3& /*origin*/)
{
  Prepare ();
  //@@@ Ok?
  cached_movable = movable;
  WorUpdate ();

  iShadowBlock *list = shadows->NewShadowBlock (0);
      //@@@polygons.Length ());
(void)list;
#if 0
  csFrustum *frust;
  int i, j;
  bool cw = true;                   //@@@ Use mirroring parameter here!
  for (i = 0; i < static_data->static_polygons.Length (); i++)
  {
    sp = static_data->static_polygons.Get (i);
    if (sp->GetPortal ()) continue;  // No portals
    p = polygons.Get (i);

    //if (p->GetPlane ()->VisibleFromPoint (origin) != cw) continue;
    float clas = p->GetWorldPlane ().Classify (origin);
    if (ABS (clas) < EPSILON) continue;
    if ((clas <= 0) != cw) continue;

    csPlane3 pl = p->GetWorldPlane ();
    pl.DD += origin * pl.norm;
    pl.Invert ();
    frust = list->AddShadow (
        origin,
        (void *)p,
        sp->GetVertices ().GetVertexCount (),
        pl);
    for (j = 0; j < sp->GetVertices ().GetVertexCount (); j++)
      frust->GetVertex (j).Set (p->Vwor (j) - origin);
  }
#endif
}

void csBezierMesh::GetRadius (float &rad, csVector3 &cent)
{
  const csBox3& b = GetBoundingBox ();
  rad = static_data->obj_radius;
  cent = b.GetCenter ();
}

const csBox3& csBezierMesh::GetBoundingBox ()
{
  int i;

  if (static_data->obj_bbox_valid)
    return static_data->obj_bbox;

  static_data->obj_bbox_valid = true;

  if (static_data->num_curve_vertices == 0)
  {
    static_data->obj_bbox.Set (0, 0, 0, 0, 0, 0);
    return static_data->obj_bbox;
  }

  if (static_data->num_curve_vertices > 0)
  {
    static_data->obj_bbox.StartBoundingBox (static_data->curve_vertices[0]);
    for (i = 1 ; i < static_data->num_curve_vertices ; i++)
    {
      static_data->obj_bbox.AddBoundingVertexSmart (
	  static_data->curve_vertices[i]);
    }
  }

  static_data->obj_radius = csQsqrt (csSquaredDist::PointPoint (
  	static_data->obj_bbox.Max (), static_data->obj_bbox.Min ())) * 0.5f;
  return static_data->obj_bbox;
}

void csBezierMesh::SetBoundingBox (const csBox3& b)
{
  static_data->obj_bbox_valid = true;
  static_data->obj_bbox = b;
  ShapeChanged ();
}

void csBezierMesh::GetBoundingBox (iMovable *movable, csBox3 &box)
{
  if (wor_bbox_movablenr != movable->GetUpdateNumber ())
  {
    // First make sure obj_bbox is valid.
    box = GetBoundingBox ();
    wor_bbox_movablenr = movable->GetUpdateNumber ();
    csBox3& obj_bbox = static_data->obj_bbox;

    // @@@ Maybe it would be better to really calculate the bounding box
    // here instead of just transforming the object space bounding box?
    if (movable->IsFullTransformIdentity ())
    {
      wor_bbox = obj_bbox;
    }
    else
    {
      csReversibleTransform mt = movable->GetFullTransform ();
      wor_bbox.StartBoundingBox (mt.This2Other (obj_bbox.GetCorner (0)));
      wor_bbox.AddBoundingVertexSmart (mt.This2Other (obj_bbox.GetCorner (1)));
      wor_bbox.AddBoundingVertexSmart (mt.This2Other (obj_bbox.GetCorner (2)));
      wor_bbox.AddBoundingVertexSmart (mt.This2Other (obj_bbox.GetCorner (3)));
      wor_bbox.AddBoundingVertexSmart (mt.This2Other (obj_bbox.GetCorner (4)));
      wor_bbox.AddBoundingVertexSmart (mt.This2Other (obj_bbox.GetCorner (5)));
      wor_bbox.AddBoundingVertexSmart (mt.This2Other (obj_bbox.GetCorner (6)));
      wor_bbox.AddBoundingVertexSmart (mt.This2Other (obj_bbox.GetCorner (7)));
    }
  }

  box = wor_bbox;
}

//-------------------------------------------------------------------------

void BezierPolyMeshHelper::Setup ()
{
  if (polygons)
  {
    // Already set up.
    return ;
  }

  polygons = 0;
  vertices = 0;

  // Count the number of needed polygons and vertices.
  num_verts = 0;
  num_poly = 0;

  int i, j;

  // Check curves.
  for (i = 0; i < thing->GetCurveCount (); i++)
  {
    csCurve *c = thing->curves.Get (i);
    csCurveTesselated *tess = c->Tesselate (1000);    // @@@ High quality?
    num_poly += (int)tess->GetTriangleCount ();
    num_verts += (int)tess->GetVertexCount ();
  }

  if (!num_verts || !num_poly) return;

  // Allocate the arrays and the copy the data.
  vertices = new csVector3[num_verts];
  polygons = new csMeshedPolygon[num_poly];

  num_verts = 0;
  num_poly = 0;
  for (i = 0; i < thing->GetCurveCount (); i++)
  {
    csCurve *c = thing->curves.Get (i);
    csCurveTesselated *tess = c->Tesselate (1000);  // @@@ High quality?
    csTriangle *tris = tess->GetTriangles ();
    int tri_count = (int)tess->GetTriangleCount ();
    for (j = 0; j < tri_count; j++)
    {
      polygons[num_poly].num_vertices = 3;
      polygons[num_poly].vertices = new int[3];

      // Adjust indices to skip the original polygon set vertices and
      // preceeding curves.
      polygons[num_poly].vertices[0] = tris[j].a + num_verts;
      polygons[num_poly].vertices[1] = tris[j].b + num_verts;
      polygons[num_poly].vertices[2] = tris[j].c + num_verts;
      num_poly++;
    }

    csVector3 *vts = tess->GetVertices ();
    int num_vt = (int)tess->GetVertexCount ();
    memcpy (vertices + num_verts, vts, sizeof (csVector3) * num_vt);
    num_verts += num_vt;
  }
}

void BezierPolyMeshHelper::Cleanup ()
{
  int i;

  // Delete all polygons which were generated from curved surfaces.
  // The other polygons just have a reference to the original polygons
  // from the parent.
  if (polygons)
  {
    for (i = 0; i < num_poly; i++)
    {
      delete[] polygons[i].vertices;
    }

    delete[] polygons;
    polygons = 0;
  }

  delete[] vertices;
  vertices = 0;

  delete[] triangles;
  triangles = 0;
}

//-------------------------------------------------------------------------

void csBezierMesh::UpdateCurveTransform (const csReversibleTransform &movtrans)
{
  if (GetCurveCount () == 0) return ;

  // since obj has changed (possibly) we need to tell all of our curves
  csReversibleTransform o2w = movtrans.GetInverse ();
  int i;
  for (i = 0; i < GetCurveCount (); i++)
  {
    csCurve *c = curves.Get (i);
    c->SetObject2World (&o2w);
  }
}

void csBezierMesh::UpdateCurveTransform ()
{
  if (curves_transf_ok) return ;
  curves_transf_ok = true;
  if (GetCurveCount () == 0) return ;

  csReversibleTransform o2w;                    // Identity transform.
  int i;
  for (i = 0; i < GetCurveCount (); i++)
  {
    csCurve *c = curves.Get (i);
    c->SetObject2World (&o2w);
  }
}

csRenderMesh** csBezierMesh::GetRenderMeshes (int &n, iRenderView* rview,
					      iMovable* movable, 
					      uint32 frustum_mask)
{
  if (GetCurveCount () <= 0) return false;

  iCamera *icam = rview->GetCamera ();
  const csReversibleTransform &camtrans = icam->GetTransform ();

  csReversibleTransform movtrans;

  // Only get the transformation if this thing can move.
  movtrans = movable->GetFullTransform ();

  int i;
  int res = 1;

  // Calculate tesselation resolution
  csVector3 wv = static_data->curves_center;
  csVector3 world_coord = movtrans.This2Other (wv);

  csVector3 camera_coord = camtrans.Other2This (world_coord);

  if (camera_coord.z >= SMALL_Z)
  {
    res = (int)(static_data->curves_scale / camera_coord.z);
  }
  else
    res = 1000;                                 // some big tesselation value...

  // Create the combined transform of object to camera by
  // combining object to world and world to camera.
  csReversibleTransform obj_cam = camtrans;
  obj_cam /= movtrans;
 
  int clip_portal, clip_plane, clip_z_plane;
  rview->CalculateClipSettings (frustum_mask, clip_portal, clip_plane,
      clip_z_plane);

  const uint currentFrame = rview->GetCurrentFrameNumber();
  bool listCreated;
  csDirtyAccessArray<csRenderMesh*>& meshes = rmListHolder.GetUnusedData (
    listCreated, currentFrame);
  meshes.SetLength (GetCurveCount(), 0);

  iSector* s = movable->GetSectors ()->Get (0);
  csColor ambient = s->GetDynamicAmbientLight ();
  bool update_ambient = false;
  if (dynamic_ambient_version != s->GetDynamicAmbientVersion ())
  {
    update_ambient = true;
    dynamic_ambient_version = s->GetDynamicAmbientVersion ();
  }
  csCurve *c;
  for (i = 0; i < GetCurveCount (); i++)
  {
    bool meshCreated;
    csRenderMesh*& rm = rmHolder.GetUnusedMesh (meshCreated, currentFrame);
    meshes[i] = rm;
    if (meshCreated)
    {
      rm->buffers.AttachNew (new csRenderBufferHolder);
      rm->variablecontext.AttachNew (new csShaderVariableContext);
    }
    rm->worldspace_origin = world_coord;
    rm->clip_portal = clip_portal;
    rm->clip_plane = clip_plane;
    rm->clip_z_plane = clip_z_plane;
    rm->do_mirror = icam->IsMirrored ();
    rm->object2world = movtrans;

    c = curves.Get (i);

    // First get a bounding box in camera space.
    csBox3 cbox;
    csBox2 sbox;
    if (c->GetScreenBoundingBox (obj_cam, icam, cbox, sbox) < 0)
      continue;                                 // Not visible.

    // If we have a dirty lightmap recombine the curves and the shadow maps.
    bool updated_lm = c->RecalculateDynamicLights ();

    // Create a new tesselation reuse an old one.
    csCurveTesselated *tess = c->Tesselate (res);

    // If the lightmap was updated or the new tesselation doesn't yet
    // have a valid colors table we need to update colors here.
    if (update_ambient || updated_lm || !tess->AreColorsValid ())
      tess->UpdateColors (c->LightMap, ambient);

    c->GetMaterial ()->Visit ();

    bool gouraud = !!c->LightMap;
    rm->mixmode = CS_FX_COPY | (gouraud ? 0 : CS_FX_FLAT);
    
    csRenderBufferHolder* holder = rm->buffers;
    /* @@@ TODO: use an SV accessor for geometry delivery */
    bool frameBuffersCreated;
    PerFrameData& frameBuffers = frameBufferHolder.GetUnusedData (
      frameBuffersCreated, currentFrame);
    if (frameBuffersCreated || (frameBuffers.vertCount < tess->GetVertexCount ()))
    {
      frameBuffers.vertices = csRenderBuffer::CreateRenderBuffer (
	tess->GetVertexCount (), CS_BUF_STREAM, 
	CS_BUFCOMP_FLOAT, 3);
      frameBuffers.colors = csRenderBuffer::CreateRenderBuffer (
	tess->GetVertexCount (), CS_BUF_STREAM, 
	CS_BUFCOMP_FLOAT, 3);
      frameBuffers.texcoords = csRenderBuffer::CreateRenderBuffer (
	tess->GetVertexCount (), CS_BUF_STREAM, 
	CS_BUFCOMP_FLOAT, 2);
      frameBuffers.indices = csRenderBuffer::CreateIndexRenderBuffer (
	tess->GetTriangleCount () * 3, CS_BUF_STREAM, 
	CS_BUFCOMP_UNSIGNED_INT, 0, tess->GetVertexCount ()-1);
      frameBuffers.vertCount = tess->GetVertexCount ();
    }

    frameBuffers.vertices->CopyInto (tess->GetVertices(), tess->GetVertexCount ());
    holder->SetRenderBuffer (CS_BUFFER_POSITION, frameBuffers.vertices);
    frameBuffers.colors->CopyInto (tess->GetColors(), tess->GetVertexCount ());
    holder->SetRenderBuffer (CS_BUFFER_COLOR, frameBuffers.colors);
    frameBuffers.texcoords->CopyInto (tess->GetTxtCoords(), tess->GetVertexCount ());
    holder->SetRenderBuffer (CS_BUFFER_TEXCOORD0, frameBuffers.texcoords);
    frameBuffers.indices->CopyInto (tess->GetTriangles(), tess->GetTriangleCount ()*3);
    holder->SetRenderBuffer (CS_BUFFER_INDEX, frameBuffers.indices);

    rm->indexstart = 0;
    rm->indexend = (uint)tess->GetTriangleCount() * 3;
    rm->meshtype = CS_MESHTYPE_TRIANGLES;
    rm->material = c->GetMaterial ();
  }

  n = (int)meshes.Length();
  return meshes.GetArray();
}

//----------------------------------------------------------------------

void csBezierMesh::CastShadows (iMovable *movable, iFrustumView *lview)
{
  Prepare ();
  //@@@ Ok?
  cached_movable = movable;
  WorUpdate ();

  int i;

  iFrustumViewUserdata* fvud = lview->GetUserdata ();
  iLightingProcessInfo* lpi = (iLightingProcessInfo*)fvud;
  bool dyn = lpi->IsDynamic ();

  lpi->GetLight ()->AddAffectedLightingInfo ((iLightingInfo*)this);

  for (i = 0; i < GetCurveCount (); i++)
  {
    csCurve* curve = curves.Get (i);
    if (dyn)
      curve->CalculateLightingDynamic (lview);
    else
      curve->CalculateLightingStatic (lview, true);
  }
}

void csBezierMesh::InitializeDefault (bool clear)
{
  Prepare ();

  int i;
  for (i = 0; i < GetCurveCount (); i++)
    curves.Get (i)->InitializeDefaultLighting (clear);
}

bool csBezierMesh::ReadFromCache (iCacheManager* cache_mgr)
{
  Prepare ();
  char* cachename = GenerateCacheName ();
  cache_mgr->SetCurrentScope (cachename);
  delete[] cachename;

  // For error reporting.
  const char* thing_name = 0;
  if (static_data->thing_type->do_verbose && logparent)
  {
    thing_name = logparent->QueryObject ()->GetName ();
  }

  bool rc = true;
  csRef<iDataBuffer> db = cache_mgr->ReadCache ("bezier_lm", 0, (uint32)~0);
  if (db)
  {
    csMemFile mf ((const char*)(db->GetData ()), db->GetSize ());
    int i;
    for (i = 0; i < GetCurveCount (); i++)
    {
      const char* error = curves.Get (i)->ReadFromCache (&mf);
      if (error != 0)
      {
        rc = false;
        if (static_data->thing_type->do_verbose)
	{
	  csPrintf ("  Bezier '%s' Curve '%s': %s\n",
	  	thing_name, curves.Get (i)->GetName (),
		error);
	  fflush (stdout);
        }
      }
    }
  }
  else
  {
    if (static_data->thing_type->do_verbose)
    {
      csPrintf ("  Bezier '%s': Couldn't find cached lightmap file for bezier!\n",
      	thing_name);
      fflush (stdout);
    }
    rc = false;
  }

  cache_mgr->SetCurrentScope (0);
  return rc;
}

bool csBezierMesh::WriteToCache (iCacheManager* cache_mgr)
{
  char* cachename = GenerateCacheName ();
  cache_mgr->SetCurrentScope (cachename);
  delete[] cachename;

  int i;
  bool rc = false;
  csMemFile mf;
  for (i = 0; i < GetCurveCount (); i++)
    if (!curves.Get (i)->WriteToCache (&mf)) goto stop;
  if (!cache_mgr->CacheData ((void*)(mf.GetData ()), mf.GetSize (),
    	"bezier_lm", 0, (uint32)~0))
    goto stop;

  rc = true;

stop:
  cache_mgr->SetCurrentScope (0);
  return rc;
}

void csBezierMesh::PrepareLighting ()
{
  int i;
  for (i = 0; i < GetCurveCount (); i++) curves.Get (i)->PrepareLighting ();
}

void csBezierMesh::Merge (csBezierMesh *other)
{
  int i;
  for (i = 0; i < other->static_data->GetCurveVertexCount (); i++)
    AddCurveVertex (other->static_data->GetCurveVertex (i),
    	other->static_data->GetCurveTexel (i));

  while (other->curves.Length () > 0)
  {
    csCurve *c = other->curves.Extract (0);
    AddCurve (c);
  }
}

void csBezierMesh::MergeTemplate (
  iBezierFactoryState *tpl,
  iMaterialWrapper *default_material,
  csVector3 *shift,
  csMatrix3 *transform)
{
  int i;

  //TODO should merge? take averages or something?
  static_data->curves_center = tpl->GetCurvesCenter ();
  static_data->curves_scale = tpl->GetCurvesScale ();

  //@@@ TEMPORARY
  csRef<iBezierState> ith = scfQueryInterface<iBezierState> (tpl);
  ParentTemplate = (csBezierMesh*)(iBezierState*)ith;

  for (i = 0; i < tpl->GetCurveVertexCount (); i++)
  {
    csVector3 v = tpl->GetCurveVertex (i);
    if (transform) v = *transform * v;
    if (shift) v += *shift;
    AddCurveVertex (v, tpl->GetCurveTexel (i));
  }

  for (i = 0; i < tpl->GetCurveCount (); i++)
  {
    iCurve *orig_curve = tpl->GetCurve (i);
    iCurve *p = CreateCurve ();
    int j;
    for (j = 0 ; j < orig_curve->GetVertexCount () ; j++)
      p->SetVertex (j, orig_curve->GetVertex (j));
    p->QueryObject ()->SetName (orig_curve->QueryObject ()->GetName ());
    if (orig_curve->GetMaterial ())
      p->SetMaterial (orig_curve->GetMaterial ());
    else
      p->SetMaterial (default_material);
  }
}

csPtr<iMeshObject> csBezierMesh::NewInstance ()
{
  csBezierMesh *thing = new csBezierMesh (this,
  	static_data->thing_type);
  thing->MergeTemplate ((iBezierFactoryState*)this, 0);
  return csPtr<iMeshObject> ((iMeshObject*)thing);
}

//---------------------------------------------------------------------------


SCF_IMPLEMENT_FACTORY (csBezierMeshObjectType)


csBezierMeshObjectType::csBezierMeshObjectType (
  iBase *pParent) : scfImplementationType (this, pParent)
{
  lightpatch_pool = 0;
  do_verbose = false;
}

csBezierMeshObjectType::~csBezierMeshObjectType ()
{
  delete lightpatch_pool;
}

bool csBezierMeshObjectType::Initialize (iObjectRegistry *object_reg)
{
  csBezierMeshObjectType::object_reg = object_reg;
  csRef<iEngine> e = csQueryRegistry<iEngine> (object_reg);
  engine = e;	// We don't want a real ref here to avoid circular refs.
  csRef<iGraphics3D> g = csQueryRegistry<iGraphics3D> (object_reg);
  G3D = g;

  lightpatch_pool = new csBezierLightPatchPool ();

  csRef<iVerbosityManager> verbosemgr (
    csQueryRegistry<iVerbosityManager> (object_reg));
  if (verbosemgr) 
    do_verbose = verbosemgr->Enabled ("bezier");

  return true;
}

void csBezierMeshObjectType::Clear ()
{
  delete lightpatch_pool;
  lightpatch_pool = new csBezierLightPatchPool ();
}

csPtr<iMeshObjectFactory> csBezierMeshObjectType::NewFactory ()
{
  csBezierMesh *cm = new csBezierMesh (this, this);
  csRef<iMeshObjectFactory> ifact (SCF_QUERY_INTERFACE (
      cm, iMeshObjectFactory));
  cm->DecRef ();
  return csPtr<iMeshObjectFactory> (ifact);
}

void csBezierMeshObjectType::ReportV (int severity, const char *description, 
                                      va_list args)
{
  csReportV (object_reg, severity, "crystalspace.mesh.object.bezier",
    description, args);
}

void csBezierMeshObjectType::Warn (const char *description, ...)
{
  va_list arg;
  va_start (arg, description);
  ReportV (CS_REPORTER_SEVERITY_WARNING, description, arg);
  va_end (arg);
}

void csBezierMeshObjectType::Bug (const char *description, ...)
{
  va_list arg;
  va_start (arg, description);
  ReportV (CS_REPORTER_SEVERITY_BUG, description, arg);
  va_end (arg);
}

void csBezierMeshObjectType::Error (const char *description, ...)
{
  va_list arg;
  va_start (arg, description);
  ReportV (CS_REPORTER_SEVERITY_ERROR, description, arg);
  va_end (arg);
}

void csBezierMeshObjectType::Notify (const char *description, ...)
{
  va_list arg;
  va_start (arg, description);
  ReportV (CS_REPORTER_SEVERITY_NOTIFY, description, arg);
  va_end (arg);
}

//---------------------------------------------------------------------------

static const csOptionDescription
  config_options[] =
{
  { 0, "cosfact", "Cosinus factor for lighting", CSVAR_FLOAT }
};
const int NUM_OPTIONS =
  (
    sizeof (config_options) /
    sizeof (config_options[0])
  );

bool csBezierMeshObjectType::SetOption (int id, csVariant *value)
{
  switch (id)
  {
    case 0:
      csBezierMesh::cfg_cosinus_factor = value->GetFloat ();
      break;
    default:
      return false;
  }

  return true;
}

bool csBezierMeshObjectType::GetOption (int id, csVariant *value)
{
  switch (id)
  {
    case 0:   value->SetFloat (csBezierMesh::cfg_cosinus_factor); break;
    default:  return false;
  }

  return true;
}

bool csBezierMeshObjectType::GetOptionDescription (
  int idx,
  csOptionDescription *option)
{
  if (idx < 0 || idx >= NUM_OPTIONS) return false;
  *option = config_options[idx];
  return true;
}

//---------------------------------------------------------------------------

}
CS_PLUGIN_NAMESPACE_END(Bezier)
