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
#include "fkdtree.h"

//---------------------------------------------------------------------------

csSimpleKDTreeChild::csSimpleKDTreeChild ()
{
  object = NULL;
  num_leafs = 0;
  max_leafs = 2;
  leafs = new csSimpleKDTree* [max_leafs];
  timestamp = 0;
}

csSimpleKDTreeChild::~csSimpleKDTreeChild ()
{
  delete[] leafs;
}

void csSimpleKDTreeChild::AddLeaf (csSimpleKDTree* leaf)
{
  if (num_leafs >= max_leafs)
  {
    max_leafs += 3;
    csSimpleKDTree** new_leafs = new csSimpleKDTree* [max_leafs];
    if (leafs && num_leafs > 0)
      memcpy (new_leafs, leafs, sizeof (csSimpleKDTree*) * num_leafs);
    delete[] leafs;
    leafs = new_leafs;
  }

  leafs[num_leafs++] = leaf;
}

void csSimpleKDTreeChild::RemoveLeaf (int idx)
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
    	sizeof (csSimpleKDTree*) * (num_leafs-idx-1));
  num_leafs--;
}

void csSimpleKDTreeChild::RemoveLeaf (csSimpleKDTree* leaf)
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

void csSimpleKDTreeChild::ReplaceLeaf (csSimpleKDTree* old_leaf,
	csSimpleKDTree* new_leaf)
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

int csSimpleKDTreeChild::FindLeaf (csSimpleKDTree* leaf)
{
  int i;
  for (i = 0 ; i < num_leafs ; i++)
  {
    if (leafs[i] == leaf) return i;
  }
  return -1;
}

//---------------------------------------------------------------------------

uint32 csSimpleKDTree::global_timestamp = 1;

#define KDTREE_MAX 10000000.

csSimpleKDTree::csSimpleKDTree (csSimpleKDTree* parent)
{
  child1 = NULL;
  child2 = NULL;
  csSimpleKDTree::parent = parent;
  objects = NULL;
  num_objects = max_objects = 0;
  disallow_distribute = false;
  split_axis = CS_KDTREE_AXISINVALID;

  obj_bbox_valid = true;
  obj_bbox.StartBoundingBox ();
  node_bbox.Set (-KDTREE_MAX, -KDTREE_MAX,
  	-KDTREE_MAX, KDTREE_MAX,
	KDTREE_MAX, KDTREE_MAX);
}

csSimpleKDTree::~csSimpleKDTree ()
{
  Clear ();
}

void csSimpleKDTree::Clear ()
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

void csSimpleKDTree::AddObject (csSimpleKDTreeChild* obj)
{
  CS_ASSERT ((max_objects == 0) == (objects == NULL));
  if (num_objects >= max_objects)
  {
    max_objects += MIN (max_objects+2, 80);
    csSimpleKDTreeChild** new_objects = new csSimpleKDTreeChild* [max_objects];
    if (objects && num_objects > 0)
      memcpy (new_objects, objects,
      	sizeof (csSimpleKDTreeChild*) * num_objects);
    delete[] objects;
    objects = new_objects;
  }

  objects[num_objects++] = obj;
}

void csSimpleKDTree::RemoveObject (int idx)
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
    	sizeof (csSimpleKDTreeChild*) * (num_objects-idx-1));
  num_objects--;
}

int csSimpleKDTree::FindObject (csSimpleKDTreeChild* obj)
{
  int i;
  for (i = 0 ; i < num_objects ; i++)
  {
    if (objects[i] == obj)
      return i;
  }
  return -1;
}

int csSimpleKDTree::FindBestSplitLocation (int axis, float& split_loc)
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
  // Make sure we don't go outside node_box.
  if (mina < node_bbox.Min (axis)) mina = node_bbox.Min (axis);
  if (maxa > node_bbox.Max (axis)) maxa = node_bbox.Max (axis);

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
      // The .0001 is for safety.
      if (bbox.Max (axis) < a-.0001) left++;
      else if (bbox.Min (axis) > a+.0001) right++;
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

void csSimpleKDTree::UpdateBBox (const csBox3& bbox)
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

const csBox3& csSimpleKDTree::GetObjectBBox ()
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

