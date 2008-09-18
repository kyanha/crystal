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

#ifndef __CS_CSUTIL_THREADMANAGER_H__
#define __CS_CSUTIL_THREADMANAGER_H__

#include "csutil/eventhandlers.h"
#include "csutil/listaccessqueue.h"
#include "csutil/objreg.h"
#include "csutil/threadevent.h"
#include "csutil/threadjobqueue.h"
#include "iengine/engine.h"
#include "imap/loader.h"
#include "iutil/event.h"
#include "iutil/eventh.h"
#include "iutil/eventq.h"
#include "iutil/threadmanager.h"

struct iEvent;

using namespace CS::Threading;

class CS_CRYSTALSPACE_EXPORT csThreadManager : public scfImplementation1<csThreadManager,
  iThreadManager>
{
public:
  csThreadManager(iObjectRegistry* objReg);
  virtual ~csThreadManager();

  void Process(uint num = 1);
  void Wait(csRef<iThreadReturn> ret);

  inline void PushToQueue(QueueType queueType, iJob* job)
  {
    if(queueType == THREADED)
    {
      MutexScopedLock lock(queuePushLock);
      threadQueue->Enqueue(job);
    }
    else
    {
      listQueue->Enqueue(job, queueType == HIGH);
    }
  }

  inline bool RunNow(QueueType queueType, bool forceQueue)
  {
    return (IsMainThread() && queueType != THREADED && !forceQueue) ||
      (queueType == THREADED && (waiting >= threadCount-1 || threadQueue->GetQueueCount() > 2*threadCount-1));
  }

protected:
  csEventID ProcessPerFrame;
  csEventID ProcessWhileWait;

private:

  static ThreadID tid;

  inline bool IsMainThread()
  {
    return tid == Thread::GetThreadID();
  }

  int32 waiting;
  int32 threadCount;

  Mutex queuePushLock;

  iObjectRegistry* objectReg;
  csRef<ThreadedJobQueue> threadQueue;
  csRef<ListAccessQueue> listQueue;
  csRef<iEventQueue> eventQueue;
  csTicks waitingTime;

  class TMEventHandler : public scfImplementation1<TMEventHandler, 
      iEventHandler>
  {
  public:
    TMEventHandler(csThreadManager* parent) :
        scfImplementationType (this), parent (parent)
    {
    }
    
    virtual ~TMEventHandler()
    {
    }

    bool HandleEvent(iEvent& Event)
    {
      if(Event.Name == parent->ProcessPerFrame ||
         Event.Name == parent->ProcessWhileWait)
      {
        parent->Process(5);
      }
      return false;
    }

    CS_EVENTHANDLER_PHASE_LOGIC("crystalspace.threadmanager")

  private:
    csThreadManager* parent;
  };
  csRef<iEventHandler> tMEventHandler;
};

class csThreadReturn : public iThreadReturn
{
public:
  csThreadReturn(iThreadManager* tm) : tm(tm)
  {
    finished = false;
    result = NULL;
  }

  bool IsFinished() { return finished; }
  bool WasSuccessful() { return true; }
  void* GetResultPtr() { return result; }
  csRef<iBase> GetResultRefPtr() { return refResult; }

  void MarkFinished() { finished = true; }
  void MarkSuccessful() { return; }
  void SetResult(void* result) { this->result = result; }
  void SetResult(csRef<iBase> result) { refResult = result; }

  void Copy(iThreadReturn* other)
  {
    result = other->GetResultPtr();
    refResult = other->GetResultRefPtr();
    finished = other->IsFinished();
  }

  void Wait()
  {
    if(tm.IsValid())
    {
      tm->Wait(this);
    }
  }

private:
  bool finished;
  void* result;
  csRef<iBase> refResult;
  csRef<iThreadManager> tm;
};

template<class T, typename A1>
void QueueEvent(csRef<iThreadManager> tm, ThreadedCallable<T>* object, bool (T::*method)(A1), void const** &argsTC, QueueType queueType)
{
  csRef<ThreadEvent1<T, A1> > threadEvent;
  threadEvent.AttachNew(new ThreadEvent1<T, A1>(object, method, argsTC));
  tm->PushToQueue(queueType, threadEvent);
}

template<class T, typename A1, typename A2>
void QueueEvent(csRef<iThreadManager> tm, ThreadedCallable<T>* object, bool (T::*method)(A1, A2), void const** &argsTC, QueueType queueType)
{
  csRef<ThreadEvent2<T, A1, A2> > threadEvent;
  threadEvent.AttachNew(new ThreadEvent2<T, A1, A2>(object, method, argsTC));
  tm->PushToQueue(queueType, threadEvent);
}

template<class T, typename A1, typename A2, typename A3>
void QueueEvent(csRef<iThreadManager> tm, ThreadedCallable<T>* object, bool (T::*method)(A1, A2, A3), void const** &argsTC, QueueType queueType)
{
  csRef<ThreadEvent3<T, A1, A2, A3> > threadEvent;
  threadEvent.AttachNew(new ThreadEvent3<T, A1, A2, A3>(object, method, argsTC));
  tm->PushToQueue(queueType, threadEvent);
}

template<class T, typename A1, typename A2, typename A3, typename A4>
void QueueEvent(csRef<iThreadManager> tm, ThreadedCallable<T>* object, bool (T::*method)(A1, A2, A3, A4), void const** &argsTC, QueueType queueType)
{
  csRef<ThreadEvent4<T, A1, A2, A3, A4> > threadEvent;
  threadEvent.AttachNew(new ThreadEvent4<T, A1, A2, A3, A4>(object, method, argsTC));
  tm->PushToQueue(queueType, threadEvent);
}

