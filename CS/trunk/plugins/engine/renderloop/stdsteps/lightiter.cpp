/*
    Copyright (C) 2003 by Jorrit Tyberghein
	      (C) 2003 by Frank Richter
              (C) 2005 by Marten Svanfeldt

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
#include "csqint.h"

#include "csgeom/sphere.h"
#include "csgfx/imagememory.h"
#include "csutil/cscolor.h"
#include "iengine/camera.h"
#include "iengine/light.h"
#include "iengine/material.h"
#include "iengine/mesh.h"
#include "iengine/rview.h"
#include "iengine/sector.h"
#include "iengine/movable.h"
#include "igraphic/image.h"
#include "iutil/document.h"
#include "ivideo/rndbuf.h"
#include "ivideo/txtmgr.h"

#include "lightiter.h"

CS_LEAKGUARD_IMPLEMENT (csLightIterRSType);
CS_LEAKGUARD_IMPLEMENT (csLightIterRSLoader);
CS_LEAKGUARD_IMPLEMENT (csLightIterRenderStepFactory);
CS_LEAKGUARD_IMPLEMENT (csLightIterRenderStep::LightSVAccessor);

SCF_IMPLEMENT_FACTORY(csLightIterRSType)
SCF_IMPLEMENT_FACTORY(csLightIterRSLoader)

//---------------------------------------------------------------------------

csLightIterRSType::csLightIterRSType (iBase* p) :
  scfImplementationType (this, p)
{
}

csPtr<iRenderStepFactory> csLightIterRSType::NewFactory()
{
  return csPtr<iRenderStepFactory> 
    (new csLightIterRenderStepFactory (object_reg));
}

//---------------------------------------------------------------------------

csLightIterRSLoader::csLightIterRSLoader (iBase* p) :
  scfImplementationType (this, p)
{
  InitTokenTable (tokens);
}

bool csLightIterRSLoader::Initialize (iObjectRegistry* object_reg)
{
  if (csBaseRenderStepLoader::Initialize (object_reg))
  {
    return rsp.Initialize (object_reg);
  }
  else
  {
    return false;
  }
}

csPtr<iBase> csLightIterRSLoader::Parse (iDocumentNode* node, 
				       iStreamSource*,
				       iLoaderContext* /*ldr_context*/,      
				       iBase* /*context*/)
{
  csRef<iLightIterRenderStep> step;
  step.AttachNew (new csLightIterRenderStep (object_reg));
  csRef<iRenderStepContainer> steps =
    scfQueryInterface<iRenderStepContainer> (step);

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    csStringID id = tokens.Request (child->GetValue ());
    switch (id)
    {
      case XMLTOKEN_STEPS:
	{
	  if (!rsp.ParseRenderSteps (steps, child))
	    return 0;
	}
	break;
      default:
	if (synldr) synldr->ReportBadToken (child);
	return 0;
    }
  }

  return csPtr<iBase> (step);
}

//---------------------------------------------------------------------------

csLightIterRenderStepFactory::csLightIterRenderStepFactory (
  iObjectRegistry* object_reg) :
  scfImplementationType (this)
{
  csLightIterRenderStepFactory::object_reg = object_reg;
}

csLightIterRenderStepFactory::~csLightIterRenderStepFactory ()
{
}

csPtr<iRenderStep> csLightIterRenderStepFactory::Create ()
{
  return csPtr<iRenderStep> 
    (new csLightIterRenderStep (object_reg));
}

//---------------------------------------------------------------------------

csLightIterRenderStep::csLightIterRenderStep (
  iObjectRegistry* object_reg) :
  scfImplementationType (this)
{
  csLightIterRenderStep::object_reg = object_reg;
  initialized = false;
}

csLightIterRenderStep::~csLightIterRenderStep ()
{
  csHash<LightSVAccessor*, csPtrKey<iLight> >::GlobalIterator it =
    knownLights.GetIterator();

  while (it.HasNext())
  {
    csPtrKey<iLight> light;
    LightSVAccessor* cb = it.Next (light);
    light->RemoveLightCallback (cb);
  }
}

