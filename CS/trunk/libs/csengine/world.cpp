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

#include "cssysdef.h"
#include "qint.h"
#include "csutil/scf.h"
#include "csengine/world.h"
#include "csengine/dumper.h"
#include "csengine/halo.h"
#include "csengine/camera.h"
#include "csengine/campos.h"
#include "csengine/keyval.h"
#include "csengine/light.h"
#include "csengine/dynlight.h"
#include "csengine/polyplan.h"
#include "csengine/polytmap.h"
#include "csengine/polygon.h"
#include "csengine/pol2d.h"
#include "csengine/polytext.h"
#include "csengine/thingtpl.h"
#include "csengine/thing.h"
#include "csengine/cssprite.h"
#include "csengine/csspr2d.h"
#include "csengine/cscoll.h"
#include "csengine/sector.h"
#include "csengine/covcube.h"
#include "csengine/cbufcube.h"
#include "csengine/texture.h"
#include "csengine/lghtmap.h"
#include "csengine/stats.h"
#include "csengine/cspmeter.h"
#include "csengine/cbuffer.h"
#include "csengine/quadtr3d.h"
#include "csengine/lppool.h"
#include "csengine/covtree.h"
#include "csengine/particle.h"
#include "csengine/radiosty.h"
#include "csgeom/fastsqrt.h"
#include "csgeom/polypool.h"
#include "cssys/csevent.h"
#include "csgfxldr/csimage.h"
#include "csutil/util.h"
#include "csutil/halogen.h"
#include "iimage.h"
#include "ivfs.h"
#include "ihalo.h"
#include "itxtmgr.h"
#include "igraph3d.h"

//---------------------------------------------------------------------------

csPolyIt::csPolyIt (csWorld* w) : world(w)
{
  Restart();
}

void csPolyIt::Restart ()
{
  sector_idx = -1;
  thing = NULL;
  polygon_idx = 0;
}

csPolygon3D* csPolyIt::Fetch ()
{
  csSector* sector;
  if (sector_idx == -1)
  {
    sector_idx = 0;
    thing = NULL;
    polygon_idx = -1;
  }

  if (sector_idx >= world->sectors.Length ()) return NULL;
  sector = (csSector*)(world->sectors[sector_idx]);

  // Try next polygon.
  polygon_idx++;

  if (thing)
  {
    // We are busy scanning the things of this sector.
    // See if the current thing has the indicated polygon number.
    // If not then we try the next thing.
    while (thing && polygon_idx >= thing->GetNumPolygons ())
    {
      thing = (csThing*)(thing->GetNext ());
      polygon_idx = 0;
    }
    if (!thing)
    {
      // There are no more things left. Go to the next sector.
      sector_idx++;
      if (sector_idx >= world->sectors.Length ()) return NULL;
      // Initialize iterator to start of sector and recurse.
      thing = NULL;
      polygon_idx = -1;
      return Fetch ();
    }
  }
  else if (polygon_idx >= sector->GetNumPolygons ())
  {
    // We are not scanning things but we have no more polygons in
    // this sector. Start scanning things.
    polygon_idx = -1;
    thing = sector->GetFirstThing ();
    // Recurse.
    if (thing) return Fetch ();
    // No things. Go to next sector.
    sector_idx++;
    if (sector_idx >= world->sectors.Length ()) return NULL;
    // Initialize iterator to start of sector and recurse.
    thing = NULL;
    return Fetch ();
  }

  return thing ?
    thing->GetPolygon3D (polygon_idx) : sector->GetPolygon3D (polygon_idx);
}

//--------------------- csCurveIt -------------------------------------------

csCurveIt::csCurveIt (csWorld* w) : world(w)
{
  Restart();
}

void csCurveIt::Restart ()
{
  sector_idx = -1;
  thing = NULL;
  curve_idx = 0;
}

csCurve* csCurveIt::Fetch ()
{
  csSector* sector;
  if (sector_idx == -1)
  {
    sector_idx = 0;
    thing = NULL;
    curve_idx = -1;
  }

  if (sector_idx >= world->sectors.Length ()) return NULL;
  sector = (csSector*)(world->sectors[sector_idx]);

  // Try next polygon.
  curve_idx++;

  if (thing)
  {
    // We are busy scanning the things of this sector.
    // See if the current thing has the indicated curve number.
    // If not then we try the next thing.
    while (thing && curve_idx >= thing->GetNumCurves ())
    {
      thing = (csThing*)(thing->GetNext ());
      curve_idx = 0;
    }
    if (!thing)
    {
      // There are no more things left. Go to the next sector.
      sector_idx++;
      if (sector_idx >= world->sectors.Length ()) return NULL;
      // Initialize iterator to start of sector and recurse.
      thing = NULL;
      curve_idx = -1;
      return Fetch ();
    }
  }
  else if (curve_idx >= sector->GetNumCurves ())
  {
    // We are not scanning things but we have no more polygons in
    // this sector. Start scanning things.
    curve_idx = -1;
    thing = sector->GetFirstThing ();

    // Recurse.
    if (thing) return Fetch ();
    
    // No things. Go to next sector.
    sector_idx++;
    
    if (sector_idx >= world->sectors.Length ()) return NULL;
    
    // Initialize iterator to start of sector and recurse.
    thing = NULL;
    
    return Fetch ();
  }

  return thing ?
    thing->GetCurve (curve_idx) : sector->GetCurve (curve_idx);
}

//---------------------------------------------------------------------------

csLightIt::csLightIt (csWorld* w) : world(w)
{
  Restart();
}

void csLightIt::Restart ()
{
  sector_idx = -1;
  light_idx = 0;
}

csLight* csLightIt::Fetch ()
{
  csSector* sector;
  if (sector_idx == -1)
  {
    sector_idx = 0;
    light_idx = -1;
  }

  if (sector_idx >= world->sectors.Length ()) return NULL;
  sector = (csSector*)(world->sectors[sector_idx]);

  // Try next light.
  light_idx++;

  if (light_idx >= sector->lights.Length ())
  {
    // Go to next sector.
    light_idx = -1;
    sector_idx++;
    if (sector_idx >= world->sectors.Length ()) return NULL;
    // Initialize iterator to start of sector and recurse.
    return Fetch ();
  }

  csLight* light;
  light = (csLight*)(sector->lights[light_idx]);
  return light;
}

//---------------------------------------------------------------------------

int csWorld::frame_width;
int csWorld::frame_height;
iSystem* csWorld::System = NULL;
csWorld* csWorld::current_world = NULL;
bool csWorld::use_new_radiosity = false;
int csWorld::max_process_polygons = 2000000000;
int csWorld::cur_process_polygons = 0;
bool csWorld::do_lighting_cache = true;
bool csWorld::do_not_force_relight = false;
bool csWorld::do_force_relight = false;
bool csWorld::do_not_force_revis = false;
bool csWorld::do_force_revis = false;
bool csWorld::do_accurate_statlight = true;
bool csWorld::do_accurate_dynlight = false;

IMPLEMENT_CSOBJTYPE (csWorld,csObject);

