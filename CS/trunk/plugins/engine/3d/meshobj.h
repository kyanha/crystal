/*
    Copyright (C) 2000-2006 by Jorrit Tyberghein

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
#include "csutil/scf_implementation.h"
#include "csutil/csobject.h"
#include "csutil/nobjvec.h"
#include "csutil/refarr.h"
#include "csutil/flags.h"
#include "csutil/dirtyaccessarray.h"
#include "csutil/weakref.h"
#include "csutil/leakguard.h"
#include "csutil/hash.h"
#include "iutil/selfdestruct.h"
#include "csgfx/shadervarcontext.h"
#include "imesh/object.h"
#include "imesh/lighting.h"
#include "iengine/mesh.h"
#include "iengine/imposter.h"
#include "iengine/viscull.h"
#include "iengine/shadcast.h"
#include "ivideo/graph3d.h"
#include "ivideo/shader/shader.h"

#include "movable.h"
#include "impmesh.h"
#include "meshlod.h"
#include "scenenode.h"
#include "light.h"

struct iMeshWrapper;
struct iMovable;
struct iRenderView;
struct iSharedVariable;
class csEngine;
class csLight;
class csMeshFactoryWrapper;
class csMeshWrapper;

/**
 * General list of meshes.
 */
class csMeshList : public scfImplementation1<csMeshList, iMeshList>
{
private:
  csRefArrayObject<iMeshWrapper> list;
  csHash<iMeshWrapper*, csString> meshes_hash;

  class NameChangeListener : public scfImplementation1<NameChangeListener,
  	iObjectNameChangeListener>
  {
  private:
    csWeakRef<csMeshList> list;

  public:
    NameChangeListener (csMeshList* list) : scfImplementationType (this),
  	  list (list)
    {
    }
    virtual ~NameChangeListener () { }

    virtual void NameChanged (iObject* obj, const char* oldname,
  	  const char* newname)
    {
      if (list)
        list->NameChanged (obj, oldname, newname);
    }
  };
  csRef<NameChangeListener> listener;

public:

  void NameChanged (iObject* object, const char* oldname,
  	const char* newname);

  /// constructor
  csMeshList (int cap, int thresshold);
  virtual ~csMeshList ();

  /// Find a mesh in <name>:<childname>:<childname> notation.
  iMeshWrapper *FindByNameWithChild (const char *Name) const;

  /// Override PrepareMesh
  virtual void PrepareMesh (iMeshWrapper*) { }
  /// Override FreeMesh
  virtual void FreeMesh (iMeshWrapper*) { }

  virtual int GetCount () const { return (int)list.Length () ; }
  virtual iMeshWrapper *Get (int n) const { return list.Get (n); }
  virtual int Add (iMeshWrapper *obj);
  virtual bool Remove (iMeshWrapper *obj);
  virtual bool Remove (int n);
  virtual void RemoveAll ();
  virtual int Find (iMeshWrapper *obj) const;
  virtual iMeshWrapper *FindByName (const char *Name) const;
};

/**
 * A list of mesh factories.
 */
class csMeshFactoryList : public scfImplementation1<csMeshFactoryList,
	iMeshFactoryList>
{
private:
  csRefArrayObject<iMeshFactoryWrapper> list;
  csHash<iMeshFactoryWrapper*, csString>
  	factories_hash;

  class NameChangeListener : public scfImplementation1<NameChangeListener,
  	iObjectNameChangeListener>
  {
  private:
    csWeakRef<csMeshFactoryList> list;

  public:
    NameChangeListener (csMeshFactoryList* list) : scfImplementationType (this),
  	  list (list)
    {
    }
    virtual ~NameChangeListener () { }

    virtual void NameChanged (iObject* obj, const char* oldname,
  	  const char* newname)
    {
      if (list)
        list->NameChanged (obj, oldname, newname);
    }
  };
  csRef<NameChangeListener> listener;


public:

  void NameChanged (iObject* object, const char* oldname,
  	const char* newname);

  /// constructor
  csMeshFactoryList ();
  virtual ~csMeshFactoryList ();
  virtual void PrepareFactory (iMeshFactoryWrapper*) { }
  virtual void FreeFactory (iMeshFactoryWrapper*) { }

  virtual int GetCount () const { return (int)list.Length (); }
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
  virtual void PrepareFactory (iMeshFactoryWrapper* item);
  virtual void FreeFactory (iMeshFactoryWrapper* item);
};

