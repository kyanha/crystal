/*
  Crystal Space Windowing System: Event manager
  Copyright (C) 1998 by Jorrit Tyberghein
  Written by Andrew Zabolotny <bit@eltech.ru>

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

#include "cssysdef.h"
#include "csutil/csevent.h"

SCF_IMPLEMENT_IBASE (csEvent)
  SCF_IMPLEMENTS_INTERFACE (iEvent)
SCF_IMPLEMENT_IBASE_END

csEvent::csEvent ()
{
  SCF_CONSTRUCT_IBASE (NULL);
}

csEvent::csEvent (csTicks iTime,int eType,int kCode,int kChar,int kModifiers)
{
  SCF_CONSTRUCT_IBASE (NULL);
  Time = iTime;
  Type = eType;
  Category = SubCategory = Flags = 0;
  Key.Code = kCode;
  Key.Char = kChar;
  Key.Modifiers = kModifiers;
}

csEvent::csEvent (csTicks iTime, int eType, int mx, int my,
  int mButton, int mModifiers)
{
  SCF_CONSTRUCT_IBASE (NULL);
  Time = iTime;
  Type = eType;
  Category = SubCategory = Flags = 0;
  Mouse.x = mx;
  Mouse.y = my;
  Mouse.Button = mButton;
  Mouse.Modifiers = mModifiers;
}

csEvent::csEvent (csTicks iTime, int eType, int jn, int jx, int jy,
  int jButton, int jModifiers)
{
  SCF_CONSTRUCT_IBASE (NULL);
  Time = iTime;
  Type = eType;
  Category = SubCategory = Flags = 0;
  Joystick.number = jn;
  Joystick.x = jx;
  Joystick.y = jy;
  Joystick.Button = jButton;
  Joystick.Modifiers = jModifiers;
}

csEvent::csEvent (csTicks iTime, int eType, int cCode, void *cInfo)
{
  SCF_CONSTRUCT_IBASE (NULL);
  Time = iTime;
  Type = eType;
  Category = SubCategory = Flags = 0;
  Command.Code = cCode;
  Command.Info = cInfo;
  if (eType == csevBroadcast)
    Flags = CSEF_BROADCAST;
}

csEvent::csEvent (csEvent const& e)
{
  Type = e.Type;
  Category = e.Category;
  SubCategory = e.SubCategory;
  Flags = e.Flags;
  Time = e.Time;

  if ((Type & CSMASK_Keyboard) != 0)
  {
    Key.Code = e.Key.Code;
    Key.Char = e.Key.Char;
    Key.Modifiers = e.Key.Modifiers;
  }
  else if ((Type & CSMASK_Mouse) != 0)
  {
    Mouse.x = e.Mouse.x;
    Mouse.y = e.Mouse.y;
    Mouse.Button = e.Mouse.Button;
    Mouse.Modifiers = e.Mouse.Modifiers;
  }
  else if ((Type & CSMASK_Joystick) != 0)
  {
    Joystick.number = e.Joystick.number;
    Joystick.x = e.Joystick.x;
    Joystick.y = e.Joystick.y;
    Joystick.Button = e.Joystick.Button;
    Joystick.Modifiers = e.Joystick.Modifiers;
  }
  else
  {
    Command.Code = e.Command.Code;
    Command.Info = e.Command.Info;
  }
}

csEvent::~csEvent ()
{
}
