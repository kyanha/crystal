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
#include "walktest.h"
#include "bot.h"
#include "infmaze.h"
#include "hugeroom.h"
#include "command.h"
#include "csengine/dumper.h"
#include "csengine/camera.h"
#include "csengine/campos.h"
#include "csengine/octree.h"
#include "csengine/engine.h"
#include "csengine/csview.h"
#include "csengine/wirefrm.h"
#include "csengine/meshobj.h"
#include "csengine/polygon.h"
#include "csengine/light.h"
#include "csengine/sector.h"
#include "csengine/thing.h"
#include "csengine/collider.h"
#include "csengine/meshobj.h"
#include "csengine/region.h"
#include "csutil/scanstr.h"
#include "csparser/impexp.h"
#include "csobject/dataobj.h"
#include "csparser/snddatao.h"
#include "csparser/csloader.h"
#include "csparser/crossbld.h"
#include "csgeom/math3d.h"
#include "cssys/system.h"
#include "csfx/cspixmap.h"
#include "qint.h"
#include "isnddata.h"
#include "isndsrc.h"
#include "isndlstn.h"
#include "isndsrc.h"
#include "isndrdr.h"
#include "igraph3d.h"
#include "icollide.h"
#include "impartic.h"
#include "imfount.h"
#include "imexplo.h"
#include "imfire.h"
#include "imsnow.h"
#include "imrain.h"
#include "imspiral.h"
#include "imspr3d.h"
#include "iskel.h"

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

extern iMeshWrapper* add_meshobj (char* tname, char* sname, csSector* where,
	csVector3 const& pos, float size);
extern void move_sprite (csSprite* sprite, csSector* where,
	csVector3 const& pos);

//===========================================================================
// Demo particle system (rain).
//===========================================================================
void add_particles_rain (csSector* sector, char* matname, int num, float speed)
{
  // First check if the material exists.
  csMaterialWrapper* mat = Sys->view->GetEngine ()->GetMaterials ()->
  	FindByName (matname);
  if (!mat)
  {
    Sys->Printf (MSG_CONSOLE, "Can't find material '%s' in memory!\n", matname);
    return;
  }

  csPolygonSetBBox* pset_bbox = sector->GetBoundingBox ();
  csPolygonSet* pset = sector;
  if (!pset_bbox)
  {
    sector->CreateBoundingBox ();
    pset_bbox = sector->GetBoundingBox ();
    if (!pset_bbox)
    {
      // If there still is no bbox we calculate one for the static
      // thing inside.
      csThing* thing = sector->GetStaticThing ();
      pset = thing;
      if (thing)
      {
        pset_bbox = thing->GetBoundingBox ();
	if (!pset_bbox)
	{
	  thing->CreateBoundingBox ();
          pset_bbox = thing->GetBoundingBox ();
	}
      }
    }
  }
  if (pset_bbox)
  {
    csBox3 bbox;
    bbox.StartBoundingBox (pset->Vwor (pset_bbox->i1));
    bbox.AddBoundingVertexSmart (pset->Vwor (pset_bbox->i2));
    bbox.AddBoundingVertexSmart (pset->Vwor (pset_bbox->i3));
    bbox.AddBoundingVertexSmart (pset->Vwor (pset_bbox->i4));
    bbox.AddBoundingVertexSmart (pset->Vwor (pset_bbox->i5));
    bbox.AddBoundingVertexSmart (pset->Vwor (pset_bbox->i6));
    bbox.AddBoundingVertexSmart (pset->Vwor (pset_bbox->i7));
    bbox.AddBoundingVertexSmart (pset->Vwor (pset_bbox->i8));

    // @@@ Memory leak on factories!
    iMeshObjectType* type = QUERY_PLUGIN_CLASS (System, "crystalspace.mesh.object.rain",
      	  "MeshObj", iMeshObjectType);
    if (!type) type = LOAD_PLUGIN (System, "crystalspace.mesh.object.rain",
      	  "MeshObj", iMeshObjectType);
    iMeshObjectFactory* factory = type->NewFactory ();
    iMeshObject* mesh = factory->NewInstance ();
    csMeshWrapper* exp = new csMeshWrapper (Sys->view->GetEngine (), mesh);
    mesh->DecRef ();
    Sys->view->GetEngine ()->sprites.Push (exp);
    exp->GetMovable ().SetSector (sector);
    exp->GetMovable ().UpdateMove ();

    iParticleState* partstate = QUERY_INTERFACE (mesh, iParticleState);
    partstate->SetMaterialWrapper (QUERY_INTERFACE (mat, iMaterialWrapper));
    partstate->SetMixMode (CS_FX_ADD);
    partstate->SetColor (csColor (.25,.25,.25));

    iRainState* rainstate = QUERY_INTERFACE (mesh, iRainState);
    rainstate->SetNumberParticles (num);
    rainstate->SetDropSize (.3/50., .3);
    rainstate->SetLighting (false);
    rainstate->SetBox (bbox.Min (), bbox.Max ());
    rainstate->SetFallSpeed (csVector3 (0, -speed, 0));
  }
}

//===========================================================================
// Demo particle system (snow).
//===========================================================================
void add_particles_snow (csSector* sector, char* matname, int num, float speed)
{
  // First check if the material exists.
  csMaterialWrapper* mat = Sys->view->GetEngine ()->GetMaterials ()->
  	FindByName (matname);
  if (!mat)
  {
    Sys->Printf (MSG_CONSOLE, "Can't find material '%s' in memory!\n", matname);
    return;
  }

  csPolygonSetBBox* pset_bbox = sector->GetBoundingBox ();
  csPolygonSet* pset = sector;
  if (!pset_bbox)
  {
    sector->CreateBoundingBox ();
    pset_bbox = sector->GetBoundingBox ();
    if (!pset_bbox)
    {
      // If there still is no bbox we calculate one for the static
      // thing inside.
      csThing* thing = sector->GetStaticThing ();
      pset = thing;
      if (thing)
      {
        pset_bbox = thing->GetBoundingBox ();
	if (!pset_bbox)
	{
	  thing->CreateBoundingBox ();
          pset_bbox = thing->GetBoundingBox ();
	}
      }
    }
  }
  if (pset_bbox)
  {
    csBox3 bbox;
    bbox.StartBoundingBox (pset->Vwor (pset_bbox->i1));
    bbox.AddBoundingVertexSmart (pset->Vwor (pset_bbox->i2));
    bbox.AddBoundingVertexSmart (pset->Vwor (pset_bbox->i3));
    bbox.AddBoundingVertexSmart (pset->Vwor (pset_bbox->i4));
    bbox.AddBoundingVertexSmart (pset->Vwor (pset_bbox->i5));
    bbox.AddBoundingVertexSmart (pset->Vwor (pset_bbox->i6));
    bbox.AddBoundingVertexSmart (pset->Vwor (pset_bbox->i7));
    bbox.AddBoundingVertexSmart (pset->Vwor (pset_bbox->i8));

    // @@@ Memory leak on factories!
    iMeshObjectType* type = QUERY_PLUGIN_CLASS (System, "crystalspace.mesh.object.snow",
      	  "MeshObj", iMeshObjectType);
    if (!type) type = LOAD_PLUGIN (System, "crystalspace.mesh.object.snow",
      	  "MeshObj", iMeshObjectType);
    iMeshObjectFactory* factory = type->NewFactory ();
    iMeshObject* mesh = factory->NewInstance ();
    csMeshWrapper* exp = new csMeshWrapper (Sys->view->GetEngine (), mesh);
    mesh->DecRef ();
    Sys->view->GetEngine ()->sprites.Push (exp);
    exp->GetMovable ().SetSector (sector);
    exp->GetMovable ().UpdateMove ();

    iParticleState* partstate = QUERY_INTERFACE (mesh, iParticleState);
    partstate->SetMaterialWrapper (QUERY_INTERFACE (mat, iMaterialWrapper));
    partstate->SetMixMode (CS_FX_ADD);
    partstate->SetColor (csColor (.25,.25,.25));

    iSnowState* snowstate = QUERY_INTERFACE (mesh, iSnowState);
    snowstate->SetNumberParticles (num);
    snowstate->SetDropSize (.07, .07);
    snowstate->SetLighting (false);
    snowstate->SetBox (bbox.Min (), bbox.Max ());
    snowstate->SetFallSpeed (csVector3 (0, -speed, 0));
    snowstate->SetSwirl (0.2);
  }
}

