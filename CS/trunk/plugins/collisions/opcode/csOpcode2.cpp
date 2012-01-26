/*
  Copyright (C) 2011 by Liu Lu

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "cssysdef.h"
#include "csqsqrt.h"
#include "ivaria/softanim.h"
#include "imesh/animesh.h"
#include "iengine/scenenode.h"
#include "iengine/mesh.h"
#include "iengine/light.h"
#include "csgeom/transfrm.h"
#include "csutil/scfstr.h"
#include "iutil/string.h"
#include "ivaria/reporter.h"
#include "csutil/scfarray.h"
#include "csOpcode2.h"
#include "collisionobject2.h"

CS_PLUGIN_NAMESPACE_BEGIN (Opcode2)
{
using namespace Opcode;

void Opcode_Log (const char* msg, ...)
{
  va_list args;
  va_start (args, msg);
  csOpcodeCollisionSystem::OpcodeReportV (CS_REPORTER_SEVERITY_NOTIFY, 
    msg, args);
  va_end (args);
}

bool Opcode_Err (const char* msg, ...)
{
  va_list args;
  va_start (args, msg);
  // Although it's called "..._Err", Opcode also reports less-than-fatal
  // messages through it
  csOpcodeCollisionSystem::OpcodeReportV (CS_REPORTER_SEVERITY_WARNING, 
    msg, args);
  va_end (args);
  return false;
}

csOpcodeCollisionSector::csOpcodeCollisionSector (csOpcodeCollisionSystem* sys)
: scfImplementationType (this), allFilter (-1), sys (sys)
{
  CS::Collisions::CollisionGroup defaultGroup ("Default");
  defaultGroup.value = 1;
  defaultGroup.mask = allFilter;
  collGroups.Push (defaultGroup);

  CS::Collisions::CollisionGroup staticGroup ("Static");
  staticGroup.value = 2;
  staticGroup.mask = allFilter ^ 2;
  collGroups.Push (staticGroup);
  systemFilterCount = 2;
}

csOpcodeCollisionSector::~csOpcodeCollisionSector ()
{

}

void csOpcodeCollisionSector::SetGravity (const csVector3& v)
{
//Set gravity for what?
}

void csOpcodeCollisionSector::AddCollisionObject (CS::Collisions::iCollisionObject* object)
{
  csRef<csOpcodeCollisionObject> obj (dynamic_cast<csOpcodeCollisionObject*>(object));
  collisionObjects.Push (obj);
  obj->sector = this;
  obj->collGroup = collGroups[0]; 

  AddMovableToSector (object);
}

void csOpcodeCollisionSector::RemoveCollisionObject (CS::Collisions::iCollisionObject* object)
{
  csOpcodeCollisionObject* collObject = dynamic_cast<csOpcodeCollisionObject*> (object);
  CS_ASSERT (collObject);
  collisionObjects.Delete (collObject);
  RemoveMovableFromSector (object);
}

CS::Collisions::iCollisionObject* csOpcodeCollisionSector::GetCollisionObject (size_t index)
{
  if (index >= 0 && index < collisionObjects.GetSize ())
    return collisionObjects[index]->QueryCollisionObject ();
  else
    return NULL;
}

CS::Collisions::iCollisionObject* csOpcodeCollisionSector::FindCollisionObject (const char* name)
{
  return collisionObjects.FindByName (name);
}

void csOpcodeCollisionSector::AddPortal (iPortal* portal, const csOrthoTransform& meshTrans)
{
//TODO
}

void csOpcodeCollisionSector::RemovePortal (iPortal* portal)
{
//TODO
}

CS::Collisions::HitBeamResult csOpcodeCollisionSector::HitBeam(const csVector3& start, 
                                                                const csVector3& end)
{
  CS::Collisions::HitBeamResult result;
  //Maybe at first do an AABB-ray collide. (Does Opcode support AABB-ray collide?)
  float depth = FLT_MAX;
  for (size_t i = 0; i < collisionObjects.GetSize (); i++)
  {
    CS::Collisions::HitBeamResult res;
    float dep;
    if (collisionObjects[i]->isTerrain)
      res = HitBeamTerrain (collisionObjects[i], start, end, dep);
    else
      res = HitBeamObject (collisionObjects[i], start, end, dep);
    if (res.hasHit && dep < depth)
    {
      depth = dep;
      result = res;
    }
  }
  return result;
}

CS::Collisions::HitBeamResult csOpcodeCollisionSector::HitBeamPortal(const csVector3& start, 
                                                                      const csVector3& end)
{
  CS::Collisions::HitBeamResult result = HitBeam (start, end);

  //TODO
  return result;
}

CS::Collisions::CollisionGroup& csOpcodeCollisionSector::CreateCollisionGroup (const char* name)
{
  size_t groupCount = collGroups.GetSize ();
  if (groupCount >= sizeof (CS::Collisions::CollisionGroupMask) * 8)
    return collGroups[0];

  CS::Collisions::CollisionGroup newGroup(name);
  newGroup.value = 1 << groupCount;
  newGroup.mask = allFilter ^ newGroup.value;
  collGroups.Push (newGroup);
  return collGroups[groupCount];
}

CS::Collisions::CollisionGroup& csOpcodeCollisionSector::FindCollisionGroup (const char* name)
{
  size_t index = collGroups.FindKey (CollisionGroupVector::KeyCmp (name));
  if (index == csArrayItemNotFound)
    return collGroups[0];
  else
    return collGroups[index];
}

void csOpcodeCollisionSector::SetGroupCollision (const char* name1, const char* name2, bool collide)
{
  size_t index1 = collGroups.FindKey (CollisionGroupVector::KeyCmp (name1));
  size_t index2 = collGroups.FindKey (CollisionGroupVector::KeyCmp (name2));
  if (index1 == csArrayItemNotFound || index2 == csArrayItemNotFound)
    return;
  if (!collide)
  {
    if (index1 >= systemFilterCount)
      collGroups[index1].value &= ~(1 << index2);
    if (index2 >= systemFilterCount)
      collGroups[index2].value &= ~(1 << index1);
  }
  else
  {
    if (index1 >= systemFilterCount)
      collGroups[index1].value |= 1 << index2;
    if (index2 >= systemFilterCount)
      collGroups[index2].value |= 1 << index1;
  }
}

bool csOpcodeCollisionSector::GetGroupCollision (const char* name1, const char* name2)
{
  size_t index1 = collGroups.FindKey (CollisionGroupVector::KeyCmp (name1));
  size_t index2 = collGroups.FindKey (CollisionGroupVector::KeyCmp (name2));
  if (index1 == csArrayItemNotFound || index2 == csArrayItemNotFound)
    return false;
  if ((collGroups[index1].value & (1 << index2)) != 0 
    || (collGroups[index2].value & (1 << index1)) != 0)
    return true;
  else
    return false;
}

bool csOpcodeCollisionSector::CollisionTest (CS::Collisions::iCollisionObject* object, csArray<CS::Collisions::CollisionData>& collisions)
{
  size_t length = collisions.GetSize ();
  csOpcodeCollisionObject* obj = dynamic_cast<csOpcodeCollisionObject*> (object);
  for (size_t i = 0; i < collisionObjects.GetSize (); i++)
  {
    if (collisionObjects[i]->QueryCollisionObject () != object)
    {
      bool collides = (obj->collGroup.value & collisionObjects[i]->collGroup.mask) != 0;
      collides = collides && (collisionObjects[i]->collGroup.value & obj->collGroup.mask);

      if(!collides)
        continue;

      if (obj->isTerrain)
      {
        if (collisionObjects[i]->isTerrain == false)
          CollideTerrain (obj, collisionObjects[i], collisions);
      }
      else
      {

        if (collisionObjects[i]->isTerrain)
          CollideTerrain (obj, collisionObjects[i], collisions);
        else
          CollideObject (obj, collisionObjects[i], collisions);
      }
    }
  }
  if (length != collisions.GetSize ())
    return true;
  else
    return false;
}

void csOpcodeCollisionSector::AddCollisionActor (CS::Collisions::iCollisionActor* actor)
{
//TODO
}

void csOpcodeCollisionSector::RemoveCollisionActor ()
{
//TODO
}

CS::Collisions::iCollisionActor* csOpcodeCollisionSector::GetCollisionActor ()
{
  return NULL;
}

void csOpcodeCollisionSector::AddMovableToSector (CS::Collisions::iCollisionObject* obj)
{
  iMovable* movable = obj->GetAttachedMovable ();
  if (movable && sector)
  {
    iMeshWrapper* mesh = movable->GetSceneNode ()->QueryMesh ();
    iLight* light = movable->GetSceneNode ()->QueryLight ();
    if (mesh)
      sector->GetMeshes ()->Add (mesh);
    else
      sector->GetLights ()->Add (light);
  }
}

void csOpcodeCollisionSector::RemoveMovableFromSector (CS::Collisions::iCollisionObject* obj)
{
  iMovable* movable = obj->GetAttachedMovable ();
  if (movable && sector)
  {
    iMeshWrapper* mesh = movable->GetSceneNode ()->QueryMesh ();
    iLight* light = movable->GetSceneNode ()->QueryLight ();
    if (mesh)
      sector->GetMeshes ()->Remove (mesh);
    else
      sector->GetLights ()->Remove (light);
  }
}

CS::Collisions::HitBeamResult csOpcodeCollisionSector::HitBeamObject (csOpcodeCollisionObject* object,
                                                                      const csVector3& start,
                                                                      const csVector3& end,
                                                                      float& depth)
{
  CS::Collisions::iCollider* col = object->collider;
  csOpcodeCollider* collider = dynamic_cast<csOpcodeCollider*> (col);
  CS::Collisions::HitBeamResult res = HitBeamCollider (collider->model, collider->vertholder, 
    collider->indexholder, object->transform, start, end, depth);
  if (res.hasHit)
    res.object = object;

  return res;
}

CS::Collisions::HitBeamResult csOpcodeCollisionSector::HitBeamTerrain (csOpcodeCollisionObject* terrainObj, 
                                                                       const csVector3& start, 
                                                                       const csVector3& end,
                                                                       float& depth)
{
  CS::Collisions::HitBeamResult result;
  CS::Collisions::iCollider* col = terrainObj->collider;
  csOpcodeColliderTerrain* terrainColl = dynamic_cast<csOpcodeColliderTerrain*> (col);
  for (size_t i = 0; i < terrainColl->colliders.GetSize (); i++)
  {
    result = HitBeamCollider (terrainColl->GetColliderModel (i), terrainColl->GetVertexHolder (i),
      terrainColl->GetIndexHolder (i), terrainColl->GetColliderTransform (i), start, end, depth);
    if (result.hasHit)
    {
      result.object = terrainObj;
      return result;
    }
  }
  return result;
}

static void ray_cb (const CollisionFace& hit, void* user_data)
{
  csArray<int>* collision_faces = (csArray<int>*)user_data;
  collision_faces->Push (hit.mFaceID);
}

CS::Collisions::HitBeamResult csOpcodeCollisionSector::HitBeamCollider (Opcode::Model* model, 
                                                                        Point* vertholder,
                                                                        udword* indexholder, 
                                                                        const csOrthoTransform& trans, 
                                                                        const csVector3& start, 
                                                                        const csVector3& end,
                                                                        float& depth)
{
  ColCache.Model0 = model;

  csMatrix3 m;
  m = trans.GetT2O ();
  csVector3 u;

  IceMaths::Matrix4x4 transform;
  transform.m[0][3] = 0;
  transform.m[1][3] = 0;
  transform.m[2][3] = 0;
  transform.m[3][3] = 1;
  u = m.Row1 ();
  transform.m[0][0] = u.x;
  transform.m[1][0] = u.y;
  transform.m[2][0] = u.z;
  u = m.Row2 ();
  transform.m[0][1] = u.x;
  transform.m[1][1] = u.y;
  transform.m[2][1] = u.z;
  u = m.Row3 ();
  transform.m[0][2] = u.x;
  transform.m[1][2] = u.y;
  transform.m[2][2] = u.z;

  u = trans.GetO2TTranslation ();
  transform.m[3][0] = u.x;
  transform.m[3][1] = u.y;
  transform.m[3][2] = u.z;

  csVector3 len = end - start;
  len.Normalize ();

  Ray ray (Point (start.x, start.y, start.z),
    Point (len.x, len.y, len.z));

  RayCol.SetHitCallback (ray_cb);
  RayCol.SetUserData ((void*)&collision_faces);
  collision_faces.SetSize (0);

  CS::Collisions::HitBeamResult result;
  bool isOk = RayCol.Collide (ray, *ColCache.Model0, &transform);
  if (isOk)
  {
    bool status = (RayCol.GetContactStatus () != FALSE);
    if (status)
    {
      // Now calculate the real intersection points for all hit faces.
      if (!vertholder) return result;
      if (!indexholder) return result;
      if (collision_faces.GetSize () == 0) return result;
      Point* c;
      size_t i;
      float dis = FLT_MAX;
      size_t index = -1;
      for (i = 0 ; i < collision_faces.GetSize () ; i++)
      {
        int idx = collision_faces[i] * 3;
        c = &vertholder[indexholder[idx+0]];
        csVector3 vec(c->x - start.x, c->y - start.y, c->z - start.z);
        float temp = trans.This2Other (vec) * len;
        if (temp < dis)
        {
          dis = temp;
          index = idx + 1;
        }

        c = &vertholder[indexholder[idx+1]];
        vec.Set(c->x - start.x, c->y - start.y, c->z - start.z);
        temp = trans.This2Other (vec) * len;
        if (temp < dis)
        {
          dis = temp;
          index = idx + 1;
        }

        c = &vertholder[indexholder[idx+2]];
        vec.Set(c->x - start.x, c->y - start.y, c->z - start.z);
        temp = trans.This2Other (vec) * len;
        if (temp < dis)
        {
          dis = temp;
          index = idx + 2;
        }
      }
      depth = dis;
      result.hasHit = true;
      result.vertexIndex = index;
      Point p = vertholder[indexholder[index]];
      result.isect = trans.This2Other(csVector3 (p.x, p.y, p.z));
      //This is not the correct normal.
      result.normal = -len;
    }
  }

  return result;

}

bool csOpcodeCollisionSector::CollideObject (csOpcodeCollisionObject* objA,
                                             csOpcodeCollisionObject* objB, 
                                             csArray<CS::Collisions::CollisionData>& collisions)
{
  CS::Collisions::iCollider* col = objA->collider;
  csOpcodeCollider* colliderA = dynamic_cast<csOpcodeCollider*> (col);
  col = objB->collider;
  csOpcodeCollider* colliderB = dynamic_cast<csOpcodeCollider*> (col);
  if (CollideDetect (colliderA->model, colliderB->model, 
    objA->transform, objB->transform))
  {
    
    GetCollisionData (colliderA->model, colliderB->model, 
      colliderA->vertholder, colliderB->vertholder,
      colliderA->indexholder, colliderB->indexholder,
      objA->transform, objB->transform);
    curCollisionData.objectA = objA;
    curCollisionData.objectB = objB;
    collisions.Push (curCollisionData);
    
    return true;
  }
  return false;
}

bool csOpcodeCollisionSector::CollideTerrain (csOpcodeCollisionObject* objA, 
                                              csOpcodeCollisionObject* objB,
                                              csArray<CS::Collisions::CollisionData>& collisions)
{
  CS::Collisions::iCollider* colA = objA->collider;
  CS::Collisions::iCollider* colB = objB->collider;
  csOpcodeColliderTerrain* terrainColl;
  csOpcodeCollider* objColl;
  bool terrainIsA = objA->isTerrain;
  if (terrainIsA)
  {
    terrainColl = dynamic_cast<csOpcodeColliderTerrain*> (colA);
    objColl = dynamic_cast<csOpcodeCollider*> (colB);
  }
  else
  {
    objColl = dynamic_cast<csOpcodeCollider*> (colA);
    terrainColl = dynamic_cast<csOpcodeColliderTerrain*> (colB);
  }

  for (size_t i = 0; i < terrainColl->colliders.GetSize (); i++)
  {
    bool isCollide;
    if (terrainIsA)
    {
      isCollide = CollideDetect (terrainColl->GetColliderModel (i), objColl->model,
      terrainColl->GetColliderTransform (i), objB->transform);
      if (isCollide)
      { 
        
        GetCollisionData (terrainColl->GetColliderModel (i), objColl->model,
          terrainColl->GetVertexHolder (i), objColl->vertholder,
          terrainColl->GetIndexHolder (i), objColl->indexholder,
          terrainColl->GetColliderTransform (i), objB->transform);
        curCollisionData.objectA = objA;
        curCollisionData.objectB = objB;
        collisions.Push (curCollisionData);
        
        return true;
      }
    }
    else
    {
      isCollide = CollideDetect (objColl->model, terrainColl->GetColliderModel (i),
      objA->transform, terrainColl->GetColliderTransform (i));
      if (isCollide)
      { 
        
        GetCollisionData (objColl->model, terrainColl->GetColliderModel (i),
        objColl->vertholder, terrainColl->GetVertexHolder (i),
        objColl->indexholder, terrainColl->GetIndexHolder (i),
        objA->transform, terrainColl->GetColliderTransform (i));
        curCollisionData.objectA = objA;
        curCollisionData.objectB = objB;
        collisions.Push (curCollisionData);
        
        return true;
      }
    }
  }
  return false;
}

bool csOpcodeCollisionSector::CollideDetect (Opcode::Model* modelA,
                                             Opcode::Model* modelB, 
                                             const csOrthoTransform& transA, 
                                             const csOrthoTransform& transB)
{
  ColCache.Model0 = modelA;
  ColCache.Model1 = modelB;

  csMatrix3 m1;
  m1 = transA.GetT2O ();
  csMatrix3 m2;
  m2 = transB.GetT2O ();
  csVector3 u;

  IceMaths::Matrix4x4 transform1;
  transform1.m[0][3] = 0;
  transform1.m[1][3] = 0;
  transform1.m[2][3] = 0;
  transform1.m[3][3] = 1;
  IceMaths::Matrix4x4 transform2;
  transform2.m[0][3] = 0;
  transform2.m[1][3] = 0;
  transform2.m[2][3] = 0;
  transform2.m[3][3] = 1;

  u = m1.Row1 ();
  transform1.m[0][0] = u.x;
  transform1.m[1][0] = u.y;
  transform1.m[2][0] = u.z;
  u = m2.Row1 ();
  transform2.m[0][0] = u.x;
  transform2.m[1][0] = u.y;
  transform2.m[2][0] = u.z;
  u = m1.Row2 ();
  transform1.m[0][1] = u.x;
  transform1.m[1][1] = u.y;
  transform1.m[2][1] = u.z;
  u = m2.Row2 ();
  transform2.m[0][1] = u.x;
  transform2.m[1][1] = u.y;
  transform2.m[2][1] = u.z;
  u = m1.Row3 ();
  transform1.m[0][2] = u.x;
  transform1.m[1][2] = u.y;
  transform1.m[2][2] = u.z;
  u = m2.Row3();
  transform2.m[0][2] = u.x;
  transform2.m[1][2] = u.y;
  transform2.m[2][2] = u.z;

  u = transA.GetO2TTranslation ();
  transform1.m[3][0] = u.x;
  transform1.m[3][1] = u.y;
  transform1.m[3][2] = u.z;

  u = transB.GetO2TTranslation ();
  transform2.m[3][0] = u.x;
  transform2.m[3][1] = u.y;
  transform2.m[3][2] = u.z;

  bool isOk = TreeCollider.Collide (ColCache, &transform1, &transform2);

  if (isOk)
  {
    bool status = (TreeCollider.GetContactStatus () != FALSE);
    return status;
  }
  return false;
}

void csOpcodeCollisionSector::GetCollisionData (Opcode::Model* modelA, 
                                                Opcode::Model* modelB, 
                                                Point* vertholderA, 
                                                Point* vertholderB, 
                                                udword* indexholderA, 
                                                udword* indexholderB, 
                                                const csOrthoTransform& transA,
                                                const csOrthoTransform& transB)
{
  int size = (int) (udword(TreeCollider.GetNbPairs ()));
  if (size == 0) return;
  int N_pairs = size;
  const Pair* colPairs=TreeCollider.GetPairs ();
  if (!vertholderA || !vertholderB
    || !indexholderA || !indexholderB) 
    return;

  Point* current;
  int i, j;

  csArray<csVector3> pointsA;
  pointsA.SetSize (N_pairs * 3);
  csArray<csVector3> pointsB;
  pointsB.SetSize (N_pairs * 3);
  csVector3 normal1 (0.0f, 0.0f, 0.0f);
  csVector3 normal2 (0.0f, 0.0f, 0.0f);
  csVector3 temp;

  int index = 0;
  for (i = 0; i < N_pairs; i++)
  {
    j = 3 * colPairs[i].id0;
    current = &vertholderA[indexholderA[j]];		
    csVector3 a1(current->x, current->y, current->z);
    current = &vertholderA[indexholderA[j + 1]];		
    csVector3 b1(current->x, current->y, current->z);
    current = &vertholderA[indexholderA[j + 2]];		
    csVector3 c1(current->x, current->y, current->z);

    temp.Cross (b1-a1, c1-a1);
    temp.Normalize ();
    normal1 += temp;

    j = 3 * colPairs[i].id1;
    current = &vertholderB[indexholderB[j]];		
    csVector3 a2(current->x, current->y, current->z);
    current = &vertholderB[indexholderB[j + 1 ]];		
    csVector3 b2(current->x, current->y, current->z);
    current = &vertholderB[indexholderB[j + 2 ]];		
    csVector3 c2(current->x, current->y, current->z);

    temp.Cross (b2-a2, c2-a2);
    temp.Normalize ();
    normal2 += temp;

    pointsA[index].Set (a1);
    pointsA[index+1].Set (b1);
    pointsA[index+2].Set (c1);
    pointsB[index++].Set (a2);
    pointsB[index++].Set (b2);
    pointsB[index++].Set (c2);
  }

  normal1 /= N_pairs;
  normal1.Normalize ();
  normal2 /= N_pairs;
  normal2.Normalize ();

  float dis1 = 0;
  float dis2 = 0;
  csVector3 posi1;
  csVector3 posi2;
  for (i = 0; i < index; i++)
  {
    float temp = pointsA[i] * normal1;
    if (temp > dis1)
    {
      dis1 = temp;
      posi1 = pointsA[i];
    }

    temp = pointsB[i] * normal2;
    if (temp > dis2)
    {
      dis2 = temp;
      posi2 = pointsB[i];
    }
  }

  csVector3 disBetweenPoints = posi1 - posi2;

  curCollisionData.normalWorldOnB = transB.This2Other (normal2);
  curCollisionData.positionWorldOnA = transA.This2Other (posi1);
  curCollisionData.positionWorldOnB = transB.This2Other (posi2);
  curCollisionData.penetration = csVector3::Norm (posi1 - posi2);
}

SCF_IMPLEMENT_FACTORY (csOpcodeCollisionSystem)

iObjectRegistry* csOpcodeCollisionSystem::rep_object_reg = NULL;

csOpcodeCollisionSystem::csOpcodeCollisionSystem (iBase* iParent)
: scfImplementationType (this, iParent)
{
}

csOpcodeCollisionSystem::~csOpcodeCollisionSystem ()
{
  rep_object_reg = 0;
  collSectors.DeleteAll ();
}

bool csOpcodeCollisionSystem::Initialize (iObjectRegistry* object_reg)
{
  this->object_reg = object_reg;
  rep_object_reg = object_reg;
  csRef<iStringSet> strings = csQueryRegistryTagInterface<iStringSet> (
    object_reg, "crystalspace.shared.stringset");
  baseID = strings->Request ("base");
  colldetID = strings->Request ("colldet");
  return true;
}

void csOpcodeCollisionSystem::SetInternalScale (float scale)
{
  //use internal scale?
}

csRef<CS::Collisions::iColliderConvexMesh> csOpcodeCollisionSystem::CreateColliderConvexMesh (iMeshWrapper* mesh, bool simplify /* = false */)
{
  return NULL;
}

