/*
    Sparse 3-D matrix.
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

#include <stdio.h>
#include "cssysdef.h"
#include "csutil/sparse3d.h"

WideSparse3D::WideSparse3D ()
{
  first_x = NULL;
}

WideSparse3D::~WideSparse3D ()
{
  clear ();
}

WideSparse3D::HdX* WideSparse3D::get_header_x (int x)
{
  HdX* l = first_x;
  while (l)
  {
    if (l->x == x) return l;
    l = l->next;
  }
  return NULL;
}

WideSparse3D::HdY* WideSparse3D::get_header_y (HdX* y_list, int y)
{
  if (!y_list) return NULL;
  HdY* l = y_list->first_y;
  while (l)
  {
    if (l->y == y) return l;
    l = l->next;
  }
  return NULL;
}

WideSparse3D::SparseCell* WideSparse3D::get_cell_z (HdY* z_list, int z)
{
  if (!z_list) return NULL;
  SparseCell* l = z_list->first_z;
  while (l)
  {
    if (l->z == z) return l;
    l = l->next;
  }
  return NULL;
}


void WideSparse3D::clear ()
{
  //@@@ To be implemented
}

void WideSparse3D::set (int x, int y, int z, void* obj)
{
  HdX* lx = get_header_x (x);
  HdY* ly = get_header_y (lx, y);
  SparseCell* lz = get_cell_z (ly, z);
  if (!lz)
  {
    if (!lx)
    {
      lx = new HdX ();
      lx->x = x;
      lx->next = first_x;
      lx->prev = NULL;
      if (first_x) first_x->prev = lx;
      first_x = lx;
    }
    if (!ly)
    {
      ly = new HdY ();
      ly->y = y;
      ly->next = lx->first_y;
      ly->prev = NULL;
      if (lx->first_y) lx->first_y->prev = ly;
      lx->first_y = ly;
    }
    lz = new SparseCell ();
    lz->z = z;
    lz->next = ly->first_z;
    lz->prev = NULL;
    if (ly->first_z) ly->first_z->prev = lz;
    ly->first_z = lz;
  }
  lz->obj = obj;
}

void* WideSparse3D::get (int x, int y, int z)
{
  HdX* lx = get_header_x (x);
  HdY* ly = get_header_y (lx, y);
  SparseCell* lz = get_cell_z (ly, z);
  if (lz) return lz->obj;
  return NULL;
}

void WideSparse3D::del (int x, int y, int z)
{
  HdX* lx = get_header_x (x);
  HdY* ly = get_header_y (lx, y);
  SparseCell* lz = get_cell_z (ly, z);
  if (lz)
  {
    if (lz->next) lz->next->prev = lz->prev;
    if (lz->prev) lz->prev->next = lz->next;
    else ly->first_z = lz->next;
    delete lz;
    if (!ly->first_z)
    {
      if (ly->next) ly->next->prev = ly->prev;
      if (ly->prev) ly->prev->next = ly->next;
      else lx->first_y = ly->next;
      delete ly;
      if (!lx->first_y)
      {
        if (lx->next) lx->next->prev = lx->prev;
        if (lx->prev) lx->prev->next = lx->next;
        else first_x = lx->next;
	delete lx;
      }
    }
  }
}

