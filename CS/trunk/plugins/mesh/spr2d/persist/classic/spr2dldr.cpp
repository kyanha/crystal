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
#include "cssys/sysfunc.h"
#include "csgeom/math3d.h"
#include "csutil/parser.h"
#include "csutil/scanstr.h"
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

CS_TOKEN_DEF_START
  CS_TOKEN_DEF (UVANIMATION)
  CS_TOKEN_DEF (FRAME)

  CS_TOKEN_DEF (COLORS)
  CS_TOKEN_DEF (FACTORY)
  CS_TOKEN_DEF (LIGHTING)
  CS_TOKEN_DEF (MATERIAL)
  CS_TOKEN_DEF (MIXMODE)
  CS_TOKEN_DEF (UV)
  CS_TOKEN_DEF (VERTICES)
  CS_TOKEN_DEF (ANIMATE)
CS_TOKEN_DEF_END

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

SCF_EXPORT_CLASS_TABLE (spr2dldr)
  SCF_EXPORT_CLASS (csSprite2DFactoryLoader,
  	"crystalspace.mesh.loader.factory.sprite.2d",
	"Crystal Space Sprite2D Mesh Factory Loader")
  SCF_EXPORT_CLASS (csSprite2DFactorySaver,
  	"crystalspace.mesh.saver.factory.sprite.2d",
	"Crystal Space Sprite2D Mesh Factory Saver")
  SCF_EXPORT_CLASS (csSprite2DLoader, "crystalspace.mesh.loader.sprite.2d",
    "Crystal Space Sprite2D Mesh Loader")
  SCF_EXPORT_CLASS (csSprite2DSaver, "crystalspace.mesh.saver.sprite.2d",
    "Crystal Space Sprite2D Mesh Saver")
SCF_EXPORT_CLASS_TABLE_END

static void ReportError (iReporter* reporter, const char* id,
	const char* description, ...)
{
  va_list arg;
  va_start (arg, description);

  if (reporter)
  {
    reporter->ReportV (CS_REPORTER_SEVERITY_ERROR, id, description, arg);
  }
  else
  {
    char buf[1024];
    vsprintf (buf, description, arg);
    csPrintf ("Error ID: %s\n", id);
    csPrintf ("Description: %s\n", buf);
  }
  va_end (arg);
}

