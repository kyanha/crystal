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
#include "csutil/parser.h"
#include "csutil/scanstr.h"
#include "csutil/cscolor.h"
#include "spirldr.h"
#include "imesh/object.h"
#include "iengine/mesh.h"
#include "iengine/engine.h"
#include "iutil/plugin.h"
#include "imesh/partsys.h"
#include "imesh/spiral.h"
#include "ivideo/graph3d.h"
#include "qint.h"
#include "csutil/csstring.h"
#include "iutil/object.h"
#include "iutil/document.h"
#include "iengine/material.h"
#include "iutil/objreg.h"
#include "iutil/vfs.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "imap/ldrctxt.h"
#include "ivaria/reporter.h"

CS_IMPLEMENT_PLUGIN

CS_TOKEN_DEF_START
  CS_TOKEN_DEF (COLOR)
  CS_TOKEN_DEF (FACTORY)
  CS_TOKEN_DEF (MATERIAL)
  CS_TOKEN_DEF (MIXMODE)
  CS_TOKEN_DEF (NUMBER)
  CS_TOKEN_DEF (SOURCE)
CS_TOKEN_DEF_END

enum
{
  XMLTOKEN_COLOR = 1,
  XMLTOKEN_FACTORY,
  XMLTOKEN_MATERIAL,
  XMLTOKEN_MIXMODE,
  XMLTOKEN_NUMBER,
  XMLTOKEN_SOURCE
};

SCF_IMPLEMENT_IBASE (csSpiralFactoryLoader)
  SCF_IMPLEMENTS_INTERFACE (iLoaderPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSpiralFactoryLoader::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csSpiralFactorySaver)
  SCF_IMPLEMENTS_INTERFACE (iSaverPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSpiralFactorySaver::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csSpiralLoader)
  SCF_IMPLEMENTS_INTERFACE (iLoaderPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSpiralLoader::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csSpiralSaver)
  SCF_IMPLEMENTS_INTERFACE (iSaverPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSpiralSaver::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csSpiralFactoryLoader)
SCF_IMPLEMENT_FACTORY (csSpiralFactorySaver)
SCF_IMPLEMENT_FACTORY (csSpiralLoader)
SCF_IMPLEMENT_FACTORY (csSpiralSaver)

SCF_EXPORT_CLASS_TABLE (spirldr)
  SCF_EXPORT_CLASS (csSpiralFactoryLoader,
  	"crystalspace.mesh.loader.factory.spiral",
	"Crystal Space Spiral Factory Loader")
  SCF_EXPORT_CLASS (csSpiralFactorySaver,
    "crystalspace.mesh.saver.factory.spiral",
    "Crystal Space Spiral Factory Saver")
  SCF_EXPORT_CLASS (csSpiralLoader, "crystalspace.mesh.loader.spiral",
    "Crystal Space Spiral Mesh Loader")
  SCF_EXPORT_CLASS (csSpiralSaver, "crystalspace.mesh.saver.spiral",
    "Crystal Space Spiral Mesh Saver")
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

csSpiralFactoryLoader::csSpiralFactoryLoader (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  plugin_mgr = NULL;
}

csSpiralFactoryLoader::~csSpiralFactoryLoader ()
{
  SCF_DEC_REF (plugin_mgr);
}

bool csSpiralFactoryLoader::Initialize (iObjectRegistry* object_reg)
{
  csSpiralFactoryLoader::object_reg = object_reg;
  plugin_mgr = CS_QUERY_REGISTRY (object_reg, iPluginManager);
  return true;
}

iBase* csSpiralFactoryLoader::Parse (const char* /*string*/,
	iLoaderContext*, iBase* /* context */)
{
  iMeshObjectType* type = CS_QUERY_PLUGIN_CLASS (plugin_mgr,
  	"crystalspace.mesh.object.spiral", iMeshObjectType);
  if (!type)
  {
    type = CS_LOAD_PLUGIN (plugin_mgr, "crystalspace.mesh.object.spiral",
    	iMeshObjectType);
    printf ("Load TYPE plugin crystalspace.mesh.object.spiral\n");
  }
  iMeshObjectFactory* fact = type->NewFactory ();
  type->DecRef ();
  return fact;
}

iBase* csSpiralFactoryLoader::Parse (iDocumentNode* /*node*/,
	iLoaderContext*, iBase* /* context */)
{
  iMeshObjectType* type = CS_QUERY_PLUGIN_CLASS (plugin_mgr,
  	"crystalspace.mesh.object.spiral", iMeshObjectType);
  if (!type)
  {
    type = CS_LOAD_PLUGIN (plugin_mgr, "crystalspace.mesh.object.spiral",
    	iMeshObjectType);
    printf ("Load TYPE plugin crystalspace.mesh.object.spiral\n");
  }
  iMeshObjectFactory* fact = type->NewFactory ();
  type->DecRef ();
  return fact;
}