csRef<CS::Collisions::iColliderConcaveMesh> csOpcodeCollisionSystem::CreateColliderConcaveMesh (iMeshWrapper* mesh)
{
  csRef<csOpcodeCollider> collider;
  collider.AttachNew (new csOpcodeCollider (mesh,this));

  return collider;
}

csRef<CS::Collisions::iColliderConcaveMeshScaled> csOpcodeCollisionSystem::CreateColliderConcaveMeshScaled
(CS::Collisions::iColliderConcaveMesh* collider, csVector3 scale)
{
  return NULL;
}

csRef<CS::Collisions::iColliderCylinder> csOpcodeCollisionSystem::CreateColliderCylinder (float length, float radius)
{
  return NULL;
}

csRef<CS::Collisions::iColliderBox> csOpcodeCollisionSystem::CreateColliderBox (const csVector3& size)
{
  return NULL;
}

csRef<CS::Collisions::iColliderSphere> csOpcodeCollisionSystem::CreateColliderSphere (float radius)
{
  return NULL;
}

csRef<CS::Collisions::iColliderCapsule> csOpcodeCollisionSystem::CreateColliderCapsule (float length, float radius)
{
  return NULL;
}

csRef<CS::Collisions::iColliderCone> csOpcodeCollisionSystem::CreateColliderCone (float length, float radius)
{
  return NULL;
}

