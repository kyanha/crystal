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

/* ViewMesh: tool for displaying mesh objects (3d sprites) */
#include "cssysdef.h"
#include "cssys/sysfunc.h"
#include "csutil/cscolor.h"
#include "cstool/csview.h"
#include "cstool/initapp.h"
#include "csutil/cmdhelp.h"
#include "csutil/util.h"
#include "viewmesh.h"
#include "iutil/eventq.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "iutil/cmdline.h"
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
#include "imesh/sprite3d.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "ivideo/natwin.h"
#include "ivideo/txtmgr.h"
#include "ivideo/texture.h"
#include "ivideo/material.h"
#include "ivideo/fontserv.h"
#include "igraphic/imageio.h"
#include "imap/parser.h"
#include "ivaria/reporter.h"
#include "ivaria/stdrep.h"
#include "iutil/vfs.h"
#include "csutil/csstrvec.h"
#include "iutil/cache.h"
#include "csutil/nulcache.h"

#include "iutil/plugin.h"
#include "imap/writer.h"

CS_IMPLEMENT_APPLICATION

#define VIEWMESH_COMMAND_LOADMESH 77701
#define VIEWMESH_COMMAND_SAVEMESH 77702
#define VIEWMESH_COMMAND_LOADLIB  77703
#define VIEWMESH_STATES_SELECT_START  77800
#define VIEWMESH_OVERRIDE_SELECT_START  77900
#define VIEWMESH_COMMAND_CAMMODE1 77711
#define VIEWMESH_COMMAND_CAMMODE2 77712
#define VIEWMESH_COMMAND_CAMMODE3 77713
#define VIEWMESH_COMMAND_MOVEANIMFASTER 77714
#define VIEWMESH_COMMAND_MOVEANIMSLOWER 77715
#define VIEWMESH_COMMAND_REVERSEACTION  77716
#define VIEWMESH_COMMAND_FORWARDACTION  77717

//-----------------------------------------------------------------------------

ViewMesh::ViewMesh (iObjectRegistry *object_reg, csSkin &Skin)
    : csApp (object_reg, Skin)
{
  SetBackgroundStyle(csabsNothing);
  menu = NULL;
  dialog = NULL;
  cammode = movenormal;
  spritepos = csVector3(0,10,0);
  move_sprite_speed = 0;
}

ViewMesh::~ViewMesh ()
{
  if (menu) delete menu;
  if (dialog) delete dialog;
}

void ViewMesh::Help ()
{
  printf ("Options for ViewMesh:\n");
  printf ("  -L=<file>          Load a library file (for textures/materials)\n");
  printf ("  -Scale=<ratio>     Scale the Object\n");
  printf ("  <meshfile>         Load the specified mesh object\n");
}

/* This is the data we keep for modal processing */
struct ModalData : public iBase
{
  uint code;
  SCF_DECLARE_IBASE;
  ModalData() { SCF_CONSTRUCT_IBASE(NULL); }
};

SCF_IMPLEMENT_IBASE (ModalData)
SCF_IMPLEMENT_IBASE_END

