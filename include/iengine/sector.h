/*
    Crystal Space 3D engine
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

#ifndef __CS_IENGINE_SECTOR_H__
#define __CS_IENGINE_SECTOR_H__

/**\file
 */
/**
 * \addtogroup engine3d
 * @{ */

#include "cstypes.h"
#include "csutil/scf.h"
#include "iutil/object.h"

class csVector3;
class csSector;
class csColor;
class csBox3;
class csReversibleTransform;
struct iMeshWrapper;
struct iMeshList;
struct iLightList;
struct iLight;
struct iThing;
struct iStatLight;
struct iVisibilityCuller;
struct iVisibilityObject;
struct iObject;
struct csFog;
struct iGraphics3D;
struct iPolygon3D;
struct iRenderView;
struct iFrustumView;
struct iSector;
class csRenderMesh;

SCF_VERSION (iSectorCallback, 0, 0, 1);

/**
 * Set a callback which is called when this sector is traversed.
 * The given context will be either an instance of iRenderView, iFrustumView,
 * or else 0.
 */
struct iSectorCallback : public iBase
{
  /**
   * Sector will be traversed. It is safe to delete this callback
   * in this function.
   */
  virtual void Traverse (iSector* sector, iBase* context) = 0;
};

#ifdef CS_USE_NEW_RENDERER

SCF_VERSION (iSectorRenderMeshList, 0, 0, 1);

/// A list of rendermeshes in a sector.
struct iSectorRenderMeshList : public iBase
{
  /// Get the number of RMs in this list.
  virtual int GetCount () = 0;
  /**
   * Get an RM in the list.
   * \param index Which rendermesh to retrieve.
   * \param mw The mesh wrapper that belongs to this object.
   * \param visobj The mesh wrapper's visibility object. Returned
   *  for convenience.
   * \param rm The actual meshobject.
   * \remark Sorting according to the renderpriorities is not done,
   *  it's needed to call PrioritySort().
   */
  virtual void Get (int index, 
    iMeshWrapper*& mw, 
    iVisibilityObject*& visobj,
    csRenderMesh*& rm) = 0;
  /**
   * Get a visible RM.
   * Retrieves the first visible rendermesh, beginning at index.
   * 'Visible' refers to those visible with the view passed to
   * iSector::PrepareDraw(). \p index is incremented, so the
   * next call to GetVisible() will retrieve the next visible
   * RM.
   * \param index Which rendermesh to retrieve.
   * \param mw The mesh wrapper that belongs to this object.
   * \param visobj The mesh wrapper's visibility object. Returned
   *  for convenience.
   * \param rm The actual meshobject.
   * \returns Whether any visible mesh was found.
   * \remark Sorting according to the renderpriorities is done
   *  'on the way', it's not needed to call PrioritySort().
   */
  virtual bool GetVisible (int& index, 
    iMeshWrapper*& mw, 
    iVisibilityObject*& visobj,
    csRenderMesh*& rm) = 0;
  /**
   * Sort meshes according to the order specified in the renderpriorities.
   */
  virtual void PrioritySort () = 0;
};

#endif

SCF_VERSION (iSector, 0, 5, 2);

/**
 * The iSector interface is used to work with "sectors". A "sector"
 * is an empty region of space that can contain other objects (mesh
 * objects). A sector itself does not represent geometry but only
 * contains other geometry. A sector does contain lights though.
 * The sector is the basic building block for any Crystal Space level.
 * A level can be made from one or more sectors. Using the thing mesh
 * object one can use portals to connect multiple sectors.
 */
struct iSector : public iBase
{
  /// @@@ Used by the engine to retrieve internal sector object (ugly)
  virtual csSector *GetPrivateObject () = 0;
  /// Get the iObject for this sector.
  virtual iObject *QueryObject() = 0;

#ifndef CS_USE_NEW_RENDERER
  /// Has this sector fog?
  virtual bool HasFog () const = 0;
  /// Return the fog structure (even if fog is disabled)
  virtual csFog *GetFog () const = 0;
  /// Fill the fog structure with the given values
  virtual void SetFog (float density, const csColor& color) = 0;
  /// Disable fog in this sector
  virtual void DisableFog () = 0;
#endif // CS_USE_NEW_RENDERER

  /// Get the list of meshes in this sector.
  virtual iMeshList* GetMeshes () = 0;
  /**
   * Get the list of static and pseudo-dynamic lights in this sector.
   * Do NOT add dynamic lights to this list. For a dynamic light
   * you only have to call SetSector() on those dynamic lights.
   */
  virtual iLightList* GetLights () = 0;

  /// Calculate lighting for all objects in this sector
  virtual void ShineLights () = 0;
  /// Version of ShineLights() which only affects one mesh object.
  virtual void ShineLights (iMeshWrapper*) = 0;

  /// Sets dynamic ambient light for all things in the sector
  virtual void SetDynamicAmbientLight(const csColor& color) = 0;

  /// Get the last set dynamic ambient light for this sector.
  virtual csColor GetDynamicAmbientLight() const = 0;

  /**
   * Calculate the bounding box of all objects in this sector.
   * This function is not very efficient as it will traverse all objects
   * in the sector one by one and compute a bounding box from that.
   */
  virtual void CalculateSectorBBox (csBox3& bbox,
    bool do_meshes) const = 0;

