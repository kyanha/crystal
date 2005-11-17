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

#ifndef __CS_STENCIL_POLYMESH_H__
#define __CS_STENCIL_POLYMESH_H__

#include "csgeom/polymesh.h"
#include "csgeom/pmtools.h"

class csStencilPolygonMesh : public csPolygonMesh
{
private:
  csDirtyAccessArray<csVector3> verts;
  csDirtyAccessArray<csMeshedPolygon> polys;
  csDirtyAccessArray<int> vertidx;
  int* secondary_vertidx;
  csTriangle* triangles;
  int tri_count;

  void Triangulate ()
  {
    if (triangles) return;
    csPolygonMeshTools::Triangulate (this, triangles, tri_count);
  }

public:
  csStencilPolygonMesh ()
  {
    triangles = 0;
    secondary_vertidx = 0;
  }

  virtual ~csStencilPolygonMesh ()
  {
    delete[] secondary_vertidx;
  }

  /// AddPolys will take over ownership of vertidx.
  void AddPolys (const csArray<csMeshedPolygon>& polysToAdd,
  	int* vertidx)
  {
    CS_ASSERT (secondary_vertidx == 0);
    polys.SetCapacity (polys.Length () + polysToAdd.Length ());
    secondary_vertidx = vertidx;
    size_t i;
    for (i = 0; i < polysToAdd.Length (); i++)
    {
      polys.Push (polysToAdd[i]);
    }
    ShapeChanged ();
  }

  void CopyFrom (iPolygonMesh* polyMesh)
  {
    delete[] secondary_vertidx;
    secondary_vertidx = 0;

    int numVerts = polyMesh->GetVertexCount ();
    csVector3* oldVerts = polyMesh->GetVertices ();
    verts.SetLength (numVerts);
    memcpy (verts.GetArray (), oldVerts, sizeof (csVector3) * numVerts);

    int numPolys = polyMesh->GetPolygonCount ();
    csMeshedPolygon* oldPolys = polyMesh->GetPolygons ();
    polys.SetLength (numPolys);

    int i;
    // First count how many vertex indices we need.
    int totvertidx = 0;
    for (i = 0 ; i < numPolys ; i++)
      totvertidx += oldPolys[i].num_vertices;
    vertidx.SetLength (totvertidx);
    int* vertidx_p = vertidx.GetArray ();

    for (i = 0; i < numPolys; i++)
    {
      csMeshedPolygon& poly = polys[i];
      poly.num_vertices = oldPolys[i].num_vertices;
      poly.vertices = vertidx_p;
      memcpy (poly.vertices, oldPolys[i].vertices, 
	poly.num_vertices * sizeof (int));
      vertidx_p += poly.num_vertices;
    }

    ShapeChanged ();
  }

  virtual int GetVertexCount () 
  { 
    return (int)verts.Length (); 
  }
  virtual csVector3* GetVertices () 
  { 
    return verts.GetArray (); 
  }
  virtual int GetPolygonCount () 
  { 
    return (int)polys.Length (); 
  }
  virtual csMeshedPolygon* GetPolygons () 
  { 
    return polys.GetArray ();
  }
  virtual int GetTriangleCount ()
  {
    Triangulate ();
    return tri_count;
  }
  virtual csTriangle* GetTriangles ()
  {
    Triangulate ();
    return triangles;
  }
};

#endif // __CS_STENCIL_POLYMESH_H__
