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
#include "csgeom/math3d.h"
#include "csgeom/math2d.h"
#include "csgeom/box.h"
#include "spr2d.h"
#include "iengine/movable.h"
#include "iengine/rview.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "ivideo/material.h"
#include "iengine/material.h"
#include "iengine/camera.h"
#include "igeom/clip2d.h"
#include "iengine/engine.h"
#include "iengine/light.h"
#include "qsqrt.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE (csSprite2DMeshObject)
  SCF_IMPLEMENTS_INTERFACE (iMeshObject)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObjectModel)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iSprite2DState)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iParticle)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite2DMeshObject::ObjectModel)
  SCF_IMPLEMENTS_INTERFACE (iObjectModel)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite2DMeshObject::Sprite2DState)
  SCF_IMPLEMENTS_INTERFACE (iSprite2DState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite2DMeshObject::Particle)
  SCF_IMPLEMENTS_INTERFACE (iParticle)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csSprite2DMeshObject::csSprite2DMeshObject (csSprite2DMeshObjectFactory* factory)
{
  SCF_CONSTRUCT_IBASE (NULL);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObjectModel);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSprite2DState);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiParticle);
  csSprite2DMeshObject::factory = factory;
  logparent = NULL;
  ifactory = SCF_QUERY_INTERFACE (factory, iMeshObjectFactory);
  material = factory->GetMaterialWrapper ();
  lighting = factory->HasLighting ();
  MixMode = factory->GetMixMode ();
  initialized = false;
  vis_cb = NULL;
  current_lod = 1;
  current_features = 0;
  uvani = NULL;
}

csSprite2DMeshObject::~csSprite2DMeshObject ()
{
  if (vis_cb) vis_cb->DecRef ();
  delete uvani;
}

void csSprite2DMeshObject::SetupObject ()
{
  if (!initialized)
  {
    initialized = true;
    float max_sq_dist = 0;
    int i;
    bbox_2d.StartBoundingBox(vertices[0].pos);
    for (i = 0 ; i < vertices.Length () ; i++)
    {
      csSprite2DVertex& v = vertices[i];
      bbox_2d.AddBoundingVertexSmart(v.pos);
      if (!lighting)
      {
        // If there is no lighting then we need to copy the color_init
        // array to color.
        v.color = vertices[i].color_init;
        v.color.Clamp (2, 2, 2);
      }
      float sqdist = v.pos.x*v.pos.x + v.pos.y*v.pos.y;
      if (sqdist > max_sq_dist) max_sq_dist = sqdist;
    }
    float max_dist = qsqrt (max_sq_dist);
    radius.Set (max_dist, max_dist, max_dist);
  }
}

static csVector3 cam;

bool csSprite2DMeshObject::DrawTest (iRenderView* rview, iMovable* movable)
{
  SetupObject ();

  // Camera transformation for the single 'position' vector.
  cam = rview->GetCamera ()->GetTransform ().Other2This (
  	movable->GetFullPosition ());
  if (cam.z < SMALL_Z) return false;
  return true;
}

void csSprite2DMeshObject::UpdateLighting (iLight** lights, int num_lights,
    const csVector3& pos)
{
  SetupObject ();
  if (!lighting) return;
  csColor color (0, 0, 0);

  // @@@ GET AMBIENT
  //csSector* sect = movable.GetSector (0);
  //if (sect)
  //{
    //int r, g, b;
    //sect->GetAmbientColor (r, g, b);
    //color.Set (r / 128.0, g / 128.0, b / 128.0);
  //}

  int i;
  for (i = 0; i < num_lights; i++)
  {
    csColor light_color = lights[i]->GetColor () * (256. / CS_NORMAL_LIGHT_LEVEL);
    float sq_light_radius = lights [i]->GetSquaredRadius ();
    // Compute light position.
    csVector3 wor_light_pos = lights [i]->GetCenter ();
    float wor_sq_dist =
      csSquaredDist::PointPoint (wor_light_pos, pos);
    if (wor_sq_dist >= sq_light_radius) continue;
    float wor_dist = qsqrt (wor_sq_dist);
    float cosinus = 1.;
    cosinus /= wor_dist;
    light_color *= cosinus * lights [i]->GetBrightnessAtDistance (wor_dist);
    color += light_color;
  }
  for (i = 0 ; i < vertices.Length () ; i++)
  {
    vertices[i].color = vertices[i].color_init + color;
    vertices[i].color.Clamp (2, 2, 2);
  }
}