bool ViewMesh::HandleEvent (iEvent& ev)
{
  if (ev.Type == csevBroadcast && ev.Command.Code == cscmdProcess)
  {
    // display next frame
    Draw();
  }

  if (csApp::HandleEvent(ev))
    return true;

  switch(ev.Type)
  {
    case csevKeyDown:
      if (ev.Key.Code == CSKEY_ESC)
      {
	csRef<iEventQueue> q (CS_QUERY_REGISTRY (object_reg, iEventQueue));
	if (q)
	  q->GetEventOutlet()->Broadcast
	    (cscmdQuit);
	return true;
      }
      break;
    case csevMouseDown:
    case csevMouseDoubleClick:
      // show the menu
      if (menu->GetState(CSS_VISIBLE))
	menu->Hide();
      else
      {
	menu->Show();
	menu->SetPos(ev.Mouse.x,ev.Mouse.y);
      }
      return true;
    case csevCommand:
      switch(ev.Command.Code)
      {
	case VIEWMESH_COMMAND_LOADMESH:
	{
	  menu->Hide();
	  delete dialog;
	  dialog = csFileDialog (this, "Select Mesh Object", "/this/", "Open",
	      true);

	  ModalData *data=new ModalData;
	  data->code = VIEWMESH_COMMAND_LOADMESH;
	  StartModal (dialog, data);

	  return true;
	}
	case VIEWMESH_COMMAND_SAVEMESH:
	{
	  menu->Hide();
	  delete dialog;
	  dialog= csFileDialog (this, "Save As...", "/this/", "Save",
	      true);

	  ModalData *data=new ModalData;
	  data->code = VIEWMESH_COMMAND_SAVEMESH;
	  StartModal (dialog, data);

	  return true;
	}
	case VIEWMESH_COMMAND_LOADLIB:
	{
	  menu->Hide();
	  delete dialog;
	  dialog = csFileDialog (this, "Select Texture Lib", "/this/", "Open",
	      true);

	  ModalData *data=new ModalData;
	  data->code = VIEWMESH_COMMAND_LOADLIB;
	  StartModal (dialog, data);

	  return true;
	}
	case VIEWMESH_COMMAND_MOVEANIMFASTER:
	  move_sprite_speed += .5;
	  menu->Hide();
	  return true;

	case VIEWMESH_COMMAND_MOVEANIMSLOWER:
	  move_sprite_speed -= .5;
	  menu->Hide();
	  return true;

	case VIEWMESH_COMMAND_REVERSEACTION:
	{
	  csRef<iSprite3DState> spstate (
	      SCF_QUERY_INTERFACE(sprite->GetMeshObject(),iSprite3DState));
	  if (spstate)
	      spstate->SetReverseAction(true);
	  menu->Hide();
	  return true;
	}
	case VIEWMESH_COMMAND_FORWARDACTION:
	{
	  csRef<iSprite3DState> spstate (
	      SCF_QUERY_INTERFACE(sprite->GetMeshObject(),iSprite3DState));
	  if (spstate)
	      spstate->SetReverseAction(false);
	  menu->Hide();
	  return true;
	}
	case VIEWMESH_COMMAND_CAMMODE1:
	  cammode = movenormal;
	  menu->Hide();
	  return true;
	case VIEWMESH_COMMAND_CAMMODE2:
	  cammode = moveorigin;
	  menu->Hide();
	  return true;
	case VIEWMESH_COMMAND_CAMMODE3:
	  cammode = rotateorigin;
	  menu->Hide();
	  return true;
	case cscmdStopModal:
	{
	  char filename[1024];
	  csQueryFileDialog (dialog, filename, sizeof(filename));
	  delete dialog;
	  dialog = NULL;
	  ModalData *data = (ModalData *) GetTopModalUserdata();

	  switch (data->code)
	  {
	    case VIEWMESH_COMMAND_LOADMESH:
	      if (!LoadSprite(filename,1))
	      {
		Printf (CS_REPORTER_SEVERITY_ERROR, "couldn't load mesh %s",
		    filename);
	      }
	      break;
	    case VIEWMESH_COMMAND_SAVEMESH:
	      if (!SaveSprite(filename))
	      {
		Printf (CS_REPORTER_SEVERITY_ERROR, "couldn't save mesh %s",
		    filename);
	      }
	      break;
	    case VIEWMESH_COMMAND_LOADLIB:
	      if (!loader->LoadLibraryFile(filename))
	      {
		Printf (CS_REPORTER_SEVERITY_ERROR, "couldn't load lib %s",
		    filename);
	      }
	      else
	      {
		// register the textures
		engine->Prepare();
	      }
	      break;
	  }
	  ConstructMenu();
	  return true;
	}
	default:
	  break;
      }
      if (ev.Command.Code > VIEWMESH_STATES_SELECT_START &&
	  ev.Command.Code < VIEWMESH_STATES_SELECT_START + 100)
      {
      	csRef<iSprite3DState> spstate (
		SCF_QUERY_INTERFACE(sprite->GetMeshObject(),
		iSprite3DState));
	if (spstate)
	  spstate->SetAction(
	      stateslist.Get(ev.Command.Code - VIEWMESH_STATES_SELECT_START) );
	menu->Hide();
	return true;
      }
      if (ev.Command.Code > VIEWMESH_OVERRIDE_SELECT_START &&
	  ev.Command.Code < VIEWMESH_OVERRIDE_SELECT_START + 100)
      {
      	csRef<iSprite3DState> spstate (
		SCF_QUERY_INTERFACE(sprite->GetMeshObject(),
		iSprite3DState));
	if (spstate)
	  spstate->SetOverrideAction(
	      stateslist.Get(ev.Command.Code - VIEWMESH_OVERRIDE_SELECT_START) );
	menu->Hide();
	return true;
      }
      break;
  }

  return false;
}

