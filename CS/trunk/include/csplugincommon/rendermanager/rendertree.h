/*
    Copyright (C) 2007-2008 by Marten Svanfeldt

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

#ifndef __CS_CSPLUGINCOMMON_RENDERMANAGER_RENDERTREE_H__
#define __CS_CSPLUGINCOMMON_RENDERMANAGER_RENDERTREE_H__

#include "iengine/camera.h"
#include "csplugincommon/rendermanager/standardtreetraits.h"
#include "csutil/dirtyaccessarray.h"
#include "csutil/metautils.h"
#include "csutil/redblacktree.h"
#include "cstool/rendermeshholder.h"

struct iMeshWrapper;
struct iPortalContainer;

namespace CS
{
namespace RenderManager
{
  class PostEffectManager;
  
  /* Helper class containing stuff which doesn't require any of the template
   * parameters to RenderTree
   */
  class CS_CRYSTALSPACE_EXPORT RenderTreeBase
  {
  protected:
    uint nextDebugId;
    csHash<uint, csString> debugIdMappings;
    csBitArray debugFlags;
  
    struct DebugTexture
    {
      csRef<iTextureHandle> texh;
      float aspect;
    };
    csArray<DebugTexture> debugTextures;
    
    RenderTreeBase ();
  public:
    //@{
    /**\name Debugging helpers: toggling of debugging features
     */
    /// Register a debug flag, returns a numeric ID
    uint RegisterDebugFlag (const char* string);
    /**
     * Query whether a debug flag was registered and return its ID or
     * (uint)-1 if not registered.
     */
    uint QueryDebugFlag (const char* string);
    
    /// Check whether a debug flag is enabled
    bool IsDebugFlagEnabled (uint flag);
    /// Enable or disable a debug flag
    void EnableDebugFlag (uint flag, bool state);
    //@}
    
    //@{
    /**\name Debugging helpers: debugging textures
     */
    /**
     * Add a texture to be rendered at the bottom of the view the next frame.
     */
    void AddDebugTexture (iTextureHandle* tex, float aspect = 1.0f);
    /**
     * Render out debug textures. To be called by the rendermanager at the end
     * of rendering a view.
     */
    void RenderDebugTextures (iGraphics3D* g3d);
    //@}
  
    //@{
    /**\name Debugging helpers: line drawing
     */
    /// Add debug line (world space)
    void AddDebugLine3D (const csVector3& v1, const csVector3& v2,
                         const csColor& color1, const csColor& color2);
    /**
     * Add debug line (arbitrary space, transformed to world space with 
     * \a toWorldSpace)
     */
    void AddDebugLine3DTF (const csVector3& v1, const csVector3& v2,
                           const csTransform& toWorldSpace,
                           const csColor& color1, const csColor& color2);
    /**
     * Add lines to visualize a bounding box (in arbitrary space, transformed 
     * to world space with \a toWorldSpace)
     */
    void AddDebugBBox (const csBox3& box,
                       const csTransform& toWorldSpace,
                       const csColor& col);
    /**
     * Add lines to visualize a plane (in arbitrary space, transformed 
     * to world space with \a toWorldSpace)
     */
    void AddDebugPlane (const csPlane3& _plane,
                        const csTransform& toWorldSpace,
                        const csColor& col,
                        const csVector3& linesOrg = csVector3 (0));
    /**
     * Visualize camera clip planes for the given view.
     */
    void AddDebugClipPlanes (RenderView* view);
    /**
     * Render out debug lines. To be called by the rendermanager at the end
     * of rendering a view.
     */
    void DrawDebugLines (iGraphics3D* g3d, RenderView* view);

    struct DebugLines
    {
      csDirtyAccessArray<csVector3> verts;
      csDirtyAccessArray<csVector4> colors;
    };
    /// Get all current debug lines. Useful to conserve the current lines
    const DebugLines& GetDebugLines () const { return debugLines; }
    /**
     * Set all current debug lines. Useful to e.g. set conserved lines from an
     * earlier frame.
     */
    void SetDebugLines (const DebugLines& lines) { debugLines = lines; }
    //@}
    
  protected:
    DebugLines debugLines;
  };

  /**
   * RenderTree is the main data-structure for the rendermanagers.
   * It contains the entire setup of meshes and where to render those meshes,
   * as well as basic operations regarding those meshes.
   */
  template<typename TreeTraits = RenderTreeStandardTraits>
  class RenderTree : public RenderTreeBase
  {
  public:
    //---- Forward declarations
    struct MeshNode;
    struct ContextNode;
    struct ContextsContainer;

    //---- Type definitions
    typedef TreeTraits TreeTraitsType;
    typedef RenderTree<TreeTraitsType> ThisType;
    typedef RenderTree<TreeTraitsType> RealTreeType;

    //---- Internal types
    typedef csRedBlackTreeMap<typename TreeTraitsType::MeshNodeKeyType, MeshNode*> MeshNodeTreeType;
    typedef typename MeshNodeTreeType::Iterator MeshNodeTreeIteratorType;
    typedef csArray<ContextNode*> ContextNodeArrayType;
    typedef typename ContextNodeArrayType::Iterator ContextNodeArrayIteratorType;
    typedef typename ContextNodeArrayType::ReverseIterator ContextNodeArrayReverseIteratorType;


    /**
     * Persistent data for the tree, needs to be stored between frames by the RM
     */
    struct PersistentData
    {
      void Initialize (iShaderManager* shmgr)
      {
        svObjectToWorldName = 
          shmgr->GetSVNameStringset()->Request ("object2world transform");
        svObjectToWorldInvName = 
          shmgr->GetSVNameStringset()->Request ("object2world transform inverse");
      }

      void Clear ()
      {
        // Clean up the persistent data
        contextNodeAllocator.Empty ();
        meshNodeAllocator.Empty ();
      }

      csBlockAllocator<MeshNode> meshNodeAllocator;
      csBlockAllocator<ContextNode> contextNodeAllocator;
      

      CS::ShaderVarStringID svObjectToWorldName;
      CS::ShaderVarStringID svObjectToWorldInvName;
    
      RenderView::Pool renderViewPool;
      csRenderMeshHolder rmHolder;
    };

    /**
     * A mesh node is a single list of meshes that can be rendered in one go
     * without any explicit order considerations.
     */
    struct MeshNode : 
      public CS::Meta::EBOptHelper<typename TreeTraitsType::MeshNodeExtraDataType>
    {
      /**
       * A single mesh within the tree to be rendered.
       */
      struct SingleMesh : 
        public CS::Meta::EBOptHelper<typename TreeTraitsType::MeshExtraDataType>
      {
        iMeshWrapper* meshWrapper;
        csRenderMesh* renderMesh;
        csZBufMode zmode;
        iShaderVariableContext* meshObjSVs;
        csRef<csShaderVariable> svObjectToWorld;
        csRef<csShaderVariable> svObjectToWorldInv;
        csBox3 bbox;
        csFlags meshFlags;

        size_t contextLocalId;
      };

      //-- Types
      typedef RealTreeType TreeType;
      typedef typename TreeType::ContextNode ContextNodeType;

      //-- Some local types
      typedef csArray<SingleMesh> MeshArrayType;
      typedef typename MeshArrayType::Iterator MeshArrayIteratorType;

      /// Owner
      ContextNode& owner;
      
      /// Our own key
      typename TreeTraitsType::MeshNodeKeyType key;

      /// All the meshes within the meshnode
      MeshArrayType meshes;

      MeshNode (ContextNode& owner)
        : owner (owner)
      {}
    };

    /**
     * 
     */
    struct ContextNode : public CS::Meta::EBOptHelper<typename TreeTraits::ContextNodeExtraDataType>
    {
      /**
       * 
       */
      struct PortalHolder
      {
      #ifdef CS_DEBUG
        const char* db_mesh_name;
      #endif
        iPortalContainer* portalContainer;
        iMeshWrapper* meshWrapper;
      };

      //-- Types
      typedef RealTreeType TreeType;

      // Owner
      TreeType& owner;

      // Target data
      csRef<RenderView> renderView;
      struct TargetTexture
      {
        iTextureHandle* texHandle;
        int subtexture;

        TargetTexture() : texHandle (0), subtexture (0) {}
      };
      TargetTexture renderTargets[rtaNumAttachments];
      csReversibleTransform cameraTransform;
      int drawFlags;
      iSector* sector;
      csRef<iShaderVariableContext> shadervars;
      
      csRef<PostEffectManager> postEffects;

      // A sub-tree of mesh nodes
      MeshNodeTreeType meshNodes;

      // All portals within context
      csArray<PortalHolder> allPortals;

      // The SVs themselves
      SVArrayHolder svArrays;

      // Arrays of per-mesh shader and ticket info
      csDirtyAccessArray<iShader*> shaderArray;
      csArray<size_t> ticketArray;

      // Total number of render meshes within the context, just for statistics
      size_t totalRenderMeshes;
      
      ContextNode(TreeType& owner) 
        : owner (owner), drawFlags (0), totalRenderMeshes (0) 
      {}
      
      /**
       * Add a rendermesh to context, putting it in the right meshnode etc.
       */
      void AddRenderMesh (csRenderMesh* rm, 
			  CS::Graphics::RenderPriority renderPrio,
			  typename MeshNode::SingleMesh& singleMeshTemplate)
      {
	typename TreeTraits::MeshNodeKeyType meshKey = 
	  TreeTraits::GetMeshNodeKey (renderPrio, *rm);
	
	// Get the mesh node
	MeshNode* meshNode = meshNodes.Get (meshKey, 0);
	if (!meshNode)
	{
	  // Get a new one
	  meshNode = owner.CreateMeshNode (*this, meshKey);
    
	  RenderTree::TreeTraitsType::SetupMeshNode(*meshNode, renderPrio, *rm);
	  meshNodes.Put (meshKey, meshNode);
	}
    
	csRef<csShaderVariable> svObjectToWorld;
	svObjectToWorld.AttachNew (new csShaderVariable (
	  owner.GetPersistentData ().svObjectToWorldName));
	svObjectToWorld->SetValue (rm->object2world);
	csRef<csShaderVariable> svObjectToWorldInv;
	svObjectToWorldInv.AttachNew (new csShaderVariable (
	  owner.GetPersistentData ().svObjectToWorldInvName));
	svObjectToWorldInv->SetValue (rm->object2world.GetInverse());
    
	typename MeshNode::SingleMesh sm (singleMeshTemplate);
	sm.renderMesh = rm;
	sm.svObjectToWorld = svObjectToWorld;
	sm.svObjectToWorldInv = svObjectToWorldInv;
	if (rm->z_buf_mode != (csZBufMode)~0) 
	  sm.zmode = rm->z_buf_mode;
    
	meshNode->meshes.Push (sm);
	totalRenderMeshes++;
      }

      void InsertLayer (size_t after)
      {
        const size_t layerCount = shaderArray.GetSize() / totalRenderMeshes;
        shaderArray.SetSize (shaderArray.GetSize() + totalRenderMeshes);
        const size_t layerOffsetNew = (after + 1) * totalRenderMeshes;
        memmove (shaderArray.GetArray() + layerOffsetNew + totalRenderMeshes,
          shaderArray.GetArray() + layerOffsetNew,
          totalRenderMeshes * (layerCount - after - 1) * sizeof(csShaderVariable*));
        memset (shaderArray.GetArray() + layerOffsetNew, 0,
          totalRenderMeshes * sizeof(csShaderVariable*));

        svArrays.InsertLayer (after, after);
      }

      void CopyLayerShader (size_t meshId, size_t fromLayer, size_t toLayer)
      {
        const size_t fromLayerOffset = fromLayer * totalRenderMeshes;
        const size_t toLayerOffset = toLayer * totalRenderMeshes;
        shaderArray[toLayerOffset + meshId] =
          shaderArray[fromLayerOffset + meshId];
      }
      
      bool GetTargetDimensions (int& renderW, int& renderH)
      {
	for (int a = 0; a < rtaNumAttachments; a++)
	{
	  if (renderTargets[a].texHandle != 0)
	  {
	    renderTargets[a].texHandle->GetRendererDimensions (
	      renderW, renderH);
	    return true;
	  }
        }
        return false;
      }
    };


    //---- Methods
    RenderTree (PersistentData& dataStorage)
      : persistentData (dataStorage)
    {
    }

    ~RenderTree ()
    {
      persistentData.Clear ();
    }

    /**
     * Create a new context
     * \param rw Render view to associate the new context with. Should be 
     * initalized before calling method.
     * \param insertAfter Context to insert this one after. Must be a valid
     * context in current render tree.
     */
    ContextNode* CreateContext (RenderView* rw, ContextNode* insertAfter = 0)
    {
      // Create an initial context
      ContextNode* newCtx = persistentData.contextNodeAllocator.Alloc (*this);
      newCtx->renderView = rw;
      newCtx->cameraTransform = rw->GetCamera ()->GetTransform ();
      newCtx->sector = rw->GetThisSector();

      if (insertAfter)
      {
        size_t n = contexts.Find (insertAfter);
        CS_ASSERT(n != csArrayItemNotFound);
        contexts.Insert (n+1, newCtx);
      }
      else
      {
        contexts.Push (newCtx);
      }

      return newCtx;
    }
    
    /**
     * Destroy a context and return it to the allocation pool.
     */
    void DestroyContext (ContextNode* context)
    {
      CS_ASSERT(contexts.Find (context) != csArrayItemNotFound);
      contexts.Delete (context);

      persistentData.contextNodeAllocator.Free (context);
    }

    /**
     * Get an iterator for iterating forward over the contexts.
     */
    ContextNodeArrayIteratorType GetContextIterator ()
    {
      return contexts.GetIterator ();
    }

    /**
     * Get an iterator for iterating backward over the contexts.
     */
    ContextNodeArrayReverseIteratorType GetReverseContextIterator ()
    {
      return contexts.GetReverseIterator ();
    }

    /**
     * Create a new mesh node associated with the given context.
     */
    MeshNode* CreateMeshNode (ContextNode& context, 
      const typename TreeTraitsType::MeshNodeKeyType& key)
    {
      MeshNode* newNode = persistentData.meshNodeAllocator.Alloc (context);
      newNode->key = key;
    
      return newNode;
    }


    /**
     * Destroy given mesh node.
     */
    void DestroyMeshNode (MeshNode* meshNode)
    {
      meshNode->owner.meshNodes.Delete (meshNode->key);
      persistentData.meshNodeAllocator.Free (meshNode);
    }

    PersistentData& GetPersistentData()
    {
      return persistentData;
    }
    const PersistentData& GetPersistentData() const
    {
      return persistentData;
    }

  protected:    
    PersistentData&         persistentData;
    ContextNodeArrayType    contexts; 
  };

}
}



#endif
