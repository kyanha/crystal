/*
    Copyright (C) 2000-2001 by Christopher Nelson
  
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

#ifndef __CS_AWS_TXTBX_H__
#define __CS_AWS_TXTBX_H__

#include "awscomp.h"
#include "iutil/csinput.h"

class awsTextBox : public awsComponent
{
private:
  /// True if the component has the mouse over it.
  bool mouse_is_over;

  /// True if the component has focus.
  bool has_focus;

  /// True if the component should mask chars.
  int should_mask;

  /**
   * Holds the background texture: either global texture, or
   * override in component.
   */
  iTextureHandle *bkg;

  /// Flags for frame style.
  int frame_style;

  /// Alpha level for this component.
  int alpha_level;

  /// Text of this component, editable.
  iString *text;

  /// Text that's not allowed to add.
  iString *disallow;

  /// Character to replace text with.
  iString *maskchar;

  /// Position of first character we display.
  //int start;
  int strStart;

  /// Position of cursor.
  //unsigned int cursor;
  uint strCursor;

  /// The timer that makes the cursor blink.
  awsTimer *blink_timer;

  /// The current blink state.
  bool blink;

  /// The key composer.
  csRef<iKeyComposer> composer;

  /**
   * Make sure that the start of the displayed text is \p dist chars before 
   * the cursor, if possible.
   */
  void EnsureCursorToStartDistance (int dist);
public:
  awsTextBox ();
  virtual ~awsTextBox ();

  /// A "normal" textbox.  Is textured if there is a background texture.
  static const int fsNormal;

  /// A textbox whose background is defined entirely by the bitmap.
  static const int fsBitmap;

  /// Occurs whenever text is changed.
  static const int signalChanged;

  /// Occurs whenever the component loses keyboard focus.
  static const int signalLostFocus;

  /// Occurs when the "enter" key is pressed.
  static const int signalEnterPressed;

  /// Occures when the "tab" key is pressed.
  static const int signalTabPressed;

  /// Occures when component becomes focused
  static const int signalFocused;

  /// Get's the texture handle and the title, plus style if there is one.
  virtual bool Setup (iAws *wmgr, iAwsComponentNode *settings);

  /// Get properties.
  bool GetProperty (const char *name, void **parm);

  /// Set properties
  bool SetProperty (const char *name, void *parm);

  /// Returns the named TYPE of the component, like "Radio Button", etc.
  virtual const char *Type ();

  /// Triggered when the component needs to draw.
  virtual void OnDraw (csRect clip);

  /// Triggered when the user presses a mouse button down.
  virtual bool OnMouseDown (int button, int x, int y);

  /// Triggered when this component loses mouse focus.
  virtual bool OnMouseExit ();

  /// Triggered when this component gains mouse focus.
  virtual bool OnMouseEnter ();

  /// Triggered when the user presses a key.
  virtual bool OnKeyboard (const csKeyEventData& eventData);

  /// Triggered when the keyboard focus is lost.
  virtual bool OnLostFocus ();

  /// Triggered when the keyboard focus is gained.
  virtual bool OnGainFocus ();

  /// Triggered when component becomes focused.
  virtual void OnSetFocus ();
};

class awsTextBoxFactory : public awsComponentFactory
{
public:
  /**
   * Calls register to register the component that it builds with
   * the window manager.
   */
  awsTextBoxFactory (iAws *wmgr);

  /// Does nothing.
  virtual ~awsTextBoxFactory ();

  /// Returns a newly created component of the type this factory handles.
  virtual iAwsComponent *Create ();
};

#endif // __CS_AWS_TXTBX_H__
