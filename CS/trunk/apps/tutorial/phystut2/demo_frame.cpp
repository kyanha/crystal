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
* The main loop
*/

#include "cssysdef.h"
#include "csgeom/poly3d.h"
#include "csgeom/sphere.h"
#include "csgeom/quaternion.h"
#include "iengine/portal.h"
#include "imesh/genmesh.h"
#include "imesh/terrain2.h"
#include "cstool/genmeshbuilder.h"
#include "cstool/materialbuilder.h"
#include "physdemo.h"

using namespace CS::Collisions;
using namespace CS::Physics;

void PhysDemo::PrintHelp ()
{
  csCommandLineHelper commandLineHelper;

  // Command line options
  commandLineHelper.AddCommandLineOption
    ("phys_engine", "Specify which physics plugin to use", csVariant ("bullet2"));
  commandLineHelper.AddCommandLineOption
    ("soft", "Enable the soft bodies", csVariant (true));
  commandLineHelper.AddCommandLineOption
    ("mapfile", csString ().Format ("The path to the map file to be loaded (try: )"), csVariant (""));
  commandLineHelper.AddCommandLineOption
    ("convexdecompose", csString ().Format ("Whether to perform convex decomposition on all render meshes"), csVariant (false));

  // Printing help
  commandLineHelper.PrintApplicationHelp
    (GetObjectRegistry (), "phystut2",
    "phystut2 <OPTIONS>",
    "Physics tutorial 2 for Crystal Space. ");
}

void PhysDemo::Frame ()
{
  // Update the demo's state information
  UpdateHUD ();

  // Simulate one step
  physicalSystem->Step (vc->GetElapsedSeconds ());

  if (actorMode != ActorModeNoclip)
  {
    // Rotate actor
    RotateActor ();

    if (!paused)
    {
      if (actorVehicle)
      {
	// Update vehicle
	MoveActorVehicle ();
      }
      else
      {
	// Update actor
	MoveActor ();
      }
    }

    // Update passengers of all vehicles (this should probably be assisted by the physics plugin)
    UpdateVehiclePassengers ();

    // Move the camera
    MoveCamera ();
  }

  // Update position of an object currently dragged by the mouse
  UpdateDragging ();

  // Weird and irritating stuff...
  ApplyGhostSlowEffect ();

  // Default behavior from DemoApplication
  DemoApplication::Frame ();

  // Display debug informations
  DoDebugDraw ();
}

void PhysDemo::MoveActor ()
{
  // Dynamics and actor simulation
  // First get elapsed time from the virtual clock.
  csTicks elapsed_time = vc->GetElapsedTicks ();
  const float moveSpeed = 15;

  const float timeMs = elapsed_time / 1000.0;

  iCamera* camera = view->GetCamera ();
  csOrthoTransform& camTrans = camera->GetTransform ();
  csOrthoTransform actorTrans = player.GetObject ()->GetTransform ();

  // handle movement
  // intended movement direction
  if (player.GetActor ())
  {
    csVector3 newVel = GetInputDirection ();
    bool hasMoveDir = newVel.Norm () > EPSILON;
    bool wantsToMove = hasMoveDir | kbd->GetKeyState (KeyJump);

    if (!wantsToMove)
    {
      // stop any player-controlled movement
      player.GetActor ()->StopMoving ();
    }
    else
    {
      // move actor
      if (hasMoveDir)
      {
        newVel = camTrans.GetT2O () * newVel;

        bool freeFall = player.GetActor ()->IsFreeFalling ();

        // actor is on ground, flying or has air control
        if (!freeFall || player.GetActor ()->GetAirControlFactor () > 0)
        {
          // wants to move and may move
          if (!IsGravityOff ())
          {
            // Only walk horizontally when gravity applies
            csVector2 newVel2 (newVel.x, newVel.z);
            player.GetActor ()->WalkHorizontal (newVel2);
            //dynamicActor->Walk (newVel);
          }
          else
          {
            // move freely when gravity is off
            player.GetActor ()->Walk (newVel);
          }
        }
      }
      if (!player.GetActor ()->IsFreeFalling () && kbd->GetKeyState (KeyJump))
      {
        // Jump
        player.GetActor ()->Jump ();
      }
    }
  }
  else
  {
    // Only move camera
    csVector3 newVel = GetInputDirection ();
    if (kbd->GetKeyState (KeyJump))
    {
      // add upward movement
      newVel[UpAxis] += moveSpeed;
    }
    camera->Move (newVel * moveSpeed * timeMs);
  }
}

