/*
    Copyright (C) 1998-2001 by Jorrit Tyberghein

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
#include "cssys/sysfunc.h"
#include "spr3d.h"
#include "csgeom/polyclip.h"
#include "csgeom/sphere.h"
#include "csutil/garray.h"
#include "csutil/randomgen.h"
#include "ivideo/graph3d.h"
#include "ivideo/vbufmgr.h"
#include "iengine/camera.h"
#include "iengine/rview.h"
#include "iengine/movable.h"
#include "iengine/light.h"
#include "iengine/engine.h"
#include "iengine/sector.h"
#include "ivaria/reporter.h"
#include "iutil/objreg.h"
#include "iutil/cache.h"
#include "iutil/object.h"
#include "iutil/vfs.h"
#include "csutil/memfile.h"
#include "csutil/csmd5.h"
#include "iengine/mesh.h"
#include "cssys/csendian.h"
#include "qsqrt.h"

// [res: moved here 'cause I get an "internal compiler error" on msvc7... whyever]
CS_IMPLEMENT_STATIC_CLASSVAR (csSprite3DMeshObject, mesh, GetLODMesh, csTriangleMesh, ())

CS_IMPLEMENT_PLUGIN

// Set the default lighting quality.
// See header file for CS_SPR_LIGHTING_* definitions.
//#define DEFAULT_LIGHTING CS_SPR_LIGHTING_HQ
#define DEFAULT_LIGHTING CS_SPR_LIGHTING_LQ
//#define DEFAULT_LIGHTING CS_SPR_LIGHTING_FAST

//--------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csSpriteLODListener)
  SCF_IMPLEMENTS_INTERFACE(iSharedVariableListener)
SCF_IMPLEMENT_IBASE_END

//--------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csSpriteFrame)
  SCF_IMPLEMENTS_INTERFACE (iSpriteFrame)
SCF_IMPLEMENT_IBASE_END

csSpriteFrame::csSpriteFrame (int anm_idx, int tex_idx)
{
  SCF_CONSTRUCT_IBASE (0);
  name = 0;
  animation_index = anm_idx;
  texturing_index = tex_idx;
  normals_calculated = false;
}

csSpriteFrame::~csSpriteFrame ()
{
  delete [] name;
}

void csSpriteFrame::SetName (char const* n)
{
  delete [] name;
  if (n) name = csStrNew (n);
  else name = 0;
}

//--------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csSpriteAction2)
  SCF_IMPLEMENTS_INTERFACE (iSpriteAction)
SCF_IMPLEMENT_IBASE_END

csSpriteAction2::csSpriteAction2() : frames (8, 8), delays (8, 8)
{
  SCF_CONSTRUCT_IBASE (0);
  name = 0;
  SetReverseAction (false);
}

csSpriteAction2::~csSpriteAction2()
{
  delete [] name;
}

void csSpriteAction2::SetName (char const* n)
{
  delete [] name;
  if (n)
  {
    name = new char [strlen (n) + 1];
    strcpy (name, n);
  }
  else
    name = 0;
}

void csSpriteAction2::AddCsFrame (csSpriteFrame * f, int d, float displacement)
{
  frames.Push (f);
  delays.Push (d);
  displacements.Push (displacement);
}

void csSpriteAction2::AddFrame (iSpriteFrame * f, int d, float displacement)
{
  frames.Push ((csSpriteFrame*)f);
  delays.Push (d);
  displacements.Push (displacement);
}

//--------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csSpriteSocket)
  SCF_IMPLEMENTS_INTERFACE (iSpriteSocket)
SCF_IMPLEMENT_IBASE_END

csSpriteSocket::csSpriteSocket()
{
  SCF_CONSTRUCT_IBASE (0);
  triangle_index = 0;
  name = 0;
  attached_mesh = 0;
}

csSpriteSocket::~csSpriteSocket ()
{
  delete [] name;
}

void csSpriteSocket::SetName (char const* n)
{
  delete [] name;
  if (n)
  {
    name = new char [strlen (n)+1];
    strcpy (name, n);
  }
  else
    name = 0;
}

//--------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csSprite3DMeshObjectFactory)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectFactory)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iSprite3DFactoryState)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iLODControl)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObjectModel)
  {
    static scfInterfaceID iPolygonMesh_scfID = (scfInterfaceID)-1;		
    if (iPolygonMesh_scfID == (scfInterfaceID)-1)				
      iPolygonMesh_scfID = iSCF::SCF->GetInterfaceID ("iPolygonMesh");		
    if (iInterfaceID == iPolygonMesh_scfID &&				
      scfCompatibleVersion(iVersion, scfInterface<iPolygonMesh>::GetVersion()))
    {
      printf ("Deprecated feature use: iPolygonMesh queried from Sprite3d "
	"factory; use iObjectModel->GetPolygonMeshColldet() instead.\n");
      iPolygonMesh* Object = scfiObjectModel.GetPolygonMeshColldet();
      (Object)->IncRef ();						
      return STATIC_CAST(iPolygonMesh*, Object);				
    }
  }
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite3DMeshObjectFactory::Sprite3DFactoryState)
  SCF_IMPLEMENTS_INTERFACE (iSprite3DFactoryState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite3DMeshObjectFactory::LODControl)
  SCF_IMPLEMENTS_INTERFACE (iLODControl)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite3DMeshObjectFactory::ObjectModel)
  SCF_IMPLEMENTS_INTERFACE (iObjectModel)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

void csSprite3DMeshObjectFactory::Report (int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csRef<iReporter> rep (CS_QUERY_REGISTRY (object_reg, iReporter));
  if (rep)
    rep->ReportV (severity, "crystalspace.mesh.sprite.3d", msg, arg);
  else
  {
    csPrintfV (msg, arg);
    csPrintf ("\n");
  }
  va_end (arg);
}

csSprite3DMeshObjectFactory::csSprite3DMeshObjectFactory (iBase *pParent) :
    texels (8, 8), vertices (8, 8), normals (8, 8)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSprite3DFactoryState);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiLODControl);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObjectModel);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiPolygonMesh);

  scfiPolygonMesh.SetFactory (this);
  scfiObjectModel.SetPolygonMeshBase (&scfiPolygonMesh);
  scfiObjectModel.SetPolygonMeshColldet (&scfiPolygonMesh);
  scfiObjectModel.SetPolygonMeshViscull (0);
  scfiObjectModel.SetPolygonMeshShadows (0);

  logparent = 0;
  cstxt = 0;
  emerge_from = 0;
  skeleton = 0;
  cachename = 0;

  texel_mesh = new csTriangleMesh ();

  tri_verts = 0;
  do_tweening = true;
  lighting_quality = DEFAULT_LIGHTING;
  lighting_quality_config = CS_SPR_LIGHT_GLOBAL;

  lod_m = 0;
  lod_a = 1;
  lod_level_config = CS_SPR_LOD_GLOBAL;
  MixMode = CS_FX_COPY;

  initialized = false;
}

csSprite3DMeshObjectFactory::~csSprite3DMeshObjectFactory ()
{
  delete texel_mesh;
  delete[] emerge_from;
  delete skeleton;
  delete tri_verts;
  delete[] cachename;
  ClearLODListeners ();
}

void csSprite3DMeshObjectFactory::GenerateCacheName ()
{
  csMemFile mf;
  int32 l;
  l = convert_endian ((int32)frames.Length ());
  mf.Write ((char*)&l, 4);
  l = convert_endian ((int32)actions.Length ());
  mf.Write ((char*)&l, 4);
  l = convert_endian ((int32)GetVertexCount ());
  mf.Write ((char*)&l, 4);
  l = convert_endian ((int32)GetTriangleCount ());
  mf.Write ((char*)&l, 4);

  if (logparent)
  {
    csRef<iMeshFactoryWrapper> mw (SCF_QUERY_INTERFACE (logparent,
    	iMeshFactoryWrapper));
    if (mw)
    {
      if (mw->QueryObject ()->GetName ())
        mf.Write (mw->QueryObject ()->GetName (),
		strlen (mw->QueryObject ()->GetName ()));
    }
  }
  
  csMD5::Digest digest = csMD5::Encode (mf.GetData (), mf.GetSize ());
  csString hex(digest.HexString());
  delete[] cachename;
  cachename = hex.Detach();
}

const char* csSprite3DMeshObjectFactory::GetCacheName ()
{
  if (!cachename)
  {
    GenerateCacheName ();
  }
  return cachename;
}
  
void csSprite3DMeshObjectFactory::AddVertices (int num)
{
  int frame;

  for (frame = 0; frame < frames.Length(); frame++)
  {
    normals.Get (frame)->SetVertexCount (GetVertexCount () + num);
    texels.Get (frame)->SetVertexCount (GetVertexCount () + num);
    vertices.Get (frame)->SetVertexCount (GetVertexCount () + num);
  }
}

void csSprite3DMeshObjectFactory::AddTriangle (int a, int b, int c)
{
  texel_mesh->AddTriangle (a, b, c);
}

void csSprite3DMeshObjectFactory::SetSkeleton (csSkel* sk)
{
  delete skeleton;
  skeleton = sk;
}

csPtr<iMeshObject> csSprite3DMeshObjectFactory::NewInstance ()
{
  if (!initialized)
  {
    initialized = true;
    GenerateLOD ();
    ComputeBoundingBox ();
  }

  csSprite3DMeshObject* spr = new csSprite3DMeshObject ();
  spr->SetFactory (this);
  spr->EnableTweening (do_tweening);

  // Set the quality config of the sprite to that of the template.
  spr->SetLightingQualityConfig (GetLightingQualityConfig());
  spr->SetAction ("default");
  spr->InitSprite ();
  csRef<iMeshObject> im (SCF_QUERY_INTERFACE (spr, iMeshObject));
  spr->DecRef ();
  return csPtr<iMeshObject> (im);
}

void csSprite3DMeshObjectFactory::GenerateLOD ()
{
  int i;

  //@@@ turn this into a parameter or member variable?
  int lod_base_frame = 0;

  csVector3* v = new csVector3[GetVertexCount()];

  for (i = 0; i < GetVertexCount(); i++)
    v[i] = GetVertex (lod_base_frame, i);

  csTriangleVerticesCost* verts = new csTriangleVerticesCost (texel_mesh, v,
  	GetVertexCount());
  delete [] v;

  delete [] emerge_from;
  emerge_from = new int [GetVertexCount()];
  int* translate = new int [GetVertexCount()];
  csTriangleMesh* new_mesh = new csTriangleMesh (*texel_mesh);

  csTriangleMeshLOD::CalculateLOD (new_mesh, verts, translate, emerge_from);

  for (i = 0 ; i < texels.Length () ; i++)
  {
    int j;
    csVector2* new_texels = new csVector2 [GetVertexCount ()];
    csVector3* new_vertices = new csVector3 [GetVertexCount ()];
    csVector3* new_normals = new csVector3 [GetVertexCount ()];
    csPoly2D* tx = texels.Get (i);
    csPoly3D* vt = vertices.Get (i);
    csPoly3D* vn = normals.Get (i);
    for (j = 0 ; j < GetVertexCount () ; j++)
    {
      new_texels[translate[j]] = (*tx)[j];
      new_vertices[translate[j]] = (*vt)[j];
      new_normals[translate[j]] = (*vn)[j];
    }
    for (j = 0 ; j < GetVertexCount () ; j++)
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

  for (i = 0 ; i < GetTriangleCount () ; i++)
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

void csSprite3DMeshObjectFactory::ComputeBoundingBox ()
{
  int frame, vertex;

  for ( frame = 0 ; frame < GetFrameCount () ; frame++ )
  {
    csBox3 box;

    csVector3& v0 = GetVertex (frame, 0);
    box.StartBoundingBox (v0);
    csVector3 max_sq_radius (v0.x*v0.x + v0.x*v0.x,
    	v0.y*v0.y + v0.y*v0.y, v0.z*v0.z + v0.z*v0.z);
    for ( vertex = 1 ; vertex < GetVertexCount() ; vertex++ )
    {
      csVector3& v = GetVertex (frame, vertex);
      box.AddBoundingVertexSmart (v);
      csVector3 sq_radius (v.x*v.x + v.x*v.x, v.y*v.y + v.y*v.y,
      	v.z*v.z + v.z*v.z);
      if (sq_radius.x > max_sq_radius.x) max_sq_radius.x = sq_radius.x;
      if (sq_radius.y > max_sq_radius.y) max_sq_radius.y = sq_radius.y;
      if (sq_radius.z > max_sq_radius.z) max_sq_radius.z = sq_radius.z;
    }

    GetFrame (frame)->SetBoundingBox (box);
    GetFrame (frame)->SetRadius (csVector3 (
    	qsqrt (max_sq_radius.x),
	qsqrt (max_sq_radius.y),
	qsqrt (max_sq_radius.z)));
  }
  if (skeleton)
  {
    // @@@ should the base frame for the skeleton be a variable?
    skeleton->ComputeBoundingBox (vertices.Get (0));
  }
}

csSpriteFrame* csSprite3DMeshObjectFactory::AddFrame ()
{
  csSpriteFrame* fr = new csSpriteFrame (frames.Length(), texels.Length());
  csPoly3D* nr = new csPoly3D ();
  csPoly2D* tx = new csPoly2D ();
  csPoly3D* vr = new csPoly3D ();

  if (frames.Length() > 0)
  {
    nr->SetVertexCount (GetVertexCount ());
    tx->SetVertexCount (GetVertexCount ());
    vr->SetVertexCount (GetVertexCount ());
  }

  frames.Push (fr);
  normals.Push (nr);
  texels.Push (tx);
  vertices.Push (vr);

  return fr;
}

csSpriteFrame* csSprite3DMeshObjectFactory::FindFrame (const char *n)
{
  int i;
  for (i = GetFrameCount () - 1; i >= 0; i--)
    if (strcmp (GetFrame (i)->GetName (), n) == 0)
      return GetFrame (i);

  return 0;
}

csSpriteAction2* csSprite3DMeshObjectFactory::AddAction ()
{
  csSpriteAction2* a = new csSpriteAction2 ();
  actions.Push (a);
  return a;
}

csSpriteSocket* csSprite3DMeshObjectFactory::AddSocket ()
{
  csSpriteSocket* socket = new csSpriteSocket();
  sockets.Push (socket);
  return socket;
}

csSpriteSocket* csSprite3DMeshObjectFactory::FindSocket (const char *n) const
{
  int i;
  for (i = GetSocketCount () - 1; i >= 0; i--)
    if (strcmp (GetSocket (i)->GetName (), n) == 0)
      return GetSocket (i);

  return 0;
}

csSpriteSocket* csSprite3DMeshObjectFactory::FindSocket (iMeshWrapper *mesh) const
{
  int i;
  for (i = GetSocketCount () - 1; i >= 0; i--)
    if (GetSocket (i)->GetMeshWrapper() == mesh)
      return GetSocket (i);

  return 0;
}
void csSprite3DMeshObjectFactory::SetMaterial (iMaterialWrapper *material)
{
  cstxt = material;
}

void csSprite3DMeshObjectFactory::ComputeNormals (csSpriteFrame* frame)
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
    tri_verts = new csTriangleVerticesCost (texel_mesh, object_verts,
      GetVertexCount());
  }

  csTriangle * tris = texel_mesh->GetTriangles();
  int num_triangles = texel_mesh->GetTriangleCount();
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

  for (i = 0; i < GetVertexCount(); i++)
  {
    csTriangleVertexCost &vt = tri_verts->GetVertex (i);
    if (vt.con_triangles.Length ())
    {
      csVector3 &n = GetNormal (frame_number, i);
      if (n.IsZero())
      {
        //n.Set (0,0,0);
        for (j = 0; j < vt.con_triangles.Length (); j++)
          n += tri_normals [vt.con_triangles[j]];
        float norm = n.Norm ();
        if (norm)
          n /= norm;
      }
    }
    else
    {
      // If there are no connecting triangles then we just
      // initialize the normal to a default value.
      GetNormal (frame_number, i).Set (1, 0, 0);
    }
  }

  delete[] tri_normals;
}

void csSprite3DMeshObjectFactory::MergeNormals ()
{
  int i;
  for (i = 0; i < GetFrameCount (); i++)
    MergeNormals (i, i);
}

void csSprite3DMeshObjectFactory::MergeNormals (int base)
{
  if (base > GetFrameCount())
  {
    Report (CS_REPORTER_SEVERITY_WARNING, "No frame number: %d", base);
    Report (CS_REPORTER_SEVERITY_WARNING, "no smoothing performed");
    return;
  }
  int i;
  for (i = 0; i < GetFrameCount (); i++)
    MergeNormals (base, i);
}

void csSprite3DMeshObjectFactory::MergeNormals (int base, int frame)
{
  int i, j;

  int num_frames = GetFrameCount();
  if (base  > num_frames)
    Report (CS_REPORTER_SEVERITY_WARNING, "No frame number: %d", base);
  if (frame > num_frames)
    Report (CS_REPORTER_SEVERITY_WARNING, "No frame number: %d", frame);
  if (frame > num_frames || base > num_frames)
  {
    Report (CS_REPORTER_SEVERITY_WARNING, "no smoothing performed");
    return;
  }

  GetFrame (frame)->SetNormalsCalculated (true);

  csRef<iEngine> engine (CS_QUERY_REGISTRY (object_reg, iEngine));
  char buf[100];
  iCacheManager* cache_mgr = 0;
  if (engine)
  {
    cache_mgr = engine->GetCacheManager ();
    sprintf (buf, "spr3dnormals_%d_%d", base, frame);
    csRef<iDataBuffer> db (cache_mgr->ReadCache (buf, GetCacheName (), 0));
    if (db)
    {
      csMemFile* cf = new csMemFile ((char*)db->GetData (), db->GetSize (),
  	  csMemFile::DISPOSITION_IGNORE);
      csVector3* fr_normals = GetNormals (frame);
      for (i = 0; i < GetVertexCount(); i++)
      {
        float f;
	csVector3 v;
        cf->Read ((char*)&f, 4); v.x = convert_endian (f);
        cf->Read ((char*)&f, 4); v.y = convert_endian (f);
        cf->Read ((char*)&f, 4); v.z = convert_endian (f);
	fr_normals[i].Set (v);
      }

      cf->DecRef ();
      return;
    }
  }


  csVector3* obj_verts  = GetVertices (frame);
  csVector3* base_verts = GetVertices (base);

  if (!tri_verts)
  {
    tri_verts = new csTriangleVerticesCost (texel_mesh, obj_verts,
    	GetVertexCount());
  }

  csTriangle * tris = texel_mesh->GetTriangles();
  int num_triangles = texel_mesh->GetTriangleCount();
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
  int * merge = new int [GetVertexCount()];
  for (i = 0; i < GetVertexCount(); i++)
  {
    merge[i] = i;
    for (j = 0; j < i; j++)
    {
      csVector3 difference = base_verts[i] - base_verts[j];
      if (difference.SquaredNorm () < 0.0001)
      {
        merge[i] = j;
        break;
      }
    }
  }

  // create a mesh which only uses the vertex indices in the table
  csTriangleMesh merge_mesh;
  for (i = 0; i < num_triangles; i++)
    merge_mesh.AddTriangle (merge[tris[i].a], merge[tris[i].b],
    	merge[tris[i].c]);
  csTriangleVerticesCost * tv = new csTriangleVerticesCost (&merge_mesh,
  	obj_verts, GetVertexCount());

  // calculate vertex normals, by averaging connected triangle normals
  csVector3* fr_normals = GetNormals (frame);
  for (i = 0; i < GetVertexCount(); i++)
  {
    csTriangleVertexCost &vt = tv->GetVertex (i);
    if (vt.con_triangles.Length ())
    {
      csVector3 &n = fr_normals[i];
      n.Set (tri_normals[vt.con_triangles[0]]);
      for (j = 1; j < vt.con_triangles.Length (); j++)
        n += tri_normals [vt.con_triangles[j]];
      float norm = n.Norm ();
      if (norm)
        n /= norm;
    }
  }

  // one last loop to fill in all of the merged vertex normals
  for (i = 0; i < GetVertexCount(); i++)
  {
    fr_normals[i].Set (fr_normals[merge[i]]);
  }

  if (cache_mgr)
  {
    csMemFile m;
    csRef<iFile> mf (SCF_QUERY_INTERFACE ((&m), iFile));

    csVector3* fr_normals = GetNormals (frame);
    for (i = 0; i < GetVertexCount(); i++)
    {
      float f;
      csVector3& v = fr_normals[i];
      f = convert_endian (v.x);
      mf->Write ((char *) &f, 4);
      f = convert_endian (v.y);
      mf->Write ((char *) &f, 4);
      f = convert_endian (v.z);
      mf->Write ((char *) &f, 4);
    }

    cache_mgr->CacheData ((void*)(m.GetData ()), m.GetSize (),
  	  buf, GetCacheName (), 0);
  }

  delete[] tri_normals;
  delete[] merge;
  delete tv;
}

int csSprite3DMeshObjectFactory::GetLODPolygonCount (float lod) const
{
  return QInt (GetTriangleCount ()*lod);
}

void csSprite3DMeshObjectFactory::ClearLODListeners ()
{
  if (lod_varm)
  {
    lod_varm->RemoveListener (lod_varm_listener);
    lod_varm_listener = 0;
    lod_varm = 0;
  }
  if (lod_vara)
  {
    lod_vara->RemoveListener (lod_vara_listener);
    lod_vara_listener = 0;
    lod_vara = 0;
  }
}

void csSprite3DMeshObjectFactory::SetupLODListeners (iSharedVariable* varm,
	iSharedVariable* vara)
{
  ClearLODListeners ();
  lod_varm = varm;
  lod_vara = vara;
  lod_varm_listener = csPtr<csSpriteLODListener> (
  	new csSpriteLODListener (&lod_m));
  lod_varm->AddListener (lod_varm_listener);
  lod_vara_listener = csPtr<csSpriteLODListener> (
  	new csSpriteLODListener (&lod_a));
  lod_vara->AddListener (lod_vara_listener);
}

csSpriteAction2* csSprite3DMeshObjectFactory::FindAction (const char *n) const
{
  int i;
  for (i = GetActionCount () - 1; i >= 0; i--)
    if (strcmp (GetAction (i)->GetName (), n) == 0)
      return GetAction (i);

  return 0;
}

void csSprite3DMeshObjectFactory::HardTransform (const csReversibleTransform& t)
{
  if (skeleton)
  {
    // If there is a skeleton we only have to transform the vertices of the
    // root limb and change the transforms of all children connecting to
    // the root.
    int vt_cnt = skeleton->GetVertexCount ();
    int* vt = skeleton->GetVertices ();
    int i;
    csVector3* verts = GetVertices (0);
    for (i = 0 ; i < vt_cnt ; i++)
      verts[vt[i]] = t.This2Other (verts[vt[i]]);
    iSkeletonLimb* c = skeleton->GetChildren ();
    while (c)
    {
      csRef<iSkeletonConnection> con = SCF_QUERY_INTERFACE (c,
      	iSkeletonConnection);
      if (con)
      {
        csTransform& tr = con->GetTransformation ();
	// @@@ Not sure about the line below.
	con->SetTransformation (t * tr);
      }
      c = c->GetNextSibling ();
    }
    ComputeBoundingBox ();
  }
  else
  {
    int num = GetVertexCount ();
    int numf = GetFrameCount ();
    int i, j;
    for (i = 0 ; i < numf ; i++)
    {
      csVector3* verts = GetVertices (i);
      csBox3 box;
      verts[0] = t.This2Other (verts[0]);
      csVector3& v0 = verts[0];
      box.StartBoundingBox (v0);
      csVector3 max_sq_radius (v0.x*v0.x + v0.x*v0.x,
    	v0.y*v0.y + v0.y*v0.y, v0.z*v0.z + v0.z*v0.z);
      for (j = 1 ; j < num ; j++)
      {
        csVector3& v = verts[j];
        v = t.This2Other (v);
        box.AddBoundingVertexSmart (v);
	csVector3 sq_radius (v.x*v.x + v.x*v.x, v.y*v.y + v.y*v.y,
		v.z*v.z + v.z*v.z);
        if (sq_radius.x > max_sq_radius.x) max_sq_radius.x = sq_radius.x;
        if (sq_radius.y > max_sq_radius.y) max_sq_radius.y = sq_radius.y;
        if (sq_radius.z > max_sq_radius.z) max_sq_radius.z = sq_radius.z;
      }
      GetFrame (i)->SetBoundingBox (box);
      GetFrame (i)->SetRadius (csVector3 (
    	qsqrt (max_sq_radius.x),
	qsqrt (max_sq_radius.y),
	qsqrt (max_sq_radius.z)));

    }
  }
  scfiObjectModel.ShapeChanged ();
}

void csSprite3DMeshObjectFactory::Sprite3DFactoryState::
	EnableSkeletalAnimation ()
{
  csSkel* skel = new csSkel ();
  scfParent->SetSkeleton (skel);
}

iSkeleton* csSprite3DMeshObjectFactory::Sprite3DFactoryState::GetSkeleton ()
  const
{
  csRef<iSkeleton> iskel (
  	SCF_QUERY_INTERFACE_SAFE (scfParent->GetSkeleton (), iSkeleton));
  return iskel;	// DecRef is ok here.
}

void csSprite3DMeshObjectFactory::GetObjectBoundingBox (csBox3& b, int /*type*/)
{
  if (skeleton)
  {
    skeleton->ComputeBoundingBox (csTransform (), b, vertices.Get (0));
  }
  else
  {
    csSpriteFrame* cframe = GetAction (0)->GetCsFrame (0);
    cframe->GetBoundingBox (b);
  }
}

