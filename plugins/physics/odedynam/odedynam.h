/*
    Copyright (C) 2002 Anders Stenberg
    Copyright (C) 2003 Leandro Motta Barros

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


#ifndef __CS_ODEDYNAMICS_H__
#define __CS_ODEDYNAMICS_H__

#include "csgeom/transfrm.h"
#include "csgeom/vector3.h"
#include "csutil/dirtyaccessarray.h"
#include "csutil/nobjvec.h"
#include "csutil/refarr.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "iutil/comp.h"
#include "iutil/eventh.h"
#include "ivaria/dynamics.h"
#include "ivaria/ode.h"

// Avoid conflicts as ODE defines the same types.
#define int8 ode_int8
#define uint8 ode_uint8
#define int32 ode_int32
#define uint32 ode_uint32
#include <ode/ode.h>
#undef uint32
#undef int32
#undef uint8
#undef int8

struct iMeshWrapper;
struct iODEFrameUpdateCallback;
struct iObjectRegistry;
struct iVirtualClock;

////////////////////////////////////////////
// NOTE
// There is lots of unused stuff in here involving iCollideSystem
// It was put there when I built the base for making mesh colliders
// The reason I left it in is because it does quite a lot of useful
// stuff conserning custom colliders in ODE
///////////////////////////////////////////



struct colliderdata
{
public:
  csRef<iCollideSystem> collsys;
  csRef<iCollider> collider;
  float friction;
  dReal aabb[6];

  colliderdata (iCollideSystem* cs, iCollider* c, float f )
  {
    collsys=cs; collider=c; friction=f;
    aabb[0]=-dInfinity; aabb[1]=dInfinity;
    aabb[2]=-dInfinity; aabb[3]=dInfinity;
    aabb[4]=-dInfinity; aabb[5]=dInfinity;
  }
  ~colliderdata()
  {
  }
};

typedef csDirtyAccessArray<dGeomID> csGeomList;

struct ContactEntry
{
  dGeomID o1, o2;
  csVector3 point;
  csVector3 dir;
};

struct MeshInfo
{
  iMeshWrapper* mesh;
};

typedef csDirtyAccessArray<ContactEntry> csContactList;

/**
 * This is the implementation for the actual plugin.
 * It is responsible for creating iDynamicSystem.
 */
class csODEDynamics : public iDynamics
{
private:
  iObjectRegistry* object_reg;
  csRef<iVirtualClock> clock;

  bool process_events;

  static int geomclassnum;
  static dJointGroupID contactjoints;

  csRefArrayObject<iDynamicSystem> systems;
  float erp, cfm;

  bool rateenabled;
  float steptime, limittime;
  float total_elapsed;
  csRefArrayObject<iODEFrameUpdateCallback> updates;

  bool stepfast;
  int sfiter;
  bool quickstep;
  int qsiter;
  bool fastobjects;

public:
  SCF_DECLARE_IBASE;

  csODEDynamics (iBase* parent);
  virtual ~csODEDynamics ();
  bool Initialize (iObjectRegistry* object_reg);

  static int GetGeomClassNum() { return geomclassnum; }

  virtual csPtr<iDynamicSystem> CreateSystem ();
  virtual void RemoveSystem (iDynamicSystem* system);
  virtual iDynamicSystem* FindSystem (const char *name);

  virtual void Step (float stepsize);

  static void NearCallback (void *data, dGeomID o1, dGeomID o2);
  static int CollideMeshMesh (dGeomID mesh1, dGeomID mesh2, int flags,
        dContactGeom *contact, int skip);
  static int CollideMeshBox (dGeomID mesh, dGeomID box, int flags,
        dContactGeom *contact, int skip);
  static int CollideMeshCylinder (dGeomID mesh, dGeomID cyl, int flags,
        dContactGeom *contact, int skip);
  static int CollideMeshSphere (dGeomID mesh, dGeomID sphere, int flags,
        dContactGeom *contact, int skip);
  static int CollideMeshPlane (dGeomID mesh, dGeomID plane, int flags,
        dContactGeom *contact, int skip);
//  static dColliderFn* CollideSelector (int num);
  static void GetAABB (dGeomID g, dReal aabb[6]);

  void SetGlobalERP (float erp);
  float GlobalERP () { return erp; }
  void SetGlobalCFM (float cfm);
  float GlobalCFM () { return cfm; }

  void EnableStepFast (bool enable);
  bool StepFastEnabled () { return stepfast; }
  void SetStepFastIterations (int iter);
  int StepFastIterations () { return sfiter; }
  void EnableQuickStep (bool enable);
  bool QuickStepEnabled () { return quickstep; };
  void SetQuickStepIterations (int iter);
  int QuickStepIterations () { return qsiter; }

  void EnableFrameRate (bool enable) { rateenabled = enable; }
  bool FrameRateEnabled () { return rateenabled; }
  void SetFrameRate (float hz) { steptime = 1.0 / hz; }
  float FrameRate () { return 1.0 / steptime; }
  void SetFrameLimit (float hz) { limittime = 1.0 / hz; }
  float FrameLimit () { return 1.0 / limittime; }
  void AddFrameUpdateCallback (iODEFrameUpdateCallback *cb) { updates.Push (cb); }
  void RemoveFrameUpdateCallback (iODEFrameUpdateCallback *cb) { updates.Delete (cb); }
  void EnableEventProcessing (bool enable);
  bool EventProcessingEnabled () { return process_events; }
  void EnableFastObjects (bool enable) { fastobjects = enable; }
  bool FastObjectsEnabled () { return false; }

  struct ODEDynamicState : public iODEDynamicState
  {
    SCF_DECLARE_EMBEDDED_IBASE (csODEDynamics);
    void SetGlobalERP (float erp)
    { scfParent->SetGlobalERP (erp); }
    float GlobalERP ()
    { return scfParent->GlobalERP (); }
    void SetGlobalCFM (float cfm)
    { scfParent->SetGlobalCFM (cfm); }
    float GlobalCFM ()
    { return scfParent->GlobalCFM (); }
    void EnableStepFast (bool enable)
    { scfParent->EnableStepFast (enable); }
    bool StepFastEnabled ()
    { return scfParent->StepFastEnabled (); }
    void SetStepFastIterations (int iter)
    { scfParent->SetStepFastIterations (iter); }
    int StepFastIterations ()
    { return scfParent->StepFastIterations (); }
    void EnableQuickStep (bool enable)
    { scfParent->EnableQuickStep (enable); }
    bool QuickStepEnabled ()
    { return scfParent->QuickStepEnabled (); }
    void SetQuickStepIterations (int iter)
    { scfParent->SetQuickStepIterations (iter); }
    int QuickStepIterations ()
    { return scfParent->QuickStepIterations (); }
    void EnableFrameRate (bool enable)
    { scfParent->EnableFrameRate (enable); }
    bool FrameRateEnabled ()
    { return scfParent->FrameRateEnabled (); }
    void SetFrameRate (float hz)
    { scfParent->SetFrameRate (hz); }
    float FrameRate ()
    { return scfParent->FrameRate (); }
    void SetFrameLimit (float hz)
    { scfParent->SetFrameLimit (hz); }
    float FrameLimit ()
    { return scfParent->FrameLimit (); }
    void AddFrameUpdateCallback (iODEFrameUpdateCallback *cb)
    { scfParent->AddFrameUpdateCallback (cb); }
    void RemoveFrameUpdateCallback (iODEFrameUpdateCallback *cb)
    { scfParent->RemoveFrameUpdateCallback (cb); }
    void EnableEventProcessing (bool enable)
    { scfParent->EnableEventProcessing (enable); }
    bool EventProcessingEnabled ()
    { return scfParent->EventProcessingEnabled (); }
    void EnableFastObjects (bool enable)
    { scfParent->EnableFastObjects (enable); }
    bool FastObjectsEnabled ()
    { return scfParent->FastObjectsEnabled (); }
  } scfiODEDynamicState;

