/*
    Copyright (C) 2003 by Jorrit Tyberghein

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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "cssysdef.h"
#include "csutil/timer.h"
#include "iutil/event.h"
#include "iutil/evdefs.h"
#include "iutil/eventh.h"
#include "iutil/eventq.h"
#include "iutil/objreg.h"
#include "iutil/virtclk.h"

struct timerevent
{
  csRef<iTimerEvent> event;
  csTicks delay;
  int time_left;
};

//-----------------------------------------------------------------------

class csTimerEventHandler : public iEventHandler
{
private:
  csEventTimer* timer;

public:
  csTimerEventHandler (csEventTimer* timer)
  {
    SCF_CONSTRUCT_IBASE (0);
    csTimerEventHandler::timer = timer;
  }
  SCF_DECLARE_IBASE;
  virtual bool HandleEvent (iEvent& e)
  {
    return timer->HandleEvent (e);
  }
};

SCF_IMPLEMENT_IBASE (csTimerEventHandler)
  SCF_IMPLEMENTS_INTERFACE (iEventHandler)
SCF_IMPLEMENT_IBASE_END

//-----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csEventTimer)
  SCF_IMPLEMENTS_INTERFACE (iEventTimer)
SCF_IMPLEMENT_IBASE_END

csEventTimer::csEventTimer (iObjectRegistry* object_reg)
{
  SCF_CONSTRUCT_IBASE (0);
  csRef<iEventQueue> q = CS_QUERY_REGISTRY (object_reg, iEventQueue);
  CS_ASSERT (q != 0);
  if (q != 0)
  {
    handler = new csTimerEventHandler (this);
    q->RegisterListener (handler, CSMASK_Nothing);
    handler->DecRef ();
  }
  else
  {
    handler = 0;
  }
  vc = CS_QUERY_REGISTRY (object_reg, iVirtualClock);
  CS_ASSERT (vc != 0);

  minimum_time = 2000000000;
  accumulate_elapsed = 0;
}

csEventTimer::~csEventTimer ()
{
  if (handler)
  {
    // Jorrit: if I enable this then I get a crash at exit because
    // the object registry is already being destructed. No idea how I
    // should solve this...
    // @@@@@
    //csRef<iEventQueue> q = CS_QUERY_REGISTRY (object_reg, iEventQueue);
    //if (q != 0)
      //q->RemoveListener (handler);
  }
}

bool csEventTimer::HandleEvent (iEvent& event)
{
  if (event.Type != csevBroadcast
   || event.Command.Code != cscmdFinalProcess)
    return false;

  csTicks elapsed = vc->GetElapsedTicks ();
  minimum_time -= elapsed;

  if (minimum_time > 0)
  {
    accumulate_elapsed += elapsed;
    return true;
  }

  minimum_time = 2000000000;
  int i;
  for (i = 0 ; i < timerevents.Length () ; i++)
  {
    timerevent& te = timerevents[i];
    te.time_left -= elapsed+accumulate_elapsed;
    if (te.time_left <= 0)
    {
      if (te.event->Perform (te.event))
      {
        te.time_left = te.delay;
	if (te.time_left < minimum_time) minimum_time = te.time_left;
      }
      else
      {
        timerevents.DeleteIndex (i);
	i--;
      }
    }
    else
    {
      if (te.time_left < minimum_time) minimum_time = te.time_left;
    }
  }

  return true;
}

int csEventTimer::FindTimerEvent (iTimerEvent* ev)
{
  int i;
  for (i = 0 ; i < timerevents.Length () ; i++)
  {
    if (timerevents[i].event == ev)
      return i;
  }
  return -1;
}

void csEventTimer::AddTimerEvent (iTimerEvent* ev, csTicks delay)
{
  timerevent te;
  te.event = ev;
  te.delay = delay;
  te.time_left = delay;
  timerevents.Push (te);
  if (delay < (csTicks)minimum_time) minimum_time = delay;
}

void csEventTimer::RemoveTimerEvent (iTimerEvent* ev)
{
  int idx = FindTimerEvent (ev);
  if (idx != -1)
    timerevents.DeleteIndex (idx);
}

void csEventTimer::RemoveAllTimerEvents ()
{
  timerevents.DeleteAll ();
  minimum_time = 2000000000;
  accumulate_elapsed = 0;
}

csPtr<iEventTimer> csEventTimer::GetStandardTimer (iObjectRegistry* object_reg)
{
  csRef<iEventTimer> timer = CS_QUERY_REGISTRY_TAG_INTERFACE (object_reg,
  	"crystalspace.timer.standard", iEventTimer);
  if (!timer)
  {
    timer = csPtr<iEventTimer> (new csEventTimer (object_reg));
    object_reg->Register (timer, "crystalspace.timer.standard");
  }
  return csPtr<iEventTimer> (timer);
}

