/*
    Copyright (C) 2000 by Jorrit Tyberghein

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
#include "csengine/thing.h"
#include "csengine/cssprite.h"
#include "isector.h"

//---------------------------------------------------------------------------

IMPLEMENT_IBASE (csMovable)
  IMPLEMENTS_INTERFACE (iBase)
  IMPLEMENTS_EMBEDDED_INTERFACE (iMovable)
IMPLEMENT_IBASE_END

IMPLEMENT_EMBEDDED_IBASE (csMovable::eiMovable)
  IMPLEMENTS_INTERFACE (iMovable)
IMPLEMENT_EMBEDDED_IBASE_END


csMovable::csMovable ()
{
  CONSTRUCT_IBASE (NULL);
  CONSTRUCT_EMBEDDED_IBASE (scfiMovable);
  parent = NULL;
}

csMovable::~csMovable ()
{
}

void csMovable::SetPosition (csSector* home, const csVector3& pos)
{
  obj.SetOrigin (pos);
  SetSector (home);
}

void csMovable::SetTransform (const csMatrix3& matrix)
{
  obj.SetT2O (matrix);
}

void csMovable::MovePosition (const csVector3& rel)
{
  obj.Translate (rel);
}

void csMovable::Transform (csMatrix3& matrix)
{
  obj.SetT2O (matrix * obj.GetT2O ());
}

void csMovable::ClearSectors ()
{
  if (parent == NULL)
  {
    if (object->GetType () >= csThing::Type)
    {
      csThing* th = (csThing*)object;
      th->RemoveFromSectors ();
    }
    else
    {
      csSprite* sp = (csSprite*)object;
      sp->RemoveFromSectors ();
    }
    sectors.SetLength (0);
  }
}

void csMovable::AddSector (csSector* sector)
{
  if (parent == NULL)
  {
    sectors.Push (sector);
    if (object->GetType () >= csThing::Type)
    {
      csThing* th = (csThing*)object;
      th->MoveToSector (sector);
    }
    else
    {
      csSprite* sp = (csSprite*)object;
      sp->MoveToSector (sector);
    }
  }
}

void csMovable::UpdateMove ()
{
  if (object->GetType () >= csThing::Type)
  {
    csThing* th = (csThing*)object;
    th->UpdateMove ();
  }
  else
  {
    csSprite* sp = (csSprite*)object;
    sp->UpdateMove ();
  }
}

//--------------------------------------------------------------------------

iMovable* csMovable::eiMovable::GetParent ()
{
  csMovable* par = scfParent->GetParent ();
  if (!par) return NULL;
  return QUERY_INTERFACE (par, iMovable);
}

void csMovable::eiMovable::SetSector (iSector* sector)
{
  scfParent->SetSector (sector->GetPrivateObject ());
}

void csMovable::eiMovable::AddSector (iSector* sector)
{
  scfParent->AddSector (sector->GetPrivateObject ());
}

void csMovable::eiMovable::SetPosition (iSector* home, const csVector3& v)
{
  scfParent->SetPosition (home->GetPrivateObject (), v);
}

iSector* csMovable::eiMovable::GetSector (int idx)
{
  csSector* sect = (csSector*)scfParent->GetSectors ()[idx];
  return QUERY_INTERFACE (sect, iSector);
}

