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
#include "emitldr.h"
#include "imesh/object.h"
#include "iengine/mesh.h"
#include "iengine/engine.h"
#include "iutil/plugin.h"
#include "imesh/partsys.h"
#include "imesh/emit.h"
#include "ivideo/graph3d.h"
#include "qint.h"
#include "iengine/material.h"
#include "iutil/vfs.h"
#include "csutil/csstring.h"
#include "iutil/object.h"
#include "iutil/document.h"
#include "iutil/objreg.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "imap/ldrctxt.h"
#include "ivaria/reporter.h"

CS_IMPLEMENT_PLUGIN

CS_TOKEN_DEF_START
  CS_TOKEN_DEF (AGING)
  CS_TOKEN_DEF (ATTRACTORFORCE)
  CS_TOKEN_DEF (ATTRACTOR)
  CS_TOKEN_DEF (EMITBOX)
  CS_TOKEN_DEF (EMITCONE)
  CS_TOKEN_DEF (EMITCYLINDERTANGENT)
  CS_TOKEN_DEF (EMITCYLINDER)
  CS_TOKEN_DEF (EMITFIXED)
  CS_TOKEN_DEF (EMITLINE)
  CS_TOKEN_DEF (EMITMIX)
  CS_TOKEN_DEF (EMITSPHERETANGENT)
  CS_TOKEN_DEF (EMITSPHERE)
  CS_TOKEN_DEF (FACTORY)
  CS_TOKEN_DEF (LIGHTING)
  CS_TOKEN_DEF (MATERIAL)
  CS_TOKEN_DEF (MIXMODE)
  CS_TOKEN_DEF (NUMBER)
  CS_TOKEN_DEF (RECTPARTICLES)
  CS_TOKEN_DEF (REGULARPARTICLES)
  CS_TOKEN_DEF (STARTACCEL)
  CS_TOKEN_DEF (STARTPOS)
  CS_TOKEN_DEF (STARTSPEED)
  CS_TOKEN_DEF (TOTALTIME)
  CS_TOKEN_DEF (WEIGHT)
CS_TOKEN_DEF_END

enum
{
  XMLTOKEN_AGING = 1,
  XMLTOKEN_ATTRACTORFORCE,
  XMLTOKEN_ATTRACTOR,
  XMLTOKEN_EMITBOX,
  XMLTOKEN_EMITCONE,
  XMLTOKEN_EMITCYLINDERTANGENT,
  XMLTOKEN_EMITCYLINDER,
  XMLTOKEN_EMITFIXED,
  XMLTOKEN_EMITLINE,
  XMLTOKEN_EMITMIX,
  XMLTOKEN_EMITSPHERETANGENT,
  XMLTOKEN_EMITSPHERE,
  XMLTOKEN_FACTORY,
  XMLTOKEN_LIGHTING,
  XMLTOKEN_MATERIAL,
  XMLTOKEN_MIXMODE,
  XMLTOKEN_NUMBER,
  XMLTOKEN_RECTPARTICLES,
  XMLTOKEN_REGULARPARTICLES,
  XMLTOKEN_STARTACCEL,
  XMLTOKEN_STARTPOS,
  XMLTOKEN_STARTSPEED,
  XMLTOKEN_TOTALTIME,
  XMLTOKEN_WEIGHT
};

SCF_IMPLEMENT_IBASE (csEmitFactoryLoader)
  SCF_IMPLEMENTS_INTERFACE (iLoaderPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csEmitFactoryLoader::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csEmitFactorySaver)
  SCF_IMPLEMENTS_INTERFACE (iSaverPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csEmitFactorySaver::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csEmitLoader)
  SCF_IMPLEMENTS_INTERFACE (iLoaderPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csEmitLoader::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csEmitSaver)
  SCF_IMPLEMENTS_INTERFACE (iSaverPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csEmitSaver::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csEmitFactoryLoader)
SCF_IMPLEMENT_FACTORY (csEmitFactorySaver)
SCF_IMPLEMENT_FACTORY (csEmitLoader)
SCF_IMPLEMENT_FACTORY (csEmitSaver)

SCF_EXPORT_CLASS_TABLE (emitldr)
  SCF_EXPORT_CLASS (csEmitFactoryLoader,
    "crystalspace.mesh.loader.factory.emit",
    "Crystal Space Emit Factory Loader")
  SCF_EXPORT_CLASS (csEmitFactorySaver, "crystalspace.mesh.saver.factory.emit",
    "Crystal Space Emit Factory Saver")
  SCF_EXPORT_CLASS (csEmitLoader, "crystalspace.mesh.loader.emit",
    "Crystal Space Emit Mesh Loader")
  SCF_EXPORT_CLASS (csEmitSaver, "crystalspace.mesh.saver.emit",
    "Crystal Space Emit Mesh Saver")
SCF_EXPORT_CLASS_TABLE_END

csEmitFactoryLoader::csEmitFactoryLoader (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  plugin_mgr = NULL;
}

