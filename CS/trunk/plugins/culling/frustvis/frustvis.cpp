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

#include <string.h>
#include "cssysdef.h"
#include "csver.h"
#include "cssys/sysfunc.h"
#include "csutil/scf.h"
#include "csutil/util.h"
#include "csutil/scfstr.h"
#include "csgeom/frustum.h"
#include "csgeom/matrix3.h"
#include "csgeom/math3d.h"
#include "csgeom/obb.h"
#include "csgeom/segment.h"
#include "csgeom/sphere.h"
#include "csgeom/kdtree.h"
#include "igeom/polymesh.h"
#include "igeom/objmodel.h"
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
#include "imesh/thing/thing.h"
#include "iutil/object.h"
#include "ivaria/reporter.h"
#include "frustvis.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (csFrustumVis)

SCF_EXPORT_CLASS_TABLE (frustvis)
  SCF_EXPORT_CLASS (csFrustumVis, "crystalspace.culling.frustvis",
    "Simple Frustum Visibility System")
SCF_EXPORT_CLASS_TABLE_END

SCF_IMPLEMENT_IBASE (csFrustumVis)
  SCF_IMPLEMENTS_INTERFACE (iVisibilityCuller)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csFrustumVis::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

//----------------------------------------------------------------------

class csFrustVisObjIt : public iVisibilityObjectIterator
{
private:
  csVector* vector;
  int position;
  bool* vistest_objects_inuse;

public:
  SCF_DECLARE_IBASE;

  csFrustVisObjIt (csVector* vector, bool* vistest_objects_inuse)
  {
    SCF_CONSTRUCT_IBASE (0);
    csFrustVisObjIt::vector = vector;
    csFrustVisObjIt::vistest_objects_inuse = vistest_objects_inuse;
    if (vistest_objects_inuse) *vistest_objects_inuse = true;
    Reset ();
  }
  virtual ~csFrustVisObjIt ()
  {
    // If the vistest_objects_inuse pointer is not 0 we set the
    // bool to false to indicate we're no longer using the base
    // vector. Otherwise we delete the vector.
    if (vistest_objects_inuse) *vistest_objects_inuse = false;
    else delete vector;
  }

  virtual bool Next()
  {
    if (position < 0) return false;
    position++;
    if (position == vector->Length ())
    {
      position = -1;
      return false;
    }
    return true;
  }

  virtual void Reset()
  {
    if (vector == 0 || vector->Length () < 1)
      position = -1;
    else
      position = 0;
  }

  virtual iVisibilityObject* GetObject () const
  {
    return (iVisibilityObject*)(vector->Get (position));
  }
  virtual bool IsFinished () const
  {
    return (position < 0);
  }
};

SCF_IMPLEMENT_IBASE (csFrustVisObjIt)
  SCF_IMPLEMENTS_INTERFACE (iVisibilityObjectIterator)
SCF_IMPLEMENT_IBASE_END

//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csFrustVisObjectWrapper)
  SCF_IMPLEMENTS_INTERFACE (iObjectModelListener)
  SCF_IMPLEMENTS_INTERFACE (iMovableListener)
SCF_IMPLEMENT_IBASE_END

void csFrustVisObjectWrapper::ObjectModelChanged (iObjectModel* /*model*/)
{
  frustvis->AddObjectToUpdateQueue (this);
}

void csFrustVisObjectWrapper::MovableChanged (iMovable* /*movable*/)
{
  frustvis->AddObjectToUpdateQueue (this);
}

//----------------------------------------------------------------------

csFrustumVis::csFrustumVis (iBase *iParent)
{
  SCF_CONSTRUCT_IBASE (iParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent);
  object_reg = 0;
  kdtree = 0;
  current_visnr = 1;
  vistest_objects_inuse = false;
  updating = false;
}

