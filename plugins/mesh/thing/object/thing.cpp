/*
    Copyright (C) 1998-2002 by Jorrit Tyberghein

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
#include "cssys/csendian.h"
#include "qint.h"
#include "thing.h"
#include "polygon.h"
#include "pol2d.h"
#include "polytext.h"
#include "lppool.h"
#include "csgeom/polypool.h"
#include "csgeom/poly3d.h"
#include "csgeom/frustum.h"
#include "iengine/light.h"
#include "iengine/engine.h"
#include "iengine/sector.h"
#include "iengine/movable.h"
#include "iengine/material.h"
#include "iengine/mesh.h"
#include "iengine/camera.h"
#include "iengine/statlght.h"
#include "iengine/dynlight.h"
#include "iengine/shadows.h"
#include "csgeom/sphere.h"
#include "csgeom/math3d.h"
#include "csutil/csstring.h"
#include "csutil/memfile.h"
#include "csutil/hashmap.h"
#include "csutil/debug.h"
#include "csutil/csmd5.h"
#include "csutil/array.h"
#include "csutil/garray.h"
#include "ivideo/txtmgr.h"
#include "ivideo/vbufmgr.h"
#include "ivideo/texture.h"
#include "iengine/texture.h"
#include "iengine/shadcast.h"
#include "iutil/vfs.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "iutil/cache.h"
#include "iutil/cmdline.h"
#include "iengine/rview.h"
#include "iengine/fview.h"
#include "qsqrt.h"
#include "ivideo/graph3d.h"
#include "ivaria/reporter.h"

CS_IMPLEMENT_PLUGIN

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csThing)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iThingState)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iLightingInfo)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iPolygonMesh)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iShadowCaster)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iShadowReceiver)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iMeshObject)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csThing::ThingState)
  SCF_IMPLEMENTS_INTERFACE(iThingState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csThing::LightingInfo)
  SCF_IMPLEMENTS_INTERFACE(iLightingInfo)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csThing::PolyMesh)
  SCF_IMPLEMENTS_INTERFACE(iPolygonMesh)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csThing::ShadowCaster)
  SCF_IMPLEMENTS_INTERFACE(iShadowCaster)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csThing::ShadowReceiver)
  SCF_IMPLEMENTS_INTERFACE(iShadowReceiver)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csThing::MeshObject)
  SCF_IMPLEMENTS_INTERFACE(iMeshObject)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

int csThing:: last_thing_id = 0;

//----------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csThingStatic)
  SCF_IMPLEMENTS_INTERFACE(iThingFactoryState)
  SCF_IMPLEMENTS_INTERFACE(iMeshObjectFactory)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iObjectModel)
  {
    static scfInterfaceID iPolygonMesh_scfID = (scfInterfaceID)-1;		
    if (iPolygonMesh_scfID == (scfInterfaceID)-1)				
      iPolygonMesh_scfID = iSCF::SCF->GetInterfaceID ("iPolygonMesh");		
    if (iInterfaceID == iPolygonMesh_scfID &&				
      scfCompatibleVersion (iVersion, iPolygonMesh_VERSION))		
    {
#ifdef CS_DEBUG
      printf ("Deprecated feature use: iPolygonMesh queried from Thing; "
	"use iObjectModel->GetPolygonMeshColldet() instead.\n");
#endif
      iPolygonMesh* Object = scfiObjectModel.GetPolygonMeshColldet();
      (Object)->IncRef ();						
      return STATIC_CAST(iPolygonMesh*, Object);				
    }
  }
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csThingStatic::ObjectModel)
  SCF_IMPLEMENTS_INTERFACE(iObjectModel)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csThingStatic::csThingStatic (iBase* parent, csThingObjectType* thing_type)
	: static_polygons (32, 64)
{
  SCF_CONSTRUCT_IBASE (parent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObjectModel);
  csThingStatic::thing_type = thing_type;
  static_polygons.SetThingType (thing_type);

  scfiPolygonMesh.SetThing (this);
  scfiPolygonMeshLOD.SetThing (this);
  scfiObjectModel.SetPolygonMeshBase (&scfiPolygonMesh);
  scfiObjectModel.SetPolygonMeshColldet (&scfiPolygonMesh);
  scfiObjectModel.SetPolygonMeshViscull (&scfiPolygonMeshLOD);

  max_vertices = num_vertices = 0;
  obj_verts = NULL;
  obj_normals = NULL;
  smoothed = false;

  obj_bbox_valid = false;

  prepared = false;
  cosinus_factor = -1;
  logparent = NULL;
}

csThingStatic::~csThingStatic ()
{
  delete[] obj_verts;
  delete[] obj_normals;
}

void csThingStatic::Prepare ()
{
  if (prepared) return;
  prepared = true;

  if (thing_type->engine)
  {
    if (static_polygons.Length () >= thing_type->engine->
    	GetFastMeshThresshold () &&
	portal_polygons.Length () == 0)
    {
      flags.Set (CS_THING_FASTMESH);
    }
  }

  if (!flags.Check (CS_THING_NOCOMPRESS))
  {
    CompressVertices ();
    RemoveUnusedVertices ();
  }

  if (smoothed)
    CalculateNormals();

  int i;
  csPolygon3DStatic* sp;
  portal_polygons.DeleteAll ();
  for (i = 0; i < static_polygons.Length (); i++)
  {
    sp = static_polygons.Get (i);
    // If a Finish() call returns false this means the textures are not
    // completely ready yet. In that case we set 'prepared' to false
    // again so that we force a new prepare later.
    if (!sp->Finish ())
      prepared = false;
    if (sp->GetPortal ())
      portal_polygons.Push (i);
  }
}

void csThingStatic::UpdatePortalList ()
{
  int i;
  csPolygon3DStatic* sp;
  portal_polygons.DeleteAll ();
  for (i = 0; i < static_polygons.Length (); i++)
  {
    sp = static_polygons.Get (i);
    if (sp->GetPortal ())
      portal_polygons.Push (i);
  }
}

int csThingStatic::AddVertex (float x, float y, float z)
{
  if (!obj_verts)
  {
    max_vertices = 10;
    obj_verts = new csVector3[max_vertices];
  }

  while (num_vertices >= max_vertices)
  {
    if (max_vertices < 10000)
      max_vertices *= 2;
    else
      max_vertices += 10000;

    csVector3 *new_obj_verts = new csVector3[max_vertices];
    memcpy (new_obj_verts, obj_verts, sizeof (csVector3) * num_vertices);
    delete[] obj_verts;
    obj_verts = new_obj_verts;
  }

  obj_verts[num_vertices].Set (x, y, z);
  num_vertices++;
  scfiObjectModel.ShapeChanged ();
  return num_vertices - 1;
}

void csThingStatic::SetVertex (int idx, const csVector3 &vt)
{
  CS_ASSERT (idx >= 0 && idx < num_vertices);
  obj_verts[idx] = vt;
  scfiObjectModel.ShapeChanged ();
}

void csThingStatic::DeleteVertex (int idx)
{
  CS_ASSERT (idx >= 0 && idx < num_vertices);

  int copysize = sizeof (csVector3) * (num_vertices - idx - 1);
  memmove (obj_verts + idx, obj_verts + idx + 1, copysize);
  scfiObjectModel.ShapeChanged ();
}

void csThingStatic::DeleteVertices (int from, int to)
{
  if (from <= 0 && to >= num_vertices - 1)
  {
    // Delete everything.
    delete[] obj_verts;
    max_vertices = num_vertices = 0;
    obj_verts = NULL;
  }
  else
  {
    if (from < 0) from = 0;
    if (to >= num_vertices) to = num_vertices - 1;

    int rangelen = to - from + 1;
    int copysize = sizeof (csVector3) * (num_vertices - from - rangelen);
    memmove (obj_verts + from,
    	obj_verts + from + rangelen, copysize);
    num_vertices -= rangelen;
  }

  scfiObjectModel.ShapeChanged ();
}

struct CompressVertex
{
  int orig_idx;
  float x, y, z;
  int new_idx;
  bool used;
};

static int compare_vt (const void *p1, const void *p2)
{
  CompressVertex *sp1 = (CompressVertex *)p1;
  CompressVertex *sp2 = (CompressVertex *)p2;
  if (sp1->x < sp2->x)
    return -1;
  else if (sp1->x > sp2->x)
    return 1;
  if (sp1->y < sp2->y)
    return -1;
  else if (sp1->y > sp2->y)
    return 1;
  if (sp1->z < sp2->z)
    return -1;
  else if (sp1->z > sp2->z)
    return 1;
  return 0;
}

static int compare_vt_orig (const void *p1, const void *p2)
{
  CompressVertex *sp1 = (CompressVertex *)p1;
  CompressVertex *sp2 = (CompressVertex *)p2;
  if (sp1->orig_idx < sp2->orig_idx)
    return -1;
  else if (sp1->orig_idx > sp2->orig_idx)
    return 1;
  return 0;
}

void csThingStatic::CompressVertices ()
{
  if (num_vertices <= 0) return ;

  // Copy all the vertices.
  CompressVertex *vt = new CompressVertex[num_vertices];
  int i, j;
  for (i = 0; i < num_vertices; i++)
  {
    vt[i].orig_idx = i;
    vt[i].x = (float)ceil (obj_verts[i].x * 1000000);
    vt[i].y = (float)ceil (obj_verts[i].y * 1000000);
    vt[i].z = (float)ceil (obj_verts[i].z * 1000000);
  }

  // First sort so that all (nearly) equal vertices are together.
  qsort (vt, num_vertices, sizeof (CompressVertex), compare_vt);

  // Count unique values and tag all doubles with the index of the unique one.
  // new_idx in the vt table will be the index inside vt to the unique vector.
  int count_unique = 1;
  int last_unique = 0;
  vt[0].new_idx = last_unique;
  for (i = 1; i < num_vertices; i++)
  {
    if (
      vt[i].x != vt[last_unique].x ||
      vt[i].y != vt[last_unique].y ||
      vt[i].z != vt[last_unique].z)
    {
      last_unique = i;
      count_unique++;
    }

    vt[i].new_idx = last_unique;
  }

  // If count_unique == num_vertices then there is nothing to do.
  if (count_unique == num_vertices)
  {
    delete[] vt;
    return ;
  }

  // Now allocate and fill new vertex tables.
  // After this new_idx in the vt table will be the new index
  // of the vector.
  csVector3 *new_obj = new csVector3[count_unique];
  new_obj[0] = obj_verts[vt[0].orig_idx];

  vt[0].new_idx = 0;
  j = 1;
  for (i = 1; i < num_vertices; i++)
  {
    if (vt[i].new_idx == i)
    {
      new_obj[j] = obj_verts[vt[i].orig_idx];
      vt[i].new_idx = j;
      j++;
    }
    else
    {
      vt[i].new_idx = j - 1;
    }
  }

  // Now we sort the table back on orig_idx so that we have
  // a mapping from the original indices to the new one (new_idx).
  qsort (vt, num_vertices, sizeof (CompressVertex),
  	compare_vt_orig);

  // Replace the old vertex tables.
  delete[] obj_verts;
  obj_verts = new_obj;
  num_vertices = max_vertices = count_unique;

  // Now we can remap the vertices in all polygons.
  for (i = 0; i < static_polygons.Length (); i++)
  {
    csPolygon3DStatic *p = static_polygons.Get (i);
    csPolyIndexed &pi = p->GetVertices ();
    int *idx = pi.GetVertexIndices ();
    for (j = 0; j < pi.GetVertexCount (); j++) idx[j] = vt[idx[j]].new_idx;
  }

  delete[] vt;
  scfiObjectModel.ShapeChanged ();
}

void csThingStatic::RemoveUnusedVertices ()
{
  if (num_vertices <= 0) return ;

  // Copy all the vertices that are actually used by polygons.
  bool *used = new bool[num_vertices];
  int i, j;
  for (i = 0; i < num_vertices; i++) used[i] = false;

  // Mark all vertices that are used as used.
  for (i = 0; i < static_polygons.Length (); i++)
  {
    csPolygon3DStatic *p = static_polygons.Get (i);
    csPolyIndexed &pi = p->GetVertices ();
    int *idx = pi.GetVertexIndices ();
    for (j = 0; j < pi.GetVertexCount (); j++) used[idx[j]] = true;
  }

  // Count relevant values.
  int count_relevant = 0;
  for (i = 0; i < num_vertices; i++)
  {
    if (used[i]) count_relevant++;
  }

  // If all vertices are relevant then there is nothing to do.
  if (count_relevant == num_vertices)
  {
    delete[] used;
    return ;
  }

  // Now allocate and fill new vertex tables.
  // Also fill the 'relocate' table.
  csVector3 *new_obj = new csVector3[count_relevant];
  int *relocate = new int[num_vertices];
  j = 0;
  for (i = 0; i < num_vertices; i++)
  {
    if (used[i])
    {
      new_obj[j] = obj_verts[i];
      relocate[i] = j;
      j++;
    }
    else
      relocate[i] = -1;
  }

  // Replace the old vertex tables.
  delete[] obj_verts;
  obj_verts = new_obj;
  num_vertices = max_vertices = count_relevant;

  // Now we can remap the vertices in all polygons.
  for (i = 0; i < static_polygons.Length (); i++)
  {
    csPolygon3DStatic *p = static_polygons.Get (i);
    csPolyIndexed &pi = p->GetVertices ();
    int *idx = pi.GetVertexIndices ();
    for (j = 0; j < pi.GetVertexCount (); j++) idx[j] = relocate[idx[j]];
  }

  delete[] relocate;
  delete[] used;

  obj_bbox_valid = false;
  scfiObjectModel.ShapeChanged ();
}

struct PolygonsForVertex
{
  csArray<int> poly_indices;
};

void csThingStatic::CalculateNormals ()
{
  int polyCount = static_polygons.Length();
  int i, j;

  delete[] obj_normals;
  obj_normals = new csVector3[num_vertices];

  // First build a table so that we can find all polygons that connect
  // to a vertex easily.
  PolygonsForVertex* pvv = new PolygonsForVertex[num_vertices];
  for (i = 0 ; i < polyCount ; i++)
  {
    csPolygon3DStatic* p = static_polygons.Get (i);
    int* vtidx = p->GetVertexIndices ();
    for (j = 0 ; j < p->GetVertexCount () ; j++)
    {
      CS_ASSERT (vtidx[j] >= 0 && vtidx[j] < num_vertices);
      pvv[vtidx[j]].poly_indices.Push (i);
    }
  }

  // Now calculate normals.
  for (i = 0 ; i < num_vertices ; i++)
  {
    csVector3 n (0);
    for (j = 0 ; j < pvv[i].poly_indices.Length () ; j++)
    {
      csPolygon3DStatic* p = static_polygons.Get (pvv[i].poly_indices[j]);
      const csVector3& normal = p->GetObjectPlane ().Normal();
      n += normal;
    }
    float norm = n.Norm ();
    if (norm) n /= norm;
    obj_normals[i] = n;
  }

  delete[] pvv;
}

int csThingStatic::FindPolygonIndex (iPolygon3DStatic *polygon) const
{
  csPolygon3DStatic *p = polygon->GetPrivateObject ();
  return static_polygons.Find (p);
}

iPolygon3DStatic *csThingStatic::GetPolygon (int idx)
{
  return &(static_polygons.Get (idx)->scfiPolygon3DStatic);
}

iPolygon3DStatic *csThingStatic::GetPolygon (const char* name)
{
  int idx = static_polygons.FindKey (name);
  return idx >= 0 ? &(static_polygons.Get (idx)->scfiPolygon3DStatic) : NULL;
}

void csThingStatic::AddPolygon (csPolygon3DStatic* spoly)
{
  spoly->SetParent (this);
  spoly->EnableTextureMapping (true);
  static_polygons.Push (spoly);
  scfiObjectModel.ShapeChanged ();
}

iPolygon3DStatic *csThingStatic::CreatePolygon (const char *name)
{
  csPolygon3DStatic* sp = thing_type->blk_polygon3dstatic.Alloc ();
  if (name) sp->SetName (name);
  AddPolygon (sp);
  return &(sp->scfiPolygon3DStatic);
}

void csThingStatic::RemovePolygon (int idx)
{
  static_polygons.Delete (idx);
  scfiObjectModel.ShapeChanged ();
}

void csThingStatic::RemovePolygons ()
{
  static_polygons.DeleteAll ();
  portal_polygons.DeleteAll ();
  scfiObjectModel.ShapeChanged ();
}

iPortal *csThingStatic::GetPortal (int idx) const
{
  csPolygon3DStatic *p = static_polygons.Get (portal_polygons[idx]);
  return &(p->GetPortal ()->scfiPortal);
}

iPolygon3DStatic *csThingStatic::GetPortalPolygon (int idx) const
{
  csPolygon3DStatic *p = (static_polygons.Get (portal_polygons[idx]));
  return &(p->scfiPolygon3DStatic);
}

int csThingStatic::IntersectSegmentIndex (
  const csVector3 &start, const csVector3 &end,
  csVector3 &isect,
  float *pr,
  bool only_portals)
{
  if (only_portals)
  {
    int i;
    float r, best_r = 2000000000.;
    csVector3 cur_isect;
    int best_p = -1;
    for (i = 0 ; i < portal_polygons.Length () ; i++)
    {
      int polygon_index = portal_polygons[i];
      csPolygon3DStatic *p = static_polygons.Get (polygon_index);
      if (p->IntersectSegment (start, end, cur_isect, &r))
      {
        if (r < best_r)
        {
          best_r = r;
          best_p = polygon_index;
          isect = cur_isect;
        }
      }
    }

    if (pr) *pr = best_r;
    return best_p;
  }

  int i;
  float r, best_r = 2000000000.;
  csVector3 cur_isect;
  int best_p = -1;

  // @@@ This routine is not very optimal. Especially for things
  // with large number of polygons.
  for (i = 0; i < static_polygons.Length (); i++)
  {
    csPolygon3DStatic *p = static_polygons.Get (i);
    if (p->IntersectSegment (start, end, cur_isect, &r))
    {
      if (r < best_r)
      {
        best_r = r;
        best_p = i;
        isect = cur_isect;
      }
    }
  }

  if (pr) *pr = best_r;
  return best_p;
}

iPolygon3DStatic* csThingStatic::IntersectSegment (
	const csVector3& start,
	const csVector3& end, csVector3& isect,
	float* pr, bool only_portals)
{
  int p = IntersectSegmentIndex (start, end, isect, pr,
  	only_portals);
  return p != -1 ? &(static_polygons.Get (p)->scfiPolygon3DStatic) : NULL;
}

csPtr<csThingStatic> csThingStatic::Clone ()
{
  csThingStatic* clone = new csThingStatic (scfParent, thing_type);
  clone->flags.SetAll (GetFlags ().Get ());
  clone->smoothed = smoothed;
  clone->obj_bbox = obj_bbox;
  clone->obj_bbox_valid = obj_bbox_valid;
  clone->obj_radius = obj_radius;
  clone->max_obj_radius = max_obj_radius;
  clone->prepared = prepared;
  clone->scfiObjectModel.SetShapeNumber (scfiObjectModel.GetShapeNumber ());
  clone->cosinus_factor = cosinus_factor;

  clone->num_vertices = num_vertices;
  clone->max_vertices = max_vertices;
  if (obj_verts)
  {
    clone->obj_verts = new csVector3[max_vertices];
    memcpy (clone->obj_verts, obj_verts, sizeof (csVector3)*num_vertices);
  }
  else
  {
    clone->obj_verts = NULL;
  }
  if (obj_normals)
  {
    clone->obj_normals = new csVector3[max_vertices];
    memcpy (clone->obj_normals, obj_normals, sizeof (csVector3)*num_vertices);
  }
  else
  {
    clone->obj_normals = NULL;
  }

  int i;
  for (i = 0 ; i < static_polygons.Length () ; i++)
  {
    csPolygon3DStatic* p = static_polygons.Get (i)->Clone ();
    p->SetParent (clone);
    clone->static_polygons.Push (p);
  }

  clone->portal_polygons = portal_polygons;

  return csPtr<csThingStatic> (clone);
}

void csThingStatic::HardTransform (const csReversibleTransform &t)
{
  int i;

  for (i = 0; i < num_vertices; i++)
  {
    obj_verts[i] = t.This2Other (obj_verts[i]);
  }

  //-------
  // Now transform the polygons.
  //-------
  for (i = 0; i < static_polygons.Length (); i++)
  {
    csPolygon3DStatic *p = GetPolygon3DStatic (i);
    p->HardTransform (t);
  }

  scfiObjectModel.ShapeChanged ();
}

csPtr<iMeshObject> csThingStatic::NewInstance ()
{
  csThing *thing = new csThing ((iBase*)(iThingFactoryState*)this, this);
  return csPtr<iMeshObject> (&thing->scfiMeshObject);
}

void csThingStatic::GetBoundingBox (csBox3 &box)
{
  int i;

  if (obj_bbox_valid)
  {
    box = obj_bbox;
    return ;
  }

  obj_bbox_valid = true;

  if (!obj_verts)
  {
    obj_bbox.Set (0, 0, 0, 0, 0, 0);
    box = obj_bbox;
    return ;
  }

  if (num_vertices > 0)
  {
    obj_bbox.StartBoundingBox (obj_verts[0]);
    for (i = 1; i < num_vertices; i++)
    {
      obj_bbox.AddBoundingVertexSmart (obj_verts[i]);
    }
  }

  obj_radius = (obj_bbox.Max () - obj_bbox.Min ()) * 0.5f;
  max_obj_radius = qsqrt (csSquaredDist::PointPoint (
  	obj_bbox.Max (), obj_bbox.Min ())) * 0.5f;
  box = obj_bbox;
}

void csThingStatic::GetRadius (csVector3 &rad, csVector3 &cent)
{
  csBox3 b;
  GetBoundingBox (b);
  rad = obj_radius;
  cent = b.GetCenter ();
}

//----------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csThingStatic::PolyMesh)
  SCF_IMPLEMENTS_INTERFACE(iPolygonMesh)
SCF_IMPLEMENT_IBASE_END

csThingStatic::PolyMesh::PolyMesh () : PolyMeshHelper (CS_POLY_COLLDET) 
{ 
  SCF_CONSTRUCT_IBASE (NULL);
}

//----------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csThingStatic::PolyMeshLOD)
  SCF_IMPLEMENTS_INTERFACE(iPolygonMesh)
SCF_IMPLEMENT_IBASE_END

csThingStatic::PolyMeshLOD::PolyMeshLOD () : PolyMeshHelper (CS_POLY_VISCULL)
{
  SCF_CONSTRUCT_IBASE (NULL);
}

//-------------------------------------------------------------------------

csThing::csThing (iBase *parent, csThingStatic* static_data) : polygons(32, 64)
{
  SCF_CONSTRUCT_IBASE (parent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiThingState);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiLightingInfo);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiPolygonMesh);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiShadowCaster);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiShadowReceiver);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiMeshObject);
  DG_TYPE (this, "csThing");

  csThing::static_data = static_data;
  polygons.SetThingType (static_data->thing_type);

  scfiPolygonMesh.SetThing (static_data);
  scfiPolygonMeshLOD.SetThing (static_data);

  last_thing_id++;
  thing_id = last_thing_id;
  logparent = NULL;

  wor_verts = NULL;
  cam_verts = NULL;
  num_cam_verts = 0;

  draw_busy = 0;

  dynamic_ambient.Set (0,0,0);
  light_version = 1;

  ParentTemplate = NULL;

  cameranr = -1;
  movablenr = -1;
  wor_bbox_movablenr = -1;
  cached_movable = NULL;

  cfg_moving = CS_THING_MOVE_NEVER;

  prepared = false;
  static_data_nr = 0xfffffffd;	// (static_nr of csThingStatic is init to -1)

  current_lod = 1;
  current_features = 0;

#ifndef CS_USE_NEW_RENDERER
  polybuf = NULL;
#endif // CS_USE_NEW_RENDERER
  polybuf_materials = NULL;

  current_visnr = 1;
}

csThing::~csThing ()
{
#ifndef CS_USE_NEW_RENDERER
  if (polybuf) polybuf->DecRef ();
#endif // CS_USE_NEW_RENDERER
  delete[] polybuf_materials;

  if (wor_verts != static_data->obj_verts)
  {
    delete[] wor_verts;
  }

  delete[] cam_verts;

  polygons.DeleteAll ();          // delete prior to portal_poly array !
}

char* csThing::GenerateCacheName ()
{
  csBox3 b;
  static_data->GetBoundingBox (b);

  csMemFile mf;
  int32 l;
  l = convert_endian ((int32)static_data->num_vertices);
  mf.Write ((char*)&l, 4);
  l = convert_endian ((int32)polygons.Length ());
  mf.Write ((char*)&l, 4);

  if (logparent)
  {
    csRef<iMeshWrapper> mw (SCF_QUERY_INTERFACE (logparent, iMeshWrapper));
    if (mw)
    {
      if (mw->QueryObject ()->GetName ())
        mf.Write (mw->QueryObject ()->GetName (),
		strlen (mw->QueryObject ()->GetName ()));
      iSector* sect = mw->GetMovable ()->GetSectors ()->Get (0);
      if (sect && sect->QueryObject ()->GetName ())
        mf.Write (sect->QueryObject ()->GetName (),
		strlen (sect->QueryObject ()->GetName ()));
    }
  }

  l = convert_endian ((int32)QInt ((b.MinX () * 1000)+.5));
  mf.Write ((char*)&l, 4);
  l = convert_endian ((int32)QInt ((b.MinY () * 1000)+.5));
  mf.Write ((char*)&l, 4);
  l = convert_endian ((int32)QInt ((b.MinZ () * 1000)+.5));
  mf.Write ((char*)&l, 4);
  l = convert_endian ((int32)QInt ((b.MaxX () * 1000)+.5));
  mf.Write ((char*)&l, 4);
  l = convert_endian ((int32)QInt ((b.MaxY () * 1000)+.5));
  mf.Write ((char*)&l, 4);
  l = convert_endian ((int32)QInt ((b.MaxZ () * 1000)+.5));
  mf.Write ((char*)&l, 4);

  csMD5::Digest digest = csMD5::Encode (mf.GetData (), mf.GetSize ());

  char* cachename = new char[33];
  sprintf (cachename,
  	"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
  	digest.data[0], digest.data[1], digest.data[2], digest.data[3],
  	digest.data[4], digest.data[5], digest.data[6], digest.data[7],
  	digest.data[8], digest.data[9], digest.data[10], digest.data[11],
  	digest.data[12], digest.data[13], digest.data[14], digest.data[15]);
  return cachename;
}

void csThing::MarkLightmapsDirty ()
{
#ifndef CS_USE_NEW_RENDERER
  if (polybuf)
    polybuf->MarkLightmapsDirty ();
#endif // CS_USE_NEW_RENDERER
  light_version++;
}

void csThing::DynamicLightChanged (iDynLight* /*dynlight*/)
{
  MarkLightmapsDirty ();
}

