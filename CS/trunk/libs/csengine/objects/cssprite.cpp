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
#include "csengine/pol2d.h"
#include "csengine/cssprite.h"
#include "csengine/skeleton.h"
#include "csengine/light.h"
#include "csengine/triangle.h"
#include "csengine/camera.h"
#include "csengine/world.h"
#include "csengine/texture.h"
#include "csengine/sector.h"
#include "csengine/bspbbox.h"
#include "csengine/dumper.h"
#include "csengine/particle.h"
#include "csgeom/polyclip.h"
#include "csgeom/fastsqrt.h"
#include "csutil/garray.h"
#include "igraph3d.h"
#include "iparticl.h"


//--------------------------------------------------------------------------

csFrame::csFrame (int anm_idx, int tex_idx)
{
  name = NULL;
  animation_index = anm_idx;
  texturing_index = tex_idx;
  normals_calculated = false;
}

csFrame::~csFrame ()
{
  delete [] name;
}

void csFrame::SetName (char * n)
{
  delete [] name;
  if (n)
  {
    name = new char [strlen (n)+1];
    strcpy (name, n);
  }
  else
    name = n;
}

//--------------------------------------------------------------------------

csSpriteAction::csSpriteAction() : frames (8, 8), delays (8, 8)
{
  name = NULL;
}

csSpriteAction::~csSpriteAction()
{
  delete [] name;
}

void csSpriteAction::SetName (char *n)
{
  delete [] name;
  if (n)
  {
    name = new char [strlen (n) + 1];
    strcpy (name, n);
  }
  else
    name = n;
}

void csSpriteAction::AddFrame (csFrame * f, int d)
{
  frames.Push (f);
  delays.Push ((csSome)d);
}

//--------------------------------------------------------------------------

IMPLEMENT_CSOBJTYPE (csSpriteTemplate, csObject)

csSpriteTemplate::csSpriteTemplate ()
  : csObject (), frames (8, 8), actions (8, 8),
    texels (8, 8), vertices (8, 8), normals (8, 8)
{
  cstxt = NULL;
  emerge_from = NULL;
  skeleton = NULL;

  texel_mesh = new csTriangleMesh ();

  tri_verts = NULL;
  do_tweening = true;
}

csSpriteTemplate::~csSpriteTemplate ()
{
  delete texel_mesh;
  delete [] emerge_from;
  delete skeleton;
  delete tri_verts;
}

void csSpriteTemplate::AddVertices (int num)
{
  int frame;

  for (frame = 0; frame < frames.Length(); frame++)
  {
    normals.Get (frame)->SetNumVertices (GetNumNormals () + num);
    texels.Get (frame)->SetNumVertices (GetNumTexels () + num);
    vertices.Get (frame)->SetNumVertices (GetNumVertices () + num);
  }
}

void csSpriteTemplate::AddTriangle (int a, int b, int c)
{
  texel_mesh->AddTriangle (a, b, c);
}

void csSpriteTemplate::SetSkeleton (csSkeleton* sk)
{
  delete skeleton;
  skeleton = sk;
}

csSprite3D* csSpriteTemplate::NewSprite (csObject* parent)
{
  csSprite3D* spr;
  spr = new csSprite3D (parent);
  spr->SetTemplate (this);
  spr->EnableTweening (do_tweening);
  spr->SetAction ("default");
  spr->InitSprite ();
  return spr;
}

void csSpriteTemplate::GenerateLOD ()
{
  int i;

  //@@@ turn this into a parameter or member variable?
  int lod_base_frame = 0;

  csVector3* v = new csVector3[GetNumTexels()];

  for (i = 0; i < GetNumTexels(); i++)
    v[i] = GetVertex (lod_base_frame, i);

  csTriangleVertices* verts = new csTriangleVertices (texel_mesh, v, GetNumTexels());
  delete [] v;

  delete [] emerge_from;
  emerge_from = new int [GetNumTexels()];
  int* translate = new int [GetNumTexels()];
  csTriangleMesh* new_mesh = new csTriangleMesh (*texel_mesh);

  csLOD::CalculateLOD (new_mesh, verts, translate, emerge_from);

  for (i = 0 ; i < texels.Length () ; i++)
  {
    int j;
    csVector2* new_texels = new csVector2 [GetNumTexels ()];
    csVector3* new_vertices = new csVector3 [GetNumTexels ()];
    csVector3* new_normals = new csVector3 [GetNumTexels ()];
    csPoly2D* tx = texels.Get (i);
    csPoly3D* vt = vertices.Get (i);
    csPoly3D* vn = normals.Get (i);
    for (j = 0 ; j < GetNumTexels () ; j++)
    {
      new_texels[translate[j]] = (*tx)[j];
      new_vertices[translate[j]] = (*vt)[j];
      new_normals[translate[j]] = (*vn)[j];
    }
    for (j = 0 ; j < GetNumTexels () ; j++)
    {
      (*tx)[j] = new_texels[j];
      (*vt)[j] = new_vertices[j];
      (*vn)[j] = new_normals[j];
    }
    delete [] new_texels;
    delete [] new_vertices;
    delete [] new_normals;
  }

  if (skeleton) skeleton->RemapVertices (translate);

  for (i = 0 ; i < GetNumTriangles () ; i++)
  {
    csTriangle& tr = texel_mesh->GetTriangles()[i];
    tr.a = translate[tr.a];
    tr.b = translate[tr.b];
    tr.c = translate[tr.c];
  }

  delete [] translate;
  delete verts;
  delete new_mesh;
}

void csSpriteTemplate::ComputeBoundingBox ()
{
  int frame, vertex;

  for ( frame = 0 ; frame < GetNumFrames () ; frame++ )
  {
    csBox3 box;
    GetFrame(frame)->GetBoundingBox (box);

    box.StartBoundingBox (GetVertex (frame, 0));
    for ( vertex = 1 ; vertex < GetNumTexels() ; vertex++ )
      box.AddBoundingVertexSmart (GetVertex (frame, vertex));

    GetFrame(frame)->SetBoundingBox (box);
  }
  if (skeleton)
    skeleton->ComputeBoundingBox (vertices.Get (0));
    // @@@ should the base frame for the skeleton be a variable?
}

