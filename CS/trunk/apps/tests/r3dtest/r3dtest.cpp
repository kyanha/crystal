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
#include "iutil/vfs.h"
#include "csgeom/polyclip.h"
#include "csgeom/transfrm.h"
#include "csutil/cscolor.h"
#include "cstool/initapp.h"
#include "iengine/camera.h"
#include "iengine/engine.h"
#include "iengine/material.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "iengine/sector.h"
#include "igeom/clip2d.h"
#include "iutil/eventq.h"
#include "iutil/event.h"
#include "iutil/objreg.h"
#include "iutil/object.h"
#include "iutil/csinput.h"
#include "iutil/virtclk.h"
#include "imesh/thing/polygon.h"
#include "imesh/thing/thing.h"
#include "imesh/object.h"
#include "ivideo/graph2d.h"
#include "ivideo/txtmgr.h"
#include "ivideo/texture.h"
#include "ivideo/material.h"
#include "ivideo/fontserv.h"
#include "igraphic/imageio.h"
#include "imap/parser.h"
#include "ivaria/reporter.h"
#include "ivaria/stdrep.h"
#include "csutil/cmdhelp.h"
#ifdef CS_USE_NEW_RENDERER
#  include "ivideo/render3d.h"
#  include "ivideo/shader/shader.h"
#  include "ivideo/rndbuf.h"
#else
#  include "ivideo/graph3d.h"
#endif
#include "cstool/csview.h"

#include "r3dtest.h"

CS_IMPLEMENT_APPLICATION


//-----------------------------------------------------------------------------

// The global pointer to simple
R3DTest *r3dtest;

R3DTest::R3DTest (iObjectRegistry* object_reg)
{
  R3DTest::object_reg = object_reg;
}

R3DTest::~R3DTest ()
{
}

void R3DTest::SetupFrame ()
{
  // First get elapsed time from the virtual clock.
  csTicks elapsed_time = vc->GetElapsedTicks ();

  // Now rotate the camera according to keyboard state
  static int frame;
  bool stop = false;
  if(++frame == 100)
  {
    frame=0;
    stop=true;
  }
  float speed = (elapsed_time / 1000.0) * (0.03 * 20);

  int w = r3d->GetDriver2D ()->GetWidth()/2;
  int h = r3d->GetDriver2D ()->GetHeight()/2;
  int x = mouse->GetLastX();
  int y = mouse->GetLastY();

  bool moved = false;

  if (hasfocus)
  {
    view->GetCamera ()->GetTransform ().RotateThis (CS_VEC_TILT_UP, (y-h) * 0.01);
    view->GetCamera ()->GetTransform ().RotateOther (CS_VEC_ROT_RIGHT, (x-w) * 0.01);
    r3d->GetDriver2D ()->SetMousePosition (w, h);
    moved |= (y-h);
    moved |= (x-w);
  }

  if (kbd->GetKeyState (CSKEY_UP))
  {
    view->GetCamera ()->Move (CS_VEC_FORWARD * speed * 25.0);
    moved = true;
  }
  if (kbd->GetKeyState (CSKEY_DOWN))
  {
    view->GetCamera ()->Move (CS_VEC_BACKWARD * speed * 25.0);
    moved = true;
  }
  if (kbd->GetKeyState (CSKEY_LEFT))
  {
    view->GetCamera ()->Move (CS_VEC_LEFT * speed * 25.0);
    moved = true;
  }
  if (kbd->GetKeyState (CSKEY_RIGHT))
  {
    view->GetCamera ()->Move (CS_VEC_RIGHT * speed * 25.0);
    moved = true;
  }
  if (kbd->GetKeyState (CSKEY_HOME))
  {
    view->GetCamera ()->Move (CS_VEC_UP * speed * 25.0);
    moved = true;
  }
  if (kbd->GetKeyState (CSKEY_END))
  {
    view->GetCamera ()->Move (CS_VEC_DOWN * speed * 25.0);
    moved = true;
  }
/*
  // dump camera position. might be useful for debugging.
  if (moved)
  {
    csReversibleTransform ct = view->GetCamera()->GetTransform();
    const csVector3 camPos = ct.GetOrigin();
    const csVector3 camPlaneZ = ct.GetT2O().Col3 ();
    printf ("(%g,%g,%g) (%g,%g,%g)\n", camPos.x, camPos.y, camPos.z,
      camPlaneZ.x, camPlaneZ.y, camPlaneZ.z);
  }
  */

  r3d->SetPerspectiveAspect (r3d->GetDriver2D ()->GetHeight ());
  r3d->SetPerspectiveCenter (r3d->GetDriver2D ()->GetWidth ()/2,
                             r3d->GetDriver2D ()->GetHeight ()/2);

  // Tell 3D driver we're going to display 3D things.
  if (!r3d->BeginDraw (CSDRAW_3DGRAPHICS | CSDRAW_CLEARSCREEN | CSDRAW_CLEARZBUFFER))
    return;

  view->Draw ();

  r3d->FinishDraw ();
  if(stop)
    stop=false;
}

