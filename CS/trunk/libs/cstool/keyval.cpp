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
#include "iengine/sector.h"

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE_EXT (csKeyValuePair)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iKeyValuePair)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csKeyValuePair::KeyValuePair)
  SCF_IMPLEMENTS_INTERFACE (iKeyValuePair)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csKeyValuePair::csKeyValuePair (const char* Key, const char* Value)
{
  SCF_CONSTRUCT_IBASE (0);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiKeyValuePair);
  SetName (Key);
  m_Value = csStrNew (Value);
}

csKeyValuePair::~csKeyValuePair ()
{
  delete [] m_Value;
}

const char *csKeyValuePair::GetKey () const
{
  return GetName ();
}

void csKeyValuePair::SetKey (const char *s)
{
  SetName (s);
}

const char *csKeyValuePair::GetValue () const
{
  return m_Value;
}

void csKeyValuePair::SetValue (const char* value)
{
  delete[] m_Value;
  m_Value = csStrNew (value);
}

