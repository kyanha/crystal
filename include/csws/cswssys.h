/*
    Crystal Space Windowing System: Miscelaneous CSWS utilites
    Copyright (C) 1998,1999 by Andrew Zabolotny <bit@eltech.ru>

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

#ifndef __CSWSSYS_H__
#define __CSWSSYS_H__

#include "cssys/sysdriv.h"
#include "cssys/system.h"
#include "csws/csapp.h"

// A special variation of SysSystemDriver for CSWS
class cswsSystemDriver : public SysSystemDriver
{
  friend void cleanup ();

  int curline, maxlines, maxwidth;
  char **textline;
  int *linecolor;
  int textcolor;
  csApp *application;

public:
  /// Initialize the application's system driver given application object
  cswsSystemDriver (csApp *ParentApp);
  /// Destroy the system driver object
  virtual ~cswsSystemDriver ();
  /// Initialize system driver and debug console
  virtual bool Initialize (int argc, char *argv[],
    const char *iConfigName, const char *iVfsConfigName, IConfig *config);
  /// Call application to process queued events
  virtual void NextFrame (long elapsed_time, long current_time);
  /// Replace DemoWrite() for output to debug console
  virtual void DemoWrite (const char *buf);
  /// Display an alert message
  virtual void Alert (const char *msg);
  /// Display a warning message
  virtual void Warn (const char *msg);
};

#endif // __CSWSSYS_H__
