/*
    Copyright (C) 2003 by Jorrit Tyberghein
	      (C) 2003 by Frank Richter

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

#include "ivideo/graph3d.h"
#include "iutil/document.h"
#include "iutil/objreg.h"
#include "iengine/engine.h"
#include "iengine/material.h"
#include "iengine/texture.h"

#include "cstool/prdots.h"
#include "cstool/prwater.h"
#include "cstool/prfire.h"
#include "cstool/prplasma.h"

#include "stdproctex.h"

// Plugin stuff

SCF_IMPLEMENT_IBASE(csBaseProctexLoader);
  SCF_IMPLEMENTS_INTERFACE(iLoaderPlugin);
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iComponent);
SCF_IMPLEMENT_IBASE_END;

SCF_IMPLEMENT_EMBEDDED_IBASE (csBaseProctexLoader::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY(csPtDotsLoader);
SCF_IMPLEMENT_FACTORY(csPtFireLoader);
SCF_IMPLEMENT_FACTORY(csPtPlasmaLoader);
SCF_IMPLEMENT_FACTORY(csPtWaterLoader);

SCF_EXPORT_CLASS_TABLE (stdpt)
  SCF_EXPORT_CLASS_DEP (csPtDotsLoader, 
    "crystalspace.proctex.loader.dots",
    "'Dots' procedural texture loader", 
    "crystalspace.graphics3d., ")
  SCF_EXPORT_CLASS_DEP (csPtFireLoader, 
    "crystalspace.proctex.loader.fire",
    "'Fire' procedural texture loader", 
    "crystalspace.graphics3d., ")
  SCF_EXPORT_CLASS_DEP (csPtWaterLoader, 
    "crystalspace.proctex.loader.water",
    "'Water' procedural texture loader", 
    "crystalspace.graphics3d., ")
  SCF_EXPORT_CLASS_DEP (csPtPlasmaLoader, 
    "crystalspace.proctex.loader.plasma",
    "'Plasma' procedural texture loader", 
    "crystalspace.graphics3d., ")
SCF_EXPORT_CLASS_TABLE_END

// Base for all PT loaders

csBaseProctexLoader::csBaseProctexLoader(iBase *p)
{
  SCF_CONSTRUCT_IBASE (p);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent);
}

csBaseProctexLoader::~csBaseProctexLoader ()
{
}

bool csBaseProctexLoader::Initialize(iObjectRegistry *object_reg)
{
  csBaseProctexLoader::object_reg = object_reg;

  return true;
}

csPtr<iBase> csBaseProctexLoader::PrepareProcTex (csProcTexture* pt,
    const char* name)
{
  csRef<iEngine> Engine = CS_QUERY_REGISTRY(object_reg, iEngine);
  if (!Engine) return NULL;

  csRef<iGraphics3D> G3D = CS_QUERY_REGISTRY(object_reg, iGraphics3D);

  csRef<iMaterialWrapper> mw = pt->Initialize (object_reg, 
    Engine,
    G3D ? G3D->GetTextureManager () : NULL,
    name);
  mw->QueryObject ()->ObjAdd (pt);
  csRef<iTextureWrapper> tw = pt->GetTextureWrapper ();
  return csPtr<iBase> (tw);
}

// 'Dots' loader.

csPtDotsLoader::csPtDotsLoader(iBase *p) : csBaseProctexLoader(p)
{
}

csPtr<iBase> csPtDotsLoader::Parse (iDocumentNode* node, 
				    iLoaderContext* ldr_context,
  				    iBase* context)
{
  csRef<csProcTexture> pt = csPtr<csProcTexture> (new csProcDots ());
  return PrepareProcTex (pt, node->GetAttributeValue ("name"));
}

// 'Fire' loader.

csPtFireLoader::csPtFireLoader(iBase *p) : csBaseProctexLoader(p)
{
}

csPtr<iBase> csPtFireLoader::Parse (iDocumentNode* node, 
				    iLoaderContext* ldr_context,
  				    iBase* context)
{
  csRef<csProcTexture> pt = csPtr<csProcTexture> (new csProcFire ());
  return PrepareProcTex (pt, node->GetAttributeValue ("name"));
}

// 'Water' loader.

csPtWaterLoader::csPtWaterLoader(iBase *p) : csBaseProctexLoader(p)
{
}

csPtr<iBase> csPtWaterLoader::Parse (iDocumentNode* node, 
				    iLoaderContext* ldr_context,
  				    iBase* context)
{
  csRef<csProcTexture> pt = csPtr<csProcTexture> (new csProcWater ());
  return PrepareProcTex (pt, node->GetAttributeValue ("name"));
}

// 'Plasma' loader.

csPtPlasmaLoader::csPtPlasmaLoader(iBase *p) : csBaseProctexLoader(p)
{
}

csPtr<iBase> csPtPlasmaLoader::Parse (iDocumentNode* node, 
				      iLoaderContext* ldr_context,
  				      iBase* context)
{
  csRef<csProcTexture> pt = csPtr<csProcTexture> (new csProcPlasma ());
  return PrepareProcTex (pt, node->GetAttributeValue ("name"));
}

