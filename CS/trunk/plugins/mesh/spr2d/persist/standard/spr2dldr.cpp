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
#include "csutil/sysfunc.h"
#include "csgeom/math3d.h"
#include "csutil/scanstr.h"
#include "csutil/util.h"
#include "spr2dldr.h"
#include "imesh/object.h"
#include "iengine/mesh.h"
#include "iengine/engine.h"
#include "iutil/plugin.h"
#include "iutil/document.h"
#include "imesh/sprite2d.h"
#include "ivideo/graph3d.h"
#include "qint.h"
#include "csutil/csstring.h"
#include "iutil/object.h"
#include "iutil/vfs.h"
#include "iengine/material.h"
#include "ivaria/reporter.h"
#include "iutil/objreg.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "imap/ldrctxt.h"

CS_IMPLEMENT_PLUGIN

enum
{
  XMLTOKEN_UVANIMATION,
  XMLTOKEN_FRAME,
  XMLTOKEN_DURATION,

  XMLTOKEN_COLOR,
  XMLTOKEN_FACTORY,
  XMLTOKEN_LIGHTING,
  XMLTOKEN_MATERIAL,
  XMLTOKEN_MIXMODE,
  XMLTOKEN_UV,
  XMLTOKEN_V,
  XMLTOKEN_ANIMATE
};

SCF_IMPLEMENT_IBASE (csSprite2DFactoryLoader)
  SCF_IMPLEMENTS_INTERFACE (iLoaderPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite2DFactoryLoader::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csSprite2DFactorySaver)
  SCF_IMPLEMENTS_INTERFACE (iSaverPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite2DFactorySaver::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csSprite2DLoader)
  SCF_IMPLEMENTS_INTERFACE (iLoaderPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite2DLoader::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csSprite2DSaver)
  SCF_IMPLEMENTS_INTERFACE (iSaverPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite2DSaver::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csSprite2DFactoryLoader)
SCF_IMPLEMENT_FACTORY (csSprite2DFactorySaver)
SCF_IMPLEMENT_FACTORY (csSprite2DLoader)
SCF_IMPLEMENT_FACTORY (csSprite2DSaver)


csSprite2DFactoryLoader::csSprite2DFactoryLoader (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csSprite2DFactoryLoader::~csSprite2DFactoryLoader ()
{
  SCF_DESTRUCT_EMBEDDED_IBASE(scfiComponent);
  SCF_DESTRUCT_IBASE ();
}

bool csSprite2DFactoryLoader::Initialize (iObjectRegistry* object_reg)
{
  csSprite2DFactoryLoader::object_reg = object_reg;
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);

  xmltokens.Register ("lighting", XMLTOKEN_LIGHTING);
  xmltokens.Register ("material", XMLTOKEN_MATERIAL);
  xmltokens.Register ("mixmode", XMLTOKEN_MIXMODE);
  xmltokens.Register ("uvanimation", XMLTOKEN_UVANIMATION);
  xmltokens.Register ("frame", XMLTOKEN_FRAME);
  xmltokens.Register ("duration", XMLTOKEN_DURATION);
  xmltokens.Register ("v", XMLTOKEN_V);
  return true;
}

bool csSprite2DFactoryLoader::ParseAnim (iDocumentNode* node,
	iReporter*, 
	iSprite2DFactoryState* spr2dLook, 
	const char *animname)
{
  int maxv = 200;
  float* verts = new float[maxv];
  int duration;

  iSprite2DUVAnimation *ani = spr2dLook->CreateUVAnimation ();
  ani->SetName (animname);

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_FRAME:
	{
	  duration = 1;
	  int numv = 0;
	  csRef<iDocumentNodeIterator> child_it = child->GetNodes ();
	  while (child_it->HasNext ())
	  {
	    csRef<iDocumentNode> childchild = child_it->Next ();
	    if (childchild->GetType () != CS_NODE_ELEMENT) continue;
	    const char* child_value = childchild->GetValue ();
	    csStringID id = xmltokens.Request (child_value);
	    switch (id)
	    {
	      case XMLTOKEN_DURATION:
		duration = childchild->GetContentsValueAsInt ();
		break;
	      case XMLTOKEN_V:
		verts[numv++] = childchild->GetAttributeValueAsFloat ("u");
		verts[numv++] = childchild->GetAttributeValueAsFloat ("v");
		if (numv >= maxv)
		{
		  maxv += 200;
		  float* newverts = new float[maxv];
		  memcpy (newverts, verts, numv*sizeof (float));
		  delete [] verts;
		  verts = newverts;
		}
		break;
	      default:
	        synldr->ReportBadToken (childchild);
		delete[] verts;
		return false;
	    }
	  }
	  iSprite2DUVAnimationFrame *frame = ani->CreateFrame (-1);
	  frame->SetFrameData (child->GetAttributeValue ("name"), duration, numv/2, verts);
	}
	break;
      default:
	synldr->ReportBadToken (child);
	delete[] verts;
	return false;
    }
  }
  delete[] verts;
  return true;
}