void csLightIterRenderStep::Init ()
{
  if (!initialized)
  {
    initialized = true;

    g3d = csQueryRegistry<iGraphics3D> (object_reg);

    csRef<iStringSet> strings = csQueryRegistryTagInterface<iStringSet> (
      object_reg, "crystalspace.shared.stringset");

    csStringID posname = strings->Request ("light 0 position");
    csStringID poswname = strings->Request ("light 0 position world");
    csStringID trname = strings->Request ("light 0 transform");
    csStringID trwname = strings->Request ("light 0 transform world");
    csStringID difname = strings->Request ("light 0 diffuse");
    csStringID spcname = strings->Request ("light 0 specular");
    csStringID attname = strings->Request ("light 0 attenuation");
    csStringID atxname = strings->Request ("light 0 attenuationtex");
    csStringID infallname = strings->Request ("light 0 inner falloff");
    csStringID ofallname = strings->Request ("light 0 outer falloff");
    trw_inv_name = strings->Request ("light 0 transform inverse world");
    CS::ShaderVarName lightcountname (strings, "light count");

    shadermgr = csQueryRegistry<iShaderManager> (object_reg);
    lightmgr = csQueryRegistry<iLightManager> (object_reg);

    shvar_light_0_position = shadermgr->GetVariable (posname);
    if (!shvar_light_0_position)
    {
      shvar_light_0_position = new csShaderVariable (posname);
      shvar_light_0_position->SetType (csShaderVariable::VECTOR4);
      shadermgr->AddVariable (shvar_light_0_position);
    }

    shvar_light_0_position_world = shadermgr->GetVariable (poswname);
    if (!shvar_light_0_position_world)
    {
      shvar_light_0_position_world = new csShaderVariable (poswname);
      shvar_light_0_position_world->SetType (csShaderVariable::VECTOR4);
      shadermgr->AddVariable (shvar_light_0_position_world);
    }

    shvar_light_0_transform = shadermgr->GetVariable (trname);
    if (!shvar_light_0_transform)
    {
      shvar_light_0_transform = new csShaderVariable (trname);
      shvar_light_0_transform->SetType (csShaderVariable::TRANSFORM);
      shadermgr->AddVariable (shvar_light_0_transform);
    }

    shvar_light_0_transform_world = shadermgr->GetVariable (trwname);
    if (!shvar_light_0_transform_world)
    {
      shvar_light_0_transform_world = new csShaderVariable (trwname);
      shvar_light_0_transform_world->SetType (csShaderVariable::TRANSFORM);
      shadermgr->AddVariable (shvar_light_0_transform_world);
    }

    shvar_light_0_diffuse = shadermgr->GetVariable (difname);
    if (!shvar_light_0_diffuse)
    {
      shvar_light_0_diffuse = new csShaderVariable (difname);
      shvar_light_0_diffuse->SetType (csShaderVariable::VECTOR4);
      shadermgr->AddVariable (shvar_light_0_diffuse);
    }

    shvar_light_0_specular = shadermgr->GetVariable (spcname);
    if (!shvar_light_0_specular)
    {
      shvar_light_0_specular = new csShaderVariable (spcname);
      shvar_light_0_specular->SetType (csShaderVariable::VECTOR4);
      shadermgr->AddVariable (shvar_light_0_specular);
    }

    shvar_light_0_attenuation = shadermgr->GetVariable (attname);
    if (!shvar_light_0_attenuation)
    {
      shvar_light_0_attenuation = new csShaderVariable (attname);
      shvar_light_0_attenuation->SetType (csShaderVariable::VECTOR4);
      shadermgr->AddVariable (shvar_light_0_attenuation);
    }

    shvar_light_0_attenuationtex = shadermgr->GetVariable (atxname);
    if (!shvar_light_0_attenuationtex)
    {
      shvar_light_0_attenuationtex = new csShaderVariable (atxname);
      shvar_light_0_attenuationtex->SetType (csShaderVariable::TEXTURE);
      shadermgr->AddVariable (shvar_light_0_attenuationtex);
    }

    shvar_light_0_inner_falloff = shadermgr->GetVariable (infallname);
    if (!shvar_light_0_inner_falloff)
    {
      shvar_light_0_inner_falloff = new csShaderVariable (infallname);
      shvar_light_0_inner_falloff->SetType (csShaderVariable::FLOAT);
      shadermgr->AddVariable (shvar_light_0_inner_falloff);
    }

    shvar_light_0_outer_falloff = shadermgr->GetVariable (ofallname);
    if (!shvar_light_0_outer_falloff)
    {
      shvar_light_0_outer_falloff = new csShaderVariable (ofallname);
      shvar_light_0_outer_falloff->SetType (csShaderVariable::FLOAT);
      shadermgr->AddVariable (shvar_light_0_outer_falloff);
    }

    csRef<csShaderVariable> svLightCount (
      shadermgr->GetVariable (lightcountname));
    if (!svLightCount)
    {
      svLightCount = new csShaderVariable (lightcountname);
      shadermgr->AddVariable (svLightCount);
    }
    svLightCount->SetValue (1);
  }
}

