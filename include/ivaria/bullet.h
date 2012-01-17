/*
    Copyright (C) 2007 by Jorrit Tyberghein

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

#ifndef __CS_IVARIA_BULLET_H__
#define __CS_IVARIA_BULLET_H__

/**\file
 * Bullet-specific interfaces
 */

#include "csutil/scf_interface.h"
#include "iutil/objreg.h"
#include "iengine/mesh.h"
#include "iengine/engine.h"
#include "imesh/genmesh.h"
#include "csgeom/tri.h"
#include "cstool/primitives.h"

struct iView;
struct iRigidBody;
struct csLockedHeightData;
struct iTerrainCell;
struct iTerrainSystem;

namespace CS
{
namespace Physics
{
namespace Bullet
{

struct iKinematicCallback;
struct iSoftBody;
struct iPivotJoint;
struct iTerrainCollider;

/**
 * The type of a Bullet's collider.
 */
enum BodyType
{
  UNDEFINED_BODY = 0,     /*!< Undefined body type. */
  RIGID_BODY,             /*!< The body is a rigid body. */
  SOFT_BODY,              /*!< The body is a soft body. */
  TERRAIN                 /*!< The body is a terrain collider. */
};

/**
 * A generic body in Bullet's dynamic simulation
 */
struct iBody : public virtual iBase
{
  SCF_INTERFACE(CS::Physics::Bullet::iBody, 1, 0, 0);

  /// Return the type of this body
  virtual BodyType GetType () const = 0;

  /**
   * Query the iRigidBody interface of this body. It returns null if the
   * interface is not valid, ie GetType() is not CS::Physics::Bullet::RIGID_BODY.
   */
  virtual ::iRigidBody* QueryRigidBody () = 0;

  /**
   * Query the CS::Physics::Bullet::iSoftBody interface of this body. It returns null if the
   * interface is not valid, ie GetType() is not CS::Physics::Bullet::SOFT_BODY.
   */
  virtual iSoftBody* QuerySoftBody () = 0;

  /**
   * Query the CS::Physics::Bullet::iTerrainCollider interface of this body. It returns null if the
   * interface is not valid, ie GetType() is not CS::Physics::Bullet::TERRAIN.
   */
  virtual iTerrainCollider* QueryTerrainCollider () = 0;
};

/**
 * Return structure for the CS::Physics::Bullet::iDynamicSystem::HitBeam() routine. It returns
 * whether a rigid body, a soft body or a physical terrain collider has been hit.
 * \sa csHitBeamResult csSectorHitBeamResult
 */
struct HitBeamResult
{
  HitBeamResult ()
  : hasHit (false), body (0), isect (0.0f), normal (0.0f), vertexIndex (0)
  {}

  /**
   * Whether the beam has hit a body or not.
   */
  bool hasHit;

  /**
   * The resulting body that was hit, or 0 if no body was hit.
   */
  iBody* body;

  /**
   * Intersection point in world space.
   */
  csVector3 isect;

  /**
   * Normal to the surface of the body at the intersection point.
   */
  csVector3 normal;

  /**
   * The index of the closest vertex of the soft body to be hit. This is only valid
   * if it is a soft body which is hit.
   */
  size_t vertexIndex;
};

/**
 * The debug modes to be used with CS::Physics::Bullet::iDynamicSystem::DebugDraw().
 */
enum DebugMode
{
  DEBUG_NOTHING = 0,     /*!< Nothing will be displayed. */
  DEBUG_COLLIDERS = 1,   /*!< Display the colliders of the bodies. */
  DEBUG_AABB = 2,        /*!< Display the axis aligned bounding boxes of the bodies. */
  DEBUG_JOINTS = 4       /*!< Display the joint positions and limits. */
};

/**
 * The Bullet implementation of iDynamicSystem also implements this
 * interface.
 * \sa iDynamicSystem iODEDynamicSystemState
 */
struct iDynamicSystem : public virtual iBase
{
  SCF_INTERFACE(CS::Physics::Bullet::iDynamicSystem, 3, 0, 1);

  /**
   * Draw the debug informations of the dynamic system. This has to be called
   * at each frame, and will add 2D lines on top of the rendered scene. The
   * objects to be displayed are defined by SetDebugMode().
   */
  virtual void DebugDraw (iView* rview) = 0;

