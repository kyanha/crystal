/*
    Copyright (C) 2000-2001 by Jorrit Tyberghein

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
#include "csgeom/sphere.h"
#include "igeom/objmodel.h"
#include "csengine/sector.h"
#include "csengine/meshobj.h"
#include "csengine/light.h"
#include "csengine/engine.h"
#include "iengine/portal.h"
#include "csutil/debug.h"
#include "iengine/rview.h"
#include "imesh/thing/thing.h"
#include "imesh/thing/polygon.h"
#include "ivideo/graph3d.h"


// ---------------------------------------------------------------------------
// csMeshWrapper
// ---------------------------------------------------------------------------
SCF_IMPLEMENT_IBASE_EXT(csMeshWrapper)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iMeshWrapper)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iVisibilityObject)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iImposter)
  SCF_IMPLEMENTS_INTERFACE(csMeshWrapper)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csMeshWrapper::MeshWrapper)
  SCF_IMPLEMENTS_INTERFACE(iMeshWrapper)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csMeshWrapper::VisObject)
  SCF_IMPLEMENTS_INTERFACE(iVisibilityObject)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csMeshWrapper::MeshImposter)
  SCF_IMPLEMENTS_INTERFACE(iImposter)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csMeshWrapper::csMeshWrapper (
  iMeshWrapper *theParent,
  iMeshObject *meshobj) :
    csObject()
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiMeshWrapper);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiVisibilityObject);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiImposter);
  DG_TYPE (this, "csMeshWrapper");

  movable.scfParent = this;
  defered_num_lights = 0;
  defered_lighting_flags = 0;
  last_anim_time = 0;
  visnr = 0;
  wor_bbox_movablenr = -1;
  Parent = theParent;
  movable.SetMeshWrapper (this);
  if (Parent) movable.SetParent (Parent->GetMovable ());

  csEngine::current_engine->AddToCurrentRegion (this);
  csMeshWrapper::meshobj = meshobj;
  if (meshobj)
  {
    light_info = SCF_QUERY_INTERFACE (meshobj, iLightingInfo);
    shadow_receiver = SCF_QUERY_INTERFACE (meshobj, iShadowReceiver);
  }
  factory = NULL;
  zbufMode = CS_ZBUF_USE;
  render_priority = csEngine::current_engine->GetObjectRenderPriority ();
  children.SetMesh (this);
  imposter_active = false;
  imposter_mesh = NULL;
#ifdef CS_USE_NEW_RENDERER
  cast_hardware_shadow = true;
  draw_after_fancy_stuff = false;
#endif
}

csMeshWrapper::csMeshWrapper (iMeshWrapper *theParent) :
  csObject()
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiMeshWrapper);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiVisibilityObject);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiImposter);
  DG_TYPE (this, "csMeshWrapper");

  movable.scfParent = this;
  defered_num_lights = 0;
  defered_lighting_flags = 0;
  last_anim_time = 0;
  visnr = 0;
  imposter_active = false;
  imposter_mesh = NULL;
  wor_bbox_movablenr = -1;
  Parent = theParent;
  movable.SetMeshWrapper (this);
  if (Parent) movable.SetParent (Parent->GetMovable ());

  csEngine::current_engine->AddToCurrentRegion (this);
  factory = NULL;
  zbufMode = CS_ZBUF_USE;
  render_priority = csEngine::current_engine->GetObjectRenderPriority ();
  children.SetMesh (this);
#ifdef CS_USE_NEW_RENDERER
  cast_hardware_shadow = true;
  draw_after_fancy_stuff = false;
#endif
}

void csMeshWrapper::SetMeshObject (iMeshObject *meshobj)
{
  csMeshWrapper::meshobj = meshobj;
  if (meshobj)
  {
    light_info = SCF_QUERY_INTERFACE (meshobj, iLightingInfo);
    shadow_receiver = SCF_QUERY_INTERFACE (meshobj, iShadowReceiver);
  }
  else
  {
    light_info = NULL;
    shadow_receiver = NULL;
  }
}

csMeshWrapper::~csMeshWrapper ()
{
  delete imposter_mesh;
}

void csMeshWrapper::UpdateMove ()
{
  int i;
  for (i = 0; i < children.GetCount (); i++)
  {
    iMeshWrapper *spr = children.Get (i);
    spr->GetMovable ()->UpdateMove ();
  }
}

void csMeshWrapper::MoveToSector (csSector *s)
{
  // Only add this mesh to a sector if the parent is the engine.
  // Otherwise we have a hierarchical object and in that case
  // the parent object controls this.
  if (!Parent) s->GetMeshes ()->Add (&scfiMeshWrapper);
}

void csMeshWrapper::RemoveFromSectors ()
{
  if (Parent) return ;

  int i;
  const iSectorList *sectors = movable.GetSectors ();
  for (i = 0; i < sectors->GetCount (); i++)
  {
    iSector *ss = sectors->Get (i);
    if (ss) ss->GetMeshes ()->Remove (&scfiMeshWrapper);
  }
}

void csMeshWrapper::SetRenderPriority (long rp)
{
  render_priority = rp;

  if (Parent) return ;

  int i;
  const iSectorList *sectors = movable.GetSectors ();
  for (i = 0; i < sectors->GetCount (); i++)
  {
    iSector *ss = sectors->Get (i);
    if (ss) ss->GetPrivateObject ()->RelinkMesh (&scfiMeshWrapper);
  }
}

/// The list of lights that hit the mesh
typedef csGrowingArray<iLight*> engine3d_LightWorkTable;
CS_IMPLEMENT_STATIC_VAR (GetStaticLightWorkTable, engine3d_LightWorkTable,())

void csMeshWrapper::UpdateDeferedLighting (const csBox3 &box)
{
  static engine3d_LightWorkTable &light_worktable = *GetStaticLightWorkTable ();
  const iSectorList *movable_sectors = movable.GetSectors ();
  if (defered_num_lights && movable_sectors->GetCount () > 0)
  {
    if (defered_num_lights > light_worktable.Limit ())
      light_worktable.SetLimit (defered_num_lights);

    iSector *sect = movable_sectors->Get (0);
    int num_lights = csEngine::current_iengine->GetNearbyLights (
        sect,
        box,
        defered_lighting_flags,
        light_worktable.GetArray (),
        defered_num_lights);
    UpdateLighting (light_worktable.GetArray (), num_lights);
  }
}

void csMeshWrapper::DeferUpdateLighting (int flags, int num_lights)
{
  defered_num_lights = num_lights;
  defered_lighting_flags = flags;
}

void csMeshWrapper::Draw (iRenderView *rview)
{
  if (flags.Check (CS_ENTITY_INVISIBLE)) return ;
  DrawInt (rview);
}

#ifdef CS_USE_NEW_RENDERER
csRenderMesh *csMeshWrapper::GetRenderMesh (/*iRenderView* rview*/)
{
  iMeshWrapper *meshwrap = &scfiMeshWrapper;

  int i;
  // Callback are traversed in reverse order so that they can safely
  // delete themselves.
/*  i = draw_cb_vector.Length ()-1;
  while (i >= 0)
  {
    iMeshDrawCallback* cb = draw_cb_vector.Get (i);
    if (!cb->BeforeDrawing (meshwrap, rview)) return NULL;
    i--;
  }*/

  /*draw_test = meshobj->DrawTest (rview, &movable.scfiMovable);
  if (draw_test)
  {
    csTicks lt = csEngine::current_engine->GetLastAnimationTime ();
    if (lt != 0)
    {
      if (lt != last_anim_time)
      {
        meshobj->NextFrame (lt,movable.GetPosition ());
        last_anim_time = lt;
      }
    }*/

    UpdateDeferedLighting (movable.GetFullPosition ());
    return meshobj->GetRenderMesh (/*rview, &movable.scfiMovable, zbufMode*/);
/*  }
  return NULL;*/

  /*
  for (i = 0; i < children.GetCount (); i++)
  {
    iMeshWrapper *spr = children.Get (i);
    spr->DrawZ (rview);
  }
  */
}

