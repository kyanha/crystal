/*
    Copyright (C) 1998 by Jorrit Tyberghein
  
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
#include "cssys/csendian.h"
#include "csengine/lghtmap.h"
#include "csengine/polygon.h"
#include "csengine/sector.h"
#include "csengine/thing.h"
#include "csengine/light.h"
#include "csengine/world.h"
#include "csengine/curve.h"
#include "csutil/util.h"
#include "ivfs.h"

csShadowMap::csShadowMap ()
{
  map = NULL;
}

csShadowMap::~csShadowMap ()
{
  delete [] map;
}

#define lightcell_size	csLightMap::lightcell_size
#define lightcell_shift	csLightMap::lightcell_shift

void csShadowMap::Alloc (csLight*, int w, int h)
{
  delete [] map; map = NULL;

  int lw = csLightMap::CalcLightMapWidth (w);
  int lh = csLightMap::CalcLightMapHeight (h);

  long lm_size = lw * lh;
  map = new unsigned char [lm_size];
  memset (map, 0, lm_size);
}

#undef lightcell_size
#undef lightcell_shift

void csShadowMap::CopyLightMap (csShadowMap *source, int size)
{
  if (map) delete [] map;
  map = new unsigned char [size];
  memcpy (map, source->map, size);
  light = source->light;
}

//---------------------------------------------------------------------------

int csLightMap::lightcell_size = 16;
int csLightMap::lightcell_shift = 4;

IMPLEMENT_IBASE (csLightMap)
  IMPLEMENTS_INTERFACE (iLightMap)
IMPLEMENT_IBASE_END

csLightMap::csLightMap ()
{
  CONSTRUCT_IBASE (NULL);
  first_smap = NULL;
  cachedata = NULL;
  delayed_light_info = NULL;
  dyn_dirty = true;
}

csLightMap::~csLightMap ()
{
  while (first_smap)
  {
    csShadowMap *smap = first_smap->next;
    delete first_smap;
    first_smap = smap;
  }
  static_lm.Clear ();
  real_lm.Clear ();
}

void csLightMap::SetLightCellSize (int size)
{
  lightcell_size = size;
  lightcell_shift = csLog2 (size);
}

void csLightMap::DelShadowMap (csShadowMap* smap)
{
  first_smap = smap->next;
  delete smap;
}

csShadowMap *csLightMap::NewShadowMap (csLight* light, int w, int h)
{
  csShadowMap *smap = new csShadowMap ();
  smap->light = light;
  smap->next = first_smap;
  first_smap = smap;

  smap->Alloc (light, w, h);

  return smap;
}

csShadowMap* csLightMap::FindShadowMap (csLight* light)
{
  csShadowMap* smap = first_smap;
  while (smap)
  {
    if (smap->light == light) return smap;
    smap = smap->next;
  }
  return NULL;
}

void csLightMap::SetSize (int w, int h)
{
  rwidth  = lwidth  = csLightMap::CalcLightMapWidth (w);
  rheight = lheight = csLightMap::CalcLightMapHeight (h);
  lm_size = lwidth * lheight;
}

void csLightMap::Alloc (int w, int h, int r, int g, int b)
{
  SetSize (w, h);
  static_lm.Clear ();
  real_lm.Clear ();

  static_lm.Alloc (lm_size);
  real_lm.Alloc (lm_size);

  UByte *mr = static_lm.GetRed ();
  memset (mr, r, lm_size);
  UByte *mg = static_lm.GetGreen ();
  memset (mg, g, lm_size);
  UByte *mb = static_lm.GetBlue ();
  memset (mb, b, lm_size);
}

void csLightMap::CopyLightMap (csLightMap* source)
{
  lm_size = source->lm_size;
  static_lm.Copy (source->static_lm, lm_size);
  real_lm.Copy (source->real_lm, lm_size);
  lwidth = source->lwidth;
  lheight = source->lheight;
  rwidth = source->rwidth;
  rheight = source->rheight;

  csShadowMap *smap, *smap2;
  while (first_smap)
  {
    smap = first_smap->next;
    delete first_smap;
    first_smap = smap;
  }

  smap = source->first_smap;
  while (smap)
  {
    smap2 = new csShadowMap ();
    smap2->next = first_smap;
    first_smap = smap2;
    smap2->CopyLightMap (smap, lm_size);
    smap = smap->next;
  }
}

struct PolySave
{
  char header[4];
  short x1, y1, z1;	// Coordinate of vertex 1
  short x2, y2, z2;	// Coordinate of vertex 2
  long lm_size;		// Size of lightmap
  long lm_cnt;		// Number of non-dynamic lightmaps (normally 3)
};

struct LightSave
{
  CS_ID id;
};

struct LightHeader
{
  char header[4];
  long dyn_cnt;		// Number of dynamic maps
};

void CacheName (char *buf, csPolygonSet *owner, int index, char *suffix)
{
  const char* name = owner->GetName ();
  if (!name)
    CsPrintf (MSG_WARNING, "Lighting cache is used while some objects don't have names!\n");

  if (owner->GetType () == csThing::Type)
  {
    const char* pname = ((csThing*)owner)->GetMovable ().GetSector (0)->GetName ();
    if (!pname)
      CsPrintf (MSG_WARNING, "Lighting cache is used while some objects don't have names!\n");
    sprintf (buf, "lm/%s_%s_%d%s", pname ? pname : ".", name ? name : ".", index, suffix);
  }
  else
    sprintf (buf, "lm/%s_%d%s", name ? name : ".", index, suffix);
}

bool csLightMap::ReadFromCache (int w, int h, csPolygonSet* owner,
  csObject* obj, bool isPolygon, int index, csWorld* world)
{
  char buf[200];
  PolySave ps, pswanted;
  LightHeader lh;
  LightSave ls;
  csLight* light;
  int i;

  csPolygon3D* poly = NULL;
  
  if (isPolygon)
    poly = (csPolygon3D*)obj;

  SetSize (w, h);

  strcpy (pswanted.header, "LM02");
  if (poly)
  {
    pswanted.x1 = convert_endian (float2short (poly->Vobj (0).x));
    pswanted.y1 = convert_endian (float2short (poly->Vobj (0).y));
    pswanted.z1 = convert_endian (float2short (poly->Vobj (0).z));
    pswanted.x2 = convert_endian (float2short (poly->Vobj (1).x));
    pswanted.y2 = convert_endian (float2short (poly->Vobj (1).y));
    pswanted.z2 = convert_endian (float2short (poly->Vobj (1).z));
  }
  pswanted.lm_size = convert_endian (lm_size);

  CacheName (buf, owner, index, "");

  iDataBuffer* data = world->VFS->ReadFile (buf);
  if (!data) return false;

  char *d = **data;
  memcpy (ps.header, d, 4); d += 4;
  memcpy (&ps.x1, d, sizeof (ps.x1)); d += sizeof (ps.x1);
  memcpy (&ps.y1, d, sizeof (ps.x1)); d += sizeof (ps.x1);
  memcpy (&ps.z1, d, sizeof (ps.x1)); d += sizeof (ps.x1);
  memcpy (&ps.x2, d, sizeof (ps.x1)); d += sizeof (ps.x1);
  memcpy (&ps.y2, d, sizeof (ps.x1)); d += sizeof (ps.x1);
  memcpy (&ps.z2, d, sizeof (ps.x1)); d += sizeof (ps.x1);
  memcpy (&ps.lm_size, d, sizeof (ps.lm_size)); d += sizeof (ps.lm_size);
  memcpy (&ps.lm_cnt, d, sizeof (ps.lm_size)); d += sizeof (ps.lm_size);

  //-------------------------------
  // Check if cached item is still valid.
  //-------------------------------
  if (strncmp (ps.header, pswanted.header, 4) != 0 || (
	poly && (
	ps.x1 != pswanted.x1 ||
	ps.y1 != pswanted.y1 ||
	ps.z1 != pswanted.z1 ||
	ps.x2 != pswanted.x2 ||
	ps.y2 != pswanted.y2 ||
	ps.z2 != pswanted.z2 ||
        ps.lm_size != pswanted.lm_size)))
  {
    // Invalid.
    data->DecRef ();
    return false;
  }

  //-------------------------------
  // The cached item is valid.
  //-------------------------------
  static_lm.Clear ();

  static_lm.Alloc (lm_size);
  memcpy (static_lm.GetMap (), d, lm_size * 3);
  d += lm_size * 3;

  data->DecRef ();

  //-------------------------------
  // Now load the dynamic data.
  //-------------------------------
  CacheName (buf, owner, index, "_d");
  data = world->VFS->ReadFile (buf);
  if (!data) return true;	// No dynamic data. @@@ Recalculate dynamic data?

  d = **data;
  memcpy (lh.header, d, 4); d += 4;
  memcpy (&lh.dyn_cnt, d, 4); d += 4;
  lh.dyn_cnt = convert_endian (lh.dyn_cnt);

  for (i = 0; i < lh.dyn_cnt; i++)
  {
    memcpy (&ls.id, d, sizeof (CS_ID)); d += sizeof (CS_ID);
    ls.id = convert_endian (ls.id);

    light = world->FindLight (ls.id);
    if (light)
    {
      if (light->GetType () == csStatLight::Type && obj)
      {
        csStatLight* slight = (csStatLight*)light;
        // @@@: HACK!!! Curves and Polygon3Ds need a common base class which would
        //      help this temporary hack out a lot
        if (isPolygon)
        {
          //@@@: is this cast neccessary or should I
          //     put MakeDynamicDirty() in iLightMap
          slight->RegisterLightMap ( (csLightMap *)( ((csPolygon3D*)obj )->GetLightMapInfo()->GetLightMap() ) ); 
        }
        else
        {
          slight->RegisterLightMap ( ((csCurve*)obj )->GetLightMap() );
        }
      }
      csShadowMap* smap = NewShadowMap (light, w, h);
      memcpy (smap->map, d, lm_size);
    }
    else
    {
      CsPrintf (MSG_WARNING, "Warning! Light (%ld) not found!\n", ls.id);
    }
    d += lm_size;
  }

  delete [] data;

  return true;
}

void csLightMap::Cache (csPolygonSet* owner, csPolygon3D* poly, int index, csWorld* world)
{
  (void) world;
  char buf[200];
  PolySave ps;
  long l;
  short s;

  strcpy (ps.header, "LM02");
  if (poly)
  {
    ps.x1 = convert_endian (float2short (poly->Vobj (0).x));
    ps.y1 = convert_endian (float2short (poly->Vobj (0).y));
    ps.z1 = convert_endian (float2short (poly->Vobj (0).z));
    ps.x2 = convert_endian (float2short (poly->Vobj (1).x));
    ps.y2 = convert_endian (float2short (poly->Vobj (1).y));
    ps.z2 = convert_endian (float2short (poly->Vobj (1).z));
  }
  ps.lm_size = convert_endian (lm_size);
  ps.lm_cnt = 0;
  if (static_lm.GetRed ()) ps.lm_cnt++;
  if (static_lm.GetGreen ()) ps.lm_cnt++;
  if (static_lm.GetBlue ()) ps.lm_cnt++;
  ps.lm_cnt = convert_endian (ps.lm_cnt);

  //-------------------------------
  // Write the normal lightmap data.
  //-------------------------------
  CacheName (buf, owner, index, "");
  iFile *cf = world->VFS->Open (buf, VFS_FILE_WRITE);
  cf->Write (ps.header, 4);
  s = ps.x1;      cf->Write ((char*)&s, sizeof (s));
  s = ps.y1;      cf->Write ((char*)&s, sizeof (s));
  s = ps.z1;      cf->Write ((char*)&s, sizeof (s));
  s = ps.x2;      cf->Write ((char*)&s, sizeof (s));
  s = ps.y2;      cf->Write ((char*)&s, sizeof (s));
  s = ps.z2;      cf->Write ((char*)&s, sizeof (s));
  l = ps.lm_size; cf->Write ((char*)&l, sizeof (l));
  l = ps.lm_cnt;  cf->Write ((char*)&l, sizeof (l));

  if (static_lm.GetRed ()) cf->Write ((char*)static_lm.GetRed (), lm_size);
  if (static_lm.GetGreen ()) cf->Write ((char*)static_lm.GetGreen (), lm_size);
  if (static_lm.GetBlue ()) cf->Write ((char*)static_lm.GetBlue (), lm_size);

  // close the file
  cf->DecRef ();

  //-------------------------------
  // Write the dynamic data.
  //-------------------------------
  LightHeader lh;

  csShadowMap* smap = first_smap;
  if (smap)
  {
    strcpy (lh.header, "DYNL");
    lh.dyn_cnt = 0;
    while (smap) { lh.dyn_cnt++; smap = smap->next; }
    smap = first_smap;

    CacheName (buf, owner, index, "_d");
    cf = world->VFS->Open (buf, VFS_FILE_WRITE);
    cf->Write (lh.header, 4);
    l = convert_endian (lh.dyn_cnt);
    cf->Write ((char*)&l, 4);
    while (smap)
    {
      csLight* light = smap->light;
      if (smap->map)
      {
        LightSave ls;
	ls.id = convert_endian (light->GetID ());
        cf->Write ((char*)&ls.id, sizeof (CS_ID));
        cf->Write ((char*)(smap->map), lm_size);
      }
      smap = smap->next;
    }
    cf->DecRef ();
  }
}

bool csLightMap::UpdateRealLightMap ()
{
  if (!dyn_dirty) return false;

  dyn_dirty = false;

  //---
  // First copy the static lightmap to the real lightmap.
  // Remember the real lightmap first so that we can see if
  // there were any changes.
  //---

  memcpy (real_lm.GetMap (), static_lm.GetMap (), 3 * lm_size);

  //---
  // Then add all pseudo-dynamic lights.
  //---
  csLight* light;
  unsigned char* mapR, * mapG, * mapB;
  float red, green, blue;
  unsigned char* p, * last_p;
  int l, s;

  if (first_smap)
  {
    csShadowMap* smap = first_smap;

    // Color mode.
    do
    {
      mapR = real_lm.GetRed ();
      mapG = real_lm.GetGreen ();
      mapB = real_lm.GetBlue ();
      light = smap->light;
      red = light->GetColor ().red;
      green = light->GetColor ().green;
      blue = light->GetColor ().blue;
      csLight::CorrectForNocolor (&red, &green, &blue);
      p = smap->map;
      last_p = p+lm_size;
      do
      {
        s = *p++;
        l = *mapR + QRound (red * s);
        *mapR++ = l < 255 ? l : 255;
        l = *mapG + QRound (green * s);
        *mapG++ = l < 255 ? l : 255;
        l = *mapB + QRound (blue * s);
        *mapB++ = l < 255 ? l : 255;
      }
      while (p < last_p);

      smap = smap->next;
    }
    while (smap);
  }

  return true;
}

void csLightMap::ConvertToMixingMode ()
{
  int i;
  int mer, meg, meb;
  mer = 0;
  meg = 0;
  meb = 0;
  unsigned char* mr, * mg, * mb;
  mr = static_lm.GetRed ();
  mg = static_lm.GetGreen ();
  mb = static_lm.GetBlue ();
  for (i = 0 ; i < lm_size ; i++)
  {
    mer += mr[i];
    meg += mg[i];
    meb += mb[i];
  }
  mean_color.red   = mer/lm_size;
  mean_color.green = meg/lm_size;
  mean_color.blue  = meb/lm_size;

  //@@@
  //if (Textures::mixing == MIX_TRUE_RGB) return;
  //else convert_to_mixing_mode (static_lm.mapR, static_lm.mapG, static_lm.mapB, lm_size);
}

void csLightMap::ConvertToMixingMode (unsigned char* mr, unsigned char* mg,
				       unsigned char* mb, int sz)
{
  if (!mr || !mg || !mb) return; // If we have a dynamic light there is no conversion needed

  while (sz > 0)
  {
    csLight::CorrectForNocolor (mr, mg, mb);
    mr++;
    mg++;
    mb++;
    sz--;
  }
}

// Only works for expanding a map.
void ResizeMap (unsigned char* old_map, int oldw, int oldh,
		 unsigned char* new_map, int neww, int /*newh*/)
{
  int row;
  for (row = 0 ; row < oldh ; row++)
    memcpy (new_map+neww*row, old_map+oldw*row, oldw);
}

