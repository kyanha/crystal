/*
    Copyright (C) 2000 by Jorrit Tyberghein
    (C) W.C.A. Wijngaards, 2000

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
#include "csgeom/matrix3.h"
#include "csgeom/transfrm.h"
#include "spiral.h"
#include "ivideo/material.h"
#include "iengine/material.h"
#include "qsqrt.h"
#include <math.h>
#include <stdlib.h>

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE_EXT (csSpiralMeshObject)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iSpiralState)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSpiralMeshObject::SpiralState)
  SCF_IMPLEMENTS_INTERFACE (iSpiralState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

void csSpiralMeshObject::SetupObject ()
{
  if (!initialized)
  {
    initialized = true;
    RemoveParticles ();
    delete[] part_pos;
    delete[] part_age;
    part_pos = new csVector3[number];
    part_age = new float[number];

    csVector3 dim = part_speed*part_time;

    float fradius = dim.x;
    float height = dim.y;
    bbox.Set(source - csVector3(fradius,0,fradius),
      source + csVector3(fradius, height, fradius) );

    // Calculate the maximum radius.
    csVector3 size = bbox.Max () - bbox.Min ();
    float max_size = size.x;
    if (size.y > max_size) max_size = size.y;
    if (size.z > max_size) max_size = size.z;
    float a = max_size/2.;
    radius = qsqrt (a*a + a*a);

    // create particles
    int i;
    for (i=0 ; i<number ; i++)
    {
      RestartParticle(FindOldest(), (part_time / float(number)) * float(number-i));
    }

    time_left = 0.0;
    last_reuse = 0;
    SetupColor ();
    SetupMixMode ();
  }
}

csSpiralMeshObject::csSpiralMeshObject (iObjectRegistry* object_reg,
  iMeshObjectFactory* factory) : csParticleSystem (object_reg, factory)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSpiralState);
  number = 50;
  source.Set (0, 0, 0);
  part_source.Set (0, 0, 0);
  part_time = 5.0; // @@@ PARAMETER
  last_reuse = 0;
  part_pos = NULL;
  part_speed.Set(0.1,0.2,3.14);
  part_age = NULL;
  part_width = .02;
  part_height = .02;
  part_random.Set(.03, .03, 0.03);
}

csSpiralMeshObject::~csSpiralMeshObject()
{
}

void csSpiralMeshObject::SetPosition(int index)
{
  csVector3 pos;
  float radius = part_pos[index].x;
  float height = part_pos[index].y;
  float angle  = part_pos[index].z;
    
  pos = csVector3(cos(angle)*radius,height,sin(angle)*radius);
  
  GetParticle(index)->SetPosition(pos);
}

void csSpiralMeshObject::RestartParticle (int index, float pre_move)
{
  part_pos[index] = GetRandomDirection(part_random, // @@@ PARAMETER 
                                       part_source);
  part_pos[index] += part_speed*pre_move;
  SetPosition(index);
}

int csSpiralMeshObject::FindOldest ()
{
  int num = GetNumParticles ();
  int part_idx;
  if (num >= number)
  {
    part_idx = last_reuse;
    last_reuse = (last_reuse+1)%number;
  }
  else
  {
    AppendRectSprite (part_width, part_height, mat, false);	// @@@ PARAMETER
    part_idx = GetNumParticles ()-1;
    GetParticle(part_idx)->SetMixMode(MixMode);
  }
  return part_idx;
}


void csSpiralMeshObject::SetSource (const csVector3& source)  
{
  initialized = false;
  csSpiralMeshObject::source = source;
  part_source.Set( qsqrt(source.x*source.x+source.z*source.z),
                  source.y,
                  atan2(source.x,-source.z));
  scfiObjectModel.ShapeChanged ();
}

void csSpiralMeshObject::Update (csTicks elapsed_time)
{
  SetupObject ();
  csParticleSystem::Update (elapsed_time);
  float delta_t = elapsed_time / 1000.0f; // in seconds

  // Update position
  int i;
  for (i=0 ; i < particles.Length () ; i++)
  {
    part_pos[i] += part_speed* delta_t;
    SetPosition (i);
    part_age[i] += delta_t;
  } 
   
  // restart a number of particles
  float intersperse = part_time / (float)number;
  float todo_time = delta_t + time_left;
  while (todo_time > intersperse)
  {
    RestartParticle (FindOldest (), todo_time);
    todo_time -= intersperse;
  }
  time_left = todo_time;
}

void csSpiralMeshObject::HardTransform (const csReversibleTransform& t)
{
  source = t.This2Other (source);
  initialized = false;
  scfiObjectModel.ShapeChanged ();
}

//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csSpiralMeshObjectFactory)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectFactory)
SCF_IMPLEMENT_IBASE_END

csSpiralMeshObjectFactory::csSpiralMeshObjectFactory (iBase *p,
	iObjectRegistry* s)
{
  SCF_CONSTRUCT_IBASE (p);
  object_reg = s;
  logparent = 0;
}

csSpiralMeshObjectFactory::~csSpiralMeshObjectFactory ()
{
}

csPtr<iMeshObject> csSpiralMeshObjectFactory::NewInstance ()
{
  csSpiralMeshObject* cm =
    new csSpiralMeshObject (object_reg, (iMeshObjectFactory*)this);
  csRef<iMeshObject> im (SCF_QUERY_INTERFACE (cm, iMeshObject));
  cm->DecRef ();
  return csPtr<iMeshObject> (im);
}

//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csSpiralMeshObjectType)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectType)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSpiralMeshObjectType::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csSpiralMeshObjectType)


csSpiralMeshObjectType::csSpiralMeshObjectType (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csSpiralMeshObjectType::~csSpiralMeshObjectType ()
{
}

csPtr<iMeshObjectFactory> csSpiralMeshObjectType::NewFactory ()
{
  csSpiralMeshObjectFactory* cm = new csSpiralMeshObjectFactory (this,
  	object_reg);
  csRef<iMeshObjectFactory> ifact (
  	SCF_QUERY_INTERFACE (cm, iMeshObjectFactory));
  cm->DecRef ();
  return csPtr<iMeshObjectFactory> (ifact);
}

