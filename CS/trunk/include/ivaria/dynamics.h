/*
    Copyright (C) 2002 Anders Stenberg

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


#ifndef __CS_IVARIA_DYNAMICS_H__
#define __CS_IVARIA_DYNAMICS_H__

#include "csutil/scf.h"
#include "csutil/ref.h"

class csVector3;
class csOrthoTransform;
class csMatrix3;
class csPlane3;
struct iMeshWrapper;
struct iSkeletonBone;

struct iDynamicSystem;
struct iRigidBody;
struct iBodyGroup;
struct iDynamicsMoveCallback;
struct iDynamicsCollisionCallback;
struct iJoint;
struct iPolygonMesh;

SCF_VERSION (iDynamics, 0, 0, 2);

/**
 * This is the interface for the actual plugin.
 * It is responsible for creating iDynamicSystem.
 */
struct iDynamics : public iBase
{
  /// Create a rigid body and add it to the simulation
  virtual csPtr<iDynamicSystem> CreateSystem () = 0;
  
  /// Create a rigid body and add it to the simulation
  virtual void RemoveSystem (iDynamicSystem* system) = 0;

  /// Finds a system by name
  virtual iDynamicSystem* FindSystem (const char *name) = 0;

  /// Step the simulation forward by stepsize.
  virtual void Step (float stepsize) = 0;
};

SCF_VERSION (iDynamicSystem, 0, 0, 2);

/**
 * This is the interface for the dynamics core.
 * It handles all bookkeeping for rigid bodies and joints.
 * It also handles collision response.
 * Collision detection is done in another plugin.
 */
struct iDynamicSystem : public iBase
{
  /// returns the underlying object
  virtual iObject *QueryObject (void) = 0;
  /// Set the global gravity.
  virtual void SetGravity (const csVector3& v) = 0;
  /// Get the global gravity.
  virtual const csVector3 GetGravity () const = 0;

  /// Step the simulation forward by stepsize.
  virtual void Step (float stepsize) = 0;

  /// Create a rigid body and add it to the simulation
  virtual csPtr<iRigidBody> CreateBody () = 0;

  /// Create a rigid body and add it to the simulation
  virtual void RemoveBody( iRigidBody* body ) = 0;

  /// Finds a body within a system
  virtual iRigidBody *FindBody (const char *name) = 0;

  /// Create a body group.  Bodies in a group don't collide with each other
  virtual csPtr<iBodyGroup> CreateGroup () = 0;

  /// Remove a group from a simulation.  Those bodies now collide
  virtual void RemoveGroup (iBodyGroup* group) = 0;

  /// Create a joint and add it to the simulation
  virtual csPtr<iJoint> CreateJoint () = 0;

  /// Remove a joint from the simulation
  virtual void RemoveJoint (iJoint* joint) = 0;

  /// Get the default move callback.
  virtual iDynamicsMoveCallback* GetDefaultMoveCallback () = 0;
};

SCF_VERSION (iDynamicsMoveCallback, 0, 0, 1);

/**
 * This is the interface for a dynamics move callback.
 * Set on iRigidBody, it can update attachments after each step.
 */
struct iDynamicsMoveCallback : public iBase
{
  virtual void Execute (iMeshWrapper* mesh, csOrthoTransform& t) = 0;
  virtual void Execute (iSkeletonBone* bone, csOrthoTransform& t) = 0;
};

SCF_VERSION (iDynamicsCollisionCallback, 0, 0, 1);

/**
 * This is the interface for attaching a collider callback to the body
 */
struct iDynamicsCollisionCallback : public iBase
{
  virtual void Execute (iRigidBody *thisbody, iRigidBody *otherbody) = 0;
};

SCF_VERSION (iBodyGroup, 0, 0, 1);

/**
 * Body Group is a collection of bodies which don't collide with
 * each other.  This can speed up processing by manually avoiding
 * certain collisions.  For instance if you have a car built of 
 * many different bodies.  The bodies can be collected into a group
 * and the car will be treated as a single object.
 */
struct iBodyGroup : public iBase
{
   /// Adds a body to this group
   virtual void AddBody (iRigidBody *body) = 0;
   /// Removes a body from this group
   virtual void RemoveBody (iRigidBody *body) = 0;
   /// Tells whether the body is in this group or not
   virtual bool BodyInGroup (iRigidBody *body) = 0;
};

SCF_VERSION (iRigidBody, 0, 0, 2);

