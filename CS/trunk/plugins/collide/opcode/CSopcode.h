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

#ifndef __CS_OPCODE_PLUGIN_H__
#define __CS_OPCODE_PLUGIN_H__

#include "iutil/comp.h"
#include "csgeom/vector3.h"
#include "ivaria/collider.h"
#include "csgeom/transfrm.h"
#include "CSopcodecollider.h"
#include "Opcode.h"

struct iObjectRegistry;

class csOPCODECollideSystem : public iCollideSystem
{
public:
	Opcode::AABBTreeCollider TreeCollider;
  //  CollisionFaces CollideFaces;
  //  csDLinkList CacheList;
	Opcode::BVTCache ColCache;
	csOPCODECollider* col1;
	csReversibleTransform T1;
	csOPCODECollider* col2;
	csReversibleTransform T2;
  csCollisionPair *pairs;
  int N_pairs;
  iObjectRegistry *object_reg;
 
  SCF_DECLARE_IBASE;

  csOPCODECollideSystem (iBase* parent);
  virtual ~csOPCODECollideSystem ();
  bool Initialize (iObjectRegistry* iobject_reg);

  virtual csPtr<iCollider> CreateCollider (iPolygonMesh* mesh);
  virtual csPtr<iCollider> CreateSphereCollider (iMeshObject* sphere);
  virtual csPtr<iCollider> CreateBoxCollider (iMeshObject* box);

  /**
   * Test collision between two colliders.
   * This is only supported for iCollider objects created by
   * this plugin. Returns false if no collision or else true.
   * The collisions will be added to the collision pair array
   * that you can query with GetCollisionPairs and reset/clear
   * with ResetCollisionPairs. Every call to Collide will
   * add to that array.
   */
  virtual bool Collide (
  	iCollider* collider1, const csReversibleTransform* trans1,
  	iCollider* collider2, const csReversibleTransform* trans2);

  /**
   * Get pointer to current array of collision pairs.
   * This array will grow with every call to Collide until you clear
   * it using 'ResetCollisionPairs'.
   */
  virtual csCollisionPair* GetCollisionPairs ();

  /**
   * Get number of collision pairs in array.
   */
  virtual int GetCollisionPairCount ();

  /**
   * Reset the array with collision pairs.
   */
  virtual void ResetCollisionPairs ();

  /**
   * Indicate if we are interested only in the first hit that is found.
   * This is only valid for CD algorithms that actually allow the
   * detection of multiple CD hit points.
   */
  virtual void SetOneHitOnly (bool o);

  /**
   * Return true if this CD system will only return the first hit
   * that is found. For CD systems that support multiple hits this
   * will return the value set by the SetOneHitOnly() function.
   * For CD systems that support one hit only this will always return true.
   */
  virtual bool GetOneHitOnly ();

  /**
   * Test if an object can move to a new position. The new position
   * vector will be modified to reflect the maximum new position that the
   * object could move to without colliding with something. This function
   * will return:
   * <ul>
   * <li>-1 if the object could not move at all (i.e. stuck at start position).
   * <li>0 if the object could not move fully to the desired position.
   * <li>1 if the object can move unhindered to the end position.
   * </ul>
   * <p>
   * This function will reset the collision pair array. If there was a
   * collision along the way the array will contain the information for
   * the first collision preventing movement.
   * <p>
   * The given transform should be the transform of the object corresponding
   * with the old position. 'colliders' and 'transforms' should be arrays
   * with 'num_colliders' elements for all the objects that we should
   * test against.
   */
  virtual int CollidePath (
    iCollider* collider, const csReversibleTransform* trans,
    csVector3& newpos, int num_colliders, iCollider** colliders,
    csReversibleTransform** transforms);

  struct Component : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE (csOPCODECollideSystem);
    virtual bool Initialize (iObjectRegistry* object_reg)
    {
      return scfParent->Initialize (object_reg);
    }
  } scfiComponent;
};

#endif // __CS_OPCODE_PLUGIN_H__