csFrame* csSpriteTemplate::AddFrame ()
{
  csFrame* fr = new csFrame (frames.Length(), texels.Length());
  csPoly3D* nr = new csPoly3D ();
  csPoly2D* tx = new csPoly2D ();
  csPoly3D* vr = new csPoly3D ();

  if (frames.Length() > 0)
  {
    nr->SetNumVertices (GetNumNormals  ());
    tx->SetNumVertices (GetNumTexels   ());
    vr->SetNumVertices (GetNumVertices ());
  }

  frames.Push (fr);
  normals.Push (nr);
  texels.Push (tx);
  vertices.Push (vr);

  return fr;
}

csFrame* csSpriteTemplate::FindFrame (char *n)
{
  for (int i = GetNumFrames () - 1; i >= 0; i--)
    if (strcmp (GetFrame (i)->GetName (), n) == 0)
      return GetFrame (i);

  return NULL;
}

csSpriteAction* csSpriteTemplate::AddAction ()
{
  csSpriteAction* a = new csSpriteAction ();
  actions.Push (a);
  return a;
}

void csSpriteTemplate::SetMaterial (csMaterialWrapper *material)
{
  cstxt = material;
}

void csSpriteTemplate::ComputeNormals (csFrame* frame)
{
  int i, j;

  // @@@ We only calculated normals once for every frame.
  // Normal calculation is too expensive to do again every time.
  // But maybe we should make this optional for fast systems?
  if (frame->NormalsCalculated ()) return;
  frame->SetNormalsCalculated (true);

  csVector3* object_verts = GetVertices (frame->GetAnmIndex());

  if (!tri_verts)
  {
    tri_verts = new csTriangleVertices (texel_mesh, object_verts, GetNumTexels());
  }

  csTriangle * tris = texel_mesh->GetTriangles();
  int num_triangles = texel_mesh->GetNumTriangles();
  // @@@ Avoid this allocate!
  csVector3 * tri_normals = new csVector3[num_triangles];

  // calculate triangle normals
  // get the cross-product of 2 edges of the triangle and normalize it
  for (i = 0; i < num_triangles; i++)
  {
    csVector3 ab = object_verts [tris[i].b] - object_verts [tris[i].a];
    csVector3 bc = object_verts [tris[i].c] - object_verts [tris[i].b];
    tri_normals [i] = ab % bc;
    float norm = tri_normals[i].Norm ();
    if (norm)
      tri_normals[i] /= norm;
  }

  // calculate vertex normals, by averaging connected triangle normals
  int frame_number = frame->GetAnmIndex();

  for (i = 0; i < GetNumTexels(); i++)
  {
    csTriangleVertex &vt = tri_verts->GetVertex (i);
    if (vt.num_con_triangles)
    {
      csVector3 &n = GetNormal (frame_number, i);
      n = csVector3 (0,0,0);
      for (j = 0; j < vt.num_con_triangles; j++)
        n += tri_normals [vt.con_triangles[j]];
      float norm = n.Norm ();
      if (norm)
        n /= norm;
    }
  }

  delete[] tri_normals;
}

void csSpriteTemplate::MergeNormals ()
{
  int i;
  for (i = 0; i < GetNumFrames (); i++)
    MergeNormals (i, i);
}
void csSpriteTemplate::MergeNormals (int base)
{
  if (base > GetNumFrames())
  {
    CsPrintf (MSG_WARNING, "No frame number: \n", base);
    CsPrintf (MSG_WARNING, "no smoothing performed\n");
    return;
  }
  int i;
  for (i = 0; i < GetNumFrames (); i++)
    MergeNormals (base, i);
}
void csSpriteTemplate::MergeNormals (int base, int frame)
{
  int i, j;

  int num_frames = GetNumFrames();
  if (base  > num_frames) CsPrintf (MSG_WARNING, "No frame number: \n", base);
  if (frame > num_frames) CsPrintf (MSG_WARNING, "No frame number: \n", frame);
  if (frame > num_frames || base > num_frames)
  {
    CsPrintf (MSG_WARNING, "no smoothing performed\n");
    return;
  }

  GetFrame(frame)->SetNormalsCalculated (true);

  csVector3* obj_verts  = GetVertices (frame);
  csVector3* base_verts = GetVertices (base);

  if (!tri_verts)
  {
    tri_verts = new csTriangleVertices (texel_mesh, obj_verts, GetNumTexels());
  }

  csTriangle * tris = texel_mesh->GetTriangles();
  int num_triangles = texel_mesh->GetNumTriangles();
  // @@@ Avoid this allocate!
  csVector3 * tri_normals = new csVector3[num_triangles];

  // calculate triangle normals
  // get the cross-product of 2 edges of the triangle and normalize it
  for (i = 0; i < num_triangles; i++)
  {
    csVector3 ab = obj_verts [tris[i].b] - obj_verts [tris[i].a];
    csVector3 bc = obj_verts [tris[i].c] - obj_verts [tris[i].b];
    tri_normals [i] = ab % bc;
    float norm = tri_normals[i].Norm ();
    if (norm)
      tri_normals[i] /= norm;
  }

  // create a table that maps each vertex to the
  // first vertex that has the same coordinates
  int * merge = new int [GetNumTexels()];
  for (i = 0; i < GetNumTexels(); i++)
  {
    merge[i] = i;
    for (j = 0; j < i; j++)
    {
      csVector3 difference = base_verts[i] - base_verts[j];
      if (difference.Norm () < 0.01)
      {
        merge[i] = j;
        break;
      }
    }
  }

  // create a mesh which only uses the vertex indices in the table
  csTriangleMesh merge_mesh;
  for (i = 0; i < num_triangles; i++)
    merge_mesh.AddTriangle (merge[tris[i].a], merge[tris[i].b], merge[tris[i].c]);
  csTriangleVertices * tv = new csTriangleVertices (&merge_mesh, obj_verts, GetNumTexels());

  // calculate vertex normals, by averaging connected triangle normals
  for (i = 0; i < GetNumTexels(); i++)
  {
    csTriangleVertex &vt = tv->GetVertex (i);
    if (vt.num_con_triangles)
    {
      csVector3 &n = GetNormal (frame, i);
      n = csVector3 (0,0,0);
      for (j = 0; j < vt.num_con_triangles; j++)
        n += tri_normals [vt.con_triangles[j]];
      float norm = n.Norm ();
      if (norm)
        n /= norm;
    }
  }

  // one last loop to fill in all of the merged vertex normals
  for (i = 0; i < GetNumTexels(); i++)
  {
  	csVector3 &n = GetNormal (frame, i);
	n = GetNormal (frame, merge [i]);
  }

  delete[] tri_normals;
  delete[] merge;
  delete tv;
}


