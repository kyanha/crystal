/*
    Copyright (C) 2000 by W.C.A. Wijngaards

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

#ifndef __CS_EXPLOSION_H__
#define __CS_EXPLOSION_H__

#include "csgeom/vector3.h"
#include "csgeom/box.h"
#include "csutil/cscolor.h"
#include "csutil/refarr.h"
#include "csutil/scf_implementation.h"
#include "csplugincommon/particlesys/partgen.h"
#include "imesh/explode.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"

struct iMaterialWrapper;
struct iSector;
struct iEngine;

/**
 * An explosive particle system.
 * particles explode outward from defined center.
 */
class csExploMeshObject :
  public scfImplementationExt1<csExploMeshObject, csNewtonianParticleSystem,
    iExplosionState>
{
protected:
  /// Center of explosion.
  csVector3 center;
  /// scaling of particles.
  bool scale_particles;
  csTicks fade_particles;
  /// starting bbox.
  csBox3 startbox;
  float maxspeed, maxaccel, radiusnow;
  int amt;

  csVector3 push;
  int nr_sides;
  float part_radius;
  bool lighted_particles;
  float spread_pos;
  float spread_accel;
  float spread_speed;

  void SetupObject ();

public:
  /**
   * Give number of particles and center.
   * push is speed added to all particles (i.e. speed of object being
   * destroyed for example, or a wind pushing all particles in a direction),
   * give a material to use as well,
   * nr_sides is the number of sides of every particle polygon.
   * part_radius is the radius of every particle,
   * spreading multipliers: a random number (1.0..+1.0) * spread is added.
   */
  csExploMeshObject (iObjectRegistry* object_reg, iMeshObjectFactory* factory);
  /// Destructor.
  virtual ~csExploMeshObject ();

  //------------------------- iExplosionState implementation ----------------

  /// Set the number of particles to use.
  virtual void SetParticleCount (int num)
  {
    csParticleSystem::SetParticleCount (num);
    SetCount ((int)number);
  }
  /// Get the number of particles
  int GetParticleCount () const {return (int)number;}
  /// Set the explosion center.
  void SetCenter (const csVector3& center)
  {
    initialized = false;
    csExploMeshObject::center = center;
    ShapeChanged ();
  }
  /// Get the explosion center
  const csVector3 &GetCenter () const {return center;}
  /// Set the push vector.
  void SetPush (const csVector3& push)
  {
    initialized = false;
    csExploMeshObject::push = push;
    ShapeChanged ();
  }
  /// Get the push vector.
  const csVector3& GetPush () const {return push;}
  /// Set the number of sides.
  void SetNrSides (int nr_sides)
  {
    initialized = false;
    csExploMeshObject::nr_sides = nr_sides;
    ShapeChanged ();
  }
  /// Get the number of sides.
  int GetNrSides () const {return nr_sides;}
  /// Set the radius of all particles.
  void SetPartRadius (float part_radius)
  {
    initialized = false;
    csExploMeshObject::part_radius = part_radius;
    ShapeChanged ();
  }
  /// Get the radius of all particles.
  float GetPartRadius () const {return part_radius;}
  /// Enable or disable lighting.
  void SetLighting (bool l)
  {
    initialized = false;
    lighted_particles = l;
  }
  /// See if lighting is enabled or disabled.
  bool GetLighting () const {return lighted_particles;}
  /// Set the spread position.
  void SetSpreadPos (float spread_pos)
  {
    initialized = false;
    csExploMeshObject::spread_pos = spread_pos;
    ShapeChanged ();
  }
  /// Get the spread position.
  float GetSpreadPos () const {return spread_pos;}
  /// Set the spread speed.
  void SetSpreadSpeed (float spread_speed)
  {
    initialized = false;
    csExploMeshObject::spread_speed = spread_speed;
    ShapeChanged ();
  }
  /// Get the spread speed.
  float GetSpreadSpeed () const {return spread_speed;}
  /// Set the spread acceleration.
  void SetSpreadAcceleration (float spread_accel)
  {
    initialized = false;
    csExploMeshObject::spread_accel = spread_accel;
    ShapeChanged ();
  }
  /// Get the spread acceleration.
  float GetSpreadAcceleration () const {return spread_accel;}
  /**
   * Set particles to be scaled to nothing starting at fade_particles msec
   * before self-destruct.
   */
  void SetFadeSprites (csTicks fade_time)
  { scale_particles = true; fade_particles = fade_time; }
  /// See if particles are faded (returns true), and returns fade time too.
  bool GetFadeSprites (csTicks& fade_time) const
  {
    if(!scale_particles) return false;
    fade_time = fade_particles; return true;
  }

  /// Update the particle system.
  virtual void Update (csTicks elapsed_time);

  /// For iMeshObject.
  virtual void HardTransform (const csReversibleTransform& t);
  virtual bool SupportsHardTransform () const { return true; }
};

/**
 * Factory for explosions.
 */
class csExploMeshObjectFactory :
  public scfImplementation1<csExploMeshObjectFactory, iMeshObjectFactory>
{
private:
  iObjectRegistry* object_reg;
  iMeshFactoryWrapper* logparent;
  iMeshObjectType* explo_type;
  csFlags flags;

public:
  /// Constructor.
  csExploMeshObjectFactory (iMeshObjectType *pParent,
  	iObjectRegistry* object_reg);

  /// Destructor.
  virtual ~csExploMeshObjectFactory ();

  //------------------------ iMeshObjectFactory implementation --------------
  virtual csFlags& GetFlags () { return flags; }
  virtual csPtr<iMeshObject> NewInstance ();
  virtual csPtr<iMeshObjectFactory> Clone () { return 0; }
  virtual void HardTransform (const csReversibleTransform&) { }
  virtual bool SupportsHardTransform () const { return false; }
  virtual void SetMeshFactoryWrapper (iMeshFactoryWrapper* lp)
  { logparent = lp; }
  virtual iMeshFactoryWrapper* GetMeshFactoryWrapper () const
  { return logparent; }
  virtual iMeshObjectType* GetMeshObjectType () const { return explo_type; }
  virtual iObjectModel* GetObjectModel () { return 0; }
  virtual bool SetMaterialWrapper (iMaterialWrapper*) { return false; }
  virtual iMaterialWrapper* GetMaterialWrapper () const { return 0; }
  virtual void SetMixMode (uint) { }
  virtual uint GetMixMode () const { return 0; }
};

/**
 * Explosion type. This is the plugin you have to use to create instances
 * of csExploMeshObjectFactory.
 */
class csExploMeshObjectType :
  public scfImplementation2<csExploMeshObjectType,
    iMeshObjectType, iComponent>
{
private:
  iObjectRegistry* object_reg;

public:
  /// Constructor.
  csExploMeshObjectType (iBase*);
  /// Destructor.
  virtual ~csExploMeshObjectType ();
  /// Draw.
  virtual csPtr<iMeshObjectFactory> NewFactory ();

  //------------------------ iComponent implementation --------------
  virtual bool Initialize (iObjectRegistry* p)
  { this->object_reg = p; return true; }
};

#endif // __CS_EXPLOSION_H__