csEmitFactoryLoader::~csEmitFactoryLoader ()
{
  SCF_DEC_REF (plugin_mgr);
}

bool csEmitFactoryLoader::Initialize (iObjectRegistry* object_reg)
{
  csEmitFactoryLoader::object_reg = object_reg;
  plugin_mgr = CS_QUERY_REGISTRY (object_reg, iPluginManager);
  return true;
}

iBase* csEmitFactoryLoader::Parse (const char* /*string*/,
	iLoaderContext*, iBase* /* context */)
{
  iMeshObjectType* type = CS_QUERY_PLUGIN_CLASS (plugin_mgr,
  	"crystalspace.mesh.object.emit", iMeshObjectType);
  if (!type)
  {
    type = CS_LOAD_PLUGIN (plugin_mgr, "crystalspace.mesh.object.emit",
    	iMeshObjectType);
    printf ("Load TYPE plugin crystalspace.mesh.object.emit\n");
  }
  iMeshObjectFactory* fact = type->NewFactory ();
  type->DecRef ();
  return fact;
}

iBase* csEmitFactoryLoader::Parse (iDocumentNode* /*node*/,
	iLoaderContext*, iBase* /* context */)
{
  iMeshObjectType* type = CS_QUERY_PLUGIN_CLASS (plugin_mgr,
  	"crystalspace.mesh.object.emit", iMeshObjectType);
  if (!type)
  {
    type = CS_LOAD_PLUGIN (plugin_mgr, "crystalspace.mesh.object.emit",
    	iMeshObjectType);
    printf ("Load TYPE plugin crystalspace.mesh.object.emit\n");
  }
  iMeshObjectFactory* fact = type->NewFactory ();
  type->DecRef ();
  return fact;
}

//---------------------------------------------------------------------------

csEmitFactorySaver::csEmitFactorySaver (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  plugin_mgr = NULL;
}

csEmitFactorySaver::~csEmitFactorySaver ()
{
  SCF_DEC_REF (plugin_mgr);
}

bool csEmitFactorySaver::Initialize (iObjectRegistry* object_reg)
{
  csEmitFactorySaver::object_reg = object_reg;
  plugin_mgr = CS_QUERY_REGISTRY (object_reg, iPluginManager);
  return true;
}

#define MAXLINE 100 /* max number of chars per line... */

void csEmitFactorySaver::WriteDown (iBase* /*obj*/, iFile * /*file*/)
{
  // nothing to do
}

//---------------------------------------------------------------------------

csEmitLoader::csEmitLoader (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  plugin_mgr = NULL;
  reporter = NULL;
  synldr = NULL;
}

csEmitLoader::~csEmitLoader ()
{
  SCF_DEC_REF (plugin_mgr);
  SCF_DEC_REF (synldr);
  SCF_DEC_REF (reporter);
}

bool csEmitLoader::Initialize (iObjectRegistry* object_reg)
{
  csEmitLoader::object_reg = object_reg;
  plugin_mgr = CS_QUERY_REGISTRY (object_reg, iPluginManager);
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);

  xmltokens.Register ("aging", XMLTOKEN_AGING);
  xmltokens.Register ("attractorforce", XMLTOKEN_ATTRACTORFORCE);
  xmltokens.Register ("attractor", XMLTOKEN_ATTRACTOR);
  xmltokens.Register ("emitbox", XMLTOKEN_EMITBOX);
  xmltokens.Register ("emitcone", XMLTOKEN_EMITCONE);
  xmltokens.Register ("emitcylindertangent", XMLTOKEN_EMITCYLINDERTANGENT);
  xmltokens.Register ("emitcylinder", XMLTOKEN_EMITCYLINDER);
  xmltokens.Register ("emitfixed", XMLTOKEN_EMITFIXED);
  xmltokens.Register ("emitline", XMLTOKEN_EMITLINE);
  xmltokens.Register ("emitmix", XMLTOKEN_EMITMIX);
  xmltokens.Register ("emitspheretangent", XMLTOKEN_EMITSPHERETANGENT);
  xmltokens.Register ("emitsphere", XMLTOKEN_EMITSPHERE);
  xmltokens.Register ("factory", XMLTOKEN_FACTORY);
  xmltokens.Register ("lighting", XMLTOKEN_LIGHTING);
  xmltokens.Register ("material", XMLTOKEN_MATERIAL);
  xmltokens.Register ("mixmode", XMLTOKEN_MIXMODE);
  xmltokens.Register ("number", XMLTOKEN_NUMBER);
  xmltokens.Register ("rectparticles", XMLTOKEN_RECTPARTICLES);
  xmltokens.Register ("regularparticles", XMLTOKEN_REGULARPARTICLES);
  xmltokens.Register ("startaccel", XMLTOKEN_STARTACCEL);
  xmltokens.Register ("startpos", XMLTOKEN_STARTPOS);
  xmltokens.Register ("startspeed", XMLTOKEN_STARTSPEED);
  xmltokens.Register ("totaltime", XMLTOKEN_TOTALTIME);
  xmltokens.Register ("weight", XMLTOKEN_WEIGHT);
  return true;
}

