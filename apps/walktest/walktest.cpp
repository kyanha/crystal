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

#define SYSDEF_PATH
#define SYSDEF_ACCESS
#include "sysdef.h"
#include "walktest/walktest.h"
#include "walktest/infmaze.h"
#include "walktest/hugeroom.h"
#include "version.h"
#include "qint.h"
#include "cssys/system.h"
#include "apps/support/command.h"
#include "cstools/simpcons.h"
#include "csparser/csloader.h"
#include "csgeom/frustrum.h"
#include "csengine/dumper.h"
#include "csengine/csview.h"
#include "csengine/stats.h"
#include "csengine/light.h"
#include "csengine/dynlight.h"
#include "csengine/texture.h"
#include "csengine/thing.h"
#include "csengine/wirefrm.h"
#include "csengine/polytext.h"
#include "csengine/polyset.h"
#include "csengine/polygon.h"
#include "csengine/pol2d.h"
#include "csengine/sector.h"
#include "csengine/world.h"
#include "csengine/covtree.h"
#include "csengine/solidbsp.h"
#include "csscript/csscript.h"
#include "csscript/intscri.h"
#include "csengine/collider.h"
#include "csengine/cspixmap.h"
#include "csengine/terrain.h"
#include "csengine/cssprite.h"
#include "csengine/rapid.h"
#include "csparser/impexp.h"
#include "csutil/inifile.h"
#include "csutil/csrect.h"
#include "csobject/dataobj.h"
#include "cssfxldr/common/snddata.h"
#include "csgfxldr/pngsave.h"
#include "csparser/snddatao.h"
#include "igraph3d.h"
#include "itxtmgr.h"
#include "isndrdr.h"
#include "iimage.h"

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


//------------------------------ We need the VFS plugin and the 3D engine -----

REGISTER_STATIC_LIBRARY (vfs)
REGISTER_STATIC_LIBRARY (engine)

//-----------------------------------------------------------------------------

void DrawZbuffer ()
{
  for (int y = 0; y < FRAME_HEIGHT; y++)
  {
    int gi_pixelbytes = System->G2D->GetPixelBytes ();

    ULong *zbuf = Gfx3D->GetZBufPoint(0, y);

    if (zbuf)
      if (gi_pixelbytes == 4)
      {
        ULong *dest = (ULong *)Gfx2D->GetPixelAt (0, y);
        for (int x = 0; x < FRAME_WIDTH; x++)
          *dest++ = *zbuf++ >> 10;
      }
      else if (gi_pixelbytes == 2)
      {
        UShort *dest = (UShort *)Gfx2D->GetPixelAt(0, y);
        for (int x = 0; x < FRAME_WIDTH; x++)
          *dest++ = (unsigned short)(*zbuf++ >> 13);
      }
      else
      {
        unsigned char *dest = Gfx2D->GetPixelAt (0, y);
        for (int x = 0; x < FRAME_WIDTH; x++)
          *dest++ = (unsigned char)(*zbuf++ >> 16);
      }
  }
}

void DrawPalette ()
{
  if (System->G2D->GetPixelBytes () != 1)
    return;
  int pw = System->G2D->GetWidth () / 16;
  int ph = System->G2D->GetHeight () / 16;
  for (int i = 0; i < 16; i++)
    for (int j = 0; j < 16; j++)
      System->G2D->DrawBox (i * pw, j * ph, pw, ph, j * 16 + i);
}

int collcount = 0;

//------------------------------------------------------------------------
// The following series of functions are all special callback functions
// which are called by csWorld::DrawFunc() or csLight::LightingFunc().
//------------------------------------------------------------------------

// Callback for LightingFunc() to show the lighting frustrum for the
// selected light.
void show_frustrum (csLightView* lview, int type, void* /*entity*/)
{
  iTextureManager* txtmgr = Gfx3D->GetTextureManager ();
  int white = txtmgr->FindRGB (255, 255, 255);
  int red = txtmgr->FindRGB (255, 0, 0);

  if (type == CALLBACK_POLYGON)
  {
    csCamera* cam = Sys->view->GetCamera ();
    csFrustrum* fr = lview->light_frustrum;
    csVector3 v0, v1, v2;
    csVector3 light_cam = cam->Other2This (fr->GetOrigin ());
    int j;

    for (j = 0 ; j < fr->GetNumVertices () ; j++)
    {
      v0 = fr->GetVertices ()[j] + fr->GetOrigin ();
      v1 = cam->Other2This (v0);
      v0 = fr->GetVertices ()[(j+1)%fr->GetNumVertices ()] + fr->GetOrigin ();
      v2 = cam->Other2This (v0);
      Gfx3D->DrawLine (light_cam, v1, cam->aspect, red);
      Gfx3D->DrawLine (light_cam, v2, cam->aspect, red);
      Gfx3D->DrawLine (v1, v2, cam->aspect, white);
    }
  }
}

// Callback for DrawFunc() to select an object with the mouse. The coordinate
// to check for is in 'coord_check_vector'.
bool check_poly;
bool check_light;
csVector2 coord_check_vector;

