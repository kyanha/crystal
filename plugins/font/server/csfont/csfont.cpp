/*
    Copyright (C) 2000 by Norman Kramer
    Copyright (C) 2000 by W.C.A. Wijngaards
    original unplugged code and fonts by Andrew Zabolotny

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
#include "csfont.h"
#include "cssys/csuctransform.h"
#include "cssys/csendian.h"
#include "iutil/vfs.h"
#include "csutil/util.h"
#include "csutil/databuf.h"
#include "csutil/garray.h"
#include "iutil/objreg.h"
#include "iutil/comp.h"
#include "ivaria/reporter.h"

#include "font_police.h"
#include "font_courier.h"	// font (C) Andrew Zabolotny
#include "font_tiny.h"		// font (C) Andrew Zabolotny
#include "font_italic.h"	// font (C) Andrew Zabolotny

CS_IMPLEMENT_PLUGIN

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csParasiticDataBuffer)
  SCF_IMPLEMENTS_INTERFACE (iDataBuffer)
SCF_IMPLEMENT_IBASE_END

//---------------------------------------------------------------------------

struct csFontDef
{
  char *Name;
  int Height;
  int Baseline;
  uint8 *FontBitmap;
  size_t bitmapSize;
  uint8 *IndividualWidth;
  csDefaultFont::CharRange* ranges;
};

static csFontDef const FontList [] =
{
  { CSFONT_LARGE,	8, 1, font_Police,  sizeof (font_Police),  
    width_Police,  ranges_Police  },
  { CSFONT_ITALIC,	8, 1, font_Italic,  sizeof (font_Italic),  
    width_Italic,  ranges_Italic  },
  { CSFONT_COURIER,	8, 1, font_Courier, sizeof (font_Courier),  
    width_Courier, ranges_Courier },
  { CSFONT_SMALL,	6, 1, font_Tiny,    sizeof (font_Tiny),  
    width_Tiny,    ranges_Tiny    }
};

int const FontListCount = sizeof (FontList) / sizeof (csFontDef);

SCF_IMPLEMENT_IBASE (csDefaultFontServer)
  SCF_IMPLEMENTS_INTERFACE (iFontServer)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csDefaultFontServer::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csDefaultFontServer)


csDefaultFontServer::csDefaultFontServer (iBase *pParent) : object_reg(0)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csDefaultFontServer::~csDefaultFontServer()
{
  SCF_DESTRUCT_EMBEDDED_IBASE(scfiComponent);
  SCF_DESTRUCT_IBASE();
}

csPtr<iFont> csDefaultFontServer::LoadFont (const char *filename)
{
  int i;

  // First of all, look for an already loaded font
  for (i = 0; i < fonts.Length (); i++)
  {
    csDefaultFont *font = fonts.Get (i);
    if (!strcmp (filename, font->Name))
    {
      font->IncRef ();
      return font;
    }
  }

  // Now check the list of static fonts
  if (filename [0] == '*')
  {
    for (i = 0; i < FontListCount; i++)
    {
      if (!strcmp (filename, FontList [i].Name))
      {
	int numRanges = 0;
	int numGlyphs = 0;

	while (FontList [i].ranges[numRanges].charCount != 0)
	{
	  numGlyphs += FontList [i].ranges[numRanges].charCount;
	  numRanges++;
	}
	csDirtyAccessArray<csBitmapMetrics> bMetrics;
	bMetrics.SetLength (numGlyphs);
	csDirtyAccessArray<csGlyphMetrics> gMetrics;
	gMetrics.SetLength (numGlyphs);
	int j;
	for (j = 0; j < numGlyphs; j++)
	{
	  bMetrics[j].width = FontList [i].IndividualWidth[j];
	  bMetrics[j].height = FontList [i].Height;
	  bMetrics[j].left = 0;
	  bMetrics[j].top = FontList [i].Height - FontList [i].Baseline;
	  gMetrics[j].advance = FontList [i].IndividualWidth[j];
	}

	csRef<iDataBuffer> db;
	db.AttachNew (new csDataBuffer ((char*)FontList[i].FontBitmap,
	  FontList[i].bitmapSize, false));
        return new csDefaultFont (this,	FontList [i].Name,
          FontList [i].ranges,
	  FontList [i].Height, FontList [i].Height-FontList [i].Baseline,
	  FontList [i].Baseline,
	  gMetrics.GetArray (), db, bMetrics.GetArray ());
      }
    }
  }
  else
  {
    // Otherwise try to load the font as a .csf file
    csDefaultFont *fontdef = ReadFontFile (filename);
    if (fontdef)
    {
      delete [] fontdef->Name;
      fontdef->Name = csStrNew (filename);
      return fontdef;
    }
  }

  return 0;
}

iFont *csDefaultFontServer::GetFont (int iIndex)
{
  if ((iIndex >= 0) && (iIndex < fonts.Length ()))
  {
    iFont *font = fonts.Get (iIndex);
    if (font) return font;
  }
  return 0;
}

void csDefaultFontServer::NotifyCreate (csDefaultFont *font)
{
  fonts.Push (font);
}

void csDefaultFontServer::NotifyDelete (csDefaultFont *font)
{
  int index = fonts.Find (font);
  if (index >= 0)
  {
    // Extract the element from the array instead of Delete to avoid
    // the array from deleting it again.
    fonts.Extract (index);
  }
}

csDefaultFont *csDefaultFontServer::ReadFontFile(const char *file)
{
  csRef<iVFS> VFS (CS_QUERY_REGISTRY (object_reg, iVFS));
  csRef<iDataBuffer> fntfile (VFS->ReadFile (file, false));
  if (!fntfile)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_WARNING,
        "crystalspace.font.csfont",
      	"Could not read font file %s.", file);
    return 0;
  }

  char *data = fntfile->GetData ();
  if (data [0] != 'C' || data [1] != 'S' ||  data [2] != 'F')
  {
error:
    return 0;
  }

  /// the new fontdef to store info into
  csFontDef fontdef;
  csString fontdefName;
  bool hasCharRanges = false;
  bool hasBitmapMetrics = false;
  bool hasGlyphAdvance = false;
  int ascent = 0;
  int descent = 0;
  memset (&fontdef, 0, sizeof (fontdef));
  int fontdefGlyphs = 256;
  int fontdefFirst = 0;
  bool fontdefAlpha = false;

  char *end = strchr (data, '\n');
  char *cur = strchr (data, '[');
  if (!end || !cur)
    goto error;

  char *binary = end + 1;
  while ((end > data) && ((end [-1] == ' ') || (end [-1] == ']')))
    end--;

  cur++;
  while (cur < end)
  {
    while ((cur < end) && (*cur == ' '))
      cur++;

    char kw [20];
    size_t kwlen = 0;
    while ((cur < end) && (*cur != '=') && (kwlen < sizeof (kw) - 1))
      kw [kwlen++] = *cur++;
    kw [kwlen] = 0;
    if (!kwlen)
      break;

    cur = strchr (cur, '=');
    if (!cur) break;
    cur++;

    if (!strcmp (kw, "Font"))
    {
      char *start = cur;
      while ((cur < end) && (*cur != ' '))
        cur++;
      fontdefName.SetCapacity (cur - start);
      fontdef.Name = fontdefName.GetData ();
      memcpy (fontdef.Name, start, cur - start);
      fontdef.Name [cur - start] = 0;
    }
    else
    {
      char val [20];
      size_t vallen = 0;
      while ((cur < end) && (*cur != ' ') && (vallen < sizeof (val) - 1))
        val [vallen++] = *cur++;
      val [vallen] = 0;
      int n = atoi (val);

      if (!strcmp (kw, "Width"))
	/* ignore */;
      else if (!strcmp (kw, "Height"))
        fontdef.Height = n;
      else if (!strcmp (kw, "Baseline"))
        fontdef.Baseline = n;
      else if (!strcmp (kw, "First"))
        fontdefFirst = n;
      else if (!strcmp (kw, "Glyphs"))
        fontdefGlyphs = n;
      else if (!strcmp (kw, "Alpha"))
        fontdefAlpha = (n != 0);
      else if (!strcmp (kw, "Ascent"))
	ascent = n;
      else if (!strcmp (kw, "Descent"))
	descent = n;
      else if (!strcmp (kw, "HasCharRanges"))
	hasCharRanges = (n != 0);
      else if (!strcmp (kw, "HasBitmapMetrics"))
	hasBitmapMetrics = (n != 0);
      else if (!strcmp (kw, "HasGlyphAdvance"))
	hasGlyphAdvance = (n != 0);
      else
      {
	csReport (object_reg, CS_REPORTER_SEVERITY_WARNING,
	  "crystalspace.font.csfont",
      	  "Unrecognized property '%s' in font file %s.", kw, file);
      }

    }
  }

  bool newStyleFont = (hasCharRanges | hasBitmapMetrics | hasGlyphAdvance);

  if (newStyleFont)
  {
  #if defined(CS_DEBUG)
    csReport (object_reg, CS_REPORTER_SEVERITY_NOTIFY,
      "crystalspace.font.csfont",
      "Reading new style Font %s.",
      fontdef.Name);
  #endif
    csDirtyAccessArray<csBitmapMetrics> bMetrics;
    csDirtyAccessArray<csBitmapMetrics> aMetrics;
    csDirtyAccessArray<csGlyphMetrics> gMetrics;
    csDirtyAccessArray<csDefaultFont::CharRange> ranges;

    int numGlyphs = 0;
    if (hasCharRanges)
    {
      int numRanges = 0;
      do
      {
	ranges.GetExtend (numRanges).startChar = get_le_long (binary);
	binary += 4;
	ranges[numRanges].charCount = get_le_long (binary);
	binary += 4;
	numGlyphs += ranges[numRanges].charCount;
	numRanges++;
      } 
      while (ranges[numRanges - 1].charCount != 0);
    }
    else
    {
      if (fontdefGlyphs == 0)
      {
	csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
	  "crystalspace.font.csfont",
	  "Malformed font %s: neither 'Glyphs' nor 'HasCharRanges' property",
	  fontdef.Name);
	return 0;
      }
      ranges.SetLength (2);
      ranges[0].startChar = fontdefFirst;
      ranges[0].charCount = fontdefGlyphs;
      ranges[1].startChar = 0;
      ranges[1].charCount = 0;
      numGlyphs = fontdefGlyphs;
    }

    size_t bitmapSize = 0;
    size_t alphaSize = 0;
    bMetrics.SetLength (numGlyphs);
    aMetrics.SetLength (numGlyphs);
    if (hasBitmapMetrics)
    {
      int j;
      for (j = 0; j < numGlyphs; j++)
      {
	memset (&(bMetrics[j]), 0, sizeof (csBitmapMetrics));
	bMetrics[j].width = get_le_long (binary);
	binary += 4;
	bMetrics[j].height = get_le_long (binary);
	binary += 4;
	bMetrics[j].left = get_le_long (binary);
	binary += 4;
	bMetrics[j].top = get_le_long (binary);
	binary += 4;
	bitmapSize += ((bMetrics[j].width + 7) / 8) * bMetrics[j].height;
      }
      if (fontdefAlpha)
      {
	for (j = 0; j < numGlyphs; j++)
	{
	  memset (&(aMetrics[j]), 0, sizeof (csBitmapMetrics));
	  aMetrics[j].width = get_le_long (binary);
	  binary += 4;
	  aMetrics[j].height = get_le_long (binary);
	  binary += 4;
	  aMetrics[j].left = get_le_long (binary);
	  binary += 4;
	  aMetrics[j].top = get_le_long (binary);
	  binary += 4;
	  alphaSize += bMetrics[j].width * bMetrics[j].height;
	}
      }
    }
    else
    {
      uint8* individualWidths = (uint8*)binary;
      int j;
      for (j = 0; j < numGlyphs; j++)
      {
	memset (&(bMetrics[j]), 0, sizeof (csBitmapMetrics));
	bMetrics[j].width = individualWidths[j];
	bMetrics[j].height = fontdef.Height;
	bMetrics[j].left = 0;
	bMetrics[j].top = ascent;

	if (fontdefAlpha)
	{
	  memcpy (&(aMetrics[j]), &(bMetrics[j]), sizeof (csBitmapMetrics));
	}
	bitmapSize += ((individualWidths[j] + 7) / 8) * fontdef.Height;
	alphaSize += individualWidths[j] * fontdef.Height;
      }
      binary += numGlyphs;
    }

    gMetrics.SetLength (numGlyphs);
    if (hasGlyphAdvance)
    {
      int j;
      for (j = 0; j < numGlyphs; j++)
      {
	memset (&(gMetrics[j]), 0, sizeof (csGlyphMetrics));
	gMetrics[j].advance = get_le_long (binary);
	binary += 4;
      }
    }
    else
    {
      int j;
      for (j = 0; j < numGlyphs; j++)
      {
	memset (&(gMetrics[j]), 0, sizeof (csGlyphMetrics));
	gMetrics[j].advance = bMetrics[j].width;
      }
    }

    size_t bitmapOffs = binary - data;
    csRef<iDataBuffer> bitmapData = csPtr<iDataBuffer> 
      (new csParasiticDataBuffer (fntfile, bitmapOffs,
        bitmapSize));

    csRef<iDataBuffer> alphaData;
    if (fontdefAlpha)
    {
      alphaData.AttachNew (new csParasiticDataBuffer (fntfile, 
	bitmapOffs + bitmapSize, alphaSize));
    }
    return new csDefaultFont (this, fontdef.Name, ranges.GetArray (), 
      fontdef.Height, ascent, descent,
      gMetrics.GetArray (), bitmapData, bMetrics.GetArray (),
      alphaData, fontdefAlpha ? aMetrics.GetArray () : 0);
  }
  else
  {
  #if defined(CS_DEBUG)
    csReport (object_reg, CS_REPORTER_SEVERITY_NOTIFY,
      "crystalspace.font.csfont",
      "Reading old style Font %s, height %d, baseline %d, First %d, %d Glyphs", 
      fontdef.Name, fontdef.Height, fontdef.Baseline, fontdefFirst, 
      fontdefGlyphs);
  #endif

    uint8* individualWidths = (uint8*)binary;
    csDirtyAccessArray<csBitmapMetrics> bMetrics;
    bMetrics.SetLength (fontdefGlyphs);
    csDirtyAccessArray<csGlyphMetrics> gMetrics;
    gMetrics.SetLength (fontdefGlyphs);
    int j;
    for (j = 0; j < fontdefGlyphs; j++)
    {
      bMetrics[j].width = individualWidths[j];
      bMetrics[j].height = fontdef.Height;
      bMetrics[j].left = 0;
      bMetrics[j].top = fontdef.Height - fontdef.Baseline;
      gMetrics[j].advance = individualWidths[j];
    }

    // Compute the font size
    int c, fontsize = 0, alphasize = 0;
    for (c = 0; c < fontdefGlyphs; c++)
    {
      fontsize += ((individualWidths [c] + 7) / 8) * fontdef.Height;
      alphasize += individualWidths [c] * fontdef.Height;
    }

    size_t binOffs = binary - data;

    csRef<iDataBuffer> bitmapData = csPtr<iDataBuffer> 
      (new csParasiticDataBuffer (fntfile, binOffs + fontdefGlyphs,
        fontsize));

    csDefaultFont::CharRange ranges[2];
    ranges[0].startChar = fontdefFirst;
    ranges[0].charCount = fontdefGlyphs;
    ranges[1].startChar = 0;
    ranges[1].charCount = 0;

    csRef<iDataBuffer> alphaData;
    if (fontdefAlpha)
    {
      alphaData.AttachNew (new csParasiticDataBuffer (fntfile, 
	binOffs + fontdefGlyphs + fontsize, alphasize));
    }
    return new csDefaultFont (this, fontdef.Name, ranges, fontdef.Height,
      fontdef.Height - fontdef.Baseline, fontdef.Baseline,
      gMetrics.GetArray (), bitmapData, bMetrics.GetArray (),
      alphaData, fontdefAlpha ? bMetrics.GetArray () : 0);
  }
}


