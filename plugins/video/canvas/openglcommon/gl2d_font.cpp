/*
    Copyright (c) 1999 Gary Haussmann
    Accelerated by Samuel Humphreys

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

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "cssysdef.h"
#include "csutil/util.h"
#include "qint.h"
#include "video/canvas/common/graph2d.h"
#include "video/canvas/openglcommon/glstates.h"

#include "gl2d_font.h"

#if !defined (OPENGL_BITMAP_FONT)

struct GLGlyph
{
  // the texture where we find this character in
  GLuint hTexture;
  // Glyph width - in pixels
  int width;
  // Glyph width - relative to texture width
  float texwidth;
  // location of the character
  float x, y;
};

struct GLGlyphSet : public GLProtoGlyphSet
{
  // The height of every glyph - in 0..1 range (e.g. height / texture height)
  float texheight;
  // The glyphs
  GLGlyph glyphs [256];

  // Constructor
  GLGlyphSet (iFont *Font);
  // The destructor
  ~GLGlyphSet ();
};

GLGlyphSet::GLGlyphSet (iFont *Font)
{
  (font = Font)->IncRef ();
  size = Font->GetSize ();
}

GLGlyphSet::~GLGlyphSet ()
{
  GLuint hTex = glyphs [0].hTexture + 1;
  int i;
  for (i = 0; i < 256; i++)
  {
    if (hTex != glyphs [i].hTexture)
    {
      hTex = glyphs [i].hTexture;
      glDeleteTextures (1, &glyphs [i].hTexture);
    }
  }
  font->DecRef ();
}

//-----------------------------------------------------// GLGlyphVector //----//

GLFontCache::GLGlyphVector::~GLGlyphVector ()
{
  DeleteAll ();
}

bool GLFontCache::GLGlyphVector::FreeItem (csSome Item)
{
  delete (GLGlyphSet *)Item;
  return true;
}

//-------------------------------------------------------// GLFontCache //----//

struct FontDeleteNotify : public iFontDeleteNotify
{
  void* glyphset;
  SCF_DECLARE_IBASE;
  FontDeleteNotify () { SCF_CONSTRUCT_IBASE (NULL); }
  virtual ~FontDeleteNotify () { }
  virtual void BeforeDelete (iFont* font);
};

SCF_IMPLEMENT_IBASE (FontDeleteNotify)
  SCF_IMPLEMENTS_INTERFACE (iFontDeleteNotify)
SCF_IMPLEMENT_IBASE_END

void FontDeleteNotify::BeforeDelete (iFont *font)
{
  GLFontCache *This = (GLFontCache *)glyphset;
  This->CacheFree (font);
}

GLFontCache::GLFontCache (iFontServer *fs, iGraphics2D *g) : FontCache (8, 8)
{
  g2d = g;
  g2d->PerformExtension( "getstatecache", &statecache );

  int i = 0;
  delete_callback = new FontDeleteNotify ();
  ((FontDeleteNotify*)delete_callback)->glyphset = this;
  iFont *font;
  if (!fs) return;
  while ((font = fs->GetFont (i++)))
    CacheFont (font);
}

GLFontCache::~GLFontCache ()
{
  // Remove deletion callbacks to avoid being deleted later -
  // when the font cache object will be already deleted
  int i;
  for (i = 0; i < FontCache.Length (); i++)
    FontCache.Get (i)->font->RemoveDeleteCallback (delete_callback);
  delete_callback->DecRef ();
}

GLGlyphSet *GLFontCache::CacheFont (iFont *font)
{
  // See if we have any instances of this font in the cache
  int i;
  for (i = FontCache.Length () - 1; i >= 0; i--)
    if (FontCache.Get (i)->font == font)
      break;
  if (i < 0)
    // Tell the font to notify us when it is freed
    font->AddDeleteCallback (delete_callback);

  GLGlyphSet *gs = new GLGlyphSet (font);
  FontCache.Push (gs);

  unsigned c;

  int maxwidth, maxheight;
  font->GetMaxSize (maxwidth, maxheight);
  const int basetexturewidth = 256;

  // figure out how many charcter rows we need
  int texwidth = 0;
  int rows = 1;
  for (c = 0; c < 256; c++)
  {
    int width, height;
    font->GetGlyphSize (c, width, height);
    texwidth += width;
    if (texwidth > basetexturewidth)
    {
      rows++;
      texwidth = width;
    }
  }

  int basetextureheight = csFindNearestPowerOf2 (MIN (maxheight * rows, 256));
  gs->texheight = float (maxheight) / basetextureheight;

  int nTextures = (rows * maxheight + 255) / 256;
  uint8 *fontbitmapdata, *characterbitmapbase;

  const int fontbitmapsize = basetexturewidth * basetextureheight;
  fontbitmapdata = new uint8 [fontbitmapsize];
  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
  // make new texture handles
  GLuint *nTexNames = new GLuint [nTextures];
  glGenTextures (nTextures, nTexNames);

  int nCurrentTex = 0;
  int x = 256, y = 256;
  for (c = 0; c < 256; c++)
  {
    int width, height;
    uint8 *fontsourcebits = font->GetGlyphBitmap (c, width, height);
    bool GlyphBitsNotByteAligned = width & 7;

    // calculate the start of this character
    if (x + width > basetexturewidth)
    {
      x = 0;
      y += maxheight;
      if (y + maxheight > basetextureheight)
      {
        y = 0;
        // if this is not the first handle we create, we hand over the data
        // to opengl
        if (c)
        {
          glTexImage2D (GL_TEXTURE_2D, 0 /*mipmap level */,
                        GL_ALPHA /* bytes-per-pixel */,
                        basetexturewidth, basetextureheight, 0 /*border*/,
                        GL_ALPHA, GL_UNSIGNED_BYTE, fontbitmapdata);
          nCurrentTex++;
        }
        // set up the texture info
        memset (fontbitmapdata, 0, fontbitmapsize);
