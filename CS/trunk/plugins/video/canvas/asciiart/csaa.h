/*
    ASCII art rendering support for Crystal Space 3D library
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

#ifndef __CSAA_H__
#define __CSAA_H__

#include "csutil/scf.h"
#include "video/canvas/common/graph2d.h"
#include <aalib.h>

/**
 * The Ascii Art driver. This is a cross-platform graphics driver which
 * implements drawing using characters and (depending on platform)
 * different intensities.
 */
class csGraphics2DAA : public csGraphics2D
{
  /// The configuration file
  csIniFile* config;
  /// Use native mouse cursor, if possible?
  bool HardwareCursor;
  /// The AAlib context
  aa_context *context;
  /// The palette
  aa_palette palette;

public:
  DECLARE_IBASE;

  csGraphics2DAA (iBase *iParent);
  virtual ~csGraphics2DAA ();

  virtual bool Initialize (iSystem *pSystem);
  virtual bool Open (const char *Title);
  virtual void Close ();

  virtual void Print (csRect *area = NULL);

  virtual void SetRGB (int i, int r, int g, int b);

  /* These procedures locks/unlocks DIVE buffer */
  virtual bool BeginDraw ();
  virtual void FinishDraw ();

  virtual bool SetMousePosition (int x, int y);
  virtual bool SetMouseCursor (csMouseCursorID iShape);

  virtual iGraphics2D *CreateOffScreenCanvas (int /*width*/, int /*height*/, 
     csPixelFormat */*pfmt*/, void */*buffer*/, RGBPixel */*palette*/, int /*pal_size*/)
  { return NULL; }
};

#endif // __CSAA_H__
