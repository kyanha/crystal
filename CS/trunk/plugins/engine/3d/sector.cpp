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
#include "csutil/csstring.h"
#include "csutil/hashmap.h"
#include "csutil/csppulse.h"
#include "csutil/debug.h"
#include "iutil/vfs.h"
#include "iutil/plugin.h"
#include "iutil/objreg.h"
#include "iutil/virtclk.h"
#include "csgeom/kdtree.h"
#include "igeom/clip2d.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/txtmgr.h"
#include "ivideo/texture.h"
#include "plugins/engine/3d/engine.h"
#include "plugins/engine/3d/sector.h"
#include "plugins/engine/3d/light.h"
#include "plugins/engine/3d/material.h"
#include "plugins/engine/3d/rview.h"
#include "iengine/rview.h"
#include "iengine/portal.h"
#include "imesh/lighting.h"

#include "ivaria/bugplug.h"

#ifdef CS_USE_NEW_RENDERER
#include "ivideo/rendermesh.h"
#endif

// Configuration variable: number of allowed reflections for static lighting.
int csSector:: cfg_reflections = 1;

// Option variable: do pseudo radiosity?
bool csSector:: do_radiosity = false;

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
  csLightList::PrepareLight (item);
  item->GetPrivateObject ()->SetSector (&(sector->scfiSector));

  const csVector3& center = item->GetCenter ();
  float radius = item->GetInfluenceRadius ();
  csBox3 lightbox (center - csVector3 (radius), center + csVector3 (radius));
  csKDTreeChild* childnode = kdtree->AddObject (lightbox, (void*)item);
  item->GetPrivateObject ()->SetChildNode (childnode);
}

void csSectorLightList::FreeLight (iLight* item)
{
  item->GetPrivateObject ()->SetSector (0);
  kdtree->RemoveObject (item->GetPrivateObject ()->GetChildNode ());
  csLightList::FreeLight (item);
}

//--------------------------------------------------------------------------

csSectorMeshList::csSectorMeshList ()
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
SCF_IMPLEMENT_IBASE_EXT(csSector)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iSector)
  SCF_IMPLEMENTS_INTERFACE (csSector);
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSector::eiSector)
  SCF_IMPLEMENTS_INTERFACE(iSector)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csSector::csSector (csEngine *engine) : csObject()
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSector);
  DG_TYPE (this, "csSector");
  csSector::engine = engine;
  fog.enabled = false;
  draw_busy = 0;
  dynamic_ambient_color.Set(0,0,0);
  meshes.SetSector (this);
  //portal_containers.SetSector (this);
  lights.SetSector (this);
  current_visnr = 0;
  renderloop = 0;
}

csSector::~csSector ()
{
  lights.RemoveAll ();
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiSector);
}

void csSector::UnlinkObjects ()
{
  int i;
  for (i = 0; i < meshes.GetCount (); i++)
  {
    iMeshWrapper* m = meshes.Get (i);
    iSectorList* sl = m->GetMovable ()->GetSectors ();
    sl->Remove (&scfiSector);
    m->GetMovable ()->UpdateMove ();
  }
}

//----------------------------------------------------------------------

void csSector::RegisterEntireMeshToCuller (iMeshWrapper* mesh)
{
  csMeshWrapper* cmesh = ((csMeshWrapper::MeshWrapper*)mesh)
    	->GetCsMeshWrapper ();
  if (cmesh->SomeParentHasStaticLOD ()) return;

  csRef<iVisibilityObject> vo = SCF_QUERY_INTERFACE (mesh,
        iVisibilityObject);
  culler->RegisterVisObject (vo);

  if (cmesh->GetStaticLODMesh ()) return;
  int i;
  const csMeshMeshList& ml = cmesh->GetChildren ();
  for (i = 0 ; i < ml.GetCount () ; i++)
  {
    iMeshWrapper* child = ml.Get (i);
    RegisterEntireMeshToCuller (child);
  }
}

