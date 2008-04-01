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

#ifndef __CS_CSPLUGINCOMMON_CANVAS_GRAPH2D_H__
#define __CS_CSPLUGINCOMMON_CANVAS_GRAPH2D_H__

/**\file
 * Base class for 2D canvases.
 */

#include "csextern.h"

#include "csutil/cfgacc.h"
#include "csutil/scf.h"
#include "csutil/scf_implementation.h"
#include "csutil/weakref.h"

#include "iutil/comp.h"
#include "iutil/dbghelp.h"
#include "iutil/eventh.h"
#include "iutil/plugin.h"
#include "iutil/pluginconfig.h"
#include "iutil/string.h"
#include "ivideo/fontserv.h"
#include "ivideo/graph2d.h"
#include "ivideo/natwin.h"

/**
 * \addtogroup plugincommon
 * @{ */

struct iObjectRegistry;
struct iPluginManager;

class csFontCache;

#include "csutil/deprecated_warn_off.h"

/**
 * This is the base class for 2D canvases. Plugins should derive their 
 * own class from this one and implement required (marked with an 
 * asterisk (*)) functions.
 * Functions not marked with an asterisk are optional, but possibly
 * slow since they are too general.
 */
class CS_CRYSTALSPACE_EXPORT csGraphics2D : 
  public scfImplementation7<csGraphics2D, 
    iGraphics2D, iComponent, iNativeWindow, iNativeWindowManager,
    iPluginConfig, iDebugHelper, iEventHandler>
{
public:
  /// The configuration file.
  csConfigAccess config;

  /// The clipping rectangle.
  int ClipX1, ClipX2, ClipY1, ClipY2;

  /// The pixel format.
  csPixelFormat pfmt;

  /// Most systems have a pointer to (real or pseudo) video RAM.
  unsigned char *Memory;

  /// Open/Close state.
  bool is_open;

  /// Keep a array of Y*width to avoid multiplications.
  int *LineAddress;

  /// The object registry.
  iObjectRegistry* object_reg;
  /// The plugin manager.
  csWeakRef<iPluginManager> plugin_mgr;

  /**
   * Callback to use for informing an external agent when several
   * canvas operations have occured.
   */
  csRef<iOffscreenCanvasCallback> ofscb;

  /// The font server
  csWeakRef<iFontServer> FontServer;
  /// The font cache
  csFontCache* fontCache;

  /// Pointer to a title.
  csString win_title;

  /// The width, height and depth of visual.
  int fbWidth, fbHeight, Depth;

  int vpLeft, vpTop, vpWidth, vpHeight;

  /**
   * Display number.  If 0, use primary display; else if greater than 0, use
   * that display number.  If that display number is not present, use primary
   * display.
   */
  int DisplayNumber;
  /// True if visual is full-screen.
  bool FullScreen;
  /// Whether to allow resizing.
  bool AllowResizing;
  /// 256-color palette.
  csRGBpixel *Palette;
  /// true if some palette entry is already allocated.
  bool PaletteAlloc[256];
  /**
   * The counter that is incremented inside BeginDraw and decremented in
   * FinishDraw().
   */
  int FrameBufferLocked;
  /**
   * Change the depth of the canvas.
   */
  virtual void ChangeDepth (int d);
  /**
   * Get the name of this canvas
   */
  virtual const char *GetName() const;

protected:
  /// Screen refresh rate
  int refreshRate;
  /// Activate Vsync
  bool vsync;

  void CreateDefaultFontCache ();

  csString name;
  csRef<iEventHandler> weakEventHandler;

private:
  /// Find a color in palette mode.
  int FindRGBPalette (int r, int g, int b);
  /**
   * Initialize the canvas for offscreen rendering.
   * Used by CreateOffscreenCanvas().
   */
  bool Initialize (iObjectRegistry* r, int width, int height,
	int depth, void* memory, iOffscreenCanvasCallback* ofscb);

public:
  /// Create csGraphics2D object
  csGraphics2D (iBase*);

  /// Destroy csGraphics2D object
  virtual ~csGraphics2D ();

  /// Initialize the plugin
  virtual bool Initialize (iObjectRegistry*);
  /// Event handler for plugin.
  virtual bool HandleEvent (iEvent&);

  /// (*) Open graphics system (set videomode, open window etc)
  virtual bool Open ();
  /// (*) Close graphics system
  virtual void Close ();

  /// Set clipping rectangle
  virtual void SetClipRect (int xmin, int ymin, int xmax, int ymax);
  /// Query clipping rectangle
  virtual void GetClipRect (int &xmin, int &ymin, int &xmax, int &ymax);

  /**
   * This routine should be called before any draw operations.
   * It should return true if graphics context is ready.
   */
  virtual bool BeginDraw ();
  /// This routine should be called when you finished drawing
  virtual void FinishDraw ();

  /// (*) Flip video pages (or dump backbuffer into framebuffer).
  virtual void Print (csRect const* /*area*/ = 0) { }

  /// Get active videopage number (starting from zero)
  virtual int GetPage ();
  /// Enable or disable double buffering; return TRUE if supported
  virtual bool DoubleBuffer (bool Enable);
  /// Return current double buffering state
  virtual bool GetDoubleBufferState ();

  /// Clear backbuffer
  virtual void Clear (int color);
  /// Clear all video pages
  virtual void ClearAll (int color);

  /**
   * To facilitate multiple pixel formats, the most critical drawing routines
   * are defined as pointers to functions, not as virtual methods.
   * This allows deciding at run-time which function we will choose.
   */
  /// Draw a pixel
  void (*_DrawPixel) (csGraphics2D *This, int x, int y, int color);
  /// Same but exposed through iGraphics2D interface
  virtual void DrawPixel (int x, int y, int color)
  { _DrawPixel (this, x, y, color); }
  virtual void DrawPixels (csPixelCoord const* pixels, int num_pixels,
    int color);
  /// Blit a memory block. Format of the image is RGBA in bytes. Row by row.
  virtual void Blit (int x, int y, int width, int height,
  	unsigned char const* data);

  /// Draw a line
  virtual void DrawLine (float x1, float y1, float x2, float y2, int color);
  /// Draw a box of given width and height
  virtual void DrawBox (int x, int y, int w, int h, int color);
  /// (*) Set a color index to given R,G,B (0..255) values
  virtual void SetRGB (int i, int r, int g, int b);
  virtual int FindRGB (int r, int g, int b, int a = 255)
  {
    if (r < 0) r = 0; else if (r > 255) r = 255;
    if (g < 0) g = 0; else if (g > 255) g = 255;
    if (b < 0) b = 0; else if (b > 255) b = 255;
    if (a < 0) a = 0; else if (a > 255) a = 255;
    if (Depth == 8)
      return FindRGBPalette (r, g, b);
    return
      ((r >> (8 - pfmt.RedBits))   << pfmt.RedShift) |
      ((g >> (8 - pfmt.GreenBits)) << pfmt.GreenShift) |
      ((b >> (8 - pfmt.BlueBits))  << pfmt.BlueShift) |
      ((255 - a) << 24);
    /* Alpha is "inverted" so '-1' can be decomposed to a 
       transparent color. (But alpha not be inverted, '-1'
       would be "opaque white". However, -1 is the color
       index for "transparent text background". */
  }
  virtual void GetRGB (int color, int& r, int& g, int& b);
  virtual void GetRGB (int color, int& r, int& g, int& b, int& a);
  //@{
  /// Write a text string into the back buffer
  virtual void Write (iFont *font , int x, int y, int fg, int bg,
    const char *text, uint flags = 0);
  virtual void Write (iFont *font , int x, int y, int fg, int bg,
    const wchar_t* text, uint flags = 0);  
  //@}
  /// (*) Get address of video RAM at given x,y coordinates
  unsigned char* (*_GetPixelAt) (csGraphics2D *This, int x, int y);
  /// Same but exposed through iGraphics2D interface
  virtual unsigned char *GetPixelAt (int x, int y)
  { return _GetPixelAt (this, vpLeft+x, vpTop+y); }

  /**
   * Return the number of palette entries that can be modified.
   * This should return 0 if there is no palette (true color displays).
   * This function is equivalent to the PalEntries field that you
   * get from GetPixelFormat(). It is just a little bit easier to obtain
   * this way.
   */
  virtual int GetPalEntryCount ()
  { return pfmt.PalEntries; }

  /**
   * Return the number of bytes for every pixel.
   * This function is equivalent to the PixelBytes field that
   * you get from GetPixelFormat().
   */
  virtual int GetPixelBytes ()
  { return pfmt.PixelBytes; }

  /**
   * Return information about about the pixel format.
   */
  virtual csPixelFormat const* GetPixelFormat ()
  { return &pfmt; }

  /**
   * Save a subarea of screen area into the variable Data.
   * Storage is allocated in this call, you should either FreeArea()
   * it after usage or RestoreArea() it.
   */
  virtual csImageArea *SaveArea (int x, int y, int w, int h);
  /// Restore a subarea of screen saved with SaveArea()
  virtual void RestoreArea (csImageArea *Area, bool Free = true);
  /// Free storage allocated for a subarea of screen
  virtual void FreeArea (csImageArea *Area);

  virtual bool SetGamma (float /*gamma*/) { return false; }
  virtual float GetGamma () const { return 1.0; }

  virtual csPtr<iGraphics2D> CreateOffscreenCanvas (
  	void* memory, int width, int height, int depth,
	iOffscreenCanvasCallback* ofscb);

private:
    /// helper function for ClipLine()
  bool CLIPt (float denom, float num, float& tE, float& tL);
public:

  /**
   * Clip a line against given rectangle
   * Function returns true if line is not visible
   */
  virtual bool ClipLine (float &x1, float &y1, float &x2, float &y2,
    int xmin, int ymin, int xmax, int ymax);

  /// Gets the font server
  virtual iFontServer *GetFontServer ()
  { return FontServer; }

  virtual int GetWidth () { return vpWidth; }
  virtual int GetHeight () { return vpHeight; }

  /// Get the palette (if there is one)
  virtual csRGBpixel *GetPalette ()
  { return pfmt.PalEntries ? Palette : 0; }

  /// Query pixel R,G,B at given screen location
  virtual void GetPixel (int x, int y, uint8 &oR, uint8 &oG, uint8 &oB);
  /// As GetPixel() above, but with alpha
  virtual void GetPixel (int x, int y, uint8 &oR, uint8 &oG, uint8 &oB, uint8 &oA);

  /**
   * Perform a system specific extension. Return false if extension
   * not supported.
   */
  virtual bool PerformExtension (char const* command, ...);

  /**
   * Perform a system specific extension. Return false if extension
   * not supported.
   */
  virtual bool PerformExtensionV (char const* command, va_list);

  /// Do a screenshot: return a new iImage object
  virtual csPtr<iImage> ScreenShot ();

  /// Enable/disable canvas resize (Over-ride in sub classes)
  virtual void AllowResize (bool /*iAllow*/) { };

  /// Resize the canvas
  virtual bool Resize (int w, int h);

  /// Return the Native Window interface for this canvas (if it has one)
  virtual iNativeWindow* GetNativeWindow ();

  /// Returns 'true' if the program is being run full-screen.
  virtual bool GetFullScreen ()
  { return FullScreen; }

  /**
   * Change the fullscreen state of the canvas.
   */
  virtual void SetFullScreen (bool b);

  /// Set mouse cursor position; return success status
  virtual bool SetMousePosition (int x, int y);

  /**
   * Set mouse cursor to one of predefined shape classes
   * (see csmcXXX enum above). If a specific mouse cursor shape
   * is not supported, return 'false'; otherwise return 'true'.
   * If system supports it and iBitmap != 0, shape should be
   * set to the bitmap passed as second argument; otherwise cursor
   * should be set to its nearest system equivalent depending on
   * iShape argument.
   */
  virtual bool SetMouseCursor (csMouseCursorID iShape);

  /**
   * Set mouse cursor using an image.  If the operation is unsupported, 
   * return 'false' otherwise return 'true'.
   * On some platforms there is only monochrome pointers available.  In this
   * all black colors in the image will become the value of 'bg' and all 
   * non-black colors will become 'fg'
   */
  virtual bool SetMouseCursor (iImage *image, const csRGBcolor* keycolor = 0, 
                               int hotspot_x = 0, int hotspot_y = 0,
                               csRGBcolor fg = csRGBcolor(255,255,255),
                               csRGBcolor bg = csRGBcolor(0,0,0));
  
  void SetViewport (int left, int top, int width, int height);
  void GetViewport (int& left, int& top, int& width, int& height)
  { left = vpLeft; top = vpTop; width = vpWidth; height = vpHeight; }
  
  void GetFramebufferDimensions (int& width, int& height)
  { width = fbWidth; height = fbHeight; }

  CS_EVENTHANDLER_NAMES("crystalspace.graphics2d.common")
  CS_EVENTHANDLER_NIL_CONSTRAINTS

protected:
  /**
   * Default drawing routines for 8-bit and 16-bit modes
   * If a system port has its own routines, it should assign
   * their addresses to respective pointers.
   */

  /// Draw a pixel in 8-bit modes
  static void DrawPixel8 (csGraphics2D *This, int x, int y, int color);
  /// Return address of a 8-bit pixel
  static unsigned char *GetPixelAt8 (csGraphics2D *This, int x, int y);

  /// Draw a pixel in 16-bit modes
  static void DrawPixel16 (csGraphics2D *This, int x, int y, int color);
  /// Return address of a 16-bit pixel
  static unsigned char *GetPixelAt16 (csGraphics2D *This, int x, int y);

  /// Draw a pixel in 32-bit modes
  static void DrawPixel32 (csGraphics2D *This, int x, int y, int color);
  /// Return address of a 32-bit pixel
  static unsigned char *GetPixelAt32 (csGraphics2D *This, int x, int y);

  /**\name iNativeWindowManager implementation
   * @{ */
  // Virtual Alert function so it can be overridden by subclasses
  // of csGraphics2D.
  virtual void AlertV (int type, const char* title, const char* okMsg,
    const char* msg, va_list args);
  virtual void Alert (int type, const char* title, const char* okMsg,
      const char* msg, ...);
  virtual void AlertV (int type, const wchar_t* title, const wchar_t* okMsg,
    const wchar_t* msg, va_list args);
  virtual void Alert (int type, const wchar_t* title, const wchar_t* okMsg,
      const wchar_t* msg, ...);
  /** @} */

  /**\name iNativeWindow implementation
   * @{ */
  // Virtual SetTitle function so it can be overridden by subclasses
  // of csGraphics2D.
  virtual void SetTitle (const char* title);
  virtual void SetTitle (const wchar_t* title)
  { SetTitle (csString (title)); }
  /** @} */

  /**\name iPluginConfig implementation
   * @{ */
  virtual bool GetOptionDescription (int idx, csOptionDescription*);
  virtual bool SetOption (int id, csVariant* value);
  virtual bool GetOption (int id, csVariant* value);
  /** @} */

  /**\name iDebugHelper implementation
   * @{ */
  virtual bool DebugCommand (const char* cmd);
  virtual int GetSupportedTests () const { return 0; }
  virtual csPtr<iString> UnitTest () { return 0; }
  virtual csPtr<iString> StateTest ()  { return 0; }
  virtual csTicks Benchmark (int /*num_iterations*/) { return 0; }
  virtual csPtr<iString> Dump ()  { return 0; }
  virtual void Dump (iGraphics3D* /*g3d*/)  { }
  /** @} */
};

#include "csutil/deprecated_warn_on.h"

/** @} */

#endif // __CS_CSPLUGINCOMMON_CANVAS_GRAPH2D_H__