void R3DTest::FinishFrame ()
{
  r3d->Print (NULL);
}

bool R3DTest::HandleEvent (iEvent& ev)
{
  if (ev.Type == csevBroadcast && ev.Command.Code == cscmdFocusChanged)
  {
    hasfocus = (bool)ev.Command.Info;
    if (hasfocus)
    {
      int w = r3d->GetDriver2D ()->GetWidth()/2;
      int h = r3d->GetDriver2D ()->GetHeight()/2;
      r3d->GetDriver2D ()->SetMousePosition (w, h);
      r3d->GetDriver2D()->SetMouseCursor (csmcNone);
    }
    else
    {
      r3d->GetDriver2D()->SetMouseCursor (csmcArrow);
    }
  }
  else if (ev.Type == csevBroadcast && ev.Command.Code == cscmdProcess)
  {
    r3dtest->SetupFrame ();
    return true;
  }
  else if (ev.Type == csevBroadcast && ev.Command.Code == cscmdFinalProcess)
  {
    r3dtest->FinishFrame ();
    return true;
  }
  else if (ev.Type == csevKeyDown && ev.Key.Code == CSKEY_ESC)
  {
    csRef<iEventQueue> q (CS_QUERY_REGISTRY (object_reg, iEventQueue));
    if (q) q->GetEventOutlet()->Broadcast (cscmdQuit);
    return true;
  }

  return false;
}

bool R3DTest::SimpleEventHandler (iEvent& ev)
{
  return r3dtest->HandleEvent (ev);
}

bool R3DTest::Initialize ()
{
  if (!csInitializer::RequestPlugins (object_reg,
  	CS_REQUEST_VFS,
#ifdef CS_USE_NEW_RENDERER
        CS_REQUEST_PLUGIN ("crystalspace.render3d.opengl", iRender3D),
#else
        CS_REQUEST_PLUGIN ("crystalspace.graphics3d.opengl", iGraphics3D),
#endif
        CS_REQUEST_ENGINE,
	CS_REQUEST_IMAGELOADER,
	CS_REQUEST_LEVELLOADER,
	CS_REQUEST_FONTSERVER,
	CS_REQUEST_REPORTER,
	CS_REQUEST_REPORTERLISTENER,
 	CS_REQUEST_END))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.r3dtest",
	"Can't initialize plugins!");
    return false;
  }

  if (!csInitializer::SetupEventHandler (object_reg, SimpleEventHandler))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.r3dtest",
	"Can't initialize event handler!");
    return false;
  }

  // Check for commandline help.
  if (csCommandLineHelper::CheckHelp (object_reg))
  {
    csCommandLineHelper::Help (object_reg);
    return false;
  }

  vc = CS_QUERY_REGISTRY (object_reg, iVirtualClock);
  if (vc == NULL)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.r3dtest",
    	"No iVirtualClock plugin!");
    return false;
  }

  vfs = CS_QUERY_REGISTRY (object_reg, iVFS);
  if (vfs == NULL)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.r3dtest",
    	"No iVFS plugin!");
    return false;
  }

#ifdef CS_USE_NEW_RENDERER
  r3d = CS_QUERY_REGISTRY (object_reg, iRender3D);
#else
  r3d = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
