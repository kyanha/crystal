/*
    Copyright (C) 1998-2001 by Jorrit Tyberghein

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
#include "qint.h"
#include "csutil/csstring.h"
#include "csutil/hashmap.h"
#include "csutil/csppulse.h"
#include "csutil/debug.h"
#include "iutil/vfs.h"
#include "iutil/plugin.h"
#include "iutil/objreg.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "ivideo/txtmgr.h"
#include "ivideo/texture.h"
#include "csengine/cbufcube.h"
#include "csengine/engine.h"
#include "csengine/sector.h"
#include "csengine/polygon.h"
#include "csengine/light.h"
#include "iengine/rview.h"

// Option variable: render portals?
bool csSector:: do_portals = true;

// Configuration variable: number of allowed reflections for static lighting.
int csSector:: cfg_reflections = 1;

// Option variable: do pseudo radiosity?
bool csSector:: do_radiosity = false;

//---------------------------------------------------------------------------
csSectorLightList::csSectorLightList ()
{
  sector = NULL;
}

csSectorLightList::~csSectorLightList ()
{
  DeleteAll ();
}

bool csSectorLightList::PrepareItem (csSome Item)
{
  iLight *light = (iLight *)Item;
  light->IncRef ();
  light->SetSector (&(sector->scfiSector));
  return true;
}

bool csSectorLightList::FreeItem (csSome Item)
{
  iLight *light = (iLight *)Item;
  light->SetSector (NULL);
  light->DecRef ();
  return true;
}

//---------------------------------------------------------------------------
csSectorMeshList::csSectorMeshList ()
{
  sector = NULL;
}

csSectorMeshList::~csSectorMeshList ()
{
  DeleteAll ();
}

bool csSectorMeshList::PrepareItem (csSome item)
{
  CS_ASSERT (sector != NULL);

  csMeshList::PrepareItem (item);
  sector->PrepareMesh ((iMeshWrapper *)item);
  return true;
}

bool csSectorMeshList::FreeItem (csSome item)
{
  CS_ASSERT (sector != NULL);

  sector->UnprepareMesh ((iMeshWrapper *)item);
  csMeshList::FreeItem (item);
  return true;
}

//---------------------------------------------------------------------------
SCF_IMPLEMENT_IBASE_EXT(csSector)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iReferencedObject)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iSector)
  SCF_IMPLEMENTS_INTERFACE (csSector);
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSector::eiSector)
  SCF_IMPLEMENTS_INTERFACE(iSector)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSector::ReferencedObject)
  SCF_IMPLEMENTS_INTERFACE(iReferencedObject)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csSector::csSector (csEngine *engine) :
  csObject()
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSector);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiReferencedObject);
  DG_TYPE (this, "csSector");
  csSector::engine = engine;
  culler_mesh = NULL;
  culler = NULL;
  engine->AddToCurrentRegion (this);
  fog.enabled = false;
  draw_busy = 0;
  meshes.SetSector (this);
  lights.SetSector (this);
}

csSector::~csSector ()
{
  // The references to this sector MUST be cleaned up before this
  // sector is destructed.
  CS_ASSERT (references.Length () == 0);

  lights.DeleteAll ();
  if (culler) culler->DecRef ();

  // rendering queues are deleted automatically.
}

void csSector::CleanupReferences ()
{
  while (references.Length () > 0)
  {
    iReference *ref = (iReference *)references[references.Length () - 1];
#ifdef CS_DEBUG
    // Sanity check.
    iReferencedObject *refobj = ref->GetReferencedObject ();
    CS_ASSERT (refobj == &scfiReferencedObject);
#endif
    ref->SetReferencedObject (NULL);
  }
}

//----------------------------------------------------------------------
void csSector::PrepareMesh (iMeshWrapper *mesh)
{
  RenderQueues.Add (mesh);

  if (culler)
  {
    iVisibilityObject *vo = SCF_QUERY_INTERFACE (
        mesh,
        iVisibilityObject);
    culler->RegisterVisObject (vo);
    vo->DecRef ();
  }
}

void csSector::UnprepareMesh (iMeshWrapper *mesh)
{
  RenderQueues.Remove (mesh);

  if (culler)
  {
    iVisibilityObject *vo = SCF_QUERY_INTERFACE (
        mesh,
        iVisibilityObject);
    culler->UnregisterVisObject (vo);
    vo->DecRef ();
  }
}

void csSector::RelinkMesh (iMeshWrapper *mesh)
{
  // @@@ this function would be a lot faster of the previous
  // priority was known!
  RenderQueues.RemoveUnknownPriority (mesh);
  RenderQueues.Add (mesh);
}

//----------------------------------------------------------------------
void csSector::UseCuller (const char *meshname)
{
  // If there is already a culler in use, do nothing.
  if (culler) return;

  // Find the culler with the given name.
  culler_mesh = meshes.FindByName (meshname);
  if (!culler_mesh) return;

  // Query the culler interface from it.
  culler = SCF_QUERY_INTERFACE (
      culler_mesh->GetMeshObject (),
      iVisibilityCuller);
  if (!culler) { culler_mesh = NULL; return; }

  // load cache data
  char cachename[256];
  sprintf (cachename, "%s_%s", GetName (), meshname);
  culler->Setup (cachename);

  // Loop through all meshes and update their bounding box in the
  // polygon trees.
  int i;
  for (i = 0; i < meshes.Length (); i++)
  {
    iMeshWrapper *th = meshes.Get (i);
    th->GetMovable ()->UpdateMove ();

    iVisibilityObject *vo = SCF_QUERY_INTERFACE (th, iVisibilityObject);
    culler->RegisterVisObject (vo);
    vo->DecRef ();
  }
}

void csSector::UseCullerPlugin (const char *plugname)
{
  // If there is already a culler in use, do nothing.
  if (culler) return;

  culler_mesh = NULL;

  // Load the culler plugin.
  iPluginManager* plugmgr = CS_QUERY_REGISTRY (
  	csEngine::object_reg, iPluginManager);
  culler = CS_LOAD_PLUGIN (plugmgr, plugname, iVisibilityCuller);
  if (!culler) return;

  // load cache data
  char cachename[256];
  sprintf (cachename, "%s_%s", GetName (), plugname);
  culler->Setup (cachename);

  // Loop through all meshes and register them to the visibility culler.
  int i;
  for (i = 0; i < meshes.Length (); i++)
  {
    iMeshWrapper *th = meshes.Get (i);
    th->GetMovable ()->UpdateMove ();

    iVisibilityObject *vo = SCF_QUERY_INTERFACE (th, iVisibilityObject);
    culler->RegisterVisObject (vo);
    vo->DecRef ();
  }
}

csPolygon3D *csSector::HitBeam (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect)
{
  csPolygon3D *p = IntersectSegment (start, end, isect);
  if (p)
  {
    csPortal *po = p->GetPortal ();
    if (po)
    {
      draw_busy++;

      csVector3 new_start = isect;
      p = po->HitBeam (new_start, end, isect);
      draw_busy--;
      return p;
    }
    else
      return p;
  }
  else
    return NULL;
}

csMeshWrapper *csSector::HitBeam (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect,
  csPolygon3D **polygonPtr)
{
  float r, best_mesh_r = 10000000000.;
  iMeshWrapper *near_mesh = NULL;
  csVector3 tsect;

  // First check all meshes in this sector.
  int i;
  for (i = 0; i < meshes.Length (); i++)
  {
    iMeshWrapper *mesh = meshes.Get (i);
    if (
      !mesh->GetFlags ().Check (CS_ENTITY_INVISIBLE) &&
      mesh->HitBeam (start, end, isect, &r))
    {
      if (r < best_mesh_r)
      {
        best_mesh_r = r;
        near_mesh = mesh;
        tsect = isect;
      }
    }
  }

  float best_poly_r;
  csMeshWrapper *poly_mesh;
  csPolygon3D *p = IntersectSegment (
      start,
      end,
      isect,
      &best_poly_r,
      false,
      &poly_mesh);

  // We hit a polygon and the polygon is closer than the mesh.
  if (p && best_poly_r < best_mesh_r)
  {
    csPortal *po = p->GetPortal ();
    if (po)
    {
      draw_busy++;

      csVector3 new_start = isect;
      csMeshWrapper *obj = po->HitBeam (new_start, end, isect, polygonPtr);
      draw_busy--;
      return obj;
    }
    else
    {
      if (polygonPtr) *polygonPtr = p;
      return poly_mesh;
    }
  }

  // The mesh is closer (or there is no mesh).
  if (polygonPtr) *polygonPtr = NULL;
  isect = tsect;
  return near_mesh ? near_mesh->GetPrivateObject () : NULL;
}

csPolygon3D *csSector::IntersectSegment (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect,
  float *pr,
  bool only_portals,
  csMeshWrapper **p_mesh)
{
  float r, best_r = 10000000000.;
  csVector3 cur_isect;
  csPolygon3D *best_p = NULL;
  csVector3 obj_start, obj_end, obj_isect;

  if (culler && !only_portals)
  {
    // culler_mesh has option CS_THING_MOVE_NEVER so
    // object space == world space.
    iMeshWrapper *mesh;
    iPolygon3D *ip = culler->IntersectSegment (start, end, isect, &r, &mesh);
    if (ip)
    {
      best_p = ip->GetPrivateObject ();
      best_r = r;
      if (p_mesh)
        *p_mesh = mesh ? mesh->GetPrivateObject ()
		: culler_mesh ? culler_mesh->GetPrivateObject () : NULL;
    }
    if (pr) *pr = best_r;
    return best_p;
  }

  csReversibleTransform movtrans;

  int i;
  for (i = 0; i < meshes.Length (); i++)
  {
    iMeshWrapper *mesh = meshes.Get (i);
    if (mesh->GetFlags ().Check (CS_ENTITY_INVISIBLE)) continue;
    //@@@ Should use 'culler' and not 'culler_mesh'!
    if (culler_mesh && !only_portals && mesh == culler_mesh)
      continue;  // Already handled above.

    // @@@ UGLY!!!
    iThingState *ith = SCF_QUERY_INTERFACE (
        mesh->GetMeshObject (),
        iThingState);
    if (ith)
    {
      csThing *sp = (csThing *) (ith->GetPrivateObject ());
      r = best_r;

      //@@@ Put this in csMeshWrapper???
      if (sp->GetMovingOption () == CS_THING_MOVE_NEVER)
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

      csPolygon3D *p = sp->IntersectSegment (
          obj_start,
          obj_end,
          obj_isect,
          &r,
          only_portals);

      if (p && r < best_r)
      {
        if (sp->GetMovingOption () == CS_THING_MOVE_NEVER)
          cur_isect = obj_isect;
        else
          cur_isect = movtrans.This2Other (obj_isect);
        best_r = r;
        best_p = p;
        isect = cur_isect;
        if (p_mesh) *p_mesh = mesh->GetPrivateObject ();
      }

      ith->DecRef ();
    }
  }

  if (pr) *pr = best_r;
  return best_p;
}

csSector *csSector::FollowSegment (
  csReversibleTransform &t,
  csVector3 &new_position,
  bool &mirror,
  bool only_portals)
{
  csVector3 isect;
  csPolygon3D *p = IntersectSegment (
      t.GetOrigin (),
      new_position,
      isect,
      NULL,
      only_portals);
  csPortal *po;

  if (p)
  {
    po = p->GetPortal ();
    if (po)
    {
      po->CompleteSector (NULL);
      if (!po->GetSector ())
      {
        new_position = isect;
        return this;
      }

      if (po->flags.Check (CS_PORTAL_WARP))
      {
        po->WarpSpace (t, mirror);
        new_position = po->Warp (new_position);
      }

      csSector *dest_sect = po->GetSector ()->GetPrivateObject ();
      return dest_sect->FollowSegment (t, new_position, mirror);
    }
    else
      new_position = isect;
  }

  return this;
}

csPolygon3D *csSector::IntersectSphere (
  csVector3 &center,
  float radius,
  float *pr)
{
  float min_d = radius;
  csPolygon3D *min_p = NULL;
  (void)center;
#if 0
  float d = .0f;
  csPolygon3D *p = NULL, *min_p = NULL;
  csVector3 obj_center;
  csReversibleTransform movtrans;

  //@@@ Support for meshes!!!
  int i;
  for (i = 0; i < things.Length (); i++)
  {
    csThing *sp = (csThing *)things[i];
    if (sp->GetMovingOption () == CS_THING_MOVE_NEVER)
      obj_center = center;
    else
    {
      movtrans = sp->GetMovable ().GetFullTransform ();
      obj_center = movtrans.Other2This (center);
    }

    p = sp->IntersectSphere (obj_center, radius, &d);
    if (p && d < min_d)
    {
      min_d = d;
      min_p = p;
    }
  }
#endif
  if (pr) *pr = min_d;
  return min_p;
}

#if 0
int compare_z_thing (const void *p1, const void *p2)
{
  csMeshWrapper *sp1 = *(csMeshWrapper **)p1;
  csMeshWrapper *sp2 = *(csMeshWrapper **)p2;
  float z1 = sp1->Vcam (sp1->GetCenter ()).z;
  float z2 = sp2->Vcam (sp2->GetCenter ()).z;
  if (z1 < z2)
    return -1;
  else if (z1 > z2)
    return 1;
  return 0;
}
#endif

/*
 * @@@ THE NOTES BELOW ARE MOSTLY OBSOLETE NOW. I DON'T REMOVE THEM
 *
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
# ifdef CS_DEBUG
  extern int viscnt_vis_poly; viscnt_vis_poly = 0;
  extern int viscnt_invis_poly; viscnt_invis_poly = 0;
  extern int viscnt_vis_node; viscnt_vis_node = 0;
  extern int viscnt_invis_node; viscnt_invis_node = 0;
  extern int viscnt_vis_obj; viscnt_vis_obj = 0;
  extern int viscnt_invis_obj; viscnt_invis_obj = 0;
  extern float viscnt_invis_node_vol; viscnt_invis_node_vol = 0;
  extern float viscnt_vis_node_vol; viscnt_vis_node_vol = 0;
# endif

  draw_busy++;

  int i, j;
  iCamera *icam = rview->GetCamera ();
  rview->SetThisSector (&scfiSector);

  G3D_FOGMETHOD fogmethod = G3DFOGMETHOD_NONE;

  if (rview->GetCallback ())
  {
    rview->CallCallback (CS_CALLBACK_SECTOR, (void *) &scfiSector);
  }
  else
  {
    if (HasFog ())
    {
      if (
        (fogmethod = csEngine::current_engine->fogmethod) == G3DFOGMETHOD_VERTEX)
      {
        csFogInfo *fog_info = new csFogInfo ();
        fog_info->next = rview->GetFirstFogInfo ();

        iPolygon3D *ipoly3d = rview->GetPortalPolygon ();
        if (ipoly3d)
        {
          fog_info->incoming_plane = ipoly3d->GetCameraPlane ();
          fog_info->incoming_plane.Invert ();
          fog_info->has_incoming_plane = true;
        }
        else
          fog_info->has_incoming_plane = false;
        fog_info->fog = &GetFog ();
        fog_info->has_outgoing_plane = true;
        rview->SetFirstFogInfo (fog_info);
      }
      else if (fogmethod != G3DFOGMETHOD_NONE)
      {
        rview->GetGraphics3D ()->OpenFogObject (GetID (), &GetFog ());
      }
    }
  }

  if (rview->AddedFogInfo ())
    rview->GetFirstFogInfo ()->has_outgoing_plane = false;

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
  if (meshes.Length () > 0)
  {
    // if we use a culler, visible objects are marked now
    bool use_culler = (culler && culler->VisTest (rview));

    // get a pointer to the previous sector
    iSector *prev_sector = rview->GetPreviousSector ();

    // look if meshes from the previous sector should be drawn
    bool draw_prev_sector = false;

    if (prev_sector)
    {
      draw_prev_sector = prev_sector->HasFog () ||
        rview->GetPortalPolygon ()->IsTransparent () ||
        rview->GetPortalPolygon ()->GetPortal ()->GetFlags ().Check (
            CS_PORTAL_WARP);
    }

    // Draw the meshes.
    for (i = 0; i < RenderQueues.GetQueueCount (); i++)
    {
      RenderQueues.Sort (rview, i);

      csMeshVectorNodelete *v = RenderQueues.GetQueue (i);
      if (!v) continue;

      for (j = 0; j < v->Length (); j++)
      {
        iMeshWrapper *sp = v->Get (j);
        if (!use_culler || sp->GetPrivateObject ()->IsVisible ())
        {
          if (
            !prev_sector ||
            sp->GetMovable ()->GetSectors ()->Find (prev_sector) == -1)
          {
            // Mesh is not in the previous sector or there is no previous

            // sector.
            sp->Draw (rview);
          }
          else if (draw_prev_sector)
          {
            // @@@ Here we should draw clipped to the portal.
            sp->Draw (rview);
          }
        }
      }
    }
  }

  // queue all halos in this sector to be drawn.
  for (i = lights.Length () - 1; i >= 0; i--)
    // Tell the engine to try to add this light into the halo queue
    csEngine::current_engine->AddHalo (lights.Get (i)->GetPrivateObject ());

  if (rview->GetCallback ())
  {
    rview->CallCallback (CS_CALLBACK_SECTOREXIT, (void *) &scfiSector);
  }
  else
  {
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
              GetID (),
              g3dpoly,
              CS_FOG_VIEW);
        }
        else
        {
          // We must add a FRONT fog polygon for the clipper to this sector.
          rview->GetClipPlane (g3dpoly.normal);
          g3dpoly.normal.Invert ();
          rview->GetGraphics3D ()->DrawFogPolygon (
              GetID (),
              g3dpoly,
              CS_FOG_FRONT);
        }
      }
      else if (fogmethod == G3DFOGMETHOD_VERTEX && rview->AddedFogInfo ())
      {
        csFogInfo *fog_info = rview->GetFirstFogInfo ();
        rview->SetFirstFogInfo (rview->GetFirstFogInfo ()->next);
        delete fog_info;
      }
    }
  }

  draw_busy--;

# ifdef CS_DEBUG
  extern bool viscnt_enabled;
  if (viscnt_enabled)
  {
    float percentage =
      100.0f - 100.0f * viscnt_invis_node_vol / viscnt_vis_node_vol;
    printf ("p+%d-%d   o+%d-%d   n+%d-%d(%g%%)\n",
	viscnt_vis_poly, viscnt_invis_poly,
	viscnt_vis_obj, viscnt_invis_obj,
	viscnt_vis_node, viscnt_invis_node, percentage);
    fflush (stdout);
  }
# endif
}

csObject **csSector::GetVisibleObjects (iFrustumView *lview, int &num_objects)
{
  csFrustum *lf = lview->GetFrustumContext ()->GetLightFrustum ();
  bool infinite = lf->IsInfinite ();
  csVector3 &c = lf->GetOrigin ();
  bool vis;
  int i, i1;
  int j;

  num_objects = meshes.Length ();
  if (!num_objects)
  {
    return NULL;
  }

  csObject **visible_objects = new csObject *[num_objects];

  num_objects = 0;

  // @@@ Unify both loops below once csThing becomes a mesh object.
  for (j = 0; j < meshes.Length (); j++)
  {
    iMeshWrapper *sp = meshes.Get (j);

    // If the light frustum is infinite then every thing

    // in this sector is of course visible.
    if (infinite)
      vis = true;
    else
    {
      csBox3 bbox;
      sp->GetWorldBoundingBox (bbox);

      /*
       * Here we do a quick test to see if the bounding box is visible in
       * in the frustum. This test is not complete in the sense that it will
       * say that some bounding boxes are visible even if they are not. But
       * it is correct in the sense that if it says a bounding box
       * is invisible, then it certainly is invisible.
       *
       * It works by taking all vertices of the bounding box. If
       * ALL of them are on the outside of the same plane from the
       * frustum then the object is certainly not visible.
       */
      vis = true;
      i1 = lf->GetVertexCount () - 1;
      for (i = 0; i < lf->GetVertexCount (); i1 = i, i++)
      {
        csVector3 &v1 = lf->GetVertex (i);
        csVector3 &v2 = lf->GetVertex (i1);
        if (csMath3::WhichSide3D (bbox.GetCorner (0) - c, v1, v2) < 0)
          continue;
        if (csMath3::WhichSide3D (bbox.GetCorner (1) - c, v1, v2) < 0)
          continue;
        if (csMath3::WhichSide3D (bbox.GetCorner (2) - c, v1, v2) < 0)
          continue;
        if (csMath3::WhichSide3D (bbox.GetCorner (3) - c, v1, v2) < 0)
          continue;
        if (csMath3::WhichSide3D (bbox.GetCorner (4) - c, v1, v2) < 0)
          continue;
        if (csMath3::WhichSide3D (bbox.GetCorner (5) - c, v1, v2) < 0)
          continue;
        if (csMath3::WhichSide3D (bbox.GetCorner (6) - c, v1, v2) < 0)
          continue;
        if (csMath3::WhichSide3D (bbox.GetCorner (7) - c, v1, v2) < 0)
          continue;

        // Here we have a case of all vertices of the bbox being on the
        // outside of the same plane.
        vis = false;
        break;
      }

      if (vis && lf->GetBackPlane ())
      {
        /*
	 * If still visible then we can also check the back plane.
         * @@@ NOTE THIS IS UNTESTED CODE. LIGHT_FRUSTUMS CURRENTLY DON'T
         * HAVE A BACK PLANE YET.
	 */
        if (
          !csMath3::Visible (
              bbox.GetCorner (0) - c,
              *lf->GetBackPlane ()) &&
          !csMath3::Visible (
              bbox.GetCorner (1) - c,
              *lf->GetBackPlane ()) &&
          !csMath3::Visible (
              bbox.GetCorner (2) - c,
              *lf->GetBackPlane ()) &&
          !csMath3::Visible (
              bbox.GetCorner (3) - c,
              *lf->GetBackPlane ()) &&
          !csMath3::Visible (
              bbox.GetCorner (4) - c,
              *lf->GetBackPlane ()) &&
          !csMath3::Visible (
              bbox.GetCorner (5) - c,
              *lf->GetBackPlane ()) &&
          !csMath3::Visible (
              bbox.GetCorner (6) - c,
              *lf->GetBackPlane ()) &&
          !csMath3::Visible (bbox.GetCorner (7) - c, *lf->GetBackPlane ()))
          vis = false;
      }
    }

    if (vis) visible_objects[num_objects++] = sp->GetPrivateObject ();
  }

  return visible_objects;
}

