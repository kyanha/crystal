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
#include "csengine/thing.h"
#include "csengine/polygon.h"
#include "csengine/polytmap.h"
#include "csengine/pol2d.h"
#include "csengine/polytext.h"
#include "csengine/light.h"
#include "csengine/engine.h"
#include "csengine/stats.h"
#include "csengine/sector.h"
#include "csengine/cbufcube.h"
#include "csengine/bspbbox.h"
#include "csengine/curve.h"
#include "csengine/texture.h"
#include "csengine/material.h"
#include "csengine/meshobj.h"
#include "csgeom/sphere.h"
#include "csgeom/math3d.h"
#include "csutil/csstring.h"
#include "csutil/memfile.h"
#include "csutil/hashmap.h"
#include "csutil/debug.h"
#include "csutil/csmd5.h"
#include "ivideo/txtmgr.h"
#include "ivideo/vbufmgr.h"
#include "ivideo/texture.h"
#include "iengine/texture.h"
#include "iengine/shadcast.h"
#include "csutil/hashmap.h"
#include "iutil/vfs.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "iutil/cache.h"
#include "iengine/rview.h"
#include "iengine/fview.h"
#include "qint.h"
#include "qsqrt.h"
#include "ivideo/graph3d.h"


long csThing::current_light_frame_number = 0;

//---------------------------------------------------------------------------
SCF_IMPLEMENT_IBASE_EXT(csThing)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iThingState)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iLightingInfo)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iObjectModel)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iPolygonMesh)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iVisibilityCuller)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iShadowCaster)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iShadowReceiver)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iMeshObject)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iMeshObjectFactory)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csThing::ThingState)
  SCF_IMPLEMENTS_INTERFACE(iThingState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csThing::LightingInfo)
  SCF_IMPLEMENTS_INTERFACE(iLightingInfo)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csThing::ObjectModel)
  SCF_IMPLEMENTS_INTERFACE(iObjectModel)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csThing::PolyMesh)
  SCF_IMPLEMENTS_INTERFACE(iPolygonMesh)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csThing::VisCull)
  SCF_IMPLEMENTS_INTERFACE(iVisibilityCuller)
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

SCF_IMPLEMENT_EMBEDDED_IBASE (csThing::MeshObjectFactory)
  SCF_IMPLEMENTS_INTERFACE(iMeshObjectFactory)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

int csThing:: last_thing_id = 0;

csThing::csThing (iBase *parent) :
  csObject(parent),
  polygons(64, 64),
  curves(16, 16)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiThingState);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiLightingInfo);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObjectModel);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiPolygonMesh);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiVisibilityCuller);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiShadowCaster);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiShadowReceiver);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiMeshObject);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiMeshObjectFactory);
  DG_TYPE (this, "csThing");

  scfiPolygonMesh.SetThing (this);
  scfiPolygonMeshLOD.SetThing (this);

  last_thing_id++;
  thing_id = last_thing_id;
  last_polygon_id = 0;
  logparent = NULL;

  curves_center.x = curves_center.y = curves_center.z = 0;
  curves_scale = 40;
  curve_vertices = NULL;
  curve_texels = NULL;
  num_curve_vertices = max_curve_vertices = 0;

  max_vertices = num_vertices = 0;
  wor_verts = NULL;
  obj_verts = NULL;
  cam_verts = NULL;
  num_cam_verts = 0;

  draw_busy = 0;
#ifndef CS_USE_NEW_RENDERER
  fog.enabled = false;
#endif // CS_USE_NEW_RENDERER
  bbox = NULL;
  obj_bbox_valid = false;
  light_frame_number = -1;

  dynamic_ambient.Set(0,0,0);
  ambient_version = 0;

  center_idx = -1;
  ParentTemplate = NULL;

  cameranr = -1;
  movablenr = -1;
  shapenr = -1;
  wor_bbox_movablenr = -1;
  cached_movable = NULL;

  cfg_moving = CS_THING_MOVE_NEVER;

  static_tree = NULL;
  prepared = false;

  current_lod = 1;
  current_features = 0;
  thing_edges_valid = false;

  curves_transf_ok = false;

#ifndef CS_USE_NEW_RENDERER
  polybuf = NULL;
#endif // CS_USE_NEW_RENDERER
  polybuf_materials = NULL;
  cachename = NULL;

  obj_normals = NULL;
  smoothed = false;
  current_visnr = 1;

  vistest_objects_inuse = false;
}

csThing::~csThing ()
{
#ifndef CS_USE_NEW_RENDERER
  if (polybuf) polybuf->DecRef ();
#endif // CS_USE_NEW_RENDERER
  delete[] polybuf_materials;

  if (wor_verts == obj_verts)
    delete[] obj_verts;
  else
  {
    delete[] wor_verts;
    delete[] obj_verts;
  }

  delete[] cam_verts;
  delete[] curve_vertices;
  delete[] curve_texels;
  delete bbox;
  delete static_tree;

  int i;
  for (i = 0; i < visobjects.Length (); i++)
  {
    csVisObjInfo *vinf = (csVisObjInfo *)visobjects[i];
    delete vinf->bbox;
    vinf->visobj->DecRef ();
    delete vinf;
  }

  polygons.DeleteAll ();          // delete prior to portal_poly array !
  if (portal_polygons.Length ()) portal_polygons.DeleteAll ();
  CleanupThingEdgeTable ();
  delete[] cachename;

  delete [] obj_normals;
}

void csThing::GenerateCacheName ()
{
  csBox3 b;
  GetBoundingBox (b);

  csMemFile mf;
  long l;
  l = convert_endian ((long)num_vertices);
  mf.Write ((char*)&l, 4);
  l = convert_endian ((long)polygons.Length ());
  mf.Write ((char*)&l, 4);
  l = convert_endian ((long)num_curve_vertices);
  mf.Write ((char*)&l, 4);
  l = convert_endian ((long)curves.Length ());
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

  l = convert_endian ((long)QInt ((b.MinX () * 1000)+.5));
  mf.Write ((char*)&l, 4);
  l = convert_endian ((long)QInt ((b.MinY () * 1000)+.5));
  mf.Write ((char*)&l, 4);
  l = convert_endian ((long)QInt ((b.MinZ () * 1000)+.5));
  mf.Write ((char*)&l, 4);
  l = convert_endian ((long)QInt ((b.MaxX () * 1000)+.5));
  mf.Write ((char*)&l, 4);
  l = convert_endian ((long)QInt ((b.MaxY () * 1000)+.5));
  mf.Write ((char*)&l, 4);
  l = convert_endian ((long)QInt ((b.MaxZ () * 1000)+.5));
  mf.Write ((char*)&l, 4);

  csMD5::Digest digest = csMD5::Encode (mf.GetData (), mf.GetSize ());

  delete[] cachename;
  cachename = new char[33];
  sprintf (cachename,
  	"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
  	digest.data[0], digest.data[1], digest.data[2], digest.data[3],
  	digest.data[4], digest.data[5], digest.data[6], digest.data[7],
  	digest.data[8], digest.data[9], digest.data[10], digest.data[11],
  	digest.data[12], digest.data[13], digest.data[14], digest.data[15]);
}

const char* csThing::GetCacheName ()
{
  if (!cachename)
  {
    GenerateCacheName ();
  }
  return cachename;
}

void csThing::MarkLightmapsDirty ()
{
#ifndef CS_USE_NEW_RENDERER
  if (polybuf)
    polybuf->MarkLightmapsDirty ();
#endif // CS_USE_NEW_RENDERER
}

void csThing::CleanupThingEdgeTable ()
{
  int i;
  for (i = 0; i < thing_edges.Length (); i++)
  {
    csThingEdge &te = thing_edges[i];
    delete[] te.polygon_indices;
  }

  thing_edges.SetLength (0);
  thing_edges_valid = false;
}

void csThing::ComputeThingEdgeTable ()
{
  if (thing_edges_valid) return ;
  CleanupThingEdgeTable ();

  //@@@
  thing_edges_valid = true;
}

void csThing::SetMovingOption (int opt)
{
  cfg_moving = opt;
  curves_transf_ok = false;
  switch (cfg_moving)
  {
    case CS_THING_MOVE_NEVER:
      if (wor_verts != obj_verts) delete[] wor_verts;
      wor_verts = obj_verts;
      break;

    case CS_THING_MOVE_OCCASIONAL:
      if ((wor_verts == NULL || wor_verts == obj_verts) && max_vertices)
      {
        wor_verts = new csVector3[max_vertices];
        memcpy (wor_verts, obj_verts, max_vertices * sizeof (csVector3));
      }

      cached_movable = NULL;
      break;

    case CS_THING_MOVE_OFTEN:
      if (wor_verts != obj_verts) delete[] wor_verts;
      wor_verts = obj_verts;
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
      UpdateCurveTransform ();
      return ;

    case CS_THING_MOVE_OCCASIONAL:
      if (cached_movable && cached_movable->GetUpdateNumber () != movablenr)
      {
        movablenr = cached_movable->GetUpdateNumber ();

        csReversibleTransform movtrans = cached_movable->GetFullTransform ();
        for (i = 0; i < num_vertices; i++)
          wor_verts[i] = movtrans.This2Other (obj_verts[i]);
        for (i = 0; i < polygons.Length (); i++)
        {
          csPolygon3D *p = GetPolygon3D (i);
          p->ObjectToWorld (movtrans, p->Vwor (0));
        }

        UpdateCurveTransform (movtrans);

        // If the movable changed we invalidate the camera number as well
        // to make sure the camera vertices are recalculated as well.
        cameranr--;
      }
      break;

    case CS_THING_MOVE_OFTEN:
      //@@@ Not implemented yet!
      return ;
  }
}

void csThing::UpdateTransformation (const csTransform &c, long cam_cameranr)
{
  if (!cam_verts || num_vertices != num_cam_verts)
  {
    delete[] cam_verts;
    cam_verts = new csVector3[num_vertices];
    num_cam_verts = num_vertices;
    cameranr = cam_cameranr - 1;  // To make sure we will transform.
  }

  if (cameranr != cam_cameranr)
  {
    cameranr = cam_cameranr;

    int i;
    for (i = 0; i < num_vertices; i++)
      cam_verts[i] = c.Other2This (wor_verts[i]);
  }
}


void csThing::SetSmoothingFlag(bool smooth)
{
  smoothed = smooth;
}

void csThing::CalculateNormals()
{
  int polyCount = polygons.Length();
  int i, j, k;
  int* vertIndices;

  delete[] obj_normals;
  obj_normals = new csVector3[num_vertices];
	csVector3** normals = new csVector3*[num_vertices];

  for(i = 0; i < num_vertices; i++)
  {
    normals[i] = new csVector3[polyCount];
    obj_normals[i].x = obj_normals[i].y = obj_normals[i].z = 0.0;
    for (j = 0; j< polyCount; j++)
      normals[i][j] = obj_normals[i];
  }


  // Get all the normals of a vertex affected by all the polygons
  for(j = 0; j < polyCount; j++)
  {
    csPolygon3D* p = polygons.Get(j);
    vertIndices = p->GetVertexIndices();
    csVector3 normal = p->GetPlane()->GetWorldPlane().Normal();

    // Add the normal to all the vertexs of the polygon
    int vertCount = p->GetVertexCount();
    for(k = 0; k < vertCount; k++)
    {
      normals[vertIndices[k]][j] = normal;
    }
  }

  // Remove the repeated normals
  float absolut;
  for (i = 0; i < num_vertices; i++)
  {
    for(j = 0; j < polyCount-1; j++)
    {
      csVector3 normalAct = normals[i][j];
      // Search in the rest of the normals
      absolut = fabs(normalAct.x) + fabs(normalAct.y) + fabs(normalAct.z);
      if ( absolut > 0.5  )
      {
	for (k=j+1; k< polyCount; k++)
	{
	  // If the normals are the same, remove the new one
	  csVector3 normalZero = normalAct - normals[i][k];
	  absolut = fabs(normalZero.x) + fabs(normalZero.y)
	  	+ fabs(normalZero.z);
	  if (absolut < 0.1)
	  {
	    normals[i][k].x = normals[i][k].y = normals[i][k].z = 0.0;
	  }
	}
      }
    }
  }


  for(i = 0; i < num_vertices; i++)
  {
    for(j = 0; j < polyCount; j++)
    {
      obj_normals[i] += normals[i][j];
    }
    obj_normals[i].Normalize();

    csVector3 v = Vwor(i);
    csVector3 normalPerfecta (-v.x, -v.y+20.0, -v.z);
    normalPerfecta.Normalize ();
  }

  for(i = 0; i < num_vertices; i++)
    delete [] normals[i];
  delete [] normals;
}

void csThing::Prepare ()
{
  if (prepared) return ;
  prepared = true;
  shapenr++;
  scfiPolygonMeshLOD.Cleanup ();
  scfiPolygonMesh.Cleanup ();
  if (!flags.Check (CS_THING_NOCOMPRESS))
  {
    CompressVertices ();
    RemoveUnusedVertices ();
  }

  if (smoothed)
    CalculateNormals();

  int i;
  csPolygon3D *p;
  for (i = 0; i < polygons.Length (); i++)
  {
    p = polygons.Get (i);
    p->Finish ();
  }
  FireListeners ();


  {
static int total_size = 0;
static int total_cnt = 0;
    int size = sizeof (csThing);
    size += cachename ? strlen (cachename)+3 : 0;
    int vtsize = sizeof (csVector3);
    size += vtsize * max_vertices;	// obj_verts;
    size += vtsize * num_cam_verts;	// cam_verts;
    if (obj_verts != wor_verts)
      size += vtsize * max_vertices;
    if (obj_normals)
      size += vtsize * num_vertices;
    size += sizeof (csThingBBox);
    size += 4 * polygons.Limit ();
    size += GetName () ? strlen (GetName ())+3 : 0;
    int polsize = sizeof (csPolygon3D) + sizeof (csPolyTexture)
    	+ sizeof (csPolyTexLightMap) + sizeof (csPolyTxtPlane)
	+ sizeof (csPolyPlane) + sizeof (csLightMap);
    polsize *= polygons.Length ();
    int lm_size = 0;
    int i;
    for (i = 0 ; i < polygons.Length () ; i++)
    {
      csPolygon3D *p = polygons.Get (i);
      polsize += 4*4;	// Average indexes.
      csPolyTexLightMap *lmi = p->GetLightMapInfo ();
      if (lmi)
      {
        csLightMap* lm = lmi->GetPolyTex ()->GetCSLightMap ();
	if (lm)
	  polsize += lm->GetSize () * sizeof (csRGBpixel) * 2;
      }
    }

    total_size += size+polsize;
    total_cnt++;
    printf ("thing size=%d polysize=%d total=%d total_size=%d average_size=%d count=%d\n", size, polsize, size + polsize, total_size, total_size / total_cnt, total_cnt);
  }

}

int csThing::AddCurveVertex (const csVector3 &v, const csVector2 &t)
{
  if (!curve_vertices)
  {
    max_curve_vertices = 10;
    curve_vertices = new csVector3[max_curve_vertices];
    curve_texels = new csVector2[max_curve_vertices];
  }

  while (num_curve_vertices >= max_curve_vertices)
  {
    max_curve_vertices += 10;

    csVector3 *new_vertices = new csVector3[max_curve_vertices];
    csVector2 *new_texels = new csVector2[max_curve_vertices];
    memcpy (
      new_vertices,
      curve_vertices,
      sizeof (csVector3) * num_curve_vertices);
    memcpy (
      new_texels,
      curve_texels,
      sizeof (csVector2) * num_curve_vertices);
    delete[] curve_vertices;
    delete[] curve_texels;
    curve_vertices = new_vertices;
    curve_texels = new_texels;
  }

  curve_vertices[num_curve_vertices] = v;
  curve_texels[num_curve_vertices] = t;
  num_curve_vertices++;
  return num_curve_vertices - 1;
}

void csThing::SetCurveVertex (int idx, const csVector3 &vt)
{
  CS_ASSERT (idx >= 0 && idx < num_curve_vertices);
  curve_vertices[idx] = vt;
  obj_bbox_valid = false;
  delete bbox;
  bbox = NULL;
  curves_transf_ok = false;
}

void csThing::SetCurveTexel (int idx, const csVector2 &vt)
{
  CS_ASSERT (idx >= 0 && idx < num_curve_vertices);
  curve_texels[idx] = vt;
}