void csThing::DynamicLightDisconnect (iDynLight* dynlight)
{
  MarkLightmapsDirty ();
  int i;
  for (i = 0 ; i < polygons.Length () ; i++)
  {
    csPolygon3D *p = GetPolygon3D (i);
    p->DynamicLightDisconnect (dynlight);
  }
}

void csThing::StaticLightChanged (iStatLight* /*statlight*/)
{
  MarkLightmapsDirty ();
}

void csThing::SetMovingOption (int opt)
{
  cfg_moving = opt;
  switch (cfg_moving)
  {
    case CS_THING_MOVE_NEVER:
      if (wor_verts != static_data->obj_verts) delete[] wor_verts;
      wor_verts = static_data->obj_verts;
      break;

    case CS_THING_MOVE_OCCASIONAL:
      if ((wor_verts == NULL || wor_verts == static_data->obj_verts)
      	&& static_data->max_vertices)
      {
        wor_verts = new csVector3[static_data->max_vertices];
        memcpy (wor_verts, static_data->obj_verts,
		static_data->max_vertices * sizeof (csVector3));
      }

      cached_movable = NULL;
      break;
  }

  movablenr = -1;                 // @@@ Is this good?
}

void csThing::WorUpdate ()
{
  int i;
  switch (cfg_moving)
  {
    case CS_THING_MOVE_NEVER:
      return ;

    case CS_THING_MOVE_OCCASIONAL:
      if (cached_movable && cached_movable->GetUpdateNumber () != movablenr)
      {
        movablenr = cached_movable->GetUpdateNumber ();

	if (cached_movable->IsFullTransformIdentity ())
	{
	  memcpy (wor_verts, static_data->obj_verts,
	  	static_data->num_vertices * (sizeof (csVector3)));
	  csReversibleTransform movtrans;	// Identity.
	  // @@@ It is possible to optimize the below too. Don't know
	  // if it is worth it though.
	  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
          for (i = 0; i < polygons.Length (); i++)
          {
            csPolygon3D *p = GetPolygon3D (i);
            p->ObjectToWorld (movtrans, p->Vwor (0));
          }
	}
	else
	{
          csReversibleTransform movtrans = cached_movable->GetFullTransform ();
          for (i = 0; i < static_data->num_vertices; i++)
            wor_verts[i] = movtrans.This2Other (static_data->obj_verts[i]);
          for (i = 0; i < polygons.Length (); i++)
          {
            csPolygon3D *p = GetPolygon3D (i);
            p->ObjectToWorld (movtrans, p->Vwor (0));
          }
	}

        // If the movable changed we invalidate the camera number as well
        // to make sure the camera vertices are recalculated as well.
        cameranr--;
      }
      break;
  }
}

