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

/**
TODO: Implement general surface hugging. Kinematic step direction can currently only hug horizontal surfaces.
*/

#include "cssysdef.h"
#include "dynamicactor.h"

#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"

using namespace CS::Physics;
using namespace CS::Collisions;

CS_PLUGIN_NAMESPACE_BEGIN (Bullet2)
{
  static const float DefaultGroundAngleCosThresh = 0.7f;             // min cos of angle between ground and up-axis (45 degrees)
  static const btVector3 BTUpVector (CSToBullet (csVector3 (0.0f, 1.0f, 0.0f), 1));
  static const float AddedMargin = 0.02f;

  csBulletActorMotionState::csBulletActorMotionState (csBulletRigidBody* body,
    const btTransform& initialTransform,
    const btTransform& principalAxis) :
  csBulletMotionState (body, initialTransform, principalAxis)
  {

  }

  void csBulletActorMotionState::setWorldTransform (const btTransform& trans)
  {
    csBulletMotionState::setWorldTransform (trans);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // csBulletDynamicActor

  csBulletDynamicActor::csBulletDynamicActor (BulletDynamicActorFactory* factory)
    : scfImplementationType (this, factory), onGround (false)
  {
    group = dynamic_cast<CollisionGroup*> (factory->GetCollisionGroup ());

    SetStepHeight (factory->GetStepHeight ());
    SetWalkSpeed (factory->GetWalkSpeed ());
    SetJumpSpeed (factory->GetJumpSpeed ());
    SetKinematicStepsEnabled (factory->GetKinematicStepsEnabled ());
    SetAirControlFactor (factory->GetAirControlFactor ());
    
    if (collider->GetColliderType () == COLLIDER_COMPOUND)
    {
      // non-convex actor shape currently not supported with kinematic step correction
      kinematicSteps = false;
    }
    
    // Do not allow any simulated rotation
    btBody->setAngularFactor (btVector3 (0, 0, 0));
    btBody->setDamping (0, 1);
  }

  csBulletDynamicActor::~csBulletDynamicActor ()
  {

  }

  csBulletMotionState* csBulletDynamicActor::CreateMotionState (const btTransform& trans)
  {
    return motionState = new csBulletActorMotionState
      (this, trans, collider->GetPrincipalAxisTransform ());
  }

  bool csBulletDynamicActor::AddBulletObject ()
  {
    return csBulletRigidBody::AddBulletObject ();
  }

  bool csBulletDynamicActor::RemoveBulletObject ()
  {
    return csBulletRigidBody::RemoveBulletObject ();
  }

  void csBulletDynamicActor::RebuildObject ()
  {
    csBulletRigidBody::RebuildObject ();
    
    if (collider->GetColliderType () == COLLIDER_COMPOUND)
    {
      // non-convex actor shape currently not supported with kinematic step correction
      kinematicSteps = false;
    }
  }

  void csBulletDynamicActor::SetTransform (const csOrthoTransform& trans)
  {
    csBulletRigidBody::SetTransform (trans);
  }

  void csBulletDynamicActor::Walk (csVector3 newVel)
  {
    newVel.Normalize ();
    newVel *= walkSpeed;

    csVector3 vel = GetLinearVelocity ();
    if (IsFreeFalling ())
    {
      // cannot entirely control movement mid-air
      float realAirFactor = airControlFactor * system->GetWorldTimeStep ();
      newVel = realAirFactor * newVel;
      newVel += (1.f - realAirFactor) * vel;
    }

    SetLinearVelocity (newVel);
  }

  void csBulletDynamicActor::WalkHorizontal (csVector2 newVel2)
  {
    newVel2.Normalize ();
    newVel2 *= walkSpeed;

    csVector3 vel = GetLinearVelocity ();
    if (IsFreeFalling ())
    {
      // cannot entirely control movement mid-air
      float realAirFactor = airControlFactor * system->GetWorldTimeStep ();
      newVel2 = realAirFactor * newVel2;
      newVel2 += (1.f - realAirFactor) * csVector2 (vel.x, vel.z);
    }

    // previous vertical movement is unchanged
    csVector3 newVel (newVel2.x, vel[1], newVel2.y);
    SetLinearVelocity (newVel);
  }

  /// Applies an upward impulse to this actor
  void csBulletDynamicActor::Jump ()
  {
    // apply upward impulse to actor
    csVector3 vel = GetLinearVelocity ();
    vel[1] += GetJumpSpeed ();
    SetLinearVelocity (vel);
  }

  void csBulletDynamicActor::StopMoving ()
  {
    if (!IsFreeFalling ())
    {
      csVector3 zero (0);
      SetLinearVelocity (zero);
    }
  }

  bool csBulletDynamicActor::IsFreeFalling () const
  { 
    return 
      (!IsOnGround () ||                 // not in contact with ground or
      IsMovingUpward ()) &&              // moving upward (away from ground)
      DoesGravityApply ();               // and gravity applies
  }

  void csBulletDynamicActor::UpdatePreStep (float dt)
  {
    onGround = TestOnGround ();

    if (kinematicSteps && !IsFreeFalling () && btBody->getLinearVelocity ().length2 () > 0)
    {
      dt *= 0.001f;
      StepUp (dt);
      //stepForwardAndStrafe (dt);
    }
  }

  void csBulletDynamicActor::UpdatePostStep (float dt)
  {
    if (kinematicSteps && !IsFreeFalling () && btBody->getLinearVelocity ().length2 () > 0)
    {
      dt *= 0.001f;
      StepDown (dt);
      SetLinearVelocity (0);              // stop moving
    }
  }
  
  inline btConvexShape* csBulletDynamicActor::GetConvexShape () const { return (btConvexShape*)btBody->getCollisionShape (); }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Code based on btKinematicCharacterController

  void csBulletDynamicActor::StepUp (float dt)
  {
    btVector3& currentPosition = btObject->getWorldTransform ().getOrigin ();
    //m_targetPosition = currentPosition + BTUpVector * stepHeight;

    currentPosition = currentPosition + BTUpVector * stepHeight;
    // TODO: This method tests intersections with objects beneath the object when moving upward. Highly unnecessary.

    // phase 1: up
    //btTransform start;
    //start.setIdentity ();
    //start.setOrigin (currentPosition + BTUpVector * (GetConvexShape ()->getMargin () + AddedMargin));
    //
    //btTransform end;
    //end.setIdentity ();
    //end.setOrigin (m_targetPosition);

    //csKinematicClosestNotMeConvexResultCallback callback (btObject, -BTUpVector, DefaultGroundAngleCosThresh);
    //callback.m_collisionFilterGroup = group->value;
    //callback.m_collisionFilterMask = group->mask;
    //
    //sector->GetBulletWorld ()->convexSweepTest (GetConvexShape (), start, end, callback);

    //if (callback.hasHit ())
    //{
    //  // Only modify the position if the hit was a slope and not a wall or ceiling.
    //  if (callback.m_hitNormalWorld.dot (BTUpVector) > 0.0)
    //  {
    //    // we moved up only a fraction of the step height, then hit something underneath us!?
    //    currentPosition.setInterpolate3 (currentPosition, m_targetPosition, callback.m_closestHitFraction);
    //  }
    //  m_currentStepOffset = stepHeight * callback.m_closestHitFraction;
    //} 
    //else 
    //{
    //  m_currentStepOffset = stepHeight;
    //  currentPosition = m_targetPosition;
    //}
  }

  void csBulletDynamicActor::StepDown (float dt)
  {
    btVector3& currentPosition = btObject->getWorldTransform ().getOrigin ();

    btTransform start, end;

    // phase 3: down
    
    // we went up by stepHeight during the StepUp routine, and what goes up must come down!
    btVector3 targetPosition = currentPosition;
    BulletVectorComponent (targetPosition, 1) -= stepHeight;

    start.setIdentity ();
    end.setIdentity ();

    start.setOrigin (currentPosition);
    end.setOrigin (targetPosition);

    csKinematicClosestNotMeConvexResultCallback callback (btObject, BTUpVector, DefaultGroundAngleCosThresh);
    callback.m_collisionFilterGroup = group->value;
    callback.m_collisionFilterMask = group->mask;

    sector->GetBulletWorld ()->convexSweepTest (GetConvexShape (), start, end, callback, sector->GetBulletWorld ()->getDispatchInfo ().m_allowedCcdPenetration);

    if (callback.hasHit ())
    {
      // we dropped a fraction of the height -> hit floor
      currentPosition.setInterpolate3 (currentPosition, targetPosition, callback.m_closestHitFraction);
    } 
    else 
    {
      // we dropped the full height without hitting anything
      currentPosition = targetPosition;
    }
  }
}
CS_PLUGIN_NAMESPACE_END (Bullet2)