//--//--//--//--//--//--//--//--//--//--//--//--//--//- The font object -//--//

SCF_IMPLEMENT_IBASE (csDefaultFont)
  SCF_IMPLEMENTS_INTERFACE (iFont)
SCF_IMPLEMENT_IBASE_END

csDefaultFont::csDefaultFont (csDefaultFontServer *parent, const char *name, 
			      CharRange* glyphs, int height, 
			      int ascent, int descent,
			      csGlyphMetrics* gMetrics,
			      iDataBuffer* bitmap, csBitmapMetrics* bMetrics,
			      iDataBuffer* alpha, csBitmapMetrics* aMetrics) 
			      : DeleteCallbacks (4, 4)
{
  SCF_CONSTRUCT_IBASE (parent);
  Parent = parent;
  Parent->NotifyCreate (this);
  Name = csStrNew (name);

  Height = height;
  Ascent = ascent;
  Descent = descent;
  bitData = bitmap;
  alphaData = alpha;

  int i = 0;
  int n = 0;
  size_t bOffs = 0, aOffs = 0;
  MaxWidth = 0;
  while (glyphs[n].charCount > 0)
  {
    int numGlyphs = glyphs[n].charCount;
    utf32_char glyph = glyphs[n].startChar;
    while (numGlyphs > 0)
    {
      int w = bMetrics[i].width + abs (bMetrics[i].left);
      if (MaxWidth < w)
        MaxWidth = w;
      int bSize = ((bMetrics[i].width + 7) / 8) * bMetrics[i].height;
      int aSize = 0;
      if (aMetrics)
      {
	w = aMetrics[i].width + abs (aMetrics[i].left);
	if (MaxWidth < w)
	  MaxWidth = w;
	aSize = aMetrics[i].width * aMetrics[i].height;
      }

      int gidx1 = glyph >> GLYPH_INDEX_UPPER_SHIFT, 
	gidx2 = glyph & GLYPH_INDEX_LOWER_MASK;

      if (Glyphs.Length () <= gidx1)
      {
	Glyphs.SetLength (gidx1 + 1);
      }
      PlaneGlyphs*& pg = Glyphs[gidx1];
      if (pg == 0)
      {
	pg = new PlaneGlyphs;
      }
      Glyph& glyphData = pg->entries[gidx2];

      glyphData.bitmapOffs = bOffs;
      glyphData.bitmapSize = bSize;
      bOffs += bSize;
      memcpy (&glyphData.bMetrics, &(bMetrics[i]), sizeof (csBitmapMetrics));
      if (alpha)
      {
	glyphData.alphaOffs = aOffs;
	glyphData.alphaSize = aSize;
	aOffs += aSize;
	memcpy (&glyphData.aMetrics, &(aMetrics[i]), sizeof (csBitmapMetrics));
      }

      memcpy (&glyphData.gMetrics, &(gMetrics[i]), sizeof (csGlyphMetrics));

      glyph++;
      numGlyphs--;
      i++;
    }
    n++;
  }
}