csFrustumVis::~csFrustumVis ()
{
  while (visobj_vector.Length () > 0)
  {
    csFrustVisObjectWrapper* visobj_wrap = (csFrustVisObjectWrapper*)
    	visobj_vector[0];
    iVisibilityObject* visobj = visobj_wrap->visobj;
    visobj->GetObjectModel ()->RemoveListener (
		      (iObjectModelListener*)visobj_wrap);
    iMovable* movable = visobj->GetMovable ();
    movable->RemoveListener ((iMovableListener*)visobj_wrap);
    kdtree->RemoveObject (visobj_wrap->child);
    visobj->DecRef ();
    delete visobj_wrap;
    visobj_vector.Delete (0);
  }
  delete kdtree;
}

bool csFrustumVis::Initialize (iObjectRegistry *object_reg)
{
  csFrustumVis::object_reg = object_reg;

  delete kdtree;

#ifndef CS_USE_NEW_RENDERER
  csRef<iGraphics3D> g3d (CS_QUERY_REGISTRY (object_reg, iGraphics3D));
#else
  csRef<iGraphics3D> g3d (CS_QUERY_REGISTRY (object_reg, iRender3D));
#endif // CS_USE_NEW_RENDERER
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

  kdtree = new csKDTree (0);

  return true;
}

void csFrustumVis::Setup (const char* /*name*/)
{
}

