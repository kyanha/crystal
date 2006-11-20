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

#include "cssysdef.h"
#include "csqsqrt.h"

#include "csgeom/obb.h"
#include "csgeom/plane3.h"
#include "csgeom/sphere.h"
#include "csgeom/pmtools.h"
#include "cstool/collider.h"
#include "csutil/event.h"
#include "csutil/eventnames.h"
#include "iengine/engine.h"
#include "imesh/objmodel.h"
#include "igeom/polymesh.h"
#include "imesh/object.h"
#include "imesh/genmesh.h"
#include "iutil/evdefs.h"
#include "iutil/event.h"
#include "iutil/eventq.h"
#include "iutil/objreg.h"
#include "iutil/plugin.h"
#include "iutil/virtclk.h"
#include "ivaria/collider.h"
#include "ivaria/reporter.h"
#include "ivaria/dynamics.h"

#include "odedynam.h"

// Some macros for passing matrix from cs to ode and back
#define CS2ODEMATRIX(csmat, odemat)  \
{  \
  odemat[0] = csmat.m11; odemat[4] = csmat.m12; odemat[8] = csmat.m13; odemat[3] = 0; \
  odemat[1] = csmat.m21; odemat[5] = csmat.m22; odemat[9] = csmat.m23; odemat[7] = 0; \
  odemat[2] = csmat.m31; odemat[6] = csmat.m32; odemat[10] = csmat.m33; odemat[11] = 0; \
}

#define ODE2CSMATRIX(odemat, csmat) \
{   \
  csmat.m11 = odemat[0]; csmat.m12 = odemat[4]; csmat.m13 = odemat[8]; \
  csmat.m21 = odemat[1]; csmat.m22 = odemat[5]; csmat.m23 = odemat[9]; \
  csmat.m31 = odemat[2]; csmat.m32 = odemat[6]; csmat.m33 = odemat[10]; \
}

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE (csODEDynamics)
SCF_IMPLEMENTS_INTERFACE (iDynamics)
SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iODEDynamicState)
SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csODEDynamics::ODEDynamicState)
SCF_IMPLEMENTS_INTERFACE (iODEDynamicState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csODEDynamics::Component)
SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csODEDynamics::EventHandler)
SCF_IMPLEMENTS_INTERFACE (iEventHandler)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_IBASE_EXT (csODEDynamicSystem)
SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iDynamicSystem)
SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iODEDynamicSystemState);
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csODEDynamicSystem::DynamicSystem)
SCF_IMPLEMENTS_INTERFACE (iDynamicSystem)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csODEDynamicSystem::ODEDynamicSystemState)
SCF_IMPLEMENTS_INTERFACE (iODEDynamicSystemState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csODEBodyGroup)
SCF_IMPLEMENTS_INTERFACE (iBodyGroup)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_IBASE_EXT (csODERigidBody)
SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iRigidBody)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_IBASE (csODECollider)
SCF_IMPLEMENTS_INTERFACE (iDynamicsSystemCollider)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csODERigidBody::RigidBody)
SCF_IMPLEMENTS_INTERFACE (iRigidBody)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (ODEBallJoint)
SCF_IMPLEMENTS_INTERFACE (iODEBallJoint)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_IBASE (ODESliderJoint)
SCF_IMPLEMENTS_INTERFACE (iODESliderJoint)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_IBASE (ODEHingeJoint)
SCF_IMPLEMENTS_INTERFACE (iODEHingeJoint)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_IBASE (ODEHinge2Joint)
SCF_IMPLEMENTS_INTERFACE (iODEHinge2Joint)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_IBASE (ODEAMotorJoint)
SCF_IMPLEMENTS_INTERFACE (iODEAMotorJoint)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_IBASE (ODEUniversalJoint)
SCF_IMPLEMENTS_INTERFACE (iODEUniversalJoint)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_IBASE (csODEDefaultMoveCallback)
SCF_IMPLEMENTS_INTERFACE (iDynamicsMoveCallback)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_FACTORY (csODEDynamics)


void DestroyGeoms( csGeomList & geoms );


int csODEDynamics::geomclassnum = 0;
dJointGroupID csODEDynamics::contactjoints = dJointGroupCreate (0);

csODEDynamics::csODEDynamics (iBase* parent)
{
  SCF_CONSTRUCT_IBASE (parent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiODEDynamicState);
  object_reg = 0;
  scfiEventHandler = 0;
  process_events = false;

  // Initialize the colliders so that the class isn't overwritten
  dGeomID id = dCreateSphere (0, 1);
  dGeomDestroy (id);

  //dGeomClass c;
  //c.bytes = sizeof (MeshInfo);
  //c.collider = &CollideSelector;
  //c.aabb = &GetAABB;
  //c.aabb_test = 0;
  //c.dtor = 0;
  //geomclassnum = dCreateGeomClass (&c);

  erp = 0.2f;
  cfm = 1e-5f;

  rateenabled = false;
  steptime = 0.1f;
  limittime = 1.0f;
  total_elapsed = 0.0f;

  stepfast = false;
  sfiter = 10;
  quickstep = false;
  qsiter = 10;
  fastobjects = false;
}

csODEDynamics::~csODEDynamics ()
{
  if (scfiEventHandler)
  {
    csRef<iEventQueue> q = CS_QUERY_REGISTRY (object_reg, iEventQueue);
    if (q)
      q->RemoveListener (scfiEventHandler);
  }
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiODEDynamicState);
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiComponent);
  SCF_DESTRUCT_IBASE();
}

bool csODEDynamics::Initialize (iObjectRegistry* object_reg)
{
  csODEDynamics::object_reg = object_reg;

  clock = CS_QUERY_REGISTRY (object_reg, iVirtualClock);
  if (!clock)
    return false;

  PreProcess = csevPreProcess (object_reg);

  return true;
}

csPtr<iDynamicSystem> csODEDynamics::CreateSystem ()
{
  csODEDynamicSystem* system = new csODEDynamicSystem (erp, cfm);
  csRef<iDynamicSystem> isystem (SCF_QUERY_INTERFACE (system, iDynamicSystem));
  systems.Push (isystem);
  isystem->DecRef ();
  if(stepfast) system->EnableStepFast(true);
  else if(quickstep) system->EnableQuickStep(true);
  return csPtr<iDynamicSystem> (isystem);
}

void csODEDynamics::RemoveSystem (iDynamicSystem* system)
{
  systems.Delete (system);
}

void csODEDynamics::RemoveSystems ()
{
  systems.DeleteAll ();
}

iDynamicSystem *csODEDynamics::FindSystem (const char *name)
{
  return systems.FindByName (name);
}

void csODEDynamics::Step (float elapsed_time)
{
  float stepsize;
  if (process_events)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR, "csODEDynamics",
      "Step was called after event processing was enabled");
    return;
  }
  if (rateenabled)
  {
    stepsize = steptime;
    if (elapsed_time > limittime) elapsed_time = limittime;
  }
  else
  {
    stepsize = elapsed_time;
  }
  total_elapsed += elapsed_time;

  // TODO handle fractional total_remaining (interpolate render)
  while (total_elapsed > stepsize)
  {
    total_elapsed -= stepsize;
    for (size_t i=0; i<systems.Length(); i++)
    {
      systems.Get (i)->Step (stepsize);
      for (size_t j = 0; j < updates.Length(); j ++)
      {
        updates[i]->Execute (stepsize);
      }
      dJointGroupEmpty (contactjoints);
    }
  }
}

void csODEDynamics::NearCallback (void *data, dGeomID o1, dGeomID o2)
{
  if (dGeomIsSpace(o1) || dGeomIsSpace (o2))
  {
    dSpaceCollide2 (o1, o2, data, &csODEDynamics::NearCallback);
    if (dGeomIsSpace(o1))
      dSpaceCollide ((dxSpace*)o1, data, &csODEDynamics::NearCallback);
    if (dGeomIsSpace(o2))
      dSpaceCollide ((dxSpace*)o2, data, &csODEDynamics::NearCallback);
    return;
  }

  csODERigidBody *b1 = 0, *b2 = 0;
  if (dGeomGetBody(o1))
  {
    b1 = (csODERigidBody *)dBodyGetData (dGeomGetBody(o1));
  }
  if (dGeomGetBody(o2))
  {
    b2 = (csODERigidBody *)dBodyGetData (dGeomGetBody(o2));
  }


  if (!b1 && !b2)
  {
    csODECollider* c1 = ((GeomData *)dGeomGetData (o1))->collider;
    csODECollider* c2 = ((GeomData *)dGeomGetData (o2))->collider;

    if (c1->IsStatic () && c2->IsStatic ())
      return;

    dContact contact[1];
    int a = dCollide (o1, o2, 1, &(contact[0].geom), sizeof (dContact));
    if (a > 0)
    {
      csVector3 pos (contact[0].geom.pos[0], contact[0].geom.pos[1],
	  contact[0].geom.pos[2]);
      csVector3 normal (contact[0].geom.normal[0], contact[0].geom.normal[1],
	  contact[0].geom.normal[2]);
      c1->Collision (c2, pos, normal, contact[0].geom.depth);
      c2->Collision (c1, pos, -normal, contact[0].geom.depth);
    }
  }

  if ((!b1 || b1->IsStatic()) && (!b2 || b2->IsStatic())) return;
  if (b1 && b2 && b1->GetGroup() != 0 && b1->GetGroup() == b2->GetGroup())
    return;

  dContact contact[512];
  int a = dCollide (o1, o2, 512, &(contact[0].geom), sizeof (dContact));
  if (a > 0)
  {
    csVector3 pos (contact[0].geom.pos[0], contact[0].geom.pos[1],
	  contact[0].geom.pos[2]);
    csVector3 normal (contact[0].geom.normal[0], contact[0].geom.normal[1],
	  contact[0].geom.normal[2]);
    float depth = contact[0].geom.depth;
    /* there is only 1 actual body per set */
    if (b1)
    {
      b1->Collision ((b2) ? &b2->scfiRigidBody : 0, pos, normal, depth);
      if (!b2)
        ((GeomData *)dGeomGetData (o2))->collider->Collision (
		&b1->scfiRigidBody, pos, -normal, depth);
    }
    if (b2)
    {
      b2->Collision ((b1) ? &b1->scfiRigidBody : 0, pos, -normal, depth);
      if (!b1)
        ((GeomData *)dGeomGetData (o1))->collider->Collision (
		&b2->scfiRigidBody, pos, normal, depth);
    }

    for( int i=0; i<a; i++ )
    {
      float *f1 = ((GeomData *)dGeomGetData (contact[i].geom.g1))->surfacedata;
      float *f2 = ((GeomData *)dGeomGetData (contact[i].geom.g2))->surfacedata;

      contact[i].surface.mode = dContactBounce | dContactSoftCFM
        | dContactSlip1 | dContactSlip2 | dContactApprox1;
      contact[i].surface.mu = f1[0]*f2[0];
      contact[i].surface.bounce = f1[1]*f2[1];
      contact[i].surface.bounce_vel = 0.1f;
      contact[i].surface.slip1 = SMALL_EPSILON;
      contact[i].surface.slip2 = SMALL_EPSILON;
      contact[i].surface.soft_cfm = f1[2]*f2[2];

      dJointID c = dJointCreateContact ( ((csODEDynamicSystem*)data)
        ->GetWorldID(), contactjoints,contact+i );
      dJointAttach (c, dGeomGetBody(o1), dGeomGetBody(o2));
    }
  }
}

