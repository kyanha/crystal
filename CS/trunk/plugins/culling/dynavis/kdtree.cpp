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
#include "cssysdef.h"
#include "cssys/sysfunc.h"
#include "csutil/scfstr.h"
#include "iutil/string.h"
#include "qint.h"
#include "qsqrt.h"
#include "csgeom/math3d.h"
#include "kdtree.h"

//---------------------------------------------------------------------------

csKDTreeChild::csKDTreeChild ()
{
  object = NULL;
  num_leafs = 0;
  max_leafs = 2;
  leafs = new csKDTree* [max_leafs];
  timestamp = 0;
}

csKDTreeChild::~csKDTreeChild ()
{
  delete[] leafs;
}

void csKDTreeChild::AddLeaf (csKDTree* leaf)
{
  if (num_leafs >= max_leafs)
  {
    max_leafs += 3;
    csKDTree** new_leafs = new csKDTree* [max_leafs];
    if (leafs && num_leafs > 0)
      memcpy (new_leafs, leafs, sizeof (csKDTree*) * num_leafs);
    delete[] leafs;
    leafs = new_leafs;
  }

  leafs[num_leafs++] = leaf;
}

void csKDTreeChild::RemoveLeaf (int idx)
{
  CS_ASSERT (idx >= 0 && idx < num_leafs);
  if (num_leafs == 1)
  {
    // Easy case.
    num_leafs = 0;
    return;
  }

  if (idx < num_leafs-1)
    memmove (&leafs[idx], &leafs[idx+1],
    	sizeof (csKDTree*) * (num_leafs-idx-1));
  num_leafs--;
}

void csKDTreeChild::RemoveLeaf (csKDTree* leaf)
{
  int i;
  for (i = 0 ; i < num_leafs ; i++)
  {
    if (leafs[i] == leaf)
    {
      RemoveLeaf (i);
      return;
    }
  }
  CS_ASSERT (false);
}

void csKDTreeChild::ReplaceLeaf (csKDTree* old_leaf, csKDTree* new_leaf)
{
  int i;
  for (i = 0 ; i < num_leafs ; i++)
  {
    if (leafs[i] == old_leaf)
    {
      leafs[i] = new_leaf;
      return;
    }
  }
  CS_ASSERT (false);
}

int csKDTreeChild::FindLeaf (csKDTree* leaf)
{
  int i;
  for (i = 0 ; i < num_leafs ; i++)
  {
    if (leafs[i] == leaf) return i;
  }
  return -1;
}

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csKDTree)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iDebugHelper)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csKDTree::DebugHelper)
  SCF_IMPLEMENTS_INTERFACE(iDebugHelper)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

uint32 csKDTree::global_timestamp = 1;

csKDTree::csKDTree (csKDTree* parent)
{
  SCF_CONSTRUCT_IBASE (NULL);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiDebugHelper);

  child1 = NULL;
  child2 = NULL;
  csKDTree::parent = parent;
  objects = NULL;
  num_objects = max_objects = 0;
  disallow_distribute = false;

  obj_bbox_valid = true;
  obj_bbox.StartBoundingBox ();
  node_bbox.Set (-CS_BOUNDINGBOX_MAXVALUE, -CS_BOUNDINGBOX_MAXVALUE,
  	-CS_BOUNDINGBOX_MAXVALUE, CS_BOUNDINGBOX_MAXVALUE,
	CS_BOUNDINGBOX_MAXVALUE, CS_BOUNDINGBOX_MAXVALUE);
}

csKDTree::~csKDTree ()
{
  Clear ();
}

void csKDTree::Clear ()
{
  int i;
  for (i = 0 ; i < num_objects ; i++)
  {
    objects[i]->RemoveLeaf (this);
    // Remove this object if there are no more leafs refering to it.
    if (objects[i]->num_leafs == 0)
      delete objects[i];
  }
  delete[] objects;
  objects = NULL;
  num_objects = 0;
  max_objects = 0;
  delete child1; child1 = NULL;
  delete child2; child2 = NULL;
  disallow_distribute = false;
  obj_bbox_valid = true;
  obj_bbox.StartBoundingBox ();
}

void csKDTree::AddObject (csKDTreeChild* obj)
{
  CS_ASSERT ((max_objects == 0) == (objects == NULL));
  if (num_objects >= max_objects)
  {
    max_objects += MIN (max_objects+2, 80);
    csKDTreeChild** new_objects = new csKDTreeChild* [max_objects];
    if (objects && num_objects > 0)
      memcpy (new_objects, objects, sizeof (csKDTreeChild*) * num_objects);
    delete[] objects;
    objects = new_objects;
  }

  objects[num_objects++] = obj;
}

