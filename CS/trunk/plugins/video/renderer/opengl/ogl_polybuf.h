/*
    Copyright (C) 2001 by Jorrit Tyberghein

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

#ifndef __CS_OGL_POLYBUF_H__
#define __CS_OGL_POLYBUF_H__

#include "csgeom/vector3.h"
#include "csgeom/plane3.h"
#include "csgeom/matrix3.h"
#include "csutil/garray.h"
#include "csutil/refarr.h"
#include "plugins/video/renderer/common/vbufmgr.h"
#include "ivideo/graph3d.h"
#include "ivideo/material.h"
#include "csutil/cscolor.h"
#include "csgeom/subrec.h"

#include "ogl_g3dcom.h"

class csSLMCacheData;

class csTrianglesPerMaterial
{
public:
  csTrianglesPerMaterial * next;
  int matIndex;
  csRef<iRendererLightmap> lmh;

  // We need a better implementation here
  // We're duplicating info, but we need the number of vertices per
  // material, so later we can call ClipTriangleMesh

  csDirtyAccessArray<csTriangle> triangles;

  csTrianglesPerMaterial ();

  ~csTrianglesPerMaterial ();

  void ClearVertexArray ();

  /// Return the number of triangles
  int TriangleCount () const { return triangles.Length (); }
};


class TrianglesList
{
public:
  csTrianglesPerMaterial* first;
  csTrianglesPerMaterial* last;

  TrianglesList ();
  ~TrianglesList ();
  int GetLastMaterial ()
  {
    if (last == 0) return -1;
    return last->matIndex;
  }
  iRendererLightmap* GetLastLMHandle ()
  {
    if (last == 0) return 0;
    return last->lmh;
  }
  void Add (csTrianglesPerMaterial* t);
  csTrianglesPerMaterial* GetLast () { return last; }
};

/**
 * This class stores triangles that could share the same superlightmap
 */
/*class csTrianglesPerSuperLightmap
{
public:
  csTrianglesPerSuperLightmap* prev;

  /// triangles which shares the same superlightmap
  csDirtyAccessArray<csTriangle> triangles;

  /// The lightmaps in the superlightmap
  csRefArray<iPolygonTexture> lightmaps;
  csDirtyAccessArray<csRGBpixel*> lm_info;

  csDirtyAccessArray<csRect> rectangles;

  csSubRectangles* region;

  csTrianglesPerSuperLightmap (int size, int queue_num);
  ~csTrianglesPerSuperLightmap ();

  /// Pointer to the cache data
  csSLMCacheData* cacheData;
  bool isUnlit;
  csLightmapHandle lmh;

  /// Number of the queue to use for this super lightmap.
  int queue_num;

  // Checks if the superlightmap is initialized or not
  bool initialized;

  // Cost of this super lightmap. A high number means that
  // this super lightmap requires a lot of work to update. This number
  // is related to the number of lightmaps in it and the total area.
  int cost;
  int CalculateCost ();

  // Timestamp indicating when this super lightmap was last used.
  uint32 timestamp;
};*/

/**
 * Single Linked List that stores all the lightmap's triangles and uv's.
 * every node strores all the lightmaps that could share the same
 * superlightmap and all the triangles and uv needed for lighting
 * with that superlightmap.
 * The list goes from the last to the first (as a stack would do, but without
 * popping)
 */
/*class TrianglesSuperLightmapList
{
public:
  csTrianglesPerSuperLightmap* first;
  csTrianglesPerSuperLightmap* last;

  TrianglesSuperLightmapList ();
  ~TrianglesSuperLightmapList ();
  void Add (csTrianglesPerSuperLightmap* t);
  csTrianglesPerSuperLightmap* GetLast () { return last; }

  // Dirty due dynamic lights, needs recalculating.
  bool dirty;

  // Marks as dirty.
  void MarkLightmapsDirty () { dirty = true; }
  // Clear the dirty state.
  void ClearLightmapsDirty() { dirty = false; }
  // Gets the dirty state.
  bool GetLightmapsDirtyState () const { return dirty; }
};*/


/**
 * This structure has enough information so that we can later
 * construct the super lightmaps.
 */
struct csLmQueue
{
  iPolygonTexture* polytext;
  csVector2* uv;	// Texture coordinates.
  int num_uv;
  int vt_idx;		// Index to continue adding lumels.
};

/**
 * This implementation is optimized to use glDrawElements.
 * It groups polygons per materials
 */
class csTriangleArrayPolygonBuffer : public csPolygonBuffer
{
private:
  /*
   * Add a single vertex to the given tri/mat buffer.
   * Returns vertex index.
   */
/*  int AddSingleVertex (csTrianglesPerMaterial* pol,
	int* verts, int i, const csVector2& uv, int& cur_vt_idx);*/

  /*
   * Add a single vertex to the given suplm.
   * Returns vertex index.
   */
  //int AddSingleVertexLM (const csVector2& uvLightmap, int& cur_vt_idx);

  // Queue with lightmaps we still have to process.
  //csDirtyAccessArray<csLmQueue> lmqueue;

