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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#ifndef __CS_IKPHYSICAL_H__
#define __CS_IKPHYSICAL_H__

#include "csutil/scf_implementation.h"
#include "csgeom/transfrm.h"
#include "cstool/animnodetmpl.h"
#include "csutil/leakguard.h"
#include "csutil/weakref.h"
#include "csutil/csstring.h"
#include "imesh/animnode/ik.h"
#include "ivaria/bullet.h"
#include "imesh/animnode/ragdoll.h"
#include "iutil/comp.h"

CS_PLUGIN_NAMESPACE_BEGIN(IKPhysical)
{

  CS_DECLARE_ANIMNODE_MANAGER(IKPhysicalNode, IKNode, CS::Animation::iSkeletonIKPhysicalNodeFactory);

  struct EffectorData
  {
    CS::Animation::iBodyChain* chain;
    CS::Animation::BoneID bone;
    csOrthoTransform transform;
  };

  class IKPhysicalNodeFactory
    : public scfImplementation3<IKPhysicalNodeFactory,
    scfFakeInterface<CS::Animation::iSkeletonAnimNodeFactory>,
    scfFakeInterface<CS::Animation::iSkeletonIKNodeFactory>,
    CS::Animation::iSkeletonIKPhysicalNodeFactory>,
    CS::Animation::csSkeletonAnimNodeFactorySingle
  {
  public:
    CS_LEAKGUARD_DECLARE(IKPhysicalNodeFactory);

    IKPhysicalNodeFactory (IKPhysicalNodeManager* manager, const char *name);

    //-- CS::Animation::iSkeletonIKNodeFactory
    virtual void SetBodySkeleton (CS::Animation::iBodySkeleton* skeleton);
    virtual CS::Animation::iBodySkeleton* GetBodySkeleton () const;
    virtual CS::Animation::EffectorID AddEffector (CS::Animation::iBodyChain* chain,
						   CS::Animation::BoneID bone,
						   csOrthoTransform& transform);
    virtual void RemoveEffector (CS::Animation::EffectorID effector);

    inline virtual void SetChildNode (CS::Animation::iSkeletonAnimNodeFactory* factory)
    { CS::Animation::csSkeletonAnimNodeFactorySingle::SetChildNode (factory); }
    inline virtual iSkeletonAnimNodeFactory* GetChildNode () const
    { return CS::Animation::csSkeletonAnimNodeFactorySingle::GetChildNode (); }

    //-- CS::Animation::iSkeletonIKPhysicalNodeFactory
    virtual void SetChainAutoReset (bool reset);
    virtual bool GetChainAutoReset () const;

    //-- CS::Animation::iSkeletonAnimNodeFactory
    virtual csPtr<CS::Animation::iSkeletonAnimNode> CreateInstance
      (CS::Animation::iSkeletonAnimPacket* packet, CS::Animation::iSkeleton* skeleton);
    inline virtual const char* GetNodeName () const
      { return csSkeletonAnimNodeFactorySingle::GetNodeName (); }
    virtual CS::Animation::iSkeletonAnimNodeFactory* FindNode (const char* name);

  protected:
    IKPhysicalNodeManager* manager;
    csRef<CS::Animation::iBodySkeleton> bodySkeleton;
    csHash<EffectorData, CS::Animation::EffectorID> effectors;
    CS::Animation::EffectorID maxEffectorID;
    bool resetChain;

    friend class IKPhysicalNode;
  };

  class IKPhysicalNode
    : public scfImplementation3<IKPhysicalNode,
    scfFakeInterface<CS::Animation::iSkeletonAnimNode>,
    scfFakeInterface<CS::Animation::iSkeletonIKNode>,
    CS::Animation::iSkeletonIKPhysicalNode>,
    CS::Animation::csSkeletonAnimNodeSingle
  {
  public:
    CS_LEAKGUARD_DECLARE(IKPhysicalNode);

    IKPhysicalNode (IKPhysicalNodeFactory* factory,
		    CS::Animation::iSkeleton* skeleton);
    ~IKPhysicalNode ();

    //-- CS::Animation::iSkeletonIKPhysicalNode
    virtual void SetRagdollNode (CS::Animation::iSkeletonRagdollNode* ragdollNode);
    virtual CS::Animation::iSkeletonRagdollNode* GetRagdollNode () const;

    //-- CS::Animation::iSkeletonIKNode
    virtual void AddConstraint (CS::Animation::EffectorID effector,
				csOrthoTransform& transform);
    virtual void AddConstraint (CS::Animation::EffectorID effector,
				iMovable* target, const csOrthoTransform& offset);
    virtual void AddConstraint (CS::Animation::EffectorID effector,
				iCamera* target, const csOrthoTransform& offset);

    virtual void RemoveConstraint (CS::Animation::EffectorID effector);

    //-- CS::Animation::iSkeletonAnimNode
    virtual void Play ();
    virtual void Stop ();

    inline virtual void SetPlaybackPosition (float time)
    { csSkeletonAnimNodeSingle::SetPlaybackPosition (time); }
    inline virtual float GetPlaybackPosition () const
    { return csSkeletonAnimNodeSingle::GetPlaybackPosition (); }

    inline virtual float GetDuration () const
    { return csSkeletonAnimNodeSingle::GetDuration (); }
    inline virtual void SetPlaybackSpeed (float speed)
    { csSkeletonAnimNodeSingle::SetPlaybackSpeed (speed); }
    inline virtual float GetPlaybackSpeed () const
    { return csSkeletonAnimNodeSingle::GetPlaybackSpeed (); }

    virtual void BlendState (CS::Animation::csSkeletalState* state,
			     float baseWeight = 1.0f);
    virtual void TickAnimation (float dt);

    inline virtual bool IsActive () const
    { return csSkeletonAnimNodeSingle::IsActive (); }

    virtual CS::Animation::iSkeletonAnimNodeFactory* GetFactory () const;
    virtual CS::Animation::iSkeletonAnimNode* FindNode (const char* name);

    inline virtual void AddAnimationCallback (CS::Animation::iSkeletonAnimCallback* callback)
    { csSkeletonAnimNodeSingle::AddAnimationCallback (callback); }
    inline virtual void RemoveAnimationCallback (CS::Animation::iSkeletonAnimCallback* callback)
    { csSkeletonAnimNodeSingle::RemoveAnimationCallback (callback); }

  private:
    enum ConstraintType
    {
      CONSTRAINT_FIXED = 0,
      CONSTRAINT_MOVABLE,
      CONSTRAINT_CAMERA
    };

    struct ConstraintData
    {
      ConstraintData ()
      : movable (0), camera (0) {}

      ConstraintType type;
      iMovable* movable;
      iCamera* camera;
      csOrthoTransform offset;
      csRef<CS::Physics::Bullet::iPivotJoint> dragJoint;
    };
    void AddConstraint (CS::Animation::EffectorID effectorID, ConstraintData& constraint);

  private:
    IKPhysicalNodeFactory* factory;
    csWeakRef<iSceneNode> sceneNode;
    csWeakRef<CS::Animation::iSkeletonRagdollNode> ragdollNode;

    csHash<ConstraintData, CS::Animation::EffectorID> constraints;

    struct ChainData
    {
      CS::Animation::RagdollState previousState;
      size_t constraintCount;
    };
    csHash<ChainData, CS::Animation::iBodyChain*> chains;

    csRef<CS::Physics::Bullet::iDynamicSystem> bulletDynamicSystem;

    friend class IKPhysicalNodeFactory;
  };

}
CS_PLUGIN_NAMESPACE_END(IKPhysical)

#endif //__CS_IKPHYSICAL_H__
