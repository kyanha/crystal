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

#define SYSDEF_SOFTWARE2D
#include "cssysdef.h"
#include "qint.h"
#include "csgeom/math2d.h"
#include "csgeom/math3d.h"
#include "csgeom/polyclip.h"
#include "video/renderer/line/line_g3d.h"
#include "video/renderer/line/line_txt.h"
#include "csutil/inifile.h"
#include "ipolygon.h"
#include "isystem.h"
#include "igraph2d.h"
#include "ilghtmap.h"

#define SysPrintf System->Printf

///---------------------------------------------------------------------------

#if defined(OS_UNIX)
static char* get_line_2d_driver ()
{
  if (getenv ("DISPLAY"))
    return "crystalspace.graphics2d.linex2d";
  else
    return SOFTWARE_2D_DRIVER;
}
#define LINE_SOFTWARE_2D_DRIVER get_line_2d_driver()
#else
#define LINE_SOFTWARE_2D_DRIVER SOFTWARE_2D_DRIVER
#endif

///---------------------------------------------------------------------------

IMPLEMENT_FACTORY (csGraphics3DLine)

EXPORT_CLASS_TABLE (line3d)
  EXPORT_CLASS (csGraphics3DLine, "crystalspace.graphics3d.line",
    "Line 3D graphics driver for Crystal Space")
EXPORT_CLASS_TABLE_END

IMPLEMENT_IBASE (csGraphics3DLine)
  IMPLEMENTS_INTERFACE (iPlugIn)
  IMPLEMENTS_INTERFACE (iGraphics3D)
  IMPLEMENTS_EMBEDDED_INTERFACE (iConfig)
IMPLEMENT_IBASE_END

csGraphics3DLine::csGraphics3DLine (iBase *iParent) : G2D (NULL), config (NULL)
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
}

csGraphics3DLine::~csGraphics3DLine ()
{
  Close ();
  delete texman;
  if (G2D)
    G2D->DecRef ();
  delete config;
}

bool csGraphics3DLine::Initialize (iSystem *iSys)
{
  System = iSys;

  iVFS* v = QUERY_PLUGIN_ID (System, CS_FUNCID_VFS, iVFS);
  config = new csIniFile (v, "/config/line3d.cfg");
  v->DecRef(); v = NULL;

  width = height = -1;

  const char *driver = iSys->GetOptionCL ("canvas");
  if (!driver)
    driver = config->GetStr ("Hardware", "Canvas", LINE_SOFTWARE_2D_DRIVER);

  G2D = LOAD_PLUGIN (System, driver, NULL, iGraphics2D);
  if (!G2D)
    return false;

  texman = new csTextureManagerLine (System, G2D, config);

  return true;
}

bool csGraphics3DLine::Open (const char *Title)
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
  bool bFullScreen = G2D->GetFullScreen ();

  csPixelFormat pfmt = *G2D->GetPixelFormat ();
  if (pfmt.PalEntries)
  {
    // If we don't have truecolor we simulate 5:6:5 bits
    // for R:G:B in the masks anyway because we still need the
    // 16-bit format for our light mixing
    pfmt.RedShift   = RGB2PAL_BITS_G + RGB2PAL_BITS_B;
    pfmt.GreenShift = RGB2PAL_BITS_B;
    pfmt.BlueShift  = 0;
    pfmt.RedMask    = ((1 << RGB2PAL_BITS_G) - 1) << pfmt.RedShift;
    pfmt.GreenMask  = ((1 << RGB2PAL_BITS_G) - 1) << pfmt.GreenShift;
    pfmt.BlueMask   = ((1 << RGB2PAL_BITS_B) - 1);
    pfmt.RedBits    = RGB2PAL_BITS_R;
    pfmt.GreenBits  = RGB2PAL_BITS_G;
    pfmt.BlueBits   = RGB2PAL_BITS_B;
  }
  texman->SetPixelFormat (pfmt);

  SetDimensions (nWidth, nHeight);

  SysPrintf (MSG_INITIALIZATION, "Using %s mode %dx%d.\n",
            bFullScreen ? "full screen" : "windowed", width, height);

  z_buf_mode = CS_ZBUF_NONE;

  return true;
}

void csGraphics3DLine::Close()
{
  if ((width == height) && (width == -1))
    return;

  G2D->Close ();
  width = height = -1;
}

void csGraphics3DLine::SetDimensions (int nwidth, int nheight)
{
  width = nwidth;
  height = nheight;
  width2 = width/2;
  height2 = height/2;
}

void csGraphics3DLine::SetPerspectiveCenter (int x, int y)
{
  width2 = x;
  height2 = y;
}

void csGraphics3DLine::SetClipper (csVector2* vertices, int num_vertices)
{
  delete clipper;
  clipper = NULL;
  if (!vertices) return;
  // @@@ This could be better! We are using a general polygon clipper
  // even in cases where a box clipper would be better. We should
  // have a special SetBoxClipper call in iGraphics3D.
  clipper = new csPolygonClipper (vertices, num_vertices, false, true);
}

void csGraphics3DLine::GetClipper (csVector2* vertices, int& num_vertices)
{
  if (!clipper) { num_vertices = 0; return; }
  num_vertices = clipper->GetNumVertices ();
  csVector2* clip_verts = clipper->GetClipPoly ();
  int i;
  for (i = 0 ; i < num_vertices ; i++)
    vertices[i] = clip_verts[i];
}

