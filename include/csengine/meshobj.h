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

#ifndef __CS_MESHOBJ_H__
#define __CS_MESHOBJ_H__

#include "csgeom/transfrm.h"
#include "csutil/csobject.h"
#include "csutil/nobjvec.h"
#include "csutil/refarr.h"
#include "csutil/flags.h"
#include "csutil/garray.h"
#include "csengine/movable.h"
#include "imesh/object.h"
#include "imesh/lighting.h"
#include "iengine/mesh.h"
#include "iengine/viscull.h"
#include "iengine/imposter.h"
#include "iengine/shadcast.h"
#include "ivideo/graph3d.h"
#include "csengine/impmesh.h"

struct iMeshWrapper;
struct iRenderView;
struct iMovable;
struct iSharedVariable;
class csMeshWrapper;
class csMeshFactoryWrapper;
class csLight;
class csStaticLODMesh;
class csStaticLODFactoryMesh;

/**
 * General list of meshes. This class implements iMeshList.
 * Subclasses of this class can override FreeItem(), AddMesh(),
 * and RemoveMesh() for more specific functionality.
 */
class csMeshList : public iMeshList
{
private:
  csRefArrayObject<iMeshWrapper> list;

public:
  SCF_DECLARE_IBASE;

  /// constructor
  csMeshList ();
  virtual ~csMeshList ();

  /// Find a mesh in <name>:<childname>:<childname> notation.
  iMeshWrapper *FindByNameWithChild (const char *Name) const;

  /// Override PrepareItem
  virtual void PrepareItem (iMeshWrapper*) { }
  /// Override FreeItem
  virtual void FreeItem (iMeshWrapper*) { }

  virtual int GetCount () const { return list.Length () ; }
  virtual iMeshWrapper *Get (int n) const { return list.Get (n); }
  virtual int Add (iMeshWrapper *obj);
  virtual bool Remove (iMeshWrapper *obj);
  virtual bool Remove (int n);
  virtual void RemoveAll ();
  virtual int Find (iMeshWrapper *obj) const;
  virtual iMeshWrapper *FindByName (const char *Name) const;
};

/**
 * Subclass of csMeshList to hold the children of another mesh object.
 */
class csMeshMeshList : public csMeshList
{
private:
  csMeshWrapper* mesh;

public:
  csMeshMeshList () : mesh (0) { }
  virtual ~csMeshMeshList () { RemoveAll (); }
  void SetMesh (csMeshWrapper* m) { mesh = m; }
  virtual void PrepareItem (iMeshWrapper* item);
  virtual void FreeItem (iMeshWrapper* item);
};

/**
 * A list of mesh factories.
 */
class csMeshFactoryList : public iMeshFactoryList
{
private:
  csRefArrayObject<iMeshFactoryWrapper> list;

public:
  SCF_DECLARE_IBASE;

  /// constructor
  csMeshFactoryList ();
  virtual ~csMeshFactoryList () { RemoveAll (); }
  virtual void PrepareItem (iMeshFactoryWrapper*) { }
  virtual void FreeItem (iMeshFactoryWrapper*) { }

  virtual int GetCount () const { return list.Length (); }
  virtual iMeshFactoryWrapper *Get (int n) const { return list.Get (n); }
  virtual int Add (iMeshFactoryWrapper *obj);
  virtual bool Remove (iMeshFactoryWrapper *obj);
  virtual bool Remove (int n);
  virtual void RemoveAll ();
  virtual int Find (iMeshFactoryWrapper *obj) const;
  virtual iMeshFactoryWrapper *FindByName (const char *Name) const;
};

/**
 * Subclass of csMeshFactoryList to hold the children of another mesh factory.
 */
class csMeshFactoryFactoryList : public csMeshFactoryList
{
private:
  csMeshFactoryWrapper* meshfact;

public:
  csMeshFactoryFactoryList () : meshfact (0) {}
  virtual ~csMeshFactoryFactoryList () { RemoveAll (); }
  void SetMeshFactory (csMeshFactoryWrapper* m) { meshfact = m; }
  virtual void PrepareItem (iMeshFactoryWrapper* item);
  virtual void FreeItem (iMeshFactoryWrapper* item);
};

SCF_VERSION (csMeshWrapper, 0, 0, 1);

/**
 * The holder class for all implementations of iMeshObject.
 */