//===========================================================================
// Demo particle system (fire).
//===========================================================================
void add_particles_fire (csSector* sector, char* matname, int num,
	const csVector3& origin)
{
  // First check if the material exists.
  csMaterialWrapper* mat = Sys->view->GetEngine ()->GetMaterials ()->
  	FindByName (matname);
  if (!mat)
  {
    Sys->Printf (MSG_CONSOLE, "Can't find material '%s' in memory!\n", matname);
    return;
  }

  // @@@ Memory leak on factories!
  iMeshObjectType* type = QUERY_PLUGIN_CLASS (System, "crystalspace.mesh.object.fire",
      	"MeshObj", iMeshObjectType);
  if (!type) type = LOAD_PLUGIN (System, "crystalspace.mesh.object.fire",
      	"MeshObj", iMeshObjectType);
  iMeshObjectFactory* factory = type->NewFactory ();
  iMeshObject* mesh = factory->NewInstance ();
  csMeshWrapper* exp = new csMeshWrapper (Sys->view->GetEngine (), mesh);
  mesh->DecRef ();
  Sys->view->GetEngine ()->sprites.Push (exp);
  exp->GetMovable ().SetSector (sector);
  exp->GetMovable ().UpdateMove ();

  iParticleState* partstate = QUERY_INTERFACE (mesh, iParticleState);
  partstate->SetMaterialWrapper (QUERY_INTERFACE (mat, iMaterialWrapper));
  partstate->SetMixMode (CS_FX_ADD);

  iFireState* firestate = QUERY_INTERFACE (mesh, iFireState);
  firestate->SetNumberParticles (num);
  firestate->SetDropSize (.02, .04);
  firestate->SetLighting (false);
  firestate->SetOrigin (origin);
  firestate->SetDirection (csVector3 (0, 1., 0));
  firestate->SetSwirl (0.6);
  firestate->SetColorScale (0.2);
}

//===========================================================================
// Demo particle system (fountain).
//===========================================================================
void add_particles_fountain (csSector* sector, char* matname, int num,
	const csVector3& origin)
{
  // First check if the material exists.
  csMaterialWrapper* mat = Sys->view->GetEngine ()->GetMaterials ()->
  	FindByName (matname);
  if (!mat)
  {
    Sys->Printf (MSG_CONSOLE, "Can't find material '%s' in memory!\n", matname);
    return;
  }

  // @@@ Memory leak on factories!
  iMeshObjectType* type = QUERY_PLUGIN_CLASS (System, "crystalspace.mesh.object.fountain",
      	"MeshObj", iMeshObjectType);
  if (!type) type = LOAD_PLUGIN (System, "crystalspace.mesh.object.fountain",
      	"MeshObj", iMeshObjectType);
  iMeshObjectFactory* factory = type->NewFactory ();
  iMeshObject* mesh = factory->NewInstance ();
  csMeshWrapper* exp = new csMeshWrapper (Sys->view->GetEngine (), mesh);
  mesh->DecRef ();
  Sys->view->GetEngine ()->sprites.Push (exp);
  exp->GetMovable ().SetSector (sector);
  exp->GetMovable ().SetPosition (origin);
  exp->GetMovable ().UpdateMove ();

  iParticleState* partstate = QUERY_INTERFACE (mesh, iParticleState);
  partstate->SetMaterialWrapper (QUERY_INTERFACE (mat, iMaterialWrapper));
  partstate->SetMixMode (CS_FX_ADD);
  partstate->SetColor (csColor (.25, .35, .55));
  partstate->SetChangeRotation (7.5);

  iFountainState* fountstate = QUERY_INTERFACE (mesh, iFountainState);
  fountstate->SetNumberParticles (num);
  fountstate->SetDropSize (.1, .1);
  fountstate->SetOrigin (csVector3 (0, 0, 0));
  fountstate->SetAcceleration (csVector3 (0, -1., 0));
  fountstate->SetFallTime (5.0);
  fountstate->SetSpeed (3.0);
  fountstate->SetElevation (3.1415926/2.);
  fountstate->SetAzimuth (0);
  fountstate->SetOpening (.2);
}

//===========================================================================
// Demo particle system (explosion).
//===========================================================================
void add_particles_explosion (csSector* sector, const csVector3& center, char* matname)
{
  // First check if the material exists.
  csMaterialWrapper* mat = Sys->view->GetEngine ()->GetMaterials ()->
  	FindByName (matname);
  if (!mat)
  {
    Sys->Printf (MSG_CONSOLE, "Can't find material '%s' in memory!\n", matname);
    return;
  }

  // @@@ Memory leak on factories!
  iMeshObjectType* type = QUERY_PLUGIN_CLASS (System, "crystalspace.mesh.object.explosion",
      	"MeshObj", iMeshObjectType);
  if (!type) type = LOAD_PLUGIN (System, "crystalspace.mesh.object.explosion",
      	"MeshObj", iMeshObjectType);
  iMeshObjectFactory* factory = type->NewFactory ();
  iMeshObject* mesh = factory->NewInstance ();
  csMeshWrapper* exp = new csMeshWrapper (Sys->view->GetEngine (), mesh);
  mesh->DecRef ();
  Sys->view->GetEngine ()->sprites.Push (exp);
  exp->GetMovable ().SetSector (sector);
  exp->GetMovable ().SetPosition (center);
  exp->GetMovable ().UpdateMove ();

  iParticleState* partstate = QUERY_INTERFACE (mesh, iParticleState);
  partstate->SetMaterialWrapper (QUERY_INTERFACE (mat, iMaterialWrapper));
  partstate->SetMixMode (CS_FX_SETALPHA (0.50));
  partstate->SetColor (csColor (1, 1, 0));
  partstate->SetChangeRotation (5.0);
  partstate->SetChangeSize (1.25);
  partstate->SetSelfDestruct (3000);

  iExplosionState* expstate = QUERY_INTERFACE (mesh, iExplosionState);
  expstate->SetNumberParticles (100);
  expstate->SetCenter (csVector3 (0, 0, 0));
  expstate->SetPush (csVector3 (0, 0, 0));
  expstate->SetNrSides (6);
  expstate->SetPartRadius (0.15);
  expstate->SetLighting (true);
  expstate->SetSpreadPos (.6);
  expstate->SetSpreadSpeed (2.);
  expstate->SetSpreadAcceleration (2.);
  expstate->SetFadeSprites (500);
  expstate->AddLight (QUERY_INTERFACE (Sys->engine, iEngine),
      	QUERY_INTERFACE (sector, iSector), 1000);
}

