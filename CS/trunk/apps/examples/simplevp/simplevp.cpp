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

/*
	SOME SPECIAL NOTES!
	This file uses the new effectsystem, and specially vertexprograms for opengl.
	To be able to use it the following requierments must be fullfilled
	* OpenGL renderer with #if 1 changed to #if 0 in DrawTriangleMesh
	* NVidia card and drivers which support vertexprograms
*/

#include "cssysdef.h"
#include "csutil/sysfunc.h"
#include "iutil/vfs.h"
#include "csutil/cscolor.h"
#include "cstool/csview.h"
#include "cstool/initapp.h"
#include "simplevp.h"
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
#include "imesh/thing/polygon.h"
#include "imesh/thing/thing.h"
#include "imesh/sprite3d.h"
#include "imesh/object.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "ivideo/txtmgr.h"
#include "ivideo/texture.h"
#include "ivideo/material.h"
#include "ivideo/fontserv.h"
#include "ivideo/effects/efserver.h"
#include "ivideo/effects/efclient.h"
#include "ivideo/effects/efdef.h"
#include "ivideo/effects/eflayer.h"
#include "ivideo/effects/eftech.h"
#include "ivideo/effects/efpass.h"
#include "igraphic/imageio.h"
#include "imap/loader.h"
#include "ivaria/reporter.h"
#include "ivaria/stdrep.h"
#include "csutil/cmdhelp.h"
#include "iutil/object.h"
#include "csutil/event.h"

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
  else if ((ev.Type == csevKeyboard) && 
    (csKeyEventHelper::GetEventType (&ev) == csKeyEventTypeDown) &&
    (csKeyEventHelper::GetCookedCode (&ev) == CSKEY_ESC))
  {
    csRef<iEventQueue> q (CS_QUERY_REGISTRY (object_reg, iEventQueue));
    if (q) q->GetEventOutlet()->Broadcast (cscmdQuit);
    return true;
  }

  return false;
}

