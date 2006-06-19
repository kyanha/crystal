/*
    Copyright (C) 2001 by Jorrit Tyberghein
    Copyright (C) 2001 by W.C.A. Wijngaards

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
#include "csgeom/math3d.h"
#include "csutil/scanstr.h"
#include "csutil/cscolor.h"
#include "csutil/util.h"
#include "rainldr.h"
#include "imesh/object.h"
#include "iengine/mesh.h"
#include "iengine/engine.h"
#include "iutil/plugin.h"
#include "imesh/partsys.h"
#include "imesh/rain.h"
#include "ivideo/graph3d.h"
#include "csqint.h"
#include "csutil/csstring.h"
#include "iutil/object.h"
#include "iutil/vfs.h"
#include "iengine/material.h"
#include "iutil/objreg.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "iutil/document.h"
#include "imap/ldrctxt.h"
#include "ivaria/reporter.h"

CS_IMPLEMENT_PLUGIN

enum
{
  XMLTOKEN_COLOR = 1,
  XMLTOKEN_DROPSIZE,
  XMLTOKEN_FACTORY,
  XMLTOKEN_FALLSPEED,
  XMLTOKEN_LIGHTING,
  XMLTOKEN_MATERIAL,
  XMLTOKEN_MIXMODE,
  XMLTOKEN_NUMBER,
  XMLTOKEN_COLLDET,
  XMLTOKEN_BOX
};

SCF_IMPLEMENT_FACTORY (csRainFactoryLoader)
SCF_IMPLEMENT_FACTORY (csRainFactorySaver)
SCF_IMPLEMENT_FACTORY (csRainLoader)
SCF_IMPLEMENT_FACTORY (csRainSaver)

csRainFactoryLoader::csRainFactoryLoader (iBase* pParent) :
  scfImplementationType(this, pParent)
{
}

csRainFactoryLoader::~csRainFactoryLoader ()
{
}

bool csRainFactoryLoader::Initialize (iObjectRegistry* object_reg)
{
  csRainFactoryLoader::object_reg = object_reg;
  return true;
}

csPtr<iBase> csRainFactoryLoader::Parse (iDocumentNode* /*node*/,
	iStreamSource*, iLoaderContext*, iBase* /* context */)
{
  csRef<iMeshObjectType> type = csLoadPluginCheck<iMeshObjectType> (
  	object_reg, "crystalspace.mesh.object.rain");
  if (!type) return 0;
  csRef<iMeshObjectFactory> fact (type->NewFactory ());
  return csPtr<iBase> (fact);
}

//---------------------------------------------------------------------------
csRainFactorySaver::csRainFactorySaver (iBase* pParent):
  scfImplementationType(this, pParent)
{
}

csRainFactorySaver::~csRainFactorySaver ()
{
}

bool csRainFactorySaver::Initialize (iObjectRegistry* object_reg)
{
  csRainFactorySaver::object_reg = object_reg;
  return true;
}

bool csRainFactorySaver::WriteDown (iBase* /*obj*/, iDocumentNode* parent,
	iStreamSource*)
{
  //Nothing gets parsed in the loader, so nothing gets saved here!
  csRef<iDocumentNode> paramsNode = parent->CreateNodeBefore(CS_NODE_ELEMENT, 0);
  paramsNode->SetValue("params");
  return true;
}
//---------------------------------------------------------------------------

csRainLoader::csRainLoader (iBase* pParent):
  scfImplementationType(this, pParent)
{
}

csRainLoader::~csRainLoader ()
{
}

bool csRainLoader::Initialize (iObjectRegistry* object_reg)
{
  csRainLoader::object_reg = object_reg;
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);

  xmltokens.Register ("color", XMLTOKEN_COLOR);
  xmltokens.Register ("dropsize", XMLTOKEN_DROPSIZE);
  xmltokens.Register ("factory", XMLTOKEN_FACTORY);
  xmltokens.Register ("fallspeed", XMLTOKEN_FALLSPEED);
  xmltokens.Register ("lighting", XMLTOKEN_LIGHTING);
  xmltokens.Register ("material", XMLTOKEN_MATERIAL);
  xmltokens.Register ("mixmode", XMLTOKEN_MIXMODE);
  xmltokens.Register ("number", XMLTOKEN_NUMBER);
  xmltokens.Register ("box", XMLTOKEN_BOX);
  xmltokens.Register ("colldet", XMLTOKEN_COLLDET);
  return true;
}

