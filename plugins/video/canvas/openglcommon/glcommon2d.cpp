/*
    Copyright (C) 1998-2001 by Jorrit Tyberghein

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
#include "glcommon2d.h"
#include "cssys/csendian.h"
#include "video/canvas/common/scrshot.h"
#include "csgeom/csrect.h"
#include "iutil/objreg.h"
#include "ivaria/reporter.h"
#include "qint.h"
#include "video/canvas/openglcommon/glstates.h"

// This header should be moved
#include "video/renderer/common/pixfmt.h"


SCF_IMPLEMENT_IBASE_EXT (csGraphics2DGLCommon)
  SCF_IMPLEMENTS_INTERFACE (iEventPlug)
SCF_IMPLEMENT_IBASE_EXT_END

csGraphics2DGLCommon::csGraphics2DGLCommon (iBase *iParent) :
  csGraphics2D (iParent), FontCache (0)
{
  EventOutlet = 0;
  screen_shot = 0;
  multiSamples = 0;
  multiFavorQuality = false;
}

bool csGraphics2DGLCommon::Initialize (iObjectRegistry *object_reg)
{
  if (!csGraphics2D::Initialize (object_reg))
    return false;

  // We don't really care about pixel format, except for ScreenShot()
  // and OpenGL software proctexes
#if (CS_24BIT_PIXEL_LAYOUT == CS_24BIT_PIXEL_ABGR)
    pfmt.RedMask = 0x000000FF;
    pfmt.GreenMask = 0x0000FF00;
    pfmt.BlueMask = 0x00FF0000;
#else 
    pfmt.RedMask = 0x00FF0000;
    pfmt.GreenMask = 0x0000FF00;
    pfmt.BlueMask = 0x000000FF;
#endif
  pfmt.PixelBytes = 4;
  pfmt.PalEntries = 0;
  pfmt.complete ();

  statecache = new csGLStateCache();

  ext.Initialize (object_reg, this);

  depthBits = config->GetInt ("Video.OpenGL.DepthBits", 32);
  multiSamples = config->GetInt ("Video.OpenGL.MultiSamples", 0);
  multiFavorQuality = config->GetBool ("Video.OpenGL.MultisampleFavorQuality");

  return true;
}

csGraphics2DGLCommon::~csGraphics2DGLCommon ()
{
  Close ();
  
  delete statecache;
  delete[] screen_shot;
}

bool csGraphics2DGLCommon::Open ()
{
  if (is_open) return true;

  statecache->InitCache();

  // initialize font cache object
  if (!FontCache)
    FontCache = new GLFontCache (FontServer, this);

  if (!csGraphics2D::Open ())
    return false;

  const char *renderer = (const char *)glGetString (GL_RENDERER);
  const char *vendor = (const char *)glGetString (GL_VENDOR);
  const char *version = (const char *)glGetString (GL_VERSION);
  csRef<iReporter> reporter (CS_QUERY_REGISTRY (object_reg, iReporter));
  if (renderer || version || vendor)
    if (reporter)
      reporter->Report (CS_REPORTER_SEVERITY_NOTIFY,
        "crystalspace.canvas.openglcommon",
        "OpenGL renderer: %s (vendor: %s) version %s",
        renderer ? renderer : "unknown", vendor ? vendor: "unknown", 
	version ? version : "unknown");

  if (reporter)
    reporter->Report (CS_REPORTER_SEVERITY_NOTIFY,
      "crystalspace.canvas.openglcommon",
      "Using %s mode at resolution %dx%d.",
      FullScreen ? "full screen" : "windowed", Width, Height);

  ext.Open ();

  if (version)
  {
    // initialize GL version pseudo-extensions
    int n, vMajor, vMinor, vRelease;
    n = sscanf (version, "%d.%d.%d", &vMajor, &vMinor, &vRelease);
    if (n >= 2)
    {
      if ((vMajor >= 1) || ((vMajor == 1) && (vMinor >= 2)))
      {
	//ext.InitGL_version_1_2 ();
      }
      if ((vMajor >= 1) || ((vMajor == 1) && (vMinor >= 3)))
      {
	//ext.InitGL_version_1_3 ();
      }
    }
  }

  ext.InitGL_ARB_multisample();

  if (ext.CS_GL_ARB_multisample)
  {
    GLint glmultisamp = (GLint)multiSamples;
    glGetIntegerv (GL_SAMPLES_ARB, &glmultisamp);

    if (multiSamples)
    {
      if (reporter)
	reporter->Report (CS_REPORTER_SEVERITY_NOTIFY,
	  "crystalspace.canvas.openglcommon",
	  "Multisample: %d samples",
	  multiSamples);

      ext.InitGL_NV_multisample_filter_hint();
      if (ext.CS_GL_NV_multisample_filter_hint)
      {
	glHint (GL_MULTISAMPLE_FILTER_HINT_NV,
	  multiFavorQuality ? GL_NICEST : GL_FASTEST);
	
	GLint msHint;
	glGetIntegerv (GL_MULTISAMPLE_FILTER_HINT_NV, &msHint);
	if (reporter)
	  reporter->Report (CS_REPORTER_SEVERITY_NOTIFY,
	    "crystalspace.canvas.openglcommon",
	    "Multisample settings: %s",
	    ((msHint == GL_NICEST) ? "quality" :
	    ((msHint == GL_FASTEST) ? "performance" : "unknown")));
      }
    }
    else
    {
      if (reporter)
	reporter->Report (CS_REPORTER_SEVERITY_NOTIFY,
	  "crystalspace.canvas.openglcommon",
	  "Multisample: disabled");
    }
  }

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
  if (!is_open) return;
  delete FontCache;
  FontCache = 0;
  ext.Close ();
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
  if (FontCache)
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
      oR = ((iColor & pfmt.RedMask  ) >> pfmt.RedShift  );
      oG = ((iColor & pfmt.GreenMask) >> pfmt.GreenShift);
      oB = ((iColor & pfmt.BlueMask ) >> pfmt.BlueShift );
      oR = oR << (8-pfmt.RedBits);
      oG = oG << (8-pfmt.GreenBits);
      oB = oB << (8-pfmt.BlueBits);
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
    bool gl_texture2d = statecache->IsEnabled_GL_TEXTURE_2D ();
    bool gl_alphaTest = glIsEnabled(GL_ALPHA_TEST);
    if (gl_texture2d) statecache->Disable_GL_TEXTURE_2D ();
    if (gl_alphaTest) statecache->Disable_GL_ALPHA_TEST ();
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

    if (gl_texture2d) statecache->Enable_GL_TEXTURE_2D ();
    if (gl_alphaTest) statecache->Enable_GL_ALPHA_TEST ();
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
  bool gl_texture2d = statecache->IsEnabled_GL_TEXTURE_2D ();
  if (gl_texture2d) statecache->Disable_GL_TEXTURE_2D ();
  setGLColorfromint (color);

  glBegin (GL_QUADS);
  glVertex2i (x, y);
  glVertex2i (x + w, y);
  glVertex2i (x + w, y - h);
  glVertex2i (x, y - h);
  glEnd ();

  if (gl_texture2d) statecache->Enable_GL_TEXTURE_2D ();
}

void csGraphics2DGLCommon::DrawPixel (int x, int y, int color)
{
  if ((x >= ClipX1) && (x < ClipX2) && (y >= ClipY1) && (y < ClipY2))
  {
    // prepare for 2D drawing--so we need no fancy GL effects!
    bool gl_texture2d = statecache->IsEnabled_GL_TEXTURE_2D ();
    if (gl_texture2d) statecache->Disable_GL_TEXTURE_2D ();
    setGLColorfromint (color);
    glBegin (GL_POINTS);
    glVertex2i (x, Height - y);
    glEnd ();

    if (gl_texture2d) statecache->Enable_GL_TEXTURE_2D ();
  }
}

void csGraphics2DGLCommon::DrawPixels (csPixelCoord* pixels,
	int num_pixels, int color)
{
  // prepare for 2D drawing--so we need no fancy GL effects!
  bool gl_texture2d = statecache->IsEnabled_GL_TEXTURE_2D ();
  if (gl_texture2d) statecache->Disable_GL_TEXTURE_2D ();
  setGLColorfromint (color);

  int i;
  glBegin (GL_POINTS);
  for (i = 0 ; i < num_pixels ; i++)
  {
    int x = pixels->x;
    int y = pixels->y;
    pixels++;
    if ((x >= ClipX1) && (x < ClipX2) && (y >= ClipY1) && (y < ClipY2))
    {
      glVertex2i (x, Height - y);
    }
  }
  glEnd ();
  if (gl_texture2d) statecache->Enable_GL_TEXTURE_2D ();
}

void csGraphics2DGLCommon::Blit (int x, int y, int w, int h,
	unsigned char* data)
{
  int orig_x = x;
  int orig_y = y;
  int orig_w = w;
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

  // If vertical clipping is needed we skip the initial part.
  data += 4*w*(y-orig_y);
  // Same for horizontal clipping.
  data += 4*(x-orig_x);

  bool gl_texture2d = statecache->IsEnabled_GL_TEXTURE_2D ();
  bool gl_alphaTest = glIsEnabled(GL_ALPHA_TEST);
  if (gl_texture2d) statecache->Disable_GL_TEXTURE_2D ();
  if (gl_alphaTest) statecache->Disable_GL_ALPHA_TEST ();

  glColor3f (0., 0., 0.);
  int j;
  for (j = y ; j < y+h ; j++)
  {
    glRasterPos2i (x, Height-j);
    glDrawPixels (w, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    data += 4*orig_w;
  }

  if (gl_texture2d) statecache->Enable_GL_TEXTURE_2D ();
  if (gl_alphaTest) statecache->Enable_GL_ALPHA_TEST ();
}

void csGraphics2DGLCommon::Write (iFont *font, int x, int y, int fg, int bg,
  const char *text)
{
  if (bg >= 0)
  {
    bool gl_texture2d = statecache->IsEnabled_GL_TEXTURE_2D ();
    if (gl_texture2d) statecache->Disable_GL_TEXTURE_2D ();

    int fw, fh;
    font->GetDimensions (text, fw, fh);
    DrawBox (x, y, fw, fh, bg);

    if (gl_texture2d) statecache->Enable_GL_TEXTURE_2D ();
  }

  setGLColorfromint (fg);
  FontCache->Write (font, x, Height - y, text);
}

// This variable is usually 0 except when doing a screen shot:
// in this case it is a temporarily allocated buffer for glReadPixels ()
//static uint8 *screen_shot = 0;

unsigned char* csGraphics2DGLCommon::GetPixelAt (int x, int y)
{
  return screen_shot ?
    (screen_shot + pfmt.PixelBytes * ((Height - y - 1) * Width + x)) : 0;
}

csImageArea *csGraphics2DGLCommon::SaveArea (int x, int y, int w, int h)
{
  // For the time being copy data into system memory.
#ifndef GL_VERSION_1_2
  if (pfmt.PixelBytes != 1 && pfmt.PixelBytes != 4)
    return 0;
#endif
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
    return 0;

  csImageArea *Area = new csImageArea (x, y, w, h);
  if (!Area)
    return 0;
  int actual_width = pfmt.PixelBytes * w;
  GLubyte* dest = new GLubyte [actual_width * h];
  Area->data = (char *)dest;
  if (!dest)
  {
    delete Area;
    return 0;
  }
  bool gl_texture2d = statecache->IsEnabled_GL_TEXTURE_2D ();
  bool gl_alphaTest = glIsEnabled(GL_ALPHA_TEST);
  if (gl_texture2d) statecache->Disable_GL_TEXTURE_2D ();
  if (gl_alphaTest) statecache->Disable_GL_ALPHA_TEST ();
  //csGLStates::Disable_GL_DITHER ();
  GLenum format, type;
  switch (pfmt.PixelBytes)
  {
    case 1:
      format = GL_COLOR_INDEX;
      type = GL_UNSIGNED_BYTE;
      break;
#ifdef GL_VERSION_1_2
    case 2:
      format = GL_RGB;
      type = GL_UNSIGNED_SHORT_5_6_5;
      break;
#endif
    case 4:
      format = GL_RGBA;
      type = GL_UNSIGNED_BYTE;
      break;
    default:
      delete Area;
      return 0; // invalid format
  }
  glReadPixels (x, y, w, h, format, type, dest);

  if (gl_texture2d) statecache->Enable_GL_TEXTURE_2D ();
  if (gl_alphaTest) statecache->Enable_GL_ALPHA_TEST ();
  return Area;
}

void csGraphics2DGLCommon::RestoreArea (csImageArea *Area, bool Free)
{
  bool gl_texture2d = statecache->IsEnabled_GL_TEXTURE_2D ();
  bool gl_alphaTest = glIsEnabled(GL_ALPHA_TEST);
  if (gl_texture2d) statecache->Disable_GL_TEXTURE_2D ();
  if (gl_alphaTest) statecache->Disable_GL_ALPHA_TEST ();
  //csGLStates::Disable_GL_DITHER ();
  if (Area)
  {
    GLenum format, type;
    switch (pfmt.PixelBytes)
    {
      case 1:
        format = GL_COLOR_INDEX;
        type = GL_UNSIGNED_BYTE;
        break;
#ifdef GL_VERSION_1_2
      case 2:
        format = GL_RGB;
        type = GL_UNSIGNED_SHORT_5_6_5;
        break;
#endif
      case 4:
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        break;
      default:
        return; // invalid format
    }
    glRasterPos2i (Area->x, Area->y);
    glDrawPixels (Area->w, Area->h, format, type, Area->data);
    glFlush ();
    if (Free)
      FreeArea (Area);
  } /* endif */

  if (gl_texture2d) statecache->Enable_GL_TEXTURE_2D ();
  if (gl_alphaTest) statecache->Enable_GL_ALPHA_TEST ();
}

