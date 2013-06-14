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
#ifndef __CS_BULLET_COLLISIONACTOR_H__
#define __CS_BULLET_COLLISIONACTOR_H__

#include "cssysdef.h"
#include "actor.h"
#include "common2.h"
#include "collisionghost.h"
#include "kinematicactorcontroller.h"

CS_PLUGIN_NAMESPACE_BEGIN (Bullet2)
{

class BulletCollisionActorFactory : public scfVirtImplementationExt1<
  BulletCollisionActorFactory, BulletGhostCollisionObjectFactory, CS::Collisions::iCollisionActorFactory> 
{
  float stepHeight;
  float walkSpeed, jumpSpeed;
  float maximumSlope;
  float airControlFactor;

public:
  BulletCollisionActorFactory
    (csBulletSystem* system, CS::Collisions::iCollider* collider = nullptr)
    : scfImplementationType (this, system, collider),
    stepHeight (.5f),
    walkSpeed (10.f),
    jumpSpeed (10.f),
    maximumSlope (0.785398163f),
    airControlFactor (0.04f)
    {}

  virtual csPtr<CS::Collisions::iActor> CreateActor ();
  virtual csPtr<CS::Collisions::iCollisionActor> CreateCollisionActor ();
  virtual csPtr<CS::Collisions::iCollisionObject> CreateCollisionObject ();

  virtual float GetStepHeight () const { return stepHeight; }
  virtual void SetStepHeight (float h) { stepHeight = h; }

  virtual float GetMaximumSlope () const { return maximumSlope; };
  virtual void SetMaximumSlope (float slope) { maximumSlope = slope; };

  virtual float GetWalkSpeed () const { return walkSpeed; }
  virtual void SetWalkSpeed (float s) { walkSpeed = s; }

  virtual float GetJumpSpeed () const { return jumpSpeed; }
  virtual void SetJumpSpeed (float s) { jumpSpeed = s; }

  virtual float GetAirControlFactor () const { return airControlFactor; }
  virtual void SetAirControlFactor (float f) { airControlFactor = f; }
};

 /**
  * TODO: IsMovingUp
  */
class csBulletCollisionActor : public scfVirtImplementationExt1<csBulletCollisionActor,
    csBulletGhostCollisionObject, CS::Collisions::iCollisionActor>, public csActor
{
  csKinematicActorController* controller;
  float airControlFactor;
  float walkSpeed;
  bool gravityEnabled;
  float stepHeight;

public:
  void CreateCollisionActor (CS::Collisions::iCollisionActorFactory* props);

public:
  csBulletCollisionActor (csBulletSystem* sys);
  virtual ~csBulletCollisionActor ();

  virtual iObject* QueryObject () { return (iObject*) this; }

  //-- iCollisionObject
  virtual CS::Collisions::iCollisionObject* QueryCollisionObject ()
  { return dynamic_cast<CS::Collisions::iCollisionObject*> (this); }
  virtual CS::Collisions::iActor* QueryActor ()
  { return dynamic_cast<CS::Collisions::iActor*> (this); }

  virtual bool IsPhysicalObject () const { return false; }

  virtual CS::Collisions::CollisionObjectType GetObjectType () const
  { return CS::Collisions::COLLISION_OBJECT_ACTOR; }

  virtual bool AddBulletObject ();

  //-- iCollisionActor
  virtual bool IsOnGround () const;

  virtual void SetAttachedCamera (iCamera* camera);
  
  virtual void UpdatePreStep (float delta);
  virtual void UpdatePostStep (float delta);

  float GetGravity () const;
  void SetGravity (float gravity);

  virtual void SetTransform (const csOrthoTransform& trans);
  virtual void SetRotation (const csMatrix3& rot);

  virtual void Walk (csVector3 vel);
  virtual void WalkHorizontal (csVector2 vel);
  virtual void Jump ();
  virtual void StopMoving ();
  
  virtual float GetAirControlFactor () const { return airControlFactor; }
  virtual void SetAirControlFactor (float f) { airControlFactor = f; }
  
  virtual float GetJumpSpeed () const { return 0; }
  virtual void SetJumpSpeed (float jumpSpeed);
  
  virtual float GetStepHeight () const { return stepHeight; }
  virtual void SetStepHeight (float stepHeight);

  virtual float GetMaximumSlope () const;
  virtual void SetMaximumSlope (float slopeRadians);

  virtual float GetWalkSpeed () const { return walkSpeed; }
  virtual void SetWalkSpeed (float s) { walkSpeed = s; }

  virtual bool GetGravityEnabled () const { return gravityEnabled; }
  virtual void SetGravityEnabled (bool enabled) { gravityEnabled = enabled; }

  bool DoesGravityApply () const { return GetGravityEnabled () && GetGravity () != 0; }
  
  /// Whether the actor is not on ground and gravity applies
  virtual bool IsFreeFalling () const
  { 
    // kinematic character controller does not reveal its linear velocity
    return 
      (!IsOnGround ()) &&                // not touching the ground
      DoesGravityApply ();               // and gravity applies
  }

  btPairCachingGhostObject* GetBulletGhostObject ()
  {
    return static_cast<btPairCachingGhostObject*> (btObject);
  }
};

}
CS_PLUGIN_NAMESPACE_END (Bullet2)
#endif