IMPLEMENT_IBASE (csWorld)
  IMPLEMENTS_INTERFACE (iPlugIn)
  IMPLEMENTS_INTERFACE (iWorld)
  IMPLEMENTS_EMBEDDED_INTERFACE (iConfig)
IMPLEMENT_IBASE_END

IMPLEMENT_FACTORY (csWorld)

EXPORT_CLASS_TABLE (engine)
  EXPORT_CLASS_DEP (csWorld, "crystalspace.engine.core",
    "Crystal Space 3D Engine", "crystalspace.kernel., crystalspace.graphics3d.")
EXPORT_CLASS_TABLE_END

csWorld::csWorld (iBase *iParent) : csObject (), camera_positions (16, 16)
{
  CONSTRUCT_IBASE (iParent);
  CONSTRUCT_EMBEDDED_IBASE (scfiConfig);
  engine_mode = CS_ENGINE_AUTODETECT;
  first_dyn_lights = NULL;
  System = NULL;
  VFS = NULL;
  G3D = NULL;
  G2D = NULL;
  textures = NULL;
  c_buffer = NULL;
  quad3d = NULL;
  covcube = NULL;
  cbufcube = NULL;
  covtree = NULL;
  covtree_lut = NULL;
  current_camera = NULL;
  current_world = this;
  use_pvs = false;
  use_pvs_only = false;
  freeze_pvs = false;
  Library = NULL;
  world_states = NULL;

  if (!covtree_lut)
  {
    covtree_lut = new csCovMaskLUT (16);
  }
  //covcube = new csCovcube (covtree_lut);
  cbufcube = new csCBufferCube (1024);

  SetCuller (CS_CULLER_CBUFFER);

  textures = new csTextureList ();

  render_pol2d_pool = new csPoly2DPool (csPolygon2DFactory::SharedFactory());
  lightpatch_pool = new csLightPatchPool ();

  BuildSqrtTable ();
  resize = false;
}

// @@@ Hack
csCamera* camera_hack = NULL;

csWorld::~csWorld ()
{
  Clear ();
  if (G3D) G3D->DecRef ();
  if (VFS) VFS->DecRef ();
  if (System) System->DecRef ();
  delete textures;
  delete render_pol2d_pool;
  delete lightpatch_pool;
  delete covcube;
  delete cbufcube;
  delete covtree_lut;

  // @@@ temp hack
  delete camera_hack;
  camera_hack = NULL;
}

bool csWorld::Initialize (iSystem* sys)
{
#if defined(JORRIT_DEBUG)
  printf ("csPolygon3D %ld\n", (long)sizeof (csPolygon3D));
  printf ("csPolyPlane %ld\n", (long)sizeof (csPolyPlane));
  printf ("csPolyTxtPlane %ld\n", (long)sizeof (csPolyTxtPlane));
  printf ("csPolyTexture %ld\n", (long)sizeof (csPolyTexture));
  printf ("csPolygonSet %ld\n", (long)sizeof (csPolygonSet));
  printf ("csLightMapped %ld\n", (long)sizeof (csLightMapped));
  printf ("csGouraudShaded %ld\n", (long)sizeof (csGouraudShaded));
  printf ("csLightMap %ld\n", (long)sizeof (csLightMap));
#endif

  (System = sys)->IncRef ();

  if (!(G3D = QUERY_PLUGIN (sys, iGraphics3D)))
    return false;

  if (!(VFS = QUERY_PLUGIN (sys, iVFS)))
    return false;

  G2D = G3D->GetDriver2D ();

  // Tell system driver that we want to handle broadcast events
  if (!System->CallOnEvents (this, CSMASK_Broadcast))
    return false;

  ReadConfig ();

  return true;
}

// Handle some system-driver broadcasts
bool csWorld::HandleEvent (csEvent &Event)
{
  if (Event.Type == csevBroadcast)
    switch (Event.Command.Code)
    {
      case cscmdSystemOpen:
      {
        csGraphics3DCaps *caps = G3D->GetCaps ();
        fogmethod = caps->fog;
        NeedPO2Maps = caps->NeedsPO2Maps;
        MaxAspectRatio = caps->MaxAspectRatio;

        frame_width = G3D->GetWidth ();
        frame_height = G3D->GetHeight ();
        if (csCamera::GetDefaultFOV () == 0)
          csCamera::SetDefaultFOV (frame_height);

        // @@@ Ugly hack to always have a camera in current_camera.
        // This is needed for the lighting routines.
        if (!current_camera)
        {
          current_camera = new csCamera ();
          camera_hack = current_camera;
        }

        StartWorld ();

        return true;
      } /* endif */
      case cscmdContextResize:
      {
	if (world_states)
	  world_states->Resize ((iGraphics2D*) Event.Command.Info);
	else
	  if (((iGraphics2D*) Event.Command.Info) == G2D)
	    resize = true;
	return false;
      }
      case cscmdContextClose:
      {
	if (world_states)
	{
	  world_states->Close ((iGraphics2D*) Event.Command.Info);
	  if (!world_states->Length())
	  {
	    delete world_states;
	    world_states = NULL;
	  }
	}
	return false;
      }
    } /* endswitch */

  return false;
}

void csWorld::Clear ()
{
  halos.DeleteAll ();
  collections.DeleteAll ();
  while (sprites.Length () > 0)
    delete (csSprite*)sprites[0];
  sprite_templates.DeleteAll ();
  thing_templates.DeleteAll ();
  sectors.DeleteAll ();
  CLights::DeleteAll ();
  camera_positions.DeleteAll ();
  int i;
  for (i = 0 ; i < planes.Length () ; i++)
  {
    csPolyTxtPlane* p = (csPolyTxtPlane*)planes[i];
    planes[i] = NULL;
    p->DecRef ();
  }
  planes.DeleteAll ();

  while (first_dyn_lights)
  {
    csDynLight* dyn = first_dyn_lights->GetNext ();
    delete first_dyn_lights;
    first_dyn_lights = dyn;
  }
  delete textures; textures = NULL;
  textures = new csTextureList ();
  SetCuller (CS_CULLER_CBUFFER);
  delete render_pol2d_pool;
  render_pol2d_pool = new csPoly2DPool (csPolygon2DFactory::SharedFactory());
  delete lightpatch_pool;
  lightpatch_pool = new csLightPatchPool ();
  use_pvs = false;

  // Clear all object libraries
  Library = NULL;
  Libraries.DeleteAll ();
  
  if (world_states)
  {
    world_states->DeleteAll ();
    delete world_states;
    world_states = NULL;
  }
}

