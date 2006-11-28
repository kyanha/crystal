/*
    Copyright (C) 1998-2001 by Jorrit Tyberghein
              (C) 2004 by Marten Svanfeldt

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
#include "csgeom/kdtree.h"
#include "csutil/csppulse.h"
#include "csutil/csstring.h"
#include "csutil/debug.h"
#include "cstool/csview.h"
#include "iengine/portal.h"
#include "iengine/rview.h"
#include "igeom/clip2d.h"
#include "imesh/lighting.h"
#include "iutil/objreg.h"
#include "iutil/plugin.h"
#include "iutil/vfs.h"
#include "iutil/virtclk.h"
#include "ivaria/bugplug.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/rendermesh.h"
#include "ivideo/texture.h"
#include "ivideo/txtmgr.h"

#include "plugins/engine/3d/engine.h"
#include "plugins/engine/3d/light.h"
#include "plugins/engine/3d/material.h"
#include "plugins/engine/3d/rview.h"
#include "plugins/engine/3d/sector.h"
#include "plugins/engine/3d/meshgen.h"
#include "plugins/engine/3d/meshobj.h"

//---------------------------------------------------------------------------
csSectorLightList::csSectorLightList ()
{
  sector = 0;
  kdtree = new csKDTree ();
}

csSectorLightList::~csSectorLightList ()
{
  RemoveAll ();
  delete kdtree;
}

void csSectorLightList::PrepareLight (iLight* item)
{
  csLight* clight = ((csLight*)item)->GetPrivateObject ();
  csLightList::PrepareLight (item);

  clight->SetSector ((iSector*)sector);

  const csVector3& center = item->GetCenter ();
  float radius = item->GetCutoffDistance ();
  csBox3 lightbox (center - csVector3 (radius), center + csVector3 (radius));
  csKDTreeChild* childnode = kdtree->AddObject (lightbox, (void*)item);

  clight->SetChildNode (childnode);
  if (sector->use_lightculling)
  {
    sector->RegisterLightToCuller (clight);
  }

}

void csSectorLightList::FreeLight (iLight* item)
{

  csLight* clight = ((csLight*)item)->GetPrivateObject ();
  clight->SetSector (0);
  kdtree->RemoveObject (clight->GetChildNode ());

  csLightList::FreeLight (item);
  if (sector->use_lightculling)
  {
    sector->UnregisterLightToCuller (clight);
  }
}

//--------------------------------------------------------------------------

csSectorMeshList::csSectorMeshList () : csMeshList (64, 128)
{
  sector = 0;
}

void csSectorMeshList::PrepareMesh (iMeshWrapper* item)
{
  CS_ASSERT (sector != 0);
  csMeshList::PrepareMesh (item);
  sector->PrepareMesh (item);
}

void csSectorMeshList::FreeMesh (iMeshWrapper* item)
{
  CS_ASSERT (sector != 0);
  sector->UnprepareMesh (item);
  csMeshList::FreeMesh (item);
}

//---------------------------------------------------------------------------

csSector::csSector (csEngine *engine) :
  scfImplementationType (this), engine (engine)
{
  DG_TYPE (this, "csSector");
  drawBusy = 0;
  dynamicAmbientLightColor.Set (0,0,0);
  dynamicAmbientLightVersion = (uint)~0;
  meshes.SetSector (this);
  //portal_containers.SetSector (this);
  lights.SetSector (this);
  currentVisibilityNumber = 0;
  renderloop = 0;
  use_lightculling = false;
  single_mesh = 0;

  SetupSVNames();
  svDynamicAmbient.AttachNew (new csShaderVariable (SVNames().dynamicAmbient));
  svDynamicAmbient->SetValue (dynamicAmbientLightColor);
  AddVariable (svDynamicAmbient);
  svFogColor.AttachNew (new csShaderVariable (SVNames().fogColor));
  AddVariable (svFogColor);
  svFogMode.AttachNew (new csShaderVariable (SVNames().fogMode));
  AddVariable (svFogMode);
  svFogStart.AttachNew (new csShaderVariable (SVNames().fogStart));
  AddVariable (svFogStart);
  svFogEnd.AttachNew (new csShaderVariable (SVNames().fogEnd));
  AddVariable (svFogEnd);
  svFogDensity.AttachNew (new csShaderVariable (SVNames().fogDensity));
  AddVariable (svFogDensity);
  UpdateFogSVs();
}

csSector::~csSector ()
{
  CleanupLSI ();
  lights.RemoveAll ();
}

void csSector::SelfDestruct ()
{
  engine->GetSectors ()->Remove ((iSector*)this);
}

void csSector::RegisterLightToCuller (csLight* light)
{
  light->UseAsCullingObject ();
  csRef<iVisibilityObject> vo = 
        scfQueryInterface<iVisibilityObject> (light);
  culler->RegisterVisObject (vo);
}

void csSector::UnregisterLightToCuller (csLight* light)
{
  csRef<iVisibilityObject> vo = 
        scfQueryInterface<iVisibilityObject> (light);
  culler->UnregisterVisObject (vo);
  light->StopUsingAsCullingObject ();
}

void csSector::SetLightCulling (bool enable)
{
  if (enable == use_lightculling) return;
  use_lightculling = enable;
  int i;
  if (use_lightculling)
  {
    for (i = 0; i < lights.GetCount (); i++)
    {
      iLight* l = lights.Get (i);
      csLight* clight = ((csLight*)l)->GetPrivateObject ();
      RegisterLightToCuller (clight);
    }
  }
  else
  {
    for (i = 0; i < lights.GetCount (); i++)
    {
      iLight* l = lights.Get (i);
      csLight* clight = ((csLight*)l)->GetPrivateObject ();
      UnregisterLightToCuller (clight);
    }
  }
}

void csSector::AddLightVisibleCallback (iLightVisibleCallback* cb)
{
  lightVisibleCallbackList.Push (cb);
}

void csSector::RemoveLightVisibleCallback (iLightVisibleCallback* cb)
{
  lightVisibleCallbackList.Delete (cb);
}

void csSector::FireLightVisibleCallbacks (iLight* light)
{
  size_t i = lightVisibleCallbackList.Length ();
  while (i > 0)
  {
    i--;
    lightVisibleCallbackList[i]->LightVisible ((iSector*)this, light);
  }
}

void csSector::UnlinkObjects ()
{
  int i;
  for (i = 0; i < meshes.GetCount (); i++)
  {
    iMeshWrapper* m = meshes.Get (i);
    iSectorList* sl = m->GetMovable ()->GetSectors ();
    sl->Remove ((iSector*)this);
    m->GetMovable ()->UpdateMove ();
  }
  for (i = 0; i < lights.GetCount (); i++)
  {
    iLight* l = lights.Get (i);
    iSectorList* sl = l->GetMovable ()->GetSectors ();
    sl->Remove ((iSector*)this);
    l->GetMovable ()->UpdateMove ();
  }
}

//----------------------------------------------------------------------

void csSector::RegisterEntireMeshToCuller (iMeshWrapper* mesh)
{
  csMeshWrapper* cmesh = (csMeshWrapper*)mesh;
  if (cmesh->SomeParentHasStaticLOD ()) return;

  csRef<iVisibilityObject> vo = 
        scfQueryInterface<iVisibilityObject> (mesh);
  culler->RegisterVisObject (vo);

  if (cmesh->GetStaticLODMesh ()) return;
  size_t i;
  const csRefArray<iSceneNode>& ml = cmesh->GetChildren ();
  for (i = 0 ; i < ml.Length () ; i++)
  {
    iMeshWrapper* child = ml[i]->QueryMesh ();
    if (child)
      RegisterEntireMeshToCuller (child);
  }
}

void csSector::RegisterMeshToCuller (iMeshWrapper* mesh)
{
  csMeshWrapper* cmesh = (csMeshWrapper*)mesh;
  if (cmesh->SomeParentHasStaticLOD ()) return;

  csRef<iVisibilityObject> vo = 
        scfQueryInterface<iVisibilityObject> (mesh);
  culler->RegisterVisObject (vo);
}

void csSector::UnregisterMeshToCuller (iMeshWrapper* mesh)
{
  csRef<iVisibilityObject> vo = 
        scfQueryInterface<iVisibilityObject> (mesh);
  culler->UnregisterVisObject (vo);
}

void csSector::PrepareMesh (iMeshWrapper *mesh)
{
  bool do_camera = mesh->GetFlags ().Check (CS_ENTITY_CAMERA);
  if (do_camera) cameraMeshes.Push (mesh);

  if (culler) RegisterMeshToCuller (mesh);
  size_t i;
  const csRefArray<iSceneNode>& ml = ((csMeshWrapper*)mesh)->GetChildren ();
  for (i = 0 ; i < ml.Length () ; i++)
  {
    iMeshWrapper* child = ml[i]->QueryMesh ();
    if (child)
      PrepareMesh (child);
  }
}

void csSector::UnprepareMesh (iMeshWrapper *mesh)
{
  cameraMeshes.Delete (mesh);

  if (culler) UnregisterMeshToCuller (mesh);
  size_t i;
  const csRefArray<iSceneNode>& ml = ((csMeshWrapper*)mesh)->GetChildren ();
  for (i = 0 ; i < ml.Length () ; i++)
  {
    iMeshWrapper* child = ml[i]->QueryMesh ();
    if (child)
      UnprepareMesh (child);
  }
}

void csSector::RelinkMesh (iMeshWrapper *mesh)
{
  cameraMeshes.Delete (mesh);
  bool do_camera = mesh->GetFlags ().Check (CS_ENTITY_CAMERA);
  if (do_camera) cameraMeshes.Push (mesh);

  size_t i;
  const csRefArray<iSceneNode>& ml = ((csMeshWrapper*)mesh)->GetChildren ();
  for (i = 0 ; i < ml.Length () ; i++)
  {
    iMeshWrapper* child = ml[i]->QueryMesh ();
    if (child)
      RelinkMesh (child);
  }
}

void csSector::PrecacheDraw ()
{
  GetVisibilityCuller ()->PrecacheCulling ();

  // First calculate the box of all objects in the level.
  csBox3 box;
  box.StartBoundingBox ();
  int i;
  for (i = 0; i < meshes.GetCount (); i++)
  {
    iMeshWrapper* m = meshes.Get (i);
    const csBox3& mesh_box = m->GetWorldBoundingBox ();
    box += mesh_box;
  }

  // Try to position our camera somewhere above the bounding
  // box of the sector so we see as much as possible.
  csVector3 pos = box.GetCenter ();
  pos.y = box.MaxY () + (box.MaxY () - box.MinY ());
  csVector3 lookat = pos;
  lookat.y = box.MinY ();

  csRef<iGraphics3D> g3d = csQueryRegistry<iGraphics3D> (
      engine->objectRegistry);
  csRef<csView> view;
  view.AttachNew (new csView (engine, g3d));
  iGraphics2D* g2d = g3d->GetDriver2D ();
  view->SetRectangle (0, 0, g2d->GetWidth (), g2d->GetHeight ());

  iCamera* camera = view->GetCamera ();
  camera->SetSector (this);
  camera->GetTransform ().SetOrigin (pos);
  camera->GetTransform ().LookAt (lookat-pos, csVector3 (0, 0, 1));

  // @@@ Ideally we would want to disable visibility culling
  // here so that all objects are visible.
  g3d->BeginDraw (CSDRAW_3DGRAPHICS);
  view->Draw ();
  g3d->FinishDraw ();
}

//----------------------------------------------------------------------

bool csSector::SetVisibilityCullerPlugin (const char *plugname,
	iDocumentNode* culler_params)
{
  if (use_lightculling)
  {
    int i;
    for (i = 0; i < lights.GetCount (); i++)
    {
      iLight* l = lights.Get (i);
      csLight* clight = ((csLight*)l)->GetPrivateObject ();
      UnregisterLightToCuller (clight);
    }
  }

  culler = 0;

  // Load the culler plugin.
  csRef<iPluginManager> plugmgr = 
  	csQueryRegistry<iPluginManager> (engine->objectRegistry);
  culler = CS_LOAD_PLUGIN (plugmgr, plugname, iVisibilityCuller);

  if (!culler)
  {
    return false;
  }

  const char* err = culler->ParseCullerParameters (culler_params);
  if (err)
  {
    engine->Error ("Error loading visibility culler: %s!",
    	err);
    return false;
  }

  // load cache data
  csString cachename;
  cachename.Format ("%s_%s", GetName (), plugname);
  culler->Setup (cachename);

  // Loop through all meshes and register them to the visibility culler.
  int i;
  for (i = 0; i < meshes.GetCount (); i++)
  {
    iMeshWrapper* m = meshes.Get (i);
    m->GetMovable ()->UpdateMove ();
    RegisterEntireMeshToCuller (m);
  }
  if (use_lightculling)
  {
    for (i = 0; i < lights.GetCount (); i++)
    {
      iLight* l = lights.Get (i);
      csLight* clight = ((csLight*)l)->GetPrivateObject ();
      RegisterLightToCuller (clight);
    }
  }
  return true;
}

iVisibilityCuller* csSector::GetVisibilityCuller ()
{
  if (!culler) SetVisibilityCullerPlugin ("crystalspace.culling.frustvis");
  CS_ASSERT (culler != 0);
  return culler;
}

class csSectorVisibleMeshCallback : public scfImplementation1<
	csSectorVisibleMeshCallback,
	iVisibilityCullerListener>
{
public:

  csSectorVisibleMeshCallback ()
    : scfImplementationType (this), privMeshlist (0)
  {
  }

  virtual ~csSectorVisibleMeshCallback()
  {
  }

  // NR version
  void Setup (csRenderMeshList *meshlist, iRenderView *rview, iSector* sector)
  {
    privMeshlist = meshlist;
    csSectorVisibleMeshCallback::rview = rview;
    csSectorVisibleMeshCallback::sector = sector;
  }

  void MarkMeshAndChildrenVisible (iMeshWrapper* mesh, uint32 frustum_mask)
  {
    csMeshWrapper* cmesh = (csMeshWrapper*)mesh;
    ObjectVisible (cmesh, frustum_mask);
    size_t i;
    const csRefArray<iSceneNode>& children = cmesh->GetChildren ();
    for (i = 0 ; i < children.Length () ; i++)
    {
      iMeshWrapper* child = children[i]->QueryMesh ();
      // @@@ Traverse too in case there are lights/cameras?
      if (child)
        MarkMeshAndChildrenVisible (child, frustum_mask);
    }
  }

  void ObjectVisible (csMeshWrapper* cmesh, uint32 frustum_mask)
  {
    csStaticLODMesh* static_lod = cmesh->GetStaticLODMesh ();
    bool mm = cmesh->DoMinMaxRange ();
    float distance = 0;
    if (static_lod || mm)
      distance = csQsqrt (cmesh->GetSquaredDistance (rview));

    if (mm)
    {
      if (distance < cmesh->csMeshWrapper::GetMinimumRenderDistance ())
        return;
      if (distance > cmesh->csMeshWrapper::GetMaximumRenderDistance ())
        return;
    }

    if (static_lod)
    {
      float lod = static_lod->GetLODValue (distance);
      csArray<iMeshWrapper*>& meshes = static_lod->GetMeshesForLOD (lod);
      size_t i;
      for (i = 0 ; i < meshes.Length () ; i++)
        MarkMeshAndChildrenVisible (meshes[i], frustum_mask);
    }

    int num;
    csRenderMesh** meshes = cmesh->GetRenderMeshes (num, rview, frustum_mask);
    CS_ASSERT(!((num != 0) && (meshes == 0)));
#ifdef CS_DEBUG
    int i;
    for (i = 0 ; i < num ; i++)
      meshes[i]->db_mesh_name = cmesh->GetName ();
#endif
    if (num > 0)
    {
      privMeshlist->AddRenderMeshes (meshes, num,
      	cmesh->csMeshWrapper::GetRenderPriority (),
	cmesh->csMeshWrapper::GetZBufMode (), (iMeshWrapper*)cmesh);
    }
  }

  virtual void ObjectVisible (iVisibilityObject* visobj, iMeshWrapper *mesh,
  	uint32 frustum_mask)
  {
    if (mesh)
    {
      csMeshWrapper* cmesh = (csMeshWrapper*)mesh;
      ObjectVisible (cmesh, frustum_mask);
    }
    else
    {
      csRef<iLight> light = scfQueryInterface<iLight> (visobj);
      if (light)
      {
        csSector* csector = (csSector*)sector;
        csector->FireLightVisibleCallbacks (light);
      }
    }
  }

private:
  csRenderMeshList *privMeshlist;
  iRenderView *rview;
  iSector* sector;
};

CS_IMPLEMENT_STATIC_VAR (GetVisMeshCb, csSectorVisibleMeshCallback, ())

csRenderMeshList *csSector::GetVisibleMeshes (iRenderView *rview)
{
  if (rview == 0) return 0;

  // This is used for csMeshObject::IsChildVisible to determine
  // when to update its cache. Should be changed to something more
  // sensible. @@@ What about engine frame number?
  currentVisibilityNumber++;

/*  if (engine->GetCurrentFrameNumber () != cachedFrameNumber ||
      rview->GetCamera () != cachedCamera )
  {
    visibleMeshCache->Empty ();
    cb.Setup (visibleMeshCache, rview);
    GetVisibilityCuller()->VisTest (rview, GetVisMeshCb ());

    cachedFrameNumber = engine->GetCurrentFrameNumber ();
    cachedCamera = rview->GetCamera();
  }

  return visibleMeshCache;*/

  size_t i;
  uint32 cur_framenr = engine->GetCurrentFrameNumber ();
  uint32 cur_context_id = rview->GetRenderContext ()->context_id;
  for (i = 0 ; i < visibleMeshCache.Length () ; i++)
  {
    visibleMeshCacheHolder& entry = visibleMeshCache[i];
    if (entry.cachedFrameNumber == cur_framenr &&
        entry.cached_context_id == cur_context_id)
    {
      return entry.meshList;
    }
  }

  //try to find a spot to do a new cache
  for (i = 0 ; i < visibleMeshCache.Length () ; i++)
  {
    visibleMeshCacheHolder& entry = visibleMeshCache[i];
    if (entry.cachedFrameNumber != cur_framenr)
    {
      //use this slot
      entry.meshList->Empty ();
      GetVisMeshCb ()->Setup (entry.meshList, rview, (iSector*)this);

      if (single_mesh)
	GetVisMeshCb ()->ObjectVisible ((csMeshWrapper*)single_mesh, ~0);
      else
        GetVisibilityCuller()->VisTest (rview, GetVisMeshCb ());

      entry.cachedFrameNumber = cur_framenr;
      entry.cached_context_id = cur_context_id;
      return entry.meshList;
    }
  }

  //create a new cache entry
  visibleMeshCacheHolder holder;
  holder.cachedFrameNumber = cur_framenr;
  holder.cached_context_id = cur_context_id;
  holder.meshList = new csRenderMeshList (engine);
  usedMeshLists.Push (holder.meshList);
  GetVisMeshCb ()->Setup (holder.meshList, rview, (iSector*)this);
  if (single_mesh)
    GetVisMeshCb ()->ObjectVisible ((csMeshWrapper*)single_mesh, ~0);
  else
    GetVisibilityCuller()->VisTest (rview, GetVisMeshCb ());
  visibleMeshCache.Push (holder);
  return holder.meshList;
}


