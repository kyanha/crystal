/*
    Copyright (C) 2000 by Jorrit Tyberghein
    (C) W.C.A. Wijngaards, 2001

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
#include "emit.h"
#include "ivideo/material.h"
#include "iengine/material.h"
#include "qsqrt.h"
#include "qint.h"
#include "csgeom/math3d.h"
#include <math.h>
#include <stdlib.h>

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE (csEmitFixed)
  SCF_IMPLEMENTS_INTERFACE (iEmitGen3D)
SCF_IMPLEMENT_IBASE_END
SCF_IMPLEMENT_IBASE (csEmitSphere)
  SCF_IMPLEMENTS_INTERFACE (iEmitSphere)
  SCF_IMPLEMENTS_INTERFACE (iEmitGen3D)
SCF_IMPLEMENT_IBASE_END
SCF_IMPLEMENT_IBASE (csEmitBox)
  SCF_IMPLEMENTS_INTERFACE (iEmitBox)
  SCF_IMPLEMENTS_INTERFACE (iEmitGen3D)
SCF_IMPLEMENT_IBASE_END
SCF_IMPLEMENT_IBASE (csEmitCone)
  SCF_IMPLEMENTS_INTERFACE (iEmitCone)
  SCF_IMPLEMENTS_INTERFACE (iEmitGen3D)
SCF_IMPLEMENT_IBASE_END
SCF_IMPLEMENT_IBASE (csEmitMix)
  SCF_IMPLEMENTS_INTERFACE (iEmitMix)
  SCF_IMPLEMENTS_INTERFACE (iEmitGen3D)
SCF_IMPLEMENT_IBASE_END
SCF_IMPLEMENT_IBASE (csEmitCylinder)
  SCF_IMPLEMENTS_INTERFACE (iEmitCylinder)
  SCF_IMPLEMENTS_INTERFACE (iEmitGen3D)
SCF_IMPLEMENT_IBASE_END
SCF_IMPLEMENT_IBASE (csEmitLine)
  SCF_IMPLEMENTS_INTERFACE (iEmitLine)
  SCF_IMPLEMENTS_INTERFACE (iEmitGen3D)
SCF_IMPLEMENT_IBASE_END
SCF_IMPLEMENT_IBASE (csEmitSphereTangent)
  SCF_IMPLEMENTS_INTERFACE (iEmitSphereTangent)
  SCF_IMPLEMENTS_INTERFACE (iEmitGen3D)
SCF_IMPLEMENT_IBASE_END
SCF_IMPLEMENT_IBASE (csEmitCylinderTangent)
  SCF_IMPLEMENTS_INTERFACE (iEmitCylinderTangent)
  SCF_IMPLEMENTS_INTERFACE (iEmitGen3D)
SCF_IMPLEMENT_IBASE_END

/// utility function
static float GetRandomFloat(float min, float max)
{
  float w = max - min;
  return min + (w*rand()/(RAND_MAX+1.0));
}

csEmitFixed::csEmitFixed(iBase *parent)
{
  SCF_CONSTRUCT_IBASE(parent);
  val.Set(0,0,0);
}

csEmitFixed::~csEmitFixed()
{
}

void csEmitFixed::GetValue(csVector3& value, csVector3 & /*given*/)
{
  value = val;
}

void csEmitFixed::SetValue(const csVector3& value)
{
  val = value;
}

csEmitBox::csEmitBox(iBase *parent)
{
  SCF_CONSTRUCT_IBASE(parent);
  min.Set(0,0,0);
  max.Set(0,0,0);
}

csEmitBox::~csEmitBox()
{
}

void csEmitBox::GetValue(csVector3& value, csVector3 & /*given*/)
{
  value.x = GetRandomFloat(min.x, max.x);
  value.y = GetRandomFloat(min.y, max.y);
  value.z = GetRandomFloat(min.z, max.z);
}

void csEmitBox::SetContent(const csVector3& min, const csVector3& max)
{
  csEmitBox::min = min;
  csEmitBox::max = max;
}

void csEmitBox::GetContent(csVector3& min, csVector3& max)
{
  min = csEmitBox::min;
  max = csEmitBox::max;
}

csEmitSphere::csEmitSphere(iBase *parent)
{
  SCF_CONSTRUCT_IBASE(parent);
  center.Set(0,0,0);
  min = 0;
  max = 0;
}

csEmitSphere::~csEmitSphere()
{
}

