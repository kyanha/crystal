/*
Copyright (C) 2001 by Jorrit Tyberghein
Piotr Obrzut

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
#include "csgeom/matrix3.h"
#include "csgeom/transfrm.h"
#include "csgeom/quaternion.h"
#include "csgeom/vector3.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "imesh/genmesh.h"

//bullet includes
#include "CcdPhysics/CcdPhysicsController.h"
#include "Dynamics/RigidBody.h"
#include "CcdPhysics/CcdPhysicsEnvironment.h"
#include "ConstraintSolver/SimpleConstraintSolver.h"
#include "CollisionDispatch/ToiContactDispatcher.h"
#include "BroadphaseCollision/SimpleBroadphase.h"
#include "CollisionShapes/BoxShape.h"
#include "CollisionShapes/EmptyShape.h"
#include "CollisionShapes/SphereShape.h"
#include "CollisionShapes/CylinderShape.h"
#include "PHY_IMotionState.h"
#include "SimdTransform.h"
#include "Dynamics/MassProps.h"

#include "bullet.h"


CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (csBulletDynamics)

//---------------------------------------------------------------------------

csBulletDynamics::csBulletDynamics (iBase *iParent)
:  scfImplementationType (this, iParent)
{

}
csBulletDynamics::~csBulletDynamics ()
{
}
bool csBulletDynamics::Initialize (iObjectRegistry* object_reg)
{
  csBulletDynamics::object_reg = object_reg;
  return true;
}
csPtr<iDynamicSystem> csBulletDynamics::CreateSystem ()
{
  csBulletDynamicsSystem* system = new csBulletDynamicsSystem ();
  csRef<iDynamicSystem> isystem (SCF_QUERY_INTERFACE (system, iDynamicSystem));
  systems.Push (system);
  isystem->DecRef ();

  return (csPtr<iDynamicSystem>)isystem;
}

void csBulletDynamics::RemoveSystem (iDynamicSystem* system)
{
  systems.Delete (system);
}

void csBulletDynamics::RemoveSystems ()
{
  systems.DeleteAll ();
}

iDynamicSystem* csBulletDynamics::FindSystem (const char *name)
{
  return systems.FindByName (name);
}

void csBulletDynamics::Step (float stepsize)
{
  for (size_t i = 0; i < systems.GetSize (); i++)
  {
    systems[i]->Step (stepsize);
  }
}
//-------------------------------csBulletDynamicsSystem----------------------------------------------//

csBulletDynamicsSystem::csBulletDynamicsSystem ()
:  scfImplementationType (this)
{
  ConstraintSolver* solver = new SimpleConstraintSolver;
  ToiContactDispatcher* dispatcher = new	ToiContactDispatcher(solver);
  BroadphaseInterface* broadphase = new SimpleBroadphase();

  bullet_sys = new CcdPhysicsEnvironment(dispatcher,broadphase);
}
csBulletDynamicsSystem::~csBulletDynamicsSystem ()
{
}
void csBulletDynamicsSystem::SetGravity (const csVector3& v)
{
  gravity = v; 
  bullet_sys->setGravity(v.x, v.y, v.z);
}
const csVector3 csBulletDynamicsSystem::GetGravity () const
{
  return gravity;
}
void csBulletDynamicsSystem::SetLinearDampener (float d)
{
}
float csBulletDynamicsSystem::GetLinearDampener () const
{
  return 0;
}
void csBulletDynamicsSystem::SetRollingDampener (float d)
{
}
float csBulletDynamicsSystem::GetRollingDampener () const
{
  return 0;
}
void csBulletDynamicsSystem::EnableAutoDisable (bool enable)
{
}
bool csBulletDynamicsSystem::AutoDisableEnabled ()
{
  return false;
}
void csBulletDynamicsSystem::SetAutoDisableParams (float linear, float angular, int steps,
                                                   float time)
{
}
void csBulletDynamicsSystem::Step (float stepsize)
{
  bullet_sys->proceedDeltaTime (0.f, stepsize);

  for (size_t i = 0; i < bodies.GetSize (); i++)
    bodies[i]->Update ();

}
csPtr<iRigidBody> csBulletDynamicsSystem::CreateBody ()
{
  csBulletRigidBody *b = new csBulletRigidBody (this);
  bullet_sys->addCcdPhysicsController (b->GetBulletBody ());

  csRef<iRigidBody> ib = SCF_QUERY_INTERFACE (b, iRigidBody);
  bodies.Push (ib);
  ib->DecRef ();

  return (csPtr<iRigidBody>) ib;
}
void csBulletDynamicsSystem::RemoveBody (iRigidBody* body)
{
}
iRigidBody *csBulletDynamicsSystem::FindBody (const char *name)
{
  return 0;
}
iRigidBody *csBulletDynamicsSystem::GetBody (unsigned int index)
{
  return 0;
}
int csBulletDynamicsSystem::GetBodysCount ()
{
  return 0;
}
csPtr<iBodyGroup> csBulletDynamicsSystem::CreateGroup ()
{
  return 0;
}
void csBulletDynamicsSystem::RemoveGroup (iBodyGroup* group)
{
}
csPtr<iJoint> csBulletDynamicsSystem::CreateJoint ()
{
  return 0;
}
void csBulletDynamicsSystem::RemoveJoint (iJoint* joint)
{
}
iDynamicsMoveCallback* csBulletDynamicsSystem::GetDefaultMoveCallback ()
{
  return 0;
}
bool csBulletDynamicsSystem::AttachColliderMesh (iMeshWrapper* mesh,
                                                 const csOrthoTransform& trans, float friction,
                                                 float elasticity, float softness)
{
  return false;
}

bool csBulletDynamicsSystem::AttachColliderCylinder (float length, float radius,
                                                     const csOrthoTransform& trans, float friction,
                                                     float elasticity, float softness)
{
  return false;
}

bool csBulletDynamicsSystem::AttachColliderBox (const csVector3 &size,
                                                const csOrthoTransform& trans, float friction,
                                                float elasticity, float softness)
{
  csBulletCollider *bulletc = new csBulletCollider (this);
  bullet_sys->addCcdPhysicsController (bulletc->GetBulletController ());
  bulletc->SetElasticity (elasticity);
  bulletc->SetFriction (friction);
  bulletc->SetSoftness (softness);
  bulletc->CreateBoxGeometry (size);
  bulletc->SetTransform (csOrthoTransform(trans.GetO2T().GetTranspose(),trans.GetOrigin()));
  colliders.Push (bulletc);

  return true;
}

bool csBulletDynamicsSystem::AttachColliderSphere (float radius, const csVector3 &offset,
                                                   float friction, float elasticity, float softness)
{
  return false;
}

bool csBulletDynamicsSystem::AttachColliderPlane (const csPlane3 &plane, float friction,
                                                  float elasticity, float softness)
{
  return false;
}

void csBulletDynamicsSystem::AttachCollider (iDynamicsSystemCollider* collider)
{
}

csRef<iDynamicsSystemCollider> csBulletDynamicsSystem::CreateCollider () 
{
  return 0;
}

//-------------------------------csBulletRigidBody----------------------------------------------//
csBulletRigidBody::csBulletRigidBody (csBulletDynamicsSystem* dynsys)
:  scfImplementationType (this)
{
  ds = dynsys;
  ms = new MyMotionState ();
  ms->setWorldPosition (0,0,0);
  ms->setWorldOrientation (0,0,0,1);

  CcdConstructionInfo ccdObjectCi;

  ccdObjectCi.m_collisionShape = new EmptyShape ();
  ccdObjectCi.m_collisionShape->SetMargin (1);
  ccdObjectCi.m_broadphaseHandle = 0;
  ccdObjectCi.m_gravity = SimdVector3(0,0,0);
  ccdObjectCi.m_localInertiaTensor = SimdVector3(0,0,0);
  mass = ccdObjectCi.m_mass = 1;

  ccdObjectCi.m_MotionState = ms;
  bbody = new CcdPhysicsController(ccdObjectCi);

  move_cb = (iDynamicsMoveCallback*)new csBulletDefaultMoveCallback ();
}
csBulletRigidBody::~csBulletRigidBody ()
{
  delete bbody;
  if (move_cb) move_cb->DecRef ();
}

bool csBulletRigidBody::MakeStatic (void)
{
  return true;
}
bool csBulletRigidBody::MakeDynamic (void)
{
  return false;
}
bool csBulletRigidBody::IsStatic (void)
{
  return false;
}
bool csBulletRigidBody::Disable (void)
{
  return false;
}
bool csBulletRigidBody::Enable (void)
{
  bbody->SetActive (true);
  return true;
}

bool csBulletRigidBody::IsEnabled (void)
{
  return bbody->GetRigidBody()->GetActivationState() == 1;
}

csRef<iBodyGroup> csBulletRigidBody::GetGroup (void)
{
  return 0;
}

void csBulletRigidBody::ResetShape ()
{
  BroadphaseProxy* bpproxy = (BroadphaseProxy*)bbody->m_broadphaseHandle;
  bpproxy->SetClientObjectType(bbody->GetRigidBody()->GetCollisionShape()->GetShapeType());
  ds->GetBulletSys ()->GetBroadphase()->CleanProxyFromPairs(bpproxy);

  SimdVector3 newinertia;
  bbody->GetRigidBody()->GetCollisionShape()->CalculateLocalInertia(mass,newinertia);
  bbody->GetRigidBody()->setMassProps(mass,newinertia);
  bbody->GetRigidBody()->updateInertiaTensor();
}

bool csBulletRigidBody::AttachColliderMesh (iMeshWrapper* mesh,
                                            const csOrthoTransform& trans, float friction, float density,
                                            float elasticity, float softness)
{
  return false;
}

bool csBulletRigidBody::AttachColliderCylinder (float length, float radius,
                                                const csOrthoTransform& trans, float friction, float density,
                                                float elasticity, float softness)
{
  if (bbody->GetRigidBody ()->GetCollisionShape ())
    delete bbody->GetCollisionShape ();

  //bbody->GetRigidBody ()->SetCollisionShape (new CylinderShape ());

  //ResetShape ();

  return false;
}

bool csBulletRigidBody::AttachColliderBox (const csVector3 &size,
                                           const csOrthoTransform& trans, float friction, float density,
                                           float elasticity, float softness)
{
  if (bbody->GetRigidBody ()->GetCollisionShape ())
    delete bbody->GetCollisionShape ();

  SimdVector3 s (size.x, size.y, size.z); 
  bbody->GetRigidBody ()->SetCollisionShape (new BoxShape (s));
  bbody->GetRigidBody ()->setFriction (friction);

  ResetShape ();

  return true;
}

bool csBulletRigidBody::AttachColliderSphere (float radius, const csVector3 &offset,
                                              float friction, float density, float elasticity,
                                              float softness)
{
  if (bbody->GetRigidBody ()->GetCollisionShape ())
    delete bbody->GetCollisionShape ();

  bbody->GetRigidBody ()->SetCollisionShape (new SphereShape (radius)); 
  bbody->GetRigidBody ()->setFriction (friction);

  ResetShape ();

  return true;
}

bool csBulletRigidBody::AttachColliderPlane (const csPlane3 &plane, float friction,
                                             float density, float elasticity, float softness)
{
  return false;
}

void csBulletRigidBody::AttachCollider (iDynamicsSystemCollider* collider)
{
}

void csBulletRigidBody::DestroyColliders ()
{
}
void csBulletRigidBody::DestroyCollider (iDynamicsSystemCollider* collider)
{
}

void csBulletRigidBody::SetPosition (const csVector3& trans)
{
  bbody->setPosition (trans.x, trans.y, trans.z);
}

const csVector3 csBulletRigidBody::GetPosition () const
{
  csVector3 v;
  ms->getWorldPosition (v.x, v.y, v.z);
  return v;
}

void csBulletRigidBody::SetOrientation (const csMatrix3& rot)
{
  csQuaternion q (rot);
  bbody->setOrientation (q.x, q.y, q.z, q.r);
}

const csMatrix3 csBulletRigidBody::GetOrientation () const
{
  csQuaternion q;
  bbody->getOrientation (q.x, q.y, q.z, q.r);
  return csMatrix3 (q);
}

void csBulletRigidBody::SetTransform (const csOrthoTransform& trans)
{
  SetPosition (trans.GetOrigin ());
  SetOrientation (trans.GetO2T ());
}

const csOrthoTransform csBulletRigidBody::GetTransform () const
{
  return csOrthoTransform (GetOrientation (), GetPosition ());
}

void csBulletRigidBody::SetLinearVelocity (const csVector3& vel)
{
  bbody->SetLinearVelocity (vel.x,vel.y,vel.z, false);
}

const csVector3 csBulletRigidBody::GetLinearVelocity () const
{
  return csVector3 (0);
}

void csBulletRigidBody::SetAngularVelocity (const csVector3& vel)
{
}

const csVector3 csBulletRigidBody::GetAngularVelocity () const
{
  return csVector3 (0);
}

void csBulletRigidBody::SetProperties (float mass, const csVector3& center,
                                       const csMatrix3& inertia)
{
  csQuaternion q (inertia); 
  bbody->GetRigidBody ()->setMassProps (mass,SimdVector3 (q.x, q.y, q.z));

  csVector3 g = ds->GetGravity ();
  bbody->GetRigidBody ()->setGravity (SimdVector3 (g.x, g.y, g.z));

  csBulletRigidBody::mass = mass;
}

void csBulletRigidBody::GetProperties (float* mass, csVector3* center,
                                       csMatrix3* inertia)
{
}

float csBulletRigidBody::GetMass ()
{
  return 0;
}

csVector3 csBulletRigidBody::GetCenter ()
{
  return csVector3 (0);
}
csMatrix3 csBulletRigidBody::GetInertia ()
{
  return csMatrix3 ();
}
void csBulletRigidBody::AdjustTotalMass (float targetmass)
{
}
void csBulletRigidBody::AddForce (const csVector3& force)
{
  SimdVector3 f (force.x, force.y, force.z);
  bbody->GetRigidBody ()->applyCentralForce(f);
}
void csBulletRigidBody::AddTorque (const csVector3& force)
{
}
void csBulletRigidBody::AddRelForce (const csVector3& force)
{
}
void csBulletRigidBody::AddRelTorque (const csVector3& force) 
{
}
void csBulletRigidBody::AddForceAtPos (const csVector3& force, const csVector3& pos)
{
}
void csBulletRigidBody::AddForceAtRelPos (const csVector3& force,
                                          const csVector3& pos)
{
}
void csBulletRigidBody::AddRelForceAtPos (const csVector3& force,
                                          const csVector3& pos)
{
}
void csBulletRigidBody::AddRelForceAtRelPos (const csVector3& force,
                                             const csVector3& pos)
{
}
const csVector3 csBulletRigidBody::GetForce () const
{
  return csVector3 (0);
}
const csVector3 csBulletRigidBody::GetTorque () const
{
  return csVector3 (0);
}
void csBulletRigidBody::AttachMesh (iMeshWrapper* m)
{
  if (m) m->IncRef ();
  if (mesh) mesh->DecRef ();
  mesh = m;
}
iMeshWrapper* csBulletRigidBody::GetAttachedMesh ()
{
  return 0;
}

void csBulletRigidBody::SetMoveCallback (iDynamicsMoveCallback* cb)
{
  if (cb) cb->IncRef ();
  if (move_cb) move_cb->DecRef ();
  move_cb = cb;
}
void csBulletRigidBody::SetCollisionCallback (iDynamicsCollisionCallback* cb)
{
}
void csBulletRigidBody::Collision (iRigidBody *other)
{
}
csRef<iDynamicsSystemCollider> csBulletRigidBody::GetCollider (unsigned int index)
{
  return 0;
}
int csBulletRigidBody::GetColliderCount ()
{
  return 0;
}
void csBulletRigidBody::Update ()
{
  if (bbody && move_cb)
  {
    csOrthoTransform trans;
    trans = GetTransform ();
    if (mesh) move_cb->Execute (mesh, trans);
    /* remainder case for all other callbacks */
    move_cb->Execute (trans);
  }
}
//-------------------------------MyMotionState----------------------------------------------//
MyMotionState::MyMotionState()
{
  m_worldTransform.setIdentity();
}


