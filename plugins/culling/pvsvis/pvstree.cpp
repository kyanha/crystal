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

#include "cssysdef.h"
#include <string.h>
#include "csutil/databuf.h"
#include "csutil/csendian.h"
#include "iutil/cache.h"
#include "iutil/objreg.h"
#include "iengine/engine.h"
#include "pvstree.h"
#include "pvsvis.h"

//-------------------------------------------------------------------------

csStaticPVSNode::csStaticPVSNode ()
{
  child1 = child2 = 0;
  invisible_number = 0;
}

csStaticPVSNode::~csStaticPVSNode ()
{
  delete child1;
  delete child2;
}

void csStaticPVSNode::PropagateBBox (const csBox3& box)
{
  node_bbox = box;
  if (child1 == 0 && child2 == 0) return;
  csBox3 box1, box2;
  switch (axis)
  {
    case 0:
      box1.Set (box.MinX (), box.MinY (), box.MinZ (),
      		where,       box.MaxY (), box.MaxZ ());
      box2.Set (where,       box.MinY (), box.MinZ (),
      		box.MaxX (), box.MaxY (), box.MaxZ ());
      break;
    case 1:
      box1.Set (box.MinX (), box.MinY (), box.MinZ (),
      		box.MaxX (), where,       box.MaxZ ());
      box2.Set (box.MinX (), where,       box.MinZ (),
      		box.MaxX (), box.MaxY (), box.MaxZ ());
      break;
    case 2:
      box1.Set (box.MinX (), box.MinY (), box.MinZ (),
      		box.MaxX (), box.MaxY (), where);
      box2.Set (box.MinX (), box.MinY (), where,
      		box.MaxX (), box.MaxY (), box.MaxZ ());
      break;
  }
  if (child1) child1->PropagateBBox (box1);
  if (child2) child2->PropagateBBox (box2);
}

void csStaticPVSNode::ResetTimestamps ()
{
  invisible_number = 0;
  size_t i;
  for (i = 0 ; i < objects.Length () ; i++)
    objects[i]->timestamp = 0;
  if (child1)
  {
    child1->ResetTimestamps ();
    child2->ResetTimestamps ();
  }
}

void csStaticPVSNode::Front2Back (const csVector3& pos,
	csPVSTreeVisitFunc* func,
  	void* userdata, uint32 cur_timestamp, uint32 frustum_mask)
{
  if (!func (this, userdata, cur_timestamp, frustum_mask))
    return;
  if (child1)
  {
    // There are children.
    if (pos[axis] <= where)
    {
      child1->Front2Back (pos, func, userdata, cur_timestamp, frustum_mask);
      CS_ASSERT (child2 != 0);
      child2->Front2Back (pos, func, userdata, cur_timestamp, frustum_mask);
    }
    else
    {
      child2->Front2Back (pos, func, userdata, cur_timestamp, frustum_mask);
      CS_ASSERT (child1 != 0);
      child1->Front2Back (pos, func, userdata, cur_timestamp, frustum_mask);
    }
  }
}

void csStaticPVSNode::TraverseRandom (csPVSTreeVisitFunc* func,
  	void* userdata, uint32 cur_timestamp, uint32 frustum_mask)
{
  if (!func (this, userdata, cur_timestamp, frustum_mask))
    return;
  if (child1)
  {
    // There are children.
    child1->TraverseRandom (func, userdata, cur_timestamp, frustum_mask);
    CS_ASSERT (child2 != 0);
    child2->TraverseRandom (func, userdata, cur_timestamp, frustum_mask);
  }
}

void csStaticPVSNode::AddObject (const csBox3& bbox,
	csPVSVisObjectWrapper* object)
{
  if (!child1)
  {
    // We are a leaf.
    objects.Push (object);
    object->parent_nodes.Push (this);
    return;
  }

  float mi = 0, ma = 0;
  switch (axis)
  {
    case 0: mi = bbox.MinX (); ma = bbox.MaxX (); break;
    case 1: mi = bbox.MinY (); ma = bbox.MaxY (); break;
    case 2: mi = bbox.MinZ (); ma = bbox.MaxZ (); break;
  }
  if (ma >= where)
    child2->AddObject (bbox, object);
  if (mi <= where)
    child1->AddObject (bbox, object);
}

void csStaticPVSNode::MarkInvisible (const csVector3& pos, uint32 cur_timestamp)
{
  size_t i;
  for (i = 0 ; i < invisible_nodes.Length () ; i++)
    invisible_nodes[i]->invisible_number = cur_timestamp;  
  if (!child1) return;

  if (pos[axis] <= where)
    child1->MarkInvisible (pos, cur_timestamp);
  else
    child2->MarkInvisible (pos, cur_timestamp);
}

