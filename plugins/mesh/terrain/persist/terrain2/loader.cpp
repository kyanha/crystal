/*
    Copyright (C) 2006 by Kapoulkine Arseny

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

#include "csutil/cscolor.h"
#include "csutil/sysfunc.h"
#include "csutil/stringarray.h"
#include "csutil/refarr.h"

#include "iengine/material.h"
#include "iengine/mesh.h"
#include "imap/services.h"
#include "imap/ldrctxt.h"
#include "imap/loader.h"
#include "imesh/object.h"
#include "imesh/terrain2.h"
#include "iutil/comp.h"
#include "iutil/document.h"
#include "iutil/object.h"
#include "iutil/objreg.h"
#include "iutil/plugin.h"

#include "loader.h"

CS_IMPLEMENT_PLUGIN

CS_PLUGIN_NAMESPACE_BEGIN(Terrain2Loader)
{
  
SCF_IMPLEMENT_FACTORY (csTerrain2FactoryLoader)
SCF_IMPLEMENT_FACTORY (csTerrain2ObjectLoader)


static const char* FACTORYERRORID = "crystalspace.mesh.loader.factory.terrain2";

csTerrain2FactoryLoader::csTerrain2FactoryLoader (iBase* parent)
 : scfImplementationType (this, parent)
{
}

csTerrain2FactoryLoader::~csTerrain2FactoryLoader ()
{
}

bool csTerrain2FactoryLoader::Initialize (iObjectRegistry* objreg)
{
  object_reg = objreg;
  reporter = csQueryRegistry<iReporter> (object_reg);
  synldr = csQueryRegistry<iSyntaxService> (object_reg);

  InitTokenTable (xmltokens);
  
  return true;
}

csPtr<iBase> csTerrain2FactoryLoader::Parse (iDocumentNode* node,
  iStreamSource*, iLoaderContext* ldr_context, iBase* /*context*/)
{
  csRef<iPluginManager> pluginManager = csQueryRegistry<iPluginManager> (object_reg);

  csRef<iMeshObjectType> meshType = csLoadPlugin<iMeshObjectType> (
    object_reg, "crystalspace.mesh.object.terrain2");

  if (!meshType)
  {
    synldr->ReportError (FACTORYERRORID, node, "Cannot load mesh object type plugin");
  }

  csRef<iMeshObjectFactory> meshFactory = meshType->NewFactory ();
  csRef<iTerrainFactory> factory = scfQueryInterface<iTerrainFactory> (meshFactory);

  csRef<iDocumentNodeIterator> it = node->GetNodes ();

  DefaultCellValues defaults;

  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();

    if (child->GetType () != CS_NODE_ELEMENT) 
      continue;

    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
    case XMLTOKEN_RENDERER:
      {
        const char* pluginname = child->GetContentsValue ();
        csRef<iTerrainRenderer> renderer = csLoadPluginCheck<iTerrainRenderer> (
          pluginManager, pluginname);

        if (!renderer)
        {
          synldr->ReportError (FACTORYERRORID, child, "Could not load %s!", pluginname);
          return 0;
        }

        factory->SetRenderer (renderer);
        break;
      }
    case XMLTOKEN_COLLIDER:
      {
        const char* pluginname = child->GetContentsValue ();
        csRef<iTerrainCollider> collider = csLoadPluginCheck<iTerrainCollider> (
          pluginManager, pluginname);

        if (!collider)
        {
          synldr->ReportError (FACTORYERRORID, child, "Could not load %s!", pluginname);
          return 0;
        }

        factory->SetCollider (collider);
        break;
      }
    case XMLTOKEN_FEEDER:
      {
        const char* pluginname = child->GetContentsValue ();
        csRef<iTerrainDataFeeder> feeder = csLoadPluginCheck<iTerrainDataFeeder> (
          pluginManager, pluginname);

        if (!feeder)
        {
          synldr->ReportError (FACTORYERRORID, child, "Could not load %s!", pluginname);
          return 0;
        }

        factory->SetFeeder (feeder);
        break;
      }
    case XMLTOKEN_CELLS:
      {
        csRef<iDocumentNodeIterator> it2 = child->GetNodes ();

        while (it2->HasNext ())
        {
          csRef<iDocumentNode> child2 = it2->Next ();

          if (child2->GetType () != CS_NODE_ELEMENT) 
            continue;

          const char* value = child2->GetValue ();
          csStringID id = xmltokens.Request (value);

          switch (id)
          {
          case XMLTOKEN_CELLDEFAULT:
            {
              if (!ParseDefaultCell (child2, ldr_context, defaults))
                return 0;

              break;
            }
          case XMLTOKEN_CELL:
            {
              if (!ParseCell (child2, ldr_context, factory, defaults))
                return 0;

              break;
            }
          default:
            {
              synldr->ReportBadToken (child2);
              return 0;
            }
          }
        }

        break;
      }
    case XMLTOKEN_MAXLOADEDCELLS:
      {
        factory->SetMaxLoadedCells (child->GetContentsValueAsInt ());
        break;
      }
    case XMLTOKEN_AUTOPRELOAD:
      {
        bool res;

        synldr->ParseBool (child, res, false);
        factory->SetAutoPreLoad (res);
        break;
      }
    case XMLTOKEN_VIRTUALVIEWDISTANCE:
      {
        factory->SetVirtualViewDistance (child->GetContentsValueAsFloat ());
        break;
      }
    default:
      {
        synldr->ReportBadToken (child);
        return 0;
      }
    }
  }

  return csPtr<iBase> (meshFactory);
}

