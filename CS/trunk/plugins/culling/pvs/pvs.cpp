/*
    Copyright (C) 2006 by Jorrit Tyberghein and Benjamin Stover

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
#include <string.h>
#include "csver.h"
#include "csutil/sysfunc.h"
#include "csutil/scf.h"
#include "csutil/util.h"
#include "csutil/scfstr.h"
#include "csutil/event.h"
#include "csutil/list.h"
#include "iutil/event.h"
#include "iutil/eventq.h"
#include "csgeom/frustum.h"
#include "csgeom/matrix3.h"
#include "csgeom/math3d.h"
#include "csgeom/obb.h"
#include "csgeom/segment.h"
#include "csgeom/sphere.h"
#include "csgeom/kdtree.h"
#include "igeom/polymesh.h"
#include "imesh/objmodel.h"
#include "csutil/flags.h"
#include "iutil/objreg.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/txtmgr.h"
#include "ivideo/fontserv.h"
#include "iengine/movable.h"
#include "iengine/rview.h"
#include "iengine/camera.h"
#include "iengine/mesh.h"
#include "iengine/shadcast.h"
#include "iengine/shadows.h"
#include "iengine/fview.h"
#include "imesh/object.h"
#include "iutil/object.h"
#include "ivaria/reporter.h"
#include "pvs.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (csPVSVis)


SCF_IMPLEMENT_IBASE (csPVSVis)
  SCF_IMPLEMENTS_INTERFACE (iVisibilityCuller)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csPVSVis::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csPVSVis::eiEventHandler)
  SCF_IMPLEMENTS_INTERFACE (iEventHandler)
SCF_IMPLEMENT_IBASE_END

//----------------------------------------------------------------------

// TODO:  enumify and maybe add to cstypes?
#define CS_XAXIS 0
#define CS_YAXIS 1
#define CS_ZAXIS 2

class csStaticKDTreeObject : public csRefCount {
public:
    friend class csStaticKDTree;

    csStaticKDTreeObject(const csBox3& box, void* object) {
        csStaticKDTreeObject::box = box;
        csStaticKDTreeObject::object = object;
    }

    /// Get the bounding box of the object.  const csBox3& GetBBox() const { return box; }
    /// Get the user data.
    void* GetObject() { return object; }

private:
    csBox3 box;
    void* object;
};

class csStaticKDTree {
public:
    csStaticKDTree(csRefArray<csStaticKDTreeObject>& items);
    ~csStaticKDTree();
    csStaticKDTreeObject* AddObject(const csBox3& bbox, void* userdata);
    void UnlinkObject(csStaticKDTreeObject* object);
    void MoveObject(const csBox3& bbox_new, csStaticKDTreeObject* object);
    csStaticKDTree* getChild1() { return child1; }
    csStaticKDTree* getChild2() { return child2; }

private:
    csStaticKDTree* child1;
    csStaticKDTree* child2;
    csBox3 node_bbox;

    int axis;
    float splitLocation;

    // If no children, this node contains objects
    csRefArray<csStaticKDTreeObject>* objects;

    bool isLeafNode() { return child1; }

    static float getMin(int axis, const csBox3& box) {
        if (axis == CS_XAXIS) return box.MinX();
        else if (axis == CS_YAXIS) return box.MinY();
        else if (axis == CS_ZAXIS) return box.MinZ();
        CS_ASSERT(false);
        return 0;
    }

    static float getMax(int axis, const csBox3& box) {
        if (axis == CS_XAXIS) return box.MaxX();
        else if (axis == CS_YAXIS) return box.MaxY();
        else if (axis == CS_ZAXIS) return box.MaxZ();
        CS_ASSERT(false);
        return 0;
    }

    void AddObject(csRef<csStaticKDTreeObject>& object);
};

// TODO:  reference counting is not good at every step
csStaticKDTree::csStaticKDTree(csRefArray<csStaticKDTreeObject>& items) {
    // Base case:  if there are less than a certain threshold, stop recursing
    if (items.Length() < 10) {
        objects = new csRefArray<csStaticKDTreeObject>(items);
    }
    // Otherwise find the axis with the longest min-max distance and use the midpoint as a splitting axis.
    else {
        csRefArray<csStaticKDTreeObject>::Iterator it = items.GetIterator();
        float xmin = 0, ymin = 0, zmin = 0;
        float xmax = 0, ymax = 0, zmax = 0;
        while (it.HasNext()) {
            csStaticKDTreeObject& obj = *it.Next();
            xmin = fmin(obj.box.MinX(), xmin);
            ymin = fmin(obj.box.MinY(), ymin);
            zmin = fmin(obj.box.MinZ(), zmin);
            xmax = fmin(obj.box.MaxX(), xmax);
            ymax = fmin(obj.box.MaxY(), ymax);
            zmax = fmin(obj.box.MaxZ(), zmax);
        }

        float xdist = xmax - xmin;
        float ydist = ymax - ymin;
        float zdist = zmax - zmin;
        float maxdist = fmax(xdist, fmax(ydist, zdist));

        if (xdist == maxdist) {
            splitLocation = (xmax + xmin) / 2;
            axis = CS_XAXIS;
        }
        else if (ydist == maxdist) {
            splitLocation = (ymax + ymin) / 2;
            axis = CS_YAXIS;
        }
        else {
            splitLocation = (zmax + zmin) / 2;
            axis = CS_ZAXIS;
        }

        it = items.GetIterator();
        csRefArray<csStaticKDTreeObject> left;
        csRefArray<csStaticKDTreeObject> right;

        while (it.HasNext()) {
            csRef<csStaticKDTreeObject> obj = it.Next();

            float min = getMin(axis, obj->box), max = getMax(axis, obj->box);
            if (splitLocation < min)
                left.Push(obj);
            else if (splitLocation > max)
                right.Push(obj);
            else {
                left.Push(obj);
                right.Push(obj);
            }
        }

        child1 = new csStaticKDTree(left);
        child1 = new csStaticKDTree(right);
    }
}

csStaticKDTree::~csStaticKDTree() {
    if (isLeafNode()) {
        CS_ASSERT(!child2);
        CS_ASSERT(objects);
        delete objects;
    }
    else {
        CS_ASSERT(child2);
        CS_ASSERT(!objects);
        delete child1;
        delete child2;
    }
}

csStaticKDTreeObject* csStaticKDTree::AddObject(const csBox3& bbox, void* userdata) {
    csRef<csStaticKDTreeObject> ref = new csStaticKDTreeObject(bbox, userdata);
    AddObject(ref);
    return ref;
}

void csStaticKDTree::AddObject(csRef<csStaticKDTreeObject>& object) {
    if (isLeafNode()) {
        objects->Push(object);
    }
    else {
        float min = getMin(axis, object->box), max = getMax(axis, object->box);
        if (splitLocation < min)
            child1->AddObject(object);
        else if (splitLocation > max)
            child2->AddObject(object);
        else {
            child1->AddObject(object);
            child2->AddObject(object);
        }
    }
}

void csStaticKDTree::UnlinkObject(csStaticKDTreeObject* object) {
    if (isLeafNode()) {
        objects->DeleteFast(object);
    }
    else {
        float min = getMin(axis, object->box), max = getMax(axis, object->box);
        if (splitLocation < min)
            child1->UnlinkObject(object);
        else if (splitLocation > max)
            child2->UnlinkObject(object);
        else {
            child1->UnlinkObject(object);
            child2->UnlinkObject(object);
        }
    }
}

void csStaticKDTree::MoveObject(const csBox3& bbox_new, csStaticKDTreeObject* object) {
    UnlinkObject(object);
    object->box = bbox_new;
    csRef<csStaticKDTreeObject> ref(object);
    AddObject(ref);
}

//----------------------------------------------------------------------

class csPVSVisObjIt : public iVisibilityObjectIterator
{
private:
  csArray<iVisibilityObject*>* vector;
  size_t position;
  bool* vistest_objects_inuse;

public:
  SCF_DECLARE_IBASE;

  csPVSVisObjIt (csArray<iVisibilityObject*>* vector,
  	bool* vistest_objects_inuse)
  {
    SCF_CONSTRUCT_IBASE (0);
    csPVSVisObjIt::vector = vector;
    csPVSVisObjIt::vistest_objects_inuse = vistest_objects_inuse;
    if (vistest_objects_inuse) *vistest_objects_inuse = true;
    Reset ();
  }
  virtual ~csPVSVisObjIt ()
  {
    // If the vistest_objects_inuse pointer is not 0 we set the
    // bool to false to indicate we're no longer using the base
    // vector. Otherwise we delete the vector.
    if (vistest_objects_inuse)
      *vistest_objects_inuse = false;
    else
      delete vector;
    SCF_DESTRUCT_IBASE();
  }

  virtual iVisibilityObject* Next()
  {
    if (position == (size_t)-1) return 0;
    iVisibilityObject* vo = vector->Get (position);
    position++;
    if (position == vector->Length ())
      position = (size_t)-1;
    return vo;
  }

  virtual void Reset()
  {
    if (vector == 0 || vector->Length () < 1)
      position = (size_t)-1;
    else
      position = 0;
  }

  virtual bool HasNext () const
  {
    return ((position != (size_t)-1) && position <= vector->Length ());
  }
};

SCF_IMPLEMENT_IBASE (csPVSVisObjIt)
  SCF_IMPLEMENTS_INTERFACE (iVisibilityObjectIterator)
SCF_IMPLEMENT_IBASE_END

//----------------------------------------------------------------------


SCF_IMPLEMENT_IBASE (csPVSVisObjectWrapper)
  SCF_IMPLEMENTS_INTERFACE (iObjectModelListener)
  SCF_IMPLEMENTS_INTERFACE (iMovableListener)
SCF_IMPLEMENT_IBASE_END

void csPVSVisObjectWrapper::ObjectModelChanged (iObjectModel* /*model*/)
{
  pvsvis->AddObjectToUpdateQueue (this);
}

