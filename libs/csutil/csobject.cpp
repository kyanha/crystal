/*
    Copyright (C) 1998-2001 by Jorrit Tyberghein
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

#include "cssysdef.h"
#include "csutil/csobject.h"
#include "csutil/dataobj.h"

#include <stdlib.h>
#include <string.h>

/*** Object Iterators ***/
class csObjectIterator : public iObjectIterator
{
public:
  SCF_DECLARE_IBASE;
  csRef<csObject> object;
  int position;

  csObjectIterator (csObject *obj) 
      : object (obj)
  {
    SCF_CONSTRUCT_IBASE (0);
    Reset ();
  }
  virtual ~csObjectIterator ()
  {
  }

  virtual bool HasNext() const
  {
    if (object->Children == 0 || position >= object->Children->Length())
      return false;

    return true;
  }

  virtual iObject* Next()
  {
    if (object->Children == 0 || position >= object->Children->Length())
      return 0;

    iObject* result = object->Children->Get(position);
    position++;
    return result;
  }
  virtual void Reset()
  {
    position = 0;
  }
  virtual iObject* GetParentObj() const
  {
    return object;
  }
  virtual iObject* FindName (const char* name)
  {
    while (HasNext ())
    {
      iObject* o = Next ();
      if (strcmp (o->GetName (), name) == 0)
        return o;
    }
    return 0;
  }
};

SCF_IMPLEMENT_IBASE (csObjectIterator)
  SCF_IMPLEMENTS_INTERFACE (iObjectIterator)
SCF_IMPLEMENT_IBASE_END

/*** csObject itself ***/

SCF_IMPLEMENT_IBASE (csObject)
  SCF_IMPLEMENTS_INTERFACE (iObject)
SCF_IMPLEMENT_IBASE_END

#include "csutil/debug.h"

void csObject::InitializeObject ()
{
  static CS_ID id = 0;
  csid = id++;
  ParentObject = 0;
  DG_ADDI (this, 0);
  DG_TYPE (this, "csObject");
}

csObject::csObject (iBase* pParent) : Children (0), Name (0)
{
  SCF_CONSTRUCT_IBASE (pParent);
  InitializeObject ();
}

csObject::csObject (csObject &o) : iObject(), Children (0), Name (0)
{
  SCF_CONSTRUCT_IBASE (0);
  InitializeObject ();

  csRef<iObjectIterator> it (o.GetIterator ());
  while (it->HasNext ())
  {
    ObjAdd (it->Next ());
  }
  SetName (o.GetName ());
}

csObject::~csObject ()
{
  ObjRemoveAll ();

  DG_REM (this);

  if (Children) { delete Children; Children = 0; }
  delete [] Name; Name = 0;

  /*
   * @@@ This should not be required for two reasons:
   * 1. If the parent keeps a pointer to this object, then the pointer was
   *    IncRef'ed, so this object cannot be deleted. Removing the object from
   *    its parent from here is only needed if the object was illegally
   *    deleted, not DecRef'ed.
   * 2. Several objects could contain this object as a child. The 'parent'
   *    pointer is not a safe way to find out which object contains this
   *    object as a child.
   */
  if (ParentObject)
  {
    DG_REMCHILD (ParentObject, this);
    DG_REMPARENT (this, ParentObject);
    ParentObject->ObjReleaseOld (this);
  }
}

void csObject::SetName (const char *iName)
{
  delete [] Name;
  Name = csStrNew (iName);
  DG_DESCRIBE1 (this, "%s", Name);
}

const char *csObject::GetName () const
{
  return Name;
}

CS_ID csObject::GetID () const
{
  return csid;
}

iObject* csObject::GetObjectParent () const
{
  return ParentObject;
}

void csObject::SetObjectParent (iObject *obj)
{
  ParentObject = obj;
}

void csObject::ObjAdd (iObject *obj)
{
  if (!obj)
    return;

  if (!Children)
    Children = new csObjectContainer ();

  obj->SetObjectParent (this);
  DG_ADDPARENT (obj, this);
  Children->Push (obj);
  DG_ADDCHILD (this, obj);
}

void csObject::ObjRemove (iObject *obj)
{
  if (!Children || !obj)
    return;

  int n = Children->Find (obj);
  if (n>=0)
  {
    obj->SetObjectParent (0);
    DG_REMPARENT (obj, this);
    DG_REMCHILD (this, obj);
    Children->Delete (n);
  }
}

void csObject::ObjReleaseOld (iObject *obj)
{
  if (!Children || !obj)
    return;

  int n = Children->Find (obj);
  if (n>=0)
  {
    obj->SetObjectParent (0);
    // @@@ WARNING! Doing only one DecRef() here does not prevent a second
    // deletion of 'obj'.  Keep in mind that we are currently executing
    // in the destructor of 'obj' itself. If only one 'IncRef()' is used
    // then the Delete() from the children vector will simply destroy the
    // object again (with bad consequences). Doing two IncRef()'s is a
    // solution for this and it doesn't prevent deletion of the object
    // since it is being deleted already.
    obj->IncRef ();
    obj->IncRef ();
    Children->Delete (n);
  }
}

void csObject::ObjRemoveAll ()
{
  if (!Children)
    return;

  int i;
  for (i=Children->Length ()-1; i>=0; i--)
  {
    iObject* child = Children->Get (i);
    child->SetObjectParent (0);
    DG_REMPARENT (child, this);
    DG_REMCHILD (this, child);
    Children->Delete (i);
  }
}

void csObject::ObjAddChildren (iObject *Parent)
{
  csRef<iObjectIterator> it (Parent->GetIterator ());
  while (it->HasNext ())
  {
    ObjAdd (it->Next ());
  }
}

void* csObject::GetChild (int InterfaceID, int Version,
	const char *Name, bool fn) const
{
  if (!Children)
    return 0;

  if (fn)
  {
    iObject *obj = GetChild (Name);
    return obj ? obj->QueryInterface (InterfaceID, Version) : 0;
  }

  int i;
  for (i = 0; i < Children->Length (); i++)
  {
    if (Name)
    {
      const char *OtherName = Children->Get (i)->GetName ();
      if (!OtherName) continue;
      if (strcmp(OtherName, Name)) continue;
    }

    void *obj = Children->Get (i)->QueryInterface (InterfaceID, Version);
    if (obj) return obj;
  }

  return 0;
}

iObject* csObject::GetChild (const char *Name) const
{
  if (!Children || !Name)
    return 0;

  int i;
  for (i = 0; i < Children->Length (); i++)
  {
    if (!strcmp (Children->Get (i)->GetName (), Name))
      return Children->Get (i);
  }
  return 0;
}

csPtr<iObjectIterator> csObject::GetIterator ()
{
  return csPtr<iObjectIterator> (new csObjectIterator (this));
}

//------------------- miscelaneous simple classes derived from csObject -----//

SCF_IMPLEMENT_IBASE_EXT (csDataObject)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iDataObject)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csDataObject::DataObject)
  SCF_IMPLEMENTS_INTERFACE (iDataObject)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