void csThing::UpdateTransformation (const csTransform &c, long cam_cameranr)
{
  if (!cam_verts || static_data->num_vertices != num_cam_verts)
  {
    delete[] cam_verts;
    cam_verts = new csVector3[static_data->num_vertices];
    num_cam_verts = static_data->num_vertices;
    cameranr = cam_cameranr - 1;  // To make sure we will transform.
  }

  if (cameranr != cam_cameranr)
  {
    cameranr = cam_cameranr;

    int i;
    for (i = 0; i < static_data->num_vertices; i++)
      cam_verts[i] = c.Other2This (wor_verts[i]);
  }
}

void csThing::HardTransform (const csReversibleTransform& t)
{
  csRef<csThingStatic> new_static_data = static_data->Clone ();
  static_data = new_static_data;
  static_data->HardTransform (t);
  scfiPolygonMesh.SetThing (static_data);
  scfiPolygonMeshLOD.SetThing (static_data);
}

void csThing::Prepare ()
{
  static_data->Prepare ();

  if (prepared)
  {
    if (static_data_nr != static_data->scfiObjectModel.GetShapeNumber ())
    {
      static_data_nr = static_data->scfiObjectModel.GetShapeNumber ();

      if (cfg_moving == CS_THING_MOVE_OCCASIONAL)
      {
        if (wor_verts != static_data->obj_verts)
          delete[] wor_verts;
	wor_verts = new csVector3[static_data->max_vertices];
      }
      else
      {
        wor_verts = static_data->obj_verts;
      }
      if (cached_movable) movablenr = cached_movable->GetUpdateNumber ()-1;
      else movablenr--;

      int i;
      csPolygon3D *p;
      csPolygon3DStatic *ps;
      polygons.DeleteAll ();
      for (i = 0 ; i < static_data->static_polygons.Length () ; i++)
      {
	p = static_data->thing_type->blk_polygon3d.Alloc ();
	ps = static_data->static_polygons.Get (i);
	p->SetStaticData (ps);
	p->SetParent (this);
	polygons.Push (p);
	p->SetMaterial (FindRealMaterial (ps->GetMaterialWrapper ()));
	p->Finish ();
      }
    }
    return;
  }

  prepared = true;

  static_data_nr = static_data->scfiObjectModel.GetShapeNumber ();

  if (cfg_moving == CS_THING_MOVE_OCCASIONAL)
  {
    if (wor_verts != static_data->obj_verts)
      delete[] wor_verts;
    wor_verts = new csVector3[static_data->max_vertices];
  }
  else
  {
    wor_verts = static_data->obj_verts;
  }
  if (cached_movable) movablenr = cached_movable->GetUpdateNumber ()-1;
  else movablenr--;

#ifndef CS_USE_NEW_RENDERER
  if (polybuf)
  {
    polybuf->DecRef ();
    polybuf = NULL;
  }
#endif // CS_USE_NEW_RENDERER

  delete[] polybuf_materials;
  polybuf_materials = NULL;

  int i;
  csPolygon3DStatic *ps;
  csPolygon3D *p;
  polygons.DeleteAll ();
  for (i = 0; i < static_data->static_polygons.Length (); i++)
  {
    p = static_data->thing_type->blk_polygon3d.Alloc ();
    ps = static_data->static_polygons.Get (i);
    p->SetStaticData (ps);
    p->SetParent (this);
    polygons.Push (p);
    p->SetMaterial (FindRealMaterial (ps->GetMaterialWrapper ()));
    p->Finish ();
  }
}