static iEmitGen3D* ParseEmit (csParser* parser, char* buf, 
			      iEmitFactoryState *fstate, float* weight)
{
  CS_TOKEN_TABLE_START (emits)
    CS_TOKEN_TABLE (WEIGHT)
    CS_TOKEN_TABLE (EMITBOX)
    CS_TOKEN_TABLE (EMITCONE)
    CS_TOKEN_TABLE (EMITCYLINDERTANGENT)
    CS_TOKEN_TABLE (EMITCYLINDER)
    CS_TOKEN_TABLE (EMITFIXED)
    CS_TOKEN_TABLE (EMITLINE)
    CS_TOKEN_TABLE (EMITMIX)
    CS_TOKEN_TABLE (EMITSPHERETANGENT)
    CS_TOKEN_TABLE (EMITSPHERE)
  CS_TOKEN_TABLE_END

  char* name;
  long cmd;
  char* params;

  iEmitGen3D* result = 0;
  iEmitMix *emix = 0;
  csVector3 a,b;
  float p,q,r,s,t;
  if (weight) *weight = 1.;

  while ((cmd = parser->GetObject (&buf, emits, &name, &params)) > 0)
  {
    if (!params)
    {
      printf ("Expected parameters instead of '%s'!\n", buf);
      return 0;
    }
    switch (cmd)
    {
      case CS_TOKEN_WEIGHT:
	if (weight == NULL)
	{
	  printf ("WEIGHT cannot be given in this context!\n");
	  return 0;
	}
        csScanStr (params, "%f", weight);
	break;
      case CS_TOKEN_EMITFIXED:
        {
          csScanStr (params, "%f,%f,%f", &a.x, &a.y, &a.z);
	  iEmitFixed *efixed = fstate->CreateFixed();
	  efixed->SetValue(a);
	  result = efixed;
	}
	break;
      case CS_TOKEN_EMITBOX:
        {
          csScanStr (params, "%f,%f,%f,%f,%f,%f", &a.x, &a.y, &a.z,
	    &b.x, &b.y, &b.z);
	  iEmitBox *ebox = fstate->CreateBox();
	  ebox->SetContent(a, b);
	  result = ebox;
	}
	break;
      case CS_TOKEN_EMITSPHERE:
        {
          csScanStr (params, "%f,%f,%f,%f,%f", &a.x, &a.y, &a.z, &p, &q);
	  iEmitSphere *esphere = fstate->CreateSphere();
	  esphere->SetContent(a, p, q);
	  result = esphere;
	}
	break;
      case CS_TOKEN_EMITCONE:
        {
          csScanStr (params, "%f,%f,%f,%f,%f,%f,%f,%f", &a.x, &a.y, &a.z,
	    &p, &q, &r, &s, &t);
	  iEmitCone *econe = fstate->CreateCone();
	  econe->SetContent(a, p, q, r, s, t);
	  result = econe;
	}
	break;
      case CS_TOKEN_EMITMIX:
        {
	  if(!emix) emix = fstate->CreateMix();
	  float amt;
	  iEmitGen3D *gen;
	  gen = ParseEmit(parser, params, fstate, &amt);
	  emix->AddEmitter(amt, gen);
	  SCF_DEC_REF (gen);
	  result = emix;
	}
	break;
      case CS_TOKEN_EMITLINE:
        {
          csScanStr (params, "%f,%f,%f,%f,%f,%f", &a.x, &a.y, &a.z,
	    &b.x, &b.y, &b.z);
	  iEmitLine *eline = fstate->CreateLine();
	  eline->SetContent(a, b);
	  result = eline;
	}
	break;
      case CS_TOKEN_EMITCYLINDER:
        {
          csScanStr (params, "%f,%f,%f,%f,%f,%f,%f,%f", &a.x, &a.y, &a.z,
	    &b.x, &b.y, &b.z, &p, &q);
	  iEmitCylinder *ecyl = fstate->CreateCylinder();
	  ecyl->SetContent(a, b, p, q);
	  result = ecyl;
	}
	break;
      case CS_TOKEN_EMITCYLINDERTANGENT:
        {
          csScanStr (params, "%f,%f,%f,%f,%f,%f,%f,%f", &a.x, &a.y, &a.z,
	    &b.x, &b.y, &b.z, &p, &q);
	  iEmitCylinderTangent *ecyltan = fstate->CreateCylinderTangent();
	  ecyltan->SetContent(a, b, p, q);
	  result = ecyltan;
	}
	break;
      case CS_TOKEN_EMITSPHERETANGENT:
        {
          csScanStr (params, "%f,%f,%f,%f,%f", &a.x, &a.y, &a.z, &p, &q);
	  iEmitSphereTangent *espheretan = fstate->CreateSphereTangent();
	  espheretan->SetContent(a, p, q);
	  result = espheretan;
	}
	break;
    }
  }
  return result;
}