csSectorHitBeamResult csSector::HitBeamPortals (
  const csVector3 &start,
  const csVector3 &end)
{
  csSectorHitBeamResult rc;
  rc.mesh = 0;
  rc.final_sector = static_cast<iSector*> (this);
  int p = IntersectSegment (start, end, rc.isect, 0, false,
		  &rc.mesh);
  if (p != -1)
  {
    iPortalContainer* portals = rc.mesh->GetPortalContainer ();
    if (portals)
    {
      // There are portals.
      iPortal* po = portals->GetPortal (p);
      if (po)
      {
	drawBusy++;
	csVector3 new_start = rc.isect;
	rc.mesh = po->HitBeamPortals (rc.mesh->GetMovable ()
		->GetFullTransform (),
		new_start, end, rc.isect, &p, &rc.final_sector);
	drawBusy--;
      }
    }
  }
  rc.polygon_idx = p;
  return rc;
}

iMeshWrapper* csSector::HitBeamPortals (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect,
  int* polygon_idx,
  iSector** final_sector)
{
  iMeshWrapper* mesh = 0;
  int p = IntersectSegment (start, end, isect, 0, false, &mesh);
  if (final_sector) *final_sector = static_cast<iSector*> (this);
  if (p != -1)
  {
    iPortalContainer* portals = mesh->GetPortalContainer ();
    if (portals)
    {
      // There are portals.
      iPortal* po = portals->GetPortal (p);
      if (po)
      {
	drawBusy++;
	csVector3 new_start = isect;
	mesh = po->HitBeamPortals (mesh->GetMovable ()->GetFullTransform (),
		      new_start, end, isect, &p, final_sector);
	drawBusy--;
      }
    }
  }
  if (polygon_idx) *polygon_idx = p;
  return mesh;
}

