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

#include "ivideo/fontserv.h"

#include "fontcache.h"

SCF_IMPLEMENT_IBASE(csFontCache::FontDeleteNotify)
  SCF_IMPLEMENTS_INTERFACE(iFontDeleteNotify)
SCF_IMPLEMENT_IBASE_END

csFontCache::FontDeleteNotify::FontDeleteNotify (csFontCache* cache)
{
  SCF_CONSTRUCT_IBASE(0);
  FontDeleteNotify::cache = cache;
}

csFontCache::FontDeleteNotify::~FontDeleteNotify ()
{
}

void csFontCache::FontDeleteNotify::BeforeDelete (iFont* font)
{
  cache->UncacheFont (font);
}

//---------------------------------------------------------------------------

csFontCache::csFontCache () : LRUAlloc (512)
{
  head = tail = 0;
  deleteCallback = new FontDeleteNotify (this);
}

csFontCache::~csFontCache ()
{
}

void csFontCache::CleanupCache ()
{
  GlyphCacheData* cacheData;

  while ((cacheData = GetLeastUsed ()) != 0)
  {
    InternalUncacheGlyph (cacheData);
  }

  for (int i = 0; i < knownFonts.Length(); i++)
  {
    knownFonts[i]->font->RemoveDeleteCallback (deleteCallback);
    PlaneGlyphsArray& planeGlyphs = knownFonts[i]->planeGlyphs;
    for (int j = 0; j < planeGlyphs.Length(); j++)
    {
      delete planeGlyphs[j];
    }
    delete knownFonts[i];
  }
  knownFonts.DeleteAll ();
  delete deleteCallback; deleteCallback = 0;
}

void csFontCache::PurgeEmptyPlanes (KnownFont* knownFont)
{
  if (knownFont->purgeNeeded)
  {
    PlaneGlyphsArray& planeGlyphs = knownFont->planeGlyphs;
    for (int j = 0; j < planeGlyphs.Length(); j++)
    {
      PlaneGlyphs*& pg = planeGlyphs[j];
      if (pg != 0)
      {
	if (pg->usedGlyphs == 0)
	{
	  delete pg;
	  pg = 0;
	}
      }
    }
    knownFont->purgeNeeded = false;
  }
}

csFontCache::LRUEntry* csFontCache::FindLRUEntry (
  KnownFont* font, utf32_char glyph)
{
  int gidx1 = glyph >> GLYPH_INDEX_UPPER_SHIFT, 
    gidx2 = glyph & GLYPH_INDEX_LOWER_MASK;
  if (font->planeGlyphs.Length () > gidx1)
  {
    PlaneGlyphs* pg = font->planeGlyphs[gidx1];
    return pg ? pg->entries[gidx2] : 0;
  }

  return 0;
}

csFontCache::LRUEntry* csFontCache::FindLRUEntry (
  GlyphCacheData* cacheData)
{
  LRUEntry* entry = head;
  while (entry)
  {
    if (entry->cacheData == cacheData)
    {
      break;
    }
    entry = entry->next;
  }

  return entry;
}

int csFontCache::KnownFontArrayCompareItems (
  KnownFont* const& item1, KnownFont* const& item2)
{
  return ((iFont*)item1->font - (iFont*)item2->font);
}

int csFontCache::KnownFontArrayCompareToKey (
  KnownFont* const& item1, void* item2)
{
  return ((iFont*)item1->font - (iFont*)item2);
}

csFontCache::KnownFont* csFontCache::GetCachedFont (iFont* font)
{
  int idx = knownFonts.FindSortedKey (font, KnownFontArrayCompareToKey);
  csFontCache::KnownFont* knownFont = (idx >= 0) ? knownFonts[idx] : 0;
  if (knownFont != 0)
  {
    if (knownFont->fontSize != font->GetSize ())
    {
      for (int i = 0; i < knownFont->planeGlyphs.Length (); i++)
      {
	PlaneGlyphs*& pg = knownFont->planeGlyphs[i];
	if (pg != 0)
	{
	  for (int j = 0; j < GLYPH_INDEX_LOWER_COUNT; j++)
	  {
	    LRUEntry* entry = pg->entries[j];
	    if (entry != 0) 
	    {
	      GlyphCacheData* cacheData = entry->cacheData;
	      RemoveLRUEntry (entry);
	      InternalUncacheGlyph (cacheData);
	    }
	  }
	  delete pg;
	  pg = 0;
	}
      }
      knownFont->fontSize = font->GetSize ();
      knownFont->purgeNeeded = false;
    }
  }
  return knownFont;
}