template<class T, typename A1, typename A2, typename A3, typename A4, typename A5>
void QueueEvent(csRef<iThreadManager> tm, ThreadedCallable<T>* object, bool (T::*method)(A1, A2, A3, A4, A5), void const** &argsTC, QueueType queueType)
{
  csRef<ThreadEvent5<T, A1, A2, A3, A4, A5> > threadEvent;
  threadEvent.AttachNew(new ThreadEvent5<T, A1, A2, A3, A4, A5>(object, method, argsTC));
  tm->PushToQueue(queueType, threadEvent);
}

template<class T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
void QueueEvent(csRef<iThreadManager> tm, ThreadedCallable<T>* object, bool (T::*method)(A1, A2, A3, A4, A5, A6), void const** &argsTC, QueueType queueType)
{
  csRef<ThreadEvent6<T, A1, A2, A3, A4, A5, A6> > threadEvent;
  threadEvent.AttachNew(new ThreadEvent6<T, A1, A2, A3, A4, A5, A6>(object, method, argsTC));
  tm->PushToQueue(queueType, threadEvent);
}

template<class T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
void QueueEvent(csRef<iThreadManager> tm, ThreadedCallable<T>* object, bool (T::*method)(A1, A2, A3, A4, A5, A6, A7), void const** &argsTC, QueueType queueType)
{
  csRef<ThreadEvent7<T, A1, A2, A3, A4, A5, A6, A7> > threadEvent;
  threadEvent.AttachNew(new ThreadEvent7<T, A1, A2, A3, A4, A5, A6, A7>(object, method, argsTC));
  tm->PushToQueue(queueType, threadEvent);
}

template<class T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
void QueueEvent(csRef<iThreadManager> tm, ThreadedCallable<T>* object, bool (T::*method)(A1, A2, A3, A4, A5, A6, A7, A8), void const** &argsTC, QueueType queueType)
{
  csRef<ThreadEvent8<T, A1, A2, A3, A4, A5, A6, A7, A8> > threadEvent;
  threadEvent.AttachNew(new ThreadEvent8<T, A1, A2, A3, A4, A5, A6, A7, A8>(object, method, argsTC));
  tm->PushToQueue(queueType, threadEvent);
}

template<class T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
void QueueEvent(csRef<iThreadManager> tm, ThreadedCallable<T>* object, bool (T::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9), void const** &argsTC, QueueType queueType)
{
  csRef<ThreadEvent9<T, A1, A2, A3, A4, A5, A6, A7, A8, A9> > threadEvent;
  threadEvent.AttachNew(new ThreadEvent9<T, A1, A2, A3, A4, A5, A6, A7, A8, A9>(object, method, argsTC));
  tm->PushToQueue(queueType, threadEvent);
}

template<class T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
void QueueEvent(csRef<iThreadManager> tm, ThreadedCallable<T>* object, bool (T::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10), void const** &argsTC, QueueType queueType)
{
  csRef<ThreadEvent10<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10> > threadEvent;
  threadEvent.AttachNew(new ThreadEvent10<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>(object, method, argsTC));
  tm->PushToQueue(queueType, threadEvent);
}

template<class T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
void QueueEvent(csRef<iThreadManager> tm, ThreadedCallable<T>* object, bool (T::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11), void const** &argsTC, QueueType queueType)
{
  csRef<ThreadEvent11<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11> > threadEvent;
  threadEvent.AttachNew(new ThreadEvent11<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>(object, method, argsTC));
  tm->PushToQueue(queueType, threadEvent);
}

template<class T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
void QueueEvent(csRef<iThreadManager> tm, ThreadedCallable<T>* object, bool (T::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12), void const** &argsTC, QueueType queueType)
{
  csRef<ThreadEvent12<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12> > threadEvent;
  threadEvent.AttachNew(new ThreadEvent12<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>(object, method, argsTC));
  tm->PushToQueue(queueType, threadEvent);
}

template<class T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
void QueueEvent(csRef<iThreadManager> tm, ThreadedCallable<T>* object, bool (T::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13), void const** &argsTC, QueueType queueType)
{
  csRef<ThreadEvent13<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13> > threadEvent;
  threadEvent.AttachNew(new ThreadEvent13<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>(object, method, argsTC));
  tm->PushToQueue(queueType, threadEvent);
}

template<class T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
void QueueEvent(csRef<iThreadManager> tm, ThreadedCallable<T>* object, bool (T::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14), void const** &argsTC, QueueType queueType)
{
  csRef<ThreadEvent14<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14> > threadEvent;
  threadEvent.AttachNew(new ThreadEvent14<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>(object, method, argsTC));
  tm->PushToQueue(queueType, threadEvent);
}

template<class T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, typename A15>
void QueueEvent(csRef<iThreadManager> tm, ThreadedCallable<T>* object, bool (T::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15), void const** &argsTC, QueueType queueType)
{
  csRef<ThreadEvent15<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15> > threadEvent;
  threadEvent.AttachNew(new ThreadEvent15<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>(object, method, argsTC));
  tm->PushToQueue(queueType, threadEvent);
}