csSectorHitBeamResult csSector::HitBeam (
  const csVector3 &start,
  const csVector3 &end,
  bool accurate)
{
  GetVisibilityCuller ();
  float r;
  csSectorHitBeamResult rc;
  rc.mesh = 0;
  rc.polygon_idx = -1;
  rc.final_sector = 0;
  bool result = culler->IntersectSegment (start, end, rc.isect, &r, &rc.mesh,
  	&rc.polygon_idx, accurate);
  if (!result) rc.mesh = 0;
  return rc;
}

iMeshWrapper *csSector::HitBeam (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect,
  int *polygonPtr,
  bool accurate)
{
  GetVisibilityCuller ();
  float r;
  iMeshWrapper* mesh = 0;
  int poly = -1;
  bool rc = culler->IntersectSegment (start, end, isect, &r, &mesh, &poly,
  	accurate);
  if (polygonPtr) *polygonPtr = poly;
  if (rc)
    return mesh;
  else
    return 0;
}

int csSector::IntersectSegment (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect,
  float *pr,
  bool only_portals,
  iMeshWrapper **p_mesh)
{
  GetVisibilityCuller ();
  float r, best_r = 10000000000.;
  csVector3 cur_isect;
  int best_p = -1;
  csVector3 obj_start, obj_end, obj_isect;

  if (!only_portals)
  {
    iMeshWrapper *mesh;
    int poly;
    bool rc = culler->IntersectSegment (start, end, isect, &r, &mesh, &poly);
    if (rc)
    {
      if (poly != -1) best_p = poly;
      best_r = r;
      if (p_mesh) *p_mesh = mesh;
    }
    if (pr) *pr = best_r;
    return best_p;
  }

  csReversibleTransform movtrans;

  GetVisibilityCuller ();
  csBox3 b (start);
  b.AddBoundingVertexSmart (end);
  csRef<iVisibilityObjectIterator> visit = culler->VisTest (b);

  while (visit->HasNext ())
  {
    iVisibilityObject* vo = visit->Next ();
    iMeshWrapper* mesh = vo->GetMeshWrapper ();
    if (!mesh || mesh->GetFlags ().Check (CS_ENTITY_NOHITBEAM)) continue;

    // Only operate on portal objects.
    if (!mesh->GetPortalContainer ()) continue;

    bool has_not_moved = mesh->GetMovable ()->IsFullTransformIdentity ();
    if (has_not_moved)
    {
      obj_start = start;
      obj_end = end;
    }
    else
    {
      movtrans = mesh->GetMovable ()->GetFullTransform ();
      obj_start = movtrans.Other2This (start);
      obj_end = movtrans.Other2This (end);
    }
    r = best_r;

    // We know it is a portal container. No other object can have a
    // portal.
    int p;
    bool rc = mesh->GetMeshObject ()->HitBeamObject (
      	obj_start, obj_end, obj_isect, &r, &p);
    if (!rc) p = -1;
    if (p != -1 && r < best_r)
    {
      if (has_not_moved)
        cur_isect = obj_isect;
      else
        cur_isect = movtrans.This2Other (obj_isect);
      best_r = r;
      best_p = p;
      isect = cur_isect;
      if (p_mesh) *p_mesh = mesh;
    }
  }

  if (pr) *pr = best_r;
  return best_p;
}

