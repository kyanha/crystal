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

    ForwardMeshTreeRenderer(iGraphics3D* g3d, iShaderManager *shaderMgr, int deferredLayer, int zonlyLayer)
      : 
    meshRender(g3d, shaderMgr),
    graphics3D(g3d),
    shaderMgr(shaderMgr),
    deferredLayer(deferredLayer),
    zonlyLayer(zonlyLayer)
    {}

    ~ForwardMeshTreeRenderer() {}

    /**
     * Render all contexts.
     */
    void operator()(typename RenderTree::ContextNode *context)
    {
      CS::RenderManager::RenderView *rview = context->renderView;
      
      iCamera *cam = rview->GetCamera ();
      iClipper2D *clipper = rview->GetClipper ();
      
      // Setup the camera etc.. @@should be delayed as well
      graphics3D->SetProjectionMatrix (context->perspectiveFixup * cam->GetProjectionMatrix ());
      graphics3D->SetClipper (clipper, CS_CLIPPER_TOPLEVEL);

      graphics3D->SetWorldToCamera (context->cameraTransform.GetInverse ());

      size_t layerCount = context->svArrays.GetNumLayers ();
      for (size_t layer = 0; layer < layerCount; ++layer)
      {
        if ((int)layer == deferredLayer || (int)layer == zonlyLayer)
          continue;

        meshRender.SetLayer (layer);
        ForEachForwardMeshNode (*context, meshRender);
      }
    }
   
  private:

    CS::RenderManager::SimpleContextRender<RenderTree> meshRender;

    iGraphics3D *graphics3D;
    iShaderManager *shaderMgr;

    int deferredLayer;
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
   *             gbuffer, lightRenderPersistent, deferredLayer, 
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
                         int zonlyLayer,
                         bool drawLightVolumes)
      : 
    meshRender(g3d, shaderMgr),
    graphics3D(g3d),
    shaderMgr(shaderMgr),
    stringSet(stringSet),
    lightRenderPersistent(lightRenderPersistent),
    deferredLayer(deferredLayer),
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
        lastGBuffer = context->gbuffer;
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

      CS::RenderManager::RenderView *rview = context->renderView;

      GBuffer* gbuffer = context->gbuffer;
      iCamera *cam = rview->GetCamera ();
      iClipper2D *clipper = rview->GetClipper ();

      // Fill the gbuffer
      gbuffer->Attach ();
      {
        // Setup the camera etc.
        graphics3D->SetProjectionMatrix (context->perspectiveFixup * cam->GetProjectionMatrix ());
        graphics3D->SetClipper (clipper, CS_CLIPPER_TOPLEVEL);

        int drawFlags = CSDRAW_3DGRAPHICS | context->drawFlags;
        drawFlags |= CSDRAW_CLEARSCREEN | CSDRAW_CLEARZBUFFER;
        graphics3D->BeginDraw (drawFlags);
        graphics3D->SetWorldToCamera (context->cameraTransform.GetInverse ());

        graphics3D->SetZMode (CS_ZBUF_MESH);

        // Any rendering required for visculling needs to be done once only per sector.
        csArray<iSector*> sectors;
        for (size_t c = 0; c < contextStack.GetSize (); ++c)
        {
          typename RenderTree::ContextNode* ctx = contextStack[c];

          size_t numSectors = sectors.GetSize ();
          if (sectors.PushSmart (ctx->sector) == numSectors)
          {
            graphics3D->SetWorldToCamera (ctx->cameraTransform.GetInverse ());
            ctx->sector->GetVisibilityCuller ()->RenderViscull (rview, ctx->shadervars);
          }
        }

        meshRender.SetLayer (deferredLayer);

        for (size_t i = 0; i < ctxCount; i++)
        {
          typename RenderTree::ContextNode *context = contextStack[i];
          
          ForEachDeferredMeshNode (*context, meshRender);
        }
        
        graphics3D->FinishDraw ();
      }
      gbuffer->Detach ();

      // Assume that the accumulation texture matches the gbuffer dimensions.
  #if CS_DEBUG
      int subTex;
      iTextureHandle* buf = GetAccumBuffer(context, subTex);
      CS_ASSERT(buf);

      int w, h;
      int bufW, bufH;
      gbuffer->GetDimensions (w, h);
      buf->GetRendererDimensions (bufW, bufH);

      CS_ASSERT (w == bufW && h == bufH);
  #endif

      // attach render targets.
      for (int a = 0; a < rtaNumAttachments; a++)
        graphics3D->SetRenderTarget (context->renderTargets[a].texHandle, false,
	    context->renderTargets[a].subtexture, csRenderTargetAttachment (a));
      CS_ASSERT(graphics3D->ValidateRenderTargets ());

      // Fill the accumulation buffer
      {
        int drawFlags = CSDRAW_3DGRAPHICS | context->drawFlags;
        drawFlags |=  CSDRAW_CLEARSCREEN | CSDRAW_CLEARZBUFFER;

        graphics3D->BeginDraw (drawFlags);
        graphics3D->SetWorldToCamera (context->cameraTransform.GetInverse ());

        // z only pass
        // @@@FIXME: this should be done during gbuffer fill pass
        //           and copy the depth buffer from the gbuffer here
        //           if the gbuffer has a depth buffer
        {
          graphics3D->SetZMode (CS_ZBUF_MESH);
          meshRender.SetLayer (zonlyLayer);

          for (size_t i = 0; i < ctxCount; i++)
          {
            typename RenderTree::ContextNode *context = contextStack[i];

            ForEachMeshNode (*context, meshRender);
          }
        }

        graphics3D->SetZMode (CS_ZBUF_MESH2);

        // forward rendering
        {
          ForwardMeshTreeRenderer<RenderTree> render (graphics3D, shaderMgr, deferredLayer, zonlyLayer);

          for (size_t i = 0; i < ctxCount; i++)
          {
            typename RenderTree::ContextNode *context = contextStack[i];

            render (context);
          }
        }

        // deferred lighting
        {
          DeferredLightRenderer lightRender (graphics3D,
                                             shaderMgr,
                                             stringSet,
                                             rview,
                                             *gbuffer,
                                             lightRenderPersistent);
          LightVolumeRenderer lightVolumeRender (lightRender, true, 0.2f);

          // ambient light
          lightRender.OutputAmbientLight ();

          for (size_t i = 0; i < ctxCount; i++)
          {
            typename RenderTree::ContextNode *context = contextStack[i];

            // other light types
            ForEachLight (*context, lightRender);

            // Output light volumes.
            if (drawLightVolumes)
              ForEachLight (*context, lightVolumeRender);
          }
        }

        // finish draw also unsets render targets
        graphics3D->FinishDraw ();
      }

      /* @@@ FIXME: When switching from RT to screen with a clipper set
         the clip rect gets wrong (stays at RT size). This workaround ensures
         that no "old" clip rect is stored which is restored later.
         Should really be fixed in the renderer. */
      graphics3D->SetClipper (nullptr, CS_CLIPPER_TOPLEVEL);

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
     * Returns true if the given context has the same GBuffer as the 
     * last context.
     */
    bool HasSameGBuffer(typename RenderTree::ContextNode* context)
    {
      return context->gbuffer == lastGBuffer;
    }

    /**
     * Returns true if the given context is different from the last context.
     */
    bool IsNew(typename RenderTree::ContextNode* context)
    {
      return !HasSameAccumBuffer (context) || !HasSameRenderView (context) || !HasSameGBuffer (context);
    }

  private:

    CS::RenderManager::SimpleContextRender<RenderTree> meshRender;

    iGraphics3D *graphics3D;
    iShaderManager *shaderMgr;
    iStringSet *stringSet;

    DeferredLightRenderer::PersistentData &lightRenderPersistent;

    csArray<typename RenderTree::ContextNode*> contextStack;

    int deferredLayer;
    int zonlyLayer;

    GBuffer* lastGBuffer;
    iTextureHandle *lastAccumBuf;
    int lastSubTex;
    CS::RenderManager::RenderView *lastRenderView;

    bool drawLightVolumes;
  };

}
CS_PLUGIN_NAMESPACE_END(RMDeferred)

#endif // __DEFERREDRENDER_H__
