/*
    Copyright (C) 1998-2001 by Jorrit Tyberghein
    Copyright (C) 2001 by W.C.A Wijngaards

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


    Bumptest: This application shows how to fake bumpmapping by overlapping
    several texture layers and moving them acoording to the light.
*/

#include "cssysdef.h"
#include "cssys/sysfunc.h"
#include "bumptest.h"
#include "prbump.h"

#include "csgeom/transfrm.h"
#include "cstool/proctex.h"
#include "cstool/prdots.h"
#include "cstool/prplasma.h"
#include "cstool/prfire.h"
#include "cstool/prwater.h"
#include "cstool/csview.h"
#include "cstool/initapp.h"

#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "ivideo/natwin.h"
#include "ivideo/txtmgr.h"
#include "ivaria/conout.h"
#include "ivaria/reporter.h"
#include "ivaria/stdrep.h"
#include "imesh/object.h"
#include "imesh/ball.h"
#include "imesh/sprite3d.h"
#include "imesh/thing/polygon.h"
#include "imesh/thing/thing.h"
#include "imesh/thing/ptextype.h"
#include "imap/parser.h"
#include "iengine/mesh.h"
#include "iengine/engine.h"
#include "iengine/material.h"
#include "iengine/texture.h"
#include "iengine/sector.h"
#include "iengine/movable.h"
#include "iengine/light.h"
#include "iengine/dynlight.h"
#include "iengine/camera.h"
#include "igraphic/imageio.h"
#include "imesh/object.h"
#include "imesh/lighting.h"
#include "iutil/comp.h"
#include "iutil/eventh.h"
#include "iutil/eventq.h"
#include "iutil/event.h"
#include "iutil/objreg.h"
#include "iutil/csinput.h"
#include "iutil/virtclk.h"
#include "iutil/plugin.h"
#include "csutil/cmdhelp.h"

//------------------------------------------------- We need the 3D engine -----

CS_IMPLEMENT_APPLICATION

//-----------------------------------------------------------------------------

// the global system driver variable
BumpTest *bumptest;

BumpTest::BumpTest (iObjectRegistry* object_reg)
{
  BumpTest::object_reg = object_reg;
  prBump = NULL;
  matBump = NULL;
  bumplight = NULL;
  animli = 0.0;
  going_right = true;
  room = NULL;
}

BumpTest::~BumpTest ()
{
  delete prBump;
}

void BumpTest::Report (int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csRef<iReporter> rep (CS_QUERY_REGISTRY (object_reg, iReporter));
  if (rep)
    rep->ReportV (severity, "crystalspace.application.bumptest", msg, arg);
  else
  {
    csPrintfV (msg, arg);
    csPrintf ("\n");
  }
  va_end (arg);
}