void csSector::RegisterMeshToCuller (iMeshWrapper* mesh)
{
  csMeshWrapper* cmesh = ((csMeshWrapper::MeshWrapper*)mesh)
    	->GetCsMeshWrapper ();
  if (cmesh->SomeParentHasStaticLOD ()) return;

  csRef<iVisibilityObject> vo = SCF_QUERY_INTERFACE (mesh,
        iVisibilityObject);
  culler->RegisterVisObject (vo);
}

void csSector::UnregisterMeshToCuller (iMeshWrapper* mesh)
{
  csRef<iVisibilityObject> vo = SCF_QUERY_INTERFACE (mesh,
        iVisibilityObject);
  culler->UnregisterVisObject (vo);
}

void csSector::PrepareMesh (iMeshWrapper *mesh)
{
  bool do_camera = mesh->GetFlags ().Check (CS_ENTITY_CAMERA);
  if (do_camera) camera_meshes.Push (mesh);

  if (culler) RegisterMeshToCuller (mesh);
  int i;
  iMeshList* ml = mesh->GetChildren ();
  for (i = 0 ; i < ml->GetCount () ; i++)
  {
    iMeshWrapper* child = ml->Get (i);
    PrepareMesh (child);
  }
}

void csSector::UnprepareMesh (iMeshWrapper *mesh)
{
  camera_meshes.Delete (mesh);

  if (culler) UnregisterMeshToCuller (mesh);
  int i;
  iMeshList* ml = mesh->GetChildren ();
  for (i = 0 ; i < ml->GetCount () ; i++)
  {
    iMeshWrapper* child = ml->Get (i);
    UnprepareMesh (child);
  }
}

void csSector::RelinkMesh (iMeshWrapper *mesh)
{
  camera_meshes.Delete (mesh);
  bool do_camera = mesh->GetFlags ().Check (CS_ENTITY_CAMERA);
  if (do_camera) camera_meshes.Push (mesh);

  int i;
  iMeshList* ml = mesh->GetChildren ();
  for (i = 0 ; i < ml->GetCount () ; i++)
  {
    iMeshWrapper* child = ml->Get (i);
    RelinkMesh (child);
  }
}

//----------------------------------------------------------------------

bool csSector::UseCullerPlugin (const char *plugname)
{
  culler = 0;

  // Load the culler plugin.
  csRef<iPluginManager> plugmgr = CS_QUERY_REGISTRY (csEngine::object_reg,
  	iPluginManager);
  culler = CS_LOAD_PLUGIN (plugmgr, plugname, iVisibilityCuller);

  if (!culler)
  {
    return false;
  }

  // load cache data
  char cachename[256];
  sprintf (cachename, "%s_%s", GetName (), plugname);
  culler->Setup (cachename);

  // Loop through all meshes and register them to the visibility culler.
  int i;
  for (i = 0; i < meshes.GetCount (); i++)
  {
    iMeshWrapper* m = meshes.Get (i);
    m->GetMovable ()->UpdateMove ();
    RegisterEntireMeshToCuller (m);
  }
  return true;
}

iVisibilityCuller* csSector::GetVisibilityCuller ()
{
  if (!culler) UseCullerPlugin ("crystalspace.culling.frustvis");
  CS_ASSERT (culler != 0);
  return culler;
}

class csSectorVisibleMeshCallback : public iVisibilityCullerListener
{
public:
  SCF_DECLARE_IBASE;

  csSectorVisibleMeshCallback ()
  {
    SCF_CONSTRUCT_IBASE(0);
    privMeshlist = 0;
    sector = 0;
  }

  virtual ~csSectorVisibleMeshCallback()
  {
    SCF_DESTRUCT_IBASE();
  }

  // NR version
  void Setup (csRenderMeshList *meshlist, iRenderView *rview)
  {
    privMeshlist = meshlist;
    csSectorVisibleMeshCallback::rview = rview;
  }
  // OR version
  void Setup (csSector* sector, iRenderView *rview)
  {
    csSectorVisibleMeshCallback::sector = sector;
    csSectorVisibleMeshCallback::rview = rview;
  }

