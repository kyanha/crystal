/*
    Copyright (C) 1998 by Jorrit Tyberghein
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
#include "video/canvas/common/graph2d.h"
#include "video/canvas/common/dyntex2d.h"
#include "qint.h"
#include "scrshot.h"
#include "isystem.h"
#include "igraph3d.h"
#include "itexture.h"

#if defined(PROC_INTEL) && !defined(NO_ASSEMBLER)
#  if defined(COMP_VC)
#    include "video/canvas/common/i386/drline.h"
#  endif
#endif

csGraphics2D::csGraphics2D ()
{
  Memory = NULL;
  FontServer = NULL;
}

bool csGraphics2D::Initialize (iSystem* pSystem)
{
  System = pSystem;
  // Get the system parameters
  System->GetSettings (Width, Height, Depth, FullScreen);

  Palette = new RGBPixel[256];

  // Get the font server
  const char *p = pSystem->ConfigGetStr (
    "FontServer", CS_FUNCID_FONT, "crystalspace.font.server.csfont");
  FontServer = LOAD_PLUGIN (pSystem, p, CS_FUNCID_FONT, iFontServer);
  Font = 0;
  if (!FontServer)
    return false;

  Palette = new RGBPixel [256];
  pfmt.PalEntries = 256;
  pfmt.PixelBytes = 1;
  // Initialize pointers to default drawing methods
  _DrawPixel = DrawPixel8;
  _WriteChar = WriteChar8;
  _GetPixelAt = GetPixelAt8;
  // Mark all slots in palette as free
  for (int i = 0; i < 256; i++)
  {
    PaletteAlloc [i] = false;
    Palette [i].red = 0;
    Palette [i].green = 0;
    Palette [i].blue = 0;
  }

  return true;
}

csGraphics2D::~csGraphics2D ()
{
  if (FontServer) FontServer->DecRef ();
  Close ();
  if (Palette)
    delete [] Palette;
}

bool csGraphics2D::Open (const char *Title)
{
  (void)Title;

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
  if (LineAddress)
  { delete [] LineAddress; LineAddress = NULL; }
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
          UShort *dest = (UShort *)GetPixelAt (fx1, fy1);
          while (count--) *dest++ = color;
          break;
        }
        case 4:
        {
          ULong *dest = (ULong *)GetPixelAt (fx1, fy1);
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
    int fy1 = QInt16 (y1), fy2 = QInt16 (y2);

    if (fx1 > fx2)
    {
      int tmp = fx1; fx1 = fx2; fx2 = tmp;
      tmp = fy1; fy1 = fy2; fy2 = tmp;
    }

    // delta Y can be negative
    int deltay = (fy2 - fy1) / (fx2 - fx1);
    for (int x = fx1, y = fy1; x <= fx2; x++)
    {
      DrawPixel (x, y >> 16, color);
      y += deltay;
    }
  }
  else
  {
    // Transform floating-point format to 16.16 fixed-point
    int fx1 = QInt16 (x1), fx2 = QInt16 (x2);

    if (fy1 > fy2)
    {
      int tmp = fy1; fy1 = fy2; fy2 = tmp;
      tmp = fx1; fx1 = fx2; fx2 = tmp;
    }

    // delta X can be negative
    int deltax = int (fx2 - fx1) / int (fy2 - fy1);
    for (int x = fx1, y = fy1; y <= fy2; y++)
    {
      DrawPixel (x >> 16, y, color);
      x += deltax;
    }
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
        register UShort *dest = (UShort *)GetPixelAt (x, y);
        register int count = w;
        while (count--) *dest++ = color;
        y++; h--;
      } /* endwhile */
      break;
    case 4:
      while (h)
      {
        register ULong *dest = (ULong *)GetPixelAt (x, y);
        register int count = w;
        while (count--) *dest++ = color;
        y++; h--;
      } /* endwhile */
      break;
  } /* endswitch */
}

void csGraphics2D::Write (int x, int y, int fg, int bg, const char *text)
{
  for (; *text; ++text)
  {
    WriteChar(x, y, fg, bg, *text);
    x += FontServer->GetCharWidth ( Font, (unsigned char)*text );
  }
}

#define WRITECHAR_NAME WriteChar8
#define WRITECHAR_PIXTYPE UByte
#include "writechr.inc"

#define WRITECHAR_NAME WriteChar16
#define WRITECHAR_PIXTYPE UShort
#include "writechr.inc"

#define WRITECHAR_NAME WriteChar32
#define WRITECHAR_PIXTYPE ULong
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

