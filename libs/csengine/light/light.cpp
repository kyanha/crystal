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

#include "cssysdef.h"
#include "csgeom/frustum.h"
#include "csengine/cbufcube.h"
#include "csengine/light.h"
#include "csengine/sector.h"
#include "csengine/thing.h"
#include "csengine/lghtmap.h"
#include "csengine/engine.h"
#include "csengine/lppool.h"
#include "csengine/polygon.h"
#include "csengine/curve.h"
#include "csengine/halo.h"
#include "csengine/meshobj.h"

int csLight::ambient_red = DEFAULT_LIGHT_LEVEL;
int csLight::ambient_green = DEFAULT_LIGHT_LEVEL;
int csLight::ambient_blue = DEFAULT_LIGHT_LEVEL;
unsigned long csLight::last_light_id = 0;

SCF_IMPLEMENT_IBASE_EXT (csLight)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iLight)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csLight::Light)
  SCF_IMPLEMENTS_INTERFACE (iLight)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csLight::csLight (float x, float y, float z, float d,
  float red, float green, float blue) : csObject()
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiLight);
  light_id = last_light_id++;
  center.x = x;
  center.y = y;
  center.z = z;

  dist = d;
  sqdist = d * d;
  inv_dist = 1 / d;
  halo = NULL;

  color.red = red;
  color.green = green;
  color.blue = blue;

  attenuation = CS_ATTN_LINEAR;
}

csLight::~csLight ()
{
  if (flags.Check (CS_LIGHT_ACTIVEHALO))
    csEngine::current_engine->RemoveHalo (this);
  delete halo;
}

void csLight::SetHalo (csHalo *Halo)
{
  delete halo;
  halo = Halo;
}

float csLight::GetBrightnessAtDistance (float d)
{
  switch (attenuation)
  {
    case CS_ATTN_NONE      : return 1;
    case CS_ATTN_LINEAR    : return 1 - d * inv_dist;
    case CS_ATTN_INVERSE   : return 1 / d;
    case CS_ATTN_REALISTIC : return 1 / (d*d);
  }
  return 0;
}

iCrossHalo* csLight::Light::CreateCrossHalo (float intensity, float cross)
{
  csCrossHalo* halo = new csCrossHalo (intensity, cross);
  scfParent->SetHalo (halo);
  iCrossHalo* ihalo = SCF_QUERY_INTERFACE_FAST (halo, iCrossHalo);
  ihalo->DecRef ();
  return ihalo;
}

iNovaHalo* csLight::Light::CreateNovaHalo (int seed, int num_spokes,
  	float roundness)
{
  csNovaHalo* halo = new csNovaHalo (seed, num_spokes, roundness);
  scfParent->SetHalo (halo);
  iNovaHalo* ihalo = SCF_QUERY_INTERFACE_FAST (halo, iNovaHalo);
  ihalo->DecRef ();
  return ihalo;
}

iFlareHalo* csLight::Light::CreateFlareHalo ()
{
  csFlareHalo* halo = new csFlareHalo ();
  scfParent->SetHalo (halo);
  iFlareHalo* ihalo = SCF_QUERY_INTERFACE_FAST (halo, iFlareHalo);
  ihalo->DecRef ();
  return ihalo;
}


//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE_EXT (csStatLight)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iStatLight)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csStatLight::eiStaticLight)
  SCF_IMPLEMENTS_INTERFACE (iStatLight)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csStatLight::csStatLight (float x, float y, float z, float dist,
  float red, float green, float blue, bool dynamic)
  : csLight (x, y, z, dist, red, green, blue)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiStatLight);
  csStatLight::dynamic = dynamic;
  flags.SetAll (CS_LIGHT_THINGSHADOWS);
}

csStatLight::~csStatLight ()
{
}

static void node_light_func (csOctreeNode* node, csFrustumView* lview, bool vis)
{
  if (vis) return;
  if (!vis)
  {
    // If this node is not visible we still have to mark all polygons
    // in this node as having no light.
    csPolygonIntArray& pols = node->GetUnsplitPolygons ();
    int i;
    for (i = 0 ; i < pols.GetPolygonCount () ; i++)
    {
      csPolygonInt* pi = pols.GetPolygon (i);
      if (pi->GetType () == 1)
      {
        csPolygon3D* p = (csPolygon3D*)pi;
	// If this polygon is the top-level polygon (i.e. base or original
	// polygon) then we know the entire polygon is shadowed. In that
	// case we have to do nothing.
	if (p->GetBasePolygon () != p)
	{
	  // Otherwise we have to calculate lighting (but with vis set
	  // to false).
	  p->CalculateLightingNew (lview, false);
	}
      }
    }
  }
}

