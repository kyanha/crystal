/*
    Copyright (C) 2007 by Marten Svanfeldt
	      (C) 2008 by Frank Richter

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

#include "csplugincommon/rendermanager/dependenttarget.h"
#include "csplugincommon/rendermanager/hdrhelper.h"
#include "csplugincommon/rendermanager/lightsetup.h"
#include "csplugincommon/rendermanager/occluvis.h"
#include "csplugincommon/rendermanager/operations.h"
#include "csplugincommon/rendermanager/portalsetup.h"
#include "csplugincommon/rendermanager/posteffects.h"
#include "csplugincommon/rendermanager/render.h"
#include "csplugincommon/rendermanager/rendergroupinghandler.h"
#include "csplugincommon/rendermanager/renderlayers.h"
#include "csplugincommon/rendermanager/rendertree.h"
#include "csplugincommon/rendermanager/shadersetup.h"
#include "csplugincommon/rendermanager/standardsorter.h"
#include "csplugincommon/rendermanager/svsetup.h"
#include "csplugincommon/rendermanager/viscull.h"

#include "shadow_pssm.h"

#include "iutil/verbositymanager.h"
#include "ivaria/reporter.h"
#include "csutil/stringquote.h"

using namespace CS::RenderManager;

CS_PLUGIN_NAMESPACE_BEGIN(RMShadowedPSSM)
{

SCF_IMPLEMENT_FACTORY(RMShadowedPSSM)


/* Template magic to deal with different initializers for different
   ShadowType::ShadowParameter types. */
template<typename ShadowType, typename RenderTreeType>
struct WrapShadowParams
{
  static typename ShadowType::ShadowParameters Create (
    typename RenderTreeType::ContextNode& context,
    RMShadowedPSSM::ShadowType::PersistentData& persist,
    CS::RenderManager::RenderView* rview)
  {
    return typename ShadowType::ShadowParameters ();
  }
};

template<typename RenderTreeType>
struct WrapShadowParams<RMShadowedPSSM::ShadowType, RenderTreeType>
{
  static RMShadowedPSSM::ShadowType::ShadowParameters Create (
    typename RenderTreeType::ContextNode& context,
    RMShadowedPSSM::ShadowType::PersistentData& shadowPersist,
    CS::RenderManager::RenderView* rview)
  {
    RMShadowedPSSM::ShadowType::ShadowParameters shadowParam (shadowPersist, context.owner, rview);

    // setup shadows for all lights
    iLightList *list = context.sector->GetLights ();
    for(int i = 0; i < list->GetCount(); ++i)
      shadowParam(list->Get(i));

    // setup shadows for all meshes
    ForEachMeshNode (context, shadowParam);

    // finish shadow setup
    shadowParam();

    return shadowParam;
  }
};

template<typename RenderTreeType, typename LayerConfigType,
         typename LightSetupType>
class StandardContextSetup
{
public:
  typedef StandardContextSetup<RenderTreeType, LayerConfigType, LightSetupType> ThisType;
  typedef StandardPortalSetup<RenderTreeType, ThisType> PortalSetupType;
  typedef typename LightSetupType::ShadowHandlerType ShadowType;

  StandardContextSetup (RMShadowedPSSM* rmanager, 
    typename LightSetupType::PersistentData& lightPersistent,
    const LayerConfigType& layerConfig)
    : rmanager (rmanager), lightPersistent (lightPersistent),
      layerConfig (layerConfig),
    recurseCount (0), maxPortalRecurse (rmanager->maxPortalRecurse)
  {
  }

  StandardContextSetup (const StandardContextSetup& other)
    : rmanager (other.rmanager), 
      lightPersistent (other.lightPersistent),
      layerConfig (other.layerConfig),
      recurseCount (other.recurseCount),
      maxPortalRecurse (other.maxPortalRecurse)
  {
  }

