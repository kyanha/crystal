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

#include "sysdef.h"
#include "csgeom/frustrum.h"
#include "csengine/sysitf.h"
#include "csengine/light/light.h"
#include "csengine/sector.h"
#include "csengine/objects/thing.h"
#include "csengine/polygon/polygon.h"

//---------------------------------------------------------------------------

int csLight::ambient_red = DEFAULT_LIGHT_LEVEL;
int csLight::ambient_green = DEFAULT_LIGHT_LEVEL;
int csLight::ambient_blue = DEFAULT_LIGHT_LEVEL;
int csLight::ambient_white = DEFAULT_LIGHT_LEVEL;

CSOBJTYPE_IMPL(csLight,csObject);

csLight::csLight (float x, float y, float z, float dist,
	      float red, float green, float blue) : csObject()
{
  center.x = x;
  center.y = y;
  center.z = z;
  csLight::dist = dist;
  csLight::sqdist = dist*dist;
  color.red = red;
  color.green = green;
  color.blue = blue;

  halo_enabled = false;
  halo_intensity = 0.0f;
  halo_max_intensity = 1.0f;
  halo_ref_count = 0;
  in_halo_queue = false;
}

csLight::~csLight ()
{
}

void csLight::CorrectForNocolor (unsigned char* rp, unsigned char* gp,
					unsigned char* bp)
{
//@@@
#if 0
  if (Textures::mixing == MIX_TRUE_RGB)
    return;
  else if (Textures::mixing == MIX_NOCOLOR)
  {
    unsigned char w = (*rp+*gp+*bp)/3;
    *rp = w;
    *gp = w;
    *bp = w;
  }
  else
  {
    int r = *rp;
    int g = *gp;
    int b = *bp;
    int w = r;
    if (g < w) w = g;
    if (b < w) w = b;
    r -= w;
    g -= w;
    b -= w;

    *rp = w;

    if (Textures::color_table2 == TABLE_RED) *gp = r;
    else if (Textures::color_table2 == TABLE_GREEN) *gp = g;
    else if (Textures::color_table2 == TABLE_BLUE) *gp = b;

    if (Textures::color_table3 == TABLE_RED) *bp = r;
    else if (Textures::color_table3 == TABLE_GREEN) *bp = g;
    else if (Textures::color_table3 == TABLE_BLUE) *bp = b;
  }
#endif
}

void csLight::CorrectForNocolor (float* rp, float* gp, float* bp)
{
//@@@
#if 0
  if (Textures::mixing == MIX_TRUE_RGB)
    return;
  else if (Textures::mixing == MIX_NOCOLOR)
  {
    float w = (*rp+*gp+*bp)/3;
    *rp = w;
    *gp = w;
    *bp = w;
  }
  else
  {
    float r = *rp;
    float g = *gp;
    float b = *bp;
    float w = r;
    if (g < w) w = g;
    if (b < w) w = b;
    r -= w;
    g -= w;
    b -= w;

    *rp = w;

    if (Textures::color_table2 == TABLE_RED) *gp = r;
    else if (Textures::color_table2 == TABLE_GREEN) *gp = g;
    else if (Textures::color_table2 == TABLE_BLUE) *gp = b;

    if (Textures::color_table3 == TABLE_RED) *bp = r;
    else if (Textures::color_table3 == TABLE_GREEN) *bp = g;
    else if (Textures::color_table3 == TABLE_BLUE) *bp = b;
  }
#endif
}

//---------------------------------------------------------------------------

CSOBJTYPE_IMPL(csStatLight,csLight);

csStatLight::csStatLight (float x, float y, float z, float dist,
	      float red, float green, float blue, bool dynamic)
	      : csLight (x, y, z, dist, red, green, blue)
{
  csStatLight::dynamic = dynamic;
  polygons = NULL;
}

csStatLight::~csStatLight ()
{
  CHK (delete [] polygons);
}