  /**
   * Follow a beam from start to end and return the first body that is hit.
   * \return True if a body was hit, false otherwise.
   * \sa HitBeamResult iMeshWrapper::HitBeam() iSector::HitBeam()
   * iSector::HitBeamPortals()
   */
  virtual HitBeamResult HitBeam (const csVector3 &start,
				 const csVector3 &end) = 0;

  /**
   * Set the internal scale to be applied to the whole dynamic world. Use this
   * to put back the range of dimensions you use for your objects to the one
   * Bullet was designed for.
   *
   * Bullet does not work well if the dimensions of your objects are smaller
   * than 0.1 to 1.0 units or bigger than 10 to 100 units. Use this method to
   * fix the problem.
   *
   * \warning You have to call this method before adding any objects in the
   * dynamic world, otherwise the objects won't have the same scale.
   */
  virtual void SetInternalScale (float scale) = 0;

  /**
   * Set the parameters of the constraint solver. Use this if you want to find a
   * compromise between accuracy of the simulation and performance cost.
   * \param timeStep The internal, constant, time step of the simulation, in seconds.
   * A smaller value gives better accuracy. Default value is 1/60 s (ie 0.0166 s).
   * \param maxSteps Maximum number of steps that Bullet is allowed to take each
   * time you call iDynamicSystem::Step(). If you pass a very small time step as
   * the first parameter, then you must increase the number of maxSteps to
   * compensate for this, otherwise your simulation is 'losing' time. Default value
   * is 1. If you pass maxSteps=0 to the function, then it will assume a variable
   * tick rate. Don't do it.
   * \param iterations Number of iterations of the constraint solver. A reasonable
   * range of iterations is from 4 (low quality, good performance) to 20 (good
   * quality, less but still reasonable performance). Default value is 10. 
   */
  virtual void SetStepParameters (float timeStep, size_t maxSteps,
				  size_t iterations) = 0;

  /**
   * Set the mode to be used when displaying debug informations. The default value
   * is 'CS::Physics::Bullet::DEBUG_COLLIDERS | CS::Physics::Bullet::DEBUG_JOINTS'.
   * \remark Don't forget to call DebugDraw() at each frame to effectively display
   * the debug informations.
   */
  virtual void SetDebugMode (DebugMode mode) = 0;

  /**
   * Return the current mode used when displaying debug informations.
   */
  virtual DebugMode GetDebugMode () = 0;

  /**
   * Set whether this dynamic world can handle soft bodies or not.
   * \warning You have to call this method before adding any objects in the
   * dynamic world.
   */
  virtual void SetSoftBodyWorld (bool isSoftBodyWorld) = 0;

  /**
   * Return whether this dynamic world can handle soft bodies or not.
   */
  virtual bool GetSoftBodyWorld () = 0;

  /**
   * Return the number of soft bodies in this dynamic world.
   */
  virtual size_t GetSoftBodyCount () = 0;

  /**
   * Return the soft body at the given index.
   */
  virtual iSoftBody* GetSoftBody (size_t index) = 0;

  /**
   * Create a soft body rope.
   * \param start Start position of the rope.
   * \param end End position of the rope.
   * \param segmentCount Number of segments in the rope.
   * \remark You must call SetSoftBodyWorld() prior to this.
   */
  virtual iSoftBody* CreateRope (csVector3 start, csVector3 end,
				 uint segmentCount) = 0;

  /**
   * Create a soft body rope with explicit positions of the vertices.
   * \param vertices The array of positions to use for the vertices.
   * \param vertexCount The amount of vertices for the rope.
   * \remark You must call SetSoftBodyWorld() prior to this.
   */
  virtual iSoftBody* CreateRope (csVector3* vertices, size_t vertexCount) = 0;

  /**
   * Create a soft body cloth.
   * \param corner1 The position of the top left corner.
   * \param corner2 The position of the top right corner.
   * \param corner3 The position of the bottom left corner.
   * \param corner4 The position of the bottom right corner.
   * \param segmentCount1 Number of horizontal segments in the cloth.
   * \param segmentCount2 Number of vertical segments in the cloth.
   * \param withDiagonals Whether there must be diagonal segments in the cloth
   * or not. Diagonal segments will make the cloth more rigid.
   * \remark You must call SetSoftBodyWorld() prior to this.
   */
  virtual iSoftBody* CreateCloth (csVector3 corner1, csVector3 corner2,
				  csVector3 corner3, csVector3 corner4,
				  uint segmentCount1, uint segmentCount2,
				  bool withDiagonals = false) = 0;