  struct Component : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE (csODEDynamics);
    virtual bool Initialize (iObjectRegistry* object_reg)
    {
      return scfParent->Initialize (object_reg);
    }
  } scfiComponent;

  bool HandleEvent (iEvent& Event);
  struct EventHandler : public iEventHandler
  {
  private:
    csODEDynamics* parent;
  public:
    SCF_DECLARE_IBASE;
    EventHandler (csODEDynamics* parent)
    {
      SCF_CONSTRUCT_IBASE (0);
      EventHandler::parent = parent;
    }
    virtual ~EventHandler ()
    { SCF_DESTRUCT_IBASE (); }
    virtual bool HandleEvent (iEvent& ev)
    { return parent->HandleEvent (ev); }
  };
  csRef<EventHandler> scfiEventHandler;
};

class csODEBodyGroup;
class csODEJoint;
class csStrictODEJoint;

/**
 * This is the implementation for the dynamics core.
 * It handles all bookkeeping for rigid bodies and joints.
 * It also handles collision response.
 * Collision detection is done in another plugin.
 */
class csODEDynamicSystem : public csObject
{
private:
  dWorldID worldID;
  dSpaceID spaceID;
  float roll_damp, lin_damp;

  csRef<iCollideSystem> collidesys;
  csRef<iDynamicsMoveCallback> move_cb;

  csRefArrayObject<iRigidBody> bodies;
  csRefArray<csODEBodyGroup> groups;
  csRefArray<csODEJoint> joints;
  csRefArray<csStrictODEJoint> strict_joints;
  csRefArray<iDynamicsSystemCollider> colliders; //here are all colliders not attached to bodies

  bool rateenabled;
  float steptime, limittime;
  float total_elapsed;
  csRefArrayObject<iODEFrameUpdateCallback> updates;

  bool stepfast;
  int sfiter;
  bool quickstep;
  int qsiter;
  bool fastobjects;
  bool autodisable;

public:
  SCF_DECLARE_IBASE_EXT (csObject);

  struct DynamicSystem : public iDynamicSystem {
    SCF_DECLARE_EMBEDDED_IBASE (csODEDynamicSystem);
    iObject *QueryObject ()
    { return scfParent; }
    void SetGravity (const csVector3 &v)
    { scfParent->SetGravity (v); }
    const csVector3 GetGravity () const
    { return scfParent->GetGravity (); }
    void SetLinearDampener (float d)
    { scfParent->SetLinearDampener (d); }
    float GetLinearDampener () const
    { return scfParent->GetLinearDampener (); }
    void SetRollingDampener (float d)
    { scfParent->SetRollingDampener (d); }
    float GetRollingDampener () const
    { return scfParent->GetRollingDampener (); }
    void EnableAutoDisable (bool enable)
    { scfParent->EnableAutoDisable (enable); }
    bool AutoDisableEnabled ()
    { return scfParent->AutoDisableEnabled (); }
    void SetAutoDisableParams (float linear, float angular, int steps, float time)
    { scfParent->SetAutoDisableParams (linear, angular, steps, time); }
    void Step (float stepsize)
    { scfParent->Step (stepsize); }
    csPtr<iRigidBody> CreateBody ()
    { return scfParent->CreateBody (); }
    void RemoveBody (iRigidBody *body)
    { scfParent->RemoveBody (body); }
    iRigidBody *FindBody (const char *name)
    { return scfParent->FindBody (name); }
    iRigidBody *GetBody (unsigned int index)
    {return scfParent->GetBody (index);}
    int GetBodysCount ()
    {return scfParent->GetBodysCount ();}
    csPtr<iBodyGroup> CreateGroup ()
    { return scfParent->CreateGroup (); }
    void RemoveGroup (iBodyGroup *group)
    { scfParent->RemoveGroup (group); }
    csPtr<iJoint> CreateJoint ()
    { return scfParent->CreateJoint (); }
    void RemoveJoint (iJoint *joint)
    { scfParent->RemoveJoint (joint); }
    iDynamicsMoveCallback* GetDefaultMoveCallback ()
    { return scfParent->GetDefaultMoveCallback (); }
    bool AttachColliderMesh (iMeshWrapper* mesh, const csOrthoTransform& trans,
            float friction, float elasticity, float softness)
    { return scfParent->AttachColliderMesh (mesh, trans, friction, elasticity, softness); }
    bool AttachColliderCylinder (float length, float radius,
            const csOrthoTransform& trans, float friction, float elasticity, float softness)
    { return scfParent->AttachColliderCylinder (length, radius, trans, friction, elasticity, softness); }
    bool AttachColliderBox (const csVector3 &size, const csOrthoTransform&
            trans, float friction, float elasticity, float softness)
    { return scfParent->AttachColliderBox (size, trans, friction, elasticity, softness); }
    bool AttachColliderSphere (float radius, const csVector3 &offset,
      float friction, float elasticity, float softness)
    { return scfParent->AttachColliderSphere (radius, offset, friction, elasticity, softness); }
    bool AttachColliderPlane (const csPlane3 &plane, float friction,
      float elasticity, float softness)
    { return scfParent->AttachColliderPlane (plane, friction, elasticity, softness); }
    csRef<iDynamicsSystemCollider> GetCollider (unsigned int index)
    {return scfParent->GetCollider (index);}
    int GetColliderCount ()
    {return scfParent->GetColliderCount ();}
    csRef<iDynamicsSystemCollider> CreateCollider ()
    {return scfParent->CreateCollider ();}
    void DestroyColliders ()
    {scfParent->DestroyColliders ();}
    void DestroyCollider (iDynamicsSystemCollider* collider) 
    {scfParent->DestroyCollider (collider);}
    void AttachCollider (iDynamicsSystemCollider* collider)
    {scfParent->AttachCollider (collider);}
  } scfiDynamicSystem;
  friend struct DynamicSystem;