csDefaultFont::~csDefaultFont ()
{
  for (int i = DeleteCallbacks.Length () - 1; i >= 0; i--)
  {
    iFontDeleteNotify* delnot = DeleteCallbacks[i];
    delnot->BeforeDelete (this);
  }

  Parent->NotifyDelete (this);
  delete [] Name;

  for (int j = 0; j < Glyphs.Length(); j++)
  {
    delete Glyphs[j];
  }

  SCF_DESTRUCT_IBASE();
}

void csDefaultFont::SetSize (int iSize)
{
  (void)iSize;
}

int csDefaultFont::GetSize ()
{
  return 0;
}

void csDefaultFont::GetMaxSize (int &oW, int &oH)
{
  oW = MaxWidth;
  oH = Height;
}

bool csDefaultFont::GetGlyphMetrics (utf32_char c, csGlyphMetrics& metrics)
{
  int gidx1 = c >> GLYPH_INDEX_UPPER_SHIFT, 
    gidx2 = c & GLYPH_INDEX_LOWER_MASK;

  if (Glyphs.Length () <= gidx1)
  {
    return false;
  }
  PlaneGlyphs*& pg = Glyphs[gidx1];
  if (pg == 0)
  {
    return false;
  }
  Glyph& glyphData = pg->entries[gidx2];

  if ((glyphData.bitmapSize == (size_t)~0) && 
      (glyphData.alphaSize == (size_t)~0))
  {
    return false;
  }

  metrics = glyphData.gMetrics;

  return true;
}