void csWorld::ResolveEngineMode ()
{
  if (engine_mode == CS_ENGINE_AUTODETECT)
  {
    // Here we do some heuristic. We scan all sectors and see if there
    // are some that have big octrees. If so we select CS_ENGINE_FRONT2BACK.
    // If not we select CS_ENGINE_BACK2FRONT.
    // @@@ It might be an interesting option to try to find a good engine
    // mode for every sector and switch dynamically based on the current
    // sector (only switch based on the position of the camera, not switch
    // based on the sector we are currently rendering).
    int switch_f2b = 0;
    for (int i = 0 ; i < sectors.Length () ; i++)
    {
      csSector* s = (csSector*)sectors[i];
      csPolygonTree* ptree = s->GetStaticTree ();
      if (ptree)
      {
        csOctree* otree = (csOctree*)ptree;
	int num_nodes = otree->GetRoot ()->CountChildren ();
	if (num_nodes > 30) switch_f2b += 10;
	else if (num_nodes > 15) switch_f2b += 5;
      }
      if (switch_f2b >= 10) break;
    }
    if (switch_f2b >= 10)
    {
      engine_mode = CS_ENGINE_FRONT2BACK;
      CsPrintf (MSG_CONSOLE, "Engine is using front2back mode\n");
    }
    else
    {
      engine_mode = CS_ENGINE_BACK2FRONT;
      CsPrintf (MSG_CONSOLE, "Engine is using back2front mode\n");
    }
  }
}

void csWorld::EnableLightingCache (bool en)
{
  do_lighting_cache = en;
  if (!do_lighting_cache) do_force_relight = true;
}

void csWorld::SetCuller (int culler)
{
  delete c_buffer; c_buffer = NULL;
  delete covtree; covtree = NULL;
  delete quad3d; quad3d = NULL;
  switch (culler)
  {
    case CS_CULLER_CBUFFER:
      c_buffer = new csCBuffer (0, frame_width-1, frame_height);
      break;
    case CS_CULLER_QUAD3D:
    {
      csVector3 corners[4];
      corners[0].Set (-1, -1, 1);
      corners[1].Set (1, -1, 1);
      corners[2].Set (1, 1, 1);
      corners[3].Set (-1, 1, 1);
      csBox3 bbox;
      quad3d = new csQuadTree3D (csVector3 (0, 0, 0),
    	  corners, bbox, 5);
      break;
    }
    case CS_CULLER_COVTREE:
    {
      csBox2 box (0, 0, frame_width, frame_height);
      if (!covtree_lut)
      {
        covtree_lut = new csCovMaskLUT (16);
      }
      covtree = new csCoverageMaskTree (covtree_lut, box);
      break;
    }
  }
}

csSpriteTemplate* csWorld::GetSpriteTemplate (const char* name)
{
  return (csSpriteTemplate *)sprite_templates.FindByName (name);
}

csThingTemplate* csWorld::GetThingTemplate (const char* name)
{
  return (csThingTemplate *)thing_templates.FindByName (name);
}

csSector* csWorld::NewSector ()
{
  csSector* s = new csSector (this);
  s->SetAmbientColor (csLight::ambient_red, csLight::ambient_green, csLight::ambient_blue);
  sectors.Push (s);
  return s;
}

csThing* csWorld::GetThing (const char* name)
{
  int i = sectors.Length ();
  while (i > 0)
  {
    i--;
    csSector* s = (csSector*)sectors[i];
    csThing* t = s->GetThing (name);
    if (t) return t;
  }
  return NULL;
}

void csWorld::PrepareTextures ()
{
  iTextureManager* txtmgr = G3D->GetTextureManager ();
  txtmgr->ResetPalette ();

  // First register all textures to the texture manager.
  for (int i = 0; i < textures->Length (); i++)
    textures->Get (i)->Register (txtmgr);

  // Prepare all the textures.
  txtmgr->PrepareTextures ();
}

void csWorld::PrepareSectors()
{
  // Now precalculate some stuff for all loaded polygons.
  for (int i = 0 ; i < sectors.Length () ; i++)
  {
    csSector* s = (csSector*)sectors[i];
    s->Prepare ();
  }
}

bool csWorld::Prepare ()
{
  PrepareTextures ();
  PrepareSectors ();

  // The images are no longer needed by the 3D engine.
  iTextureManager *txtmgr = G3D->GetTextureManager ();
  txtmgr->FreeImages ();

  G3D->ClearCache ();

  // Prepare lightmaps if we have any sectors
  if (sectors.Length ())
    ShineLights ();

  CheckConsistency ();

  long memory = 0;
  long tree_memory = 0;
  for (int i = 0 ; i < sectors.Length () ; i++)
  {
    csSector* s = (csSector*)sectors[i];
    memory += Dumper::Memory (s, 0);
    if (s->GetStaticTree ())
    {
      csOctree* otree = (csOctree*)(s->GetStaticTree ());
      tree_memory += Dumper::Memory (otree, 0);
    }
  }
  CsPrintf (MSG_INITIALIZATION, "World geometry is using %ld bytes.\n", memory);
  CsPrintf (MSG_INITIALIZATION, "Octree/BSP trees are using %ld bytes.\n", tree_memory);
  CsPrintf (MSG_INITIALIZATION, "Textures are using %ld texels.\n",
  	Dumper::TotalTexels (textures));

  return true;
}

