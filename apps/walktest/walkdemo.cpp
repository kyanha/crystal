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

#include "cssysdef.h"
#include "qint.h"
#include "cssys/system.h"
#include "walktest/walktest.h"
#include "walktest/bot.h"
#include "walktest/infmaze.h"
#include "walktest/hugeroom.h"
#include "apps/support/command.h"
#include "csengine/dumper.h"
#include "csengine/camera.h"
#include "csengine/octree.h"
#include "csengine/world.h"
#include "csengine/csview.h"
#include "csengine/wirefrm.h"
#include "csengine/cssprite.h"
#include "csengine/skeleton.h"
#include "csengine/triangle.h"
#include "csengine/polygon.h"
#include "csengine/light.h"
#include "csengine/sector.h"
#include "csengine/thing.h"
#include "csengine/cspixmap.h"
#include "csengine/collider.h"
#include "csengine/particle.h"
#include "csengine/region.h"
#include "csutil/scanstr.h"
#include "csparser/impexp.h"
#include "csobject/dataobj.h"
#include "isnddata.h"
#include "csparser/snddatao.h"
#include "csparser/csloader.h"
#include "csparser/crossbld.h"
#include "csgeom/math3d.h"
#include "isndsrc.h"
#include "isndlstn.h"
#include "isndsrc.h"
#include "isndrdr.h"
#include "igraph3d.h"
#include "icollide.h"

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

extern csSprite3D* add_sprite (char* tname, char* sname, csSector* where,
	csVector3 const& pos, float size);
extern void move_sprite (csSprite3D* sprite, csSector* where,
	csVector3 const& pos);

//===========================================================================
// Demo particle system (rain).
//===========================================================================
void add_particles_rain (csSector* sector, char* matname, int num, float speed)
{
  // First check if the material exists.
  csMaterialWrapper* mat = Sys->view->GetWorld ()->GetMaterials ()->
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

    csRainParticleSystem* exp = new csRainParticleSystem (
    	  Sys->view->GetWorld (), num,
  	  mat, CS_FX_ADD, false, .3/50., .3,
	  bbox.Min (), bbox.Max (),
	  csVector3 (0, -speed, 0));
    exp->GetMovable ().SetSector (sector);
    exp->SetColor (csColor (.25,.25,.25));
    exp->GetMovable ().UpdateMove ();
  }
}

//===========================================================================
// Demo particle system (snow).
//===========================================================================
void add_particles_snow (csSector* sector, char* matname, int num, float speed)
{
  // First check if the material exists.
  csMaterialWrapper* mat = Sys->view->GetWorld ()->GetMaterials ()->
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

    csSnowParticleSystem* exp = new csSnowParticleSystem (
    	  Sys->view->GetWorld (), num,
  	  mat, CS_FX_ADD, false, .07, .07,
	  bbox.Min (), bbox.Max (),
	  csVector3 (0, -speed, 0), .2);
    exp->GetMovable ().SetSector (sector);
    exp->SetColor (csColor (.25,.25,.25));
    exp->GetMovable ().UpdateMove ();
  }
}

//===========================================================================
// Demo particle system (fire).
//===========================================================================
void add_particles_fire (csSector* sector, char* matname, int num,
	const csVector3& origin)
{
  // First check if the material exists.
  csMaterialWrapper* mat = Sys->view->GetWorld ()->GetMaterials ()->
  	FindByName (matname);
  if (!mat)
  {
    Sys->Printf (MSG_CONSOLE, "Can't find material '%s' in memory!\n", matname);
    return;
  }

   csFireParticleSystem* exp = new csFireParticleSystem(
     Sys->view->GetWorld (), num, mat,
     CS_FX_ADD, false, 0.02, 0.04,
     3.0, csVector3(0,1.0,0), origin,
     0.6, 0.20);
  exp->GetMovable ().SetSector (sector);
  exp->GetMovable ().UpdateMove ();
}