  /**
   * Create a volumetric soft body from a genmesh.
   * \param genmeshFactory The genmesh factory to use.
   * \param bodyTransform The initial transform of the soft body.
   * \remark You must call SetSoftBodyWorld() prior to this.
   */
  virtual iSoftBody* CreateSoftBody (iGeneralFactoryState* genmeshFactory,
				     const csOrthoTransform& bodyTransform) = 0;

  /**
   * Create a custom volumetric soft body.
   * \param vertices The vertices of the soft body. The position is absolute.
   * \param vertexCount The count of vertices of the soft body.
   * \param triangles The faces of the soft body.
   * \param triangleCount The count of faces of the soft body.
   * \remark You must call SetSoftBodyWorld() prior to this.
   */
  virtual iSoftBody* CreateSoftBody (csVector3* vertices, size_t vertexCount,
				     csTriangle* triangles, size_t triangleCount) = 0;

  /**
   * Remove the given soft body from this dynamic world and delete it.
   */
  virtual void RemoveSoftBody (iSoftBody* body) = 0;

  /**
   * Create a pivot joint and add it to the simulation.
   */
  virtual csPtr<iPivotJoint> CreatePivotJoint () = 0;

  /**
   * Remove the given pivot joint from the simulation.
   */
  virtual void RemovePivotJoint (iPivotJoint* joint) = 0;

  /**
   * Save the current state of the dynamic world in a .bullet serialization file.
   * \return True if the operation succeeds, false otherwise.
   */
  virtual bool SaveBulletWorld (const char* filename) = 0;

  /**
   * Create a new terrain collider and add it to the simulation. All the heights of
   * the terrain must be kept between minimumHeight and maximumHeight, even when the
   * terrain is deformed. If these values are not provided then they will be computed
   * from the current state of the cell.
   * \param heightData The height map of the terrain
   * \param gridWidth The width of the height map data
   * \param gridHeight The height of the height map data
   * \param gridSize The size of the terrain, in world units
   * \param transform The position of the terrain
   * \param minimumHeight The minimum height that will ever be contained in the height map
   * \param maximumHeight The maximum height that will ever be contained in the height map
   */
  virtual iTerrainCollider* AttachColliderTerrain (csLockedHeightData& heightData,
						   int gridWidth, int gridHeight,
						   csVector3 gridSize,
						   csOrthoTransform& transform,
						   float minimumHeight = 0,
						   float maximumHeight = 0) = 0;

  /**
   * Create a new terrain collider and add it to the simulation. All the heights of
   * the terrain must be kept between minimumHeight and maximumHeight, even when the
   * terrain is deformed. If these values are not provided then they will be computed
   * from the current state of the cell.
   * \param cell The terrain cell from which the collider will be created
   * \param minimumHeight The minimum height that will ever be contained in the height map
   * \param maximumHeight The maximum height that will ever be contained in the height map
   */
  virtual iTerrainCollider* AttachColliderTerrain (iTerrainCell* cell,
						   float minimumHeight = 0,
						   float maximumHeight = 0) = 0;

  /**
   * Create a new terrain collider and add it to the simulation. This will create a
   * collider for each cell of the terrain.
   *
   * All the heights of
   * the terrain must be kept between minimumHeight and maximumHeight, even when the
   * terrain is deformed. If these values are not provided then they will be computed
   * from the current state of the cell.
   * \param terrain The terrain from which the colliders will be created
   * \param minimumHeight The minimum height that will ever be contained in the height map
   * \param maximumHeight The maximum height that will ever be contained in the height map
   */
  virtual iTerrainCollider* AttachColliderTerrain (iTerrainSystem* terrain,
						   float minimumHeight = 0,
						   float maximumHeight = 0) = 0;
  /**
   * Remove the given terrain collider from the simulation.
   */
  virtual void DestroyCollider (iTerrainCollider* collider) = 0;

  /**
   * Start the profiling of the simulation. This would add an overhead to the
   * computations, but allows to display meaningful information on the behavior
   * of the simulation.
   */
  virtual void StartProfile () = 0;

  /**
   * Stop the profiling of the simulation. This would add an overhead to the
   */
  virtual void StopProfile () = 0;