void select_object (csRenderView* rview, int type, void* entity)
{
  static csPolygon3D* last_poly = NULL;

  if (type == CALLBACK_POLYGON)
  {
    // Here we depend on CALLBACK_POLYGON being called right before CALLBACK_POLYGON2D.
    last_poly = (csPolygon3D*)entity;
  }
  else if (type == CALLBACK_POLYGON2D)
  {
    int i;
    csPolygon2D* polygon = (csPolygon2D*)entity;
    int num = polygon->GetNumVertices ();
    CHK (csPolygon2D* pp = new csPolygon2D ());
    if (rview->IsMirrored ())
      for (i = 0 ; i < num ; i++)
        pp->AddVertex  (polygon->GetVertices ()[num-i-1]);
    else
      for (i = 0 ; i < num ; i++)
        pp->AddVertex  (polygon->GetVertices ()[i]);
    if (csMath2::InPoly2D (coord_check_vector, pp->GetVertices (),
        pp->GetNumVertices (), &pp->GetBoundingBox ()) != CS_POLY_OUT)
    {
      csPolygonSet* ps = (csPolygonSet*)(last_poly->GetParent ());
      Sys->Printf (MSG_DEBUG_0, "Hit polygon '%s/%s'\n",
        ps->GetName (), last_poly->GetName ());
      Dumper::dump (polygon, "csPolygon2D");
      Dumper::dump (last_poly);
      if (check_poly && !last_poly->GetPortal ())
      {
        if (Sys->selected_polygon == last_poly) Sys->selected_polygon = NULL;
        else Sys->selected_polygon = last_poly;
	//check_poly = false;
      }
    }

    CHK (delete pp);
  }
  else if (type == CALLBACK_SECTOR)
  {
    csSector* sector = (csSector*)entity;
    int i;
    csVector3 v;
    float iz;
    int px, py, r;
    for (i = 0 ; i < sector->lights.Length () ; i++)
    {
      v = rview->Other2This (((csStatLight*)sector->lights[i])->GetCenter ());
      if (v.z > SMALL_Z)
      {
        iz = rview->aspect/v.z;
        px = QInt (v.x * iz + rview->shift_x);
        py = csWorld::frame_height - 1 - QInt (v.y * iz + rview->shift_y);
        r = QInt (.3 * iz);
        if (ABS (coord_check_vector.x - px) < 5 && ABS (coord_check_vector.y - (csWorld::frame_height-1-py)) < 5)
        {
	  csLight* light = (csLight*)sector->lights[i];
	  if (check_light)
	  {
            if (Sys->selected_light == light) Sys->selected_light = NULL;
	    else Sys->selected_light = light;
	    //check_light = false;
	  }
          Sys->Printf (MSG_CONSOLE, "Selected light %s/(%f,%f,%f).\n",
                    sector->GetName (), light->GetCenter ().x,
                    light->GetCenter ().y, light->GetCenter ().z);
          Sys->Printf (MSG_DEBUG_0, "Selected light %s/(%f,%f,%f).\n",
                    sector->GetName (), light->GetCenter ().x,
                    light->GetCenter ().y, light->GetCenter ().z);
        }
      }
    }
  }
}

// Callback for DrawFunc() to show an outline for all polygons and lights.
// If callback_data in 'rview' is not NULL then we only show outline for
// selected light and/or polygon.
void draw_edges (csRenderView* rview, int type, void* entity)
{
  iTextureManager* txtmgr = Gfx3D->GetTextureManager ();
  int selcol;
  int white = txtmgr->FindRGB (255, 255, 255);
  int red = txtmgr->FindRGB (255, 0, 0);
  int blue = txtmgr->FindRGB (0, 0, 255);
  int yellow = txtmgr->FindRGB (255, 255, 0);

  bool hilighted_only = !!rview->callback_data;
  if (hilighted_only) selcol = yellow;
  else selcol = white;
  static csPolygon3D* last_poly = NULL;

  if (type == CALLBACK_POLYGON)
  {
    // Here we depend on CALLBACK_POLYGON being called right before CALLBACK_POLYGON2D.
    last_poly = (csPolygon3D*)entity;
  }
  else if (type == CALLBACK_POLYGON2D)
  {
    csPolygon2D* polygon = (csPolygon2D*)entity;
    if (!hilighted_only || Sys->selected_polygon == last_poly)
      polygon->Draw (rview->g2d, selcol);
  }
  else if (type == CALLBACK_POLYGONQ)
  {
    if (!hilighted_only)
    {
      G3DPolygonDPFX* dpfx = (G3DPolygonDPFX*)entity;
      int i1 = dpfx->num-1;
      int i;
      for (i = 0 ; i < dpfx->num ; i++)
      {
        rview->g2d->DrawLine (dpfx->vertices[i].sx, csWorld::frame_height - 1 - dpfx->vertices[i].sy,
      	  dpfx->vertices[i1].sx, csWorld::frame_height - 1 - dpfx->vertices[i1].sy, blue);
        i1 = i;
      }
    }
  }
  else if (type == CALLBACK_SECTOR)
  {
    csSector* sector = (csSector*)entity;
    int i;
    csVector3 v;
    float iz;
    int px, py, r;
    for (i = 0 ; i < sector->lights.Length () ; i++)
    {
      csStatLight* light = (csStatLight*)(sector->lights[i]);
      if (!hilighted_only || Sys->selected_light == light)
      {
        v = rview->Other2This (light->GetCenter ());
        if (v.z > SMALL_Z)
        {
          iz = rview->aspect/v.z;
          px = QInt (v.x * iz + rview->shift_x);
          py = csWorld::frame_height - 1 - QInt (v.y * iz + rview->shift_y);
          r = QInt (.3 * iz);
          rview->g2d->DrawLine (px-r, py-r, px+r, py+r, selcol);
          rview->g2d->DrawLine (px+r, py-r, px-r, py+r, selcol);
          rview->g2d->DrawLine (px, py-2, px, py+2, red);
          rview->g2d->DrawLine (px+2, py, px-2, py, red);
        }
      }
    }
  }
}

// Callback for DrawFunc() to show a 3D map of everything that is visible.
void draw_map (csRenderView* /*rview*/, int type, void* entity)
{
  csWireFrame* wf = Sys->wf->GetWireframe ();
  if (type == CALLBACK_POLYGON)
  {
    csPolygon3D* poly = (csPolygon3D*)entity;
    int j;
    csWfPolygon* po = wf->AddPolygon ();
    po->SetVisColor (wf->GetYellow ());
    po->SetNumVertices (poly->GetVertices ().GetNumVertices ());
    for (j = 0 ; j < poly->GetVertices ().GetNumVertices () ; j++)
      po->SetVertex (j, poly->Vwor (j));
    po->Prepare ();
  }
  else if (type == CALLBACK_SECTOR)
  {
    csSector* sector = (csSector*)entity;
    int i;
    for (i = 0 ; i < sector->lights.Length () ; i++)
    {
      csWfVertex* vt = wf->AddVertex (((csStatLight*)sector->lights[i])->GetCenter ());
      vt->SetColor (wf->GetRed ());
    }
  }
}