static void poly_light_func (csObject* obj, csFrustumView* lview, bool vis)
{
  csPolygon3D* poly = (csPolygon3D*)obj;
  //if (lview->IsDynamic ())
  //{
    if (!vis) return;
    poly->CalculateLighting (lview);
  //}
  //else
    //poly->CalculateLightingNew (lview, vis);
}

static void curve_light_func (csObject* obj, csFrustumView* lview, bool vis)
{
  csCurve* curve = (csCurve*)obj;
  if (vis)
    curve->CalculateLighting (*lview);
}

void csStatLight::CalculateLighting ()
{
  csFrustumView lview;
  csFrustumContext* ctxt = lview.GetFrustumContext ();
  csLightingInfo& linfo = ctxt->GetLightingInfo ();
  linfo.SetGouraudOnly (false);
  linfo.SetColor (GetColor ());
  lview.SetUserData ((void*)this);
  lview.SetNodeFunction (node_light_func);
  lview.SetPolygonFunction (poly_light_func);
  lview.SetCurveFunction (curve_light_func);
  lview.SetRadius (GetRadius ());
  lview.EnableThingShadows (flags.Get () & CS_LIGHT_THINGSHADOWS);
  lview.SetDynamic (false);
  lview.SetShadowMask (CS_ENTITY_NOSHADOWS, 0);
  lview.SetProcessMask (CS_ENTITY_NOLIGHTING, 0);
  csLightingPolyTexQueue* lptq = new csLightingPolyTexQueue ();
  //@@@ TWO KINDS OF USERDATA!!!
  lview.SetUserdata (lptq);

  ctxt->SetLightFrustum (new csFrustum (center));
  ctxt->GetLightFrustum ()->MakeInfinite ();
  sector->CheckFrustum ((iFrustumView*)&lview);

  lptq->UpdateMaps (this, GetCenter (), GetColor ());
  lptq->DecRef ();
}

void csStatLight::CalculateLighting (iMeshWrapper* th)
{
  csFrustumView lview;
  csFrustumContext* ctxt = lview.GetFrustumContext ();
  csLightingInfo& linfo = ctxt->GetLightingInfo ();
  linfo.SetGouraudOnly (false);
  linfo.SetColor (GetColor ());
  lview.SetUserData ((void*)this);
  lview.SetNodeFunction (node_light_func);
  lview.SetPolygonFunction (poly_light_func);
  lview.SetCurveFunction (curve_light_func);
  lview.SetRadius (GetRadius ());
  lview.EnableThingShadows (flags.Get () & CS_LIGHT_THINGSHADOWS);
  lview.SetDynamic (false);
  lview.SetShadowMask (CS_ENTITY_NOSHADOWS, 0);
  lview.SetProcessMask (CS_ENTITY_NOLIGHTING, 0);

  csLightingPolyTexQueue* lptq = new csLightingPolyTexQueue ();
  //@@@ TWO KINDS OF USERDATA!!!
  lview.SetUserdata (lptq);

  ctxt->SetLightFrustum (new csFrustum (center));
  ctxt->GetLightFrustum ()->MakeInfinite ();
  // @@@ Engine should not know about iThingState!!!
  if (th)
  {
    iThingState* thing_state = SCF_QUERY_INTERFACE_FAST (th->GetMeshObject (),
  	  iThingState);
    if (thing_state)
    {
      thing_state->CheckFrustum ((iFrustumView*)&lview, th->GetMovable ());
      thing_state->DecRef ();
    }
  }

  lptq->UpdateMaps (this, GetCenter (), GetColor ());
  lptq->DecRef ();
}

void csStatLight::LightingFunc (csLightingFunc* callback, void* callback_data)
{
#if 0
  csFrustumView lview;
  csFrustumContext* ctxt = lview.GetFrustumContext ();
  csLightingInfo& linfo = ctxt->GetLightingInfo ();
  linfo.SetGouraudOnly (false);
  linfo.SetColor (GetColor ());
  lview.SetUserData ((void*)this);
  lview.SetNodeFunction (node_light_func);
  lview.SetPolygonFunction (poly_light_func);
  lview.SetCurveFunction (curve_light_func);
  lview.SetRadius (GetRadius ());
  lview.EnableThingShadows (flags.Get () & CS_LIGHT_THINGSHADOWS);
  lview.SetDynamic (false);
  lview.callback = callback;
  lview.callback_data = callback_data;
  lview.shadow_thing_mask = CS_ENTITY_NOSHADOWS;
  lview.shadow_thing_value = 0;
  lview.process_thing_mask = CS_ENTITY_NOLIGHTING;
  lview.process_thing_value = 0;

  ctxt->light_frustum = new csFrustum (center);
  ctxt->light_frustum->MakeInfinite ();
  sector->CheckFrustum ((iFrustumView*)&lview);
#endif
}