void csSprite3DMeshObjectFactory::GetRadius (csVector3& rad, csVector3& cent)
{
  csVector3 r; csBox3 bbox;
  GetObjectBoundingBox (bbox);
  cent = bbox.GetCenter();

  csSpriteFrame* cframe = GetAction (0)->GetCsFrame (0);
  cframe->GetRadius (r);
  rad =  r;
}

csMeshedPolygon* csSprite3DMeshObjectFactory::PolyMesh::GetPolygons ()
{
  if (!polygons)
  {
    csTriangle* triangles = factory->GetTriangles ();
    polygons = new csMeshedPolygon [GetPolygonCount ()];
    int i;
    for (i = 0 ; i < GetPolygonCount () ; i++)
    {
      polygons[i].num_vertices = 3;
      polygons[i].vertices = &triangles[i].a;
    }
  }
  return polygons;
}

//=============================================================================

SCF_IMPLEMENT_IBASE (csSprite3DMeshObjectFactory::PolyMesh)
  SCF_IMPLEMENTS_INTERFACE (iPolygonMesh)
SCF_IMPLEMENT_IBASE_END

//=============================================================================

SCF_IMPLEMENT_IBASE (csSprite3DMeshObject)
  SCF_IMPLEMENTS_INTERFACE (iMeshObject)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iSprite3DState)