bool BumpTest::InitProcDemo ()
{
  iTextureManager* txtmgr = myG3D->GetTextureManager ();
  iMaterialWrapper* itm = engine->GetMaterialList ()->FindByName ("wood");

  char *vfsfilename = "/lib/std/stone4.gif";
  iTextureWrapper *bptex = engine->CreateTexture("bumptex", vfsfilename, 0,
    CS_TEXTURE_2D| CS_TEXTURE_3D);
  iMaterialWrapper* ibp = engine->CreateMaterial("bumptexture", bptex);
  csRef<iImage> map = bptex->GetImageFile();
  engine->Prepare ();

  prBump = new csProcBump (map);

  matBump = prBump->Initialize (object_reg, engine, txtmgr, "bumps");
  prBump->PrepareAnim ();

  csRef<iPluginManager> plugin_mgr (
  	CS_QUERY_REGISTRY (object_reg, iPluginManager));
  csRef<iMeshObjectType> thing_type (CS_QUERY_PLUGIN_CLASS (plugin_mgr,
  	"crystalspace.mesh.object.thing", iMeshObjectType));
  if (!thing_type)
    thing_type = CS_LOAD_PLUGIN (plugin_mgr,
    	"crystalspace.mesh.object.thing", iMeshObjectType);
  if (!thing_type)
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "No thing mesh plugin!");
    return false;
  }

  csRef<iMeshObjectFactory> thing_fact (thing_type->NewFactory ());
  csRef<iMeshObject> thing_obj (SCF_QUERY_INTERFACE (thing_fact, iMeshObject));

  csRef<iMaterialWrapper> imatBump (
  	SCF_QUERY_INTERFACE (matBump, iMaterialWrapper));
  csRef<iThingState> thing_state (SCF_QUERY_INTERFACE (thing_obj, iThingState));
  float dx = 1, dy = 1, dz = 1;
  iPolygon3D* p;

  /// the stone
  p = thing_state->CreatePolygon ();
  p->SetMaterial (itm);
  p->CreateVertex (csVector3 (-dx, +dy, -dz));
  p->CreateVertex (csVector3 (+dx, +dy, -dz));
  p->CreateVertex (csVector3 (+dx, -dy, -dz));
  p->CreateVertex (csVector3 (-dx, -dy, -dz));
  p->SetTextureSpace (p->GetVertex (0), p->GetVertex (1), 1.0);

  // to seee unbumpmapped
  p = thing_state->CreatePolygon ();
  p->SetMaterial (itm);
  p->CreateVertex (csVector3 (-dx, +dy, -dz) + csVector3(-2.5,0,0));
  p->CreateVertex (csVector3 (+dx, +dy, -dz) + csVector3(-2.5,0,0));
  p->CreateVertex (csVector3 (+dx, -dy, -dz) + csVector3(-2.5,0,0));
  p->CreateVertex (csVector3 (-dx, -dy, -dz) + csVector3(-2.5,0,0));
  p->SetTextureSpace (p->GetVertex (0), p->GetVertex (1), 1.0);
  csVector3 overdist(0,0,-0.01); // to move slightly in front

  /// the bumpoverlay
  // this does not work - the texture is flat shaded too.
  p = thing_state->CreatePolygon ();
  p->SetTextureType(POLYTXT_LIGHTMAP);
  p->GetFlags().Set(CS_POLY_LIGHTING, 0); // the overlay is not lit
  p->SetMaterial (imatBump);
  p->CreateVertex (csVector3 (-dx, +dy, -dz)+overdist);
  p->CreateVertex (csVector3 (+dx, +dy, -dz)+overdist);
  p->CreateVertex (csVector3 (+dx, -dy, -dz)+overdist);
  p->CreateVertex (csVector3 (-dx, -dy, -dz)+overdist);
  p->SetTextureSpace (p->GetVertex (0), p->GetVertex (1), 1.0);

  iPolyTexType *ipn = p->GetPolyTexType();
  if(!ipn) printf("No PolyTexNone info!\n");
  else ipn->SetMixMode(CS_FX_MULTIPLY2);

  ////// copy of bumps for debug
  p = thing_state->CreatePolygon ();
  p->SetMaterial (imatBump);
  p->GetFlags().Set(CS_POLY_LIGHTING, 0); // not lit
  p->CreateVertex (csVector3 (-dx, +0, -dz) + csVector3(2.5,0,0));
  p->CreateVertex (csVector3 (+0, +0, -dz) + csVector3(2.5,0,0));
  p->CreateVertex (csVector3 (+0, -dy, -dz) + csVector3(2.5,0,0));
  p->CreateVertex (csVector3 (-dx, -dy, -dz) + csVector3(2.5,0,0));
  p->SetTextureSpace (p->GetVertex (0), p->GetVertex (1), 1.0);

  //  /*
  p = thing_state->CreatePolygon ();
  p->SetMaterial (ibp);
  p->CreateVertex (csVector3 (-dx, +0, -dz) + csVector3(2.5,1,0));
  p->CreateVertex (csVector3 (+0, +0, -dz) + csVector3(2.5,1,0));
  p->CreateVertex (csVector3 (+0, -dy, -dz) + csVector3(2.5,1,0));
  p->CreateVertex (csVector3 (-dx, -dy, -dz) + csVector3(2.5,1,0));
  p->SetTextureSpace (p->GetVertex (0), p->GetVertex (1), 1.0);
  //  */

  csRef<iMeshWrapper> thing_wrap (engine->CreateMeshWrapper ("Bumpy"));

  thing_wrap->SetMeshObject (thing_obj);
  thing_wrap->HardTransform (csTransform (csMatrix3 (), csVector3 (0, 5, 1)));
  thing_wrap->GetMovable ()->SetSector (room);
  thing_wrap->GetMovable ()->UpdateMove ();

  csRef<iLightingInfo> linfo (SCF_QUERY_INTERFACE (thing_obj, iLightingInfo));
  linfo->InitializeDefault ();
  room->ShineLights (thing_wrap);
  linfo->PrepareLighting ();