void PhysDemo::RotateActor ()
{
  csTicks elapsed_time = vc->GetElapsedTicks ();

  const float timeMs = elapsed_time / 1000.0;

  static const float MaxPitchCos = .965f;      // can't get closer than 15 degrees to UpAxis to prevent gimbal lock

  iCamera* camera = view->GetCamera ();
  csOrthoTransform& camTrans = camera->GetTransform ();
  // player.GetObject ()->SetTransform (actorTrans);
  csOrthoTransform actorTrans = player.GetObject ()->GetTransform ();

  // Rotate camera
  if (kbd->GetKeyState (KeyLeft) || kbd->GetKeyState (CSKEY_LEFT) ||
    kbd->GetKeyState (KeyRight) || kbd->GetKeyState (CSKEY_RIGHT) ||
    kbd->GetKeyState (KeyUp) ||
    kbd->GetKeyState (KeyDown))
  {
    float turnAmount = turnSpeed * timeMs;

    float yaw = 0;
    if (kbd->GetKeyState (KeyLeft) || kbd->GetKeyState (CSKEY_LEFT))
    {
      yaw -= turnAmount;
    }
    if (kbd->GetKeyState (KeyRight) || kbd->GetKeyState (CSKEY_RIGHT))
    {
      yaw += turnAmount;
    }
    
    csVector3 actorDir3 = actorTrans.GetT2O ().Col3 ();
    csVector2 actorDir2 (actorDir3.x, actorDir3.z);

    // Update actor yaw
    if (yaw)
    {
      actorDir2.Rotate (yaw);
      actorDir3.Set (actorDir2.x, 0, actorDir2.y);
      actorTrans.LookAt (actorDir3, UpVector);
      // TODO: Ouch!!! Remove that!!!
      player.GetObject ()->SetTransform (actorTrans);
    }

    // Update camera pitch
    // TODO: Zoom out/in when in 3rd person mode
    if (cameraMode == CameraMode1stPerson)
    {
      // Get pitch
      csVector3 camDir3 = camTrans.GetT2O ().Col3 ();
      float currentPitchCos = camDir3[UpAxis];

      float pitch = 0;
      if (kbd->GetKeyState (KeyUp))
      {
        if (currentPitchCos + turnAmount < MaxPitchCos) pitch -= turnAmount;
      }
      if (kbd->GetKeyState (KeyDown))
      {
        if (currentPitchCos - turnAmount > -MaxPitchCos) pitch += turnAmount;
      }
      
      // Set horizontal camera direction equal to actor direction, scaled correspondingly
      csVector2 camDir2 (camDir3.x, camDir3.z);
      actorDir2.Normalize ();
      actorDir2 *= camDir2.Norm ();
      camDir3.Set (actorDir2.x, currentPitchCos, actorDir2.y);

      // Update camera pitch
      if (pitch)
      {
        actorDir2.Normalize ();
        actorDir2.Rotate (HALF_PI);
        csVector3 camOrth3 (actorDir2.x, 0, actorDir2.y);
        
        // rotate by quaternion
        csQuaternion q;
        q.SetAxisAngle (camOrth3, pitch);
        
        camDir3 = q.Rotate (camDir3);
      }
      camTrans.LookAt (camDir3, UpVector);
    }
  }
}