csSpriteAction* csSpriteTemplate::FindAction (const char *n)
{
  for (int i = GetNumActions () - 1; i >= 0; i--)
    if (strcmp (GetAction (i)->GetName (), n) == 0)
      return GetAction (i);

  return NULL;
}

void csSpriteTemplate::HardTransform (const csReversibleTransform& t)
{
  int num = GetNumVertices ();
  int numf = GetNumFrames ();
  int i, j;
  for (i = 0 ; i < numf ; i++)
  {
    csVector3* verts = GetVertices (i);
    for (j = 0 ; j < num ; j++)
      verts[j] = t.This2Other (verts[j]);
  }
}

//=============================================================================

IMPLEMENT_CSOBJTYPE (csSprite, csObject)
IMPLEMENT_IBASE (csSprite)
  IMPLEMENTS_INTERFACE (iBase)
  IMPLEMENTS_EMBEDDED_INTERFACE (iParticle)
IMPLEMENT_IBASE_END

IMPLEMENT_EMBEDDED_IBASE (csSprite::Particle)
  IMPLEMENTS_INTERFACE (iParticle)
IMPLEMENT_EMBEDDED_IBASE_END

csSprite::csSprite (csObject* theParent) : csObject ()
{
  CONSTRUCT_IBASE (NULL);
  CONSTRUCT_EMBEDDED_IBASE (scfiParticle);
  movable.scfParent = this;
  dynamiclights = NULL;
  MixMode = CS_FX_COPY;
  defered_num_lights = 0;
  defered_lighting_flags = 0;
  draw_callback = NULL;
  draw_callback2 = NULL;
  is_visible = false;
  camera_cookie = 0;
  ptree_obj = NULL;
  myOwner = NULL;
  parent = theParent;
  movable.SetObject (this);
  if (parent->GetType () >= csSprite::Type)
  {
    csSprite* sparent = (csSprite*)parent;
    movable.SetParent (&sparent->GetMovable ());
  }
}

csSprite::~csSprite ()
{
  while (dynamiclights) delete dynamiclights;
  if (parent->GetType () == csWorld::Type)
  {
    csWorld* world = (csWorld*)parent;
    world->UnlinkSprite (this);
  }
}

void csSprite::UpdateMove ()
{
  UpdateInPolygonTrees ();
}

void csSprite::MoveToSector (csSector* s)
{
  s->sprites.Push (this);
}

void csSprite::RemoveFromSectors ()
{
  if (GetPolyTreeObject ())
    GetPolyTreeObject ()->RemoveFromTree ();
  if (parent->GetType () != csWorld::Type) return;
  int i;
  csVector& sectors = movable.GetSectors ();
  for (i = 0 ; i < sectors.Length () ; i++)
  {
    csSector* ss = (csSector*)sectors[i];
    if (ss)
    {
      int idx = ss->sprites.Find (this);
      if (idx >= 0)
      {
        ss->sprites[idx] = NULL;
        ss->sprites.Delete (idx);
      }
    }
  }
}

/// The list of lights that hit the sprite
static DECLARE_GROWING_ARRAY_REF (light_worktable, csLight*);

void csSprite::UpdateDeferedLighting (const csVector3& pos)
{
  if (defered_num_lights)
  {
    if (defered_num_lights > light_worktable.Limit ())
      light_worktable.SetLimit (defered_num_lights);

    csSector* sect = movable.GetSector (0);
    int num_lights = csWorld::current_world->GetNearbyLights (sect,
      pos, defered_lighting_flags,
      light_worktable.GetArray (), defered_num_lights);
    UpdateLighting (light_worktable.GetArray (), num_lights);
  }
}

void csSprite::DeferUpdateLighting (int flags, int num_lights)
{
  defered_num_lights = num_lights;
  defered_lighting_flags = flags;
}

void csSprite::UnlinkDynamicLight (csLightHitsSprite* lp)
{
  if (lp->next_sprite) lp->next_sprite->prev_sprite = lp->prev_sprite;
  if (lp->prev_sprite) lp->prev_sprite->next_sprite = lp->next_sprite;
  else dynamiclights = lp->next_sprite;
  lp->prev_sprite = lp->next_sprite = NULL;
  lp->sprite = NULL;
}

void csSprite::AddDynamicLight (csLightHitsSprite* lp)
{
  lp->next_sprite = dynamiclights;
  lp->prev_sprite = NULL;
  if (dynamiclights) dynamiclights->prev_sprite = lp;
  dynamiclights = lp;
  lp->sprite = this;
}

bool csSprite::HitBeam (const csVector3& start, const csVector3& end,
	csVector3& isect, float* pr)
{
  const csReversibleTransform& trans = movable.GetTransform ();
  csVector3 startObj = trans * start;
  csVector3 endObj = trans * end;
  bool rc = HitBeamObject (startObj, endObj, isect, pr);
  if (rc)
    isect = trans.This2Other (isect);
  return rc;
}

void csSprite::Particle::MoveToSector(csSector* s)
  { scfParent->GetMovable ().SetSector (s); scfParent->GetMovable ().UpdateMove (); }
void csSprite::Particle::SetPosition(const csVector3& v)
  { scfParent->GetMovable().SetPosition(v); scfParent->GetMovable ().UpdateMove (); }
void csSprite::Particle::MovePosition(const csVector3& v)
  { scfParent->GetMovable().MovePosition(v); scfParent->GetMovable ().UpdateMove (); }
void csSprite::Particle::SetColor(const csColor& c)
  { scfParent->SetColor(c); }
void csSprite::Particle::AddColor(const csColor& c)
  { scfParent->AddColor(c); }
void csSprite::Particle::ScaleBy(float factor)
  { scfParent->ScaleBy(factor); }
void csSprite::Particle::SetMixmode(UInt mode)
  { scfParent->SetMixmode(mode); }
