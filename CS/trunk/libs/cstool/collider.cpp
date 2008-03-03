/*
    Copyright (C) 2000-2003 by Jorrit Tyberghein
    Written by Daniel Gudbjartsson

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

#include "csqsqrt.h"
#include "csgeom/math3d.h"
#include "csgeom/quaternion.h"
#include "csgeom/transfrm.h"
#include "csgeom/trimesh.h"

#include "cstool/collider.h"
#include "iengine/collection.h"
#include "iengine/camera.h"
#include "iengine/engine.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "iengine/portal.h"
#include "iengine/portalcontainer.h"
#include "iengine/region.h"
#include "iengine/sector.h"
#include "iengine/viscull.h"
#include "iengine/scenenode.h"

#include "imesh/objmodel.h"
#include "imesh/object.h"
#include "ivaria/collider.h"

//----------------------------------------------------------------------

CS_LEAKGUARD_IMPLEMENT (csColliderWrapper);



csColliderWrapper::csColliderWrapper (csObject& parent,
	iCollideSystem* collide_system,
	iTriangleMesh* mesh)
  : scfImplementationType (this)
{
  parent.ObjAdd (this);
  csColliderWrapper::collide_system = collide_system;
  collider = collide_system->CreateCollider (mesh);
}

csColliderWrapper::csColliderWrapper (iObject* parent,
        iCollideSystem* collide_system,
        iTriangleMesh* mesh)
  : scfImplementationType (this)
{
  parent->ObjAdd (this);
  csColliderWrapper::collide_system = collide_system;
  collider = collide_system->CreateCollider (mesh);
}

csColliderWrapper::csColliderWrapper (iObject* parent,
	iCollideSystem* collide_system,
	iTerraFormer* terrain)
  : scfImplementationType (this)
{
  parent->ObjAdd (this);
  csColliderWrapper::collide_system = collide_system;
  collider = collide_system->CreateCollider (terrain);
}

csColliderWrapper::csColliderWrapper (iObject* parent,
    iCollideSystem* collide_system,
  	iTerrainSystem* terrain)
  : scfImplementationType (this)
{
  parent->ObjAdd (this);
  csColliderWrapper::collide_system = collide_system;
  collider = collide_system->CreateCollider (terrain);
}

csColliderWrapper::csColliderWrapper (iObject* parent,
	iCollideSystem* collide_system,
	iCollider* collider)
  : scfImplementationType (this)
{
  parent->ObjAdd (this);
  csColliderWrapper::collide_system = collide_system;
  csColliderWrapper::collider = collider;
}

csColliderWrapper::~csColliderWrapper ()
{
}

bool csColliderWrapper::Collide (csObject& otherObject,
                          csReversibleTransform* pThisTransform,
                          csReversibleTransform* pOtherTransform)
{
  csColliderWrapper *pOtherCollider = GetColliderWrapper (otherObject);
  if (pOtherCollider)
    return Collide (*pOtherCollider, pThisTransform, pOtherTransform);
  else
    return false;
}

bool csColliderWrapper::Collide (iObject* otherObject,
                          csReversibleTransform* pThisTransform,
                          csReversibleTransform* pOtherTransform)
{
  csColliderWrapper *pOtherCollider = GetColliderWrapper (otherObject);
  if (pOtherCollider)
    return Collide (*pOtherCollider, pThisTransform, pOtherTransform);
  else
    return false;
}

bool csColliderWrapper::Collide (csColliderWrapper& otherCollider,
                          csReversibleTransform* pTransform1,
                          csReversibleTransform* pTransform2)
{
  if (!collider) return false;
  csColliderWrapper *pCollider2 = &otherCollider;
  if (pCollider2 == this) return false;

  return collide_system->Collide (collider, pTransform1,
  	pCollider2->collider, pTransform2);
}

csColliderWrapper* csColliderWrapper::GetColliderWrapper (csObject &object)
{
  csRef<csColliderWrapper> w (CS::GetChildObject<csColliderWrapper> (&object));
  return w;	// This will DecRef() but that's ok in this case.
}

csColliderWrapper* csColliderWrapper::GetColliderWrapper (iObject* object)
{
  csRef<csColliderWrapper> w (CS::GetChildObject<csColliderWrapper> (object));
  return w;	// This will DecRef() but that's ok in this case.
}

void csColliderWrapper::UpdateCollider (iTriangleMesh* mesh)
{
  collider = collide_system->CreateCollider (mesh);
}

void csColliderWrapper::UpdateCollider (iTerraFormer* terrain)
{
  collider = collide_system->CreateCollider (terrain);
}
//----------------------------------------------------------------------

csColliderWrapper* csColliderHelper::InitializeCollisionWrapper (
	iCollideSystem* colsys, iMeshWrapper* mesh)
{
  iMeshFactoryWrapper* factory = mesh->GetFactory ();

  iObjectModel* obj_objmodel = mesh->GetMeshObject ()->GetObjectModel ();
  csStringID basemesh_id = colsys->GetBaseDataID ();
  csStringID trianglemesh_id = colsys->GetTriangleDataID ();

  iTriangleMesh* obj_trimesh;
  // If the collision detection triangle mesh is not set then we
  // use the base mesh instead.
  bool obj_trimesh_set = obj_objmodel->IsTriangleDataSet (trianglemesh_id);
  if (obj_trimesh_set)
    obj_trimesh = obj_objmodel->GetTriangleData (trianglemesh_id);
  else
    obj_trimesh = obj_objmodel->GetTriangleData (basemesh_id);

  iTerraFormer* obj_terraformer = obj_objmodel->GetTerraFormerColldet ();
  iTerrainSystem* obj_terrain = obj_objmodel->GetTerrainColldet ();

  csRef<csColliderWrapper> cw;
  if (factory)
  {
    iObjectModel* fact_objmodel = factory->GetMeshObjectFactory ()
    	->GetObjectModel ();
    if (fact_objmodel)
    {
      if (fact_objmodel->GetTerraFormerColldet ())
      {
        iTerraFormer* fact_terraformer = fact_objmodel
	  ->GetTerraFormerColldet ();

        if (fact_terraformer && (fact_terraformer == obj_terraformer
	      || !obj_terraformer))
        {
          // First check if the parent factory has a collider wrapper.
          iCollider* collider;
          csColliderWrapper* cw_fact = csColliderWrapper::GetColliderWrapper (
            factory->QueryObject ());
          if (cw_fact)
          {
            collider = cw_fact->GetCollider ();
          }
          else
          {
            csColliderWrapper *cw_fact = new csColliderWrapper (
              factory->QueryObject (), colsys, fact_terraformer);
            cw_fact->SetName (factory->QueryObject ()->GetName());
            collider = cw_fact->GetCollider ();
            cw_fact->DecRef ();
          }

          // Now add the collider wrapper to the mesh. We need a new
          // csColliderWrapper because the csObject system is strictly
          // a tree and one csColliderWrapper cannot have multiple parents.
          cw.AttachNew (new csColliderWrapper (mesh->QueryObject (),
            colsys, collider));
          cw->SetName (mesh->QueryObject ()->GetName());
          cw = 0;
          // Clear object triangle mesh so we will no longer try to create
          // a collider from that (we already have a collider).
          obj_terraformer = 0;
        }
      }
      else 
      {
	if (!obj_trimesh_set)
	{
	  bool fact_trimesh_set = fact_objmodel->IsTriangleDataSet (
	        trianglemesh_id);
          iTriangleMesh* fact_trimesh;
	  // If the collision detection triangle mesh is not set then we
	  // use the base mesh instead.
	  if (fact_trimesh_set)
	    fact_trimesh  = fact_objmodel->GetTriangleData (trianglemesh_id);
	  else
	    fact_trimesh  = fact_objmodel->GetTriangleData (basemesh_id);

	  if (fact_trimesh)
	  {
            // First check if the parent factory has a collider wrapper.
            iCollider* collider;
            csColliderWrapper* cw_fact =
		csColliderWrapper::GetColliderWrapper (factory->QueryObject ());
            if (cw_fact)
            {
              collider = cw_fact->GetCollider ();
            }
            else
            {
              csColliderWrapper *cw_fact = new csColliderWrapper (
                  factory->QueryObject (), colsys, fact_trimesh);
              cw_fact->SetName (factory->QueryObject ()->GetName());
              collider = cw_fact->GetCollider ();
              cw_fact->DecRef ();
            }

            // Now add the collider wrapper to the mesh. We need a new
            // csColliderWrapper because the csObject system is strictly
            // a tree and one csColliderWrapper cannot have multiple parents.
            cw.AttachNew (new csColliderWrapper (mesh->QueryObject (),
                colsys, collider));
            cw->SetName (mesh->QueryObject ()->GetName());
	  }
          // Clear object triangle mesh so we will no longer try to create
          // a collider from that (we already have a collider).
          obj_trimesh = 0;
	}
      }
    }
  }

  if (obj_terraformer)
    cw.AttachNew (new csColliderWrapper (mesh->QueryObject (),
      colsys, obj_terraformer));
  else if (obj_terrain)
    cw.AttachNew (new csColliderWrapper (mesh->QueryObject (),
      colsys, obj_terrain));
  else if (obj_trimesh)
    cw.AttachNew (new csColliderWrapper (mesh->QueryObject (),
      colsys, obj_trimesh));
  if (cw)
    cw->SetName (mesh->QueryObject ()->GetName());

  const csRef<iSceneNodeArray> ml = 
    mesh->QuerySceneNode ()->GetChildrenArray ();
  size_t i;
  for (i = 0 ; i < ml->GetSize () ; i++)
  {
    iMeshWrapper* child = ml->Get (i)->QueryMesh ();
    // @@@ What if we have a light containing another mesh?
    if (child)
      InitializeCollisionWrapper (colsys, child);
  }

  return cw;
}

#include "csutil/deprecated_warn_off.h"

void csColliderHelper::InitializeCollisionWrappers(iCollideSystem* colsys,
  	iEngine* engine, iBase* base)
{
  csRef<iRegion> region (scfQueryInterfaceSafe<iRegion>(base));
  if(region)
  {
    return InitializeCollisionWrappersRegion(colsys, engine, region);
  }
  else
  {
    csRef<iCollection> collection (scfQueryInterfaceSafe<iCollection>(base));
    return InitializeCollisionWrappersCollection(colsys, engine, collection);
  }
}

void csColliderHelper::InitializeCollisionWrappersCollection(iCollideSystem* colsys,
  	iEngine* engine, iCollection* collection)
{
  // Initialize all mesh objects for collision detection.
  int i;
  iMeshList* meshes = engine->GetMeshes ();
  for (i = 0 ; i < meshes->GetCount () ; i++)
  {
    iMeshWrapper* sp = meshes->Get (i);
    if (collection && collection->IsParentOf(sp->QueryObject ()))
    {
      InitializeCollisionWrapper (colsys, sp);
    }
  }
}

void csColliderHelper::InitializeCollisionWrappersRegion (iCollideSystem* colsys,
  	iEngine* engine, iRegion* region)
{
  // Initialize all mesh objects for collision detection.
  int i;
  iMeshList* meshes = engine->GetMeshes ();
  for (i = 0 ; i < meshes->GetCount () ; i++)
  {
    iMeshWrapper* sp = meshes->Get (i);
    if (region && !region->IsInRegion (sp->QueryObject ())) continue;
    InitializeCollisionWrapper (colsys, sp);
  }
}

#include "csutil/deprecated_warn_on.h"

bool csColliderHelper::CollideArray (
	iCollideSystem* colsys,
	iCollider* collider,
  	const csReversibleTransform* trans,
  	int num_colliders,
	iCollider** colliders,
	csReversibleTransform **transforms)
{
  int i;
  for (i = 0 ; i < num_colliders ; i++)
  {
    bool rc = colsys->Collide (collider, trans,
    	colliders[i], transforms[i]);
    if (rc) return rc;
  }
  return false;
}

int csColliderHelper::CollidePath (
	iCollideSystem* colsys,
	iCollider* collider,
  	const csReversibleTransform* trans,
	float nbrsteps,
	csVector3& newpos,
	int num_colliders,
	iCollider** colliders,
	csReversibleTransform** transforms)
{
  csReversibleTransform test = *trans;
  csVector3 start = test.GetOrigin ();
  csVector3 end = newpos;
  csVector3 testpos;
  float step = 1.0f / nbrsteps;
  float curdist = 0;
  bool rc = false;
  bool firsthit = true;
  for (;;)
  {
    testpos = start+curdist * (end-start);
    test.SetOrigin (testpos);
    colsys->ResetCollisionPairs ();
    rc = CollideArray (colsys, collider, &test,
    	num_colliders, colliders, transforms);
    if (rc) break;
    firsthit = false;

    if (curdist >= 1) break;
    curdist += step;
    if (curdist > 1) curdist = 1;
  }

  if (rc)
  {
    // We had a collision.
    if (firsthit)
    {
      // The collision happened on the start point. In that case
      // we cannot move at all. Return -1.
      return -1;
    }

    // Here we try to find more exactly where the collision occured by
    // doing a binary search.
    end = testpos;
    while (csSquaredDist::PointPoint (start, end) > .05)
    {
      testpos = (start+end) / 2;
      test.SetOrigin (testpos);
      colsys->ResetCollisionPairs ();
      rc = CollideArray (colsys, collider, &test,
      	num_colliders, colliders, transforms);
      if (rc)
      {
        // Use left segment.
        end = testpos;
      }
      else
      {
        // Use right segment.
	start = testpos;
      }
    }
    // We know that the object can move to the 'start' position safely
    // because of the way we handle the binary search and the starting
    // condition that firsthit == false.
    newpos = start;

    // But first we set the collision detection array to the position
    // which resulted in collision.
    test.SetOrigin (end);
    colsys->ResetCollisionPairs ();
    CollideArray (colsys, collider, &test,
    	num_colliders, colliders, transforms);

    return 0;
  }
  else
  {
    // There was no collision.
    return 1;
  }
}

csTraceBeamResult csColliderHelper::TraceBeam (iCollideSystem* cdsys,
	iSector* sector, const csVector3& start, const csVector3& end,
	bool traverse_portals)
{
  csTraceBeamResult rc;
  rc.sqdistance = TraceBeam (cdsys, sector, start, end, traverse_portals,
  	rc.closest_tri, rc.closest_isect, &rc.closest_mesh, &rc.end_sector);
  return rc;
}

float csColliderHelper::TraceBeam (iCollideSystem* cdsys, iSector* sector,
	const csVector3& start, const csVector3& end,
	bool traverse_portals,
	csIntersectingTriangle& closest_tri,
	csVector3& closest_isect,
	iMeshWrapper** closest_mesh,
	iSector** end_sector)
{
  if (!sector)
  {
    if (closest_mesh) *closest_mesh = 0;
	if (end_sector) *end_sector = 0;
    return -1.0f;
  }
  if (end_sector) *end_sector = sector;

  iVisibilityCuller* culler = sector->GetVisibilityCuller ();
  csRef<iVisibilityObjectIterator> it = culler->IntersectSegmentSloppy (
  	start, end);

  // We loop over all objects that intersect with the beam. For every
  // such object we will use CollideRay() to find colliding triangles. We
  // will look for the colliding triangle closest to 'start'.
  float best_squared_dist = 10000000000.0;
  csSegment3 seg (start, end);
  iMeshWrapper* best_mesh = 0;
  bool have_hit = false;
  // This will be set to the hit portal index if the best polygon we
  // hit so far is actually a portal.
  int last_portal_index = -1;
 
  while (it->HasNext ())
  {
    iVisibilityObject* visobj = it->Next ();
    iMeshWrapper* mesh = visobj->GetMeshWrapper ();
    csColliderWrapper* colwrap = csColliderWrapper::GetColliderWrapper (
    	mesh->QueryObject ());
    if (colwrap)
    {
      iMovable* movable = mesh->GetMovable ();
      csReversibleTransform trans = movable->GetFullTransform ();
      if (cdsys->CollideRay (colwrap->GetCollider (), &trans, start, end))
      {
        // This ray hits the mesh.
	const csArray<csIntersectingTriangle>& tris = cdsys->
		GetIntersectingTriangles ();
	size_t i;
	for (i = 0 ; i < tris.GetSize () ; i++)
	{
	  csVector3 isect;
	  csIntersectingTriangle tri;
	  if (movable->IsFullTransformIdentity ())
	  {
	    tri = tris[i];
	  }
	  else
	  {
	    tri.a = trans.This2Other (tris[i].a);
	    tri.b = trans.This2Other (tris[i].b);
	    tri.c = trans.This2Other (tris[i].c);
	  }

	  // The function below should always return true but you never know
	  // due to numerical inaccuracies.
	  if (csIntersect3::SegmentTriangle (seg, tri.a, tri.b, tri.c,
	  	isect))
	  {
	    float squared_dist = csSquaredDist::PointPoint (isect, start);
	    if (squared_dist < best_squared_dist)
	    {
	      have_hit = true;
	      best_squared_dist = squared_dist;
	      closest_tri = tri;
	      closest_isect = isect;
	      // This is not a portal we want to traverse because it
	      // has a collider which means it is solid.
	      last_portal_index = -1;
	      best_mesh = mesh;
	    }
	  }
	}
      }
    }
    if (mesh->GetPortalContainer () && traverse_portals)
    {
      // We have a portal container and we want to specifically traverse
      // portals. In that case we also have to trace a beam to whatever
      // portal we might hit.
      iMovable* movable = mesh->GetMovable ();
      csReversibleTransform trans = movable->GetFullTransform ();
      csVector3 obj_start, obj_end;
      if (movable->IsFullTransformIdentity ())
      {
        obj_start = start;
        obj_end = end;
      }
      else
      {
        obj_start = trans.Other2This (start);
        obj_end = trans.Other2This (end);
      }
      csVector3 obj_isect;
      int polygon_idx;
      if (mesh->GetMeshObject ()->HitBeamObject (obj_start, obj_end,
      		obj_isect, 0, &polygon_idx))
      {
        if (!movable->IsFullTransformIdentity ())
	  obj_isect = trans.This2Other (obj_isect);
	float squared_dist = csSquaredDist::PointPoint (obj_isect, start);
	if (squared_dist < best_squared_dist)
	{
	  have_hit = true;
	  best_squared_dist = squared_dist;
	  closest_isect = obj_isect;
	  last_portal_index = polygon_idx;
	  best_mesh = mesh;
	}
      }
    }
  }

  // If our best hit is a portal then we must traverse it here.
  if (last_portal_index != -1)
  {
    iPortal* portal = best_mesh->GetPortalContainer ()->GetPortal (
    	last_portal_index);
    // First we calculate a new beam starting in the new sector from
    // the intersection point. We make sure the new start is slightly
    // beyond the intersection point to avoid numerical inaccuracies.
    // We take a point 0.1% on the way from 'closest_isect' to 'end'.
    csVector3 new_start = closest_isect + 0.001 * (end-closest_isect);
    csVector3 new_end = end;

    // Now we have to consider space warping for the portal.
    if (portal->GetFlags ().Check (CS_PORTAL_WARP))
    {
      iMovable* movable = best_mesh->GetMovable ();
      csReversibleTransform trans = movable->GetFullTransform ();

      csReversibleTransform warp_wor;
      portal->ObjectToWorld (trans, warp_wor);
      new_start = portal->Warp (warp_wor, new_start);
      new_end = portal->Warp (warp_wor, new_end);
    }

    if (end_sector) *end_sector = portal->GetSector();

    // Recurse with the new beam to the new sector.
    float new_squared_dist = TraceBeam (cdsys, portal->GetSector (),
	new_start, new_end, traverse_portals,
	closest_tri, closest_isect,
	closest_mesh, end_sector);
    if (new_squared_dist >= 0)
    {
      // We have a hit. We have to add the distance so far to the
      // new distance.
      float new_dist = csQsqrt (best_squared_dist) + csQsqrt (new_squared_dist);
      return new_dist * new_dist;
    }
    return -1.0f;
  }

  if (closest_mesh) *closest_mesh = best_mesh;
  if (have_hit)
    return best_squared_dist;
  else
    return -1.0f;
}
//----------------------------------------------------------------------

csColliderActor::csColliderActor ()
{
  revertMove = false;
  gravity = 9.806f;
  onground = false;
  cd = true;
  velWorld.Set (0, 0, 0);
  engine = 0;
  cdsys = 0;
  mesh = 0;
  camera = 0;
  movable = 0;
  revertCount = 0;
  do_hit_meshes = false;

  // Only used in case a camera is used.
  rotation.Set (0, 0, 0);
}

void csColliderActor::InitializeColliders (
	const csVector3& legs, const csVector3& body, const csVector3& shift)
{
  csColliderActor::shift = shift;
  bottomSize = legs;
  topSize = body;
  intervalSize.x = MIN(topSize.x, bottomSize.x);
  intervalSize.y = MIN(topSize.y, bottomSize.y);
  intervalSize.z = MIN(topSize.z, bottomSize.z);

  float maxX = MAX(body.x, legs.x)+shift.x;
  float maxZ = MAX(body.z, legs.z)+shift.z;

  csRef<iTriangleMesh> pm;

  float bX2 = body.x / 2.0f;
  float bZ2 = body.z / 2.0f;
  float bYbottom = legs.y;
  float bYtop = legs.y + body.y;

  csBox3 top (csVector3 (-bX2, bYbottom, -bZ2) + shift,
		csVector3 (bX2, bYtop, bZ2) + shift);
  pm = csPtr<iTriangleMesh> (new csTriangleMeshBox (top));
  topCollider = cdsys->CreateCollider (pm);

  float lX2 = legs.x / 2.0f;
  float lZ2 = legs.z / 2.0f;

  csBox3 bot (csVector3 (-lX2, 0, -lZ2) + shift,
		csVector3 (lX2, 0 + legs.y, lZ2) + shift);
  pm = csPtr<iTriangleMesh> (new csTriangleMeshBox (bot));
  bottomCollider = cdsys->CreateCollider (pm);

  boundingBox.Set(csVector3(-maxX / 2.0f, 0, -maxZ / 2.0f) + shift,
    csVector3(maxX / 2.0f, bYtop, maxZ / 2.0f) + shift);

// @@@ Don't know why this is needed!
  csColliderActor::shift.x = -shift.x;
  csColliderActor::shift.y = -shift.y;
  csColliderActor::shift.z = -shift.z;
}

void csColliderActor::InitializeColliders (iMeshWrapper* mesh,
	const csVector3& legs, const csVector3& body, const csVector3& shift)
{
  csColliderActor::mesh = mesh;
  camera = 0;
  movable = mesh ? mesh->GetMovable () : 0;
  InitializeColliders (legs, body, shift);
}

void csColliderActor::InitializeColliders (iCamera* camera,
	const csVector3& legs, const csVector3& body, const csVector3& shift)
{
  mesh = 0;
  movable = 0;
  csColliderActor::camera = camera;
  InitializeColliders (legs, body, shift);
}

void csColliderActor::SetCamera (iCamera* camera, bool adjustRotation)
{ 
  this->camera = camera; 
  if (adjustRotation)
  {
    csQuaternion quat; quat.SetMatrix (camera->GetTransform().GetT2O());
    rotation = quat.GetEulerAngles ();
    // Angle fixups.
    /* @@@ FIXME: Are those right in the math sense or do they indicate
     * csQuaternion bugs? */
    if ((ABS(rotation.z - PI) < EPSILON)
      || (ABS(rotation.z + PI) < EPSILON))
    {
      rotation.x = PI-rotation.x;
      rotation.y = PI-rotation.y;
      rotation.z = 0;
    }
    else
      rotation.x = -rotation.x;
  }
}

