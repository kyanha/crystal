/*
    Copyright (C) 2002 by Norman Kr�mer

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
#include "cssys/sysfunc.h"
#include "csutil/csstrvec.h"
#include "csutil/csstring.h"
#include "iutil/cfgfile.h"
#include "iutil/objreg.h"
#include "ivaria/reporter.h"
#include "iutil/vfs.h"
#include "iutil/plugin.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include <ft2build.h>
#include FT_INTERNAL_OBJECTS_H
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_MODULE_H
#include "freefnt2.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (csFreeType2Server)

SCF_EXPORT_CLASS_TABLE (freefnt2)
  SCF_EXPORT_CLASS (csFreeType2Server, "crystalspace.font.server.freetype2",
    "Crystal Space FreeType2 font server" )
SCF_EXPORT_CLASS_TABLE_END

SCF_IMPLEMENT_IBASE (csFreeType2Server)
  SCF_IMPLEMENTS_INTERFACE (iFontServer)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csFreeType2Server::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csFreeType2Server::csFreeType2Server (iBase *pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  library = 0;
  freetype_inited = false;
}

csFreeType2Server::~csFreeType2Server ()
{
  fonts.DeleteAll();
  if (freetype_inited) 
  {
    FT_Done_FreeType (library);
  }
}

void csFreeType2Server::Report (int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csRef<iReporter> rep (CS_QUERY_REGISTRY (object_reg, iReporter));
  if (rep)
    rep->ReportV (severity, "crystalspace.font.freefont2", msg, arg);
  else
  {
    csPrintfV (msg, arg);
    csPrintf ("\n");
  }
  va_end (arg);
}

const char* csFreeType2Server::GetErrorDescription(int code)
{
  #undef __FTERRORS_H__
  #define FT_ERRORDEF( e, v, s )  case v: return s;
  #define FT_ERROR_START_LIST     switch (code) {
  #define FT_ERROR_END_LIST       default: return "unknown error"; }
  #include FT_ERRORS_H
}

bool csFreeType2Server::Initialize (iObjectRegistry *object_reg)
{
  csFreeType2Server::object_reg = object_reg;

  freetype_inited = !FT_Init_FreeType (&library);

  if (!freetype_inited)
  {
    Report (CS_REPORTER_SEVERITY_ERROR,
      "Could not create a FreeType engine instance !");
    return false;
  }

  VFS = CS_QUERY_REGISTRY (object_reg, iVFS);
  ftconfig.AddConfig(object_reg, "config/freetype.cfg");

  defaultSize = ftconfig->GetInt ("Freetype2.Settings.Size", 10);
  platform_id = ftconfig->GetInt ("Freetype2.Settings.PlatformID", 3);
  encoding_id = ftconfig->GetInt ("Freetype2.Settings.EncodingID", 1);

  fontset = ftconfig->GetStr ("Freetype2.Settings.FontSet", 0);
  //  DEBUG_BREAK;
  csString s;
  s << fontset << '.';
  csRef<iConfigIterator> fontenum (ftconfig->Enumerate (s));
  while (fontenum->Next ())
    if (fontenum->GetKey (true) [0] == '*')
    {
      csRef<iFont> fnt (LoadFont (fontenum->GetKey (true)));
    }

  return true;
}

csPtr<iFont> csFreeType2Server::LoadFont (const char *filename)
{
  // First of all look for an alias in config file
  if (ftconfig && fontset)
  {
    csString Keyname;
    Keyname << fontset << '.' << filename;
    const char *s = ftconfig->GetStr (Keyname, 0);
    if (s) filename = s;
  }

  // see if we already loaded that font
  int idx = fonts.FindKey (filename);
  if (idx >= 0)
  {
    csFreeType2Font *font = fonts.Get (idx);
    font->IncRef ();
    return font;
  }

  // not yet loaded, so do it now
  csFreeType2Font *font = new csFreeType2Font (filename);
  if (!font->Load (VFS, this))
  {
    delete font;
    return 0;
  }
  fonts.Put (font);
  return font;
}

iFont *csFreeType2Server::GetFont (int iIndex)
{
  if (iIndex >= 0 && iIndex < fonts.Length ())
  {
    csFreeType2Font *font = fonts.Get (iIndex);
    if (font) return font;
  }
  return 0;
}

//-------------------------------------------// A FreeType font object //----//

SCF_IMPLEMENT_IBASE (csFreeType2Font)
  SCF_IMPLEMENTS_INTERFACE (iFont)
SCF_IMPLEMENT_IBASE_END

csFreeType2Font::csFreeType2Font (const char *filename) : DeleteCallbacks (4, 4)
{
  SCF_CONSTRUCT_IBASE (0);
  name = csStrNew (filename);
  face = 0;
  current = 0;
  fontdata = 0;
}

csFreeType2Font::~csFreeType2Font ()
{
  int i;
  for (i = DeleteCallbacks.Length () - 1; i >= 0; i--)
  {
    iFontDeleteNotify* delnot = (iFontDeleteNotify*)(DeleteCallbacks.Get (i));
    delnot->BeforeDelete (this);
    delnot->DecRef ();
  }
  if (face)
  {
    FT_Done_Face (face);
  }
  delete [] fontdata;
  delete [] name;
}

void csFreeType2Font::SetSize (int iSize)
{
  CreateGlyphBitmaps (iSize);
  current = FindGlyphSet (iSize);
}

int csFreeType2Font::GetSize ()
{
  return current ? current->size : 0;
}

void csFreeType2Font::GetMaxSize (int &oW, int &oH)
{
  if (current)
  {
    oW = current->maxW;
    oH = current->maxH;
  }
  else
    oW = oH = 0;
}

bool csFreeType2Font::GetGlyphSize (uint8 c, int &oW, int &oH, int &adv, int &left, int &top)
{
  if (!current || !current->glyphs[c].isOk) return false;
  
  const GlyphBitmap &glyph = current->glyphs[c];
  oW = MAX( glyph.advance, glyph.width);
  oH = glyph.rows;
  adv = glyph.advance;
  left = glyph.left;
  top = glyph.top;
  return true;
}

bool csFreeType2Font::GetGlyphSize (uint8 c, int &oW, int &oH)
{
  if (!current || !current->glyphs[c].isOk) return false;
  
  const GlyphBitmap &glyph = current->glyphs[c];

  oW = MAX( glyph.advance, glyph.width);
  oH = current->maxH;
  return true;
}

uint8 *csFreeType2Font::GetGlyphBitmap (uint8 c, int &oW, int &oH, int &adv, int &left, int &top)
{
  if (!current || !current->glyphs[c].isOk) return 0;

  const GlyphBitmap &glyph = current->glyphs[c];
  oW = MAX( glyph.advance, glyph.width);
  oH = glyph.rows;
  adv = glyph.advance;
  left = glyph.left;
  top = glyph.top;
  return glyph.bitmap;
}

uint8 *csFreeType2Font::GetGlyphBitmap (uint8 c, int &oW, int &oH)
{
  if (!current || !current->glyphs[c].isOk) return 0;

  const GlyphBitmap &glyph = current->glyphs[c];
  oW = MAX( glyph.advance, glyph.width);
  oH = current->maxH;
  return glyph.bitmap;
}

uint8 *csFreeType2Font::GetGlyphAlphaBitmap (uint8 c, int &oW, int &oH, int &adv, int &left, int &top)
{
  if (!current || !current->glyphs[c].isOk) return 0;

  const GlyphBitmap &glyph = current->glyphs[c];
  oW = MAX( glyph.advance, glyph.width);
  oH = glyph.rows;
  adv = glyph.advance;
  left = glyph.left;
  top = glyph.top;
  return glyph.alphabitmap;
}

uint8 *csFreeType2Font::GetGlyphAlphaBitmap (uint8 c, int &oW, int &oH)
{
  if (!current || !current->glyphs[c].isOk) return 0;

  const GlyphBitmap &glyph = current->glyphs[c];
  oW = MAX( glyph.advance, glyph.width);
  oH = current->maxH;
  return glyph.alphabitmap;
}

void csFreeType2Font::GetDimensions (const char *text, int &oW, int &oH, int &desc)
{
  if (!text || !current)
  {
    oW = oH = 0;
    return;
  }

  oW = 0; oH = 0; desc = 0;
  int h, d;
  while (*text)
  {
    const GlyphBitmap &glyph = current->glyphs[*(const uint8 *)text];
    oW += glyph.advance;
    h = glyph.top;
    d = h-glyph.rows;
    h -=  MIN(0, d); // add the descender
    oH = MAX (oH, h);
    desc = MAX (desc, -d);
    text++;
  }
}

void csFreeType2Font::GetDimensions (const char *text, int &oW, int &oH)
{
  if (!text || !current)
  {
    oW = oH = 0;
    return;
  }

  oW = 0; oH = current->maxH;
  while (*text)
  {
    const GlyphBitmap &glyph = current->glyphs[*(const uint8 *)text];
    oW += MAX(glyph.advance, glyph.width);
    text++;
  }
}

int csFreeType2Font::GetLength (const char *text, int maxwidth)
{
  if (!text || !current)
    return 0;

  int count = 0, w = 0;
  while (*text)
  {
    w += current->glyphs[*(const uint8 *)text].advance;
    if (w > maxwidth)
      break;
    text++; count++;
  }
  return count;
}

bool csFreeType2Font::Load (iVFS *pVFS, csFreeType2Server *server)
{
  int error;
  csRef<iFile> file (pVFS->Open (name, VFS_FILE_READ));
  if (file)
  {
    size_t size = file->GetSize ();
    if (size)
    {
      delete [] fontdata;
      fontdata = 0;
      fontdata = new FT_Byte[size];
      if (file->Read ((char*)fontdata, size) != size)
      {
        server->Report (CS_REPORTER_SEVERITY_WARNING,
                        "Font file %s could not be read!\n", name);
        return false;
        
      }
      // @@@ kludge: don't report error on CSF files(or?)
      if ((size >= 3) && (!strncmp ((const char*)fontdata, "CSF", 3)))
	return false;
      if ((error=FT_New_Memory_Face (server->library, fontdata, size, 0, &face)))
      {
        server->Report (CS_REPORTER_SEVERITY_WARNING,
	  "Font file %s could not be loaded: %s (%d)\n", 
	  name, server->GetErrorDescription (error), error);
        return false;
      }
    }
    else
    {
      server->Report (CS_REPORTER_SEVERITY_WARNING,
                      "Could not determine filesize for fontfile %s!\n", name);
      return false;
    }
  }
  else
  {
    server->Report (CS_REPORTER_SEVERITY_WARNING,
                    "Could not open fontfile %s!\n", name);
    return false;
  }

  // we do not change the default values of the new instance

  // next we scan the charmap table if there is an encoding
  // that matches the requested platform and encoding ids
  FT_UShort i = 0;
  FT_CharMap charmap = 0;
  while (i < face->num_charmaps)
  {
    FT_CharMap cm = face->charmaps[i];
    if (server->platform_id == cm->platform_id && server->encoding_id == cm->encoding_id)
    {
      charmap = cm;
      break;
    }
    i++;
  }

  if (!charmap)
  {
    // encoding scheme not found
    server->Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Font %s does not contain encoding %d for platform %d.",
      name, server->encoding_id, server->platform_id);

    charmap = face->charmaps[0];
    server->Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Will instead use encoding %d for platform %d.", charmap->encoding_id, charmap->platform_id);
  }

  if ((error=FT_Set_Charmap (face, charmap)))
  {
    server->Report (CS_REPORTER_SEVERITY_WARNING,
      "Could not set CharMap: %s(%d)", 
      server->GetErrorDescription (error), error);
    return false;
  }
  // now we create the bitmap of all glyphs in the face
  return CreateGlyphBitmaps (server->defaultSize);
}

bool csFreeType2Font::CreateGlyphBitmaps (int size)
{
  if (FindGlyphSet (size))
    return true;

  if (FT_Set_Char_Size (face, 0, 
    size * 64, 96, 96))
    return false;
  //    DEBUG_BREAK;
  // Create the glyphset
  GlyphSet *glyphset;
  glyphset = new GlyphSet;
  glyphset->size = size;
//  int maxrows = (-face->size->metrics.descender + face->size->metrics.ascender + 63)>>6;
  int maxrows = (face->size->metrics.height + 63)>>6;
  glyphset->maxW = (face->size->metrics.max_advance + 63) >> 6;
  glyphset->maxH = maxrows;
  glyphset->ascend = (face->size->metrics.ascender + 63)>>6;
  glyphset->descend = (-face->size->metrics.descender + 63)>>6;
  int bitmapsize, stride, n, i;

  memset (glyphset->glyphs, 0, sizeof (glyphset->glyphs));
  cache.Push (glyphset);

  for (FT_UShort iso_char = 0; iso_char < 256; iso_char++)
  {
    FT_Glyph glyph;
    if (FT_Load_Char (face, iso_char, FT_LOAD_RENDER|FT_LOAD_MONOCHROME) || FT_Get_Glyph (face->glyph, &glyph))
      continue;

    GlyphBitmap &g = glyphset->glyphs[iso_char];
    g.isOk = true;
    g.advance = glyph->advance.x >> 16;
    //stride = MAX((g.advance+7)/8, ((FT_BitmapGlyph)glyph)->bitmap.pitch);
    stride = MAX((g.advance+7)/8, (((FT_BitmapGlyph)glyph)->bitmap.width+7)/8);
    g.bitmap = new unsigned char [bitmapsize=maxrows*stride];
    memset (g.bitmap, 0, bitmapsize);
    int startrow = maxrows-(glyphset->descend + ((FT_BitmapGlyph)glyph)->top);
    int endrow = startrow+((FT_BitmapGlyph)glyph)->bitmap.rows;
    if (startrow < 0) startrow = 0;
    if (endrow > maxrows) endrow = maxrows;
    for (n=0,i=startrow; i < endrow; i++,n++)
      memcpy (g.bitmap + stride*i, 
              ((FT_BitmapGlyph)glyph)->bitmap.buffer + n*((FT_BitmapGlyph)glyph)->bitmap.pitch,
              /*((FT_BitmapGlyph)glyph)->bitmap.pitch*/MIN(stride, ((FT_BitmapGlyph)glyph)->bitmap.pitch));

    g.width = ((FT_BitmapGlyph)glyph)->bitmap.width;
    //    g.rows = ((FT_BitmapGlyph)glyph)->bitmap.rows;
    g.rows = maxrows;
    g.left = ((FT_BitmapGlyph)glyph)->left;
    //    g.top = ((FT_BitmapGlyph)glyph)->top;
    g.top = maxrows-glyphset->descend;

    //    FT_Done_Glyph (glyph);

    if (FT_Load_Char (face, iso_char, FT_LOAD_RENDER) || FT_Get_Glyph (face->glyph, &glyph))
      continue;

    stride = MAX(g.advance, ((FT_BitmapGlyph)glyph)->bitmap.width);
    g.alphabitmap = new unsigned char [bitmapsize=maxrows*stride];
    memset (g.alphabitmap, 0, bitmapsize);
    startrow = maxrows-(glyphset->descend + ((FT_BitmapGlyph)glyph)->top);
    endrow = startrow+((FT_BitmapGlyph)glyph)->bitmap.rows;
    if (startrow < 0) startrow = 0;
    if (endrow > maxrows) endrow = maxrows;
    for (n=0,i=startrow; i < endrow; i++,n++)
      memcpy (g.alphabitmap + stride*i, 
              ((FT_BitmapGlyph)glyph)->bitmap.buffer + n*((FT_BitmapGlyph)glyph)->bitmap.pitch,
              MIN(stride, ((FT_BitmapGlyph)glyph)->bitmap.pitch));
  }

  current = glyphset;
  return true;
}

void csFreeType2Font::AddDeleteCallback (iFontDeleteNotify* func)
{
  DeleteCallbacks.Push ((void *)func);
  func->IncRef ();
}

bool csFreeType2Font::RemoveDeleteCallback (iFontDeleteNotify* func)
{
  int i;
  for (i = DeleteCallbacks.Length () - 1; i >= 0; i--)
  {
    iFontDeleteNotify* delnot = (iFontDeleteNotify*)(DeleteCallbacks.Get (i));
    if (delnot == func)
    {
      DeleteCallbacks.Delete (i);
      func->DecRef ();
      return true;
    }
  }
  return false;
}

int csFreeType2Font::GetAscent ()
{
  return current ? current->ascend : -1;
}

int csFreeType2Font::GetDescent ()
{
  return current ? current->descend : -1;
}
