/*
    Copyright (C) 2003 by Jorrit Tyberghein
	      (C) 2003 by Frank Richter

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

#ifndef __CS_CANVAS_OPENGLCOMMON_GLFONTCACHE_H__
#define __CS_CANVAS_OPENGLCOMMON_GLFONTCACHE_H__

#include "csgeom/subrec2.h"
#include "csgeom/vector2.h"
#include "csutil/dirtyaccessarray.h"

#include "../common/fontcache.h"

class csGLStateCache;

class csGraphics2DGLCommon;

class csGLFontCache : public csFontCache
{
  struct GLGlyphCacheData : public csFontCache::GlyphCacheData
  {
    csSubRect2* subrect;
    float tx1, ty1, tx2, ty2;
    size_t texNum;
    csBitmapMetrics bmetrics;
  };

  csGraphics2DGLCommon* G2D;
  csGLStateCache* statecache;

  int texSize;
  size_t maxTxts;
  size_t usedTexs;
  int glyphAlign;
  GLuint texWhite;

  struct CacheTexture
  {
    GLuint handle;
    csSubRectangles2* glyphRects;

    CacheTexture () 
    { 
      glyphRects = 0; 
    }
    ~CacheTexture () 
    { 
      delete glyphRects; 
    }
    void InitRects (int size) 
    { 
      glyphRects = new csSubRectangles2 (csRect (0, 0, size, size));
    }
  };
  csArray<CacheTexture> textures;
  csBlockAllocator<GLGlyphCacheData> cacheDataAlloc;

  struct TextJob
  {
    GLuint texture;
    int fg, bg;
    size_t vertOffset, vertCount, bgVertOffset, bgVertCount;

    void ClearRanges()
    {
      vertOffset = vertCount = bgVertOffset = bgVertCount = 0;
    }
  };
  csArray<TextJob> jobs;
  size_t jobCount;
  bool textWriting;
  bool needStates;
  int envColor;
  size_t numFloats;
  size_t maxFloats;
  bool tcaEnabled, vaEnabled, caEnabled;
  csDirtyAccessArray<float> verts2d;
  csDirtyAccessArray<float> texcoords;

  TextJob& GetJob (int fg, int bg, GLuint texture, size_t bgOffset);

  inline void FlushArrays ();
  void BeginText ();
protected:
  virtual GlyphCacheData* InternalCacheGlyph (KnownFont* font,
    utf32_char glyph, uint flags);
  virtual void InternalUncacheGlyph (GlyphCacheData* cacheData);

  void CopyGlyphData (iFont* font, utf32_char glyph, size_t tex, 
    const csBitmapMetrics& bmetrics, const csRect& texRect, 
    iDataBuffer* bitmapDataBuf, iDataBuffer* alphaDataBuf);
public:
  csGLFontCache (csGraphics2DGLCommon* G2D);
  virtual ~csGLFontCache ();

  /**
   * Draw a string using OpenGL, where the baseline starts at x,y. 
   */
  virtual void WriteString (iFont *font, int pen_x, int pen_y, 
    int fg, int bg, const utf8_char* text, uint flags);

  /**
   * Flush the cached text vertices.
   * The text writer caches some text (and sets some needed states).
   * Before drawing anything else, the text cache has to be flushed
   * (and the states reset).
   */
  void FlushText ();

  /**
   * Create images of all the texture pages used for caching.
   */
  void DumpFontCache (csRefArray<iImage>& pages);
};

#endif // __CS_CANVAS_OPENGLCOMMON_GLFONTCACHE_H__
