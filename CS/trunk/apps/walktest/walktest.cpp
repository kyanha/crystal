/*
    Copyright (C) 1998-2000 by Jorrit Tyberghein

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

#include <stdarg.h>

#define SYSDEF_PATH
#define SYSDEF_ACCESS
#include "cssysdef.h"
#include "walktest.h"
#include "infmaze.h"
#include "hugeroom.h"
#include "command.h"
#include "csparser/csloader.h"
#include "csgeom/frustum.h"
#include "csengine/dumper.h"
#include "csengine/campos.h"
#include "csengine/region.h"
#include "csengine/csview.h"
#include "csengine/stats.h"
#include "csengine/light.h"
#include "csengine/texture.h"
#include "csengine/thing.h"
#include "csengine/wirefrm.h"
#include "csengine/polytext.h"
#include "csengine/polytmap.h"
#include "csengine/polygon.h"
#include "csengine/pol2d.h"
#include "csengine/cbuffer.h"
#include "csengine/sector.h"
#include "csengine/engine.h"
#include "csengine/covtree.h"
#include "csengine/solidbsp.h"
#include "csengine/meshobj.h"
#include "csparser/impexp.h"
#include "csutil/csrect.h"
#include "csutil/scanstr.h"
#include "csobject/dataobj.h"
#include "csobject/csobject.h"
#include "csobject/objiter.h"
#include "csgfx/pngsave.h"
#include "csparser/snddatao.h"
#include "cssys/system.h"
#include "csfx/cspixmap.h"
#include "version.h"
#include "qint.h"
#include "iutil/cfgmgr.h"
#include "ivideo/graph3d.h"
#include "ivideo/txtmgr.h"
#include "isound/handle.h"
#include "isound/source.h"
#include "isound/listener.h"
#include "isound/source.h"
#include "isound/renderer.h"
#include "igraphic/image.h"
#include "iengine/collider.h"
#include "iengine/motion.h"
#include "ivaria/perfstat.h"

#if defined(OS_DOS) || defined(OS_WIN32) || defined (OS_OS2)
#  include <io.h>
#elif defined(OS_UNIX)
#  include <unistd.h>
#endif

#include "debug/fpu80x86.h"	// for debugging numerical instabilities

WalkTest *Sys;
converter *ImportExport;

#define Gfx3D System->G3D
#define Gfx2D System->G2D


//------------------------------------------------- We need the 3D engine -----

REGISTER_STATIC_LIBRARY (engine)

//-----------------------------------------------------------------------------

char WalkTest::map_dir [100];
bool WalkTest::move_3d = false;

WalkTest::WalkTest () :
  SysSystemDriver (), pos (0, 0, 0), velocity (0, 0, 0)
{
  extern bool CommandHandler (const char *cmd, const char *arg);
  csCommandProcessor::ExtraHandler = CommandHandler;
  auto_script = NULL;
  view = NULL;
  infinite_maze = NULL;
  huge_room = NULL;
  wMissile_boom = NULL;
  wMissile_whoosh = NULL;
  cslogo = NULL;
  engine = NULL;
  anim_sky = NULL;

  wf = NULL;
  map_mode = MAP_OFF;
  map_projection = WF_ORTHO_PERSP;
  do_fps = true;
  do_stats = false;
  do_clear = false;
  do_edges = false;
  do_light_frust = false;
  do_show_coord = false;
  do_show_cbuffer = false;
  busy_perf_test = false;
  do_show_z = false;
  do_show_palette = false;
  do_infinite = false;
  do_huge = false;
  do_cd = true;
  do_freelook = false;
  player_spawned = false;
  do_gravity = true;
  inverse_mouse = false;
  selected_light = NULL;
  selected_polygon = NULL;
  move_forward = false;
  cfg_draw_octree = 0;
  cfg_recording = -1;
  recorded_perf_stats_name = NULL;
  recorded_perf_stats = NULL;
  perf_stats = NULL;
  recorded_cmd = NULL;
  recorded_arg = NULL;
  cfg_playrecording = -1;
  cfg_debug_check_frustum = 0;

  plbody = pllegs = NULL;

  velocity.Set (0, 0, 0);
  angle.Set (0, 0, 0);
  angle_velocity.Set (0, 0, 0);

//pl=new PhysicsLibrary;

  timeFPS = 0.0;

  bgcolor_txtmap = 255;
  bgcolor_map = 0;
  bgcolor_fclear = 255;

  ConsoleInput = NULL;
  SmallConsole = false;

  debug_box1.Set (csVector3 (-1, -1, -1), csVector3 (1, 1, 1));
  debug_box2.Set (csVector3 (2, 2, 2), csVector3 (3, 3, 3));
  do_show_debug_boxes = false;
}

WalkTest::~WalkTest ()
{
  if (ConsoleInput) ConsoleInput->DecRef ();
  if (collide_system) collide_system->DecRef ();
  if (Font) Font->DecRef ();
  delete wf;
  delete [] auto_script;
  delete view;
  delete infinite_maze;
  delete huge_room;
  delete cslogo;
  delete plbody;
  delete pllegs;
  delete [] recorded_perf_stats_name;
  if (perf_stats) perf_stats->DecRef ();
  if (Engine) Engine->DecRef ();
}

void WalkTest::SetSystemDefaults (iConfigManager *Config)
{
  superclass::SetSystemDefaults (Config);
  do_fps = Config->GetBool ("Walktest.Settings.FPS", true);
  do_stats = Config->GetBool ("Walktest.Settings.Stats", false);
  do_cd = Config->GetBool ("Walktest.Settings.Colldet", true);

  const char *val;
  if (!(val = GetNameCL ()))
    val = Config->GetStr ("Walktest.Settings.WorldFile");

  // if an absolute path is given, copy it. Otherwise prepend "/lev/".
  if (val[0] == '/')
    strcpy (map_dir, val);
  else
    sprintf (map_dir, "/lev/%s", val);
  
  if (GetOptionCL ("clear"))
  {
    do_clear = true;
    Sys->Printf (MSG_INITIALIZATION, "Screen will be cleared every frame.\n");
  }
  else if (GetOptionCL ("noclear"))
  {
    do_clear = false;
    Sys->Printf (MSG_INITIALIZATION, "Screen will not be cleared every frame.\n");
  }

  if (GetOptionCL ("stats"))
  {
    do_stats = true;
    Sys->Printf (MSG_INITIALIZATION, "Statistics enabled.\n");
  }
  else if (GetOptionCL ("nostats"))
  {
    do_stats = false;
    Sys->Printf (MSG_INITIALIZATION, "Statistics disabled.\n");
  }

  if (GetOptionCL ("fps"))
  {
    do_fps = true;
    Sys->Printf (MSG_INITIALIZATION, "Frame Per Second enabled.\n");
  }
  else if (GetOptionCL ("nofps"))
  {
    do_fps = false;
    Sys->Printf (MSG_INITIALIZATION, "Frame Per Second disabled.\n");
  }

  if (GetOptionCL ("infinite"))
    do_infinite = true;

  if (GetOptionCL ("huge"))
    do_huge = true;

  extern bool do_bots;
  if (GetOptionCL ("bots"))
    do_bots = true;

  if (GetOptionCL ("colldet"))
  {
    do_cd = true;
    Sys->Printf (MSG_INITIALIZATION, "Enabled collision detection system.\n");
  }
  else if (GetOptionCL ("nocolldet"))
  {
    do_cd = false;
    Sys->Printf (MSG_INITIALIZATION, "Disabled collision detection system.\n");
  }

  if ((val = GetOptionCL ("exec")))
  {
    delete [] auto_script;
    auto_script = strnew (val);
  }
}

void WalkTest::Help ()
{
  SysSystemDriver::Help ();
  Sys->Printf (MSG_STDOUT, "  -exec=<script>     execute given script at startup\n");
  Sys->Printf (MSG_STDOUT, "  -[no]clear         clear display every frame (default '%sclear')\n", do_clear ? "" : "no");
  Sys->Printf (MSG_STDOUT, "  -[no]stats         statistics (default '%sstats')\n", do_stats ? "" : "no");
  Sys->Printf (MSG_STDOUT, "  -[no]fps           frame rate printing (default '%sfps')\n", do_fps ? "" : "no");
  Sys->Printf (MSG_STDOUT, "  -[no]colldet       collision detection system (default '%scolldet')\n", do_cd ? "" : "no");
  Sys->Printf (MSG_STDOUT, "  -infinite          special infinite level generation (ignores map file!)\n");
  Sys->Printf (MSG_STDOUT, "  -huge              special huge level generation (ignores map file!)\n");
  Sys->Printf (MSG_STDOUT, "  -bots              allow random generation of bots\n");
  Sys->Printf (MSG_STDOUT, "  <path>             load map from VFS <path> (default '%s')\n",
        Config->GetStr ("Walktest.Settings.WorldFile", "world"));
}

//-----------------------------------------------------------------------------
extern bool CommandHandler (const char *cmd, const char *arg);

void WalkTest::NextFrame ()
{
  // The following will fetch all events from queue and handle them
  SysSystemDriver::NextFrame ();

  cs_time elapsed_time, current_time;
  GetElapsedTime (elapsed_time, current_time);

  if (perf_stats) timeFPS = perf_stats->GetFPS ();
  else timeFPS = 0;

  if (!Console || !Console->GetVisible ())
  {
    // If the console was turned off last frame no stats have been accumulated
    // as it was paused so we return here and loop again.
    if (perf_stats && perf_stats->Pause (false))
      return;
    // Emit recorded commands directly to the CommandHandler
    if (cfg_playrecording > 0 &&
	recording.Length () > 0)
    {
      csRecordedCamera* reccam = (csRecordedCamera*)recording[cfg_playrecording];
      if (reccam->cmd)
	CommandHandler(reccam->cmd, reccam->arg);
    }
  }
  else
    // The console has been turned on so we pause the stats plugin.
    if (perf_stats) perf_stats->Pause (true);

  // Update the Motion Manager
  if (System->MotionMan)
    System->MotionMan->UpdateAll ();

  MoveSystems (elapsed_time, current_time);
  PrepareFrame (elapsed_time, current_time);
  DrawFrame (elapsed_time, current_time);

  // Execute one line from the script.
  if (!busy_perf_test)
  {
    char buf[256];
    if (csCommandProcessor::get_script_line (buf, 255)) csCommandProcessor::perform_line (buf);
  }
}


void WalkTest::MoveSystems (cs_time elapsed_time, cs_time current_time)
{
  // First move the sky.
  if (anim_sky)
  {
    csMovable& move = anim_sky->GetMovable ();
    switch (anim_sky_rot)
    {
      case 0:
	{
          csXRotMatrix3 mat (anim_sky_speed * 2. * M_PI * (float)elapsed_time/1000.);
          move.Transform (mat);
	  break;
	}
      case 1:
	{
          csYRotMatrix3 mat (anim_sky_speed * 2. * M_PI * (float)elapsed_time/1000.);
          move.Transform (mat);
	  break;
	}
      case 2:
	{
          csZRotMatrix3 mat (anim_sky_speed * 2. * M_PI * (float)elapsed_time/1000.);
          move.Transform (mat);
	  break;
	}
    }
    move.UpdateMove ();
  }

  // Update all busy entities.
  // We first push all entities in a vector so that NextFrame() can safely
  // remove it self from the busy_entities list (or add other entities).
  int i;
  busy_vector.SetLength (0);
  csWalkEntity* wentity;
  for (i = 0 ; i < busy_entities.Length () ; i++)
  {
    wentity = (csWalkEntity*)busy_entities[i];
    busy_vector.Push (wentity);
  }
  for (i = 0 ; i < busy_vector.Length () ; i++)
  {
    wentity = (csWalkEntity*)busy_vector[i];
    wentity->NextFrame (elapsed_time);
  }

  // Move all particle systems and meshes.
  Sys->engine->NextFrame (current_time);

  // Record the first time this routine is called.
  extern bool do_bots;
  if (do_bots)
  {
    static long first_time = -1;
    static cs_time next_bot_at;
    if (first_time == -1)
    {
      first_time = current_time;
      next_bot_at = current_time+1000*10;
    }
    if (current_time > next_bot_at)
    {
      extern void add_bot (float size, csSector* where, csVector3 const& pos,
	                        float dyn_radius);
      add_bot (2, view->GetCamera ()->GetSector (),
               view->GetCamera ()->GetOrigin (), 0);
      next_bot_at = current_time+1000*10;
    }
  }
  if (!Console || !Console->GetVisible ())
  {
    int alt,shift,ctrl;
    float speed = 1;

    alt = GetKeyState (CSKEY_ALT);
    ctrl = GetKeyState (CSKEY_CTRL);
    shift = GetKeyState (CSKEY_SHIFT);
    if (ctrl)
      speed = .5;
    if (shift)
      speed = 2;

    /// Act as usual...
    strafe (0,1);
    look (0,1);
    step (0,1);
    rotate (0,1);

    if (Sys->Sound)
    {
      iSoundListener *sndListener = Sys->Sound->GetListener();
      if(sndListener)
      {
        // take position/direction from view->GetCamera ()
        csVector3 v = view->GetCamera ()->GetOrigin ();
        csMatrix3 m = view->GetCamera ()->GetC2W();
        csVector3 f = m.Col3();
        csVector3 t = m.Col2();
        sndListener->SetPosition(v);
        sndListener->SetDirection(f,t);
        //sndListener->SetDirection(...);
      }
    }
  }

  extern void move_bots (cs_time);
  move_bots (elapsed_time);

  if (move_forward)
    step (1, 0);
}


//-----------------------------------------------------------------------------

//@@@
int covtree_level = 1;
bool do_covtree_dump = false;

void WalkTest::DrawFrameDebug ()
{
  if (do_show_z)
  {
    extern void DrawZbuffer ();
    DrawZbuffer ();
  }
  if (do_show_palette)
  {
    extern void DrawPalette ();
    DrawPalette ();
  }
  extern void draw_edges (csRenderView*, int, void*);
  if (do_edges)
  {
    view->GetEngine ()->DrawFunc (view->GetCamera (), view->GetClipper (),
    	draw_edges);
  }
  if (selected_polygon || selected_light)
    view->GetEngine ()->DrawFunc (view->GetCamera (), view->GetClipper (),
	draw_edges, (void*)1);
  if (do_light_frust && selected_light)
  {
    extern void show_frustum (csFrustumView*, int, void*);
    ((csStatLight*)selected_light)->LightingFunc (show_frustum);
  }
  if (cfg_draw_octree)
  {
    extern void DrawOctreeBoxes (int);
    DrawOctreeBoxes (cfg_draw_octree == -1 ? -1 : cfg_draw_octree-1);
  }
  if (cfg_debug_check_frustum)
  {
    extern void ShowCheckFrustum (csView* view, csSector* room,
    	const csVector3& pos, int num_vis);
    ShowCheckFrustum (view, view->GetCamera ()->GetSector (),
    	view->GetCamera ()->GetOrigin (), cfg_debug_check_frustum);
  }
  if (do_show_cbuffer)
  {
    csCBuffer* cbuf = view->GetEngine ()->GetCBuffer ();
    if (cbuf)
    {
      cbuf->GfxDump (Gfx2D, Gfx3D);
    }
  }
  if (do_show_debug_boxes)
  {
    extern void DrawDebugBoxes (csCamera* cam, bool do3d);
    DrawDebugBoxes (view->GetCamera (), false);
  }
}

void WalkTest::DrawFrameExtraDebug ()
{
  csCoverageMaskTree* covtree = Sys->engine->GetCovtree ();
  if (covtree)
  {
    Gfx2D->Clear (0);
#   if 0
    covtree->GfxDump (Gfx2D, covtree_level);
#   else
    //covtree->MakeInvalid ();
    covtree->MakeEmpty ();
    csCamera* c = view->GetCamera ();
    csPolygon2D poly1, poly2, poly3;
    poly1.AddPerspective (c->Other2This (csVector3 (-1.6, 1, 5)));
    poly1.AddPerspective (c->Other2This (csVector3 (1, 1.6, 5)));
    poly1.AddPerspective (c->Other2This (csVector3 (1, -1, 5)));
    poly1.AddPerspective (c->Other2This (csVector3 (-1, -1.3, 5)));
    covtree->InsertPolygon (poly1.GetVertices (),
	poly1.GetNumVertices (), poly1.GetBoundingBox ());
    poly2.AddPerspective (c->Other2This (csVector3 (-1.5, .5, 6)));
    poly2.AddPerspective (c->Other2This (csVector3 (.5, .5, 6)));
    poly2.AddPerspective (c->Other2This (csVector3 (.5, -1.5, 6)));
    poly2.AddPerspective (c->Other2This (csVector3 (-1.5, -1.5, 6)));
    printf ("T2:%d ", covtree->TestPolygon (poly2.GetVertices (),
	poly2.GetNumVertices (), poly2.GetBoundingBox ()));
    printf ("P2:%d ", covtree->InsertPolygon (poly2.GetVertices (),
	poly2.GetNumVertices (), poly2.GetBoundingBox ()));
    poly3.AddPerspective (c->Other2This (csVector3 (-.5, .15, 7)));
    poly3.AddPerspective (c->Other2This (csVector3 (1.5, .15, 7)));
    poly3.AddPerspective (c->Other2This (csVector3 (1.5, -.5, 7)));
    printf ("T3:%d ", covtree->TestPolygon (poly3.GetVertices (),
	poly3.GetNumVertices (), poly3.GetBoundingBox ()));
    printf ("P3:%d\n", covtree->InsertPolygon (poly3.GetVertices (),
	poly3.GetNumVertices (), poly3.GetBoundingBox ()));
    covtree->GfxDump (Gfx2D, covtree_level);
    poly1.Draw (Gfx2D, 0x0303);
    poly2.Draw (Gfx2D, 0x07e0);
    poly3.Draw (Gfx2D, 0x008f);
    //covtree->TestConsistency ();
#   endif
  }
}

void WalkTest::DrawFrameDebug3D ()
{
  if (do_show_debug_boxes)
  {
    extern void DrawDebugBoxes (csCamera* cam, bool do3d);
    DrawDebugBoxes (view->GetCamera (), true);
  }
}

void WalkTest::GfxWrite (int x, int y, int fg, int bg, char *str, ...)
{
  va_list arg;
  char buf[256];

  va_start (arg, str);
  vsprintf (buf, str, arg);
  va_end (arg);

  G2D->Write (Font, x, y, fg, bg, buf);
}

void WalkTest::DrawFrameConsole ()
{
  if (Console)
    Console->Draw2D (NULL);

  if (!Console || !Console->GetVisible ())
  {
    int fw, fh;
    Font->GetMaxSize (fw, fh);

    if (do_fps)
    {
      GfxWrite (11, FRAME_HEIGHT - fh - 3, 0, -1, "FPS=%.2f", timeFPS);
      GfxWrite (10, FRAME_HEIGHT - fh - 2, fgcolor_stats, -1, "FPS=%.2f", timeFPS);
    }
    if (do_stats)
    {
      char buffer[50];
      sprintf (buffer, "pc=%d pd=%d po=%d pa=%d pr=%d",
      	Stats::polygons_considered, Stats::polygons_drawn,
	Stats::portals_drawn, Stats::polygons_accepted,
	Stats::polygons_rejected);
      GfxWrite(FRAME_WIDTH - 30 * 8 - 1, FRAME_HEIGHT - fh - 3, 0, -1, "%s", buffer);
      GfxWrite(FRAME_WIDTH - 30 * 8, FRAME_HEIGHT - fh - 2, fgcolor_stats, -1,
      	"%s", buffer);
    }
    else if (do_show_coord)
    {
      char buffer[100];
      sprintf (buffer, "%2.2f,%2.2f,%2.2f: %s",
        view->GetCamera ()->GetW2CTranslation ().x,
	view->GetCamera ()->GetW2CTranslation ().y,
        view->GetCamera ()->GetW2CTranslation ().z,
	view->GetCamera ()->GetSector()->GetName ());
      GfxWrite (FRAME_WIDTH - 24 * 8 - 1, FRAME_HEIGHT - fh - 3, 0, -1, buffer);
      GfxWrite (FRAME_WIDTH - 24 * 8, FRAME_HEIGHT - fh - 2, fgcolor_stats, -1, buffer);
    }
  }
}


void WalkTest::DrawFrame3D (int drawflags, cs_time /*current_time*/)
{
  // Tell Gfx3D we're going to display 3D things
  if (!Gfx3D->BeginDraw (engine->GetBeginDrawFlags () | drawflags
  	| CSDRAW_3DGRAPHICS))
    return;

  // Apply lighting BEFORE the very first frame
  csDynLight* dyn = Sys->engine->GetFirstDynLight ();
  while (dyn)
  {
    extern void HandleDynLight (csDynLight*);
    csDynLight* dn = dyn->GetNext ();
    if (dyn->GetChild (csDataObject::Type)) HandleDynLight (dyn);
    dyn = dn;
  }
  // Apply lighting to all meshes
  light_statics ();

  //------------
  // Here comes the main call to the engine. view->Draw() actually
  // takes the current camera and starts rendering.
  //------------
  if (map_mode != MAP_ON && map_mode != MAP_TXT && !do_covtree_dump)
    view->Draw ();

  // no need to clear screen anymore
  drawflags = 0;

  // Display the 3D parts of the console
  if (Console)
    Console->Draw3D (NULL);
}