csPtr<iImage> csGraphics2DGLCommon::ScreenShot ()
{
/*#ifndef GL_VERSION_1_2
  if (pfmt.PixelBytes != 1 && pfmt.PixelBytes != 4)
    return 0;
#endif*/

  // Need to resolve pixel alignment issues
#ifdef GL_VERSION_1_2
  int screen_width = Width * pfmt.PixelBytes;
#else
  int screen_width = Width * (pfmt.PixelBytes == 1? 1 : 4);
#endif
  if (!screen_shot) screen_shot = new uint8 [screen_width * Height];
  //if (!screen_shot) return 0;

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

// Pixel format is read as RGBA (in a byte array) but as soon as we
// cast it to a 32 bit integer we have to deal with endianess, so convert
// to big endian and convert RGBA to ARGB
// On ABGR machines, we also need to swap B/R bytes
#ifdef GL_VERSION_1_2
  if (pfmt.PixelBytes == 4)
#else
  if (pfmt.PixelBytes > 1)
#endif
  {
    uint32* s = (uint32*)screen_shot;
    int i;
    for (i = 0 ; i < Width*Height ; i++)
    {
        *s = big_endian_long(*s);
#if (CS_24BIT_PIXEL_LAYOUT == CS_24BIT_PIXEL_ABGR)
        *s = ((*s & 0x000000FF) << 24) | ((*s & 0x0000FF00) << 8) |
                ((*s & 0x00FF0000) >> 8) | ((*s & 0xFF000000) >> 24);
#else 
        *s = ((*s & 0xFF) << 24) | ((*s & 0xFFFFFF00) >> 8);
#endif
  s++;
    }
  }

#ifndef GL_VERSION_1_2
  /* kludge: the data is already captured in the right format
    (RGBA). However, pfmt may contain the correct 16-bit
    masks and shift which csScreenShot will use to convert the
    data and thus totally garble it. 
   */
  csPixelFormat oldpfmt;
  memcpy (&oldpfmt, &pfmt, sizeof(csPixelFormat));

#if (CS_24BIT_PIXEL_LAYOUT == CS_24BIT_PIXEL_ABGR)
    pfmt.RedMask = 0x000000FF;
    pfmt.GreenMask = 0x0000FF00;
    pfmt.BlueMask = 0x00FF0000;
#else 
    pfmt.RedMask = 0x00FF0000;
    pfmt.GreenMask = 0x0000FF00;
    pfmt.BlueMask = 0x000000FF;
#endif
  pfmt.PixelBytes = 4;
  pfmt.PalEntries = 0;
  pfmt.complete ();
#endif

  csScreenShot *ss = new csScreenShot (this);

#ifndef GL_VERSION_1_2
  memcpy (&pfmt, &oldpfmt, sizeof(csPixelFormat));
#endif

  //delete [] screen_shot;
  //screen_shot = 0;

  return ss;
}

bool csGraphics2DGLCommon::PerformExtensionV (char const* command, va_list args)
{
  if (!strcasecmp (command, "flush"))
  {
    glFlush ();
    glFinish ();
    return true;
  }
  if (!strcasecmp (command, "getstatecache"))
  {
    csGLStateCache** cache = va_arg (args, csGLStateCache**);
    *cache = statecache;
    return true;
  }
  if (!strcasecmp (command, "getextmanager"))
  {
    csGLExtensionManager** extmgr = va_arg (args, csGLExtensionManager**);
    *extmgr = &ext;
    return true;
  }
  return false;
}

bool csGraphics2DGLCommon::Resize (int width, int height)
{
  if (!is_open)
  {
    Width = width;
    Height = height;
    SetClipRect (0, 0, Width, Height);
    return true;
  }
  if (!AllowResizing)
    return false;
  Width = width;
  Height = height;
  SetClipRect (0, 0, Width, Height);
  EventOutlet->Broadcast (cscmdContextResize, (iGraphics2D *)this);
  return true;
}


