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

#include <stdarg.h>
#include "cssysdef.h"
#include "csutil/scf.h"
#include "cssys/unix/iunix.h"
#include "cssys/csevent.h"
#include "video/canvas/softx/x2d.h"
#include "video/canvas/common/dyntex2d.h"
#include "csutil/csrect.h"
#include "isystem.h"

IMPLEMENT_FACTORY (csGraphics2DXLib)

EXPORT_CLASS_TABLE (x2d)
  EXPORT_CLASS (csGraphics2DXLib, "crystalspace.graphics2d.xlib",
    "X-Windows 2D graphics driver for Crystal Space")
EXPORT_CLASS_TABLE_END

IMPLEMENT_IBASE (csGraphics2DXLib)
  IMPLEMENTS_INTERFACE (iPlugIn)
  IMPLEMENTS_INTERFACE (iGraphics2D)
IMPLEMENT_IBASE_END

// csGraphics2DXLib functions
csGraphics2DXLib::csGraphics2DXLib (iBase *iParent) :
  csGraphics2D (), xim (NULL), cmap (0),
  sim_lt8 (NULL), sim_lt16 (NULL)
{
  CONSTRUCT_IBASE (iParent);
}

bool csGraphics2DXLib::Initialize (iSystem *pSystem)
{
  if (!csGraphics2D::Initialize (pSystem))
    return false;

  UnixSystem = QUERY_INTERFACE (System, iUnixSystemDriver);
  if (!UnixSystem)
  {
    CsPrintf (MSG_FATAL_ERROR, "FATAL: The system driver does not support "
                               "the iUnixSystemDriver interface\n");
    return false;
  }

  Screen* screen_ptr;

  // Open display
  dpy = XOpenDisplay (NULL);

  if (!dpy)
  {
    CsPrintf (MSG_FATAL_ERROR, "FATAL: Cannot open X display\n");
    exit (-1);
  }

  // Query system settings
  UnixSystem->GetExtSettings (sim_depth, do_shm, do_hwmouse);

  screen_num = DefaultScreen (dpy);
  screen_ptr = DefaultScreenOfDisplay (dpy);
  display_width = DisplayWidth (dpy, screen_num);
  display_height = DisplayHeight (dpy, screen_num);

  // Determine visual information.
  // Try in order:
  //   screen depth
  //   24 bit TrueColor
  //   16 bit TrueColor
  //   15 bit TrueColor
  //   8 bit PseudoColor
  int d = DefaultDepthOfScreen(DefaultScreenOfDisplay(dpy));
  if (XMatchVisualInfo(dpy, screen_num, d, (d == 8 ? PseudoColor : TrueColor), &vinfo)
   || XMatchVisualInfo(dpy, screen_num, 24, TrueColor, &vinfo)
   || XMatchVisualInfo(dpy, screen_num, 16, TrueColor, &vinfo)
   || XMatchVisualInfo(dpy, screen_num, 15, TrueColor, &vinfo)
   || XMatchVisualInfo(dpy, screen_num, 8, PseudoColor, &vinfo))
  {
    visual = vinfo.visual;
    // Visual is supposed to be opaque, sorry for breaking in
    vclass = visual->c_class;
  }
  else
  {
    CsPrintf (MSG_FATAL_ERROR, "FATAL: Current screen depth not supported (8, 15, 16 or 32 bpp only)\n");
    exit (1);
  }

  pfmt.RedMask = vinfo.red_mask;
  pfmt.GreenMask = vinfo.green_mask;
  pfmt.BlueMask = vinfo.blue_mask;

  pfmt.complete ();
  pfmt.PalEntries = vinfo.colormap_size;
  if (vclass == TrueColor)
    pfmt.PalEntries = 0;
  if (vinfo.depth == 24 || vinfo.depth == 32)
    pfmt.PixelBytes = 4;
  else if (pfmt.PalEntries)
    pfmt.PixelBytes = 1;		// Palette mode
  else
    pfmt.PixelBytes = 2;		// Truecolor mode

  // First check if we can disable simulation because the format is the same.
  if ((sim_depth == 8 && pfmt.PixelBytes == 1)
   || (sim_depth == 15 && pfmt.PixelBytes == 2 && pfmt.RedMask == (0x1f<<10))
   || (sim_depth == 16 && pfmt.PixelBytes == 2 && pfmt.RedMask == (0x1f<<11))
   || (sim_depth == 32 && pfmt.PixelBytes == 4))
    sim_depth = 0;

  // If we are going to simulate another depth then the real information
  // is put aside and we will fill pfmt with faked values.
  if (sim_depth != 0)
  {
    real_pfmt = pfmt;
    switch (sim_depth)
    {
      case 8:
        pfmt.RedMask = pfmt.GreenMask = pfmt.BlueMask = 0;
	pfmt.PalEntries = 256;
	pfmt.PixelBytes = 1;
	break;
      case 15:
      	pfmt.RedMask   = 0x1f << 10;
      	pfmt.GreenMask = 0x1f << 5;
      	pfmt.BlueMask  = 0x1f;
	pfmt.PalEntries = 0;
	pfmt.PixelBytes = 2;
	break;
      case 16:
      	pfmt.RedMask   = 0x1f << 11;
      	pfmt.GreenMask = 0x3f << 5;
      	pfmt.BlueMask  = 0x1f;
	pfmt.PalEntries = 0;
	pfmt.PixelBytes = 2;
	break;
      case 32:
      	pfmt.RedMask = 0xff << 16;
	pfmt.GreenMask = 0xff << 8;
	pfmt.BlueMask = 0xff;
	pfmt.PalEntries = 0;
	pfmt.PixelBytes = 4;
	break;
    }
    pfmt.complete ();
  }

  // Allocate the palette if not truecolor or if simulating truecolor on 8-bit display.
  if ((sim_depth == 0 && pfmt.PalEntries) || (sim_depth != 0 && real_pfmt.PalEntries))
    cmap = XCreateColormap (dpy, RootWindow (dpy, screen_num), visual, AllocAll);
  else
    cmap = 0;

  // If we are simulating truecolor on an 8-bit display then we will create
  // a truecolor 3:3:2 colormap. This is ugly but simulated depth is for testing
  // only. So who cares?
  if ((sim_depth == 15 || sim_depth == 16 || sim_depth == 32) && cmap)
  {
    int i;
    for (i = 0 ; i < 256 ; i++)
    {
      XColor color;
      color.pixel = i;
      color.red   = (((i&0xe0)>>5)<<5)*256;
      color.green = (((i&0x1c)>>2)<<5)*256;
      color.blue  = ((i&0x3)<<6)*256;
      color.flags = DoRed | DoGreen | DoBlue;
      XStoreColor (dpy, cmap, &color);
    }
  }

  // Create the simulated lookup tables if any.
  if (sim_depth == 15 && real_pfmt.PixelBytes == 1)
  {
    // Simulate 15-bit on 8-bit.
    sim_lt8 = new unsigned char [65536];
    int i, r, g, b;
    for (i = 0 ; i < 32768 ; i++)
    {
      r = (i&0x7c00)>>10; r >>= 2;
      g = (i&0x03e0)>>5;  g >>= 2;
      b = (i&0x001f);     b >>= 3;
      sim_lt8[i] = (r<<5) | (g<<2) | b;
    }
  }
  else if ((sim_depth == 16 || sim_depth == 32) && real_pfmt.PixelBytes == 1)
  {
    // Simulate 16-bit on 8-bit.
    sim_lt8 = new unsigned char [65536];
    int i, r, g, b;
    for (i = 0 ; i < 65536 ; i++)
    {
      r = (i&0xf800)>>11; r >>= 2;
      g = (i&0x07e0)>>5;  g >>= 3;
      b = (i&0x001f);     b >>= 3;
      sim_lt8[i] = (r<<5) | (g<<2) | b;
    }
  }
  else if (sim_depth == 32 && real_pfmt.PixelBytes == 2)
  {
    // Simulate 32-bit on 16-bit.
    // No lookup tables needed.
  }

  xim = NULL;
  Memory = NULL;

  // If in 16-bit mode, redirect drawing routines
  if (pfmt.PixelBytes == 2)
  {
    _DrawPixel = DrawPixel16;
    _WriteChar = WriteChar16;
    _GetPixelAt = GetPixelAt16;
  }
  else if (pfmt.PixelBytes == 4)
  {
    _DrawPixel = DrawPixel32;
    _WriteChar = WriteChar32;
    _GetPixelAt = GetPixelAt32;
  } /* endif */

  memset (MouseCursor, 0, sizeof (MouseCursor));

  // Tell system driver to call us on every frame
  System->CallOnEvents (this, CSMASK_Nothing);

  return true;
}

