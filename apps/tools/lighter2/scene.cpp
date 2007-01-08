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
#include "object_genmesh.h"

using namespace CS;

namespace lighter
{
  void Sector::Initialize ()
  {
    // Initialize objects
    ObjectHash::GlobalIterator objIt = allObjects.GetIterator ();
    while (objIt.HasNext ())
    {
      csRef<Object> obj = objIt.Next ();
      obj->Initialize ();
    }

    // Build KD-tree
    objIt.Reset ();
    KDTreeBuilder builder;
    kdTree = builder.BuildTree (objIt);
  }

  Scene::Scene () : lightmapPostProc (this)
  {
  }

  Scene::~Scene ()
  {
    PDLightmapsHash::GlobalIterator pdlIt = pdLightmaps.GetIterator();
    while (pdlIt.HasNext())
    {
      LightmapPtrDelArray* lm = pdlIt.Next();
      delete lm;
    }
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
    // Parse sectors
    iSectorList *sectorList = globalLighter->engine->GetSectors ();
    for (int i = 0; i < sectorList->GetCount (); i++)
    {
      ParseSector (sectorList->Get (i));
    }

    // Parse portals
    SectorOrigSectorHash::GlobalIterator sectIt = originalSectorHash.GetIterator ();
    while (sectIt.HasNext ())
    {
      Sector* sector;
      csPtrKey<iSector> srcSector;

      sector = sectIt.Next (srcSector);
      ParsePortals (srcSector, sector);
    }

    // Propagate light in sectors
    sectIt.Reset ();
    while (sectIt.HasNext ())
    {
      Sector* sector = sectIt.Next ();
      
      LightRefArray tmpArray = sector->allNonPDLights;
      LightRefArray::Iterator lid = tmpArray.GetIterator ();
      while (lid.HasNext ())
      {
        Light* l = lid.Next ();
        if (l->IsRealLight ())
          PropagateLight (l, l->GetFrustum ());
      }

      tmpArray = sector->allPDLights;
      lid = tmpArray.GetIterator ();
      while (lid.HasNext ())
      {
        Light* l = lid.Next ();
        
        if (l->IsRealLight ())
          PropagateLight (l, l->GetFrustum ());
      }
    }

    return true;
  }


  Lightmap* Scene::GetLightmap (uint lightmapID, Light* light)
  {
    if (!light || !light->IsPDLight ())
      return lightmaps[lightmapID];

    LightmapPtrDelArray* pdLights = pdLightmaps.Get (light, 0);
    if (pdLights == 0)
    {
      pdLights = new LightmapPtrDelArray;
      for (size_t i = 0; i < lightmaps.GetSize(); i++)
      {
        Lightmap* lm = new Lightmap (lightmaps[i]->GetWidth(), 
          lightmaps[i]->GetHeight());
        lm->Initialize ();
        pdLights->Push (lm);
      }
      pdLightmaps.Put (light, pdLights);
    }
    return pdLights->Get (lightmapID);
  }

  csArray<LightmapPtrDelArray*> Scene::GetAllLightmaps ()
  {
    csArray<LightmapPtrDelArray*> allLms;
    allLms.Push (&lightmaps);

    PDLightmapsHash::GlobalIterator pdlIt = pdLightmaps.GetIterator();
    while (pdlIt.HasNext())
    {
      LightmapPtrDelArray* lm = pdlIt.Next();
      allLms.Push (lm);
    }

    return allLms;
  }

