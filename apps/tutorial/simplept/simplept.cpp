/*
    Copyright (C) 2001 by Jorrit Tyberghein

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
#include "cssys/sysfunc.h"
#include "csutil/cscolor.h"
#include "cstool/csview.h"
#include "cstool/initapp.h"
#include "csutil/cmdhelp.h"
#include "simplept.h"
#include "iutil/eventq.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "iutil/event.h"
#include "iutil/objreg.h"
#include "iutil/csinput.h"
#include "iutil/virtclk.h"
#include "iengine/sector.h"
#include "iengine/engine.h"
#include "iengine/camera.h"
#include "iengine/light.h"
#include "iengine/statlght.h"
#include "iengine/texture.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "iengine/material.h"
#include "imesh/thing/polygon.h"
#include "imesh/thing/thing.h"
#include "imesh/object.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "ivideo/txtmgr.h"
#include "ivideo/texture.h"
#include "ivideo/material.h"
#include "ivideo/fontserv.h"
#include "igraphic/imageio.h"
#include "imap/parser.h"
#include "ivaria/reporter.h"
#include "ivaria/stdrep.h"
#include "iutil/vfs.h"

CS_IMPLEMENT_APPLICATION

//-----------------------------------------------------------------------------

void CreatePolygon (iThingState *th, int v1, int v2, int v3, int v4,
  iMaterialWrapper *mat)
{
  iPolygon3D* p = th->CreatePolygon ();
  p->SetMaterial (mat);
  p->CreateVertex (v1);
  p->CreateVertex (v2);
  p->CreateVertex (v3);
  p->CreateVertex (v4);
  p->SetTextureSpace (p->GetVertex (0), p->GetVertex (1), -6);
}

//-----------------------------------------------------------------------------

// The global system driver
Simple *simple;

Simple::Simple ()
{
  view = NULL;
  engine = NULL;
  loader = NULL;
  g3d = NULL;
  kbd = NULL;
  vc = NULL;
}

Simple::~Simple ()
{
  // @@@ this is a bug in the engine that turns up when using the software
  // implementation of OpenGL procedural textures: The texture is created
  // by the software renderer, so it must also be uncached by the software
  // renderer. This is not correctly done by the engine.

  // To fix the problem, we do the following: Set the engine context to
  // the real graphics renderer and uncache the wall texture used in our
  // small room. Then set the engine context to the procedural texture and
  // close the system. This will shut down the engine, uncaching all
  // textures used in the world that is displayed on the procedural texture.

  // Note that this fixes one error, but only to dig up the next one
  // which is now related to lightmap caching. I give up! My knowledge
  // about the texture cache is not enough to fix that.       -- mgeisse

  if (engine && g3d)
  {
    engine->SetContext(g3d);
    engine->GetSectors()->Remove(engine->GetSectors()->FindByName("room"));

    engine->SetContext(
      ProcTexture->GetTextureHandle()->GetProcTextureInterface());
  }

  if (view) view->DecRef ();
  if (engine) engine->DecRef ();
  if (loader) loader->DecRef();
  if (g3d) g3d->DecRef ();
  if (kbd) kbd->DecRef ();
  if (vc) vc->DecRef ();
  
  delete ProcTexture;
  
  csInitializer::DestroyApplication (object_reg);
}

static bool SimpleEventHandler (iEvent& ev)
{
  if (ev.Type == csevBroadcast && ev.Command.Code == cscmdProcess)
  {
    simple->SetupFrame ();
    return true;
  }
  else if (ev.Type == csevBroadcast && ev.Command.Code == cscmdFinalProcess)
  {
    simple->FinishFrame ();
    return true;
  }
  else
  {
    return simple ? simple->HandleEvent (ev) : false;
  }
}

bool Simple::Initialize (int argc, const char* const argv[])
{
  object_reg = csInitializer::CreateEnvironment (argc, argv);
  if (!object_reg) return false;

  if (!csInitializer::RequestPlugins (object_reg,
  	CS_REQUEST_VFS,
	CS_REQUEST_SOFTWARE3D,
	CS_REQUEST_ENGINE,
	CS_REQUEST_FONTSERVER,
	CS_REQUEST_IMAGELOADER,
	CS_REQUEST_LEVELLOADER,
	CS_REQUEST_REPORTER,
	CS_REQUEST_REPORTERLISTENER,
	CS_REQUEST_END))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simplept",
	"Can't initialize plugins!");
    return false;
  }

  if (!csInitializer::SetupEventHandler (object_reg, SimpleEventHandler))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simplept",
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
  vc = CS_QUERY_REGISTRY (object_reg, iVirtualClock);

  // Find the pointer to engine plugin
  engine = CS_QUERY_REGISTRY (object_reg, iEngine);
  if (!engine)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simplept",
    	"No iEngine plugin!");
    return false;
  }

  loader = CS_QUERY_REGISTRY (object_reg, iLoader);
  if (!loader)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simplept",
    	"No iLoader plugin!");
    return false;
  }

  g3d = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
  if (!g3d)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simplept",
    	"No iGraphics3D pluginn");
    return false;
  }

  kbd = CS_QUERY_REGISTRY (object_reg, iKeyboardDriver);
  if (!kbd)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simplept",
    	"No iKeyboardDriver pluginn");
    return false;
  }

  // Open the main system. This will open all the previously loaded plug-ins.
  if (!csInitializer::OpenApplication (object_reg))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simplept",
    	"Error opening system!");
    return false;
  }

  // Setup the texture manager
  iTextureManager* txtmgr = g3d->GetTextureManager ();
  txtmgr->SetVerbose (true);

  // Initialize the texture manager
  txtmgr->ResetPalette ();

  csReport (object_reg, CS_REPORTER_SEVERITY_NOTIFY,
    	"crystalspace.application.simplept",
  	"Simple Procedural Texture Crystal Space Application version 0.1.");

  // First disable the lighting cache. Our app is simple enough
  // not to need this.
  engine->SetLightingCacheMode (0);

  // Create our world.
  csReport (object_reg, CS_REPORTER_SEVERITY_NOTIFY,
    	"crystalspace.application.simplept",
  	"Creating world!...");

  if (!loader->LoadTexture ("stone", "/lib/std/stone4.gif"))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simplept",
    	"Error loading 'stone4' texture!");
    return false;
  }
  iMaterialWrapper* tm = engine->GetMaterialList ()->FindByName ("stone");
  // Create the procedural texture and a material for it
  ProcTexture = new csEngineProcTex ();
  // Find the pointer to VFS.
  csRef<iVFS> VFS (CS_QUERY_REGISTRY (object_reg, iVFS));
  if (!VFS)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simplept",
    	"No iVFS plugin!");
    return false;
  }

  if (!ProcTexture->Initialize (g3d, engine, VFS, loader))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simplept",
    	"Could not initialize procedural texture!");
    return false;
  }
  iTextureWrapper *tw = engine->GetTextureList ()->NewTexture
      (ProcTexture->GetTextureHandle());
  iMaterialWrapper *ProcMat = engine->CreateMaterial ("procmat", tw);
  room = engine->CreateSector ("proctex-room");
  csRef<iMeshWrapper> walls (engine->CreateSectorWallsMesh (room, "walls"));
  csRef<iThingState> walls_state (SCF_QUERY_INTERFACE (walls->GetMeshObject (),
  	iThingState));

  walls_state->CreateVertex (csVector3 (-8, -8, -5));
  walls_state->CreateVertex (csVector3 (-3, -3, +8));
  walls_state->CreateVertex (csVector3 (+3, -3, +8));
  walls_state->CreateVertex (csVector3 (+8, -8, -5));
  walls_state->CreateVertex (csVector3 (-8, +8, -5));
  walls_state->CreateVertex (csVector3 (-3, +3, +8));
  walls_state->CreateVertex (csVector3 (+3, +3, +8));
  walls_state->CreateVertex (csVector3 (+8, +8, -5));

  CreatePolygon (walls_state, 0, 1, 2, 3, tm);
  CreatePolygon (walls_state, 1, 0, 4, 5, tm);
  CreatePolygon (walls_state, 2, 1, 5, 6, ProcMat);
  CreatePolygon (walls_state, 3, 2, 6, 7, tm);
  CreatePolygon (walls_state, 0, 3, 7, 4, tm);
  CreatePolygon (walls_state, 7, 6, 5, 4, tm);

  csRef<iStatLight> light;
  light = engine->CreateLight (NULL, csVector3 (0, 0, 0), 20,
  	csColor (1, 1, 1), false);
  room->GetLights ()->Add (light->QueryLight ());

  engine->Prepare ();
  csReport (object_reg, CS_REPORTER_SEVERITY_NOTIFY,
    	"crystalspace.application.simplept",
  	"Created.");

  view = new csView (engine, g3d);
  view->GetCamera ()->SetSector (room);
  view->GetCamera ()->GetTransform ().SetOrigin (csVector3 (0, 0, 0));
  iGraphics2D* g2d = g3d->GetDriver2D ();
  view->SetRectangle (0, 0, g2d->GetWidth (), g2d->GetHeight ());

  txtmgr->SetPalette ();
  return true;
}

bool Simple::HandleEvent (iEvent& Event)
{
  if (Event.Type == csevKeyDown && Event.Key.Code == CSKEY_ESC)
  {
    csRef<iEventQueue> q (CS_QUERY_REGISTRY (object_reg, iEventQueue));
    if (q)
      q->GetEventOutlet()->Broadcast (cscmdQuit);
    return true;
  }

  return false;
}

void Simple::SetupFrame ()
{
  // First get elapsed time from the system driver.
  csTicks elapsed_time, current_time;
  elapsed_time = vc->GetElapsedTicks ();
  current_time = vc->GetCurrentTicks ();

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

  // Update our procedural texture
  ProcTexture->Update (current_time);

  // Tell 3D driver we're going to display 3D things.
  if (!g3d->BeginDraw (
      engine->GetBeginDrawFlags () | CSDRAW_3DGRAPHICS))
      return;

  // Tell the camera to render into the frame buffer.
  view->Draw ();
}

void Simple::FinishFrame ()
{
  g3d->FinishDraw ();
  g3d->Print (NULL);
}

void Simple::Start ()
{
  csDefaultRunLoop(object_reg);
}

/*---------------------------------------------------------------------*
 * Main function
 *---------------------------------------------------------------------*/
int main (int argc, char* argv[])
{
  srand (time (NULL));

  // Create our main class.
  simple = new Simple ();

  // Initialize the main system. This will load all needed plug-ins
  // (3D, 2D, network, sound, ...) and initialize them.
  if (simple->Initialize (argc, argv))
	  simple->Start ();

  delete simple;

  return 0;
}