csGraphics2DXLib::~csGraphics2DXLib(void)
{
  Close();
  if (UnixSystem)
    UnixSystem->DecRef ();
  delete [] sim_lt8;
  delete [] sim_lt16;
}

bool csGraphics2DXLib::Open(const char *Title)
{
  CsPrintf (MSG_INITIALIZATION, "Crystal Space X windows driver");
  if (do_shm)
    CsPrintf (MSG_INITIALIZATION, " (Using SHM extension).\n");
  else
    CsPrintf (MSG_INITIALIZATION, ".\n");
  CsPrintf (MSG_INITIALIZATION, "Using %d bit %sColor visual\n",
              vinfo.depth, (vclass == PseudoColor) ? "Pseudo" : "True");

  // Open your graphic interface
  if (!csGraphics2D::Open (Title))
    return false;

  // Create window
  XSetWindowAttributes swa;
  swa.background_pixel = 0;
  swa.border_pixel = 0;
  swa.colormap = cmap;
  swa.bit_gravity = CenterGravity;
  window = XCreateWindow (dpy, DefaultRootWindow(dpy), 64, 16,
			  Width, Height, 4, vinfo.depth, InputOutput, visual,
			  CWBackPixel | CWBorderPixel | CWBitGravity | 
			  (cmap ? CWColormap : 0), &swa);
  XMapWindow (dpy, window);
  XStoreName (dpy, window, Title);

  XGCValues values;
  gc = XCreateGC (dpy, window, 0, &values);
  XSetForeground (dpy, gc, BlackPixel (dpy, screen_num));
  XSetLineAttributes (dpy, gc, 0, LineSolid, CapButt, JoinMiter);
  XSetGraphicsExposures (dpy, gc, False);

  XSetWindowAttributes attr;
  attr.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
    FocusChangeMask | PointerMotionMask | ButtonPressMask | 
    ButtonReleaseMask | StructureNotifyMask;
  XChangeWindowAttributes (dpy, window, CWEventMask, &attr);

  if (cmap)
    XSetWindowColormap (dpy, window, cmap);

  // Intern WM_DELETE_WINDOW and set window manager protocol
  // (Needed to catch user using window manager "delete window" button)
  wm_delete_window = XInternAtom (dpy, "WM_DELETE_WINDOW", False);
  XSetWMProtocols (dpy, window, &wm_delete_window, 1);

  // Create mouse cursors
  XColor Black;
  memset (&Black, 0, sizeof (Black));
  EmptyPixmap = XCreatePixmap (dpy, window, 1, 1, 1);
  EmptyMouseCursor = XCreatePixmapCursor (dpy, EmptyPixmap, EmptyPixmap,
    &Black, &Black, 0, 0);
  MouseCursor [csmcArrow] = XCreateFontCursor (dpy, XC_left_ptr);
//MouseCursor [csmcLens] = XCreateFontCursor (dpy, 
  MouseCursor [csmcCross] = XCreateFontCursor (dpy, 33/*XC_crosshair*/);
  MouseCursor [csmcPen] = XCreateFontCursor (dpy, XC_hand2/*XC_pencil*/);
  MouseCursor [csmcMove] = XCreateFontCursor (dpy, XC_fleur);
  /// Diagonal (\) resizing cursor
//MouseCursor [csmcSizeNWSE] = XCreateFontCursor (dpy, 
  /// Diagonal (/) resizing cursor
//MouseCursor [csmcSizeNESW] = XCreateFontCursor (dpy, 
  /// Vertical sizing cursor
  MouseCursor [csmcSizeNS] = XCreateFontCursor (dpy, XC_sb_v_double_arrow);
  /// Horizontal sizing cursor
  MouseCursor [csmcSizeEW] = XCreateFontCursor (dpy, XC_sb_h_double_arrow);
  /// Invalid operation cursor
//MouseCursor [csmcStop] = XCreateFontCursor (dpy, XC_pirate);
  /// Wait (longplay operation) cursor
  MouseCursor [csmcWait] = XCreateFontCursor (dpy, XC_watch);

  // Wait for expose event
  XEvent event;
  for (;;)
  {
    XNextEvent (dpy, &event);
    if (event.type == Expose)
      break;
  }
  if (!AllocateMemory ())
  {
#ifdef DO_SHM
    if(do_shm)
    {
      CsPrintf (MSG_INITIALIZATION, "SHM available but could not allocate. "
        "Trying without SHM.\n");
      if(xim) { XDestroyImage(xim); xim=NULL; }
      do_shm=false;
      if(!AllocateMemory())
      {
#endif //DO_SHM
      CsPrintf (MSG_FATAL_ERROR, "Unable to allocate memory!\n");
      return false;
#ifdef DO_SHM
      }
    }
#endif //DO_SHM
  }
  Clear (0);
  return true;
}

