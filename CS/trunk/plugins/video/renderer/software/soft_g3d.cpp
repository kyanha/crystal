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

#define CS_SYSDEF_PROVIDE_SOFTWARE2D
#include "cssysdef.h"
#include "soft_g3d.h"
#include "protex3d.h"
#include "isys/system.h"
#include "isys/plugin.h"
#include "ivideo/graph2d.h"
#include "iutil/cfgfile.h"
#include "iutil/cmdline.h"
#include "iutil/objreg.h"
#include "ivaria/reporter.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (csGraphics3DSoftware)
SCF_DECLARE_FACTORY (csSoftProcTexture3D)

SCF_EXPORT_CLASS_TABLE (soft3d)
  SCF_EXPORT_CLASS_DEP (csGraphics3DSoftware,
    "crystalspace.graphics3d.software",
    "Software 3D graphics driver for Crystal Space",
    "crystalspace.font.server.")
  SCF_EXPORT_CLASS (csSoftProcTexture3D, 
    "crystalspace.graphics3d.software.offscreen",
    "Software 3D off screen driver")
SCF_EXPORT_CLASS_TABLE_END

SCF_IMPLEMENT_IBASE_EXT (csGraphics3DSoftware)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iConfig)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csGraphics3DSoftware::eiSoftConfig)
  SCF_IMPLEMENTS_INTERFACE (iConfig)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csGraphics3DSoftware::csGraphics3DSoftware (iBase *iParent)
  : csGraphics3DSoftwareCommon (iParent)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiConfig);
  is_for_procedural_textures = false;
  csScan_Initialize ();
}

csGraphics3DSoftware::~csGraphics3DSoftware ()
{
  csScan_Finalize ();
}

bool csGraphics3DSoftware::Initialize (iSystem *iSys)
{
  csGraphics3DSoftwareCommon::Initialize(iSys);
  iObjectRegistry* object_reg = iSys->GetObjectRegistry ();
  iPluginManager* plugin_mgr = CS_QUERY_REGISTRY (object_reg, iPluginManager);
  iCommandLineParser* cmdline = CS_QUERY_REGISTRY (object_reg,
  	iCommandLineParser);
  
  NewInitialize ();
  const char *driver = cmdline->GetOption ("canvas");
  if (!driver)
    driver = config->GetStr ("Video.Software.Canvas", CS_SOFTWARE_2D_DRIVER);
  G2D = CS_LOAD_PLUGIN (plugin_mgr, driver, NULL, iGraphics2D);
  return G2D ? true : false;
}

bool csGraphics3DSoftware::Open ()
{
  if (!csGraphics3DSoftwareCommon::Open () || !NewOpen ())
    return false;

  bool bFullScreen = G2D->GetFullScreen ();
  Report (CS_REPORTER_SEVERITY_NOTIFY, 
	    "Using %s mode %dx%d (internal rendering at %dx%d).",
            bFullScreen ? "full screen" : "windowed", 
	    G2D->GetWidth (), G2D->GetHeight (), width, height);

  if (pfmt.PixelBytes == 4)
    Report (CS_REPORTER_SEVERITY_NOTIFY, 
      "Using truecolor mode with %d bytes per pixel and %d:%d:%d RGB mode.",
      pfmt.PixelBytes, pfmt.RedBits, pfmt.GreenBits, pfmt.BlueBits);
  else if (pfmt.PixelBytes == 2)
    Report (CS_REPORTER_SEVERITY_NOTIFY, 
      "Using truecolor mode with %d bytes per pixel and %d:%d:%d RGB mode.",
      pfmt.PixelBytes, pfmt.RedBits, pfmt.GreenBits, pfmt.BlueBits);
  else
    Report (CS_REPORTER_SEVERITY_NOTIFY, 
	       "Using palette mode with 1 byte per pixel (256 colors).");

  return true;
}

//---------------------------------------------------------------------------

#define NUM_OPTIONS 8

static const csOptionDescription config_options [NUM_OPTIONS] =
{
  { 0, "ilace", "Interlacing", CSVAR_BOOL },
  { 1, "light", "Texture lighting", CSVAR_BOOL },
  { 2, "alpha", "Semi-transparent textures", CSVAR_BOOL },
  { 3, "txtmap", "Texture mapping", CSVAR_BOOL },
  { 4, "mmx", "MMX support", CSVAR_BOOL },
  { 5, "gamma", "Gamma value", CSVAR_FLOAT },
  { 6, "gouraud", "Gouraud shading", CSVAR_BOOL },
  { 7, "smaller", "Smaller rendering", CSVAR_BOOL },
};

bool csGraphics3DSoftware::eiSoftConfig::SetOption (int id, csVariant* value)
{
  if (value->GetType () != config_options[id].type)
    return false;
  switch (id)
  {
    case 0: scfParent->do_interlaced = value->GetBool () ? 0 : -1; break;
    case 1: scfParent->do_lighting = value->GetBool (); break;
    case 2: scfParent->do_alpha = value->GetBool (); break;
    case 3: scfParent->do_textured = value->GetBool (); break;
#ifdef DO_MMX
    case 4: scfParent->do_mmx = value->GetBool (); break;
#endif
    case 5: scfParent->texman->Gamma = value->GetFloat (); break;
    case 6: scfParent->do_gouraud = value->GetBool (); break;
    case 7: scfParent->do_smaller_rendering = value->GetBool (); break;
    default: return false;
  }
  scfParent->ScanSetup ();
  return true;
}

bool csGraphics3DSoftware::eiSoftConfig::GetOption (int id, csVariant* value)
{
  switch (id)
  {
    case 0: value->SetBool (scfParent->do_interlaced != -1); break;
    case 1: value->SetBool (scfParent->do_lighting); break;
    case 2: value->SetBool (scfParent->do_alpha); break;
    case 3: value->SetBool (scfParent->do_textured); break;
#ifdef DO_MMX
    case 4: value->SetBool (scfParent->do_mmx); break;
#else
    case 4: value->SetBool (false); break;
#endif
    case 5: value->SetFloat (scfParent->Gamma / 65536.); break;
    case 6: value->SetBool (scfParent->do_gouraud); break;
    case 7: value->SetBool (scfParent->do_smaller_rendering); break;
    default: return false;
  }
  return true;
}

bool csGraphics3DSoftware::eiSoftConfig::GetOptionDescription
  (int idx, csOptionDescription* option)
{
  if (idx < 0 || idx >= NUM_OPTIONS)
    return false;
  *option = config_options[idx];
  return true;
}