void csWorld::ShineLights ()
{
  tr_manager.NewFrame ();

  if (!do_not_force_relight)
  {
    // If recalculation is not forced then we check if the 'precalc_info'
    // file exists on the VFS. If not then we will recalculate in any case.
    // If the file exists but is not valid (some flags are different) then
    // we recalculate again.
    // If we recalculate then we also update this 'precalc_info' file with
    // the new settings.
    struct PrecalcInfo
    {
      int lm_version;           // This number identifies a version of the lightmap format.
                                // If different then the format is different and we need
                                // to recalculate.
      int normal_light_level;   // Normal light level (unlighted level).
      int ambient_red;
      int ambient_green;
      int ambient_blue;
      int reflect;
      int radiosity;
      int accurate_things;
      float cosinus_factor;
      int lightmap_size;
    };
    PrecalcInfo current;
    memset (&current, 0, sizeof (current));
    current.lm_version = 1;
    current.normal_light_level = NORMAL_LIGHT_LEVEL;
    current.ambient_red = csLight::ambient_red;
    current.ambient_green = csLight::ambient_green;
    current.ambient_blue = csLight::ambient_blue;
    current.reflect = csSector::cfg_reflections;
    current.radiosity = (int)csSector::do_radiosity;
    current.accurate_things = csPolyTexture::do_accurate_things;
    current.cosinus_factor = csPolyTexture::cfg_cosinus_factor;
    current.lightmap_size = csLightMap::lightcell_size;
    char *reason = NULL;

    size_t size;
    char *data = VFS->ReadFile ("precalc_info", size);
    char *input = data;
    if (!data)
      reason = "no 'precalc_info' found";
    else
      while (*input)
      {
        char *keyword = input + strspn (input, " \t");
        char *endkw = strchr (input, '=');
        if (!endkw) break;
        *endkw++ = 0;
        input = strchr (endkw, '\n');
        if (input) *input++ = 0;
        float xf;
        sscanf (endkw, "%f", &xf);
        int xi = QRound (xf);

#define CHECK(keyw,cond,reas) \
        else if (!strcmp (keyword, keyw)) \
        { if (cond) { reason = reas " changed"; break; } }

        if (false) {}
        CHECK ("LMVERSION", xi != current.lm_version, "lightmap format")
        CHECK ("NORMALLIGHTLEVEL", xi != current.normal_light_level, "normal light level")
        CHECK ("AMBIENT_RED", xi != current.ambient_red, "ambient red level")
        CHECK ("AMBIENT_GREEN", xi != current.ambient_green, "ambient green level")
        CHECK ("AMBIENT_BLUE", xi != current.ambient_blue, "ambient blue level")
        CHECK ("REFLECT", xi != current.reflect, "reflection value")
        CHECK ("RADIOSITY", xi != current.radiosity, "radiosity value")
        CHECK ("ACCURATE_THINGS", xi != current.accurate_things, "'accurate things' flag")
        CHECK ("COSINUS_FACTOR", ABS (xf - current.cosinus_factor) > SMALL_EPSILON, "cosinus factor")
        CHECK ("LIGHTMAP_SIZE", xi != current.lightmap_size, "lightmap size")

#undef CHECK
      }
    delete [] data;

    if (reason)
    {
      char data [500];
      sprintf (data,
        "LMVERSION=%d\n"
        "NORMALLIGHTLEVEL=%d\n"
        "AMBIENT_RED=%d\n"
        "AMBIENT_GREEN=%d\n"
        "AMBIENT_BLUE=%d\n"
        "REFLECT=%d\n"
        "RADIOSITY=%d\n"
        "ACCURATE_THINGS=%d\n"
        "COSINUS_FACTOR=%g\n"
        "LIGHTMAP_SIZE=%d\n",
        current.lm_version, current.normal_light_level, current.ambient_red,
        current.ambient_green, current.ambient_blue, current.reflect,
        current.radiosity, current.accurate_things, current.cosinus_factor,
        current.lightmap_size);
      VFS->WriteFile ("precalc_info", data, strlen (data));
      CsPrintf (MSG_INITIALIZATION, "Lightmap data is not up to date (reason: %s).\n", reason);
      do_force_relight = true;
    }
  }

  if (do_force_relight)
  {
    CsPrintf (MSG_INITIALIZATION, "Recalculation of lightmaps forced.\n");
    CsPrintf (MSG_INITIALIZATION, "  Pseudo-radiosity system %s.\n", csSector::do_radiosity ? "enabled" : "disabled");
    CsPrintf (MSG_INITIALIZATION, "  Maximum number of visits per sector = %d.\n", csSector::cfg_reflections);
  }
  else
  {
    // If no recalculation is forced we set these variables to default to
    // make sure that we don't do too many unneeded calculations.
    csSector::do_radiosity = false;
    csSector::cfg_reflections = 1;
  }

  csPolyIt* pit = NewPolyIterator ();
  csLightIt* lit = NewLightIterator ();

  // Reinit all lightmaps. This loop also counts all polygons.
  csPolygon3D* p;
  int polygon_count = 0;
  pit->Restart ();
  while ((p = pit->Fetch ()) != NULL)
    polygon_count++;

  // Count number of lights to process.
  csLight* l;
  int light_count = 0;
  lit->Restart ();
  while (lit->Fetch ()) light_count++;

  int sn = 0;
  int num_sectors = sectors.Length ();
  csProgressMeter meter (System);

  CsPrintf (MSG_INITIALIZATION, "Initializing lightmaps (%d sectors):\n  ", num_sectors);
  meter.SetTotal (num_sectors);
  meter.Restart ();
  for (sn = 0; sn < num_sectors ; sn++)
  {
    csSector* s = (csSector*)sectors [sn];
    s->InitLightMaps ();
    meter.Step();
  }

  csPolyTexture::do_accurate_shadows = do_accurate_statlight;

  time_t start, stop;
  start = System->GetTime ();
  CsPrintf (MSG_INITIALIZATION, "\nShining lights (%d lights):\n  ", light_count);
  meter.SetTotal (light_count);
  meter.Restart ();
  lit->Restart ();
  while ((l = lit->Fetch ()) != NULL)
  {
    ((csStatLight*)l)->CalculateLighting ();
    meter.Step();
  }
  stop = System->GetTime ();
  CsPrintf (MSG_INITIALIZATION, "\n(%.4f seconds)", (float)(stop-start)/1000.);

  // Render radiosity
  if (use_new_radiosity && !do_not_force_relight && do_force_relight)
  {
    start = System->GetTime ();
    csRadiosity *rad = new csRadiosity(this);
    rad->DoRadiosity();
    delete rad;
    stop = System->GetTime ();
    CsPrintf (MSG_INITIALIZATION, "(%.4f seconds)", (float)(stop-start)/1000.);
  }

  csPolyTexture::do_accurate_shadows = do_accurate_dynlight;

  CsPrintf (MSG_INITIALIZATION, "\nCaching lightmaps (%d sectors):\n  ", num_sectors);
  meter.SetTotal (num_sectors);
  meter.Restart ();
  for (sn = 0; sn < num_sectors ; sn++)
  {
    csSector* s = (csSector*)sectors[sn];
    s->CacheLightMaps ();
    meter.Step();
  }

  CsPrintf (MSG_INITIALIZATION, "\nPreparing lightmaps (%d maps):\n  ", polygon_count);
  meter.SetTotal (polygon_count);
  meter.Restart ();
  pit->Restart ();
  while ((p = pit->Fetch ()) != NULL)
  {
    p->CreateLightMaps (G3D);
    meter.Step();
  }

  csPolygonSet::current_light_frame_number++;

  CsPrintf (MSG_INITIALIZATION, "\nUpdating VFS...\n");
  if (!VFS->Sync ())
    CsPrintf (MSG_WARNING, "WARNING: error updating lighttable cache!\n");
  CsPrintf (MSG_INITIALIZATION, "DONE!\n");

  delete pit;
  delete lit;
}

bool csWorld::CheckConsistency ()
{
  csPolyIt* pit = NewPolyIterator ();
  bool error = false;

  csPolygon3D* p;

  CsPrintf (MSG_INITIALIZATION, "Validating world...\n");
  pit->Restart ();
  while ((p = pit->Fetch ()) != NULL)
  {
    if (p->GetNumVertices () < 3)
    {
      CsPrintf (MSG_WARNING, "  Polygon with only %d vertices! (id=%d)\n", p->GetNumVertices (), p->GetID ());
      CsPrintf (MSG_DEBUG_0, "============ Polygon with only %d vertices (id=%d)!\n", p->GetNumVertices (), p->GetID ());
      Dumper::dump (p);
      error = true;
    }
    else if (p->GetNumVertices () > 3)
    {
      csVector3 poly[3];
      int i, j;
      poly[0] = p->Vobj (0);
      j = 1;
      for (i = 1 ; i < p->GetNumVertices () ; i++)
        if (!((poly[j-1] - p->Vobj (i)) < SMALL_EPSILON))
	{
	  poly[j] = p->Vobj (i);
	  j++;
	  if (j > 2) break;
	}
      if (j > 2)
      {
        csVector3 normal;
        float D;
        csMath3::CalcPlane (poly[0], poly[1], poly[2], normal, D);
        csPlane3 pl (normal, D);
        for (i = 3 ; i < p->GetNumVertices () ; i++)
        {
          if (ABS (pl.Classify (p->Vobj (i))) > EPSILON)
	  {
            CsPrintf (MSG_WARNING, "  Non-coplanar polygon! (id=%d)\n", p->GetID ());
            CsPrintf (MSG_DEBUG_0, "============ Non-coplanar polygon (id=%d)!\n", p->GetID ());
            Dumper::dump (p);
            error = true;
	    break;
	  }
	}
      }
    }
  }

  delete pit;
  CsPrintf (MSG_INITIALIZATION, "DONE\n");
  return error;
}