bool csGraphics2DXLib::AllocateMemory ()
{
  // Create backing store
  if (!xim)
  {
//    xim = XGetImage (dpy, window, 0, 0, Width, Height, AllPlanes, ZPixmap);
    int sc = DefaultScreen(dpy);
    int disp_depth = DefaultDepth(dpy,sc);
    int bitmap_pad = (disp_depth + 7) / 8;
        bitmap_pad = (bitmap_pad==3) ? 32 : bitmap_pad*8;

#   ifdef DO_SHM
    if (do_shm && !XShmQueryExtension (dpy))
    {
      do_shm = false;
      CsPrintf (MSG_INITIALIZATION, "SHM extension not available on display!\n");
    }
    if (do_shm)
    {
      xim = XShmCreateImage(dpy, DefaultVisual(dpy,sc), disp_depth, 
			    ZPixmap, 0, &shmi, Width, Height);
      if (!xim)
      {
	CsPrintf (MSG_FATAL_ERROR, "XShmCreateImage failed!\n");
	return false;
      }
      shm_image = *xim;
      shmi.shmid = shmget (IPC_PRIVATE, xim->bytes_per_line*xim->height,
        IPC_CREAT | 0777);
      if (shmi.shmid == -1)
      {
	CsPrintf (MSG_FATAL_ERROR, "shmget failed!\n");
	return false;
      }
      shmi.shmaddr = (char*)shmat (shmi.shmid, 0, 0);
      if (shmi.shmaddr == (char*) -1)
      {
	CsPrintf (MSG_FATAL_ERROR, "shmat failed!\n");
	return false;
      }
      shmi.readOnly = FALSE;
      XShmAttach (dpy, &shmi);

      // Delete memory segment. The memory stays available until
      // the last client detaches from it.
      XSync (dpy, False);
      shmctl (shmi.shmid, IPC_RMID, 0);

      shm_image.data = shmi.shmaddr;
      real_Memory = (unsigned char *)shmi.shmaddr;
      shm_image.obdata = (char *)&shmi;
    }
    else
#   endif /* DO_SHM */
    {
      xim = XCreateImage(dpy, DefaultVisual(dpy,sc), disp_depth, ZPixmap, 0,
			 NULL, Width, Height, bitmap_pad, 0);
      xim->data = new char[xim->bytes_per_line*xim->height];
      real_Memory = (unsigned char*)(xim->data);
    }

    // If not simulating depth then Memory is equal to real_Memory.
    // If simulating then we allocate a new Memory array in the faked format.
    if (!sim_depth)
      Memory = real_Memory;
    else
      Memory = new unsigned char [Width*Height*pfmt.PixelBytes];
  }

  return true;
}