iEmitGen3D* csEmitLoader::ParseEmit (iDocumentNode* node,
			      iEmitFactoryState *fstate, float* weight)
{
  iEmitGen3D* result = NULL;
  iEmitMix *emix = NULL;
  csVector3 a,b;
  float p,q,r,s,t;
  if (weight) *weight = 1.;

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_WEIGHT:
	if (weight == NULL)
	{
	  synldr->ReportError ("crystalspace.emitloader.parse",
		child, "'weight' cannot be given in this context!");
	  return NULL;
	}
	*weight = child->GetContentsValueAsFloat ();
	break;
      case XMLTOKEN_EMITFIXED:
        {
	  a.x = child->GetAttributeValueAsFloat ("x");
	  a.y = child->GetAttributeValueAsFloat ("y");
	  a.z = child->GetAttributeValueAsFloat ("z");
	  iEmitFixed *efixed = fstate->CreateFixed ();
	  efixed->SetValue (a);
	  result = efixed;
	}
	break;
      case XMLTOKEN_EMITBOX:
        {
	  csBox3 box;
	  if (!synldr->ParseBox (child, box))
	  {
	    synldr->ReportError ("crystalspace.emitloader.parse",
		  child, "Error parsing box for 'emitbox'!");
	    return NULL;
	  }
	  iEmitBox *ebox = fstate->CreateBox ();
	  ebox->SetContent (box.Min (), box.Max ());
	  result = ebox;
	}
	break;
      case XMLTOKEN_EMITSPHERE:
        {
	  a.x = child->GetAttributeValueAsFloat ("x");
	  a.y = child->GetAttributeValueAsFloat ("y");
	  a.z = child->GetAttributeValueAsFloat ("z");
	  p = child->GetAttributeValueAsFloat ("p");
	  q = child->GetAttributeValueAsFloat ("q");
	  iEmitSphere *esphere = fstate->CreateSphere ();
	  esphere->SetContent (a, p, q);
	  result = esphere;
	}
	break;
      case XMLTOKEN_EMITCONE:
        {
	  a.x = child->GetAttributeValueAsFloat ("x");
	  a.y = child->GetAttributeValueAsFloat ("y");
	  a.z = child->GetAttributeValueAsFloat ("z");
	  p = child->GetAttributeValueAsFloat ("p");
	  q = child->GetAttributeValueAsFloat ("q");
	  r = child->GetAttributeValueAsFloat ("r");
	  s = child->GetAttributeValueAsFloat ("s");
	  t = child->GetAttributeValueAsFloat ("t");
	  iEmitCone *econe = fstate->CreateCone ();
	  econe->SetContent (a, p, q, r, s, t);
	  result = econe;
	}
	break;
      case XMLTOKEN_EMITMIX:
        {
	  if (!emix) emix = fstate->CreateMix ();
	  float amt;
	  iEmitGen3D *gen;
	  gen = ParseEmit (child, fstate, &amt);
	  emix->AddEmitter (amt, gen);
	  SCF_DEC_REF (gen);
	  result = emix;
	}
	break;
      case XMLTOKEN_EMITLINE:
        {
	  csBox3 box;
	  if (!synldr->ParseBox (child, box))
	  {
	    synldr->ReportError ("crystalspace.emitloader.parse",
		  child, "Error parsing box for 'emitline'!");
	    return NULL;
	  }
	  iEmitLine *eline = fstate->CreateLine ();
	  eline->SetContent (box.Min (), box.Max ());
	  result = eline;
	}
	break;
      case XMLTOKEN_EMITCYLINDER:
        {
	  csBox3 box;
	  if (!synldr->ParseBox (child, box))
	  {
	    synldr->ReportError ("crystalspace.emitloader.parse",
		  child, "Error parsing box for 'emitcylinder'!");
	    return NULL;
	  }
	  p = child->GetAttributeValueAsFloat ("p");
	  q = child->GetAttributeValueAsFloat ("q");
	  iEmitCylinder *ecyl = fstate->CreateCylinder ();
	  ecyl->SetContent (box.Min (), box.Max (), p, q);
	  result = ecyl;
	}
	break;
      case XMLTOKEN_EMITCYLINDERTANGENT:
        {
	  csBox3 box;
	  if (!synldr->ParseBox (child, box))
	  {
	    synldr->ReportError ("crystalspace.emitloader.parse",
		  child, "Error parsing box for 'emitcylindertangent'!");
	    return NULL;
	  }
	  p = child->GetAttributeValueAsFloat ("p");
	  q = child->GetAttributeValueAsFloat ("q");
	  iEmitCylinderTangent *ecyltan = fstate->CreateCylinderTangent ();
	  ecyltan->SetContent (box.Min (), box.Max (), p, q);
	  result = ecyltan;
	}
	break;
      case XMLTOKEN_EMITSPHERETANGENT:
        {
	  csRef<iDocumentNode> minnode = child->GetNode ("min");
	  if (!minnode)
	  {
	    synldr->ReportError ("crystalspace.emitloader.parse",
		child, "'min' is missing in 'emitspheretangent'!");
	    return NULL;
	  }
	  a.x = minnode->GetAttributeValueAsFloat ("x");
	  a.y = minnode->GetAttributeValueAsFloat ("y");
	  a.z = minnode->GetAttributeValueAsFloat ("z");
	  p = child->GetAttributeValueAsFloat ("p");
	  q = child->GetAttributeValueAsFloat ("q");
	  iEmitSphereTangent *espheretan = fstate->CreateSphereTangent ();
	  espheretan->SetContent (a, p, q);
	  result = espheretan;
	}
	break;
      default:
	synldr->ReportBadToken (child);
	return NULL;
    }
  }
  return result;
}