csSprite2DFactoryLoader::csSprite2DFactoryLoader (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csSprite2DFactoryLoader::~csSprite2DFactoryLoader ()
{
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
  return true;
}

static void ParseAnim (csParser* parser, iReporter* reporter, 
		       iSprite2DFactoryState* spr2dLook, 
		       const char *animname, char *buf)
{
  CS_TOKEN_TABLE_START (commands)
    CS_TOKEN_TABLE (FRAME)
  CS_TOKEN_TABLE_END

  char* name;
  long cmd;
  char* params;
  float verts[200];
  int duration;

  iSprite2DUVAnimation *ani = spr2dLook->CreateUVAnimation ();
  ani->SetName (animname);

  while ((cmd = parser->GetObject (&buf, commands, &name, &params)) > 0)
  {
    if (!params)
    {
      ReportError (reporter,
		"crystalspace.sprite2dfactoryloader.parse.badtoken",
		"Expected FRAME instead of '%s'!", buf);
      return;
    }
    switch (cmd)
    {
      case CS_TOKEN_FRAME:
	{
	  int num;
          csScanStr (params, "%d, %F", &duration, &verts, &num);
	  iSprite2DUVAnimationFrame *frame = ani->CreateFrame (-1);
	  frame->SetFrameData (name, duration, num/2, verts);
	}
	break;
    }
  }
  if (cmd == CS_PARSERR_TOKENNOTFOUND)
  {
    ReportError (reporter,
		"crystalspace.sprite2dfactoryloader.parse.badtoken",
		"Token '%s' not found while parsing FRAME!",
		parser->GetLastOffender ());
    return;
  }
}

bool csSprite2DFactoryLoader::ParseAnim (iDocumentNode* node, iReporter* reporter, 
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

csPtr<iBase> csSprite2DFactoryLoader::Parse (const char* string,
	iLoaderContext* ldr_context, iBase* /* context */)
{
  CS_TOKEN_TABLE_START (commands)
    CS_TOKEN_TABLE (LIGHTING)
    CS_TOKEN_TABLE (MATERIAL)
    CS_TOKEN_TABLE (MIXMODE)
    CS_TOKEN_TABLE (UVANIMATION)
  CS_TOKEN_TABLE_END

  char* name;
  long cmd;
  char* params;
  char str[255];

  csParser* parser = ldr_context->GetParser ();

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
    ReportError (reporter,
		"crystalspace.sprite2dfactoryloader.setup.objecttype",
		"Could not load the sprite.2d mesh object plugin!");
    return NULL;
  }
  csRef<iMeshObjectFactory> fact (type->NewFactory ());
  csRef<iSprite2DFactoryState> spr2dLook (SCF_QUERY_INTERFACE (fact,
  	iSprite2DFactoryState));

  char* buf = (char*)string;
  while ((cmd = parser->GetObject (&buf, commands, &name, &params)) > 0)
  {
    if (!params)
    {
      ReportError (reporter,
		"crystalspace.sprite2dfactoryloader.parse.badformat",
		"Bad format while parsing sprite2d factory!");
      return NULL;
    }
    switch (cmd)
    {
      case CS_TOKEN_MATERIAL:
	{
          csScanStr (params, "%s", str);
          iMaterialWrapper* mat = ldr_context->FindMaterial (str);
	  if (!mat)
	  {
	    ReportError (reporter,
		"crystalspace.sprite2dfactoryloader.parse.unknownmaterial",
		"Couldn't find material named '%s'", str);
            return NULL;
	  }
	  spr2dLook->SetMaterialWrapper (mat);
	}
	break;
      case CS_TOKEN_LIGHTING:
        {
          bool do_lighting;
          csScanStr (params, "%b", &do_lighting);
          spr2dLook->SetLighting (do_lighting);
        }
	break;
      case CS_TOKEN_MIXMODE:
        {
	  uint mm;
	  if (!synldr->ParseMixmode (parser, params, mm))
	    return NULL;
          spr2dLook->SetMixMode (mm);
	}
	break;
      case CS_TOKEN_UVANIMATION:
	{
	  ::ParseAnim (parser, reporter, spr2dLook, name, params);
	}
        break;
    }
  }

  if (fact) fact->IncRef ();	// Prevent smart pointer release.
  return csPtr<iBase> (fact);
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
    return NULL;
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
            return NULL;
	  }
	  spr2dLook->SetMaterialWrapper (mat);
	}
	break;
      case XMLTOKEN_LIGHTING:
        {
          bool do_lighting;
	  if (!synldr->ParseBool (child, do_lighting, true))
	    return NULL;
          spr2dLook->SetLighting (do_lighting);
        }
	break;
      case XMLTOKEN_MIXMODE:
        {
	  uint mm;
	  if (!synldr->ParseMixmode (child, mm))
	    return NULL;
          spr2dLook->SetMixMode (mm);
	}
	break;
      case XMLTOKEN_UVANIMATION:
	if (!ParseAnim (child, reporter, spr2dLook,
		child->GetAttributeValue ("name")))
	  return NULL;
        break;
      default:
	synldr->ReportBadToken (child);
	return NULL;
    }
  }

  // Incref so that smart pointer doesn't delete.
  if (fact) fact->IncRef ();
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
}

bool csSprite2DFactorySaver::Initialize (iObjectRegistry* object_reg)
{
  csSprite2DFactorySaver::object_reg = object_reg;
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  return true;
}

#define MAXLINE 100 /* max number of chars per line... */

void csSprite2DFactorySaver::WriteDown (iBase* obj, iFile * file)
{
  csString str;
  csRef<iSprite2DFactoryState> state (
    SCF_QUERY_INTERFACE (obj, iSprite2DFactoryState));
  char buf[MAXLINE];

  sprintf(buf, "MATERIAL (%s)\n", state->GetMaterialWrapper()->
    QueryObject ()->GetName());
  str.Append(buf);
  if(state->GetMixMode() != CS_FX_COPY)
  {
    str.Append (synldr->MixmodeToText (state->GetMixMode(), 0, true));
  }
  sprintf(buf, "LIGHTING (%s)\n", state->HasLighting()?"true":"false");
  str.Append(buf);

  file->Write ((const char*)str, str.Length ());
}
//---------------------------------------------------------------------------

