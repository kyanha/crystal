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

#define CS_SYSDEF_PROVIDE_ALLOCA
#include "cssysdef.h"
#include "csgeom/math3d.h"
#include "haze.h"
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

SCF_IMPLEMENT_IBASE (csHazeHull)
  SCF_IMPLEMENTS_INTERFACE (iHazeHull)
SCF_IMPLEMENT_IBASE_END

csHazeHull::csHazeHull()
{
  SCF_CONSTRUCT_IBASE (NULL);
  total_poly = 0;
  total_vert = 0;
  total_edge = 0;
  verts = NULL;
  edgept1 = NULL;
  edgept2 = NULL;
  pol_num = NULL;
  pol_verts = NULL;
  pol_edges = NULL;
}

csHazeHull::~csHazeHull()
{
  /// delete and clear the object
  delete[] verts; verts = NULL;
  delete[] edgept1; edgept1 = NULL;
  delete[] edgept2; edgept2 = NULL;
  for(int p=0; p<total_poly; p++)
  {
    delete[] pol_verts[p]; pol_verts[p] = NULL;
    delete[] pol_edges[p]; pol_edges[p] = NULL;
  }
  delete[] pol_verts; pol_verts = NULL;
  delete[] pol_edges; pol_edges = NULL;
  delete[] pol_num; pol_num = NULL;
  total_poly = 0;
  total_vert = 0;
  total_edge = 0;
}


void csHazeHull::ComputeEdges()
{
  /// create table with edges.
  /// matrix: verticeidx x verticeidx
  int *matrix = new int [total_vert * total_vert];
  int i;
  for(i=0; i<total_vert*total_vert; i++)
    matrix[i] = 0;
  /// mark 1 where vertex goes to another vertex.
  /// (from smaller to larger vertex only)
  int p;
  for(p=0; p<total_poly; p++)
  {
    for(i=0; i<pol_num[p]; i++)
    {
      /// get two indices
      int idx1 = pol_verts[p][i];
      int idx2 = pol_verts[p][ (i+1) % pol_num[p] ];
      /// swap so idx1 <= idx2
      if(idx2>idx1)
      {
        int swp = idx1;
	idx1 = idx2;
	idx2 = swp;
      }
      /// mark
      matrix[ idx1*total_vert + idx2 ] = 1;
    }
  }
  /// use matrix to fill fields.

  total_edge = 0;
  /// count number of 1s
  int j;
  for(i=0; i< total_vert; i++)
    for(j=i; j<total_vert; j++)
    {
      /// i <= j
      if(matrix[ i*total_vert + j ] != 0) 
        total_edge++;
    }
  /// fill edgept tables.
  if(edgept1) delete[] edgept1;
  if(edgept2) delete[] edgept2;
  edgept1 = new int[total_edge];
  edgept2 = new int[total_edge];
  int edge_now = 0;
  for(i=0; i< total_vert; i++)
    for(j=i; j<total_vert; j++)
    {
      /// i <= j
      if(matrix[ i*total_vert + j ] != 0) 
      {
        edgept1[edge_now] = i;
        edgept2[edge_now] = j;
	/// fill matrix with edge_numbers of edge from i<->j
	matrix[ i*total_vert + j ] = edge_now;
	matrix[ j*total_vert + i ] = edge_now;
	/// next edge number
        edge_now ++;
      }
    }
  /// now use matrix filled with edge numbers to get edges for polygons
  /// and alloc pol_edges;
  if(pol_edges)
  {
    for(p=0; p<total_poly; p++)
      delete[] pol_edges[p]; 
    delete[] pol_edges;
  }
  pol_edges = new int* [total_poly];

  for(p=0; p<total_poly; p++)
  {
    pol_edges[p] = new int [pol_num[p]];
    for(i=0; i<pol_num[p]; i++)
    {
      /// get two vert indices
      int i1 = pol_verts[p][i];
      int i2 = pol_verts[p][ (i+1) % pol_num[p] ];
      /// get edge
      int edge_here = matrix[ i1*total_vert + i2 ];
      pol_edges[p][i] = edge_here;
    }
  }

  /// remove temp matrix
  delete[] matrix;
}


