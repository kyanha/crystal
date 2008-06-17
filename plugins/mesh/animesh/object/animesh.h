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

#ifndef __CS_ANIMESH_H__
#define __CS_ANIMESH_H__

#include "csgfx/shadervarcontext.h"
#include "cstool/rendermeshholder.h"
#include "csutil/dirtyaccessarray.h"
#include "csutil/flags.h"
#include "csutil/refarr.h"
#include "csutil/scf_implementation.h"
#include "imesh/animesh.h"
#include "imesh/object.h"
#include "iutil/comp.h"
#include "csgeom/box.h"
#include "cstool/objmodel.h"

#include "morphtarget.h"

CS_PLUGIN_NAMESPACE_BEGIN(Animesh)
{

  class FactorySubmesh;

  class AnimeshObjectType : 
    public scfImplementation2<AnimeshObjectType, 
                              iMeshObjectType, 
                              iComponent>
  {
  public:
    AnimeshObjectType (iBase* parent);

    //-- iMeshObjectType
    virtual csPtr<iMeshObjectFactory> NewFactory ();

    //-- iComponent
    virtual bool Initialize (iObjectRegistry*);
  };


  class AnimeshObjectFactory :
    public scfImplementation2<AnimeshObjectFactory,
                              iAnimatedMeshFactory,
                              iMeshObjectFactory>
  {
  public:
    AnimeshObjectFactory (AnimeshObjectType* objType);

    //-- iAnimatedMeshFactory
    virtual iAnimatedMeshFactorySubMesh* CreateSubMesh (iRenderBuffer* indices);
    virtual iAnimatedMeshFactorySubMesh* CreateSubMesh (
      const csArray<iRenderBuffer*>& indices, 
      const csArray<csArray<unsigned int> >& boneIndices);
    virtual iAnimatedMeshFactorySubMesh* GetSubMesh (size_t index) const;
    virtual size_t GetSubMeshCount () const;
    virtual void DeleteSubMesh (iAnimatedMeshFactorySubMesh* mesh);

    virtual uint GetVertexCount () const;

    virtual iRenderBuffer* GetVertices ();
    virtual bool SetVertices (iRenderBuffer* renderBuffer);
    virtual iRenderBuffer* GetTexCoords ();
    virtual bool SetTexCoords (iRenderBuffer* renderBuffer);
    virtual iRenderBuffer* GetNormals ();
    virtual bool SetNormals (iRenderBuffer* renderBuffer);
    virtual iRenderBuffer* GetTangents ();
    virtual bool SetTangents (iRenderBuffer* renderBuffer);
    virtual iRenderBuffer* GetBinormals ();
    virtual bool SetBinormals (iRenderBuffer* renderBuffer);
    virtual iRenderBuffer* GetColors ();
    virtual bool SetColors (iRenderBuffer* renderBuffer);

    virtual void Invalidate ();

    virtual void SetSkeletonFactory (iSkeletonFactory2* skeletonFactory);
    virtual iSkeletonFactory2* GetSkeletonFactory () const;
    virtual void SetBoneInfluencesPerVertex (uint num);
    virtual uint GetBoneInfluencesPerVertex () const;
    virtual csAnimatedMeshBoneInfluence* GetBoneInfluences ();

    virtual iAnimatedMeshMorphTarget* CreateMorphTarget (const char* name);
    virtual iAnimatedMeshMorphTarget* GetMorphTarget (uint target);
    virtual uint GetMorphTargetCount () const;
    virtual void ClearMorphTargets ();
    virtual uint FindMorphTarget (const char* name) const;

    //-- iMeshObjectFactory
    virtual csFlags& GetFlags ();

    virtual csPtr<iMeshObject> NewInstance ();

    virtual csPtr<iMeshObjectFactory> Clone ();

    virtual void HardTransform (const csReversibleTransform& t);
    virtual bool SupportsHardTransform () const;

    virtual void SetMeshFactoryWrapper (iMeshFactoryWrapper* logparent);
    virtual iMeshFactoryWrapper* GetMeshFactoryWrapper () const;

    virtual iMeshObjectType* GetMeshObjectType () const;

    virtual iObjectModel* GetObjectModel ();

    virtual bool SetMaterialWrapper (iMaterialWrapper* material);
    virtual iMaterialWrapper* GetMaterialWrapper () const;

    virtual void SetMixMode (uint mode);
    virtual uint GetMixMode () const;

    //-- Private
    inline uint GetVertexCountP () const
    {
      return vertexCount;
    }
  
  private: 

    // required but stupid stuff..
    AnimeshObjectType* objectType;
    iMeshFactoryWrapper* logParent;
    iMaterialWrapper* material;
    csFlags factoryFlags;
    uint mixMode;
    csBox3 factoryBB;

    // Main data storage...
    uint vertexCount;
    csRef<iRenderBuffer> vertexBuffer;
    csRef<iRenderBuffer> texcoordBuffer;
    csRef<iRenderBuffer> normalBuffer;
    csRef<iRenderBuffer> tangentBuffer;
    csRef<iRenderBuffer> binormalBuffer;
    csRef<iRenderBuffer> colorBuffer;
    csDirtyAccessArray<csAnimatedMeshBoneInfluence> boneInfluences;
    csRef<iRenderBuffer> masterBWBuffer;
    csRef<iRenderBuffer> boneWeightAndIndexBuffer[2];

    csRef<iSkeletonFactory2> skeletonFactory;

    // Submeshes
    csRefArray<FactorySubmesh> submeshes;

    csRefArray<MorphTarget> morphTargets;
    csHash<uint, csString> morphTargetNames;

    friend class AnimeshObject;
  };

  class FactorySubmesh : 
    public scfImplementation1<FactorySubmesh, 
                              iAnimatedMeshFactorySubMesh>
  {
  public:
    FactorySubmesh ()
      : scfImplementationType (this)
    {}

    virtual iRenderBuffer* GetIndices (size_t set)
    {
      return indexBuffers[set];
    }

    virtual uint GetIndexSetCount () const
    {
      return (uint)indexBuffers.GetSize ();
    }

    virtual const csArray<unsigned int>& GetBoneIndices (size_t set)
    {
      static const csArray<unsigned int> noBI;
      if (boneMapping.GetSize () > 0)
      {
        return boneMapping[set].boneRemappingTable;
      }
      else
      {
        return noBI;
      }
    }

    csRefArray<iRenderBuffer> indexBuffers;
    csRefArray<csRenderBufferHolder> bufferHolders;

    // For every subpart(index buffer), hold:
    //  the "real" bone index to use for "virtual" bone index i
    // When using remappings, have these two RBs...
    struct RemappedBones
    {
      csArray<unsigned int> boneRemappingTable;
      csRef<iRenderBuffer> masterBWBuffer;
      csRef<iRenderBuffer> boneWeightAndIndexBuffer[2];
    };
    csArray<RemappedBones> boneMapping;
  };



  class AnimeshObject :
    public scfImplementationExt2<AnimeshObject,
                                 csObjectModel,
                                 iAnimatedMesh,
                                 iMeshObject>
  {
  public:
    AnimeshObject (AnimeshObjectFactory* factory);

    //-- iAnimatedMesh
    virtual void SetSkeleton (iSkeleton2* skeleton);
    virtual iSkeleton2* GetSkeleton () const;

    virtual iAnimatedMeshSubMesh* GetSubMesh (size_t index) const;
    virtual size_t GetSubMeshCount () const;

    virtual void SetMorphTargetWeight (uint target, float weight);
    virtual float GetMorphTargetWeight (uint target) const;

    //-- iMeshObject
    virtual iMeshObjectFactory* GetFactory () const;

    virtual csFlags& GetFlags ();

    virtual csPtr<iMeshObject> Clone ();

    virtual CS::Graphics::RenderMesh** GetRenderMeshes (int& num, iRenderView* rview, 
      iMovable* movable, uint32 frustum_mask);

    virtual void SetVisibleCallback (iMeshObjectDrawCallback* cb);

    virtual iMeshObjectDrawCallback* GetVisibleCallback () const;

    virtual void NextFrame (csTicks current_time,const csVector3& pos,
      uint currentFrame);

    virtual void HardTransform (const csReversibleTransform& t);
    virtual bool SupportsHardTransform () const;

    virtual bool HitBeamOutline (const csVector3& start,
      const csVector3& end, csVector3& isect, float* pr);
    virtual bool HitBeamObject (const csVector3& start, const csVector3& end,
      csVector3& isect, float* pr, int* polygon_idx,
      iMaterialWrapper** material);

    virtual void SetMeshWrapper (iMeshWrapper* logparent);
    virtual iMeshWrapper* GetMeshWrapper () const;

    virtual iObjectModel* GetObjectModel ();

    virtual bool SetColor (const csColor& color);
    virtual bool GetColor (csColor& color) const;

    virtual bool SetMaterialWrapper (iMaterialWrapper* material);
    virtual iMaterialWrapper* GetMaterialWrapper () const;

    virtual void SetMixMode (uint mode);
    virtual uint GetMixMode () const;

    virtual void InvalidateMaterialHandles ();

    virtual void PositionChild (iMeshObject* child,csTicks current_time);

    virtual void BuildDecal(const csVector3* pos, float decalRadius,
      iDecalBuilder* decalBuilder);

    //-- iObjectModel
    virtual const csBox3& GetObjectBoundingBox ();
    virtual void SetObjectBoundingBox (const csBox3& bbox);
    virtual void GetRadius (float& radius, csVector3& center);

    //-- iRenderBufferAccessor
    void PreGetBuffer (csRenderBufferHolder* holder, 
      csRenderBufferName buffer);

    //
    void SetupSubmeshes ();
    void UpdateLocalBoneTransforms ();

  private:
    void SkinVertices ();
    void SkinNormals ();
    void SkinVerticesAndNormals ();
    void SkinTangentAndBinormal ();
    void SkinAll ();

    void PreskinLF ();

    class RenderBufferAccessor :
      public scfImplementation1<RenderBufferAccessor, 
                                iRenderBufferAccessor>
    {
    public:
      RenderBufferAccessor (AnimeshObject* meshObject)
        : scfImplementationType (this), meshObject (meshObject)
      {}

      void PreGetBuffer (csRenderBufferHolder* holder, 
	csRenderBufferName buffer)
      { meshObject->PreGetBuffer (holder, buffer); }
      
      AnimeshObject* meshObject;
    };

    class Submesh : 
      public scfImplementation1<Submesh, 
                                iAnimatedMeshSubMesh>
    {
    public:
      Submesh (AnimeshObject* meshObject, FactorySubmesh* factorySubmesh)
        : scfImplementationType (this), meshObject (meshObject),
        factorySubmesh (factorySubmesh), isRendering (true)
      {}

      virtual iAnimatedMeshFactorySubMesh* GetFactorySubMesh ()
      {
        return factorySubmesh;
      }

      virtual void SetRendering (bool doRender)
      {
        isRendering = doRender;
      }

      virtual bool IsRendering () const
      {
        return isRendering;
      }

      AnimeshObject* meshObject;
      FactorySubmesh* factorySubmesh;
      bool isRendering;

      csRefArray<csShaderVariableContext> svContexts;
      csRefArray<csRenderBufferHolder> bufferHolders;
      csRefArray<csShaderVariable> boneTransformArray;
    };    

    AnimeshObjectFactory* factory;
    iMeshWrapper* logParent;
    iMaterialWrapper* material;
    uint mixMode;
    csFlags meshObjectFlags;

    csRef<iSkeleton2> skeleton;
    unsigned int skeletonVersion;
    csTicks lastTick;

    // Hold the bone transforms
    csRef<csShaderVariable> boneTransformArray;
    csRef<csSkeletalState2> lastSkeletonState;

    csRenderMeshHolder rmHolder;
    csDirtyAccessArray<CS::Graphics::RenderMesh*> renderMeshList;

    csRefArray<Submesh> submeshes;

    // Holder for skinned vertex buffers
    csRef<iRenderBuffer> skinnedVertices;
    csRef<iRenderBuffer> skinnedNormals;
    csRef<iRenderBuffer> skinnedTangents;
    csRef<iRenderBuffer> skinnedBinormals;

    csArray<float> morphTargetWeights;

    // Version numbers for the software skinning
    unsigned int skinVertexVersion, skinNormalVersion, skinTangentVersion, skinBinormalVersion;
    // Things we skinned in software last frame
    bool skinVertexLF, skinNormalLF, skinTangentLF, skinBinormalLF;
  };

}
CS_PLUGIN_NAMESPACE_END(Animesh)


#endif