struct LSIAndDist
{
  csLightSectorInfluence* lsi;
  // An indication of how powerful this light affects the object.
  // Higher values mean more influence.
  float influence;
};

#include "csutil/win32/msvc_deprecated_warn_off.h"

/**
 * The holder class for all implementations of iMeshObject.
 */
class csMeshWrapper : public scfImplementationExt6<csMeshWrapper,
                                                   csObject,
                                                   iMeshWrapper,
                                                   iShaderVariableContext,
                                                   iVisibilityObject,
                                                   iImposter,
						   iSceneNode,
						   iSelfDestruct>
{
  friend class csMovable;
  friend class csMovableSectorList;

protected:
  /**
   * Bounding box in world space.
   * This is a cache for GetWorldBoundingBox() which will recalculate this
   * if the movable changes (by using movablenr).
   */
  csBox3 wor_bbox;
  /// Last used movable number for wor_bbox.
  long wor_bbox_movablenr;

  /**
   * Position in the world.
   */
  csMovable movable; //@@MS: BAAAD

  /**
   * The renderer will render all objects in a sector based on this
   * number. Low numbers get rendered first. High numbers get rendered
   * later. There are a few predefined slots which the application is
   * free to use or not.
   */
  long render_priority;

  /// For NR: Cached value from DrawTest
  bool draw_test;
  /// For NR: Cached light test
  bool in_light;
  /// For NR: Should we draw anything in drawshadow at all?
  bool cast_hardware_shadow;
  /// For NR: should we draw last
  bool draw_after_fancy_stuff;

  /**
   * This value indicates the last time that was used to do animation.
   * If 0 then we haven't done animation yet. We compare this value
   * with the value returned by engine->GetLastAnimationTime() to see
   * if we need to call meshobj->NextFrame() again.
   */
  csTicks last_anim_time;

  /**
   * Optional LOD control that will turn a hierarchical mesh in a
   * mesh that supports static LOD.
   */
  csRef<csStaticLODMesh> static_lod;

  /**
   * Minimum/maximum range for rendering this object.
   */
  bool do_minmax_range;
  float min_render_dist, max_render_dist;
  csRef<iSharedVariable> var_min_render_dist, var_max_render_dist;
  csRef<csLODListener> var_min_render_dist_listener;
  csRef<csLODListener> var_max_render_dist_listener;

  csShaderVariableContext svcontext;
  csRef<iShaderVariableContext> factorySVC;

  csEngine* engine;
private:
  /// Mesh object corresponding with this csMeshWrapper.
  csRef<iMeshObject> meshobj;
  /// For optimization purposes we keep the iLightingInfo interface here.
  csRef<iLightingInfo> light_info;
  /**
   * For optimization purposes we keep the iShadowReceiver interface here.
   * Also for maintaining the special version of the shadow receiver that
   * multiplexes in case of static lod.
   */
  csRef<iShadowReceiver> shadow_receiver;
  bool shadow_receiver_valid;
  /**
   * For optimization purposes we keep the iShadowCaster interface here.
   * Also for maintaining the special version of the shadow caster that
   * multiplexes in case of static lod.
   */
  csRef<iShadowCaster> shadow_caster;
  bool shadow_caster_valid;

  /**
   * For optimization purposes we keep the portal container interface here
   * (only if this object is a portal container of course).
   */
  csRef<iPortalContainer> portal_container;

  /**
   * The callbacks which are called just before drawing.
   * Type: iMeshDrawCallback.
   */
  csRefArray<iMeshDrawCallback> draw_cb_vector;

  /// Optional reference to the parent csMeshFactoryWrapper.
  iMeshFactoryWrapper* factory;

  /// Z-buf mode to use for drawing this object.
  csZBufMode zbufMode;

  /// Flag indicating whether this mesh should try to imposter or not.
  bool imposter_active;
  /// Imposter Threshold Range.
  csRef<iSharedVariable> min_imposter_distance;
  /// Imposter Redo Threshold angle change.
  csRef<iSharedVariable> imposter_rotation_tolerance;
  csImposterMesh *imposter_mesh;

  /**
   * Array of lights affecting this mesh object. This is calculated
   * by the csLightManager class.
   */
  csDirtyAccessArray<iLightSectorInfluence*> relevant_lights;
#define MAX_INF_LIGHTS 100
  static LSIAndDist relevant_lights_cache[MAX_INF_LIGHTS];

  // This is a mirror of 'relevant_lights' which we use to detect if
  // a light has been removed or changed. It is only used in case we
  // are not updating all the time (if CS_LIGHTINGUPDATE_ALWAYSUPDATE is
  // not set).
  struct LightRef
  {
    csWeakRef<iLightSectorInfluence> lsi;
    uint32 light_nr;
  };
  csSafeCopyArray<LightRef> relevant_lights_ref;
  bool relevant_lights_valid;
  size_t relevant_lights_max;
  csFlags relevant_lights_flags;

  // In case the mesh has CS_ENTITY_NOCLIP set then this will
  // contain the value of the last frame number and camera pointer.
  // This is used to detect if we can skip rendering the mesh.
  iCamera* last_camera;
  uint last_frame_number;

public:
  CS_LEAKGUARD_DECLARE (csMeshWrapper);

  /// Set of flags
  csFlags flags;
  /// Culler flags.
  csFlags culler_flags;

  /**
   * Clear this object from all sector portal lists.
   * If a sector is given then it will only clear for that sector.
   */
  void ClearFromSectorPortalLists (iSector* sector = 0);
  /// Add this object to all sector portal lists.
  void AddToSectorPortalLists ();

protected:
  /// Get the bounding box in world space and correct in hierarchy.
  void GetFullBBox (csBox3& box);

  /// Move this object to the specified sector. Can be called multiple times.
  void MoveToSector (iSector* s);

  /**
   * Remove this object from all sectors it is in (but not from the engine).
   * If a sector is given then it will only be removed from that sector.
   */
  void RemoveFromSectors (iSector* sector = 0);

  /**
   * Update transformations after the object has moved
   * (through updating the movable instance).
   * This MUST be done after you change the movable otherwise
   * some of the internal data structures will not be updated
   * correctly. This function is called by movable.UpdateMove();
   */
  void UpdateMove ();

  /**
   * Destructor.  This is private in order to force clients to use DecRef()
   * for object destruction.
   */
  virtual ~csMeshWrapper ();

public:
  /// Constructor.
  csMeshWrapper (csEngine* engine, iMeshObject* meshobj = 0);

  /// Set the mesh factory.
  virtual void SetFactory (iMeshFactoryWrapper* factory)
  {
    csMeshWrapper::factory = factory;
    factorySVC = factory ? factory->GetSVContext() : 0;
  }
  /// Get the mesh factory.
  virtual iMeshFactoryWrapper* GetFactory () const
  {
    return factory;
  }

  /// Set the mesh object.
  virtual void SetMeshObject (iMeshObject* meshobj);
  /// Get the mesh object.
  virtual iMeshObject* GetMeshObject () const { return meshobj; }

  virtual iPortalContainer* GetPortalContainer () const
  { return portal_container; }
  virtual iShadowReceiver* GetShadowReceiver ();
  virtual iShadowCaster* GetShadowCaster ();

  /// For iVisibilityObject: Get the object model.
  virtual iObjectModel* GetObjectModel ()
  {
    return meshobj->GetObjectModel ();
  }

  // For iVisibilityObject:
  virtual iMeshWrapper* GetMeshWrapper () const
  {
    return (iMeshWrapper*)this;
  }

  // For iVisibilityObject:
  virtual csFlags& GetCullerFlags () { return culler_flags; }

  /**
   * Get the movable instance for this object.
   * It is very important to call GetMovable().UpdateMove()
   * after doing any kind of modification to this movable
   * to make sure that internal data structures are
   * correctly updated.
   */
  csMovable& GetCsMovable () { return movable; }

  // For iVisibilityObject.
  virtual iMovable* GetMovable () const
  {
    return (iMovable*)&movable;
  }

  /// Set the render priority for this object.
  virtual void SetRenderPriority (long rp);
  /// Set the render priority for this object and children.
  virtual void SetRenderPriorityRecursive (long rp);
  /// Get the render priority for this object.
  virtual long GetRenderPriority () const
  {
    return render_priority;
  }

  /// Set the Z-buf drawing mode to use for this object.
  virtual void SetZBufMode (csZBufMode mode) { zbufMode = mode; }
  void SetZBufModeRecursive (csZBufMode mode);
  /// Get the Z-buf drawing mode.
  virtual csZBufMode GetZBufMode () const { return zbufMode; }

  /// Set flags for mesh and children.
  virtual void SetFlagsRecursive (uint32 mask, uint32 value);

  /**
   * Set a callback which is called just before the object is drawn.
   * This is useful to do some expensive computations which only need
   * to be done on a visible object. Note that this function will be
   * called even if the object is not visible. In general it is called
   * if there is a likely probability that the object is visible (i.e.
   * it is in the same sector as the camera for example).
   */
  virtual void SetDrawCallback (iMeshDrawCallback* cb)
  {
    draw_cb_vector.Push (cb);
  }

  virtual void RemoveDrawCallback (iMeshDrawCallback* cb)
  {
    draw_cb_vector.Delete (cb);
  }

  virtual int GetDrawCallbackCount () const
  {
    return (int)draw_cb_vector.Length ();
  }

  virtual iMeshDrawCallback* GetDrawCallback (int idx) const
  {
    return draw_cb_vector.Get (idx);
  }

  virtual void SetLightingUpdate (int flags, int num_lights);

  /**
   * Get the array of relevant lights for this object.
   */
  const csArray<iLightSectorInfluence*>& GetRelevantLights (
  	int maxLights, bool desireSorting);
  /**
   * Forcibly invalidate relevant lights.
   */
  void InvalidateRelevantLights () { relevant_lights_valid = false; }

  /**
   * Draw this mesh object given a camera transformation.
   * If needed the skeleton state will first be updated.
   */
  void Draw (iRenderView* rview, uint32 frustum_mask);

  // Static LOD methods.
  virtual iLODControl* CreateStaticLOD ();
  virtual void DestroyStaticLOD ();
  virtual iLODControl* GetStaticLOD ();
  virtual void RemoveMeshFromStaticLOD (iMeshWrapper* mesh);
  virtual void AddMeshToStaticLOD (int lod, iMeshWrapper* mesh);
  csStaticLODMesh* GetStaticLODMesh () const { return static_lod; }
  /// Return true if there is a parent mesh that has static lod.
  bool SomeParentHasStaticLOD () const;

  /**
   * Draw the zpass for the object.  If this object doesn't use lighting
   * then it can be drawn fully here.
   */
  csRenderMesh** GetRenderMeshes (int& num, iRenderView* rview,
  	uint32 frustum_mask);

  /**
   * Do a hard transform of this object.
   * This transformation and the original coordinates are not
   * remembered but the object space coordinates are directly
   * computed (world space coordinates are set to the object space
   * coordinates by this routine). Note that some implementations
   * of mesh objects will not change the orientation of the object but
   * only the position.
   */
  virtual void HardTransform (const csReversibleTransform& t);

  //---------- iImposter Functions -----------------//

  /// Set true if this Mesh should use Impostering.
  virtual void SetImposterActive (bool flag);

  /**
   * Determine if this mesh is using Impostering
   * (not if Imposter is being drawn, but simply considered).
   */
  virtual bool GetImposterActive () const { return imposter_active; }

  /**
   * Minimum Imposter Distance is the distance from camera 
   * beyond which imposter is used. Imposter gets a 
   * ptr here because value is a shared variable 
   * which can be changed at runtime for many objects.
   */
  virtual void SetMinDistance (iSharedVariable* dist)
  { min_imposter_distance = dist; }

  /** 
   * Rotation Tolerance is the maximum allowable 
   * angle difference between when the imposter was 
   * created and the current position of the camera.
   * Angle greater than this triggers a re-render of
   * the imposter.
   */
  virtual void SetRotationTolerance (iSharedVariable* angle)
  { imposter_rotation_tolerance = angle; }

  /**
   * Tells the object to create its proctex and polygon
   * for use by main render process later, relative to
   * the specified Point Of View.
   */
  virtual void CreateImposter (csReversibleTransform& /*pov*/)
  { /* implement later */ }

  /**
   * Renders the imposter on the screen.
   */
  bool DrawImposter (iRenderView *rview);

  /// Determine if imposter or true rendering will be used.
  virtual bool WouldUseImposter (csReversibleTransform& /*pov*/) const
  { /* implement later */ return false; }

  /// This is the function to check distances.  Fn above may not be needed.
  bool CheckImposterRelevant (iRenderView *rview);
  
  //---------- Bounding volume and beam functions -----------------//

  virtual void GetRadius (float& rad, csVector3& cent) const;
  virtual csSphere GetRadius () const;

  virtual int HitBeamBBox (const csVector3& start, const csVector3& end,
         csVector3& isect, float* pr);
  virtual bool HitBeamOutline (const csVector3& start, const csVector3& end,
         csVector3& isect, float* pr);
  virtual bool HitBeamObject (const csVector3& start, const csVector3& end,
  	csVector3& isect, float* pr, int* polygon_idx = 0);
  virtual bool HitBeam (const csVector3& start, const csVector3& end,
  	csVector3& isect, float* pr, iMaterialWrapper** material = 0);
  virtual csHitBeamResult HitBeamBBox (const csVector3& start,
  	const csVector3& end);
  virtual csHitBeamResult HitBeamOutline (const csVector3& start,
  	const csVector3& end);
  virtual csHitBeamResult HitBeamObject (const csVector3& start,
  	const csVector3& end, bool do_material = false);
  virtual csHitBeamResult HitBeam (const csVector3& start,
  	const csVector3& end, bool do_material = false);

  /**
   * Calculate the squared distance between the camera and the object.
   */
  float GetSquaredDistance (iRenderView *rview);
  /**
   * Calculate the squared distance between a position and the object.
   */
  float GetSquaredDistance (const csVector3& pos);

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
  virtual void PlaceMesh ();

  virtual void GetWorldBoundingBox (csBox3& cbox);
  virtual void GetTransformedBoundingBox (const csReversibleTransform& trans,
  	csBox3& cbox);
  virtual float GetScreenBoundingBox (iCamera *camera, csBox2& sbox,
  	csBox3& cbox);
  virtual const csBox3& GetWorldBoundingBox ();
  virtual csBox3 GetTransformedBoundingBox (const csReversibleTransform& trans);
  virtual csScreenBoxResult GetScreenBoundingBox (iCamera *camera);

  //--------------------- SCF stuff follows ------------------------------//
  /**\name iShaderVariableContext implementation
   * @{ */
  /// Add a variable to this context
  void AddVariable (csShaderVariable *variable)
  { svcontext.AddVariable (variable); }

  /// Get a named variable from this context
  csShaderVariable* GetVariable (csStringID name) const
  { 
    csShaderVariable* sv = svcontext.GetVariable (name); 
    if ((sv == 0) && (factorySVC.IsValid()))
      sv = factorySVC->GetVariable (name);
    return sv;
  }

  /// Get Array of all ShaderVariables
  const csRefArray<csShaderVariable>& GetShaderVariables () const
  { 
    // @@@ Will not return factory SVs
    return svcontext.GetShaderVariables (); 
  }

  /**
   * Push the variables of this context onto the variable stacks
   * supplied in the "stacks" argument
   */
  void PushVariables (iShaderVarStack* stacks) const
  { 
    if (factorySVC.IsValid()) factorySVC->PushVariables (stacks);
    svcontext.PushVariables (stacks); 
  }

  bool IsEmpty () const 
  {
    return svcontext.IsEmpty() 
      && (!factorySVC.IsValid() || factorySVC->IsEmpty());
  }

  void ReplaceVariable (csShaderVariable *variable)
  { svcontext.ReplaceVariable (variable); }
  void Clear () { svcontext.Clear(); }
  /** @} */

  //--------------------- iSelfDestruct implementation -------------------//

  virtual void SelfDestruct ();

  /**\name iSceneNode implementation
   * @{ */
  virtual void SetParent (iSceneNode* parent);
  virtual iSceneNode* GetParent () const
  {
    if (movable.GetParent ())
      return movable.GetParent ()->GetSceneNode ();
    else
      return 0;
  }
  virtual const csRefArray<iSceneNode>& GetChildren () const
  {
    return movable.GetChildren ();
  }
  virtual iMeshWrapper* QueryMesh () { return this; }
  virtual iLight* QueryLight () { return 0; }
  virtual iCamera* QueryCamera () { return 0; }
  virtual csPtr<iSceneNodeArray> GetChildrenArray () const
  {
    return csPtr<iSceneNodeArray> (
      new scfArrayWrapConst<iSceneNodeArray, csRefArray<iSceneNode> > (
      movable.GetChildren ()));
  }
  /** @} */

  //--------------------- iMeshWrapper implementation --------------------//

  virtual iObject *QueryObject ()
  {
    return this;
  }
  virtual iSceneNode* QuerySceneNode ()
  {
    return this;
  }
  virtual iMeshWrapper* FindChildByName (const char* name);
  virtual iLightingInfo* GetLightingInfo () const
  {
    return light_info;
  }
  virtual csFlags& GetFlags ()
  {
    return flags;
  }
  virtual iShaderVariableContext* GetSVContext()
  {
    return (iShaderVariableContext*)this;
  }

  void ClearMinVariable ();
  void ClearMaxVariable ();
  bool DoMinMaxRange () const { return do_minmax_range; }
  virtual void ResetMinMaxRenderDistance ();
  virtual void SetMinimumRenderDistance (float min);
  virtual float GetMinimumRenderDistance () const { return min_render_dist; }
  virtual void SetMaximumRenderDistance (float max);
  virtual float GetMaximumRenderDistance () const { return max_render_dist; }
  virtual void SetMinimumRenderDistanceVar (iSharedVariable* min);
  virtual iSharedVariable* GetMinimumRenderDistanceVar () const
  {
    return var_min_render_dist;
  }
  virtual void SetMaximumRenderDistanceVar (iSharedVariable* max);
  virtual iSharedVariable* GetMaximumRenderDistanceVar () const
  {
    return var_max_render_dist;
  }
};