  /**
   * Dump the profile information on the standard output. StartProfile() must
   * have been called before.
   * \param resetProfile Whether or not the profile data must be reset after
   * the dumping.
   */
  virtual void DumpProfile (bool resetProfile = true) = 0;
};

/**
 * This class can be implemented in order to update the position of an anchor of a
 * CS::Physics::Bullet::iSoftBody. This can be used to try to control manually the
 * position of a vertex of a soft body.
 *
 * \warning This feature uses a hack around the physical simulation of soft bodies
 * and may not always be stable. Use it at your own risk.
 * \sa CS::Physics::Bullet::iSoftBody::AnchorVertex(size_t,iAnchorAnimationControl)
 */
struct iAnchorAnimationControl : public virtual iBase
{
  SCF_INTERFACE(CS::Physics::Bullet::iAnchorAnimationControl, 1, 0, 0);

  /**
   * Return the new position of the anchor, in world coordinates.
   */
  virtual csVector3 GetAnchorPosition () const = 0;
};

/**
 * A soft body is a physical body that can be deformed by the physical
 * simulation. It can be used to simulate eg ropes, clothes or any soft
 * volumetric object.
 *
 * A soft body does not have a positional transform by itself, but the
 * position of every vertex of the body can be queried through GetVertexPosition().
 *
 * A soft body can neither be static or kinematic, it is always dynamic.
 * \sa iRigidBody CS::Physics::Bullet::iRigidBody CS::Animation::iSoftBodyAnimationControl
 * CS::Physics::Bullet::SoftBodyHelper
 */
struct iSoftBody : public iBody
{
  SCF_INTERFACE(CS::Physics::Bullet::iSoftBody, 2, 0, 2);

  /**
   * Draw the debug informations of this soft body. This has to be called
   * at each frame, and will add 2D lines on top of the rendered scene.
   */
  virtual void DebugDraw (iView* rview) = 0;

  /**
   * Set the total mass of this body.
   */
  virtual void SetMass (float mass) = 0;

  /**
   * Return the total mass of this body.
   */
  virtual float GetMass () const = 0;

  /**
   * Return the count of vertices of this soft body.
   */
  virtual size_t GetVertexCount () const = 0;

  /**
   * Return the position in world coordinates of the given vertex.
   */
  virtual csVector3 GetVertexPosition (size_t index) const = 0;

  /**
   * Anchor the given vertex to its current position. This vertex will no more move.
   */
  virtual void AnchorVertex (size_t vertexIndex) = 0;

  /**
   * Anchor the given vertex to the given rigid body. The relative position of the
   * vertex and the body will remain constant.
   */
  virtual void AnchorVertex (size_t vertexIndex, iRigidBody* body) = 0;

  /**
   * Anchor the given vertex to the given controller. The relative position of the
   * vertex and the controller will remain constant.
   */
  virtual void AnchorVertex (size_t vertexIndex, iAnchorAnimationControl* controller) = 0;

  /**
   * Update the position of the anchor of the given vertex relatively to the anchored
   * rigid body. This can be used to have a finer control of the anchor position
   * relatively to the rigid body.
   *
   * This would work only if you called AnchorVertex(size_t,iRigidBody*) before.
   * The position to be provided is in world coordinates.
   *
   * \warning The stability of the simulation can be lost if you move the position too far
   * from the previous position.
   * \sa CS::Animation::iSoftBodyAnimationControl::CreateAnimatedMeshAnchor()
   */
  virtual void UpdateAnchor (size_t vertexIndex, csVector3& position) = 0;

  /**
   * Remove the given anchor. This won't work if you anchored the vertex to a rigid body, due
   * to a limitation in the Bullet library.
   */
  virtual void RemoveAnchor (size_t vertexIndex) = 0;

  /**
   * Set the rigidity of this body. The value should be in the 0 to 1 range, with
   * 0 meaning soft and 1 meaning rigid.
   */
  virtual void SetRigidity (float rigidity) = 0;

  /**
   * Get the rigidity of this body.
   */
  virtual float GetRigidity () const = 0;

  /**
   * Set the linear velocity of the whole body.
   */
  virtual void SetLinearVelocity (csVector3 velocity) = 0;

  /**
   * Set the linear velocity of the given vertex of the body.
   */
  virtual void SetLinearVelocity (csVector3 velocity, size_t vertexIndex) = 0;

  /**
   * Get the linear velocity of the given vertex of the body.
   */
  virtual csVector3 GetLinearVelocity (size_t vertexIndex) const = 0;

  /**
   * Add a force to the whole body.
   */
  virtual void AddForce (csVector3 force) = 0;

