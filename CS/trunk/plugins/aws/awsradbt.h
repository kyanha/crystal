#ifndef __CS_AWS_RADIO_BUTTON_H__
#define __CS_AWS_RADIO_BUTTON_H__

/**************************************************************************
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
*****************************************************************************/
# include "awscomp.h"

class awsRadButton :
  public awsComponent
{
  /// True when button is down, false if up
  bool is_down;

  /// True if the component has the mouse over it
  bool mouse_is_over;

  /// True if this radio button is on
  bool is_on;

  /// Holds the image bitmaps for the radio button (on/off and up/down)
  iTextureHandle *tex[4];

  /// Flags for frame style.
  int frame_style;

  /// Alpha level for this component
  int alpha_level;

  /// Alignment of this component
  int alignment;

  /// Caption text for this component
  iString *caption;
protected:
  /// Broadcasts the clear event to peers.
  void ClearGroup ();
public:
  awsRadButton ();
  virtual ~awsRadButton ();

  /******* Alignment Options ************/

  /// Align text to left.
  static const int alignLeft;

  /// Align text to right
  static const int alignRight;

  /// Align text centered
  static const int alignCenter;

  /******* Signals **********************/

  /// An up and down motion for the button
  static const int signalClicked;

  /// When this button is turned on.
  static const int signalTurnedOn;

  /// When this button is turned off.
  static const int signalTurnedOff;

  /// When this button is becomes focused.
  static const int signalFocused;
public:
  /// Get's the texture handle and the title, plus style if there is one.
  virtual bool Setup (iAws *wmgr, iAwsComponentNode *settings);

  /// Gets properties
  bool GetProperty (const char *name, void **parm);

  /// Sets properties
  bool SetProperty (const char *name, void *parm);

  /// Returns the named TYPE of the component, like "Radio Button", etc.
  virtual const char *Type ();
public:

  /// Overridden to listen to GroupOff messages.
  virtual bool HandleEvent (iEvent &Event);

  /// Triggered when the component needs to draw
  virtual void OnDraw (csRect clip);

  /// Triggered when the user presses a mouse button down
  virtual bool OnMouseDown (int button, int x, int y);

  /// Triggered when the user unpresses a mouse button
  virtual bool OnMouseUp (int button, int x, int y);

  /// Triggered when this component loses mouse focus
  virtual bool OnMouseExit ();

  /// Triggered when this component gains mouse focus
  virtual bool OnMouseEnter ();

  /// Triggered when the user presses a key
  virtual bool OnKeypress (int key, int cha, int modifiers);

  /// Triggered when the component becomes focused
  virtual void OnSetFocus ();
};

class awsRadButtonFactory :
  public awsComponentFactory
{
public:

  /// Calls register to register the component that it builds with the window manager
  awsRadButtonFactory (iAws *wmgr);

  /// Does nothing
  virtual ~awsRadButtonFactory ();

  /// Returns a newly created component of the type this factory handles.
  virtual iAwsComponent *Create ();
};
#endif // __CS_AWS_RADIO_BUTTON_H__