void csMeshWrapper::DrawShadow (iRenderView* rview, iLight* light)
{
  /*
  if (cast_hardware_shadow)
    meshobj->DrawShadow (rview, &movable.scfiMovable, zbufMode, light);
  */
}

void csMeshWrapper::DrawLight (iRenderView* rview, iLight* light)
{
  /*
  if (draw_test) 
    meshobj->DrawLight (rview, &movable.scfiMovable, zbufMode, light);
  */
}

void csMeshWrapper::CastHardwareShadow (bool castShadow)
{
  cast_hardware_shadow = castShadow;
}

void csMeshWrapper::SetDrawAfterShadow (bool drawAfter)
{
  draw_after_fancy_stuff = drawAfter;
}

bool csMeshWrapper::GetDrawAfterShadow ()
{
  return draw_after_fancy_stuff;
}
#endif // CS_USE_NEW_RENDERER

void csMeshWrapper::DrawInt (iRenderView *rview)
{
  if (imposter_active && CheckImposterRelevant(rview) )
    if (DrawImposter (rview))
    return;

  DrawIntFull (rview);
}

bool csMeshWrapper::CheckImposterRelevant (iRenderView *rview)
{
  iCamera* camera = rview->GetCamera ();
  // calculate distance from camera to mesh
  csBox3 obox;
  GetWorldBoundingBox (obox);
  csVector3 obj_center = (obox.Min () + obox.Max ()) / 2;
  csVector3 wor_center;
  if (movable.IsFullTransformIdentity ())
    wor_center = obj_center;
  else
    wor_center = movable.GetFullTransform ().This2Other (obj_center);
  csVector3 cam_origin = camera->GetTransform ().GetOrigin ();
  float wor_sq_dist = csSquaredDist::PointPoint (cam_origin, wor_center);
  float dist = min_imposter_distance->Get ();

  return (wor_sq_dist > dist*dist);
}

