/*
    Copyright (C) 2000 by Norman Kramer
  
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

#include <freetype.h>

#include "cssysdef.h"
#include "csutil/inifile.h"
#include "csutil/csstrvec.h"
#include "freefont.h"

IMPLEMENT_FACTORY (csFreeTypeServer)

EXPORT_CLASS_TABLE (freefont)
  EXPORT_CLASS (csFreeTypeServer, "crystalspace.font.server.freetype", 
    "Crystal Space FreeType font server" )
EXPORT_CLASS_TABLE_END

IMPLEMENT_IBASE (csFreeTypeServer)
  IMPLEMENTS_INTERFACE (iPlugIn)
  IMPLEMENTS_INTERFACE (iFontServer)
IMPLEMENT_IBASE_END

csFreeTypeServer::csFreeTypeServer (iBase *pParent)
{
  CONSTRUCT_IBASE (pParent);
  ftini = NULL;
  VFS = NULL;
}

csFreeTypeServer::~csFreeTypeServer ()
{
  if (VFS) VFS->DecRef ();
  if (ftini) ftini->DecRef ();
  if (System) System->DecRef ();
}

bool csFreeTypeServer::Initialize (iSystem *Sys)
{
  (System = Sys)->IncRef ();

  if (TT_Init_FreeType (&engine))
  {
    System->Printf (MSG_FATAL_ERROR,
      "Could not create a TrueType engine instance !\n");
    return false;
  }

  VFS = QUERY_PLUGIN_ID (System, CS_FUNCID_VFS, iVFS);
  ftini = System->CreateConfig ("config/freetype.cfg");
  if (ftini)
  {
    defaultSize = ftini->GetInt ("Default", "size", 10);
    platformID = ftini->GetInt ("Default", "platformid", 3);
    encodingID = ftini->GetInt ("Default", "encodingid", 1);

    fontset = ftini->GetStr ("Default", "fontset", NULL);

    iConfigDataIterator *fontenum = ftini->EnumData (fontset);
    while (fontenum->Next ())
      if (fontenum->GetKey () [0] == '*')
        LoadFont (fontenum->GetKey ());
    fontenum->DecRef ();
  }
  else
  {
    defaultSize = 10;
    platformID = 3;
    encodingID = 1;
    fontset = NULL;
  }

  return true;
}

iFont *csFreeTypeServer::LoadFont (const char *filename)
{
  // First of all look for an alias in config file
  if (ftini && fontset)
  {
    const char *s = ftini->GetStr (fontset, filename, NULL);
    if (s) filename = s;
  }
  // Okay, now convert the VFS path into a real path
  iDataBuffer *db = VFS->GetRealPath (filename);
  if (db) filename = (const char *)db->GetData ();

  // see if we already loaded that font
  int idx = fonts.FindKey (filename);
  if (idx >= 0)
  {
    if (db) db->DecRef ();
    csFreeTypeFont *font = fonts.Get (idx);
    font->IncRef ();
    return font;
  }

  // not yet loaded, so do it now
  csFreeTypeFont *font = new csFreeTypeFont (filename);
  if (db) db->DecRef ();
  if (!font->Load (this))
  {
    delete font;
    return NULL;
  }
  fonts.Put (font);
  font->IncRef ();
  return font;
}

iFont *csFreeTypeServer::GetFont (int iIndex)
{
  if (iIndex >= 0 && iIndex < fonts.Length ())
  {
    csFreeTypeFont *font = fonts.Get (iIndex);
    if (font) return font;
  }
  return NULL;
}

//-------------------------------------------// A FreeType font object //----//

IMPLEMENT_IBASE (csFreeTypeFont)
  IMPLEMENTS_INTERFACE (iFont)
IMPLEMENT_IBASE_END

csFreeTypeFont::csFreeTypeFont (const char *filename) : DeleteCallbacks (4, 4)
{
  CONSTRUCT_IBASE (NULL);
  name = strnew (filename);
  face.z = NULL;
  current = NULL;
}

csFreeTypeFont::~csFreeTypeFont ()
{
  for (int i = DeleteCallbacks.Length () - 2; i >= 0; i -= 2)
    ((DeleteNotify)DeleteCallbacks.Get (i))(this, DeleteCallbacks.Get (i + 1));
  if (face.z)
    TT_Close_Face (face);
}

void csFreeTypeFont::SetSize (int iSize)
{
  CreateGlyphBitmaps (iSize);
  current = FindGlyphSet (iSize);
}

int csFreeTypeFont::GetSize ()
{
  return current ? current->size : 0;
}

void csFreeTypeFont::GetMaxSize (int &oW, int &oH)
{
  if (current)
  {
    oW = current->maxW;
    oH = current->maxH;
  }
  else
    oW = oH = 0;
}

bool csFreeTypeFont::GetGlyphSize (uint8 c, int &oW, int &oH)
{
  if (!current) return false;
  oW = current->glyphs [c].w;
  oH = current->glyphs [c].h;
  return true;
}

uint8 *csFreeTypeFont::GetGlyphBitmap (uint8 c, int &oW, int &oH)
{
  if (!current) return NULL;
  oW = current->glyphs [c].w;
  oH = current->glyphs [c].h;
  return current->glyphs [c].bitmap;
}

void csFreeTypeFont::GetDimensions (const char *text, int &oW, int &oH)
{
  if (!text || !current)
  {
    oW = oH = 0;
    return;
  }

  oW = 0; oH = current->maxH;
  while (*text)
  {
    oW += current->glyphs [*(const uint8 *)text].w;
    text++;
  }
}

int csFreeTypeFont::GetLength (const char *text, int maxwidth)
{
  if (!text || !current)
    return 0;

  int count = 0, w = 0;
  while (*text)
  {
    w += current->glyphs [*(const uint8 *)text].w;
    if (w > maxwidth)
      break;
    text++; count++;
  }
  return count;
}

bool csFreeTypeFont::Load (csFreeTypeServer *server)
{
  if (TT_Open_Face (server->engine, name, &face))
  {
    server->System->Printf (MSG_WARNING,
      "Font file %s could not be loaded!\n", name);
    return false;
  }

  int error;
  if ((error = TT_Get_Face_Properties (face, &prop)))
  {
    server->System->Printf(MSG_WARNING,
      "Get_Face_Properties: error %d.\n", error);
    return false;
  }

  if ((error = TT_New_Instance (face, &instance)))
  {
    server->System->Printf(MSG_WARNING,
      "Could not create an instance of Font %s."
      " The font is probably broken!\n", name);
    return false;
  }

  // we do not change the default values of the new instance
  // ( 96 dpi, 10 pt. size, no trafo flags )

  // next we scan the charmap table if there is an encoding
  // that matches the requested platform and encoding ids
  TT_UShort i = 0;
  while (i < prop.num_CharMaps)
  {
    if ((error = TT_Get_CharMap_ID (face, i, &pID, &eID)))
      server->System->Printf(MSG_WARNING,"Get_CharMap_ID: error %d.\n",error);
    if (server->platformID == pID && server->encodingID == eID)
      break;
    i++;
  }

  if (server->platformID != pID || server->encodingID != eID)
  {
    // encoding scheme not found
    server->System->Printf(MSG_INITIALIZATION,
      "Font %s does not contain encoding %d for platform %d.\n",
      name, server->encodingID, server->platformID);

    if ((error = TT_Get_CharMap_ID (face, 0, &pID, &eID)))
    {
      server->System->Printf(MSG_WARNING,"Get_CahrMap_ID: error %d.\n",error);
      return false;
    }
    server->System->Printf (MSG_INITIALIZATION, "Will instead use encoding %d"
      " for platform %d.\n", eID, pID);
    i = 0;
  }

  // ok. now lets retrieve a handle the the charmap
  if ((error = TT_Get_CharMap (face, i, &charMap)))
  {
    server->System->Printf (MSG_WARNING, "Get_CharMap: error %d.\n", error);
    return false;
  }
	
  // now we create the bitmap of all glyphs in the face
  return CreateGlyphBitmaps (server->defaultSize);
}

bool csFreeTypeFont::CreateGlyphBitmaps (int size)
{
  if (FindGlyphSet (size))
    return true;

  if (TT_Set_Instance_CharSize (instance, size * 64))
    return false;

  // first get the instance metrics
  TT_Instance_Metrics im;
  if (TT_Get_Instance_Metrics (instance, &im))
    return false;

  // compute the maximum height a glyph from this font would have
  // including linegap
  int maxY;
  int maxDesc;
  int lineGap;
  if (prop.os2->version == 0xffff)
  {
    maxDesc = -ABS (prop.horizontal->Descender);
    lineGap = prop.horizontal->Line_Gap;
    maxY= prop.horizontal->Ascender - maxDesc + lineGap;
  }
  else
  {
    lineGap = prop.os2->sTypoLineGap;
    /// @@@ EVIL: i found that some fonts incorrectly define descends
    // as positive numbers while they should be negative - so i force
    // the numbers being negative
    maxDesc = -ABS (prop.os2->sTypoDescender);
    maxY = prop.os2->sTypoAscender - maxDesc + lineGap;
  }

  lineGap = (lineGap * im.y_scale) / 0x10000;
  maxDesc = (maxDesc * im.y_scale) / 0x10000;
  maxY    = (maxY * im.y_scale) / 0x10000;

  lineGap = lineGap / 64;
  maxY    = (maxY + 32) / 64;

  // Create the glyphset
  GlyphSet *glyphset;
  glyphset = new GlyphSet;
  glyphset->size = size;
  glyphset->maxW = glyphset->maxH = 0;
  memset (glyphset->glyphs, 0, sizeof (glyphset->glyphs));
  cache.Push (glyphset);

  for (TT_UShort iso_char = 0; iso_char < 256; iso_char++)
  {
    TT_Glyph glyph;
    if ((TT_New_Glyph (face, &glyph)))
      // The font does not contain required character, continue quietly
      continue;

    TT_UShort glyphindex = TT_Char_Index (charMap, iso_char);
    if (TT_Load_Glyph (instance, glyph, glyphindex, TTLOAD_DEFAULT))
      continue;

    // now we are going to render the glyphs outline into a bitmap
    TT_Raster_Map bitmap;
    TT_Big_Glyph_Metrics m;
    GlyphBitmap *glyphbitmap = &glyphset->glyphs [iso_char];

    // compute the extend
    if (TT_Get_Glyph_Big_Metrics (glyph, &m))
      continue;

    // grid fit ( numbers are 26.6 fixed point )
    /*
    m.bbox.xMin &= -64;
    m.bbox.yMin &= -64;
    m.bbox.xMax = (m.bbox.xMax+63) & -64;
    m.bbox.yMax = (m.bbox.yMax+63) & -64;
    */
    // prepare the bitmap
    bitmap.rows  = maxY;
    // bitmap.width = (m.bbox.xMax - m.bbox.xMin)/64;
    bitmap.width = MAX (m.horiAdvance,
      (m.horiBearingX + m.bbox.xMax - m.bbox.xMin)) / 64;
    bitmap.cols  = (bitmap.width + 7) / 8;
    bitmap.flow  = TT_Flow_Down;
    bitmap.size  = bitmap.rows * bitmap.cols;

    glyphbitmap->w = bitmap.width;
    glyphbitmap->h = bitmap.rows;
    glyphbitmap->bitmap = new uint8 [bitmap.size];

    if (!glyphbitmap->bitmap)
      continue;

    if (glyphset->maxW < glyphbitmap->w)
      glyphset->maxW = glyphbitmap->w;
    if (glyphset->maxH < glyphbitmap->h)
      glyphset->maxH = glyphbitmap->h;

    bitmap.bitmap = glyphbitmap->bitmap;
    // zero out memory
    memset (bitmap.bitmap, 0, bitmap.size);
    // and finally render it into bitmap
    TT_Get_Glyph_Bitmap (glyph, &bitmap, 0/*-m.bbox.xMin*/, -maxDesc);
    TT_Done_Glyph (glyph);
  }

  current = glyphset;
  return true;
}

void csFreeTypeFont::AddDeleteCallback (DeleteNotify func, void *databag)
{
  DeleteCallbacks.Push ((void *)func);
  DeleteCallbacks.Push (databag);
}

bool csFreeTypeFont::RemoveDeleteCallback (DeleteNotify func, void *databag)
{
  for (int i = DeleteCallbacks.Length () - 2; i >= 0; i -= 2)
    if ((DeleteCallbacks.Get (i) == (void *)func)
     && (DeleteCallbacks.Get (i + 1) == databag))
    {
      DeleteCallbacks.Delete (i);
      DeleteCallbacks.Delete (i);
      return true;
    }
  return false;
}