void csSprite2DMeshObject::UpdateLighting (iLight** lights, int num_lights,
    iMovable* movable)
{
  if (!lighting) return;
  csVector3 pos = movable->GetFullPosition ();
  UpdateLighting (lights, num_lights, pos);
}

#define INTERPOLATE1_S(var) \
  g3dpoly->var [i]= inpoly_##var [vt]+ \
    t * (inpoly_##var [vt2]- inpoly_##var [vt]);

#define INTERPOLATE1(var,component) \
  g3dpoly->var [i].component = inpoly_##var [vt].component + \
    t * (inpoly_##var [vt2].component - inpoly_##var [vt].component);

#define INTERPOLATE_S(var) \
{ \
  float v1 = inpoly_##var [edge_from [0]] + \
    t1 * (inpoly_##var [edge_to [0]] - inpoly_##var [edge_from [0]]); \
  float v2 = inpoly_##var [edge_from [1]] + \
    t2 * (inpoly_##var [edge_to [1]] - inpoly_##var [edge_from [1]]); \
  g3dpoly->var [i] = v1 + t * (v2 - v1); \
}

#define INTERPOLATE(var,component) \
{ \
  float v1 = inpoly_##var [edge_from [0]].component + \
    t1 * (inpoly_##var [edge_to [0]].component - inpoly_##var [edge_from [0]].component); \
  float v2 = inpoly_##var [edge_from [1]].component + \
    t2 * (inpoly_##var [edge_to [1]].component - inpoly_##var [edge_from [1]].component); \
  g3dpoly->var [i].component = v1 + t * (v2 - v1); \
}

static void PreparePolygonFX2 (G3DPolygonDPFX* g3dpoly,
  csVector2* clipped_verts, int num_vertices, csVertexStatus* clipped_vtstats,
  int orig_num_vertices, bool gouraud)
{
  // first we copy the first texture coordinates to a local buffer
  // to avoid that they are overwritten when interpolating.
  CS_ALLOC_STACK_ARRAY (csVector2, inpoly_vertices, orig_num_vertices);
  CS_ALLOC_STACK_ARRAY (csVector2, inpoly_texels, orig_num_vertices);
  CS_ALLOC_STACK_ARRAY (csColor, inpoly_colors, orig_num_vertices);
  CS_ALLOC_STACK_ARRAY (float, inpoly_z, orig_num_vertices);
  int i;
  memcpy (inpoly_vertices, g3dpoly->vertices,
  	orig_num_vertices * sizeof (csVector2));
  memcpy (inpoly_texels, g3dpoly->texels,
  	orig_num_vertices * sizeof (csVector2));
  memcpy (inpoly_colors, g3dpoly->colors, orig_num_vertices * sizeof (csColor));
  memcpy (inpoly_z, g3dpoly->z, orig_num_vertices * sizeof (float));

  int vt, vt2;
  float t;
  for (i = 0; i < num_vertices; i++)
  {
    g3dpoly->vertices [i] = clipped_verts [i];
    switch (clipped_vtstats[i].Type)
    {
      case CS_VERTEX_ORIGINAL:
        vt = clipped_vtstats[i].Vertex;
        g3dpoly->z [i] = inpoly_z [vt];
        g3dpoly->texels [i] = inpoly_texels [vt];
	if (gouraud)
          g3dpoly->colors [i] = inpoly_colors [vt];
	break;
      case CS_VERTEX_ONEDGE:
        vt = clipped_vtstats[i].Vertex;
	vt2 = vt + 1; if (vt2 >= orig_num_vertices) vt2 = 0;
	t = clipped_vtstats [i].Pos;
	INTERPOLATE1_S (z);
	INTERPOLATE1 (texels,x);
	INTERPOLATE1 (texels,y);
	if (gouraud)
	{
	  INTERPOLATE1 (colors,red);
	  INTERPOLATE1 (colors,green);
	  INTERPOLATE1 (colors,blue);
	}
	break;
      case CS_VERTEX_INSIDE:
        float x = clipped_verts [i].x;
        float y = clipped_verts [i].y;
        int edge_from [2], edge_to [2];
	int edge = 0;
	int j, j1;
	j1 = orig_num_vertices - 1;
	for (j = 0; j < orig_num_vertices; j++)
	{
          if ((y >= inpoly_vertices [j].y && y <= inpoly_vertices [j1].y) ||
	      (y <= inpoly_vertices [j].y && y >= inpoly_vertices [j1].y))
	  {
	    edge_from [edge] = j;
	    edge_to [edge] = j1;
	    edge++;
	    if (edge >= 2) break;
	  }
	  j1 = j;
	}
	if (edge == 1)
	{
	  // Safety if we only found one edge.
	  edge_from [1] = edge_from [0];
	  edge_to [1] = edge_to [0];
	}
	csVector2& A = inpoly_vertices [edge_from [0]];
	csVector2& B = inpoly_vertices [edge_to [0]];
	csVector2& C = inpoly_vertices [edge_from [1]];
	csVector2& D = inpoly_vertices [edge_to [1]];
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
	break;
    }
  }
}

#undef INTERPOLATE
#undef INTERPOLATE1

bool csSprite2DMeshObject::Draw (iRenderView* rview, iMovable* /*movable*/,
	csZBufMode mode)
{
// @@@ TODO:
//     - Z fill vs Z use
  if (!material)
  {
    printf ("INTERNAL ERROR: sprite2D used without material!\n");
    return false;
  }
  iMaterialHandle* mat = material->GetMaterialHandle ();
  if (!mat)
  {
    printf ("INTERNAL ERROR: sprite2D used without valid material handle!\n");
    return false;
  }

  if (vis_cb) if (!vis_cb->BeforeDrawing (this, rview)) return false;

  iGraphics3D* g3d = rview->GetGraphics3D ();
  iCamera* camera = rview->GetCamera ();

  // Prepare for rendering.
  g3d->SetRenderState (G3DRENDERSTATE_ZBUFFERMODE, mode);

  material->Visit ();

  G3DPolygonDPFX& g3dpolyfx = factory->g3dpolyfx;
  g3dpolyfx.num = vertices.Length ();
  g3dpolyfx.mat_handle = mat;
  g3dpolyfx.mat_handle->GetTexture ()->GetMeanColor (g3dpolyfx.flat_color_r,
    g3dpolyfx.flat_color_g, g3dpolyfx.flat_color_b);

  CS_ALLOC_STACK_ARRAY (csVector2, poly2d, vertices.Length ());
  csVector2 clipped_poly2d[MAX_OUTPUT_VERTICES];
  csVertexStatus clipped_vtstats[MAX_OUTPUT_VERTICES];

  float iz = 1. / cam.z;
  float iza = iz * camera->GetFOV ();

  int i;

  for (i = 0; i < vertices.Length (); i++)
  {
    g3dpolyfx.z [i] = iz;
    g3dpolyfx.vertices [i].x =
    poly2d [i].x = (cam.x + vertices [i].pos.x) * iza + camera->GetShiftX ();
    g3dpolyfx.vertices [i].y =
    poly2d [i].y = (cam.y + vertices [i].pos.y) * iza + camera->GetShiftY ();
    g3dpolyfx.colors [i] = vertices [i].color;
  }

  if (!uvani)
  {
    for (i = 0; i < vertices.Length (); i++)
    {
      g3dpolyfx.texels [i].x = vertices [i].u;
      g3dpolyfx.texels [i].y = vertices [i].v;
    }
  }
  else
  {
    int n;
    const csVector2 *uv = uvani->GetVertices (n);
    for (i = 0; i < n; i++)
    {
      g3dpolyfx.texels [i].x = uv [i].x;
      g3dpolyfx.texels [i].y = uv [i].y;
    }
  }

  int num_clipped_verts;
  uint8 clip_result = rview->GetClipper ()->Clip (poly2d, vertices.Length (),
    clipped_poly2d, num_clipped_verts, clipped_vtstats);
  if (clip_result == CS_CLIP_OUTSIDE) return false;
  g3dpolyfx.num = num_clipped_verts;

  if (clip_result != CS_CLIP_INSIDE)
    PreparePolygonFX2 (&g3dpolyfx, clipped_poly2d, num_clipped_verts,
    	clipped_vtstats, vertices.Length (), true);

  rview->CalculateFogPolygon (g3dpolyfx);
  g3dpolyfx.mixmode = MixMode | CS_FX_GOURAUD;
  g3d->DrawPolygonFX (g3dpolyfx);

  return true;
}

void csSprite2DMeshObject::GetObjectBoundingBox (csBox3& bbox, int /*type*/)
{
  SetupObject ();
  //@@@ TODO. This is an initial test....and is invalid
  csVector2 st = bbox_2d.Min(), end = bbox_2d.Max();
  bbox.StartBoundingBox(csVector3(st.x,st.y,0));
  bbox.AddBoundingVertexSmart(csVector3(end.x,end.y,0));
}

void csSprite2DMeshObject::HardTransform (const csReversibleTransform& t)
{
  (void)t;
  //@@@ TODO
}

void csSprite2DMeshObject::CreateRegularVertices (int n, bool setuv)
{
  double angle_inc = TWO_PI / n;
  double angle = 0.0;
  vertices.SetLimit (n);
  vertices.SetLength (n);
  int i;
  for (i = 0; i < vertices.Length (); i++, angle += angle_inc)
  {
    vertices [i].pos.y = cos (angle);
    vertices [i].pos.x = sin (angle);
    if (setuv)
    {
      // reuse sin/cos values and scale to [0..1]
      vertices [i].u = vertices [i].pos.x / 2.0f + 0.5f;
      vertices [i].v = vertices [i].pos.y / 2.0f + 0.5f;
    }
    vertices [i].color.Set (1, 1, 1);
    vertices [i].color_init.Set (1, 1, 1);
  }
  scfiObjectModel.ShapeChanged ();
}

void csSprite2DMeshObject::NextFrame (csTicks current_time, const csVector3& /*pos*/)
{
  if (uvani && !uvani->halted)
    uvani->Advance (current_time);
}

void csSprite2DMeshObject::Particle::UpdateLighting (iLight** lights,
    int num_lights, const csReversibleTransform& transform)
{
  csVector3 new_pos = transform.This2Other (part_pos);
  scfParent->UpdateLighting (lights, num_lights, new_pos);
}

void csSprite2DMeshObject::Particle::Draw (iRenderView* rview,
    const csReversibleTransform& transform, csZBufMode mode)
{
  scfParent->SetupObject ();

  // Camera transformation for the single 'position' vector.
  csVector3 new_pos = transform.This2Other (part_pos);
  cam = rview->GetCamera ()->GetTransform ().Other2This (new_pos);
  if (cam.z < SMALL_Z) return;
  scfParent->Draw (rview, NULL, mode);
}

void csSprite2DMeshObject::Particle::SetColor (const csColor& col)
{
  csColoredVertices& vertices = scfParent->GetVertices ();
  int i;
  for (i = 0 ; i < vertices.Length () ; i++)
    vertices[i].color_init = col;
  if (!scfParent->lighting)
    for (i = 0 ; i < vertices.Length () ; i++)
      vertices[i].color = col;
}

void csSprite2DMeshObject::Particle::AddColor (const csColor& col)
{
  csColoredVertices& vertices = scfParent->GetVertices ();
  int i;
  for (i = 0 ; i < vertices.Length () ; i++)
    vertices[i].color_init += col;
  if (!scfParent->lighting)
    for (i = 0 ; i < vertices.Length () ; i++)
      vertices[i].color = vertices[i].color_init;
}

void csSprite2DMeshObject::Particle::ScaleBy (float factor)
{
  csColoredVertices& vertices = scfParent->GetVertices ();
  int i;
  for (i = 0; i < vertices.Length (); i++)
    vertices[i].pos *= factor;
  scfParent->scfiObjectModel.ShapeChanged ();
}

void csSprite2DMeshObject::Particle::Rotate (float angle)
{
  csColoredVertices& vertices = scfParent->GetVertices ();
  int i;
  for (i = 0; i < vertices.Length (); i++)
    vertices[i].pos.Rotate (angle);
  scfParent->scfiObjectModel.ShapeChanged ();
}

void csSprite2DMeshObject::Sprite2DState::SetUVAnimation (const char *name,
	int style, bool loop)
{
  if (name)
  {
    iSprite2DUVAnimation *ani = scfParent->factory->GetUVAnimation (name);
    if (ani && ani->GetFrameCount ())
    {
      scfParent->uvani = new uvAnimationControl ();
      scfParent->uvani->ani = ani;
      scfParent->uvani->last_time = 0;
      scfParent->uvani->frameindex = 0;
      scfParent->uvani->framecount = ani->GetFrameCount ();
      scfParent->uvani->frame = ani->GetFrame (0);
      scfParent->uvani->style = style;
      scfParent->uvani->counter = 0;
      scfParent->uvani->loop = loop;
      scfParent->uvani->halted = false;
    }
  }
  else
  {
    // stop animation and show the normal texture
    delete scfParent->uvani;
    scfParent->uvani = NULL;
  }
}

void csSprite2DMeshObject::Sprite2DState::StopUVAnimation (int idx)
{
  if (scfParent->uvani)
  {
    if (idx != -1)
    {
      scfParent->uvani->frameindex = MIN(MAX(idx, 0),
      	scfParent->uvani->framecount-1);
      scfParent->uvani->frame = scfParent->uvani->ani->GetFrame (
      	scfParent->uvani->frameindex);
    }
    scfParent->uvani->halted = true;
  }
}

void csSprite2DMeshObject::Sprite2DState::PlayUVAnimation (int idx, int style, bool loop)
{
  if (scfParent->uvani)
  {
    if (idx != -1)
    {
      scfParent->uvani->frameindex = MIN(MAX(idx, 0), scfParent->uvani->framecount-1);
      scfParent->uvani->frame = scfParent->uvani->ani->GetFrame (scfParent->uvani->frameindex);
    }
    scfParent->uvani->halted = false;
    scfParent->uvani->counter = 0;
    scfParent->uvani->last_time = 0;
    scfParent->uvani->loop = loop;
    scfParent->uvani->style = style;
  }
}

int csSprite2DMeshObject::Sprite2DState::GetUVAnimationCount () const
{
  return scfParent->factory->GetUVAnimationCount ();
}

iSprite2DUVAnimation *csSprite2DMeshObject::Sprite2DState::CreateUVAnimation ()
{
  return scfParent->factory->CreateUVAnimation ();
}

void csSprite2DMeshObject::Sprite2DState::RemoveUVAnimation (
	iSprite2DUVAnimation *anim)
{
  scfParent->factory->RemoveUVAnimation (anim);
}

iSprite2DUVAnimation *csSprite2DMeshObject::Sprite2DState::GetUVAnimation (
	const char *name)
{
  return scfParent->factory->GetUVAnimation (name);
}

iSprite2DUVAnimation *csSprite2DMeshObject::Sprite2DState::GetUVAnimation (
	int idx)
{
  return scfParent->factory->GetUVAnimation (idx);
}


void csSprite2DMeshObject::uvAnimationControl::Advance (csTicks current_time)
{
  int oldframeindex = frameindex;
  // the goal is to find the next frame to show
  if (style < 0)
  { // every (-1*style)-th frame show a new pic
    counter--;
    if (counter < style)
    {
      counter = 0;
      frameindex++;
      if (frameindex == framecount)
	if (loop)
	  frameindex = 0;
	else
	{
	  frameindex = framecount-1;
	  halted = true;
	}
    }
  }
  else
    if (style > 0)
    { // skip to next frame every <style> millisecond
      if (last_time == 0)
	last_time = current_time;
      counter += (current_time - last_time);
      last_time = current_time;
      while (counter > style)
      {
	counter -= style;
	frameindex++;
	if (frameindex == framecount)
	  if (loop)
	    frameindex = 0;
	  else
	    {
	      frameindex = framecount-1;
	      halted = true;
	    }
      }
    }
    else
    { // style == 0 -> use time indices attached to the frames
      if (last_time == 0)
	last_time = current_time;
      while (frame->GetDuration () + last_time < current_time)
      {
	frameindex++;
	if (frameindex == framecount)
	  if (loop)
	  {
	    frameindex = 0;
	  }
	  else
	  {
	    frameindex = framecount-1;
	    halted = true;
	    break;
	  }
	last_time += frame->GetDuration ();
	frame = ani->GetFrame (frameindex);
      }
    }

  if (oldframeindex != frameindex)
    frame = ani->GetFrame (frameindex);

}

const csVector2 *csSprite2DMeshObject::uvAnimationControl::GetVertices (int &num)
{
  num = frame->GetUVCount ();
  return frame->GetUVCoo ();
}

// The hit beam methods in sprite2d make a couple of small presumptions.
// 1) The sprite is always facing the start of the beam.
// 2) Since it is always facing the the beam, only one side
// of its bounding box can be hit (if at all).

void csSprite2DMeshObject::CheckBeam (const csVector3& start,
      const csVector3& pl, float sqr, csMatrix3& o2t)
{
  // This method is an optimized version of LookAt() based on
  // the presumption that the up vector is always (0,1,0).
  // This is used to create a transform to move the intersection
  // to the sprites vector space, then it is tested against the 2d
  // coords, which are conveniently located at z=0.
  // The transformation matrix is stored and used again if the
  // start vector for the beam is in the same position. MHV.

  csVector3 pl2 = pl * qisqrt (sqr);
  csVector3 v1( pl2.z, 0, -pl2.x);
  sqr = v1*v1;
  v1 *= qisqrt(sqr);
  csVector3 v2(pl2.y * v1.z, pl2.z * v1.x - pl2.x * v1.z, -pl2.y * v1.x);
  o2t.Set (v1.x, v2.x, pl2.x,
           v1.y, v2.y, pl2.y,
           v1.z, v2.z, pl2.z);
}


bool csSprite2DMeshObject::HitBeamOutline(const csVector3& start,
      const csVector3& end, csVector3& isect, float* pr)
{
  csVector2 cen = bbox_2d.GetCenter();
  csVector3 pl = start - csVector3(cen.x, cen.y, 0);
  float sqr = pl * pl;
  if (sqr < SMALL_EPSILON) return false; // Too close, Cannot intersect
  float dist;
  csIntersect3::Plane(start, end, pl, 0, isect, dist);
  if (pr) *pr = dist;
  csMatrix3 o2t;
  CheckBeam (start, pl, sqr, o2t);
  csVector3 r = o2t * isect;
  int trail, len = vertices.Length();
  trail = len - 1;
  csVector2 isec(r.x, r.y);
  int i;
  for (i = 0; i < len; trail = i++)
    if (csMath2::WhichSide2D(isec, vertices[trail].pos, vertices[i].pos) > 0)
      return false;
  return true;
}

//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csSprite2DMeshObjectFactory)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectFactory)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iSprite2DFactoryState)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite2DMeshObjectFactory::Sprite2DFactoryState)
  SCF_IMPLEMENTS_INTERFACE (iSprite2DFactoryState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csSprite2DMeshObjectFactory::csSprite2DMeshObjectFactory (iBase *pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSprite2DFactoryState);
  material = NULL;
  MixMode = 0;
  lighting = true;
  logparent = NULL;
}

csSprite2DMeshObjectFactory::~csSprite2DMeshObjectFactory ()
{
}

csPtr<iMeshObject> csSprite2DMeshObjectFactory::NewInstance ()
{
  csSprite2DMeshObject* cm = new csSprite2DMeshObject (this);
  csRef<iMeshObject> im (SCF_QUERY_INTERFACE (cm, iMeshObject));
  cm->DecRef ();
  return csPtr<iMeshObject> (im);
}

//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csSprite2DMeshObjectType)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectType)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite2DMeshObjectType::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csSprite2DMeshObjectType)

SCF_EXPORT_CLASS_TABLE (spr2d)
  SCF_EXPORT_CLASS (csSprite2DMeshObjectType, "crystalspace.mesh.object.sprite.2d",
    "Crystal Space Sprite2D Mesh Type")
SCF_EXPORT_CLASS_TABLE_END

csSprite2DMeshObjectType::csSprite2DMeshObjectType (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csSprite2DMeshObjectType::~csSprite2DMeshObjectType ()
{
}

csPtr<iMeshObjectFactory> csSprite2DMeshObjectType::NewFactory ()
{
  csSprite2DMeshObjectFactory* cm = new csSprite2DMeshObjectFactory (this);
  csRef<iMeshObjectFactory> ifact (
  	SCF_QUERY_INTERFACE (cm, iMeshObjectFactory));
  cm->DecRef ();
  return csPtr<iMeshObjectFactory> (ifact);
}

