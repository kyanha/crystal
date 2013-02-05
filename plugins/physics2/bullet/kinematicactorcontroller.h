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
/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2008 Erwin Coumans  http://bulletphysics.com

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

/**
 * 
 */


#ifndef BT_KINEMATIC_CHARACTER_CONTROLLER_H
#define BT_KINEMATIC_CHARACTER_CONTROLLER_H

#include "LinearMath/btVector3.h"

#include "BulletDynamics/Character/btCharacterControllerInterface.h"

#include "BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h"


class btCollisionShape;
class btConvexShape;
class btRigidBody;
class btCollisionWorld;
class btCollisionDispatcher;
class btPairCachingGhostObject;

CS_PLUGIN_NAMESPACE_BEGIN (Bullet2)
{
  ///csKinematicActorController is an object that supports a sliding motion in a world.
  ///It uses a ghost object and convex sweep test to test for upcoming collisions. This is combined with discrete collision detection to recover from penetrations.
  ///Interaction between csKinematicActorController and dynamic rigid bodies needs to be explicity implemented by the user.
  class csKinematicActorController
  {
  protected:

    btScalar m_halfHeight;

    btPairCachingGhostObject* m_ghostObject;
    btConvexShape*	m_convexShape;//is also in m_ghostObject, but it needs to be convex, so we store it here to avoid upcast

    btScalar m_verticalVelocity;
    btScalar m_verticalOffset;
    btScalar m_jumpSpeed;
    btScalar m_maxSlopeRadians; // Slope angle that is set (used for returning the exact value)
    btScalar m_maxSlopeCosine;  // Cosine equivalent of m_maxSlopeRadians (calculated once when set, for optimization)
    btScalar m_gravity;

    btScalar m_turnAngle;

    btScalar m_stepHeight;

    btScalar	m_addedMargin;//@todo: remove this and fix the code

    ///this is the desired walk direction, set by the user
    btVector3	m_walkDirection;
    btVector3	m_normalizedDirection;

    //some internal variables
    btVector3 m_currentPosition;
    btScalar  m_currentStepOffset;
    btVector3 m_targetPosition;

    ///keep track of the contact manifolds
    btManifoldArray	m_manifoldArray;

    bool m_touchingContact;
    btVector3 m_touchingNormal;

    bool  m_wasOnGround;
    bool  m_wasJumping;
    bool	m_useGhostObjectSweepTest;
    bool	m_useWalkDirection;
    btScalar	m_velocityTimeInterval;
    int m_upAxis;

    static btVector3* getUpAxisDirections();

    bool recoverFromPenetration ( btCollisionWorld* collisionWorld);
    void stepUp (btCollisionWorld* collisionWorld);
    void updateTargetPositionBasedOnCollision (const btVector3& hit_normal, btScalar tangentMag = btScalar(0.0), btScalar normalMag = btScalar(1.0));
    void stepForwardAndStrafe (btCollisionWorld* collisionWorld, const btVector3& walkMove);
    void stepDown (btCollisionWorld* collisionWorld, btScalar dt);
  public:
    csKinematicActorController (btPairCachingGhostObject* ghostObject,btConvexShape* convexShape,btScalar stepHeight, int upAxis = 1);
    ~csKinematicActorController ();


    ///btActionInterface interface
    virtual void updateAction( btCollisionWorld* collisionWorld,btScalar deltaTime)
    {
      preStep ( collisionWorld);
      playerStep (collisionWorld, deltaTime);
    }

    ///btActionInterface interface
    void	debugDraw(btIDebugDraw* debugDrawer);

    void setUpAxis (int axis)
    {
      if (axis < 0)
        axis = 0;
      if (axis > 2)
        axis = 2;
      m_upAxis = axis;
    }

    /// This should probably be called setPositionIncrementPerSimulatorStep.
    /// This is neither a direction nor a velocity, but the amount to
    ///	increment the position each simulation iteration, regardless
    ///	of dt.
    /// This call will reset any velocity set by setVelocityForTimeInterval().
    virtual void	setWalkDirection(const btVector3& walkDirection);

    /// Caller provides a velocity with which the character should move for
    ///	the given time period.  After the time period, velocity is reset
    ///	to zero.
    /// This call will reset any walk direction set by setWalkDirection().
    /// Negative time intervals will result in no motion.
    virtual void setVelocityForTimeInterval(const btVector3& velocity,
      btScalar timeInterval);

    void preStep (  btCollisionWorld* collisionWorld);
    void playerStep ( btCollisionWorld* collisionWorld, btScalar dt);

    void jump ();
    
    void setJumpSpeed (btScalar jumpSpeed);
    btScalar getJumpSpeed () const;

    void setGravity(btScalar gravity);
    btScalar getGravity() const;

    /// The max slope determines the maximum angle that the controller can walk up.
    /// The slope angle is measured in radians.
    void setMaxSlope(btScalar slopeRadians);
    btScalar getMaxSlope() const;

    btPairCachingGhostObject* getGhostObject();
    void	setUseGhostSweepTest(bool useGhostObjectSweepTest)
    {
      m_useGhostObjectSweepTest = useGhostObjectSweepTest;
    }

    bool onGround () const;
  };



  ///@todo Interact with dynamic objects,
  ///Ride kinematicly animated platforms properly
  ///More realistic (or maybe just a config option) falling
  /// -> Should integrate falling velocity manually and use that in stepDown()
  ///Support jumping
  ///Support ducking
  class csKinematicClosestNotMeRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
  {
  public:
    csKinematicClosestNotMeRayResultCallback (btCollisionObject* me) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
    {
      m_me = me;
    }

    virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
    {
      if (rayResult.m_collisionObject == m_me)
        return 1.0;

      return ClosestRayResultCallback::addSingleResult (rayResult, normalInWorldSpace);
    }
  protected:
    btCollisionObject* m_me;
  };

  class csKinematicClosestNotMeConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback
  {
  public:
    csKinematicClosestNotMeConvexResultCallback (btCollisionObject* me, const btVector3& up, btScalar minSlopeDot)
      : btCollisionWorld::ClosestConvexResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
      , m_me(me)
      , m_up(up)
      , m_minSlopeDot(minSlopeDot)
    {
    }

    virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult,bool normalInWorldSpace)
    {
      if (convexResult.m_hitCollisionObject == m_me)
        return btScalar(1.0);

      btVector3 hitNormalWorld;
      if (normalInWorldSpace)
      {
        hitNormalWorld = convexResult.m_hitNormalLocal;
      } else
      {
        ///need to transform normal into worldspace
        hitNormalWorld = convexResult.m_hitCollisionObject->getWorldTransform().getBasis()*convexResult.m_hitNormalLocal;
      }

      btScalar dotUp = m_up.dot(hitNormalWorld);
      if (dotUp < m_minSlopeDot) {
        return btScalar(1.0);
      }

      return ClosestConvexResultCallback::addSingleResult (convexResult, normalInWorldSpace);
    }
  protected:
    btCollisionObject* m_me;
    const btVector3 m_up;
    btScalar m_minSlopeDot;
  };
}
CS_PLUGIN_NAMESPACE_END(Bullet2)

#endif