//===========================================================================
// Demo particle system (spiral).
//===========================================================================
void add_particles_spiral (csSector* sector, const csVector3& bottom, char* matname)
{
  // First check if the material exists.
  csMaterialWrapper* mat = Sys->view->GetEngine ()->GetMaterials ()->
  	FindByName (matname);
  if (!mat)
  {
    Sys->Printf (MSG_CONSOLE, "Can't find material '%s' in memory!\n", matname);
    return;
  }

  // @@@ Memory leak on factories!
  iMeshObjectType* type = QUERY_PLUGIN_CLASS (System, "crystalspace.mesh.object.spiral",
      	"MeshObj", iMeshObjectType);
  if (!type) type = LOAD_PLUGIN (System, "crystalspace.mesh.object.spiral",
      	"MeshObj", iMeshObjectType);
  iMeshObjectFactory* factory = type->NewFactory ();
  iMeshObject* mesh = factory->NewInstance ();
  csMeshWrapper* exp = new csMeshWrapper (Sys->view->GetEngine (), mesh);
  mesh->DecRef ();
  Sys->view->GetEngine ()->sprites.Push (exp);
  exp->GetMovable ().SetSector (sector);
  exp->GetMovable ().SetPosition (bottom);
  exp->GetMovable ().UpdateMove ();

  iParticleState* partstate = QUERY_INTERFACE (mesh, iParticleState);
  partstate->SetMaterialWrapper (QUERY_INTERFACE (mat, iMaterialWrapper));
  partstate->SetMixMode (CS_FX_SETALPHA (0.50));
  partstate->SetColor (csColor (1, 1, 0));
  partstate->SetChangeColor (csColor(+0.01,0.,-0.012));

  iSpiralState* spirstate = QUERY_INTERFACE (mesh, iSpiralState);
  spirstate->SetNumberParticles (500);
  spirstate->SetSource (csVector3 (0, 0, 0));
}

//===========================================================================
// Everything for skeletal tree demo.
//===========================================================================

// Recursive function to add limbs to a skeletal tree. This also builds
// the sprite template.
void add_tree_limbs (iSprite3DFactoryState* state, iSpriteFrame* frame,
	iSkeletonLimb* parent, int& vertex_idx,
	int prev_par_idx, int maxdepth, int width, int recursion)
{
  int par_vertex_idx = vertex_idx;
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);

  state->AddVertices (6);

  int anm_idx = frame->GetAnmIndex ();
  int tex_idx = frame->GetTexIndex ();

  state->GetVertex (anm_idx, par_vertex_idx+0) = csVector3(-.05, 0, -.05);
  state->GetVertex (anm_idx, par_vertex_idx+1) = csVector3(.05, 0, -.05);
  state->GetVertex (anm_idx, par_vertex_idx+2) = csVector3(0, 0, .05);
  state->GetVertex (anm_idx, par_vertex_idx+3) = csVector3(-.05, .45, -.05);
  state->GetVertex (anm_idx, par_vertex_idx+4) = csVector3(.05, .45, -.05);
  state->GetVertex (anm_idx, par_vertex_idx+5) = csVector3(0, .45, .05);

  state->GetTexel (tex_idx, par_vertex_idx+0) = csVector2(0, 0);
  state->GetTexel (tex_idx, par_vertex_idx+1) = csVector2(.99, 0);
  state->GetTexel (tex_idx, par_vertex_idx+2) = csVector2(0, .99);
  state->GetTexel (tex_idx, par_vertex_idx+3) = csVector2(.99, .99);
  state->GetTexel (tex_idx, par_vertex_idx+4) = csVector2(.5, .5);
  state->GetTexel (tex_idx, par_vertex_idx+5) = csVector2(.5, 0);

  if (recursion > 0)
  {
    // Create connection triangles with previous set
    state->AddTriangle (prev_par_idx+3, prev_par_idx+5, par_vertex_idx+0);
    state->AddTriangle (prev_par_idx+5, par_vertex_idx+2, par_vertex_idx+0);
    state->AddTriangle (prev_par_idx+4, par_vertex_idx+1, par_vertex_idx+2);
    state->AddTriangle (prev_par_idx+5, prev_par_idx+4, par_vertex_idx+2);
    state->AddTriangle (prev_par_idx+4, par_vertex_idx+0, par_vertex_idx+1);
    state->AddTriangle (prev_par_idx+4, prev_par_idx+3, par_vertex_idx+0);
  }
  // Create base triangles
  state->AddTriangle (par_vertex_idx+0, par_vertex_idx+5, par_vertex_idx+3);
  state->AddTriangle (par_vertex_idx+0, par_vertex_idx+2, par_vertex_idx+5);
  state->AddTriangle (par_vertex_idx+2, par_vertex_idx+4, par_vertex_idx+5);
  state->AddTriangle (par_vertex_idx+2, par_vertex_idx+1, par_vertex_idx+4);
  state->AddTriangle (par_vertex_idx+1, par_vertex_idx+3, par_vertex_idx+4);
  state->AddTriangle (par_vertex_idx+1, par_vertex_idx+0, par_vertex_idx+3);

  if (recursion >= maxdepth) return;
  iSkeletonConnection* con;
  int i;
  int rwidth;
  if (width < 0)
    rwidth = 1 + ((rand () >> 3) % (-width));
  else rwidth = width;

  for (i = 0 ; i < rwidth ; i++)
  {
    con = parent->CreateConnection ();
    csMatrix3 tr = csYRotMatrix3 (0) * csZRotMatrix3(.15) *
                                                 csXRotMatrix3(.15);
    csTransform trans (tr, -tr.GetInverse () * csVector3 (0, .5, 0));
    con->SetTransformation (trans);
    add_tree_limbs (state, frame, QUERY_INTERFACE (con, iSkeletonLimb),
    	vertex_idx, par_vertex_idx, maxdepth, width, recursion+1);
  }
}

// Create a skeletal tree.
iSkeleton* create_skeltree (iSprite3DFactoryState* state, iSpriteFrame* frame,
	int& vertex_idx, int maxdepth, int width)
{
  state->EnableSkeletalAnimation ();
  iSkeleton* skel = state->GetSkeleton ();
  add_tree_limbs (state, frame, QUERY_INTERFACE (skel, iSkeletonLimb),
  	vertex_idx, 0, maxdepth, width, 0);
  return skel;
}

// Object added to every skeletal tree node to keep the animation
// information.
class TreeSkelSpriteInfo
{
public:
  float z_angle_base;
  float z_angle;
  float x_angle_base;
  float x_angle;
  float y_angle;
  float dx;
  float dz;
  float dy;
};