iMaterialWrapper* csThing::FindRealMaterial (iMaterialWrapper* old_mat)
{
  int i;
  for (i = 0 ; i < replace_materials.Length () ; i++)
  {
    if (replace_materials[i].old_mat == old_mat)
      return replace_materials[i].new_mat;
  }
  return NULL;
}

void csThing::ReplaceMaterial (iMaterialWrapper* oldmat,
	iMaterialWrapper* newmat)
{
  replace_materials.Push (RepMaterial (oldmat, newmat));
  prepared = false;
}

void csThing::ClearReplacedMaterials ()
{
  replace_materials.DeleteAll ();
  prepared = false;
}

csPolygon3D *csThing::GetPolygon3D (const char *name)
{
  int idx = polygons.FindKey (name);
  return idx >= 0 ? polygons.Get (idx) : NULL;
}

int csThing::FindPolygonIndex (iPolygon3D *polygon) const
{
  csPolygon3D *p = polygon->GetPrivateObject ();
  return polygons.Find (p);
}

void csThing::InvalidateThing ()
{
#ifndef CS_USE_NEW_RENDERER
  if (polybuf)
  {
    polybuf->DecRef ();
    polybuf = NULL;
  }
#endif // CS_USE_NEW_RENDERER

  delete[] polybuf_materials;
  polybuf_materials = NULL;
  prepared = false;
  static_data->obj_bbox_valid = false;

  delete [] static_data->obj_normals; static_data->obj_normals = NULL;
  static_data->scfiObjectModel.ShapeChanged ();
}

