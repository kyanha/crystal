/*
    Copyright (C) 2011-2012 Christian Van Brussel, Institute of Information
      and Communication Technologies, Electronics and Applied Mathematics
      at Universite catholique de Louvain, Belgium
      http://www.uclouvain.be/en-icteam.html
    Copyright (C) 2012 by Dominik Seifert
    Copyright (C) 2011 by Liu Lu

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

#ifndef __CS_BULLET_COLLISIONTERRAIN_H__
#define __CS_BULLET_COLLISIONTERRAIN_H__

#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "csutil/weakref.h"
#include "csgeom/plane3.h"
#include "imesh/terrain2.h"
#include "ivaria/collisions.h"
#include "common2.h"
#include "colliderprimitives.h"
#include "rigidbody2.h"

struct csLockedHeightData;
struct iTerrainSystem;
struct iTriangleMesh;

CS_PLUGIN_NAMESPACE_BEGIN (Bullet2)
{

class csBulletSector;
class csBulletSystem;

class csBulletColliderTerrain : public scfVirtImplementationExt1<
  csBulletColliderTerrain, csBulletCollider, CS::Collisions::iColliderTerrain>
{
  friend class csBulletCollisionTerrain;
  
private:
  csWeakRef<iTerrainCell> cell;
  float* heightData;

  btVector3 localScale;

  void UpdataMinHeight (float minHeight);
  void UpdateMaxHeight (float maxHeight);
  void UpdateHeight (const csRect& area);

protected:
  virtual float ComputeShapeVolume () const { return 0; }

public:
  csBulletColliderTerrain (float* gridData, iTerrainCell* cell,
			   float minHeight, float maxHeight, float internalScale, csBulletSystem* sys);
  virtual ~csBulletColliderTerrain ();
  
  virtual CS::Collisions::ColliderType GetColliderType () const
  { return CS::Collisions::COLLIDER_TERRAIN_CELL; }

  virtual iTerrainCell* GetCell () const { return cell; }
  
  btHeightfieldTerrainShape* GetBulletHeightfieldShape () const
  { return static_cast<btHeightfieldTerrainShape*>(shape); }
};

class csBulletCollisionTerrain:
  public scfImplementation3<csBulletCollisionTerrain, 
  CS::Collisions::iCollisionTerrain, iTerrainCellLoadCallback, iTerrainCellHeightDataCallback>
{
  friend class csBulletSector;
  friend class csBulletCollisionObject;
  
  csRefArray<csBulletRigidBody> bodies;
  csOrthoTransform terrainTransform;
  csWeakRef<csBulletSector> collSector;
  csWeakRef<csBulletSystem> collSystem;
  csWeakRef<iTerrainSystem> terrainSystem;
  float minimumHeight;
  float maximumHeight;
  
  csRef<CS::Physics::iRigidBodyFactory> cellFactory;

  void LoadCellToCollider (iTerrainCell* cell);
public:
  csBulletCollisionTerrain (iTerrainSystem* terrain,
    float minimumHeight, float maximumHeight,
    csBulletSystem* sys);
  virtual ~csBulletCollisionTerrain ();

  virtual iTerrainSystem* GetTerrain () const {return terrainSystem;}

  //-- iTerrainCellLoadCallback
  virtual void OnCellLoad (iTerrainCell *cell);
  virtual void OnCellPreLoad (iTerrainCell *cell);
  virtual void OnCellUnload (iTerrainCell *cell);

  //-- iTerrainCellHeightDataCallback
  virtual void OnHeightUpdate (iTerrainCell* cell, const csRect& rectangle);

  /**
   * Returns the collider that represents the given cell in the physical world
   */
  csBulletRigidBody* GetCellBody (iTerrainCell* cell) const;
  csBulletRigidBody* GetCellBody (size_t index) const { return bodies[index]; }
  
  csBulletColliderTerrain* GetCellCollider (iTerrainCell* cell) const;
  csBulletColliderTerrain* GetCellCollider (size_t index) const
  { return dynamic_cast<csBulletColliderTerrain*>(bodies[index]->GetCollider ()); }
  
  void AddRigidBodies (csBulletSector* sector);
  void RemoveRigidBodies ();
};


}
CS_PLUGIN_NAMESPACE_END (Bullet2)
#endif