void csEmitSphere::GetValue(csVector3& value, csVector3 & /*given*/)
{
  //because the volume of a slice at a certain distance
  // is the dist*dist(*pi..) in size. Taking random min..max will
  // cause an uneven spread of points in the sphere.
  float sqdist = GetRandomFloat( min*min*min, max*max*max );
  float dist = pow(sqdist, (float)(1./3.));
  value.Set(dist, 0, 0);
  float rotz_open = 2.0 * PI * (rand() / (1.0+RAND_MAX));
  csZRotMatrix3 openrot(rotz_open);
  value = openrot * value;
  float rot_around = 2.0 * PI * (rand() / (1.0+RAND_MAX));
  csXRotMatrix3 xaround(rot_around);
  value = xaround * value;
  value += center;

  // slow but gives a good even spreading
  //while(1)
  //{
    //value.Set (GetRandomFloat(-max,max), GetRandomFloat(-max,max),
      //GetRandomFloat(-max,max));
    //float dist = value.SquaredNorm();
    //if( (min*min <= dist) && (dist <= max*max) )
      //break;
  //}
  //value += center;
}

void csEmitSphere::SetContent(const csVector3& center, float min, float max)
{
  csEmitSphere::center = center;
  csEmitSphere::min = min;
  csEmitSphere::max = max;
}

void csEmitSphere::GetContent(csVector3& center, float& min, float& max)
{
  center = csEmitSphere::center;
  min = csEmitSphere::min;
  max = csEmitSphere::max;
}

csEmitCone::csEmitCone(iBase *parent)
{
  SCF_CONSTRUCT_IBASE(parent);
  origin.Set(0,0,0);
  elevation = 0;
  azimuth = 0;
  aperture = 0;
  min = 0;
  max = 0;
}

csEmitCone::~csEmitCone()
{
}

void csEmitCone::GetValue(csVector3& value, csVector3 & /*given*/)
{
  csVector3 dest(GetRandomFloat(min, max), 0, 0);
  /// from fountain code

  // now make it shoot to a circle in the x direction
  float rotz_open = 2.0 * aperture * (rand() / (1.0+RAND_MAX)) - aperture;
  csZRotMatrix3 openrot(rotz_open);
  dest = openrot * dest;
  float rot_around = 2.0 * PI * (rand() / (1.0+RAND_MAX));
  csXRotMatrix3 xaround(rot_around);
  dest = xaround * dest;
  // now dest point to somewhere in a circular cur of a sphere around the
  // x axis.

  // direct the fountain to the users dirction
  csZRotMatrix3 elev(elevation);
  dest = elev * dest;
  csYRotMatrix3 compassdir(azimuth);
  dest = compassdir * dest;

  // now dest points to the exit speed of the spout if that spout was
  // at 0,0,0.

  // for cones atr a different origin - translate the point
  dest += origin;
  value = dest;
}

void csEmitCone::SetContent(const csVector3& origin, float elevation,
      float azimuth, float aperture, float min, float max)
{
  csEmitCone::origin = origin;
  csEmitCone::elevation = elevation;
  csEmitCone::azimuth = azimuth;
  csEmitCone::aperture = aperture;
  csEmitCone::min = min;
  csEmitCone::max = max;
}

void csEmitCone::GetContent(csVector3& origin, float& elevation,
      float& azimuth, float& aperture, float& min, float& max)
{
  origin = csEmitCone::origin;
  elevation = csEmitCone::elevation;
  azimuth = csEmitCone::azimuth;
  aperture = csEmitCone::aperture;
  min = csEmitCone::min;
  max = csEmitCone::max;
}

csEmitMix::csEmitMix(iBase *parent)
{
  SCF_CONSTRUCT_IBASE(parent);
  list = NULL;
  totalweight = 0.0;
  nr = 0;
}

csEmitMix::~csEmitMix()
{
  struct part *p = list, *np =0;
  while(p)
  {
    np = p->next;
    if(p->emit) p->emit->DecRef();
    delete p;
    p = np;
  }
}

void csEmitMix::GetValue(csVector3& value, csVector3 & given)
{
  float num = GetRandomFloat(0., totalweight);
  float passed = 0.0;
  struct part *p = list;
  struct part *found = list;
  while(p)
  {
    passed += p->weight;
    if(num < passed)
    {
      found = p;
      break;
    }
    p = p->next;
  }
  if(!found) value.Set(0,0,0);
  else found->emit->GetValue(value, given);
}