#if 0
// Small helper function that returns the angle when given an x and y
// coordinate.
static float GetAngle (float x, float y)
{
  if (x > 1.0 )  x = 1.0;
  if (x < -1.0 ) x = -1.0;

  float angle = acos (x);
  if (y < 0)
    angle = 2*PI - angle;

  return angle;
}

static float Matrix2YRot (const csMatrix3& mat)
{
  csVector3 vec (0,0,1);
  vec = mat * vec;

  return GetAngle (vec.z, vec.x);
}
#endif

static inline bool FindIntersection (const csCollisionPair& cd,
    			   csVector3 line[2])
{
  csVector3 tri1[3]; tri1[0]=cd.a1; tri1[1]=cd.b1; tri1[2]=cd.c1;
  csVector3 tri2[3]; tri2[0]=cd.a2; tri2[1]=cd.b2; tri2[2]=cd.c2;
  csSegment3 isect;
  bool coplanar, ret;

  ret = csIntersect3::TriangleTriangle (tri1, tri2, isect, coplanar);
  line[0]=isect.Start ();
  line[1]=isect.End ();
  return ret;
}

int csColliderActor::CollisionDetect (
	iCollider *collider,
	iSector* sector,
	csReversibleTransform* transform,
	csReversibleTransform* old_transform)
{
  int hits = 0;

  // Do we need to check if a collision has really occurred because
  // of multiple sectors nearby.
  bool checkSectors = false;

  csVector3 testpos;
  csVector3 line[2];
  csCollisionPair temppair;
  csBox3 playerBoxStart = boundingBox;
  csBox3 playerBoxEnd = boundingBox;

  playerBoxStart.SetCenter (old_transform->GetOrigin()+boundingBox.GetCenter());
  playerBoxEnd.SetCenter (transform->GetOrigin()+boundingBox.GetCenter());

  csCollisionPair* CD_contact;

  csRef<iMeshWrapperIterator> objectIter = engine->GetNearbyMeshes (sector,
        playerBoxStart + playerBoxEnd,
        true);

  // Check if any portal mesh is close to the player object.
  while (objectIter->HasNext () && !checkSectors)
  {
    iMeshWrapper* meshwrap = objectIter->Next();
    if (meshwrap->GetPortalContainer())
      checkSectors = true;
  }
  objectIter->Reset();

  while (objectIter->HasNext ())
  {
    iMeshWrapper* meshWrapper = objectIter->Next ();

    iMovable* meshMovable = meshWrapper->GetMovable ();
    // Avoid hitting the mesh from this entity itself.
    if (meshWrapper != mesh)
    {
      cdsys->ResetCollisionPairs ();
      csReversibleTransform tr = meshMovable->GetFullTransform ();
      csColliderWrapper* otherwrap = csColliderWrapper::GetColliderWrapper (
      	meshWrapper->QueryObject ());
      if (!otherwrap) continue;
      iCollider* othercollider = otherwrap->GetCollider ();
      if (!(othercollider && cdsys->Collide (collider,
          transform, othercollider, &tr)))
        continue;

      // Check if we really collided
      bool reallycollided = false;

      CD_contact = cdsys->GetCollisionPairs ();
      size_t count = cdsys->GetCollisionPairCount();
      iSectorList * sectors = meshMovable->GetSectors();
      int sector_max = sectors->GetCount ();
      csReversibleTransform temptrans(*old_transform);

      for (size_t j = 0; j < count; j++ )
      {
        /*
        * Here we follow a segment from our current position to the
        * position of the collision. If the sector the collision occured
        * in is not the sector of the mesh we collided with,
        * this is invalid.
        */
        int sector_idx;
        iSector* CollisionSector;
        bool mirror=false;

        // Move the triangles from object space into world space
        temppair.a1 = transform->This2Other (CD_contact[j].a1);
        temppair.b1 = transform->This2Other (CD_contact[j].b1);
        temppair.c1 = transform->This2Other (CD_contact[j].c1);
        if (meshWrapper->GetMovable()->IsFullTransformIdentity())
        {
          temppair.a2 = CD_contact[j].a2;
          temppair.b2 = CD_contact[j].b2;
          temppair.c2 = CD_contact[j].c2;
        }
        else
        {
          temppair.a2 = tr.This2Other (CD_contact[j].a2);
          temppair.b2 = tr.This2Other (CD_contact[j].b2);
          temppair.c2 = tr.This2Other (CD_contact[j].c2);
        }
        if (checkSectors)
        {
          if(!FindIntersection (temppair, line))
            continue;
          // Collided at this line segment. Pick a point in the middle of
          // the segment to test.
          testpos=(line[0]+line[1])/2;

          // This follows a line segment from start to finish and returns
          // the sector you are ultimately in.
          CollisionSector = sector->FollowSegment (temptrans,
            testpos, mirror, true);

          // Iterate through all the sectors of the destination mesh,
          // incase it's in multiple sectors.
          for (sector_idx=0 ; sector_idx<sector_max ; sector_idx++)
          {
            // Check to see if this sector is the sector of the collision.
            if (CollisionSector == sectors->Get (sector_idx))
            {
              reallycollided = true;
              our_cd_contact.Push (temppair);
              // One valid sector is enough
              break;
            }
          }
        }
        else
        {
          reallycollided = true;
          our_cd_contact.Push (temppair);
        }
      }

      // We don't increase hits unless a collision really occurred after
      // all tests.
      if (reallycollided)
      {
        hits++;
	if (do_hit_meshes)
	  hit_meshes.Add (meshWrapper);
        if (cdsys->GetOneHitOnly ()) return 1;
      }
    }
  }

  return hits;
}

