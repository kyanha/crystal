/*  -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
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
//#include "csutil/sysfunc.h"
#include "iutil/vfs.h"
#include "csutil/cscolor.h"
#include "cstool/csview.h"
#include "cstool/initapp.h"
#include "vostest.h"
#include "iutil/eventq.h"
#include "iutil/event.h"
#include "iutil/objreg.h"
#include "iutil/csinput.h"
#include "iutil/virtclk.h"
#include "iutil/plugin.h"
#include "iengine/sector.h"
#include "iengine/engine.h"
#include "iengine/camera.h"
#include "iengine/light.h"
#include "iengine/texture.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "iengine/material.h"
#include "imesh/thing.h"
#include "imesh/object.h"
#include "imesh/crossbld.h"
#include "imesh/mdlconv.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "ivideo/txtmgr.h"
#include "ivideo/texture.h"
#include "ivideo/material.h"
#include "ivideo/fontserv.h"
#include "ivideo/natwin.h"
#include "igraphic/imageio.h"
#include "imap/parser.h"
#include "ivaria/reporter.h"
#include "ivaria/stdrep.h"
#include "ivaria/engseq.h"
#include "csutil/cmdhelp.h"
#include "csutil/event.h"

#include "inetwork/vosa3dl.h"

#include <iostream>

CS_IMPLEMENT_APPLICATION

//-----------------------------------------------------------------------------

// The global pointer to vostest
Vostest *vostest;

Vostest::Vostest (iObjectRegistry* object_reg)
{
  Vostest::object_reg = object_reg;
}

Vostest::~Vostest ()
{
}

void Vostest::SetupFrame ()
{
  // First get elapsed time from the virtual clock.
  csTicks elapsed_time = vc->GetElapsedTicks ();
  // Now rotate the camera according to keyboard state
  float speed = (elapsed_time / 1000.0) * (0.06 * 20);

  iCamera* c = view->GetCamera();

  if (kbd->GetKeyState (CSKEY_SHIFT)) {
      if (kbd->GetKeyState (CSKEY_RIGHT))
          c->Move (CS_VEC_RIGHT * 4 * speed);
      if (kbd->GetKeyState (CSKEY_LEFT))
          c->Move (CS_VEC_LEFT * 4 * speed);
      if (kbd->GetKeyState (CSKEY_UP))
          c->Move (CS_VEC_UP * 4 * speed);
      if (kbd->GetKeyState (CSKEY_DOWN))
          c->Move (CS_VEC_DOWN * 4 * speed);
  } else {
      if (kbd->GetKeyState (CSKEY_RIGHT))
          rotY += speed;
      if (kbd->GetKeyState (CSKEY_LEFT))
          rotY -= speed;
      if (kbd->GetKeyState (CSKEY_PGUP))
          rotX += speed;
      if (kbd->GetKeyState (CSKEY_PGDN))
          rotX -= speed;
      if (kbd->GetKeyState (CSKEY_UP))
          c->Move (CS_VEC_FORWARD * 4 * speed);
      if (kbd->GetKeyState (CSKEY_DOWN))
          c->Move (CS_VEC_BACKWARD * 4 * speed);
  }

  csMatrix3 rot =  csXRotMatrix3(rotX) * csYRotMatrix3(rotY);
  csOrthoTransform ot(rot, c->GetTransform().GetOrigin());
  c->SetTransform(ot);


  // Tell 3D driver we're going to display 3D things.
  if (!g3d->BeginDraw (engine->GetBeginDrawFlags () | CSDRAW_3DGRAPHICS | CSDRAW_CLEARZBUFFER | CSDRAW_CLEARSCREEN))
    return;

  // Tell the camera to render into the frame buffer.
  view->Draw ();
}

void Vostest::FinishFrame ()
{
  g3d->FinishDraw ();
  g3d->Print (0);
}

bool Vostest::HandleEvent (iEvent& ev)
{
  if (ev.Type == csevBroadcast && ev.Command.Code == cscmdProcess)
  {
    vostest->SetupFrame ();
    return true;
  }
  else if (ev.Type == csevBroadcast && ev.Command.Code == cscmdFinalProcess)
  {
    vostest->FinishFrame ();
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

bool Vostest::VostestEventHandler (iEvent& ev)
{
  return vostest->HandleEvent (ev);
}

bool Vostest::Initialize ()
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
                                      CS_REQUEST_PLUGIN("crystalspace.network.vos.a3dl", iVosA3DL),
									  CS_REQUEST_PLUGIN("crystalspace.mesh.crossbuilder", iCrossBuilder),
									  CS_REQUEST_PLUGIN("crystalspace.modelconverter.multiplexer", iModelConverter),
                                      CS_REQUEST_END))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.application.vostest",
    "Can't initialize plugins!");
    return false;
  }

  if (!csInitializer::SetupEventHandler (object_reg, VostestEventHandler))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.application.vostest",
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
  if (vc == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.application.vostest",
    "Can't find the virtual clock!");
    return false;
  }

  // Find the pointer to engine plugin
  engine = CS_QUERY_REGISTRY (object_reg, iEngine);
  if (engine == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.application.vostest",
    "No iEngine plugin!");
    return false;
  }

#if 0
  csRef<iPluginManager> pm = CS_QUERY_REGISTRY (object_reg, iPluginManager);
  pm->LoadPlugin("crystalspace.utilities.sequence.engine", "iEngineSequenceManager", 3, true);
  pm->LoadPlugin("crystalspace.culling.frustvis", "iVisibilityCuller", 262144, true);
#endif

  loader = CS_QUERY_REGISTRY (object_reg, iLoader);
  if (loader == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.application.vostest",
        "No iLoader plugin!");
    return false;
  }

  g3d = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
  if (g3d == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.application.vostest",
        "No iGraphics3D plugin!");
    return false;
  }

  kbd = CS_QUERY_REGISTRY (object_reg, iKeyboardDriver);
  if (kbd == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.application.vostest",
        "No iKeyboardDriver plugin!");
    return false;
  }

  vosa3dl = CS_QUERY_REGISTRY (object_reg, iVosA3DL);
  if (vosa3dl == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.application.vostest",
        "No iVosA3DL plugin!");
    return false;
  }

  // Open the main system. This will open all the previously loaded plug-ins.
  if (!csInitializer::OpenApplication (object_reg))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.application.vostest",
        "Error opening system!");
    return false;
  }

  rotY = 0;
  rotX = 0;

  // First disable the lighting cache. Our app is simple enough
  // not to need this.
  engine->SetLightingCacheMode (0);

  if (!loader->LoadTexture ("stone", "/lib/std/stone4.gif"))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.application.vostest",
        "Error loading 'stone4' texture!");
    return false;
  }
  iMaterialWrapper* tm = engine->GetMaterialList ()->FindByName ("stone");

  engine->Prepare ();

  view = csPtr<iView> (new csView (engine, g3d));
  view->GetCamera ()->SetSector (engine->CreateSector("_tmp"));
  view->GetCamera ()->GetTransform ().SetOrigin (csVector3 (0, 5, -3));
  iGraphics2D* g2d = g3d->GetDriver2D ();
  view->SetRectangle (0, 0, g2d->GetWidth (), g2d->GetHeight ());

  if (!g3d->BeginDraw (engine->GetBeginDrawFlags () | CSDRAW_3DGRAPHICS | CSDRAW_CLEARZBUFFER | CSDRAW_CLEARSCREEN));
  view->Draw ();

  csRef<iVosSector> vossector = vosa3dl->GetSector("vop://localhost/world");
  vossector->Load();
  view->GetCamera()->SetSector(vossector->GetSector());

  return true;
}

void Vostest::Start ()
{
  csDefaultRunLoop (object_reg);
}

/*---------------------------------------------------------------------*
 * Main function
 *---------------------------------------------------------------------*/
int main (int argc, char* argv[])
{
  iObjectRegistry* object_reg = csInitializer::CreateEnvironment (argc, argv);

  vostest = new Vostest (object_reg);
  if (vostest->Initialize ())
    vostest->Start ();
  delete vostest;

  csInitializer::DestroyApplication (object_reg);
  return 0;
}

