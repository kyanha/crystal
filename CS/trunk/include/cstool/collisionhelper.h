/*
    Copyright (C) 2012 Christian Van Brussel, Institute of Information
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
#ifndef __CS_CSTOOL_COLLISIONHELPER_H
#define __CS_CSTOOL_COLLISIONHELPER_H

/**\file
 * Utility class to help building and managing collision and physical systems
 */

#include "csutil/strhash.h"
#include "csutil/scf_implementation.h"
#include "iutil/strset.h"

struct iCollection;
struct iEngine;
struct iMeshFactoryWrapper;
struct iMeshWrapper;
struct iSector;
struct iSyntaxService;
struct iTriangleMesh;

namespace CS {
namespace Physics {

struct iJointFactory;
struct iPhysicalObjectFactory;
struct iRigidBodyFactory;

} // namespace Physics
} // namespace CS

namespace CS {
namespace Collisions {

struct iCollider;
struct iCollisionObjectFactory;
struct iCollisionSystem;
struct iConvexDecomposer;

/**
 * A class to help managing the colliders and physical models of the meshes and scenes.
 */
class CS_CRYSTALSPACE_EXPORT CollisionHelper
{
  iObjectRegistry* objectRegistry;
  csRef<iEngine> engine;
  csRef<CS::Collisions::iCollisionSystem> collisionSystem;
  csRef<CS::Collisions::iConvexDecomposer> decomposer;
  csRef<iSyntaxService> synldr;
  csStringHash xmltokens;
  csStringID baseID;
  csStringID collisionID;

  void ReportError (const char* msg, ...);
  void ReportWarning (const char* msg, ...);

  void ParseCollisionObjectProperties
    (iDocumentNode* node, CS::Collisions::iCollisionObjectFactory* object,
     iLoaderContext* loaderContext, iBase* context) const;
  void ParsePhysicalObjectProperties
    (iDocumentNode* node, CS::Physics::iPhysicalObjectFactory* object,
     iLoaderContext* loaderContext, iBase* context) const;
  bool ParseJointConstraint
    (iDocumentNode *node, bool& x, bool& y, bool& z, csVector3& min, csVector3& max) const;

public:
  /// Initialize this collision helper
  bool Initialize (iObjectRegistry* objectRegistry,
		   CS::Collisions::iCollisionSystem* collisionSystem = nullptr,
		   CS::Collisions::iConvexDecomposer* decomposer = nullptr);

  /// Creates and adds all collision objects of all meshes in the given engine to the collision system
  void InitializeCollisionObjects (iEngine* engine, 
				   iCollection* collection = nullptr) const;
  
  /// Creates and adds all collision objects of all meshes in the given sector to the collision system
  void InitializeCollisionObjects (iSector* sector, 
				   iCollection* collection = nullptr) const;

  /// Recursively creates and adds all collision objects of the mesh and it's children to the collision system
  void InitializeCollisionObjects (iSector* sector, 
				   iMeshWrapper* mesh) const;

  /**
   * Search for the collision iTriangleMesh of the given mesh. Return nullptr in case
   * there are explicitely none.
   */
  iTriangleMesh* FindCollisionMesh (iMeshWrapper* mesh) const;

  /**
   * Search for the collision iTriangleMesh of the given mesh factory. Return nullptr
   * in case there are explicitely none.
   */
  iTriangleMesh* FindCollisionMesh (iMeshFactoryWrapper* meshFactory) const;

  // TODO: find coll/physical factory

  /**
   * Perform a convex decomposition on the given concave triangle mesh and compound the
   * resulting convex parts into the given collider.
   */
  void DecomposeConcaveMesh (iTriangleMesh* mesh, CS::Collisions::iCollider* collider,
			     CS::Collisions::iConvexDecomposer* decomposer) const;

  /**
   * Perform a convex decomposition on the given concave mesh wrapper and compound the
   * resulting convex parts into the given collider. This is equivalent to a call to
   * FindCollisionMesh() followed by a call to
   * DecomposeConcaveMesh(iTriangleMesh*,CS::Collisions::iCollider*,CS::Collisions::iConvexDecomposer*).
   */
  void DecomposeConcaveMesh (iMeshWrapper* mesh, CS::Collisions::iCollider* collider,
			     CS::Collisions::iConvexDecomposer* decomposer) const;

  // TODO: CollisionParser?
  // TODO: Document me
  csPtr<CS::Collisions::iCollider> ParseCollider
    (iDocumentNode* node, csTransform& transform, iLoaderContext* loaderContext,
     iBase* context) const;
  csPtr<CS::Collisions::iCollisionObjectFactory> ParseCollisionObjectFactory
    (iDocumentNode* node, iLoaderContext* loaderContext, iBase* context) const;
  csPtr<CS::Collisions::iCollisionObjectFactory> ParseCollisionObjectSimpleFactory
    (iDocumentNode* node, iLoaderContext* loaderContext, iBase* context) const;
  csPtr<CS::Collisions::iCollisionObjectFactory> ParseCollisionObjectGhostFactory
    (iDocumentNode* node, iLoaderContext* loaderContext, iBase* context) const;
  csPtr<CS::Physics::iRigidBodyFactory> ParseRigidBodyFactory
    (iDocumentNode* node, iLoaderContext* loaderContext, iBase* context) const;
  csPtr<CS::Physics::iJointFactory> ParseJointFactory
    (iDocumentNode* node, iLoaderContext* loaderContext, iBase* context) const;
};

} // namespace Collisions
} // namespace CS

#endif // __CS_CSTOOL_COLLISIONHELPER_H
