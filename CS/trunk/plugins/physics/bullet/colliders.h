/*
  Copyright (C) 2010 Christian Van Brussel, Communications and Remote
      Sensing Laboratory of the School of Engineering at the 
      Universite catholique de Louvain, Belgium
      http://www.tele.ucl.ac.be

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

#ifndef __CS_BULLET_COLLIDERS_H__
#define __CS_BULLET_COLLIDERS_H__

#include "bullet.h"
#include "common.h"

CS_PLUGIN_NAMESPACE_BEGIN(Bullet)
{

class csBulletCollider : public scfImplementation1<csBulletCollider,
  iDynamicsSystemCollider>
{
  friend class csBulletDynamicsSystem;
  friend class csBulletRigidBody;

  csOrthoTransform localTransform;
  csBulletDynamicsSystem* dynSys;
  csBulletRigidBody* body;
  csRef<iDynamicsColliderCollisionCallback> collCb;
  bool isStaticBody;
  csColliderGeometryType geomType;
  btCollisionShape* shape;
  float density;
  float friction;
  float softness;
  float elasticity;

  // Data we need to keep for the body so we can clean it up
  // later.
  size_t triangleCount, vertexCount;
  btVector3* vertices;
  int* indices;

public:
  csBulletCollider (csBulletDynamicsSystem* dynSys,
		    csBulletRigidBody* body = NULL, bool isStaticBody = false);
  virtual ~csBulletCollider ();

  virtual bool CreateSphereGeometry (const csSphere& sphere);
  virtual bool CreatePlaneGeometry (const csPlane3& plane);
  virtual bool CreateConvexMeshGeometry (iMeshWrapper *mesh);
  virtual bool CreateMeshGeometry (iMeshWrapper *mesh);
  void RebuildMeshGeometry ();
  virtual bool CreateBoxGeometry (const csVector3& box_size);
  virtual bool CreateCapsuleGeometry (float length, float radius);
  virtual bool CreateCylinderGeometry (float length, float radius);

  virtual void SetCollisionCallback (
    iDynamicsColliderCollisionCallback* cb);

  virtual void SetFriction (float friction);
  virtual void SetSoftness (float softness);
  virtual void SetDensity (float density);
  virtual void SetElasticity (float elasticity);
  virtual float GetFriction ();
  virtual float GetSoftness ();
  virtual float GetDensity ();
  virtual float GetElasticity ();

  virtual void FillWithColliderGeometry (
      csRef<iGeneralFactoryState> genmesh_fact);
  virtual csColliderGeometryType GetGeometryType () 
  { return geomType; }
  virtual csOrthoTransform GetTransform ();
  virtual csOrthoTransform GetLocalTransform ();
  virtual void SetTransform (const csOrthoTransform& trans);

  virtual bool GetBoxGeometry (csVector3& size); 
  virtual bool GetSphereGeometry (csSphere& sphere);
  virtual bool GetPlaneGeometry (csPlane3& plane); 
  virtual bool GetCylinderGeometry (float& length, float& radius);
  virtual bool GetCapsuleGeometry (float& length, float& radius);
  virtual bool GetMeshGeometry (csVector3*& vertices, size_t& vertexCount,
				int*& indices, size_t& triangleCount);
  virtual bool GetConvexMeshGeometry (csVector3*& vertices, size_t& vertexCount,
				      int*& indices, size_t& triangleCount);

  virtual void MakeStatic ();
  virtual void MakeDynamic ();
  virtual bool IsStatic ();

  float GetVolume ();
};

}
CS_PLUGIN_NAMESPACE_END(Bullet)

#endif //__CS_BULLET_COLLIDERS_H__
