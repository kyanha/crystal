/*
    Copyright (C) 2003 by Boyan Hristov

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
#include "lghtngldr.h"
#include "imesh/object.h"
#include "iengine/mesh.h"
#include "iengine/engine.h"
#include "iutil/plugin.h"
#include "imesh/partsys.h"
#include "imesh/lghtng.h"
#include "ivideo/graph3d.h"
#include "qint.h"
#include "iutil/vfs.h"
#include "csutil/csstring.h"
#include "iutil/object.h"
#include "iutil/document.h"
#include "iengine/material.h"
#include "iutil/objreg.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "imap/ldrctxt.h"
#include "ivaria/reporter.h"

CS_IMPLEMENT_PLUGIN

enum
{
  XMLTOKEN_BANDWIDTH = 1,
  XMLTOKEN_DIRECTIONAL,
  XMLTOKEN_FACTORY,
  XMLTOKEN_INTERVAL,
  XMLTOKEN_LENGTH,
  XMLTOKEN_MATERIAL,
  XMLTOKEN_MIXMODE,
  XMLTOKEN_ORIGIN,
  XMLTOKEN_POINTCOUNT,
  XMLTOKEN_VIBRATION,
  XMLTOKEN_WILDNESS
};

SCF_IMPLEMENT_IBASE (csLightningFactoryLoader)
  SCF_IMPLEMENTS_INTERFACE (iLoaderPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csLightningFactoryLoader::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csLightningFactorySaver)
  SCF_IMPLEMENTS_INTERFACE (iSaverPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csLightningFactorySaver::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csLightningLoader)
  SCF_IMPLEMENTS_INTERFACE (iLoaderPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csLightningLoader::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csLightningSaver)
  SCF_IMPLEMENTS_INTERFACE (iSaverPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csLightningSaver::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csLightningFactoryLoader)
SCF_IMPLEMENT_FACTORY (csLightningFactorySaver)
SCF_IMPLEMENT_FACTORY (csLightningLoader)
SCF_IMPLEMENT_FACTORY (csLightningSaver)

SCF_EXPORT_CLASS_TABLE (lghtngldr)
  SCF_EXPORT_CLASS (csLightningFactoryLoader,
    "crystalspace.mesh.loader.factory.lightning",
    "Crystal Space Lightning Factory Loader")
  SCF_EXPORT_CLASS (csLightningFactorySaver, "crystalspace.mesh.saver.factory.lightning",
    "Crystal Space Lightning Factory Saver")
  SCF_EXPORT_CLASS (csLightningLoader, "crystalspace.mesh.loader.lightning",
    "Crystal Space Lightning Mesh Loader")
  SCF_EXPORT_CLASS (csLightningSaver, "crystalspace.mesh.saver.lightning",
    "Crystal Space Lightning Mesh Saver")
SCF_EXPORT_CLASS_TABLE_END

csLightningFactoryLoader::csLightningFactoryLoader (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csLightningFactoryLoader::~csLightningFactoryLoader ()
{
}

bool csLightningFactoryLoader::Initialize (iObjectRegistry* object_reg)
{
  csLightningFactoryLoader::object_reg = object_reg;
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);

  xmltokens.Register ("bandwidth", XMLTOKEN_BANDWIDTH);
  xmltokens.Register ("directional", XMLTOKEN_DIRECTIONAL);
  xmltokens.Register ("factory", XMLTOKEN_FACTORY);
  xmltokens.Register ("interval", XMLTOKEN_INTERVAL);
  xmltokens.Register ("length", XMLTOKEN_LENGTH);
  xmltokens.Register ("material", XMLTOKEN_MATERIAL);
  xmltokens.Register ("mixmode", XMLTOKEN_MIXMODE);
  xmltokens.Register ("origin", XMLTOKEN_ORIGIN);
  xmltokens.Register ("pointcount", XMLTOKEN_POINTCOUNT);
  xmltokens.Register ("vibration", XMLTOKEN_VIBRATION);
  xmltokens.Register ("wildness", XMLTOKEN_WILDNESS);
  return true;
}


csPtr<iBase> csLightningFactoryLoader::Parse (iDocumentNode* node,
        iLoaderContext* ldr_context,
        iBase* /* context */)
{
  csVector3 a;

  csRef<iPluginManager> plugin_mgr (CS_QUERY_REGISTRY (object_reg,
        iPluginManager));
  csRef<iMeshObjectType> type (CS_QUERY_PLUGIN_CLASS (plugin_mgr,
        "crystalspace.mesh.object.lightning", iMeshObjectType));
  if (!type)
  {
    type = CS_LOAD_PLUGIN (plugin_mgr, "crystalspace.mesh.object.lightning",
        iMeshObjectType);
    printf ("Load TYPE plugin crystalspace.mesh.object.lightning\n");
  }
  csRef<iMeshObjectFactory> fact;
  fact = type->NewFactory ();
  csRef<iLightningFactoryState> LightningFactoryState (
        SCF_QUERY_INTERFACE (fact, iLightningFactoryState));
  CS_ASSERT (LightningFactoryState);

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
                "crystalspace.lightningloader.parse.badmaterial",
                child, "Could not find material '%s'!", matname);
            return 0;
          }
          LightningFactoryState->SetMaterialWrapper (mat);
        }
        break;

      case XMLTOKEN_MIXMODE:
        {
          uint mode;
          if (!synldr->ParseMixmode (child, mode))
            return 0;
          LightningFactoryState->SetMixMode (mode);
        }
        break;

      case XMLTOKEN_ORIGIN:
        if (!synldr->ParseVector (child, a))
          return 0;
        LightningFactoryState->SetOrigin (a);
        break;

      case XMLTOKEN_DIRECTIONAL:
        if (!synldr->ParseVector (child, a))
          return 0;
        LightningFactoryState->SetDirectional (a);
        break;

      case XMLTOKEN_POINTCOUNT:
        {
          int pcount = child->GetContentsValueAsInt ();
          LightningFactoryState->SetPointCount (pcount);
        }
        break;

      case XMLTOKEN_LENGTH:
        {
          float len = child->GetContentsValueAsFloat ();
          LightningFactoryState->SetLength (len);
        }
        break;

      case XMLTOKEN_VIBRATION:
        {
          float vibr = child->GetContentsValueAsFloat ();
          LightningFactoryState->SetVibration (vibr);
        }
        break;

      case XMLTOKEN_WILDNESS:
        {
          float wildness = child->GetContentsValueAsFloat ();
          LightningFactoryState->SetWildness (wildness);
        }
        break;

      case XMLTOKEN_INTERVAL:
        {
          csTicks ticks = child->GetContentsValueAsInt ();
          LightningFactoryState->SetUpdateInterval (ticks);
        }
        break;

      default:
        synldr->ReportBadToken (child);
        return 0;
    }
  }

  return csPtr<iBase> (fact);
}

