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
#include "qint.h"
#include "csengine/thing.h"
#include "csengine/thingtpl.h"
#include "csengine/polygon.h"
#include "csengine/polytmap.h"
#include "csengine/pol2d.h"
#include "csengine/polytext.h"
#include "csengine/light.h"
#include "csengine/world.h"
#include "csengine/stats.h"
#include "csengine/sector.h"
#include "csengine/cbufcube.h"
#include "csengine/covcube.h"
#include "csengine/curve.h"
#include "csengine/texture.h"
#include "igraph3d.h"
#include "itxtmgr.h"
#include "itexture.h"
#include "qint.h"

//---------------------------------------------------------------------------

IMPLEMENT_CSOBJTYPE (csThing,csPolygonSet);

IMPLEMENT_IBASE_EXT (csThing)
  IMPLEMENTS_EMBEDDED_INTERFACE (iThing)
IMPLEMENT_IBASE_EXT_END

IMPLEMENT_EMBEDDED_IBASE (csThing::eiThing)
  IMPLEMENTS_INTERFACE (iThing)
IMPLEMENT_EMBEDDED_IBASE_END

csThing::csThing (csWorld* world) : csPolygonSet (world), obj(), tree_bbox (NULL)
{
  CONSTRUCT_EMBEDDED_IBASE (scfiThing);
  center_idx = -1;
  ParentTemplate = NULL;
  tree_bbox.SetOwner (this);
}

csThing::~csThing ()
{
}

void csThing::SetConvex (bool c)
{
  flags.Set (CS_ENTITY_CONVEX, c ? CS_ENTITY_CONVEX : 0);
  if (c)
  {
    if (center_idx == -1) center_idx = AddVertex (0, 0, 0);
    int i;
    float minx = 1000000000., miny = 1000000000., minz = 1000000000.;
    float maxx = -1000000000., maxy = -1000000000., maxz = -1000000000.;
    for (i = 0 ; i < num_vertices ; i++)
      if (i != center_idx)
      {
        if (obj_verts[i].x < minx) minx = obj_verts[i].x;
  	if (obj_verts[i].x > maxx) maxx = obj_verts[i].x;
        if (obj_verts[i].y < miny) miny = obj_verts[i].y;
  	if (obj_verts[i].y > maxy) maxy = obj_verts[i].y;
        if (obj_verts[i].z < minz) minz = obj_verts[i].z;
  	if (obj_verts[i].z > maxz) maxz = obj_verts[i].z;
      }
    obj_verts[center_idx].Set ((minx+maxx)/2, (miny+maxy)/2, (minz+maxz)/2);
    wor_verts[center_idx].Set ((minx+maxx)/2, (miny+maxy)/2, (minz+maxz)/2);
  }
}

void csThing::Transform ()
{
  int i;
  if (!bbox) CreateBoundingBox ();
  for (i = 0 ; i < num_vertices ; i++)
    wor_verts[i] = obj.This2Other (obj_verts[i]);
  for (i = 0 ; i < polygons.Length () ; i++)
  {
    csPolygon3D* p = GetPolygon3D (i);
    p->ObjectToWorld (obj);
  }
  UpdateInPolygonTrees ();
}

void csThing::SetPosition (csSector* home, const csVector3& pos)
{
  obj.SetOrigin (pos);
  sector = home;

  UpdateCurveTransform();
  UpdateInPolygonTrees ();
}

void csThing::UpdateCurveTransform()
{
  // since obj has changed (possibly) we need to tell all of our curves
  for (int i = 0 ; i < GetNumCurves () ; i++)
  {
    csCurve* c = curves.Get (i);
 
    csReversibleTransform o2w = obj.GetInverse();
    c->SetObject2World (&o2w);
  }
}


void csThing::SetTransform (const csMatrix3& matrix)
{
  obj.SetT2O (matrix);
  UpdateCurveTransform ();
}

void csThing::MovePosition (const csVector3& rel)
{
  obj.Translate (rel);
  UpdateCurveTransform ();
}