//===========================================================================
// Demo particle system (fountain).
//===========================================================================
void add_particles_fountain (csSector* sector, char* matname, int num,
	const csVector3& origin)
{
  // First check if the material exists.
  csMaterialWrapper* mat = Sys->view->GetWorld ()->GetMaterials ()->
  	FindByName (matname);
  if (!mat)
  {
    Sys->Printf (MSG_CONSOLE, "Can't find material '%s' in memory!\n", matname);
    return;
  }

  csFountainParticleSystem* exp = new csFountainParticleSystem(
      Sys->view->GetWorld (), num, mat,
      CS_FX_ADD, false, 0.1, 0.1,
      origin, csVector3(0, -1.0, 0), 5.0,
      3.0, 0.2,
      0.0, 3.1415926/2.);
  exp->GetMovable ().SetSector (sector);
  exp->SetChangeRotation(7.5);
  //exp->SetColor (csColor (.25,.25,.25));
  exp->SetColor (csColor (.25,.35,.55));
  exp->GetMovable ().UpdateMove ();
}

//===========================================================================
// Demo particle system (explosion).
//===========================================================================
void add_particles_explosion (csSector* sector, const csVector3& center, char* matname)
{
  // First check if the material exists.
  csMaterialWrapper* mat = Sys->view->GetWorld ()->GetMaterials ()->
  	FindByName (matname);
  if (!mat)
  {
    Sys->Printf (MSG_CONSOLE, "Can't find material '%s' in memory!\n", matname);
    return;
  }

  csParSysExplosion* exp = new csParSysExplosion (
  	Sys->view->GetWorld (), 100,
  	center, csVector3 (0, 0, 0), mat, 6, 0.15, true, .6, 2., 2.);
  exp->GetMovable ().SetSector (sector);
  exp->SetSelfDestruct (3000);
  exp->SetMixmode (CS_FX_SETALPHA (0.50));
  exp->SetChangeRotation(5.0);
  exp->SetChangeSize (1.25);
  exp->SetFadeSprites (500);
  exp->SetColor( csColor(1,1,0) );
  exp->SetChangeColor( csColor(0,-1.0/3.2,0) );
  exp->AddLight (Sys->world, sector, 1000);
  exp->GetMovable ().UpdateMove ();
}

//===========================================================================
// Demo particle system (spiral).
//===========================================================================
void add_particles_spiral (csSector* sector, const csVector3& bottom, char* matname)
{
  // First check if the material exists.
  csMaterialWrapper* mat = Sys->view->GetWorld ()->GetMaterials ()->
  	FindByName (matname);
  if (!mat)
  {
    Sys->Printf (MSG_CONSOLE, "Can't find material '%s' in memory!\n", matname);
    return;
  }

  csSpiralParticleSystem* exp = new csSpiralParticleSystem (
  	Sys->view->GetWorld (), 500,
  	bottom, mat);
  exp->GetMovable ().SetSector (sector);
  //exp->SetSelfDestruct (3000);
  exp->SetMixmode (CS_FX_SETALPHA (0.50));
  //exp->SetChangeRotation(5.0);
  //exp->SetChangeSize (1.25);
  //exp->SetFadeSprites (500);
  exp->SetColor( csColor(1,1,0) );
  exp->SetChangeColor( csColor(+0.01,0.,-0.012) );
  //exp->AddLight (Sys->world, sector, 1000);
  exp->GetMovable ().UpdateMove ();
}

//===========================================================================
// Everything for skeletal tree demo.
//===========================================================================

