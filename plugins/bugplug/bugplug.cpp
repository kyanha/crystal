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

#include <string.h>

#include "cssys/sysfunc.h"
#include "csver.h"
#include "csutil/scf.h"
#include "csutil/scanstr.h"
#include "csutil/cscolor.h"
#include "csutil/debug.h"
#include "csutil/snprintf.h"
#include "csgeom/vector2.h"
#include "csgeom/vector3.h"
#include "csgeom/plane3.h"
#include "csgeom/box.h"
#include "bugplug.h"
#include "spider.h"
#include "shadow.h"
#include "iutil/plugin.h"
#include "iutil/vfs.h"
#include "iutil/string.h"
#include "iutil/event.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "iutil/eventq.h"
#include "iutil/objreg.h"
#include "iutil/dbghelp.h"
#include "iutil/virtclk.h"
#include "igraphic/image.h"
#include "igraphic/imageio.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "ivideo/txtmgr.h"
#include "ivideo/fontserv.h"
#include "ivideo/material.h"
#include "ivaria/conout.h"
#include "ivaria/reporter.h"
#include "iutil/object.h"
#include "imesh/object.h"
#include "imesh/terrfunc.h"
#include "imesh/thing/polygon.h"
#include "imesh/genmesh.h"
#include "iengine/engine.h"
#include "iengine/sector.h"
#include "iengine/viscull.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "iengine/camera.h"
#include "iengine/light.h"
#include "iengine/statlght.h"
#include "iengine/region.h"
#include "iengine/material.h"
#include "cstool/csview.h"
#include "qint.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (csBugPlug)


SCF_IMPLEMENT_IBASE (csBugPlug)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iBugPlug)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csBugPlug::BugPlug)
  SCF_IMPLEMENTS_INTERFACE (iBugPlug)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csBugPlug::EventHandler)
  SCF_IMPLEMENTS_INTERFACE (iEventHandler)
SCF_IMPLEMENT_IBASE_END

void csBugPlug::Report (int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csRef<iReporter> rep (CS_QUERY_REGISTRY (object_reg, iReporter));
  if (rep)
    rep->ReportV (severity, "crystalspace.bugplug", msg, arg);
  else
  {
    csPrintfV (msg, arg);
    csPrintf ("\n");
  }
  va_end (arg);
}

csBugPlug::csBugPlug (iBase *iParent)
{
  SCF_CONSTRUCT_IBASE (iParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiBugPlug);
  object_reg = 0;
  mappings = 0;
  visculler = 0;
  process_next_key = false;
  process_next_mouse = false;
  edit_mode = false;
  edit_string[0] = 0;
  initialized = false;
#ifndef CS_USE_NEW_RENDERER
  spider = new csSpider ();
#else
  spider = 0;
#endif
#ifndef CS_USE_NEW_RENDERER
  shadow = new csShadow ();
#else
  shadow = 0;
#endif
  selected_mesh = 0;
#ifndef CS_USE_NEW_RENDERER
  shadow->SetShadowMesh (selected_mesh);
#endif
  spider_hunting = false;
  prev_selected_mesh = 0;
  scfiEventHandler = 0;

  do_fps = true;
  fps_frame_count = 0;
  fps_tottime = 0;
  fps_cur = -1;
  counter_frames = 0;
  counter_freeze = false;

  debug_sector.sector = 0;
  debug_sector.view = 0;
  debug_sector.show = false;

  debug_view.show = false;
  debug_view.clear = true;
  debug_view.num_points = 0;
  debug_view.max_points = 0;
  debug_view.points = 0;
  debug_view.num_lines = 0;
  debug_view.max_lines = 0;
  debug_view.lines = 0;
  debug_view.num_boxes = 0;
  debug_view.max_boxes = 0;
  debug_view.boxes = 0;
  debug_view.object = 0;
  debug_view.drag_point = -1;

  captureFormat = 0;

  do_shadow_debug = false;
}

csBugPlug::~csBugPlug ()
{
  CleanDebugSector ();
  CleanDebugView ();

  if (selected_mesh) selected_mesh->DecRef ();
  if (spider)
  {
    if (Engine) spider->UnweaveWeb (Engine);
    delete spider;
  }
  if (shadow)
  {
    if (Engine) shadow->RemoveFromEngine (Engine);
    delete shadow;
  }
  while (mappings)
  {
    csKeyMap* n = mappings->next;
    delete[] mappings->args;
    delete mappings;
    mappings = n;
  }
  if (scfiEventHandler)
  {
    csRef<iEventQueue> q (CS_QUERY_REGISTRY (object_reg, iEventQueue));
    if (q)
      q->RemoveListener (scfiEventHandler);
    scfiEventHandler->DecRef ();
  }
  delete[] captureFormat;
}

bool csBugPlug::Initialize (iObjectRegistry *object_reg)
{
  csBugPlug::object_reg = object_reg;
  if (!scfiEventHandler)
  {
    scfiEventHandler = new EventHandler (this);
  }
  csRef<iEventQueue> q (CS_QUERY_REGISTRY (object_reg, iEventQueue));
  if (q != 0)
    q->RegisterListener (scfiEventHandler,
    	CSMASK_Nothing|CSMASK_KeyUp|CSMASK_KeyDown|
  	CSMASK_MouseUp|CSMASK_MouseDown|CSMASK_MouseMove);
  return true;
}

void csBugPlug::SetupPlugin ()
{
  if (initialized) return;

  if (!Engine) Engine = CS_QUERY_REGISTRY (object_reg, iEngine);

#ifdef CS_USE_NEW_RENDERER
  if (!G3D) G3D = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
#else
  if (!G3D) G3D = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
#endif

  if (!G3D)
  {
    initialized = true;
    printf ("No G3D!\n");
    return;
  }

  if (!G2D)
    G2D = G3D->GetDriver2D ();
  if (!G2D)
  {
    printf ("No G2D!\n");
    return;
  }

  if (!VFS) VFS = CS_QUERY_REGISTRY (object_reg, iVFS);
  if (!VFS)
  {
    printf ("No VFS!\n");
    return;
  }

  if (!vc) vc = CS_QUERY_REGISTRY (object_reg, iVirtualClock);
  if (!vc)
  {
    printf ("No vc!\n");
    return;
  }

  if (!Conout) Conout = CS_QUERY_REGISTRY (object_reg, iConsoleOutput);

  config.AddConfig (object_reg, "/config/bugplug.cfg");

  ReadKeyBindings (config->GetStr ("Bugplug.Keybindings", 
    "/config/bugplug.key"));

  captureFormat = csStrNew (config->GetStr ("Bugplug.Capture.FilenameFormat",
    "/this/cryst000.png"));
  // since this string is passed to format later,
  // replace all '%' with '%%'
  {
    char* newstr = new char[strlen(captureFormat)*2 + 1];
    memset (newstr, 0, strlen(captureFormat)*2 + 1);

    char* pos = captureFormat;
    while (pos)
    {
      char* percent = strchr (pos, '%');
      if (percent)
      {
	strncat (newstr, pos, percent-pos);
	strcat (newstr, "%%");
	pos = percent + 1;
      }
      else
      {
	strcat (newstr, pos);
	pos = 0;
      }
    }
    delete[] captureFormat;
    captureFormat = newstr;
  }
  // scan for the rightmost string of digits
  // and create an appropriate format string
  {
    captureFormatNumberMax = 1;
    int captureFormatNumberDigits = 0;

    char* end = strrchr (captureFormat, 0);
    if (end != captureFormat)
    {
      do
      {
	end--;
      }
      while ((end >= captureFormat) && (!isdigit (*end)));
      if (end >= captureFormat)
      {
	do
	{
	  captureFormatNumberMax *= 10;
	  captureFormatNumberDigits++; 
	  end--;
	}
	while ((end >= captureFormat) && (isdigit (*end)));
	
	char nameForm [6];
	cs_snprintf (nameForm, 6, "%%0%dd", captureFormatNumberDigits);

	int newlen = strlen(captureFormat)+strlen(nameForm)-
	  captureFormatNumberDigits+1;
	char* newCapForm = new char[newlen];
	memset (newCapForm, 0, newlen);
	strncpy (newCapForm, captureFormat, end-captureFormat+1);
	strcat (newCapForm, nameForm);
	strcat (newCapForm, end+captureFormatNumberDigits+1);
	delete[] captureFormat;
	captureFormat = newCapForm;
      }
    }
  }

  captureMIME = config->GetStr ("Bugplug.Capture.Image.MIME",
    "image/png");

  captureOptions = config->GetStr ("Bugplug.Capture.Image.Options");

  initialized = true;

  Report (CS_REPORTER_SEVERITY_NOTIFY, "BugPlug loaded...");

  do_clear = false;
}

void csBugPlug::UnleashSpider (int cmd)
{
  if (Engine)
  {
    spider->ClearCamera ();
    if (spider->WeaveWeb (Engine))
    {
      spider_command = cmd;
      spider_hunting = true;
      spider_timeout = 20;
    }
    else
    {
      Report (CS_REPORTER_SEVERITY_NOTIFY,
	"Spider could not weave its web (No sectors)!");
    }
  }
  else
  {
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Spider could not weave its web (No engine)!");
  }
}

void csBugPlug::VisculCmd (const char* cmd)
{
  if (!visculler)
  {
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Bugplug is currently now tracking a visibility culler!");
    return;
  }
  csRef<iDebugHelper> dbghelp (SCF_QUERY_INTERFACE (visculler, iDebugHelper));
  if (!dbghelp)
  {
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "This visibility culler does not support iDebugHelper!");
    return;
  }
  if (dbghelp->DebugCommand (cmd))
  {
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Viscul command '%s' performed.", cmd);
  }
  else
  {
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Viscul command '%s' not supported!", cmd);
  }
}

void csBugPlug::VisculView (iCamera* camera)
{
  if (visculler)
  {
    visculler = 0;
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Disabled visculler graphical dumping");
    return;
  }

  // If we are not tracking a visculler yet we try to find one in current
  // sector.
  iSector* sector = camera->GetSector ();
  visculler = sector->GetVisibilityCuller ();
  if (!visculler)
  {
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Bugplug found no visibility culler in this sector!");
    return;
  }
  Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Bugplug is now tracking a visibility culler");
}