void csKDTree::RemoveObject (int idx)
{
  CS_ASSERT (idx >= 0 && idx < num_objects);
  if (num_objects == 1)
  {
    // Easy case.
    num_objects = 0;
    return;
  }

  if (idx < num_objects-1)
    memmove (&objects[idx], &objects[idx+1],
    	sizeof (csKDTreeChild*) * (num_objects-idx-1));
  num_objects--;
}

int csKDTree::FindObject (csKDTreeChild* obj)
{
  int i;
  for (i = 0 ; i < num_objects ; i++)
  {
    if (objects[i] == obj)
      return i;
  }
  return -1;
}

int csKDTree::FindBestSplitLocation (int axis, float& split_loc)
{
  int i, j;

  // If we have only two objects we use the average location between
  // the two objects.
  if (num_objects == 2)
  {
    const csBox3& bbox0 = objects[0]->bbox;
    const csBox3& bbox1 = objects[1]->bbox;
    float max0 = bbox0.Max (axis);
    float min1 = bbox1.Min (axis);
    if (max0 < min1-.01)	// Small thresshold to avoid bad split location.
    {
      split_loc = max0 + (min1-max0) * 0.5;
      CS_ASSERT (split_loc > max0);
      CS_ASSERT (split_loc < min1);
      return 32768;	// Good quality split.
    }
    float min0 = bbox0.Min (axis);
    float max1 = bbox1.Max (axis);
    if (max1 < min0-.01)
    {
      split_loc = max1 + (min0-max1) * 0.5;
      CS_ASSERT (split_loc > max1);
      CS_ASSERT (split_loc < min0);
      return 32768;	// Good quality split.
    }
    return -1;		// Very bad quality split.
  }

  // Calculate minimum and maximum value along the axis.
  float mina = objects[0]->bbox.Min (axis);
  float maxa = objects[0]->bbox.Max (axis);
  for (i = 1 ; i < num_objects ; i++)
  {
    const csBox3& bbox = objects[i]->bbox;
    if (bbox.Min (axis) < mina) mina = bbox.Min (axis);
    if (bbox.Max (axis) > maxa) maxa = bbox.Max (axis);
  }

  // Do 10 tests to find best split location. This should
  // probably be a configurable parameter.

  // @@@ Is the routine below very efficient?
# define FBSL_ATTEMPTS 10
  float a;
  int best_qual = -2;
  for (i = 0 ; i < FBSL_ATTEMPTS ; i++)
  {
    // Calculate a possible split location.
    a = mina + float (i+1)*(maxa-mina)/float (FBSL_ATTEMPTS+1.0);
    // Now count the number of objects that are completely
    // on the left and the number of objects completely on the right
    // side. The remaining objects are cut by this position.
    int left = 0;
    int right = 0;
    for (j = 0 ; j < num_objects ; j++)
    {
      const csBox3& bbox = objects[j]->bbox;
      if (bbox.Max (axis) < a) left++;
      else if (bbox.Min (axis) > a) right++;
    }
    int cut = num_objects-left-right;
    // If we have no object on the left or right then this is a bad
    // split which we should never take.
    int qual;
    if (left == 0 || right == 0)
    {
      qual = -1;
    }
    else
    {
      uint32 qual_cut = 32768 - ((cut << 15) / num_objects);
      uint32 qual_balance = 32768 - ((ABS (left-right) << 15) / num_objects);
      // Currently we just give 'cut' and 'balance' quality an equal share.
      // We should consider if that is a good measure.
      qual = int ((qual_cut * qual_balance) >> 15);
    }
    if (qual > best_qual)
    {
      best_qual = qual;
      split_loc = a;
    }
  }
# undef FBSL_ATTEMPTS
  return best_qual;
}

void csKDTree::UpdateBBox (const csBox3& bbox)
{
  // This function assumes that the object is already
  // added to this node.
  if (!obj_bbox_valid)
  {
    // If obj_bbox was not valid we do nothing. The bbox will be calculated
    // later when we really need it anyway.
    return;
  }
  if (num_objects > 1)
    obj_bbox += bbox;
  else
    obj_bbox = bbox;
}

const csBox3& csKDTree::GetObjectBBox ()
{
  if (!obj_bbox_valid)
  {
    obj_bbox_valid = true;
    if (num_objects > 0)
    {
      obj_bbox = objects[0]->bbox;
      int i;
      for (i = 1 ; i < num_objects ; i++)
        obj_bbox += objects[i]->bbox;
    }
    else
      obj_bbox.StartBoundingBox ();
  }
  return obj_bbox;
}

