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

#ifndef __UNIX_H__
#define __UNIX_H__

#include "csutil/scf.h"
#include "cssys/csinput.h"
#include "cssys/system.h"
#include "igraph2d.h"

/// Unix version.
class SysSystemDriver : public csSystemDriver
{
public:
  // Constructor
  SysSystemDriver ();

  // Main event loop
  virtual void Loop ();
  // Display system-specific help
  virtual void Help ();
  // Sleep for given number of 1/1000 seconds
  virtual void Sleep (int SleepTime);

  /// Toggle console text output (for consoles that share text/graphics mode)
  virtual void EnablePrintf (bool /*iEnable*/) { }
};

#endif // __UNIX_H__
