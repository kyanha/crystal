/*
    Copyright (C) 1998-2001 by Jorrit Tyberghein

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

#define CS_SYSDEF_PROVIDE_SOFTWARE2D
#include "cssysdef.h"
#include "cssys/sysfunc.h"
#include "qint.h"
#include "csgeom/math2d.h"
#include "csgeom/math3d.h"
#include "csgeom/transfrm.h"
#include "csgeom/polyclip.h"
#include "csgeom/plane3.h"
#include "scan.h"
#include "tcache.h"
#include "soft_txt.h"
#include "ivideo/graph2d.h"
#include "ivideo/rendermesh.h"
#include "imesh/thing/lightmap.h"	//@@@!!!
#include "sft3dcom.h"
#include "csutil/garray.h"
#include "csutil/scfstrset.h"
#include "iutil/cfgfile.h"
#include "iutil/cmdline.h"
#include "iutil/event.h"
#include "iutil/eventq.h"
#include "iutil/objreg.h"
#include "iutil/plugin.h"
#include "ivaria/reporter.h"

#include "renderbuffer.h"

#if defined (DO_MMX)
#  include "video/renderer/software/i386/cpuid.h"
#endif

int csSoftwareGraphics3DCommon::filter_bf = 1;

//-------------------------- The indices into arrays of scanline routines ------

/*
 *  The rules for scanproc index name building:
 *  Curly brackets means optional name components
 *  Square brackets denote enforced name components
 *  Everything outside brackets is a must
 *
 *  SCANPROC_{Persp_}{Source_}{Effects_}{Zmode}
 *
 *  Persp       = PI for perspective-incorrect routines
 *  Source      = TEX for non-lightmapped textures
 *                MAP for lightmapped textures
 *                FLAT for flat-shaded
 *                FOG for drawing fog
 *  Effects     = GOU for Gouraud-shading applied to the texture
 *                KEY for "key-color" source pixel removal
 *                FX if routine supports table-driven effects
 *                FXKEY for both FX and KEY effects
 *                ALPHA for alpha-mapped textures
 *  Zmode       = ZUSE for polys that are tested against Z-buffer (and fills)
 *                ZFIL for polys that just fills Z-buffer without testing
 *
 *  Example:
 *      SCANPROC_TEX_ZFIL
 *              scanline procedure for drawing a non-lightmapped
 *              texture with Z-fill
 *  Note:
 *      For easier runtime decisions odd indices use Z buffer
 *      while even indices fills Z-buffer (if appropiate)
 */
#define SCANPROC_FLAT_ZNONE             0x00
#define SCANPROC_FLAT_ZFIL              0x01
#define SCANPROC_FLAT_ZUSE              0x02
#define SCANPROC_FLAT_ZTEST             0x03
#define SCANPROC_MAP_ZNONE              0x04
#define SCANPROC_MAP_ZFIL               0x05
#define SCANPROC_MAP_ZUSE               0x06
#define SCANPROC_MAP_ZTEST              0x07
#define SCANPROC_MAP_KEY_ZNONE          0x08
#define SCANPROC_MAP_KEY_ZFIL           0x09
#define SCANPROC_MAP_KEY_ZUSE           0x0a
#define SCANPROC_MAP_KEY_ZTEST          0x0b
#define SCANPROC_TEX_ZNONE              0x0c
#define SCANPROC_TEX_ZFIL               0x0d
#define SCANPROC_TEX_ZUSE               0x0e
#define SCANPROC_TEX_ZTEST              0x0f
#define SCANPROC_TEX_KEY_ZNONE          0x10
#define SCANPROC_TEX_KEY_ZFIL           0x11
#define SCANPROC_TEX_KEY_ZUSE           0x12
#define SCANPROC_TEX_KEY_ZTEST          0x13
#define SCANPROC_TEX_ALPHA_ZNONE        0x14
#define SCANPROC_TEX_ALPHA_ZFIL         0x15
#define SCANPROC_TEX_ALPHA_ZUSE         0x16
#define SCANPROC_TEX_ALPHA_ZTEST        0x17
#define SCANPROC_MAP_ALPHA_ZNONE        0x18
#define SCANPROC_MAP_ALPHA_ZFIL         0x19
#define SCANPROC_MAP_ALPHA_ZUSE         0x1a
#define SCANPROC_MAP_ALPHA_ZTEST        0x1b
#define SCANPROC_TEX_FX_ZNONE           0x1c
#define SCANPROC_TEX_FX_ZFIL            0x1d
#define SCANPROC_TEX_FX_ZUSE            0x1e
#define SCANPROC_TEX_FX_ZTEST           0x1f
#define SCANPROC_MAP_FX_ZNONE           0x20
#define SCANPROC_MAP_FX_ZFIL            0x21
#define SCANPROC_MAP_FX_ZUSE            0x22
#define SCANPROC_MAP_FX_ZTEST           0x23
// these do not have "zuse" counterparts
#define SCANPROC_ZFIL                   0x24
#define SCANPROC_FOG                    0x25
#define SCANPROC_FOG_VIEW               0x26

// The following routines have a different prototype

// Flat-shaded perspective-incorrect routines
#define SCANPROC_PI_FLAT_ZNONE          0x00
#define SCANPROC_PI_FLAT_ZFIL           0x01
#define SCANPROC_PI_FLAT_ZUSE           0x02
#define SCANPROC_PI_FLAT_ZTEST          0x03
// Textured flat-shaded polygons
#define SCANPROC_PI_TEX_ZNONE           0x04
#define SCANPROC_PI_TEX_ZFIL            0x05
#define SCANPROC_PI_TEX_ZUSE            0x06
#define SCANPROC_PI_TEX_ZTEST           0x07
#define SCANPROC_PI_TEX_KEY_ZNONE       0x08
#define SCANPROC_PI_TEX_KEY_ZFIL        0x09
#define SCANPROC_PI_TEX_KEY_ZUSE        0x0a
#define SCANPROC_PI_TEX_KEY_ZTEST       0x0b
// Textured flat-shaded polygons with tiling
#define SCANPROC_PI_TILE_TEX_ZNONE      0x0c
#define SCANPROC_PI_TILE_TEX_ZFIL       0x0d
#define SCANPROC_PI_TILE_TEX_ZUSE       0x0e
#define SCANPROC_PI_TILE_TEX_ZTEST      0x0f
#define SCANPROC_PI_TILE_TEX_KEY_ZNONE  0x10
#define SCANPROC_PI_TILE_TEX_KEY_ZFIL   0x11
#define SCANPROC_PI_TILE_TEX_KEY_ZUSE   0x12
#define SCANPROC_PI_TILE_TEX_KEY_ZTEST  0x13
// Scanline drawing routines with flat shading + effects.
#define SCANPROC_PI_FLAT_FX_ZNONE       0x14
#define SCANPROC_PI_FLAT_FX_ZFIL        0x15
#define SCANPROC_PI_FLAT_FX_ZUSE        0x16
#define SCANPROC_PI_FLAT_FX_ZTEST       0x17
#define SCANPROC_PI_TEX_FX_ZNONE        0x18
#define SCANPROC_PI_TEX_FX_ZFIL         0x19
#define SCANPROC_PI_TEX_FX_ZUSE         0x1a
#define SCANPROC_PI_TEX_FX_ZTEST        0x1b
#define SCANPROC_PI_TEX_FXKEY_ZNONE     0x1c
#define SCANPROC_PI_TEX_FXKEY_ZFIL      0x1d
#define SCANPROC_PI_TEX_FXKEY_ZUSE      0x1e
#define SCANPROC_PI_TEX_FXKEY_ZTEST     0x1f
#define SCANPROC_PI_TILE_TEX_FX_ZNONE   0x20
#define SCANPROC_PI_TILE_TEX_FX_ZFIL    0x21
#define SCANPROC_PI_TILE_TEX_FX_ZUSE    0x22
#define SCANPROC_PI_TILE_TEX_FX_ZTEST   0x23
#define SCANPROC_PI_TILE_TEX_FXKEY_ZNONE 0x24
#define SCANPROC_PI_TILE_TEX_FXKEY_ZFIL 0x25
#define SCANPROC_PI_TILE_TEX_FXKEY_ZUSE 0x26
#define SCANPROC_PI_TILE_TEX_FXKEY_ZTEST 0x27
// Perspective-incorrect flat-shaded alpha-mapped texture
#define SCANPROC_PI_TEX_ALPHA_ZNONE     0x28
#define SCANPROC_PI_TEX_ALPHA_ZFIL      0x29
#define SCANPROC_PI_TEX_ALPHA_ZUSE      0x2a
#define SCANPROC_PI_TEX_ALPHA_ZTEST     0x2b

// Gouraud-shaded PI routines should have same indices
// as their non-Gouraud counterparts. Every routine except
// flat-shaded ones have two versions: without table-driven
// effects (FX) and one with them.
#define SCANPROC_PI_FLAT_GOU_ZNONE           0x00
#define SCANPROC_PI_FLAT_GOU_ZFIL            0x01
#define SCANPROC_PI_FLAT_GOU_ZUSE            0x02
#define SCANPROC_PI_FLAT_GOU_ZTEST           0x03
// Textured Gouraud-shaded polygons
#define SCANPROC_PI_TEX_GOU_ZNONE            0x04
#define SCANPROC_PI_TEX_GOU_ZFIL             0x05
#define SCANPROC_PI_TEX_GOU_ZUSE             0x06
#define SCANPROC_PI_TEX_GOU_ZTEST            0x07
#define SCANPROC_PI_TEX_GOUKEY_ZNONE         0x08
#define SCANPROC_PI_TEX_GOUKEY_ZFIL          0x09
#define SCANPROC_PI_TEX_GOUKEY_ZUSE          0x0a
#define SCANPROC_PI_TEX_GOUKEY_ZTEST         0x0b
// Textured Gouraud-shaded polygons with tiling
#define SCANPROC_PI_TILE_TEX_GOU_ZNONE       0x0c
#define SCANPROC_PI_TILE_TEX_GOU_ZFIL        0x0d
#define SCANPROC_PI_TILE_TEX_GOU_ZUSE        0x0e
#define SCANPROC_PI_TILE_TEX_GOU_ZTEST       0x0f
#define SCANPROC_PI_TILE_TEX_GOUKEY_ZNONE    0x10
#define SCANPROC_PI_TILE_TEX_GOUKEY_ZFIL     0x11
#define SCANPROC_PI_TILE_TEX_GOUKEY_ZUSE     0x12
#define SCANPROC_PI_TILE_TEX_GOUKEY_ZTEST    0x13
// Scanline drawing routines with Gouraud shading + effects.
#define SCANPROC_PI_FLAT_GOUFX_ZNONE         0x14
#define SCANPROC_PI_FLAT_GOUFX_ZFIL          0x15
#define SCANPROC_PI_FLAT_GOUFX_ZUSE          0x16
#define SCANPROC_PI_FLAT_GOUFX_ZTEST         0x17
#define SCANPROC_PI_TEX_GOUFX_ZNONE          0x18
#define SCANPROC_PI_TEX_GOUFX_ZFIL           0x19
#define SCANPROC_PI_TEX_GOUFX_ZUSE           0x1a
#define SCANPROC_PI_TEX_GOUFX_ZTEST          0x1b
#define SCANPROC_PI_TEX_GOUFXKEY_ZNONE       0x1c
#define SCANPROC_PI_TEX_GOUFXKEY_ZFIL        0x1d
#define SCANPROC_PI_TEX_GOUFXKEY_ZUSE        0x1e
#define SCANPROC_PI_TEX_GOUFXKEY_ZTEST       0x1f
#define SCANPROC_PI_TILE_TEX_GOUFX_ZNONE     0x20
#define SCANPROC_PI_TILE_TEX_GOUFX_ZFIL      0x21
#define SCANPROC_PI_TILE_TEX_GOUFX_ZUSE      0x22
#define SCANPROC_PI_TILE_TEX_GOUFX_ZTEST     0x23
#define SCANPROC_PI_TILE_TEX_GOUFXKEY_ZNONE  0x24
#define SCANPROC_PI_TILE_TEX_GOUFXKEY_ZFIL   0x25
#define SCANPROC_PI_TILE_TEX_GOUFXKEY_ZUSE   0x26
#define SCANPROC_PI_TILE_TEX_GOUFXKEY_ZTEST  0x27

///---------------------------------------------------------------------------
SCF_IMPLEMENT_IBASE(csSoftwareGraphics3DCommon)
  SCF_IMPLEMENTS_INTERFACE(iGraphics3D)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSoftwareGraphics3DCommon::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csSoftwareGraphics3DCommon::EventHandler)
  SCF_IMPLEMENTS_INTERFACE (iEventHandler)
SCF_IMPLEMENT_IBASE_END

csSoftwareGraphics3DCommon::csSoftwareGraphics3DCommon (iBase* parent)
{
  SCF_CONSTRUCT_IBASE (parent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);

  tcache = 0;
  scfiEventHandler = 0;
  texman = 0;
  //vbufmgr = 0;
  partner = 0;
  clipper = 0;
  cliptype = CS_CLIPPER_NONE;
  do_near_plane = false;

#ifdef DO_MMX
  do_mmx = true;
#endif
  do_lighting = true;
  do_alpha = true;
  do_textured = true;
  do_interlaced = -1;
  ilace_fastmove = false;
  bilinear_filter = 0;
  do_smaller_rendering = false;
  smaller_buffer = 0;
  pixel_shift = 0;
  rstate_mipmap = 0;
  do_gouraud = true;

  dbg_max_polygons_to_draw = 2000000000; // After 2 billion polygons we give up :-)

  z_buffer = 0;
  line_table = 0;

  /*Caps.CanClip = false;
  Caps.minTexHeight = 2;
  Caps.minTexWidth = 2;
  Caps.maxTexHeight = 1024;
  Caps.maxTexWidth = 1024;
  Caps.fog = G3DFOGMETHOD_ZBUFFER;
  Caps.NeedsPO2Maps = false;
  Caps.MaxAspectRatio = 32768;*/
  width = height = -1;
  partner = 0;
  is_for_procedural_textures = false;

  dpfx_valid = false;

  object_reg = 0;

  for (int i=0; i<16; ++i)
    activebuffers[i] = 0;
}

csSoftwareGraphics3DCommon::~csSoftwareGraphics3DCommon ()
{
  if (scfiEventHandler)
  {
    csRef<iEventQueue> q (CS_QUERY_REGISTRY (object_reg, iEventQueue));
    if (q != 0)
      q->RemoveListener (scfiEventHandler);
    scfiEventHandler->DecRef ();
  }

  Close ();
  if (partner) partner->DecRef ();
  if (clipper)
  {
    clipper->DecRef ();
    clipper = 0;
    cliptype = CS_CLIPPER_NONE;
  }
}

bool csSoftwareGraphics3DCommon::Initialize (iObjectRegistry* p)
{
  object_reg = p;
  if (!scfiEventHandler)
    scfiEventHandler = new EventHandler (this);
  csRef<iEventQueue> q (CS_QUERY_REGISTRY(object_reg, iEventQueue));
  if (q != 0)
    q->RegisterListener (scfiEventHandler, CSMASK_Broadcast);

  strings = CS_QUERY_REGISTRY_TAG_INTERFACE (
    object_reg, "crystalspace.renderer.stringset", iStringSet);
  if (!strings)
  {
    strings = csPtr<iStringSet> (new csScfStringSet ());
    object_reg->Register (strings, "crystalspace.renderer.stringset");
  }

  return true;
}

bool csSoftwareGraphics3DCommon::HandleEvent (iEvent& Event)
{
  if (Event.Type == csevBroadcast)
    switch (Event.Command.Code)
    {
      case cscmdSystemOpen:
      {
        Open ();
        return true;
      }
      case cscmdSystemClose:
      {
        Close ();
        return true;
      }
    }
  return false;
}

void csSoftwareGraphics3DCommon::NewInitialize ()
{
  config.AddConfig(object_reg, "/config/soft3d.cfg");
  do_smaller_rendering = config->GetBool ("Video.Software.Smaller", false);
  mipmap_coef = config->GetFloat ("Video.Software.TextureManager.MipmapCoef", 1.3);
  do_interlaced = config->GetBool ("Video.Software.Interlacing", false) ? 0 : -1;

#ifdef DO_MMX
  do_mmx = config->GetBool ("Video.Software.MMX", true);
#endif
}

void csSoftwareGraphics3DCommon::Report (int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csRef<iReporter> rep (CS_QUERY_REGISTRY (object_reg, iReporter));
  if (rep)
    rep->ReportV (severity, "crystalspace.video.software", msg, arg);
  else
  {
    csPrintfV (msg, arg);
    csPrintf ("\n");
  }
  va_end (arg);
}

bool csSoftwareGraphics3DCommon::Open ()
{
  if (!G2D->Open ())
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Error opening Graphics2D context.");
    // set "not opened" flag
    width = height = -1;
    return false;
  }

  pfmt = *G2D->GetPixelFormat ();

  if (pfmt.PixelBytes == 4)
    pixel_shift = 2;
  else if (pfmt.PixelBytes == 2)
    pixel_shift = 1;
  else
  {
    Report (CS_REPORTER_SEVERITY_ERROR,
    	"8-bit palette mode no longer works in the software renderer!");
    return false;
  }

  DrawMode = 0;
  SetDimensions (G2D->GetWidth (), G2D->GetHeight ());
  z_buf_mode = CS_ZBUF_NONE;

  csRef<iPluginManager> plugin_mgr (
    CS_QUERY_REGISTRY (object_reg, iPluginManager));

  shadermgr = CS_QUERY_REGISTRY(object_reg, iShaderManager);
  if( !shadermgr )
  {
    shadermgr = csPtr<iShaderManager>
      (CS_LOAD_PLUGIN(plugin_mgr, "crystalspace.graphics3d.shadermanager", iShaderManager));
    object_reg->Register( shadermgr, "iShaderManager");
  }

  return true;
}

bool csSoftwareGraphics3DCommon::NewOpen ()
{
#if defined (DO_MMX)
  int family, features;
  char vendor [13];
  csDetectCPU (&family, vendor, &features);
  /* Higher 686 processors like P4 report higher cpu family, although they're
     still 686 architecture, so limit that until higher architecture follows
     Can someone test on IA64 or Xeon ? */
  if (family > 686) family = 686;
  cpu_mmx = (features & CPUx86_FEATURE_MMX) != 0;
  Report (CS_REPORTER_SEVERITY_NOTIFY,
  	"%d %s CPU detected; FPU (%s) MMX (%s) CMOV (%s)",
    family, vendor,
    (features & CPUx86_FEATURE_FPU) ? "yes" : "no",
    (features & CPUx86_FEATURE_MMX) ? "yes" : "no",
    (features & CPUx86_FEATURE_CMOV) ? "yes" : "no");
#endif

  alpha_mask = 0;
  alpha_mask |= 1 << (pfmt.RedShift);
  alpha_mask |= 1 << (pfmt.GreenShift);
  alpha_mask |= 1 << (pfmt.BlueShift);
  alpha_mask = ~alpha_mask;

  //fog_buffers = 0;

  // Create the texture manager
  texman = new csSoftwareTextureManager (object_reg, this, config);
  texman->SetPixelFormat (pfmt);

  // Create the vertex buffer manager.
  //vbufmgr = new csPolArrayVertexBufferManager (object_reg);

  tcache = new csSoftwareTextureCache (texman);
  const char *cache_size = config->GetStr
        ("Video.Software.TextureManager.Cache", 0);
  int csize = DEFAULT_CACHE_SIZE;
  if (cache_size)
  {
    char suffix [100];
    sscanf (cache_size, "%d%s", &csize, suffix);
    if (!strcasecmp (suffix, "KP"))
      csize *= 1024 * pfmt.PixelBytes;
    else if (!strcasecmp (suffix, "MP"))
      csize *= 1024 * 1024 * pfmt.PixelBytes;
    else if (!strcasecmp (suffix, "KB"))
      csize *= 1024;
    else if (!strcasecmp (suffix, "MB"))
      csize *= 1024 * 1024;
    else
      csize = 0;

    if (!csize)
    {
      Report (CS_REPORTER_SEVERITY_NOTIFY,
        "Invalid cache size specified, using default");
      csize = DEFAULT_CACHE_SIZE;
    }
  }
  tcache->set_cache_size (csize);

  ScanSetup ();

  //SetRenderState (G3DRENDERSTATE_INTERLACINGENABLE, do_interlaced == 0);

  return true;
}

bool csSoftwareGraphics3DCommon::SharedOpen ()
{
  pixel_shift = partner->pixel_shift;
  //fog_buffers = partner->fog_buffers;
  alpha_mask = partner->alpha_mask;
  z_buf_mode = partner->z_buf_mode;
#if defined (DO_MMX)
  cpu_mmx = partner->cpu_mmx;
#endif
  texman = partner->texman;
  //vbufmgr = partner->vbufmgr;
  tcache = partner->tcache;
  ScanSetup ();
  //SetRenderState (G3DRENDERSTATE_INTERLACINGENABLE, do_interlaced == 0);
  return true;
}