// Animate a skeleton.
void animate_skeleton_tree (iSkeletonLimbState* limb)
{
  iSkeletonLimbState* child = limb->GetChildren ();
  while (child)
  {
    TreeSkelSpriteInfo* o = (TreeSkelSpriteInfo*)child->GetUserData ();
    if (!o)
    {
      o = new TreeSkelSpriteInfo ();
      if ((rand () >> 3) & 0x1)
      {
        o->x_angle_base = (((float)((rand () >> 3)&0xff)) / 255.) * .4 -.2;
        o->x_angle = 0;
        o->dx = (rand () & 0x4) ? .005 : -.005;
        o->z_angle_base = (((float)((rand () >> 3)&0xff)) / 255.) * 1.2 -.6;
        o->z_angle = 0;
        o->dz = (rand () & 0x4) ? .02 : -.02;
      }
      else
      {
        o->z_angle_base = (((float)((rand () >> 3)&0xff)) / 255.) * .4 -.2;
        o->z_angle = 0;
        o->dz = (rand () & 0x4) ? .005 : -.005;
        o->x_angle_base = (((float)((rand () >> 3)&0xff)) / 255.) * 1.2 -.6;
        o->x_angle = 0;
        o->dx = (rand () & 0x4) ? .02 : -.02;
      }
      o->y_angle = 0;
      o->dy = (rand () & 0x4) ? .04 : -.04;
      child->SetUserData (o);
    }
    o->x_angle += o->dx;
    if (o->x_angle > .1 || o->x_angle < -.1) o->dx = -o->dx;
    o->z_angle += o->dz;
    if (o->z_angle > .1 || o->z_angle < -.1) o->dz = -o->dz;
    o->y_angle += o->dy;
    if (o->y_angle > .3 || o->y_angle < -.3) o->dy = -o->dy;

    // @@@ Don't use the code below in a real-time environment.
    // This is only demo code and HIGHLY inefficient.
    csMatrix3 tr = csYRotMatrix3 (o->y_angle) *
    	csZRotMatrix3 (o->z_angle + o->z_angle_base) *
	csXRotMatrix3 (o->x_angle + o->x_angle_base);
    csTransform trans (tr, -tr.GetInverse () * csVector3 (0, .5, 0));
    iSkeletonConnectionState* con = QUERY_INTERFACE (child, iSkeletonConnectionState);
    con->SetTransformation (trans);
    animate_skeleton_tree (child);
    child = child->GetNextSibling ();
  }
}

void animate_skeleton_tree_cb (iMeshWrapper* spr, iRenderView* /*rview*/, void* /*data*/)
{
  iMeshObject* obj = spr->GetMeshObject ();
  iSprite3DState* state = QUERY_INTERFACE (obj, iSprite3DState);
  iSkeletonState* sk_state = state->GetSkeletonState ();
  animate_skeleton_tree (QUERY_INTERFACE (sk_state, iSkeletonLimbState));
}

// Add a skeletal tree sprite. If needed it will also create
// the template for this.
void add_skeleton_tree (csSector* where, csVector3 const& pos, int depth,
	int width)
{
  char skelname[50];
  sprintf (skelname, "__skeltree__%d,%d\n", depth, width);
  iMeshFactoryWrapper* tmpl = Sys->engine->FindMeshFactory (skelname);
  if (!tmpl)
  {
    tmpl = Sys->engine->CreateMeshFactory (
    	"crystalspace.mesh.object.sprite.3d", skelname);
    if (tmpl == NULL)
    {
      CsPrintf (MSG_WARNING, "Could not load the sprite 3d plugin!\n");
      return;
    }
    iMeshObjectFactory* fact = tmpl->GetMeshObjectFactory ();
    iSprite3DFactoryState* state = QUERY_INTERFACE (fact, iSprite3DFactoryState);
    state->SetMaterialWrapper (Sys->engine->FindMaterial ("white"));
    int vertex_idx = 0;
    iSpriteFrame* fr = state->AddFrame ();
    fr->SetName ("f");
    iSpriteAction* act = state->AddAction ();
    act->SetName ("a");
    act->AddFrame (fr, 100);
    create_skeltree (state, fr, vertex_idx, depth, width);
  }
  iMeshWrapper* spr = add_meshobj (skelname, "__skeltree__", where, pos-csVector3 (0, Sys->cfg_body_height, 0), 1);
  spr->SetDrawCallback (animate_skeleton_tree_cb, NULL);
}

//===========================================================================
// Everything for skeletal ghost demo.
//===========================================================================

// Object added to every skeletal tree node to keep the animation
// information.
class GhostSkelSpriteInfo
{
public:
  float z_angle_base;
  float z_angle;
  float x_angle_base;
  float x_angle;
  float y_angle;
  float dx;
  float dz;
  float dy;
};

// Object added to the ghost sprite itself to hold some information
// about movement.
class GhostSpriteInfo : public csObject
{
public:
  float dir;
  CSOBJTYPE;
};

IMPLEMENT_CSOBJTYPE (GhostSpriteInfo, csObject);

// Recursive function to add limbs to a skeletal ghost. This also builds
// the sprite template.
void add_ghost_limbs (iSprite3DFactoryState* state, iSpriteFrame* frame,
	iSkeletonLimb* parent, int& vertex_idx,
	int prev_par_idx, int maxdepth, int width, int recursion, float dim)
{
  int par_vertex_idx = vertex_idx;
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);

  state->AddVertices (6);

  int anm_idx = frame->GetAnmIndex ();
  int tex_idx = frame->GetTexIndex ();

  state->GetVertex (anm_idx, par_vertex_idx+0) = csVector3(-dim, 0, -dim);
  state->GetVertex (anm_idx, par_vertex_idx+1) = csVector3(dim, 0, -dim);
  state->GetVertex (anm_idx, par_vertex_idx+2) = csVector3(0, 0, dim);
  state->GetVertex (anm_idx, par_vertex_idx+3) = csVector3(-dim, .45, -dim);
  state->GetVertex (anm_idx, par_vertex_idx+4) = csVector3(dim, .45, -dim);
  state->GetVertex (anm_idx, par_vertex_idx+5) = csVector3(0, .45, dim);

  state->GetTexel (tex_idx, par_vertex_idx+0) = csVector2(0, 0);
  state->GetTexel (tex_idx, par_vertex_idx+1) = csVector2(.99, 0);
  state->GetTexel (tex_idx, par_vertex_idx+2) = csVector2(0, .99);
  state->GetTexel (tex_idx, par_vertex_idx+3) = csVector2(.99, .99);
  state->GetTexel (tex_idx, par_vertex_idx+4) = csVector2(.5, .5);
  state->GetTexel (tex_idx, par_vertex_idx+5) = csVector2(.5, 0);

  if (recursion > 0)
  {
    // Create connection triangles with previous set
    state->AddTriangle (prev_par_idx+3, prev_par_idx+5, par_vertex_idx+0);
    state->AddTriangle (prev_par_idx+5, par_vertex_idx+2, par_vertex_idx+0);
    state->AddTriangle (prev_par_idx+4, par_vertex_idx+1, par_vertex_idx+2);
    state->AddTriangle (prev_par_idx+5, prev_par_idx+4, par_vertex_idx+2);
    state->AddTriangle (prev_par_idx+4, par_vertex_idx+0, par_vertex_idx+1);
    state->AddTriangle (prev_par_idx+4, prev_par_idx+3, par_vertex_idx+0);
  }
  // Create base triangles
  state->AddTriangle (par_vertex_idx+0, par_vertex_idx+5, par_vertex_idx+3);
  state->AddTriangle (par_vertex_idx+0, par_vertex_idx+2, par_vertex_idx+5);
  state->AddTriangle (par_vertex_idx+2, par_vertex_idx+4, par_vertex_idx+5);
  state->AddTriangle (par_vertex_idx+2, par_vertex_idx+1, par_vertex_idx+4);
  state->AddTriangle (par_vertex_idx+1, par_vertex_idx+3, par_vertex_idx+4);
  state->AddTriangle (par_vertex_idx+1, par_vertex_idx+0, par_vertex_idx+3);

  if (recursion >= maxdepth) return;
  iSkeletonConnection* con;
  int i;
  for (i = 0 ; i < width ; i++)
  {
    con = parent->CreateConnection ();
    csMatrix3 tr = csYRotMatrix3 (0) *
    	csZRotMatrix3 (.15) *
	csXRotMatrix3 (.15);
    csTransform trans (tr, -tr.GetInverse () * csVector3 (0, .5, 0));
    con->SetTransformation (trans);
    add_ghost_limbs (state, frame, QUERY_INTERFACE (con, iSkeletonLimb),
    	vertex_idx, par_vertex_idx,
    	maxdepth, 1, recursion+1, dim * .7);
  }
}

