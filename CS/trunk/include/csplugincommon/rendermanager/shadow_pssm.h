/*
    Copyright (C) 2008 by Frank Richter

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

#ifndef __CS_CSPLUGINCOMMON_RENDERMANAGER_SHADOW_PSSM_H__
#define __CS_CSPLUGINCOMMON_RENDERMANAGER_SHADOW_PSSM_H__

/**\file
 * PSSM shadow handler
 */

#include "ivideo/shader/shader.h"

#include "cstool/meshfilter.h"

#include "csplugincommon/rendermanager/operations.h"
#include "csplugincommon/rendermanager/rendertree.h"
#include "csplugincommon/rendermanager/shadow_common.h"
#include "csplugincommon/rendermanager/standardsorter.h"
#include "csplugincommon/rendermanager/viscull.h"

#include "csgeom/matrix4.h"

class csShaderVariable;

namespace CS
{
namespace RenderManager
{

  /**
   * PSSM shadow handler.
   * Usage: in the \c ShadowHandler argument of the LightSetup class.
   * In addition, on initialization the SetConfigPrefix() method of the shadow
   * handler persistent data (accessible through the light setup persistent
   * data) must be called, before the light setup persistent data is
   * initialized:
   * \code
   * // First, set prefix for configuration settings
   * lightPersistent.shadowPersist.SetConfigPrefix ("RenderManager.ShadowPSSM");
   * // Then, light setup (and shadow handler) persistent data is initialized
   * lightPersistent.Initialize (objectReg, treePersistent.debugPersist);
   * \endcode
   */
  template<typename RenderTree, typename LayerConfigType>
  class ShadowPSSM
  {
  public:
    struct PersistentData;
    
    /**
     * Shadow per-view specific data.
     * An instance of this class needs to be created when rendering a view and
     * passed the light setup.
     * Example:
     * \code
     * // ... basic setup ...
     *
     * // Setup shadow handler per-view data
     * ShadowType::ViewSetup shadowViewSetup (
     *   rednerManager->lightPersistent.shadowPersist, renderView);
     *
     * // ... perform various tasks ...
     *
     * // Setup lighting for meshes
     * LightSetupType lightSetup (
     *  renderManager->lightPersistent, renderManager->lightManager,
     *  context.svArrays, layerConfig, shadowViewSetup);
     * \endcode
     * \sa LightSetup
     */
    class ViewSetup
    {
    public:
      PersistentData& persist;
      
      CS::RenderManager::RenderView* rview;
      int numParts;
      float* splitDists;
      float lx, rx, ty, by;
      bool doFixedCloseShadow;
      
      SingleRenderLayer depthRenderLayer;
      uint lastMeshID;
      csHash<csRef<csShaderVariable>, csPtrKey<iShaderVariableContext> > meshIDs;
      
      ViewSetup (PersistentData& persist, CS::RenderManager::RenderView* rview)
       : persist (persist), rview (rview),
         doFixedCloseShadow (persist.fixedCloseShadow > 0),
         depthRenderLayer (persist.settings.shadowShaderType, 
           persist.settings.shadowDefaultShader),
         lastMeshID (0)
      {
	// PSSM: split layers
	
	float _near = SMALL_Z;
	// @@@ FIXME: arbitrary; make configurable
	float _far = persist.farZ;
	
	int firstPart = 0;
	numParts = persist.numSplits + 1;
	if (doFixedCloseShadow)
	{
	  numParts++;
	}
	splitDists = new float[numParts+1];
	if (doFixedCloseShadow)
	{
	  firstPart = 1;
	  splitDists[0] = _near;
	  _near = persist.fixedCloseShadow;
	}
      
	splitDists[firstPart] = _near;
	for (int i = firstPart; i <= numParts; i++)
	{
	  const float n = _near, f = _far;
	  const float iFrac = (float)(i-firstPart)/(float)(numParts-firstPart);
	  splitDists[i] = (n * pow (f/n, iFrac) + n + (f-n)*iFrac)*0.5f;
	}
	// Get visible frustum in to lx/ty,rx/by
	{
	  int frameWidth = rview->GetGraphics3D()->GetWidth ();
	  int frameHeight = rview->GetGraphics3D()->GetHeight ();
	  lx = 0;
	  rx = frameWidth;
	  ty = frameHeight;
	  by = 0;
	}
      }
      
      ~ViewSetup() { delete[] splitDists; }
    
      void PostLightSetup (typename RenderTree::ContextNode& context,
        const LayerConfigType& layerConfig)
      {
      }
    };
    
