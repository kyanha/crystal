/*
    Copyright (C) 2001 by W.C.A. Wijngaards

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

#ifndef __CS_EMIT_H__
#define __CS_EMIT_H__

#include "csgeom/vector3.h"
#include "csgeom/box.h"
#include "csutil/cscolor.h"
#include "csutil/refarr.h"
#include "plugins/mesh/partgen/partgen.h"
#include "imesh/emit.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"

struct iMaterialWrapper;

/** fixed value emitter */
class csEmitFixed : public iEmitFixed
{
  csVector3 val;
public:
  SCF_DECLARE_IBASE;
  csEmitFixed(iBase *parent);
  virtual ~csEmitFixed();
  virtual void GetValue(csVector3& value, csVector3 &given);
  virtual void SetValue(const csVector3& value);
};

/** box value emitter */
class csEmitBox : public iEmitBox
{
  csVector3 min, max;
public:
  SCF_DECLARE_IBASE;
  csEmitBox(iBase *parent);
  virtual ~csEmitBox();
  virtual void GetValue(csVector3& value, csVector3 &given);
  virtual void SetContent(const csVector3& min, const csVector3& max);
  virtual void GetContent(csVector3& min, csVector3& max);
};

/** Sphere value emitter */
class csEmitSphere : public iEmitSphere
{
  csVector3 center;
  float min, max;
public:
  SCF_DECLARE_IBASE;
  csEmitSphere(iBase *parent);
  virtual ~csEmitSphere();
  virtual void GetValue(csVector3& value, csVector3 &given);
  virtual void SetContent(const csVector3& center, float min, float max);
  virtual void GetContent(csVector3& center, float& min, float& max);
};

/** Cone value emitter */
class csEmitCone : public iEmitCone
{
  csVector3 origin;
  float elevation, azimuth, aperture, min, max;
public:
  SCF_DECLARE_IBASE;
  csEmitCone(iBase *parent);
  virtual ~csEmitCone();
  virtual void GetValue(csVector3& value, csVector3 &given);
  virtual void SetContent(const csVector3& origin, float elevation,
      float azimuth, float aperture, float min, float max);
  virtual void GetContent(csVector3& origin, float& elevation,
      float& azimuth, float& aperture, float& min, float& max);
};

/** Mix value emitter */
class csEmitMix : public iEmitMix
{
  struct part {iEmitGen3D* emit; float weight; struct part* next;} *list;
  float totalweight;
  int nr;
public:
  SCF_DECLARE_IBASE;
  csEmitMix(iBase *parent);
  virtual ~csEmitMix();
  virtual void GetValue(csVector3& value, csVector3 &given);
  virtual void AddEmitter(float weight, iEmitGen3D* emit);
  virtual float GetTotalWeight() {return totalweight;}
  virtual int GetEmitterCount() {return nr;}
  virtual void GetContent(int num, float& weight, iEmitGen3D*& emit);
};

/** Line value emitter */
class csEmitLine : public iEmitLine
{
  csVector3 start, end;
public:
  SCF_DECLARE_IBASE;
  csEmitLine(iBase *parent);
  virtual ~csEmitLine();
  virtual void GetValue(csVector3& value, csVector3 &given);
  virtual void SetContent(const csVector3& start, const csVector3& end);
  virtual void GetContent(csVector3& start, csVector3& end);
};

/** Cylinder value emitter */
class csEmitCylinder : public iEmitCylinder
{
  csVector3 start, end;
  float min, max;
public:
  SCF_DECLARE_IBASE;
  csEmitCylinder(iBase *parent);
  virtual ~csEmitCylinder();
  virtual void GetValue(csVector3& value, csVector3 &given);
  virtual void SetContent(const csVector3& start, const csVector3& end,
      float min, float max);
  virtual void GetContent(csVector3& start, csVector3& end,
    float& min, float& max);
};

/** SphereTangent value emitter */
class csEmitSphereTangent : public iEmitSphereTangent
{
  csVector3 center;
  float min, max;
public:
  SCF_DECLARE_IBASE;
  csEmitSphereTangent(iBase *parent);
  virtual ~csEmitSphereTangent();
  virtual void GetValue(csVector3& value, csVector3 &given);
  virtual void SetContent(const csVector3& center, float min, float max);
  virtual void GetContent(csVector3& center, float& min, float& max);
};