void csEmitMix::AddEmitter(float weight, iEmitGen3D* emit)
{
  struct part *np = new struct csEmitMix::part;
  np->next = list;
  np->emit = emit;
  list = np;
  if(emit) emit->IncRef();
  np->weight = weight;
  nr++;
  totalweight += weight;
}

void csEmitMix::GetContent(int num, float& weight, iEmitGen3D*& emit)
{
  struct part *p = list;
  if(num >= nr) return;
  int i = 0;
  while(i < num)
  {
    p = p->next;
    i++;
  }
  weight = p->weight;
  emit = p->emit;
}

csEmitLine::csEmitLine(iBase *parent)
{
  SCF_CONSTRUCT_IBASE(parent);
  start.Set(0,0,0);
  end.Set(0,0,0);
}

csEmitLine::~csEmitLine()
{
}

void csEmitLine::GetValue(csVector3& value, csVector3 & /*given*/)
{
  float v = GetRandomFloat(0., 1.);
  value = start + (end-start)*v;
}

void csEmitLine::SetContent(const csVector3& start, const csVector3& end)
{
  csEmitLine::start = start;
  csEmitLine::end = end;
}

void csEmitLine::GetContent(csVector3& start, csVector3& end)
{
  start = csEmitLine::start;
  end = csEmitLine::end;
}

csEmitCylinder::csEmitCylinder(iBase *parent)
{
  SCF_CONSTRUCT_IBASE(parent);
  start.Set(0,0,0);
  end.Set(0,0,0);
  min = 0;
  max = 0;
}

/// helper func to find a vector (any vector) 90 degrees on the given one
static void FindAxis(const csVector3& n, csVector3& res)
{
  //// n*res must be 0.
  if(n.x==0.0) {res.Set(1,0,0); return;}
  if(n.y==0.0) {res.Set(0,1,0); return;}
  if(n.z==0.0) {res.Set(0,0,1); return;}
  // so, x, y and z of n are not 0.
  res.x = 0;
  res.y = -n.z;
  res.z = n.y;
  // so that nx*resx = 0
  // and  ny*resy + nz*resz will be
  //     -ny*nz + nz*ny which is 0
}

csEmitCylinder::~csEmitCylinder()
{
}

void csEmitCylinder::GetValue(csVector3& value, csVector3 & /*given*/)
{
  // point on the center line of the cylinder
  float v = GetRandomFloat(0., 1.);
  value = start + (end-start)*v;

  // setup 3 axis for the cylinder
  csVector3 normal = (end-start).Unit();
  csVector3 udir; FindAxis(normal, udir);
  csVector3 vdir = udir % normal;
  float angle = GetRandomFloat(0, 2.*PI);
  // direction on the circle
  csVector3 oncirc = udir*cos(angle) + vdir*sin(angle);

  // distance from cylinder line
  float amount = GetRandomFloat(min*min, max*max);
  amount = qsqrt(amount); // cause even spread of points in the circle
  value += oncirc * amount;
}

void csEmitCylinder::SetContent(const csVector3& start, const csVector3& end,
  float min, float max)
{
  csEmitCylinder::start = start;
  csEmitCylinder::end = end;
  csEmitCylinder::min = min;
  csEmitCylinder::max = max;
}

void csEmitCylinder::GetContent(csVector3& start, csVector3& end,
  float& min, float& max)
{
  start = csEmitCylinder::start;
  end = csEmitCylinder::end;
  min = csEmitCylinder::min;
  max = csEmitCylinder::max;
}

csEmitCylinderTangent::csEmitCylinderTangent(iBase *parent)
{
  SCF_CONSTRUCT_IBASE(parent);
  start.Set(0,0,0);
  end.Set(0,0,0);
  min = 0;
  max = 0;
}

csEmitCylinderTangent::~csEmitCylinderTangent()
{
}

void csEmitCylinderTangent::GetValue(csVector3& value, csVector3 &given)
{
  // cylinder direction
  csVector3 cyldir = (end - start).Unit();

  // compute normal at the point
  csPlane3 pl(cyldir, 0);
  float pldist = csSquaredDist::PointPlane(given, pl);
  pl.Set(cyldir, pldist); // cylinder plane through given point.
  csVector3 isect;
  float sectdist;
  if(!csIntersect3::Plane(start, end, pl, isect, sectdist))
    isect.Set(end);
  csVector3 normal = (given - isect).Unit();

  // need a direction tangential to normal and cyldirection
  csVector3 direction = normal % cyldir;
  float amount = GetRandomFloat(min, max);
  value = direction * amount;
}

