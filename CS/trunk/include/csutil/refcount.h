/*
    Crystal Space Reference Counting Interface
    Copyright (C) 2002 by Jorrit Tyberghein

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

#ifndef __CS_REFCOUNT_H__
#define __CS_REFCOUNT_H__

/**
 * This is a class which provides basic reference-counting semantics.  It can
 * be used in conjunction with the smart pointer template class csRef (see
 * <csutil/ref.h>).  This class itself provides no functionality beyond
 * reference counting.  It is intended that you should subclass csRef and add
 * needed functionality.
 */
class csRefCount
{
protected:
  int ref_count;

  // To avoid a problem with MSVC and multiple DLLs (with seperate memory
  // space) we have to use a virtual destructor.
  // @@@ Another alternative is to force this function to be non-inline, as
  // doing so will most likely achieve the same result.
  virtual void Delete ()
  {
    delete this;
  }

  virtual ~csRefCount () {}

public:
  /// Initialize object and set reference to 1.
  csRefCount () : ref_count (1) {}

  /// Increase the number of references to this object.
  void IncRef () { ref_count++; }
  /// Decrease the number of references to this object.
  void DecRef ()
  {
    ref_count--;
    if (ref_count <= 0)
      Delete ();
  }
  /// Get the reference count (only for debugging).
  int GetRefCount () const { return ref_count; }
};

#endif // __CS_REFCOUNT_H__
