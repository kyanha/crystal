/*
    Copyright (C) 1998 by Jorrit Tyberghein
    csObject library (C) 1999 by Ivan Avramovic <ivan@avramovic.com>

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

#ifndef __CS_DATAOBJ_H_
#define __CS_DATAOBJ_H_

#include "csutil/csobject.h"

/**
 * A generic data object.
 * This class contains a pointer to a generic, untyped block of data which
 * is not destroyed when the csObject is freed.  Users are encouraged to
 * implement their own custom csObjects, but this one can be used as a
 * general enough and simple data storage that can be added to any object.
 */
class csDataObject : public csObject
{
protected:
  /// Pointer to data.
  void* data;

public:
  /// Initialize this object with data pointer initialized to 'd'
  csDataObject (void *d) : csObject (), data (d)
  { SCF_CONSTRUCT_IBASE (0); SCF_CONSTRUCT_EMBEDDED_IBASE (scfiDataObject); }
  /// Get the data associated with this object
  void* GetData () const
  { return data; }
  /**
   * Get first data pointer associated with other object.
   */
  static void* GetData (iObject* obj)
  {
    csRef<iDataObject> d (CS_GET_CHILD_OBJECT (obj, iDataObject));
    void *res = (d ? d->GetData () : 0);
    return res;
  }

  SCF_DECLARE_IBASE_EXT (csObject);

  /// iDataObject implementation.
  struct DataObject : public iDataObject
  {
    SCF_DECLARE_EMBEDDED_IBASE (csDataObject);

    virtual iObject* QueryObject ()
    { return scfParent; }
    virtual void* GetData ()
    { return scfParent->GetData (); }
  } scfiDataObject;
};

#endif // __CS_DATAOBJ_H__
