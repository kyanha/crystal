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

#include "cssysdef.h"
#include "cssys/sysdriv.h"
#include "video/canvas/openglcommon/glcommon2d.h"
#include "video/canvas/common/scrshot.h"
#include "csutil/csrect.h"
#include "isystem.h"
#include "qint.h"

IMPLEMENT_IBASE (csGraphics2DGLCommon)
  IMPLEMENTS_INTERFACE (iPlugIn)
  IMPLEMENTS_INTERFACE (iGraphics2D)
IMPLEMENT_IBASE_END

csGraphics2DGLCommon::csGraphics2DGLCommon (iBase *iParent) :
  csGraphics2D (),
  FontCache (NULL)
{
  CONSTRUCT_IBASE (iParent);
  EventOutlet = NULL;
}

bool csGraphics2DGLCommon::Initialize (iSystem *pSystem)
{
  if (!csGraphics2D::Initialize (pSystem))
    return false;

  // We don't really care about pixel format, except for ScreenShot ()

  // We do now with copying frame data about with the dynamic textures.
  // Besides, screenshot doesn't work properly in 16bit modes.
  // This is now over-ridden in the glX drivers
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
  if (EventOutlet)
    EventOutlet->DecRef ();
}

bool csGraphics2DGLCommon::Open (const char *Title)
{
  // initialize font cache object
  if (!FontCache)
    FontCache = new GLFontCache (FontServer);

  if (!csGraphics2D::Open (Title))
    return false;

  const char *renderer = (const char *)glGetString (GL_RENDERER);
  const char *version = (const char *)glGetString (GL_VERSION);
  if (renderer || version)
  CsPrintf (MSG_INITIALIZATION, "OpenGL renderer: %s version %s\n",
    renderer ? renderer : "unknown", version ? version : "unknown");

  CsPrintf (MSG_INITIALIZATION, "Using %s mode at resolution %dx%d.\n",
	     FullScreen ? "full screen" : "windowed", Width, Height);

  glClearColor (0., 0., 0., 0.);
  glClearDepth (-1.0);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  glViewport (0, 0, Width, Height);
  Clear (0);

  return true;
}

void csGraphics2DGLCommon::Close ()
{
  delete FontCache;
  FontCache = NULL;
  csGraphics2D::Close ();
}

bool csGraphics2DGLCommon::BeginDraw ()
{
  if (!csGraphics2D::BeginDraw ())
    return false;
  if (FrameBufferLocked != 1)
    return true;

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho (0, Width, 0, Height, -1.0, 10.0);
  glViewport (0, 0, Width, Height);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  glColor3f (1., 0., 0.);
  glClearColor (0., 0., 0., 0.);

  return true;
}

void csGraphics2DGLCommon::SetClipRect (int xmin, int ymin, int xmax, int ymax)
{
  csGraphics2D::SetClipRect (xmin, ymin, xmax, ymax);
  FontCache->SetClipRect (xmin, Height - ymax, xmax, Height - ymin);
}

void csGraphics2DGLCommon::DecomposeColor (int iColor,
  GLubyte &oR, GLubyte &oG, GLubyte &oB)
{
  switch (pfmt.PixelBytes)
  {
    case 1: // paletted colors
      oR = Palette [iColor].red;
      oG = Palette [iColor].green;
      oB = Palette [iColor].blue;
      break;
    case 2: // 16bit color
    case 4: // truecolor
      oR = ((iColor & pfmt.RedMask  ) >> pfmt.RedShift  ) << (8 - pfmt.RedBits  );
      oG = ((iColor & pfmt.GreenMask) >> pfmt.GreenShift) << (8 - pfmt.GreenBits);
      oB = ((iColor & pfmt.BlueMask ) >> pfmt.BlueShift ) << (8 - pfmt.BlueBits );
      break;
  }
}

void csGraphics2DGLCommon::DecomposeColor (int iColor,
  float &oR, float &oG, float &oB)
{
  GLubyte r, g, b;
  DecomposeColor (iColor, r, g, b);
  oR = r / 255.0;
  oG = g / 255.0;
  oB = b / 255.0;
}

void csGraphics2DGLCommon::setGLColorfromint (int color)
{
  GLubyte r, g, b;
  DecomposeColor (color, r, g, b);
  glColor3ub (r, g, b);
}

void csGraphics2DGLCommon::Clear (int color)
{
  float r, g, b;
  DecomposeColor (color, r, g, b);
  glClearColor (r, g, b, 0.0);
  glClear (GL_COLOR_BUFFER_BIT);
}

void csGraphics2DGLCommon::SetRGB (int i, int r, int g, int b)
{
  csGraphics2D::SetRGB (i, r, g, b);
}

void csGraphics2DGLCommon::DrawLine (
  float x1, float y1, float x2, float y2, int color)
{
  if (!ClipLine (x1, y1, x2, y2, ClipX1, ClipY1, ClipX2, ClipY2))
  {
    // prepare for 2D drawing--so we need no fancy GL effects!
    glDisable (GL_TEXTURE_2D);
    glDisable (GL_BLEND);
    glDisable (GL_DEPTH_TEST);
    glDisable (GL_ALPHA_TEST);
    setGLColorfromint (color);

    // This is a workaround for a hard-to-really fix problem with OpenGL:
    // whole Y coordinates are "rounded" up, this leads to one-pixel-shift
    // compared to software line drawing. This is not exactly a bug (because
    // this is an on-the-edge case) but it's different, thus we'll slightly
    // shift whole coordinates down.
    if (QInt (y1) == y1) { y1 += 0.05; }
    if (QInt (y2) == y2) { y2 += 0.05; }

    glBegin (GL_LINES);
    glVertex2f (x1, Height - y1);
    glVertex2f (x2, Height - y2);
    glEnd ();
  }
}