void csHazeHull::ComputeOutline(iHazeHull *hull, const csVector3& campos, 
  int& numv, int* pts)
{
  // create some temporary arrays
  numv = 0;
  int *use_edge = new int[hull->GetEdgeCount()];
  int *use_start = new int[hull->GetEdgeCount()];
  int *use_end = new int[hull->GetEdgeCount()];
  int *nextvert = new int[hull->GetVerticeCount()];

  /// no edges used
  int i;
  for(i=0; i<hull->GetEdgeCount(); i++)
    use_edge[i] = 0;

  /// mark number of times an edge is used by visible polygons
  for(int p=0; p<hull->GetPolygonCount(); p++)
  {
    csVector3 v0,v1,v2;
    hull->GetVertex(v0, hull->GetPolVertex(p, 0));
    hull->GetVertex(v1, hull->GetPolVertex(p, 1));
    hull->GetVertex(v2, hull->GetPolVertex(p, 2));
    if(csMath3::WhichSide3D(campos-v0, v1-v0, v2-v0)) {
      for(i=0; i<hull->GetPolVerticeCount(p); i++)
      {
        int edge, i1, i2;
	edge = hull->GetPolEdge(p, i, i1, i2);
	use_edge[edge] ++;
	use_start[edge] = i1;
	use_end[edge] = i2;
      }
    }
  }
  /// connect use_edge where usage is once.
#ifdef CS_DEBUG
  for(i=0; i<hull->GetVerticeCount(); i++)
    nextvert[i] = -1;
#endif
  int startpt = -1;
  for(i=0; i<hull->GetEdgeCount(); i++)
  {
    if(use_edge[i] == 1)
    {
      if(startpt==-1) startpt = use_start[i];
      nextvert[use_start[i]] = use_end[i];
    }
  }

  if(startpt == -1)
  {
    /// no outline, no polygon.
    delete[] use_edge;
    delete[] use_start;
    delete[] use_end;
    delete[] nextvert;
    numv = 0;
    return;
  }

  pts = new int[hull->GetVerticeCount()];
  numv = 0;
  int pt = startpt;
  do
  {
    pts[numv++] = pt;
    pt = nextvert[pt];
    CS_ASSERT(pt != -1);
  }
  while( pt != startpt );

  delete[] use_edge;
  delete[] use_start;
  delete[] use_end;
  delete[] nextvert;
}

//------------ csHazeHullBox -----------------------------------

csHazeHullBox::csHazeHullBox(const csVector3& a, const csVector3& b)
  : csHazeHull()
{
  min = a;
  max = b;
  /// fill with data
  total_vert = 8;
  total_poly = 12;
  verts = new csVector3 [total_vert];
  pol_num = new int [total_poly];
  pol_verts = new int* [total_poly];
  int i;
  for(i=0; i<total_poly; i++)
  {
    pol_num[i] = 3;
    pol_verts[i] = new int [ pol_num[i] ];
  }

  verts[0].Set (min.x, min.y, min.z);
  verts[1].Set (max.x, min.y, min.z);
  verts[2].Set (min.x, max.y, min.z);
  verts[3].Set (max.x, max.y, min.z);
  verts[4].Set (min.x, min.y, max.z);
  verts[5].Set (max.x, min.y, max.z);
  verts[6].Set (min.x, max.y, max.z);
  verts[7].Set (max.x, max.y, max.z);

  pol_verts[0][0] = 0; pol_verts[0][1] = 2; pol_verts[0][2] = 3;
  pol_verts[1][0] = 0; pol_verts[1][1] = 3; pol_verts[1][2] = 1;
  pol_verts[2][0] = 1; pol_verts[2][1] = 3; pol_verts[2][2] = 7;
  pol_verts[3][0] = 1; pol_verts[3][1] = 7; pol_verts[3][2] = 5;
  pol_verts[4][0] = 7; pol_verts[4][1] = 4; pol_verts[4][2] = 5;
  pol_verts[5][0] = 7; pol_verts[5][1] = 6; pol_verts[5][2] = 4;
  pol_verts[6][0] = 6; pol_verts[6][1] = 0; pol_verts[6][2] = 4;
  pol_verts[7][0] = 6; pol_verts[7][1] = 2; pol_verts[7][2] = 0;
  pol_verts[8][0] = 6; pol_verts[8][1] = 7; pol_verts[8][2] = 3;
  pol_verts[9][0] = 6; pol_verts[9][1] = 3; pol_verts[9][2] = 2;
  pol_verts[10][0] = 0; pol_verts[10][1] = 1; pol_verts[10][2] = 4;
  pol_verts[11][0] = 1; pol_verts[11][1] = 5; pol_verts[11][2] = 4;

  ComputeEdges();
}

csHazeHullBox::~csHazeHullBox()
{
}

//------------ csHazeMeshObject -------------------------------

