/*
    Copyright (C) 2001 by Jorrit Tyberghein

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

#ifndef __VIEWMESH_H__
#define __VIEWMESH_H__

#include <stdarg.h>

#include "csws/csws.h"
#include "csutil/csstrvec.h"

struct iEngine;
struct iLoader;
struct iGraphics3D;
struct iEvent;
struct iMeshWrapper;
struct iKeyboardDriver;
struct iObjectRegistry;
struct iVirtualClock;
struct iSector;
struct iView;

class ViewMesh : public csApp
{
private:
  iEngine* engine;
  iLoader* loader;
  iGraphics3D* g3d;
  iSector* room;
  iView* view;
  iMeshWrapper *sprite;
  csVector3 spritepos;

  csMenu *menu;
  csWindow *dialog;
  csStrVector stateslist;
  enum { movenormal, moveorigin, rotateorigin } cammode;
public:
  ViewMesh (iObjectRegistry *object_reg, csSkin &Skin);
  virtual ~ViewMesh ();

  static void Help ();
  bool LoadSprite (const char *filename,float scale);
  void ConstructMenu();

  virtual bool Initialize ();
  virtual bool HandleEvent (iEvent&);
  virtual void Draw();
};

#endif // __VIEWMESH_H__

