/*
    Copyright (C) 2001 by Jorrit Tyberghein

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
#include "csgeom/math3d.h"
#include "stars.h"
#include "iengine/movable.h"
#include "iengine/rview.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "ivideo/txtmgr.h"
#include "ivideo/material.h"
#include "iengine/material.h"
#include "iengine/camera.h"
#include "igeom/clip2d.h"
#include "iengine/engine.h"
#include "iengine/light.h"
#include "qsqrt.h"
#include "qint.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE (csStarsMeshObject)
  SCF_IMPLEMENTS_INTERFACE (iMeshObject)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObjectModel)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iStarsState)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csStarsMeshObject::ObjectModel)
  SCF_IMPLEMENTS_INTERFACE (iObjectModel)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csStarsMeshObject::StarsState)
  SCF_IMPLEMENTS_INTERFACE (iStarsState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csStarsMeshObject::csStarsMeshObject (iMeshObjectFactory* factory)
{
  SCF_CONSTRUCT_IBASE (NULL);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObjectModel);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiStarsState);
  csStarsMeshObject::factory = factory;
  logparent = NULL;
  initialized = false;
  box.Set (csVector3 (-10, -10, -10), csVector3 (10, 10, 10));
  vis_cb = NULL;
  shapenr = 0;
  color.red = 1;
  color.green = 1;
  color.blue = 1;
  use_max_color = false;
  max_color.Set(0,0,0);
  max_dist = 20;
  density = .1;
  seed = 3939394;
  current_lod = 1;
  current_features = 0;
}

csStarsMeshObject::~csStarsMeshObject ()
{
  if (vis_cb) vis_cb->DecRef ();
}

void csStarsMeshObject::SetupObject ()
{
  if (!initialized)
  {
    initialized = true;
  }
}

bool csStarsMeshObject::DrawTest (iRenderView*, iMovable*)
{
  SetupObject ();
  return true;
}

void csStarsMeshObject::UpdateLighting (iLight**, int,
    iMovable*)
{
  SetupObject ();
  return;
}

static void Perspective (const csVector3& v, csVector3& p, float fov,
    	float sx, float sy)
{
  p.z = 1. / v.z;
  float iza = fov * p.z;
  p.x = v.x * iza + sx;
  p.y = v.y * iza + sy;
}

float csStarsMeshObject::GetRandom(float max)
{
  return max * (rand()/(RAND_MAX+1.0));
}

void csStarsMeshObject::DrawPoint(iRenderView *rview,
  const csVector3& pos, const csColor& col, csZBufMode zbufmode)
{
  iGraphics2D *g2d = rview->GetGraphics2D();
  iGraphics3D *g3d = rview->GetGraphics3D();
  int x = QInt(pos.x);
  int y = QInt(pos.y);
  // clip to screen and to clipper
  if(x < 0.0 || y < 0.0 || x >= g2d->GetWidth() || y >= g2d->GetHeight())
    return;
  if(!rview->GetClipper()->IsInside(csVector2(pos.x, pos.y)))
    return;

  // test zbuffer?
  if(zbufmode & CS_ZBUF_TEST) // tests if zbuf is ZBUF_TEST or ZBUF_USE
  {
    float atpoint = g3d->GetZBuffValue(x, y);
    if(pos.z < atpoint) return;
  }

  // draw
  int colidx = g3d->GetTextureManager()->FindRGB(QInt(col.red * 255),
    QInt(col.green*255), QInt(col.blue*255));
  g2d->DrawPixel(x, y, colidx);

}

void csStarsMeshObject::DrawStarBox (iRenderView* rview,
  const csReversibleTransform &tr_o2c, csZBufMode zbufmode,
  csBox3& starbox, const csVector3& origin)
{
  iCamera* camera = rview->GetCamera ();
  float fov = camera->GetFOV ();
  float shiftx = camera->GetShiftX ();
  float shifty = camera->GetShiftY ();

  float sqmaxdist = max_dist * max_dist;

  /// primitive box clipping
  if(!starbox.In(origin)
    && (starbox.GetCorner(0)-origin).SquaredNorm() > sqmaxdist
    && (starbox.GetCorner(1)-origin).SquaredNorm() > sqmaxdist
    && (starbox.GetCorner(2)-origin).SquaredNorm() > sqmaxdist
    && (starbox.GetCorner(3)-origin).SquaredNorm() > sqmaxdist
    && (starbox.GetCorner(4)-origin).SquaredNorm() > sqmaxdist
    && (starbox.GetCorner(5)-origin).SquaredNorm() > sqmaxdist
    && (starbox.GetCorner(6)-origin).SquaredNorm() > sqmaxdist
    && (starbox.GetCorner(7)-origin).SquaredNorm() > sqmaxdist
    )
    return;

  unsigned int starseed = seed ^ QInt(starbox.Min().x) ^ QInt(starbox.Min().y)
    ^ QInt(starbox.Min().z);
  srand(starseed);

  csVector3 boxsize = starbox.Max() - starbox.Min();
  int number = 100; // number of stars is volume * density
  //printf("boxsize.x %g %g %g\n", boxsize.x, boxsize.y, boxsize.z);
  number = QInt(
    boxsize.x * boxsize.y * boxsize.z * density
    * ( GetRandom(0.4) + 0.8 ) /// * 0.8 ... * 1.2, so +- 20%
    );
  //printf("number is %d\n", number);

  csVector3 pos; // star position in 3d
  csVector3 screenpos;  // star position on screen
  csColor starcolor = color; // color of the star
  bool color_per_box = false; // debug coloring

  if(color_per_box)
  {
  starcolor.red = 0.2 + GetRandom(0.8);
  starcolor.green = 0.2 + GetRandom(0.8);
  starcolor.blue = 0.2 + GetRandom(0.8);
  }
  int i;
  int drawn = 0;
  for (i = 0 ; i < number ; i++)
  {
    pos = starbox.Min();
    pos += csVector3( GetRandom(boxsize.x), GetRandom(boxsize.y),
      GetRandom(boxsize.z));
    Perspective(tr_o2c*pos, screenpos, fov, shiftx, shifty);
    float sqdist = (pos-origin).SquaredNorm();
    if(!color_per_box)
    {
      starcolor = color;
      starcolor.red += -0.3 + GetRandom(0.6);
      starcolor.green += -0.3 + GetRandom(0.6);
      starcolor.blue += -0.3 + GetRandom(0.6);
      starcolor.Clamp(1.,1.,1.);
      starcolor.ClampDown();
    }
    if(sqdist > sqmaxdist) continue;
    if(screenpos.z <= SMALL_Z) continue;
    if(use_max_color) {
      /// fade to max dist
      float fadeamt = sqdist / sqmaxdist;
      starcolor = starcolor * (1.0-fadeamt) + fadeamt * max_color;
    }
    DrawPoint(rview, screenpos, starcolor, zbufmode);
    drawn++;
  }
  //printf("drawn is %d\n", drawn);

}

bool csStarsMeshObject::Draw (iRenderView* rview, iMovable* movable,
	csZBufMode zbufmode)
{
  if (vis_cb) if (!vis_cb->BeforeDrawing (this, rview)) return false;

  iCamera* camera = rview->GetCamera ();
  csReversibleTransform tr_o2c = camera->GetTransform () / movable->GetFullTransform ();
  const csVector3& origin = movable->GetFullTransform ().GetInverse () *
    camera->GetTransform().GetOrigin();

  if(max_dist <= 0.0) return false; // protect against divide by zero
  srand (seed);

  /// walk in boxes around origin
  /// snap boxes to a grid
  const float gridsize = 20;
  csVector3 boxsize = box.Max() - box.Min();
  int nr_x = QInt(gridsize * max_dist / boxsize.x)+1;
  int nr_y = QInt(gridsize * max_dist / boxsize.y)+1;
  int nr_z = QInt(gridsize * max_dist / boxsize.z)+1;
  csVector3 starboxsize( boxsize.x / gridsize, boxsize.y / gridsize,
    boxsize.z / gridsize );
  csVector3 starmin = box.Min();
  starmin.x += QInt((origin.x-box.Min().x)/starboxsize.x)*starboxsize.x;
  starmin.y += QInt((origin.y-box.Min().y)/starboxsize.y)*starboxsize.y;
  starmin.z += QInt((origin.z-box.Min().z)/starboxsize.z)*starboxsize.z;
  csBox3 starbox(starmin, starmin+starboxsize);
  csBox3 passbox;

  for(int x = -nr_x; x<=nr_x; x++)
   for(int y = -nr_y; y<=nr_y; y++)
    for(int z = -nr_z; z<=nr_z; z++)
    {
      csVector3 offset(x*starboxsize.x,y*starboxsize.y,z*starboxsize.z);
      passbox.Set(starbox.Min()+offset, starbox.Max()+offset);
      DrawStarBox (rview, tr_o2c, zbufmode, passbox, origin);
    }

  /// before exiting, set the seed to be pretty random again
  static unsigned int funkyrand = 0;
  if(funkyrand==0) funkyrand = (unsigned int) time(0);
  srand(funkyrand++);

  return true;
}

void csStarsMeshObject::GetObjectBoundingBox (csBox3& b, int /*type*/)
{
  SetupObject ();
  b = box;
}