void WalkTest::DrawFrame2D (void)
{
  if (cslogo)
  {
    unsigned w = cslogo->Width()  * FRAME_WIDTH  / 640;
    unsigned h = cslogo->Height() * FRAME_HEIGHT / 480;
    cslogo->DrawScaled (Gfx3D, FRAME_WIDTH - 2 - (w * 151) / 256 , 2, w, h);
  }

  // White-board for debugging purposes.
  if (do_covtree_dump)
    DrawFrameExtraDebug ();
}

void WalkTest::DrawFrameMap ()
{
  if (map_mode == MAP_TXT)
  {
    // Texture mapped map.
    csPolyIt* pi = view->GetEngine ()->NewPolyIterator ();
    csPolygon3D* p;
    csCamera* cam = wf->GetCamera ();
    const csVector3& c = cam->GetOrigin ();
    float scale = 10.;
    csBox2 box (2, 2, FRAME_WIDTH-2, FRAME_HEIGHT-2);
    csClipper* clipper = new csBoxClipper (box);
    csVector2 maxdist (FRAME_WIDTH/(2*scale), FRAME_HEIGHT/(2*scale));
    csPoly2DPool* render_pool = view->GetEngine ()->render_pol2d_pool;
    Gfx3D->SetRenderState (G3DRENDERSTATE_ZBUFFERMODE, CS_ZBUF_USE);
    int i;
    bool in;
    while ((p = pi->Fetch ()) != NULL)
    {
      if (p->GetPortal ()) continue;
      in = false;
      for (i = 0 ; i < p->GetNumVertices () ; i++)
      {
	const csVector3& v = p->Vwor (i);
	if (ABS (v.x - c.x) < maxdist.x &&
	    ABS (v.z - c.z) < maxdist.y &&
	    v.y <= c.y && v.y >= (c.y-5))
	{
	  in = true;
	  break;
	}
      }
      if (in)
      {
        csPolygon2D* clip = (csPolygon2D*)(render_pool->Alloc ());
	clip->MakeEmpty ();
	for (i = 0 ; i < p->GetNumVertices () ; i++)
	{
	  const csVector3& v = p->Vwor (i);
	  csVector2 v2;
	  v2.x = (v-c).x;
	  v2.y = (v-c).z;
	  v2 *= scale;
	  v2.x += FRAME_WIDTH/2;
	  v2.y += FRAME_HEIGHT/2;
	  clip->AddVertex (v2);
	}
	if (clip->ClipAgainst (clipper))
	{
  	  if (p->GetTextureType () != POLYTXT_LIGHTMAP)
  	  {
	    // @@@ Unsupported for now.
	  }
	  else
	  {
    	    static G3DPolygonDP g3dpoly;
	    g3dpoly.num = clip->GetNumVertices ();
    	    g3dpoly.mat_handle = p->GetMaterialHandle ();
    	    g3dpoly.inv_aspect = view->GetCamera ()->GetInvFOV ();
	    for (i = 0 ; i <g3dpoly.num ; i++)
	    {
	      g3dpoly.vertices[i].sx = clip->GetVertex (i)->x;
	      g3dpoly.vertices[i].sy = clip->GetVertex (i)->y;
	    }
	    g3dpoly.alpha = 0;
	    g3dpoly.z_value = p->Vwor (0).y;
	    g3dpoly.poly_texture = p->GetLightMapInfo ()->GetPolyTex ();

	    csPolyTxtPlane* txt_plane = p->GetLightMapInfo ()->GetTxtPlane ();
    	    csMatrix3 m_cam2tex;
    	    csVector3 v_cam2tex;
	    txt_plane->GetTextureSpace (m_cam2tex, v_cam2tex);
	    float s;
	    s = m_cam2tex.m12; m_cam2tex.m12 = m_cam2tex.m13; m_cam2tex.m13 = s;
	    s = m_cam2tex.m22; m_cam2tex.m22 = m_cam2tex.m23; m_cam2tex.m23 = s;
	    s = m_cam2tex.m32; m_cam2tex.m32 = m_cam2tex.m33; m_cam2tex.m33 = s;
	    s = v_cam2tex.y; v_cam2tex.y = v_cam2tex.z; v_cam2tex.z = s;
    	    g3dpoly.plane.m_cam2tex = &m_cam2tex;
    	    g3dpoly.plane.v_cam2tex = &v_cam2tex;

	    csPlane3* plane = p->GetPolyPlane ();
    	    g3dpoly.normal.A () = plane->A ();
    	    g3dpoly.normal.B () = plane->C ();
    	    g3dpoly.normal.C () = plane->B ();
    	    g3dpoly.normal.D () = plane->D ();

	    Gfx3D->DrawPolygon (g3dpoly);
	  }
	}
        render_pool->Free (clip);
      }
    }

    delete pi;
    delete clipper;
  }
  else
  {
    // Wireframe map.
    wf->GetWireframe ()->Clear ();
    extern void draw_map (csRenderView*, int, void*);
    view->GetEngine ()->DrawFunc (view->GetCamera (),
    	view->GetClipper (), draw_map);
    wf->GetWireframe ()->Draw (Gfx3D, wf->GetCamera (), map_projection);
  }
}

