/*
    Copyright (C) 1998-2001 by Jorrit Tyberghein
    Written by Andrew Zabolotny <bit@eltech.ru>

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
#include "cssysdef.h"
#include "graph2d.h"
#include "protex2d.h"
#include "qint.h"
#include "scrshot.h"
#include "iutil/plugin.h"
#include "ivideo/graph3d.h"
#include "ivideo/texture.h"
#include "iengine/texture.h"
#include "iutil/event.h"
#include "iutil/eventq.h"
#include "iutil/objreg.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"

SCF_IMPLEMENT_IBASE(csGraphics2D)
  SCF_IMPLEMENTS_INTERFACE(iGraphics2D)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iComponent)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iConfig)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iNativeWindowManager)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iNativeWindow)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csGraphics2D::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csGraphics2D::CanvasConfig)
  SCF_IMPLEMENTS_INTERFACE (iConfig)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csGraphics2D::NativeWindow)
  SCF_IMPLEMENTS_INTERFACE (iNativeWindow)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csGraphics2D::NativeWindowManager)
  SCF_IMPLEMENTS_INTERFACE (iNativeWindowManager)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csGraphics2D::EventHandler)
  SCF_IMPLEMENTS_INTERFACE (iEventHandler)
SCF_IMPLEMENT_IBASE_END

csGraphics2D::csGraphics2D (iBase* parent)
{
  SCF_CONSTRUCT_IBASE (parent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiConfig);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiNativeWindow);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiNativeWindowManager);
  scfiEventHandler = NULL;
  Memory = NULL;
  FontServer = NULL;
  LineAddress = NULL;
  Palette = NULL;
  Width = 640;
  Height = 480;
  Depth = 16;
  FullScreen = false;
  is_open = false;
  win_title = csStrNew ("Crystal Space Application");
  object_reg = NULL;
  AllowResizing = false;
  plugin_mgr = NULL;
}

bool csGraphics2D::Initialize (iObjectRegistry* r)
{
  CS_ASSERT (r != NULL);
  object_reg = r;
  plugin_mgr = CS_QUERY_REGISTRY (object_reg, iPluginManager);
  // Get the system parameters
  config.AddConfig (object_reg, "/config/video.cfg");
  Width = config->GetInt ("Video.ScreenWidth", Width);
  Height = config->GetInt ("Video.ScreenHeight", Height);
  Depth = config->GetInt ("Video.ScreenDepth", Depth);
  FullScreen = config->GetBool ("Video.FullScreen", FullScreen);

  // Get the font server: A missing font server is NOT an error
  if (!FontServer)
  {
    FontServer = CS_QUERY_REGISTRY (object_reg, iFontServer);
  }
#ifdef CS_DEBUG
  if (!FontServer)
  {
    printf (
      "WARNING: Canvas driver couldn't find a font server plugin!\n"
      "This is normal if you don't want one (warning displays only in "
      "debug mode)\n");
    fflush (stdout);
  }
#endif

  Palette = new csRGBpixel [256];
  pfmt.PalEntries = 256;
  pfmt.PixelBytes = 1;
  // Initialize pointers to default drawing methods
  _DrawPixel = DrawPixel8;
  _WriteString = WriteString8;
  _WriteStringBaseline = WriteStringBaseline8;
  _GetPixelAt = GetPixelAt8;
  // Mark all slots in palette as free
  int i;
  for (i = 0; i < 256; i++)
  {
    PaletteAlloc [i] = false;
    Palette [i].red = 0;
    Palette [i].green = 0;
    Palette [i].blue = 0;
  }

  if (!scfiEventHandler)
    scfiEventHandler = new EventHandler (this);
  iEventQueue* q = CS_QUERY_REGISTRY(object_reg, iEventQueue);
  if (q != 0)
  {
    q->RegisterListener (scfiEventHandler, CSMASK_Broadcast);
    q->DecRef ();
  }

  return true;
}

void csGraphics2D::ChangeDepth (int d)
{
  if (Depth == d) return;
  Depth = d;
}

csGraphics2D::~csGraphics2D ()
{
  if (scfiEventHandler)
  {
    iEventQueue* q = CS_QUERY_REGISTRY(object_reg, iEventQueue);
    if (q != 0)
    {
      q->RemoveListener (scfiEventHandler);
      q->DecRef ();
    }
    scfiEventHandler->DecRef ();
  }
  if (plugin_mgr) plugin_mgr->DecRef ();
  if (FontServer) FontServer->DecRef ();
  Close ();
  delete [] Palette;
  delete [] win_title;
}

bool csGraphics2D::HandleEvent (iEvent& Event)
{
  if (Event.Type == csevBroadcast)
    switch (Event.Command.Code)
    {
      case cscmdSystemOpen:
      {
        Open ();
        return true;
      }
      case cscmdSystemClose:
      {
        Close ();
        return true;
      }
    }
  return false;
}

bool csGraphics2D::Open ()
{
  if (is_open) return true;
  is_open = true;

  FrameBufferLocked = 0;

  // Allocate buffer for address of each scan line to avoid multuplication
  LineAddress = new int [Height];
  if (LineAddress == NULL) return false;

  // Initialize scanline address array
  int i,addr,bpl = Width * pfmt.PixelBytes;
  for (i = 0, addr = 0; i < Height; i++, addr += bpl)
    LineAddress[i] = addr;

  SetClipRect (0, 0, Width, Height);
  return true;
}

void csGraphics2D::Close ()
{
  if (!is_open) return;
  is_open = false;
  delete [] LineAddress;
  LineAddress = NULL;
}

bool csGraphics2D::BeginDraw ()
{
  FrameBufferLocked++;
  return true;
}

void csGraphics2D::FinishDraw ()
{
  if (FrameBufferLocked)
    FrameBufferLocked--;
}

void csGraphics2D::Clear(int color)
{
  DrawBox (0, 0, Width, Height, color);
}

bool csGraphics2D::DoubleBuffer (bool Enable)
{
  return !Enable;
}

bool csGraphics2D::GetDoubleBufferState ()
{
  return false;
}

int csGraphics2D::GetPage ()
{
  return 0;
}

void csGraphics2D::ClearAll (int color)
{
  int CurPage = GetPage ();
  do
  {
    if (!BeginDraw ())
      break;
    Clear (color);
    FinishDraw ();
    Print ();
  } while (GetPage () != CurPage);
}

void csGraphics2D::DrawPixel8 (csGraphics2D *This, int x, int y, int color)
{
  if ((x >= This->ClipX1) && (x < This->ClipX2)
   && (y >= This->ClipY1) && (y < This->ClipY2))
    *(This->GetPixelAt (x, y)) = color;
}

void csGraphics2D::DrawPixel16 (csGraphics2D *This, int x, int y, int color)
{
  if ((x >= This->ClipX1) && (x < This->ClipX2)
   && (y >= This->ClipY1) && (y < This->ClipY2))
    *(short *)(This->GetPixelAt (x, y)) = color;
}

void csGraphics2D::DrawPixel32 (csGraphics2D *This, int x, int y, int color)
{
  if ((x >= This->ClipX1) && (x < This->ClipX2)
   && (y >= This->ClipY1) && (y < This->ClipY2))
    *(long *)(This->GetPixelAt (x, y)) = color;
}

#ifndef NO_DRAWLINE
void csGraphics2D::DrawLine (float x1, float y1, float x2, float y2, int color)
{
  if (ClipLine (x1, y1, x2, y2, ClipX1, ClipY1, ClipX2, ClipY2))
    return;

  int fx1 = QInt (x1), fx2 = QInt (x2),
      fy1 = QInt (y1), fy2 = QInt (y2);


  // Adjust the farthest margin
  //if (fx1 < fx2) { if (float (fx2) == x2) fx2--; }
  //else if (fx1 > fx2) { if (float (fx1) == x1) fx1--; }
  //if (fy1 < fy2) { if (float (fy2) == y2) fy2--; }
  //else if (fy1 > fy2) { if (float (fy1) == y1) fy1--; }


  if (fy1 == fy2)
  {
    if (fx2 - fx1)
    {
      if (fx1 > fx2) { int tmp = fx1; fx1 = fx2; fx2 = tmp; }
      int count = fx2 - fx1 + 1;
      switch (pfmt.PixelBytes)
      {
        case 1:
          memset (GetPixelAt (fx1, fy1), color, count);
          break;
        case 2:
        {
          uint16 *dest = (uint16 *)GetPixelAt (fx1, fy1);
          while (count--) *dest++ = color;
          break;
        }
        case 4:
        {
          uint32 *dest = (uint32 *)GetPixelAt (fx1, fy1);
          while (count--) *dest++ = color;
          break;
        }
      } /* endswitch */
    }
    else
      DrawPixel (fx1, fy1, color);
  }
  else if (abs (fx2 - fx1) > abs (fy2 - fy1))
  {
    // Transform floating-point format to 16.16 fixed-point
    fy1 = QInt16 (y1); fy2 = QInt16 (y2);

    if (fx1 > fx2)
    {
      int tmp = fx1; fx1 = fx2; fx2 = tmp;
      tmp = fy1; fy1 = fy2; fy2 = tmp;
    }

    // delta Y can be negative
    int deltay = (fy2 - fy1) / (fx2 - fx1 + 1);

#define H_LINE(pixtype)                     \
  {                             \
    int x, y;  \
    for (x = fx1, y = fy1 + deltay / 2; x <= fx2; x++)  \
    {                               \
      pixtype *p = (pixtype *)(Memory +             \
        (x * sizeof (pixtype) + LineAddress [y >> 16]));    \
      *p = color; y += deltay;                  \
    }                               \
  }

/*#define H_LINE(pixtype)                       \
  {                             \
    int x, y;  \
    for (x = fx1, y = fy1 + deltay / 2; x <= fx2; x++)  \
    {                               \
      DrawPixel(x, (y>>16), color); y += deltay;                    \
    }                               \ */

    switch (pfmt.PixelBytes)
    {
      case 1: H_LINE (uint8); break;
      case 2: H_LINE (uint16); break;
      case 4: H_LINE (uint32); break;
    } /* endswitch */

#undef H_LINE
  }
  else
  {
    // Transform floating-point format to 16.16 fixed-point
    fx1 = QInt16 (x1); fx2 = QInt16 (x2);

    if (fy1 > fy2)
    {
      int tmp = fy1; fy1 = fy2; fy2 = tmp;
      tmp = fx1; fx1 = fx2; fx2 = tmp;
    }

    // delta X can be negative
    int deltax = (fx2 - fx1) / (fy2 - fy1 + 1);

#define V_LINE(pixtype)                     \
  {                             \
    int x, y; \
    for (x = fx1 + deltax / 2, y = fy1; y <= fy2; y++)  \
    {                               \
      pixtype *p = (pixtype *)(Memory +             \
        ((x >> 16) * sizeof (pixtype) + LineAddress [y]));  \
      *p = color; x += deltax;                  \
    }                               \
  }

/*#define V_LINE(pixtype)                       \
  {                             \
    int x, y; \
    for (x = fx1 + deltax / 2, y = fy1; y <= fy2; y++)  \
    {                               \
      DrawPixel((x>>16), y, color); \
      x += deltax; \
    }                               \
  }*/

    switch (pfmt.PixelBytes)
    {
      case 1: V_LINE (uint8); break;
      case 2: V_LINE (uint16); break;
      case 4: V_LINE (uint32); break;
    } /* endswitch */

#undef V_LINE
  }
}
#endif