iPolygonMesh* csThing::GetWriteObject ()
{
  return &scfiPolygonMeshLOD;
}

bool csThing::HitBeamOutline (const csVector3& start,
  const csVector3& end, csVector3& isect, float* pr)
{
  int i;
  float r;

  // @@@ This routine is not very optimal. Especially for things
  // with large number of polygons.
  for (i = 0; i < static_data->static_polygons.Length (); i++)
  {
    csPolygon3DStatic *p = static_data->static_polygons.Get (i);
    if (p->IntersectSegment (start, end, isect, &r))
    {
      if (pr) *pr = r;
      return true;
    }
  }

  return false;
}

bool csThing::HitBeamObject (const csVector3& start,
  const csVector3& end, csVector3& isect, float *pr)
{
  int idx = static_data->IntersectSegmentIndex (start, end, isect, pr, false);
  if (idx == -1) return false;
  return true;
}

#ifndef CS_USE_NEW_RENDERER
void csThing::DrawOnePolygon (
  csPolygon3D *p,
  csPolygon2D *poly,
  const csReversibleTransform& t,
  iRenderView *d,
  csZBufMode zMode,
  const csPlane3& camera_plane)
{
  iCamera *icam = d->GetCamera ();

  if (d->AddedFogInfo ())
  {
    // If fog info was added then we are dealing with vertex fog and
    // the current sector has fog. This means we have to complete the
    // fog_info structure with the plane of the current polygon.
    d->GetFirstFogInfo ()->outgoing_plane = camera_plane;
  }

  csPolygon3DStatic* sp = p->GetStaticData ();
  csPortal *po = sp->GetPortal ();
  //@@@if (csSector::do_portals && po)
  if (po)
  {
    bool filtered = false;

    // is_this_fog is true if this sector is fogged.
    bool is_this_fog = d->GetThisSector ()->HasFog ();

    // If there is filtering (alpha mapping or something like that) we need
    // to keep the texture plane so that it can be drawn after the sector has
    // been drawn. The texture plane needs to be kept because this polygon
    // may be rendered again (through mirrors) possibly overwriting the plane.
    csPlane3 keep_plane;
    if (d->GetGraphics3D ()->GetRenderState (
          G3DRENDERSTATE_TRANSPARENCYENABLE))
      filtered = sp->IsTransparent ();
    if (filtered || is_this_fog || (po && po->flags.Check (CS_PORTAL_ZFILL)))
    {
      keep_plane = camera_plane;
    }

    // First call OpenPortal() if needed.
    bool use_float_portal = po->flags.Check (CS_PORTAL_FLOAT);
    if (use_float_portal)
    {
      static G3DPolygonDFP g3dpoly;
      g3dpoly.num = poly->GetVertexCount ();
      memcpy (g3dpoly.vertices, poly->GetVertices (),
      	g3dpoly.num * sizeof (csVector2));
      g3dpoly.normal = camera_plane;
      d->GetGraphics3D ()->OpenPortal (&g3dpoly);
    }

    // Draw through the portal. If this fails we draw the original polygon
    // instead. Drawing through a portal can fail because we have reached
    // the maximum number that a sector is drawn (for mirrors).
    if (po->Draw (poly, &(p->scfiPolygon3D), t, d, keep_plane))
    {
      if (filtered)
      {
        //csZBufMode new_mode = zMode;
        //switch (zMode)
	//{
	  //case CS_ZBUF_NONE: new_mode = CS_ZBUF_NONE; break;
	  //case CS_ZBUF_TEST: new_mode = CS_ZBUF_TEST; break;
	  //case CS_ZBUF_FILL: new_mode = CS_ZBUF_FILL; break;
	  //case CS_ZBUF_USE: new_mode = CS_ZBUF_FILL; break;
	//}
        poly->DrawFilled (d, p, keep_plane, zMode);
      }
      if (is_this_fog)
      {
        poly->AddFogPolygon (
            d->GetGraphics3D (),
            p,
            keep_plane,
            icam->IsMirrored (),
            d->GetThisSector ()->QueryObject ()->GetID (),
            CS_FOG_BACK);
      }

      // Here we z-fill the portal contents to make sure that sprites
      // that are drawn outside of this portal cannot accidently cross
      // into the others sector space (we cannot trust the Z-buffer here).
      if (po->flags.Check (CS_PORTAL_ZFILL))
        poly->FillZBuf (d, p, keep_plane);
    }
    else
    {
      poly->DrawFilled (d, p, camera_plane, zMode);
    }

    // Make sure to close the portal again.
    if (use_float_portal)
      d->GetGraphics3D ()->ClosePortal ();
  }
  else
  {
    poly->DrawFilled (d, p, camera_plane, zMode);
  }
}

void csThing::DrawPolygonArray (
  csPolygon3D **polygon,
  int num,
  const csReversibleTransform& t,
  iRenderView *d,
  csZBufMode zMode)
{
  csPolygon3D *p;
  csVector3 *verts;
  int num_verts;
  int i;
  //@@@@ TOO MANY POINTERS:
  csPoly2DPool *render_pool = static_data->thing_type->render_pol2d_pool;
  csPolygon2D *clip;
  iCamera *icam = d->GetCamera ();
  const csReversibleTransform &camtrans = icam->GetTransform ();

  // Setup clip and far plane.
  csPlane3 clip_plane, *pclip_plane;
  bool do_clip_plane = d->GetClipPlane (clip_plane);
  if (do_clip_plane)
    pclip_plane = &clip_plane;
  else
    pclip_plane = NULL;

  csPlane3 *plclip = icam->GetFarPlane ();
  bool mirrored = icam->IsMirrored ();
  int fov = icam->GetFOV ();
  float shift_x = icam->GetShiftX ();
  float shift_y = icam->GetShiftY ();

  for (i = 0; i < num; i++)
  {
    p = polygon[i];
    p->UpdateTransformation (camtrans, icam->GetCameraNumber ());
    if (p->ClipToPlane (pclip_plane, camtrans.GetOrigin (), verts, num_verts))  //@@@Pool for verts?
    {
      // The far plane is defined negative. So if the polygon is entirely
      // in front of the far plane it is not visible. Otherwise we will render
      // it.
      if (
        !plclip ||
        csPoly3D::Classify (*plclip, verts, num_verts) != CS_POL_FRONT)
      {
        clip = (csPolygon2D *) (render_pool->Alloc ());
	csPlane3 plane_cam;
        p->WorldToCameraPlane (camtrans, verts[0], plane_cam);
        if (
          p->DoPerspective (
              verts, num_verts,
              clip, mirrored, fov, shift_x, shift_y,
	      plane_cam) &&
          clip->ClipAgainst (d->GetClipper ()))
        {
          DrawOnePolygon (p, clip, t, d, zMode, plane_cam);
        }

        render_pool->Free (clip);
      }
    }
  }
}

struct MatPol
{
  iMaterialWrapper *mat;
  int mat_index;
  csPolygon3DStatic *spoly;
  csPolygon3D *poly;
};

static int compare_material (const void *p1, const void *p2)
{
  MatPol *sp1 = (MatPol *)p1;
  MatPol *sp2 = (MatPol *)p2;
  if (sp1->mat < sp2->mat)
    return -1;
  else if (sp1->mat > sp2->mat)
    return 1;
  return 0;
}
#endif // CS_USE_NEW_RENDERER