// Recursive function to add limbs to a skeletal tree. This also builds
// the sprite template.
void add_tree_limbs (csSpriteTemplate* tmpl, csFrame* frame,
	csSkeletonLimb* parent, int& vertex_idx,
	int prev_par_idx, int maxdepth, int width, int recursion)
{
  int par_vertex_idx = vertex_idx;
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);

  tmpl->AddVertices(6);

  int anm_idx = frame->GetAnmIndex ();
  int tex_idx = frame->GetTexIndex ();

  tmpl->GetVertex(anm_idx, par_vertex_idx+0) = csVector3(-.05, 0, -.05);
  tmpl->GetVertex(anm_idx, par_vertex_idx+1) = csVector3(.05, 0, -.05);
  tmpl->GetVertex(anm_idx, par_vertex_idx+2) = csVector3(0, 0, .05);
  tmpl->GetVertex(anm_idx, par_vertex_idx+3) = csVector3(-.05, .45, -.05);
  tmpl->GetVertex(anm_idx, par_vertex_idx+4) = csVector3(.05, .45, -.05);
  tmpl->GetVertex(anm_idx, par_vertex_idx+5) = csVector3(0, .45, .05);

  tmpl->GetTexel(tex_idx, par_vertex_idx+0) = csVector2(0, 0);
  tmpl->GetTexel(tex_idx, par_vertex_idx+1) = csVector2(.99, 0);
  tmpl->GetTexel(tex_idx, par_vertex_idx+2) = csVector2(0, .99);
  tmpl->GetTexel(tex_idx, par_vertex_idx+3) = csVector2(.99, .99);
  tmpl->GetTexel(tex_idx, par_vertex_idx+4) = csVector2(.5, .5);
  tmpl->GetTexel(tex_idx, par_vertex_idx+5) = csVector2(.5, 0);

  if (recursion > 0)
  {
    // Create connection triangles with previous set
    tmpl->AddTriangle (prev_par_idx+3, prev_par_idx+5, par_vertex_idx+0);
    tmpl->AddTriangle (prev_par_idx+5, par_vertex_idx+2, par_vertex_idx+0);
    tmpl->AddTriangle (prev_par_idx+4, par_vertex_idx+1, par_vertex_idx+2);
    tmpl->AddTriangle (prev_par_idx+5, prev_par_idx+4, par_vertex_idx+2);
    tmpl->AddTriangle (prev_par_idx+4, par_vertex_idx+0, par_vertex_idx+1);
    tmpl->AddTriangle (prev_par_idx+4, prev_par_idx+3, par_vertex_idx+0);
  }
  // Create base triangles
  tmpl->AddTriangle (par_vertex_idx+0, par_vertex_idx+5, par_vertex_idx+3);
  tmpl->AddTriangle (par_vertex_idx+0, par_vertex_idx+2, par_vertex_idx+5);
  tmpl->AddTriangle (par_vertex_idx+2, par_vertex_idx+4, par_vertex_idx+5);
  tmpl->AddTriangle (par_vertex_idx+2, par_vertex_idx+1, par_vertex_idx+4);
  tmpl->AddTriangle (par_vertex_idx+1, par_vertex_idx+3, par_vertex_idx+4);
  tmpl->AddTriangle (par_vertex_idx+1, par_vertex_idx+0, par_vertex_idx+3);

  if (recursion >= maxdepth) return;
  csSkeletonConnection* con;
  int i;
  int rwidth;
  if (width < 0)
    rwidth = 1 + ((rand () >> 3) % (-width));
  else rwidth = width;

  for (i = 0 ; i < rwidth ; i++)
  {
    con = new csSkeletonConnection ();
    parent->AddChild (con);
    csMatrix3 tr = csYRotMatrix3 (0) * csZRotMatrix3(.15) *
                                                 csXRotMatrix3(.15);
    csTransform trans (tr, -tr.GetInverse () * csVector3 (0, .5, 0));
    con->SetTransformation (trans);
    add_tree_limbs (tmpl, frame, con, vertex_idx, par_vertex_idx, maxdepth, width, recursion+1);
  }
}

// Create a skeletal tree.
csSkeleton* create_skeltree (csSpriteTemplate* tmpl, csFrame* frame,
	int& vertex_idx, int maxdepth, int width)
{
  csSkeleton* skel = new csSkeleton ();
  add_tree_limbs (tmpl, frame, skel, vertex_idx, 0, maxdepth, width, 0);
  return skel;
}

// Object added to every skeletal tree node to keep the animation
// information.
class TreeSkelSpriteInfo : public csObject
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
  CSOBJTYPE;
};

IMPLEMENT_CSOBJTYPE (TreeSkelSpriteInfo, csObject);

