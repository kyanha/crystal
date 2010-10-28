/*
  Copyright (C) 2010 Christian Van Brussel, Communications and Remote
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

#include "cssysdef.h"
#include "cstool/csview.h"
#include "cstool/cspixmap.h"
#include "cstool/genmeshbuilder.h"
#include "cstool/materialbuilder.h"
#include "cstool/csdemoapplication.h"

namespace CS
{
namespace Demo
{

// ------------------------ CommandLineHelper ------------------------

CommandLineHelper::CommandLineHelper (const char* applicationCommand,
				      const char* applicationCommandUsage,
				      const char* applicationDescription)
  : applicationCommand (applicationCommand),
    applicationCommandUsage (applicationCommandUsage),
    applicationDescription (applicationDescription)
{
}

void CommandLineHelper::AddCommandLineOption (const char* option,
						    const char* description)
{
  commandOptions.Push (CommandOption (option, description));
}

void CommandLineHelper::WriteHelp (iObjectRegistry* registry) const
{
  csPrintf ("%s\n\n", applicationDescription.GetData ());
  csPrintf ("Usage: %s\n\n", applicationCommandUsage.GetData ());
  csPrintf ("Available options:\n\n");

  if (commandOptions.GetSize ())
    csPrintf ("Specific options for %s:\n", applicationCommand.GetData ());

  for (csArray<CommandOption>::ConstIterator it = commandOptions.GetIterator ();
       it.HasNext (); )
  {
    CommandOption commandOption = it.Next ();
    csPrintf ("  -%-*s%s\n", 18, commandOption.option.GetData (),
	      commandOption.description.GetData ());
  }

  if (commandOptions.GetSize ())
    csPrintf ("\n");

  csCommandLineHelper::Help (registry);
}

// ------------------------ HUDHelper ------------------------

HUDHelper::HUDHelper ()
  : applicationFramework (nullptr), cslogo (0),
    frameCount (0), frameTime (0), currentFPS (0.0f), currentKeyPage (0)
{
}

HUDHelper::~HUDHelper ()
{
  delete cslogo;
}

bool HUDHelper::SetContext (csApplicationFramework* applicationFramework,
			    iGraphics3D* g3d, iGraphics2D* g2d, iLoader* loader,
			    iVirtualClock* vc)
{
  // Init the application framework
  if (!applicationFramework)
  {
    printf ("ERROR in HUDHelper::SetContext: No csApplicationFramework defined!\n");
    return false;
  }
  this->applicationFramework = applicationFramework;

  // Copy the pointers
  if (!g3d) return applicationFramework->ReportError ("no 3D graphics!");
  this->g3d = g3d;
  if (!g2d) return applicationFramework->ReportError ("no 2D graphics!");
  this->g2d = g2d;
  if (!loader) return applicationFramework->ReportError ("no loader!");
  //this->loader = loader;
  if (!vc) return applicationFramework->ReportError ("no virtual clock!");
  this->vc = vc;

  // Load the font
  csRef<iFontServer> fontServer = g2d->GetFontServer ();
  if (!fontServer)
    return applicationFramework->ReportError ("Failed to locate font server!");
  font = fontServer->LoadFont (CSFONT_COURIER);
  if (!font)
    return applicationFramework->ReportError ("Failed to load font!");

  // Load the Crystal Space logo image
  csRef<iTextureWrapper> texture = loader->LoadTexture
    ("cslogo2", "/lib/std/cslogo2.png", CS_TEXTURE_2D, 0, true, true, true);
  if (!texture.IsValid ())
    applicationFramework->ReportWarning ("Failed to load CS logo!\n");

  else
  {
    // Create a 2D sprite for the logo
    iTextureHandle* textureHandle = texture->GetTextureHandle ();
    if (textureHandle)
      cslogo = new csSimplePixmap (textureHandle);
  }

  return true;
}

bool HUDHelper::OnKeyboard (iEvent &event)
{
  csKeyEventType eventtype = csKeyEventHelper::GetEventType(&event);
  if (eventtype == csKeyEventTypeDown)
  {
    if (csKeyEventHelper::GetCookedCode (&event) == CSKEY_F1
	&& keyDescriptions.GetSize ())
      currentKeyPage =
	(currentKeyPage + 1) % (keyDescriptions.GetSize () / maxKeys + 1);
  }

  return false;
}

void HUDHelper::WriteShadow (int x, int y, int fg, const char *str,...)
{
  csString buf;
  va_list arg;

  va_start (arg, str);
  buf.FormatV (str, arg);
  va_end (arg);

  Write (x + 1, y - 1, 0, -1, "%s", buf.GetData ());
  Write (x, y, fg, -1, "%s", buf.GetData ());
}

void HUDHelper::Write (int x, int y, int fg, int bg, const char *str,...)
{
  csString buf;
  va_list arg;

  va_start (arg,str);
  buf.FormatV (str, arg);
  va_end (arg);

  g2d->Write (font, x, y, fg, bg, buf);
}

void HUDHelper::DisplayHUD ()
{
  // Get elasped time
  csTicks elapsed_time = vc->GetElapsedTicks ();

  // Update FPS data
  frameCount++;
  frameTime += elapsed_time;

  if (frameTime > 500)
  {
    currentFPS = ((float) (frameCount * 1000)) / (float) frameTime;
    frameCount = 0;
    frameTime = 0;
  }

  // Tell the 3D driver we're going to display 2D things.
  if (!g3d->BeginDraw (CSDRAW_2DGRAPHICS))
    return;

  int margin = 15;
  int logoMargin = 5;
  int lineSize = 18;
  int fontColor = g2d->FindRGB (255, 150, 100);

  // Display available keys
  if (keyDescriptions.GetSize ())
  {
    int y = margin;

    // Check if there is enough room to display all keys
    maxKeys = (uint) ((g2d->GetHeight () - 2 * margin
		       - (stateDescriptions.GetSize () + 5) * lineSize)
		      / lineSize);
    if (keyDescriptions.GetSize () < maxKeys)
    {
      currentKeyPage = 0;
      WriteShadow (margin, y, fontColor, "Keys available:");
    }
    else
      WriteShadow (margin, y, fontColor, "Keys available (%i/%i):",
		   currentKeyPage + 1, keyDescriptions.GetSize () / maxKeys + 1);
    y += lineSize;

    // Write all keys
    uint index = 0;
    for (csStringArray::Iterator it = keyDescriptions.GetIterator ();
	 it.HasNext (); index++)
    {
      if (index / maxKeys == currentKeyPage)
      {
	WriteShadow (margin + 5, y, fontColor, it.Next ());
	y += lineSize;
      }
      else
	it.Next ();
    }

    if (keyDescriptions.GetSize () > maxKeys)
    {
      WriteShadow (margin, y, fontColor, "F1: more keys");
      y += lineSize;
    }
  }

  // Display state description
  int y = g2d->GetHeight () - margin - lineSize;

  WriteShadow (margin, y, fontColor, "FPS: %.2f", currentFPS);
  y -= lineSize;

  for (csStringArray::ReverseIterator it = stateDescriptions.GetReverseIterator ();
       it.HasNext (); )
  {
    WriteShadow (margin, y, fontColor, it.Next ());
    y -= lineSize;
  }

  // Display Crystal Space logo
  if (cslogo)
    cslogo->Draw (g3d,
		  g2d->GetWidth () - cslogo->Width() - logoMargin,
		  logoMargin);
}

// ------------------------ CameraHelper ------------------------

CameraHelper::CameraHelper ()
  : cameraMode (CSDEMO_CAMERA_MOVE_FREE), mouseMoveEnabled (true)
{
}

bool CameraHelper::SetContext (CameraManager* manager, iKeyboardDriver* kbd,
			       iVirtualClock* vc, iCamera* camera)
{
  if (!manager)
    { printf ("ERROR in CameraHelper::SetContext: no camera manager!"); return false; }
  if (!kbd)
    { printf ("ERROR in CameraHelper::SetContext: no keyboard driver!"); return false; }
  if (!vc)
    { printf ("ERROR in CameraHelper::SetContext: no virtual clock!"); return false; }
  if (!camera)
    { printf ("ERROR in CameraHelper::SetContext: no camera!"); return false; }

  this->cameraManager = manager;
  this->kbd = kbd;
  this->vc = vc;
  this->camera = camera;

  if (cameraMode != CSDEMO_CAMERA_NONE)
  {
    ResetCamera ();
    UpdateCamera ();

    cameraModePan = false;
    cameraModeRotate = false;
    cameraModeZoom = false;
  }

  return true;
}

void CameraHelper::Frame ()
{
  // Get elasped time
  csTicks elapsed_time = vc->GetElapsedTicks ();
  float speed = (elapsed_time / 1000.0) * (0.06 * 20);

  // Update the camera
  csVector3 cameraOrigin = camera->GetTransform ().GetOrigin ();
  csVector3 cameraTarget = cameraManager->GetCameraTarget ();

  switch (cameraMode)
  {
  case CSDEMO_CAMERA_MOVE_FREE:
    {
      if (kbd->GetKeyState (CSKEY_SHIFT))
      {
	// If the user is holding down shift, the arrow keys will cause
	// the camera to strafe up, down, left or right from it's
	// current position.
	if (kbd->GetKeyState (CSKEY_RIGHT))
	  camera->Move (CS_VEC_RIGHT * 4 * speed);
	if (kbd->GetKeyState (CSKEY_LEFT))
	  camera->Move (CS_VEC_LEFT * 4 * speed);
	if (kbd->GetKeyState (CSKEY_UP))
	  camera->Move (CS_VEC_UP * 4 * speed);
	if (kbd->GetKeyState (CSKEY_DOWN))
	  camera->Move (CS_VEC_DOWN * 4 * speed);
      }
      else
      {
	// left and right cause the camera to rotate on the global Y
	// axis; page up and page down cause the camera to rotate on the
	// _camera's_ X axis (more on this in a second) and up and down
	// arrows cause the camera to go forwards and backwards.
	if (kbd->GetKeyState (CSKEY_RIGHT))
	  camera->GetTransform ().RotateThis (CS_VEC_ROT_RIGHT, speed);
	if (kbd->GetKeyState (CSKEY_LEFT))
	  camera->GetTransform ().RotateThis (CS_VEC_ROT_LEFT, speed);
	if (kbd->GetKeyState (CSKEY_PGUP))
	  camera->GetTransform ().RotateThis (CS_VEC_TILT_UP, speed);
	if (kbd->GetKeyState (CSKEY_PGDN))
	  camera->GetTransform ().RotateThis (CS_VEC_TILT_DOWN, speed);
	if (kbd->GetKeyState (CSKEY_UP))
	  camera->Move (CS_VEC_FORWARD * 4 * speed);
	if (kbd->GetKeyState (CSKEY_DOWN))
	  camera->Move (CS_VEC_BACKWARD * 4 * speed);
      }

      break;
    }

  case CSDEMO_CAMERA_MOVE_LOOKAT:
    {
      if (kbd->GetKeyState (CSKEY_DOWN))
	cameraOrigin.z -= 4 * speed;
      if (kbd->GetKeyState (CSKEY_UP))
	cameraOrigin.z += 4 * speed;
      if (kbd->GetKeyState (CSKEY_LEFT))
	cameraOrigin.x -= 4 * speed;
      if (kbd->GetKeyState (CSKEY_RIGHT))
	cameraOrigin.x += 4 * speed;
      if (kbd->GetKeyState (CSKEY_PGUP))
	cameraOrigin.y += 4 * speed;
      if (kbd->GetKeyState (CSKEY_PGDN))
	cameraOrigin.y -= 4 * speed;
      UpdateCameraOrigin (cameraOrigin);
      UpdateCamera ();
      break;
    }

  case CSDEMO_CAMERA_ROTATE:
    {
      if (kbd->GetKeyState (CSKEY_LEFT))
	cameraYaw += speed * 2.5f;
      if (kbd->GetKeyState (CSKEY_RIGHT))
	cameraYaw -= speed * 2.5f;
      if (kbd->GetKeyState (CSKEY_UP))
      {
	if (kbd->GetKeyState (CSKEY_SHIFT))
	  cameraDist =
	    csMax<float> (cameraManager->GetCameraMinimumDistance (),
			  cameraDist - speed * 4);
	else
	  cameraPitch =
	    csMax<float> (-3.14159f * 0.5f + 0.01f, cameraPitch - speed * 2.5f);
      }
      if (kbd->GetKeyState (CSKEY_DOWN))
      {
	if (kbd->GetKeyState (CSKEY_SHIFT))
	  cameraDist += speed * 4;
	else
	  cameraPitch =
	    csMin<float> (3.14159f * 0.5f - 0.01f, cameraPitch + speed * 2.5f);
      }
      if (kbd->GetKeyState (CSKEY_PGUP))
	cameraDist =
	  csMax<float> (cameraManager->GetCameraMinimumDistance (),
			cameraDist - speed * 4);
      if (kbd->GetKeyState (CSKEY_PGDN))
	cameraDist += speed * 4;
      UpdateCamera ();
      break;
    }
  default:
    break;
  }
}

bool CameraHelper::OnMouseDown (iEvent &event)
{
  if (!mouseMoveEnabled)
    return false;

  const float mouseWheelZoomAmount = 0.25f;

  uint button = csMouseEventHelper::GetButton (&event);
  switch (button)
  {
  case 0:
    cameraModePan = true;
    panCameraTarget = cameraManager->GetCameraTarget ();
    break;
  case 1:
    cameraModeRotate = true;
    break;
  case 2:
    cameraModeZoom = true;
    break;
  case 3:
    cameraDist = csMax<float> (cameraManager->GetCameraMinimumDistance (),
			       cameraDist - mouseWheelZoomAmount);
    UpdateCamera ();
    break;
  case 4:
    cameraDist = csMax<float> (cameraManager->GetCameraMinimumDistance (),
			       cameraDist + mouseWheelZoomAmount);
    UpdateCamera ();
    break;
  }
  return false;
}

bool CameraHelper::OnMouseUp (iEvent &event)
{
  if (!mouseMoveEnabled)
    return false;

  uint button = csMouseEventHelper::GetButton (&event);
  switch (button)
  {
  case 0:
    cameraModePan = false;
    UpdateCameraOrigin
      (camera->GetTransform ().GetOrigin ());
    UpdateCamera ();
    break;
  case 1:
    cameraModeRotate = false;
    break;
  case 2:
    cameraModeZoom = false;
    break;
  }

  return false;
}

bool CameraHelper::OnMouseMove (iEvent &event)
{
  if (!mouseMoveEnabled)
    return false;

  int x = csMouseEventHelper::GetX (&event);
  int y = csMouseEventHelper::GetY (&event);
  float dx = (float) (x - lastMouseX) * 0.02f;
  float dy = (float) (y - lastMouseY) * -0.02f;

  lastMouseX = x;
  lastMouseY = y;

  if (cameraModePan)
  {
    panCameraTarget +=
      camera->GetTransform ().This2OtherRelative (csVector3 (1,0,0)) * dx 
      + camera->GetTransform ().This2OtherRelative (csVector3 (0,1,0)) * dy;
  }
  if (cameraModeRotate)
  {
    cameraYaw += dx;
    cameraPitch += dy;
    cameraPitch = csMax<float> (-3.14159f * 0.5f + 0.01f, cameraPitch);
    cameraPitch = csMin<float> (3.14159f * 0.5f - 0.01f, cameraPitch);
  }
  if (cameraModeZoom)
  {
    cameraDist = csMax<float> (cameraManager->GetCameraMinimumDistance (),
			       cameraDist - (dx + dy));
  }

  if (cameraModePan || cameraModeRotate || cameraModePan)
    UpdateCamera ();

  return false;
}

void CameraHelper::SetCameraMode (CameraMode cameraMode)
{
  this->cameraMode = cameraMode;
}

CameraMode CameraHelper::GetCameraMode ()
{
  return cameraMode;
}

void CameraHelper::SetMouseMoveEnabled (bool enabled)
{
  mouseMoveEnabled = enabled;
}

bool CameraHelper::GetMouseMoveEnabled ()
{
  return mouseMoveEnabled;
}

void CameraHelper::ResetCamera ()
{
  cameraModePan = false;
  cameraModeRotate = false;
  cameraModeZoom = false;

  UpdateCameraOrigin (cameraManager->GetCameraStart ());
  UpdateCamera ();
}

void CameraHelper::UpdateCamera ()
{
  csVector3 cameraPos;
  csVector3 cameraTarget = cameraModePan ?
    panCameraTarget : cameraManager->GetCameraTarget ();

  cameraPos.x = cameraTarget.x
    - cameraDist * (float) cos (cameraPitch) * (float) sin (cameraYaw);
  cameraPos.y = cameraTarget.y
    - cameraDist * (float) sin (cameraPitch);
  cameraPos.z = cameraTarget.z
    - cameraDist * (float) cos (cameraPitch) * (float) cos (cameraYaw);

  camera->GetTransform ().SetOrigin (cameraPos);
  camera->GetTransform ().LookAt
    (cameraTarget - cameraPos, csVector3 (0,1,0));
}

void CameraHelper::UpdateCameraOrigin
(const csVector3& desiredOrigin)
{
  // Compute distance, yaw, and pitch values that will put the
  // origin at the desired origin
  csVector3 cameraTarget = cameraManager->GetCameraTarget ();
  cameraDist = (cameraTarget - desiredOrigin).Norm ();

  if (fabs (cameraDist) < EPSILON)
  {
    cameraPitch = 0.0f;
    cameraYaw = 0.0f;
  }

  else
  {
    cameraPitch =
      (float) asin ((cameraTarget.y - desiredOrigin.y) / cameraDist);

    cameraYaw = (float) asin ((cameraTarget.x - desiredOrigin.x)
			      / (cameraDist * (float) cos (cameraPitch)));
    if ((cameraTarget.z - desiredOrigin.z)
	/ (cameraDist * (float) cos (cameraPitch)) < 0.0f)
      cameraYaw = 3.14159f - cameraYaw;
  }
}

// ------------------------ DemoApplication ------------------------

DemoApplication::DemoApplication (const char* applicationName,
				  const char* applicationCommand,
				  const char* applicationCommandUsage,
				  const char* applicationDescription)
  : commandLineHelper (applicationCommand, applicationCommandUsage, applicationDescription),
    hudDisplayed (true)
{
  SetApplicationName (applicationName);
}

void DemoApplication::Frame ()
{
  // Update the camera
  cameraHelper.Frame ();

  // Tell the 3D driver we're going to display 3D things.
  if (!g3d->BeginDraw (engine->GetBeginDrawFlags () | CSDRAW_3DGRAPHICS))
    return;

  // Tell the camera to render into the frame buffer.
  view->Draw ();

  // Display of visual debugging informations
  visualDebugger->Display (view);

  // Display the HUD if it is enabled
  if (hudDisplayed)
    hudHelper.DisplayHUD ();
}

bool DemoApplication::OnKeyboard (iEvent &event)
{
  csKeyEventType eventtype = csKeyEventHelper::GetEventType(&event);
  if (eventtype == csKeyEventTypeDown)
  {
    // Check for ESC key
    if (csKeyEventHelper::GetCookedCode (&event) == CSKEY_ESC)
    {
      csRef<iEventQueue> q (csQueryRegistry<iEventQueue> (GetObjectRegistry ()));
      if (q) q->GetEventOutlet()->Broadcast (csevQuit (GetObjectRegistry ()));
      return true;
    }
  }

  return hudHelper.OnKeyboard (event);
}

bool DemoApplication::OnMouseDown (iEvent& event)
{
  return cameraHelper.OnMouseDown (event);
}

bool DemoApplication::OnMouseUp (iEvent& event)
{
  return cameraHelper.OnMouseUp (event);
}

bool DemoApplication::OnMouseMove (iEvent& event)
{
  mouseX = csMouseEventHelper::GetX (&event);
  mouseY = csMouseEventHelper::GetY (&event);

  return cameraHelper.OnMouseMove (event);
}

void DemoApplication::OnExit ()
{
  printer.Invalidate ();
}

bool DemoApplication::OnInitialize (int argc, char* argv[])
{
  // Check for commandline help.
  if (csCommandLineHelper::CheckHelp (GetObjectRegistry ()))
  {
    commandLineHelper.WriteHelp (GetObjectRegistry ());
    return false;
  }

  // Request for the engine plugins
  if (!csInitializer::RequestPlugins (GetObjectRegistry (),
    CS_REQUEST_VFS,
    CS_REQUEST_OPENGL3D,
    CS_REQUEST_ENGINE,
    CS_REQUEST_FONTSERVER,
    CS_REQUEST_IMAGELOADER,
    CS_REQUEST_LEVELLOADER,
    CS_REQUEST_REPORTER,
    CS_REQUEST_REPORTERLISTENER,
    CS_REQUEST_PLUGIN ("crystalspace.utilities.visualdebugger",
		       CS::Debug::iVisualDebugger),
    CS_REQUEST_END))
    return ReportError ("Failed to initialize plugins!");

  csBaseEventHandler::Initialize (GetObjectRegistry ());
  if (!RegisterQueue (GetObjectRegistry (), csevAllEvents (GetObjectRegistry ())))
    return ReportError ("Failed to set up event handler!");

  return true;
}

bool DemoApplication::Application ()
{
  // Open the application and load engine objects
  if (!OpenApplication (GetObjectRegistry ()))
    return ReportError ("Error opening system!");

  g3d = csQueryRegistry<iGraphics3D> (GetObjectRegistry ());
  if (!g3d) return ReportError("Failed to locate 3D renderer!");

  engine = csQueryRegistry<iEngine> (GetObjectRegistry ());
  if (!engine) return ReportError("Failed to locate 3D engine!");

  vc = csQueryRegistry<iVirtualClock> (GetObjectRegistry ());
  if (!vc) return ReportError("Failed to locate Virtual Clock!");

  vfs = csQueryRegistry<iVFS> (GetObjectRegistry ());
  if (!vfs) return ReportError("Failed to locate Virtual File System!");

  kbd = csQueryRegistry<iKeyboardDriver> (GetObjectRegistry ());
  if (!kbd) return ReportError("Failed to locate Keyboard Driver!");

  loader = csQueryRegistry<iLoader> (GetObjectRegistry ());
  if (!loader) return ReportError("Failed to locate Loader!");

  g2d = csQueryRegistry<iGraphics2D> (GetObjectRegistry ());
  if (!g2d) return ReportError("Failed to locate 2D renderer!");

  visualDebugger = csQueryRegistry<CS::Debug::iVisualDebugger> (GetObjectRegistry ());
  if (!visualDebugger) return ReportError("Failed to locate visual debugger!");

  printer.AttachNew (new FramePrinter (GetObjectRegistry ()));

  // Initialize the view
  view = csPtr<iView> (new csView (engine, g3d));
  view->SetRectangle (0, 0, g2d->GetWidth (), g2d->GetHeight ());

  // Initialize the HUD helper
  hudHelper.SetContext (this, g3d, g2d, loader, vc);

  // Workaround for correct initialization of mouseX and mouseY:
  // initialize the mouse position to the center of the window
  g2d->SetMousePosition (g2d->GetWidth () / 2, g2d->GetHeight () / 2);

  return true;
}

bool DemoApplication::CreateRoom ()
{
  // Create the main sector
  room = engine->CreateSector ("room");
  view->GetCamera ()->SetSector (room);

  // Initialize the camera helper
  cameraHelper.SetContext (this, kbd, vc, view->GetCamera ());

  // Creating the background
  // First we make a primitive for our geometry.
  CS::Geometry::DensityTextureMapper bgMapper (0.3f);
  CS::Geometry::TesselatedBox bgBox (csVector3 (-4000.0f), csVector3 (4000.0f));
  bgBox.SetMapper (&bgMapper);
  bgBox.SetFlags (CS::Geometry::Primitives::CS_PRIMBOX_INSIDE);
  
  // Now we make a factory and a mesh at once.
  csRef<iMeshWrapper> background =
    CS::Geometry::GeneralMeshBuilder::CreateFactoryAndMesh (engine, room,
				   "background", "background_factory", &bgBox);
  background->SetRenderPriority (engine->GetRenderPriority ("sky"));

  csRef<iMaterialWrapper> bgMaterial =
    CS::Material::MaterialBuilder::CreateColorMaterial
    (GetObjectRegistry (), "background", csColor (0.398f));
  background->GetMeshObject()->SetMaterialWrapper (bgMaterial);

  // Creating lights
  csRef<iLight> light;
  iLightList* lightList = room->GetLights ();

  // This light is for the background
  light = engine->CreateLight (0, csVector3 (-1, -1, 0), 9000, csColor (1));
  light->SetAttenuationMode (CS_ATTN_NONE);
  lightList->Add (light);

  // Other lights
  light = engine->CreateLight (0, csVector3 (1, 0, 0), 8, csColor4 (1, 1, 1, 1));
  light->SetAttenuationMode (CS_ATTN_REALISTIC);
  lightList->Add (light);

  light = engine->CreateLight (0, csVector3 (-3, 0,  0), 8, csColor (1));
  light->SetAttenuationMode (CS_ATTN_REALISTIC);
  lightList->Add (light);

  light = engine->CreateLight (0, csVector3 (0, 0, -3), 8, csColor (1));
  light->SetAttenuationMode (CS_ATTN_REALISTIC);
  lightList->Add (light);

  light = engine->CreateLight (0, csVector3 (0, 0, 3), 8, csColor (1));
  light->SetAttenuationMode (CS_ATTN_REALISTIC);
  lightList->Add (light);

  light = engine->CreateLight (0, csVector3 (0, -3, 0), 8, csColor (1));
  light->SetAttenuationMode (CS_ATTN_REALISTIC);
  lightList->Add (light);

  engine->Prepare ();
  CS::Lighting::SimpleStaticLighter::ShineLights
    (room, engine, CS::Lighting::SimpleStaticLighter::CS_SHADOW_FULL);

  return true;
}

void DemoApplication::SetHUDDisplayed (bool displayed)
{
  hudDisplayed = displayed;
}

bool DemoApplication::GetHUDDisplayed ()
{
  return hudDisplayed;
}

} // namespace Demo
} // namespace CS
