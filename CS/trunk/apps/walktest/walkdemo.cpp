/*
    Copyright (C) 2000-2001 by Jorrit Tyberghein

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
#include "csqint.h"

#include "csgeom/math3d.h"
#include "csgeom/poly3d.h"
#include "cstool/collider.h"
#include "cstool/cspixmap.h"
#include "csutil/csstring.h"
#include "csutil/flags.h"
#include "csutil/scanstr.h"
#include "iengine/camera.h"
#include "iengine/campos.h"
#include "iengine/light.h"
#include "iengine/material.h"
#include "iengine/movable.h"
#include "iengine/sector.h"
#include "iengine/portal.h"
#include "iengine/sharevar.h"
#include "imesh/objmodel.h"
#include "imap/loader.h"
#include "imesh/particles.h"
#include "imesh/object.h"
#include "imesh/partsys.h"
#include "imesh/sprite3d.h"
#include "isndsys.h"
#include "ivaria/collider.h"
#include "ivaria/engseq.h"
#include "ivaria/reporter.h"
#include "ivaria/view.h"
#include "ivideo/graph3d.h"
#include "ivaria/decal.h"
#include "ivideo/material.h"

#include "bot.h"
#include "command.h"
#include "walktest.h"
#include "splitview.h"
#include "particles.h"

extern WalkTest* Sys;


void RandomColor (float& r, float& g, float& b)
{
  float sig = (float)(900+(rand () % 100))/1000.;
  float sm1= (float)(rand () % 1000)/1000.;
  float sm2 = (float)(rand () % 1000)/1000.;
  switch ((rand ()>>3) % 3)
  {
    case 0: r = sig; g = sm1; b = sm2; break;
    case 1: r = sm1; g = sig; b = sm2; break;
    case 2: r = sm1; g = sm2; b = sig; break;
  }
}

extern iMeshWrapper* add_meshobj (const char* tname, char* sname, iSector* where,
	csVector3 const& pos, float size);
extern void move_mesh (iMeshWrapper* sprite, iSector* where,
	csVector3 const& pos);

//===========================================================================
// Everything for bots.
//===========================================================================

bool do_bots = false;

// Add a bot with some size at the specified position.
void WalkTest::add_bot (float size, iSector* where, csVector3 const& pos,
	float dyn_radius, bool manual)
{
  csRef<iLight> dyn;
  if (dyn_radius)
  {
    float r, g, b;
    RandomColor (r, g, b);
    dyn = Sys->Engine->CreateLight ("",
    	pos, dyn_radius, csColor(r, g, b), CS_LIGHT_DYNAMICTYPE_DYNAMIC);
    where->GetLights ()->Add (dyn);
    Sys->dynamic_lights.Push (dyn);
  }
  iMeshFactoryWrapper* tmpl = Sys->Engine->GetMeshFactories ()
  	->FindByName ("bot");
  if (!tmpl) return;
  csRef<iMeshObject> botmesh (tmpl->GetMeshObjectFactory ()->NewInstance ());
  csRef<iMeshWrapper> botWrapper = Engine->CreateMeshWrapper (botmesh, "bot",
    where);

  //csMatrix3 m; m.Identity (); m = m * size;
  //botWrapper->GetMovable ()->SetTransform (m);
  
  botWrapper->GetMovable ()->UpdateMove ();
  csRef<iSprite3DState> state (scfQueryInterface<iSprite3DState> (botmesh));
  state->SetAction ("default");
  
  Bot* bot = new Bot (Sys->Engine, botWrapper);
  bot->set_bot_move (pos);
  bot->set_bot_sector (where);
  bot->light = dyn;
  if (manual)
    manual_bots.Push (bot);
  else
    bots.Push (bot);
}

void WalkTest::del_bot (bool manual)
{
  if (manual)
  {
    if (manual_bots.GetSize () > 0)
      manual_bots.DeleteIndex (0);
  }
  else
  {
    if (bots.GetSize () > 0)
      bots.DeleteIndex (0);
  }
}

void WalkTest::move_bots (csTicks elapsed_time)
{
  size_t i;
  for (i = 0; i < bots.GetSize (); i++)
  {
    bots[i]->move (elapsed_time);
  }
}

//===========================================================================
// Everything for the missile.
//===========================================================================

#define DYN_TYPE_MISSILE 1
#define DYN_TYPE_RANDOM 2
#define DYN_TYPE_EXPLOSION 3

struct LightStruct
{
  int type;
};

struct MissileStruct
{
  int type;		// type == DYN_TYPE_MISSILE
  csOrthoTransform dir;
  csRef<iMeshWrapper> sprite;
  csRef<iSndSysSource> snd;
  csRef<iSndSysStream> snd_stream;
};

struct ExplosionStruct
{
  int type;		// type == DYN_TYPE_EXPLOSION
  float radius;
  int dir;
};

struct RandomLight
{
  int type;		// type == DYN_TYPE_RANDOM
  float dyn_move_dir;
  float dyn_move;
  float dyn_r1, dyn_g1, dyn_b1;
};

bool HandleDynLight (iLight* dyn, iEngine* engine)
{
  LightStruct* ls = (LightStruct*)(WalkDataObject::GetData(dyn->QueryObject ()));
  switch (ls->type)
  {
    case DYN_TYPE_MISSILE:
    {
      MissileStruct* ms = (MissileStruct*)(WalkDataObject::GetData(
      	dyn->QueryObject ()));
      csVector3 v (0, 0, 2.5);
      csVector3 old = dyn->GetCenter ();
      v = old + ms->dir.GetT2O () * v;
      iSector* s = dyn->GetSector ();
      bool mirror = false;
      csVector3 old_v = v;
      s = s->FollowSegment (ms->dir, v, mirror);
      if (ABS (v.x-old_v.x) > SMALL_EPSILON ||
      	ABS (v.y-old_v.y) > SMALL_EPSILON ||
	ABS (v.z-old_v.z) > SMALL_EPSILON)
      {
        v = old;
        if (ms->sprite)
      	{
          if ((rand () & 0x3) == 1)
	  {
	    int i;
	    if (do_bots)
	      for (i = 0 ; i < 40 ; i++)
            Sys->add_bot (1, dyn->GetSector (), dyn->GetCenter (), 0);
	  }
	  ms->sprite->GetMovable ()->ClearSectors ();
	  Sys->Engine->GetMeshes ()->Remove (ms->sprite);
	}
	csRef<WalkDataObject> ido (
		CS::GetChildObject<WalkDataObject>(dyn->QueryObject()));
        dyn->QueryObject ()->ObjRemove (ido);
        if (ms->snd)
        {
          ms->snd_stream->Pause();
        }
        delete ms;
        if (Sys->mySound)
        {
	  if (Sys->wMissile_boom)
	  {
	    csRef<iSndSysStream> st = Sys->mySound->CreateStream (
		Sys->wMissile_boom->GetData (), CS_SND3D_ABSOLUTE);
	    csRef<iSndSysSource> sndsource = Sys->mySound->
	      	CreateSource (st);
	    if (sndsource)
	    {
	      csRef<iSndSysSource3D> sndsource3d
		= scfQueryInterface<iSndSysSource3D> (sndsource);

	      sndsource3d->SetPosition (v);
	      sndsource->SetVolume (1.0f);
	      st->SetLoopState (CS_SNDSYS_STREAM_DONTLOOP);
	      st->Unpause ();
	    }
	  }
        }
        ExplosionStruct* es = new ExplosionStruct;
        es->type = DYN_TYPE_EXPLOSION;
        es->radius = 2;
        es->dir = 1;
        WalkDataObject* esdata = new WalkDataObject (es);
        dyn->QueryObject ()->ObjAdd (esdata);
	esdata->DecRef ();
	WalkTestParticleDemos::Explosion (Sys, dyn->GetSector (),
		dyn->GetCenter (), "explo");
        return false;
      }
      else ms->dir.SetOrigin (v);
      if (dyn->GetSector () != s)
      {
	dyn->IncRef ();
        dyn->GetSector ()->GetLights ()->Remove (dyn);
        s->GetLights ()->Add (dyn);
	dyn->DecRef ();
      }
      dyn->SetCenter (v);
      if (ms->sprite) move_mesh (ms->sprite, s, v);
      if (Sys->mySound && ms->snd)
      {
	csRef<iSndSysSource3D> sndsource3d
		= scfQueryInterface<iSndSysSource3D> (ms->snd);
	sndsource3d->SetPosition (v);
	ms->snd->SetVolume (1.0f);
      }
      break;
    }
    case DYN_TYPE_EXPLOSION:
    {
      ExplosionStruct* es = (ExplosionStruct*)(WalkDataObject::GetData(
      	dyn->QueryObject ()));
      if (es->dir == 1)
      {
        es->radius += 3;
	if (es->radius > 6) es->dir = -1;
      }
      else
      {
        es->radius -= 2;
	if (es->radius < 1)
	{
	  csRef<WalkDataObject> ido (
		CS::GetChildObject<WalkDataObject>(dyn->QueryObject()));
	  dyn->QueryObject ()->ObjRemove (ido);
	  delete es;
	  dyn->GetSector ()->GetLights ()->Remove (dyn);
	  return true;
	}
      }
      dyn->SetCutoffDistance (es->radius);
      break;
    }
    case DYN_TYPE_RANDOM:
    {
      RandomLight* rl = (RandomLight*)(WalkDataObject::GetData(
      	dyn->QueryObject ()));
      rl->dyn_move += rl->dyn_move_dir;
      if (rl->dyn_move < 0 || rl->dyn_move > 2)
      	rl->dyn_move_dir = -rl->dyn_move_dir;
      if (ABS (rl->dyn_r1-dyn->GetColor ().red) < .01 &&
      	  ABS (rl->dyn_g1-dyn->GetColor ().green) < .01 &&
	  ABS (rl->dyn_b1-dyn->GetColor ().blue) < .01)
        RandomColor (rl->dyn_r1, rl->dyn_g1, rl->dyn_b1);
      else
        dyn->SetColor (csColor ((rl->dyn_r1+7.*dyn->GetColor ().red)/8.,
		(rl->dyn_g1+7.*dyn->GetColor ().green)/8.,
		(rl->dyn_b1+7.*dyn->GetColor ().blue)/8.));
      dyn->SetCenter (dyn->GetCenter () + csVector3 (0, rl->dyn_move_dir, 0));
      break;
    }
  }
  return false;
}

void show_lightning ()
{
  csRef<iEngineSequenceManager> seqmgr(
  	csQueryRegistry<iEngineSequenceManager> (Sys->object_reg));
  if (seqmgr)
  {
    // This finds the light L1 (the colored light over the stairs) and
    // makes the lightning restore this color back after it runs.
    iLight *light = Sys->Engine->FindLight("l1");
    iSharedVariable *var = Sys->Engine->GetVariableList()
    	->FindByName("Lightning Restore Color");
    if (light && var)
    {
      var->SetColor (light->GetColor ());
    }
    seqmgr->RunSequenceByName ("seq_lightning", 0);
  }
  else
  {
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
    	             "Could not find engine sequence manager!");
  }
}

void fire_missile ()
{
  csVector3 dir (0, 0, 0);
  csVector3 pos = Sys->views->GetCamera ()->GetTransform ().This2Other (dir);
  float r, g, b;
  RandomColor (r, g, b);
  csRef<iLight> dyn =
  	Sys->Engine->CreateLight ("", pos, 4, csColor (r, g, b),
		CS_LIGHT_DYNAMICTYPE_DYNAMIC);
  Sys->views->GetCamera ()->GetSector ()->GetLights ()->Add (dyn);
  Sys->dynamic_lights.Push (dyn);

  MissileStruct* ms = new MissileStruct;
  ms->snd = 0;
  if (Sys->mySound)
  {
    iSndSysData* snddata = Sys->wMissile_whoosh->GetData ();
    ms->snd_stream = Sys->mySound->CreateStream (snddata, CS_SND3D_ABSOLUTE);
    ms->snd = Sys->mySound->CreateSource (ms->snd_stream);
    if (ms->snd)
    {
      csRef<iSndSysSource3D> sndsource3d
		= scfQueryInterface<iSndSysSource3D> (ms->snd);

      sndsource3d->SetPosition (pos);
      ms->snd->SetVolume (1.0f);
      ms->snd_stream->Unpause ();
    }
  }
  ms->type = DYN_TYPE_MISSILE;
  ms->dir = (csOrthoTransform)(Sys->views->GetCamera ()->GetTransform ());
  ms->sprite = 0;
  WalkDataObject* msdata = new WalkDataObject(ms);
  dyn->QueryObject ()->ObjAdd(msdata);
  msdata->DecRef ();

  csString misname;
  misname.Format ("missile%d", ((rand () >> 3) & 1)+1);

  iMeshFactoryWrapper *tmpl = Sys->Engine->GetMeshFactories ()
  	->FindByName (misname);
  if (!tmpl)
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
    	"Could not find '%s' sprite factory!", misname.GetData());
  else
  {
    csRef<iMeshWrapper> sp (
    	Sys->Engine->CreateMeshWrapper (tmpl,
	"missile",Sys->views->GetCamera ()->GetSector (), pos));

    ms->sprite = sp;
    csMatrix3 m = ms->dir.GetT2O ();
    sp->GetMovable ()->SetTransform (m);
    sp->GetMovable ()->UpdateMove ();
  }
}

void test_decal ()
{
  csRef<iDecalManager> decalMgr = csLoadPluginCheck<iDecalManager> (
  	Sys->object_reg, "crystalspace.decal.manager");
  if (!decalMgr)
    return;

  iMaterialWrapper * material = 
    Sys->Engine->GetMaterialList()->FindByName("decal");
  if (!material)
  {
    csRef<iLoader> loader = csQueryRegistry<iLoader>(Sys->object_reg);
    if (!loader)
    {
      Sys->Report(CS_REPORTER_SEVERITY_NOTIFY, "Couldn't find iLoader");
      return; 
    }

    if (!loader->LoadTexture ("decal", "/lib/std/cslogo2.png"))
      Sys->Report(CS_REPORTER_SEVERITY_NOTIFY, 
                  "Couldn't load decal texture!");

    material = Sys->Engine->GetMaterialList()->FindByName("decal");
    if (!material)
    {
      Sys->Report(CS_REPORTER_SEVERITY_NOTIFY, 
                  "Error finding decal material");
      return;
    }
  }

  // create a template for our new decal
  csRef<iDecalTemplate> decalTemplate = 
      decalMgr->CreateDecalTemplate(material);
  decalTemplate->SetTimeToLive(5.0f);
  
  csVector3 start = Sys->views->GetCamera()->GetTransform().GetOrigin();

  csVector3 normal = 
    Sys->views->GetCamera()->GetTransform().This2OtherRelative(csVector3(0,0,-1));

  csVector3 end = start - normal * 10000.0f;

  csSectorHitBeamResult result = Sys->views->GetCamera()->GetSector()->HitBeamPortals(start, end);
  if (!result.mesh)
      return;
      
/*
  // figure out the starting point
  csRef<iCollideSystem> cdsys = csQueryRegistry<iCollideSystem>(Sys->object_reg);
  if (!cdsys)
  {
    Sys->Report(CS_REPORTER_SEVERITY_NOTIFY, "Couldn't find iCollideSystem");
    return;
  }


  // intersect with world to get a decal position
  csVector3 iSect;
  csIntersectingTriangle closestTri;
  iMeshWrapper * selMesh;
  if (csColliderHelper::TraceBeam(cdsys, Sys->views->GetCamera()->GetSector(), 
        start, end, true, closestTri, iSect, &selMesh) <= 0.0f)
  {
      printf("No Decal Tracebeam\n");
      return;
  }

  start = iSect;
*/

  // make the up direction of the decal the same as the camera
  csVector3 up =
    Sys->views->GetCamera()->GetTransform().This2OtherRelative(
	csVector3(0,1,0));

  // create the decal
  decalMgr->CreateDecal(decalTemplate, result.final_sector,
	  result.isect, up, normal, 1.0f, 0.5f);
}

