/*
    Copyright (C) 1998 by Jorrit Tyberghein

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

#include "sysdef.h"
#include "cssys/sysdriv.h"
#include "video/canvas/openglcommon/glcommon2d.h"
#include "video/canvas/common/scrshot.h"
#include "csutil/csrect.h"
#include "isystem.h"

IMPLEMENT_IBASE (csGraphics2DGLCommon)
  IMPLEMENTS_INTERFACE (iPlugIn)
  IMPLEMENTS_INTERFACE (iGraphics2D)
IMPLEMENT_IBASE_END

csGraphics2DGLCommon::csGraphics2DGLCommon (iBase *iParent) :
  csGraphics2D (),
  LocalFontServer (NULL)
{
  CONSTRUCT_IBASE (iParent);
}

bool csGraphics2DGLCommon::Initialize (iSystem *pSystem)
{
  if (!csGraphics2D::Initialize (pSystem))
    return false;

  // We don't really care about pixel format, except for ScreenShot ()
#if defined (CS_BIG_ENDIAN)
  pfmt.RedMask   = 0xff000000;
  pfmt.GreenMask = 0x00ff0000;
  pfmt.BlueMask  = 0x0000ff00;
#else
  pfmt.RedMask   = 0x000000ff;
  pfmt.GreenMask = 0x0000ff00;
  pfmt.BlueMask  = 0x00ff0000;
#endif
  pfmt.PixelBytes = 4;
  pfmt.PalEntries = 0;
  pfmt.complete ();

  return true;
}

csGraphics2DGLCommon::~csGraphics2DGLCommon ()
{
  Close ();
}

bool csGraphics2DGLCommon::Open (const char *Title)
{
  if (glGetString (GL_RENDERER))
    CsPrintf (MSG_INITIALIZATION, "OpenGL renderer %s ", glGetString (GL_RENDERER));
  if (glGetString (GL_VERSION))
    CsPrintf (MSG_INITIALIZATION, "Version %s", glGetString(GL_VERSION));
  CsPrintf (MSG_INITIALIZATION, "\n");

  if (!csGraphics2D::Open (Title))
    return false;

  // load font 'server'
  if (LocalFontServer == NULL)
  {
    int nFonts = FontRenderer->GetFontCount ();
    LocalFontServer = new csGraphics2DOpenGLFontServer (nFonts, FontRenderer);
    for (int fontindex = 0; fontindex < nFonts; fontindex++)
      LocalFontServer->AddFont (fontindex);
  }

  Clear (0);
  return true;
}

void csGraphics2DGLCommon::Close(void)
{
  csGraphics2D::Close ();
  delete LocalFontServer;
  LocalFontServer = NULL;
}

void csGraphics2DGLCommon::Clear (int color)
{
  float r, g, b;
  switch (pfmt.PixelBytes)
  {
    case 1: // paletted colors
      r = float (Palette [color].red  ) / 255;
      g = float (Palette [color].green) / 255;
      b = float (Palette [color].blue ) / 255;
      break;
    case 2: // 16bit color
    case 4: // truecolor
      r = float (color & pfmt.RedMask  ) / pfmt.RedMask;
      g = float (color & pfmt.GreenMask) / pfmt.GreenMask;
      b = float (color & pfmt.BlueMask ) / pfmt.BlueMask;
      break;
    default:
      return;
  }
  glClearColor (r, g, b, 0.0);
  glClear (GL_COLOR_BUFFER_BIT);
}

void csGraphics2DGLCommon::SetRGB (int i, int r, int g, int b)
{
  csGraphics2D::SetRGB (i, r, g, b);
}

void csGraphics2DGLCommon::setGLColorfromint (int color)
{
  switch (pfmt.PixelBytes)
  {
    case 1: // paletted colors
      glColor3i (Palette [color].red, Palette [color].green, Palette [color].blue);
      break;
    case 2: // 16bit color
    case 4: // truecolor
      glColor3f (
        float (color & pfmt.RedMask  ) / pfmt.RedMask,
        float (color & pfmt.GreenMask) / pfmt.GreenMask,
        float (color & pfmt.BlueMask ) / pfmt.BlueMask);
      break;
    default:
      return;
  }
}

void csGraphics2DGLCommon::DrawLine (
  float x1, float y1, float x2, float y2, int color)
{
  // prepare for 2D drawing--so we need no fancy GL effects!
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_BLEND);
  glDisable (GL_DEPTH_TEST);
  setGLColorfromint (color);

  glBegin (GL_LINES);
  glVertex2i (GLint (x1), GLint (Height - y1 - 1));
  glVertex2i (GLint (x2), GLint (Height - y2 - 1));
  glEnd ();
}

void csGraphics2DGLCommon::DrawBox (int x, int y, int w, int h, int color)
{
  // prepare for 2D drawing--so we need no fancy GL effects!
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_BLEND);
  glDisable (GL_DEPTH_TEST);
  setGLColorfromint (color);

  glBegin (GL_QUADS);
  glVertex2i (x, Height - y - 1);
  glVertex2i (x + w - 1, Height - y - 1);
  glVertex2i (x + w - 1, Height - (y + h - 1) - 1);
  glVertex2i (x, Height - (y + h - 1) - 1);
  glEnd ();
}

void csGraphics2DGLCommon::DrawPixel (int x, int y, int color)
{
  // prepare for 2D drawing--so we need no fancy GL effects!
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_BLEND);
  glDisable (GL_DEPTH_TEST);
  setGLColorfromint(color);

  glBegin (GL_POINTS);
  glVertex2i (x, Height - y - 1);
  glEnd ();
}

void csGraphics2DGLCommon::WriteChar (int x, int y, int fg, int /*bg*/, char c)
{
  // prepare for 2D drawing--so we need no fancy GL effects!
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_BLEND);
  glDisable (GL_DEPTH_TEST);
  
  setGLColorfromint(fg);

  // in fact the WriteCharacter() method properly shifts over
  // the current modelview transform on each call, so that characters
  // are drawn left-to-write.  But we bypass that because we know the
  // exact x,y location of each letter.  We manipulate the transform
  // directly, so any shift in WriteCharacter() is effectively ignored
  // due to the Push/PopMatrix calls
//printf("%c x=%d\n", c, x);
  glPushMatrix();
  glTranslatef (x, Height - y - FontRenderer->GetCharHeight (Font, 'T'),0.0);

  LocalFontServer->WriteCharacter(c, Font);
  glPopMatrix ();
}

// This variable is usually NULL except when doing a screen shot:
// in this case it is a temporarily allocated buffer for glReadPixels ()
static UByte *screen_shot = NULL;

unsigned char* csGraphics2DGLCommon::GetPixelAt (int x, int y)
{
  return screen_shot ?
    (screen_shot + pfmt.PixelBytes * ((Height - 1 - y) * Width + x)) : NULL;
}

iImage *csGraphics2DGLCommon::ScreenShot ()
{
  if (pfmt.PixelBytes != 1 && pfmt.PixelBytes != 4)
    return NULL;

  int screen_width = Width * pfmt.PixelBytes;
  screen_shot = new UByte [screen_width * Height];
  if (!screen_shot) return NULL;

  //glPixelStore ()?
  glReadPixels (0, 0, Width, Height,
    pfmt.PixelBytes == 1 ? GL_COLOR_INDEX : GL_RGBA,
    GL_UNSIGNED_BYTE, screen_shot);

  csScreenShot *ss = new csScreenShot (this);

  delete [] screen_shot;
  screen_shot = NULL;

  return ss;
}