void csThing::ClearCurveVertices ()
{
  delete[] curve_vertices;
  curve_vertices = NULL;
  delete[] curve_texels;
  curve_texels = NULL;
  obj_bbox_valid = false;
  delete bbox;
  bbox = NULL;
  curves_transf_ok = false;
}

int csThing::AddVertex (float x, float y, float z)
{
  if (!obj_verts)
  {
    max_vertices = 10;
    obj_verts = new csVector3[max_vertices];

    // Only if we occasionally move do we use the world vertex cache.
    if (cfg_moving == CS_THING_MOVE_OCCASIONAL)
      wor_verts = new csVector3[max_vertices];
    else
      wor_verts = obj_verts;
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

    if (cfg_moving == CS_THING_MOVE_OCCASIONAL)
    {
      csVector3 *new_wor_verts = new csVector3[max_vertices];
      memcpy (new_wor_verts, wor_verts, sizeof (csVector3) * num_vertices);
      delete[] wor_verts;
      wor_verts = new_wor_verts;
    }
    else
      wor_verts = obj_verts;
  }

  // By default all vertices are set with the same object space and world space.
  obj_verts[num_vertices].Set (x, y, z);
  if (cfg_moving == CS_THING_MOVE_OCCASIONAL)
    wor_verts[num_vertices].Set (x, y, z);
  num_vertices++;
  return num_vertices - 1;
}

int csThing::AddVertexSmart (float x, float y, float z)
{
  if (!obj_verts)
  {
    AddVertex (x, y, z);
    return 0;
  }

  int i;
  for (i = 0; i < num_vertices; i++)
  {
    if (
      ABS (x - obj_verts[i].x) < SMALL_EPSILON &&
      ABS (y - obj_verts[i].y) < SMALL_EPSILON &&
      ABS (z - obj_verts[i].z) < SMALL_EPSILON)
    {
      return i;
    }
  }

  AddVertex (x, y, z);
  return num_vertices - 1;
}

void csThing::SetVertex (int idx, const csVector3 &vt)
{
  CS_ASSERT (idx >= 0 && idx < num_vertices);
  obj_verts[idx] = vt;
  if (wor_verts && wor_verts != obj_verts) wor_verts[idx] = vt;
}

void csThing::DeleteVertex (int idx)
{
  CS_ASSERT (idx >= 0 && idx < num_vertices);

  int copysize = sizeof (csVector3) * (num_vertices - idx - 1);
  memmove (obj_verts + idx, obj_verts + idx + 1, copysize);
  if (wor_verts && wor_verts != obj_verts)
    memmove (wor_verts + idx, wor_verts + idx + 1, copysize);
  if (cam_verts) memmove (cam_verts + idx, cam_verts + idx + 1, copysize);
  num_vertices--;
}

void csThing::DeleteVertices (int from, int to)
{
  if (from <= 0 && to >= num_vertices - 1)
  {
    // Delete everything.
    if (wor_verts == obj_verts)
      delete[] obj_verts;
    else
    {
      delete[] wor_verts;
      delete[] obj_verts;
    }

    delete[] cam_verts;
    max_vertices = num_vertices = 0;
    wor_verts = NULL;
    obj_verts = NULL;
    cam_verts = NULL;
    num_cam_verts = 0;
  }
  else
  {
    if (from < 0) from = 0;
    if (to >= num_vertices) to = num_vertices - 1;

    int rangelen = to - from + 1;
    int copysize = sizeof (csVector3) * (num_vertices - from - rangelen);
    memmove (obj_verts + from, obj_verts + from + rangelen, copysize);
    if (wor_verts && wor_verts != obj_verts)
      memmove (wor_verts + from, wor_verts + from + rangelen, copysize);
    if (cam_verts)
      memmove (cam_verts + from, cam_verts + from + rangelen, copysize);
    num_vertices -= rangelen;
  }
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

void csThing::CompressVertices ()
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

  csVector3 *new_wor = 0;
  if (cfg_moving == CS_THING_MOVE_OCCASIONAL)
  {
    new_wor = new csVector3[count_unique];
    new_wor[0] = wor_verts[vt[0].orig_idx];
  }

  vt[0].new_idx = 0;
  j = 1;
  for (i = 1; i < num_vertices; i++)
  {
    if (vt[i].new_idx == i)
    {
      new_obj[j] = obj_verts[vt[i].orig_idx];
      if (cfg_moving == CS_THING_MOVE_OCCASIONAL)
        new_wor[j] = wor_verts[vt[i].orig_idx];
      vt[i].new_idx = j;
      j++;
    }
    else
      vt[i].new_idx = j - 1;
  }

  // Now we sort the table back on orig_idx so that we have
  // a mapping from the original indices to the new one (new_idx).
  qsort (vt, num_vertices, sizeof (CompressVertex), compare_vt_orig);

  // Replace the old vertex tables.
  delete[] obj_verts;
  obj_verts = new_obj;
  if (cfg_moving == CS_THING_MOVE_OCCASIONAL)
  {
    delete[] wor_verts;
    wor_verts = new_wor;
  }
  else
    wor_verts = obj_verts;
  num_vertices = max_vertices = count_unique;

  // Now we can remap the vertices in all polygons.
  for (i = 0; i < polygons.Length (); i++)
  {
    csPolygon3D *p = polygons.Get (i);
    csPolyIndexed &pi = p->GetVertices ();
    int *idx = pi.GetVertexIndices ();
    for (j = 0; j < pi.GetVertexCount (); j++) idx[j] = vt[idx[j]].new_idx;
  }

  delete[] vt;

  // If there is a bounding box we recreate it.
  if (bbox) CreateBoundingBox ();
}

