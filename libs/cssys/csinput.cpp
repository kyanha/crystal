/*
    Crystal Space input library
    Copyright (C) 1998,2000 by Jorrit Tyberghein
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

#include "sysdef.h"
#include "cssys/system.h"
#include "cssys/csinput.h"
#include "csutil/inifile.h"
#include "isystem.h"

// This array defines first 32..128 character codes with SHIFT key applied
char ShiftedKey [128-32] =
{
' ', '!', '"', '#', '$', '%', '&', '"', '(', ')', '*', '+', '<', '_', '>', '?',
')', '!', '@', '#', '$', '%', '^', '&', '*', '(', ':', ':', '<', '+', '>', '?',
'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '{', '|', '}', '^', '_',
'~', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '{', '|', '}', '~', 127
};

// And this one performs backward conversion
char UnshiftedKey [128-32] =
{
' ', '1', '\'','3', '4', '5', '7', '\'','9', '0', '8', '=', ',', '-', '.', '/',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ';', ';', ',', '=', '.', '/',
'2', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '[', '\\',']', '6', '-',
'`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '[', '\\',']', '`', 127
};

//--//--//--//--//--//--//--//--//--//--//--//--//--/> Keyboard driver <--//--/

csKeyboardDriver::csKeyboardDriver (csSystemDriver *system) :
  KeyState (256 + (CSKEY_LAST - CSKEY_FIRST + 1))
{
  System = system;
  KeyState.Reset ();
}

void csKeyboardDriver::Reset ()
{
  for (size_t i = 0; i < KeyState.GetBitCount (); i++)
    if (KeyState [i])
      do_key (i < 256 ? i : i - 256 + CSKEY_FIRST, false);
}

void csKeyboardDriver::do_key (int key, bool down)
{
  int smask = (GetKeyState (CSKEY_SHIFT) ? CSMASK_SHIFT : 0)
            | (GetKeyState (CSKEY_CTRL) ? CSMASK_CTRL : 0)
            | (GetKeyState (CSKEY_ALT) ? CSMASK_ALT : 0);

  if (down && !GetKeyState (key))
    smask |= CSMASK_FIRST;

  if ((key >= 32) && (key < 128))
    if (smask & CSMASK_SHIFT)
      key = ShiftedKey [key - 32];

  SetKeyState (key, down);
  System->EventQueue.Put (new csEvent (System->GetTime (),
    down ? csevKeyDown : csevKeyUp, key, smask));
}

void csKeyboardDriver::do_key_extended (int key, int keytranslated, bool down)
{
  int smask = (GetKeyState (CSKEY_SHIFT) ? CSMASK_SHIFT : 0)
            | (GetKeyState (CSKEY_CTRL) ? CSMASK_CTRL : 0)
            | (GetKeyState (CSKEY_ALT) ? CSMASK_ALT : 0);

  if (down && !GetKeyState (key))
    smask |= CSMASK_FIRST;

  SetKeyState (key, down);
  System->EventQueue.Put (new csEvent (System->GetTime (),
    down ? csevKeyDown : csevKeyUp, keytranslated, smask));
}

void csKeyboardDriver::SetKeyState (int key, bool state)
{
  int idx = (key < 256) ? key : (256 + key - CSKEY_FIRST);
  if (state) KeyState.Set (idx); else KeyState.Reset (idx);
}

bool csKeyboardDriver::GetKeyState (int key)
{
  int idx = (key < 256) ? key : (256 + key - CSKEY_FIRST);
  return KeyState [idx];
}

//--//--//--//--//--//--//--//--//--//--//--//--//--//--> Mouse driver <--//--/

time_t csMouseDriver::DoubleClickTime;
size_t csMouseDriver::DoubleClickDist;

csMouseDriver::csMouseDriver (csSystemDriver *system)
{
  System = system;
  LastX = LastY = 0;
  memset (&Button, 0, sizeof (Button));
}

void csMouseDriver::Reset ()
{
  for (int i = 0; i < CS_MAX_MOUSE_BUTTONS; i++)
    if (Button[i])
      do_button (i + 1, false, LastX, LastY);
  LastClickButton = -1;
}

void csMouseDriver::do_button (int button, bool down, int x, int y)
{
  if (x != LastX || y != LastY)
    do_motion (x, y);

  if (button == 0 || button >= CS_MAX_MOUSE_BUTTONS)
    return;

  int smask = (System->GetKeyState (CSKEY_SHIFT) ? CSMASK_SHIFT : 0)
            | (System->GetKeyState (CSKEY_ALT)   ? CSMASK_ALT   : 0)
            | (System->GetKeyState (CSKEY_CTRL)  ? CSMASK_CTRL  : 0);

  Button [button - 1] = down;

  time_t evtime = System->GetTime ();
  int ev = down ? csevMouseDown : csevMouseUp;

  if (down)
  {
    if ((button == LastClickButton)
     && (evtime - LastClickTime <= DoubleClickTime)
     && (unsigned (ABS (x - LastClickX)) <= DoubleClickDist)
     && (unsigned (ABS (y - LastClickY)) <= DoubleClickDist))
      ev = csevMouseDoubleClick;

    LastClickButton = button;
    LastClickTime = evtime;
    LastClickX = x;
    LastClickY = y;
  }

  System->EventQueue.Put (new csEvent (evtime, ev, x, y, button, smask));
}

void csMouseDriver::do_motion (int x, int y)
{
  if (x != LastX || y != LastY)
  {
    int smask = (System->GetKeyState (CSKEY_SHIFT) ? CSMASK_SHIFT : 0)
              | (System->GetKeyState (CSKEY_ALT)   ? CSMASK_ALT   : 0)
              | (System->GetKeyState (CSKEY_CTRL)  ? CSMASK_CTRL  : 0);

    LastX = x;
    LastY = y;

    System->EventQueue.Put (new csEvent (System->GetTime (), csevMouseMove,
      x, y, 0, smask));
  }
}

void csMouseDriver::SetDoubleClickTime (int iTime, size_t iDist)
{
  DoubleClickTime = iTime;
  DoubleClickDist = iDist;
}

//--//--//--//--//--//--//--//--//--//--//--//--//--/> Joystick driver <--//--/

csJoystickDriver::csJoystickDriver (csSystemDriver *system)
{
  System = system;
  memset (&Button, 0, sizeof (Button));
  memset (&LastX, sizeof (LastX), 0);
  memset (&LastY, sizeof (LastY), 0);
}

void csJoystickDriver::Reset ()
{
  for (int i = 0; i < CS_MAX_JOYSTICK_COUNT; i++)
    for (int j = 0; j < CS_MAX_JOYSTICK_BUTTONS; j++)
      if (Button [i][j])
        do_button (i + 1, j + 1, false, LastX [i], LastY [i]);
}

void csJoystickDriver::do_button (int number, int button, bool down,
  int x, int y)
{
  if (number <= 0 && number > CS_MAX_JOYSTICK_COUNT)
    return;

  if (x != LastX [number - 1] || y != LastY [number - 1])
    do_motion (number, x, y);

  if (button <= 0 || button > CS_MAX_JOYSTICK_BUTTONS)
    return;

  int smask = (System->GetKeyState (CSKEY_SHIFT) ? CSMASK_SHIFT : 0)
            | (System->GetKeyState (CSKEY_ALT)   ? CSMASK_ALT   : 0)
            | (System->GetKeyState (CSKEY_CTRL)  ? CSMASK_CTRL  : 0);

  Button [number - 1] [button - 1] = down;
  System->EventQueue.Put (new csEvent (System->GetTime (),
    down ? csevJoystickDown : csevJoystickUp, number, x, y, button, smask));
}

void csJoystickDriver::do_motion (int number, int x, int y)
{
  if (number <= 0 && number > CS_MAX_JOYSTICK_COUNT)
    return;

  if (x != LastX [number - 1] || y != LastY [number - 1])
  {
    int smask = (System->GetKeyState (CSKEY_SHIFT) ? CSMASK_SHIFT : 0)
              | (System->GetKeyState (CSKEY_ALT)   ? CSMASK_ALT   : 0)
              | (System->GetKeyState (CSKEY_CTRL)  ? CSMASK_CTRL  : 0);

    LastX [number - 1] = x;
    LastY [number - 1] = y;

    System->EventQueue.Put (new csEvent (System->GetTime (), csevJoystickMove,
      number, x, y, 0, smask));
  }
}