void csGraphics2DXLib::Close ()
{
  if (EmptyMouseCursor)
  {
    XFreeCursor (dpy, EmptyMouseCursor);
    EmptyMouseCursor = 0;
    XFreePixmap (dpy, EmptyPixmap);
    EmptyPixmap = 0;
  }
  for (int i = sizeof (MouseCursor) / sizeof (Cursor) - 1; i >= 0; i--)
  {
    if (MouseCursor [i])
      XFreeCursor (dpy, MouseCursor [i]);
    MouseCursor [i] = None;
  }
  if (window)
  {
    XDestroyWindow (dpy, window);
    window = 0;
  }
  if (Memory && sim_depth)
  {
    delete [] Memory;
    Memory = NULL;
  }

  csGraphics2D::Close ();
}

struct palent
{
  UShort idx;
  unsigned char r, g, b;
  int cnt;
};

int cmp_palent (const void* p1, const void* p2)
{
  const palent* pe1 = (palent*)p1;
  const palent* pe2 = (palent*)p2;
  if (pe1->cnt < pe2->cnt)
    return 1;
  else if (pe1->cnt > pe2->cnt)
    return -1;
  else
    return 0;
}

int find_rgb_palent (palent* pe, int r, int g, int b)
{
  int i, min, mindist;
  mindist = 1000*256*256;
  min = -1;
  register int red, green, blue, dist;
  for (i = 0 ; i < 256 ; i++)
    if (pe [i].cnt)
    {
      red = r - pe [i].r;
      green = g - pe [i].g;
      blue = b - pe [i].b;
      dist = (299 * red * red) + (587 * green * green) + (114 * blue * blue);
      if (dist == 0)
        return i;
      if (dist < mindist)
      {
        mindist = dist;
	min = i;
      }
    }
    else
      return min;
  return min;
}

void csGraphics2DXLib::restore_332_palette ()
{
  if ((sim_depth != 15 && sim_depth != 16 && sim_depth != 32)
   || (real_pfmt.PixelBytes != 1))
    return;

  CsPrintf (MSG_DEBUG_0, "Restore 3:3:2 palette\n");
  //System->Console->Print ();
  //Print ();
  //XFlush (dpy);

  int i, r, g, b;

  if (sim_depth == 15)
    for (i = 0 ; i < 32768 ; i++)
    {
      r = (i&0x7c00)>>10; r >>= 2;
      g = (i&0x03e0)>>5;  g >>= 2;
      b = (i&0x001f);     b >>= 3;
      sim_lt8[i] = (r<<5) | (g<<2) | b;
    }
  else
    for (i = 0 ; i < 65536 ; i++)
    {
      r = (i&0xf800)>>11; r >>= 2;
      g = (i&0x07e0)>>5;  g >>= 3;
      b = (i&0x001f);     b >>= 3;
      sim_lt8[i] = (r<<5) | (g<<2) | b;
    }

  for (i = 0 ; i < 256 ; i++)
  {
    XColor color;
    color.pixel = i;
    color.red   = (((i&0xe0)>>5)<<5)*256;
    color.green = (((i&0x1c)>>2)<<5)*256;
    color.blue  = ((i&0x3)<<6)*256;
    color.flags = DoRed | DoGreen | DoBlue;
    XStoreColor (dpy, cmap, &color);
  }
  CsPrintf (MSG_DEBUG_0, "Done!\n");
}

