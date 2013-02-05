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
#include "cssysdef.h"

#include "csutil/stringquote.h"
#include "igeom/trimesh.h"
#include "imesh/objmodel.h"

// Bullet includes
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.h"

#include "collisionterrain.h"
#include "collisionobject2.h"
#include "bulletsystem.h"

using namespace CS::Collisions;
using namespace CS::Physics;

CS_PLUGIN_NAMESPACE_BEGIN (Bullet2)
{

csBulletColliderTerrain::csBulletColliderTerrain ( float* gridData, 
                                   iTerrainCell *cell, 
                                   float minHeight, float maxHeight,
                                   float internalScale, csBulletSystem* sys)
                                   :  scfImplementationType (this),
                                   cell (cell), heightData (gridData)
{
  collSystem = sys;

  // create shape
  shape = new btHeightfieldTerrainShape (cell->GetGridWidth (), cell->GetGridHeight (),
                                   gridData, 1.0f, minHeight, maxHeight,
                                   1, PHY_FLOAT, false);
  
  // Apply the local scaling on the shape
  const csVector3& size = cell->GetSize ();
  localScale.setValue (size[0] * internalScale / (cell->GetGridWidth () - 1),
    internalScale,
    size[2] * internalScale/ (cell->GetGridHeight () - 1));
  shape->setLocalScaling (localScale);

  margin = 0.04 * collSystem->GetInverseInternalScale ();
}

csBulletColliderTerrain::~csBulletColliderTerrain ()
{
  delete heightData;
}

void csBulletColliderTerrain::UpdataMinHeight (float minHeight)
{
  //this->initialize (m_heightStickWidth, m_heightStickLength,
  //  heightData, 1.0f, minHeight, m_maxHeight, 1, PHY_FLOAT, false);

}

void csBulletColliderTerrain::UpdateMaxHeight (float maxHeight)
{
  //this->initialize (m_heightStickWidth, m_heightStickLength,
  //  heightData, 1.0f, m_minHeight, maxHeight, 1, PHY_FLOAT, false);
}

void csBulletColliderTerrain::UpdateHeight (const csRect& area)
{
  int w = cell->GetGridWidth ();
  int h = cell->GetGridHeight ();
  csLockedHeightData newData = cell->LockHeightData (area);

  for (int y = 0; y < area.Height (); y++)
  {
    for (int x = 0; x < area.Width (); x++)
    {
      heightData[CSToBulletIndex2D (x + area.xmin, y + area.ymin, w, h)] = newData.data[y * w + x];
    }
  }
}



csBulletCollisionTerrain::csBulletCollisionTerrain (iTerrainSystem* terrain, float minimumHeight,
                                                  float maximumHeight, csBulletSystem* sys)
  : scfImplementationType (this), terrainSystem (terrain), minimumHeight (minimumHeight), 
  maximumHeight (maximumHeight)
{
  collSystem = sys;

  terrain->AddCellLoadListener (this);
  terrain->AddCellHeightUpdateListener (this);

  // Find the transform of the terrain
  csRef<iMeshObject> mesh = scfQueryInterface<iMeshObject> (terrain);
  terrainTransform = mesh->GetMeshWrapper ()->GetMovable ()->GetFullTransform ();

  // Create COs for already loaded cells
  for (size_t i = 0; i<terrainSystem->GetCellCount (); i++)
  {
    iTerrainCell* cell = terrainSystem->GetCell (i);
    if (cell->GetLoadState () != iTerrainCell::Loaded)
      continue;

    LoadCellToCollider (cell);       
  }
}

csBulletCollisionTerrain::~csBulletCollisionTerrain ()
{
  if (collSector)
  {
    for (size_t i = 0; i < bodies.GetSize (); i++)
    {
      collSector->RemoveCollisionObject (bodies[i]);
    }
  }
}

void csBulletCollisionTerrain::OnCellLoad (iTerrainCell *cell)
{
  LoadCellToCollider (cell);
}

void csBulletCollisionTerrain::OnCellPreLoad (iTerrainCell *cell)
{
}

void csBulletCollisionTerrain::OnCellUnload (iTerrainCell *cell)
{
  for (int i = 0; i < (int)bodies.GetSize (); ++i)
  {
    csBulletRigidBody* body = bodies[i];

    csBulletColliderTerrain* collider = dynamic_cast<csBulletColliderTerrain*>(body->GetCollider ());
    if (collider->cell == cell) 
    {
      collSector->RemoveCollisionObject (bodies[i]);
      bodies.DeleteIndexFast (i);
      break;
    }
  }
}

void csBulletCollisionTerrain::LoadCellToCollider (iTerrainCell *cell)
{
  float minHeight,maxHeight;
  csLockedHeightData cellData = cell->GetHeightData ();
  // Check if the min/max have to be computed
  bool needExtremum =  (minimumHeight == 0.0f && maximumHeight == 0.0f);
  if  (needExtremum)
    minHeight = maxHeight = cellData.data[0];
  int gridWidth = cell->GetGridWidth ();
  int gridHeight = cell->GetGridHeight ();

  // Initialize the bullet terrain height data
  float* btHeightData = new float[gridHeight*gridWidth];
  for (int j=0;j<gridHeight;j++)
  {
    for (int i=0;i<gridWidth;i++)
    {
      float height =
	btHeightData[CSToBulletIndex2D (i, j, gridWidth, gridHeight)]
	= cellData.data[j * gridWidth + i];

      if (needExtremum)
      {
        minHeight = csMin (minHeight, height);
        maxHeight = csMax (maxHeight, height);
      }
    }
  }

  // set the transform
  csOrthoTransform cellTransform (terrainTransform);
  csVector3 cellPosition  (cell->GetPosition ()[0], 0.0f, cell->GetPosition ()[1]);

  csVector3 cellCenter (
    cell->GetSize ()[0] * 0.5f,
    (maxHeight - minHeight) * 0.5f + minHeight,
    cell->GetSize ()[2] * 0.5f);

  cellTransform.SetOrigin (
    terrainTransform.GetOrigin () +
    terrainTransform.This2OtherRelative (cellPosition) +
    terrainTransform.This2OtherRelative (cellCenter));

  // Create the terrain shape
  csBulletColliderTerrain* collider = new csBulletColliderTerrain (
    btHeightData, cell, minHeight, maxHeight,
    collSystem->GetInternalScale (), collSystem);

  // Create the rigid body and add it to the world

  // TODO: Re-use factory
  cellFactory = collSystem->CreateRigidBodyFactory (collider);
  cellFactory->SetCollisionGroup (collSystem->FindCollisionGroup ("Static"));
  csRef<iRigidBody> ibody = cellFactory->CreateRigidBody ();
  csBulletRigidBody* body = dynamic_cast<csBulletRigidBody*> (&*ibody);
  
  body->SetTransform (cellTransform);
  
  //btRigidBody* btBody = body->btBody;
  //btBody->setCollisionFlags
  //(body->getCollisionFlags () | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);    // do not debug draw the terrain

  // Add to body collection and to world
  bodies.Push (body);
  if (collSector)
  {
    collSector->AddCollisionObject (body);
  }

}

// height in the given rectangle of the given cell has changed
void csBulletCollisionTerrain::OnHeightUpdate (iTerrainCell* cell, const csRect& rectangle) 
{
  // find cell collider:
  csBulletColliderTerrain* collider = GetCellCollider (cell);
  
  // this method is actually fired prior to the load event
  if (collider)
  {
    collider->UpdateHeight (rectangle);
  }
}

csBulletRigidBody* csBulletCollisionTerrain::GetCellBody (iTerrainCell* cell) const
{
  for (int i = 0; i < (int)bodies.GetSize (); ++i)
  {
    csBulletRigidBody* body = bodies[i];

    csBulletColliderTerrain* collider = dynamic_cast<csBulletColliderTerrain*>(body->GetCollider ());
    if (collider->cell == cell) return body;
  }
  return nullptr;
}

csBulletColliderTerrain* csBulletCollisionTerrain::GetCellCollider (iTerrainCell* cell) const
{
  for (int i = 0; i < (int)bodies.GetSize (); ++i)
  {
    csBulletRigidBody* body = bodies[i];

    csBulletColliderTerrain* collider = dynamic_cast<csBulletColliderTerrain*>(body->GetCollider ());
    if (collider->cell == cell) return collider;
  }
  return nullptr;
}

void csBulletCollisionTerrain::RemoveRigidBodies ()
{
  if (!collSector) return;

  for (size_t i = 0; i < bodies.GetSize (); i++)
  {
    collSector->RemoveCollisionObject (bodies[i]);
  }
}

void csBulletCollisionTerrain::AddRigidBodies (csBulletSector* sector)
{
  collSector = sector;
  for (size_t i = 0; i < bodies.GetSize (); i++)
  {
    iTerrainCell* cell = terrainSystem->GetCell (i);
    if (cell->GetLoadState () != iTerrainCell::Loaded)
      continue;
    sector->AddCollisionObject (bodies[i]);
  }
}

}
CS_PLUGIN_NAMESPACE_END (Bullet2)