#include "csutil/win32/msvc_deprecated_warn_on.h"

/**
 * The holder class for all implementations of iMeshObjectFactory.
 */
class csMeshFactoryWrapper : public scfImplementationExt3<csMeshFactoryWrapper,
                                                          csObject, 
                                                          iMeshFactoryWrapper,
                                                          iShaderVariableContext,
							  iSelfDestruct>
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

  /// Suggestion for new children created from factory.
  long render_priority;
  /// Suggestion for new children created from factory.
  csZBufMode zbufMode;

  csShaderVariableContext svcontext;

  csFlags flags;

  csEngine* engine;
public:
  /// Constructor.
  csMeshFactoryWrapper (csEngine* engine, iMeshObjectFactory* meshFact);
  /// Constructor.
  csMeshFactoryWrapper (csEngine* engine);
  /// Destructor.
  virtual ~csMeshFactoryWrapper ();

  CS_LEAKGUARD_DECLARE (csMeshFactoryWrapper);

  /// Set the mesh object factory.
  void SetMeshObjectFactory (iMeshObjectFactory* meshFact);

  /// Get the mesh object factory.
  iMeshObjectFactory* GetMeshObjectFactory () const
  {
    return meshFact;
  }

  virtual csFlags& GetFlags ()
  {
    return flags;
  }

  /**
   * Create a new mesh object for this template.
   */
  csPtr<iMeshWrapper> CreateMeshWrapper ();

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

  virtual iMeshFactoryWrapper* GetParentContainer () const { return parent; }
  virtual void SetParentContainer (iMeshFactoryWrapper *p) { parent = p; }
  virtual iMeshFactoryList* GetChildren (){ return &children; }

  virtual iObject* QueryObject () { return this; }

  virtual iShaderVariableContext* GetSVContext()
  { return static_cast<iShaderVariableContext*> (this); }

  // Static LOD methods.
  iLODControl* CreateStaticLOD ();
  void DestroyStaticLOD ();
  iLODControl* GetStaticLOD ();
  void SetStaticLOD (float m, float a);
  void GetStaticLOD (float& m, float& a) const;
  void RemoveFactoryFromStaticLOD (iMeshFactoryWrapper* mesh);
  void AddFactoryToStaticLOD (int lod, iMeshFactoryWrapper* mesh);

  // Flags that are used for children.
  void SetZBufMode (csZBufMode mode) { zbufMode = mode; }
  csZBufMode GetZBufMode () const { return zbufMode; }
  void SetZBufModeRecursive (csZBufMode mode);
  void SetRenderPriority (long rp);
  long GetRenderPriority () const
  {
    return render_priority;
  }
  void SetRenderPriorityRecursive (long rp);

  //--------------------- iSelfDestruct implementation -------------------//

  virtual void SelfDestruct ();

  /**\name iShaderVariableContext implementation
   * @{ */
  /// Add a variable to this context
  void AddVariable (csShaderVariable *variable)
  { svcontext.AddVariable (variable); }

  /// Get a named variable from this context
  csShaderVariable* GetVariable (csStringID name) const
  { return svcontext.GetVariable (name); }

  /// Get Array of all ShaderVariables
  const csRefArray<csShaderVariable>& GetShaderVariables () const
  { return svcontext.GetShaderVariables (); }

  /**
   * Push the variables of this context onto the variable stacks
   * supplied in the "stacks" argument
   */
  void PushVariables (iShaderVarStack* stacks) const
  { svcontext.PushVariables (stacks); }

  bool IsEmpty () const 
  { return svcontext.IsEmpty(); }

  void ReplaceVariable (csShaderVariable *variable)
  { svcontext.ReplaceVariable (variable); }
  void Clear () { svcontext.Clear(); }
  /** @} */
};

#endif // __CS_MESHOBJ_H__
