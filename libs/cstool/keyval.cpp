/*
    Copyright (C) 2001 by Jorrit Tyberghein
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

#include "cssysdef.h"
#include "cstool/keyval.h"

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE_EXT (csKeyValuePair)
  SCF_IMPLEMENTS_INTERFACE (iKeyValuePair)
SCF_IMPLEMENT_IBASE_EXT_END

csKeyValuePair::csKeyValuePair (const char* Key, const char* Value)
{
  SetName (Key);
  SetValue (Value);
}

csKeyValuePair::csKeyValuePair (const char* Key)
{
  SetName (Key);
  m_Value = 0;
}

csKeyValuePair::~csKeyValuePair ()
{
}

const char *csKeyValuePair::GetKey () const
{
  return GetName ();
}

void csKeyValuePair::SetKey (const char *s)
{
  SetName (s);
}

const char *csKeyValuePair::GetValue (const char* vname) const
{
  return values.Get (vname, 0);
}

const char *csKeyValuePair::GetValue () const
{
  return m_Value;
}

void csKeyValuePair::SetValue (const char* value)
{
  values.Put ("value", value);
  m_Value = values.Get ("value", 0);
}

void csKeyValuePair::SetValue (const char* vname, const char* value)
{
  if (!strcmp (vname, "value"))
    SetValue (value);
  else
  {
    values.Put (vname, value);
  }
}