void WalkTest::DrawFrame (cs_time elapsed_time, cs_time current_time)
{
  if (!Console || !Console->GetVisible ())
  {
    if (cfg_recording >= 0)
    {
      // @@@ Memory leak!
      csRecordedCamera* reccam = new csRecordedCamera ();
      csCamera* c = view->GetCamera ();
      const csMatrix3& m = c->GetO2T ();
      const csVector3& v = c->GetOrigin ();
      reccam->mat = m;
      reccam->vec = v;
      reccam->mirror = c->IsMirrored ();
      reccam->sector = c->GetSector ();
      reccam->angle = Sys->angle;
      reccam->cmd = recorded_cmd;
      reccam->arg = recorded_arg;
      recorded_cmd = recorded_arg = NULL;
      recording.Push ((void*)reccam);
    }
    if (cfg_playrecording >= 0 && recording.Length () > 0)
    {
      csRecordedCamera* reccam = (csRecordedCamera*)recording[cfg_playrecording];
      cfg_playrecording++;
      if (cfg_playrecording >= recording.Length ()) 
      {
	if (cfg_playloop)
	  cfg_playrecording = 0;
	else
	{
	  // A performance measuring demo has finished..stop and write to
	  // file
	  cfg_playrecording = -1;
	  if (perf_stats) perf_stats->FinishSubsection ();
	  recorded_perf_stats = NULL;
	  Sys->Printf (MSG_CONSOLE, "Demo '%s' finished\n", 
		       recorded_perf_stats_name);
	}
      }
      csCamera* c = view->GetCamera ();
      Sys->angle = reccam->angle;
      c->SetSector (reccam->sector);
      c->SetMirrored (reccam->mirror);
      c->SetO2T (reccam->mat);
      c->SetOrigin (reccam->vec);
    }
  }


  (void)elapsed_time; (void)current_time;

  //not used since we need WHITE background not black
  int drawflags = (do_clear || map_mode == MAP_TXT) ? CSDRAW_CLEARZBUFFER : 0;
  if (do_clear || map_mode == MAP_ON || map_mode == MAP_TXT)
  {
    if (!Gfx3D->BeginDraw (CSDRAW_2DGRAPHICS))
      return;
    int col;
    if (map_mode == MAP_ON) col = bgcolor_map;
    else if (map_mode == MAP_TXT) col = bgcolor_txtmap;
    else col = bgcolor_fclear;
    Gfx2D->Clear (col);
  }

  if (!Console
   || !Console->GetVisible ()
   || SmallConsole
   || Console->GetTransparency ())
  {
    DrawFrame3D (drawflags, current_time);
    DrawFrameDebug3D ();
  }

  // Start drawing 2D graphics
  if (!Gfx3D->BeginDraw (drawflags | CSDRAW_2DGRAPHICS))
    return;

  if (map_mode != MAP_OFF)
    DrawFrameMap ();
  else
    DrawFrameDebug ();

  DrawFrameConsole ();

  // If console is not active we draw a few additional things.
  if (!Console
   || !Console->GetVisible ())
    DrawFrame2D ();

  // Drawing code ends here
  Gfx3D->FinishDraw ();
  // Print the output.
  Gfx3D->Print (NULL);
}