bool csTerrain2FactoryLoader::ParseParams (csArray<ParamPair>& pairs, 
                                           iDocumentNode* node)
{
  csRef<iDocumentNodeIterator> it2 = node->GetNodes ();

  while (it2->HasNext ())
  {
    csRef<iDocumentNode> child2 = it2->Next ();

    if (child2->GetType () != CS_NODE_ELEMENT) 
      continue;

    const char* value = child2->GetValue ();
    csStringID id = xmltokens.Request (value);

    switch (id)
    {
    case XMLTOKEN_PARAM:
      {
        ParamPair p;
        p.name = child2->GetAttributeValue ("name");
        p.value = child2->GetContentsValue ();

        pairs.Push (p);
        break;
      }
    default:
      {
        synldr->ReportBadToken (child2);
        return false;
      }
    }
  }    

  return true;
}

bool csTerrain2FactoryLoader::ParseRenderParams (csArray<ParamPair>& pairs, 
                                                 csRefArray<csShaderVariable>& svs,
                                                 iLoaderContext* ldr_context,
                                                 iDocumentNode* node)
{
  csRef<iDocumentNodeIterator> it2 = node->GetNodes ();

  while (it2->HasNext ())
  {
    csRef<iDocumentNode> child2 = it2->Next ();

    if (child2->GetType () != CS_NODE_ELEMENT) 
      continue;

    const char* value = child2->GetValue ();
    csStringID id = xmltokens.Request (value);

    switch (id)
    {
    case XMLTOKEN_PARAM:
      {
        ParamPair p;
        p.name = child2->GetAttributeValue ("name");
        p.value = child2->GetContentsValue ();

        pairs.Push (p);
        break;
      }
    case XMLTOKEN_SHADERVAR:
      {
        csRef<csShaderVariable> sv;
        sv.AttachNew (new csShaderVariable);
        if (!synldr->ParseShaderVar (ldr_context, child2, *sv)) return false;
        svs.Push (sv);
        break;
      }
    default:
      {
        synldr->ReportBadToken (child2);
        return false;
      }
    }
  }    

  return true;
}

bool csTerrain2FactoryLoader::ParseFeederParams (ParamPairArray& pairs, 
  ParamPairArray& alphaMaps, iDocumentNode* node)
{
  csRef<iDocumentNodeIterator> it2 = node->GetNodes ();

  while (it2->HasNext ())
  {
    csRef<iDocumentNode> child2 = it2->Next ();

    if (child2->GetType () != CS_NODE_ELEMENT) 
      continue;

    const char* value = child2->GetValue ();
    csStringID id = xmltokens.Request (value);

    switch (id)
    {
    case XMLTOKEN_PARAM:
      {
        ParamPair p;
        p.name = child2->GetAttributeValue ("name");
        p.value = child2->GetContentsValue ();

        pairs.Push (p);
        break;
      }
    case XMLTOKEN_ALPHAMAP:
      {
        ParamPair p;
        p.name = child2->GetAttributeValue ("material");
        p.value = child2->GetContentsValue ();

        alphaMaps.Push (p);
      }
    default:
      {
        synldr->ReportBadToken (child2);
        return false;
      }
    }
  }    

  return true;
}