csReversibleTransform GetGeomTransform (dGeomID id)
{
  const dReal* pos = dGeomGetPosition (id);
  const dReal* mat = dGeomGetRotation (id);
  /* Need to use the inverse in this case */
  csMatrix3 rot;
  ODE2CSMATRIX(mat,rot);
  return csReversibleTransform (rot, csVector3 (pos[0], pos[1], pos[2]));
}

typedef csDirtyAccessArray<csMeshedPolygon> csPolyMeshList;

#if 0
void csODEDynamics::GetAABB (dGeomID g, dReal aabb[6])
{
  csBox3 box;
  csReversibleTransform mesht = GetGeomTransform (g);
  MeshInfo *mi = (MeshInfo *)dGeomGetClassData (g);
  iMeshWrapper *m = mi->mesh;
  iPolygonMesh* p = m->GetMeshObject()->GetObjectModel()->GetPolygonMeshColldet();
  csVector3 *vertex_table = p->GetVertices ();
  box.StartBoundingBox ();
  for (int i = 0; i < p->GetVertexCount(); i ++)
  {
    box.AddBoundingVertex (vertex_table[i] / mesht);
  }
  aabb[0] = box.MinX(); aabb[1] = box.MaxX();
  aabb[2] = box.MinY(); aabb[3] = box.MaxY();
  aabb[4] = box.MinZ(); aabb[5] = box.MaxZ();
}
#endif

void csODEDynamics::SetGlobalERP (float erp)
{
  csODEDynamics::erp = erp;

  for (size_t i = 0; i < systems.Length(); i ++)
  {
    csRef<iODEDynamicSystemState> sys = SCF_QUERY_INTERFACE (systems[i],
      iODEDynamicSystemState);
    sys->SetERP (erp);
  }
}

void csODEDynamics::SetGlobalCFM (float cfm)
{
  csODEDynamics::cfm = cfm;
  for (size_t i = 0; i < systems.Length(); i ++)
  {
    csRef<iODEDynamicSystemState> sys = SCF_QUERY_INTERFACE (systems[i],
      iODEDynamicSystemState);
    sys->SetCFM (cfm);
  }
}

void csODEDynamics::EnableStepFast (bool enable)
{
  stepfast = enable;
  quickstep = false;

  for (size_t i = 0; i < systems.Length(); i ++)
  {
    csRef<iODEDynamicSystemState> sys = SCF_QUERY_INTERFACE (systems[i],
      iODEDynamicSystemState);
    sys->EnableStepFast (enable);
  }
}

void csODEDynamics::SetStepFastIterations (int iter)
{
  sfiter = iter;

  for (size_t i = 0; i < systems.Length(); i ++)
  {
    csRef<iODEDynamicSystemState> sys = SCF_QUERY_INTERFACE (systems[i],
      iODEDynamicSystemState);
    sys->SetStepFastIterations (iter);
  }
}

void csODEDynamics::EnableQuickStep (bool enable)
{
  quickstep = enable;
  stepfast = false;

  for (size_t i = 0; i < systems.Length(); i ++)
  {
    csRef<iODEDynamicSystemState> sys = SCF_QUERY_INTERFACE (systems[i],
      iODEDynamicSystemState);
    sys->EnableQuickStep (enable);
  }
}

void csODEDynamics::SetQuickStepIterations (int iter)
{
  qsiter = iter;

  for (size_t i = 0; i < systems.Length(); i ++)
  {
    csRef<iODEDynamicSystemState> sys = SCF_QUERY_INTERFACE (systems[i],
      iODEDynamicSystemState);
    sys->SetQuickStepIterations (iter);
  }
}

void csODEDynamics::EnableEventProcessing (bool enable)
{
  if (enable && !process_events)
  {
    process_events = true;

    if (!scfiEventHandler)
      scfiEventHandler = csPtr<EventHandler> (new EventHandler (this));
    csRef<iEventQueue> q = CS_QUERY_REGISTRY (object_reg, iEventQueue);
    if (q)
      q->RegisterListener (scfiEventHandler, PreProcess);
  }
  else if (!enable && process_events)
  {
    process_events = false;

    if (scfiEventHandler)
    {
      csRef<iEventQueue> q = CS_QUERY_REGISTRY (object_reg, iEventQueue);
      if (q)
        q->RemoveListener (scfiEventHandler);
      scfiEventHandler = 0;
    }
  }
}

bool csODEDynamics::HandleEvent (iEvent& Event)
{
  if (Event.Name == PreProcess)
  {
    float stepsize = steptime;
    float elapsed_time = ((float)clock->GetElapsedTicks ())/1000.0;
    if (elapsed_time > limittime) elapsed_time = limittime;
    total_elapsed += elapsed_time;

    // TODO handle fractional total_remaining (interpolate render)
    while (total_elapsed > stepsize)
    {
      total_elapsed -= stepsize;
      for (size_t i=0; i<systems.Length(); i++)
      {
        systems.Get (i)->Step (stepsize);
        for (size_t j = 0; j < updates.Length(); j ++)
        {
          updates[i]->Execute (stepsize);
        }
        dJointGroupEmpty (contactjoints);
      }
    }
    return true;
  }
  return false;
}

csODEDynamicSystem::csODEDynamicSystem (float erp, float cfm)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiDynamicSystem);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiODEDynamicSystemState);

  //TODO: QUERY for collidesys

  worldID = dWorldCreate ();
  spaceID = dHashSpaceCreate (0);
  dWorldSetERP (worldID, erp);
  dWorldSetCFM (worldID, cfm);

  roll_damp = 1.0;
  lin_damp = 1.0;
  move_cb = (iDynamicsMoveCallback*)new csODEDefaultMoveCallback ();

  rateenabled = false;
  steptime = limittime = total_elapsed = 0.0;

  stepfast = false;
  sfiter = 10;
  quickstep = false;
  qsiter = 10;
  fastobjects = false;
  autodisable = true;
  dWorldSetAutoDisableFlag (worldID, autodisable);
}

csODEDynamicSystem::~csODEDynamicSystem ()
{
  // must delete all these before deleting the actual world
  colliders.DeleteAll ();
  joints.DeleteAll ();
  strict_joints.DeleteAll ();
  groups.DeleteAll ();
  bodies.DeleteAll ();

  dSpaceDestroy (spaceID);
  dWorldDestroy (worldID);

  SCF_DESTRUCT_EMBEDDED_IBASE (scfiODEDynamicSystemState);
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiDynamicSystem);
}


csPtr<iRigidBody> csODEDynamicSystem::CreateBody ()
{
  csODERigidBody* body = new csODERigidBody (this);
  bodies.Push (&body->scfiRigidBody);
  body->scfiRigidBody.SetMoveCallback(move_cb);
  return &body->scfiRigidBody;
}


void csODEDynamicSystem::RemoveBody (iRigidBody* body)
{
  bodies.Delete (body);
}
iRigidBody *csODEDynamicSystem::GetBody (unsigned int index)
{
  if ((unsigned)index < bodies.GetSize ())
    return bodies[index];
  else return 0;
}
iRigidBody *csODEDynamicSystem::FindBody (const char *name)
{
  return bodies.FindByName (name);
}

csPtr<iBodyGroup> csODEDynamicSystem::CreateGroup ()
{
  csODEBodyGroup* group = new csODEBodyGroup (this);
  groups.Push (group);
  return csPtr<iBodyGroup> (group);
}

void csODEDynamicSystem::RemoveGroup (iBodyGroup *group)
{
  groups.Delete ((csODEBodyGroup*)group);
}

csPtr<iJoint> csODEDynamicSystem::CreateJoint ()
{
  csODEJoint* joint = new csODEJoint (this);
  joints.Push (joint);
  return csPtr<iJoint> (joint);
}

csPtr<iODEUniversalJoint> csODEDynamicSystem::CreateUniversalJoint ()
{
  ODEUniversalJoint* joint = new ODEUniversalJoint (GetWorldID());
  strict_joints.Push (joint);
  return joint;
}

csPtr<iODESliderJoint> csODEDynamicSystem::CreateSliderJoint ()
{
  ODESliderJoint* joint = new ODESliderJoint (GetWorldID());
  strict_joints.Push (joint);
  return  csPtr<iODESliderJoint> (joint);
}

csPtr<iODEHingeJoint> csODEDynamicSystem::CreateHingeJoint ()
{
  ODEHingeJoint* joint = new ODEHingeJoint (GetWorldID());
  strict_joints.Push (joint);
  return  csPtr<iODEHingeJoint> (joint);
}

csPtr<iODEHinge2Joint> csODEDynamicSystem::CreateHinge2Joint ()
{
  ODEHinge2Joint* joint = new ODEHinge2Joint (GetWorldID());
  strict_joints.Push (joint);
  return  csPtr<iODEHinge2Joint> (joint);
}

csPtr<iODEAMotorJoint> csODEDynamicSystem::CreateAMotorJoint ()
{
  ODEAMotorJoint* joint = new ODEAMotorJoint (GetWorldID());
  strict_joints.Push (joint);
  return  csPtr<iODEAMotorJoint> (joint);
}

csPtr<iODEBallJoint> csODEDynamicSystem::CreateBallJoint ()
{
  ODEBallJoint* joint = new ODEBallJoint (GetWorldID());
  strict_joints.Push (joint);
  return  csPtr<iODEBallJoint> (joint);
}

void csODEDynamicSystem::RemoveJoint (iODEUniversalJoint *joint)
{
  strict_joints.Delete ((ODEUniversalJoint*)joint);
}
void csODEDynamicSystem::RemoveJoint (iODEHingeJoint *joint)
{
  strict_joints.Delete ((ODEHingeJoint*)joint);
}
void csODEDynamicSystem::RemoveJoint (iODEHinge2Joint *joint)
{
  strict_joints.Delete ((ODEHinge2Joint*)joint);
}
void csODEDynamicSystem::RemoveJoint (iODEAMotorJoint *joint)
{
  strict_joints.Delete ((ODEAMotorJoint*)joint);
}
void csODEDynamicSystem::RemoveJoint (iODEBallJoint *joint)
{
  strict_joints.Delete ((ODEBallJoint*)joint);
}
void csODEDynamicSystem::RemoveJoint (iODESliderJoint *joint)
{
  strict_joints.Delete ((ODESliderJoint*) joint);
}
void csODEDynamicSystem::RemoveJoint (iJoint *joint)
{
  joints.Delete ((csODEJoint*)joint);
}

void csODEDynamicSystem::SetGravity (const csVector3& v)
{
  dWorldSetGravity (worldID, v.x, v.y, v.z);
}

const csVector3 csODEDynamicSystem::GetGravity () const
{
  dVector3 grav;
  dWorldGetGravity (worldID, grav);
  return csVector3 (grav[0], grav[1], grav[2]);
}

