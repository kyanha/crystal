/*
    Copyright (C) 1998 by Jorrit Tyberghein
  
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
#include "csgeom/frustum.h"
#include "csgeom/poly2d.h"
#include "csengine/quadtree.h"
#include "isystem.h"

csQuadtreeNode::csQuadtreeNode ()
{
  children = NULL;
  state = CS_QUAD_EMPTY;
}

csQuadtreeNode::~csQuadtreeNode ()
{
  delete [] children;
}

//--------------------------------------------------------------------------

void csQuadtree::Build (csQuadtreeNode* node, const csBox2& box, int depth)
{
  node->SetCenter ((box.Min () + box.Max ())/2);
  if (depth <= 0) return;
  const csVector2& center = node->GetCenter ();

  csBox2 childbox;

  node->children = new csQuadtreeNode [4];

  csQuadtreeNode* children = node->children;

  childbox.Set (box.Min (), center);
  Build (&children[0], childbox, depth-1);

  childbox.Set (center.x, box.MinY (), box.MaxX (), center.y);
  Build (&children[1], childbox, depth-1);

  childbox.Set (center, box.Max ());
  Build (&children[2], childbox, depth-1);

  childbox.Set (box.MinX (), center.y, center.x, box.MaxY ());
  Build (&children[3], childbox, depth-1);
}

csQuadtree::csQuadtree (const csBox2& box, int depth)
{
  bbox = box;
  root = new csQuadtreeNode ();
  Build (root, box, depth-1);
}

csQuadtree::~csQuadtree ()
{
  delete root;
}

bool BoxEntirelyInPolygon (csVector2* verts, int num_verts, const csBox2& bbox)
{
  return (csPoly2D::In (verts, num_verts, bbox.GetCorner (0)) &&
          csPoly2D::In (verts, num_verts, bbox.GetCorner (1)) &&
          csPoly2D::In (verts, num_verts, bbox.GetCorner (2)) &&
          csPoly2D::In (verts, num_verts, bbox.GetCorner (3)));
}

bool csQuadtree::InsertPolygon (csQuadtreeNode* node,
	csVector2* verts, int num_verts,
	const csBox2& cur_bbox, const csBox2& pol_bbox)
{
  // If node is completely full already then nothing can happen.
  if (node->GetState () == CS_QUAD_FULL) return false;

  csQuadtreeNode* children = node->children;
  // If there are no children then this node is set to state partial.
  if (!children)
  {
    node->SetState (CS_QUAD_PARTIAL);
    return true;
  }

  // If there are children and we are empty then we propagate
  // the empty state to the children. This is because our
  // state is going to change so the children need to be valid.
  if (node->GetState () == CS_QUAD_EMPTY)
  {
    children[0].SetState (CS_QUAD_EMPTY);
    children[1].SetState (CS_QUAD_EMPTY);
    children[2].SetState (CS_QUAD_EMPTY);
    children[3].SetState (CS_QUAD_EMPTY);
  }

  csBox2 childbox;
  const csVector2& center = node->GetCenter ();
  bool vis, rc1, rc2, rc3, rc4;
  csVector2 v;
  rc1 = rc2 = rc3 = rc4 = false;

  // center_vis contains visibility info about the visibility
  // of the center inside the polygon.
  bool center_vis = csPoly2D::In (verts, num_verts, center);
  // Visibility information for the four central points of the sides
  // of every edge of the total area. We precompute this because
  // we're going to need this information anyway.
  v.x = cur_bbox.MinX ();
  v.y = center.y;
  bool left_vis = csPoly2D::In (verts, num_verts, v);
  v.x = cur_bbox.MaxX ();
  bool right_vis = csPoly2D::In (verts, num_verts, v);
  v.x = center.x;
  v.y = cur_bbox.MinY ();
  bool top_vis = csPoly2D::In (verts, num_verts, v);
  v.y = cur_bbox.MaxY ();
  bool bottom_vis = csPoly2D::In (verts, num_verts, v);

  // Check the bounding box of the polygon against all four
  // child areas (by comparing the bounding box against the center).
  // If the bounding box overlaps the child area then we continue
  // the check to see if the polygon also overlaps the child area.

  // Child 0 (top/left).
  if (children[0].GetState () != CS_QUAD_FULL &&
  	pol_bbox.MinX () <= center.x && pol_bbox.MinY () <= center.y)
  {
    vis = false;
    // If any of the three corners is in polygon then polygon is visible.
    if (center_vis || left_vis || top_vis) vis = true;
    // If bbox is entirely in child area then polygon is visible.
    else if (pol_bbox.MaxX () <= center.x && pol_bbox.MaxY () <= center.y) vis = true;
    // If bbox crosses child area vertically but does not cross
    // it horizontally (both left and right) then polygon is visible.
    else if (pol_bbox.MaxX () <= center.x && pol_bbox.MinX () >= cur_bbox.MinX ()) vis = true;
    else if (pol_bbox.MaxY () <= center.y && pol_bbox.MinY () >= cur_bbox.MinY ()) vis = true;
    // Most general case: just do the intersection test for the area and the polygon.
    else vis = csBox2::Intersect (cur_bbox.Min (), center, verts, num_verts);

    if (vis)
    {
      // We already calculated wether or not three of the four corners
      // of the child area are inside the polygon. We only need
      // to test the fourth one. If all are in the polygon then
      // the node is full and we can stop recursion.
      if (center_vis && left_vis && top_vis && csPoly2D::In (verts, num_verts, cur_bbox.Min ()))
      {
        children[0].SetState (CS_QUAD_FULL);
	rc1 = true;
      }
      else
      {
        childbox.Set (cur_bbox.Min (), center);
        rc1 = InsertPolygon (&children[0], verts, num_verts, childbox, pol_bbox);
      }
    }
  }

  // Child 1 (top/right).
  if (children[1].GetState () != CS_QUAD_FULL &&
  	pol_bbox.MaxX () > center.x && pol_bbox.MinY () <= center.y)
  {
    vis = false;
    if (center_vis || right_vis || top_vis) vis = true;
    else if (pol_bbox.MinX () >= center.x && pol_bbox.MaxY () <= center.y) vis = true;
    else if (pol_bbox.MinX () >= center.x && pol_bbox.MaxX () <= cur_bbox.MaxX ()) vis = true;
    else if (pol_bbox.MaxY () <= center.y && pol_bbox.MinY () >= cur_bbox.MinY ()) vis = true;
    else vis = csBox2::Intersect (center.x, cur_bbox.MinY (), cur_bbox.MaxX (), center.y, verts, num_verts);

    if (vis)
    {
      v.x = cur_bbox.MaxX ();
      v.y = cur_bbox.MinY ();
      if (center_vis && right_vis && top_vis && csPoly2D::In (verts, num_verts, v))
      {
        children[1].SetState (CS_QUAD_FULL);
	rc2 = true;
      }
      else
      {
        childbox.Set (center.x, cur_bbox.MinY (), cur_bbox.MaxX (), center.y);
        rc2 = InsertPolygon (&children[1], verts, num_verts, childbox, pol_bbox);
      }
    }
  }

  // Child 2 (bottom/right).
  if (children[2].GetState () != CS_QUAD_FULL &&
  	pol_bbox.MaxX () > center.x && pol_bbox.MaxY () > center.y)
  {
    vis = false;
    if (center_vis || right_vis || bottom_vis) vis = true;
    else if (pol_bbox.MinX () >= center.x && pol_bbox.MinY () >= center.y) vis = true;
    else if (pol_bbox.MinX () >= center.x && pol_bbox.MaxX () <= cur_bbox.MaxX ()) vis = true;
    else if (pol_bbox.MinY () >= center.y && pol_bbox.MaxY () <= cur_bbox.MaxY ()) vis = true;
    else vis = csBox2::Intersect (center, cur_bbox.Max (), verts, num_verts);

    if (vis)
    {
      if (center_vis && right_vis && bottom_vis && csPoly2D::In (verts, num_verts, cur_bbox.Max ()))
      {
        children[2].SetState (CS_QUAD_FULL);
	rc3 = true;
      }
      else
      {
        childbox.Set (center, cur_bbox.Max ());
        rc3 = InsertPolygon (&children[2], verts, num_verts, childbox, pol_bbox);
      }
    }
  }

  // Child 3 (bottom/left).
  if (children[3].GetState () != CS_QUAD_FULL &&
  	pol_bbox.MinX () <= center.x && pol_bbox.MaxY () > center.y)
  {
    vis = false;
    if (center_vis || left_vis || bottom_vis) vis = true;
    else if (pol_bbox.MaxX () <= center.x && pol_bbox.MinY () >= center.y) vis = true;
    else if (pol_bbox.MaxX () <= center.x && pol_bbox.MinX () >= cur_bbox.MinX ()) vis = true;
    else if (pol_bbox.MinY () >= center.y && pol_bbox.MaxY () <= cur_bbox.MaxY ()) vis = true;
    else vis = csBox2::Intersect (cur_bbox.MinX (), center.y, center.x, cur_bbox.MaxY (), verts, num_verts);

    if (vis)
    {
      v.x = cur_bbox.MinX ();
      v.y = cur_bbox.MaxY ();
      if (center_vis && left_vis && bottom_vis && csPoly2D::In (verts, num_verts, v))
      {
        children[3].SetState (CS_QUAD_FULL);
	rc4 = true;
      }
      else
      {
  	childbox.Set (cur_bbox.MinX (), center.y, center.x, cur_bbox.MaxY ());
        rc4 = InsertPolygon (&children[3], verts, num_verts, childbox, pol_bbox);
      }
    }
  }

  if (children[0].GetState () == CS_QUAD_FULL &&
      children[1].GetState () == CS_QUAD_FULL &&
      children[2].GetState () == CS_QUAD_FULL &&
      children[3].GetState () == CS_QUAD_FULL)
  {
    node->SetState (CS_QUAD_FULL);
    return true;
  }

  if (rc1 || rc2 || rc3 || rc4)
  {
    node->SetState (CS_QUAD_PARTIAL);
    return true;
  }
  return false;
}

bool csQuadtree::InsertPolygon (csVector2* verts, int num_verts, const csBox2& pol_bbox)
{
  // If root is already full then there is nothing that can happen further.
  if (root->GetState () == CS_QUAD_FULL) return false;

  // If the bounding box of the tree does not overlap with the bounding box of
  // the polygon then we can return false here.
  if (!bbox.Overlap (pol_bbox)) return false;

  // If bounding box of tree is completely inside bounding box of polygon then
  // it is possible that tree is completely in polygon. We test that condition
  // further.
  if (bbox < pol_bbox)
  {
    if (BoxEntirelyInPolygon (verts, num_verts, bbox))
    {
      // Polygon completely covers tree. In that case set state
      // of tree to full and return true.
      root->SetState (CS_QUAD_FULL);
      return true;
    }
  }

  return InsertPolygon (root, verts, num_verts, bbox, pol_bbox);
}

bool csQuadtree::TestPolygon (csQuadtreeNode* node,
	csVector2* verts, int num_verts,
	const csBox2& cur_bbox, const csBox2& pol_bbox)
{
  // If node is completely full already then nothing can happen.
  if (node->GetState () == CS_QUAD_FULL) return false;
  // If node is completely empty then polygon is always visible.
  if (node->GetState () == CS_QUAD_EMPTY) return true;

  csQuadtreeNode* children = node->children;
  // If there are no children then we assume polygon is not visible.
  // This is an optimization which is not entirely correct@@@
  if (!children) return false;

  csBox2 childbox;
  const csVector2& center = node->GetCenter ();
  bool vis;
  csVector2 v;

  // center_vis contains visibility info about the visibility
  // of the center inside the polygon.
  bool center_vis = csPoly2D::In (verts, num_verts, center);
  // Visibility information for the four central points of the sides
  // of every edge of the total area. We precompute this because
  // we're going to need this information anyway.
  v.x = cur_bbox.MinX ();
  v.y = center.y;
  bool left_vis = csPoly2D::In (verts, num_verts, v);
  v.x = cur_bbox.MaxX ();
  bool right_vis = csPoly2D::In (verts, num_verts, v);
  v.x = center.x;
  v.y = cur_bbox.MinY ();
  bool top_vis = csPoly2D::In (verts, num_verts, v);
  v.y = cur_bbox.MaxY ();
  bool bottom_vis = csPoly2D::In (verts, num_verts, v);

  // Check the bounding box of the polygon against all four
  // child areas (by comparing the bounding box against the center).
  // If the bounding box overlaps the child area then we continue
  // the check to see if the polygon also overlaps the child area.

  // Child 0 (top/left).
  if (children[0].GetState () != CS_QUAD_FULL &&
  	pol_bbox.MinX () <= center.x && pol_bbox.MinY () <= center.y)
  {
    vis = false;
    // If any of the three corners is in polygon then polygon is visible.
    if (center_vis || left_vis || top_vis) vis = true;
    // If bbox is entirely in child area then polygon is visible.
    else if (pol_bbox.MaxX () <= center.x && pol_bbox.MaxY () <= center.y) vis = true;
    // If bbox crosses child area vertically but does not cross
    // it horizontally (both left and right) then polygon is visible.
    else if (pol_bbox.MaxX () <= center.x && pol_bbox.MinX () >= cur_bbox.MinX ()) vis = true;
    else if (pol_bbox.MaxY () <= center.y && pol_bbox.MinY () >= cur_bbox.MinY ()) vis = true;
    // Most general case: just do the intersection test for the area and the polygon.
    else vis = csBox2::Intersect (cur_bbox.Min (), center, verts, num_verts);

    if (vis)
    {
      // We already calculated wether or not three of the four corners
      // of the child area are inside the polygon. We only need
      // to test the fourth one. If all are in the polygon then
      // we cover the node and we can stop recursion. Polygon is
      // visible in that case becase node is not full.
      if (center_vis && left_vis && top_vis && csPoly2D::In (verts, num_verts, cur_bbox.Min ()))
      {
        return true;
      }
      else
      {
        childbox.Set (cur_bbox.Min (), center);
        if (TestPolygon (&children[0], verts, num_verts, childbox, pol_bbox)) return true;
      }
    }
  }

  // Child 1 (top/right).
  if (children[1].GetState () != CS_QUAD_FULL &&
  	pol_bbox.MaxX () > center.x && pol_bbox.MinY () <= center.y)
  {
    vis = false;
    if (center_vis || right_vis || top_vis) vis = true;
    else if (pol_bbox.MinX () >= center.x && pol_bbox.MaxY () <= center.y) vis = true;
    else if (pol_bbox.MinX () >= center.x && pol_bbox.MaxX () <= cur_bbox.MaxX ()) vis = true;
    else if (pol_bbox.MaxY () <= center.y && pol_bbox.MinY () >= cur_bbox.MinY ()) vis = true;
    else vis = csBox2::Intersect (center.x, cur_bbox.MinY (), cur_bbox.MaxX (), center.y, verts, num_verts);

    if (vis)
    {
      v.x = cur_bbox.MaxX ();
      v.y = cur_bbox.MinY ();
      if (center_vis && right_vis && top_vis && csPoly2D::In (verts, num_verts, v))
      {
        return true;
      }
      else
      {
        childbox.Set (center.x, cur_bbox.MinY (), cur_bbox.MaxX (), center.y);
        if (TestPolygon (&children[1], verts, num_verts, childbox, pol_bbox)) return true;
      }
    }
  }

  // Child 2 (bottom/right).
  if (children[2].GetState () != CS_QUAD_FULL &&
  	pol_bbox.MaxX () > center.x && pol_bbox.MaxY () > center.y)
  {
    vis = false;
    if (center_vis || right_vis || bottom_vis) vis = true;
    else if (pol_bbox.MinX () >= center.x && pol_bbox.MinY () >= center.y) vis = true;
    else if (pol_bbox.MinX () >= center.x && pol_bbox.MaxX () <= cur_bbox.MaxX ()) vis = true;
    else if (pol_bbox.MinY () >= center.y && pol_bbox.MaxY () <= cur_bbox.MaxY ()) vis = true;
    else vis = csBox2::Intersect (center, cur_bbox.Max (), verts, num_verts);

    if (vis)
    {
      if (center_vis && right_vis && bottom_vis && csPoly2D::In (verts, num_verts, cur_bbox.Max ()))
      {
        return true;
      }
      else
      {
        childbox.Set (center, cur_bbox.Max ());
        if (TestPolygon (&children[2], verts, num_verts, childbox, pol_bbox)) return true;
      }
    }
  }

  // Child 3 (bottom/left).
  if (children[3].GetState () != CS_QUAD_FULL &&
  	pol_bbox.MinX () <= center.x && pol_bbox.MaxY () > center.y)
  {
    vis = false;
    if (center_vis || left_vis || bottom_vis) vis = true;
    else if (pol_bbox.MaxX () <= center.x && pol_bbox.MinY () >= center.y) vis = true;
    else if (pol_bbox.MaxX () <= center.x && pol_bbox.MinX () >= cur_bbox.MinX ()) vis = true;
    else if (pol_bbox.MinY () >= center.y && pol_bbox.MaxY () <= cur_bbox.MaxY ()) vis = true;
    else vis = csBox2::Intersect (cur_bbox.MinX (), center.y, center.x, cur_bbox.MaxY (), verts, num_verts);

    if (vis)
    {
      v.x = cur_bbox.MinX ();
      v.y = cur_bbox.MaxY ();
      if (center_vis && left_vis && bottom_vis && csPoly2D::In (verts, num_verts, v))
      {
        return true;
      }
      else
      {
  	childbox.Set (cur_bbox.MinX (), center.y, center.x, cur_bbox.MaxY ());
        if (TestPolygon (&children[3], verts, num_verts, childbox, pol_bbox)) return true;
      }
    }
  }

  return false;
}

bool csQuadtree::TestPolygon (csVector2* verts, int num_verts, const csBox2& pol_bbox)
{
  // If root is already full then there is nothing that can happen further.
  if (root->GetState () == CS_QUAD_FULL) return false;

  // If the bounding box of the tree does not overlap with the bounding box of
  // the polygon then we can return false here.
  if (!bbox.Overlap (pol_bbox)) return false;

  // If bounding box of tree is completely inside bounding box of polygon then
  // it is possible that tree is completely in polygon. We test that condition
  // further.
  if (bbox < pol_bbox)
  {
    if (BoxEntirelyInPolygon (verts, num_verts, bbox))
    {
      // Polygon completely covers tree. In that case return
      // true because polygon will be visible (node is not full).
      return true;
    }
  }

  return TestPolygon (root, verts, num_verts, bbox, pol_bbox);
}

int csQuadtree::TestPoint (csQuadtreeNode* node, const csVector2& point)
{
  csQuadtreeNode* children = node->children;
  if (!children) return node->GetState ();

  const csVector2& center = node->GetCenter ();
  if (point.x <= center.x)
  {
    if (point.y <= center.y)
    {
      if (children[0].GetState () != CS_QUAD_PARTIAL) return children[0].GetState ();
      return TestPoint (&children[0], point);
    }
    else
    {
      if (children[3].GetState () != CS_QUAD_PARTIAL) return children[3].GetState ();
      return TestPoint (&children[3], point);
    }
  }
  else
  {
    if (point.y <= center.y)
    {
      if (children[1].GetState () != CS_QUAD_PARTIAL) return children[1].GetState ();
      return TestPoint (&children[1], point);
    }
    else
    {
      if (children[2].GetState () != CS_QUAD_PARTIAL) return children[2].GetState ();
      return TestPoint (&children[2], point);
    }
  }
  return CS_QUAD_EMPTY;
}


int csQuadtree::TestPoint (const csVector2& point)
{
  if (!bbox.In (point)) return CS_QUAD_UNKNOWN;
  if (root->GetState () != CS_QUAD_PARTIAL) return root->GetState ();
  return TestPoint (root, point);
}

/*----------------------------------------------------------------*/
/* Wouter's Wild QuadTree implementation */