/**
 * This is the interface for a rigid body.
 * It keeps all properties for the body.
 * It can also be attached to a movable or a bone,
 * to automatically update it.
 */
struct iRigidBody : public iBase
{
  /// returns the underlying object
  virtual iObject *QueryObject (void) = 0;
  /** 
   * Makes a body stop reacting dynamically.  This is especially useful
   * for environmental objects.  It will also increase speed in some cases
   * by ignoring all physics for that body
   */
  virtual bool MakeStatic (void) = 0;
  /// Returns a static body to a dynamic state
  virtual bool MakeDynamic (void) = 0;
  /// Tells whether a body has been made static or not
  virtual bool IsStatic (void) = 0;

  /// Returns which group a body belongs to
  virtual csRef<iBodyGroup> GetGroup (void) = 0;

  /// Add a collider with a associated friction coefficient
  virtual bool AttachColliderMesh (iMeshWrapper* mesh,
  	const csOrthoTransform& trans, float friction, float density,
	float elasticity) = 0;
  /// Cylinder orientated along its local z axis
  virtual bool AttachColliderCylinder (float length, float radius,
  	const csOrthoTransform& trans, float friction, float density,
	float elasticity) = 0;
  virtual bool AttachColliderBox (const csVector3 &size,
  	const csOrthoTransform& trans, float friction, float density,
	float elasticity) = 0;
  virtual bool AttachColliderSphere (float radius, const csVector3 &offset,
  	float friction, float density, float elasticity) = 0;
  virtual bool AttachColliderPlane (const csPlane3 &plane, float friction,
    float density, float elasticity) = 0;

  /// Set the position
  virtual void SetPosition (const csVector3& trans) = 0;
  /// Get the position
  virtual const csVector3 GetPosition () const = 0;
  /// Set the orientation
  virtual void SetOrientation (const csMatrix3& trans) = 0;
  /// Get the orientation
  virtual const csMatrix3 GetOrientation () const = 0;
  /// Set the transform
  virtual void SetTransform (const csOrthoTransform& trans) = 0;
  /// Get the transform
  virtual const csOrthoTransform GetTransform () const = 0;
  /// Set the linear velocity (movement)
  virtual void SetLinearVelocity (const csVector3& vel) = 0;
  /// Get the linear velocity (movement)
  virtual const csVector3 GetLinearVelocity () const = 0;
  /// Set the angular velocity (rotation)
  virtual void SetAngularVelocity (const csVector3& vel) = 0;
  /// Get the angular velocity (rotation)
  virtual const csVector3 GetAngularVelocity () const = 0;

  /// Set the physic properties
  virtual void SetProperties (float mass, const csVector3& center,
  	const csMatrix3& inertia) = 0;
  /// Get the physic properties. 0 parameters are ignored
  virtual void GetProperties (float* mass, csVector3* center,
    csMatrix3* inertia) = 0;
  /// Set total mass to targetmass, and adjust properties
  virtual void AdjustTotalMass (float targetmass) = 0;

  /// Add a force (world space) (active for one timestep)
  virtual void AddForce	(const csVector3& force) = 0;
  /// Add a torque (world space) (active for one timestep)
  virtual void AddTorque (const csVector3& force) = 0;
  /// Add a force (local space) (active for one timestep)
  virtual void AddRelForce (const csVector3& force) = 0;
  /// Add a torque (local space) (active for one timestep)
  virtual void AddRelTorque (const csVector3& force) = 0 ;
  /**
   * Add a force (world space) at a specific position (world space)
   * (active for one timestep)
   */
  virtual void AddForceAtPos (const csVector3& force, const csVector3& pos) = 0;
  /**
   * Add a force (world space) at a specific position (local space)
   * (active for one timestep)
   */
  virtual void AddForceAtRelPos (const csVector3& force,
    const csVector3& pos) = 0;
  /**
   * Add a force (local space) at a specific position (world space)
   * (active for one timestep)
   */
  virtual void AddRelForceAtPos (const csVector3& force,
  	const csVector3& pos) = 0;
  /**
   * Add a force (local space) at a specific position (loacl space)
   * (active for one timestep)
   */
  virtual void AddRelForceAtRelPos (const csVector3& force,
  	const csVector3& pos) = 0;

  /// Get total force (world space)
  virtual const csVector3 GetForce () const = 0;
  /// Get total torque (world space)
  virtual const csVector3 GetTorque () const = 0;