void csPVSVisObjectWrapper::MovableChanged (iMovable* /*movable*/)
{
  pvsvis->AddObjectToUpdateQueue (this);
}

class csPVSVisObjectDescriptor : public scfImplementation1<
	csPVSVisObjectDescriptor, iKDTreeObjectDescriptor>
{
public:
  csPVSVisObjectDescriptor () : scfImplementationType (this) { }
  ~csPVSVisObjectDescriptor () { }
  virtual csPtr<iString> DescribeObject (csKDTreeChild* child)
  {
    scfString* str = new scfString ();
    const csBox3& b = child->GetBBox ();
    csPVSVisObjectWrapper* obj = (csPVSVisObjectWrapper*)(
    	child->GetObject ());
    str->Format ("'%s' (%g,%g,%g)-(%g,%g,%g)",
    	obj->mesh->QueryObject ()->GetName (),
	b.MinX (), b.MinY (), b.MinZ (),
	b.MaxX (), b.MaxY (), b.MaxZ ());
    return str;
  }
};

//----------------------------------------------------------------------

csPVSVis::csPVSVis (iBase *iParent) :
	vistest_objects (256, 256),
	visobj_vector (256, 256),
	update_queue (151, 59)
{
  SCF_CONSTRUCT_IBASE (iParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent);
  object_reg = 0;
  kdtree = 0;
  current_vistest_nr = 1;
  vistest_objects_inuse = false;
  updating = false;

  scfiEventHandler = new eiEventHandler (this);
}

csPVSVis::~csPVSVis ()
{
  if (scfiEventHandler)
  {
    csRef<iEventQueue> q = CS_QUERY_REGISTRY (object_reg, iEventQueue);
    if (q)
      q->RemoveListener (scfiEventHandler);
    scfiEventHandler->DecRef ();
  }

  while (visobj_vector.Length () > 0)
  {
    csPVSVisObjectWrapper* visobj_wrap = visobj_vector.Pop ();
    iVisibilityObject* visobj = visobj_wrap->visobj;
    visobj->GetObjectModel ()->RemoveListener (
		      (iObjectModelListener*)visobj_wrap);
    iMovable* movable = visobj->GetMovable ();
    movable->RemoveListener ((iMovableListener*)visobj_wrap);
    kdtree->RemoveObject (visobj_wrap->child);
    visobj->DecRef ();
  }
  delete kdtree;
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiComponent);
  SCF_DESTRUCT_IBASE ();
}