  template<typename T2>
  StandardContextSetup (const T2& other,
    typename LightSetupType::PersistentData& lightPersistent,
    const LayerConfigType& layerConfig)
    : rmanager (other.rmanager), 
      lightPersistent (lightPersistent), layerConfig (layerConfig),
      recurseCount (other.recurseCount), maxPortalRecurse(other.maxPortalRecurse)
  {
  }
  
  void operator() (typename RenderTreeType::ContextNode& context,
    typename PortalSetupType::ContextSetupData& portalSetupData,
    bool recursePortals = true)
  {
    CS::RenderManager::RenderView* rview = context.renderView;
    iSector* sector = rview->GetThisSector ();

    if (recurseCount > maxPortalRecurse) return;
    
    // @@@ This is somewhat "boilerplate" sector/rview setup.
    sector->PrepareDraw (rview);
    // Make sure the clip-planes are ok
    CS::RenderViewClipper::SetupClipPlanes (rview->GetRenderContext ());
    
    if (context.owner.IsDebugFlagEnabled (rmanager->dbgFlagClipPlanes))
      context.owner.AddDebugClipPlanes (rview);

    // Do the culling
    iVisibilityCuller* culler = sector->GetVisibilityCuller ();
    Viscull<RenderTreeType> (context, rview, culler);

    // Set up all portals
    if (recursePortals)
    {
      recurseCount++;
      PortalSetupType portalSetup (rmanager->portalPersistent, *this);
      portalSetup (context, portalSetupData);
      recurseCount--;
    }
    
    /* Split out mesh nodes with different render grouping
       Note: Since creates new contexts and should be done after portal setup,
       lest portal rendering order gets wrong */
    {
      ContextSetupMeshes setupMeshes (*this);
      CS::RenderManager::RenderGroupingHandler<RenderTreeType,
	ContextSetupMeshes> groupingHandler (rview->GetEngine (), context.owner,
					     setupMeshes);
      groupingHandler (&context);
    }

    HandleContextMeshes (context);
  }