//---------------------------------------------------------------------------
csSpiralFactorySaver::csSpiralFactorySaver (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  plugin_mgr = NULL;
}

csSpiralFactorySaver::~csSpiralFactorySaver ()
{
  SCF_DEC_REF (plugin_mgr);
}

bool csSpiralFactorySaver::Initialize (iObjectRegistry* object_reg)
{
  csSpiralFactorySaver::object_reg = object_reg;
  plugin_mgr = CS_QUERY_REGISTRY (object_reg, iPluginManager);
  return true;
}

#define MAXLINE 100 /* max number of chars per line... */

void csSpiralFactorySaver::WriteDown (iBase* /*obj*/, iFile * /*file*/)
{
  // nothing to do
}
//---------------------------------------------------------------------------
csSpiralLoader::csSpiralLoader (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  plugin_mgr = NULL;
  synldr = NULL;
  reporter = NULL;
}

csSpiralLoader::~csSpiralLoader ()
{
  SCF_DEC_REF (plugin_mgr);
  SCF_DEC_REF (synldr);
  SCF_DEC_REF (reporter);
}

bool csSpiralLoader::Initialize (iObjectRegistry* object_reg)
{
  csSpiralLoader::object_reg = object_reg;
  plugin_mgr = CS_QUERY_REGISTRY (object_reg, iPluginManager);
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);

  xmltokens.Register ("color", XMLTOKEN_COLOR);
  xmltokens.Register ("factory", XMLTOKEN_FACTORY);
  xmltokens.Register ("material", XMLTOKEN_MATERIAL);
  xmltokens.Register ("mixmode", XMLTOKEN_MIXMODE);
  xmltokens.Register ("number", XMLTOKEN_NUMBER);
  xmltokens.Register ("source", XMLTOKEN_SOURCE);
  return true;
}

iBase* csSpiralLoader::Parse (const char* string,
	iLoaderContext* ldr_context, iBase*)
{
  CS_TOKEN_TABLE_START (commands)
    CS_TOKEN_TABLE (MATERIAL)
    CS_TOKEN_TABLE (FACTORY)
    CS_TOKEN_TABLE (SOURCE)
    CS_TOKEN_TABLE (COLOR)
    CS_TOKEN_TABLE (NUMBER)
    CS_TOKEN_TABLE (MIXMODE)
  CS_TOKEN_TABLE_END

  char* name;
  long cmd;
  char* params;
  char str[255];

  iMeshObject* mesh = NULL;
  iParticleState* partstate = NULL;
  iSpiralState* spiralstate = NULL;

  csParser* parser = ldr_context->GetParser ();

  char* buf = (char*)string;
  while ((cmd = parser->GetObject (&buf, commands, &name, &params)) > 0)
  {
    if (!params)
    {
      // @@@ Error handling!
      if (partstate) partstate->DecRef ();
      if (spiralstate) spiralstate->DecRef ();
      return NULL;
    }
    switch (cmd)
    {
      case CS_TOKEN_COLOR:
	{
	  csColor color;
	  csScanStr(params, "%f,%f,%f", &color.red, &color.green, &color.blue);
	  partstate->SetColor (color);
	}
	break;
      case CS_TOKEN_SOURCE:
	{
	  csVector3 s;
	  csScanStr (params, "%f,%f,%f", &s.x, &s.y, &s.z);
	  spiralstate->SetSource (s);
	}
	break;
      case CS_TOKEN_FACTORY:
	{
          csScanStr (params, "%s", str);
	  iMeshFactoryWrapper* fact = ldr_context->FindMeshFactory (str);
	  if (!fact)
	  {
	    // @@@ Error handling!
	    if (partstate) partstate->DecRef ();
	    if (spiralstate) spiralstate->DecRef ();
	    return NULL;
	  }
	  mesh = fact->GetMeshObjectFactory ()->NewInstance ();
          partstate = SCF_QUERY_INTERFACE (mesh, iParticleState);
          spiralstate = SCF_QUERY_INTERFACE (mesh, iSpiralState);
	}
	break;
      case CS_TOKEN_MATERIAL:
	{
          csScanStr (params, "%s", str);
          iMaterialWrapper* mat = ldr_context->FindMaterial (str);
	  if (!mat)
	  {
            // @@@ Error handling!
            mesh->DecRef ();
	    if (partstate) partstate->DecRef ();
	    if (spiralstate) spiralstate->DecRef ();
            return NULL;
	  }
	  partstate->SetMaterialWrapper (mat);
	}
	break;
      case CS_TOKEN_MIXMODE:
	uint mode;
	if (synldr->ParseMixmode (parser, params, mode))
          partstate->SetMixMode (mode);
	break;
      case CS_TOKEN_NUMBER:
        {
          int nr;
          csScanStr (params, "%d", &nr);
          spiralstate->SetParticleCount (nr);
        }
        break;
    }
  }

  if (partstate) partstate->DecRef ();
  if (spiralstate) spiralstate->DecRef ();
  return mesh;
}

