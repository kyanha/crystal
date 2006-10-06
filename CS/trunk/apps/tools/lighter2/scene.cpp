/*
  Copyright (C) 2005 by Marten Svanfeldt

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

#include "crystalspace.h"

#include "scene.h"
#include "lighter.h"
#include "kdtree.h"
#include "statistics.h"
#include "radobject_genmesh.h"

using namespace CS;

namespace lighter
{
  void Sector::Initialize ()
  {
    // Initialize objects
    RadObjectHash::GlobalIterator objIt = allObjects.GetIterator ();
    while (objIt.HasNext ())
    {
      csRef<RadObject> obj = objIt.Next ();
      obj->Initialize ();
    }

    // Build KD-tree
    objIt.Reset ();
    kdTree = KDTreeBuilder::BuildTree (objIt);
  }


  Scene::Scene ()
  {
    
  }

  void Scene::AddFile (const char* directory)
  {
    //ugly check to see that file isn't loaded twice
    for (unsigned int i = 0; i < sceneFiles.GetSize (); i++)
    {
      if (sceneFiles[i].directory.Compare (directory))
      {
        return;
      }
    }

    // Now setup loading of it
    LoadedFile newFile;
    newFile.directory = directory;
    sceneFiles.Push (newFile);
  }

  bool Scene::LoadFiles ()
  {
    if (sceneFiles.GetSize () == 0) 
      return globalLighter->Report ("No files to load!");

    globalStats.SetTaskProgress ("Loading files", 5);

    const float div = 90.0f / sceneFiles.GetSize ();

    for (unsigned int i = 0; i < sceneFiles.GetSize (); i++)
    {
      //Change path
      csStringArray paths;
      paths.Push ("/lev/");
      if (!globalLighter->vfs->ChDirAuto (sceneFiles[i].directory, &paths, 0, "world"))
        return globalLighter->Report ("Error setting directory '%s'!", 
          sceneFiles[i].directory.GetData());

      // Load it
      csRef<iFile> buf = globalLighter->vfs->Open ("world", VFS_FILE_READ);
      if (!buf) return globalLighter->Report ("Error opening file 'world'!");

      csRef<iDocument> doc = globalLighter->docSystem->CreateDocument ();
      const char* error = doc->Parse (buf, true);
      if (error != 0)
      {
        return globalLighter->Report ("Document system error: %s!", error);
      }

      sceneFiles[i].document = doc;
      sceneFiles[i].rootNode = doc->GetRoot ();
      // ChDirAuto() may have created an automount, use that...
      sceneFiles[i].directory = globalLighter->vfs->GetCwd ();

      // Pass it to the loader
      iBase *res;
      if (!globalLighter->loader->Load (sceneFiles[i].rootNode, res))
        return globalLighter->Report ("Error loading file 'world'!");

      globalStats.SetTaskProgress ("Loading files", 5+(unsigned int)(div*(i+1)));
    }

    globalStats.SetTaskProgress ("Loading files", 100);
    return true;
  }

  bool Scene::SaveFiles ()
  {
    if (sceneFiles.GetSize () == 0) 
      return globalLighter->Report ("No files to save!");

    for (unsigned int i = 0; i < sceneFiles.GetSize (); i++)
    {
      //Traverse the DOM, save any factory we encounter
      SaveSceneToDom (sceneFiles[i].rootNode, &sceneFiles[i]);

      //Change path
      csStringArray paths;
      paths.Push ("/lev/");
      if (!globalLighter->vfs->ChDirAuto (sceneFiles[i].directory, &paths, 0, "world"))
        return globalLighter->Report ("Error setting directory '%s'!", 
          sceneFiles[i].directory.GetData());

      // Save it
      csRef<iFile> buf = globalLighter->vfs->Open ("world", VFS_FILE_WRITE);
      if (!buf) return globalLighter->Report ("Error opening file 'world'!");

      const char *err = sceneFiles[i].document->Write (buf);
      if (err)
        return globalLighter->Report ("Error writing file 'world': %s", err);

    }

    return true;
  }

  bool Scene::ParseEngine ()
  {
    int i=0;
 
    iSectorList *sectorList = globalLighter->engine->GetSectors ();
    for (i = 0; i < sectorList->GetCount (); i++)
    {
      ParseSector (sectorList->Get (i));
    }

    return true;
  }

  void Scene::ParseSector (iSector *sector)
  {
    if (!sector) return;

    // Setup a sector struct
    Sector* radSector = new Sector (this);
    radSector->sectorName = sector->QueryObject ()->GetName ();
    sectors.Put (radSector->sectorName, radSector);

    // Parse all meshes (should have selector later!)
    iMeshList *meshList = sector->GetMeshes ();
    int i;
    for (i = 0; i < meshList->GetCount (); i++)
    {
      iMeshWrapper* mesh = meshList->Get (i);
      if (ParseMesh (radSector, mesh) == Failure)
        globalLighter->Report ("Error parsing mesh '%s' in sector '%s'!", 
          mesh->QueryObject()->GetName(), radSector->sectorName.GetData ());
    }

    // Parse all lights (should have selector later!)
    iLightList *lightList = sector->GetLights ();
    for (i = 0; i < lightList->GetCount (); i++)
    {
      iLight *light = lightList->Get (i);
      csRef<Light> radLight; radLight.AttachNew (new Light);
      radLight->position = light->GetMovable ()->GetFullPosition ();
      radLight->color = light->GetColor ();
      radLight->attenuation = light->GetAttenuationMode ();
      radLight->attenuationConsts = light->GetAttenuationConstants();

      // Only point-lights for now
      float r = light->GetCutoffDistance ();
      radLight->boundingBox.SetSize (csVector3 (r));
      radLight->boundingBox.SetCenter (radLight->position);
     
      //add more
      radSector->allLights.Push (radLight);
    }
  }

  Scene::MeshParseResult Scene::ParseMesh (Sector *sector, iMeshWrapper *mesh)
  {
    if (!sector || !mesh) return Failure;

    // Get the factory
    RadObjectFactory *factory = 0;
    MeshParseResult parseFact = ParseMeshFactory (mesh->GetFactory (), factory);
    if (parseFact != Success) return parseFact;
    if (!factory) return Failure;

    // Construct a new mesh
    RadObject* obj = factory->CreateObject ();
    if (!obj) return Failure;

    obj->ParseMesh (mesh);
    obj->StripLightmaps (texturesToClean);

    // Save it
    sector->allObjects.Put (obj->meshName, obj);

    return Success;
  }

  Scene::MeshParseResult Scene::ParseMeshFactory (iMeshFactoryWrapper *factory,
                                                  RadObjectFactory*& radFact)
  {
    if (!factory) return Failure;

    // Check for duplicate
    csString factName = factory->QueryObject ()->GetName ();
    radFact = radFactories.Get (factName, 0);
    if (radFact) return Success;

    csRef<iFactory> ifact = scfQueryInterface<iFactory> (
      factory->GetMeshObjectFactory ()->GetMeshObjectType());

    const char* type = ifact->QueryClassID ();

    if (!strcasecmp (type, "crystalspace.mesh.object.genmesh"))
    {
      // Genmesh
      radFact = new RadObjectFactory_Genmesh ();
    }
    else
      return NotAGenMesh;
    radFact->ParseFactory (factory);
    radFactories.Put (radFact->factoryName, radFact);

    return Success;
  }

  void Scene::CleanOldLightmaps (LoadedFile* fileInfo, 
                                 const csStringArray& texFileNames)
  {
    csVfsDirectoryChanger chdir (globalLighter->vfs);
    chdir.ChangeTo (fileInfo->directory);

    for (size_t i = 0; i < texFileNames.GetSize(); i++)
    {
      globalLighter->vfs->DeleteFile (texFileNames[i]);
    }
  }

  void Scene::SaveSceneToDom (iDocumentNode* r, LoadedFile* fileInfo)
  {
    for (unsigned int i = 0; i < lightmaps.GetSize (); i++)
    {
      csString textureFilename;
      // Save the lightmap
      {
        textureFilename = "lightmaps/";
        textureFilename += i;
        textureFilename += ".png";
      }
      {
        // Texture file name is relative to world file
        csVfsDirectoryChanger chdir (globalLighter->vfs);
        chdir.ChangeTo (fileInfo->directory);
        lightmaps[i]->SaveLightmap (textureFilename);
      }
      SaveTexture savetex;
      savetex.filename = textureFilename;
      savetex.texname = lightmaps[i]->GetTextureName();
      texturesToSave.Push (savetex);
    }

    csRef <iDocumentNode> worldRoot = r->GetNode ("world");

    csRef<iDocumentNodeIterator> it = worldRoot->GetNodes ();
    while (it->HasNext ())
    {
      csRef<iDocumentNode> node = it->Next ();
      if (node->GetType () != CS_NODE_ELEMENT) continue;
      
      const char* nodeName = node->GetValue ();

      if (!strcasecmp (nodeName, "meshfact"))
      {
        SaveMeshFactoryToDom (node, fileInfo);
      }
      else if (!strcasecmp (nodeName, "sector"))
      {
        SaveSectorToDom (node, fileInfo);
      }
    }

    // And now save the textures
    // see if we have any <textures> node
    csRef<iDocumentNode> texturesNode = worldRoot->GetNode ("textures");
    if (!texturesNode)
    {
      csRef<iDocumentNode> firstNode;
      csRef<iDocumentNodeIterator> it = worldRoot->GetNodes ();
      if (it->HasNext ())
        firstNode = it->Next ();

      //Create one
      texturesNode = worldRoot->CreateNodeBefore (CS_NODE_ELEMENT, firstNode);
      texturesNode->SetValue ("textures");
    }

    for (unsigned int i = 0; i < texturesToSave.GetSize (); i++)
    {
      const csString& textureFile = texturesToSave[i].filename;
      const csString& textureName = texturesToSave[i].texname;
      csRef<iDocumentNode> textureNode = 
        texturesNode->CreateNodeBefore (CS_NODE_ELEMENT);
      textureNode->SetValue ("texture");
      textureNode->SetAttribute ("name", textureName.GetData ());
           
      csRef<iDocumentNode> classNode = 
        textureNode->CreateNodeBefore (CS_NODE_ELEMENT);
      classNode->SetValue ("class");
      csRef<iDocumentNode> classContNode = 
        classNode->CreateNodeBefore (CS_NODE_TEXT);
      classContNode->SetValue ("lightmap");

      csRef<iDocumentNode> fileNode = 
        textureNode->CreateNodeBefore (CS_NODE_ELEMENT);
      fileNode->SetValue ("file");

      csRef<iDocumentNode> filenameNode =
        fileNode->CreateNodeBefore (CS_NODE_TEXT);
      filenameNode->SetValue (textureFile.GetData ());

      texturesToClean.Delete (textureName);
    }

    // Clean out old lightmap textures
    {
      csStringArray texFileNames;
      csRefArray<iDocumentNode> nodesToDelete;
      csRef<iDocumentNodeIterator> it = texturesNode->GetNodes ("texture");
      while (it->HasNext())
      {
        csRef<iDocumentNode> child = it->Next();
        if (child->GetType() != CS_NODE_ELEMENT) continue;

        const char* name = child->GetAttributeValue ("name");
        if ((name != 0) && texturesToClean.Contains (name))
        {
          csRef<iDocumentNode> fileNode = child->GetNode ("file");
          if (fileNode.IsValid())
            texFileNames.Push (fileNode->GetContentsValue());
          nodesToDelete.Push (child);
        }
      }
      for (size_t i = 0; i < nodesToDelete.GetSize(); i++)
        texturesNode->RemoveNode (nodesToDelete[i]);

      CleanOldLightmaps (fileInfo, texFileNames);
    }

    DocumentHelper::RemoveDuplicateChildren(texturesNode, 
      texturesNode->GetNodes ("texture"),
      DocumentHelper::NodeAttributeCompare("name"));
  }

  void Scene::SaveMeshFactoryToDom (iDocumentNode* factNode, LoadedFile* fileInfo)
  {
    // Save a single factory to the dom
    csString name = factNode->GetAttributeValue ("name");
    csRef<RadObjectFactory> radFact = radFactories.Get (name, 0);
    if (radFact)
    {
      // We do have one
      radFact->SaveFactory (factNode);
    }

    // Check if we have any child factories
    csRef<iDocumentNodeIterator> it = factNode->GetNodes ("meshfact");
    while (it->HasNext ())
    {
      csRef<iDocumentNode> node = it->Next ();
      SaveMeshFactoryToDom (node, fileInfo);
    }
  }

  void Scene::SaveSectorToDom (iDocumentNode* sectorNode, LoadedFile* fileInfo)
  {
    csString name = sectorNode->GetAttributeValue ("name");
    csRef<Sector> sector = sectors.Get (name, 0);
    if (sector)
    {
      //ok, have the sector, try to save all objects
      csRef<iDocumentNodeIterator> it = sectorNode->GetNodes ("meshobj");
      while (it->HasNext ())
      {
        csRef<iDocumentNode> node = it->Next ();
        SaveMeshObjectToDom (node, sector, fileInfo);
      }
    }
  }

  void Scene::SaveMeshObjectToDom (iDocumentNode *objNode, Sector* sect, LoadedFile* fileInfo)
  {
    // Save the mesh
    csString name = objNode->GetAttributeValue ("name");
    csRef<RadObject> radObj = sect->allObjects.Get (name, 0);
    if (radObj)
    {
      // We do have one
      radObj->RenormalizeLightmapUVs (lightmaps);
      radObj->SaveMesh (this, objNode);

      // Remove any old lightmap svs
      objNode->RemoveNodes (DocumentHelper::FilterDocumentNodeIterator (
        objNode->GetNodes ("shadervar"),
        DocumentHelper::NodeAttributeRegexpTest ("name", "tex lightmap.*")));
      
    }

    // Check if we have any child factories
    csRef<iDocumentNodeIterator> it = objNode->GetNodes ("meshobj");
    while (it->HasNext ())
    {
      csRef<iDocumentNode> node = it->Next ();
      SaveMeshObjectToDom (node, sect, fileInfo);
    }
  }
}