bool csPVSVis::HandleEvent (iEvent& ev)
{
  if (ev.Name == CanvasResize)
  {
    csRef<iGraphics3D> g3d = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
    scr_width = g3d->GetWidth ();
    scr_height = g3d->GetHeight ();
  }
  return false;
}

bool csPVSVis::Initialize (iObjectRegistry *object_reg)
{
  csPVSVis::object_reg = object_reg;

  delete kdtree;

  csRef<iGraphics3D> g3d = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
  if (g3d)
  {
    scr_width = g3d->GetWidth ();
    scr_height = g3d->GetHeight ();
  }
  else
  {
    // If there is no g3d we currently assume we are testing.
    scr_width = 640;
    scr_height = 480;
  }

  kdtree = new csKDTree ();
  csPVSVisObjectDescriptor* desc = new csPVSVisObjectDescriptor ();
  kdtree->SetObjectDescriptor (desc);
  desc->DecRef ();

  csRef<iGraphics2D> g2d = csQueryRegistry<iGraphics2D> (object_reg);
  if (g2d)
  {
    CanvasResize = csevCanvasResize(object_reg, g2d);
    csRef<iEventQueue> q = CS_QUERY_REGISTRY (object_reg, iEventQueue);
    if (q)
      q->RegisterListener (scfiEventHandler, CanvasResize);
  }

  return true;
}

void csPVSVis::Setup (const char* /*name*/)
{
}

void csPVSVis::CalculateVisObjBBox (iVisibilityObject* visobj, csBox3& bbox)
{
  iMovable* movable = visobj->GetMovable ();
  if (movable->IsFullTransformIdentity ())
  {
    visobj->GetObjectModel ()->GetObjectBoundingBox (bbox);
  }
  else
  {
    csBox3 box;
    visobj->GetObjectModel ()->GetObjectBoundingBox (box);
    csReversibleTransform trans = movable->GetFullTransform ();
    bbox.StartBoundingBox (trans.This2Other (box.GetCorner (0)));
    bbox.AddBoundingVertexSmart (trans.This2Other (box.GetCorner (1)));
    bbox.AddBoundingVertexSmart (trans.This2Other (box.GetCorner (2)));
    bbox.AddBoundingVertexSmart (trans.This2Other (box.GetCorner (3)));
    bbox.AddBoundingVertexSmart (trans.This2Other (box.GetCorner (4)));
    bbox.AddBoundingVertexSmart (trans.This2Other (box.GetCorner (5)));
    bbox.AddBoundingVertexSmart (trans.This2Other (box.GetCorner (6)));
    bbox.AddBoundingVertexSmart (trans.This2Other (box.GetCorner (7)));
  }
}

void csPVSVis::RegisterVisObject (iVisibilityObject* visobj)
{
#ifdef CS_DEBUG
  size_t i;
  for (i = 0 ; i < visobj_vector.Length () ; i++)
  {
    if (visobj_vector[i]->visobj == visobj)
    {
      CS_ASSERT (false);
    }
  }
#endif
  csPVSVisObjectWrapper* visobj_wrap = new csPVSVisObjectWrapper (
		  this);
  visobj_wrap->visobj = visobj;
  visobj->IncRef ();
  iMovable* movable = visobj->GetMovable ();
  visobj_wrap->update_number = movable->GetUpdateNumber ();
  visobj_wrap->shape_number = visobj->GetObjectModel ()->GetShapeNumber ();

  csBox3 bbox;
  CalculateVisObjBBox (visobj, bbox);
  visobj_wrap->child = kdtree->AddObject (bbox, (void*)visobj_wrap);
  kdtree_box += bbox;

  iMeshWrapper* mesh = visobj->GetMeshWrapper ();
  visobj_wrap->mesh = mesh;
  if (mesh)
  {
    visobj_wrap->caster = mesh->GetShadowCaster ();
  }

  // Only add the listeners at the very last moment to prevent them to
  // be called by the calls above (i.e. especially the calculation of
  // the bounding box could cause a listener to be fired).
  movable->AddListener ((iMovableListener*)visobj_wrap);
  visobj->GetObjectModel ()->AddListener (
		  (iObjectModelListener*)visobj_wrap);

  visobj_vector.Push (visobj_wrap);
}

void csPVSVis::UnregisterVisObject (iVisibilityObject* visobj)
{
  size_t i;
  for (i = 0 ; i < visobj_vector.Length () ; i++)
  {
    csPVSVisObjectWrapper* visobj_wrap = visobj_vector[i];
    if (visobj_wrap->visobj == visobj)
    {
      update_queue.Delete (visobj_wrap);
      visobj->GetMovable ()->RemoveListener (
		  (iMovableListener*)visobj_wrap);
      iObjectModel* objmodel = visobj->GetObjectModel ();
      objmodel->RemoveListener ((iObjectModelListener*)visobj_wrap);
      kdtree->RemoveObject (visobj_wrap->child);
      visobj->DecRef ();
#ifdef CS_DEBUG
      // To easily recognize that the vis wrapper has been deleted:
      visobj_wrap->pvsvis = (csPVSVis*)0xdeadbeef;
#endif
      visobj_vector.DeleteIndexFast (i);
      return;
    }
  }
}

void csPVSVis::AddObjectToUpdateQueue (csPVSVisObjectWrapper* visobj_wrap)
{
  if (updating) return;
  CS_ASSERT (visobj_wrap->pvsvis != (csPVSVis*)0xdeadbeef);
  update_queue.Add (visobj_wrap);
}

void csPVSVis::UpdateObjects ()
{
  updating = true;
  {
    csSet<csPtrKey<csPVSVisObjectWrapper> >::GlobalIterator it = 
      update_queue.GetIterator ();
    while (it.HasNext ())
    {
      csPVSVisObjectWrapper* vw = it.Next ();
      UpdateObject (vw);
    }
  }
  update_queue.DeleteAll ();
  updating = false;
}