iBase* csSpiralLoader::Parse (iDocumentNode* node,
	iLoaderContext* ldr_context, iBase*)
{
  csRef<iMeshObject> mesh;
  csRef<iParticleState> partstate;
  csRef<iSpiralState> spiralstate;

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
	    return NULL;
	  partstate->SetColor (color);
	}
	break;
      case XMLTOKEN_SOURCE:
	{
	  csVector3 s;
	  if (!synldr->ParseVector (child, s))
	    return NULL;
	  spiralstate->SetSource (s);
	}
	break;
      case XMLTOKEN_FACTORY:
	{
	  const char* factname = child->GetContentsValue ();
	  iMeshFactoryWrapper* fact = ldr_context->FindMeshFactory (factname);
	  if (!fact)
	  {
      	    ReportError (reporter,
		"crystalspace.ballloader.parse.unknownfactory",
		"Couldn't find factory '%s'!", factname);
	    return NULL;
	  }
	  mesh.Take (fact->GetMeshObjectFactory ()->NewInstance ());
          partstate.Take (SCF_QUERY_INTERFACE (mesh, iParticleState));
          spiralstate.Take (SCF_QUERY_INTERFACE (mesh, iSpiralState));
	}
	break;
      case XMLTOKEN_MATERIAL:
	{
	  const char* matname = child->GetContentsValue ();
          iMaterialWrapper* mat = ldr_context->FindMaterial (matname);
	  if (!mat)
	  {
      	    ReportError (reporter,
		"crystalspace.ballloader.parse.unknownmaterial",
		"Couldn't find material '%s'!", matname);
            return NULL;
	  }
	  partstate->SetMaterialWrapper (mat);
	}
	break;
      case XMLTOKEN_MIXMODE:
	{
	  uint mode;
	  if (!synldr->ParseMixmode (child, mode))
	    return NULL;
          partstate->SetMixMode (mode);
	}
	break;
      case XMLTOKEN_NUMBER:
        spiralstate->SetParticleCount (child->GetContentsValueAsInt ());
        break;
      default:
      	ReportError (reporter,
		"crystalspace.ballloader.parse.badtoken",
		"Unexpected token '%s' in spiral loader!", value);
	return NULL;
    }
  }

  // Incref to avoid smart pointer from cleaning up.
  if (mesh) mesh->IncRef ();
  return mesh;
}

//---------------------------------------------------------------------------

csSpiralSaver::csSpiralSaver (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  plugin_mgr = NULL;
}

csSpiralSaver::~csSpiralSaver ()
{
  SCF_DEC_REF (plugin_mgr);
  SCF_DEC_REF (synldr);
}

bool csSpiralSaver::Initialize (iObjectRegistry* object_reg)
{
  csSpiralSaver::object_reg = object_reg;
  plugin_mgr = CS_QUERY_REGISTRY (object_reg, iPluginManager);
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  return true;
}

void csSpiralSaver::WriteDown (iBase* obj, iFile *file)
{
  csString str;
  iFactory *fact = SCF_QUERY_INTERFACE (this, iFactory);
  iParticleState *partstate = SCF_QUERY_INTERFACE (obj, iParticleState);
  iSpiralState *state = SCF_QUERY_INTERFACE (obj, iSpiralState);
  char buf[MAXLINE];
  char name[MAXLINE];

  csFindReplace(name, fact->QueryDescription (), "Saver", "Loader", MAXLINE);
  sprintf(buf, "FACTORY ('%s')\n", name);
  str.Append(buf);

  if(partstate->GetMixMode() != CS_FX_COPY)
  {
    str.Append (synldr->MixmodeToText (partstate->GetMixMode(), 0, true));
  }

  sprintf(buf, "MATERIAL (%s)\n", partstate->GetMaterialWrapper()->
    QueryObject ()->GetName());
  str.Append(buf);
  sprintf(buf, "COLOR (%g, %g, %g)\n", partstate->GetColor().red,
    partstate->GetColor().green, partstate->GetColor().blue);
  str.Append(buf);
  printf(buf, "NUMBER (%d)\n", state->GetParticleCount());
  str.Append(buf);
  printf(buf, "SOURCE (%g, %g, %g)\n", state->GetSource().x,
    state->GetSource().y, state->GetSource().z);
  str.Append(buf);

  fact->DecRef();
  partstate->DecRef();
  state->DecRef();
  file->Write ((const char*)str, str.Length ());
}
