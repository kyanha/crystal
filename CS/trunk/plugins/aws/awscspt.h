#ifndef __CS_AWS_CSPT_H__
#define __CS_AWS_CSPT_H__

/**************************************************************************
    Copyright (C) 2000-2001 by Christopher Nelson
    	      (c) 2001 F.Richter

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
# include "cstool/proctex.h"
# include "ivideo/graph2d.h"
# include "csutil/csvector.h"
# include "ivideo/graph3d.h"
# include "csgeom/transfrm.h"
# include "av3dtxt.h"

/// Handle to our procedural texture, which the user can have us draw on.
class awsSimpleCanvas :
  public csProcTexture
{
public:
  /// Create a new texture.
  awsSimpleCanvas ();

  /// Destroy this texture
  virtual ~awsSimpleCanvas () { }

  /// This is actually not used ever.  The window manager doesn't "animate", and only refreshes the canvas when needed.
  virtual void Animate (csTicks current_time);

  /// Set dimensions of texture
  void SetSize (int w, int h);
};

class awsSingleProctexCanvas :
  public iAwsCanvas
{
private:
  awsSimpleCanvas *canvas;
public:
  SCF_DECLARE_IBASE;

  iGraphics2D *rG2D;
  iGraphics3D *rG3D;

  awsSingleProctexCanvas (
    int w,
    int h,
    iObjectRegistry *object_reg,
    iEngine *engine,
    iTextureManager *txtmgr,
    const char *name);
  virtual ~awsSingleProctexCanvas ();

  virtual void Animate (csTicks current_time);

  iGraphics2D *G2D () { return canvas->GetG2D (); }
  iGraphics3D *G3D () { return canvas->GetG3D (); }
  virtual void Show (
                csRect *area = 0,
                iGraphics3D *g3d = 0,
                uint8 Alpha = 0);
};
#endif // __CS_AWS_CSPT_H__