  void SetERP (float erp) { dWorldSetERP (worldID, erp); }
  float ERP () { return dWorldGetERP (worldID); }
  void SetCFM (float cfm) { dWorldSetCFM (worldID, cfm); }
  float CFM () { return dWorldGetCFM (worldID); }
  void EnableStepFast (bool enable) { stepfast = enable; quickstep = false; };
  bool StepFastEnabled () { return stepfast; }
  void SetStepFastIterations (int iter) { sfiter = iter; }
  int StepFastIterations () { return sfiter; }
  void EnableQuickStep (bool enable) { quickstep = enable; stepfast = false; };
  bool QuickStepEnabled () { return quickstep; };
  void SetQuickStepIterations (int iter) { qsiter = iter; };
  int QuickStepIterations () { return qsiter; }
  void EnableFrameRate (bool enable) { rateenabled = enable; }
  bool FrameRateEnabled () { return rateenabled; }
  void SetFrameRate (float hz) { steptime = 1.0 / hz; }
  float FrameRate () { return 1.0 / steptime; }
  void SetFrameLimit (float hz) { limittime = 1.0 / hz; }
  float FrameLimit () { return 1.0 / limittime; }
  void AddFrameUpdateCallback (iODEFrameUpdateCallback *cb) { updates.Push (cb); }
  void RemoveFrameUpdateCallback (iODEFrameUpdateCallback *cb) { updates.Delete (cb); }
  void EnableFastObjects (bool enable) { fastobjects = enable; }
  bool FastObjectsEnabled () { return false; }
  void EnableAutoDisable (bool enable);
  bool AutoDisableEnabled () { return autodisable; }
  void SetAutoDisableParams (float linear, float angular, int steps, float time);
  void SetContactMaxCorrectingVel (float v);
  float GetContactMaxCorrectingVel ();
  void SetContactSurfaceLayer (float depth);
  float GetContactSurfaceLayer ();

  struct ODEDynamicSystemState : public iODEDynamicSystemState
  {
    SCF_DECLARE_EMBEDDED_IBASE (csODEDynamicSystem);
    void SetERP (float erp)
    { scfParent->SetERP (erp); }
    float ERP ()
    { return scfParent->ERP (); }
    void SetCFM (float cfm)
    { scfParent->SetCFM (cfm); }
    float CFM ()
    { return scfParent->CFM (); }
    void EnableStepFast (bool enable)
    { scfParent->EnableStepFast (enable); }
    bool StepFastEnabled ()
    { return scfParent->StepFastEnabled (); }
    void SetStepFastIterations (int iter)
    { scfParent->SetStepFastIterations (iter); }
    int StepFastIterations ()
    { return scfParent->StepFastIterations (); }
    void EnableQuickStep (bool enable)
    { scfParent->EnableQuickStep (enable); }
    bool QuickStepEnabled ()
    { return scfParent->QuickStepEnabled (); }
    void SetQuickStepIterations (int iter)
    { scfParent->SetQuickStepIterations (iter); }
    int QuickStepIterations ()
    { return scfParent->QuickStepIterations (); }
    void EnableFrameRate (bool enable)
    { scfParent->EnableFrameRate (enable); }
    bool FrameRateEnabled ()
    { return scfParent->FrameRateEnabled (); }
    void SetFrameRate (float hz)
    { scfParent->SetFrameRate (hz); }
    float FrameRate ()
    { return scfParent->FrameRate (); }
    void SetFrameLimit (float hz)
    { scfParent->SetFrameLimit (hz); }
    float FrameLimit ()
    { return scfParent->FrameLimit (); }
    void AddFrameUpdateCallback (iODEFrameUpdateCallback *cb)
    { scfParent->AddFrameUpdateCallback (cb); }
    void RemoveFrameUpdateCallback (iODEFrameUpdateCallback *cb)
    { scfParent->RemoveFrameUpdateCallback (cb); }
    void EnableFastObjects (bool enable)
    { scfParent->EnableFastObjects (enable); }
    bool FastObjectsEnabled ()
    { return scfParent->FastObjectsEnabled (); }
    void EnableAutoDisable (bool enable)
    { scfParent->EnableAutoDisable (enable); }
    bool AutoDisableEnabled ()
    { return scfParent->AutoDisableEnabled (); }
    void SetAutoDisableParams (float linear, float angular, int steps, float time)
    { scfParent->SetAutoDisableParams (linear, angular, steps, time); }
    csPtr<iODEBallJoint> CreateBallJoint ()
    { return scfParent->CreateBallJoint (); }
    csPtr<iODEUniversalJoint> CreateUniversalJoint ()
    { return scfParent->CreateUniversalJoint (); }
    csPtr<iODEAMotorJoint> CreateAMotorJoint ()
    { return scfParent->CreateAMotorJoint (); }
    csPtr<iODEHingeJoint> CreateHingeJoint ()
    { return scfParent->CreateHingeJoint (); }
    csPtr<iODESliderJoint> CreateSliderJoint ()
    { return scfParent->CreateSliderJoint (); }
    void RemoveJoint (iODEBallJoint *joint)
    { scfParent->RemoveJoint (joint); }
    void RemoveJoint (iODEAMotorJoint *joint)
    { scfParent->RemoveJoint (joint); }
    void RemoveJoint (iODEHingeJoint *joint)
    { scfParent->RemoveJoint (joint); }
    void RemoveJoint (iODEUniversalJoint *joint)
    { scfParent->RemoveJoint (joint); }
    void RemoveJoint (iODESliderJoint *joint)
    { scfParent->RemoveJoint (joint); }
    void SetContactMaxCorrectingVel (float v)
    { scfParent->SetContactMaxCorrectingVel(v); }
    float GetContactMaxCorrectingVel ()
    { return scfParent->GetContactMaxCorrectingVel(); }
    void SetContactSurfaceLayer (float depth)
    { scfParent->SetContactSurfaceLayer(depth); }
    float GetContactSurfaceLayer ()
    { return scfParent->GetContactSurfaceLayer(); }
  } scfiODEDynamicSystemState;


  csODEDynamicSystem (float erp, float cfm);
  virtual ~csODEDynamicSystem ();

  dWorldID GetWorldID() { return worldID; }
  dSpaceID GetSpaceID() { return spaceID; }
  iCollideSystem* GetCollideSystem() { return collidesys; }

  virtual void SetGravity (const csVector3& v);
  virtual const csVector3 GetGravity () const;
  virtual void SetRollingDampener (float d) { roll_damp = (d > 1) ? 1.0 : d; }
  virtual float GetRollingDampener () const { return roll_damp; }
  virtual void SetLinearDampener (float d) { lin_damp = (d > 1) ? 1.0 : d; }
  virtual float GetLinearDampener () const { return lin_damp; }

  virtual void Step (float stepsize);

  virtual csPtr<iRigidBody> CreateBody ();
  virtual void RemoveBody (iRigidBody* body);
  virtual iRigidBody *FindBody (const char *name);
  iRigidBody *GetBody (unsigned int index);
  int GetBodysCount () {return (int)bodies.GetSize();};

  virtual csPtr<iBodyGroup> CreateGroup ();
  virtual void RemoveGroup (iBodyGroup *group);

  virtual csPtr<iJoint> CreateJoint ();
  virtual csPtr<iODEBallJoint> CreateBallJoint ();
  virtual csPtr<iODEHingeJoint> CreateHingeJoint ();
  virtual csPtr<iODEAMotorJoint> CreateAMotorJoint ();
  virtual csPtr<iODEUniversalJoint> CreateUniversalJoint ();
  virtual csPtr<iODESliderJoint> CreateSliderJoint ();

  virtual void RemoveJoint (iJoint* joint);
  virtual void RemoveJoint (iODEBallJoint* joint);
  virtual void RemoveJoint (iODEAMotorJoint* joint);
  virtual void RemoveJoint (iODEHingeJoint* joint);
  virtual void RemoveJoint (iODEUniversalJoint* joint);
  virtual void RemoveJoint (iODESliderJoint* joint);

  virtual iDynamicsMoveCallback* GetDefaultMoveCallback () { return move_cb; }