csPtr<iDataBuffer> csDefaultFont::GetGlyphBitmap (utf32_char c,
    csBitmapMetrics& metrics)
{
  if (bitData == 0) return 0;

  int gidx1 = c >> GLYPH_INDEX_UPPER_SHIFT, 
    gidx2 = c & GLYPH_INDEX_LOWER_MASK;

  if (Glyphs.Length () <= gidx1)
  {
    return 0;
  }
  PlaneGlyphs*& pg = Glyphs[gidx1];
  if (pg == 0)
  {
    return 0;
  }
  Glyph& glyphData = pg->entries[gidx2];

  if (glyphData.bitmapSize == (size_t)~0) return 0;

  metrics = glyphData.bMetrics;

  csParasiticDataBuffer* db = 
    new csParasiticDataBuffer (bitData, glyphData.bitmapOffs, 
    glyphData.bitmapSize);

  return csPtr<iDataBuffer> (db);
}

csPtr<iDataBuffer> csDefaultFont::GetGlyphAlphaBitmap (utf32_char c,
    csBitmapMetrics& metrics)
{
  if (alphaData == 0) return 0;

  int gidx1 = c >> GLYPH_INDEX_UPPER_SHIFT, 
    gidx2 = c & GLYPH_INDEX_LOWER_MASK;

  if (Glyphs.Length () <= gidx1)
  {
    return 0;
  }
  PlaneGlyphs*& pg = Glyphs[gidx1];
  if (pg == 0)
  {
    return 0;
  }
  Glyph& glyphData = pg->entries[gidx2];

  if (glyphData.alphaSize == (size_t)~0) return 0;

  metrics = glyphData.aMetrics;

  csParasiticDataBuffer* db = 
    new csParasiticDataBuffer (alphaData, glyphData.alphaOffs, 
    glyphData.alphaSize);

  return csPtr<iDataBuffer> (db);
}

