/*
    Copyright (C) 2000 by Michael Dale Long

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

#ifndef __CSCONOUT_H__
#define __CSCONOUT_H__

#include "ivaria/conout.h"
#include "isys/plugin.h"
#include "csgeom/csrect.h"
#include "csgfx/rgbpixel.h"

struct iGraphics2D;
struct iGraphics3D;
class csConsoleBuffer;

class csConsoleOutput : public iConsoleOutput
{
public:
  DECLARE_IBASE;

  csConsoleOutput (iBase *base);
  virtual ~csConsoleOutput ();

  /// Initialize the console
  virtual bool Initialize (iSystem *);
  /// Handle broadcast events
  virtual bool HandleEvent (iEvent &Event);

  /**
   * Put some text to the console. Console acts like a simple
   * TTY and should interpret basical symbols like '\n' and '\b'.
   * The '\r' character has a special meaning: it sets a flag that
   * tells console to clear the current line before next character
   * is output. That is, you can emmit non-persistent messages
   * this way: PutText ("some text\r"); This message will disappear
   * as soon as any other message will be sent to console.
   */
  virtual void PutText (int iMode, const char *iText);

  /// Return a line from the buffer (-1 = current line)
  virtual const char *GetLine (int iLine = -1) const;

  /**
   * Display the console and return the dirty rectangle.
   * The graphics driver should be in 2D draw mode.
   */
  virtual void Draw2D (csRect *oRect = NULL);

  /**
   * Update the 3D part of the console on the window.
   * The graphics driver should be in 3D draw mode.
   */
  virtual void Draw3D (csRect *oRect = NULL)
  {(void) oRect;}

  /**
   * Clear console. If wipe = false, it just moves the top line to the
   * current line; if wipe is true, it clears the buffer completely.
   */
  virtual void Clear (bool iWipe = false);

  /// Set the buffer size in lines
  virtual void SetBufferSize (int iMaxLines);

  /// Retrieve the transparency setting
  virtual bool GetTransparency () const
  { return transparent; }
  /// Set transparency
  virtual void SetTransparency (bool iTransp)
  { transparent = iTransp; }

  /// Gets the ID of current font.
  virtual iFont *GetFont () const
  { return font; }
  /// Sets the type of the font.
  virtual void SetFont (iFont *Font);

  /// Get the current top line being displayed
  virtual int GetTopLine () const;
  /**
   * Set the current top line, or use of the constants above for scrolling.
   * If snap is true, the console returns to the very bottom of the display
   * when a new line is printed.
   */
  virtual void ScrollTo (int iTopLine, bool iSnap = true);

  /// Retrieve the cursor style
  virtual int GetCursorStyle () const
  { return cursor; }
  /// Assign the cursor style
  virtual void SetCursorStyle (int iStyle)
  { cursor = iStyle; }

  /**
   * Show/hide the console. In 'hidden' state console should not display
   * anything at all (when Draw() is called) or draw some minimal information
   */
  virtual void SetVisible (bool iShow);
  /**
   * Query whether the console is visible or hidden.
   */
  virtual bool GetVisible ()
  { return visible; }

  /**
   * Enable or disable automatic console updates.
   * When the console is in console auto-update mode, it automatically
   * calls BeginDraw/Console->Draw methods on every PutText call.
   * Otherwise it is your responsability to call Draw() at appropiate
   * times. Initially this mode is enabled.
   */
  virtual void AutoUpdate (bool iAutoUpdate)
  { auto_update = iAutoUpdate; }

  /// Set cursor horizontal position (-1 == follow output)
  virtual void SetCursorPos (int iCharNo);

  /// Query maximal line width in characters
  virtual int GetMaxLineWidth ()
  { return 200; }

  /**
   * Tell console that this object should be notified when console 
   * visibility status changes.
   */
  virtual void RegisterPlugin (iPlugIn *iClient)
  { Client = iClient; }

  /// Implement simple extension commands.
  virtual bool ConsoleExtension (const char *iCommand, ...);

  /// Implement simple extension commands.
  virtual bool ConsoleExtensionV (const char *iCommand, va_list);

  // Implement iPlugIn interface.
  struct eiPlugIn : public iPlugIn
  {
    DECLARE_EMBEDDED_IBASE(csConsoleOutput);
    virtual bool Initialize (iSystem* p) { return scfParent->Initialize(p); }
    virtual bool HandleEvent (iEvent& e) { return scfParent->HandleEvent(e); }
  } scfiPlugIn;

private:
  void GetPosition (int &x, int &y, int &width, int &height) const;
  void SetPosition (int x, int y, int width = -1, int height = -1);
  void Invalidate (csRect &area);
  void DeleteText (int start, int end);
  void CacheColors ();
  void GetCursorPos(int &x, int &y) const;
  void SetCursorPos(int x, int y);

  csConsoleBuffer *buffer;
  bool transparent, do_snap;
  iGraphics2D *G2D;
  iGraphics3D *G3D;
  iSystem *System;
  csRect size, invalid;
  int cursor, cx, cy;
  iFont *font;
  cs_time flash_time, flash_interval;
  bool cursor_visible;
  bool clear_input;
  bool auto_update;
  bool system_ready;
  bool visible;
  iPlugIn *Client;

  //  Foreground and background colors
  csRGBcolor fg_rgb;
  csRGBcolor bg_rgb;
  // The texture manager codes for the colors
  int fg, bg;
};

#endif // __CSCONOUT_H__