  void HandleContextMeshes (typename RenderTreeType::ContextNode& context)
  {
    CS::RenderManager::RenderView* rview = context.renderView;
    iShaderManager* shaderManager = rmanager->shaderManager;
    iSector* sector = rview->GetThisSector ();

    typename ShadowType::ShadowParameters shadowViewSetup (
      WrapShadowParams<ShadowType, RenderTreeType>::Create (
        context, rmanager->lightPersistent.shadowPersist, rview));
    
    // Sort the mesh lists  
    {
      StandardMeshSorter<RenderTreeType> mySorter (rview->GetEngine ());
      mySorter.SetupCameraLocation (rview->GetCamera ()->GetTransform ().GetOrigin ());
      ForEachMeshNode (context, mySorter);
    }

    // After sorting, assign in-context per-mesh indices
    {
      SingleMeshContextNumbering<RenderTreeType> numbering;
      ForEachMeshNode (context, numbering);
    }

    // Setup the SV arrays
    // Push the default stuff
    SetupStandardSVs (context, layerConfig, shaderManager, sector);

    // Setup the material&mesh SVs
    {
      StandardSVSetup<RenderTreeType, MultipleRenderLayer> svSetup (
        context.svArrays, layerConfig);

      ForEachMeshNode (context, svSetup);
    }

    SetupStandardShader (context, shaderManager, layerConfig);

    LightSetupType lightSetup (
      lightPersistent, rmanager->lightManager,
      context.svArrays, layerConfig, shadowViewSetup);

    ForEachMeshNode (context, lightSetup);

    // Setup shaders and tickets
    SetupStandardTicket (context, shaderManager,
      lightSetup.GetPostLightingLayers());
  
  
    RMShadowedPSSM::AutoFramebufferTexType fxFB (
      rmanager->framebufferTexPersistent);
    
    switch (rmanager->refrRefrShadows)
    {
      case 0:
	{
	  RMShadowedPSSM::ContextSetupType_Unshadowed ctxRefl (*this,
	   rmanager->lightPersistent_unshadowed,
	   rmanager->renderLayerReflect);
	  RMShadowedPSSM::ContextSetupType_Unshadowed ctxRefr (*this,
	   rmanager->lightPersistent_unshadowed,
	   rmanager->renderLayerRefract);
	  RMShadowedPSSM::AutoReflectRefractType_UU fxRR (
	    rmanager->reflectRefractPersistent, ctxRefl, ctxRefr);
	    
	  // Set up a functor that combines the AutoFX functors
	  typedef CS::Meta::CompositeFunctorType2<
	    RMShadowedPSSM::AutoReflectRefractType_UU,
	    RMShadowedPSSM::AutoFramebufferTexType> FXFunctor;
	  FXFunctor fxf (fxRR, fxFB);
	  
	  typedef TraverseUsedSVSets<RenderTreeType,
	    FXFunctor> SVTraverseType;
	  SVTraverseType svTraverser
	    (fxf, shaderManager->GetSVNameStringset ()->GetSize ());
	  // And do the iteration
	  ForEachMeshNode (context, svTraverser);
	}
        break;
      case RMShadowedPSSM::rrShadowReflect:
	{
	  RMShadowedPSSM::ContextSetupType ctxRefl (*this,
	   rmanager->lightPersistent,
	   rmanager->renderLayerReflect);
	  RMShadowedPSSM::ContextSetupType_Unshadowed ctxRefr (*this,
	   rmanager->lightPersistent_unshadowed,
	   rmanager->renderLayerRefract);
	  RMShadowedPSSM::AutoReflectRefractType_SU fxRR (
	    rmanager->reflectRefractPersistent, ctxRefl, ctxRefr);
	    
	  // Set up a functor that combines the AutoFX functors
	  typedef CS::Meta::CompositeFunctorType2<
	    RMShadowedPSSM::AutoReflectRefractType_SU,
	    RMShadowedPSSM::AutoFramebufferTexType> FXFunctor;
	  FXFunctor fxf (fxRR, fxFB);
	  
	  typedef TraverseUsedSVSets<RenderTreeType,
	    FXFunctor> SVTraverseType;
	  SVTraverseType svTraverser
	    (fxf, shaderManager->GetSVNameStringset ()->GetSize ());
	  // And do the iteration
	  ForEachMeshNode (context, svTraverser);
	}
        break;
      case RMShadowedPSSM::rrShadowRefract:
	{
	  RMShadowedPSSM::ContextSetupType_Unshadowed ctxRefl (*this,
	   rmanager->lightPersistent_unshadowed,
	   rmanager->renderLayerReflect);
	  RMShadowedPSSM::ContextSetupType ctxRefr (*this,
	   rmanager->lightPersistent,
	   rmanager->renderLayerRefract);
	  RMShadowedPSSM::AutoReflectRefractType_US fxRR (
	    rmanager->reflectRefractPersistent, ctxRefl, ctxRefr);
	    
	  // Set up a functor that combines the AutoFX functors
	  typedef CS::Meta::CompositeFunctorType2<
	    RMShadowedPSSM::AutoReflectRefractType_US,
	    RMShadowedPSSM::AutoFramebufferTexType> FXFunctor;
	  FXFunctor fxf (fxRR, fxFB);
	  
	  typedef TraverseUsedSVSets<RenderTreeType,
	    FXFunctor> SVTraverseType;
	  SVTraverseType svTraverser
	    (fxf, shaderManager->GetSVNameStringset ()->GetSize ());
	  // And do the iteration
	  ForEachMeshNode (context, svTraverser);
	}
        break;
      case RMShadowedPSSM::rrShadowReflect | RMShadowedPSSM::rrShadowRefract:
	{
	  RMShadowedPSSM::ContextSetupType ctxRefl (*this,
	   rmanager->lightPersistent,
	   rmanager->renderLayerReflect);
	  RMShadowedPSSM::ContextSetupType ctxRefr (*this,
	   rmanager->lightPersistent,
	   rmanager->renderLayerRefract);
	  RMShadowedPSSM::AutoReflectRefractType_SS fxRR (
	    rmanager->reflectRefractPersistent, ctxRefl, ctxRefr);
	    
	  // Set up a functor that combines the AutoFX functors
	  typedef CS::Meta::CompositeFunctorType2<
	    RMShadowedPSSM::AutoReflectRefractType_SS,
	    RMShadowedPSSM::AutoFramebufferTexType> FXFunctor;
	  FXFunctor fxf (fxRR, fxFB);
	  
	  typedef TraverseUsedSVSets<RenderTreeType,
	    FXFunctor> SVTraverseType;
	  SVTraverseType svTraverser
	    (fxf, shaderManager->GetSVNameStringset ()->GetSize (),
	     fxRR.svUserFlags | fxFB.svUserFlags);
	  // And do the iteration
	  ForEachMeshNode (context, svTraverser);
	}
        break;
    }
  }


