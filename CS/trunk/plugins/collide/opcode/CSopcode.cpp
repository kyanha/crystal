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

/*
-------------------------------------------------------------------------
*
*           OPCODE collision detection plugin for CrystalSpace
*
*           OPCODE library was written by Pierre Terdiman
*                  ported to CS by Charles Quarra
*
-------------------------------------------------------------------------
*/

#include "cssysdef.h"
#include "CSopcode.h"
#include "csqsqrt.h"
#include "igeom/polymesh.h"
#include "csgeom/transfrm.h"
#include "csutil/scfstr.h"
#include "iutil/string.h"
#include "ivaria/reporter.h"

CS_IMPLEMENT_PLUGIN

//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csOPCODECollideSystem)
  SCF_IMPLEMENTS_INTERFACE (iCollideSystem)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csOPCODECollideSystem::Component)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csOPCODECollideSystem)

using namespace CS::Plugins::Opcode;
using namespace CS::Plugins::Opcode::Opcode;

void CS::Plugins::Opcode::Opcode_Log (const char* msg, ...)
{
  va_list args;
  va_start (args, msg);
  csOPCODECollideSystem::OpcodeReportV (CS_REPORTER_SEVERITY_NOTIFY, 
    msg, args);
  va_end (args);
}

bool CS::Plugins::Opcode::Opcode_Err (const char* msg, ...)
{
  va_list args;
  va_start (args, msg);
  // Although it's called "..._Err", Opcode also reports less-than-fatal
  // messages through it
  csOPCODECollideSystem::OpcodeReportV (CS_REPORTER_SEVERITY_WARNING, 
    msg, args);
  va_end (args);
  return false;
}

iObjectRegistry* csOPCODECollideSystem::rep_object_reg = 0;

void csOPCODECollideSystem::OpcodeReportV (int severity, const char* message, 
                                           va_list args)
{
  csReportV (rep_object_reg,
    severity, "crystalspace.collisiondetection.opcode", message, args);
}

csOPCODECollideSystem::csOPCODECollideSystem (iBase *pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent);
  TreeCollider.SetFirstContact (false);
  TreeCollider.SetFullBoxBoxTest (false);
  TreeCollider.SetFullPrimBoxTest (false);
  // TreeCollider.SetFullPrimPrimTest (true);
  TreeCollider.SetTemporalCoherence (true);

  RayCol.SetCulling (false);
}

csOPCODECollideSystem::~csOPCODECollideSystem ()
{
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiComponent);
  SCF_DESTRUCT_IBASE ();
  rep_object_reg = 0;
}

bool csOPCODECollideSystem::Initialize (iObjectRegistry* iobject_reg)
{
  object_reg = iobject_reg;
  rep_object_reg = object_reg;
  return true;
}

csPtr<iCollider> csOPCODECollideSystem::CreateCollider (iPolygonMesh* mesh)
{
  csOPCODECollider* col = new csOPCODECollider (mesh);
  // here we must store the caches (and the trees)?
  return csPtr<iCollider> (col);
}