void csThing::Transform (csMatrix3& matrix)
{
  obj.SetT2O (matrix * obj.GetT2O ());
  UpdateCurveTransform ();
}

void csThing::CreateLightMaps (iGraphics3D* g3d)
{
  int i;
  for (i = 0 ; i < polygons.Length () ; i++)
  {
    csPolygon3D* p = GetPolygon3D (i);
    p->CreateLightMaps (g3d);
  }
}

csPolygon3D* csThing::IntersectSphere (csVector3& center, float radius, float* pr)
{
  float d, min_d = radius;
  int i;
  csPolygon3D* p, * min_p = NULL;
  csVector3 hit;

  for (i = 0 ; i < polygons.Length () ; i++)
  {
    p = GetPolygon3D (i);
    const csPlane3& wpl = p->GetPlane ()->GetWorldPlane ();
    d = wpl.Distance (center);
    if (d < min_d && csMath3::Visible (center, wpl))
    {
      hit = -center;
      hit -= wpl.GetNormal ();
      hit *= d;
      hit += center;
      if (p->IntersectRay (center, hit))
      {
  	min_d = d;
  	min_p = p;
      }
    }
  }
  if (pr) *pr = min_d;
  return min_p;
}

/// The list of fog vertices
static DECLARE_GROWING_ARRAY (fog_verts, G3DFogInfo);

void csThing::DrawCurves (csRenderView& rview, bool use_z_buf)
{
  int i;
  int res=1;

  // Calculate tesselation resolution
  csVector3 wv = curves_center;
  csVector3 world_coord = obj.This2Other (wv);
  csVector3 camera_coord = rview.Other2This (world_coord);

  if (camera_coord.z >= SMALL_Z)
  {
    res=(int)(curves_scale/camera_coord.z);
  }
  else
    res=1000; // some big tesselation value...

  // Create the combined transform of object to camera by
  // combining object to world and world to camera.
  csReversibleTransform obj_cam = rview;
  obj_cam /= obj;
  rview.g3d->SetObjectToCamera (&obj_cam);
  rview.g3d->SetClipper (rview.view->GetClipPoly (), rview.view->GetNumVertices ());
  rview.g3d->SetPerspectiveAspect (rview.GetFOV ());
  rview.g3d->SetRenderState (G3DRENDERSTATE_ZBUFFERMODE,
      use_z_buf ? CS_ZBUF_USE : CS_ZBUF_FILL);

  // Base of the mesh.
  G3DTriangleMesh mesh;
  mesh.morph_factor = 0;
  mesh.num_vertices_pool = 1;
  mesh.num_textures = 1;
  mesh.do_mirror = rview.IsMirrored ();
  mesh.do_morph_texels = false;
  mesh.do_morph_colors = false;
  mesh.vertex_mode = G3DTriangleMesh::VM_WORLDSPACE;

  // Loop over all curves
  csCurve* c;
  for (i = 0 ; i < GetNumCurves () ; i++)
  {
    c = curves.Get (i);

    // Test visibility of entire curve by clipping bounding box against clipper.
    // There are three possibilities:
    //	1. box is not visible -> curve is not visible.
    //	2. box is entirely visible -> curve is visible and need not be clipped.
    //	3. box is partially visible -> curve is visible and needs to be clipped
    //	   if rview has do_clip_plane set to true.
    csBox2 bbox;
    if (c->GetScreenBoundingBox (obj_cam, rview, bbox) < 0) continue;	// Not visible.
    // Test if we need and should clip to the current portal.
    int box_class;
    box_class = rview.view->ClassifyBox (bbox);
    if (box_class == -1) continue; // Not visible.

    bool do_clip = false;
    if (rview.do_clip_plane || rview.do_clip_frustum)
    {
      if (box_class == 0) do_clip = true;
    }

    // If we don't need to clip to the current portal then we
    // test if we need to clip to the top-level portal.
    // Top-level clipping is always required unless we are totally
    // within the top-level frustum.
    // IF it is decided that we need to clip here then we still
    // clip to the inner portal. We have to do clipping anyway so
    // why not do it to the smallest possible clip area.
    if (!do_clip)
    {
      box_class = world->top_clipper->ClassifyBox (bbox);
      if (box_class == 0) do_clip = true;
    }

    // If we have a dirty lightmap recombine the curves and the shadow maps.
    bool updated_lm = c->RecalculateDynamicLights ();

    // Create a new tesselation reuse an old one.
    csCurveTesselated* tess = c->Tesselate (res);
    // If the lightmap was updated or the new tesselation doesn't yet
    // have a valid colors table we need to update colors here.
    if (updated_lm || !tess->AreColorsValid ())
      tess->UpdateColors (c->lightmap);

    // Setup the structure for DrawTriangleMesh.
    if (tess->GetNumVertices () > fog_verts.Limit ())
    {
      fog_verts.SetLimit (tess->GetNumVertices ());
    }

    mesh.txt_handle[0] = c->GetTextureHandle ();
    mesh.num_vertices = tess->GetNumVertices ();
    mesh.vertices[0] = tess->GetVertices ();
    mesh.texels[0][0] = tess->GetTxtCoords ();
    mesh.vertex_colors[0] = tess->GetColors ();
    mesh.num_triangles = tess->GetNumTriangles ();
    mesh.triangles = tess->GetTriangles ();
    mesh.do_clip = do_clip;
    mesh.vertex_fog = fog_verts.GetArray ();
    bool gouraud = !!c->lightmap;
    mesh.fxmode = CS_FX_COPY | (gouraud ? CS_FX_GOURAUD : 0);
    mesh.use_vertex_color = gouraud;
    if (mesh.txt_handle[0] == NULL)
    {
      CsPrintf (MSG_STDOUT, "Warning! Curve without texture!\n");
      continue;
    }
    extern void CalculateFogMesh (csRenderView* rview, csTransform* tr_o2c,
	  G3DTriangleMesh& mesh);
    CalculateFogMesh (&rview, &obj_cam, mesh);

    if (rview.callback)
      rview.callback (&rview, CALLBACK_MESH, (void*)&mesh);
    else
      rview.g3d->DrawTriangleMesh (mesh);
  }
}

