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
#include "iengine/sector.h"
#include "csgeom/transfrm.h"
#include "rain.h"
#include "ivideo/material.h"
#include "iengine/material.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "imesh/thing/polygon.h"
#include "qsqrt.h"
#include <math.h>
#include <stdlib.h>

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE_EXT (csRainMeshObject)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iRainState)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csRainMeshObject::RainState)
  SCF_IMPLEMENTS_INTERFACE (iRainState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

void csRainMeshObject::SetupObject ()
{
  if (!initialized)
  {
    RemoveParticles ();
    initialized = true;
    delete[] part_pos;
    delete[] drop_stop;

    part_pos = new csVector3[number];
    drop_stop = new float[number];

    bbox = rainbox;
    /// spread particles evenly through box
    csVector3 size = rainbox.Max () - rainbox.Min ();

    // Calculate the maximum radius.
    float max_size = size.x;
    if (size.y > max_size) max_size = size.y;
    if (size.z > max_size) max_size = size.z;
    float a = max_size/2.;
    radius = qsqrt (a*a + a*a);

    csVector3 pos;
    int i;
    for (i=0 ; i < number ; i++)
    {
      AppendRectSprite (drop_width, drop_height, mat, lighted_particles);
      GetParticle (i)->SetMixMode (MixMode);
      pos = GetRandomDirection (size, rainbox.Min ()) ;
      GetParticle (i)->SetPosition (pos);
      part_pos[i] = pos;
      if (useCD)
        drop_stop[i] = FindStopHeight(i);
    }
    SetupColor ();
    SetupMixMode ();
  }
}

csRainMeshObject::csRainMeshObject (iObjectRegistry* object_reg,
  iMeshObjectFactory* factory) : csParticleSystem (object_reg, factory)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiRainState);
  part_pos  = NULL;
  drop_stop = NULL;
  sector    = NULL;
  useCD     = false;
  rainbox.Set (csVector3 (0, 0, 0), csVector3 (1, 1, 1));
  rain_dir.Set (0, -1, 0);
  drop_width = drop_height = .1;
  lighted_particles = false;
  number = 50;
}

csRainMeshObject::~csRainMeshObject()
{
  delete[] part_pos;
  delete[] drop_stop;
}


void csRainMeshObject::Update (csTicks elapsed_time)
{
  SetupObject ();
  csParticleSystem::Update (elapsed_time);
  float delta_t = elapsed_time / 1000.0f; // in seconds
  // move particles;
  csVector3 move, pos;
  int i;
  for (i=0 ; i < particles.Length () ; i++)
  {
    move = rain_dir * delta_t;
    part_pos[i] += move;
    GetParticle (i)->SetPosition (part_pos[i]);
  }
  // check if particles are out of the box.
  for (i=0 ; i < particles.Length () ; i++)
  {
    if (!rainbox.In(part_pos[i]) ||
        (useCD && part_pos[i].y < drop_stop[i]) )
    {
      // this particle has left the box.
      // it will disappear.
      // To keep the number of particles (and thus the raininess)
      // constant another particle will appear in sight now.
      // @@@ rain only appears in box ceiling now, should appear on
      // opposite side of rain_dir...

      // @@@ also shifty will not work very nicely with slanted rain.
      //   but perhaps it won't be too bad...
      float toolow = ABS(rainbox.MinY() - part_pos[i].y);
      float height = rainbox.MaxY() - rainbox.MinY();
      while (toolow>height) toolow-=height;
      pos = GetRandomDirection( csVector3 (rainbox.MaxX() - rainbox.MinX(),
        0.0f, rainbox.MaxZ() - rainbox.MinZ()), rainbox.Min() );
      pos.y = rainbox.MaxY() - toolow;
      if(pos.y < rainbox.MinY() || pos.y > rainbox.MaxY())
        pos.y = rainbox.MaxY() - height * ((float)rand() / (1.0 + RAND_MAX));
      GetParticle (i)->SetPosition (pos);
      part_pos[i] = pos;
      if (useCD)
        drop_stop[i] = FindStopHeight(i);
    }
  }
}

void csRainMeshObject::SetCollisionDetection(bool cd)
{
  useCD = cd;
  if (cd && logparent)
  {
    csRef<iMeshWrapper> mesh = SCF_QUERY_INTERFACE (logparent, iMeshWrapper);
    if (mesh)
    {
      // First sector is only one considered.
      sector = mesh->GetMovable()->GetSectors()->Get(0);
    }
    else sector = NULL;
  }
};

float csRainMeshObject::FindStopHeight(int i)
{
  if (!sector)  // this is only set when SetCD true is successful
    return rainbox.MinY();

  csVector3 isect, end = part_pos[i] + rain_dir
  	* (rainbox.MaxY() - rainbox.MinY() );

  iPolygon3D* poly = NULL;
  sector->HitBeam (part_pos[i], end, isect, &poly);
  
  if (poly)  // found hit
    return isect.y;         // where it hit
  else
    return rainbox.MinY();  // bottom of bounding box
}

void csRainMeshObject::HardTransform (const csReversibleTransform& /*t*/)
{
}

//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csRainMeshObjectFactory)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectFactory)
SCF_IMPLEMENT_IBASE_END

csRainMeshObjectFactory::csRainMeshObjectFactory (iBase* p, iObjectRegistry* s)
{
  SCF_CONSTRUCT_IBASE (p);
  object_reg = s;
  logparent = NULL;
}

csRainMeshObjectFactory::~csRainMeshObjectFactory ()
{
}

csPtr<iMeshObject> csRainMeshObjectFactory::NewInstance ()
{
  csRainMeshObject* cm =
    new csRainMeshObject (object_reg, (iMeshObjectFactory*)this);
  csRef<iMeshObject> im (SCF_QUERY_INTERFACE (cm, iMeshObject));
  cm->DecRef ();
  return csPtr<iMeshObject> (im);
}

//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csRainMeshObjectType)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectType)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csRainMeshObjectType::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csRainMeshObjectType)

SCF_EXPORT_CLASS_TABLE (rain)
  SCF_EXPORT_CLASS (csRainMeshObjectType, "crystalspace.mesh.object.rain",
    "Crystal Space Rain Mesh Type")
SCF_EXPORT_CLASS_TABLE_END

csRainMeshObjectType::csRainMeshObjectType (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csRainMeshObjectType::~csRainMeshObjectType ()
{
}

csPtr<iMeshObjectFactory> csRainMeshObjectType::NewFactory ()
{
  csRainMeshObjectFactory* cm = new csRainMeshObjectFactory (this, object_reg);
  csRef<iMeshObjectFactory> ifact (
  	SCF_QUERY_INTERFACE (cm, iMeshObjectFactory));
  cm->DecRef ();
  return csPtr<iMeshObjectFactory> (ifact);
}