void csSector::CheckFrustum (iFrustumView *lview)
{
  csCBufferCube *cb = engine->GetCBufCube ();
  cb->MakeEmpty ();
  RealCheckFrustum (lview);
}

void csSector::RealCheckFrustum (iFrustumView *lview)
{
  if (draw_busy > cfg_reflections) return ;
  draw_busy++;

  int i;

  // Translate this sector so that it is oriented around
  // the position of the light (position of the light becomes
  // the new origin).
  csVector3 &center = lview->GetFrustumContext ()->GetLightFrustum ()
    ->GetOrigin ();

  iShadowBlockList *shadows = lview->GetFrustumContext ()->GetShadows ();

  // Mark a new region so that we can restore the shadows later.
  uint32 prev_region = shadows->MarkNewRegion ();

  if (culler && culler->SupportsShadowCasting ())
  {
    culler->CastShadows (lview);
  }
  else
  {
    /*
     * Here we have no octree so we know the sector polygons are
     * convex. First find all objects that are visible in the frustum.
     */
    int num_visible_objects;
    csObject **visible_objects = GetVisibleObjects (
        lview,
        num_visible_objects);

    /*
     * Append the shadows for these objects to the shadow list.
     * This list is appended to the one given in 'lview'. After
     * returning, the list in 'lview' will be restored.
     */
    if (lview->ThingShadowsEnabled ())
    {
      for (i = 0; i < num_visible_objects; i++)
      {
        csObject *o = visible_objects[i];
        csMeshWrapper *mesh = (csMeshWrapper *)o;
        // Only if the thing has right flags do we consider it for shadows.
        if (lview->CheckShadowMask (mesh->flags.Get ()))
	{
          iShadowCaster *shadcast = SCF_QUERY_INTERFACE (
            mesh->GetMeshObject (), iShadowCaster);
          if (shadcast)
          {
            shadcast->AppendShadows (&(mesh->GetMovable ().scfiMovable), shadows, center);
            shadcast->DecRef ();
	  }
        }
      }
    }

    // Calculate lighting for all objects in the current sector.
    for (i = 0; i < num_visible_objects; i++)
    {
      csObject *o = visible_objects[i];
      csMeshWrapper *mesh = (csMeshWrapper *)o;
      // Only if the thing has right flags do we consider it for shadows.
      if (lview->CheckProcessMask (mesh->flags.Get ()))
      {
        iShadowReceiver *shadrcv = SCF_QUERY_INTERFACE (
          mesh->GetMeshObject (),
          iShadowReceiver);
        if (shadrcv)
        {
          shadrcv->CastShadows (&(mesh->GetMovable ().scfiMovable), lview);
          shadrcv->DecRef ();
	}
      }
    }

    delete[] visible_objects;
  }

  // Restore the shadow list in 'lview' and then delete
  // all the shadow frustums that were added in this recursion
  // level.
  while (shadows->GetLastShadowBlock ())
  {
    iShadowBlock *sh = shadows->GetLastShadowBlock ();
    if (!shadows->FromCurrentRegion (sh))
      break;
    shadows->RemoveLastShadowBlock ();
    sh->DecRef ();
  }
  shadows->RestoreRegion (prev_region);

  draw_busy--;
}

