/*
    Crystal Space Windowing System: default skin
    Copyright (C) 2000 by Andrew Zabolotny <bit@eltech.ru>

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

#ifndef __SDEFAULT_H__
#define __SDEFAULT_H__

#include "csskin.h"

#define SKIN_DECLARE_DEFAULT(var)	\
  SKIN_DECLARE (my##var##Type, csSkin);	\
    SKIN_SLICE (DefaultButton);		\
    SKIN_SLICE (DefaultWindow);		\
    SKIN_SLICE (DefaultDialog);		\
  SKIN_DECLARE_END var

class csButton;
struct iTextureHandle;

/**
 * This is the default skin for buttons.
 */
class csDefaultButtonSkin : public csButtonSkin
{
public:
  /// Draw the component we are responsible for
  virtual void Draw (csComponent &iComp);

  /// Suggest the optimal size of the button, given an already filled object
  virtual void SuggestSize (csButton &This, int &w, int &h);
};

/**
 * This is the default skin for windows.
 */
class csDefaultWindowSkin : public csWindowSkin
{
  iTextureHandle *ButtonTex;
  csSkin *Skin;

public:
  /// Initialize the window skin slice
  csDefaultWindowSkin () : ButtonTex (NULL) {}

  /// Query the required resources from application
  virtual void Initialize (csApp *iApp, csSkin *Parent);

  /// Free the resources allocated in Initialize()
  virtual void Deinitialize ();

  /// Draw the component we are responsible for
  virtual void Draw (csComponent &iComp);

  /// Place all gadgets (titlebar, buttons, menu and toolbar)
  virtual void PlaceGadgets (csWindow &This);

  /// Create a button for window's titlebar
  virtual csButton *CreateButton (csWindow &This, int ButtonID);

  /// Called to reflect some specific window state change on gagdets
  virtual void SetState (csWindow &This, int Which, bool State);

  /// Set window border width and height depending on frame style
  virtual void SetBorderSize (csWindow &This);

protected:
  void SetButtBitmap (csButton *button, const char *id);
};

/**
 * This is the default skin for dialogs.
 */
class csDefaultDialogSkin : public csDialogSkin
{
public:
  /// Draw the component we are responsible for
  virtual void Draw (csComponent &iComp);

  /// Set dialog border width and height depending on frame style
  virtual void SetBorderSize (csDialog &This);
};

#endif // __SDEFAULT_H__