#ifdef CS_USE_NEW_RENDERER'
        glBindTexture (GL_TEXTURE_2D, nTexNames [nCurrentTex]);
#else
        statecache->SetTexture (GL_TEXTURE_2D, nTexNames [nCurrentTex]);
#endif   
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      }
    }

    characterbitmapbase = fontbitmapdata + y * basetexturewidth + x;

    // Construct a buffer of data for OpenGL. We must do some transformation
    // of the Crystal Space data:
    // - use unsigned bytes instead of bits (GL_BITMAP not supported? GJH)
    // - width and maxheight must be a power of 2
    // - characters are laid out in a grid format, going across and
    //   then down

    int alpha_width,alpha_height;
    uint8 * alphasource = font->GetGlyphAlphaBitmap (
    	c, alpha_width, alpha_height);

    // grab bits from the source, and stuff them into the font bitmap
    // one at a time
    uint8 currentsourcebyte = *fontsourcebits;
    int pixelx, pixely;
    for (pixely = 0; pixely < maxheight; pixely++)
    {
      for (pixelx = 0; pixelx < width; pixelx++)
      {
	if (alphasource == NULL)
	{
	  // strip a bit off and dump it into the base bitmap
	  *characterbitmapbase++ = (currentsourcebyte & 128) ? 255 : 0;
	}
	else
	{
	  // Copy the value from the alphasource to bitmap
	  *characterbitmapbase++ = alphasource[pixely * width + pixelx];
	}

        if ((pixelx & 7) == 7)
          currentsourcebyte = *++fontsourcebits;
        else
          currentsourcebyte = currentsourcebyte << 1;
      }

      if (GlyphBitsNotByteAligned)
        currentsourcebyte = *++fontsourcebits;

      // advance base bitmap pointer to the next row
      characterbitmapbase += basetexturewidth - width;
    }

    GLGlyph &glyph = gs->glyphs [c];
    glyph.width = width;
    glyph.texwidth = ((float)width) / basetexturewidth;
    glyph.x = float (x) / basetexturewidth;
    glyph.y = float (y) / basetextureheight;
    glyph.hTexture = nTexNames [nCurrentTex];

    x += width;
  }

  if (x || y)
    glTexImage2D (GL_TEXTURE_2D, 0 /*mipmap level */,
                  GL_ALPHA /* bytes-per-pixel */,
                  basetexturewidth, basetextureheight, 0 /*border*/,
                  GL_ALPHA, GL_UNSIGNED_BYTE, fontbitmapdata);

  delete [] nTexNames;
  delete [] fontbitmapdata;
  return gs;
}

void GLFontCache::CacheFree (iFont *font)
{
  font->RemoveDeleteCallback (delete_callback);
  int i;
  for (i = FontCache.Length () - 1; i >= 0; i--)
  {
    GLGlyphSet *gs = FontCache.Get (i);
    if (gs->font == font)
      FontCache.Delete (i);
  }
}

bool GLFontCache::ClipRect (float x, float y,
  float &x1, float &y1, float &x2, float &y2,
  float &tx1, float &ty1, float &tx2, float &ty2)
{
  float nx1 = x1 + x, ny1 = y1 + y, nx2 = x2 + x, ny2 = y2 + y;
  float ntx1 = tx1, nty1 = ty1, ntx2 = tx2, nty2 = ty2;

  if ((nx1 > float (ClipX2)) || (nx2 < float (ClipX1))
   || (ny1 > float (ClipY2)) || (ny2 < float (ClipY1)))
      return false;

  if (nx1 < ClipX1)
    tx1 += (ntx2 - ntx1) * (ClipX1 - nx1) / (nx2 - nx1), x1 = ClipX1 - x;
  if (nx2 > ClipX2)
    tx2 -= (ntx2 - ntx1) * (nx2 - ClipX2) / (nx2 - nx1), x2 = ClipX2 - x;
  if (tx2 <= tx1)
    return false;

  if (ny1 < ClipY1)
    ty2 -= (nty2 - nty1) * (ClipY1 - ny1) / (ny2 - ny1), y1 = ClipY1 - y;
  if (ny2 > ClipY2)
    ty1 += (nty2 - nty1) * (ny2 - ClipY2) / (ny2 - ny1), y2 = ClipY2 - y;
  if (ty2 <= ty1)
    return false;

  return true;
}

