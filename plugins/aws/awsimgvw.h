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

#ifndef __CS_AWS_IMGVW_H__
#define __CS_AWS_IMGVW_H__

#include "awscomp.h"

class awsImageView : public awsComponent
{
private:
  /// True when button is down, false if up.
  bool is_down;

  /// True if the component has the mouse over it.
  bool mouse_is_over;

  /// True if button was down, and button is in switch mode (toggle = yes).
  bool was_down;

  /// Holds the texture handle for the image we are viewing.
  iTextureHandle *img1; // Via Image.
  iTextureHandle *img2; // Via Texture.

  /// True if we should draw a solid color instead.
  bool draw_color;

  /// The color to draw if draw_color is true.
  int color;

  /// Flags for frame style.
  int frame_style;

  /// Alpha level for this component.
  int alpha_level;
public:
  awsImageView ();
  virtual ~awsImageView ();

  /// Frame styles.
  static const int fsBump;
  static const int fsSimple;
  static const int fsRaised;
  static const int fsSunken;
  static const int fsFlat;
  static const int fsNone;
  static const int fsScaled;
  static const int fsTiled;
  static const int fsFixed;
  static const int frameMask;
  static const int imageMask;

  /// An up and down motion for the button.
  static const int signalClicked;

  /// A down motion for the button.
  static const int signalMouseDown;

  /// An up motion for the button.
  static const int signalMouseUp;

  /// A movement of the mouse.
  static const int signalMouseMoved;

  /// Get's the texture handle and the title, plus style if there is one.
  virtual bool Setup (iAws *wmgr, iAwsComponentNode *settings);

  /// Gets properties.
  bool GetProperty (const char *name, void **parm);

  /// Sets properties.
  bool SetProperty (const char *name, void *parm);

  void SetColor(int color);

  int GetColor();

  /// Returns the named TYPE of the component, like "Radio Button", etc.
  virtual const char *Type ();

  /// Triggered when the component needs to draw.
  virtual void OnDraw (csRect clip);

  /// Triggered when the user presses a mouse button down.
  virtual bool OnMouseDown (int button, int x, int y);

  /// Triggered when the user unpresses a mouse button.
  virtual bool OnMouseUp (int button, int x, int y);

  /// Triggered when the user moves the mouse.
  virtual bool OnMouseMove (int button, int x, int y);

  /// Triggered when this component loses mouse focus.
  virtual bool OnMouseExit ();

  /// Triggered when this component gains mouse focus.
  virtual bool OnMouseEnter ();
};

class awsImageViewFactory : public awsComponentFactory
{
public:
  /**
   * Calls register to register the component that it builds with the
   * window manager.
   */
  awsImageViewFactory (iAws *wmgr);

  /// Does nothing.
  virtual ~awsImageViewFactory ();

  /// Returns a newly created component of the type this factory handles.
  virtual iAwsComponent *Create ();
};

#endif // __CS_AWS_IMGVW_H__