void csGraphics2D::DrawBox (int x, int y, int w, int h, int color)
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
  switch (pfmt.PixelBytes)
  {
    case 1:
      while (h)
      {
        memset (GetPixelAt (x, y), color, w);
        y++; h--;
      } /* endwhile */
      break;
    case 2:
      while (h)
      {
        register uint16 *dest = (uint16 *)GetPixelAt (x, y);
        register int count = w;
        while (count--) *dest++ = color;
        y++; h--;
      } /* endwhile */
      break;
    case 4:
      while (h)
      {
        register uint32 *dest = (uint32 *)GetPixelAt (x, y);
        register int count = w;
        while (count--) *dest++ = color;
        y++; h--;
      } /* endwhile */
      break;
  } /* endswitch */
}

#define WR_NAME WriteString8
#define WR_NAME2 WriteStringBaseline8
#define WR_PIXTYPE uint8
#include "writechr.inc"

#define WR_NAME WriteString16
#define WR_NAME2 WriteStringBaseline16
#define WR_PIXTYPE uint16
#include "writechr.inc"

#define WR_NAME WriteString32
#define WR_NAME2 WriteStringBaseline32
#define WR_PIXTYPE uint32
#include "writechr.inc"

void csGraphics2D::SetClipRect (int xmin, int ymin, int xmax, int ymax)
{
  ClipX1 = xmin; ClipX2 = xmax;
  ClipY1 = ymin; ClipY2 = ymax;
}