  /**
   * Use the specified plugin as the visibility culler for
   * this sector. Returns false if the culler could not be
   * loaded for some reason.
   */
  virtual bool SetVisibilityCullerPlugin (const char *Name) = 0;
  /**
   * Get the visibility culler that is used for this sector.
   * If there is no culler yet a culler of type 'crystalspace.culling.frustvis'
   * will be created and used for this sector.
   */
  virtual iVisibilityCuller* GetVisibilityCuller () = 0;

  /**
   * Get the current draw recursion level.
   */
  virtual int GetRecLevel () const = 0;

  /**
   * Follow a beam from start to end and return the first polygon that
   * is hit. This function correctly traverse portals and space warping
   * portals. Normally the sector you call this on should be the sector
   * containing the 'start' point. 'isect' will be the intersection point
   * if a polygon is returned.
   */
  virtual iPolygon3D* HitBeam (const csVector3& start, const csVector3& end,
    csVector3& isect) = 0;

  /**
   * Follow a beam from start to end and return the first object
   * that is hit. In case it is a thing the iPolygon3D field will be
   * filled with the polygon that was hit.
   * If polygonPtr is null then the polygon will not be filled in.
   */
  virtual iMeshWrapper* HitBeam (const csVector3& start, const csVector3& end,
    csVector3& intersect, iPolygon3D** polygonPtr, bool accurate = false) = 0;

  /**
   * Follow a segment starting at this sector. If the segment intersects
   * with a polygon it will stop there unless the polygon is a portal in which
   * case it will recursively go to that sector (possibly applying warping
   * transformations) and continue there.<p>
   *
   * This routine will modify all the given parameters to reflect space warping.
   * These should be used as the new camera transformation when you decide to
   * really go to the new position.<p>
   *
   * This function returns the resulting sector and new_position will be set
   * to the last position that you can go to before hitting a wall.<p>
   *
   * If only_portals is true then only portals will be checked. This
   * means that intersection with normal polygons is not checked. This
   * is a lot faster but it does mean that you need to use another
   * collision detection system to test with walls.
   */
  virtual iSector* FollowSegment (csReversibleTransform& t,
    csVector3& new_position, bool& mirror, bool only_portals = false) = 0;

  /// Draw the sector with the given render view
  virtual void Draw (iRenderView* rview) = 0;
#ifdef CS_USE_NEW_RENDERER
  /**
   * The following three Draw* calls divide the draw into three passes.
   * They correspond to similar Draw* calls in the MeshObject.  The 
   * first fills the zBuffer and draws any meshes into the framebuffer
   * which do not use the lighting system (i.e., lightmapped meshes)
   * It will also store all objects drawn for the current scene, so that
   * on the following passes, occlusions will not need to be redetermined.
   * The object_list should be deleted by the calling function with
   * delete [].
   */
  virtual void DrawZ (iRenderView* rview) = 0;
  /**
   * The second pass draws a shadow volume version of the object which
   * uses the filled zBuffer to determine how to fill the stencil 
   * buffer to mask out portions of light which are in shadow.
   */
  virtual void DrawShadow (iRenderView* rview, iLight *light) = 0;
  /**
   * The third pass draws the diffuse lit mesh using the stencil buffer
   * to enable the shadows.
   */
  virtual void DrawLight (iRenderView* rview, iLight *light) = 0;

  /// 
  virtual void PrepareDraw (iRenderView* rview) = 0;

  virtual iSectorRenderMeshList* GetRenderMeshes () = 0;
#endif

  /**
   * Set the sector callback. This will call IncRef() on the callback
   * So make sure you call DecRef() to release your own reference.
   */
  virtual void SetSectorCallback (iSectorCallback* cb) = 0;

  /**
   * Remove a sector callback.
   */
  virtual void RemoveSectorCallback (iSectorCallback* cb) = 0;

  /// Get the number of sector callbacks.
  virtual int GetSectorCallbackCount () const = 0;

  /// Get the specified sector callback.
  virtual iSectorCallback* GetSectorCallback (int idx) const = 0;

  /// Used for portal traversal.
  virtual void CheckFrustum (iFrustumView* lview) = 0;
};


SCF_VERSION (iSectorList, 0, 0, 2);

/**
 * A list of sectors.
 */
struct iSectorList : public iBase
{
  /// Return the number of sectors in this list.
  virtual int GetCount () const = 0;

  /// Return a sector by index.
  virtual iSector *Get (int n) const = 0;

  /// Add a sector.
  virtual int Add (iSector *obj) = 0;

  /// Remove a sector.
  virtual bool Remove (iSector *obj) = 0;

  /// Remove the nth sector.
  virtual bool Remove (int n) = 0;

  /// Remove all sectors.
  virtual void RemoveAll () = 0;

  /// Find a sector and return its index.
  virtual int Find (iSector *obj) const = 0;

  /// Find a sector by name.
  virtual iSector *FindByName (const char *Name) const = 0;
};

SCF_VERSION (iSectorIterator, 0, 0, 1);

/**
 * An iterator to iterate over sectors. Some functions in CS
 * return this.
 */
struct iSectorIterator : public iBase
{
  /// Restart iterator.
  virtual void Restart () = 0;

  /// Get sector from iterator. Return 0 at end.
  virtual iSector* Fetch () = 0;

  /**
   * Get last position that was used from Fetch. This can be
   * different from 'pos' because of space warping.
   */
  virtual const csVector3& GetLastPosition () = 0;
};

/** @} */

#endif // __CS_IENGINE_SECTOR_H__

