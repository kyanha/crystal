/*
    Event outlet object implementation
    Copyright (C) 2000 by Andrew Zabolotny

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
#include "cssys/system.h"
#include "cssys/csevent.h"

IMPLEMENT_IBASE (csSystemDriver::csEventOutlet)
  IMPLEMENTS_INTERFACE (iEventOutlet)
IMPLEMENT_IBASE_END

csSystemDriver::csEventOutlet::csEventOutlet (iEventPlug *iPlug, csSystemDriver *iSys)
{
  CONSTRUCT_IBASE (NULL);
  Plug = iPlug;
  EnableMask = unsigned (-1);
  System = iSys;
}

csSystemDriver::csEventOutlet::~csEventOutlet ()
{
  int idx = System->EventOutlets.Find (this);
  if (idx >= 0)
  {
    System->EventOutlets [idx] = NULL;
    System->EventOutlets.Delete (idx);
  }
}

iEvent *csSystemDriver::csEventOutlet::CreateEvent ()
{
  return new csEvent ();
}

void csSystemDriver::csEventOutlet::PutEvent (iEvent *Event)
{
  if ((1 << Event->Type) & EnableMask)
    System->EventQueue.Put (Event);
  else
    delete Event;
}

void csSystemDriver::csEventOutlet::Key (int iKey, int iChar, bool iDown)
{
  if ((iKey || iChar) && (EnableMask & CSEVTYPE_Keyboard))
    System->Keyboard.do_key (iKey, iChar, iDown);
}

void csSystemDriver::csEventOutlet::Mouse (int iButton, bool iDown, int x, int y)
{
  if (EnableMask & CSEVTYPE_Mouse)
    if (iButton == 0)
      System->Mouse.do_motion (x, y);
    else
      System->Mouse.do_button (iButton, iDown, x, y);
}

void csSystemDriver::csEventOutlet::Joystick (int iNumber, int iButton,
  bool iDown, int x, int y)
{
  if (EnableMask & CSEVTYPE_Joystick)
    if (iButton == 0)
      System->Joystick.do_motion (iNumber, x, y);
    else
      System->Joystick.do_button (iNumber, iButton, iDown, x, y);
}

void csSystemDriver::csEventOutlet::Broadcast (int iCode, void *iInfo)
{
  System->EventQueue.Put (new csEvent (System->Time (), csevBroadcast,
    iCode, iInfo));
}

void csSystemDriver::csEventOutlet::ImmediateBroadcast (int iCode, void *iInfo)
{
  csEvent Event (System->Time (), csevBroadcast, iCode, iInfo);
  System->HandleEvent (Event);
}
