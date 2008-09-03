/*
    Copyright (C) 2007 by Marten Svanfeldt

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

#ifndef __CS_RM_UNSHADOWED_H__
#define __CS_RM_UNSHADOWED_H__

#include "csplugincommon/rendermanager/autofx_reflrefr.h"
#include "csplugincommon/rendermanager/debugcommon.h"
#include "csplugincommon/rendermanager/hdrexposure.h"
#include "csplugincommon/rendermanager/shadow_pssm.h"
#include "csutil/scf_implementation.h"
#include "iutil/comp.h"
#include "iengine/rendermanager.h"

CS_PLUGIN_NAMESPACE_BEGIN(RMShadowedPSSM)
{
  template<typename RenderTreeType, typename LayerConfigType, bool doShadow>
  class StandardContextSetup;
  
  class RenderTreeTraits : public CS::RenderManager::RenderTreeStandardTraits
  {
  public:
    typedef CS::RenderManager::ShadowPSSMExtraMeshData MeshExtraDataType;
  };
  
  typedef CS::RenderManager::RenderTree<RenderTreeTraits> RenderTreeType;

  class RMShadowedPSSM : public scfImplementation5<RMShadowedPSSM, 
                                                 iRenderManager, 
                                                 iRenderManagerTargets,
                                                 iRenderManagerPostEffects,
                                                 iComponent,
                                                 scfFakeInterface<iDebugHelper> >,
                         public CS::RenderManager::RMDebugCommon<RenderTreeType>
  {
  public:
    RMShadowedPSSM (iBase* parent);

    //---- iRenderManager ----
    virtual bool RenderView (iView* view);

    //---- iRenderManagerTargets ----
    virtual void RegisterRenderTarget (iTextureHandle* target, 
      iView* view, int subtexture = 0, uint flags = 0)
    {
      targets.RegisterRenderTarget (target, view, subtexture, flags);
    }
    virtual void UnregisterRenderTarget (iTextureHandle* target,
      int subtexture = 0)
    {
      targets.UnregisterRenderTarget (target, subtexture);
    }
  
    //---- iRenderManagerPostEffects ----
    void ClearLayers() { postEffects.ClearLayers(); }
    bool AddLayersFromDocument (iDocumentNode* node)
    {
      CS::RenderManager::PostEffectLayersParser postEffectsParser (objectReg);
      return postEffectsParser.AddLayersFromDocument (node, postEffects);
    }
    bool AddLayersFromFile (const char* filename)
    {
      CS::RenderManager::PostEffectLayersParser postEffectsParser (objectReg);
      return postEffectsParser.AddLayersFromFile (filename, postEffects);
    }

    //---- iComponent ----
    virtual bool Initialize (iObjectRegistry*);

    // Target manager handler
    bool HandleTargetSetup (CS::ShaderVarStringID svName, csShaderVariable* sv, 
      iTextureHandle* textureHandle, iView*& localView)
    {
      return false;
    }
    
    typedef StandardContextSetup<RenderTreeType, 
      CS::RenderManager::MultipleRenderLayer, true> ContextSetupType;
    typedef StandardContextSetup<RenderTreeType, 
      CS::RenderManager::MultipleRenderLayer, false> ContextSetupType_Unshadowed;

    typedef CS::RenderManager::StandardPortalSetup<RenderTreeType, 
      ContextSetupType> PortalSetupType;

    typedef CS::RenderManager::DependentTargetManager<RenderTreeType, RMShadowedPSSM>
      TargetManagerType;

    typedef CS::RenderManager::ShadowPSSM<RenderTreeType,
      CS::RenderManager::MultipleRenderLayer> ShadowType;
    typedef CS::RenderManager::LightSetup<RenderTreeType, 
      CS::RenderManager::MultipleRenderLayer,
      ShadowType> LightSetupType;
    typedef CS::RenderManager::LightSetup<RenderTreeType, 
      CS::RenderManager::MultipleRenderLayer> LightSetupType_Unshadowed;

    typedef CS::RenderManager::AutoFX_ReflectRefract<RenderTreeType, 
      ContextSetupType, ContextSetupType> AutoReflectRefractType_SS;
    typedef CS::RenderManager::AutoFX_ReflectRefract<RenderTreeType, 
      ContextSetupType_Unshadowed, ContextSetupType> AutoReflectRefractType_US;
    typedef CS::RenderManager::AutoFX_ReflectRefract<RenderTreeType, 
      ContextSetupType, ContextSetupType_Unshadowed> AutoReflectRefractType_SU;
    typedef CS::RenderManager::AutoFX_ReflectRefract<RenderTreeType, 
      ContextSetupType_Unshadowed, ContextSetupType_Unshadowed>
      AutoReflectRefractType_UU;

  public:
    iObjectRegistry* objectReg;

    bool HandleTarget (RenderTreeType& renderTree, 
      const TargetManagerType::TargetSettings& settings);

    RenderTreeType::PersistentData treePersistent;
    PortalSetupType::PersistentData portalPersistent;
    LightSetupType::PersistentData lightPersistent;
    CS::RenderManager::AutoFX_ReflectRefract_Base::PersistentData
      reflectRefractPersistent;

    CS::RenderManager::PostEffectManager       postEffects;
    CS::RenderManager::HDRHelper hdr;
    CS::RenderManager::HDRExposureLinear hdrExposure;
    bool doHDRExposure;

    csRef<iShaderVarStringSet>  svNameStringSet;
    csRef<iStringSet>           stringSet;
    csRef<iShaderManager>       shaderManager;
    csRef<iEngine>              engine;
    csRef<iLightManager>        lightManager;

    CS::RenderManager::MultipleRenderLayer renderLayer;

    TargetManagerType targets;
    csSet<RenderTreeType::ContextNode*> contextsScannedForTargets;
    
    enum { rrShadowReflect = 1, rrShadowRefract = 2 };
    int refrRefrShadows;
    uint dbgFlagClipPlanes;
  };  

}
CS_PLUGIN_NAMESPACE_END(RMShadowedPSSM)

template<>
class csHashComputer<CS_PLUGIN_NAMESPACE_NAME(RMShadowedPSSM)::RenderTreeType::ContextNode*> : 
  public csHashComputerIntegral<CS_PLUGIN_NAMESPACE_NAME(RMShadowedPSSM)::RenderTreeType::ContextNode*> 
{};

#endif // __CS_RM_UNSHADOWED_H__