int cnt = 1;
cs_time time0 = (cs_time)-1;

void WalkTest::PrepareFrame (cs_time elapsed_time, cs_time current_time)
{
  static cs_time prev_time = 0;
  if (prev_time == 0) prev_time = Time () - 10;

  // If the time interval is too big, limit to something reasonable
  // This will help a little for software OpenGL :-)
  elapsed_time = current_time - prev_time;
  if (elapsed_time > 250)
    prev_time = current_time - (elapsed_time = 250);

  if (do_cd)
  {
    extern void DoGravity (csVector3& pos, csVector3& vel);
    if (!player_spawned)
    {
      CreateColliders ();
      player_spawned=true;
    }

    for (; elapsed_time >= 10; elapsed_time -= 10, prev_time += 10)
    {
      if (move_3d)
      {
        // If we are moving in 3d then don't do any camera correction.
      }
      else
      {
        view->GetCamera ()->SetT2O (csMatrix3 ());
        view->GetCamera ()->RotateWorld (csVector3 (0,1,0), angle.y);
        if (!do_gravity)
          view->GetCamera ()->Rotate (csVector3 (1,0,0), angle.x);
      }

      csVector3 vel = view->GetCamera ()->GetT2O ()*velocity;

      static bool check_once = false;
      if (ABS (vel.x) < SMALL_EPSILON && ABS (vel.y) < SMALL_EPSILON && ABS (vel.z) < SMALL_EPSILON)
      {
        // If we don't move we don't have to do the collision detection tests.
	// However, we need to do it once to make sure that we are standing
	// on solid ground. So we first set 'check_once' to true to enable
	// one more test.
	if (check_once == false) { check_once = true; DoGravity (pos, vel); }
      }
      else { check_once = false; DoGravity (pos, vel); }

      if (do_gravity && !move_3d)
        view->GetCamera ()->Rotate (csVector3 (1,0,0), angle.x);

      // Apply angle velocity to camera angle
      angle += angle_velocity;
    }
  }

#if 0
  if (do_cd && csBeing::init)
  {
    // TODO ALEX: In future this should depend on whether the whole world
    // or 'active' sectors need to be set up as monsters hunt for player
    // outside of current sector, but this will do for now.

    // Test camera collision.
    // Load camera location into player.
    csBeing::player->sector = view->GetCamera ()->GetSector ();
    csBeing::player->transform = view->GetCamera ();
    collcount = csBeing::player->CollisionDetect ();
    // Load player transformation back into camera.
    view->GetCamera ()->SetW2C (csBeing::player->transform->GetO2T ());
    view->GetCamera ()->SetPosition (csBeing::player->transform->GetO2TTranslation ());
    view->GetCamera ()->SetSector (csBeing::player->sector);

  }
#endif
}