#define THREADED_CALLABLE_DECL(type, function, returnClass, queueType, wait, forceQueue) \
  bool function##TC(csRef<iThreadReturn> ret); \
  csRef<iThreadReturn> function() \
{ \
  const type* objTC = const_cast<const type*>(this); \
  return objTC->function(); \
} \
  csRef<iThreadReturn> function() const \
{ \
  csRef<iThreadManager> tm = csQueryRegistry<iThreadManager>(GetObjectRegistry()); \
  csRef<iThreadReturn> ret; \
  ret.AttachNew(new returnClass(tm)); \
  if(!tm.IsValid() || tm->RunNow(queueType, forceQueue)) \
  { \
    type* objTC = const_cast<type*>(this); \
    objTC->function##TC(ret); \
    return ret; \
  } \
  void const** argsTC = new void const*[2]; \
  TEventMemPool *mempool = new TEventMemPool; \
  argsTC[0] = mempool; \
  argsTC[1] = mempool->Store<csRef<iThreadReturn> >(&ret); \
  QueueEvent<type, csRef<iThreadReturn> >(tm, (ThreadedCallable<type>*)this, &type::function##TC, argsTC, queueType); \
  if(wait) \
  { \
    ret->Wait(); \
  } \
  return ret; \
}

#define THREADED_CALLABLE_DECL1(type, function, returnClass, T1, A1, queueType, wait, forceQueue) \
  bool function##TC(csRef<iThreadReturn> ret, T1 A1); \
  csRef<iThreadReturn> function(T1 A1) \
{ \
  const type* objTC = const_cast<const type*>(this); \
  return objTC->function(A1); \
} \
  csRef<iThreadReturn> function(T1 A1) const \
{ \
  csRef<iThreadManager> tm = csQueryRegistry<iThreadManager>(GetObjectRegistry()); \
  csRef<iThreadReturn> ret; \
  ret.AttachNew(new returnClass(tm)); \
  if(!tm.IsValid() || tm->RunNow(queueType, forceQueue)) \
  { \
    type* objTC = const_cast<type*>(this); \
    objTC->function##TC(ret, A1); \
    return ret; \
  } \
  void const** argsTC = new void const*[3]; \
  TEventMemPool *mempool = new TEventMemPool; \
  argsTC[0] = mempool; \
  argsTC[1] = mempool->Store<csRef<iThreadReturn> >(&ret); \
  argsTC[2] = mempool->Store<T1>(&A1); \
  QueueEvent<type, csRef<iThreadReturn> , T1>(tm, (ThreadedCallable<type>*)this, &type::function##TC, argsTC, queueType); \
  if(wait) \
  { \
    ret->Wait(); \
  } \
  return ret; \
}

#define THREADED_CALLABLE_DECL2(type, function, returnClass, T1, A1, T2, A2, queueType, wait, forceQueue) \
  bool function##TC(csRef<iThreadReturn> ret, T1 A1, T2 A2); \
  csRef<iThreadReturn> function(T1 A1, T2 A2) \
{ \
  const type* objTC = const_cast<const type*>(this); \
  return objTC->function(A1, A2); \
} \
  csRef<iThreadReturn> function(T1 A1, T2 A2) const \
{ \
  csRef<iThreadManager> tm = csQueryRegistry<iThreadManager>(GetObjectRegistry()); \
  csRef<iThreadReturn> ret; \
  ret.AttachNew(new returnClass(tm)); \
  if(!tm.IsValid() || tm->RunNow(queueType, forceQueue)) \
  { \
    type* objTC = const_cast<type*>(this); \
    objTC->function##TC(ret, A1, A2); \
    return ret; \
  } \
  void const** argsTC = new void const*[4]; \
  TEventMemPool *mempool = new TEventMemPool; \
  argsTC[0] = mempool; \
  argsTC[1] = mempool->Store<csRef<iThreadReturn> >(&ret); \
  argsTC[2] = mempool->Store<T1>(&A1); \
  argsTC[3] = mempool->Store<T2>(&A2); \
  QueueEvent<type, csRef<iThreadReturn> , T1, T2>(tm, (ThreadedCallable<type>*)this, &type::function##TC, argsTC, queueType); \
  if(wait) \
  { \
    ret->Wait(); \
  } \
  return ret; \
}

#define THREADED_CALLABLE_DECL3(type, function, returnClass, T1, A1, T2, A2, T3, A3, queueType, wait, forceQueue) \
  bool function##TC(csRef<iThreadReturn> ret, T1 A1, T2 A2, T3 A3); \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3) \
{ \
  const type* objTC = const_cast<const type*>(this); \
  return objTC->function(A1, A2, A3); \
} \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3) const \
{ \
  csRef<iThreadManager> tm = csQueryRegistry<iThreadManager>(GetObjectRegistry()); \
  csRef<iThreadReturn> ret; \
  ret.AttachNew(new returnClass(tm)); \
  if(!tm.IsValid() || tm->RunNow(queueType, forceQueue)) \
  { \
    type* objTC = const_cast<type*>(this); \
    objTC->function##TC(ret, A1, A2, A3); \
    return ret; \
  } \
  void const** argsTC = new void const*[5]; \
  TEventMemPool *mempool = new TEventMemPool; \
  argsTC[0] = mempool; \
  argsTC[1] = mempool->Store<csRef<iThreadReturn> >(&ret); \
  argsTC[2] = mempool->Store<T1>(&A1); \
  argsTC[3] = mempool->Store<T2>(&A2); \
  argsTC[4] = mempool->Store<T3>(&A3); \
  QueueEvent<type, csRef<iThreadReturn>, T1, T2, T3>(tm, (ThreadedCallable<type>*)this, &type::function##TC, argsTC, queueType); \
  if(wait) \
  { \
    ret->Wait(); \
  } \
  return ret; \
}

#define THREADED_CALLABLE_DECL4(type, function, returnClass, T1, A1, T2, A2, T3, A3, T4, A4, queueType, wait, forceQueue) \
  bool function##TC(csRef<iThreadReturn> ret, T1 A1, T2 A2, T3 A3, T4 A4); \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4) \
{ \
  const type* objTC = const_cast<const type*>(this); \
  return objTC->function(A1, A2, A3, A4); \
} \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4) const \
{ \
  csRef<iThreadManager> tm = csQueryRegistry<iThreadManager>(GetObjectRegistry()); \
  csRef<iThreadReturn> ret; \
  ret.AttachNew(new returnClass(tm)); \
  if(!tm.IsValid() || tm->RunNow(queueType, forceQueue)) \
  { \
    type* objTC = const_cast<type*>(this); \
    objTC->function##TC(ret, A1, A2, A3, A4); \
    return ret; \
  } \
  void const** argsTC = new void const*[6]; \
  TEventMemPool *mempool = new TEventMemPool; \
  argsTC[0] = mempool; \
  argsTC[1] = mempool->Store<csRef<iThreadReturn> >(&ret); \
  argsTC[2] = mempool->Store<T1>(&A1); \
  argsTC[3] = mempool->Store<T2>(&A2); \
  argsTC[4] = mempool->Store<T3>(&A3); \
  argsTC[5] = mempool->Store<T4>(&A4); \
  QueueEvent<type, csRef<iThreadReturn>, T1, T2, T3, T4>(tm, (ThreadedCallable<type>*)this, &type::function##TC, argsTC, queueType); \
  if(wait) \
  { \
    ret->Wait(); \
  } \
  return ret; \
}

#define THREADED_CALLABLE_DECL5(type, function, returnClass, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, queueType, wait, forceQueue) \
  bool function##TC(csRef<iThreadReturn> ret, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5); \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5) \
{ \
  const type* objTC = const_cast<const type*>(this); \
  return objTC->function(A1, A2, A3, A4, A5); \
} \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5) const \
{ \
  csRef<iThreadManager> tm = csQueryRegistry<iThreadManager>(GetObjectRegistry()); \
  csRef<iThreadReturn> ret; \
  ret.AttachNew(new returnClass(tm)); \
  if(!tm.IsValid() || tm->RunNow(queueType, forceQueue)) \
  { \
    type* objTC = const_cast<type*>(this); \
    objTC->function##TC(ret, A1, A2, A3, A4, A5); \
    return ret; \
  } \
  void const** argsTC = new void const*[7]; \
  TEventMemPool *mempool = new TEventMemPool; \
  argsTC[0] = mempool; \
  argsTC[1] = mempool->Store<csRef<iThreadReturn> >(&ret); \
  argsTC[2] = mempool->Store<T1>(&A1); \
  argsTC[3] = mempool->Store<T2>(&A2); \
  argsTC[4] = mempool->Store<T3>(&A3); \
  argsTC[5] = mempool->Store<T4>(&A4); \
  argsTC[6] = mempool->Store<T5>(&A5); \
  QueueEvent<type, csRef<iThreadReturn>, T1, T2, T3, T4, T5>(tm, (ThreadedCallable<type>*)this, &type::function##TC, argsTC, queueType); \
  if(wait) \
  { \
    ret->Wait(); \
  } \
  return ret; \
}

#define THREADED_CALLABLE_DECL6(type, function, returnClass, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, queueType, wait, forceQueue) \
  bool function##TC(csRef<iThreadReturn> ret, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6); \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6) \
{ \
  const type* objTC = const_cast<const type*>(this); \
  return objTC->function(A1, A2, A3, A4, A5, A6); \
} \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6) const \
{ \
  csRef<iThreadManager> tm = csQueryRegistry<iThreadManager>(GetObjectRegistry()); \
  csRef<iThreadReturn> ret; \
  ret.AttachNew(new returnClass(tm)); \
  if(!tm.IsValid() || tm->RunNow(queueType, forceQueue)) \
  { \
    type* objTC = const_cast<type*>(this); \
    objTC->function##TC(ret, A1, A2, A3, A4, A5, A6); \
    return ret; \
  } \
  void const** argsTC = new void const*[8]; \
  TEventMemPool *mempool = new TEventMemPool; \
  argsTC[0] = mempool; \
  argsTC[1] = mempool->Store<csRef<iThreadReturn> >(&ret); \
  argsTC[2] = mempool->Store<T1>(&A1); \
  argsTC[3] = mempool->Store<T2>(&A2); \
  argsTC[4] = mempool->Store<T3>(&A3); \
  argsTC[5] = mempool->Store<T4>(&A4); \
  argsTC[6] = mempool->Store<T5>(&A5); \
  argsTC[7] = mempool->Store<T6>(&A6); \
  QueueEvent<type, csRef<iThreadReturn>, T1, T2, T3, T4, T5, T6>(tm, (ThreadedCallable<type>*)this, &type::function##TC, argsTC, queueType); \
  if(wait) \
  { \
    ret->Wait(); \
  } \
  return ret; \
}

#define THREADED_CALLABLE_DECL7(type, function, returnClass, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, queueType, wait, forceQueue) \
  bool function##TC(csRef<iThreadReturn> ret, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7); \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7) \
{ \
  const type* objTC = const_cast<const type*>(this); \
  return objTC->function(A1, A2, A3, A4, A5, A6, A7); \
} \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7) const \
{ \
  csRef<iThreadManager> tm = csQueryRegistry<iThreadManager>(GetObjectRegistry()); \
  csRef<iThreadReturn> ret; \
  ret.AttachNew(new returnClass(tm)); \
  if(!tm.IsValid() || tm->RunNow(queueType, forceQueue)) \
  { \
    type* objTC = const_cast<type*>(this); \
    objTC->function##TC(ret, A1, A2, A3, A4, A5, A6, A7); \
    return ret; \
  } \
  void const** argsTC = new void const*[9]; \
  TEventMemPool *mempool = new TEventMemPool; \
  argsTC[0] = mempool; \
  argsTC[1] = mempool->Store<csRef<iThreadReturn> >(&ret); \
  argsTC[2] = mempool->Store<T1>(&A1); \
  argsTC[3] = mempool->Store<T2>(&A2); \
  argsTC[4] = mempool->Store<T3>(&A3); \
  argsTC[5] = mempool->Store<T4>(&A4); \
  argsTC[6] = mempool->Store<T5>(&A5); \
  argsTC[7] = mempool->Store<T6>(&A6); \
  argsTC[8] = mempool->Store<T7>(&A7); \
  QueueEvent<type, csRef<iThreadReturn>, T1, T2, T3, T4, T5, T6, T7>(tm, (ThreadedCallable<type>*)this, &type::function##TC, argsTC, queueType); \
  if(wait) \
  { \
    ret->Wait(); \
  } \
  return ret; \
}

#define THREADED_CALLABLE_DECL8(type, function, returnClass, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, queueType, wait, forceQueue) \
  bool function##TC(csRef<iThreadReturn> ret, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8); \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8) \
{ \
  const type* objTC = const_cast<const type*>(this); \
  return objTC->function(A1, A2, A3, A4, A5, A6, A7, A8); \
} \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8) const \
{ \
  csRef<iThreadManager> tm = csQueryRegistry<iThreadManager>(GetObjectRegistry()); \
  csRef<iThreadReturn> ret; \
  ret.AttachNew(new returnClass(tm)); \
  if(!tm.IsValid() || tm->RunNow(queueType, forceQueue)) \
  { \
    type* objTC = const_cast<type*>(this); \
    objTC->function##TC(ret, A1, A2, A3, A4, A5, A6, A7, A8); \
    return ret; \
  } \
  void const** argsTC = new void const*[10]; \
  TEventMemPool *mempool = new TEventMemPool; \
  argsTC[0] = mempool; \
  argsTC[1] = mempool->Store<csRef<iThreadReturn> >(&ret); \
  argsTC[2] = mempool->Store<T1>(&A1); \
  argsTC[3] = mempool->Store<T2>(&A2); \
  argsTC[4] = mempool->Store<T3>(&A3); \
  argsTC[5] = mempool->Store<T4>(&A4); \
  argsTC[6] = mempool->Store<T5>(&A5); \
  argsTC[7] = mempool->Store<T6>(&A6); \
  argsTC[8] = mempool->Store<T7>(&A7); \
  argsTC[9] = mempool->Store<T8>(&A8); \
  QueueEvent<type, csRef<iThreadReturn>, T1, T2, T3, T4, T5, T6, T7, T8>(tm, (ThreadedCallable<type>*)this, &type::function##TC, argsTC, queueType); \
  if(wait) \
  { \
    ret->Wait(); \
  } \
  return ret; \
}

#define THREADED_CALLABLE_DECL9(type, function, returnClass, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9, queueType, wait, forceQueue) \
  bool function##TC(csRef<iThreadReturn> ret, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9); \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9) \
{ \
  const type* objTC = const_cast<const type*>(this); \
  return objTC->function(A1, A2, A3, A4, A5, A6, A7, A8, A9); \
} \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9) const \
{ \
  csRef<iThreadManager> tm = csQueryRegistry<iThreadManager>(GetObjectRegistry()); \
  csRef<iThreadReturn> ret; \
  ret.AttachNew(new returnClass(tm)); \
  if(!tm.IsValid() || tm->RunNow(queueType, forceQueue)) \
  { \
    type* objTC = const_cast<type*>(this); \
    objTC->function##TC(ret, A1, A2, A3, A4, A5, A6, A7, A8, A9); \
    return ret; \
  } \
  void const** argsTC = new void const*[11]; \
  TEventMemPool *mempool = new TEventMemPool; \
  argsTC[0] = mempool; \
  argsTC[1] = mempool->Store<csRef<iThreadReturn> >(&ret); \
  argsTC[2] = mempool->Store<T1>(&A1); \
  argsTC[3] = mempool->Store<T2>(&A2); \
  argsTC[4] = mempool->Store<T3>(&A3); \
  argsTC[5] = mempool->Store<T4>(&A4); \
  argsTC[6] = mempool->Store<T5>(&A5); \
  argsTC[7] = mempool->Store<T6>(&A6); \
  argsTC[8] = mempool->Store<T7>(&A7); \
  argsTC[9] = mempool->Store<T8>(&A8); \
  argsTC[10] = mempool->Store<T9>(&A9); \
  QueueEvent<type, csRef<iThreadReturn>, T1, T2, T3, T4, T5, T6, T7, T8, T9>(tm, (ThreadedCallable<type>*)this, &type::function##TC, argsTC, queueType); \
  if(wait) \
  { \
    ret->Wait(); \
  } \
  return ret; \
}

#define THREADED_CALLABLE_DECL10(type, function, returnClass, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9, T10, A10, queueType, wait, forceQueue) \
  bool function##TC(csRef<iThreadReturn> ret, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10); \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10) \
{ \
  const type* objTC = const_cast<const type*>(this); \
  return objTC->function(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10); \
} \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10) const \
{ \
  csRef<iThreadManager> tm = csQueryRegistry<iThreadManager>(GetObjectRegistry()); \
  csRef<iThreadReturn> ret; \
  ret.AttachNew(new returnClass(tm)); \
  if(!tm.IsValid() || tm->RunNow(queueType, forceQueue)) \
  { \
    type* objTC = const_cast<type*>(this); \
    objTC->function##TC(ret, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10); \
    return ret; \
  } \
  void const** argsTC = new void const*[12]; \
  TEventMemPool *mempool = new TEventMemPool; \
  argsTC[0] = mempool; \
  argsTC[1] = mempool->Store<csRef<iThreadReturn> >(&ret); \
  argsTC[2] = mempool->Store<T1>(&A1); \
  argsTC[3] = mempool->Store<T2>(&A2); \
  argsTC[4] = mempool->Store<T3>(&A3); \
  argsTC[5] = mempool->Store<T4>(&A4); \
  argsTC[6] = mempool->Store<T5>(&A5); \
  argsTC[7] = mempool->Store<T6>(&A6); \
  argsTC[8] = mempool->Store<T7>(&A7); \
  argsTC[9] = mempool->Store<T8>(&A8); \
  argsTC[10] = mempool->Store<T9>(&A9); \
  argsTC[11] = mempool->Store<T10>(&A10); \
  QueueEvent<type, csRef<iThreadReturn>, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>(tm, (ThreadedCallable<type>*)this, &type::function##TC, argsTC, queueType); \
  if(wait) \
  { \
    ret->Wait(); \
  } \
  return ret; \
}

#define THREADED_CALLABLE_DECL11(type, function, returnClass, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9, T10, A10, T11, A11, queueType, wait, forceQueue) \
  bool function##TC(csRef<iThreadReturn> ret, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10, T11 A11); \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10, T11 A11) \
{ \
  const type* objTC = const_cast<const type*>(this); \
  return objTC->function(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11); \
} \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10, T11 A11) const \
{ \
  csRef<iThreadManager> tm = csQueryRegistry<iThreadManager>(GetObjectRegistry()); \
  csRef<iThreadReturn> ret; \
  ret.AttachNew(new returnClass(tm)); \
  if(!tm.IsValid() || tm->RunNow(queueType, forceQueue)) \
  { \
    type* objTC = const_cast<type*>(this); \
    objTC->function##TC(ret, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11); \
    return ret; \
  } \
  void const** argsTC = new void const*[13]; \
  TEventMemPool *mempool = new TEventMemPool; \
  argsTC[0] = mempool; \
  argsTC[1] = mempool->Store<csRef<iThreadReturn> >(&ret); \
  argsTC[2] = mempool->Store<T1>(&A1); \
  argsTC[3] = mempool->Store<T2>(&A2); \
  argsTC[4] = mempool->Store<T3>(&A3); \
  argsTC[5] = mempool->Store<T4>(&A4); \
  argsTC[6] = mempool->Store<T5>(&A5); \
  argsTC[7] = mempool->Store<T6>(&A6); \
  argsTC[8] = mempool->Store<T7>(&A7); \
  argsTC[9] = mempool->Store<T8>(&A8); \
  argsTC[10] = mempool->Store<T9>(&A9); \
  argsTC[11] = mempool->Store<T10>(&A10); \
  argsTC[12] = mempool->Store<T11>(&A11); \
  QueueEvent<type, csRef<iThreadReturn>, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>(tm, (ThreadedCallable<type>*)this, &type::function##TC, argsTC, queueType); \
  if(wait) \
  { \
    ret->Wait(); \
  } \
  return ret; \
}

#define THREADED_CALLABLE_DECL12(type, function, returnClass, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9, T10, A10, T11, A11, T12, A12, queueType, wait, forceQueue) \
  bool function##TC(csRef<iThreadReturn> ret, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10, T11 A11, T12 A12); \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10, T11 A11, T12 A12) \
{ \
  const type* objTC = const_cast<const type*>(this); \
  return objTC->function(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12); \
} \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10, T11 A11, T12 A12) const \
{ \
  csRef<iThreadManager> tm = csQueryRegistry<iThreadManager>(GetObjectRegistry()); \
  csRef<iThreadReturn> ret; \
  ret.AttachNew(new returnClass(tm)); \
  if(!tm.IsValid() || tm->RunNow(queueType, forceQueue)) \
  { \
    type* objTC = const_cast<type*>(this); \
    objTC->function##TC(ret, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12); \
    return ret; \
  } \
  void const** argsTC = new void const*[14]; \
  TEventMemPool *mempool = new TEventMemPool; \
  argsTC[0] = mempool; \
  argsTC[1] = mempool->Store<csRef<iThreadReturn> >(&ret); \
  argsTC[2] = mempool->Store<T1>(&A1); \
  argsTC[3] = mempool->Store<T2>(&A2); \
  argsTC[4] = mempool->Store<T3>(&A3); \
  argsTC[5] = mempool->Store<T4>(&A4); \
  argsTC[6] = mempool->Store<T5>(&A5); \
  argsTC[7] = mempool->Store<T6>(&A6); \
  argsTC[8] = mempool->Store<T7>(&A7); \
  argsTC[9] = mempool->Store<T8>(&A8); \
  argsTC[10] = mempool->Store<T9>(&A9); \
  argsTC[11] = mempool->Store<T10>(&A10); \
  argsTC[12] = mempool->Store<T11>(&A11); \
  argsTC[13] = mempool->Store<T12>(&A12); \
  QueueEvent<type, csRef<iThreadReturn>, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12>(tm, (ThreadedCallable<type>*)this, &type::function##TC, argsTC, queueType); \
  if(wait) \
  { \
    ret->Wait(); \
  } \
  return ret; \
}

#define THREADED_CALLABLE_DECL13(type, function, returnClass, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9, T10, A10, T11, A11, T12, A12, T13, A13, queueType, wait, forceQueue) \
  bool function##TC(csRef<iThreadReturn> ret, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10, T11 A11, T12 A12, T13 A13); \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10, T11 A11, T12 A12, T13 A13) \
{ \
  const type* objTC = const_cast<const type*>(this); \
  return objTC->function(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13); \
} \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10, T11 A11, T12 A12, T13 A13) const \
{ \
  csRef<iThreadManager> tm = csQueryRegistry<iThreadManager>(GetObjectRegistry()); \
  csRef<iThreadReturn> ret; \
  ret.AttachNew(new returnClass(tm)); \
  if(!tm.IsValid() || tm->RunNow(queueType, forceQueue)) \
  { \
    type* objTC = const_cast<type*>(this); \
    objTC->function##TC(ret, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13); \
    return ret; \
  } \
  void const** argsTC = new void const*[15]; \
  TEventMemPool *mempool = new TEventMemPool; \
  argsTC[0] = mempool; \
  argsTC[1] = mempool->Store<csRef<iThreadReturn> >(&ret); \
  argsTC[2] = mempool->Store<T1>(&A1); \
  argsTC[3] = mempool->Store<T2>(&A2); \
  argsTC[4] = mempool->Store<T3>(&A3); \
  argsTC[5] = mempool->Store<T4>(&A4); \
  argsTC[6] = mempool->Store<T5>(&A5); \
  argsTC[7] = mempool->Store<T6>(&A6); \
  argsTC[8] = mempool->Store<T7>(&A7); \
  argsTC[9] = mempool->Store<T8>(&A8); \
  argsTC[10] = mempool->Store<T9>(&A9); \
  argsTC[11] = mempool->Store<T10>(&A10); \
  argsTC[12] = mempool->Store<T11>(&A11); \
  argsTC[13] = mempool->Store<T12>(&A12); \
  argsTC[14] = mempool->Store<T13>(&A13); \
  QueueEvent<type, csRef<iThreadReturn>, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>(tm, (ThreadedCallable<type>*)this, &type::function##TC, argsTC, queueType); \
  if(wait) \
  { \
    ret->Wait(); \
  } \
  return ret; \
}

#define THREADED_CALLABLE_DECL14(type, function, returnClass, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9, T10, A10, T11, A11, T12, A12, T13, A13, T14, A14, queueType, wait, forceQueue) \
  bool function##TC(csRef<iThreadReturn> ret, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10, T11 A11, T12 A12, T13 A13, T14 A14); \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10, T11 A11, T12 A12, T13 A13, T14 A14) \
{ \
  const type* objTC = const_cast<const type*>(this); \
  return objTC->function(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14); \
} \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10, T11 A11, T12 A12, T13 A13, T14 A14) const \
{ \
  csRef<iThreadManager> tm = csQueryRegistry<iThreadManager>(GetObjectRegistry()); \
  csRef<iThreadReturn> ret; \
  ret.AttachNew(new returnClass(tm)); \
  if(!tm.IsValid() || tm->RunNow(queueType, forceQueue))) \
  { \
    type* objTC = const_cast<type*>(this); \
    objTC->function##TC(ret, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14); \
    return ret; \
  } \
  void const** argsTC = new void const*[16]; \
  TEventMemPool *mempool = new TEventMemPool; \
  argsTC[0] = mempool; \
  argsTC[1] = mempool->Store<csRef<iThreadReturn> >(ret); \
  argsTC[2] = mempool->Store<T1>(&A1); \
  argsTC[3] = mempool->Store<T2>(&A2); \
  argsTC[4] = mempool->Store<T3>(&A3); \
  argsTC[5] = mempool->Store<T4>(&A4); \
  argsTC[6] = mempool->Store<T5>(&A5); \
  argsTC[7] = mempool->Store<T6>(&A6); \
  argsTC[8] = mempool->Store<T7>(&A7); \
  argsTC[9] = mempool->Store<T8>(&A8); \
  argsTC[10] = mempool->Store<T9>(&A9); \
  argsTC[11] = mempool->Store<T10>(&A10); \
  argsTC[12] = mempool->Store<T11>(&A11); \
  argsTC[13] = mempool->Store<T12>(&A12); \
  argsTC[14] = mempool->Store<T13>(&A13); \
  argsTC[15] = mempool->Store<T14>(&A14); \
  QueueEvent<type, csRef<iThreadReturn>, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14>(tm, (ThreadedCallable<type>*)this, &type::function##TC, argsTC, queueType); \
  if(wait) \
  { \
    ret->Wait(); \
  } \
  return ret; \
}

#define THREADED_CALLABLE_DECL15(type, function, returnClass, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9, T10, A10, T11, A11, T12, A12, T13, A13, T14, A14, T15, A15, queueType, wait, forceQueue) \
  bool function##TC(csRef<iThreadReturn> ret, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10, T11 A11, T12 A12, T13 A13, T14 A14, T15 A15); \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10, T11 A11, T12 A12, T13 A13, T14 A14, T15 A15) \
{ \
  const type* objTC = const_cast<const type*>(this); \
  return objTC->function(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15); \
} \
  csRef<iThreadReturn> function(T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10, T11 A11, T12 A12, T13 A13, T14 A14, T15 A15) const \
{ \
  csRef<iThreadManager> tm = csQueryRegistry<iThreadManager>(GetObjectRegistry()); \
  csRef<iThreadReturn> ret; \
  ret.AttachNew(new returnClass(tm)); \
  if(!tm.IsValid() || tm->RunNow(queueType, forceQueue))) \
  { \
    type* objTC = const_cast<type*>(this); \
    objTC->function##TC(ret, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15); \
    return ret; \
  } \
  void const** argsTC = new void const*[17]; \
  TEventMemPool *mempool = new TEventMemPool; \
  argsTC[0] = mempool; \
  argsTC[1] = mempool->Store<csRef<iThreadReturn> >(ret); \
  argsTC[2] = mempool->Store<T1>(&A1); \
  argsTC[3] = mempool->Store<T2>(&A2); \
  argsTC[4] = mempool->Store<T3>(&A3); \
  argsTC[5] = mempool->Store<T4>(&A4); \
  argsTC[6] = mempool->Store<T5>(&A5); \
  argsTC[7] = mempool->Store<T6>(&A6); \
  argsTC[8] = mempool->Store<T7>(&A7); \
  argsTC[9] = mempool->Store<T8>(&A8); \
  argsTC[10] = mempool->Store<T9>(&A9); \
  argsTC[11] = mempool->Store<T10>(&A10); \
  argsTC[12] = mempool->Store<T11>(&A11); \
  argsTC[13] = mempool->Store<T12>(&A12); \
  argsTC[14] = mempool->Store<T13>(&A13); \
  argsTC[15] = mempool->Store<T14>(&A14); \
  argsTC[16] = mempool->Store<T15>(&A15); \
  QueueEvent<type, csRef<iThreadReturn>, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>(tm, (ThreadedCallable<type>*)this, &type::function##TC, argsTC, queueType); \
  if(wait) \
  { \
    ret->Wait(); \
  } \
  return ret; \
}

#define THREADED_CALLABLE_IMPL(type, function) \
  bool type::function##TC(csRef<iThreadReturn> ret)

#define THREADED_CALLABLE_IMPL1(type, function, A1) \
  bool type::function##TC(csRef<iThreadReturn> ret, A1)

#define THREADED_CALLABLE_IMPL2(type, function, A1, A2) \
  bool type::function##TC(csRef<iThreadReturn> ret, A1, A2)

#define THREADED_CALLABLE_IMPL3(type, function, A1, A2, A3) \
  bool type::function##TC(csRef<iThreadReturn> ret, A1, A2, A3)

#define THREADED_CALLABLE_IMPL4(type, function, A1, A2, A3, A4) \
  bool type::function##TC(csRef<iThreadReturn> ret, A1, A2, A3, A4)

#define THREADED_CALLABLE_IMPL5(type, function, A1, A2, A3, A4, A5) \
  bool type::function##TC(csRef<iThreadReturn> ret, A1, A2, A3, A4, A5)

#define THREADED_CALLABLE_IMPL6(type, function, A1, A2, A3, A4, A5, A6) \
  bool type::function##TC(csRef<iThreadReturn> ret, A1, A2, A3, A4, A5, A6)

#define THREADED_CALLABLE_IMPL7(type, function, A1, A2, A3, A4, A5, A6, A7) \
  bool type::function##TC(csRef<iThreadReturn> ret, A1, A2, A3, A4, A5, A6, A7)

#define THREADED_CALLABLE_IMPL8(type, function, A1, A2, A3, A4, A5, A6, A7, A8) \
  bool type::function##TC(csRef<iThreadReturn> ret, A1, A2, A3, A4, A5, A6, A7, A8)

#define THREADED_CALLABLE_IMPL9(type, function, A1, A2, A3, A4, A5, A6, A7, A8, A9) \
  bool type::function##TC(csRef<iThreadReturn> ret, A1, A2, A3, A4, A5, A6, A7, A8, A9)

#define THREADED_CALLABLE_IMPL10(type, function, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) \
  bool type::function##TC(csRef<iThreadReturn> ret, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)

#define THREADED_CALLABLE_IMPL11(type, function, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) \
  bool type::function##TC(csRef<iThreadReturn> ret, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)

#define THREADED_CALLABLE_IMPL12(type, function, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) \
  bool type::function##TC(csRef<iThreadReturn> ret, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)

#define THREADED_CALLABLE_IMPL13(type, function, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) \
  bool type::function##TC(csRef<iThreadReturn> ret, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)

#define THREADED_CALLABLE_IMPL14(type, function, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) \
  bool type::function##TC(csRef<iThreadReturn> ret, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)

#define THREADED_CALLABLE_IMPL15(type, function, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15) \
  bool type::function##TC(csRef<iThreadReturn> ret, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15)

#endif // __CS_IUTIL_THREADMANAGER_H__