void csSoftwareGraphics3DCommon::ScanSetup ()
{
  // Select the right scanline drawing functions
  memset (&ScanProc, 0, sizeof (ScanProc));
  memset (&ScanProcPI, 0, sizeof (ScanProcPI));
  memset (&ScanProcPIG, 0, sizeof (ScanProcPIG));
  ScanProc_Alpha = 0;

#ifdef DO_MMX
  bool UseMMX = (cpu_mmx && do_mmx);
#endif

  // Bits-per-pixel independent routine
  ScanProc [SCANPROC_ZFIL] = csScan_scan_zfil;

  switch (pfmt.PixelBytes)
  {
    case 2:
      if (do_alpha) ScanProc_Alpha = ScanProc_16_Alpha;

      ScanProc [SCANPROC_FLAT_ZNONE] = csScan_16_scan_flat_znone;
      ScanProc [SCANPROC_FLAT_ZFIL] = csScan_16_scan_flat_zfil;
      ScanProc [SCANPROC_FLAT_ZUSE] = csScan_16_scan_flat_zuse;
      ScanProc [SCANPROC_FLAT_ZTEST] = csScan_16_scan_flat_ztest;

      ScanProc [SCANPROC_TEX_ZNONE] = csScan_16_scan_tex_znone;
      ScanProc [SCANPROC_TEX_ZFIL] =
#ifdef DO_MMX
        UseMMX ? csScan_16_mmx_scan_tex_zfil :
#endif
        csScan_16_scan_tex_zfil;
      ScanProc [SCANPROC_TEX_ZUSE] = csScan_16_scan_tex_zuse;
      ScanProc [SCANPROC_TEX_ZTEST] = csScan_16_scan_tex_ztest;

      ScanProc [SCANPROC_MAP_ZNONE] =
        bilinear_filter == 2 ?
        (pfmt.GreenBits == 5 ?
          csScan_16_555_scan_map_filt2_znone :
          csScan_16_565_scan_map_filt2_znone) :
        bilinear_filter == 1 ? csScan_16_scan_map_filt_znone :
        csScan_16_scan_map_znone;
      ScanProc [SCANPROC_MAP_ZFIL] =
        bilinear_filter == 2 ?
        (pfmt.GreenBits == 5 ?
          csScan_16_555_scan_map_filt2_zfil :
          csScan_16_565_scan_map_filt2_zfil) :
        bilinear_filter == 1 ? csScan_16_scan_map_filt_zfil :
#ifdef DO_MMX
        UseMMX ? csScan_16_mmx_scan_map_zfil :
#endif
        csScan_16_scan_map_zfil;
      ScanProc [SCANPROC_MAP_ZUSE] =
        bilinear_filter == 2 ?
        (pfmt.GreenBits == 5 ?
          csScan_16_555_scan_map_filt2_zuse :
          csScan_16_565_scan_map_filt2_zuse) :
        csScan_16_scan_map_zuse;
      ScanProc [SCANPROC_MAP_ZTEST] =
        bilinear_filter == 2 ?
        (pfmt.GreenBits == 5 ?
          csScan_16_555_scan_map_filt2_ztest :
          csScan_16_565_scan_map_filt2_ztest) :
        csScan_16_scan_map_ztest;

      ScanProc [SCANPROC_TEX_KEY_ZNONE] = csScan_16_scan_tex_key_znone;
      ScanProc [SCANPROC_TEX_KEY_ZFIL] = csScan_16_scan_tex_key_zfil;
      ScanProc [SCANPROC_TEX_KEY_ZUSE] = csScan_16_scan_tex_key_zuse;
      ScanProc [SCANPROC_TEX_KEY_ZTEST] = csScan_16_scan_tex_key_ztest;
      ScanProc [SCANPROC_MAP_KEY_ZNONE] = csScan_16_scan_map_key_znone;
      ScanProc [SCANPROC_MAP_KEY_ZFIL] = csScan_16_scan_map_key_zfil;
      ScanProc [SCANPROC_MAP_KEY_ZUSE] = csScan_16_scan_map_key_zuse;
      ScanProc [SCANPROC_MAP_KEY_ZTEST] = csScan_16_scan_map_key_ztest;

      ScanProc [SCANPROC_TEX_ALPHA_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_tex_alpha_znone :
        csScan_16_565_scan_tex_alpha_znone;
      ScanProc [SCANPROC_TEX_ALPHA_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_tex_alpha_zfil :
        csScan_16_565_scan_tex_alpha_zfil;
      ScanProc [SCANPROC_TEX_ALPHA_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_tex_alpha_zuse :
        csScan_16_565_scan_tex_alpha_zuse;
      ScanProc [SCANPROC_TEX_ALPHA_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_tex_alpha_ztest :
        csScan_16_565_scan_tex_alpha_ztest;
      ScanProc [SCANPROC_MAP_ALPHA_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_map_alpha_znone :
        csScan_16_565_scan_map_alpha_znone;
      ScanProc [SCANPROC_MAP_ALPHA_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_map_alpha_zfil :
        csScan_16_565_scan_map_alpha_zfil;
      ScanProc [SCANPROC_MAP_ALPHA_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_map_alpha_zuse :
        csScan_16_565_scan_map_alpha_zuse;
      ScanProc [SCANPROC_MAP_ALPHA_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_map_alpha_ztest :
        csScan_16_565_scan_map_alpha_ztest;

      ScanProc [SCANPROC_TEX_FX_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_tex_fx_znone :
        csScan_16_565_scan_tex_fx_znone;
      ScanProc [SCANPROC_TEX_FX_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_tex_fx_zfil :
        csScan_16_565_scan_tex_fx_zfil;
      ScanProc [SCANPROC_TEX_FX_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_tex_fx_zuse :
        csScan_16_565_scan_tex_fx_zuse;
      ScanProc [SCANPROC_TEX_FX_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_tex_fx_ztest :
        csScan_16_565_scan_tex_fx_ztest;
      ScanProc [SCANPROC_MAP_FX_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_map_fx_znone :
        csScan_16_565_scan_map_fx_znone;
      ScanProc [SCANPROC_MAP_FX_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_map_fx_zfil :
        csScan_16_565_scan_map_fx_zfil;
      ScanProc [SCANPROC_MAP_FX_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_map_fx_zuse :
        csScan_16_565_scan_map_fx_zuse;
      ScanProc [SCANPROC_MAP_FX_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_map_fx_ztest :
        csScan_16_565_scan_map_fx_ztest;

      ScanProc [SCANPROC_FOG] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_fog :
        csScan_16_565_scan_fog;
      ScanProc [SCANPROC_FOG_VIEW] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_fog_view :
        csScan_16_565_scan_fog_view;

      ScanProcPI [SCANPROC_PI_FLAT_ZNONE] = csScan_16_scan_pi_flat_znone;
      ScanProcPI [SCANPROC_PI_FLAT_ZFIL] = csScan_16_scan_pi_flat_zfil;
      ScanProcPI [SCANPROC_PI_FLAT_ZUSE] = csScan_16_scan_pi_flat_zuse;
      ScanProcPI [SCANPROC_PI_FLAT_ZTEST] = csScan_16_scan_pi_flat_ztest;
      ScanProcPI [SCANPROC_PI_TEX_ZNONE] = csScan_16_scan_pi_tex_znone;
      ScanProcPI [SCANPROC_PI_TEX_ZFIL] = csScan_16_scan_pi_tex_zfil;
      ScanProcPI [SCANPROC_PI_TEX_ZUSE] =
#ifdef DO_MMX
        UseMMX ? csScan_16_mmx_scan_pi_tex_zuse :
#endif
        csScan_16_scan_pi_tex_zuse;
      ScanProcPI [SCANPROC_PI_TEX_ZTEST] = csScan_16_scan_pi_tex_ztest;
      ScanProcPI [SCANPROC_PI_TEX_KEY_ZNONE] = csScan_16_scan_pi_tex_key_znone;
      ScanProcPI [SCANPROC_PI_TEX_KEY_ZFIL] = csScan_16_scan_pi_tex_key_zfil;
      ScanProcPI [SCANPROC_PI_TEX_KEY_ZUSE] = csScan_16_scan_pi_tex_key_zuse;
      ScanProcPI [SCANPROC_PI_TEX_KEY_ZTEST] = csScan_16_scan_pi_tex_key_ztest;
      ScanProcPI [SCANPROC_PI_TILE_TEX_ZNONE] = csScan_16_scan_pi_tile_tex_znone;
      ScanProcPI [SCANPROC_PI_TILE_TEX_ZFIL] = csScan_16_scan_pi_tile_tex_zfil;
      ScanProcPI [SCANPROC_PI_TILE_TEX_ZUSE] = csScan_16_scan_pi_tile_tex_zuse;
      ScanProcPI [SCANPROC_PI_TILE_TEX_ZTEST] = csScan_16_scan_pi_tile_tex_ztest;
      ScanProcPI [SCANPROC_PI_TILE_TEX_KEY_ZNONE] = csScan_16_scan_pi_tile_tex_key_znone;
      ScanProcPI [SCANPROC_PI_TILE_TEX_KEY_ZFIL] = csScan_16_scan_pi_tile_tex_key_zfil;
      ScanProcPI [SCANPROC_PI_TILE_TEX_KEY_ZUSE] = csScan_16_scan_pi_tile_tex_key_zuse;
      ScanProcPI [SCANPROC_PI_TILE_TEX_KEY_ZTEST] = csScan_16_scan_pi_tile_tex_key_ztest;

      ScanProcPI [SCANPROC_PI_FLAT_FX_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_flat_fx_znone :
        csScan_16_565_scan_pi_flat_fx_znone;
      ScanProcPI [SCANPROC_PI_FLAT_FX_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_flat_fx_zfil :
        csScan_16_565_scan_pi_flat_fx_zfil;
      ScanProcPI [SCANPROC_PI_FLAT_FX_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_flat_fx_zuse :
        csScan_16_565_scan_pi_flat_fx_zuse;
      ScanProcPI [SCANPROC_PI_FLAT_FX_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_flat_fx_ztest :
        csScan_16_565_scan_pi_flat_fx_ztest;
      ScanProcPI [SCANPROC_PI_TEX_FX_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_fx_znone :
        csScan_16_565_scan_pi_tex_fx_znone;
      ScanProcPI [SCANPROC_PI_TEX_FX_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_fx_zfil :
        csScan_16_565_scan_pi_tex_fx_zfil;
      ScanProcPI [SCANPROC_PI_TEX_FX_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_fx_zuse :
        csScan_16_565_scan_pi_tex_fx_zuse;
      ScanProcPI [SCANPROC_PI_TEX_FX_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_fx_ztest :
        csScan_16_565_scan_pi_tex_fx_ztest;
      ScanProcPI [SCANPROC_PI_TEX_FXKEY_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_fxkey_znone :
        csScan_16_565_scan_pi_tex_fxkey_znone;
      ScanProcPI [SCANPROC_PI_TEX_FXKEY_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_fxkey_zfil :
        csScan_16_565_scan_pi_tex_fxkey_zfil;
      ScanProcPI [SCANPROC_PI_TEX_FXKEY_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_fxkey_zuse :
        csScan_16_565_scan_pi_tex_fxkey_zuse;
      ScanProcPI [SCANPROC_PI_TEX_FXKEY_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_fxkey_ztest :
        csScan_16_565_scan_pi_tex_fxkey_ztest;
      ScanProcPI [SCANPROC_PI_TILE_TEX_FX_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_fx_znone :
        csScan_16_565_scan_pi_tile_tex_fx_znone;
      ScanProcPI [SCANPROC_PI_TILE_TEX_FX_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_fx_zfil :
        csScan_16_565_scan_pi_tile_tex_fx_zfil;
      ScanProcPI [SCANPROC_PI_TILE_TEX_FX_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_fx_zuse :
        csScan_16_565_scan_pi_tile_tex_fx_zuse;
      ScanProcPI [SCANPROC_PI_TILE_TEX_FX_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_fx_ztest :
        csScan_16_565_scan_pi_tile_tex_fx_ztest;
      ScanProcPI [SCANPROC_PI_TILE_TEX_FXKEY_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_fxkey_znone :
        csScan_16_565_scan_pi_tile_tex_fxkey_znone;
      ScanProcPI [SCANPROC_PI_TILE_TEX_FXKEY_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_fxkey_zfil :
        csScan_16_565_scan_pi_tile_tex_fxkey_zfil;
      ScanProcPI [SCANPROC_PI_TILE_TEX_FXKEY_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_fxkey_zuse :
        csScan_16_565_scan_pi_tile_tex_fxkey_zuse;
      ScanProcPI [SCANPROC_PI_TILE_TEX_FXKEY_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_fxkey_ztest :
        csScan_16_565_scan_pi_tile_tex_fxkey_ztest;
      ScanProcPI [SCANPROC_PI_TEX_ALPHA_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_alpha_znone :
        csScan_16_565_scan_pi_tex_alpha_znone;
      ScanProcPI [SCANPROC_PI_TEX_ALPHA_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_alpha_zfil :
        csScan_16_565_scan_pi_tex_alpha_zfil;
      ScanProcPI [SCANPROC_PI_TEX_ALPHA_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_alpha_zuse :
        csScan_16_565_scan_pi_tex_alpha_zuse;
      ScanProcPI [SCANPROC_PI_TEX_ALPHA_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_alpha_ztest :
        csScan_16_565_scan_pi_tex_alpha_ztest;

      ScanProcPIG [SCANPROC_PI_FLAT_GOU_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_flat_gou_znone :
        csScan_16_565_scan_pi_flat_gou_znone;
      ScanProcPIG [SCANPROC_PI_FLAT_GOU_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_flat_gou_zfil :
        csScan_16_565_scan_pi_flat_gou_zfil;
      ScanProcPIG [SCANPROC_PI_FLAT_GOU_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_flat_gou_zuse :
        csScan_16_565_scan_pi_flat_gou_zuse;
      ScanProcPIG [SCANPROC_PI_FLAT_GOU_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_flat_gou_ztest :
        csScan_16_565_scan_pi_flat_gou_ztest;
      ScanProcPIG [SCANPROC_PI_TEX_GOU_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_gou_znone :
        csScan_16_565_scan_pi_tex_gou_znone;
      ScanProcPIG [SCANPROC_PI_TEX_GOU_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_gou_zfil :
        csScan_16_565_scan_pi_tex_gou_zfil;
      ScanProcPIG [SCANPROC_PI_TEX_GOU_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_gou_zuse :
        csScan_16_565_scan_pi_tex_gou_zuse;
      ScanProcPIG [SCANPROC_PI_TEX_GOU_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_gou_ztest :
        csScan_16_565_scan_pi_tex_gou_ztest;
      ScanProcPIG [SCANPROC_PI_TEX_GOUKEY_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_goukey_znone :
        csScan_16_565_scan_pi_tex_goukey_znone;
      ScanProcPIG [SCANPROC_PI_TEX_GOUKEY_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_goukey_zfil :
        csScan_16_565_scan_pi_tex_goukey_zfil;
      ScanProcPIG [SCANPROC_PI_TEX_GOUKEY_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_goukey_zuse :
        csScan_16_565_scan_pi_tex_goukey_zuse;
      ScanProcPIG [SCANPROC_PI_TEX_GOUKEY_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_goukey_ztest :
        csScan_16_565_scan_pi_tex_goukey_ztest;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOU_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_gou_znone :
        csScan_16_565_scan_pi_tile_tex_gou_znone;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOU_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_gou_zfil :
        csScan_16_565_scan_pi_tile_tex_gou_zfil;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOU_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_gou_zuse :
        csScan_16_565_scan_pi_tile_tex_gou_zuse;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOU_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_gou_ztest :
        csScan_16_565_scan_pi_tile_tex_gou_ztest;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUKEY_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_goukey_znone :
        csScan_16_565_scan_pi_tile_tex_goukey_znone;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUKEY_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_goukey_zfil :
        csScan_16_565_scan_pi_tile_tex_goukey_zfil;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUKEY_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_goukey_zuse :
        csScan_16_565_scan_pi_tile_tex_goukey_zuse;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUKEY_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_goukey_ztest :
        csScan_16_565_scan_pi_tile_tex_goukey_ztest;
      ScanProcPIG [SCANPROC_PI_FLAT_GOUFX_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_flat_goufx_znone :
        csScan_16_565_scan_pi_flat_goufx_znone;
      ScanProcPIG [SCANPROC_PI_FLAT_GOUFX_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_flat_goufx_zfil :
        csScan_16_565_scan_pi_flat_goufx_zfil;
      ScanProcPIG [SCANPROC_PI_FLAT_GOUFX_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_flat_goufx_zuse :
        csScan_16_565_scan_pi_flat_goufx_zuse;
      ScanProcPIG [SCANPROC_PI_FLAT_GOUFX_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_flat_goufx_ztest :
        csScan_16_565_scan_pi_flat_goufx_ztest;
      ScanProcPIG [SCANPROC_PI_TEX_GOUFX_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_goufx_znone :
        csScan_16_565_scan_pi_tex_goufx_znone;
      ScanProcPIG [SCANPROC_PI_TEX_GOUFX_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_goufx_zfil :
        csScan_16_565_scan_pi_tex_goufx_zfil;
      ScanProcPIG [SCANPROC_PI_TEX_GOUFX_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_goufx_zuse :
        csScan_16_565_scan_pi_tex_goufx_zuse;
      ScanProcPIG [SCANPROC_PI_TEX_GOUFX_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_goufx_ztest :
        csScan_16_565_scan_pi_tex_goufx_ztest;
      ScanProcPIG [SCANPROC_PI_TEX_GOUFXKEY_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_goufxkey_znone :
        csScan_16_565_scan_pi_tex_goufxkey_znone;
      ScanProcPIG [SCANPROC_PI_TEX_GOUFXKEY_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_goufxkey_zfil :
        csScan_16_565_scan_pi_tex_goufxkey_zfil;
      ScanProcPIG [SCANPROC_PI_TEX_GOUFXKEY_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_goufxkey_zuse :
        csScan_16_565_scan_pi_tex_goufxkey_zuse;
      ScanProcPIG [SCANPROC_PI_TEX_GOUFXKEY_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tex_goufxkey_ztest :
        csScan_16_565_scan_pi_tex_goufxkey_ztest;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUFX_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_goufx_znone :
        csScan_16_565_scan_pi_tile_tex_goufx_znone;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUFX_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_goufx_zfil :
        csScan_16_565_scan_pi_tile_tex_goufx_zfil;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUFX_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_goufx_zuse :
        csScan_16_565_scan_pi_tile_tex_goufx_zuse;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUFX_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_goufx_ztest :
        csScan_16_565_scan_pi_tile_tex_goufx_ztest;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUFXKEY_ZNONE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_goufxkey_znone :
        csScan_16_565_scan_pi_tile_tex_goufxkey_znone;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUFXKEY_ZFIL] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_goufxkey_zfil :
        csScan_16_565_scan_pi_tile_tex_goufxkey_zfil;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUFXKEY_ZUSE] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_goufxkey_zuse :
        csScan_16_565_scan_pi_tile_tex_goufxkey_zuse;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUFXKEY_ZTEST] = (pfmt.GreenBits == 5) ?
        csScan_16_555_scan_pi_tile_tex_goufxkey_ztest :
        csScan_16_565_scan_pi_tile_tex_goufxkey_ztest;
      break;

    case 4:
      if (do_alpha) ScanProc_Alpha = ScanProc_32_Alpha;

      ScanProc [SCANPROC_FLAT_ZNONE] = csScan_32_scan_flat_znone;
      ScanProc [SCANPROC_FLAT_ZFIL] = csScan_32_scan_flat_zfil;
      ScanProc [SCANPROC_FLAT_ZUSE] = csScan_32_scan_flat_zuse;
      ScanProc [SCANPROC_FLAT_ZTEST] = csScan_32_scan_flat_ztest;

      ScanProc [SCANPROC_TEX_ZNONE] = csScan_32_scan_tex_znone;
      ScanProc [SCANPROC_TEX_ZFIL] =
#if defined (DO_MMX)
        UseMMX ? csScan_32_mmx_scan_tex_zfil :
#endif
        csScan_32_scan_tex_zfil;
      ScanProc [SCANPROC_TEX_ZUSE] = csScan_32_scan_tex_zuse;
      ScanProc [SCANPROC_TEX_ZTEST] = csScan_32_scan_tex_ztest;

      ScanProc [SCANPROC_MAP_ZNONE] =
        csScan_32_scan_map_znone;
      ScanProc [SCANPROC_MAP_ZFIL] =
        bilinear_filter == 2 ? csScan_32_scan_map_filt2_zfil :
#if defined (DO_MMX)
        UseMMX ? csScan_32_mmx_scan_map_zfil :
#endif
        csScan_32_scan_map_zfil;
      ScanProc [SCANPROC_MAP_ZUSE] =
        bilinear_filter == 2 ? csScan_32_scan_map_filt2_zuse :
        csScan_32_scan_map_zuse;
      ScanProc [SCANPROC_MAP_ZTEST] =
        csScan_32_scan_map_ztest;

      ScanProc [SCANPROC_TEX_KEY_ZNONE] = csScan_32_scan_tex_key_znone;
      ScanProc [SCANPROC_TEX_KEY_ZFIL] = csScan_32_scan_tex_key_zfil;
      ScanProc [SCANPROC_TEX_KEY_ZUSE] = csScan_32_scan_tex_key_zuse;
      ScanProc [SCANPROC_TEX_KEY_ZTEST] = csScan_32_scan_tex_key_ztest;
      ScanProc [SCANPROC_MAP_KEY_ZNONE] = csScan_32_scan_map_key_znone;
      ScanProc [SCANPROC_MAP_KEY_ZFIL] = csScan_32_scan_map_key_zfil;
      ScanProc [SCANPROC_MAP_KEY_ZUSE] = csScan_32_scan_map_key_zuse;
      ScanProc [SCANPROC_MAP_KEY_ZTEST] = csScan_32_scan_map_key_ztest;

      ScanProc [SCANPROC_TEX_ALPHA_ZNONE] = csScan_32_scan_tex_alpha_znone;
      ScanProc [SCANPROC_TEX_ALPHA_ZFIL] = csScan_32_scan_tex_alpha_zfil;
      ScanProc [SCANPROC_TEX_ALPHA_ZUSE] = csScan_32_scan_tex_alpha_zuse;
      ScanProc [SCANPROC_TEX_ALPHA_ZTEST] = csScan_32_scan_tex_alpha_ztest;

      ScanProc [SCANPROC_MAP_ALPHA_ZNONE] = csScan_32_scan_map_alpha_znone;
      ScanProc [SCANPROC_MAP_ALPHA_ZFIL] = csScan_32_scan_map_alpha_zfil;
      ScanProc [SCANPROC_MAP_ALPHA_ZUSE] = csScan_32_scan_map_alpha_zuse;
      ScanProc [SCANPROC_MAP_ALPHA_ZTEST] = csScan_32_scan_map_alpha_ztest;

      ScanProc [SCANPROC_TEX_FX_ZNONE] = csScan_32_scan_tex_fx_znone;
      ScanProc [SCANPROC_TEX_FX_ZFIL] = csScan_32_scan_tex_fx_zfil;
      ScanProc [SCANPROC_TEX_FX_ZUSE] = csScan_32_scan_tex_fx_zuse;
      ScanProc [SCANPROC_TEX_FX_ZTEST] = csScan_32_scan_tex_fx_ztest;
      ScanProc [SCANPROC_MAP_FX_ZNONE] = csScan_32_scan_map_fx_znone;
      ScanProc [SCANPROC_MAP_FX_ZFIL] = csScan_32_scan_map_fx_zfil;
      ScanProc [SCANPROC_MAP_FX_ZUSE] = csScan_32_scan_map_fx_zuse;
      ScanProc [SCANPROC_MAP_FX_ZTEST] = csScan_32_scan_map_fx_ztest;

      ScanProc [SCANPROC_FOG] = csScan_32_scan_fog;
      ScanProc [SCANPROC_FOG_VIEW] = csScan_32_scan_fog_view;

      ScanProcPI [SCANPROC_PI_FLAT_ZNONE] = csScan_32_scan_pi_flat_znone;
      ScanProcPI [SCANPROC_PI_FLAT_ZFIL] = csScan_32_scan_pi_flat_zfil;
      ScanProcPI [SCANPROC_PI_FLAT_ZUSE] = csScan_32_scan_pi_flat_zuse;
      ScanProcPI [SCANPROC_PI_FLAT_ZTEST] = csScan_32_scan_pi_flat_ztest;
      ScanProcPI [SCANPROC_PI_TEX_ZNONE] = csScan_32_scan_pi_tex_znone;
      ScanProcPI [SCANPROC_PI_TEX_ZFIL] = csScan_32_scan_pi_tex_zfil;
      ScanProcPI [SCANPROC_PI_TEX_ZUSE] = csScan_32_scan_pi_tex_zuse;
      ScanProcPI [SCANPROC_PI_TEX_ZTEST] = csScan_32_scan_pi_tex_ztest;
      ScanProcPI [SCANPROC_PI_TEX_KEY_ZNONE] = csScan_32_scan_pi_tex_key_znone;
      ScanProcPI [SCANPROC_PI_TEX_KEY_ZFIL] = csScan_32_scan_pi_tex_key_zfil;
      ScanProcPI [SCANPROC_PI_TEX_KEY_ZUSE] = csScan_32_scan_pi_tex_key_zuse;
      ScanProcPI [SCANPROC_PI_TEX_KEY_ZTEST] = csScan_32_scan_pi_tex_key_ztest;
      ScanProcPI [SCANPROC_PI_TILE_TEX_ZNONE] = csScan_32_scan_pi_tile_tex_znone;
      ScanProcPI [SCANPROC_PI_TILE_TEX_ZFIL] = csScan_32_scan_pi_tile_tex_zfil;
      ScanProcPI [SCANPROC_PI_TILE_TEX_ZUSE] = csScan_32_scan_pi_tile_tex_zuse;
      ScanProcPI [SCANPROC_PI_TILE_TEX_ZTEST] = csScan_32_scan_pi_tile_tex_ztest;
      ScanProcPI [SCANPROC_PI_TILE_TEX_KEY_ZNONE] = csScan_32_scan_pi_tile_tex_key_znone;
      ScanProcPI [SCANPROC_PI_TILE_TEX_KEY_ZFIL] = csScan_32_scan_pi_tile_tex_key_zfil;
      ScanProcPI [SCANPROC_PI_TILE_TEX_KEY_ZUSE] = csScan_32_scan_pi_tile_tex_key_zuse;
      ScanProcPI [SCANPROC_PI_TILE_TEX_KEY_ZTEST] = csScan_32_scan_pi_tile_tex_key_ztest;
      ScanProcPI [SCANPROC_PI_FLAT_FX_ZNONE] = csScan_32_scan_pi_flat_fx_znone;
      ScanProcPI [SCANPROC_PI_FLAT_FX_ZFIL] = csScan_32_scan_pi_flat_fx_zfil;
      ScanProcPI [SCANPROC_PI_FLAT_FX_ZUSE] = csScan_32_scan_pi_flat_fx_zuse;
      ScanProcPI [SCANPROC_PI_FLAT_FX_ZTEST] = csScan_32_scan_pi_flat_fx_ztest;
      ScanProcPI [SCANPROC_PI_TEX_FX_ZNONE] = csScan_32_scan_pi_tex_fx_znone;
      ScanProcPI [SCANPROC_PI_TEX_FX_ZFIL] = csScan_32_scan_pi_tex_fx_zfil;
      ScanProcPI [SCANPROC_PI_TEX_FX_ZUSE] = csScan_32_scan_pi_tex_fx_zuse;
      ScanProcPI [SCANPROC_PI_TEX_FX_ZTEST] = csScan_32_scan_pi_tex_fx_ztest;
      ScanProcPI [SCANPROC_PI_TEX_FXKEY_ZNONE] = csScan_32_scan_pi_tex_fxkey_znone;
      ScanProcPI [SCANPROC_PI_TEX_FXKEY_ZFIL] = csScan_32_scan_pi_tex_fxkey_zfil;
      ScanProcPI [SCANPROC_PI_TEX_FXKEY_ZUSE] = csScan_32_scan_pi_tex_fxkey_zuse;
      ScanProcPI [SCANPROC_PI_TEX_FXKEY_ZTEST] = csScan_32_scan_pi_tex_fxkey_ztest;
      ScanProcPI [SCANPROC_PI_TILE_TEX_FX_ZNONE] = csScan_32_scan_pi_tile_tex_fx_znone;
      ScanProcPI [SCANPROC_PI_TILE_TEX_FX_ZFIL] = csScan_32_scan_pi_tile_tex_fx_zfil;
      ScanProcPI [SCANPROC_PI_TILE_TEX_FX_ZUSE] = csScan_32_scan_pi_tile_tex_fx_zuse;
      ScanProcPI [SCANPROC_PI_TILE_TEX_FX_ZTEST] = csScan_32_scan_pi_tile_tex_fx_ztest;
      ScanProcPI [SCANPROC_PI_TILE_TEX_FXKEY_ZNONE] = csScan_32_scan_pi_tile_tex_fxkey_znone;
      ScanProcPI [SCANPROC_PI_TILE_TEX_FXKEY_ZFIL] = csScan_32_scan_pi_tile_tex_fxkey_zfil;
      ScanProcPI [SCANPROC_PI_TILE_TEX_FXKEY_ZUSE] = csScan_32_scan_pi_tile_tex_fxkey_zuse;
      ScanProcPI [SCANPROC_PI_TILE_TEX_FXKEY_ZTEST] = csScan_32_scan_pi_tile_tex_fxkey_ztest;
      ScanProcPI [SCANPROC_PI_TEX_ALPHA_ZNONE] = csScan_32_scan_pi_tex_alpha_znone;
      ScanProcPI [SCANPROC_PI_TEX_ALPHA_ZFIL] = csScan_32_scan_pi_tex_alpha_zfil;
      ScanProcPI [SCANPROC_PI_TEX_ALPHA_ZUSE] = csScan_32_scan_pi_tex_alpha_zuse;
      ScanProcPI [SCANPROC_PI_TEX_ALPHA_ZTEST] = csScan_32_scan_pi_tex_alpha_ztest;

      ScanProcPIG [SCANPROC_PI_FLAT_GOU_ZNONE] = csScan_32_scan_pi_flat_gou_znone;
      ScanProcPIG [SCANPROC_PI_FLAT_GOU_ZFIL] = csScan_32_scan_pi_flat_gou_zfil;
      ScanProcPIG [SCANPROC_PI_FLAT_GOU_ZUSE] = csScan_32_scan_pi_flat_gou_zuse;
      ScanProcPIG [SCANPROC_PI_FLAT_GOU_ZTEST] = csScan_32_scan_pi_flat_gou_ztest;
      ScanProcPIG [SCANPROC_PI_TEX_GOU_ZNONE] = csScan_32_scan_pi_tex_gou_znone;
      ScanProcPIG [SCANPROC_PI_TEX_GOU_ZFIL] = csScan_32_scan_pi_tex_gou_zfil;
      ScanProcPIG [SCANPROC_PI_TEX_GOU_ZUSE] = csScan_32_scan_pi_tex_gou_zuse;
      ScanProcPIG [SCANPROC_PI_TEX_GOU_ZTEST] = csScan_32_scan_pi_tex_gou_ztest;
      ScanProcPIG [SCANPROC_PI_TEX_GOUKEY_ZNONE] = csScan_32_scan_pi_tex_goukey_znone;
      ScanProcPIG [SCANPROC_PI_TEX_GOUKEY_ZFIL] = csScan_32_scan_pi_tex_goukey_zfil;
      ScanProcPIG [SCANPROC_PI_TEX_GOUKEY_ZUSE] = csScan_32_scan_pi_tex_goukey_zuse;
      ScanProcPIG [SCANPROC_PI_TEX_GOUKEY_ZTEST] = csScan_32_scan_pi_tex_goukey_ztest;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOU_ZNONE] = csScan_32_scan_pi_tile_tex_gou_znone;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOU_ZFIL] = csScan_32_scan_pi_tile_tex_gou_zfil;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOU_ZUSE] = csScan_32_scan_pi_tile_tex_gou_zuse;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOU_ZTEST] = csScan_32_scan_pi_tile_tex_gou_ztest;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUKEY_ZNONE] = csScan_32_scan_pi_tile_tex_goukey_znone;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUKEY_ZFIL] = csScan_32_scan_pi_tile_tex_goukey_zfil;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUKEY_ZUSE] = csScan_32_scan_pi_tile_tex_goukey_zuse;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUKEY_ZTEST] = csScan_32_scan_pi_tile_tex_goukey_ztest;
      ScanProcPIG [SCANPROC_PI_FLAT_GOUFX_ZNONE] = csScan_32_scan_pi_flat_goufx_znone;
      ScanProcPIG [SCANPROC_PI_FLAT_GOUFX_ZFIL] = csScan_32_scan_pi_flat_goufx_zfil;
      ScanProcPIG [SCANPROC_PI_FLAT_GOUFX_ZUSE] = csScan_32_scan_pi_flat_goufx_zuse;
      ScanProcPIG [SCANPROC_PI_FLAT_GOUFX_ZTEST] = csScan_32_scan_pi_flat_goufx_ztest;
      ScanProcPIG [SCANPROC_PI_TEX_GOUFX_ZNONE] = csScan_32_scan_pi_tex_goufx_znone;
      ScanProcPIG [SCANPROC_PI_TEX_GOUFX_ZFIL] = csScan_32_scan_pi_tex_goufx_zfil;
      ScanProcPIG [SCANPROC_PI_TEX_GOUFX_ZUSE] = csScan_32_scan_pi_tex_goufx_zuse;
      ScanProcPIG [SCANPROC_PI_TEX_GOUFX_ZTEST] = csScan_32_scan_pi_tex_goufx_ztest;
      ScanProcPIG [SCANPROC_PI_TEX_GOUFXKEY_ZNONE] = csScan_32_scan_pi_tex_goufxkey_znone;
      ScanProcPIG [SCANPROC_PI_TEX_GOUFXKEY_ZFIL] = csScan_32_scan_pi_tex_goufxkey_zfil;
      ScanProcPIG [SCANPROC_PI_TEX_GOUFXKEY_ZUSE] = csScan_32_scan_pi_tex_goufxkey_zuse;
      ScanProcPIG [SCANPROC_PI_TEX_GOUFXKEY_ZTEST] = csScan_32_scan_pi_tex_goufxkey_ztest;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUFX_ZNONE] = csScan_32_scan_pi_tile_tex_goufx_znone;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUFX_ZFIL] = csScan_32_scan_pi_tile_tex_goufx_zfil;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUFX_ZUSE] = csScan_32_scan_pi_tile_tex_goufx_zuse;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUFX_ZTEST] = csScan_32_scan_pi_tile_tex_goufx_ztest;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUFXKEY_ZNONE] = csScan_32_scan_pi_tile_tex_goufxkey_znone;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUFXKEY_ZFIL] = csScan_32_scan_pi_tile_tex_goufxkey_zfil;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUFXKEY_ZUSE] = csScan_32_scan_pi_tile_tex_goufxkey_zuse;
      ScanProcPIG [SCANPROC_PI_TILE_TEX_GOUFXKEY_ZTEST] = csScan_32_scan_pi_tile_tex_goufxkey_ztest;
      break;
  } /* endswitch */

  static int o_rbits = -1, o_gbits, o_bbits;
  if ((o_rbits != pfmt.RedBits)
   || (o_gbits != pfmt.GreenBits)
   || (o_bbits != pfmt.BlueBits))
    /// make blending tables
    if(!is_for_procedural_textures) /// if this is not procedural manager
    {
      csScan_CalcBlendTables (Scan.BlendingTable, o_rbits = pfmt.RedBits,
        o_gbits = pfmt.GreenBits, o_bbits = pfmt.BlueBits);
    }
    else
    {
      csScan_CalcBlendTables (Scan.BlendingTableProc, o_rbits = pfmt.RedBits,
        o_gbits = pfmt.GreenBits, o_bbits = pfmt.BlueBits);
    }
}

csDrawScanline* csSoftwareGraphics3DCommon::ScanProc_16_Alpha
  (csSoftwareGraphics3DCommon *This, int alpha, bool keycolor, bool alphamap)
{
  csDrawScanline* const ScanProcs[24] = {
    0, csScan_16_scan_map_fixalpha50, csScan_16_scan_map_zfil, csScan_16_565_scan_map_fixalpha,
    0, csScan_16_scan_map_fixalpha50_key, csScan_16_scan_map_key_zfil, csScan_16_565_scan_map_fixalpha_key,
    0, csScan_16_565_scan_map_fixalpha50_alphamap, csScan_16_565_scan_map_alpha_zfil, csScan_16_565_scan_map_fixalpha_alphamap,
    0, csScan_16_scan_map_fixalpha50, csScan_16_scan_map_zfil, csScan_16_555_scan_map_fixalpha,
    0, csScan_16_scan_map_fixalpha50_key, csScan_16_scan_map_key_zfil, csScan_16_555_scan_map_fixalpha_key,
    0, csScan_16_555_scan_map_fixalpha50_alphamap, csScan_16_555_scan_map_alpha_zfil, csScan_16_555_scan_map_fixalpha_alphamap
  };

  Scan.AlphaMask = This->alpha_mask;
  Scan.AlphaFact = alpha;

  // In 16 bits mode we can get max 32 levels of transparency

  int scanproc = 3;

  // completely transparent?
  if (alpha <= 256/32)
    scanproc = 0;
  // approximate alpha from 47% to 53% with fast 50% routine
  if ((alpha >= 128 - 256/32) && (alpha <= 128 + 256/32))
    scanproc = 1;
  // completely opaque?
  if (alpha >= 256 - 256/32)
    scanproc = 2;
  if (keycolor) 
    scanproc += 4;
  else if (alphamap) 
    scanproc += 8;
  if (This->pfmt.GreenBits == 5)
    scanproc += 12;
  return ScanProcs[scanproc];
}

csDrawScanline* csSoftwareGraphics3DCommon::ScanProc_32_Alpha
  (csSoftwareGraphics3DCommon* /*This*/, int alpha, bool keycolor, bool alphamap)
{
  csDrawScanline* const ScanProcs[12] = {
    0, csScan_32_scan_map_fixalpha50, csScan_32_scan_map_zfil, csScan_32_scan_map_fixalpha,
    0, csScan_32_scan_map_fixalpha50_key, csScan_32_scan_map_key_zfil, csScan_32_scan_map_fixalpha_key,
    0, csScan_32_scan_map_fixalpha50_alphamap, csScan_32_scan_map_alpha_zfil, csScan_32_scan_map_fixalpha_alphamap};

  Scan.AlphaFact = alpha;

  int scanproc = 3;

  // completely transparent?
  if (alpha <= 1)
    scanproc = 0;
  // for 50% use fast routine
  else if (alpha >= 127 && alpha <= 129)
    scanproc = 1;
  // completely opaque?
  else if (alpha >= 254)
    scanproc = 2;
  if (keycolor) 
    scanproc += 4;
  else if (alphamap) 
    scanproc += 8;
  return ScanProcs[scanproc];
}

void csSoftwareGraphics3DCommon::Close ()
{
  if ((width == height) && (width == -1))
    return;

  /*while (fog_buffers)
  {
    FogBuffer* n = fog_buffers->next;
    delete fog_buffers;
    fog_buffers = n;
  }*/
  if (!partner)
  {
    delete tcache;
    tcache = 0;
    texman->Clear();
    texman->DecRef(); texman = 0;
    //vbufmgr->DecRef (); vbufmgr = 0;
  }
  if (clipper)
  {
    clipper->DecRef ();
    clipper = 0;
    cliptype = CS_CLIPPER_NONE;
  }

  delete [] z_buffer; z_buffer = 0;
  delete [] smaller_buffer; smaller_buffer = 0;
  delete [] line_table; line_table = 0;

  G2D->Close ();
  width = height = -1;
}

void csSoftwareGraphics3DCommon::SetDimensions (int nwidth, int nheight)
{
  display_width = nwidth;
  display_height = nheight;
  if (do_smaller_rendering)
  {
    width = nwidth/2;
    height = nheight/2;
  }
  else
  {
    width = nwidth;
    height = nheight;
  }
  width2 = width/2;
  height2 = height/2;

  delete [] smaller_buffer;
  smaller_buffer = 0;
  if (do_smaller_rendering)
  {
    smaller_buffer = new uint8 [(width*height) * pfmt.PixelBytes];
  }

  delete [] z_buffer;
  z_buffer = new uint32 [width*height];
  z_buf_size = sizeof (uint32)*width*height;

  delete [] line_table;
  line_table = new uint8* [height+1];
}

void csSoftwareGraphics3DCommon::SetClipper (iClipper2D* clip, int cliptype)
{
  if (clip) clip->IncRef ();
  if (clipper) clipper->DecRef ();
  clipper = clip;
  if (!clipper) cliptype = CS_CLIPPER_NONE;
  csSoftwareGraphics3DCommon::cliptype = cliptype;
}

bool csSoftwareGraphics3DCommon::BeginDraw (int DrawFlags)
{
  dpfx_valid = false;

  if ((G2D->GetWidth() != display_width) ||
      (G2D->GetHeight() != display_height))
    SetDimensions (G2D->GetWidth(), G2D->GetHeight());

  // if 2D graphics is not locked, lock it
  if ((DrawFlags & (CSDRAW_2DGRAPHICS | CSDRAW_3DGRAPHICS))
   && (!(DrawMode & (CSDRAW_2DGRAPHICS | CSDRAW_3DGRAPHICS))))
  {
    if (!G2D->BeginDraw())
      return false;
  }

  // Initialize the line table.
  int i;
  for (i = 0 ; i < height ; i++)
    if (do_smaller_rendering)
      line_table[i] = smaller_buffer + ((i*width)*pfmt.PixelBytes);
    else
      line_table[i] = G2D->GetPixelAt (0, i);

  if (render_target)
  {
    int txt_w, txt_h;
    render_target->GetMipMapDimensions (0, txt_w, txt_h);
    if (!rt_cliprectset)
    {
      G2D->GetClipRect (rt_old_minx, rt_old_miny, rt_old_maxx, rt_old_maxy);
      G2D->SetClipRect (-1, -1, txt_w+1, txt_h+1);
      rt_cliprectset = true;
    }

    if (!rt_onscreen)
    {
      int txt_w, txt_h;
      render_target->GetMipMapDimensions (0, txt_w, txt_h);
      csSoftwareTextureHandle* tex_mm = (csSoftwareTextureHandle *)
	    render_target->GetPrivateObject ();
      csSoftwareTexture *tex_0 = (csSoftwareTexture*)(tex_mm->get_texture (0));
      int x, y;
      uint8* bitmap = tex_0->bitmap;
      switch (pfmt.PixelBytes)
      {
	case 2:
	  {
	    uint16* pal2glob = (uint16*)(tex_mm->GetPaletteToGlobal ());
	    for (y = txt_h-1 ; y >= 0 ; y--)
	    {
              uint16* d = (uint16*)line_table[y];
	      for (x = 0 ; x < txt_w ; x++)
	      {
	        uint8 pix = *bitmap++;
		*d++ = pal2glob[pix];
	      }
	    }
	  }
	  break;
	case 4:
	  {
	    uint32* pal2glob = (uint32*)(tex_mm->GetPaletteToGlobal ());
	    for (y = txt_h-1 ; y >= 0 ; y--)
	    {
              uint32* d = (uint32*)line_table[y];
	      for (x = 0 ; x < txt_w ; x++)
	      {
	        uint8 pix = *bitmap++;
		*d++ = pal2glob[pix];
	      }
	    }
	  }
	  break;
      }
      rt_onscreen = true;
    }
  }

  if (DrawFlags & CSDRAW_CLEARZBUFFER)
    memset (z_buffer, 0, z_buf_size);

  if (DrawFlags & CSDRAW_CLEARSCREEN)
    G2D->Clear (0);

  if (DrawFlags & CSDRAW_3DGRAPHICS)
  {
    dbg_current_polygon = 0;
  }
  else if (DrawMode & CSDRAW_3DGRAPHICS)
  {
    // Finished 3D drawing. If we are simulating the flush output to real frame buffer.
    if (do_smaller_rendering)
    {
      int x, y;
      switch (pfmt.PixelBytes)
      {
        case 2:
          if (pfmt.GreenBits == 5)
            for (y = 0 ; y < height ; y++)
            {
              uint16* src = (uint16*)line_table[y];
              uint16* dst1 = (uint16*)G2D->GetPixelAt (0, y+y);
              uint16* dst2 = (uint16*)G2D->GetPixelAt (0, y+y+1);
              for (x = 0 ; x < width ; x++)
              {
                dst1[x+x] = src[x];
                dst1[x+x+1] = ((src[x]&0x7bde)>>1) + ((src[x+1]&0x7bde)>>1);
                dst2[x+x] = ((src[x]&0x7bde)>>1) + ((src[x+width]&0x7bde)>>1);
                dst2[x+x+1] = ((dst1[x+x+1]&0x7bde)>>1) + ((dst2[x+x]&0x7bde)>>1);
              }
            }
          else
            for (y = 0 ; y < height ; y++)
            {
              uint16* src = (uint16*)line_table[y];
              uint16* dst1 = (uint16*)G2D->GetPixelAt (0, y+y);
              uint16* dst2 = (uint16*)G2D->GetPixelAt (0, y+y+1);
              for (x = 0 ; x < width ; x++)
              {
                dst1[x+x] = src[x];
                dst1[x+x+1] = ((src[x]&0xf7de)>>1) + ((src[x+1]&0xf7de)>>1);
                dst2[x+x] = ((src[x]&0xf7de)>>1) + ((src[x+width]&0xf7de)>>1);
                dst2[x+x+1] = ((dst1[x+x+1]&0xf7de)>>1) + ((dst2[x+x]&0xf7de)>>1);
              }
            }
          break;
        case 4:
          for (y = 0 ; y < height ; y++)
          {
            uint32* src = (uint32*)line_table[y];
            uint32* dst1 = (uint32*)G2D->GetPixelAt (0, y+y);
            uint32* dst2 = (uint32*)G2D->GetPixelAt (0, y+y+1);
            for (x = 0 ; x < width ; x++)
            {
              dst1[x+x] = src[x];
              dst1[x+x+1] = ((src[x]&0xfefefe)>>1) + ((src[x+1]&0xfefefe)>>1);
              dst2[x+x] = ((src[x]&0xfefefe)>>1) + ((src[x+width]&0xfefefe)>>1);
              dst2[x+x+1] = ((dst1[x+x+1]&0xfefefe)>>1) + ((dst2[x+x]&0xfefefe)>>1);
            }
          }
          break;
      }
    }
  }

  DrawMode = DrawFlags;
  return true;
}

void csSoftwareGraphics3DCommon::Print (csRect *area)
{
  G2D->Print (area);
  if (do_interlaced != -1)
    do_interlaced ^= 1;
  if (tcache)
    tcache->frameno++;
}


void csSoftwareGraphics3DCommon::FinishDraw ()
{
  if (DrawMode & (CSDRAW_2DGRAPHICS | CSDRAW_3DGRAPHICS))
    G2D->FinishDraw ();
  DrawMode = 0;

  if (render_target)
  {
    if (rt_cliprectset)
    {
      rt_cliprectset = false;
      G2D->SetClipRect (rt_old_minx, rt_old_miny, rt_old_maxx, rt_old_maxy);
    }

    if (rt_onscreen)
    {
      rt_onscreen = false;
      int txt_w, txt_h;
      render_target->GetMipMapDimensions (0, txt_w, txt_h);
      csSoftwareTextureHandle* tex_mm = (csSoftwareTextureHandle *)
	    render_target->GetPrivateObject ();
      //tex_mm->DeleteMipmaps ();
      tex_mm->UpdateTexture ();
      csSoftwareTexture *tex_0 = (csSoftwareTexture*)(tex_mm->get_texture (0));
      int x, y;
      uint8* bitmap = tex_0->bitmap;
      switch (pfmt.PixelBytes)
      {
	case 2:
	  {
	    for (y = 0 ; y < txt_h ; y++)
	    {
              uint16* d = (uint16*)line_table[y];
	      for (x = 0 ; x < txt_w ; x++)
	      {
		uint16 pix = *d++;
		uint8 pix8;
		pix8 = (((pix & pfmt.RedMask) >> pfmt.RedShift) >>
			(pfmt.RedBits - 3)) << 5;
		pix8 |= (((pix & pfmt.GreenMask) >> pfmt.GreenShift) >>
			(pfmt.GreenBits - 3)) << 2;
		pix8 |= (((pix & pfmt.BlueMask) >> pfmt.BlueShift) >>
			(pfmt.BlueBits - 2));
		*bitmap++ = pix8;
	      }
	    }
	  }
	  break;
	case 4:
	  {
	    for (y = 0 ; y < txt_h ; y++)
	    {
              uint32* d = (uint32*)line_table[y];
	      for (x = 0 ; x < txt_w ; x++)
	      {
		uint32 pix = *d++;
		uint8 pix8;
		pix8 = (((pix & pfmt.RedMask) >> pfmt.RedShift) >> 5) << 5;
		pix8 |= (((pix & pfmt.GreenMask) >> pfmt.GreenShift) >> 5) << 2;
		pix8 |= (((pix & pfmt.BlueMask) >> pfmt.BlueShift) >> 6);
		*bitmap++ = pix8;
	      }
	    }
	  }
	  break;
      }
    }
  }
  render_target = 0;
}

#define SMALL_D 0.01

/**
 * The engine often generates "empty" polygons, for example
 * (2, 2) - (317,2) - (317,2) - (2, 2)
 * To avoid too much computations, DrawPolygon detects such polygons by
 * counting the number of "real" vertices (i.e. the number of vertices,
 * distance between which is bigger that some amount). The "right" formula
 * for distance is sqrt(dX^2 + dY^2) but to avoid root and multiply
 * DrawPolygon checks abs(dX) + abs(dY). This is enough.
 */
#define VERTEX_NEAR_THRESHOLD   0.001

/*
 * For the four interpolation modes.
 */
struct
{
  int step1, shift1;
  int step2, shift2;
  int step3, shift3;
  int step4, shift4;
} inter_modes[4] =
{
  { 128, 7, 64, 6, 32, 5, 16, 4 },      // Selective
  { 32, 5, 32, 5, 32, 5, 32, 5 },       // 32-steps
  { 16, 4, 16, 4, 16, 4, 16, 4 },       // 16-steps
  { 8, 3, 8, 3, 8, 3, 8, 3 }            // 8-steps
};

inline static void SelectInterpolationStep (float M)
{
  // Select the right interpolation factor based on the z-slope of our
  // polygon. This will greatly increase the speed of polygons which are
  // horizontally constant in z.
  if (ABS (M) < .000001)
  {
    Scan.InterpolStep = inter_modes[Scan.InterpolMode].step1;
    Scan.InterpolShift = inter_modes[Scan.InterpolMode].shift1;
  }
  else if (ABS (M) < .00005)
  {
    Scan.InterpolStep = inter_modes[Scan.InterpolMode].step2;
    Scan.InterpolShift = inter_modes[Scan.InterpolMode].shift2;
  }
  else if (ABS (M) < .001)
  {
    Scan.InterpolStep = inter_modes[Scan.InterpolMode].step3;
    Scan.InterpolShift = inter_modes[Scan.InterpolMode].shift3;
  }
  else
  {
    Scan.InterpolStep = inter_modes[Scan.InterpolMode].step4;
    Scan.InterpolShift = inter_modes[Scan.InterpolMode].shift4;
  }
}

void csSoftwareGraphics3DCommon::DrawPolygonFlat (G3DPolygonDPF& poly)
{
  int i;
  int max_i, min_i;
  float max_y, min_y;
  unsigned char *d;
  uint32 *z_buf;

  if (poly.num < 3) return;

  // Get the plane normal of the polygon. Using this we can calculate
  // '1/z' at every screen space point.
  float Ac, Bc, Cc, Dc, inv_Dc;
  Ac = poly.normal.A ();
  Bc = poly.normal.B ();
  Cc = poly.normal.C ();
  Dc = poly.normal.D ();

  float M, N, O;
  if (ABS (Dc) < SMALL_D) Dc = -SMALL_D;
  if (ABS (Dc) < SMALL_D)
  {
    // The Dc component of the plane normal is too small. This means that
    // the plane of the polygon is almost perpendicular to the eye of the
    // viewer. In this case, nothing much can be seen of the plane anyway
    // so we just take one value for the entire polygon.
    M = 0;
    N = 0;
    // For O choose the transformed z value of one vertex.
    // That way Z buffering should at least work.
    O = 1 / poly.z_value;
  }
  else
  {
    inv_Dc = 1/Dc;
    M = -Ac * inv_Dc * inv_aspect;
    N = -Bc * inv_Dc * inv_aspect;
    O = -Cc * inv_Dc;
  }

  // Compute the min_y and max_y for this polygon in screen space coordinates.
  // We are going to use these to scan the polygon from top to bottom.
  min_i = max_i = 0;
  min_y = max_y = poly.vertices[0].y;
  // count 'real' number of vertices
  int num_vertices = 1;
  for (i = 1 ; i < poly.num ; i++)
  {
    // Sometimes double precision in the clipper is not enough.
    // Do an epsilon fuzz so not to reject cases when bounds exceeded
    // by less than epsilon.
    if (((poly.vertices[i].x + EPSILON) < 0) ||
	((poly.vertices[i].x - EPSILON) > width))
      return;

    if (poly.vertices[i].y > max_y)
    {
      max_y = poly.vertices[i].y;
      max_i = i;
    }
    else if (poly.vertices[i].y < min_y)
    {
      min_y = poly.vertices[i].y;
      min_i = i;
    }
    // theoretically we should do sqrt(dx^2+dy^2) here, but
    // we can approximate it by just abs(dx)+abs(dy)
    if ((ABS (poly.vertices [i].x - poly.vertices [i - 1].x)
       + ABS (poly.vertices [i].y - poly.vertices [i - 1].y))
       	> VERTEX_NEAR_THRESHOLD)
      num_vertices++;
    // the above does not catch cases like this:
    // p1   p2   p3
    // x----x----x    a degenerated hexagon :)  norman
    // p6   p5   p4
  }

  if (((min_y + EPSILON) < 0) ||
      ((max_y - EPSILON) > height))
    return;

  // if this is a 'degenerate' polygon, skip it.
  if (num_vertices < 3)
    return;

  // For debugging: if we reach the maximum number of polygons to draw we
  // simply stop.
  dbg_current_polygon++;
  if (dbg_current_polygon == dbg_max_polygons_to_draw-1)
    return;
  if (dbg_current_polygon >= dbg_max_polygons_to_draw-1)
    return;

  iPolygonTexture *tex = 0;
  iLightMap *lm = 0;
  if (do_lighting)
  {
    tex = poly.poly_texture;
    lm = tex->GetLightMap ();
  }

  csRGBpixel color;
  iTextureHandle *txt_handle = poly.mat_handle->GetTexture ();
  if (txt_handle)
    txt_handle->GetMeanColor (color.red, color.green, color.blue);
  else
    poly.mat_handle->GetFlatColor (color);

  if (lm)
  {
    // Lighted polygon
    int lr, lg, lb;
    lm->GetMeanLighting (lr, lg, lb);

    // Make lighting a little bit brighter because average
    // lighting is really dark otherwise.
    lr = lr << 1; if (lr > 255) lr = 255;
    lg = lg << 1; if (lg > 255) lg = 255;
    lb = lb << 1; if (lb > 255) lb = 255;

    Scan.FlatColor = texman->encode_rgb ((color.red * lr) >> 8,
      (color.green * lg) >> 8, (color.blue * lb) >> 8);
  }
  Scan.FlatColor = texman->encode_rgb (color.red, color.green, color.blue);

  Scan.M = M;

  int alpha = poly.mixmode & CS_FX_MASK_ALPHA;
  // Select the right scanline drawing function.
  if (do_alpha && (alpha || (txt_handle && txt_handle->GetKeyColor ())))
    return;
  int scan_index = SCANPROC_FLAT_ZNONE;
  if (z_buf_mode == CS_ZBUF_FILL) scan_index++;
  else if (z_buf_mode == CS_ZBUF_USE) scan_index += 2;
  else if (z_buf_mode == CS_ZBUF_TEST) scan_index += 3;
  csDrawScanline* dscan = ScanProc [scan_index];
  if (!dscan)
    return;				// Nothing to do.

  // Scan both sides of the polygon at once.
  // We start with two pointers at the top (as seen in y-inverted
  // screen-space: bottom of display) and advance them until both
  // join together at the bottom. The way this algorithm works, this
  // should happen automatically; the left pointer is only advanced
  // when it is further away from the bottom than the right pointer
  // and vice versa.
  // Using this we effectively partition our polygon in trapezoids
  // with at most two triangles (one at the top and one at the bottom).

  int scanL1, scanL2, scanR1, scanR2;   // scan vertex left/right start/final
  float sxL, sxR, dxL, dxR;             // sl X left/right and deltas
  int sy, fyL, fyR;                     // sl Y, final Y left, final Y right
  int xL, xR;
  int screenY;

  sxL = sxR = dxL = dxR = 0;
  scanL2 = scanR2 = max_i;
  sy = fyL = fyR = QRound (poly.vertices [scanL2].y);

  for ( ; ; )
  {
    //-----
    // We have reached the next segment. Recalculate the slopes.
    //-----
    bool leave;
    do
    {
      leave = true;
      if (sy <= fyR)
      {
        // Check first if polygon has been finished
        if (scanR2 == min_i)
	  return;
        scanR1 = scanR2;
	if (++scanR2 >= poly.num)
	  scanR2 = 0;

        leave = false;
        fyR = QRound (poly.vertices [scanR2].y);
        if (sy <= fyR)
          continue;

        float dyR = (poly.vertices [scanR1].y - poly.vertices [scanR2].y);
        if (dyR)
        {
          sxR = poly.vertices [scanR1].x;
          dxR = (poly.vertices [scanR2].x - sxR) / dyR;
          // horizontal pixel correction
          sxR += dxR * (poly.vertices [scanR1].y - (float (sy) - 0.5));
        } /* endif */
      } /* endif */
      if (sy <= fyL)
      {
        scanL1 = scanL2;
	if (--scanL2 < 0)
	  scanL2 = poly.num - 1;

        leave = false;
        fyL = QRound (poly.vertices [scanL2].y);
        if (sy <= fyL)
          continue;

        float dyL = (poly.vertices [scanL1].y - poly.vertices [scanL2].y);
        if (dyL)
        {
          sxL = poly.vertices [scanL1].x;
          dxL = (poly.vertices [scanL2].x - sxL) / dyL;
          // horizontal pixel correction
          sxL += dxL * (poly.vertices [scanL1].y - (float (sy) - 0.5));
        } /* endif */
      } /* endif */
    } while (!leave); /* enddo */

    // Steps for interpolating vertically over scanlines.
    float vd_inv_z = - dxL * M + N;

    float cx = (sxL - float (width2));
    float cy = (float (sy) - 0.5 - float (height2));
    float inv_z = M * cx + N * cy + O;

    // Find the trapezoid top (or bottom in inverted Y coordinates)
    int fin_y;
    if (fyL > fyR)
      fin_y = fyL;
    else
      fin_y = fyR;

    screenY = height - sy;

    while (sy > fin_y)
    {
      //@@@ Normally I would not need to have to check against screen
      // boundaries but apparantly there are cases where this test is
      // needed (maybe a bug in the clipper?). I have to look at this later.
#if 1
      if ((sy & 1) != do_interlaced)
#else
      if (((sy & 1) != do_interlaced) && (sxR >= 0) && (sxL < width) && (screenY >= 0) && (screenY < height))
#endif
      {
        // Compute the rounded screen coordinates of horizontal strip
        xL = QRound (sxL);
        xR = QRound (sxR);

        d = line_table [screenY] + (xL << pixel_shift);
        z_buf = z_buffer + width * screenY + xL;

        // do not draw the rightmost pixel - it will be covered
        // by neightbour polygon's left bound
        dscan (xR - xL, d, z_buf, inv_z, 0, 0);
      }

      sxL += dxL;
      sxR += dxR;
      inv_z -= vd_inv_z;
      sy--;
      screenY++;
    } /* endwhile */
  } /* endfor */
}
#if 0

void csSoftwareGraphics3DCommon::DrawPolygon (G3DPolygonDP& poly)
{
  if (z_buf_mode == CS_ZBUF_FILLONLY)
  {
    DrawFogPolygon (0, poly, CS_FOG_BACK);
    return;
  }

  if (!do_textured || !poly.mat_handle->GetTexture ())
  {
    DrawPolygonFlat (poly);
    return;
  }

  int i;
  float P1, P2, P3, P4;
  float Q1, Q2, Q3, Q4;
  int max_i, min_i, min_z_i;
  float max_y, min_y;
  float min_z;
  unsigned char *d;
  uint32 *z_buf;

  if (poly.num < 3)
    return;

  // Get the plane normal of the polygon. Using this we can calculate
  // '1/z' at every screen space point.
  float Ac, Bc, Cc, Dc, inv_Dc;
  Ac = poly.normal.A ();
  Bc = poly.normal.B ();
  Cc = poly.normal.C ();
  Dc = poly.normal.D ();

  float M, N, O;
  if (ABS (Dc) < SMALL_D) Dc = -SMALL_D;
  if (ABS (Dc) < SMALL_D)
  {
    // The Dc component of the plane normal is too small. This means that
    // the plane of the polygon is almost perpendicular to the eye of the
    // viewer. In this case, nothing much can be seen of the plane anyway
    // so we just take one value for the entire polygon.
    M = 0;
    N = 0;
    // For O choose the transformed z value of one vertex.
    // That way Z buffering should at least work.
    O = 1/poly.z_value;
  }
  else
  {
    inv_Dc = 1/Dc;
    M = -Ac*inv_Dc*inv_aspect;
    N = -Bc*inv_Dc*inv_aspect;
    O = -Cc*inv_Dc;
  }

  // Compute the min_y and max_y for this polygon in screen space coordinates.
  // We are going to use these to scan the polygon from top to bottom.
  // Also compute the min_z in camera space coordinates. This is going to be
  // used for mipmapping.

  // Sometimes double precision in the clipper is not enough. Do an epsilon fuzz
  // so not to reject cases when bounds exceeded by less than epsilon. smgh
  if (((poly.vertices[0].x + EPSILON) < 0) ||
      ((poly.vertices[0].x - EPSILON) > width))
    return;

  min_i = max_i = min_z_i = 0;
  min_y = max_y = poly.vertices[0].y;

  float t = (M == 0.f ? N : N/M);
  bool M_neg = (M<0.f);
  bool M_zero = (M == 0.f);
  min_z = (M_zero ? 0 : poly.vertices[0].x) + t*poly.vertices[0].y;

  // count 'real' number of vertices
  int num_vertices = 1;
  for (i = 1 ; i < poly.num ; i++)
  {
    if (((poly.vertices[i].x + EPSILON) < 0) ||
	((poly.vertices[i].x - EPSILON) > width))
      return;

    if (poly.vertices[i].y > max_y)
    {
      max_y = poly.vertices[i].y;
      max_i = i;
    }
    else if (poly.vertices[i].y < min_y)
    {
      min_y = poly.vertices[i].y;
      min_i = i;
    }
    float inv_z = (M_zero ? 0 : poly.vertices[i].x) + t*poly.vertices[i].y;

    if ((inv_z > min_z) ^ M_neg)
    {
      min_z = inv_z;
      min_z_i = i;
    }
    // theoretically we should do sqrt(dx^2+dy^2) here, but
    // we can approximate it by just abs(dx)+abs(dy)
    if ((ABS (poly.vertices [i].x - poly.vertices [i - 1].x)
       + ABS (poly.vertices [i].y - poly.vertices [i - 1].y))
       	> VERTEX_NEAR_THRESHOLD)
      num_vertices++;
  }

  if (((min_y + EPSILON) < 0) ||
      ((max_y - EPSILON) > height))
    return;

  min_z = M * (poly.vertices[min_z_i].x - width2)
        + N * (poly.vertices[min_z_i].y - height2) + O;

  // if this is a 'degenerate' polygon, skip it.
  if (num_vertices < 3)
    return;

  // For debugging: if we reach the maximum number of polygons to draw
  // we simply stop.
  dbg_current_polygon++;
  if (dbg_current_polygon == dbg_max_polygons_to_draw-1)
    return;
  if (dbg_current_polygon >= dbg_max_polygons_to_draw-1)
    return;

  // @@@ The texture transform matrix is currently written as T = M*(C-V)
  // (with V being the transform vector, M the transform matrix, and C
  // the position in camera space coordinates. It would be better (more
  // suitable for the following calculations) if it would be written
  // as T = M*C - V.
  P1 = poly.plane.m_cam2tex->m11;
  P2 = poly.plane.m_cam2tex->m12;
  P3 = poly.plane.m_cam2tex->m13;
  P4 = - (P1 * poly.plane.v_cam2tex->x
        + P2 * poly.plane.v_cam2tex->y
        + P3 * poly.plane.v_cam2tex->z);
  Q1 = poly.plane.m_cam2tex->m21;
  Q2 = poly.plane.m_cam2tex->m22;
  Q3 = poly.plane.m_cam2tex->m23;
  Q4 = - (Q1 * poly.plane.v_cam2tex->x
        + Q2 * poly.plane.v_cam2tex->y
        + Q3 * poly.plane.v_cam2tex->z);

  iPolygonTexture *tex = poly.poly_texture;
  const csLightMapMapping& mapping = tex->GetMapping ();
  csSoftwareTextureHandle *tex_mm =
    (csSoftwareTextureHandle *)poly.mat_handle->GetTexture ()
    	->GetPrivateObject ();

  float fdu, fdv;
  if (tex)
  {
    fdu = mapping.GetFDU ();
    fdv = mapping.GetFDV ();
  }
  else
  {
    fdu = 0;
    fdv = 0;
  }

  // Now we're in the right shape to determine the mipmap level.
  // We'll use the following formula to determine the required level of
  // mipmap: we'll take the x and y screen coordinates of nearest point.
  // Now we compute the difference between u (x + 1, y + 1) and u (x, y),
  // and between v (x + 1, y + 1) and v (x, y). We're using the following
  // formulas in our texture mapping to compute u, v an z values at any
  // given x, y screen point:
  //
  //   u/z (x, y) = J1 * x + J2 * y + J3
  //   v/z (x, y) = K1 * x + K2 * y + K3
  //   1/z (x, y) = M * x + N * y + O
  //
  // Thus u (x, y) = u/z (x, y) / 1/z (x, y). Similary for v (x, y).
  // Also we know that at nearest polygon point (where we will research
  // for texel density) the 1/z value is already stored in min_z variable
  // (i.e. M * x + N * y + O = min_z). Thus we compute the formula for
  // u (x + 1, y + 1) - u (x, y):
  //
  //   J1 * (x + 1) + J2 * (y + 1) + J3     J1 * x + J2 * y + J3
  //  ---------------------------------- - ---------------------- =
  //    M * (x + 1) + N * (x + 1) + O        M * x + N * y + O
  //
  //  min_z*(J1*(x+1) + J2*(y+1) + J3) - (min_z + M + N)*(J1*x + J2*y + J3)
  //  --------------------------------------------------------------------- =
  //                        min_z * (min_z + M + N)
  //
  //   min_z * (J1 + J2) - (M + N) * (J1 * x + J2 * y + J3)
  //  ------------------------------------------------------
  //                     min_z * (min_z + M + N)
  //
  // Thus we can compute delta U and delta V (we'll refer them as du and dv),
  // the amount of texels we should move in texture space if we move by 1 in
  // x and y direction.
  //
  // Now we should take sqrt (du^2 + dv^2) and decide the mipmap level
  // depending on this value. We can ommit the square root and work with the
  // squared value. Thus, we can select the required mipmap level depending
  // on this value this way:
  //
  //   if <= 2*2, mipmap level 0
  //   if <= 4*4, mipmap level 1
  //   if <= 8*8, mipmap level 2
  //   if above, mipmap level 3

  // Mipmapping.
  int mipmap;
  if (rstate_mipmap == 1)
    mipmap = 0;
  else if (rstate_mipmap == 0)
  {
    if (!min_z)
      mipmap = 0;
    else if (ABS (Dc) < SMALL_D)
      mipmap = 3;
    else
    {
      // Mipmap level 0 size
      int m0w = tex_mm->get_texture (0)->get_width ();
      int m0h = tex_mm->get_texture (0)->get_height ();

      float _P1 = P1*m0w, _P2 = P2*m0w, _P3 = P3*m0w, _P4 = P4*m0w - fdu;
      float _Q1 = Q1*m0h, _Q2 = Q2*m0h, _Q3 = Q3*m0h, _Q4 = Q4*m0h - fdv;

      float J1 = _P1 * inv_aspect + _P4 * M;
      float J2 = _P2 * inv_aspect + _P4 * N;
      float J3 = _P3              + _P4 * O;
      float K1 = _Q1 * inv_aspect + _Q4 * M;
      float K2 = _Q2 * inv_aspect + _Q4 * N;
      float K3 = _Q3              + _Q4 * O;

      float x = poly.vertices [min_z_i].x - width2;
      float y = poly.vertices [min_z_i].y - height2;

      float du = (min_z * (J1 + J2) - (M + N) * (J1 * x + J2 * y + J3)) /
                 (min_z * (min_z + M + N));
      float dv = (min_z * (K1 + K2) - (M + N) * (K1 * x + K2 * y + K3)) /
                 (min_z * (min_z + M + N));

      float mipmap_sel = mipmap_coef * (du * du + dv * dv);

      // Now look which mipmap we should use
      if (mipmap_sel <= 2 * 2)
        mipmap = 0;
      else if (mipmap_sel <= 4 * 4)
        mipmap = 1;
      else if (mipmap_sel <= 8 * 8)
        mipmap = 2;
      else
        mipmap = 3;
    }
  }
  else
    mipmap = rstate_mipmap - 1;

  // If mipmap is too small or not available, use the mipmap level
  // that is still visible or available ...
  int shf_u = mapping.GetShiftU () - mipmap;
  if (shf_u < 0) mipmap += shf_u;
  if (mipmap < 0) mipmap = 0;
  while (mipmap && !tex_mm->get_texture (mipmap))
    mipmap--;

  if (mipmap)
  {
    int duv = (1 << mipmap);
    fdu /= duv;
    fdv /= duv;
  }

  // Now get the unlighted texture corresponding to mipmap level we choosen
  csSoftwareTexture *txt_unl = (csSoftwareTexture *)tex_mm->get_texture (
  	mipmap);

  // Check if polygon has a lightmap (i.e. if it is lighted)
  bool has_lightmap = tex->GetLightMap () && do_lighting && !poly.do_fullbright;
  if (has_lightmap)
  {
    // If there is a lightmap we check if the size of the lighted
    // texture would not exceed MAX_LIGHTMAP_SIZE pixels. In that case we
    // revert to unlighted texture mapping.
    long size = mapping.GetWidth () * mapping.GetHeight ();
    if (size > MAX_LIGHTMAP_SIZE) has_lightmap = false;
  }

  // Continue with texture mapping
  Scan.tw = txt_unl->get_width ();
  Scan.th = txt_unl->get_height ();

  P1 *= Scan.tw; P2 *= Scan.tw; P3 *= Scan.tw; P4 *= Scan.tw;
  Q1 *= Scan.th; Q2 *= Scan.th; Q3 *= Scan.th; Q4 *= Scan.th;
  P4 -= fdu; Q4 -= fdv;

  // Precompute everything so that we can calculate (u,v) (texture space
  // coordinates) for every (sx,sy) (screen space coordinates). We make
  // use of the fact that 1/z, u/z and v/z are linear in screen space.
  float J1, J2, J3, K1, K2, K3;
  if (ABS (Dc) < SMALL_D)
  {
    // The Dc component of the plane of the polygon is too small.
    J1 = J2 = J3 = 0;
    K1 = K2 = K3 = 0;
  }
  else
  {
    J1 = P1 * inv_aspect + P4 * M;
    J2 = P2 * inv_aspect + P4 * N;
    J3 = P3              + P4 * O;
    K1 = Q1 * inv_aspect + Q4 * M;
    K2 = Q2 * inv_aspect + Q4 * N;
    K3 = Q3              + Q4 * O;
  }

  SelectInterpolationStep (M);

  // Steps for interpolating horizontally accross a scanline.
  Scan.M = M;
  Scan.J1 = J1;
  Scan.K1 = K1;
  Scan.dM = M * Scan.InterpolStep;
  Scan.dJ1 = J1 * Scan.InterpolStep;
  Scan.dK1 = K1 * Scan.InterpolStep;

  // Scan both sides of the polygon at once.
  // We start with two pointers at the top (as seen in y-inverted
  // screen-space: bottom of display) and advance them until both
  // join together at the bottom. The way this algorithm works, this
  // should happen automatically; the left pointer is only advanced
  // when it is further away from the bottom than the right pointer
  // and vice versa.
  // Using this we effectively partition our polygon in trapezoids
  // with at most two triangles (one at the top and one at the bottom).

  int scanL1, scanL2, scanR1, scanR2;   // scan vertex left/right start/final
  float sxL, sxR, dxL, dxR;             // sl X left/right and deltas
  int sy, fyL, fyR;                     // sl Y, final Y left, final Y right
  int xL, xR;
  int screenY;

  // If we have a lighted polygon, prepare the texture
  if (has_lightmap)
  {
    sxL = sxR = dxL = dxR = 0;
    scanL2 = scanR2 = max_i;
    sy = fyL = fyR = QRound (poly.vertices [scanL2].y);

    // Find the largest texture rectangle that is going to be displayed
    float u_min = +99999999;
    float v_min = +99999999;
    float u_max = -99999999;
    float v_max = -99999999;

    // Do a quick polygon scan at the edges to find mi/max u and v
    for ( ; ; )
    {
      bool leave;
      do
      {
        leave = true;
        if (sy <= fyR)
        {
          // Check first if polygon has been finished
          if (scanR2 == min_i)
            goto texr_done;
          scanR1 = scanR2;
          if (++scanR2 >= poly.num)
            scanR2 = 0;

          leave = false;
          fyR = QRound (poly.vertices [scanR2].y);
          if (sy <= fyR)
            continue;

          float dyR = (poly.vertices [scanR1].y - poly.vertices [scanR2].y);
          if (dyR)
          {
            sxR = poly.vertices [scanR1].x;
            dxR = (poly.vertices [scanR2].x - sxR) / dyR;
            // horizontal pixel correction
            sxR += dxR * (poly.vertices [scanR1].y - (float (sy) - 0.5));
          }
        }
        if (sy <= fyL)
        {
          scanL1 = scanL2;
          if (--scanL2 < 0)
            scanL2 = poly.num - 1;

          leave = false;
          fyL = QRound (poly.vertices [scanL2].y);
          if (sy <= fyL)
            continue;

          float dyL = (poly.vertices [scanL1].y - poly.vertices [scanL2].y);
          if (dyL)
          {
            sxL = poly.vertices [scanL1].x;
            dxL = (poly.vertices [scanL2].x - sxL) / dyL;
            // horizontal pixel correction
            sxL += dxL * (poly.vertices [scanL1].y - (float (sy) - 0.5));
          }
        }
      } while (!leave);

      // Compute U/V and check against bounding box
#define CHECK(sx)					\
      {							\
        float cx = sx - float (width2);			\
        float z = M * cx + N * cy + O;			\
        if (ABS (z) > SMALL_EPSILON)			\
        {						\
          z = 1 / z;					\
          float u = (J1 * cx + J2 * cy + J3) * z;	\
          float v = (K1 * cx + K2 * cy + K3) * z;	\
							\
          if (u < u_min) u_min = u;			\
          if (u > u_max) u_max = u;			\
          if (v < v_min) v_min = v;			\
          if (v > v_max) v_max = v;			\
        }						\
      }

      float cy = (float (sy) - 0.5 - float (height2));
      // Due to sub-pixel correction we can arrive half of pixel left or right
      CHECK (sxL - 0.5);
      CHECK (sxR + 0.5);

      // Find the trapezoid top (or bottom in inverted Y coordinates)
      int old_sy = sy;
      if (fyL > fyR)
        sy = fyL;
      else
        sy = fyR;

      sxL += dxL * (old_sy - sy);
      sxR += dxR * (old_sy - sy);

      cy = (float (sy) - 0.5 - float (height2));
      CHECK (sxL);
      CHECK (sxR);
#undef CHECK
    }
texr_done:
    tcache->fill_texture (mipmap, tex, tex_mm, u_min, v_min, u_max, v_max);
  }
  csScan_InitDraw (mipmap, this, tex, tex_mm, txt_unl);

  // Select the right scanline drawing function.
  bool tex_keycolor = tex_mm->GetKeyColor ();
  csDrawScanline* dscan;

  // poly.alpha old way or with mixmode!!!???
  int alpha = poly.mixmode & CS_FX_MASK_ALPHA;
  if (!alpha && poly.mixmode != CS_FX_COPY)
  {
    int scan_index = Scan.bitmap2 ?
    	SCANPROC_MAP_FX_ZNONE :
	SCANPROC_TEX_FX_ZNONE;
    if (z_buf_mode == CS_ZBUF_FILL) scan_index++;
    else if (z_buf_mode == CS_ZBUF_USE) scan_index += 2;
    else if (z_buf_mode == CS_ZBUF_TEST) scan_index += 3;
    dscan = ScanProc [scan_index];
    uint mode = poly.mixmode;
    Scan.PaletteTable = tex_mm->GetPaletteToGlobal ();
    Scan.TexturePalette = tex_mm->GetColorMap ();
    Scan.BlendTable = 0;
    // array to select blend tables from
    unsigned char **BlendingTable = Scan.BlendingTable;
    switch (mode & CS_FX_MASK_MIXMODE)
    {
      case CS_FX_ADD:
        Scan.BlendTable = BlendingTable [BLENDTABLE_ADD];
        break;
      case CS_FX_MULTIPLY:
        Scan.BlendTable = BlendingTable [BLENDTABLE_MULTIPLY];
        break;
      case CS_FX_MULTIPLY2:
        Scan.BlendTable = BlendingTable [BLENDTABLE_MULTIPLY2];
        break;
      case CS_FX_ALPHA:
      {
      	// @@@ Can currently not happen!
        if (alpha < 12)
          mode = (mode & ~CS_FX_MASK_MIXMODE) | CS_FX_COPY;
        else if (alpha < 96)
          Scan.BlendTable = BlendingTable [BLENDTABLE_ALPHA25];
        else if (alpha < 160)
          Scan.BlendTable = BlendingTable [BLENDTABLE_ALPHA50];
        else if (alpha < 244)
          Scan.BlendTable = BlendingTable [BLENDTABLE_ALPHA75];
        //else
          // goto zfill_only; @@@ Not supported here!!!
        break;
      }
      case CS_FX_TRANSPARENT:
        //@@@ Not supported!
        break;
    }
  }
  else if (!alpha || !Scan.bitmap2 || !ScanProc_Alpha || !do_alpha)
  {
    int scan_index = Scan.bitmap2 ? SCANPROC_MAP_ZNONE : SCANPROC_TEX_ZNONE;
    if (z_buf_mode == CS_ZBUF_FILL) scan_index++;
    else if (z_buf_mode == CS_ZBUF_USE) scan_index += 2;
    else if (z_buf_mode == CS_ZBUF_TEST) scan_index += 3;
    if (do_alpha)
    {
      if (tex_keycolor)
        scan_index += 4;
      else if ((Scan.AlphaMap = txt_unl->get_alphamap ()))
      {
        scan_index = Scan.bitmap2
		? SCANPROC_MAP_ALPHA_ZNONE
		: SCANPROC_TEX_ALPHA_ZNONE;
        if (z_buf_mode == CS_ZBUF_FILL) scan_index++;
        else if (z_buf_mode == CS_ZBUF_USE) scan_index += 2;
        else if (z_buf_mode == CS_ZBUF_TEST) scan_index += 3;
      }
    }
    dscan = ScanProc [scan_index];
  }
  else
  {
    dscan = ScanProc_Alpha (this, alpha, tex_keycolor, (Scan.AlphaMap = txt_unl->get_alphamap ()));
  }

  if (!dscan) return;

  sxL = sxR = dxL = dxR = 0; // Avoid warnings about "uninitialized variables"
  scanL2 = scanR2 = max_i;
  sy = fyL = fyR = QRound (poly.vertices [scanL2].y);

  if (do_alpha) 
  {
    // cached texture has different coords than original tex.
    Scan.amap_uofs = mapping.GetIMinU() >> mipmap; 
    Scan.amap_vofs = mapping.GetIMinV() >> mipmap; 
  }

  for ( ; ; )
  {
    //-----
    // We have reached the next segment. Recalculate the slopes.
    //-----
    bool leave;
    do
    {
      leave = true;
      if (sy <= fyR)
      {
        // Check first if polygon has been finished
        if (scanR2 == min_i)
          goto finish;
        scanR1 = scanR2;
	if (++scanR2 >= poly.num)
	  scanR2 = 0;

        leave = false;
        fyR = QRound (poly.vertices [scanR2].y);
        if (sy <= fyR)
          continue;

        float dyR = (poly.vertices [scanR1].y - poly.vertices [scanR2].y);
        if (dyR)
        {
          sxR = poly.vertices [scanR1].x;
          dxR = (poly.vertices [scanR2].x - sxR) / dyR;
          // horizontal pixel correction
          sxR += dxR * (poly.vertices [scanR1].y - (float (sy) - 0.5));
        }
      }
      if (sy <= fyL)
      {
        scanL1 = scanL2;
	if (--scanL2 < 0)
	  scanL2 = poly.num - 1;

        leave = false;
        fyL = QRound (poly.vertices [scanL2].y);
        if (sy <= fyL)
          continue;

        float dyL = (poly.vertices [scanL1].y - poly.vertices [scanL2].y);
        if (dyL)
        {
          sxL = poly.vertices [scanL1].x;
          dxL = (poly.vertices [scanL2].x - sxL) / dyL;
          // horizontal pixel correction
          sxL += dxL * (poly.vertices [scanL1].y - (float (sy) - 0.5));
        }
      }
    } while (!leave);

    // Steps for interpolating vertically over scanlines.
    float vd_inv_z = - dxL * M + N;
    float vd_u_div_z = - dxL * J1 + J2;
    float vd_v_div_z = - dxL * K1 + K2;

    float cx = (sxL - float (width2));
    float cy = (float (sy) - 0.5 - float (height2));
    float inv_z = M * cx + N * cy + O;
    float u_div_z = J1 * cx + J2 * cy + J3;
    float v_div_z = K1 * cx + K2 * cy + K3;

    // Find the trapezoid top (or bottom in inverted Y coordinates)
    int fin_y;
    if (fyL > fyR)
      fin_y = fyL;
    else
      fin_y = fyR;

    screenY = height - sy;

    while (sy > fin_y)
    {
      if ((sy & 1) != do_interlaced)
      {
        // Compute the rounded screen coordinates of horizontal strip
        xL = QRound (sxL);
        xR = QRound (sxR);

        // Sub-pixel U & V correction
        float deltaX = (float)xL - sxL;

        d = line_table [screenY] + (xL << pixel_shift);
        z_buf = z_buffer + width * screenY + xL;

        // Select the right filter depending if we are drawing an odd or
	// even line. This is only used by scan_map_filt_zfil currently and
	// is still experimental.
        if (sy & 1) filter_bf = 3; else filter_bf = 1;

        // do not draw the rightmost pixel - it will be covered
        // by neightbour polygon's left bound
        dscan (xR - xL, d, z_buf,
		inv_z + deltaX * M,
		u_div_z + deltaX * J1,
		v_div_z + deltaX * K1);
      }

      sxL += dxL;
      sxR += dxR;
      inv_z -= vd_inv_z;
      u_div_z -= vd_u_div_z;
      v_div_z -= vd_v_div_z;
      sy--;
      screenY++;
    } /* endwhile */
  } /* endfor */

finish:
  ;
}

void csSoftwareGraphics3DCommon::DrawPolygonDebug (G3DPolygonDP& poly)
{
  (void)poly;
}

FogBuffer* csSoftwareGraphics3DCommon::find_fog_buffer (CS_ID id)
{
  FogBuffer* f = fog_buffers;
  while (f)
  {
    if (f->id == id) return f;
    f = f->next;
  }
  return 0;
}

void csSoftwareGraphics3DCommon::OpenFogObject (CS_ID id, csFog* fog)
{
  FogBuffer* fb = new FogBuffer ();
  fb->next = fog_buffers;
  fb->prev = 0;
  fb->id = id;
  fb->density = fog->density;
  fb->red = fog->red;
  fb->green = fog->green;
  fb->blue = fog->blue;
  if (fog_buffers) fog_buffers->prev = fb;
  fog_buffers = fb;
}

void csSoftwareGraphics3DCommon::CloseFogObject (CS_ID id)
{
  FogBuffer* fb = find_fog_buffer (id);
  if (!fb)
  {
    Report (CS_REPORTER_SEVERITY_BUG,
    	"ENGINE FAILURE! Try to close a non-open fog object!");
    return;
  }
  if (fb->next) fb->next->prev = fb->prev;
  if (fb->prev) fb->prev->next = fb->next;
  else fog_buffers = fb->next;
  delete fb;
}

void csSoftwareGraphics3DCommon::DrawFogPolygon (CS_ID id,
	G3DPolygonDFP& poly, int fog_type)
{
  int i;
  int max_i, min_i;
  float max_y, min_y;
  unsigned char *d;
  uint32 *z_buf;

  if (poly.num < 3)
    return;

  float M = 0, N = 0, O = 0;
  if (fog_type == CS_FOG_FRONT || fog_type == CS_FOG_BACK)
  {
    // Get the plane normal of the polygon. Using this we can calculate
    // '1/z' at every screen space point.
    float Ac, Bc, Cc, Dc, inv_Dc;
    Ac = poly.normal.A ();
    Bc = poly.normal.B ();
    Cc = poly.normal.C ();
    Dc = poly.normal.D ();

    if (ABS (Dc) < SMALL_D)
    {
      // The Dc component of the plane normal is too small. This means that
      // the plane of the polygon is almost perpendicular to the eye of the
      // viewer. In this case, nothing much can be seen of the plane anyway
      // so we just take one value for the entire polygon.
      M = 0;
      N = 0;
      // For O choose the transformed z value of one vertex.
      // That way Z buffering should at least work.
      O = 1;
    }
    else
    {
      inv_Dc = 1 / Dc;
      M = -Ac * inv_Dc * inv_aspect;
      N = -Bc * inv_Dc * inv_aspect;
      O = -Cc * inv_Dc;
    }
  }

  // Compute the min_y and max_y for this polygon in screen space coordinates.
  // We are going to use these to scan the polygon from top to bottom.
  min_i = max_i = 0;
  min_y = max_y = poly.vertices[0].y;
  // count 'real' number of vertices
  int num_vertices = 1;
  for (i = 1 ; i < poly.num ; i++)
  {
    if (poly.vertices[i].y > max_y)
    {
      max_y = poly.vertices[i].y;
      max_i = i;
    }
    else if (poly.vertices[i].y < min_y)
    {
      min_y = poly.vertices[i].y;
      min_i = i;
    }
    // theoretically we should do here sqrt(dx^2+dy^2), but
    // we can approximate it just by abs(dx)+abs(dy)
    if ((ABS (poly.vertices [i].x - poly.vertices [i - 1].x)
       + ABS (poly.vertices [i].y - poly.vertices [i - 1].y))
       	> VERTEX_NEAR_THRESHOLD)
      num_vertices++;
  }

  // if this is a 'degenerate' polygon, skip it
  if (num_vertices < 3)
    return;

  if (fog_type != CS_FOG_BACK)
  {
    FogBuffer* fb = find_fog_buffer (id);
    if (!fb)
    {
      Report (CS_REPORTER_SEVERITY_BUG,
      	"ENGINE FAILURE! Fog object not open!");
      return;
    }

    Scan.FogDensity = QRound (fb->density * 100);
    Scan.FogR = QRound (fb->red * ((1 << pfmt.RedBits) - 1))
      	<< pfmt.RedShift;
    Scan.FogG = QRound (fb->green * ((1 << pfmt.GreenBits) - 1))
      	<< pfmt.GreenShift;
    Scan.FogB = QRound (fb->blue * ((1 << pfmt.BlueBits) - 1))
      	<< pfmt.BlueShift;

    if (pfmt.PixelBytes == 4)
    {
      // trick: in 32-bit modes set FogR,G,B so that "R" uses bits 16-23,
      // "G" uses bits 8-15 and "B" uses bits 0-7. This is to accomodate
      // different pixel encodings such as RGB, BGR, RBG and so on...
      uint32 r = (R8G8B8_SHIFT_ADJUST(pfmt.RedShift) == 16) ? Scan.FogR :
          (R8G8B8_SHIFT_ADJUST(pfmt.GreenShift) == 16) ? Scan.FogG : Scan.FogB;
      uint32 g = (R8G8B8_SHIFT_ADJUST(pfmt.RedShift) == 8) ? Scan.FogR :
          (R8G8B8_SHIFT_ADJUST(pfmt.GreenShift) == 8) ? Scan.FogG : Scan.FogB;
      uint32 b = (R8G8B8_SHIFT_ADJUST(pfmt.RedShift) == 0) ? Scan.FogR :
          (R8G8B8_SHIFT_ADJUST(pfmt.GreenShift) == 0) ? Scan.FogG : Scan.FogB;
      Scan.FogR = R8G8B8_PIXEL_PREPROC(r);
      Scan.FogG = R8G8B8_PIXEL_PREPROC(g);
      Scan.FogB = R8G8B8_PIXEL_PREPROC(b);
    }
    Scan.FogPix = Scan.FogR | Scan.FogG | Scan.FogB;
  }

  SelectInterpolationStep (M);

  // Steps for interpolating horizontally accross a scanline.
  Scan.M = M;
  Scan.dM = M*Scan.InterpolStep;

  // Select the right scanline drawing function.
  csDrawScanline* dscan = 0;
  int scan_index =
  	fog_type == CS_FOG_FRONT ? SCANPROC_FOG :
	fog_type == CS_FOG_BACK ? SCANPROC_ZFIL :
	fog_type == CS_FOG_VIEW ? SCANPROC_FOG_VIEW :
	-1;

  if ((scan_index < 0) || !(dscan = ScanProc [scan_index]))
    return;   // Nothing to do.

  //@@@ Optimization note! We should have a seperate loop for CS_FOG_VIEW
  // as that is much simpler and does not require the calculations for z.
  // This would make things more efficient.

  // Scan both sides of the polygon at once.
  // We start with two pointers at the top (as seen in y-inverted
  // screen-space: bottom of display) and advance them until both
  // join together at the bottom. The way this algorithm works, this
  // should happen automatically; the left pointer is only advanced
  // when it is further away from the bottom than the right pointer
  // and vice versa.
  // Using this we effectively partition our polygon in trapezoids
  // with at most two triangles (one at the top and one at the bottom).

  int scanL1, scanL2, scanR1, scanR2; // scan vertex left/right start/final
  float sxL, sxR, dxL, dxR;           // scanline X left/right and deltas
  int sy, fyL, fyR;                   // scanline Y, final Y left, final Y right
  int xL, xR;
  int screenY;

  sxL = sxR = dxL = dxR = 0; // Avoid warnings about "uninitialized variables"
  scanL2 = scanR2 = max_i;
  sy = fyL = fyR = QRound (poly.vertices [scanL2].y);

  for ( ; ; )
  {
    //-----
    // We have reached the next segment. Recalculate the slopes.
    //-----
    bool leave;
    do
    {
      leave = true;
      if (sy <= fyR)
      {
        // Check first if polygon has been finished
        if (scanR2 == min_i)
          return;
        scanR1 = scanR2;
	if (++scanR2 >= poly.num)
	  scanR2 = 0;

        leave = false;
        fyR = QRound (poly.vertices [scanR2].y);
        if (sy <= fyR)
          continue;

        float dyR = (poly.vertices [scanR1].y - poly.vertices [scanR2].y);
        if (dyR)
        {
          sxR = poly.vertices [scanR1].x;
          dxR = (poly.vertices [scanR2].x - sxR) / dyR;
          // horizontal pixel correction
          sxR += dxR * (poly.vertices [scanR1].y - (float (sy) - 0.5));
        } /* endif */
      } /* endif */
      if (sy <= fyL)
      {
        scanL1 = scanL2;
	if (--scanL2 < 0)
	  scanL2 = poly.num - 1;

        leave = false;
        fyL = QRound (poly.vertices [scanL2].y);
        if (sy <= fyL)
          continue;

        float dyL = (poly.vertices [scanL1].y - poly.vertices [scanL2].y);
        if (dyL)
        {
          sxL = poly.vertices [scanL1].x;
          dxL = (poly.vertices [scanL2].x - sxL) / dyL;
          // horizontal pixel correction
          sxL += dxL * (poly.vertices [scanL1].y - (float (sy) - 0.5));
        } /* endif */
      } /* endif */
    } while (!leave); /* enddo */

    // Steps for interpolating vertically over scanlines.
    float vd_inv_z = - dxL * M + N;

    float cx = (sxL - float (width2));
    float cy = (float (sy) - 0.5 - float (height2));
    float inv_z = M * cx + N * cy + O;

    // Find the trapezoid top (or bottom in inverted Y coordinates)
    int fin_y;
    if (fyL > fyR)
      fin_y = fyL;
    else
      fin_y = fyR;

    screenY = height - sy;

    while (sy > fin_y)
    {
      if ((sy & 1) != do_interlaced)
      {
        // Compute the rounded screen coordinates of horizontal strip
        xL = QRound (sxL);
        xR = QRound (sxR);

        // Sub-pixel U & V correction
        float deltaX = (float)xL - sxL;

        //G2D->GetPixelAt(xL, screenY, &d);
        d = line_table[screenY] + (xL << pixel_shift);
        z_buf = z_buffer + width * screenY + xL;

        // do not draw the rightmost pixel - it will be covered
        // by neightbour polygon's left bound
        dscan (xR - xL, d, z_buf, inv_z + deltaX * M, 0, 0);
      }

      sxL += dxL;
      sxR += dxR;
      inv_z -= vd_inv_z;
      sy--;
      screenY++;
    } /* endwhile */
  } /* endfor */
}

void csSoftwareGraphics3DCommon::OpenPortal (G3DPolygonDFP* poly)
{
  (void)poly;
}

void csSoftwareGraphics3DCommon::ClosePortal ()
{
}
#endif

// Calculate round (f) of a 16:16 fixed pointer number
// and return a long integer.
inline long round16 (long f)
{
  return (f + 0x8000) >> 16;
}

// The static global variable that holds current PolygonQuick settings
static struct
{
  iMaterialHandle *mat_handle;
  int redFact, greenFact, blueFact;
  int max_r, max_g, max_b;
  int twfp, thfp;
  float tw, th;
  unsigned char *bm;
  int shf_w;
  bool keycolor;
  bool textured;
  bool tiling;
  uint mixmode;
  csDrawPIScanline *drawline;
  csDrawPIScanlineGouraud *drawline_gouraud;
} pqinfo;

#define EPS   0.0001

void csSoftwareGraphics3DCommon::RealStartPolygonFX (iMaterialHandle* handle,
  uint mode, bool use_fog)
{
  if (!dpfx_valid ||
  	use_fog != dpfx_use_fog ||
	handle != dpfx_mat_handle ||
	z_buf_mode != dpfx_z_buf_mode ||
	mode != dpfx_mixmode)
  {
    dpfx_valid = true;
    dpfx_use_fog = use_fog;
    dpfx_mat_handle = handle;
    dpfx_z_buf_mode = z_buf_mode;
    dpfx_mixmode = mode;
  }
  else return;

  if (!do_gouraud || !do_lighting)
    mode &= ~CS_FX_GOURAUD;

  pqinfo.mat_handle = handle;

  iTextureHandle *txt_handle = handle ? handle->GetTexture () : 0;
  if (txt_handle)
  {
    csSoftwareTextureHandle *tex_mm = (csSoftwareTextureHandle*)
    	txt_handle->GetPrivateObject ();
    csSoftwareTexture *txt_unl = (csSoftwareTexture *)tex_mm->get_texture (0);
    csScan_InitDrawFX (tex_mm, txt_unl);
    pqinfo.bm = txt_unl->get_bitmap ();
    pqinfo.tw = txt_unl->get_width ();
    pqinfo.th = txt_unl->get_height ();
    pqinfo.shf_w = txt_unl->get_w_shift ();
    pqinfo.twfp = QInt16 (pqinfo.tw) - 1;
    pqinfo.thfp = QInt16 (pqinfo.th) - 1;
    pqinfo.keycolor = tex_mm->GetKeyColor ();
    pqinfo.textured = do_textured;
    pqinfo.tiling = !!(mode & CS_FX_TILING);
  }
  else
    pqinfo.textured = false;

  Scan.AlphaMask = alpha_mask;

  Scan.BlendTable = 0;
  // array to select blend tables from
  unsigned char **BlendingTable = Scan.BlendingTable;
  if(is_for_procedural_textures) // proc manager uses its own blend tables
    BlendingTable = Scan.BlendingTableProc;
  pqinfo.drawline = 0;
  pqinfo.drawline_gouraud = 0;

  if (pqinfo.textured && Scan.AlphaMap)
  {
    int scan_index =
      (z_buf_mode == CS_ZBUF_USE) ? SCANPROC_PI_TEX_ALPHA_ZUSE :
      (z_buf_mode == CS_ZBUF_FILL) ? SCANPROC_PI_TEX_ALPHA_ZFIL :
      (z_buf_mode == CS_ZBUF_TEST) ? SCANPROC_PI_TEX_ALPHA_ZTEST :
      SCANPROC_PI_TEX_ALPHA_ZNONE;
    pqinfo.drawline = ScanProcPI [scan_index];
  }
  switch (mode & CS_FX_MASK_MIXMODE)
  {
    case CS_FX_ADD:
      Scan.BlendTable = BlendingTable [BLENDTABLE_ADD];
      break;
    case CS_FX_MULTIPLY:
      Scan.BlendTable = BlendingTable [BLENDTABLE_MULTIPLY];
      break;
    case CS_FX_MULTIPLY2:
      Scan.BlendTable = BlendingTable [BLENDTABLE_MULTIPLY2];
      break;
    case CS_FX_ALPHA:
    {
      int alpha = mode & CS_FX_MASK_ALPHA;
      if (alpha < 12)
        mode = (mode & ~CS_FX_MASK_MIXMODE) | CS_FX_COPY;
      else if (alpha < 96)
        Scan.BlendTable = BlendingTable [BLENDTABLE_ALPHA25];
      else if (alpha < 160)
        Scan.BlendTable = BlendingTable [BLENDTABLE_ALPHA50];
      else if (alpha < 244)
        Scan.BlendTable = BlendingTable [BLENDTABLE_ALPHA75];
      else
        goto zfill_only;
      break;
    }
    case CS_FX_TRANSPARENT:
zfill_only:
      mode &= ~CS_FX_GOURAUD;
      pqinfo.drawline = (z_buf_mode == CS_ZBUF_USE)
      	? 0
	: csScan_scan_pi_zfil;
      break;
    default:
      break;
  }

  // Select draw scanline routines
  int scan_index = pqinfo.textured
  	? SCANPROC_PI_TEX_ZNONE
	: SCANPROC_PI_FLAT_ZNONE;
  if (z_buf_mode == CS_ZBUF_FILL) scan_index++;
  else if (z_buf_mode == CS_ZBUF_USE) scan_index += 2;
  else if (z_buf_mode == CS_ZBUF_TEST) scan_index += 3;
  if (pqinfo.textured && pqinfo.keycolor)
    scan_index += 4;
  if ((mode & CS_FX_MASK_MIXMODE) != CS_FX_COPY)
    scan_index += 20;
  if (pqinfo.textured && (mode & CS_FX_TILING))
    scan_index += 8;
  if (!pqinfo.drawline)
    pqinfo.drawline = ScanProcPI [scan_index];
  if (mode & CS_FX_GOURAUD)
    pqinfo.drawline_gouraud = ScanProcPIG [scan_index];

  pqinfo.mixmode = mode;
  // We use #.16 fixed-point format for R,G,B factors
  // where # is the number of bits per component (with the exception of
  // 32bpp modes/textured where we use (#-2).16 format).
  int shift_amount =
    ((pfmt.PixelBytes == 4) && (Scan.BlendTable || pqinfo.textured)) ? 6 : 8;
  pqinfo.redFact = (((pfmt.RedMask>>pfmt.RedShift)+1) << shift_amount)-1;
  pqinfo.greenFact = (((pfmt.GreenMask>>pfmt.GreenShift)+1) << shift_amount)-1;
  pqinfo.blueFact  = (((pfmt.BlueMask>>pfmt.BlueShift)+1)   << shift_amount)-1;

  pqinfo.max_r = (1 << (pfmt.RedBits   + shift_amount + 8)) - 1;
  pqinfo.max_g = (1 << (pfmt.GreenBits + shift_amount + 8)) - 1;
  pqinfo.max_b = (1 << (pfmt.BlueBits  + shift_amount + 8)) - 1;
}

void csSoftwareGraphics3DCommon::DrawPolygonFX (G3DPolygonDPFX& poly)
{
  RealStartPolygonFX (poly.mat_handle, poly.mixmode, poly.use_fog);

  if (!pqinfo.drawline && !pqinfo.drawline_gouraud)
    return;

  // Determine the R/G/B of the polygon's flat color
  if (pqinfo.textured)
    Scan.FlatRGB.Set (255, 255, 255);
  else
  {
    if (pqinfo.mat_handle)
      pqinfo.mat_handle->GetFlatColor (Scan.FlatRGB);
    else
      Scan.FlatRGB.Set (
      	poly.flat_color_r, poly.flat_color_g, poly.flat_color_b);
  }

  // Get the same value as a pixel-format-encoded value
  Scan.FlatColor = texman->encode_rgb (Scan.FlatRGB.red, Scan.FlatRGB.green,
    Scan.FlatRGB.blue);

  //-----
  // Get the values from the polygon for more convenient local access.
  // Also look for the top-most and bottom-most vertices.
  //-----
  float uu[64], vv[64], iz[64];
  float rr[64], gg[64], bb[64];
  int top, bot;
  float top_y = -99999;
  float bot_y = 99999;
  top = bot = 0;                        // avoid GCC complains
  int i;
  for (i = 0 ; i < poly.num ; i++)
  {
    uu[i] = pqinfo.tw * poly.texels [i].x;
    vv[i] = pqinfo.th * poly.texels [i].y;
    iz[i] = poly.z [i];
    if (poly.colors [i].red > 2.0) poly.colors [i].red = 2.0;
    if (poly.colors [i].red < 0.0) poly.colors [i].red = 0.0;
    rr[i] = poly.colors [i].red * pqinfo.redFact   * Scan.FlatRGB.red;
    if (poly.colors [i].green > 2.0) poly.colors [i].green = 2.0;
    if (poly.colors [i].green < 0.0) poly.colors [i].green = 0.0;
    gg[i] = poly.colors [i].green * pqinfo.greenFact * Scan.FlatRGB.green;
    if (poly.colors [i].blue > 2.0) poly.colors [i].blue = 2.0;
    if (poly.colors [i].blue < 0.0) poly.colors [i].blue = 0.0;
    bb[i] = poly.colors [i].blue * pqinfo.blueFact  * Scan.FlatRGB.blue;
    if (poly.vertices [i].y > top_y)
      top_y = poly.vertices [top = i].y;
    if (poly.vertices [i].y < bot_y)
      bot_y = poly.vertices [bot = i].y;
  }

  // If the polygon exceeds the screen, it is an engine failure
  if (((bot_y + EPSILON) < 0) ||
      ((top_y - EPSILON) > height))
    return;

  //-----
  // Scan from top to bottom.
  // The following structure contains all the data for one side
  // of the scanline conversion. 'L' is responsible for the left
  // side, 'R' for the right side respectively.
  //-----
  struct
  {
    // Start and final vertex number
    char sv, fv;
    // The X coordinates and its per-scanline delta; also the final Y coordinate
    int x, dxdy, fy;
    // The `U/V/Z' texture coordinates and their per-scanline delta
    int u, dudy, v, dvdy, z, dzdy;
    // The `R/G/B' colors and their per-scanline delta
    int r, drdy, g, dgdy, b, dbdy;
  } L,R;

// Start of code to stop MSVC bitching about uninitialized variables
  L.sv = R.sv = top;
  L.fv = R.fv = top;
  int sy = L.fy = R.fy = QRound (poly.vertices [top].y);

  L.x = R.x = 0;
  L.dxdy = R.dxdy = 0;

  L.u = R.u = 0;
  L.dudy = R.dudy = 0;
  L.v = R.v = 0;
  L.dvdy = R.dvdy = 0;
  L.z = R.z = 0;
  L.dzdy = R.dzdy = 0;

  L.r = R.r = 0;
  L.drdy = R.drdy = 0;
  L.g = R.g = 0;
  L.dgdy = R.dgdy = 0;
  L.b = R.b = 0;
  L.dbdy = R.dbdy = 0;
// End of MSVC specific code

  // Decide whenever we should use Gouraud or flat (faster) routines
  bool do_gouraud = (pqinfo.drawline_gouraud != 0)
    && (pqinfo.mixmode & CS_FX_GOURAUD);

  //-----
  // Main scanline loop.
  //-----
  for ( ; ; )
  {
    //-----
    // We have reached the next segment. Recalculate the slopes.
    //-----
    bool leave;
    do
    {
      leave = true;
      if (sy <= R.fy)
      {
        // Check first if polygon has been finished
        if (R.fv == bot)
          return;
        R.sv = R.fv;
	if (++R.fv >= poly.num)
	  R.fv = 0;

        leave = false;
	R.fy = QRound (poly.vertices [R.fv].y);
	if (sy <= R.fy)
	  continue;

        float dyR = poly.vertices [R.sv].y - poly.vertices [R.fv].y;
        if (dyR)
        {
          float inv_dyR = 1 / dyR;
          R.x = QInt16 (poly.vertices [R.sv].x);
          R.dxdy = QInt16 (
            (poly.vertices [R.fv].x - poly.vertices [R.sv].x) * inv_dyR);
          R.dzdy = QInt24 ((iz [R.fv] - iz [R.sv]) * inv_dyR);
          if (pqinfo.textured)
          {
            R.dudy = QInt16 ((uu [R.fv] - uu [R.sv]) * inv_dyR);
            R.dvdy = QInt16 ((vv [R.fv] - vv [R.sv]) * inv_dyR);
          }
          if (pqinfo.mixmode & CS_FX_GOURAUD)
          {
            R.drdy = QRound ((rr [R.fv] - rr [R.sv]) * inv_dyR);
            R.dgdy = QRound ((gg [R.fv] - gg [R.sv]) * inv_dyR);
            R.dbdy = QRound ((bb [R.fv] - bb [R.sv]) * inv_dyR);
          }

          // horizontal pixel correction
          float deltaX = (R.dxdy * 1/65536.) *
            (poly.vertices [R.sv].y - (float (sy) - 0.5));
          R.x += QInt16 (deltaX);

          // apply sub-pixel accuracy factor
          float Factor;
          if (poly.vertices [R.fv].x != poly.vertices [R.sv].x)
            Factor = deltaX / (poly.vertices [R.fv].x - poly.vertices [R.sv].x);
          else
            Factor = 0;
          if (pqinfo.textured)
          {
            R.u = QInt16 (uu [R.sv] + (uu [R.fv] - uu [R.sv]) * Factor);
            R.v = QInt16 (vv [R.sv] + (vv [R.fv] - vv [R.sv]) * Factor);
          }
          R.z = QInt24 (iz [R.sv] + (iz [R.fv] - iz [R.sv]) * Factor);
          if (pqinfo.mixmode & CS_FX_GOURAUD)
          {
            R.r = QRound (rr [R.sv] + (rr [R.fv] - rr [R.sv]) * Factor);
            R.g = QRound (gg [R.sv] + (gg [R.fv] - gg [R.sv]) * Factor);
            R.b = QRound (bb [R.sv] + (bb [R.fv] - bb [R.sv]) * Factor);
          }
        } /* endif */
      } /* endif */
      if (sy <= L.fy)
      {
        if (L.fv == bot)
          return;
        L.sv = L.fv;
	if (--L.fv < 0)
	  L.fv = poly.num - 1;

        leave = false;
	L.fy = QRound (poly.vertices [L.fv].y);
	if (sy <= L.fy)
	  continue;

        float dyL = poly.vertices [L.sv].y - poly.vertices [L.fv].y;
        if (dyL)
        {
          float inv_dyL = 1 / dyL;
          L.x = QInt16 (poly.vertices [L.sv].x);
          L.dxdy = QInt16 (
            (poly.vertices [L.fv].x - poly.vertices [L.sv].x) * inv_dyL);
          L.dzdy = QInt24 ((iz [L.fv] - iz [L.sv]) * inv_dyL);
          if (pqinfo.textured)
          {
            L.dudy = QInt16 ((uu [L.fv] - uu [L.sv]) * inv_dyL);
            L.dvdy = QInt16 ((vv [L.fv] - vv [L.sv]) * inv_dyL);
          }
          if (pqinfo.mixmode & CS_FX_GOURAUD)
          {
            L.drdy = QRound ((rr [L.fv] - rr [L.sv]) * inv_dyL);
            L.dgdy = QRound ((gg [L.fv] - gg [L.sv]) * inv_dyL);
            L.dbdy = QRound ((bb [L.fv] - bb [L.sv]) * inv_dyL);
          }

          // horizontal pixel correction
          float deltaX = (L.dxdy * 1/65536.) *
            (poly.vertices [L.sv].y - (float (sy) - 0.5));
          L.x += QInt16 (deltaX);

          // apply sub-pixel accuracy factor
          float Factor;
          if (poly.vertices [L.fv].x != poly.vertices [L.sv].x)
            Factor = deltaX / (poly.vertices [L.fv].x - poly.vertices [L.sv].x);
          else
            Factor = 0;
          if (pqinfo.textured)
          {
            L.u = QInt16 (uu [L.sv] + (uu [L.fv] - uu [L.sv]) * Factor);
            L.v = QInt16 (vv [L.sv] + (vv [L.fv] - vv [L.sv]) * Factor);
          }
          L.z = QInt24 (iz [L.sv] + (iz [L.fv] - iz [L.sv]) * Factor);
          if (pqinfo.mixmode & CS_FX_GOURAUD)
          {
            L.r = QRound (rr [L.sv] + (rr [L.fv] - rr [L.sv]) * Factor);
            L.g = QRound (gg [L.sv] + (gg [L.fv] - gg [L.sv]) * Factor);
            L.b = QRound (bb [L.sv] + (bb [L.fv] - bb [L.sv]) * Factor);
          }
        } /* endif */
      } /* endif */
    } while (!leave); /* enddo */

    //-----
    // Now draw a trapezoid.
    //-----
    int fin_y;
    if (L.fy > R.fy)
      fin_y = L.fy;
    else
      fin_y = R.fy;

    int screenY = height - sy;
    while (sy > fin_y)
    {
      if ((sy & 1) != do_interlaced)
      {
        //-----
        // Draw one scanline.
        //-----
        int xl = round16 (L.x);
        int xr = round16 (R.x);

        if (xr > xl)
        {
          int l = xr - xl;
          float inv_l = 1. / l;

          int dzz = QRound ((R.z - L.z) * inv_l);
          int uu = 0, duu = 0, vv = 0, dvv = 0;
          if (pqinfo.textured)
          {
            int span_u = R.u - L.u;
            int span_v = R.v - L.v;
            uu = L.u; duu = QInt (span_u * inv_l);
            vv = L.v; dvv = QInt (span_v * inv_l);

	    if (!pqinfo.tiling)
	    {
              // Check for texture overflows
              if (uu < 0) uu = 0; if (uu > pqinfo.twfp) uu = pqinfo.twfp;
              if (vv < 0) vv = 0; if (vv > pqinfo.thfp) vv = pqinfo.thfp;

              int tmpu = uu + span_u;
              if (tmpu < 0 || tmpu > pqinfo.twfp)
              {
                if (tmpu < 0) tmpu = 0; if (tmpu > pqinfo.twfp)
		  tmpu = pqinfo.twfp;
                duu = QInt ((tmpu - uu) * inv_l);
              }
              int tmpv = vv + span_v;
              if (tmpv < 0 || tmpv > pqinfo.thfp)
              {
                if (tmpv < 0) tmpv = 0; if (tmpv > pqinfo.thfp)
		  tmpv = pqinfo.thfp;
                dvv = QInt ((tmpv - vv) * inv_l);
              }
	    }
          }

          // R,G,B brightness can underflow due to subpixel correction
          // Underflow will cause visual artifacts while small overflows
          // will be neutralized by our "clamp to 1.0" circuit.
          int rr = 0, drr = 0, gg = 0, dgg = 0, bb = 0, dbb = 0;
          bool clamp = false;
          if (pqinfo.mixmode & CS_FX_GOURAUD)
          {
            int span_r = R.r - L.r;
            int span_g = R.g - L.g;
            int span_b = R.b - L.b;
            rr = L.r, drr = QInt (span_r * inv_l);
            gg = L.g, dgg = QInt (span_g * inv_l);
            bb = L.b, dbb = QInt (span_b * inv_l);

            if (rr < 0) rr = 0;
            if (gg < 0) gg = 0;
            if (bb < 0) bb = 0;

            int tmp = rr + span_r;
            if (tmp < 0) drr = - QInt (rr * inv_l);
            clamp |= (rr > pqinfo.max_r) || (tmp > pqinfo.max_r);
            tmp = gg + span_g;
            if (tmp < 0) dgg = - QInt (gg * inv_l);
            clamp |= (gg > pqinfo.max_g) || (tmp > pqinfo.max_g);
            tmp = bb + span_b;
            if (tmp < 0) dbb = - QInt (bb * inv_l);
            clamp |= (bb > pqinfo.max_b) || (tmp > pqinfo.max_b);
          }

          uint32 *zbuff = z_buffer + width * screenY + xl;
          unsigned char *dest = line_table [screenY] + (xl << pixel_shift);

          if (do_gouraud)
            pqinfo.drawline_gouraud (dest, l, zbuff, uu, duu, vv, dvv,
              L.z, dzz, pqinfo.bm, pqinfo.shf_w, rr, gg, bb, drr, dgg,
	      dbb, clamp);
          else
            pqinfo.drawline (dest, l, zbuff, uu, duu, vv, dvv,
              L.z, dzz, pqinfo.bm, pqinfo.shf_w);
        }
      }

      L.x += L.dxdy; R.x += R.dxdy;
      L.z += L.dzdy; R.z += R.dzdy;
      if (pqinfo.textured)
        L.u += L.dudy, L.v += L.dvdy,
        R.u += R.dudy, R.v += R.dvdy;
      if (pqinfo.mixmode & CS_FX_GOURAUD)
        L.r += L.drdy, L.g += L.dgdy, L.b += L.dbdy,
        R.r += R.drdy, R.g += R.dgdy, R.b += R.dbdy;

      sy--;
      screenY++;
    }
  }
}
#if 0
void csSoftwareGraphics3DCommon::DrawTriangleMesh (G3DTriangleMesh& mesh)
{
  iClipper2D* cl;
  if (mesh.clip_portal >= CS_CLIPPER_NONE)
    cl = clipper;
  else
    cl = 0;
  DefaultDrawTriangleMesh (mesh, this, o2c, cl,
	false /*lazyclip*/, aspect, width2, height2);
}

void csSoftwareGraphics3DCommon::DrawPolygonMesh (G3DPolygonMesh& mesh)
{
  iClipper2D* cl;
  if (mesh.clip_portal >= CS_CLIPPER_NONE)
    cl = clipper;
  else
    cl = 0;
  DefaultDrawPolygonMesh (mesh, this, o2c, cl,
	false /*lazyclip*/, aspect, width2, height2);
}

bool csSoftwareGraphics3DCommon::SetRenderState (G3D_RENDERSTATEOPTION op,
  long value)
{
  switch (op)
  {
    case G3DRENDERSTATE_ZBUFFERMODE:
      z_buf_mode = csZBufMode (value);
      break;
    case G3DRENDERSTATE_DITHERENABLE:
      texman->dither_textures = value;
      break;
    case G3DRENDERSTATE_BILINEARMAPPINGENABLE:
      bilinear_filter = value ? 1 : 0;
      ScanSetup ();
      break;
    case G3DRENDERSTATE_TRILINEARMAPPINGENABLE:
      bilinear_filter = value ? 2 : 0;
      ScanSetup ();
      break;
    case G3DRENDERSTATE_TRANSPARENCYENABLE:
      do_alpha = value;
      ScanSetup ();
      break;
    case G3DRENDERSTATE_MIPMAPENABLE:
      rstate_mipmap = value;
      break;
    case G3DRENDERSTATE_TEXTUREMAPPINGENABLE:
      do_textured = value;
      ScanSetup ();
      break;
    case G3DRENDERSTATE_MMXENABLE:
#ifdef DO_MMX
      do_mmx = value;
      ScanSetup ();
      break;
#else
      return false;
#endif
    case G3DRENDERSTATE_INTERLACINGENABLE:
      CS_ASSERT (G2D);
      if (!value)
      {
	G2D->DoubleBuffer (true);
	do_interlaced = -1;
      }
      else
      {
        if (!G2D->DoubleBuffer (false))
          return false;
        do_interlaced = 0;
      }
      break;
    case G3DRENDERSTATE_INTERPOLATIONSTEP:
      Scan.InterpolMode = value;
      break;
    case G3DRENDERSTATE_LIGHTINGENABLE:
      do_lighting = value;
      break;
    case G3DRENDERSTATE_MAXPOLYGONSTODRAW:
      dbg_max_polygons_to_draw = value;
      if (dbg_max_polygons_to_draw < 0) dbg_max_polygons_to_draw = 0;
      break;
    case G3DRENDERSTATE_GOURAUDENABLE:
      do_gouraud = value;
      break;
    default:
      return false;
  }

  return true;
}

long csSoftwareGraphics3DCommon::GetRenderState(G3D_RENDERSTATEOPTION op)
{
  switch (op)
  {
    case G3DRENDERSTATE_ZBUFFERMODE:
      return z_buf_mode;
    case G3DRENDERSTATE_DITHERENABLE:
      return texman->dither_textures;
    case G3DRENDERSTATE_BILINEARMAPPINGENABLE:
      return bilinear_filter == 1 ? 1 : 0;
    case G3DRENDERSTATE_TRILINEARMAPPINGENABLE:
      return bilinear_filter == 2 ? 1 : 0;
    case G3DRENDERSTATE_TRANSPARENCYENABLE:
      return do_alpha;
    case G3DRENDERSTATE_MIPMAPENABLE:
      return rstate_mipmap;
    case G3DRENDERSTATE_TEXTUREMAPPINGENABLE:
      return do_textured;
    case G3DRENDERSTATE_MMXENABLE:
#ifdef DO_MMX
      return do_mmx;
#else
      return 0;
#endif
    case G3DRENDERSTATE_INTERLACINGENABLE:
      return do_interlaced == -1 ? false : true;
    case G3DRENDERSTATE_INTERPOLATIONSTEP:
      return Scan.InterpolMode;
    case G3DRENDERSTATE_LIGHTINGENABLE:
      return do_lighting;
    case G3DRENDERSTATE_MAXPOLYGONSTODRAW:
      return dbg_max_polygons_to_draw;
    case G3DRENDERSTATE_GOURAUDENABLE:
      return do_gouraud;
    default:
      return 0;
  }
}
#endif

void csSoftwareGraphics3DCommon::ClearCache()
{
  if (tcache) tcache->Clear ();
}

void csSoftwareGraphics3DCommon::RemoveFromCache (iPolygonTexture* poly_texture)
{
  if (tcache)
  {
    tcache->uncache_texture (0, poly_texture);
    tcache->uncache_texture (1, poly_texture);
    tcache->uncache_texture (2, poly_texture);
    tcache->uncache_texture (3, poly_texture);
  }
}

void csSoftwareGraphics3DCommon::DumpCache()
{
  if (tcache) tcache->dump (this);
}

void csSoftwareGraphics3DCommon::DrawLine (const csVector3& v1,
	const csVector3& v2, float fov, int color)
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
  int py1 = height - QInt (y1 * iz1 + (height/2));
  float iz2 = fov/z2;
  int px2 = QInt (x2 * iz2 + (width/2));
  int py2 = height - QInt (y2 * iz2 + (height/2));

  G2D->DrawLine (px1, py1, px2, py2, color);
}

float csSoftwareGraphics3DCommon::GetZBuffValue (int x, int y)
{
  unsigned long zbf = z_buffer [x + y * width];
  if (!zbf) return 1000000000.;
  return 16777216.0 / float (zbf);
}

/*bool csSoftwareGraphics3DCommon::IsLightmapOK (iPolygonTexture* poly_texture)
{
  const csLightMapMapping& mapping = poly_texture->GetMapping ();
  return ((mapping.GetWidth () * mapping.GetHeight ()) < MAX_LIGHTMAP_SIZE);
}*/

void csSoftwareGraphics3DCommon::SetRenderTarget (iTextureHandle* handle,
	bool persistent)
{
  render_target = handle;
  csSoftwareTextureHandle* tex_mm = (csSoftwareTextureHandle *)
	    render_target->GetPrivateObject ();
  tex_mm->Setup332Palette ();

  rt_onscreen = !persistent;
  rt_cliprectset = false;
}

/*void csSoftwareGraphics3DCommon::DrawMesh (csRenderMesh* mesh)
{
  iRenderBufferSource* source = mesh->buffersource;
  iRenderBuffer* indexbuf = source->GetRenderBuffer (
    strings->Request ("indices"));
  if (!indexbuf)
    return;

  uint32 *indices = (uint32*)indexbuf->Lock (CS_BUF_LOCK_NORMAL);

  do_lighting = false;

  void* locks[16];
  int i=0;
  for (i=0; i<16; ++i)
  {
    if (activebuffers[i] != 0)
      locks[i] = activebuffers[i]->Lock (CS_BUF_LOCK_NORMAL);
  }

  G3DTriangleMesh trimesh;
  trimesh.
  for (i=mesh->indexstart; i<mesh->indexend; i+=3)
  {
    G3DPolygonDPF poly;
    poly.do_fullbright = true;
    poly.mat_handle = mesh->material->GetMaterialHandle();
    poly.mixmode = CS_FX_COPY;
    poly.use_fog = false;
    csVector3 v[3];
    v[0] = csVector3 (((float*)locks[CS_VATTRIB_POSITION])[indices[i]*3],
                  ((float*)locks[CS_VATTRIB_POSITION])[indices[i]*3+1],
                  ((float*)locks[CS_VATTRIB_POSITION])[indices[i]*3+2]);
    v[1] = csVector3 (((float*)locks[CS_VATTRIB_POSITION])[indices[i+1]*3],
                  ((float*)locks[CS_VATTRIB_POSITION])[indices[i+1]*3+1],
                  ((float*)locks[CS_VATTRIB_POSITION])[indices[i+1]*3+2]);
    v[2] = csVector3 (((float*)locks[CS_VATTRIB_POSITION])[indices[i+2]*3],
                  ((float*)locks[CS_VATTRIB_POSITION])[indices[i+2]*3+1],
                  ((float*)locks[CS_VATTRIB_POSITION])[indices[i+2]*3+2]);

    for (int j=0; j<3; j++)
    {
      v[j] *= *mesh->transform;
      float iz = aspect / v[j].z;
      poly.vertices[j].x = v[j].x * iz + width2;
      poly.vertices[j].y = v[j].y * iz + height2;
    }
    poly.num = 3;
    poly.z_value = v[0].z;
    poly.normal = csPlane3(v[0], v[1], v[2]);
    poly.poly_texture = 0;
    z_buf_mode = CS_ZBUF_USE;
    //DrawPolygonFlat (poly);
    DrawTriangle (this, clipper, mesh, poly, 
      poly.vertices[0], poly.vertices[1], poly.vertices[2],)
  }
  
  indexbuf->Release ();
  for (i=0; i<16; ++i)
  {
    if (activebuffers[i] != 0)
      activebuffers[i]->Release ();
  }
}*/

/// Static vertex array.
typedef csDirtyAccessArray<csVector3> dtmesh_tr_verts;
CS_IMPLEMENT_STATIC_VAR (Get_tr_verts, dtmesh_tr_verts, ())
/// Static z array.
typedef csDirtyAccessArray<float> dtmesh_z_verts;
CS_IMPLEMENT_STATIC_VAR (Get_z_verts, dtmesh_z_verts, ())
/// Static uv array.
typedef csDirtyAccessArray<csVector2> dtmesh_uv_verts;
CS_IMPLEMENT_STATIC_VAR (Get_uv_verts, dtmesh_uv_verts, ())
/// The perspective corrected vertices.
typedef csDirtyAccessArray<csVector2> dtmesh_persp;
CS_IMPLEMENT_STATIC_VAR (Get_persp, dtmesh_persp, ())
/// Array with colors.
typedef csDirtyAccessArray<csColor> dtmesh_color_verts;
CS_IMPLEMENT_STATIC_VAR (Get_color_verts, dtmesh_color_verts, ())


static dtmesh_tr_verts *tr_verts = 0;
static dtmesh_z_verts *z_verts = 0;
static dtmesh_uv_verts *uv_verts = 0;
static dtmesh_persp *persp = 0;
static dtmesh_color_verts *color_verts = 0;

#define INTERPOLATE1_S(var) \
  g3dpoly->var [i] = tritexcoords_##var [vt]+ \
  t * (tritexcoords_##var [vt2] - tritexcoords_##var [vt]);

#define INTERPOLATE1(var,component) \
  g3dpoly->var [i].component = tritexcoords_##var [vt].component + \
  t * (tritexcoords_##var [vt2].component - tritexcoords_##var [vt].component);

#define INTERPOLATE1_FOG(component) \
  g3dpoly->fog_info [i].component = trifoginfo [vt].component + \
  t * (trifoginfo [vt2].component - trifoginfo [vt].component);

#define INTERPOLATE_S(var) \
{ \
  float v1 = tritexcoords_##var [edge1 [0]] + \
  t1 * (tritexcoords_##var [edge1 [1]] - \
  tritexcoords_##var [edge1 [0]]); \
  float v2 = tritexcoords_##var [edge2 [0]] + \
  t2 * (tritexcoords_##var [edge2 [1]] - \
  tritexcoords_##var [edge2 [0]]); \
  g3dpoly->var [i] = v1 + t * (v2 - v1); \
}
#define INTERPOLATE(var,component) \
{ \
  float v1 = tritexcoords_##var [edge1 [0]].component + \
  t1 * (tritexcoords_##var [edge1 [1]].component - \
  tritexcoords_##var [edge1 [0]].component); \
  float v2 = tritexcoords_##var [edge2 [0]].component + \
  t2 * (tritexcoords_##var [edge2 [1]].component - \
  tritexcoords_##var [edge2 [0]].component); \
  g3dpoly->var [i].component = v1 + t * (v2 - v1); \
}
#define INTERPOLATE_FOG(component) \
{ \
  float v1 = trifoginfo [edge1 [0]].component + \
  t1 * (trifoginfo [edge1 [1]].component - \
  trifoginfo [edge1 [0]].component); \
  float v2 = trifoginfo [edge2 [0]].component + \
  t2 * (trifoginfo [edge2 [1]].component - \
  trifoginfo [edge2 [0]].component); \
  g3dpoly->fog_info [i].component = v1 + t * (v2 - v1); \
}

static void G3DPreparePolygonFX (G3DPolygonDPFX* g3dpoly,
                                 csVector2* clipped_verts, int num_vertices,
                                 csVertexStatus* clipped_vtstats, csVector2* orig_triangle,
                                 bool use_fog, bool gouraud)
{
  // first we copy the first three texture coordinates to a local buffer
  // to avoid that they are overwritten when interpolating.
  csVector2 tritexcoords_vertices[3];
  csVector2 tritexcoords_texels[3];
  csColor tritexcoords_colors[3];
  float tritexcoords_z[3];
  G3DFogInfo trifoginfo [3];
  int i;
  memcpy (tritexcoords_vertices, g3dpoly->vertices, 3 * sizeof (csVector2));
  memcpy (tritexcoords_texels, g3dpoly->texels, 3 * sizeof (csVector2));
  memcpy (tritexcoords_colors, g3dpoly->colors, 3 * sizeof (csColor));
  memcpy (tritexcoords_z, g3dpoly->z, 3 * sizeof (float));

  int vt, vt2;
  float t;
  for (i = 0 ; i < num_vertices ; i++)
  {
    g3dpoly->vertices[i] = clipped_verts[i];
    switch (clipped_vtstats[i].Type)
    {
    case CS_VERTEX_ORIGINAL:
      vt = clipped_vtstats[i].Vertex;
      g3dpoly->z[i] = tritexcoords_z[vt];
      g3dpoly->texels[i] = tritexcoords_texels[vt];
      if (gouraud)
        g3dpoly->colors[i] = tritexcoords_colors[vt];
      if (use_fog) g3dpoly->fog_info[i] = trifoginfo[vt];
      break;
    case CS_VERTEX_ONEDGE:
      vt = clipped_vtstats[i].Vertex;
      vt2 = vt + 1; if (vt2 >= 3) vt2 = 0;
      t = clipped_vtstats[i].Pos;
      INTERPOLATE1_S (z);
      INTERPOLATE1 (texels,x);
      INTERPOLATE1 (texels,y);
      if (gouraud)
      {
        INTERPOLATE1 (colors,red);
        INTERPOLATE1 (colors,green);
        INTERPOLATE1 (colors,blue);
      }
      if (use_fog)
      {
        INTERPOLATE1_FOG (r);
        INTERPOLATE1_FOG (g);
        INTERPOLATE1_FOG (b);
        INTERPOLATE1_FOG (intensity);
      }
      break;
    case CS_VERTEX_INSIDE:
      float x = clipped_verts [i].x;
      float y = clipped_verts [i].y;
      int edge1 [2], edge2 [2];
      if ((y >= orig_triangle [0].y && y <= orig_triangle [1].y) ||
        (y <= orig_triangle [0].y && y >= orig_triangle [1].y))
      {
        edge1[0] = 0;
        edge1[1] = 1;
        if ((y >= orig_triangle [1].y && y <= orig_triangle [2].y) ||
          (y <= orig_triangle [1].y && y >= orig_triangle [2].y))
        {
          edge2[0] = 1;
          edge2[1] = 2;
        }
        else
        {
          edge2[0] = 0;
          edge2[1] = 2;
        }
      }
      else
      {
        edge1[0] = 1;
        edge1[1] = 2;
        edge2[0] = 0;
        edge2[1] = 2;
      }
      csVector2& A = orig_triangle [edge1 [0]];
      csVector2& B = orig_triangle [edge1 [1]];
      csVector2& C = orig_triangle [edge2 [0]];
      csVector2& D = orig_triangle [edge2 [1]];
      float t1 = (y - A.y) / (B.y - A.y);
      float t2 = (y - C.y) / (D.y - C.y);
      float x1 = A.x + t1 * (B.x - A.x);
      float x2 = C.x + t2 * (D.x - C.x);
      t = (x - x1) / (x2 - x1);
      INTERPOLATE_S (z);
      INTERPOLATE (texels,x);
      INTERPOLATE (texels,y);
      if (gouraud)
      {
        INTERPOLATE (colors,red);
        INTERPOLATE (colors,green);
        INTERPOLATE (colors,blue);
      }
      if (use_fog)
      {
        INTERPOLATE_FOG (r);
        INTERPOLATE_FOG (g);
        INTERPOLATE_FOG (b);
        INTERPOLATE_FOG (intensity);
      }
      break;
    }
  }
}

static void DrawTriangle (
                          csSoftwareGraphics3DCommon* g3d,
                          iClipper2D* clipper,
                          csRenderMesh* mesh,
                          G3DPolygonDPFX& poly,
                          const csVector2& pa, const csVector2& pb, const csVector2& pc,
                          int* trivert,
                          float* z_verts,
                          csVector2* uv_verts,
                          csColor* colors,
                          G3DFogInfo* fog)
{
  // The triangle in question
  csVector2 triangle[3];
  csVector2 clipped_triangle[MAX_OUTPUT_VERTICES];	//@@@BAD HARCODED!
  csVertexStatus clipped_vtstats[MAX_OUTPUT_VERTICES];
  uint8 clip_result;

  if (!tr_verts)
  {
    tr_verts = Get_tr_verts ();
    persp= Get_persp();
    color_verts = Get_color_verts ();
  }

  //-----
  // Do backface culling. Note that this depends on the
  // mirroring of the current view.
  //-----
  float area = csMath2::Area2 (pa, pb, pc);
  int j, idx, dir;
  if (!area) return;
  if (mesh->do_mirror)
  {
    if (area <= -SMALL_EPSILON) return;
    triangle[2] = pa; triangle[1] = pb; triangle[0] = pc;
    // Setup loop variables for later.
    idx = 2; dir = -1;
  }
  else
  {
    if (area >= SMALL_EPSILON) return;
    triangle[0] = pa; triangle[1] = pb; triangle[2] = pc;
    // Setup loop variables for later.
    idx = 0; dir = 1;
  }

  // Clip triangle. Note that the clipper doesn't care about the
  // orientation of the triangle vertices. It works just as well in
  // mirrored mode.
  int rescount = 0;
  if (clipper)
  {
    clip_result = clipper->Clip (triangle, 3, clipped_triangle, rescount,
      clipped_vtstats);
    if (clip_result == CS_CLIP_OUTSIDE) return;
    poly.num = rescount;
  }
  else
  {
    clip_result = CS_CLIP_INSIDE;
    poly.num = 3;
  }

  // If mirroring we store the vertices in the other direction.
  for (j = 0; j < 3; j++)
  {
    poly.z [idx] = z_verts[trivert [j]];
    poly.texels [idx] = uv_verts[trivert [j]];
    if (colors)
    {
      poly.colors [idx] = colors[trivert[j]];
    }
    if (poly.use_fog) poly.fog_info [idx] = fog[trivert[j]];
    idx += dir;
  }
  if (clip_result != CS_CLIP_INSIDE)
    G3DPreparePolygonFX (&poly, clipped_triangle, rescount, clipped_vtstats,
    (csVector2 *)triangle, poly.use_fog, colors != 0);
  else
  {
    poly.vertices [0].x = triangle [0].x;
    poly.vertices [0].y = triangle [0].y;
    poly.vertices [1].x = triangle [1].x;
    poly.vertices [1].y = triangle [1].y;
    poly.vertices [2].x = triangle [2].x;
    poly.vertices [2].y = triangle [2].y;
  }
  g3d->DrawPolygonFX (poly);
}


void csSoftwareGraphics3DCommon::DrawMesh (csRenderMesh* mesh)
{
  int i;

  if (!z_verts)
  {
    tr_verts = Get_tr_verts ();
    z_verts = Get_z_verts ();
    uv_verts = Get_uv_verts ();
    persp= Get_persp();
    color_verts = Get_color_verts ();
  }

  iRenderBufferSource* source = mesh->buffersource;
  iRenderBuffer* indexbuf = source->GetRenderBuffer (
    strings->Request ("indices"));
  if (!indexbuf)
    return;

  uint32 *indices = (uint32*)indexbuf->Lock (CS_BUF_LOCK_NORMAL);

  //do_lighting = false;
  bool lazyclip = false;

  SetObjectToCamera (mesh->transform);
  z_buf_mode = CS_ZBUF_USE;

  void* locks[16];
  for (i=0; i<16; ++i)
  {
    if (activebuffers[i] != 0)
      locks[i] = activebuffers[i]->Lock (CS_BUF_LOCK_NORMAL);
    else
      locks[i] = 0;
  }

/*#if CS_DEBUG
  // Check if the vertex buffers are locked.
  CS_ASSERT (mesh.buffers[0]->IsLocked ());
  if (mesh.num_vertices_pool > 1)
  {
    CS_ASSERT (mesh.buffers[1]->IsLocked ());
  }
#endif*/

  // @@@ Currently we don't implement multi-texture
  // in the generic implementation. This is a todo...
  
  // Make sure we don't process too many vertices;
  int num_vertices = 0;
  for (i = mesh->indexstart; i<mesh->indexend; ++i)
  {
    if (indices[i]+1>num_vertices)
      num_vertices = indices[i]+1;
  }

  // Update work tables.
  if (num_vertices > tr_verts->Limit ())
  {
    tr_verts->SetLimit (num_vertices);
    z_verts->SetLimit (num_vertices);
    uv_verts->SetLimit (num_vertices);
    persp->SetLimit (num_vertices);
    color_verts->SetLimit (num_vertices);
  }

  // Do vertex tweening and/or transformation to camera space
  // if any of those are needed. When this is done 'verts' will
  // point to an array of camera vertices.
  csVector3* f1 = (csVector3*)locks[CS_VATTRIB_POSITION];
  csVector2* uv1 = (csVector2*)locks[CS_VATTRIB_TEXCOORD];
  csVector3* work_verts;
  csVector2* work_uv_verts;
  csColor* work_col;
  csColor* col1 = 0;
  
  //if (mesh.use_vertex_color)
    col1 = (csColor*)locks[CS_VATTRIB_COLOR];

  //if (mesh.vertex_mode == G3DTriangleMesh::VM_WORLDSPACE)
  {
    for (i = 0 ; i < num_vertices ; i++)
      (*tr_verts)[i] = o2c * f1[i];
    work_verts = tr_verts->GetArray ();
  }
  /*else
    work_verts = f1;*/
  work_uv_verts = uv1;
  work_col = col1;

  // Perspective project.
  for (i = 0 ; i < num_vertices ; i++)
  {
    if (work_verts[i].z >= SMALL_Z)
    {
      (*z_verts)[i] = 1. / work_verts[i].z;
      float iz = aspect * (*z_verts)[i];
      (*persp)[i].x = work_verts[i].x * iz + width2;
      (*persp)[i].y = work_verts[i].y * iz + height2;
    }
  }

  // Clipped polygon (assume it cannot have more than 64 vertices)
  G3DPolygonDPFX poly;
  memset (&poly, 0, sizeof(poly));
  poly.mat_handle = mesh->material->GetMaterialHandle ();
  poly.mixmode = mesh->mixmode | CS_FX_TILING;

  // Fill flat color if renderer decide to paint it flat-shaded
  iTextureHandle* txt_handle = mesh->material->GetMaterialHandle ()->GetTexture ();
  if (txt_handle)
    txt_handle->GetMeanColor (poly.flat_color_r,
      poly.flat_color_g, poly.flat_color_b);

  poly.use_fog = false;

  // Draw all triangles.
  csTriangle* triangles = (csTriangle*)indices;
  for (i = mesh->indexstart/3 ; i < mesh->indexend/3 ; i++)
  {
    int a = triangles[i].a;
    int b = triangles[i].b;
    int c = triangles[i].c;
    int cnt_vis = int (work_verts[a].z >= SMALL_Z) +
    		  int (work_verts[b].z >= SMALL_Z) +
    		  int (work_verts[c].z >= SMALL_Z);
    if (cnt_vis == 0)
    {
      //=====
      // Easy case: the triangle is completely not visible.
      //=====
      continue;
    }
    else if (cnt_vis == 3 || lazyclip)
    {
      //=====
      // Another easy case: all vertices are visible or we are using
      // lazy clipping in which case we draw the triangle completely.
      //=====
      int trivert [3] = { a, b, c };
      DrawTriangle (this, clipper, mesh, poly,
      	(*persp)[a], (*persp)[b], (*persp)[c], trivert,
	z_verts->GetArray (), work_uv_verts, work_col, 0);
    }
    else if (cnt_vis == 1)
    {
      //=====
      // A reasonably complex case: one vertex is visible. We need
      // to clip to the Z-plane but fortunatelly this will result in
      // another triangle.
      //=====

      // The following com_iz is valid for all points on Z-plane.
      float com_zv = 1. / (SMALL_Z*10);
      float com_iz = aspect * (1. / (SMALL_Z*10));

      csVector3 va = work_verts[a];
      csVector3 vb = work_verts[b];
      csVector3 vc = work_verts[c];
      csVector3 v;
      csVector2 pa, pb, pc;
      float zv[3];
      csVector2 uv[3];
      csColor col[3];
      G3DFogInfo fog[3];
#undef COPYVT
#define COPYVT(id,idl,i) \
	p##idl = (*persp)[i]; zv[id] = (*z_verts)[i]; uv[id] = work_uv_verts[i]; \
	if (work_col) col[id] = work_col[i];
#undef INTERPOL
#define INTERPOL(id,idl,i1,i2) \
	uv[id] = work_uv_verts[i1] + r*(work_uv_verts[i2]-work_uv_verts[i1]); \
        zv[id] = com_zv; \
	p##idl.x = v.x * com_iz + width2; p##idl.y = v.y * com_iz + width2; \
	if (work_col) \
	{ \
	  col[id].red = work_col[i1].red+r*(work_col[i2].red-work_col[i1].red); \
	  col[id].green = work_col[i1].green+r*(work_col[i2].green-work_col[i1].green); \
	  col[id].blue = work_col[i1].blue+r*(work_col[i2].blue-work_col[i1].blue); \
	}

      if (va.z >= SMALL_Z)
      {
	// Point a is visible.
	COPYVT(0,a,a)

	// Calculate intersection between a-b and Z=SMALL_Z*10.
	// p = a + r * (b-a) (parametric line equation between a and b).
	float r = (SMALL_Z*10-va.z)/(vb.z-va.z);
	v.Set (va.x + r * (vb.x-va.x), va.y + r * (vb.y-va.y), SMALL_Z*10);
	INTERPOL(1,b,a,b)
	// Calculate intersection between a-c and Z=SMALL_Z*10.
	r = (SMALL_Z*10-va.z)/(vc.z-va.z);
	v.Set (va.x + r * (vc.x-va.x), va.y + r * (vc.y-va.y), SMALL_Z*10);
	INTERPOL(2,c,a,c)
      }
      else if (vb.z >= SMALL_Z)
      {
	// Point b is visible.
	COPYVT(1,b,b)

	// Calculate intersection between b-a and Z=SMALL_Z*10.
	float r = (SMALL_Z*10-vb.z)/(va.z-vb.z);
	v.Set (vb.x + r * (va.x-vb.x), vb.y + r * (va.y-vb.y), SMALL_Z*10);
	INTERPOL(0,a,b,a)
	// Calculate intersection between b-c and Z=SMALL_Z*10.
	r = (SMALL_Z*10-vb.z)/(vc.z-vb.z);
	v.Set (vb.x + r * (vc.x-vb.x), vb.y + r * (vc.y-vb.y), SMALL_Z*10);
	INTERPOL(2,c,b,c)
      }
      else
      {
	// Point c is visible.
	COPYVT(2,c,c)

	// Calculate intersection between c-a and Z=SMALL_Z*10.
	float r = (SMALL_Z*10-vc.z)/(va.z-vc.z);
	v.Set (vc.x + r * (va.x-vc.x), vc.y + r * (va.y-vc.y), SMALL_Z*10);
	INTERPOL(0,a,c,a)
	// Calculate intersection between c-b and Z=SMALL_Z*10.
	r = (SMALL_Z*10-vc.z)/(vb.z-vc.z);
	v.Set (vc.x + r * (vb.x-vc.x), vc.y + r * (vb.y-vc.y), SMALL_Z*10);
	INTERPOL(1,b,c,b)
      }

      // Now pa, pb, pc will be a triangle that is completely visible.
      // uv[0..2] contains the texture coordinates.
      // zv[0..2] contains 1/z
      // col[0..2] contains color information
      // fog[0..2] contains fog information

      int trivert [3] = { 0, 1, 2 };
      DrawTriangle (this, clipper, mesh, poly,
      	pa, pb, pc, trivert,
	zv, uv, work_col ? col : 0, fog);
    }
    else
    {
      //=====
      // The most complicated case: two vertices are visible. In this
      // case clipping to the Z-plane does not result in a triangle.
      // So we have to triangulate.
      // We will triangulate to triangles a,b,c, and a,c,d.
      //=====

      // The following com_iz is valid for all points on Z-plane.
      float com_zv = 1. / (SMALL_Z*10);
      float com_iz = aspect * (1. / (SMALL_Z*10));

      csVector3 va = work_verts[a];
      csVector3 vb = work_verts[b];
      csVector3 vc = work_verts[c];
      csVector3 v;
      csVector2 pa, pb, pc, pd;
      float zv[4];
      csVector2 uv[4];
      csColor col[4];
      G3DFogInfo fog[4];
      if (va.z < SMALL_Z)
      {
	// Point a is not visible.
	COPYVT(1,b,b)
	COPYVT(2,c,c)

	// Calculate intersection between a-b and Z=SMALL_Z*10.
	// p = a + r * (b-a) (parametric line equation between a and b).
	float r = (SMALL_Z*10-va.z)/(vb.z-va.z);
	v.Set (va.x + r * (vb.x-va.x), va.y + r * (vb.y-va.y), SMALL_Z*10);
	INTERPOL(0,a,a,b)
	// Calculate intersection between a-c and Z=SMALL_Z*10.
	r = (SMALL_Z*10-va.z)/(vc.z-va.z);
	v.Set (va.x + r * (vc.x-va.x), va.y + r * (vc.y-va.y), SMALL_Z*10);
	INTERPOL(3,d,a,c)
      }
      else if (vb.z < SMALL_Z)
      {
	// Point b is not visible.
	COPYVT(0,a,a)
	COPYVT(3,d,c)

	// Calculate intersection between b-a and Z=SMALL_Z*10.
	float r = (SMALL_Z*10-vb.z)/(va.z-vb.z);
	v.Set (vb.x + r * (va.x-vb.x), vb.y + r * (va.y-vb.y), SMALL_Z*10);
	INTERPOL(1,b,b,a)
	// Calculate intersection between b-c and Z=SMALL_Z*10.
	r = (SMALL_Z*10-vb.z)/(vc.z-vb.z);
	v.Set (vb.x + r * (vc.x-vb.x), vb.y + r * (vc.y-vb.y), SMALL_Z*10);
	INTERPOL(2,c,b,c)
      }
      else
      {
	// Point c is not visible.
	COPYVT(0,a,a)
	COPYVT(1,b,b)

	// Calculate intersection between c-a and Z=SMALL_Z*10.
	float r = (SMALL_Z*10-vc.z)/(va.z-vc.z);
	v.Set (vc.x + r * (va.x-vc.x), vc.y + r * (va.y-vc.y), SMALL_Z*10);
	INTERPOL(3,d,c,a)
	// Calculate intersection between c-b and Z=SMALL_Z*10.
	r = (SMALL_Z*10-vc.z)/(vb.z-vc.z);
	v.Set (vc.x + r * (vb.x-vc.x), vc.y + r * (vb.y-vc.y), SMALL_Z*10);
	INTERPOL(2,c,c,b)
      }

      // Now pa,pb,pc and pa,pc,pd will be triangles that are visible.
      // uv[0..3] contains the texture coordinates.
      // zv[0..3] contains 1/z
      // col[0..3] contains color information
      // fog[0..3] contains fog information

      int trivert1[3] = { 0, 1, 2 };
      DrawTriangle (this, clipper, mesh, poly,
      	pa, pb, pc, trivert1,
	zv, uv, work_col ? col : 0, fog);
      int trivert2[3] = { 0, 2, 3 };
      DrawTriangle (this, clipper, mesh, poly,
      	pa, pc, pd, trivert2,
	zv, uv, work_col ? col : 0, fog);
    }
  }

  indexbuf->Release ();
  for (i=0; i<16; ++i)
  {
    if (activebuffers[i] != 0)
      activebuffers[i]->Release ();
  }
}
