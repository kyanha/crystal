/*
    Copyright (C) 1998-2001 by Jorrit Tyberghein
    Largely rewritten by Ivan Avramovic <ivan@avramovic.com>

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

#include <math.h>
#include <float.h>
#include "cssysdef.h"
#include "qint.h"
#include "qsqrt.h"
#include "csgeom/math3d.h"
#include "csgeom/math3d_d.h"
#include "csgeom/matrix3.h"
#include "csgeom/poly3d.h"
#include "csgeom/segment.h"
#include "csgeom/box.h"
#include "csgeom/obb.h"
#include "csgeom/pmtools.h"
#include "igeom/polymesh.h"
#include "csutil/scfstr.h"

//---------------------------------------------------------------------------
bool csMath3::FindIntersection (
  const csVector3 tri1[3],
  const csVector3 tri2[3],
  csVector3 line[2])
{
  int i, j;
  csVector3 v1[3], v2[3];

  for (i = 0; i < 3; i++)
  {
    j = (i + 1) % 3;
    v1[i] = tri1[j] - tri1[i];
    v2[i] = tri2[j] - tri2[i];
  }

  csVector3 n1 = v1[0] % v1[1];
  csVector3 n2 = v2[0] % v2[1];

  float d1 = -n1 * tri1[0], d2 = -n2 * tri2[0];

  csVector3 d = n1 % n2;

  int index = 0;
  float max = ABS (d.x);
  if (ABS (d.y) > max) max = ABS (d.y), index = 1;
  if (ABS (d.z) > max) max = ABS (d.z), index = 2;

  int m1 = 0, m2 = 0, n = 0;
  float t1[3], t2[3];
  csVector3 p1[2], p2[2];
  p1[0].Set (0, 0, 0);
  p1[1].Set (0, 0, 0);
  p2[0].Set (0, 0, 0);
  p2[1].Set (0, 0, 0);

  float isect1[2], isect2[2], isect[4];
  csVector3 *idx[4];

  for (i = 0; i < 3; i++)
  {
    float div1 = n2 * v1[i], div2 = n1 * v2[i];
    float pr1 = -(n2 * tri1[i] + d2), pr2 = -(n1 * tri2[i] + d1);

    if (ABS (div1) < SMALL_EPSILON)
    {
      if (ABS (pr1) < SMALL_EPSILON)
      {
        // line is in the plane of intersection
        t1[i] = 0;
      }
      else
      {
        // line is parallel to the plane of
        // intersection, so we don't need it ;)
        t1[i] = 15.0;
      }
    }
    else
      t1[i] = pr1 / div1;

    if (ABS (div2) < SMALL_EPSILON)
    {
      if (ABS (pr2) < SMALL_EPSILON)
      {
        // line is in the plane of intersection
        t2[i] = 0;
      }
      else
      {
        // line is parallel to the plane of
        // intersection, so we don't need it ;)
        t2[i] = 15.0;
      }
    }
    else
      t2[i] = pr2 / div2;

    if (t1[i] >= 0.0 && t1[i] <= 1.0 && m1 != 2)
    {
      p1[m1] = tri1[i] + v1[i] * t1[i];
      isect1[m1] = p1[m1][index];
      idx[n] = p1 + m1;
      isect[n++] = isect1[m1++];
    }

    if (t2[i] >= 0.0 && t2[i] <= 1.0 && m2 != 2)
    {
      p2[m2] = tri2[i] + v2[i] * t2[i];
      isect2[m2] = p2[m2][index];
      idx[n] = p2 + m2;
      isect[n++] = isect2[m2++];
    }
  }

  if (n < 4)
  {
    // triangles are not intersecting
    return 0;
  }

  for (i = 0; i < 4; i++)
  {
    for (j = i + 1; j < 4; j++)
    {
      if (isect[i] > isect[j])
      {
        csVector3 *p = idx[j];
        idx[j] = idx[i];
        idx[i] = p;

        float _ = isect[i];
        isect[i] = isect[j];
        isect[j] = _;
      }
    }
  }

  line[0] = *idx[1];
  line[1] = *idx[2];

  return 1;
}

void csMath3::Between (
  const csVector3 &v1,
  const csVector3 &v2,
  csVector3 &v,
  float pct,
  float wid)
{
  if (pct != -1)
    pct /= 100.;
  else
  {
    float sqdist = (v1 - v2) * (v1 - v2);
    if (sqdist < SMALL_EPSILON)
    {
      v = v1;
      return ;
    }

    float invdist = qisqrt (sqdist);
    pct = wid * invdist;
  }

  v = v1 + pct * (v2 - v1);
}

bool csMath3::Visible (
  const csVector3 &p,
  const csVector3 &t1,
  const csVector3 &t2,
  const csVector3 &t3)
{
  float x1 = t1.x - p.x;
  float y1 = t1.y - p.y;
  float z1 = t1.z - p.z;
  float x2 = t2.x - p.x;
  float y2 = t2.y - p.y;
  float z2 = t2.z - p.z;
  float x3 = t3.x - p.x;
  float y3 = t3.y - p.y;
  float z3 = t3.z - p.z;
  float c = x3 * ((z1 * y2) - (y1 * z2)) + y3 * ((x1 * z2) - (z1 * x2)) + z3 *
    ((y1 * x2) - (x1 * y2));
  return c > 0;
}

bool csMath3::PlanesClose (const csPlane3 &p1, const csPlane3 &p2)
{
  if (PlanesEqual (p1, p2)) return true;

  csPlane3 p1n = p1;
  p1n.Normalize ();

  csPlane3 p2n = p2;
  p2n.Normalize ();
  return PlanesEqual (p1n, p2n);
}

int csMath3::OuterPlanes (
  const csBox3 &box1,
  const csBox3 &box2,
  csPlane3 *planes)
{
  int i, j, k;

  // @@@ This is a HIGHLY INEFFICIENT routine.
  // However, I haven't yet found something better.
  int num_planes = 0;
  for (i = 0; i < 8; i++)
  {
    csVector3 v1 = box1.GetCorner (i);

    // Try 24 edges instead of 12. The 12 other edges
    // are just edges with inverted direction.
    for (j = 0; j < 24; j++)
    {
      csSegment3 seg = box2.GetEdge (j);
      csVector3 v2a, v2b;
      v2a = seg.Start ();
      v2b = seg.End ();

      csPlane3 pl (v1, v2a, v2b);
      pl.Normalize ();

      // Check if we already have this plane.
      bool equal = false;
      for (k = 0; k < num_planes; k++)
        if (csMath3::PlanesEqual (planes[k], pl))
        {
          equal = true;
          break;
        }

      if (equal) continue;

      // Count how many vertices of the two boxes are inside or outside
      // the plane. We need planes with all vertices either
      // on the plane or inside.
      int cnt_out = 0;
      for (k = 0; k < 8; k++)
      {
        float cl = pl.Classify (box1.GetCorner (k));
        if (cl < -EPSILON)
        {
          cnt_out++;
          break;
        }

        cl = pl.Classify (box2.GetCorner (k));
        if (cl < -EPSILON)
        {
          cnt_out++;
          break;
        }
      }

      // If no vertices are outside then we have a good plane.
      if (cnt_out == 0)
      {
        CS_ASSERT (num_planes < 8);
        planes[num_planes++] = pl;
      }
    }
  }

  return num_planes;
}

int csMath3::FindObserverSides (
  const csBox3 &box1,
  const csBox3 &box2,
  int *sides)
{
  int num_sides = 0;
  csPlane3 pl;
  pl.Set (1, 0, 0, -box1.MinX ());
  if (pl.Classify (box2.GetCorner (CS_BOX_CORNER_xyz)) < 0)
    sides[num_sides++] = 0;
  pl.Set (-1, 0, 0, box1.MaxX ());
  if (pl.Classify (box2.GetCorner (CS_BOX_CORNER_Xyz)) < 0)
    sides[num_sides++] = 1;
  pl.Set (0, 1, 0, -box1.MinY ());
  if (pl.Classify (box2.GetCorner (CS_BOX_CORNER_xyz)) < 0)
    sides[num_sides++] = 2;
  pl.Set (0, -1, 0, box1.MaxY ());
  if (pl.Classify (box2.GetCorner (CS_BOX_CORNER_xYz)) < 0)
    sides[num_sides++] = 3;
  pl.Set (0, 0, 1, -box1.MinZ ());
  if (pl.Classify (box2.GetCorner (CS_BOX_CORNER_xyz)) < 0)
    sides[num_sides++] = 4;
  pl.Set (0, 0, -1, box1.MaxZ ());
  if (pl.Classify (box2.GetCorner (CS_BOX_CORNER_xyZ)) < 0)
    sides[num_sides++] = 5;
  return num_sides;
}

void csMath3::SpherePosition (
  float angle_xz,
  float angle_vert,
  csVector3 &pos)
{
  float cosxz = cos (angle_xz);
  float sinxz = sin (angle_xz);
  float cosve = cos (angle_vert);
  float sinve = sin (angle_vert);
  pos.Set (cosxz * cosve, sinve, sinxz * cosve);
}

//---------------------------------------------------------------------------
float csSquaredDist::PointLine (
  const csVector3 &p,
  const csVector3 &l1,
  const csVector3 &l2)
{
  csVector3 W = l1 - p;
  csVector3 L = l2 - l1;
  csVector3 p2l = W - L * (W * L) / (L * L);
  return p2l * p2l;
}

float csSquaredDist::PointPoly (
  const csVector3 &p,
  csVector3 *V,
  int n,
  const csPlane3 &plane,
  float sqdist)
{
  csVector3 W, L;
  bool lflag = true, lflag0 = true;
  int i;
  for (i = 0; i < n - 1; i++)
  {
    W = V[i] - p;
    if (i == 0)
    {
      if (!(W * (V[n - 1] - V[0]) > 0))
        lflag0 = false;
      else if (W * (V[1] - V[0]) > 0)
        return W * W;
      else
        lflag = false;
    }
    else if (!(W * (L = V[i - 1] - V[i]) > 0))
    {
      if (!lflag && W * (plane.norm % L) > 0)
      {
        L = W - L * (W * L) / (L * L);
        return L * L;
      }

      lflag = (W * (V[i + 1] - V[i]) > 0);
    }
    else if (W * (V[i + 1] - V[i]) > 0)
      return W * W;
    else
      lflag = false;
  }

  W = V[n - 1] - p;
  if (!lflag)
  {
    lflag = W * (L = V[n - 2] - V[n - 1]) <= 0;
    if (lflag && (W * (plane.norm % L) > 0))
    {
      L = W - L * (W * L) / (L * L);
      return L * L;
    }
  }

  if (!lflag0)
  {
    lflag0 = W * (L = V[0] - V[n - 1]) <= 0;
    if (lflag0 && (W * (plane.norm % L) < 0))
    {
      L = W - L * (W * L) / (L * L);
      return L * L;
    }
  }

  if (!lflag && !lflag0) return W * W;
  if (sqdist >= 0) return sqdist;
  return csSquaredDist::PointPlane (p, plane);
}

//---------------------------------------------------------------------------
int csIntersect3::IntersectSegment (
  csPlane3 *planes,
  int num_planes,
  csSegment3 &seg)
{
  csVector3 &v1 = seg.Start ();
  csVector3 &v2 = seg.End ();
  csVector3 isect;
  float dist;
  bool mod = false;
  bool out1 = false, out2 = false;  // If v1 or v2 is out the frustum.
  int i;
  for (i = 0; i < num_planes; i++)
  {
    csPlane3 &pl = planes[i];
    float c1 = pl.Classify (v1);
    float c2 = pl.Classify (v2);
    if (c1 < 0) out1 = true;
    if (c2 < 0) out2 = true;
    if (c1 < 0 && c2 > 0)
    {
      if (Plane (v1, v2, pl, isect, dist))
      {
        mod = true;
        v1 = isect;
        if ((v2 - v1) < (float).0001) return -1;
      }
    }
    else if (c1 > 0 && c2 < 0)
    {
      if (Plane (v1, v2, pl, isect, dist))
      {
        mod = true;
        v2 = isect;
        if ((v2 - v1) < (float).0001) return -1;
      }
    }
  }

  if (out1 && out2 && !mod) return -1;
  return mod ? 1 : 0;
}

bool csIntersect3::IntersectPolygon (
  const csPlane3 &plane,
  csPoly3D *poly,
  csSegment3 &seg)
{
  csVector3 &v1 = seg.Start ();
  csVector3 &v2 = seg.End ();
  int i, i1;
  float c, c1;
  csVector3 isect;
  float dist;
  i1 = poly->GetVertexCount () - 1;
  c1 = plane.Classify ((*poly)[i1]);

  bool found_v1 = false;
  bool found_v2 = false;
  for (i = 0; i < poly->GetVertexCount (); i++)
  {
    c = plane.Classify ((*poly)[i]);
    if ((c < 0 && c1 > 0) || (c1 < 0 && c > 0))
    {
      Plane ((*poly)[i1], (*poly)[i], plane, isect, dist);
      if (!found_v1)
      {
        v1 = isect;
        found_v1 = true;
      }
      else
      {
        v2 = isect;
        found_v2 = true;
        break;
      }
    }

    i1 = i;
    c1 = c;
  }

  if (!found_v1) return false;
  if (!found_v2) v2 = v1;
  return true;
}

bool csIntersect3::IntersectTriangle (
  const csVector3 &tr1,
  const csVector3 &tr2,
  const csVector3 &tr3,
  const csSegment3 &seg,
  csVector3 &isect)
{
  csPlane3 plane (tr1, tr2, tr3);
  float dist;
  if (!Plane (seg.Start (), seg.End (), plane, isect, dist)) return false;

  // 'isect' is the intersection of the segment and the
  // plane. Now we have to see if this intersection is
  // in the triangle.
  if (plane.D () > SMALL_EPSILON)   // Check if plane is not near origin.
  {
    if (csMath3::WhichSide3D (isect, tr3, tr1) > 0) return false;
    if (csMath3::WhichSide3D (isect, tr1, tr2) > 0) return false;
    if (csMath3::WhichSide3D (isect, tr2, tr3) > 0) return false;
  }
  else
  {
    // Bug fix for WichSide3D. Slower but valid.
    csVector3 norm = plane.Normal ();
    csVector3 nsect = isect + norm;
    csVector3 ntr1 = tr1 + norm;
    csVector3 ntr2 = tr2 + norm;
    csVector3 ntr3 = tr3 + norm;
    if (csMath3::WhichSide3D (nsect, ntr3, ntr1) > 0) return false;
    if (csMath3::WhichSide3D (nsect, ntr1, ntr2) > 0) return false;
    if (csMath3::WhichSide3D (nsect, ntr2, ntr3) > 0) return false;
  }

  return true;
}

bool csIntersect3::Plane (
  const csVector3 &u,
  const csVector3 &v,
  const csVector3 &normal,
  const csVector3 &a,
  csVector3 &isect,
  float &dist)
{
  float counter = normal * (u - a);
  float divider = normal * (v - u);
  if (divider == 0)
  {
    isect = v;
    return false;
  }

  dist = -counter / divider;
  isect = u + dist * (v - u);
  return true;
}

bool csIntersect3::Plane (
  const csVector3 &u,
  const csVector3 &v,
  const csPlane3 &p,
  csVector3 &isect,
  float &dist)
{
  float denom;
  csVector3 vu = v - u;

  denom = p.norm * vu;
  if (denom == 0)
  {
    // they are parallel
    isect = v;
    return false;
  }
  dist = -(p.norm * u + p.DD) / denom;
  if (dist < -SMALL_EPSILON || dist > 1 + SMALL_EPSILON) return false;

  isect = u + dist * vu;
  return true;
}

bool csIntersect3::Planes (
  const csPlane3 &p1,
  const csPlane3 &p2,
  const csPlane3 &p3,
  csVector3 &isect)
{
  //To find the one point that is on all three planes, we need to solve
  //the following equation system (we need to find the x, y and z which
  //are true for all equations):
  // A1*x+B1*y+C1*z+D1=0 //plane1
  // A2*x+B2*y+C2*z+D2=0 //plane2
  // A3*x+B3*y+C3*z+D3=0 //plane3
  //This can be solved according to Cramers rule by looking at the
  //determinants of the equation system.
  csMatrix3 mdet (
              p1.A (),
              p1.B (),
              p1.C (),
              p2.A (),
              p2.B (),
              p2.C (),
              p3.A (),
              p3.B (),
              p3.C ());
  float det = mdet.Determinant ();
  if (det == 0) return false; //some planes are parallel.
  csMatrix3 mx (
              -p1.D (),
              p1.B (),
              p1.C (),
              -p2.D (),
              p2.B (),
              p2.C (),
              -p3.D (),
              p3.B (),
              p3.C ());
  float xdet = mx.Determinant ();

  csMatrix3 my (
              p1.A (),
              -p1.D (),
              p1.C (),
              p2.A (),
              -p2.D (),
              p2.C (),
              p3.A (),
              -p3.D (),
              p3.C ());
  float ydet = my.Determinant ();

  csMatrix3 mz (
              p1.A (),
              p1.B (),
              -p1.D (),
              p2.A (),
              p2.B (),
              -p2.D (),
              p3.A (),
              p3.B (),
              -p3.D ());
  float zdet = mz.Determinant ();

  isect.x = xdet / det;
  isect.y = ydet / det;
  isect.z = zdet / det;
  return true;
}

bool csIntersect3::PlaneXPlane (const csPlane3 &p1, float x2, csPlane2 &isect)
{
  // p1: A*x+B*y+C*z+D=0
  if (ABS (p1.B ()) < SMALL_EPSILON && ABS (p1.C ()) < SMALL_EPSILON)
    return false;
  isect.Set (p1.B (), p1.C (), p1.D () + x2 * p1.A ());
  return true;
}

bool csIntersect3::PlaneYPlane (const csPlane3 &p1, float y2, csPlane2 &isect)
{
  // p1: A*x+B*y+C*z+D=0
  if (ABS (p1.A ()) < SMALL_EPSILON && ABS (p1.C ()) < SMALL_EPSILON)
    return false;
  isect.Set (p1.A (), p1.C (), p1.D () + y2 * p1.B ());
  return true;
}

bool csIntersect3::PlaneZPlane (const csPlane3 &p1, float z2, csPlane2 &isect)
{
  // p1: A*x+B*y+C*z+D=0
  if (ABS (p1.A ()) < SMALL_EPSILON && ABS (p1.B ()) < SMALL_EPSILON)
    return false;
  isect.Set (p1.A (), p1.B (), p1.D () + z2 * p1.C ());
  return true;
}

float csIntersect3::Z0Plane (
  const csVector3 &u,
  const csVector3 &v,
  csVector3 &isect)
{
  float r = u.z / (u.z - v.z);
  isect.x = r * (v.x - u.x) + u.x;
  isect.y = r * (v.y - u.y) + u.y;
  isect.z = 0;
  return r;
}

float csIntersect3::ZPlane (
  float zval,
  const csVector3 &u,
  const csVector3 &v,
  csVector3 &isect)
{
  float r = (zval - u.z) / (v.z - u.z);
  isect.x = r * (v.x - u.x) + u.x;
  isect.y = r * (v.y - u.y) + u.y;
  isect.z = zval;
  return r;
}

float csIntersect3::XFrustum (
  float A,
  const csVector3 &u,
  const csVector3 &v,
  csVector3 &isect)
{
  float r = (A * u.x + u.z) / (A * (u.x - v.x) + u.z - v.z);
  isect.x = r * (v.x - u.x) + u.x;
  isect.y = r * (v.y - u.y) + u.y;
  isect.z = r * (v.z - u.z) + u.z;
  return r;
}

float csIntersect3::YFrustum (
  float B,
  const csVector3 &u,
  const csVector3 &v,
  csVector3 &isect)
{
  float r = (B * u.y + u.z) / (B * (u.y - v.y) + u.z - v.z);
  isect.x = r * (v.x - u.x) + u.x;
  isect.y = r * (v.y - u.y) + u.y;
  isect.z = r * (v.z - u.z) + u.z;
  return r;
}

int csIntersect3::BoxSegment (
  const csBox3 &box,
  const csSegment3 &seg,
  csVector3 &isect,
  float *pr)
{
  const csVector3 &u = seg.Start ();
  const csVector3 &v = seg.End ();
  float r, plane_pos = 0;
  int sides[3];
  int num_sides = box.GetVisibleSides (u, sides);
  int i;

  // If there are no sides then we're in the box so we can return true.
  if (num_sides == 0)
  {
    isect = u;
    if (pr) *pr = 0;
    return CS_BOX_INSIDE;
  }

  for (i = 0; i < num_sides; i++)
  {
    switch (sides[i])
    {
      case CS_BOX_SIDE_x:
        plane_pos = box.MinX ();

      // Fall THRU...
      case CS_BOX_SIDE_X:
        if (sides[i] == CS_BOX_SIDE_X) plane_pos = box.MaxX ();
        if (ABS (v.x - u.x) > SMALL_EPSILON)
        {
          r = (plane_pos - u.x) / (v.x - u.x);
          if (r < 0 || r > 1) break;
          isect.x = plane_pos;
          isect.y = r * (v.y - u.y) + u.y;
          isect.z = r * (v.z - u.z) + u.z;
          if (
            isect.y >= box.MinY () &&
            isect.y <= box.MaxY () &&
            isect.z >= box.MinZ () &&
            isect.z <= box.MaxZ ())
          {
            if (pr) *pr = r;
            return sides[i];
          }
        }
        break;
      case CS_BOX_SIDE_y:
        plane_pos = box.MinY ();

      // Fall THRU...
      case CS_BOX_SIDE_Y:
        if (sides[i] == CS_BOX_SIDE_Y) plane_pos = box.MaxY ();
        if (ABS (v.y - u.y) > SMALL_EPSILON)
        {
          r = (plane_pos - u.y) / (v.y - u.y);
          if (r < 0 || r > 1) break;
          isect.x = r * (v.x - u.x) + u.x;
          isect.y = plane_pos;
          isect.z = r * (v.z - u.z) + u.z;
          if (
            isect.x >= box.MinX () &&
            isect.x <= box.MaxX () &&
            isect.z >= box.MinZ () &&
            isect.z <= box.MaxZ ())
          {
            if (pr) *pr = r;
            return sides[i];
          }
        }
        break;
      case CS_BOX_SIDE_z:
        plane_pos = box.MinZ ();

      // Fall THRU...
      case CS_BOX_SIDE_Z:
        if (sides[i] == CS_BOX_SIDE_Z) plane_pos = box.MaxZ ();
        if (ABS (v.z - u.z) > SMALL_EPSILON)
        {
          r = (plane_pos - u.z) / (v.z - u.z);
          if (r < 0 || r > 1) break;
          isect.x = r * (v.x - u.x) + u.x;
          isect.y = r * (v.y - u.y) + u.y;
          isect.z = plane_pos;
          if (
            isect.x >= box.MinX () &&
            isect.x <= box.MaxX () &&
            isect.y >= box.MinY () &&
            isect.y <= box.MaxY ())
          {
            if (pr) *pr = r;
            return sides[i];
          }
        }
        break;
    }
  }

  return -1;
}

bool csIntersect3::BoxFrustum (const csBox3& box, csPlane3* f,
	uint32 inClipMask, uint32& outClipMask)
{
  csVector3 m = box.GetCenter ();
  csVector3 d = box.Max ()-m;		// Half-diagonal.
  uint32 mk = 1;
  outClipMask = 0;

  // Loop for all active planes.
  while (mk <= inClipMask)
  {
    if (inClipMask & mk)
    {
      // This is an active clip plane.
      float NP = (float)(d.x*fabs(f->A ())+d.y*fabs(f->B ())+d.z*fabs(f->C ()));
      float MP = f->Classify (m);
      if ((MP+NP) < 0.0f) return false;       // behind clip plane
      if ((MP-NP) < 0.0f) outClipMask |= mk;
    }
    mk += mk;
    f++;
  }
  return true;	// There is an intersection.
}


//-------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csGeomDebugHelper)
  SCF_IMPLEMENTS_INTERFACE(iDebugHelper)
SCF_IMPLEMENT_IBASE_END

csGeomDebugHelper::csGeomDebugHelper ()
{
  SCF_CONSTRUCT_IBASE (NULL);
}

#define GEO_ASSERT(test,msg) \
  if (!(test)) \
  { \
    csString ss; \
    ss.Format ("csGeom failure (%d,%s): %s\n", int(__LINE__), \
    	#msg, #test); \
    str.Append (ss); \
    return rc; \
  }

/**
 * A cube mesh for unit testing.
 */
