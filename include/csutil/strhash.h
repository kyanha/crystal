/*
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

#ifndef __CS_STRHASH_H__
#define __CS_STRHASH_H__

#include "csextern.h"
#include "hash.h"
#include "hashhandlers.h"
#include "iutil/strset.h"

/**\file
 * String-to-ID hash table
 */
 
class csStringHashIterator;

/**
 * A string-to-ID hash table.  Useful when you need to work with strings but
 * want the performance characteristics of simple numeric comparisons.
 * Register a string with a unique numeric ID and then compare ID's rather than
 * comparing strings.  You can fetch a string's ID via Request().
 * \sa csStringSet
 */
class CS_CRYSTALSPACE_EXPORT csStringHash
{
private:
  friend class csStringHashIterator;

  struct csRegisteredString
  {
    typedef csStringID IDtype;
    IDtype ID;
    //char String[];

    static csRegisteredString* Alloc (const char* str);
    static void Free (csRegisteredString* regStr);
    const char* GetString() const { return ((char*)this) + sizeof (IDtype); }
  };
  
  csHash<csRegisteredString*, const char*, csConstCharHashKeyHandler> Registry;

public:
  /// Constructor
  csStringHash (int size = 23);
  /// Destructor
  ~csStringHash ();

  /**
   * Register a string with an ID.
   * \param s The string with which to associate the ID.
   * \param id A numeric value with which to identify this string.
   * \return A pointer to the copy of the string in this hash.
   * \remarks If the string is already registered with a different ID, the old
   *   ID will be replaced with the one specified here. If you would like the
   *   convenience of having the ID assigned automatically, then consider using
   *   csStringSet, instead.
   */
  const char* Register (const char* s, csStringID id);

  /**
   * Request the ID for the given string.
   * \return The string's ID or csInvalidStringID if the string has not yet
   *   been registered.
   */
  csStringID Request (const char* s) const;

  /**
   * Request the string for a given ID.
   * \return The string associated with the given ID, or the null pointer if
   *   the string has not yet been registered.
   * \remarks This operation is relatively slow.  If you find that you must
   *   perform this reverse lookup frequently, then consider using csStringSet,
   *   instead.
   */
  const char* Request (csStringID id) const;

  /**
   * Delete all stored strings.
   */
  void Clear ();
};

/**
 * An iterator to iterate over elements in a csStringHash.
 * When you have an open iterator you should not alter the
 * string hash that this object iterates over. 
 */
class CS_CRYSTALSPACE_EXPORT csStringHashIterator
{
  friend class csStringHash;

private:
  //csGlobalHashIterator* hashIt;
  csHash<csStringHash::csRegisteredString*, const char*, 
    csConstCharHashKeyHandler>::GlobalIterator hashIt;

public:

  /**
   * Construct an iterator to iterate over all elements in the string hash.
   * \remarks You should not make changes to the hashmap when you have open
   *   iterators.
   */
  csStringHashIterator (csStringHash*);
  /// Destructor.
  virtual ~csStringHashIterator ();

  /// Is there a next element in this iterator?
  bool HasNext ();
  /// Get the next element.
  csStringID Next ();
};

#endif // __CS_STRHASH_H__