void csODEDynamicSystem::Step (float elapsed_time)
{
  dSpaceCollide (spaceID, this, &csODEDynamics::NearCallback);
  float stepsize;
  if (rateenabled)
  {
    stepsize = steptime;
    if (elapsed_time > limittime) { elapsed_time = limittime; }
  }
  else
  {
    stepsize = elapsed_time;
  }
  total_elapsed += elapsed_time;

  // TODO handle fractional total_remaining (interpolate render)
  while (total_elapsed > stepsize)
  {
    total_elapsed -= stepsize;
    if (!stepfast)
    {
      if (!quickstep)
      {
        dWorldStep (worldID, stepsize);
      }
      else
      {
        dWorldQuickStep (worldID, stepsize);
      }
    }
    else
    {
      dWorldStepFast1 (worldID, stepsize, sfiter);
    }
    for (size_t i = 0; i < bodies.Length(); i ++)
    {
      iRigidBody *b = bodies.Get(i);
      // only do this if the body is enabled
      if (b->IsEnabled())
      {
        b->SetAngularVelocity (b->GetAngularVelocity () * roll_damp);
        b->SetLinearVelocity (b->GetLinearVelocity () * lin_damp);
      }
    }
    for (size_t j = 0; j < updates.Length(); j ++)
    {
      updates[j]->Execute (stepsize);
    }
  }

  for (size_t i=0; i<bodies.Length(); i++)
  {
    iRigidBody *b = bodies.Get(i);
    b->Update ();
  }
}

bool csODEDynamicSystem::AttachColliderMesh (iMeshWrapper* mesh,
                                             const csOrthoTransform& trans,
					     float friction, float elasticity,
					     float softness)
{
  csODECollider *odec = new csODECollider ();
  odec->SetElasticity (elasticity);
  odec->SetFriction (friction);
  odec->SetSoftness (softness);
  odec->CreateMeshGeometry (mesh);
  odec->SetTransform (trans);
  odec->AddToSpace (spaceID);
  colliders.Push (odec);

  return true;
}

bool csODEDynamicSystem::AttachColliderCylinder (float length, float radius,
                                                 const csOrthoTransform& trans,
						 float friction,
						 float elasticity,
						 float softness)
{
  csODECollider *odec = new csODECollider ();
  odec->SetElasticity (elasticity);
  odec->SetFriction (friction);
  odec->SetSoftness (softness);
  odec->CreateCCylinderGeometry (length, radius);
  odec->SetTransform (trans);
  odec->AddToSpace (spaceID);
  colliders.Push (odec);

  return true;
}

bool csODEDynamicSystem::AttachColliderBox (const csVector3 &size,
                                            const csOrthoTransform& trans, float friction, float elasticity, float softness)
{
  csODECollider *odec = new csODECollider ();
  //odec->SetElasticity (elasticity);
  //odec->SetFriction (friction);
  //odec->SetSoftness (softness);
  odec->CreateBoxGeometry (size);
  odec->SetTransform (csOrthoTransform(trans.GetO2T().GetTranspose(),trans.GetOrigin()));
  //odec->SetTransform (trans);
  odec->AddToSpace (spaceID);
  colliders.Push (odec);

  return true;
}

bool csODEDynamicSystem::AttachColliderSphere (float radius,
                                               const csVector3 &offset, float friction, 
                                               float elasticity, float softness)
{
  if (radius > 0) //otherwise ODE will treat radius as a 'bad argument'
  {
    csODECollider *odec = new csODECollider ();
    odec->SetElasticity (elasticity);
    odec->SetFriction (friction);
    odec->SetSoftness (softness);
    odec->CreateSphereGeometry (csSphere (offset, radius));
    odec->AddToSpace (spaceID);
    colliders.Push (odec);
    return true;
  }
  return false;
}
bool csODEDynamicSystem::AttachColliderPlane (const csPlane3 &plane,
                                              float friction, float elasticity, float softness)
{
  csODECollider *odec = new csODECollider ();
  odec->SetElasticity (elasticity);
  odec->SetFriction (friction);
  odec->SetSoftness (softness);
  odec->CreatePlaneGeometry (plane);
  odec->AddToSpace (spaceID);
  colliders.Push (odec);

  return true;
}

csRef<iDynamicsSystemCollider> csODEDynamicSystem::GetCollider (
	unsigned int index)
{
  if (index < colliders.GetSize ())
    return csRef<iDynamicsSystemCollider> (colliders[index]);
  else return 0;
}
csRef<iDynamicsSystemCollider> csODEDynamicSystem::CreateCollider ()
{
  csODECollider *odec = new csODECollider ();
  odec->AddToSpace (spaceID);
  csRef<iDynamicsSystemCollider> c = (csPtr<iDynamicsSystemCollider>) odec;
  colliders.Push (c);
  return c;
}
void csODEDynamicSystem::AttachCollider (iDynamicsSystemCollider* collider)
{
  ((csODECollider*)collider)->AddToSpace (spaceID);
  colliders.Push (collider);
}
void csODEDynamicSystem::EnableAutoDisable (bool enable)
{
  autodisable=enable;
  dWorldSetAutoDisableFlag (worldID, enable);
}

void csODEDynamicSystem::SetAutoDisableParams (float linear, float angular,
                                               int steps, float time)
{
  dWorldSetAutoDisableLinearThreshold (worldID, linear);
  dWorldSetAutoDisableAngularThreshold (worldID, angular);
  dWorldSetAutoDisableSteps (worldID, steps);
  dWorldSetAutoDisableTime (worldID, time);
}

void csODEDynamicSystem::SetContactMaxCorrectingVel (float v)
{
  dWorldSetContactMaxCorrectingVel (worldID, v);
}

float csODEDynamicSystem::GetContactMaxCorrectingVel ()
{
  return dWorldGetContactMaxCorrectingVel (worldID);
}

void csODEDynamicSystem::SetContactSurfaceLayer (float depth)
{
  dWorldSetContactSurfaceLayer(worldID, depth);
}

float csODEDynamicSystem::GetContactSurfaceLayer ()
{
  return dWorldGetContactSurfaceLayer(worldID);
}


csODEBodyGroup::csODEBodyGroup (csODEDynamicSystem* sys)
{
  SCF_CONSTRUCT_IBASE (0);
  system = sys;
}

csODEBodyGroup::~csODEBodyGroup ()
{
  for (size_t i = 0; i < bodies.Length(); i ++)
  {
    ((csODERigidBody *)(iRigidBody*)bodies[i])->UnsetGroup ();
  }
  SCF_DESTRUCT_IBASE();
}

void csODEBodyGroup::AddBody (iRigidBody *body)
{
  bodies.Push (body);
  ((csODERigidBody *)(body->QueryObject()))->SetGroup (this);
}

void csODEBodyGroup::RemoveBody (iRigidBody *body)
{
  bodies.Delete (body);
  ((csODERigidBody *)(body->QueryObject()))->UnsetGroup ();
}

bool csODEBodyGroup::BodyInGroup (iRigidBody *body)
{
  return bodies.Find (body) != csArrayItemNotFound;
}

//--------------------------csODECollider-------------------------------------
csODECollider::csODECollider ()
{
  SCF_CONSTRUCT_IBASE (0);
  surfacedata[0] = 0;
  surfacedata[1] = 0;
  surfacedata[2] = 0;
  density = 0;
  geomID = 0;
  spaceID = 0;
  coll_cb = 0;
  transformID = dCreateGeomTransform (0);
  dGeomTransformSetCleanup (transformID, 1);
  geom_type =  NO_GEOMETRY;
  is_static = true;
}
void csODECollider::SetCollisionCallback (
	iDynamicsColliderCollisionCallback* cb)
{
  coll_cb = cb;
}

void csODECollider::KillGeoms ()
{
  if (transformID)
  {
    if (dGeomTransformGetGeom (transformID))
      geomID = 0;

    dGeomDestroy (transformID);
  }
  if (geomID) dGeomDestroy (geomID);

  geomID = transformID = 0;
}

csODECollider::~csODECollider ()
{
  KillGeoms ();
  SCF_DESTRUCT_IBASE();
}

void csODECollider::MakeStatic ()
{
  is_static = true;
}

void csODECollider::MakeDynamic ()
{
  is_static = false;
}

bool csODECollider::IsStatic ()
{
  return is_static;
}

void csODECollider::Collision (csODECollider* other,
    const csVector3& pos, const csVector3& normal, float depth)
{
  if (coll_cb) coll_cb->Execute (this, other);
}

void csODECollider::Collision (iRigidBody* other,
    const csVector3& pos, const csVector3& normal, float depth)
{
  if (coll_cb) coll_cb->Execute (this, other);
}

void csODECollider::ClearContents ()
{
  KillGeoms ();

  transformID = dCreateGeomTransform (0);
  dGeomTransformSetCleanup (transformID, 1);
  geom_type =  NO_GEOMETRY;
}