// Callback for DrawFunc() to dump debug information about everything
// that is currently visible. This is useful to debug clipping errors
// and other visual errors.
int dump_visible_indent = 0;
void dump_visible (csRenderView* /*rview*/, int type, void* entity)
{
  int i;
  char indent_spaces[255];
  int ind = dump_visible_indent;
  if (ind > 254) ind = 254;
  for (i = 0 ; i < ind ; i++) indent_spaces[i] = ' ';
  indent_spaces[ind] = 0;

  if (type == CALLBACK_POLYGON)
  {
    csPolygon3D* poly = (csPolygon3D*)entity;
    const char* name = poly->GetName ();
    if (!name) name = "(NULL)";
    const char* pname = ((csPolygonSet*)poly->GetParent ())->GetName ();
    if (!pname) pname = "(NULL)";
    Sys->Printf (MSG_DEBUG_0, "%03d%sPolygon '%s/%s' ------\n",
    	dump_visible_indent, indent_spaces, pname, name);
    if (poly->GetPortal ())
      Sys->Printf (MSG_DEBUG_0, "%03d%s   | Polygon has a portal.\n",
        dump_visible_indent, indent_spaces);
    for (i = 0 ; i < poly->GetVertices ().GetNumVertices () ; i++)
    {
      csVector3& vw = poly->Vwor (i);
      csVector3& vc = poly->Vcam (i);
      Sys->Printf (MSG_DEBUG_0, "%03d%s   | %d: wor=(%f,%f,%f) cam=(%f,%f,%f)\n",
      	dump_visible_indent, indent_spaces, i, vw.x, vw.y, vw.z, vc.x, vc.y, vc.z);
    }
  }
  else if (type == CALLBACK_POLYGON2D)
  {
    csPolygon2D* poly = (csPolygon2D*)entity;
    Sys->Printf (MSG_DEBUG_0, "%03d%s2D Polygon ------\n", dump_visible_indent, indent_spaces);
    for (i = 0 ; i < poly->GetNumVertices () ; i++)
    {
      csVector2 v = *poly->GetVertex (i);
      Sys->Printf (MSG_DEBUG_0, "%03d%s   | %d: persp=(%f,%f)\n",
      	dump_visible_indent, indent_spaces, i, v.x, v.y);
    }
  }
  else if (type == CALLBACK_POLYGONQ)
  {
    // G3DPolygonDPQ* dpq = (G3DPolygonDPQ*)entity;
  }
  else if (type == CALLBACK_SECTOR)
  {
    csSector* sector = (csSector*)entity;
    const char* name = sector->GetName ();
    if (!name) name = "(NULL)";
    Sys->Printf (MSG_DEBUG_0, "%03d%s BEGIN Sector '%s' ------------\n",
    	dump_visible_indent+1, indent_spaces, name);
    for (i = 0 ; i < sector->GetNumVertices () ; i++)
    {
      csVector3& vw = sector->Vwor (i);
      csVector3& vc = sector->Vcam (i);
      Sys->Printf (MSG_DEBUG_0, "%03d%s   | %d: wor=(%f,%f,%f) cam=(%f,%f,%f)\n",
      	dump_visible_indent+1, indent_spaces, i, vw.x, vw.y, vw.z, vc.x, vc.y, vc.z);
    }
    dump_visible_indent++;
  }
  else if (type == CALLBACK_SECTOREXIT)
  {
    csSector* sector = (csSector*)entity;
    const char* name = sector->GetName ();
    if (!name) name = "(NULL)";
    Sys->Printf (MSG_DEBUG_0, "%03d%sEXIT Sector '%s' ------------\n",
    	dump_visible_indent, indent_spaces, name);
    dump_visible_indent--;
  }
  else if (type == CALLBACK_THING)
  {
    csThing* thing = (csThing*)entity;
    const char* name = thing->GetName ();
    if (!name) name = "(NULL)";
    Sys->Printf (MSG_DEBUG_0, "%03d%s BEGIN Thing '%s' ------------\n",
    	dump_visible_indent+1, indent_spaces, name);
    for (i = 0 ; i < thing->GetNumVertices () ; i++)
    {
      csVector3& vw = thing->Vwor (i);
      csVector3& vc = thing->Vcam (i);
      Sys->Printf (MSG_DEBUG_0, "%03d%s   | %d: wor=(%f,%f,%f) cam=(%f,%f,%f)\n",
      	dump_visible_indent+1, indent_spaces, i, vw.x, vw.y, vw.z, vc.x, vc.y, vc.z);
    }
    dump_visible_indent++;
  }
  else if (type == CALLBACK_THINGEXIT)
  {
    csThing* thing = (csThing*)entity;
    const char* name = thing->GetName ();
    if (!name) name = "(NULL)";
    Sys->Printf (MSG_DEBUG_0, "%03d%sEXIT Thing '%s' ------------\n",
    	dump_visible_indent, indent_spaces, name);
    dump_visible_indent--;
  }
}

//------------------------------------------------------------------------

//@@@
int covtree_level = 1;
bool do_covtree_dump = false;