csPtr<iBase> csSprite2DFactoryLoader::Parse (iDocumentNode* node,
	iLoaderContext* ldr_context, iBase* /* context */)
{
  csRef<iPluginManager> plugin_mgr (CS_QUERY_REGISTRY (object_reg,
  	iPluginManager));
  csRef<iMeshObjectType> type (CS_QUERY_PLUGIN_CLASS (plugin_mgr,
  	"crystalspace.mesh.object.sprite.2d", iMeshObjectType));
  if (!type)
  {
    type = CS_LOAD_PLUGIN (plugin_mgr, "crystalspace.mesh.object.sprite.2d",
    	iMeshObjectType);
  }
  if (!type)
  {
    synldr->ReportError (
		"crystalspace.sprite2dfactoryloader.setup.objecttype",
		node, "Could not load the sprite.2d mesh object plugin!");
    return 0;
  }
  csRef<iMeshObjectFactory> fact (type->NewFactory ());
  csRef<iSprite2DFactoryState> spr2dLook (
  	SCF_QUERY_INTERFACE (fact, iSprite2DFactoryState));

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_MATERIAL:
	{
	  const char* matname = child->GetContentsValue ();
          iMaterialWrapper* mat = ldr_context->FindMaterial (matname);
	  if (!mat)
	  {
	    synldr->ReportError (
		"crystalspace.sprite2dfactoryloader.parse.unknownmaterial",
		child, "Couldn't find material named '%s'", matname);
            return 0;
	  }
	  spr2dLook->SetMaterialWrapper (mat);
	}
	break;
      case XMLTOKEN_LIGHTING:
        {
          bool do_lighting;
	  if (!synldr->ParseBool (child, do_lighting, true))
	    return 0;
          spr2dLook->SetLighting (do_lighting);
        }
	break;
      case XMLTOKEN_MIXMODE:
        {
	  uint mm;
	  if (!synldr->ParseMixmode (child, mm))
	    return 0;
          spr2dLook->SetMixMode (mm);
	}
	break;
      case XMLTOKEN_UVANIMATION:
	if (!ParseAnim (child, reporter, spr2dLook,
		child->GetAttributeValue ("name")))
	  return 0;
        break;
      default:
	synldr->ReportBadToken (child);
	return 0;
    }
  }

  return csPtr<iBase> (fact);
}

