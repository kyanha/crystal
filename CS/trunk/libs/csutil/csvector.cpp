/*
    Crystal Space Vector class
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

#include <stdlib.h>
#include <string.h>
#include "cssysdef.h"
#include "csutil/csvector.h"

csBasicVector::csBasicVector (int ilimit, int ithreshold) : count(0), root(0)
{
  limit = (ilimit > 0 ? ilimit : 0);
  threshold = (ithreshold > 0 ? ithreshold : 16);
  if (limit != 0)
    root = (void**) malloc (limit * sizeof(void*));
}

csBasicVector::~csBasicVector ()
{
  free (root);
}

void csBasicVector::SetLength (int n)
{
  count = n;
  if (n > limit || (limit > threshold && n < limit - threshold))
  {
    n = ((n + threshold - 1) / threshold ) * threshold;
    if (!n)
    {
      free (root);
      root = 0;
    }
    else if (root == 0)
      root = (void**)malloc(n * sizeof(void*));
    else
      root = (void**)realloc(root, n * sizeof(void*));
    limit = n;
  }
}

int csBasicVector::Find (void* which) const
{
  for (int i = 0; i < Length (); i++)
    if (root [i] == which)
      return i;

  return -1;
}

bool csBasicVector::Insert (int n, void* Item)
{
  if (n <= count)
  {
    SetLength (count + 1); // Increments 'count' as a side-effect.
    const int nmove = (count - n - 1);
    if (nmove > 0)
      memmove (&root [n + 1], &root [n], nmove * sizeof (void*));
    root [n] = Item;
    return true;
  }
  else
   return false;
}

bool csBasicVector::Delete (int n)
{
  if (n >= 0 && n < count)
  {
    const int ncount = count - 1;
    const int nmove = ncount - n;
    if (nmove > 0)
      memmove (&root [n], &root [n + 1], nmove * sizeof (void*));
    SetLength (ncount);
    return true;
  }
  else
    return false;
}


bool csBasicVector::InsertChunk (int n, int size , void** Item)
{ // no run-time checks here, just in debug mode
  CS_ASSERT( n <= count );
  CS_ASSERT( size>0 );
  SetLength (count + size); // Increments 'count' as a side-effect
  const int nmove = (count - (n + size));
  if (nmove > 0)
    memmove ( &root [ n + size ] , &root [ n ] , nmove * sizeof (void*) );
    
  memcpy  ( &root [ n ] , Item , size * sizeof (void*) );    
  return true;  
}

bool csBasicVector::DeleteChunk (int n, int size)
{ // no run-time checks here, just in debug mode
  CS_ASSERT( (n >= 0) && (n < count) );
  CS_ASSERT( (size > 0) && ( n + size <= count ) );
  const int nmove  = count - n - size;
  if ( nmove > 0 )
    memmove (&root [ n ] , &root [ n + size ] , nmove * sizeof (void*) );
  
  SetLength ( count - size );
  return true;
}

void csVector::DeleteAll (bool FreeThem)
{
  if (FreeThem)
    for (int i = count-1; i >= 0; i--)
      Delete (i);
  else
    SetLength (0);
}

bool csVector::FreeItem (void* Item)
{
  (void)Item;
  return true;
}

bool csVector::Delete (int n, bool FreeIt)
{
  if (n < 0 || n >= count)
    return false;

  void* obj = root[n];

  if (!csBasicVector::Delete(n))
      return false;

  if (FreeIt && !FreeItem (obj))
      return false;

  return true;
}

bool csVector::Replace (int n, void* what, bool FreePrevious)
{
  if (n >= count)
    return false;

  if (FreePrevious)
  {
    if (!FreeItem (root [n]))
      return false;
  }

  root [n] = what;
  return true;
}

int csVector::FindKey (const void* Key, int Mode) const
{
  int i;
  for (i = 0; i < Length (); i++)
    if (CompareKey (root [i], Key, Mode) == 0)
      return i;
  return -1;
}

int csVector::FindSortedKey (const void* Key, int Mode) const
{
  int l = 0, r = Length () - 1;
  while (l <= r)
  {
    int m = (l + r) / 2;
    int cmp = CompareKey (root [m], Key, Mode);

    if (cmp == 0)
      return m;
    else if (cmp < 0)
      l = m + 1;
    else
      r = m - 1;
  }
  return -1;
}

int csVector::InsertSorted (void* Item, int *oEqual, int Mode)
{
  int m = 0, l = 0, r = Length () - 1;
  while (l <= r)
  {
    m = (l + r) / 2;
    int cmp = Compare (root [m], Item, Mode);

    if (cmp == 0)
    {
      if (oEqual)
        *oEqual = m;
      Insert (++m, Item);
      return m;
    }
    else if (cmp < 0)
      l = m + 1;
    else
      r = m - 1;
  }
  if (r == m)
    m++;
  Insert (m, Item);
  if (oEqual)
    *oEqual = -1;
  return m;
}

void csVector::QuickSort (int Left, int Right, int Mode)
{
recurse:
  int i = Left, j = Right;
  int x = (Left + Right) / 2;
  do
  {
    while ((i != x) && (Compare (Get (i), Get (x), Mode) < 0))
      i++;
    while ((j != x) && (Compare (Get (j), Get (x), Mode) > 0))
      j--;
    if (i < j)
    {
      Exchange (i, j);
      if (x == i)
        x = j;
      else if (x == j)
        x = i;
    }
    if (i <= j)
    {
      i++;
      if (j > Left)
        j--;
    }
  } while (i <= j);

  if (j - Left < Right - i)
  {
    if (Left < j)
      QuickSort (Left, j, Mode);
    if (i < Right)
    {
      Left = i;
      goto recurse;
    }
  }
  else
  {
    if (i < Right)
      QuickSort (i, Right, Mode);
    if (Left < j)
    {
      Right = j;
      goto recurse;
    }
  }
}

int csVector::Compare (void* Item1, void* Item2, int Mode) const
{
  (void)Mode;
  return ((int)Item1 > (int)Item2) ? +1 : ((int)Item1 == (int)Item2) ? 0 : -1;
}

int csVector::CompareKey (void* Item, const void* Key, int Mode) const
{
  (void)Mode;
  return (Item > Key) ? +1 : (Item == Key) ? 0 : -1;
}
