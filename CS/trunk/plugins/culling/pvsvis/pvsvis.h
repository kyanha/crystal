/*
    Copyright (C) 2004 by Jorrit Tyberghein

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

#ifndef __CS_PVSVIS_H__
#define __CS_PVSVIS_H__

#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "iutil/dbghelp.h"
#include "csutil/array.h"
#include "csutil/parray.h"
#include "csutil/scf.h"
#include "csutil/hash.h"
#include "csgeom/plane3.h"
#include "igeom/objmodel.h"
#include "iengine/viscull.h"
#include "iengine/movable.h"
#include "iengine/shadcast.h"
#include "iengine/mesh.h"
#include "imesh/thing.h"
#include "pvstree.h"

class csKDTree;
class csKDTreeChild;
class csPVSVis;
struct iPolygonMesh;
struct iMovable;
struct iMeshWrapper;
struct iThingState;

struct PVSTest_Front2BackData;

/**
 * This object is a wrapper for an iVisibilityObject from the engine.
 */
class csPVSVisObjectWrapper : public iObjectModelListener,
	public iMovableListener
{
public:
  csPVSVis* pvsvis;
  iVisibilityObject* visobj;
  csKDTreeChild* child;
  long update_number;	// Last used update_number from movable.
  long shape_number;	// Last used shape_number from model.

  // Optional data for shadows. Both fields can be 0.
  csRef<iMeshWrapper> mesh;
  csRef<iShadowCaster> caster;

  csPVSVisObjectWrapper (csPVSVis* pvsvis)
  {
    SCF_CONSTRUCT_IBASE (0);
    csPVSVisObjectWrapper::pvsvis = pvsvis;
  }
  virtual ~csPVSVisObjectWrapper ()
  {
    SCF_DESTRUCT_IBASE();
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
 * A PVS based visisibility culling system.
 */
class csPVSVis : public iVisibilityCuller, public iPVSCuller
{
public:
  // List of objects to iterate over (after VisTest()).
  csArray<iVisibilityObject*> vistest_objects;
  bool vistest_objects_inuse;	// If true the vector is in use.

private:
  iObjectRegistry *object_reg;
  csKDTree* kdtree;
  // Ever growing box of all objects that were ever in the tree.
  // This puts an upper limit of all boxes in the kdtree itself because
  // those go off to infinity.
  csBox3 kdtree_box;
  csPDelArray<csPVSVisObjectWrapper> visobj_vector;
  int scr_width, scr_height;	// Screen dimensions.
  uint32 current_vistest_nr;

  // The PVS information.
  csStaticPVSTree pvstree;

  // This hash set holds references to csPVSVisObjectWrapper instances
  // that require updating in the culler.
  csSet<csPVSVisObjectWrapper*> update_queue;
  // The 'updating' flag is true if the objects are being updated. This flag
  // is to prevent us from updating it again (if the callback is fired
  // again).
  bool updating;

  // Update all objects in the update queue.
  void UpdateObjects ();

  // Fill the bounding box with the current object status.
  void CalculateVisObjBBox (iVisibilityObject* visobj, csBox3& bbox);

  // Traverse the kdtree for frustum culling.
  void PVSTest_Traverse (csKDTree* treenode,
	PVSTest_Front2BackData* data,
	uint32 cur_timestamp, uint32 frustum_mask);

public:
  SCF_DECLARE_IBASE;

  csPVSVis (iBase *iParent);
  virtual ~csPVSVis ();
  virtual bool Initialize (iObjectRegistry *object_reg);

#define NODE_INSIDE 2
#define NODE_VISIBLE 1
#define NODE_INVISIBLE 0
  // Test visibility for the given node. Returns 2 if camera is inside node,
  // 1 if visible normally, or 0 if not visible.
  // This function will also modify the frustum_mask in 'data'. So
  // take care to restore this later if you recurse down.
  int TestNodeVisibility (csKDTree* treenode,
  	PVSTest_Front2BackData* data, uint32& frustum_mask);

  // Test visibility for the given object. Returns true if visible.
  bool TestObjectVisibility (csPVSVisObjectWrapper* obj,
  	PVSTest_Front2BackData* data, uint32 frustum_mask);

  // Add an object to the update queue. That way it will be updated
  // in the kdtree later when needed.
  void AddObjectToUpdateQueue (csPVSVisObjectWrapper* visobj_wrap);

  // Update one object in PVSVis. This is called whenever the movable
  //   or object model changes.
  void UpdateObject (csPVSVisObjectWrapper* visobj_wrap);

  virtual void Setup (const char* name);
  virtual void RegisterVisObject (iVisibilityObject* visobj);
  virtual void UnregisterVisObject (iVisibilityObject* visobj);
  virtual bool VisTest (iRenderView* rview, 
    iVisibilityCullerListener* viscallback);
  virtual void PrecacheCulling () { VisTest ((iRenderView*)0, 0); }
  virtual csPtr<iVisibilityObjectIterator> VisTest (const csBox3& box);
  virtual csPtr<iVisibilityObjectIterator> VisTest (const csSphere& sphere);
  virtual void VisTest (const csSphere& sphere, 
    iVisibilityCullerListener* viscallback);
  virtual csPtr<iVisibilityObjectIterator> VisTest (csPlane3* planes,
  	int num_planes);
  virtual void VisTest (csPlane3* planes,
  	int num_planes, iVisibilityCullerListener* viscallback);
  virtual csPtr<iVisibilityObjectIterator> IntersectSegmentSloppy (
    const csVector3& start, const csVector3& end);
  virtual csPtr<iVisibilityObjectIterator> IntersectSegment (
    const csVector3& start, const csVector3& end, bool accurate = false);
  virtual bool IntersectSegment (const csVector3& start,
    const csVector3& end, csVector3& isect, float* pr = 0,
    iMeshWrapper** p_mesh = 0, int* poly_idx = 0,
    bool accurate = true);
  virtual void CastShadows (iFrustumView* fview);
  //virtual uint32 GetCurrentVisibilityNumber () const { return current_visnr; }

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE (csPVSVis);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize (p); }
  } scfiComponent;

  // For iPVSCuller
  virtual iStaticPVSTree* GetPVSTree () { return &pvstree; }
};

#endif // __CS_PVSVIS_H__