void csBugPlug::HideSpider (iCamera* camera)
{
  spider_hunting = false;
  spider->UnweaveWeb (Engine);
  if (camera)
  {
    char buf[80];
    Report (CS_REPORTER_SEVERITY_NOTIFY, "Spider caught a camera!");
    switch (spider_command)
    {
      case DEBUGCMD_DUMPCAM:
	Dump (camera);
	break;
      case DEBUGCMD_FOV:
	{
          int fov = camera->GetFOV ();
	  sprintf (buf, "%d", fov);
	  EnterEditMode (spider_command, "Enter new fov value:", buf);
	}
	break;
      case DEBUGCMD_FOVANGLE:
	{
          float fov = camera->GetFOVAngle ();
	  sprintf (buf, "%g", fov);
	  EnterEditMode (spider_command, "Enter new fov angle:", buf);
	}
	break;
      case DEBUGCMD_SCRSHOT:
        CaptureScreen ();
	break;
      case DEBUGCMD_DEBUGSECTOR:
	SwitchDebugSector (camera->GetTransform ());
        break;
      case DEBUGCMD_MOUSE1:
        MouseButton3 (camera); //@@@ Temp hack to make bugplug cross platform.
	break;
      case DEBUGCMD_MOUSE2:
        MouseButton2 (camera);
	break;
      case DEBUGCMD_MOUSE3:
        MouseButton3 (camera);
	break;
      case DEBUGCMD_VISCULVIEW:
        VisculView (camera);
        break;
    }
  }
}

#ifndef CS_USE_NEW_RENDERER
void csBugPlug::ToggleG3DState (G3D_RENDERSTATEOPTION op, const char* name)
{
  if (!G3D) return;
  bool v;
  v = G3D->GetRenderState (op);
  v = !v;
  if (G3D->SetRenderState (op, v))
  {
    Report (CS_REPORTER_SEVERITY_NOTIFY, "BugPlug %s %s.",
	v ? "enabled" : "disabled", name);
  }
  else
  {
    Report (CS_REPORTER_SEVERITY_NOTIFY, "%s not supported for this renderer!",
    	name);
  }
}
#else
void csBugPlug::ToggleR3DState (R3D_RENDERSTATEOPTION op, const char* name)
{
  if (!G3D) return;
  bool v;
  v = G3D->GetRenderState (op);
  v = !v;
  if (G3D->SetRenderState (op, v))
  {
    Report (CS_REPORTER_SEVERITY_NOTIFY, "BugPlug %s %s.",
	v ? "enabled" : "disabled", name);
  }
  else
  {
    Report (CS_REPORTER_SEVERITY_NOTIFY, "%s not supported for this renderer!",
    	name);
  }
}
#endif

void csBugPlug::MouseButton1 (iCamera*)
{
}

void csBugPlug::MouseButton2 (iCamera*)
{
}

void csBugPlug::MouseButton3 (iCamera* camera)
{
  csVector3 v;
  // Setup perspective vertex, invert mouse Y axis.
  csVector2 p (mouse_x, camera->GetShiftY() * 2 - mouse_y);

  camera->InvPerspective (p, 100, v);
  csVector3 end = camera->GetTransform ().This2Other (v);

  iSector* sector = camera->GetSector ();
  csVector3 origin = camera->GetTransform ().GetO2TTranslation ();
  csVector3 isect;

  iPolygon3D* poly = 0;
  iMeshWrapper* sel = sector->HitBeam (origin, end, isect, &poly);
  const char* poly_name;
  if (poly)
  {
    poly_name = poly->GetStaticData ()->GetName ();
    Dump (poly);
  }
  else
  {
    poly_name = 0;
  }

  csVector3 vw = isect;
  v = camera->GetTransform ().Other2This (vw);
  Report (CS_REPORTER_SEVERITY_NOTIFY,
    "LMB down : c:(%f,%f,%f) w:(%f,%f,%f) p:'%s'",
    v.x, v.y, v.z, vw.x, vw.y, vw.z, poly_name ? poly_name : "<none>");

  if (sel)
  {
    // First release the ref to the previous selected_mesh.
    if (selected_mesh) selected_mesh->DecRef ();

    // Here we incref our mesh.
    sel->IncRef ();
    // But of course we have to make sure that it will actually get
    // deleted when the app/engine wants it deleted. So BugPlug will
    // monitor the ref count of this mesh and decrease its own reference
    // as soon as the ref count reaches one.
    selected_mesh = sel;
    const char* n = selected_mesh->QueryObject ()->GetName ();
    Report (CS_REPORTER_SEVERITY_NOTIFY, "BugPlug found mesh '%s'!",
      	n ? n : "<noname>");
    bool bbox, rad, bm;
    shadow->GetShowOptions (bbox, rad, bm);
    shadow->SetShadowMesh (selected_mesh);

    shadow->SetBeam (origin, end, isect);
    if (bbox || rad || bm)
      shadow->AddToEngine (Engine);
    else
      shadow->RemoveFromEngine (Engine);
  }
}

bool csBugPlug::EatMouse (iEvent& event)
{
  SetupPlugin ();
  if (!process_next_mouse && !debug_view.show) return false;

  bool down = (event.Type == csevMouseDown);
  bool up = (event.Type == csevMouseUp);
  int button = event.Mouse.Button;

  mouse_x = event.Mouse.x;
  mouse_y = event.Mouse.y;

  if (down)
  {
    if (debug_view.show)
    {
      int i;
      debug_view.drag_point = -1;
      for (i = 0 ; i < debug_view.num_points ; i++)
      {
        int x = int (debug_view.points[i].x);
        int y = int (debug_view.points[i].y);
	if (ABS (mouse_x-x) < 4 && ABS ((mouse_y)-y) < 4)
	{
	  debug_view.drag_point = i;
	  break;
	}
      }
    }
    else
    {
      UnleashSpider (DEBUGCMD_MOUSE1+button-1);
      process_next_mouse = false;
    }
  }
  else if (up)
  {
    debug_view.drag_point = -1;
  }
  else
  {
    if (debug_view.show && debug_view.drag_point != -1)
    {
      debug_view.points[debug_view.drag_point].x = mouse_x;
      debug_view.points[debug_view.drag_point].y = mouse_y;
    }
  }
  return true;
}

void csBugPlug::CaptureScreen ()
{
  int i = 0;
  char name[CS_MAXPATHLEN];

  bool exists = false;
  do
  {
    cs_snprintf (name, CS_MAXPATHLEN, captureFormat, i);
    if (exists = VFS->Exists (name)) i++;
  }
  while ((i < captureFormatNumberMax) && (exists));

  if (i >= captureFormatNumberMax)
  {
    Report (CS_REPORTER_SEVERITY_NOTIFY,
    	"Too many screenshot files in current directory");
    return;
  }

  csRef<iImage> img (csPtr<iImage> (G2D->ScreenShot ()));
  if (!img)
  {
    Report (CS_REPORTER_SEVERITY_NOTIFY,
    	"The 2D graphics driver does not support screen shots");
    return;
  }
  csRef<iImageIO> imageio (CS_QUERY_REGISTRY (object_reg, iImageIO));
  if (imageio)
  {
    csRef<iDataBuffer> db (imageio->Save (img, captureMIME, 
      captureOptions));
    if (db)
    {
      Report (CS_REPORTER_SEVERITY_NOTIFY, "Screenshot: %s", name);
      if (!VFS->WriteFile (name, (const char*)db->GetData (),
      		db->GetSize ()))
      {
        Report (CS_REPORTER_SEVERITY_NOTIFY,
		"There was an error while writing screen shot");
      }
    }
    else
    {
      Report (CS_REPORTER_SEVERITY_NOTIFY, 
	      "Could not encode screen shot");
    }
  }
}