#ifndef CS_USE_NEW_RENDERER
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iVertexBufferManagerClient)
#else
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iRenderBufferSource)
#endif // CS_USE_NEW_RENDERER
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iLODControl)
  {
    static scfInterfaceID iPolygonMesh_scfID = (scfInterfaceID)-1;		
    if (iPolygonMesh_scfID == (scfInterfaceID)-1)				
      iPolygonMesh_scfID = iSCF::SCF->GetInterfaceID ("iPolygonMesh");		
    if (iInterfaceID == iPolygonMesh_scfID &&				
      scfCompatibleVersion(iVersion, scfInterface<iPolygonMesh>::GetVersion()))
    {
#ifdef CS_DEBUG
      printf ("Deprecated feature use: iPolygonMesh queried from Sprite3d "
	"object; use iMeshObject->GetObjectModel()->"
	"GetPolygonMeshColldet() instead.\n");
#endif
      (&scfiPolygonMesh)->IncRef ();						
      return STATIC_CAST(iPolygonMesh*, &scfiPolygonMesh);				
    }
  }
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite3DMeshObject::Sprite3DState)
  SCF_IMPLEMENTS_INTERFACE (iSprite3DState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite3DMeshObject::PolyMesh)
  SCF_IMPLEMENTS_INTERFACE (iPolygonMesh)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

#ifndef CS_USE_NEW_RENDERER
SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite3DMeshObject::eiVertexBufferManagerClient)
  SCF_IMPLEMENTS_INTERFACE (iVertexBufferManagerClient)
SCF_IMPLEMENT_EMBEDDED_IBASE_END
#else
SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite3DMeshObject::RenderBufferSource)
  SCF_IMPLEMENTS_INTERFACE (iRenderBufferSource)
SCF_IMPLEMENT_EMBEDDED_IBASE_END
#endif // CS_USE_NEW_RENDERER

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite3DMeshObject::LODControl)
  SCF_IMPLEMENTS_INTERFACE (iLODControl)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

/// Static vertex array.
typedef csDirtyAccessArray<csVector3> spr3d_tr_verts;
CS_IMPLEMENT_STATIC_VAR (Get_tr_verts, spr3d_tr_verts, ())
/// Static uv array.
typedef csDirtyAccessArray<csVector2> spr3d_uv_verts;
CS_IMPLEMENT_STATIC_VAR (Get_uv_verts, spr3d_uv_verts, ())
/// The list of fog vertices
#ifndef CS_USE_NEW_RENDERER
typedef csDirtyAccessArray<G3DFogInfo> spr3d_fog_verts;
CS_IMPLEMENT_STATIC_VAR (Get_fog_verts, spr3d_fog_verts, ())
#endif // CS_USE_NEW_RENDERER
/// The list of object vertices.
typedef csDirtyAccessArray<csVector3> spr3d_obj_verts;
CS_IMPLEMENT_STATIC_VAR (Get_obj_verts, spr3d_obj_verts, ())
/// The list of tween vertices.
typedef csDirtyAccessArray<csVector3> spr3d_tween_verts;
CS_IMPLEMENT_STATIC_VAR (Get_tween_verts, spr3d_tween_verts, ())

spr3d_tr_verts *tr_verts = 0;
spr3d_uv_verts *uv_verts = 0;
#ifndef CS_USE_NEW_RENDERER
spr3d_fog_verts *fog_verts = 0;
#endif // CS_USE_NEW_RENDERER
spr3d_obj_verts *obj_verts = 0;
spr3d_tween_verts *tween_verts = 0;

csSprite3DMeshObject::csSprite3DMeshObject ()
{
  SCF_CONSTRUCT_IBASE (0);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiPolygonMesh);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSprite3DState);
#ifndef CS_USE_NEW_RENDERER
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiVertexBufferManagerClient);
#else
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiRenderBufferSource);
#endif // CS_USE_NEW_RENDERER
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiLODControl);
  logparent = 0;
  cur_frame = 0;
  factory = 0;
  force_otherskin = false;
  cur_action = 0;
  vertex_colors = 0;
  skeleton_state = 0;
  tween_ratio = 0;
  do_lighting = true;
  num_verts_for_lod = -1;

  tr_verts = Get_tr_verts ();
  uv_verts = Get_uv_verts ();
#ifndef CS_USE_NEW_RENDERER
  fog_verts = Get_fog_verts ();
#endif // CS_USE_NEW_RENDERER
  obj_verts = Get_obj_verts ();
  tween_verts = Get_tween_verts ();
  GetLODMesh ();

  tr_verts->IncRef ();
  uv_verts->IncRef ();
#ifndef CS_USE_NEW_RENDERER
  fog_verts->IncRef ();
#endif // CS_USE_NEW_RENDERER
  obj_verts->IncRef ();
  tween_verts->IncRef ();

  rand_num = new csRandomGen();

  do_tweening = true;
  vis_cb = 0;

  cur_cameranr = -1;
  cur_movablenr = -1;
  MixMode = CS_FX_COPY;
  base_color.Set (0, 0, 0);
  initialized = false;
  local_lod_m = 0;
  local_lod_a = 1;
  speedfactor = 1;
  loopaction = true;
  fullstop = false;
  last_action = 0;

#ifndef CS_USE_NEW_RENDERER
  vbufmgr = 0;
#endif // CS_USE_NEW_RENDERER
}

csSprite3DMeshObject::~csSprite3DMeshObject ()
{
  if (vis_cb) vis_cb->DecRef ();
#ifndef CS_USE_NEW_RENDERER
  if (vbufmgr) vbufmgr->RemoveClient (&scfiVertexBufferManagerClient);
#endif // CS_USE_NEW_RENDERER
  uv_verts->DecRef ();
  tr_verts->DecRef ();
#ifndef CS_USE_NEW_RENDERER
  fog_verts->DecRef ();
#endif // CS_USE_NEW_RENDERER
  obj_verts->DecRef ();
  tween_verts->DecRef ();

  delete [] vertex_colors;
  delete skeleton_state;
  delete rand_num;
  ClearLODListeners ();
}

int csSprite3DMeshObject::GetLODPolygonCount (float lod) const
{
  return QInt (factory->GetTriangleCount () * lod);
}

void csSprite3DMeshObject::ClearLODListeners ()
{
  if (local_lod_varm)
  {
    local_lod_varm->RemoveListener (local_lod_varm_listener);
    local_lod_varm_listener = 0;
    local_lod_varm = 0;
  }
  if (local_lod_vara)
  {
    local_lod_vara->RemoveListener (local_lod_vara_listener);
    local_lod_vara_listener = 0;
    local_lod_vara = 0;
  }
}