void csGraphics2D::GetClipRect (int &xmin, int &ymin, int &xmax, int &ymax)
{
  xmin = ClipX1; xmax = ClipX2;
  ymin = ClipY1; ymax = ClipY2;
}

/* helper function for ClipLine below */
bool csGraphics2D::CLIPt(float denom, float num, float& tE, float& tL)
{
    float t;

    if(denom > 0) {
        t = num / denom;
        if(t > tL) return false;
        else if(t > tE) tE = t;
    } else if(denom < 0) {
        t = num / denom;
        if(t < tE) return false;
        else tL = t;
    } else if(num > 0) return false;
    return true;
}

/* This function and the next one were taken
   from _Computer Graphics: Principals and Practice_ (2nd ed)
   by Foley et al
   This implements the Liang-Barsky efficient parametric
   line-clipping algorithm
*/
bool csGraphics2D::ClipLine (float &x0, float &y0, float &x1, float &y1,
                             int xmin, int ymin, int xmax, int ymax)
{
    float dx = x1 - x0;
    float dy = y1 - y0;
    bool visible = false;

    if(dx == 0 && dy == 0 && x0 >= xmin && y0 >= ymin && x0 < xmax && y0 < ymax) {
        visible = true;
    } else {
        float tE = 0.0;
        float tL = 1.0;
        if(CLIPt(dx, xmin - x0, tE, tL))
            if(CLIPt(-dx, x0 - xmax, tE, tL))
                if(CLIPt(dy, ymin - y0, tE, tL))
                    if(CLIPt(-dy, y0 - ymax, tE, tL))
                    {
                        visible = true;
                        if(tL < 1.0) {
                            x1 = x0 + tL * dx;
                            y1 = y0 + tL * dy;
                        }
                        if(tE > 0) {
                            x0 += tE * dx;
                            y0 += tE * dy;
                        }
                    }
    }
    return !visible;
}


