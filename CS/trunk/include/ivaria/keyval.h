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

#ifndef __CS_IVARIA_KEYVAL_H__
#define __CS_IVARIA_KEYVAL_H__

#include "csutil/scf.h"
#include "csutil/array.h"

struct iObject;
struct iSector;

SCF_VERSION (iKeyValuePair, 0, 0, 3);

/**
 * A Key Value pair. This object contains a 'key' string and one or more
 * 'value' strings.  Typically key value pairs are specified in map files
 * (using the \<key\> tag).  They allow a game developer to tag game specific
 * information to any Crystal Space object.
 * <p>
 * Main creators of instances implementing this interface:
 *   <ul>
 *   <li>The main loader creates instances of this internally.
 *   </ul>
 * Main ways to get pointers to this interface:
 *   <ul>
 *   <li>SCF_QUERY_INTERFACE() from iObject instances you get by
 *       calling iObject::GetIterator() on iObject instances you
 *       get from CS objects (typically using QueryObject()).
 *   </ul>
 */
struct iKeyValuePair : public iBase
{
  /// Get the iObject.
  virtual iObject *QueryObject() = 0;

  /// Get the key string of the pair.
  virtual const char *GetKey () const = 0;

  /// Set the key string of the pair
  virtual void SetKey (const char* key) = 0;

  /**
   * Get a value string from the pair.
   */
  virtual const char *GetValue (const char* vname) const = 0;

  /**
   * Get the 'value' string of the pair. This
   * is the same as calling 'GetValue ("value")'.
   */
  virtual const char *GetValue () const = 0;

  /**
   * Set a value string of the pair.
   */
  virtual void SetValue (const char* vname, const char* value) = 0;

  /**
   * Set the value string of the pair. This
   * is the same as calling 'SetValue ("value", value)'.
   */
  virtual void SetValue (const char* value) = 0;
};

#endif // __CS_IVARIA_KEYVAL_H__
