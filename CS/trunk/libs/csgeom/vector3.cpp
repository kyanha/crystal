/*
    Copyright (C) 1998,1999,2000 by Jorrit Tyberghein
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
#include "csgeom/vector3.h"

//---------------------------------------------------------------------------

float csVector3::Norm () const
{ 
  return qsqrt (x*x + y*y + z*z); 
}

void csVector3::Normalize ()
{
  float invlen = qisqrt (x*x + y*y + z*z);
  *this *= invlen;
  //float len;
  //len = this->Norm ();
  //if (len > SMALL_EPSILON)
    //*this /= len;
}

csVector3::csVector3 (const csDVector3& v)
{
  x = (float)v.x;
  y = (float)v.y;
  z = (float)v.z;
}

//---------------------------------------------------------------------------