csFontCache::KnownFont* csFontCache::CacheFont (iFont* font)
{
  KnownFont* knownFont = new KnownFont;
  knownFont->font = font;
  knownFont->fontSize = font->GetSize ();
  knownFont->purgeNeeded = false;

  knownFonts.InsertSorted (knownFont, KnownFontArrayCompareItems);

  font->AddDeleteCallback (deleteCallback);

  return knownFont;
}

void csFontCache::UncacheFont (iFont* font)
{
  int idx = knownFonts.FindSortedKey (font, KnownFontArrayCompareToKey);
  if (idx >= 0)
  {
    KnownFont* knownFont = knownFonts[idx];
    for (int i = 0; i < knownFont->planeGlyphs.Length (); i++)
    {
      PlaneGlyphs*& pg = knownFont->planeGlyphs[i];
      if (pg != 0)
      {
	for (int j = 0; j < GLYPH_INDEX_LOWER_COUNT; j++)
	{
	  LRUEntry* entry = pg->entries[j];
	  if (entry != 0) 
	  {
	    GlyphCacheData* cacheData = entry->cacheData;
	    RemoveLRUEntry (entry);
	    InternalUncacheGlyph (cacheData);
	  }
	}
	delete pg;
	pg = 0;
      }
    }
    knownFonts.DeleteIndex (idx);
  }
}

csFontCache::GlyphCacheData* csFontCache::GetCacheData (KnownFont* font, 
							utf32_char glyph)
{
  LRUEntry* entry = FindLRUEntry (font, glyph);
  if (entry != 0)
  {
    // Moving the list entry to the top.
    // Not necessary if already there.
    if (entry->prev != 0)
    {
      // First, it's "extracted" from the DL list
      if (entry == tail)
      {
	// Special case if last entry.
	// We can just do "prev->next" because the list will
	// have at least 2 entries if we're here.
	tail->prev->next = 0;
	tail = tail->prev;
      }
      else
      {
	// Some entry in the middle.
        LRUEntry* ePrev = entry->prev;
        LRUEntry* eNext = entry->next;

        ePrev->next = eNext;
        eNext->prev = ePrev;
      }

      // Then, insert at the top.
      entry->prev = 0;
      entry->next = head;
      head->prev = entry;
      head = entry;
    }
    return entry->cacheData;
  }
  else
  {
    return 0;
  }
}

csFontCache::GlyphCacheData* csFontCache::GetLeastUsed ()
{
  if (tail == 0) return 0;

  GlyphCacheData* cacheData = tail->cacheData;

  // Remove the entry
  LRUEntry* el = tail;
  if (el->prev)
    el->prev->next = 0;
  else
    head = 0;
  
  tail = el->prev;

  LRUAlloc.Free (el);

  int gidx1 = cacheData->glyph >> GLYPH_INDEX_UPPER_SHIFT, 
    gidx2 = cacheData->glyph & GLYPH_INDEX_LOWER_MASK;

  PlaneGlyphsArray& planeGlyphs = cacheData->font->planeGlyphs;
  if (planeGlyphs.Length () > gidx1)
  {
    PlaneGlyphs*& pg = planeGlyphs[gidx1];
    if (pg != 0)
    {
      pg->entries[gidx2] = 0;
      pg->usedGlyphs--;
    }
  }

  cacheData->font->purgeNeeded = true;

  return cacheData;
}

void csFontCache::AddCacheData (KnownFont* font, utf32_char glyph, 
				GlyphCacheData* cacheData)
{
  CS_ASSERT (font != 0);
  CS_ASSERT (FindLRUEntry (font, glyph) == 0);

  LRUEntry* entry = LRUAlloc.Alloc ();
  entry->prev = 0;
  entry->next = head;
  if (head)
    head->prev = entry;
  else
    tail = entry;
  head = entry;
  
  entry->cacheData = cacheData;

  int gidx1 = glyph >> GLYPH_INDEX_UPPER_SHIFT, 
    gidx2 = glyph & GLYPH_INDEX_LOWER_MASK;

  PlaneGlyphsArray& planeGlyphs = font->planeGlyphs;
  if (planeGlyphs.Length () <= gidx1)
  {
    planeGlyphs.SetLength (gidx1 + 1);
  }
  PlaneGlyphs*& pg = planeGlyphs[gidx1];
  if (pg == 0)
  {
    pg = new PlaneGlyphs;
  }
  pg->usedGlyphs++;
  pg->entries[gidx2] = entry;
}