void csDefaultFont::GetDimensions (const char *text, int &oW, int &oH)
{
  int dummy;
  GetDimensions (text, oW, oH, dummy);
}

void csDefaultFont::GetDimensions (const char *text, int &oW, int &oH, 
				   int &desc)
{
  oH = Height;
  oW = 0;
  desc = GetDescent ();

  int defW = 0;
  if (Glyphs.Length () > (CS_FONT_DEFAULT_GLYPH >> GLYPH_INDEX_UPPER_SHIFT))
  {
    PlaneGlyphs* pg = Glyphs[(CS_FONT_DEFAULT_GLYPH >> GLYPH_INDEX_UPPER_SHIFT)];
    if (pg != 0)
    {
      Glyph& glyphData = 
	pg->entries[(CS_FONT_DEFAULT_GLYPH & GLYPH_INDEX_LOWER_MASK)];
      if ((glyphData.bitmapSize != (size_t)~0) || 
	(glyphData.alphaSize != (size_t)~0))
	defW = glyphData.gMetrics.advance;
    }
  }

  int textLen = strlen ((char*)text);
  while (textLen > 0)
  {
    utf32_char glyph;
    int skip = csUnicodeTransform::UTF8Decode ((utf8_char*)text, textLen, 
      glyph, 0);
    if (skip == 0) break;

    text += skip;
    textLen -= skip;

    int gidx1 = glyph >> GLYPH_INDEX_UPPER_SHIFT, 
      gidx2 = glyph & GLYPH_INDEX_LOWER_MASK;

    if (Glyphs.Length () <= gidx1)
    {
      oW += defW;
      continue;
    }
    PlaneGlyphs* pg = Glyphs[gidx1];
    if (pg == 0)
    {
      oW += defW;
      continue;
    }
    Glyph& glyphData = pg->entries[gidx2];

    if ((glyphData.bitmapSize != (size_t)~0) || 
      (glyphData.alphaSize != (size_t)~0))
      oW += glyphData.gMetrics.advance;
    else
      oW += defW;
  }
}