int csColliderActor::CollisionDetectIterative (
	iCollider *collider,
	iSector* sector,
	csReversibleTransform* transform,
	csReversibleTransform* old_transform, csVector3& maxmove)
{
  // The maximum position it's possible for the player to move to
  // If we collide at the start point or don't collide at the end point
  // then there is no need for recursion.
  int hits = CollisionDetect (collider, sector, transform, old_transform);
  if (hits == 0)
  {
    maxmove = transform->GetOrigin ();
    return hits;
  }

  cdsys->ResetCollisionPairs ();
  our_cd_contact.Empty ();

  maxmove = old_transform->GetOrigin();
  hits = CollisionDetect (collider, sector, old_transform, old_transform);
  if (hits > 0)
    return hits;

  // The upper and lower bounds
  csVector3 upper = transform->GetOrigin();
  csVector3 lower = old_transform->GetOrigin();

  csMatrix3 id;

  // The last hit that was made, used so that a hit will always be returned
  csVector3 lastHit = upper;

  //cdsys->SetOneHitOnly(true);
  // Repeatedly split the range with which to test the collision against
  while ((upper - lower).SquaredNorm() > EPSILON)
  {
    // Test in the middle between upper and lower bounds
    csOrthoTransform current (id, lower + (upper - lower)/2);
    cdsys->ResetCollisionPairs ();
    our_cd_contact.Empty ();

    hits = CollisionDetect (collider, sector, &current, old_transform);
    
    // Adjust bounds
    if (hits > 0)
    {
      lastHit = lower + (upper - lower)/2;
      upper = lastHit;
    }
    else
    {
      maxmove = lower + (upper - lower) / 2;
      lower = maxmove;
    }
  }
  if (hits == 0)
  {
    cdsys->SetOneHitOnly (false);
    // Make sure we actually return a hit
    csOrthoTransform current (id, lastHit);
    cdsys->ResetCollisionPairs ();
    our_cd_contact.Empty ();
    hits = CollisionDetect (collider, sector, &current, old_transform) > 0;
  }

  return hits;
}