iSector *csSector::FollowSegment (
  csReversibleTransform &t,
  csVector3 &new_position,
  bool &mirror,
  bool only_portals)
{
  csVector3 isect;
  iMeshWrapper* mesh;
  int p = IntersectSegment (
    t.GetOrigin (),
    new_position,
    isect,
    0,
    only_portals,
    &mesh);

  if (p != -1)
  {
    iPortalContainer* portals = mesh->GetPortalContainer ();
    if (portals)
    {
      iPortal *po = portals->GetPortal (p);
      if (po)
      {
        po->CompleteSector (0);
        if (!po->GetSector ())
        {
          new_position = isect;
          return (iSector*)this;
        }

        if (po->GetFlags ().Check (CS_PORTAL_WARP))
        {
	  csReversibleTransform warp_wor;
	  po->ObjectToWorld (mesh->GetMovable ()->GetTransform (), warp_wor);
          po->WarpSpace (warp_wor, t, mirror);
          new_position = po->Warp (warp_wor, new_position);
        }

        iSector *dest_sect = po->GetSector ();
        return dest_sect->FollowSegment (t, new_position, mirror);
      }
      else
      {
        new_position = isect;
      }
    }
    else
    {
      new_position = isect;
    }
  }
  return (iSector*)this;
}

void csSector::PrepareDraw (iRenderView *rview)
{
  if (engine->bugplug)
    engine->bugplug->AddCounter ("Sector Count", 1);

  // Make sure the visibility culler is loaded.
  GetVisibilityCuller ();
  csRenderView* csrview = (csRenderView*)rview;
  csrview->SetThisSector ((iSector*)this);

  size_t i = sectorCallbackList.Length ();
  while (i > 0)
  {
    i--;
    iSectorCallback* cb = sectorCallbackList.Get (i);
    cb->Traverse ((iSector*)this, rview);
  }

  // Mesh generators.
  const csVector3& pos = rview->GetCamera ()->GetTransform ().GetOrigin ();
  for (i = 0 ; i < meshGenerators.Length () ; i++)
  {
    meshGenerators[i]->AllocateBlocks (pos);
  }

  // CS_ENTITY_CAMERA meshes have to be moved to right position first.
  const csArray<iMeshWrapper*>& cm = cameraMeshes;
  for (i = 0 ; i < cm.Length () ; i++)
  {
    iMeshWrapper* m = cm.Get (i);
    if (m->GetFlags ().Check (CS_ENTITY_CAMERA))
    {
      iMovable* mov = m->GetMovable ();
      // Temporarily move the object to the current camera.
      csReversibleTransform &mov_trans = mov->GetTransform ();
      // @@@ TEMPORARY: now CS_ENTITY_CAMERA only works at 0,0,0 position.
      csVector3 old_pos = mov_trans.GetOrigin ();
      mov_trans.SetOrigin (csVector3 (0));
      iCamera *orig_cam = rview->GetOriginalCamera ();
      csOrthoTransform &orig_trans = orig_cam->GetTransform ();
      csVector3 v = orig_trans.GetO2TTranslation ();
      mov_trans.SetOrigin (mov_trans.GetOrigin () + v);
      csVector3 diff = old_pos - mov_trans.GetOrigin ();
      if (!(diff < .00001f))
        mov->UpdateMove ();
    }
  }
}