void csGraphics2DXLib::recompute_simulated_palette ()
{
  if ((sim_depth != 15 && sim_depth != 16 && sim_depth != 32)
   || (real_pfmt.PixelBytes != 1))
    return;

  CsPrintf (MSG_DEBUG_0, "Recompute simulated palette\n");
  //System->Console->Print ();
  //Print ();
  //XFlush (dpy);

  palent* pe;
  int i;

  // Create a palette of 16K entries
  pe = new palent [65536];
  if (sim_depth == 32)
    for (i = 0 ; i < 65536 ; i++)
    {
      pe[i].idx = i;
      pe[i].r = ((i&0xf800) >> 11) << 3;
      pe[i].g = ((i&0x07e0) >> 5) << 2;
      pe[i].b = ((i&0x001f) >> 0) << 3;
      pe[i].cnt = 0;
    }
  else
    for (i = 0 ; i < 65536 ; i++)
    {
      pe[i].idx = i;
      pe[i].r = ((i&pfmt.RedMask) >> pfmt.RedShift) << (8-pfmt.RedBits);
      pe[i].g = ((i&pfmt.GreenMask) >> pfmt.GreenShift) << (8-pfmt.GreenBits);
      pe[i].b = ((i&pfmt.BlueMask) >> pfmt.BlueShift) << (8-pfmt.BlueBits);
      pe[i].cnt = 0;
    }

  // Count how many times each color appears.
  if (sim_depth == 15 || sim_depth == 16)
  {
    UShort* m = (UShort*)Memory;
    i = Width*Height;
    while (i > 0)
    {
      pe[*m++].cnt++;
      i--;
    }
  }
  else if (sim_depth == 32)
  {
    ULong* m = (ULong*)Memory;
    i = Width*Height;
    while (i > 0)
    {
      ULong col = *m++;
      UShort scol =
      	(((col & pfmt.RedMask) >> (pfmt.RedShift+3)) << 11) |
      	(((col & pfmt.GreenMask) >> (pfmt.GreenShift+2)) << 5) |
      	(((col & pfmt.BlueMask) >> (pfmt.BlueShift+3)));
      pe[scol].cnt++;
      i--;
    }
  }

  // Fake that color 0 (black) is used very much so that
  // it will appear in front of all colors after sorting.
  pe[0].cnt = Width*Height+1;

  // Sort based on count.
  qsort ((void*)pe, 65536, sizeof (palent), cmp_palent);

  palent* pe_new = new palent[257];
  pe_new[0].r = 0;
  pe_new[0].g = 0;
  pe_new[0].b = 0;
  pe_new[0].cnt = 1;

  int j = 1;
  for (i = 1 ; i < 65536 ; i++)
  {
    int r = pe[i].r;
    int g = pe[i].g;
    int b = pe[i].b;
    int k, min, mindist;
    mindist = 1000*256*256;
    min = -1;
    register int dr, dg, db, dist;
    for (k = 0 ; k < j ; k++)
    {
      dr = r - pe_new[k].r;
      dg = g - pe_new[k].g;
      db = b - pe_new[k].b;
      dist = (299*dr*dr) + (587*dg*dg) + (114*db*db);
      if (dist == 0) { min = k; mindist = dist; break; }
      if (dist < mindist) { mindist = dist; min = k; }
    }
    if (mindist >= 16333)
    {
      pe_new[j].r = r;
      pe_new[j].g = g;
      pe_new[j].b = b;
      pe_new[j].cnt = 1;
      j++;
      if (j >= 255) break;
    }
  }

  // Fake that color 255 (white) is set to white.
  pe_new[255].r = 255;
  pe_new[255].g = 255;
  pe_new[255].b = 255;

  CsPrintf (MSG_DEBUG_0, "Recomputing lookup table...\n");
  //System->Console->Print ();
  //Print ();
  //XFlush (dpy);

  // Recompute the lookup table from 15/16-bit to 8-bit.
  int r, g, b;
  if (sim_depth == 15)
    for (i = 0 ; i < 65536 ; i++)
    {
      r = (i&0x7c00)>>10; r <<= 3;
      g = (i&0x03e0)>>5;  g <<= 3;
      b = (i&0x001f);     b <<= 3;
      sim_lt8[i] = find_rgb_palent (pe_new, r, g, b);
    }
  else
    for (i = 0 ; i < 65536 ; i++)
    {
      r = (i&0xf800)>>11; r <<= 3;
      g = (i&0x07e0)>>5;  g <<= 2;
      b = (i&0x001f);     b <<= 3;
      sim_lt8[i] = find_rgb_palent (pe_new, r, g, b);
    }

  // Allocate colors for the 256 most often occuring
  // colors on the display. This also makes the first 256
  // elements of pe the palette.
  for (i = 0 ; i < 256 ; i++)
  {
    XColor color;
    color.pixel = i;
    color.red   = pe_new[i].r*256;
    color.green = pe_new[i].g*256;
    color.blue  = pe_new[i].b*256;
    color.flags = DoRed | DoGreen | DoBlue;
    XStoreColor (dpy, cmap, &color);
  }

  delete [] pe;
  delete [] pe_new;
  CsPrintf (MSG_DEBUG_0, "Done!\n");
}

void csGraphics2DXLib::recompute_grey_palette ()
{
  if ((sim_depth != 15 && sim_depth != 16 && sim_depth != 32)
   || (real_pfmt.PixelBytes != 1))
    return;

  CsPrintf (MSG_DEBUG_0, "Compute grey palette\n");
  //System->Console->Print ();
  //Print ();
  //XFlush (dpy);

  palent* pe;
  int i;

  // Create a grey palette
  pe = new palent [256];
  for (i = 0 ; i < 256 ; i++)
  {
    pe[i].idx = i;
    pe[i].r = i;
    pe[i].g = i;
    pe[i].b = i;
    pe[i].cnt = 1;
  }

  CsPrintf (MSG_DEBUG_0, "Recomputing lookup table...\n");
  //System->Console->Print ();
  //Print ();
  //XFlush (dpy);

  // Recompute the lookup table from 15/16-bit to 8-bit.
  int r, g, b;
  if (sim_depth == 15)
    for (i = 0 ; i < 65536 ; i++)
    {
      r = (i&0x7c00)>>10; r <<= 3;
      g = (i&0x03e0)>>5;  g <<= 3;
      b = (i&0x001f);     b <<= 3;
      sim_lt8[i] = find_rgb_palent (pe, r, g, b);
    }
  else
    for (i = 0 ; i < 65536 ; i++)
    {
      r = (i&0xf800)>>11; r <<= 3;
      g = (i&0x07e0)>>5;  g <<= 2;
      b = (i&0x001f);     b <<= 3;
      sim_lt8[i] = find_rgb_palent (pe, r, g, b);
    }

  // Allocate colors for the 256 most often occuring
  // colors on the display. This also makes the first 256
  // elements of pe the palette.
  for (i = 0 ; i < 256 ; i++)
  {
    XColor color;
    color.pixel = i;
    color.red   = pe[i].r*256;
    color.green = pe[i].g*256;
    color.blue  = pe[i].b*256;
    color.flags = DoRed | DoGreen | DoBlue;
    XStoreColor (dpy, cmap, &color);
  }

  delete [] pe;
  CsPrintf (MSG_DEBUG_0, "Done!\n");
}