void csKDTree::DistributeLeafObjects ()
{
  CS_ASSERT (split_axis >= CS_KDTREE_AXISX && split_axis <= CS_KDTREE_AXISZ);
  int i;
  for (i = 0 ; i < num_objects ; i++)
  {
    const csBox3& bbox = objects[i]->bbox;
    float bbox_min = bbox.Min (split_axis);
    float bbox_max = bbox.Max (split_axis);
    bool leaf_replaced = false;
    if (bbox_min <= split_location)
    {
      objects[i]->ReplaceLeaf (this, child1);
      leaf_replaced = true;
      child1->AddObject (objects[i]);
      child1->UpdateBBox (bbox);
    }
    if (bbox_max >= split_location)
    {
      if (leaf_replaced)
      {
        // If we also added this object to child1
	// we need to call AddLeaf() instead of ReplaceLeaf().
        objects[i]->AddLeaf (child2);
      }
      else
      {
        objects[i]->ReplaceLeaf (this, child2);
        leaf_replaced = true;
      }
      child2->AddObject (objects[i]);
      child2->UpdateBBox (bbox);
    }
    CS_ASSERT (leaf_replaced);
  }

  num_objects = 0;
  // @@@ Clean up objects array if there are too many objects?
  // There should be some thresshold at least.
}

void csKDTree::AddObject (const csBox3& bbox, csKDTreeChild* obj)
{
  // Add this object to the list of objects to be distributed
  // later.
  disallow_distribute = false;
  obj->AddLeaf (this);
  AddObject (obj);
  UpdateBBox (bbox);
}

csKDTreeChild* csKDTree::AddObject (const csBox3& bbox, void* object)
{
  csKDTreeChild* obj = new csKDTreeChild ();
  obj->object = object;
  obj->bbox = bbox;
  AddObject (bbox, obj);
  return obj;
}

void csKDTree::UnlinkObject (csKDTreeChild* object)
{
  int i;
  for (i = 0 ; i < object->num_leafs ; i++)
  {
    csKDTree* leaf = object->leafs[i];
    int idx = leaf->FindObject (object);
    CS_ASSERT (idx != -1);
    leaf->RemoveObject (idx);
    leaf->obj_bbox_valid = false;
    leaf->disallow_distribute = false;	// Give distribute a new chance.
  }
  object->num_leafs = 0;
}

void csKDTree::RemoveObject (csKDTreeChild* object)
{
  UnlinkObject (object);
  delete object;
}

void csKDTree::MoveObject (csKDTreeChild* object, const csBox3& new_bbox)
{
  // If the object is in only one leaf then we test if this object
  // will still be in the bounding box of that leaf after moving it around.
  if (object->num_leafs == 1)
  {
    if (object->leafs[0]->GetNodeBBox ().Contains (new_bbox))
    {
      // Even after moving we are still completely inside the bounding box
      // of the current leaf.
      object->bbox = new_bbox;
      object->leafs[0]->obj_bbox_valid = false;
      object->leafs[0]->disallow_distribute = false;
      return;
    }
  }

  // When updating the bounding box of an object we move the object upwards
  // in the tree until we find a node that completely contains the
  // new bounding box. For small movements this usually means that the
  // object will stay in its current node. Note that the case above already
  // catches that situation but we keep the above case because it is
  // slightly more efficient even.
  csKDTree* node = this;
  if (object->num_leafs > 0)
  {
    node = object->leafs[0];
    UnlinkObject (object);
    object->bbox = new_bbox;
    while (node->parent && !node->GetNodeBBox ().Contains (new_bbox))
    {
      node = node->parent;
    }
  }
  node->AddObject (new_bbox, object);
}
  
void csKDTree::Distribute ()
{
  // Check if there are objects to distribute or if distribution
  // is not allowed.
  if (num_objects == 0 || disallow_distribute) return;

  CS_ASSERT ((child1 == NULL) == (child2 == NULL));
  if (child1)
  {
    // This node has children. So we have to see to what child (or both)
    // we distribute the objects in the this node.
    DistributeLeafObjects ();
    CS_ASSERT (num_objects == 0);

    // Update the bounding box of this node.
    obj_bbox.StartBoundingBox ();
    obj_bbox_valid = true;
  }
  else
  {
    // This node doesn't have children yet.

    // If we only have one object we do nothing.
    if (num_objects == 1) return;

    // Here we have 2 or more objects.
    // We use FindBestSplitLocation() to see how we can best split this
    // node.
    float split_loc_x, split_loc_y, split_loc_z;
    int qual_x = FindBestSplitLocation (CS_KDTREE_AXISX, split_loc_x);
    int qual_y = FindBestSplitLocation (CS_KDTREE_AXISY, split_loc_y);
    int qual_z = FindBestSplitLocation (CS_KDTREE_AXISZ, split_loc_z);
    if (qual_x >= 0 && qual_x >= qual_y && qual_x >= qual_z)
    {
      split_axis = CS_KDTREE_AXISX;
      split_location = split_loc_x;
    }
    else if (qual_y >= 0 && qual_y >= qual_x && qual_y >= qual_z)
    {
      split_axis = CS_KDTREE_AXISY;
      split_location = split_loc_y;
    }
    else if (qual_z >= 0)
    {
      split_axis = CS_KDTREE_AXISZ;
      split_location = split_loc_z;
    }
    else
    {
      // No good quality split was found so we don't split. This
      // can happen if all objects are placed on top of each other.
      disallow_distribute = true;
    }
    if (!disallow_distribute)
    {
      child1 = new csKDTree (this);
      child2 = new csKDTree (this);
      DistributeLeafObjects ();
      CS_ASSERT (num_objects == 0);
      // Update the bounding box of this node.
      obj_bbox.StartBoundingBox ();
      obj_bbox_valid = true;
      child1->node_bbox = GetNodeBBox ();
      child1->node_bbox.SetMax (split_axis, split_location);
      child2->node_bbox = GetNodeBBox ();
      child2->node_bbox.SetMin (split_axis, split_location);
    }
  }
}