csPtr<iBase> csRainLoader::Parse (iDocumentNode* node,
	iStreamSource*, iLoaderContext* ldr_context, iBase*)
{
  csRef<iMeshObject> mesh;
  csRef<iRainState> rainstate;

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_COLOR:
	{
	  csColor color;
	  if (!synldr->ParseColor (child, color))
	    return 0;
	  mesh->SetColor (color);
	}
	break;
      case XMLTOKEN_DROPSIZE:
	{
	  float dw, dh;
	  dw = child->GetAttributeValueAsFloat ("w");
	  dh = child->GetAttributeValueAsFloat ("h");
	  rainstate->SetDropSize (dw, dh);
	}
	break;
      case XMLTOKEN_BOX:
	{
	  csBox3 box;
	  if (!synldr->ParseBox (child, box))
	    return 0;
	  rainstate->SetBox (box.Min (), box.Max ());
	}
	break;
      case XMLTOKEN_FALLSPEED:
	{
	  csVector3 s;
	  if (!synldr->ParseVector (child, s))
	    return 0;
	  rainstate->SetFallSpeed (s);
	}
	break;
      case XMLTOKEN_FACTORY:
	{
	  const char* factname = child->GetContentsValue ();
	  iMeshFactoryWrapper* fact = ldr_context->FindMeshFactory (factname);
	  if (!fact)
	  {
      	    synldr->ReportError (
		"crystalspace.rainloader.parse.unknownfactory",
		child, "Couldn't find factory '%s'!", factname);
	    return 0;
	  }
	  mesh = fact->GetMeshObjectFactory ()->NewInstance ();
	  rainstate = SCF_QUERY_INTERFACE (mesh, iRainState);
	  if (!rainstate)
	  {
      	    synldr->ReportError (
		"crystalspace.rainloader.parse.badfactory",
		child, "Factory '%s' doesn't appear to be a rain factory!",
		factname);
	    return 0;
	  }
	}
	break;
      case XMLTOKEN_MATERIAL:
	{
	  const char* matname = child->GetContentsValue ();
          iMaterialWrapper* mat = ldr_context->FindMaterial (matname);
	  if (!mat)
	  {
      	    synldr->ReportError (
		"crystalspace.rainloader.parse.unknownmaterial",
		child, "Couldn't find material '%s'!", matname);
            return 0;
	  }
	  mesh->SetMaterialWrapper (mat);
	}
	break;
      case XMLTOKEN_MIXMODE:
	{
	  uint mode;
	  if (!synldr->ParseMixmode (child, mode))
	    return 0;
          mesh->SetMixMode (mode);
	}
	break;
      case XMLTOKEN_COLLDET:
        {
          bool do_cd;
	  if (!synldr->ParseBool (child, do_cd, true))
	    return 0;
          rainstate->SetCollisionDetection (do_cd);
        }
        break;
      case XMLTOKEN_LIGHTING:
        {
          bool do_lighting;
	  if (!synldr->ParseBool (child, do_lighting, true))
	    return 0;
          rainstate->SetLighting (do_lighting);
        }
        break;
      case XMLTOKEN_NUMBER:
        rainstate->SetParticleCount (child->GetContentsValueAsInt ());
        break;
      default:
	synldr->ReportBadToken (child);
	return 0;
    }
  }

  return csPtr<iBase> (mesh);
}

//---------------------------------------------------------------------------

csRainSaver::csRainSaver (iBase* pParent):
  scfImplementationType(this, pParent)
{
}

csRainSaver::~csRainSaver ()
{
}

