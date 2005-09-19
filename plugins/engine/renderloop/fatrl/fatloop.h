/*
    Copyright (C) 2005 by Jorrit Tyberghein
	      (C) 2005 by Frank Richter

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

#ifndef __CS_FATLOOP_H__
#define __CS_FATLOOP_H__

#include "csutil/dirtyaccessarray.h"
#include "csutil/redblacktree.h"
#include "csutil/weakref.h"
#include "csgfx/lightsvcache.h"
#include "csgfx/shadervarframeholder.h"
#include "csgfx/shadervarcontext.h"

#include "iengine/mesh.h"
#include "iengine/rendersteps/irenderstep.h"

#include "csplugincommon/renderstep/basesteptype.h"
#include "csplugincommon/renderstep/basesteploader.h"

#include "rendernode.h"
#include "meshnode.h"
#include "portalnode.h"

#include <limits.h>

class csFatLoopType : public csBaseRenderStepType
{
public:
  csFatLoopType (iBase* p);

  virtual csPtr<iRenderStepFactory> NewFactory();
};

struct RenderPass
{
  csStringID shadertype;
  csRef<iShader> defShader;
  int maxLights;
  int maxPasses;
  /**
   * Means this pass is also used for stuff like ambient, lightmapping etc. 
   * and is also rendered if no lights are present, but also only rendered
   * at most once. */
  bool basepass;
  bool zoffset;

  RenderPass() : shadertype (csInvalidStringID),
    maxLights (INT_MAX), maxPasses (INT_MAX), basepass (false), 
    zoffset(false) {}
};

class csFatLoopLoader : public csBaseRenderStepLoader
{
  csStringHash tokens;
#define CS_TOKEN_ITEM_FILE "plugins/engine/renderloop/fatrl/fatloop.tok"
#include "cstool/tokenlist.h"

  bool ParsePass (iDocumentNode* node, RenderPass& pass);
public:
  csFatLoopLoader (iBase* p);

  virtual csPtr<iBase> Parse (iDocumentNode* node, 
    iStreamSource*, iLoaderContext* ldr_context, 	
    iBase* context);
};

class csFatLoopFactory : public iRenderStepFactory
{
private:
  iObjectRegistry* object_reg;

public:
  SCF_DECLARE_IBASE;

  csFatLoopFactory (iObjectRegistry* object_reg);
  virtual ~csFatLoopFactory ();

  virtual csPtr<iRenderStep> Create ();
};

class csFatLoopStep : public iRenderStep
{
  iObjectRegistry* object_reg;

  csWeakRef<iShaderManager> shaderManager;
  csWeakRef<iEngine> engine;
  csWeakRef<iShader> nullShader;
  csRef<iLightManager> lightmgr;

  /**
   * Render tree node.
   * Determines the order in what is drawn and also some setup for drawing.
   * Tree nodes are e.g. portals (setup portal transform), meshes(basically 
   * just rendered) ...
   */
  struct RenderNode
  {
    csRenderNode* renderNode;
    iRenderView* rview;

    // @@@ FIXME: Fog here?
    struct FogInfo
    {
      float density;
      csVector3 color;
      csVector4 plane;
    };
    FogInfo fog;

    csArray<RenderNode*> containedNodes;

    RenderNode() : renderNode(0), rview(0) {}
    ~RenderNode() { if (renderNode) renderNode->Free(); }
  };
  csBlockAllocator<RenderNode> renderNodeAlloc;

  csArray<RenderPass> passes;

  csShaderVariableContext shadervars;
  csMeshRenderNodeFactory meshNodeFact;
  csPortalRenderNodeFactory portalNodeFact;
  csStringID fogplane_name, fogdensity_name, fogcolor_name;

  void SetupFog (RenderNode* node);

  /**\name Render tree building and stuff
   * @{ */
  void CleanEmptyMeshNodes (RenderNode* node, 
    const csArray<csMeshRenderNode*>& meshNodes);
  void BuildNodeGraph (RenderNode* node, iRenderView* rview, 
    iSector* sector);
  void BuildPortalNodes (RenderNode* node, iMeshWrapper* meshwrapper, 
    iPortalContainer* portals, iRenderView* rview);
  void ProcessNode (iRenderView* rview, RenderNode* node);
  /** @} */

  /**\name SV-related stuff
   * @{ */
  csShaderVariableFrameHolder svHolder;
  csRef<csShaderVariable> GetFrameUniqueSV (uint framenr,
    csShaderVariableContext& shadervars, csStringID name);
  void FillStacks (csShaderVarStack& stacks, csRenderMesh* rm, 
    iMeshWrapper* mw, iMaterial* hdl, iShader* shader);
  /** @} */

  /**\name Lighting
   * @{ */
  csLightShaderVarCache lsvCache;
  csRef<iTextureHandle> attTex;

  csPtr<iTextureHandle> GetAttenuationTexture (int attnType);
  csPtr<iTextureHandle> GetAttenuationTexture (const csVector3& attnVec);

  void SetLightSVs (csShaderVariableContext& shadervars, iLight* light,
    size_t lightId, const csReversibleTransform& camTransR,
    const csReversibleTransform &objT, uint framenr);
  /** @} */
public:
  SCF_DECLARE_IBASE;

  csFatLoopStep (iObjectRegistry* object_reg);
  virtual ~csFatLoopStep ();

  bool AddPass (const RenderPass& pass)
  { passes.Push (pass); return true; }

  virtual void Perform (iRenderView* rview, iSector* sector,
    csShaderVarStack &stacks);
};

#endif // __CS_FATLOOP_H__