// Create a skeletal ghost.
iSkeleton* create_skelghost (iSprite3DFactoryState* state, iSpriteFrame* frame,
	int& vertex_idx, int maxdepth, int width)
{
  state->EnableSkeletalAnimation ();
  iSkeleton* skel = state->GetSkeleton ();
  add_ghost_limbs (state, frame, QUERY_INTERFACE (skel, iSkeletonLimb),
  	vertex_idx, 0, maxdepth, width, 0, .2);
  return skel;
}

// Animate a skeleton.
void animate_skeleton_ghost (iSkeletonLimbState* limb)
{
  iSkeletonLimbState* child = limb->GetChildren ();
  while (child)
  {
    GhostSkelSpriteInfo* o = (GhostSkelSpriteInfo*)child->GetUserData ();
    if (!o)
    {
      o = new GhostSkelSpriteInfo ();
      if ((rand () >> 3) & 0x1)
      {
        o->x_angle_base = (((float)((rand () >> 3)&0xff)) / 255.) * .4 -.2;
        o->x_angle = 0;
        o->dx = (rand () & 0x4) ? .005 : -.005;
        o->z_angle_base = (((float)((rand () >> 3)&0xff)) / 255.) * 1.2 -.6;
        o->z_angle = 0;
        o->dz = (rand () & 0x4) ? .02 : -.02;
      }
      else
      {
        o->z_angle_base = (((float)((rand () >> 3)&0xff)) / 255.) * .4 -.2;
        o->z_angle = 0;
        o->dz = (rand () & 0x4) ? .005 : -.005;
        o->x_angle_base = (((float)((rand () >> 3)&0xff)) / 255.) * 1.2 -.6;
        o->x_angle = 0;
        o->dx = (rand () & 0x4) ? .02 : -.02;
      }
      o->y_angle = 0;
      o->dy = (rand () & 0x4) ? .04 : -.04;
      child->SetUserData (o);
    }
    o->x_angle += o->dx;
    if (o->x_angle > .1 || o->x_angle < -.1) o->dx = -o->dx;
    o->z_angle += o->dz;
    if (o->z_angle > .1 || o->z_angle < -.1) o->dz = -o->dz;
    o->y_angle += o->dy;
    if (o->y_angle > .3 || o->y_angle < -.3) o->dy = -o->dy;

    // @@@ Don't use the code below in a real-time environment.
    // This is only demo code and HIGHLY inefficient.
    csMatrix3 tr = csYRotMatrix3 (o->y_angle) *
    	csZRotMatrix3 (o->z_angle + o->z_angle_base) *
	csXRotMatrix3 (o->x_angle + o->x_angle_base);
    csTransform trans (tr, -tr.GetInverse () * csVector3 (0, .5, 0));
    iSkeletonConnectionState* con = QUERY_INTERFACE (child, iSkeletonConnectionState);
    con->SetTransformation (trans);
    animate_skeleton_ghost (child);
    child = child->GetNextSibling ();
  }
}

void animate_skeleton_ghost_cb (iMeshWrapper* spr, iRenderView* /*rview*/, void* /*data*/)
{
  iMeshObject* obj = spr->GetMeshObject ();
  iSprite3DState* state = QUERY_INTERFACE (obj, iSprite3DState);
  iSkeletonState* sk_state = state->GetSkeletonState ();
  animate_skeleton_ghost (QUERY_INTERFACE (sk_state, iSkeletonLimbState));
}


// Add a skeletal ghost sprite. If needed it will also create
// the template for this.
void add_skeleton_ghost (csSector* where, csVector3 const& pos, int maxdepth,
	int width)
{
  char skelname[50];
  sprintf (skelname, "__skelghost__\n");
  iMeshFactoryWrapper* tmpl = Sys->engine->FindMeshFactory (skelname);
  if (!tmpl)
  {
    tmpl = Sys->engine->CreateMeshFactory (
    	"crystalspace.mesh.object.sprite.3d", skelname);
    if (tmpl == NULL)
    {
      CsPrintf (MSG_WARNING, "Could not load the sprite 3d plugin!\n");
      return;
    }
    iMeshObjectFactory* fact = tmpl->GetMeshObjectFactory ();
    iSprite3DFactoryState* state = QUERY_INTERFACE (fact, iSprite3DFactoryState);
    state->SetMaterialWrapper (Sys->engine->FindMaterial ("green"));
    int vertex_idx = 0;
    iSpriteFrame* fr = state->AddFrame ();
    fr->SetName ("f");
    iSpriteAction* act = state->AddAction ();
    act->SetName ("a");
    act->AddFrame (fr, 100);
    create_skelghost (state, fr, vertex_idx, maxdepth, width);
  }
  iMeshWrapper* spr = add_meshobj (skelname, "__skelghost__", where, pos, 1);
  iMeshObject* obj = spr->GetMeshObject ();
  iSprite3DState* state = QUERY_INTERFACE (obj, iSprite3DState);
  state->SetMixMode (CS_FX_SETALPHA (0.75));
  iPolygonMesh* mesh = QUERY_INTERFACE (obj, iPolygonMesh);
  iObject* sprobj = QUERY_INTERFACE (spr, iObject);
  (void)new csCollider (sprobj, Sys->collide_system, mesh);
  GhostSpriteInfo* gh_info = new GhostSpriteInfo ();
  sprobj->ObjAdd (QUERY_INTERFACE (gh_info, iObject));
  gh_info->dir = 1;
  spr->SetDrawCallback (animate_skeleton_ghost_cb, NULL);
}

#define MAXSECTORSOCCUPIED  20

extern int FindSectors (csVector3 v, csVector3 d, csSector *s, csSector **sa);
extern int CollisionDetect (csCollider *c, csSector* sp, csTransform *cdt);
extern csCollisionPair our_cd_contact[1000];//=0;
extern int num_our_cd;