void csFontCache::RemoveCacheData (GlyphCacheData* cacheData)
{
  LRUEntry* entry = FindLRUEntry (cacheData);
  if (entry != 0) 
  {
    RemoveLRUEntry (entry);
  }
}

void csFontCache::RemoveLRUEntry (LRUEntry* entry)
{
  CS_ASSERT (entry != 0);

  if (entry->prev)
    entry->prev->next = entry->next;
  else
    head = entry->next;
  
  if (entry->next)
    entry->next->prev = entry->prev;
  else
    tail = entry->prev;

  LRUAlloc.Free (entry);
}

csFontCache::GlyphCacheData* csFontCache::InternalCacheGlyph (KnownFont* font,
							      utf32_char glyph)
{
  // This method is meant to be overridden by the actual canvas' font cache.
  // However, here you have the minimal stuff such a method should perform:
  GlyphCacheData* newData = new GlyphCacheData;
  SetupCacheData (newData, font, glyph);
  return newData;
}

void csFontCache::InternalUncacheGlyph (GlyphCacheData* cacheData)
{
  // Same as InternalCacheGlyph().
  delete cacheData;
}

void csFontCache::SetupCacheData (GlyphCacheData* cacheData,
  KnownFont* font, utf32_char glyph)
{
  memset (cacheData, 0, sizeof (GlyphCacheData));
  cacheData->font = font;
  cacheData->glyph = glyph;
  if (cacheData->hasGlyph = font->font->HasGlyph (glyph))
  {
    font->font->GetGlyphMetrics (glyph, cacheData->glyphMetrics);
  }
  else
  {
    memset (&cacheData->glyphMetrics, 0, sizeof (cacheData->glyphMetrics));
  }
}

csFontCache::GlyphCacheData* csFontCache::CacheGlyphUnsafe (
  KnownFont* font, utf32_char glyph)
{
  GlyphCacheData* cacheData;
  while ((cacheData = InternalCacheGlyph (font, glyph)) == 0)
  {
    GlyphCacheData* LUData = GetLeastUsed ();
    CS_ASSERT (LUData != 0);
    InternalUncacheGlyph (LUData);
  }
  AddCacheData (font, glyph, cacheData);

  return cacheData;
}

csFontCache::GlyphCacheData* csFontCache::CacheGlyph (KnownFont* font, 
						      utf32_char glyph)
{
  CS_ASSERT (font != 0);

  GlyphCacheData* cacheData = GetCacheData (font, glyph);
  if (cacheData == 0) 
  {
    return CacheGlyphUnsafe (font, glyph);
  }

  return cacheData;
}

void csFontCache::UncacheGlyph (GlyphCacheData* cacheData)
{
  CS_ASSERT (cacheData != 0);

  int gidx1 = cacheData->glyph >> GLYPH_INDEX_UPPER_SHIFT, 
    gidx2 = cacheData->glyph & GLYPH_INDEX_LOWER_MASK;

  PlaneGlyphsArray& planeGlyphs = cacheData->font->planeGlyphs;
  if (planeGlyphs.Length () > gidx1)
  {
    PlaneGlyphs*& pg = planeGlyphs[gidx1];
    if (pg != 0)
    {
      pg->entries[gidx2] = 0;
      pg->usedGlyphs--;
    }
  }

  cacheData->font->purgeNeeded = true;

  RemoveCacheData (cacheData);
  InternalUncacheGlyph (cacheData);
}

void csFontCache::WriteString (iFont *font, int x, int y, int fg, int bg, 
  const utf8_char* text)
{
  WriteStringBaseline (font, x, y + font->GetAscent (), fg, bg, text);
}

void csFontCache::WriteStringBaseline (iFont *font, int x, int y, int fg, int bg, 
  const utf8_char* text)
{
}