class UnitCubeMesh : public iPolygonMesh
{
private:
  csVector3 verts[8];
  csMeshedPolygon poly[6];
  int vertices[4*6];
  
public:
  UnitCubeMesh ();

  ///---------------------- iPolygonMesh implementation ----------------------
  SCF_DECLARE_IBASE;
  /// Get the number of vertices for this mesh.
  virtual int GetVertexCount () { return 8; }
  /// Get the pointer to the array of vertices.
  virtual csVector3* GetVertices () { return verts; }
  /// Get the number of polygons for this mesh.
  virtual int GetPolygonCount () { return 6; }
  /// Get the pointer to the array of polygons.
  virtual csMeshedPolygon* GetPolygons () { return poly; }
  /// Cleanup.
  virtual void Cleanup () { }
};

SCF_IMPLEMENT_IBASE (UnitCubeMesh)
  SCF_IMPLEMENTS_INTERFACE (iPolygonMesh)
SCF_IMPLEMENT_IBASE_END

UnitCubeMesh::UnitCubeMesh ()
{
  SCF_CONSTRUCT_IBASE (NULL);
  csVector3 dim (1, 1, 1);
  csVector3 d = dim * .5;
  verts[0].Set (-d.x, -d.y, -d.z);
  verts[1].Set ( d.x, -d.y, -d.z);
  verts[2].Set (-d.x, -d.y,  d.z);
  verts[3].Set ( d.x, -d.y,  d.z);
  verts[4].Set (-d.x,  d.y, -d.z);
  verts[5].Set ( d.x,  d.y, -d.z);
  verts[6].Set (-d.x,  d.y,  d.z);
  verts[7].Set ( d.x,  d.y,  d.z);
  int i;
  for (i = 0 ; i < 6 ; i++)
  {
    poly[i].num_vertices = 4;
    poly[i].vertices = &vertices[i*4];
  }
  vertices[0*4+0] = 4;
  vertices[0*4+1] = 5;
  vertices[0*4+2] = 1;
  vertices[0*4+3] = 0;
  vertices[1*4+0] = 5;
  vertices[1*4+1] = 7;
  vertices[1*4+2] = 3;
  vertices[1*4+3] = 1;
  vertices[2*4+0] = 7;
  vertices[2*4+1] = 6;
  vertices[2*4+2] = 2;
  vertices[2*4+3] = 3;
  vertices[3*4+0] = 6;
  vertices[3*4+1] = 4;
  vertices[3*4+2] = 0;
  vertices[3*4+3] = 2;
  vertices[4*4+0] = 6;
  vertices[4*4+1] = 7;
  vertices[4*4+2] = 5;
  vertices[4*4+3] = 4;
  vertices[5*4+0] = 0;
  vertices[5*4+1] = 1;
  vertices[5*4+2] = 3;
  vertices[5*4+3] = 2;
}