void csThing::PreparePolygonBuffer ()
{
#ifndef CS_USE_NEW_RENDERER
  if (polybuf) return ;

  iVertexBufferManager *vbufmgr = static_data->thing_type->G3D->
    GetVertexBufferManager ();
  polybuf = vbufmgr->CreatePolygonBuffer ();
  polybuf->SetVertexArray (static_data->obj_verts, static_data->num_vertices);

  int i;

  //-----
  // First collect all material wrappers and polygons.
  //-----
  MatPol *matpol = new MatPol[static_data->static_polygons.Length ()];
  for (i = 0; i < static_data->static_polygons.Length (); i++)
  {
    matpol[i].spoly = static_data->GetPolygon3DStatic (i);
    matpol[i].poly = GetPolygon3D (i);
    matpol[i].mat = matpol[i].poly->GetRealMaterial ();
  }

  //-----
  // Sort on material.
  //-----
  qsort (matpol, polygons.Length (), sizeof (MatPol), compare_material);

  //-----
  // Now count all different materials we have and add them to the polygon
  // buffer. Also update the matpol structure with the index in the
  // material table.
  //-----
  polybuf->AddMaterial (matpol[0].mat->GetMaterialHandle ());
  matpol[0].mat_index = 0;
  polybuf_material_count = 1;
  for (i = 1; i < polygons.Length (); i++)
  {
    if (matpol[i].mat != matpol[i - 1].mat)
    {
      polybuf->AddMaterial (matpol[i].mat->GetMaterialHandle ());
      matpol[i].mat_index = polybuf_material_count;
      polybuf_material_count++;
    }
    else
    {
      matpol[i].mat_index = matpol[i - 1].mat_index;
    }
  }

  //-----
  // Update our local material wrapper table.
  //-----
  polybuf_materials = new iMaterialWrapper *[polybuf_material_count];
  polybuf_materials[0] = matpol[0].mat;
  polybuf_material_count = 1;
  for (i = 1; i < polygons.Length (); i++)
  {
    if (matpol[i].mat != matpol[i - 1].mat)
    {
      polybuf_materials[polybuf_material_count] = matpol[i].mat;
      polybuf_material_count++;
    }
  }

  //-----
  // Now add the polygons to the polygon buffer sorted by material.
  //-----
  for (i = 0; i < static_data->static_polygons.Length (); i++)
  {
    csPolygon3DStatic *spoly = matpol[i].spoly;
    csLightMapMapping *mapping = spoly->GetLightMapMapping ();
    csPolygon3D *poly = matpol[i].poly;
    csPolyTexture* lmi = poly->GetPolyTexture ();

    // @@@ what if lmi == NULL?
    //CS_ASSERT (lmi != NULL);
    if (mapping)
    {
      polybuf->AddPolygon (
          spoly->GetVertexIndices (),
          spoly->GetVertexCount (),
          spoly->GetObjectPlane (),
          matpol[i].mat_index,
          mapping->m_obj2tex,
          mapping->v_obj2tex,
          lmi);
    }
    else
    {
      csMatrix3 m_obj2tex;	// @@@
      csVector3 v_obj2tex;
      polybuf->AddPolygon (
          spoly->GetVertexIndices (),
          spoly->GetVertexCount (),
          spoly->GetObjectPlane (),
          matpol[i].mat_index,
          m_obj2tex,
          v_obj2tex,
          NULL);
    }
  }

  delete[] matpol;
  polybuf->Prepare ();
#endif // CS_USE_NEW_RENDERER
}

#ifndef CS_USE_NEW_RENDERER
void csThing::DrawPolygonArrayDPM (
  csPolygon3D ** /*polygon*/,
  int /*num*/,
  iRenderView *rview,
  iMovable *movable,
  csZBufMode zMode)
{
  PreparePolygonBuffer ();

  iCamera *icam = rview->GetCamera ();
  csReversibleTransform tr_o2c = icam->GetTransform ();
  if (!movable->IsFullTransformIdentity ())
    tr_o2c /= movable->GetFullTransform ();

  G3DPolygonMesh mesh;
  csVector3 radius;
  csSphere sphere;
  static_data->GetRadius (radius, sphere.GetCenter ());
  float max_radius = radius.x;
  if (max_radius < radius.y) max_radius = radius.y;
  if (max_radius < radius.z) max_radius = radius.z;
  sphere.SetRadius (max_radius);
  if (rview->ClipBSphere (tr_o2c, sphere, mesh.clip_portal, mesh.clip_plane,
  	mesh.clip_z_plane) == false)
    return;	// Not visible.

  rview->GetGraphics3D ()->SetObjectToCamera (&tr_o2c);
  rview->GetGraphics3D ()->SetRenderState (G3DRENDERSTATE_ZBUFFERMODE, zMode);

  mesh.polybuf = polybuf;
  int i;
  for (i = 0; i < polybuf_material_count; i++)
  {
    polybuf_materials[i]->Visit ();
    polybuf->SetMaterial (i, polybuf_materials[i]->GetMaterialHandle ());
  }

  mesh.do_fog = false;
  mesh.do_mirror = icam->IsMirrored ();
  mesh.vertex_mode = G3DPolygonMesh::VM_WORLDSPACE;
  mesh.vertex_fog = NULL;
  mesh.mixmode = CS_FX_COPY;

  rview->CalculateFogMesh(tr_o2c,mesh);
  rview->GetGraphics3D ()->DrawPolygonMesh (mesh);
}
#endif // CS_USE_NEW_RENDERER

// @@@ We need a better algorithm here. We should try
// to recognize convex sub-parts of a polygonset and return
// convex shadow frustums for those. This will significantly
// reduce the number of shadow frustums. There are basicly
// two ways to do this:
//	- Split object into convex sub-parts in 3D.
//	- Split object into convex sub-parts in 2D.
// The first way is probably best because it is more efficient
// at runtime (important if we plan to use dynamic shadows for things)
// and also more correct in that a convex 3D object has no internal
// shadowing while a convex outline may have no correspondance to internal
// shadows.
void csThing::AppendShadows (
  iMovable* movable,
  iShadowBlockList *shadows,
  const csVector3 &origin)
{
  Prepare ();
  //@@@ Ok?
  cached_movable = movable;
  WorUpdate ();

  iShadowBlock *list = shadows->NewShadowBlock (
      polygons.Length ());
  csFrustum *frust;
  int i, j;
  csPolygon3DStatic *sp;
  csPolygon3D *p;
  bool cw = true;                   //@@@ Use mirroring parameter here!
  for (i = 0; i < static_data->static_polygons.Length (); i++)
  {
    sp = static_data->static_polygons.Get (i);
    if (sp->GetPortal ()) continue;  // No portals
    p = polygons.Get (i);

    //if (p->GetPlane ()->VisibleFromPoint (origin) != cw) continue;
    float clas = p->GetWorldPlane ().Classify (origin);
    if (ABS (clas) < EPSILON) continue;
    if ((clas <= 0) != cw) continue;

    csPlane3 pl = p->GetWorldPlane ();
    pl.DD += origin * pl.norm;
    pl.Invert ();
    frust = list->AddShadow (
        origin,
        (void *)p,
        sp->GetVertices ().GetVertexCount (),
        pl);
    for (j = 0; j < sp->GetVertices ().GetVertexCount (); j++)
      frust->GetVertex (j).Set (p->Vwor (j) - origin);
  }
}

void csThing::GetBoundingBox (iMovable *movable, csBox3 &box)
{
  if (wor_bbox_movablenr != movable->GetUpdateNumber ())
  {
    // First make sure obj_bbox is valid.
    static_data->GetBoundingBox (box);
    wor_bbox_movablenr = movable->GetUpdateNumber ();
    csBox3& obj_bbox = static_data->obj_bbox;

    // @@@ Maybe it would be better to really calculate the bounding box
    // here instead of just transforming the object space bounding box?
    if (movable->IsFullTransformIdentity ())
    {
      wor_bbox = obj_bbox;
    }
    else
    {
      csReversibleTransform mt = movable->GetFullTransform ();
      wor_bbox.StartBoundingBox (mt.This2Other (obj_bbox.GetCorner (0)));
      wor_bbox.AddBoundingVertexSmart (mt.This2Other (obj_bbox.GetCorner (1)));
      wor_bbox.AddBoundingVertexSmart (mt.This2Other (obj_bbox.GetCorner (2)));
      wor_bbox.AddBoundingVertexSmart (mt.This2Other (obj_bbox.GetCorner (3)));
      wor_bbox.AddBoundingVertexSmart (mt.This2Other (obj_bbox.GetCorner (4)));
      wor_bbox.AddBoundingVertexSmart (mt.This2Other (obj_bbox.GetCorner (5)));
      wor_bbox.AddBoundingVertexSmart (mt.This2Other (obj_bbox.GetCorner (6)));
      wor_bbox.AddBoundingVertexSmart (mt.This2Other (obj_bbox.GetCorner (7)));
    }
  }

  box = wor_bbox;
}

