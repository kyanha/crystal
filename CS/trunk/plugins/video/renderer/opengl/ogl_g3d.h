/*
    Copyright (C) 1998-2000 by Jorrit Tyberghein

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

#ifndef GRAPH3D_OPENGL_H
#define GRAPH3D_OPENGL_H

// GRAPH3D.H
// csGraphics3DOpenGL OpenGL rasterizer class.

// Concieved by Jorrit Tyberghein and Gary Clark
// Expanded by Dan Ogles
// Further expanded by Gary Haussmann

#include <GL/gl.h>

#include "csutil/scf.h"
#include "ogl_g3dcom.h"
#include "video/renderer/common/dtmesh.h"
#include "video/renderer/common/dpmesh.h"
#include "iplugin.h"

struct iGraphics3D;

///
class csGraphics3DOpenGL : public csGraphics3DOGLCommon
{
public:
  DECLARE_IBASE;
  ///
  csGraphics3DOpenGL (iBase *iParent);
  ///
  virtual ~csGraphics3DOpenGL ();

  ///
  virtual bool Initialize (iSystem *iSys);
  ///
  virtual bool Open(const char *Title);
  ///
  virtual void Close();

};

#endif