csImageArea *csGraphics2D::SaveArea (int x, int y, int w, int h)
{
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
  w *= pfmt.PixelBytes;
  char *dest = Area->data = new char [w * h];
  if (!dest)
  {
    delete Area;
    return NULL;
  }
  for (; h > 0; y++, h--)
  {
    unsigned char *VRAM = GetPixelAt (x, y);
    memcpy (dest, VRAM, w);
    dest += w;
  } /* endfor */
  return Area;
}

void csGraphics2D::RestoreArea (csImageArea *Area, bool Free)
{
  if (Area)
  {
    char *dest = Area->data;
    int x = Area->x, y = Area->y, w = Area->w, h = Area->h;
    w *= pfmt.PixelBytes;
    for (; h; y++, h--)
    {
      unsigned char *VRAM = GetPixelAt (x, y);
      memcpy (VRAM, dest, w);
      dest += w;
    } /* endfor */
    if (Free)
      FreeArea (Area);
  } /* endif */
}

void csGraphics2D::FreeArea (csImageArea *Area)
{
  if (Area)
  {
    if (Area->data)
      delete [] Area->data;
    delete Area;
  } /* endif */
}

void csGraphics2D::SetRGB (int i, int r, int g, int b)
{
  Palette[i].red = r;
  Palette[i].green = g;
  Palette[i].blue = b;
  PaletteAlloc[i] = true;
}

unsigned char *csGraphics2D::GetPixelAt8 (csGraphics2D *This, int x, int y)
{
  return (This->Memory + (x + This->LineAddress[y]));
}

unsigned char *csGraphics2D::GetPixelAt16 (csGraphics2D *This, int x, int y)
{
  return (This->Memory + (x + x + This->LineAddress[y]));
}

unsigned char *csGraphics2D::GetPixelAt32 (csGraphics2D *This, int x, int y)
{
  return (This->Memory + ((x<<2) + This->LineAddress[y]));
}

bool csGraphics2D::PerformExtensionV (char const*, va_list)
{
  return false;
}

bool csGraphics2D::PerformExtension (char const* command, ...)
{
  va_list args;
  va_start (args, command);
  bool rc = PerformExtensionV(command, args);
  va_end (args);
  return rc;
}

void csGraphics2D::GetPixel (int x, int y, uint8 &oR, uint8 &oG, uint8 &oB)
{
  oR = oG = oB = 0;

  if (x < 0 || y < 0 || x >= Width || y >= Height)
    return;

  uint8 *vram = GetPixelAt (x, y);
  if (!vram)
    return;

  if (pfmt.PalEntries)
  {
    uint8 pix = *vram;
    oR = Palette [pix].red;
    oG = Palette [pix].green;
    oB = Palette [pix].blue;
  }
  else
  {
    uint32 pix = 0;
    switch (pfmt.PixelBytes)
    {
      case 1: pix = *vram; break;
      case 2: pix = *(uint16 *)vram; break;
      case 4: pix = *(uint32 *)vram; break;
    }
    oR = ((pix & pfmt.RedMask)   >> pfmt.RedShift)   << (8 - pfmt.RedBits);
    oG = ((pix & pfmt.GreenMask) >> pfmt.GreenShift) << (8 - pfmt.GreenBits);
    oB = ((pix & pfmt.BlueMask)  >> pfmt.BlueShift)  << (8 - pfmt.BlueBits);
  }
}

