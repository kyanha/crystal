/*
    Copyright (C) 2002 by Mathew Sutcliffe

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

#ifndef __CS_HASHMAPR_H__
#define __CS_HASHMAPR_H__

#include "csutil/hashmap.h"

class csHashIteratorReversible;

/// A csHashMap that maintains a reverse hash for indexing key strings by keys.
class csHashMapReversible : public csHashMap
{
  friend class csHashIteratorReversible;

  protected:
  csHashMap *Reverse;
  csHashMap *HalfReverse;

  public:
  csHashMapReversible (uint32 size = 211);

  virtual ~csHashMapReversible ();

  /// Put element into hash while implementing reversible features.
  /// Does csHashCompute automatically.
  void Put (const char *key, csHashObject object);

  /// Get the first original key string with the given value.
  const char* GetKey (csHashObject value) const;

  /// Get the first original key string with the given key number.
  const char* GetKey (csHashKey key) const;
};

/// A csHashIterator that knows about csHashMapReversible's reverse hash.
class csHashIteratorReversible : public csHashIterator
{
  friend class csHashMapReversible;

  private:
  csHashMapReversible *hashr;
  const char *iterr;
  csHashKey keyr;

  public:
  /// Construct an iterator over all elements of a reversible hash
  csHashIteratorReversible (csHashMapReversible *hash);

  /// Construct an iterator over specific elements of a reversible hash
  csHashIteratorReversible (csHashMapReversible *hash, csHashKey key);

  /// Construct an iterator over specific elements of a reversible hash
  csHashIteratorReversible (csHashMapReversible *hash, const char *key);

  /// Get next object, for iterators constructed from a string key
  csHashObject Next ();

  /// Get the key of the current value.
  const char* GetKey () const;
};

#endif // __CS_HASHMAPR_H__

