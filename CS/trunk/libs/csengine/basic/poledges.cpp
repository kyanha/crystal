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
#include "csengine/polyint.h"
#include "csengine/poledges.h"
#include "csengine/polygon.h"

//---------------------------------------------------------------------------

csPolEdgeIterator::csPolEdgeIterator (csHashMap& edges, int i1, int i2)
{
  if (i1 > i2) { int swap = i1; i1 = i2; i2 = swap; }
  csPolEdgeIterator::i1 = i1;
  csPolEdgeIterator::i2 = i2;
  iterator = edges.GetIterator ((i1+1)*(i2+1));
  if (iterator->HasNext ())
  {
    csPolEdge* pol_edge = (csPolEdge*)(iterator->Next ());
    while (pol_edge && (pol_edge->i1 != i1 || pol_edge->i2 != i2))
    {
      pol_edge = (csPolEdge*)(iterator->Next ());
    }
  }
  else
    current = NULL;
}

csPolEdgeIterator::~csPolEdgeIterator ()
{
  delete iterator;
}

csPolygon3D* csPolEdgeIterator::Next ()
{
  if (!current) return NULL;
  csPolygon3D* rc_p = current->p;

  // Prepare to go to next polygon.
  current = (csPolEdge*)(iterator->Next ());
  while (current && (current->i1 != i1 || current->i2 != i2))
  {
    current = (csPolEdge*)(iterator->Next ());
  }

  return rc_p;
}

//---------------------------------------------------------------------------

csPolygonEdges::csPolygonEdges (csPolygonInt** polygons, int num_polygons)
	: edges (20003)	//@@@ Should be a prime. This probably isn't one.
{
  int i, j, j1;
  for (i = 0 ; i < num_polygons ; i++)
  {
    csPolygon3D* p = (csPolygon3D*)polygons[i];
    const csPolyIndexed& pi = p->GetVertices ();
    j1 = pi.GetNumVertices ()-1;
    for (j = 0 ; j < pi.GetNumVertices () ; j++)
    {
      int i1 = pi[j];
      int i2 = pi[j1];
      if (i1 > i2) { int swap = i1; i1 = i2; i2 = swap; }
      int key = (i1+1)*(i2+1);
      csPolEdge* pol_edge = new csPolEdge ();
      pol_edge->p = p;
      pol_edge->i1 = i1;
      pol_edge->i2 = i2;
      edges.Put (key, (void*)pol_edge);
      j1 = j;
    }
  }
}

csPolygonEdges::~csPolygonEdges ()
{
  csHashIterator* iterator = edges.GetIterator ();
  while (iterator->HasNext ())
  {
    csPolEdge* pol_edge = (csPolEdge*)(iterator->Next ());
    delete pol_edge;
  }
}

//---------------------------------------------------------------------------