void csEmitCylinderTangent::SetContent(const csVector3& start,
 const csVector3& end, float min, float max)
{
  csEmitCylinderTangent::start = start;
  csEmitCylinderTangent::end = end;
  csEmitCylinderTangent::min = min;
  csEmitCylinderTangent::max = max;
}

void csEmitCylinderTangent::GetContent(csVector3& start, csVector3& end,
   float& min, float& max)
{
  start = csEmitCylinderTangent::start;
  end = csEmitCylinderTangent::end;
  min = csEmitCylinderTangent::min;
  max = csEmitCylinderTangent::max;
}

csEmitSphereTangent::csEmitSphereTangent(iBase *parent)
{
  SCF_CONSTRUCT_IBASE(parent);
  center.Set(0,0,0);
  min = 0;
  max = 0;
}

csEmitSphereTangent::~csEmitSphereTangent()
{
}

void csEmitSphereTangent::GetValue(csVector3& value, csVector3 & given)
{
  csVector3 path = given - center;
  // setup axis
  csVector3 normal = path.Unit();
  csVector3 udir; FindAxis(normal, udir);
  csVector3 vdir = udir % normal;
  float angle = GetRandomFloat(0, 2.*PI);
  // direction on the circle
  csVector3 oncirc = udir*cos(angle) + vdir*sin(angle);

  // size of direction
  float amount = GetRandomFloat(min*min, max*max);
  amount = qsqrt(amount); // cause even spread of points in the circle
  value = oncirc * amount;
}

void csEmitSphereTangent::SetContent(const csVector3& center,
 float min, float max)
{
  csEmitSphereTangent::center = center;
  csEmitSphereTangent::min = min;
  csEmitSphereTangent::max = max;
}

void csEmitSphereTangent::GetContent(csVector3& center, float& min, float& max)
{
  center = csEmitSphereTangent::center;
  min = csEmitSphereTangent::min;
  max = csEmitSphereTangent::max;
}

//----------------- csEmitMeshObject -----------------------------------

SCF_IMPLEMENT_IBASE_EXT (csEmitMeshObject)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iEmitState)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csEmitMeshObject::EmitState)
  SCF_IMPLEMENTS_INTERFACE (iEmitState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END


void csEmitMeshObject::SetupObject ()
{
  if (!initialized)
  {
    RemoveParticles ();
    initialized = true;
    delete[] ages;
    delete[] part_pos;
    delete[] part_speed;
    delete[] part_accel;
    delete[] part_attract;

    ages = new int[number];
    part_pos = new csVector3[number];
    part_speed = new csVector3[number];
    part_accel = new csVector3[number];
    part_attract = new csVector3[number];
    bbox.StartBoundingBox();

    /// create new particles and add to particle system
    for (int i=0 ; i < number ; i++)
    {
      if(using_rect_sprites)
        AppendRectSprite (drop_width, drop_height, mat, lighted_particles);
      else AppendRegularSprite (drop_sides, drop_radius, mat,
        lighted_particles);
      StartParticle(i);
      /// age each particle randomly, to spread out particles over ages.
      int elapsed = QInt(GetRandomFloat(0,timetolive));
      MoveAgeParticle(i, elapsed, elapsed/1000.);
    }
    SetupColor ();
    SetupMixMode ();
  }
}

csEmitMeshObject::csEmitMeshObject (iSystem* system,
  iMeshObjectFactory* factory)
	: csParticleSystem (system, factory)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiEmitState);
  lighted_particles = false;
  number = 50;
  ages = NULL;
  part_pos = NULL;
  part_speed = NULL;
  part_accel = NULL;
  part_attract = NULL;
  attractor_force = 1.0;
  attractor = NULL;
  startpos = NULL;
  startspeed = NULL;
  startaccel = NULL;
  timetolive = 1000;
  aging = NULL;
  nr_aging_els = 0;
  using_rect_sprites = true;
  drop_width = 0.2;
  drop_height = 0.2;
  drop_sides = 6;
  drop_radius = 0.1;
}

csEmitMeshObject::~csEmitMeshObject()
{
  delete[] ages;
  delete[] part_pos;
  delete[] part_speed;
  delete[] part_accel;
  delete[] part_attract;
  if(startpos) startpos->DecRef();
  if(startspeed) startspeed->DecRef();
  if(startaccel) startaccel->DecRef();
  if(attractor) attractor->DecRef();
  csEmitAge *p = aging, *np = 0;
  while(p)
  {
    np = p->next;
    delete p;
    p = p->next;
  }
}

