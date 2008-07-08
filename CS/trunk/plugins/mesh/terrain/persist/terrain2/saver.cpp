/*
    Copyright (C) 2008 by Frank Richter

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

#include "iengine/material.h"
#include "iengine/mesh.h"
#include "imesh/object.h"
#include "imesh/terrain2.h"
#include "iutil/document.h"
#include "iutil/object.h"
#include "iutil/objreg.h"

#include "saver.h"

CS_PLUGIN_NAMESPACE_BEGIN(Terrain2Loader)
{
  SCF_IMPLEMENT_FACTORY (Terrain2FactorySaver)
  SCF_IMPLEMENT_FACTORY (Terrain2ObjectSaver)

  Terrain2FactorySaver::Terrain2FactorySaver (iBase* parent)
  : scfImplementationType (this, parent) {}

  Terrain2FactorySaver::~Terrain2FactorySaver () {}

  bool Terrain2FactorySaver::Initialize (iObjectRegistry *objreg)
  {
    synldr = csQueryRegistry<iSyntaxService> (objreg);

    return true;
  }

  bool Terrain2FactorySaver::WriteDown (iBase *obj, iDocumentNode *parent,
					iStreamSource *ssource)
  {
    if (!parent) return false; //you never know...

    if (obj)
    {
      csRef<iDocumentNode> paramsNode = 
	parent->CreateNodeBefore(CS_NODE_ELEMENT, 0);
      paramsNode->SetValue("params");

      csRef<iTerrainFactory> tfact = 
	scfQueryInterface<iTerrainFactory> (obj);
      if (!tfact) return false;

      {
	iTerrainRenderer* render (tfact->GetRenderer());
	csRef<iFactory> renderFact (scfQueryInterfaceSafe<iFactory> (render));
	if (renderFact.IsValid())
	{
	  csRef<iDocumentNode> node = 
	    paramsNode->CreateNodeBefore (CS_NODE_ELEMENT, 0);
	  node->SetValue ("renderer");
	  node->CreateNodeBefore (CS_NODE_TEXT, 0)
	    ->SetValue (renderFact->QueryClassID());
	}
      }
      {
	iTerrainCollider* collide (tfact->GetCollider());
	csRef<iFactory> collideFact (scfQueryInterfaceSafe<iFactory> (collide));
	if (collideFact.IsValid())
	{
	  csRef<iDocumentNode> node = 
	    paramsNode->CreateNodeBefore (CS_NODE_ELEMENT, 0);
	  node->SetValue ("collider");
	  node->CreateNodeBefore (CS_NODE_TEXT, 0)
	    ->SetValue (collideFact->QueryClassID());
	}
      }
      {
	iTerrainDataFeeder* feed (tfact->GetFeeder());
	csRef<iFactory> feedFact (scfQueryInterfaceSafe<iFactory> (feed));
	if (feedFact.IsValid())
	{
	  csRef<iDocumentNode> node = 
	    paramsNode->CreateNodeBefore (CS_NODE_ELEMENT, 0);
	  node->SetValue ("feeder");
	  node->CreateNodeBefore (CS_NODE_TEXT, 0)
	    ->SetValue (feedFact->QueryClassID());
	}
      }
      {
        csRef<iDocumentNode> node = 
          paramsNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
        node->SetValue ("maxloadedcells");
        node->CreateNodeBefore (CS_NODE_TEXT, 0)
	  ->SetValueAsInt ((int)tfact->GetMaxLoadedCells());
      }

      iTerrainFactoryCell* defaultCell = tfact->GetDefaultCell();

      csRef<iDocumentNode> cellsNode = 
        paramsNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
      cellsNode->SetValue ("cells");

      {
        csRef<iDocumentNode> defaultNode = 
          cellsNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
        defaultNode->SetValue ("celldefault");

	{
	  csRef<iDocumentNode> node = 
	    defaultNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
	  node->SetValue ("gridsize");
	  node->SetAttributeAsInt ("width", defaultCell->GetGridWidth());
	  node->SetAttributeAsInt ("height", defaultCell->GetGridHeight());
	}

	{
	  csRef<iDocumentNode> node = 
	    defaultNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
	  node->SetValue ("materialmapsize");
	  node->SetAttributeAsInt ("width", defaultCell->GetMaterialMapWidth());
	  node->SetAttributeAsInt ("height", defaultCell->GetMaterialMapHeight());
	}

	{
	  csRef<iDocumentNode> node = 
	    defaultNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
	  node->SetValue ("size");
	  synldr->WriteVector (node, defaultCell->GetSize());
	}

	{
	  csRef<iDocumentNode> node = 
	    defaultNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
	  node->SetValue ("basematerial");
	  node->CreateNodeBefore (CS_NODE_TEXT, 0)
	    ->SetValue (defaultCell->GetBaseMaterial()->QueryObject()->GetName());
	}

	synldr->WriteBool (defaultNode, "materialmappersistent",
	  defaultCell->GetMaterialPersistent(), false);

	{
	  iTerrainRenderer* render = tfact->GetRenderer();
	  csRef<iTerrainCellRenderProperties> defRenderProp =
	    render ? render->CreateProperties () : 0;

	  csRef<iDocumentNode> node = 
	    defaultNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
	  node->SetValue ("renderproperties");
	  if (!SaveProperties (node, defaultCell->GetRenderProperties(),
	      (iTerrainCellRenderProperties*)defRenderProp))
	    defaultNode->RemoveNode (node);
	}

	{
	  iTerrainDataFeeder* feeder = tfact->GetFeeder();
	  csRef<iTerrainCellFeederProperties> defFeedProp =
	    feeder ? feeder->CreateProperties () : 0;

	  csRef<iDocumentNode> node = 
	    defaultNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
	  node->SetValue ("feederproperties");
	  if (!SaveProperties (node, defaultCell->GetFeederProperties(),
	      (iTerrainCellFeederProperties*)defFeedProp))
	    defaultNode->RemoveNode (node);
	}

	{
	  iTerrainCollider* collider = tfact->GetCollider();
	  csRef<iTerrainCellCollisionProperties> defCollProp =
	    collider ? collider->CreateProperties () : 0;

	  csRef<iDocumentNode> node = 
	    defaultNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
	  node->SetValue ("colliderproperties");
	  if (!SaveProperties (node, defaultCell->GetCollisionProperties(),
	      (iTerrainCellCollisionProperties*)defCollProp))
	    defaultNode->RemoveNode (node);
	}
      }

      size_t numCells = tfact->GetCellCount();
      for (size_t c = 0; c < numCells; c++)
      {
	iTerrainFactoryCell* cell = tfact->GetCell (c);

        csRef<iDocumentNode> cellNode = 
          cellsNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
        cellNode->SetValue ("cell");
	cellNode->SetAttribute ("name", cell->GetName());

	{
	  csRef<iDocumentNode> node = 
	    cellNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
	  node->SetValue ("position");
	  synldr->WriteVector (node, cell->GetPosition());
	}

	if ((defaultCell->GetGridWidth() != cell->GetGridWidth())
	    || (defaultCell->GetGridHeight() != cell->GetGridHeight()))
	{
	  csRef<iDocumentNode> node = 
	    cellNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
	  node->SetValue ("gridsize");
	  node->SetAttributeAsInt ("width", cell->GetGridWidth());
	  node->SetAttributeAsInt ("height", cell->GetGridHeight());
	}

	if ((defaultCell->GetMaterialMapWidth() != cell->GetMaterialMapWidth())
	    || (defaultCell->GetMaterialMapHeight() != cell->GetMaterialMapHeight()))
	{
	  csRef<iDocumentNode> node = 
	    cellNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
	  node->SetValue ("materialmapsize");
	  node->SetAttributeAsInt ("width", cell->GetMaterialMapWidth());
	  node->SetAttributeAsInt ("height", cell->GetMaterialMapHeight());
	}

	if (defaultCell->GetSize() != cell->GetSize())
	{
	  csRef<iDocumentNode> node = 
	    cellNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
	  node->SetValue ("size");
	  synldr->WriteVector (node, cell->GetSize());
	}

	if (defaultCell->GetBaseMaterial() != cell->GetBaseMaterial())
	{
	  csRef<iDocumentNode> node = 
	    cellNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
	  node->SetValue ("basematerial");
	  node->CreateNodeBefore (CS_NODE_TEXT, 0)
	    ->SetValue (defaultCell->GetBaseMaterial()->QueryObject()->GetName());
	}

	synldr->WriteBool (cellNode, "materialmappersistent",
	  cell->GetMaterialPersistent(), defaultCell->GetMaterialPersistent());

	{
	  csRef<iDocumentNode> node = 
	    cellNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
	  node->SetValue ("renderproperties");
	  if (!SaveProperties (node, cell->GetRenderProperties(),
	      defaultCell->GetRenderProperties()))
	    cellNode->RemoveNode (node);
	}

	{
	  csRef<iDocumentNode> node = 
	    cellNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
	  node->SetValue ("feederproperties");
	  if (!SaveProperties (node, cell->GetFeederProperties(),
	      defaultCell->GetFeederProperties()))
	    cellNode->RemoveNode (node);
	}

	{
	  csRef<iDocumentNode> node = 
	    cellNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
	  node->SetValue ("colliderproperties");
	  if (!SaveProperties (node, cell->GetCollisionProperties(),
	      defaultCell->GetCollisionProperties()))
	    cellNode->RemoveNode (node);
	}
      }

    }
    return true;
  }


  template<typename IProp>
  bool Terrain2FactorySaver::SaveProperties (iDocumentNode* node, IProp* props,
					     IProp* dfltProp)
  {
    if (props == 0) return false;
    if (dfltProp == 0)
    {
      size_t numProps = props->GetParameterCount ();
      if (numProps == 0) return false;
      for (size_t p = 0; p < numProps; p++)
      {
	const char* v = props->GetParameterValue (p);
	if (v == 0) continue;
        csRef<iDocumentNode> parNode = 
          node->CreateNodeBefore(CS_NODE_ELEMENT, 0);
        parNode->SetValue ("param");
	parNode->SetAttribute ("name", props->GetParameterName (p));
	parNode->CreateNodeBefore (CS_NODE_TEXT, 0)
	  ->SetValue (v);
      }
      return true;
    }
    else
    {
      size_t numProps = props->GetParameterCount ();
      if (numProps == 0) return false;
      bool wroteProps = false;
      for (size_t p = 0; p < numProps; p++)
      {
	const char* dfltVal = dfltProp->GetParameterValue (
	  props->GetParameterName (p));
	const char* val = props->GetParameterValue (p);
	if (val == 0) continue;

	if ((dfltVal != 0)  && (strcmp (val, dfltVal) == 0)) continue;
        csRef<iDocumentNode> parNode = 
          node->CreateNodeBefore(CS_NODE_ELEMENT, 0);
        parNode->SetValue ("param");
	parNode->SetAttribute ("name", props->GetParameterName (p));
	parNode->CreateNodeBefore (CS_NODE_TEXT, 0)->SetValue (val);
	wroteProps = true;
      }
      return wroteProps;
    }
  }

  //-------------------------------------------------------------------------

  Terrain2ObjectSaver::Terrain2ObjectSaver (iBase* parent)
  : scfImplementationType (this, parent) {}
    
  Terrain2ObjectSaver::~Terrain2ObjectSaver () {}

  bool Terrain2ObjectSaver::Initialize (iObjectRegistry *objreg)
  {
    return true;
  }

  bool Terrain2ObjectSaver::WriteDown (iBase *obj, iDocumentNode *parent,
				       iStreamSource *ssource)
  {
    if (!parent) return false; //you never know...

    csRef<iDocumentNode> paramsNode = 
      parent->CreateNodeBefore(CS_NODE_ELEMENT, 0);
    paramsNode->SetValue("params");

    if (obj)
    {
      csRef<iTerrainSystem> tmesh = 
	scfQueryInterface<iTerrainSystem> (obj);
      if (!tmesh) return false;
      csRef<iMeshObject> mesh = 
	scfQueryInterface<iMeshObject> (obj);
      if (!mesh) return false;
      csRef<iTerrainFactory> tfact = 
	scfQueryInterface<iTerrainFactory> (mesh->GetFactory());

      // Write down factory tag
      iMeshFactoryWrapper* fact = mesh->GetFactory()->GetMeshFactoryWrapper ();
      if (fact)
      {
	const char* factname = fact->QueryObject()->GetName();
	if (factname && *factname)
	{
	  csRef<iDocumentNode> factNode = 
	    paramsNode->CreateNodeBefore(CS_NODE_ELEMENT, 0);
	  factNode->SetValue("factory");
	  factNode->CreateNodeBefore(CS_NODE_TEXT, 0)->SetValue(factname);
	}    
      }

      {
        csRef<iDocumentNode> matpalNode = 
          paramsNode->CreateNodeBefore (CS_NODE_ELEMENT, 0);
        matpalNode->SetValue ("materialpalette");

	const csTerrainMaterialPalette& matpal = tmesh->GetMaterialPalette();
	for (size_t i = 0; i < matpal.GetSize(); i++)
	{
	  csRef<iDocumentNode> node = 
	    matpalNode->CreateNodeBefore (CS_NODE_ELEMENT, 0);
	  node->SetValue ("material");
	  node->CreateNodeBefore (CS_NODE_TEXT, 0)
	    ->SetValue (matpal[i]->QueryObject()->GetName());
	}
      }

      // @@@ TODO: cells
    }
    return true;
  }

}
CS_PLUGIN_NAMESPACE_END(Terrain2Loader)
