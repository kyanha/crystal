/*
    Copyright (C) 2004 by Jorrit Tyberghein
	      (C) 2004 by Frank Richter

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
#undef CS_REF_TRACKER
#include "csutil/callstack.h"
#include "csutil/scf.h"
#include "csutil/scopedmutexlock.h"
#include "csutil/sysfunc.h"
#include "reftrack.h"

SCF_IMPLEMENT_IBASE(csRefTracker)
  SCF_IMPLEMENTS_INTERFACE (iRefTracker)
SCF_IMPLEMENT_IBASE_END


csRefTracker::csRefTracker () : riAlloc(1000)
{
  SCF_CONSTRUCT_IBASE(0);

  (mutex = csMutex::Create ())->IncRef();
}

csRefTracker::~csRefTracker ()
{
  {
    csScopedMutexLock lock (mutex);

    csHash<RefInfo*, void*>::GlobalIterator it (
      trackedRefs.GetIterator ());

    while (it.HasNext ())
    {
      RefInfo* info = it.Next ();
      riAlloc.Free (info);
    }

    for (int i = 0; i < oldData.Length(); i++)
    {
      const OldRefInfo& oldInfo = oldData[i];

      riAlloc.Free (oldInfo.ri);
    }
  }

  mutex->DecRef();
  SCF_DESTRUCT_IBASE();
}

csRefTracker::RefInfo& csRefTracker::GetObjRefInfo (void* obj)
{
  if (aliases.In (obj)) obj = aliases.Get (obj);
  RefInfo* info = trackedRefs.Get (obj);
  if (info == 0)
  {
    info = riAlloc.Alloc();
    trackedRefs.Put (obj, info);
  }
  return *info;
}

void csRefTracker::TrackIncRef (void* object, int refCount)
{
  csScopedMutexLock lock (mutex);

  RefInfo& refInfo = GetObjRefInfo (object);
  RefAction& action = refInfo.actions.GetExtend (refInfo.actions.Length ());
  action.type = Increased;
  action.refCount = refCount;
  action.stack = csCallStackHelper::CreateCallStack (1);
  action.tag = 0;
  refInfo.refCount = refCount + 1;
}

void csRefTracker::TrackDecRef (void* object, int refCount)
{
  csScopedMutexLock lock (mutex);

  RefInfo& refInfo = GetObjRefInfo (object);
  RefAction& action = refInfo.actions.GetExtend (refInfo.actions.Length ());
  action.type = Decreased;
  action.refCount = refCount;
  action.stack = csCallStackHelper::CreateCallStack (1);
  action.tag = 0;
  refInfo.refCount = refCount - 1;
}

void csRefTracker::TrackConstruction (void* object)
{
  csScopedMutexLock lock (mutex);

  /*
    Move the already tracked object to the "old data".
    The new one might just coincidentally be alloced at the same spot.
   */
  if (trackedRefs.In (object))
  {
    OldRefInfo oldInfo = {object, trackedRefs.Get (object)};
    oldData.Push (oldInfo);
    trackedRefs.DeleteAll (object);
  }
  /*
    @@@ It may happen that this pointer was aliased to some other location,
    but the alias hasn't been removed.
   */
  aliases.DeleteAll (object);
  TrackIncRef (object, 0);
}

void csRefTracker::TrackDestruction (void* object, int refCount)
{
  csScopedMutexLock lock (mutex);

  RefInfo& refInfo = GetObjRefInfo (object);
  RefAction& action = refInfo.actions.GetExtend (refInfo.actions.Length ());
  action.type = Destructed;
  action.refCount = refCount;
  action.stack = csCallStackHelper::CreateCallStack (1);
  action.tag = 0;
  refInfo.destructed = true;
}

