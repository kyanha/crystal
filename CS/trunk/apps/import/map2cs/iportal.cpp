/*
    Map2cs: a convertor to convert the frequently used MAP format, into
    something, that can be directly understood by Crystal Space.

    Copyright (C) 1999 Thomas Hieber (thieber@gmx.net)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "cssysdef.h"
#include "mapstd.h"
#include "iportal.h"

CIPortal::CIPortal()
{
  m_pSector = NULL;
}

CIPortal::CIPortal(const CMapPolygonSet& Set)
 : CMapPolygonSet(Set)
{
  m_pSector = NULL;
}

CIPortal::CIPortal(const CIPortal& Portal)
 : CMapPolygonSet(Portal)
{
  m_pSector = NULL;
}

CIPortal::~CIPortal()
{
  //m_pSector is only a reference, so we don't free it _here_
}

CIPortal& CIPortal::operator=(const CIPortal& Other)
{
  CMapPolygonSet::operator=(Other);
  m_pSector = Other.m_pSector;
  return *this;
}