void perf_test (int num)
{
  Sys->busy_perf_test = true;
  cs_time t1, t2, t;
  Sys->Printf (MSG_CONSOLE, "Performance test busy...\n");
  t = t1 = Sys->Time ();
  int i;
  for (i = 0 ; i < num ; i++)
  {
    Sys->DrawFrame (Sys->Time ()-t, Sys->Time ());
    t = Sys->Time ();
  }
  t2 = Sys->Time ();
  Sys->Printf (MSG_CONSOLE, "%f secs to render %d frames: %f fps\n",
        (float)(t2-t1)/1000., num, 100000./(float)(t2-t1));
  Sys->Printf (MSG_DEBUG_0, "%f secs to render %d frames: %f fps\n",
        (float)(t2-t1)/1000., num, 100000./(float)(t2-t1));
  cnt = 1;
  time0 = (cs_time)-1;
  Sys->busy_perf_test = false;
}

void CaptureScreen ()
{
  int i = 0;
  char name [20];
  do
  {
    sprintf (name, "cryst%03d.png", i++);
  } while (i < 1000 && (access (name, 0) == 0));
  if (i >= 1000)
  {
    System->Printf (MSG_CONSOLE, "Too many screenshot files in current directory\n");
    return;
  }

  iImage *img = System->G2D->ScreenShot ();
  if (!img)
  {
    Sys->Printf (MSG_CONSOLE, "The 2D graphics driver does not support screen shots\n");
    return;
  }

  Sys->Printf (MSG_CONSOLE, "Screenshot: %s\n", name);
  if (!csSavePNG (name, img))
    Sys->Printf (MSG_CONSOLE, "There was an error while writing screen shot\n");
  img->DecRef ();
}

/*---------------------------------------------
 * Our main event loop.
 *---------------------------------------------*/

/*
 * Do a large debug dump just before the program
 * exits. This function can be installed as a last signal
 * handler if the program crashes (for systems that support
 * this).
 */
void debug_dump ()
{
  if (Sys->VFS)
    SaveCamera (Sys->VFS, "/temp/walktest.bug");
  Sys->Printf (MSG_DEBUG_0, "Camera saved in /temp/walktest.bug\n");
  Dumper::dump (Sys->view->GetCamera ());
  Sys->Printf (MSG_DEBUG_0, "Camera dumped in debug.txt\n");
  Dumper::dump (Sys->engine);
  Sys->Printf (MSG_DEBUG_0, "Engine dumped in debug.txt\n");
}

//---------------------------------------------------------------------------


void cleanup ()
{
  Sys->console_out ("Cleaning up...\n");
  free_keymap ();
  Sys->EndEngine ();
  delete Sys; Sys = NULL;
}

void start_console ()
{
  iTextureManager* txtmgr = Gfx3D->GetTextureManager ();

  // Initialize the texture manager
  txtmgr->ResetPalette ();

  // Allocate a uniformly distributed in R,G,B space palette for console
  int r,g,b;
  for (r = 0; r < 8; r++)
    for (g = 0; g < 8; g++)
      for (b = 0; b < 4; b++)
        txtmgr->ReserveColor (r * 32, g * 32, b * 64);

  txtmgr->PrepareTextures ();
  txtmgr->SetPalette ();
}

