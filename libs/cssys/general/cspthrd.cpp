/*
    Copyright (C) 2002 by Norman Kr�mer

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
#include "cspthrd.h"
#include <sys/time.h>

// Depending upon the platform, one of these headers declares strerror().
#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef CS_DEBUG
#define CS_SHOW_ERROR if (lasterr) printf ("%s\n",lasterr)
#else
#define CS_SHOW_ERROR
#endif

csRef<csMutex> csMutex::Create (bool needrecursive)
{
  return csPtr<csMutex>(new csPosixMutex (needrecursive));
}

csPosixMutex::csPosixMutex (bool needrecursive)
{
  lasterr = NULL;

#ifdef CS_PTHREAD_MUTEX_RECURSIVE
  if (needrecursive)
  {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init (&attr);
    pthread_mutexattr_settype (&attr, CS_PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init (&mutex, &attr);
  }
  else
  {
    pthread_mutex_init (&mutex, NULL);
  }
#else
  (void) needrecursive;
  count = 0;
  owner = 0;
  pthread_mutex_init (&mutex, NULL);
#endif
}

csPosixMutex::~csPosixMutex ()
{
  Destroy ();
}

bool csPosixMutex::Destroy ()
{
  int rc = pthread_mutex_destroy (&mutex);
  switch (rc)
  {
  case EBUSY:
    lasterr = "Mutex busy";
    break;
  case 0:
    lasterr = NULL;
    break;
  default:
    lasterr = "Unknown error while destroying mutex";
    break;
  }
  CS_SHOW_ERROR;
  return rc == 0;
}

bool csPosixMutex::LockWait()
{
  int rc;
#ifdef CS_PTHREAD_MUTEX_RECURSIVE
  rc = pthread_mutex_lock (&mutex);
#else
  pthread_t self = pthread_self();
  if (owner != self)
  {
    rc = pthread_mutex_lock(&mutex);
    owner = self;
  }
  else
    rc = 0;
  count += 1;
#endif

  switch (rc)
  {
  case EINVAL:
    lasterr = "Mutex not initialized";
    break;
  case EDEADLK:
    lasterr = "Deadlock";
    break;
  case 0:
    lasterr = NULL;
    break;
  default:
    lasterr = "Unknown error while locking mutex";
    break;
  }
  CS_SHOW_ERROR;
  return rc == 0;
}

bool csPosixMutex::LockTry ()
{
  int rc;
#ifdef CS_PTHREAD_MUTEX_RECURSIVE
  rc = pthread_mutex_trylock (&mutex);
#else
  pthread_t self = pthread_self();
  if (owner != self)
  {
    rc = pthread_mutex_trylock(&mutex);
    if (rc!=0)
      return false;
    owner = self;                     
  }
  else
    rc = 0;
  count += 1;
#endif

  switch (rc)
  {
  case EINVAL:
    lasterr = "Mutex not initialized";
    break;
  case EBUSY:
    lasterr = "Mutex locked";
    break;
  case EDEADLK:
    lasterr = "Deadlock";
    break;
  case 0:
    lasterr = NULL;
    break;
  default:
    lasterr = "Unknown error while trying to lock mutex";
    //    printf ("rc = %d\n", rc);
    break;
  }
  CS_SHOW_ERROR;
  return rc == 0;
}

bool csPosixMutex::Release ()
{
  int rc;
  
#ifdef CS_PTHREAD_MUTEX_RECURSIVE
  rc = pthread_mutex_unlock (&mutex);
#else
  CS_ASSERT (pthread_self() == owner);
  if (--count == 0)
  {
    owner = 0;
    rc = pthread_mutex_unlock(&mutex);
  }
  else
    rc = 0;
#endif
 
  switch (rc)
  {
  case EINVAL:
    lasterr = "Mutex not initialized";
    break;
  case EPERM:
    lasterr = "No permission";
    break;
  case 0:
    lasterr = NULL;
    break;
  default:
    lasterr = "Unknown error while releasing mutex";
    break;
  }
  CS_SHOW_ERROR;
  return rc == 0;
}

void csPosixMutex::Cleanup (void* arg)
{
  ((csPosixMutex*)arg)->Release ();
}

char const* csPosixMutex::GetLastError ()
{
  return lasterr;
}


csRef<csSemaphore> csSemaphore::Create (uint32 value)
{
  return csPtr<csSemaphore>(new csPosixSemaphore (value));
}

csPosixSemaphore::csPosixSemaphore (uint32 value)
{
  int rc = sem_init (&sem, 0, (unsigned int)value);
  if (rc)
    lasterr = strerror(errno);
  else
    lasterr = NULL;
  CS_SHOW_ERROR;
}

csPosixSemaphore::~csPosixSemaphore ()
{
  Destroy ();
}

bool csPosixSemaphore::LockWait ()
{
  sem_wait (&sem);
  return true;
}

bool csPosixSemaphore::LockTry ()
{
  int rc = sem_trywait (&sem);
  if (rc)
    lasterr = strerror(errno);
  else
    lasterr = NULL;
  CS_SHOW_ERROR;
  return rc == 0;
}

bool csPosixSemaphore::Release ()
{
  int rc = sem_post (&sem);
  if (rc)
    lasterr = strerror(errno);
  else
    lasterr = NULL;
  CS_SHOW_ERROR;
  return rc == 0;
}

uint32 csPosixSemaphore::Value ()
{
  int val;
  sem_getvalue (&sem, &val);
  return (uint32)val;
}

bool csPosixSemaphore::Destroy ()
{
  int rc = sem_destroy (&sem);
  if (rc)
    lasterr = strerror(errno);
  else
    lasterr = NULL;
  CS_SHOW_ERROR;
  return rc == 0;
}

char const* csPosixSemaphore::GetLastError ()
{
  return lasterr;
}


csRef<csCondition> csCondition::Create (uint32 conditionAttributes)
{
  return csPtr<csCondition>(new csPosixCondition (conditionAttributes));
}

csPosixCondition::csPosixCondition (uint32 /*conditionAttributes*/)
{
  pthread_cond_init (&cond, NULL);
  lasterr = NULL;
}

