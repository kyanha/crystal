/*
    Copyright (C) 2010 by Joe Forte

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

#include "csplugincommon/rendermanager/renderview.h"
#include "csplugincommon/rendermanager/dependenttarget.h"
#include "csplugincommon/rendermanager/hdrhelper.h"
#include "csplugincommon/rendermanager/lightsetup.h"
#include "csplugincommon/rendermanager/occluvis.h"
#include "csplugincommon/rendermanager/operations.h"
#include "csplugincommon/rendermanager/portalsetup.h"
#include "csplugincommon/rendermanager/posteffects.h"
#include "csplugincommon/rendermanager/render.h"
#include "csplugincommon/rendermanager/renderlayers.h"
#include "csplugincommon/rendermanager/rendertree.h"
#include "csplugincommon/rendermanager/shadersetup.h"
#include "csplugincommon/rendermanager/standardsorter.h"
#include "csplugincommon/rendermanager/svsetup.h"
#include "csplugincommon/rendermanager/viscull.h"

#include "iengine.h"
#include "ivideo.h"
#include "ivaria/reporter.h"
#include "csutil/cfgacc.h"

#include "deferredoperations.h"
#include "deferredshadersetup.h"
#include "deferredrender.h"
#include "deferred.h"

using namespace CS::RenderManager;

CS_PLUGIN_NAMESPACE_BEGIN(RMDeferred)
{

SCF_IMPLEMENT_FACTORY(RMDeferred);

//----------------------------------------------------------------------
template<typename RenderTreeType, typename LayerConfigType>
class StandardContextSetup
{
public:
  typedef StandardContextSetup<RenderTreeType, LayerConfigType> ThisType;
  typedef StandardPortalSetup<RenderTreeType, ThisType> PortalSetupType;

  StandardContextSetup(RMDeferred *rmanager, const LayerConfigType &layerConfig, GBuffer* gbuffer)
    : 
  rmanager(rmanager),
  gbuffer(gbuffer), 
  layerConfig(layerConfig),
  recurseCount(0), 
  deferredLayer(rmanager->deferredLayer),
  lightingLayer(rmanager->lightingLayer),
  zonlyLayer(rmanager->zonlyLayer),
  maxPortalRecurse(rmanager->maxPortalRecurse)
  {}

  StandardContextSetup (const StandardContextSetup &other, const LayerConfigType &layerConfig)
    :
  rmanager(other.rmanager), 
  gbuffer(other.gbuffer),
  layerConfig(layerConfig),
  recurseCount(other.recurseCount),
  deferredLayer(other.deferredLayer),
  lightingLayer(other.lightingLayer),
  zonlyLayer(other.zonlyLayer),
  maxPortalRecurse(other.maxPortalRecurse)
  {}

  void operator()(typename RenderTreeType::ContextNode &context, 
    typename PortalSetupType::ContextSetupData &portalSetupData,
    bool recursePortals = true)
  {
    // set custom gbuffer if there is one
    context.gbuffer = gbuffer;

    CS::RenderManager::RenderView* rview = context.renderView;
    iSector* sector = rview->GetThisSector ();

    if (recurseCount > maxPortalRecurse) return;
    
    iShaderManager* shaderManager = rmanager->shaderManager;

    // @@@ This is somewhat "boilerplate" sector/rview setup.
    sector->PrepareDraw (rview);
    // Make sure the clip-planes are ok
    CS::RenderViewClipper::SetupClipPlanes (rview->GetRenderContext ());

    // Do the culling
    iVisibilityCuller *culler = sector->GetVisibilityCuller ();
    Viscull<RenderTreeType> (context, rview, culler);

    // Set up all portals
    if (recursePortals)
    {
      recurseCount++;
      PortalSetupType portalSetup (rmanager->portalPersistent, *this);      
      portalSetup (context, portalSetupData);
      recurseCount--;
    }
    
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
      StandardSVSetup<RenderTreeType, MultipleRenderLayer> svSetup (context.svArrays, layerConfig);
      ForEachMeshNode (context, svSetup);
    }

    // Setup shaders and tickets
    DeferredSetupShader (context, shaderManager, layerConfig, deferredLayer, lightingLayer, zonlyLayer);

    // Setup lighting (only needed for transparent objects)
    RMDeferred::LightSetupType::ShadowParamType shadowParam;
    RMDeferred::LightSetupType lightSetup (rmanager->lightPersistent, 
                                           rmanager->lightManager,
                                           context.svArrays, 
                                           layerConfig, 
                                           shadowParam);

    ForEachForwardMeshNode (context, lightSetup);

    // Setup shaders and tickets
    SetupStandardTicket (context, shaderManager, lightSetup.GetPostLightingLayers ());

    {
      ThisType ctxRefl (*this, layerConfig);
      ThisType ctxRefr (*this, layerConfig);
      RMDeferred::AutoReflectRefractType fxRR (
        rmanager->reflectRefractPersistent, ctxRefl, ctxRefr);
        
      RMDeferred::AutoFramebufferTexType fxFB (
        rmanager->framebufferTexPersistent);
      
      // Set up a functor that combines the AutoFX functors
      typedef CS::Meta::CompositeFunctorType2<
        RMDeferred::AutoReflectRefractType,
        RMDeferred::AutoFramebufferTexType> FXFunctor;
      FXFunctor fxf (fxRR, fxFB);
      
      typedef TraverseUsedSVSets<RenderTreeType,
        FXFunctor> SVTraverseType;
      SVTraverseType svTraverser
        (fxf, shaderManager->GetSVNameStringset ()->GetSize (),
	 fxRR.svUserFlags | fxFB.svUserFlags);
      // And do the iteration
      ForEachMeshNode (context, svTraverser);
    }
  }

  // Called by AutoReflectRefractType
  void operator() (typename RenderTreeType::ContextNode& context)
  {
    typename PortalSetupType::ContextSetupData portalData (&context);

    operator() (context, portalData);
  }

private:

  RMDeferred *rmanager;
  csRef<GBuffer> gbuffer;

  const LayerConfigType &layerConfig;

  int recurseCount;
  int deferredLayer;
  int lightingLayer;
  int zonlyLayer;
  int maxPortalRecurse;
};

//----------------------------------------------------------------------
RMDeferred::RMDeferred(iBase *parent) 
  : 
scfImplementationType(this, parent),
portalPersistent(CS::RenderManager::TextureCache::tcacheExactSizeMatch),
doHDRExposure (false),
targets (*this)
{
  SetTreePersistent (treePersistent);
}

//----------------------------------------------------------------------
bool RMDeferred::Initialize(iObjectRegistry *registry)
{
  const char *messageID = "crystalspace.rendermanager.deferred";

  objRegistry = registry;

  csRef<iGraphics3D> graphics3D = csQueryRegistry<iGraphics3D> (objRegistry);
  iGraphics2D *graphics2D = graphics3D->GetDriver2D ();
   
  shaderManager = csQueryRegistry<iShaderManager> (objRegistry);
  lightManager = csQueryRegistry<iLightManager> (objRegistry);
  stringSet = csQueryRegistryTagInterface<iStringSet> (objRegistry, "crystalspace.shared.stringset");

  // Initialize the extra data in the persistent tree data.
  RenderTreeType::TreeTraitsType::Initialize (treePersistent, registry);
  
  // Read Config settings.
  csConfigAccess cfg (objRegistry);
  maxPortalRecurse = cfg->GetInt ("RenderManager.Deferred.MaxPortalRecurse", 30);
  showGBuffer = false;
  drawLightVolumes = false;

  // load render layers
  bool layersValid = false;
  const char *layersFile = cfg->GetStr ("RenderManager.Deferred.Layers", nullptr);
  if (layersFile)
  {
    csReport (objRegistry, CS_REPORTER_SEVERITY_NOTIFY, messageID, 
      "Reading render layers from %s", CS::Quote::Single (layersFile));

    layersValid = CS::RenderManager::AddLayersFromFile (objRegistry, layersFile, renderLayer);
    
    if (!layersValid) 
    {
      renderLayer.Clear();
    }
    else
    {
      // Locates the deferred shading layer.
      deferredLayer = LocateLayer (renderLayer, stringSet->Request("gbuffer fill"));
      if (deferredLayer < 0)
      {
        csReport (objRegistry, CS_REPORTER_SEVERITY_WARNING,
          messageID, "The render layers file %s does not contain a %s layer.",
	  CS::Quote::Single (layersFile),
	  CS::Quote::Single ("gbuffer fill"));

        AddDeferredLayer (renderLayer, deferredLayer);
      }

      // locate the final lighting layer if there's any
      lightingLayer = LocateLayer (renderLayer, stringSet->Request("gbuffer use"));
      csReport (objRegistry, CS_REPORTER_SEVERITY_NOTIFY,
        messageID, "Using deferred %s.",
	(lightingLayer < 0) ? "shading" : "lighting");

      // Locates the zonly shading layer.
      zonlyLayer = LocateLayer (renderLayer, stringSet->Request("depthwrite"));
    }
  }
  
  csRef<iLoader> loader = csQueryRegistry<iLoader> (objRegistry);
  if (!layersValid)
  {
    csReport (objRegistry, CS_REPORTER_SEVERITY_NOTIFY, messageID,
      "Using default render layers");

    AddZOnlyLayer (renderLayer, zonlyLayer);
    AddDeferredLayer (renderLayer, deferredLayer);

    if (!loader->LoadShader ("/shader/lighting/lighting_default.xml"))
    {
      csReport (objRegistry, CS_REPORTER_SEVERITY_WARNING,
        messageID, "Could not load lighting_default shader");
    }

    CS::RenderManager::AddDefaultBaseLayers (objRegistry, renderLayer);
  }

  // Create GBuffer
  const char *gbufferFmt = cfg->GetStr ("RenderManager.Deferred.GBuffer.BufferFormat", "rgba16_f");
  const char *accumFmt = cfg->GetStr ("RenderManager.Deferred.GBuffer.AccumBufferFormat", "rgb16_f");
  int bufferCount = cfg->GetInt ("RenderManager.Deferred.GBuffer.BufferCount", 1);
  int accumCount = cfg->GetInt ("RenderManager.Deferred.GBuffer.AccumBufferCount", 2);

  gbufferDescription.colorBufferCount = bufferCount;
  gbufferDescription.accumBufferCount = accumCount;
  gbufferDescription.width = graphics2D->GetWidth ();
  gbufferDescription.height = graphics2D->GetHeight ();
  gbufferDescription.colorBufferFormat = gbufferFmt;
  gbufferDescription.accumBufferFormat = accumFmt;

  if (!gbuffer.Initialize (gbufferDescription, 
                           graphics3D, 
                           shaderManager->GetSVNameStringset (), 
                           objRegistry))
  {
    return false;
  }

  treePersistent.Initialize (shaderManager);
  portalPersistent.Initialize (shaderManager, graphics3D, treePersistent.debugPersist);
  lightPersistent.Initialize (registry, treePersistent.debugPersist);
  lightRenderPersistent.Initialize (registry);

  // Fix portal texture cache to only query textures that match the gbuffer dimensions.
  portalPersistent.fixedTexCacheWidth = gbufferDescription.width;
  portalPersistent.fixedTexCacheHeight = gbufferDescription.height;

  // Make sure the texture cache creates matching texture buffers.
  portalPersistent.texCache.SetFormat (accumFmt);
  portalPersistent.texCache.SetFlags (CS_TEXTURE_2D | CS_TEXTURE_NOMIPMAPS | CS_TEXTURE_CLAMP | CS_TEXTURE_NPOTS);

  // initialize post-effects
  PostEffectsSupport::Initialize (registry, "RenderManager.Deferred");

  // initialize hdr
  HDRSettings hdrSettings (cfg, "RenderManager.Deferred");
  if (hdrSettings.IsEnabled())
  {
    doHDRExposure = true;
    
    hdr.Setup (registry, 
      hdrSettings.GetQuality(), 
      hdrSettings.GetColorRange());
    postEffects.SetChainedOutput (hdr.GetHDRPostEffects());
  
    hdrExposure.Initialize (registry, hdr, hdrSettings);
  }

  // initialize reflect/refract
  dbgFlagClipPlanes = treePersistent.debugPersist.RegisterDebugFlag ("draw.clipplanes.view");
  reflectRefractPersistent.Initialize (registry, treePersistent.debugPersist, &postEffects);
  framebufferTexPersistent.Initialize (registry, &postEffects);

  RMViscullCommon::Initialize (objRegistry, "RenderManager.Deferred");
  
  return true;
}

//----------------------------------------------------------------------
bool RMDeferred::RenderView(iView *view, bool recursePortals)
{
  iGraphics3D *graphics3D = view->GetContext ();

  view->UpdateClipper ();

  int frameWidth = graphics3D->GetWidth ();
  int frameHeight = graphics3D->GetHeight ();
  view->GetCamera ()->SetViewportSize (frameWidth, frameHeight);

  // Setup renderview
  csRef<CS::RenderManager::RenderView> rview;
  rview = treePersistent.renderViews.GetRenderView (view);
  rview->SetOriginalCamera (view->GetCamera ());

  // Computes the left, right, top, and bottom of the view frustum.
  iPerspectiveCamera *camera = view->GetPerspectiveCamera ();
  float invFov = camera->GetInvFOV ();
  float l = -invFov * camera->GetShiftX ();
  float r =  invFov * (frameWidth - camera->GetShiftX ());
  float t = -invFov * camera->GetShiftY ();
  float b =  invFov * (frameHeight - camera->GetShiftY ());
  rview->SetFrustum (l, r, t, b);

  contextsScannedForTargets.Empty ();
  portalPersistent.UpdateNewFrame ();
  lightPersistent.UpdateNewFrame ();
  reflectRefractPersistent.UpdateNewFrame ();
  framebufferTexPersistent.UpdateNewFrame ();

  iEngine *engine = view->GetEngine ();
  engine->UpdateNewFrame ();  
  engine->FireStartFrame (rview);

  iSector *startSector = rview->GetThisSector ();
  if (!startSector)
    return false;

  RenderTreeType renderTree (treePersistent);
  RenderTreeType::ContextNode *startContext = renderTree.CreateContext (rview);
  startContext->drawFlags |= CSDRAW_CLEARSCREEN;

  // Add gbuffer textures to be visualized.
  if (showGBuffer)
    ShowGBuffer (renderTree, &gbuffer);

  CS::Math::Matrix4 perspectiveFixup;
  postEffects.SetupView (view, perspectiveFixup);

  startContext->renderTargets[rtaColor0].texHandle = postEffects.GetScreenTarget ();
  startContext->perspectiveFixup = perspectiveFixup;

  // Setup the main context
  {
    ContextSetupType contextSetup (this, renderLayer, &gbuffer);
    ContextSetupType::PortalSetupType::ContextSetupData portalData (startContext);

    contextSetup (*startContext, portalData, recursePortals);

    targets.StartRendering (shaderManager);
    targets.EnqueueTargets (renderTree, shaderManager, renderLayer, contextsScannedForTargets);
  }

  // Setup all dependent targets
  while (targets.HaveMoreTargets ())
  {
    TargetManagerType::TargetSettings ts;
    targets.GetNextTarget (ts);

    HandleTarget (renderTree, ts, recursePortals, graphics3D);
  }

  targets.FinishRendering ();

  // Render all contexts.
  {
    DeferredTreeRenderer<RenderTreeType> render (graphics3D,
                                                 shaderManager,
                                                 stringSet,
                                                 lightRenderPersistent,
                                                 deferredLayer,
						 lightingLayer,
                                                 zonlyLayer,
                                                 drawLightVolumes);

    ForEachContextReverse (renderTree, render);

    // clear clipper
    graphics3D->SetClipper (nullptr, CS_CLIPPER_TOPLEVEL);
  }

  postEffects.DrawPostEffects (renderTree);

  if (doHDRExposure) hdrExposure.ApplyExposure (renderTree, view);

  DebugFrameRender (rview, renderTree);

  return true;
}

bool RMDeferred::RenderView(iView *view)
{
  return RenderView (view, true);
}

//----------------------------------------------------------------------
bool RMDeferred::PrecacheView(iView *view)
{
  return RenderView (view, false);

  postEffects.ClearIntermediates ();
  hdr.GetHDRPostEffects().ClearIntermediates();
}

bool RMDeferred::HandleTarget (RenderTreeType& renderTree,
                               const TargetManagerType::TargetSettings& settings,
                               bool recursePortals, iGraphics3D* g3d)
{
  // Prepare
  csRef<CS::RenderManager::RenderView> rview;
  rview = treePersistent.renderViews.GetRenderView (settings.view);
  iCamera* c = settings.view->GetCamera ();
  rview->SetOriginalCamera (c);

  iSector* startSector = rview->GetThisSector ();

  if (!startSector)
    return false;

  RenderTreeType::ContextNode* startContext = renderTree.CreateContext (rview);
  startContext->renderTargets[rtaColor0].texHandle = settings.target;
  startContext->renderTargets[rtaColor0].subtexture = settings.targetSubTexture;
  startContext->drawFlags = settings.drawFlags;

  settings.target->GetRendererDimensions(gbufferDescription.width, gbufferDescription.height);

  csRef<GBuffer> buffer;
  buffer.AttachNew(new GBuffer);

  if (!buffer->Initialize (gbufferDescription, 
                           g3d, 
                           shaderManager->GetSVNameStringset (), 
                           objRegistry))
  {
    return false;
  }

  if(showGBuffer)
    ShowGBuffer(renderTree, buffer);

  ContextSetupType contextSetup (this, renderLayer, buffer);
  ContextSetupType::PortalSetupType::ContextSetupData portalData (startContext);

  contextSetup (*startContext, portalData, recursePortals);
  
  targets.EnqueueTargets (renderTree, shaderManager, renderLayer, contextsScannedForTargets);

  return true;
}

//----------------------------------------------------------------------
void RMDeferred::AddDeferredLayer(CS::RenderManager::MultipleRenderLayer &layers, int &addedLayer)
{
  const char *messageID = "crystalspace.rendermanager.deferred";

  csRef<iLoader> loader = csQueryRegistry<iLoader> (objRegistry);

  if (!loader->LoadShader ("/shader/deferred/fill_gbuffer.xml"))
  {
    csReport (objRegistry, CS_REPORTER_SEVERITY_WARNING,
      messageID, "Could not load fill_gbuffer shader");
  }

  iShader *shader = shaderManager->GetShader ("fill_gbuffer");

  SingleRenderLayer baseLayer (shader, 0, 0);
  baseLayer.AddShaderType (stringSet->Request("gbuffer fill"));

  renderLayer.AddLayers (baseLayer);

  addedLayer = renderLayer.GetLayerCount () - 1;
}

//----------------------------------------------------------------------
void RMDeferred::AddZOnlyLayer(CS::RenderManager::MultipleRenderLayer &layers, int &addedLayer)
{
  const char *messageID = "crystalspace.rendermanager.deferred";

  csRef<iLoader> loader = csQueryRegistry<iLoader> (objRegistry);

  if (!loader->LoadShader ("/shader/early_z/z_only.xml"))
  {
    csReport (objRegistry, CS_REPORTER_SEVERITY_WARNING,
      messageID, "Could not load z_only shader");
  }

  iShader *shader = shaderManager->GetShader ("z_only");

  SingleRenderLayer baseLayer (shader, 0, 0);
  baseLayer.AddShaderType (stringSet->Request("depthwrite"));

  renderLayer.AddLayers (baseLayer);

  addedLayer = renderLayer.GetLayerCount () - 1;
}

//----------------------------------------------------------------------
int RMDeferred::LocateLayer(const CS::RenderManager::MultipleRenderLayer &layers,
                            csStringID shaderType)
{
  size_t count = renderLayer.GetLayerCount ();
  for (size_t i = 0; i < count; i++)
  {
    size_t num;
    const csStringID *strID = renderLayer.GetShaderTypes (i, num);
    for (size_t j = 0; j < num; j++)
    {
      if (strID[j] == shaderType)
      {
        return i;
      }
    }
  }

  return -1;
}

//----------------------------------------------------------------------
void RMDeferred::ShowGBuffer(RenderTreeType &tree, GBuffer* buffer)
{
  int w, h;
  buffer->GetColorBuffer (0)->GetRendererDimensions (w, h);
  float aspect = (float)w / h;

  for (size_t i = 0; i < buffer->GetColorBufferCount(); i++)
  {
    tree.AddDebugTexture (buffer->GetColorBuffer (i), aspect);
  }

  tree.AddDebugTexture (buffer->GetDepthBuffer (), aspect);

  for (size_t i = 0; i < buffer->GetAccumulationBufferCount(); ++i)
  {
    tree.AddDebugTexture (buffer->GetAccumulationBuffer(i), aspect);
  }
}

//----------------------------------------------------------------------
bool RMDeferred::DebugCommand(const char *cmd)
{
  if (strcmp (cmd, "toggle_visualize_gbuffer") == 0)
  {
    showGBuffer = !showGBuffer;
    return true;
  }
  else if (strcmp (cmd, "toggle_visualize_lightvolumes") == 0)
  {
    drawLightVolumes = !drawLightVolumes;
    return true;
  }

  return false;
}

}
CS_PLUGIN_NAMESPACE_END(RMDeferred)
