/*
    Crystal Space: string vector class
    Copyright (C) 1998,1999,2000 by Andrew Zabolotny <bit@eltech.ru>

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

#include <string.h>
#include "cssysdef.h"
#include "csutil/csstrvec.h"

bool csStrVector::FreeItem (void* Item)
{
  delete [] (char *) Item;
  return true;
}

csStrVector::~csStrVector ()
{
  DeleteAll ();
}

int csStrVector::Compare (void* Item1, void* Item2, int Mode) const
{
  if (Mode == CASE_INSENSITIVE)
    return strcasecmp ((const char*)Item1, (const char*)Item2);
  else
    return strcmp ((const char*)Item1, (const char*)Item2);
}

int csStrVector::CompareKey (void* Item, const void* Key, int Mode) const
{
  if (Mode == CASE_INSENSITIVE)
    return strcasecmp ((const char*)Item, (const char*)Key);
  else
    return strcmp ((const char*)Item, (const char*)Key);
}
