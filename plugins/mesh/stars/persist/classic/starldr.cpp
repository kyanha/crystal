/*
    Copyright (C) 2002 by Jorrit Tyberghein

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
#include "csutil/cscolor.h"
#include "starldr.h"
#include "imesh/object.h"
#include "imesh/stars.h"
#include "iengine/mesh.h"
#include "iengine/engine.h"
#include "iutil/plugin.h"
#include "iutil/document.h"
#include "ivideo/graph3d.h"
#include "qint.h"
#include "iutil/object.h"
#include "iengine/material.h"
#include "ivaria/reporter.h"
#include "iutil/objreg.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "imap/services.h"
#include "imap/ldrctxt.h"

CS_IMPLEMENT_PLUGIN

CS_TOKEN_DEF_START
  CS_TOKEN_DEF (BOX)
  CS_TOKEN_DEF (COLOR)
  CS_TOKEN_DEF (MAXCOLOR)
  CS_TOKEN_DEF (DENSITY)
  CS_TOKEN_DEF (MAXDISTANCE)
  CS_TOKEN_DEF (FACTORY)
CS_TOKEN_DEF_END

enum
{
  XMLTOKEN_BOX = 1,
  XMLTOKEN_COLOR,
  XMLTOKEN_MAXCOLOR,
  XMLTOKEN_DENSITY,
  XMLTOKEN_MAXDISTANCE,
  XMLTOKEN_FACTORY
};

SCF_IMPLEMENT_IBASE (csStarFactoryLoader)
  SCF_IMPLEMENTS_INTERFACE (iLoaderPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csStarFactoryLoader::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csStarFactorySaver)
  SCF_IMPLEMENTS_INTERFACE (iSaverPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csStarFactorySaver::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csStarLoader)
  SCF_IMPLEMENTS_INTERFACE (iLoaderPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csStarLoader::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csStarSaver)
  SCF_IMPLEMENTS_INTERFACE (iSaverPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csStarSaver::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csStarFactoryLoader)
SCF_IMPLEMENT_FACTORY (csStarFactorySaver)
SCF_IMPLEMENT_FACTORY (csStarLoader)
SCF_IMPLEMENT_FACTORY (csStarSaver)

SCF_EXPORT_CLASS_TABLE (starldr)
  SCF_EXPORT_CLASS (csStarFactoryLoader,
    "crystalspace.mesh.loader.factory.stars",
    "Crystal Space Star Factory Loader")
  SCF_EXPORT_CLASS (csStarFactorySaver, "crystalspace.mesh.saver.factory.stars",
    "Crystal Space Star Factory Saver")
  SCF_EXPORT_CLASS (csStarLoader, "crystalspace.mesh.loader.stars",
		    "Crystal Space Star Mesh Loader")
  SCF_EXPORT_CLASS (csStarSaver, "crystalspace.mesh.saver.stars",
    "Crystal Space Star Mesh Saver")
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

csStarFactoryLoader::csStarFactoryLoader (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csStarFactoryLoader::~csStarFactoryLoader ()
{
}

bool csStarFactoryLoader::Initialize (iObjectRegistry* object_reg)
{
  csStarFactoryLoader::object_reg = object_reg;
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);
  return true;
}

csPtr<iBase> csStarFactoryLoader::Parse (const char* /*string*/,
	iLoaderContext*, iBase* /* context */)
{
  csRef<iPluginManager> plugin_mgr (CS_QUERY_REGISTRY (object_reg,
  	iPluginManager));
  csRef<iMeshObjectType> type (CS_QUERY_PLUGIN_CLASS (plugin_mgr,
  	"crystalspace.mesh.object.stars", iMeshObjectType));
  if (!type)
  {
    type = CS_LOAD_PLUGIN (plugin_mgr, "crystalspace.mesh.object.stars",
    	iMeshObjectType);
  }
  if (!type)
  {
    ReportError (reporter,
		"crystalspace.starfactoryloader.setup.objecttype",
		"Could not load the stars mesh object plugin!");
    return NULL;
  }
  csRef<iMeshObjectFactory> fact (type->NewFactory ());
  if (fact) fact->IncRef ();	// Avoid smart pointer release.
  return csPtr<iBase> (fact);
}

