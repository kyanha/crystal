/*
    Crystal Space 3D engine
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

#ifndef __CS_IGEOM_OBJMODEL_H__
#define __CS_IGEOM_OBJMODEL_H__

/**\file
 */

/**
 * \addtogroup geom_utils
 * @{ */
 
#include "csutil/scf.h"

struct iPolygonMesh;
struct iObjectModel;
class csBox3;

/**
 * \name Bounding box types
 * @{ */
 
/**
 * For iObjectModel::GetObjectBoundingBox() get a normal bounding box which
 * may or may not be recalculated depending on the changing geometry of
 * the object.
 */
#define CS_BBOX_NORMAL 0
/**
 * For iObjectModel::GetObjectBoundingBox() get a totally accurate bounding
 * box. Not all plugins support this. Some will just return a normal
 * bounding box.
 */
#define CS_BBOX_ACCURATE 1
/**
 * For iObjectModel::GetObjectBoundingBox() get the maximum bounding box
 * that this object will ever use. For objects that don't have a preset
 * maximum bounding box this just has to be a reasonable estimate of
 * a realistic maximum bounding box.
 */
#define CS_BBOX_MAX 2

/** @} */

SCF_VERSION (iObjectModelListener, 0, 0, 1);

/**
 * Implement this class if you're interested in hearing about
 * when the object model changes.
 */
struct iObjectModelListener : public iBase
{
  /// The object model has changed.
  virtual void ObjectModelChanged (iObjectModel* model) = 0;
};

SCF_VERSION (iObjectModel, 0, 3, 0);

/**
 * This interface represents data related to some geometry in object
 * space. It is a generic way to describe meshes in the engine. By using
 * this interface you can make sure your code works on all engine geometry.
 * The data returned by this class is in local object space.
 */
struct iObjectModel : public iBase
{
  /**
   * Returns a number that will change whenever the shape of this object
   * changes. If that happens then the data in all the returned polygon
   * meshes and bounding volumes will be invalid.
   */
  virtual long GetShapeNumber () const = 0;

  /**
   * Get a polygon mesh representing the basic geometry of the object.
   * Can return NULL if this object model doesn't support that.
   */
  virtual iPolygonMesh* GetPolygonMeshBase () = 0;

  /**
   * Get a polygon mesh representing the geometry of the object.
   * This mesh is useful for collision detection.
   * Can return NULL if this object model doesn't support that.
   */
  virtual iPolygonMesh* GetPolygonMeshColldet () = 0;

  /**
   * Set a polygon mesh representing the geometry of the object.
   * This mesh is useful for collision detection.
   * This can be used to replace the default polygon mesh returned
   * by GetPolygonMeshColldet() with one that has less detail or
   * even to support polygon mesh for mesh objects that otherwise don't
   * support it. The object model will keep a reference to the
   * given polymesh.
   */
  virtual void SetPolygonMeshColldet (iPolygonMesh* polymesh) = 0;

  /**
   * Get a polygon mesh specifically for visibility culling (to be used
   * as an occluder). This polygon mesh is guaranteed to be smaller or equal
   * to the real object. In other words: if you would render the original
   * mesh in red and this one in blue you should not see any blue anywhere.
   * This kind of lower detail version can be used for occlusion writing
   * in a visibility culling system.
   * Can return NULL if this object model doesn't support that. In that
   * case the object will not be used for visibility culling.
   */
  virtual iPolygonMesh* GetPolygonMeshViscull () = 0;

  /**
   * Set a polygon mesh representing the geometry of the object.
   * This mesh is useful for visibility culling.
   * This can be used to replace the default polygon mesh returned
   * by GetPolygonMeshViscull() with one that has less detail or
   * even to support polygon mesh for mesh objects that otherwise don't
   * support it. The object model will keep a reference to the
   * given polymesh.
   */
  virtual void SetPolygonMeshViscull (iPolygonMesh* polymesh) = 0;

  /**
   * Get a polygon mesh specifically for shadow casting (to be used by the
   * shadow manager). This polygon mesh is guaranteed to be smaller or equal
   * to the real object. In other words: if you would render the original
   * mesh in red and this one in blue you should not see any blue anywhere.
   * Can return NULL if this object model doesn't support that. In that
   * case the object will not be used for shadow casting.
   */
  virtual iPolygonMesh* GetPolygonMeshShadows () = 0;

  /**
   * Set a polygon mesh representing the geometry of the object.
   * This mesh is useful for shadow casting.
   * This can be used to replace the default polygon mesh returned
   * by GetPolygonMeshShadows() with one that has less detail or
   * even to support polygon mesh for mesh objects that otherwise don't
   * support it. The object model will keep a reference to the
   * given polymesh.
   */
  virtual void SetPolygonMeshShadows (iPolygonMesh* polymesh) = 0;

  /**
   * Create a polygon mesh representing a lower detail version of the
   * object but without the restrictions of GetPolygonMeshViscull().
   * The floating point input number is 0 for minimum detail and
   * 1 for highest detail. This function may return the same polygon
   * mesh as GetPolygonMeshColldet() (but with ref count incremented by one).
   * Can return NULL if this object model doesn't support that.
   */
  virtual csPtr<iPolygonMesh> CreateLowerDetailPolygonMesh (float detail) = 0;

  /**
   * Get the bounding box in object space for this mesh object.
   * Type has three possibilities:
   * <ul>
   * <li> #CS_BBOX_NORMAL: get a normal bounding box which
   *      may or may not be recalculated depending on the changing
   *      geometry of the object.
   * <li> #CS_BBOX_ACCURATE: get a totally accurate bounding
   *      box. Not all plugins support this. Some will just return a normal
   *      bounding box.
   * <li> #CS_BBOX_MAX: get the maximum bounding box
   *      that this object will ever use. For objects that don't have a
   *      preset maximum bounding box this just has to be a reasonable
   *      estimate of a realistic maximum bounding box.
   * </ul>
   */
  virtual void GetObjectBoundingBox (csBox3& bbox,
  	int type = CS_BBOX_NORMAL) = 0;

  /**
   * Get the radius and center of this object in object space.
   */
  virtual void GetRadius (csVector3& radius, csVector3& center) = 0;

  /**
   * Add a listener to this object model. This listener will be called whenever
   * the object model changes or right before it is destroyed.
   */
  virtual void AddListener (iObjectModelListener* listener) = 0;

  /**
   * Remove a listener from this object model.
   */
  virtual void RemoveListener (iObjectModelListener* listener) = 0;
};

/** @} */

#endif // __CS_IGEOM_OBJMODEL_H__