bool ViewMesh::LoadSprite(const char *filename,float scale)
{
  // grab the directory.
  char *path = new char[strlen(filename)+1];
  strcpy (path, filename);
  char* fn = path;
  char* slash = strrchr (path, '/');
  char* dir;
  if (slash)
  {
    fn = slash + 1;
    *slash = 0;
    dir = path;
  }
  else
    dir = "/";
  VFS->ChDir (dir);

  csRef<iMeshFactoryWrapper> imeshfactwrap (
  	loader->LoadMeshObjectFactory (fn));
  delete[] path;

  if (!imeshfactwrap)
    return false;

  // eventually remove the old sprite
  // FIXME: This badly fails if you load the same object again!
  if (sprite)
  {
    sprite->GetMovable()->ClearSectors();
    engine->GetMeshes()->Remove(sprite);
  }

  sprite = engine->CreateMeshWrapper(
      imeshfactwrap, "MySprite", room,
      csVector3 (0, 10, 0));
  csMatrix3 m; m.Identity(); m *= scale;
  csRef<iSprite3DState> spstate (SCF_QUERY_INTERFACE(sprite->GetMeshObject(),
      iSprite3DState));
  if (spstate)
    spstate->SetAction("default");

  // Update Sprite States menu
  stateslist.Push (csStrNew("default"));
  imeshfact = imeshfactwrap->GetMeshObjectFactory();
  csRef<iSprite3DFactoryState> factstate(SCF_QUERY_INTERFACE(imeshfact,
      iSprite3DFactoryState));
  if (factstate)
  {
    for (int i=0;i<factstate->GetActionCount();i++)
    {
      iSpriteAction *spaction = factstate->GetAction(i);
      stateslist.Push (csStrNew(spaction->GetName()));
    }
  }
  sprite->DeferUpdateLighting (CS_NLIGHT_DYNAMIC|CS_NLIGHT_STATIC, 10);

  // try to get center of the sprite
  csBox3 box;
  sprite->GetWorldBoundingBox(box);
  spritepos = box.GetCenter();

  return true;
}

bool ViewMesh::SaveSprite(const char *filename)
{
  csRef<iPluginManager> plugin_mgr (
  	CS_QUERY_REGISTRY (object_reg, iPluginManager));

  csRef<iSaverPlugin> saver (CS_LOAD_PLUGIN (plugin_mgr,
    "crystalspace.mesh.saver.factory.sprite.3d.binary", iSaverPlugin));
	
	
  csRef<iVFS> VFS (CS_QUERY_REGISTRY (object_reg, iVFS));

  csRef<iFile> cf (VFS->Open (filename, VFS_FILE_WRITE));
  saver->WriteDown(imeshfact, cf);

  return true;
}

void ViewMesh::ConstructMenu()
{
  int i;

  if (menu)
    delete menu;

  menu = new csMenu(this, csmfs3D, 0);
  (void)new csMenuItem(menu,"Load Mesh", VIEWMESH_COMMAND_LOADMESH);
  (void)new csMenuItem(menu,"Save Mesh (Binary)", VIEWMESH_COMMAND_SAVEMESH);
  (void)new csMenuItem(menu,"Load TextureLib", VIEWMESH_COMMAND_LOADLIB);

  // AnimMenu
  csMenu *animmenu = new csMenu(NULL);
  (void)new csMenuItem(animmenu,"Move Sprite Faster", VIEWMESH_COMMAND_MOVEANIMFASTER);
  (void)new csMenuItem(animmenu,"Move Sprite Slower", VIEWMESH_COMMAND_MOVEANIMSLOWER);
  (void)new csMenuItem(animmenu,"Reverse Action",     VIEWMESH_COMMAND_REVERSEACTION);
  (void)new csMenuItem(animmenu,"Forward Action",     VIEWMESH_COMMAND_FORWARDACTION);
  (void)new csMenuItem(menu, "Action Effects",animmenu);

  // StateMenu
  csMenu *statesmenu = new csMenu(NULL);
  for (i=0;i<stateslist.Length();i++)
  {
    (void)new csMenuItem(statesmenu, stateslist.Get(i),
			 VIEWMESH_STATES_SELECT_START+i);
  }
  (void)new csMenuItem(menu, "States", statesmenu);

  // OverrideActionMenu
  csMenu *overridemenu = new csMenu(NULL);
  for (i=0;i<stateslist.Length();i++)
  {
    (void)new csMenuItem(overridemenu, stateslist.Get(i),
			 VIEWMESH_OVERRIDE_SELECT_START+i);
  }
  (void)new csMenuItem(menu, "Overrides", overridemenu);

  // Camera Mode
  csMenu *cammode = new csMenu(NULL);
  (void)new csMenuItem(cammode, "Normal Movement", VIEWMESH_COMMAND_CAMMODE1);
  (void)new csMenuItem(cammode, "Look to Origin", VIEWMESH_COMMAND_CAMMODE2);
  (void)new csMenuItem(cammode, "Rotate", VIEWMESH_COMMAND_CAMMODE3);
  (void)new csMenuItem(menu, "Camera Mode", cammode);

  (void)new csMenuItem(menu,"~Quit", cscmdQuit);
  menu->Hide();
}