/** CylinderTangent value emitter */
class csEmitCylinderTangent : public iEmitCylinderTangent
{
  csVector3 start, end;
  float min, max;
public:
  SCF_DECLARE_IBASE;
  csEmitCylinderTangent(iBase *parent);
  virtual ~csEmitCylinderTangent();
  virtual void GetValue(csVector3& value, csVector3 &given);
  virtual void SetContent(const csVector3& start, const csVector3& end,
      float min, float max);
  virtual void GetContent(csVector3& start, csVector3& end,
      float& min, float& max);
};


/** emit ages structure */
class csEmitAge {
public:
  /// moment of age (in msec)
  int time;
  /// color at the age
  csColor color;
  /// alpha value at the age (0=opaque, 1.=invisible)
  float alpha;
  /// swirl at the age
  float swirl;
  /// rotationspeed at the age
  float rotspeed;
  /// scale at the age
  float scale;
  /// next age
  csEmitAge *next;
};

/**
 * A generic emitter particle system.
 * The particles behave in a newtonian manner.
 * You can construct iEmitGen3D structures that will generate xyz values.
 * These values are used to set particle start position, start velocity,
 * and start acceleration.
 * Depending on the age of each particle, it can be coloured, scaled, faded...
 */
class csEmitMeshObject : public csParticleSystem
{
protected:
  /// number of particles
  int number;
  ///
  bool lighted_particles;
  /// the start position generator
  iEmitGen3D *startpos;
  /// the start speed generator
  iEmitGen3D *startspeed;
  /// the start accel generator
  iEmitGen3D *startaccel;
  /// attractor position generator (can be NULL)
  iEmitGen3D *attractor;
  /// attractor force
  float attractor_force;
  /// the time to live for particles in msec
  int timetolive;
  /// the aging table
  csEmitAge *aging;
  /// size of aging table
  int nr_aging_els;
  /// is using Rect Sprites as particles
  bool using_rect_sprites;
  /// size for rect sprites
  float drop_width, drop_height;
  /// shape for regular sprites
  int drop_sides; float drop_radius;


  /// the ages (time they lived) of the particles, in msec.
  int* ages;
  /// particle position
  csVector3 *part_pos;
  /// particle speed m/s
  csVector3 *part_speed;
  /// particle acceleration m/s*s
  csVector3 *part_accel;
  /// attractor position per particle
  csVector3 *part_attract;

  /// give particle i new start values
  void StartParticle(int i);
  /// age particle i elapsed msec. delta_t is elapsed/1000.
  void MoveAgeParticle(int i, int elapsed, float delta_t);

  void SetupObject ();

public:
  /**
   */
  csEmitMeshObject (iObjectRegistry* object_reg, iMeshObjectFactory* factory);
  /// Destructor.
  virtual ~csEmitMeshObject ();

  /// Set the number of particles to use.
  void SetParticleCount (int num) { initialized = false; number = num; }
  /// Get the number of particles
  int GetParticleCount () const { return number; }
  /// Enable or disable lighting.
  void SetLighting (bool l)
  {
    initialized = false;
    lighted_particles = l;
  }
  /// see if lighting is enabled
  bool GetLighting () const
  { return lighted_particles; }
  /// set ttl
  void SetParticleTime (int ttl) { timetolive = ttl; }
  /// get ttl
  int GetParticleTime () const { return timetolive; }
  /// set startposemit
  void SetStartPosEmit(iEmitGen3D *emit)
  {startpos = emit; if(startpos) startpos->IncRef();}
  /// get startposemit
  iEmitGen3D* GetStartPosEmit() const {return startpos;}
  /// set startspeedemit
  void SetStartSpeedEmit(iEmitGen3D *emit)
  {startspeed = emit; if(startspeed) startspeed->IncRef();}
  /// get startspeedemit
  iEmitGen3D* GetStartSpeedEmit() const {return startspeed;}
  /// set startaccelemit
  void SetStartAccelEmit(iEmitGen3D *emit)
  {startaccel = emit; if(startaccel) startaccel->IncRef();}
  /// get startemit
  iEmitGen3D* GetStartAccelEmit() const {return startaccel;}
  /// set startaccelemit
  void SetAttractorEmit(iEmitGen3D *emit)
  {attractor = emit; if(attractor) attractor->IncRef();}
  /// get startemit
  iEmitGen3D* GetAttractorEmit() const {return attractor;}
  /// Set attractor force
  void SetAttractorForce(float f) {attractor_force = f;}
  /// Get attractor force
  float GetAttractorForce() const {return attractor_force;}