  virtual bool AttachColliderMesh (iMeshWrapper* mesh,
        const csOrthoTransform& trans, float friction, float elasticity, float softness);
  virtual bool AttachColliderCylinder (float length, float radius,
        const csOrthoTransform& trans, float friction, float elasticity, float softness);
  virtual bool AttachColliderBox (const csVector3 &size,
        const csOrthoTransform& trans, float friction, float elasticity, float softness);
  virtual bool AttachColliderSphere (float radius, const csVector3 &offset,
        float friction, float elasticity, float softness);
  virtual bool AttachColliderPlane (const csPlane3 &plane, float friction,
    float elasticity, float softness);
  csRef<iDynamicsSystemCollider> GetCollider (unsigned int index);
  int GetColliderCount () {return (int)colliders.GetSize ();};
  csRef<iDynamicsSystemCollider> CreateCollider ();
  void DestroyColliders () {colliders.DeleteAll ();};
  void DestroyCollider (iDynamicsSystemCollider* collider) {colliders.Delete (collider);};
  void AttachCollider (iDynamicsSystemCollider* collider);
};

class csODERigidBody;

/**
 * odedynam implementation of iBodyGroup.  This will set a
 * variable inside the body which will be compared against
 * inside NearCallback
 */
class csODEBodyGroup : public iBodyGroup
{
  csRefArray<iRigidBody> bodies;

  csODEDynamicSystem* system;

public:
  SCF_DECLARE_IBASE;

  csODEBodyGroup (csODEDynamicSystem *sys);
  virtual ~csODEBodyGroup ();

  void AddBody (iRigidBody *body);
  void RemoveBody (iRigidBody *body);
  bool BodyInGroup (iRigidBody *body);
};
class csODECollider : public iDynamicsSystemCollider
{
  csColliderGeometryType geom_type;
  dGeomID geomID;
  dGeomID transformID;
  dSpaceID spaceID; 
  float density;
  csRef<iDynamicsColliderCollisionCallback> coll_cb;
  float surfacedata[3];

public:

  SCF_DECLARE_IBASE;

  csODECollider ();
  virtual ~csODECollider (); 
  
  bool CreateSphereGeometry (const csSphere& sphere);
  bool CreatePlaneGeometry (const csPlane3& plane);
  bool CreateMeshGeometry (iMeshWrapper *mesh);
  bool CreateBoxGeometry (const csVector3& box_size);
  bool CreateCCylinderGeometry (float length, float radius);

  void SetCollisionCallback (iDynamicsColliderCollisionCallback* cb);
  void Collision (csODECollider* other);
  void Collision (iRigidBody* other);
  void SetFriction (float friction) {surfacedata[0] = friction;};
  void SetSoftness (float softness) {surfacedata[2] = softness;};
  void SetElasticity (float elasticity) {surfacedata[1] = elasticity;};
  void SetDensity (float density) {csODECollider::density = density;}
  
  float GetFriction () {return surfacedata[0];};
  float GetSoftness () {return surfacedata[1];};
  float GetElasticity () {return surfacedata[2];};
  float GetDensity () {return density;};

  void FillWithColliderGeometry (csRef<iGeneralFactoryState> genmesh_fact);

  csOrthoTransform GetTransform ();
  void SetTransform (const csOrthoTransform& trans);

  csColliderGeometryType GetGeometryType () {return geom_type;};

  void AttachBody (dBodyID bodyID);
  void AddTransformToSpace (dSpaceID spaceID);
  void AddToSpace (dSpaceID spaceID);

private:

  inline void CS2ODEMatrix (const csMatrix3& csmat, dMatrix3& odemat);
  inline void ODE2CSMatrix (const dReal* odemat, csMatrix3& csmat);
  void MassCorrection ();
  void ClearContents ();
  void KillGeoms ();

};
struct GeomData 
{
  float *surfacedata;
  csODECollider *collider;
  ~GeomData () 
  {if (surfacedata) delete [] surfacedata; if (collider) collider->DecRef ();}
};

/**
 * This is the implementation for a rigid body.
 * It keeps all properties for the body.
 * It can also be attached to a movable,
 * to automatically update it.
 */
class csODERigidBody : public csObject
{
private:
  dBodyID bodyID;
  dSpaceID groupID;
  dJointID statjoint;
  csRefArray<iDynamicsSystemCollider> colliders;

  /* these must be ptrs to avoid circular referencing */
  iBodyGroup* collision_group;
  csODEDynamicSystem* dynsys;

  csRef<iMeshWrapper> mesh;
  csRef<iDynamicsMoveCallback> move_cb;
  csRef<iDynamicsCollisionCallback> coll_cb;

public:

  SCF_DECLARE_IBASE_EXT (csObject);