SCF_IMPLEMENT_IBASE (csHazeMeshObject)
  SCF_IMPLEMENTS_INTERFACE (iMeshObject)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iHazeState)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csHazeMeshObject::HazeState)
  SCF_IMPLEMENTS_INTERFACE (iHazeState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csHazeMeshObject::csHazeMeshObject (csHazeMeshObjectFactory* factory)
{
  SCF_CONSTRUCT_IBASE (NULL);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiHazeState);
  csHazeMeshObject::factory = factory;
  ifactory = SCF_QUERY_INTERFACE (factory, iMeshObjectFactory);
  material = factory->GetMaterialWrapper ();
  MixMode = factory->GetMixMode ();
  initialized = false;
  vis_cb = NULL;
  shapenr = 0;
  current_lod = 1;
  current_features = 0;
  origin.Set(0,0,0);
  directional.Set(0,0,0);
}

csHazeMeshObject::~csHazeMeshObject ()
{
  if (vis_cb) vis_cb->DecRef ();
  if (ifactory) ifactory->DecRef ();
}

void csHazeMeshObject::SetupObject ()
{
  if (!initialized)
  {
    initialized = true;
  }
}

bool csHazeMeshObject::DrawTest (iRenderView* rview, iMovable* movable)
{
  SetupObject ();

  if(layers.Length() <= 0) return false;

  // Camera transformation for the single 'position' vector.
  csVector3 cam = rview->GetCamera ()->GetTransform ().Other2This (movable->GetFullPosition ());
  if (cam.z < SMALL_Z) return false;
  return true;
}

void csHazeMeshObject::UpdateLighting (iLight** lights, int num_lights,
    const csVector3& pos)
{
  SetupObject ();
}

void csHazeMeshObject::UpdateLighting (iLight** lights, int num_lights,
    iMovable* movable)
{
  csVector3 pos = movable->GetFullPosition ();
  UpdateLighting (lights, num_lights, pos);
}