// Rasterize string
void GLFontCache::Write (iFont *font, int x, int y, const char *text)
{
  if (!text || !*text) return;

  int idx = FontCache.FindKey (font);
  GLGlyphSet *gs = idx >= 0 ? FontCache.Get (idx) : CacheFont (font);
  if (!gs) return;

  int maxwidth, maxheight;
  font->GetMaxSize (maxwidth, maxheight);

  y = y - maxheight;
  if (y >= ClipY2) return;

  glPushMatrix ();
  glTranslatef (x, y, 0);

  statecache->Enable_GL_TEXTURE_2D ();
  statecache->SetShadeModel (GL_FLAT);
  glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  statecache->Enable_GL_BLEND ();
  statecache->SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifdef CS_USE_NEW_RENDERER
  glColorMask (true, true, true, true);
#endif
//  statecache->EnableState (GL_ALPHA_TEST);
//  statecache->SetAlphaFunc (GL_EQUAL, 1.0);

  float texheight = gs->texheight;

  GLGlyph *glyphs = gs->glyphs;
  GLuint hLastTexture = glyphs [*text].hTexture - 1;

  float x1 = 0.0;

  GLboolean vaenabled = glIsEnabled(GL_VERTEX_ARRAY);
  GLboolean tcaenabled = glIsEnabled(GL_VERTEX_ARRAY);
  GLboolean caenabled = glIsEnabled(GL_COLOR_ARRAY);

  if(vaenabled == GL_FALSE)
    glEnableClientState(GL_VERTEX_ARRAY);
  if(tcaenabled == GL_FALSE)
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  if(caenabled == GL_TRUE)
    glDisableClientState(GL_COLOR_ARRAY);

  int curvert = 0;
  for (; *text; ++text)
  {
    GLGlyph &glyph = glyphs [*text];

    float x_right, x2 = x_right = x1 + glyph.width;
    float tx1, tx2, ty1, ty2, y1, y2;

    GLuint hTexture = glyph.hTexture;
    if (hTexture != hLastTexture)
    {
      hLastTexture = hTexture;
#ifdef CS_USE_NEW_RENDERER
      glBindTexture (GL_TEXTURE_2D, hTexture);
#else
      statecache->SetTexture (GL_TEXTURE_2D, hTexture);
#endif

      if(verts2d.Length() > 0)
      {
        glTexCoordPointer(2, GL_FLOAT, sizeof(csVector2),texcoords.GetArray());
        glVertexPointer(2, GL_FLOAT, sizeof(csVector2), verts2d.GetArray());
        glDrawArrays(GL_QUADS, 0, curvert);
        curvert = 0;
      }
      
    }
    // the texture coordinates must point to the correct character
    // the texture is a strip a wide as a single character and
    // as tall as 256 characters.  We must select a single
    // character from it
    tx1 = glyph.x;
    tx2 = tx1 + glyph.texwidth;
    ty1 = glyph.y;
    ty2 = ty1 + texheight;
    y1 = 0.0;
    y2 = maxheight;

    if (ClipRect (x, y, x1, y1, x2, y2, tx1, ty1, tx2, ty2))
    {
      texcoords[curvert].x = tx1;
      texcoords[curvert].y = ty1;

      verts2d[curvert].x = x1;
      verts2d[curvert].y = y2;

      curvert++;

      texcoords[curvert].x = tx2;
      texcoords[curvert].y = ty1;

      verts2d[curvert].x = x2;
      verts2d[curvert].y = y2;

      curvert++;

      texcoords[curvert].x = tx2;
      texcoords[curvert].y = ty2;

      verts2d[curvert].x = x2;
      verts2d[curvert].y = y1;

      curvert++;
      
      texcoords[curvert].x = tx1;
      texcoords[curvert].y = ty2;

      verts2d[curvert].x = x1;
      verts2d[curvert].y = y1;

      curvert++;

      /*
      glTexCoord2f (tx1,ty1); glVertex2f (x1,y2);
      glTexCoord2f (tx2,ty1); glVertex2f (x2,y2);
      glTexCoord2f (tx2,ty2); glVertex2f (x2,y1);
      glTexCoord2f (tx1,ty2); glVertex2f (x1,y1);*/
    }

    x1 = x_right;
  }

  glTexCoordPointer(2, GL_FLOAT, sizeof(csVector2),texcoords.GetArray());
  glVertexPointer(2, GL_FLOAT, sizeof(csVector2), verts2d.GetArray());
  glDrawArrays(GL_QUADS, 0, curvert);

  if(vaenabled == GL_FALSE)
    glDisableClientState(GL_VERTEX_ARRAY);
  if(tcaenabled == GL_FALSE)
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  if(caenabled == GL_TRUE)
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  statecache->Disable_GL_BLEND ();
//  statecache->DisableState (GL_ALPHA_TEST);
  glPopMatrix ();
}

#endif