csSprite2DLoader::csSprite2DLoader (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csSprite2DLoader::~csSprite2DLoader ()
{
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

csPtr<iBase> csSprite2DLoader::Parse (const char* string,
				iLoaderContext* ldr_context, iBase*)
{
  CS_TOKEN_TABLE_START (commands)
    CS_TOKEN_TABLE (FACTORY)
    CS_TOKEN_TABLE (VERTICES)
    CS_TOKEN_TABLE (UV)
    CS_TOKEN_TABLE (MATERIAL)
    CS_TOKEN_TABLE (MIXMODE)
    CS_TOKEN_TABLE (COLORS)
    CS_TOKEN_TABLE (LIGHTING)
    CS_TOKEN_TABLE (ANIMATE)
  CS_TOKEN_TABLE_END

  char* name;
  long cmd;
  char* params;
  char str[255];

  csRef<iMeshObject> mesh;
  csRef<iSprite2DState> spr2dLook;
  csColoredVertices* verts = NULL;

  csParser* parser = ldr_context->GetParser ();

  char* buf = (char*)string;
  while ((cmd = parser->GetObject (&buf, commands, &name, &params)) > 0)
  {
    if (!params)
    {
      ReportError (reporter,
		"crystalspace.sprite2dloader.parse.badformat",
		"Bad format while parsing sprite2d!");
      return NULL;
    }
    switch (cmd)
    {
      case CS_TOKEN_FACTORY:
	{
          csScanStr (params, "%s", str);
	  iMeshFactoryWrapper* fact = ldr_context->FindMeshFactory (str);
	  if (!fact)
	  {
      	    ReportError (reporter,
		"crystalspace.sprite2dloader.parse.unknownfactory",
		"Couldn't find factory '%s'!", str);
	    return NULL;
	  }
	  mesh = fact->GetMeshObjectFactory ()->NewInstance ();
          spr2dLook = SCF_QUERY_INTERFACE (mesh, iSprite2DState);
	  verts = &(spr2dLook->GetVertices ());
	}
	break;
      case CS_TOKEN_MATERIAL:
	{
          csScanStr (params, "%s", str);
          iMaterialWrapper* mat = ldr_context->FindMaterial (str);
	  if (!mat)
	  {
      	    ReportError (reporter,
		"crystalspace.sprite2dloader.parse.unknownmaterial",
		"Couldn't find material '%s'!", str);
            return NULL;
	  }
	  spr2dLook->SetMaterialWrapper (mat);
	}
	break;
      case CS_TOKEN_MIXMODE:
        {
	  uint mm;
	  if (!synldr->ParseMixmode (parser, params, mm))
	    return NULL;
          spr2dLook->SetMixMode (mm);
	}
	break;
      case CS_TOKEN_VERTICES:
        {
          float list[100];
	  int num;
          csScanStr (params, "%F", list, &num);
	  num /= 2;
	  verts->SetLength (num);
	  int i;
          for (i = 0 ; i < num ; i++)
	  {
	    (*verts)[i].pos.x = list[i*2+0];
	    (*verts)[i].pos.y = list[i*2+1];
	    (*verts)[i].color_init.Set (0, 0, 0);
	    (*verts)[i].color.Set (0, 0, 0);
	  }
        }
        break;
      case CS_TOKEN_UV:
        {
          float list[100];
	  int num;
          csScanStr (params, "%F", list, &num);
	  num /= 2;
	  verts->SetLength (num);
	  int i;
          for (i = 0 ; i < num ; i++)
	  {
	    (*verts)[i].u = list[i*2+0];
	    (*verts)[i].v = list[i*2+1];
	  }
        }
        break;
      case CS_TOKEN_LIGHTING:
        {
          bool do_lighting;
          csScanStr (params, "%b", &do_lighting);
          spr2dLook->SetLighting (do_lighting);
        }
        break;
      case CS_TOKEN_COLORS:
        {
          float list[100];
	  int num;
          csScanStr (params, "%F", list, &num);
	  num /= 3;
	  verts->SetLength (num);
	  int i;
          for (i = 0 ; i < num ; i++)
	  {
	    (*verts)[i].color_init.red = list[i*3+0];
	    (*verts)[i].color_init.green = list[i*3+1];
	    (*verts)[i].color_init.blue = list[i*3+2];
	  }
        }
        break;
      case CS_TOKEN_ANIMATE:
        {
          bool loop;
	  int type;
          csScanStr (params, "%s, %d, %b", str, &type, &loop);
	  iSprite2DUVAnimation *ani = spr2dLook->GetUVAnimation (str);
	  if (ani)
	    spr2dLook->SetUVAnimation (str, type, loop);
	  else
    	  {
	    ReportError (reporter,
		"crystalspace.sprite2dloader.parse.uvanim",
		"UVAnimation '%s' not found!", str);
	    return NULL;
	  }
        }
        break;
    }
  }

  if (mesh) mesh->IncRef ();	// Prevent smart pointer release.
  return csPtr<iBase> (mesh);
}

csPtr<iBase> csSprite2DLoader::Parse (iDocumentNode* node,
				iLoaderContext* ldr_context, iBase*)
{
  csRef<iMeshObject> mesh;
  csRef<iSprite2DState> spr2dLook;
  csColoredVertices* verts = NULL;
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
	    return NULL;
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
            return NULL;
	  }
	  spr2dLook->SetMaterialWrapper (mat);
	}
	break;
      case XMLTOKEN_MIXMODE:
        {
	  uint mm;
	  if (!synldr->ParseMixmode (child, mm))
	    return NULL;
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
	    return NULL;
          spr2dLook->SetLighting (do_lighting);
        }
        break;
      case XMLTOKEN_ANIMATE:
        {
          bool loop = false;
	  int type = 0;
	  const char* animname = child->GetAttributeValue ("name");
	  csRef<iDocumentNode> loopnode = child->GetNode ("loop");
	  if (loopnode) synldr->ParseBool (loopnode, loop, true);
	  csRef<iDocumentNode> typenode = child->GetNode ("type");
	  if (typenode) type = typenode->GetContentsValueAsInt ();
	  iSprite2DUVAnimation *ani = spr2dLook->GetUVAnimation (animname);
	  if (ani)
	    spr2dLook->SetUVAnimation (animname, type, loop);
	  else
    	  {
	    synldr->ReportError (
		"crystalspace.sprite2dloader.parse.uvanim",
		child, "UVAnimation '%s' not found!", animname);
	    return NULL;
	  }
        }
        break;
      default:
	synldr->ReportBadToken (child);
	return NULL;
    }
  }

  // Incref so that smart pointer doesn't remove mesh.
  if (mesh) mesh->IncRef ();
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
}