void csPVSVis::UpdateObject (csPVSVisObjectWrapper* visobj_wrap)
{
  CS_ASSERT (visobj_wrap->pvsvis != (csPVSVis*)0xdeadbeef);
  iVisibilityObject* visobj = visobj_wrap->visobj;
  iMovable* movable = visobj->GetMovable ();
  csBox3 bbox;
  CalculateVisObjBBox (visobj, bbox);
  kdtree->MoveObject (visobj_wrap->child, bbox);
  kdtree_box += bbox;
  visobj_wrap->shape_number = visobj->GetObjectModel ()->GetShapeNumber ();
  visobj_wrap->update_number = movable->GetUpdateNumber ();
}

struct FrustTest_Front2BackData
{
  csVector3 pos;
  iRenderView* rview;
  csPlane3* frustum;
  // this is the callback to call when we discover a visible node
  iVisibilityCullerListener* viscallback;
};

int csPVSVis::TestNodeVisibility (csKDTree* treenode,
	FrustTest_Front2BackData* data, uint32& frustum_mask)
{
  csBox3 node_bbox = treenode->GetNodeBBox ();
  node_bbox *= kdtree_box;

  if (node_bbox.Contains (data->pos))
  {
    return NODE_INSIDE;
  }

  uint32 new_mask;
  if (!csIntersect3::BoxFrustum (node_bbox, data->frustum, frustum_mask,
  	new_mask))
  {
    return NODE_INVISIBLE;
  }
  frustum_mask = new_mask;
  return NODE_VISIBLE;
}

bool csPVSVis::TestObjectVisibility (csPVSVisObjectWrapper* obj,
  	FrustTest_Front2BackData* data, uint32 frustum_mask)
{
  if (obj->mesh && obj->mesh->GetFlags ().Check (CS_ENTITY_INVISIBLEMESH))
    return false;

  const csBox3& obj_bbox = obj->child->GetBBox ();
  if (obj_bbox.Contains (data->pos))
  {
    data->viscallback->ObjectVisible (obj->visobj, obj->mesh, frustum_mask);
    return true;
  }
  
  uint32 new_mask;
  if (!csIntersect3::BoxFrustum (obj_bbox, data->frustum,
		frustum_mask, new_mask))
  {
    return false;
  }

  data->viscallback->ObjectVisible (obj->visobj, obj->mesh, new_mask);

  return true;
}

//======== VisTest =========================================================

static void CallVisibilityCallbacksForSubtree (csKDTree* treenode,
	FrustTest_Front2BackData* data, uint32 cur_timestamp)
{
  int num_objects = treenode->GetObjectCount ();
  csKDTreeChild** objects = treenode->GetObjects ();
  int i;
  for (i = 0 ; i < num_objects ; i++)
  {
    if (objects[i]->timestamp != cur_timestamp)
    {
      objects[i]->timestamp = cur_timestamp;
      csPVSVisObjectWrapper* visobj_wrap = (csPVSVisObjectWrapper*)
      	objects[i]->GetObject ();
      iMeshWrapper* mesh = visobj_wrap->mesh;
      if (!(mesh && mesh->GetFlags ().Check (CS_ENTITY_INVISIBLEMESH)))
        data->viscallback->ObjectVisible (visobj_wrap->visobj, mesh, 0);
    }
  }

  csKDTree* child1 = treenode->GetChild1 ();
  if (child1) CallVisibilityCallbacksForSubtree (child1, data, cur_timestamp);
  csKDTree* child2 = treenode->GetChild2 ();
  if (child2) CallVisibilityCallbacksForSubtree (child2, data, cur_timestamp);

}

void csPVSVis::FrustTest_Traverse (csKDTree* treenode,
	FrustTest_Front2BackData* data,
	uint32 cur_timestamp, uint32 frustum_mask)
{
  // In the first part of this test we are going to test if the node
  // itself is visible. If it is not then we don't need to continue.
  int nodevis = TestNodeVisibility (treenode, data, frustum_mask);
  if (nodevis == NODE_INVISIBLE)
    return;

  if (nodevis == NODE_VISIBLE && frustum_mask == 0)
  {
    // Special case. The node is visible and the frustum mask is 0.
    // This means that the node is completely visible and it doesn't
    // make sense to continue testing visibility. However we need
    // to call the callback on all visible objects. So we traverse the
    // tree manually from this point on. To stop the Front2Back traversal
    // we return false here.
    CallVisibilityCallbacksForSubtree (treenode, data, cur_timestamp);
    return;
  }

  treenode->Distribute ();

  int num_objects;
  csKDTreeChild** objects;
  num_objects = treenode->GetObjectCount ();
  objects = treenode->GetObjects ();
  int i;
  for (i = 0 ; i < num_objects ; i++)
  {
    if (objects[i]->timestamp != cur_timestamp)
    {
      objects[i]->timestamp = cur_timestamp;
      csPVSVisObjectWrapper* visobj_wrap = (csPVSVisObjectWrapper*)
      	objects[i]->GetObject ();
      TestObjectVisibility (visobj_wrap, data, frustum_mask);
    }
  }

  csKDTree* child1 = treenode->GetChild1 ();
  if (child1) FrustTest_Traverse (child1, data, cur_timestamp, frustum_mask);
  csKDTree* child2 = treenode->GetChild2 ();
  if (child2) FrustTest_Traverse (child2, data, cur_timestamp, frustum_mask);

  return;
}

bool csPVSVis::VisTest (iRenderView* rview, 
                            iVisibilityCullerListener* viscallback)
{
  // We update the objects before testing the callback so that
  // we can use this VisTest() call to make sure the objects in the
  // culler are precached.
  UpdateObjects ();
  current_vistest_nr++;

  // just make sure we have a callback
  if (viscallback == 0)
    return false;

  // Data for the vis tester.
  FrustTest_Front2BackData data;

  // First get the current view frustum from the rview.
  csRenderContext* ctxt = rview->GetRenderContext ();
  data.frustum = ctxt->clip_planes;
  uint32 frustum_mask = ctxt->clip_planes_mask;

  // The big routine: traverse from front to back and mark all objects
  // visible that are visible.
  data.pos = rview->GetCamera ()->GetTransform ().GetOrigin ();
  data.rview = rview;
  data.viscallback = viscallback;
  FrustTest_Traverse (kdtree, &data, kdtree->NewTraversal (), frustum_mask);

  return true;
}

