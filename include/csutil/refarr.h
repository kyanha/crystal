/*
  Crystal Space Smart Pointers
  Copyright (C) 2002 by Jorrit Tyberghein and Matthias Braun

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

#ifndef __CS_REFARR_H__
#define __CS_REFARR_H__

#include "ref.h"
#include "array.h"

#ifdef CS_REF_TRACKER
 #include <typeinfo>
 #include "csutil/reftrackeraccess.h"

 #define CSREFARR_TRACK(x, cmd, refCount, obj, tag) \
  {						    \
    const int rc = obj ? refCount : -1;		    \
    if (obj) cmd;				    \
    if (obj)					    \
    {						    \
      csRefTrackerAccess::SetDescription (obj,	    \
	typeid(T).name());			    \
      csRefTrackerAccess::Match ## x (obj, rc, tag);\
    }						    \
  }
 #define CSREFARR_TRACK_INCREF(obj,tag)	\
  CSREFARR_TRACK(IncRef, obj->IncRef(), obj->GetRefCount(), obj, tag);
 #define CSREFARR_TRACK_DECREF(obj,tag)	\
  CSREFARR_TRACK(DecRef, obj->DecRef(), obj->GetRefCount(), obj, tag);
#else
 #define CSREFARR_TRACK_INCREF(obj,tag) \
  if (obj) obj->IncRef();
 #define CSREFARR_TRACK_DECREF(obj,tag) \
  if (obj) obj->DecRef();
#endif

template <class T>
class csRefArrayElementHandler
{
public:
  static void Construct (T* address, T const& src)
  {
    *address = src;
    CSREFARR_TRACK_INCREF (src, address);
  }

  static void Destroy (T* address)
  {
    CSREFARR_TRACK_DECREF ((*address), address);
  }

  static void InitRegion (T* address, int count)
  {
    memset (address, 0, count*sizeof (T));
  }
};

/**
 * An array of smart pointers.
 */
template <class T>
class csRefArray : public csArray<T*, csRefArrayElementHandler<T*> >
{
public:
  /**
   * Initialize object to hold initially 'ilimit' elements, and increase
   * storage by 'ithreshold' each time the upper bound is exceeded.
   */
  csRefArray (int ilimit = 0, int ithreshold = 0)
  	: csArray<T*, csRefArrayElementHandler<T*> > (ilimit, ithreshold)
  {
  }

  /// Pop an element from tail end of array.
  csPtr<T> Pop ()
  {
    CS_ASSERT (Length () > 0);
    csRef<T> ret = Get (Length () - 1);
    SetLength (Length () - 1);
    return csPtr<T> (ret);
  }

};

#undef CSREFARR_TRACK_INCREF
#undef CSREFARR_TRACK_DECREF

#endif // __CS_REFARR_H__