class csMeshWrapper : public csObject
{
  friend class csMovable;
  friend class csMovableSectorList;

protected:
  /// The parent mesh object, or 0
  iMeshWrapper *Parent;

  /**
   * Bounding box in world space.
   * This is a cache for GetWorldBoundingBox() which will recalculate this
   * if the movable changes (by using movablenr).
   */
  csBox3 wor_bbox;
  /// Last used movable number for wor_bbox.
  long wor_bbox_movablenr;

  /// Defered lighting. If > 0 then we have defered lighting.
  int defered_num_lights;

  /// Flags to use for defered lighting.
  int defered_lighting_flags;

#ifdef CS_USE_NEW_RENDERER
  /// Cached value from DrawTest
  bool draw_test;
  /// Cached light test
  bool in_light;
  /// Should we draw anything in drawshadow at all?
  bool cast_hardware_shadow;
  /// should we draw last
  bool draw_after_fancy_stuff;
#endif

  /**
   * This value indicates the last time that was used to do animation.
   * If 0 then we haven't done animation yet. We compare this value
   * with the value returned by engine->GetLastAnimationTime() to see
   * if we need to call meshobj->NextFrame() again.
   */
  csTicks last_anim_time;

  /**
   * Current visibility number used by the visibility culler.
   */
  uint32 visnr;

  /**
   * Position in the world.
   */
  csMovable movable;

  /**
   * Optional LOD control that will turn a hierarchical mesh in a
   * mesh that supports static LOD.
   */
  csRef<csStaticLODMesh> static_lod;

  /**
   * The renderer will render all objects in a sector based on this
   * number. Low numbers get rendered first. High numbers get rendered
   * later. There are a few predefined slots which the application is
   * free to use or not.
   */
  long render_priority;

  /// Get the bounding box in world space and correct in hierarchy.
  void GetFullBBox (csBox3& box);

  /// Update defered lighting.
  void UpdateDeferedLighting (const csBox3& box);

private:
  /// Mesh object corresponding with this csMeshWrapper.
  csRef<iMeshObject> meshobj;
  /// For optimization purposes we keep the iLightingInfo interface here.
  csRef<iLightingInfo> light_info;
  /// For optimization purposes we keep the iShadowReceiver interface here.
  csRef<iShadowReceiver> shadow_receiver;

  /// Children of this object (other instances of iMeshWrapper).
  csMeshMeshList children;

  /**
   * The callbacks which are called just before drawing.
   * Type: iMeshDrawCallback.
   */
  csRefArray<iMeshDrawCallback> draw_cb_vector;

  /// Optional reference to the parent csMeshFactoryWrapper.
  iMeshFactoryWrapper* factory;

  /// Z-buf mode to use for drawing this object.
  csZBufMode zbufMode;

  /// Flag indicating whether this mesh should try to imposter or not
  bool imposter_active;

  /// Imposter Threshold Range
  csRef<iSharedVariable> min_imposter_distance;

  /// Imposter Redo Threshold angle change
  csRef<iSharedVariable> imposter_rotation_tolerance;

  csImposterMesh *imposter_mesh;

public:
  /// Set of flags
  csFlags flags;
  /// Culler flags.
  csFlags culler_flags;

protected:
  /// Move this object to the specified sector. Can be called multiple times.
  void MoveToSector (iSector* s);

  /// Remove this object from all sectors it is in (but not from the engine).
  void RemoveFromSectors ();

  /**
   * Update transformations after the object has moved
   * (through updating the movable instance).
   * This MUST be done after you change the movable otherwise
   * some of the internal data structures will not be updated
   * correctly. This function is called by movable.UpdateMove();
   */
  void UpdateMove ();

  /**
   * This function determines whether to draw the imposter
   * or the true mesh and calls the appropriate function.
   */
  void DrawInt (iRenderView* rview);

  /**
   * Destructor.  This is private in order to force clients to use DecRef()
   * for object destruction.
   */
  virtual ~csMeshWrapper ();

public:
  /// Constructor.
  csMeshWrapper (iMeshWrapper* theParent, iMeshObject* meshobj);
  /// Constructor.
  csMeshWrapper (iMeshWrapper* theParent);

  /// Set parent container for this object.
  void SetParentContainer (iMeshWrapper* newParent) { Parent = newParent; }
  /// Get parent container for this object.
  iMeshWrapper* GetParentContainer () const { return Parent; }