void WalkTest::DrawFrame (time_t elapsed_time, time_t current_time)
{
  (void)elapsed_time; (void)current_time;

  //not used since we need WHITE background not black
  int drawflags = 0; /* do_clear ? CSDRAW_CLEARSCREEN : 0; */
  if (do_clear || map_mode == MAP_ON)
  {
    if (!Gfx3D->BeginDraw (CSDRAW_2DGRAPHICS))
      return;
    Gfx2D->Clear (map_mode == MAP_ON ? 0 : 255);
  }

  if (!System->Console->IsActive ()
   || ((csSimpleConsole*)(System->Console))->IsTransparent ())
  {
    // Tell Gfx3D we're going to display 3D things
    if (!Gfx3D->BeginDraw (drawflags | CSDRAW_3DGRAPHICS))
      return;

    // Advance sprite frames
    Sys->world->AdvanceSpriteFrames (current_time);

    // Apply lighting BEFORE the very first frame
    csDynLight* dyn = Sys->world->GetFirstDynLight ();
    while (dyn)
    {
      extern void HandleDynLight (csDynLight*);
      csDynLight* dn = dyn->GetNext ();
      if (dyn->GetChild (csDataObject::Type)) HandleDynLight (dyn);
      dyn = dn;
    }
    // Apply lighting to all sprites
    light_statics ();

    if (map_mode != MAP_ON && !do_covtree_dump) view->Draw ();
    // no need to clear screen anymore
    drawflags = 0;
  }

  // Start drawing 2D graphics
  if (!Gfx3D->BeginDraw (drawflags | CSDRAW_2DGRAPHICS))
    return;

  if (map_mode != MAP_OFF)
  {
    wf->GetWireframe ()->Clear ();
    view->GetWorld ()->DrawFunc (view->GetCamera (), view->GetClipper (), draw_map);
    wf->GetWireframe ()->Draw (Gfx3D, wf->GetCamera ());
  }
  else
  {
    if (do_show_z) DrawZbuffer ();
    if (do_show_palette) DrawPalette ();
    if (do_edges) view->GetWorld ()->DrawFunc (view->GetCamera (), view->GetClipper (), draw_edges);
    if (selected_polygon || selected_light) view->GetWorld ()->DrawFunc (view->GetCamera (), view->GetClipper (), draw_edges, (void*)1);
    if (do_light_frust && selected_light) ((csStatLight*)selected_light)->LightingFunc (show_frustrum);
  }

  csSimpleConsole* scon = (csSimpleConsole*)System->Console;
  scon->Print (NULL);

  if (!scon->IsActive ())
  {
    if (do_fps)
    {
      GfxWrite(11, FRAME_HEIGHT-11, 0, -1, "FPS=%f", timeFPS);
      GfxWrite(10, FRAME_HEIGHT-10, scon->get_fg (), -1, "FPS=%f", timeFPS);
    } /* endif */
    if (do_stats)
    {
      char buffer[50];
      sprintf (buffer, "pc=%d pd=%d po=%d pa=%d pr=%d", Stats::polygons_considered,
        Stats::polygons_drawn, Stats::portals_drawn, Stats::polygons_accepted,
	Stats::polygons_rejected);
      GfxWrite(FRAME_WIDTH-30*8-1, FRAME_HEIGHT-11, 0, -1, "%s", buffer);
      GfxWrite(FRAME_WIDTH-30*8, FRAME_HEIGHT-10, scon->get_fg (), -1, "%s", buffer);
    }
    else if (do_show_coord)
    {
      char buffer[100];
      sprintf (buffer, "%2.2f,%2.2f,%2.2f: %s",
        view->GetCamera ()->GetW2CTranslation ().x, view->GetCamera ()->GetW2CTranslation ().y,
        view->GetCamera ()->GetW2CTranslation ().z, view->GetCamera ()->GetSector()->GetName ());
      Gfx2D->Write(FRAME_WIDTH-24*8-1, FRAME_HEIGHT-11, 0, -1, buffer);
      Gfx2D->Write(FRAME_WIDTH-24*8, FRAME_HEIGHT-10, scon->get_fg (), -1, buffer);
    }

    if (cslogo)
    {
      unsigned w = cslogo->Width()  * FRAME_WIDTH  / 640;
      unsigned h = cslogo->Height() * FRAME_HEIGHT / 480;
      cslogo->Draw (Gfx2D, FRAME_WIDTH - 2 - (w * 151) / 256 , 2, w, h);
    }

    // White-board for debugging purposes.
    if (do_covtree_dump)
    {
      csCoverageMaskTree* covtree = Sys->world->GetCovtree ();
      csSolidBsp* solidbsp = Sys->world->GetSolidBsp ();
      if (solidbsp)
      {
#	if 1
        Gfx2D->Clear (0);
	solidbsp->GfxDump (Gfx2D, Gfx3D->GetTextureManager (), covtree_level);
	extern csPolygon2D debug_poly2d;
	debug_poly2d.Draw (Gfx2D, Gfx3D->GetTextureManager ()->FindRGB (0, 255, 0));
#	elif 0
        Gfx2D->Clear (0);
	solidbsp->MakeEmpty ();
	csCamera* c = view->GetCamera ();
	csPolygon2D poly1, poly2, poly3, poly4;
	poly1.AddPerspective (c->Other2This (csVector3 (-1.6, 1, 5)));
	poly1.AddPerspective (c->Other2This (csVector3 (1, 1.6, 5)));
	poly1.AddPerspective (c->Other2This (csVector3 (1, -1, 5)));
	poly1.AddPerspective (c->Other2This (csVector3 (-1, -1.3, 5)));
	//solidbsp->InsertPolygon (poly1.GetVertices (),
		//poly1.GetNumVertices ());
	poly2.AddPerspective (c->Other2This (csVector3 (-1.5, .5, 6)));
	poly2.AddPerspective (c->Other2This (csVector3 (.5, .5, 6)));
	poly2.AddPerspective (c->Other2This (csVector3 (.5, -1.5, 6)));
	poly2.AddPerspective (c->Other2This (csVector3 (-1.5, -1.5, 6)));
	printf ("T2:%d ", solidbsp->TestPolygon (poly2.GetVertices (),
		poly2.GetNumVertices ()));
	//printf ("P2:%d ", solidbsp->InsertPolygon (poly2.GetVertices (),
		//poly2.GetNumVertices ()));
	poly3.AddPerspective (c->Other2This (csVector3 (-.5, .15, 7)));
	poly3.AddPerspective (c->Other2This (csVector3 (1.5, .15, 7)));
	poly3.AddPerspective (c->Other2This (csVector3 (1.5, -.5, 7)));
	printf ("T3:%d ", solidbsp->TestPolygon (poly3.GetVertices (),
		poly3.GetNumVertices ()));
	printf ("P3:%d ", solidbsp->InsertPolygon (poly3.GetVertices (),
		poly3.GetNumVertices ()));
	poly4.AddPerspective (c->Other2This (csVector3 (1.5, -.5, 7)));
	poly4.AddPerspective (c->Other2This (csVector3 (1.5, .15, 7)));
	poly4.AddPerspective (c->Other2This (csVector3 (2.5, .15, 7)));
	printf ("T4:%d ", solidbsp->TestPolygon (poly4.GetVertices (),
		poly4.GetNumVertices ()));
	printf ("P4:%d\n", solidbsp->InsertPolygon (poly4.GetVertices (),
		poly4.GetNumVertices ()));
	//poly1.Draw (Gfx2D, Gfx3D->GetTextureManager ()->FindRGB (0, 100, 100));
	//poly2.Draw (Gfx2D, Gfx3D->GetTextureManager ()->FindRGB (100, 0, 100));
	poly3.Draw (Gfx2D, Gfx3D->GetTextureManager ()->FindRGB (100, 100, 0));
	poly4.Draw (Gfx2D, Gfx3D->GetTextureManager ()->FindRGB (100, 100, 100));
	solidbsp->GfxDump (Gfx2D, Gfx3D->GetTextureManager (), covtree_level);
#	endif
      }
      else if (covtree)
      {
        Gfx2D->Clear (0);
#	if 0
	covtree->GfxDump (Gfx2D, covtree_level);
#	else
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
#	endif
      }
    }

  } /* endif */

  // Drawing code ends here
  Gfx3D->FinishDraw ();
  // Print the output.
  Gfx3D->Print (NULL);
}

int cnt = 1;
time_t time0 = (time_t)-1;

int FindIntersection(csCdTriangle *t1,csCdTriangle *t2,csVector3 line[2])
{
  csVector3 tri1[3]; tri1[0]=t1->p1; tri1[1]=t1->p2; tri1[2]=t1->p3;
  csVector3 tri2[3]; tri2[0]=t2->p1; tri2[1]=t2->p2; tri2[2]=t2->p3;

  return csMath3::FindIntersection(tri1,tri2,line);
}