#if 0
  iMeshFactoryWrapper* sprfact = engine->CreateMeshFactory (
    "crystalspace.mesh.object.sprite.3d", "sprite3d");
  csRef<iSprite3DFactoryState> sprfactstate (SCF_QUERY_INTERFACE(
    sprfact->GetMeshObjectFactory(), iSprite3DFactoryState));
  sprfactstate->SetMaterialWrapper(imatBump);
  iSpriteAction *a0 = sprfactstate->AddAction();
  a0->SetName("Action0");
  iSpriteFrame *f0 = sprfactstate->AddFrame();
  a0->AddFrame(f0, 100);
  sprfactstate->AddVertices(4);
  sprfactstate->GetVertex(0,0).Set (csVector3 (-dx, +dy, -dz)+overdist);
  sprfactstate->GetVertex(0,1).Set (csVector3 (+dx, +dy, -dz)+overdist);
  sprfactstate->GetVertex(0,2).Set (csVector3 (+dx, -dy, -dz)+overdist);
  sprfactstate->GetVertex(0,3).Set (csVector3 (-dx, -dy, -dz)+overdist);
  sprfactstate->GetTexel(0,0).Set(0,0);
  sprfactstate->GetTexel(0,1).Set(2,0);
  sprfactstate->GetTexel(0,2).Set(2,2);
  sprfactstate->GetTexel(0,3).Set(0,2);
  sprfactstate->AddTriangle(0,1,2);
  sprfactstate->AddTriangle(0,2,3);
  iMeshWrapper* sprite = engine->CreateMeshWrapper(sprfact, "bumpspr",
    room, csVector3(0, 5, 1) );
  sprite->GetMovable ()->UpdateMove ();
  csRef<iSprite3DState> spstate (SCF_QUERY_INTERFACE (sprite->GetMeshObject (),
    iSprite3DState));

  spstate->SetLighting(false);
  spstate->SetBaseColor(csColor(1.,1.,1.));
  spstate->SetMaterialWrapper(imatBump);
  spstate->SetMixMode(CS_FX_MULTIPLY2 | CS_FX_TILING);
#endif

  return true;
}

static bool BumpEventHandler (iEvent& ev)
{
  if (ev.Type == csevBroadcast && ev.Command.Code == cscmdProcess)
  {
    bumptest->SetupFrame ();
    return true;
  }
  else if (ev.Type == csevBroadcast && ev.Command.Code == cscmdFinalProcess)
  {
    bumptest->FinishFrame ();
    return true;
  }
  else
  {
    return bumptest->BumpHandleEvent (ev);
  }
}