void csMeshWrapper::GetFullBBox (csBox3& box)
{
  meshobj->GetObjectModel ()->GetObjectBoundingBox (box, CS_BBOX_MAX);
  if (movable.GetParent ())
  {
    iMovable* mov = &movable.scfiMovable;
    while (mov->GetParent ())
    {
      const csReversibleTransform& trans = mov->GetTransform ();
      csBox3 b (trans.This2Other (box.GetCorner (0)));
      b.AddBoundingVertexSmart (trans.This2Other (box.GetCorner (1)));
      b.AddBoundingVertexSmart (trans.This2Other (box.GetCorner (2)));
      b.AddBoundingVertexSmart (trans.This2Other (box.GetCorner (3)));
      b.AddBoundingVertexSmart (trans.This2Other (box.GetCorner (4)));
      b.AddBoundingVertexSmart (trans.This2Other (box.GetCorner (5)));
      b.AddBoundingVertexSmart (trans.This2Other (box.GetCorner (6)));
      b.AddBoundingVertexSmart (trans.This2Other (box.GetCorner (7)));
      box = b;
      mov = mov->GetParent ();
    }
  }
}

void csMeshWrapper::DrawIntFull (iRenderView *rview)
{
  iMeshWrapper *meshwrap = &scfiMeshWrapper;

  int i;
  // Callback are traversed in reverse order so that they can safely
  // delete themselves.
  i = draw_cb_vector.Length ()-1;
  while (i >= 0)
  {
    iMeshDrawCallback* cb = draw_cb_vector.Get (i);
    if (!cb->BeforeDrawing (meshwrap, rview)) return ;
    i--;
  }

  if (meshobj->DrawTest (rview, &movable.scfiMovable))
  {
    csTicks lt = csEngine::current_engine->GetLastAnimationTime ();
    if (lt != 0)
    {
      if (lt != last_anim_time)
      {
        meshobj->NextFrame (lt,movable.GetPosition ());
        last_anim_time = lt;
      }
    }

    csBox3 bbox;
    GetFullBBox (bbox);
    UpdateDeferedLighting (bbox);
    meshobj->Draw (rview, &movable.scfiMovable, zbufMode);
  }

  for (i = 0; i < children.GetCount (); i++)
  {
    iMeshWrapper *spr = children.Get (i);
    spr->Draw (rview);
  }
}

bool csMeshWrapper::DrawImposter (iRenderView *rview)
{
  // Check for imposter existence.  If not, create it.
  if (!imposter_mesh)
  {
      return false;

  }
  // Check for imposter already ready
  if (!imposter_mesh->GetImposterReady())
      return false;

  // Check for too much camera movement since last imposter render
  if (!imposter_mesh->CheckIncidenceAngle (rview,
    imposter_rotation_tolerance->Get () ))
      return false;

  // Else draw imposter as-is.
  imposter_mesh->Draw (rview);
  return true;
}