  // Called by RMShadowedPSSM::AutoReflectRefractType
  void operator() (typename RenderTreeType::ContextNode& context)
  {
    typename PortalSetupType::ContextSetupData portalData (&context);

    operator() (context, portalData);
  }

public:
  RMShadowedPSSM* rmanager;
  typename LightSetupType::PersistentData& lightPersistent;
  const LayerConfigType& layerConfig;
  int recurseCount;
  int maxPortalRecurse;

  // Used to set up contexts split out by render grouping
  struct ContextSetupMeshes
  {
    StandardContextSetup& owner;

    ContextSetupMeshes (StandardContextSetup& owner) : owner (owner) {}

    void operator() (typename RenderTreeType::ContextNode& context)
    {
      owner.HandleContextMeshes (context);
    }
  };
};



RMShadowedPSSM::RMShadowedPSSM (iBase* parent)
  : scfImplementationType (this, parent), doHDRExposure (false), targets (*this)
{
  SetTreePersistent (treePersistent);
}

bool RMShadowedPSSM::RenderView (iView* view, bool recursePortals)
{
  // Setup a rendering view
  csRef<CS::RenderManager::RenderView> rview;
  rview = treePersistent.renderViews.GetRenderView (view);
  rview->SetOriginalCamera (view->GetCamera ());
  iGraphics3D* G3D = rview->GetGraphics3D ();
  int frameWidth = G3D->GetWidth ();
  int frameHeight = G3D->GetHeight ();
#include "csutil/deprecated_warn_off.h"
  view->GetCamera ()->SetViewportSize (frameWidth, frameHeight);
#include "csutil/deprecated_warn_on.h"

  view->GetEngine ()->UpdateNewFrame ();  
  view->GetEngine ()->FireStartFrame (rview);

  contextsScannedForTargets.Empty ();
  portalPersistent.UpdateNewFrame ();
  lightPersistent.UpdateNewFrame ();
  lightPersistent_unshadowed.UpdateNewFrame ();
  reflectRefractPersistent.UpdateNewFrame ();

  // Draw the background
  view->DrawBackground (G3D);

  iSector* startSector = rview->GetThisSector ();

  if (!startSector)
    return false;

  CS::Math::Matrix4 perspectiveFixup;
  PostEffectsSupport::SetupView (view, perspectiveFixup);

  // Pre-setup culling graph
  RenderTreeType renderTree (treePersistent);

  RenderTreeType::ContextNode* startContext = renderTree.CreateContext (rview);
  startContext->drawFlags |= CSDRAW_CLEARZBUFFER;
  startContext->renderTargets[rtaColor0].texHandle = PostEffectsSupport::GetScreenTarget ();
  startContext->renderTargets[rtaDepth].texHandle = PostEffectsSupport::GetDepthTarget ();
  startContext->perspectiveFixup = perspectiveFixup;

  // Setup the main context
  {
    ContextSetupType contextSetup (this, lightPersistent, renderLayer);
    ContextSetupType::PortalSetupType::ContextSetupData portalData (startContext);

    contextSetup (*startContext, portalData);
  
    targets.StartRendering (shaderManager);
    targets.EnqueueTargets (renderTree, shaderManager, renderLayer, contextsScannedForTargets);  
  }

  // Setup all dependent targets
  while (targets.HaveMoreTargets ())
  {
    TargetManagerType::TargetSettings ts;
    targets.GetNextTarget (ts);

    HandleTarget (renderTree, ts, recursePortals);
  }

  targets.FinishRendering ();
  
  // Render all contexts, back to front
  {
    view->GetContext()->SetZMode (CS_ZBUF_MESH);

    SimpleTreeRenderer<RenderTreeType> render (rview->GetGraphics3D (),
      shaderManager);
    ForEachContextReverse (renderTree, render);
  }

  PostEffectsSupport::DrawPostEffects (renderTree);
  
  
  if (doHDRExposure) hdrExposure.ApplyExposure (renderTree, view);
  
  DebugFrameRender (rview, renderTree);

  return true;
}

