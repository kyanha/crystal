/*
    Copyright (C) 2003 by Jorrit Tyberghein
	      (C) 2003 by Frank Richter

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

#include "cstool/basetexfact.h"

SCF_IMPLEMENT_IBASE (csBaseTextureFactory)
  SCF_IMPLEMENTS_INTERFACE (iTextureFactory)
SCF_IMPLEMENT_IBASE_END

csBaseTextureFactory::csBaseTextureFactory (iTextureType* parent, 
					    iObjectRegistry* object_reg)
{
  SCF_CONSTRUCT_IBASE (parent);
  
  csBaseTextureFactory::object_reg = object_reg;
  
  texture_type = parent;

  width = 128;
  height = 128;
}

csBaseTextureFactory::~csBaseTextureFactory ()
{
  SCF_DESTRUCT_IBASE ();
}

void csBaseTextureFactory::SetSize (int w, int h)
{
  width = w; height = h;
}

void csBaseTextureFactory::GetSize (int& w, int& h)
{
  w = width; h = height;
}

iTextureType* csBaseTextureFactory::GetTextureType () const
{
  return texture_type;
}