csLightIterRenderStep::LightSVAccessor* 
csLightIterRenderStep::GetLightAccessor (iLight* light)
{
  LightSVAccessor* acc = knownLights.Get (light, 0);
  if (acc == 0)
  {
    acc = new LightSVAccessor (light, this);
    knownLights.Put (light, acc);
    acc->DecRef();
  }
  return acc;
}

void csLightIterRenderStep::Perform (iRenderView* rview, iSector* sector,
  iShaderVarStack* stacks)
{
  Init ();

  // @@@ This code is ignoring dynamic lights. Perhaps we need a better
  // way to represent those.
  //iLightList* lights = sector->GetLights();
  //int nlights = lights->GetCount();
  const csArray<iLightSectorInfluence*>& lights = lightmgr->GetRelevantLights (sector,
      -1, false);
  int nlights = lights.GetSize ();

  csArray<iLight*> lightList (16);

  while (nlights-- > 0)
  {
    //iLight* light = lights->Get (nlights);
    iLight* light = lights.Get (nlights)->GetLight ();
    const csVector3 lightPos = light->GetMovable ()->GetFullPosition ();

    /* 
    @@@ material specific diffuse/specular/ambient.
    Realized as shader variables maybe?
    */
    csReversibleTransform camTransR = rview->GetCamera()->GetTransform();

    const csColor& color = light->GetColor ();
    shvar_light_0_diffuse->SetValue (
      csVector3 (color.red, color.green, color.blue));

    shvar_light_0_specular->SetValue (csVector3 (1));

    csLightAttenuationMode attnMode = light->GetAttenuationMode ();
    if (attnMode == CS_ATTN_LINEAR)
    {
      float r = light->GetAttenuationConstants ().x;
      shvar_light_0_attenuation->SetValue (csVector3(r, 1/r, 0));
    }
    else
    {
      shvar_light_0_attenuation->SetValue (light->GetAttenuationConstants ());
    }
    shvar_light_0_position->SetValue (lightPos * camTransR);
    shvar_light_0_position_world->SetValue (lightPos);
    shvar_light_0_transform->SetValue (light->GetMovable()->GetFullTransform()
    	* camTransR);
    shvar_light_0_transform_world->SetValue (light->GetMovable()
    	->GetFullTransform());
    float falloffInner, falloffOuter;
    light->GetSpotLightFalloff (falloffInner, falloffOuter);
    shvar_light_0_inner_falloff->SetValue (falloffInner);
    shvar_light_0_outer_falloff->SetValue (falloffOuter);

    shvar_light_0_attenuationtex->SetAccessor (GetLightAccessor (light));

	csShaderVariable *sv;
	sv = light->GetSVContext()->GetVariableAdd(trw_inv_name);
    sv->SetValue (light->GetMovable()->GetFullTransform().GetInverse());

    lightList.Push (light);
    shadermgr->SetActiveLights (lightList);
    lightList.Empty ();

    csSphere lightSphere (lightPos, light->GetCutoffDistance ());
    if (rview->TestBSphere (camTransR, lightSphere))
    {
      size_t i;
      for (i = 0; i < steps.GetSize (); i++)
      {
        steps[i]->Perform (rview, sector, light, stacks);
      }
    }
  }
}

size_t csLightIterRenderStep::AddStep (iRenderStep* step)
{
  csRef<iLightRenderStep> lrs = 
    scfQueryInterface<iLightRenderStep> (step);
  if (!lrs) return csArrayItemNotFound;
  return steps.Push (lrs);
}

bool csLightIterRenderStep::DeleteStep (iRenderStep* step)
{
  csRef<iLightRenderStep> lrs = 
    scfQueryInterface<iLightRenderStep> (step);
  if (!lrs) return false;
  return steps.Delete(lrs);
}

iRenderStep* csLightIterRenderStep::GetStep (size_t n) const
{
  return (iRenderStep*) steps.Get(n);
}