  struct RigidBody : public iRigidBody
  {
    SCF_DECLARE_EMBEDDED_IBASE (csODERigidBody);
    iObject *QueryObject ()
    { return scfParent; }
    bool MakeStatic (void)
    { return scfParent->MakeStatic (); }
    bool MakeDynamic (void)
    { return scfParent->MakeDynamic (); }
    bool IsStatic (void)
    { return scfParent->IsStatic (); }
    bool Disable (void)
    { return scfParent->Disable (); }
    bool Enable (void)
    { return scfParent->Enable (); }
    bool IsEnabled (void)
    { return scfParent->IsEnabled (); }
    csRef<iBodyGroup> GetGroup (void)
    { return scfParent->GetGroup (); }
    bool AttachColliderMesh (iMeshWrapper* mesh, const csOrthoTransform& trans,
        float friction, float density, float elasticity, float softness)
    { return scfParent->AttachColliderMesh (mesh, trans, friction, density, elasticity, softness); }
    bool AttachColliderCylinder (float length, float radius,
        const csOrthoTransform& trans, float friction, float density, float elasticity, float softness)
    { return scfParent->AttachColliderCylinder (length, radius, trans, friction, density, elasticity, softness); }
    bool AttachColliderBox (const csVector3 &size,
        const csOrthoTransform& trans, float friction, float density, float elasticity, float softness)
    { return scfParent->AttachColliderBox (size, trans, friction, density, elasticity, softness); }
    bool AttachColliderSphere (float radius, const csVector3 &offset,
        float friction, float density, float elasticity, float softness)
    { return scfParent->AttachColliderSphere (radius, offset, friction, density, elasticity, softness); }
    bool AttachColliderPlane (const csPlane3 &plane, float friction,
    float density, float elasticity, float softness)
        { return scfParent->AttachColliderPlane (plane, friction, density, elasticity, softness); }
    void SetPosition (const csVector3& trans)
    { scfParent->SetPosition (trans); }
    const csVector3 GetPosition () const
    { return scfParent->GetPosition (); }
    void SetOrientation (const csMatrix3& trans)
    { scfParent->SetOrientation (trans); }
    const csMatrix3 GetOrientation () const
    { return scfParent->GetOrientation (); }
    void SetTransform (const csOrthoTransform& trans)
    { scfParent->SetTransform (trans); }
    const csOrthoTransform GetTransform () const
    { return scfParent->GetTransform (); }
    void SetLinearVelocity (const csVector3& vel)
    { scfParent->SetLinearVelocity (vel); }
    const csVector3 GetLinearVelocity () const
    { return scfParent->GetLinearVelocity(); }
    void SetAngularVelocity (const csVector3& vel)
    { scfParent->SetAngularVelocity (vel); }
    const csVector3 GetAngularVelocity () const
    { return scfParent->GetAngularVelocity (); }
    void SetProperties (float mass, const csVector3& center, const csMatrix3& inertia)
    { scfParent->SetProperties (mass, center, inertia); }
    void GetProperties (float* mass, csVector3* center, csMatrix3* inertia)
    { scfParent->GetProperties (mass, center, inertia); }
    float GetMass ()
    { return scfParent->GetMass (); }
    csVector3 GetCenter ()
    { return scfParent->GetCenter (); }
    csMatrix3 GetInertia ()
    { return scfParent->GetInertia (); }
    void AdjustTotalMass (float targetmass)
    { scfParent->AdjustTotalMass (targetmass); }
    void AddForce (const csVector3& force)
    { scfParent->AddForce (force); }
    void AddTorque (const csVector3& force)
    { scfParent->AddTorque (force); }
    void AddRelForce (const csVector3& force)
    { scfParent->AddRelForce (force); }
    void AddRelTorque (const csVector3& force)
    { scfParent->AddRelTorque (force); }
    void AddForceAtPos (const csVector3& force, const csVector3& pos)
    { scfParent->AddForceAtPos (force, pos); }
    void AddForceAtRelPos (const csVector3& force,
        const csVector3& pos)
    { scfParent->AddForceAtRelPos (force, pos); }
    void AddRelForceAtPos (const csVector3& force,
        const csVector3& pos)
    { scfParent->AddRelForceAtPos (force, pos); }
    void AddRelForceAtRelPos (const csVector3& force,
        const csVector3& pos)
    { scfParent->AddRelForceAtRelPos (force, pos); }
    const csVector3 GetForce () const
    { return scfParent->GetForce (); }
    const csVector3 GetTorque () const
    { return scfParent->GetTorque (); }
    void AttachMesh (iMeshWrapper* mesh)
    { scfParent->AttachMesh (mesh); }
    csRef<iMeshWrapper> GetAttachedMesh ()
    { return scfParent->GetAttachedMesh (); }
    void SetMoveCallback (iDynamicsMoveCallback* cb)
    { scfParent->SetMoveCallback (cb); }
    void SetCollisionCallback (iDynamicsCollisionCallback* cb)
    { scfParent->SetCollisionCallback (cb); }
    void Collision (iRigidBody *other)
    { scfParent->Collision (other); }
    void Update ()
    { scfParent->Update (); }
    csRef<iDynamicsSystemCollider> GetCollider (unsigned int index)
    {return scfParent->GetCollider (index);}
    int GetColliderCount () 
    {return scfParent->GetColliderCount ();}
    void AttachCollider (iDynamicsSystemCollider* collider)
    {scfParent->AttachCollider (collider);}
    void DestroyColliders ()
    {scfParent->DestroyColliders ();}
    void DestroyCollider (iDynamicsSystemCollider* collider)
    {scfParent->DestroyCollider (collider);}
  } scfiRigidBody;
  friend struct RigidBody;

  csODERigidBody (csODEDynamicSystem* sys);
  virtual ~csODERigidBody ();

  inline dBodyID GetID() { return bodyID; }

  bool MakeStatic (void);
  bool IsStatic (void) { return statjoint != 0; }
  bool MakeDynamic (void);

  bool Disable (void);
  bool Enable (void);
  bool IsEnabled (void);

  void SetGroup (iBodyGroup *group);
  void UnsetGroup () { collision_group = 0; }
  csRef<iBodyGroup> GetGroup (void) { return collision_group; }

  bool AttachColliderMesh (iMeshWrapper* mesh,
        const csOrthoTransform& trans, float friction, float density,
        float elasticity, float softness);
  bool AttachColliderCylinder (float length, float radius,
        const csOrthoTransform& trans, float friction, float density,
        float elasticity, float softness);
  bool AttachColliderBox (const csVector3 &size,
        const csOrthoTransform& trans, float friction, float density,
        float elasticity, float softness);
  bool AttachColliderSphere (float radius, const csVector3 &offset,
        float friction, float density, float elasticity, float softness);
  /// ODE planes are globally transformed, immobile, infinitely dense
  bool AttachColliderPlane (const csPlane3 &plane, float friction,
    float density, float elasticity, float softness);
  void AttachCollider (iDynamicsSystemCollider* collider);
  void DestroyColliders () {colliders.DeleteAll ();};
  void DestroyCollider (iDynamicsSystemCollider* collider) {colliders.Delete (collider);};

  void SetPosition (const csVector3& trans);
  const csVector3 GetPosition () const;
  void SetOrientation (const csMatrix3& trans);
  const csMatrix3 GetOrientation () const;
  void SetTransform (const csOrthoTransform& trans);
  const csOrthoTransform GetTransform () const;
  void SetLinearVelocity (const csVector3& vel);
  const csVector3 GetLinearVelocity () const;
  void SetAngularVelocity (const csVector3& vel);
  const csVector3 GetAngularVelocity () const;

  void SetProperties (float mass, const csVector3& center,
        const csMatrix3& inertia);
  void GetProperties (float* mass, csVector3* center,
    csMatrix3* inertia);
  float GetMass ();
  csVector3 GetCenter ();
  csMatrix3 GetInertia ();
  void AdjustTotalMass (float targetmass);

  void AddForce (const csVector3& force);
  void AddTorque (const csVector3& force);
  void AddRelForce (const csVector3& force);
  void AddRelTorque (const csVector3& force) ;
  void AddForceAtPos (const csVector3& force, const csVector3& pos);
  void AddForceAtRelPos (const csVector3& force, const csVector3& pos);
  void AddRelForceAtPos (const csVector3& force, const csVector3& pos);
  void AddRelForceAtRelPos (const csVector3& force, const csVector3& pos);

  const csVector3 GetForce () const;
  const csVector3 GetTorque () const;

  /*const csVector3 GetRelForce () const;
  const csVector3 GetRelTorque () const;*/

  //int GetJointCount () const = 0;

  void AttachMesh (iMeshWrapper* mesh);
  csRef<iMeshWrapper> GetAttachedMesh () { return mesh; }
  void SetMoveCallback (iDynamicsMoveCallback* cb);
  void SetCollisionCallback (iDynamicsCollisionCallback* cb);

  void Collision (iRigidBody *other);
  void Update ();

  csRef<iDynamicsSystemCollider> GetCollider (unsigned int index);
  int GetColliderCount () {return (int)colliders.GetSize ();};
};


struct csStrictODEJoint : public iBase
{
  dJointID jointID;
  csRef<iRigidBody> body[2];
  dBodyID bodyID[2];
  dJointFeedback *feedback;

  csStrictODEJoint () {feedback = NULL;};
  virtual ~csStrictODEJoint () {;};

  void Attach (iRigidBody *body1, iRigidBody *body2);
  csRef<iRigidBody> GetAttachedBody (int body);
  void SetParam (int joint_type, int param, int axis, float value);
  float GetParam (int joint_type, int param, int axis);
  csVector3 GetFeedbackForce1 ();
  csVector3 GetFeedbackTorque1 ();
  csVector3 GetFeedbackForce2 ();
  csVector3 GetFeedbackTorque2 ();

private:

  void CreateFeedback ();

};

/**
* This implements the slider joint.  It does this by strict copying
* ODEs interface.
*/
struct ODESliderJoint : public csStrictODEJoint, iODESliderJoint
{
  SCF_DECLARE_IBASE;