void WalkTest::EndEngine ()
{
  delete view; view = NULL;
}

void WalkTest::InitCollDet (csEngine* engine, csRegion* region)
{
  Sys->Printf (MSG_INITIALIZATION, "Computing OBBs ...\n");

  iPolygonMesh* mesh;
  int sn = engine->sectors.Length ();
  while (sn > 0)
  {
    sn--;
    csSector* sp = (csSector*)engine->sectors[sn];
    if (region && !region->IsInRegion (sp)) continue;
    // Initialize the things in this sector.
    int i;
    for (i = 0 ; i < sp->GetNumThings () ; i++)
    {
      csThing* tp = sp->GetThing (i);
      mesh = QUERY_INTERFACE (tp, iPolygonMesh);
      (void)new csCollider (*tp, collide_system, mesh);
      mesh->DecRef ();
    }
  }
  // Initialize all mesh objects for collision detection.
  int i;
  for (i = 0 ; i < engine->meshes.Length () ; i++)
  {
    csMeshWrapper* sp = (csMeshWrapper*)engine->meshes[i];
    if (region && !region->IsInRegion (sp)) continue;
    mesh = QUERY_INTERFACE (sp, iPolygonMesh);
    if (mesh)
    {
      (void)new csCollider (*sp, collide_system, mesh);
      mesh->DecRef ();
    }
  }

  // Create a player object that follows the camera around.
//  player = csBeing::PlayerSpawn("Player");

//  init = true;
//  Sys->Printf (MSG_INITIALIZATION, "DONE\n");
}

void WalkTest::LoadLibraryData(void)
{
  // Load the "standard" library
  csLoader::LoadLibraryFile (engine, "/lib/std/library");
}

void WalkTest::Inititalize2DTextures ()
{
  csTextureWrapper *texh;
  csTextureList *texlist = engine->GetTextures ();

  // Find the Crystal Space logo and set the renderer Flag to for_2d, to allow
  // the use in the 2D part.
  texh = texlist->FindByName ("cslogo");
  if (texh)
    texh->flags = CS_TEXTURE_2D;
}


void WalkTest::Create2DSprites(void)
{
  int w, h;
  csTextureWrapper *texh;
  iTextureHandle* phTex;
  csTextureList *texlist = engine->GetTextures ();

  // Create a 2D sprite for the Logo.
  texh = texlist->FindByName ("cslogo");
  if (texh)
  {
    phTex = texh->GetTextureHandle();
    if (phTex)
    {
      phTex->GetMipMapDimensions (0, w, h);
      cslogo = new csSimplePixmap (phTex, 0, 0, w, h);
    }
  }
}