/*
 * @@@ THE NOTES BELOW ARE MOSTLY OBSOLETE NOW. I DON'T REMOVE THEM
 * BECAUSE THERE IS STILL A GRAIN OF USEFUL INFORMATION IN THEM.
 *
 * Some notes about drawing here. These notes are the start for
 * a rethinking about how rendering objects in one sector actually
 * should happen. Note that the current implementation actually
 * implements very little of the things discussed here. Currently
 * the entities are just rendered one after the other which can cause
 * some problems.
 *
 * There are a few issues here:
 *
 * 1. Z-buffering/Z-filling.
 * Some objects/entities are more efficiently rendered back
 * to front using Z-filling instead of Z-buffering. In some cases
 * Z-filling is also required because rendering a sector starts
 * with an uninitialized Z-buffer (CS normally doesn't clear the
 * Z buffer every frame). In some cases it might be more optimal
 * to use Z buffering in any case (to avoid sorting back to front)
 * (for hardware 3D) so we would like to have the option to clear
 * the Z buffer every frame and use Z-buffering.
 *
 * 2. Alpha transparency.
 * Some entities have alpha transparency. Alpha transparent surfaces
 * actually need to be sorted back to front to render correctly.
 * Also before rendering an alpha surface all objects behind it should
 * already be rendered.
 *
 * 3. Floating portals.
 * Floating portals also take some special consideration. First
 * of all the assume a new intialize of the Z buffer for the 2D
 * area of the portal in question. This is ok if the first entities
 * that are rendered through the portal use Z-fill and cover the
 * entire portal (this is the case if you use sector walls for
 * example). If Z-fill cannot be used for the portal then an
 * extra initial pass would have to clear the Z buffer for the portal
 * area in 2D. Also geometry needs to be clipped in 3D if you have
 * a floating portal. The reason is that the Z buffer information
 * outside of the floating portal may actually contain information
 * further than the contents of the portal. This would cause entities
 * visible inside the portal to be rendered as if they are in the
 * parent sector too.
 * After rendering through a floating portal, the floating portal
 * itself needs to be covered by the Z-buffer. i.e. we need to make
 * sure that the Z-buffer thinks the portal is a regular polygon.
 * This is to make sure that meshes or other entities rendered
 * afterwards will not get rendered INSIDE the portal contents.
 *
 * Here is a list of all the entities that we can draw in a sector:
 *
 * 1. Sector walls.
 * Sectors are always convex. So sectors walls are ideal for rendering
 * first through Z-fill.
 *
 * 2. Static things in octree.
 * In some cases all static things are collected into one big
 * octree with mini-bsp trees. This structure ensures that we can
 * actually easily sort polygon back to front or front to back if
 * needed. This structure can also easily be rendered using Z-fill.
 * The c-buffer/coverage mask tree can also be used to detect
 * visibility before rendering. This pushes visible polygons into
 * a queue. There is the issue here that it should be possible
 * to ignore the mini-bsp trees and only use the octree information.
 * This can be done on hardware where Z-buffering is fast. This
 * of course implies either the use of a Z-filled sector or else
 * a clear of the Z buffer every frame.
 * A related issue is when there are portals between the polygons.
 * Those portals need to be handled as floating portals (i.e. geometry
 * needs to be clipped in 3D) because the Z buffer information
 * will not be correct. If rendering the visible octree polygons
 * back to front then rendering through the portals presents no
 * other difficulties.
 *
 * 3. Terrain triangles.
 * The terrain engine generates a set of triangles. These triangles
 * can easily be sorted back to front so they are also suitable for
 * Z-fill rendering. However, this conflicts with the use of the
 * static octree. You cannot use Z-fill for both because that could
 * cause wrong rendering. Using Z-buffer for one of them might be
 * expensive but the only solution. Here there is also the issue
 * if it isn't possible to combine visibility algorithms for landscape
 * and octree stuff. i.e. cull octree nodes if occluded by a part
 * of the landscape.
 *
 * 4. 3D Sprites.
 * Sprites are entities that need to be rendered using the Z-buffer
 * because the triangles cannot easily be sorted.
 *
 * 5. Dynamic things.
 * Things that are not part of the static octree are handled much
 * like normal 3D sprites. The most important exception is when
 * such a thing has a floating portal. In this case all the normal
 * floating portal issues are valid. However, there are is an important
 * issue here: if you are rendering a floating portal that is BEHIND
 * an already rendered entity then there is a problem. The contents
 * of the portal may actually use Z-fill and thus would overrender
 * the entity in front. One obvious solution is to sort ALL entities
 * to make sure that everything is rendered back to front. That's of
 * course not always efficient and easy to do. Also it is not possible
 * in all cases to do it 100% correct (i.e. complex sprites with
 * skeletal animation and so on). The ideal solution would be to have
 * a way to clear the Z-buffer for an invisible polygon but only
 * where the polygon itself is visible according to the old Z-buffer
 * values. This is possible with software but I'm currently unsure
 * about hardware. With such a routine you could draw the floating
 * portal at any time you want. First you clear the Z-buffer for the
 * visible area. Then you force Z-buffer use for the contents inside
 * (i.e. everything normally rendered using Z-fill will use Z-buffer
 * instead), then you render. Finally you update the Z-buffer with
 * the Z-value of the polygon to make it 'hard'.
 *
 * If we can treat floating portals this way then we can in fact
 * consider them as normal polygons that behave correctly for the
 * Z buffer. Aside from the fact that they clip geometry in 3D
 * that passes through the portal. Note that 3D sprites don't
 * currently support 3D geometry clipping yet.
 */
