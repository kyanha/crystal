/*
    Copyright (C) 1998 by Jorrit Tyberghein
  
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

#ifndef CSCOLL_H
#define CSCOLL_H

#include "csgeom/matrix3.h"
#include "csobject/csobject.h"
#include "csutil/csvector.h"

class csSector;

/**
 * A collection object is for conveniance of the script language.
 * Objects are (currently) not really hierarchical in Crystal Space.
 * A collection simulates a hierarchy. The script can then perform
 * operations like 'move' and 'transform' on all the objects in
 * the collection together.
 */
class csCollection : public csObject
{
private:
  /// The list of objects contained in this csCollection.
  csVector objects;

public:
  /**
   * Create a new csCollection with the given name.
   */
  csCollection ();

  ///
  virtual ~csCollection () {}

  /**
   * Set the location of all objects in the collection.
   */
  void SetPosition (csSector* home, const csVector3& v);

  /**
   * Set the transformation matrix for all objects in the collection.
   */
  void SetTransform (const csMatrix3& matrix);

  /**
   * Relative move.
   */
  void MovePosition (const csVector3& v);

  /**
   * Relative transform.
   */
  void Transform (csMatrix3& matrix);

  /**
   * Really do the transformation and moving of all objects in
   * the collection. This should be called after calling any of
   * the other set_move, set_transform, move or transform calls.
   */
  void Transform ();

  /**
   * Find an object with the given name inside this collection.
   */
  csObject* FindObject (char* name);

  /**
   * Get the number of objects in this collection.
   */
  int GetNumObjects () const { return objects.Length(); }

  /// Add an object to the collection.
  void AddObject (csObject* obj) { objects.Push((csSome)obj); }

  ///
  csObject* operator[] (int i) { return (csObject*) (objects[i]); }
  
  CSOBJTYPE;
};

#endif /*CSCOLL_H*/
