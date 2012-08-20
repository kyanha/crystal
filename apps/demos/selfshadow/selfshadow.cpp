/*
  Copyright (C) 2011 Alexandru - Teodor Voicu
      Imperial College London
      http://www3.imperial.ac.uk/

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

#include "iengine/campos.h"
#include "selfshadow.h"

SelfShadowDemo::SelfShadowDemo ()
: DemoApplication ("CrystalSpace.SelfShadow")
{
}

void SelfShadowDemo::PrintHelp ()
{
  csCommandLineHelper commandLineHelper;

  // Printing help
  commandLineHelper.PrintApplicationHelp
    (GetObjectRegistry (), "selfshadowdemo", "selfshadowdemo", 
      "Crystal Space's self shadow RM demo.");
}

void SelfShadowDemo::Frame ()
{
  csTicks elapsed_time = vc->GetElapsedTicks ();
  float speed = (elapsed_time / 1000.0) * (0.03 * 20);
  float rotateFactor = speed;

  // Translucent objects become dynamic by being rotated
  if (rotateGrass)
  {
    // Search for mesh named 'Hair'
    csRef<iMeshWrapper> objectfact =
      engine->FindMeshObject ("Hair");

    if (objectfact)
    {
      objectfact->GetMovable()->SetTransform(
        objectfact->GetMovable()->GetTransform() *
        csMatrix3(cos(rotateFactor), 0, sin(rotateFactor),
        0, 1, 0, -sin(rotateFactor), 0, cos(rotateFactor)) );
      objectfact->GetMovable()->UpdateMove();
    }
  }

  // Default behavior from DemoApplication
  DemoApplication::Frame ();
}

bool SelfShadowDemo::OnKeyboard (iEvent &ev)
{
  // Default behavior from csDemoApplication
  DemoApplication::OnKeyboard (ev);
  // First get elapsed time from the virtual clock.
  csTicks elapsed_time = vc->GetElapsedTicks ();
  float speed = (elapsed_time / 1000.0) * (0.03 * 20);

  // Only moves the first light
  csRef<iLight> light = engine->GetSectors()->Get(0)->GetLights()->Get(0);

  float moveFactor = 0;
  csMatrix3 rotateMatrix = csMatrix3();
  float rotateFactor = speed;

  csKeyEventType eventtype = csKeyEventHelper::GetEventType(&ev);
  if (eventtype == csKeyEventTypeDown)
  {
    if (csKeyEventHelper::GetCookedCode (&ev) == 'w')
    {
      rotateMatrix = csMatrix3(1, 0, 0, 0, cos(rotateFactor), 
        -sin(rotateFactor), 0, sin(rotateFactor), cos(rotateFactor));
    }
    else if (csKeyEventHelper::GetCookedCode (&ev) == 's')
    {
      rotateMatrix = csMatrix3(1, 0, 0, 0, cos(rotateFactor), 
        sin(rotateFactor), 0, -sin(rotateFactor), cos(rotateFactor));
    }
    else if (csKeyEventHelper::GetCookedCode (&ev) == 'a')
    {
      rotateMatrix = csMatrix3(cos(rotateFactor), -sin(rotateFactor), 
        0, sin(rotateFactor), cos(rotateFactor), 0, 0, 0, 1);
    }
    else if (csKeyEventHelper::GetCookedCode (&ev) == 'd')
    {
      rotateMatrix = csMatrix3(cos(rotateFactor), sin(rotateFactor), 
        0, -sin(rotateFactor), cos(rotateFactor), 0, 0, 0, 1);
    }
    // Recomputed the split ratio
    else if (csKeyEventHelper::GetCookedCode (&ev) == 'r')
    {
      rm_dbg->DebugCommand("reset_split_ratio");
      return true;
    }
    // Switch between showing the render textures
    else if (csKeyEventHelper::GetCookedCode (&ev) == 't')
    {
      rm_dbg->DebugCommand("show_render_textures");
      return true;
    }
    // Load next scene
    else if (csKeyEventHelper::GetCookedCode (&ev) == 'n')
    {
      // manually clear engine - not removing mesh factories
      engine->GetMeshes()->RemoveAll();
      engine->GetCameraPositions()->RemoveAll();
      engine->GetSectors()->RemoveAll();
      engine->GetMaterialList()->RemoveAll();
      engine->GetTextureList()->RemoveAll();
      engine->GetVariableList()->RemoveAll();
      engine->QueryObject()->ObjRemoveAll ();

      sceneNumber = ( sceneNumber + 1 ) % numberOfScenes;
      CreateScene();
      return true;
    }
    // Load previous scene
    else if (csKeyEventHelper::GetCookedCode (&ev) == 'p')
    {
      // manually clear engine - not removing mesh factories
      engine->GetMeshes()->RemoveAll();
      engine->GetCameraPositions()->RemoveAll();
      engine->GetSectors()->RemoveAll();
      engine->GetMaterialList()->RemoveAll();
      engine->GetTextureList()->RemoveAll();
      engine->GetVariableList()->RemoveAll();
      engine->QueryObject()->ObjRemoveAll ();

      sceneNumber = ( sceneNumber - 1 );
      if (sceneNumber < 0)
        sceneNumber += numberOfScenes;
      CreateScene();
      return true;
    }
    // Start dynamic scene
    else if (csKeyEventHelper::GetCookedCode (&ev) == 'g')
    {
      rotateGrass = !rotateGrass;
      return true;
    }

    // Rotate light
    light->GetMovable()->Transform(rotateMatrix);

    csVector3 oldDirection = light->GetMovable()->GetPosition();
    float oldLength = oldDirection.Norm();
    oldDirection.Normalize();

    csVector3 newDirection = (rotateMatrix * oldDirection);
    float newLength = oldLength + moveFactor;
    newDirection.Normalize();

    csVector3 position = newDirection * newLength;
    light->GetMovable()->SetPosition(position);
    light->GetMovable()->UpdateMove();

    return true;
  }

  return false;
}

bool SelfShadowDemo::OnInitialize (int argc, char* argv[])
{
  // Default behavior from csDemoApplication
  if (!DemoApplication::OnInitialize (argc, argv))
    return false;

  // Load furmesh for krystal scene
  if (!csInitializer::RequestPlugins (GetObjectRegistry (),
    CS_REQUEST_PLUGIN("crystalspace.mesh.object.furmesh", 
      CS::Mesh::iFurMeshType), 
    CS_REQUEST_END))
    return ReportError ("Failed to initialize plugins!");

  return true;
}

bool SelfShadowDemo::Application ()
{
  // Default behavior from DemoApplication
  if (!DemoApplication::Application ())
    return false;

  // Add keys descriptions
  hudManager->GetKeyDescriptions()->Push ("w a s d keys: rotate light");
  hudManager->GetKeyDescriptions()->Push ("r: recompute splitting function");
  hudManager->GetKeyDescriptions()->Push ("t: show render textures");
  hudManager->GetKeyDescriptions()->Push ("g: dynamic grass");
  hudManager->GetKeyDescriptions()->Push ("n: next scene");
  hudManager->GetKeyDescriptions()->Push ("p: previous scene");

  /* NOTE: Config settings for render managers are stored in 'engine.cfg' 
   * and are needed when loading a render manager. Normally these settings 
   * are added by the engine when it loads a render manager. However, since
   * we are loading the shadow_pssm render manager manually we must also manually
   * add the proper config file. */
  csRef<iConfigManager> cfg = 
    csQueryRegistry<iConfigManager> (GetObjectRegistry());
  cfg->AddDomain 
    ("/config/engine.cfg", vfs, iConfigManager::ConfigPriorityPlugin);

  csRef<iRenderManager> rm = csLoadPlugin<iRenderManager> 
    (GetObjectRegistry(), "crystalspace.rendermanager.deferred");
  if (!rm)
    return ReportError("Failed to load Deferred Render Manager!");

  // Load debuger for changing various settings
  rm_dbg = scfQueryInterface<iDebugHelper>(rm);
  sceneNumber = 7;
  numberOfScenes =8;
  rotateGrass = false;

  cfg->RemoveDomain ("/config/engine.cfg");

  engine->SetRenderManager(rm);

  // Create the scene
  if (!CreateScene ())
    return false;

  // Run the application
  Run();

  return true;
}