bool BumpTest::Initialize (int argc, const char* const argv[],
  const char *iConfigName)
{
  (void)argc;
  (void)argv;
  if (!csInitializer::SetupConfigManager (object_reg, iConfigName))
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Error initializing system!");
    return false;
  }

  if (!csInitializer::RequestPlugins (object_reg,
  	CS_REQUEST_REPORTER,
  	CS_REQUEST_REPORTERLISTENER,
  	CS_REQUEST_END))
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Initialization error!");
    return false;
  }

  if (!csInitializer::SetupEventHandler (object_reg, BumpEventHandler))
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Initialization error!");
    return false;
  }

  // Check for commandline help.
  if (csCommandLineHelper::CheckHelp (object_reg))
  {
    csCommandLineHelper::Help (object_reg);
    exit (0);
  }

  // The virtual clock.
  vc = CS_QUERY_REGISTRY (object_reg, iVirtualClock);

  // Find the pointer to engine plugin
  engine = CS_QUERY_REGISTRY (object_reg, iEngine);
  if (!engine)
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "No iEngine plugin!");
    abort ();
  }

  LevelLoader = CS_QUERY_REGISTRY (object_reg, iLoader);
  if (!LevelLoader)
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "No iLoader plugin!");
    abort ();
  }

  myG3D = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
  if (!myG3D)
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "No iGraphics3D plugin!");
    abort ();
  }

  kbd = CS_QUERY_REGISTRY (object_reg, iKeyboardDriver);
  if (!kbd)
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "No iKeyboardDriver plugin!");
    abort ();
  }

  // Open the main system. This will open all the previously loaded plug-ins.
  iNativeWindow* nw = myG3D->GetDriver2D ()->GetNativeWindow ();
  if (nw) nw->SetTitle ("Bumptest Crystal Space Application");
  if (!csInitializer::OpenApplication (object_reg))
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Error opening system!");
    return false;
  }

  // Setup the texture manager
  iTextureManager* txtmgr = myG3D->GetTextureManager ();
  txtmgr->SetVerbose (true);

  // Some commercials...
  Report (CS_REPORTER_SEVERITY_NOTIFY,
    "BumpTest Crystal Space Application version 0.1.");

  // First disable the lighting cache. Our app is simple enough
  // not to need this.
  engine->SetLightingCacheMode (0);

  // Create our world.
  Report (CS_REPORTER_SEVERITY_NOTIFY, "Creating world!...");

  LevelLoader->LoadTexture ("stone", "/lib/std/stone4.gif");
  LevelLoader->LoadTexture ("wood", "/lib/std/andrew_wood.gif");
  iMaterialWrapper* tm = engine->GetMaterialList ()->FindByName ("stone");

  room = engine->CreateSector ("room");
  csRef<iMeshWrapper> walls (engine->CreateSectorWallsMesh (room, "walls"));
  iPolygon3D* p;
  csRef<iThingState> walls_state (
  	SCF_QUERY_INTERFACE (walls->GetMeshObject (), iThingState));
  p = walls_state->CreatePolygon ();
  p->SetMaterial (tm);
  p->CreateVertex (csVector3 (-5, 0, 5));
  p->CreateVertex (csVector3 (5, 0, 5));
  p->CreateVertex (csVector3 (5, 0, -5));
  p->CreateVertex (csVector3 (-5, 0, -5));
  p->SetTextureSpace (p->GetVertex (0), p->GetVertex (1), 3);

  p = walls_state->CreatePolygon ();
  p->SetMaterial (tm);
  p->CreateVertex (csVector3 (-5, 20, -5));
  p->CreateVertex (csVector3 (5, 20, -5));
  p->CreateVertex (csVector3 (5, 20, 5));
  p->CreateVertex (csVector3 (-5, 20, 5));
  p->SetTextureSpace (p->GetVertex (0), p->GetVertex (1), 3);

  p = walls_state->CreatePolygon ();
  p->SetMaterial (tm);
  p->CreateVertex (csVector3 (-5, 20, 5));
  p->CreateVertex (csVector3 (5, 20, 5));
  p->CreateVertex (csVector3 (5, 0, 5));
  p->CreateVertex (csVector3 (-5, 0, 5));
  p->SetTextureSpace (p->GetVertex (0), p->GetVertex (1), 3);

  p = walls_state->CreatePolygon ();
  p->SetMaterial (tm);
  p->CreateVertex (csVector3 (5, 20, 5));
  p->CreateVertex (csVector3 (5, 20, -5));
  p->CreateVertex (csVector3 (5, 0, -5));
  p->CreateVertex (csVector3 (5, 0, 5));
  p->SetTextureSpace (p->GetVertex (0), p->GetVertex (1), 3);

  p = walls_state->CreatePolygon ();
  p->SetMaterial (tm);
  p->CreateVertex (csVector3 (-5, 20, -5));
  p->CreateVertex (csVector3 (-5, 20, 5));
  p->CreateVertex (csVector3 (-5, 0, 5));
  p->CreateVertex (csVector3 (-5, 0, -5));
  p->SetTextureSpace (p->GetVertex (0), p->GetVertex (1), 3);

  p = walls_state->CreatePolygon ();
  p->SetMaterial (tm);
  p->CreateVertex (csVector3 (5, 20, -5));
  p->CreateVertex (csVector3 (-5, 20, -5));
  p->CreateVertex (csVector3 (-5, 0, -5));
  p->CreateVertex (csVector3 (5, 0, -5));
  p->SetTextureSpace (p->GetVertex (0), p->GetVertex (1), 3);

#if 0
  LevelLoader->LoadTexture ("flare_center", "/lib/std/snow.jpg");
  iMaterialWrapper* fmc = engine->GetMaterialList ()->
  	FindByName ("flare_center");
  LevelLoader->LoadTexture ("flare_spark", "/lib/std/spark.png");
  iMaterialWrapper* fms = engine->GetMaterialList ()->
  	FindByName ("flare_spark");