void csEmitMeshObject::StartParticle (int i)
{
  csVector3 pos;
  csVector3 startgiven(0,0,0);
  startpos->GetValue(pos, startgiven);
  startspeed->GetValue(part_speed[i], pos);
  startaccel->GetValue(part_accel[i], pos);
  if(attractor) attractor->GetValue(part_attract[i], pos);
  GetParticle (i)->SetMixMode (MixMode);
  GetParticle (i)->SetPosition (pos);
  part_pos[i] = pos;
  bbox.AddBoundingVertex(part_pos[i]);
  ages[i] = 0;
  /// use first element to start particles with
  if(!aging) return;
  GetParticle(i)->ScaleBy(aging->scale);
  if(MixMode & CS_FX_ADD)
  {
    csColor col = aging->color;
    col *= (1.-aging->alpha);
    GetParticle(i)->SetColor(col);
  }
  else
  {
    GetParticle(i)->SetColor(aging->color);
    if(aging->alpha!=0.0)
      GetParticle(i)->SetMixMode(MixMode | CS_FX_SETALPHA(aging->alpha));
  }
}

void csEmitMeshObject::MoveAgeParticle (int i, int elapsed, float delta_t)
{
  /// age
  // find old scalefactor
  csEmitAge *belowage = 0, *aboveage = aging;
  float oldscale = 1.;
  float between;
  while(aboveage && (aboveage->time < ages[i]))
  {
    belowage = aboveage;
    aboveage = aboveage->next;
  }
  if(!belowage && aboveage)
    oldscale = aboveage->scale;
  else if(belowage && !aboveage)
    oldscale = belowage->scale;
  else if(belowage && aboveage)
  {
    between = float(aboveage->time - ages[i]) /
      float(aboveage->time - belowage->time);
    oldscale = between*belowage->scale + (1.-between)*aboveage->scale;
  }
  ages[i] += elapsed;
  /// find new settings
  float swirlamount = 0.0;
  float rotspeed = 0.0;
  float newscale = 1.0;
  float alpha = 0.0;
  csColor col(1, 1, 1);
  aboveage = aging; belowage = 0;
  while(aboveage && (aboveage->time < ages[i]))
  {
    belowage = aboveage;
    aboveage = aboveage->next;
  }
  if(!belowage && aboveage)
  {
    swirlamount = aboveage->swirl;
    rotspeed = aboveage->rotspeed;
    alpha = aboveage->alpha;
    newscale = aboveage->scale;
    col = aboveage->color;
  }
  else if(belowage && !aboveage)
  {
    swirlamount = belowage->swirl;
    rotspeed = belowage->rotspeed;
    alpha = belowage->alpha;
    newscale = belowage->scale;
    col = belowage->color;
  }
  else if(belowage && aboveage)
  {
    between = float(aboveage->time - ages[i]) /
      float(aboveage->time - belowage->time);
    float invbet = 1.f-between;
    swirlamount = between*belowage->swirl + invbet*aboveage->swirl;
    rotspeed = between*belowage->rotspeed + invbet*aboveage->rotspeed;
    alpha = between*belowage->alpha + invbet*aboveage->alpha;
    newscale = between*belowage->scale + invbet*aboveage->scale;
    col = between*belowage->color + invbet*aboveage->color;
  }
  // adjust the particle
  GetParticle(i)->ScaleBy(newscale / oldscale);
  GetParticle(i)->Rotate(rotspeed * delta_t);
  if(MixMode & CS_FX_ADD)
  {
    col *= (1.-alpha);
    GetParticle(i)->SetColor(col);
  }
  else
  {
    GetParticle(i)->SetColor(col);
    if(alpha!=0.0)
      GetParticle(i)->SetMixMode(MixMode | CS_FX_SETALPHA(alpha));
    else GetParticle(i)->SetMixMode(MixMode);
  }

  /// move the particle
  if(attractor)
  {
    // do attractor influence
    csVector3 d = part_attract[i] - part_pos[i];
    part_accel[i] += d * (attractor_force * delta_t);
  }
  csVector3 swirl = GetRandomDirection() * swirlamount;
  part_speed[i] += swirl * delta_t;
  part_speed[i] += part_accel[i]*delta_t;
  csVector3 move = part_speed[i]*delta_t;
  GetParticle(i)->MovePosition(move);
  part_pos[i] += move;
  bbox.AddBoundingVertexSmart(part_pos[i]);
}