void csKDTree::FullDistribute ()
{
  Distribute ();
  if (child1)
  {
    child1->FullDistribute ();
    CS_ASSERT (child2 != NULL);
    child2->FullDistribute ();
  }
}

void csKDTree::Flatten ()
{
  if (!child1) return;	// Nothing to do.

  // First flatten the children.
  // @@@ Is this the most optimal solution?
  child1->Flatten ();
  child2->Flatten ();

  csKDTree* c1 = child1; child1 = NULL;
  csKDTree* c2 = child2; child2 = NULL;

  int i;
  for (i = 0 ; i < c1->num_objects ; i++)
  {
    csKDTreeChild* obj = c1->objects[i];
    if (obj->num_leafs == 1)
    {
      CS_ASSERT (obj->leafs[0] == c1);
      obj->leafs[0] = this;
      AddObject (obj);
      UpdateBBox (obj->bbox);
    }
    else
    {
      if (obj->FindLeaf (this) == -1)
      {
        obj->ReplaceLeaf (c1, this);
	AddObject (obj);
        UpdateBBox (obj->bbox);
      }
    }
  }
  for (i = 0 ; i < c2->num_objects ; i++)
  {
    csKDTreeChild* obj = c2->objects[i];
    if (obj->num_leafs == 1)
    {
      CS_ASSERT (obj->leafs[0] == c2);
      obj->leafs[0] = this;
      AddObject (obj);
      UpdateBBox (obj->bbox);
    }
    else
    {
      if (obj->FindLeaf (this) == -1)
      {
        obj->ReplaceLeaf (c2, this);
	AddObject (obj);
        UpdateBBox (obj->bbox);
      }
    }
  }
}

bool csKDTree::Front2Back (const csVector3& pos, csKDTreeVisitFunc* func,
  	void* userdata, uint32 cur_timestamp)
{
  if (!func (this, userdata, cur_timestamp))
    return false;
  if (child1)
  {
    // There are children.
    if (pos[split_axis] <= split_location)
    {
      child1->Front2Back (pos, func, userdata, cur_timestamp);
      child2->Front2Back (pos, func, userdata, cur_timestamp);
    }
    else
    {
      child2->Front2Back (pos, func, userdata, cur_timestamp);
      child1->Front2Back (pos, func, userdata, cur_timestamp);
    }
  }
  return true;
}

void csKDTree::ResetTimestamps ()
{
  int i;
  for (i = 0 ; i < num_objects ; i++)
    objects[i]->timestamp = 0;
  if (child1)
  {
    child1->ResetTimestamps ();
    child2->ResetTimestamps ();
  }
}

void csKDTree::Front2Back (const csVector3& pos, csKDTreeVisitFunc* func,
  	void* userdata)
{
  if (global_timestamp > 4000000000u)
  {
    // For safety reasons we will reset all timestamps to 0
    // for all objects in the tree and also set the global
    // timestamp to 1 again. This should be very rare (every
    // 4000000000 calls of Front2Back :-)
    ResetTimestamps ();
    global_timestamp = 1;
  }
  else
  {
    global_timestamp++;
  }
  Front2Back (pos, func, userdata, global_timestamp);
}