  /**
   * Add a force at the given vertex of the body.
   */
  virtual void AddForce (csVector3 force, size_t vertexIndex) = 0;

  /**
   * Return the count of triangles of this soft body.
   */
  virtual size_t GetTriangleCount () const = 0;

  /**
   * Return the triangle with the given index.
   */
  virtual csTriangle GetTriangle (size_t index) const = 0;

  /**
   * Return the normal vector in world coordinates for the given vertex.
   */
  virtual csVector3 GetVertexNormal (size_t index) const = 0;

  /**
   * Generate bending constraints between the vertices of this soft body.
   * This can be used to make the body more rigid.
   * \param distance Maximum number of triangle edges that can exist
   * between two vertices in order to have a constraint generated for
   * this pair of vertices. Typical values are 2 or 3.
   */
  virtual void GenerateBendingConstraints (size_t distance) = 0;
};

/**
 * General helper class for CS::Physics::Bullet::iSoftBody.
 */
struct SoftBodyHelper
{
  /**
   * Create a genmesh from the given cloth soft body.
   * The genmesh will be double-sided, in order to have correct normals on both
   * sides of the cloth (ie the vertices of the soft body will be duplicated for the
   * genmesh).
   * \warning Don't forget to use doubleSided = true in
   * CS::Animation::iSoftBodyAnimationControl::SetSoftBody()
   */
  static csPtr<iMeshFactoryWrapper> CreateClothGenMeshFactory
  (iObjectRegistry* object_reg, const char* factoryName, iSoftBody* cloth)
  {
    csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);

    // Create the cloth mesh factory.
    csRef<iMeshFactoryWrapper> clothFact = engine->CreateMeshFactory
      ("crystalspace.mesh.object.genmesh", factoryName);
    if (!clothFact)
      return 0;

    csRef<iGeneralFactoryState> gmstate = scfQueryInterface<iGeneralFactoryState>
      (clothFact->GetMeshObjectFactory ());

    // Create the vertices of the genmesh
    size_t vertexCount = cloth->GetVertexCount ();
    gmstate->SetVertexCount (int (vertexCount * 2));
    csVector3* vertices = gmstate->GetVertices ();
    for (size_t i = 0; i < vertexCount; i++)
    {
      vertices[i] = cloth->GetVertexPosition (i);
      vertices[i + vertexCount] = cloth->GetVertexPosition (i);
    }

    // Create the triangles of the genmesh
    gmstate->SetTriangleCount (int (cloth->GetTriangleCount () * 2));
    csTriangle* triangles = gmstate->GetTriangles ();
    for (size_t i = 0; i < cloth->GetTriangleCount (); i++)
    {
      csTriangle triangle = cloth->GetTriangle (i);
      triangles[i * 2] = triangle;
      triangles[i * 2 + 1] = csTriangle (int (triangle[2] + vertexCount),
					 int (triangle[1] + vertexCount),
					 int (triangle[0] + vertexCount));
    }

    gmstate->CalculateNormals ();

    // Set up the texels of the genmesh
    csVector2* texels = gmstate->GetTexels ();
    csVector3* normals = gmstate->GetNormals ();
    CS::Geometry::TextureMapper* mapper = new CS::Geometry::DensityTextureMapper (1.0f);
    for (size_t i = 0; i < vertexCount * 2; i++)
      texels[i] = mapper->Map (vertices[i], normals[i], i);

    gmstate->Invalidate ();

    return csPtr<iMeshFactoryWrapper> (clothFact);
  }
};

/**
 * The physical state of a rigid body.
 */
enum BodyState
{
  STATE_STATIC = 0,     /*!< The body is static, ie this body won't move
			  anymore but dynamic objects will still collide with it. */
  STATE_DYNAMIC,        /*!< The body is dynamic, ie the motion of 
			  the body is controlled by the dynamic simulation. */
  STATE_KINEMATIC       /*!< The body is kinematic, ie the motion 
			  of the body is controlled by the animation system,
			  but it interacts with the dynamic simulation. */
};

/**
 * The Bullet implementation of iRigidBody also implements this
 * interface.
 * \sa iRigidBody CS::Physics::Bullet::iSoftBody
 */
struct iRigidBody : public iBody
{
  SCF_INTERFACE(CS::Physics::Bullet::iRigidBody, 1, 0, 1);