MyMotionState::~MyMotionState()
{

}

void	MyMotionState::getWorldPosition(float& posX,float& posY,float& posZ)
{
  posX = m_worldTransform.getOrigin().x();
  posY = m_worldTransform.getOrigin().y();
  posZ = m_worldTransform.getOrigin().z();
}

void	MyMotionState::getWorldScaling(float& scaleX,float& scaleY,float& scaleZ)
{
  scaleX = 1.;
  scaleY = 1.;
  scaleZ = 1.;
}

void	MyMotionState::getWorldOrientation(float& quatIma0,float& quatIma1,float& quatIma2,float& quatReal)
{
  quatIma0 = m_worldTransform.getRotation().x();
  quatIma1 = m_worldTransform.getRotation().y();
  quatIma2 = m_worldTransform.getRotation().z();
  quatReal = m_worldTransform.getRotation()[3];
}

void	MyMotionState::setWorldPosition(float posX,float posY,float posZ)
{
  SimdPoint3 pos(posX,posY,posZ);
  m_worldTransform.setOrigin( pos );
}

void	MyMotionState::setWorldOrientation(float quatIma0,float quatIma1,float quatIma2,float quatReal)
{
  SimdQuaternion orn(quatIma0,quatIma1,quatIma2,quatReal);
  m_worldTransform.setRotation( orn );
}

