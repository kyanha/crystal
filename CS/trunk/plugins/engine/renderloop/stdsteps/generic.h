/*
    Copyright (C) 2003 by Jorrit Tyberghein
	      (C) 2003 by Frank Richter
              (C) 2003 by Marten Svanfeldt

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

#ifndef __CS_GENERIC_H__
#define __CS_GENERIC_H__

#include "csutil/scf.h"
#include "csutil/csstring.h"
#include "iengine/light.h"
#include "iengine/renderloop.h"
#include "iengine/viscull.h"
#include "iengine/rendersteps/irenderstep.h"
#include "iengine/rendersteps/igeneric.h"
#include "iengine/rendersteps/ilightiter.h"
#include "ivideo/shader/shader.h"
#include "csgfx/shadervarcontext.h"

#include "cstool/rendermeshlist.h"

#include "../common/basesteptype.h"
#include "../common/basesteploader.h"

class csGenericRSType : public csBaseRenderStepType
{
public:
  csGenericRSType (iBase* p);

  virtual csPtr<iRenderStepFactory> NewFactory();
};

class csGenericRSLoader : public csBaseRenderStepLoader
{
  csStringHash tokens;
#define CS_TOKEN_ITEM_FILE "plugins/engine/renderloop/stdsteps/generic.tok"
#include "cstool/tokenlist.h"

public:
  csGenericRSLoader (iBase* p);

  virtual csPtr<iBase> Parse (iDocumentNode* node, 
    iLoaderContext* ldr_context, 	
    iBase* context);
};

class csGenericRenderStepFactory : public iRenderStepFactory
{
  csRef<iObjectRegistry> object_reg;
public:
  SCF_DECLARE_IBASE;

  csGenericRenderStepFactory (iObjectRegistry* object_reg);
  virtual ~csGenericRenderStepFactory ();

  virtual csPtr<iRenderStep> Create ();
};

class csGenericRenderStep : public iRenderStep, 
			    public iGenericRenderStep,
			    public iLightRenderStep
{
private:
//  csRenderLoop* rl;
  csStringID shadertype;
  bool zOffset;
  bool portalTraversal;
  csZBufMode zmode;
  csRef<iStringSet> strings;
  csRef<iShaderManager> shaderManager;
  iObjectRegistry *objreg;

  bool currentSettings;
  csShaderVariableContext shadervars;

  static csStringID o2c_matrix_name;
  static csStringID o2c_vector_name;
  static csStringID fogplane_name;
  static csStringID fogdensity_name;
public:
  SCF_DECLARE_IBASE;

  csGenericRenderStep (iObjectRegistry* object_reg);
  virtual ~csGenericRenderStep ();

  virtual void Perform (iRenderView* rview, iSector* sector,
    csShaderVarStack &stacks);
  virtual void Perform (iRenderView* rview, iSector* sector,
    iLight* light, csShaderVarStack &stacks);

  virtual void SetShaderType (const char* type);
  virtual const char* GetShaderType ();

  virtual void SetZOffset (bool zOffset);
  virtual bool GetZOffset () const;
  virtual void SetPortalTraversal (bool p) { portalTraversal = p; }
  virtual bool GetPortalTraversal () const { return portalTraversal; }

  virtual void SetZBufMode (csZBufMode zmode);
  virtual csZBufMode GetZBufMode () const;

  inline void RenderMeshes (iGraphics3D* g3d, iShader* shader, 
    csRenderMesh** meshes, int num, csShaderVarStack &stacks);

  /// Enables/disables z offset and z mode as needed
  inline void ToggleStepSettings (iGraphics3D* g3d, bool settings);
};


#endif