bool WalkTest::Initialize (int argc, const char* const argv[], const char *iConfigName)
{
  if (!SysSystemDriver::Initialize (argc, argv, iConfigName))
  {
    Printf (MSG_FATAL_ERROR, "Failed to initialize SysSystemDriver!\n");
    return false;
  }

  // Some commercials...
  Printf (MSG_INITIALIZATION, "Crystal Space version %s (%s).\n", CS_VERSION, CS_RELEASE_DATE);
  Printf (MSG_INITIALIZATION, "Created by Jorrit Tyberghein and others...\n\n");

  // Get all collision detection and movement config file parameters.
  cfg_jumpspeed = Config->GetFloat ("Walktest.CollDet.JumpSpeed", 0.08);
  cfg_walk_accelerate = Config->GetFloat ("Walktest.CollDet.WalkAccelerate", 0.007);
  cfg_walk_maxspeed = Config->GetFloat ("Walktest.CollDet.WalkMaxSpeed", 0.1);
  cfg_walk_brake = Config->GetFloat ("Walktest.CollDet.WalkBrake", 0.014);
  cfg_rotate_accelerate = Config->GetFloat ("Walktest.CollDet.RotateAccelerate", 0.005);
  cfg_rotate_maxspeed = Config->GetFloat ("Walktest.CollDet.RotateMaxSpeed", 0.03);
  cfg_rotate_brake = Config->GetFloat ("Walktest.CollDet.RotateBrake", 0.015);
  cfg_look_accelerate = Config->GetFloat ("Walktest.CollDet.LookAccelerate", 0.028);
  cfg_body_height = Config->GetFloat ("Walktest.CollDet.BodyHeight", 1.4);
  cfg_body_width = Config->GetFloat ("Walktest.CollDet.BodyWidth", 0.5);
  cfg_body_depth = Config->GetFloat ("Walktest.CollDet.BodyDepth", 0.5);
  cfg_eye_offset = Config->GetFloat ("Walktest.CollDet.EyeOffset", -0.7);
  cfg_legs_width = Config->GetFloat ("Walktest.CollDet.LegsWidth", 0.4);
  cfg_legs_depth = Config->GetFloat ("Walktest.CollDet.LegsDepth", 0.4);
  cfg_legs_offset = Config->GetFloat ("Walktest.CollDet.LegsOffset", -1.1);

  //--- create the converter class for testing
  ImportExport = new converter();
  // process import/export files from config and print log for testing
  ImportExport->ProcessConfig (Config);
  // free memory - delete this if you want to use the data in the buffer
  delete ImportExport;
  //--- end converter test

#ifdef CS_DEBUG
  // enable all kinds of useful FPU exceptions on a x86
  // note that we can't do it above since at least on OS/2 each dynamic
  // library on loading/initialization resets the control word to default
  _control87 (0x33, 0x3f);
#else
  // this will disable exceptions on DJGPP (for the non-debug version)
  _control87 (0x3f, 0x3f);
#endif

  // Start the engine
  if (!Open ("Crystal Space"))
  {
    Printf (MSG_FATAL_ERROR, "Error opening system!\n");
    return false;
  }

  // Find the font we'll use
  Font = Gfx2D->GetFontServer ()->LoadFont (CSFONT_LARGE);

  // Open the startup console
  start_console ();

  // Set texture manager mode to verbose (useful for debugging)
  iTextureManager* txtmgr = Gfx3D->GetTextureManager ();
  txtmgr->SetVerbose (true);

  // Find the engine plugin and query the csEngine object from it...
  Engine = QUERY_PLUGIN (Sys, iEngine);
  if (!Engine)
  {
    Printf (MSG_FATAL_ERROR, "No iEngine plugin!\n");
    return false;
  }
  engine = Engine->GetCsEngine ();

  // performance statistics module, also takes care of fps
  perf_stats = QUERY_PLUGIN (this, iPerfStats);
  if (!perf_stats)
  {
    Printf (MSG_WARNING, "No iPerfStats plugin: you will have no performance statistics!\n");
  }

  // csView is a view encapsulating both a camera and a clipper.
  // You don't have to use csView as you can do the same by
  // manually creating a camera and a clipper but it makes things a little
  // easier.
  view = new csView (engine, Gfx3D);

  // Get the collide system plugin.
  const char* p = Config->GetStr ("Walktest.Settings.CollDetPlugIn",
  	"crystalspace.colldet.rapid");
  collide_system = LOAD_PLUGIN (Sys, p, "CollDet", iCollideSystem);
  if (!collide_system)
  {
    Printf (MSG_FATAL_ERROR, "No Collision Detection plugin found!\n");
    return false;
  }

  // Initialize the command processor with the engine and camera.
  csCommandProcessor::Initialize (engine, view->GetCamera (), Gfx3D, System->Console, System);

  // Now we have two choices. Either we create an infinite
  // maze (random). This happens when the '-infinite' commandline
  // option is given. Otherwise we load the given map.
  csSector* room;

  if (do_infinite || do_huge)
  {
    // The infinite maze.

    if (!VFS->ChDir ("/tmp"))
    {
      Printf (MSG_FATAL_ERROR, "Temporary directory /tmp not mounted on VFS!\n");
      return false;
    }

    Printf (MSG_INITIALIZATION, "Creating initial room!...\n");
    engine->EnableLightingCache (false);

    // Unfortunately the current movement system does not allow the user to
    // move around the maze unless collision detection is enabled, even
    // though collision detection does not really make sense in this context.
    // Hopefully the movement system will be fixed some day so that the user
    // can move around even with collision detection disabled.
    do_cd = true;

    // Load two textures that are used in the maze.
    csLoader::LoadTexture (engine, "txt", "/lib/std/stone4.gif");
    csLoader::LoadTexture (engine, "txt2", "/lib/std/mystone2.gif");

    if (do_infinite)
    {
      // Create the initial (non-random) part of the maze.
      infinite_maze = new InfiniteMaze ();
      room = infinite_maze->create_six_room (engine, 0, 0, 0)->sector;
      infinite_maze->create_six_room (engine, 0, 0, 1);
      infinite_maze->create_six_room (engine, 0, 0, 2);
      infinite_maze->create_six_room (engine, 1, 0, 2);
      infinite_maze->create_six_room (engine, 0, 1, 2);
      infinite_maze->create_six_room (engine, 1, 1, 2);
      infinite_maze->create_six_room (engine, 0, 0, 3);
      infinite_maze->create_six_room (engine, 0, 0, 4);
      infinite_maze->create_six_room (engine, -1, 0, 4);
      infinite_maze->create_six_room (engine, -2, 0, 4);
      infinite_maze->create_six_room (engine, 0, -1, 3);
      infinite_maze->create_six_room (engine, 0, -2, 3);
      infinite_maze->create_six_room (engine, 0, 1, 3);
      infinite_maze->create_six_room (engine, 0, 2, 3);
      infinite_maze->connect_infinite (0, 0, 0, 0, 0, 1);
      infinite_maze->connect_infinite (0, 0, 1, 0, 0, 2);
      infinite_maze->connect_infinite (0, 0, 2, 0, 0, 3);
      infinite_maze->connect_infinite (0, 0, 2, 1, 0, 2);
      infinite_maze->connect_infinite (0, 0, 2, 0, 1, 2);
      infinite_maze->connect_infinite (1, 1, 2, 0, 1, 2);
      infinite_maze->connect_infinite (1, 1, 2, 1, 0, 2);
      infinite_maze->connect_infinite (0, 0, 3, 0, 0, 4);
      infinite_maze->connect_infinite (-1, 0, 4, 0, 0, 4);
      infinite_maze->connect_infinite (-2, 0, 4, -1, 0, 4);
      infinite_maze->connect_infinite (0, 0, 3, 0, -1, 3);
      infinite_maze->connect_infinite (0, -1, 3, 0, -2, 3);
      infinite_maze->connect_infinite (0, 0, 3, 0, 1, 3);
      infinite_maze->connect_infinite (0, 1, 3, 0, 2, 3);
      infinite_maze->create_loose_portal (-2, 0, 4, -2, 1, 4);
    }
    else
    {
      // Create the huge world.
      huge_room = new HugeRoom ();
      room = huge_room->create_huge_world (engine);
    }

    // Prepare the engine. This will calculate all lighting and
    // prepare the lightmaps for the 3D rasterizer.
    engine->Prepare ();
  }
  else
  {
    // Load from a map file.
    Printf (MSG_INITIALIZATION, "Loading map '%s'...\n", map_dir);

    // Check the map and mount it if required
    char tmp [100];
    sprintf (tmp, "%s/", map_dir);
    if (!VFS->Exists (map_dir))
    {
      char *name = strrchr (map_dir, '/');
      if (name)
      {
        name++;
        //sprintf (tmp, "$.$/data$/%s.zip, $.$/%s.zip, $(..)$/data$/%s.zip",
        //  name, name, name);
	const char *valfiletype = "";
	valfiletype = Config->GetStr ("Walktest.Settings.WorldZipType", "");
	if (strcmp (valfiletype, "") ==0)
	{
	  valfiletype = "zip";
	}
        sprintf (tmp, "$.$/data$/%s.%s, $.$/%s.%s, $(..)$/data$/%s.%s",
           name, valfiletype, name, valfiletype, name, valfiletype );
        VFS->Mount (map_dir, tmp);
      }
    }

    if (!VFS->ChDir (map_dir))
    {
      Printf (MSG_FATAL_ERROR, "The directory on VFS for map file does not exist!\n");
      return false;
    }


    // Load the map from the file.
    if (!csLoader::LoadMapFile (engine, "world"))
    {
      Printf (MSG_FATAL_ERROR, "Loading of map failed!\n");
      return false;
    }

    LoadLibraryData ();
    Inititalize2DTextures ();
    ParseKeyCmds ();

    // Prepare the engine. This will calculate all lighting and
    // prepare the lightmaps for the 3D rasterizer.
    engine->Prepare ();

    Create2DSprites ();

    // Look for the start sector in this map.
    csCameraPosition *cp = (csCameraPosition *)engine->camera_positions.FindByName ("Start");
    const char *room_name;
    if (cp)
    {
      room_name = cp->Sector;
      if (!cp->Load (*view->GetCamera (), engine))
        room_name = "room";
    }
    else
      room_name = "room";

    room = (csSector *)engine->sectors.FindByName (room_name);
    if (!room)
    {
      Printf (MSG_FATAL_ERROR,  "Map does not contain a room called '%s'"
        " which is used\nas a starting point!\n", room_name);
      return false;
    }
  }

  // Initialize collision detection system (even if disabled so that we can enable it later).
  InitCollDet (engine, NULL);

  // Create a wireframe object which will be used for debugging.
  wf = new csWireFrameCam (txtmgr);

  // Load a few sounds.
#ifdef DO_SOUND
  wMissile_boom = csSoundDataObject::GetSound(*engine, "boom.wav");
  wMissile_whoosh = csSoundDataObject::GetSound(*engine, "whoosh.wav");
#endif

  Printf (MSG_INITIALIZATION, "--------------------------------------\n");
  if (Console)
  {
    Console->SetVisible (false);
    Console->AutoUpdate (false);
    ConsoleInput = QUERY_PLUGIN (System, iConsoleInput);
    if (ConsoleInput)
    {
      ConsoleInput->Bind (Console);
      ConsoleInput->SetPrompt ("cs# ");
      ConsoleInput->ExecuteCallback (csCommandProcessor::perform_callback, NULL);
    }

    // Set console to center of screen, if supported
    int DeltaX = G2D->GetWidth () / 10;
    int DeltaY = G2D->GetHeight () / 10;
    SmallConsole = Console->ConsoleExtension ("SetPos", DeltaX, DeltaY,
      G2D->GetWidth () - DeltaX * 2, G2D->GetHeight () - DeltaY * 2);
  }

  // Wait one second before starting.
  cs_time t = Sys->Time ()+1000;
  while (Sys->Time () < t) ;

  // Allocate the palette as calculated by the texture manager.
  txtmgr->SetPalette ();
  bgcolor_txtmap = txtmgr->FindRGB (128, 128, 128);
  bgcolor_map = 0;
  bgcolor_fclear = txtmgr->FindRGB (0, 255, 255);
  fgcolor_stats = txtmgr->FindRGB (255, 255, 255);

  // Reinit console object for 3D engine use.
  if (Console) Console->Clear ();

  // Initialize our 3D view.
  view->SetSector (room);
  // We use the width and height from the 3D renderer because this
  // can be different from the frame size (rendering in less res than
  // real window for example).
  int w3d = Gfx3D->GetWidth ();
  int h3d = Gfx3D->GetHeight ();
  view->SetRectangle (2, 2, w3d - 4, h3d - 4);

  // clear all backbuffers to black
  G2D->BeginDraw ();
  G2D->ClearAll (txtmgr->FindRGB(0,0,0));
  G2D->FinishDraw ();

  return true;
}