    struct CachedLightData :
      public CS::Memory::CustomAllocated
    {
      bool frustumsSetup;
      // Transform light space to post-project light space
      CS::Math::Matrix4 lightProject;
      struct SuperFrustum : public CS::Utility::FastRefCount<SuperFrustum>
      {
	// Transform world space to light space
	csReversibleTransform world2light_base;
	csReversibleTransform world2light_rotated;
	csMatrix3 frustumRotation;
	struct Frustum
	{
	  // Volume (in post-project light space)
	  csBox3 volumeLS;
	  csBox3 volumePP;
	  csShaderVariable* shadowMapProjectSV;
	  csShaderVariable* shadowMapUnscaleSV;
	  csShaderVariable* shadowMapDimSV;
	  csShaderVariable* shadowClipSV;
	  csShaderVariable** textureSVs;
	  
	  // Object bboxes in post-project light space
	  csArray<csBox3> containedObjectsPP;
	  
	  CS::Utility::MeshFilter meshFilter;
	  
	  Frustum() : textureSVs (0) {}
	  ~Frustum() { delete[] textureSVs; }
	};
	Frustum* frustums;
	
	~SuperFrustum() { delete[] frustums; }
      };
      csRefArray<SuperFrustum> lightFrustums;
      
      CachedLightData() : frustumsSetup (false) {}
      
      uint GetSublightNum() const { return (uint)lightFrustums.GetSize(); }

      void SetupFrame (RenderTree& tree, ShadowPSSM& shadows, iLight* light)
      {
        if (light->GetFlags().Check (CS_LIGHT_NOSHADOWS)) return;
        
        ViewSetup& viewSetup = shadows.viewSetup;
        csRef<iCamera> camera (viewSetup.rview->GetCamera());
        if (!frustumsSetup)
        {
          float lightNear = SMALL_Z;
          float lightCutoff = light->GetCutoffDistance();
          CS::Math::Matrix4 lightProject;
          csLightType ltype = light->GetType();
          int numFrustums = 1;
          switch (ltype)
          {
            case CS_LIGHT_DIRECTIONAL:
              numFrustums = 1;
              {
		lightProject = CS::Math::Matrix4 (
		  1.0f/lightCutoff, 0, 0, 0,
		  0, 1.0f/lightCutoff, 0, 0,
		  0, 0, -1, 0,
		  0, 0, 0, 1);
	      }
              break;
            case CS_LIGHT_POINTLIGHT:
            case CS_LIGHT_SPOTLIGHT:
              numFrustums = (ltype == CS_LIGHT_POINTLIGHT) ? 6 : 1;
              {
		CS::Math::Matrix4 flipZW (
		  1, 0, 0, 0,
		  0, 1, 0, 0,
		  0, 0, -1, 0,
		  0, 0, 0, -1);
		lightProject = flipZW * CS::Math::Projections::Frustum (
		  -lightNear, lightNear, -lightNear, lightNear, // @@@ TODO: use spot angle
		  lightNear, lightCutoff);
              }
              break;
          }
          const csReversibleTransform& world2light_base (
            light->GetMovable()->GetFullTransform());
	  this->lightProject = lightProject;
            
          const csBox3& lightBBox = light->GetLocalBBox();
          if (tree.IsDebugFlagEnabled (shadows.persist.dbgLightBBox))
          {
	    tree.AddDebugBBox (lightBBox,
	      world2light_base.GetInverse(),
	      csColor (1, 1, 1));
	  }

          static const csMatrix3 frustRotationMatrices[6] =
          {
            csMatrix3 (), // must be identity
            csMatrix3 (0, 0, 1,  0, 1, 0,  -1, 0, 0),
            csMatrix3 (1, 0, 0,  0, 0, 1,  0, -1, 0),
            csMatrix3 (0, 0, -1,  0, 1, 0,  1, 0, 0),
            csMatrix3 (1, 0, 0,  0, 0, -1,  0, 1, 0),
            csMatrix3 (1, 0, 0,  0, -1, 0,  0, 0, -1) // last must be 180 deg into other direction
          };
	  csVector2 corner2D[4] = {
	    csVector2 (viewSetup.lx, viewSetup.ty),
	    csVector2 (viewSetup.lx, viewSetup.by),
	    csVector2 (viewSetup.rx, viewSetup.ty),
	    csVector2 (viewSetup.rx, viewSetup.by)
	  };

	  LightingVariablesHelper lightVarsHelper (viewSetup.persist.lightVarsPersist);
	  for (int f = 0; f < numFrustums; f++)
	  {
	    /* Compute intersection of light frustum with view frustums:
		1. Compute corners of split frustums
		2. Translate corners to light space
		3. Compute bounding box from corners
	      */
	    csRef<SuperFrustum> newFrust;
	    newFrust.AttachNew (new SuperFrustum);
	    SuperFrustum& superFrustum = *(lightFrustums[lightFrustums.Push (
	      newFrust)]);
	    superFrustum.world2light_base = world2light_base;
	    superFrustum.world2light_rotated = world2light_base;
	    superFrustum.frustumRotation = frustRotationMatrices[f];
	    superFrustum.world2light_rotated.SetO2T (
	      superFrustum.world2light_rotated.GetO2T()
	      * frustRotationMatrices[f]);
	   
            superFrustum.frustums = new typename SuperFrustum::Frustum[viewSetup.numParts];
        
	    for (int i = 0; i < viewSetup.numParts; i++)
	    {
	      bool partFixed = viewSetup.doFixedCloseShadow && (i == 0);
	      
	      typename SuperFrustum::Frustum& lightFrustum =
	        superFrustum.frustums[i];
	      
	      if (shadows.persist.limitedShadow)
	        lightFrustum.meshFilter.SetFilterMode (CS::Utility::MESH_FILTER_INCLUDE);
	        
	      // Frustum corner, camera space
	      csVector3 cornerCam;
	      // Frustum corner, world space
	      csVector3 cornerWorld;
	      // Frustum corner, light space
	      csVector3 cornerLight;
	      // Frustum slice bbox, light space
	      csBox3 frustumLight;
	      csBox3 frustumCam;
	      frustumLight.StartBoundingBox();
	      frustumCam.StartBoundingBox();
	      for (int c = 0; c < 4; c++)
	      {
		cornerCam = camera->InvPerspective (
		  corner2D[c], viewSetup.splitDists[i]);
		frustumCam.AddBoundingVertex (cornerCam);
		cornerWorld = camera->GetTransform().This2Other (
		  cornerCam);
		cornerLight = superFrustum.world2light_rotated.Other2This (cornerWorld);
		frustumLight.AddBoundingVertex (cornerLight);
		
		cornerCam = camera->InvPerspective (
		  corner2D[c], viewSetup.splitDists[i+1]);
		frustumCam.AddBoundingVertex (cornerCam);
		cornerWorld = camera->GetTransform().This2Other (
		  cornerCam);
		cornerLight = superFrustum.world2light_rotated.Other2This (cornerWorld);
		frustumLight.AddBoundingVertex (cornerLight);
	      }
	      if (tree.IsDebugFlagEnabled (shadows.persist.dbgSplitFrustumCam))
	      {
		tree.AddDebugBBox (frustumCam,
		  camera->GetTransform().GetInverse(),
		  csColor (1, 0, 1));
	      }
	      if (tree.IsDebugFlagEnabled (shadows.persist.dbgSplitFrustumLight))
	      {
		tree.AddDebugBBox (frustumLight,
		  superFrustum.world2light_rotated.GetInverse(),
		  csColor (0, 1, 0));
	      }
	      if (!partFixed) frustumLight *= lightBBox;
	      if (tree.IsDebugFlagEnabled (shadows.persist.dbgSplitFrustumLight))
	      {
		tree.AddDebugBBox (frustumLight,
		  superFrustum.world2light_rotated.GetInverse(),
		  csColor (0, 1, 1));
	      }
	      
	      // Frustum slice corner, light space before W divide
	      csVector4 cornerUndiv;
	      // Frustum slice corner, light space after W divide
	      csVector3 cornerDiv;
	      
	      lightFrustum.volumePP.StartBoundingBox();
	      for (int c = 0; c < 7; c++)
	      {
		cornerLight = frustumLight.GetCorner (c);
		cornerUndiv = lightProject * csVector4 (cornerLight);
		cornerDiv =
		  csVector3 (cornerUndiv.x, cornerUndiv.y, cornerUndiv.z);
		cornerDiv /= cornerUndiv.w;
		lightFrustum.volumePP.AddBoundingVertex (cornerDiv);
	      }
	      lightFrustum.volumeLS = frustumLight;
	    
	      lightFrustum.shadowMapProjectSV = lightVarsHelper.CreateTempSV (
		viewSetup.persist.svNames.GetLightSVId (
		  csLightShaderVarCache::lightShadowMapProjection));
	      lightFrustum.shadowMapProjectSV->SetArraySize (4);
	      for (int j = 0; j < 4; j++)
	      {
		csShaderVariable* item = lightVarsHelper.CreateTempSV (
		  CS::InvalidShaderVarStringID);
		lightFrustum.shadowMapProjectSV->SetArrayElement (j, item);
	      }
	      lightFrustum.shadowMapUnscaleSV = lightVarsHelper.CreateTempSV (
		viewSetup.persist.unscaleSVName);
	      lightFrustum.shadowMapDimSV = lightVarsHelper.CreateTempSV (
		viewSetup.persist.svNames.GetLightSVId (
		  csLightShaderVarCache::lightShadowMapPixelSize));
		  
	      lightFrustum.shadowClipSV = lightVarsHelper.CreateTempSV (
		viewSetup.persist.shadowClipSVName);
	      lightFrustum.shadowClipSV->SetValue (csVector2 (
		viewSetup.splitDists[i], 
		(i+1 == viewSetup.numParts) ? FLT_MAX : viewSetup.splitDists[i+1]));
		  
	      size_t numTex = viewSetup.persist.settings.targets.GetSize();
	      if (lightFrustum.textureSVs == 0)
	      {
		lightFrustum.textureSVs = new csShaderVariable*[numTex];
	      }
	      for (size_t t = 0; t < numTex; t++)
	      {
		lightFrustum.textureSVs[t] = lightVarsHelper.CreateTempSV (
		  viewSetup.persist.settings.targets[t]->svName);
	      }
	    }
	  }
        
          frustumsSetup = true;
        }
      }
      
      void AddShadowMapTarget (typename RenderTree::MeshNode* meshNode,
	PersistentData& persist, const SingleRenderLayer& layerConfig,
	RenderTree& renderTree, iLight* light, CachedLightData& lightData,
	ViewSetup& viewSetup)
      {
        if (light->GetFlags().Check (CS_LIGHT_NOSHADOWS)) return;
        
        typename RenderTree::ContextNode& context = meshNode->owner;
      
        CS_ALLOC_STACK_ARRAY(iTextureHandle*, texHandles,
          persist.settings.targets.GetSize());
	for (size_t l = 0; l < lightData.lightFrustums.GetSize(); l++)
	{
	  const SuperFrustum& superFrust = *(lightData.lightFrustums[l]);
	  for (int frustNum = 0; frustNum < viewSetup.numParts; frustNum++)
	  {
	    const typename SuperFrustum::Frustum& lightFrust = superFrust.frustums[frustNum];
	    //if (lightFrust.containedObjectsPP.GetSize() == 0) continue;
	    
            bool partFixed = viewSetup.doFixedCloseShadow && (frustNum == 0);
	    
	    /* Fit map to the bounding box of all shadowed objects.
	      - If the shadowed objects are smaller than the light frustum in some
		dimension makes sure the shadow map is used optimally. */
	    csBox3 allObjsBoxPP;
	    float allObjsMaxZ;
	    if (!partFixed && (lightFrust.containedObjectsPP.GetSize() > 0))
	    {
	      allObjsBoxPP = lightFrust.containedObjectsPP[0];
	      for (size_t i = 1; i < lightFrust.containedObjectsPP.GetSize(); i++)
	      {
		allObjsBoxPP += lightFrust.containedObjectsPP[i];
	      }
	      allObjsMaxZ = allObjsBoxPP.MaxZ();
	      allObjsBoxPP *= lightFrust.volumePP;
	      csBox3 clipToView;
	      if (light->GetType() == CS_LIGHT_DIRECTIONAL)
		clipToView = csBox3 (csVector3 (-1, -1, -FLT_MAX),
		  csVector3 (1, 1, 0));
	      else
		clipToView = csBox3 (csVector3 (-1, -1, 0),
		  csVector3 (1, 1, FLT_MAX));
	      if (allObjsBoxPP.Empty())
	      {
		//continue;
	      }
	      allObjsBoxPP *= clipToView;
	    }
	    else
	    {
	      allObjsBoxPP = lightFrust.volumePP;
	      allObjsMaxZ = allObjsBoxPP.MaxZ();
	    }
	    
	    CS::RenderManager::RenderView* rview = context.renderView;
	  #include "csutil/custom_new_disable.h"
	    csRef<CS::RenderManager::RenderView> newRenderView;
	    newRenderView.AttachNew (new (
	      renderTree.GetPersistentData().renderViewPool) RenderView);
	  #include "csutil/custom_new_enable.h"
	    newRenderView->SetEngine (rview->GetEngine ());
	    newRenderView->SetThisSector (rview->GetThisSector ());
	    
	    CS::Math::Matrix4 crop;
	    if (lightFrust.containedObjectsPP.GetSize() == 0)
	    {
	      crop = CS::Math::Matrix4 (
		0, 0, 0, 10,
		0, 0, 0, 10,
		0, 0, 1, 0,
		0, 0, 0, 1);
		
	      lightFrust.shadowMapUnscaleSV->SetValue (csVector4 (
	        0, 0, -10, -10));
	    }
	    else
	    {
	      const float frustW = allObjsBoxPP.MaxX() - allObjsBoxPP.MinX();
	      const float frustH = allObjsBoxPP.MaxY() - allObjsBoxPP.MinY();
	      const float cropScaleX = 2.0f/frustW;
	      const float cropScaleY = -2.0f/frustH;
	      const float cropShiftX =
		(-1.0f * (allObjsBoxPP.MaxX() + allObjsBoxPP.MinX()))/frustW;
	      const float cropShiftY =
		(1.0f * (allObjsBoxPP.MaxY() + allObjsBoxPP.MinY()))/frustH;
	      crop = CS::Math::Matrix4 (
		cropScaleX, 0, 0, cropShiftX,
		0, cropScaleY, 0, cropShiftY,
		0, 0, 1, 0,
		0, 0, 0, 1);
		
	      const float uncropScaleX = 1.0f/cropScaleX;
	      const float uncropScaleY = 1.0f/cropScaleY;
	      const float uncropShiftX = -cropShiftX/cropScaleX;
	      const float uncropShiftY = -cropShiftY/cropScaleY;
	      lightFrust.shadowMapUnscaleSV->SetValue (csVector4 (
	        uncropScaleX, uncropScaleY, uncropShiftX, uncropShiftY));
	     }
	      
	    /* The minimum Z over all parts is used to avoid clipping shadows of 
	      casters closer to the light than the split plane.
	      Ideally, allObjsBoxPP.MinZ() would be mapped to depth -1, and depths
	      below clamped */
	    float n = allObjsBoxPP.MinZ();
	    float maxZ = allObjsMaxZ;
	    float f = maxZ + EPSILON;
	    /* Sometimes n==f which would result in an invalid matrix w/o EPSILON */
	    CS::Math::Matrix4 Mortho = CS::Math::Projections::Ortho (-1, 1, 1, -1, -n, -f);
	    CS::Math::Matrix4 matrix = ((Mortho * crop) * lightData.lightProject)
	     * CS::Math::Matrix4 (superFrust.frustumRotation);
	    
	    for (int i = 0; i < 4; i++)
	    {
	      csShaderVariable* item = lightFrust.shadowMapProjectSV->GetArrayElement (i);
	      item->SetValue (matrix.Row (i));
	    }
		
	    int shadowMapSize;
            if (partFixed)
              shadowMapSize = persist.shadowMapResClose;
            else
            {
	      switch (light->GetType())
	      {
		default:
		case CS_LIGHT_DIRECTIONAL:
		  shadowMapSize = persist.shadowMapResD;
		  break;
		case CS_LIGHT_POINTLIGHT:
		  shadowMapSize = persist.shadowMapResP;
		  break;
		case CS_LIGHT_SPOTLIGHT:
		  shadowMapSize = persist.shadowMapResS;
		  break;
	      }
	    }
	    lightFrust.shadowMapDimSV->SetValue (csVector4 (1.0f/shadowMapSize,
	      1.0f/shadowMapSize, shadowMapSize, shadowMapSize));
	      
	    if (lightFrust.containedObjectsPP.GetSize() == 0)
	    {
	      for (size_t t = 0; t < persist.settings.targets.GetSize(); t++)
	      {
		lightFrust.textureSVs[t]->SetValue (0);
	      }
	      continue;
	    }
    
	    csReversibleTransform view = superFrust.world2light_base;
	    
	    csRef<iCustomMatrixCamera> shadowViewCam =
	      newRenderView->GetEngine()->CreateCustomMatrixCamera();
	    newRenderView->SetCamera (shadowViewCam->GetCamera());
	    shadowViewCam->SetProjectionMatrix (matrix);
	    shadowViewCam->GetCamera()->SetTransform (view);
	    
	    for (size_t t = 0; t < persist.settings.targets.GetSize(); t++)
	    {
	      iTextureHandle* tex =
		persist.settings.targets[t]->texCache.QueryUnusedTexture (
		  shadowMapSize, shadowMapSize, 0);
	      lightFrust.textureSVs[t]->SetValue (tex);
	      if (renderTree.IsDebugFlagEnabled (persist.dbgShadowTex))
	        renderTree.AddDebugTexture (tex);
	      texHandles[t] = tex;
	    }
	    
	    newRenderView->SetViewDimensions (shadowMapSize, shadowMapSize);
	    csBox2 clipBox (0, 0, shadowMapSize, shadowMapSize);
	    csRef<iClipper2D> newView;
	    newView.AttachNew (new csBoxClipper (clipBox));
	    newRenderView->SetClipper (newView);
	    newRenderView->GetMeshFilter() = lightFrust.meshFilter;
    
	    // Create a new context for shadow map w/ computed view
	    typename RenderTree::ContextNode* shadowMapCtx = 
	      renderTree.CreateContext (newRenderView);
	    for (size_t t = 0; t < persist.settings.targets.GetSize(); t++)
	    {
	      shadowMapCtx->renderTargets[
		persist.settings.targets[t]->attachment].texHandle = 
		  texHandles[t];
	    }
	    shadowMapCtx->drawFlags = CSDRAW_CLEARSCREEN | CSDRAW_CLEARZBUFFER;
	    shadowMapCtx->postEffects = persist.settings.postEffects;
	    
	    /* @@@ FIXME: This will break as soon as the shadowmaps have
	      different resolutions!
	      Probably the post effects manager should be changed to handle
	      changing resolutions well */
	    if (shadowMapCtx->postEffects.IsValid())
	      shadowMapCtx->postEffects->SetupView (shadowMapSize, shadowMapSize);
    
	    // Setup the new context
	    ShadowmapContextSetup contextFunction (layerConfig,
	      persist.shaderManager, viewSetup, persist.settings.provideIDs);
	    contextFunction (*shadowMapCtx);
	  }
	}
      }

      void ClearFrameData()
      {
        frustumsSetup = false;
        lightFrustums.Empty();
      }
    };
private:
    class ShadowmapContextSetup
    {
      class MeshIDSVSetup
      {
      public:    
	MeshIDSVSetup (SVArrayHolder& svArrays, ViewSetup& viewSetup) 
	 : svArrays (svArrays), viewSetup (viewSetup)
	{
	  tempStack.Setup (svArrays.GetNumSVNames ());
	}
    