// Define the player bounding box.
// The camera's lens or person's eye is assumed to be
// at 0,0,0.  The height (DY), width (DX) and depth (DZ).
// Is the size of the camera/person and the origin
// coordinates (OX,OY,OZ) locate the bbox with respect to the eye.
// This player is 1.8 metres tall (assuming 1cs unit = 1m) (6feet)
#define DX    cfg_body_width
#define DY    cfg_body_height
#define DZ    cfg_body_depth
#define OY    Sys->cfg_eye_offset

#define DX_L  cfg_legs_width
#define DZ_L  cfg_legs_depth

#define DX_2  (DX/2)
#define DZ_2  (DZ/2)

#define DX_2L (DX_L/2)
#define DZ_2L (DZ_L/2)

#define OYL  Sys->cfg_legs_offset
#define DYL  (OY-OYL)

void WalkTest::CreateColliders (void)
{
  csPolygon3D *p;
  CHK (csPolygonSet *pb = new csPolygonSet ());
  pb->SetName ("Player's Body");

  pb->AddVertex(-DX_2, OY,    -DZ_2);
  pb->AddVertex(-DX_2, OY,    DZ_2);
  pb->AddVertex(-DX_2, OY+DY, DZ_2);
  pb->AddVertex(-DX_2, OY+DY, -DZ_2);
  pb->AddVertex(DX_2,  OY,    -DZ_2);
  pb->AddVertex(DX_2,  OY,    DZ_2);
  pb->AddVertex(DX_2,  OY+DY, DZ_2);
  pb->AddVertex(DX_2,  OY+DY, -DZ_2);

  // Left
  p = pb->NewPolygon (0);

  p->AddVertex (0); p->AddVertex (1);
  p->AddVertex (2); p->AddVertex (3);

  // Right
  p = pb->NewPolygon (0);
  p->AddVertex (4); p->AddVertex (5);
  p->AddVertex (6); p->AddVertex (7);

  // Bottom
  p = pb->NewPolygon (0);
  p->AddVertex (0); p->AddVertex (1);
  p->AddVertex (5); p->AddVertex (4);

  // Top
  p = pb->NewPolygon (0);
  p->AddVertex (3); p->AddVertex (2);
  p->AddVertex (6); p->AddVertex (7);

  // Front
  p = pb->NewPolygon (0);
  p->AddVertex (1); p->AddVertex (5);
  p->AddVertex (6); p->AddVertex (2);

  // Back
  p = pb->NewPolygon (0);
  p->AddVertex (0); p->AddVertex (4);
  p->AddVertex (7); p->AddVertex (3);

  CHK (body = new csRAPIDCollider (pb));
//  CHK (delete pb);

  CHK (csPolygonSet *pl = new csPolygonSet ());

  pl->AddVertex(-DX_2L, OYL,     -DZ_2L);
  pl->AddVertex(-DX_2L, OYL,     DZ_2L);
  pl->AddVertex(-DX_2L, OYL+DYL, DZ_2L);
  pl->AddVertex(-DX_2L, OYL+DYL, -DZ_2L);
  pl->AddVertex(DX_2L,  OYL,     -DZ_2L);
  pl->AddVertex(DX_2L,  OYL,     DZ_2L);
  pl->AddVertex(DX_2L,  OYL+DYL, DZ_2L);
  pl->AddVertex(DX_2L,  OYL+DYL, -DZ_2L);

  // Left
  p = pl->NewPolygon (0);

  p->AddVertex (0); p->AddVertex (1);
  p->AddVertex (2); p->AddVertex (3);

  // Right
  p = pl->NewPolygon (0);
  p->AddVertex (4); p->AddVertex (5);
  p->AddVertex (6); p->AddVertex (7);

  // Bottom
  p = pl->NewPolygon (0);
  p->AddVertex (0); p->AddVertex (1);
  p->AddVertex (5); p->AddVertex (4);

  // Top
  p = pl->NewPolygon (0);
  p->AddVertex (3); p->AddVertex (2);
  p->AddVertex (6); p->AddVertex (7);

  // Front
  p = pl->NewPolygon (0);
  p->AddVertex (1); p->AddVertex (5);
  p->AddVertex (6); p->AddVertex (2);

  // Back
  p = pl->NewPolygon (0);
  p->AddVertex (0); p->AddVertex (4);
  p->AddVertex (7); p->AddVertex (3);

  CHK (legs = new csRAPIDCollider(pl));
//  CHK (delete pl);

  if (!body || !legs)
    do_cd = false;
}

#define MAXSECTORSOCCUPIED  20

// No more than 1000 collisions ;)
collision_pair our_cd_contact[1000];
int num_our_cd;

int FindSectors (csVector3 v, csVector3 d, csSector *s, csSector **sa)
{
  sa[0] = s;
  int i, c = 1;
  float size = d.x * d.x + d.y * d.y + d.z * d.z;
  for(i = 0;i < s->GetNumPolygons() && c < MAXSECTORSOCCUPIED; i++)
  {
    // Get the polygon of for this sector.
    csPolygon3D* p = (csPolygon3D*) s->GetPolygon (i);
    csPortal* portal = p->GetPortal ();
    // Handle only portals.
    if (portal != NULL)
    {
      if (p->GetPlane ()->SquaredDistance (v) < size)
      {
        if (Sys->do_infinite && !portal->GetSector ())
	{
	  ((InfPortalCS*)portal)->ConnectNewSector ();
	}
        sa[c] = portal->GetSector ();
        c++;
      }
    }
  }
  return c;
}

int CollisionDetect (csRAPIDCollider *c, csSector* sp, csTransform *cdt)
{
  int hit = 0;

  // Check collision with this sector.
  csRAPIDCollider::CollideReset();
  if (c->Collide(*sp, cdt)) hit++;
  collision_pair *CD_contact = csRAPIDCollider::GetCollisions ();

  for (int i=0 ; i<csRAPIDCollider::numHits ; i++)
    our_cd_contact[num_our_cd++] = CD_contact[i];

  if (csRAPIDCollider::GetFirstHit() && hit)
    return 1;

  // Check collision with the things in this sector.
  csThing *tp = sp->GetFirstThing ();
  while (tp)
  {
    // TODO, if and when Things can move, their transform must be passed in.
    csRAPIDCollider::numHits = 0;
    if (c->Collide(*tp, cdt)) hit++;

    CD_contact = csRAPIDCollider::GetCollisions ();
    for (int i=0 ; i<csRAPIDCollider::numHits ; i++)
      our_cd_contact[num_our_cd++] = CD_contact[i];

    if (csRAPIDCollider::GetFirstHit() && hit)
      return 1;
    tp = (csThing*)(tp->GetNext ());
    // TODO, should test which one is the closest.
  }

  return hit;
}