void csWorld::StartWorld ()
{
  Clear ();
}

void csWorld::StartDraw (csCamera* c, csClipper* view, csRenderView& rview)
{
  Stats::polygons_considered = 0;
  Stats::polygons_drawn = 0;
  Stats::portals_drawn = 0;
  Stats::polygons_rejected = 0;
  Stats::polygons_accepted = 0;

  // Make sure we select an engine mode.
  ResolveEngineMode ();

  current_camera = c;
  rview.world = this;

  // This flag is set in HandleEvent on a cscmdContextResize event
  if (resize) 
  {
    resize = false;
    Resize ();
  }

  // when many cameras per context, need to make sure each camera has updated
  // shift_* fields, after resizing.
  //  current_camera->SetPerspectiveCenter (frame_width/2, frame_height/2);
  // smgh: depreciated.. it is now the apps responsibility to monitor context
  // resize events and update the cameras accordingly. (dtsimple implements this)
  top_clipper = view;

  rview.clip_plane.Set (0, 0, 1, -1);   //@@@CHECK!!!

  // Calculate frustum for screen dimensions (at z=1).
  float leftx = - c->GetShiftX () * c->GetInvFOV ();
  float rightx = (frame_width - c->GetShiftX ()) * c->GetInvFOV ();
  float topy = - c->GetShiftY () * c->GetInvFOV ();
  float boty = (frame_height - c->GetShiftY ()) * c->GetInvFOV ();
  rview.SetFrustum (leftx, rightx, topy, boty);

  // Initialize our transformation manager so that we will get
  // new transformed coordinates.
  tr_manager.NewFrame ();

  // Initialize the 2D/3D culler.
  if (engine_mode == CS_ENGINE_FRONT2BACK)
  {
    if (c_buffer)
    {
      c_buffer->Initialize ();
      c_buffer->InsertPolygon (view->GetClipPoly (), view->GetNumVertices (), true);
    }
    else if (quad3d)
    {
      csVector3 corners[4];
      c->InvPerspective (csVector2 (0, 0), 1, corners[0]);
      corners[0] = c->Camera2World (corners[0]);
      c->InvPerspective (csVector2 (frame_width-1, 0), 1, corners[1]);
      corners[1] = c->Camera2World (corners[1]);
      c->InvPerspective (csVector2 (frame_width-1, frame_height-1), 1, corners[2]);
      corners[2] = c->Camera2World (corners[2]);
      c->InvPerspective (csVector2 (0, frame_height-1), 1, corners[3]);
      corners[3] = c->Camera2World (corners[3]);
      quad3d->SetMainFrustum (c->GetOrigin (), corners);
      quad3d->MakeEmpty ();
    }
    else if (covtree)
    {
      covtree->MakeEmpty ();
      covtree->UpdatePolygonInverted (view->GetClipPoly (), view->GetNumVertices ());
    }
  }

  cur_process_polygons = 0;
}

void csWorld::Draw (csCamera* c, csClipper* view)
{
  csRenderView rview (*c, view, G3D, G2D);
  StartDraw (c, view, rview);
  rview.callback = NULL;

  csSector* s = c->GetSector ();
  s->Draw (rview);

  // draw all halos on the screen
  for (int halo = halos.Length () - 1; halo >= 0; halo--)
    if (!ProcessHalo (halos.Get (halo)))
      halos.Delete (halo);
}

void csWorld::DrawFunc (csCamera* c, csClipper* view,
	csDrawFunc* callback, void* callback_data)
{
  csRenderView rview (*c, view, G3D, G2D);
  StartDraw (c, view, rview);

  rview.callback = callback;
  rview.callback_data = callback_data;

  csSector* s = c->GetSector ();
  s->Draw (rview);
}

void csWorld::AddHalo (csLight* Light)
{
  if (!Light->flags.Check (CS_LIGHT_HALO)
   || Light->GetHaloInQueue ())
    return;

  // Transform light pos into camera space and see if it is directly visible
  csVector3 v = current_camera->World2Camera (Light->GetCenter ());

  // Check if light is behind us
  if (v.z <= SMALL_Z)
    return;

  // Project X,Y into screen plane
  float iz = current_camera->GetFOV () / v.z;
  v.x = v.x * iz + current_camera->GetShiftX ();
  v.y = frame_height - 1 - (v.y * iz + current_camera->GetShiftY ());

  // If halo is not inside visible region, return
  if (!top_clipper->IsInside (v.x, v.y))
    return;

  // Check if light is not obscured by anything
  float zv = G3D->GetZBuffValue (QRound (v.x), QRound (v.y));
  if (v.z > zv)
    return;

  // Halo size is 1/4 of the screen height; also we make sure its odd
  int hs = (frame_height / 4) | 1;
  float hi, hc;
  Light->GetHaloType (hi, hc);
  unsigned char *Alpha = GenerateHalo (hs, hi, hc);

  // Okay, put the light into the queue: first we generate the alphamap
  iHalo *handle = G3D->CreateHalo (Light->GetColor ().red,
    Light->GetColor ().green, Light->GetColor ().blue, Alpha, hs, hs);

  // We don't need alpha map anymore
  delete [] Alpha;

  // Does 3D rasterizer support halos?
  if (!handle)
    return;

  halos.Push (new csLightHalo (Light, handle));
}

bool csWorld::HasHalo (csLight* Light)
{
  return halos.FindKey (Light) >= 0;
}

void csWorld::RemoveHalo (csLight* Light)
{
  int idx = halos.FindKey (Light);
  if(idx!=-1)
    halos.Delete (idx);
}

#define HALO_INTENSITY_STEP	0.15f