  /*
  /// Get total force (local space)
  virtual const csVector3& GetRelForce () const = 0;
  /// Get total force (local space)
  virtual const csVector3& GetRelTorque () const = 0;
  */

  /*
  /// Get the number of joints attached to this body
  virtual int GetJointCount () const = 0;
  */

  /// Attach a iMeshWrapper to this body
  virtual void AttachMesh (iMeshWrapper* mesh) = 0;
  /// Returns the attached MeshWrapper
  virtual csRef<iMeshWrapper> GetAttachedMesh () = 0;
  /// Attach a bone to this body
  virtual void AttachBone (iSkeletonBone* bone) = 0;
  /// Returns the attached bone
  virtual csRef<iSkeletonBone> GetAttachedBone () = 0;

  /**
   * Set a callback to be executed when this body moves.
   * If 0, no callback is executed.
   */
  virtual void SetMoveCallback (iDynamicsMoveCallback* cb) = 0;
  /**
   * Set a callback to be executed when this body collides with another
   * If 0, no callback is executed.
   */
  virtual void SetCollisionCallback (iDynamicsCollisionCallback* cb) = 0;

  /// If there's a collision callback with this body, execute it
  virtual void Collision (iRigidBody *other) = 0;

  /// Update transforms for mesh and/or bone
  virtual void Update () = 0;
};

SCF_VERSION (iJoint, 0, 0, 1);

/**
 * This is the interface for a joint.  It works by constraining 
 * the relative motion between the two bodies it attaches.  For 
 * instance if all motion in along the local X axis is constrained
 * then the bodies will stay motionless relative to each other 
 * along an x axis rotated and positioned by the Joint's transform.
 */
struct iJoint : public iBase
{
  /// Set which two bodies to be affected by this joint
  virtual void Attach (iRigidBody* body1, iRigidBody* body2) = 0;
  /// Get an attached body (valid values for body are 0 and 1)
  virtual csRef<iRigidBody> GetAttachedBody (int body) = 0;
  /**
   * Set the local transformation of the joint.  This transform 
   * sets the position of the constraining axes in the world 
   * not relative to the attached bodies.
   */
  virtual void SetTransform (const csOrthoTransform &trans) = 0;
  /// Get the local transformation of the joint
  virtual csOrthoTransform GetTransform () = 0;
  /**
   * Sets the translation constraints on the 3 axes.  If true is 
   * passed for an axis the Joint will constrain all motion along
   * that axis.  If false is passed in then all motion along that 
   * axis free, but bounded by the minimum and maximum distance
   * if set.
   */
  virtual void SetTransConstraints (bool X, bool Y, bool Z) = 0;
  /// True if this axis' translation is constrained
  virtual bool IsXTransConstrained () = 0;
  /// True if this axis' translation is constrained
  virtual bool IsYTransConstrained () = 0;
  /// True if this axis' translation is constrained
  virtual bool IsZTransConstrained () = 0;
  /// Sets the minimum constrained distance between bodies
  virtual void SetMinimumDistance (const csVector3 &min) = 0;
  /// Gets the minimum constrained distance between bodies
  virtual csVector3 GetMinimumDistance () = 0;
  /// Sets the maximum constrained distance between bodies
  virtual void SetMaximumDistance (const csVector3 &max) = 0;
  /// Gets the maximum constrained distance between bodies
  virtual csVector3 GetMaximumDistance () = 0;
  /**
   * Sets the rotational constraints on the 3 axes.  Works like
   * the above translational constraints, but for rotation about
   * the respective axes.
   */
  virtual void SetRotConstraints (bool X, bool Y, bool Z) = 0;
  /// True if this axis' rotation is constrained
  virtual bool IsXRotConstrained () = 0;
  /// True if this axis' rotation is constrained
  virtual bool IsYRotConstrained () = 0;
  /// True if this axis' rotation is constrained
  virtual bool IsZRotConstrained () = 0;
  /// Sets the minimum constrained angle between bodies
  virtual void SetMinimumAngle (const csVector3 &min) = 0;
  /// Gets the minimum constrained angle between bodies
  virtual csVector3 GetMinimumAngle () = 0;
  /// Sets the maximum constrained angle between bodies
  virtual void SetMaximumAngle (const csVector3 &max) = 0;
  /// Gets the maximum constrained angle between bodies
  virtual csVector3 GetMaximumAngle () = 0;
};

#endif // __CS_IVARIA_DYNAMICS_H__