void csThing::Draw (csRenderView& rview, bool use_z_buf)
{
  if (flags.Check (CS_ENTITY_CAMERA))
  {
    csRenderView rview_new = rview;
    rview_new.SetO2TTranslation (csVector3 (0));
    DrawInt (rview_new, use_z_buf);
  }
  else DrawInt (rview, use_z_buf);
}

void csThing::DrawInt (csRenderView& rview, bool use_z_buf)
{
  draw_busy++;
  UpdateTransformation (rview);

  // @@@ Wouldn't it be nice if we checked if all vertices are behind the view plane?
  {
    if (rview.callback) rview.callback (&rview, CALLBACK_THING, (void*)this);
    Stats::polygons_considered += polygons.Length ();

    DrawCurves (rview, use_z_buf);

    int res=1;

    // Calculate tesselation resolution
    // TODO should depend on a point in space, and a scale param in place of 40
    if (num_vertices>0)
    {
      csVector3 wv = wor_verts[0];
      csVector3 world_coord = obj.This2Other (wv);
      csVector3 camera_coord = rview.Other2This (world_coord);
  
      if (camera_coord.z > 0.0001)
      {
        res=(int)(40.0/camera_coord.z);
        if (res<1) res=1;
        else if (res>9) res=9;
      }
    }

    if (flags.Check (CS_ENTITY_DETAIL))
      DrawPolygonArrayDPM (polygons.GetArray (), polygons.Length (), &rview, use_z_buf);
    else
      DrawPolygonArray (polygons.GetArray (), polygons.Length (), &rview, use_z_buf);
    if (rview.callback) rview.callback (&rview, CALLBACK_THINGEXIT, (void*)this);
  }

  draw_busy--;
}

