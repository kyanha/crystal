/*
    Copyright (C) 2012 by Christian Van Brussel

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
#include "cstool/initapp.h"
#include "csutil/event.h"
#include "csutil/scf.h"
#include "ieditor/context.h"
#include "ieditor/operator.h"
#include "iengine/engine.h"
#include "iengine/camera.h"
#include "iengine/campos.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "iengine/sector.h"
#include "imap/loader.h"
#include "imap/saver.h"
#include "imesh/particles.h"
#include "iutil/object.h"
#include "iutil/plugin.h"
#include "ivaria/pmeter.h"

#include "scenemanager.h"

#include <wx/frame.h>

CS_PLUGIN_NAMESPACE_BEGIN(CSEditor)
{

SCF_IMPLEMENT_FACTORY (SceneManager)

SceneManager::SceneManager (iBase* parent)
  : scfImplementationType (this, parent), internalChange (false)
{

}

SceneManager::~SceneManager()
{
}

bool SceneManager::Initialize (iEditor* editor)
{
  this->editor = editor;
  object_reg = editor->GetContext ()->GetObjectRegistry ();

  engine = csQueryRegistry<iEngine> (object_reg);

  // Register this event handler to the editor events
  iEventNameRegistry* registry =
    csEventNameRegistry::GetRegistry (object_reg);
  eventSetActiveObject =
    registry->GetID ("crystalspace.editor.context.selection.setactiveobject");
  eventSetCamera =
    registry->GetID ("crystalspace.editor.context.camera.setcamera");
  eventSetCollection = 
    registry->GetID ("crystalspace.editor.context.fileloader.setcollection");

  csEventID events[] = {
    eventSetActiveObject,
    eventSetCamera,
    eventSetCollection,
    CS_EVENTLIST_END
  };

  RegisterQueue (editor->GetContext ()->GetEventQueue (), events);

  // Register a mouse listener to the global event queue
  eventListener.AttachNew (new SceneManager::EventListener (this));

  return true;
}

void SceneManager::Update ()
{
}

void SceneManager::Save (iDocumentNode* node) const
{
}

bool SceneManager::Load (iDocumentNode* node)
{
  return false;
}

bool SceneManager::HandleEvent (iEvent &event)
{
  if (internalChange) return false;

  csRef<iContextCamera> cameraContext =
    scfQueryInterface<iContextCamera> (editor->GetContext ());
  iCamera* camera = cameraContext->GetCamera ();

  if (event.Name == eventSetCamera)
  {
    OnSetCamera (camera);
    return false;
  }

  if (event.Name == eventSetActiveObject)
  {
    OnSetActiveObject (camera);
    return false;
  }

  if (event.Name == eventSetCollection)
  {
    OnSetCollection (camera);
    return false;
  }

  return false;
}

void SceneManager::OnSetCamera (iCamera* camera)
{
  // Setup the camera manager
  csRef<iPluginManager> pluginManager =
    csQueryRegistry<iPluginManager> (object_reg);

  cameraManager = csLoadPlugin<CS::Utility::iCameraManager>
    (pluginManager, "crystalspace.utilities.cameramanager");
  if (!cameraManager)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.component.scenemanager",
	      "Failed to locate the camera manager plugin!");
    return;
  }
  cameraManager->SetCamera (camera);

  // If the camera is not in a sector then try to find one
  if (camera && !camera->GetSector ())
    OnSetCollection (camera);
}

void SceneManager::OnSetActiveObject (iCamera* camera)
{
  if (!camera) return;

  csRef<iContextObjectSelection> objectSelectionContext =
    scfQueryInterface<iContextObjectSelection> (editor->GetContext ());
  iObject* object = objectSelectionContext->GetActiveObject ();
  if (!object) return;

  // Search the type of the object

  // If it's a iCameraPosition then move the camera there
  csRef<iCameraPosition> cameraPosition =
    scfQueryInterface<iCameraPosition> (object);

  if (cameraPosition)
  {
    cameraPosition->Load (camera, engine);
    cameraManager->SetStartPosition (camera->GetTransform ().GetOrigin ());
    cameraManager->SetCameraMode (CS::Utility::CAMERA_MOVE_FREE);
    cameraManager->SetMotionSpeed (5.0f);
    cameraManager->ResetCamera ();

    return;
  }

  // If it's a iMeshFactoryWrapper then create a new sector to
  // visualize it
  csRef<iMeshFactoryWrapper> meshFactory =
    scfQueryInterface<iMeshFactoryWrapper> (object);

  if (meshFactory)
  {
    CreateViewmeshScene (meshFactory, camera);
    return;
  }

  // If it's a iMeshWrapper then visualize it
  csRef<iMeshWrapper> mesh =
    scfQueryInterface<iMeshWrapper> (object);

  if (mesh)
  {
    iSectorList* sectorList = mesh->GetMovable ()->GetSectors ();

    // TODO: if not in a sector then put it in a temporary one
    if (!sectorList->GetCount ()) return;

    camera->SetSector (sectorList->Get (0));
    csBox3 bbox = mesh->GetWorldBoundingBox ();
    PositionCamera (camera, bbox);
    cameraManager->SetCameraMode (CS::Utility::CAMERA_ROTATE);
    cameraManager->SetMotionSpeed (5.0f);
    cameraManager->ResetCamera ();

    return;
  }

  // If it's a iLight then visualize it
  csRef<iLight> light =
    scfQueryInterface<iLight> (object);

  if (light)
  {
    iSectorList* sectorList = light->GetMovable ()->GetSectors ();
    if (!sectorList->GetCount ()) return;

    camera->SetSector (sectorList->Get (0));
    csBox3 bbox = light->GetWorldBBox ();
    bbox.SetSize (bbox.GetSize () * 0.5f);
    PositionCamera (camera, bbox);
    cameraManager->SetCameraMode (CS::Utility::CAMERA_ROTATE);
    cameraManager->SetMotionSpeed (5.0f);
    cameraManager->ResetCamera ();

    return;
  }

  // If it's a iSector then visualize it
  csRef<iSector> sector =
    scfQueryInterface<iSector> (object);

  if (sector)
  {
    csBox3 bbox;
    // TODO: is it normal that the value computed by
    // CalculateSectorBBox is not usable?
    //sector->CalculateSectorBBox (bbox, true);
    iMeshList* meshList = sector->GetMeshes ();
    for (int i = 0; i < meshList->GetCount (); i++)
    {
      iMeshWrapper* mesh = meshList->Get (i);
      const csBox3& meshBbox = mesh->GetWorldBoundingBox ();
      bbox += meshBbox;
    }

    camera->SetSector (sector);
    PositionCamera (camera, bbox);
    cameraManager->SetCameraMode (CS::Utility::CAMERA_ROTATE);
    cameraManager->SetMotionSpeed (5.0f);
    cameraManager->ResetCamera ();

    return;
  }
}

void SceneManager::OnSetCollection (iCamera* camera)
{
  if (!camera) return;

  csRef<iContextFileLoader> fileLoaderContext =
    scfQueryInterface<iContextFileLoader> (editor->GetContext ());
  iCollection* collection = fileLoaderContext->GetCollection ();

  csRef<iContextObjectSelection> objectSelectionContext =
    scfQueryInterface<iContextObjectSelection> (editor->GetContext ());

  if (!collection)
  {
    camera->SetSector (nullptr);
    return;
  }

  // Analyze the collection to find a suitable starting position
  // for the camera
  iCameraPosition* firstCameraPosition = nullptr;
  iSector* firstSector = nullptr;
  iMeshFactoryWrapper* firstMeshFactory = nullptr;

  iObject* collisionObject = collection->QueryObject ();
  for (csRef<iObjectIterator> it = collisionObject->GetIterator (); it->HasNext (); )
  {
    iObject* object = it->Next ();

    csRef<iMeshFactoryWrapper> meshFactory =
      scfQueryInterface<iMeshFactoryWrapper> (object);
    if (meshFactory)
    {
      if (!firstMeshFactory) firstMeshFactory = meshFactory;
      continue;
    }

    csRef<iSector> sector =
      scfQueryInterface<iSector> (object);
    if (sector)
    {
      if (!firstSector) firstSector = sector;
      continue;
    }

    csRef<iCameraPosition> cameraPosition =
      scfQueryInterface<iCameraPosition> (object);
    if (cameraPosition)
    {
      if (!firstCameraPosition) firstCameraPosition = cameraPosition;
      break;
    }
   }

  // If there is a valid starting position defined in the level file
  // then go to it.
  if (firstCameraPosition)
  {
    firstCameraPosition->Load (camera, engine);
    cameraManager->SetStartPosition (camera->GetTransform ().GetOrigin ());
    cameraManager->SetCameraTarget (csVector3 (0.0f));
    cameraManager->SetCameraMode (CS::Utility::CAMERA_MOVE_FREE);
    cameraManager->SetMotionSpeed (5.0f);
    cameraManager->ResetCamera ();
    return;
  }

  // If there is a sector then go to its origin
  if (firstSector)
  {
    camera->SetSector (firstSector);
    camera->GetTransform ().SetO2T (csMatrix3 ());
    cameraManager->SetStartPosition (csVector3 (0.0f));
    cameraManager->SetCameraTarget (csVector3 (0.0f));
    cameraManager->SetCameraMode (CS::Utility::CAMERA_ROTATE);
    cameraManager->SetMotionSpeed (5.0f);
    cameraManager->ResetCamera ();
    return;
  }

  // If there is a mesh factory then create a dedicated sector for it and set it
  // as the active object
  if (firstMeshFactory)
  {
    CreateViewmeshScene (firstMeshFactory, camera);
    internalChange = true;
    objectSelectionContext->SetActiveObject (firstMeshFactory->QueryObject ());
    internalChange = false;
    return;
  }
}

void SceneManager::CreateViewmeshScene (iMeshFactoryWrapper* meshFactory,
					iCamera* camera)
{
  // Create a temporary sector
  meshSector = engine->CreateSector ("viewmesh");

  // Setup the camera
  camera->SetSector (meshSector);
  cameraManager->SetCameraMode (CS::Utility::CAMERA_ROTATE);

  // Hack: if this is a particle mesh factory, then set its deep creation
  // parameter to 'false'.
  csRef<iParticleSystemFactory> partSys =
    scfQueryInterface<iParticleSystemFactory> (meshFactory->GetMeshObjectFactory ()); 
  bool deepCreation;
  if (partSys)
  {
    deepCreation = partSys->GetDeepCreation ();
    partSys->SetDeepCreation (false);
  }

  // Create the mesh
  csRef<iMeshWrapper> meshWrapper =
    engine->CreateMeshWrapper (meshFactory, "viewmesh", meshSector);

  // Put back the 'deep creation' parameter
  if (partSys) partSys->SetDeepCreation (deepCreation);

  // Setup the camera manager
  csBox3 bbox = meshWrapper->GetWorldBoundingBox ();
  cameraManager->SetCameraTarget (bbox.GetCenter ());
  float boxSize = bbox.GetSize ().Norm ();
  cameraManager->SetStartPosition
    (bbox.GetCenter () + csVector3 (0.0f, 0.0f, - boxSize));
  cameraManager->SetMotionSpeed (boxSize * 5.0f);
  cameraManager->ResetCamera ();

  float roomSize = bbox.GetSize ().Norm () * 5.0f;

  // Initialize the lights
  csRef<iLight> light;
  light = engine->CreateLight
    (0, csVector3 (0.0f), 100.0f, csColor(1, 1, 1));
  meshSector->GetLights ()->Add (light);

  light = engine->CreateLight
    (0, csVector3 (0.0f), 100.0f, csColor(1, 1, 1));
  meshSector->GetLights ()->Add (light);

  light = engine->CreateLight
    (0, csVector3(0.0f), 100.0f, csColor(1, 1, 1));
  meshSector->GetLights ()->Add (light);

  iLightList* ll = meshSector->GetLights ();
  ll->Get (0)->GetMovable()->SetPosition
    (csVector3 (-roomSize / 2.0f, roomSize / 2.0f, 0.0f));
  ll->Get (1)->GetMovable()->SetPosition
    (csVector3 (roomSize / 2.0f,  -roomSize / 2.0f, 0.0f));
  ll->Get (2)->GetMovable()->SetPosition
    (csVector3 (0.0f, 0.0f, -roomSize / 2.0f));

  ll->Get (0)->SetCutoffDistance (roomSize);
  ll->Get (1)->SetCutoffDistance (roomSize);
  ll->Get (2)->SetCutoffDistance (roomSize);

  ll->Get (0)->GetMovable()->UpdateMove();
  ll->Get (1)->GetMovable()->UpdateMove();
  ll->Get (2)->GetMovable()->UpdateMove();
}

void SceneManager::PositionCamera (iCamera* camera, csBox3& bbox)
{
  float boxSize = bbox.GetSize ().Norm ();
  cameraManager->SetCameraTarget (bbox.GetCenter ());
  cameraManager->SetStartPosition
    (bbox.GetCenter () + csVector3 (0.0f, 0.0f, - boxSize));
  camera->GetTransform ().LookAt
    (csVector3 (0.0f, 0.0f, boxSize), csVector3 (0.0f, 1.0f, 0.0f));
}

// -------------------------------------------------------------------------------------

SceneManager::EventListener::EventListener (SceneManager* manager)
  : scfImplementationType (this), manager (manager)
{
  csRef<iEventQueue> eventQueue =
    csQueryRegistry<iEventQueue> (manager->object_reg);

  csEventID events[] = {
    csevMouseDown (manager->object_reg, 0),
    csevKeyboardEvent (manager->object_reg),
    CS_EVENTLIST_END
  };

  eventQueue->RegisterListener (this, events);
}

bool SceneManager::EventListener::HandleEvent (iEvent &event)
{
  iEventNameRegistry* nameRegistry = csEventNameRegistry::GetRegistry (manager->object_reg);

  // Object selection through mouse click
  if (event.Name == csevMouseDown (nameRegistry, 0)
      && csMouseEventHelper::GetButton (&event) == 0)
  {
    iOperatorManager* operatorManager = manager->editor->GetOperatorManager ();
    csRef<iOperator> op = operatorManager->CreateOperator ("crystalspace.editor.operator.select");
    operatorManager->Invoke (op, &event);
    return true;
  }

  // Object motion through keyboard key
  if (CS_IS_KEYBOARD_EVENT (manager->object_reg, event))
  {
    if (csKeyEventHelper::GetEventType (&event) == csKeyEventTypeDown
	&& csKeyEventHelper::GetCookedCode (&event) == 'g')
    {
      iOperatorManager* operatorManager = manager->editor->GetOperatorManager ();
      csRef<iOperator> op = operatorManager->CreateOperator ("crystalspace.editor.operator.move");
      operatorManager->Invoke (op, &event);
      return true;
    }
  }

  return false;
}

}
CS_PLUGIN_NAMESPACE_END(CSEditor)

