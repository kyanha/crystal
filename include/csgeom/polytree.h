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

#ifndef POLYTREE_H
#define POLYTREE_H

#include "csgeom/math3d.h"

class csPolygonInt;
class csPolygonParentInt;


/**
 * A general node in a polygon tree.
 */
class csPolygonTreeNode
{
public:
  /**
   * Constructor.
   */
  csPolygonTreeNode () { }

  /**
   * Destructor.
   */
  virtual ~csPolygonTreeNode () { }

  /// Remove all dynamic added polygons.
  virtual void RemoveDynamicPolygons () = 0;

  /// Return true if node is empty.
  virtual bool IsEmpty () = 0;
};

/**
 * Visit a node in a polygon tree. If this function returns non-NULL
 * the scanning will stop and the pointer will be returned.
 */
typedef void* (csTreeVisitFunc)(csPolygonParentInt*, csPolygonInt**,
	int num, void*);

/**
 * A general polygon tree. This is an abstract data type.
 * Concrete implementations like csBspTree or csOctree inherit
 * from this class.
 */
class csPolygonTree
{
protected:
  /// The root of the tree.
  csPolygonTreeNode* root;

  /// The parent that this tree is made for.
  csPolygonParentInt* pset;

  /// Clear the nodes.
  void Clear () { CHK (delete root); }

public:
  /**
   * Constructor.
   */
  csPolygonTree (csPolygonParentInt* ps) : root (NULL), pset (ps) { }

  /**
   * Destructor.
   */
  virtual ~csPolygonTree () { }

  /// Get the polygonset for this tree.
  csPolygonParentInt* GetParent () { return pset; }

  /**
   * Create the tree for the default parent set.
   */
  virtual void Build () = 0;

  /**
   * Create the tree with a given set of polygons.
   */
  virtual void Build (csPolygonInt** polygons, int num) = 0;

  /**
   * Add a bunch of polygons to the tree. They will be marked
   * as dynamic polygons so that you can remove them from the tree again
   * with RemoveDynamicPolygons(). Note that adding polygons dynamically
   * will not modify the existing tree and splits but instead continue
   * splitting in the leaves where the new polygons arrive.
   */
  virtual void AddDynamicPolygons (csPolygonInt** polygons, int num) = 0;

  /**
   * Remove all dynamically added polygons from the node. Note that
   * the polygons are not really destroyed. Only unlinked from the BSP
   * tree.
   */
  virtual void RemoveDynamicPolygons () = 0;

  /// Traverse the tree from back to front starting at the root and 'pos'.
  virtual void* Back2Front (const csVector3& pos, csTreeVisitFunc* func,
  	void* data) = 0;
  /// Traverse the tree from front to back starting at the root and 'pos'.
  virtual void* Front2Back (const csVector3& pos, csTreeVisitFunc* func,
  	void* data) = 0;
};

#endif /*POLYTREE_H*/