  ODESliderJoint (dWorldID w_id);
  virtual ~ODESliderJoint ();

  void SetLoStop (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamLoStop, axis, value);};
  void SetHiStop (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamHiStop, axis, value);};
  void SetVel (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamVel, axis, value);};
  void SetFMax (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamFMax, axis, value);};
  void SetFudgeFactor (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamFudgeFactor, axis, value);};
  void SetBounce (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamBounce, axis, value);};
  void SetCFM (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamCFM, axis, value);};
  void SetStopERP (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamStopERP, axis, value);};
  void SetStopCFM (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamStopCFM, axis, value);};
  void SetSuspensionERP (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamSuspensionERP, axis, value);};
  void SetSuspensionCFM (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamSuspensionCFM, axis, value);};

  float GetLoStop (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamLoStop, axis);};
  float GetHiStop (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamHiStop, axis);};
  float GetVel (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamVel, axis);};
  float GetFMax (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamFMax, axis);};
  float GetFudgeFactor (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamFudgeFactor, axis);};
  float GetBounce (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamBounce, axis);};
  float GetCFM (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamCFM, axis);};
  float GetStopERP (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamStopERP, axis);};
  float GetStopCFM (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamStopCFM, axis);};
  float GetSuspensionERP (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamSuspensionERP, axis);};
  virtual float GetSuspensionCFM (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_SLIDER, dParamSuspensionCFM, axis);};

  void SetSliderAxis (float x, float y, float z) {dJointSetSliderAxis (jointID, x, y, z);};
  csVector3 GetSliderAxis ();
  float GetSliderPosition () {return dJointGetSliderPosition (jointID);};
  float GetSliderPositionRate () {return dJointGetSliderPositionRate (jointID);};
  csVector3 GetFeedbackForce1 () {return csStrictODEJoint::GetFeedbackForce1 ();};
  csVector3 GetFeedbackTorque1 () {return csStrictODEJoint::GetFeedbackForce2 ();};
  csVector3 GetFeedbackForce2 () {return csStrictODEJoint::GetFeedbackTorque1 ();};
  csVector3 GetFeedbackTorque2 () {return csStrictODEJoint::GetFeedbackTorque2 ();};

  void Attach (iRigidBody *body1, iRigidBody *body2) {csStrictODEJoint::Attach (body1, body2);};
  csRef<iRigidBody> GetAttachedBody (int body) {return csStrictODEJoint::GetAttachedBody (body);};
};

struct ODEUniversalJoint : public csStrictODEJoint, iODEUniversalJoint
{
  SCF_DECLARE_IBASE;

  ODEUniversalJoint (dWorldID w_id);
  virtual ~ODEUniversalJoint ();

  void SetLoStop (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamLoStop, axis, value);};
  void SetHiStop (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamHiStop, axis, value);};
  void SetVel (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamVel, axis, value);};
  void SetFMax (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamFMax, axis, value);};
  void SetFudgeFactor (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamFudgeFactor, axis, value);};
  void SetBounce (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamBounce, axis, value);};
  void SetCFM (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamCFM, axis, value);};
  void SetStopERP (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamStopERP, axis, value);};
  void SetStopCFM (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamStopCFM, axis, value);};
  void SetSuspensionERP (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamSuspensionERP, axis, value);};
  void SetSuspensionCFM (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamSuspensionCFM, axis, value);};

  float GetLoStop (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamLoStop, axis);};
  float GetHiStop (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamHiStop, axis);};
  float GetVel (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamVel, axis);};
  float GetFMax (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamFMax, axis);};
  float GetFudgeFactor (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamFudgeFactor, axis);};
  float GetBounce (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamBounce, axis);};
  float GetCFM (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamCFM, axis);};
  float GetStopERP (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamStopERP, axis);};
  float GetStopCFM (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamStopCFM, axis);};
  float GetSuspensionERP (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamSuspensionERP, axis);};
  virtual float GetSuspensionCFM (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_UNIVERSAL, dParamSuspensionCFM, axis);};

  void SetUniversalAnchor (float x, float y, float z) {dJointSetUniversalAnchor (jointID, x, y, z);};
  void SetUniversalAxis1 (float x, float y, float z) {dJointSetUniversalAxis1 (jointID, x, y, z);};
  void SetUniversalAxis2 (float x, float y, float z) {dJointSetUniversalAxis2 (jointID, x, y, z);};
  csVector3 GetUniversalAnchor1 ();
  csVector3 GetUniversalAnchor2 ();
  csVector3 GetUniversalAxis1 ();
  csVector3 GetUniversalAxis2 ();
  csVector3 GetFeedbackForce1 () {return csStrictODEJoint::GetFeedbackForce1 ();};
  csVector3 GetFeedbackTorque1 () {return csStrictODEJoint::GetFeedbackForce2 ();};
  csVector3 GetFeedbackForce2 () {return csStrictODEJoint::GetFeedbackTorque1 ();};
  csVector3 GetFeedbackTorque2 () {return csStrictODEJoint::GetFeedbackTorque2 ();};

  void Attach (iRigidBody *body1, iRigidBody *body2) {csStrictODEJoint::Attach (body1, body2);};
  csRef<iRigidBody> GetAttachedBody (int body) {return csStrictODEJoint::GetAttachedBody (body);};
};

struct ODEBallJoint : public csStrictODEJoint, iODEBallJoint
{
  SCF_DECLARE_IBASE;

  ODEBallJoint (dWorldID w_id);

  void SetBallAnchor (const csVector3 &pos) {dJointSetBallAnchor (jointID, pos.x, pos.y, pos.z);};
  csVector3 GetBallAnchor1 ();
  csVector3 GetBallAnchor2 ();
  csVector3 GetAnchorError ();
  csVector3 GetFeedbackForce1 () {return csStrictODEJoint::GetFeedbackForce1 ();};
  csVector3 GetFeedbackTorque1 () {return csStrictODEJoint::GetFeedbackForce2 ();};
  csVector3 GetFeedbackForce2 () {return csStrictODEJoint::GetFeedbackTorque1 ();};
  csVector3 GetFeedbackTorque2 () {return csStrictODEJoint::GetFeedbackTorque2 ();};

  virtual ~ODEBallJoint ();

  void Attach (iRigidBody *body1, iRigidBody *body2) {csStrictODEJoint::Attach (body1, body2);};
  csRef<iRigidBody> GetAttachedBody (int body) {return csStrictODEJoint::GetAttachedBody (body);};

};

struct ODEAMotorJoint : public csStrictODEJoint, iODEAMotorJoint
{
  SCF_DECLARE_IBASE;

  ODEAMotorJoint (dWorldID w_id);
  virtual ~ODEAMotorJoint ();