bool csSprite2DSaver::Initialize (iObjectRegistry* object_reg)
{
  csSprite2DSaver::object_reg = object_reg;
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  return true;
}

void csSprite2DSaver::WriteDown (iBase* obj, iFile *file)
{
  csString str;
  csRef<iFactory> fact (SCF_QUERY_INTERFACE (this, iFactory));
  csRef<iSprite2DState> state (SCF_QUERY_INTERFACE (obj, iSprite2DState));
  char buf[MAXLINE];
  char name[MAXLINE];

  csFindReplace(name, fact->QueryDescription (), "Saver", "Loader", MAXLINE);
  sprintf(buf, "FACTORY ('%s')\n", name);
  str.Append(buf);

  sprintf(buf, "MATERIAL (%s)\n", state->GetMaterialWrapper()->
    QueryObject ()->GetName());
  str.Append(buf);
  sprintf(buf, "LIGHTING (%s)\n", state->HasLighting()?"true":"false");
  str.Append(buf);
  if(state->GetMixMode() != CS_FX_COPY)
  {
    str.Append (synldr->MixmodeToText (state->GetMixMode(), 0, true));
  }

  csColoredVertices& vs = state->GetVertices();
  int i;
  str.Append("VERTICES(");
  for(i=0; vs.Length(); i++)
  {
    sprintf(buf, "%g,%g%s", vs[i].pos.x, vs[i].pos.y,
      (i==vs.Length()-1)?"":", ");
    str.Append(buf);
  }
  str.Append(")\n");

  str.Append("UV(");
  for(i=0; vs.Length(); i++)
  {
    sprintf(buf, "%g,%g%s", vs[i].u, vs[i].v, (i==vs.Length()-1)?"":", ");
    str.Append(buf);
  }
  str.Append(")\n");

  str.Append("COLORS(");
  for(i=0; vs.Length(); i++)
  {
    sprintf(buf, "%g,%g,%g%s", vs[i].color_init.red, vs[i].color_init.green,
      vs[i].color_init.blue, (i==vs.Length()-1)?"":", ");
    str.Append(buf);
  }
  str.Append(")\n");

  file->Write ((const char*)str, str.Length ());
}