//---------------------------------------------------------------------------

csLightningFactorySaver::csLightningFactorySaver (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csLightningFactorySaver::~csLightningFactorySaver ()
{
}

bool csLightningFactorySaver::Initialize (iObjectRegistry* object_reg)
{
  csLightningFactorySaver::object_reg = object_reg;
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  return true;
}


void csLightningFactorySaver::WriteDown (iBase* obj, iFile *file)
{
  csString str;
  csRef<iLightningFactoryState> Lightningstate (
        SCF_QUERY_INTERFACE (obj, iLightningFactoryState));
  // @@@TODO
}

//---------------------------------------------------------------------------

csLightningLoader::csLightningLoader (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csLightningLoader::~csLightningLoader ()
{
}

bool csLightningLoader::Initialize (iObjectRegistry* object_reg)
{
  csLightningLoader::object_reg = object_reg;
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);

  xmltokens.Register ("factory", XMLTOKEN_FACTORY);
  // @@@TODO
  return true;
}

csPtr<iBase> csLightningLoader::Parse (iDocumentNode* node,
    iLoaderContext* ldr_context, iBase*)
{
  csRef<iMeshObject> mesh;
  csRef<iLightningFactoryState> LightningFactoryState;
  csRef<iLightningState> Lightningstate;

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
          iMeshFactoryWrapper* fact =
              ldr_context->FindMeshFactory (factname);
          if (!fact)
          {
            synldr->ReportError (
                "crystalspace.lightningloader.parse.badfactory",
                child, "Could not find factory '%s'!", factname);
            return 0;
          }
          mesh = fact->GetMeshObjectFactory ()->NewInstance ();
          Lightningstate = SCF_QUERY_INTERFACE (mesh, iLightningState);
          LightningFactoryState = SCF_QUERY_INTERFACE (
              fact->GetMeshObjectFactory(), iLightningFactoryState);
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


csLightningSaver::csLightningSaver (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csLightningSaver::~csLightningSaver ()
{
}

bool csLightningSaver::Initialize (iObjectRegistry* object_reg)
{
  csLightningSaver::object_reg = object_reg;
  return true;
}

void csLightningSaver::WriteDown (iBase* obj, iFile *file)
{
  // TODO
}