// Animate a skeleton.
void animate_skeleton_tree (csSkeletonLimbState* limb)
{
  csSkeletonConnectionState* con = (csSkeletonConnectionState*)limb->GetChildren ();
  while (con)
  {
    TreeSkelSpriteInfo* o = (TreeSkelSpriteInfo*)con->GetChild (TreeSkelSpriteInfo::Type);
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
      con->ObjAdd (o);
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
    con->SetTransformation (trans);
    animate_skeleton_tree (con);
    con = (csSkeletonConnectionState*)(con->GetNext ());
  }
}

void animate_skeleton_tree_cb (csSprite3D* spr, csRenderView* /*rview*/)
{
  csSkeletonState* sk_state = spr->GetSkeletonState ();
  animate_skeleton_tree (sk_state);
}

// Add a skeletal tree sprite. If needed it will also create
// the template for this.
void add_skeleton_tree (csSector* where, csVector3 const& pos, int depth,
	int width)
{
  char skelname[50];
  sprintf (skelname, "__skeltree__%d,%d\n", depth, width);
  csSpriteTemplate* tmpl = (csSpriteTemplate*)
  	Sys->view->GetWorld ()->sprite_templates.FindByName (skelname);
  if (!tmpl)
  {
    tmpl = new csSpriteTemplate ();
    tmpl->SetName (skelname);
    Sys->world->sprite_templates.Push (tmpl);
    tmpl->SetMaterial (Sys->world->GetMaterials ()->FindByName ("white"));
    int vertex_idx = 0;
    csFrame* fr = tmpl->AddFrame ();
    fr->SetName ("f");
    csSpriteAction* act = tmpl->AddAction ();
    act->SetName ("a");
    act->AddFrame (fr, 100);
    tmpl->SetSkeleton (create_skeltree (tmpl, fr, vertex_idx, depth, width));
    tmpl->GenerateLOD ();
    tmpl->ComputeBoundingBox ();
  }
  csSprite3D* spr = add_sprite (skelname, "__skeltree__", where, pos-csVector3 (0, Sys->cfg_body_height, 0), 1);
  spr->SetDrawCallback (animate_skeleton_tree_cb);
}

//===========================================================================
// Everything for skeletal ghost demo.
//===========================================================================

// Object added to every skeletal tree node to keep the animation
// information.
class GhostSkelSpriteInfo : public csObject
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
  CSOBJTYPE;
};