void csODECollider::MassCorrection ()
{
  if (density > 0 && dGeomGetBody (transformID) && geomID)
  {
    dMass m, om;
    dMassSetZero (&m);

    switch (geom_type)
    {
    case BOX_COLLIDER_GEOMETRY:
      {
        dVector3 size;
        dGeomBoxGetLengths (geomID, size);
        dMassSetBox (&m, density, size[0], size[1], size[2]);
      }
      break;
    case SPHERE_COLLIDER_GEOMETRY:
      dMassSetSphere (&m, density, dGeomSphereGetRadius (geomID));
      break;
    case CYLINDER_COLLIDER_GEOMETRY:
      {
        dReal radius, length;
        dGeomCCylinderGetParams (geomID, &radius, &length);
        dMassSetCappedCylinder (&m, density, 3, radius, length);
      }
      break;
    case TRIMESH_COLLIDER_GEOMETRY:
      {
        // ODE doesn't have any function to get the mass of arbitrary triangles
        // so we will just use the OBB
        int tri_count = dGeomTriMeshGetTriangleCount (geomID);
        csOBB b;
        for (int i = 0; i < tri_count; i++)
        {
          dVector3 v0, v1, v2;
          dGeomTriMeshGetTriangle (geomID, i, &v0, &v1, &v2);
          b.AddBoundingVertex (csVector3 (v0[0], v0[1], v0[2]));
          b.AddBoundingVertex (csVector3 (v1[0], v1[1], v1[2]));
          b.AddBoundingVertex (csVector3 (v2[0], v2[1], v2[2]));
        }
        dMassSetBox (&m, density, b.MaxX()-b.MinX(), b.MaxY()-b.MinY(), b.MaxZ()-b.MinZ());
      }
    default:
      break;
    }

    const dReal *pos = dGeomGetPosition (geomID);
    dMassTranslate (&m, pos[0], pos[1], pos[2]);

    dMassRotate (&m, dGeomGetRotation (geomID));

    dBodyID bodyID = dGeomGetBody (transformID);
    dBodyGetMass (bodyID, &om);
    dMassAdd (&om, &m);
    dBodySetMass (bodyID, &om);
  }
}
void csODECollider::AttachBody (dBodyID bodyID)
{
  if (geom_type != PLANE_COLLIDER_GEOMETRY)
  {
    dGeomSetBody (transformID, bodyID);
    if (geomID)
    {
      MassCorrection ();
    }
  }
}
void csODECollider::SetDensity (float density)
{
  csODECollider::density = density;
  MassCorrection ();
}
bool csODECollider::CreateMeshGeometry (iMeshWrapper *mesh)
{
  dBodyID b = dGeomGetBody (transformID);
  ClearContents ();

  geom_type = TRIMESH_COLLIDER_GEOMETRY;

  // From Eroroman & Marc Rochel with modifications by Mike Handverger and Piotr Obrzut

  iPolygonMesh* p = mesh->GetMeshObject()->GetObjectModel()
  	->GetPolygonMeshColldet();

  if (!p || p->GetVertexCount () == 0 || p->GetTriangleCount () == 0)
  {
    csFPrintf(stderr, "csODECollider: No collision polygons, triangles or vertices on %s\n",mesh->QueryObject()->GetName());
    return false;
  }

  csTriangle *c_triangle = p->GetTriangles();
  int tr_num = p->GetTriangleCount();
  // Slight problem here is that we need to keep vertices and indices around
  // since ODE only uses the pointers. I am not sure if ODE cleans them up
  // on exit or not. If not, we need some way to keep track of all mesh colliders
  // and clean them up on destruct.
  float *vertices = new float[p->GetVertexCount()*3];
  int *indeces = new int[tr_num*3];
  csVector3 *c_vertex = p->GetVertices();
  //csFPrintf(stderr, "vertex count: %d\n", p->GetVertexCount());
  //csFPrintf(stderr, "triangles count: %d\n", tr_num);
  int i=0, j=0;
  for (i=0, j=0; i < p->GetVertexCount(); i++)
  {
    vertices[j++] = c_vertex[i].x;
    vertices[j++] = c_vertex[i].y;
    vertices[j++] = c_vertex[i].z;
    //csFPrintf(stderr, "vertex %d coords -> x=%.1f, y=%.1f, z=%.1f\n", i,c_vertex[i].x, c_vertex[i].y, c_vertex[i].z);
  }
  for (i=0, j=0; i < tr_num; i++)
  {
    indeces[j++] = c_triangle[i].a;
    indeces[j++] = c_triangle[i].b;
    indeces[j++] = c_triangle[i].c;
    //csFPrintf(stderr, "triangle %d -> a=%d, b=%d, c=%d\n", i,c_triangle[i].a, c_triangle[i].b, c_triangle[i].c);
  }
  dTriMeshDataID TriData = dGeomTriMeshDataCreate();

  dGeomTriMeshDataBuildSingle(TriData, vertices, 3*sizeof(float),
    p->GetVertexCount(), indeces, 3*tr_num, 3*sizeof(int));

  geomID = dCreateTriMesh(0, TriData, 0, 0, 0);

  GeomData *gd = new GeomData ();
  gd->surfacedata = surfacedata;
  gd->collider = this;
  dGeomSetData (geomID, (void*)gd);

  if (b)
  {
    AddTransformToSpace (spaceID);
    dGeomSetBody (transformID, b);
    MassCorrection ();
  } else if (spaceID) AddToSpace (spaceID);

  return true;
}
bool csODECollider::CreateCCylinderGeometry (float length, float radius)
{
  dBodyID b = dGeomGetBody (transformID);
  ClearContents ();

  geom_type = CYLINDER_COLLIDER_GEOMETRY;
  geomID = dCreateCCylinder (0, radius, length);

  GeomData *gd = new GeomData ();
  gd->surfacedata = surfacedata;
  gd->collider = this;
  dGeomSetData (geomID, (void*)gd);

  if (b)
  {
    AddTransformToSpace (spaceID);
    dGeomSetBody (transformID, b);
    MassCorrection ();
  } else if (spaceID) AddToSpace (spaceID);

  return true;
}
bool csODECollider::CreatePlaneGeometry (const csPlane3& plane)
{
  dBodyID b = dGeomGetBody (transformID);
  ClearContents ();

  geom_type = PLANE_COLLIDER_GEOMETRY;
  geomID = dCreatePlane (0, -plane.A(), -plane.B(), -plane.C(), plane.D());

  GeomData *gd = new GeomData ();
  gd->surfacedata = surfacedata;
  gd->collider = this;
  dGeomSetData (geomID, (void*)gd);

  if (b)
  {
    AddTransformToSpace (spaceID);
    dGeomSetBody (transformID, b);
    MassCorrection ();
  } else if (spaceID) AddToSpace (spaceID);

  return true;
}
bool csODECollider::CreateSphereGeometry (const csSphere& sphere)
{
  if (sphere.GetRadius () > 0) //otherwise ODE will treat radius as a 'bad argument'
  {
    dBodyID b = dGeomGetBody (transformID);
    ClearContents ();

    geom_type = SPHERE_COLLIDER_GEOMETRY;
    geomID = dCreateSphere (0, sphere.GetRadius ());

    csVector3 offset = sphere.GetCenter ();
    dGeomSetPosition (transformID, offset.x, offset.y, offset.z);

    GeomData *gd = new GeomData ();
    gd->surfacedata = surfacedata;
    gd->collider = this;
    dGeomSetData (geomID, (void*)gd);

    if (b)
    {
      if (spaceID) AddTransformToSpace (spaceID);
      dGeomSetBody (transformID, b);
      MassCorrection ();
    } else if (spaceID) AddToSpace (spaceID);

    return true;
  }

  return false;
}
bool csODECollider::CreateBoxGeometry (const csVector3& size)
{
  dBodyID b = dGeomGetBody (transformID);
  ClearContents ();

  geom_type = BOX_COLLIDER_GEOMETRY;
  geomID = dCreateBox (0, size.x, size.y, size.z);

  GeomData *gd = new GeomData ();
  gd->surfacedata = surfacedata;
  gd->collider = this;
  dGeomSetData (geomID, (void*)gd);

  if (b)
  {
    AddTransformToSpace (spaceID);
    dGeomSetBody (transformID, b);
    MassCorrection ();
  } else if (spaceID) AddToSpace (spaceID);

  return true;
}

void csODECollider::CS2ODEMatrix (const csMatrix3& csmat, dMatrix3& odemat)
{
  CS2ODEMATRIX(csmat,odemat);
}
void csODECollider::ODE2CSMatrix (const dReal* odemat, csMatrix3& csmat)
{
  ODE2CSMATRIX(odemat,csmat);
}
void csODECollider::SetTransform (const csOrthoTransform& transform)
{
  // can't set plane's transform b/c it causes non placeable geom run-time
  // error w/debug build of ode
  if (!geomID || geom_type == PLANE_COLLIDER_GEOMETRY)
    return;

  csVector3 pos = transform.GetOrigin ();
  dGeomSetPosition (geomID, pos.x, pos.y, pos.z);

  dMatrix3 rot;
  CS2ODEMatrix (transform.GetO2T (), rot);
  dGeomSetRotation (geomID, rot);

  const dReal *gv = dGeomGetPosition (geomID);
  csVector3 g_pos (gv[0], gv[1], gv[2]);


  if (dGeomGetBody (transformID))
    MassCorrection ();
}
csOrthoTransform csODECollider::GetLocalTransform ()
{
  const dReal *tv = dGeomGetPosition (geomID);
  csVector3 t_pos (tv[0], tv[1], tv[2]);

  csMatrix3 t_rot;
  ODE2CSMatrix (dGeomGetRotation (geomID), t_rot);

  return csOrthoTransform (t_rot, t_pos);
}
csOrthoTransform csODECollider::GetTransform ()
{
  const dReal *tv = dGeomGetPosition (transformID);
  csVector3 t_pos (tv[0], tv[1], tv[2]);

  csMatrix3 t_rot;
  ODE2CSMatrix (dGeomGetRotation (transformID), t_rot);

  csOrthoTransform t_transf (t_rot, t_pos);

  if (!geomID || geom_type == PLANE_COLLIDER_GEOMETRY)
  {
    return t_transf;
  }

  const dReal *gv = dGeomGetPosition (geomID);
  csVector3 g_pos (gv[0], gv[1], gv[2]);

  csMatrix3 g_rot;
  ODE2CSMatrix (dGeomGetRotation (geomID), g_rot);

  return t_transf * csOrthoTransform (g_rot, g_pos);
}
void csODECollider::AddTransformToSpace (dSpaceID spaceID)
{
  csODECollider::spaceID = spaceID;
  if (geomID)
  {
    dSpaceID gspace = dGeomGetSpace (geomID);
    if (gspace)
      dSpaceRemove (gspace, geomID);
  }
  if (geomID && !dGeomTransformGetGeom(transformID))
    dGeomTransformSetGeom (transformID, geomID);
  dSpaceID prev = dGeomGetSpace (transformID);
  if (prev)dSpaceRemove (prev, transformID);
  if (geomID) dSpaceAdd (spaceID, transformID);
}
void csODECollider::AddToSpace (dSpaceID spaceID)
{
  if (geomID)
  {
    dSpaceID prev = dGeomGetSpace (geomID);
    if (prev)dSpaceRemove (prev, geomID);
    dSpaceAdd (spaceID, geomID);
  }
  csODECollider::spaceID = spaceID;
}
bool csODECollider::GetBoxGeometry (csVector3& size)
{
  if (geom_type == BOX_COLLIDER_GEOMETRY)
  {
    dVector3 s;
    dGeomBoxGetLengths (geomID, s);
    size.x = s[0];
    size.y = s[1];
    size.z = s[2];
    return true;
  }
  return false;
}
bool csODECollider::GetSphereGeometry (csSphere& sphere)
{
  if (geom_type == SPHERE_COLLIDER_GEOMETRY)
  {
    sphere = csSphere (csVector3 (0), dGeomSphereGetRadius (geomID));
    return true;
  }
  return false;
}
bool csODECollider::GetPlaneGeometry (csPlane3& plane)
{
  if (geom_type == PLANE_COLLIDER_GEOMETRY)
  {
    dVector4 p;
    dGeomPlaneGetParams (geomID, p);
    plane = csPlane3 (-p[0], -p[1], -p[2], p[3]);
    return true;
  }
  return false;
}
bool csODECollider::GetCylinderGeometry (float& length, float& radius)
{
  if (geom_type == CYLINDER_COLLIDER_GEOMETRY)
  {
    dReal odeR, odeL;
    dGeomCCylinderGetParams (geomID, &odeR, &odeL);
    radius = odeR;
    length = odeL;
    return true;
  }
  return false;
}
void csODECollider::FillWithColliderGeometry (csRef<iGeneralFactoryState> genmesh_fact)
{
  switch (geom_type)
  {
  case BOX_COLLIDER_GEOMETRY:
    {
      dVector3 size;
      dGeomBoxGetLengths (geomID, size);

      csBox3 box (csVector3 (0));
      box.SetSize (csVector3 (size[0], size[1], size[2]));
      genmesh_fact->GenerateBox (box);
      genmesh_fact->CalculateNormals ();
    }
    break;
  case SPHERE_COLLIDER_GEOMETRY:
    {
      float r = dGeomSphereGetRadius (geomID);
      csEllipsoid sphere (csVector3 (0), csVector3 (r, r, r));
      genmesh_fact->GenerateSphere (sphere, 30);
      genmesh_fact->CalculateNormals ();
    }
  case PLANE_COLLIDER_GEOMETRY:
    {
      //dVector4 plane;
      //dGeomPlaneGetParams (geomID, plane);
      //genmesh_fact->GeneratePlane (csPlane3 (-plane[0], -plane[1], -plane[2], plane[3]),
      //  csBox3 (csVector3 (-CS_BOUNDINGBOX_MAXVALUE),csVector3 (CS_BOUNDINGBOX_MAXVALUE)));
    }
    break;
  case TRIMESH_COLLIDER_GEOMETRY:
    {
      int tri_count = dGeomTriMeshGetTriangleCount (geomID);
      genmesh_fact->SetVertexCount (tri_count * 3);
      genmesh_fact->SetTriangleCount (tri_count);
      csVector3* vertices = genmesh_fact->GetVertices ();
      csTriangle* triangles = genmesh_fact->GetTriangles ();

      for (int i = 0; i < tri_count; i++)
      {
        dVector3 v0, v1, v2;
        dGeomTriMeshGetTriangle (geomID, i, &v0, &v1, &v2);
        vertices[i*3] = csVector3 (v0[0], v0[1], v0[2]);
        vertices[i*3+1] = csVector3 (v1[0], v1[1], v1[2]);
        vertices[i*3+2] = csVector3 (v2[0], v2[1], v2[2]);
        triangles[i].c = i*3; triangles[i].b = i*3+1; triangles[i].a = i*3+2;
      }
      genmesh_fact->CalculateNormals ();
    }
    break;
  default:
    break;
  }
}
//--------------------------csODERigidBody-------------------------------------
csODERigidBody::csODERigidBody (csODEDynamicSystem* sys)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiRigidBody);

  dynsys = sys;

  bodyID = dBodyCreate (dynsys->GetWorldID());
  dBodySetData (bodyID, this);
  groupID = dSimpleSpaceCreate (dynsys->GetSpaceID ());
  statjoint = 0;
  collision_group = 0;
}

