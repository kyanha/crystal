/*
    Copyright (C) 2000-2001 by Jorrit Tyberghein

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
#include "qint.h"
#include "csengine/movable.h"
#include "csengine/sector.h"
#include "csengine/cscoll.h"
#include "csengine/engine.h"

//---------------------------------------------------------------------------
SCF_IMPLEMENT_IBASE(csMovableSectorList)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iSectorList)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csMovableSectorList::SectorList)
  SCF_IMPLEMENTS_INTERFACE(iSectorList)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csMovableSectorList::csMovableSectorList ()
{
  SCF_CONSTRUCT_IBASE (0);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSectorList);
  movable = 0;
}

csMovableSectorList::~csMovableSectorList ()
{
  DeleteAll ();
}

bool csMovableSectorList::PrepareSector (iSector* sector)
{
  // Check for a valid item.
  if (sector == 0) return false;

  // if the movable has a parent, no sectors can be added.
  CS_ASSERT (movable != 0);
  if (movable->GetParent ()) return false;

  csMeshWrapper *mw = movable->GetMeshWrapper ();
  if (mw) mw->MoveToSector (sector);
  return true;
}

int csMovableSectorList::SectorList::GetCount () const
{
  return scfParent->Length ();
}

iSector *csMovableSectorList::SectorList::Get (int n) const
{
  return scfParent->Get (n);
}

int csMovableSectorList::SectorList::Add (iSector *obj)
{
  if (!scfParent->PrepareSector (obj)) return -1;
  return scfParent->Push (obj);
}

bool csMovableSectorList::SectorList::Remove (iSector *obj)
{
  return scfParent->Delete (obj);
}

bool csMovableSectorList::SectorList::Remove (int n)
{
  return scfParent->DeleteIndex (n);
}

void csMovableSectorList::SectorList::RemoveAll ()
{
  scfParent->DeleteAll ();
}

int csMovableSectorList::SectorList::Find (iSector *obj) const
{
  return scfParent->Find (obj);
}

iSector *csMovableSectorList::SectorList::FindByName (const char *Name) const
{
  return scfParent->FindByName (Name);
}

//---------------------------------------------------------------------------
SCF_IMPLEMENT_IBASE(csMovable)
  SCF_IMPLEMENTS_INTERFACE(iBase)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iMovable)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csMovable::eiMovable)
  SCF_IMPLEMENTS_INTERFACE(iMovable)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csMovable::csMovable ()
{
  SCF_CONSTRUCT_IBASE (0);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiMovable);
  parent = 0;
  object = 0;
  updatenr = 0;
  sectors.SetMovable (this);
  is_identity = true;
}

csMovable::~csMovable ()
{
  int i = listeners.Length ()-1;
  while (i >= 0)
  {
    iMovableListener *ml = listeners[i];
    ml->MovableDestroyed (&scfiMovable);
    i--;
  }
}

void csMovable::SetPosition (iSector *home, const csVector3 &pos)
{
  obj.SetOrigin (pos);
  SetSector (home);
}

void csMovable::SetTransform (const csMatrix3 &matrix)
{
  obj.SetT2O (matrix);
}

void csMovable::MovePosition (const csVector3 &rel)
{
  obj.Translate (rel);
}

void csMovable::Transform (const csMatrix3 &matrix)
{
  obj.SetT2O (matrix * obj.GetT2O ());
}

void csMovable::SetSector (iSector *sector)
{
  if (sectors.Length () == 1 && sector == sectors[0]) return ;
  ClearSectors ();
  if (sectors.PrepareSector (sector))
    sectors.Push (sector);
}

void csMovable::ClearSectors ()
{
  if (parent == 0)
  {
    if (object) object->RemoveFromSectors ();
    sectors.DeleteAll ();
    sectors.SetLength (0);
  }
}

void csMovable::AddListener (iMovableListener *listener)
{
  RemoveListener (listener);
  listeners.Push (listener);
}

void csMovable::RemoveListener (iMovableListener *listener)
{
  listeners.Delete (listener);
}

void csMovable::UpdateMove ()
{
  updatenr++;
  is_identity = obj.IsIdentity ();

  if (object) object->UpdateMove ();

  int i = listeners.Length ()-1;
  while (i >= 0)
  {
    iMovableListener *ml = listeners[i];
    ml->MovableChanged (&scfiMovable);
    i--;
  }
}

csReversibleTransform csMovable::GetFullTransform () const
{
  if (parent == 0)
    return GetTransform ();
  else if (is_identity)
    return parent->GetFullTransform ();
  else
    return GetTransform () * parent->GetFullTransform ();
}

//--------------------------------------------------------------------------
iMovable *csMovable::eiMovable::GetParent () const
{
  return scfParent->parent;
}

void csMovable::eiMovable::SetSector (iSector *sector)
{
  scfParent->SetSector (sector);
}

void csMovable::eiMovable::ClearSectors ()
{
  scfParent->ClearSectors ();
}

iSectorList *csMovable::eiMovable::GetSectors ()
{
  return scfParent->GetSectors ();
}

bool csMovable::eiMovable::InSector () const
{
  return scfParent->InSector ();
}

void csMovable::eiMovable::SetPosition (iSector *home, const csVector3 &v)
{
  scfParent->SetPosition (home, v);
}

void csMovable::eiMovable::SetPosition (const csVector3 &v)
{
  scfParent->SetPosition (v);
}

const csVector3 &csMovable::eiMovable::GetPosition () const
{
  return scfParent->GetPosition ();
}

const csVector3 csMovable::eiMovable::GetFullPosition () const
{
  return scfParent->GetFullPosition ();
}

void csMovable::eiMovable::SetTransform (const csMatrix3 &matrix)
{
  scfParent->SetTransform (matrix);
}

void csMovable::eiMovable::SetTransform (const csReversibleTransform &t)
{
  scfParent->SetTransform (t);
}

csReversibleTransform &csMovable::eiMovable::GetTransform ()
{
  return scfParent->GetTransform ();
}

csReversibleTransform csMovable::eiMovable::GetFullTransform () const
{
  return scfParent->GetFullTransform ();
}

void csMovable::eiMovable::MovePosition (const csVector3 &v)
{
  scfParent->MovePosition (v);
}

void csMovable::eiMovable::Transform (const csMatrix3 &matrix)
{
  scfParent->Transform (matrix);
}

void csMovable::eiMovable::AddListener (
  iMovableListener *listener)
{
  scfParent->AddListener (listener);
}

void csMovable::eiMovable::RemoveListener (iMovableListener *listener)
{
  scfParent->RemoveListener (listener);
}

void csMovable::eiMovable::UpdateMove ()
{
  scfParent->UpdateMove ();
}

long csMovable::eiMovable::GetUpdateNumber () const
{
  return scfParent->GetUpdateNumber ();
}
