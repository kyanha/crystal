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

#include "cssysdef.h"

#include "cstool/fogmath.h"
#include "iutil/document.h"
#include "ivideo/rndbuf.h"
#include "ivideo/graph3d.h"
#include "ivideo/rendermesh.h"
#include "ivideo/material.h"
#include "iengine/rview.h"
#include "iengine/sector.h"
#include "iengine/mesh.h"
#include "iengine/material.h"
#include "iengine/camera.h"
#include "iengine/portal.h"
#include "ivideo/material.h"
#include "imesh/object.h"
#include "iengine/portalcontainer.h"
#include "csutil/sysfunc.h"

#include "generic.h"

SCF_IMPLEMENT_FACTORY(csGenericRSType);
SCF_IMPLEMENT_FACTORY(csGenericRSLoader);

//---------------------------------------------------------------------------

csGenericRSType::csGenericRSType (iBase* p) : csBaseRenderStepType (p)
{
}

csPtr<iRenderStepFactory> csGenericRSType::NewFactory()
{
  return csPtr<iRenderStepFactory> 
    (new csGenericRenderStepFactory (object_reg));
}

//---------------------------------------------------------------------------

csGenericRSLoader::csGenericRSLoader (iBase* p) : csBaseRenderStepLoader (p)
{
  InitTokenTable (tokens);
}

csPtr<iBase> csGenericRSLoader::Parse (iDocumentNode* node, 
				       iLoaderContext* ldr_context,      
				       iBase* context)
{
  csRef<iGenericRenderStep> step;
  step.AttachNew (new csGenericRenderStep (object_reg));

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    csStringID id = tokens.Request (child->GetValue ());
    switch (id)
    {
      case XMLTOKEN_PORTALTRAVERSAL:
        {
	  bool result;
	  if (!synldr->ParseBool (child, result, true))
	    return 0;
	  step->SetPortalTraversal (result);
	}
	break;
      case XMLTOKEN_ZOFFSET:
	{
	  bool result;
	  if (!synldr->ParseBool (child, result, true))
	    return 0;
	  step->SetZOffset (result);
	}
	break;
      case XMLTOKEN_SHADERTYPE:
	step->SetShaderType (child->GetContentsValue ());
	break;
      default:
	{
	  csZBufMode zmode;
	  if (synldr->ParseZMode (child, zmode, true))
	  {
	    step->SetZBufMode (zmode);
	    break;
	  }
	  synldr->ReportBadToken (child);
	}
	return 0;
    }
  }

  return csPtr<iBase> (step);
}

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csGenericRenderStepFactory);
  SCF_IMPLEMENTS_INTERFACE(iRenderStepFactory);
SCF_IMPLEMENT_IBASE_END;

csGenericRenderStepFactory::csGenericRenderStepFactory (
  iObjectRegistry* object_reg)
{
  SCF_CONSTRUCT_IBASE(0);
  csGenericRenderStepFactory::object_reg = object_reg;
}

csGenericRenderStepFactory::~csGenericRenderStepFactory ()
{
  SCF_DESTRUCT_IBASE();
}

csPtr<iRenderStep> csGenericRenderStepFactory::Create ()
{
  return csPtr<iRenderStep> 
    (new csGenericRenderStep (object_reg));
}

//---------------------------------------------------------------------------

csStringID csGenericRenderStep::o2c_matrix_name;
csStringID csGenericRenderStep::o2c_vector_name;
csStringID csGenericRenderStep::fogplane_name;
csStringID csGenericRenderStep::fogdensity_name;
csStringID csGenericRenderStep::fogcolor_name;

SCF_IMPLEMENT_IBASE(csGenericRenderStep)
  SCF_IMPLEMENTS_INTERFACE(iRenderStep)
  SCF_IMPLEMENTS_INTERFACE(iGenericRenderStep)
  SCF_IMPLEMENTS_INTERFACE(iLightRenderStep)
SCF_IMPLEMENT_IBASE_END

