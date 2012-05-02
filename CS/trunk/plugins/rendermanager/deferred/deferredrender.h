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

#ifndef __DEFERREDRENDER_H__
#define __DEFERREDRENDER_H__

#include "cssysdef.h"

#include "csplugincommon/rendermanager/render.h"
#include "csplugincommon/rendermanager/rendertree.h"

#include "deferredlightrender.h"
#include "deferredoperations.h"
#include "gbuffer.h"

CS_PLUGIN_NAMESPACE_BEGIN(RMDeferred)
{
  /**
   * Renderer for multiple contexts where all forward rendering objects are drawn.
   */
  template<typename RenderTree>
  class ForwardMeshTreeRenderer
  {
  public:

    ForwardMeshTreeRenderer(iGraphics3D* g3d, iShaderManager *shaderMgr, int deferredLayer, int lightingLayer, int zonlyLayer)
      : 
    meshRender(g3d, shaderMgr),
    deferredLayer(deferredLayer),
    lightingLayer(lightingLayer),
    zonlyLayer(zonlyLayer)
    {}

    ~ForwardMeshTreeRenderer() {}

    /**
     * Render single context.
     */
    void operator()(typename RenderTree::ContextNode *context)
    {
      size_t layerCount = context->svArrays.GetNumLayers ();
      for (size_t layer = 0; layer < layerCount; ++layer)
      {
        if ((int)layer == deferredLayer || (int)layer == lightingLayer
	  || (int)layer == zonlyLayer)
          continue;

        meshRender.SetLayer (layer);
        ForEachForwardMeshNode (*context, meshRender);
      }
    }
   
  private:

    CS::RenderManager::SimpleContextRender<RenderTree> meshRender;

    int deferredLayer;
    int lightingLayer;
    int zonlyLayer;
  };

  /**
   * Deferred renderer for multiple contexts.
   *
   * Example:
   * \code
   * // ... contexts setup etc. ...
   *
   * {
   *   DeferredTreeRenderer<RenderTree> 
   *     render (graphics3D, shaderManager, stringSet, 
   *             lightRenderPersistent, deferredLayer, 
   *             zonlyLayer, drawLightVolumes);
   *
   *   ForEachContextReverse (renderTree, render);
   * }
   *
   * // ... apply post processing ...
   * \endcode
   */
  template<typename RenderTree>
  class DeferredTreeRenderer
  {
  public:

    DeferredTreeRenderer(iGraphics3D *g3d, 
                         iShaderManager *shaderMgr,
                         iStringSet *stringSet,
                         DeferredLightRenderer::PersistentData &lightRenderPersistent,
			 GBuffer& gbuffer,
                         int deferredLayer,
			 int lightingLayer,
                         int zonlyLayer,
                         bool drawLightVolumes)
      : 
    meshRender(g3d, shaderMgr),
    graphics3D(g3d),
    shaderMgr(shaderMgr),
    stringSet(stringSet),
    lightRenderPersistent(lightRenderPersistent),
    gbuffer(gbuffer),
    deferredLayer(deferredLayer),
    lightingLayer(lightingLayer),
    zonlyLayer(zonlyLayer),
    lastAccumBuf(nullptr),
    lastSubTex(-1),
    lastRenderView(nullptr),
    drawLightVolumes(drawLightVolumes)
    {}

    ~DeferredTreeRenderer() 
    {
      RenderContextStack ();
    }

    /**
     * Render all contexts.
     */
    void operator()(typename RenderTree::ContextNode *context)
    {
      if (IsNew (context))
      {
        // New context, render out the old ones
        RenderContextStack ();

        lastAccumBuf = context->renderTargets[rtaColor0].texHandle;
        lastSubTex = context->renderTargets[rtaColor0].subtexture;
        lastRenderView = context->renderView;
      }
      
      contextStack.Push (context);
    }

  protected:

    void RenderContextStack()
    {
      const size_t ctxCount = contextStack.GetSize ();

      if (ctxCount == 0)
        return;
      
      typename RenderTree::ContextNode *context = contextStack[0];

      // obtain some variables we'll need
      CS::RenderManager::RenderView *rview = context->renderView;
      iCamera *cam = rview->GetCamera ();

      // seriously, we need those
      CS_ASSERT(rview);
      CS_ASSERT(cam);

      // filter out zbuffer draw flags - they'll most certainly break the rendering
      context->drawFlags &= ~CSDRAW_CLEARZBUFFER;

      // create the light render here as we'll use it a lot
      DeferredLightRenderer lightRender (graphics3D,
                                         shaderMgr,
                                         stringSet,
                                         rview,
                                         gbuffer,
                                         lightRenderPersistent);

      // check whether we're rendering off-screen (and hence have flipped y in the end)
      iTextureHandle* target = nullptr;
      for(int i = 0; i < rtaNumAttachments; ++i)
      {
	if(context->renderTargets[i].texHandle != (iTextureHandle*)nullptr)
	{
	  target = context->renderTargets[i].texHandle;
	  break;
	}
      }

      // set tex scale to default
      lightRenderPersistent.scale->SetValue(csVector4(0.5,0.5,0.5,0.5));

      // shared setup for all passes - projection only
      CS::Math::Matrix4 projMatrix = context->perspectiveFixup * cam->GetProjectionMatrix();

      // shared setup for deferred passes
      graphics3D->SetProjectionMatrix (context->gbufferFixup * projMatrix);

      // Fill the gbuffer
      gbuffer.Attach ();
      {
	// setup clipper
	graphics3D->SetClipper (rview->GetClipper(), CS_CLIPPER_TOPLEVEL);

        int drawFlags = CSDRAW_3DGRAPHICS | context->drawFlags;
        drawFlags |= CSDRAW_CLEARSCREEN | CSDRAW_CLEARZBUFFER;

        CS::RenderManager::BeginFinishDrawScope bd(graphics3D, drawFlags);

	// we want to fill the depth buffer, use pass 1 modes
        graphics3D->SetZMode (CS_ZBUF_MESH);

        // z only pass - maybe we shouldn't allow disabling it.
	if(zonlyLayer >= 0)
        {
          meshRender.SetLayer (zonlyLayer);

          for (size_t i = 0; i < ctxCount; i++)
          {
            typename RenderTree::ContextNode *ctx = contextStack[i];

	    graphics3D->SetWorldToCamera (ctx->cameraTransform.GetInverse ());

	    // for the depth buffer we want *all* meshes, not just deferred ones
            ForEachMeshNode (*ctx, meshRender);
          }

	  // zbuffer is already filled, use pass 2 modes
	  graphics3D->SetZMode (CS_ZBUF_MESH2);
        }

	// deferred pass
        // @@@TODO: we could check for CS_ENTITY_NOLIGHTING here
        //          and use it to fill the stencil buffer so those
        //          meshes can be skipped during the lighting pass
	{
	  meshRender.SetLayer (deferredLayer);

	  for (size_t i = 0; i < ctxCount; i++)
	  {
            typename RenderTree::ContextNode *ctx = contextStack[i];
          
	    graphics3D->SetWorldToCamera (ctx->cameraTransform.GetInverse ());

	    // only deferred ones contribute to the gbuffer
	    ForEachDeferredMeshNode (*ctx, meshRender);
	  }
	}


	// clear clipper
	graphics3D->SetClipper (nullptr, CS_CLIPPER_TOPLEVEL);
      }

      // Fill the accumulation buffers
      gbuffer.AttachAccumulation(); // attach accumulation buffers
      {
	graphics3D->SetClipper(rview->GetClipper(), CS_CLIPPER_TOPLEVEL);

        int drawFlags = CSDRAW_3DGRAPHICS | context->drawFlags;
	drawFlags |= CSDRAW_CLEARSCREEN;

	CS::RenderManager::BeginFinishDrawScope bd (graphics3D, drawFlags);

	// use pass 1 zmodes for re-populating the zbuffer
	graphics3D->SetZMode (CS_ZBUF_MESH);

	// we use the inaccurate version from our gbuffer here - it's sufficient
        // @@@NOTE: appearently using an explicit depth output prevents hardware
        //          optimizations for the zbuffer to kick in - maybe we should
	//	    just go for a fragkill solution?
	lightRender.OutputDepth();

	// we're done with all depth writes - use pass 2 modes
	graphics3D->SetZMode (CS_ZBUF_MESH2);

	// accumulate lighting data
        for (size_t i = 0; i < ctxCount; i++)
        {
          typename RenderTree::ContextNode *ctx = contextStack[i];

	  graphics3D->SetWorldToCamera (ctx->cameraTransform.GetInverse ());

          // other light types
          ForEachLight (*ctx, lightRender);
        }

	// clear clipper
	graphics3D->SetClipper (nullptr, CS_CLIPPER_TOPLEVEL);
      }

      // setup projection matrix for final pass
      graphics3D->SetProjectionMatrix (projMatrix);

      // attach output render targets if any.
      if(target)
      {
	for (int a = 0; a < rtaNumAttachments; a++)
	  graphics3D->SetRenderTarget (context->renderTargets[a].texHandle, false,
	      context->renderTargets[a].subtexture, csRenderTargetAttachment (a));
	CS_ASSERT(graphics3D->ValidateRenderTargets ());
      }
      {
	// setup clipper
	graphics3D->SetClipper (rview->GetClipper(), CS_CLIPPER_TOPLEVEL);

        int drawFlags = CSDRAW_3DGRAPHICS | context->drawFlags;
	drawFlags |= CSDRAW_CLEARSCREEN;

        CS::RenderManager::BeginFinishDrawScope bd (graphics3D, drawFlags);

	// set tex scale for lookups.
	lightRenderPersistent.scale->SetValue(context->texScale);

	// we want to re-populate the depth buffer, use pass 1 modes.
	graphics3D->SetZMode (CS_ZBUF_MESH);

	// Visibility Culling
	{
	  // visculling only needs to be rendered once per sector.
	  csSet<iSector*> sectors;
	  for (size_t c = 0; c < contextStack.GetSize (); ++c)
	  {
	    typename RenderTree::ContextNode* ctx = contextStack[c];

	    if (!sectors.Contains(ctx->sector))
	    {
	      sectors.AddNoTest(ctx->sector);

              graphics3D->SetWorldToCamera (ctx->cameraTransform.GetInverse ());
	      ctx->sector->GetVisibilityCuller ()->RenderViscull (rview, ctx->shadervars);
	    }
	  }
	}

	// early z pass - this one could be disabled if occluvis is used - but how would we know?
	if(zonlyLayer >= 0)
        {
          meshRender.SetLayer (zonlyLayer);

          for (size_t i = 0; i < ctxCount; i++)
          {
            typename RenderTree::ContextNode *ctx = contextStack[i];

	    graphics3D->SetWorldToCamera (ctx->cameraTransform.GetInverse ());

	    // for the depth buffer we want *all* meshes, not just deferred ones
            ForEachMeshNode (*ctx, meshRender);
          }

	  // we have a depth buffer, use pass 2 modes
	  graphics3D->SetZMode (CS_ZBUF_MESH2);
        }
	else if(lightingLayer < 0)
	{
	  // for deferred shading the inaccurate gbuffer version is enough.
	  lightRender.OutputDepth();

	  // we have a depth buffer, use pass 2 modes
	  graphics3D->SetZMode (CS_ZBUF_MESH2);
	}

	// deferred shading - output step
	if(lightingLayer < 0)
	{
	  lightRender.OutputResults();
	}
	else // deferred lighting - output step
	{
	  meshRender.SetLayer (lightingLayer);

	  for(size_t i = 0; i < ctxCount; ++i)
	  {
            typename RenderTree::ContextNode *ctx = contextStack[i];

	    graphics3D->SetWorldToCamera (ctx->cameraTransform.GetInverse ());

	    // Output deferred lighting results
	    ForEachDeferredMeshNode (*ctx, meshRender);
	  }
	}

        // forward rendering
        {
          ForwardMeshTreeRenderer<RenderTree> render (graphics3D, shaderMgr, deferredLayer, lightingLayer, zonlyLayer);

          for (size_t i = 0; i < ctxCount; i++)
          {
            typename RenderTree::ContextNode *ctx = contextStack[i];

	    graphics3D->SetWorldToCamera (ctx->cameraTransform.GetInverse ());
            render (ctx);
          }
        }

	// deferred rendering - debug step if wanted
	if(drawLightVolumes)
	{
          LightVolumeRenderer lightVolumeRender (lightRender, true, 0.2f);

          for (size_t i = 0; i < ctxCount; i++)
          {
            typename RenderTree::ContextNode *ctx = contextStack[i];

	    graphics3D->SetWorldToCamera (ctx->cameraTransform.GetInverse ());

            // Output light volumes.
            ForEachLight (*ctx, lightVolumeRender);
          }
	}

	// clear clipper
	graphics3D->SetClipper (nullptr, CS_CLIPPER_TOPLEVEL);
      }

      contextStack.Empty ();
    }

     /**
      * Returns the contexts accumulation buffer or NULL if no such buffer exists.
      */
    iTextureHandle* GetAccumBuffer(typename RenderTree::ContextNode* context, int &subTex)
    {
      subTex = context->renderTargets[rtaColor0].subtexture;
      return context->renderTargets[rtaColor0].texHandle;
    }

    /**
     * Returns true if the given context has the same accumulation buffer 
     * as the last context.
     */
    bool HasSameAccumBuffer(typename RenderTree::ContextNode* context)
    {
      int subTex;
      iTextureHandle *buf = GetAccumBuffer (context, subTex);

      return buf == lastAccumBuf && subTex == lastSubTex;
    }

    /**
     * Returns true if the given context has the same render view as the 
     * last context.
     */
    bool HasSameRenderView(typename RenderTree::ContextNode* context)
    {
      return context->renderView == lastRenderView;
    }

    /**
     * Returns true if the given context is different from the last context.
     */
    bool IsNew(typename RenderTree::ContextNode* context)
    {
      return !HasSameAccumBuffer (context) || !HasSameRenderView (context);
    }

  private:

    CS::RenderManager::SimpleContextRender<RenderTree> meshRender;

    iGraphics3D *graphics3D;
    iShaderManager *shaderMgr;
    iStringSet *stringSet;

    DeferredLightRenderer::PersistentData &lightRenderPersistent;
    GBuffer& gbuffer;

    csArray<typename RenderTree::ContextNode*> contextStack;

    int deferredLayer;
    int lightingLayer;
    int zonlyLayer;

    iTextureHandle *lastAccumBuf;
    int lastSubTex;
    CS::RenderManager::RenderView *lastRenderView;

    bool drawLightVolumes;
  };

}
CS_PLUGIN_NAMESPACE_END(RMDeferred)

#endif // __DEFERREDRENDER_H__
