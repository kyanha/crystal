/*
    Copyright (C) 1999 by Gary Haussmann

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

#ifndef __GL2D_FONT_H__
#define __GL2D_FONT_H__

#include <GL/gl.h>
#include "cs2d/common/graph2d.h"

/**
  This class contains
  basic code to read font information and build appropriate bitmaps in
  OpenGL.  It acts as a GL 'font server'.  You add a font by passing
  in a FontDef struct (see graph2d.h), and the server will add it to
  the list of fonts.  Destruction of the server will free up all the
  bitmaps currently in use.

  To write a series of characters, set the color and position using
  glColor(r,g,b) and glRasterPos2i(x,y)  Note that the raster
  position points to the lower left corner of the first character!
  Then call WriteChars() with the string to print.  If you want to
  use a different font than the default font (the first font built
  in the server) there exists a version of WriteChars() for that
  as well
  */
class csGraphics2DOpenGLFontServer
{
  /// number of fonts currently stored here
  int mFont_Count, mMax_Font_Count;

  /**
   * each font needs some extra information connected to how it is stored
   * internally
   */
  class GLFontInfo;

  GLFontInfo **mFont_Information_Array;

public:
  /**
   * The maximal number of fonts that can be registered.
   * Additional fonts must be added via AddFont()
   */
  csGraphics2DOpenGLFontServer (int MaxFonts);

  /// Destructor cleans up all the OpenGL mess we left around
  ~csGraphics2DOpenGLFontServer ();

  /**
   * Add more fonts to the font server by passing FontDef's into this
   * method.  The font bitmap data will be encoded into an openGL-friendly
   * form
   */
  void AddFont (FontDef &addme);

  /// Check how many fonts are stored in here
  int CountFonts () const { return mFont_Count; }

  /**
   * Draw a string using OpenGL.  It is assumed you have
   * set up the render state using glColor and a modelview transform.
   * To use a non-default font, pass in a second argument with
   * the number of the font to use
   */
  void WriteCharacters (char *writeme, int fontnumber = 0);

  /**
   * Draw a single character using OpenGL.  It is assumed you have
   * set up the render state using glColor and set the modelview
   * transform so that the character can be drawn at (0,0).  The
   * transform will be updated so that the next character will
   * be drawn shifted over, in the correct position.
   * To use a non-default font, pass in a second argument with
   * the number of the font to use
   */
  void WriteCharacter (char writeme, int fontnumber = 0);
};

#endif


