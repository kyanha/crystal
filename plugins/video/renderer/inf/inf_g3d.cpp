/*
    Copyright (C) 2000 by Jorrit Tyberghein

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

#include "cssysdef.h"
#include "qint.h"
#include "csgeom/math2d.h"
#include "csgeom/math3d.h"
#include "csgeom/polyclip.h"
#include "video/renderer/inf/inf_g3d.h"
#include "video/renderer/inf/inf_txt.h"
#include "csutil/inifile.h"
#include "ipolygon.h"
#include "isystem.h"
#include "igraph2d.h"
#include "ilghtmap.h"

#define SysPrintf System->Printf

///---------------------------------------------------------------------------

IMPLEMENT_FACTORY (csGraphics3DInfinite)
IMPLEMENT_FACTORY (csGraphics2DInfinite)

EXPORT_CLASS_TABLE (inf3d)
  EXPORT_CLASS (csGraphics3DInfinite, "crystalspace.graphics3d.infinite",
    "Infinite 3D graphics driver for Crystal Space")
  EXPORT_CLASS (csGraphics2DInfinite, "crystalspace.graphics2d.infinite",
    "Infinite 2D graphics driver for Crystal Space")
EXPORT_CLASS_TABLE_END

IMPLEMENT_IBASE (csGraphics3DInfinite)
  IMPLEMENTS_INTERFACE (iPlugIn)
  IMPLEMENTS_INTERFACE (iGraphics3D)
  IMPLEMENTS_EMBEDDED_INTERFACE (iConfig)
IMPLEMENT_IBASE_END

csGraphics3DInfinite::csGraphics3DInfinite (iBase *iParent) :
  G2D (NULL), config (NULL)
{
  CONSTRUCT_IBASE (iParent);
  CONSTRUCT_EMBEDDED_IBASE (scfiConfig);

  clipper = NULL;
  texman = NULL;

  Caps.CanClip = false;
  Caps.minTexHeight = 2;
  Caps.minTexWidth = 2;
  Caps.maxTexHeight = 1024;
  Caps.maxTexWidth = 1024;
  Caps.fog = G3DFOGMETHOD_NONE;
  Caps.NeedsPO2Maps = false;
  Caps.MaxAspectRatio = 32768;

  num_frames = 0;
  num_drawpolygon = 0;
  num_drawpolygonfx = 0;
  num_drawpolygon_mesh = 0;
  num_drawpolygonfx_mesh = 0;
  num_drawtrianglemesh = 0;
  num_drawpolymesh = 0;
  total_2d_time = 0;
  total_3d_time = 0;
  total_none_time = 0;
  total_time = 0;
  in_mesh = false;
  do_overdraw = false;
  do_fastmesh = false;
  pixels_drawn = 0;
  pixels_drawn_fx = 0;
}

csGraphics3DInfinite::~csGraphics3DInfinite ()
{
  Close ();
  delete texman;
  if (G2D) G2D->DecRef ();
  delete config;
}

bool csGraphics3DInfinite::Initialize (iSystem *iSys)
{
  System = iSys;

  iVFS* v = QUERY_PLUGIN_ID (System, CS_FUNCID_VFS, iVFS);
  config = new csIniFile (v, "/config/inf3d.cfg");
  v->DecRef(); v = NULL;

  width = height = -1;

  G2D = LOAD_PLUGIN (System, "crystalspace.graphics2d.infinite", NULL, iGraphics2D);
  if (!G2D)
    return false;

  texman = new csTextureManagerInfinite (System, G2D, config);

  return true;
}

bool csGraphics3DInfinite::Open (const char *Title)
{
  DrawMode = 0;

  if (!G2D->Open (Title))
  {
    SysPrintf (MSG_FATAL_ERROR, "Error opening Graphics2D context.\n");
    // set "not opened" flag
    width = height = -1;

    return false;
  }

  int nWidth = G2D->GetWidth ();
  int nHeight = G2D->GetHeight ();

  csPixelFormat pfmt = *G2D->GetPixelFormat ();
  texman->SetPixelFormat (pfmt);

  SetDimensions (nWidth, nHeight);

  SysPrintf (MSG_INITIALIZATION, "Using virtual mode %dx%d.\n", width, height);

  z_buf_mode = CS_ZBUF_NONE;

  return true;
}

void csGraphics3DInfinite::Close()
{
  printf ("=============================\n");
  printf ("Rendered frames: %d\n", num_frames);
  printf ("# DrawPolygon: %d (outside DPM)\n", num_drawpolygon);
  printf ("# DrawPolygonFX: %d (outside DTM)\n", num_drawpolygonfx);
  printf ("# DrawTriangleMesh: %d\n", num_drawtrianglemesh);
  printf ("   # DrawPolygonFX: %d (inside DTM)\n", num_drawpolygonfx_mesh);
  printf ("# DrawPolygonMesh: %d\n", num_drawpolymesh);
  printf ("   # DrawPolygon: %d (inside DPM)\n", num_drawpolygon_mesh);
  printf ("Time spend in 3D rendering: %ld\n", total_3d_time);
  printf ("Time spend in 2D rendering: %ld\n", total_2d_time);
  printf ("Time spend while not rendering: %ld\n", total_none_time);
  printf ("Total time : %ld\n", total_time);
  printf ("Average FPS: %f\n", 1000. * (float)num_frames / (float)total_time);
  if (do_overdraw)
  {
    float tot_pixels = screen_pixels * (float)num_frames;
    printf ("Overdraw ratio (DrawPolygon): %f\n",
    	(pixels_drawn + pixels_drawn_fx) / tot_pixels);
    printf ("   Pixels DrawPolygon: %d\n", (int)pixels_drawn);
    printf ("   Pixels DrawPolygonFX: %d\n", (int)pixels_drawn_fx);
  }

  if ((width == height) && (width == -1))
    return;

  G2D->Close ();
  width = height = -1;
}

void csGraphics3DInfinite::SetDimensions (int nwidth, int nheight)
{
  width = nwidth;
  height = nheight;
  width2 = width/2;
  height2 = height/2;
  screen_pixels = (float)width * (float)height;
}

void csGraphics3DInfinite::SetPerspectiveCenter (int x, int y)
{
  width2 = x;
  height2 = y;
}

void csGraphics3DInfinite::SetClipper (csVector2* vertices, int num_vertices)
{
  delete clipper;
  clipper = NULL;
  if (!vertices) return;
  // @@@ This could be better! We are using a general polygon clipper
  // even in cases where a box clipper would be better. We should
  // have a special SetBoxClipper call in iGraphics3D.
  clipper = new csPolygonClipper (vertices, num_vertices, false, true);
}

void csGraphics3DInfinite::GetClipper (csVector2* vertices, int& num_vertices)
{
  if (!clipper) { num_vertices = 0; return; }
  num_vertices = clipper->GetNumVertices ();
  csVector2* clip_verts = clipper->GetClipPoly ();
  int i;
  for (i = 0 ; i < num_vertices ; i++)
    vertices[i] = clip_verts[i];
}

long csGraphics3DInfinite::GetAccurateTime ()
{
  return System->GetTime ();
}

static cs_time start2d = 0;
static cs_time start3d = 0;
static cs_time startnone = 0;
static cs_time startfirst = 0;

bool csGraphics3DInfinite::BeginDraw (int DrawFlags)
{
  if (startfirst == 0) startfirst = GetAccurateTime ();

  cs_time endnone = GetAccurateTime ();
  if (startnone != 0)
  {
    total_none_time += endnone-startnone;
    startnone = 0;
  }
  
  if ((DrawFlags & CSDRAW_3DGRAPHICS) && !(DrawMode & CSDRAW_3DGRAPHICS))
  {
    start3d = GetAccurateTime ();
  }
  else if (!(DrawFlags & CSDRAW_3DGRAPHICS) && (DrawMode & CSDRAW_3DGRAPHICS))
  {
    cs_time end3d = GetAccurateTime ();
    total_3d_time += end3d-start3d;
  }

  if ((DrawFlags & CSDRAW_2DGRAPHICS) && !(DrawMode & CSDRAW_2DGRAPHICS))
  {
    start2d = GetAccurateTime ();
  }
  else if (!(DrawFlags & CSDRAW_2DGRAPHICS) && (DrawMode & CSDRAW_2DGRAPHICS))
  {
    cs_time end2d = GetAccurateTime ();
    total_2d_time += end2d-start2d;
  }

  DrawMode = DrawFlags;

  return true;
}

void csGraphics3DInfinite::FinishDraw ()
{
  DrawMode = 0;
  startnone = GetAccurateTime ();
  total_time = startnone-startfirst;
}

void csGraphics3DInfinite::Print (csRect *area)
{
  G2D->Print (area);
  num_frames++;
}

void csGraphics3DInfinite::DrawTriangleMesh (G3DTriangleMesh& mesh)
{
  num_drawtrianglemesh++;
  if (do_fastmesh)
  {
    num_drawpolygonfx_mesh += mesh.num_triangles;
  }
  else
  {
    in_mesh = true;
    DefaultDrawTriangleMesh (mesh, this, o2c, clipper, aspect, width2, height2);
    in_mesh = false;
  }
}

void csGraphics3DInfinite::DrawPolygonMesh (G3DPolygonMesh& mesh)
{
  num_drawpolygon_mesh++;
  if (do_fastmesh)
  {
    num_drawpolygon_mesh += mesh.num_polygons;
  }
  else
  {
    in_mesh = true;
    DefaultDrawPolygonMesh (mesh, this, o2c, clipper, aspect, inv_aspect, width2, height2);
    in_mesh = false;
  }
}

void csGraphics3DInfinite::DrawPolygon (G3DPolygonDP& poly)
{
  if (poly.num < 3) return;
  if (in_mesh)
    num_drawpolygon_mesh++;
  else
    num_drawpolygon++;
  if (do_overdraw)
  {
    int i;
    csVector2 p1 (poly.vertices[0].sx, poly.vertices[0].sy);
    csVector2 p2 (poly.vertices[1].sx, poly.vertices[1].sy);
    csVector2 p3;
    for (i = 2 ; i < poly.num ; i++)
    {
      p3.x = poly.vertices[i].sx;
      p3.y = poly.vertices[i].sy;
      pixels_drawn += ABS (csMath2::Area2 (p1, p2, p3) / 2);
    }
  }
}

void csGraphics3DInfinite::DrawPolygonFX (G3DPolygonDPFX& poly)
{
  if (poly.num < 3) return;
  if (in_mesh)
    num_drawpolygonfx_mesh++;
  else
    num_drawpolygonfx++;
  if (do_overdraw)
  {
    int i;
    csVector2 p1 (poly.vertices[0].sx, poly.vertices[0].sy);
    csVector2 p2 (poly.vertices[1].sx, poly.vertices[1].sy);
    csVector2 p3;
    for (i = 2 ; i < poly.num ; i++)
    {
      p3.x = poly.vertices[i].sx;
      p3.y = poly.vertices[i].sy;
      pixels_drawn_fx += ABS (csMath2::Area2 (p1, p2, p3) / 2);
    }
  }
}

bool csGraphics3DInfinite::SetRenderState (G3D_RENDERSTATEOPTION op,
  long value)
{
  switch (op)
  {
    case G3DRENDERSTATE_ZBUFFERMODE:
      z_buf_mode = csZBufMode (value);
      break;
    default:
      return false;
  }

  return true;
}

long csGraphics3DInfinite::GetRenderState (G3D_RENDERSTATEOPTION op)
{
  switch (op)
  {
    case G3DRENDERSTATE_ZBUFFERMODE:
      return z_buf_mode;
    default:
      return 0;
  }
}

void csGraphics3DInfinite::DrawLine (const csVector3&, const csVector3&, float, int)
{
}

//---------------------------------------------------------------------------

IMPLEMENT_EMBEDDED_IBASE (csGraphics3DInfinite::csInfiniteConfig)
  IMPLEMENTS_INTERFACE (iConfig)
IMPLEMENT_EMBEDDED_IBASE_END

#define NUM_OPTIONS 2

static const csOptionDescription config_options [NUM_OPTIONS] =
{
  { 0, "overdraw", "Enable overdraw calculation", CSVAR_BOOL },
  { 1, "fastmesh", "Enable fast mesh emulation", CSVAR_BOOL },
};

bool csGraphics3DInfinite::csInfiniteConfig::SetOption (int id, csVariant* value)
{
  if (value->type != config_options[id].type)
    return false;
  switch (id)
  {
    case 0: scfParent->do_overdraw = value->v.b; break;
    case 1: scfParent->do_fastmesh = value->v.b; break;
    default: return false;
  }
  return true;
}

bool csGraphics3DInfinite::csInfiniteConfig::GetOption (int id, csVariant* value)
{
  value->type = config_options[id].type;
  switch (id)
  {
    case 0: value->v.b = scfParent->do_overdraw; break;
    case 1: value->v.b = scfParent->do_fastmesh; break;
    default: return false;
  }
  return true;
}

bool csGraphics3DInfinite::csInfiniteConfig::GetOptionDescription
  (int idx, csOptionDescription* option)
{
  if (idx < 0 || idx >= NUM_OPTIONS)
    return false;
  *option = config_options[idx];
  return true;
}

//====================================================================

IMPLEMENT_IBASE (csGraphics2DInfinite)
  IMPLEMENTS_INTERFACE (iPlugIn)
  IMPLEMENTS_INTERFACE (iGraphics2D)
IMPLEMENT_IBASE_END

// csGraphics2DInfinite functions
csGraphics2DInfinite::csGraphics2DInfinite (iBase *iParent) :
  csGraphics2D ()
{
  CONSTRUCT_IBASE (iParent);
}

bool csGraphics2DInfinite::Initialize (iSystem *pSystem)
{
  if (!csGraphics2D::Initialize (pSystem))
    return false;

  pfmt.RedMask = 0xf800;
  pfmt.GreenMask = 0x07e0;
  pfmt.BlueMask = 0x001f;

  pfmt.complete ();
  pfmt.PalEntries = 0;
  pfmt.PixelBytes = 2;

  return true;
}

csGraphics2DInfinite::~csGraphics2DInfinite ()
{
  Close();
}