bool csBugPlug::EatKey (iEvent& event)
{
  SetupPlugin ();
  int key = event.Key.Code;
  bool down = (event.Type == csevKeyDown);
  bool shift = (event.Key.Modifiers & CSMASK_SHIFT) != 0;
  bool alt = (event.Key.Modifiers & CSMASK_ALT) != 0;
  bool ctrl = (event.Key.Modifiers & CSMASK_CTRL) != 0;

  // If we are in edit mode we do special processing.
  if (edit_mode)
  {
    if (down)
    {
      int l = strlen (edit_string);
      if (key == '\n')
      {
        // Exit edit_mode.
	edit_mode = false;
	ExitEditMode ();
      }
      else if (key == '\b')
      {
        // Backspace.
	if (edit_cursor > 0)
	{
	  edit_cursor--;
	  edit_string[edit_cursor] = 0;
	}
      }
      else if (key == CSKEY_DEL)
      {
        // Delete.
	int i;
	for (i = edit_cursor ; i < 79 ; i++)
	  edit_string[i] = edit_string[i+1];
      }
      else if (key == CSKEY_HOME)
      {
        edit_cursor = 0;
      }
      else if (key == CSKEY_END)
      {
        edit_cursor = l;
      }
      else if (key == CSKEY_LEFT)
      {
        if (edit_cursor > 0) edit_cursor--;
      }
      else if (key == CSKEY_RIGHT)
      {
        if (edit_cursor < l) edit_cursor++;
      }
      else if (key == CSKEY_ESC)
      {
        // Cancel.
	edit_string[0] = 0;
	edit_mode = false;
      }
      else if (edit_cursor < 79)
      {
        int i;
	for (i = 78 ; i >= edit_cursor ; i--)
	  edit_string[i+1] = edit_string[i];
        edit_string[edit_cursor] = key;
	edit_cursor++;
      }
    }
    return true;
  }

  // Get command.
  char* args;
  int cmd = GetCommandCode (key, shift, alt, ctrl, args);
  if (down)
  {
    // First we check if it is the 'debug enter' key.
    if (cmd == DEBUGCMD_DEBUGENTER)
    {
      process_next_key = !process_next_key;
      if (process_next_key)
      {
        Report (CS_REPORTER_SEVERITY_NOTIFY, "Press debug key...");
      }
      else
      {
        Report (CS_REPORTER_SEVERITY_NOTIFY, "Back to normal key processing.");
      }
      return true;
    }
    if (cmd == DEBUGCMD_MOUSEENTER)
    {
      process_next_mouse = !process_next_mouse;
      if (process_next_mouse)
      {
        Report (CS_REPORTER_SEVERITY_NOTIFY, "Click on screen...");
      }
      return true;
    }
  }

  // Return false if we are not processing our own keys.
  // If debug_sector.show is true we will process our own keys to...
  if (!process_next_key && !debug_sector.show && !debug_view.show)
    return false;
  if (debug_sector.show || debug_view.show)
  {
    process_next_key = false;
  }
  if (down)
  {
    char buf[100];
    switch (cmd)
    {
      case DEBUGCMD_UNKNOWN:
        return true;
      case DEBUGCMD_QUIT:
        Report (CS_REPORTER_SEVERITY_NOTIFY, "Nah nah! I will NOT quit!");
        break;
      case DEBUGCMD_STATUS:
        Report (CS_REPORTER_SEVERITY_NOTIFY,
		"I'm running smoothly, thank you...");
        break;
      case DEBUGCMD_DEBUGGRAPH:
        Report (CS_REPORTER_SEVERITY_NOTIFY, "Debug graph dumped!");
	csDebuggingGraph::Dump (object_reg);
        break;
      case DEBUGCMD_ENGINECMD:
	{
	  csRef<iDebugHelper> dbghelp (
	  	SCF_QUERY_INTERFACE (Engine, iDebugHelper));
	  if (dbghelp)
	  {
	    if (dbghelp->DebugCommand (args))
	    {
              Report (CS_REPORTER_SEVERITY_NOTIFY,
	        "Engine command '%s' performed.", args);
	    }
	    else
	    {
              Report (CS_REPORTER_SEVERITY_NOTIFY,
	        "Engine command '%s' not supported!", args);
	    }
	  }
	}
        break;
      case DEBUGCMD_VISCULCMD:
        VisculCmd (args);
        break;
      case DEBUGCMD_DEBUGCMD:
	{
	  DebugCmd (args);
	  break;
	}
      case DEBUGCMD_ENGINESTATE:
	{
	  csRef<iDebugHelper> dbghelp (
	  	SCF_QUERY_INTERFACE (Engine, iDebugHelper));
	  if (dbghelp)
	  {
	    if (dbghelp->GetSupportedTests () & CS_DBGHELP_STATETEST)
	    {
	      csRef<iString> rc (dbghelp->StateTest ());
	      if (rc)
	      {
                Report (CS_REPORTER_SEVERITY_NOTIFY,
	          "Engine StateTest() failed:");
                Report (CS_REPORTER_SEVERITY_DEBUG,
	          "Engine StateTest() failed:");
                Report (CS_REPORTER_SEVERITY_DEBUG,
		  rc->GetData ());
	      }
	      else
	      {
                Report (CS_REPORTER_SEVERITY_NOTIFY,
	          "Engine StateTest() succeeded!");
	      }
	    }
	    else
	    {
              Report (CS_REPORTER_SEVERITY_NOTIFY,
	        "Engine doesn't support StateTest()!");
	    }
	  }
	}
        break;
      case DEBUGCMD_HELP:
        Report (CS_REPORTER_SEVERITY_NOTIFY, "Sorry, cannot help you yet.");
        break;
      case DEBUGCMD_DUMPENG:
        if (Engine)
	{
          Report (CS_REPORTER_SEVERITY_NOTIFY,
		"Dumping entire engine contents to debug.txt.");
	  Dump (Engine);
	  Report (CS_REPORTER_SEVERITY_DEBUG, "");
	}
        break;
      case DEBUGCMD_DUMPSEC:
        Report (CS_REPORTER_SEVERITY_NOTIFY, "Not implemented yet.");
        break;
      case DEBUGCMD_CLEAR:
        do_clear = !do_clear;
        Report (CS_REPORTER_SEVERITY_NOTIFY, "BugPlug %s screen clearing.",
	  	do_clear ? "enabled" : "disabled");
        break;
#ifndef CS_USE_NEW_RENDERER
      case DEBUGCMD_EDGES:
        ToggleG3DState (G3DRENDERSTATE_EDGES, "edge drawing");
        break;
      case DEBUGCMD_TEXTURE:
        ToggleG3DState (G3DRENDERSTATE_TEXTUREMAPPINGENABLE, "texture mapping");
        break;
      case DEBUGCMD_BILINEAR:
        ToggleG3DState (G3DRENDERSTATE_BILINEARMAPPINGENABLE,
		"bi-linear filtering");
	break;
      case DEBUGCMD_TRILINEAR:
        ToggleG3DState (G3DRENDERSTATE_TRILINEARMAPPINGENABLE,
		"tri-linear filtering");
	break;
      case DEBUGCMD_LIGHTING:
        ToggleG3DState (G3DRENDERSTATE_LIGHTINGENABLE, "lighting");
	break;
      case DEBUGCMD_GOURAUD:
        ToggleG3DState (G3DRENDERSTATE_GOURAUDENABLE, "gouraud shading");
	break;
      case DEBUGCMD_ILACE:
        ToggleG3DState (G3DRENDERSTATE_INTERLACINGENABLE, "interlaced mode");
	break;
      case DEBUGCMD_MMX:
        ToggleG3DState (G3DRENDERSTATE_MMXENABLE, "mmx mode");
	break;
      case DEBUGCMD_TRANSP:
        ToggleG3DState (G3DRENDERSTATE_TRANSPARENCYENABLE, "transp mode");
        break;
      case DEBUGCMD_CACHECLEAR:
        if (G3D)
	{
	  G3D->ClearCache ();
          Report (CS_REPORTER_SEVERITY_NOTIFY,
	    "BugPlug cleared the texture cache.");
	}
        break;
      case DEBUGCMD_CACHEDUMP:
        if (G3D) G3D->DumpCache ();
        break;
      case DEBUGCMD_MIPMAP:
        {
	  if (!G3D) break;
	  char* choices[6] = { "on", "off", "1", "2", "3", 0 };
	  long v = G3D->GetRenderState (G3DRENDERSTATE_MIPMAPENABLE);
	  v = (v+1)%5;
	  G3D->SetRenderState (G3DRENDERSTATE_MIPMAPENABLE, v);
	  Report (CS_REPORTER_SEVERITY_NOTIFY, "BugPlug set mipmap to '%s'",
	  	choices[v]);
  	}
	break;
      case DEBUGCMD_INTER:
	{
	  if (!G3D) break;
	  char* choices[5] = { "smart", "step32", "step16", "step8", 0 };
	  long v = G3D->GetRenderState (G3DRENDERSTATE_INTERPOLATIONSTEP);
	  v = (v+1)%4;
	  G3D->SetRenderState (G3DRENDERSTATE_INTERPOLATIONSTEP, v);
	  Report (CS_REPORTER_SEVERITY_NOTIFY, "BugPlug set interpolation to '%s'",
	  	choices[v]);
	}
	break;
      case DEBUGCMD_GAMMA:
        {
	  if (!G3D) break;
	  float val = G2D->GetGamma ();
	  sprintf (buf, "%g", val);
          EnterEditMode (cmd, "Enter new gamma:", buf);
	}
        break;
#endif
      case DEBUGCMD_DBLBUFF:
        {
	  bool state = G2D->GetDoubleBufferState ();
	  state = !state;
	  if (!G2D->DoubleBuffer (state))
	  {
	    Report (CS_REPORTER_SEVERITY_NOTIFY,
	    	"Double buffer not supported in current video mode!");
	  }
	  else
	  {
	    Report (CS_REPORTER_SEVERITY_NOTIFY,
	    	"BugPlug %s double buffering.",
		state ? "enabled" : "disabled");
	  }
	}
        break;
      case DEBUGCMD_TERRVIS:
        if (Engine)
	{
	  int enable_disable = -1;
	  int i, j;
	  for (i = 0 ; i < Engine->GetSectors ()->GetCount () ; i++)
	  {
	    iSector* sector = Engine->GetSectors ()->Get (i);
	    iMeshList* ml = sector->GetMeshes ();
	    for (j = 0 ; j < ml->GetCount () ; j++)
	    {
	      iMeshWrapper* terr = ml->Get (j);
	      csRef<iTerrFuncState> st (
	      	SCF_QUERY_INTERFACE (terr->GetMeshObject (),
	      	iTerrFuncState));
	      if (st)
	      {
	        if (enable_disable == -1)
		{
		  enable_disable = (int) (!st->IsVisTestingEnabled ());
		}
		st->SetVisTesting ((bool) enable_disable);
	      }
	    }
	  }
	  if (enable_disable == -1)
	  {
	    Report (CS_REPORTER_SEVERITY_NOTIFY,
	      "BugPlug found no terrains to work with!");
	  }
	  else
	  {
	    Report (CS_REPORTER_SEVERITY_NOTIFY,
	      "BugPlug %s terrain visibility checking!",
	      enable_disable ? "enabled" : "disabled");
	  }
	}
	else
	{
	  Report (CS_REPORTER_SEVERITY_NOTIFY,
	    	"BugPlug has no engine to work on!");
	}
        break;
      case DEBUGCMD_MESHBBOX:
	{
	  bool bbox, rad, bm;
	  shadow->GetShowOptions (bbox, rad, bm);
          bbox = !bbox;
	  Report (CS_REPORTER_SEVERITY_NOTIFY,
	    	"BugPlug %s bounding box display.",
		bbox ? "enabled" : "disabled");
	  shadow->SetShowOptions (bbox, rad, bm);
	  if ((bbox || rad || bm) && selected_mesh)
	    shadow->AddToEngine (Engine);
	  else
	    shadow->RemoveFromEngine (Engine);
	}
        break;
      case DEBUGCMD_MESHRAD:
        {
	  bool bbox, rad, bm;
	  shadow->GetShowOptions (bbox, rad, bm);
          rad = !rad;
	  Report (CS_REPORTER_SEVERITY_NOTIFY,
	    	"BugPlug %s bounding sphere display.",
		rad ? "enabled" : "disabled");
	  shadow->SetShowOptions (bbox, rad, bm);
	  if ((bbox || rad || bm) && selected_mesh)
	    shadow->AddToEngine (Engine);
	  else
	    shadow->RemoveFromEngine (Engine);
	}
        break;
      case DEBUGCMD_DEBUGVIEW:
	SwitchDebugView ();
        break;
      case DEBUGCMD_VISCULVIEW:
      case DEBUGCMD_DUMPCAM:
      case DEBUGCMD_FOV:
      case DEBUGCMD_FOVANGLE:
      case DEBUGCMD_DEBUGSECTOR:
      case DEBUGCMD_SCRSHOT:
        // Set spider on a hunt.
	UnleashSpider (cmd);
        break;
      case DEBUGCMD_DS_LEFT:
        if (debug_sector.show)
	{
	  debug_sector.view->GetCamera ()->Move (csVector3 (-1, 0, 0), false);
	}
	else
	{
	  Report (CS_REPORTER_SEVERITY_NOTIFY,
	    	"Debug sector is not active now!");
	}
        break;
      case DEBUGCMD_DS_RIGHT:
        if (debug_sector.show)
	{
	  debug_sector.view->GetCamera ()->Move (csVector3 (1, 0, 0), false);
	}
	else
	{
	  Report (CS_REPORTER_SEVERITY_NOTIFY,
	    	"Debug sector is not active now!");
	}
        break;
      case DEBUGCMD_DS_FORWARD:
        if (debug_sector.show)
	{
	  debug_sector.view->GetCamera ()->Move (csVector3 (0, 0, 1), false);
	}
	else
	{
	  Report (CS_REPORTER_SEVERITY_NOTIFY,
	    	"Debug sector is not active now!");
	}
        break;
      case DEBUGCMD_DS_BACKWARD:
        if (debug_sector.show)
	{
	  debug_sector.view->GetCamera ()->Move (csVector3 (0, 0, -1), false);
	}
	else
	{
	  Report (CS_REPORTER_SEVERITY_NOTIFY,
	    	"Debug sector is not active now!");
	}
        break;
      case DEBUGCMD_DS_UP:
        if (debug_sector.show)
	{
	  debug_sector.view->GetCamera ()->Move (csVector3 (0, 1, 0), false);
	}
	else
	{
	  Report (CS_REPORTER_SEVERITY_NOTIFY,
	    	"Debug sector is not active now!");
	}
        break;
      case DEBUGCMD_DS_DOWN:
        if (debug_sector.show)
	{
	  debug_sector.view->GetCamera ()->Move (csVector3 (0, -1, 0), false);
	}
	else
	{
	  Report (CS_REPORTER_SEVERITY_NOTIFY,
	    	"Debug sector is not active now!");
	}
        break;
      case DEBUGCMD_DS_TURNLEFT:
        if (debug_sector.show)
	{
	  debug_sector.view->GetCamera ()->GetTransform ().
	  	RotateThis (CS_VEC_ROT_LEFT, .2);
	}
	else
	{
	  Report (CS_REPORTER_SEVERITY_NOTIFY,
	    	"Debug sector is not active now!");
	}
        break;
      case DEBUGCMD_DS_TURNRIGHT:
        if (debug_sector.show)
	{
	  debug_sector.view->GetCamera ()->GetTransform ().
	  	RotateThis (CS_VEC_ROT_RIGHT, .2);
	}
	else
	{
	  Report (CS_REPORTER_SEVERITY_NOTIFY,
	    	"Debug sector is not active now!");
	}
        break;
      case DEBUGCMD_FPS:
        do_fps = !do_fps;
	Report (CS_REPORTER_SEVERITY_NOTIFY,
	    	"BugPlug %s fps display.",
		do_fps ? "enabled" : "disabled");
	fps_frame_count = 0;
	fps_tottime = 0;
	fps_cur = -1;
        break;
      case DEBUGCMD_HIDESELECTED:
        if (selected_mesh)
	{
	  iMovable* m = selected_mesh->GetMovable ();
	  iSectorList* sl = m->GetSectors ();
	  int i;
	  for (i = 0 ; i < sl->GetCount () ; i++)
	    mesh_sectors[i] = sl->Get (i);
	  mesh_num_sectors = sl->GetCount ();
	  m->ClearSectors ();
	  m->UpdateMove ();
	  prev_selected_mesh = selected_mesh;
	  selected_mesh = 0;
	}
	else
	{
	  Report (CS_REPORTER_SEVERITY_NOTIFY,
	    	"There is no selected mesh to hide!");
	}
        break;
      case DEBUGCMD_UNDOHIDE:
        if (prev_selected_mesh)
	{
	  iMovable* m = prev_selected_mesh->GetMovable ();
	  iSectorList* sl = m->GetSectors ();
	  int i;
	  for (i = 0 ; i < mesh_num_sectors ; i++)
	    sl->Add (mesh_sectors[i]);
	  m->UpdateMove ();
	  prev_selected_mesh = 0;
	}
        break;
      case DEBUGCMD_COUNTERRESET:
        FullResetCounters ();
	break;
      case DEBUGCMD_COUNTERREMOVE:
        counters.DeleteAll ();
	break;
      case DEBUGCMD_COUNTERFREEZE:
        counter_freeze = !counter_freeze;
	Report (CS_REPORTER_SEVERITY_NOTIFY,
	    	"BugPlug %s counting.",
		counter_freeze ? "disabled" : "enabled");
	break;
#ifdef CS_USE_NEW_RENDERER
      case DEBUGCMD_SHADOWDEBUG:
	// swap the default shadow volume material shader to/from a version
	// better visualizing the volume.
	/*csRef<iMaterialWrapper> shadowmat = 
	  Engine->FindMaterial ("shadow extruder");
	if (!standardShadowShader)
	  standardShadowShader = shadowmat->GetMaterial()->GetShader();
	if (!debugShadowShader)
	{
	  csRef<iShaderManager> shmgr ( CS_QUERY_REGISTRY(object_reg, iShaderManager));
	  if(shmgr)
	  {
	    debugShadowShader = shmgr->CreateShader();
	    if(debugShadowShader)
	    {
	      debugShadowShader->Load (csRef<iDataBuffer> 
		(VFS->ReadFile("/shader/shadowdebug.xml")));
	      if(!debugShadowShader->Prepare())
	      {
		debugShadowShader = 0;
  		return true;
	      }
	    }
	    else
	      return true;
	  }
	  else
	    return true;
	}
	do_shadow_debug = !do_shadow_debug;
	if (do_shadow_debug)
	{
	  shadowmat->GetMaterial ()->SetShader(debugShadowShader);
	}
	else
	{
	  shadowmat->GetMaterial ()->SetShader(standardShadowShader);
	}
	Report (CS_REPORTER_SEVERITY_NOTIFY,
	    "BugPlug %s shadow debugging.",
	    do_shadow_debug ? "enabled" : "disabled");*/
        break;
#endif
    }
    process_next_key = false;
  }
  return true;
}