csPtr<iCollider> csOPCODECollideSystem::CreateCollider (iTerraFormer* terraformer)
{
  csTerraFormerCollider* col = new csTerraFormerCollider (terraformer, object_reg);
  return csPtr<iCollider> (col);
}
bool csOPCODECollideSystem::TestTriangleTerraFormer (csVector3 triangle[3], 
                                                   csTerraFormerCollider* c,
                                                   csCollisionPair* pair)
{
  float height[3];
  height[0] = c->SampleFloat (triangle[0].x, triangle[0].z);
  height[1] = c->SampleFloat (triangle[1].x, triangle[1].z);
  height[2] = c->SampleFloat (triangle[2].x, triangle[2].z);
  for (int i = 0; i < 3; i++)
  {
    if (height[i] > triangle[i].y)
    {
      pair->a1 = triangle[2];
      pair->b1 = triangle[1];
      pair->c1 = triangle[0];

      pair->a2 = csVector3 (triangle[2].x, height[2], triangle[2].z);
      pair->b2 = csVector3 (triangle[1].x, height[1], triangle[1].z);
      pair->c2 = csVector3 (triangle[0].x, height[0], triangle[0].z);

      return true;
    }
  }
 return false;
}
void csOPCODECollideSystem::CopyCollisionPairs (csOPCODECollider* col1,
                                                csTerraFormerCollider* terraformer)
{
  int size = (int) (udword(TreeCollider.GetNbPairs ()));
  if (size == 0) return;
  int N_pairs = size;
  const Pair* colPairs=TreeCollider.GetPairs ();
  Point* vertholder0 = col1->vertholder;
  if (!vertholder0) return;
  Point* vertholder1 = terraformer->vertices.GetArray ();
  if (!vertholder1) return;
  udword* indexholder0 = col1->indexholder;
  if (!indexholder0) return;
    udword* indexholder1 = terraformer->indexholder;
  if (!indexholder1) return;
  Point* current;
  int i, j;

  size_t oldlen = pairs.Length ();
  pairs.SetLength (oldlen + N_pairs);

  for (i = 0 ; i < N_pairs ; i++)
  {
    j = 3 * colPairs[i].id0;
    current = &vertholder0[indexholder0[j]];		
    pairs[oldlen].a1 = csVector3 (current->x, current->y, current->z);
    current = &vertholder0[indexholder0[j + 1]];		
    pairs[oldlen].b1 = csVector3 (current->x, current->y, current->z);
    current = &vertholder0[indexholder0[j + 2]];		
    pairs[oldlen].c1 = csVector3 (current->x, current->y, current->z);

    j = 3 * colPairs[i].id1;
    current = &vertholder1[indexholder1[j]];		
    pairs[oldlen].a2 = csVector3 (current->x, current->y, current->z);
    current = &vertholder1[indexholder1[j + 1 ]];		
    pairs[oldlen].b2 = csVector3 (current->x, current->y, current->z);
    current = &vertholder1[indexholder1[j + 2 ]];		
    pairs[oldlen].c2 = csVector3 (current->x, current->y, current->z);


    oldlen++;
  }
}
bool csOPCODECollideSystem::Collide (
  csOPCODECollider* col1, const csReversibleTransform* trans1,
  csTerraFormerCollider* terraformer)
{
  ColCache.Model0 = col1->m_pCollisionModel;
  terraformer->UpdateOPCODEModel (trans1->GetOrigin (), col1->GetRadius ());
  ColCache.Model1 = terraformer->opcode_model;

  csMatrix3 m1;
  if (trans1) m1 = trans1->GetT2O ();
  csVector3 u;

  u = m1.Row1 ();
  col1->transform.m[0][0] = u.x;
  col1->transform.m[1][0] = u.y;
  col1->transform.m[2][0] = u.z;
  u = m1.Row2 ();
  col1->transform.m[0][1] = u.x;
  col1->transform.m[1][1] = u.y;
  col1->transform.m[2][1] = u.z;
  u = m1.Row3 ();
  col1->transform.m[0][2] = u.x;
  col1->transform.m[1][2] = u.y;
  col1->transform.m[2][2] = u.z;

  if (trans1) u = trans1->GetO2TTranslation ();
  else u.Set (0, 0, 0);
  col1->transform.m[3][0] = u.x;
  col1->transform.m[3][1] = u.y;
  col1->transform.m[3][2] = u.z;

  bool isOk = TreeCollider.Collide (ColCache, &col1->transform,
  	&terraformer->transform);
  if (isOk)
  {
    bool status = (TreeCollider.GetContactStatus () != FALSE);
    if (status)
    {
      CopyCollisionPairs (col1, terraformer);
    }
    return status;
  }
  else
  {
    return false;
  }
}

