/*
  Copyright (C) 2011 by Liu Lu

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __CS_BULLET_PHYSICS_H__
#define __CS_BULLET_PHYSICS_H__

#include "iutil/comp.h"
#include "csutil/scf.h"
#include "csutil/nobjvec.h"
#include "csutil/scf_implementation.h"
#include "ivaria/collisions.h"
#include "ivaria/physics.h"
#include "ivaria/bullet2.h"
#include "ivaria/view.h"
#include "iengine/sector.h"
#include "iengine/movable.h"
#include "csutil/csobject.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"

struct iSector;
class btCollisionObject;
class btCompoundShape;
class btDynamicsWorld;
class btCollisionDispatcher;
class btDefaultCollisionConfiguration;
class btSequentialImpulseConstraintSolver;
class btBroadphaseInterface;
struct btSoftBodyWorldInfo;

//class btGhostObject;

CS_PLUGIN_NAMESPACE_BEGIN (Bullet2)
{
class csBulletSystem;
class csBulletSector;
class csBulletDebugDraw;
class csBulletRigidBody;
class csBulletSoftBody;
class csBulletCollisionObject;
class csBulletCollisionActor;
class csBulletCollider;
class csBulletJoint;

struct CollisionPortal
{
  csOrthoTransform warpTrans;
  csRefArray<csBulletCollisionObject> objects;
  csArray<csOrthoTransform> oldTrans;
  iPortal* portal;
  csBulletSector*  desSector;
  btGhostObject* ghostPortal;

  CollisionPortal (iPortal* portal) : portal (portal), desSector (NULL), ghostPortal (NULL) {}
  ~CollisionPortal ();
  void AddObject (csRef<csBulletCollisionObject> object) {objects.Push (object);}
};

//Will also implement iPhysicalSector...
class csBulletSector : public scfImplementationExt3<
  csBulletSector, csObject, CS::Collision2::iCollisionSector, 
  CS::Physics2::Bullet2::iPhysicalSector,
  CS::Physics2::iPhysicalSector>
{
  friend class csBulletCollisionObject;
  friend class csBulletCollisionActor;
  friend class csBulletRigidBody;
  friend class csBulletSoftBody;
  friend class csBulletJoint;
  friend class csBulletColliderTerrain;
  friend class csBulletKinematicMotionState;
  friend class csBulletMotionState;
  friend class csBulletSystem;

  class CollisionGroupVector : public csArray<CS::Collision2::CollisionGroup>
  {
  public:
    CollisionGroupVector () : csArray<CS::Collision2::CollisionGroup> () {}
    static int CompareKey (CS::Collision2::CollisionGroup const& item,
      char const* const& key)
    {
      return strcmp (item.name.GetData (), key);
    }
    static csArrayCmp<CS::Collision2::CollisionGroup, char const*>
      KeyCmp(char const* k)
    {
      return csArrayCmp<CS::Collision2::CollisionGroup, char const*> (k,CompareKey);
    }
  };

  csBulletSystem* sys;

  bool isSoftWorld;
  csVector3 gravity;
  btGhostObject* hitPortal;
  csBulletDebugDraw* debugDraw;
  btDynamicsWorld* bulletWorld;
  btCollisionDispatcher* dispatcher;
  btDefaultCollisionConfiguration* configuration;
  btSequentialImpulseConstraintSolver* solver;
  btBroadphaseInterface* broadphase;
  btSoftBodyWorldInfo* softWorldInfo;

  size_t systemFilterCount;

  float linearDampening;
  float angularDampening;

  float linearDisableThreshold;
  float angularDisableThreshold;
  float timeDisableThreshold;
  float worldTimeStep;
  size_t worldMaxSteps;

  CollisionGroupVector collGroups;
  csRefArray<csBulletJoint> joints;
  csArray<CollisionPortal*> portals;
  csRefArrayObject<csBulletCollisionObject> collisionObjects;
  csRefArrayObject<csBulletRigidBody> rigidBodies;
  csRefArrayObject<csBulletSoftBody> softBodies;
  csWeakRefArray<csBulletSoftBody> anchoredSoftBodies;
  csRef<csBulletCollisionActor> collisionActor;
  csRef<iSector> sector;

  void CheckCollisions();
  void UpdateCollisionPortals ();
  void SetInformationToCopy (csBulletCollisionObject* obj, csBulletCollisionObject* cpy,
    const csOrthoTransform& warpTrans);
  void GetInformationFromCopy (csBulletCollisionObject* obj, csBulletCollisionObject* cpy);

public:
  csBulletSector (csBulletSystem* sys);
  virtual ~csBulletSector ();

  virtual iObject* QueryObject () {return (iObject*) this;}
  //iCollisionSector
  virtual void SetGravity (const csVector3& v);
  virtual csVector3 GetGravity () const {return gravity;}

  virtual void AddCollisionObject(CS::Collision2::iCollisionObject* object);
  virtual void RemoveCollisionObject(CS::Collision2::iCollisionObject* object);

  virtual size_t GetCollisionObjectCount () {return collisionObjects.GetSize ();}
  virtual CS::Collision2::iCollisionObject* GetCollisionObject (size_t index);
  virtual CS::Collision2::iCollisionObject* FindCollisionObject (const char* name);

  virtual void AddPortal(iPortal* portal, const csOrthoTransform& meshTrans);
  virtual void RemovePortal(iPortal* portal);

  virtual void SetSector(iSector* sector);
  virtual iSector* GetSector(){return sector;}

  virtual CS::Collision2::HitBeamResult HitBeam(const csVector3& start, 
    const csVector3& end);

  virtual CS::Collision2::HitBeamResult HitBeamPortal(const csVector3& start, 
    const csVector3& end);

  virtual CS::Collision2::CollisionGroup& CreateCollisionGroup (const char* name);
  virtual CS::Collision2::CollisionGroup& FindCollisionGroup (const char* name);

  virtual void SetGroupCollision (const char* name1,
    const char* name2, bool collide);
  virtual bool GetGroupCollision (const char* name1,
    const char* name2);

  virtual bool CollisionTest(CS::Collision2::iCollisionObject* object, 
    csArray<CS::Collision2::CollisionData>& collisions);

  virtual void AddCollisionActor (CS::Collision2::iCollisionActor* actor);
  virtual void RemoveCollisionActor ();
  virtual CS::Collision2::iCollisionActor* GetCollisionActor ();

  /*virtual MoveResult MoveTest (iCollisionObject* object,
    const csOrthoTransform& fromWorld, const csOrthoTransform& toWorld);*/

  //iPhysicalSector
  virtual void SetSimulationSpeed (float speed);
  virtual void SetStepParameters (float timeStep,
    size_t maxSteps, size_t iterations);
  virtual void Step (float duration);

  virtual void SetLinearDampener (float d);
  virtual float GetLinearDampener () const {return linearDampening;}

  virtual void SetRollingDampener (float d);
  virtual float GetRollingDampener () const {return angularDampening;}

  virtual void SetAutoDisableParams (float linear,
    float angular, float time);

  virtual void AddRigidBody (CS::Physics2::iRigidBody* body);
  virtual void RemoveRigidBody (CS::Physics2::iRigidBody* body);

  virtual size_t GetRigidBodyCount () {return rigidBodies.GetSize ();}
  virtual CS::Physics2::iRigidBody* GetRigidBody (size_t index);
  virtual CS::Physics2::iRigidBody* FindRigidBody (const char* name);

  virtual void AddSoftBody (CS::Physics2::iSoftBody* body);
  virtual void RemoveSoftBody (CS::Physics2::iSoftBody* body);

  virtual size_t GetSoftBodyCount () {return softBodies.GetSize ();}
  virtual CS::Physics2::iSoftBody* GetSoftBody (size_t index);
  virtual CS::Physics2::iSoftBody* FindSoftBody (const char* name);

  virtual void AddJoint (CS::Physics2::iJoint* joint);
  virtual void RemoveJoint (CS::Physics2::iJoint* joint);

  virtual void SetSoftBodyEnabled (bool enabled);
  virtual bool GetSoftBodyEnabled () {return isSoftWorld;}

  //Bullet::iPhysicalSector
  //Currently will not use gimpact shape...
  //virtual void SetGimpactEnabled (bool enabled);
  //virtual bool GetGimpactEnabled ();

  virtual bool SaveWorld (const char* filename);

  virtual void DebugDraw (iView* rview);
  virtual void SetDebugMode (CS::Physics2::Bullet2::DebugMode mode);
  virtual CS::Physics2::Bullet2::DebugMode GetDebugMode ();

  virtual void StartProfile ();

  virtual void StopProfile ();

  virtual void DumpProfile (bool resetProfile = true);

  bool BulletCollide (btCollisionObject* objectA,
    btCollisionObject* objectB,
    csArray<CS::Collision2::CollisionData>& data);

  CS::Collision2::HitBeamResult RigidHitBeam(btCollisionObject* object, 
			     const csVector3& start,
			     const csVector3& end);

  void UpdateSoftBodies (float timeStep);

  void AddMovableToSector (CS::Collision2::iCollisionObject* obj);

  void RemoveMovableFromSector (CS::Collision2::iCollisionObject* obj);
};