void csGraphics2DGLCommon::DrawBox (int x, int y, int w, int h, int color)
{
  if ((x > ClipX2) || (y > ClipY2))
    return;
  if (x < ClipX1)
    w -= (ClipX1 - x), x = ClipX1;
  if (y < ClipY1)
    h -= (ClipY1 - y), y = ClipY1;
  if (x + w > ClipX2)
    w = ClipX2 - x;
  if (y + h > ClipY2)
    h = ClipY2 - y;
  if ((w <= 0) || (h <= 0))
    return;                                                                                        

  y = Height - y;
  // prepare for 2D drawing--so we need no fancy GL effects!
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_BLEND);
  glDisable (GL_DEPTH_TEST);
  setGLColorfromint (color);

  glBegin (GL_QUADS);
  glVertex2i (x, y);
  glVertex2i (x + w, y);
  glVertex2i (x + w, y - h);
  glVertex2i (x, y - h);
  glEnd ();
}

void csGraphics2DGLCommon::DrawPixel (int x, int y, int color)
{
  if ((x >= ClipX1) && (y < ClipX2) && (y >= ClipY1) && (y < ClipY2))
  {
    // prepare for 2D drawing--so we need no fancy GL effects!
    glDisable (GL_TEXTURE_2D);
    glDisable (GL_BLEND);
    glDisable (GL_DEPTH_TEST);
    setGLColorfromint(color);

    glBegin (GL_POINTS);
    glVertex2i (x, Height - y);
    glEnd ();
  }    
}

void csGraphics2DGLCommon::Write (iFont *font, int x, int y, int fg, int bg, const char *text)
{
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_BLEND);
  glDisable (GL_DEPTH_TEST);

  if (bg >= 0)
  {
    int fw, fh;
    font->GetDimensions (text, fw, fh);
    DrawBox (x, y, fw, fh, bg);
  }

  setGLColorfromint (fg);
  FontCache->Write (font, x, Height - y, text);
}

// This variable is usually NULL except when doing a screen shot:
// in this case it is a temporarily allocated buffer for glReadPixels ()
static UByte *screen_shot = NULL;

unsigned char* csGraphics2DGLCommon::GetPixelAt (int x, int y)
{
  return screen_shot ?
    (screen_shot + pfmt.PixelBytes * ((Height - y) * Width + x)) : NULL;
}

csImageArea *csGraphics2DGLCommon::SaveArea (int x, int y, int w, int h)
{
  // For the time being copy data into system memory.
  if (pfmt.PixelBytes != 1 && pfmt.PixelBytes != 4)
    return NULL;

  // Convert to Opengl co-ordinate system
  y = Height - (y + h);

  if (x < 0)
  { w += x; x = 0; }
  if (x + w > Width)
    w = Width - x;
  if (y < 0)
  { h += y; y = 0; }
  if (y + h > Height)
    h = Height - y;
  if ((w <= 0) || (h <= 0))
    return NULL;

  csImageArea *Area = new csImageArea (x, y, w, h);
  if (!Area)
    return NULL;
  int actual_width = pfmt.PixelBytes * w;
  GLubyte* dest = new GLubyte [actual_width * h];
  Area->data = (char *)dest;
  if (!dest)
  {
    delete Area;
    return NULL;
  }
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_BLEND);
  glDisable (GL_DEPTH_TEST);
  glDisable (GL_DITHER);
  glDisable (GL_ALPHA_TEST);
  glReadPixels (x, y, w, h, pfmt.PixelBytes == 1 ? GL_COLOR_INDEX : GL_RGBA,
    GL_UNSIGNED_BYTE, dest);

  return Area;
}

void csGraphics2DGLCommon::RestoreArea (csImageArea *Area, bool Free)
{
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_BLEND);
  glDisable (GL_DEPTH_TEST);
  glDisable (GL_DITHER);
  glDisable (GL_ALPHA_TEST);
  if (Area)
  {
    glRasterPos2i (Area->x, Area->y);
    glDrawPixels (Area->w, Area->h, pfmt.PixelBytes == 1 ? GL_COLOR_INDEX : GL_RGBA,
      GL_UNSIGNED_BYTE, Area->data);
    glFlush ();
    if (Free)
      FreeArea (Area);
  } /* endif */
}

iImage *csGraphics2DGLCommon::ScreenShot ()
{
#ifndef GL_VERSION_1_2
  if (pfmt.PixelBytes != 1 && pfmt.PixelBytes != 4)
    return NULL;
#endif

  // Need to resolve pixel alignment issues
  int screen_width = Width * pfmt.PixelBytes;
  screen_shot = new UByte [screen_width * Height];
  if (!screen_shot) return NULL;

  // glPixelStore ()?
  switch (pfmt.PixelBytes)
  {
    case 1:
      glReadPixels (0, 0, Width, Height, GL_COLOR_INDEX,
		    GL_UNSIGNED_BYTE, screen_shot);
      break;
#ifdef GL_VERSION_1_2
    case 2:
      // experimental
      glReadPixels (0, 0, Width, Height, GL_RGB,
		    GL_UNSIGNED_SHORT_5_6_5, screen_shot);
      break;
#endif
    default:
      glReadPixels (0, 0, Width, Height, GL_RGBA,
		    GL_UNSIGNED_BYTE, screen_shot);
      break;
  }
  csScreenShot *ss = new csScreenShot (this);

  delete [] screen_shot;
  screen_shot = NULL;

  return ss;
}

bool csGraphics2DGLCommon::PerformExtension (const char* iCommand, ...)
{
  if (!strcasecmp (iCommand, "flush"))
  {
    glFlush ();
    glFinish ();
    return true;
  }

  return false;
}

