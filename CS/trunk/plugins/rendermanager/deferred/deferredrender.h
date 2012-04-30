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
      GBuffer* gbuffer = context->gbuffer;
      iCamera *cam = rview->GetCamera ();
      iClipper2D *clipper = rview->GetClipper ();

      // seriously, we need those
      CS_ASSERT(rview);
      CS_ASSERT(gbuffer);
      CS_ASSERT(cam);
      CS_ASSERT(clipper);

      // filter out zbuffer draw flags - they'll most certainly break the rendering
      context->drawFlags &= ~CSDRAW_CLEARZBUFFER;

      // shared setup for all passes - projection only
      graphics3D->SetProjectionMatrix (context->perspectiveFixup * cam->GetProjectionMatrix ());

      // create the light render here as we'll use it a lot
      DeferredLightRenderer lightRender (graphics3D,
                                         shaderMgr,
                                         stringSet,
                                         rview,
                                         *gbuffer,
                                         lightRenderPersistent);

      // set tex scale to default
      lightRenderPersistent.scale->SetValue(csVector4(0.5,0.5,0.5,0.5));

      // Fill the gbuffer
      gbuffer->Attach ();
      {
	// setup clipper
	graphics3D->SetClipper (clipper, CS_CLIPPER_TOPLEVEL);

        int drawFlags = CSDRAW_3DGRAPHICS | context->drawFlags;
        drawFlags |= CSDRAW_CLEARSCREEN | CSDRAW_CLEARZBUFFER;

        CS::RenderManager::BeginFinishDrawScope bd(graphics3D, drawFlags);

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

        // z only pass - should be disabled if occluvis is used
	if(zonlyLayer >= 0)
        {
	  // we want to fill the zbuffer, use pass 1 modes
          graphics3D->SetZMode (CS_ZBUF_MESH);

          meshRender.SetLayer (zonlyLayer);

          for (size_t i = 0; i < ctxCount; i++)
          {
            typename RenderTree::ContextNode *ctx = contextStack[i];

	    graphics3D->SetWorldToCamera (ctx->cameraTransform.GetInverse ());

	    // for the depth buffer we want *all* meshes, not just deferred ones
            ForEachMeshNode (*ctx, meshRender);
          }
        }

	// deferred pass
        // @@@TODO: we could check for CS_ENTITY_NOLIGHTING here
        //          and use it to fill the stencil buffer so those
        //          meshes can be skipped during the lighting pass
	{
	  // zbuffer is already filled, use pass 2 modes
	  graphics3D->SetZMode (CS_ZBUF_MESH2);

	  meshRender.SetLayer (deferredLayer);

	  for (size_t i = 0; i < ctxCount; i++)
	  {
            typename RenderTree::ContextNode *ctx = contextStack[i];
          
	    graphics3D->SetWorldToCamera (ctx->cameraTransform.GetInverse ());

	    // only deferred ones contribute to the gbuffer
	    ForEachDeferredMeshNode (*ctx, meshRender);
	  }
	}
      }

      // Fill the accumulation buffers
      gbuffer->AttachAccumulation(); // attach accumulation buffers
      CS_ASSERT(graphics3D->ValidateRenderTargets ());
      {
	graphics3D->SetClipper(clipper, CS_CLIPPER_TOPLEVEL);

        int drawFlags = CSDRAW_3DGRAPHICS | context->drawFlags;
	drawFlags |= CSDRAW_CLEARSCREEN;

	CS::RenderManager::BeginFinishDrawScope bd (graphics3D, drawFlags);

	// use pass 1 zmodes for populating the zbuffer if needed
	graphics3D->SetZMode (CS_ZBUF_MESH);

	// @@@FIXME: re-using zbuffer doesn't work, see below
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
      }

      // attach output render targets.
      for (int a = 0; a < rtaNumAttachments; a++)
	graphics3D->SetRenderTarget (context->renderTargets[a].texHandle, false,
	    context->renderTargets[a].subtexture, csRenderTargetAttachment (a));
      //CS_ASSERT(graphics3D->ValidateRenderTargets ());
      {
	// setup clipper
	graphics3D->SetClipper (clipper, CS_CLIPPER_TOPLEVEL);

        int drawFlags = CSDRAW_3DGRAPHICS | context->drawFlags;
	drawFlags |= CSDRAW_CLEARSCREEN;

        CS::RenderManager::BeginFinishDrawScope bd (graphics3D, drawFlags);

	// set tex scale for lookups as we may have flipped screen here
	lightRenderPersistent.scale->SetValue(csVector4(
	  0.5, (context->renderTargets[rtaColor0].texHandle == (iTextureHandle*)nullptr) ? -0.5 : 0.5,
	  0.5,0.5));

	// @@@FIXME: preserving the depth buffer doesn't work at all
        // @@@NOTE: it does an implicit CLEARZBUFFER with clipping enabled
        //          on each BeginDraw that has CSDRAW_3DGRAPHCIS
	graphics3D->SetZMode (CS_ZBUF_MESH);
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
        }
	else
	{
	  lightRender.OutputDepth();
	}

	// we're in the output pass - use pass 2 zmodes
	graphics3D->SetZMode (CS_ZBUF_MESH2);

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