void csMeshWrapper::SetImposterActive(bool flag,iObjectRegistry *objreg)
{
  imposter_active = flag;
  if (flag)
  {
    imposter_mesh = new csImposterMesh (this, objreg);
    imposter_mesh->SetImposterReady (false);
  }
}

void csMeshWrapper::UpdateLighting (iLight **lights, int num_lights)
{
  defered_num_lights = 0;

  //if (num_lights <= 0) return;
  meshobj->UpdateLighting (lights, num_lights, &movable.scfiMovable);

  int i;
  for (i = 0; i < children.GetCount (); i++)
  {
    iMeshWrapper *spr = children.Get (i);
    spr->UpdateLighting (lights, num_lights);
  }
}

void csMeshWrapper::PlaceMesh ()
{
  iSectorList *movable_sectors = movable.GetSectors ();
  if (movable_sectors->GetCount () == 0) return ; // Do nothing
  csSphere sphere;
  csVector3 radius;
  meshobj->GetObjectModel ()->GetRadius (radius, sphere.GetCenter ());

  iSector *sector = movable_sectors->Get (0);
  movable.SetSector (sector);       // Make sure all other sectors are removed

  // Transform the sphere from object to world space.
  float max_radius = radius.x;
  if (max_radius < radius.y) max_radius = radius.y;
  if (max_radius < radius.z) max_radius = radius.z;
  sphere.SetRadius (max_radius);
  if (!movable.IsFullTransformIdentity ())
    sphere = movable.GetFullTransform ().This2Other (sphere);
  max_radius = sphere.GetRadius ();

  float max_sq_radius = max_radius * max_radius;

  // @@@ This function only goes one level deep in portals. Should be fixed!
  // @@@ It would be nice if we could find a more optimal portal representation
  // for large sectors.

  // @@@ THIS FUNCTION SHOULD USE GetNearbyObjects()!!!

  int i, j;
  iMeshList *ml = sector->GetMeshes ();
  for (i = 0; i < ml->GetCount (); i++)
  {
    iMeshWrapper *mesh = ml->Get (i);

    if (mesh->GetMeshObject ()->GetPortalCount () == 0)
      continue;		// No portals to consider.

    csRef<iThingState> thing (SCF_QUERY_INTERFACE (
        mesh->GetMeshObject (),
        iThingState));
    if (thing)
    {
      // @@@ This function will currently only consider portals on things
      // that cannot move.
      if (thing->GetMovingOption () == CS_THING_MOVE_NEVER)
      {
        iThingFactoryState* thing_fact = thing->GetFactory ();
        for (j = 0; j < thing_fact->GetPortalCount (); j++)
        {
          iPortal *portal = thing_fact->GetPortal (j);
          iSector *dest_sector = portal->GetSector ();
          if (movable_sectors->Find (dest_sector) == -1)
          {
            iPolygon3D *portal_poly = thing->GetPortalPolygon (j);
            const csPlane3 &pl = portal_poly->GetWorldPlane ();

            float sqdist = csSquaredDist::PointPlane (
                sphere.GetCenter (),
                pl);
            if (sqdist <= max_sq_radius)
            {
              // Plane of portal is close enough.
              // If N is the normal of the portal plane then we
              // can use that to calculate the point on the portal plane.
              csVector3 testpoint = sphere.GetCenter () + pl.Normal () * qsqrt (
                  sqdist);
              if (portal_poly->GetStaticData ()->PointOnPolygon (testpoint))
                movable_sectors->Add (dest_sector);
            }
          }
        }
      }
    }
  }
}

int csMeshWrapper::HitBeamBBox (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect,
  float *pr)
{
  csBox3 b;
  meshobj->GetObjectModel ()->GetObjectBoundingBox (b, CS_BBOX_MAX);

  csSegment3 seg (start, end);
  return csIntersect3::BoxSegment (b, seg, isect, pr);
}

bool csMeshWrapper::HitBeamOutline (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect,
  float *pr)
{
  return meshobj->HitBeamOutline (start, end, isect, pr);
}

bool csMeshWrapper::HitBeamObject (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect,
  float *pr)
{
  return meshobj->HitBeamObject (start, end, isect, pr);
}