//======== VisTest planes ==================================================

struct FrustTestPlanes_Front2BackData
{

  uint32 current_vistest_nr;
  csArray<iVisibilityObject*>* vistest_objects;

  // During VisTest() we use the current frustum as five planes.
  // Associated with this frustum we also have a clip mask which
  // is maintained recursively during VisTest() and indicates the
  // planes that are still active for the current kd-tree node.
  csPlane3* frustum;

  iVisibilityCullerListener* viscallback;
};

static bool FrustTestPlanes_Front2Back (csKDTree* treenode,
	void* userdata, uint32 cur_timestamp, uint32& frustum_mask)
{
  FrustTestPlanes_Front2BackData* data
  	= (FrustTestPlanes_Front2BackData*)userdata;

  // In the first part of this test we are going to test if the node
  // itself is visible. If it is not then we don't need to continue.
  const csBox3& node_bbox = treenode->GetNodeBBox ();
  uint32 new_mask;
  if (!csIntersect3::BoxFrustum (node_bbox, data->frustum, frustum_mask,
  	new_mask))
  {
    return false;
  }

  frustum_mask = new_mask;

  treenode->Distribute ();

  int num_objects;
  csKDTreeChild** objects;
  num_objects = treenode->GetObjectCount ();
  objects = treenode->GetObjects ();
  int i;
  for (i = 0 ; i < num_objects ; i++)
  {
    if (objects[i]->timestamp != cur_timestamp)
    {
      objects[i]->timestamp = cur_timestamp;
      csPVSVisObjectWrapper* visobj_wrap = (csPVSVisObjectWrapper*)
      	objects[i]->GetObject ();
      const csBox3& obj_bbox = visobj_wrap->child->GetBBox ();
      uint32 new_mask2;
      if (csIntersect3::BoxFrustum (obj_bbox, data->frustum,
		frustum_mask, new_mask2))
      {
	if (data->viscallback)
	{
	  data->viscallback->ObjectVisible (visobj_wrap->visobj, 
	      visobj_wrap->mesh, new_mask2);
	}
	else
	{
	  data->vistest_objects->Push (visobj_wrap->visobj);
	}
      }
    }
  }

  return true;
}

csPtr<iVisibilityObjectIterator> csPVSVis::VisTest (csPlane3* planes,
	int num_planes)
{
  UpdateObjects ();
  current_vistest_nr++;

  csArray<iVisibilityObject*>* v;
  if (vistest_objects_inuse)
  {
    // Vector is already in use by another iterator. Allocate a new vector
    // here.
    v = new csArray<iVisibilityObject*> ();
  }
  else
  {
    v = &vistest_objects;
    vistest_objects.Empty ();
  }
  
  FrustTestPlanes_Front2BackData data;
  data.current_vistest_nr = current_vistest_nr;
  data.vistest_objects = v;
  data.frustum = planes;
  data.viscallback = 0;
  uint32 frustum_mask = (1 << num_planes)-1;

  kdtree->TraverseRandom (FrustTestPlanes_Front2Back,
  	(void*)&data, frustum_mask);

  csPVSVisObjIt* vobjit = new csPVSVisObjIt (v,
  	vistest_objects_inuse ? 0 : &vistest_objects_inuse);
  return csPtr<iVisibilityObjectIterator> (vobjit);
}

void csPVSVis::VisTest (csPlane3* planes,
	int num_planes, iVisibilityCullerListener* viscallback)
{
  UpdateObjects ();
  current_vistest_nr++;

  FrustTestPlanes_Front2BackData data;
  data.current_vistest_nr = current_vistest_nr;
  data.frustum = planes;
  data.viscallback = viscallback;
  uint32 frustum_mask = (1 << num_planes)-1;

  kdtree->TraverseRandom (FrustTestPlanes_Front2Back,
  	(void*)&data, frustum_mask);
}

//======== VisTest box =====================================================

struct FrustTestBox_Front2BackData
{
  uint32 current_vistest_nr;
  csBox3 box;
  csArray<iVisibilityObject*>* vistest_objects;
};

static bool FrustTestBox_Front2Back (csKDTree* treenode, void* userdata,
	uint32 cur_timestamp, uint32&)
{
  FrustTestBox_Front2BackData* data = (FrustTestBox_Front2BackData*)userdata;

  // In the first part of this test we are going to test if the
  // box vector intersects with the node. If not then we don't
  // need to continue.
  const csBox3& node_bbox = treenode->GetNodeBBox ();
  if (!node_bbox.TestIntersect (data->box))
  {
    return false;
  }

  treenode->Distribute ();

  int num_objects;
  csKDTreeChild** objects;
  num_objects = treenode->GetObjectCount ();
  objects = treenode->GetObjects ();

  int i;
  for (i = 0 ; i < num_objects ; i++)
  {
    if (objects[i]->timestamp != cur_timestamp)
    {
      objects[i]->timestamp = cur_timestamp;
      csPVSVisObjectWrapper* visobj_wrap = (csPVSVisObjectWrapper*)
      	objects[i]->GetObject ();

      // Test the bounding box of the object.
      const csBox3& obj_bbox = visobj_wrap->child->GetBBox ();
      if (obj_bbox.TestIntersect (data->box))
      {
	data->vistest_objects->Push (visobj_wrap->visobj);
      }
    }
  }

  return true;
}

csPtr<iVisibilityObjectIterator> csPVSVis::VisTest (const csBox3& box)
{
  UpdateObjects ();
  current_vistest_nr++;

  csArray<iVisibilityObject*>* v;
  if (vistest_objects_inuse)
  {
    // Vector is already in use by another iterator. Allocate a new vector
    // here.
    v = new csArray<iVisibilityObject*> ();
  }
  else
  {
    v = &vistest_objects;
    vistest_objects.Empty ();
  }
  
  FrustTestBox_Front2BackData data;
  data.current_vistest_nr = current_vistest_nr;
  data.box = box;
  data.vistest_objects = v;
  kdtree->Front2Back (box.GetCenter (), FrustTestBox_Front2Back, (void*)&data,
  	0);

  csPVSVisObjIt* vobjit = new csPVSVisObjIt (v,
  	vistest_objects_inuse ? 0 : &vistest_objects_inuse);
  return csPtr<iVisibilityObjectIterator> (vobjit);
}