//-------------------------------------------------------------------------

uint32 csStaticPVSTree::global_timestamp = 1;

SCF_IMPLEMENT_IBASE (csStaticPVSTree)
  SCF_IMPLEMENTS_INTERFACE (iStaticPVSTree)
SCF_IMPLEMENT_IBASE_END

csStaticPVSTree::csStaticPVSTree ()
{
  SCF_CONSTRUCT_IBASE (0);
  root = 0;
  node_minbox_set = false;
}

csStaticPVSTree::~csStaticPVSTree ()
{
  Clear ();
  SCF_DESTRUCT_IBASE();
}

void csStaticPVSTree::Clear ()
{
  delete root;
  root = 0;
  nodes_by_id.DeleteAll ();
}

csStaticPVSNode* csStaticPVSTree::CreateNode ()
{
  csStaticPVSNode* node = new csStaticPVSNode;
  node->id = nodes_by_id.Length ();
  nodes_by_id.Push (node);
  return node;
}

csStaticPVSNode* csStaticPVSTree::CheckOrCreateNode (uint32 id)
{
  if (id < nodes_by_id.Length () && nodes_by_id[id] != 0)
    return nodes_by_id[id];

  csStaticPVSNode* node = new csStaticPVSNode;
  node->id = id;
  if (id >= nodes_by_id.Length ())
  {
    nodes_by_id.SetLength (id+1, 0);
  }
  nodes_by_id[id] = node;
  return node;
}

void* csStaticPVSTree::CreateRootNode ()
{
  Clear ();
  root = CreateNode ();
  return (void*)root;
}

void csStaticPVSTree::SplitNode (void* parent, int axis, float where,
  	void*& child1, void*& child2)
{
  csStaticPVSNode* parent_node = (csStaticPVSNode*)parent;
  delete parent_node->child1;
  delete parent_node->child2;
  parent_node->child1 = CreateNode ();
  parent_node->child2 = CreateNode ();
  child1 = (void*)(parent_node->child1);
  child2 = (void*)(parent_node->child2);
  parent_node->axis = axis;
  parent_node->where = where;
}

void csStaticPVSTree::MarkInvisible (void* source, void* target)
{
  csStaticPVSNode* source_node = (csStaticPVSNode*)source;
  csStaticPVSNode* target_node = (csStaticPVSNode*)target;
  source_node->invisible_nodes.Push (target_node);
}

size_t csStaticPVSTree::CalculateSize (csStaticPVSNode* node)
{
  if (!node) return 0;
  return 4 + // id
  	 1 + // axis
  	 4 + // where
	 4 + // number of invisible nodes
	 4 * node->invisible_nodes.Length () +
	 1 + // child 1 available
	 CalculateSize (node->child1) +
	 1 + // child 2 available
	 CalculateSize (node->child2);
}

void csStaticPVSTree::WriteOut (char*& data, csStaticPVSNode* node)
{
  if (!node) return;
  csSetLittleEndianLong (data, node->id); data += 4;
  *data++ = node->axis;
  csSetLittleEndianFloat32 (data, node->where); data += 4;
  csSetLittleEndianLong (data, node->invisible_nodes.Length ()); data += 4;
  size_t i;
  for (i = 0 ; i < node->invisible_nodes.Length () ; i++)
  {
    csSetLittleEndianLong (data, node->invisible_nodes[i]->id); data += 4;
  }
  *data++ = node->child1 ? 1 : 0;
  WriteOut (data, node->child1);
  *data++ = node->child2 ? 1 : 0;
  WriteOut (data, node->child2);
}

bool csStaticPVSTree::WriteOut ()
{
  size_t total_len =
  	4 +	// marker
	CalculateSize (root);
  csDataBuffer* buf = new csDataBuffer (total_len);

  char* data = buf->GetData ();
  *data++ = 'P';
  *data++ = 'V';
  *data++ = 'S';
  *data++ = '1';
  WriteOut (data, root);

  csRef<iEngine> engine = CS_QUERY_REGISTRY (object_reg, iEngine);
  iCacheManager* cache_mgr = engine->GetCacheManager ();
  return cache_mgr->CacheData ((void*)buf->GetData (), buf->GetSize (),
  	"PVS", pvscache, 0);
}