void csSprite3DMeshObject::SetupLODListeners (iSharedVariable* varm,
	iSharedVariable* vara)
{
  ClearLODListeners ();
  local_lod_varm = varm;
  local_lod_vara = vara;
  local_lod_varm_listener = csPtr<csSpriteLODListener> (
  	new csSpriteLODListener (&local_lod_m));
  local_lod_varm->AddListener (local_lod_varm_listener);
  local_lod_vara_listener = csPtr<csSpriteLODListener> (
  	new csSpriteLODListener (&local_lod_a));
  local_lod_vara->AddListener (local_lod_vara_listener);
}

void csSprite3DMeshObject::SetFactory (csSprite3DMeshObjectFactory* tmpl)
{
  factory = tmpl;
  delete skeleton_state;
  skeleton_state = 0;
  if (tmpl->GetSkeleton ())
    skeleton_state = (csSkelState*)tmpl->GetSkeleton ()->CreateState ();
  EnableTweening (tmpl->IsTweeningEnabled ());
  MixMode = tmpl->GetMixMode ();
  SetLodLevelConfig (factory->GetLodLevelConfig ());

  // Copy the sockets list down to the mesh
  iSpriteSocket *factory_socket,*new_socket;
  int i;
  for (i=0; i<tmpl->GetSocketCount(); i++)
  {
    factory_socket = tmpl->GetSocket(i);
    new_socket = AddSocket();  // mesh now
    new_socket->SetName (factory_socket->GetName() );
    new_socket->SetTriangleIndex (factory_socket->GetTriangleIndex() );
    new_socket->SetMeshWrapper (0);
  }
}

void csSprite3DMeshObject::SetMaterial (iMaterialWrapper *material)
{
  force_otherskin = true;
  cstxt = material;
}

void csSprite3DMeshObject::AddVertexColor (int i, const csColor& col)
{
  if (!vertex_colors)
  {
    int vt = factory->GetVertexCount ();
    vertex_colors = new csColor [vt];
    int j;
    for (j = 0 ; j < factory->GetVertexCount (); j++)
      vertex_colors[j] = base_color;
  }
  vertex_colors [i] += col;
}

void csSprite3DMeshObject::ResetVertexColors ()
{
  if (vertex_colors)
  {
    int i;
    for (i = 0 ; i < factory->GetVertexCount (); i++)
      vertex_colors [i] = base_color;
  }
  //delete [] vertex_colors;
  //vertex_colors = 0;
}

void csSprite3DMeshObject::FixVertexColors ()
{
  if (vertex_colors)
  {
    int i;
    for (i = 0 ; i < factory->GetVertexCount (); i++)
      vertex_colors [i].Clamp (2., 2., 2.);
  }
}

float csSprite3DMeshObject::global_lod_m = 0;
float csSprite3DMeshObject::global_lod_a = 1;
csRef<iSharedVariable> csSprite3DMeshObject::global_lod_varm;
csRef<iSharedVariable> csSprite3DMeshObject::global_lod_vara;

// Set the default lighting quality.
int csSprite3DMeshObject::global_lighting_quality = DEFAULT_LIGHTING;

static int map (int* emerge_from, int idx, int num_verts)
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

int csSprite3DMeshObject::GetVertexToLightCount ()
{
  // @@@
  if (GetLodLevel (0) < .99)
  {
    if (num_verts_for_lod == -1)
      return factory->GetVertexCount ();
    else
      return num_verts_for_lod;
  }
  else
    return factory->GetVertexCount ();
}

void csSprite3DMeshObject::GenerateSpriteLOD (int num_vts)
{
  int* emerge_from = factory->GetEmergeFrom ();
  csTriangleMesh* base_mesh = factory->GetTexelMesh ();
  mesh->SetSize (0);
  int i;
  int a, b, c;
  for (i = 0 ; i < base_mesh->GetTriangleCount () ; i++)
  {
    csTriangle& tr = base_mesh->GetTriangles ()[i];
    a = map (emerge_from, tr.a, num_vts);
    b = map (emerge_from, tr.b, num_vts);
    c = map (emerge_from, tr.c, num_vts);
    if (a != b && b != c && a != c) mesh->AddTriangle (a, b, c);
  }
}

void csSprite3DMeshObject::UpdateWorkTables (int max_size)
{
  if (max_size > tr_verts->Length ())
  {
    tr_verts->SetLength (max_size);
    uv_verts->SetLength (max_size);
#ifndef CS_USE_NEW_RENDERER
    fog_verts->SetLength (max_size);
#endif // CS_USE_NEW_RENDERER
    obj_verts->SetLength (max_size);
    tween_verts->SetLength (max_size);
  }
}