void AttachRandomLight (iLight* light)
{
  RandomLight* rl = new RandomLight;
  rl->type = DYN_TYPE_RANDOM;
  rl->dyn_move_dir = 0.2f;
  rl->dyn_move = 0;
  rl->dyn_r1 = rl->dyn_g1 = rl->dyn_b1 = 1;
  WalkDataObject* rldata = new WalkDataObject (rl);
  light->QueryObject ()->ObjAdd (rldata);
  rldata->DecRef ();
}

//===========================================================================

#if 0
static csPtr<iMeshWrapper> CreateMeshWrapper (const char* name)
{
  csRef<iMeshObjectType> ThingType = csLoadPluginCheck<iMeshObjectType> (
  	Sys->object_reg, "crystalspace.mesh.object.thing");
  if (!ThingType)
    return 0;

  csRef<iMeshObjectFactory> thing_fact = ThingType->NewFactory ();
  csRef<iMeshObject> mesh_obj = thing_fact->NewInstance ();

  csRef<iMeshWrapper> mesh_wrap =
  	Sys->Engine->CreateMeshWrapper (mesh_obj, name);
  return csPtr<iMeshWrapper> (mesh_wrap);
}
#endif

#if 0
static csPtr<iMeshWrapper> CreatePortalThing (const char* name, iSector* room,
    	iMaterialWrapper* tm, int& portalPoly)
{
  csRef<iMeshWrapper> thing = CreateMeshWrapper (name);
  csRef<iThingFactoryState> thing_fact_state = 
    scfQueryInterface<iThingFactoryState> (
    thing->GetMeshObject ()->GetFactory());
  thing->GetMovable ()->SetSector (room);
  float dx = 1, dy = 3, dz = 0.3f;
  float border = 0.3f; // width of border around the portal

  // bottom
  thing_fact_state->AddQuad (
    csVector3 (-dx, 0, -dz),
    csVector3 (dx, 0, -dz),
    csVector3 (dx, 0, dz),
    csVector3 (-dx, 0, dz));
  thing_fact_state->SetPolygonTextureMapping (CS_POLYRANGE_LAST,
      csVector2 (0.25, 0.875),
      csVector2 (0.75, 0.875),
      csVector2 (0.75, 0.75));

  // top
  thing_fact_state->AddQuad (
    csVector3 (-dx, dy, dz),
    csVector3 (dx, dy, dz),
    csVector3 (dx, dy, -dz),
    csVector3 (-dx, dy, -dz));
  thing_fact_state->SetPolygonTextureMapping (CS_POLYRANGE_LAST,
      csVector2 (0.25, 0.25),
      csVector2 (0.75, 0.25),
      csVector2 (0.75, 0.125));

  // back
  thing_fact_state->AddQuad (
    csVector3 (-dx, 0, dz),
    csVector3 (dx, 0, dz),
    csVector3 (dx, dy, dz),
    csVector3 (-dx, dy, dz));
  thing_fact_state->SetPolygonTextureMapping (CS_POLYRANGE_LAST,
      csVector2 (0.25, 0.75),
      csVector2 (0.75, 0.75),
      csVector2 (0.75, 0.25));

  // right
  thing_fact_state->AddQuad (
    csVector3 (dx, 0, dz),
    csVector3 (dx, 0, -dz),
    csVector3 (dx, dy, -dz),
    csVector3 (dx, dy, dz));
  thing_fact_state->SetPolygonTextureMapping (CS_POLYRANGE_LAST,
      csVector2 (0.75, 0.75),
      csVector2 (0.875, 0.75),
      csVector2 (0.875, 0.25));

  // left
  thing_fact_state->AddQuad (
    csVector3 (-dx, 0, -dz),
    csVector3 (-dx, 0, dz),
    csVector3 (-dx, dy, dz),
    csVector3 (-dx, dy, -dz));
  thing_fact_state->SetPolygonTextureMapping (CS_POLYRANGE_LAST,
      csVector2 (0.125, 0.75),
      csVector2 (0.25, 0.75),
      csVector2 (0.25, 0.25));

  // front border
  // border top
  thing_fact_state->AddQuad (
    csVector3 (-dx+border, dy, -dz),
    csVector3 (dx-border, dy, -dz),
    csVector3 (dx-border, dy-border, -dz),
    csVector3 (-dx+border, dy-border, -dz));
  thing_fact_state->SetPolygonTextureMapping (CS_POLYRANGE_LAST,
      csVector2 (0.125, 0.125),
      csVector2 (0.875, 0.125),
      csVector2 (0.875, 0.0));
  // border right
  thing_fact_state->AddQuad (
    csVector3 (dx-border, dy-border, -dz),
    csVector3 (dx, dy-border, -dz),
    csVector3 (dx, border, -dz),
    csVector3 (dx-border, border, -dz));
  thing_fact_state->SetPolygonTextureMapping (CS_POLYRANGE_LAST,
      csVector2 (1.0, 0.125),
      csVector2 (0.875, 0.125),
      csVector2 (0.875, 0.875));
  // border bottom
  thing_fact_state->AddQuad (
    csVector3 (-dx+border, border, -dz),
    csVector3 (+dx-border, border, -dz),
    csVector3 (+dx-border, 0.0, -dz),
    csVector3 (-dx+border, 0.0, -dz));
  thing_fact_state->SetPolygonTextureMapping (CS_POLYRANGE_LAST,
      csVector2 (0.125, 1.0),
      csVector2 (0.875, 1.0),
      csVector2 (0.875, 0.875));
  // border left
  thing_fact_state->AddQuad (
    csVector3 (-dx, dy-border, -dz),
    csVector3 (-dx+border, dy-border, -dz),
    csVector3 (-dx+border, border, -dz),
    csVector3 (-dx, border, -dz));
  thing_fact_state->SetPolygonTextureMapping (CS_POLYRANGE_LAST,
      csVector2 (0.125, 0.125),
      csVector2 (0.0, 0.125),
      csVector2 (0.0, 0.875));
  // border topleft
  thing_fact_state->AddQuad (
    csVector3 (-dx, dy, -dz),
    csVector3 (-dx+border, dy, -dz),
    csVector3 (-dx+border, dy-border, -dz),
    csVector3 (-dx, dy-border, -dz));
  thing_fact_state->SetPolygonTextureMapping (CS_POLYRANGE_LAST,
      csVector2 (0.125, 0.125),
      csVector2 (0.0, 0.125),
      csVector2 (0.0, 0.0));
  // border topright
  thing_fact_state->AddQuad (
    csVector3 (dx-border, dy, -dz),
    csVector3 (dx, dy, -dz),
    csVector3 (dx, dy-border, -dz),
    csVector3 (dx-border, dy-border, -dz));
  thing_fact_state->SetPolygonTextureMapping (CS_POLYRANGE_LAST,
      csVector2 (1.0, 0.125),
      csVector2 (0.875, 0.125),
      csVector2 (0.875, 0.0));
  // border botright
  thing_fact_state->AddQuad (
    csVector3 (dx-border, border, -dz),
    csVector3 (dx, border, -dz),
    csVector3 (dx, 0.0, -dz),
    csVector3 (dx-border, 0.0, -dz));
  thing_fact_state->SetPolygonTextureMapping (CS_POLYRANGE_LAST,
      csVector2 (1.0, 1.0),
      csVector2 (0.875, 1.0),
      csVector2 (0.875, 0.875));
  // border botleft
  thing_fact_state->AddQuad (
    csVector3 (-dx, border, -dz),
    csVector3 (-dx+border, border, -dz),
    csVector3 (-dx+border, 0.0, -dz),
    csVector3 (-dx, 0.0, -dz));
  thing_fact_state->SetPolygonTextureMapping (CS_POLYRANGE_LAST,
      csVector2 (0.125, 1.0),
      csVector2 (0.0, 1.0),
      csVector2 (0.0, 0.875));

  // front - the portal
  portalPoly = thing_fact_state->AddQuad (
    csVector3 (dx-border, border, -dz),
    csVector3 (-dx+border, border, -dz),
    csVector3 (-dx+border, dy-border, -dz),
    csVector3 (dx-border, dy-border, -dz));
  thing_fact_state->SetPolygonTextureMapping (CS_POLYRANGE_LAST,
      csVector2 (0, 0),
      csVector2 (1, 0),
      csVector2 (1, 1));

  thing_fact_state->SetPolygonMaterial (CS_POLYRANGE_ALL, tm);
  thing_fact_state->SetPolygonFlags (CS_POLYRANGE_ALL, CS_POLY_COLLDET);

  csRef<iLightingInfo> linfo (
    scfQueryInterface<iLightingInfo> (thing->GetMeshObject ()));
  linfo->InitializeDefault (true);
  room->ShineLights (thing);
  linfo->PrepareLighting ();

  return csPtr<iMeshWrapper> (thing);
}
#endif