  void Scene::ParseSector (iSector *sector)
  {
    if (!sector) return;

    // Setup a sector struct
    Sector* radSector = new Sector (this);
    radSector->sectorName = sector->QueryObject ()->GetName ();
    sectors.Put (radSector->sectorName, radSector);
    originalSectorHash.Put (sector, radSector);

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
      if (light->GetDynamicType() == CS_LIGHT_DYNAMICTYPE_DYNAMIC) continue;

      bool isPD = light->GetDynamicType() == CS_LIGHT_DYNAMICTYPE_PSEUDO;

      // Atm, only point light
      csRef<PointLight> intLight;
      intLight.AttachNew (new PointLight (radSector));

      intLight->SetPosition (light->GetMovable ()->GetFullPosition ());
      intLight->SetColor (isPD ? csColor (1.0f) : light->GetColor ());

      intLight->SetAttenuation (light->GetAttenuationMode (),
        light->GetAttenuationConstants ());
      intLight->SetPDLight (isPD);
      intLight->SetLightID (light->GetLightID());

      intLight->SetRadius (light->GetCutoffDistance ());

      if (isPD)
        radSector->allPDLights.Push (intLight);
      else
        radSector->allNonPDLights.Push (intLight);
    }
  }

  void Scene::ParsePortals (iSector *srcSect, Sector* sector)
  {
    // Parse portals
    const csSet<csPtrKey<iMeshWrapper> >& allPortals = srcSect->GetPortalMeshes ();
    csSet<csPtrKey<iMeshWrapper> >::GlobalIterator it = allPortals.GetIterator ();

    while (it.HasNext ())
    {
      iMeshWrapper *portalMesh = it.Next ();
      iPortalContainer *portalCont = portalMesh->GetPortalContainer ();

      for (int i = 0; i < portalCont->GetPortalCount (); ++i)
      {
        iPortal* portal = portalCont->GetPortal (i);
        portal->CompleteSector (0);

        iSector* destSector = portal->GetSector ();
        if (!destSector)
          continue;

        csRef<Portal> lightPortal; lightPortal.AttachNew (new Portal);
        lightPortal->sourceSector = sector;
        lightPortal->destSector = originalSectorHash.Get (destSector, 0);
        lightPortal->portalPlane = portal->GetWorldPlane ();
        
        if (portal->GetFlags ().Check (CS_PORTAL_WARP))
        {
          //Compute wrapping
          portal->ObjectToWorld (portalMesh->GetMovable ()->GetFullTransform (),
            lightPortal->wrapTransform);
        }

        // Get vertices, in world space
        int* vi = portal->GetVertexIndices ();
        const csVector3* vertices = portal->GetWorldVertices ();
        for (int j = 0; j < portal->GetVertexIndicesCount (); ++j)
        {
          lightPortal->worldVertices.Push (vertices[vi[j]]);
        }

        sector->allPortals.Push (lightPortal);
      }
    }
  }

  void Scene::PropagateLight (Light* light, const csFrustum& lightFrustum)
  {
    //Propagate light through all portals in its current sector, setup proxy-lights in targets
    Sector* sourceSector = light->GetSector ();
    const csVector3& lightCenter = light->GetPosition ();
    const csBox3& lightBB = light->GetBoundingBox ();

    PortalRefArray::Iterator it = sourceSector->allPortals.GetIterator ();
    while (it.HasNext ())
    {
      Portal* portal = it.Next ();
      
      if (portal->portalPlane.Classify (lightCenter) < -0.01f && //light in front of portal
        csIntersect3::BoxPlane (lightBB, portal->portalPlane)) //light at least cuts portal plane
      {
        csRef<csFrustum> newFrustum = lightFrustum.Intersect (
          portal->worldVertices.GetArray (), portal->worldVertices.GetSize ());

        if (newFrustum && !newFrustum->IsEmpty ())
        {
          //Have something left to push through, use that
          newFrustum->SetBackPlane (portal->portalPlane);
          newFrustum->Transform (&portal->wrapTransform);

          // Now, setup our proxy light
          csRef<ProxyLight> proxyLight;
          proxyLight.AttachNew (new ProxyLight (portal->destSector, light));
          
          if (proxyLight->IsPDLight ())
            portal->destSector->allPDLights.Push (proxyLight);
          else
            portal->destSector->allNonPDLights.Push (proxyLight);

          PropagateLight (proxyLight, proxyLight->GetFrustum ());
        }
      }
    }
  }

  Scene::MeshParseResult Scene::ParseMesh (Sector *sector, iMeshWrapper *mesh)
  {
    if (!sector || !mesh) return Failure;

    // Get the factory
    ObjectFactory *factory = 0;
    MeshParseResult parseFact = ParseMeshFactory (mesh->GetFactory (), factory);
    if (parseFact != Success) return parseFact;
    if (!factory) return Failure;

    // Construct a new mesh
    Object* obj = factory->CreateObject ();
    if (!obj) return Failure;

    obj->ParseMesh (mesh);
    obj->StripLightmaps (texturesToClean);

    // Save it
    sector->allObjects.Put (obj->meshName, obj);

    return Success;
  }

  Scene::MeshParseResult Scene::ParseMeshFactory (iMeshFactoryWrapper *factory,
                                                  ObjectFactory*& radFact)
  {
    if (!factory) return Failure;

    // Check for duplicate
    csString factName = factory->QueryObject ()->GetName ();
    radFact = radFactories.Get (factName, (ObjectFactory*)0);
    if (radFact) return Success;

    csRef<iFactory> ifact = scfQueryInterface<iFactory> (
      factory->GetMeshObjectFactory ()->GetMeshObjectType());

    const char* type = ifact->QueryClassID ();

    if (!strcasecmp (type, "crystalspace.mesh.object.genmesh"))
    {
      // Genmesh
      radFact = new ObjectFactory_Genmesh ();
    }
    else
      return NotAGenMesh;
    radFact->ParseFactory (factory);
    radFactories.Put (radFact->factoryName, radFact);

    return Success;
  }

  void Scene::CollectDeleteTextures (iDocumentNode* textureNode,
                                     csSet<csString>& filesToDelete)
  {
    csRef<iDocumentNode> fileNode = textureNode->GetNode ("file");
    if (fileNode.IsValid())
      filesToDelete.Add (fileNode->GetContentsValue());
    csRef<iDocumentNode> typeNode = textureNode->GetNode ("type");
    if (typeNode.IsValid())
    {
      const char* typeStr = typeNode->GetContentsValue();
      if (typeStr && (strcmp (typeStr,
          "crystalspace.texture.loader.pdlight") == 0))
      {
        csRef<iDocumentNode> paramsNode = textureNode->GetNode ("params");
        if (paramsNode.IsValid())
        {
          csRef<iDocumentNodeIterator> mapNodes = 
            paramsNode->GetNodes ("map");
          while (mapNodes->HasNext())
          {
            csRef<iDocumentNode> mapNode = mapNodes->Next();
            if (mapNode->GetType() != CS_NODE_ELEMENT) continue;
            const char* mapName = mapNode->GetContentsValue();
            if (mapName != 0)
              filesToDelete.Add (mapName);
          }
        }
      }
    }
  }

  void Scene::CleanOldLightmaps (LoadedFile* fileInfo, 
                                 const csSet<csString>& texFileNames)
  {
    csVfsDirectoryChanger chdir (globalLighter->vfs);
    chdir.ChangeTo (fileInfo->directory);

    csSet<csString>::GlobalIterator iter = texFileNames.GetIterator();
    while (iter.HasNext ())
    {
      globalLighter->vfs->DeleteFile (iter.Next());
    }
  }

  void Scene::SaveSceneToDom (iDocumentNode* r, LoadedFile* fileInfo)
  {
    csStringSet pdlightNums;
    for (unsigned int i = 0; i < lightmaps.GetSize (); i++)
    {
      csString textureFilename;
      // Save the lightmap
      textureFilename = "lightmaps/";
      textureFilename += i;
      textureFilename += ".png";
      {
        // Texture file name is relative to world file
        csVfsDirectoryChanger chdir (globalLighter->vfs);
        chdir.ChangeTo (fileInfo->directory);
      #ifndef DUMP_NORMALS
        lightmapPostProc.ApplyAmbient (lightmaps[i]);
        lightmapPostProc.ApplyExposure (lightmaps[i]);
      #endif
        lightmaps[i]->SaveLightmap (textureFilename);
      }
      SaveTexture savetex;
      savetex.filename = textureFilename;
      savetex.texname = lightmaps[i]->GetTextureName();

      PDLightmapsHash::GlobalIterator pdlIt = pdLightmaps.GetIterator();
      while (pdlIt.HasNext())
      {
        csPtrKey<Light> key;
        LightmapPtrDelArray* lm = pdlIt.Next(key);
        if (lm->Get (i)->IsNull()) continue;

        csString lmID (key->GetLightID ().HexString());
        textureFilename = "lightmaps/";
        textureFilename += i;
        textureFilename += "_";
        textureFilename += pdlightNums.Request (lmID);
        textureFilename += ".png";

        {
          // Texture file name is relative to world file
          csVfsDirectoryChanger chdir (globalLighter->vfs);
          chdir.ChangeTo (fileInfo->directory);
        #ifndef DUMP_NORMALS
          lightmapPostProc.ApplyExposure (lm->Get (i));
        #endif
          lm->Get (i)->SaveLightmap (textureFilename);
        }
        savetex.pdLightmapFiles.Push (textureFilename);
        savetex.pdLightIDs.Push (lmID);
      }

      texturesToSave.Push (savetex);
    }

    csRef <iDocumentNode> worldRoot = r->GetNode ("world");

    csRef<iDocumentNodeIterator> it = worldRoot->GetNodes ();
    while (it->HasNext ())
    {
      csRef<iDocumentNode> node = it->Next ();
      if (node->GetType () != CS_NODE_ELEMENT) continue;
      
      const char* nodeName = node->GetValue ();

      if (!strcasecmp (nodeName, "library"))
      {
        HandleLibraryNode (node, fileInfo);
      }
      else if (!strcasecmp (nodeName, "meshfact"))
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

    csSet<csString> texFileNamesToDelete;
    for (unsigned int i = 0; i < texturesToSave.GetSize (); i++)
    {
      const SaveTexture& textureToSave = texturesToSave[i];

      csRef<iDocumentNode> textureNode;
      {
        csRef<iDocumentNodeIterator> textureNodes = 
          texturesNode->GetNodes ("texture");
        while (textureNodes->HasNext())
        {
          csRef<iDocumentNode> texNode = textureNodes->Next();
          if (texNode->GetType() != CS_NODE_ELEMENT) continue;
          const char* texName = texNode->GetAttributeValue ("name");
          if (texName 
            && (strcmp (texName, textureToSave.texname.GetData ()) == 0))
          {
            textureNode = texNode;
            break;
          }
        }
      }
      if (!textureNode.IsValid())
      {
        textureNode = texturesNode->CreateNodeBefore (CS_NODE_ELEMENT);
        textureNode->SetValue ("texture");
      }
      else
      {
        CollectDeleteTextures (textureNode, texFileNamesToDelete);
        textureNode->RemoveNodes ();
      }
      textureNode->SetAttribute ("name", textureToSave.texname.GetData ());
           
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
      filenameNode->SetValue (textureToSave.filename.GetData ());
      texFileNamesToDelete.Delete (textureToSave.filename);

      csRef<iDocumentNode> mipmapNode = 
        textureNode->CreateNodeBefore (CS_NODE_ELEMENT);
      mipmapNode->SetValue ("mipmap");

      csRef<iDocumentNode> mipmapContents =
        mipmapNode->CreateNodeBefore (CS_NODE_TEXT);
      mipmapContents->SetValue ("no");

      if (textureToSave.pdLightmapFiles.GetSize() > 0)
      {
        csRef<iDocumentNode> typeNode = 
          textureNode->CreateNodeBefore (CS_NODE_ELEMENT, fileNode);
        typeNode->SetValue ("type");
        csRef<iDocumentNode> typeContent = 
          typeNode->CreateNodeBefore (CS_NODE_TEXT, 0);
        typeContent->SetValue ("crystalspace.texture.loader.pdlight");

        csRef<iDocumentNode> pdlightParamsNode = 
          textureNode->CreateNodeBefore (CS_NODE_ELEMENT, 0);
        pdlightParamsNode->SetValue ("params");

        for (size_t p = 0; p < textureToSave.pdLightmapFiles.GetSize(); p++)
        {
          csRef<iDocumentNode> mapNode = 
            pdlightParamsNode->CreateNodeBefore (CS_NODE_ELEMENT, 0);
          mapNode->SetValue ("map");
          mapNode->SetAttribute ("lightid", textureToSave.pdLightIDs[p]);

          csRef<iDocumentNode> mapContents = 
            mapNode->CreateNodeBefore (CS_NODE_TEXT, 0);
          mapContents->SetValue (textureToSave.pdLightmapFiles[p]);
          texFileNamesToDelete.Delete (textureToSave.pdLightmapFiles[p]);
        }
      }

      texturesToClean.Delete (textureToSave.texname.GetData ());
    }

    // Clean out old lightmap textures
    {
      csRefArray<iDocumentNode> nodesToDelete;
      csRef<iDocumentNodeIterator> it = texturesNode->GetNodes ("texture");
      while (it->HasNext())
      {
        csRef<iDocumentNode> child = it->Next();
        if (child->GetType() != CS_NODE_ELEMENT) continue;

        const char* name = child->GetAttributeValue ("name");
        if ((name != 0) && texturesToClean.Contains (name))
        {
          CollectDeleteTextures (child, texFileNamesToDelete);
          nodesToDelete.Push (child);
        }
      }
      for (size_t i = 0; i < nodesToDelete.GetSize(); i++)
        texturesNode->RemoveNode (nodesToDelete[i]);

      CleanOldLightmaps (fileInfo, texFileNamesToDelete);
    }

    DocumentHelper::RemoveDuplicateChildren(texturesNode, 
      texturesNode->GetNodes ("texture"),
      DocumentHelper::NodeAttributeCompare("name"));
  }

  bool Scene::SaveSceneLibrary (const char* libFile, LoadedFile* fileInfo)
  {
    csRef<iFile> buf = globalLighter->vfs->Open (libFile, VFS_FILE_READ);
    if (!buf) 
    {
      globalLighter->Report ("Error opening file '%s'!", libFile);
      return false;
    }

    csRef<iDocument> doc = globalLighter->docSystem->CreateDocument ();
    const char* error = doc->Parse (buf, true);
    if (error != 0)
    {
      globalLighter->Report ("Document system error: %s!", error);
      return false;
    }

    csRef<iDocumentNode> docRoot = doc->GetRoot();

    csRef<iDocumentNode> libRoot = docRoot->GetNode ("library");
    if (!libRoot)
    {
      globalLighter->Report ("'%s' is not a library", libFile);
      return false;
    }

    csRef<iDocumentNodeIterator> it = libRoot->GetNodes ();
    while (it->HasNext ())
    {
      csRef<iDocumentNode> node = it->Next ();
      if (node->GetType () != CS_NODE_ELEMENT) continue;
      
      const char* nodeName = node->GetValue ();

      if (!strcasecmp (nodeName, "library"))
      {
        HandleLibraryNode (node, fileInfo);
      }
      else if (!strcasecmp (nodeName, "meshfact"))
      {
        SaveMeshFactoryToDom (node, fileInfo);
      }
    }

    buf = globalLighter->vfs->Open (libFile, VFS_FILE_WRITE);
    if (!buf) 
    {
      globalLighter->Report ("Error opening file '%s' for writing!", libFile);
      return false;
    }
    error = doc->Write (buf);
    if (error != 0)
    {
      globalLighter->Report ("Document system error: %s!", error);
      return false;
    }

    return true;
  }

  void Scene::HandleLibraryNode (iDocumentNode* node, LoadedFile* fileInfo)
  {
    const char* file = node->GetAttributeValue ("file");
    if (file)
    {
      csVfsDirectoryChanger changer (globalLighter->vfs);
      const char* path = node->GetAttributeValue ("path");

      if (path)
      {
        changer.PushDir ();
        globalLighter->vfs->ChDir (path);
      }
      SaveSceneLibrary (file, fileInfo);
    }
    else
    {
      SaveSceneLibrary (node->GetContentsValue (), fileInfo);
    }
  }

  void Scene::SaveMeshFactoryToDom (iDocumentNode* factNode, LoadedFile* fileInfo)
  {
    // Save a single factory to the dom
    csString name = factNode->GetAttributeValue ("name");
    csRef<ObjectFactory> radFact = radFactories.Get (name, 
      (ObjectFactory*)0);
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
    csRef<Sector> sector = sectors.Get (name, (Sector*)0);
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
    csRef<Object> radObj = sect->allObjects.Get (name, (Object*)0);
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

  //-------------------------------------------------------------------------

  Scene::LightingPostProcessor::LightingPostProcessor (Scene* scene) : scene (scene)
  {
  }

  void Scene::LightingPostProcessor::ApplyExposure (Lightmap* lightmap)
  {
    // 0.5 to account for the fact that the shader does *2
    lightmap->ApplyExposureFunction (1.8f, 0.5f);
  }

  void Scene::LightingPostProcessor::ApplyExposure (csColor* colors, size_t numColors)
  {
    // @@@ ATM shader does *not* do *2 for vertex lighting
    LightmapPostProcess::ApplyExposureFunction(colors, numColors, 1.8f, 1.0f);
  }
  
  void Scene::LightingPostProcessor::ApplyAmbient (Lightmap* lightmap)
  {
    //if (!<indirect lighting enabled>)
    {
      csColor amb;
      globalLighter->engine->GetAmbientLight (amb);
      lightmap->AddAmbientTerm (amb);
    }
  }

  void Scene::LightingPostProcessor::ApplyAmbient (csColor* colors, size_t numColors)
  {
    //if (!<indirect lighting enabled>)
    {
      csColor amb;
      globalLighter->engine->GetAmbientLight (amb);
      LightmapPostProcess::AddAmbientTerm (colors, numColors, amb);
    }
  }

}