void csSector::Draw (iRenderView *rview)
{
  iRenderLoop* rl = renderloop;
  if (!rl) rl = engine->GetCurrentDefaultRenderloop ();
  rl->Draw (rview, (iSector*)this);
}

void csSector::AddSectorMeshCallback (iSectorMeshCallback* cb)
{
  sectorMeshCallbackList.Push (cb);
}

void csSector::RemoveSectorMeshCallback (iSectorMeshCallback* cb)
{
  sectorMeshCallbackList.Delete (cb);
}

void csSector::FireNewMesh (iMeshWrapper* mesh)
{
  size_t i = sectorMeshCallbackList.Length ();
  while (i > 0)
  {
    i--;
    sectorMeshCallbackList[i]->NewMesh ((iSector*)this, mesh);
  }
}

void csSector::FireRemoveMesh (iMeshWrapper* mesh)
{
  size_t i = sectorMeshCallbackList.Length ();
  while (i > 0)
  {
    i--;
    sectorMeshCallbackList[i]->RemoveMesh ((iSector*)this, mesh);
  }
}

void csSector::CheckFrustum (iFrustumView *lview)
{
  int i = (int)sectorCallbackList.Length ()-1;
  while (i >= 0)
  {
    iSectorCallback* cb = sectorCallbackList.Get (i);
    cb->Traverse ((iSector*)this, lview);
    i--;
  }

  RealCheckFrustum (lview);
}