csGenericRenderStep::csGenericRenderStep (
  iObjectRegistry* object_reg)
{
  SCF_CONSTRUCT_IBASE(0);

  objreg = object_reg;

  strings = CS_QUERY_REGISTRY_TAG_INTERFACE (object_reg, 
    "crystalspace.shared.stringset", iStringSet);
  shaderManager = CS_QUERY_REGISTRY (object_reg, iShaderManager);

  shadertype = 0;
  zOffset = false;
  portalTraversal = false;
  zmode = CS_ZBUF_USE;
  currentSettings = false;
  fogplane_name = strings->Request ("fogplane");
  fogdensity_name = strings->Request ("fog density");
  fogcolor_name = strings->Request ("fog color");

  visible_meshes_index = 0;
}

csGenericRenderStep::~csGenericRenderStep ()
{
  SCF_DESTRUCT_IBASE();
}

void csGenericRenderStep::RenderMeshes (iGraphics3D* g3d,
                                        iShader* shader, size_t ticket,
					iShaderVariableContext** meshContexts,
                                        csRenderMesh** meshes, 
                                        size_t num,
                                        csShaderVarStack& stacks)
{
  if (num == 0) return;
  ToggleStepSettings (g3d, true);
  if (!shaderManager)
  {
    shaderManager = CS_QUERY_REGISTRY (objreg, iShaderManager);
  }

  iMaterial *material = 0;
  iShaderVariableContext* lastMeshContext = 0;
  
  size_t numPasses = shader->GetNumberOfPasses (ticket);
  for (size_t p = 0; p < numPasses; p++)
  {
    shader->ActivatePass (ticket, p);

    size_t j;
    for (j = 0; j < num; j++)
    {
      csRenderMesh* mesh = meshes[j];
      iShaderVariableContext* meshContext = meshContexts[j];
      if (meshContext->IsEmpty())
	meshContext = 0;
      if ((!portalTraversal) && mesh->portal != 0) continue;

      if ((mesh->material->GetMaterial () != material)
	|| (meshContext != lastMeshContext))
      {
	if (lastMeshContext) meshContext->PopVariables (stacks);
        if (material != 0)
        {
          material->PopVariables (stacks);
          shader->PopVariables (stacks);
        }
        material = mesh->material->GetMaterial ();
	lastMeshContext = meshContext;
        shader->PushVariables (stacks);
        material->PushVariables (stacks);
	if (meshContext) meshContext->PushVariables (stacks);
      }
      shadervars.Top ().PushVariables (stacks);
      if (mesh->variablecontext)
        mesh->variablecontext->PushVariables (stacks);
      
      csRenderMeshModes modes (*mesh);
      shader->SetupPass (ticket, mesh, modes, stacks);
      g3d->DrawMesh (mesh, modes, stacks);
      shader->TeardownPass (ticket);

      if (mesh->variablecontext)
        mesh->variablecontext->PopVariables (stacks);
      shadervars.Top ().PopVariables (stacks);
    }
    shader->DeactivatePass (ticket);
  }

  if (lastMeshContext) lastMeshContext->PopVariables (stacks);
  if (material != 0)
  {
    material->PopVariables (stacks);
    shader->PopVariables (stacks);
  }
}

void csGenericRenderStep::Perform (iRenderView* rview, iSector* sector,
  csShaderVarStack &stacks)
{
  Perform (rview, sector, 0, stacks);
}

void csGenericRenderStep::ToggleStepSettings (iGraphics3D* g3d, 
					      bool settings)
{
  if (settings != currentSettings)
  {
    if (settings)
    {
      if (zOffset)
	g3d->EnableZOffset ();
      g3d->SetZMode (zmode);
    }
    else
    {
      if (zOffset)
	g3d->DisableZOffset ();
    }
    currentSettings = settings;
  }
}

class ShaderTicketHelper
{
private:
  csShaderVarStack& stacks;
  const csArray<csShaderVariableContext>& shadervars;
  size_t shadervars_idx;
  //csShaderVariableContext& shadervars;

  iMaterial* lastMat;
  iShader* lastShader;
  iShaderVariableContext* lastMeshContext;
  size_t matShadMeshTicket;

  void Reset ()
  {
    matShadMeshTicket = (size_t)~0;
  }

public:
  ShaderTicketHelper (csShaderVarStack& Stacks,
    const csArray<csShaderVariableContext>& sv,
    size_t sv_idx) :
    	stacks (Stacks), 
    	shadervars (sv),
	shadervars_idx (sv_idx)
  {
    lastMat = 0;
    lastShader = 0;
    lastMeshContext = 0;
    Reset ();
  }