bool Simple::SimpleEventHandler (iEvent& ev)
{
  return simple->HandleEvent (ev);
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
	CS_REQUEST_END))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simple2",
	"Can't initialize plugins!");
    return false;
  }

  if (!csInitializer::SetupEventHandler (object_reg, SimpleEventHandler))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simple2",
	"Can't initialize event handler!");
    return false;
  }

  // Check for commandline help.
  if (csCommandLineHelper::CheckHelp (object_reg))
  {
    csCommandLineHelper::Help (object_reg);
    return false;
  }

  //Mount effectsdir'
  csRef<iVFS> vfs = CS_QUERY_REGISTRY(object_reg, iVFS);
  if(!vfs.IsValid())
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
      "crystalspace.application.simplevp",
      "Couldn't get a iVFS to mount effectdata!");
	return false;
  }
    
  vfs->Mount("/effect/", "$@data$/effectsys$/");

  // The virtual clock.
  vc = CS_QUERY_REGISTRY (object_reg, iVirtualClock);
  if (vc == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simple2",
	"Can't find the virtual clock!");
    return false;
  }

  // Find the pointer to engine plugin
  engine = CS_QUERY_REGISTRY (object_reg, iEngine);
  if (engine == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simple2",
	"No iEngine plugin!");
    return false;
  }

  loader = CS_QUERY_REGISTRY (object_reg, iLoader);
  if (loader == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simple2",
    	"No iLoader plugin!");
    return false;
  }

  g3d = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
  if (g3d == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simple2",
    	"No iGraphics3D plugin!");
    return false;
  }

  kbd = CS_QUERY_REGISTRY (object_reg, iKeyboardDriver);
  if (kbd == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simple2",
    	"No iKeyboardDriver plugin!");
    return false;
  }

  // Open the main system. This will open all the previously loaded plug-ins.
  if (!csInitializer::OpenApplication (object_reg))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simple2",
    	"Error opening system!");
    return false;
  }

  efserver = CS_QUERY_REGISTRY(object_reg, iEffectServer);
  if(efserver == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
			"crystalspace.application.simple2",
			"No effectsystem to use!");
    return false;
  }

  // First disable the lighting cache. Our app is simple enough
  // not to need this.
  engine->SetLightingCacheMode (0);

  if (!loader->LoadTexture ("stone", "/lib/std/stone4.gif"))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simple2",
    	"Error loading 'stone4' texture!");
    return false;
  }
  iMaterialWrapper* tm = engine->GetMaterialList ()->FindByName ("stone");

  room = engine->CreateSector ("room");
  csRef<iMeshWrapper> walls (engine->CreateSectorWallsMesh (room, "walls"));
  csRef<iThingState> ws =
  	SCF_QUERY_INTERFACE (walls->GetMeshObject (), iThingState);
  csRef<iThingFactoryState> walls_state = ws->GetFactory ();
  walls_state->AddInsideBox (csVector3 (-5, 0, -5), csVector3 (5, 20, 5));
  walls_state->SetPolygonMaterial (CS_POLYRANGE_LAST, tm);
  walls_state->SetPolygonTextureMapping (CS_POLYRANGE_LAST, 3);

  csRef<iLight> light;
  iLightList* ll = room->GetLights ();

  light = engine->CreateLight (0, csVector3 (-3, 5, 0), 10,
  	csColor (1, 1, 1));
  ll->Add (light);

  engine->Prepare ();

  view = csPtr<iView> (new csView (engine, g3d));
  view->GetCamera ()->SetSector (room);
  view->GetCamera ()->GetTransform ().SetOrigin (csVector3 (0, 5, -3));
  iGraphics2D* g2d = g3d->GetDriver2D ();
  view->SetRectangle (0, 0, g2d->GetWidth (), g2d->GetHeight ());

  iTextureManager* txtmgr = g3d->GetTextureManager ();

  iTextureWrapper* txt = loader->LoadTexture ("ms",
  	"/lib/std/stone4.gif", CS_TEXTURE_3D, txtmgr, true);
  if (txt == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simple2",
    	"Error loading texture!");
    return false;
  }


  //effectserver test
  loader->LoadEffectFile("/effect/ef1.xml");
  
  iMaterialWrapper *MatWrapper=engine->GetMaterialList()->FindByName("ms");
  iMaterial *Mat=MatWrapper->GetMaterial();
  /*
  effect=efserver->CreateEffect();
  technique=effect->CreateTechnique();
  pass1=technique->CreatePass();

  //Create and set our variables
  int vn;
  vn = effect->GetVariableID(efserver->RequestString("color"));
  effect->SetVariableVector4(vn, csEffectVector4(1.0f,0.0f,0.0f) );

  vn = effect->GetVariableID(efserver->RequestString("multiple"));
  effect->SetVariableFloat(vn, 0.4f  );

  //load our vertex-shader
  //	csRef<iVFS> vfs = CS_QUERY_REGISTRY(object_reg, iVFS);
  csRef<iDataBuffer> id = vfs->ReadFile("/effect/ms.nvv");

  if( !id.IsValid())
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR, "crystalspace.application", "Couldn't load vertex-program");
    return false;
  }

  char *vdata;
  vdata = (char*)(id->GetData());

  pass1->SetStateString( efserver->RequestString("nvvertex program gl"), efserver->RequestString(vdata) );
  pass1->SetStateString( efserver->RequestString("vertex program constant 4"), efserver->RequestString("color") );
  pass1->SetStateString( efserver->RequestString("vertex program constant 5"), efserver->RequestString("multiple") );

  layer1=pass1->CreateLayer();

  layer1->SetStateFloat( efserver->RequestString("texture source"), 1);
  layer1->SetStateFloat( efserver->RequestString("texture coordinate source"), efserver->RequestString("mesh"));
  //layer1->SetStateString( efserver->RequestString("color source 1") , efserver->RequestString("texture color"));
  layer1->SetStateString( efserver->RequestString("color source 1") , efserver->RequestString("vertex color"));
  layer1->SetStateString( efserver->RequestString("color operation") , efserver->RequestString("use source 1"));
*/
  effect = efserver->GetEffect("rainbow");
  Mat->SetEffect (effect);
//  efserver->Validate(effect);

	
  MatWrapper->Register (txtmgr);
  MatWrapper->GetMaterialHandle()->Prepare ();



  // Load a sprite template from disk.
  csRef<iMeshFactoryWrapper> imeshfact (
  	loader->LoadMeshObjectFactory ("/effect/sprite1"));
  if (imeshfact == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.simple2",
    	"Error loading mesh object factory!");
    return false;
  }

  // Create the sprite and add it to the engine.
  csRef<iMeshWrapper> sprite (engine->CreateMeshWrapper (
  	imeshfact, "MySprite", room,
	csVector3 (-3, 5, 3)));
  csMatrix3 m; m.Identity (); m *= 5.;
  sprite->GetMovable ()->SetTransform (m);
  sprite->GetMovable ()->UpdateMove ();
  csRef<iSprite3DState> spstate (
  	SCF_QUERY_INTERFACE (sprite->GetMeshObject (), iSprite3DState));
  spstate->SetAction ("default");

  // The following two calls are not needed since CS_ZBUF_USE and
  // Object render priority are the default but they show how you
  // can do this.
  sprite->SetZBufMode (CS_ZBUF_USE);
  sprite->SetRenderPriority (engine->GetObjectRenderPriority ());

  sprite->DeferUpdateLighting (CS_NLIGHT_STATIC|CS_NLIGHT_DYNAMIC, 10);
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

  csInitializer::DestroyApplication (object_reg);
  return 0;
}