void csFrustumVis::CalculateVisObjBBox (iVisibilityObject* visobj, csBox3& bbox)
{
  iMovable* movable = visobj->GetMovable ();
  if (movable->IsFullTransformIdentity ())
  {
    visobj->GetObjectModel ()->GetObjectBoundingBox (bbox, CS_BBOX_MAX);
  }
  else
  {
    csBox3 box;
    visobj->GetObjectModel ()->GetObjectBoundingBox (box, CS_BBOX_MAX);
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

void csFrustumVis::RegisterVisObject (iVisibilityObject* visobj)
{
#ifdef CS_DEBUG
  int i;
  for (i = 0 ; i < visobj_vector.Length () ; i++)
  {
    if (((csFrustVisObjectWrapper*)visobj_vector[i])->visobj == visobj)
    {
      CS_ASSERT (false);
    }
  }
#endif
  csFrustVisObjectWrapper* visobj_wrap = new csFrustVisObjectWrapper (
		  this);
  visobj_wrap->visobj = visobj;
  visobj->IncRef ();
  iMovable* movable = visobj->GetMovable ();
  visobj_wrap->update_number = movable->GetUpdateNumber ();
  visobj_wrap->shape_number = visobj->GetObjectModel ()->GetShapeNumber ();

  csBox3 bbox;
  CalculateVisObjBBox (visobj, bbox);
  visobj_wrap->child = kdtree->AddObject (bbox, (void*)visobj_wrap);

  iMeshWrapper* mesh = visobj->GetMeshWrapper ();
  visobj_wrap->mesh = mesh;
  if (mesh)
  {
    visobj_wrap->caster = SCF_QUERY_INTERFACE (mesh->GetMeshObject (),
    	iShadowCaster);
    visobj_wrap->thing_state = SCF_QUERY_INTERFACE (mesh->GetMeshObject (),
	iThingState);
  }

  // Only add the listeners at the very last moment to prevent them to
  // be called by the calls above (i.e. especially the calculation of
  // the bounding box could cause a listener to be fired).
  movable->AddListener ((iMovableListener*)visobj_wrap);
  visobj->GetObjectModel ()->AddListener (
		  (iObjectModelListener*)visobj_wrap);

  visobj_vector.Push (visobj_wrap);
}

void csFrustumVis::UnregisterVisObject (iVisibilityObject* visobj)
{
  int i;
  for (i = 0 ; i < visobj_vector.Length () ; i++)
  {
    csFrustVisObjectWrapper* visobj_wrap = (csFrustVisObjectWrapper*)
      visobj_vector[i];
    if (visobj_wrap->visobj == visobj)
    {
      update_queue.Delete (visobj_wrap);
      visobj->GetMovable ()->RemoveListener (
		  (iMovableListener*)visobj_wrap);
      visobj->GetObjectModel ()->RemoveListener (
		  (iObjectModelListener*)visobj_wrap);
      kdtree->RemoveObject (visobj_wrap->child);
      visobj->DecRef ();
#ifdef CS_DEBUG
      // To easily recognize that the vis wrapper has been deleted:
      visobj_wrap->frustvis = (csFrustumVis*)0xdeadbeef;
#endif
      delete visobj_wrap;
      visobj_vector.Delete (i);
      return;
    }
  }
}

void csFrustumVis::AddObjectToUpdateQueue (csFrustVisObjectWrapper* visobj_wrap)
{
  if (updating) return;
  CS_ASSERT (visobj_wrap->frustvis != (csFrustumVis*)0xdeadbeef);
  update_queue.Add (visobj_wrap);
}

void csFrustumVis::UpdateObjects ()
{
  updating = true;
  {
    csGlobalHashIterator it (update_queue.GetHashMap ());
    while (it.HasNext ())
    {
      csFrustVisObjectWrapper* vw = (csFrustVisObjectWrapper*)it.Next ();
      UpdateObject (vw);
    }
  }
  update_queue.DeleteAll ();
  updating = false;
}

void csFrustumVis::UpdateObject (csFrustVisObjectWrapper* visobj_wrap)
{
  CS_ASSERT (visobj_wrap->frustvis != (csFrustumVis*)0xdeadbeef);
  iVisibilityObject* visobj = visobj_wrap->visobj;
  iMovable* movable = visobj->GetMovable ();
  csBox3 bbox;
  CalculateVisObjBBox (visobj, bbox);
  kdtree->MoveObject (visobj_wrap->child, bbox);
  visobj_wrap->shape_number = visobj->GetObjectModel ()->GetShapeNumber ();
  visobj_wrap->update_number = movable->GetUpdateNumber ();
}

struct FrustTest_Front2BackData
{
  csVector3 pos;
  iRenderView* rview;
  csFrustumVis* frustvis;

  // During VisTest() we use the current frustum as five planes.
  // Associated with this frustum we also have a clip mask which
  // is maintained recursively during VisTest() and indicates the
  // planes that are still active for the current kd-tree node.
  csPlane3 frustum[32];
};

bool csFrustumVis::TestNodeVisibility (csKDTree* treenode,
	FrustTest_Front2BackData* data, uint32& frustum_mask)
{
  const csBox3& node_bbox = treenode->GetNodeBBox ();

  if (node_bbox.Contains (data->pos))
  {
    return true;
  }

  uint32 new_mask;
  if (!csIntersect3::BoxFrustum (node_bbox, data->frustum, frustum_mask,
  	new_mask))
  {
    return false;
  }
  frustum_mask = new_mask;
  return true;
}

bool csFrustumVis::TestObjectVisibility (csFrustVisObjectWrapper* obj,
  	FrustTest_Front2BackData* data, uint32 frustum_mask)
{
  if (obj->visobj->GetVisibilityNumber () != current_visnr)
  {
    const csBox3& obj_bbox = obj->child->GetBBox ();
    if (obj_bbox.Contains (data->pos))
    {
      obj->visobj->SetVisibilityNumber (current_visnr);
      return true;
    }
  
    uint32 new_mask;
    if (!csIntersect3::BoxFrustum (obj_bbox, data->frustum,
		frustum_mask, new_mask))
    {
      return false;
    }

    obj->visobj->SetVisibilityNumber (current_visnr);
  }

  return true;
}

//======== VisTest =========================================================

static bool FrustTest_Front2Back (csKDTree* treenode, void* userdata,
	uint32 cur_timestamp, uint32& frustum_mask)
{
  FrustTest_Front2BackData* data = (FrustTest_Front2BackData*)userdata;
  csFrustumVis* frustvis = data->frustvis;

  // In the first part of this test we are going to test if the node
  // itself is visible. If it is not then we don't need to continue.
  if (!frustvis->TestNodeVisibility (treenode, data, frustum_mask))
    return false;

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
      csFrustVisObjectWrapper* visobj_wrap = (csFrustVisObjectWrapper*)
      	objects[i]->GetObject ();
      frustvis->TestObjectVisibility (visobj_wrap, data, frustum_mask);
    }
  }

  return true;
}

