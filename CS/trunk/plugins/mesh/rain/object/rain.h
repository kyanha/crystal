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

#ifndef __CS_RAIN_H__
#define __CS_RAIN_H__

#include "csgeom/vector3.h"
#include "csgeom/box.h"
#include "csutil/cscolor.h"
#include "csutil/refarr.h"
#include "plugins/mesh/partgen/partgen.h"
#include "imesh/rain.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"

struct iMaterialWrapper;

/**
 * A rain particle system. Particles start falling down.
 * Since speed if fixed due to air friction, this is not a NewtonianPartSys.
 */
class csRainMeshObject : public csParticleSystem
{
protected:
  csBox3 rainbox;       /// bounding box for rain
  csVector3 rain_dir;   /// vector to simulate wind and handle downward motion
  csVector3 *part_pos;  /// list of xyz positions of each drop
  float     *drop_stop; /// list of y positions to stop each drop if CD is used
  float drop_width, drop_height;  /// sizes of 2dsprite to use
  int number;           /// count of particles
  bool useCD;           /// flag whether to use exact stopping locations or not
  iSector *sector;      /// sector of rain mesh, used for CD
  bool lighted_particles; /// flag whether to use lighting on particles or not

  void  SetupObject ();
  float FindStopHeight (int i);  // Use HitBeam to determine where raindrop will land

public:
  /**
   * creates a rain particle system.
   * number : number of raindrops visible at one time
   * mat: material of raindrops. mixmode = mixmode used.
   * lighted: the particles will be lighted if true.
   * drop_width, drop_height: size of rectangular raindrops.
   * rainbox_min and max: give the box in the world where it will rain.
   *   raindrops will start ...
   *   and when they exit this box they will disappear.
   * fall_speed: the direction and speed of the falling raindrops.
   *   You can make slanted rain this way. Although you would also want to
   *   slant the particles in that case...
   */
  csRainMeshObject (iObjectRegistry* object_reg, iMeshObjectFactory* factory);
  /// Destructor.
  virtual ~csRainMeshObject ();

  /// Set the number of particles to use.
  void SetParticleCount (int num)
  {
    initialized = false;
    number = num;
    scfiObjectModel.ShapeChanged ();
  }
  /// Get the number of particles
  int GetParticleCount () const { return number; }
  /// Set the size of the drops.
  void SetDropSize (float dropwidth, float dropheight)
  {
    initialized = false;
    drop_width = dropwidth;
    drop_height = dropheight;
    scfiObjectModel.ShapeChanged ();
  }
  /// Get the size of the drops.
  void GetDropSize (float& dropwidth, float& dropheight) const
  { dropwidth = drop_width; dropheight = drop_height; }
  /// Set box.
  void SetBox (const csVector3& minbox, const csVector3& maxbox)
  {
    initialized = false;
    rainbox.Set (minbox, maxbox);
    scfiObjectModel.ShapeChanged ();
  }
  /// Get box.
  void GetBox (csVector3& minbox, csVector3& maxbox) const
  { minbox = rainbox.Min(); maxbox = rainbox.Max(); }
  /// Enable or disable lighting.
  void SetLighting (bool l)
  {
    initialized = false;
    lighted_particles = l;
  }
  /// see if lighting is enabled
  bool GetLighting () const
  { return lighted_particles; }
  /// Set fall speed.
  void SetFallSpeed (const csVector3& fspeed)
  {
    initialized = false;
    rain_dir = fspeed;
    scfiObjectModel.ShapeChanged ();
  }
  /// Get fall speed.
  const csVector3& GetFallSpeed () const
  { return rain_dir; }

  /// Enable/Disable Collision Detection for drops
  void SetCollisionDetection(bool cd);
  /// Get CD flag
  bool GetCollisionDetection() const
  { return useCD; };

  /// Update the particle system.
  virtual void Update (csTicks elapsed_time);

  /// For iMeshObject.
  virtual void HardTransform (const csReversibleTransform& t);
  virtual bool SupportsHardTransform () const { return false; }

  SCF_DECLARE_IBASE_EXT (csParticleSystem);

  //------------------------- iRainState implementation ----------------
  class RainState : public iRainState
  {
    SCF_DECLARE_EMBEDDED_IBASE (csRainMeshObject);
    virtual void SetParticleCount (int num)
    {
      scfParent->SetParticleCount (num);
    }
    virtual void SetDropSize (float dropwidth, float dropheight)
    {
      scfParent->SetDropSize (dropwidth, dropheight);
    }
    virtual void SetBox (const csVector3& minbox, const csVector3& maxbox)
    {
      scfParent->SetBox (minbox, maxbox);
    }
    virtual void SetLighting (bool l)
    {
      scfParent->SetLighting (l);
    }
    virtual void SetFallSpeed (const csVector3& fspeed)
    {
      scfParent->SetFallSpeed (fspeed);
    }
    virtual int GetParticleCount () const
    { return scfParent->GetParticleCount(); }
    virtual void GetDropSize (float& dropwidth, float& dropheight) const
    { scfParent->GetDropSize(dropwidth, dropheight); }
    virtual void GetBox (csVector3& minbox, csVector3& maxbox) const
    { scfParent->GetBox(minbox, maxbox); }
    virtual bool GetLighting () const
    { return scfParent->GetLighting(); }
    virtual const csVector3& GetFallSpeed () const
    { return scfParent->GetFallSpeed(); }
    virtual void SetCollisionDetection(bool cd)
    { scfParent->SetCollisionDetection(cd); };
    virtual bool GetCollisionDetection() const
    { return scfParent->GetCollisionDetection(); };
  } scfiRainState;
  friend class RainState;
};

/**
 * Factory for rain.
 */
class csRainMeshObjectFactory : public iMeshObjectFactory
{
private:
  iObjectRegistry* object_reg;
  iBase* logparent;

public:
  /// Constructor.
  csRainMeshObjectFactory (iBase *pParent, iObjectRegistry* object_reg);

  /// Destructor.
  virtual ~csRainMeshObjectFactory ();

  //------------------------ iMeshObjectFactory implementation --------------
  SCF_DECLARE_IBASE;

  virtual csPtr<iMeshObject> NewInstance ();
  virtual void HardTransform (const csReversibleTransform&) { }
  virtual bool SupportsHardTransform () const { return false; }
  virtual void SetLogicalParent (iBase* lp) { logparent = lp; }
  virtual iBase* GetLogicalParent () const { return logparent; }
  virtual iObjectModel* GetObjectModel () { return NULL; }
};

/**
 * Rain type. This is the plugin you have to use to create instances
 * of csRainMeshObjectFactory.
 */
class csRainMeshObjectType : public iMeshObjectType
{
private:
  iObjectRegistry* object_reg;

public:
  SCF_DECLARE_IBASE;

  /// Constructor.
  csRainMeshObjectType (iBase*);
  /// Destructor.
  virtual ~csRainMeshObjectType ();

  /// Draw.
  virtual csPtr<iMeshObjectFactory> NewFactory ();

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csRainMeshObjectType);
    virtual bool Initialize (iObjectRegistry* p)
    { scfParent->object_reg = p; return true; }
  } scfiComponent;
  friend struct eiComponent;
};

#endif // __CS_RAIN_H__