//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csStarsMeshObjectFactory)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectFactory)
SCF_IMPLEMENT_IBASE_END

csStarsMeshObjectFactory::csStarsMeshObjectFactory (iBase *pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  logparent = NULL;
}

csStarsMeshObjectFactory::~csStarsMeshObjectFactory ()
{
}

csPtr<iMeshObject> csStarsMeshObjectFactory::NewInstance ()
{
  csStarsMeshObject* cm = new csStarsMeshObject ((iMeshObjectFactory*)this);
  csRef<iMeshObject> im (SCF_QUERY_INTERFACE (cm, iMeshObject));
  cm->DecRef ();
  return csPtr<iMeshObject> (im);
}

//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csStarsMeshObjectType)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectType)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csStarsMeshObjectType::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csStarsMeshObjectType)

SCF_EXPORT_CLASS_TABLE (stars)
  SCF_EXPORT_CLASS (csStarsMeshObjectType, "crystalspace.mesh.object.stars",
    "Crystal Space Stars Mesh Type")
SCF_EXPORT_CLASS_TABLE_END

csStarsMeshObjectType::csStarsMeshObjectType (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csStarsMeshObjectType::~csStarsMeshObjectType ()
{
}

csPtr<iMeshObjectFactory> csStarsMeshObjectType::NewFactory ()
{
  csStarsMeshObjectFactory* cm = new csStarsMeshObjectFactory (this);
  csRef<iMeshObjectFactory> ifact (
  	SCF_QUERY_INTERFACE (cm, iMeshObjectFactory));
  cm->DecRef ();
  return csPtr<iMeshObjectFactory> (ifact);
}