bool csTerrain2FactoryLoader::ParseCell (iDocumentNode *node, 
  iLoaderContext *ldr_ctx, iTerrainFactory *fact, const DefaultCellValues& defaults)
{
  ParamPairArray renderParams, collParams, feederParams, alphaMaps;
  csRefArray<csShaderVariable> svs;

  csString name;
  csVector3 size = defaults.size;
  csVector2 position (0,0);
  
  unsigned int gridWidth = defaults.gridWidth, gridHeight = defaults.gridHeight;
  unsigned int materialmapWidth = defaults.materialmapWidth, 
    materialmapHeight = defaults.materialmapHeight;

  bool materialmapPersist = defaults.materialmapPersist;
  csRef<iMaterialWrapper> baseMaterial = defaults.baseMaterial;

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();

    if (child->GetType () != CS_NODE_ELEMENT) 
      continue;

    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
    case XMLTOKEN_NAME:
      name = child->GetContentsValue ();
      break;
    case XMLTOKEN_SIZE:
      synldr->ParseVector (child, size);
      break;
    case XMLTOKEN_POSITION:
      synldr->ParseVector (child, position);
      break;
    case XMLTOKEN_GRIDSIZE:
      gridWidth = child->GetAttributeValueAsInt ("width");
      gridHeight = child->GetAttributeValueAsInt ("height");
      break;
    case XMLTOKEN_MATERIALMAPSIZE:
      materialmapWidth = child->GetAttributeValueAsInt ("width");
      materialmapHeight = child->GetAttributeValueAsInt ("height");
      break;
    case XMLTOKEN_BASEMATERIAL:
      {
        const char* matname = child->GetContentsValue ();
        baseMaterial = ldr_ctx->FindMaterial (matname);
        
        if (!baseMaterial)
        {
          synldr->ReportError (
            "crystalspace.terrain.object.loader.basematerial",
            child, "Couldn't find material '%s'!", matname);
          return false;
        }
        break;
      }
    case XMLTOKEN_MATERIALMAPPERSISTENT:
      synldr->ParseBool (child, materialmapPersist, false);
      break;
    case XMLTOKEN_RENDERPROPERTIES:
      {
        if (!ParseRenderParams (renderParams, svs, ldr_ctx, child))
          return false;

        break;
      }
    case XMLTOKEN_COLLIDERPROPERTIES:
      {
        if (!ParseParams (collParams, child))
          return false;

        break;
      }
    case XMLTOKEN_FEEDERPROPERTIES:
      {
        if (!ParseFeederParams (feederParams, alphaMaps, child))
          return false;

        break;
      }
    default:
      {
        synldr->ReportBadToken (child);
        return false;
      }
    }
  }

  // Time to build the cell
  iTerrainFactoryCell* cell = fact->AddCell (name.GetDataSafe (), gridWidth, gridHeight, 
    materialmapWidth, materialmapHeight, materialmapPersist, position, size);
  
  cell->SetBaseMaterial (baseMaterial);

  iTerrainCellRenderProperties* renderProperties = cell->GetRenderProperties ();
  for (size_t i = 0; i < defaults.renderParams.GetSize (); ++i)
  {
    renderProperties->SetParameter (defaults.renderParams[i].name.GetDataSafe (), 
      defaults.renderParams[i].value.GetDataSafe ());
  }
  for (size_t i = 0; i < defaults.svs.GetSize(); ++i)
  {
    renderProperties->AddVariable (defaults.svs[i]);
  }
  for (size_t i = 0; i < renderParams.GetSize (); ++i)
  {
    renderProperties->SetParameter (renderParams[i].name.GetDataSafe (), 
      renderParams[i].value.GetDataSafe ());
  }
  for (size_t i = 0; i < svs.GetSize(); ++i)
  {
    renderProperties->AddVariable (svs[i]);
  }

  iTerrainCellCollisionProperties* colliderProperties = cell->GetCollisionProperties ();
  for (size_t i = 0; i < defaults.collParams.GetSize (); ++i)
  {
    colliderProperties->SetParameter (defaults.collParams[i].name.GetDataSafe (), 
      defaults.collParams[i].value.GetDataSafe ());
  }
  for (size_t i = 0; i < collParams.GetSize (); ++i)
  {
    colliderProperties->SetParameter (collParams[i].name.GetDataSafe (), 
      collParams[i].value.GetDataSafe ());
  }

  iTerrainCellFeederProperties* feederProperties = cell->GetFeederProperties ();
  for (size_t i = 0; i < defaults.feederParams.GetSize (); ++i)
  {
    feederProperties->SetParameter (defaults.feederParams[i].name.GetDataSafe (), 
      defaults.feederParams[i].value.GetDataSafe ());
  }
  for (size_t i = 0; i < feederParams.GetSize (); ++i)
  {
    feederProperties->SetParameter (feederParams[i].name.GetDataSafe (), 
      feederParams[i].value.GetDataSafe ());
  }
  for (size_t i = 0; i < defaults.alphaMaps.GetSize (); ++i)
  {
    feederProperties->AddAlphaMap (defaults.alphaMaps[i].name.GetDataSafe (),
      defaults.alphaMaps[i].value.GetDataSafe ());
  }
  for (size_t i = 0; i < alphaMaps.GetSize (); ++i)
  {
    feederProperties->AddAlphaMap (alphaMaps[i].name.GetDataSafe (),
      alphaMaps[i].value.GetDataSafe ());
  }

  return true;
}