void csSimpleKDTree::DistributeLeafObjects ()
{
  CS_ASSERT (split_axis >= CS_KDTREE_AXISX && split_axis <= CS_KDTREE_AXISZ);
  int i;
  for (i = 0 ; i < num_objects ; i++)
  {
    const csBox3& bbox = objects[i]->bbox;
    float bbox_min = bbox.Min (split_axis);
    float bbox_max = bbox.Max (split_axis);
    bool leaf_replaced = false;
    // The SMALL_EPSILON is used to ensure that when bbox_min
    // is equal to bbox_max we don't get a situation where
    // both of the if's are not used.
    if (bbox_min-SMALL_EPSILON <= split_location)
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

void csSimpleKDTree::AddObject (const csBox3& bbox, csSimpleKDTreeChild* obj)
{
  // Add this object to the list of objects to be distributed
  // later.
  disallow_distribute = false;
  obj->AddLeaf (this);
  AddObject (obj);
  UpdateBBox (bbox);
}

csSimpleKDTreeChild* csSimpleKDTree::AddObject (const csBox3& bbox,
	void* object)
{
  csSimpleKDTreeChild* obj = new csSimpleKDTreeChild ();
  obj->object = object;
  obj->bbox = bbox;
  AddObject (bbox, obj);
  return obj;
}

void csSimpleKDTree::UnlinkObject (csSimpleKDTreeChild* object)
{
  int i;
  for (i = 0 ; i < object->num_leafs ; i++)
  {
    csSimpleKDTree* leaf = object->leafs[i];
    int idx = leaf->FindObject (object);
    CS_ASSERT (idx != -1);
    leaf->RemoveObject (idx);
    leaf->obj_bbox_valid = false;
    leaf->disallow_distribute = false;	// Give distribute a new chance.
  }
  object->num_leafs = 0;
}

void csSimpleKDTree::RemoveObject (csSimpleKDTreeChild* object)
{
  UnlinkObject (object);
  delete object;
}

void csSimpleKDTree::MoveObject (csSimpleKDTreeChild* object,
	const csBox3& new_bbox)
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

  object->bbox = new_bbox;

  // When updating the bounding box of an object we move the object upwards
  // in the tree until we find a node that completely contains the
  // new bounding box. For small movements this usually means that the
  // object will stay in its current node. Note that the case above already
  // catches that situation but we keep the above case because it is
  // slightly more efficient even.

  // The following counter makes sure we flatten the top-most parent
  // node every 50 times an object has moved. This ensures the tree
  // will keep reasonable quality. We don't do this every time because
  // Flatten() itself has some overhead.
  static int cnt = 50;
  cnt--;
  bool do_flatten = false;
  if (cnt < 0)
  {
    cnt = 50;
    do_flatten = true;
  }

  csSimpleKDTree* node = this;
  if (object->num_leafs > 0)
  {
    node = object->leafs[0];
    if (!do_flatten) UnlinkObject (object);
    while (node->parent && !node->GetNodeBBox ().Contains (new_bbox))
    {
      node = node->parent;
    }
    if (do_flatten)
      node->Flatten ();
    else
      node->AddObject (new_bbox, object);
  }
}
  
void csSimpleKDTree::Distribute ()
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
      child1 = new csSimpleKDTree (this);
      child2 = new csSimpleKDTree (this);
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

void csSimpleKDTree::FullDistribute ()
{
  Distribute ();
  if (child1)
  {
    child1->FullDistribute ();
    CS_ASSERT (child2 != NULL);
    child2->FullDistribute ();
  }
}

void csSimpleKDTree::FlattenTo (csSimpleKDTree* node)
{
  if (!child1) return;	// Nothing to do.

  // First flatten the children.
  // @@@ Is this the most optimal solution?
  child1->FlattenTo (node);
  child2->FlattenTo (node);

  csSimpleKDTree* c1 = child1; child1 = NULL;
  csSimpleKDTree* c2 = child2; child2 = NULL;

  int i;
  for (i = 0 ; i < c1->num_objects ; i++)
  {
    csSimpleKDTreeChild* obj = c1->objects[i];
    if (obj->num_leafs == 1)
    {
      CS_ASSERT (obj->leafs[0] == c1);
      obj->leafs[0] = node;
      node->AddObject (obj);
      node->UpdateBBox (obj->bbox);
    }
    else
    {
      if (obj->FindLeaf (node) == -1)
      {
        obj->ReplaceLeaf (c1, node);
	node->AddObject (obj);
        node->UpdateBBox (obj->bbox);
      }
      else
      {
        obj->RemoveLeaf (c1);
      }
    }
  }
  for (i = 0 ; i < c2->num_objects ; i++)
  {
    csSimpleKDTreeChild* obj = c2->objects[i];
    if (obj->num_leafs == 1)
    {
      CS_ASSERT (obj->leafs[0] == c2);
      obj->leafs[0] = node;
      node->AddObject (obj);
      node->UpdateBBox (obj->bbox);
    }
    else
    {
      if (obj->FindLeaf (node) == -1)
      {
        obj->ReplaceLeaf (c2, node);
	node->AddObject (obj);
        node->UpdateBBox (obj->bbox);
      }
      else
      {
        obj->RemoveLeaf (c2);
      }
    }
  }
  delete[] c1->objects;
  c1->objects = NULL;
  c1->num_objects = 0;
  c1->max_objects = 0;
  delete[] c2->objects;
  c2->objects = NULL;
  c2->num_objects = 0;
  c2->max_objects = 0;
  delete c1;
  delete c2;
}

void csSimpleKDTree::Flatten ()
{
  if (!child1) return;	// Nothing to do.

  disallow_distribute = false;
  obj_bbox_valid = false;

  FlattenTo (this);
  return;
}

bool csSimpleKDTree::Front2Back (const csVector3& pos,
	csSimpleKDTreeVisitFunc* func,
  	void* userdata, uint32 cur_timestamp)
{
  CS_ASSERT (this != NULL);
  if (!func (this, userdata, cur_timestamp))
    return false;
  if (child1)
  {
    // There are children.
    if (pos[split_axis] <= split_location)
    {
      child1->Front2Back (pos, func, userdata, cur_timestamp);
      CS_ASSERT (child2 != NULL);
      child2->Front2Back (pos, func, userdata, cur_timestamp);
    }
    else
    {
      child2->Front2Back (pos, func, userdata, cur_timestamp);
      CS_ASSERT (child1 != NULL);
      child1->Front2Back (pos, func, userdata, cur_timestamp);
    }
  }
  return true;
}

void csSimpleKDTree::ResetTimestamps ()
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

void csSimpleKDTree::Front2Back (const csVector3& pos,
	csSimpleKDTreeVisitFunc* func,
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