void DoGravity (csVector3& pos, csVector3& vel)
{
  pos=Sys->view->GetCamera ()->GetOrigin ();

  csVector3 new_pos = pos+vel;
  csMatrix3 m;
  csOrthoTransform test (m, new_pos);

  csSector *n[MAXSECTORSOCCUPIED];
  int num_sectors = FindSectors (new_pos, 4*Sys->body->GetRadius(),
    Sys->view->GetCamera()->GetSector(), n);

  num_our_cd = 0;
  csRAPIDCollider::SetFirstHit (false);
  int hits = 0;

  // Check to see if there are any terrains, if so test against those.
  // This routine will automatically adjust the transform to the highest
  // terrain at this point.
  int k;
  for ( k = 0; k < num_sectors ; k++)
  {
    if (n[k]->terrains.Length () > 0)
    {
      int i;
      for (i = 0 ; i < n[k]->terrains.Length () ; i++)
      {
	csTerrain* terrain = (csTerrain*)n[k]->terrains[i];
	hits += terrain->CollisionDetect (&test);
      }
    }
    if (hits)
    {
      new_pos = test.GetOrigin ();
    }
  }
  if (hits == 0)
  {
    csRAPIDCollider::CollideReset ();

    for ( ; num_sectors-- ; )
      hits += CollisionDetect (Sys->body, n[num_sectors], &test);

    for (int j=0 ; j<hits ; j++)
    {
      csCdTriangle *wall = our_cd_contact[j].tr2;
      csVector3 n = ((wall->p3-wall->p2)%(wall->p2-wall->p1)).Unit();
      if (n*vel<0)
        continue;
      vel = -(vel%n)%n;
    }

    // We now know our (possible) velocity. Let's try to move up or down, if possible
    new_pos = pos+vel;
    test = csOrthoTransform (csMatrix3(), new_pos);

    num_sectors = FindSectors (new_pos, 4*Sys->legs->GetRadius(), 
		Sys->view->GetCamera()->GetSector(), n);

    num_our_cd = 0;
    csRAPIDCollider::SetFirstHit (false);
    csRAPIDCollider::numHits = 0;
    int hit = 0;

    csRAPIDCollider::CollideReset ();

    for ( ; num_sectors-- ; )
      hit += CollisionDetect (Sys->legs, n[num_sectors], &test);
    
    if (!hit)
    {
      Sys->on_ground = false;
      if (Sys->do_gravity && !Sys->move_3d)
	vel.y -= 0.004;
    }
    else
    {
      float max_y=-1e10;
      
      for (int j=0 ; j<hit ; j++)
      {
	csCdTriangle first  = *our_cd_contact[j].tr1;
	csCdTriangle second = *our_cd_contact[j].tr2;

	csVector3 n=((second.p3-second.p2)%(second.p2-second.p1)).Unit ();

	if (n*csVector3(0,-1,0)<0.7) continue;

	csVector3 line[2];

	first.p1 += new_pos;
	first.p2 += new_pos;
	first.p3 += new_pos;

	if (FindIntersection (&first,&second,line))
	{
	  if (line[0].y>max_y)
	    max_y=line[0].y;
	  if (line[1].y>max_y)
	    max_y=line[1].y;
	}
      }

      float p = new_pos.y-max_y+OYL+0.01;
      if (fabs(p)<DYL-0.01)
      {
	if (max_y != -1e10)
	  new_pos.y = max_y-OYL-0.01;

	if (vel.y<0)
	  vel.y = 0;
      }
      Sys->on_ground = true;
    }
  }
  new_pos -= Sys->view->GetCamera ()->GetOrigin ();
  if (Sys->world->sectors.Length()==1)
  {
    //This is a workaround until sector->FollowSegment() will use the
    //octree system. In worlds created by map2cs, walking will be
    //very slow, because of the search for a new sector. This is
    //not really necessarey, because there is only 1 sector. This
    //optimisation here is sane in all cases, so it won't hurt anyway.
    Sys->view->GetCamera ()->MoveWorldUnrestricted(new_pos);
  }
  else
  {
    Sys->view->GetCamera ()->MoveWorld (new_pos);
  }

  Sys->velocity = Sys->view->GetCamera ()->GetO2T ()*vel;

  if(!Sys->do_gravity)
    Sys->velocity.y -= SIGN (Sys->velocity.y) * MIN (0.017, fabs (Sys->velocity.y));
}