#if 0

/// computes 2**x
static int Pow2(int x)
{
  int res = 1;
  for(int i=0; i<x; i++)
    res <<= 1;
  return res;
}

WWQuadTree :: WWQuadTree (const csBox2& the_box, int the_depth)
{
  bbox = the_box;
  depth= the_depth;
  root_state = CS_QUAD_EMPTY;
  if(depth < 1)
  {
    CsPrintf(MSG_FATAL_ERROR, "QuadTree: Depth too small\n");
    exit(1);
  }
  /// first calculate the number of nodes.
  /// each depth 4* the nodes at the previous depth are added.
  /// depth 1 has the root node.
  int nr_leaves = 1;
  int nr_nodes = 0;
  for(int i=1; i<=depth; i++)
  {
    nr_nodes += nr_leaves;
    nr_leaves *= 4;
  }
  /// 4 nodes per byte, the root node is stored seperately
  state_size = (nr_nodes - 1) / 4;
  CsPrintf(MSG_STDOUT, "QuadTree: depth %d, statesize %d bytes\n", depth,
    state_size);

  if(state_size > 0)
  {
    states = new unsigned char[state_size];
    /// and every node is EMPTY at the start
    memset(states, CS_QUAD_ALL_EMPTY, state_size);  
  }
  else states = NULL;
}


void WWQuadTree :: CallChildren(quad_traverse_func func, int offset, 
  int node_nr, void *data, int& rc1, int& rc2, int& rc3, int& rc4)
{

}


WWQuadTree :: ~WWQuadTree ()
{
  delete[] states;
}


bool WWQuadTree :: InsertPolygon (csVector2* verts, int num_verts,
  const csBox2& pol_bbox)
{
}


bool WWQuadTree :: TestPolygon (csVector2* verts, int num_verts,
  const csBox2& pol_bbox)
{
}


int WWQuadTree :: TestPoint (const csVector2& point)
{
  if(root_state == CS_QUAD_FULL || root_state == CS_QUAD_EMPTY) 
    return root_state;
}

#endif