void	MyMotionState::calculateWorldTransformations()
{

}

//-------------------------------csBulletDefaultMoveCallback----------------------------------------------//

csBulletDefaultMoveCallback::csBulletDefaultMoveCallback () 
:  scfImplementationType (this)
{

}

csBulletDefaultMoveCallback::~csBulletDefaultMoveCallback ()
{

}

void csBulletDefaultMoveCallback::Execute (iMeshWrapper* mesh,
                                           csOrthoTransform& t)
{
  // Dont do anything if nothing has changed
  if (mesh->GetMovable()->GetPosition() == t.GetOrigin() &&
    mesh->GetMovable()->GetTransform().GetT2O() == t.GetT2O())
    return;

  // Update movable
  mesh->GetMovable ()->SetPosition (t.GetOrigin ());
  mesh->GetMovable ()->GetTransform ().SetT2O (t.GetT2O ());
  mesh->GetMovable ()->UpdateMove ();
}

void csBulletDefaultMoveCallback::Execute (csOrthoTransform& t)
{
  /* do nothing by default */
}

//-------------------------------csBulletCollider----------------------------------------------//
csBulletCollider::csBulletCollider (csBulletDynamicsSystem* dynsys)
:  scfImplementationType (this)
{
  CcdConstructionInfo ccdObjectCi;

  MyMotionState *ms = new MyMotionState ();

  ms->setWorldPosition (0, 0, 0);
  ms->setWorldOrientation (0,0,0,1);

  ccdObjectCi.m_collisionShape = new EmptyShape ();
  ccdObjectCi.m_collisionShape->SetMargin (0.2f);
  ccdObjectCi.m_broadphaseHandle = 0;
  ccdObjectCi.m_gravity = SimdVector3(0,0,0);
  ccdObjectCi.m_localInertiaTensor = SimdVector3(0,0,0);
  ccdObjectCi.m_mass = 0; //collider need 0 mass
  ccdObjectCi.m_MotionState = ms;

  bc = new CcdPhysicsController (ccdObjectCi);

  ds = dynsys;
}
csBulletCollider::~csBulletCollider ()
{
  delete bc;
}
void csBulletCollider::ResetShape ()
{
  BroadphaseProxy* bpproxy = (BroadphaseProxy*)bc->m_broadphaseHandle;
  bpproxy->SetClientObjectType(bc->GetRigidBody()->GetCollisionShape()->GetShapeType());
  ds->GetBulletSys ()->GetBroadphase()->CleanProxyFromPairs(bpproxy);

  //SimdVector3 newinertia;
  //bbody->GetRigidBody()->GetCollisionShape()->CalculateLocalInertia(mass,newinertia);
  //bbody->GetRigidBody()->setMassProps(mass,newinertia);
  //bbody->GetRigidBody()->updateInertiaTensor();
}