bool RMShadowedPSSM::RenderView (iView* view)
{
  return RenderView (view, true);
}

bool RMShadowedPSSM::PrecacheView (iView* view)
{
  if (!RenderView (view, false)) return false;

  //PostEffectsSupport::ClearIntermediates();

  /* @@@ Other ideas for precache drawing:
    - No frame advancement?
   */

  return true;
}

bool RMShadowedPSSM::HandleTarget (RenderTreeType& renderTree,
                                 const TargetManagerType::TargetSettings& settings,
                                 bool recursePortals)
{
  // Prepare
  csRef<CS::RenderManager::RenderView> rview;
  rview = treePersistent.renderViews.GetRenderView (settings.view);
  rview->SetOriginalCamera (settings.view->GetCamera ());

  iSector* startSector = rview->GetThisSector ();

  if (!startSector)
    return false;

  RenderTreeType::ContextNode* startContext = renderTree.CreateContext (rview);
  startContext->renderTargets[rtaColor0].texHandle = settings.target;
  startContext->renderTargets[rtaColor0].subtexture = settings.targetSubTexture;
  startContext->drawFlags = settings.drawFlags;

  ContextSetupType contextSetup (this, lightPersistent, renderLayer);
  ContextSetupType::PortalSetupType::ContextSetupData portalData (startContext);

  contextSetup (*startContext, portalData, recursePortals);
  
  targets.EnqueueTargets (renderTree, shaderManager, renderLayer, contextsScannedForTargets);

  return true;
}