void csRefTracker::MatchIncRef (void* object, int refCount, void* tag)
{
  csScopedMutexLock lock (mutex);

  RefInfo& refInfo = GetObjRefInfo (object);
  bool foundAction = false;
  int i;
  for (i = refInfo.actions.Length() - 1; i >= 0; i--)
  {
    if (refInfo.actions[i].refCount == refCount)
    {
      if (refInfo.actions[i].tag == 0)
      {
	refInfo.actions[i].tag = tag;
	foundAction = true;
      }
      break;
    }
  }
  if (!foundAction)
  {
    RefAction& action = refInfo.actions.GetExtend (refInfo.actions.Length ());
    action.type = Increased;
    action.refCount = refCount;
    action.stack = csCallStackHelper::CreateCallStack (1);
    action.tag = tag;
    refInfo.refCount = refCount + 1;
  }
}

void csRefTracker::MatchDecRef (void* object, int refCount, void* tag)
{
  csScopedMutexLock lock (mutex);

  RefInfo& refInfo = GetObjRefInfo (object);
  bool foundAction = false;
  int i;
  for (i = refInfo.actions.Length() - 1; i >= 0; i--)
  {
    if (refInfo.actions[i].refCount == refCount)
    {
      if (refInfo.actions[i].tag == 0)
      {
	refInfo.actions[i].tag = tag;
	foundAction = true;
      }
      break;
    }
  }
  if (!foundAction)
  {
    RefAction& action = refInfo.actions.GetExtend (refInfo.actions.Length ());
    action.type = Decreased;
    action.refCount = refCount;
    action.stack = csCallStackHelper::CreateCallStack (1);
    action.tag = tag;
    refInfo.refCount = refCount - 1;
  }
  if (refCount == 0)
  {
    /*
      Move the tracked object to the "old data". A new one might just 
      coincidentally be alloced at the same spot.
    */
    if (trackedRefs.In (object))
    {
      OldRefInfo oldInfo = {object, trackedRefs.Get (object)};
      oldData.Push (oldInfo);
      trackedRefs.DeleteAll (object);
    }
  }
}

void csRefTracker::AddAlias (void* obj, void* mapTo)
{
  csScopedMutexLock lock (mutex);

  if (obj == mapTo) return;
  aliases.PutFirst (obj, mapTo);
}

void csRefTracker::RemoveAlias (void* obj, void* mapTo)
{
  csScopedMutexLock lock (mutex);

  if (obj == mapTo) return;
  aliases.Delete (obj, mapTo);
}

void csRefTracker::SetDescription (void* obj, const char* description)
{
  csScopedMutexLock lock (mutex);

  RefInfo& refInfo = GetObjRefInfo (obj);
  if (refInfo.descr == 0) refInfo.descr = description;
}

void csRefTracker::ReportOnObj (void* obj, RefInfo* info)
{
  bool okay = (info->refCount == 0) ||
    (info->destructed && (info->refCount <= 1));
  if (!okay)
  {
    csPrintf ("object %.8x (%s), refcount %d, %s\n",
      (uint32)obj, 
      info->descr ? info->descr : "<unknown>",
      info->refCount,
      info->destructed ? "destructed" : "not destructed");
    for (int i = 0; i < info->actions.Length(); i++)
    {
      csPrintf ("%s by %.8x from %d\n",
	(info->actions[i].type == Increased) ? "Increase" : "Decrease",
	(uint32)info->actions[i].tag,
	info->actions[i].refCount);
      if (info->actions[i].stack != 0)
	info->actions[i].stack->Print ();
    }
    csPrintf ("\n");
  }
}

void csRefTracker::Report ()
{
  csScopedMutexLock lock (mutex);

  for (int i = 0; i < oldData.Length(); i++)
  {
    const OldRefInfo& oldInfo = oldData[i];

    ReportOnObj (oldInfo.obj, oldInfo.ri);
  }

  csHash<RefInfo*, void*>::GlobalIterator it (
    trackedRefs.GetIterator ());

  while (it.HasNext ())
  {
    void* obj;
    RefInfo* info = it.Next (obj);

    ReportOnObj (obj, info);
  }
}