void move_ghost (csMeshWrapper* spr)
{
  csCollider* col = csCollider::GetCollider (*spr);
  csSector* first_sector = spr->GetMovable ().GetSector (0);

  // Create a transformation 'test' which indicates where the ghost
  // is moving too.
  const csVector3& pos = spr->GetMovable ().GetPosition ();
  csVector3 vel (0, 0, .1);
  vel = spr->GetMovable ().GetTransform ().GetO2T () * vel;
  csVector3 new_pos = pos+vel;
  csMatrix3 m;
  csOrthoTransform test (m, new_pos);

  // Find all sectors that the ghost will occupy on the new position.
  csSector *n[MAXSECTORSOCCUPIED];
  int num_sectors = FindSectors (new_pos, 4.0f*spr->GetRadius(),
  	first_sector, n);

  // Start collision detection.
  Sys->collide_system->ResetCollisionPairs ();
  num_our_cd = 0;
  Sys->collide_system->SetOneHitOnly (false);
  int hits = 0;
  for ( ; num_sectors-- ; )
    hits += CollisionDetect (col, n[num_sectors], &test);

  // Change our velocity according to the collisions.
  for (int j=0 ; j<hits ; j++)
  {
    csCollisionPair& cd = our_cd_contact[j];
    csVector3 n = ((cd.c2-cd.b2)%(cd.b2-cd.a2)).Unit();
    if (n*vel<0)
      continue;
    vel = -(vel%n)%n;
  }

  if (!(vel < EPSILON))
  {
    // We move to our new position.
    new_pos = pos+vel;
    test = csReversibleTransform (csMatrix3 (), pos);
    bool mirror = true;
    first_sector = first_sector->FollowSegment (test, new_pos, mirror);
    spr->GetMovable ().SetSector (first_sector);
    spr->GetMovable ().SetPosition (new_pos);
  }

  // Turn around at random intervals.
  GhostSpriteInfo* gh_info = (GhostSpriteInfo*)
  	spr->GetChild (GhostSpriteInfo::Type);
  if (rand () % 40 == 1) gh_info->dir = -gh_info->dir;

  // OpenStep compiler bug prevents Transform(GetYRotation()), which is why
  // the expressions are split across two statements below.
  if (vel < 0.01f)
  {
    // We did not move much. Turn around quickly.
    csMatrix3 m = csYRotMatrix3 (gh_info->dir*.2);
    spr->GetMovable ().Transform (m);
  }
  else if (vel < 0.05f)
  {
    // We did a bit. Turn around slightly.
    csMatrix3 m = csYRotMatrix3 (gh_info->dir*.1);
    spr->GetMovable ().Transform (m);
  }
  else
  {
    csMatrix3 m = csYRotMatrix3 (gh_info->dir*.01);
    spr->GetMovable ().Transform (m);
  }
  spr->GetMovable ().UpdateMove ();
}

//===========================================================================
// Everything for bots.
//===========================================================================

Bot* first_bot = NULL;
bool do_bots = false;

// Add a bot with some size at the specified positin.
void add_bot (float size, csSector* where, csVector3 const& pos,
	float dyn_radius)
{
  csDynLight* dyn = NULL;
  if (dyn_radius)
  {
    float r, g, b;
    RandomColor (r, g, b);
    //@@@ MEMORY LEAK?
    dyn = new csDynLight (pos.x, pos.y, pos.z, dyn_radius, r, g, b);
    Sys->view->GetEngine ()->AddDynLight (dyn);
    dyn->SetSector (where);
    dyn->Setup ();
  }
  csMeshFactoryWrapper* tmpl = (csMeshFactoryWrapper*)
  	Sys->view->GetEngine ()->meshobj_factories.FindByName ("bot");
  if (!tmpl) return;
  iMeshObject* botmesh = tmpl->GetMeshObjectFactory ()->NewInstance ();
  Bot* bot;
  bot = new Bot (Sys->view->GetEngine(), botmesh);
  botmesh->DecRef ();
  bot->SetName ("bot");
  Sys->view->GetEngine ()->sprites.Push (bot);
  bot->GetMovable ().SetSector (where);
  csMatrix3 m; m.Identity (); m = m * size;
  bot->GetMovable ().SetTransform (m);
  bot->set_bot_move (pos);
  bot->set_bot_sector (where);
  bot->GetMovable ().UpdateMove ();
  iSprite3DState* state = QUERY_INTERFACE (botmesh, iSprite3DState);
  state->SetAction ("default");
  bot->next = first_bot;
  bot->light = dyn;
  first_bot = bot;
}

void del_bot ()
{
  if (first_bot)
  {
    Bot* bot = first_bot;
    first_bot = bot->next;
    Sys->view->GetEngine ()->RemoveSprite (bot);
  }
}

void move_bots (cs_time elapsed_time)
{
  if (first_bot)
  {
    Bot* bot = first_bot;
    while (bot)
    {
      bot->move (elapsed_time);
      bot = bot->next;
    }
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
  csMeshWrapper* sprite;
  iSoundSource *snd;
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

void HandleDynLight (csDynLight* dyn)
{
  LightStruct* ls = (LightStruct*)(csDataObject::GetData(*dyn));
  switch (ls->type)
  {
    case DYN_TYPE_MISSILE:
    {
      MissileStruct* ms = (MissileStruct*)(csDataObject::GetData(*dyn));
      csVector3 v (0, 0, 2.5);
      csVector3 old = dyn->GetCenter ();
      v = old + ms->dir.GetT2O () * v;
      csSector* s = dyn->GetSector ();
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
            add_bot (1, dyn->GetSector (), dyn->GetCenter (), 0);
	  }
	  ms->sprite->GetMovable ().ClearSectors ();
	  Sys->view->GetEngine ()->RemoveSprite (ms->sprite);
	}
        dyn->ObjRemove(dyn->GetChild (csDataObject::Type));
        if (ms->snd)
        {
          ms->snd->Stop();
          ms->snd->DecRef();
        }
        delete ms;
        if (Sys->Sound)
        {
          iSoundSource *sndsrc;
          if ((sndsrc = Sys->Sound->CreateSource (Sys->wMissile_boom, SOUND3D_ABSOLUTE)))
          {
            sndsrc->SetPosition (v);
            sndsrc->Play();
            sndsrc->DecRef();
          }
        }
        ExplosionStruct* es = new ExplosionStruct;
        es->type = DYN_TYPE_EXPLOSION;
        es->radius = 2;
        es->dir = 1;
        csDataObject* esdata = new csDataObject (es);
        dyn->ObjAdd (esdata);
        add_particles_explosion (dyn->GetSector (), dyn->GetCenter (), "explo");
        return;
      }
      else ms->dir.SetOrigin (v);
      dyn->Move (s, v.x, v.y, v.z);
      dyn->Setup ();
      if (ms->sprite) move_sprite (ms->sprite, s, v);
      if (Sys->Sound && ms->snd) ms->snd->SetPosition (v);
      break;
    }
    case DYN_TYPE_EXPLOSION:
    {
      ExplosionStruct* es = (ExplosionStruct*)(csDataObject::GetData(*dyn));
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
	  delete es;
          Sys->view->GetEngine ()->RemoveDynLight (dyn);
          delete dyn;
	  return;
	}
      }
      dyn->Resize (es->radius);
      dyn->Setup ();
      break;
    }
    case DYN_TYPE_RANDOM:
    {
      RandomLight* rl = (RandomLight*)(csDataObject::GetData(*dyn));
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
      dyn->Move (dyn->GetSector (), dyn->GetCenter ().x,
      	dyn->GetCenter ().y+rl->dyn_move_dir, dyn->GetCenter ().z);
      dyn->Setup ();
      break;
    }
  }
}