  /// get the number of ageing moments
  int GetAgingCount() const {return nr_aging_els;}
  /// add an age
  void AddAge(int time, const csColor& color, float alpha,
        float swirl, float rotspeed, float scale);
  /// get aging data
  void GetAgingMoment(int i, int& time, csColor& color, float &alpha,
        float& swirl, float& rotspeed, float& scale);
  /// replace an age
  void ReplaceAge(int time, const csColor& color, float alpha,
        float swirl, float rotspeed, float scale);

  /// set rectangular particles
  void SetRectParticles(float w, float h)
  { using_rect_sprites = true; drop_width=w; drop_height=h;}
  /// set regular polygon particles
  void SetRegularParticles(int n, float radius)
  { using_rect_sprites = false; drop_sides=n; drop_radius=radius;}
  /// is using rects?
  bool UsingRectParticles() const { return using_rect_sprites; }
  /// get rect size
  void GetRectParticles(float &w, float &h) const
  { w = drop_width; h = drop_height; }
  /// get regular shape
  void GetRegularParticles(int& n, float& radius) const
  { n = drop_sides; radius = drop_radius;}

  /// Update the particle system.
  virtual void Update (csTicks elapsed_time);

  /// For iMeshObject.
  virtual void HardTransform (const csReversibleTransform& t);
  virtual bool SupportsHardTransform () const { return false; }
  /// draw back to front if needed
  virtual bool Draw (iRenderView* rview, iMovable* movable, csZBufMode mode);

  SCF_DECLARE_IBASE_EXT (csParticleSystem);

  //------------------------- iEmitState implementation ----------------
  class EmitState : public iEmitState
  {
    SCF_DECLARE_EMBEDDED_IBASE (csEmitMeshObject);
    virtual void SetParticleCount (int num)
    { scfParent->SetParticleCount (num); }
    virtual void SetLighting (bool l)
    { scfParent->SetLighting (l); }
    virtual int GetParticleCount () const
    { return scfParent->GetParticleCount(); }
    virtual bool GetLighting () const
    { return scfParent->GetLighting(); }
    virtual void SetParticleTime (int l)
    { scfParent->SetParticleTime (l); }
    virtual int GetParticleTime () const
    { return scfParent->GetParticleTime(); }
    virtual void SetStartPosEmit(iEmitGen3D *emit)
    { scfParent->SetStartPosEmit(emit); }
    virtual iEmitGen3D* GetStartPosEmit() const
    { return scfParent->GetStartPosEmit(); }
    virtual void SetStartSpeedEmit(iEmitGen3D *emit)
    { scfParent->SetStartSpeedEmit(emit); }
    virtual iEmitGen3D* GetStartSpeedEmit() const
    { return scfParent->GetStartSpeedEmit(); }
    virtual void SetStartAccelEmit(iEmitGen3D *emit)
    { scfParent->SetStartAccelEmit(emit); }
    virtual iEmitGen3D* GetStartAccelEmit() const
    { return scfParent->GetStartAccelEmit(); }
    virtual void SetAttractorEmit(iEmitGen3D *emit)
    { scfParent->SetAttractorEmit(emit); }
    virtual iEmitGen3D* GetAttractorEmit() const
    { return scfParent->GetAttractorEmit(); }
    virtual void SetAttractorForce(float f) {scfParent->SetAttractorForce(f);}
    virtual float GetAttractorForce() const
    {return scfParent->GetAttractorForce();}
    virtual int GetAgingCount() const { return scfParent->GetAgingCount();}
    virtual void AddAge(int time, const csColor& color, float alpha,
        float swirl, float rotspeed, float scale)
    { scfParent->AddAge(time, color, alpha, swirl, rotspeed, scale);}
    virtual void GetAgingMoment(int i, int& time, csColor& color, float &alpha,
        float& swirl, float& rotspeed, float& scale)
    {scfParent->GetAgingMoment(i, time, color, alpha, swirl, rotspeed, scale);}
    virtual void ReplaceAge(int time, const csColor& color, float alpha,
        float swirl, float rotspeed, float scale)
    { scfParent->ReplaceAge(time, color, alpha, swirl, rotspeed, scale);}
    virtual void SetRectParticles(float w, float h)
    { scfParent->SetRectParticles(w,h); }
    virtual void SetRegularParticles(int s, float r)
    { scfParent->SetRegularParticles(s,r); }
    virtual bool UsingRectParticles() const
    { return scfParent->UsingRectParticles(); }
    virtual void GetRectParticles(float &w, float &h) const
    { scfParent->GetRectParticles(w,h); }
    virtual void GetRegularParticles(int &s, float &r) const
    { scfParent->GetRegularParticles(s,r); }
  } scfiEmitState;
  friend class EmitState;
};

