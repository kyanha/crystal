/*
    Crystal Space Windowing System: Windowing System Application class interface
    Copyright (C) 1998,1999 by Andrew Zabolotny <bit@eltech.ru>

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

#ifndef __CSAPP_H__
#define __CSAPP_H__

#define CSWS_INTERNAL
#include "csws.h"
#include "cscomp.h"
#include "cswstex.h"
#include "csgfxppl.h"
#include "cssys/cseventq.h"
#include "csutil/csstrvec.h"

class cswsSystemDriver;

/**
 * Application's background styles
 */
enum csAppBackgroundStyle
{
  /// Nothing: use only if desktop is not visible to avoid extra drawing
  csabsNothing = 0,
  /// Solid background of palette[0] color
  csabsSolid
};

/**
 * This class is a top-level CrystalSpace Windowing Toolkit object.<p>
 * Generally there should be only one object of this class.
 * Usually it is the root of entire window hierarchy.
 * This class also owns the application-global mouse object,
 * event queue object, graphics pipeline and some others.
 */
class csApp : public csComponent
{
protected:
  friend class cswsSystemDriver;

  /// The graphics pipeline
  csGraphicsPipeline *GfxPpl;
  /// The mouse pointer
  csMouse *Mouse;
  /// The list of windowing system textures
  csWSTexVector Textures;
  /// Application background style
  csAppBackgroundStyle BackgroundStyle;
  /// Window list width and height
  int WindowListWidth, WindowListHeight;
  /// Current & old mouse pointer ID
  csMouseCursorID MouseCursorID, OldMouseCursorID;
  /// The recursion level within System->Loop ()
  int LoopLevel;
  /// The code that dialog passed to Dismiss ()
  int DismissCode;
  /// This is equal to 8 if any of physical r,g,b masks is 0xff000000
  int PhysColorShift;
  /// Current time (passed by systemdriver to StartFrame())
  time_t CurrentTime;

  /// Set up initial application layout (read configs, create windows, menus etc)
  virtual bool InitialSetup (int argc, const char* const argv[],
    const char *iConfigName, const char* iDataDir);

  /// Force redrawing of full screen
  void ForceFullRedraw (bool bFull)
  { GfxPpl->bFullRedraw = bFull; }
  
public:
  /// The system driver
  cswsSystemDriver *System;
  /// Application's adaptive palette
  int Pal [cs_Color_Last];
  /// The component that captured the mouse
  csComponent *MouseOwner;
  /// The component that captured the keyboard
  csComponent *KeyboardOwner;
  /// The component that captured all focused events (mouse & keyboard)
  csComponent *FocusOwner;
  /// If this flag is set, there are still unrefreshed components
  bool RedrawFlag;
  /// This is set to TRUE each time top-level window list changes
  bool WindowListChanged;
  /// Screen width and height (application can be actually smaller)
  int ScreenWidth, ScreenHeight;

  /// Titlebar buttons definitions
  csStrVector *titlebardefs;
  /// Dialog buttons definitions
  csStrVector *dialogdefs;
  /// Global "Insert" key state
  bool insert;

  /// Initialize windowing system
  csApp (const char *AppTitle, csAppBackgroundStyle iBackgroundStyle = csabsSolid);
  /// Deinitialize windowing system
  virtual ~csApp ();

  /// Shut down the program
  void ShutDown ();

  /// The windowing system is idle: do some lazy work
  virtual void Idle ();

  /// Draw the application background
  virtual void Draw ();

  /// This should be called once per frame by system driver
  virtual void StartFrame (time_t ElapsedTime, time_t CurrentTime);
  /// This is called at the end of each frame
  virtual void FinishFrame ();

  /// Process all events in event queue
  virtual bool ProcessEvents ();

  /// Process all events in the queue and refresh the screen
  void FlushEvents ();

  /// Display a string on the console using almost usual printf() syntax
  void printf (int mode, char* str, ...);

  /// Add a single texture to application's texture list
  bool LoadTexture (const char *iTexName, const char *iTexParams,
    int iFlags);

  /// Prepare textures for usage (register them with the graphics driver)
  virtual void PrepareTextures ();

  /// Start endless event loop
  virtual void Loop ();

  /// Return application's texture list
  csWSTexVector *GetTextures ()
  { return &Textures; }

  /// Find a texture by name
  iTextureHandle *GetTexture (char *Name)
  {
    csWSTexture *tex = GetTextures ()->FindTexture (Name);
    return tex ? tex->GetHandle () : NULL;
  }

  /// Return application's global mouse object
  csMouse *GetMouse () { return Mouse; }

  /// Set mouse cursor pointer
  void SetMouseCursor (csMouseCursorID ID) { MouseCursorID = ID; }

  /// Query mouse cursor pointer
  csMouseCursorID GetMouseCursor () { return MouseCursorID; }

  /// Add a event to event queue
  void PutEvent (csEvent *Event);

  /// Handle a event and return true if processed
  virtual bool HandleEvent (csEvent &Event);

  /// Return active page number
  int GetPage ();

  /// Capture all mouse events (or disable capture if NULL)
  csComponent *CaptureMouse (csComponent *who)
  { csComponent *c = MouseOwner; MouseOwner = who; return c; }

  /// Capture all keyboard events (or disable capture if NULL)
  csComponent *CaptureKeyboard (csComponent *who)
  { csComponent *c = KeyboardOwner; KeyboardOwner = who; return c; }

  /// Capture all focused events (or disable capture if NULL)
  csComponent *CaptureFocus (csComponent *who)
  { csComponent *c = FocusOwner; FocusOwner = who; return c; }

  /// Query shift key state
  bool GetKeyState (int iKey);