csPtr<iBase> csStarFactoryLoader::Parse (iDocumentNode* /*node*/,
	iLoaderContext*, iBase* /* context */)
{
  csRef<iPluginManager> plugin_mgr (CS_QUERY_REGISTRY (object_reg,
  	iPluginManager));
  csRef<iMeshObjectType> type (CS_QUERY_PLUGIN_CLASS (plugin_mgr,
  	"crystalspace.mesh.object.stars", iMeshObjectType));
  if (!type)
  {
    type = CS_LOAD_PLUGIN (plugin_mgr, "crystalspace.mesh.object.stars",
    	iMeshObjectType);
  }
  if (!type)
  {
    ReportError (reporter,
		"crystalspace.starfactoryloader.setup.objecttype",
		"Could not load the stars mesh object plugin!");
    return NULL;
  }
  csRef<iMeshObjectFactory> fact (type->NewFactory ());
  if (fact) fact->IncRef ();	// Avoid smart pointer release.
  return csPtr<iBase> (fact);
}

//---------------------------------------------------------------------------

csStarFactorySaver::csStarFactorySaver (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csStarFactorySaver::~csStarFactorySaver ()
{
}

bool csStarFactorySaver::Initialize (iObjectRegistry* object_reg)
{
  csStarFactorySaver::object_reg = object_reg;
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);
  return true;
}

#define MAXLINE 100 /* max number of chars per line... */

void csStarFactorySaver::WriteDown (iBase* /*obj*/, iFile * /*file*/)
{
  // no params
}

//---------------------------------------------------------------------------

csStarLoader::csStarLoader (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csStarLoader::~csStarLoader ()
{
}

bool csStarLoader::Initialize (iObjectRegistry* object_reg)
{
  csStarLoader::object_reg = object_reg;
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);

  xmltokens.Register ("box", XMLTOKEN_BOX);
  xmltokens.Register ("color", XMLTOKEN_COLOR);
  xmltokens.Register ("maxcolor", XMLTOKEN_MAXCOLOR);
  xmltokens.Register ("density", XMLTOKEN_DENSITY);
  xmltokens.Register ("maxdistance", XMLTOKEN_MAXDISTANCE);
  xmltokens.Register ("factory", XMLTOKEN_FACTORY);

  return true;
}