  /// Set the mesh factory.
  void SetFactory (iMeshFactoryWrapper* factory)
  {
    csMeshWrapper::factory = factory;
  }
  /// Get the mesh factory.
  iMeshFactoryWrapper* GetFactory () const
  {
    return factory;
  }

  /// Set the mesh object.
  void SetMeshObject (iMeshObject* meshobj);
  /// Get the mesh object.
  iMeshObject* GetMeshObject () const { return meshobj; }

  /// Set the Z-buf drawing mode to use for this object.
  void SetZBufMode (csZBufMode mode) { zbufMode = mode; }
  /// Get the Z-buf drawing mode.
  csZBufMode GetZBufMode () const { return zbufMode; }

  /**
   * Set a callback which is called just before the object is drawn.
   * This is useful to do some expensive computations which only need
   * to be done on a visible object. Note that this function will be
   * called even if the object is not visible. In general it is called
   * if there is a likely probability that the object is visible (i.e.
   * it is in the same sector as the camera for example).
   */
  void SetDrawCallback (iMeshDrawCallback* cb)
  {
    draw_cb_vector.Push (cb);
  }

  void RemoveDrawCallback (iMeshDrawCallback* cb)
  {
    draw_cb_vector.Delete (cb);
  }

  virtual int GetDrawCallbackCount () const
  {
    return draw_cb_vector.Length ();
  }

  iMeshDrawCallback* GetDrawCallback (int idx) const
  {
    return draw_cb_vector.Get (idx);
  }

  /// Mark this object as visible.
  void SetVisibilityNumber (uint32 vis)
  {
    visnr = vis;
    if (Parent)
    {
      ((csMeshWrapper::MeshWrapper*)Parent)->scfParent
      	->SetVisibilityNumber (vis);
    }
  }

  /// Return if this object is visible.
  uint32 GetVisibilityNumber () const { return visnr; }

  /**
   * Light object according to the given array of lights (i.e.
   * fill the vertex color array).
   */
  void UpdateLighting (iLight** lights, int num_lights);

  /**
   * Update lighting as soon as the object becomes visible.
   * This will call engine->GetNearestLights with the supplied
   * parameters.
   */
  void DeferUpdateLighting (int flags, int num_lights);

  /**
   * Draw this mesh object given a camera transformation.
   * If needed the skeleton state will first be updated.
   * Optionally update lighting if needed (DeferUpdateLighting()).
   */
  void Draw (iRenderView* rview);

  // Static LOD methods.
  iLODControl* CreateStaticLOD ();
  void DestroyStaticLOD ();
  iLODControl* GetStaticLOD ();
  void RemoveMeshFromStaticLOD (iMeshWrapper* mesh);
  void AddMeshToStaticLOD (int lod, iMeshWrapper* mesh);

#ifdef CS_USE_NEW_RENDERER
  /**
   * Draw the zpass for the object.  If this object doesn't use lighting
   * then it can be drawn fully here.
   */
  csRenderMesh** GetRenderMeshes (int& num);
  /// This pass sets up the shadow stencil buffer
  void DrawShadow (iRenderView* rview, iLight* light);
  /// This pass draws the diffuse lit mesh
  void DrawLight (iRenderView* rview, iLight* light);
  ///Enable/disable hardware based shadows alltogheter
  void CastHardwareShadow (bool castShadow);
  /// Sets so that the meshobject is rendered after all fancy HW-shadow-stuff
  void SetDrawAfterShadow (bool drawAfter);
  /// Get if the meshobject is rendered after all fancy HW-shadow-stuff
  bool GetDrawAfterShadow ();
#endif

  /**
   * Calculate the squared distance between the camera and the object.
   */
  float GetSquaredDistance (iRenderView *rview);

  /**
   * Get the movable instance for this object.
   * It is very important to call GetMovable().UpdateMove()
   * after doing any kind of modification to this movable
   * to make sure that internal data structures are
   * correctly updated.
   */
  csMovable& GetMovable () { return movable; }

  /**
   * This routine will find out in which sectors a mesh object
   * is positioned. To use it the mesh has to be placed in one starting
   * sector. This routine will then start from that sector, find all
   * portals that touch the sprite and add all additional sectors from
   * those portals. Note that this routine using a bounding sphere for
   * this test so it is possible that the mesh will be added to sectors
   * where it really isn't located (but the sphere is).
   * <p>
   * If the mesh is already in several sectors those additional sectors
   * will be ignored and only the first one will be used for this routine.
   */
  void PlaceMesh ();