bool csMeshWrapper::HitBeam (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect,
  float *pr)
{
  csVector3 startObj;
  csVector3 endObj;
  csReversibleTransform trans;
  if (movable.IsFullTransformIdentity ())
  {
    startObj = start;
    endObj = end;
  }
  else
  {
    trans = movable.GetFullTransform ();
    startObj = trans.Other2This (start);
    endObj = trans.Other2This (end);
  }
  bool rc = false;
  if (HitBeamBBox (startObj, endObj, isect, NULL) > -1)
  {
    rc = HitBeamOutline (startObj, endObj, isect, pr);
    if (rc)
    {
      if (!movable.IsFullTransformIdentity ())
        isect = trans.This2Other (isect);
    }
  }

  return rc;
}

void csMeshWrapper::HardTransform (const csReversibleTransform &t)
{
  meshobj->HardTransform (t);

  int i;
  for (i = 0; i < children.GetCount (); i++)
  {
    iMeshWrapper *spr = children.Get (i);
    spr->HardTransform (t);
  }
}

void csMeshWrapper::GetWorldBoundingBox (csBox3 &cbox)
{
  if (wor_bbox_movablenr != movable.GetUpdateNumber ())
  {
    wor_bbox_movablenr = movable.GetUpdateNumber ();

    if (movable.IsFullTransformIdentity ())
      meshobj->GetObjectModel ()->GetObjectBoundingBox (wor_bbox, CS_BBOX_MAX);
    else
    {
      csBox3 obj_bbox;
      meshobj->GetObjectModel ()->GetObjectBoundingBox (obj_bbox, CS_BBOX_MAX);

      // @@@ Maybe it would be better to really calculate the bounding box
      // here instead of just transforming the object space bounding box?
      csReversibleTransform mt = movable.GetFullTransform ();
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

  cbox = wor_bbox;
}

void csMeshWrapper::GetRadius (csVector3 &rad, csVector3 &cent) const
{
  meshobj->GetObjectModel ()->GetRadius (rad, cent);
  if (children.GetCount () > 0)
  {
    float max_radius = rad.x;
    if (max_radius < rad.y) max_radius = rad.y;
    if (max_radius < rad.z) max_radius = rad.z;

    csSphere sphere (cent, max_radius);
    int i;
    for (i = 0; i < children.GetCount (); i++)
    {
      iMeshWrapper *spr = children.Get (i);
      csVector3 childrad, childcent;
      spr->GetRadius (childrad, childcent);

      float child_max_radius = childrad.x;
      if (child_max_radius < childrad.y) child_max_radius = childrad.y;
      if (child_max_radius < childrad.z) child_max_radius = childrad.z;

      csSphere childsphere (childcent, child_max_radius);

      // @@@ Is this the right transform?
      childsphere *= spr->GetMovable ()->GetTransform ();
      sphere += childsphere;
    }

    rad.Set (sphere.GetRadius (), sphere.GetRadius (), sphere.GetRadius ());
    cent.Set (sphere.GetCenter ());
  }
}

void csMeshWrapper::GetTransformedBoundingBox (
  const csReversibleTransform &trans,
  csBox3 &cbox)
{
  csBox3 box;
  meshobj->GetObjectModel ()->GetObjectBoundingBox (box);
  cbox.StartBoundingBox (trans * box.GetCorner (0));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (1));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (2));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (3));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (4));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (5));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (6));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (7));
}

float csMeshWrapper::GetScreenBoundingBox (
  const iCamera *camera,
  csBox2 &sbox,
  csBox3 &cbox)
{
  csVector2 oneCorner;
  csReversibleTransform tr_o2c = camera->GetTransform ();
  if (!movable.IsFullTransformIdentity ())
    tr_o2c /= movable.GetFullTransform ();
  GetTransformedBoundingBox (tr_o2c, cbox);

  // if the entire bounding box is behind the camera, we're done
  if ((cbox.MinZ () < 0) && (cbox.MaxZ () < 0))
  {
    return -1;
  }

  // Transform from camera to screen space.
  if (cbox.MinZ () <= 0)
  {
    // Mesh is very close to camera.
    // Just return a maximum bounding box.
    sbox.Set (-10000, -10000, 10000, 10000);
  }
  else
  {
    camera->Perspective (cbox.Max (), oneCorner);
    sbox.StartBoundingBox (oneCorner);

    csVector3 v (cbox.MinX (), cbox.MinY (), cbox.MaxZ ());
    camera->Perspective (v, oneCorner);
    sbox.AddBoundingVertexSmart (oneCorner);
    camera->Perspective (cbox.Min (), oneCorner);
    sbox.AddBoundingVertexSmart (oneCorner);
    v.Set (cbox.MaxX (), cbox.MaxY (), cbox.MinZ ());
    camera->Perspective (v, oneCorner);
    sbox.AddBoundingVertexSmart (oneCorner);
  }

  return cbox.MaxZ ();
}

