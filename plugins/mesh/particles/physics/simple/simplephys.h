/*
    Copyright (C) 2003 by Jorrit Tyberghein, John Harger, Daniel Duhprey

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

#ifndef __CS_PARTPHYSTMPL_H__
#define __CS_PARTPHYSTMPL_H__

#include "iutil/comp.h"
#include "iutil/event.h"
#include "iutil/eventh.h"
#include "iutil/virtclk.h"
#include "imesh/particles.h"
#include "csutil/randomgen.h"

/**
 * This is an abstract implementation of iParticlePhysics. It can be
 * used to write custom particle physics implementations more easily.
 */
class csParticlesPhysicsSimple : public iParticlesPhysics
{
  /// Object registry.
  iObjectRegistry* object_reg;

  struct particles_object {
    iParticlesObjectState *particles;
    csArray<csParticlesData> *data;
  };
  csArray<particles_object*> partobjects;

  csRef<iVirtualClock> vclock;
  csTicks leftover_time;

  csRandomGen rng;

  void StepPhysics (float elapsed_time, iParticlesObjectState *particles,
    csArray<csParticlesData> *data);
public:
  SCF_DECLARE_IBASE;

  /// Constructor
  csParticlesPhysicsSimple (iBase *p);

  /// Destructor
  virtual ~csParticlesPhysicsSimple ();

  /**
   * Initialize this plugin.
   */
  bool Initialize (iObjectRegistry* reg);

  /**
   * Register a particles object with the physics plugin
   */
  virtual void RegisterParticles (iParticlesObjectState *particles,
    csArray<csParticlesData> *data);

  /**
   * Remove a particles object from the physics plugin
   */
  virtual void RemoveParticles (iParticlesObjectState *particles);

  /**
   * Event Handler
   */
  bool HandleEvent (iEvent &event);

  /**
   * iComponent implementation.
   */
  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE (csParticlesPhysicsSimple);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize (p); }
  } scfiComponent;

  struct eiEventHandler : public iEventHandler
  {
    SCF_DECLARE_EMBEDDED_IBASE (csParticlesPhysicsSimple);
    virtual bool HandleEvent (iEvent &event)
    { return scfParent->HandleEvent (event); }
  } scfiEventHandler;
  friend struct eiEventHandler;

};

#endif // __CS_PARTPHYSMPL_H__