void PhysDemo::MoveCamera ()
{
  iCamera* camera = view->GetCamera ();
  csOrthoTransform& camTrans = camera->GetTransform ();

  // TODO: use iCollisionObject::SetAttachedCamera() instead
  if (player.GetObject () && actorMode != ActorModeNoclip)
  {
    // Update the sector of the camera if needed
    if (camera->GetSector () != player.GetObject ()->GetSector ()->GetSector ())
      camera->SetSector (player.GetObject ()->GetSector ()->GetSector ());

    // adjust camera relative to actor
    csOrthoTransform actorTrans = player.GetObject ()->GetTransform ();
    csVector3 targetPos = player.GetObject ()->GetTransform ().GetOrigin ();

    if (cameraMode != CameraMode1stPerson)
    {
      csVector3 pos = camTrans.GetOrigin ();

      // camera follows the actor, looking over the shoulder
      float camDistFactor = cameraMode == CameraMode3rdPersonFar ? 3 : 1;
      float camDistance = 2 * camDistFactor * ActorDimensions.Norm ();

      targetPos -= camDistance * actorTrans.GetT2O () * csVector3 (0, -1, 1); // * (1 / SQRT2)

      // interpolate between current pos and target pos
      static const float targetWeight = 0.1;
      targetPos = targetWeight * targetPos + (1 - targetWeight) * pos;
      camTrans.SetOrigin (targetPos);

      // let the camera look at a point in front of the actor
      csVector3 lookAtPos = actorTrans.GetOrigin () + actorTrans.GetT2O () * csVector3 (0, 0, 1) - targetPos;
      camTrans.LookAt (lookAtPos, UpVector);
    }
    else
    {
      // Move camera eye level (~ 0.9 * actorheight)
      camTrans.SetOrigin (targetPos + csVector3 (0, .4f * ActorDimensions.y, 0));
    }
  }
}

void PhysDemo::UpdateDragging ()
{
  if (dragging)
  {
    iCamera* cam = view->GetCamera ();
    csOrthoTransform& camTrans = cam->GetTransform ();

    // Keep the drag joint at the same distance to the camera
    csVector2 v2d (mouse->GetLastX (), g2d->GetHeight () - mouse->GetLastY ());
    csVector3 v3d = cam->InvPerspective (v2d, 10000);
    csVector3 startBeam = camTrans.GetOrigin ();
    csVector3 endBeam = camTrans.This2Other (v3d);

    csVector3 newPosition = endBeam - startBeam;
    newPosition.Normalize ();
    newPosition = camTrans.GetOrigin () + newPosition * dragDistance;
    // TODO: switch the joint's collision sector if crossing a portal...
    dragJoint->SetPosition (newPosition);
  }
}

void PhysDemo::UpdateHUD ()
{
  hudManager->GetStateDescriptions ()->Empty ();
  csString txt;

  if (actorVehicle || player.GetObject ()->QueryPhysicalBody ())
  {
    float speed;
    if (actorVehicle)
    {
      speed = actorVehicle->GetSpeedKMH ();
    }
    else
    {
      speed = 3.6f * player.GetObject ()->QueryPhysicalBody ()->GetLinearVelocity ().Norm ();
    }
    txt.Format ("Speed : %.3f km/h", speed);
    hudManager->GetStateDescriptions ()->Push (txt);
  }

  txt.Format ("Collision objects: %zu", GetCurrentSector ()->GetCollisionObjectCount ());
  hudManager->GetStateDescriptions ()->Push (txt);

  txt.Format ("Rigid bodies: %zu", GetCurrentSector ()->GetRigidBodyCount ());
  hudManager->GetStateDescriptions ()->Push (txt);

  if (isSoftBodyWorld)
  {
    txt.Format ("Soft bodies: %zu", GetCurrentSector ()->GetSoftBodyCount ());
    hudManager->GetStateDescriptions ()->Push (txt);
  }

  switch (actorMode)
  {
  case ActorModeDynamic:
    hudManager->GetStateDescriptions ()->Push (csString ("Actor mode: Dynamic"));
    break;

  case ActorModeNoclip:
    hudManager->GetStateDescriptions ()->Push (csString ("Actor mode: No Clip"));
    break;

  case ActorModeKinematic:
    hudManager->GetStateDescriptions ()->Push (csString ("Actor mode: Kinematic"));
    break;

  default:
    break;
  }
}

void PhysDemo::DoDebugDraw ()
{
  if (do_bullet_debug)
    physicalSystem->DebugDraw (view);
  else if (isSoftBodyWorld && do_soft_debug)
    for (size_t i = 0; i < GetCurrentSector ()->GetSoftBodyCount (); i++)
    {
      CS::Physics::iSoftBody* softBody = GetCurrentSector ()->GetSoftBody (i);
      csRef<CS::Physics::iSoftBody> bulletSoftBody = 
        scfQueryInterface<CS::Physics::iSoftBody> (softBody);
      if (softBody->GetVertexCount ())
        bulletSoftBody->DebugDraw (view);
    }
}