int csDefaultFont::GetLength (const char *text, int maxwidth)
{
  int defW = 0;
  if (Glyphs.Length () > (CS_FONT_DEFAULT_GLYPH >> GLYPH_INDEX_UPPER_SHIFT))
  {
    PlaneGlyphs* pg = Glyphs[(CS_FONT_DEFAULT_GLYPH >> GLYPH_INDEX_UPPER_SHIFT)];
    if (pg != 0)
    {
      Glyph& glyphData = 
	pg->entries[(CS_FONT_DEFAULT_GLYPH & GLYPH_INDEX_LOWER_MASK)];
      if ((glyphData.bitmapSize != (size_t)~0) || 
	(glyphData.alphaSize != (size_t)~0))
	defW = glyphData.gMetrics.advance;
    }
  }

  int n = 0;
  int textLen = strlen ((char*)text);
  while (textLen > 0)
  {
    utf32_char glyph;
    int skip = csUnicodeTransform::UTF8Decode ((utf8_char*)text, textLen, 
      glyph, 0);
    if (skip == 0) break;

    text += skip;
    textLen -= skip;

    int charW = defW;

    int gidx1 = glyph >> GLYPH_INDEX_UPPER_SHIFT, 
      gidx2 = glyph & GLYPH_INDEX_LOWER_MASK;

    if (Glyphs.Length () > gidx1)
    {
      PlaneGlyphs*& pg = Glyphs[gidx1];
      if (pg != 0)
      {
	Glyph& glyphData = pg->entries[gidx2];
	charW = glyphData.gMetrics.advance;
      }
    }

    if (maxwidth < charW)
      return n;

    maxwidth -= charW;
    n += skip;
  }
  return n;
}