bool csWorld::ProcessHalo (csLightHalo *Halo)
{
  // Whenever the center of halo (the light) is directly visible
  bool halo_vis = false;
  // Whenever at least a piece of halo is visible
  bool draw_halo = false;
  // top-left coordinates of halo rectangle
  float xtl = 0, ytl = 0;

  // Project the halo.
  csVector3 v = current_camera->World2Camera (Halo->Light->GetCenter ());
  // The clipped halo polygon
  csVector2 HaloClip [32];
  // Number of vertices in HaloClip array
  int HaloVCount = 32;

  if (v.z > SMALL_Z)
  {
    float iz = current_camera->GetFOV () / v.z;
    v.x = v.x * iz + current_camera->GetShiftX ();
    v.y = frame_height - 1 - (v.y * iz + current_camera->GetShiftY ());

    if (top_clipper->IsInside (v.x, v.y))
    {
      float zv = G3D->GetZBuffValue (QRound (v.x), QRound (v.y));
      halo_vis = (v.z <= zv);
    }

    // Create a rectangle containing the halo and clip it against screen
    int hw = Halo->Handle->GetWidth ();
    int hh = Halo->Handle->GetHeight ();
    float hw2 = float (hw) / 2.0;
    float hh2 = float (hh) / 2.0;
    csVector2 HaloPoly [4] =
    {
      csVector2 (v.x - hw2, v.y - hh2),
      csVector2 (v.x - hw2, v.y + hh2),
      csVector2 (v.x + hw2, v.y + hh2),
      csVector2 (v.x + hw2, v.y - hh2)
    };
    // Clip the halo against clipper
    if (top_clipper->Clip (HaloPoly, 4, HaloClip, HaloVCount))
    {
      xtl = HaloPoly [0].x;
      ytl = HaloPoly [0].y;
      draw_halo = true;
    }
  }

  float hintensity = Halo->Light->GetHaloIntensity ();
  if (halo_vis)
  {
    float maxintensity = Halo->Light->GetHaloMaxIntensity ();
    if (hintensity < maxintensity - HALO_INTENSITY_STEP)
      hintensity += HALO_INTENSITY_STEP;
    else
      hintensity = maxintensity;
  }
  else
  {
    hintensity -= HALO_INTENSITY_STEP;

    // this halo is completely invisible. kill it.
    if (hintensity <= 0)
      return false;
  }
  Halo->Light->SetHaloIntensity (hintensity);

  if (draw_halo)
    Halo->Handle->Draw (xtl, ytl, -1, -1, hintensity, HaloClip, HaloVCount);
  return true;
}

csStatLight* csWorld::FindLight (float x, float y, float z, float dist)
{
  csStatLight* l;
  int sn = sectors.Length ();
  while (sn > 0)
  {
    sn--;
    csSector* s = (csSector*)sectors[sn];
    l = s->FindLight (x, y, z, dist);
    if (l) return l;
  }
  return NULL;
}

csStatLight* csWorld::FindLight (CS_ID id)
{
  csStatLight* l;
  int sn = sectors.Length ();
  while (sn > 0)
  {
    sn--;
    csSector* s = (csSector*)sectors[sn];
    l = s->FindLight (id);
    if (l) return l;
  }
  return NULL;
}

void csWorld::AddDynLight (csDynLight* dyn)
{
  dyn->SetNext (first_dyn_lights);
  dyn->SetPrev (NULL);
  if (first_dyn_lights) first_dyn_lights->SetPrev (dyn);
  first_dyn_lights = dyn;
}

void csWorld::RemoveDynLight (csDynLight* dyn)
{
  if (dyn->GetNext ()) dyn->GetNext ()->SetPrev (dyn->GetPrev ());
  if (dyn->GetPrev ()) dyn->GetPrev ()->SetNext (dyn->GetNext ());
  else if (dyn == first_dyn_lights) first_dyn_lights = dyn->GetNext ();
  dyn->SetNext (NULL);
  dyn->SetPrev (NULL);
}

void csWorld::UpdateParticleSystems (time_t elapsed_time)
{
  int i;
  for (i = 0 ; i < sprites.Length () ; i++)
  {
    csSprite* sp = (csSprite*)sprites[i];
    if (sp->GetType () >= csParticleSystem::Type)
    {
      csParticleSystem* partsys = (csParticleSystem*)sp;
      partsys->Update (elapsed_time);
    }
  }

  // Delete particle systems that self-destructed now.
  i = sprites.Length ()-1;
  while (i >= 0)
  {
    csSprite* sp = (csSprite*)sprites[i];
    if (sp->GetType () >= csParticleSystem::Type)
    {
      csParticleSystem* partsys = (csParticleSystem*)sp;
      if (partsys->GetDelete ())
        delete partsys;
    }
    i--;
  }

}

void csWorld::AdvanceSpriteFrames (time_t current_time)
{
  int i;
  for (i = 0 ; i < sprites.Length () ; i++)
  {
    csSprite* sp = (csSprite*)sprites[i];
    if (sp->GetType () == csSprite3D::Type)
    {
      csSprite3D* s = (csSprite3D*)sp;
      s->NextFrame (current_time);
    }
  }
}

void csWorld::ReadConfig ()
{
  if (!System) return;
  csLightMap::SetLightCellSize (System->ConfigGetInt ("Lighting", "LightmapSize", 16));

  csLight::ambient_red = System->ConfigGetInt ("Lighting", "Ambient.Red", DEFAULT_LIGHT_LEVEL);
  csLight::ambient_green = System->ConfigGetInt ("Lighting", "Ambient.Green", DEFAULT_LIGHT_LEVEL);
  csLight::ambient_blue = System->ConfigGetInt ("Lighting", "Ambient.Blue", DEFAULT_LIGHT_LEVEL);
  int ambient_white = System->ConfigGetInt ("Lighting", "Ambient.White", DEFAULT_LIGHT_LEVEL);
  csLight::ambient_red += ambient_white;
  csLight::ambient_green += ambient_white;
  csLight::ambient_blue += ambient_white;

  // Do not allow too black environments since software renderer hates it
  if (csLight::ambient_red + csLight::ambient_green + csLight::ambient_blue < 6)
    csLight::ambient_red = csLight::ambient_green = csLight::ambient_blue = 2;

  csSector::cfg_reflections = System->ConfigGetInt ("Lighting", "Reflections", csSector::cfg_reflections);
  csPolyTexture::cfg_cosinus_factor = System->ConfigGetFloat ("Lighting", "CosinusFactor", csPolyTexture::cfg_cosinus_factor);
  csSprite3D::do_quality_lighting = System->ConfigGetYesNo ("Lighting", "SpriteHighQual", csSprite3D::do_quality_lighting);
  csSector::do_radiosity = System->ConfigGetYesNo ("Lighting", "Radiosity", csSector::do_radiosity);

  csPolyTexture::do_accurate_things = System->ConfigGetYesNo ("Lighting",
    "Shadows.Things", csPolyTexture::do_accurate_things);
  do_accurate_statlight = System->ConfigGetYesNo ("Lighting",
    "Shadows.AccurateStatic", do_accurate_statlight);
  do_accurate_dynlight = System->ConfigGetYesNo ("Lighting",
    "Shadows.AccurateDynamic", do_accurate_dynlight);

  // radiosity options
  csWorld::use_new_radiosity = System->ConfigGetYesNo ("Lighting",
    "Radiosity.Enable", csWorld::use_new_radiosity);
  csRadiosity::do_static_specular = System->ConfigGetYesNo ("Lighting",
    "Radiosity.DoStaticSpecular", csRadiosity::do_static_specular);
  csRadiosity::static_specular_amount = System->ConfigGetFloat ("Lighting",
    "Radiosity.StaticSpecularAmount", csRadiosity::static_specular_amount);
  csRadiosity::static_specular_tightness = System->ConfigGetInt ("Lighting",
    "Radiosity.StaticSpecularTightness", csRadiosity::static_specular_tightness);
  csRadiosity::colour_bleed = System->ConfigGetFloat ("Lighting",
    "Radiosity.ColourBleed", csRadiosity::colour_bleed);
  csRadiosity::stop_priority = System->ConfigGetFloat ("Lighting",
    "Radiosity.StopPriority", csRadiosity::stop_priority);
  csRadiosity::stop_improvement = System->ConfigGetFloat ("Lighting",
    "Radiosity.StopImprovement", csRadiosity::stop_improvement);
  csRadiosity::stop_iterations = System->ConfigGetInt ("Lighting",
    "Radiosity.StopIterations", csRadiosity::stop_iterations);
  csRadiosity::source_patch_size = System->ConfigGetInt ("Lighting",
    "Radiosity.SourcePatchSize", csRadiosity::source_patch_size);
}