bool csColliderActor::AdjustForCollisions (
	const csVector3& oldpos,
	csVector3& newpos,
	const csVector3& vel,
	float delta)
{
  revertMove = false;

  if (movable && movable->GetSectors()->GetCount() == 0)
    return true;

  int hits;
  size_t i;
  iSector* current_sector;
  if (movable) current_sector = movable->GetSectors ()->Get (0);
  else current_sector = camera->GetSector ();

  csMatrix3 id;

  csOrthoTransform transform_oldpos (id, oldpos);
  csOrthoTransform transform_newpos (id, newpos);
  csVector3 maxmove;

  // Part1: find body collisions => movement
  // Find possible colliding sectors.
  csVector3 localvel = (vel * delta);

  // Travel all relevant sectors and do collision detection.

  our_cd_contact.Empty ();
  cdsys->SetOneHitOnly (false);
  cdsys->ResetCollisionPairs ();

  // Perform recursive collision testing to minimise hits and
  // find distance we can travel
  if (cd)
    hits = CollisionDetectIterative (topCollider, current_sector,
      &transform_newpos, &transform_oldpos, maxmove);
  else
  {
    hits = 0;
    maxmove = transform_newpos.GetOrigin ();
  }

  // localvel is smaller because we can partly move the object in that direction
  localvel -= maxmove - oldpos;
  csVector3 correctedVel(localvel);

  for (i = 0; i < our_cd_contact.GetSize () ; i++ )
  {
    csCollisionPair& cd = our_cd_contact[i];
    csPlane3 obstacle (cd.a2, cd.b2, cd.c2);
    csVector3 normal = obstacle.Normal ();

    float norm = normal.Norm ();
    if (fabs (norm) < SMALL_EPSILON) continue;
    csVector3 unit = normal / norm;

    if (unit * localvel > 0) continue;

    // Bounce back

    correctedVel -= unit * (unit * correctedVel)*1.1; //(-(localvel % unit) % unit);
  }
 
  localvel = correctedVel;
  newpos = maxmove + localvel;

  transform_newpos = csOrthoTransform (csMatrix3(), newpos);
 
  // Part2: legs

  our_cd_contact.Empty ();

  transform_newpos = csOrthoTransform (csMatrix3(), newpos);

  cdsys->ResetCollisionPairs ();	

  if (cd)
    hits = CollisionDetect (bottomCollider, current_sector,
      &transform_newpos, &transform_oldpos);
  else
    hits = 0;

  bool stepDown;

  // Only able to step down if we aren't jumping or falling
  if (hits > 0 || vel.y != 0)
    stepDown = false;
  else
  {
    stepDown = true;

    // Try stepping down
    newpos.y -= bottomSize.y/2;
    transform_newpos = csOrthoTransform (csMatrix3(), newpos);

    our_cd_contact.Empty ();

    cdsys->ResetCollisionPairs ();	

    if (cd)
      hits = CollisionDetect (bottomCollider, current_sector,
        &transform_newpos, &transform_oldpos);
    else
      hits = 0;
  }

  // Falling unless proven otherwise
  onground = false;

  float maxJump = newpos.y + bottomSize.y;
  float max_y = -1e9;

  // Keep moving the model up until it no longer collides
  while (hits > 0 && newpos.y < maxJump)
  {
    bool adjust = false;
    for (i = 0; i < our_cd_contact.GetSize (); i++ )
    {
      csCollisionPair cd = our_cd_contact[i];
      csPlane3 obstacle (cd.a2, cd.b2, cd.c2);
      csVector3 normal = obstacle.Normal();
      float norm = normal.Norm ();

      // Ensure this is a big-enough triangle to count as a collision.
      if (fabs (norm) < 1e-4f ) continue;

      csVector3 n = normal / norm;

      csVector3 line[2];

      // This needs to be done for numerical inaccuracies in this test
      // versus the collision system test.
      if(!FindIntersection (cd,line))
        continue;

      // Is it a collision with a ground polygon?
      //  (this tests for the angle between ground and colldet
      //  triangle)
      if(!(n.y < 0.7))
          onground = true;
      adjust = true;
      max_y = MAX(MAX(line[0].y, line[1].y)+shift.y,max_y);
      if (max_y > maxJump)
      {
        max_y = maxJump;
        break;
      }
    }
    hits = 0;

    // This prevents us from going up if there is no surface to rest on.
    if(!onground && max_y > oldpos.y)
    {
        newpos.y = oldpos.y;
        break;
    }
    if (adjust)
    {
      // Temporarily lift the model up so that it passes the final check
      newpos.y = max_y + 0.01f;
      our_cd_contact.Empty ();

      transform_newpos = csOrthoTransform (csMatrix3(), newpos);

      cdsys->ResetCollisionPairs ();	

      if (cd)
        hits = CollisionDetect (bottomCollider, current_sector,
          &transform_newpos, &transform_oldpos);
      else
        hits = 0;
    }
  }

  if (!onground)
  {
    // Reaction force - Disabled because no distinction made between physics
    // engine predicted velocity and player controlled velocity

    // vel = (mat.GetInverse()*localvel)/delta;

    if (stepDown)
      // No steps here, so readjust position back up
      newpos.y += bottomSize.y / 2;
  }

  // Part 2.5: check top again and revert move if we're still in a wall.
  our_cd_contact.Empty ();
  cdsys->ResetCollisionPairs ();
  transform_newpos = csOrthoTransform (csMatrix3(), newpos);

  // Bring the model back down now that it has passed the final check
  if (onground)
    newpos.y -= 0.02f;

  if (cd)
    hits = CollisionDetect (topCollider, current_sector,
      &transform_newpos,&transform_oldpos);
  else
    cd = 0;

  hits = 0;
  for (i = 0; i < our_cd_contact.GetSize () ; i++ )
  {
    csCollisionPair& cd = our_cd_contact[i];
    csPlane3 obstacle (cd.a2, cd.b2, cd.c2);
    csVector3 normal = obstacle.Normal ();

    float norm = normal.Norm ();
    if (fabs (norm) < SMALL_EPSILON) continue;
    csVector3 unit = normal / norm;

    if (unit * (newpos - oldpos) > 0) continue;
    hits++;
  }

  if (hits > 0)
  {
    // No move possible without a collision with the torso
    revertMove = true;
    if(vel.y < 0)// && fabs(vel.x) < 0.01 && fabs(vel.y) < 0.01)
      revertCount++;
    newpos = oldpos;
    return false;
  }

  revertCount = 0;
  return true;
}