#define INTERPOLATE1(component) \
  g3dpoly->vertices [i].##component## = inpoly [vt].##component## + \
    t * (inpoly [vt2].##component## - inpoly [vt].##component##);

#define INTERPOLATE(component) \
{ \
  float v1 = inpoly [edge_from [0]].##component## + \
    t1 * (inpoly [edge_to [0]].##component## - inpoly [edge_from [0]].##component##); \
  float v2 = inpoly [edge_from [1]].##component## + \
    t2 * (inpoly [edge_to [1]].##component## - inpoly [edge_from [1]].##component##); \
  g3dpoly->vertices [i].##component## = v1 + t * (v2 - v1); \
}

static void PreparePolygonFX2 (G3DPolygonDPFX* g3dpoly,
  csVector2* clipped_verts, int num_vertices, csVertexStatus* clipped_vtstats,
  int orig_num_vertices, bool gouraud)
{
  // first we copy the first texture coordinates to a local buffer
  // to avoid that they are overwritten when interpolating.
  ALLOC_STACK_ARRAY (inpoly, G3DTexturedVertex, orig_num_vertices);
  int i;
  for (i = 0; i < orig_num_vertices; i++)
    inpoly[i] = g3dpoly->vertices[i];

  int vt, vt2;
  float t;
  for (i = 0; i < num_vertices; i++)
  {
    g3dpoly->vertices [i].sx = clipped_verts [i].x;
    g3dpoly->vertices [i].sy = clipped_verts [i].y;
    switch (clipped_vtstats[i].Type)
    {
      case CS_VERTEX_ORIGINAL:
        vt = clipped_vtstats[i].Vertex;
        g3dpoly->vertices [i].z = inpoly [vt].z;
        g3dpoly->vertices [i].u = inpoly [vt].u;
        g3dpoly->vertices [i].v = inpoly [vt].v;
	if (gouraud)
	{
          g3dpoly->vertices [i].r = inpoly [vt].r;
          g3dpoly->vertices [i].g = inpoly [vt].g;
          g3dpoly->vertices [i].b = inpoly [vt].b;
	}
	break;
      case CS_VERTEX_ONEDGE:
        vt = clipped_vtstats[i].Vertex;
	vt2 = vt + 1; if (vt2 >= orig_num_vertices) vt2 = 0;
	t = clipped_vtstats [i].Pos;
	INTERPOLATE1 (z);
	INTERPOLATE1 (u);
	INTERPOLATE1 (v);
	if (gouraud)
	{
	  INTERPOLATE1 (r);
	  INTERPOLATE1 (g);
	  INTERPOLATE1 (b);
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
          if ((y >= inpoly [j].sy && y <= inpoly [j1].sy) ||
	      (y <= inpoly [j].sy && y >= inpoly [j1].sy))
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
	G3DTexturedVertex& A = inpoly [edge_from [0]];
	G3DTexturedVertex& B = inpoly [edge_to [0]];
	G3DTexturedVertex& C = inpoly [edge_from [1]];
	G3DTexturedVertex& D = inpoly [edge_to [1]];
	float t1 = (y - A.sy) / (B.sy - A.sy);
	float t2 = (y - C.sy) / (D.sy - C.sy);
	float x1 = A.sx + t1 * (B.sx - A.sx);
	float x2 = C.sx + t2 * (D.sx - C.sx);
	t = (x - x1) / (x2 - x1);
	INTERPOLATE (z);
	INTERPOLATE (u);
	INTERPOLATE (v);
	if (gouraud)
	{
	  INTERPOLATE (r);
	  INTERPOLATE (g);
	  INTERPOLATE (b);
	}
	break;
    }
  }
}

#undef INTERPOLATE
#undef INTERPOLATE1

bool csHazeMeshObject::Draw (iRenderView* rview, iMovable* movable,
	csZBufMode mode)
{
  int i;
  if (!material)
  {
    printf ("INTERNAL ERROR: haze used without material!\n");
    return false;
  }
  iMaterialHandle* mat = material->GetMaterialHandle ();
  if (!mat)
  {
    printf ("INTERNAL ERROR: haze used without valid material handle!\n");
    return false;
  }
  if(layers.Length() <= 0) return false;

  if (vis_cb) if (!vis_cb->BeforeDrawing (this, rview)) return false;

  /// preapre to transform the points
  iGraphics3D* g3d = rview->GetGraphics3D ();
  iCamera* camera = rview->GetCamera ();
  csVector3 campos = camera->GetTransform().GetOrigin();
  float fov = camera->GetFOV ();
  float shx = camera->GetShiftX ();
  float shy = camera->GetShiftY ();
  /// obj to camera space
  csReversibleTransform tr_o2c = camera->GetTransform ()
        * movable->GetFullTransform ().GetInverse ();

  // Prepare for rendering.
  g3d->SetRenderState (G3DRENDERSTATE_ZBUFFERMODE, mode);
  material->Visit ();

  // project to screenspace
  csVector3 scr_orig;
  ProjectO2S(tr_o2c, fov, shx, shy, origin, scr_orig);

  // get hull 0 outline in screenspace
  iHazeHull *hull = layers.GetLayer(0)->hull;
  int layer_num = 0;
  int *layer_poly = 0;
  csHazeHull::ComputeOutline(hull, campos, layer_num, layer_poly);
  if(layer_num <= 0) return false;
  csVector3* layer_pts = new csVector3[layer_num];
  for(i=0; i<layer_num; i++)
  {
    csVector3 objpos;
    hull->GetVertex(objpos, layer_poly[i] );
    ProjectO2S(tr_o2c, fov, shx, shy, objpos, layer_pts[i]);
  }
  // get hull 0 uv values
  float layer_scale = layers.GetLayer(0)->scale;
  csVector2* layer_uvs = new csVector2[layer_num];
  csVector2 center(0.5, 0.5);
  csVector2 dir;
  for(i=0; i<layer_num; i++)
  {
    dir.x = layer_pts[i].x - scr_orig.x;
    dir.y = layer_pts[i].y - scr_orig.y;
    dir /= dir.Norm();
    layer_uvs[i] = center + dir * layer_scale;
  }

  // draw triangles from orig to layer 0
  csVector3 tri_pts[3];
  csVector2 tri_uvs[3];
  tri_pts[0] = scr_orig;
  tri_uvs[0] = center;
  for(i=0; i<layer_num; i++)
  {
    int nexti = (i+1)%layer_num;
    tri_pts[1] = layer_pts[i];
    tri_pts[2] = layer_pts[nexti];
    tri_uvs[1] = layer_uvs[i];
    tri_uvs[2] = layer_uvs[nexti];
    DrawPoly(rview, g3d, mat, 3, tri_pts, tri_uvs);
  }

  delete[] layer_poly;
  delete[] layer_pts;
  delete[] layer_uvs;

  return true;
}
  
void csHazeMeshObject::ProjectO2S(csReversibleTransform& tr_o2c, float fov,
  float shiftx, float shifty, const csVector3& objpos, csVector3& scrpos)
{
  scrpos = tr_o2c * objpos;  // to camera space
  scrpos.z = fov / scrpos.z; // = iz
  scrpos.x = scrpos.x * scrpos.z + shiftx;
  scrpos.y = scrpos.y * scrpos.z + shifty;
}

void csHazeMeshObject::DrawPoly(iRenderView *rview, iGraphics3D *g3d, 
  iMaterialHandle *mat, int num, const csVector3* pts, const csVector2* uvs)
{
  g3dpolyfx.num = num;
  g3dpolyfx.mat_handle = mat;
  g3dpolyfx.mat_handle->GetTexture ()->GetMeanColor (g3dpolyfx.flat_color_r,
    g3dpolyfx.flat_color_g, g3dpolyfx.flat_color_b);

  ALLOC_STACK_ARRAY (poly2d, csVector2, num);
  csVector2 clipped_poly2d[MAX_OUTPUT_VERTICES];
  csVertexStatus clipped_vtstats[MAX_OUTPUT_VERTICES];

  int i;
  for(i = 0; i < num; i++)
  {
    g3dpolyfx.vertices [i].sx = pts[i].x;
    g3dpolyfx.vertices [i].sy = pts[i].y;
    g3dpolyfx.vertices [i].z = pts[i].z;
    g3dpolyfx.vertices [i].r = 1.0;
    g3dpolyfx.vertices [i].g = 1.0;
    g3dpolyfx.vertices [i].b = 1.0;
    g3dpolyfx.vertices [i].u = uvs [i].x;
    g3dpolyfx.vertices [i].v = uvs [i].y;
  }

  int num_clipped_verts;
  UByte clip_result = rview->GetClipper ()->Clip (poly2d, num,
    clipped_poly2d, num_clipped_verts, clipped_vtstats);
  if (clip_result == CS_CLIP_OUTSIDE) return;
  g3dpolyfx.num = num_clipped_verts;

  if (clip_result != CS_CLIP_INSIDE)
    PreparePolygonFX2 (&g3dpolyfx, clipped_poly2d, num_clipped_verts,
    	clipped_vtstats, num, false); // no gouraud interpol, no true);

  rview->CalculateFogPolygon (g3dpolyfx);
  g3dpolyfx.mixmode = MixMode; // | CS_FX_GOURAUD;
  g3d->DrawPolygonFX (g3dpolyfx);

}

void csHazeMeshObject::GetObjectBoundingBox (csBox3& /*bbox*/, int /*type*/)
{
  SetupObject ();
  //@@@ TODO
  //bbox = object_bbox;
}

void csHazeMeshObject::HardTransform (const csReversibleTransform& t)
{
  (void)t;
  //@@@ TODO
}

void csHazeMeshObject::NextFrame (csTicks /*current_time*/)
{
  // nothing
}

//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csHazeMeshObjectFactory)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectFactory)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iHazeFactoryState)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csHazeMeshObjectFactory::HazeFactoryState)
  SCF_IMPLEMENTS_INTERFACE (iHazeFactoryState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csHazeMeshObjectFactory::csHazeMeshObjectFactory (iBase *pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiHazeFactoryState);
  material = NULL;
  MixMode = 0;
  origin.Set(0,0,0);
  directional.Set(0,0,0);
}

csHazeMeshObjectFactory::~csHazeMeshObjectFactory ()
{
}

iMeshObject* csHazeMeshObjectFactory::NewInstance ()
{
  csHazeMeshObject* cm = new csHazeMeshObject (this);
  iMeshObject* im = SCF_QUERY_INTERFACE (cm, iMeshObject);
  im->DecRef ();
  return im;
}

//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csHazeMeshObjectType)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectType)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iPlugin)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csHazeMeshObjectType::eiPlugin)
  SCF_IMPLEMENTS_INTERFACE (iPlugin)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csHazeMeshObjectType)

SCF_EXPORT_CLASS_TABLE (haze)
  SCF_EXPORT_CLASS (csHazeMeshObjectType, "crystalspace.mesh.object.haze",
    "Crystal Space Haze Mesh Type")
SCF_EXPORT_CLASS_TABLE_END

csHazeMeshObjectType::csHazeMeshObjectType (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiPlugin);
}

csHazeMeshObjectType::~csHazeMeshObjectType ()
{
}

iMeshObjectFactory* csHazeMeshObjectType::NewFactory ()
{
  csHazeMeshObjectFactory* cm = new csHazeMeshObjectFactory (this);
  iMeshObjectFactory* ifact = SCF_QUERY_INTERFACE (cm, iMeshObjectFactory);
  ifact->DecRef ();
  return ifact;
}