void ViewMesh::UpdateSpritePosition(csTicks elapsed)
{
    if (!sprite)
	return;
    if (!move_sprite_speed)
	return;

    csRef<iMovable> mov = sprite->GetMovable();
    
    csVector3 v(0,0,-move_sprite_speed*elapsed/1000);
    mov->MovePosition(v);

    v = mov->GetFullPosition();
    float const absz = (v.z < 0 ? -v.z : v.z);
    if (absz > 4.5)  // this should make the sprite loop.
    {
	v.z = -v.z;
	mov->SetPosition(v);
    }
    mov->UpdateMove();
}

void ViewMesh::Draw()
{
  // First get elapsed time from the system driver.
  csTicks elapsed_time, current_time;
  elapsed_time = vc->GetElapsedTicks ();
  current_time = vc->GetCurrentTicks ();

  // Now rotate the camera according to keyboard state
  float speed = (elapsed_time / 1000.0) * (0.03 * 20);

  if (sprite)
    UpdateSpritePosition(elapsed_time);

  if (!dialog && !menu->GetState(CSS_VISIBLE))
  {
    iCamera* c = view->GetCamera();
    switch (cammode)
    {
      case movenormal:
    	if (GetKeyState (CSKEY_RIGHT))
	  c->GetTransform ().RotateThis (CS_VEC_ROT_RIGHT, speed);
	if (GetKeyState (CSKEY_LEFT))
	  c->GetTransform ().RotateThis (CS_VEC_ROT_LEFT, speed);
	if (GetKeyState (CSKEY_PGUP))
	  c->GetTransform ().RotateThis (CS_VEC_TILT_UP, speed);
	if (GetKeyState (CSKEY_PGDN))
	  c->GetTransform ().RotateThis (CS_VEC_TILT_DOWN, speed);
	if (GetKeyState (CSKEY_UP))
	  c->Move (CS_VEC_FORWARD * 4 * speed);
	if (GetKeyState (CSKEY_DOWN))
	  c->Move (CS_VEC_BACKWARD * 4 * speed);
	break;
      case moveorigin:
	{
	  csVector3 orig = c->GetTransform().GetOrigin();

	  if (GetKeyState (CSKEY_DOWN))
	    c->GetTransform().SetOrigin (orig + CS_VEC_BACKWARD * 4 * speed);
	  if (GetKeyState (CSKEY_UP))
	    c->GetTransform().SetOrigin (orig + CS_VEC_FORWARD * 4 * speed);
	  if (GetKeyState (CSKEY_LEFT))
	    c->GetTransform().SetOrigin (orig + CS_VEC_LEFT * 4 * speed);
	  if (GetKeyState (CSKEY_RIGHT))
	    c->GetTransform().SetOrigin (orig + CS_VEC_RIGHT * 4 * speed);
	  if (GetKeyState (CSKEY_PGUP))
	    c->GetTransform().SetOrigin (orig + CS_VEC_UP * 4 * speed);
	  if (GetKeyState (CSKEY_PGDN))
	    c->GetTransform().SetOrigin (orig + CS_VEC_DOWN * 4 * speed);
	  c->GetTransform().LookAt (spritepos-orig, csVector3(0,1,0) );
  	  break;
	}
      case rotateorigin:
	{
	  csVector3 orig = c->GetTransform().GetOrigin();
	  if (GetKeyState (CSKEY_LEFT))
	    orig = csYRotMatrix3(-speed) * (orig-spritepos) + spritepos;
	  if (GetKeyState (CSKEY_RIGHT))
	    orig = csYRotMatrix3(speed) * (orig-spritepos) + spritepos;
	  if (GetKeyState (CSKEY_UP))
	    orig = csXRotMatrix3(speed) * (orig-spritepos) + spritepos;
	  if (GetKeyState (CSKEY_DOWN))
	    orig = csXRotMatrix3(-speed) * (orig-spritepos) + spritepos;
	  c->GetTransform().SetOrigin(orig);
	  if (GetKeyState (CSKEY_PGUP))
	    c->Move(CS_VEC_FORWARD * 4 * speed);
	  if (GetKeyState (CSKEY_PGDN))
	    c->Move(CS_VEC_BACKWARD * 4 * speed);
	  c->GetTransform().LookAt (spritepos-orig, csVector3(0,1,0) );
	  break;
	}
      default:
	break;
    }
  }

  csApp::Draw();
  pplBeginDraw(CSDRAW_3DGRAPHICS);
  view->Draw();
  pplInvalidate(bound);
  if (menu->GetState(CSS_VISIBLE)) {
    menu->Invalidate(true);
  }
  if (dialog) {
    dialog->Invalidate(true);
  }
}