csODERigidBody::~csODERigidBody ()
{
  colliders.DeleteAll ();
  dSpaceDestroy (groupID);
  dBodyDestroy (bodyID);

  SCF_DESTRUCT_EMBEDDED_IBASE (scfiRigidBody);
}


void DestroyGeoms( csGeomList & geoms )
{
  dGeomID tempID;
  size_t i=0;

  for (;i < geoms.Length(); i++)
  {
    tempID = geoms[i];
    if (dGeomGetClass (geoms[i]) == dGeomTransformClass)
      tempID = dGeomTransformGetGeom (geoms[i]);

    GeomData *gd = (GeomData *)dGeomGetData (tempID);
    delete  gd;

    if( dGeomGetClass (tempID) == csODEDynamics::GetGeomClassNum() )
    {
      //MeshInfo *gdata = (MeshInfo*)dGeomGetClassData (tempID);
    }

    //for transform geoms, only need to destroy the container,
    //they've been set herein to destroy their contained geom
    dGeomDestroy (geoms[i]);
  }
}


bool csODERigidBody::MakeStatic ()
{
  if (statjoint == 0)
  {
    statjoint = dJointCreateFixed (dynsys->GetWorldID(), 0);
    dJointAttach (statjoint, bodyID, 0);
    dJointSetFixed(statjoint);
    dBodySetGravityMode (bodyID, 0);
  }
  return true;
}

bool csODERigidBody::MakeDynamic ()
{
  if (statjoint != 0)
  {
    dJointDestroy (statjoint);
    dBodySetGravityMode (bodyID, 1);
    statjoint = 0;
  }
  return true;
}

bool csODERigidBody::Disable ()
{
  dBodyDisable (bodyID);
  return true;
}

bool csODERigidBody::Enable ()
{
  dBodyEnable (bodyID);
  return true;
}

bool csODERigidBody::IsEnabled ()
{
  return (dBodyIsEnabled (bodyID) != 0);
}

void csODERigidBody::SetGroup(iBodyGroup *group)
{
  if (collision_group)
  {
    collision_group->RemoveBody (&scfiRigidBody);
  }
  collision_group = group;
}

bool csODERigidBody::AttachColliderMesh (iMeshWrapper *mesh,
                                         const csOrthoTransform &trans,
					 float friction, float density,
                                         float elasticity, float softness)
{
  csODECollider *odec = new csODECollider ();
  odec->SetElasticity (elasticity);
  odec->SetFriction (friction);
  odec->SetSoftness (softness);
  odec->SetDensity (density);
  odec->CreateMeshGeometry (mesh);
  odec->SetTransform (trans);
  odec->AttachBody (bodyID);
  odec->AddTransformToSpace (groupID);
  odec->MakeDynamic ();
  colliders.Push (odec);

  return true;
}

bool csODERigidBody::AttachColliderCylinder (float length, float radius,
                                             const csOrthoTransform& trans,
					     float friction, float density,
                                             float elasticity, float softness)
{
  csODECollider *odec = new csODECollider ();
  odec->SetElasticity (elasticity);
  odec->SetFriction (friction);
  odec->SetSoftness (softness);
  odec->SetDensity (density);
  odec->CreateCCylinderGeometry (length, radius);
  odec->SetTransform (trans);
  odec->AttachBody (bodyID);
  odec->AddTransformToSpace (groupID);
  odec->MakeDynamic ();
  colliders.Push (odec);

  return true;
}

bool csODERigidBody::AttachColliderBox (const csVector3 &size,
                                        const csOrthoTransform& trans,
					float friction, float density,
                                        float elasticity, float softness)
{
  csODECollider *odec = new csODECollider ();
  odec->SetElasticity (elasticity);
  odec->SetFriction (friction);
  odec->SetSoftness (softness);
  odec->SetDensity (density);
  odec->CreateBoxGeometry (size);
  odec->AttachBody (bodyID);
  odec->SetTransform (trans);
  odec->AddTransformToSpace (groupID);
  odec->MakeDynamic ();
  colliders.Push (odec);

  return true;
}

bool csODERigidBody::AttachColliderSphere (float radius,
					   const csVector3 &offset,
                                           float friction, float density,
					   float elasticity, float softness)
{
  if (radius > 0) //otherwise ODE will treat radius as a 'bad argument'
  {
    csODECollider *odec = new csODECollider ();
    odec->SetElasticity (elasticity);
    odec->SetFriction (friction);
    odec->SetSoftness (softness);
    odec->SetDensity (density);
    odec->CreateSphereGeometry (csSphere (offset, radius));
    odec->AttachBody (bodyID);
    odec->SetTransform (csReversibleTransform (csMatrix3 (), offset));
    odec->AddTransformToSpace (groupID);
    odec->MakeDynamic ();
    colliders.Push (odec);
    return true;
  }
  return false;
}

bool csODERigidBody::AttachColliderPlane (const csPlane3& plane,
                                          float friction, float density,
					  float elasticity, float softness)
{
  csODECollider *odec = new csODECollider ();
  odec->SetElasticity (elasticity);
  odec->SetFriction (friction);
  odec->SetSoftness (softness);
  odec->SetDensity (density);
  odec->CreatePlaneGeometry (plane);
  colliders.Push (odec);
  //causes non placeable geom run-time error w/debug build of ode.
  //odec->AttachBody (bodyID);
  odec->MakeDynamic ();
  odec->AddToSpace (dynsys->GetSpaceID());

  return true;
}

void csODERigidBody::AttachCollider (iDynamicsSystemCollider* collider)
{
  dynsys->DestroyCollider (collider);
  if (collider->GetGeometryType () == PLANE_COLLIDER_GEOMETRY)
    ((csODECollider*) collider)->AddToSpace (dynsys->GetSpaceID());
  else
    ((csODECollider*) collider)->AddTransformToSpace (groupID);

  ((csODECollider*) collider)->AttachBody (bodyID);
  collider->MakeDynamic ();
  colliders.Push (collider);
}

void csODERigidBody::SetPosition (const csVector3& pos)
{
  dBodySetPosition (bodyID, pos.x, pos.y, pos.z);
  if (statjoint != 0) dJointSetFixed(statjoint);
}

const csVector3 csODERigidBody::GetPosition () const
{
  const dReal* pos = dBodyGetPosition (bodyID);
  return csVector3 (pos[0], pos[1], pos[2]);
}

void csODERigidBody::SetOrientation (const csMatrix3& rot)
{
  dMatrix3 mat;
  CS2ODEMATRIX(rot,mat);
  dBodySetRotation (bodyID, mat);
}

const csMatrix3 csODERigidBody::GetOrientation () const
{
  const dReal* mat = dBodyGetRotation (bodyID);
  csMatrix3 rot;
  ODE2CSMATRIX(mat,rot);
  return rot;
}

void csODERigidBody::SetTransform (const csOrthoTransform& trans)
{
  csVector3 pos = trans.GetOrigin ();
  dBodySetPosition (bodyID, pos.x, pos.y, pos.z);
  csMatrix3 rot = trans.GetO2T ();
  dMatrix3 mat;
  CS2ODEMATRIX(rot,mat);
  dBodySetRotation (bodyID, mat);
}

const csOrthoTransform csODERigidBody::GetTransform () const
{
  const dReal* pos = dBodyGetPosition (bodyID);
  const dReal* mat = dBodyGetRotation (bodyID);
  csMatrix3 rot;
  ODE2CSMATRIX(mat,rot);
  return csOrthoTransform (rot, csVector3 (pos[0], pos[1], pos[2]));
}

void csODERigidBody::SetLinearVelocity (const csVector3& vel)
{
  Enable ();
  dBodySetLinearVel (bodyID, vel.x, vel.y, vel.z);
}

const csVector3 csODERigidBody::GetLinearVelocity () const
{
  const dReal* vel = dBodyGetLinearVel (bodyID);
  return csVector3 (vel[0], vel[1], vel[2]);
}

void csODERigidBody::SetAngularVelocity (const csVector3& vel)
{
  Enable ();
  dBodySetAngularVel (bodyID, vel.x, vel.y, vel.z);
}

const csVector3 csODERigidBody::GetAngularVelocity () const
{
  const dReal* vel = dBodyGetAngularVel (bodyID);
  return csVector3 (vel[0], vel[1], vel[2]);
}

void csODERigidBody::SetProperties (float mass,
                                    const csVector3& center, const csMatrix3& inertia)
{
  dMass* m = new dMass;
  m->mass = mass;
  m->c[0] = center.x; m->c[1] = center.y; m->c[2] = center.z; m->c[3] = 0;
  m->I[0] = inertia.m11; m->I[1] = inertia.m12; m->I[2] = inertia.m13; m->I[3] = 0;
  m->I[4] = inertia.m21; m->I[5] = inertia.m22; m->I[6] = inertia.m23; m->I[7] = 0;
  m->I[8] = inertia.m31; m->I[9] = inertia.m32; m->I[10] = inertia.m33; m->I[11] = 0;
  dBodySetMass (bodyID, m);
}

void csODERigidBody::GetProperties (float* mass,
                                    csVector3* center, csMatrix3* inertia)
{
  dMass m;
  dBodyGetMass (bodyID, &m);
  if (mass != 0) *mass = m.mass;
  if (center != 0) center->Set (m.c[0], m.c[1], m.c[2]);
  if (inertia != 0)
  {
    inertia->Set (m.I[0], m.I[1], m.I[2],
      m.I[4], m.I[5], m.I[6],
      m.I[8], m.I[9], m.I[10]);
  }
}
float csODERigidBody::GetMass ()
{
  dMass m;
  dBodyGetMass (bodyID, &m);
  return m.mass;
}
csVector3 csODERigidBody::GetCenter ()
{
  dMass m;
  dBodyGetMass (bodyID, &m);
  return csVector3 (m.c[0], m.c[1], m.c[2]);
}
csMatrix3 csODERigidBody::GetInertia ()
{
  dMass m;
  dBodyGetMass (bodyID, &m);
  return csMatrix3 (m.I[0], m.I[1], m.I[2],
    m.I[4], m.I[5], m.I[6],
    m.I[8], m.I[9], m.I[10]);
}
void csODERigidBody::AdjustTotalMass (float targetmass)
{
  dMass m;
  dBodyGetMass (bodyID, &m);
  dMassAdjust (&m, targetmass);
  dBodySetMass (bodyID, &m);
}

