/*
    Alternate Windowing System: timer class
    Copyright (C) 2002 by Norman Kr�mer
    partly copied from csws' csTimer class

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
#include "awstimer.h"
#include "iutil/eventq.h"
#include "iutil/event.h"

SCF_IMPLEMENT_IBASE_EXT(awsTimer)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iEventHandler)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (awsTimer::eiEventHandler)
  SCF_IMPLEMENTS_INTERFACE(iEventHandler)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

const int awsTimer:: signalTick = 0x1;

awsTimer::awsTimer (iObjectRegistry *object_reg, iAwsComponent *comp) :
  awsSource(comp)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiEventHandler);
  this->object_reg = object_reg;
  ehSetup = false;
  bSetup = false;
  stopped = true;
  vc = NULL;
  eq = NULL;
}

awsTimer::~awsTimer ()
{
  if (ehSetup)
  {
    if (!stopped) eq->RemoveListener (&scfiEventHandler);
    eq->DecRef ();
  }

  SCF_DEC_REF (vc);
  printf("destr\n");
}

bool awsTimer::Setup ()
{
  if (!bSetup)
  {
    if (!ehSetup)
    {
      eq = CS_QUERY_REGISTRY (object_reg, iEventQueue);
      ehSetup = (eq != NULL);
    }

    if (!vc) vc = CS_QUERY_REGISTRY (object_reg, iVirtualClock);

    bSetup = ehSetup && (vc != NULL);
  }

  return bSetup;
}

bool awsTimer::SetTimer (csTicks nTicks)
{
  this->nTicks = nTicks;
  return Setup ();
}

bool awsTimer::HandleEvent (iEvent &Event)
{
  if (Event.Type == csevBroadcast && Event.Command.Code == cscmdPreProcess)
  {
    csTicks now = vc->GetCurrentTicks ();
    csTicks delta = now - start;

    if (delta >= nTicks)
    {
      // fire
      Broadcast (signalTick);

      // if we're not too far behind, switch to next pulse

      // otherwise we'll have to jump far to the current time
      start += nTicks;
      if (now - start >= nTicks) start = now;
    }
  }

  return false;
}

void awsTimer::Stop ()
{
  stopped = true;
  if (ehSetup) eq->RemoveListener (&scfiEventHandler);
}

bool awsTimer::Start ()
{
  if (Setup ())
  {
    eq->RegisterListener (&scfiEventHandler, CSMASK_Nothing);
    stopped = false;
    start = vc->GetCurrentTicks ();
  }

  return bSetup;
}

bool awsTimer::IsRunning ()
{
  return !stopped;
}