#define VM_QUERYPLUGIN(var, intf, str)				\
  var = CS_QUERY_REGISTRY (object_reg, intf);			\
  if (!var)							\
  {								\
    Printf (CS_REPORTER_SEVERITY_ERROR, "No " str " plugin!");	\
    return false;						\
  }

bool ViewMesh::Initialize ()
{
  if (!csApp::Initialize())
    return false;

  // Query for plugins
  // Find the pointer to engine plugin
  VM_QUERYPLUGIN (engine, iEngine, "iEngine");

  VM_QUERYPLUGIN (loader, iLoader, "iLoader");
  VM_QUERYPLUGIN (g3d, iGraphics3D, "iGraphics3D");

  // Set up a null cache.
  iCacheManager *cachemgr = new csNullCacheManager ();
  engine->SetCacheManager (cachemgr);
  cachemgr->DecRef ();

  // Open the main system. This will open all the previously loaded plug-ins.
  iGraphics2D* g2d = g3d->GetDriver2D ();
  iNativeWindow* nw = g2d->GetNativeWindow ();
  if (nw)
    nw->SetTitle ("View Mesh");

  // Setup the texture manager
  iTextureManager* txtmgr = g3d->GetTextureManager ();
  txtmgr->SetVerbose (true);

  Printf (CS_REPORTER_SEVERITY_NOTIFY,
    "View Mesh version 0.1.");

  // First disable the lighting cache. Our app is simple enough
  // not to need this.
  engine->SetLightingCacheMode (0);

  // Create our world.
  Printf (CS_REPORTER_SEVERITY_NOTIFY, "Creating world!...");

  if (!loader->LoadTexture ("stone", "/lib/std/stone4.gif"))
  {
    Printf (CS_REPORTER_SEVERITY_ERROR, "Error loading 'stone4' texture!");
    return false;
  }
  iMaterialWrapper* tm = engine->GetMaterialList ()->FindByName ("stone");

  room = engine->CreateSector ("room");
  csRef<iMeshWrapper> walls (engine->CreateSectorWallsMesh (room, "walls"));
  csRef<iThingState> walls_state (SCF_QUERY_INTERFACE (walls->GetMeshObject (),
  	iThingState));
  iPolygon3D* p;
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

  csRef<iStatLight> light;
  iLightList* ll = room->GetLights ();
  light = engine->CreateLight (NULL, csVector3 (-3, 10, 0), 10,
  	csColor (.8, .8, .8), false);
  ll->Add (light->QueryLight ());

  light = engine->CreateLight (NULL, csVector3 (3, 10,  0), 10,
  	csColor (.8, .8, .8), false);
  ll->Add (light->QueryLight ());

  light = engine->CreateLight (NULL, csVector3 (0, 10, -3), 10,
  	csColor (.8, .8, .8), false);
  ll->Add (light->QueryLight ());

  light = engine->CreateLight (NULL, csVector3 (0, 10,  3), 10,
  	csColor (.8, .8, .8), false);
  ll->Add (light->QueryLight ());

  engine->Prepare ();
  Printf (CS_REPORTER_SEVERITY_NOTIFY, "Created.");

  view = csPtr<iView> (new csView (engine, g3d));
  view->GetCamera ()->SetSector (room);
  view->GetCamera ()->GetTransform ().SetOrigin (csVector3 (0, 10, -4));
  view->SetRectangle (0, 0, g2d->GetWidth (), g2d->GetHeight ());

  csRef<iCommandLineParser> cmdline;
  VM_QUERYPLUGIN (cmdline, iCommandLineParser, "iCommandLineParser");

  const char* meshfilename = cmdline->GetName (0);
  const char* texturefilename = cmdline->GetName (1);
  const char* texturename = cmdline->GetName (2);
  const char* scaleTxt = cmdline->GetOption("Scale");
  float scale = 1;
  if (scaleTxt != NULL)
  {
    sscanf (scaleTxt, "%f", &scale);
  }

  // Load specified Libraries
  Printf (CS_REPORTER_SEVERITY_NOTIFY, "Loading libs...");
  const char *libname;
  int i;
  for (i=0; (libname=cmdline->GetOption("Lib",i)); i++)
  {
    if (!loader->LoadLibraryFile(libname))
    {
      Printf (CS_REPORTER_SEVERITY_ERROR, "Couldn load lib %s.", libname);
    }
  }
  if (i>0)
    engine->Prepare();

  // Load a texture for our sprite.
  if (texturefilename && texturename)
  {
    iTextureWrapper* txt = loader->LoadTexture (texturename,
  	  texturefilename);
    if (txt == NULL)
    {
      Printf (CS_REPORTER_SEVERITY_ERROR, "Error loading texture '%s'!",
      	texturefilename);
      return false;
    }
    txt->Register (txtmgr);
    txt->GetTextureHandle()->Prepare ();
    iMaterialWrapper* mat = engine->GetMaterialList ()->FindByName (
    	texturename);
    mat->Register (txtmgr);
    mat->GetMaterialHandle ()->Prepare ();
  }

  // Load a sprite template from disk.
  if (meshfilename && !LoadSprite(meshfilename,scale))
  {
    Printf (CS_REPORTER_SEVERITY_ERROR,
    	"Error loading mesh object factory '%s'!", meshfilename);
    return false;
  }

  // Create an menu
  ConstructMenu();

  return true;
}