  /**
   * Check if this object is hit by this object space vector.
   * BBox version.
   */
  int HitBeamBBox (const csVector3& start, const csVector3& end,
         csVector3& isect, float* pr);
  /**
   * Check if this object is hit by this object space vector.
   * Outline version.
   */
  bool HitBeamOutline (const csVector3& start, const csVector3& end,
         csVector3& isect, float* pr);
  /**
   * Check if this object is hit by this object space vector.
   * Return the collision point in object space coordinates.
   */
  bool HitBeamObject (const csVector3& start, const csVector3& end,
  	csVector3& isect, float* pr);
  /**
   * Check if this object is hit by this world space vector.
   * Return the collision point in world space coordinates.
   */
  bool HitBeam (const csVector3& start, const csVector3& end,
  	csVector3& isect, float* pr);

  /// Get the children of this mesh object.
  const csMeshMeshList& GetChildren () const { return children; }

  /// Get the radius of this mesh and all its children.
  void GetRadius (csVector3& rad, csVector3& cent) const;

  /**
   * Do a hard transform of this object.
   * This transformation and the original coordinates are not
   * remembered but the object space coordinates are directly
   * computed (world space coordinates are set to the object space
   * coordinates by this routine). Note that some implementations
   * of mesh objects will not change the orientation of the object but
   * only the position.
   */
  void HardTransform (const csReversibleTransform& t);

  /**
   * Get the bounding box of this object in world space.
   * This routine will cache the bounding box and only recalculate it
   * if the movable changes.
   */
  void GetWorldBoundingBox (csBox3& cbox);

  /**
   * Get the bounding box of this object after applying a transformation to it.
   * This is really a very inaccurate function as it will take the bounding
   * box of the object in object space and then transform this bounding box.
   */
  void GetTransformedBoundingBox (const csReversibleTransform& trans,
  	csBox3& cbox);

  /**
   * Get a very inaccurate bounding box of the object in screen space.
   * Returns -1 if object behind the camera or else the distance between
   * the camera and the furthest point of the 3D box.
   */
  float GetScreenBoundingBox (const iCamera *camera, csBox2& sbox,
  	csBox3& cbox);

  /// Set the render priority for this object.
  void SetRenderPriority (long rp);
  /// Get the render priority for this object.
  long GetRenderPriority () const
  {
    return render_priority;
  }

  //---------- iImposter Functions -----------------//

  /// Set true if this Mesh should use Impostering
  void SetImposterActive(bool flag,iObjectRegistry *objreg);

  /**
   * Determine if this mesh is using Impostering
   * (not if Imposter is being drawn, but simply considered).
   */
  bool GetImposterActive() const
  { return imposter_active; }

  /**
   * Minimum Imposter Distance is the distance from camera 
   * beyond which imposter is used. Imposter gets a 
   * ptr here because value is a shared variable 
   * which can be changed at runtime for many objects.
   */
  void SetMinDistance(iSharedVariable* dist)
  { min_imposter_distance = dist; }

  /** 
   * Rotation Tolerance is the maximum allowable 
   * angle difference between when the imposter was 
   * created and the current position of the camera.
   * Angle greater than this triggers a re-render of
   * the imposter.
   */
  void SetRotationTolerance(iSharedVariable* angle)
  { imposter_rotation_tolerance = angle; }

  /**
   * Tells the object to create its proctex and polygon
   * for use by main render process later, relative to
   * the specified Point Of View.
   */
  void CreateImposter(csReversibleTransform& /*pov*/)
  { /* implement later */ }

  /**
   * Renders the imposter on the screen
   */
  bool DrawImposter (iRenderView *rview);

  /// Determine if imposter or true rendering will be used
  bool WouldUseImposter(csReversibleTransform& /*pov*/)
  { /* implement later */ return false; }

  /// This is true function to check distances.  Fn above may not be needed.
  bool CheckImposterRelevant (iRenderView *rview);
  
  /**
   * Draw this mesh object given a camera transformation, non-impostered.
   * If needed the skeleton state will first be updated.
   * Optionally update lighting if needed (DeferUpdateLighting()).
   */
  void DrawIntFull (iRenderView* rview);

  //--------------------- SCF stuff follows ------------------------------//
  SCF_DECLARE_IBASE_EXT (csObject);