iBase* csEmitLoader::Parse (const char* string, 
			    iLoaderContext* ldr_context, iBase*)
{
  CS_TOKEN_TABLE_START (commands)
    CS_TOKEN_TABLE (AGING)
    CS_TOKEN_TABLE (ATTRACTORFORCE)
    CS_TOKEN_TABLE (ATTRACTOR)
    CS_TOKEN_TABLE (FACTORY)
    CS_TOKEN_TABLE (LIGHTING)
    CS_TOKEN_TABLE (MATERIAL)
    CS_TOKEN_TABLE (MIXMODE)
    CS_TOKEN_TABLE (NUMBER)
    CS_TOKEN_TABLE (RECTPARTICLES)
    CS_TOKEN_TABLE (REGULARPARTICLES)
    CS_TOKEN_TABLE (STARTACCEL)
    CS_TOKEN_TABLE (STARTPOS)
    CS_TOKEN_TABLE (STARTSPEED)
    CS_TOKEN_TABLE (TOTALTIME)
  CS_TOKEN_TABLE_END

  char* name;
  long cmd;
  char* params;
  char str[255];

  iEmitGen3D *emit;
  iMeshObject* mesh = NULL;
  iParticleState* partstate = NULL;
  iEmitFactoryState* emitfactorystate = NULL;
  iEmitState* emitstate = NULL;

  csParser* parser = ldr_context->GetParser ();

  char* buf = (char*)string;
  while ((cmd = parser->GetObject (&buf, commands, &name, &params)) > 0)
  {
    if (!params)
    {
      // @@@ Error handling!
      SCF_DEC_REF (mesh);
      SCF_DEC_REF (partstate);
      SCF_DEC_REF (emitstate);
      SCF_DEC_REF (emitfactorystate);
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
	    // @@@ Error handling!
            SCF_DEC_REF (mesh);
            SCF_DEC_REF (partstate);
            SCF_DEC_REF (emitstate);
            SCF_DEC_REF (emitfactorystate);
	    return NULL;
	  }
	  mesh = fact->GetMeshObjectFactory ()->NewInstance ();
          partstate = SCF_QUERY_INTERFACE (mesh, iParticleState);
          emitstate = SCF_QUERY_INTERFACE (mesh, iEmitState);
	  emitfactorystate = SCF_QUERY_INTERFACE (fact->GetMeshObjectFactory(),
	    iEmitFactoryState);
	}
	break;
      case CS_TOKEN_MATERIAL:
	{
          csScanStr (params, "%s", str);
          iMaterialWrapper* mat = ldr_context->FindMaterial (str);
	  if (!mat)
	  {
	    printf ("Could not find material '%s'!\n", str);
            // @@@ Error handling!
            SCF_DEC_REF (mesh);
            SCF_DEC_REF (partstate);
            SCF_DEC_REF (emitstate);
            SCF_DEC_REF (emitfactorystate);
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
	  int num = 10;
          csScanStr (params, "%d", &num);
	  emitstate->SetParticleCount (num);
	}
	break;
      case CS_TOKEN_LIGHTING:
	{
	  bool light = false;
          csScanStr (params, "%b", &light);
	  emitstate->SetLighting (light);
	}
	break;
      case CS_TOKEN_TOTALTIME:
	{
	  int totaltime = 100;
          csScanStr (params, "%d", &totaltime);
	  emitstate->SetParticleTime (totaltime);
	}
	break;
      case CS_TOKEN_RECTPARTICLES:
	{
	  float w,h;
          csScanStr (params, "%f,%f", &w, &h);
	  emitstate->SetRectParticles (w, h);
	}
	break;
      case CS_TOKEN_REGULARPARTICLES:
	{
	  int sides;
	  float radius;
          csScanStr (params, "%d,%f", &sides, &radius);
	  emitstate->SetRegularParticles (sides, radius);
	}
	break;
      case CS_TOKEN_AGING:
        {
	  int time;
	  csColor col;
	  float alpha, swirl, rotspeed, scale;
          csScanStr (params, "%d,%f,%f,%f,%f,%f,%f,%f", &time, &col.red,
	    &col.green, &col.blue, &alpha, &swirl, &rotspeed, &scale);
	  emitstate->AddAge (time, col, alpha, swirl, rotspeed, scale);
	}
	break;
      case CS_TOKEN_STARTPOS:
	{
	  emit = ::ParseEmit(parser, params, emitfactorystate, NULL);
	  emitstate->SetStartPosEmit (emit);
	  SCF_DEC_REF (emit);
	}
	break;
      case CS_TOKEN_STARTSPEED:
	{
	  emit = ::ParseEmit(parser, params, emitfactorystate, NULL);
	  emitstate->SetStartSpeedEmit (emit);
	  SCF_DEC_REF (emit);
	}
	break;
      case CS_TOKEN_STARTACCEL:
	{
	  emit = ::ParseEmit(parser, params, emitfactorystate, NULL);
	  emitstate->SetStartAccelEmit (emit);
	  SCF_DEC_REF (emit);
	}
	break;
      case CS_TOKEN_ATTRACTORFORCE:
	{
	  float force;
          csScanStr (params, "%f", &force);
	  emitstate->SetAttractorForce (force);
	}
	break;
      case CS_TOKEN_ATTRACTOR:
	{
	  emit = ::ParseEmit(parser, params, emitfactorystate, NULL);
	  emitstate->SetAttractorEmit (emit);
	  SCF_DEC_REF (emit);
	}
	break;
    }
  }

  SCF_DEC_REF (partstate);
  SCF_DEC_REF (emitstate);
  SCF_DEC_REF (emitfactorystate);
  return mesh;
}