void csDefaultFont::AddDeleteCallback (iFontDeleteNotify* func)
{
  DeleteCallbacks.Push (func);
}

bool csDefaultFont::RemoveDeleteCallback (iFontDeleteNotify* func)
{
  int idx = DeleteCallbacks.Find (func);
  if (idx != -1)
  {
    DeleteCallbacks.Delete (func);
    return true;
  }
  return false;
}

bool csDefaultFontServer::eiComponent::Initialize (iObjectRegistry* p)
{
  scfParent->object_reg = p;
  return true;
}

int csDefaultFont::GetAscent ()
{
  return Ascent;
}

int csDefaultFont::GetDescent ()
{
  return Descent;
}

bool csDefaultFont::HasGlyph (utf32_char c)
{
  int gidx1 = c >> GLYPH_INDEX_UPPER_SHIFT, 
    gidx2 = c & GLYPH_INDEX_LOWER_MASK;

  if (Glyphs.Length () <= gidx1)
  {
    return false;
  }
  PlaneGlyphs*& pg = Glyphs[gidx1];
  if (pg == 0)
  {
    return false;
  }
  Glyph& glyphData = pg->entries[gidx2];

  return ((glyphData.bitmapSize != (size_t)~0) || 
    (glyphData.alphaSize != (size_t)~0));
}