float csMeshWrapper::MeshWrapper::GetScreenBoundingBox (
  iCamera *camera,
  csBox2 &sbox,
  csBox3 &cbox)
{
  return scfParent->GetScreenBoundingBox (camera, sbox, cbox);
}

// ---------------------------------------------------------------------------
// csMeshFactoryWrapper
// ---------------------------------------------------------------------------
SCF_IMPLEMENT_IBASE_EXT(csMeshFactoryWrapper)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iMeshFactoryWrapper)
  SCF_IMPLEMENTS_INTERFACE(csMeshFactoryWrapper)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csMeshFactoryWrapper::MeshFactoryWrapper)
  SCF_IMPLEMENTS_INTERFACE(iMeshFactoryWrapper)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csMeshFactoryWrapper::csMeshFactoryWrapper (
  iMeshObjectFactory *meshFact)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiMeshFactoryWrapper);
  csMeshFactoryWrapper::meshFact = meshFact;
  parent = NULL;
  children.SetMeshFactory (this);
  csEngine::current_engine->AddToCurrentRegion (this);
}

csMeshFactoryWrapper::csMeshFactoryWrapper ()
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiMeshFactoryWrapper);
  parent = NULL;
  children.SetMeshFactory (this);
  csEngine::current_engine->AddToCurrentRegion (this);
}

csMeshFactoryWrapper::~csMeshFactoryWrapper ()
{
}

void csMeshFactoryWrapper::SetMeshObjectFactory (iMeshObjectFactory *meshFact)
{
  csMeshFactoryWrapper::meshFact = meshFact;
}

iMeshWrapper *csMeshFactoryWrapper::NewMeshObject ()
{
  csRef<iMeshObject> basemesh (meshFact->NewInstance ());
  iMeshWrapper *mesh = &(new csMeshWrapper (NULL, basemesh))->scfiMeshWrapper;

  if (GetName ()) mesh->QueryObject ()->SetName (GetName ());
  mesh->SetFactory (&scfiMeshFactoryWrapper);

  int i;
  for (i = 0; i < children.GetCount (); i++)
  {
    iMeshFactoryWrapper *childfact = children.Get (i);
    iMeshWrapper *child = childfact->CreateMeshWrapper ();
    mesh->GetChildren ()->Add (child);
    child->GetMovable ()->SetTransform (childfact->GetTransform ());
    child->GetMovable ()->UpdateMove ();
    child->DecRef ();
  }

  return mesh;
}

void csMeshFactoryWrapper::HardTransform (const csReversibleTransform &t)
{
  meshFact->HardTransform (t);
}

//--------------------------------------------------------------------------
// csMeshList
//--------------------------------------------------------------------------
SCF_IMPLEMENT_IBASE(csMeshList)
  SCF_IMPLEMENTS_INTERFACE(iMeshList)
SCF_IMPLEMENT_IBASE_END

csMeshList::csMeshList ()
{
  SCF_CONSTRUCT_IBASE (NULL);
}

csMeshList::~csMeshList ()
{
  RemoveAll ();
}

iMeshWrapper* csMeshList::FindByNameWithChild (const char *Name) const
{
  char* p = strchr (Name, ':');
  if (!p) return list.FindByName (Name);

  int i;
  for (i = 0 ; i < list.Length () ; i++)
  {
    iMeshWrapper* m = list.Get (i);
    if (!strncmp (m->QueryObject ()->GetName (), Name, p-Name))
    {
      return m->GetChildren ()->FindByName (p+1);
    }
  }
  return NULL;
}

int csMeshList::Add (iMeshWrapper *obj)
{
  PrepareItem (obj);
  list.Push (obj);
  return true;
}

bool csMeshList::Remove (iMeshWrapper *obj)
{
  FreeItem (obj);
  list.Delete (obj);
  return true;
}

bool csMeshList::Remove (int n)
{
  FreeItem (list[n]);
  list.Delete (n);
  return true;
}