iImage *csGraphics2D::ScreenShot ()
{
  BeginDraw ();
  csScreenShot *ss = new csScreenShot (this);
  FinishDraw ();
  return ss;
}

iGraphics2D *csGraphics2D::CreateOffScreenCanvas
  (int width, int height, void *buffer, bool alone_hint,
   csPixelFormat *pfmt, csRGBpixel *palette, int pal_size)
{
  // default return a software canvas
  CS_ASSERT (object_reg != NULL);
  csProcTextureSoft2D *tex = new csProcTextureSoft2D (object_reg);
  return tex->CreateOffScreenCanvas (width, height, buffer, alone_hint,
                     pfmt, palette, pal_size);
}

void csGraphics2D::AlertV (int type, const char* title, const char* okMsg,
    const char* msg, va_list arg)
{
  (void)type; (void)title; (void)okMsg;
  printf ("ALERT: ");
  vprintf (msg, arg);
  fflush (stdout);
}

void csGraphics2D::NativeWindowManager::Alert (int type,
    const char* title, const char* okMsg, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  scfParent->AlertV (type, title, okMsg, msg, arg);
  va_end (arg);
}

iNativeWindow* csGraphics2D::GetNativeWindow ()
{
  return &scfiNativeWindow;
}

void csGraphics2D::SetTitle (const char* title)
{
  delete[] win_title;
  win_title = csStrNew (title);
}

bool csGraphics2D::Resize (int w, int h)
{
  if (!LineAddress)
  {
    // Still in Initialization phase, configuring size of canvas
    Width = w;
    Height = h;
    SetClipRect (0, 0, Width, Height);
    return true;
  }

  if (!AllowResizing)
    return false;

  if (Width != w || Height != h)
  {
    Width = w;
    Height = h;

    delete [] LineAddress;
    LineAddress = NULL;

    // Allocate buffer for address of each scan line to avoid multuplication
    LineAddress = new int [Height];
    CS_ASSERT (LineAddress != NULL);

    // Initialize scanline address array
    int i,addr,bpl = Width * pfmt.PixelBytes;
    for (i = 0, addr = 0; i < Height; i++, addr += bpl)
      LineAddress[i] = addr;

    SetClipRect (0, 0, Width, Height);
  }
  return true;
}

void csGraphics2D::SetFullScreen (bool b)
{
  if (FullScreen == b) return;
  FullScreen = b;
}

bool csGraphics2D::SetMousePosition (int x, int y)
{
  (void)x; (void)y;
  return false;
}

bool csGraphics2D::SetMouseCursor (csMouseCursorID iShape)
{
  return (iShape == csmcArrow);
}

//---------------------------------------------------------------------------

#define NUM_OPTIONS 3

static const csOptionDescription config_options [NUM_OPTIONS] =
{
  { 0, "depth", "Display depth", CSVAR_LONG },
  { 1, "fs", "Fullscreen if available", CSVAR_BOOL },
  { 2, "mode", "Window size or resolution", CSVAR_STRING },
};

bool csGraphics2D::CanvasConfig::SetOption (int id, csVariant* value)
{
  if (value->GetType () != config_options[id].type)
    return false;
  switch (id)
  {
    case 0: scfParent->ChangeDepth (value->GetLong ()); break;
    case 1: scfParent->SetFullScreen (value->GetBool ()); break;
    case 2:
    {
      const char* buf = value->GetString ();
      int wres, hres;
      if (sscanf (buf, "%dx%d", &wres, &hres) == 2)
        scfParent->Resize (wres, hres);
      break;
    }
    default: return false;
  }
  return true;
}

bool csGraphics2D::CanvasConfig::GetOption (int id, csVariant* value)
{
  switch (id)
  {
    case 0: value->SetLong (scfParent->Depth); break;
    case 1: value->SetBool (scfParent->FullScreen); break;
    case 2:
    {
      char buf[100];
      sprintf (buf, "%dx%d", scfParent->GetWidth (), scfParent->GetHeight ());
      value->SetString (buf);
      break;
    }
    default: return false;
  }
  return true;
}

bool csGraphics2D::CanvasConfig::GetOptionDescription
  (int idx, csOptionDescription* option)
{
  if (idx < 0 || idx >= NUM_OPTIONS)
    return false;
  *option = config_options[idx];
  return true;
}

