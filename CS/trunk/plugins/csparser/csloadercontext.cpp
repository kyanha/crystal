/*
    Copyright (C) 1998-2001 by Jorrit Tyberghein
    Copyright (C) 1998-2000 by Ivan Avramovic <ivan@avramovic.com>

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

#include "iengine/engine.h"
#include "iengine/material.h"
#include "imap/loader.h"
#include "imesh/genmesh.h"
#include "iutil/object.h"
#include "iutil/stringarray.h"
#include "ivaria/reporter.h"
#include "ivideo/material.h"

#include "csloadercontext.h"

using namespace CS::Threading;

CS_PLUGIN_NAMESPACE_BEGIN(csparser)
{
  csLoaderContext::csLoaderContext (iObjectRegistry* object_reg, iEngine* Engine,
    csThreadedLoader* loader, iCollection* collection, iMissingLoaderData* missingdata,
    uint keepFlags, bool do_verbose)
    : scfImplementationType (this), object_reg(object_reg), Engine(Engine), loader(loader),
    collection(collection), missingdata(missingdata), keepFlags(keepFlags), do_verbose(do_verbose)
  {
    tm = csQueryRegistry<iTextureManager>(object_reg);
  }

  csLoaderContext::~csLoaderContext ()
  {
  }

  iSector* csLoaderContext::FindSector(const char* name)
  {
    csRef<iSector> s;
    {
      CS::Threading::MutexScopedLock lock(loader->sectorsLock);
      for(size_t i=0; i<loader->loaderSectors.GetSize(); i++)
      {
        if(!strcmp(loader->loaderSectors[i]->QueryObject()->GetName(), name))
        {
          s = loader->loaderSectors[i];
          break;
        }
      }
    }

    if(!s.IsValid() && missingdata)
    {
      s = missingdata->MissingSector(name);
    }
    
    if(!s.IsValid())
    {
      s = Engine->FindSector(name, collection);
    }

    if(!s.IsValid() && collection)
    {
      s = Engine->FindSector(name);
    }

    return s;
  }

  iMaterialWrapper* csLoaderContext::FindMaterial(const char* name, bool dontWaitForLoad)
  {
    csRef<iMaterialWrapper> mat;

    if(FindAvailMaterial(name))
    {
      while(!mat.IsValid())
      {
        {
          CS::Threading::MutexScopedLock lock(loader->materialsLock);
          for(size_t i=0; i<loader->loaderMaterials.GetSize(); i++)
          {
            if(!strcmp(loader->loaderMaterials[i]->QueryObject()->GetName(), name))
            {
              mat = loader->loaderMaterials[i];
              return mat;
            }
          }
        }

        if(!mat.IsValid() && missingdata)
        {
          mat = missingdata->MissingMaterial(name, name);
        }

        if(!mat.IsValid())
        {
          mat = Engine->FindMaterial(name, collection);
        }

        if(!mat.IsValid() && collection)
        {
            mat = Engine->FindMaterial(name);
        }

        if(dontWaitForLoad)
        {
          break;
        }
      }
    }

    // General search, as the object may not have been added via the loader.
    if(!mat.IsValid() && missingdata)
    {
      mat = missingdata->MissingMaterial(name, name);
    }

    if(!mat.IsValid())
    {
      mat = Engine->FindMaterial(name, collection);
    }

    if(!mat.IsValid() && collection)
    {
      mat = Engine->FindMaterial(name);
    }
    
    // *** This is deprecated behaviour ***
    if(!dontWaitForLoad && !mat.IsValid())
    {
      ReportWarning("Could not find material '%s'. Creating material. This behaviour is deprecated.", name);
      if(missingdata)
      {
        mat = missingdata->MissingMaterial(name, name);
        if(mat)
        {
          return mat;
        }
      }

      iTextureWrapper* tex = FindTexture (name, false);
      if (tex)
      {
        // Add a default material with the same name as the texture
        csRef<iMaterial> material = Engine->CreateBaseMaterial (tex);
        // First we have to extract the optional region name from the name:
        char const* n = strchr (name, '/');
        if (!n) n = name;
        else n++;
        csRef<iMaterialWrapper> mat = Engine->GetMaterialList()->CreateMaterial (material, n);
        loader->AddMaterialToList(mat);

        if(collection)
        {
          collection->Add(mat->QueryObject());
        }

        tex->Register(tm);
        return mat;
      }
    }
    /// ***

    if(!mat.IsValid() && !dontWaitForLoad && do_verbose)
    {
      ReportNotify("Could not find material '%s'.", name);
    }

    return mat;
  }

  iMeshFactoryWrapper* csLoaderContext::FindMeshFactory(const char* name, bool dontWaitForLoad)
  {
    csRef<iMeshFactoryWrapper> fact;
    
    if(FindAvailMeshFact(name))
    {
      while(!fact.IsValid())
      {
        {
          CS::Threading::MutexScopedLock lock(loader->meshfactsLock);
          for(size_t i=0; i<loader->loaderMeshFactories.GetSize(); i++)
          {
            if(!strcmp(loader->loaderMeshFactories[i]->QueryObject()->GetName(), name))
            {
              fact = loader->loaderMeshFactories[i];
              return fact;
            }
          }
        }

        if(!fact.IsValid() && missingdata)
        {
          fact = missingdata->MissingFactory(name);
        }

        if(!fact.IsValid())
        {
          fact = Engine->FindMeshFactory(name, collection);
        }

        if(!fact.IsValid() && collection)
        {
            fact = Engine->FindMeshFactory(name);
        }

        if(dontWaitForLoad)
        {
          break;
        }

        for(size_t i=0; i<loader->failedMeshFacts->GetSize(); i++)
        {
          if(!strcmp(loader->failedMeshFacts->Get(i), name))
          {
            // Break out of the loop, it's never going to be loaded.
            break;
          }
        }
      }
    }

    // General search, as the object may not have been added via the loader.
    if(!fact.IsValid() && missingdata)
    {
      fact = missingdata->MissingFactory(name);
    }

    if(!fact.IsValid())
    {
      fact = Engine->FindMeshFactory(name, collection);
    }

    if(!fact.IsValid() && collection)
    {
      fact = Engine->FindMeshFactory(name);
    }

    if(!fact.IsValid() && !dontWaitForLoad && do_verbose)
    {
      ReportNotify("Could not find mesh factory '%s'.", name);
    }

    return fact;
  }

  iMeshWrapper* csLoaderContext::FindMeshObject(const char* name)
  {
    csRef<iMeshWrapper> mesh;

    if(FindAvailMeshObj(name))
    {
      while(!mesh.IsValid())
      {
        csRef<iThreadReturn> itr = loader->loadingMeshObjects.Get(name, NULL);
        if(itr.IsValid())
        {
          itr->Wait();
          if(itr->WasSuccessful())
          {
            mesh = scfQueryInterface<iMeshWrapper>(itr->GetResultRefPtr());
            return mesh;
          }            
        }

        {
          CS::Threading::MutexScopedLock lock(loader->meshesLock);
          for(size_t i=0; i<loader->loaderMeshes.GetSize(); i++)
          {
            if(!strcmp(loader->loaderMeshes[i]->QueryObject()->GetName(), name))
            {
              mesh = loader->loaderMeshes[i];
              return mesh;
            }
          }
        }

        if (!mesh.IsValid() && missingdata)
        {
          mesh = missingdata->MissingMesh(name);
        }

        if(!mesh.IsValid())
        {
          mesh = Engine->FindMeshObject(name, collection);
        }

        if(!mesh.IsValid() && collection)
        {
            mesh = Engine->FindMeshObject(name);
        }
      }
    }

    // General search, as the object may not have been added via the loader.
    if (!mesh.IsValid() && missingdata)
    {
      mesh = missingdata->MissingMesh(name);
    }

    if(!mesh.IsValid())
    {
      mesh = Engine->FindMeshObject(name, collection);
    }

    if(!mesh.IsValid() && collection)
    {
      mesh = Engine->FindMeshObject(name);
    }

    if(!mesh.IsValid() && do_verbose)
    {
      ReportNotify("Could not find mesh object '%s'.", name);
    }

    return mesh;
  }

  iLight* csLoaderContext::FindLight(const char *name)
  {
    csRef<iLight> light;
    if(FindAvailLight(name))
    {
      while(!light.IsValid())
      {
        {
          CS::Threading::MutexScopedLock lock(loader->lightsLock);
          light = loader->loadedLights.Get(csString(name), 0);
        }

        if(!light.IsValid() && missingdata)
        {
          light = missingdata->MissingLight(name);
        }

        if(!light.IsValid())
        {
          csRef<iLightIterator> li = Engine->GetLightIterator(collection);

          while(li->HasNext())
          {
            light = li->Next();
            if(!strcmp(light->QueryObject()->GetName(), name))
            {
              break;
            }
          }
        }

        if(!light.IsValid() && collection)
        {
          csRef<iLightIterator> li = Engine->GetLightIterator();

          while(li->HasNext())
          {
            light = li->Next();
            if(!strcmp(light->QueryObject()->GetName(), name))
            {
              break;
            }
          }
        }
      }
    }

    // General search, as the object may not have been added via the loader.
    if(!light.IsValid() && missingdata)
    {
      light = missingdata->MissingLight(name);
    }

    if(!light.IsValid())
    {
      csRef<iLightIterator> li = Engine->GetLightIterator(collection);

      while(li->HasNext())
      {
        light = li->Next();
        if(!strcmp(light->QueryObject()->GetName(), name))
        {
          break;
        }
      }
    }

    if(!light.IsValid() && collection)
    {
      csRef<iLightIterator> li = Engine->GetLightIterator();

      while(li->HasNext())
      {
        light = li->Next();
        if(!strcmp(light->QueryObject()->GetName(), name))
        {
          break;
        }
      }
    }

    if(!light.IsValid() && do_verbose)
    {
      ReportNotify("Could not find light '%s'.", name);
    }

    return light;
  }

  iShader* csLoaderContext::FindShader(const char *name)
  {
    csRef<iShaderManager> shaderMgr = csQueryRegistry<iShaderManager>(object_reg);

    if(!shaderMgr)
    {
      return 0;
    }

    iShader* shader = 0;

    // Always look up builtin shaders globally
    if(!collection || (name && *name == '*'))
    {
      if(missingdata)
      {
        shader = missingdata->MissingShader(name);
      }

      if(!shader)
      {
        shader = shaderMgr->GetShader(name);
      }
    }
    else
    {
      if(missingdata)
      {
        shader = missingdata->MissingShader(name);
      }

      if(!shader)
      {
        csRefArray<iShader> shaders = shaderMgr->GetShaders();
        for(size_t i=0; i<shaders.GetSize(); i++)
        {
          shader = shaders[i];
          if(collection)
          {
            if((collection->IsParentOf(shader->QueryObject()) ||
              collection->FindShader(shader->QueryObject()->GetName())) &&
              !strcmp(name, shader->QueryObject()->GetName()))
            {
              break;
            }
          }
        }
      }
    }

    if(!shader && do_verbose)
    {
      ReportNotify("Could not find shader '%s'.", name);
    }

    return shader;
  }

  iTextureWrapper* csLoaderContext::FindTexture(const char* name, bool dontWaitForLoad)
  {
    csRef<iTextureWrapper> result;

    if(FindAvailTexture(name))
    {
      while(!result.IsValid())
      {
        {
          CS::Threading::MutexScopedLock lock(loader->texturesLock);
          for(size_t i=0; i<loader->loaderTextures.GetSize(); i++)
          {
            if(loader->loaderTextures[i]->QueryObject()->GetName() &&
              !strcmp(loader->loaderTextures[i]->QueryObject()->GetName(), name))
            {
              result = loader->loaderTextures[i];
              return result;
            }
          }
        }

        if(!result.IsValid() && missingdata)
        {
          result = missingdata->MissingTexture (name, name);
        }

        if(!result.IsValid())
        {
          result = Engine->FindTexture(name, collection);
        }

        if(!result.IsValid() && collection)
        {
          result = Engine->FindTexture(name);
        }

        if(dontWaitForLoad)
        {
          break;
        }
      }
    }

    // General search, as the object may not have been added via the loader.
    if(!result.IsValid() && missingdata)
    {
      result = missingdata->MissingTexture (name, name);
    }

    if(!result.IsValid())
    {
      result = Engine->FindTexture(name, collection);
    }

    if(!result.IsValid() && collection)
    {
      result = Engine->FindTexture(name);
    }

    // *** This is deprecated behaviour ***
    if(!dontWaitForLoad && !result.IsValid())
    {
      ReportWarning("Could not find texture '%s'. Loading texture. This is deprecated behaviour.", 
        name);
      csRef<iThreadManager> tman = csQueryRegistry<iThreadManager>(object_reg);
      csRef<iThreadReturn> itr = csPtr<iThreadReturn>(new csLoaderReturn(tman));
      loader->LoadTextureTC(itr, name, name, CS_TEXTURE_3D, tm, true, false, true, collection,
        KEEP_ALL, do_verbose);
      result = scfQueryInterfaceSafe<iTextureWrapper>(itr->GetResultRefPtr());
    }
    // ***

    if(!result.IsValid() && !dontWaitForLoad && do_verbose)
    {
      ReportNotify ("Could not find texture '%s'. Attempting to load.", name);
    }

    return result;
  }

  iGeneralMeshSubMesh* csLoaderContext::FindSubmesh(iGeneralMeshState* state, const char* name)
  {
    csRef<iGeneralMeshSubMesh> submesh;
    if(FindAvailSubmesh(name))
    {
      while(!submesh.IsValid())
      {
        submesh = state->FindSubMesh(name);
      }
    }

    return submesh;
  }

  void csLoaderContext::AddToCollection(iObject* obj)
  {
    if(collection)
    {
      CS::Threading::MutexScopedLock lock(collectionLock);
      collection->Add(obj);
    }
  }

  void csLoaderContext::ReportNotify (const char* description, ...)
  {
    va_list arg;
    va_start (arg, description);
    csReportV (object_reg, CS_REPORTER_SEVERITY_NOTIFY, "crystalspace.maploader", description, arg);
    va_end (arg);
  }

  void csLoaderContext::ReportWarning (const char* description, ...)
  {
    va_list arg;
    va_start (arg, description);
    csReportV (object_reg, CS_REPORTER_SEVERITY_WARNING, "crystalspace.maploader", description, arg);
    va_end (arg);
  }

  void csLoaderContext::ParseAvailableTextures(iDocumentNode* doc)
  {
    csRef<iDocumentNodeIterator> itr = doc->GetNodes("texture");
    while(itr->HasNext())
    {
      csRef<iDocumentNode> texture = itr->Next();
      if(texture->GetAttributeValue("name"))
      {
        CS::Threading::MutexScopedLock lock(textureObjects);
        availTextures.PushSmart(texture->GetAttributeValue("name"));
      }
    }
  }

  void csLoaderContext::ParseAvailableMaterials(iDocumentNode* doc)
  {
    csRef<iDocumentNodeIterator> itr = doc->GetNodes("material");
    while(itr->HasNext())
    {
      csRef<iDocumentNode> material = itr->Next();
      if(material->GetAttributeValue("name"))
      {
        CS::Threading::MutexScopedLock lock(materialObjects);
        availMaterials.PushSmart(material->GetAttributeValue("name"));
      }
    }
  }

  void csLoaderContext::ParseAvailableMeshfacts(iDocumentNode* doc)
  {
    if(doc->GetAttributeValue("name"))
    {
      CS::Threading::MutexScopedLock lock(meshfactObjects);
      availMeshfacts.Push(doc->GetAttributeValue("name"));
    }

    if(doc->GetNode("params"))
    {
      ParseAvailableSubmeshes(doc->GetNode("params"));
    }

    csRef<iDocumentNodeIterator> itr = doc->GetNodes("meshfact");
    while(itr->HasNext())
    {
      ParseAvailableMeshfacts(itr->Next());
    }
  }

  void csLoaderContext::ParseAvailableSubmeshes(iDocumentNode* doc)
  {
    csRef<iDocumentNodeIterator> itr = doc->GetNodes("submesh");
    while(itr->HasNext())
    {
      csRef<iDocumentNode> submesh = itr->Next();
      if(submesh->GetAttributeValue("name"))
      {
        CS::Threading::MutexScopedLock lock(submeshObjects);
        availSubmeshes.Push(submesh->GetAttributeValue("name"));
      }
    }
  }

  void csLoaderContext::ParseAvailableMeshes(iDocumentNode* doc, const char* prefix)
  {
    csRef<iDocumentNodeIterator> itr = doc->GetNodes("meshobj");
    while(itr->HasNext())
    {
      csRef<iDocumentNode> meshobj = itr->Next();
      const char* name = meshobj->GetAttributeValue("name");
      csString realName = name;
      if(name)
      {
        CS::Threading::MutexScopedLock lock(meshObjects);
        if(prefix)
        {
          realName.Format("%s:%s", prefix, name);
        }
        availMeshes.Push(realName);
      }
      ParseAvailableMeshes(meshobj, realName);
    }

    itr = doc->GetNodes("meshref");
    while(itr->HasNext())
    {
      csRef<iDocumentNode> meshobj = itr->Next();
      const char* name = meshobj->GetAttributeValue("name");
      if(name)
      {
        CS::Threading::MutexScopedLock lock(meshObjects);
        availMeshes.Push(name);
      }
    }
  }

  void csLoaderContext::ParseAvailableLights(iDocumentNode* doc)
  {
    csRef<iDocumentNodeIterator> itr = doc->GetNodes("light");
    while(itr->HasNext())
    {
      csRef<iDocumentNode> light = itr->Next();
      if(light->GetAttributeValue("name"))
      {
        CS::Threading::MutexScopedLock lock(lightObjects);
        availLights.Push(light->GetAttributeValue("name"));
      }
    }
  }
}
CS_PLUGIN_NAMESPACE_END(csparser)
