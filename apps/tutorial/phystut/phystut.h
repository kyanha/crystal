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

#ifndef __SIMPLE1_H__
#define __SIMPLE1_H__

#include <stdarg.h>
#include "csgeom/vector3.h"
#include "csutil/csvector.h"

struct iEngine;
struct iLoader;
struct iGraphics3D;
struct iKeyboardDriver;
struct iVirtualClock;
struct iObjectRegistry;
struct iEvent;
struct iSector;
struct iView;
struct iDynamics;
struct iDynamicSystem;
struct iRigidBody;
struct iJoint;
struct iMeshFactoryWrapper;

class Simple
{
private:
  iObjectRegistry* object_reg;
  csRef<iEngine> engine;
  csRef<iLoader> loader;
  csRef<iGraphics3D> g3d;
  csRef<iKeyboardDriver> kbd;
  csRef<iVirtualClock> vc;
  iSector* room;
  csRef<iView> view;

  csRef<iDynamics> dyn;
  csRef<iDynamicSystem> dynSys;
  csRef<iMeshFactoryWrapper> boxFact;
  csRef<iMeshFactoryWrapper> ballFact;

  static bool SimpleEventHandler (iEvent& ev);
  bool HandleEvent (iEvent& ev);
  void SetupFrame ();
  void FinishFrame ();

  iRigidBody* CreateBox (void);
  iRigidBody* CreateSphere (void);
  iJoint* CreateJointed (void);
  iRigidBody* CreateRoomSolids (const csVector3& center, const csVector3& radius, float thickness);

public:
  Simple (iObjectRegistry *obj);
  ~Simple ();

  bool Initialize ();
  void Start ();
};

#endif // __SIMPLE1_H__

