/*
    Copyright (C) 2001 by Jorrit Tyberghein

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

#include <string.h>
#include "cssysdef.h"
#include "csver.h"
#include "csutil/scf.h"
#include "csutil/util.h"
#include "csutil/refarr.h"
#include "reporter.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (csReporter)

SCF_EXPORT_CLASS_TABLE (reporter)
  SCF_EXPORT_CLASS (csReporter, "crystalspace.utilities.reporter",
    "Reporting utility")
SCF_EXPORT_CLASS_TABLE_END

SCF_IMPLEMENT_IBASE (csReporter)
  SCF_IMPLEMENTS_INTERFACE (iReporter)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csReporter::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csReporter::csReporter (iBase *iParent)
{
  SCF_CONSTRUCT_IBASE (iParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent);
  object_reg = NULL;
  mutex = csMutex::Create (true);
}

csReporter::~csReporter ()
{
  Clear (-1);
  while (listeners.Length () > 0)
  {
    iReporterListener* listener = (iReporterListener*)listeners[0];
    listener->DecRef ();
    listeners.Delete (0);
  }
}

bool csReporter::Initialize (iObjectRegistry *object_reg)
{
  csReporter::object_reg = object_reg;
  return true;
}

void csReporter::Report (int severity, const char* msgId,
  	const char* description, ...)
{
  va_list arg;
  va_start (arg, description);
  ReportV (severity, msgId, description, arg);
  va_end (arg);
}

void csReporter::ReportV (int severity, const char* msgId,
  	const char* description, va_list arg)
{
  char buf[4000];
  vsprintf (buf, description, arg);

  // To ensure thread-safety we first copy the listeners.
  csRefArray<iReporterListener> copy;
  {
    csScopedMutexLock lock (mutex);
    int i;
    for (i = 0 ; i < listeners.Length () ; i++)
    {
      iReporterListener* listener = (iReporterListener*)listeners[i];
      copy.Push (listener);
    }
  }

  bool add_msg = true;
  int i;
  for (i = 0 ; i < copy.Length () ; i++)
  {
    iReporterListener* listener = copy[i];
    if (listener->Report (this, severity, msgId, buf))
    {
      add_msg = false;
      break;
    }
  }

  if (add_msg)
  {
    csReporterMessage* msg = new csReporterMessage ();
    msg->severity = severity;
    msg->id = csStrNew (msgId);
    msg->description = csStrNew (buf);
    csScopedMutexLock lock (mutex);
    messages.Push (msg);
  }
}

void csReporter::Clear (int severity)
{
  csScopedMutexLock lock (mutex);

  int i = 0;
  int len = messages.Length ();
  while (i < len)
  {
    csReporterMessage* msg = (csReporterMessage*)messages[i];
    if (severity == -1 || msg->severity == severity)
    {
      delete msg;
      messages.Delete (i);
      len--;
    }
    else
    {
      i++;
    }
  }
}

void csReporter::Clear (const char* mask)
{
  csScopedMutexLock lock (mutex);
  int i = 0;
  int len = messages.Length ();
  while (i < len)
  {
    csReporterMessage* msg = (csReporterMessage*)messages[i];
    if (csGlobMatches (msg->id, mask))
    {
      delete msg;
      messages.Delete (i);
      len--;
    }
    else
    {
      i++;
    }
  }
}

int csReporter::GetMessageCount () const
{
  csScopedMutexLock lock (mutex);
  return messages.Length ();
}

int csReporter::GetMessageSeverity (int idx) const
{
  csScopedMutexLock lock (mutex);
  if (idx < 0 || idx >= messages.Length ()) return -1;
  csReporterMessage* msg = (csReporterMessage*)messages[idx];
  return msg->severity;
}

const char* csReporter::GetMessageId (int idx) const
{
  csScopedMutexLock lock (mutex);
  if (idx < 0 || idx >= messages.Length ()) return NULL;
  csReporterMessage* msg = (csReporterMessage*)messages[idx];
  return msg->id;
}

const char* csReporter::GetMessageDescription (int idx) const
{
  csScopedMutexLock lock (mutex);
  if (idx < 0 || idx >= messages.Length ()) return NULL;
  csReporterMessage* msg = (csReporterMessage*)messages[idx];
  return msg->description;
}

void csReporter::AddReporterListener (iReporterListener* listener)
{
  csScopedMutexLock lock (mutex);
  listeners.Push (listener);
  listener->IncRef ();
}

void csReporter::RemoveReporterListener (iReporterListener* listener)
{
  csScopedMutexLock lock (mutex);
  int idx = listeners.Find (listener);
  if (idx != -1)
  {
    listeners.Delete (idx);
    listener->DecRef ();
  }
}

bool csReporter::FindReporterListener (iReporterListener* listener)
{
  csScopedMutexLock lock (mutex);
  int idx = listeners.Find (listener);
  return idx != -1;
}

csReporterMessage::~csReporterMessage ()
{
  delete[] id;
  delete[] description;
}