void csColliderActor::SetRotation (const csVector3& rot)
{
  rotation = rot;
  if (camera)
  {
    csMatrix3 rot;
    if (fabs (rotation.x) < SMALL_EPSILON && fabs (rotation.z) < SMALL_EPSILON)
      rot = csYRotMatrix3 (rotation.y);
    else
      rot = csXRotMatrix3 (rotation.x) * csYRotMatrix3 (rotation.y)
    	* csZRotMatrix3 (rotation.z);
    csOrthoTransform ot (rot, camera->GetTransform().GetOrigin ());
    camera->SetTransform (ot);
  }
}

bool csColliderActor::RotateV (float delta,
	const csVector3& angularVelocity)
{
  // rotation
  if (angularVelocity < SMALL_EPSILON)
    return false;

  csVector3 angle = angularVelocity * delta;
#if 0
  if (angleToReachFlag)
  {
    const csMatrix3& transf = movable->GetTransform ().GetT2O ();
    float current_yrot = Matrix2YRot (transf);
    current_yrot = atan2f (sin (current_yrot), cos (current_yrot) );
    float yrot_delta = fabs (atan2f (sin (angleToReach.y - current_yrot),
    	cos (angleToReach.y - current_yrot)));
    if (fabs(angle.y) > yrot_delta)
    {
      angle.y = (angle.y / fabs (angle.y)) * yrot_delta;
      angularVelocity = 0;
      angleToReachFlag = false;
    }
  }
#endif

  if (movable)
  {
    csYRotMatrix3 rotMat (angle.y);
    movable->SetTransform (movable->GetTransform().GetT2O() * rotMat);
  }
  else
  {
    SetRotation (rotation + angle);
  }
  return true;
}