void csThing::DrawFoggy (csRenderView& d)
{
  draw_busy++;
  UpdateTransformation (d);
  csPolygon3D* p;
  csVector3* verts;
  int num_verts;
  int i;
  csPoly2DPool* render_pool = world->render_pol2d_pool;
  csPolygon2D* clip;

  // @@@ Wouldn't it be nice if we checked all vertices against the Z plane?
  {
    csVector2 orig_triangle[3];
    if (!d.callback) d.g3d->OpenFogObject (GetID (), &GetFog ());
    Stats::polygons_considered += polygons.Length ();

    d.SetMirrored (!d.IsMirrored ());
    for (i = 0 ; i < polygons.Length () ; i++)
    {
      p = GetPolygon3D (i);
      if (p->flags.Check (CS_POLY_NO_DRAW)) continue;
      clip = (csPolygon2D*)(render_pool->Alloc ());
      const csPlane3& wplane = p->GetPlane ()->GetWorldPlane ();
      bool front = csMath3::Visible (d.GetOrigin (), wplane);

      if (!front &&
        p->ClipToPlane (d.do_clip_plane ? &d.clip_plane : (csPlane3*)NULL, d.GetOrigin (),
              verts, num_verts, false) &&
        p->DoPerspective (d, verts, num_verts, clip, orig_triangle, d.IsMirrored ()) &&
        clip->ClipAgainst (d.view))
      {
        p->GetPlane ()->WorldToCamera (d, verts[0]);
	if (d.callback)
	{
          d.callback (&d, CALLBACK_POLYGON, (void*)p);
          d.callback (&d, CALLBACK_POLYGON2D, (void*)clip);
	}

        Stats::polygons_drawn++;

	if (!d.callback)
          clip->AddFogPolygon (d.g3d, p, p->GetPlane (), d.IsMirrored (),
	  	GetID (), CS_FOG_BACK);
      }
      render_pool->Free (clip);
    }
    d.SetMirrored (!d.IsMirrored ());
    for (i = 0 ; i < polygons.Length () ; i++)
    {
      p = GetPolygon3D (i);
      if (p->flags.Check (CS_POLY_NO_DRAW)) continue;
      clip = (csPolygon2D*)(render_pool->Alloc ());
      const csPlane3& wplane = p->GetPlane ()->GetWorldPlane ();
      bool front = csMath3::Visible (d.GetOrigin (), wplane);

      if (front &&
        p->ClipToPlane (d.do_clip_plane ? &d.clip_plane : (csPlane3*)NULL,
		d.GetOrigin (), verts, num_verts, true) &&
        p->DoPerspective (d, verts, num_verts, clip, orig_triangle,
		d.IsMirrored ()) &&
        clip->ClipAgainst (d.view))
      {
        p->GetPlane ()->WorldToCamera (d, verts[0]);
	if (d.callback)
	{
          d.callback (&d, CALLBACK_POLYGON, (void*)p);
          d.callback (&d, CALLBACK_POLYGON2D, (void*)clip);
	}

        Stats::polygons_drawn++;

        if (!d.callback)
	  clip->AddFogPolygon (d.g3d, p, p->GetPlane (), d.IsMirrored (),
	  	GetID (), CS_FOG_FRONT);
      }
      render_pool->Free (clip);
    }
    if (!d.callback) d.g3d->CloseFogObject (GetID ());
  }

  draw_busy--;
}


void csThing::CheckFrustum (csFrustumView& lview)
{
  csCBufferCube* cb = world->GetCBufCube ();
  csCovcube* cc = world->GetCovcube ();
  if (cb) cb->MakeEmpty ();
  else cc->MakeEmpty ();
  RealCheckFrustum (lview);
}

void csThing::RealCheckFrustum (csFrustumView& lview)
{
  csPolygon3D* p;
  int i;

  draw_busy++;
  UpdateTransformation (lview.light_frustum->GetOrigin ());

  if (light_frame_number != current_light_frame_number)
  {
    light_frame_number = current_light_frame_number;
    lview.gouraud_color_reset = true;
  }
  else lview.gouraud_color_reset = false;

  for (i = 0 ; i < polygons.Length () ; i++)
  {
    p = GetPolygon3D (i);
    p->CamUpdate ();
    lview.poly_func ((csObject*)p, &lview);
  }

  for (i = 0 ; i < GetNumCurves () ; i++)
  {
    csCurve* c = curves.Get (i);
    
    if (!lview.dynamic)
    {
      csReversibleTransform o2w = obj.GetInverse();
      c->SetObject2World (&o2w);
    }
    
    lview.curve_func ((csObject*)c, &lview);
  }

  draw_busy--;
}