  size_t GetTicket (iMaterial* material, iShader* shader, 
    iShaderVariableContext* meshContext, csRenderMesh* mesh)
  {
    if ((material != lastMat) || (shader != lastShader)
      || (meshContext != lastMeshContext))
    {
      Reset ();
      lastMat = material;
      lastShader = shader;
      lastMeshContext = meshContext;
    }
    if (mesh->variablecontext.IsValid () 
      && !mesh->variablecontext->IsEmpty())
    {
      shader->PushVariables (stacks);
      material->PushVariables (stacks);
      if (meshContext) meshContext->PushVariables (stacks);
      mesh->variablecontext->PushVariables (stacks);
      shadervars[shadervars_idx].PushVariables (stacks);

      csRenderMeshModes modes (*mesh);
      size_t retTicket = shader->GetTicket (modes, stacks);

      shadervars[shadervars_idx].PopVariables (stacks);
      mesh->variablecontext->PopVariables (stacks);
      if (meshContext) meshContext->PopVariables (stacks);
      material->PopVariables (stacks);
      shader->PopVariables (stacks);
      return retTicket;
    }
    else
    {
      if (matShadMeshTicket == (size_t)~0)
      {
	shader->PushVariables (stacks);
	material->PushVariables (stacks);
	if (meshContext) meshContext->PushVariables (stacks);
	shadervars[shadervars_idx].PushVariables (stacks);

	csRenderMeshModes modes (*mesh);
	matShadMeshTicket = shader->GetTicket (modes, stacks);

	shadervars[shadervars_idx].PopVariables (stacks);
	if (meshContext) meshContext->PopVariables (stacks);
	material->PopVariables (stacks);
	shader->PopVariables (stacks);
      }
      return matShadMeshTicket;
    }
  }
};