void csEmitMeshObject::Update (csTime elapsed_time)
{
  SetupObject ();
  /// do particle system stuff.
  csParticleSystem::Update (elapsed_time);
  float delta_t = elapsed_time / 1000.0f; // in seconds
  int elapsed = elapsed_time;
  // age particles;
  int i;
  for (i=0 ; i < particles.Length () ; i++)
  {
    if(ages[i] + elapsed > timetolive)
    {
      // find old scalefactor
      csEmitAge *belowage = 0, *aboveage = aging;
      float oldscale = 1.;
      while(aboveage && (aboveage->time < ages[i]))
      {
        belowage = aboveage;
        aboveage = aboveage->next;
      }
      if(!belowage && aboveage)
        oldscale = aboveage->scale;
      else if(belowage && !aboveage)
        oldscale = belowage->scale;
      else if(belowage && aboveage)
      {
        float between = float(aboveage->time - ages[i]) /
          float(aboveage->time - belowage->time);
        oldscale = between*belowage->scale + (1.-between)*aboveage->scale;
      }
      GetParticle(i)->ScaleBy(1./oldscale); // reset the scale
      /// restart the particle
      int afterstart = (ages[i] + elapsed_time) % timetolive;
      StartParticle(i);
      // move a little after start
      MoveAgeParticle(i, afterstart, afterstart/1000.);
    }
    else
    {
      /// age the particle
      MoveAgeParticle(i, elapsed, delta_t);
    }
  }
}

void csEmitMeshObject::HardTransform (const csReversibleTransform& /*t*/)
{
}

void csEmitMeshObject::AddAge(int time, const csColor& color, float alpha,
        float swirl, float rotspeed, float scale)
{
  // insert sorted into linked list;
  csEmitAge *p = aging, *prevp = 0;
  while(p && (p->time <= time))
  {
    prevp = p;
    p=p->next;
  }
  csEmitAge *np = new csEmitAge;
  np->next = p;
  if(!prevp) aging = np;
  else prevp->next = np;
  nr_aging_els ++;
  np->time = time;
  np->color = color;
  np->alpha = alpha;
  np->swirl = swirl;
  np->rotspeed = rotspeed;
  np->scale = scale;
}


void csEmitMeshObject::GetAgingMoment(int i, int& time, csColor& color,
  float &alpha, float& swirl, float& rotspeed, float& scale)
{
  int n = 0;
  if(i >= nr_aging_els) return;
  csEmitAge *p = aging;
  while(n < i)
  {
    n++;
    p=p->next;
  }
  time = p->time;
  color = p->color;
  alpha = p->alpha;
  swirl = p->swirl;
  rotspeed = p->rotspeed;
  scale = p->scale;
}


//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csEmitMeshObjectFactory)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectFactory)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iEmitFactoryState)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csEmitMeshObjectFactory::EmitFactoryState)
  SCF_IMPLEMENTS_INTERFACE (iEmitFactoryState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csEmitMeshObjectFactory::csEmitMeshObjectFactory (iBase *p, iSystem* s)
{
  SCF_CONSTRUCT_IBASE (p);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiEmitFactoryState);
  system = s;
}

csEmitMeshObjectFactory::~csEmitMeshObjectFactory ()
{
}

iMeshObject* csEmitMeshObjectFactory::NewInstance ()
{
  csEmitMeshObject* cm =
    new csEmitMeshObject (system, (iMeshObjectFactory*)this);
  iMeshObject* im = SCF_QUERY_INTERFACE (cm, iMeshObject);
  im->DecRef ();
  return im;
}

//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csEmitMeshObjectType)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectType)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iPlugin)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csEmitMeshObjectType::eiPlugin)
  SCF_IMPLEMENTS_INTERFACE (iPlugin)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csEmitMeshObjectType)

SCF_EXPORT_CLASS_TABLE (emit)
  SCF_EXPORT_CLASS (csEmitMeshObjectType, "crystalspace.mesh.object.emit",
    "Crystal Space Emit Mesh Type")
SCF_EXPORT_CLASS_TABLE_END

csEmitMeshObjectType::csEmitMeshObjectType (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiPlugin);
}

csEmitMeshObjectType::~csEmitMeshObjectType ()
{
}

iMeshObjectFactory* csEmitMeshObjectType::NewFactory ()
{
  csEmitMeshObjectFactory* cm = new csEmitMeshObjectFactory (this, system);
  iMeshObjectFactory* ifact = SCF_QUERY_INTERFACE (cm, iMeshObjectFactory);
  ifact->DecRef ();
  return ifact;
}