void csODERigidBody::AddForce (const csVector3& force)
{
  Enable ();
  dBodyAddForce (bodyID, force.x, force.y, force.z);
}

void csODERigidBody::AddTorque (const csVector3& force)
{
  Enable ();
  dBodyAddTorque (bodyID, force.x, force.y, force.z);
}

void csODERigidBody::AddRelForce (const csVector3& force)
{
  Enable ();
  dBodyAddRelForce (bodyID, force.x, force.y, force.z);
}

void csODERigidBody::AddRelTorque (const csVector3& force)
{
  Enable ();
  dBodyAddRelTorque (bodyID, force.x, force.y, force.z);
}

void csODERigidBody::AddForceAtPos (const csVector3& force,
                                    const csVector3& pos)
{
  Enable ();
  dBodyAddForceAtPos (bodyID, force.x, force.y, force.z, pos.x, pos.y, pos.z);
}

void csODERigidBody::AddForceAtRelPos (const csVector3& force,
                                       const csVector3& pos)
{
  Enable ();
  dBodyAddForceAtRelPos (bodyID, force.x, force.y, force.z,
    pos.x, pos.y, pos.z);
}

void csODERigidBody::AddRelForceAtPos (const csVector3& force,
                                       const csVector3& pos)
{
  Enable ();
  dBodyAddRelForceAtPos (bodyID, force.x, force.y, force.z,
    pos.x, pos.y, pos.z);
}

void csODERigidBody::AddRelForceAtRelPos (const csVector3& force,
                                          const csVector3& pos)
{
  Enable ();
  dBodyAddRelForceAtRelPos (bodyID, force.x, force.y, force.z,
    pos.x, pos.y, pos.z);
}

const csVector3 csODERigidBody::GetForce () const
{
  const dReal* force = dBodyGetForce (bodyID);
  return csVector3 (force[0], force[1], force[2]);
}

const csVector3 csODERigidBody::GetTorque () const
{
  const dReal* force = dBodyGetTorque (bodyID);
  return csVector3 (force[0], force[1], force[2]);
}

void csODERigidBody::AttachMesh (iMeshWrapper* m)
{
  mesh = m;
}

void csODERigidBody::AttachLight (iLight* m)
{
  light = m;
}

void csODERigidBody::AttachCamera (iCamera* m)
{
  camera = m;
}

void csODERigidBody::SetMoveCallback (iDynamicsMoveCallback* cb)
{
  move_cb = cb;
}

void csODERigidBody::SetCollisionCallback (iDynamicsCollisionCallback* cb)
{
  coll_cb = cb;
}

void csODERigidBody::Collision (iRigidBody *other,
    const csVector3& pos, const csVector3& normal, float depth)
{
  if (coll_cb) coll_cb->Execute (&scfiRigidBody, other,
      pos, normal, depth);
}

void csODERigidBody::Update ()
{
  if (bodyID && move_cb)
  {
    csOrthoTransform trans;
    trans = GetTransform ();
    if (mesh) move_cb->Execute (mesh, trans);
    if (light) move_cb->Execute (light, trans);
    if (camera) move_cb->Execute (camera, trans);
    /* remainder case for all other callbacks */
    move_cb->Execute (trans);
  }
}
csRef<iDynamicsSystemCollider> csODERigidBody::GetCollider (unsigned int index)
{
  if (index < colliders.GetSize ())
    return csRef<iDynamicsSystemCollider> (colliders[index]);
  else return 0;
}
//-----------------------csStrictODEJoint-------------------------------------
void csStrictODEJoint::Attach (iRigidBody *b1, iRigidBody *b2)
{
  if (b1)
  {
    bodyID[0] = ((csODERigidBody *)(b1->QueryObject()))->GetID();
  }
  else
  {
    bodyID[0] = 0;
  }
  if (b2)
  {
    bodyID[1] = ((csODERigidBody *)(b2->QueryObject()))->GetID();
  }
  else
  {
    bodyID[1] = 0;
  }

  body[0] = b1;
  body[1] = b2;
  dJointAttach (jointID, bodyID[0], bodyID[1]);
}
csRef<iRigidBody> csStrictODEJoint::GetAttachedBody (int b)
{
  return (b == 0) ? body[0] : body[1];
}
void  csStrictODEJoint::SetParam (int joint_type, int parameter, int axis, float value)
{
  int param = parameter;

  switch (axis)
  {
  case 1:
    switch (param)
    {
    case dParamLoStop:
      param = dParamLoStop2;
      break;
    case dParamHiStop:
      param = dParamHiStop2;
      break;
    case dParamVel:
      param = dParamVel2;
      break;
    case dParamFMax:
      param = dParamFMax2;
      break;
    case dParamFudgeFactor:
      param = dParamFudgeFactor2;
      break;
    case dParamBounce:
      param = dParamBounce2;
      break;
    case dParamCFM:
      param = dParamCFM2;
      break;
    case dParamStopERP:
      param = dParamStopERP2;
      break;
    case dParamStopCFM:
      param = dParamStopCFM2;
      break;
    case dParamSuspensionERP:
      param = dParamSuspensionERP2;
      break;
    case dParamSuspensionCFM:
      param = dParamSuspensionCFM2;
      break;
    }
    break;

  case 2:
    switch (param)
    {
    case dParamLoStop:
      param = dParamLoStop3;
      break;
    case dParamHiStop:
      param = dParamHiStop3;
      break;
    case dParamVel:
      param = dParamVel3;
      break;
    case dParamFMax:
      param = dParamFMax3;
      break;
    case dParamFudgeFactor:
      param = dParamFudgeFactor3;
      break;
    case dParamBounce:
      param = dParamBounce3;
      break;
    case dParamCFM:
      param = dParamCFM3;
      break;
    case dParamStopERP:
      param = dParamStopERP3;
      break;
    case dParamStopCFM:
      param = dParamStopCFM3;
      break;
    case dParamSuspensionERP:
      param = dParamSuspensionERP3;
      break;
    case dParamSuspensionCFM:
      param = dParamSuspensionCFM3;
      break;
    }
    break;
  default:
    ; // do nothing
  }

  switch (joint_type)
  {
  case CS_ODE_JOINT_TYPE_HINGE:
    dJointSetHingeParam (jointID, param, value);
    break;
  case CS_ODE_JOINT_TYPE_SLIDER:
    dJointSetSliderParam (jointID, param, value);
    break;
  case CS_ODE_JOINT_TYPE_HINGE2:
    dJointSetHinge2Param (jointID, param, value);
    break;
  case CS_ODE_JOINT_TYPE_AMOTOR:
    dJointSetAMotorParam (jointID, param, value);
    break;
  case CS_ODE_JOINT_TYPE_UNIVERSAL:
    dJointSetUniversalParam (jointID, param, value);
    break;
  default:
    ; // do nothing
  }
}

float csStrictODEJoint::GetParam (int joint_type, int parameter, int axis)
{
  int param = parameter;

  switch (axis)
  {
  case 1:
    switch (param)
    {
    case dParamLoStop:
      param = dParamLoStop2;
      break;
    case dParamHiStop:
      param = dParamHiStop2;
      break;
    case dParamVel:
      param = dParamVel2;
      break;
    case dParamFMax:
      param = dParamFMax2;
      break;
    case dParamFudgeFactor:
      param = dParamFudgeFactor2;
      break;
    case dParamBounce:
      param = dParamBounce2;
      break;
    case dParamCFM:
      param = dParamCFM2;
      break;
    case dParamStopERP:
      param = dParamStopERP2;
      break;
    case dParamStopCFM:
      param = dParamStopCFM2;
      break;
    case dParamSuspensionERP:
      param = dParamSuspensionERP2;
      break;
    case dParamSuspensionCFM:
      param = dParamSuspensionCFM2;
      break;
    }
    break;

  case 2:
    switch (param)
    {
    case dParamLoStop:
      param = dParamLoStop3;
      break;
    case dParamHiStop:
      param = dParamHiStop3;
      break;
    case dParamVel:
      param = dParamVel3;
      break;
    case dParamFMax:
      param = dParamFMax3;
      break;
    case dParamFudgeFactor:
      param = dParamFudgeFactor3;
      break;
    case dParamBounce:
      param = dParamBounce3;
      break;
    case dParamCFM:
      param = dParamCFM3;
      break;
    case dParamStopERP:
      param = dParamStopERP3;
      break;
    case dParamStopCFM:
      param = dParamStopCFM3;
      break;
    case dParamSuspensionERP:
      param = dParamSuspensionERP3;
      break;
    case dParamSuspensionCFM:
      param = dParamSuspensionCFM3;
      break;
    }
    break;
  default:
    ; // do nothing
  }

  switch (joint_type)
  {
  case CS_ODE_JOINT_TYPE_HINGE:
    return dJointGetHingeParam (jointID, param);
    break;
  case CS_ODE_JOINT_TYPE_SLIDER:
    return dJointGetSliderParam (jointID, param);
    break;
  case CS_ODE_JOINT_TYPE_HINGE2:
    return dJointGetHinge2Param (jointID, param);
    break;
  case CS_ODE_JOINT_TYPE_AMOTOR:
    return dJointGetAMotorParam (jointID, param);
    break;
  case CS_ODE_JOINT_TYPE_UNIVERSAL:
    return dJointGetUniversalParam (jointID, param);
    break;
  default:
    return 0.0;
  }
}

void csStrictODEJoint::CreateFeedback ()
{
  feedback = new dJointFeedback ();

  feedback->f1[0] = 0;
  feedback->f1[1] = 0;
  feedback->f1[2] = 0;

  feedback->f2[0] = 0;
  feedback->f2[1] = 0;
  feedback->f2[2] = 0;

  feedback->t1[0] = 0;
  feedback->t1[1] = 0;
  feedback->t1[2] = 0;

  feedback->t2[0] = 0;
  feedback->t2[1] = 0;
  feedback->t2[2] = 0;

  dJointSetFeedback (jointID, feedback);
}

csVector3 csStrictODEJoint::GetFeedbackForce1 ()
{
  if (feedback == 0) CreateFeedback ();
  return csVector3(feedback->f1[0], feedback->f1[1], feedback->f1[2]);
}

csVector3 csStrictODEJoint::GetFeedbackTorque1 ()
{
  if (feedback == 0) CreateFeedback ();
  return csVector3(feedback->f2[0], feedback->f2[1], feedback->f2[2]);
}

csVector3 csStrictODEJoint::GetFeedbackForce2 ()
{
  if (feedback == 0) CreateFeedback ();
  return csVector3(feedback->t1[0], feedback->t1[1], feedback->t1[2]);
}

csVector3 csStrictODEJoint::GetFeedbackTorque2 ()
{
  if (feedback == 0) CreateFeedback ();
  return csVector3(feedback->t2[0], feedback->t2[1], feedback->t2[2]);
}