void OpenPortal (iView* view, char* lev)
{
#if 0
  iSector* room = view->GetCamera ()->GetSector ();
  csVector3 pos = view->GetCamera ()->GetTransform ().This2Other (
  	csVector3 (0, 0, 1));
  iMaterialWrapper* tm = Sys->Engine->GetMaterialList ()->
  	FindByName ("portal");

  int portalPoly;
  csRef<iMeshWrapper> thing = CreatePortalThing ("portalTo", room, tm,
  	portalPoly);
  csRef<iThingFactoryState> thing_fact_state = 
    scfQueryInterface<iThingFactoryState> (
    thing->GetMeshObject ()->GetFactory());

  bool collectionExists = (Sys->Engine->GetCollection (lev) != 0);
  iCollection* collection = Sys->Engine->CreateCollection (lev);
  // If the collection did not already exist then we load the level in it.
  if (!collectionExists)
  {
    // @@@ No error checking!
    csString buf;
    buf.Format ("/lev/%s", lev);
    Sys->myVFS->ChDir (buf);

    if(Sys->threaded)
    {
      Sys->TLevelLoader->LoadMapFile ("world", false, collection);
    }
    else
    {
      Sys->LevelLoader->LoadMapFile ("world", false, collection);
    }
  }

  iMovable* tmov = thing->GetMovable ();
  tmov->SetPosition (pos + csVector3 (0, Sys->cfg_legs_offset, 0));
  tmov->Transform (view->GetCamera ()->GetTransform ().GetT2O ());
  tmov->UpdateMove ();

  // First make a portal to the new level.
  iCameraPosition* cp = collection->FindCameraPosition ("Start");
  const char* room_name;
  csVector3 topos;
  if (cp) { room_name = cp->GetSector (); topos = cp->GetPosition (); }
  else { room_name = "room"; topos.Set (0, 0, 0); }
  topos.y -= Sys->cfg_eye_offset;
  iSector* start_sector = collection->FindSector (room_name);
  if (start_sector)
  {
    csPoly3D poly;
    poly.AddVertex (thing_fact_state->GetPolygonVertex (portalPoly, 0));
    poly.AddVertex (thing_fact_state->GetPolygonVertex (portalPoly, 1));
    poly.AddVertex (thing_fact_state->GetPolygonVertex (portalPoly, 2));
    poly.AddVertex (thing_fact_state->GetPolygonVertex (portalPoly, 3));
    iPortal* portal;
    csRef<iMeshWrapper> portalMesh = Sys->Engine->CreatePortal (
    	"new_portal", tmov->GetSectors ()->Get (0), csVector3 (0),
	start_sector, poly.GetVertices (), (int)poly.GetVertexCount (),
	portal);
    //iPortal* portal = portalPoly->CreatePortal (start_sector);
    portal->GetFlags ().Set (CS_PORTAL_ZFILL);
    portal->GetFlags ().Set (CS_PORTAL_CLIPDEST);
    portal->SetWarp (view->GetCamera ()->GetTransform ().GetT2O (), topos, pos);

    if (!collectionExists)
    {
      // Only if the collection did not already exist do we create a portal
      // back. So even if multiple portals go to the collection we only have
      // one portal back.
      int portalPolyBack;
      csRef<iMeshWrapper> thingBack = CreatePortalThing ("portalFrom",
	  	start_sector, tm, portalPolyBack);
      csRef<iThingFactoryState> thing_fact_state = 
	scfQueryInterface<iThingFactoryState> (
	thingBack->GetMeshObject ()->GetFactory());
      iMovable* tbmov = thingBack->GetMovable ();
      tbmov->SetPosition (topos + csVector3 (0, Sys->cfg_legs_offset, -0.1f));
      tbmov->Transform (csYRotMatrix3 (PI));//view->GetCamera ()->GetW2C ());
      tbmov->UpdateMove ();
      iPortal* portalBack;
      csPoly3D polyBack;
      polyBack.AddVertex (thing_fact_state->GetPolygonVertex (portalPoly, 0));
      polyBack.AddVertex (thing_fact_state->GetPolygonVertex (portalPoly, 1));
      polyBack.AddVertex (thing_fact_state->GetPolygonVertex (portalPoly, 2));
      polyBack.AddVertex (thing_fact_state->GetPolygonVertex (portalPoly, 3));
      csRef<iMeshWrapper> portalMeshBack = Sys->Engine->CreatePortal (
    	  "new_portal_back", tbmov->GetSectors ()->Get (0), csVector3 (0),
	  tmov->GetSectors ()->Get (0), polyBack.GetVertices (),
	  (int)polyBack.GetVertexCount (),
	  portalBack);
      //iPortal* portalBack = portalPolyBack->CreatePortal (room);
      portalBack->GetFlags ().Set (CS_PORTAL_ZFILL);
      portalBack->GetFlags ().Set (CS_PORTAL_CLIPDEST);
      portalBack->SetWarp (view->GetCamera ()->GetTransform ().GetO2T (),
      	-pos, -topos);
    }
  }

  if (!collectionExists)
    Sys->InitCollDet (Sys->Engine, collection);
#endif
}