  void MarkMeshAndChildrenVisible (iMeshWrapper* mesh, uint32 frustum_mask)
  {
    csMeshWrapper* cmesh = ((csMeshWrapper::MeshWrapper*)mesh)
    	  ->GetCsMeshWrapper ();
    ObjectVisible (cmesh, frustum_mask);
    int i;
    const csMeshMeshList& children = cmesh->GetChildren ();
    for (i = 0 ; i < children.GetCount () ; i++)
    {
      iMeshWrapper* child = children.Get (i);
      MarkMeshAndChildrenVisible (child, frustum_mask);
    }
  }

  void ObjectVisible (csMeshWrapper* cmesh, uint32 frustum_mask)
  {
    csStaticLODMesh* static_lod = cmesh->GetStaticLODMesh ();
    if (static_lod)
    {
      float distance = csQsqrt (cmesh->GetSquaredDistance (rview));
      float lod = static_lod->GetLODValue (distance);
      csArray<iMeshWrapper*>& meshes = static_lod->GetMeshesForLOD (lod);
      size_t i;
      for (i = 0 ; i < meshes.Length () ; i++)
        MarkMeshAndChildrenVisible (meshes[i], frustum_mask);
    }

#ifdef CS_USE_NEW_RENDERER
    int num;
    csRenderMesh** meshes = cmesh->GetRenderMeshes (num, rview, 
      &(cmesh->GetCsMovable ()).scfiMovable, frustum_mask);
    CS_ASSERT(!((num != 0) && (meshes == 0)));
#ifdef CS_DEBUG
    int i;
    for (i = 0 ; i < num ; i++)
      meshes[i]->db_mesh_name = cmesh->GetName ();
#endif
    if (num > 0)
    {
      csBox3 box;
      //@@@ Don't call this here! cmesh->GetWorldBoundingBox (box);
      privMeshlist->AddRenderMeshes (meshes, num, cmesh->GetRenderPriority (),
	  cmesh->GetZBufMode (), box);
    }
#else
    sector->GetRenderQueues ().AddVisible (cmesh, frustum_mask);
#endif
  }

  virtual void ObjectVisible (iVisibilityObject* visobj, iMeshWrapper *mesh,
  	uint32 frustum_mask)
  {
#ifdef CS_USE_NEW_RENDERER
    if (privMeshlist == 0)
      return;
#endif

    csMeshWrapper* cmesh = ((csMeshWrapper::MeshWrapper*)mesh)
    	->GetCsMeshWrapper ();
    ObjectVisible (cmesh, frustum_mask);
  }

private:
  csSector* sector;	// OR only
  csRenderMeshList *privMeshlist;	// NR only
  iRenderView *rview;
};

SCF_IMPLEMENT_IBASE(csSectorVisibleMeshCallback)
  SCF_IMPLEMENTS_INTERFACE(iVisibilityCullerListener)
SCF_IMPLEMENT_IBASE_END

CS_IMPLEMENT_STATIC_VAR (GetVisMeshCb, csSectorVisibleMeshCallback, ())

csRenderMeshList *csSector::GetVisibleMeshes (iRenderView *rview)
{
  if (rview == 0) return 0;

  // This is used for csMeshObject::IsChildVisible to determine
  // when to update its cache. Should be changed to something more
  // sensible. @@@ What about engine frame number?
  current_visnr++;

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
      GetVisMeshCb ()->Setup (entry.meshList, rview);
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
  GetVisMeshCb ()->Setup (holder.meshList, rview);
  GetVisibilityCuller()->VisTest (rview, GetVisMeshCb ());
  visibleMeshCache.Push (holder);
  return holder.meshList;
}


iMeshWrapper* csSector::HitBeamPortals (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect,
  int* polygon_idx)
{
  iMeshWrapper* mesh = 0;
  int p = IntersectSegment (start, end, isect, 0, false,
		  &mesh);
  if (p != -1)
  {
    iPortalContainer* portals = mesh->GetPortalContainer ();
    if (portals)
    {
      // There are portals.
      iPortal* po = portals->GetPortal (p);
      if (po)
      {
	draw_busy++;
	csVector3 new_start = isect;
	mesh = po->HitBeamPortals (mesh->GetMovable ()->GetFullTransform (),
		      new_start, end, isect, &p);
	draw_busy--;
      }
    }
  }
  if (polygon_idx) *polygon_idx = p;
  return mesh;
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
          return &scfiSector;
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
  return &scfiSector;
}