bool SelfShadowDemo::CreateScene ()
{
  // Invalid scene number
  if (sceneNumber < 0 || sceneNumber >= numberOfScenes)
  {
    ReportError("Invalid scene number!");
    return false;
  }
  // Available scenes
  const char *worlds[] = {"world_krystal", "world", "world_tree", "world_grass", 
    "world_grass_small", "world_grass_big", "world_hair", "world_fur"};

  // Load scene
  printf ("Loading level...\n");
  vfs->ChDir ("/lev/selfshadow");

  if(!vfs->Exists(worlds[sceneNumber]))
  {
    ReportInfo("Extended scenes not present!");
    sceneNumber = 5;
    numberOfScenes = 6;
  }

  if (!loader->LoadMapFile (worlds[sceneNumber], false))
    ReportError("Error couldn't load level!");    

  csRef<iMeshWrapper> meshWrapper =
    engine->FindMeshObject ("Plane");
  if (!meshWrapper)
    ReportInfo ("Can't find Plane object!");
  else
    meshWrapper->SetFlagsRecursive(CS_ENTITY_NOSHADOWCAST);

  // Load krystal
  if (sceneNumber == 0)
  {
    LoadKrystal();
    // command might need to be removed
    rm_dbg->DebugCommand("show_opaque_objects");
  }
  else
    rm_dbg->DebugCommand("show_opaque_objects");
  
  engine->Prepare ();

  // Setup the camera
  cameraManager->SetCamera(view->GetCamera());
  cameraManager->SetCameraMode (CS::Utility::CAMERA_ROTATE);
  cameraManager->SetMotionSpeed (10.0f);
  if (sceneNumber == 0)
    cameraManager->SetCameraTarget(csVector3(0,1.5f,0));
  else
    cameraManager->SetCameraTarget(csVector3(0));

  printf ("Ready!\n");

  return true;
}