//======== VisTest sphere ==================================================

struct FrustTestSphere_Front2BackData
{
  uint32 current_vistest_nr;
  csVector3 pos;
  float sqradius;
  csArray<iVisibilityObject*>* vistest_objects;

  iVisibilityCullerListener* viscallback;
};

static bool FrustTestSphere_Front2Back (csKDTree* treenode,
	void* userdata, uint32 cur_timestamp, uint32&)
{
  FrustTestSphere_Front2BackData* data =
  	(FrustTestSphere_Front2BackData*)userdata;

  // In the first part of this test we are going to test if the
  // box vector intersects with the node. If not then we don't
  // need to continue.
  const csBox3& node_bbox = treenode->GetNodeBBox ();
  if (!csIntersect3::BoxSphere (node_bbox, data->pos, data->sqradius))
  {
    return false;
  }

  treenode->Distribute ();

  int num_objects;
  csKDTreeChild** objects;
  num_objects = treenode->GetObjectCount ();
  objects = treenode->GetObjects ();

  int i;
  for (i = 0 ; i < num_objects ; i++)
  {
    if (objects[i]->timestamp != cur_timestamp)
    {
      objects[i]->timestamp = cur_timestamp;
      csPVSVisObjectWrapper* visobj_wrap = (csPVSVisObjectWrapper*)
      	objects[i]->GetObject ();

      // Test the bounding box of the object.
      const csBox3& obj_bbox = visobj_wrap->child->GetBBox ();
      if (csIntersect3::BoxSphere (obj_bbox, data->pos, data->sqradius))
      {
	if (data->viscallback)
	{
	  data->viscallback->ObjectVisible (
	    visobj_wrap->visobj, visobj_wrap->mesh, 0xff);
	}
	else
	{
	  data->vistest_objects->Push (visobj_wrap->visobj);
	}
      }
    }
  }

  return true;
}

csPtr<iVisibilityObjectIterator> csPVSVis::VisTest (const csSphere& sphere)
{
  UpdateObjects ();
  current_vistest_nr++;

  csArray<iVisibilityObject*>* v;
  if (vistest_objects_inuse)
  {
    // Vector is already in use by another iterator. Allocate a new vector
    // here.
    v = new csArray<iVisibilityObject*> ();
  }
  else
  {
    v = &vistest_objects;
    vistest_objects.Empty ();
  }

  FrustTestSphere_Front2BackData data;
  data.current_vistest_nr = current_vistest_nr;
  data.pos = sphere.GetCenter ();
  data.sqradius = sphere.GetRadius () * sphere.GetRadius ();
  data.vistest_objects = v;
  data.viscallback = 0;
  kdtree->Front2Back (data.pos, FrustTestSphere_Front2Back, (void*)&data,
  	0);

  csPVSVisObjIt* vobjit = new csPVSVisObjIt (v,
  	vistest_objects_inuse ? 0 : &vistest_objects_inuse);
  return csPtr<iVisibilityObjectIterator> (vobjit);
}

void csPVSVis::VisTest (const csSphere& sphere, 
			    iVisibilityCullerListener* viscallback)
{
  UpdateObjects ();
  current_vistest_nr++;

  FrustTestSphere_Front2BackData data;
  data.current_vistest_nr = current_vistest_nr;
  data.pos = sphere.GetCenter ();
  data.sqradius = sphere.GetRadius () * sphere.GetRadius ();
  data.viscallback = viscallback;
  kdtree->Front2Back (data.pos, FrustTestSphere_Front2Back, (void*)&data,
  	0);
}

//======== IntersectSegment ================================================

struct IntersectSegment_Front2BackData
{
  csSegment3 seg;
  csVector3 isect;
  float sqdist;		// Squared distance between seg.start and isect.
  float r;
  iMeshWrapper* mesh;
  int polygon_idx;
  csArray<iVisibilityObject*>* vector;	// If not-null we need all objects.
  bool accurate;
};

static bool IntersectSegmentSloppy_Front2Back (csKDTree* treenode,
	void* userdata, uint32 cur_timestamp, uint32&)
{
  IntersectSegment_Front2BackData* data
  	= (IntersectSegment_Front2BackData*)userdata;

  const csBox3& node_bbox = treenode->GetNodeBBox ();

  // In the first part of this test we are going to test if the
  // start-end vector intersects with the node. If not then we don't
  // need to continue.
  csVector3 box_isect;
  if (csIntersect3::BoxSegment (node_bbox, data->seg, box_isect) == -1)
  {
    return false;
  }

  treenode->Distribute ();

  int num_objects;
  csKDTreeChild** objects;
  num_objects = treenode->GetObjectCount ();
  objects = treenode->GetObjects ();

  int i;
  for (i = 0 ; i < num_objects ; i++)
  {
    if (objects[i]->timestamp != cur_timestamp)
    {
      objects[i]->timestamp = cur_timestamp;
      csPVSVisObjectWrapper* visobj_wrap = (csPVSVisObjectWrapper*)
      	objects[i]->GetObject ();

      // First test the bounding box of the object.
      const csBox3& obj_bbox = visobj_wrap->child->GetBBox ();

      if (csIntersect3::BoxSegment (obj_bbox, data->seg, box_isect) != -1)
      {
        // This object is possibly intersected by this beam.
	if (visobj_wrap->mesh)
	  if (!visobj_wrap->mesh->GetFlags ().Check (CS_ENTITY_NOHITBEAM))
	    data->vector->Push (visobj_wrap->visobj);
      }
    }
  }
  return true;
}