bool csBugPlug::HandleStartFrame (iEvent& /*event*/)
{
  SetupPlugin ();
  if (!G3D) return false;

  if (do_clear)
  {
    G3D->BeginDraw (CSDRAW_2DGRAPHICS);
    int bgcolor_clear = G2D->FindRGB (0, 255, 255);
    G2D->Clear (bgcolor_clear);
  }
  if (selected_mesh)
  {
    // Here we see if we have the last ref count to the selected mesh.
    // In that case we also release it.
    if (selected_mesh->GetRefCount () == 1)
    {
      shadow->SetShadowMesh (0);
      shadow->RemoveFromEngine (Engine);
      selected_mesh->DecRef ();
      selected_mesh = 0;
      Report (CS_REPORTER_SEVERITY_NOTIFY, "Selected mesh is deleted!");
    }
  }
  return false;
}

void GfxWrite (iGraphics2D* g2d, iFont* font,
	int x, int y, int fg, int bg, char *str, ...)
{
  va_list arg;
  char buf[256];

  va_start (arg, str);
  vsprintf (buf, str, arg);
  va_end (arg);

  g2d->Write (font, x, y, fg, bg, buf);
}

static inline void BugplugBox (iGraphics2D* G2D, 
			       int x, int y, int  w, int h)
{
  int bgcolor = G2D->FindRGB (255, 255, 192);
  G2D->DrawBox (x, y, w, h, bgcolor);
  int bordercolor = G2D->FindRGB (192, 192, 64);
  G2D->DrawLine (x, y, x+w, y, bordercolor);
  G2D->DrawLine (x+w, y, x+w, y+h, bordercolor);
  G2D->DrawLine (x+w, y+h, x, y+h, bordercolor);
  G2D->DrawLine (x, y+h, x, y, bordercolor);
}

bool csBugPlug::HandleEndFrame (iEvent& /*event*/)
{
  SetupPlugin ();
  if (!G3D) return false;

  if (do_fps)
  {
    csTicks elapsed_time = vc->GetElapsedTicks ();
    fps_tottime += elapsed_time;
    fps_frame_count++;
    if (fps_tottime > 500)
    {
      fps_cur = (float (fps_frame_count) * 1000.0) / float (fps_tottime);
      fps_frame_count = 0;
      fps_tottime = 0;
    }
  }

  if (visculler)
  {
    csRef<iDebugHelper> dbghelp (SCF_QUERY_INTERFACE (visculler, iDebugHelper));
#ifndef CS_USE_NEW_RENDERER   
    if (dbghelp)
      dbghelp->Dump (G3D);
#endif
  }

  if (debug_sector.show)
  {
    G3D->BeginDraw (CSDRAW_3DGRAPHICS |
    	CSDRAW_CLEARZBUFFER | CSDRAW_CLEARSCREEN);
    debug_sector.view->Draw ();
  }

  if (debug_view.show)
  {
    G3D->BeginDraw (CSDRAW_2DGRAPHICS |
    	(debug_view.clear ? CSDRAW_CLEARSCREEN : 0));
    if (debug_view.object)
      debug_view.object->Render (G3D, &scfiBugPlug);
    int pointcol = G2D->FindRGB (255, 255, 0);
    int linecol = G2D->FindRGB (255, 255, 255);
    int i;
    for (i = 0 ; i < debug_view.num_lines ; i++)
    {
      int i1 = debug_view.lines[i].i1;
      int i2 = debug_view.lines[i].i2;
      G2D->DrawLine (
      	debug_view.points[i1].x, debug_view.points[i1].y,
      	debug_view.points[i2].x, debug_view.points[i2].y, linecol);
    }
    for (i = 0 ; i < debug_view.num_boxes ; i++)
    {
      int i1 = debug_view.boxes[i].i1;
      int i2 = debug_view.boxes[i].i2;
      float x1 = debug_view.points[i1].x;
      float y1 = debug_view.points[i1].y;
      float x2 = debug_view.points[i2].x;
      float y2 = debug_view.points[i2].y;
      G2D->DrawLine (x1, y1, x2, y1, linecol);
      G2D->DrawLine (x2, y1, x2, y2, linecol);
      G2D->DrawLine (x2, y2, x1, y2, linecol);
      G2D->DrawLine (x1, y2, x1, y1, linecol);
    }
    for (i = 0 ; i < debug_view.num_points ; i++)
    {
      float x = debug_view.points[i].x;
      float y = debug_view.points[i].y;
      G2D->DrawLine (x-5, y-5, x+5, y+5, pointcol);
      G2D->DrawLine (x-5, y+5, x+5, y-5, pointcol);
    }
  }

  if (process_next_key || process_next_mouse)
  {
    G3D->BeginDraw (CSDRAW_2DGRAPHICS);
    iFontServer* fntsvr = G2D->GetFontServer ();
    if (fntsvr)
    {
      csRef<iFont> fnt (fntsvr->GetFont (0));
      if (fnt == 0)
      {
        fnt = fntsvr->LoadFont (CSFONT_COURIER);
      }
      CS_ASSERT (fnt != 0);
      int fw, fh;
      fnt->GetMaxSize (fw, fh);
      int sh = G2D->GetHeight ();
      int x = 150;
      int y = sh/2 - (fh+5*2)/2;
      int w = 200;
      int h = fh+5*2;
      BugplugBox (G2D, x, y, w, h);
      int fgcolor = G2D->FindRGB (0, 0, 0);
      char* msg;
      if (process_next_key) msg = "Press a BugPlug key...";
      else msg = "Click on screen...";
      G2D->Write (fnt, x+5, y+5, fgcolor, -1, msg);
    }
  }

  if (edit_mode)
  {
    G3D->BeginDraw (CSDRAW_2DGRAPHICS);
    iFontServer* fntsvr = G2D->GetFontServer ();
    if (fntsvr)
    {
      csRef<iFont> fnt (fntsvr->GetFont (0));
      if (fnt == 0)
      {
        fnt = fntsvr->LoadFont (CSFONT_COURIER);
      }
      CS_ASSERT (fnt != 0);
      int fw, fh;
      fnt->GetMaxSize (fw, fh);
      int sw = G2D->GetWidth ();
      int sh = G2D->GetHeight ();
      int x = 10;
      int y = sh/2 - (fh*2+5*3)/2;
      int w = sw-20;
      int h = fh*2+5*3;
      BugplugBox (G2D, x, y, w, h);
      int fgcolor = G2D->FindRGB (0, 0, 0);
      int maxlen = fnt->GetLength (msg_string, w-10);
      if (maxlen < 80) msg_string[maxlen] = 0;
      G2D->Write (fnt, x+5, y+5, fgcolor, -1, msg_string);
      maxlen = fnt->GetLength (edit_string, w-10);
      if (maxlen < 80) edit_string[maxlen] = 0;
      G2D->Write (fnt, x+5, y+5+fh+5, fgcolor, -1, edit_string);
      char cursor[83];
      strcpy (cursor, edit_string);
      cursor[edit_cursor] = 0;
      int cursor_w, cursor_h;
      fnt->GetDimensions (cursor, cursor_w, cursor_h);
      G2D->Write (fnt, x+5+cursor_w, y+5+fh+7, fgcolor, -1, "_");
    }
  }

  if (do_fps)
  {
    G3D->BeginDraw (CSDRAW_2DGRAPHICS);
    iFontServer* fntsvr = G2D->GetFontServer ();
    if (fntsvr)
    {
      csRef<iFont> fnt (fntsvr->GetFont (0));
      if (fnt == 0)
      {
        fnt = fntsvr->LoadFont (CSFONT_COURIER);
      }
      int sh = G2D->GetHeight ();
      int fw, fh;
      fnt->GetMaxSize (fw, fh);
      int fgcolor = G2D->FindRGB (255, 255, 255);
      GfxWrite (G2D, fnt, 11, sh - fh - 3, 0, -1, "FPS=%.2f", fps_cur);
      GfxWrite (G2D, fnt, 10, sh - fh - 2, fgcolor, -1, "FPS=%.2f", fps_cur);
    }
    G3D->FinishDraw ();
  }

  ShowCounters ();

  if (spider_hunting)
  {
    iCamera* camera = spider->GetCamera ();
    if (camera)
    {
      HideSpider (camera);
    }
    else
    {
      spider_timeout--;
      if (spider_timeout < 0)
      {
	HideSpider (0);
        Report (CS_REPORTER_SEVERITY_NOTIFY,
		"Spider could not catch a camera!");
      }
    }
  }

  return false;
}

