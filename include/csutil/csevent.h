/*
    Crystal Space 3D engine: Event class interface
    Written by Andrew Zabolotny <bit@eltech.ru>, Jonathan Tarbox, 
      Frank Richter, Adam D. Bradley <artdodge@cs.bu.edu>

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

#include "csextern.h"

#include "csutil/hash.h"
#include "csutil/strset.h"
#include "csutil/scf_implementation.h"
#include "csutil/weakref.h"

#include "iutil/event.h"
#include "hashr.h"
#include "csendian.h"
#include "weakref.h"
#include "cseventq.h"
#include "strset.h"
#include "eventnames.h"

class csEventQueue;

/**\file
 * Implementation for iEvent
 */

class csEventAttributeIterator;
class csEvent;

/**
 * This class represents a system event.<p>
 * Events can be generated by hardware (keyboard, mouse)
 * as well as by software. There are so much constructors of
 * this class as much different types of events exists.
 */
class CS_CRYSTALSPACE_EXPORT csEvent : public scfImplementation1<csEvent, iEvent>
{
private:
  struct attribute
  {
    union
    {
      int64 intVal;
      double doubleVal;
      char* bufferVal;
      iBase* ibaseVal;
    };
    csEventAttributeType type;
    size_t dataSize;
    attribute (csEventAttributeType t) { type = t; }
    attribute (const attribute &o) 
    {
      type = o.type;
      ibaseVal = o.ibaseVal;
      dataSize = o.dataSize;
      if ((o.type == csEventAttrEvent) || (o.type == csEventAttriBase))
        ibaseVal->IncRef();
    }
    ~attribute () 
    { 
      if (type == csEventAttrDatabuffer) 
        delete[] bufferVal; 
      else if ((type == csEventAttrEvent) || (type == csEventAttriBase))
        ibaseVal->DecRef();
    }
  };
  csHash<attribute*, csStringID> attributes;
  friend class csEventAttributeIterator;

  size_t count;

  bool CheckForLoops(iEvent *current, iEvent *e);

  template <class T>
  bool InternalAddInt (const char* name, T value)
  {
    if (attributes.In (GetKeyID (name))) return false;
    attribute* object = new attribute (csEventAttrInt);	
    object->intVal = (int64)value;				
    attributes.Put (GetKeyID (name), object);				
    count++;							
    return true;						
  }

  template <class T>
  bool InternalAddUInt (const char* name, T value)
  {
    if (attributes.In (GetKeyID (name))) return false;
    attribute* object = new attribute (csEventAttrUInt);	
    object->intVal = (int64)value;				
    attributes.Put (GetKeyID (name), object);				
    count++;							
    return true;						
  }

  csEventError InternalReportMismatch (attribute* attr) const
  {
    switch (attr->type)
    {
      case csEventAttrInt:
	return csEventErrMismatchInt;
      case csEventAttrUInt:
	return csEventErrMismatchUInt;
      case csEventAttrFloat:
	return csEventErrMismatchFloat;
      case csEventAttrDatabuffer:
	return csEventErrMismatchBuffer;
      case csEventAttrEvent:
	return csEventErrMismatchEvent;
      case csEventAttriBase:
	return csEventErrMismatchIBase;
      default:
	break;
    }
    return csEventErrUhOhUnknown;
  }

  template <class T>
  csEventError InternalRetrieveInt (const char* name, T& value) const
  {								
    attribute* object = attributes.Get (GetKeyID (name), 0);
    if (!object) return csEventErrNotFound;
    if ((object->type == csEventAttrInt) || (object->type == csEventAttrUInt))
    {									
      value = (T)object->intVal;
      const T rangeMin = (T)(1 << (sizeof(T) * 8 - 1));
      const T rangeMax = ~rangeMin;
      if ((object->intVal < rangeMin) || (object->intVal > rangeMax))
	return csEventErrLossy;
      else
	return csEventErrNone;
    }
    else
    {
      return InternalReportMismatch (object);
    }
  }

  template <class T>
  csEventError InternalRetrieveUint (const char* name, T& value) const
  {								
    attribute* object = attributes.Get (GetKeyID (name), 0);
    if (!object) return csEventErrNotFound;
    if ((object->type == csEventAttrInt) || (object->type == csEventAttrUInt))
    {									
      value = (T)object->intVal;
      const T rangeMax = (T)~0;
      if ((uint64)object->intVal > rangeMax)
	return csEventErrLossy;
      else
	return csEventErrNone;							
    }
    else
    {
      return InternalReportMismatch (object);
    }
  }

  static char const* GetTypeName (csEventAttributeType t);
  static csStringID GetKeyID (const char* key);
  static const char* GetKeyName (csStringID id);

protected:
  virtual csRef<iEvent> CreateEvent();

public:
  /// Empty initializer
  csEvent ();

  /**
   * Cloning constructor.  Note that for command style events, this performs
   * only a shallow copy of the `Info' attribute.
   */
  csEvent (csEvent const&);

  /**
   * Basic constructor.
   */
  csEvent (csTicks iTime, csEventID iName, bool iBroadcast);

  /// Destructor
  virtual ~csEvent ();

  /// Return the event's name
  const csEventID GetName();