  //--------------------- iMeshWrapper implementation --------------------//
  struct MeshWrapper : public iMeshWrapper
  {
    SCF_DECLARE_EMBEDDED_IBASE (csMeshWrapper);
    virtual iObject *QueryObject ()
    {
      return scfParent;
    }
    virtual iMeshObject* GetMeshObject () const
    {
      return scfParent->GetMeshObject ();
    }
    virtual void SetMeshObject (iMeshObject* m)
    {
      scfParent->SetMeshObject (m);
    }
    virtual iLightingInfo* GetLightingInfo () const
    {
      return scfParent->light_info;
    }
    virtual iShadowReceiver* GetShadowReceiver () const
    {
      return scfParent->shadow_receiver;
    }
    virtual uint32 GetVisibilityNumber () const
    {
      return scfParent->GetVisibilityNumber ();
    }
    virtual iMeshFactoryWrapper* GetFactory () const
    {
      return scfParent->GetFactory ();
    }
    virtual void SetFactory (iMeshFactoryWrapper* m)
    {
      scfParent->SetFactory (m);
    }
    virtual void DeferUpdateLighting (int flags, int num_lights)
    {
      scfParent->DeferUpdateLighting (flags, num_lights);
    }
    virtual void UpdateLighting (iLight** lights, int num_lights)
    {
      scfParent->UpdateLighting (lights, num_lights);
    }
    virtual iMovable* GetMovable () const
    {
      return &(scfParent->movable.scfiMovable);
    }
    virtual void PlaceMesh ()
    {
      scfParent->PlaceMesh ();
    }
    virtual int HitBeamBBox (const csVector3& start, const csVector3& end,
          csVector3& isect, float* pr)
    {
      return scfParent->HitBeamBBox (start, end, isect, pr);
    }
    virtual bool HitBeamOutline (const csVector3& start, const csVector3& end,
          csVector3& isect, float* pr)
    {
      return scfParent->HitBeamOutline (start, end, isect, pr);
    }
    virtual bool HitBeamObject (const csVector3& start, const csVector3& end,
  	csVector3& isect, float* pr)
    {
      return scfParent->HitBeamObject (start, end, isect, pr);
    }
    virtual bool HitBeam (const csVector3& start, const csVector3& end,
  	csVector3& isect, float* pr)
    {
      return scfParent->HitBeam (start, end, isect, pr);
    }
    virtual void SetDrawCallback (iMeshDrawCallback* cb)
    {
      scfParent->SetDrawCallback (cb);
    }
    virtual void RemoveDrawCallback (iMeshDrawCallback* cb)
    {
      scfParent->RemoveDrawCallback (cb);
    }
    virtual int GetDrawCallbackCount () const
    {
      return scfParent->GetDrawCallbackCount ();
    }
    virtual iMeshDrawCallback* GetDrawCallback (int idx) const
    {
      return scfParent->GetDrawCallback (idx);
    }
    virtual void SetRenderPriority (long rp)
    {
      scfParent->SetRenderPriority (rp);
    }
    virtual long GetRenderPriority () const
    {
      return scfParent->GetRenderPriority ();
    }
    virtual csFlags& GetFlags ()
    {
      return scfParent->flags;
    }
    virtual void SetZBufMode (csZBufMode mode)
    {
      scfParent->SetZBufMode (mode);
    }
    virtual csZBufMode GetZBufMode () const
    {
      return scfParent->GetZBufMode ();
    }
    virtual void HardTransform (const csReversibleTransform& t)
    {
      scfParent->HardTransform (t);
    }
    virtual void GetWorldBoundingBox (csBox3& cbox)
    {
      scfParent->GetWorldBoundingBox (cbox);
    }
    virtual void GetTransformedBoundingBox (const csReversibleTransform& trans,
  	csBox3& cbox)
    {
      scfParent->GetTransformedBoundingBox (trans, cbox);
    }
    virtual float GetScreenBoundingBox (iCamera* camera, csBox2& sbox,
  	csBox3& cbox);
    virtual iMeshList* GetChildren ()
    {
      return &scfParent->children;
    }
    virtual iMeshWrapper* GetParentContainer ()
    {
      return scfParent->GetParentContainer ();
    }
    virtual void SetParentContainer (iMeshWrapper* p)
    {
      scfParent->SetParentContainer (p);
    }
    virtual void GetRadius (csVector3& rad, csVector3 &cent) const
    {
      scfParent->GetRadius (rad,cent);
    }
    virtual void Draw (iRenderView* rview)
    {
      scfParent->Draw (rview);
    }
    virtual iLODControl* CreateStaticLOD ()
    {
      return scfParent->CreateStaticLOD ();
    }
    virtual void DestroyStaticLOD ()
    {
      scfParent->DestroyStaticLOD ();
    }
    virtual iLODControl* GetStaticLOD ()
    {
      return scfParent->GetStaticLOD ();
    }
    virtual void RemoveMeshFromStaticLOD (iMeshWrapper* mesh)
    {
      scfParent->RemoveMeshFromStaticLOD (mesh);
    }
    virtual void AddMeshToStaticLOD (int lod, iMeshWrapper* mesh)
    {
      scfParent->AddMeshToStaticLOD (lod, mesh);
    }
#ifdef CS_USE_NEW_RENDERER
    virtual csRenderMesh** GetRenderMeshes (int& num) 
    {
      return scfParent->GetRenderMeshes (num);
    }
    virtual void DrawShadow (iRenderView* rview, iLight* light)
    {
      scfParent->DrawShadow (rview, light);
    }
    virtual void DrawLight (iRenderView* rview, iLight* light)
    {
      scfParent->DrawLight (rview, light);
    }
    virtual void CastHardwareShadow (bool castShadow) 
    {
      scfParent->CastHardwareShadow (castShadow);
    }
    virtual void SetDrawAfterShadow (bool drawAfter)
    {
      scfParent->SetDrawAfterShadow (drawAfter);
    }
    virtual bool GetDrawAfterShadow ()
    {
      return scfParent->GetDrawAfterShadow ();
    }
#endif
  } scfiMeshWrapper;
  friend struct MeshWrapper;