void csBugPlug::EnterEditMode (int cmd, const char* msg, const char* def)
{
  if (edit_mode) return;
  iFontServer* fntsvr = G2D->GetFontServer ();
  if (!fntsvr) return;	// No edit mode if no font server
  edit_mode = true;
  strcpy (msg_string, msg);
  if (def) strcpy (edit_string, def);
  else edit_string[0] = 0;
  edit_cursor = strlen (edit_string);
  edit_command = cmd;
}

void csBugPlug::ExitEditMode ()
{
  if (edit_string[0] == 0) return;
  int i;
  float f;
  switch (edit_command)
  {
    case DEBUGCMD_GAMMA:
      csScanStr (edit_string, "%f", &f);
      G2D->SetGamma (f);
      break;
    case DEBUGCMD_FOV:
      csScanStr (edit_string, "%d", &i);
      spider->GetCamera ()->SetFOV (i, G3D->GetWidth ());
      break;
    case DEBUGCMD_FOVANGLE:
      csScanStr (edit_string, "%f", &f);
      spider->GetCamera ()->SetFOVAngle (f, G3D->GetWidth ());
      break;
  }
}

void csBugPlug::DebugCmd (const char* cmd)
{
  char* cmdstr = csStrNew (cmd);
  char* params;
  char* space = strchr (cmdstr, ' ');
  if (space == 0)
  {
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "debugcmd syntax: <plugin> <command>");
  }
  else
  {
    params = space + 1;
    *space = 0;

    csRef<iBase> comp;
    comp = CS_QUERY_REGISTRY_TAG(object_reg, cmdstr);

    if (comp == 0)
    {
      csRef<iPluginManager> plugmgr = 
	CS_QUERY_REGISTRY (object_reg, iPluginManager);
      CS_ASSERT (plugmgr);
      csRef<iBase> comp =
	CS_QUERY_PLUGIN_CLASS (plugmgr, cmdstr, iBase);
    }

    if (!comp)
    {
      Report (CS_REPORTER_SEVERITY_NOTIFY,
	"Could not load plugin '%s' for debug command execution.",
	cmdstr);
    }
    else
    {
      csRef<iDebugHelper> dbghelp = 
	SCF_QUERY_INTERFACE (comp, iDebugHelper);
      if (!dbghelp)
      {
	Report (CS_REPORTER_SEVERITY_NOTIFY,
	  "Plugin '%s' doesn't support debug command execution.",
	  cmdstr);
      }
      else
      {
	bool res = dbghelp->DebugCommand (params);
	Report (CS_REPORTER_SEVERITY_NOTIFY,
	  "Debug command execution %s.",
	  res ? "successful" : "failed");
      }
    }
  }
  delete[] cmdstr;
}

int csBugPlug::GetKeyCode (const char* keystring, bool& shift, bool& alt,
	bool& ctrl)
{
  shift = alt = ctrl = false;
  char* dash = strchr (keystring, '-');
  while (dash)
  {
    if (!strncmp (keystring, "shift", int (dash-keystring))) shift = true;
    else if (!strncmp (keystring, "alt", int (dash-keystring))) alt = true;
    else if (!strncmp (keystring, "ctrl", int (dash-keystring))) ctrl = true;
    keystring = dash+1;
    dash = strchr (keystring, '-');
  }

  int keycode = -1;
  if (!strcmp (keystring, "tab")) keycode = CSKEY_TAB;
  else if (!strcmp (keystring, "space")) keycode = ' ';
  else if (!strcmp (keystring, "esc")) keycode = CSKEY_ESC;
  else if (!strcmp (keystring, "enter")) keycode = CSKEY_ENTER;
  else if (!strcmp (keystring, "bs")) keycode = CSKEY_BACKSPACE;
  else if (!strcmp (keystring, "up")) keycode = CSKEY_UP;
  else if (!strcmp (keystring, "down")) keycode = CSKEY_DOWN;
  else if (!strcmp (keystring, "right")) keycode = CSKEY_RIGHT;
  else if (!strcmp (keystring, "left")) keycode = CSKEY_LEFT;
  else if (!strcmp (keystring, "pgup")) keycode = CSKEY_PGUP;
  else if (!strcmp (keystring, "pgdn")) keycode = CSKEY_PGDN;
  else if (!strcmp (keystring, "home")) keycode = CSKEY_HOME;
  else if (!strcmp (keystring, "end")) keycode = CSKEY_END;
  else if (!strcmp (keystring, "ins")) keycode = CSKEY_INS;
  else if (!strcmp (keystring, "del")) keycode = CSKEY_DEL;
  else if (!strcmp (keystring, "f1")) keycode = CSKEY_F1;
  else if (!strcmp (keystring, "f2")) keycode = CSKEY_F2;
  else if (!strcmp (keystring, "f3")) keycode = CSKEY_F3;
  else if (!strcmp (keystring, "f4")) keycode = CSKEY_F4;
  else if (!strcmp (keystring, "f5")) keycode = CSKEY_F5;
  else if (!strcmp (keystring, "f6")) keycode = CSKEY_F6;
  else if (!strcmp (keystring, "f7")) keycode = CSKEY_F7;
  else if (!strcmp (keystring, "f8")) keycode = CSKEY_F8;
  else if (!strcmp (keystring, "f9")) keycode = CSKEY_F9;
  else if (!strcmp (keystring, "f10")) keycode = CSKEY_F10;
  else if (!strcmp (keystring, "f11")) keycode = CSKEY_F11;
  else if (!strcmp (keystring, "f12")) keycode = CSKEY_F12;
  else if (*(keystring+1) != 0) return -1;
  else if ((*keystring >= 'A' && *keystring <= 'Z')
  	|| strchr ("!@#$%^&*()_+", *keystring))
  {
    shift = 1;
    keycode = *keystring;
  }
  else
    keycode = *keystring;

  return keycode;
}