  /// Add a named parameter and typed value
#define CS_CSEVENT_ADDINT(type)					\
  virtual bool Add (const char* name, type value)		\
  { return InternalAddInt (name, value); }
  CS_CSEVENT_ADDINT(int8)
  CS_CSEVENT_ADDINT(int16)
  CS_CSEVENT_ADDINT(int32)
  CS_CSEVENT_ADDINT(int64)
#undef CS_CSEVENT_ADDINT
#define CS_CSEVENT_ADDUINT(type)				\
  virtual bool Add (const char* name, type value)		\
  { return InternalAddUInt (name, value); }
  CS_CSEVENT_ADDUINT(uint8)
  CS_CSEVENT_ADDUINT(uint16)
  CS_CSEVENT_ADDUINT(uint32)
  CS_CSEVENT_ADDUINT(uint64)
#undef CS_CSEVENT_ADDUINT
  virtual bool Add (const char *name, float v);
  virtual bool Add (const char *name, double v);
  virtual bool Add (const char *name, const char *v);
  virtual bool Add (const char *name, const void *v, size_t size);
  virtual bool Add (const char *name, bool v);
  virtual bool Add (const char *name, iEvent* v);
  virtual bool Add (const char *name, iBase* v);

  /// Find a named event for a given type.
#define CS_CSEVENT_FINDINT(T)						\
  virtual csEventError Retrieve (const char* name, T& value) const	\
  { return InternalRetrieveInt (name, value); }
  CS_CSEVENT_FINDINT(int8)
  CS_CSEVENT_FINDINT(int16)
  CS_CSEVENT_FINDINT(int32)
#undef CS_CSEVENT_FINDINT
  virtual csEventError Retrieve (const char* name, int64& value) const
  {								
    attribute* object = attributes.Get (GetKeyID (name), 0);
    if (!object) return csEventErrNotFound;
    if ((object->type == csEventAttrInt) || (object->type == csEventAttrUInt))
    {									
      value = object->intVal;
      return csEventErrNone;
    }
    else
    {
      return InternalReportMismatch (object);
    }
  }

#define CS_CSEVENT_FINDUINT(T)						\
  virtual csEventError Retrieve (const char* name, T& value) const	\
  { return InternalRetrieveUint (name, value); }
  CS_CSEVENT_FINDUINT(uint8)
  CS_CSEVENT_FINDUINT(uint16)
  CS_CSEVENT_FINDUINT(uint32)
#undef CS_CSEVENT_FINDUINT
  virtual csEventError Retrieve (const char* name, uint64& value) const
  {								
    attribute* object = attributes.Get (GetKeyID (name), 0);
    if (!object) return csEventErrNotFound;
    if ((object->type == csEventAttrInt) || (object->type == csEventAttrUInt))
    {									
      value = (uint64)object->intVal;
      return csEventErrNone;
    }
    else
    {
      return InternalReportMismatch (object);
    }
  }

  virtual csEventError Retrieve (const char *name, float &v) const;
  virtual csEventError Retrieve (const char *name, double &v) const;
  virtual csEventError Retrieve (const char *name, const char *&v) const;
  virtual csEventError Retrieve (const char *name, const void *&v,
  	size_t &size) const;
  virtual csEventError Retrieve (const char *name, bool &v) const;
  virtual csEventError Retrieve (const char *name, csRef<iEvent> &v) const;
  virtual csEventError Retrieve (const char *name, csRef<iBase> &v) const;

  virtual bool AttributeExists (const char* name);
  virtual csEventAttributeType GetAttributeType (const char* name);

  virtual bool Remove (const char *name);
  virtual bool RemoveAll ();

  virtual csRef<iEventAttributeIterator> GetAttributeIterator();

  virtual bool Print (int level = 0);

};

/**
 * This class is a system event designed for the pool system<p>
 * Due to the possibilities of networking traffic and other assorted
 * events traversing the event system, a more efficient method of
 * event creation was needed.  Thus, the event pool was born, and
 * there are the events that reside within it.
 */
class CS_CRYSTALSPACE_EXPORT csPoolEvent : public csEvent
{
  typedef csEvent superclass;
  friend class csEventQueue;
  friend class csEvent;

private:
  // As per the XML pool, keep a reference to the pool container obejct
  // and this also allows our overridden DecRef() to place the event back
  // into the pool when users are done with it.
  csWeakRef<csEventQueue> pool;

  // The next event in the pool, or null if the event is in use.
  csPoolEvent *next;

  // The 'real' DecRef() call that deletes the event, should in theory only be
  // called from csEventQueue.
  void Free () { csEvent::DecRef(); }

protected:
  virtual csRef<iEvent> CreateEvent();

public:
  /// The constructor; should only be called from within csEventQueue.
  csPoolEvent (csEventQueue *q);

  /// Places the event back into the pool if this is the last reference.
  virtual void DecRef ();
};

/**
 * \internal csEvent attribute iterator
 */
class csEventAttributeIterator : 
  public scfImplementation1<csEventAttributeIterator, iEventAttributeIterator>
{
  csHash<csEvent::attribute*, csStringID>::GlobalIterator iterator;								 
public:
  
  csEventAttributeIterator (
    csHash<csEvent::attribute*, csStringID>::GlobalIterator& iter) 
    : scfImplementationType (this),  iterator(iter)
  {
  }

  virtual ~csEventAttributeIterator()
  {
  }

  virtual bool HasNext()
  {
    return iterator.HasNext();
  }
  virtual const char* Next();
  virtual void Reset()
  {
    iterator.Reset();
  }
};

#endif // __CS_CSEVENT_H__
