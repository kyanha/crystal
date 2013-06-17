/*
    Copyright (C) 2012 by Dominik Seifert

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

#ifndef __CS_BULLET_DYNAMICACTOR_H__
#define __CS_BULLET_DYNAMICACTOR_H__

#include "actor.h"
#include "common2.h"
#include "rigidbody2.h"

//#include "kinematicactorcontroller.h"

#include "csutil/custom_new_disable.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "csutil/custom_new_enable.h"

CS_PLUGIN_NAMESPACE_BEGIN (Bullet2)
{
  class csBulletActorMotionState : public csBulletMotionState
  {
  private:

  public:
    csBulletActorMotionState (csBulletRigidBody* body,
		         const btTransform& initialTransform,
		         const btTransform& principalAxis);

    virtual void setWorldTransform (const btTransform& trans);
  };

  class csBulletDynamicActor : public scfVirtImplementationExt1<csBulletDynamicActor,
	csBulletRigidBody, 
    CS::Physics::iDynamicActor>, public csActor
  {
    friend class csBulletSystem;
    friend class csBulletSector;
    friend class BulletDynamicActorFactory;

  private:
    float stepHeight;
    float walkSpeed, jumpSpeed;
    float airControlFactor;
    bool onGround;
    bool kinematicSteps;

  protected:
    virtual csBulletMotionState* CreateMotionState (const btTransform& trans);

  public:
    csBulletDynamicActor (BulletDynamicActorFactory* factory);
    virtual ~csBulletDynamicActor ();
    
    virtual CS::Physics::PhysicalObjectType GetPhysicalObjectType () const
    { return CS::Physics::PHYSICAL_OBJECT_DYNAMICACTOR; }

    bool AddBulletObject ();
    bool RemoveBulletObject ();
    void RebuildObject ();

    //-- iCollisionActor
    virtual CS::Collisions::iCollisionObject* QueryCollisionObject ()
    { return dynamic_cast<CS::Collisions::iCollisionObject*> (this); }
    
    virtual CS::Collisions::iActor* QueryActor ()
    { return dynamic_cast<CS::Collisions::iActor*> (this); }

    virtual void UpdatePreStep (float delta);
    virtual void UpdatePostStep (float delta);
    
    virtual void Walk (csVector3 dir);
    virtual void WalkHorizontal (csVector2 newVel2);
    virtual void Jump ();
    virtual void StopMoving ();

    virtual bool IsFreeFalling () const;

    bool TestOnGround ();

    //virtual bool IsOnGround () const { return touchedGroundObjectCount > 0; }
    virtual bool IsOnGround () const { return onGround; }

    virtual float GetStepHeight () const { return stepHeight; }
    virtual void SetStepHeight (float h) { stepHeight = h; }

    virtual float GetWalkSpeed () const { return walkSpeed; }
    virtual void SetWalkSpeed (float s) { walkSpeed = s; }

    virtual float GetJumpSpeed () const { return jumpSpeed; }
    virtual void SetJumpSpeed (float s) { jumpSpeed = s; }

    virtual float GetAirControlFactor () const { return airControlFactor; }
    virtual void SetAirControlFactor (float f) { airControlFactor = f; }
    
    virtual bool GetKinematicStepsEnabled () const { return kinematicSteps; }
    virtual void SetKinematicStepsEnabled (bool u) { kinematicSteps = u; }

    /// Override SetTransform
    virtual void SetTransform (const csOrthoTransform& trans);
    virtual void SetRotation (const csMatrix3& rot);
    
    virtual bool GetGravityEnabled () const
    { return csBulletRigidBody::GetGravityEnabled (); }
    virtual void SetGravityEnabled (bool enabled)
    { csBulletRigidBody::SetGravityEnabled (enabled); }

    // Kinematic stuff
    inline btConvexShape* GetConvexShape () const;

  protected:
    void StepUp (float dt);
    void StepDown (float dt);
  };
}

CS_PLUGIN_NAMESPACE_END (Bullet2)

#endif