int csBugPlug::GetCommandCode (const char* cmd, char* args)
{
  char* spc = strchr (cmd, ' ');
  if (spc)
  {
    *spc = 0;
    strcpy (args, spc+1);
  }
  else
    args[0] = 0;

  if (!strcmp (cmd, "debugenter"))	return DEBUGCMD_DEBUGENTER;
  if (!strcmp (cmd, "mouseenter"))	return DEBUGCMD_MOUSEENTER;
  if (!strcmp (cmd, "quit"))		return DEBUGCMD_QUIT;
  if (!strcmp (cmd, "status"))		return DEBUGCMD_STATUS;
  if (!strcmp (cmd, "help"))		return DEBUGCMD_HELP;

  if (!strcmp (cmd, "dumpeng"))		return DEBUGCMD_DUMPENG;
  if (!strcmp (cmd, "dumpsec"))		return DEBUGCMD_DUMPSEC;
  if (!strcmp (cmd, "edges"))		return DEBUGCMD_EDGES;
  if (!strcmp (cmd, "clear"))		return DEBUGCMD_CLEAR;
  if (!strcmp (cmd, "cacheclear"))	return DEBUGCMD_CACHECLEAR;
  if (!strcmp (cmd, "cachedump"))	return DEBUGCMD_CACHEDUMP;
  if (!strcmp (cmd, "texture"))		return DEBUGCMD_TEXTURE;
  if (!strcmp (cmd, "bilinear"))	return DEBUGCMD_BILINEAR;
  if (!strcmp (cmd, "trilinear"))	return DEBUGCMD_TRILINEAR;
  if (!strcmp (cmd, "lighting"))	return DEBUGCMD_LIGHTING;
  if (!strcmp (cmd, "gouraud"))		return DEBUGCMD_GOURAUD;
  if (!strcmp (cmd, "ilace"))		return DEBUGCMD_ILACE;
  if (!strcmp (cmd, "mmx"))		return DEBUGCMD_MMX;
  if (!strcmp (cmd, "transp"))		return DEBUGCMD_TRANSP;
  if (!strcmp (cmd, "mipmap"))		return DEBUGCMD_MIPMAP;
  if (!strcmp (cmd, "inter"))		return DEBUGCMD_INTER;
  if (!strcmp (cmd, "gamma"))		return DEBUGCMD_GAMMA;
  if (!strcmp (cmd, "dblbuff"))		return DEBUGCMD_DBLBUFF;
  if (!strcmp (cmd, "dumpcam"))		return DEBUGCMD_DUMPCAM;
  if (!strcmp (cmd, "fov"))		return DEBUGCMD_FOV;
  if (!strcmp (cmd, "fovangle"))	return DEBUGCMD_FOVANGLE;
  if (!strcmp (cmd, "terrvis"))		return DEBUGCMD_TERRVIS;
  if (!strcmp (cmd, "meshbbox"))	return DEBUGCMD_MESHBBOX;
  if (!strcmp (cmd, "meshrad"))		return DEBUGCMD_MESHRAD;
  if (!strcmp (cmd, "debuggraph"))	return DEBUGCMD_DEBUGGRAPH;
  if (!strcmp (cmd, "enginecmd"))	return DEBUGCMD_ENGINECMD;
  if (!strcmp (cmd, "enginestate"))	return DEBUGCMD_ENGINESTATE;
  if (!strcmp (cmd, "visculview"))	return DEBUGCMD_VISCULVIEW;
  if (!strcmp (cmd, "visculcmd"))	return DEBUGCMD_VISCULCMD;
  if (!strcmp (cmd, "debugsector"))	return DEBUGCMD_DEBUGSECTOR;
  if (!strcmp (cmd, "ds_forward"))	return DEBUGCMD_DS_FORWARD;
  if (!strcmp (cmd, "ds_backward"))	return DEBUGCMD_DS_BACKWARD;
  if (!strcmp (cmd, "ds_up"))		return DEBUGCMD_DS_UP;
  if (!strcmp (cmd, "ds_down"))		return DEBUGCMD_DS_DOWN;
  if (!strcmp (cmd, "ds_turnleft"))	return DEBUGCMD_DS_TURNLEFT;
  if (!strcmp (cmd, "ds_turnright"))	return DEBUGCMD_DS_TURNRIGHT;
  if (!strcmp (cmd, "ds_left"))		return DEBUGCMD_DS_LEFT;
  if (!strcmp (cmd, "ds_right"))	return DEBUGCMD_DS_RIGHT;
  if (!strcmp (cmd, "debugview"))	return DEBUGCMD_DEBUGVIEW;
  if (!strcmp (cmd, "scrshot"))		return DEBUGCMD_SCRSHOT;
  if (!strcmp (cmd, "fps"))		return DEBUGCMD_FPS;
  if (!strcmp (cmd, "hideselected"))	return DEBUGCMD_HIDESELECTED;
  if (!strcmp (cmd, "undohide"))	return DEBUGCMD_UNDOHIDE;
  if (!strcmp (cmd, "counterreset"))	return DEBUGCMD_COUNTERRESET;
  if (!strcmp (cmd, "counterfreeze"))	return DEBUGCMD_COUNTERFREEZE;
  if (!strcmp (cmd, "counterremove"))	return DEBUGCMD_COUNTERREMOVE;
  if (!strcmp (cmd, "shadowdebug"))	return DEBUGCMD_SHADOWDEBUG;
  if (!strcmp (cmd, "debugcmd"))	return DEBUGCMD_DEBUGCMD;

  return DEBUGCMD_UNKNOWN;
}

int csBugPlug::GetCommandCode (int key, bool shift, bool alt, bool ctrl,
	char*& args)
{
  csKeyMap* m = mappings;
  while (m)
  {
    if (m->key == key && m->shift == shift && m->alt == alt && m->ctrl == ctrl)
    {
      args = m->args;
      return m->cmd;
    }
    m = m->next;
  }
  args = 0;
  return DEBUGCMD_UNKNOWN;
}

void csBugPlug::AddCommand (const char* keystring, const char* cmdstring)
{
  bool shift, alt, ctrl;
  int keycode = GetKeyCode (keystring, shift, alt, ctrl);
  // Check if valid key name.
  if (keycode == -1) return;

  char args[512];
  int cmdcode = GetCommandCode (cmdstring, args);
  // Check if valid command name.
  if (cmdcode == DEBUGCMD_UNKNOWN) return;

  // Check if key isn't already defined.
  char* args2;
  if (GetCommandCode (keycode, shift, alt, ctrl, args2) != DEBUGCMD_UNKNOWN)
    return;

  // Make new key assignment.
  csKeyMap* map = new csKeyMap ();
  map->key = keycode;
  map->shift = shift;
  map->alt = alt;
  map->ctrl = ctrl;
  map->cmd = cmdcode;
  map->next = mappings;
  if (mappings) mappings->prev = map;
  map->prev = 0;
  if (args[0])
    map->args = csStrNew (args);
  else
    map->args = 0;
  mappings = map;
}

bool csBugPlug::ReadLine (iFile* file, char* buf, int nbytes)
{
  if (!file)
    return false;

  char c = '\n';
  while (c == '\n' || c == '\r')
    if (!file->Read (&c, 1))
      break;

  if (file->AtEOF())
    return false;

  char* p = buf;
  const char* plim = p + nbytes - 1;
  while (p < plim)
  {
    if (c == '\n' || c == '\r')
      break;
    *p++ = c;
    if (!file->Read (&c, 1))
      break;
  }
  *p = '\0';
  return true;
}

void csBugPlug::ReadKeyBindings (const char* filename)
{
  csRef<iFile> f (VFS->Open (filename, VFS_FILE_READ));
  if (f)
  {
    char buf[256];
    while (ReadLine (f, buf, 255))
    {
      char* del = strchr (buf, '=');
      if (del)
      {
        *del = 0;
	AddCommand (buf, del+1);
      }
      else
      {
        Report (CS_REPORTER_SEVERITY_WARNING,
    	  "BugPlug hit a badly formed line in '%s'!", filename);
        return;
      }
    }
  }
  else
  {
    Report (CS_REPORTER_SEVERITY_WARNING,
    	"BugPlug could not read '%s'!", filename);
  }
}

void csBugPlug::Dump (iEngine* engine)
{
  Report (CS_REPORTER_SEVERITY_DEBUG,
  	"===========================================");
  iSectorList* sectors = engine->GetSectors ();
  iMeshList* meshes = engine->GetMeshes ();
  iMeshFactoryList* factories = engine->GetMeshFactories ();
  Report (CS_REPORTER_SEVERITY_DEBUG,
    "%d sectors, %d mesh factories, %d mesh objects",
    sectors->GetCount (),
    factories->GetCount (),
    meshes->GetCount ());
  int i;
  for (i = 0 ; i < sectors->GetCount () ; i++)
  {
    iSector* sector = sectors->Get (i);
    Dump (sector);
  }
  for (i = 0 ; i < factories->GetCount () ; i++)
  {
    iMeshFactoryWrapper* meshfact = factories->Get (i);
    Dump (meshfact);
  }
  for (i = 0 ; i < meshes->GetCount () ; i++)
  {
    iMeshWrapper* mesh = meshes->Get (i);
    Dump (mesh);
  }
  Report (CS_REPORTER_SEVERITY_DEBUG,
  	"===========================================");
}

void csBugPlug::Dump (iSector* sector)
{
  const char* sn = sector->QueryObject ()->GetName ();
  Report (CS_REPORTER_SEVERITY_DEBUG, "    Sector '%s' (%08lx)",
  	sn ? sn : "?", sector);
  Report (CS_REPORTER_SEVERITY_DEBUG, "    %d meshes, %d lights",
  	sector->GetMeshes ()->GetCount (),
	sector->GetLights ()->GetCount ());
  int i;
  for (i = 0 ; i < sector->GetMeshes ()->GetCount () ; i++)
  {
    iMeshWrapper* mesh = sector->GetMeshes ()->Get (i);
    const char* n = mesh->QueryObject ()->GetName ();
    Report (CS_REPORTER_SEVERITY_DEBUG, "        Mesh '%s' (%08lx)",
    	n ? n : "?", mesh);
  }
}

void csBugPlug::Dump (iMeshWrapper* mesh)
{
  const char* mn = mesh->QueryObject ()->GetName ();
  Report (CS_REPORTER_SEVERITY_DEBUG, "    Mesh wrapper '%s' (%08lx)",
  	mn ? mn : "?", mesh);
  iMeshObject* obj = mesh->GetMeshObject ();
  if (!obj)
  {
    Report (CS_REPORTER_SEVERITY_DEBUG, "        Mesh object missing!");
  }
  else
  {
    csRef<iFactory> fact (SCF_QUERY_INTERFACE (obj, iFactory));
    if (fact)
      Report (CS_REPORTER_SEVERITY_DEBUG, "        Plugin '%s'",
  	  fact->QueryDescription () ? fact->QueryDescription () : "0");
    csBox3 bbox;
    obj->GetObjectModel ()->GetObjectBoundingBox (bbox);
    Report (CS_REPORTER_SEVERITY_DEBUG, "        Object bounding box:");
    Dump (8, bbox);
  }
  iMovable* movable = mesh->GetMovable ();
  if (!movable)
  {
    Report (CS_REPORTER_SEVERITY_DEBUG, "        Mesh object missing!");
  }
  else
  {
    csReversibleTransform& trans = movable->GetTransform ();
    Dump (8, trans.GetOrigin (), "Movable origin");
    Dump (8, trans.GetO2T (), "Movable O2T");
    int cnt = movable->GetSectors ()->GetCount ();
    int i;
    for (i = 0 ; i < cnt ; i++)
    {
      iSector* sec = movable->GetSectors ()->Get (i);
      const char* sn = sec->QueryObject ()->GetName ();
      Report (CS_REPORTER_SEVERITY_DEBUG, "        In sector '%s'",
      	sn ? sn : "?");
    }
  }
}

void csBugPlug::Dump (iMeshFactoryWrapper* meshfact)
{
  const char* mn = meshfact->QueryObject ()->GetName ();
  Report (CS_REPORTER_SEVERITY_DEBUG, "        Mesh factory wrapper '%s' (%08lx)",
  	mn ? mn : "?", meshfact);
}