#if 1
// moved this out of main() to make it easier for app developer
// to override
void CreateSystem(void)
{
  // Create the system driver object
  Sys = new WalkTest ();
}
#endif

/*---------------------------------------------------------------------*
 * Signal handling for unix only.
 *---------------------------------------------------------------------*/

#ifdef OS_UNIX

/*
 * Signal handler to clean up and give some
 * final debugging information.
 */
void debug_dump ();
void cleanup ();
void handler (int sig)
{
  static bool in_exit = false;
  if (in_exit)
    exit (1);
  in_exit = true;

  int err = errno;
#if defined (__USE_GNU)
  fprintf (stderr, "\n%s signal caught; last error: %s\n",
    strsignal (sig), strerror (err));
#elif defined (OS_LINUX) || defined (OS_FREEBSD)
  fprintf (stderr, "\n%s signal caught; last error: %s\n",
    sys_siglist [sig], strerror (err));
#else
  fprintf (stderr, "\nSignal %d caught; last error: %s\n",
    sig, strerror (err));
#endif

  if (sig != SIGINT)
    debug_dump ();

  if (System) System->Shutdown = true;
  cleanup ();
  exit (1);
}

void init_sig ()
{
#ifndef DO_COREDUMP
  signal (SIGHUP, handler);
  signal (SIGINT, handler);
  signal (SIGTRAP, handler);
  signal (SIGABRT, handler);
  signal (SIGALRM, handler);
  signal (SIGTERM, handler);
  signal (SIGPIPE, handler);
  signal (SIGSEGV, handler);
  signal (SIGBUS, handler);
  signal (SIGFPE, handler);
  signal (SIGILL, handler);
#endif // ! DO_COREDUMP
}

#endif //OS_UNIX


/*---------------------------------------------------------------------*
 * Main function
 *---------------------------------------------------------------------*/
int main (int argc, char* argv[])
{
#ifdef OS_UNIX
  init_sig ();
#endif

  // Initialize the random number generator
  srand (time (NULL));

  extern void CreateSystem(void);
  CreateSystem();

  // Initialize the main system. This will load all needed plugins
  // (3D, 2D, network, sound, ..., engine) and initialize them.
  if (!Sys->Initialize (argc, argv, "/config/walktest.cfg"))
  {
    Sys->Printf (MSG_FATAL_ERROR, "Error initializing system!\n");
    fatal_exit (-1, false);
  }

  // Start the 'autoexec.cfg' script and fully execute it.
  csCommandProcessor::start_script ("/config/autoexec.cfg");
  char cmd_buf[512];
  while (csCommandProcessor::get_script_line (cmd_buf, 511))
    csCommandProcessor::perform_line (cmd_buf);

  // If there is another script given on the commandline
  // start it but do not execute it yet. This will be done
  // frame by frame.
  if (Sys->auto_script)
    csCommandProcessor::start_script (Sys->auto_script);

  // The main loop.
  Sys->Loop ();

  cleanup ();

  return 1;
}

#if 0

#define DETECT_SIZE 20
#define DETECT     "ABCDabcd01234567890+"
#define DETECTAR   "abcdABCD+09876543210"
#define DETECTFREE "@*@*@*@*@*@*@*@*@*@*"

void* operator new (size_t s)
{
  if (s <= 0) printf ("BAD SIZE in new %d\n", s);
  char* rc = (char*)malloc (s+4+DETECT_SIZE+DETECT_SIZE);
  memcpy (rc, DETECT, DETECT_SIZE);
  memcpy (rc+DETECT_SIZE, &s, 4);
  memcpy (rc+DETECT_SIZE+4+s, DETECT, DETECT_SIZE);
  return (void*)(rc+4+DETECT_SIZE);
}

void* operator new[] (size_t s)
{
  if (s <= 0) printf ("BAD SIZE in new[] %d\n", s);
  char* rc = (char*)malloc (s+4+DETECT_SIZE+DETECT_SIZE);
  memcpy (rc, DETECTAR, DETECT_SIZE);
  memcpy (rc+DETECT_SIZE, &s, 4);
  memcpy (rc+DETECT_SIZE+4+s, DETECTAR, DETECT_SIZE);
  return (void*)(rc+4+DETECT_SIZE);
}

void operator delete (void* p)
{
  if (!p) return;
  char* rc = (char*)p;
  rc -= 4+DETECT_SIZE;
  size_t s;
  memcpy (&s, rc+DETECT_SIZE, 4);
  if (strncmp (rc, DETECT, DETECT_SIZE) != 0) { printf ("operator delete: BAD START!\n"); CRASH; }
  if (strncmp (rc+4+DETECT_SIZE+s, DETECT, DETECT_SIZE) != 0) { printf ("operator delete: BAD END!\n"); CRASH; }
  memcpy (rc, DETECTFREE, DETECT_SIZE);
  memcpy (rc+4+s+DETECT_SIZE, DETECTFREE, DETECT_SIZE);
  free (rc);
}

void operator delete[] (void* p)
{
  if (!p) return;
  char* rc = (char*)p;
  rc -= 4+DETECT_SIZE;
  size_t s;
  memcpy (&s, rc+DETECT_SIZE, 4);
  if (strncmp (rc, DETECTAR, DETECT_SIZE) != 0) { printf ("operator delete[]: BAD START!\n"); CRASH; }
  if (strncmp (rc+4+DETECT_SIZE+s, DETECTAR, DETECT_SIZE) != 0) { printf ("operator delete[]: BAD END!\n"); CRASH; }
  memcpy (rc, DETECTFREE, DETECT_SIZE);
  memcpy (rc+4+s+DETECT_SIZE, DETECTFREE, DETECT_SIZE);
  free (rc);
}

#endif
