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

#ifndef __SIMPLEVP_H__
#define __SIMPLEVP_H__

#include <stdarg.h>
#include "csutil/ref.h"

struct iEngine;
struct iLoader;
struct iGraphics3D;
struct iKeyboardDriver;
struct iVirtualClock;
struct iObjectRegistry;
struct iEvent;
struct iSector;
struct iView;

struct iEffectServer;
struct iEffectDefinition;
struct iEffectTechnique;
struct iEffectPass;
struct iEffectLayer;

class Simple
{
private:
  iObjectRegistry* object_reg;
  csRef<iEngine> engine;
  csRef<iLoader> loader;
  csRef<iGraphics3D> g3d;
  csRef<iKeyboardDriver> kbd;
  csRef<iVirtualClock> vc;
  csRef<iView> view;
  iSector* room;

	//effectserver variables
	iEffectServer* efserver;
  //csRef<iEffectServer> efserver;
  iEffectDefinition* effect;
  iEffectTechnique* technique;
  iEffectPass* pass1;
	iEffectLayer* layer1;

  static bool SimpleEventHandler (iEvent& ev);
  bool HandleEvent (iEvent& ev);
  void SetupFrame ();
  void FinishFrame ();

public:
  Simple (iObjectRegistry* object_reg);
  ~Simple ();

  bool Initialize ();
  void Start ();
};

#endif // __SIMPLEVP_H__