void csBugPlug::Dump (int indent, const csMatrix3& m, char const* name)
{
  char ind[255];
  int i;
  for (i = 0 ; i < indent ; i++) ind[i] = ' ';
  ind[i] = 0;
  Report (CS_REPORTER_SEVERITY_DEBUG, "%sMatrix '%s':", ind, name);
  Report (CS_REPORTER_SEVERITY_DEBUG, "%s/", ind);
  Report (CS_REPORTER_SEVERITY_DEBUG, "%s| %3.2f %3.2f %3.2f",
  	ind, m.m11, m.m12, m.m13);
  Report (CS_REPORTER_SEVERITY_DEBUG, "%s| %3.2f %3.2f %3.2f",
  	ind, m.m21, m.m22, m.m23);
  Report (CS_REPORTER_SEVERITY_DEBUG, "%s| %3.2f %3.2f %3.2f",
  	ind, m.m31, m.m32, m.m33);
  Report (CS_REPORTER_SEVERITY_DEBUG, "%s\\", ind);
}

void csBugPlug::Dump (int indent, const csVector3& v, char const* name)
{
  char ind[255];
  int i;
  for (i = 0 ; i < indent ; i++) ind[i] = ' ';
  ind[i] = 0;
  Report (CS_REPORTER_SEVERITY_DEBUG,
  	"%sVector '%s': (%f,%f,%f)", ind, name, v.x, v.y, v.z);
}

void csBugPlug::Dump (int indent, const csVector2& v, char const* name)
{
  char ind[255];
  int i;
  for (i = 0 ; i < indent ; i++) ind[i] = ' ';
  ind[i] = 0;
  Report (CS_REPORTER_SEVERITY_DEBUG, "%sVector '%s': (%f,%f)",
  	ind, name, v.x, v.y);
}

void csBugPlug::Dump (int indent, const csPlane3& p)
{
  char ind[255];
  int i;
  for (i = 0 ; i < indent ; i++) ind[i] = ' ';
  ind[i] = 0;
  Report (CS_REPORTER_SEVERITY_DEBUG, "%sA=%2.2f B=%2.2f C=%2.2f D=%2.2f",
            ind, p.norm.x, p.norm.y, p.norm.z, p.DD);
}

void csBugPlug::Dump (int indent, const csBox2& b)
{
  char ind[255];
  int i;
  for (i = 0 ; i < indent ; i++) ind[i] = ' ';
  ind[i] = 0;
  Report (CS_REPORTER_SEVERITY_DEBUG, "%s(%2.2f,%2.2f)-(%2.2f,%2.2f)", ind,
  	b.MinX (), b.MinY (), b.MaxX (), b.MaxY ());
}

void csBugPlug::Dump (int indent, const csBox3& b)
{
  char ind[255];
  int i;
  for (i = 0 ; i < indent ; i++) ind[i] = ' ';
  ind[i] = 0;
  Report (CS_REPORTER_SEVERITY_DEBUG, "%s(%2.2f,%2.2f,%2.2f)-(%2.2f,%2.2f,%2.2f)",
  	ind, b.MinX (), b.MinY (), b.MinZ (), b.MaxX (), b.MaxY (), b.MaxZ ());
}

void csBugPlug::Dump (iCamera* c)
{
  const char* sn = c->GetSector ()->QueryObject ()->GetName ();
  if (!sn) sn = "?";
  csPlane3* far_plane = c->GetFarPlane ();
  Report (CS_REPORTER_SEVERITY_DEBUG,
  	"Camera: %s (mirror=%d, fov=%d, fovangle=%g,",
  	sn, c->IsMirrored (), c->GetFOV (), c->GetFOVAngle ());
  Report (CS_REPORTER_SEVERITY_DEBUG, "    shiftx=%g shifty=%g camnr=%d)",
  	c->GetShiftX (), c->GetShiftY (), c->GetCameraNumber ());
  if (far_plane)
    Report (CS_REPORTER_SEVERITY_DEBUG, "    far_plane=(%g,%g,%g,%g)",
    	far_plane->A (), far_plane->B (), far_plane->C (), far_plane->D ());
  csReversibleTransform& trans = c->GetTransform ();
  Dump (4, trans.GetO2TTranslation (), "Camera vector");
  Dump (4, trans.GetO2T (), "Camera matrix");
}

void csBugPlug::Dump (iPolygon3D* poly)
{
  iPolygon3DStatic* ps = poly->GetStaticData ();
  const char* poly_name = ps->GetName ();
  if (!poly_name) poly_name = "<noname>";
  Report (CS_REPORTER_SEVERITY_DEBUG, "Polygon '%s'",
  	poly_name);
  int nv = ps->GetVertexCount ();
  int i;
  int* idx = ps->GetVertexIndices ();
  char buf[256];
  sprintf (buf, "  Vertices: ");
  for (i = 0 ; i < nv ; i++)
    sprintf (buf+strlen (buf), "%d ", idx[i]);
  Report (CS_REPORTER_SEVERITY_DEBUG, buf);
}

bool csBugPlug::HandleEvent (iEvent& event)
{
  if (event.Type == csevKeyDown)
  {
    return EatKey (event);
  }
  else if (event.Type == csevKeyUp)
  {
    return EatKey (event);
  }
  else if (event.Type == csevMouseDown)
  {
    return EatMouse (event);
  }
  else if (event.Type == csevMouseUp)
  {
    return EatMouse (event);
  }
  else if (event.Type == csevMouseMove)
  {
    return EatMouse (event);
  }
  else if (event.Type == csevBroadcast)
  {
    if (event.Command.Code == cscmdPreProcess)
    {
      return HandleStartFrame (event);
    }
    if (event.Command.Code == cscmdPostProcess)
    {
      return HandleEndFrame (event);
    }
  }

  return false;
}

//---------------------------------------------------------------------------

void csBugPlug::CleanDebugSector ()
{
  if (!debug_sector.sector) return;
  iRegion* db_region = Engine->CreateRegion ("__BugPlug_region__");
  db_region->DeleteAll ();

  iRegionList* reglist = Engine->GetRegions ();
  reglist->Remove (db_region);

  delete debug_sector.view;

  debug_sector.sector = 0;
  debug_sector.view = 0;
}

void csBugPlug::SetupDebugSector ()
{
  CleanDebugSector ();
  if (!Engine)
  {
    Report (CS_REPORTER_SEVERITY_NOTIFY, "There is no engine!");
    return;
  }

  iRegion* db_region = Engine->CreateRegion ("__BugPlug_region__");
  debug_sector.sector = Engine->CreateSector ("__BugPlug_sector__");
  db_region->QueryObject ()->ObjAdd (debug_sector.sector->QueryObject ());

  debug_sector.view = new csView (Engine, G3D);
  int w3d = G3D->GetWidth ();
  int h3d = G3D->GetHeight ();
  debug_sector.view->SetRectangle (0, 0, w3d, h3d);
  debug_sector.view->GetCamera ()->SetSector (debug_sector.sector);
}

iMaterialWrapper* csBugPlug::FindColor (float r, float g, float b)
{
  // Assumes the current region is the debug region.
  char name[100];
  sprintf (name, "mat%d,%d,%d\n", int (r*255), int (g*255), int (b*255));
  iMaterialWrapper* mw = Engine->FindMaterial (name);
  if (mw) return mw;
  // Create a new material.
  csRef<iMaterial> mat (Engine->CreateBaseMaterial (0, 0, 0, 0));
  mat->SetFlatColor (csRGBcolor (int (r*255), int (g*255), int (b*255)));
  mw = Engine->GetMaterialList ()->NewMaterial (mat);
  mw->QueryObject ()->SetName (name);
  mw->Register (G3D->GetTextureManager ());
  mw->GetMaterialHandle ()->Prepare ();
  return mw;
}

void csBugPlug::DebugSectorBox (const csBox3& box, float r, float g, float b,
  	const char* name, iMeshObject*)
{
  if (!debug_sector.sector) return;

  iMaterialWrapper* mat = FindColor (r, g, b);
  // Create the box and add it to the engine.
  csVector3 pos = box.GetCenter ();
  csBox3 tbox;
  tbox.Set (box.Min ()-pos, box.Max ()-pos);

  csRef<iMeshFactoryWrapper> mf (Engine->CreateMeshFactory (
  	"crystalspace.mesh.object.genmesh", name ? name : "__BugPlug_fact__"));
  csRef<iGeneralFactoryState> gfs (
  	SCF_QUERY_INTERFACE (mf->GetMeshObjectFactory (),
  	iGeneralFactoryState));
  CS_ASSERT (gfs != 0);
  gfs->SetMaterialWrapper (mat);
  gfs->GenerateBox (tbox);
  gfs->CalculateNormals ();
  gfs->GetColors ()[0].Set (1, 1, 1);
  gfs->GetColors ()[1].Set (.5, .5, .5);
  gfs->GetColors ()[2].Set (.3, .3, .3);
  gfs->GetColors ()[3].Set (.8, .8, .8);
  gfs->GetColors ()[4].Set (.4, .4, .4);
  gfs->GetColors ()[5].Set (.2, .2, .2);
  gfs->GetColors ()[6].Set (.9, .9, .9);
  gfs->GetColors ()[7].Set (.6, .6, .6);

  csRef<iMeshWrapper> mw (Engine->CreateMeshWrapper (
  	mf, name ? name : "__BugPlug_mesh__", debug_sector.sector, pos));
  csRef<iGeneralMeshState> gms (SCF_QUERY_INTERFACE (mw->GetMeshObject (),
  	iGeneralMeshState));
  CS_ASSERT (gms != 0);
  gms->SetLighting (false);
  gms->SetColor (csColor (0, 0, 0));
  gms->SetManualColors (true);

  // The following two calls are not needed since CS_ZBUF_USE and
  // Object render priority are the default but they show how you
  // can do this.
  mw->SetZBufMode (CS_ZBUF_USE);
  mw->SetRenderPriority (Engine->GetObjectRenderPriority ());

  //mw->DeferUpdateLighting (CS_NLIGHT_STATIC|CS_NLIGHT_DYNAMIC, 10);
}