bool RMShadowedPSSM::Initialize(iObjectRegistry* objectReg)
{
  const char messageID[] = "crystalspace.rendermanager.shadow_pssm";
  
  this->objectReg = objectReg;

  svNameStringSet = csQueryRegistryTagInterface<iShaderVarStringSet> (objectReg,
    "crystalspace.shader.variablenameset");

  stringSet = csQueryRegistryTagInterface<iStringSet> (objectReg,
    "crystalspace.shared.stringset");
    
  shaderManager = csQueryRegistry<iShaderManager> (objectReg);
  lightManager = csQueryRegistry<iLightManager> (objectReg);
  
  csRef<iVerbosityManager> verbosity = csQueryRegistry<iVerbosityManager> (
    objectReg);
  bool doVerbose = verbosity && verbosity->Enabled ("rendermanager.shadow_pssm");
  
  csConfigAccess cfg (objectReg);
  bool layersValid = false;
  const char* layersFile = cfg->GetStr ("RenderManager.ShadowPSSM.Layers", 0);
  if (layersFile)
  {
    if (doVerbose)
      csReport (objectReg, CS_REPORTER_SEVERITY_NOTIFY, messageID,
	"Reading render layers from %s", CS::Quote::Single (layersFile));
    layersValid = CS::RenderManager::AddLayersFromFile (objectReg, layersFile,
      renderLayer);
    if (!layersValid) renderLayer.Clear();
  }
  layersFile = cfg->GetStr (
    "RenderManager.ShadowPSSM.Layers.Reflections", 0);
  if (layersFile)
  {
    if (doVerbose)
      csReport (objectReg, CS_REPORTER_SEVERITY_NOTIFY, messageID,
	"Reading reflection render layers from %s",
	CS::Quote::Single (layersFile));
    layersValid = CS::RenderManager::AddLayersFromFile (objectReg, layersFile,
      renderLayerReflect);
    if (!layersValid) renderLayerReflect = renderLayer;
  }
  else
    renderLayerReflect = renderLayer;
  layersFile = cfg->GetStr (
    "RenderManager.ShadowPSSM.Layers.Refractions", 0);
  if (layersFile)
  {
    if (doVerbose)
      csReport (objectReg, CS_REPORTER_SEVERITY_NOTIFY, messageID,
	"Reading refraction render layers from %s",
	CS::Quote::Single (layersFile));
    layersValid = CS::RenderManager::AddLayersFromFile (objectReg, layersFile,
      renderLayerRefract);
    if (!layersValid) renderLayerRefract = renderLayer;
  }
  else
    renderLayerRefract = renderLayer;
  
  csRef<iLoader> loader (csQueryRegistry<iLoader> (objectReg));
  if (!layersValid)
  {
    if (!loader->LoadShader ("/shader/lighting/lighting_default.xml").IsValid())
    {
      csReport (objectReg, CS_REPORTER_SEVERITY_WARNING,
	messageID, "Could not load lighting_default shader");
    }
  
    if (doVerbose)
      csReport (objectReg, CS_REPORTER_SEVERITY_NOTIFY, messageID,
	"Using default render layers");
    CS::RenderManager::AddDefaultBaseLayers (objectReg, renderLayer);
  }
  
  maxPortalRecurse = cfg->GetInt("RenderManager.ShadowPSSM.MaxPortalRecurse", 30);
  
  csRef<iGraphics3D> g3d = csQueryRegistry<iGraphics3D> (objectReg);
  treePersistent.Initialize (shaderManager);
  dbgFlagClipPlanes =
    treePersistent.debugPersist.RegisterDebugFlag ("draw.clipplanes.view");
  PostEffectsSupport::Initialize (objectReg, "RenderManager.ShadowPSSM");
  
  HDRSettings hdrSettings (cfg, "RenderManager.ShadowPSSM");
  if (hdrSettings.IsEnabled())
  {
    doHDRExposure = true;
    
    hdr.Setup (objectReg, 
      hdrSettings.GetQuality(), 
      hdrSettings.GetColorRange(),
	  this);
    hdrExposure.Initialize (objectReg, hdr, hdrSettings);
  }
  
  portalPersistent.Initialize (objectReg, treePersistent.debugPersist);
  lightPersistent.shadowPersist.SetConfigPrefix ("RenderManager.ShadowPSSM");
  lightPersistent.Initialize (objectReg, treePersistent.debugPersist);
  lightPersistent_unshadowed.Initialize (objectReg, treePersistent.debugPersist);
  reflectRefractPersistent.Initialize (objectReg, treePersistent.debugPersist,
    this);
  framebufferTexPersistent.Initialize (objectReg,
    this);
    
  refrRefrShadows = 0;
  if (cfg->GetBool ("RenderManager.ShadowPSSM.ShadowsInReflections", true))
    refrRefrShadows |= rrShadowReflect;
  if (cfg->GetBool ("RenderManager.ShadowPSSM.ShadowsInRefractions", true))
    refrRefrShadows |= rrShadowRefract;
  
  RMViscullCommon::Initialize (objectReg, "RenderManager.ShadowPSSM");
  
  return true;
}

}
CS_PLUGIN_NAMESPACE_END(RMShadowedPSSM)