void csSprite::Particle::Rotate(float angle)
  { scfParent->Rotate(angle); }
void csSprite::Particle::Draw(csRenderView& v)
  { scfParent->Draw(v); }
void csSprite::Particle::UpdateLighting(csLight** lights, int num_lights)
  { scfParent->UpdateLighting(lights, num_lights); }
void csSprite::Particle::DeferUpdateLighting(int flags, int num_lights)
  { scfParent->DeferUpdateLighting(flags, num_lights); }

//=============================================================================

IMPLEMENT_CSOBJTYPE (csSprite3D, csSprite)

IMPLEMENT_IBASE_EXT (csSprite3D)
  IMPLEMENTS_EMBEDDED_INTERFACE (iPolygonMesh)
IMPLEMENT_IBASE_EXT_END

IMPLEMENT_EMBEDDED_IBASE (csSprite3D::PolyMesh)
  IMPLEMENTS_INTERFACE (iPolygonMesh)
IMPLEMENT_EMBEDDED_IBASE_END

/// Static vertex array.
static DECLARE_GROWING_ARRAY_REF (tr_verts, csVector3);
/// Static uv array.
static DECLARE_GROWING_ARRAY_REF (uv_verts, csVector2);
/// The list of fog vertices
static DECLARE_GROWING_ARRAY_REF (fog_verts, G3DFogInfo);
/// The list of object vertices.
static DECLARE_GROWING_ARRAY_REF (obj_verts, csVector3);
/// The list of tween vertices.
static DECLARE_GROWING_ARRAY_REF (tween_verts, csVector3);

csSprite3D::csSprite3D (csObject* theParent) : csSprite (theParent), bbox (NULL)
{
  CONSTRUCT_EMBEDDED_IBASE (scfiPolygonMesh);
  bbox.SetOwner (this);
  ptree_obj = &bbox;
  cur_frame = 0;
  tpl = NULL;
  force_otherskin = false;
  cur_action = NULL;
  vertex_colors = NULL;
  skeleton_state = NULL;
  tween_ratio = 0;

  tr_verts.IncRef ();
  uv_verts.IncRef ();
  fog_verts.IncRef ();
  obj_verts.IncRef ();
  tween_verts.IncRef ();
  light_worktable.IncRef ();

  do_tweening = true;
}

csSprite3D::~csSprite3D ()
{
  light_worktable.DecRef ();
  uv_verts.DecRef ();
  tr_verts.DecRef ();
  fog_verts.DecRef ();
  obj_verts.DecRef ();
  tween_verts.DecRef ();

  delete [] vertex_colors;
  delete skeleton_state;
}

void csSprite3D::SetTemplate (csSpriteTemplate* tmpl)
{
  tpl = tmpl;
  delete skeleton_state;
  skeleton_state = NULL;
  if (tmpl->GetSkeleton ())
    skeleton_state = (csSkeletonState*)tmpl->GetSkeleton ()->CreateState ();
  EnableTweening (tmpl->IsTweeningEnabled ());
}

void csSprite3D::SetMaterial (csMaterialWrapper *material)
{
  force_otherskin = true;
  cstxt = material;
}


void csSprite3D::ScaleBy (float factor)
{
  csMatrix3 trans = movable.GetTransform ().GetT2O ();
  trans.m11 *= factor;
  trans.m22 *= factor;
  trans.m33 *= factor;
  movable.SetTransform (trans);
  UpdateMove ();
}


void csSprite3D::Rotate (float angle)
{
  csZRotMatrix3 rotz(angle);
  movable.Transform (rotz);
  csXRotMatrix3 rotx(angle);
  movable.Transform (rotx);
  UpdateMove ();
}


void csSprite3D::SetColor (const csColor& col)
{
  for(int i=0; i<tpl->GetNumTexels(); i++)
    SetVertexColor(i, col);
}


void csSprite3D::AddColor (const csColor& col)
{
  for(int i=0; i<tpl->GetNumTexels(); i++)
    AddVertexColor(i, col);
}


void csSprite3D::SetVertexColor (int i, const csColor& col)
{
  if (!vertex_colors)
  {
    vertex_colors = new csColor [tpl->GetNumTexels ()];
    int j;
    for (j = 0 ; j < tpl->GetNumTexels (); j++)
      vertex_colors[j].Set (0, 0, 0);
  }
  vertex_colors[i] = col;
}

void csSprite3D::AddVertexColor (int i, const csColor& col)
{
  if (!vertex_colors)
  {
    vertex_colors = new csColor [tpl->GetNumTexels ()];
    int j;
    for (j = 0 ; j < tpl->GetNumTexels (); j++)
      vertex_colors[j].Set (0, 0, 0);
  }
  vertex_colors [i] += col;
}

void csSprite3D::ResetVertexColors ()
{
  if (vertex_colors)
    for (int i = 0 ; i < tpl->GetNumTexels (); i++)
      vertex_colors [i].Set (0, 0, 0);
  //delete [] vertex_colors;
  //vertex_colors = NULL;
}

void csSprite3D::FixVertexColors ()
{
  if (vertex_colors)
    for (int i = 0 ; i < tpl->GetNumTexels (); i++)
      vertex_colors [i].Clamp (2., 2., 2.);
}

csTriangleMesh csSprite3D::mesh;
float csSprite3D::cfg_lod_detail = 1;
bool csSprite3D::do_quality_lighting = false;

int map (int* emerge_from, int idx, int num_verts)
{
  if (num_verts <= 0) return 0;
  while (idx >= num_verts)
  {
    int idx2 = emerge_from[idx];
    // @@@ THIS SHOULD NOT BE NEEDED! DEBUG WHY IT IS NEEDED
    if (idx == idx2) return idx;
    idx = idx2;
  }
  return idx;
}

void csSprite3D::GenerateSpriteLOD (int num_vts)
{
  int* emerge_from = tpl->GetEmergeFrom ();
  csTriangleMesh* base_mesh = tpl->GetTexelMesh ();
  mesh.Reset ();
  int i;
  int a, b, c;
  for (i = 0 ; i < base_mesh->GetNumTriangles () ; i++)
  {
    csTriangle& tr = base_mesh->GetTriangles ()[i];
    a = map (emerge_from, tr.a, num_vts);
    b = map (emerge_from, tr.b, num_vts);
    c = map (emerge_from, tr.c, num_vts);
    if (a != b && b != c && a != c) mesh.AddTriangle (a, b, c);
  }
}

