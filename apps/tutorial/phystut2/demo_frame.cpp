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
  // Compute the rotation delta of the actor from the keys activated
  float yaw = 0.0f;
  float pitch = 0.0f;
  float rotationSpeed = vc->GetElapsedSeconds () * turnSpeed;

  if (kbd->GetKeyState (CSKEY_RIGHT))
    yaw += rotationSpeed;

  if (kbd->GetKeyState (CSKEY_LEFT))
    yaw -= rotationSpeed;

  if (kbd->GetKeyState (KeyUp))
    pitch -= rotationSpeed;

  if (kbd->GetKeyState (KeyDown))
    pitch += rotationSpeed;

  // Apply the actor rotation
  player.GetActor ()->Rotate (yaw, pitch);

  // Compute the speed of the actor from the keys activated
  csVector3 speed (0.0f);

  float motionSpeed = moveSpeed;

  if (kbd->GetKeyState (CSKEY_SHIFT))
    motionSpeed *= 3.0f;

  if (kbd->GetKeyState (KeyForward) || kbd->GetKeyState (CSKEY_UP))
    speed[2] += motionSpeed;

  if (kbd->GetKeyState (KeyBackward) || kbd->GetKeyState (CSKEY_DOWN))
    speed[2] -= motionSpeed;

  if (kbd->GetKeyState (KeyLeft))
    speed[0] -= motionSpeed;

  if (kbd->GetKeyState (KeyRight))
    speed[0] += motionSpeed;

  // Apply the actor speed
  player.GetActor ()->SetSpeed (speed);

  // Jump if needed
  if (kbd->GetKeyState (KeyJump) && player.GetActor ()->IsOnGround ())
    player.GetActor ()->Jump ();
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
  case ActorModePhysical:
    hudManager->GetStateDescriptions ()->Push (csString ("Actor mode: Physical"));
    break;

  case ActorModeNoclip:
    hudManager->GetStateDescriptions ()->Push (csString ("Actor mode: No Clip"));
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