bool csGraphics3DLine::BeginDraw (int DrawFlags)
{
  if ((G2D->GetWidth() != width) || 
      (G2D->GetHeight() != height)) 
    SetDimensions (G2D->GetWidth(), G2D->GetHeight());
  // if 2D graphics is not locked, lock it
  if ((DrawFlags & (CSDRAW_2DGRAPHICS | CSDRAW_3DGRAPHICS))
   && (!(DrawMode & (CSDRAW_2DGRAPHICS | CSDRAW_3DGRAPHICS))))
  {
    if (!G2D->BeginDraw())
      return false;
  }

  if (DrawFlags & CSDRAW_CLEARSCREEN)
    G2D->Clear (0);

  if (DrawFlags & CSDRAW_3DGRAPHICS)
  {
    G2D->Clear (0);
  }
  else if (DrawMode & CSDRAW_3DGRAPHICS)
  {
  }

  DrawMode = DrawFlags;

  return true;
}

void csGraphics3DLine::FinishDraw ()
{
  if (DrawMode & (CSDRAW_2DGRAPHICS | CSDRAW_3DGRAPHICS))
    G2D->FinishDraw ();
  DrawMode = 0;
}

void csGraphics3DLine::Print (csRect *area)
{
  G2D->Print (area);
}

void csGraphics3DLine::DrawPolygon (G3DPolygonDP& poly)
{
  if (poly.num < 3)
    return;
  int i, color;

  UByte flat_r, flat_g, flat_b;
  poly.txt_handle->GetMeanColor (flat_r, flat_g, flat_b);

  if (flat_r < 50 && flat_g < 50 && flat_b < 50)
    color = texman->FindRGB (50, 50, 50);
  else
    color = texman->FindRGB (flat_r, flat_g, flat_b);

  for (i = 0 ; i < poly.num ; i++)
  {
    G2D->DrawLine (poly.vertices[i].sx, height-poly.vertices[i].sy,
    	poly.vertices[(i+1)%poly.num].sx, height-poly.vertices[(i+1)%poly.num].sy,
	color);
  }
}

void csGraphics3DLine::DrawPolygonFX (G3DPolygonDPFX& poly)
{
  if (poly.num < 3)
    return;
  int i, color;

  UByte flat_r, flat_g, flat_b;
  if (poly.txt_handle)
    poly.txt_handle->GetMeanColor (flat_r, flat_g, flat_b);
  else
  {
    flat_r = poly.flat_color_r;
    flat_g = poly.flat_color_g;
    flat_b = poly.flat_color_b;
  }

  if (flat_r < 50 && flat_g < 50 && flat_b < 50)
    color = texman->FindRGB (50, 50, 50);
  else
    color = texman->FindRGB (flat_r, flat_g, flat_b);

  for (i = 0 ; i < poly.num ; i++)
  {
    G2D->DrawLine (poly.vertices[i].sx, height-poly.vertices[i].sy,
    	poly.vertices[(i+1)%poly.num].sx, height-poly.vertices[(i+1)%poly.num].sy,
	color);
  }
}

bool csGraphics3DLine::SetRenderState (G3D_RENDERSTATEOPTION op,
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

long csGraphics3DLine::GetRenderState (G3D_RENDERSTATEOPTION op)
{
  switch (op)
  {
    case G3DRENDERSTATE_ZBUFFERMODE:
      return z_buf_mode;
    default:
      return 0;
  }
}

void csGraphics3DLine::DrawLine (const csVector3& v1, const csVector3& v2,
	float fov, int color)
{
  if (v1.z < SMALL_Z && v2.z < SMALL_Z)
    return;

  float x1 = v1.x, y1 = v1.y, z1 = v1.z;
  float x2 = v2.x, y2 = v2.y, z2 = v2.z;

  if (z1 < SMALL_Z)
  {
    // x = t*(x2-x1)+x1;
    // y = t*(y2-y1)+y1;
    // z = t*(z2-z1)+z1;
    float t = (SMALL_Z-z1) / (z2-z1);
    x1 = t*(x2-x1)+x1;
    y1 = t*(y2-y1)+y1;
    z1 = SMALL_Z;
  }
  else if (z2 < SMALL_Z)
  {
    // x = t*(x2-x1)+x1;
    // y = t*(y2-y1)+y1;
    // z = t*(z2-z1)+z1;
    float t = (SMALL_Z-z1) / (z2-z1);
    x2 = t*(x2-x1)+x1;
    y2 = t*(y2-y1)+y1;
    z2 = SMALL_Z;
  }

  float iz1 = fov/z1;
  int px1 = QInt (x1 * iz1 + (width/2));
  int py1 = height - 1 - QInt (y1 * iz1 + (height/2));
  float iz2 = fov/z2;
  int px2 = QInt (x2 * iz2 + (width/2));
  int py2 = height - 1 - QInt (y2 * iz2 + (height/2));

  G2D->DrawLine (px1, py1, px2, py2, color);
}

//---------------------------------------------------------------------------

IMPLEMENT_EMBEDDED_IBASE (csGraphics3DLine::csLineConfig)
  IMPLEMENTS_INTERFACE (iConfig)
IMPLEMENT_EMBEDDED_IBASE_END

#define NUM_OPTIONS 1

static const csOptionDescription config_options [NUM_OPTIONS] =
{
  { 0, "dummy", "Dummy", CSVAR_BOOL },
};

bool csGraphics3DLine::csLineConfig::SetOption (int id, csVariant* value)
{
  if (value->type != config_options[id].type)
    return false;
  switch (id)
  {
    case 0: break;
    default: return false;
  }
  return true;
}

bool csGraphics3DLine::csLineConfig::GetOption (int id, csVariant* value)
{
  value->type = config_options[id].type;
  switch (id)
  {
    case 0: value->v.b = false; break;
    default: return false;
  }
  return true;
}

bool csGraphics3DLine::csLineConfig::GetOptionDescription
  (int idx, csOptionDescription* option)
{
  if (idx < 0 || idx >= NUM_OPTIONS)
    return false;
  *option = config_options[idx];
  return true;
}