void csSector::PrepareDraw (iRenderView *rview)
{
#ifndef CS_USE_NEW_RENDERER
  draw_busy++;
#endif

  if (csEngine::current_engine->bugplug)
    csEngine::current_engine->bugplug->AddCounter ("Sector Count", 1);

  // Make sure the visibility culler is loaded.
  GetVisibilityCuller ();
  csRenderView* csrview = (csRenderView*)rview;
  csrview->SetThisSector (&scfiSector);

  size_t i = sector_cb_vector.Length ();
  while (i-- > 0)
  {
    iSectorCallback* cb = sector_cb_vector.Get (i);
    cb->Traverse (&scfiSector, rview);
  }

  // CS_ENTITY_CAMERA meshes have to be moved to right position first.
  const csArray<iMeshWrapper*>& cm = camera_meshes;
  for (i = 0 ; i < cm.Length () ; i++)
  {
    iMeshWrapper* m = cm.Get (i);
    if (m->GetFlags ().Check (CS_ENTITY_CAMERA))
    {
      iMovable* mov = m->GetMovable ();
      // Temporarily move the object to the current camera.
      csReversibleTransform &mov_trans = mov->GetTransform ();
      // @@@ TEMPORARY: now CS_ENTITY_CAMERA only works at 0,0,0 position.
      mov_trans.SetOrigin (csVector3 (0));
      iCamera *orig_cam = rview->GetOriginalCamera ();
      csOrthoTransform &orig_trans = orig_cam->GetTransform ();
      csVector3 v = orig_trans.GetO2TTranslation ();
      mov_trans.SetOrigin (mov_trans.GetOrigin () + v);
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
#ifndef CS_USE_NEW_RENDERER
  csRenderView* csrview = (csRenderView*)rview;
  csrview->SetupClipPlanes ();

  PrepareDraw (rview);
  iCamera *icam = rview->GetCamera ();
  size_t i;

  G3D_FOGMETHOD fogmethod = G3DFOGMETHOD_NONE;

  if (HasFog ())
  {
    fogmethod = csEngine::current_engine->fogmethod;
    if (fogmethod == G3DFOGMETHOD_VERTEX)
    {
      csFogInfo *fog_info = new csFogInfo ();
      fog_info->next = csrview->GetFirstFogInfo ();

      iPortal* last_portal = rview->GetLastPortal ();
      if (last_portal)
      {
        last_portal->ComputeCameraPlane (icam->GetTransform (),
		  fog_info->incoming_plane);
        fog_info->incoming_plane.Invert ();
        fog_info->has_incoming_plane = true;
      }
      else
      {
        fog_info->has_incoming_plane = false;
      }
      fog_info->fog = &GetFog ();
      fog_info->has_outgoing_plane = true;
      csrview->SetFirstFogInfo (fog_info);
    }
    else if (fogmethod != G3DFOGMETHOD_NONE)
    {
      rview->GetGraphics3D ()->OpenFogObject (GetID (), &GetFog ());
    }
  }

  if (csrview->AddedFogInfo ())
    csrview->GetFirstFogInfo ()->has_outgoing_plane = false;

  /*
   * Draw meshes.
   * To correctly support meshes in multiple sectors we only draw a
   * mesh if the mesh is not in the sector we came from. If the
   * mesh is also present in the previous sector then we will still
   * draw it in any of the following cases:
   *    - the previous sector has fog
   *    - the portal we just came through has alpha transparency
   *    - the portal is a portal on a thing (i.e. a floating portal)
   *    - the portal does space warping
   * In those cases we draw the mesh anyway. @@@ Note that we should
   * draw it clipped (in 3D) to the portal polygon. This is currently not
   * done.
   */

  if (meshes.GetCount () > 0)
  {
    // Mark visible objects.
    current_visnr++;
    RenderQueues.ClearVisible ();
    GetVisMeshCb ()->Setup (this, rview);
    culler->VisTest (rview, GetVisMeshCb ());
    //uint32 current_visnr = culler->GetCurrentVisibilityNumber ();

    // get a pointer to the previous sector
    iSector *prev_sector = rview->GetPreviousSector ();

    // look if meshes from the previous sector should be drawn
    bool draw_prev_sector = false;

    if (prev_sector)
    {
      iPortal* portal = rview->GetLastPortal ();
      draw_prev_sector = prev_sector->HasFog () ||
        portal->GetFlags ().Check (CS_PORTAL_WARP | CS_PORTAL_CLIPSTRADDLING);
    }

    // First sort everything based on render priority and return
    // a big list of visible objects. This will use the visibility
    // information calculated by VisTest() above.
    csArrayMeshMask objects;
    objects.SetCapacity (meshes.GetCount ());
    RenderQueues.SortAll (objects, rview);

    // Draw the objects.
    for (i = 0 ; i < objects.Length () ; i++)
    {
      csMeshWithMask& mesh_with_mask = objects[i];
      csMeshWrapper* sp = mesh_with_mask.mesh;
      if (!prev_sector ||
          sp->GetMovable ()->GetSectors ()->Find (prev_sector) == -1)
      {
        // Mesh is not in the previous sector or there is no previous
        // sector.
        sp->Draw (rview, mesh_with_mask.frustum_mask);
      }
      else if (draw_prev_sector)
      {
        // @@@ Here we should draw clipped to the portal.
        sp->Draw (rview, mesh_with_mask.frustum_mask);
      }
    }
  }

  // queue all halos in this sector to be drawn.
  i = lights.GetCount ();
  while (i > 0)
  {
    i--;
    // Tell the engine to try to add this light into the halo queue
    csEngine::current_engine->AddHalo (
      csEngine::current_engine->current_camera,  
      lights.Get (i)->GetPrivateObject ());
  }

  // Handle the fog, if any
  if (fogmethod != G3DFOGMETHOD_NONE)
  {
    G3DPolygonDFP g3dpoly;
    if (fogmethod == G3DFOGMETHOD_ZBUFFER)
    {
      g3dpoly.num = rview->GetClipper ()->GetVertexCount ();

      csVector2 *clipview = rview->GetClipper ()->GetClipPoly ();
      memcpy (g3dpoly.vertices, clipview, g3dpoly.num * sizeof (csVector2));
      if (icam->GetSector () == &scfiSector && draw_busy == 0)
      {
        // Since there is fog in the current camera sector we simulate
        // this by adding the view plane polygon.
        rview->GetGraphics3D ()->DrawFogPolygon (
              GetID (), g3dpoly, CS_FOG_VIEW);
      }
      else
      {
        // We must add a FRONT fog polygon for the clipper to this sector.
        csrview->GetClipPlane (g3dpoly.normal);
        g3dpoly.normal.Invert ();
        rview->GetGraphics3D ()->DrawFogPolygon (
            GetID (), g3dpoly, CS_FOG_FRONT);
      }
    }
    else if (fogmethod == G3DFOGMETHOD_VERTEX && csrview->AddedFogInfo ())
    {
      csFogInfo *fog_info = csrview->GetFirstFogInfo ();
      csrview->SetFirstFogInfo (csrview->GetFirstFogInfo ()->next);
      delete fog_info;
    }
  }

  draw_busy--;
#else
  iRenderLoop* rl = renderloop;
  if (!rl) rl = engine->GetCurrentDefaultRenderloop ();
  rl->Draw (rview, (iSector*)&scfiSector);
#endif // CS_USE_NEW_RENDERER
}

void csSector::CheckFrustum (iFrustumView *lview)
{
  int i = sector_cb_vector.Length ()-1;
  while (i >= 0)
  {
    iSectorCallback* cb = sector_cb_vector.Get (i);
    cb->Traverse (&scfiSector, lview);
    i--;
  }

  RealCheckFrustum (lview);
}

void csSector::RealCheckFrustum (iFrustumView *lview)
{
  if (draw_busy > cfg_reflections) return ;
  draw_busy++;

  // Make sure we have a culler.
  GetVisibilityCuller ();
  culler->CastShadows (lview);

  draw_busy--;
}

void csSector::ShineLights (csProgressPulse *pulse)
{
  int i;
  for (i = 0; i < lights.GetCount (); i++)
  {
    if (pulse != 0) pulse->Step ();

    csLight *cl = lights.Get (i)->GetPrivateObject ();
    cl->CalculateLighting ();
  }
}

void csSector::ShineLights (iMeshWrapper *mesh, csProgressPulse *pulse)
{
  int i;
  for (i = 0; i < lights.GetCount (); i++)
  {
    if (pulse != 0) pulse->Step ();

    csLight *cl = lights.Get (i)->GetPrivateObject ();
    cl->CalculateLighting (mesh);
  }
}

void csSector::SetDynamicAmbientLight (const csColor& color)
{
  iMeshList* ml = GetMeshes ();
  dynamic_ambient_color = color;
  for (int i = 0 ; i < ml->GetCount () ; i++)
  {
    iMeshWrapper* mesh = ml->Get (i);
    iLightingInfo* li = mesh->GetLightingInfo ();
    if (li)
      li->SetDynamicAmbientLight (color);
  }
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
      mesh->GetTransformedBoundingBox (
          mesh->GetMovable ()->GetTransform (),
          b);
      bbox += b;
    }
  }
}

