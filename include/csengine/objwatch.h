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

#ifndef __CS_OBJWATCH_H__
#define __CS_OBJWATCH_H__

#include "csutil/refarr.h"
#include "csutil/parray.h"
#include "iengine/objwatch.h"

struct iLight;
struct iMovable;
class csLightCallback;
class csMovableListener;

/**
 * This class implements iObjectWatcher and is capable of keeping
 * track of lights and movables.
 */
class csObjectWatcher : public iObjectWatcher
{
private:
  // Lights we are watching.
  csPArray<iLight> lights;
  // Movables we are watching.
  csPArray<iMovable> movables;
  // Our light listener.
  csLightCallback* light_callback;
  // Our movable listener.
  csMovableListener* movable_listener;
  // Listeners that are listening to this object watcher.
  csRefArray<iObjectWatcherListener> listeners;

  // Update number.
  long updatenr;

  // Last operation.
  int last_op;
  // Last light (or NULL if last operation has nothing to do with lights).
  iLight* last_light;
  // Last movable (or NULL if last operation has nothing to do with movables).
  iMovable* last_movable;

public:
  /**
   * Create a default movable.
   */
  csObjectWatcher ();

  /// Destructor.
  virtual ~csObjectWatcher ();

  void ReportOperation (int op, iMovable* movable, iLight* light);

  SCF_DECLARE_IBASE;

  virtual void WatchLight (iLight* light);
  virtual void RemoveLight (iLight* light);
  virtual int GetWatchedLightCount () const { return lights.Length (); }
  virtual iLight* GetLight (int idx);

  virtual void WatchMovable (iMovable* movable);
  virtual void RemoveMovable (iMovable* movable);
  virtual int GetWatchedMovableCount () const { return movables.Length (); }
  virtual iMovable* GetMovable (int idx);

  virtual void Reset ();

  virtual uint32 GetWatchNumber () const { return updatenr; }
  virtual int GetLastOperation () const { return last_op; }
  virtual iLight* GetLastLight () const { return last_light; }
  virtual iMovable* GetLastMovable () const { return last_movable; }

  virtual void AddListener (iObjectWatcherListener* cb)
  {
    listeners.Push (cb);
  }

  virtual void RemoveListener (iObjectWatcherListener* cb)
  {
    listeners.Delete (cb);
  }
};

#endif // __CS_OBJWATCH_H__