bool csFrustumVis::VisTest (iRenderView* rview)
{
  UpdateObjects ();
  current_visnr++;

  // Data for the vis tester.
  FrustTest_Front2BackData data;

  // First get the current view frustum from the rview.
  csRenderContext* ctxt = rview->GetRenderContext ();
  csVector3* frust = ctxt->iview_frustum->frustum;

  const csReversibleTransform& trans = rview->GetCamera ()->GetTransform ();
  csVector3 o2tmult = trans.GetO2T () * trans.GetO2TTranslation ();
  data.frustum[0].Set (trans.GetT2O () * frust[0], - frust[0] * o2tmult);
  data.frustum[1].Set (trans.GetT2O () * frust[1], - frust[1] * o2tmult);
  data.frustum[2].Set (trans.GetT2O () * frust[2], - frust[2] * o2tmult);
  data.frustum[3].Set (trans.GetT2O () * frust[3], - frust[3] * o2tmult);
  // @@@ DO z=0 plane too!

  // The big routine: traverse from front to back and mark all objects
  // visible that are visible.
  data.pos = rview->GetCamera ()->GetTransform ().GetOrigin ();
  data.rview = rview;
  data.frustvis = this;
  kdtree->Front2Back (data.pos, FrustTest_Front2Back, (void*)&data,
  	0xf);	// 0xf == frustum_mask for four planes.

  return true;
}

//======== VisTest planes ==================================================

struct FrustTestPlanes_Front2BackData
{
  uint32 current_visnr;
  csVector* vistest_objects;
  // During VisTest() we use the current frustum as five planes.
  // Associated with this frustum we also have a clip mask which
  // is maintained recursively during VisTest() and indicates the
  // planes that are still active for the current kd-tree node.
  csPlane3* frustum;
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
      csFrustVisObjectWrapper* visobj_wrap = (csFrustVisObjectWrapper*)
      	objects[i]->GetObject ();
      if (visobj_wrap->visobj->GetVisibilityNumber () != data->current_visnr)
      {
	const csBox3& obj_bbox = visobj_wrap->child->GetBBox ();
	uint32 new_mask2;
	if (csIntersect3::BoxFrustum (obj_bbox, data->frustum,
		frustum_mask, new_mask2))
	{
	  visobj_wrap->visobj->SetVisibilityNumber (data->current_visnr);
	  data->vistest_objects->Push (visobj_wrap->visobj);
	}
      }
    }
  }

  return true;
}

csPtr<iVisibilityObjectIterator> csFrustumVis::VisTest (csPlane3* planes,
	int num_planes)
{
  UpdateObjects ();
  current_visnr++;

  csVector* v;
  if (vistest_objects_inuse)
  {
    // Vector is already in use by another iterator. Allocate a new vector
    // here.
    v = new csVector ();
  }
  else
  {
    v = &vistest_objects;
    vistest_objects.DeleteAll ();
  }
  
  FrustTestPlanes_Front2BackData data;
  data.current_visnr = current_visnr;
  data.vistest_objects = v;
  data.frustum = planes;
  uint32 frustum_mask = (1 << num_planes)-1;

  kdtree->Front2Back (csVector3 (0, 0, 0), FrustTestPlanes_Front2Back,
  	(void*)&data, frustum_mask);

  csFrustVisObjIt* vobjit = new csFrustVisObjIt (v,
  	vistest_objects_inuse ? 0 : &vistest_objects_inuse);
  return csPtr<iVisibilityObjectIterator> (vobjit);
}

