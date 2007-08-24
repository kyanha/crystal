/*
    Copyright (C) 2003 by Boyan Hristov

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
#include "csutil/sysfunc.h"
#include "iutil/vfs.h"
#include "csutil/cscolor.h"
#include "cstool/csview.h"
#include "cstool/genmeshbuilder.h"
#include "cstool/initapp.h"
#include "lightningtest.h"
#include "iutil/eventq.h"
#include "iutil/event.h"
#include "iutil/objreg.h"
#include "iutil/csinput.h"
#include "iutil/virtclk.h"
#include "iengine/sector.h"
#include "iengine/engine.h"
#include "iengine/camera.h"
#include "iengine/light.h"
#include "iengine/texture.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "iengine/material.h"
#include "imesh/object.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "ivideo/txtmgr.h"
#include "ivideo/texture.h"
#include "ivideo/material.h"
#include "ivideo/fontserv.h"
#include "igraphic/imageio.h"
#include "imap/loader.h"
#include "ivaria/reporter.h"
#include "ivaria/stdrep.h"
#include "csutil/cmdhelp.h"
#include "csutil/event.h"

#include "imesh/lghtng.h"
#include "iutil/plugin.h"



CS_IMPLEMENT_APPLICATION

//-----------------------------------------------------------------------------

// The global pointer to simple
Simple *simple;

Simple::Simple (iObjectRegistry* object_reg)
{
  Simple::object_reg = object_reg;
}

Simple::~Simple ()
{
}

void Simple::SetupFrame ()
{
  // First get elapsed time from the virtual clock.
  csTicks elapsed_time = vc->GetElapsedTicks ();

  // Now rotate the camera according to keyboard state
  float speed = (elapsed_time / 1000.0) * (0.03 * 20);

  iCamera* c = view->GetCamera();
  if (kbd->GetKeyState (CSKEY_RIGHT))
    c->GetTransform ().RotateThis (CS_VEC_ROT_RIGHT, speed);
  if (kbd->GetKeyState (CSKEY_LEFT))
    c->GetTransform ().RotateThis (CS_VEC_ROT_LEFT, speed);
  if (kbd->GetKeyState (CSKEY_PGUP))
    c->GetTransform ().RotateThis (CS_VEC_TILT_UP, speed);
  if (kbd->GetKeyState (CSKEY_PGDN))
    c->GetTransform ().RotateThis (CS_VEC_TILT_DOWN, speed);
  if (kbd->GetKeyState (CSKEY_UP))
    c->Move (CS_VEC_FORWARD * 4 * speed);
  if (kbd->GetKeyState (CSKEY_DOWN))
    c->Move (CS_VEC_BACKWARD * 4 * speed);

  // Tell 3D driver we're going to display 3D things.
  if (!g3d->BeginDraw (engine->GetBeginDrawFlags () | CSDRAW_3DGRAPHICS))
    return;

  // Tell the camera to render into the frame buffer.
  view->Draw ();
}

void Simple::FinishFrame ()
{
  g3d->FinishDraw ();
  g3d->Print (0);
}

bool Simple::HandleEvent (iEvent& ev)
{
  if (ev.Name == Process)
  {
    simple->SetupFrame ();
    return true;
  }
  else if (ev.Name == FinalProcess)
  {
    simple->FinishFrame ();
    return true;
  }
  else if ((ev.Name == KeyboardDown) &&
	   (csKeyEventHelper::GetCookedCode (&ev) == CSKEY_ESC))
  {
    csRef<iEventQueue> q (csQueryRegistry<iEventQueue> (object_reg));
    if (q) q->GetEventOutlet()->Broadcast (csevQuit (object_reg));
    return true;
  }

  return false;
}

bool Simple::SimpleEventHandler (iEvent& ev)
{
  if (simple)
    return simple->HandleEvent (ev);
  else
    return false;
}

bool Simple::Initialize ()
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
    CS_REQUEST_PLUGIN("crystalspace.mesh.object.lightning", iLightningState),
	CS_REQUEST_END))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.lightningtest",
	"Can't initialize plugins!");
    return false;
  }

  Process = csevProcess (object_reg);
  FinalProcess = csevFinalProcess (object_reg);
  KeyboardDown = csevKeyboardDown (object_reg);

  if (!csInitializer::SetupEventHandler (object_reg, SimpleEventHandler))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.lightningtest",
	"Can't initialize event handler!");
    return false;
  }

  // Check for commandline help.
  if (csCommandLineHelper::CheckHelp (object_reg))
  {
    csCommandLineHelper::Help (object_reg);
    return false;
  }

  // The virtual clock.
  vc = csQueryRegistry<iVirtualClock> (object_reg);
  if (vc == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.lightningtest",
	"Can't find the virtual clock!");
    return false;
  }

  // Find the pointer to engine plugin
  engine = csQueryRegistry<iEngine> (object_reg);
  if (engine == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.lightningtest",
	"No iEngine plugin!");
    return false;
  }

  loader = csQueryRegistry<iLoader> (object_reg);
  if (loader == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.lightningtest",
    	"No iLoader plugin!");
    return false;
  }

  g3d = csQueryRegistry<iGraphics3D> (object_reg);
  if (g3d == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.lightningtest",
    	"No iGraphics3D plugin!");
    return false;
  }

  kbd = csQueryRegistry<iKeyboardDriver> (object_reg);
  if (kbd == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.lightningtest",
    	"No iKeyboardDriver plugin!");
    return false;
  }

  // Open the main system. This will open all the previously loaded plug-ins.
  if (!csInitializer::OpenApplication (object_reg))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.lightningtest",
    	"Error opening system!");
    return false;
  }

  PluginManager = csQueryRegistry<iPluginManager> (object_reg);
  if (!PluginManager)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.lightningtest",
    	"No iPluginManager plugin!");
    return false;
  }

  // First disable the lighting cache. Our app is simple enough
  // not to need this.
  engine->SetLightingCacheMode (0);

  if (!loader->LoadTexture ("stone", "/lib/std/stone4.gif"))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.lightningtest",
    	"Error loading 'stone4' texture!");
    return false;
  }

  if (!loader->LoadTexture ("energy", "/lib/std/energy.jpg"))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.lightningtest",
    	"Error loading 'stone4' texture!");
    return false;
  }

  iMaterialWrapper* tm = engine->GetMaterialList ()->FindByName ("stone");

  room = engine->CreateSector ("room");

  // First we make a primitive for our geometry.
  using namespace CS::Geometry;
  DensityTextureMapper mapper (0.3f);
  TesselatedBox box (csVector3 (-5, 0, -5), csVector3 (5, 20, 5));
  box.SetLevel (3);
  box.SetMapper (&mapper);
  box.SetFlags (Primitives::CS_PRIMBOX_INSIDE);

  // Now we make a factory and a mesh at once.
  csRef<iMeshWrapper> walls = GeneralMeshBuilder::CreateFactoryAndMesh (
      engine, room, "walls", "walls_factory", &box);

  csRef<iGeneralMeshState> mesh_state = scfQueryInterface<
    iGeneralMeshState> (walls->GetMeshObject ());
  mesh_state->SetShadowReceiving (true);
  walls->GetMeshObject ()->SetMaterialWrapper (tm);

  csRef<iLight> light;
  iLightList* ll = room->GetLights ();

  light = engine->CreateLight (0, csVector3 (-3, 5, 0), 10,
  	csColor (1, 0, 0));
  ll->Add (light);

  light = engine->CreateLight (0, csVector3 (3, 5,  0), 10,
  	csColor (0, 0, 1));
  ll->Add (light);

  light = engine->CreateLight (0, csVector3 (0, 5, -3), 10,
  	csColor (0, 1, 0));
  ll->Add (light);

 
  csRef<iMeshObjectType> type (CS_LOAD_PLUGIN(PluginManager,
  	"crystalspace.mesh.object.lightning", iMeshObjectType));  

  /// Lightning 1
  csRef<iMeshObjectFactory> LightningObjectFactory1 = type->NewFactory();
  csRef<iLightningFactoryState> LightningFactoryState1 = scfQueryInterface<iLightningFactoryState> (LightningObjectFactory1);

  LightningObjectFactory1->SetMaterialWrapper(engine->GetMaterialList()->FindByName("energy"));
  LightningObjectFactory1->SetMixMode(CS_FX_ADD);
  LightningFactoryState1->SetOrigin(csVector3(0, 0, 0));
  LightningFactoryState1->SetPointCount(20);
  LightningFactoryState1->SetLength(5);
  LightningFactoryState1->SetVibration(0.02f);
  LightningFactoryState1->SetWildness(0.03f);
  LightningFactoryState1->SetUpdateInterval(30);
  LightningFactoryState1->SetDirectional(csVector3(0, 1, 0));  

  csRef<iMeshObject> mesh1 = LightningObjectFactory1->NewInstance();    
  csRef<iMeshWrapper> mw1 = engine->CreateMeshWrapper(mesh1, "lightning1", room, csVector3(-2, 3, 4));
  mw1->SetRenderPriority (engine->GetRenderPriority ("alpha"));

  /// Lightning 2
  csRef<iMeshObjectFactory> LightningObjectFactory2 = type->NewFactory();
  csRef<iLightningFactoryState> LightningFactoryState2 = scfQueryInterface<iLightningFactoryState> (LightningObjectFactory2);

  LightningObjectFactory2->SetMaterialWrapper(engine->GetMaterialList()->FindByName("energy"));
  LightningObjectFactory2->SetMixMode(CS_FX_ADD);
  LightningFactoryState2->SetOrigin(csVector3(0, 0, 0));
  LightningFactoryState2->SetPointCount(60);
  LightningFactoryState2->SetLength(5);
  LightningFactoryState2->SetVibration(0.03f);
  LightningFactoryState2->SetWildness(0.06f);
  LightningFactoryState2->SetUpdateInterval(50);
  LightningFactoryState2->SetDirectional(csVector3(0, 1, 0));  
                       
  csRef<iMeshObject> mesh2 = LightningObjectFactory2->NewInstance();    
  csRef<iMeshWrapper> mw2 = engine->CreateMeshWrapper(mesh2, "lightning2", room, csVector3(0, 3, 4));
  mw2->SetRenderPriority (engine->GetRenderPriority ("alpha"));

  ///Lightning 3
  csRef<iMeshObjectFactory> LightningObjectFactory3 = type->NewFactory();
  csRef<iLightningFactoryState> LightningFactoryState3 = scfQueryInterface<iLightningFactoryState> (LightningObjectFactory3);

  LightningObjectFactory3->SetMaterialWrapper(engine->GetMaterialList()->FindByName("energy"));
  LightningObjectFactory3->SetMixMode(CS_FX_ADD);
  LightningFactoryState3->SetOrigin(csVector3(0, 0, 0));
  LightningFactoryState3->SetPointCount(30);
  LightningFactoryState3->SetLength(5);
  LightningFactoryState3->SetVibration(0.02f);
  LightningFactoryState3->SetWildness(0.09f);
  LightningFactoryState3->SetUpdateInterval(60);
  LightningFactoryState3->SetDirectional(csVector3(0, 1, 0));  
  LightningFactoryState3->SetBandWidth (0.6f);

  csRef<iMeshObject> mesh3 = LightningObjectFactory3->NewInstance();    
  csRef<iMeshWrapper> mw3 = engine->CreateMeshWrapper(mesh3, "lightning3", room, csVector3(2, 3, 4));
  mw3->SetRenderPriority (engine->GetRenderPriority ("alpha"));

  engine->Prepare ();

  view = csPtr<iView> (new csView (engine, g3d));
  view->GetCamera ()->SetSector (room);
  view->GetCamera ()->GetTransform ().SetOrigin (csVector3 (0, 5, -3));
  iGraphics2D* g2d = g3d->GetDriver2D ();
  view->SetRectangle (0, 0, g2d->GetWidth (), g2d->GetHeight ());

  return true;
}

void Simple::Start ()
{
  csDefaultRunLoop (object_reg);
}

/*---------------------------------------------------------------------*
 * Main function
 *---------------------------------------------------------------------*/
int main (int argc, char* argv[])
{
  iObjectRegistry* object_reg = csInitializer::CreateEnvironment (argc, argv);

  simple = new Simple (object_reg);
  if (simple->Initialize ())
    simple->Start ();
  delete simple;
  simple = 0;

  csInitializer::DestroyApplication (object_reg);
  return 0;
}