void csThing::RemoveUnusedVertices ()
{
  if (num_vertices <= 0) return ;

  // Copy all the vertices that are actually used by polygons.
  bool *used = new bool[num_vertices];
  int i, j;
  for (i = 0; i < num_vertices; i++) used[i] = false;

  // Mark all vertices that are used as used.
  for (i = 0; i < polygons.Length (); i++)
  {
    csPolygon3D *p = polygons.Get (i);
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
  csVector3 *new_wor = 0;
  int *relocate = new int[num_vertices];
  if (cfg_moving == CS_THING_MOVE_OCCASIONAL)
    new_wor = new csVector3[count_relevant];
  j = 0;
  for (i = 0; i < num_vertices; i++)
  {
    if (used[i])
    {
      new_obj[j] = obj_verts[i];
      if (cfg_moving == CS_THING_MOVE_OCCASIONAL) new_wor[j] = wor_verts[i];
      relocate[i] = j;
      j++;
    }
    else
      relocate[i] = -1;
  }

  // Replace the old vertex tables.
  delete[] obj_verts;
  obj_verts = new_obj;
  if (cfg_moving == CS_THING_MOVE_OCCASIONAL)
  {
    delete[] wor_verts;
    wor_verts = new_wor;
  }
  else
    wor_verts = obj_verts;
  num_vertices = max_vertices = count_relevant;

  // Now we can remap the vertices in all polygons.
  for (i = 0; i < polygons.Length (); i++)
  {
    csPolygon3D *p = polygons.Get (i);
    csPolyIndexed &pi = p->GetVertices ();
    int *idx = pi.GetVertexIndices ();
    for (j = 0; j < pi.GetVertexCount (); j++) idx[j] = relocate[idx[j]];
  }

  delete[] relocate;
  delete[] used;

  // If there is a bounding box we recreate it.
  if (bbox) CreateBoundingBox ();
}

void csThing::BuildStaticTree (const char *name, int mode)
{
  //mode = BSP_BALANCE_AND_SPLITS;
  delete static_tree;
  static_tree = NULL;

  CreateBoundingBox ();

  csBox3 bbox;
  GetBoundingBox (bbox);
  static_tree = new csOctree (
      this,
      bbox.Min (),
      bbox.Max (),
      150 /*15*/,
      mode);

  csEngine *w = csEngine::current_engine;
  iCacheManager* cache_mgr = w->GetCacheManager ();

  if (GetCacheName ())
  {
    char buf[100];
    sprintf (buf, "%s_%s", GetCacheName (), name);
    cache_mgr->SetCurrentScope (buf);
  }
  else
  {
    char buf[100];
    sprintf (buf, "defO_%s", name);
    cache_mgr->SetCurrentScope (buf);
  }

  bool recalc_octree = true;
  if (!csEngine::do_force_revis)
  {
    recalc_octree = false;
    csEngine::current_engine->Report ("Loading bsp/octree...");
    recalc_octree = !((csOctree *)static_tree)->ReadFromCache (
        cache_mgr,
        GetPolygonArray ().GetArray (),
        GetPolygonArray ().Length ());
    if (recalc_octree)
    {
      delete static_tree;
      static_tree = new csOctree (
          this,
          bbox.Min (),
          bbox.Max (),
          150 /*15*/,
          mode);
    }

    cache_mgr->SetCurrentScope (NULL);
  }

  if (recalc_octree)
  {
    csEngine::current_engine->Report ("Calculate bsp/octree...");
    static_tree->Build (GetPolygonArray ());

    csEngine::current_engine->Report ("Caching bsp/octree...");
    ((csOctree *)static_tree)->Cache (cache_mgr);

    cache_mgr->SetCurrentScope (NULL);
  }

  csEngine::current_engine->Report ("Compress vertices...");
  CompressVertices ();
  RemoveUnusedVertices ();
  csEngine::current_engine->Report ("Build vertex tables...");
  ((csOctree *)static_tree)->BuildVertexTables ();

  static_tree->Statistics ();
}

csPolygonInt *csThing::GetPolygonInt (int idx)
{
  return (csPolygonInt *)GetPolygon3D (idx);
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

csPolygon3D *csThing::NewPolygon (csMaterialWrapper *material)
{
  csPolygon3D *p = new csPolygon3D (material);
  AddPolygon (p);
  return p;
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
  thing_edges_valid = false;
  prepared = false;
  obj_bbox_valid = false;
  delete bbox;
  bbox = NULL;
  CleanupThingEdgeTable ();

  shapenr++;
  scfiPolygonMeshLOD.Cleanup ();
  scfiPolygonMesh.Cleanup ();
  delete [] obj_normals; obj_normals = NULL;
  FireListeners ();
}

void csThing::RemovePolygon (int idx)
{
  InvalidateThing ();

  csPolygon3D *poly3d = GetPolygon3D (idx);
  if (poly3d->GetPortal ())
  {
    RemovePortalPolygon (poly3d);
  }

  polygons.Delete (idx);
}

iPolygonMesh* csThing::GetWriteObject ()
{
  return &scfiPolygonMeshLOD;
}

void csThing::RemovePolygons ()
{
  InvalidateThing ();
  polygons.DeleteAll ();          // delete prior to portal_poly array !
  if (portal_polygons.Length ()) portal_polygons.DeleteAll ();
}

void csThing::AddPolygon (csPolygonInt *poly)
{
  InvalidateThing ();

  csPolygon3D *poly3d = (csPolygon3D *)poly;
  poly3d->SetParent (this);
  polygons.Push (poly3d);
  if (poly3d->GetPortal ()) AddPortalPolygon (poly3d);
}

csCurve *csThing::GetCurve (char *name) const
{
  int idx = curves.FindKey (name);
  return idx >= 0 ? curves.Get (idx) : NULL;
}

void csThing::AddCurve (csCurve *curve)
{
  curve->SetParentThing (this);
  curves.Push (curve);
  curves_transf_ok = false;
  obj_bbox_valid = false;
  delete bbox;
  bbox = NULL;
}

iCurve *csThing::CreateCurve (iCurveTemplate *tmpl)
{
  iCurve *curve = tmpl->MakeCurve ();
  csCurve *c = curve->GetOriginalObject ();
  c->SetParentThing (this);

  int i;
  for (i = 0; i < tmpl->GetVertexCount (); i++)
    curve->SetControlPoint (i, tmpl->GetVertex (i));
  AddCurve (c);
  return curve;
}

int csThing::FindCurveIndex (iCurve *curve) const
{
  return curves.Find (curve->GetOriginalObject ());
}

void csThing::RemoveCurve (int idx)
{
  curves.Delete (idx);
  curves_transf_ok = false;
  obj_bbox_valid = false;
  delete bbox;
  bbox = NULL;
}

void csThing::RemoveCurves ()
{
  curves.DeleteAll ();
  curves_transf_ok = false;
  obj_bbox_valid = false;
  delete bbox;
  bbox = NULL;
}

void csThing::HardTransform (const csReversibleTransform &t)
{
  int i, j;

  for (i = 0; i < num_vertices; i++)
  {
    obj_verts[i] = t.This2Other (obj_verts[i]);
    if (cfg_moving == CS_THING_MOVE_OCCASIONAL) wor_verts[i] = obj_verts[i];
  }

  curves_center = t.This2Other (curves_center);
  if (curve_vertices)
    for (i = 0; i < num_curve_vertices; i++)
      curve_vertices[i] = t.This2Other (curve_vertices[i]);

  //-------
  // First we collect all planes from the set of polygons
  // and transform each plane one by one. We actually create
  // new planes and ditch the others (DecRef()) to avoid
  // sharing planes with polygons that were not affected by
  // this HardTransform().
  csHashSet planes (8087);
  for (i = 0; i < polygons.Length (); i++)
  {
    csPolygon3D *p = GetPolygon3D (i);
    csPolyTexLightMap *lmi = p->GetLightMapInfo ();
    if (lmi)
    {
      iPolyTxtPlane *pl = lmi->GetPolyTxtPlane ();
      planes.Add (pl);
    }
  }

  // As a special optimization we don't copy planes that have no
  // name. This under the assumption that planes with no name
  // cannot be shared (let's make this a rule!).
  csHashIterator *hashit = new csHashIterator (planes.GetHashMap ());
  while (hashit->HasNext ())
  {
    iPolyTxtPlane *pl = (iPolyTxtPlane *)hashit->Next ();
    if (pl->QueryObject ()->GetName () == NULL)
    {
      // This is a non-shared plane.
      pl->GetPrivateObject ()->HardTransform (t);
    }
    else
    {
      // This plane is potentially shared. We have to make a duplicate
      // and modify all polygons to use this one. Note that this will
      // mean that potentially two planes exist with the same name.
      csRef<iPolyTxtPlane> new_pl (csEngine::current_engine->thing_type->
        CreatePolyTxtPlane (pl->QueryObject ()->GetName ()));
      csMatrix3 m;
      csVector3 v;
      pl->GetTextureSpace (m, v);
      new_pl->SetTextureSpace (m, v);
      new_pl->GetPrivateObject ()->HardTransform (t);
      for (j = 0; j < polygons.Length (); j++)
      {
        csPolygon3D *p = GetPolygon3D (j);
        csPolyTexLightMap *lmi = p->GetLightMapInfo ();
        if (lmi && lmi->GetPolyTxtPlane () == pl)
        {
          lmi->SetTxtPlane (new_pl->GetPrivateObject ());
          lmi->Setup (p, p->GetMaterialWrapper ());
        }
      }
    }
  }

  delete hashit;

  //-------
  //-------
  // Now do a similar thing for the normal planes.
  planes.DeleteAll ();
  for (i = 0; i < polygons.Length (); i++)
  {
    csPolygon3D *p = GetPolygon3D (i);
    csPolyPlane *pl = p->GetPlane ();
    if (!planes.In (pl))
    {
      planes.Add (pl);

      csPlane3 new_plane;
      t.This2Other (pl->GetObjectPlane (), p->Vobj (0), new_plane);
      pl->GetObjectPlane () = new_plane;
      pl->GetWorldPlane () = new_plane;
    }
  }

  //-------
  for (i = 0; i < polygons.Length (); i++)
  {
    csPolygon3D *p = GetPolygon3D (i);
    p->HardTransform (t);
  }

  curves_transf_ok = false;
  for (i = 0; i < curves.Length (); i++)
  {
    csCurve *c = GetCurve (i);
    c->HardTransform (t);
  }
}

struct ISectData
{
  csSegment3 seg;
  csVector3 isect;
  float r;
  bool only_local;      // If true we only consider polygons in the culler mesh
  csMeshWrapper *mesh;  // Hit mesh
};

/*
 * @@@
 * This function does not yet do anything but it should
 * use the PVS as soon as we have that to make IntersectSegment
 * even faster.
 */
static bool IntersectSegmentCull (
  csPolygonTree *

  /*tree*/,
  csPolygonTreeNode *node,
  const csVector3 &

  /*pos*/,
  void *data)
{
  if (!node) return false;
  if (node->Type () != NODE_OCTREE) return true;

  ISectData *idata = (ISectData *)data;
  csOctreeNode *onode = (csOctreeNode *)node;
  csVector3 isect;
  if (csIntersect3::BoxSegment (onode->GetBox (), idata->seg, isect) > -1)
    return true;

  // Segment does not intersect with node so we return false here.
  return false;
}

static void *IntersectSegmentTestPol (
  csThing *thing,
  csPolygonInt **polygon,
  int num,
  bool

  /*same_plane*/,
  void *data)
{
  ISectData *idata = (ISectData *)data;
  int i;
  for (i = 0; i < num; i++)
  {
    if (polygon[i]->GetType () == 1)
    {
      csPolygon3D *p = (csPolygon3D *)polygon[i];
      if (
        p->IntersectSegment (
            idata->seg.Start (),
            idata->seg.End (),
            idata->isect,
            &(idata->r)))
      {
        idata->mesh = NULL;
        return (void *)p;
      }
    }
    else if ((!idata->only_local) && polygon[i]->GetType () == 3)
    {
      csBspPolygon *bsppol = (csBspPolygon *)polygon[i];
      csVisObjInfo *obj = bsppol->GetOriginator ();
      csRef<iMeshWrapper> mesh (SCF_QUERY_INTERFACE (
          obj->visobj,
          iMeshWrapper));
      if (mesh)
      {
        if (mesh->GetMeshObject () == &(thing->scfiMeshObject)) continue;
        if (
          bsppol->IntersectSegment (
              idata->seg.Start (),
              idata->seg.End (),
              idata->isect,
              NULL))
        {
          csRef<iThingState> thing (SCF_QUERY_INTERFACE (
              mesh->GetMeshObject (),
              iThingState));
          if (thing)
          {
            //@@@@@@ UGLY!
            csThing *th = (csThing *) (thing->GetPrivateObject ());

            csPolygon3D *p = th->IntersectSegment (
                idata->seg.Start (),
                idata->seg.End (),
                idata->isect,
                &(idata->r));
            if (p)
            {
              idata->mesh = mesh->GetPrivateObject ();
              return p;
            }
          }
        }
      }
    }
  }

  return NULL;
}

csPolygon3D *csThing::IntersectSegmentFull (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect,
  float *pr,
  csMeshWrapper **p_mesh)
{
  if (p_mesh) *p_mesh = NULL;
  if (static_tree)
  {
    // Version with culler.
    ISectData idata;
    idata.mesh = NULL;
    idata.only_local = false;
    idata.seg.Set (start, end);

    void *rc = static_tree->Front2Back (
        start,
        IntersectSegmentTestPol,
        (void *) &idata,
        IntersectSegmentCull,
        (void *) &idata);
    if (rc)
    {
      if (pr) *pr = idata.r;
      isect = idata.isect;
      if (p_mesh) *p_mesh = idata.mesh;
      return (csPolygon3D *)rc;
    }

    return NULL;
  }
  else
  {
    // Version without culler.
    int i;
    float r, best_r = 2000000000.;
    csVector3 cur_isect;
    csPolygon3D *best_p = NULL;

    // @@@ This routine is not very optimal. Especially for things

    // with large number of polygons.
    for (i = 0; i < polygons.Length (); i++)
    {
      csPolygon3D *p = polygons.Get (i);
      if (p->IntersectSegment (start, end, cur_isect, &r))
      {
        if (r < best_r)
        {
          best_r = r;
          best_p = p;
          isect = cur_isect;
        }
      }
    }

    if (pr) *pr = best_r;
    if (p_mesh) *p_mesh = NULL;
    return best_p;
  }
}

csPolygon3D *csThing::IntersectSegment (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect,
  float *pr,
  bool only_portals)
{
  if (only_portals)
  {
    int i;
    float r, best_r = 2000000000.;
    csVector3 cur_isect;
    csPolygon3D *best_p = NULL;
    for (i = 0; i < portal_polygons.Length (); i++)
    {
      csPolygon3D *p = (csPolygon3D *)portal_polygons[i];
      if (p->IntersectSegment (start, end, cur_isect, &r))
      {
        if (r < best_r)
        {
          best_r = r;
          best_p = p;
          isect = cur_isect;
        }
      }
    }

    if (pr) *pr = best_r;
    return best_p;
  }

  if (static_tree)
  {
    // Version with culler.
    ISectData idata;
    idata.only_local = true;
    idata.seg.Set (start, end);

    void *rc = static_tree->Front2Back (
        start,
        IntersectSegmentTestPol,
        (void *) &idata,
        IntersectSegmentCull,
        (void *) &idata);
    if (rc)
    {
      if (pr) *pr = idata.r;
      isect = idata.isect;
      return (csPolygon3D *)rc;
    }

    return NULL;
  }
  else
  {
    // Version without culler.
    int i;
    float r, best_r = 2000000000.;
    csVector3 cur_isect;
    csPolygon3D *best_p = NULL;

    // @@@ This routine is not very optimal. Especially for things

    // with large number of polygons.
    for (i = 0; i < polygons.Length (); i++)
    {
      csPolygon3D *p = polygons.Get (i);
      if (p->IntersectSegment (start, end, cur_isect, &r))
      {
        if (r < best_r)
        {
          best_r = r;
          best_p = p;
          isect = cur_isect;
        }
      }
    }

    if (pr) *pr = best_r;
    return best_p;
  }
}

bool csThing::HitBeamOutline (const csVector3& start,
  const csVector3& end, csVector3& isect, float* pr)
{
  if (static_tree)
  {
    // Version with culler.
    ISectData idata;
    idata.only_local = true;
    idata.seg.Set (start, end);

    void *rc = static_tree->Front2Back (
        start,
        IntersectSegmentTestPol,
        (void *) &idata,
        IntersectSegmentCull,
        (void *) &idata);
    if (rc)
    {
      if (pr) *pr = idata.r;
      isect = idata.isect;
      return rc != NULL;
    }

    return false;
  }
  else
  {
    // Version without culler.
    int i;
    float r;

    // @@@ This routine is not very optimal. Especially for things
    // with large number of polygons.
    for (i = 0; i < polygons.Length (); i++)
    {
      csPolygon3D *p = polygons.Get (i);
      if (p->IntersectSegment (start, end, isect, &r))
      {
        if (pr) *pr = r;
        return true;
      }
    }

    return false;
  }
}

bool csThing::HitBeamObject (const csVector3& start,
  const csVector3& end, csVector3& isect, float *pr)
{
  csPolygon3D* poly = IntersectSegment (start, end, isect, pr, false);
  if (poly) return true;
  return false;
}

#ifndef CS_USE_NEW_RENDERER
void csThing::DrawOnePolygon (
  csPolygon3D *p,
  csPolygon2D *poly,
  iRenderView *d,
  csZBufMode zMode)
{
  iCamera *icam = d->GetCamera ();

  if (d->AddedFogInfo ())
  {
    // If fog info was added then we are dealing with vertex fog and
    // the current sector has fog. This means we have to complete the
    // fog_info structure with the plane of the current polygon.
    d->GetFirstFogInfo ()->outgoing_plane = p->GetPlane ()->GetCameraPlane ();
  }

  Stats::polygons_drawn++;

  csPortal *po = p->GetPortal ();
  if (csSector::do_portals && po)
  {
    bool filtered = false;

    // is_this_fog is true if this sector is fogged.
    bool is_this_fog = d->GetThisSector ()->HasFog ();

    // If there is filtering (alpha mapping or something like that) we need
    // to keep the texture plane so that it can be drawn after the sector has
    // been drawn. The texture plane needs to be kept because this polygon
    // may be rendered again (through mirrors) possibly overwriting the plane.
    csPolyPlane *keep_plane = NULL;
    if (
      d->GetGraphics3D ()->GetRenderState (
          G3DRENDERSTATE_TRANSPARENCYENABLE))
      filtered = p->IsTransparent ();
    if (filtered || is_this_fog || (po && po->flags.Check (CS_PORTAL_ZFILL)))
    {
      keep_plane = new csPolyPlane (*(p->GetPlane ()));
    }
    // Before we draw through the portal we see if we are rendering
    // for an object that uses a static tree (i.e. c-buffer). If that is
    // the case we clear the c-buffer for the portal shape. We can safely
    // do this because the c-buffer algo for this sector has finished
    // anyway.
    if (
      csEngine::current_engine->GetEngineMode () == CS_ENGINE_FRONT2BACK)
    {
      csCBuffer *c_buffer = csEngine::current_engine->GetCBuffer ();
      if (c_buffer)
      {
        c_buffer->Initialize ();
        c_buffer->InsertPolygon (
            poly->GetVertices (),
            poly->GetVertexCount (),
            true);
      }
    }

    // Draw through the portal. If this fails we draw the original polygon
    // instead. Drawing through a portal can fail because we have reached
    // the maximum number that a sector is drawn (for mirrors).
    if (po->Draw (poly, p, d))
    {
      if (filtered) poly->DrawFilled (d, p, keep_plane, zMode);
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
      poly->DrawFilled (d, p, p->GetPlane (), zMode);

    // Cleanup.
    if (keep_plane) keep_plane->DecRef ();
  }
  else
    poly->DrawFilled (d, p, p->GetPlane (), zMode);
}

void csThing::DrawPolygonArray (
  csPolygonInt **polygon,
  int num,
  iRenderView *d,
  csZBufMode zMode)
{
  csPolygon3D *p;
  csVector3 *verts;
  int num_verts;
  int i;
  csPoly2DPool *render_pool = csEngine::current_engine->render_pol2d_pool;
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

  for (i = 0; i < num; i++)
  {
    if (polygon[i]->GetType () != 1) continue;
    p = (csPolygon3D *)polygon[i];
    if (p->flags.Check (CS_POLY_NO_DRAW)) continue;
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
        if (
          p->DoPerspective (
              camtrans,
              verts,
              num_verts,
              clip,
              NULL,
              icam->IsMirrored ()) &&
          clip->ClipAgainst (d->GetClipper ()))
        {
          p->GetPlane ()->WorldToCamera (camtrans, verts[0]);
          DrawOnePolygon (p, clip, d, zMode);
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

void csThing::PreparePolygonBuffer ()
{
  if (polybuf) return ;

  iVertexBufferManager *vbufmgr = csEngine::current_engine->G3D->
    GetVertexBufferManager ();
  polybuf = vbufmgr->CreatePolygonBuffer ();
  polybuf->SetVertexArray (obj_verts, num_vertices);

  int i;

  //-----
  // First collect all material wrappers and polygons.
  //-----
  MatPol *matpol = new MatPol[polygons.Length ()];
  for (i = 0; i < polygons.Length (); i++)
  {
    matpol[i].poly = GetPolygon3D (i);
    matpol[i].mat = &(matpol[i].poly->GetMaterialWrapper ()
    	->scfiMaterialWrapper);
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
  for (i = 0; i < polygons.Length (); i++)
  {
    csPolygon3D *poly = matpol[i].poly;
    csPolyTexLightMap *lmi = poly->GetLightMapInfo ();

    // @@@ what if lmi == NULL?
    //CS_ASSERT (lmi != NULL);
    if (lmi)
    {
      csPolyTxtPlane *txt_plane = lmi->GetTxtPlane ();
      csMatrix3 *m_obj2tex;
      csVector3 *v_obj2tex;
      txt_plane->GetObjectToTexture (m_obj2tex, v_obj2tex);
      polybuf->AddPolygon (
          poly->GetVertexIndices (),
          poly->GetVertexCount (),
          poly->GetPlane ()->GetObjectPlane (),
          matpol[i].mat_index,
          *m_obj2tex,
          *v_obj2tex,
          lmi->GetPolyTex ());
    }
    else
    {
      csMatrix3 m_obj2tex;	// @@@
      csVector3 v_obj2tex;
      polybuf->AddPolygon (
          poly->GetVertexIndices (),
          poly->GetVertexCount (),
          poly->GetPlane ()->GetObjectPlane (),
          matpol[i].mat_index,
          m_obj2tex,
          v_obj2tex,
          NULL);
    }
  }

  delete[] matpol;
}
#endif // CS_USE_NEW_RENDERER

void csThing::GetTransformedBoundingBox (
  const csReversibleTransform &trans,
  csBox3 &cbox)
{
  //@@@@@@@@@@@@@@

  // @@@ Shouldn't we try to cache this depending on camera/movable number?

  // Similar to what happens in csSprite3D.
  csBox3 box;
  GetBoundingBox (box);
  cbox.StartBoundingBox (trans * box.GetCorner (0));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (1));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (2));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (3));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (4));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (5));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (6));
  cbox.AddBoundingVertexSmart (trans * box.GetCorner (7));
}

static void Perspective (
  const csVector3 &v,
  csVector2 &p,
  float fov,
  float sx,
  float sy)
{
  float iz = fov / v.z;
  p.x = v.x * iz + sx;
  p.y = v.y * iz + sy;
}

float csThing::GetScreenBoundingBox (
  float fov,
  float sx,
  float sy,
  const csReversibleTransform &trans,
  csBox2 &sbox,
  csBox3 &cbox)
{
  csVector2 oneCorner;

  GetTransformedBoundingBox (trans, cbox);

  // if the entire bounding box is behind the camera, we're done
  if ((cbox.MinZ () < 0) && (cbox.MaxZ () < 0))
  {
    return -1;
  }

  // Transform from camera to screen space.
  if (cbox.MinZ () <= 0)
  {
    // Sprite is very close to camera.

    // Just return a maximum bounding box.
    sbox.Set (-10000, -10000, 10000, 10000);
  }
  else
  {
    Perspective (cbox.Max (), oneCorner, fov, sx, sy);
    sbox.StartBoundingBox (oneCorner);

    csVector3 v (cbox.MinX (), cbox.MinY (), cbox.MaxZ ());
    Perspective (v, oneCorner, fov, sx, sy);
    sbox.AddBoundingVertexSmart (oneCorner);
    Perspective (cbox.Min (), oneCorner, fov, sx, sy);
    sbox.AddBoundingVertexSmart (oneCorner);
    v.Set (cbox.MaxX (), cbox.MaxY (), cbox.MinZ ());
    Perspective (v, oneCorner, fov, sx, sy);
    sbox.AddBoundingVertexSmart (oneCorner);
  }

  return cbox.MaxZ ();
}

#ifndef CS_USE_NEW_RENDERER
void csThing::DrawPolygonArrayDPM (
  csPolygonInt ** /*polygon*/,
  int /*num*/,
  iRenderView *rview,
  iMovable *movable,
  csZBufMode zMode)
{
  PreparePolygonBuffer ();

  iCamera *icam = rview->GetCamera ();
  csReversibleTransform tr_o2c = icam->GetTransform ()
  	/ movable->GetFullTransform ();

  G3DPolygonMesh mesh;
  csBox2 bbox;
  csBox3 bbox3;
  if (
    GetScreenBoundingBox (
        icam->GetFOV (),
        icam->GetShiftX (),
        icam->GetShiftY (),
        tr_o2c,
        bbox,
        bbox3) < 0)
    return ;  // Not visible.
  if (
    !rview->ClipBBox (
        bbox,
        bbox3,
        mesh.clip_portal,
        mesh.clip_plane,
        mesh.clip_z_plane))
    return ;  // Not visible.
  int i;

  rview->GetGraphics3D ()->SetObjectToCamera (&tr_o2c);
  rview->GetGraphics3D ()->SetRenderState (G3DRENDERSTATE_ZBUFFERMODE, zMode);

  mesh.polybuf = polybuf;
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

#ifdef CS_DEBUG
bool viscnt_enabled;
int viscnt_vis_poly;
int viscnt_invis_poly;
int viscnt_vis_node;
int viscnt_invis_node;
int viscnt_vis_obj;
int viscnt_invis_obj;
float viscnt_vis_node_vol;
float viscnt_invis_node_vol;
#endif

void *csThing::TestQueuePolygonArray (
  csPolygonInt **polygon,
  int num,
  iRenderView *d,
  csPolygon2DQueue *poly_queue,
  bool pvs,
  uint32 current_visnr)
{
  csPolygon3D *p;
  csVector3 *verts;
  int num_verts;
  int i;
  csCBuffer *c_buffer = csEngine::current_engine->GetCBuffer ();
  bool visible;
  csPoly2DPool *render_pool = csEngine::current_engine->render_pol2d_pool;
  csPolygon2D *clip;
  iCamera *icam = d->GetCamera ();
  const csReversibleTransform &camtrans = icam->GetTransform ();

  // Normally visibility of objects in the bsp tree is handled by inserting
  // the bounding box polygons of the object in the tree. However, this
  // insertion will not modify the tree meaning that if we reach a bsp leaf
  // we will just insert all remaining bounding box polygons in that leaf
  // without further subdividing the leaf as would be needed for real
  // bsp building.
  //
  // When doing visibility testing we traverse the tree from front to back
  // and insert all polygons in the c-buffer. Bounding box polygons
  // are not inserted but only tested to see if the object is visible or not.
  //
  // So this means that the bsp code should always traverse the bounding
  // box polygons first before giving the real polygons.
  for (i = 0; i < num; i++)
  {
    if (!csEngine::ProcessPolygon ()) return (void *)1;

    // For every polygon we perform some type of culling (2D or 3D culling).
    // If 2D culling (c-buffer) then transform it to screen space
    // and perform clipping to Z plane. Then test against the c-buffer
    // to see if it is visible.
    // If 3D culling (quadtree3D) then we just test it.
    if (polygon[i]->GetType () == 3)
    {
      // We're dealing with a csBspPolygon.
      csBspPolygon *bsppol = (csBspPolygon *)polygon[i];
      csVisObjInfo *obj = bsppol->GetOriginator ();
      bool obj_vis = obj->visobj->GetVisibilityNumber () == current_visnr;
      csPolyTreeBBox *tbb = obj->bbox;

      // If the object is already marked visible then we don't have
      // to do any of the other processing for this polygon.
      if (!obj_vis)
      {
        // Since we have a csBspPolygon we know that the poly tree object
        // is a csPolyTreeBBox instance.
        if (!tbb->IsTransformed (icam->GetCameraNumber ()))
        {
          // The bbox of this object has not yet been transformed
          // to camera space.
          tbb->World2Camera (camtrans, icam->GetCameraNumber ());
        }

        // Culling.
        bool mark_vis = false;
        csPlane3 clip_plane, *pclip_plane;
        bool do_clip_plane = d->GetClipPlane (clip_plane);
        if (do_clip_plane)
          pclip_plane = &clip_plane;
        else
          pclip_plane = NULL;
        clip = (csPolygon2D *) (render_pool->Alloc ());
        if (
          bsppol->ClipToPlane (
              pclip_plane,
              camtrans.GetOrigin (),
              verts,
              num_verts,
              !icam->IsMirrored ()))
        {
          csPlane3 *plclip = icam->GetFarPlane ();

          // The far plane is defined negative. So if the polygon is entirely
          // in front of the far plane it is not visible. Otherwise we will
          // render it.
          if (
            !plclip ||
            csPoly3D::Classify (*plclip, verts, num_verts) != CS_POL_FRONT)
          {
            if (
              bsppol->DoPerspective (
                  camtrans,
                  verts,
                  num_verts,
                  clip,
                  icam->IsMirrored ()) &&
              clip->ClipAgainst (d->GetClipper ()))
            {
              if (c_buffer->TestPolygon (
                    clip->GetVertices (),
                    clip->GetVertexCount ()))
              {
                mark_vis = true;
#		ifdef CS_DEBUG
		viscnt_vis_obj++;
#		endif
              }
#	      ifdef CS_DEBUG
	      else viscnt_invis_obj++;
#	      endif
	    }
          }
        }

        if (mark_vis) obj->visobj->SetVisibilityNumber (current_visnr);
        if (clip) render_pool->Free (clip);
      }
    }
    else
    {
      // We're dealing with a csPolygon3D.
      // @@@ We should only alloc the 2D polygon when we need to do it.
      // So this means further down the 'if'.
      p = (csPolygon3D *)polygon[i];
      if (pvs)
      {
        //if (!p->IsVisible ())
        //{
        // Polygon is not visible because of PVS.
        //@@@ CURRENTLY DISABLED continue;
        //}
      }

      visible = false;
      clip = NULL;

      // Culling.
      if (p->flags.Check (CS_POLY_NO_DRAW))
      {
        // Don't draw this polygon.
      }
      else
      {
        csPlane3 clip_plane, *pclip_plane;
        bool do_clip_plane = d->GetClipPlane (clip_plane);
        if (do_clip_plane)
          pclip_plane = &clip_plane;
        else
          pclip_plane = NULL;

        csPlane3 *plclip = icam->GetFarPlane ();

        clip = (csPolygon2D *) (render_pool->Alloc ());
        if (
          p->ClipToPlane (
              pclip_plane,
              camtrans.GetOrigin (),
              verts,
              num_verts) &&
          (
            !plclip ||
            csPoly3D::Classify (*plclip, verts, num_verts) != CS_POL_FRONT
          ) &&
          p->DoPerspective (
              camtrans,
              verts,
              num_verts,
              clip,
              NULL,
              icam->IsMirrored ()) &&
          clip->ClipAgainst (d->GetClipper ()))
        {
          if (csEngine::current_engine->IsPVSOnly ())
            visible = true;
          else
          {
            // A portal is considered an non-transparent surface
            // here (i.e. a normal polygon). So we simply do
            // InsertPolygon() as opposed to TestPolygon() for portals.
            visible = c_buffer->InsertPolygon (
                  clip->GetVertices (),
                  clip->GetVertexCount ());
#	    ifdef CS_DEBUG
	    if (visible) viscnt_vis_poly++;
	    else viscnt_invis_poly++;
#	    endif
          }
        }
      }

      if (visible && !clip)
      {
        // If visible and we don't already have a clip (i.e. we did 3D culling)
        // then we need to make it here. It is still possible that the
        // polygon will be culled at this stage.
        csPlane3 clip_plane, *pclip_plane;
        bool do_clip_plane = d->GetClipPlane (clip_plane);
        if (do_clip_plane)
          pclip_plane = &clip_plane;
        else
          pclip_plane = NULL;

        csPlane3 *plclip = icam->GetFarPlane ();
        clip = (csPolygon2D *) (render_pool->Alloc ());
        if (
          !(
            p->ClipToPlane (
                pclip_plane,
                camtrans.GetOrigin (),
                verts,
                num_verts) &&
              (
                !plclip ||
                csPoly3D::Classify (
                    *plclip,
                    verts,
                    num_verts) != CS_POL_FRONT
              ) && p->DoPerspective (
                  camtrans,
                  verts,
                  num_verts,
                  clip,
                  NULL,
                  icam->IsMirrored ()) && clip->ClipAgainst (d->GetClipper ())
          ))
        {
          visible = false;
        }
      }

      if (visible)
      {
        poly_queue->Push (p, clip);
        Stats::polygons_accepted++;
        if (c_buffer && c_buffer->IsFull ()) return (void *)1;  // Stop
      }
      else
      {
        if (clip) render_pool->Free (clip);
        Stats::polygons_rejected++;
      }
    }
  }

  return NULL;
}

// @@@ We need a more clever algorithm here. We should try
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
  //@@@ Ok?
  cached_movable = movable;
  WorUpdate ();

  iShadowBlock *list = shadows->NewShadowBlock (
      polygons.Length ());
  csFrustum *frust;
  int i, j;
  csPolygon3D *p;
  bool cw = true;                   //@@@ Use mirroring parameter here!
  for (i = 0; i < polygons.Length (); i++)
  {
    p = polygons.Get (i);
    if (p->GetPortal ()) continue;  // No portals

    //if (p->GetPlane ()->VisibleFromPoint (origin) != cw) continue;
    float clas = p->GetPlane ()->GetWorldPlane ().Classify (origin);
    if (ABS (clas) < EPSILON) continue;
    if ((clas <= 0) != cw) continue;

    csPlane3 pl = p->GetPlane ()->GetWorldPlane ();
    pl.DD += origin * pl.norm;
    pl.Invert ();
    frust = list->AddShadow (
        origin,
        (void *)p,
        p->GetVertices ().GetVertexCount (),
        pl);
    for (j = 0; j < p->GetVertices ().GetVertexCount (); j++)
      frust->GetVertex (j).Set (p->Vwor (j) - origin);
  }
}

void csThing::CreateBoundingBox ()
{
  float minx, miny, minz, maxx, maxy, maxz;
  delete bbox;
  bbox = NULL;
  if (num_vertices <= 0 && num_curve_vertices <= 0) return ;
  bbox = new csThingBBox ();
  int i;
  if (num_vertices > 0)
  {
    minx = maxx = obj_verts[0].x;
    miny = maxy = obj_verts[0].y;
    minz = maxz = obj_verts[0].z;

    for (i = 1; i < num_vertices; i++)
    {
      if (obj_verts[i].x < minx)
        minx = obj_verts[i].x;
      else if (obj_verts[i].x > maxx)
        maxx = obj_verts[i].x;
      if (obj_verts[i].y < miny)
        miny = obj_verts[i].y;
      else if (obj_verts[i].y > maxy)
        maxy = obj_verts[i].y;
      if (obj_verts[i].z < minz)
        minz = obj_verts[i].z;
      else if (obj_verts[i].z > maxz)
        maxz = obj_verts[i].z;
    }
  }
  else if (num_curve_vertices == 0)
  {
    minx = 10000000.;
    miny = 10000000.;
    minz = 10000000.;
    maxx = -10000000.;
    maxy = -10000000.;
    maxz = -10000000.;
  }

  if (num_curve_vertices > 0)
  {
    int stidx = 0;
    if (num_vertices == 0)
    {
      csVector3 &cv = curve_vertices[0];
      minx = maxx = cv.x;
      miny = maxy = cv.y;
      minz = maxz = cv.z;
      stidx = 1;
    }

    for (i = stidx ; i < num_curve_vertices ; i++)
    {
      csVector3 &cv = curve_vertices[i];
      if (cv.x < minx)
        minx = cv.x;
      else if (cv.x > maxx)
        maxx = cv.x;
      if (cv.y < miny)
        miny = cv.y;
      else if (cv.y > maxy)
        maxy = cv.y;
      if (cv.z < minz)
        minz = cv.z;
      else if (cv.z > maxz)
        maxz = cv.z;
    }
  }

  bbox->i7 = AddVertex (minx, miny, minz);
  bbox->i3 = AddVertex (minx, miny, maxz);
  bbox->i5 = AddVertex (minx, maxy, minz);
  bbox->i1 = AddVertex (minx, maxy, maxz);
  bbox->i8 = AddVertex (maxx, miny, minz);
  bbox->i4 = AddVertex (maxx, miny, maxz);
  bbox->i6 = AddVertex (maxx, maxy, minz);
  bbox->i2 = AddVertex (maxx, maxy, maxz);
}

void csThing::GetRadius (csVector3 &rad, csVector3 &cent)
{
  csBox3 b;
  GetBoundingBox (b);
  rad = obj_radius;
  cent = b.GetCenter ();
}

void csThing::GetBoundingBox (csBox3 &box)
{
  if (obj_bbox_valid)
  {
    box = obj_bbox;
    return ;
  }

  obj_bbox_valid = true;

  if (!bbox) CreateBoundingBox ();

  if (!obj_verts)
  {
    obj_bbox.Set (0, 0, 0, 0, 0, 0);
    box = obj_bbox;
    return ;
  }

  csVector3 min_bbox, max_bbox;
  min_bbox = max_bbox = Vobj (bbox->i1);
  if (Vobj (bbox->i2).x < min_bbox.x)
    min_bbox.x = Vobj (bbox->i2).x;
  else if (Vobj (bbox->i2).x > max_bbox.x)
    max_bbox.x = Vobj (bbox->i2).x;
  if (Vobj (bbox->i2).y < min_bbox.y)
    min_bbox.y = Vobj (bbox->i2).y;
  else if (Vobj (bbox->i2).y > max_bbox.y)
    max_bbox.y = Vobj (bbox->i2).y;
  if (Vobj (bbox->i2).z < min_bbox.z)
    min_bbox.z = Vobj (bbox->i2).z;
  else if (Vobj (bbox->i2).z > max_bbox.z)
    max_bbox.z = Vobj (bbox->i2).z;
  if (Vobj (bbox->i3).x < min_bbox.x)
    min_bbox.x = Vobj (bbox->i3).x;
  else if (Vobj (bbox->i3).x > max_bbox.x)
    max_bbox.x = Vobj (bbox->i3).x;
  if (Vobj (bbox->i3).y < min_bbox.y)
    min_bbox.y = Vobj (bbox->i3).y;
  else if (Vobj (bbox->i3).y > max_bbox.y)
    max_bbox.y = Vobj (bbox->i3).y;
  if (Vobj (bbox->i3).z < min_bbox.z)
    min_bbox.z = Vobj (bbox->i3).z;
  else if (Vobj (bbox->i3).z > max_bbox.z)
    max_bbox.z = Vobj (bbox->i3).z;
  if (Vobj (bbox->i4).x < min_bbox.x)
    min_bbox.x = Vobj (bbox->i4).x;
  else if (Vobj (bbox->i4).x > max_bbox.x)
    max_bbox.x = Vobj (bbox->i4).x;
  if (Vobj (bbox->i4).y < min_bbox.y)
    min_bbox.y = Vobj (bbox->i4).y;
  else if (Vobj (bbox->i4).y > max_bbox.y)
    max_bbox.y = Vobj (bbox->i4).y;
  if (Vobj (bbox->i4).z < min_bbox.z)
    min_bbox.z = Vobj (bbox->i4).z;
  else if (Vobj (bbox->i4).z > max_bbox.z)
    max_bbox.z = Vobj (bbox->i4).z;
  if (Vobj (bbox->i5).x < min_bbox.x)
    min_bbox.x = Vobj (bbox->i5).x;
  else if (Vobj (bbox->i5).x > max_bbox.x)
    max_bbox.x = Vobj (bbox->i5).x;
  if (Vobj (bbox->i5).y < min_bbox.y)
    min_bbox.y = Vobj (bbox->i5).y;
  else if (Vobj (bbox->i5).y > max_bbox.y)
    max_bbox.y = Vobj (bbox->i5).y;
  if (Vobj (bbox->i5).z < min_bbox.z)
    min_bbox.z = Vobj (bbox->i5).z;
  else if (Vobj (bbox->i5).z > max_bbox.z)
    max_bbox.z = Vobj (bbox->i5).z;
  if (Vobj (bbox->i6).x < min_bbox.x)
    min_bbox.x = Vobj (bbox->i6).x;
  else if (Vobj (bbox->i6).x > max_bbox.x)
    max_bbox.x = Vobj (bbox->i6).x;
  if (Vobj (bbox->i6).y < min_bbox.y)
    min_bbox.y = Vobj (bbox->i6).y;
  else if (Vobj (bbox->i6).y > max_bbox.y)
    max_bbox.y = Vobj (bbox->i6).y;
  if (Vobj (bbox->i6).z < min_bbox.z)
    min_bbox.z = Vobj (bbox->i6).z;
  else if (Vobj (bbox->i6).z > max_bbox.z)
    max_bbox.z = Vobj (bbox->i6).z;
  if (Vobj (bbox->i7).x < min_bbox.x)
    min_bbox.x = Vobj (bbox->i7).x;
  else if (Vobj (bbox->i7).x > max_bbox.x)
    max_bbox.x = Vobj (bbox->i7).x;
  if (Vobj (bbox->i7).y < min_bbox.y)
    min_bbox.y = Vobj (bbox->i7).y;
  else if (Vobj (bbox->i7).y > max_bbox.y)
    max_bbox.y = Vobj (bbox->i7).y;
  if (Vobj (bbox->i7).z < min_bbox.z)
    min_bbox.z = Vobj (bbox->i7).z;
  else if (Vobj (bbox->i7).z > max_bbox.z)
    max_bbox.z = Vobj (bbox->i7).z;
  if (Vobj (bbox->i8).x < min_bbox.x)
    min_bbox.x = Vobj (bbox->i8).x;
  else if (Vobj (bbox->i8).x > max_bbox.x)
    max_bbox.x = Vobj (bbox->i8).x;
  if (Vobj (bbox->i8).y < min_bbox.y)
    min_bbox.y = Vobj (bbox->i8).y;
  else if (Vobj (bbox->i8).y > max_bbox.y)
    max_bbox.y = Vobj (bbox->i8).y;
  if (Vobj (bbox->i8).z < min_bbox.z)
    min_bbox.z = Vobj (bbox->i8).z;
  else if (Vobj (bbox->i8).z > max_bbox.z)
    max_bbox.z = Vobj (bbox->i8).z;
  obj_bbox.Set (min_bbox, max_bbox);
  box = obj_bbox;
  obj_radius = (max_bbox - min_bbox) * 0.5f;
  max_obj_radius = qsqrt (csSquaredDist::PointPoint (max_bbox, min_bbox)) * 0.5f;
}

void csThing::GetBoundingBox (iMovable *movable, csBox3 &box)
{
  if (wor_bbox_movablenr != movable->GetUpdateNumber ())
  {
    // First make sure obj_bbox is valid.
    GetBoundingBox (box);
    wor_bbox_movablenr = movable->GetUpdateNumber ();

    // @@@ Maybe it would be better to really calculate the bounding box

    // here instead of just transforming the object space bounding box?
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

  box = wor_bbox;
}

//-------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csThing::PolyMeshLOD)
  SCF_IMPLEMENTS_INTERFACE(iPolygonMesh)
SCF_IMPLEMENT_IBASE_END

csThing::PolyMeshLOD::PolyMeshLOD ()
{
  SCF_CONSTRUCT_IBASE (NULL);
}

//-------------------------------------------------------------------------

void PolyMeshHelper::Setup ()
{
  if (polygons || alloc_vertices)
  {
    // Already set up. First we check if the object vertex array
    // is still valid (if it is not a copy).
    if (alloc_vertices) return ;
    if (vertices == thing->obj_verts) return ;
  }

  vertices = NULL;

  // Count the number of needed polygons and vertices.
  num_verts = thing->GetVertexCount ();
  num_poly = 0;

  int i, j;
  const csPolygonArray &pol = thing->polygons;
  for (i = 0; i < thing->GetPolygonCount (); i++)
  {
    csPolygon3D *p = pol.Get (i);
    if (!p->GetUnsplitPolygon () && p->flags.Check (CS_POLY_COLLDET))
      num_poly++;
  }

  // Check curves.
  for (i = 0; i < thing->GetCurveCount (); i++)
  {
    csCurve *c = thing->curves.Get (i);
    csCurveTesselated *tess = c->Tesselate (1000);    // @@@ High quality?
    num_poly += tess->GetTriangleCount ();
    num_verts += tess->GetVertexCount ();
  }

  // Allocate the arrays and the copy the data.
  if (num_verts)
  {
    // If there are no curves we don't need to copy vertex data.
    if (thing->GetCurveCount () == 0)
    {
      vertices = thing->obj_verts;
    }
    else
    {
      alloc_vertices = new csVector3[num_verts];
      vertices = alloc_vertices;

      // Copy the polygon vertices.
      // Set num_verts to the number of vertices in polygon set so
      // that we can continue copying vertices from curves.
      num_verts = thing->GetVertexCount ();
      if (num_verts)
      {
        memcpy (
          vertices,
          thing->obj_verts,
          sizeof (csVector3) * num_verts);
      }
    }
  }

  if (num_poly)
  {
    polygons = new csMeshedPolygon[num_poly];
    num_poly = 0;
    for (i = 0; i < thing->GetPolygonCount (); i++)
    {
      csPolygon3D *p = pol.Get (i);
      if (!p->GetUnsplitPolygon () && p->flags.Check (CS_POLY_COLLDET))
      {
        polygons[num_poly].num_vertices = p->GetVertexCount ();
        polygons[num_poly].vertices = p->GetVertexIndices ();
        num_poly++;
      }
    }

    // Indicate that polygons after and including this index need to
    // have their 'vertices' array cleaned up. These polygons were generated
    // from curves.
    curve_poly_start = num_poly;
#ifndef CS_USE_NEW_RENDERER
    for (i = 0; i < thing->GetCurveCount (); i++)
    {
      csCurve *c = thing->curves.Get (i);
      csCurveTesselated *tess = c->Tesselate (1000);  // @@@ High quality?
      csTriangle *tris = tess->GetTriangles ();
      int tri_count = tess->GetTriangleCount ();
      for (j = 0; j < tri_count; j++)
      {
        polygons[num_poly].num_vertices = 3;
        polygons[num_poly].vertices = new int[3];

        // Adjust indices to skip the original polygon set vertices and

        // preceeding curves.
        polygons[num_poly].vertices[0] = tris[j].a + num_verts;
        polygons[num_poly].vertices[1] = tris[j].b + num_verts;
        polygons[num_poly].vertices[2] = tris[j].c + num_verts;
        num_poly++;
      }

      csVector3 *vts = tess->GetVertices ();
      int num_vt = tess->GetVertexCount ();
      memcpy (vertices + num_verts, vts, sizeof (csVector3) * num_vt);
      num_verts += num_vt;
    }
#endif // CS_USE_NEW_RENDERER
  }
}

void PolyMeshHelper::Cleanup ()
{
  int i;

  // Delete all polygons which were generated from curved surfaces.
  // The other polygons just have a reference to the original polygons
  // from the parent.
  if (polygons)
  {
    for (i = curve_poly_start; i < num_poly; i++)
    {
      delete[] polygons[i].vertices;
    }

    delete[] polygons;
    polygons = NULL;
  }

  delete[] alloc_vertices;
  alloc_vertices = NULL;
  vertices = NULL;
}

//-------------------------------------------------------------------------

void csThing::SetConvex (bool c)
{
  flags.Set (CS_ENTITY_CONVEX, c ? CS_ENTITY_CONVEX : 0);
  if (c)
  {
    if (center_idx == -1) center_idx = AddVertex (0, 0, 0);

    int i;
    float minx = 1000000000., miny = 1000000000., minz = 1000000000.;
    float maxx = -1000000000., maxy = -1000000000., maxz = -1000000000.;
    for (i = 0; i < num_vertices; i++)
    {
      if (i != center_idx)
      {
        if (obj_verts[i].x < minx) minx = obj_verts[i].x;
        if (obj_verts[i].x > maxx) maxx = obj_verts[i].x;
        if (obj_verts[i].y < miny) miny = obj_verts[i].y;
        if (obj_verts[i].y > maxy) maxy = obj_verts[i].y;
        if (obj_verts[i].z < minz) minz = obj_verts[i].z;
        if (obj_verts[i].z > maxz) maxz = obj_verts[i].z;
      }
    }

    obj_verts[center_idx].Set (
        (minx + maxx) / 2,
        (miny + maxy) / 2,
        (minz + maxz) / 2);
    if (cfg_moving == CS_THING_MOVE_OCCASIONAL)
      wor_verts[center_idx].Set (
          (minx + maxx) / 2,
          (miny + maxy) / 2,
          (minz + maxz) / 2);
  }
}

void csThing::UpdateCurveTransform (const csReversibleTransform &movtrans)
{
  if (GetCurveCount () == 0) return ;

  // since obj has changed (possibly) we need to tell all of our curves
  csReversibleTransform o2w = movtrans.GetInverse ();
  int i;
  for (i = 0; i < GetCurveCount (); i++)
  {
    csCurve *c = curves.Get (i);
    c->SetObject2World (&o2w);
  }
}

void csThing::UpdateCurveTransform ()
{
  if (curves_transf_ok) return ;
  curves_transf_ok = true;
  if (GetCurveCount () == 0) return ;

  csReversibleTransform o2w;                    // Identity transform.
  int i;
  for (i = 0; i < GetCurveCount (); i++)
  {
    csCurve *c = curves.Get (i);
    c->SetObject2World (&o2w);
  }
}

csPolygon3D *csThing::IntersectSphere (
  csVector3 &center,
  float radius,
  float *pr)
{
  float d, min_d = radius;
  int i;
  csPolygon3D *p, *min_p = NULL;
  csVector3 hit;

  for (i = 0; i < polygons.Length (); i++)
  {
    p = GetPolygon3D (i);

    const csPlane3 &wpl = p->GetPlane ()->GetObjectPlane ();
    d = wpl.Distance (center);
    if (d < min_d && csMath3::Visible (center, wpl))
    {
      hit = -center;
      hit -= wpl.Normal ();
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
#ifndef CS_USE_NEW_RENDERER
CS_TYPEDEF_GROWING_ARRAY (engine3d_StaticFogVerts, G3DFogInfo);
CS_IMPLEMENT_STATIC_VAR (GetStaticFogVerts, engine3d_StaticFogVerts,())

bool csThing::DrawCurves (
  iRenderView *rview,
  iMovable *movable,
  csZBufMode zMode)
{
  static engine3d_StaticFogVerts &fog_verts = *GetStaticFogVerts ();
  if (GetCurveCount () <= 0) return false;

  iCamera *icam = rview->GetCamera ();
  const csReversibleTransform &camtrans = icam->GetTransform ();

  csReversibleTransform movtrans;

  // Only get the transformation if this thing can move.
  bool can_move = false;
  if (movable && cfg_moving != CS_THING_MOVE_NEVER)
  {
    movtrans = movable->GetFullTransform ();
    can_move = true;
  }

  int i;
  int res = 1;

  // Calculate tesselation resolution
  csVector3 wv = curves_center;
  csVector3 world_coord;
  if (can_move)
    world_coord = movtrans.This2Other (wv);
  else
    world_coord = wv;

  csVector3 camera_coord = camtrans.Other2This (world_coord);

  if (camera_coord.z >= SMALL_Z)
  {
    res = (int)(curves_scale / camera_coord.z);
  }
  else
    res = 1000;                                 // some big tesselation value...

  // Create the combined transform of object to camera by
  // combining object to world and world to camera.
  csReversibleTransform obj_cam = camtrans;
  if (can_move) obj_cam /= movtrans;
  rview->GetGraphics3D ()->SetObjectToCamera (&obj_cam);
  rview->GetGraphics3D ()->SetRenderState (G3DRENDERSTATE_ZBUFFERMODE, zMode);

  // Base of the mesh.
  G3DTriangleMesh mesh;
  mesh.morph_factor = 0;
  mesh.num_vertices_pool = 1;
  mesh.do_mirror = icam->IsMirrored ();
  mesh.do_morph_texels = false;
  mesh.do_morph_colors = false;
  mesh.vertex_mode = G3DTriangleMesh::VM_WORLDSPACE;

  iVertexBufferManager *vbufmgr = rview->GetGraphics3D ()
    ->GetVertexBufferManager ();

  // Loop over all curves
  csCurve *c;
  for (i = 0; i < GetCurveCount (); i++)
  {
    c = curves.Get (i);

    // First get a bounding box in camera space.
    csBox3 cbox;
    csBox2 sbox;
    if (c->GetScreenBoundingBox (obj_cam, icam, cbox, sbox) < 0)
      continue;                                 // Not visible.
    int clip_portal, clip_plane, clip_z_plane;
    if (!rview->ClipBBox (sbox, cbox, clip_portal, clip_plane, clip_z_plane))
      continue;                                 // Not visible.

    // If we have a dirty lightmap recombine the curves and the shadow maps.
    bool updated_lm = c->RecalculateDynamicLights ();

    // Create a new tesselation reuse an old one.
    csCurveTesselated *tess = c->Tesselate (res);

    // If the lightmap was updated or the new tesselation doesn't yet
    // have a valid colors table we need to update colors here.
    if (updated_lm || !tess->AreColorsValid ())
      tess->UpdateColors (c->LightMap);

    // Setup the structure for DrawTriangleMesh.
    if (tess->GetVertexCount () > fog_verts.Limit ())
    {
      fog_verts.SetLimit (tess->GetVertexCount ());
    }

    c->GetMaterial ()->Visit ();

    iVertexBuffer *vbuf = c->GetVertexBuffer ();

    mesh.mat_handle = c->GetMaterialHandle ();
    mesh.buffers[0] = vbuf;
    mesh.num_triangles = tess->GetTriangleCount ();
    mesh.triangles = tess->GetTriangles ();
    mesh.clip_portal = clip_portal;
    mesh.clip_plane = clip_plane;
    mesh.clip_z_plane = clip_z_plane;
    mesh.vertex_fog = fog_verts.GetArray ();

    bool gouraud = !!c->LightMap;
    mesh.mixmode = CS_FX_COPY | (gouraud ? CS_FX_GOURAUD : 0);
    mesh.use_vertex_color = gouraud;
    if (mesh.mat_handle == NULL)
    {
      csEngine::current_engine->Warn ("Warning! Curve without material!");
      continue;
    }

    CS_ASSERT (!vbuf->IsLocked ());
    vbufmgr->LockBuffer (
        vbuf,
        tess->GetVertices (),
        tess->GetTxtCoords (),
        tess->GetColors (),
        tess->GetVertexCount (),
        0);
    rview->CalculateFogMesh (obj_cam, mesh);
    rview->GetGraphics3D ()->DrawTriangleMesh (mesh);
    vbufmgr->UnlockBuffer (vbuf);
  }

  return true;                                  //@@@ RETURN correct vis info
}
#endif // CS_USE_NEW_RENDERER

void csThing::FireListeners ()
{
  int i;
  for (i = 0 ; i < listeners.Length () ; i++)
    listeners[i]->ObjectModelChanged (&scfiObjectModel);
}

void csThing::AddListener (iObjectModelListener *listener)
{
  RemoveListener (listener);
  listeners.Push (listener);
}

void csThing::RemoveListener (iObjectModelListener *listener)
{
  int idx = listeners.Find (listener);
  if (idx == -1) return ;
  listeners.Delete (idx);
}

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

#if 1
  csBox3 b;
  GetBoundingBox (b);

  csSphere sphere;
  sphere.SetCenter (b.GetCenter ());
  sphere.SetRadius (max_obj_radius);
  if (can_move)
  {
    csReversibleTransform tr_o2c = camtrans / movable->GetFullTransform ();
    bool rc = rview->TestBSphere (tr_o2c, sphere);
    return rc;
  }
  else
  {
    bool rc = rview->TestBSphere (camtrans, sphere);
    return rc;
  }

#else
  csBox3 b;
  GetBoundingBox (b);

  csVector3 center = b.GetCenter ();
  float maxradius = max_obj_radius;
  if (can_move) center = movable->GetFullTransform ().This2Other (center);
  center = camtrans.Other2This (center);
  if (center.z + maxradius < 0) return false;

  //-----
  // If the camera location (origin in camera space) is inside the
  // bounding sphere then the object is certainly visible.
  //-----
  if (
    ABS (center.x) <= maxradius &&
    ABS (center.y) <= maxradius &&
    ABS (center.z) <= maxradius)
    return true;

  //-----
  // Also do frustum checking.
  //-----
  float lx, rx, ty, by;
  rview->GetFrustum (lx, rx, ty, by);
  lx *= center.z;
  if (center.x + maxradius < lx) return false;
  rx *= center.z;
  if (center.x - maxradius > rx) return false;
  ty *= center.z;
  if (center.y + maxradius < ty) return false;
  by *= center.z;
  if (center.y - maxradius > by) return false;

  return true;
#endif
}

CS_IMPLEMENT_STATIC_VAR (GetCullOctreeNodePerspPoly,csPolygon2D,())

// @@@ This routine need to be cleaned up!!! It probably needs to

// be part of the class.
bool CullOctreeNode (
  csPolygonTree *tree,
  csPolygonTreeNode *node,
  const csVector3 &pos,
  void *data)
{
  if (!node) return false;
  if (node->Type () != NODE_OCTREE) return true;

  bool vis = true;

  int i;
  csOctree *otree = (csOctree *)tree;
  csOctreeNode *onode = (csOctreeNode *)node;

  csCBuffer *c_buffer;
  iRenderView *rview = (iRenderView *)data;
  static csPolygon2D &persp = *GetCullOctreeNodePerspPoly ();
  csVector3 array[7];
  csEngine *w = csEngine::current_engine;
  iCamera *icam = rview->GetCamera ();
  const csReversibleTransform &camtrans = icam->GetTransform ();

  //if (w->IsPVS ())
  //{
  // Test for PVS.
  //if (!onode->IsVisible ()) { vis = false; goto the_end; }
  //else if (w->IsPVSOnly ()) goto is_vis;
  //}
  c_buffer = w->GetCBuffer ();

  int num_array;
  csPlane3 *far_plane = icam->GetFarPlane ();
  otree->GetConvexOutline (onode, pos, array, num_array, far_plane != NULL);

  if (num_array)
  {
    // If we use a farplane we could have up to 7 corners,
    // but the 7th we'll need not here.
    int nVert = MIN (6, num_array);

    csVector3 cam[7];

    // If all vertices are behind z plane then the node is
    // not visible. If some vertices are behind z plane then we
    // need to clip the polygon.
    // We also test the node against the view frustum here.
    int num_z_0 = 0;
    bool left = true, right = true, top = true, bot = true;
    float lx, rx, ty, by;
    rview->GetFrustum (lx, rx, ty, by);
    for (i = 0; i < nVert; i++)
    {
      cam[i] = camtrans.Other2This (array[i]);
      if (cam[i].z < SMALL_EPSILON) num_z_0++;
      if (left && cam[i].x >= cam[i].z * lx) left = false;
      if (right && cam[i].x <= cam[i].z * rx) right = false;
      if (top && cam[i].y >= cam[i].z * ty) top = false;
      if (bot && cam[i].y <= cam[i].z * by) bot = false;
    }

    if (left || right || top || bot)
    {
      vis = false;
      goto the_end;
    }

    if (num_z_0 == nVert)
    {
      // Node behind camera.
      vis = false;
      goto the_end;
    }

    if (far_plane)
    {
      if (num_array == 7)                 // we havent transformed the 7th yet
        cam[6] = camtrans.Other2This (array[6]);
      for (i = 0; i < num_array; i++)
        if (far_plane->Classify (cam[i]) > SMALL_EPSILON) break;
      if (i == num_array)
      {
	vis = false;
        goto the_end;
      }
    }

    persp.MakeEmpty ();
    if (num_z_0 == 0)
    {
      // No vertices are behind. Just perspective correct.
      for (i = 0; i < nVert; i++) persp.AddPerspective (cam[i]);
    }
    else
    {
      // Some vertices are behind. We need to clip.
      csVector3 isect;
      int i1 = nVert - 1;
      for (i = 0; i < nVert; i++)
      {
        if (cam[i].z < SMALL_EPSILON)
        {
          if (cam[i1].z < SMALL_EPSILON)
          {
            // Just skip vertex.
          }
          else
          {
            // We need to intersect and add intersection point.
            csIntersect3::ZPlane (SMALL_EPSILON, cam[i], cam[i1], isect);
            persp.AddPerspective (isect);
          }
        }
        else
        {
          if (cam[i1].z < SMALL_EPSILON)
          {
            // We need to intersect and add both intersection point and
	    // this point.
            csIntersect3::ZPlane (SMALL_EPSILON, cam[i], cam[i1], isect);
            persp.AddPerspective (isect);
          }

          // Just perspective correct and add to the 2D polygon.
          persp.AddPerspective (cam[i]);
        }

        i1 = i;
      }
    }

    if (!persp.ClipAgainst (rview->GetClipper ()))
    {
      vis = false;
      goto the_end;
    }

    // c-buffer test.
    vis = c_buffer->TestPolygon (
          persp.GetVertices (),
          persp.GetVertexCount ());
    if (!vis) goto the_end;
  }

  // If a node is visible we check wether or not it has a minibsp.
  // If it has a minibsp then we need to transform all vertices used
  // by that minibsp to camera space.
  csVector3 *cam;
  int *indices;
  int num_indices;
  if (onode->GetMiniBspVerts ())
  {
    // Here we get the polygon set as the static thing from the sector itself.
    csThing *pset = otree->GetThing ();
    cam = pset->GetCameraVertices (camtrans, icam->GetCameraNumber ());
    indices = onode->GetMiniBspVerts ();
    num_indices = onode->GetMiniBspVertexCount ();
    for (i = 0; i < num_indices; i++)
      cam[indices[i]] = camtrans.Other2This (pset->Vwor (indices[i]));
  }

the_end:
# ifdef CS_DEBUG
  if (viscnt_enabled)
  {
    if (vis) viscnt_vis_node++;
    else
    {
      viscnt_invis_node++;
      const csBox3& b = onode->GetBox ();
      float volume = (b.MaxX ()-b.MinX ()) * (b.MaxY ()-b.MinY ()) *
		(b.MaxZ ()-b.MinZ ());
      viscnt_invis_node_vol += volume;
    }
  }
# endif

  return vis;
}

/**
 * This is data that is created by the visibility culler
 * and registered to the current recursion level (as render
 * context data). The data contains the queue of all visible
 * polygons. Since visibility testing and drawing is now seperate
 * we need to remember everything in a queue. Since it is also
 * possible that between visibility testing and drawing other
 * objects (or even this thing again!) can be called we have to
 * attach this queue to the current recursion level.
 */
struct csPolygonVisInfo :
  public iBase
{
  uint32 current_visnr;
  csPolygon2DQueue *poly_queue;
  csPolygonVisInfo (int num);
  virtual~csPolygonVisInfo()
  {
    delete poly_queue;
  }
  SCF_DECLARE_IBASE;
};

SCF_IMPLEMENT_IBASE(csPolygonVisInfo)
  SCF_IMPLEMENTS_INTERFACE(iBase)
SCF_IMPLEMENT_IBASE_END

csPolygonVisInfo::csPolygonVisInfo (int num)
{
  SCF_CONSTRUCT_IBASE (NULL);
  poly_queue = new csPolygon2DQueue (num);
}

void *csThing::TestQueuePolygons (
  csThing *thing,
  csPolygonInt **polygon,
  int num,
  bool /*same_plane*/,
  void *data)
{
  iRenderView *d = (iRenderView *)data;
  csPolygonVisInfo *pvi = (csPolygonVisInfo *)
    (d->FindRenderContextData ((void *)thing));
  if (!pvi)
  {
    csEngine::current_engine->ReportBug ("INTERNAL ERROR! polygon queue is missing!!!");
    fflush (stdout);
    DEBUG_BREAK;
  }

  return csThing::TestQueuePolygonArray (
      polygon,
      num,
      d,
      pvi->poly_queue,
      d->GetEngine ()->IsPVS (),
      pvi->current_visnr);
}

void csThing::DrawPolygonsFromQueue (
  csPolygon2DQueue *queue,
  iRenderView *rview)
{
#ifndef CS_USE_NEW_RENDERER
  csPolygon3D *poly3d;
  csPolygon2D *poly2d;
  csPoly2DPool *render_pool = csEngine::current_engine->render_pol2d_pool;
  const csReversibleTransform &camtrans = rview->GetCamera ()->GetTransform ();
  while (queue->Pop (&poly3d, &poly2d))
  {
    poly3d->UpdateTransformation (
        camtrans,
        rview->GetCamera ()->GetCameraNumber ());
    poly3d->GetPlane ()->WorldToCamera (camtrans, poly3d->Vcam (0));
    DrawOnePolygon (poly3d, poly2d, rview, CS_ZBUF_FILL);
    render_pool->Free (poly2d);
  }
#endif // CS_USE_NEW_RENDERER
}

void *csThing::DrawPolygons (
  csThing * /*thing*/,
  csPolygonInt **polygon,
  int num,
  bool /*same_plane*/,
  void *data)
{
#ifndef CS_USE_NEW_RENDERER
  iRenderView *d = (iRenderView *)data;
  csThing::DrawPolygonArray (polygon, num, d, CS_ZBUF_FILL);
#endif // CS_USE_NEW_RENDERER
  return NULL;
}

bool csThing::Draw (iRenderView *rview, iMovable *movable, csZBufMode zMode)
{
#ifndef CS_USE_NEW_RENDERER
  iCamera *icam = rview->GetCamera ();
  const csReversibleTransform &camtrans = icam->GetTransform ();

  draw_busy++;

  // If this thing has a static tree we follow a totally different procedure
  // to draw it.
  if (static_tree)
  {
#   ifdef CS_DEBUG
    if (viscnt_enabled)
    {
      csOctree* otree = (csOctree*)static_tree;
      const csBox3& b = otree->GetRoot ()->GetBox ();
      viscnt_vis_node_vol = (b.MaxX ()-b.MinX ()) * (b.MaxY ()-b.MinY ()) *
		(b.MaxZ ()-b.MinZ ());
    }
#   endif

    int engine_mode = csEngine::current_engine->GetEngineMode ();

    // If this thing has a static polygon tree (octree) there
    // are three possibilities.
    if (engine_mode == CS_ENGINE_FRONT2BACK)
    {
      csPolygonVisInfo *pvi = (csPolygonVisInfo *)
        (rview->FindRenderContextData ((void *)this));
      if (!pvi)
      {
        csEngine::current_engine->ReportBug (
		"INTERNAL ERROR! polygon queue is missing!!!");
        fflush (stdout);
        DEBUG_BREAK;
      }

      // Render all polygons that are visible back to front.
      DrawPolygonsFromQueue (pvi->poly_queue, rview);
      rview->DeleteRenderContextData ((void *)this);
    }
    else if (engine_mode == CS_ENGINE_BACK2FRONT)
    {
      //-----
      // Here we don't use the c-buffer or 2D culler but just render back
      // to front.
      //-----
      static_tree->Back2Front (
          camtrans.GetOrigin (),
          &DrawPolygons,
          (void *)rview);
    }
    else
    {
      //-----
      // Here we render using the Z-buffer.
      //-----
      csOctree *otree = (csOctree *)static_tree;
      csPolygonIntArray &unsplit = otree->GetRoot ()->GetUnsplitPolygons ();
      DrawPolygonArray (
        unsplit.GetPolygons (),
        unsplit.GetPolygonCount (),
        rview,
        CS_ZBUF_USE);
    }
  }
  else
  {
    // This thing has no static tree
    Stats::polygons_considered += polygons.Length ();
    DrawCurves (rview, movable, zMode);

    if (flags.Check (CS_THING_FASTMESH))
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
        rview,
        zMode);
    }
  }

  draw_busy--;
#endif // CS_USE_NEW_RENDERER
  return true;                                  // @@@@ RETURN correct vis info
}

#ifndef CS_USE_NEW_RENDERER
bool csThing::DrawFoggy (iRenderView *d, iMovable *)
{
  draw_busy++;

  iCamera *icam = d->GetCamera ();
  const csReversibleTransform &camtrans = icam->GetTransform ();
  UpdateTransformation (camtrans, icam->GetCameraNumber ());

  csPolygon3D *p;
  csVector3 *verts;
  int num_verts;
  int i;
  csPoly2DPool *render_pool = csEngine::current_engine->render_pol2d_pool;
  csPolygon2D *clip;

  // @@@ Wouldn't it be nice if we checked all vertices against the Z plane?
  {
    csVector2 orig_triangle[3];
    d->GetGraphics3D ()->OpenFogObject (GetID (), &GetFog ());
    Stats::polygons_considered += polygons.Length ();

    icam->SetMirrored (!icam->IsMirrored ());
    for (i = 0; i < polygons.Length (); i++)
    {
      p = GetPolygon3D (i);
      if (p->flags.Check (CS_POLY_NO_DRAW)) continue;
      clip = (csPolygon2D *) (render_pool->Alloc ());

      const csPlane3 &wplane = p->GetPlane ()->GetWorldPlane ();
      bool front = csMath3::Visible (camtrans.GetOrigin (), wplane);

      csPlane3 clip_plane, *pclip_plane;
      bool do_clip_plane = d->GetClipPlane (clip_plane);
      if (do_clip_plane)
        pclip_plane = &clip_plane;
      else
        pclip_plane = NULL;
      if (
        !front &&
        p->ClipToPlane (
            pclip_plane,
            camtrans.GetOrigin (),
            verts,
            num_verts,
            false) &&
        p->DoPerspective (
            camtrans,
            verts,
            num_verts,
            clip,
            orig_triangle,
            icam->IsMirrored ()) &&
        clip->ClipAgainst (d->GetClipper ()))
      {
        p->GetPlane ()->WorldToCamera (camtrans, verts[0]);

        Stats::polygons_drawn++;

        clip->AddFogPolygon (
            d->GetGraphics3D (),
            p,
            p->GetPlane (),
            icam->IsMirrored (),
            GetID (),
            CS_FOG_BACK);
      }

      render_pool->Free (clip);
    }

    icam->SetMirrored (!icam->IsMirrored ());
    for (i = 0; i < polygons.Length (); i++)
    {
      p = GetPolygon3D (i);
      if (p->flags.Check (CS_POLY_NO_DRAW)) continue;
      clip = (csPolygon2D *) (render_pool->Alloc ());

      const csPlane3 &wplane = p->GetPlane ()->GetWorldPlane ();
      bool front = csMath3::Visible (camtrans.GetOrigin (), wplane);

      csPlane3 clip_plane, *pclip_plane;
      bool do_clip_plane = d->GetClipPlane (clip_plane);
      if (do_clip_plane)
        pclip_plane = &clip_plane;
      else
        pclip_plane = NULL;
      if (
        front &&
        p->ClipToPlane (
            pclip_plane,
            camtrans.GetOrigin (),
            verts,
            num_verts,
            true) &&
        p->DoPerspective (
            camtrans,
            verts,
            num_verts,
            clip,
            orig_triangle,
            icam->IsMirrored ()) &&
        clip->ClipAgainst (d->GetClipper ()))
      {
        p->GetPlane ()->WorldToCamera (camtrans, verts[0]);

        Stats::polygons_drawn++;

        clip->AddFogPolygon (
            d->GetGraphics3D (),
            p,
            p->GetPlane (),
            icam->IsMirrored (),
            GetID (),
            CS_FOG_FRONT);
      }

      render_pool->Free (clip);
    }

    d->GetGraphics3D ()->CloseFogObject (GetID ());
  }

  draw_busy--;
  return true;                                  // @@@@ RETURN correct vis info
}
#endif // CS_USE_NEW_RENDERER

void csThing::RegisterVisObject (iVisibilityObject *visobj)
{
  csVisObjInfo *vinf = new csVisObjInfo ();
  vinf->visobj = visobj;

  csRef<iShadowCaster> shadcast (SCF_QUERY_INTERFACE (visobj, iShadowCaster));
  vinf->shadcast = shadcast;
  vinf->bbox = new csPolyTreeBBox ();

  // Initialize the movable and shape numbers with a number different
  // from the one that the objects currently have so that we know
  // we will add the object to the tree when we first need it.
  vinf->last_movablenr = visobj->GetMovable ()->GetUpdateNumber () - 1;
  vinf->last_shapenr = visobj->GetObjectModel ()->GetShapeNumber () - 1;
  visobjects.Push (vinf);
  visobj->IncRef ();
}

void csThing::UnregisterVisObject (iVisibilityObject *visobj)
{
  int idx;
  csVisObjInfo *vinf = NULL;
  for (idx = 0; idx < visobjects.Length (); idx++)
  {
    vinf = (csVisObjInfo *)visobjects[idx];
    if (vinf->visobj == visobj) break;
    vinf = NULL;
  }

  if (vinf == NULL) return ;
  visobjects.Delete (idx);
  delete vinf->bbox;
  delete vinf;
  visobj->DecRef ();
}

void csThing::CheckVisUpdate (csVisObjInfo *vinf)
{
  iVisibilityObject *visobj = vinf->visobj;
  iMovable *movable = visobj->GetMovable ();
  long movablenr = movable->GetUpdateNumber ();
  long shapenr = visobj->GetObjectModel ()->GetShapeNumber ();
  if (movablenr != vinf->last_movablenr || shapenr != vinf->last_shapenr)
  {
    vinf->last_movablenr = movablenr;
    vinf->last_shapenr = shapenr;

    csPolyTreeBBox *bbox = vinf->bbox;

    // If object has not been placed in a sector we do nothing here.
    if (!movable->InSector ()) return ;

    csBox3 b;
    visobj->GetObjectModel ()->GetObjectBoundingBox (b, CS_BBOX_MAX);

    csReversibleTransform trans = movable->GetFullTransform ().GetInverse ();
    bbox->Update (b, trans, vinf);

    // Temporarily increase reference to prevent free.
    bbox->GetBaseStub ()->IncRef ();
    static_tree->AddObject (bbox);
    bbox->GetBaseStub ()->DecRef ();
  }
}

bool csThing::VisTest (iRenderView *irview)
{
  if (!static_tree) return false;
  current_visnr++;

  iEngine *iengine = irview->GetEngine ();
  iCamera *icam = irview->GetCamera ();
  int engine_mode = iengine->GetEngineMode ();

  // If this thing has a static polygon tree (octree) there
  // are three possibilities.
  if (engine_mode == CS_ENGINE_FRONT2BACK)
  {
    //-----
    // In this part of the rendering we use the c-buffer or another
    // 2D/3D visibility culler.
    //-----
    csOrthoTransform &camtrans = icam->GetTransform ();
    const csVector3 &origin = camtrans.GetOrigin ();

    int i;
    for (i = 0; i < visobjects.Length (); i++)
    {
      csVisObjInfo *vinf = (csVisObjInfo *)visobjects[i];

      // First update visibility information if object has moved or
      // changed shape fundamentally.
      CheckVisUpdate (vinf);

      iVisibilityObject *vo = vinf->visobj;

      csRef<iMeshWrapper> mw (SCF_QUERY_INTERFACE (vo, iMeshWrapper));
      if (mw->GetMeshObject () == &scfiMeshObject)
      {
        // If the object represents the object of the culler then
        // the object is automatically visible.
        vo->SetVisibilityNumber (current_visnr);
      }
      else
      {
        // If the current viewpoint is inside the bounding box of the
        // object then we consider the object visible.
        csBox3 bbox;
        vo->GetObjectModel ()->GetObjectBoundingBox (bbox, CS_BBOX_MAX);
        if (bbox.In (origin))
          vo->SetVisibilityNumber (current_visnr);
      }

      vinf->bbox->ClearTransform ();
    }

    // Using the PVS, mark all sectors and polygons that are visible
    // from the current node.
    //if (engine->IsPVS ())
    //{
    //csOctree* otree = (csOctree*)static_tree;
    //@@@if (engine->IsPVSFrozen ())
    //@@@otree->MarkVisibleFromPVS (engine->GetFrozenPosition ());
    //@@@else
    //otree->MarkVisibleFromPVS (origin);
    //}
    // Initialize a queue on which all visible polygons will be pushed.
    // The octree is traversed front to back but we want to render
    // back to front. That's one of the reasons for this queue.
    csPolygonVisInfo *pvi = new csPolygonVisInfo (GetPolygonCount ());
    pvi->current_visnr = current_visnr;
    irview->AttachRenderContextData ((void *)this, (iBase *)pvi);

    // Update the transformation for the static tree. This will
    // not actually transform all vertices from world to camera but
    // it will make sure that when a node (octree node) is visited,
    // the transformation will happen at that time.
    UpdateTransformation (camtrans, icam->GetCameraNumber ());

    // Traverse the tree front to back and push all visible polygons
    // on the queue. This traversal will also mark all visible
    // meshes and things. They will be put on a queue later.
    static_tree->Front2Back (
        origin,
        &TestQueuePolygons,
        irview,
        CullOctreeNode,
        irview);
    return true;
  }
  else if (engine_mode == CS_ENGINE_BACK2FRONT)
  {
    //-----
    // Here we don't use the c-buffer or 2D culler but just render back
    // to front.
    //-----
    UpdateTransformation (icam->GetTransform (), icam->GetCameraNumber ());
    return false;
  }
  else
  {
    //-----
    // Here we render using the Z-buffer.
    //-----
    UpdateTransformation (icam->GetTransform (), icam->GetCameraNumber ());
    return false;
  }
}

//----------------------------------------------------------------------

class csThingVisObjIt : public iVisibilityObjectIterator
{
private:
  csVector* vector;
  int position;
  bool* vistest_objects_inuse;

public:
  SCF_DECLARE_IBASE;

  csThingVisObjIt (csVector* vector, bool* vistest_objects_inuse)
  {
    SCF_CONSTRUCT_IBASE (NULL);
    csThingVisObjIt::vector = vector;
    csThingVisObjIt::vistest_objects_inuse = vistest_objects_inuse;
    if (vistest_objects_inuse) *vistest_objects_inuse = true;
    Reset ();
  }
  virtual ~csThingVisObjIt ()
  {
    // If the vistest_objects_inuse pointer is not NULL we set the
    // bool to false to indicate we're no longer using the base
    // vector. Otherwise we delete the vector.
    if (vistest_objects_inuse) *vistest_objects_inuse = false;
    else delete vector;
  }

  virtual bool Next()
  {
    if (position < 0) return false;
    position++;
    if (position == vector->Length ())
    {
      position = -1;
      return false;
    }
    return true;
  }

  virtual void Reset()
  {
    if (vector == NULL || vector->Length () < 1)
      position = -1;
    else
      position = 0;
  }

  virtual iVisibilityObject* GetObject () const
  {
    return (iVisibilityObject*)(vector->Get (position));
  }
  virtual bool IsFinished () const
  {
    return (position < 0);
  }
};

SCF_IMPLEMENT_IBASE (csThingVisObjIt)
  SCF_IMPLEMENTS_INTERFACE (iVisibilityObjectIterator)
SCF_IMPLEMENT_IBASE_END

//----------------------------------------------------------------------

csPtr<iVisibilityObjectIterator> csThing::VisTest (const csBox3& box)
{
  if (!static_tree) return NULL;
  current_visnr++;

  csVector* v;
  if (vistest_objects_inuse)
  {
    // Vector is already in use by another iterator. Allocate a new vector
    // here.
    v = new csVector ();
  }
  else
  {
    v = &vistest_objects;
    vistest_objects.DeleteAll ();
  }

  // @@@ Very ugly implementation. Should at least try
  // to use the octree!!!
  int i;
  for (i = 0; i < visobjects.Length (); i++)
  {
    csVisObjInfo *vinf = (csVisObjInfo *)visobjects[i];
    iVisibilityObject *vo = vinf->visobj;
    CheckVisUpdate (vinf);
    csPolyTreeBBox *pt_bbox = vinf->bbox;
    const csBox3& bbox = pt_bbox->GetWorldBoundingBox ();
    if (bbox.TestIntersect (box))
    {
      vo->SetVisibilityNumber (current_visnr);
      v->Push (vo);
    }
  }

  csThingVisObjIt* vobjit = new csThingVisObjIt (v,
  	vistest_objects_inuse ? NULL : &vistest_objects_inuse);
  return csPtr<iVisibilityObjectIterator> (vobjit);
}

csPtr<iVisibilityObjectIterator> csThing::VisTest (const csSphere& sphere)
{
  if (!static_tree) return NULL;
  current_visnr++;

  csVector* v;
  if (vistest_objects_inuse)
  {
    // Vector is already in use by another iterator. Allocate a new vector
    // here.
    v = new csVector ();
  }
  else
  {
    v = &vistest_objects;
    vistest_objects.DeleteAll ();
  }

  // @@@ Very ugly implementation. Should at least try
  // to use the octree!!!
  csVector3 pos = sphere.GetCenter ();
  float sqradius = sphere.GetRadius () * sphere.GetRadius ();
  int i;
  for (i = 0; i < visobjects.Length (); i++)
  {
    csVisObjInfo *vinf = (csVisObjInfo *)visobjects[i];
    iVisibilityObject *vo = vinf->visobj;
    CheckVisUpdate (vinf);
    csPolyTreeBBox *pt_bbox = vinf->bbox;
    const csBox3& bbox = pt_bbox->GetWorldBoundingBox ();
    if (csIntersect3::BoxSphere (bbox, pos, sqradius))
    {
      vo->SetVisibilityNumber (current_visnr);
      v->Push (vo);
    }
  }

  csThingVisObjIt* vobjit = new csThingVisObjIt (v,
  	vistest_objects_inuse ? NULL : &vistest_objects_inuse);
  return csPtr<iVisibilityObjectIterator> (vobjit);
}

struct CheckFrustData
{
  iFrustumView *fview;
  csHashSet visible_things;                     // Contains objects of type iMeshWrapper.
};

static int frust_cnt = 50;

//@@@ Needs to be part of sector?
//@@@ TODO Optimization!: sort shadow frustums with relative size
//as seen from light position. i.e. bigger shadow frustums come first.
//After that do the compression step. This should reduce a lot more
//of the shadow frustums.
static void CompressShadowFrustums (iShadowBlockList *list)
{
  iShadowIterator *shadow_it = list->GetShadowIterator (true);
  csFrustum *sf;
  if (!shadow_it->HasNext ())
  {
    shadow_it->DecRef ();
    return ;
  }

  csCBufferCube *cb = csEngine::current_engine->GetCBufCube ();
  cb->MakeEmpty ();

  while (shadow_it->HasNext ())
  {
    iShadowBlock *shadlist = shadow_it->GetNextShadowBlock ();
    sf = shadow_it->Next ();
    if (!list->FromCurrentRegion (shadlist))
      break;

    bool vis = cb->InsertPolygon (sf->GetVertices (), sf->GetVertexCount ());
    if (!vis) shadow_it->DeleteCurrent ();
  }

  shadow_it->DecRef ();
}

//@@@ Needs to be part of sector?
static void *CheckFrustumPolygonsFB (
  csThing *thing,
  csPolygonInt **polygon,
  int num,
  bool

  /*same_plane*/,
  void *data)
{
  iShadowCaster *ishadcast = &(thing->scfiShadowCaster);

  csPolygon3D *p;
  CheckFrustData *fdata = (CheckFrustData *)data;
  iFrustumView *fview = fdata->fview;
  csFrustumContext *ctxt = fview->GetFrustumContext ();
  iShadowBlockList *shadows = ctxt->GetShadows ();
  csVector3 &center = ctxt->GetLightFrustum ()->GetOrigin ();
  csCBufferCube *cb = csEngine::current_engine->GetCBufCube ();
  bool cw = true;                               // @@@ Mirror flag?
  int i, j;
  for (i = 0; i < num; i++)
  {
    csVector3 poly[128];                        // @@@ HARDCODED! BAD!
    if (polygon[i]->GetType () == 3)
    {
      // A BSP polygon. Used for testing visibility of things.
      csBspPolygon *bsppol = (csBspPolygon *)polygon[i];
      csVisObjInfo *obj = bsppol->GetOriginator ();

      // @@@ The code below is all not very nice. We should not assume
      // that only meshes can be used in vis info.
      csRef<iMeshWrapper> mesh (SCF_QUERY_INTERFACE (
          obj->visobj,
          iMeshWrapper));
      if (mesh)
      {
        if (!fdata->visible_things.In (mesh))
        {
          csRef<iShadowCaster> shadcast (SCF_QUERY_INTERFACE (
              mesh->GetMeshObject (), iShadowCaster));
          if (shadcast)
          {
            csPolyIndexed &pi = bsppol->GetPolygon ();
            csPolyTreeBBox *pi_par = bsppol->GetParent ();
            csVector3Array &verts = pi_par->GetVertices ();
            for (j = 0; j < pi.GetVertexCount (); j++)
              poly[j] = verts[pi[j]] - center;

            bool vis = cb->TestPolygon (poly, pi.GetVertexCount ());
            if (vis)
            {
              if (fview->ThingShadowsEnabled ())
              {
                // The thing is visible and we want things to cast
                // shadows. So we add all shadows generated by this
                // thing to the shadow list.
                if (shadcast != ishadcast)
                {
                  if ((!mesh->GetFlags ().Check (CS_ENTITY_CAMERA)) &&
                      fview->CheckShadowMask (mesh->GetFlags ().Get ()))
                    shadcast->AppendShadows (mesh->GetMovable (), shadows, center);
                }
              }

              fdata->visible_things.AddNoTest (mesh);
            }
          }
        }
      }

      continue;
    }

    if (polygon[i]->GetType () != 1) continue;
    p = (csPolygon3D *)polygon[i];

    // Polygons that are merged with the octree have world==obj space.
    for (j = 0; j < p->GetVertexCount (); j++)
      poly[j] = p->Vobj (j) - center;

    bool vis = false;

    float clas = p->GetPlane ()->GetWorldPlane ().Classify (center);
    if (ABS (clas) < EPSILON) continue;
    if ((clas <= 0) != cw) continue;

    if (p->GetPortal ())
    {
      vis = cb->TestPolygon (poly, p->GetVertexCount ());
    }
    else
    {
      vis = cb->InsertPolygon (poly, p->GetVertexCount ());
    }

    fview->CallPolygonFunction ((csObject *)p, vis);
    if (vis)
    {
      if (!shadows->GetLastShadowBlock ())
      {
        shadows->NewShadowBlock ();
      }

      csPlane3 pl = p->GetPlane ()->GetWorldPlane ();
      pl.DD += center * pl.norm;
      pl.Invert ();

      csFrustum *frust = shadows->GetLastShadowBlock ()->AddShadow (
          center,
          (void *)p,
          p->GetVertices ().GetVertexCount (),
          pl);

      // Polygons that are merged with the octree have world==obj space.
      for (j = 0; j < p->GetVertices ().GetVertexCount (); j++)
        frust->GetVertex (j).Set (poly[j]);
      frust_cnt--;
      if (frust_cnt < 0)
      {
        frust_cnt = 200;
        CompressShadowFrustums (shadows);
      }
    }
  }

  return NULL;
}

// @@@ This routine need to be cleaned up!!! It needs to
// be part of the class.
// @@@ This function needs to use the PVS. However, this function itself
// is used for the PVS so we need to take care!
static bool CullOctreeNodeLighting (
  csPolygonTree *tree,
  csPolygonTreeNode *node,
  const csVector3 &

  /*pos*/,
  void *data)
{
  if (!node) return false;
  if (node->Type () != NODE_OCTREE) return true;

  csOctree *otree = (csOctree *)tree;
  csOctreeNode *onode = (csOctreeNode *)node;
  iFrustumView *fview = (iFrustumView *)data;

  csFrustumContext *ctxt = fview->GetFrustumContext ();
  const csVector3 &center = ctxt->GetLightFrustum ()->GetOrigin ();
  csVector3 bmin = onode->GetMinCorner () - center;
  csVector3 bmax = onode->GetMaxCorner () - center;

  // Calculate the distance between (0,0,0) and the box.
  csVector3 result (0, 0, 0);
  if (bmin.x > 0)
    result.x = bmin.x;
  else if (bmax.x < 0)
    result.x = -bmax.x;
  if (bmin.y > 0)
    result.y = bmin.y;
  else if (bmax.y < 0)
    result.y = -bmax.y;
  if (bmin.z > 0)
    result.z = bmin.z;
  else if (bmax.z < 0)
    result.z = -bmax.z;

  float dist = result.Norm ();
  float radius = fview->GetRadius ();
  if (radius < dist)
  {
    return false;
  }

  if (ABS (dist) < EPSILON)
  {
    // We are in the node.
    fview->CallNodeFunction (onode, true);
    return true;
  }

  // Test node against quad-tree.
  csVector3 outline[6];
  int num_outline;
  otree->GetConvexOutline (onode, center, outline, num_outline);

  bool vis = true;
  if (num_outline > 0)
  {
    int i;
    for (i = 0; i < num_outline; i++) outline[i] -= center;

    csCBufferCube *cb = csEngine::current_engine->GetCBufCube ();
    if (cb && !cb->TestPolygon (outline, num_outline)) vis = false;
  }

  fview->CallNodeFunction (onode, vis);
  return vis;
}

void csThing::CastShadows (iFrustumView *fview)
{
  CS_ASSERT (static_tree != NULL);
  current_visnr++;

  iShadowBlockList *shadows = fview->GetFrustumContext ()->GetShadows ();
  // Mark a new region so that we can restore the shadows later.
  uint32 prev_region = shadows->MarkNewRegion ();

  // If there is a static tree (BSP and/or octree) then we
  // go front to back and add shadows to the list while we are doing
  // that. In future I would like to add some extra culling stage
  // here using a quad-tree or something similar (for example six
  // quad-trees arranged in a cube around the light).
  // First make sure the visibility objects are correctly put in the trees.
  int i;
  for (i = 0; i < visobjects.Length (); i++)
  {
    csVisObjInfo *vinf = (csVisObjInfo *)visobjects[i];

    // First update visibility information if object has moved or
    // changed shape fundamentally.
    CheckVisUpdate (vinf);
  }

  CheckFrustData fdata;
  fdata.fview = fview;

  // Translate this sector so that it is oriented around
  // the position of the light (position of the light becomes
  // the new origin).
  csVector3 &center = fview->GetFrustumContext ()->GetLightFrustum ()
    ->GetOrigin ();

  // All visible things will cause shadows to be added to 'lview'.
  // Later on we'll restore these shadows.
  frust_cnt = 50;
  static_tree->Front2Back (
      center,
      CheckFrustumPolygonsFB,
      (void *) &fdata,
      CullOctreeNodeLighting,
      (void *)fview);
  frust_cnt = 50;

  // Calculate lighting for all things in this sector.
  // The 'visible_things' hashset that is in fdata will contain
  // all things that are found visible while traversing the octree.
  // This queue is filled while traversing the octree
  // (CheckFrustumPolygonsFB).
  csHashIterator it (fdata.visible_things.GetHashMap ());
  iMeshWrapper *mesh;

  while (it.HasNext ())
  {
    mesh = (iMeshWrapper *) (it.Next ());
    // Only if the thing has right flags do we consider it for shadows.
    if (fview->CheckProcessMask (mesh->GetFlags ().Get ()))
    {
      csRef<iShadowReceiver> shadrcv (SCF_QUERY_INTERFACE (
        mesh->GetMeshObject (),
        iShadowReceiver));
      if (shadrcv)
      {
        // Skip doing lighting for the thing containing the culler itself.
        if (shadrcv != &(scfiShadowReceiver))
          shadrcv->CastShadows (mesh->GetMovable (), fview);
      }
    }
  }

  // Restore the shadow list in 'lview' and then delete
  // all the shadow frustums that were added in this recursion
  // level.
  while (shadows->GetLastShadowBlock ())
  {
    iShadowBlock *sh = shadows->GetLastShadowBlock ();
    if (!shadows->FromCurrentRegion (sh))
      break;
    shadows->RemoveLastShadowBlock ();
    sh->DecRef ();
  }
  shadows->RestoreRegion (prev_region);
}

void csThing::CheckFrustum (iFrustumView *lview, iMovable *movable)
{
  csCBufferCube *cb = csEngine::current_engine->GetCBufCube ();
  cb->MakeEmpty ();
  RealCheckFrustum (lview, movable);
}

void csThing::RealCheckFrustum (iFrustumView *lview, iMovable *movable)
{
  //@@@ Ok?
  cached_movable = movable;
  WorUpdate ();

  csPolygon3D *p;
  int i;

  draw_busy++;

  if (light_frame_number != current_light_frame_number)
  {
    light_frame_number = current_light_frame_number;
    lview->GetFrustumContext ()->SetFirstTime (true);
  }
  else
    lview->GetFrustumContext ()->SetFirstTime (false);

  for (i = 0; i < polygons.Length (); i++)
  {
    p = GetPolygon3D (i);
    lview->CallPolygonFunction ((csObject *)p, true);
  }

  for (i = 0; i < GetCurveCount (); i++)
  {
    csCurve *c = curves.Get (i);
    lview->CallCurveFunction ((csObject *)c, true);
  }

  draw_busy--;
}

void csThing::InitializeDefault ()
{
  Prepare ();

  int i;
  for (i = 0; i < polygons.Length (); i++)
    polygons.Get (i)->InitializeDefault ();
  for (i = 0; i < GetCurveCount (); i++)
    curves.Get (i)->InitializeDefaultLighting ();
}

bool csThing::ReadFromCache (iCacheManager* cache_mgr, int id)
{
  Prepare ();
  if (id == 0) id = thing_id;
  if (GetCacheName ())
  {
    id = 0;
    cache_mgr->SetCurrentScope (GetCacheName ());
  }
  else
  {
    char buf[20];
    sprintf (buf, "def%d", id);
    cache_mgr->SetCurrentScope (buf);
  }

  int i;
  bool rc = false;
  for (i = 0; i < polygons.Length (); i++)
    if (!polygons.Get (i)->ReadFromCache (cache_mgr, id)) goto stop;
  for (i = 0; i < GetCurveCount (); i++)
    if (!curves.Get (i)->ReadFromCache (cache_mgr, id)) goto stop;
  rc = true;

stop:
  cache_mgr->SetCurrentScope (NULL);
  return rc;
}

bool csThing::WriteToCache (iCacheManager* cache_mgr, int id)
{
  if (id == 0) id = thing_id;
  if (GetCacheName ())
  {
    id = 0;
    cache_mgr->SetCurrentScope (GetCacheName ());
  }
  else
  {
    char buf[20];
    sprintf (buf, "def%d", id);
    cache_mgr->SetCurrentScope (buf);
  }

  int i;
  bool rc = false;
  for (i = 0; i < polygons.Length (); i++)
    if (!polygons.Get (i)->WriteToCache (cache_mgr, id)) goto stop;
  for (i = 0; i < GetCurveCount (); i++)
    if (!curves.Get (i)->WriteToCache (cache_mgr, id)) goto stop;
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
  for (i = 0; i < GetCurveCount (); i++) curves.Get (i)->PrepareLighting ();
}

void csThing::Merge (csThing *other)
{
  int i, j;
  int *merge_vertices = new int[other->GetVertexCount () + 1];
  for (i = 0; i < other->GetVertexCount (); i++)
    merge_vertices[i] = AddVertex (other->Vwor (i));

  for (i = 0; i < other->polygons.Length (); i++)
  {
    csPolygon3D *p = other->GetPolygon3D (i);
    int *idx = p->GetVertices ().GetVertexIndices ();
    for (j = 0; j < p->GetVertices ().GetVertexCount (); j++)
      idx[j] = merge_vertices[idx[j]];
    AddPolygon (p);
    other->polygons[i] = NULL;
  }

  for (i = 0; i < other->GetCurveVertexCount (); i++)
    AddCurveVertex (other->GetCurveVertex (i), other->GetCurveTexel (i));

  for (i = 0; i < other->curves.Length (); i++)
  {
    csCurve *c = other->GetCurve (i);
    AddCurve (c);
    other->curves[i] = NULL;
  }

  delete[] merge_vertices;
}

void csThing::MergeTemplate (
  iThingState *tpl,
  iMaterialWrapper *default_material,
  csVector3 *shift,
  csMatrix3 *transform)
{
  int i, j;
  int *merge_vertices;

  flags.SetAll (tpl->GetFlags ().Get ());

  //@@@ OBSOLETE: SetZBufMode (tpl->GetZBufMode ());
  SetMovingOption (tpl->GetMovingOption ());

  //TODO should merge? take averages or something?
  curves_center = tpl->GetCurvesCenter ();
  curves_scale = tpl->GetCurvesScale ();

  //@@@ TEMPORARY
  csRef<iThingState> ith (SCF_QUERY_INTERFACE (tpl, iThingState));
  ParentTemplate = (csThing *) (ith->GetPrivateObject ());

  merge_vertices = new int[tpl->GetVertexCount () + 1];
  for (i = 0; i < tpl->GetVertexCount (); i++)
  {
    csVector3 v;
    v = tpl->GetVertex (i);
    if (transform) v = *transform * v;
    if (shift) v += *shift;
    merge_vertices[i] = AddVertex (v);
  }

  for (i = 0; i < tpl->GetPolygonCount (); i++)
  {
    iPolygon3D *pt = tpl->GetPolygon (i);
    csPolygon3D *p;
    iMaterialWrapper *mat = pt->GetMaterial ();
    p = NewPolygon (mat->GetPrivateObject ());  //@@@
    p->SetName (pt->QueryObject ()->GetName ());

    iMaterialWrapper *wrap = pt->GetMaterial ();
    if (!wrap && default_material)
      p->SetMaterial (default_material->GetPrivateObject ());

    int *idx = pt->GetVertexIndices ();
    for (j = 0; j < pt->GetVertexCount (); j++)
      p->AddVertex (merge_vertices[idx[j]]);

    p->flags.SetAll (pt->GetFlags ().Get ());
    p->CopyTextureType (pt);
  }

  for (i = 0; i < tpl->GetCurveVertexCount (); i++)
  {
    csVector3 v = tpl->GetCurveVertex (i);
    if (transform) v = *transform * v;
    if (shift) v += *shift;
    AddCurveVertex (v, tpl->GetCurveTexel (i));
  }

  for (i = 0; i < tpl->GetCurveCount (); i++)
  {
    iCurve *orig_curve = tpl->GetCurve (i);
    iCurve *p = CreateCurve (orig_curve->GetParentTemplate ());
    p->QueryObject ()->SetName (orig_curve->QueryObject ()->GetName ());
    if (orig_curve->GetMaterial ())
      p->SetMaterial (orig_curve->GetMaterial ());
    else
      p->SetMaterial (default_material);
  }

  delete[] merge_vertices;
}

void csThing::ReplaceMaterials (iMaterialList *matList, const char *prefix)
{
  int i;
  for (i = 0; i < GetPolygonCount (); i++)
  {
    csPolygon3D *p = GetPolygon3D (i);
    const char *txtname = p->GetMaterialWrapper ()->GetName ();
    char *newname = new char[strlen (prefix) + strlen (txtname) + 2];
    sprintf (newname, "%s_%s", prefix, txtname);

    iMaterialWrapper *th = matList->FindByName (newname);
    if (th != NULL) p->SetMaterial (th->GetPrivateObject ());
    delete[] newname;
  }
}

void csThing::AddPortalPolygon (csPolygon3D *poly)
{
  int idx = portal_polygons.Find (poly);

  //@@@???CS_ASSERT (idx == -1);
  if (idx != -1) return ;

  CS_ASSERT (poly->GetPortal () != NULL);
  CS_ASSERT (poly->GetParent () == this);
  portal_polygons.Push (poly);
}

void csThing::RemovePortalPolygon (csPolygon3D *poly)
{
  int idx = portal_polygons.Find (poly);

  //@@@???CS_ASSERT (idx != -1);
  if (idx == -1) return ;

  CS_ASSERT (poly->GetPortal () == NULL || poly->GetParent () != this);
  portal_polygons.Delete (idx);
}

//---------------------------------------------------------------------------
iCurve *csThing::ThingState::GetCurve (int idx) const
{
  csCurve *c = scfParent->GetCurve (idx);
  return &(c->scfiCurve);
}

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

iPolygon3D *csThing::ThingState::CreatePolygon (const char *iName)
{
  csPolygon3D *p = new csPolygon3D ((csMaterialWrapper *)NULL);
  if (iName) p->SetName (iName);
  scfParent->AddPolygon (p);

  csRef<iPolygon3D> ip (SCF_QUERY_INTERFACE (p, iPolygon3D));
  return ip;	// DecRef is ok here.
}

int csThing::ThingState::GetPortalCount () const
{
  return scfParent->portal_polygons.Length ();
}

iPortal *csThing::ThingState::GetPortal (int idx) const
{
  csPolygon3D *p = (csPolygon3D *) (scfParent->portal_polygons)[idx];
  return &(p->GetPortal ()->scfiPortal);
}

iPolygon3D *csThing::ThingState::GetPortalPolygon (int idx) const
{
  csPolygon3D *p = (csPolygon3D *) (scfParent->portal_polygons)[idx];
  return &(p->scfiPolygon3D);
}

iPolygon3D* csThing::ThingState::IntersectSegment (const csVector3& start,
	const csVector3& end, csVector3& isect,
	float* pr, bool only_portals)
{
  csPolygon3D* p = scfParent->IntersectSegment (start, end, isect, pr,
  	only_portals);
  return p ? &(p->scfiPolygon3D) : NULL;
}

//---------------------------------------------------------------------------
iMeshObjectFactory *csThing::MeshObject::GetFactory () const
{
  if (!scfParent->ParentTemplate) return NULL;
  return &scfParent->ParentTemplate->scfiMeshObjectFactory;
}

//---------------------------------------------------------------------------
bool csThing::VisCull::IntersectSegment (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect,
  float *pr,
  iMeshWrapper **p_mesh,
  iPolygon3D** poly)
{
  scfParent->current_visnr++;
  csMeshWrapper *m;
  csPolygon3D *p = scfParent->IntersectSegmentFull (
      start,
      end,
      isect,
      pr,
      &m);
  if (p_mesh)
  {
    if (m)
      *p_mesh = &(m->scfiMeshWrapper);
    else
      *p_mesh = NULL;
  }

  if (poly) *poly = &(p->scfiPolygon3D);
  if (p)
    return true;
  else
    return false;
}

//---------------------------------------------------------------------------
csPtr<iMeshObject> csThing::MeshObjectFactory::NewInstance ()
{
  csThing *thing = new csThing (scfParent);
  thing->MergeTemplate (&(scfParent->scfiThingState), NULL);
  return csPtr<iMeshObject> (&thing->scfiMeshObject);
}

//---------------------------------------------------------------------------
SCF_IMPLEMENT_IBASE(csThingObjectType)
  SCF_IMPLEMENTS_INTERFACE(iMeshObjectType)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iComponent)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iThingEnvironment)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csThingObjectType::eiComponent)
  SCF_IMPLEMENTS_INTERFACE(iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csThingObjectType::eiThingEnvironment)
  SCF_IMPLEMENTS_INTERFACE(iThingEnvironment)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csThingObjectType)