#define ABS_MAX_FREEFALL_VELOCITY 107.3f
// Do the actual move
bool csColliderActor::MoveV (float delta,
	const csVector3& velBody)
{
  hit_meshes.Empty ();
  
  if (velBody < SMALL_EPSILON && velWorld < SMALL_EPSILON
  	&& onground)
    return false;  // didn't move anywhere

  csMatrix3 mat;

  // To test collision detection we use absolute position and transformation
  // (this is relevant if we are anchored). Later on we will correct that.
  csReversibleTransform fulltransf;
  if (movable)
  {
    fulltransf = movable->GetFullTransform ();
  }
  else
  {
    //fulltransf = camera->GetTransform ();
    // For camera we ignore the x and z rotation when moving so here we
    // construct a new transform containing only the y rotation.
    fulltransf.SetO2T (csYRotMatrix3 (rotation.y));
    fulltransf.SetOrigin (camera->GetTransform ().GetOrigin ());
  }
  mat = fulltransf.GetT2O ();

  csVector3 worldVel (fulltransf.This2OtherRelative (velBody) + velWorld);
  csVector3 oldpos (fulltransf.GetOrigin ());
  csVector3 newpos (worldVel*delta + oldpos);

  // Check for collisions and adjust position
  if (!AdjustForCollisions (oldpos, newpos, worldVel, delta))
    return false;                   // We haven't moved so return early
  
  bool mirror = false;

  // Update position to account for portals
  iSector* new_sector;
  if (movable)
    new_sector = movable->GetSectors ()->Get (0);
  else
    new_sector = camera->GetSector ();
  iSector* old_sector = new_sector;

  // @@@ Jorrit: had to do this add!
  // We need to measure slightly above the position of the actor or else
  // we won't really cross a portal.
  float height5 = (bottomSize.y + topSize.y) / 20.0;
  newpos.y += height5;
  csMatrix3 id;
  csOrthoTransform transform_oldpos (id, oldpos + csVector3 (0, height5, 0));

  new_sector = new_sector->FollowSegment (transform_oldpos,
      newpos, mirror, true);
  newpos.y -= height5;
  if (new_sector != old_sector)
  {
    if (movable)
      movable->SetSector (new_sector);
    else
      camera->SetSector (new_sector);
  }

  if (!onground)
  {
    // gravity! move down!
    velWorld.y  -= gravity * delta;
    /*
    * Terminal velocity
    *   ((120 miles/hour  / 3600 second/hour) * 5280 feet/mile)
    *   / 3.28 feet/meter = 53.65 m/s
    */
    // The body velocity is figured in here too.
    if (velWorld.y < 0)
    {
      if (fulltransf.This2OtherRelative(velBody).y
      		+ velWorld.y < -(ABS_MAX_FREEFALL_VELOCITY))
        velWorld.y = -(ABS_MAX_FREEFALL_VELOCITY)
		- fulltransf.This2OtherRelative(velBody).y;
      if (velWorld.y > 0)
        velWorld.y = 0;
    }
  }
  else
  {
    if (velWorld.y < 0)
    {
      velWorld.y = 0;
    }
  }

  if (movable)
  {
    movable->GetTransform ().SetOrigin (newpos);
    mesh->PlaceMesh ();
    movable->UpdateMove ();
  }
  else
  {
    camera->GetTransform ().SetOrigin (newpos);
  }

  return true;
}

