/*
    Copyright (C) 1998,1999 by Jorrit Tyberghein
    Written by Xavier Planet.
    Overhauled and re-engineered by Eric Sunshine <sunshine@sunshineco.com>
  
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

#ifndef __CS_CSBE_H__
#define __CS_CSBE_H__

#include "csutil/scf.h"
#include "def.h"
#include "cssys/csinput.h"
#include "cssys/system.h"
#include "cssys/be/icsbe.h"
class CrystApp;

class SysSystemDriver : public csSystemDriver, public iBeLibSystemDriver
{
protected:
  bool running;
  CrystApp* app;

public:
  SysSystemDriver ();

  DECLARE_IBASE_EXT (csSystemDriver);

  // Main event loop
  virtual void Loop ();
  long LoopThread ();

  /// Implementation of iBeLibSystemDriver
  void ProcessUserEvent (BMessage*);
  bool SetMouseCursor (csMouseCursorID shape);
};

#endif // __CS_CSBE_H__