//-------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csThing::PolyMeshLOD)
  SCF_IMPLEMENTS_INTERFACE(iPolygonMesh)
SCF_IMPLEMENT_IBASE_END

csThing::PolyMeshLOD::PolyMeshLOD () : PolyMeshHelper (CS_POLY_VISCULL)
{
  SCF_CONSTRUCT_IBASE (NULL);
}

//-------------------------------------------------------------------------

void PolyMeshHelper::SetThing (csThingStatic* thing)
{
  PolyMeshHelper::thing = thing;
  static_data_nr = thing->GetStaticDataNumber ()-1;
}

void PolyMeshHelper::Setup ()
{
  thing->Prepare ();
  if (static_data_nr != thing->GetStaticDataNumber ())
  {
    static_data_nr = thing->GetStaticDataNumber ();
    Cleanup ();
  }

  if (polygons)
  {
    // Already set up. First we check if the object vertex array
    // is still valid.
    if (vertices == thing->obj_verts) return ;
  }

  vertices = NULL;

  // Count the number of needed polygons and vertices.
  num_verts = thing->GetVertexCount ();
  num_poly = 0;

  int i;
  const csPolygonStaticArray &pol = thing->static_polygons;
  for (i = 0 ; i < pol.Length () ; i++)
  {
    csPolygon3DStatic *p = pol.Get (i);
    if (p->flags.Check (poly_flag)) num_poly++;
  }

  // Allocate the arrays and the copy the data.
  if (num_verts)
  {
    vertices = thing->obj_verts;
  }

  if (num_poly)
  {
    polygons = new csMeshedPolygon[num_poly];
    num_poly = 0;
    for (i = 0 ; i < pol.Length () ; i++)
    {
      csPolygon3DStatic *p = pol.Get (i);
      if (p->flags.Check (poly_flag))
      {
        polygons[num_poly].num_vertices = p->GetVertexCount ();
        polygons[num_poly].vertices = p->GetVertexIndices ();
        num_poly++;
      }
    }
  }
}

void PolyMeshHelper::Cleanup ()
{
  delete[] polygons;
  polygons = NULL;
  vertices = NULL;
}

//-------------------------------------------------------------------------

bool csThing::DrawTest (iRenderView *rview, iMovable *movable)
{
  Prepare ();

  iCamera *icam = rview->GetCamera ();
  const csReversibleTransform &camtrans = icam->GetTransform ();

  // Only get the transformation if this thing can move.
  bool can_move = false;
  if (movable && cfg_moving != CS_THING_MOVE_NEVER)
  {
    can_move = true;
  }

  //@@@ Ok?
  cached_movable = movable;
  WorUpdate ();

  csBox3 b;
  static_data->GetBoundingBox (b);

  csSphere sphere;
  sphere.SetCenter (b.GetCenter ());
  sphere.SetRadius (static_data->max_obj_radius);
  if (can_move)
  {
    csReversibleTransform tr_o2c = camtrans;
    if (!movable->IsFullTransformIdentity ())
      tr_o2c /= movable->GetFullTransform ();
    bool rc = rview->TestBSphere (tr_o2c, sphere);
    return rc;
  }
  else
  {
    bool rc = rview->TestBSphere (camtrans, sphere);
    return rc;
  }
}

bool csThing::Draw (iRenderView *rview, iMovable *movable, csZBufMode zMode)
{
#ifndef CS_USE_NEW_RENDERER
  iCamera *icam = rview->GetCamera ();
  const csReversibleTransform &camtrans = icam->GetTransform ();

  draw_busy++;

  if (static_data->flags.Check (CS_THING_FASTMESH))
    DrawPolygonArrayDPM (
      polygons.GetArray (),
      polygons.Length (),
      rview,
      movable,
      zMode);
  else
  {
    UpdateTransformation (camtrans, icam->GetCameraNumber ());
    DrawPolygonArray (
      polygons.GetArray (),
      polygons.Length (),
      movable->GetTransform (),
      rview,
      zMode);
  }

  draw_busy--;
#endif // CS_USE_NEW_RENDERER
  return true;                                  // @@@@ RETURN correct vis info
}

//----------------------------------------------------------------------

void csThing::CastShadows (iFrustumView *lview, iMovable *movable)
{
  Prepare ();
  //@@@ Ok?
  cached_movable = movable;
  WorUpdate ();

  int i;

  draw_busy++;

  iFrustumViewUserdata* fvud = lview->GetUserdata ();
  iLightingProcessInfo* lpi = (iLightingProcessInfo*)fvud;
  bool dyn = lpi->IsDynamic ();

  csRef<csLightingPolyTexQueue> lptq;
  if (!dyn)
  {
    csRef<iLightingProcessData> lpd = lpi->QueryUserdata (
    	csLightingPolyTexQueue_scfGetID (),
	csLightingPolyTexQueue_VERSION);
    lptq = (csLightingPolyTexQueue*)(iLightingProcessData*)lpd;
    if (!lptq)
    {
      lptq = csPtr<csLightingPolyTexQueue> (new csLightingPolyTexQueue (
    	  lpi->GetLight ()));
      lpi->AttachUserdata (lptq);
    }
    csRef<iStatLight> sl = SCF_QUERY_INTERFACE (lpi->GetLight (),
    	iStatLight);
    sl->AddAffectedLightingInfo (&scfiLightingInfo);
  }
  else
  {
    csRef<iDynLight> dl = SCF_QUERY_INTERFACE (lpi->GetLight (),
    	iDynLight);
    dl->AddAffectedLightingInfo (&scfiLightingInfo);
  }

  for (i = 0; i < polygons.Length (); i++)
  {
    csPolygon3D* poly = GetPolygon3D (i);
    if (dyn)
      poly->CalculateLightingDynamic (lview, movable);
    else
      poly->CalculateLightingStatic (lview, movable, lptq, true);
  }

  draw_busy--;
}

void csThing::InitializeDefault ()
{
  Prepare ();

  int i;
  for (i = 0; i < polygons.Length (); i++)
    polygons.Get (i)->InitializeDefault ();
}

bool csThing::ReadFromCache (iCacheManager* cache_mgr)
{
  Prepare ();
  char* cachename = GenerateCacheName ();
  cache_mgr->SetCurrentScope (cachename);
  delete[] cachename;

  // For error reporting.
  const char* thing_name = NULL;
  if (static_data->thing_type->do_verbose && logparent)
  {
    csRef<iMeshWrapper> mw (SCF_QUERY_INTERFACE (logparent, iMeshWrapper));
    if (mw) thing_name = mw->QueryObject ()->GetName ();
  }

  bool rc = true;
  csRef<iDataBuffer> db = cache_mgr->ReadCache ("thing_lm", NULL, (uint32) ~0);
  if (db)
  {
    csMemFile mf ((const char*)(db->GetData ()), db->GetSize ());
    int i;
    for (i = 0; i < polygons.Length (); i++)
    {
      const char* error = polygons.Get (i)->ReadFromCache (&mf);
      if (error != NULL)
      {
        rc = false;
        if (static_data->thing_type->do_verbose)
	{
	  printf ("  Thing '%s' Poly '%s': %s\n",
	  	thing_name, static_data->static_polygons.Get (i)->GetName (),
		error);
	  fflush (stdout);
        }
      }
    }
  }
  else
  {
    if (static_data->thing_type->do_verbose)
    {
      printf ("  Thing '%s': Could not find cached lightmap file for thing!\n",
      	thing_name);
      fflush (stdout);
    }
    rc = false;
  }

  cache_mgr->SetCurrentScope (NULL);
  return rc;
}

bool csThing::WriteToCache (iCacheManager* cache_mgr)
{
  char* cachename = GenerateCacheName ();
  cache_mgr->SetCurrentScope (cachename);
  delete[] cachename;

  int i;
  bool rc = false;
  csMemFile mf;
  for (i = 0; i < polygons.Length (); i++)
    if (!polygons.Get (i)->WriteToCache (&mf)) goto stop;
  if (!cache_mgr->CacheData ((void*)(mf.GetData ()), mf.GetSize (),
    	"thing_lm", NULL, (uint32) ~0))
    goto stop;

  rc = true;

stop:
  cache_mgr->SetCurrentScope (NULL);
  return rc;
}

