/*
    Crystal Space 3D engine: Event class interface
    Written by Andrew Zabolotny <bit@eltech.ru>

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

#ifndef __CS_CSEVENT_H__
#define __CS_CSEVENT_H__

#include "iutil/event.h"
#include "csutil/hashmapr.h"
#include "cssys/csendian.h"

enum {
  CS_DATATYPE_INT8 = 0x00,
  CS_DATATYPE_UINT8,
  CS_DATATYPE_INT16,
  CS_DATATYPE_UINT16,
  CS_DATATYPE_INT32,
  CS_DATATYPE_UINT32,
  CS_DATATYPE_INT64,
  CS_DATATYPE_UINT64,
  CS_DATATYPE_FLOAT,
  CS_DATATYPE_DOUBLE,
  CS_DATATYPE_BOOL,
  CS_DATATYPE_STRING,
  CS_DATATYPE_DATABUFFER,
  CS_DATATYPE_EVENT
};

/**
 * This class represents a system event.<p>
 * Events can be generated by hardware (keyboard, mouse)
 * as well as by software. There are so much constructors of
 * this class as much different types of events exists.
 */

class csEvent : public iEvent
{
private:
  csHashMapReversible attributes;
  
  uint32 count;

  bool CheckForLoops(csEvent *current, csEvent *e);

  bool FlattenCrystal(char *buffer);
  bool FlattenMuscle(char *buffer);
  bool FlattenXML(char *buffer);

  uint32 FlattenSizeCrystal();
  uint32 FlattenSizeMuscle();
  uint32 FlattenSizeXML();
  
  bool UnflattenCrystal(const char *buffer, uint32 length);
  bool UnflattenMuscle(const char *buffer, uint32 length);
  bool UnflattenXML(const char *buffer, uint32 length);
public:
  /// Empty initializer
  csEvent ();

  /**
   * Cloning constructor.  Note that for command style events, this performs
   * only a shallow copy of the `Info' attribute.
   */
  csEvent (csEvent const&);

  /// Create a keyboard event object
  csEvent (csTicks, int type, int kcode, int kchar, int modifiers);

  /// Create a mouse event object
  csEvent (csTicks, int type, int x, int y, int button, int modifiers);

  /// Create a joystick event object
  csEvent (csTicks, int type, int n, int x, int y, int button, int modifiers);

  /// Create a command event object
  csEvent (csTicks, int type, int code, void* info = NULL);

  /// Destructor
  virtual ~csEvent ();

  virtual bool Add(const char *name, int8 v);
  virtual bool Add(const char *name, uint8 v);
  virtual bool Add(const char *name, int16 v);
  virtual bool Add(const char *name, uint16 v);
  virtual bool Add(const char *name, int32 v, bool force_boolean = false);
  virtual bool Add(const char *name, uint32 v);
  virtual bool Add(const char *name, int64 v);
  virtual bool Add(const char *name, uint64 v);
  virtual bool Add(const char *name, float v);
  virtual bool Add(const char *name, double v);
  virtual bool Add(const char *name, char *v);
  virtual bool Add(const char *name, void *v, uint32 size);
#ifndef CS_USE_FAKE_BOOL_TYPE
  virtual bool Add(const char *name, bool v, bool force_boolean = true);
#endif
  virtual bool Add(const char *name, iEvent *v);

  virtual bool Find(const char *name, int8 &v, int index = 0);
  virtual bool Find(const char *name, uint8 &v, int index = 0);
  virtual bool Find(const char *name, int16 &v, int index = 0);
  virtual bool Find(const char *name, uint16 &v, int index = 0);
  virtual bool Find(const char *name, int32 &v, int index = 0);
  virtual bool Find(const char *name, uint32 &v, int index = 0);
  virtual bool Find(const char *name, int64 &v, int index = 0);
  virtual bool Find(const char *name, uint64 &v, int index = 0);
  virtual bool Find(const char *name, float &v, int index = 0);
  virtual bool Find(const char *name, double &v, int index = 0);
  virtual bool Find(const char *name, char **v, int index = 0);
  virtual bool Find(const char *name, void **v, uint32 &size, int index = 0);
#ifndef CS_USE_FAKE_BOOL_TYPE
  virtual bool Find(const char *name, bool &v, int index = 0);
#endif
  virtual bool Find(const char *name, iEvent **v, int index = 0);

  virtual bool Remove(const char *name, int index = -1);
  virtual bool RemoveAll();

  virtual uint32 FlattenSize(int format = CS_CRYSTAL_PROTOCOL);
  virtual bool Flatten(char *buffer, int format = CS_CRYSTAL_PROTOCOL);
  virtual bool Unflatten(const char *buffer, uint32 length);
  
  virtual bool Print(int level = 0);

  virtual bool Pooled();
  
  SCF_DECLARE_IBASE;
};

/**
 * This class is a system event designed for the pool system<p>
 * Due to the possibilities of networking traffic and other assorted
 * events traversing the event system, a more efficient method of
 * event creation was needed.  Thus, the event pool was born, and
 * there are the events that reside within it.
 */
class csPoolEvent : public csEvent
{
  // make csEventQueue and csEvent as a friend class
  friend class csEventQueue;
  friend class csEvent;

private:
  // As per the XML pool, keep a reference to the pool container obejct
  // and this also allows our overridden DecRef() to place the event back
  // into the pool when users are done with it.
  csRef<csEventQueue> pool;
 
  // The next event in the pool, or null if the event is in use.
  csPoolEvent *next;
 
  // The 'real' DecRef() call that deletes the event, should in theory only be 
  // called from the csEventQueue;
  void Free() { csEvent::DecRef(); }
 
public:
  /// The constructor, this should only be called from within the csEventQueue
  csPoolEvent(csEventQueue *q);

  virtual bool Pooled();
 
  /// The DecRef() that places the event back into the pool at a ref count of 1
  void DecRef();
};

#endif // __CS_CSEVENT_H__