bool csOPCODECollideSystem::Collide (
  iCollider* collider1, const csReversibleTransform* trans1,
  iCollider* collider2, const csReversibleTransform* trans2)
{
  // csPrintf( " we are in Collide \n");
  if (collider1->GetColliderType () == CS_TERRAFORMER_COLLIDER && 
    collider2->GetColliderType () == CS_MESH_COLLIDER)
    return Collide ((csOPCODECollider*)collider2, trans2, (csTerraFormerCollider*)collider1);
    
  if (collider2->GetColliderType () == CS_TERRAFORMER_COLLIDER && 
    collider1->GetColliderType () == CS_MESH_COLLIDER)
    return Collide ((csOPCODECollider*)collider1, trans1, (csTerraFormerCollider*)collider2);

  csOPCODECollider* col1 = (csOPCODECollider*) collider1;
  csOPCODECollider* col2 = (csOPCODECollider*) collider2;
  if (col1 == col2) return false;

  ColCache.Model0 = col1->m_pCollisionModel;
  ColCache.Model1 = col2->m_pCollisionModel;

  csMatrix3 m1;
  if (trans1) m1 = trans1->GetT2O ();
  csMatrix3 m2;
  if (trans2) m2 = trans2->GetT2O ();
  csVector3 u;

  u = m1.Row1 ();
  col1->transform.m[0][0] = u.x;
  col1->transform.m[1][0] = u.y;
  col1->transform.m[2][0] = u.z;
  u = m2.Row1 ();
  col2->transform.m[0][0] = u.x;
  col2->transform.m[1][0] = u.y;
  col2->transform.m[2][0] = u.z;
  u = m1.Row2 ();
  col1->transform.m[0][1] = u.x;
  col1->transform.m[1][1] = u.y;
  col1->transform.m[2][1] = u.z;
  u = m2.Row2 ();
  col2->transform.m[0][1] = u.x;
  col2->transform.m[1][1] = u.y;
  col2->transform.m[2][1] = u.z;
  u = m1.Row3 ();
  col1->transform.m[0][2] = u.x;
  col1->transform.m[1][2] = u.y;
  col1->transform.m[2][2] = u.z;
  u = m2.Row3();
  col2->transform.m[0][2] = u.x;
  col2->transform.m[1][2] = u.y;
  col2->transform.m[2][2] = u.z;

  if (trans1) u = trans1->GetO2TTranslation ();
  else u.Set (0, 0, 0);
  col1->transform.m[3][0] = u.x;
  col1->transform.m[3][1] = u.y;
  col1->transform.m[3][2] = u.z;

  if (trans2) u = trans2->GetO2TTranslation ();
  else u.Set (0, 0, 0);
  col2->transform.m[3][0] = u.x;
  col2->transform.m[3][1] = u.y;
  col2->transform.m[3][2] = u.z;

  bool isOk = TreeCollider.Collide (ColCache, &col1->transform,
  	&col2->transform);
  if (isOk)
  {
    bool status = (TreeCollider.GetContactStatus () != FALSE);
    if (status)
    {
      CopyCollisionPairs (col1, col2);
    }
    return status;
  }
  else
  {
    return false;
  }
}

static float max_dist;

static void ray_cb (const CollisionFace& hit, void* user_data)
{
  csArray<int>* collision_faces = (csArray<int>*)user_data;
  if (hit.mDistance <= max_dist)
    collision_faces->Push (hit.mFaceID);
}

bool csOPCODECollideSystem::CollideRaySegment (
  	iCollider* collider, const csReversibleTransform* trans,
	const csVector3& start, const csVector3& end, bool use_ray)
{
  if (!collider) return false;
  if (collider->GetColliderType () != CS_MESH_COLLIDER) return false;
  
  csOPCODECollider* col = (csOPCODECollider*) collider;
  ColCache.Model0 = col->m_pCollisionModel;

  csMatrix3 m;
  if (trans) m = trans->GetT2O ();
  csVector3 u;

  u = m.Row1 ();
  col->transform.m[0][0] = u.x;
  col->transform.m[1][0] = u.y;
  col->transform.m[2][0] = u.z;
  u = m.Row2 ();
  col->transform.m[0][1] = u.x;
  col->transform.m[1][1] = u.y;
  col->transform.m[2][1] = u.z;
  u = m.Row3 ();
  col->transform.m[0][2] = u.x;
  col->transform.m[1][2] = u.y;
  col->transform.m[2][2] = u.z;

  if (trans) u = trans->GetO2TTranslation ();
  else u.Set (0, 0, 0);
  col->transform.m[3][0] = u.x;
  col->transform.m[3][1] = u.y;
  col->transform.m[3][2] = u.z;

  Ray ray (Point (start.x, start.y, start.z),
  	   Point (end.x-start.x, end.y-start.y, end.z-start.z));

  RayCol.SetHitCallback (ray_cb);
  RayCol.SetUserData ((void*)&collision_faces);
  intersecting_triangles.SetLength (0);
  collision_faces.SetLength (0);
  if (use_ray)
  {
    max_dist = MAX_FLOAT;
    RayCol.SetMaxDist ();
  }
  else
  {
    max_dist = csQsqrt (csSquaredDist::PointPoint (start, end));
    RayCol.SetMaxDist (max_dist);
  }
  bool isOk = RayCol.Collide (ray, *ColCache.Model0, &col->transform);
  if (isOk)
  {
    bool status = (RayCol.GetContactStatus () != FALSE);
    if (status)
    {
      // Now calculate the real intersection points for all hit faces.
      Point* vertholder = col->vertholder;
      if (!vertholder) return true;
      udword* indexholder = col->indexholder;
      if (!indexholder) return true;
      if (collision_faces.Length () == 0) return false;
      Point* c;
      size_t i;
      for (i = 0 ; i < collision_faces.Length () ; i++)
      {
        int idx = collision_faces[i] * 3;
	int it_idx = (int)intersecting_triangles.Push (csIntersectingTriangle ());
	c = &vertholder[indexholder[idx+0]];
	intersecting_triangles[it_idx].a.Set (c->x, c->y, c->z);
	c = &vertholder[indexholder[idx+1]];
	intersecting_triangles[it_idx].b.Set (c->x, c->y, c->z);
	c = &vertholder[indexholder[idx+2]];
	intersecting_triangles[it_idx].c.Set (c->x, c->y, c->z);
      }
    }
    return status;
  }
  else
  {
    return false;
  }
}