  /// Query current time
  time_t GetCurrentTime ()
  { return CurrentTime; }

  /// Show window list
  void WindowList ();

  /// Set window list size
  void SetWindowListSize (int iWidth, int iHeight)
  { WindowListWidth = iWidth; WindowListHeight = iHeight; }

  /// Insert a child component
  virtual void Insert (csComponent *comp);

  /// Delete a child component
  virtual void Delete (csComponent *comp);

  /// Execute a dialog box (or entire app if NULL) and return its dismiss code
  int Execute (csComponent *comp);

  /// Dismiss a dialog box with given return code
  void Dismiss (int iCode = cscmdCancel);

  /// Handle a event before all others
  virtual bool PreHandleEvent (csEvent &Event);

  /// Handle a event if nobody eaten it.
  virtual bool PostHandleEvent (csEvent &Event);

/*
 * The following methods are simple redirectors to csGraphicsPipeline
 * object (which is private property of csApp class).
 */

  /// Return a color identifier given R,G,B (each 0..255)
  int FindColor (int r, int g, int b);

  /// Convert a logical color into physical
  int pplColor (int color)
  { return color & 0x80000000 ? (color & 0x7fffffff) << PhysColorShift : Pal [color]; }

  /// Draw a box
  void pplBox (int x, int y, int w, int h, int color)
  { GfxPpl->Box (x, y, w, h, pplColor (color)); }

  /// Draw a line
  void pplLine (float x1, float y1, float x2, float y2, int color)
  { GfxPpl->Line (x1, y1, x2, y2, pplColor (color)); }

  /// Draw a pixel
  void pplPixel (int x, int y, int color)
  { GfxPpl->Pixel (x, y, pplColor (color)); }

  /// Draw a text string: if bg < 0 background is not drawn
  void pplText (int x, int y, int fg, int bg, int Font, int FontSize, const char *s)
  { GfxPpl->Text (x, y, pplColor (fg), bg >= 0 ? pplColor (bg) : bg, Font, FontSize, s); }

  /// Draw a (scaled) 2D sprite
  void pplSprite2D (csPixmap *s2d, int x, int y, int w, int h)
  { GfxPpl->Sprite2D (s2d, x, y, w, h); }

  /// Save a part of screen
  void pplSaveArea (csImageArea *&Area, int x, int y, int w, int h)
  { GfxPpl->SaveArea (&Area, x, y, w, h); }
  /// Restore a part of screen
  void pplRestoreArea (csImageArea *Area, bool Free = false)
  { GfxPpl->RestoreArea (Area, Free); }
  /// Free buffer used to keep an area of screen
  void pplFreeArea (csImageArea *Area)
  { GfxPpl->FreeArea (Area); }

  /// Clear page with specified color
  void pplClear (int color)
  { GfxPpl->Clear (pplColor (color)); }

  /// Set clipping rectangle: SHOULD CALL pplRestoreClipRect() AFTER DRAWING!
  void pplSetClipRect (int xmin, int ymin, int xmax, int ymax)
  { GfxPpl->SetClipRect (xmin, ymin, xmax, ymax); }

  /// Same, but with csRect argument
  void pplSetClipRect (csRect &clip)
  { GfxPpl->SetClipRect (clip.xmin, clip.ymin, clip.xmax, clip.ymax); }

  /// Restore clipping rectangle to (0, 0, ScreenW, ScreenH);
  void pplRestoreClipRect ()
  { GfxPpl->RestoreClipRect (); }

  /// Return the width of given text using currently selected font
  int TextWidth (const char *text, int Font, int FontSize)
  { return GfxPpl->TextWidth (text, Font, FontSize); }
  /// Return the height of currently selected font
  int TextHeight (int Font, int FontSize)
  { return GfxPpl->TextHeight (Font, FontSize); }

  /// Clip a line against a rectangle and return true if its clipped out
  bool ClipLine (float &x1, float &y1, float &x2, float &y2,
    int ClipX1, int ClipY1, int ClipX2, int ClipY2)
  { return GfxPpl->ClipLine (x1, y1, x2, y2, ClipX1, ClipY1, ClipX2, ClipY2); }

  /// Change system mouse cursor and return success status
  bool SwitchMouseCursor (csMouseCursorID Shape)
  { return GfxPpl->SwitchMouseCursor (Shape); }

  /// Get R,G,B at given screen location
  void GetPixel (int x, int y, UByte &oR, UByte &oG, UByte &oB)
  { GfxPpl->GetPixel (x, y, oR, oG, oB); }

  //--- 3D drawing ---//

  /// Draw a 3D polygon
  void pplPolygon3D (G3DPolygonDPFX &poly, UInt mode)
  { GfxPpl->Polygon3D (poly, mode); }

  /// Clear the Z-buffer in the given area
  void pplClearZbuffer (int x1, int y1, int x2, int y2)
  { GfxPpl->ClearZbuffer (x1, y1, x2, y2); }

  /// Clear the entire Z-buffer (takes effect before next 3D draw operation)
  void pplClearZbuffer ()
  { GfxPpl->ClearZbuffer (); }

  /// Set the respective Z-buffer mode (one of CS_ZBUF_XXX constants)
  void SetZbufferMode (unsigned mode)
  { GfxPpl->SetZbufferMode (mode); }

  /// Begin drawing: users of CSWS should NEVER invoke G2D/G3D->BeginDraw!
  void pplBeginDraw (unsigned mode)
  { GfxPpl->BeginDraw (mode); }

protected:
  /// Initialize configuration data: load csws.cfg
  virtual void LoadConfig ();
  /// setup palette
  void SetupPalette ();
};

#endif // __CSAPP_H__