bool csBulletCollider::CreateSphereGeometry (const csSphere& sphere)
{
  return false;
}
bool csBulletCollider::CreatePlaneGeometry (const csPlane3& plane)
{
  return false;
}
bool csBulletCollider::CreateMeshGeometry (iMeshWrapper *mesh)
{
  return false;
}
bool csBulletCollider::CreateBoxGeometry (const csVector3& size)
{
  if (bc->GetRigidBody ()->GetCollisionShape ())
    delete bc->GetCollisionShape ();

  SimdVector3 s (size.x, size.y, size.z); 
  bc->GetRigidBody ()->SetCollisionShape (new BoxShape (s));
  ResetShape ();
  geom_type = BOX_COLLIDER_GEOMETRY;

  return true;
}
bool csBulletCollider::CreateCCylinderGeometry (float length, float radius)
{
  return false;
}
void csBulletCollider::SetCollisionCallback (
  iDynamicsColliderCollisionCallback* cb)
{
}
void csBulletCollider::SetFriction (float friction)
{
  bc->GetRigidBody ()->setFriction (friction);
}
void csBulletCollider::SetSoftness (float softness)
{
}
void csBulletCollider::SetDensity (float density)
{
}
void csBulletCollider::SetElasticity (float elasticity)
{
}
float csBulletCollider::GetFriction ()
{
  return 0;
}
float csBulletCollider::GetSoftness ()
{
  return 0;
}
float csBulletCollider::GetDensity ()
{
  return 0;
}
float csBulletCollider::GetElasticity ()
{
  return 0;
}
void csBulletCollider::FillWithColliderGeometry (csRef<iGeneralFactoryState> genmesh_fact)
{
}
csColliderGeometryType csBulletCollider::GetGeometryType ()
{
  return NO_GEOMETRY;
}
csOrthoTransform csBulletCollider::GetTransform ()
{
  return csOrthoTransform ();
}
csOrthoTransform csBulletCollider::GetLocalTransform ()
{
  return csOrthoTransform ();
}
void csBulletCollider::SetTransform (const csOrthoTransform& trans)
{
  csQuaternion q (trans.GetO2T ());
  bc->setOrientation (q.x, q.y, q.z, q.r);
  csVector3 pos = trans.GetOrigin ();
  bc->setPosition (pos.x, pos.y, pos.z);
}
bool csBulletCollider::GetBoxGeometry (csVector3& size)
{
  return false;
}
bool csBulletCollider::GetSphereGeometry (csSphere& sphere)
{
  return false;
}
bool csBulletCollider::GetPlaneGeometry (csPlane3& plane)
{
  return false;
}
bool csBulletCollider::GetCylinderGeometry (float& length, float& radius)
{
  return false;
}
void csBulletCollider::MakeStatic ()
{
}
void csBulletCollider::MakeDynamic ()
{
}
bool csBulletCollider::IsStatic ()
{
  return false;
}