void csLightMap::ConvertFor3dDriver (bool requirePO2, int maxAspect)
{
  if (!requirePO2) return; // Nothing to do.
  int oldw = lwidth, oldh = lheight;

  lwidth  = FindNearestPowerOf2 (lwidth);
  lheight = FindNearestPowerOf2 (lheight);

  while (lwidth/lheight > maxAspect) lheight += lheight;
  while (lheight/lwidth > maxAspect) lwidth  += lwidth;

  if (oldw == lwidth && oldh == lheight) return;	// Already ok, nothing to do.

  // Move the old data to o_stat and o_real.
  csRGBLightMap o_stat, o_real;
  o_stat.SetMap (static_lm.GetMap ()); static_lm.SetMap (NULL);
  o_stat.SetMaxSize (static_lm.GetMaxSize ());
  o_real.SetMap (real_lm.GetMap ()); real_lm.SetMap (NULL);
  o_real.SetMaxSize (real_lm.GetMaxSize ());

  lm_size = lwidth * lheight;
 
  // Allocate new data and transform old to new.
  static_lm.Alloc (lm_size);
  ResizeMap (o_stat.GetRed (), oldw, oldh, static_lm.GetRed (), lwidth, lheight);
  ResizeMap (o_stat.GetGreen (), oldw, oldh, static_lm.GetGreen (), lwidth, lheight);
  ResizeMap (o_stat.GetBlue (), oldw, oldh, static_lm.GetBlue (), lwidth, lheight);

  real_lm.Alloc (lm_size);
  ResizeMap (o_real.GetRed (), oldw, oldh, real_lm.GetRed (), lwidth, lheight);
  ResizeMap (o_real.GetGreen (), oldw, oldh, real_lm.GetGreen (), lwidth, lheight);
  ResizeMap (o_real.GetBlue (), oldw, oldh, real_lm.GetBlue (), lwidth, lheight);

  // Convert all shadowmaps.
  csShadowMap* smap = first_smap;
  while (smap)
  {
    unsigned char* old_map = smap->map;
    smap->map = new unsigned char [lm_size];
    ResizeMap (old_map, oldw, oldh, smap->map, lwidth, lheight);
    delete [] old_map;
    smap = smap->next;
  }
}

unsigned char *csLightMap::GetMap (int nMap)
{
  switch (nMap)
  {
    case 0:
      return GetRealMap ().GetRed ();
      break;
    case 1:
      return GetRealMap ().GetGreen ();
      break;
    case 2:
      return GetRealMap ().GetBlue ();
      break;
  }
  return NULL;
}
