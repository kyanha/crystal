/*
    Crystal Space Windowing System: keyboard accelerator class
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

#include <stdio.h>
#include "sysdef.h"
#include "csws/cscomp.h"
#include "csws/cskeyacc.h"
#include "csws/csapp.h"

struct csAccElement
{
  int Key;
  int Shifts;
  csEvent Event;
};

csKeyboardAccelerator::csAccVector::csAccVector () : csVector (16, 16)
{
}

csKeyboardAccelerator::csAccVector::~csAccVector ()
{
  DeleteAll ();
}

bool csKeyboardAccelerator::csAccVector::FreeItem (csSome Item)
{
  delete (csAccElement *)Item;
  return true;
}

csKeyboardAccelerator::csKeyboardAccelerator (csComponent *iParent)
  : csComponent (iParent)
{
}

csKeyboardAccelerator::~csKeyboardAccelerator ()
{
}

void csKeyboardAccelerator::Event (int iKey, int iShifts, csEvent &iEvent)
{
  CHK (csAccElement *ae = new csAccElement);
  ae->Key = iKey;
  ae->Shifts = iShifts;
  ae->Event = iEvent;
  Accelerators.Push (ae);
}

void csKeyboardAccelerator::Command (int iKey, int iShifts, int iCommand,
  void *iInfo)
{
  csEvent ev (0, csevCommand, iCommand, iInfo);
  Event (iKey, iShifts, ev);
}

void csKeyboardAccelerator::Broadcast (int iKey, int iShifts, int iCommand,
  void *iInfo)
{
  csEvent ev (0, csevBroadcast, iCommand, iInfo);
  Event (iKey, iShifts, ev);
}

bool csKeyboardAccelerator::PostHandleEvent (csEvent &Event)
{
  if (csComponent::PostHandleEvent (Event))
    return true;

  if ((Event.Type == csevKeyDown)
   && (app->FocusOwner == NULL)
   && (app->KeyboardOwner == NULL))
  {
    for (int i = Accelerators.Length () - 1; i >= 0; i--)
    {
      csAccElement *ae = (csAccElement *)Accelerators [i];
      if ((ae->Key == Event.Key.Code)
       && (ae->Shifts == (Event.Key.Modifiers & CSMASK_ALLSHIFTS)))
      {
        CHK (csEvent *ev = new csEvent (ae->Event));
        app->PutEvent (ev);
        return true;
      } /* endif */
    } /* endfor */
  } /* endif */
  return false;
}