void csWorld::UnlinkSprite (csSprite* sprite)
{
  sprite->RemoveFromSectors ();
  int idx = sprites.Find (sprite);
  if (idx == -1) return;
  sprites[idx] = NULL;
  sprites.Delete (idx);
}

void csWorld::RemoveSprite (csSprite* sprite)
{
  sprite->RemoveFromSectors ();
  int idx = sprites.Find (sprite);
  if (idx == -1) return;
  sprites[idx] = NULL;
  sprites.Delete (idx);
  delete sprite;
}

struct LightAndDist
{
  csLight* light;
  float sqdist;
};

// csLightArray is a subclass of csCleanable which is registered
// to csWorld.cleanup.
class csLightArray : public csBase
{
public:
  LightAndDist* array;
  // Size is the physical size of the array. num_lights is the number of lights in it.
  int size, num_lights;

  csLightArray () : array (NULL), size (0), num_lights (0) { }
  virtual ~csLightArray () { delete [] array; }
  void Reset () { num_lights = 0; }
  void AddLight (csLight* l, float sqdist)
  {
    if (num_lights >= size)
    {
      LightAndDist* new_array;
      new_array = new LightAndDist [size+5];
      if (array)
      {
        memcpy (new_array, array, sizeof (LightAndDist)*num_lights);
        delete [] array;
      }
      array = new_array;
      size += 5;
    }
    array[num_lights].light = l;
    array[num_lights++].sqdist = sqdist;
  };
  csLight* GetLight (int i) { return array[i].light; }
};

int compare_light (const void* p1, const void* p2)
{
  LightAndDist* sp1 = (LightAndDist*)p1;
  LightAndDist* sp2 = (LightAndDist*)p2;
  float z1 = sp1->sqdist;
  float z2 = sp2->sqdist;
  if (z1 < z2) return -1;
  else if (z1 > z2) return 1;
  return 0;
}

int csWorld::GetNearbyLights (csSector* sector, const csVector3& pos, ULong flags,
  	csLight** lights, int max_num_lights)
{
  int i;
  float sqdist;

  // This is a static light array which is adapted to the
  // right size everytime it is used. In the beginning it means
  // that this array will grow a lot but finally it will
  // stabilize to a maximum size (not big). The advantage of
  // this approach is that we don't have a static array which can
  // overflow. And we don't have to do allocation every time we
  // come here. We register this memory to the 'cleanup' array
  // in csWorld so that it will be freed later.

  static csLightArray* light_array = NULL;
  if (!light_array)
  {
    light_array = new csLightArray ();
    csWorld::current_world->cleanup.Push (light_array);
  }
  light_array->Reset ();

  // Add all dynamic lights to the array (if CS_NLIGHT_DYNAMIC is set).
  if (flags & CS_NLIGHT_DYNAMIC)
  {
    csDynLight* dl = first_dyn_lights;
    while (dl)
    {
      if (dl->GetSector () == sector)
      {
        sqdist = csSquaredDist::PointPoint (pos, dl->GetCenter ());
        if (sqdist < dl->GetSquaredRadius ()) light_array->AddLight (dl, sqdist);
      }
      dl = dl->GetNext ();
    }
  }

  // Add all static lights to the array (if CS_NLIGHT_STATIC is set).
  if (flags & CS_NLIGHT_STATIC)
  {
    for (i = 0 ; i < sector->lights.Length () ; i++)
    {
      csStatLight* sl = (csStatLight*)sector->lights[i];
      sqdist = csSquaredDist::PointPoint (pos, sl->GetCenter ());
      if (sqdist < sl->GetSquaredRadius ()) light_array->AddLight (sl, sqdist);
    }
  }

  if (light_array->num_lights <= max_num_lights)
  {
    // The number of lights that we found is smaller than what fits
    // in the array given us by the user. So we just copy them all
    // and don't need to sort.
    for (i = 0 ; i < light_array->num_lights ; i++)
      lights[i] = light_array->GetLight (i);
    return light_array->num_lights;
  }
  else
  {
    // We found more lights than we can put in the given array
    // so we sort the lights and then return the nearest.
    qsort (light_array->array, light_array->num_lights, sizeof (LightAndDist), compare_light);
    for (i = 0 ; i < max_num_lights; i++)
      lights[i] = light_array->GetLight (i);
    return max_num_lights;
  }
}

int csWorld::GetTextureFormat ()
{
  iTextureManager* txtmgr = G3D->GetTextureManager ();
  return txtmgr->GetTextureFormat ();
}

void csWorld::SelectLibrary (const char *iName)
{
  Library = Libraries.FindByName (iName);
  if (!Library)
  {
    Library = new csObjectNoDel ();
    Library->SetName (iName);
    Libraries.Push (Library);
  }
}

bool csWorld::DeleteLibrary (const char *iName)
{
  csObject *lib = Libraries.FindByName (iName);
  if (!lib) return false;

#define DELETE_ALL_OBJECTS(vector,type)				\
  { for (csObjIterator iter = lib->GetIterator (type::Type);	\
         !iter.IsFinished (); ++iter)				\
    {								\
      type &x = (type&)*iter;					\
      int idx = vector.Find (&x);					\
      if (idx >= 0) vector.Delete (idx);				\
  } }

  DELETE_ALL_OBJECTS (collections, csCollection)
  DELETE_ALL_OBJECTS (sprites, csSprite)
  DELETE_ALL_OBJECTS (sprite_templates, csSpriteTemplate)
  DELETE_ALL_OBJECTS (thing_templates, csThingTemplate)
  DELETE_ALL_OBJECTS (sectors, csSector)
  DELETE_ALL_OBJECTS ((*textures), csTextureHandle)

#undef DELETE_ALL_OBJECTS
#define DELETE_ALL_OBJECTS(vector,type)				\
  for (csObjIterator iter = lib->GetIterator (type::Type);	\
       !iter.IsFinished (); ++iter)				\
  {								\
    type &x = (type&)*iter;					\
    int idx = vector.Find (&x);					\
    if (idx >= 0) { x.DecRef (); vector [idx] = 0; }		\
  }

  DELETE_ALL_OBJECTS (planes, csPolyTxtPlane)

/*
@@todo: move all dynamic lights to a vector
  while (first_dyn_lights)
  {
    csDynLight *dyn = first_dyn_lights->GetNext ();
    delete first_dyn_lights;
    first_dyn_lights = dyn;
  }
*/

  return true;
}