  /**
   * Set a body in the kinematic state, ie the motion of the body is
   * controlled by you, but it interacts with the dynamic simulation.
   * 
   * You may need to set a callback with SetKinematicCallback() to let
   * the dynamic system know how to update the transform of the body.
   * \sa SetDynamicState() iRigidBody::MakeStatic() iRigidBody::MakeDynamic()
   */
  virtual void MakeKinematic () = 0;

  /**
   * Return the current state of the body.
   */
  virtual BodyState GetDynamicState () const = 0;

  /**
   * Set the current state of the body.
   * \sa iRigidBody::MakeStatic() iRigidBody::MakeDynamic() MakeKinematic()
   */
  virtual void SetDynamicState (BodyState state) = 0;

  /**
   * Set the callback to be used to update the transform of the kinematic body.
   * If no callback are provided then the dynamic system will use a default one.
   */
  virtual void SetKinematicCallback (iKinematicCallback* callback) = 0;

  /**
   * Get the callback used to update the transform of the kinematic body.
   */
  virtual iKinematicCallback* GetKinematicCallback () = 0;

  /**
   * Set the linear dampener for this rigid body. The dampening correspond to
   * how much the movements of the objects will be reduced. It is a value
   * between 0 and 1, giving the ratio of speed that will be reduced
   * in one second. 0 means that the movement will not be reduced, while
   * 1 means that the object will not move.
   * The default value is 0.
   * \sa iDynamicSystem::SetLinearDampener()
   */
  virtual void SetLinearDampener (float d) = 0;

  /**
   * Get the linear dampener for this rigid body.
   */
  virtual float GetLinearDampener () const = 0;

  /**
   * Set the angular dampener for this rigid body. The dampening correspond to
   * how much the movements of the objects will be reduced. It is a value
   * between 0 and 1, giving the ratio of speed that will be reduced
   * in one second. 0 means that the movement will not be reduced, while
   * 1 means that the object will not move.
   * The default value is 0.
   * \sa iDynamicSystem::SetRollingDampener()
   */
  virtual void SetRollingDampener (float d) = 0;

  /**
   * Get the angular dampener for this rigid body.
   */
  virtual float GetRollingDampener () const = 0;
};

/**
 * A callback to be implemented when you are using kinematic bodies. If no
 * callback are provided then the dynamic system will use a default one which
 * will update the transform of the body from the position of the attached
 * mesh, body or camera (see iRigidBody::AttachMesh(),
 * iRigidBody::AttachLight(), iRigidBody::AttachCamera()).
 * \sa CS::Physics::Bullet::iRigidBody::SetKinematicCallback()
 */
struct iKinematicCallback : public virtual iBase
{
  SCF_INTERFACE (CS::Physics::Bullet::iKinematicCallback, 1, 0, 0);

  /**
   * Update the new transform of the rigid body.
   */
  virtual void GetBodyTransform (::iRigidBody* body,
				 csOrthoTransform& transform) const = 0;
};

/**
 * A joint to attach to a rigid body in order to manipulate it. It is contrained
 * in translation and has free rotation. You can move freely the position of the
 * joint, the body will keep attached to the joint.
 */
struct iPivotJoint : public virtual iBase
{
  SCF_INTERFACE (CS::Physics::Bullet::iPivotJoint, 1, 0, 0);

  /**
   * Attach a rigid body to the joint.
   * \param body The rigid body to attach to the joint.
   * \param position The initial position of the joint, in world coordinates.
   */
  virtual void Attach (::iRigidBody* body, const csVector3& position) = 0;

  /**
   * Return the body attached to this joint, or 0 if there are none.
   */
  virtual iRigidBody* GetAttachedBody () const = 0;

  /**
   * Set the new position of the joint, in world coordinates.
   */
  virtual void SetPosition (const csVector3& position) = 0;

  /**
   * Get the current position of the joint, in world coordinates.
   */
  virtual csVector3 GetPosition () const = 0;
};

/**
 * A terrain collider for the dynamic simulation.
 */
struct iTerrainCollider : public iBody
{
  SCF_INTERFACE (CS::Physics::Bullet::iTerrainCollider, 1, 0, 0);

};

} //namespace Bullet
} //namespace Physics
} //namespace CS

CS_DEPRECATED_METHOD_MSG("Use CS::Physics::Bullet::iDynamicSystem instead")
typedef CS::Physics::Bullet::iDynamicSystem iBulletDynamicSystem;

#endif // __CS_IVARIA_BULLET_H__