  //-------------------- iVisibilityObject interface implementation ----------
  struct VisObject : public iVisibilityObject
  {
    SCF_DECLARE_EMBEDDED_IBASE (csMeshWrapper);
    virtual iMovable* GetMovable () const
    {
      return &(scfParent->movable.scfiMovable);
    }
    virtual iMeshWrapper* GetMeshWrapper () const
    {
      return &(scfParent->scfiMeshWrapper);
    }
    virtual void SetVisibilityNumber (uint32 vis)
    {
      scfParent->SetVisibilityNumber (vis);
    }
    virtual uint32 GetVisibilityNumber () const
    {
      return scfParent->GetVisibilityNumber ();
    }
    virtual iObjectModel* GetObjectModel ()
    {
      return scfParent->meshobj->GetObjectModel ();
    }
    virtual csFlags& GetCullerFlags ()
    {
      return scfParent->culler_flags;
    }
  } scfiVisibilityObject;
  friend struct VisObject;

  //-------------------- iImposter interface implementation ----------
  struct MeshImposter : public iImposter
  {
    SCF_DECLARE_EMBEDDED_IBASE (csMeshWrapper);
    virtual void SetImposterActive(bool flag,iObjectRegistry *objreg)
    { scfParent->SetImposterActive(flag,objreg); }
    virtual bool GetImposterActive() const
    { return scfParent->GetImposterActive(); }
    virtual void SetMinDistance(iSharedVariable* dist)
    { scfParent->SetMinDistance(dist); }
    virtual void SetRotationTolerance(iSharedVariable* angle)
    { scfParent->SetRotationTolerance(angle); }
    virtual void CreateImposter(csReversibleTransform& pov)
    { scfParent->CreateImposter(pov); }
    virtual bool WouldUseImposter(csReversibleTransform& pov) const 
    { return scfParent->WouldUseImposter(pov); }
  } scfiImposter;
};

SCF_VERSION (csMeshFactoryWrapper, 0, 0, 3);

/**
 * The holder class for all implementations of iMeshObjectFactory.
 */
class csMeshFactoryWrapper : public csObject
{
private:
  /// Mesh object factory corresponding with this csMeshFactoryWrapper.
  csRef<iMeshObjectFactory> meshFact;

  /// Optional parent of this object (can be 0).
  iMeshFactoryWrapper* parent;

  /// Optional relative transform to parent.
  csReversibleTransform transform;

  /// Children of this object (other instances of iMeshFactoryWrapper).
  csMeshFactoryFactoryList children;