static bool IntersectSegment_Front2Back (csKDTree* treenode,
	void* userdata, uint32 cur_timestamp, uint32&)
{
  IntersectSegment_Front2BackData* data
  	= (IntersectSegment_Front2BackData*)userdata;

  const csBox3& node_bbox = treenode->GetNodeBBox ();

  // If mesh != 0 then we have already found our mesh. In that
  // case we will compare the distance of the origin with the the
  // box of the treenode and the already found shortest distance to
  // see if we have to proceed.
  if (data->mesh)
  {
    csBox3 b (node_bbox.Min ()-data->seg.Start (),
    	      node_bbox.Max ()-data->seg.Start ());
    if (b.SquaredOriginDist () > data->sqdist) return false;
  }

  // In the first part of this test we are going to test if the
  // start-end vector intersects with the node. If not then we don't
  // need to continue.
  csVector3 box_isect;
  if (csIntersect3::BoxSegment (node_bbox, data->seg, box_isect) == -1)
  {
    return false;
  }

  treenode->Distribute ();

  int num_objects;
  csKDTreeChild** objects;
  num_objects = treenode->GetObjectCount ();
  objects = treenode->GetObjects ();

  int i;
  for (i = 0 ; i < num_objects ; i++)
  {
    if (objects[i]->timestamp != cur_timestamp)
    {
      objects[i]->timestamp = cur_timestamp;
      csPVSVisObjectWrapper* visobj_wrap = (csPVSVisObjectWrapper*)
      	objects[i]->GetObject ();

      // First test the bounding box of the object.
      const csBox3& obj_bbox = visobj_wrap->child->GetBBox ();

      if (csIntersect3::BoxSegment (obj_bbox, data->seg, box_isect) != -1)
      {
        // This object is possibly intersected by this beam.
	if (visobj_wrap->mesh)
	{
	  if (!visobj_wrap->mesh->GetFlags ().Check (CS_ENTITY_NOHITBEAM))
	  {
	    // Transform our vector to object space.
	    csVector3 obj_start;
	    csVector3 obj_end;
	    iMovable* movable = visobj_wrap->visobj->GetMovable ();
	    bool identity = movable->IsFullTransformIdentity ();
	    csReversibleTransform movtrans;
	    if (identity)
	    {
	      obj_start = data->seg.Start ();
	      obj_end = data->seg.End ();
	    }
	    else
	    {
	      movtrans = movable->GetFullTransform ();
	      obj_start = movtrans.Other2This (data->seg.Start ());
	      obj_end = movtrans.Other2This (data->seg.End ());
	    }
	    csVector3 obj_isect;
	    float r;

	    bool rc;
	    int pidx = -1;
	    if (data->accurate)
	      rc = visobj_wrap->mesh->GetMeshObject ()->HitBeamObject (
	    	  obj_start, obj_end, obj_isect, &r, &pidx);
	    else
	      rc = visobj_wrap->mesh->GetMeshObject ()->HitBeamOutline (
	    	  obj_start, obj_end, obj_isect, &r);
	    if (rc)
	    {
	      if (data->vector)
	      {
		data->vector->Push (visobj_wrap->visobj);
	      }
	      else if (r < data->r)
	      {
		data->r = r;
		data->polygon_idx = pidx;
		if (identity)
		  data->isect = obj_isect;
		else
		  data->isect = movtrans.This2Other (obj_isect);
		data->sqdist = csSquaredDist::PointPoint (
			data->seg.Start (), data->isect);
		data->mesh = visobj_wrap->mesh;
	      }
	    }
	  }
	}
      }
    }
  }
  return true;
}

bool csPVSVis::IntersectSegment (const csVector3& start,
    const csVector3& end, csVector3& isect, float* pr,
    iMeshWrapper** p_mesh, int* poly_idx, bool accurate)
{
  UpdateObjects ();
  current_vistest_nr++;
  IntersectSegment_Front2BackData data;
  data.seg.Set (start, end);
  data.sqdist = 10000000000.0;
  data.isect.Set (0, 0, 0);
  data.r = 10000000000.;
  data.mesh = 0;
  data.polygon_idx = -1;
  data.vector = 0;
  data.accurate = accurate;
  data.isect = 0;
  kdtree->Front2Back (start, IntersectSegment_Front2Back, (void*)&data, 0);

  if (p_mesh) *p_mesh = data.mesh;
  if (pr) *pr = data.r;
  if (poly_idx) *poly_idx = data.polygon_idx;
  isect = data.isect;

  return data.mesh != 0;
}

csPtr<iVisibilityObjectIterator> csPVSVis::IntersectSegment (
    const csVector3& start, const csVector3& end, bool accurate)
{
  UpdateObjects ();
  current_vistest_nr++;
  IntersectSegment_Front2BackData data;
  data.seg.Set (start, end);
  data.sqdist = 10000000000.0;
  data.r = 10000000000.;
  data.mesh = 0;
  data.polygon_idx = -1;
  data.vector = new csArray<iVisibilityObject*> ();
  data.accurate = accurate;
  kdtree->Front2Back (start, IntersectSegment_Front2Back, (void*)&data, 0);

  csPVSVisObjIt* vobjit = new csPVSVisObjIt (data.vector, 0);
  return csPtr<iVisibilityObjectIterator> (vobjit);
}

csPtr<iVisibilityObjectIterator> csPVSVis::IntersectSegmentSloppy (
    const csVector3& start, const csVector3& end)
{
  UpdateObjects ();
  current_vistest_nr++;
  IntersectSegment_Front2BackData data;
  data.seg.Set (start, end);
  data.vector = new csArray<iVisibilityObject*> ();
  kdtree->Front2Back (start, IntersectSegmentSloppy_Front2Back,
  	(void*)&data, 0);

  csPVSVisObjIt* vobjit = new csPVSVisObjIt (data.vector, 0);
  return csPtr<iVisibilityObjectIterator> (vobjit);
}

//======== CastShadows =====================================================

struct ShadObj
{
  float sqdist;
  iMeshWrapper* mesh;
  iShadowCaster* caster;
  iMovable* movable;
};

struct CastShadows_Front2BackData
{
  uint32 current_vistest_nr;
  iFrustumView* fview;
  csPlane3 planes[32];
  ShadObj* shadobjs;
  int num_shadobjs;
};