IMPLEMENT_CSOBJTYPE (GhostSkelSpriteInfo, csObject);

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
void add_ghost_limbs (csSpriteTemplate* tmpl, csFrame* frame, csSkeletonLimb* parent, int& vertex_idx,
	int prev_par_idx, int maxdepth, int width, int recursion, float dim)
{
  int par_vertex_idx = vertex_idx;
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);
  parent->AddVertex (vertex_idx++);

  tmpl->AddVertices (6);

  int anm_idx = frame->GetAnmIndex ();
  int tex_idx = frame->GetTexIndex ();

  tmpl->GetVertex(anm_idx, par_vertex_idx+0) = csVector3(-dim, 0, -dim);
  tmpl->GetVertex(anm_idx, par_vertex_idx+1) = csVector3(dim, 0, -dim);
  tmpl->GetVertex(anm_idx, par_vertex_idx+2) = csVector3(0, 0, dim);
  tmpl->GetVertex(anm_idx, par_vertex_idx+3) = csVector3(-dim, .45, -dim);
  tmpl->GetVertex(anm_idx, par_vertex_idx+4) = csVector3(dim, .45, -dim);
  tmpl->GetVertex(anm_idx, par_vertex_idx+5) = csVector3(0, .45, dim);

  tmpl->GetTexel(tex_idx, par_vertex_idx+0) = csVector2(0, 0);
  tmpl->GetTexel(tex_idx, par_vertex_idx+1) = csVector2(.99, 0);
  tmpl->GetTexel(tex_idx, par_vertex_idx+2) = csVector2(0, .99);
  tmpl->GetTexel(tex_idx, par_vertex_idx+3) = csVector2(.99, .99);
  tmpl->GetTexel(tex_idx, par_vertex_idx+4) = csVector2(.5, .5);
  tmpl->GetTexel(tex_idx, par_vertex_idx+5) = csVector2(.5, 0);

  if (recursion > 0)
  {
    // Create connection triangles with previous set
    tmpl->AddTriangle (prev_par_idx+3, prev_par_idx+5, par_vertex_idx+0);
    tmpl->AddTriangle (prev_par_idx+5, par_vertex_idx+2, par_vertex_idx+0);
    tmpl->AddTriangle (prev_par_idx+4, par_vertex_idx+1, par_vertex_idx+2);
    tmpl->AddTriangle (prev_par_idx+5, prev_par_idx+4, par_vertex_idx+2);
    tmpl->AddTriangle (prev_par_idx+4, par_vertex_idx+0, par_vertex_idx+1);
    tmpl->AddTriangle (prev_par_idx+4, prev_par_idx+3, par_vertex_idx+0);
  }
  // Create base triangles
  tmpl->AddTriangle (par_vertex_idx+0, par_vertex_idx+5, par_vertex_idx+3);
  tmpl->AddTriangle (par_vertex_idx+0, par_vertex_idx+2, par_vertex_idx+5);
  tmpl->AddTriangle (par_vertex_idx+2, par_vertex_idx+4, par_vertex_idx+5);
  tmpl->AddTriangle (par_vertex_idx+2, par_vertex_idx+1, par_vertex_idx+4);
  tmpl->AddTriangle (par_vertex_idx+1, par_vertex_idx+3, par_vertex_idx+4);
  tmpl->AddTriangle (par_vertex_idx+1, par_vertex_idx+0, par_vertex_idx+3);

  if (recursion >= maxdepth) return;
  csSkeletonConnection* con;
  int i;
  for (i = 0 ; i < width ; i++)
  {
    con = new csSkeletonConnection ();
    parent->AddChild (con);
    csMatrix3 tr = csYRotMatrix3 (0) *
    	csZRotMatrix3 (.15) *
	csXRotMatrix3 (.15);
    csTransform trans (tr, -tr.GetInverse () * csVector3 (0, .5, 0));
    con->SetTransformation (trans);
    add_ghost_limbs (tmpl, frame, con, vertex_idx, par_vertex_idx,
    	maxdepth, 1, recursion+1, dim * .7);
  }
}

// Create a skeletal ghost.
csSkeleton* create_skelghost (csSpriteTemplate* tmpl, csFrame* frame,
	int& vertex_idx, int maxdepth, int width)
{
  csSkeleton* skel = new csSkeleton ();
  add_ghost_limbs (tmpl, frame, skel, vertex_idx, 0, maxdepth, width, 0, .2);
  return skel;
}

// Animate a skeleton.
void animate_skeleton_ghost (csSkeletonLimbState* limb)
{
  csSkeletonConnectionState* con = (csSkeletonConnectionState*)limb->
  	GetChildren ();
  while (con)
  {
    GhostSkelSpriteInfo* o = (GhostSkelSpriteInfo*)con->GetChild
    	(GhostSkelSpriteInfo::Type);
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
      con->ObjAdd (o);
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
    con->SetTransformation (trans);
    animate_skeleton_ghost (con);
    con = (csSkeletonConnectionState*)(con->GetNext ());
  }
}

void animate_skeleton_ghost_cb (csSprite3D* spr, csRenderView* /*rview*/)
{
  csSkeletonState* sk_state = spr->GetSkeletonState ();
  animate_skeleton_ghost (sk_state);
}


