/*
    Copyright (C) 1998 by Jorrit Tyberghein
  
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

#include "sysdef.h"
#include "cscom/com.h"
#include "cs2d/openglwin/oglg2d.h"
#include "cs2d/openglwin/xsysg2d.h"


IMPLEMENT_COMPOSITE_UNKNOWN (csGraphics2DOpenGL, XOpenGLGraphicsInfo)

STDMETHODIMP IXOpenGLGraphicsInfo::Open(const char* szTitle)
{
  METHOD_PROLOGUE( csGraphics2DOpenGL, XOpenGLGraphicsInfo);

  pThis->Open(szTitle);
  return S_OK;
}

STDMETHODIMP IXOpenGLGraphicsInfo::Close()
{
  METHOD_PROLOGUE( csGraphics2DOpenGL, XOpenGLGraphicsInfo);

  pThis->Close();
  return S_OK;
}

STDMETHODIMP IXOpenGLGraphicsInfo::SetColorPalette(void)
{
  METHOD_PROLOGUE( csGraphics2DOpenGL, XOpenGLGraphicsInfo);

  return pThis->SetColorPalette();
}