void csThing::InitLightMaps (bool do_cache)
{
  int i;
  for (i = 0 ; i < polygons.Length () ; i++)
    polygons.Get (i)->InitLightMaps (this, do_cache, i);
  for (i = 0 ; i < GetNumCurves () ; i++)
    curves.Get (i)->InitLightMaps (this, do_cache, polygons.Length ()+i);
}

void csThing::CacheLightMaps ()
{
  int i;
  for (i = 0 ; i < polygons.Length () ; i++)
    polygons.Get (i)->CacheLightMaps (this, i);
  for (i = 0 ; i < GetNumCurves () ; i++)
    curves.Get (i)->CacheLightMaps (this, polygons.Length ()+i);
}

void csThing::Merge (csThing* other)
{
  int i, j;
  int* merge_vertices = new int [other->GetNumVertices ()+1];
  for (i = 0 ; i < other->GetNumVertices () ; i++)
    merge_vertices[i] = AddVertex (other->Vwor (i));

  for (i = 0 ; i < other->polygons.Length () ; i++)
  {
    csPolygon3D* p = other->GetPolygon3D (i);
    int* idx = p->GetVertices ().GetVertexIndices ();
    for (j = 0 ; j < p->GetVertices ().GetNumVertices () ; j++)
      idx[j] = merge_vertices[idx[j]];
    AddPolygon (p);
    other->polygons[i] = NULL;
  }

  for (i = 0 ; i < other->GetNumCurveVertices () ; i++)
    AddCurveVertex (other->CurveVertex (i), other->CurveTexel (i));

  for (i = 0 ; i < other->curves.Length () ; i++)
  {
    csCurve* c = other->GetCurve (i);
    AddCurve (c);
    other->curves[i] = NULL;
  }

  delete [] merge_vertices;
}


void csThing::MergeTemplate (csThingTemplate* tpl,
  csTextureHandle* default_texture, float default_texlen,
  CLights* default_lightx,
  csVector3* shift, csMatrix3* transform)
{
  (void)default_texlen;
  int i, j;
  int* merge_vertices;

  //TODO should merge? take averages or something?
  curves_center = tpl->curves_center;
  curves_scale = tpl->curves_scale;
  ParentTemplate = tpl;

  merge_vertices = new int [tpl->GetNumVertices ()+1];
  for (i = 0 ; i < tpl->GetNumVertices () ; i++)
  {
    csVector3 v = tpl->Vtex (i);
    if (transform) v = *transform * v;
    if (shift) v += *shift;
    merge_vertices[i] = AddVertex (v);
  }

  for (i = 0 ; i < tpl->GetNumPolygon () ; i++)
  {
    csPolygonTemplate* pt = tpl->GetPolygon (i);
    csPolygon3D* p;
    p = NewPolygon (pt->GetTexture ());
    p->SetName (pt->GetName());
    if (!pt->GetTexture ()) p->SetTexture (default_texture);
    csLightMapped* pol_lm = p->GetLightMapInfo ();
    if (pol_lm) pol_lm->SetUniformDynLight (default_lightx);
    int* idx = pt->GetVerticesIdx ();
    for (j = 0 ; j < pt->GetNumVertices () ; j++)
      p->AddVertex (merge_vertices[idx[j]]);
    p->flags.Set (CS_POLY_LIGHTING|CS_POLY_FLATSHADING,
      (pt->IsLighted () ? CS_POLY_LIGHTING : 0) |
      (pt->UseFlatColor () ? CS_POLY_FLATSHADING : 0));
    if (pt->GetCollDet () == -1) p->flags.Reset (CS_POLY_COLLDET);
    else if (pt->GetCollDet () == 1) p->flags.Set (CS_POLY_COLLDET);
    if (pt->GetUVCoords ())
    {
      p->SetTextureType (POLYTXT_GOURAUD);
      csGouraudShaded* gs = p->GetGouraudInfo ();
      gs->Setup (p->GetVertices ().GetNumVertices ());
      for (j = 0 ; j < pt->GetNumVertices () ; j++)
        gs->SetUV (j, pt->GetUVCoords ()[j].x, pt->GetUVCoords ()[j].y);
      if (pt->UseGouraud ())
        gs->EnableGouraud (true);
    }
    if (pt->UseFlatColor ()){
      p->SetFlatColor( pt->GetFlatColor().red, pt->GetFlatColor().green, pt->GetFlatColor().blue);
    }
    
    p->SetTextureSpace (pt->GetTextureMatrix (), pt->GetTextureVector ());
  }

  for (i=0;i< tpl->GetNumCurveVertices();i++)
  {
    AddCurveVertex (tpl->CurveVertex (i),tpl->CurveTexel (i));
  }

  for (i = 0 ; i < tpl->GetNumCurves () ; i++)
  {
    csCurveTemplate* pt = tpl->GetCurve (i);
    csCurve* p = pt->MakeCurve ();
    p->SetName(pt->GetName ());
    p->SetParent (this);
    p->SetSector( GetSector() );

    if (!pt->GetTextureHandle ()) p->SetTextureHandle (default_texture);
    for (j = 0 ; j < pt->NumVertices () ; j++)
    {
      p->SetControlPoint (j, pt->GetVertex (j));
    }
    AddCurve (p);
  }

  delete [] merge_vertices;
}

