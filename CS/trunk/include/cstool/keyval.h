/*
    Copyright (C) 2000 by Thomas Hieber

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

#ifndef __CS_KEYVAL_H__
#define __CS_KEYVAL_H__

#include "csextern.h"

#include "csgeom/vector3.h"
#include "csutil/csobject.h"
#include "ivaria/keyval.h"

/**
 * A Key Value pair. This object contains a 'key' string and a 'value' string.
 * The 'key' string is the same as the name of the object as returned from
 * the iObject.
 * <p>
 * One way to attach key value pairs to CS objects is to add the following
 * xml to your object: <key name="somename" value="somevalue" />
 */
class CS_CSTOOL_EXPORT csKeyValuePair : public csObject, public iKeyValuePair
{
public:
  /// The constructor. Requires both key and value. Data is being copied!
  csKeyValuePair (const char* Key, const char* Value);
  /// The destructor as usual
  virtual ~csKeyValuePair ();

  SCF_DECLARE_IBASE_EXT (csObject);
  //----------------------- iKeyValuePair --------------------------
  virtual iObject *QueryObject() { return (csObject*)this; }
  virtual const char *GetKey () const;
  virtual void SetKey (const char* s);
  virtual const char *GetValue () const;
  virtual void SetValue (const char* value);

private:
  char *m_Value;
};

#endif // __CS_KEYVAL_H__