void csSector::ShineLights (csProgressPulse *pulse)
{
  int i;
  for (i = 0; i < lights.Length (); i++)
  {
    if (pulse != 0) pulse->Step ();

    csLight *cl = lights.Get (i)->GetPrivateObject ();
    ((csStatLight *)cl)->CalculateLighting ();
  }
}

void csSector::ShineLights (iMeshWrapper *mesh, csProgressPulse *pulse)
{
  int i;
  for (i = 0; i < lights.Length (); i++)
  {
    if (pulse != 0) pulse->Step ();

    csLight *cl = lights.Get (i)->GetPrivateObject ();
    ((csStatLight *)cl)->CalculateLighting (mesh);
  }
}

void csSector::CalculateSectorBBox (csBox3 &bbox, bool do_meshes) const
{
  bbox.StartBoundingBox ();

  csBox3 b;
  int i;
  if (do_meshes)
  {
    for (i = 0; i < meshes.Length (); i++)
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
iPolygon3D *csSector::eiSector::HitBeam (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect)
{
  csPolygon3D *p = scfParent->HitBeam (start, end, isect);
  if (p)
    return &(p->scfiPolygon3D);
  else
    return NULL;
}

iMeshWrapper *csSector::eiSector::HitBeam (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect,
  iPolygon3D **polygonPtr)
{
  csPolygon3D *p = NULL;
  csMeshWrapper *obj = scfParent->HitBeam (
      start,
      end,
      isect,
      polygonPtr ? &p : NULL);
  if (obj)
  {
    if (p)
    {
      *polygonPtr = &(p->scfiPolygon3D);
    }
  }

  return obj ? &(obj->scfiMeshWrapper) : NULL;
}

iSector *csSector::eiSector::FollowSegment (
  csReversibleTransform &t,
  csVector3 &new_position,
  bool &mirror,
  bool only_portals)
{
  csSector *s = scfParent->FollowSegment (
      t,
      new_position,
      mirror,
      only_portals);
  if (s)
    return &(s->scfiSector);
  else
    return NULL;
}

void csSector::ReferencedObject::AddReference (iReference *ref)
{
  scfParent->references.Push (ref);
}

void csSector::ReferencedObject::RemoveReference (iReference *ref)
{
  int i;

  // We scan backwards because we know that the code to remove all
  // refs to a sector will also scan backwards. So this is more efficient.
  for (i = scfParent->references.Length () - 1; i >= 0; i--)
  {
    iReference *r = (iReference *) (scfParent->references[i]);
    if (r == ref)
    {
      scfParent->references.Delete (i);
      return ;
    }
  }

  // Hopefully we never come here.
  CS_ASSERT (false);
}

SCF_IMPLEMENT_IBASE(csSectorList)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iSectorList)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSectorList::SectorList)
  SCF_IMPLEMENTS_INTERFACE(iSectorList)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csSectorList::csSectorList (bool cr)
{
  SCF_CONSTRUCT_IBASE (NULL);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSectorList);

  CleanupReferences = cr;
}

csSectorList::~csSectorList ()
{
  DeleteAll ();
}

bool csSectorList::FreeItem (csSome Item)
{
  if (CleanupReferences)
    ((iSector *)Item)->GetPrivateObject ()->CleanupReferences ();
  return csSectorListHelper::FreeItem (Item);
}

int csSectorList::SectorList::GetCount () const
{
  return scfParent->Length ();
}

iSector *csSectorList::SectorList::Get (int n) const
{
  return scfParent->Get (n);
}

int csSectorList::SectorList::Add (iSector *obj)
{
  return scfParent->Push (obj);
}

bool csSectorList::SectorList::Remove (iSector *obj)
{
  return scfParent->Delete (obj);
}

bool csSectorList::SectorList::Remove (int n)
{
  return scfParent->Delete (n);
}

void csSectorList::SectorList::RemoveAll ()
{
  scfParent->DeleteAll ();
}

int csSectorList::SectorList::Find (iSector *obj) const
{
  return scfParent->Find (obj);
}

iSector *csSectorList::SectorList::FindByName (const char *Name) const
{
  return scfParent->FindByName (Name);
}