#define MAX_CD_INTERVAL 1
#define MAX_CD_ITERATIONS 20

static float ComputeLocalMaxInterval (
	const csVector3& bodyVel,
	const csVector3& intervalSize)
{
  float local_max_interval =
    MIN (MIN ((fabs (bodyVel.y) < SMALL_EPSILON)
  	? MAX_CD_INTERVAL
	: fabs (intervalSize.y/bodyVel.y), (fabs (bodyVel.x) < SMALL_EPSILON)
		? MAX_CD_INTERVAL
		: fabs (intervalSize.x/bodyVel.x)),
			(fabs (bodyVel.z) < SMALL_EPSILON)
			? MAX_CD_INTERVAL
			: fabs (intervalSize.z/bodyVel.z));
  return local_max_interval;
}

bool csColliderActor::Move (float delta, float speed, const csVector3& velBody,
	const csVector3& angularVelocity)
{
  // Artificial cap to avoid falling through objects when the framerate
  // is extremely low.
  if (delta > .3f) delta = .3f;

  //float local_max_interval;
  bool rc = false;

  csReversibleTransform fulltransf;
  if (movable)
  {
    fulltransf = movable->GetFullTransform ();
  }
  else
  {
    //fulltransf = camera->GetTransform ();
    // For camera we ignore the x and z rotation when moving so here we
    // construct a new transform containing only the y rotation.
    fulltransf.SetO2T (csYRotMatrix3 (rotation.y));
    fulltransf.SetOrigin (camera->GetTransform ().GetOrigin ());
  }
  //const csMatrix3& transf = fulltransf.GetT2O ();
  //float yrot = Matrix2YRot (transf);

  // Calculate the total velocity (body and world) in OBJECT space.
  csVector3 bodyVel (fulltransf.Other2ThisRelative (velWorld) + velBody);

  float local_max_interval = ComputeLocalMaxInterval (bodyVel, intervalSize - csVector3(0.005f));

  // Compensate for speed
  local_max_interval /= speed;

  int maxiter = MAX_CD_ITERATIONS;

  while (delta > local_max_interval && maxiter > 0)
  {
    maxiter--;
    rc |= MoveV (local_max_interval * speed, velBody);

    rc |= RotateV (local_max_interval * speed, angularVelocity);
    //yrot = Matrix2YRot (transf);

    if (!rc) return rc;

    // The velocity may have changed by now
    bodyVel = fulltransf.Other2ThisRelative(velWorld) + velBody;

    delta -= local_max_interval;
    local_max_interval = ComputeLocalMaxInterval (bodyVel, intervalSize);

    // Compensate for speed
    local_max_interval /= speed;
    // Err on the side of safety
    local_max_interval -= 0.005f;
  }

  if (delta)
  {
    rc |= MoveV (delta * speed, velBody);
    rc |= RotateV (delta * speed, angularVelocity);
  }

  return rc;
}


