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

#include "cssysdef.h"

#include "cssys/csuctransform.h"
#include "iutil/databuff.h"

#include "graph2d.h"
#include "softfontcache.h"

//---------------------------------------------------------------------------

csSoftFontCache::csSoftFontCache (csGraphics2D* G2D)
{
  csSoftFontCache::G2D = G2D;

  cacheRemaining = G2D->config->GetInt ("Video.FontCache.MaxSize",
    1024*1024);
}

csSoftFontCache::~csSoftFontCache ()
{
  CleanupCache ();
}

csSoftFontCache::GlyphCacheData* csSoftFontCache::InternalCacheGlyph (
  KnownFont* font, utf32_char glyph, uint flags)
{
  SoftGlyphCacheData* newData = new SoftGlyphCacheData;
  SetupCacheData (newData, font, glyph, flags);

  newData->glyphDataBuf = font->font->GetGlyphBitmap (glyph, 
    newData->bitmapMetrics);
  newData->glyphData = 
    newData->glyphDataBuf ? newData->glyphDataBuf->GetUint8 () : 0;

  if (flags & CS_WRITE_NOANTIALIAS)
  {
    newData->glyphAlphaData = 0;
  }
  else
  {
    newData->glyphAlphaDataBuf = font->font->GetGlyphAlphaBitmap (glyph,
      newData->alphaMetrics);
    newData->glyphAlphaData = 
      newData->glyphAlphaDataBuf ? newData->glyphAlphaDataBuf->GetUint8 () : 0;
  }

  size_t glyphSize = 0;
  if (newData->glyphDataBuf) glyphSize += newData->glyphDataBuf->GetSize ();
  if (newData->glyphAlphaDataBuf) glyphSize += newData->glyphAlphaDataBuf->GetSize ();

  if (glyphSize > cacheRemaining)
  {
    delete newData;
    return 0;
  }

  cacheRemaining -= glyphSize;

  return newData;

}

void csSoftFontCache::InternalUncacheGlyph (GlyphCacheData* cacheData)
{
  SoftGlyphCacheData* softCacheData = (SoftGlyphCacheData*)cacheData;
  size_t glyphSize = 0;
  if (softCacheData->glyphDataBuf) glyphSize += 
    softCacheData->glyphDataBuf->GetSize ();
  if (softCacheData->glyphAlphaDataBuf) glyphSize += 
    softCacheData->glyphAlphaDataBuf->GetSize ();
  cacheRemaining += glyphSize;
  delete softCacheData;
}

//---------------------------------------------------------------------------

csSoftFontCache8::csSoftFontCache8 (csGraphics2D* G2D) : csSoftFontCache (G2D)
{
}

#define WS_NAME csSoftFontCache8::WriteString
#define WS_PIXTYPE uint8
#include "writechr.inc"

//---------------------------------------------------------------------------

csSoftFontCache16_NoAA::csSoftFontCache16_NoAA (csGraphics2D* G2D) : csSoftFontCache (G2D)
{
}

#define WS_NAME csSoftFontCache16_NoAA::WriteString
#define WS_PIXTYPE uint16
#include "writechr.inc"

//---------------------------------------------------------------------------

csSoftFontCache16_555::csSoftFontCache16_555 (csGraphics2D* G2D) : csSoftFontCache (G2D)
{
}

#define WS_NAME csSoftFontCache16_555::WriteString
#define WS_PIXTYPE uint16
#define WS_ALPHA_AVAILABLE
#define WS_A_R5G5B5
#include "writechr.inc"

//---------------------------------------------------------------------------

csSoftFontCache16_565::csSoftFontCache16_565 (csGraphics2D* G2D) : csSoftFontCache (G2D)
{
}

#define WS_NAME csSoftFontCache16_565::WriteString
#define WS_PIXTYPE uint16
#define WS_ALPHA_AVAILABLE
#define WS_A_R5G6B5
#include "writechr.inc"

//---------------------------------------------------------------------------

csSoftFontCache32::csSoftFontCache32 (csGraphics2D* G2D) : csSoftFontCache (G2D)
{
}

#define WS_NAME csSoftFontCache32::WriteString
#define WS_PIXTYPE uint32
#define WS_ALPHA_AVAILABLE
#define WS_A_R8G8B8
#include "writechr.inc"