void csStatLight::RegisterLightMap (csLightMap* lmap)
{
  if (!dynamic) return;
  lightmaps.SetLength (0);

  int i;
  for (i = 0 ; i < lightmaps.Length () ; i++)
    if (((csLightMap*)lightmaps[i]) == lmap)
      return;
  lightmaps.Push (lmap);
}

void csStatLight::SetColor (const csColor& col)
{
  csLight::SetColor (col);
  int i;
  for (i = 0 ; i < lightmaps.Length () ; i++)
  {
    ((csLightMap*)lightmaps[i])->MakeDirtyDynamicLights ();
  }
}

//---------------------------------------------------------------------------

csLightPatch::csLightPatch ()
{
  next_poly = prev_poly = NULL;
  next_light = prev_light = NULL;
  num_vertices = 0;
  max_vertices = 0;
  vertices = NULL;
  polygon = NULL;
  light = NULL;
  light_frustum = NULL;
}

csLightPatch::~csLightPatch ()
{
  delete [] vertices;
  if (light_frustum) light_frustum->DecRef ();
  RemovePatch ();
}

void csLightPatch::RemovePatch ()
{
  if (polygon) polygon->UnlinkLightpatch (this);
  if (curve) curve->UnlinkLightPatch (this);
  if (light) light->UnlinkLightpatch (this);
  shadows.DeleteShadows ();
  if (light_frustum)
  {
    light_frustum->DecRef ();
    light_frustum = NULL;
  }
}

void csLightPatch::Initialize (int n)
{
  if (n > max_vertices)
  {
    delete [] vertices;
    max_vertices = n;
    vertices = new csVector3 [max_vertices];
  }
  num_vertices = n;
}

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE_EXT (csDynLight)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iDynLight)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csDynLight::eiDynLight)
  SCF_IMPLEMENTS_INTERFACE (iDynLight)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csDynLight::csDynLight (float x, float y, float z, float dist,
  float red, float green, float blue)
  : csLight (x, y, z, dist, red, green, blue)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiDynLight);
  lightpatches = NULL;
}

csDynLight::~csDynLight ()
{
  while (lightpatches)
    csEngine::current_engine->lightpatch_pool->Free (lightpatches);
  csEngine::current_engine->RemoveDynLight (this);
}

void csDynLight::Setup ()
{
  while (lightpatches)
    csEngine::current_engine->lightpatch_pool->Free (lightpatches);
  csFrustumView lview;
  csFrustumContext* ctxt = lview.GetFrustumContext ();
  csLightingInfo& linfo = ctxt->GetLightingInfo ();
  linfo.SetGouraudOnly (false);
  linfo.SetColor (GetColor ());
  lview.SetUserData ((void*)this);
  lview.SetNodeFunction (node_light_func);
  lview.SetPolygonFunction (poly_light_func);
  lview.SetCurveFunction (curve_light_func);
  lview.SetRadius (GetRadius ());
  lview.EnableThingShadows (flags.Get () & CS_LIGHT_THINGSHADOWS);
  lview.SetDynamic (true);
  lview.SetShadowMask (CS_ENTITY_NOSHADOWS, 0);
  lview.SetProcessMask (CS_ENTITY_NOLIGHTING, 0);

#if 0
  csLightingPolyTexQueue* lptq = new csLightingPolyTexQueue ();
  //@@@ TWO KINDS OF USERDATA!!!
  lview.SetUserdata (lptq);
#endif

  ctxt->SetLightFrustum (new csFrustum (center));
  ctxt->GetLightFrustum ()->MakeInfinite ();
  sector->CheckFrustum ((iFrustumView*)&lview);

#if 0
  lptq->DecRef ();
#endif
}

void csDynLight::SetColor (const csColor& col)
{
  csLight::SetColor (col);
  csLightPatch* lp = lightpatches;
  while (lp)
  {
    if (lp->GetPolygon ())
      lp->GetPolygon()->MakeDirtyDynamicLights ();
    else
      lp->GetCurve()->MakeDirtyDynamicLights ();

    lp = lp->GetNextLight ();
  }
}

void csDynLight::UnlinkLightpatch (csLightPatch* lp)
{
  lp->RemoveLightList (lightpatches);
}

void csDynLight::AddLightpatch (csLightPatch* lp)
{
  lp->AddLightList (lightpatches);
  lp->SetLight (this);
}

