/*
    Copyright (C) 1998 by Jorrit Tyberghein
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
#include "sysdef.h"
#include "csgeom/transfrm.h"

//---------------------------------------------------------------------------

csTransform csTransform::GetReflect (const csPlane3& pl)
{
  // Suppose that n is the plane normal in the direction of th reflection.
  // Suppose that u is the unit vector in the direction of the reflection
  // normal.  For any vector v, the component of v in the direction of
  // u is equal to (v * u) * u.  Thus, if v is reflected across a plane
  // through the origin with the given normal, the resulting vector is
  //  v' = v - 2 * [ (v * u) * u ] = v - 2 [ (v * n) * n ] / (n * n)
  //
  // x = <1,0,0>  =>  x' = <1,0,0> - 2 ( n.x * n ) / (n*n)
  // y = <0,1,0>  =>  y' = <0,1,0> - 2 ( n.y * n ) / (n*n)
  // z = <0,0,1>  =>  z' = <0,0,1> - 2 ( n.z * n ) / (n*n)
  //
  // 3x3 transformation matrix = [x' y' z']

  float i_normsq = 1/(pl.norm*pl.norm);
  csVector3 xvec = (-2 * pl.norm.x * i_normsq) * pl.norm;
  csVector3 yvec = (-2 * pl.norm.y * i_normsq) * pl.norm;
  csVector3 zvec = (-2 * pl.norm.z * i_normsq) * pl.norm;
  xvec.x+=1;  yvec.y+=1;  zvec.z+=1;

  return csTransform (csMatrix3 ( xvec.x, yvec.x, zvec.x,
                                  xvec.y, yvec.y, zvec.y,
                                  xvec.z, yvec.z, zvec.z ),
                /* neworig = */ (-2 * pl.DD * i_normsq ) * pl.norm );
}

//---------------------------------------------------------------------------

csPlane3 csTransform::Other2This (const csPlane3& p) const
{
  csVector3 newnorm = m_o2t*p.norm;
// let N represent norm <A,B,C>, and X represent point <x,y,z>
//
// Old plane equation: N*X + D = 0
// There exists point X = <r*A,r*B,r*C> = r*N which satisfies the
// plane equation.  
//  => r*(N*N) + D = 0
//  => r = -D/(N*N)
//
// New plane equation: N'*X' + D' = 0
// If M is the transformation matrix, and V the transformation vector,
// N' = M*N, and X' = M*(X-V).  Assume that N' is already calculated.
//  => N'*(M*(X-V)) + D' = 0
//  => D' = -N'*(M*X) + N'*(M*V)
//        = -N'*(M*(r*N)) + N'*(M*V)
//        = -r*(N'*N') + N'*(M*V) = D*(N'*N')/(N*N) + N'*(M*V)
// Since N' is a rotation of N, (N'*N') = (N*N), thus
//  D' = D + N'*(M*V)
//
  return csPlane3 (newnorm, p.DD + newnorm * (m_o2t*v_o2t) );
}

void csTransform::Other2This (const csPlane3& p, const csVector3& point,
                           csPlane3& result) const
{
  result.norm = m_o2t*p.norm;
  result.DD = - (result.norm * point);
}

csVector3 operator* (const csVector3& v, const csTransform& t)
{ return t.Other2This (v); }

csVector3 operator* (const csTransform& t, const csVector3& v)
{ return t.Other2This (v); }

csVector3& operator*= (csVector3& v, const csTransform& t)
{ v = t.Other2This (v); return v; }

csPlane3 operator* (const csPlane3& p, const csTransform& t)
{ return t.Other2This (p); }

csPlane3 operator* (const csTransform& t, const csPlane3& p)
{ return t.Other2This (p); }

csPlane3& operator*= (csPlane3& p, const csTransform& t)
{
  p.norm = t.m_o2t * p.norm;
  p.DD += p.norm * (t.m_o2t*t.v_o2t);
  return p;
}

csMatrix3 operator* (const csMatrix3& m, const csTransform& t)
{ return m*t.m_o2t; }

csMatrix3 operator* (const csTransform& t, const csMatrix3& m) 
{ return t.m_o2t*m; }

csMatrix3& operator*= (csMatrix3& m, const csTransform& t) 
{ return m *= t.m_o2t; }

//---------------------------------------------------------------------------

csPlane3 csReversibleTransform::This2Other (const csPlane3& p) const
{
  csVector3 newnorm = m_t2o * p.norm;
  return csPlane3 (newnorm, p.DD - p.norm * (m_o2t*v_o2t) );
}

void csReversibleTransform::This2Other (const csPlane3& p, const csVector3& point,
                           csPlane3& result) const
{
  result.norm = m_t2o*p.norm;
  result.DD = - (result.norm * point);
}

csVector3 operator/ (const csVector3& v, const csReversibleTransform& t) 
{ return t.This2Other (v); }

csVector3& operator/= (csVector3& v, const csReversibleTransform& t) 
{ v = t.This2Other (v); return v; }

csPlane3 operator/ (const csPlane3& p, const csReversibleTransform& t)
{ return t.This2Other (p); }

csPlane3& operator/= (csPlane3& p, const csReversibleTransform& t)
{
  p.DD -= p.norm * (t.m_o2t*t.v_o2t);
  p.norm = t.m_t2o * p.norm;
  return p;
}

csReversibleTransform& operator*= (csReversibleTransform& t1,
                                 const csReversibleTransform& t2)
{
  t1.v_o2t = t2.v_o2t + t2.m_t2o*t1.v_o2t;
  t1.m_o2t *= t2.m_o2t;
  t1.m_t2o = t2.m_t2o*t1.m_t2o;
  return t1;
}

csReversibleTransform operator* (const csReversibleTransform& t1,
                               const csReversibleTransform& t2)
{
  return csReversibleTransform (t1.m_o2t*t2.m_o2t, t2.m_t2o*t1.m_t2o, 
                             t2.v_o2t + t2.m_t2o*t1.v_o2t); 
}

csTransform operator* (const csTransform& t1, const csReversibleTransform& t2)
{
  return csTransform (t1.m_o2t*t2.m_o2t, t2.v_o2t + t2.m_t2o*t1.v_o2t);
}

csReversibleTransform& operator/=(csReversibleTransform& t1,
                                const csReversibleTransform& t2)
{
 t1.v_o2t = t2.m_o2t*(t1.v_o2t - t2.v_o2t);
 t1.m_o2t *= t2.m_t2o;
 t1.m_t2o = t2.m_o2t*t1.m_t2o;
 return t1;
}

csReversibleTransform operator/ (const csReversibleTransform& t1,
                               const csReversibleTransform& t2)
{
  return csReversibleTransform (t1.m_o2t*t2.m_t2o, t2.m_o2t*t1.m_t2o, 
                   t2.m_o2t*(t1.v_o2t - t2.v_o2t)); 
}

//---------------------------------------------------------------------------
