/*
    Copyright (C) 1998-2001 by Jorrit Tyberghein
  
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

#ifndef __IENGINE_THING_H__
#define __IENGINE_THING_H__

#include "csutil/scf.h"

class csVector3;
class csMatrix3;
class csThing;
struct iSector;
struct iMovable;
struct iPolygon3D;
struct iGraphics3D;
struct iFrustumView;
struct iCurve;
struct iMaterialWrapper;

/**
 * If CS_THING_VISTREE is set then an octree will be calculated for the
 * polygons in this thing. In this case the thing will implement a
 * fully working iVisibilityCuller which the sector can use.
 */
#define CS_THING_VISTREE 1

/**
 * The following flags affect movement options for a thing. See
 * SetMovingOption() for more info.
 */
#define CS_THING_MOVE_NEVER 0
#define CS_THING_MOVE_OFTEN 1
#define CS_THING_MOVE_OCCASIONAL 2


SCF_VERSION (iThing, 0, 0, 4);

/**
 * This is the generalized interface to Things.<p>
 * A thing is a 3D model, in most cases non-moveable, which is mostly used
 * for details and also for walls of a sector.
 */
struct iThing : public iBase
{
  /// @@@ Used by the engine to retrieve internal thing object (ugly)
  virtual csThing *GetPrivateObject () = 0;

  /// Get polygon set name
  virtual const char *GetName () const = 0;
  /// Set polygon set name
  virtual void SetName (const char *iName) = 0;

  /**
   * Compress the vertex table so that all nearly identical vertices
   * are compressed. The polygons in the set are automatically adapted.
   * This function can be called at any time in the creation of the object
   * and it can be called multiple time but it normally only makes sense
   * to call this function after you have finished adding all polygons
   * and all vertices.<p>
   * Note that calling this function will make the camera vertex array
   * invalid.
   */
  virtual void CompressVertices () = 0;

  /// Query number of polygons in set
  virtual int GetPolygonCount () = 0;
  /// Get a polygon from set by his index
  virtual iPolygon3D *GetPolygon (int idx) = 0;
  /// Create a new polygon and return a pointer to it
  virtual iPolygon3D *CreatePolygon (const char *iName) = 0;

  /// Query number of vertices in set
  virtual int GetVertexCount () = 0;
  /// Get the given vertex coordinates in object space
  virtual csVector3 &GetVertex (int idx) = 0;
  /// Get the given vertex coordinates in world space
  virtual csVector3 &GetVertexW (int idx) = 0;
  /// Get the given vertex coordinates in camera space
  virtual csVector3 &GetVertexC (int idx) = 0;
  /// Create a vertex given his object-space coords and return his index
  virtual int CreateVertex (csVector3 &iVertex) = 0;

  /// Create a key/value pair object
  virtual bool CreateKey (const char *iName, const char *iValue) = 0;

  /// Get the movable for this thing.
  virtual iMovable* GetMovable () = 0;
};

SCF_VERSION (iThingState, 0, 0, 2);

/**
 * This is the state interface to access the internals of a thing
 * mesh object and factory (both).
 */
struct iThingState : public iBase
{
  /**
   * Compress the vertex table so that all nearly identical vertices
   * are compressed. The polygons in the set are automatically adapted.
   * This function can be called at any time in the creation of the object
   * and it can be called multiple time but it normally only makes sense
   * to call this function after you have finished adding all polygons
   * and all vertices.<p>
   * Note that calling this function will make the camera vertex array
   * invalid.
   */
  virtual void CompressVertices () = 0;

  /// Query number of polygons in set
  virtual int GetPolygonCount () = 0;
  /// Get a polygon from set by his index
  virtual iPolygon3D *GetPolygon (int idx) = 0;
  /// Create a new polygon and return a pointer to it
  virtual iPolygon3D *CreatePolygon (const char *iName = NULL) = 0;