//-------------------------------------------------------------------------------
ODESliderJoint::ODESliderJoint (dWorldID w_id)
{
  SCF_CONSTRUCT_IBASE (0);
  jointID = dJointCreateSlider (w_id, 0);
}
ODESliderJoint::~ODESliderJoint ()
{
  SCF_DESTRUCT_IBASE();
  dJointDestroy (jointID);
}
csVector3 ODESliderJoint::GetSliderAxis ()
{
  dVector3 pos;
  dJointGetSliderAxis (jointID, pos);
  return csVector3 (pos[0], pos[1], pos[2]);
}
//-------------------------------------------------------------------------------
ODEUniversalJoint::ODEUniversalJoint (dWorldID w_id)
{
  SCF_CONSTRUCT_IBASE (0);
  jointID = dJointCreateUniversal (w_id, 0);
}
ODEUniversalJoint::~ODEUniversalJoint ()
{
  SCF_DESTRUCT_IBASE();
  dJointDestroy (jointID);
}
csVector3 ODEUniversalJoint::GetUniversalAnchor1 ()
{
  dVector3 pos;
  dJointGetUniversalAnchor (jointID, pos);
  return csVector3 (pos[0], pos[1], pos[2]);
}
csVector3 ODEUniversalJoint::GetUniversalAnchor2 ()
{
  dVector3 pos;
  dJointGetUniversalAnchor2 (jointID, pos);
  return csVector3 (pos[0], pos[1], pos[2]);
}
csVector3 ODEUniversalJoint::GetUniversalAxis1 ()
{
  dVector3 pos;
  dJointGetUniversalAxis1 (jointID, pos);
  return csVector3 (pos[0], pos[1], pos[2]);
}
csVector3 ODEUniversalJoint::GetUniversalAxis2 ()
{
  dVector3 pos;
  dJointGetUniversalAxis2 (jointID, pos);
  return csVector3 (pos[0], pos[1], pos[2]);
}
//-------------------------------------------------------------------------------

ODEAMotorJoint::ODEAMotorJoint (dWorldID w_id)
{
  SCF_CONSTRUCT_IBASE (0);
  jointID = dJointCreateAMotor (w_id, 0);
}

ODEAMotorJoint::~ODEAMotorJoint ()
{
  SCF_DESTRUCT_IBASE();
  dJointDestroy (jointID);
}

void ODEAMotorJoint::SetAMotorMode (ODEAMotorMode mode)
{
  if ((mode <= CS_ODE_AMOTOR_MODE_UNKNOWN)
    || (mode >= CS_ODE_AMOTOR_MODE_LAST))
    return;

  static const int ODEAMotorModeTodAMotor[CS_ODE_AMOTOR_MODE_LAST] =
  {dAMotorUser, dAMotorEuler};

  dJointSetAMotorMode (jointID, ODEAMotorModeTodAMotor[mode]);
}

ODEAMotorMode ODEAMotorJoint::GetAMotorMode ()
{
  switch (dJointGetAMotorMode (jointID))
  {
  case dAMotorUser: return CS_ODE_AMOTOR_MODE_USER;
  case dAMotorEuler: return CS_ODE_AMOTOR_MODE_EULER;
  }
  return CS_ODE_AMOTOR_MODE_UNKNOWN;
}

csVector3 ODEAMotorJoint::GetAMotorAxis (int axis_num)
{
  dVector3 pos;
  dJointGetAMotorAxis (jointID, axis_num, pos);
  return csVector3 (pos[0], pos[1], pos[2]);
}
//-------------------------------------------------------------------------------

ODEHinge2Joint::ODEHinge2Joint (dWorldID w_id)
{
  SCF_CONSTRUCT_IBASE (0);
  jointID = dJointCreateHinge2 (w_id, 0);
}

ODEHinge2Joint::~ODEHinge2Joint ()
{
  SCF_DESTRUCT_IBASE();
  dJointDestroy (jointID);
}

csVector3 ODEHinge2Joint::GetHingeAnchor1 ()
{
  dVector3 pos;
  dJointGetHinge2Anchor (jointID, pos);
  return csVector3 (pos[0], pos[1], pos[2]);
}

csVector3 ODEHinge2Joint::GetHingeAnchor2 ()
{
  dVector3 pos;
  dJointGetHinge2Anchor2 (jointID, pos);
  return csVector3 (pos[0], pos[1], pos[2]);
}

csVector3 ODEHinge2Joint::GetHingeAxis1 ()
{
  dVector3 pos;
  dJointGetHinge2Axis1 (jointID, pos);
  return csVector3 (pos[0], pos[1], pos[2]);
}

csVector3 ODEHinge2Joint::GetHingeAxis2 ()
{
  dVector3 pos;
  dJointGetHinge2Axis2 (jointID, pos);
  return csVector3 (pos[0], pos[1], pos[2]);
}

csVector3 ODEHinge2Joint::GetAnchorError ()
{
  csVector3 pos1 = GetHingeAnchor1 ();
  csVector3 pos2 = GetHingeAnchor2 ();

  csVector3 result =  pos1 - pos2;
  if (result.x < 0) result.x = - result.x;
  if (result.y < 0) result.y = - result.y;
  if (result.z < 0) result.z = - result.z;

  return result;
}

//-------------------------------------------------------------------------------

ODEHingeJoint::ODEHingeJoint (dWorldID w_id)
{
  SCF_CONSTRUCT_IBASE (0);
  jointID = dJointCreateHinge (w_id, 0);
}

ODEHingeJoint::~ODEHingeJoint ()
{
  SCF_DESTRUCT_IBASE();
  dJointDestroy (jointID);
}

csVector3 ODEHingeJoint::GetHingeAnchor1 ()
{
  dVector3 pos;
  dJointGetHingeAnchor (jointID, pos);
  return csVector3 (pos[0], pos[1], pos[2]);
}

csVector3 ODEHingeJoint::GetHingeAnchor2 ()
{
  dVector3 pos;
  dJointGetHingeAnchor2 (jointID, pos);
  return csVector3 (pos[0], pos[1], pos[2]);
}

csVector3 ODEHingeJoint::GetHingeAxis ()
{
  dVector3 pos;
  dJointGetHingeAxis (jointID, pos);
  return csVector3 (pos[0], pos[1], pos[2]);
}

csVector3 ODEHingeJoint::GetAnchorError ()
{
  csVector3 pos1 = GetHingeAnchor1 ();
  csVector3 pos2 = GetHingeAnchor2 ();

  csVector3 result =  pos1 - pos2;
  if (result.x < 0) result.x = - result.x;
  if (result.y < 0) result.y = - result.y;
  if (result.z < 0) result.z = - result.z;

  return result;
}

//-------------------------------------------------------------------------------

ODEBallJoint::ODEBallJoint (dWorldID w_id)
{
  SCF_CONSTRUCT_IBASE (0);
  jointID = dJointCreateBall (w_id, 0);
}

ODEBallJoint::~ODEBallJoint ()
{
  SCF_DESTRUCT_IBASE();
  dJointDestroy (jointID);
}

csVector3 ODEBallJoint::GetBallAnchor1 ()
{
  dVector3 pos;
  dJointGetBallAnchor (jointID, pos);
  return csVector3 (pos[0], pos[1], pos[2]);
}

csVector3 ODEBallJoint::GetBallAnchor2 ()
{
  dVector3 pos;
  dJointGetBallAnchor2 (jointID, pos);
  return csVector3 (pos[0], pos[1], pos[2]);
}

csVector3 ODEBallJoint::GetAnchorError ()
{
  csVector3 pos1, pos2;
  pos1 = GetBallAnchor1 ();
  pos2 = GetBallAnchor2 ();

  csVector3 result =  pos1 - pos2;
  if (result.x < 0) result.x = - result.x;
  if (result.y < 0) result.y = - result.y;
  if (result.z < 0) result.z = - result.z;

  return result;
}

//-------------------------------------------------------------------------------

csODEJoint::csODEJoint (csODEDynamicSystem *sys) : scfImplementationType (this, sys)
{
  jointID = 0;
  motor_jointID = 0;

  body[0] = body[1] = 0;
  bodyID[0] = bodyID[1] = 0;

  transConstraint[0] = 1;
  transConstraint[1] = 1;
  transConstraint[2] = 1;
  rotConstraint[0] = 1;
  rotConstraint[1] = 1;
  rotConstraint[2] = 1;

  aconstraint_axis[0] = csVector3 (0,1,0);
  aconstraint_axis[1] = csVector3 (0,0,1);

  lo_stop = csVector3 (dInfinity, dInfinity, dInfinity);
  hi_stop = csVector3 (-dInfinity, -dInfinity, -dInfinity); 
  vel = csVector3 (0);
  fmax = csVector3 (0);
  fudge_factor = csVector3 (1);
  bounce = csVector3 (0);
  cfm = csVector3 (9.9999997e-006f);
  stop_erp = csVector3 (0.2f);
  stop_cfm = csVector3 (9.9999997e-006f);
  suspension_erp = csVector3 (0.0f);
  suspension_cfm = csVector3 (0.0f);
  custom_aconstraint_axis = false;

  dynsys = sys;
}

csODEJoint::~csODEJoint ()
{
  if (jointID)
    dJointDestroy (jointID);
  if (motor_jointID)
    dJointDestroy (motor_jointID);
}

void csODEJoint::Attach (iRigidBody *b1, iRigidBody *b2)
{
  if (b1)
  {
    bodyID[0] = ((csODERigidBody *)(b1->QueryObject()))->GetID();
  }
  else
  {
    bodyID[0] = 0;
  }
  if (b2)
  {
    bodyID[1] = ((csODERigidBody *)(b2->QueryObject()))->GetID();
  }
  else
  {
    bodyID[1] = 0;
  }
  body[0] = b1;
  body[1] = b2;
  BuildJoint ();
}

csRef<iRigidBody> csODEJoint::GetAttachedBody (int b)
{
  return (b == 0) ? body[0] : body[1];
}

void csODEJoint::SetTransform (const csOrthoTransform &trans)
{
  transform = trans;
  BuildJoint ();
}

csOrthoTransform csODEJoint::GetTransform ()
{
  return transform;
}

void csODEJoint::SetTransConstraints (bool X, bool Y, bool Z)
{
  /* 1 means free and 0 means constrained */
  transConstraint[0] = (X) ? 0 : 1;
  transConstraint[1] = (Y) ? 0 : 1;
  transConstraint[2] = (Z) ? 0 : 1;
  BuildJoint ();
}

void csODEJoint::SetMinimumDistance (const csVector3 &min)
{
  lo_stop = min;
  BuildJoint ();
}
csVector3 csODEJoint::GetMinimumDistance ()
{
  return lo_stop;
}
void csODEJoint::SetMaximumDistance (const csVector3 &max)
{
  hi_stop = max;
  BuildJoint ();
}
csVector3 csODEJoint::GetMaximumDistance ()
{
  return hi_stop;
}

void csODEJoint::SetRotConstraints (bool X, bool Y, bool Z)
{
  /* 1 means free and 0 means constrained */
  rotConstraint[0] = (X) ? 0 : 1;
  rotConstraint[1] = (Y) ? 0 : 1;
  rotConstraint[2] = (Z) ? 0 : 1;
  BuildJoint ();
}
void csODEJoint::SetMinimumAngle (const csVector3 &min)
{
  lo_stop = min;
  BuildJoint ();
}
csVector3 csODEJoint::GetMinimumAngle ()
{
  return lo_stop;
}
void csODEJoint::SetMaximumAngle (const csVector3 &max)
{
  hi_stop = max;
  BuildJoint ();
}
csVector3 csODEJoint::GetMaximumAngle ()
{
  return hi_stop;
}

void csODEJoint::BuildHinge (const csVector3 &axis)
{
  dJointSetHingeAxis (jointID, axis.x, axis.y, axis.z);
}