	void operator() (typename RenderTree::MeshNode* node)
	{
	  for (size_t i = 0; i < node->meshes.GetSize (); ++i)
	  {
	    typename RenderTree::MeshNode::SingleMesh& mesh = node->meshes[i];
	    csShaderVariable* svMeshID = viewSetup.meshIDs.Get (mesh.meshObjSVs, 0);
	    if (!svMeshID) continue;
	    
	    tempStack[svMeshID->GetName()] = svMeshID;
	    
	    for (size_t layer = 0; layer < svArrays.GetNumLayers(); ++layer)
	    {
	      // Back-merge it onto the real one
	      csShaderVariableStack localStack;
	      svArrays.SetupSVStack (localStack, layer, mesh.contextLocalId);
	      localStack.MergeFront (tempStack);
	    }
	  }
	}
    
      private:
	SVArrayHolder& svArrays; 
	csShaderVariableStack tempStack;
	ViewSetup& viewSetup;
      };
    public:
      ShadowmapContextSetup (const SingleRenderLayer& layerConfig,
        iShaderManager* shaderManager, ViewSetup& viewSetup,
        bool doIDTexture)
	: layerConfig (layerConfig), shaderManager (shaderManager),
	  viewSetup (viewSetup), doIDTexture (doIDTexture)
      {
    
      }
    
