/*
    Crystal Space String Array SCF interface
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

#ifndef __CS_SCFSTRINGARRAY_H__
#define __CS_SCFSTRINGARRAY_H__

#include "iutil/stringarray.h"
#include "csutil/stringarray.h"

/// This class is a thin wrapper around csStringArray with SCF capability
class scfStringArray : public iStringArray
{
  csStringArray v;

public:
  SCF_DECLARE_IBASE;

  /// Create a iStringArray from scratch.
  scfStringArray (int iLimit = 16, int iDelta = 16) : v (iLimit, iDelta)
  { SCF_CONSTRUCT_IBASE (0); }

  /// Destructor - nothing to do.
  virtual ~scfStringArray ()
  { }

  /// Query array length.
  virtual int Length () const
  {
    return v.Length ();
  }

  /// Push a string onto the stack.
  virtual void Push (char const *value)
  {
    v.Push ((char*)value);
  }

  /// Pop a string from the top of stack.
  virtual char *Pop ()
  {
    return v.Pop ();
  }

  /// Get Nth string in vector.
  virtual char const *Get (int n) const
  {
    return v.Get (n);
  }

  /// Find index of given string pointer.
  virtual int Find (const char *value) const
  {
    return v.Find ((char*)value);
  }

  /// Find index of given string contents.
  virtual int FindContent (const char *value) const
  {
    return v.FindContent ((char*)value);
  }

  /// Case insensitive version of FindContent().
  virtual int FindCaseContent (const char *value) const
  {
    return v.FindCaseContent ((char*)value);
  }

  /// Find index of a string in a pre-sorted string array.
  virtual int FindSortedKey (const char *value) const
  {
    return v.FindSortedKey ((char*)value);
  }

  /// Sort the string array.
  virtual void Sort ()
  {
    v.Sort ();
  }

  /// Delete Nth string in the array.
  virtual bool DeleteIndex (int n)
  {
    return v.DeleteIndex (n);
  }

  /// Insert a string before Nth string in the array.
  virtual bool Insert (int n, char const *value)
  {
    return v.Insert (n, (char*)value);
  }

  /// Delete all strings in array.
  virtual void DeleteAll ()
  {
    v.DeleteAll ();
  }
};

#endif // __CS_SCFSTRINGARRAY_H__