csThingObjectType::csThingObjectType (
  iBase *pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiThingEnvironment);
}

csThingObjectType::~csThingObjectType ()
{
  ClearPolyTxtPlanes ();
  ClearCurveTemplates ();
}

bool csThingObjectType::Initialize (iObjectRegistry *object_reg)
{
  csThingObjectType::object_reg = object_reg;
  return true;
}

csPtr<iMeshObjectFactory> csThingObjectType::NewFactory ()
{
  csThing *cm = new csThing (this);
  csRef<iMeshObjectFactory> ifact (SCF_QUERY_INTERFACE (
      cm, iMeshObjectFactory));
  cm->DecRef ();
  return csPtr<iMeshObjectFactory> (ifact);
}

csPtr<iPolyTxtPlane> csThingObjectType::CreatePolyTxtPlane (const char *name)
{
  csPolyTxtPlane *pl = new csPolyTxtPlane ();
  planes.Push (pl);
  if (name) pl->SetName (name);
  pl->IncRef ();
  return &(pl->scfiPolyTxtPlane);
}

iPolyTxtPlane *csThingObjectType::FindPolyTxtPlane (const char *iName)
{
  csPolyTxtPlane *pl;
  pl = (csPolyTxtPlane *)planes.FindByName (iName);
  if (!pl) return NULL;
  return &(pl->scfiPolyTxtPlane);
}