void WalkTest::PrepareFrame (time_t elapsed_time, time_t current_time)
{
  (void)elapsed_time; (void)current_time;

  CLights::LightIdle (); // SJI

  if (do_cd)
  {
    if (!player_spawned)
    {
      CreateColliders ();
      player_spawned=true;
    }

    for (int repeats = 0 ; repeats < (elapsed_time/25.0 + 0.5); repeats++)
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

  if (cnt <= 0)
  {
    time_t time1 = SysGetTime ();
    if (time0 != (time_t)-1)
    {
      if (time1 != time0)
        timeFPS = 10000.0f / (float)(time1 - time0);
    }
    cnt = 10;
    time0 = SysGetTime ();
  }
  cnt--;

  layer->step_run ();
}

void perf_test ()
{
  Sys->busy_perf_test = true;
  time_t t1, t2, t;
  Sys->Printf (MSG_CONSOLE, "Performance test busy...\n");
  t = t1 = SysGetTime ();
  int i;
  for (i = 0 ; i < 100 ; i++)
  {
    Sys->layer->step_run ();
    Sys->DrawFrame (SysGetTime ()-t, SysGetTime ());
    t = SysGetTime ();
  }
  t2 = SysGetTime ();
  Sys->Printf (MSG_CONSOLE, "%f secs to render 100 frames: %f fps\n",
        (float)(t2-t1)/1000., 100000./(float)(t2-t1));
  Sys->Printf (MSG_DEBUG_0, "%f secs to render 100 frames: %f fps\n",
        (float)(t2-t1)/1000., 100000./(float)(t2-t1));
  cnt = 1;
  time0 = (time_t)-1;
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
  SaveCamera ("coord.bug");
  Sys->Printf (MSG_DEBUG_0, "Camera saved in coord.bug\n");
  Dumper::dump (Sys->view->GetCamera ());
  Sys->Printf (MSG_DEBUG_0, "Camera dumped in debug.txt\n");
  Dumper::dump (Sys->world);
  Sys->Printf (MSG_DEBUG_0, "World dumped in debug.txt\n");
}

/*
 * A sample script which just prints a message on screen.
 */
bool do_message_script (IntRunScript* sc, char* data)
{
  sc->get_layer ()->message (data);
  return true;
}

//---------------------------------------------------------------------------


void cleanup ()
{
  Sys->console_out ("Cleaning up...\n");
  free_keymap ();
  Sys->EndWorld ();
  CHK (delete Sys); Sys = NULL;
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

  txtmgr->SetPalette ();

  ((csSimpleConsole *)System->Console)->SetupColors (txtmgr);
  ((csSimpleConsole *)System->Console)->SetMaxLines (1000);       // Some arbitrary high value.
  ((csSimpleConsole *)System->Console)->SetTransparent (0);

  System->ConsoleReady = true;
}

void WalkTest::EndWorld ()
{
  CHK (delete view); view = NULL;
}

void WalkTest::InitWorld (csWorld* world, csCamera* /*camera*/)
{
  Sys->Printf (MSG_INITIALIZATION, "Computing OBBs ...\n");

  int sn = world->sectors.Length ();
  while (sn > 0)
  {
    sn--;
    csSector* sp = (csSector*)world->sectors[sn];
    // Initialize the sector itself.
    CHK((void)new csRAPIDCollider(*sp, sp));
    // Initialize the things in this sector.
    csThing* tp = sp->GetFirstThing ();
    while (tp)
    {
      CHK((void)new csRAPIDCollider(*tp, tp));
      tp = (csThing*)(tp->GetNext ());
    }
  }
  // Initialize all sprites for collision detection.
  // @@@ This routine ignores 2D sprites for the moment.
  csSprite3D* spp;
  int i;
  for (i = 0 ; i < world->sprites.Length () ; i++)
  {
    csSprite* sp = (csSprite*)world->sprites[i];
    if (sp->GetType () != csSprite3D::Type) continue;
    spp = (csSprite3D*)sp;

    // TODO: Should create beings for these.
    CHK((void)new csRAPIDCollider(*spp, spp));
  }

  // Create a player object that follows the camera around.
//  player = csBeing::PlayerSpawn("Player");

//  init = true;
//  Sys->Printf (MSG_INITIALIZATION, "DONE\n");
}


bool WalkTest::Initialize (int argc, const char* const argv[], const char *iConfigName)
{
  if (!SysSystemDriver::Initialize (argc, argv, iConfigName))
    return false;

  // Get all collision detection and movement config file parameters.
  cfg_jumpspeed = Config->GetFloat ("CD", "JUMPSPEED", 0.08);
  cfg_walk_accelerate = Config->GetFloat ("CD", "WALKACCELERATE", 0.007);
  cfg_walk_maxspeed = Config->GetFloat ("CD", "WALKMAXSPEED", 0.1);
  cfg_walk_brake = Config->GetFloat ("CD", "WALKBRAKE", 0.014);
  cfg_rotate_accelerate = Config->GetFloat ("CD", "ROTATEACCELERATE", 0.005);
  cfg_rotate_maxspeed = Config->GetFloat ("CD", "ROTATEMAXSPEED", 0.03);
  cfg_rotate_brake = Config->GetFloat ("CD", "ROTATEBRAKE", 0.015);
  cfg_look_accelerate = Config->GetFloat ("CD", "LOOKACCELERATE", 0.028);
  cfg_body_height = Config->GetFloat ("CD", "BODYHEIGHT", 1.4);
  cfg_body_width = Config->GetFloat ("CD", "BODYWIDTH", 0.5);
  cfg_body_depth = Config->GetFloat ("CD", "BODYDEPTH", 0.5);
  cfg_eye_offset = Config->GetFloat ("CD", "EYEOFFSET", -0.7);
  cfg_legs_width = Config->GetFloat ("CD", "LEGSWIDTH", 0.4);
  cfg_legs_depth = Config->GetFloat ("CD", "LEGSDEPTH", 0.4);
  cfg_legs_offset = Config->GetFloat ("CD", "LEGSOFFSET", -1.1);

  //--- create the converter class for testing
  CHK (ImportExport = new converter());
  // process import/export files from config and print log for testing
  ImportExport->ProcessConfig (Config);
  // free memory - delete this if you want to use the data in the buffer
  CHK (delete ImportExport);
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

  // Create console object for text and commands.
  CHK (System->Console = new csSimpleConsole (Config, Command::SharedInstance ()));

  // Open the startup console
  start_console ();

  // Some commercials...
  Printf (MSG_INITIALIZATION, "Crystal Space version %s (%s).\n", VERSION, RELEASE_DATE);
  Printf (MSG_INITIALIZATION, "Created by Jorrit Tyberghein and others...\n\n");

  // Set texture manager mode to verbose (useful for debugging)
  iTextureManager* txtmgr = Gfx3D->GetTextureManager ();
  txtmgr->SetVerbose (true);

  // Find the world plugin and query the csWorld object from it...
  World = QUERY_PLUGIN (Sys, iWorld);
  if (!World)
  {
    Printf (MSG_FATAL_ERROR, "No iWorld plugin!\n");
    return false;
  }
  world = World->GetCsWorld ();

  // csView is a view encapsulating both a camera and a clipper.
  // You don't have to use csView as you can do the same by
  // manually creating a camera and a clipper but it makes things a little
  // easier.
  CHK (view = new csView (world, Gfx3D));

  // Initialize the command processor with the world and camera.
  Command::Initialize (world, view->GetCamera (), Gfx3D, System->Console, System);

  // Create the language layer needed for scripting.
  // Also register a small C script so that it can be used
  // by levels. large.zip uses this script.
  CHK (layer = new LanguageLayer (world, view->GetCamera ()));
  int_script_reg.reg ("message", &do_message_script);

  // Now we have two choices. Either we create an infinite
  // maze (random). This happens when the '-infinite' commandline
  // option is given. Otherwise we load the given world.
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
    world->EnableLightingCache (false);

    // Unfortunately the current movement system does not allow the user to
    // move around the maze unless collision detection is enabled, even
    // though collision detection does not really make sense in this context.
    // Hopefully the movement system will be fixed some day so that the user
    // can move around even with collision detection disabled.
    do_cd = true;

    // Load two textures that are used in the maze.
    csLoader::LoadTexture (world, "txt", "/lib/std/stone4.gif");
    csLoader::LoadTexture (world, "txt2", "/lib/std/mystone2.gif");

    if (do_infinite)
    {
      // Create the initial (non-random) part of the maze.
      CHK (infinite_maze = new InfiniteMaze ());
      room = infinite_maze->create_six_room (world, 0, 0, 0)->sector;
      infinite_maze->create_six_room (world, 0, 0, 1);
      infinite_maze->create_six_room (world, 0, 0, 2);
      infinite_maze->create_six_room (world, 1, 0, 2);
      infinite_maze->create_six_room (world, 0, 1, 2);
      infinite_maze->create_six_room (world, 1, 1, 2);
      infinite_maze->create_six_room (world, 0, 0, 3);
      infinite_maze->create_six_room (world, 0, 0, 4);
      infinite_maze->create_six_room (world, -1, 0, 4);
      infinite_maze->create_six_room (world, -2, 0, 4);
      infinite_maze->create_six_room (world, 0, -1, 3);
      infinite_maze->create_six_room (world, 0, -2, 3);
      infinite_maze->create_six_room (world, 0, 1, 3);
      infinite_maze->create_six_room (world, 0, 2, 3);
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
      CHK (huge_room = new HugeRoom ());
      room = huge_room->create_huge_world (world);
    }

    // Prepare the world. This will calculate all lighting and
    // prepare the lightmaps for the 3D rasterizer.
    world->Prepare ();
  }
  else
  {
    // Load from a world file.
    Printf (MSG_INITIALIZATION, "Loading world '%s'...\n", world_dir);

    // Check the world and mount it if required
    char tmp [100];
    sprintf (tmp, "%s/", world_dir);
    if (!VFS->Exists (world_dir))
    {
      char *name = strrchr (world_dir, '/');
      if (name)
      {
        name++;
        sprintf (tmp, "$.$/data$/%s.zip, $.$/%s.zip, $(..)$/data$/%s.zip",
          name, name, name);
        VFS->Mount (world_dir, tmp);
      }
    }

    if (!VFS->ChDir (world_dir))
    {
      Printf (MSG_FATAL_ERROR, "The directory on VFS for world file does not exist!\n");
      return false;
    }

    // Load the world from the file.
    if (!csLoader::LoadWorldFile (world, layer, "world"))
    {
      Printf (MSG_FATAL_ERROR, "Loading of world failed!\n");
      return false;
    }

    // Load the "standard" library
    csLoader::LoadLibraryFile (world, "/lib/std/library");

    // Find the Crystal Space logo and set the renderer Flag to for_2d, to allow
    // the use in the 2D part.
    csTextureList *texlist = world->GetTextures ();
    csTextureHandle *texh = texlist->FindByName ("cslogo.gif");
    if (texh) texh->flags = CS_TEXTURE_2D;

    // Prepare the world. This will calculate all lighting and
    // prepare the lightmaps for the 3D rasterizer.
    world->Prepare ();

    // Create a 2D sprite for the Logo.
    if (texh)
    {
      int w, h;
      iTextureHandle* phTex = texh->GetTextureHandle();
      phTex->GetMipMapDimensions (0, w, h);
      CHK (cslogo = new csPixmap (phTex, 0, 0, w, h));
    }

    // Look for the start sector in this world.
    char* strt = (char*)(world->start_sector ? world->start_sector : "room");
    room = (csSector*)world->sectors.FindByName (strt);
    if (!room)
    {
      Printf (MSG_FATAL_ERROR,
          "World file does not contain a room called '%s' which is used\nas a starting point!\n",
	  strt);
      return false;
    }
  }

  // Initialize collision detection system (even if disabled so that we can enable it later).
  InitWorld (world, view->GetCamera ());

  // Create a wireframe object which will be used for debugging.
  CHK (wf = new csWireFrameCam (txtmgr));

  // Load a few sounds.
#ifdef DO_SOUND
  csSoundData* w = csSoundDataObject::GetSound(*world, "tada.wav");
  if (w && Sound) Sound->PlayEphemeral (w);

  wMissile_boom = csSoundDataObject::GetSound(*world, "boom.wav");
  wMissile_whoosh = csSoundDataObject::GetSound(*world, "whoosh.wav");
#endif

  Printf (MSG_INITIALIZATION, "--------------------------------------\n");

  // Wait one second before starting.
  long t = Sys->Time ()+1000;
  while (Sys->Time () < t) ;

  // Allocate the palette as calculated by the texture manager.
  txtmgr->SetPalette ();

  // Reinit console object for 3D engine use.
  ((csSimpleConsole *)System->Console)->SetupColors (txtmgr);
  ((csSimpleConsole *)System->Console)->SetMaxLines ();
  ((csSimpleConsole *)System->Console)->SetTransparent ();
  System->Console->Clear ();

  // Initialize our 3D view.
  view->SetSector (room);
  view->GetCamera ()->SetPosition (world->start_vec);
  // We use the width and height from the 3D renderer because this
  // can be different from the frame size (rendering in less res than
  // real window for example).
  int w3d = Gfx3D->GetWidth ();
  int h3d = Gfx3D->GetHeight ();
  view->SetRectangle (2, 2, w3d - 4, h3d - 4);

  return true;
}