//======== VisTest box =====================================================

struct FrustTestBox_Front2BackData
{
  uint32 current_visnr;
  csBox3 box;
  csVector* vistest_objects;
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
      csFrustVisObjectWrapper* visobj_wrap = (csFrustVisObjectWrapper*)
      	objects[i]->GetObject ();

      // Test the bounding box of the object.
      const csBox3& obj_bbox = visobj_wrap->child->GetBBox ();
      if (obj_bbox.TestIntersect (data->box))
      {
	visobj_wrap->visobj->SetVisibilityNumber (data->current_visnr);
	data->vistest_objects->Push (visobj_wrap->visobj);
      }
    }
  }

  return true;
}

csPtr<iVisibilityObjectIterator> csFrustumVis::VisTest (const csBox3& box)
{
  UpdateObjects ();
  current_visnr++;

  csVector* v;
  if (vistest_objects_inuse)
  {
    // Vector is already in use by another iterator. Allocate a new vector
    // here.
    v = new csVector ();
  }
  else
  {
    v = &vistest_objects;
    vistest_objects.DeleteAll ();
  }
  
  FrustTestBox_Front2BackData data;
  data.current_visnr = current_visnr;
  data.box = box;
  data.vistest_objects = v;
  kdtree->Front2Back (box.GetCenter (), FrustTestBox_Front2Back, (void*)&data,
  	0);

  csFrustVisObjIt* vobjit = new csFrustVisObjIt (v,
  	vistest_objects_inuse ? 0 : &vistest_objects_inuse);
  return csPtr<iVisibilityObjectIterator> (vobjit);
}

//======== VisTest sphere ==================================================

struct FrustTestSphere_Front2BackData
{
  uint32 current_visnr;
  csVector3 pos;
  float sqradius;
  csVector* vistest_objects;
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
      csFrustVisObjectWrapper* visobj_wrap = (csFrustVisObjectWrapper*)
      	objects[i]->GetObject ();

      // Test the bounding box of the object.
      const csBox3& obj_bbox = visobj_wrap->child->GetBBox ();
      if (csIntersect3::BoxSphere (obj_bbox, data->pos, data->sqradius))
      {
	visobj_wrap->visobj->SetVisibilityNumber (data->current_visnr);
	data->vistest_objects->Push (visobj_wrap->visobj);
      }
    }
  }

  return true;
}

csPtr<iVisibilityObjectIterator> csFrustumVis::VisTest (const csSphere& sphere)
{
  UpdateObjects ();
  current_visnr++;

  csVector* v;
  if (vistest_objects_inuse)
  {
    // Vector is already in use by another iterator. Allocate a new vector
    // here.
    v = new csVector ();
  }
  else
  {
    v = &vistest_objects;
    vistest_objects.DeleteAll ();
  }

  FrustTestSphere_Front2BackData data;
  data.current_visnr = current_visnr;
  data.pos = sphere.GetCenter ();
  data.sqradius = sphere.GetRadius () * sphere.GetRadius ();
  data.vistest_objects = v;
  kdtree->Front2Back (data.pos, FrustTestSphere_Front2Back, (void*)&data,
  	0);

  csFrustVisObjIt* vobjit = new csFrustVisObjIt (v,
  	vistest_objects_inuse ? 0 : &vistest_objects_inuse);
  return csPtr<iVisibilityObjectIterator> (vobjit);
}

//======== IntersectSegment ================================================