bool csGraphics2DXLib::PerformExtension (const char* args)
{
  if (!strcasecmp (args, "sim_pal"))
  {
    recompute_simulated_palette ();
    return true;
  }
  else if (!strcasecmp (args, "sim_grey"))
  {
    recompute_grey_palette ();
    return true;
  }
  else if (!strcasecmp (args, "sim_332"))
  {
    restore_332_palette ();
    return true;
  }
  return false;
}

void csGraphics2DXLib::Print (csRect *area)
{
  if (sim_depth)
  {
    if (sim_depth == 16 && real_pfmt.PixelBytes == 4)
    {
      UShort* src = (UShort*)Memory;
      ULong* dst = (ULong*)real_Memory;
      int i = Width * Height;
      while (i > 0)
      {
        UShort rgb = *src++;
        *dst++ = (((rgb>>11)&31)<<(real_pfmt.RedShift+3)) | (((rgb>>5)&63)<<(real_pfmt.GreenShift+2)) | (((rgb>>0)&31)<<(real_pfmt.BlueShift+3));
        i--;
      }
    }
    else if (sim_depth == 15 && real_pfmt.PixelBytes == 4)
    {
      UShort* src = (UShort*)Memory;
      ULong* dst = (ULong*)real_Memory;
      int i = Width * Height;
      while (i > 0)
      {
        UShort rgb = *src++;
        *dst++ = (((rgb>>10)&31)<<(real_pfmt.RedShift+3)) | (((rgb>>5)&31)<<(real_pfmt.GreenShift+3)) | (((rgb>>0)&31)<<(real_pfmt.BlueShift+3));
        i--;
      }
    }
    else if (sim_depth == 16 && real_pfmt.PixelBytes == 2)
    {
      UShort* src = (UShort*)Memory;
      UShort* dst = (UShort*)real_Memory;
      int i = Width * Height;
      while (i > 0)
      {
        UShort rgb = *src++;
        *dst++ = (((rgb>>11)&31)<<10) | (((rgb>>6)&31)<<5) | (((rgb>>0)&31)<<0);
        i--;
      }
    }
    else if (sim_depth == 15 && real_pfmt.PixelBytes == 2)
    {
      UShort* src = (UShort*)Memory;
      UShort* dst = (UShort*)real_Memory;
      int i = Width * Height;
      while (i > 0)
      {
        UShort rgb = *src++;
        *dst++ = (((rgb>>10)&31)<<11) | (((rgb>>5)&31)<<(5+1)) | (((rgb>>0)&31)<<0);
        i--;
      }
    }
    else if (sim_depth == 15 || sim_depth == 16)
    {
      UShort* src = (UShort*)Memory;
      unsigned char* dst = real_Memory;
      int i = Width * Height;
      while (i > 0)
      {
        *dst++ = sim_lt8[*src++];
        i--;
      }
    }
    else if (sim_depth == 32 && real_pfmt.PixelBytes == 1)
    {
      ULong* src = (ULong*)Memory;
      unsigned char* dst = real_Memory;
      int i = Width*Height;
      while (i > 0)
      {
        ULong col = *src++;
        UShort scol =
      	  (((col & pfmt.RedMask) >> (pfmt.RedShift+3)) << 11) |
      	  (((col & pfmt.GreenMask) >> (pfmt.GreenShift+2)) << 5) |
      	  (((col & pfmt.BlueMask) >> (pfmt.BlueShift+3)));
        *dst++ = sim_lt8[scol];
        i--;
      }
    }
    else if (sim_depth == 32 && real_pfmt.PixelBytes == 2)
    {
      ULong* src = (ULong*)Memory;
      UShort* dst = (UShort*)real_Memory;
      int i = Width*Height;
      while (i > 0)
      {
        ULong col = *src++;
        UShort scol =
      	  (((col & pfmt.RedMask) >> (pfmt.RedShift+(8-real_pfmt.RedBits))) << real_pfmt.RedShift) |
      	  (((col & pfmt.GreenMask) >> (pfmt.GreenShift+(8-real_pfmt.GreenBits))) << real_pfmt.GreenShift) |
      	  (((col & pfmt.BlueMask) >> (pfmt.BlueShift+(8-real_pfmt.BlueBits))) << real_pfmt.BlueShift);
        *dst++ = scol;
        i--;
      }
    }
    else if (sim_depth == 8)
    {
      // If it is the first time we come here we will create
      // the lookup-table from 8-bit to 15/16-bit. We can only do
      // it here because the palette can change (gamma correction).
      // If the palette changes this lookup table is automatically made
      // invalid (by SetRGB).
      // In case we are in 32-bit mode we also create the lookup-table
      // from 8-bit to 16-bit and we change 16-bit to 32-bit on the fly.
      if (!sim_lt16)
      {
        sim_lt16 = new UShort [256];
	int i, r, g, b;
	for (i = 0 ; i < 256 ; i++)
	{
	  r = Palette[i].red;
	  g = Palette[i].green;
	  b = Palette[i].blue;
          if (real_pfmt.PixelBytes == 2)
	    sim_lt16[i] =
		((r >> (8-real_pfmt.RedBits))   << real_pfmt.RedShift) |
		((g >> (8-real_pfmt.GreenBits)) << real_pfmt.GreenShift) |
		((b >> (8-real_pfmt.BlueBits))  << real_pfmt.BlueShift);
	  else
	    sim_lt16[i] =
		((r >> (8-5)) << 11) |
		((g >> (8-6)) << 5) |
		((b >> (8-5)) << 0);
        }
      }
      if (real_pfmt.PixelBytes == 2)
      {
        unsigned char* src = Memory;
        UShort* dst = (UShort*)real_Memory;
        int i = Width*Height;
        while (i > 0)
        {
          *dst++ = sim_lt16[*src++];
          i--;
        }
      }
      else if (real_pfmt.PixelBytes == 4)
      {
        unsigned char* src = Memory;
        ULong* dst = (ULong*)real_Memory;
        int i = Width*Height;
        while (i > 0)
        {
          UShort rgb = sim_lt16[*src++];
          *dst++ = (((rgb>>11)&31)<<(real_pfmt.RedShift+3)) | (((rgb>>5)&63)<<(real_pfmt.GreenShift+2)) | (((rgb>>0)&31)<<(real_pfmt.BlueShift+3));
          i--;
        }
      }
    }
  }

#ifdef DO_SHM
  if (do_shm)
  {
    if (area)
      XShmPutImage (dpy, window, gc, &shm_image,
		    area->xmin, area->ymin, area->xmin, area->ymin,
		    area->Width (), area->Height (),
		    False);
    else
      XShmPutImage (dpy, window, gc, &shm_image, 0, 0, 0, 0, Width, Height, False);
    XSync (dpy, False);
  }
  else
#endif
  {
    if (area)
      XPutImage (dpy, window, gc, xim,
		 area->xmin, area->ymin, area->xmin, area->ymin,
		 area->Width (), area->Height ());
    else
      XPutImage (dpy, window, gc, xim, 0, 0, 0, 0, Width, Height);
  }
#if 0 // @@@ Temporarily disabled
  if (!LoopStarted)
    XFlush (dpy);
#endif
}