void csOPCODECollideSystem::CopyCollisionPairs (csOPCODECollider* col1,
	csOPCODECollider* col2)
{
  int size = (int) (udword(TreeCollider.GetNbPairs ()));
  if (size == 0) return;
  int N_pairs = size;
  const Pair* colPairs=TreeCollider.GetPairs ();
  Point* vertholder0 = col1->vertholder;
  if (!vertholder0) return;
  Point* vertholder1 = col2->vertholder;
  if (!vertholder1) return;
  udword* indexholder0 = col1->indexholder;
  if (!indexholder0) return;
  udword* indexholder1 = col2->indexholder;
  if (!indexholder1) return;
  Point* current;
  int i, j;

  size_t oldlen = pairs.Length ();
  pairs.SetLength (oldlen + N_pairs);

  // it really sucks having to copy all this each Collide query, but
  // since opcode library uses his own vector types,
  // but there are some things to consider:
  // first, since opcode doesnt store the mesh, it relies on a callback
  // to query for precise positions of vertices when doing close-contact
  // checks, hence are two options:
  // 1. Do the triangulation only at the creation of the collider, but
  //    keeping copies of the meshes inside the colliders, which are then used
  //    for constructing the csCollisionPair and the opcode callbacks, but
  //    duplicates mesh data in memory (which is how is it currently)
  // 2. Do the triangulation again when a collision returns inside this
  //    precise routine (csOPCODECollider::Collide), which wouldn't be that bad,
  //    but doing it at the Callback would be plain wrong, since that Callback
  //    should be as fast as possible
  //    so, the question is, what we should prefer? memory or speed?
  //	   csPrintf( " 1) \n");

  for (i = 0 ; i < N_pairs ; i++)
  {
    j = 3 * colPairs[i].id0;
    current = &vertholder0[indexholder0[j]];		
    pairs[oldlen].a1 = csVector3 (current->x, current->y, current->z);
    current = &vertholder0[indexholder0[j + 1]];		
    pairs[oldlen].b1 = csVector3 (current->x, current->y, current->z);
    current = &vertholder0[indexholder0[j + 2]];		
    pairs[oldlen].c1 = csVector3 (current->x, current->y, current->z);

    j = 3 * colPairs[i].id1;
    current = &vertholder1[indexholder1[j]];		
    pairs[oldlen].a2 = csVector3 (current->x, current->y, current->z);
    current = &vertholder1[indexholder1[j + 1 ]];		
    pairs[oldlen].b2 = csVector3 (current->x, current->y, current->z);
    current = &vertholder1[indexholder1[j + 2 ]];		
    pairs[oldlen].c2 = csVector3 (current->x, current->y, current->z);

    oldlen++;
  }
}

csCollisionPair* csOPCODECollideSystem::GetCollisionPairs ()
{
  return pairs.GetArray ();
}

size_t csOPCODECollideSystem::GetCollisionPairCount ()
{
  return pairs.Length ();
}

void csOPCODECollideSystem::ResetCollisionPairs ()
{
  pairs.Empty ();
}

void csOPCODECollideSystem::SetOneHitOnly (bool on)
{
  TreeCollider.SetFirstContact (on);
}

/**
* Return true if this CD system will only return the first hit
* that is found. For CD systems that support multiple hits this
* will return the value set by the SetOneHitOnly() function.
* For CD systems that support one hit only this will always return true.
*/
bool csOPCODECollideSystem::GetOneHitOnly ()
{
  return (TreeCollider.FirstContactEnabled () != FALSE);
}
