/*
    Copyright (C) 1998-2001 by Jorrit Tyberghein
    Copyright (C) 1998 by Dan Ogles.

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

#ifndef __GL_TEXTURECACHE_H__
#define __GL_TEXTURECACHE_H__

#include "csutil/scf.h"
#include "csgeom/csrect.h"
#include "ivideo/graph3d.h"
#include "ogl_polybuf.h"

#if defined(CS_OPENGL_PATH)
#include CS_HEADER_GLOBAL(CS_OPENGL_PATH,gl.h)
#else
#include <GL/gl.h>
#endif

struct iLightMap;
struct iTextureHandle;
struct iPolygonTexture;
struct iObjectRegistry;
class csGraphics3DOGLCommon;

/**
 * Cache element for a texture. This element will be stored
 * in the OpenGL texture cache and is also kept with the polygon
 * itself.
 */
struct csTxtCacheData
{
  /// The size of this texture.
  long Size;
  /// iTextureHandle.
  iTextureHandle* Source;
  /// GL texture handle.
  GLuint Handle;
  /// Linked list.
  csTxtCacheData *next, *prev;
};

/**
 * This is the OpenGL texture cache.
 */
class OpenGLTextureCache
{
private:
  csGraphics3DOGLCommon* g3d;
protected:
  bool rstate_bilinearmap;

  /// the head and tail of the cache data
  csTxtCacheData *head, *tail;

  /// the maximum size of the cache
  long cache_size;
  /// number of items
  int num;
  /// Total size of all loaded textures
  long total_size;
  /// Peak total size of all loaded textures
  long peak_total_size;
public:
  /// Takes the maximum size of the cache.
  OpenGLTextureCache (int max_size, csGraphics3DOGLCommon* g3d);
  ///
  ~OpenGLTextureCache ();

  /// Make sure this texture is active in OpenGL.
  void Cache (iTextureHandle *texture);
  /// Remove an individual texture from cache.
  void Uncache (iTextureHandle *texh);

  /// Clear the cache.
  void Clear ();

  ///
  void SetBilinearMapping (bool m) { rstate_bilinearmap = m; }
  ///
  bool GetBilinearMapping () { return rstate_bilinearmap; }

  /// returns the total amount of cached textures.
  int GetTotalTextureSize() { return total_size; }
  /// returns the total amount of cached textures.
  int GetPeakTotalTextureSize() { return peak_total_size; }

protected:

  /// Really load the texture in OpenGL memory.
  void Load (csTxtCacheData *d, bool reload = false);
  ///
  void Unload (csTxtCacheData *d);
};

#define DEFAULT_SUPER_LM_SIZE 256
#define DEFAULT_SUPER_LM_NUM 10

/**
 * Cache data for a whole superlightmap.
 */
class csSLMCacheData
{
public:
  csTrianglesPerSuperLightmap* source;
  GLuint Handle;
  GLuint FogHandle;
  bool hasFog;
  bool isUnlit;
  void Alloc (csTrianglesPerSuperLightmap* s);
  void Clear ();
  bool IsPrecalcSuperlightmap () const { return true; }
  bool HasFog () const { return hasFog; }
  bool IsUnlit () const { return isUnlit; }
};

/**
 * This class represents a super-lightmap.
 * A super-lightmap is a collection of smaller lightmaps that
 * fit together in one big texture.
 */
class csSuperLightMap
{
public:
  /// An OpenGL texture handle.
  GLuint Handle;
  /// the head and tail of the cache data
  csSLMCacheData *cacheData;

  /// Constructor.
  csSuperLightMap ();
  /// Destructor.
  ~csSuperLightMap ();

  /// Try to allocate a lightmap here. Return NULL on failure.
  void Alloc (csTrianglesPerSuperLightmap* s);
  /// Clear all lightmaps in this super lightmap.
  void Clear ();
};

/**
 * Cache for OpenGL lightmaps. This cache keeps a number of
 * super lightmaps. Every super lightmaps holds a number of lightmaps.
 */
class OpenGLLightmapCache
{
public:
  /// Number of static lightmaps.
  static int super_lm_num;
  /// Size of static lightmaps.
  static int super_lm_size;

  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  // This OpenGL texture handle is temporary for the version of
  // DrawPolygon that doesn't use the super lightmap cache.
  // Should be removed in future.
  GLuint TempHandle;
  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

private:
  csGraphics3DOGLCommon* g3d;

  /// A number of super-lightmaps to contain all other lightmaps.
  csSuperLightMap* suplm;
  /// Current super lightmap.
  int cur_lm;
  /// If true then setup is ok.
  bool initialized;

  void Setup ();

public:
  ///
  OpenGLLightmapCache (csGraphics3DOGLCommon* g3d);
  ///
  ~OpenGLLightmapCache ();

  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  GLuint GetTempHandle () { Setup (); return TempHandle; }
  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  /// Cache a whole precalculated superlightmap
  void Cache(csTrianglesPerSuperLightmap* s, bool dirty,
             bool* modified);

  /// Finds an empty superlightmap (returns -1 if none is found)
  int FindFreeSuperLightmap();

  /// Clear the entire lightmap cache.
  void Clear ();

  /// Check if lightmap blows size limit
  bool IsLightmapOK (iPolygonTexture *polytex);
};

#endif // __GL_TEXTURECACHE_H__