void csGraphics2DXLib::SetRGB(int i, int r, int g, int b)
{
  // If there is a colormap AND we are not simulating a display with
  // a colormap then we really set the color.
  if (cmap && !sim_depth)
  {
    XColor color;
    color.pixel = i;
    color.red = r*256;
    color.green = g*256;
    color.blue = b*256;
    color.flags = DoRed | DoGreen | DoBlue;

    XStoreColor (dpy, cmap, &color);
  }

  // If we are simulating an 8-bit display on truecolor displays then we
  // need to delete the lookup table. It has to be recomputed after changing
  // a palette entry.
  if (sim_depth == 8)
    if (sim_lt16)
    {
      delete [] sim_lt16;
      sim_lt16 = NULL;
    }

  csGraphics2D::SetRGB (i, r, g, b);
}

bool csGraphics2DXLib::SetMousePosition (int x, int y)
{
  XWarpPointer (dpy, None, window, 0, 0, 0, 0, x, y);
  return true;
}

bool csGraphics2DXLib::SetMouseCursor (csMouseCursorID iShape)
{
  if (do_hwmouse
   && (iShape >= 0)
   && (iShape <= csmcWait)
   && (MouseCursor [iShape] != None))
  {
    XDefineCursor (dpy, window, MouseCursor [iShape]);
    return true;
  }
  else
  {
    XDefineCursor (dpy, window, EmptyMouseCursor);
    return (iShape == csmcNone);
  } /* endif */
}

static Bool CheckKeyPress (Display* /*dpy*/, XEvent *event, XPointer arg)
{
  XEvent *curevent = (XEvent *)arg;
  if ((event->type == KeyPress)
   && (event->xkey.keycode == curevent->xkey.keycode)
   && (event->xkey.state == curevent->xkey.state))
    return true;
  return false;
}

// XCheckMaskEvent() doesn't get ClientMessage Events so use XCheckIfEvent()
//    with this Predicate function as a work-around ( ClientMessage events
//    are needed in order to catch "WM_DELETE_WINDOW")
static Bool AlwaysTruePredicate (Display*, XEvent*, char*) { return True; }