bool csTerrain2FactoryLoader::ParseDefaultCell (iDocumentNode* node, 
  iLoaderContext* ldr_ctx, DefaultCellValues& defaults)
{
  
  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();

    if (child->GetType () != CS_NODE_ELEMENT) 
      continue;

    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {    
    case XMLTOKEN_SIZE:
      synldr->ParseVector (child, defaults.size);
      break;
    case XMLTOKEN_GRIDSIZE:
      defaults.gridWidth = child->GetAttributeValueAsInt ("width");
      defaults.gridHeight = child->GetAttributeValueAsInt ("height");
      break;
    case XMLTOKEN_MATERIALMAPSIZE:
      defaults.materialmapWidth = child->GetAttributeValueAsInt ("width");
      defaults.materialmapHeight = child->GetAttributeValueAsInt ("height");
      break;
    case XMLTOKEN_BASEMATERIAL:
      {
        const char* matname = child->GetContentsValue ();
        defaults.baseMaterial = ldr_ctx->FindMaterial (matname);

        if (!defaults.baseMaterial)
        {
          synldr->ReportError (
            "crystalspace.terrain.object.loader.basematerial",
            child, "Couldn't find material '%s'!", matname);
          return false;
        }
        break;
      }
    case XMLTOKEN_MATERIALMAPPERSISTENT:
      synldr->ParseBool (child, defaults.materialmapPersist, false);
      break;
    case XMLTOKEN_RENDERPROPERTIES:
      {
        if (!ParseRenderParams (defaults.renderParams, defaults.svs, 
            ldr_ctx, child))
          return false;

        break;
      }
    case XMLTOKEN_COLLIDERPROPERTIES:
      {
        if (!ParseParams (defaults.collParams, child))
          return false;

        break;
      }
    case XMLTOKEN_FEEDERPROPERTIES:
      {
        if (!ParseFeederParams (defaults.feederParams, defaults.alphaMaps, child))
          return false;

        break;
      }
    default:
      {
        synldr->ReportBadToken (child);
        return false;
      }
    }
  }

  

  return true;
}


csTerrain2ObjectLoader::csTerrain2ObjectLoader (iBase* parent)
 : scfImplementationType (this, parent)
{
}

csTerrain2ObjectLoader::~csTerrain2ObjectLoader ()
{
}

bool csTerrain2ObjectLoader::Initialize (iObjectRegistry* objreg)
{
  object_reg = objreg;
  reporter = csQueryRegistry<iReporter> (object_reg);
  synldr = csQueryRegistry<iSyntaxService> (object_reg);

  InitTokenTable (xmltokens);

  return true;
}

csPtr<iBase> csTerrain2ObjectLoader::Parse (iDocumentNode* node, 
  iStreamSource*, iLoaderContext* ldr_context, iBase* /*context*/)
{
  csRef<iMeshObject> mesh;
  csRef<iTerrainSystem> terrain;

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char *value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_FACTORY:
      {
        const char* factname = child->GetContentsValue ();
        csRef<iMeshFactoryWrapper> fact = ldr_context->FindMeshFactory (
          factname);
        if (!fact)
        {
          synldr->ReportError ("crystalspace.terrain.object.loader",
            child, "Couldn't find factory '%s'!", factname);
          return 0;
        }
        mesh = fact->GetMeshObjectFactory ()->NewInstance ();
        terrain = scfQueryInterface<iTerrainSystem> (mesh);
            
        if (!terrain)
        {
          synldr->ReportError (
                    "crystalspace.terrain.parse.badfactory", child,
                    "Factory '%s' doesn't appear to be a terrain factory!",
                    factname);
          return 0;
        }
            
        break;
      }
      case XMLTOKEN_MATERIALPALETTE:
      {
        csRefArray<iMaterialWrapper> pal;
  
        csRef<iDocumentNodeIterator> it = child->GetNodes ();
        while (it->HasNext ())
        {
          csRef<iDocumentNode> child = it->Next ();
          if (child->GetType () != CS_NODE_ELEMENT) continue;
          const char *value = child->GetValue ();
          csStringID id = xmltokens.Request (value);
          switch (id)
          {
            case XMLTOKEN_MATERIAL:
            {
              const char* matname = child->GetContentsValue ();
              csRef<iMaterialWrapper> mat = ldr_context->FindMaterial (matname);
              if (!mat)
              {
                synldr->ReportError (
                  "crystalspace.terrain.object.loader.materialpalette",
                  child, "Couldn't find material '%s'!", matname);
                return 0;
              }
              pal.Push (mat);
              break;
            }
            default:
              synldr->ReportBadToken (child);
          }
        }

        terrain->SetMaterialPalette (pal);
        break;
      }
      default:
        synldr->ReportBadToken (child);
    }
  }

  return csPtr<iBase>(mesh);
}

}
CS_PLUGIN_NAMESPACE_END(Terrain2Loader)