struct IntersectSegment_Front2BackData
{
  csSegment3 seg;
  csVector3 isect;
  float sqdist;		// Squared distance between seg.start and isect.
  float r;
  iMeshWrapper* mesh;
  iPolygon3D* polygon;
  csVector* vector;	// If not-null we need all objects.
  bool accurate;
};

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
      csFrustVisObjectWrapper* visobj_wrap = (csFrustVisObjectWrapper*)
      	objects[i]->GetObject ();

      // First test the bounding box of the object.
      const csBox3& obj_bbox = visobj_wrap->child->GetBBox ();

      if (csIntersect3::BoxSegment (obj_bbox, data->seg, box_isect) != -1)
      {
        // This object is possibly intersected by this beam.
	if (visobj_wrap->mesh)
	{
	  if (!visobj_wrap->mesh->GetFlags ().Check (CS_ENTITY_INVISIBLE))
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

	    if (!data->vector && visobj_wrap->thing_state)
	    {
	      iThingState* st = visobj_wrap->thing_state;
	      iPolygon3D* p = st->IntersectSegment (
			obj_start, obj_end,
			obj_isect, &r, false);
	      if (p && r < data->r)
	      {
		data->r = r;
		data->polygon = p;
		if (identity)
		  data->isect = obj_isect;
		else
		  data->isect = movtrans.This2Other (obj_isect);
		data->sqdist = csSquaredDist::PointPoint (
			data->seg.Start (), data->isect);
		data->mesh = visobj_wrap->mesh;
	      }
	    }
	    else
	    {
	      bool rc;
	      if (data->accurate)
	        rc = visobj_wrap->mesh->GetMeshObject ()->HitBeamObject (
	      	  obj_start, obj_end, obj_isect, &r);
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
		  data->polygon = 0;
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
  }
  return true;
}

bool csFrustumVis::IntersectSegment (const csVector3& start,
    const csVector3& end, csVector3& isect, float* pr,
    iMeshWrapper** p_mesh, iPolygon3D** poly, bool accurate)
{
  UpdateObjects ();
  current_visnr++;
  IntersectSegment_Front2BackData data;
  data.seg.Set (start, end);
  data.sqdist = 10000000000.0;
  data.r = 10000000000.;
  data.mesh = 0;
  data.polygon = 0;
  data.vector = 0;
  data.accurate = accurate;
  kdtree->Front2Back (start, IntersectSegment_Front2Back, (void*)&data, 0);

  if (p_mesh) *p_mesh = data.mesh;
  if (pr) *pr = data.r;
  if (poly) *poly = data.polygon;
  isect = data.isect;

  return data.mesh != 0;
}

csPtr<iVisibilityObjectIterator> csFrustumVis::IntersectSegment (
    const csVector3& start, const csVector3& end, bool accurate)
{
  UpdateObjects ();
  current_visnr++;
  IntersectSegment_Front2BackData data;
  data.seg.Set (start, end);
  data.sqdist = 10000000000.0;
  data.r = 10000000000.;
  data.mesh = 0;
  data.polygon = 0;
  data.vector = new csVector ();
  data.accurate = accurate;
  kdtree->Front2Back (start, IntersectSegment_Front2Back, (void*)&data, 0);

  csFrustVisObjIt* vobjit = new csFrustVisObjIt (data.vector, 0);
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
  uint32 current_visnr;
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
      csFrustVisObjectWrapper* visobj_wrap = (csFrustVisObjectWrapper*)
      	objects[i]->GetObject ();
      const csBox3& obj_bbox = visobj_wrap->child->GetBBox ();
      csBox3 b (obj_bbox.Min ()-center, obj_bbox.Max ()-center);
      if (b.SquaredOriginDist () > sqrad)
	continue;

      if (visobj_wrap->caster && fview->ThingShadowsEnabled () &&
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


void csFrustumVis::CastShadows (iFrustumView* fview)
{
  UpdateObjects ();
  current_visnr++;
  CastShadows_Front2BackData data;
  data.current_visnr = current_visnr;
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
    printf ("INTERNAL ERROR! #vertices in GetVisibleObjects() exceeded!\n");
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