void csWorld::DeleteAll ()
{
  Clear ();
}

bool csWorld::CreateTexture (const char *iName, const char *iFileName,
  csColor *iTransp, int iFlags)
{
  // First of all, load the image file
  size_t size;
  char *data = VFS->ReadFile (iFileName, size);
  if (!data || !size)
  {
    CsPrintf (MSG_WARNING, "Cannot read image file \"%s\" from VFS\n",
      iFileName);
    return false;
  }

  iImage *ifile = csImageLoader::Load ((UByte *)data, size,
    CS_IMGFMT_TRUECOLOR);// GetTextureFormat ());
  delete [] data;

  if (!ifile)
  {
    CsPrintf (MSG_WARNING, "Unknown image file format: \"%s\"\n", iFileName);
    return false;
  }

  char *xname = VFS->ExpandPath (iFileName);
  ifile->SetName (xname);
  delete [] xname;

  // Okay, now create the respective texture handle object
  csTextureHandle *tex = GetTextures ()->FindByName (iName);
  if (tex)
    tex->SetImageFile (ifile);
  else
    tex = GetTextures ()->NewTexture (ifile);

  tex->flags = iFlags;
  tex->SetName (iName);

  // dereference image pointer since tex already incremented it
  ifile->DecRef ();

  if (iTransp)
    tex->SetKeyColor (QInt (iTransp->red * 255.2),
      QInt (iTransp->green * 255.2), QInt (iTransp->blue * 255.2));

  return true;
}

bool csWorld::CreateCamera (const char *iName, const char *iSector,
  const csVector3 &iPos, const csVector3 &iForward, const csVector3 &iUpward)
{
  csCameraPosition *cp = (csCameraPosition *)camera_positions.FindByName (iName);
  if (cp)
    cp->Set (iSector, iPos, iForward, iUpward);
  else
    camera_positions.Push (new csCameraPosition (iName, iSector,
      iPos, iForward, iUpward));

  return true;
}

bool csWorld::CreateKey (const char *iName, const char *iValue)
{
  ObjAdd (new csKeyValuePair (iName, iValue));
  return true;
}

bool csWorld::CreatePlane (const char *iName, const csVector3 &iOrigin,
  const csMatrix3 &iMatrix)
{
  csPolyTxtPlane *ppl = new csPolyTxtPlane ();
  ppl->SetName (iName);
  ppl->SetTextureSpace (iMatrix, iOrigin);
  planes.Push (ppl);
  return true;
}

iSector* csWorld::CreateSector (const char *iName)
{
  csSector* sector = new csSector (this);
  sector->SetAmbientColor (csLight::ambient_red, csLight::ambient_green, csLight::ambient_blue);
  sector->SetName (iName);
  sectors.Push (sector);
  iSector *s = QUERY_INTERFACE (sector, iSector);
  sector->DecRef ();
  return s;
}

iThing *csWorld::CreateThing (const char *iName, iSector *iParent)
{
  csThing *thing = new csThing (this);
  thing->SetName (iName);
  csSector *sector = iParent->GetPrivateObject ();
  thing->SetSector (sector);
  sector->AddThing (thing);
  iThing *p = QUERY_INTERFACE (thing, iThing);
  thing->DecRef ();
  return p;
}

iSector *csWorld::GetSector (int iIndex)
{
  return &((csSector *)sectors.Get (iIndex))->scfiSector;
}

iSector *csWorld::FindSector (const char *iName)
{
  csSector *sec = (csSector *)sectors.FindByName (iName);
  return sec ? &sec->scfiSector : NULL;
}

iThing *csWorld::FindThing (const char *iName)
{
  csThing *thing = GetThing (iName);
  return thing ? &thing->scfiThing : NULL;
}

iSprite *csWorld::FindSprite (const char *iName)
{
  (void)iName;
  return NULL; /*not implemented yet*/
}

//----------------Begin-Multi-Context-Support------------------------------

void csWorld::Resize ()
{
  frame_width = G3D->GetWidth ();
  frame_height = G3D->GetHeight ();
  // Reset the culler.
  SetCuller (GetCuller ());
}

csWorld::csWorldState::csWorldState (csWorld *w)
{
  world    = w;
  c_buffer = w->c_buffer;
  covtree  = w->covtree;
  quad3d   = w->quad3d;
  G2D      = w->G2D;
  G3D      = w->G3D;
  resize   = false;
}

csWorld::csWorldState::~csWorldState ()
{
  delete c_buffer;
  delete quad3d;
  delete covtree;
}

void csWorld::csWorldState::Activate ()
{
  world->c_buffer     = c_buffer;
  world->quad3d       = quad3d;
  world->covtree      = covtree;
  world->frame_width  = G3D->GetWidth ();
  world->frame_height = G3D->GetHeight ();

  if (resize)
  {
    world->Resize ();

    c_buffer = world->c_buffer;
    quad3d   = world->quad3d;
    covtree  = world->covtree;
    resize   = false;
  }
}

void csWorld::csWorldStateVector::Close (iGraphics2D *g2d)
{
  // Hack-- with the glide back buffer implementations of procedural textures
  // circumstances are that many G3D can be associated with one G2D.
  // It is impossible to tell which is which so destroy them both, and allow
  // regeneration next time context is set to the surviving G3D associated
  // with this G2D
  for (int i = 0; i < Length (); i++)
    if (((csWorldState*)root [i])->G2D == g2d)
      Delete (i);
}

void csWorld::csWorldStateVector::Resize (iGraphics2D *g2d)
{ 
  // With the glide back buffer implementations of procedural textures
  // circumstances are that many G3D can be associated with one G2D, so
  // we test for width and height also.
  for (int i = 0; i < Length (); i++)
  {
    csWorldState *state = (csWorldState*)root [i];
    if (state->G2D == g2d)
      if ((state->G2D->GetWidth() == state->G3D->GetWidth ()) &&
	  (state->G2D->GetHeight() == state->G3D->GetHeight ()))
	  ((csWorldState*)root [i])->resize = true; 
  }
}

void csWorld::SetContext (iGraphics3D* g3d)
{
  G2D = g3d->GetDriver2D ();
  if (g3d != G3D)
  {
    G3D->DecRef ();
    G3D = g3d;
    if (!world_states) world_states = new csWorldStateVector();
    int idg3d = world_states->FindKey (g3d);
    if (idg3d < 0)
    {
      Resize ();
      world_states->Push (new csWorldState (this));
    }
    else
    {
      csWorldState *state = (csWorldState *)world_states->Get (idg3d);
      state->Activate ();
    }
    G3D->IncRef ();
  }
}  

//-------------------End-Multi-Context-Support--------------------------------