csPtr<iBase> csStarLoader::Parse (const char* string,
	iLoaderContext* ldr_context, iBase*)
{
  CS_TOKEN_TABLE_START (commands)
    CS_TOKEN_TABLE (BOX)
    CS_TOKEN_TABLE (COLOR)
    CS_TOKEN_TABLE (MAXCOLOR)
    CS_TOKEN_TABLE (DENSITY)
    CS_TOKEN_TABLE (MAXDISTANCE)
    CS_TOKEN_TABLE (FACTORY)
  CS_TOKEN_TABLE_END

  char* name;
  long cmd;
  char* params;
  char str[255];

  csRef<iMeshObject> mesh;
  csRef<iStarsState> starstate;

  csParser* parser = ldr_context->GetParser ();
  
  char* buf = (char*)string;
  while ((cmd = parser->GetObject (&buf, commands, &name, &params)) > 0)
  {
    if (!params)
    {
      ReportError (reporter,
		"crystalspace.starloader.parse.badformat",
		"Bad format while parsing star object!");
      return NULL;
    }
    switch (cmd)
    {
      case CS_TOKEN_BOX:
	{
	  csVector3 v1, v2;
	  csScanStr (params, "%f,%f,%f,%f,%f,%f",
	    &v1.x, &v1.y, &v1.z,
	    &v2.x, &v2.y, &v2.z);
	  starstate->SetBox (csBox3 (v1, v2));
	}
	break;
      case CS_TOKEN_COLOR:
	{
	  csColor col;
	  csScanStr (params, "%f,%f,%f", &col.red, &col.green, &col.blue);
	  starstate->SetColor (col);
	}
	break;
      case CS_TOKEN_MAXCOLOR:
	{
	  csColor col;
	  csScanStr (params, "%f,%f,%f", &col.red, &col.green, &col.blue);
	  starstate->SetMaxColor (col);
	}
	break;
      case CS_TOKEN_DENSITY:
	{
	  float d;
	  csScanStr (params, "%f", &d);
	  starstate->SetDensity (d);
	}
	break;
      case CS_TOKEN_MAXDISTANCE:
	{
	  float d;
	  csScanStr (params, "%f", &d);
	  starstate->SetMaxDistance (d);
	}
	break;
      case CS_TOKEN_FACTORY:
	{
          csScanStr (params, "%s", str);
	  iMeshFactoryWrapper* fact = ldr_context->FindMeshFactory (str);
	  if (!fact)
	  {
      	    ReportError (reporter,
		"crystalspace.starloader.parse.unknownfactory",
		"Couldn't find factory '%s'!", str);
	    return NULL;
	  }
	  mesh = fact->GetMeshObjectFactory ()->NewInstance ();
          starstate = SCF_QUERY_INTERFACE (mesh, iStarsState);
	}
	break;
    }
  }

  if (mesh) mesh->IncRef ();	// Avoid smart pointer release.
  return csPtr<iBase> (mesh);
}

csPtr<iBase> csStarLoader::Parse (iDocumentNode* node,
	iLoaderContext* ldr_context, iBase*)
{
  csRef<iMeshObject> mesh;
  csRef<iStarsState> starstate;

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_BOX:
	{
	  csBox3 box;
	  if (!synldr->ParseBox (child, box))
	    return NULL;
	  starstate->SetBox (box);
	}
	break;
      case XMLTOKEN_COLOR:
	{
	  csColor col;
	  if (!synldr->ParseColor (child, col))
	    return NULL;
	  starstate->SetColor (col);
	}
	break;
      case XMLTOKEN_MAXCOLOR:
	{
	  csColor col;
	  if (!synldr->ParseColor (child, col))
	    return NULL;
	  starstate->SetMaxColor (col);
	}
	break;
      case XMLTOKEN_DENSITY:
	starstate->SetDensity (child->GetContentsValueAsFloat ());
	break;
      case XMLTOKEN_MAXDISTANCE:
	starstate->SetMaxDistance (child->GetContentsValueAsFloat ());
	break;
      case XMLTOKEN_FACTORY:
	{
	  const char* factname = child->GetContentsValue ();
	  iMeshFactoryWrapper* fact = ldr_context->FindMeshFactory (factname);
	  if (!fact)
	  {
      	    synldr->ReportError (
		"crystalspace.starloader.parse.unknownfactory",
		child, "Couldn't find factory '%s'!", factname);
	    return NULL;
	  }
	  mesh = fact->GetMeshObjectFactory ()->NewInstance ();
          starstate = SCF_QUERY_INTERFACE (mesh, iStarsState);
	}
	break;
      default:
        synldr->ReportBadToken (child);
	return NULL;
    }
  }

  // Incref to prevent smart pointer from deleting it.
  if (mesh) mesh->IncRef ();
  return csPtr<iBase> (mesh);
}

//---------------------------------------------------------------------------

csStarSaver::csStarSaver (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csStarSaver::~csStarSaver ()
{
}

bool csStarSaver::Initialize (iObjectRegistry* object_reg)
{
  csStarSaver::object_reg = object_reg;
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  return true;
}

void csStarSaver::WriteDown (iBase* /*obj*/, iFile* /*file*/)
{
  // @@@ Not implemented yet.
}