void fire_missile ()
{
  csVector3 dir (0, 0, 0);
  csVector3 pos = Sys->view->GetCamera ()->Camera2World (dir);
  float r, g, b;
  RandomColor (r, g, b);
  csDynLight* dyn = new csDynLight (pos.x, pos.y, pos.z, 4, r, g, b);
  Sys->view->GetEngine ()->AddDynLight (dyn);
  dyn->SetSector (Sys->view->GetCamera ()->GetSector ());
  dyn->Setup ();
  MissileStruct* ms = new MissileStruct;
  ms->snd = NULL;
  if (Sys->Sound)
    if ((ms->snd = Sys->Sound->CreateSource (Sys->wMissile_whoosh, SOUND3D_ABSOLUTE)))
    {
      ms->snd->SetPosition (pos);
      ms->snd->Play();
    }
  ms->type = DYN_TYPE_MISSILE;
  ms->dir = (csOrthoTransform)*(Sys->view->GetCamera ());
  ms->sprite = NULL;
  csDataObject* msdata = new csDataObject(ms);
  dyn->ObjAdd(msdata);

  char misname[10];
  sprintf (misname, "missile%d", ((rand () >> 3) & 1)+1);

  csMeshFactoryWrapper* tmpl = (csMeshFactoryWrapper*)
  	Sys->view->GetEngine ()->meshobj_factories.FindByName (misname);
  if (!tmpl)
    Sys->Printf (MSG_CONSOLE, "Could not find '%s' sprite factory!\n", misname);
  else
  {
    csMeshWrapper* sp = tmpl->NewMeshObject (Sys->view->GetEngine ());
    sp->SetName ("missile");
    Sys->view->GetEngine ()->sprites.Push (sp);
    sp->GetMovable ().SetSector (Sys->view->GetCamera ()->GetSector ());
    ms->sprite = sp;
    sp->GetMovable ().SetPosition (pos);
    csMatrix3 m = ms->dir.GetT2O ();
    sp->GetMovable ().SetTransform (m);
    move_sprite (sp, Sys->view->GetCamera ()->GetSector (), pos);
    sp->GetMovable ().UpdateMove ();
  } 
}

void AttachRandomLight (csDynLight* light)
{
  RandomLight* rl = new RandomLight;
  rl->type = DYN_TYPE_RANDOM;
  rl->dyn_move_dir = .2;
  rl->dyn_move = 0;
  rl->dyn_r1 = rl->dyn_g1 = rl->dyn_b1 = 1;
  csDataObject* rldata = new csDataObject (rl);
  light->ObjAdd (rldata);
}

//===========================================================================

// Light all sprites and animate the skeletal trees.
// This function does no effort at all to optimize stuff. It does
// not test if the sprite is visible or not.
void light_statics ()
{
  csEngine* e = Sys->view->GetEngine ();
  for (int i = 0 ; i < e->sprites.Length () ; i++)
  {
    csSprite* sp = (csSprite*)e->sprites [i];
    if (sp->GetType () == csMeshWrapper::Type)
    {
      csMeshWrapper* wrap = (csMeshWrapper*)sp;
      iSprite3DState* state = QUERY_INTERFACE (wrap->GetMeshObject (), iSprite3DState);
      if (state != NULL)
      {
        if (state->GetSkeletonState ())
	{
          const char* name = wrap->GetName ();
          if (!strcmp (name, "__skelghost__")) move_ghost (wrap);
	}
      }
    }
    sp->DeferUpdateLighting (CS_NLIGHT_STATIC|CS_NLIGHT_DYNAMIC, 10);
  }
}

//===========================================================================

csThing* CreatePortalThing (const char* name, csSector* room,
    	csMaterialWrapper* tm, csPolygon3D*& portalPoly)
{
  csThing* thing = new csThing (Sys->engine);
  Sys->engine->things.Push (thing);
  thing->SetName (name);
  thing->GetMovable ().SetSector (room);
  float dx = 1, dy = 3, dz = .3;
  float border = 0.3; // width of border around the portal

  // bottom
  csPolygon3D* p;
  p = thing->NewPolygon (tm);
  p->flags.Reset (CS_POLY_COLLDET);
  p->AddVertex (-dx, 0, -dz);
  p->AddVertex (dx, 0, -dz);
  p->AddVertex (dx, 0, dz);
  p->AddVertex (-dx, 0, dz);
  p->SetTextureSpace (
      p->Vobj (0), csVector2 (0.25, 0.875),
      p->Vobj (1), csVector2 (0.75, 0.875),
      p->Vobj (2), csVector2 (0.75, 0.75));

  // top
  p = thing->NewPolygon (tm);
  p->flags.Reset (CS_POLY_COLLDET);
  p->AddVertex (-dx, dy, dz);
  p->AddVertex (dx, dy, dz);
  p->AddVertex (dx, dy, -dz);
  p->AddVertex (-dx, dy, -dz);
  p->SetTextureSpace (
      p->Vobj (0), csVector2 (0.25, 0.25),
      p->Vobj (1), csVector2 (0.75, 0.25),
      p->Vobj (2), csVector2 (0.75, 0.125));

  // back
  p = thing->NewPolygon (tm);
  p->flags.Reset (CS_POLY_COLLDET);
  p->AddVertex (-dx, 0, dz);
  p->AddVertex (dx, 0, dz);
  p->AddVertex (dx, dy, dz);
  p->AddVertex (-dx, dy, dz);
  p->SetTextureSpace (
      p->Vobj (0), csVector2 (0.25, 0.75),
      p->Vobj (1), csVector2 (0.75, 0.75),
      p->Vobj (2), csVector2 (0.75, 0.25));

  // right
  p = thing->NewPolygon (tm);
  p->flags.Reset (CS_POLY_COLLDET);
  p->AddVertex (dx, 0, dz);
  p->AddVertex (dx, 0, -dz);
  p->AddVertex (dx, dy, -dz);
  p->AddVertex (dx, dy, dz);
  p->SetTextureSpace (
      p->Vobj (0), csVector2 (0.75, 0.75),
      p->Vobj (1), csVector2 (0.875, 0.75),
      p->Vobj (2), csVector2 (0.875, 0.25));

  // left
  p = thing->NewPolygon (tm);
  p->flags.Reset (CS_POLY_COLLDET);
  p->AddVertex (-dx, 0, -dz);
  p->AddVertex (-dx, 0, dz);
  p->AddVertex (-dx, dy, dz);
  p->AddVertex (-dx, dy, -dz);
  p->SetTextureSpace (
      p->Vobj (0), csVector2 (0.125, 0.75),
      p->Vobj (1), csVector2 (0.25, 0.75),
      p->Vobj (2), csVector2 (0.25, 0.25));

  // front border
  // border top
  p = thing->NewPolygon (tm);
  p->flags.Reset (CS_POLY_COLLDET);
  p->AddVertex (-dx+border, dy, -dz);
  p->AddVertex (dx-border, dy, -dz);
  p->AddVertex (dx-border, dy-border, -dz);
  p->AddVertex (-dx+border, dy-border, -dz);
  p->SetTextureSpace (
      p->Vobj (0), csVector2 (0.125, 0.125),
      p->Vobj (1), csVector2 (0.875, 0.125),
      p->Vobj (2), csVector2 (0.875, 0.0));
  // border right
  p = thing->NewPolygon (tm);
  p->flags.Reset (CS_POLY_COLLDET);
  p->AddVertex (dx-border, dy-border, -dz);
  p->AddVertex (dx, dy-border, -dz);
  p->AddVertex (dx, border, -dz);
  p->AddVertex (dx-border, border, -dz);
  p->SetTextureSpace (
      p->Vobj (0), csVector2 (1.0, 0.125),
      p->Vobj (1), csVector2 (0.875, 0.125),
      p->Vobj (2), csVector2 (0.875, 0.875));
  // border bottom
  p = thing->NewPolygon (tm);
  p->flags.Reset (CS_POLY_COLLDET);
  p->AddVertex (-dx+border, border, -dz);
  p->AddVertex (+dx-border, border, -dz);
  p->AddVertex (+dx-border, 0.0, -dz);
  p->AddVertex (-dx+border, 0.0, -dz);
  p->SetTextureSpace (
      p->Vobj (0), csVector2 (0.125, 1.0),
      p->Vobj (1), csVector2 (0.875, 1.0),
      p->Vobj (2), csVector2 (0.875, 0.875));
  // border left
  p = thing->NewPolygon (tm);
  p->flags.Reset (CS_POLY_COLLDET);
  p->AddVertex (-dx, dy-border, -dz);
  p->AddVertex (-dx+border, dy-border, -dz);
  p->AddVertex (-dx+border, border, -dz);
  p->AddVertex (-dx, border, -dz);
  p->SetTextureSpace (
      p->Vobj (0), csVector2 (0.125, 0.125),
      p->Vobj (1), csVector2 (0.0, 0.125),
      p->Vobj (2), csVector2 (0.0, 0.875));
  // border topleft
  p = thing->NewPolygon (tm);
  p->flags.Reset (CS_POLY_COLLDET);
  p->AddVertex (-dx, dy, -dz);
  p->AddVertex (-dx+border, dy, -dz);
  p->AddVertex (-dx+border, dy-border, -dz);
  p->AddVertex (-dx, dy-border, -dz);
  p->SetTextureSpace (
      p->Vobj (0), csVector2 (0.125, 0.125),
      p->Vobj (1), csVector2 (0.0, 0.125),
      p->Vobj (2), csVector2 (0.0, 0.0));
  // border topright
  p = thing->NewPolygon (tm);
  p->flags.Reset (CS_POLY_COLLDET);
  p->AddVertex (dx-border, dy, -dz);
  p->AddVertex (dx, dy, -dz);
  p->AddVertex (dx, dy-border, -dz);
  p->AddVertex (dx-border, dy-border, -dz);
  p->SetTextureSpace (
      p->Vobj (0), csVector2 (1.0, 0.125),
      p->Vobj (1), csVector2 (0.875, 0.125),
      p->Vobj (2), csVector2 (0.875, 0.0));
  // border botright
  p = thing->NewPolygon (tm);
  p->flags.Reset (CS_POLY_COLLDET);
  p->AddVertex (dx-border, border, -dz);
  p->AddVertex (dx, border, -dz);
  p->AddVertex (dx, 0.0, -dz);
  p->AddVertex (dx-border, 0.0, -dz);
  p->SetTextureSpace (
      p->Vobj (0), csVector2 (1.0, 1.0),
      p->Vobj (1), csVector2 (0.875, 1.0),
      p->Vobj (2), csVector2 (0.875, 0.875));
  // border botleft
  p = thing->NewPolygon (tm);
  p->flags.Reset (CS_POLY_COLLDET);
  p->AddVertex (-dx, border, -dz);
  p->AddVertex (-dx+border, border, -dz);
  p->AddVertex (-dx+border, 0.0, -dz);
  p->AddVertex (-dx, 0.0, -dz);
  p->SetTextureSpace (
      p->Vobj (0), csVector2 (0.125, 1.0),
      p->Vobj (1), csVector2 (0.0, 1.0),
      p->Vobj (2), csVector2 (0.0, 0.875));

  // front - the portal
  p = thing->NewPolygon (tm);
  p->flags.Reset (CS_POLY_COLLDET);
  // old
  //p->AddVertex (dx, 0, -dz);
  //p->AddVertex (-dx, 0, -dz);
  //p->AddVertex (-dx, dy, -dz);
  //p->AddVertex (dx, dy, -dz);
  p->AddVertex (dx-border, border, -dz);
  p->AddVertex (-dx+border, border, -dz);
  p->AddVertex (-dx+border, dy-border, -dz);
  p->AddVertex (dx-border, dy-border, -dz);
  p->SetTextureSpace (
      p->Vobj (0), csVector2 (0, 0),
      p->Vobj (1), csVector2 (1, 0),
      p->Vobj (2), csVector2 (1, 1));
  portalPoly = p;

  thing->Prepare (room);
  thing->InitLightMaps (false);
  room->ShineLights (thing);
  thing->CreateLightMaps (Sys->G3D);

  return thing;
}