void SelfShadowDemo::LoadKrystal()
{
  iSector* sector = engine->FindSector("Scene");

  if (!sector)
    ReportError("Could not find default room!");

  printf ("Loading Krystal...\n");

  // Load animesh factory
  csLoadResult rc = loader->Load ("/lib/krystal/krystal.xml");
  if (!rc.success)
    ReportError ("Can't load Krystal library file!");

  csRef<iMeshFactoryWrapper> meshfact =
    engine->FindMeshFactory ("krystal");
  if (!meshfact)
    ReportError ("Can't find Krystal's mesh factory!");

  csRef<CS::Mesh::iAnimatedMeshFactory> animeshFactory = 
    scfQueryInterface<CS::Mesh::iAnimatedMeshFactory>
    (meshfact->GetMeshObjectFactory ());
  if (!animeshFactory)
    ReportError ("Can't find Krystal's animesh factory!");

  // Create the animated mesh
  csRef<iMeshWrapper> avatarMesh =
    engine->CreateMeshWrapper (meshfact, "krystal",
    room, csVector3 (0.0f));

  csRef<CS::Mesh::iAnimatedMesh> animesh = 
    scfQueryInterface<CS::Mesh::iAnimatedMesh> (avatarMesh->GetMeshObject ());
  avatarMesh->GetMovable()->SetSector(sector);
  avatarMesh->GetMovable()->UpdateMove();

  // Load some fur
  rc = loader->Load ("/lib/hairtest/krystal_furmesh.xml");
  if (!rc.success)
    ReportError ("Can't load krystal furmesh library!");

  csRef<iMeshWrapper> krystalFurmeshObject = 
    engine->FindMeshObject ("krystal_furmesh_object");
  if (!krystalFurmeshObject)
    ReportError ("Can't find fur mesh object!");
  krystalFurmeshObject->SetRenderPriority(engine->GetRenderPriority("alpha"));

  krystalFurmeshObject->GetMovable()->SetSector(sector);
  krystalFurmeshObject->GetMovable()->UpdateMove();

  // Load the fur material
  rc = loader->Load ("/lib/hairtest/fur_material_krystal.xml");
  if (!rc.success)
    ReportError ("Can't load Fur library file!");

  // Find the fur mesh plugin
  csRef<CS::Mesh::iFurMeshType> furMeshType = 
    csQueryRegistry<CS::Mesh::iFurMeshType> (GetObjectRegistry ());
  if (!furMeshType)
    ReportError("Failed to locate CS::Mesh::iFurMeshType plugin!");

  // Load the Marschner shader
  csRef<iMaterialWrapper> materialWrapper = 
    engine->FindMaterial ("hair_trans");
  if (!materialWrapper)
    ReportError ("Can't find marschner material!");

  // Create the fur properties for the hairs
  csRef<CS::Mesh::iFurMeshMaterialProperties> hairMeshProperties = 
    furMeshType->CreateHairMeshMarschnerProperties ("krystal_marschner");
  hairMeshProperties->SetMaterial(materialWrapper->GetMaterial ());
//   animesh->GetSubMesh (1)->SetMaterial (materialWrapper);

  csRef<CS::Animation::iFurAnimatedMeshControl> animationPhysicsControl = 
    scfQueryInterface<CS::Animation::iFurAnimatedMeshControl>
    (furMeshType->CreateFurAnimatedMeshControl ("krystal_hairs_animation"));

  animationPhysicsControl->SetAnimatedMesh (animesh);

  csRef<iMeshObject> imo = krystalFurmeshObject->GetMeshObject();

  // Get reference to the iFurMesh interface
  csRef<CS::Mesh::iFurMesh> furMesh = 
    scfQueryInterface<CS::Mesh::iFurMesh> (imo);

  csRef<CS::Mesh::iFurMeshState> ifms = 
    scfQueryInterface<CS::Mesh::iFurMeshState> (furMesh);

  animationPhysicsControl->SetDisplacement (ifms->GetDisplacement ());

  furMesh->SetFurMeshProperties (hairMeshProperties);

  furMesh->SetAnimatedMesh (animesh);
  furMesh->SetMeshFactory (animeshFactory);
  furMesh->SetMeshFactorySubMesh 
    (animesh->GetSubMesh (2)->GetFactorySubMesh ());
  furMesh->GenerateGeometry (view, room);

  furMesh->SetAnimationControl (animationPhysicsControl);
  furMesh->StartAnimationControl ();

  furMesh->SetGuideLOD (0);
  furMesh->SetStrandLOD (1);
  furMesh->SetControlPointsLOD (0.0f);

  furMesh->ResetMesh ();
}