static int compare_shadobj (const void* el1, const void* el2)
{
  ShadObj* m1 = (ShadObj*)el1;
  ShadObj* m2 = (ShadObj*)el2;
  if (m1->sqdist < m2->sqdist) return -1;
  if (m1->sqdist > m2->sqdist) return 1;
  return 0;
}

static bool CastShadows_Front2Back (csKDTree* treenode, void* userdata,
	uint32 cur_timestamp, uint32& planes_mask)
{
  CastShadows_Front2BackData* data = (CastShadows_Front2BackData*)userdata;

  iFrustumView* fview = data->fview;
  const csVector3& center = fview->GetFrustumContext ()->GetLightFrustum ()
    ->GetOrigin ();
  float sqrad = fview->GetSquaredRadius ();

  // First check the distance between the origin and the node box and see
  // if we are within the radius.
  const csBox3& node_bbox = treenode->GetNodeBBox ();
  csBox3 b (node_bbox.Min ()-center, node_bbox.Max ()-center);
  if (b.SquaredOriginDist () > sqrad)
    return false;

  // First we do frustum checking if relevant. See if the current node
  // intersects with the frustum.
  if (planes_mask)
  {
    uint32 out_mask;
    if (!csIntersect3::BoxFrustum (node_bbox, data->planes, planes_mask,
    	out_mask))
      return false;
    planes_mask = out_mask;
  }

  treenode->Distribute ();

  int num_objects;
  csKDTreeChild** objects;
  num_objects = treenode->GetObjectCount ();
  objects = treenode->GetObjects ();

  int i;
  for (i = 0 ; i < num_objects ; i++)
  {
    if (objects[i]->timestamp != cur_timestamp)
    {
      objects[i]->timestamp = cur_timestamp;
      csPVSVisObjectWrapper* visobj_wrap = (csPVSVisObjectWrapper*)
      	objects[i]->GetObject ();
      const csBox3& obj_bbox = visobj_wrap->child->GetBBox ();
      csBox3 b (obj_bbox.Min ()-center, obj_bbox.Max ()-center);
      if (b.SquaredOriginDist () > sqrad)
	continue;

      if (visobj_wrap->caster &&
            fview->CheckShadowMask (visobj_wrap->mesh->GetFlags ().Get ()))
      {
        data->shadobjs[data->num_shadobjs].sqdist = b.SquaredOriginDist ();
	data->shadobjs[data->num_shadobjs].caster = visobj_wrap->caster;
	data->shadobjs[data->num_shadobjs].mesh = 0;
	data->shadobjs[data->num_shadobjs].movable =
		visobj_wrap->visobj->GetMovable ();
	data->num_shadobjs++;
      }
      if (fview->CheckProcessMask (visobj_wrap->mesh->GetFlags ().Get ()))
      {
        data->shadobjs[data->num_shadobjs].sqdist = b.SquaredOriginMaxDist ();
	data->shadobjs[data->num_shadobjs].mesh = visobj_wrap->mesh;
	data->shadobjs[data->num_shadobjs].caster = 0;
	data->shadobjs[data->num_shadobjs].movable =
		visobj_wrap->visobj->GetMovable ();
	data->num_shadobjs++;
      }
    }
  }

  return true;
}


void csPVSVis::CastShadows (iFrustumView* fview)
{
  UpdateObjects ();
  current_vistest_nr++;
  CastShadows_Front2BackData data;
  data.current_vistest_nr = current_vistest_nr;
  data.fview = fview;

  const csVector3& center = fview->GetFrustumContext ()->GetLightFrustum ()
    ->GetOrigin ();

  //======================================
  // First we find all relevant objects. For all these objects we add
  // both the shadow-caster as the receiver (as mesh) to the array (as
  // two different entries). The caster is added with the distance from
  // the light position to the nearest point on the bounding box while
  // the receiver is added with the distance from the light position to
  // the farthest point on the bounding box. Later on we can then traverse
  // the resulting list so that all relevant shadow casters are added before
  // the receivers are processed.
  //======================================

  data.shadobjs = new ShadObj [visobj_vector.Length () * 2];
  data.num_shadobjs = 0;

  // First check if we need to do frustum clipping.
  csFrustum* lf = fview->GetFrustumContext ()->GetLightFrustum ();
  uint32 planes_mask = 0;
  int i;

  // Traverse the kd-tree to find all relevant objects.
  // @@@ What if the frustum is bigger???
  CS_ASSERT (lf->GetVertexCount () <= 31);
  if (lf->GetVertexCount () > 31)
  {
    csPrintf ("INTERNAL ERROR! #vertices in GetVisibleObjects() exceeded!\n");
    fflush (stdout);
    return;
  }
  int i1 = lf->GetVertexCount () - 1;
  for (i = 0 ; i < lf->GetVertexCount () ; i1 = i, i++)
  {
    planes_mask = (planes_mask<<1)|1;
    const csVector3 &v1 = lf->GetVertex (i);
    const csVector3 &v2 = lf->GetVertex (i1);
    data.planes[i].Set (center, v1+center, v2+center);
  }
  if (lf->GetBackPlane ())
  {
    // @@@ UNTESTED CODE! There are no backplanes yet in frustums.
    // It is possible this plane has to be inverted.
    planes_mask = (planes_mask<<1)|1;
    data.planes[i] = *(lf->GetBackPlane ());
  }

  kdtree->Front2Back (center, CastShadows_Front2Back, (void*)&data,
  	planes_mask);

  // Now sort the list of shadow objects on radius.
  qsort (data.shadobjs, data.num_shadobjs, sizeof (ShadObj), compare_shadobj);

  // Mark a new region so that we can restore the shadows later.
  iShadowBlockList *shadows = fview->GetFrustumContext ()->GetShadows ();
  uint32 prev_region = shadows->MarkNewRegion ();

  // Now scan all objects and cast and receive shadows as appropriate. 
  ShadObj* so = data.shadobjs;
  for (i = 0 ; i < data.num_shadobjs ; i++)
  {
    if (so->caster) so->caster->AppendShadows (so->movable, shadows, center);
    if (so->mesh) fview->CallObjectFunction (so->mesh, true);
    so++;
  }
  delete[] data.shadobjs;

  // Restore the shadow list in 'fview' and then delete
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
}
