/*
    Copyright (C) 1998-2000 by Jorrit Tyberghein
    Copyright (C) 2001 by W.C.A Wijngaards

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

#ifndef BUMPTEST_H
#define BUMPTEST_H

#include <stdarg.h>
#include "cssys/sysdriv.h"
#include "csgeom/math2d.h"
#include "csgeom/math3d.h"

struct iMeshFactoryWrapper;
struct iMeshWrapper;
struct iLoader;
struct iLight;
struct iSector;
struct iView;
struct iEngine;
struct iDynLight;
struct iMaterialWrapper;
class csProcBump;

class BumpTest : public SysSystemDriver
{
  typedef SysSystemDriver superclass;
private:
  iSector* room;
  iView* view;
  iEngine* engine;
  iDynLight* dynlight;
  iLight *bumplight;
  iMaterialWrapper* matBump;
  iLoader *LevelLoader;
  iGraphics3D *myG3D;

  float animli;
  bool going_right;
  csProcBump* prBump;

  bool InitProcDemo();

public:
  BumpTest ();
  virtual ~BumpTest ();

  virtual bool Initialize (int argc, const char* const argv[],
    const char *iConfigName);
  virtual void NextFrame ();
  virtual bool HandleEvent (iEvent &Event);
};

#endif // BUMPTEST_H