      void operator() (typename RenderTree::ContextNode& context)
      {
	CS::RenderManager::RenderView* rview = context.renderView;
	iSector* sector = rview->GetThisSector ();
    
	// @@@ This is somewhat "boilerplate" sector/rview setup.
	rview->SetThisSector (sector);
	sector->CallSectorCallbacks (rview);
	// Make sure the clip-planes are ok
	CS::RenderViewClipper::SetupClipPlanes (rview->GetRenderContext ());
    
	if (context.owner.IsDebugFlagEnabled (
	    viewSetup.persist.dbgSplitFrustumLight))
	  context.owner.AddDebugClipPlanes (rview);
	
	// Do the culling
	iVisibilityCuller* culler = sector->GetVisibilityCuller ();
	Viscull<RenderTree> (context, rview, culler);
    
        // TODO: portals
	
	// Sort the mesh lists  
	{
	  StandardMeshSorter<RenderTree> mySorter (rview->GetEngine ());
	  mySorter.SetupCameraLocation (rview->GetCamera ()->GetTransform ().GetOrigin ());
	  ForEachMeshNode (context, mySorter);
	}
    
	// After sorting, assign in-context per-mesh indices
	{
	  SingleMeshContextNumbering<RenderTree> numbering;
	  ForEachMeshNode (context, numbering);
	}

	// Setup the SV arrays
	// Push the default stuff
	SetupStandardSVs (context, layerConfig, shaderManager, sector);
    
	// Setup the material&mesh SVs
	{
	  StandardSVSetup<RenderTree, SingleRenderLayer> svSetup (
	    context.svArrays, layerConfig);
    
	  ForEachMeshNode (context, svSetup);
	}
	// Set up mesh ID SVs
	if (doIDTexture)
	{
	  MeshIDSVSetup svSetup (context.svArrays, viewSetup);
    
	  ForEachMeshNode (context, svSetup);
	}
    
	SetupStandardShader (context, shaderManager, layerConfig);
    
	// Setup shaders and tickets
	SetupStandardTicket (context, shaderManager, layerConfig);
      }
    
    
    private:
      const SingleRenderLayer& layerConfig;
      iShaderManager* shaderManager;
      ViewSetup& viewSetup;
      bool doIDTexture;
    };
  public:

    /**
     * Data used by the shadow handler that needs to persist over multiple frames.
     * Generally stored inside the light setup's persistent data.
     */
    struct PersistentData
    {
      uint dbgSplitFrustumCam;
      uint dbgSplitFrustumLight;
      uint dbgLightBBox;
      uint dbgShadowTex;
      uint dbgFlagShadowClipPlanes;
      csLightShaderVarCache svNames;
      CS::ShaderVarStringID unscaleSVName;
      CS::ShaderVarStringID shadowClipSVName;
      LightingSorter::PersistentData lightSorterPersist;
      LightingVariablesHelper::PersistentData lightVarsPersist;
      iShaderManager* shaderManager;

      csString configPrefix;
      ShadowSettings settings;
      
      bool limitedShadow;
      int shadowMapResD, shadowMapResP, shadowMapResS;
      int shadowMapResClose;
      int numSplits;
      float farZ;
      float fixedCloseShadow;

      PersistentData() : limitedShadow (false)
      {
      }

      ~PersistentData()
      {
      }
      
      /// Set the prefix for configuration settings
      void SetConfigPrefix (const char* configPrefix)
      {
        this->configPrefix = configPrefix;
      }
      
      void Initialize (iObjectRegistry* objectReg,
                       RenderTreeBase::DebugPersistent& dbgPersist)
      {
        csRef<iShaderManager> shaderManager =
          csQueryRegistry<iShaderManager> (objectReg);
      
        this->shaderManager = shaderManager;
        iShaderVarStringSet* strings = shaderManager->GetSVNameStringset();
	svNames.SetStrings (strings);
	
	csConfigAccess cfg (objectReg);
	if (configPrefix.IsEmpty())
	{
	  settings.ReadSettings (objectReg, "Depth");
	}
	else
	{
	  settings.ReadSettings (objectReg, 
	    cfg->GetStr (
	      csString().Format ("%s.ShadowsType", configPrefix.GetData()), "Depth"));
	  limitedShadow = cfg->GetBool (
            csString().Format ("%s.LimitedShadow", configPrefix.GetData()), false);
          numSplits = cfg->GetInt (
            csString().Format ("%s.NumSplits", configPrefix.GetData()), 2);
          farZ = cfg->GetFloat (
            csString().Format ("%s.FarZ", configPrefix.GetData()), 100);
          int shadowMapRes = cfg->GetInt (
            csString().Format ("%s.ShadowMapResolution", configPrefix.GetData()), 512);
          shadowMapResD = cfg->GetInt (
            csString().Format ("%s.ShadowMapResolution.Directional", configPrefix.GetData()),
            shadowMapRes);
          shadowMapResP = cfg->GetInt (
            csString().Format ("%s.ShadowMapResolution.Point", configPrefix.GetData()),
            shadowMapRes/2);
          shadowMapResS = cfg->GetInt (
            csString().Format ("%s.ShadowMapResolution.Spot", configPrefix.GetData()),
            shadowMapRes/2);
          shadowMapResClose = cfg->GetInt (
            csString().Format ("%s.CloseShadowMapResolution", configPrefix.GetData()),
            shadowMapRes);
          fixedCloseShadow = cfg->GetFloat (
            csString().Format ("%s.FixedCloseShadow", configPrefix.GetData()), 0);
	}
	
	unscaleSVName = strings->Request ("light shadow map unscale");
	shadowClipSVName = strings->Request ("light shadow clip");
	
	dbgSplitFrustumCam = dbgPersist.RegisterDebugFlag ("draw.pssm.split.frustum.cam");
	dbgSplitFrustumLight = dbgPersist.RegisterDebugFlag ("draw.pssm.split.frustum.light");
	dbgLightBBox = dbgPersist.RegisterDebugFlag ("draw.pssm.lightbbox");
	dbgShadowTex = dbgPersist.RegisterDebugFlag ("textures.shadow");
	dbgFlagShadowClipPlanes =
	  dbgPersist.RegisterDebugFlag ("draw.clipplanes.shadow");
      }
      void UpdateNewFrame ()
      {
        csTicks time = csGetTicks ();
        settings.AdvanceFrame (time);
        lightVarsPersist.UpdateNewFrame();
      }
    };
    