void csSprite3D::UpdateWorkTables (int max_size)
{
  if (max_size > tr_verts.Limit ())
  {
    tr_verts.SetLimit (max_size);
    uv_verts.SetLimit (max_size);
    fog_verts.SetLimit (max_size);
    obj_verts.SetLimit (max_size);
    tween_verts.SetLimit (max_size);
  }
}

void csSprite3D::UpdateInPolygonTrees ()
{
  bbox.RemoveFromTree ();

  // If we are not in a sector which has a polygon tree
  // then we don't really update. We should consider if this is
  // a good idea. Do we only want this object updated when we
  // want to use it in a polygon tree? It is certainly more
  // efficient to do it this way when the sprite is currently
  // moving in normal convex sectors.
  int i;
  csPolygonTree* tree = NULL;
  csVector& sects = movable.GetSectors ();
  for (i = 0 ; i < sects.Length () ; i++)
  {
    tree = ((csSector*)sects[i])->GetStaticTree ();
    if (tree) break;
  }
  if (!tree) return;

  csBox3 b;
  GetObjectBoundingBox (b);

  // This transform should be part of the sprite class and not just calculated
  // every time we need it. @@@!!!
  csTransform trans = movable.GetTransform ().GetInverse ();

  bbox.Update (b, trans, this);

  // Here we need to insert in trees where this sprite lives.
  for (i = 0 ; i < sects.Length () ; i++)
  {
    tree = ((csSector*)sects[i])->GetStaticTree ();
    if (tree)
    {
      // Temporarily increase reference to prevent free.
      bbox.GetBaseStub ()->IncRef ();
      tree->AddObject (&bbox);
      bbox.GetBaseStub ()->DecRef ();
    }
  }
}

void csSprite3D::GetObjectBoundingBox (csBox3& b)
{
  if (skeleton_state)
  {
    skeleton_state->ComputeBoundingBox (csTransform (), b);
  }
  else
  {
    csFrame* cframe = cur_action->GetFrame (cur_frame);
    cframe->GetBoundingBox (b);
  }
}

void csSprite3D::GetWorldBoundingBox (csBox3& b)
{
  csBox3 ob;
  GetObjectBoundingBox (ob);
  csTransform& trans = GetMovable ().GetTransform ();
  b.StartBoundingBox (trans * ob.GetCorner (0));
  b.AddBoundingVertexSmart (trans * ob.GetCorner (1));
  b.AddBoundingVertexSmart (trans * ob.GetCorner (2));
  b.AddBoundingVertexSmart (trans * ob.GetCorner (3));
  b.AddBoundingVertexSmart (trans * ob.GetCorner (4));
  b.AddBoundingVertexSmart (trans * ob.GetCorner (5));
  b.AddBoundingVertexSmart (trans * ob.GetCorner (6));
  b.AddBoundingVertexSmart (trans * ob.GetCorner (7));
}

csVector3 csSprite3D::GetRadius ()
{
  csBox3 b;
  GetObjectBoundingBox (b);
  return (b.Max () - b.Min ()) * .5f;
}

void csSprite3D::GetCameraBoundingBox (const csCamera& camtrans, csBox3& cbox)
{
  csTranCookie cur_cookie = csWorld::current_world->tr_manager.GetCookie ();
  if (camera_cookie == cur_cookie)
  {
    cbox = camera_bbox;
    return;
  }
  camera_cookie = cur_cookie;

  csTransform trans = camtrans * movable.GetTransform ().GetInverse ();
  if (skeleton_state)
  {
    skeleton_state->ComputeBoundingBox (trans, camera_bbox);
  }
  else
  {
    csFrame* cframe = cur_action->GetFrame (cur_frame);
    csBox3 box;
    cframe->GetBoundingBox (box);
    camera_bbox.StartBoundingBox (trans * box.GetCorner (0));
    camera_bbox.AddBoundingVertexSmart (trans * box.GetCorner (1));
    camera_bbox.AddBoundingVertexSmart (trans * box.GetCorner (2));
    camera_bbox.AddBoundingVertexSmart (trans * box.GetCorner (3));
    camera_bbox.AddBoundingVertexSmart (trans * box.GetCorner (4));
    camera_bbox.AddBoundingVertexSmart (trans * box.GetCorner (5));
    camera_bbox.AddBoundingVertexSmart (trans * box.GetCorner (6));
    camera_bbox.AddBoundingVertexSmart (trans * box.GetCorner (7));
  }
  cbox = camera_bbox;
}

float csSprite3D::GetScreenBoundingBox (const csCamera& camtrans, csBox2& boundingBox)
{
  csVector2   oneCorner;
  csBox3      cbox;

  // @@@ Note. The bounding box created by this function greatly
  // exagerates the real bounding box. However, this function
  // needs to be fast. I'm not sure how to do this more accuratelly.

  GetCameraBoundingBox (camtrans, cbox);

  // if the entire bounding box is behind the camera, we're done
  if ((cbox.MinZ () < 0) && (cbox.MaxZ () < 0))
    return -1;

  // Transform from camera to screen space.
  if (cbox.MinZ () <= 0)
  {
    // Sprite is very close to camera.
    // Just return a maximum bounding box.
    boundingBox.Set (-10000, -10000, 10000, 10000);
  }
  else
  {
    camtrans.Perspective (cbox.Max (), oneCorner);
    boundingBox.StartBoundingBox (oneCorner);
    csVector3 v (cbox.MinX (), cbox.MinY (), cbox.MaxZ ());
    camtrans.Perspective (v, oneCorner);
    boundingBox.AddBoundingVertexSmart (oneCorner);
    camtrans.Perspective (cbox.Min (), oneCorner);
    boundingBox.AddBoundingVertexSmart (oneCorner);
    v.Set (cbox.MaxX (), cbox.MaxY (), cbox.MinZ ());
    camtrans.Perspective (v, oneCorner);
    boundingBox.AddBoundingVertexSmart (oneCorner);
  }

  return cbox.MaxZ ();
}