  void SetLoStop (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamLoStop, axis, value);};
  void SetHiStop (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamHiStop, axis, value);};
  void SetVel (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamVel, axis, value);};
  void SetFMax (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamFMax, axis, value);};
  void SetFudgeFactor (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamFudgeFactor, axis, value);};
  void SetBounce (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamBounce, axis, value);};
  void SetCFM (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamCFM, axis, value);};
  void SetStopERP (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamStopERP, axis, value);};
  void SetStopCFM (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamStopCFM, axis, value);};
  void SetSuspensionERP (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamSuspensionERP, axis, value);};
  void SetSuspensionCFM (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamSuspensionCFM, axis, value);};

  float GetLoStop (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamLoStop, axis);};
  float GetHiStop (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamHiStop, axis);};
  float GetVel (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamVel, axis);};
  float GetFMax (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamFMax, axis);};
  float GetFudgeFactor (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamFudgeFactor, axis);};
  float GetBounce (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamBounce, axis);};
  float GetCFM (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamCFM, axis);};
  float GetStopERP (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamStopERP, axis);};
  float GetStopCFM (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamStopCFM, axis);};
  float GetSuspensionERP (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamSuspensionERP, axis);};
  virtual float GetSuspensionCFM (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_AMOTOR, dParamSuspensionCFM, axis);};

  void SetAMotorMode (ODEAMotorMode mode);
  ODEAMotorMode GetAMotorMode ();
  void SetAMotorNumAxes (int axis_num) {dJointSetAMotorNumAxes (jointID, axis_num);};
  int GetAMotorNumAxes () {return dJointGetAMotorNumAxes (jointID);};
  void SetAMotorAxis (int axis_num, int rel_orient, float x, float y, float z)
  {dJointSetAMotorAxis (jointID, axis_num, rel_orient, x, y, z);};
  void SetAMotorAxis (int axis_num, int rel_orient, const csVector3 &axis)
  {dJointSetAMotorAxis (jointID, axis_num, rel_orient, axis.x, axis.y, axis.z);};
  csVector3 GetAMotorAxis (int axis_num);
  int GetAMotorAxisRelOrientation (int axis_num) {return dJointGetAMotorAxisRel (jointID, axis_num);};
  void SetAMotorAngle (int axis_num, float angle) {dJointSetAMotorAngle (jointID, axis_num, angle);};
  float GetAMotorAngle (int axis_num) {return dJointGetAMotorAngle (jointID, axis_num);};
  float GetAMotorAngleRate (int axis_num) {return dJointGetAMotorAngle (jointID, axis_num);};
  csVector3 GetFeedbackForce1 () {return csStrictODEJoint::GetFeedbackForce1 ();};
  csVector3 GetFeedbackTorque1 () {return csStrictODEJoint::GetFeedbackForce2 ();};
  csVector3 GetFeedbackForce2 () {return csStrictODEJoint::GetFeedbackTorque1 ();};
  csVector3 GetFeedbackTorque2 () {return csStrictODEJoint::GetFeedbackTorque2 ();};

  void Attach (iRigidBody *body1, iRigidBody *body2) {csStrictODEJoint::Attach (body1, body2);};
  csRef<iRigidBody> GetAttachedBody (int body) {return csStrictODEJoint::GetAttachedBody (body);};

};

struct ODEHingeJoint : public csStrictODEJoint, iODEHingeJoint
{
  SCF_DECLARE_IBASE;

  ODEHingeJoint (dWorldID w_id);
  virtual ~ODEHingeJoint ();

  void SetLoStop (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_HINGE, dParamLoStop, axis, value);};
  void SetHiStop (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_HINGE, dParamHiStop, axis, value);};
  void SetVel (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_HINGE, dParamVel, axis, value);};
  void SetFMax (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_HINGE, dParamFMax, axis, value);};
  void SetFudgeFactor (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_HINGE, dParamFudgeFactor, axis, value);};
  void SetBounce (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_HINGE, dParamBounce, axis, value);};
  void SetCFM (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_HINGE, dParamCFM, axis, value);};
  void SetStopERP (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_HINGE, dParamStopERP, axis, value);};
  void SetStopCFM (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_HINGE, dParamStopCFM, axis, value);};
  void SetSuspensionERP (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_HINGE, dParamSuspensionERP, axis, value);};
  void SetSuspensionCFM (float value, int axis)
  {csStrictODEJoint::SetParam (CS_ODE_JOINT_TYPE_HINGE, dParamSuspensionCFM, axis, value);};

  float GetLoStop (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_HINGE, dParamLoStop, axis);};
  float GetHiStop (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_HINGE, dParamHiStop, axis);};
  float GetVel (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_HINGE, dParamVel, axis);};
  float GetFMax (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_HINGE, dParamFMax, axis);};
  float GetFudgeFactor (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_HINGE, dParamFudgeFactor, axis);};
  float GetBounce (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_HINGE, dParamBounce, axis);};
  float GetCFM (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_HINGE, dParamCFM, axis);};
  float GetStopERP (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_HINGE, dParamStopERP, axis);};
  float GetStopCFM (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_HINGE, dParamStopCFM, axis);};
  float GetSuspensionERP (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_HINGE, dParamSuspensionERP, axis);};
  virtual float GetSuspensionCFM (int axis)
  {return csStrictODEJoint::GetParam (CS_ODE_JOINT_TYPE_HINGE, dParamSuspensionCFM, axis);};

  void SetHingeAnchor (const csVector3 &pos) {dJointSetHingeAnchor (jointID, pos.x, pos.y, pos.z);};
  void SetHingeAxis (const csVector3 &axis) {dJointSetHingeAxis (jointID, axis.x, axis.y, axis.z);};
  csVector3 GetHingeAnchor1 ();
  csVector3 GetHingeAnchor2 ();
  csVector3 GetHingeAxis ();
  float GetHingeAngle () {return dJointGetHingeAngle (jointID);};
  float GetHingeAngleRate () {return dJointGetHingeAngleRate (jointID);};
  csVector3 GetAnchorError ();
  csVector3 GetFeedbackForce1 () {return csStrictODEJoint::GetFeedbackForce1 ();};
  csVector3 GetFeedbackTorque1 () {return csStrictODEJoint::GetFeedbackForce2 ();};
  csVector3 GetFeedbackForce2 () {return csStrictODEJoint::GetFeedbackTorque1 ();};
  csVector3 GetFeedbackTorque2 () {return csStrictODEJoint::GetFeedbackTorque2 ();};

  void Attach (iRigidBody *body1, iRigidBody *body2) {csStrictODEJoint::Attach (body1, body2);};
  csRef<iRigidBody> GetAttachedBody (int body) {return csStrictODEJoint::GetAttachedBody (body);};

};


/**
 * This implements the joint.  It does this by determining
 * which type of ODE joint best represents that described
 */
class csODEJoint : public iJoint
{
  dJointID jointID;
  csRef<iRigidBody> body[2];
  dBodyID bodyID[2];

  int transConstraint[3], rotConstraint[3];
  csVector3 maxTrans, minTrans, maxAngle, minAngle;
  csVector3 stopBounce, desiredVelocity, fMax;

  csOrthoTransform transform;

  csODEDynamicSystem* dynsys;

public:
  SCF_DECLARE_IBASE;

  csODEJoint (csODEDynamicSystem *sys);
  virtual ~csODEJoint ();

  inline dJointID GetID () { return jointID; }

  void Attach (iRigidBody *body1, iRigidBody *body2);
  csRef<iRigidBody> GetAttachedBody (int body);

  void SetTransform (const csOrthoTransform &trans);
  csOrthoTransform GetTransform ();

  void SetTransConstraints (bool X, bool Y, bool Z);
  inline bool IsXTransConstrained () { return transConstraint[0]; }
  inline bool IsYTransConstrained () { return transConstraint[1]; }
  inline bool IsZTransConstrained () { return transConstraint[2]; }
  void SetMinimumDistance (const csVector3 &min);
  csVector3 GetMinimumDistance ();
  void SetMaximumDistance (const csVector3 &max);
  csVector3 GetMaximumDistance ();

