/*
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

#include "cssysdef.h"
#include "csgeom/poly3d.h"
#include "csgeom/poly2d.h"

csPoly3D::csPoly3D (int start_size)
{
  max_vertices = start_size;
  vertices = new csVector3 [max_vertices];
  MakeEmpty ();
}

csPoly3D::csPoly3D (csPoly3D& copy)
{
  max_vertices = copy.max_vertices;
  vertices = new csVector3 [max_vertices];
  num_vertices = copy.num_vertices;
  memcpy (vertices, copy.vertices, sizeof (csVector3)*num_vertices);
}

csPoly3D::~csPoly3D ()
{
  delete [] vertices;
}

void csPoly3D::MakeEmpty ()
{
  num_vertices = 0;
}

bool csPoly3D::In (const csVector3& v)
{
  int i, i1;
  i1 = num_vertices-1;
  for (i = 0 ; i < num_vertices ; i++)
  {
    if (csMath3::WhichSide3D (v, vertices[i1], vertices[i]) < 0) return false;
    i1 = i;
  }
  return true;
}

bool csPoly3D::In (csVector3* poly, int num_poly, const csVector3& v)
{
  int i, i1;
  i1 = num_poly-1;
  for (i = 0 ; i < num_poly ; i++)
  {
    if (csMath3::WhichSide3D (v, poly[i1], poly[i]) < 0) return false;
    i1 = i;
  }
  return true;
}

void csPoly3D::MakeRoom (int new_max)
{
  if (new_max <= max_vertices) return;
  csVector3* new_vertices = new csVector3 [new_max];
  memcpy (new_vertices, vertices, num_vertices*sizeof (csVector3));
  delete [] vertices;
  vertices = new_vertices;
  max_vertices = new_max;
}

int csPoly3D::AddVertex (float x, float y, float z)
{
  if (num_vertices >= max_vertices)
    MakeRoom (max_vertices+5);
  vertices[num_vertices].x = x;
  vertices[num_vertices].y = y;
  vertices[num_vertices].z = z;
  num_vertices++;
  return num_vertices-1;
}

bool csPoly3D::ProjectXPlane (const csVector3& point, float plane_x,
	csPoly2D* poly2d)
{
  poly2d->SetNumVertices (0);
  poly2d->GetBoundingBox ().StartBoundingBox ();
  csVector2 p;
  csVector3 v;
  float x_dist = plane_x - point.x;
  int i;
  for (i = 0 ; i < num_vertices ; i++)
  {
    v = vertices[i]-point;
    if (ABS (v.x) < SMALL_EPSILON) return false;
    p.x = point.y + x_dist * v.y / v.x;
    p.y = point.z + x_dist * v.z / v.x;
    poly2d->AddVertex (p);
  }
  return true;
}

bool csPoly3D::ProjectYPlane (const csVector3& point, float plane_y,
	csPoly2D* poly2d)
{
  poly2d->SetNumVertices (0);
  poly2d->GetBoundingBox ().StartBoundingBox ();
  csVector2 p;
  csVector3 v;
  float y_dist = plane_y - point.y;
  int i;
  for (i = 0 ; i < num_vertices ; i++)
  {
    v = vertices[i]-point;
    if (ABS (v.y) < SMALL_EPSILON) return false;
    p.x = point.x + y_dist * v.x / v.y;
    p.y = point.z + y_dist * v.z / v.y;
    poly2d->AddVertex (p);
  }
  return true;
}

bool csPoly3D::ProjectZPlane (const csVector3& point, float plane_z,
	csPoly2D* poly2d)
{
  poly2d->SetNumVertices (0);
  poly2d->GetBoundingBox ().StartBoundingBox ();
  csVector2 p;
  csVector3 v;
  float z_dist = plane_z - point.z;
  int i;
  for (i = 0 ; i < num_vertices ; i++)
  {
    v = vertices[i]-point;
    if (ABS (v.z) < SMALL_EPSILON) return false;
    p.x = point.x + z_dist * v.x / v.z;
    p.y = point.y + z_dist * v.y / v.z;
    poly2d->AddVertex (p);
  }
  return true;
}


int csPoly3D::Classify (const csPlane3& pl) const
{
  int i;
  int front = 0, back = 0;

  for (i = 0 ; i < num_vertices ; i++)
  {
    float dot = pl.Classify (vertices[i]);
    if (ABS (dot) < EPSILON) dot = 0;
    if (dot > 0) back++;
    else if (dot < 0) front++;
  }
  if (back == 0 && front == 0) return POL_SAME_PLANE;
  if (back == 0) return POL_FRONT;
  if (front == 0) return POL_BACK;
  return POL_SPLIT_NEEDED;
}

int csPoly3D::ClassifyX (float x) const
{
  int i;
  int front = 0, back = 0;

  for (i = 0 ; i < num_vertices ; i++)
  {
    float xx = vertices[i].x-x;
    if (xx < -EPSILON) front++;
    else if (xx > EPSILON) back++;
  }
  if (back == 0) return POL_FRONT;
  if (front == 0) return POL_BACK;
  return POL_SPLIT_NEEDED;
}

int csPoly3D::ClassifyY (float y) const
{
  int i;
  int front = 0, back = 0;

  for (i = 0 ; i < num_vertices ; i++)
  {
    float yy = vertices[i].y-y;
    if (yy < -EPSILON) front++;
    else if (yy > EPSILON) back++;
  }
  if (back == 0) return POL_FRONT;
  if (front == 0) return POL_BACK;
  return POL_SPLIT_NEEDED;
}

int csPoly3D::ClassifyZ (float z) const
{
  int i;
  int front = 0, back = 0;

  for (i = 0 ; i < num_vertices ; i++)
  {
    float zz = vertices[i].z-z;
    if (zz < -EPSILON) front++;
    else if (zz > EPSILON) back++;
  }
  if (back == 0) return POL_FRONT;
  if (front == 0) return POL_BACK;
  return POL_SPLIT_NEEDED;
}

void csPoly3D::SplitWithPlane (csPoly3D& poly1, csPoly3D& poly2,
				  const csPlane3& split_plane) const
{
  poly1.MakeEmpty ();
  poly2.MakeEmpty ();

  csVector3 ptB;
  float sideA, sideB;
  csVector3 ptA = vertices[num_vertices - 1];
  sideA = split_plane.Classify (ptA);
  if (ABS (sideA) < SMALL_EPSILON) sideA = 0;

  for (int i = -1 ; ++i < num_vertices ; )
  {
    ptB = vertices[i];
    sideB = split_plane.Classify (ptB);
    if (ABS (sideB) < SMALL_EPSILON) sideB = 0;
    if (sideB > 0)
    {
      if (sideA < 0)
      {
	// Compute the intersection point of the line
	// from point A to point B with the partition
	// plane. This is a simple ray-plane intersection.
	csVector3 v = ptB; v -= ptA;
	float sect = - split_plane.Classify (ptA) / ( split_plane.GetNormal () * v ) ;
	v *= sect; v += ptA;
	poly1.AddVertex (v);
	poly2.AddVertex (v);
      }
      poly2.AddVertex (ptB);
    }
    else if (sideB < 0)
    {
      if (sideA > 0)
      {
	// Compute the intersection point of the line
	// from point A to point B with the partition
	// plane. This is a simple ray-plane intersection.
	csVector3 v = ptB; v -= ptA;
	float sect = - split_plane.Classify (ptA) / ( split_plane.GetNormal () * v );
	v *= sect; v += ptA;
	poly1.AddVertex (v);
	poly2.AddVertex (v);
      }
      poly1.AddVertex (ptB);
    }
    else
    {
      poly1.AddVertex (ptB);
      poly2.AddVertex (ptB);
    }
    ptA = ptB;
    sideA = sideB;
  }
}

void csPoly3D::SplitWithPlaneX (csPoly3D& poly1, csPoly3D& poly2,
				  float x) const
{
  poly1.MakeEmpty ();
  poly2.MakeEmpty ();

  csVector3 ptB;
  float sideA, sideB;
  csVector3 ptA = vertices[num_vertices - 1];
  sideA = ptA.x - x;
  if (ABS (sideA) < SMALL_EPSILON) sideA = 0;

  for (int i = -1 ; ++i < num_vertices ; )
  {
    ptB = vertices[i];
    sideB = ptB.x - x;
    if (ABS (sideB) < SMALL_EPSILON) sideB = 0;
    if (sideB > 0)
    {
      if (sideA < 0)
      {
	// Compute the intersection point of the line
	// from point A to point B with the partition
	// plane. This is a simple ray-plane intersection.
	csVector3 v = ptB; v -= ptA;
	float sect = - (ptA.x - x) / v.x;
	v *= sect; v += ptA;
	poly1.AddVertex (v);
	poly2.AddVertex (v);
      }
      poly2.AddVertex (ptB);
    }
    else if (sideB < 0)
    {
      if (sideA > 0)
      {
	// Compute the intersection point of the line
	// from point A to point B with the partition
	// plane. This is a simple ray-plane intersection.
	csVector3 v = ptB; v -= ptA;
	float sect = - (ptA.x - x) / v.x;
	v *= sect; v += ptA;
	poly1.AddVertex (v);
	poly2.AddVertex (v);
      }
      poly1.AddVertex (ptB);
    }
    else
    {
      poly1.AddVertex (ptB);
      poly2.AddVertex (ptB);
    }
    ptA = ptB;
    sideA = sideB;
  }
}

void csPoly3D::SplitWithPlaneY (csPoly3D& poly1, csPoly3D& poly2,
				  float y) const
{
  poly1.MakeEmpty ();
  poly2.MakeEmpty ();

  csVector3 ptB;
  float sideA, sideB;
  csVector3 ptA = vertices[num_vertices - 1];
  sideA = ptA.y - y;
  if (ABS (sideA) < SMALL_EPSILON) sideA = 0;

  for (int i = -1 ; ++i < num_vertices ; )
  {
    ptB = vertices[i];
    sideB = ptB.y - y;
    if (ABS (sideB) < SMALL_EPSILON) sideB = 0;
    if (sideB > 0)
    {
      if (sideA < 0)
      {
	// Compute the intersection point of the line
	// from point A to point B with the partition
	// plane. This is a simple ray-plane intersection.
	csVector3 v = ptB; v -= ptA;
	float sect = - (ptA.y - y) / v.y;
	v *= sect; v += ptA;
	poly1.AddVertex (v);
	poly2.AddVertex (v);
      }
      poly2.AddVertex (ptB);
    }
    else if (sideB < 0)
    {
      if (sideA > 0)
      {
	// Compute the intersection point of the line
	// from point A to point B with the partition
	// plane. This is a simple ray-plane intersection.
	csVector3 v = ptB; v -= ptA;
	float sect = - (ptA.y - y) / v.y;
	v *= sect; v += ptA;
	poly1.AddVertex (v);
	poly2.AddVertex (v);
      }
      poly1.AddVertex (ptB);
    }
    else
    {
      poly1.AddVertex (ptB);
      poly2.AddVertex (ptB);
    }
    ptA = ptB;
    sideA = sideB;
  }
}

void csPoly3D::SplitWithPlaneZ (csPoly3D& poly1, csPoly3D& poly2,
				  float z) const
{
  poly1.MakeEmpty ();
  poly2.MakeEmpty ();

  csVector3 ptB;
  float sideA, sideB;
  csVector3 ptA = vertices[num_vertices - 1];
  sideA = ptA.z - z;
  if (ABS (sideA) < SMALL_EPSILON) sideA = 0;

  for (int i = -1 ; ++i < num_vertices ; )
  {
    ptB = vertices[i];
    sideB = ptB.z - z;
    if (ABS (sideB) < SMALL_EPSILON) sideB = 0;
    if (sideB > 0)
    {
      if (sideA < 0)
      {
	// Compute the intersection point of the line
	// from point A to point B with the partition
	// plane. This is a simple ray-plane intersection.
	csVector3 v = ptB; v -= ptA;
	float sect = - (ptA.z - z) / v.z;
	v *= sect; v += ptA;
	poly1.AddVertex (v);
	poly2.AddVertex (v);
      }
      poly2.AddVertex (ptB);
    }
    else if (sideB < 0)
    {
      if (sideA > 0)
      {
	// Compute the intersection point of the line
	// from point A to point B with the partition
	// plane. This is a simple ray-plane intersection.
	csVector3 v = ptB; v -= ptA;
	float sect = - (ptA.z - z) / v.z;
	v *= sect; v += ptA;
	poly1.AddVertex (v);
	poly2.AddVertex (v);
      }
      poly1.AddVertex (ptB);
    }
    else
    {
      poly1.AddVertex (ptB);
      poly2.AddVertex (ptB);
    }
    ptA = ptB;
    sideA = sideB;
  }
}


//---------------------------------------------------------------------------

int csVector3Array::AddVertexSmart (float x, float y, float z)
{
  int i;
  for (i = 0 ; i < num_vertices ; i++)
    if (ABS (x-vertices[i].x) < SMALL_EPSILON &&
    	ABS (y-vertices[i].y) < SMALL_EPSILON &&
	ABS (z-vertices[i].z) < SMALL_EPSILON)
      return i;
  AddVertex (x, y, z);
  return num_vertices-1;
}

//---------------------------------------------------------------------------