iBase* csEmitLoader::Parse (iDocumentNode* node,
			    iLoaderContext* ldr_context, iBase*)
{
  iEmitGen3D *emit;
  csRef<iMeshObject> mesh;
  csRef<iParticleState> partstate;
  csRef<iEmitFactoryState> emitfactorystate;
  csRef<iEmitState> emitstate;

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
	    synldr->ReportError ("crystalspace.emitloader.parse",
		child, "Cannot find factory '%s' for emit!", factname);
	    return NULL;
	  }
	  mesh = fact->GetMeshObjectFactory ()->NewInstance ();
          partstate = SCF_QUERY_INTERFACE (mesh, iParticleState);
          emitstate = SCF_QUERY_INTERFACE (mesh, iEmitState);
	  emitfactorystate = SCF_QUERY_INTERFACE (
	  	fact->GetMeshObjectFactory(), iEmitFactoryState);
	}
	break;
      case XMLTOKEN_MATERIAL:
	{
	  const char* matname = child->GetContentsValue ();
          iMaterialWrapper* mat = ldr_context->FindMaterial (matname);
	  if (!mat)
	  {
	    synldr->ReportError ("crystalspace.emitloader.parse",
		child, "Cannot find material '%s' for emit!", matname);
            return NULL;
	  }
	  partstate->SetMaterialWrapper (mat);
	}
	break;
      case XMLTOKEN_MIXMODE:
        {
	  uint mode;
	  if (synldr->ParseMixmode (child, mode))
            partstate->SetMixMode (mode);
	}
	break;
      case XMLTOKEN_NUMBER:
	emitstate->SetParticleCount (child->GetContentsValueAsInt ());
	break;
      case XMLTOKEN_LIGHTING:
	{
	  bool light = false;
	  if (!synldr->ParseBool (child, light, true))
	    return NULL;
	  emitstate->SetLighting (light);
	}
	break;
      case XMLTOKEN_TOTALTIME:
	emitstate->SetParticleTime (child->GetContentsValueAsInt ());
	break;
      case XMLTOKEN_RECTPARTICLES:
	{
	  float w, h;
	  w = child->GetAttributeValueAsFloat ("w");
	  h = child->GetAttributeValueAsFloat ("h");
	  emitstate->SetRectParticles (w, h);
	}
	break;
      case XMLTOKEN_REGULARPARTICLES:
	{
	  int sides;
	  float radius;
	  sides = child->GetAttributeValueAsInt ("sides");
	  radius = child->GetAttributeValueAsFloat ("radius");
	  emitstate->SetRegularParticles (sides, radius);
	}
	break;
      case XMLTOKEN_AGING:
        {
	  int time;
	  csColor col (1, 1, 1);
	  float alpha, swirl, rotspeed, scale;
	  csRef<iDocumentNode> alphanode = child->GetNode ("alpha");
	  if (!alphanode)
	  {
	    synldr->ReportError ("crystalspace.emitloader.parse",
		child, "Missing 'alpha' in 'aging'!");
            return NULL;
	  }
	  alpha = alphanode->GetContentsValueAsFloat ();
	  csRef<iDocumentNode> swirlnode = child->GetNode ("swirl");
	  if (!swirlnode)
	  {
	    synldr->ReportError ("crystalspace.emitloader.parse",
		child, "Missing 'swirl' in 'aging'!");
            return NULL;
	  }
	  swirl = swirlnode->GetContentsValueAsFloat ();
	  csRef<iDocumentNode> scalenode = child->GetNode ("scale");
	  if (!scalenode)
	  {
	    synldr->ReportError ("crystalspace.emitloader.parse",
		child, "Missing 'scale' in 'aging'!");
            return NULL;
	  }
	  scale = scalenode->GetContentsValueAsFloat ();
	  csRef<iDocumentNode> rotspeednode = child->GetNode ("rotspeed");
	  if (!rotspeednode)
	  {
	    synldr->ReportError ("crystalspace.emitloader.parse",
		child, "Missing 'rotspeed' in 'aging'!");
            return NULL;
	  }
	  rotspeed = rotspeednode->GetContentsValueAsFloat ();
	  csRef<iDocumentNode> timenode = child->GetNode ("time");
	  if (!timenode)
	  {
	    synldr->ReportError ("crystalspace.emitloader.parse",
		child, "Missing 'time' in 'aging'!");
            return NULL;
	  }
	  time = timenode->GetContentsValueAsInt ();
	  csRef<iDocumentNode> colornode = child->GetNode ("color");
	  if (colornode)
	    if (!synldr->ParseColor (colornode, col))
	      return NULL;
	  emitstate->AddAge (time, col, alpha, swirl, rotspeed, scale);
	}
	break;
      case XMLTOKEN_STARTPOS:
	{
	  emit = ParseEmit (child, emitfactorystate, NULL);
	  emitstate->SetStartPosEmit (emit);
	  SCF_DEC_REF (emit);
	}
	break;
      case XMLTOKEN_STARTSPEED:
	{
	  emit = ParseEmit (child, emitfactorystate, NULL);
	  emitstate->SetStartSpeedEmit (emit);
	  SCF_DEC_REF (emit);
	}
	break;
      case XMLTOKEN_STARTACCEL:
	{
	  emit = ParseEmit (child, emitfactorystate, NULL);
	  emitstate->SetStartAccelEmit (emit);
	  SCF_DEC_REF (emit);
	}
	break;
      case XMLTOKEN_ATTRACTORFORCE:
	emitstate->SetAttractorForce (child->GetContentsValueAsFloat ());
	break;
      case XMLTOKEN_ATTRACTOR:
	{
	  emit = ParseEmit (child, emitfactorystate, NULL);
	  emitstate->SetAttractorEmit (emit);
	  SCF_DEC_REF (emit);
	}
	break;
      default:
	synldr->ReportBadToken (child);
        return NULL;
    }
  }

  // Incref so smart pointer doesn't release.
  if (mesh) mesh->IncRef ();
  return mesh;
}