// New version of sprite drawing routine using DrawTriangleMesh.
void csSprite3D::Draw (csRenderView& rview)
{
  int i;
  if (draw_callback) draw_callback (this, &rview);

  if (!tpl->cstxt)
  {
    CsPrintf (MSG_FATAL_ERROR, "Error! Trying to draw a sprite with no material!\n");
    fatal_exit (0, false);
  }

  // Test visibility of entire sprite by clipping bounding box against clipper.
  // There are three possibilities:
  //	1. box is not visible -> sprite is not visible.
  //	2. box is entirely visible -> sprite is visible and need not be clipped.
  //	3. box is partially visible -> sprite is visible and needs to be clipped
  //	   if rview has do_clip_plane set to true.
  csBox2 bbox;
  if (GetScreenBoundingBox (rview, bbox) < 0) return;	// Not visible.
  //@@@ Debug output: this should be an optional feature for WalkTest.
  //{
    //csPolygon2D* p2d = new csPolygon2D ();
    //p2d->AddVertex (bbox.GetCorner (0));
    //p2d->AddVertex (bbox.GetCorner (1));
    //p2d->AddVertex (bbox.GetCorner (3));
    //p2d->AddVertex (bbox.GetCorner (2));
    //p2d->Draw (rview.g2d, 255);
    //delete p2d;
  //}

  // Test if we need and should clip to the current portal.
  int box_class;
  box_class = rview.view->ClassifyBox (bbox);
  if (box_class == -1) return; // Not visible.
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
    box_class = csWorld::current_world->top_clipper->ClassifyBox (bbox);
    if (box_class == 0) do_clip = true;
  }

  UpdateWorkTables (tpl->GetNumTexels());
  UpdateDeferedLighting (movable.GetPosition ());

  csFrame * cframe = cur_action->GetFrame (cur_frame);

  // Get next frame for animation tweening.
  csFrame * next_frame;
  if (cur_frame + 1 < cur_action->GetNumFrames())
    next_frame = cur_action->GetFrame (cur_frame + 1);
  else
    next_frame = cur_action->GetFrame (0);

  // First create the transformation from object to camera space directly:
  //   W = Mow * O - Vow;
  //   C = Mwc * (W - Vwc)
  // ->
  //   C = Mwc * (Mow * O - Vow - Vwc)
  //   C = Mwc * Mow * O - Mwc * (Vow + Vwc)
  csReversibleTransform tr_o2c = rview * movable.GetTransform ().GetInverse ();
  rview.g3d->SetObjectToCamera (&tr_o2c);
  rview.g3d->SetClipper (rview.view->GetClipPoly (), rview.view->GetNumVertices ());
  // @@@ This should only be done when aspect changes...
  rview.g3d->SetPerspectiveAspect (rview.GetFOV ());

  rview.g3d->SetRenderState (G3DRENDERSTATE_ZBUFFERMODE, CS_ZBUF_USE);

  bool do_tween = false;
  if (!skeleton_state && tween_ratio) do_tween = true;

  int cf_idx = cframe->GetAnmIndex();

  csVector3* real_obj_verts;
  csVector3* real_tween_verts = NULL;

  real_obj_verts = tpl->GetVertices (cf_idx);
  if (do_tween)
  {
    int nf_idx = next_frame->GetAnmIndex();
    real_tween_verts = tpl->GetVertices (nf_idx);
  }

  // If we have a skeleton then we transform all vertices through
  // the skeleton. In that case we also include the camera transformation
  // so that the 3D renderer does not need to do it anymore.
  csVector3* verts;
  if (skeleton_state)
  {
    skeleton_state->Transform (tr_o2c, real_obj_verts, tr_verts.GetArray ());
    verts = tr_verts.GetArray ();
  }
  else
  {
    verts = real_obj_verts;
  }

  // Calculate the right LOD level for this sprite.
  // Select the appropriate mesh.
  csTriangleMesh* m;
  int* emerge_from = NULL;
  int num_verts;
  float fnum = 0.0f;
  if (cfg_lod_detail < 0 || cfg_lod_detail == 1)
  {
    m = tpl->GetTexelMesh ();
    num_verts = tpl->GetNumTexels ();
  }
  else
  {
    m = &mesh;
    // We calculate the number of vertices to use for this LOD
    // level. The integer part will be the number of vertices.
    // The fractional part will determine how much to morph
    // between the new vertex and the previous last vertex.
    fnum = cfg_lod_detail*(float)(tpl->GetNumTexels()+1);
    num_verts = (int)fnum;
    fnum -= num_verts;  // fnum is now the fractional part.
    GenerateSpriteLOD (num_verts);
    emerge_from = tpl->GetEmergeFrom ();
  }

  csVector2* real_uv_verts;
  // Do vertex morphing if needed.
  if (cfg_lod_detail < 0 || cfg_lod_detail == 1)
  {
    real_uv_verts = tpl->GetTexels (cf_idx);
  }
  else
  {
    for (i = 0 ; i < num_verts ; i++)
    {
      csVector2 uv;
      if (i < num_verts-1)
      {
        uv = tpl->GetTexel (cf_idx, i);
      }
      else
      {
        // Morph between the last vertex and the one we morphed from.
        uv = (1-fnum) * tpl->GetTexel (cf_idx, emerge_from[i])
          + fnum * tpl->GetTexel (cf_idx, i);
      }

      uv_verts[i] = uv;
    }
    real_uv_verts = uv_verts.GetArray ();
  }

  // Setup the structure for DrawTriangleMesh.
  G3DTriangleMesh mesh;
  if (force_otherskin)
    mesh.mat_handle[0] = cstxt->GetMaterialHandle ();
  else
    mesh.mat_handle[0] = tpl->cstxt->GetMaterialHandle ();
  mesh.num_vertices = num_verts;
  mesh.vertices[0] = verts;
  mesh.texels[0][0] = real_uv_verts;
  mesh.vertex_colors[0] = vertex_colors;
  if (do_tween)
  {
    mesh.morph_factor = tween_ratio;
    mesh.num_vertices_pool = 2;
    mesh.vertices[1] = real_tween_verts;
    mesh.texels[1][0] = real_uv_verts;
    mesh.vertex_colors[1] = vertex_colors;
  }
  else
  {
    mesh.morph_factor = 0;
    mesh.num_vertices_pool = 1;
  }
  mesh.num_materials = 1;

  mesh.num_triangles = m->GetNumTriangles ();
  mesh.triangles = m->GetTriangles ();

  mesh.use_vertex_color = !!vertex_colors;
  mesh.do_clip = do_clip;
  mesh.do_mirror = rview.IsMirrored ();
  mesh.do_morph_texels = false;
  mesh.do_morph_colors = false;
  mesh.vertex_fog = fog_verts.GetArray ();

  if (skeleton_state)
    mesh.vertex_mode = G3DTriangleMesh::VM_VIEWSPACE;
  else
    mesh.vertex_mode = G3DTriangleMesh::VM_WORLDSPACE;
  mesh.fxmode = MixMode | (vertex_colors ? CS_FX_GOURAUD : 0);

  extern void CalculateFogMesh (csRenderView* rview, csTransform* tr_o2c,
	G3DTriangleMesh& mesh);
  CalculateFogMesh (&rview, &tr_o2c, mesh);

  if (rview.callback)
    rview.callback (&rview, CALLBACK_MESH, (void*)&mesh);
  else
    rview.g3d->DrawTriangleMesh (mesh);
  //else
  // @@@ Provide functionality for visible edges here...

  if (draw_callback2)
    draw_callback2 (this, &rview, myOwner);
}