void csStatLight::ShineLightmaps ()
{
  //CsPrintf (MSG_INITIALIZATION, "  Shine light (%f,%f,%f).\n", center.x, center.y, center.z);
  csLightView lview;
  lview.l = this;
  lview.center = center;
  lview.mirror = false;
  lview.gouroud_only = false;
  lview.gouroud_color_reset = false;
  lview.r = GetColor ().red;
  lview.g = GetColor ().green;
  lview.b = GetColor ().blue;
  lview.dynamic = false;

  sector->ShineLightmaps (lview);

  //CHK (lview.light_frustrum = new csFrustrum (center));
  //lview.light_frustrum->MakeInfinite ();
  //sector->CalculateLightmaps (lview);
}

void csStatLight::ShineLightmaps (csThing* th)
{
  //CsPrintf (MSG_INITIALIZATION, "  Shine light (%f,%f,%f).\n", center.x, center.y, center.z);
  csLightView lview;
  lview.l = this;
  lview.center = center;
  lview.mirror = false;
  lview.gouroud_only = false;
  lview.gouroud_color_reset = false;
  lview.r = GetColor ().red;
  lview.g = GetColor ().green;
  lview.b = GetColor ().blue;
  lview.dynamic = false;
  th->ShineLightmaps (lview);
}


void csStatLight::DumpFrustrum (csTransform& t)
{
  sector->DumpFrustrum (this, NULL, 0, t);
}

void csStatLight::RegisterPolygon (csPolygon3D* poly)
{
  if (dynamic && !polygons)
  {
    num_polygon = 0;
    CHK (polygons = new csPolygon3D* [MAX_NUM_POLYGON]);
  }

  int i;
  for (i = 0 ; i < num_polygon ; i++)
    if (polygons[i] == poly) return;
  polygons[num_polygon++] = poly;
  if (num_polygon >= MAX_NUM_POLYGON)
  {
    CsPrintf (MSG_WARNING, "Overflow number of polygons for dynamic light!\n");
  }
}

void csStatLight::SetColor (const csColor& col)
{
  csLight::SetColor (col);
  int i;
  for (i = 0 ; i < num_polygon ; i++)
    polygons[i]->MakeDirtyDynamicLights ();
}

//---------------------------------------------------------------------------

csLightPatch::csLightPatch ()
{
  next_poly = prev_poly = NULL;
  next_light = prev_light = NULL;
  num_vertices = 0;
  vertices = NULL;
  polygon = NULL;
  light = NULL;
}

csLightPatch::~csLightPatch ()
{
  CHK (delete [] vertices);
  if (polygon) polygon->UnlinkLightpatch (this);
  if (light) light->UnlinkLightpatch (this);
}

//---------------------------------------------------------------------------

CSOBJTYPE_IMPL(csDynLight,csLight);

csDynLight::csDynLight (float x, float y, float z, float dist,
	      float red, float green, float blue)
	      : csLight (x, y, z, dist, red, green, blue)
{
  lightpatches = NULL;
}

csDynLight::~csDynLight ()
{
  while (lightpatches) CHKB (delete lightpatches);
}

void csDynLight::Setup ()
{
  while (lightpatches) CHKB (delete lightpatches);
  csLightView lview;
  lview.l = this;
  lview.center = center;
  lview.frustrum = NULL;
  lview.num_frustrum = 0;
  lview.mirror = false;
  lview.gouroud_only = false;
  lview.gouroud_color_reset = false;
  lview.r = GetColor ().red;
  lview.g = GetColor ().green;
  lview.b = GetColor ().blue;
  lview.dynamic = true;
  sector->ShineLightmaps (lview);
}

void csDynLight::Move (csSector* sector, float x, float y, float z)
{
  if (sector == csDynLight::sector && center.x == x && center.y == y && center.z == z)
  {
    // No move. Just return.
    return;
  }
  SetSector (sector);
  center.x = x;
  center.y = y;
  center.z = z;
}

void csDynLight::UnlinkLightpatch (csLightPatch* lp)
{
  if (lp->next_light) lp->next_light->prev_light = lp->prev_light;
  if (lp->prev_light) lp->prev_light->next_light = lp->next_light;
  else lightpatches = lp->next_light;
  lp->prev_light = lp->next_light = NULL;
  lp->light = NULL;
}

void csDynLight::AddLightpatch (csLightPatch* lp)
{
  lp->next_light = lightpatches;
  lp->prev_light = NULL;
  if (lightpatches) lightpatches->prev_light = lp;
  lightpatches = lp;
  lp->light = this;
}


//---------------------------------------------------------------------------