//---------------------------------------------------------------------------
csSprite2DFactorySaver::csSprite2DFactorySaver (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csSprite2DFactorySaver::~csSprite2DFactorySaver ()
{
  SCF_DESTRUCT_EMBEDDED_IBASE(scfiComponent);
  SCF_DESTRUCT_IBASE ();
}

bool csSprite2DFactorySaver::Initialize (iObjectRegistry* object_reg)
{
  csSprite2DFactorySaver::object_reg = object_reg;
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  return true;
}

void csSprite2DFactorySaver::WriteDown (iBase*, iFile*)
{
}

//---------------------------------------------------------------------------

csSprite2DLoader::csSprite2DLoader (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csSprite2DLoader::~csSprite2DLoader ()
{
  SCF_DESTRUCT_EMBEDDED_IBASE(scfiComponent);
  SCF_DESTRUCT_IBASE ();
}

bool csSprite2DLoader::Initialize (iObjectRegistry* object_reg)
{
  csSprite2DLoader::object_reg = object_reg;
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);

  xmltokens.Register ("color", XMLTOKEN_COLOR);
  xmltokens.Register ("factory", XMLTOKEN_FACTORY);
  xmltokens.Register ("lighting", XMLTOKEN_LIGHTING);
  xmltokens.Register ("material", XMLTOKEN_MATERIAL);
  xmltokens.Register ("mixmode", XMLTOKEN_MIXMODE);
  xmltokens.Register ("uv", XMLTOKEN_UV);
  xmltokens.Register ("v", XMLTOKEN_V);
  xmltokens.Register ("animate", XMLTOKEN_ANIMATE);
  return true;
}

csPtr<iBase> csSprite2DLoader::Parse (iDocumentNode* node,
				iLoaderContext* ldr_context, iBase*)
{
  csRef<iMeshObject> mesh;
  csRef<iSprite2DState> spr2dLook;
  csColoredVertices* verts = 0;
  int vnum = 0;
  int uvnum = 0;
  int colnum = 0;

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_FACTORY:
	{
	  const char* factname = child->GetContentsValue ();
	  iMeshFactoryWrapper* fact = ldr_context->FindMeshFactory (factname);
	  if (!fact)
	  {
      	    synldr->ReportError (
		"crystalspace.sprite2dloader.parse.unknownfactory",
		child, "Couldn't find factory '%s'!", factname);
	    return 0;
	  }
	  mesh = fact->GetMeshObjectFactory ()->NewInstance ();
          spr2dLook = SCF_QUERY_INTERFACE (mesh, iSprite2DState);
	  verts = &(spr2dLook->GetVertices ());
	}
	break;
      case XMLTOKEN_MATERIAL:
	{
	  const char* matname = child->GetContentsValue ();
          iMaterialWrapper* mat = ldr_context->FindMaterial (matname);
	  if (!mat)
	  {
      	    synldr->ReportError (
		"crystalspace.sprite2dloader.parse.unknownmaterial",
		child, "Couldn't find material '%s'!", matname);
            return 0;
	  }
	  spr2dLook->SetMaterialWrapper (mat);
	}
	break;
      case XMLTOKEN_MIXMODE:
        {
	  uint mm;
	  if (!synldr->ParseMixmode (child, mm))
	    return 0;
          spr2dLook->SetMixMode (mm);
	}
	break;
      case XMLTOKEN_V:
        {
	  float x = child->GetAttributeValueAsFloat ("x");
	  float y = child->GetAttributeValueAsFloat ("y");
	  vnum++;
	  if (vnum > uvnum && vnum > colnum)
	    verts->SetLength (vnum);
	  (*verts)[vnum-1].pos.x = x;
	  (*verts)[vnum-1].pos.y = y;
	  (*verts)[vnum-1].color_init.Set (0, 0, 0);
	  (*verts)[vnum-1].color.Set (0, 0, 0);
        }
        break;
      case XMLTOKEN_UV:
        {
	  float u = child->GetAttributeValueAsFloat ("u");
	  float v = child->GetAttributeValueAsFloat ("v");
	  uvnum++;
	  if (uvnum > vnum && uvnum > colnum)
	    verts->SetLength (uvnum);
	  (*verts)[uvnum-1].u = u;
	  (*verts)[uvnum-1].v = v;
        }
        break;
      case XMLTOKEN_COLOR:
        {
	  float r = child->GetAttributeValueAsFloat ("red");
	  float g = child->GetAttributeValueAsFloat ("green");
	  float b = child->GetAttributeValueAsFloat ("blue");
	  colnum++;
	  if (colnum > vnum && colnum > uvnum)
	    verts->SetLength (colnum);
	  (*verts)[colnum-1].color_init.red = r;
	  (*verts)[colnum-1].color_init.green = g;
	  (*verts)[colnum-1].color_init.blue = b;
        }
        break;
      case XMLTOKEN_LIGHTING:
        {
          bool do_lighting;
	  if (!synldr->ParseBool (child, do_lighting, true))
	    return 0;
          spr2dLook->SetLighting (do_lighting);
        }
        break;
      case XMLTOKEN_ANIMATE:
        {
          bool loop = false;
	  int timing = 0;
	  const char* animname = child->GetAttributeValue ("name");
	  csRef<iDocumentNode> loopnode = child->GetNode ("loop");
	  if (loopnode) synldr->ParseBool (loopnode, loop, true);
	  csRef<iDocumentNode> timingnode = child->GetNode ("timing");
	  if (timingnode) timing = timingnode->GetContentsValueAsInt ();
	  iSprite2DUVAnimation *ani = spr2dLook->GetUVAnimation (animname);
	  if (ani)
	    spr2dLook->SetUVAnimation (animname, timing, loop);
	  else
    	  {
	    synldr->ReportError (
		"crystalspace.sprite2dloader.parse.uvanim",
		child, "UVAnimation '%s' not found!", animname);
	    return 0;
	  }
        }
        break;
      default:
	synldr->ReportBadToken (child);
	return 0;
    }
  }

  return csPtr<iBase> (mesh);
}

//---------------------------------------------------------------------------

csSprite2DSaver::csSprite2DSaver (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csSprite2DSaver::~csSprite2DSaver ()
{
  SCF_DESTRUCT_EMBEDDED_IBASE(scfiComponent);
  SCF_DESTRUCT_IBASE ();
}

bool csSprite2DSaver::Initialize (iObjectRegistry* object_reg)
{
  csSprite2DSaver::object_reg = object_reg;
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  return true;
}

void csSprite2DSaver::WriteDown (iBase*, iFile*)
{
}