void csSprite3D::InitSprite ()
{
  if (!tpl)
  {
    CsPrintf (MSG_FATAL_ERROR, "There is no defined template for this sprite!\n");
    fatal_exit (0, false);
  }

  if (!cur_action) { SetFrame (0); cur_action = tpl->GetFirstAction (); }

  last_time = csWorld::System->GetTime ();

  MixMode = CS_FX_COPY;
}

bool csSprite3D::NextFrame (cs_time current_time, bool onestep, bool stoptoend)
{
  bool ret = false;

  if(onestep)
  {
    if(current_time>last_time+cur_action->GetFrameDelay (cur_frame))
    {
      last_time = current_time;
      cur_frame++;
      if (cur_frame >= cur_action->GetNumFrames ())
      {
        if(stoptoend) cur_frame --;
        else cur_frame = 0;
        ret = true;
      }
    }
  }
  else
  {
    while(1)
    {
      if(current_time>last_time+cur_action->GetFrameDelay (cur_frame))
      {
        last_time += cur_action->GetFrameDelay (cur_frame);
        cur_frame++;
        if (cur_frame >= cur_action->GetNumFrames ())
        {
          if(stoptoend)
          {
            cur_frame--;
            ret = true;
            break;
          }
          cur_frame = 0;
          ret = true;
        }        
      }
      else break;
    }
  }

  if (do_tweening)
    tween_ratio = (current_time - last_time) / (float)cur_action->GetFrameDelay (cur_frame);
  else
    tween_ratio = 0;

  return ret;
}

csVector3* csSprite3D::GetObjectVerts (csFrame* fr)
{
  UpdateWorkTables (tpl->GetNumTexels ());
  int fr_idx = fr->GetAnmIndex();

  for (int i = 0; i < tpl->GetNumTexels (); i++)
    obj_verts[i] = tpl->GetVertex(fr_idx, i);

  if (skeleton_state)
  {
    UpdateWorkTables (tpl->GetNumTexels());
    skeleton_state->Transform (csTransform (), obj_verts.GetArray (), tr_verts.GetArray ());
    return tr_verts.GetArray ();
  }
  else
    return obj_verts.GetArray ();
}

void csSprite3D::UpdateLighting (csLight** lights, int num_lights)
{
  int i;

  defered_num_lights = 0;

  csFrame* this_frame = cur_action->GetFrame (cur_frame);
  csFrame* next_frame; // next frame for vertex animation interpolation
  if (cur_frame + 1 < cur_action->GetNumFrames())
    next_frame = cur_action->GetFrame (cur_frame + 1);
  else
    next_frame = cur_action->GetFrame (0);

  ResetVertexColors ();

  // this is so that sprite gets blackened if no light strikes it
  AddVertexColor (0, csColor (0, 0, 0));

  csSector * sect = movable.GetSector (0);
  if (sect)
  {
    int r, g, b;
    sect->GetAmbientColor (r, g, b);
    csColor ambient_color (r / 128.0, g / 128.0, b / 128.0);
    for (i = 0 ; i < tpl->GetNumTexels (); i++)
      AddVertexColor (i, ambient_color);
  }

  // make sure normals are computed
  tpl->ComputeNormals (this_frame);
  if (tween_ratio) tpl->ComputeNormals (next_frame);

  csVector3* work_obj_verts;

  if (do_quality_lighting)
  {
    if (tween_ratio)
    {
      UpdateWorkTables (tpl->GetNumTexels ()); // make room in obj_verts;

      int tf_idx = this_frame->GetAnmIndex();
      int nf_idx = next_frame->GetAnmIndex();

      float remainder = 1 - tween_ratio;

      for (i = 0 ; i < tpl->GetNumTexels() ; i++)
        obj_verts[i] = tween_ratio * tpl->GetVertex (tf_idx, i)
          + remainder * tpl->GetVertex (nf_idx, i);

      work_obj_verts = obj_verts.GetArray ();
    }
    else
      work_obj_verts = GetObjectVerts (this_frame);

    UpdateLightingHQ (lights, num_lights, work_obj_verts);
  }
  else
    UpdateLightingLQ (lights, num_lights, work_obj_verts);
}