static bool ContainsEdge (csPolygonMeshEdge* edges, int num_edges,
	int vt1, int vt2)
{
  int i;
  for (i = 0 ; i < num_edges ; i++)
  {
    if (edges[i].vt1 == vt1 && edges[i].vt2 == vt2) return true;
  }
  return false;
}

static bool ContainsTwoPoly (csPolygonMeshEdge* edges, int num_edges,
	int poly1, int poly2)
{
  int i;
  for (i = 0 ; i < num_edges ; i++)
  {
    if (edges[i].poly1 == poly1 && edges[i].poly2 == poly2) return true;
    if (edges[i].poly2 == poly1 && edges[i].poly1 == poly2) return true;
  }
  return false;
}

static bool ContainsEdge (int* outline_edges, int num_outline_edges,
	int vt1, int vt2)
{
  int i;
  for (i = 0 ; i < num_outline_edges ; i++)
  {
    int e1 = *outline_edges++;
    int e2 = *outline_edges++;
    if (e1 == vt1 && e2 == vt2) return true;
  }
  return false;
}

iString* csGeomDebugHelper::UnitTest ()
{
  scfString* rc = new scfString ();
  csString& str = rc->GetCsString ();

  //==========================================================================
  // Tests for csIntersect3::BoxSegment.
  //==========================================================================

  csSegment3 seg (csVector3 (0, 0, 0), csVector3 (0, 0, 100));
  csBox3 b (-10, -10, 50, 10, 10, 70);
  float r;
  csVector3 isect;
  int result = csIntersect3::BoxSegment (b, seg, isect, &r);
  GEO_ASSERT (result == CS_BOX_SIDE_z, "BoxSegment");
  GEO_ASSERT (isect.x == 0 && isect.y == 0 && ABS (isect.z-50.0) < .00001,
  	"BoxSegment");
  GEO_ASSERT (ABS (r-.5) < .00001, "BoxSegment");

  //==========================================================================
  // Tests for csPolygonMeshTools.
  //==========================================================================

  UnitCubeMesh* mesh = new UnitCubeMesh ();
  int num_edges;
  csPolygonMeshEdge* edges = csPolygonMeshTools::CalculateEdges (
  	mesh, num_edges);
  GEO_ASSERT (num_edges == 12, "edges");
  GEO_ASSERT (ContainsEdge (edges, num_edges, 0, 1), "contains edge");
  GEO_ASSERT (ContainsEdge (edges, num_edges, 0, 2), "contains edge");
  GEO_ASSERT (ContainsEdge (edges, num_edges, 0, 4), "contains edge");
  GEO_ASSERT (ContainsEdge (edges, num_edges, 1, 3), "contains edge");
  GEO_ASSERT (ContainsEdge (edges, num_edges, 1, 5), "contains edge");
  GEO_ASSERT (ContainsEdge (edges, num_edges, 2, 3), "contains edge");
  GEO_ASSERT (ContainsEdge (edges, num_edges, 2, 6), "contains edge");
  GEO_ASSERT (ContainsEdge (edges, num_edges, 3, 7), "contains edge");
  GEO_ASSERT (ContainsEdge (edges, num_edges, 4, 5), "contains edge");
  GEO_ASSERT (ContainsEdge (edges, num_edges, 4, 6), "contains edge");
  GEO_ASSERT (ContainsEdge (edges, num_edges, 5, 7), "contains edge");
  GEO_ASSERT (ContainsEdge (edges, num_edges, 6, 7), "contains edge");
  GEO_ASSERT (ContainsTwoPoly (edges, num_edges, 0, 1), "contains 2poly");
  GEO_ASSERT (ContainsTwoPoly (edges, num_edges, 1, 2), "contains 2poly");
  GEO_ASSERT (ContainsTwoPoly (edges, num_edges, 2, 3), "contains 2poly");
  GEO_ASSERT (ContainsTwoPoly (edges, num_edges, 3, 0), "contains 2poly");
  GEO_ASSERT (ContainsTwoPoly (edges, num_edges, 4, 0), "contains 2poly");
  GEO_ASSERT (ContainsTwoPoly (edges, num_edges, 4, 1), "contains 2poly");
  GEO_ASSERT (ContainsTwoPoly (edges, num_edges, 4, 2), "contains 2poly");
  GEO_ASSERT (ContainsTwoPoly (edges, num_edges, 4, 3), "contains 2poly");
  GEO_ASSERT (ContainsTwoPoly (edges, num_edges, 5, 0), "contains 2poly");
  GEO_ASSERT (ContainsTwoPoly (edges, num_edges, 5, 1), "contains 2poly");
  GEO_ASSERT (ContainsTwoPoly (edges, num_edges, 5, 2), "contains 2poly");
  GEO_ASSERT (ContainsTwoPoly (edges, num_edges, 5, 3), "contains 2poly");

  csPlane3 planes[6];
  csPolygonMeshTools::CalculatePlanes (mesh, planes);
  int aedge = csPolygonMeshTools::CheckActiveEdges (edges, num_edges, planes);
  GEO_ASSERT (aedge == 12, "active edges");
  GEO_ASSERT (edges[0].active == true, "active edge");
  GEO_ASSERT (edges[1].active == true, "active edge");
  GEO_ASSERT (edges[2].active == true, "active edge");
  GEO_ASSERT (edges[3].active == true, "active edge");
  GEO_ASSERT (edges[4].active == true, "active edge");
  GEO_ASSERT (edges[5].active == true, "active edge");
  GEO_ASSERT (edges[6].active == true, "active edge");
  GEO_ASSERT (edges[7].active == true, "active edge");
  GEO_ASSERT (edges[8].active == true, "active edge");
  GEO_ASSERT (edges[9].active == true, "active edge");
  GEO_ASSERT (edges[10].active == true, "active edge");
  GEO_ASSERT (edges[11].active == true, "active edge");

  int outline_edges[24];
  bool outline_verts[8];
  int num_outline_edges;
  float valid_radius;

  csPolygonMeshTools::CalculateOutline (edges, num_edges,
  	planes, mesh->GetVertexCount (),
	csVector3 (0, 0, -10),
	outline_edges, num_outline_edges,
	outline_verts,
	valid_radius);
  GEO_ASSERT (valid_radius > 0.49, "radius");
  GEO_ASSERT (valid_radius < 0.51, "radius");
  GEO_ASSERT (num_outline_edges == 4, "outline edges");
  GEO_ASSERT (outline_verts[0] == true, "cont vt");
  GEO_ASSERT (outline_verts[1] == true, "cont vt");
  GEO_ASSERT (outline_verts[2] == false, "cont vt");
  GEO_ASSERT (outline_verts[3] == false, "cont vt");
  GEO_ASSERT (outline_verts[4] == true, "cont vt");
  GEO_ASSERT (outline_verts[5] == true, "cont vt");
  GEO_ASSERT (outline_verts[6] == false, "cont vt");
  GEO_ASSERT (outline_verts[7] == false, "cont vt");
  GEO_ASSERT (ContainsEdge (outline_edges, num_outline_edges, 0, 1), "cont e");
  GEO_ASSERT (ContainsEdge (outline_edges, num_outline_edges, 0, 4), "cont e");
  GEO_ASSERT (ContainsEdge (outline_edges, num_outline_edges, 1, 5), "cont e");
  GEO_ASSERT (ContainsEdge (outline_edges, num_outline_edges, 4, 5), "cont e");

  csPolygonMeshTools::CalculateOutline (edges, num_edges,
  	planes, mesh->GetVertexCount (),
	csVector3 (2, 0, -2),
	outline_edges, num_outline_edges,
	outline_verts,
	valid_radius);
  GEO_ASSERT (valid_radius > 0.49, "radius");
  GEO_ASSERT (valid_radius < 0.51, "radius");
  GEO_ASSERT (num_outline_edges == 6, "outline edges");
  GEO_ASSERT (outline_verts[0] == true, "cont vt");
  GEO_ASSERT (outline_verts[1] == true, "cont vt");
  GEO_ASSERT (outline_verts[2] == false, "cont vt");
  GEO_ASSERT (outline_verts[3] == true, "cont vt");
  GEO_ASSERT (outline_verts[4] == true, "cont vt");
  GEO_ASSERT (outline_verts[5] == true, "cont vt");
  GEO_ASSERT (outline_verts[6] == false, "cont vt");
  GEO_ASSERT (outline_verts[7] == true, "cont vt");
  GEO_ASSERT (ContainsEdge (outline_edges, num_outline_edges, 0, 1), "cont e");
  GEO_ASSERT (ContainsEdge (outline_edges, num_outline_edges, 0, 4), "cont e");
  GEO_ASSERT (ContainsEdge (outline_edges, num_outline_edges, 4, 5), "cont e");
  GEO_ASSERT (ContainsEdge (outline_edges, num_outline_edges, 1, 3), "cont e");
  GEO_ASSERT (ContainsEdge (outline_edges, num_outline_edges, 3, 7), "cont e");
  GEO_ASSERT (ContainsEdge (outline_edges, num_outline_edges, 5, 7), "cont e");
 
  csPolygonMeshTools::CalculateOutline (edges, num_edges,
  	planes, mesh->GetVertexCount (),
	csVector3 (2, 2, -2),
	outline_edges, num_outline_edges,
	outline_verts,
	valid_radius);
  GEO_ASSERT (valid_radius > 1.49, "radius");
  GEO_ASSERT (valid_radius < 1.51, "radius");
  GEO_ASSERT (num_outline_edges == 6, "outline edges");
  GEO_ASSERT (outline_verts[0] == true, "cont vt");
  GEO_ASSERT (outline_verts[1] == true, "cont vt");
  GEO_ASSERT (outline_verts[2] == false, "cont vt");
  GEO_ASSERT (outline_verts[3] == true, "cont vt");
  GEO_ASSERT (outline_verts[4] == true, "cont vt");
  GEO_ASSERT (outline_verts[5] == false, "cont vt");
  GEO_ASSERT (outline_verts[6] == true, "cont vt");
  GEO_ASSERT (outline_verts[7] == true, "cont vt");
  GEO_ASSERT (ContainsEdge (outline_edges, num_outline_edges, 0, 1), "cont e");
  GEO_ASSERT (ContainsEdge (outline_edges, num_outline_edges, 1, 3), "cont e");
  GEO_ASSERT (ContainsEdge (outline_edges, num_outline_edges, 3, 7), "cont e");
  GEO_ASSERT (ContainsEdge (outline_edges, num_outline_edges, 6, 7), "cont e");
  GEO_ASSERT (ContainsEdge (outline_edges, num_outline_edges, 4, 6), "cont e");
  GEO_ASSERT (ContainsEdge (outline_edges, num_outline_edges, 0, 4), "cont e");
 
  // Border case.
  csPolygonMeshTools::CalculateOutline (edges, num_edges,
  	planes, mesh->GetVertexCount (),
	csVector3 (.5, 0, -10),
	outline_edges, num_outline_edges,
	outline_verts,
	valid_radius);
  GEO_ASSERT (valid_radius < 0.01, "radius");

  delete[] edges;
  delete mesh;

  //==========================================================================
  // Tests for csOBB.
  //==========================================================================

  csVector3 vertex_table[4];
  vertex_table[0].Set (10, 10, 10);
  vertex_table[1].Set (0, 0, 2);
  vertex_table[2].Set (1, 0, 3);
  vertex_table[3].Set (2, 0, 0);
  csOBB obb;
  obb.FindOBB (vertex_table, 4);
  int i;
  for (i = 0 ; i < 8 ; i++)
  {
    csVector3 v = obb.GetCorner (i);
    printf ("%d %g,%g,%g\n", i, v.x, v.y, v.z);
    fflush (stdout);
  }
  obb.FindOBBAccurate (vertex_table, 4);
  for (i = 0 ; i < 8 ; i++)
  {
    csVector3 v = obb.GetCorner (i);
    printf ("%d %g,%g,%g\n", i, v.x, v.y, v.z);
    fflush (stdout);
  }

  rc->DecRef ();
  return NULL;
}