const char* csStaticPVSTree::ReadPVS (char*& data, csStaticPVSNode*& node)
{
  uint32 id = csGetLittleEndianLong (data); data += 4;
  node = CheckOrCreateNode (id);
  node->axis = *data++;
  node->where = csGetLittleEndianFloat32 (data); data += 4;
  size_t inv_nodes = csGetLittleEndianLong (data); data += 4;
  if (inv_nodes)
  {
    node->invisible_nodes.SetLength (inv_nodes);
    size_t i;
    for (i = 0 ; i < inv_nodes ; i++)
    {
      uint32 node_id = csGetLittleEndianLong (data); data += 4;
      node->invisible_nodes[i] = CheckOrCreateNode (node_id);
    }
  }
  char child1_present = *data++;
  if (child1_present)
  {
    const char* err = ReadPVS (data, node->child1);
    if (err) return err;
  }
  char child2_present = *data++;
  if (child2_present)
  {
    const char* err = ReadPVS (data, node->child2);
    if (err) return err;
  }
  return 0;
}

const char* csStaticPVSTree::ReadPVS ()
{
  csRef<iEngine> engine = CS_QUERY_REGISTRY (object_reg, iEngine);
  iCacheManager* cache_mgr = engine->GetCacheManager ();
  csRef<iDataBuffer> buf = cache_mgr->ReadCache (
  	"PVS", pvscache, 0);
  if (!buf)
  {
    CreateRootNode ();
    root->PropagateBBox (root_box);
    return "Couldn't get PVS from cache manager!";
  }
  return ReadPVS (buf);
}

const char* csStaticPVSTree::ReadPVS (iDataBuffer* buf)
{
  Clear ();
  char* data = buf->GetData ();
  if (*data++ != 'P')
    return "File marker invalid! Probably not a PVS file!";
  if (*data++ != 'V')
    return "File marker invalid! Probably not a PVS file!";
  if (*data++ != 'S')
    return "File marker invalid! Probably not a PVS file!";
  if (*data++ != '1')
    return "File marker invalid! Could be wrong version of PVS file!";

  const char* err = ReadPVS (data, root);
  if (err) return err;

  root->PropagateBBox (root_box);
  return 0;
}

void csStaticPVSTree::AddObject (const csBox3& bbox,
	csPVSVisObjectWrapper* object)
{
  root->AddObject (bbox, object);
}

void csStaticPVSTree::RemoveObject (csPVSVisObjectWrapper* object)
{
  size_t i;
  csArray<csStaticPVSNode*>& parent_nodes = object->parent_nodes;
  for (i = 0 ; i < parent_nodes.Length () ; i++)
  {
    // @@@ Use a hash set for objects?
    parent_nodes[i]->objects.Delete (object);
  }
  parent_nodes.DeleteAll ();
}

void csStaticPVSTree::MoveObject (csPVSVisObjectWrapper* object,
	const csBox3& bbox)
{
  csArray<csStaticPVSNode*>& parent_nodes = object->parent_nodes;
  if (parent_nodes.Length () == 0)
  {
    // Object is not in tree. Add it.
    AddObject (bbox, object);
    return;
  }
  if (parent_nodes.Length () == 1)
  {
    // Special case. Object is only in one leaf. Check if we're
    // still there.
    if (parent_nodes[0]->node_bbox.Contains (bbox))
      return;	// Yes, nothing to do.
    // We no longer fit in that leaf.
    parent_nodes[0]->objects.Delete (object);
    parent_nodes.DeleteAll ();
    AddObject (bbox, object);
    return;
  }
  // General case.
  RemoveObject (object);
  AddObject (bbox, object);
}

void csStaticPVSTree::ResetTimestamps ()
{
  root->ResetTimestamps ();
}

uint32 csStaticPVSTree::NewTraversal ()
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
  return global_timestamp;
}

void csStaticPVSTree::Front2Back (const csVector3& pos,
	csPVSTreeVisitFunc* func,
  	void* userdata, uint32 frustum_mask)
{
  NewTraversal ();
  root->Front2Back (pos, func, userdata, global_timestamp, frustum_mask);
}

void csStaticPVSTree::TraverseRandom (csPVSTreeVisitFunc* func,
  	void* userdata, uint32 frustum_mask)
{
  NewTraversal ();
  root->TraverseRandom (func, userdata, global_timestamp, frustum_mask);
}

void csStaticPVSTree::MarkInvisible (const csVector3& pos, uint32 cur_timestamp)
{
  root->MarkInvisible (pos, cur_timestamp);
}

void csStaticPVSTree::SetBoundingBox (const csBox3& bbox)
{
  root_box = bbox;
  // If the node_minbox is not set we calculated it here from
  // the root box.
  if (!node_minbox_set)
  {
    node_minbox = (root_box.Max () - root_box.Min ()) / 20.0;
  }

  if (root)
    root->PropagateBBox (root_box);
}

void csStaticPVSTree::SetPVSCacheName (const char* pvscache)
{
  csStaticPVSTree::pvscache = pvscache;
}