void OpenPortal (csView* view, char* lev)
{
  csSector* room = view->GetCamera ()->GetSector ();
  csVector3 pos = view->GetCamera ()->Camera2World (csVector3 (0, 0, 1));
  csMaterialWrapper* tm = Sys->engine->GetMaterials ()->FindByName ("portal");

  csPolygon3D* portalPoly;
  csThing* thing = CreatePortalThing ("portalTo", room, tm, portalPoly);

  bool regionExists = (Sys->engine->regions.FindByName (lev) != NULL);
  Sys->engine->SelectRegion (lev);
  // If the region did not already exist then we load the level in it.
  if (!regionExists)
  {
    // @@@ No error checking!
    char buf[255];
    sprintf (buf, "/lev/%s", lev);
    Sys->VFS->ChDir (buf);
    csLoader::AppendMapFile (Sys->engine, "world");
    Sys->engine->GetCurrentRegion ()->Prepare ();
  }

  thing->GetMovable ().SetPosition (pos + csVector3 (0, Sys->cfg_legs_offset, 0));
  thing->GetMovable ().Transform (view->GetCamera ()->GetC2W ());
  thing->GetMovable ().UpdateMove ();

  // First make a portal to the new level.
  csCameraPosition* cp = (csCameraPosition*)
    	Sys->engine->GetCurrentRegion ()->FindCameraPosition ("Start");
  const char* room_name;
  csVector3 topos;
  if (cp) { room_name = cp->Sector; topos = cp->Position; }
  else { room_name = "room"; topos.Set (0, 0, 0); }
  topos.y -= Sys->cfg_eye_offset;
  iSector* start_sector = Sys->engine->GetCurrentRegion ()->FindSector (room_name);
  if (start_sector)
  {
    portalPoly->SetCSPortal (start_sector->GetPrivateObject ());
    csPortal* portal = portalPoly->GetPortal ();
    portal->flags.Set (CS_PORTAL_ZFILL);
    portal->flags.Set (CS_PORTAL_CLIPDEST);
    portal->SetWarp (view->GetCamera ()->GetC2W (), topos, pos);

    if (!regionExists)
    {
      // Only if the region did not already exist do we create a portal
      // back. So even if multiple portals go to the region we only have
      // one portal back.
      csPolygon3D* portalPolyBack;
      csThing* thingBack = CreatePortalThing ("portalFrom",
	  	start_sector->GetPrivateObject (), tm, portalPolyBack);
      thingBack->GetMovable ().SetPosition (topos + csVector3 (0, Sys->cfg_legs_offset, -.1));
      thingBack->GetMovable ().Transform (csYRotMatrix3 (M_PI));//view->GetCamera ()->GetW2C ());
      thingBack->GetMovable ().UpdateMove ();
      portalPolyBack->SetCSPortal (room);
      csPortal* portalBack = portalPolyBack->GetPortal ();
      portalBack->flags.Set (CS_PORTAL_ZFILL);
      portalBack->flags.Set (CS_PORTAL_CLIPDEST);
      portalBack->SetWarp (view->GetCamera ()->GetW2C (), pos, topos);
    }
  }

  if (!regionExists)
    Sys->InitCollDet (Sys->engine, Sys->engine->GetCsCurrentRegion ());
  Sys->engine->SelectRegion (NULL);
}