  /// Query number of vertices in set
  virtual int GetVertexCount () = 0;
  /// Get the given vertex coordinates in object space
  virtual csVector3 &GetVertex (int idx) = 0;
  /// Get the given vertex coordinates in world space
  virtual csVector3 &GetVertexW (int idx) = 0;
  /// Get the given vertex coordinates in camera space
  virtual csVector3 &GetVertexC (int idx) = 0;
  /// Create a vertex given his object-space coords and return his index
  virtual int CreateVertex (csVector3 &iVertex) = 0;
  /**
   * Prepare the lightmaps for all polys so that they are suitable
   * for the 3D rasterizer.
   */
  virtual void CreateLightMaps (iGraphics3D* g3d) = 0;
  /**
   * Init the lightmaps for all polygons in this thing.
   */
  virtual void InitLightMaps (bool do_cache = true) = 0;
  /**
   * Cache the lightmaps for all polygons in this thing.
   */
  virtual void CacheLightMaps () = 0;
  /**
   * Check frustum visibility on this thing.
   * First initialize the 2D culler cube.
   * @@@ Does this belong here?
   */
  virtual void CheckFrustum (iFrustumView* fview) = 0;

  /// Get the flags for this thing.
  virtual unsigned GetFlags () = 0;
  /// Set any number of flags for this thing.
  virtual void SetFlags (unsigned iMask, unsigned iValue) = 0;

  /**
   * Get the moving option.
   */
  virtual int GetMovingOption () = 0;

  /**
   * Control how this thing will be moved.
   * There are currently three options.
   * <ul>
   *   <li>CS_THING_MOVE_NEVER: this option is set for a thing that cannot
   *       move at all. In this case the movable will be ignored and only
   *       hard transforms can be used to move a thing with this flag. This
   *       setting is both efficient for memory (object space coordinates are
   *       equal to world space coordinates so only one array is kept) and
   *       render speed (only the camera transform is needed). This option
   *       is very useful for static geometry like walls.
   *       This option is default.
   *   <li>CS_THING_MOVE_OCCASIONAL: this option is set for a thing that
   *       is movable but doesn't move all the time usually. Setting this
   *       option means that the world space vertices will be cached (taking
   *       up more memory that way) but the coordinates will be recalculated
   *       only at rendertime (and cached at that time). This option has
   *       the same speed efficiency as MOVE_NEVER when the object doesn't
   *       move but more memory is used as all the vertices are duplicated.
   *       Use this option for geometry that is not too big (in number of
   *       vertices) and only moves occasionally like doors of elevators.
   *   <li>CS_THING_MOVE_OFTEN: this option is set for a thing that moves
   *       very often (i.e. almost every frame). Setting this option means
   *       that the object->world and camera transformations will be combined
   *       at render time. It has the same memory efficiency as MOVE_NEVER
   *       but the transforms need to be combined every frame (if the object
   *       is visible). Use this option for geometry that moves a lot. Also
   *       very useful for objects that often move and have lots of vertices
   *       since in that case combining the transforms ones is a lot more
   *       efficient than doing two transforms on every vertex.
   * </ul>
   */
  virtual void SetMovingOption (int opt) = 0;

  /**
   * Get the center of the curves.
   */
  virtual const csVector3& GetCurvesCenter () const = 0;
  /**
   * Set the center of the curves.
   */
  virtual void SetCurvesCenter (const csVector3& cen) = 0;

  /**
   * Get the scale of the curves.
   */
  virtual float GetCurvesScale () = 0;
  /**
   * Set the scale of the curves.
   */
  virtual void SetCurvesScale (float scale) = 0;

  /// Get the number of curves.
  virtual int GetNumCurves () = 0;
  /// Get the curve.
  virtual iCurve* GetCurve (int idx) = 0;
  /// Get the number of curve vertices.
  virtual int GetNumCurveVertices () = 0;
  /// Get the specified curve vertex.
  virtual csVector3& CurveVertex (int i) = 0;
  /// Get the curve vertices.
  virtual csVector3* GetCurveVertices () = 0;
  /// Get the specified curve texture coordinate (texel).
  virtual csVector2& CurveTexel (int i) = 0;

  /**
   * Add polygons and vertices from the specified thing (seen as template).
   */
  virtual void MergeTemplate (iThingState* tpl,
  	iMaterialWrapper* default_material = NULL,
	csVector3* shift = NULL, csMatrix3* transform = NULL) = 0;
};

#endif