  /**
   * Optional LOD control that will turn a hierarchical mesh in a
   * mesh that supports static LOD.
   */
  csRef<csStaticLODFactoryMesh> static_lod;

private:
  /// Destructor.
  virtual ~csMeshFactoryWrapper ();

public:
  /// Constructor.
  csMeshFactoryWrapper (iMeshObjectFactory* meshFact);
  /// Constructor.
  csMeshFactoryWrapper ();

  /// Set the mesh object factory.
  void SetMeshObjectFactory (iMeshObjectFactory* meshFact);

  /// Get the mesh object factory.
  iMeshObjectFactory* GetMeshObjectFactory () const
  {
    return meshFact;
  }

  /**
   * Create a new mesh object for this template.
   */
  iMeshWrapper* NewMeshObject ();

  /**
   * Do a hard transform of this factory.
   * This transformation and the original coordinates are not
   * remembered but the object space coordinates are directly
   * computed (world space coordinates are set to the object space
   * coordinates by this routine). Note that some implementations
   * of mesh objects will not change the orientation of the object but
   * only the position.
   */
  void HardTransform (const csReversibleTransform& t);

  /// This function is called for every child that is added
  void PrepareChild (iMeshFactoryWrapper* child);
  /// This function is called for every child that is removed
  void UnprepareChild (iMeshFactoryWrapper* child);

  /**
   * Get optional relative transform (relative to parent).
   */
  csReversibleTransform& GetTransform () { return transform; }

  /**
   * Set optional relative transform (relative to parent).
   */
  void SetTransform (const csReversibleTransform& tr) { transform = tr; }

  // Static LOD methods.
  iLODControl* CreateStaticLOD ();
  void DestroyStaticLOD ();
  iLODControl* GetStaticLOD ();
  void SetStaticLOD (float m, float a);
  void GetStaticLOD (float& m, float& a) const;
  void RemoveFactoryFromStaticLOD (iMeshFactoryWrapper* mesh);
  void AddFactoryToStaticLOD (int lod, iMeshFactoryWrapper* mesh);

  SCF_DECLARE_IBASE_EXT (csObject);

  //----------------- iMeshFactoryWrapper implementation --------------------//
  struct MeshFactoryWrapper : public iMeshFactoryWrapper
  {
    SCF_DECLARE_EMBEDDED_IBASE (csMeshFactoryWrapper);
    virtual iMeshObjectFactory* GetMeshObjectFactory () const
      { return scfParent->GetMeshObjectFactory (); }
    virtual void SetMeshObjectFactory (iMeshObjectFactory* fact)
      { scfParent->SetMeshObjectFactory (fact); }
    virtual iObject *QueryObject ()
      { return scfParent; }
    virtual void HardTransform (const csReversibleTransform& t)
      { scfParent->HardTransform (t); }
    virtual iMeshWrapper* CreateMeshWrapper ()
      { return scfParent->NewMeshObject (); }
    virtual iMeshFactoryWrapper* GetParentContainer () const
      { return scfParent->parent; }
    virtual void SetParentContainer (iMeshFactoryWrapper *p)
      { scfParent->parent = p; }
    virtual iMeshFactoryList* GetChildren ()
      { return &scfParent->children; }
    virtual csReversibleTransform& GetTransform ()
      { return scfParent->GetTransform (); }
    virtual void SetTransform (const csReversibleTransform& tr)
      { scfParent->SetTransform (tr); }
    virtual iLODControl* CreateStaticLOD ()
    {
      return scfParent->CreateStaticLOD ();
    }
    virtual void DestroyStaticLOD ()
    {
      scfParent->DestroyStaticLOD ();
    }
    virtual iLODControl* GetStaticLOD ()
    {
      return scfParent->GetStaticLOD ();
    }
    virtual void SetStaticLOD (float m, float a)
    {
      scfParent->SetStaticLOD (m, a);
    }
    virtual void GetStaticLOD (float& m, float& a) const
    {
      scfParent->GetStaticLOD (m, a);
    }
    virtual void RemoveFactoryFromStaticLOD (iMeshFactoryWrapper* fact)
    {
      scfParent->RemoveFactoryFromStaticLOD (fact);
    }
    virtual void AddFactoryToStaticLOD (int lod, iMeshFactoryWrapper* fact)
    {
      scfParent->AddFactoryToStaticLOD (lod, fact);
    }
  } scfiMeshFactoryWrapper;
  friend struct MeshFactoryWrapper;
};

#endif // __CS_MESHOBJ_H__