// Add a skeletal ghost sprite. If needed it will also create
// the template for this.
void add_skeleton_ghost (csSector* where, csVector3 const& pos, int maxdepth,
	int width)
{
  char skelname[50];
  sprintf (skelname, "__skelghost__\n");
  csSpriteTemplate* tmpl = (csSpriteTemplate*)
  	Sys->view->GetWorld ()->sprite_templates.FindByName (skelname);
  if (!tmpl)
  {
    tmpl = new csSpriteTemplate ();
    tmpl->SetName (skelname);
    Sys->world->sprite_templates.Push (tmpl);
    tmpl->SetMaterial (Sys->world->GetMaterials ()->FindByName ("green"));
    int vertex_idx = 0;
    csFrame* fr = tmpl->AddFrame ();
    fr->SetName ("f");
    csSpriteAction* act = tmpl->AddAction ();
    act->SetName ("a");
    act->AddFrame (fr, 100);
    tmpl->SetSkeleton (create_skelghost (tmpl, fr, vertex_idx, maxdepth, width));
    tmpl->GenerateLOD ();
    tmpl->ComputeBoundingBox ();
  }
  csSprite3D* spr = add_sprite (skelname, "__skelghost__", where, pos, 1);
  spr->SetMixmode (CS_FX_SETALPHA (0.75));
  iPolygonMesh* mesh = QUERY_INTERFACE (spr, iPolygonMesh);
  (void)new csCollider (*spr, Sys->collide_system, mesh);
  GhostSpriteInfo* gh_info = new GhostSpriteInfo ();
  spr->ObjAdd (gh_info);
  gh_info->dir = 1;
  spr->SetDrawCallback (animate_skeleton_ghost_cb);
}

#define MAXSECTORSOCCUPIED  20

extern int FindSectors (csVector3 v, csVector3 d, csSector *s, csSector **sa);
extern int CollisionDetect (csCollider *c, csSector* sp, csTransform *cdt);
extern csCollisionPair our_cd_contact[1000];//=0;
extern int num_our_cd;

void move_ghost (csSprite3D* spr)
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
    Sys->view->GetWorld ()->AddDynLight (dyn);
    dyn->SetSector (where);
    dyn->Setup ();
  }
  csSpriteTemplate* tmpl = (csSpriteTemplate*)
  	Sys->view->GetWorld ()->sprite_templates.FindByName ("bot");
  if (!tmpl) return;
  Bot* bot;
  bot = new Bot (tmpl, Sys->view->GetWorld());
  bot->SetName ("bot");
  Sys->view->GetWorld ()->sprites.Push (bot);
  bot->GetMovable ().SetSector (where);
  csMatrix3 m; m.Identity (); m = m * size;
  bot->GetMovable ().SetTransform (m);
  bot->set_bot_move (pos);
  bot->set_bot_sector (where);
  bot->GetMovable ().UpdateMove ();
  bot->SetAction ("default");
  bot->InitSprite ();
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
    Sys->view->GetWorld ()->RemoveSprite (bot);
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
  csSprite3D* sprite;
  iSoundSource *snd;
};

struct ExplosionStruct
{
  int type;		// type == DYN_TYPE_EXPLOSION
  float radius;
  int dir;
  iSoundSource *snd;
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
	  Sys->view->GetWorld ()->RemoveSprite (ms->sprite);
	}
        dyn->ObjRemove(dyn->GetChild (csDataObject::Type));
        delete ms;
        ExplosionStruct* es = new ExplosionStruct;
        if (Sys->Sound)
          if ((es->snd = Sys->Sound->CreateSource (Sys->wMissile_boom, true)))
          {
            es->snd->SetPosition (v);
            es->snd->Play();
          }
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
          if (Sys->Sound && es->snd)
	  {
            es->snd->Stop ();
            es->snd->DecRef ();
	  }
	  delete es;
          Sys->view->GetWorld ()->RemoveDynLight (dyn);
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
  Sys->view->GetWorld ()->AddDynLight (dyn);
  dyn->SetSector (Sys->view->GetCamera ()->GetSector ());
  dyn->Setup ();
  MissileStruct* ms = new MissileStruct;
  if (Sys->Sound)
    if ((ms->snd = Sys->Sound->CreateSource (Sys->wMissile_whoosh, true)))
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

  csSpriteTemplate* tmpl = (csSpriteTemplate*)
  	Sys->view->GetWorld ()->sprite_templates.FindByName (misname);
  if (!tmpl)
    Sys->Printf (MSG_CONSOLE, "Could not find '%s' sprite template!\n", misname);
  else
  {
    csSprite3D* sp = tmpl->NewSprite (Sys->view->GetWorld ());
    sp->SetName ("missile");
    Sys->view->GetWorld ()->sprites.Push (sp);
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
  csWorld *w = Sys->view->GetWorld ();
  for (int i = 0 ; i < w->sprites.Length () ; i++)
  {
    csSprite *sp = (csSprite*)w->sprites [i];
    if (sp->GetType () == csSprite3D::Type)
    {
      csSprite3D *sp3d = (csSprite3D *)sp;
      csSkeletonState* sk_state = sp3d->GetSkeletonState ();
      if (sk_state)
      {
        const char* name = sp3d->GetName ();
        if (!strcmp (name, "__skelghost__")) move_ghost (sp3d);
      }
    }
    sp->DeferUpdateLighting (CS_NLIGHT_STATIC|CS_NLIGHT_DYNAMIC, 10);
  }
}