//---------------------------------------------------------------------------


csEmitSaver::csEmitSaver (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  plugin_mgr = NULL;
}

csEmitSaver::~csEmitSaver ()
{
  SCF_DEC_REF (plugin_mgr);
  SCF_DEC_REF (synldr);
}

bool csEmitSaver::Initialize (iObjectRegistry* object_reg)
{
  csEmitSaver::object_reg = object_reg;
  plugin_mgr = CS_QUERY_REGISTRY (object_reg, iPluginManager);
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  return true;
}

/// write emitter to string
static void WriteEmit(csString& str, iEmitGen3D *emit)
{
  char buf[MAXLINE];
  csVector3 a,b;
  float p, q, r, s, t;
  iEmitFixed *efixed = SCF_QUERY_INTERFACE(emit, iEmitFixed);
  if(efixed)
  {
    /// b is ignored
    efixed->GetValue(a, b);
    sprintf(buf, "  EMITFIXED(%g, %g, %g)\n", a.x, a.y, a.z);
    str.Append(buf);
    efixed->DecRef();
    return;
  }
  iEmitSphere *esphere = SCF_QUERY_INTERFACE(emit, iEmitSphere);
  if(esphere)
  {
    esphere->GetContent(a, p, q);
    sprintf(buf, "  EMITSPHERE(%g,%g,%g, %g, %g)\n", a.x, a.y, a.z, p, q);
    str.Append(buf);
    esphere->DecRef();
    return;
  }
  iEmitBox *ebox = SCF_QUERY_INTERFACE(emit, iEmitBox);
  if(ebox)
  {
    ebox->GetContent(a, b);
    sprintf(buf, "  EMITBOX(%g,%g,%g, %g,%g,%g)\n", a.x,a.y,a.z, b.x,b.y,b.z);
    str.Append(buf);
    ebox->DecRef();
    return;
  }
  iEmitCone *econe = SCF_QUERY_INTERFACE(emit, iEmitCone);
  if(econe)
  {
    econe->GetContent(a, p, q, r, s, t);
    sprintf(buf, "  EMITCONE(%g,%g,%g, %g, %g, %g, %g, %g)\n", a.x,a.y,a.z,
      p, q, r, s, t);
    str.Append(buf);
    econe->DecRef();
    return;
  }
  iEmitMix *emix = SCF_QUERY_INTERFACE(emit, iEmitMix);
  if(emix)
  {
	int i;
	float w;
    for(i=0; i<emix->GetEmitterCount(); i++)
    {
      iEmitGen3D *gen;
      emix->GetContent(i, w, gen);
      sprintf(buf, "  EMITMIX( WEIGHT(%g)\n", w);
      str.Append(buf);
      WriteEmit(str, gen);
      str.Append("  )\n");
    }
    emix->DecRef();
    return;
  }
  iEmitLine *eline = SCF_QUERY_INTERFACE(emit, iEmitLine);
  if(eline)
  {
    eline->GetContent(a, b);
    sprintf(buf, "  EMITLINE(%g,%g,%g, %g,%g,%g)\n", a.x,a.y,a.z, b.x,b.y,b.z);
    str.Append(buf);
    eline->DecRef();
    return;
  }
  iEmitCylinder *ecyl = SCF_QUERY_INTERFACE(emit, iEmitCylinder);
  if(ecyl)
  {
    ecyl->GetContent(a, b, p, q);
    sprintf(buf, "  EMITCYLINDER(%g,%g,%g, %g,%g,%g, %g, %g)\n", a.x,a.y,a.z,
      b.x,b.y,b.z, p, q);
    str.Append(buf);
    ecyl->DecRef();
    return;
  }
  iEmitCylinderTangent *ecyltan =
    SCF_QUERY_INTERFACE(emit, iEmitCylinderTangent);
  if(ecyltan)
  {
    ecyltan->GetContent(a, b, p, q);
    sprintf(buf, "  EMITCYLINDERTANGENT(%g,%g,%g, %g,%g,%g, %g, %g)\n",
      a.x,a.y,a.z, b.x,b.y,b.z, p, q);
    str.Append(buf);
    ecyltan->DecRef();
    return;
  }
  iEmitSphereTangent *espheretan =
    SCF_QUERY_INTERFACE(emit, iEmitSphereTangent);
  if(espheretan)
  {
    espheretan->GetContent(a, p, q);
    sprintf(buf, "  EMITSPHERETANGENT(%g,%g,%g, %g, %g)\n", a.x,a.y,a.z, p, q);
    str.Append(buf);
    espheretan->DecRef();
    return;
  }
  printf ("Unknown emitter type, cannot writedown!\n");
}