void csSector::RealCheckFrustum (iFrustumView *lview)
{
  if (drawBusy > 1) return ;
  drawBusy++;

  // Make sure we have a culler.
  GetVisibilityCuller ();
  culler->CastShadows (lview);

  drawBusy--;
}

void csSector::ShineLightsInt (csProgressPulse *pulse)
{
  int i;
  for (i = 0; i < lights.GetCount (); i++)
  {
    if (pulse != 0) pulse->Step ();

    csLight *cl = ((csLight*)lights.Get (i))->GetPrivateObject ();
    cl->CalculateLighting ();
  }
}

void csSector::ShineLightsInt (iMeshWrapper *mesh, csProgressPulse *pulse)
{
  int i;
  for (i = 0; i < lights.GetCount (); i++)
  {
    if (pulse != 0) pulse->Step ();

    csLight *cl = ((csLight*)lights.Get (i))->GetPrivateObject ();
    cl->CalculateLighting (mesh);
  }
}

void csSector::SetDynamicAmbientLight (const csColor& color)
{
  dynamicAmbientLightColor = color;
  dynamicAmbientLightVersion++;
  svDynamicAmbient->SetValue (dynamicAmbientLightColor);
}

void csSector::CalculateSectorBBox (csBox3 &bbox, bool do_meshes) const
{
  bbox.StartBoundingBox ();

  csBox3 b;
  int i;
  if (do_meshes)
  {
    for (i = 0; i < meshes.GetCount (); i++)
    {
      iMeshWrapper *mesh = meshes.Get (i);
      b = mesh->GetTransformedBoundingBox (
          mesh->GetMovable ()->GetTransform ());
      bbox += b;
    }
  }
}