csPtr<iCurveTemplate> csThingObjectType::CreateBezierTemplate (const char *name)
{
  csBezierTemplate *ptemplate = new csBezierTemplate ();
  if (name) ptemplate->SetName (name);
  curve_templates.Push (ptemplate);
  return SCF_QUERY_INTERFACE (ptemplate, iCurveTemplate);
}

iCurveTemplate *csThingObjectType::FindCurveTemplate (const char *iName)
{
  csCurveTemplate *pl;
  pl = (csCurveTemplate *)curve_templates.FindByName (iName);
  if (!pl) return NULL;

  csRef<iCurveTemplate> itmpl (SCF_QUERY_INTERFACE (pl, iCurveTemplate));
  return itmpl;	// DecRef is ok here.
}

void csThingObjectType::RemovePolyTxtPlane (iPolyTxtPlane *pl)
{
  int i;
  for (i = 0; i < planes.Length (); i++)
  {
    csPolyTxtPlane *pli = (csPolyTxtPlane *)planes[i];
    if (pl == &(pli->scfiPolyTxtPlane))
    {
      planes.Delete (i);
      return ;
    }
  }
}

void csThingObjectType::RemoveCurveTemplate (iCurveTemplate *ct)
{
  int i;
  for (i = 0; i < curve_templates.Length (); i++)
  {
    csCurveTemplate *cti = (csCurveTemplate *)curve_templates[i];
    csRef<iCurveTemplate> i_cti (SCF_QUERY_INTERFACE (cti, iCurveTemplate));
    if (ct == i_cti)
    {
      curve_templates.Delete (i);
      return ;
    }
  }
}

void csThingObjectType::ClearPolyTxtPlanes ()
{
  int i;
  for (i = 0; i < planes.Length (); i++)
  {
    csPolyTxtPlane *p = (csPolyTxtPlane *)planes[i];
    planes[i] = NULL;
    p->DecRef ();
  }

  planes.DeleteAll ();
}

void csThingObjectType::ClearCurveTemplates ()
{
  curve_templates.DeleteAll ();
}

//---------------------------------------------------------------------------