void csSprite3DMeshObject::GetTransformedBoundingBox (
	long cameranr, long movablenr,
	const csReversibleTransform& trans, csBox3& cbox)
{
  if (cur_cameranr == cameranr && cur_movablenr == movablenr)
  {
    cbox = camera_bbox;
    return;
  }
  cur_cameranr = cameranr;
  cur_movablenr = movablenr;

  if (skeleton_state)
  {
    skeleton_state->ComputeBoundingBox (trans, camera_bbox,
    	factory->vertices.Get (0));
  }
  else
  {
    CS_ASSERT (cur_action != 0);
    csSpriteFrame* cframe = cur_action->GetCsFrame (cur_frame);
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

static void Perspective (const csVector3& v, csVector2& p, float fov,
    	float sx, float sy)
{
  float iz = fov / v.z;
  p.x = v.x * iz + sx;
  p.y = v.y * iz + sy;
}

float csSprite3DMeshObject::GetScreenBoundingBox (
	long cameranr, long movablenr, float fov, float sx, float sy,
	const csReversibleTransform& trans, csBox2& sbox, csBox3& cbox)
{
  csVector2 oneCorner;

  GetTransformedBoundingBox (cameranr, movablenr, trans, cbox);

  // if the entire bounding box is behind the camera, we're done
  if (cbox.MaxZ () < 0)
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

void csSprite3DMeshObject::GetObjectBoundingBox (csBox3& b, int /*type*/)
{
  if (skeleton_state)
  {
    skeleton_state->ComputeBoundingBox (csTransform (), b,
    	factory->vertices.Get (0));
  }
  else
  {
    CS_ASSERT (cur_action != 0);
    csSpriteFrame* cframe = cur_action->GetCsFrame (cur_frame);
    cframe->GetBoundingBox (b);
  }
}

void csSprite3DMeshObject::GetRadius (csVector3& rad, csVector3& cent)
{
  csVector3 r; csBox3 bbox;
  GetObjectBoundingBox(bbox);
  cent = bbox.GetCenter();

  if (skeleton_state)
  {
    skeleton_state->ComputeSqRadius (csTransform (), r,
    	factory->vertices.Get (0));
    r.x = qsqrt (r.x);
    r.y = qsqrt (r.y);
    r.z = qsqrt (r.z);
  }
  else
  {
    CS_ASSERT (cur_action != 0);
    csSpriteFrame* cframe = cur_action->GetCsFrame (cur_frame);
    cframe->GetRadius (r);
  }
  rad =  r;
}

void csSprite3DMeshObject::SetupObject ()
{
  if (!initialized)
  {
    initialized = true;

#ifdef CS_USE_NEW_RENDERER
    csRef<iStringSet> strings = 
      CS_QUERY_REGISTRY_TAG_INTERFACE (factory->object_reg,
      "crystalspace.renderer.stringset", iStringSet);
    vertices_name = strings->Request ("vertices");
    normals_name = strings->Request ("normals");
    texcoords_name = strings->Request ("texture coordinates");
    colors_name = strings->Request ("colors");
    indices_name = strings->Request ("indices");
#endif // CS_USE_NEW_RENDERER

    InitSprite ();
  }
}

csSpriteSocket* csSprite3DMeshObject::AddSocket ()
{
  csSpriteSocket* socket = new csSpriteSocket();
  sockets.Push (socket);
  return socket;
}

csSpriteSocket* csSprite3DMeshObject::FindSocket (const char *n) const
{
  int i;
  for (i = GetSocketCount () - 1; i >= 0; i--)
    if (strcmp (GetSocket (i)->GetName (), n) == 0)
      return GetSocket (i);

  return 0;
}

csSpriteSocket* csSprite3DMeshObject::FindSocket (iMeshWrapper *mesh) const
{
  int i;
  for (i = GetSocketCount () - 1; i >= 0; i--)
    if (GetSocket (i)->GetMeshWrapper() == mesh)
      return GetSocket (i);

  return 0;
}

bool csSprite3DMeshObject::DrawTest (iRenderView* rview, iMovable* movable)
{
  SetupObject ();

  if (!factory->cstxt)
  {
    factory->Report (CS_REPORTER_SEVERITY_ERROR,
    	"Error! Trying to draw a sprite with no material!");
    return false;
  }

#ifndef CS_USE_NEW_RENDERER
  iGraphics3D* g3d = rview->GetGraphics3D ();
#endif
  iCamera* camera = rview->GetCamera ();

  /*
   *  If we're going to figure out the position of a socket-attached mesh here in the drawing function
   *   we should be setting this position for THIS frame not for the next frame!
   */

    // Drill down into the parent and compute the center of
  // the socket where this mesh should be located along with the rotation angles
  csRef<iMeshWrapper> mw = 0;
  if(logparent) mw = SCF_QUERY_INTERFACE (logparent, iMeshWrapper);
  if (mw)
  {
    // Get the parent of this wrapper
    iMeshWrapper* parent = mw->GetParentContainer();
    if (parent)
    {
      // Get the parent of this wrapper
      iMeshFactoryWrapper* mfw = parent->GetFactory();
      if (mfw)
      {
        // Get the parent of this wrapper
        iMeshObjectFactory* mof = mfw->GetMeshObjectFactory();
        if (mof)
        {
	  // Find the parent mesh factory to get vertices
          csRef<iSprite3DFactoryState> sof (SCF_QUERY_INTERFACE (
              mof, iSprite3DFactoryState));
          if (sof)
          {
            // Find the socket in the parent mesh
	    csRef<iSprite3DState> parent_sprite (SCF_QUERY_INTERFACE (
              parent->GetMeshObject(), iSprite3DState));
	    if (parent_sprite)
	    {
              iSpriteSocket * socket = parent_sprite->FindSocket(mw);
              if (socket)
              {
                // Get the index of the triange at that spot
                int tri_index = socket->GetTriangleIndex();
                csTriangle& tri = sof->GetTriangles()[tri_index];
              
                // This cast is crummy, but the only way to get at it.
                csSprite3DMeshObject *cs = 
                  (csSprite3DMeshObject *)parent->GetMeshObject();
              
                int current_frame = cs->GetCurFrame();
                csSpriteAction2* current_action = cs->GetCurAction();
              
                csSpriteFrame* cf = 
                  current_action->GetCsFrame (current_frame);
              
                int idx = cf->GetAnmIndex();
                csVector3 * current_verts = sof->GetVertices(idx);
  
                csVector3 spot_verts[3];
                csVector3 center;
                if (!cs->IsTweeningEnabled())
                {
                  spot_verts[0] = current_verts[tri.a];
                  spot_verts[1] = current_verts[tri.b];
                  spot_verts[2] = current_verts[tri.c];
                  center = 
                    (spot_verts[0] + spot_verts[1] + spot_verts[2]) / 3;
                }
                else
                {
                  // Get the verts for the next frame
                  csSpriteFrame * nframe = 0;
                  if (current_frame + 1 < current_action->GetFrameCount())
                    nframe = current_action->GetCsFrame (current_frame + 1);
                  else
                    nframe = current_action->GetCsFrame (0);
                  int nf_idx = nframe->GetAnmIndex();
                  csVector3 * next_verts = sof->GetVertices(nf_idx);

                  // Interpolate between them
                  float parent_tween_ratio = cs->GetTweenRatio();
                  float remainder = 1 - parent_tween_ratio;
    
                  // Lets look at the tween ratio also... Maybe this is the glitch
                  spot_verts[0] = 
                    parent_tween_ratio * next_verts[tri.a] + 
                    remainder * current_verts[tri.a];

                  spot_verts[1] = 
                    parent_tween_ratio * next_verts[tri.b] + 
                    remainder * current_verts[tri.b];

                  spot_verts[2] = 
                    parent_tween_ratio * next_verts[tri.c] + 
                    remainder * current_verts[tri.c];
  
              
                  // Create the center of the triangle for translation
                  center = 
                    (spot_verts[0] + spot_verts[1] + spot_verts[2]) / 3;
                }
              
                // Get the normal to this triangle based on the verts
                csVector3 ab = spot_verts[1] - spot_verts[0];
                csVector3 bc = spot_verts[2] - spot_verts[1];
                csVector3 normal = ab % bc;
              
                csReversibleTransform trans = movable->GetFullTransform();
                trans.SetOrigin(center);
                trans.LookAt(normal, csVector3(0,1,0));
                movable->SetTransform(trans);
                movable->UpdateMove();
	      }
            }
          }
        }
      }
    }
  }




  // First create the transformation from object to camera space directly:
  //   W = Mow * O - Vow;
  //   C = Mwc * (W - Vwc)
  // ->
  //   C = Mwc * (Mow * O - Vow - Vwc)
  //   C = Mwc * Mow * O - Mwc * (Vow + Vwc)
  tr_o2c = camera->GetTransform ();
  if (!movable->IsFullTransformIdentity ())
    tr_o2c /= movable->GetFullTransform ();
#ifdef CS_USE_NEW_RENDERER
  rendermesh.object2camera = tr_o2c;
#endif

#if 1
  csVector3 radius;
  csSphere sphere;
  GetRadius (radius, sphere.GetCenter ());
  float max_radius = radius.x;
  if (max_radius < radius.y) max_radius = radius.y;
  if (max_radius < radius.z) max_radius = radius.z;
  sphere.SetRadius (max_radius);
  int clip_portal, clip_plane, clip_z_plane;
  if (rview->ClipBSphere (tr_o2c, sphere, clip_portal, clip_plane,
  	clip_z_plane) == false)
    return false;
#else
  float fov = camera->GetFOV ();
  float shiftx = camera->GetShiftX ();
  float shifty = camera->GetShiftY ();

  // Test visibility of entire sprite by clipping bounding box against clipper.
  // There are three possibilities:
  //	1. box is not visible -> sprite is not visible.
  //	2. box is entirely visible -> sprite is visible and need not be clipped.
  //	3. box is partially visible -> sprite is visible and needs to be clipped
  //	   if rview has do_clip_plane set to true.
  csBox2 bbox;
  csBox3 bbox3;
  if (GetScreenBoundingBox (camera->GetCameraNumber (),
  	movable->GetUpdateNumber (), fov, shiftx, shifty,
  	tr_o2c, bbox, bbox3) < 0) return false;	// Not visible.
  int clip_portal, clip_plane, clip_z_plane;
  if (rview->ClipBBox (bbox, bbox3, clip_portal, clip_plane,
  	clip_z_plane) == false)
    return false;
#endif

  UpdateWorkTables (factory->GetVertexCount());

  CS_ASSERT (cur_action != 0);
  csSpriteFrame * cframe = cur_action->GetCsFrame (cur_frame);

  // Get next frame for animation tweening.
  csSpriteFrame * next_frame = cur_action->GetCsNextFrame (cur_frame);

  // First create the transformation from object to camera space directly:
  //   W = Mow * O - Vow;
  //   C = Mwc * (W - Vwc)
  // ->
  //   C = Mwc * (Mow * O - Vow - Vwc)
  //   C = Mwc * Mow * O - Mwc * (Vow + Vwc)

#ifndef CS_USE_NEW_RENDERER
  g3d->SetObjectToCamera (&tr_o2c);
#endif

  bool do_tween = false;
  if (!skeleton_state && tween_ratio) do_tween = true;

  int cf_idx = cframe->GetAnmIndex();

  csVector3* real_obj_verts;
  csVector3* real_tween_verts = 0;

  real_obj_verts = factory->GetVertices (cf_idx);
  if (do_tween)
  {
    int nf_idx = next_frame->GetAnmIndex();
    real_tween_verts = factory->GetVertices (nf_idx);
  }

  // If we have a skeleton then we transform all vertices through
  // the skeleton. In that case we also include the camera transformation
  // so that the 3D renderer does not need to do it anymore.
  csVector3* verts;
  if (skeleton_state)
  {
    skeleton_state->Transform (tr_o2c, real_obj_verts, tr_verts->GetArray ());
    verts = tr_verts->GetArray ();
  }
  else
  {
    verts = real_obj_verts;
  }

  // Calculate the right LOD level for this sprite.

  // Select the appropriate mesh.
  csTriangleMesh* m;
  int* emerge_from = 0;

  float fnum = 0.0f;

  // GetLodLevel() is the distance at which you will see full detail
  float level_of_detail = 1;

  if (IsLodEnabled ())
  {
    // reduce LOD based on distance from camera to center of sprite
    csBox3 obox;
    GetObjectBoundingBox (obox);
    csVector3 obj_center = (obox.Min () + obox.Max ()) / 2;
    csVector3 wor_center;
    if (movable->IsFullTransformIdentity ())
      wor_center = obj_center;
    else
      wor_center = movable->GetFullTransform ().This2Other (obj_center);
    csVector3 cam_origin = camera->GetTransform ().GetOrigin ();
    float wor_sq_dist = csSquaredDist::PointPoint (cam_origin, wor_center);
    level_of_detail = GetLodLevel (qsqrt (wor_sq_dist));

    // reduce LOD based on field-of-view
    float aspect = 2 * (float) tan (camera->GetFOVAngle () * PI / 360);
    level_of_detail *= aspect;

    if (level_of_detail < 0) level_of_detail = 0;
    else if (level_of_detail >= 1) level_of_detail = 1;
  }

  if (level_of_detail < 1)
  {
    // We calculate the number of vertices to use for this LOD
    // level. The integer part will be the number of vertices.
    // The fractional part will determine how much to morph
    // between the new vertex and the previous last vertex.
    fnum = level_of_detail * (factory->GetVertexCount() + 1);
    num_verts_for_lod = (int)fnum;
    fnum -= num_verts_for_lod;  // fnum is now the fractional part.

    GenerateSpriteLOD (num_verts_for_lod);
    emerge_from = factory->GetEmergeFrom ();
    m = mesh;
  }
  else
  {
    num_verts_for_lod = factory->GetVertexCount ();
    m = factory->GetTexelMesh ();
  }

  if (num_verts_for_lod <= 1) return false;

  int i;

  csVector2* real_uv_verts;
  // Do vertex morphing if needed.
  //
  // @@@ Don't understand this piece of code.
  //   Why is it checking if the level == 0, and negative?  neg is supposed
  //    to be off.  zero is a valid on number...???
  if (level_of_detail <= 0 || level_of_detail >= 1)
  {
    real_uv_verts = factory->GetTexels (cf_idx);
  }
  else
  {
    for (i = 0 ; i < num_verts_for_lod ; i++)
    {
      csVector2 uv;
      if (i < num_verts_for_lod-1)
      {
        uv = factory->GetTexel (cf_idx, i);
      }
      else
      {
        // Morph between the last vertex and the one we morphed from.
        uv = (1-fnum) * factory->GetTexel (cf_idx, emerge_from[i])
          + fnum * factory->GetTexel (cf_idx, i);
      }

      (*uv_verts)[i] = uv;
    }
    real_uv_verts = uv_verts->GetArray ();
  }

  // Setup the structure for DrawTriangleMesh.
  if (force_otherskin)
  {
#ifndef CS_USE_NEW_RENDERER
    g3dmesh.mat_handle = cstxt->GetMaterialHandle ();
#else
    rendermesh.material = cstxt;
#endif // CS_USE_NEW_RENDERER
    cstxt->Visit ();
  }
  else
  {
#ifndef CS_USE_NEW_RENDERER
    g3dmesh.mat_handle = factory->cstxt->GetMaterialHandle ();
#else
    rendermesh.material = factory->cstxt;
#endif // CS_USE_NEW_RENDERER
    factory->cstxt->Visit ();
  }
  if (!vertex_colors) AddVertexColor (0, csColor (0, 0, 0));

#ifndef CS_USE_NEW_RENDERER
  vbuf_verts = verts;
  vbuf_texels = real_uv_verts;
  vbuf_colors = vertex_colors;
  vbuf_num_vertices = num_verts_for_lod;

  // @@@ The priority of the vertex buffer should be a parameter for the sprite.
  if (!vbuf)
  {
    // we need that to remove us from the manager later on. Bad we have no
    // object registry at hand
    vbufmgr = g3d->GetVertexBufferManager ();
    vbuf = vbufmgr->CreateBuffer (1);
    vbufmgr->AddClient (&scfiVertexBufferManagerClient);
  }

  g3dmesh.buffers[0] = vbuf;
  if (do_tween)
  {
    vbuf_tween_verts = real_tween_verts;
    vbuf_tween_texels = real_uv_verts;
    vbuf_tween_colors = vertex_colors;
    if (!vbuf_tween)
      vbuf_tween = vbufmgr->CreateBuffer (0);
    g3dmesh.buffers[1] = vbuf_tween;
    g3dmesh.morph_factor = tween_ratio;
    g3dmesh.num_vertices_pool = 2;
  }
  else
  {
    vbuf_tween_verts = 0;
    g3dmesh.morph_factor = 0;
    g3dmesh.num_vertices_pool = 1;
  }

  g3dmesh.num_triangles = m->GetTriangleCount ();
  g3dmesh.triangles = m->GetTriangles ();

  g3dmesh.use_vertex_color = !!vertex_colors;
  g3dmesh.clip_portal = clip_portal;
  g3dmesh.clip_plane = clip_plane;
  g3dmesh.clip_z_plane = clip_z_plane;
  g3dmesh.do_mirror = camera->IsMirrored ();
  g3dmesh.do_morph_texels = false;
  g3dmesh.do_morph_colors = false;
  g3dmesh.vertex_fog = fog_verts->GetArray ();

  if (skeleton_state)
    g3dmesh.vertex_mode = G3DTriangleMesh::VM_VIEWSPACE;
  else
    g3dmesh.vertex_mode = G3DTriangleMesh::VM_WORLDSPACE;
  g3dmesh.mixmode = MixMode | (vertex_colors ? CS_FX_GOURAUD : 0);

#else
  final_verts = verts;
  final_texcoords = real_uv_verts;
  final_colors = vertex_colors;
  final_triangles = m->GetTriangles ();
  final_num_vertices = num_verts_for_lod;
  final_num_triangles = m->GetTriangleCount ();

  rendermesh.clip_portal = clip_portal;
  rendermesh.clip_plane = clip_plane;
  rendermesh.clip_z_plane = clip_z_plane;
  rendermesh.do_mirror = camera->IsMirrored ();

#endif // CS_USE_NEW_RENDERER
  return true;
}

bool csSprite3DMeshObject::Draw (iRenderView* rview, iMovable* /*movable*/,
	csZBufMode mode)
{
  //@@@if (rview.callback)
    //rview.callback (&rview, CALLBACK_MESH, (void*)&g3dmesh);
  //else
#ifndef CS_USE_NEW_RENDERER
  iGraphics3D* g3d = rview->GetGraphics3D ();
  iVertexBufferManager* vbufmgr = g3d->GetVertexBufferManager ();

  CS_ASSERT (!vbuf->IsLocked ());
  csBox3 bbox;
  GetObjectBoundingBox (bbox);
  vbufmgr->LockBuffer (vbuf, vbuf_verts, vbuf_texels,
  	vbuf_colors, vbuf_num_vertices, 0, bbox);
  if (vbuf_tween_verts)
  {
    CS_ASSERT (!vbuf_tween->IsLocked ());
    vbufmgr->LockBuffer (vbuf_tween, vbuf_tween_verts,
    	vbuf_tween_texels, vbuf_tween_colors, vbuf_num_vertices, 0, bbox);
  }

  g3d->SetRenderState (G3DRENDERSTATE_ZBUFFERMODE, mode);
  rview->CalculateFogMesh (g3d->GetObjectToCamera (), g3dmesh);
  g3d->DrawTriangleMesh (g3dmesh);

  CS_ASSERT (vbuf->IsLocked ());
  vbufmgr->UnlockBuffer (vbuf);
  if (vbuf_tween_verts)
  {
    CS_ASSERT (vbuf_tween->IsLocked ());
    vbufmgr->UnlockBuffer (vbuf_tween);
  }
#endif // CS_USE_NEW_RENDERER

  if (vis_cb) if (!vis_cb->BeforeDrawing (this, rview)) return false;
  return true;
}

csRenderMesh** csSprite3DMeshObject::GetRenderMeshes (int& n)
{
#ifdef CS_USE_NEW_RENDERER
  {
    csShaderVariable* sv;
    sv = dynDomain.GetVariableAdd (vertices_name);
    sv->SetValue (GetRenderBuffer (vertices_name));
    sv = dynDomain.GetVariableAdd (normals_name);
    sv->SetValue (GetRenderBuffer (normals_name));
    sv = dynDomain.GetVariableAdd (texcoords_name);
    sv->SetValue (GetRenderBuffer (texcoords_name));
    sv = dynDomain.GetVariableAdd (colors_name);
    sv->SetValue (GetRenderBuffer (colors_name));
    sv = dynDomain.GetVariableAdd (indices_name);
    sv->SetValue (GetRenderBuffer (indices_name));
  }
  n = 1;
  if (skeleton_state) 
  {
    /* set to identify for software skeleton */
    rendermesh.object2camera = csReversibleTransform ();
  }
  rendermesh.z_buf_mode = CS_ZBUF_TEST;
  rendermesh.mixmode = CS_FX_COPY;
  rendermesh.indexstart = 0;
  rendermesh.indexend = final_num_triangles * 3;
  
  rendermesh.dynDomain = &dynDomain;
  rendermesh.meshtype = CS_MESHTYPE_TRIANGLES;
  meshptr = &rendermesh;
  return &meshptr;
#else
  n = 0;
  return 0;
#endif
}

void csSprite3DMeshObject::InitSprite ()
{
  if (!factory)
  {
    factory->Report (CS_REPORTER_SEVERITY_ERROR,
    	"There is no defined template for this sprite!");
    return;
  }

  if (!cur_action) { SetFrame (0); cur_action = factory->GetFirstAction (); }

  last_time = factory->vc->GetCurrentTicks ();
  last_pos = csVector3(0,0,0);
  last_displacement = 0;
}


bool csSprite3DMeshObject::OldNextFrame (csTicks current_time,
	const csVector3& cur_pos, bool onestep, bool stoptoend)
{
  bool ret = false;

  /**
   * First check if the animation is over.  If it is over, there
   * is no updating to do unless another animation was overridden
   * by this one, in which case we need to restore that one to
   * be active again.
   */ 
  if (fullstop)
  {
    if (!last_action)  // no overridden action to revert to
      return true;
    else
    {
      // Restore overridden action
      SetAction(last_action->GetName(),last_loop,last_speed);
      SetReverseAction(last_reverse);
      last_action = 0;
      return true;
    }
  }

  /**
   * Here we know the animation is active.  There are two ways frames
   * get advanced.  The first is the "onestep" way, which ensures that
   * no keyframes are skipped, no matter how slow the framerate.  The
   * second method updates to whatever is the most accurate frame based
   * on either the timing or position, even if multiple keyframes are
   * skipped.  This is much more accurate, and should always be used
   * unless you have a specific effect you need.
   */

  // Calculate distance moved by sprite since last frame drawn.
  float cur_displacement = qsqrt (csSquaredDist::PointPoint (last_pos, cur_pos));
  last_pos = cur_pos;

  // If the sprite has only one frame we disable tweening here.
  bool do_tween = do_tweening;
  if (cur_action->GetFrameCount () <= 1) do_tween = false;

  // Single-step mode starts here
  if (onestep)
  {
    /**
     * This if statement says that if we have a time delay frame
     * and enough time has passed, or if we have a displacement frame
     * and the sprite has moved far enough, then advance the anim
     * one frame.  (Note "advance" could mean go backwards also.)
     */
    if (((cur_action->GetFrameDelay (cur_frame) ) &&
	 (current_time > last_time+
    	   cur_action->GetFrameDelay (cur_frame)/speedfactor)) || 
	((cur_action->GetFrameDisplacement(cur_frame) &&
	 (cur_displacement + last_displacement >
	       cur_action->GetFrameDisplacement (cur_frame)/speedfactor)))
	)
    {
      last_time = current_time;
      last_displacement = cur_displacement + last_displacement -
	          cur_action->GetFrameDisplacement (cur_frame)/speedfactor;
      cur_frame+=frame_increment;
      
      // Reset frame to end of anim if advancing in reverse
      if (cur_frame<0)
	  cur_frame=cur_action->GetFrameCount() - 1;

      // Now check for end of non-looping animation
      if (stoptoend && cur_frame + 1 >= cur_action->GetFrameCount ())
      {
        ret = true;
        fullstop = true;
      }
      else if(cur_frame >= cur_action->GetFrameCount ())
      {
        cur_frame = 0;
        ret = true;
        fullstop = true;
      }
    }
  }
  else  // not in single frame advance mode
  {
    /**
     * Here it is the same basic process, but we loop because more than
     * one key frame may have elapsed since the last displayed frame.
     */

    // Update the distance moved to include any unused distance from before.
    cur_displacement += last_displacement;  // include partial from last frame

    while (1)
    {
      // If the current frame is a time-delayed frame, 
      if (cur_action->GetFrameDelay(cur_frame))
      {
        // If enough time has gone by to switch to the next frame
        if (current_time > last_time+
      	   cur_action->GetFrameDelay (cur_frame)/speedfactor)
        {
	  /**
	   * Add the time taken by this frame to the last keyframe time used.
	   * We have to track this because even with advancing this frame,
	   * we still may have excess elapsed time and may need to advance
	   * another frame, or the excess will affect our tweening between
	   * key frames.
	   */
          last_time += csTicks(cur_action->GetFrameDelay (cur_frame)/speedfactor);
          cur_frame+=frame_increment;
	  cur_displacement = 0;

	  // Make sure to wrap the counter around to implement the looping
	  // Note: bidirectional in case of reversed actions
          if (cur_frame >= cur_action->GetFrameCount ())
          {
            cur_frame = 0;
            ret = true;
          }
	  else if (cur_frame < 0)
	  {
	    cur_frame = cur_action->GetFrameCount() - 1;
	    ret = true;
	  }
        }
        else 
	{
	  /**
	   * This break means that not enough time has gone by to advance
	   * another frame, so the loop is done.  Excess time will be
	   * accommodated for by the tweening done below.
	   */
	  break;
	}
      }
      else  // distance based frame delays handled below
      {
        /**
	 * If the sprite has moved in space more than the displacement
	 * of the keyframe, then we need to advance key frames.
	 */
        if (cur_displacement > 
      	     cur_action->GetFrameDisplacement (cur_frame)/speedfactor)
        {
	  /**
	   * Again we need to track how much displacement is used up
	   * by this frame, so we subtract instead, in case we need
	   * to advance again next time through the loop.
	   */
          cur_displacement -= cur_action->GetFrameDisplacement (cur_frame)/speedfactor;
          cur_frame+=frame_increment;

	  // Again loop the frame counter around if necessary
          if (cur_frame >= cur_action->GetFrameCount ())
          {
            cur_frame = 0;
            ret = true;
          }
	  else if (cur_frame < 0)
	  {
	    cur_frame = cur_action->GetFrameCount() - 1;
	    ret = true;
	  }
        }
        else
	{
	  /* 
	   * Remember how much displacement we've had in total since last key frame switch
	   */
          last_displacement = cur_displacement;
	  break;
	}
      }
    }

    // Now that we have advanced enough, see if we have exhausted
    // a non-looping anim and flag it for next time if so.
    if (!loopaction)
    {
      if (cur_frame + 1 >= cur_action->GetFrameCount () )
      {
	  fullstop = true;
      }
    }
  }

  /**
   * Ok now regardless how which method was used, or how many frames
   * were advanced (including zero), we will probably have to interpolate
   * between keyframes.  This section determines the % interpolation
   * based on the excess time or excess displacement.  The Draw code
   * actually does the tweening of the vertices.
   */
  if (do_tween)
  {
    if (cur_action->GetFrameDelay(cur_frame))
    {
      if (current_time <= last_time) tween_ratio = 0;
      else tween_ratio = (current_time - last_time)
        / float (cur_action->GetFrameDelay (cur_frame)/speedfactor);
    }
    else
    {
      if (cur_displacement <= 0) tween_ratio = 0;
      else tween_ratio = cur_displacement /
        (cur_action->GetFrameDisplacement (cur_frame)/speedfactor);
      last_time = current_time; // probably not 100% accurate
    }
  }
  else
  {
    last_time = current_time;
    tween_ratio = 0;
  }
//  printf("Disp: %1.4f Frame #%d TweenDisp:%1.4f  Tween:%1.4f\n",cur_displacement,cur_frame,last_displacement,tween_ratio);

  return ret;
}

csVector3* csSprite3DMeshObject::GetObjectVerts (csSpriteFrame* fr)
{
  UpdateWorkTables (factory->GetVertexCount ());
  int fr_idx = fr->GetAnmIndex();

  int i;
  for (i = 0; i < factory->GetVertexCount (); i++)
    (*obj_verts)[i] = factory->GetVertex(fr_idx, i);

  if (skeleton_state)
  {
    UpdateWorkTables (factory->GetVertexCount());
    skeleton_state->Transform (csTransform (), obj_verts->GetArray (),
    	tr_verts->GetArray ());
    return tr_verts->GetArray ();
  }
  else
    return obj_verts->GetArray ();
}

void csSprite3DMeshObject::UpdateLighting (iLight** lights, int num_lights,
	iMovable* movable)
{
  SetupObject ();

  // Make sure that the color array is initialized.
  AddVertexColor (0, csColor (0, 0, 0));

  if (!do_lighting) return;

  // Make sure the normals are computed
  factory->ComputeNormals (cur_action->GetCsFrame (cur_frame));
  if (tween_ratio && GetLightingQuality() != CS_SPR_LIGHTING_FAST)
    factory->ComputeNormals (cur_action->GetCsNextFrame (cur_frame));

  if (GetLightingQuality() == CS_SPR_LIGHTING_LQ ||
      GetLightingQuality() == CS_SPR_LIGHTING_HQ )
  {
    int num_texels = factory->GetVertexCount();
    // Reseting all of the vertex_colors to the base color.
    csColor col;
    if (factory->engine)
    {
      factory->engine->GetAmbientLight (col);
      col += base_color;
      iSector* sect = movable->GetSectors ()->Get (0);
      if (sect)
        col += sect->GetDynamicAmbientLight ();
    }
    else
    {
      col = base_color;
    }

    int i;
    for (i = 0 ; i < num_texels; i++)
      vertex_colors [i] = col;
  }

// @@@
// NOTE: lighting fast does not need to reset the vertex colors, it does this.
//
//  else
//    ResetVertexColors();

  switch (GetLightingQuality())
  {
    case CS_SPR_LIGHTING_HQ:   UpdateLightingHQ   (lights, num_lights, movable); break;
    case CS_SPR_LIGHTING_LQ:   UpdateLightingLQ   (lights, num_lights, movable); break;
    case CS_SPR_LIGHTING_FAST: UpdateLightingFast (lights, num_lights, movable); break;
    case CS_SPR_LIGHTING_RANDOM: UpdateLightingRandom (); break;
  }

  // @@@ TODO: Make FixVertexColors an option.
  // I would like lighting fast to not bother clamping the colors.
  //   Could we instead put some debug code in lighting fast to check if
  //    in the application programmers app that the colors don't go
  //    over 2.0?
  FixVertexColors ();  // Clamp all vertex colors to 2.0
}


void csSprite3DMeshObject::UpdateLightingRandom ()
{
//  int num_texels = factory->GetVertexCount();
  int num_texels = GetVertexToLightCount();
  float r,g,b;

  int i;
  for (i = 0; i < num_texels; i++)
  {
    // By seeding the rng with the current time each time, we get a slower
    //  cycling of colors.
    // rand_num->Initialize()
    r = rand_num->Get()*2;
    g = rand_num->Get()*2;
    b = rand_num->Get()*2;

    vertex_colors[i].Set(r,g,b);
  }
}




void csSprite3DMeshObject::UpdateLightingFast (iLight** lights, int num_lights,
	iMovable* movable)
{
  int light_num, j;

  float cosinus;
  //int num_texels = factory->GetVertexCount();
  int num_texels = GetVertexToLightCount();

  float light_bright_wor_dist;

  // convert frame number in current action to absolute frame number
  int tf_idx = cur_action->GetCsFrame (cur_frame)->GetAnmIndex();

  csBox3 obox;
  GetObjectBoundingBox (obox);
  csVector3 obj_center = (obox.Min () + obox.Max ()) / 2;
  csVector3 wor_center;
  csReversibleTransform movtrans;
  bool identity = movable->IsFullTransformIdentity ();
  if (identity)
    wor_center = obj_center;
  else
  {
    movtrans = movable->GetFullTransform ();
    wor_center = movtrans.This2Other (obj_center);
  }
  csColor color;

  csColor light_color;
  float sq_light_radius;
  float cosinus_light;
  float light_color_r;
  float light_color_g;
  float light_color_b;

#if 0
  // ambient colors.
  int r, g, b;

  //@@@GetAmbientColor (r, g, b);
  r = g = b = 0;
  //@@@@@
  float amb_r = r / 128.0;
  float amb_g = g / 128.0;
  float amb_b = b / 128.0;
#endif

  for (light_num = 0 ; light_num < num_lights ; light_num++)
  {
    light_color = lights [light_num]->GetColor () * (256. / CS_NORMAL_LIGHT_LEVEL);
    sq_light_radius = lights [light_num]->GetInfluenceRadiusSq ();

    // Compute light position in object coordinates
    csVector3 wor_light_pos = lights [light_num]->GetCenter ();
    float wor_sq_dist = csSquaredDist::PointPoint (wor_light_pos, wor_center);
    if (wor_sq_dist >= sq_light_radius) continue;

    csVector3 obj_light_pos;
    if (identity)
      obj_light_pos = wor_light_pos;
    else
      obj_light_pos = movtrans.Other2This (wor_light_pos);
    float obj_sq_dist = csSquaredDist::PointPoint (obj_light_pos, obj_center);
    float inv_obj_dist = qisqrt (obj_sq_dist);
    float wor_dist = qsqrt (wor_sq_dist);

    csVector3 obj_light_dir = (obj_light_pos - obj_center);
    light_bright_wor_dist = lights[light_num]->GetBrightnessAtDistance (wor_dist);

    color = light_color;

    // This part of code will hardly ever be called.
    if (obj_sq_dist < SMALL_EPSILON)
    {
      for (j = 0 ; j < num_texels ; j++)
      {
        // TODO: need to add the ambient color here.
        AddVertexColor (j, color);
      }
      continue;
    }

    light_color_r = light_color.red;
    light_color_g = light_color.green;
    light_color_b = light_color.blue;

    // NOTE: Doing this to get rid of a divide in the loop.
    //obj_dist = 1 / obj_dist;

    // The first light should have the base color added.
    if(light_num == 0)
    {
      for (j = 0 ; j < num_texels ; j++)
      {
        // this obj_dist is not the obj_dist, see NOTE above.
        cosinus = (obj_light_dir * factory->GetNormal (tf_idx, j)) * inv_obj_dist;
        cosinus_light = (cosinus * light_bright_wor_dist);
        vertex_colors[j].Set(light_color_r * cosinus_light + base_color.red,
                             light_color_g * cosinus_light + base_color.green,
                             light_color_b * cosinus_light + base_color.blue);
      }
    }
    else  // The next lights should have the light color added.
    {
      for (j = 0 ; j < num_texels ; j++)
      {
        // this obj_dist is not the obj_dist, see NOTE above.
        cosinus = (obj_light_dir * factory->GetNormal (tf_idx, j)) * inv_obj_dist;
        cosinus_light = (cosinus * light_bright_wor_dist);
        vertex_colors[j].Add(light_color_r * cosinus_light,
                             light_color_g * cosinus_light,
                             light_color_b * cosinus_light);
      }
    }
  } // end of light loop.
}


void csSprite3DMeshObject::UpdateLightingLQ (iLight** lights, int num_lights,
	iMovable* movable)
{
  int i, j;

  //int num_texels = factory->GetVertexCount ();
  int num_texels = GetVertexToLightCount();

  float remainder = 1 - tween_ratio;

  // convert frame number in current action to absolute frame number
  int tf_idx = cur_action->GetCsFrame     (cur_frame)->GetAnmIndex ();
  int nf_idx = cur_action->GetCsNextFrame (cur_frame)->GetAnmIndex ();

  csBox3 obox;
  GetObjectBoundingBox (obox);
  csVector3 obj_center = (obox.Min () + obox.Max ()) / 2;
  csVector3 wor_center;
  csReversibleTransform movtrans;
  bool identity = movable->IsFullTransformIdentity ();
  if (identity)
    wor_center = obj_center;
  else
  {
    movtrans = movable->GetFullTransform ();
    wor_center = movtrans.This2Other (obj_center);
  }
  csColor color;

  for (i = 0 ; i < num_lights ; i++)
  {
    // Compute light position in object coordinates
    csVector3 wor_light_pos = lights [i]->GetCenter ();
    float wor_sq_dist = csSquaredDist::PointPoint (wor_light_pos, wor_center);
    if (wor_sq_dist >= lights[i]->GetInfluenceRadiusSq ()) continue;

    csVector3 obj_light_pos;
    if (identity)
      obj_light_pos = wor_light_pos;
    else
      obj_light_pos = movtrans.Other2This (wor_light_pos);
    float obj_sq_dist = csSquaredDist::PointPoint (obj_light_pos, obj_center);

    float in_obj_dist = 0.0f;

    // FIX: Check for divide by zero
    if (obj_sq_dist > SMALL_EPSILON)
      in_obj_dist = qisqrt (obj_sq_dist);

    csVector3 obj_light_dir = (obj_light_pos - obj_center);

    csColor light_color = lights[i]->GetColor () * (256. / CS_NORMAL_LIGHT_LEVEL)
      * lights[i]->GetBrightnessAtDistance (qsqrt (wor_sq_dist));

    for (j = 0 ; j < num_texels ; j++)
    {
      csVector3 normal = factory->GetNormal (tf_idx, j);
      if (tween_ratio)
      {
        normal = remainder * normal + tween_ratio * factory->GetNormal (nf_idx, j);
	float norm = normal.Norm ();
	if (ABS (norm) > SMALL_EPSILON)
          normal /= norm;
      }

      float cosinus;
      if (obj_sq_dist < SMALL_EPSILON) cosinus = 1;
      else cosinus = obj_light_dir * normal;

      if (cosinus > 0)
      {
        color = light_color;
        if (obj_sq_dist >= SMALL_EPSILON) cosinus *= in_obj_dist;
        if (cosinus < 1) color *= cosinus;
        AddVertexColor (j, color);
      }
    }
  }
}

void csSprite3DMeshObject::UpdateLightingHQ (iLight** lights, int num_lights,
	iMovable* movable)
{
  int i, j;

  // convert frame number in current action to absolute frame number
  int tf_idx = cur_action->GetCsFrame     (cur_frame)->GetAnmIndex ();
  int nf_idx = cur_action->GetCsNextFrame (cur_frame)->GetAnmIndex ();

  float remainder = 1 - tween_ratio;
//  int num_texels = factory->GetVertexCount ();
  int num_texels = GetVertexToLightCount();

  // need vertices to calculate distance from light to each vertex
  csVector3* object_vertices;
  if (tween_ratio)
  {
    UpdateWorkTables (num_texels); // make room in obj_verts;

    for (i = 0 ; i < num_texels ; i++)
      (*obj_verts)[i] = tween_ratio * factory->GetVertex (tf_idx, i)
                        + remainder   * factory->GetVertex (nf_idx, i);

    object_vertices = obj_verts->GetArray ();
  }
  else
    object_vertices = GetObjectVerts (cur_action->GetCsFrame (cur_frame));

  csColor color;

  csReversibleTransform movtrans = movable->GetFullTransform ();
  for (i = 0 ; i < num_lights ; i++)
  {
    csColor light_color = lights [i]->GetColor () * (256. / CS_NORMAL_LIGHT_LEVEL);
    float sq_light_radius = lights [i]->GetInfluenceRadiusSq ();

    // Compute light position in object coordinates
    csVector3 wor_light_pos = lights [i]->GetCenter ();
    csVector3 obj_light_pos = movtrans.Other2This (wor_light_pos);

    for (j = 0 ; j < num_texels ; j++)
    {
      csVector3& obj_vertex = object_vertices[j];
      csVector3 wor_vertex = movtrans.This2Other (obj_vertex);

      // @@@ We have the distance in object space. Can't we use
      // that to calculate the distance in world space as well?
      // These calculations aren't optimal. I have the feeling they
      // can be optimized somewhat.
      float obj_sq_dist = csSquaredDist::PointPoint (obj_light_pos, obj_vertex);
      float wor_sq_dist = csSquaredDist::PointPoint (wor_light_pos, wor_vertex);
      float obj_dist = qsqrt (obj_sq_dist);

      csVector3 normal = factory->GetNormal (tf_idx, j);
      if (tween_ratio)
      {
        normal = remainder * normal + tween_ratio *
		factory->GetNormal (nf_idx, j);
	float norm = normal.Norm ();
	if (ABS (norm) > SMALL_EPSILON)
          normal /= norm;
      }

      float cosinus;
      if (obj_sq_dist < SMALL_EPSILON) cosinus = 1;
      else cosinus = (obj_light_pos - obj_vertex) * normal;

      if ((cosinus > 0) && (wor_sq_dist < sq_light_radius))
      {
        color = light_color;
        if (obj_sq_dist >= SMALL_EPSILON) cosinus /= obj_dist;
        if (cosinus < 1) color *= cosinus *
		lights[i]->GetBrightnessAtDistance (obj_dist);
        AddVertexColor (j, color);
      }
    }
  }
}

bool csSprite3DMeshObject::HitBeamOutline (const csVector3& start,
	const csVector3& end, csVector3& isect, float* pr)
{
  // @@@ We might consider checking to a lower LOD version only.
  csSegment3 seg (start, end);
  csSpriteFrame* cframe = cur_action->GetCsFrame (cur_frame);
  csVector3* verts = GetObjectVerts (cframe);
  csTriangle* tris = factory->GetTriangles ();
  int i;
  for (i = 0 ; i < factory->GetTriangleCount () ; i++)
  {
    csTriangle& tr = tris[i];
    if (csIntersect3::IntersectTriangle (verts[tr.a], verts[tr.b],
    	verts[tr.c], seg, isect))
    {
      if (pr) *pr = qsqrt (csSquaredDist::PointPoint (start, isect) /
	                     csSquaredDist::PointPoint (start, end));
      return true;
    }
  }
  return false;
}

bool csSprite3DMeshObject::HitBeamObject (const csVector3& start,
	const csVector3& end, csVector3& isect, float* pr, int* polygon_idx)
{
  if (polygon_idx) *polygon_idx = -1;
  // This routine is slow, but it is intended to be accurate.

  csSegment3 seg (start, end);
  csSpriteFrame* cframe = cur_action->GetCsFrame (cur_frame);
  csVector3* verts = GetObjectVerts (cframe), tsect;
  csTriangle* tris = factory->GetTriangles ();
  int i;
  float dist, temp, max;
  temp = dist = max = csSquaredDist::PointPoint (start, end);
  for (i = 0 ; i < factory->GetTriangleCount () ; i++)
  {
    csTriangle& tr = tris[i];
    if (csIntersect3::IntersectTriangle (verts[tr.a], verts[tr.b],
    	verts[tr.c], seg, tsect))
    {
      temp = csSquaredDist::PointPoint (start, tsect);
      if (temp < dist)
      {
        dist = temp;
	isect = tsect;
      }
    }
  }
  if (pr) *pr = qsqrt (dist / max);
  if (dist >= max) return false;
  return true;
}

//--------------------------------------------------------------------------

csMeshedPolygon* csSprite3DMeshObject::PolyMesh::GetPolygons ()
{
  if (!polygons)
  {
    csSprite3DMeshObjectFactory* tmpl = scfParent->GetFactory3D ();
    csTriangle* triangles = tmpl->GetTriangles ();
    polygons = new csMeshedPolygon [GetPolygonCount ()];
    int i;
    for (i = 0 ; i < GetPolygonCount () ; i++)
    {
      polygons[i].num_vertices = 3;
      polygons[i].vertices = &triangles[i].a;
    }
  }
  return polygons;
}

iSkeletonState* csSprite3DMeshObject::Sprite3DState::GetSkeletonState () const
{
  csRef<iSkeletonState> iskelstate (
  	SCF_QUERY_INTERFACE_SAFE (scfParent->GetSkeletonState (),
	iSkeletonState));
  return iskelstate;	// DecRef is ok here.
}

#ifndef CS_USE_NEW_RENDERER
void csSprite3DMeshObject::eiVertexBufferManagerClient::ManagerClosing ()
{
  scfParent->vbuf = scfParent->vbuf_tween = 0;
  scfParent->vbufmgr = 0;
}
#else
iRenderBuffer *csSprite3DMeshObject::GetRenderBuffer (csStringID name)
{
  if (name == vertices_name)
  {
    if (!vertices)
    {
      vertices = factory->g3d->CreateRenderBuffer (
        sizeof (csVector3)*final_num_vertices, CS_BUF_DYNAMIC,
		CS_BUFCOMP_FLOAT, 3, false);
    }
    vertices->CopyToBuffer(final_verts, sizeof (csVector3)*final_num_vertices);
    return vertices;
  }
  if (name == normals_name)
  {
    if (!normals)
    {
      normals = factory->g3d->CreateRenderBuffer (
        sizeof (csVector3)*final_num_vertices, CS_BUF_DYNAMIC,
		CS_BUFCOMP_FLOAT, 3, false);
    }
    /*int tf_idx = cur_action->GetCsFrame (cur_frame)->GetAnmIndex ();
    factory->ComputeNormals (cur_action->GetCsFrame (cur_frame));*/
    csVector3* norm = new csVector3[final_num_vertices];
    memset (norm, 0, sizeof(csVector3)*final_num_vertices);
    int i;
    for (i=0; i<final_num_triangles; i++)
    {
      csVector3 ab = final_verts [final_triangles[i].b] 
        - final_verts [final_triangles[i].a];
      csVector3 bc = final_verts [final_triangles[i].c] 
        - final_verts [final_triangles[i].b];
      csVector3 normal = ab % bc;
      norm[final_triangles[i].a] += normal;
      norm[final_triangles[i].b] += normal;
      norm[final_triangles[i].c] += normal;
    }
    normals->CopyToBuffer (norm, sizeof (csVector3)*final_num_vertices);
    delete[] norm;
    return normals;
  }
  if (name == texcoords_name)
  {
    if (!texcoords)
    {
      texcoords = factory->g3d->CreateRenderBuffer (
        sizeof (csVector2)*final_num_vertices, CS_BUF_DYNAMIC,
		    CS_BUFCOMP_FLOAT, 2, false);
    }
    texcoords->CopyToBuffer (final_texcoords, sizeof (csVector2)*final_num_vertices);
    texcoords->Release ();
    return texcoords;
  }
  if (name == colors_name)
  {
    if (!colors)
    {
      colors = factory->g3d->CreateRenderBuffer (
        sizeof (csColor)*final_num_vertices, CS_BUF_DYNAMIC,
		    CS_BUFCOMP_FLOAT, 3, false);
    }
    colors->CopyToBuffer (final_colors, sizeof (csColor)*final_num_vertices);
    return colors;
  }
  if (name == indices_name)
  {
    if (!indices)
    {
      indices = factory->g3d->CreateRenderBuffer (
        sizeof (csTriangle)*final_num_triangles*12, CS_BUF_STATIC,
		    CS_BUFCOMP_UNSIGNED_INT, 1, true);
    }
    indices->CopyToBuffer (final_triangles, sizeof (csTriangle)*final_num_triangles);
    return indices;
  }
  return 0;
}

#endif // CS_USE_NEW_RENDERER

//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csSprite3DMeshObjectType)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectType)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iConfig)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iLODControl)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite3DMeshObjectType::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite3DMeshObjectType::csSprite3DConfig)
  SCF_IMPLEMENTS_INTERFACE (iConfig)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite3DMeshObjectType::LODControl)
  SCF_IMPLEMENTS_INTERFACE (iLODControl)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csSprite3DMeshObjectType)

csSprite3DMeshObjectType::csSprite3DMeshObjectType (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiConfig);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiLODControl);
}