size_t csLightIterRenderStep::Find (iRenderStep* step) const
{
  csRef<iLightRenderStep> lrs = 
    scfQueryInterface<iLightRenderStep> (step);
  if (!lrs) return csArrayItemNotFound;
  return steps.Find(lrs);
}

size_t csLightIterRenderStep::GetStepCount () const
{
  return steps.GetSize ();
}

csPtr<iTextureHandle> csLightIterRenderStep::GetAttenuationTexture (
  int /*attnType*/)
{
  if (!attTex.IsValid())
  {
  #define CS_ATTTABLE_SIZE	  128
  #define CS_HALF_ATTTABLE_SIZE	  ((float)CS_ATTTABLE_SIZE/2.0f)

    csRGBpixel *attenuationdata = 
      new csRGBpixel[CS_ATTTABLE_SIZE * CS_ATTTABLE_SIZE];
    csRGBpixel* data = attenuationdata;
    for (int y=0; y < CS_ATTTABLE_SIZE; y++)
    {
      for (int x=0; x < CS_ATTTABLE_SIZE; x++)
      {
	float yv = 3.0f * ((y + 0.5f)/CS_HALF_ATTTABLE_SIZE - 1.0f);
	float xv = 3.0f * ((x + 0.5f)/CS_HALF_ATTTABLE_SIZE - 1.0f);
	float i = exp (-0.7 * (xv*xv + yv*yv));
	unsigned char v = i>1.0f ? 255 : csQint (i*255.99f);
	(data++)->Set (v, v, v, v);
      }
    }

    csRef<iImage> img = csPtr<iImage> (new csImageMemory (
      CS_ATTTABLE_SIZE, CS_ATTTABLE_SIZE, attenuationdata, true, 
      CS_IMGFMT_TRUECOLOR | CS_IMGFMT_ALPHA));
    attTex = g3d->GetTextureManager()->RegisterTexture (
	img, CS_TEXTURE_3D | CS_TEXTURE_CLAMP | CS_TEXTURE_NOMIPMAPS);
    attTex->SetTextureClass ("lookup");
  }
  return csPtr<iTextureHandle> (attTex);
}

csPtr<iTextureHandle> csLightIterRenderStep::GetAttenuationTexture (
  const csVector3& attnVec)
{
  if (attnVec.z != 0)
    return GetAttenuationTexture (CS_ATTN_REALISTIC);
  else if (attnVec.y != 0)
    return GetAttenuationTexture (CS_ATTN_INVERSE);
  else 
    return GetAttenuationTexture (CS_ATTN_NONE);
}


//---------------------------------------------------------------------------

csLightIterRenderStep::LightSVAccessor::LightSVAccessor (iLight* light,
  csLightIterRenderStep* parent) :
  scfImplementationType (this)
{
  LightSVAccessor::light = light;
  LightSVAccessor::parent = parent;

  needUpdate = true;
  light->SetLightCallback (this);
}

csLightIterRenderStep::LightSVAccessor::~LightSVAccessor ()
{
}

void csLightIterRenderStep::LightSVAccessor::OnColorChange (iLight* /*light*/,
  const csColor& /*newcolor*/)
{
}

void csLightIterRenderStep::LightSVAccessor::OnPositionChange (iLight* /*light*/, 
  const csVector3& /*newpos*/)
{
}

void csLightIterRenderStep::LightSVAccessor::OnSectorChange (iLight* /*light*/, 
  iSector* /*newsector*/)
{
}

void csLightIterRenderStep::LightSVAccessor::OnRadiusChange (iLight* /*light*/, 
  float /*newradius*/)
{
}

void csLightIterRenderStep::LightSVAccessor::OnDestroy (iLight* /*light*/)
{
  parent->knownLights.Delete (this->light, this);
  //delete this;
}

void csLightIterRenderStep::LightSVAccessor::OnAttenuationChange (
  iLight* /*light*/, int newatt)
{
  needUpdate = true;
  attnType = newatt;
}

void csLightIterRenderStep::LightSVAccessor::PreGetValue (
  csShaderVariable *variable)
{
  if (needUpdate)
  {
    //CreateTexture ();
    if (attnType == CS_ATTN_CLQ)
    {
      const csVector3& attnVec = light->GetAttenuationConstants ();
      attTex = parent->GetAttenuationTexture (attnVec);
    }
    else
      attTex = parent->GetAttenuationTexture (attnType);

    needUpdate = false;
  }
  variable->SetValue (attTex);
}