void csBugPlug::DebugSectorTriangle (const csVector3& s1, const csVector3& s2,
  	const csVector3& s3, float r, float g, float b)
{
  if (!debug_sector.sector) return;

  iMaterialWrapper* mat = FindColor (r, g, b);
  // Create the box and add it to the engine.
  csVector3 pos = s1;
  csVector3 ss1 (0);
  csVector3 ss2 = s2-s1;
  csVector3 ss3 = s3-s1;

  csRef<iMeshFactoryWrapper> mf (Engine->CreateMeshFactory (
  	"crystalspace.mesh.object.genmesh", "__BugPlug_tri__"));
  csRef<iGeneralFactoryState> gfs (
  	SCF_QUERY_INTERFACE (mf->GetMeshObjectFactory (),
  	iGeneralFactoryState));
  CS_ASSERT (gfs != 0);
  gfs->SetMaterialWrapper (mat);
  gfs->SetVertexCount (3);
  gfs->GetVertices ()[0] = ss1;
  gfs->GetVertices ()[1] = ss2;
  gfs->GetVertices ()[2] = ss3;
  gfs->GetTexels ()[0].Set (0, 0);
  gfs->GetTexels ()[1].Set (1, 0);
  gfs->GetTexels ()[2].Set (0, 1);
  gfs->SetTriangleCount (2);
  gfs->GetTriangles ()[0].a = 0;
  gfs->GetTriangles ()[0].b = 1;
  gfs->GetTriangles ()[0].c = 2;
  gfs->GetTriangles ()[1].a = 2;
  gfs->GetTriangles ()[1].b = 1;
  gfs->GetTriangles ()[1].c = 0;

  gfs->CalculateNormals ();
  gfs->GetColors ()[0].Set (1, 1, 1);
  gfs->GetColors ()[1].Set (0, 0, 0);
  gfs->GetColors ()[2].Set (0, 0, 0);

  csRef<iMeshWrapper> mw (Engine->CreateMeshWrapper (
  	mf, "__BugPlug_tri__", debug_sector.sector, pos));
  csRef<iGeneralMeshState> gms (SCF_QUERY_INTERFACE (mw->GetMeshObject (),
  	iGeneralMeshState));
  CS_ASSERT (gms != 0);
  gms->SetLighting (false);
  gms->SetColor (csColor (0, 0, 0));
  gms->SetManualColors (true);
  gms->SetMixMode (CS_FX_ADD);

  mw->SetZBufMode (CS_ZBUF_TEST);
  mw->SetRenderPriority (Engine->GetAlphaRenderPriority ());

  //mw->DeferUpdateLighting (CS_NLIGHT_STATIC|CS_NLIGHT_DYNAMIC, 10);
}

void csBugPlug::SwitchDebugSector (const csReversibleTransform& trans)
{
  if (!debug_sector.sector)
  {
    Report (CS_REPORTER_SEVERITY_NOTIFY, "There is no debug sector!");
    return;
  }
  debug_sector.show = !debug_sector.show;
  if (debug_sector.show)
  {
    debug_sector.view->GetCamera ()->SetTransform (trans);
    debug_view.show = false;
  }
}

//---------------------------------------------------------------------------

void csBugPlug::CleanDebugView ()
{
  delete[] debug_view.lines;
  debug_view.lines = 0;
  debug_view.num_lines = 0;
  debug_view.max_lines = 0;
  delete[] debug_view.boxes;
  debug_view.boxes = 0;
  debug_view.num_boxes = 0;
  debug_view.max_boxes = 0;
  delete[] debug_view.points;
  debug_view.points = 0;
  debug_view.num_points = 0;
  debug_view.max_points = 0;
  if (debug_view.object)
  {
    debug_view.object->DecRef ();
    debug_view.object = 0;
  }
}

void csBugPlug::SetupDebugView ()
{
  CleanDebugView ();
}

int csBugPlug::DebugViewPoint (const csVector2& point)
{
  if (debug_view.num_points >= debug_view.max_points)
  {
    debug_view.max_points += 50;
    csVector2* new_points = new csVector2 [debug_view.max_points];
    if (debug_view.num_points > 0)
    {
      memcpy (new_points, debug_view.points,
      	sizeof (csVector2)*debug_view.num_points);
      delete[] debug_view.points;
    }
    debug_view.points = new_points;
  }
  debug_view.points[debug_view.num_points++] = point;
  return debug_view.num_points-1;
}

void csBugPlug::DebugViewLine (int i1, int i2)
{
  if (debug_view.num_lines >= debug_view.max_lines)
  {
    debug_view.max_lines += 30;
    dbLine* new_lines = new dbLine [debug_view.max_lines];
    if (debug_view.num_lines > 0)
    {
      memcpy (new_lines, debug_view.lines,
      	sizeof (dbLine)*debug_view.num_lines);
      delete[] debug_view.lines;
    }
    debug_view.lines = new_lines;
  }
  debug_view.lines[debug_view.num_lines].i1 = i1;
  debug_view.lines[debug_view.num_lines].i2 = i2;
  debug_view.num_lines++;
}

void csBugPlug::DebugViewBox (int i1, int i2)
{
  if (debug_view.num_boxes >= debug_view.max_boxes)
  {
    debug_view.max_boxes += 30;
    dbLine* new_boxes = new dbLine [debug_view.max_boxes];
    if (debug_view.num_boxes > 0)
    {
      memcpy (new_boxes, debug_view.boxes,
      	sizeof (dbLine)*debug_view.num_boxes);
      delete[] debug_view.boxes;
    }
    debug_view.boxes = new_boxes;
  }
  debug_view.boxes[debug_view.num_boxes].i1 = i1;
  debug_view.boxes[debug_view.num_boxes].i2 = i2;
  debug_view.num_boxes++;
}

void csBugPlug::DebugViewRenderObject (iBugPlugRenderObject* obj)
{
  if (obj) obj->IncRef ();
  if (debug_view.object) debug_view.object->DecRef ();
  debug_view.object = obj;
}

void csBugPlug::SwitchDebugView ()
{
  debug_view.show = !debug_view.show;
  if (debug_view.show)
  {
    debug_sector.show = false;
    debug_view.drag_point = -1;
  }
}

int csBugPlug::FindCounter (const char* countername)
{
  int i;
  for (i = 0 ; i < counters.Length () ; i++)
    if (!strcmp (counters[i]->countername, countername))
      return i;
  return -1;
}

void csBugPlug::FullResetCounters ()
{
  int i;
  for (i = 0 ; i < counters.Length () ; i++)
  {
    int j;
    for (j = 0 ; j < 10 ; j++)
    {
      counters[i]->values[j].total = 0;
      counters[i]->values[j].current = 0;
    }
  }
  counter_frames = 0;
}

void csBugPlug::ShowCounters ()
{
  if (counters.Length () == 0) return;

  G3D->BeginDraw (CSDRAW_2DGRAPHICS);
  iFontServer* fntsvr = G2D->GetFontServer ();
  if (!fntsvr) return;
  csRef<iFont> fnt (fntsvr->GetFont (0));
  if (fnt == 0)
  {
    fnt = fntsvr->LoadFont (CSFONT_COURIER);
  }
  CS_ASSERT (fnt != 0);
  int fw, fh;
  fnt->GetMaxSize (fw, fh);
  int sh = G2D->GetHeight ();
  int bgcolor = G2D->FindRGB (255, 255, 255);
  int fgcolor = G2D->FindRGB (0, 0, 0);

  if (!counter_freeze) counter_frames++;
  int i;
  int cur_y = 10;
  for (i = 0 ; i < counters.Length () ; i++)
  {
    csCounter* c = counters[i];
    int j;
    for (j = 0 ; j < 10 ; j++)
    {
      c->values[j].total += c->values[j].current;
    }
    if (c->is_enum)
    {
      float tottotal = 0;
      float totcurrent = 0;
      for (j = 0 ; j < 10 ; j++)
      {
        tottotal += c->values[j].total;
	totcurrent += float (c->values[j].current);
      }
      if (totcurrent == 0) totcurrent = 1;
      GfxWrite (G2D, fnt, 10, cur_y, fgcolor, bgcolor,
    	"%s: %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f / %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f",
	c->countername,
	c->values[0].total * 100.0 / tottotal,
	c->values[1].total * 100.0 / tottotal,
	c->values[2].total * 100.0 / tottotal,
	c->values[3].total * 100.0 / tottotal,
	c->values[4].total * 100.0 / tottotal,
	c->values[5].total * 100.0 / tottotal,
	c->values[6].total * 100.0 / tottotal,
	c->values[7].total * 100.0 / tottotal,
	c->values[8].total * 100.0 / tottotal,
	c->values[9].total * 100.0 / tottotal,
	c->values[0].current * 100.0 / totcurrent,
	c->values[1].current * 100.0 / totcurrent,
	c->values[2].current * 100.0 / totcurrent,
	c->values[3].current * 100.0 / totcurrent,
	c->values[4].current * 100.0 / totcurrent,
	c->values[5].current * 100.0 / totcurrent,
	c->values[6].current * 100.0 / totcurrent,
	c->values[7].current * 100.0 / totcurrent,
	c->values[8].current * 100.0 / totcurrent,
	c->values[9].current * 100.0 / totcurrent);
      for (j = 0 ; j < 10 ; j++)
      {
	c->values[j].current = 0;
      }
    }
    else
    {
      GfxWrite (G2D, fnt, 10, cur_y, fgcolor, bgcolor,
    	"%s: last=%d tot=%g avg=%g",
	c->countername,
	c->values[0].current, c->values[0].total,
	c->values[0].total / float (counter_frames));
      c->values[0].current = 0;
    }
    cur_y += fh+4;
    if (cur_y > sh-10) break;
  }
}

void csBugPlug::AddCounter (const char* countername, int amount)
{
  if (counter_freeze) return;
  int i = FindCounter (countername);
  if (i == -1)
  {
    csCounter* c = new csCounter ();
    c->is_enum = false;
    c->countername = csStrNew (countername);
    c->values[0].total = 0;
    c->values[0].current = amount;
    counters.Push (c);
  }
  else
  {
    counters[i]->is_enum = false;
    counters[i]->values[0].current += amount;
  }
}

void csBugPlug::AddCounterEnum (const char* countername, int enumval,
	int amount)
{
  if (counter_freeze) return;
  if (enumval < 0 || enumval > 9) return;
  int i = FindCounter (countername);
  if (i == -1)
  {
    csCounter* c = new csCounter ();
    c->is_enum = true;
    c->countername = csStrNew (countername);
    int j;
    for (j = 0 ; j < 10 ; j++)
    {
      c->values[j].total = 0;
      c->values[j].current = 0;
    }
    c->values[enumval].current = amount;
    counters.Push (c);
  }
  else
  {
    if (!counters[i]->is_enum)
    {
      int j;
      for (j = 0 ; j < 10 ; j++)
      {
        counters[i]->values[j].total = 0;
        counters[i]->values[j].current = 0;
      }
      counters[i]->is_enum = true;
    }
    counters[i]->values[enumval].current += amount;
  }
}

void csBugPlug::ResetCounter (const char* countername, int value)
{
  if (counter_freeze) return;
  int i = FindCounter (countername);
  if (i != -1)
  {
    int j;
    for (j = 0 ; j < 10 ; j++)
    {
      counters[i]->values[j].current = value;
    }
  }
}

void csBugPlug::RemoveCounter (const char* countername)
{
  int i = FindCounter (countername);
  if (i != -1)
    counters.DeleteIndex (i);
}

//---------------------------------------------------------------------------