/*---------------------------------------------------------------------*
 * Main function
 *---------------------------------------------------------------------*/
int main (int argc, char* argv[])
{
  // Initialize the random number generator
  srand (time (NULL));

  // Create the system driver object
  CHK (Sys = new WalkTest ());

  // Initialize the main system. This will load all needed plugins
  // (3D, 2D, network, sound, ..., engine) and initialize them.
  if (!Sys->Initialize (argc, argv, "cryst.cfg"))
  {
    Sys->Printf (MSG_FATAL_ERROR, "Error initializing system!\n");
    fatal_exit (-1, false);
  }

  // Start the 'autoexec.cfg' script and fully execute it.
  Command::start_script ("autoexec.cfg");
  char cmd_buf[512];
  while (Command::get_script_line (cmd_buf, 511))
    Command::perform_line (cmd_buf);

  // If there is another script given on the commandline
  // start it but do not execute it yet. This will be done
  // frame by frame.
  if (Sys->auto_script)
    Command::start_script (Sys->auto_script);

  // The main loop.
  Sys->Loop ();

  cleanup ();

  return 1;
}

#if 0
void* operator new (size_t s)
{
  printf ("Alloc with size %d\n", s);
  if (Sys && Sys->do_edges && s > 100)
  {
    int a;
    a=1;
  }
  return (void*)malloc (s);
}
#endif

