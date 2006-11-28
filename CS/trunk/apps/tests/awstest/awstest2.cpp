/*
    Copyright (C) 2001 by Christopher Nelson

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

#include "awstest_config.h"

#ifdef TEST_AWS2

#include "csutil/sysfunc.h"
#include "csutil/cscolor.h"
#include "csutil/csevent.h"
#include "csutil/event.h"

#include "cstool/csfxscr.h"
#include "cstool/csview.h"
#include "cstool/initapp.h"
#include "csutil/cmdhelp.h"
#include "ivideo/graph3d.h"
#include "ivideo/fontserv.h"
#include "ivideo/graph2d.h"
#include "ivideo/natwin.h"
#include "ivaria/conout.h"
#include "iengine/engine.h"
#include "iengine/sector.h"
#include "iengine/light.h"
#include "iengine/camera.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "iengine/halo.h"
#include "imesh/thing.h"
#include "imesh/particle.h"
#include "imesh/sprite2d.h"
#include "imesh/sprite3d.h"
#include "imesh/object.h"
#include "imap/reader.h"
#include "igraphic/imageio.h"
#include "imap/loader.h"
#include "iengine/material.h"
#include "ivaria/reporter.h"
#include "iutil/eventq.h"
#include "iutil/virtclk.h"
#include "iutil/plugin.h"
#include "iutil/vfs.h"
#include "csqsqrt.h"

#include "csgeom/csrect.h"
#include "csgeom/csrectrg.h"

#include "awstest2.h"
#include <stdio.h>

#define  QUERY_REG(myPlug, iFace, errMsg) \
  myPlug = csQueryRegistry<iFace> (object_reg); \
  if (!myPlug) \
  { \
    Report (CS_REPORTER_SEVERITY_ERROR, errMsg); \
    return false; \
  }

extern awsTest *System;

awsTest::awsTest()
{
}

awsTest::~awsTest()
{
}

static bool AwsEventHandler (iEvent& ev)
{
  if (!System)
    return false;
  if (ev.Name == System->Process)
  {
    System->SetupFrame ();
    return true;
  }
  else if (ev.Name == System->FinalProcess)
  {
    System->FinishFrame ();
    return true;
  }
  else
  {
    return System->HandleEvent (ev);
  }
}

bool
awsTest::Initialize(int argc, const char* const argv[], const char *iConfigName)
{
  object_reg = csInitializer::CreateEnvironment (argc, argv);
  if (!object_reg) return false;

  if (!csInitializer::SetupConfigManager (object_reg, iConfigName))
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Could not init app!");
    return false;
  }

  if (!csInitializer::RequestPlugins (object_reg, CS_REQUEST_END))
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Could not init app!");
    return false;
  }

  Process = csevProcess (object_reg);
  FinalProcess = csevFinalProcess (object_reg);
  KeyboardDown = csevKeyboardDown (object_reg);

  if (!csInitializer::SetupEventHandler (object_reg, AwsEventHandler))
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Could not setup event handler!");
    return false;
  }

   // Check for commandline help.
  if (csCommandLineHelper::CheckHelp (object_reg))
  {
    csCommandLineHelper::Help (object_reg);
    exit (0);
  }

  // The virtual clock.
  vc = csQueryRegistry<iVirtualClock> (object_reg);
  csRef<iPluginManager> plugin_mgr (
  	csQueryRegistry<iPluginManager> (object_reg));

  // Load the engine plugin.
  Report (CS_REPORTER_SEVERITY_NOTIFY, "Loading engine...");
  engine = CS_LOAD_PLUGIN(plugin_mgr, "crystalspace.engine.3d", iEngine);
  if (!engine)
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Could not load the engine plugin!");
    return false;
  }
  if (!object_reg->Register (engine, "iEngine"))
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Could not register engine!");
    return false;
  }

  QUERY_REG (myG3D, iGraphics3D, "Couldn't load iGraphics3D plugin!");
  QUERY_REG (myG2D, iGraphics2D, "Couldn't load  iGraphics2D plugin!");
  QUERY_REG (myVFS, iVFS, "Couldn't load  iVFS plugin!");
  QUERY_REG (myConsole, iConsoleOutput, "Couldn't load iConsoleOutput plugin!");

  // Load AWS
  Report(CS_REPORTER_SEVERITY_NOTIFY, "Loading AWS 2.0 ...");
  aws = CS_LOAD_PLUGIN(plugin_mgr,
  	"crystalspace.window.alternatemanager2", iAws2);

  if (!aws)
  {
    Report(CS_REPORTER_SEVERITY_ERROR, "Could not load the AWS plugin!");
    return false;
  }


  loader = CS_LOAD_PLUGIN(plugin_mgr, "crystalspace.level.loader", iLoader);
  if (!loader)
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "No iLoader plugin!");
    return false;
  }
  if (!object_reg->Register (loader, "iLoader"))
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Could not register loader!");
    return false;
  }

  // Open the main system. This will open all the previously loaded plug-ins.
  iNativeWindow* nw = myG2D->GetNativeWindow ();
  if (nw) nw->SetTitle ("AWS 2.0 Test Harness");

  if (!csInitializer::OpenApplication (object_reg))
  {
    Report(CS_REPORTER_SEVERITY_ERROR, "Error opening system!");
    return false;
  }

  font = myG2D->GetFontServer()->LoadFont (CSFONT_LARGE);

  // Initialize the console
  if (myConsole != 0)
    // Don't let messages before this one appear
    myConsole->Clear ();

  // Some commercials...
  Report (CS_REPORTER_SEVERITY_NOTIFY,
    "The Alternate Window System 2.0 Test Harness.");

  // First disable the lighting cache. Our app is simple enough
  // not to need this.
  engine->SetLightingCacheMode (0);

  // Create our world.
  Report (CS_REPORTER_SEVERITY_NOTIFY, "Creating world!...");

  if (!loader->LoadTexture ("stone", "/lib/stdtex/parket.jpg"))
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Error loading 'parket' texture!");
    exit (1);
  }
  iMaterialWrapper* tm = engine->GetMaterialList ()->FindByName ("stone");

  room = engine->CreateSector ("room");
  csRef<iMeshWrapper> walls (engine->CreateSectorWallsMesh (room, "walls"));
  csRef<iThingFactoryState> walls_state = 
    scfQueryInterface<iThingFactoryState> (walls->GetMeshObject ()->GetFactory());
  walls_state->AddInsideBox (csVector3 (-5, 0, -5), csVector3 (5, 20, 5));
  walls_state->SetPolygonMaterial (CS_POLYRANGE_LAST, tm);
  walls_state->SetPolygonTextureMapping (CS_POLYRANGE_LAST, 3);

  csRef<iLight> light;
  iLightList* ll = room->GetLights ();
  light = engine->CreateLight (0, csVector3 (-10, 10.5, -10), 15,
  	csColor (1, 0, 0));
  ll->Add (light);

  light = engine->CreateLight (0, csVector3 (10, 10.5,  10), 15,
  	csColor (0, 0, 1));
  ll->Add (light);

  light = engine->CreateLight (0, csVector3 (-10, 10.5, 10), 15,
  	csColor (0, 1, 0));
  ll->Add (light);

  light = engine->CreateLight (0, csVector3 (10, 10.5, -10), 15,
  	csColor (1, 1, 1));
  ll->Add (light);

  engine->Prepare ();

  Report(CS_REPORTER_SEVERITY_NOTIFY, "--------------------------------------");

  view = csPtr<iView> (new csView (engine, myG3D));
  view->GetCamera ()->SetSector (room);
  view->GetCamera ()->GetTransform ().SetOrigin (csVector3 (0, 5, -3));
  view->SetRectangle (0, 0, myG2D->GetWidth (), myG2D->GetHeight ());

  wview = csPtr<iView> (new csView (engine, myG3D));
  wview->GetCamera ()->SetSector (room);
  wview->GetCamera ()->GetTransform ().SetOrigin (csVector3 (0, 5, -3));

  col_red = myG2D->FindRGB (255, 0, 0);
  col_blue = myG2D->FindRGB (0, 0, 255);
  col_white = myG2D->FindRGB (255, 255, 255);
  col_gray = myG2D->FindRGB (50, 50, 50);
  col_black = myG2D->FindRGB (0, 0, 0);
  col_yellow = myG2D->FindRGB (255, 255, 0);
  col_cyan = myG2D->FindRGB (0, 255, 255);
  col_green = myG2D->FindRGB (0, 255, 0);

 // Setup AWS specific stuff here.
 //aws->Initialize(object_reg);
 aws->SetDrawTarget(myG2D, myG3D);

 // Load a definition file
 if (aws->Load("/aws/awstest.js.def")==false)
   Report(CS_REPORTER_SEVERITY_ERROR, "Unable to load the definition file '/aws/awstest.js.def'");  

  Report(CS_REPORTER_SEVERITY_NOTIFY, "Init done.");

  return true;
}

void
awsTest::SetupFrame()
{
  static int counter=0;

  iCamera* c = view->GetCamera();
  iCamera* c2 = wview->GetCamera();

  counter++;

  // First get elapsed time from the system driver.
  csTicks elapsed_time, current_time;
  elapsed_time = vc->GetElapsedTicks ();
  current_time = vc->GetCurrentTicks ();

  // Now rotate the camera according to keyboard state
  float speed = (elapsed_time / 1000.0f) * (0.03f * 2);

  c->GetTransform ().RotateThis (CS_VEC_ROT_RIGHT, speed);
  c2->GetTransform ().RotateThis (CS_VEC_ROT_LEFT, speed);

  // Tell 3D driver we're going to display 3D things.
  if (!myG3D->BeginDraw (
      engine->GetBeginDrawFlags () | CSDRAW_3DGRAPHICS))
      return;

  // Tell the camera to render into the frame buffer.
  view->Draw ();
  wview->Draw();

  // Start drawing 2D graphics.
  if (!myG3D->BeginDraw (CSDRAW_2DGRAPHICS)) return;

  message.Format ("awsTest2(%d)", counter);
  myG2D->Write(font, 5, 5, col_green, -1, message);

  aws->Redraw();
}

void
awsTest::FinishFrame ()
{
  myG3D->FinishDraw ();
  myG3D->Print (0);
}

bool
awsTest::HandleEvent (iEvent &Event)
{
   if ((Event.Name == KeyboardDown) && 
      (csKeyEventHelper::GetCookedCode (&Event) == CSKEY_ESC))
  {
    csRef<iEventQueue> q (csQueryRegistry<iEventQueue> (object_reg));
    if (q)
      q->GetEventOutlet()->Broadcast (csevQuit (object_reg));
    return true;
  }

  if (aws) return aws->HandleEvent(Event);
  return false;
}

void
awsTest::Report (int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csRef<iReporter> rep (csQueryRegistry<iReporter> (object_reg));
  if (rep)
    rep->ReportV (severity, "crystalspace.application.awstest", msg, arg);
  else
  {
    csPrintfV (msg, arg);
    csPrintf ("\n");
  }
  va_end (arg);
}

#endif // end only compile if testing aws2.
