/*
    Copyright (C) 2002 by Jorrit Tyberghein

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

#ifndef __CS_FRUSTVIS_H__
#define __CS_FRUSTVIS_H__

#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "iutil/dbghelp.h"
#include "csutil/array.h"
#include "csutil/parray.h"
#include "csutil/scf.h"
#include "csutil/hashmap.h"
#include "csgeom/plane3.h"
#include "igeom/objmodel.h"
#include "iengine/viscull.h"
#include "iengine/movable.h"
#include "iengine/shadcast.h"
#include "iengine/mesh.h"
#include "imesh/thing/thing.h"

class csKDTree;
class csKDTreeChild;
class csFrustumVis;
struct iPolygonMesh;
struct iMovable;
struct iMeshWrapper;
struct iThingState;

struct FrustTest_Front2BackData;

/**
 * This object is a wrapper for an iVisibilityObject from the engine.
 */
class csFrustVisObjectWrapper : public iObjectModelListener,
	public iMovableListener
{
public:
  csFrustumVis* frustvis;
  iVisibilityObject* visobj;
  csKDTreeChild* child;
  long update_number;	// Last used update_number from movable.
  long shape_number;	// Last used shape_number from model.

  uint32 last_visible_vistest_nr;

  // Optional data for shadows. Both fields can be 0.
  csRef<iMeshWrapper> mesh;
  csRef<iShadowCaster> caster;
  csRef<iThingState> thing_state;

  csFrustVisObjectWrapper (csFrustumVis* frustvis)
  {
    SCF_CONSTRUCT_IBASE (0);
    csFrustVisObjectWrapper::frustvis = frustvis;
  }
  virtual ~csFrustVisObjectWrapper ()
  {
  }

  SCF_DECLARE_IBASE;

  /// The object model has changed.
  virtual void ObjectModelChanged (iObjectModel* model);
  /// The movable has changed.
  virtual void MovableChanged (iMovable* movable);
  /// The movable is about to be destroyed.
  virtual void MovableDestroyed (iMovable*) { }
};

/**
 * A simple frustum based visisibility culling system.
 */
class csFrustumVis : public iVisibilityCuller
{
public:
  // List of objects to iterate over (after VisTest()).
  csArray<iVisibilityObject*> vistest_objects;
  bool vistest_objects_inuse;	// If true the vector is in use.

private:
  iObjectRegistry *object_reg;
  csKDTree* kdtree;
  csPDelArray<csFrustVisObjectWrapper> visobj_vector;
  int scr_width, scr_height;	// Screen dimensions.
  uint32 current_vistest_nr;

  // This hash set holds references to csFrustVisObjectWrapper instances
  // that require updating in the culler.
  csHashSet update_queue;
  // The 'updating' flag is true if the objects are being updated. This flag
  // is to prevent us from updating it again (if the callback is fired
  // again).
  bool updating;

  // Update all objects in the update queue.
  void UpdateObjects ();

  // Fill the bounding box with the current object status.
  void CalculateVisObjBBox (iVisibilityObject* visobj, csBox3& bbox);

public:
  SCF_DECLARE_IBASE;

  csFrustumVis (iBase *iParent);
  virtual ~csFrustumVis ();
  virtual bool Initialize (iObjectRegistry *object_reg);

  // Test visibility for the given node. Returns true if visible.
  // This function will also modify the frustum_mask in 'data'. So
  // take care to restore this later if you recurse down.
  bool TestNodeVisibility (csKDTree* treenode,
  	FrustTest_Front2BackData* data, uint32& frustum_mask);

  // Test visibility for the given object. Returns true if visible.
  bool TestObjectVisibility (csFrustVisObjectWrapper* obj,
  	FrustTest_Front2BackData* data, uint32 frustum_mask);

  // Add an object to the update queue. That way it will be updated
  // in the kdtree later when needed.
  void AddObjectToUpdateQueue (csFrustVisObjectWrapper* visobj_wrap);

  // Update one object in FrustVis. This is called whenever the movable
  //   or object model changes.
  void UpdateObject (csFrustVisObjectWrapper* visobj_wrap);

  virtual void Setup (const char* name);
  virtual void RegisterVisObject (iVisibilityObject* visobj);
  virtual void UnregisterVisObject (iVisibilityObject* visobj);
  virtual bool VisTest (iRenderView* rview, 
    iVisibilityCullerListner* viscallback);
  virtual csPtr<iVisibilityObjectIterator> VisTest (const csBox3& box);
  virtual csPtr<iVisibilityObjectIterator> VisTest (const csSphere& sphere);
  virtual csPtr<iVisibilityObjectIterator> VisTest (csPlane3* planes,
  	int num_planes);
  virtual csPtr<iVisibilityObjectIterator> IntersectSegment (
    const csVector3& start, const csVector3& end, bool accurate = false);
  virtual bool IntersectSegment (const csVector3& start,
    const csVector3& end, csVector3& isect, float* pr = 0,
    iMeshWrapper** p_mesh = 0, iPolygon3D** poly = 0,
    bool accurate = false);
  virtual void CastShadows (iFrustumView* fview);
  //virtual uint32 GetCurrentVisibilityNumber () const { return current_visnr; }

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE (csFrustumVis);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize (p); }
  } scfiComponent;
};

#endif // __CS_FRUSTVIS_H__

