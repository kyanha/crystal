/*
    Copyright (C) 2013 Christian Van Brussel, Institute of Information
      and Communication Technologies, Electronics and Applied Mathematics
      at Universite catholique de Louvain, Belgium
      http://www.uclouvain.be/en-icteam.html
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
#include "cssysdef.h"
#include "collisionactor.h"
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

#include "iengine/scenenode.h"
#include "ivaria/collisions.h"

using namespace CS::Collisions;

CS_PLUGIN_NAMESPACE_BEGIN (Bullet2)
{   

  //----------------------------- BulletCollisionActorFactory -----------------------------

  csPtr<CS::Collisions::iCollisionActor> BulletCollisionActorFactory::CreateCollisionActor ()
  {
    return new BulletCollisionActor (this);
  }

  csPtr<CS::Collisions::iCollisionObject> BulletCollisionActorFactory::CreateCollisionObject () 
  { 
    return new BulletCollisionActor (this);
  }

  //----------------------------- BulletCollisionActor -----------------------------

  BulletCollisionActor::BulletCollisionActor (BulletCollisionActorFactory* factory)
    : scfImplementationType (this, factory->system)
  {
    CreateGhostCollisionObject (factory);

    btPairCachingGhostObject* ghost = GetPairCachingGhostObject ();
    btConvexShape* convShape = (btConvexShape*) (ghost->getCollisionShape ());
    controller = new btKinematicCharacterController
      (ghost, convShape, factory->stepHeight, 1);
    controller->setMaxSlope (factory->maximumSlope);
    controller->setJumpSpeed (factory->jumpSpeed);

    ghost->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);
  }

  BulletCollisionActor::~BulletCollisionActor ()
  {
    delete controller;
  }

  void BulletCollisionActor::SetRotation (const csMatrix3& rot)
  {
    btObject->getWorldTransform ().setBasis (CSToBullet (rot));

    iSceneNode* sceneNode = GetAttachedSceneNode ();
    if (sceneNode)
      sceneNode->GetMovable ()->GetTransform ().SetT2O (rot);
    if (camera)
      camera->GetTransform ().SetT2O (rot);
  }

  void BulletCollisionActor::Rotate (float yaw, float pitch)
  {
    // Compute the Euler angles of the current rotation
    btMatrix3x3 rotation = btObject->getWorldTransform ().getBasis ();
    csQuaternion quaternion;
    quaternion.SetMatrix (BulletToCS (rotation).GetInverse ());

    // TODO: merge this code within the csQuaternion class
    csVector3 v = quaternion.v;
    float w = quaternion.w;

    const float case1 = HALF_PI;
    const float case2 = -HALF_PI;

    float sine = -2.0f * (v.y*v.z - w*v.x);

    float currentPitch;
    if (sine >= 1)     // cases where value is 1 or -1 cause NAN
      currentPitch = case1;
    else if (sine <= -1)
      currentPitch = case2;
    else
      currentPitch = asinf (sine);

    float currentYaw = atan2f (2.0f * (w*v.y + v.x*v.z), (w*w - v.y*v.y - v.x*v.x + v.z*v.z)) ;

    // Apply the rotation delta
    currentYaw += yaw;
    currentPitch += pitch;

    // Clamping of the pitch value
    currentPitch = csMin (currentPitch, 1.4f);
    currentPitch = csMax (currentPitch, -1.4f);

    // Compute and apply the new rotation matrix
    rotation = btMatrix3x3 (btQuaternion (btVector3 (0.0f, 1.0f, 0.0f), currentYaw))
      * btMatrix3x3 (btQuaternion (btVector3 (1.0f, 0.0f, 0.0f), currentPitch));
    btObject->getWorldTransform ().setBasis (rotation);

    iSceneNode* sceneNode = GetAttachedSceneNode ();
    if (sceneNode)
      sceneNode->GetMovable ()->GetTransform ().SetO2T (BulletToCS (rotation));
    if (camera)
      camera->GetTransform ().SetO2T (BulletToCS (rotation));
  }

  bool BulletCollisionActor::AddBulletObject ()
  {
    if (insideWorld)
      RemoveBulletObject ();
    insideWorld = true;

    controller->setGravity (- sector->bulletWorld->getGravity ()[1] * 3);
    sector->bulletWorld->addCollisionObject (btObject, group->value, group->mask);
    sector->bulletWorld->addAction (this);

    return true;
  }

  bool BulletCollisionActor::RemoveBulletObject ()
  {
    insideWorld = false;

    sector->bulletWorld->removeAction (this);
    sector->bulletWorld->removeCollisionObject (btObject);

    return true;
  }

  void BulletCollisionActor::SetStepHeight (float height)
  {
    controller->setStepHeight (height * system->GetInternalScale ());
  }

  float BulletCollisionActor::GetStepHeight () const
  {
    return controller->getStepHeight () * system->GetInverseInternalScale ();
  }

  void BulletCollisionActor::SetMaximumSlope (float slope) 
  { 
    controller->setMaxSlope (slope);
  }

  float BulletCollisionActor::GetMaximumSlope () const 
  {
    return controller->getMaxSlope ();
  }

  void BulletCollisionActor::SetJumpSpeed (float jumpSpeed) 
  {
    controller->setJumpSpeed (jumpSpeed * system->GetInternalScale ());
  }

  void BulletCollisionActor::SetSpeed (const csVector3& speed)
  {
    // TODO: use a real value for the time interval parameter?
    controller->setVelocityForTimeInterval
      (btObject->getWorldTransform ().getBasis () * CSToBullet (speed, system->GetInternalScale ()), INT_MAX);
  }

  void BulletCollisionActor::Jump ()
  {
    controller->jump ();
  }
  
  bool BulletCollisionActor::IsOnGround () const
  {
    return controller->onGround ();
  }
/*
  float BulletCollisionActor::GetGravity () const
  {
    return controller->getGravity ();
  }
*/

  void BulletCollisionActor::SetGravity (float gravity)
  {
    // For some reason, the actor controller needs a 3G acceleration
    controller->setGravity (- gravity * 3);
  }

  void BulletCollisionActor::updateAction (btCollisionWorld* collisionWorld, btScalar deltaTime)
  {
    // Update the character controller
    controller->updateAction (collisionWorld, deltaTime);

    // Update the camera & movable position
    if (camera || sceneNode)
    {
      csVector3 position =
	BulletToCS (controller->getGhostObject ()->getWorldTransform ().getOrigin (),
		    system->GetInverseInternalScale ());

      if (camera)
	camera->GetTransform ().SetOrigin (position);

      if (sceneNode)
      {
	sceneNode->GetMovable ()->SetFullPosition (position);
	sceneNode->GetMovable ()->UpdateMove ();
      }
    }
  }

}
CS_PLUGIN_NAMESPACE_END (Bullet2)