#define KDT_ASSERT(test,msg) \
  if (!(test)) \
  { \
    csString ss; \
    ss.Format ("csKDTree failure (%d,%s): %s\n", int(__LINE__), \
    	#msg, #test); \
    str.Append (ss); \
    return rc; \
  }

bool csKDTree::Debug_CheckTree (csString& str)
{
  bool rc = false;

  KDT_ASSERT ((child1 == NULL) == (child2 == NULL), "child consistency");

  if (child1)
  {
    //-------
    // Test-cases in case this is a node.
    //-------

    KDT_ASSERT (split_axis >= CS_KDTREE_AXISX && split_axis <= CS_KDTREE_AXISZ,
    	"axis");
    KDT_ASSERT (GetNodeBBox ().Contains (child1->GetNodeBBox ()),
    	"node_bbox mismatch");
    KDT_ASSERT (GetNodeBBox ().Contains (child2->GetNodeBBox ()),
    	"node_bbox mismatch");
    csBox3 new_node_bbox = child1->GetNodeBBox ();
    new_node_bbox += child2->GetNodeBBox ();
    KDT_ASSERT (new_node_bbox == GetNodeBBox (), "node_bbox mismatch");
    if (num_objects > 0)
    {
      csBox3 intersect = GetNodeBBox () * GetObjectBBox ();
      KDT_ASSERT (!intersect.Empty (), "node_bbox * tree_box == empty!");
    }
    else
    {
      KDT_ASSERT (GetObjectBBox ().Empty (), "obj_bbox is not empty!");
    }

    KDT_ASSERT (child1->parent == this, "parent check");
    KDT_ASSERT (child2->parent == this, "parent check");

    if (!child1->Debug_CheckTree (str))
      return false;
    if (!child2->Debug_CheckTree (str))
      return false;
  }

  //-------
  // Test-cases in case this is a leaf (or not a leaf but has
  // objects waiting for distribution).
  //-------

  KDT_ASSERT (num_objects <= max_objects, "object list");

  int i, j;
  for (i = 0 ; i < num_objects ; i++)
  {
    csKDTreeChild* o = objects[i];

    KDT_ASSERT (GetObjectBBox ().Contains (o->bbox), "object not in obj_bbox");

    KDT_ASSERT (o->num_leafs <= o->max_leafs, "leaf list");
    int parcnt = 0;
    for (j = 0 ; j < o->num_leafs ; j++)
    {
      if (o->leafs[j] == this)
      {
	parcnt++;
        KDT_ASSERT (parcnt <= 1, "parent occurs multiple times");
      }
    }
    KDT_ASSERT (parcnt == 1, "leaf list doesn't contain parent");
  }

  return true;
}

static float rnd (float range)
{
  return float ((rand () >> 4) % 1000) * range / 1000.0;
}

// Number of objects we use for UnitTest().
#define CS_UNITTEST_OBJECTS 500

struct Debug_TraverseData
{
  int obj_counter;
  // The min_node_bbox array contains pointers to the node bounding
  // boxes we encounter before traversing further objects.
  const csBox3* min_node_bbox[CS_UNITTEST_OBJECTS*10];

  // The obj_bbox array contains pointers to the object bounding
  // boxes we encounter during traversing a node. Or NULL
  // if this is a node entry.
  const csBox3* obj_bbox[CS_UNITTEST_OBJECTS*10];

  // Counter of the number of pointers in the above three arrays.
  int num_bbox_pointers;
};

static bool Debug_TraverseFunc (csKDTree* treenode, void* userdata,
	uint32 cur_timestamp)
{
  Debug_TraverseData* data = (Debug_TraverseData*)userdata;

  treenode->Distribute ();

  data->min_node_bbox[data->num_bbox_pointers] = &(treenode->GetNodeBBox ());
  data->obj_bbox[data->num_bbox_pointers] = NULL;
  data->num_bbox_pointers++;
  CS_ASSERT (data->num_bbox_pointers < CS_UNITTEST_OBJECTS * 10);

  int num_objects = treenode->GetObjectCount ();
  csKDTreeChild** objects = treenode->GetObjects ();
  int i;
  for (i = 0 ; i < num_objects ; i++)
  {
    if (objects[i]->timestamp != cur_timestamp)
    {
      objects[i]->timestamp = cur_timestamp;
      data->obj_counter++;

      data->min_node_bbox[data->num_bbox_pointers] = &(treenode->GetNodeBBox ());
      data->obj_bbox[data->num_bbox_pointers] = &(objects[i]->GetBBox ());
      data->num_bbox_pointers++;
      CS_ASSERT (data->num_bbox_pointers < CS_UNITTEST_OBJECTS * 10);
    }
  }

  return true;
}

iString* csKDTree::Debug_UnitTest ()
{
  csTicks seed = csGetTicks ();
  srand (seed);

  iString* dbdump;
  scfString* rc = new scfString ();
  csString& str = rc->GetCsString ();

  str.Format ("Seed %d\n", seed);

  //=================
  // Test if after Clear() the tree is sufficiently empty :-)
  //=================
  Clear ();
  KDT_ASSERT (child1 == NULL, "clear ok?");
  KDT_ASSERT (child2 == NULL, "clear ok?");
  KDT_ASSERT (objects == NULL, "clear ok?");
  KDT_ASSERT (num_objects == 0, "clear ok?");
  KDT_ASSERT (max_objects == 0, "clear ok?");

  //=================
  // First we test a simple case but one in which there are two
  // bounding boxes are exactly the same. This could cause problems
  // in the algorithm.
  //=================
  csBox3 b;
  b.Set (9, 7, 8, 11, 9, 10);
  AddObject (b, (void*)1);
  b.Set (12, 7, 8, 13, 9, 10);
  AddObject (b, (void*)2);
  b.Set (10, 11, 8, 14, 12, 10);
  AddObject (b, (void*)3);
  b.Set (10, 11, 11, 14, 12, 14);
  AddObject (b, (void*)3);
  b.Set (10, 11, 11, 14, 12, 14);
  AddObject (b, (void*)4);
  if (!Debug_CheckTree (str)) return rc;
  Distribute ();
  if (!Debug_CheckTree (str)) return rc;
  FullDistribute ();
  if (!Debug_CheckTree (str)) return rc;

  Clear ();

  //=================
  // In this test we are going to insert CS_UNITTEST_OBJECTS
  // objects randomly in the tree. Every 20 objects we do a
  // FullDistribute() which optimizes the tree. Since we do
  // it every 20 objects instead of after all objects have
  // been added the quality of the tree will not be optimal.
  // The tests below will print out statistics to show that.
  // Every 20 objects we will also remove one object to test
  // if RemoveObject() is working properly.
  //=================
  int i, j;
  csKDTreeChild* remove_obj = NULL;
  for (i = 0 ; i < CS_UNITTEST_OBJECTS ; i++)
  {
    float x = rnd (100.0)-50.0;
    float y = rnd (100.0)-50.0;
    float z = rnd (100.0)-50.0;
    b.Set (x, y, z, x+rnd (7.0)+.5, y+rnd (7.0)+.5, z+rnd (7.0)+.5);
    csKDTreeChild* new_obj = AddObject (b, (void*)0);
    if (!Debug_CheckTree (str)) return rc;
    if (i % 20 == 0)
    {
      FullDistribute ();
      if (!Debug_CheckTree (str)) return rc;
      // Remove the previous object and store pointer to this one
      // for next removal.
      if (remove_obj)
      {
        csBox3 bb = remove_obj->GetBBox ();
	void* obj = remove_obj->GetObject ();
        RemoveObject (remove_obj);
	AddObject (bb, obj);
      }
      remove_obj = new_obj;
    }
  }

  //=================
  // 'remove_obj' will now point to one of the objects in the tree.
  // We will move that around randomly.
  //=================
  for (i = 0 ; i < 1000 ; i++)
  {
    if (rnd (10.0) < 1.0)
    {
      // Total new position.
      float x = rnd (100.0)-50.0;
      float y = rnd (100.0)-50.0;
      float z = rnd (100.0)-50.0;
      b.Set (x, y, z, x+rnd (7.0)+.5, y+rnd (7.0)+.5, z+rnd (7.0)+.5);
    }
    else
    {
      // Move the current box slightly.
      b = remove_obj->GetBBox ();
      float dx = rnd (1.0)-0.5;
      float dy = rnd (1.0)-0.5;
      float dz = rnd (1.0)-0.5;
      b.SetMin (0, b.MinX ()+dx);
      b.SetMin (1, b.MinY ()+dy);
      b.SetMin (2, b.MinZ ()+dz);
      b.SetMax (0, b.MaxX ()+dx);
      b.SetMax (1, b.MaxY ()+dy);
      b.SetMax (2, b.MaxZ ()+dz);
    }
    MoveObject (remove_obj, b);
    if (!Debug_CheckTree (str)) return rc;
  }

  //=================
  // Do a final FullDistribute() to force the tree to be fully
  // distributed. Then print out statistics to show current quality.
  // This will not be extremely good since we distributed every 20 objects.
  //=================
  FullDistribute ();
  if (!Debug_CheckTree (str)) return rc;
  dbdump = Debug_Statistics ();
  printf ("Step 1: %s", dbdump->GetData ()); fflush (stdout);
  dbdump->DecRef ();

  //=================
  // Now we flatten the tree completely.
  //=================
  Flatten ();
  if (!Debug_CheckTree (str)) return rc;
  dbdump = Debug_Statistics ();
  printf ("Flat  : %s", dbdump->GetData ()); fflush (stdout);
  dbdump->DecRef ();

  //=================
  // Do a FullDistribute() again. Since we now start from a flattened
  // tree this should result in a much better quality tree since all
  // information is available.
  //=================
  FullDistribute ();
  if (!Debug_CheckTree (str)) return rc;
  dbdump = Debug_Statistics ();
  printf ("Optim : %s", dbdump->GetData ()); fflush (stdout);
  dbdump->DecRef ();

  //=================
  // Now we are going to test traversal. The crucial thing here is
  // that the traversal of the entire tree should return exactly
  // CS_UNITTEST_OBJECTS objects. We are also going to test if
  // Front2Back() traversal is really front to back.
  //=================
  Debug_TraverseData data;
  // Test 20 different starting positions.
  for (i = 0 ; i < 20 ; i++)
  {
    data.obj_counter = 0;
    data.num_bbox_pointers = 0;
    csVector3 start (rnd (100.0)-50.0, rnd (100.0)-50.0, rnd (100.0)-50.0);
    Front2Back (start, Debug_TraverseFunc, (void*)&data);
    KDT_ASSERT (data.obj_counter == CS_UNITTEST_OBJECTS,
  	"number of objects traversed doesn't match tree!");

    // Test 20 different end positions for Front2Back testing.
    for (j = 0 ; j < 20 ; j++)
    {
      csVector3 end;
      end.Set (rnd (1000.0)+1.0, rnd (1000.0)+1.0, rnd (1000.0)+1.0);
      end.Normalize ();
      end *= 1000000.0;
      end += start;

      csSegment3 seg (start, end);
      float max_minsqdist = 0.0;
      float max_nodesqdist = 0.0;
      csVector3 isect;
      for (i = 0 ; i < data.num_bbox_pointers ; i++)
      {
        if (data.obj_bbox[i])
        {
	  // Take bbox of object and intersect with current node
	  // bbox. This is required because otherwise perfect
	  // Front2Back for object/nodes is not guaranteed.
	  csBox3 obox = *data.obj_bbox[i];
	  obox = obox * *data.min_node_bbox[i];
          if (csIntersect3::BoxSegment (obox, seg, isect) != -1)
          {
            float obj_sqdist = csSquaredDist::PointPoint (start, isect);
            if (obj_sqdist > max_minsqdist)
	      max_minsqdist = obj_sqdist;
          }
        }
        else
        {
          if (csIntersect3::BoxSegment (*data.min_node_bbox[i], seg, isect)
	  	!= -1)
          {
            float node_sqdist = csSquaredDist::PointPoint (start, isect);
	    if (node_sqdist < 1000000.0)
	    {
	      KDT_ASSERT (node_sqdist >= (max_minsqdist-0.1),
	    	  "bad front2back sorting!");
	      if (!(node_sqdist >= (max_nodesqdist-0.1)))
	      {
	        printf ("node_sqdist=%g max_nodesqdist=%g\n", node_sqdist,
	          max_nodesqdist);
	      }
	      KDT_ASSERT (node_sqdist >= (max_nodesqdist-0.1),
	    	  "bad front2back sorting!");
	    }
	    max_nodesqdist = node_sqdist;
          }
        }
      }
    }
  }

  //=================
  // Flatten the tree and test traversal again. The traversal function
  // will do Distribute() so the end result should be exactly the same
  // as the FullDistribute(). Statistics are printed to show this.
  //=================
  Flatten ();
  data.obj_counter = 0;
  data.num_bbox_pointers = 0;
  Front2Back (csVector3 (0, 0, 0), Debug_TraverseFunc, (void*)&data);
  KDT_ASSERT (data.obj_counter == CS_UNITTEST_OBJECTS,
  	"number of objects traversed doesn't match tree!");
  if (!Debug_CheckTree (str)) return rc;
  dbdump = Debug_Statistics ();
  printf ("Traver: %s", dbdump->GetData ()); fflush (stdout);
  dbdump->DecRef ();

  rc->DecRef ();
  return NULL;
}

static bool Debug_TraverseFuncBenchmark (csKDTree* treenode, void*,
	uint32 cur_timestamp)
{
  treenode->Distribute ();

  int num_objects = treenode->GetObjectCount ();
  csKDTreeChild** objects = treenode->GetObjects ();
  int i;
  for (i = 0 ; i < num_objects ; i++)
  {
    if (objects[i]->timestamp != cur_timestamp)
      objects[i]->timestamp = cur_timestamp;
  }

  return true;
}

csTicks csKDTree::Debug_Benchmark (int num_iterations)
{
  int i, j;

  srand (12345678);

  csTicks pass0 = csGetTicks ();

  csBox3 b;
  for (i = 0 ; i < num_iterations ; i++)
  {
    Clear ();
    for (j = 0 ; j < 500 ; j++)
    {
      float x = rnd (100.0)-50.0;
      float y = rnd (100.0)-50.0;
      float z = rnd (100.0)-50.0;
      b.Set (x, y, z, x+rnd (7.0)+.5, y+rnd (7.0)+.5, z+rnd (7.0)+.5);
      AddObject (b, (void*)0);
      if (i % 20 == 0) FullDistribute ();
    }
  }

  csTicks pass1 = csGetTicks ();

  for (i = 0 ; i < num_iterations ; i++)
  {
    Front2Back (csVector3 (0, 0, 0), Debug_TraverseFuncBenchmark, NULL);
  }

  csTicks pass2 = csGetTicks ();

  for (i = 0 ; i < num_iterations ; i++)
  {
    Flatten ();
    FullDistribute ();
  }

  csTicks pass3 = csGetTicks ();

  for (i = 0 ; i < num_iterations ; i++)
  {
    Front2Back (csVector3 (0, 0, 0), Debug_TraverseFuncBenchmark, NULL);
  }

  csTicks pass4 = csGetTicks ();

  printf ("Creating the tree:        %d ms\n", pass1-pass0);
  printf ("Unoptimized Front2Back:   %d ms\n", pass2-pass1);
  printf ("Flatten + FullDistribute: %d ms\n", pass3-pass2);
  printf ("Optimized Front2Back:     %d ms\n", pass4-pass3);

  return pass4-pass0;
}

void csKDTree::Debug_Statistics (int& tot_objects,
	int& tot_nodes, int& tot_leaves, int depth, int& max_depth,
	float& balance_quality)
{
  tot_objects += num_objects;
  if (child1) tot_nodes++;
  else tot_leaves++;
  depth++;
  if (depth > max_depth) max_depth = depth;
  if (child1)
  {
    int left = 0;
    int right = 0;
    child1->Debug_Statistics (left, tot_nodes,
    	tot_leaves, depth, max_depth, balance_quality);
    child2->Debug_Statistics (right, tot_nodes,
    	tot_leaves, depth, max_depth, balance_quality);
    tot_objects += left;
    tot_objects += right;

    float qual_balance = 1.0 - float (ABS (left-right)) / float (left+right);
    balance_quality += qual_balance;
  }
}

iString* csKDTree::Debug_Statistics ()
{
  scfString* rc = new scfString ();
  csString& str = rc->GetCsString ();

  int tot_objects, tot_nodes, tot_leaves, max_depth;
  float balance_quality;
  tot_objects = 0;
  tot_nodes = 0;
  tot_leaves = 0;
  max_depth = 0;
  balance_quality = 0.0;
  Debug_Statistics (tot_objects, tot_nodes, tot_leaves, 0, max_depth,
  	balance_quality);
  str.Format ("#o=%d #n=%d #l=%d maxd=%d balqual=%g\n",
  	tot_objects, tot_nodes, tot_leaves, max_depth,
	balance_quality / float (tot_nodes));

  return rc;
}

void csKDTree::Debug_Dump (csString& str, int indent)
{
  char* spaces = new char[indent+1];
  char* s = spaces;
  int ind = indent;
  while (ind >= 10)
  {
    strcpy (s, "          ");
    s += 10;
    ind -= 10;
  }
  while (ind > 0) { *s++ = ' '; ind--; }
  *s = 0;

  csString ss;
  iString* stats = Debug_Statistics ();
  ss.Format ("%s KDT obj_bbox(%g,%g,%g)-(%g,%g,%g) disallow_dist=%d\n%s     node_bbox=(%g,%g,%g)-(%g,%g,%g)\n%s %s",
  	spaces, GetObjectBBox ().MinX (), GetObjectBBox ().MinY (),
	GetObjectBBox ().MinZ (), GetObjectBBox ().MaxX (),
	GetObjectBBox ().MaxY (), GetObjectBBox ().MaxZ (),
	disallow_distribute,
  	spaces, GetNodeBBox ().MinX (), GetNodeBBox ().MinY (),
	GetNodeBBox ().MinZ (), GetNodeBBox ().MaxX (),
	GetNodeBBox ().MaxY (), GetNodeBBox ().MaxZ (),
  	spaces, stats->GetData ());
  stats->DecRef ();
  str.Append (ss);
  if (child1)
  {
    ss.Format ("%s   axis=%c loc=%g\n",
        spaces,
    	split_axis == CS_KDTREE_AXISX ? 'x' :
	split_axis == CS_KDTREE_AXISY ? 'y' : 'z',
	split_location);
    str.Append (ss);
    child1->Debug_Dump (str, indent+2);
    child2->Debug_Dump (str, indent+2);
  }
  else
  {
    ss.Format ("%s   %d objects\n", spaces, num_objects);
    str.Append (ss);
  }
}