void csThing::PrepareLighting ()
{
  int i;
  for (i = 0; i < polygons.Length (); i++)
    polygons.Get (i)->PrepareLighting ();
}

void csThing::Merge (csThing *other)
{
  int i, j;
  int *merge_vertices = new int[other->static_data->GetVertexCount () + 1];
  for (i = 0; i < other->static_data->GetVertexCount (); i++)
    merge_vertices[i] = static_data->AddVertex (other->Vwor (i));

  for (i = 0; i < other->polygons.Length (); i++)
  {
    //csPolygon3D *p = other->GetPolygon3D (i);
    csPolygon3DStatic *sp = other->static_data->GetPolygon3DStatic (i);
    int *idx = sp->GetVertices ().GetVertexIndices ();
    for (j = 0; j < sp->GetVertices ().GetVertexCount (); j++)
      idx[j] = merge_vertices[idx[j]];
    static_data->AddPolygon (sp);
    other->polygons[i] = NULL;
  }

  delete[] merge_vertices;
}

//---------------------------------------------------------------------------

iPolygon3D *csThing::ThingState::GetPolygon (int idx)
{
  csPolygon3D *p = scfParent->GetPolygon3D (idx);
  if (!p) return NULL;
  return &(p->scfiPolygon3D);
}

iPolygon3D *csThing::ThingState::GetPolygon (const char *name)
{
  csPolygon3D *p = scfParent->GetPolygon3D (name);
  if (!p) return NULL;
  return &(p->scfiPolygon3D);
}

iPolygon3D *csThing::ThingState::GetPortalPolygon (int idx) const
{
  csPolygon3D *p = (scfParent->polygons.Get (
		  scfParent->static_data->portal_polygons[idx]));
  return &(p->scfiPolygon3D);
}

iPolygon3D* csThing::ThingState::IntersectSegment (
	const csVector3& start,
	const csVector3& end, csVector3& isect,
	float* pr, bool only_portals)
{
  int p = scfParent->static_data->IntersectSegmentIndex (start, end, isect, pr,
  	only_portals);
  return p != -1 ? &(scfParent->polygons.Get (p)->scfiPolygon3D) : NULL;
}

//---------------------------------------------------------------------------
iMeshObjectFactory *csThing::MeshObject::GetFactory () const
{
  if (!scfParent->ParentTemplate) return NULL;
  return (iMeshObjectFactory*)(scfParent->ParentTemplate->GetStaticData ());
}

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csThingObjectType)
  SCF_IMPLEMENTS_INTERFACE(iMeshObjectType)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iComponent)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iThingEnvironment)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iConfig)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csThingObjectType::eiComponent)
  SCF_IMPLEMENTS_INTERFACE(iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csThingObjectType::eiThingEnvironment)
  SCF_IMPLEMENTS_INTERFACE(iThingEnvironment)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csThingObjectType::eiConfig)
  SCF_IMPLEMENTS_INTERFACE(iConfig)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csThingObjectType)

SCF_EXPORT_CLASS_TABLE (thing)
  SCF_EXPORT_CLASS (csThingObjectType,
  	"crystalspace.mesh.object.thing",
	"Crystal Space Thing Mesh Type")
SCF_EXPORT_CLASS_TABLE_END

csThingObjectType::csThingObjectType (iBase *pParent) :
	blk_polygon3d (2000),
	blk_polygon3dstatic (2000),
	blk_lightmapmapping (2000),
	blk_polytex (2000),
	blk_lightmap (2000)
{
printf (">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"); fflush (stdout);
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiThingEnvironment);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiConfig);
  lightpatch_pool = NULL;
  render_pol2d_pool = NULL;
  do_verbose = false;
}

csThingObjectType::~csThingObjectType ()
{
  delete render_pol2d_pool;
  delete lightpatch_pool;
printf ("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n"); fflush (stdout);
}

bool csThingObjectType::Initialize (iObjectRegistry *object_reg)
{
  csThingObjectType::object_reg = object_reg;
  csRef<iEngine> e = CS_QUERY_REGISTRY (object_reg, iEngine);
  engine = e;	// We don't want a real ref here to avoid circular refs.
#ifndef CS_USE_NEW_RENDERER
  csRef<iGraphics3D> g = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
#else
  csRef<iGraphics3D> g = CS_QUERY_REGISTRY (object_reg, iRender3D);
#endif // CS_USE_NEW_RENDERER
  G3D = g;

  lightpatch_pool = new csLightPatchPool ();
  render_pol2d_pool = new csPoly2DPool (csPolygon2DFactory::SharedFactory ());

  csRef<iCommandLineParser> cmdline = CS_QUERY_REGISTRY (object_reg,
  	iCommandLineParser);
  if (cmdline)
  {
    do_verbose = cmdline->GetOption ("verbose") != NULL;
  }

  return true;
}

void csThingObjectType::Clear ()
{
  delete lightpatch_pool;
  delete render_pol2d_pool;
  lightpatch_pool = new csLightPatchPool ();
  render_pol2d_pool = new csPoly2DPool (csPolygon2DFactory::SharedFactory ());
}

csPtr<iMeshObjectFactory> csThingObjectType::NewFactory ()
{
  csThingStatic *cm = new csThingStatic (this, this);
  csRef<iMeshObjectFactory> ifact (SCF_QUERY_INTERFACE (
      cm, iMeshObjectFactory));
  cm->DecRef ();
  return csPtr<iMeshObjectFactory> (ifact);
}

void csThingObjectType::Warn (const char *description, ...)
{
  va_list arg;
  va_start (arg, description);

  csRef<iReporter> Reporter = CS_QUERY_REGISTRY (object_reg, iReporter);

  if (Reporter)
  {
    Reporter->ReportV (
        CS_REPORTER_SEVERITY_WARNING,
        "crystalspace.engine.warning",
        description,
        arg);
  }
  else
  {
    csPrintfV (description, arg);
    csPrintf ("\n");
  }

  va_end (arg);
}

void csThingObjectType::Bug (const char *description, ...)
{
  va_list arg;
  va_start (arg, description);

  csRef<iReporter> Reporter = CS_QUERY_REGISTRY (object_reg, iReporter);

  if (Reporter)
  {
    Reporter->ReportV (
        CS_REPORTER_SEVERITY_BUG,
        "crystalspace.engine.warning",
        description,
        arg);
  }
  else
  {
    csPrintfV (description, arg);
    csPrintf ("\n");
  }

  va_end (arg);
}

void csThingObjectType::Error (const char *description, ...)
{
  va_list arg;
  va_start (arg, description);

  csRef<iReporter> Reporter = CS_QUERY_REGISTRY (object_reg, iReporter);

  if (Reporter)
  {
    Reporter->ReportV (
        CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.engine.warning",
        description,
        arg);
  }
  else
  {
    csPrintfV (description, arg);
    csPrintf ("\n");
  }

  va_end (arg);
}

void csThingObjectType::Notify (const char *description, ...)
{
  va_list arg;
  va_start (arg, description);

  csRef<iReporter> Reporter = CS_QUERY_REGISTRY (object_reg, iReporter);

  if (Reporter)
  {
    Reporter->ReportV (
        CS_REPORTER_SEVERITY_NOTIFY,
        "crystalspace.engine.warning",
        description,
        arg);
  }
  else
  {
    csPrintfV (description, arg);
    csPrintf ("\n");
  }

  va_end (arg);
}

//---------------------------------------------------------------------------

static const csOptionDescription
  config_options[] =
{
  { 0, "cosfact", "Cosinus factor for lighting", CSVAR_FLOAT }
};
const int NUM_OPTIONS =
  (
    sizeof (config_options) /
    sizeof (config_options[0])
  );

bool csThingObjectType::eiConfig::SetOption (int id, csVariant *value)
{
  switch (id)
  {
    case 0:
      csPolyTexture::cfg_cosinus_factor = value->GetFloat ();
      break;
    default:
      return false;
  }

  return true;
}

bool csThingObjectType::eiConfig::GetOption (int id, csVariant *value)
{
  switch (id)
  {
    case 0:   value->SetFloat (csPolyTexture::cfg_cosinus_factor); break;
    default:  return false;
  }

  return true;
}

bool csThingObjectType::eiConfig::GetOptionDescription (
  int idx,
  csOptionDescription *option)
{
  if (idx < 0 || idx >= NUM_OPTIONS) return false;
  *option = config_options[idx];
  return true;
}

//---------------------------------------------------------------------------

