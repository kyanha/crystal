/*
    Crystal Space utility library: vector class interface
    Copyright (C) 1998,1999,2000 by Andrew Zabolotny <bit@eltech.ru>

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

#ifndef __CS_GARRAY_H__
#define __CS_GARRAY_H__

#include "array.h"

/**\file 
 * vector class interface
 */

/**
 * An automatically growing array of objects. The only difference with
 * csArray is that this class allows you to get the address of the internal
 * array. This is of course dangerous so use of this class should be
 * restricted.
 * <p>
 * The main use of this class is when you absolutely need access
 * to the internal array that is in this class. This can be useful
 * if you want to access some external module (like OpenGL).
 */
template <class T>
class csDirtyAccessArray : public csArray<T>
{
private:
  int RefCount;

public:
  /**
   * Initialize object to hold initially 'ilimit' elements, and increase
   * storage by 'ithreshold' each time the upper bound is exceeded.
   */
  csDirtyAccessArray (int ilimit = 0, int ithreshold = 0)
  	: csArray<T> (ilimit, ithreshold)
  {
    RefCount = 0;
  }

  // Reference counting.
  void IncRef () { RefCount++; }

  // Reference counting. Delete array when reference reaches 0.
  void DecRef ()
  {
    if (RefCount == 1) { DeleteAll (); }
    RefCount--;
  }

  /// Get the pointer to the start of the array.
  T* GetArray ()
  {
    if (Length () > 0)
      return &Get (0);
    else
      return 0;
  }

  /// Get the pointer to the start of the array.
  const T* GetArray () const
  {
    if (Length () > 0)
      return &Get (0);
    else
      return 0;
  }

  /**
   * Get a copy of the array. The caller is responsible for
   * deleting this with 'delete[]'. Returns 0 if there are no
   * items in the array.
   */
  T* GetArrayCopy ()
  {
    if (Length () > 0)
    {
      T* copy = new T [Length ()];
      memcpy (copy, &Get (0), sizeof (T) * Length ());
      return copy;
    }
    else
      return 0;
  }
};

#endif // __CS_GARRAY_H__