void csMeshList::RemoveAll ()
{
  int i;
  for (i = 0 ; i < list.Length () ; i++)
  {
    FreeItem (list[i]);
  }
  list.DeleteAll ();
}

int csMeshList::Find (iMeshWrapper *obj) const
{
  return list.Find (obj);
}

iMeshWrapper *csMeshList::FindByName (const char *Name) const
{
  if (strchr (Name, ':'))
    return FindByNameWithChild (Name);
  else
    return list.FindByName (Name);
}

//--------------------------------------------------------------------------
// csMeshMeshList
//--------------------------------------------------------------------------
void csMeshMeshList::PrepareItem (iMeshWrapper* child)
{
  CS_ASSERT (mesh != NULL);
  csMeshList::PrepareItem (child);

  // unlink the mesh from the engine or another parent.
  iMeshWrapper *oldParent = child->GetParentContainer ();
  if (oldParent)
    oldParent->GetChildren ()->Remove (child);
  else
    csEngine::current_engine->GetMeshes ()->Remove (child);

  /* csSector->PrepareMesh tells the culler about the mesh
     (since removing the mesh above also removes it from the culler...) */
  for (int i = 0 ; i < mesh->GetMovable().GetSectors()->GetCount() ; i++)
  {
    csSector* sector = mesh->GetMovable ().GetSectors ()->Get (i)
    	->GetPrivateObject ();
    sector->UnprepareMesh (child);
    sector->PrepareMesh (child);
  }

  child->SetParentContainer (&mesh->scfiMeshWrapper);
  child->GetMovable ()->SetParent (&mesh->GetMovable ().scfiMovable);
}

void csMeshMeshList::FreeItem (iMeshWrapper* item)
{
  CS_ASSERT (mesh != NULL);

  for (int i = 0 ; i < mesh->GetMovable().GetSectors()->GetCount() ; i++)
  {
    csSector* sector = mesh->GetMovable ().GetSectors ()->Get (i)
        ->GetPrivateObject ();
    sector->UnprepareMesh (item);
  }

  item->SetParentContainer (NULL);
  item->GetMovable ()->SetParent (NULL);
  csMeshList::FreeItem (item);
}

//--------------------------------------------------------------------------
// csMeshFactoryList
//--------------------------------------------------------------------------
SCF_IMPLEMENT_IBASE(csMeshFactoryList)
  SCF_IMPLEMENTS_INTERFACE(iMeshFactoryList)
SCF_IMPLEMENT_IBASE_END

csMeshFactoryList::csMeshFactoryList ()
{
  SCF_CONSTRUCT_IBASE (NULL);
}

int csMeshFactoryList::Add (iMeshFactoryWrapper *obj)
{
  PrepareItem (obj);
  list.Push (obj);
  return true;
}

bool csMeshFactoryList::Remove (iMeshFactoryWrapper *obj)
{
  FreeItem (obj);
  list.Delete (obj);
  return true;
}

bool csMeshFactoryList::Remove (int n)
{
  FreeItem (list[n]);
  list.Delete (Get (n));
  return true;
}

void csMeshFactoryList::RemoveAll ()
{
  int i;
  for (i = 0 ; i < list.Length () ; i++)
  {
    FreeItem (list[i]);
  }
  list.DeleteAll ();
}

int csMeshFactoryList::Find (iMeshFactoryWrapper *obj) const
{
  return list.Find (obj);
}

iMeshFactoryWrapper *csMeshFactoryList::FindByName (
  const char *Name) const
{
  return list.FindByName (Name);
}

//--------------------------------------------------------------------------
// csMeshFactoryFactoryList
//--------------------------------------------------------------------------
void csMeshFactoryFactoryList::PrepareItem (iMeshFactoryWrapper* child)
{
  CS_ASSERT (meshfact != NULL);
  csMeshFactoryList::PrepareItem (child);

  // unlink the factory from another possible parent.
  if (child->GetParentContainer ())
    child->GetParentContainer ()->GetChildren ()->Remove (child);

  child->SetParentContainer (&meshfact->scfiMeshFactoryWrapper);
}

void csMeshFactoryFactoryList::FreeItem (iMeshFactoryWrapper* item)
{
  CS_ASSERT (meshfact != NULL);
  item->SetParentContainer (NULL);
  csMeshFactoryList::FreeItem (item);
}