  void SetRotConstraints (bool X, bool Y, bool Z);
  inline bool IsXRotConstrained () { return rotConstraint[0]; }
  inline bool IsYRotConstrained () { return rotConstraint[1]; }
  inline bool IsZRotConstrained () { return rotConstraint[2]; }
  void SetMinimumAngle (const csVector3 &min);
  csVector3 GetMinimumAngle ();
  void SetMaximumAngle (const csVector3 &max);
  csVector3 GetMaximumAngle ();

  void SetBounce (const csVector3 & bounce );
  csVector3 GetBounce ();
  void SetDesiredVelocity (const csVector3 & velocity );
  csVector3 GetDesiredVelocity ();
  void SetMaxForce (const csVector3 & maxForce );
  csVector3 GetMaxForce ();

  struct ODEJointState : public iODEJointState
  {
    SCF_DECLARE_EMBEDDED_IBASE (csODEJoint);
    ODEJointType GetType();
    void SetLoStop (float value) { SetParam (dParamLoStop, value); }
    void SetHiStop (float value) { SetParam (dParamHiStop, value); }
    void SetVel (float value) { SetParam (dParamVel, value); }
    void SetFMax (float value) { SetParam (dParamFMax, value); }
    void SetFudgeFactor (float value) { SetParam (dParamFudgeFactor, value); }
    void SetBounce (float value) { SetParam (dParamBounce, value); }
    void SetCFM (float value) { SetParam (dParamCFM, value); }
    void SetStopERP (float value) { SetParam (dParamStopERP, value); }
    void SetStopCFM (float value) { SetParam (dParamStopCFM, value); }
    void SetSuspensionERP (float value) { SetParam (dParamSuspensionERP, value); }
    void SetSuspensionCFM (float value) { SetParam (dParamSuspensionCFM, value); }

    void SetLoStop2 (float value) { SetParam (dParamLoStop2, value); }
    void SetHiStop2 (float value) { SetParam (dParamHiStop2, value); }
    void SetVel2 (float value) { SetParam (dParamVel2, value); }
    void SetFMax2 (float value) { SetParam (dParamFMax2, value); }
    void SetFudgeFactor2 (float value) { SetParam (dParamFudgeFactor2, value); }
    void SetBounce2 (float value) { SetParam (dParamBounce2, value); }
    void SetCFM2 (float value) { SetParam (dParamCFM2, value); }
    void SetStopERP2 (float value) { SetParam (dParamStopERP2, value); }
    void SetStopCFM2 (float value) { SetParam (dParamStopCFM2, value); }
    void SetSuspensionERP2 (float value) { SetParam (dParamSuspensionERP2, value); }
    void SetSuspensionCFM2 (float value) { SetParam (dParamSuspensionCFM2, value); }

    void SetLoStop3 (float value) { SetParam (dParamLoStop3, value); }
    void SetHiStop3 (float value) { SetParam (dParamHiStop3, value); }
    void SetVel3 (float value) { SetParam (dParamVel3, value); }
    void SetFMax3 (float value) { SetParam (dParamFMax3, value); }
    void SetFudgeFactor3 (float value) { SetParam (dParamFudgeFactor3, value); }
    void SetBounce3 (float value) { SetParam (dParamBounce3, value); }
    void SetCFM3 (float value) { SetParam (dParamCFM3, value); }
    void SetStopERP3 (float value) { SetParam (dParamStopERP3, value); }
    void SetStopCFM3 (float value) { SetParam (dParamStopCFM3, value); }
    void SetSuspensionERP3 (float value) { SetParam (dParamSuspensionERP3, value); }
    void SetSuspensionCFM3 (float value) { SetParam (dParamSuspensionCFM3, value); }

    float GetLoStop () { return GetParam (dParamLoStop); }
    float GetHiStop () { return GetParam (dParamHiStop); }
    float GetVel () { return GetParam (dParamVel); }
    float GetFMax () { return GetParam (dParamFMax); }
    float GetFudgeFactor () { return GetParam (dParamFudgeFactor); }
    float GetBounce () { return GetParam (dParamBounce); }
    float GetCFM () { return GetParam (dParamCFM); }
    float GetStopERP () { return GetParam (dParamStopERP); }
    float GetStopCFM () { return GetParam (dParamStopCFM); }
    float GetSuspensionERP () { return GetParam (dParamSuspensionERP); }
    float GetSuspensionCFM () { return GetParam (dParamSuspensionCFM); }

    float GetLoStop2 () { return GetParam (dParamLoStop2); }
    float GetHiStop2 () { return GetParam (dParamHiStop2); }
    float GetVel2 () { return GetParam (dParamVel2); }
    float GetFMax2 () { return GetParam (dParamFMax2); }
    float GetFudgeFactor2 () { return GetParam (dParamFudgeFactor2); }
    float GetBounce2 () { return GetParam (dParamBounce2); }
    float GetCFM2 () { return GetParam (dParamCFM2); }
    float GetStopERP2 () { return GetParam (dParamStopERP2); }
    float GetStopCFM2 () { return GetParam (dParamStopCFM2); }
    float GetSuspensionERP2 () { return GetParam (dParamSuspensionERP2); }
    float GetSuspensionCFM2 () { return GetParam (dParamSuspensionCFM2); }

    float GetLoStop3 () { return GetParam (dParamLoStop3); }
    float GetHiStop3 () { return GetParam (dParamHiStop3); }
    float GetVel3 () { return GetParam (dParamVel3); }
    float GetFMax3 () { return GetParam (dParamFMax3); }
    float GetFudgeFactor3 () { return GetParam (dParamFudgeFactor3); }
    float GetBounce3 () { return GetParam (dParamBounce3); }
    float GetCFM3 () { return GetParam (dParamCFM3); }
    float GetStopERP3 () { return GetParam (dParamStopERP3); }
    float GetStopCFM3 () { return GetParam (dParamStopCFM3); }
    float GetSuspensionERP3 () { return GetParam (dParamSuspensionERP3); }
    float GetSuspensionCFM3 () { return GetParam (dParamSuspensionCFM3); }

    void SetHinge2Axis1 (const csVector3& axis);
    void SetHinge2Axis2 (const csVector3& axis);
    void SetHinge2Anchor (const csVector3& point);

  private:
    void SetParam (int parameter, float value);
    float GetParam (int parameter);
  } scfiODEJointState;

  friend struct ODEJointState;

private:

  void BuildHinge (const csVector3 &axis, float min, float max);
  void BuildHinge2 (const csVector3 &axis1, float min1, float max1,
         const csVector3 &axis2, float min2, float max2);
  void BuildSlider (const csVector3 &axis, float min, float max);
  void BuildJoint ();

  void ApplyJointProperty (int parameter, csVector3 & values);
};

/**
 * This is the implementation for a default dynamics move callback.
 * It can update mesh.
 */
class csODEDefaultMoveCallback : public iDynamicsMoveCallback
{
public:
  SCF_DECLARE_IBASE;

  csODEDefaultMoveCallback ();
  virtual ~csODEDefaultMoveCallback ();

  void Execute (iMeshWrapper* mesh, csOrthoTransform& t);
  void Execute (csOrthoTransform& t);
};

#endif // __CS_ODEDYNAMICS_H__