#endif
  if (r3d == NULL)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.r3dtest",
    	"No iRender3D plugin!");
    return false;
  }


  engine = CS_QUERY_REGISTRY (object_reg, iEngine);
  if (engine == NULL)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.r3dtest",
    	"No iEngine plugin!");
    return false;
  }

  kbd = CS_QUERY_REGISTRY (object_reg, iKeyboardDriver);
  if (kbd == NULL)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.r3dtest",
    	"No iKeyboardDriver plugin!");
    return false;
  }

  mouse = CS_QUERY_REGISTRY (object_reg, iMouseDriver);
  if (mouse == NULL)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.r3dtest",
    	"No iMouseDriver plugin!");
    return false;
  }

  csRef<iLoader> loader = CS_QUERY_REGISTRY (object_reg, iLoader);
  if (loader == NULL)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.r3dtest",
    	"No iLoader plugin!");
    return false;
  }


  // Open the main system. This will open all the previously loaded plug-ins.
  if (!csInitializer::OpenApplication (object_reg))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.r3dtest",
    	"Error opening system!");
    return false;
  }

  csRef<iMaterial> shadow (engine->CreateBaseMaterial (NULL));
  csRef<iMaterialWrapper> mat = engine->GetMaterialList ()->NewMaterial (shadow);
  mat->QueryObject ()->SetName ("shadow extruder");

  // Change this path to something /Anders Stenberg
  vfs->Mount ("/lev/testrender", "$@data$/r3dbox.zip");
  vfs->ChDir ("/lev/testrender");
  if (!loader->LoadMapFile ("world", false))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.r3dtest",
    	"Couldn't load the test level!\nNote that this application is experimental.\nIf you don't have a test level for this app then you probably don't want to run it.");
    return false;
  }
  
  //loader->LoadMapFile("/this/data/test.xml", false);

  csRef<iSector> room = engine->FindSector ("room");

  view = csPtr<iView> (new csView (engine, r3d));
  view->GetCamera ()->SetSector (room);
  view->GetCamera ()->GetTransform ().SetOrigin (csVector3 (0, 0, 0));
  csRef<iGraphics2D> g2d = r3d->GetDriver2D ();
  view->SetRectangle (0, 0, g2d->GetWidth (), g2d->GetHeight ());

  bool hasAccel;
  if (g2d->PerformExtension ("hardware_accelerated", &hasAccel))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_NOTIFY,
      "crystalspace.application.r3dtest",
      "Hardware acceleration %s.\n",
      hasAccel ? "present" : "not present");
  }
  else
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_NOTIFY,
      "crystalspace.application.r3dtest",
      "Hardware acceleration check not available.\n");
  }

  r3d->GetDriver2D ()->SetMouseCursor( csmcNone );

#ifdef CS_USE_NEW_RENDERER
  // Load in lighting shaders
  csRef<iShaderManager> shmgr ( CS_QUERY_REGISTRY(object_reg, iShaderManager));
  if(shmgr)
  {
    csRef<iShader> shader (shmgr->CreateShader());
    if(shader)
    {
      shader->Load(csRef<iDataBuffer>(vfs->ReadFile("/shader/light.xml")));
      if(shader->Prepare())
      {
        for (int i=0; i<engine->GetMaterialList ()->GetCount (); i++)
          engine->GetMaterialList ()->Get (i)->GetMaterial ()->SetShader(shader);
      }
    }

    shader = shmgr->CreateShader();
    if(shader)
    {
      shader->Load(csRef<iDataBuffer>(vfs->ReadFile("/shader/shadow.xml")));
      if(shader->Prepare())
      {
        shadow->SetShader(shader);
      }
    }
  }

#endif // CS_USE_NEW_RENDERER

  engine->Prepare ();

  hasfocus = true;
  int w = r3d->GetDriver2D ()->GetWidth()/2;
  int h = r3d->GetDriver2D ()->GetHeight()/2;
  r3d->GetDriver2D ()->SetMousePosition (w, h);

  return true;
}

void R3DTest::Start ()
{
  csDefaultRunLoop (object_reg);
}

/*---------------------------------------------------------------------*
 * Main function
 *---------------------------------------------------------------------*/
int main (int argc, char* argv[])
{
  iObjectRegistry* object_reg = csInitializer::CreateEnvironment (argc, argv);

  r3dtest = new R3DTest (object_reg);
  if (r3dtest->Initialize ())
    r3dtest->Start ();
  delete r3dtest;

  csInitializer::DestroyApplication (object_reg);
  return 0;
}