/**
 * Factory for emitter.
 */
class csEmitMeshObjectFactory : public iMeshObjectFactory
{
private:
  iObjectRegistry* object_reg;
  iBase* logparent;

public:
  /// Constructor.
  csEmitMeshObjectFactory (iBase *pParent, iObjectRegistry* object_reg);

  /// Destructor.
  virtual ~csEmitMeshObjectFactory ();

  //------------------------ iMeshObjectFactory implementation --------------
  SCF_DECLARE_IBASE;

  virtual csPtr<iMeshObject> NewInstance ();
  virtual void HardTransform (const csReversibleTransform&) { }
  virtual bool SupportsHardTransform () const { return false; }
  virtual void SetLogicalParent (iBase* lp) { logparent = lp; }
  virtual iBase* GetLogicalParent () const { return logparent; }

  //------------------------- iEmitFactoryState implementation ----------------
  class EmitFactoryState : public iEmitFactoryState
  {
    SCF_DECLARE_EMBEDDED_IBASE (csEmitMeshObjectFactory);
    virtual iEmitFixed* CreateFixed() {return new csEmitFixed(scfParent);}
    virtual iEmitBox* CreateBox() {return new csEmitBox(scfParent);}
    virtual iEmitSphere* CreateSphere() {return new csEmitSphere(scfParent);}
    virtual iEmitCone* CreateCone() {return new csEmitCone(scfParent);}
    virtual iEmitMix* CreateMix() {return new csEmitMix(scfParent);}
    virtual iEmitLine* CreateLine() {return new csEmitLine(scfParent);}
    virtual iEmitCylinder* CreateCylinder()
      {return new csEmitCylinder(scfParent);}
    virtual iEmitSphereTangent* CreateSphereTangent()
      {return new csEmitSphereTangent(scfParent);}
    virtual iEmitCylinderTangent* CreateCylinderTangent()
      {return new csEmitCylinderTangent(scfParent);}
  } scfiEmitFactoryState;
  friend class EmitFactoryState;
};

/**
 * Emit type. This is the plugin you have to use to create instances
 * of csEmitMeshObjectFactory.
 */
class csEmitMeshObjectType : public iMeshObjectType
{
private:
  iObjectRegistry* object_reg;

public:
  SCF_DECLARE_IBASE;

  /// Constructor.
  csEmitMeshObjectType (iBase*);
  /// Destructor.
  virtual ~csEmitMeshObjectType ();
  /// Draw.
  virtual csPtr<iMeshObjectFactory> NewFactory ();

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csEmitMeshObjectType);
    virtual bool Initialize (iObjectRegistry* p)
    { scfParent->object_reg = p; return true; }
  } scfiComponent;
  friend struct eiComponent;
};

#endif // __CS_EMIT_H__
