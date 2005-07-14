/*
    Copyright (C) 2005 by Christopher Nelson

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

#ifndef __AWS_FRAME_H__
#define __AWS_FRAME_H__

#include "cstool/pen.h"



namespace aws
{

  /** A frame presents different transformation methods for various vector operations.  It may also attempt to transform certain raster/blitting operations. */
  class frame
  {    
    /// The pen that we use to draw with.
    iPen *pen;

    /// This is the transformation that we apply to each operation that occurs.
    csMatrix3 m;

  public:
    frame();

    /// Sets the current pen.
    void SetPen(iPen *_pen);
    
    /// Clears all transformations applied to this frame.
    void ClearTransforms();

    /// Rotates the frame by the given amount (in radians).
    void Rotate(float angle);

    /// Translates the frame by the given amount.
    void Translate(float x, float y);

    // Scales the frame by the given amount.
    void Scale(float x, float y);

  };


};

#endif