/*---------------------------------------------------------------------*
 * Main function
 *---------------------------------------------------------------------*/

// define a skin for csws
CSWS_SKIN_DECLARE_DEFAULT (DefaultSkin);

int main (int argc, char* argv[])
{
  srand (time (NULL));

  iObjectRegistry *object_reg = csInitializer::CreateEnvironment(argc, argv);
  if (!object_reg)
    return 1;

  if (!csInitializer::SetupConfigManager (object_reg, NULL))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
	"crystalspace.application.viewmesh", "Couldn't load config file!");
    return 1;
  }
	
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
	"crystalspace.application.viewmesh", "Couldn't find plugins!\n"
	"Is your CRYSTAL environment var properly set?");
    return 1;
  }

  if (csCommandLineHelper::CheckHelp (object_reg))
  {
    ViewMesh::Help();
    csCommandLineHelper::Help(object_reg);
    return 0;
  }

  if (!csInitializer::OpenApplication(object_reg))
  {
    return 1;
  }

  // Create our main class.
  ViewMesh *app = new ViewMesh (object_reg, DefaultSkin);

  // Initialize the main system. This will load all needed plug-ins
  // (3D, 2D, network, sound, ...) and initialize them.
  if (!app->Initialize ())
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
	"crystalspace.application.viewmesh", "Error initializing system!");
    csInitializer::DestroyApplication(object_reg);
    return 1;
  }

  // Main loop.
  csDefaultRunLoop(object_reg);

  // Cleanup.
  delete app;
  csInitializer::DestroyApplication(object_reg);

  return 0;
}