void csSector::CleanupLSI ()
{
  csLightSectorInfluences::GlobalIterator it = influences.GetIterator ();
  while (it.HasNext ())
  {
    csLightSectorInfluence* inf = it.Next ();
    ((csLight*)inf->light)->RemoveLSI (inf);
  }
  influences.Empty ();
}

void csSector::AddLSI (csLightSectorInfluence* inf)
{
  influences.Add (inf);
}

void csSector::RemoveLSI (csLightSectorInfluence* inf)
{
  influences.Delete (inf);
}

//---------------------------------------------------------------------------

iMeshGenerator* csSector::CreateMeshGenerator (const char* name)
{
  csRef<csMeshGenerator> meshgen;
  meshgen.AttachNew (new csMeshGenerator (engine));
  meshgen->SetSector (this);
  meshgen->QueryObject ()->SetName (name);
  meshGenerators.Push (meshgen);
  return (iMeshGenerator*)meshgen;
}

iMeshGenerator* csSector::GetMeshGeneratorByName (const char* name)
{
  return meshGenerators.FindByName (name);
}

void csSector::RemoveMeshGenerator (size_t idx)
{
  meshGenerators.DeleteIndex (idx);
}

void csSector::RemoveMeshGenerators ()
{
  meshGenerators.DeleteAll ();
}

//---------------------------------------------------------------------------

void csSector::RegisterPortalMesh (iMeshWrapper* mesh)
{
  portalMeshes.Add (mesh);
}

void csSector::UnregisterPortalMesh (iMeshWrapper* mesh)
{
  portalMeshes.Delete (mesh);
}

CS_IMPLEMENT_STATIC_CLASSVAR_REF(csSector, svNames, SVNames,
                                 csSector::SVNamesHolder, ());

void csSector::UpdateFogSVs ()
{
  svFogColor->SetValue (fog.color);
  svFogMode->SetValue (int (fog.mode));
  svFogStart->SetValue (fog.start);
  svFogEnd->SetValue (fog.end);
  svFogDensity->SetValue (fog.density);
}

void csSector::SetupSVNames()
{
  if (SVNames().dynamicAmbient == csInvalidStringID)
  {
    SVNames().dynamicAmbient = CS::ShaderVarName (engine->globalStringSet,
      "dynamic ambient");
    SVNames().fogColor = CS::ShaderVarName (engine->globalStringSet,
      "fog color");
    SVNames().fogMode = CS::ShaderVarName (engine->globalStringSet,
      "fog mode");
    SVNames().fogStart = CS::ShaderVarName (engine->globalStringSet,
      "fog start");
    SVNames().fogEnd = CS::ShaderVarName (engine->globalStringSet,
      "fog end");
    SVNames().fogDensity = CS::ShaderVarName (engine->globalStringSet,
      "fog density");
  }
}

//---------------------------------------------------------------------------


csSectorList::csSectorList (csEngine* engine)
  : scfImplementationType (this), engine (engine)
{
  listener.AttachNew (new NameChangeListener (this));
}

csSectorList::~csSectorList ()
{
  RemoveAll ();
}

void csSectorList::NameChanged (iObject* object, const char* oldname,
  	const char* newname)
{
  csRef<iSector> sector = scfQueryInterface<iSector> (object);
  CS_ASSERT (sector != 0);
  if (oldname) sectors_hash.Delete (oldname, sector);
  if (newname) sectors_hash.Put (newname, sector);
}

void csSectorList::FreeSector (iSector* item)
{
  // We scan all objects in this sector and unlink those
  // objects.
  item->UnlinkObjects ();
}

int csSectorList::Add (iSector *obj)
{
  const char* name = obj->QueryObject ()->GetName ();
  if (name)
    sectors_hash.Put (name, obj);
  obj->QueryObject ()->AddNameChangeListener (listener);
  return (int)list.Push (obj);
}

bool csSectorList::Remove (iSector *obj)
{
  engine->FireRemoveSector (obj);
  FreeSector (obj);
  const char* name = obj->QueryObject ()->GetName ();
  if (name)
    sectors_hash.Delete (name, obj);
  obj->QueryObject ()->RemoveNameChangeListener (listener);
  return list.Delete (obj);
}

bool csSectorList::Remove (int n)
{
  iSector* obj = list[n];
  FreeSector (obj);
  const char* name = obj->QueryObject ()->GetName ();
  if (name)
    sectors_hash.Delete (name, obj);
  obj->QueryObject ()->RemoveNameChangeListener (listener);
  return list.DeleteIndex (n);
}

void csSectorList::RemoveAll ()
{
  size_t i;
  for (i = 0 ; i < list.Length () ; i++)
  {
    list[i]->QueryObject ()->RemoveNameChangeListener (listener);
    FreeSector (list[i]);
  }
  list.DeleteAll ();
  sectors_hash.DeleteAll ();
}

int csSectorList::Find (iSector *obj) const
{
  return (int)list.Find (obj);
}

iSector *csSectorList::FindByName (const char *Name) const
{
  return sectors_hash.Get (Name, 0);
}