void csSprite3D::UpdateLightingLQ (csLight** lights, int num_lights, csVector3* object_vertices)
{
  int i, j;
  csFrame* this_frame = cur_action->GetFrame (cur_frame);
  csFrame* next_frame;
  if (cur_frame + 1 < cur_action->GetNumFrames())
    next_frame = cur_action->GetFrame (cur_frame + 1);
  else
    next_frame = cur_action->GetFrame (0);
  int tf_idx = this_frame->GetAnmIndex();
  int nf_idx = next_frame->GetAnmIndex();

  csBox3 obox;
  GetObjectBoundingBox (obox);
  csVector3 obj_center = (obox.Min () + obox.Max ()) / 2;
  csVector3 wor_center = movable.GetTransform ().This2Other (obj_center);
  csColor color;

  for (i = 0 ; i < num_lights ; i++)
  {
    csColor light_color = lights [i]->GetColor () * (256. / NORMAL_LIGHT_LEVEL);
    float sq_light_radius = lights [i]->GetSquaredRadius ();

    // Compute light position in object coordinates
    csVector3 wor_light_pos = lights [i]->GetCenter ();
    float wor_sq_dist = csSquaredDist::PointPoint (wor_light_pos, wor_center);
    if (wor_sq_dist >= sq_light_radius) continue;

    csVector3 obj_light_pos = movable.GetTransform ().Other2This (wor_light_pos);
    float obj_sq_dist = csSquaredDist::PointPoint (obj_light_pos, obj_center);
    float obj_dist = FastSqrt (obj_sq_dist);
    float wor_dist = FastSqrt (wor_sq_dist);

    csVector3 obj_light_direction = (obj_light_pos - obj_center);

    for (j = 0 ; j < tpl->GetNumTexels () ; j++)
    {
      csVector3 normal = tpl->GetNormal (tf_idx, j);
      if (tween_ratio)
      {
        normal = (1 - tween_ratio) * normal
          + tween_ratio * tpl->GetNormal (nf_idx, j);
	float norm = normal.Norm ();
	if (ABS (norm) > SMALL_EPSILON)
          normal /= norm;
      }

      float cosinus;
      if (obj_sq_dist < SMALL_EPSILON)
        cosinus = 1;
      else
        cosinus = obj_light_direction * normal;

      if (cosinus > 0)
      {
        color = light_color;
        if (obj_sq_dist >= SMALL_EPSILON)
          cosinus /= obj_dist;
        if (cosinus < 1)
          color *= cosinus * lights[i]->GetBrightnessAtDistance (wor_dist);
        AddVertexColor (j, color);
      }
    }
  }

  // Clamp all vertice colors to 2.0
  FixVertexColors ();
}

void csSprite3D::UpdateLightingHQ (csLight** lights, int num_lights, csVector3* object_vertices)
{
  int i, j;
  csFrame* this_frame = cur_action->GetFrame (cur_frame);
  csFrame* next_frame;
  if (cur_frame + 1 < cur_action->GetNumFrames())
    next_frame = cur_action->GetFrame (cur_frame + 1);
  else
    next_frame = cur_action->GetFrame (0);
  int tf_idx = this_frame->GetAnmIndex();
  int nf_idx = next_frame->GetAnmIndex();

  csColor color;

  for (i = 0 ; i < num_lights ; i++)
  {
    csColor light_color = lights [i]->GetColor () * (256. / NORMAL_LIGHT_LEVEL);
    float sq_light_radius = lights [i]->GetSquaredRadius ();

    // Compute light position in object coordinates
    csVector3 wor_light_pos = lights [i]->GetCenter ();
    csVector3 obj_light_pos = movable.GetTransform ().Other2This (wor_light_pos);

    for (j = 0 ; j < tpl->GetNumTexels () ; j++)
    {
      csVector3& obj_vertex = object_vertices[j];
      csVector3 wor_vertex = movable.GetTransform ().This2Other (obj_vertex);

      // @@@ We have the distance in object space. Can't we use
      // that to calculate the distance in world space as well?
      // These calculations aren't optimal. I have the feeling they
      // can be optimized somewhat.
      float obj_sq_dist = csSquaredDist::PointPoint (obj_light_pos, obj_vertex);
      float wor_sq_dist = csSquaredDist::PointPoint (wor_light_pos, wor_vertex);

      csVector3 normal = tpl->GetNormal (tf_idx, j);
      if (tween_ratio)
      {
        normal = (1 - tween_ratio) * normal
          + tween_ratio * tpl->GetNormal (nf_idx, j);
	float norm = normal.Norm ();
	if (ABS (norm) > SMALL_EPSILON)
          normal /= norm;
      }

      float cosinus;
      if (obj_sq_dist < SMALL_EPSILON)
        cosinus = 1;
      else
        cosinus = (obj_light_pos - obj_vertex) * normal;


      if ((cosinus > 0) && (wor_sq_dist < sq_light_radius))
      {
        color = light_color;
        if (obj_sq_dist >= SMALL_EPSILON)
          cosinus /= FastSqrt (obj_sq_dist);
        if (cosinus < 1)
          color *= cosinus * lights[i]->GetBrightnessAtDistance (FastSqrt (wor_sq_dist));
        AddVertexColor (j, color);
      }
    }
  }

  // Clamp all vertice colors to 2.0
  FixVertexColors ();
}

bool csSprite3D::HitBeamObject (const csVector3& start, const csVector3& end,
	csVector3& isect, float* pr)
{
  // @@@ We might consider checking to a lower LOD version only.
  // This function is not very fast if the bounding box test succeeds.
  csBox3 b;
  GetObjectBoundingBox (b);
  csSegment3 seg (start, end);
  if (!csIntersect3::BoxSegment (b, seg, isect, pr))
    return false;
  csFrame* cframe = cur_action->GetFrame (cur_frame);
  csVector3* verts = GetObjectVerts (cframe);
  csTriangle* tris = tpl->GetTriangles ();
  int i;
  for (i = 0 ; i < tpl->GetNumTriangles () ; i++)
  {
    csTriangle& tr = tris[i];
    if (csIntersect3::IntersectTriangle (verts[tr.a], verts[tr.b],
    	verts[tr.c], seg, isect))
    {
      if (pr)
      {
        *pr = sqrt (csSquaredDist::PointPoint (start, isect) /
		csSquaredDist::PointPoint (start, end));
      }
      return true;
    }
  }
  return false;
}


//--------------------------------------------------------------------------

csMeshedPolygon* csSprite3D::PolyMesh::GetPolygons ()
{
  if (!polygons)
  {
    csSpriteTemplate* tmpl = scfParent->GetTemplate ();
    csTriangle* triangles = tmpl->GetTriangles ();
    polygons = new csMeshedPolygon [GetNumPolygons ()];
    int i;
    for (i = 0 ; i < GetNumPolygons () ; i++)
    {
      polygons[i].num_vertices = 3;
      polygons[i].vertices = &triangles[i].a;
    }
  }
  return polygons;
}

//--------------------------------------------------------------------------