csPosixCondition::~csPosixCondition ()
{
  Destroy ();
}

void csPosixCondition::Signal (bool WakeAll)
{
  if (WakeAll)
    pthread_cond_broadcast (&cond);
  else
    pthread_cond_signal (&cond);
}

bool csPosixCondition::Wait (csMutex* mutex, csTicks timeout)
{
  int rc = 0;
  if (timeout > 0)
  {
    struct timeval now;
    struct timezone tz;
    struct timespec to;
    gettimeofday (&now, &tz);
    to.tv_sec = now.tv_sec + (timeout / 1000);
    to.tv_nsec = (now.tv_usec + (timeout % 1000) * 1000) * 1000;
    rc = pthread_cond_timedwait (&cond, &((csPosixMutex*)mutex)->mutex, &to);
    switch (rc)
    {
    case ETIMEDOUT:
      lasterr = "Timeout";
      break;
    case EINTR:
      lasterr = "Wait interrupted";
      break;
    case 0:
      lasterr = NULL;
      break;
    default:
      lasterr = "Unknown error while timed waiting for condition";
      break;
    }
  }
  else
    pthread_cond_wait (&cond, &((csPosixMutex*)mutex)->mutex);
  CS_SHOW_ERROR;
  return rc == 0;
}

bool csPosixCondition::Destroy ()
{
  int rc = pthread_cond_destroy (&cond);
  switch (rc)
  {
  case EBUSY:
    lasterr = "Condition busy";
    break;
  case 0:
    lasterr = NULL;
    break;
  default:
    lasterr = "Unknown error while destroying condition";
    break;
  }
  CS_SHOW_ERROR;
  return rc == 0;
}

char const* csPosixCondition::GetLastError ()
{
  return lasterr;
}


csRef<csThread> csThread::Create (csRunnable* r, uint32 options)
{
  return csPtr<csThread>(new csPosixThread (r, options));
}

csPosixThread::csPosixThread (csRunnable* r, uint32 /*options*/)
{
  runnable = r;
  running = false;
  created = false;
  lasterr = NULL;
}

csPosixThread::~csPosixThread ()
{
  if (running)
    Stop ();
//if (created)
//  pthread_join (thread, NULL); // clean up resources
}

bool csPosixThread::Start ()
{
  if (!running && runnable)
  {
    if (created)
    {
      pthread_join (thread, NULL); // clean up resources
      created = false;
    }
    pthread_attr_t attr;
    int rc;

    // Force thread to be joinable, in later pthread implementations this
    // is default already Thread cancellation state is _assumed_ to be
    // PTHREAD_CANCEL_ENABLE and cancellation type is
    // PTHREAD_CANCEL_DEFERRED
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    rc = pthread_create(&thread, &attr, ThreadRun, (void*)this); 

    switch (rc)
    {
    case EAGAIN:
      lasterr = "Out of system resources.";
      break;
    case EINVAL:
      lasterr = "Tried to create thread with wrong attributes";
      break;
    case EPERM:
      lasterr = "No permission to create thread";
      break;
    case 0:
      lasterr = NULL;
      running = true;
      created = true;
      break;
    default:
      lasterr = "Unknown error while creating thread";
      break;
    }
    pthread_attr_destroy(&attr);
  }
  CS_SHOW_ERROR;
  return running;
}

bool csPosixThread::Stop ()
{
  if (running)
  {
    int rc = pthread_cancel (thread);
    switch (rc)
    {
    case ESRCH:
      lasterr = "Trying to stop unknown thread";
      break;
    case 0:
      lasterr = NULL;
      running = false;
      break;
    default:
      lasterr = "Unknown error while cancelling thread";
      break;
    }
  }
  CS_SHOW_ERROR;
  return !running;
}

bool csPosixThread::Wait ()
{
  if (running)
  {
    int rc = pthread_join (thread,NULL);
    switch (rc)
    {
    case ESRCH:
      lasterr = "Trying to wait for unknown thread";
      break;
    case 0:
      lasterr = NULL;
      running = false;
      created=false;
      break;
    default:
      //      lasterr = "Unknown error while waiting for thread";
      lasterr = strerror(errno);
      break;
    }
  }
  CS_SHOW_ERROR;
  return !running;
}

char const* csPosixThread::GetLastError ()
{
  return lasterr;
}

void* csPosixThread::ThreadRun (void* param)
{
  csPosixThread* thread = (csPosixThread*)param;
  thread->runnable->Run ();
  thread->running = false;
  pthread_exit (NULL);
  return NULL;
}

#undef CS_SHOW_ERROR