void csThing::MergeTemplate (csThingTemplate* tpl, csTextureList* txtList,
  const char* prefix, csTextureHandle* default_texture, float default_texlen,
  CLights* default_lightx, csVector3* shift, csMatrix3* transform)
{
  int i;
  const char *txtname;
  char *newname=NULL;
    
  MergeTemplate (tpl, default_texture, default_texlen, default_lightx,
    shift, transform);
    
  // now replace the textures
  for (i = 0; i < GetNumPolygons (); i++)
  {
    csPolygon3D *p = GetPolygon3D (i);
    txtname = p->GetCsTextureHandle ()->GetName ();
    newname = new char [strlen (prefix) + strlen (txtname) + 2];
    sprintf (newname, "%s_%s", prefix, txtname);
    csTextureHandle *th = txtList->FindByName (newname);
    if (th != NULL)
      p->SetTexture (th);
    delete [] newname;
  }
}

void csThing::UpdateInPolygonTrees ()
{
  // If thing has not been placed in a sector we do nothing here.
  if (!sector) return;

  // If we are not in a sector which has a polygon tree
  // then we don't really update. We should consider if this is
  // a good idea. Do we only want this object updated when we
  // want to use it in a polygon tree? It is certainly more
  // efficient to do it this way when the thing is currently
  // moving in normal convex sectors.
  csPolygonTree* tree = NULL;
  tree = sector->GetStaticTree ();
  if (!tree) return;

  csBox3 b;
  GetBoundingBox (b);

  //csTransform trans = csTransform (m_obj2world, m_world2obj * -v_obj2world);
  csReversibleTransform trans = obj.GetInverse ();
  tree_bbox.Update (b, trans, this);

  // Here we need to insert in trees where this thing lives.
  tree = sector->GetStaticTree ();
  if (tree)
  {
    // Temporarily increase reference to prevent free.
    tree_bbox.GetBaseStub ()->IncRef ();
    tree->AddObject (&tree_bbox);
    tree_bbox.GetBaseStub ()->DecRef ();
  }
}


//---------------------------------------------------------------------------

void csThing::eiThing::SetPosition (const csVector3 &iPos)
{
  scfParent->obj.SetOrigin (iPos);
}

void csThing::eiThing::SetPosition (iSector *iSec)
{
  iSec->GetPrivateObject ()->AddThing (scfParent);
}