    typedef ViewSetup ShadowParameters;

    ShadowPSSM (PersistentData& persist,
      const LayerConfigType& layerConfig,
      typename RenderTree::MeshNode* node, 
      ViewSetup& viewSetup) 
      : persist (persist), layerConfig (layerConfig), 
        renderTree (node->owner.owner), meshNode (node),
        viewSetup (viewSetup)
    { }
        
    uint HandleOneLight (typename RenderTree::MeshNode::SingleMesh& singleMesh,
                         iLight* light, CachedLightData& lightData,
                         csShaderVariableStack* lightStacks,
                         uint lightNum, uint subLightNum)
    {
      LightingVariablesHelper lightVarsHelper (viewSetup.persist.lightVarsPersist);
      
      if (persist.settings.provideIDs && !singleMesh.svMeshID.IsValid())
      {
        singleMesh.svMeshID = lightVarsHelper.CreateTempSV (
	  viewSetup.persist.settings.svMeshIDName);
        lightStacks[0][singleMesh.svMeshID->GetName()] = singleMesh.svMeshID;
        uint meshID = ++viewSetup.lastMeshID;
        singleMesh.svMeshID->SetValue ((int)meshID);
        viewSetup.meshIDs.Put (singleMesh.meshObjSVs, singleMesh.svMeshID);
      }
      
      typename CachedLightData::SuperFrustum& superFrust =
	*(lightData.lightFrustums[subLightNum]);
      
      csBox3 meshBboxLS;
      csVector3 vLight;
      vLight = superFrust.world2light_rotated.Other2This (
        singleMesh.bbox.GetCorner (0));
      meshBboxLS.StartBoundingBox (csVector3 (vLight.x,
	vLight.y, vLight.z));
      csBox3 meshBboxLightPP;
      csVector4 vLightPP;
      vLightPP = lightData.lightProject * csVector4 (vLight);
      vLightPP /= vLightPP.w;
      meshBboxLightPP.StartBoundingBox (csVector3 (vLightPP.x,
	vLightPP.y, vLightPP.z));
      for (int c = 1; c < 8; c++)
      {
	vLight = superFrust.world2light_rotated.Other2This (
	  singleMesh.bbox.GetCorner (c));
	meshBboxLS.AddBoundingVertexSmart (csVector3 (vLight.x,
	  vLight.y, vLight.z));
	vLightPP = lightData.lightProject * csVector4 (vLight);
	vLightPP /= vLightPP.w;
	meshBboxLightPP.AddBoundingVertexSmart (csVector3 (vLightPP.x,
	  vLightPP.y, vLightPP.z));
      }
	
      uint spreadFlags = 0;
      int s = 0;
      for (int f = 0; f < viewSetup.numParts; f++)
      {
	typename CachedLightData::SuperFrustum::Frustum& lightFrustum =
	  superFrust.frustums[f];
        //if (lightFrustum.volumePP.Empty()) continue;
	if (!lightFrustum.volumeLS.TestIntersect (meshBboxLS))
	//if (!lightFrustum.volumePP.TestIntersect (meshBboxLightPP))
	{
	  continue;
	}
      
        if (persist.limitedShadow)
        {
	  if (singleMesh.meshFlags.Check (CS_ENTITY_LIMITEDSHADOWCAST))
	  {
	    /* Default for focused shadows: only the focused meshes cast shadows;
	      the mesh filter is inverted */
	    lightFrustum.meshFilter.AddFilterMesh (singleMesh.meshWrapper);
	    lightFrustum.containedObjectsPP.Push (meshBboxLightPP
	      /* * lightFrustum.volumePP*/);
	  }
        }
        else
        {
	  if (!singleMesh.meshFlags.Check (CS_ENTITY_NOSHADOWCAST))
	  {
	    // Mesh casts shadow
	    lightFrustum.containedObjectsPP.Push (meshBboxLightPP
	      /* * lightFrustum.volumePP*/);
	  }
	  else
	  {
	    // Mesh does not cast shadow
	    lightFrustum.meshFilter.AddFilterMesh (singleMesh.meshWrapper);
	  }
        }
	  
	if (singleMesh.meshFlags.Check (CS_ENTITY_NOSHADOWRECEIVE))
          continue;
      
	// Add shadow map SVs
	lightVarsHelper.MergeAsArrayItem (lightStacks[s],
	  lightFrustum.shadowMapProjectSV, lightNum);
	lightVarsHelper.MergeAsArrayItem (lightStacks[s],
	  lightFrustum.shadowMapUnscaleSV, lightNum);
	lightVarsHelper.MergeAsArrayItem (lightStacks[s], 
	  lightFrustum.shadowMapDimSV, lightNum);
	if (lightStacks[s].GetSize() > lightFrustum.shadowClipSV->GetName())
	{
	  lightStacks[s][lightFrustum.shadowClipSV->GetName()] =
	    lightFrustum.shadowClipSV;
	 }
	  
	for (size_t t = 0; t < persist.settings.targets.GetSize(); t++)
	{
	  lightVarsHelper.MergeAsArrayItem (lightStacks[s], 
	    lightFrustum.textureSVs[t], lightNum);
	}
	spreadFlags |= (1 << s);
	s++;
      }
      return spreadFlags;
    }
    
    static bool NeedFinalHandleLight() { return true; }
    void FinalHandleLight (iLight* light, CachedLightData& lightData)
    {
      lightData.AddShadowMapTarget (meshNode, persist,
        viewSetup.depthRenderLayer, renderTree, light, lightData, viewSetup);
      
      lightData.ClearFrameData();
    }

    csFlags GetLightFlagsMask () const { return csFlags (0); }
    
    size_t GetLightLayerSpread() const { return viewSetup.numParts; }
  protected:
    PersistentData& persist;
    const LayerConfigType& layerConfig;
    RenderTree& renderTree;
    typename RenderTree::MeshNode* meshNode;
    ViewSetup& viewSetup;
  };

}
}

#endif // __CS_CSPLUGINCOMMON_RENDERMANAGER_SHADOW_PSSM_H__