void csODEJoint::BuildHinge2 (const csVector3 &axis1, const csVector3 &axis2)
{
  if (custom_aconstraint_axis)
  {
    dJointSetHinge2Axis1 (jointID,
      aconstraint_axis[0].x, aconstraint_axis[0].y, aconstraint_axis[0].z);
    dJointSetHinge2Axis2 (jointID,
      aconstraint_axis[1].x, aconstraint_axis[1].y, aconstraint_axis[1].z);
  }else
  {
    dJointSetHinge2Axis1 (jointID, axis1.x, axis1.y, axis1.z);
    dJointSetHinge2Axis2 (jointID, axis2.x, axis2.y, axis2.z);
  }
}

void csODEJoint::BuildSlider (const csVector3 &axis)
{
  dJointSetSliderAxis (jointID, axis.x, axis.y, axis.z);
}

void csODEJoint::SetAngularConstraintAxis (const csVector3 &axis, int body)
{
  custom_aconstraint_axis = true;
  aconstraint_axis[body] = axis; 
  BuildJoint ();
}
csVector3 csODEJoint::GetAngularConstraintAxis (int body)
{
  return aconstraint_axis[body];
}

// parameter: one of ODE joint parameters.
// values: up to three possible values for up to 3 possible axis
// For slider joints, property must correspond to axis with
// translational constraint.  For hinges, the first element is used.
// for 2 axis 'steering' type joints, the first 2 elements are used.
// for ball and socket joints and angular motors, all three elements
// are used (NYI).

void csODEJoint::ApplyJointProperty (int parameter, const csVector3 &values)
{
  int jointType = dJointGetType (jointID);
  switch(jointType)
  {
  case dJointTypeHinge:
    if (rotConstraint[0])
    {
      dJointSetHingeParam (jointID, parameter, values.x);
    }
    else if (rotConstraint[1])
    {
      dJointSetHingeParam (jointID, parameter, values.y);
    }
    else if (rotConstraint[2])
    {
      dJointSetHingeParam (jointID, parameter, values.z);
    }
    
    break;
  case dJointTypeSlider:
    if (transConstraint[0])
      dJointSetSliderParam (jointID, parameter, values.x);
    else if (transConstraint[1])
      dJointSetSliderParam (jointID, parameter, values.y);
    else
      dJointSetSliderParam (jointID, parameter, values.z);
    break;
  case dJointTypeHinge2:
    //looks like axis 2 is meant to be axle,
    //axis 1 is steering, I may need to check that later though.
    dJointSetHinge2Param (jointID, parameter, values.x);
    dJointSetHinge2Param (jointID, parameter + dParamGroup, values.y);
    //dParamXi = dParamX + dParamGroup * (i-1)
  case dJointTypeBall:       
    if (motor_jointID)
    {
      dJointSetAMotorParam (motor_jointID, parameter, values.x);
      //We use only euler mode, se we only need to setup parameter for 2 axes
      dJointSetAMotorParam (motor_jointID, parameter + 2*dParamGroup, values.z);
    }
  default:
    //case dJointTypeAMotor:     // not supported here
    //case dJointTypeUniversal:  // not sure if that's supported in here
    break;
  }
}
void csODEJoint::SetStopAndMotorsParams ()
{
  //stop&motor properties
  if (jointID)
  {
    ApplyJointProperty (dParamLoStop, lo_stop);
    ApplyJointProperty (dParamHiStop, hi_stop);
    ApplyJointProperty (dParamVel, vel);
    ApplyJointProperty (dParamFMax, fmax);
    ApplyJointProperty (dParamFudgeFactor, fudge_factor);
    ApplyJointProperty (dParamBounce, bounce);
    ApplyJointProperty (dParamCFM, cfm);
    ApplyJointProperty (dParamStopERP, stop_erp);
    ApplyJointProperty (dParamStopCFM, stop_cfm);
    ApplyJointProperty (dParamSuspensionERP, suspension_erp);
    ApplyJointProperty (dParamSuspensionCFM, suspension_cfm);
  }

}
void csODEJoint::BuildJoint ()
{
  if (!(bodyID[0] || bodyID[1]))
  {
    return;
  }
  if (motor_jointID)
  {
    dJointDestroy (motor_jointID);
  }
  if (jointID)
  {
    dJointDestroy (jointID);
  }
  int transcount = transConstraint[0] + transConstraint[1] + transConstraint[2];
  int rotcount = rotConstraint[0] + rotConstraint[1] + rotConstraint[2];

  csVector3 pos;
  csMatrix3 rot;
  if (transcount == 0)
  {
    switch (rotcount)
    {
    case 0:
      jointID = dJointCreateFixed (dynsys->GetWorldID(), 0);
      dJointAttach (jointID, bodyID[0], bodyID[1]);
      SetStopAndMotorsParams ();
      dJointSetFixed (jointID);
      break;
    case 1:
      jointID = dJointCreateHinge (dynsys->GetWorldID(), 0);
      dJointAttach (jointID, bodyID[0], bodyID[1]);
      SetStopAndMotorsParams ();
      pos = transform.GetOrigin();
      dJointSetHingeAnchor (jointID, pos.x, pos.y, pos.z);
      rot = transform.GetO2T();
      if (rotConstraint[0])
      {
        BuildHinge (rot.Col1());
      }
      else if (rotConstraint[1])
      {
        BuildHinge (rot.Col2());
      }
      else if (rotConstraint[2])
      {
        BuildHinge (rot.Col3());
      }
      // TODO: insert some mechanism for bounce, erp and cfm
      break;
    case 2:
      jointID = dJointCreateHinge2 (dynsys->GetWorldID(), 0);
      SetStopAndMotorsParams ();
      dJointAttach (jointID, bodyID[0], bodyID[1]);
      pos = transform.GetOrigin();
      dJointSetHinge2Anchor (jointID, pos.x, pos.y, pos.z);
      rot = transform.GetO2T();

      if (rotConstraint[0])
      {
        if (rotConstraint[1])
        {
          BuildHinge2 (rot.Col2(), rot.Col1());
        }
        else
        {
          BuildHinge2 (rot.Col3(), rot.Col1());
        }
      }
      else
      {
        BuildHinge2 (rot.Col2(), rot.Col3());
      }
      break;
    case 3:
      jointID = dJointCreateBall (dynsys->GetWorldID(), 0);
      SetStopAndMotorsParams ();
      dJointAttach (jointID, bodyID[0], bodyID[1]);
      pos = transform.GetOrigin();
      dJointSetBallAnchor (jointID, pos.x, pos.y, pos.z);
      if (hi_stop.x > lo_stop.x || hi_stop.y > lo_stop.y || hi_stop.z > lo_stop.z )
      {
        motor_jointID = dJointCreateAMotor (dynsys->GetWorldID(), 0);
        dJointAttach (jointID, bodyID[0], bodyID[1]);
        dJointSetAMotorMode (motor_jointID, dAMotorEuler);
        dJointSetAMotorAxis (motor_jointID, 0, 1,
          aconstraint_axis[0].x, aconstraint_axis[0].y, aconstraint_axis[0].z);
        dJointSetAMotorAxis (motor_jointID, 2, 2,
          aconstraint_axis[1].x, aconstraint_axis[1].y, aconstraint_axis[1].z);
      }
      break;
    }
  }
  else if (rotcount == 0)
  {
    switch (transcount)
    {
      /* 0 is accounted for in the previous condition */
    case 1:
      jointID = dJointCreateSlider (dynsys->GetWorldID(), 0);
      SetStopAndMotorsParams ();
      dJointAttach (jointID, bodyID[0], bodyID[1]);
      rot = transform.GetO2T();
      if (transConstraint[0])
      {
        BuildSlider (rot.Col1());
      }
      else if (transConstraint[1])
      {
        BuildSlider (rot.Col2());
      }
      else
      {
        BuildSlider (rot.Col3());
      }
      break;
    case 2:
      // TODO fill this in with a contact joint
      break;
    case 3:
      /* doesn't exist */
      break;
    }
  } else {
    /* too unconstrained, don't create joint */
  }
}

ODEJointType csODEJoint::GetType()
{
  switch (dJointGetType (jointID))
  {
  case dJointTypeBall: return CS_ODE_JOINT_TYPE_BALL;
  case dJointTypeHinge: return CS_ODE_JOINT_TYPE_HINGE;
  case dJointTypeSlider: return CS_ODE_JOINT_TYPE_SLIDER;
  case dJointTypeContact: return CS_ODE_JOINT_TYPE_CONTACT;
  case dJointTypeUniversal: return CS_ODE_JOINT_TYPE_UNIVERSAL;
  case dJointTypeHinge2: return CS_ODE_JOINT_TYPE_HINGE2;
  case dJointTypeFixed: return CS_ODE_JOINT_TYPE_FIXED;
  case dJointTypeAMotor: return CS_ODE_JOINT_TYPE_AMOTOR;
  }
  return CS_ODE_JOINT_TYPE_UNKNOWN;
}

csVector3 csODEJoint::GetParam (int parameter)
{
  //switch(GetType())
  //{
  //case CS_ODE_JOINT_TYPE_HINGE:
  //  return dJointGetHingeParam (scfParent->jointID, parameter);
  //case CS_ODE_JOINT_TYPE_SLIDER:
  //  return dJointGetSliderParam (scfParent->jointID, parameter);
  //case CS_ODE_JOINT_TYPE_HINGE2:
  //  return dJointGetHinge2Param (scfParent->jointID, parameter);
  //case CS_ODE_JOINT_TYPE_AMOTOR:
  //  return dJointGetAMotorParam (scfParent->jointID, parameter);
  //default:
    return 0.0; // this is not a good... the error is ignored silently...
  //}
}

csODEDefaultMoveCallback::csODEDefaultMoveCallback ()
{
  SCF_CONSTRUCT_IBASE (0);
}

csODEDefaultMoveCallback::~csODEDefaultMoveCallback ()
{
  SCF_DESTRUCT_IBASE();
}

void csODEDefaultMoveCallback::Execute (iMovable* movable,
                                        csOrthoTransform& t)
{
  // Dont do anything if nothing has changed
  if (movable->GetPosition() == t.GetOrigin() &&
    movable->GetTransform().GetT2O() == t.GetT2O())
    return;

  // Update movable
  movable->SetPosition (t.GetOrigin ());
  movable->GetTransform ().SetT2O (t.GetT2O ());
  movable->UpdateMove ();
}

void csODEDefaultMoveCallback::Execute (iMeshWrapper* mesh,
                                        csOrthoTransform& t)
{
  Execute (mesh->GetMovable (), t);
}

void csODEDefaultMoveCallback::Execute (iLight* light,
                                        csOrthoTransform& t)
{
  Execute (light->GetMovable (), t);
}

void csODEDefaultMoveCallback::Execute (iCamera* camera,
                                        csOrthoTransform& t)
{
  // Dont do anything if nothing has changed
  csOrthoTransform& cam_trans = camera->GetTransform ();
  if (cam_trans.GetOrigin() == t.GetOrigin() &&
    cam_trans.GetT2O() == t.GetT2O())
    return;

  // Update movable
  cam_trans.SetOrigin (t.GetOrigin ());
  cam_trans.SetT2O (t.GetT2O ());
}

void csODEDefaultMoveCallback::Execute (csOrthoTransform& /*t*/)
{
  /* do nothing by default */
}

