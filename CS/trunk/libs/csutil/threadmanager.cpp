/*
  Copyright (C) 2008 by Michael Gist

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
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
#include "csutil/platform.h"
#include "csutil/sysfunc.h"
#include "csutil/threadmanager.h"
#include "iengine/engine.h"
#include "iutil/cfgmgr.h"

using namespace CS::Threading;

ThreadID csThreadManager::tid = Thread::GetThreadID();

csThreadManager::csThreadManager(iObjectRegistry* objReg) : scfImplementationType(this), 
  waiting(0), alwaysRunNow(false), objectReg(objReg), exiting(false)
{
  threadCount = CS::Platform::GetProcessorCount();

  // If we can't detect, assume we have one.
  if(threadCount == 0)
  {
    csFPrintf(stderr, "Processor count couldn't be detected!\n");
    threadCount = 1;
  }

  // Have 'processor count' extra processing threads.
  threadQueue.AttachNew(new ThreadedJobQueue(threadCount));
  listQueue.AttachNew(new ListAccessQueue());

  // Event handler.
  tMEventHandler.AttachNew(new TMEventHandler(this));

  eventQueue = csQueryRegistry<iEventQueue>(objectReg);
  if(eventQueue.IsValid())
  {
    ProcessPerFrame = csevFrame(objReg);
    ProcessWhileWait = csevThreadWait(objReg);
    eventQueue->RegisterListener(tMEventHandler, ProcessPerFrame);
    eventQueue->RegisterListener(tMEventHandler, ProcessWhileWait);
  }
}

csThreadManager::~csThreadManager()
{
  exiting = true;
  eventQueue->RemoveListener(tMEventHandler);
}

void csThreadManager::Init(iConfigManager* config)
{
  int32 oldCount = threadCount;
  threadCount = config->GetInt("ThreadManager.Threads", threadCount);
  if(oldCount != threadCount)
  {
    threadQueue.AttachNew(new ThreadedJobQueue(threadCount));
  }

  alwaysRunNow = config->GetBool("ThreadManager.AlwaysRunNow");
}

void csThreadManager::Process(uint num)
{
  listQueue->ProcessQueue(num);  
}

void csThreadManager::Wait(csRef<iThreadReturn> result)
{
  if(!result->IsFinished())
  {
    if(!IsMainThread())
    {
      AtomicOperations::Increment(&waiting);
    }

    while(!result->IsFinished())
    {
      if(IsMainThread())
      {
        csRef<iEvent> evt = eventQueue->CreateBroadcastEvent(ProcessWhileWait);
        eventQueue->Dispatch(*evt);
        csSleep(1);
      }
    }

    if(!IsMainThread())
    {
      AtomicOperations::Decrement(&waiting);
    }
  }
}

bool csThreadManager::Wait(csRefArray<iThreadReturn>& threadReturns)
{
  bool success = true;

  while(threadReturns.GetSize() != 0)
  {
    if(threadReturns[0]->IsFinished())
    {
      success &= threadReturns[0]->WasSuccessful();
      threadReturns.DeleteIndexFast(0);
    }
    else
    {
      threadQueue->PopAndRun();
    }
  }

  return success;
}
