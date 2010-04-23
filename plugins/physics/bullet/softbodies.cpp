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

#include "cssysdef.h"
#include "csgeom/matrix3.h"
#include "csgeom/transfrm.h"
#include "csgeom/quaternion.h"
#include "csgeom/vector3.h"
#include "iengine/movable.h"
#include "iutil/strset.h"

// Bullet includes.
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"
#include "BulletSoftBody/btSoftBody.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"

#include "softbodies.h"

CS_PLUGIN_NAMESPACE_BEGIN(Bullet)
{

csBulletSoftBody::csBulletSoftBody (csBulletDynamicsSystem* dynSys,
				    btSoftBody* body)
  : scfImplementationType (this), dynSys (dynSys), body (body)
{
}

csBulletSoftBody::~csBulletSoftBody ()
{
  btSoftRigidDynamicsWorld* softWorld =
    static_cast<btSoftRigidDynamicsWorld*> (dynSys->bulletWorld);
  softWorld->removeSoftBody (body);
  delete body;
}

void csBulletSoftBody::DebugDraw (iView* rview)
{
  if (!dynSys->debugDraw)
  {
    dynSys->debugDraw = new csBulletDebugDraw (dynSys->inverseInternalScale);
    dynSys->bulletWorld->setDebugDrawer (dynSys->debugDraw);
  }

  btSoftBodyHelpers::Draw (body, dynSys->debugDraw);
  dynSys->debugDraw->DebugDraw (rview);
}

void csBulletSoftBody::SetMass (float mass)
{
  CS_ASSERT(mass > 0);

  btSoftRigidDynamicsWorld* softWorld =
    static_cast<btSoftRigidDynamicsWorld*> (dynSys->bulletWorld);
  softWorld->removeSoftBody (body);

  body->setTotalMass (mass);

  softWorld->addSoftBody (body);
}

float csBulletSoftBody::GetMass ()
{
  return body->getTotalMass ();
}

}
CS_PLUGIN_NAMESPACE_END(Bullet)