bool csRainSaver::Initialize (iObjectRegistry* object_reg)
{
  csRainSaver::object_reg = object_reg;
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  return true;
}

bool csRainSaver::WriteDown (iBase* obj, iDocumentNode* parent,
	iStreamSource*)
{
  if (!parent) return false; //you never know...
  if (!obj)    return false; //you never know...
  
  csRef<iDocumentNode> paramsNode = parent->CreateNodeBefore(CS_NODE_ELEMENT, 0);
  paramsNode->SetValue("params");

  csRef<iParticleState> partstate = SCF_QUERY_INTERFACE (obj, iParticleState);
  csRef<iRainState> rainstate = SCF_QUERY_INTERFACE (obj, iRainState);
  csRef<iMeshObject> mesh = SCF_QUERY_INTERFACE (obj, iMeshObject);

  if ( partstate && rainstate && mesh )
  {
    //Writedown Factory tag
    iMeshFactoryWrapper* fact = mesh->GetFactory()->GetMeshFactoryWrapper ();
    if (fact)
    {
      const char* factname = fact->QueryObject()->GetName();
      if (factname && *factname)
      {
        csRef<iDocumentNode> factNode = paramsNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
        factNode->SetValue("factory");
        csRef<iDocumentNode> factnameNode = factNode->CreateNodeBefore(CS_NODE_TEXT, 0);
        factnameNode->SetValue(factname);
      }    
    }    

    //Writedown Color tag
    csColor col;
    mesh->GetColor(col);
    csRef<iDocumentNode> colorNode = paramsNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
    colorNode->SetValue("color");
    synldr->WriteColor(colorNode, col);

    //Writedown DropSize tag
    float dw, dh;
    rainstate->GetDropSize(dw, dh);
    csRef<iDocumentNode> dropsizeNode = paramsNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
    dropsizeNode->SetValue("dropsize");
    dropsizeNode->SetAttributeAsFloat("w", dw);
    dropsizeNode->SetAttributeAsFloat("h", dh);

    //Writedown Box tag
    csVector3 minBox, maxBox;
    rainstate->GetBox(minBox, maxBox);
    csBox3 box(minBox, maxBox);
    csRef<iDocumentNode> boxNode = paramsNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
    boxNode->SetValue("box");
    synldr->WriteBox(boxNode, box);

    //Writedown FallSpeed tag
    csVector3 fallspeed = rainstate->GetFallSpeed();
    csRef<iDocumentNode> fallspeedNode = paramsNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
    fallspeedNode->SetValue("fallspeed");
    synldr->WriteVector(fallspeedNode, fallspeed);

    //Writedown CollDet tag
    synldr->WriteBool(paramsNode,"colldet", rainstate->GetCollisionDetection(), true);

    //Writedown Material tag
    iMaterialWrapper* mat = mesh->GetMaterialWrapper();
    if (mat)
    {
      const char* matname = mat->QueryObject()->GetName();
      if (matname && *matname)
      {
        csRef<iDocumentNode> matNode = paramsNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
        matNode->SetValue("material");
        csRef<iDocumentNode> matnameNode = matNode->CreateNodeBefore(CS_NODE_TEXT, 0);
        matnameNode->SetValue(matname);
      }
    }

    //Writedown Mixmode tag
    int mixmode = partstate->GetMixMode();
    csRef<iDocumentNode> mixmodeNode = paramsNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
    mixmodeNode->SetValue("mixmode");
    synldr->WriteMixmode(mixmodeNode, mixmode, true);
	  
    //Writedown Lighting tag
    synldr->WriteBool(paramsNode, "lighting", rainstate->GetLighting(), true);

    //Writedown Number tag
    int number = rainstate->GetParticleCount();
    csRef<iDocumentNode> numberNode = paramsNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
    numberNode->SetValue("number");
    csRef<iDocumentNode> numberValueNode = numberNode->CreateNodeBefore(CS_NODE_TEXT, 0);
    numberValueNode->SetValueAsInt(number);
  }

  paramsNode=0;

  return true;
}

