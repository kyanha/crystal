/*
  Copyright (C) 2008 by Marten Svanfeldt

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

#ifndef __CS_SKELETON2LDR_H__
#define __CS_SKELETON2LDR_H__

#include "csutil/scf_implementation.h"
#include "imap/reader.h"
#include "imap/writer.h"
#include "iutil/comp.h"
#include "csutil/csstring.h"
#include "imesh/skeleton2.h"

namespace CS {
namespace Animation {
struct iBodyManager;
}
}

CS_PLUGIN_NAMESPACE_BEGIN(Skeleton2Ldr)
{

  class SkeletonLoader :
    public scfImplementation2<SkeletonLoader,
                              iLoaderPlugin,
                              iComponent>
  {
  public:
    SkeletonLoader (iBase* parent);

    //-- iLoaderPlugin
    virtual csPtr<iBase> Parse (iDocumentNode* node,
      iStreamSource* ssource, iLoaderContext* ldr_context,
      iBase* context);

    virtual bool IsThreadSafe() { return true; }

    //-- iComponent
    virtual bool Initialize (iObjectRegistry*);

  private: 
    bool ParseSkeleton (iDocumentNode* node);
    bool ParseBone (iDocumentNode* node, CS::Animation::iSkeletonFactory* factory, CS::Animation::BoneID parent);

    bool ParseAnimPacket (iDocumentNode* node);    
    CS::Animation::iSkeletonAnimation* ParseAnimation (iDocumentNode* node, 
      CS::Animation::iSkeletonAnimPacketFactory* packet);

    csPtr<CS::Animation::iSkeletonAnimNodeFactory> ParseAnimTreeNode (iDocumentNode* node,
      CS::Animation::iSkeletonAnimPacketFactory* packet);

    csPtr<CS::Animation::iSkeletonAnimNodeFactory> ParseAnimationNode (iDocumentNode* node,
      CS::Animation::iSkeletonAnimPacketFactory* packet);
    csPtr<CS::Animation::iSkeletonAnimNodeFactory> ParseBlendNode (iDocumentNode* node,
      CS::Animation::iSkeletonAnimPacketFactory* packet);
    csPtr<CS::Animation::iSkeletonAnimNodeFactory> ParsePriorityNode (iDocumentNode* node,
      CS::Animation::iSkeletonAnimPacketFactory* packet);
    csPtr<CS::Animation::iSkeletonAnimNodeFactory> ParseRandomNode (iDocumentNode* node,
      CS::Animation::iSkeletonAnimPacketFactory* packet);
    csPtr<CS::Animation::iSkeletonAnimNodeFactory> ParseFSMNode (iDocumentNode* node,
      CS::Animation::iSkeletonAnimPacketFactory* packet);
    csPtr<CS::Animation::iSkeletonAnimNodeFactory> ParseDebugNode (iDocumentNode* node,
      CS::Animation::iSkeletonAnimPacketFactory* packet);
    csPtr<CS::Animation::iSkeletonAnimNodeFactory> ParseIKNode (iDocumentNode* node,
      CS::Animation::iSkeletonAnimPacketFactory* packet);
    bool ParseIKCCDNode (iDocumentNode* node, CS::Animation::iSkeletonIKNodeFactory* factnode);
    bool ParseIKPhysicalNode (iDocumentNode* node, CS::Animation::iSkeletonIKNodeFactory* factnode);
    csPtr<CS::Animation::iSkeletonAnimNodeFactory> ParseLookAtNode (iDocumentNode* node,
      CS::Animation::iSkeletonAnimPacketFactory* packet);
    csPtr<CS::Animation::iSkeletonAnimNodeFactory> ParseRagdollNode (iDocumentNode* node,
      CS::Animation::iSkeletonAnimPacketFactory* packet);
    csPtr<CS::Animation::iSkeletonAnimNodeFactory> ParseRetargetNode (iDocumentNode* node,
      CS::Animation::iSkeletonAnimPacketFactory* packet);
    csPtr<CS::Animation::iSkeletonAnimNodeFactory> ParseSpeedNode (iDocumentNode* node,
      CS::Animation::iSkeletonAnimPacketFactory* packet);

    bool ParseBoneMapping (iDocumentNode* node, CS::Animation::BoneMapping& mapping,
			   CS::Animation::iSkeletonFactory* sourceSkeleton,
			   CS::Animation::iSkeletonFactory* targetSkeleton);
    bool ParseEffector (iDocumentNode* node, CS::Animation::iSkeletonIKNodeFactory* factory);

    iObjectRegistry* object_reg;
    csRef<iSyntaxService> synldr;
    csRef<CS::Animation::iSkeletonManager> skelManager;
    csRef<CS::Animation::iBodyManager> bodyManager;

    csStringHash xmltokens;
#define CS_TOKEN_ITEM_FILE \
  "plugins/mesh/animesh/persist/skeleton2/skeleton2ldr.tok"
#include "cstool/tokenlist.h"
#undef CS_TOKEN_ITEM_FILE
  };


  class SkeletonSaver :
    public scfImplementation2<SkeletonSaver,
                              iSaverPlugin,
                              iComponent>
  {
  private:
    iObjectRegistry* object_reg;
    csRef<iReporter> reporter;
    csRef<iSyntaxService> synldr;

    void WriteAnimPacket (CS::Animation::iSkeletonAnimPacketFactory* packet,
			iDocumentNode* parent);
    void WriteAnimation (CS::Animation::iSkeletonAnimation* animation,
			 iDocumentNode* parent);
    void WriteAnimNode (CS::Animation::iSkeletonAnimNodeFactory* animNode,
			iDocumentNode* parent);
    void WriteSkeleton (CS::Animation::iSkeletonFactory* factory,
			iDocumentNode* parent);
    void WriteBone (CS::Animation::iSkeletonFactory* factory,
		    CS::Animation::BoneID boneID,
		    iDocumentNode* parent);

  public:
    SkeletonSaver (iBase* parent);

    //-- iSaverPlugin
    virtual bool WriteDown (iBase *obj, iDocumentNode* parent,
      iStreamSource*);

    //-- iComponent
    virtual bool Initialize (iObjectRegistry*);
  };

}
CS_PLUGIN_NAMESPACE_END(Skeleton2Ldr)


#endif