//#define DO_STUFF_EXTENDED_KEYCODE
bool csGraphics2DXLib::HandleEvent (csEvent &/*Event*/)
{
  static int button_mapping[6] = {0, 1, 3, 2, 4, 5};
  XEvent event;
  int state, key;
  bool cskey=true;
  bool down;
  bool resize = false;

#ifdef DO_STUFF_EXTENDED_KEYCODE
  unsigned char keytranslated;
#endif

  while (XCheckIfEvent (dpy, &event, AlwaysTruePredicate, 0))
    switch (event.type)
    {
      case ConfigureNotify:
	if ((Width  != event.xconfigure.width) ||
	    (Height != event.xconfigure.height))
	{
	  Width = event.xconfigure.width;
	  Height = event.xconfigure.height;
	  resize = true;
	}
	break;
      case ClientMessage:
	if (static_cast<Atom>(event.xclient.data.l[0]) == wm_delete_window)
	{
	  System->StartShutdown();
	}
	break;
      case ButtonPress:
        state = ((XButtonEvent*)&event)->state;
        System->QueueMouseEvent (button_mapping [event.xbutton.button],
          true, event.xbutton.x, event.xbutton.y);
          break;
      case ButtonRelease:
        System->QueueMouseEvent (button_mapping [event.xbutton.button],
          false, event.xbutton.x, event.xbutton.y);
        break;
      case MotionNotify:
        System->QueueMouseEvent (0, false,
	  event.xbutton.x, event.xbutton.y);
        break;
      case KeyPress:
      case KeyRelease:
        // Neat trick: look in event queue if we have KeyPress events ahead
	// with same keycode. If this is the case, discard the KeyUp event
	// in favour of KeyDown since this is most (sure?) an autorepeat
        XCheckIfEvent (event.xkey.display, &event, CheckKeyPress, (XPointer)&event);
        down = (event.type == KeyPress);
#ifdef DO_STUFF_EXTENDED_KEYCODE
        XLookupString ((XKeyEvent*)&event, (char*)&keytranslated, 1, (KeySym*)&key, NULL);
#else
        key = XLookupKeysym (&event.xkey, 0);
        state = event.xkey.state;
#endif
        switch (key)
        {
          case XK_Meta_L:
	  case XK_Meta_R:
	  case XK_Alt_L:
          case XK_Alt_R:      key = CSKEY_ALT; break;
          case XK_Control_L:
          case XK_Control_R:  key = CSKEY_CTRL; break;
          case XK_Shift_L:
          case XK_Shift_R:    key = CSKEY_SHIFT; break;
	  case XK_KP_Up:
	  case XK_KP_8:
          case XK_Up:         key = CSKEY_UP; break;
	  case XK_KP_Down:
	  case XK_KP_2:
          case XK_Down:       key = CSKEY_DOWN; break;
	  case XK_KP_Left:
	  case XK_KP_4:
          case XK_Left:       key = CSKEY_LEFT; break;
	  case XK_KP_Right:
	  case XK_KP_6:
          case XK_Right:      key = CSKEY_RIGHT; break;
          case XK_BackSpace:  key = CSKEY_BACKSPACE; break;
	  case XK_KP_Insert:
	  case XK_KP_0:
          case XK_Insert:     key = CSKEY_INS; break;
	  case XK_KP_Delete:
	  case XK_KP_Decimal:
          case XK_Delete:     key = CSKEY_DEL; break;
	  case XK_KP_Page_Up:
	  case XK_KP_9:
          case XK_Page_Up:    key = CSKEY_PGUP; break;
	  case XK_KP_Page_Down:
	  case XK_KP_3:
          case XK_Page_Down:  key = CSKEY_PGDN; break;
	  case XK_KP_Home:
	  case XK_KP_7:
          case XK_Home:       key = CSKEY_HOME; break;
	  case XK_KP_End:
	  case XK_KP_1:
          case XK_End:        key = CSKEY_END; break;
          case XK_Escape:     key = CSKEY_ESC; break;
          case XK_Tab:        key = CSKEY_TAB; break;
	  case XK_KP_Enter:
          case XK_Return:     key = CSKEY_ENTER; break;
          case XK_F1:         key = CSKEY_F1; break;
          case XK_F2:         key = CSKEY_F2; break;
          case XK_F3:         key = CSKEY_F3; break;
          case XK_F4:         key = CSKEY_F4; break;
          case XK_F5:         key = CSKEY_F5; break;
          case XK_F6:         key = CSKEY_F6; break;
          case XK_F7:         key = CSKEY_F7; break;
          case XK_F8:         key = CSKEY_F8; break;
          case XK_F9:         key = CSKEY_F9; break;
          case XK_F10:        key = CSKEY_F10; break;
          case XK_F11:        key = CSKEY_F11; break;
          case XK_F12:        key = CSKEY_F12; break;
          default:            cskey=false; break;
        }
#ifdef DO_STUFF_EXTENDED_KEYCODE
        if ( cskey )
	  System->QueueKeyEvent (key, down);
	else
  	  System->QueueExtendedKeyEvent (key, (int)keytranslated, down);
#else
	System->QueueKeyEvent (key, down);
#endif
        break;
      case FocusIn:
      case FocusOut:
        System->QueueFocusEvent (event.type == FocusIn);
        break;
      case Expose:
	if (!resize)
        {
	  csRect rect (event.xexpose.x, event.xexpose.y,
		       event.xexpose.x + event.xexpose.width, 
		       event.xexpose.y + event.xexpose.height);
	  Print (&rect);
	}
	break; 
      default:
        //if (event.type == CompletionType) shm_busy = 0;
        break;
    }
  if (resize)
  { 
    if (!ReallocateMemory ())
    {
      CsPrintf (MSG_FATAL_ERROR, "Unable to allocate memory on resize!\n");
      System->StartShutdown ();
    }
  }
  return false;
}

bool csGraphics2DXLib::ReallocateMemory ()
{
  XSync (dpy, False);
  if (do_shm)
  {
    XShmDetach (dpy, &shmi);
    XDestroyImage (xim);
    shmdt (shmi.shmaddr);
  }
  else
  {
    XDestroyImage (xim);
  }
  xim = NULL;
  if (!AllocateMemory())
  {
    CsPrintf (MSG_FATAL_ERROR, "Unable to allocate memory!\n");
    return false;
  }
  delete [] LineAddress;
  LineAddress = new int [Height];
  if (LineAddress == NULL) return false;
 
  // Initialize scanline address array
  int i,addr,bpl = Width * pfmt.PixelBytes;
  for (i = 0, addr = 0; i < Height; i++, addr += bpl)
    LineAddress[i] = addr;
 
  SetClipRect (0, 0, Width, Height);
  XSync (dpy, False);
  return true;
}

iGraphics2D *csGraphics2DXLib::CreateOffScreenCanvas (int width, int height, 
	   csPixelFormat *pfmt, void *buffer, RGBPixel *palette, int pal_size)
{
  csDynamicTexture2D *tex = new csDynamicTexture2D (System);
  return tex->CreateOffScreenCanvas (width, height, pfmt, buffer, 
				     palette, pal_size);
}