// This algorithm has been borrowed about eight years ago from a book on
// computer graphics, I believe its author was R.Wilson or something alike.
// It was first converted from ASM to Pascal, and now I converted it to C :-)
bool csGraphics2D::ClipLine (float &x1, float &y1, float &x2, float &y2,
  int xmin, int ymin, int xmax, int ymax)
{
  float fxmin = xmin;
  float fxmax = float (xmax) - 0.0001;
  float fymin = ymin;
  float fymax = float (ymax) - 0.0001;

#define CLIP_LEFT   0x01
#define CLIP_TOP    0x02
#define CLIP_RIGHT  0x04
#define CLIP_BOTTOM 0x08

#define SetOutCodes(u, x, y)                \
  if (x < fxmin) u = CLIP_LEFT; else u = 0; \
  if (y < fymin) u |= CLIP_TOP;             \
  if (x > fxmax) u |= CLIP_RIGHT;           \
  if (y > fymax) u |= CLIP_BOTTOM;

#define FSWAP(a,b) { float __tmp__ = a; a = b; b = __tmp__; }
#define CSWAP(a,b) { char __tmp__ = a; a = b; b = __tmp__; }

  char ocu1,ocu2;
  SetOutCodes (ocu1, x1, y1);
  SetOutCodes (ocu2, x2, y2);

  bool Inside,Outside;
  Inside  = (ocu1 | ocu2) == 0;
  Outside = (ocu1 & ocu2) != 0;

  while ((!Outside) && (!Inside))
  {
    if (ocu1 == 0)                      // swap endpoints if necessary
    {                                   // so that (x1,y1) needs to be clipped
      FSWAP (x1, x2);
      FSWAP (y1, y2);
      CSWAP (ocu1, ocu2);
    }
    if (ocu1 & CLIP_LEFT)               // clip left
    {
      y1 = y1 + ((y2 - y1) * (fxmin - x1)) / (x2 - x1);
      x1 = fxmin;
    }
    else if (ocu1 & CLIP_TOP)           // clip above
    {
      x1 = x1 + ((x2 - x1) * (fymin - y1)) / (y2 - y1);
      y1 = fymin;
    }
    else if (ocu1 & CLIP_RIGHT)         // clip right
    {
      y1 = y1 + ((y2 - y1) * (fxmax - x1)) / (x2 - x1);
      x1 = fxmax;
    }
    else if (ocu1 & CLIP_BOTTOM)        // clip below
    {
      x1 = x1 + ((x2 - x1) * (fymax - y1)) / (y2 - y1);
      y1 = fymax;
    }
    SetOutCodes (ocu1, x1, y1);         // update for (x1,y1)
    Inside  = (ocu1 | ocu2) == 0;
    Outside = (ocu1 & ocu2) != 0;
  }
  return Outside;
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
      delete[] Area->data;
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

bool csGraphics2D::SetMousePosition (int x, int y)
{
  (void)x; (void)y;
  return false;
}

bool csGraphics2D::SetMouseCursor (csMouseCursorID iShape)
{
  return (iShape == csmcArrow);
}

bool csGraphics2D::PerformExtension (const char* args)
{
  (void)args;
  return false;
}

int csGraphics2D::GetTextWidth (int Font, const char *text)
{
  int w=0, h=0;
  FontServer->GetTextDimensions (Font, text, w, h);
  return w;
}

int csGraphics2D::GetTextHeight (int Font)
{
  return FontServer->GetMaximumHeight (Font);
}

void csGraphics2D::GetPixel (int x, int y, UByte &oR, UByte &oG, UByte &oB)
{
  oR = oG = oB = 0;

  if (x < 0 || y < 0 || x >= Width || y >= Height)
    return;

  UByte *vram = GetPixelAt (x, y);
  if (!vram)
    return;

  if (pfmt.PalEntries)
  {
    UByte pix = *vram;
    oR = Palette [pix].red;
    oG = Palette [pix].green;
    oB = Palette [pix].blue;
  }
  else
  {
    ULong pix = 0;
    switch (pfmt.PixelBytes)
    {
      case 1: pix = *vram; break;
      case 2: pix = *(UShort *)vram; break;
      case 4: pix = *(ULong *)vram; break;
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

iGraphics2D *csGraphics2D::CreateOffScreenCanvas (int width, int height, 
  csPixelFormat *pfmt, void *buffer, RGBPixel *palette, int pal_size, int flags)
{
  csDynamicTextureSoft2D *tex = new csDynamicTextureSoft2D (System);
  return tex->CreateOffScreenCanvas (width, height, pfmt, buffer, 
				     palette, pal_size, flags);
}