#endif

  InitProcDemo ();

  dynlight = engine->CreateDynLight (csVector3 (-3, 5, -2), 7, csColor (1, 1, 1));
  dynlight->QueryLight ()->CreateCrossHalo (1.0, 0.7);  // intensity, crossfactor
  dynlight->QueryLight ()->SetSector (room);
  dynlight->Setup ();
  bumplight = dynlight->QueryLight ();

  Report (CS_REPORTER_SEVERITY_NOTIFY, "--------------------------------------");

  // csView is a view encapsulating both a camera and a clipper.
  // You don't have to use csView as you can do the same by
  // manually creating a camera and a clipper but it makes things a little
  // easier.
  view = csPtr<iView> (new csView (engine, myG3D));
  view->GetCamera ()->SetSector (room);
  view->GetCamera ()->GetTransform ().SetOrigin (csVector3 (0, 5, -3));
  iGraphics2D* g2d = myG3D->GetDriver2D ();
  view->SetRectangle (0, 0, g2d->GetWidth (), g2d->GetHeight ());

  return true;
}

void BumpTest::SetupFrame ()
{
  csTicks elapsed_time, current_time;
  elapsed_time = vc->GetElapsedTicks ();
  current_time = vc->GetCurrentTicks ();

  // Now rotate the camera according to keyboard state
  float speed = (elapsed_time / 1000.) * (0.03 * 20);

  if (kbd->GetKeyState (CSKEY_RIGHT))
    view->GetCamera ()->GetTransform ().RotateThis (CS_VEC_ROT_RIGHT, speed);
  if (kbd->GetKeyState (CSKEY_LEFT))
    view->GetCamera ()->GetTransform ().RotateThis (CS_VEC_ROT_LEFT, speed);
  if (kbd->GetKeyState (CSKEY_PGUP))
    view->GetCamera ()->GetTransform ().RotateThis (CS_VEC_TILT_UP, speed);
  if (kbd->GetKeyState (CSKEY_PGDN))
    view->GetCamera ()->GetTransform ().RotateThis (CS_VEC_TILT_DOWN, speed);
  if (kbd->GetKeyState (CSKEY_UP))
    view->GetCamera ()->Move (CS_VEC_FORWARD * 4.0f * speed);
  if (kbd->GetKeyState (CSKEY_DOWN))
    view->GetCamera ()->Move (CS_VEC_BACKWARD * 4.0f * speed);


  // Move the -dynamic light around.
  if(going_right)
  {
    animli += speed * 2.5;
    if(animli > 7.0) going_right = false;
  }
  else
  {
    animli -= speed * 2.5;
    if(animli < 0.0) going_right = true;
  }
  dynlight->QueryLight ()->SetSector (room);
  dynlight->QueryLight ()->SetCenter (csVector3(-3 + animli, 5, -2));
  dynlight->Setup ();


  csVector3 center(0,5,-1);
  csVector3 normal(0,0,-1);
  //(-1,5+1,-1); (+1,5-1,-1);
  csVector3 xdir(1,0,0);
  csVector3 ydir(0,-1,0);
  iLight *l = bumplight;

  prBump->RecalcFast(center, normal, xdir, ydir, 1, &l);

  // Tell 3D driver we're going to display 3D things.
  if (!myG3D->BeginDraw (engine->GetBeginDrawFlags () | CSDRAW_3DGRAPHICS))
    return;

  view->Draw ();

  // Start drawing 2D graphics.
  if (!myG3D->BeginDraw (CSDRAW_2DGRAPHICS)) return;
}

void BumpTest::FinishFrame ()
{
  myG3D->FinishDraw ();
  myG3D->Print (NULL);
}

bool BumpTest::BumpHandleEvent (iEvent &Event)
{
  if ((Event.Type == csevKeyDown) && (Event.Key.Code == CSKEY_ESC))
  {
    csRef<iEventQueue> q (CS_QUERY_REGISTRY (object_reg, iEventQueue));
    if (q)
      q->GetEventOutlet()->Broadcast (cscmdQuit);
    return true;
  }

  return false;
}

void BumpTest::Start ()
{
  csDefaultRunLoop (object_reg);
}

/*---------------------------------------------------------------------*
 * Main function
 *---------------------------------------------------------------------*/
int main (int argc, char* argv[])
{
  srand (time (NULL));

  iObjectRegistry* object_reg = csInitializer::CreateEnvironment (argc, argv);
  if (!object_reg) return -1;

  // Create our main class.
  bumptest = new BumpTest (object_reg);

  if (bumptest->Initialize (argc, argv, "/config/csbumptest.cfg"))
    bumptest->Start ();

  delete bumptest;

  csInitializer::DestroyApplication (object_reg);

  return 0;
}