class csBulletSystem : public scfImplementation3<
  csBulletSystem, CS::Collision2::iCollisionSystem, 
  CS::Physics2::iPhysicalSystem, iComponent>
{
  friend class csBulletColliderConvexMesh;
  friend class csBulletColliderConcaveMesh;
  friend class csBulletSector;
private:
  iObjectRegistry* object_reg;
  /*csRefArrayObject<CS::Collision2::iCollider> colliders;
  csRefArrayObject<csBulletCollisionObject> objects;
  csRefArrayObject<csBulletRigidBody> rigidBodies;
  csRefArrayObject<csBulletSoftBody> softBodies;
  csRefArrayObject<csBulletJoint> joints;
  csRefArrayObject<iCollisionActor> actors;*/
  csRefArrayObject<csBulletSector> collSectors;
  btSoftBodyWorldInfo* defaultInfo;
  float internalScale;
  float inverseInternalScale;
  csStringID baseID;
  csStringID colldetID;

public:
  csBulletSystem (iBase* iParent);
  virtual ~csBulletSystem ();

  // iComponent
  virtual bool Initialize (iObjectRegistry* object_reg);

  // iCollisionSystem
  virtual void SetInternalScale (float scale);
  virtual csRef<CS::Collision2::iColliderConvexMesh> CreateColliderConvexMesh (
    iMeshWrapper* mesh, bool simplify = false);
  virtual csRef<CS::Collision2::iColliderConcaveMesh> CreateColliderConcaveMesh (iMeshWrapper* mesh);
  virtual csRef<CS::Collision2::iColliderConcaveMeshScaled> CreateColliderConcaveMeshScaled
      (CS::Collision2::iColliderConcaveMesh* collider, csVector3 scale);
  virtual csRef<CS::Collision2::iColliderCylinder> CreateColliderCylinder (float length, float radius);
  virtual csRef<CS::Collision2::iColliderBox> CreateColliderBox (const csVector3& size);
  virtual csRef<CS::Collision2::iColliderSphere> CreateColliderSphere (float radius);
  virtual csRef<CS::Collision2::iColliderCapsule> CreateColliderCapsule (float length, float radius);
  virtual csRef<CS::Collision2::iColliderCone> CreateColliderCone (float length, float radius);
  virtual csRef<CS::Collision2::iColliderPlane> CreateColliderPlane (const csPlane3& plane);
  virtual csRef<CS::Collision2::iColliderTerrain> CreateColliderTerrain (iTerrainSystem* terrain,
      float minHeight = 0, float maxHeight = 0);

  virtual csRef<CS::Collision2::iCollisionObject> CreateCollisionObject ();
  virtual csRef<CS::Collision2::iCollisionActor> CreateCollisionActor ();
  virtual csRef<CS::Collision2::iCollisionSector> CreateCollisionSector ();
  virtual CS::Collision2::iCollisionSector* FindCollisionSector (const char* name);

  virtual void DecomposeConcaveMesh (CS::Collision2::iCollisionObject* object,
    iMeshWrapper* mesh, bool simplify = false); 

  //iPhysicalSystem
  virtual csRef<CS::Physics2::iRigidBody> CreateRigidBody ();

  virtual csRef<CS::Physics2::iJoint> CreateJoint ();
  virtual csRef<CS::Physics2::iJoint> CreateRigidP2PJoint (const csVector3 position);
  virtual csRef<CS::Physics2::iJoint> CreateRigidSlideJoint (const csOrthoTransform trans,
    float minDist, float maxDist, float minAngle, float maxAngle, int axis);
  virtual csRef<CS::Physics2::iJoint> CreateRigidHingeJoint (const csVector3 position,
    float minAngle, float maxAngle, int axis);
  virtual csRef<CS::Physics2::iJoint> CreateRigidConeTwistJoint (const csOrthoTransform trans,
    float swingSpan1,float swingSpan2,float twistSpan);
  virtual csRef<CS::Physics2::iJoint> CreateSoftLinearJoint (const csVector3 position);
  virtual csRef<CS::Physics2::iJoint> CreateSoftAngularJoint (int axis);
  virtual csRef<CS::Physics2::iJoint> CreateRigidPivotJoint (CS::Physics2::iRigidBody* body, const csVector3 position);
 
  virtual csRef<CS::Physics2::iSoftBody> CreateRope (csVector3 start,
      csVector3 end, size_t segmentCount);
  virtual csRef<CS::Physics2::iSoftBody> CreateRope (csVector3* vertices, size_t vertexCount);
  virtual csRef<CS::Physics2::iSoftBody> CreateCloth (csVector3 corner1, csVector3 corner2,
      csVector3 corner3, csVector3 corner4,
      size_t segmentCount1, size_t segmentCount2,
      bool withDiagonals = false);

  virtual csRef<CS::Physics2::iSoftBody> CreateSoftBody (iGeneralFactoryState* genmeshFactory, 
    const csOrthoTransform& bodyTransform);

  virtual csRef<CS::Physics2::iSoftBody> CreateSoftBody (csVector3* vertices,
      size_t vertexCount, csTriangle* triangles, size_t triangleCount,
      const csOrthoTransform& bodyTransform);
  float getInverseInternalScale() {return inverseInternalScale;}
  float getInternalScale() {return internalScale;}
};
}
CS_PLUGIN_NAMESPACE_END(Bullet2)

#endif