//---------------------------------------------------------------------------
iMeshWrapper *csSector::eiSector::HitBeamPortals (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect,
  int* polygon_idx)
{
  return scfParent->HitBeamPortals (start, end, isect, polygon_idx);
}

iMeshWrapper *csSector::eiSector::HitBeam (
  const csVector3 &start, const csVector3 &end,
  csVector3 &isect,
  int *polygonPtr,
  bool accurate)
{
  return scfParent->HitBeam (
      start, end, isect,
      polygonPtr, accurate);
}

iSector *csSector::eiSector::FollowSegment (
  csReversibleTransform &t,
  csVector3 &new_position,
  bool &mirror,
  bool only_portals)
{
  iSector *s = scfParent->FollowSegment (
      t,
      new_position,
      mirror,
      only_portals);
  return s;
}

//---------------------------------------------------------------------------

void csSector::RegisterPortalMesh (iMeshWrapper* mesh)
{
  portal_meshes.Add (mesh);
}

void csSector::UnregisterPortalMesh (iMeshWrapper* mesh)
{
  portal_meshes.Delete (mesh);
}

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csSectorList)
  SCF_IMPLEMENTS_INTERFACE(iSectorList)
SCF_IMPLEMENT_IBASE_END

csSectorList::csSectorList ()
{
  SCF_CONSTRUCT_IBASE (0);
}

csSectorList::~csSectorList ()
{
  RemoveAll ();
  SCF_DESTRUCT_IBASE();
}

void csSectorList::FreeSector (iSector* item)
{
  // We scan all objects in this sector and unlink those
  // objects.
  item->UnlinkObjects ();
}

int csSectorList::Add (iSector *obj)
{
  return list.Push (obj);
}

bool csSectorList::Remove (iSector *obj)
{
  FreeSector (obj);
  return list.Delete (obj);
}

bool csSectorList::Remove (int n)
{
  FreeSector (list[n]);
  return list.DeleteIndex (n);
}

void csSectorList::RemoveAll ()
{
  size_t i;
  for (i = 0 ; i < list.Length () ; i++)
  {
    FreeSector (list[i]);
  }
  list.DeleteAll ();
}

int csSectorList::Find (iSector *obj) const
{
  return list.Find (obj);
}

iSector *csSectorList::FindByName (const char *Name) const
{
  return list.FindByName (Name);
}