//===========================================================================

void OpenPortal (csView* view, char* lev)
{
  csSector* room = view->GetCamera ()->GetSector ();
  csVector3 pos = view->GetCamera ()->Camera2World (csVector3 (0, 0, 1));
  csMaterialWrapper* tm = Sys->world->GetMaterials ()->FindByName ("spark");
  csThing* thing = new csThing (Sys->world);
  Sys->world->things.Push (thing);
  thing->SetName ("portal");
  thing->GetMovable ().SetSector (room);
  float dx = 1, dy = 3, dz = .3;

  csPolygon3D* p;
  p = thing->NewPolygon (tm);
  p->AddVertex (-dx, -1, dz);
  p->AddVertex (dx, -1, dz);
  p->AddVertex (dx, -1, -dz);
  p->AddVertex (-dx, -1, -dz);
  p->SetTextureSpace (p->Vobj (0), p->Vobj (1), 3);

  p = thing->NewPolygon (tm);
  p->AddVertex (-dx, dy-1, -dz);
  p->AddVertex (dx, dy-1, -dz);
  p->AddVertex (dx, dy-1, dz);
  p->AddVertex (-dx, dy-1, dz);
  p->SetTextureSpace (p->Vobj (0), p->Vobj (1), 3);

  p = thing->NewPolygon (tm);
  p->AddVertex (-dx, dy-1, dz);
  p->AddVertex (dx, dy-1, dz);
  p->AddVertex (dx, -1, dz);
  p->AddVertex (-dx, -1, dz);
  p->SetTextureSpace (p->Vobj (0), p->Vobj (1), 3);

  p = thing->NewPolygon (tm);
  p->AddVertex (dx, dy-1, dz);
  p->AddVertex (dx, dy-1, -dz);
  p->AddVertex (dx, -1, -dz);
  p->AddVertex (dx, -1, dz);
  p->SetTextureSpace (p->Vobj (0), p->Vobj (1), 3);

  p = thing->NewPolygon (tm);
  p->AddVertex (-dx, dy-1, -dz);
  p->AddVertex (-dx, dy-1, dz);
  p->AddVertex (-dx, -1, dz);
  p->AddVertex (-dx, -1, -dz);
  p->SetTextureSpace (p->Vobj (0), p->Vobj (1), 3);

  p = thing->NewPolygon (tm);
  p->AddVertex (dx, dy-1, -dz);
  p->AddVertex (-dx, dy-1, -dz);
  p->AddVertex (-dx, -1, -dz);
  p->AddVertex (dx, -1, -dz);
  p->SetTextureSpace (p->Vobj (0), p->Vobj (1), 3);

  thing->Prepare (room);
  thing->InitLightMaps (false);
  room->ShineLights (thing);
  thing->CreateLightMaps (Sys->G3D);

  thing->GetMovable ().SetPosition (pos);
  thing->GetMovable ().UpdateMove ();

  Sys->world->SelectRegion (lev);
  // @@@ No error checking!
  char buf[255];
  sprintf (buf, "/lev/%s", lev);
  Sys->VFS->ChDir (buf);
  csLoader::AppendWorldFile (Sys->world, "world");
  Sys->world->GetCsCurrentRegion ()->Prepare ();
  Sys->world->SelectRegion (NULL);
}