csSprite3DMeshObjectType::~csSprite3DMeshObjectType ()
{
}

bool csSprite3DMeshObjectType::Initialize (iObjectRegistry* object_reg)
{
  csSprite3DMeshObjectType::object_reg = object_reg;
  vc = CS_QUERY_REGISTRY (object_reg, iVirtualClock);
  csRef<iEngine> eng = CS_QUERY_REGISTRY (object_reg, iEngine);
  // We don't want to keep a reference to the engine (circular ref otherwise).
  engine = eng;
  return true;
}

csPtr<iMeshObjectFactory> csSprite3DMeshObjectType::NewFactory ()
{
  csSprite3DMeshObjectFactory* cm = new csSprite3DMeshObjectFactory (this);
  cm->object_reg = object_reg;
  cm->vc = vc;
  cm->engine = engine;
#ifdef CS_USE_NEW_RENDERER
  cm->g3d = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
  cm->anon_buffers = new csAnonRenderBufferManager (object_reg);
#endif
  csRef<iMeshObjectFactory> ifact (
  	SCF_QUERY_INTERFACE (cm, iMeshObjectFactory));
  cm->DecRef ();
  return csPtr<iMeshObjectFactory> (ifact);
}

#define NUM_OPTIONS 3

static const csOptionDescription config_options [NUM_OPTIONS] =
{
  { 0, "sprlod_m", "Sprite LOD Level (m factor)", CSVAR_FLOAT },
  { 1, "sprlod_a", "Sprite LOD Level (a factor)", CSVAR_FLOAT },
  { 2, "sprlq", "Sprite Lighting Quality", CSVAR_LONG },
};

bool csSprite3DMeshObjectType::csSprite3DConfig::SetOption (int id,
	csVariant* value)
{
  if (value->GetType () != config_options[id].type)
    return false;
  switch (id)
  {
    case 0: csSprite3DMeshObject::global_lod_m = value->GetFloat (); break;
    case 1: csSprite3DMeshObject::global_lod_a = value->GetFloat (); break;
    case 2: csSprite3DMeshObject::global_lighting_quality = value->GetLong ();
    	break;
    default: return false;
  }
  return true;
}

bool csSprite3DMeshObjectType::csSprite3DConfig::GetOption (int id,
	csVariant* value)
{
  switch (id)
  {
    case 0: value->SetFloat (csSprite3DMeshObject::global_lod_m); break;
    case 1: value->SetFloat (csSprite3DMeshObject::global_lod_a); break;
    case 2: value->SetLong (csSprite3DMeshObject::global_lighting_quality);
    	break;
    default: return false;
  }
  return true;
}

bool csSprite3DMeshObjectType::csSprite3DConfig::GetOptionDescription
  (int idx, csOptionDescription* option)
{
  if (idx < 0 || idx >= NUM_OPTIONS)
    return false;
  *option = config_options[idx];
  return true;
}