  //void ClearLmQueue ();
  //void AddLmQueue (iPolygonTexture* polytext, const csVector2* uv,
		  //int num_uv, int vt_idx);
  // size is the size of the super lightmap to use.
  //void ProcessLmQueue (iPolygonTexture* polytext, const csVector2* uv,
		  //int num_uv, int vt_idx, int size, int queue_num);
public:
  // SuperLightMap list.
//  TrianglesSuperLightmapList superLM;

protected:
  // Mesh triangles grouped by material list.
  TrianglesList polygons;

  csRefArray<iMaterialHandle> materials;

  /**
   * Vertices per triangle (every vertex is duplicated here for every
   * triangle in the list).
   */
  csDirtyAccessArray<csVector3> vec_vertices;
  /// Texels for those triangles
  csDirtyAccessArray<csVector2> texels;
  /// Lumels for those triangles
  csDirtyAccessArray<csVector2> lumels;

  int matCount;
  csDirtyAccessArray<csTriangle> orig_triangles;
  csVector3* vertices;
  int num_vertices;

  csBox3 bbox;

  /**
   * Search a superlightmap to fit the lighmap in the superLM list
   * if it can't find any it creates a new one.
   * The case that the polygon has no superlightmap is supported too.
   * If the polygontexture has no lightmap it means its not lighted,
   * then a special superlightmap has to be created, just to store
   * the triangles and vertices that will be used in fog.
   * 'size' is the size of the super lightmap to use.
   * 'queue_num' is the number of the super lightmap queue to use (0 to 3).
   */
/*  csTrianglesPerSuperLightmap* SearchFittingSuperLightmap (
    iPolygonTexture* poly_texture, csRect& rect, int size,
    int queue_num);*/
public:
  csTrianglesPerMaterial* GetFirst () { return polygons.first; }
  //csTrianglesPerSuperLightmap* GetFirstTrianglesSLM () { return superLM.last; }

  /// Gets the number of materials of the mesh
  virtual int GetMaterialCount() const { return matCount;}

  /// Gets the material handler for a given node (by material)
  iMaterialHandle* GetMaterialPolygon(csTrianglesPerMaterial* t)
  { return (iMaterialHandle*)(materials[t->matIndex]);}

  /// Constructor
  csTriangleArrayPolygonBuffer (iVertexBufferManager* mgr);
  /// Destructor
  virtual ~csTriangleArrayPolygonBuffer ();

  /// Get the number of vertices.
  virtual int GetVertexCount () const { return num_vertices; }
  /// Get the vertices array.
  virtual csVector3* GetVertices () const { return vertices; }
  virtual void SetVertexArray (csVector3* verts, int num_verts);

  /// Adds a material to the polygon buffer
  virtual void AddMaterial (iMaterialHandle* mat_handle);

  /// Gets the material handler for a given index
  virtual iMaterialHandle* GetMaterial (int idx) const
  {
    return materials[idx];
  }

  /// Sets a material
  virtual void SetMaterial (int idx, iMaterialHandle* mat_handle);

  /// Clear the polygon buffer
  virtual void Clear ();

  /// Gets the original triangles.
  csTriangle* GetTriangles () { return orig_triangles.GetArray (); }

  /// Gets the original triangle count
  int GetTriangleCount () const { return orig_triangles.Length (); }

  /// Get the total number of vertices.
  int GetTotalVertexCount () const { return vec_vertices.Length () ; }
  /// Get the total vertices.
  csVector3* GetTotalVertices () { return vec_vertices.GetArray (); }
  /// Get the total texels.
  csVector2* GetTotalTexels () { return texels.GetArray (); }
  /// Get the total lumels.
  csVector2* GetTotalLumels () { return lumels.GetArray (); }

  /// Marks the polygon buffer as affected by any light
  virtual void MarkLightmapsDirty();

  /// Prepare.
  virtual void Prepare ();

  /// Get the bounding box.
  virtual const csBox3& GetBoundingBox () const { return bbox; }

  /// Adds a polygon to the polygon buffer
  virtual void AddPolygon (int num_verts,
	int* verts,
	csPolyTextureMapping* texmap,
	csPolyLightMapMapping* lmap,
	/*csVector2* texcoords,
	csVector2* lmcoords,*/
	const csPlane3& poly_normal,
	int mat_index,
	iRendererLightmap* lm);
};

/**
 * Version of the vertex buffer manager that understands
 * csPolArrayPolygonBuffer.
 */
class csTriangleArrayVertexBufferManager : public csVertexBufferManager
{
  friend class csTriangleArrayPolygonBuffer;
public:
  csGraphics3DOGLCommon* g3d;

  /// Initialize the vertex buffer manager
  csTriangleArrayVertexBufferManager (iObjectRegistry* object_reg,
    csGraphics3DOGLCommon* g3d);
  /// Destroy the vertex buffer manager
  virtual ~csTriangleArrayVertexBufferManager ();

  virtual iPolygonBuffer* CreatePolygonBuffer ();
};

#endif // __CS_OGL_POLYBUF_H__

