/*
  Copyright (C) 2009-10 Christian Van Brussel, Communications and Remote
      Sensing Laboratory of the School of Engineering at the 
      Universite catholique de Louvain, Belgium
      http://www.tele.ucl.ac.be

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

#include "avatartest.h"
#include "imesh/lookat.h"
#include "imesh/basicskelanim.h"

#define LOOKAT_CAMERA 1
#define LOOKAT_POSITION 2
#define LOOKAT_NOTHING 3

#define ROTATION_SLOW 1
#define ROTATION_NORMAL 2
#define ROTATION_IMMEDIATE 3

CS_IMPLEMENT_APPLICATION

AvatarTest *avatarTest;

AvatarTest::AvatarTest (iObjectRegistry* object_reg)
  : scfImplementationType (this), object_reg (object_reg), targetMode (LOOKAT_CAMERA),
    alwaysRotate (false), rotationSpeed (ROTATION_NORMAL), targetReached (false),
    currentSpeed (0)
{
}

AvatarTest::~AvatarTest ()
{
  lookAtNode->RemoveListener (this);
}

void AvatarTest::SetupFrame ()
{
  // First get elapsed time from the virtual clock.
  csTicks elapsed_time = vc->GetElapsedTicks ();

  // Now rotate the camera according to keyboard state
  const float speed = elapsed_time / 1000.0f;

  // Compute camera and animesh position
  iCamera* c = view->GetCamera ();
  csVector3 cameraPosition = c->GetTransform ().GetOrigin ();
  csRef<iMeshObject> animeshObject = scfQueryInterface<iMeshObject> (animesh);
  csVector3 avatarPosition = animeshObject->GetMeshWrapper ()->QuerySceneNode ()
    ->GetMovable ()->GetTransform ().GetOrigin () + csVector3 (0.0f, 0.35f, 0.0f);

  if (kbd->GetKeyState (CSKEY_SHIFT))
  {
    // If the user is holding down shift, the Up/Down arrow keys will cause
    // the camera to go forwards and backwards (forward only allowed if camera 
    // not too close). Left/Right arrows work also when shift is hold.
    if (kbd->GetKeyState (CSKEY_UP)
	&& (cameraPosition - avatarPosition).Norm () > 0.5f)
      c->Move (CS_VEC_FORWARD * 4 * speed);
    if (kbd->GetKeyState (CSKEY_DOWN))
      c->Move (CS_VEC_BACKWARD * 4 * speed);
    if (kbd->GetKeyState (CSKEY_RIGHT))
      c->Move (CS_VEC_RIGHT * 4 * speed);
    if (kbd->GetKeyState (CSKEY_LEFT))
      c->Move (CS_VEC_LEFT * 4 * speed);
  }
  else
  {
    // Left and right arrows cause the camera to strafe on the X axis; up and 
    // down arrows cause the camera to strafe on the Y axis
    if (kbd->GetKeyState (CSKEY_RIGHT))
      c->Move (CS_VEC_RIGHT * 4 * speed);
    if (kbd->GetKeyState (CSKEY_LEFT))
      c->Move (CS_VEC_LEFT * 4 * speed);

    // Avoid gimbal lock of camera
    cameraPosition.Normalize ();
    float cameraDot = cameraPosition * csVector3 (0.0f, 1.0f, 0.0f);
    if (kbd->GetKeyState (CSKEY_UP)
	&& cameraDot < 0.98f)
      c->Move (CS_VEC_UP * 4 * speed);
    if (kbd->GetKeyState (CSKEY_DOWN)
	&& cameraDot > -0.98f)
      c->Move (CS_VEC_DOWN * 4 * speed);
  }

  // Make the camera look at the animesh
  c->GetTransform().LookAt (avatarPosition - c->GetTransform().GetOrigin (),
  			    csVector3(0,1,0) );

  // Step the dynamic simulation (we slow down artificially the simulation in
  // order to achieve a 'slow motion' effect)
  if (physicsEnabled)
    dynamics->Step (speed / 4.0f);

  // Update the morph state (frankie smiles sadistically if no target in view)
  if (targetReached)
    smileWeight -= (float) elapsed_time / 250.0f;
  else 
    smileWeight += (float) elapsed_time / 1500.0f;

  if (smileWeight > 1.0f)
    smileWeight = 1.0f;
  else if (smileWeight < 0.0f)
    smileWeight = 0.0f;

  animesh->SetMorphTargetWeight (animeshFactory->FindMorphTarget ("smile.B"),
				 smileWeight);
  animesh->SetMorphTargetWeight (animeshFactory->FindMorphTarget ("eyebrows_down.B"),
				 smileWeight);

  // Tell 3D driver we're going to display 3D things.
  if (!g3d->BeginDraw (engine->GetBeginDrawFlags () | CSDRAW_3DGRAPHICS))
    return;

  // Tell the camera to render into the frame buffer.
  view->Draw ();

  // Write FPS and other info..
  if(!g3d->BeginDraw (CSDRAW_2DGRAPHICS)) return;

  if (targetMode == LOOKAT_CAMERA)
    WriteShadow(10, 480, g2d->FindRGB (255, 150, 100), "Watch out, Frankie is looking at you!");
  else if (targetMode == LOOKAT_POSITION)
    WriteShadow(10, 480, g2d->FindRGB (255, 150, 100), "Frankie is looking at something");
  else if (targetMode == LOOKAT_NOTHING)
    WriteShadow(10, 480, g2d->FindRGB (255, 150, 100), "Frankie doesn't care about anything");

  if (alwaysRotate)
    WriteShadow(10, 500, g2d->FindRGB (255, 150, 100), "Always rotate: ON");
  else
    WriteShadow(10, 500, g2d->FindRGB (255, 150, 100), "Always rotate: OFF");

  if (rotationSpeed == ROTATION_SLOW)
    WriteShadow(10, 520, g2d->FindRGB (255, 150, 100), "Rotation speed: really slow");
  if (rotationSpeed == ROTATION_NORMAL)
    WriteShadow(10, 520, g2d->FindRGB (255, 150, 100), "Rotation speed: normal");
  if (rotationSpeed == ROTATION_IMMEDIATE)
    WriteShadow(10, 520, g2d->FindRGB (255, 150, 100), "Rotation speed: infinite");

  WriteShadow(10, 540, g2d->FindRGB (255, 150, 100), "Walk speed: %.1f",
	      ((float) currentSpeed) / 10.0f);

  if (speed != 0.0f)
    WriteShadow(10, 560, g2d->FindRGB (255, 150, 100), "FPS: %.2f",
		 1.0f / speed);

  // Write available keys
  DisplayKeys ();
}

bool AvatarTest::HandleEvent (iEvent& ev)
{
  if (ev.Name == Frame)
  {
    avatarTest->SetupFrame ();
    return true;
  }
  else if (CS_IS_KEYBOARD_EVENT(object_reg, ev)) 
  {
    if (ev.Name == KeyboardDown)
    {
      // Toggle the target mode of the 'LookAt' controller
      if (csKeyEventHelper::GetCookedCode (&ev) == 't')
      {
	if (targetMode == LOOKAT_CAMERA)
	{
	  lookAtNode->SetTarget (view->GetCamera ()->GetTransform ().GetOrigin ());
	  targetMode = LOOKAT_POSITION;
	}

	else if (targetMode == LOOKAT_POSITION)
	{
	  lookAtNode->RemoveTarget ();
	  targetMode = LOOKAT_NOTHING;
	}

	else
	{
	  lookAtNode->SetTarget (view->GetCamera (), csVector3 (0.0f));
	  targetMode = LOOKAT_CAMERA;
	}

	return true;
      }
    }

    // Toggle 'always rotate' option of the 'LookAt' controller
    else if (csKeyEventHelper::GetCookedCode (&ev) == 'a')
    {
      alwaysRotate = !alwaysRotate;
      lookAtNode->SetAlwaysRotate (alwaysRotate);
      return true;
    }

    // Toggle rotation speed of the 'LookAt' controller
    else if (csKeyEventHelper::GetCookedCode (&ev) == 's')
    {
      if (rotationSpeed == ROTATION_SLOW)
      {
	rotationSpeed = ROTATION_NORMAL;
	lookAtNode->SetMaximumSpeed (5.0f);
      }

      else if (rotationSpeed == ROTATION_NORMAL)
      {
	rotationSpeed = ROTATION_IMMEDIATE;
	lookAtNode->SetMaximumSpeed (0.0f);
      }

      else if (rotationSpeed == ROTATION_IMMEDIATE)
      {
	rotationSpeed = ROTATION_SLOW;
	lookAtNode->SetMaximumSpeed (0.5f);
      }

      return true;
    }

    // Update walk speed of the 'speed' controller
    else if (csKeyEventHelper::GetCookedCode (&ev) == '+')
    {
      if (currentSpeed < 29)
      {
	currentSpeed += 1;
	speedNode->SetSpeed (((float) currentSpeed) / 10.0f);
      }
      return true;
    }

    else if (csKeyEventHelper::GetCookedCode (&ev) == '-')
    {
      if (currentSpeed > 0)
      {
	currentSpeed -= 1;
	speedNode->SetSpeed (((float) currentSpeed) / 10.0f);
      }
      return true;
    }

    else if (csKeyEventHelper::GetCookedCode (&ev) == 'k'
	     && physicsEnabled)
    {
      KillFrankie ();
      return true;
    }

    // Reset of the scene
    else if (csKeyEventHelper::GetCookedCode (&ev) == 'r')
    {
      ResetScene ();
      return true;
    }

    // Check for ESC key
    else if (csKeyEventHelper::GetCookedCode (&ev) == CSKEY_ESC)
    {
      csRef<iEventQueue> q (csQueryRegistry<iEventQueue> (object_reg));
      if (q) q->GetEventOutlet()->Broadcast (csevQuit (object_reg));
      return true;
    }
  }

  return false;
}

bool AvatarTest::AvatarTestEventHandler (iEvent& ev)
{
  return avatarTest ? avatarTest->HandleEvent (ev) : false;
}

bool AvatarTest::Initialize ()
{
  if (!csInitializer::RequestPlugins (object_reg,
    CS_REQUEST_VFS,
    CS_REQUEST_OPENGL3D,
    CS_REQUEST_ENGINE,
    CS_REQUEST_FONTSERVER,
    CS_REQUEST_IMAGELOADER,
    CS_REQUEST_LEVELLOADER,
    CS_REQUEST_REPORTER,
    CS_REQUEST_REPORTERLISTENER,
    CS_REQUEST_PLUGIN ("crystalspace.mesh.animesh.controllers.lookat",
		       iSkeletonLookAtManager2),
    CS_REQUEST_PLUGIN ("crystalspace.mesh.animesh.controllers.basic",
		       iSkeletonBasicNodesManager2),
    CS_REQUEST_END))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
      "crystalspace.application.avatartest",
      "Can't initialize plugins!");
    return false;
  }

  // Check if physics effects are enabled
  csRef<iCommandLineParser> clp = csQueryRegistry<iCommandLineParser> (object_reg);
  physicsEnabled = !clp->GetBoolOption ("no_physics", false);

  if (physicsEnabled)
  {
    csRef<iPluginManager> plugmgr = 
      csQueryRegistry<iPluginManager> (object_reg);

    // Load the Bullet plugin
    dynamics = csLoadPlugin<iDynamics> (plugmgr, "crystalspace.dynamics.bullet");
    if (!dynamics)
    {
      csReport (object_reg, CS_REPORTER_SEVERITY_WARNING,
		"crystalspace.application.avatartest",
		"Can't load Bullet plugin, continuing with reduced functionalities");
      physicsEnabled = false;
    }

    else
    {
      // Create the dynamic system
      dynamicSystem = dynamics->CreateSystem ();
      if (!dynamicSystem) 
      {
	csReport (object_reg, CS_REPORTER_SEVERITY_WARNING,
		  "crystalspace.application.avatartest",
		  "Can't create dynamic system, continuing with reduced functionalities");
	physicsEnabled = false;
      }

      else
      {
	// Load the ragdoll plugin
	ragdollManager = csLoadPlugin<iSkeletonRagdollManager2>
	  (plugmgr, "crystalspace.mesh.animesh.controllers.ragdoll");
	if (!ragdollManager)
	{
	  csReport (object_reg, CS_REPORTER_SEVERITY_WARNING,
		    "crystalspace.application.avatartest",
		    "Can't create dynamic system, continuing with reduced functionalities");
	  physicsEnabled = false;
	}
      }
    }
  }

  // Events
  if (!csInitializer::SetupEventHandler (object_reg, AvatarTestEventHandler))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
      "crystalspace.application.avatartest",
      "Can't initialize event handler!");
    return false;
  }
  CS_INITIALIZE_EVENT_SHORTCUTS (object_reg);

  KeyboardDown = csevKeyboardDown (object_reg);
  KeyboardUp = csevKeyboardUp (object_reg);

  // Check for commandline help.
  if (csCommandLineHelper::CheckHelp (object_reg))
  {
    csPrintf ("Usage: avatartest\n");
    csPrintf ("Tests on animesh animation\n\n");
    csPrintf ("Options for avatartest:\n");
    csPrintf ("  -no_physics:       disable physical animations\n");
    csCommandLineHelper::Help (object_reg);
    return false;
  }

  // The virtual clock.
  vc = csQueryRegistry<iVirtualClock> (object_reg);
  if (vc == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
      "crystalspace.application.avatartest",
      "Can't find the virtual clock!");
    return false;
  }

  // Find the pointer to engine and other plugins
  engine = csQueryRegistry<iEngine> (object_reg);
  if (engine == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
      "crystalspace.application.avatartest",
      "No iEngine plugin!");
    return false;
  }

  loader = csQueryRegistry<iLoader> (object_reg);
  if (loader == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
      "crystalspace.application.avatartest",
      "No iLoader plugin!");
    return false;
  }

  g3d = csQueryRegistry<iGraphics3D> (object_reg);
  if (g3d == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
      "crystalspace.application.avatartest",
      "No iGraphics3D plugin!");
    return false;
  }

  g2d = csQueryRegistry<iGraphics2D> (object_reg);
  if (g2d == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
      "crystalspace.application.avatartest",
      "No iGraphics2D plugin!");
    return false;
  }

  kbd = csQueryRegistry<iKeyboardDriver> (object_reg);
  if (kbd == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
      "crystalspace.application.avatartest",
      "No iKeyboardDriver plugin!");
    return false;
  }

  lookAtManager = csQueryRegistry<iSkeletonLookAtManager2> (object_reg);
  if (lookAtManager == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
      "crystalspace.application.avatartest",
      "No iLookAtManager plugin!");
    return false;
  }

  basicNodesManager = csQueryRegistry<iSkeletonBasicNodesManager2> (object_reg);
  if (basicNodesManager == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
      "crystalspace.application.avatartest",
      "No iSkeletonBasicNodesManager2 plugin!");
    return false;
  }

  // Open the main system. This will open all the previously loaded plug-ins.
  if (!csInitializer::OpenApplication (object_reg))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
      "crystalspace.application.avatartest",
      "Error opening system!");
    return false;
  }

  printer.AttachNew (new FramePrinter (object_reg));

  csRef<iFontServer> fs = g3d->GetDriver2D()->GetFontServer ();
  if (fs)
    courierFont = fs->LoadFont (CSFONT_COURIER);
  else
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
      "crystalspace.application.avatartest",
      "Error getting FontServer!");
    return false;
  };

  // Create sector
  room = engine->CreateSector ("room");

  // Initialize camera
  view = csPtr<iView> (new csView (engine, g3d));
  view->GetCamera ()->SetSector (room);
  view->GetCamera ()->GetTransform ().SetOrigin (csVector3 (0.0f, 0.0f, -1.25f));
  iGraphics2D* g2d = g3d->GetDriver2D ();
  view->SetRectangle (0, 0, g2d->GetWidth (), g2d->GetHeight ());

  // Create scene
  CreateRoom ();
  CreateAvatar ();

  return true;
}

void AvatarTest::Shutdown ()
{
  printer.Invalidate ();
}

void AvatarTest::Start ()
{
  csDefaultRunLoop (object_reg);
}

void AvatarTest::CreateRoom ()
{
  // Creating the background
  // First we make a primitive for our geometry.
  CS::Geometry::DensityTextureMapper bgMapper (0.3f);
  CS::Geometry::TesselatedBox bgBox (csVector3 (-4000), csVector3 (4000));
  bgBox.SetMapper(&bgMapper);
  bgBox.SetFlags(CS::Geometry::Primitives::CS_PRIMBOX_INSIDE);
  
  // Now we make a factory and a mesh at once.
  csRef<iMeshWrapper> background =
    CS::Geometry::GeneralMeshBuilder::CreateFactoryAndMesh(engine, room,
				   "background", "background_factory", &bgBox);

  csRef<iMaterialWrapper> bgMaterial =
    CS::Material::MaterialBuilder::CreateColorMaterial
    (object_reg, "background", csColor (0.398f));
  background->GetMeshObject()->SetMaterialWrapper(bgMaterial);

  // Set up of the physical collider for the roof
  if (physicsEnabled)
    dynamicSystem->AttachColliderPlane (csPlane3 (csVector3 (0.0f, 1.0f, 0.0f), 0.0f),
					10.0f, 0.0f);

  // Creating lights
  csRef<iLight> light;
  iLightList* ll = room->GetLights ();

  // This light is for the background
  // TODO: putting instead the following line creates a black background, otherwise it is grey
  // the behavior doesn't persist if the other lights are removed
  //light = engine->CreateLight(0, csVector3(1, 1, -1), 9000, csColor (1));
  light = engine->CreateLight(0, csVector3(1, 1, 0), 9000, csColor (1));
  light->SetAttenuationMode (CS_ATTN_NONE);
  ll->Add (light);

  // Other lights
  light = engine->CreateLight (0, csVector3 (3, 0, 0), 8, csColor (1));
  light->SetAttenuationMode (CS_ATTN_REALISTIC);
  ll->Add (light);

  light = engine->CreateLight (0, csVector3 (-3, 0,  0), 8, csColor (1));
  light->SetAttenuationMode (CS_ATTN_REALISTIC);
  ll->Add (light);

  light = engine->CreateLight (0, csVector3 (0, 0, -3), 8, csColor (1));
  light->SetAttenuationMode (CS_ATTN_REALISTIC);
  ll->Add (light);

  light = engine->CreateLight (0, csVector3 (0, -3, 0), 8, csColor (1));
  light->SetAttenuationMode (CS_ATTN_REALISTIC);
  ll->Add (light);

  engine->Prepare ();

  CS::Lighting::SimpleStaticLighter::ShineLights (room, engine, 4);
}

void AvatarTest::CreateAvatar ()
{
  // Load animesh factory
  csLoadResult rc = loader->Load ("/lib/frankie/frankie.xml");
  if (!rc.success)
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.application.avatartest",
	      "Can't load frankie!");

  csRef<iMeshFactoryWrapper> meshfact = engine->FindMeshFactory ("franky_frankie");
  if (!meshfact)
    return;

  animeshFactory = scfQueryInterface<iAnimatedMeshFactory>
    (meshfact->GetMeshObjectFactory ());
  if (!animeshFactory)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.application.avatartest",
	      "Can't find frankie's animesh factory!");
    return;
  }

  // Load bodymesh (animesh's physical properties)
  rc = loader->Load ("/lib/frankie/skelfrankie_body");
  if (!rc.success)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.application.avatartest",
	      "Can't find frankie's body mesh file!");
    return;    
  }

  csRef<iBodyManager> bodyManager = csQueryRegistry<iBodyManager> (object_reg);
  csRef<iBodySkeleton> bodySkeleton = bodyManager->FindBodySkeleton ("frankie_body");
  if (!bodySkeleton)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.application.avatartest",
	      "Can't find Frankie's body mesh description!");
    return;
  }

  // Create a new animation tree. The structure of the tree is:
  //   + Finite State Machine node (root node)
  //     + 'LookAt' controller node
  //       + 'speed' controller node
  //         + animation nodes for all speeds
  //     + ragdoll controller node
  csRef<iSkeletonAnimPacketFactory2> animPacketFactory =
    animeshFactory->GetSkeletonFactory ()->GetAnimationPacket ();

  // Create the Finite State Machine node
  csRef<iSkeletonFSMNodeFactory2> FSMNodeFactory =
    animPacketFactory->CreateFSMNode ("fsm");
  animPacketFactory->SetAnimationRoot (FSMNodeFactory);

  // Create the 'LookAt' controller
  csRef<iSkeletonLookAtNodeFactory2> lookAtNodeFactory =
    lookAtManager->CreateAnimNodeFactory ("lookat", bodySkeleton);
  mainFSMState = FSMNodeFactory->AddState
    ("main_state", lookAtNodeFactory);
  FSMNodeFactory->SetStartState (mainFSMState);

  // Create the 'idle' animation node
  csRef<iSkeletonAnimationNodeFactory2> idleNodeFactory =
    animPacketFactory->CreateAnimationNode ("idle");
  idleNodeFactory->SetAnimation (animPacketFactory->FindAnimation ("Frankie_Idle1"));

  // Create the 'walk_slow' animation node
  csRef<iSkeletonAnimationNodeFactory2> walkSlowNodeFactory =
    animPacketFactory->CreateAnimationNode ("walk_slow");
  walkSlowNodeFactory->SetAnimation (animPacketFactory->FindAnimation ("Frankie_WalkSlow"));

  // Create the 'walk' animation node
  csRef<iSkeletonAnimationNodeFactory2> walkNodeFactory =
    animPacketFactory->CreateAnimationNode ("walk");
  walkNodeFactory->SetAnimation (animPacketFactory->FindAnimation ("Frankie_Walk"));

  // Create the 'walk_fast' animation node
  csRef<iSkeletonAnimationNodeFactory2> walkFastNodeFactory =
    animPacketFactory->CreateAnimationNode ("walk_fast");
  walkFastNodeFactory->SetAnimation (animPacketFactory->FindAnimation ("Frankie_WalkFast"));

  // Create the 'footing' animation node
  csRef<iSkeletonAnimationNodeFactory2> footingNodeFactory =
    animPacketFactory->CreateAnimationNode ("footing");
  footingNodeFactory->SetAnimation (animPacketFactory->FindAnimation ("Frankie_Runs"));

  // Create the 'run_slow' animation node
  csRef<iSkeletonAnimationNodeFactory2> runSlowNodeFactory =
    animPacketFactory->CreateAnimationNode ("run_slow");
  runSlowNodeFactory->SetAnimation (animPacketFactory->FindAnimation ("Frankie_RunSlow"));

  // Create the 'run' animation node
  csRef<iSkeletonAnimationNodeFactory2> runNodeFactory =
    animPacketFactory->CreateAnimationNode ("run");
  runNodeFactory->SetAnimation (animPacketFactory->FindAnimation ("Frankie_Run"));

  // Create the 'run_fast' animation node
  csRef<iSkeletonAnimationNodeFactory2> runFastNodeFactory =
    animPacketFactory->CreateAnimationNode ("run_fast");
  runFastNodeFactory->SetAnimation (animPacketFactory->FindAnimation ("Frankie_RunFaster"));

  // Create the 'run_jump' animation node
  csRef<iSkeletonAnimationNodeFactory2> runJumpNodeFactory =
    animPacketFactory->CreateAnimationNode ("run_jump");
  runJumpNodeFactory->SetAnimation (animPacketFactory->FindAnimation ("Frankie_RunFast2Jump"));

  // Create the 'speed' controller (and add all animations of Frankie moving at different speeds)
  // Unfortunately, the Frankie animations from 'walk fast' to 'footing'
  // do not blend well together, but this is just an example...
  csRef<iSkeletonSpeedNodeFactory2> speedNodeFactory =
    basicNodesManager->CreateSpeedNodeFactory ("speed");
  speedNodeFactory->AddNode (idleNodeFactory, 0.0f);
  speedNodeFactory->AddNode (walkSlowNodeFactory, 0.1f);
  speedNodeFactory->AddNode (walkNodeFactory, 0.3f);
  speedNodeFactory->AddNode (walkFastNodeFactory, 0.6f);
  speedNodeFactory->AddNode (footingNodeFactory, 0.8f);
  speedNodeFactory->AddNode (runSlowNodeFactory, 1.3f);
  speedNodeFactory->AddNode (runNodeFactory, 1.7f);
  speedNodeFactory->AddNode (runFastNodeFactory, 2.5f);
  speedNodeFactory->AddNode (runJumpNodeFactory, 2.9f);

  lookAtNodeFactory->SetChildNode (speedNodeFactory);

  if (physicsEnabled)
  {
    // Create the ragdoll controller
    csRef<iSkeletonRagdollNodeFactory2> ragdollNodeFactory =
      ragdollManager->CreateAnimNodeFactory ("ragdoll",
					     bodySkeleton, dynamicSystem);
    ragdollFSMState = FSMNodeFactory->AddState
      ("ragdoll_state", ragdollNodeFactory);

    // Create bone chain
    iBodyChain* chain = bodySkeleton->CreateBodyChain
      ("body_chain", animeshFactory->GetSkeletonFactory ()->FindBone ("Frankie_Main"),
       animeshFactory->GetSkeletonFactory ()->FindBone ("CTRL_Head"),
       animeshFactory->GetSkeletonFactory ()->FindBone ("Tail_8"), 0);
    ragdollNodeFactory->AddBodyChain (chain, RAGDOLL_STATE_DYNAMIC);
  }

  // Create the animated mesh
  csRef<iMeshWrapper> avatarMesh = engine->CreateMeshWrapper (meshfact, "Frankie",
					   room, csVector3 (0.0f));
  animesh = scfQueryInterface<iAnimatedMesh> (avatarMesh->GetMeshObject ());

  // When the animated mesh is created, the animation nodes are created too.
  // We can therefore set them up
  iSkeletonAnimNode2* rootNode =
    animesh->GetSkeleton ()->GetAnimationPacket ()->GetAnimationRoot ();

  // Setup of the FSM node
  FSMNode = scfQueryInterface<iSkeletonFSMNode2> (rootNode->FindNode ("fsm"));

  // Setup of the LookAt controller
  lookAtNode = scfQueryInterface<iSkeletonLookAtNode2> (rootNode->FindNode ("lookat"));
  lookAtNode->AddListener (this);
  lookAtNode->SetAnimatedMesh (animesh);
  lookAtNode->SetBone (animeshFactory->GetSkeletonFactory ()->FindBone ("CTRL_Head"));
  lookAtNode->SetListenerDelay (0.6f);

  // Setup of the speed controller
  speedNode = scfQueryInterface<iSkeletonSpeedNode2> (rootNode->FindNode ("speed"));

  // Setup of the ragdoll controller
  if (physicsEnabled)
  {
    ragdollNode = scfQueryInterface<iSkeletonRagdollNode2> (rootNode->FindNode ("ragdoll"));
    ragdollNode->SetAnimatedMesh (animesh);
  }

  // Reset the scene so as to put the parameters of the animation nodes in a default state
  ResetScene ();

  // Start animation
  rootNode->Play ();
}

void AvatarTest::TargetReached ()
{
  printf ("'LookAt' target reached\n");
  targetReached = true;
}

void AvatarTest::TargetLost ()
{
  printf ("'LookAt' target lost\n");
  targetReached = false;
}

void AvatarTest::KillFrankie ()
{
  // Set the ragdoll animation node as the active state of the Finite State Machine
  FSMNode->SwitchToState (ragdollFSMState);

  // Fling the body.
  // (start the ragdoll node before so that the rigid bodies are created)
  FSMNode->GetStateNode (ragdollFSMState)->Play ();
  const csOrthoTransform& tc = view->GetCamera ()->GetTransform ();
  for (uint i = 0; i < ragdollNode->GetBoneCount (RAGDOLL_STATE_DYNAMIC); i++)
  {
    BoneID boneID = ragdollNode->GetBone (RAGDOLL_STATE_DYNAMIC, i);
    iRigidBody* rb = ragdollNode->GetBoneRigidBody (boneID);
    rb->SetLinearVelocity (tc.GetT2O () * csVector3 (0, 0, 5));
    rb->SetAngularVelocity (tc.GetT2O () * csVector3 (5, 5, 0));
  }
}

void AvatarTest::ResetScene ()
{
  // Reset animesh position
  csRef<iMeshObject> animeshObject = scfQueryInterface<iMeshObject> (animesh);
  animeshObject->GetMeshWrapper ()->QuerySceneNode ()->GetMovable ()->SetTransform
    (csOrthoTransform (csMatrix3 (), csVector3 (0.0f)));

  // Reset initial Finite State Machine state
  FSMNode->SwitchToState (mainFSMState);

  // The FSM doesn't stop the child nodes
  ragdollNode->Stop ();

  // Reset 'LookAt' controller
  alwaysRotate = false;
  lookAtNode->SetAlwaysRotate (alwaysRotate);
  lookAtNode->SetTarget (view->GetCamera(), csVector3 (0.0f));
  lookAtNode->SetMaximumSpeed (5.0f);

  // Reset 'speed' controller
  currentSpeed = 0;
  speedNode->SetSpeed (((float) currentSpeed) / 10.0f);

  // Reset morphing
  smileWeight = 1.0f;
  animesh->SetMorphTargetWeight (animeshFactory->FindMorphTarget ("smile.B"), 1.0f);
  animesh->SetMorphTargetWeight (animeshFactory->FindMorphTarget ("eyebrows_down.B"), 1.0f);
  animesh->SetMorphTargetWeight (animeshFactory->FindMorphTarget ("wings_in"), 1.0f);
}

void AvatarTest::WriteShadow (int x, int y, int fg, const char *str,...)
{
  csString buf;

  va_list arg;
  va_start (arg, str);
  buf.FormatV (str, arg);
  va_end (arg);

  Write (x+1, y-1, 0, -1, "%s", buf.GetData());
  Write (x, y, fg, -1, "%s", buf.GetData());
}

void AvatarTest::Write(int x, int y, int fg, int bg, const char *str,...)
{
  va_list arg;
  csString buf;

  va_start (arg,str);
  buf.FormatV (str, arg);
  va_end (arg);

  g2d->Write (courierFont, x, y, fg, bg, buf);
}

void AvatarTest::DisplayKeys ()
{
  int x = 20;
  int y = 20;
  int fg = g2d->FindRGB (255, 150, 100);
  int lineSize = 18;

  WriteShadow (x - 5, y, fg, "Keys available:");
  y += lineSize;

  WriteShadow (x, y, fg, "arrow keys: move camera");
  y += lineSize;

  WriteShadow (x, y, fg, "SHIFT-up/down keys: camera closer/farther");
  y += lineSize;

  WriteShadow (x, y, fg, "+/-: walk faster/slower");
  y += lineSize;

  WriteShadow (x, y, fg, "t: toggle 'LookAt' target mode");
  y += lineSize;

  WriteShadow (x, y, fg, "a: toggle 'LookAt: always rotate' mode");
  y += lineSize;

  WriteShadow (x, y, fg, "s: toggle 'LookAt: rotation speed'");
  y += lineSize;

  if (physicsEnabled)
  {
    WriteShadow (x, y, fg, "k: kill Frankie");
    y += lineSize;
  }

  WriteShadow (x, y, fg, "r: reset scene");
  y += lineSize;
}

/*---------------------------------------------------------------------*
* Main function
*---------------------------------------------------------------------*/
int main (int argc, char* argv[])
{
  iObjectRegistry* object_reg = csInitializer::CreateEnvironment (argc, argv);

  avatarTest = new AvatarTest (object_reg);
  if (avatarTest->Initialize ())
    avatarTest->Start ();
  avatarTest->Shutdown ();
  delete avatarTest; avatarTest = 0;

  csInitializer::DestroyApplication (object_reg);
  return 0;
}