void csGenericRenderStep::Perform (iRenderView* rview, iSector* sector,
				   iLight* light,
                                   csShaderVarStack &stacks)
{
  iGraphics3D* g3d = rview->GetGraphics3D();

  csRenderMeshList* meshlist = sector->GetVisibleMeshes (rview);
  size_t num = meshlist->SortMeshLists (rview);
  visible_meshes.SetLength (visible_meshes_index+num);
  imeshes_scratch.SetLength (num);
  mesh_svc.SetLength (visible_meshes_index+num);
  csRenderMesh** sameShaderMeshes = visible_meshes.GetArray ()
  	+ visible_meshes_index;
  iShaderVariableContext** sameShaderMeshSvcs = mesh_svc.GetArray ()
  	+ visible_meshes_index;
  int prev_visible_meshes_index = visible_meshes_index;
  visible_meshes_index += (int)num;
  meshlist->GetSortedMeshes (sameShaderMeshes, imeshes_scratch.GetArray());
  for (size_t i = 0; i < num; i++)
    sameShaderMeshSvcs[i] = imeshes_scratch[i]->GetSVContext();
 
  size_t lastidx = 0;
  size_t numSSM = 0;
  iShader* shader = 0;
  size_t currentTicket = ~0;

  shadervars.Push (csShaderVariableContext ());
  shadervars.Top ().GetVariableAdd (o2c_matrix_name);
  shadervars.Top ().GetVariableAdd (o2c_vector_name);

  csRef<csShaderVariable> sv;
  if (sector->HasFog())
  {
    sv = shadervars.Top ().GetVariableAdd (fogdensity_name);
    sv->SetValue (sector->GetFog()->density);
    sv = shadervars.Top ().GetVariableAdd (fogcolor_name);
    sv->SetValue (csVector3 (sector->GetFog()->red,
    	sector->GetFog()->green,
	sector->GetFog()->blue));

    //construct a cameraplane
    csVector4 fogPlane;
    iPortal *lastPortal = rview->GetLastPortal();
    if(lastPortal)
    {
      csPlane3 plane;
      lastPortal->ComputeCameraPlane(rview->GetCamera()->GetTransform(), plane);
      fogPlane = plane.norm;
      fogPlane.w = plane.DD;
    }
    else
    {
      fogPlane = csVector4(0.0,0.0,1.0,0.0);
    }
    sv = shadervars.Top ().GetVariableAdd (fogplane_name);
    sv->SetValue (fogPlane);
  }
  else
  {
    sv = shadervars.Top ().GetVariableAdd (fogdensity_name);
    sv->SetValue (0);
  }

  ShaderTicketHelper ticketHelper (stacks, shadervars, shadervars.Length ()-1);
  const csReversibleTransform& camt = rview->GetCamera ()->GetTransform ();

  for (size_t n = 0; n < num; n++)
  {
    csRenderMesh* mesh = sameShaderMeshes[n];

    if (mesh->portal) 
    {
      if (numSSM > 0)
      {
        if (shader != 0)
	{
          g3d->SetWorldToCamera (camt);
	  RenderMeshes (g3d, shader, currentTicket, sameShaderMeshSvcs + lastidx,
	    sameShaderMeshes+lastidx, numSSM, stacks);
          shader = 0;
	}
        numSSM = 0;
      }

      if (portalTraversal)
      {
        ToggleStepSettings (g3d, false);
        shadervars.Top ().PushVariables (stacks);
        mesh->portal->Draw (rview);
        shadervars.Top ().PopVariables (stacks);
      }

      // Portal traversal can relocate the visible_meshes
      // growing array. So after portal traversal we have to fix
      // the sameShaderMeshes pointer because it may now point
      // to an invalid area.
      sameShaderMeshes = visible_meshes.GetArray () + prev_visible_meshes_index;
      sameShaderMeshSvcs = mesh_svc.GetArray () + prev_visible_meshes_index;
    }
    else 
    {
#ifdef CS_DEBUG
      if (!mesh->material)
      {
	csPrintfErr ("INTERNAL ERROR: mesh '%s' is missing a material!\n",
	  mesh->db_mesh_name);
	exit (-1);
      }
#endif
      iMaterialHandle* hdl = mesh->material->GetMaterialHandle ();
#ifdef CS_DEBUG
      if (!hdl)
      {
        csPrintfErr ("INTERNAL ERROR: mesh '%s' is missing a material handle!\n",
	  mesh->db_mesh_name);
	exit (-1);
      }
#endif
      iShader* meshShader = hdl->GetShader (shadertype);
      size_t newTicket = meshShader ? ticketHelper.GetTicket (
	mesh->material->GetMaterial (), meshShader, 
	sameShaderMeshSvcs[n], mesh) : ~0;
      if ((meshShader != shader) || (newTicket != currentTicket))
      {
        // @@@ Need error reporter
        if (shader != 0)
	{
          g3d->SetWorldToCamera (camt);
          RenderMeshes (g3d, shader, currentTicket, sameShaderMeshSvcs + lastidx, 
	    sameShaderMeshes + lastidx, numSSM, stacks);
	}
	lastidx = n;
        shader = meshShader;
	currentTicket = newTicket;
        numSSM = 0;
      }
      numSSM++;
    }
  }
  
  if (numSSM != 0)
  {
    // @@@ Need error reporter
    if (shader != 0)
    {
      g3d->SetWorldToCamera (camt);
      RenderMeshes (g3d, shader, currentTicket, sameShaderMeshSvcs + lastidx,
        sameShaderMeshes + lastidx, numSSM, stacks);
    }
  }

  shadervars.Pop ();

  ToggleStepSettings (g3d, false);

  visible_meshes_index = prev_visible_meshes_index;
}

void csGenericRenderStep::SetShaderType (const char* type)
{
  shadertype = strings->Request (type);
}

const char* csGenericRenderStep::GetShaderType ()
{
  return strings->Request (shadertype);
}

void csGenericRenderStep::SetZOffset (bool zOffset)
{
  csGenericRenderStep::zOffset = zOffset;
}

bool csGenericRenderStep::GetZOffset () const
{
  return zOffset;
}

void csGenericRenderStep::SetZBufMode (csZBufMode zmode)
{
  csGenericRenderStep::zmode = zmode;
}

csZBufMode csGenericRenderStep::GetZBufMode () const
{
  return zmode;
}