csRef<CS::Collisions::iColliderPlane> csOpcodeCollisionSystem::CreateColliderPlane (const csPlane3& plane)
{
  return NULL;
}

csRef<CS::Collisions::iColliderTerrain> csOpcodeCollisionSystem::CreateColliderTerrain (iTerrainSystem* terrain,
                                                               float minHeight, float maxHeight)
{
  csRef<csOpcodeColliderTerrain> collider;
  collider.AttachNew (new csOpcodeColliderTerrain (terrain, this));

  return collider;
}

csRef<CS::Collisions::iCollisionObject> csOpcodeCollisionSystem::CreateCollisionObject ()
{
  csRef<csOpcodeCollisionObject> collObject;
  collObject.AttachNew (new csOpcodeCollisionObject (this));

  return collObject;
}

csRef<CS::Collisions::iCollisionActor> csOpcodeCollisionSystem::CreateCollisionActor ()
{
  //TODO
  return NULL;
}

csRef<CS::Collisions::iCollisionSector> csOpcodeCollisionSystem::CreateCollisionSector ()
{
  csRef<csOpcodeCollisionSector> collSector;
  collSector.AttachNew (new csOpcodeCollisionSector (this));

  collSectors.Push (collSector);
  return collSector;
}

CS::Collisions::iCollisionSector* csOpcodeCollisionSystem::FindCollisionSector (const char* name)
{
  return this->collSectors.FindByName (name);
}

void csOpcodeCollisionSystem::DecomposeConcaveMesh (CS::Collisions::iCollisionObject* object,
                                   iMeshWrapper* mesh, bool simplify)
{
}

void csOpcodeCollisionSystem::OpcodeReportV (int severity, const char* message, 
                           va_list args)
{
  csReportV (rep_object_reg,
    severity, "crystalspace.collisiondetection.opcode", message, args);
}
}
CS_PLUGIN_NAMESPACE_END (Opcode2)