void csEmitSaver::WriteDown (iBase* obj, iFile *file)
{
  csString str;
  iFactory *fact = SCF_QUERY_INTERFACE (this, iFactory);
  iParticleState *partstate = SCF_QUERY_INTERFACE (obj, iParticleState);
  iEmitState *state = SCF_QUERY_INTERFACE (obj, iEmitState);
  char buf[MAXLINE];
  char name[MAXLINE];

  csFindReplace(name, fact->QueryDescription (), "Saver", "Loader", MAXLINE);
  sprintf(buf, "FACTORY ('%s')\n", name);
  str.Append(buf);

  if(partstate->GetMixMode() != CS_FX_COPY)
  {
    str.Append (synldr->MixmodeToText (partstate->GetMixMode(), 2, true));
  }
  sprintf(buf, "MATERIAL (%s)\n", partstate->GetMaterialWrapper()->
    QueryObject()->GetName());
  str.Append(buf);
  printf(buf, "NUMBER (%d)\n", state->GetParticleCount());
  str.Append(buf);
  sprintf(buf, "LIGHTING (%s)\n", state->GetLighting()?"true":"false");
  str.Append(buf);
  sprintf(buf, "TOTALTIME (%d)\n", state->GetParticleTime());
  str.Append(buf);
  if(state->UsingRectParticles())
  {
    float w,h; state->GetRectParticles(w,h);
    sprintf(buf, "RECTPARTICLES (%g, %g)\n", w, h);
  }
  else
  {
    int n; float r; state->GetRegularParticles(n, r);
    sprintf(buf, "REGULARPARTICLES (%d, %g)\n", n, r);
  }
  str.Append(buf);

  str.Append("STARTPOS(\n");
  WriteEmit(str, state->GetStartPosEmit());
  str.Append(")\n");

  str.Append("STARTSPEED(\n");
  WriteEmit(str, state->GetStartSpeedEmit());
  str.Append(")\n");

  str.Append("STARTACCEL(\n");
  WriteEmit(str, state->GetStartAccelEmit());
  str.Append(")\n");

  if(state->GetAttractorEmit())
  {
    str.Append("ATTRACTOR(\n");
    WriteEmit(str, state->GetAttractorEmit());
    str.Append(")\n");
    sprintf(buf, "ATTRACTORFORCE (%g)\n", state->GetAttractorForce());
    str.Append(buf);
  }

  int i, time;
  for(i=0; i<state->GetAgingCount(); i++)
  {
    csColor col;
    float alpha, swirl, rotspeed, scale;
    state->GetAgingMoment(i, time, col, alpha, swirl, rotspeed, scale);
    sprintf(buf, "AGING (%d, %g,%g,%g, %g, %g, %g, %g)\n", time, col.red,
      col.green, col.blue, alpha, swirl, rotspeed, scale);
    str.Append(buf);
  }

  file->Write ((const char*)str, str.Length ());

  fact->DecRef();
  partstate->DecRef();
  state->DecRef();
}
