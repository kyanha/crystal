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

enum
{
  XMLTOKEN_AGING = 1,
  XMLTOKEN_ATTRACTORFORCE,
  XMLTOKEN_ATTRACTOR,
  XMLTOKEN_CONTAINERBOX,
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
  XMLTOKEN_FIELDSPEED,
  XMLTOKEN_FIELDACCEL,
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


csEmitFactoryLoader::csEmitFactoryLoader (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csEmitFactoryLoader::~csEmitFactoryLoader ()
{
}

bool csEmitFactoryLoader::Initialize (iObjectRegistry* object_reg)
{
  csEmitFactoryLoader::object_reg = object_reg;
  return true;
}

csPtr<iBase> csEmitFactoryLoader::Parse (iDocumentNode* /*node*/,
	iLoaderContext*, iBase* /* context */)
{
  csRef<iPluginManager> plugin_mgr (CS_QUERY_REGISTRY (object_reg,
		iPluginManager));
  csRef<iMeshObjectType> type (CS_QUERY_PLUGIN_CLASS (plugin_mgr,
  	"crystalspace.mesh.object.emit", iMeshObjectType));
  if (!type)
  {
    type = CS_LOAD_PLUGIN (plugin_mgr, "crystalspace.mesh.object.emit",
    	iMeshObjectType);
    #ifdef CS_DEBUG
      printf ("Load TYPE plugin crystalspace.mesh.object.emit\n");
    #endif  
  }
  csRef<iMeshObjectFactory> fact (type->NewFactory ());
  return csPtr<iBase> (fact);
}

//---------------------------------------------------------------------------

csEmitFactorySaver::csEmitFactorySaver (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csEmitFactorySaver::~csEmitFactorySaver ()
{
}

bool csEmitFactorySaver::Initialize (iObjectRegistry* object_reg)
{
  csEmitFactorySaver::object_reg = object_reg;
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
}

csEmitLoader::~csEmitLoader ()
{
}

bool csEmitLoader::Initialize (iObjectRegistry* object_reg)
{
  csEmitLoader::object_reg = object_reg;
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);

  xmltokens.Register ("aging", XMLTOKEN_AGING);
  xmltokens.Register ("attractorforce", XMLTOKEN_ATTRACTORFORCE);
  xmltokens.Register ("attractor", XMLTOKEN_ATTRACTOR);
  xmltokens.Register ("containerbox", XMLTOKEN_CONTAINERBOX);
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
  xmltokens.Register ("fieldspeed", XMLTOKEN_FIELDSPEED);
  xmltokens.Register ("fieldaccel", XMLTOKEN_FIELDACCEL);
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

iEmitGen3D* csEmitLoader::ParseEmit (iDocumentNode* node,
			      iEmitFactoryState *fstate, float* weight)
{
  iEmitGen3D* result = 0;
  iEmitMix *emix = 0;
  csVector3 a;
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
	if (weight == 0)
	{
	  synldr->ReportError ("crystalspace.emitloader.parse",
		child, "'weight' cannot be given in this context!");
	  return 0;
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
	    return 0;
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
	    return 0;
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
	    return 0;
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
	    return 0;
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
	    return 0;
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
	return 0;
    }
  }
  return result;
}

csPtr<iBase> csEmitLoader::Parse (iDocumentNode* node,
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
	    return 0;
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
            return 0;
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
	    return 0;
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
            return 0;
	  }
	  alpha = alphanode->GetContentsValueAsFloat ();
	  csRef<iDocumentNode> swirlnode = child->GetNode ("swirl");
	  if (!swirlnode)
	  {
	    synldr->ReportError ("crystalspace.emitloader.parse",
		child, "Missing 'swirl' in 'aging'!");
            return 0;
	  }
	  swirl = swirlnode->GetContentsValueAsFloat ();
	  csRef<iDocumentNode> scalenode = child->GetNode ("scale");
	  if (!scalenode)
	  {
	    synldr->ReportError ("crystalspace.emitloader.parse",
		child, "Missing 'scale' in 'aging'!");
            return 0;
	  }
	  scale = scalenode->GetContentsValueAsFloat ();
	  csRef<iDocumentNode> rotspeednode = child->GetNode ("rotspeed");
	  if (!rotspeednode)
	  {
	    synldr->ReportError ("crystalspace.emitloader.parse",
		child, "Missing 'rotspeed' in 'aging'!");
            return 0;
	  }
	  rotspeed = rotspeednode->GetContentsValueAsFloat ();
	  csRef<iDocumentNode> timenode = child->GetNode ("time");
	  if (!timenode)
	  {
	    synldr->ReportError ("crystalspace.emitloader.parse",
		child, "Missing 'time' in 'aging'!");
            return 0;
	  }
	  time = timenode->GetContentsValueAsInt ();
	  csRef<iDocumentNode> colornode = child->GetNode ("color");
	  if (colornode)
	    if (!synldr->ParseColor (colornode, col))
	      return 0;
	  emitstate->AddAge (time, col, alpha, swirl, rotspeed, scale);
	}
	break;
      case XMLTOKEN_STARTPOS:
	{
	  emit = ParseEmit (child, emitfactorystate, 0);
	  emitstate->SetStartPosEmit (emit);
	  SCF_DEC_REF (emit);
	}
	break;
      case XMLTOKEN_STARTSPEED:
	{
	  emit = ParseEmit (child, emitfactorystate, 0);
	  emitstate->SetStartSpeedEmit (emit);
	  SCF_DEC_REF (emit);
	}
	break;
      case XMLTOKEN_STARTACCEL:
	{
	  emit = ParseEmit (child, emitfactorystate, 0);
	  emitstate->SetStartAccelEmit (emit);
	  SCF_DEC_REF (emit);
	}
	break;
      case XMLTOKEN_ATTRACTORFORCE:
	emitstate->SetAttractorForce (child->GetContentsValueAsFloat ());
	break;
      case XMLTOKEN_ATTRACTOR:
	{
	  emit = ParseEmit (child, emitfactorystate, 0);
	  emitstate->SetAttractorEmit (emit);
	  SCF_DEC_REF (emit);
	}
	break;
      case XMLTOKEN_FIELDSPEED:
	{
	  emit = ParseEmit (child, emitfactorystate, 0);
	  emitstate->SetFieldSpeedEmit (emit);
	  SCF_DEC_REF (emit);
	}
	break;
      case XMLTOKEN_FIELDACCEL:
	{
	  emit = ParseEmit (child, emitfactorystate, 0);
	  emitstate->SetFieldAccelEmit (emit);
	  SCF_DEC_REF (emit);
	}
	break;
      case XMLTOKEN_CONTAINERBOX:
	{
	  csBox3 box;
	  if(!synldr->ParseBox(child, box))
	    return 0;
	  emitstate->SetContainerBox(true, box.Min(), box.Max());
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


csEmitSaver::csEmitSaver (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csEmitSaver::~csEmitSaver ()
{
}

bool csEmitSaver::Initialize (iObjectRegistry* object_reg)
{
  csEmitSaver::object_reg = object_reg;
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  return true;
}

/// write emitter to string
static void WriteEmit(csString& str, iEmitGen3D *emit)
{
  char buf[MAXLINE];
  csVector3 a,b;
  float p, q, r, s, t;
  csRef<iEmitFixed> efixed (SCF_QUERY_INTERFACE(emit, iEmitFixed));
  if(efixed)
  {
    /// b is ignored
    efixed->GetValue(a, b);
    sprintf(buf, "  EMITFIXED(%g, %g, %g)\n", a.x, a.y, a.z);
    str.Append(buf);
    return;
  }
  csRef<iEmitSphere> esphere (SCF_QUERY_INTERFACE(emit, iEmitSphere));
  if(esphere)
  {
    esphere->GetContent(a, p, q);
    sprintf(buf, "  EMITSPHERE(%g,%g,%g, %g, %g)\n", a.x, a.y, a.z, p, q);
    str.Append(buf);
    return;
  }
  csRef<iEmitBox> ebox (SCF_QUERY_INTERFACE(emit, iEmitBox));
  if(ebox)
  {
    ebox->GetContent(a, b);
    sprintf(buf, "  EMITBOX(%g,%g,%g, %g,%g,%g)\n", a.x,a.y,a.z, b.x,b.y,b.z);
    str.Append(buf);
    return;
  }
  csRef<iEmitCone> econe (SCF_QUERY_INTERFACE(emit, iEmitCone));
  if(econe)
  {
    econe->GetContent(a, p, q, r, s, t);
    sprintf(buf, "  EMITCONE(%g,%g,%g, %g, %g, %g, %g, %g)\n", a.x,a.y,a.z,
      p, q, r, s, t);
    str.Append(buf);
    return;
  }
  csRef<iEmitMix> emix (SCF_QUERY_INTERFACE(emit, iEmitMix));
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
    return;
  }
  csRef<iEmitLine> eline (SCF_QUERY_INTERFACE(emit, iEmitLine));
  if(eline)
  {
    eline->GetContent(a, b);
    sprintf(buf, "  EMITLINE(%g,%g,%g, %g,%g,%g)\n", a.x,a.y,a.z, b.x,b.y,b.z);
    str.Append(buf);
    return;
  }
  csRef<iEmitCylinder> ecyl (SCF_QUERY_INTERFACE(emit, iEmitCylinder));
  if(ecyl)
  {
    ecyl->GetContent(a, b, p, q);
    sprintf(buf, "  EMITCYLINDER(%g,%g,%g, %g,%g,%g, %g, %g)\n", a.x,a.y,a.z,
      b.x,b.y,b.z, p, q);
    str.Append(buf);
    return;
  }
  csRef<iEmitCylinderTangent> ecyltan (
    SCF_QUERY_INTERFACE(emit, iEmitCylinderTangent));
  if(ecyltan)
  {
    ecyltan->GetContent(a, b, p, q);
    sprintf(buf, "  EMITCYLINDERTANGENT(%g,%g,%g, %g,%g,%g, %g, %g)\n",
      a.x,a.y,a.z, b.x,b.y,b.z, p, q);
    str.Append(buf);
    return;
  }
  csRef<iEmitSphereTangent> espheretan (
    SCF_QUERY_INTERFACE(emit, iEmitSphereTangent));
  if(espheretan)
  {
    espheretan->GetContent(a, p, q);
    sprintf(buf, "  EMITSPHERETANGENT(%g,%g,%g, %g, %g)\n", a.x,a.y,a.z, p, q);
    str.Append(buf);
    return;
  }
  printf ("Unknown emitter type, cannot writedown!\n");
}

void csEmitSaver::WriteDown (iBase*, iFile*)
{
}

