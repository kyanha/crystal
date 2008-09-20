/*
    Copyright (C) 2000-2001 by Jorrit Tyberghein
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
#include "csutil/sysfunc.h"
#include "csqint.h"
#include "csqsqrt.h"
#include "iutil/stringarray.h"
#include "ivideo/graph3d.h"
#include "csutil/xmltiny.h"
#include "csutil/cfgfile.h"
#include "csutil/scanstr.h"
#include "csutil/scfstr.h"
#include "cstool/keyval.h"
#include "cstool/mapnode.h"
#include "cstool/saverfile.h"
#include "cstool/saverref.h"
#include "cstool/unusedresourcehelper.h"
#include "cstool/vfsdirchange.h"
#include "csloader.h"

#include "iutil/databuff.h"
#include "iutil/document.h"
#include "imap/reader.h"
#include "imap/saverfile.h"
#include "imap/ldrctxt.h"
#include "imap/modelload.h"
#include "imesh/lighting.h"
#include "imesh/object.h"
#include "iengine/collection.h"
#include "iengine/engine.h"
#include "iengine/texture.h"
#include "iengine/material.h"
#include "iengine/sector.h"
#include "iengine/movable.h"
#include "iengine/halo.h"
#include "iengine/campos.h"
#include "iengine/light.h"
#include "iengine/mesh.h"
#include "iengine/lod.h"
#include "iengine/camera.h"
#include "iengine/imposter.h"
#include "iengine/sharevar.h"
#include "iengine/viscull.h"
#include "iengine/portal.h"
#include "iengine/portalcontainer.h"
#include "iengine/scenenode.h"
#include "iengine/meshgen.h"
#include "ivideo/material.h"
#include "ivideo/texture.h"
#include "igraphic/imageio.h"
#include "isndsys/ss_loader.h"
#include "isndsys/ss_manager.h"
#include "isndsys/ss_renderer.h"
#include "isndsys/ss_stream.h"
#include "iutil/vfs.h"
#include "iutil/plugin.h"
#include "iutil/comp.h"
#include "iutil/objreg.h"
#include "iutil/verbositymanager.h"
#include "imesh/nullmesh.h"
#include "ivaria/reporter.h"
#include "csgeom/poly3d.h"
#include "csgeom/trimesh.h"
#include "imesh/objmodel.h"
#include "ivaria/terraform.h"

#include "loadtex.h"

#include "ivideo/shader/shader.h"
#include "iengine/renderloop.h"

#include <ctype.h>

CS_IMPLEMENT_PLUGIN

CS_PLUGIN_NAMESPACE_BEGIN(csparser)
{

//---------------------------------------------------------------------------

StdLoaderContext::StdLoaderContext (iEngine* Engine,
	iCollection* collection, bool searchCollectionOnly, csLoader* loader,
    bool checkDupes, iMissingLoaderData* missingdata, uint keepFlags) : scfImplementationType (this)
{
  StdLoaderContext::Engine = Engine;
  StdLoaderContext::collection = collection;
  StdLoaderContext::searchCollectionOnly = searchCollectionOnly;
  StdLoaderContext::loader = loader;
  StdLoaderContext::checkDupes = checkDupes;
  StdLoaderContext::missingdata = missingdata;
  StdLoaderContext::keepFlags = keepFlags;
}

StdLoaderContext::~StdLoaderContext ()
{
}

iSector* StdLoaderContext::FindSector (const char* name)
{
  iSector* s = Engine->FindSector(name, searchCollectionOnly ? collection : 0);

  if (!s && missingdata)
    s = missingdata->MissingSector (name);
  return s;
}

iMaterialWrapper* StdLoaderContext::FindMaterial (const char* filename)
{
  iMaterialWrapper* mat = Engine->FindMaterial(filename, searchCollectionOnly ? collection : 0);

  if (mat)
    return mat;
  if (missingdata)
  {
    mat = missingdata->MissingMaterial (0, filename);
    if (mat)
      return mat;
  }

  if (csLoader::do_verbose)
    loader->ReportNotify ("Could not find material '%s'. "
      "Creating new material using texture with that name", filename);
  iTextureWrapper* tex = FindTexture (filename);
  if (tex)
  {
    // Add a default material with the same name as the texture
    csRef<iMaterial> material = Engine->CreateBaseMaterial (tex);
    // First we have to extract the optional region name from the name:
    char const* n = strchr (filename, '/');
    if (!n) n = filename;
    else n++;
    iMaterialWrapper *mat = Engine->GetMaterialList ()
      	->NewMaterial (material, n);

    if(collection)
    {
      collection->Add(mat->QueryObject());
    }

    iTextureManager *tm;
    if ((loader->G3D) && (tm = loader->G3D->GetTextureManager()))
    {
      tex->Register (tm);
    }
    return mat;
  }

  return 0;
}

iMaterialWrapper* StdLoaderContext::FindNamedMaterial (const char* name, 
                                                       const char *filename)
{
  iMaterialWrapper* mat = Engine->FindMaterial (name, searchCollectionOnly ? collection : 0);

  if (mat)
    return mat;
  if (missingdata)
  {
    mat = missingdata->MissingMaterial (name, filename);
    if (mat)
      return mat;
  }

  if (csLoader::do_verbose)
    loader->ReportNotify ("Could not find material '%s'. "
      "Creating new material using texture with that name", name);
  iTextureWrapper* tex = FindNamedTexture (name,filename);
  if (tex)
  {
    // Add a default material with the same name as the texture
    csRef<iMaterial> material = Engine->CreateBaseMaterial (tex);
    // First we have to extract the optional region name from the name:
    char const* n = strchr (name, '/');
    if (!n) n = name;
    else n++;
    iMaterialWrapper *mat = Engine->GetMaterialList ()
      	->NewMaterial (material, n);
    if(collection)
    {
        collection->Add (mat->QueryObject ());
    }

    iTextureManager *tm;
    if ((loader->G3D) && (tm = loader->G3D->GetTextureManager()))
    {
      tex->Register (tm);
    }
    return mat;
  }

  return 0;
}


iMeshFactoryWrapper* StdLoaderContext::FindMeshFactory (const char* name)
{
  iMeshFactoryWrapper* fact = Engine->FindMeshFactory(name, searchCollectionOnly ? collection : 0);

  if (!fact && missingdata)
    fact = missingdata->MissingFactory (name);
  return fact;
}

iMeshWrapper* StdLoaderContext::FindMeshObject (const char* name)
{
  iMeshWrapper* mesh = Engine->FindMeshObject(name, searchCollectionOnly ? collection : 0);

  if (!mesh && missingdata)
    mesh = missingdata->MissingMesh (name);
  return mesh;
}

iLight* StdLoaderContext::FindLight (const char *name)
{
  csRef<iLightIterator> li = Engine->GetLightIterator(searchCollectionOnly ? collection : 0);

  iLight *light;

  while (li->HasNext ())
  {
    light = li->Next ();
    if (!strcmp (light->QueryObject ()->GetName (), name))
      return light;
  }
  if (missingdata)
    return missingdata->MissingLight (name);
  return 0;
}

iShader* StdLoaderContext::FindShader (const char *name)
{
  csRef<iShaderManager> shaderMgr = 
  	csQueryRegistry<iShaderManager> (loader->object_reg);

  if (!shaderMgr)
    return 0;

  if ((!searchCollectionOnly || !collection)
    || (name && *name == '*')) // Always look up builtin shaders globally
  {
    iShader* shader = shaderMgr->GetShader (name);
    if (!shader && missingdata)
    {
      shader = missingdata->MissingShader (name);
    }
    return shader;
  }

  csRefArray<iShader> shaders = shaderMgr->GetShaders ();
  size_t i;
  for (i = 0 ; i < shaders.GetSize () ; i++)
  {
    iShader* s = shaders[i];
    if(collection)
    {
      if((collection->IsParentOf(s->QueryObject()) ||
        collection->FindShader(s->QueryObject()->GetName())) &&
        !strcmp (name, s->QueryObject ()->GetName ()))
      {
        return s;
      }
    }
  }
  if (missingdata)
    return missingdata->MissingShader (name);

  return 0;
}

iTextureWrapper* StdLoaderContext::FindTexture (const char* name)
{
  iTextureWrapper* result;
  if(collection && searchCollectionOnly)
    result = collection->FindTexture(name);
  else
    result = Engine->GetTextureList ()->FindByName (name);
  if (!result && missingdata)
    result = missingdata->MissingTexture (name, 0);

  if (!result)
  {
    if (csLoader::do_verbose)
      loader->ReportNotify ("Could not find texture '%s'. Attempting to load.", 
      name);
      csRef<iTextureWrapper> rc = loader->LoadTexture(name, name,
        CS_TEXTURE_3D, 0, true, false, true, collection);
      result = rc;
  }
  return result;
}

iTextureWrapper* StdLoaderContext::FindNamedTexture (const char* name,
                                                     const char *filename)
{
  iTextureWrapper* result;
  if(collection && searchCollectionOnly)
    result = collection->FindTexture(name);
  else
    result = Engine->GetTextureList ()->FindByName (name);
  if (!result && missingdata)
    result = missingdata->MissingTexture (name, filename);

  if (!result)
  {
    if (csLoader::do_verbose)
      loader->ReportNotify ("Could not find texture '%s'. Attempting to load.", 
      name);
    csRef<iTextureWrapper> rc = loader->LoadTexture(name, filename,
        CS_TEXTURE_3D, 0, false, false, true, collection);
    result = rc;
  }
  return result;
}

void StdLoaderContext::AddToCollection(iObject *obj)
{
  if(collection)
  {
    collection->Add(obj);
  } 
}

//---------------------------------------------------------------------------

bool csLoader::do_verbose = false;

void csLoader::ReportError (const char* id, const char* description, ...)
{
  va_list arg;
  va_start (arg, description);
  csReportV (object_reg, CS_REPORTER_SEVERITY_ERROR, id, description, arg);
  va_end (arg);
}

void csLoader::ReportNotify (const char* description, ...)
{
  va_list arg;
  va_start (arg, description);
  ReportNotifyV ("crystalspace.maploader", description, arg);
  va_end (arg);
}

void csLoader::ReportNotifyV (const char* id, const char* description,
	va_list arg)
{
  csReportV (object_reg, CS_REPORTER_SEVERITY_NOTIFY, id, description, arg);
}

void csLoader::ReportNotify2 (const char* id, const char* description, ...)
{
  va_list arg;
  va_start (arg, description);
  ReportNotifyV (id, description, arg);
  va_end (arg);
}

void csLoader::ReportWarning (const char* id, const char* description, ...)
{
  va_list arg;
  va_start (arg, description);
  csReportV (object_reg, CS_REPORTER_SEVERITY_WARNING, id, description, arg);
  va_end (arg);
}

void csLoader::ReportWarning (const char* id, iDocumentNode* node,
	const char* description, ...)
{
  va_list arg;
  va_start (arg, description);
  csString buf;
  buf.FormatV (description, arg);
  va_end (arg);
  SyntaxService->Report (id, CS_REPORTER_SEVERITY_WARNING, node, "%s", 
    buf.GetData());
}
//---------------------------------------------------------------------------

bool csLoader::LoadStructuredDoc (const char* file, iDataBuffer* buf,
	csRef<iDocument>& doc)
{
  csRef<iDocumentSystem> docsys (
      csQueryRegistry<iDocumentSystem> (object_reg));
  if (!docsys) docsys = csPtr<iDocumentSystem> (new csTinyDocumentSystem ());
  doc = docsys->CreateDocument ();
  const char* error = doc->Parse (buf, true);
  if (error != 0)
  {
    ReportError (
	    "crystalspace.maploader.parse.plugin",
	    file
	      ? "Document system error for file '%s': %s!"
	      : "Document system error for buffer%s: %s!",
	    file ? file : "", error);
    doc = 0;
    return false;
  }
  return true;
}

bool csLoader::LoadStructuredDoc (const char* file, iFile* buf,
	csRef<iDocument>& doc)
{
  csRef<iDocumentSystem> docsys (
      csQueryRegistry<iDocumentSystem> (object_reg));
  if (!docsys) docsys = csPtr<iDocumentSystem> (new csTinyDocumentSystem ());
  doc = docsys->CreateDocument ();
  const char* error = doc->Parse (buf, true);
  if (error != 0)
  {
    ReportError (
	    "crystalspace.maploader.parse.plugin",
	    "Document system error for file '%s': %s!", file, error);
    doc = 0;
    return false;
  }
  return true;
}

csPtr<iBase> csLoader::LoadStructuredMap (iLoaderContext* ldr_context,
	iLoaderPlugin* plug, iFile* buf,
  	iBase* context, const char* fname, iStreamSource* ssource)
{
  csRef<iDocument> doc;
  csString filename (fname);
  csVfsDirectoryChanger dirChanger (VFS);
  size_t slashPos = filename.FindLast ('/');
  if (slashPos != (size_t)-1)
  {
    dirChanger.ChangeTo (filename);
    filename.DeleteAt (0, slashPos + 1);
  }
  bool er = LoadStructuredDoc (filename, buf, doc);
  csRef<iBase> ret;
  if (er)
  {
    if (doc)
    {
      // First find the <params> node in the loaded file.
      csRef<iDocumentNode> paramsnode = doc->GetRoot ()->GetNode ("params");
      if (!paramsnode)
      {
        SyntaxService->ReportError (
	        "crystalspace.maploader.load.plugin",
                doc->GetRoot (), "Could not find <params> in '%s'!", fname);
      }
      else
      {
        ret = plug->Parse (paramsnode, ssource, ldr_context, context);
      }
    }
    else
    {
      ReportError ("crystalspace.maploader.load.plugin",
	      "File does not appear to be a structured map file (%s)!", fname);
    }
  }
  return csPtr<iBase> (ret);
}

//---------------------------------------------------------------------------

static bool TestXML (const char* b)
{
  while (*b && isspace (*b)) b++;
  if (*b != '<') return false;
  b++;
  if (*b == '?')
  {
    return (b[1] == 'x' && b[2] == 'm' && b[3] == 'l' && isspace (b[4]));
  }
  else if (*b == '!')
  {
    return (b[1] == '-' && b[2] == '-');
  }
  else
  {
    b++;
    if (!isalpha (*b) && *b != '_')
      return false;
    b++;
    while (isalnum (*b)) b++;
    if (!isspace (*b) && *b != '>')
      return false;
    return true;
  }
}

//--------------------------- Collections -----------------------------------

csLoadResult csLoader::Load (iDataBuffer* buffer, const char* fname,
	iCollection* collection, bool searchCollectionOnly, bool checkDupes,
  iStreamSource* ssource, const char* override_name, iMissingLoaderData* missingdata,
  uint keepFlags)
{
  csLoadResult rc;
  rc.success = false;
  rc.result = 0;

  if (TestXML (buffer->GetData ()))
  {
    csRef<iDocument> doc;
    bool er = LoadStructuredDoc (fname, buffer, doc);
    if (!er) return rc;

    if (doc)
    {
      csRef<iDocumentNode> node = doc->GetRoot ();
      return Load (node, collection, searchCollectionOnly, checkDupes,
                   ssource, override_name, missingdata, keepFlags);
    }
    else
    {
      ReportError ("crystalspace.maploader.parse",
        fname
	  ? "File does not appear to be correct XML file (%s)!"
	  : "Buffer does not appear to be correct XML!", fname);
      return rc;
    }
  }
  else
  {
    csRef<iPluginManager> plugin_mgr = csQueryRegistry<iPluginManager> (
    	object_reg);
    csRef<iStringArray> model_loader_ids = iSCF::SCF->QueryClassList (
      "crystalspace.mesh.loader.factory.");
    for (size_t i = 0; i < model_loader_ids->GetSize(); i++)
    {
      const char* plugin = model_loader_ids->Get (i);
      csRef<iModelLoader> l = csQueryPluginClass<iModelLoader> (plugin_mgr,
        plugin);
      if (!l)
        l = csLoadPlugin<iModelLoader> (plugin_mgr, plugin);
      if (l && l->IsRecognized (buffer))
      {
        iMeshFactoryWrapper* ff = l->Load (
		override_name ? override_name :
		fname ? fname : "__model__", buffer);
	if (!ff) return rc;
	rc.result = ff;
        rc.success = true;
	return rc;
      }
    }
    ReportError ("crystalspace.maploader.parse",
        fname
	  ? "Model file not recognized (%s)!"
	  : "Model buffer not recognized!", fname);
  }

  return rc;
}

csLoadResult csLoader::Load (const char* fname, iCollection* collection,
  	bool searchCollectionOnly, bool checkDupes, iStreamSource* ssource,
	const char* override_name, iMissingLoaderData* missingdata, uint keepFlags)
{
  csRef<iDataBuffer> buf = VFS->ReadFile (fname);

  if (!buf)
  {
    ReportError (
	      "crystalspace.maploader.parse",
    	      "Could not open map file '%s' on VFS!", fname);
    csLoadResult rc;
    rc.success = false;
    rc.result = 0;
    return rc;
  }
  
  return Load (buf, fname, collection, searchCollectionOnly, checkDupes, ssource,
  	override_name, missingdata, keepFlags);
}

csLoadResult csLoader::Load (iDataBuffer* buffer, iCollection* collection,
  	bool searchCollectionOnly, bool checkDupes, iStreamSource* ssource,
	const char* override_name, iMissingLoaderData* missingdata, uint keepFlags)
{
  return Load (buffer, 0, collection, searchCollectionOnly, checkDupes, ssource,
  	override_name, missingdata, keepFlags);
}

csLoadResult csLoader::Load (iDocumentNode* node, iCollection* collection,
  	bool searchCollectionOnly, bool checkDupes, iStreamSource* ssource,
	const char* override_name, iMissingLoaderData* missingdata, uint keepFlags)
{
  csLoadResult rc;
  rc.success = false;
  rc.result = 0;

  csRef<iLoaderContext> ldr_context = csPtr<iLoaderContext> (
	new StdLoaderContext (Engine, collection, searchCollectionOnly, this, checkDupes,
	  missingdata, keepFlags));

  csRef<iDocumentNode> meshfactnode = node->GetNode ("meshfact");
  if (meshfactnode)
  {
    const char* meshfactname = override_name ? override_name :
    	meshfactnode->GetAttributeValue ("name");
    if (ldr_context->CheckDupes () && meshfactname)
    {
      iMeshFactoryWrapper* mfw = Engine->FindMeshFactory (meshfactname);
      if (mfw)
      {
        AddToCollection (ldr_context, mfw->QueryObject ());
        rc.result = mfw;
        rc.success = true;
        return rc;
      }
    }

    csRef<iMeshFactoryWrapper> t = Engine->CreateMeshFactory (
        meshfactname);
    if (LoadMeshObjectFactory (ldr_context, t, 0, meshfactnode, 0, ssource))
    {
      AddToCollection (ldr_context, t->QueryObject ());
      rc.result = t;
      rc.success = true;
      return rc;
    }
    else
    {
      // Error is already reported.
      Engine->GetMeshFactories ()->Remove (t);
      return rc;
    }
  }

  csRef<iDocumentNode> meshobjnode = node->GetNode ("meshobj");
  if (meshobjnode)
  {
    const char* meshobjname = override_name ? override_name :
    	meshobjnode->GetAttributeValue ("name");
    if (ldr_context->CheckDupes () && meshobjname)
    {
      iMeshWrapper* mw = Engine->FindMeshObject (meshobjname);
      if (mw)
      {
        AddToCollection (ldr_context, mw->QueryObject ());
        rc.result = mw;
        rc.success = true;
        return rc;
      }
    }
    csRef<iMeshWrapper> mw = Engine->CreateMeshWrapper (meshobjname);
    if (LoadMeshObject (ldr_context, mw, 0, meshobjnode, ssource))
    {
      AddToCollection (ldr_context, mw->QueryObject ());
      rc.result = mw;
      rc.success = true;
      return rc;
    }
    else
    {
      // Error is already reported.
      Engine->GetMeshes ()->Remove (mw);
      return rc;
    }
  }

  csRef<iDocumentNode> worldnode = node->GetNode ("world");
  if (worldnode)
  {
    rc.result = Engine;
    rc.success = LoadMap (ldr_context, worldnode, ssource, missingdata);
    if (!rc.success) rc.result = 0;
    return rc;
  }

  csRef<iDocumentNode> libnode = node->GetNode ("library");
  if (libnode)
  {
    rc.result = 0;
    rc.success = LoadLibrary (ldr_context, libnode, ssource, missingdata,	true);
    return rc;
  }

  csRef<iDocumentNode> portalsnode = node->GetNode ("portals");
  if (portalsnode)
  {
    const char* portalsname = override_name ? override_name :
    	portalsnode->GetAttributeValue ("name");
    if (ldr_context->CheckDupes () && portalsname)
    {
      iMeshWrapper* mw = Engine->FindMeshObject (portalsname);
      if (mw)
      {
        csRef<iPortalContainer> pc = 
          scfQueryInterface<iPortalContainer>(mw->GetMeshObject ());
        if (pc)
        {
          AddToCollection (ldr_context, mw->QueryObject ());
          rc.result = mw;
          rc.success = true;
          return rc;
        }
      }
    }
    if (ParsePortals (ldr_context, portalsnode, 0, 0, ssource))
    {
      iMeshWrapper* mw = 0;
      if (ldr_context->GetCollection())
        mw = ldr_context->GetCollection()->FindMeshObject(portalsname);

      if (mw)
      {
        mw->QueryObject()->SetName(portalsname);
        rc.result = mw;
        rc.success = true;
        return rc;
      }
    }

    rc.result = 0;
    rc.success = false;
    return rc;
  }

  csRef<iDocumentNode> lightnode = node->GetNode ("light");
  if (lightnode)
  {
    const char* lightname = override_name ? override_name :
    	lightnode->GetAttributeValue ("name");
    iLight* light = ParseStatlight (ldr_context, lightnode);
    if (light)
    {
      light->QueryObject()->SetName(lightname);
      rc.result = light;
      rc.success = true;
      return rc;
    }

    rc.result = 0;
    rc.success = false;
    return rc;
  }

  csRef<iDocumentNode> meshrefnode = node->GetNode ("meshref");
  if (meshrefnode)
  {
    const char* meshobjname = override_name ? override_name :
    	meshrefnode->GetAttributeValue ("name");
    if (ldr_context->CheckDupes () && meshobjname)
    {
      iMeshWrapper* mw = Engine->FindMeshObject (meshobjname);
      if (mw)
      {
        AddToCollection (ldr_context, mw->QueryObject ());
        rc.result = mw;
        rc.success = true;
        return rc;
      }
    }
    csRef<iMeshWrapper> mesh = LoadMeshObjectFromFactory (ldr_context, meshrefnode, ssource);
    if (mesh)
    {
      AddToCollection (ldr_context, mesh->QueryObject ());
      rc.result = mesh;
      rc.success = true;
      return rc;
    }
    else
    {
      // Error is already reported.
      rc.result = 0;
      rc.success = false;
      return rc;
    }
  }

  ReportError ("crystalspace.maploader.parse",
    "File doesn't seem to be a world, library, meshfact, meshobj, meshref, portals or light file!");

  return rc;
}

bool csLoader::LoadMapFile (const char* file, bool clearEngine,
  iCollection* collection, bool searchCollectionOnly, bool checkdupes,
  iStreamSource* ssource, iMissingLoaderData* missingdata, uint keepFlags)
{
  csRef<iFile> buf = VFS->Open (file, VFS_FILE_READ);

  if (!buf)
  {
    ReportError (
	      "crystalspace.maploader.parse.map",
    	      "Could not open map file '%s' on VFS!", file);
    return false;
  }

  csRef<iDocument> doc;
  bool er = LoadStructuredDoc (file, buf, doc);
  if (!er) return false;

  if (doc)
  {
    csRef<iDocumentNode> world_node = doc->GetRoot ()->GetNode ("world");
    if (!world_node)
    {
      SyntaxService->ReportError (
        "crystalspace.maploader.parse.expectedworld",
        world_node, "Expected 'world' token!");
      return false;
    }
    
    if (Engine->GetSaveableFlag () && collection)
    {
      csRef<iSaverFile> saverFile;
      saverFile.AttachNew (new csSaverFile (file, CS_SAVER_FILE_WORLD));
      collection->Add(saverFile->QueryObject());
    }
    
    return LoadMap (world_node, clearEngine, collection, searchCollectionOnly, checkdupes,
    	            ssource, missingdata, keepFlags);
  }
  else
  {
    ReportError ("crystalspace.maploader.parse.plugin", 
      "File does not appear to be a structured map file (%s)!", file);
    return false;
  }

  return true;
}

bool csLoader::LoadMap (iDocumentNode* world_node, bool clearEngine,
  iCollection* collection, bool searchCollectionOnly, bool checkdupes,
  iStreamSource* ssource, iMissingLoaderData* missingdata, uint keepFlags)
{
  if (clearEngine)
  {
    Engine->DeleteAll ();
    Engine->ResetWorldSpecificSettings();
  }
  csRef<iLoaderContext> ldr_context = csPtr<iLoaderContext> (
	new StdLoaderContext (Engine, collection, searchCollectionOnly, this, checkdupes,
	  missingdata, keepFlags));

  return LoadMap (ldr_context, world_node, ssource, missingdata);
}

//---------------------------------------------------------------------------

bool csLoader::LoadLibraryFile (const char* fname, iCollection* collection,
	bool searchCollectionOnly, bool checkDupes,iStreamSource* ssource, 
    iMissingLoaderData* missingdata, uint keepFlags)
{
    return LoadMapLibraryFile (fname, collection, searchCollectionOnly,
                               checkDupes, ssource, missingdata,
			                         keepFlags);
}

bool csLoader::LoadMapLibraryFile (const char* fname, iCollection* collection,
	bool searchCollectionOnly, bool checkDupes, iStreamSource* ssource, iMissingLoaderData* missingdata,
  uint keepFlags, bool loadProxyTex)
{
  csRef<iFile> buf = VFS->Open (fname, VFS_FILE_READ);

  if (!buf)
  {
    ReportError (
	      "crystalspace.maploader.parse.library",
    	      "Could not open library file '%s' on VFS!", fname);
    return false;
  }
 
  if(Engine->GetSaveableFlag () && collection)
  {
    csRef<iSaverFile> saverFile;
    saverFile.AttachNew (new csSaverFile (fname, CS_SAVER_FILE_LIBRARY));
    collection->Add(saverFile->QueryObject());
  }

  csRef<iLoaderContext> ldr_context = csPtr<iLoaderContext> (
	new StdLoaderContext (Engine, collection, searchCollectionOnly, this, checkDupes,
	  missingdata, keepFlags));

  csRef<iDocument> doc;
  bool er = LoadStructuredDoc (fname, buf, doc);
  if (!er) return false;
  if (doc)
  {
    csRef<iDocumentNode> lib_node = doc->GetRoot ()->GetNode ("library");
    if (!lib_node)
    {
      SyntaxService->ReportError (
        "crystalspace.maploader.parse.expectedlib",
        lib_node, "Expected 'library' token!");
      return false;
    }
    return LoadLibrary (ldr_context, lib_node, ssource, missingdata,
    	loadProxyTex);
  }
  else
  {
    ReportError ("crystalspace.maploader.parse.plugin",
      "File does not appear to be a structure map library (%s)!", fname);
  }
  return false;
}

bool csLoader::LoadLibrary (iDocumentNode* lib_node, iCollection* collection,
    bool searchCollectionOnly, bool checkDupes, iStreamSource* ssource,
	iMissingLoaderData* missingdata, uint keepFlags)
{
  csRef<iLoaderContext> ldr_context = csPtr<iLoaderContext> (
	new StdLoaderContext (Engine, collection, searchCollectionOnly, this, checkDupes,
	  missingdata, keepFlags));

  return LoadLibrary (ldr_context, lib_node, ssource, missingdata, true);
}

//---------------------------------------------------------------------------

void csLoader::AddToCollection(iLoaderContext* ldr_context, iObject* obj)
{
  ldr_context->AddToCollection(obj);
}

void csLoader::AddChildrenToCollection (iLoaderContext* ldr_context,
                                    const iSceneNodeArray* children)
{
  size_t i;
  for (i = 0 ; i < children->GetSize(); i++)
  {
    iSceneNode* sn = children->Get(i);
    iObject* obj = 0;
    if (sn->QueryMesh ()) obj = sn->QueryMesh ()->QueryObject ();
    else if (sn->QueryLight ()) obj = sn->QueryLight ()->QueryObject ();
    //else if (sn->QueryCamera ()) obj = sn->QueryCamera ()->QueryObject ();
    if (obj)
      AddToCollection (ldr_context, obj);
    const csRef<iSceneNodeArray> nodeChildren = sn->GetChildrenArray ();
    AddChildrenToCollection (ldr_context, nodeChildren);
  }
}

//---------------------------------------------------------------------------

csPtr<iMeshFactoryWrapper> csLoader::LoadMeshObjectFactory (const char* fname,
	iStreamSource* ssource)
{
  if (!Engine) return 0;

  csRef<iLoaderContext> ldr_context = csPtr<iLoaderContext> (
	new StdLoaderContext (Engine, 0, true, this, false, 0, KEEP_ALL));

  csRef<iFile> databuff (VFS->Open (fname, VFS_FILE_READ));

  if (!databuff || !databuff->GetSize ())
  {
    ReportError (
	      "crystalspace.maploader.parse.meshfactory",
    	      "Could not open mesh object file '%s' on VFS!", fname);
    return 0;
  }

  csRef<iDocument> doc;
  bool er = LoadStructuredDoc (fname, databuff, doc);
  if (!er) return 0;
  if (doc)
  {
    csRef<iDocumentNode> meshfactnode = doc->GetRoot ()->GetNode ("meshfact");
    if (!meshfactnode)
    {
      ReportError (
	      "crystalspace.maploader.parse.map",
    	      "File '%s' does not seem to contain a 'meshfact'!", fname);
      return 0;
    }
    csRef<iMeshFactoryWrapper> t = Engine->CreateMeshFactory (
      	meshfactnode->GetAttributeValue ("name"));
    if (LoadMeshObjectFactory (ldr_context, t, 0, meshfactnode, 0, ssource))
    {
      AddToCollection (ldr_context, t->QueryObject ());
      return csPtr<iMeshFactoryWrapper> (t);
    }
    else
    {
      // Error is already reported.
      Engine->GetMeshFactories ()->Remove (t);
      return 0;
    }
  }
  else
  {
    ReportError ("crystalspace.maploader.parse.plugin",
      "File does not appear to be a structured mesh factory (%s)!", fname);
    return 0;
  }
  return 0;
}

//---------------------------------------------------------------------------

csPtr<iMeshWrapper> csLoader::LoadMeshObject (const char* fname,
	iStreamSource* ssource)
{
  if (!Engine) return 0;

  csRef<iFile> databuff (VFS->Open (fname, VFS_FILE_READ));
  csRef<iMeshWrapper> mesh;
  csRef<iLoaderContext> ldr_context = csPtr<iLoaderContext> (
	new StdLoaderContext (Engine, 0, true, this, false, 0, KEEP_ALL));

  if (!databuff || !databuff->GetSize ())
  {
    ReportError (
	      "crystalspace.maploader.parse.meshobject",
    	      "Could not open mesh object file '%s' on VFS!", fname);
    return 0;
  }

  csRef<iDocument> doc;
  bool er = LoadStructuredDoc (fname, databuff, doc);
  if (!er) return 0;
  if (doc)
  {
    csRef<iDocumentNode> meshobjnode = doc->GetRoot ()->GetNode ("meshobj");
    if (!meshobjnode)
    {
      ReportError (
	      "crystalspace.maploader.parse.map",
    	      "File '%s' does not seem to contain a 'meshobj'!", fname);
      return 0;
    }
    mesh = Engine->CreateMeshWrapper (
    	meshobjnode->GetAttributeValue ("name"));
    if (LoadMeshObject (ldr_context, mesh, 0, meshobjnode, ssource))
    {
      AddToCollection (ldr_context, mesh->QueryObject ());
    }
    else
    {
      // Error is already reported.
      Engine->GetMeshes ()->Remove (mesh);
      mesh = 0;
    }
  }
  else
  {
    ReportError ("crystalspace.maploader.parse.plugin",
      "File does not appear to be a structured mesh object (%s)!", fname);
    return 0;
  }
  return csPtr<iMeshWrapper> (mesh);
}

/************ iLoader implementation **************/

//--- Plugin stuff -----------------------------------------------------------

SCF_IMPLEMENT_FACTORY(csLoader)

csLoader::csLoader (iBase *p) : scfImplementationType (this, p)
{
  object_reg = 0;
  do_verbose = false;
  autoRegions = false;
}

csLoader::~csLoader()
{
  loaded_plugins.DeleteAll ();
}

#define GET_PLUGIN(var, intf, msgname)				\
  var = csQueryRegistry<intf> (object_reg);			\
  if (!var && do_verbose) ReportNotify ("Could not get " msgname);

#define GET_CRITICAL_PLUGIN(var, intf, msgname)			\
  var = csQueryRegistry<intf> (object_reg);			\
  if (!var) { ReportError ("crystalspace.maploader",		\
    "Failed to initialize loader: "				\
    "Could not get " msgname); return false; }

bool csLoader::Initialize (iObjectRegistry *object_Reg)
{
  csLoader::object_reg = object_Reg;
  loaded_plugins.SetObjectRegistry (object_reg);
  csRef<iVerbosityManager> verbosemgr (
    csQueryRegistry<iVerbosityManager> (object_reg));
  if (verbosemgr) 
    do_verbose = verbosemgr->Enabled ("loader");
  else
    do_verbose = false;

  csRef<iPluginManager> plugin_mgr (
  	csQueryRegistry<iPluginManager> (object_reg));

  Reporter = csQueryRegistry<iReporter> (object_reg);

  loaded_plugins.plugin_mgr = plugin_mgr;

  // Get the virtual file system plugin.
  GET_CRITICAL_PLUGIN (VFS, iVFS, "VFS");
  // Get syntax services.
  SyntaxService = csQueryRegistryOrLoad<iSyntaxService> (object_reg,
  	"crystalspace.syntax.loader.service.text");
  if (!SyntaxService) return false;

  // Get all optional plugins.
  GET_PLUGIN (ImageLoader, iImageIO, "image loader");
  GET_PLUGIN (SndSysLoader, iSndSysLoader, "sound loader (v2)");
  GET_PLUGIN (Engine, iEngine, "engine");
  GET_PLUGIN (G3D, iGraphics3D, "video driver");
  GET_PLUGIN (SndSysRender, iSndSysRenderer, "sound driver (v2)");

  SndSysManager = csQueryRegistryOrLoad<iSndSysManager> (object_reg,
    "crystalspace.sndsys.manager", false);
  if (!SndSysManager)
  {
    if (do_verbose) ReportNotify ("Could not get sound manager!");
  }

  InitTokenTable (xmltokens);

  stringSet = csQueryRegistryTagInterface<iStringSet> (
    object_reg, "crystalspace.shared.stringset");
  stringSetSvName = csQueryRegistryTagInterface<iShaderVarStringSet> (
    object_reg, "crystalspace.shader.variablenameset");

  return true;
}

//--- Parsing of Engine Objects ---------------------------------------------

bool csLoader::LoadMap (iLoaderContext* ldr_context, iDocumentNode* worldnode,
                        iStreamSource* ssource, iMissingLoaderData* missingdata)
{
  if (!Engine)
  {
    SyntaxService->ReportError (
		"crystalspace.maploader.parse",
		worldnode, "The engine plugin is missing!");
    return false;
  }

  // Will be set to true if we find a <shader> section.
  bool shader_given = false;

  csRef<iDocumentNode> sequences;
  csRef<iDocumentNode> triggers;

  csRef<iDocumentNodeIterator> it = worldnode->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_SHADEREXPRESSIONS:
	if (!LoadShaderExpressions (ldr_context, child))
	  return false;
        break;
      case XMLTOKEN_SETTINGS:
	if (!LoadSettings (child))
	  return false;
	break;
      case XMLTOKEN_RENDERPRIORITIES:
	ReportWarning (
		"crystalspace.maploader.parse.region",
		worldnode, "<renderpriorities> is no longer supported!");
	break;
      case XMLTOKEN_ADDON:
	if (!LoadAddOn (ldr_context, child, (iEngine*)Engine, false, ssource))
	  return false;
      	break;
      case XMLTOKEN_META:
	if (!LoadAddOn (ldr_context, child, (iEngine*)Engine, true, ssource))
	  return false;
      	break;
      case XMLTOKEN_MESHFACT:
        {
	  csRef<iDocumentAttribute> attr_name = child->GetAttribute ("name");
	  const char* name = attr_name ? attr_name->GetValue () : 0;
	  csRef<iDocumentAttribute> attr_file = child->GetAttribute ("file");
	  if (attr_file)
	  {
          const char* filename = attr_file->GetValue ();
          csLoadResult rc = Load (filename, ldr_context->GetCollection (),
              ldr_context->CurrentCollectionOnly (),
              ldr_context->CheckDupes (),
              ssource, name, missingdata,
              ldr_context->GetKeepFlags());
          if (!rc.success)
	    {
              SyntaxService->ReportError (
	        "crystalspace.maploader.parse.loadingmodel",
	        child, "Error opening file model '%s'!", filename);
	      return false;
	    }
	    break;
	  }

	  if (ldr_context->CheckDupes () && name)
	  {
	    iMeshFactoryWrapper* mfw = Engine->FindMeshFactory (name);
	    if (mfw)
	    {
	      AddToCollection (ldr_context, mfw->QueryObject ());
	      break;
	    }
	  }
          csRef<iMeshFactoryWrapper> t = Engine->CreateMeshFactory (name);
	  if (!t || !LoadMeshObjectFactory (ldr_context, t, 0, child, 0,
	  	ssource))
	  {
	    // Error is already reported.
	    return false;
	  }
	  else
	  {
	    AddToCollection (ldr_context, t->QueryObject ());
	  }
        }
	break;
      case XMLTOKEN_REGION:
	SyntaxService->ReportError (
		"crystalspace.maploader.parse.region",
		worldnode, "<region> is no longer supported!");
	break;
      case XMLTOKEN_SECTOR:
        if (!ParseSector (ldr_context, child, ssource))
	  return false;
        break;
      case XMLTOKEN_SEQUENCES:
	// Defer sequence parsing to later.
	sequences = child;
	break;
      case XMLTOKEN_TRIGGERS:
	// Defer trigger parsing to later.
	triggers = child;
	break;
      case XMLTOKEN_PLUGINS:
	if (!LoadPlugins (child))
	  return false;
	break;
      case XMLTOKEN_TEXTURES:
        if (!ParseTextureList (ldr_context, child))
          return false;
        break;
      case XMLTOKEN_MATERIALS:
        if (!ParseMaterialList (ldr_context, child))
          return false;
        break;
      case  XMLTOKEN_VARIABLES:
	if (!ParseVariableList (ldr_context, child))
	  return false;
	break;
      case XMLTOKEN_SOUNDS:
        if (!LoadSounds (child))
          return false;
        break;
      case XMLTOKEN_LIBRARY:
      {
	if (!LoadLibraryFromNode (ldr_context, child, ssource, missingdata,
		false))
	  return false;
	break;
      }
      case XMLTOKEN_START:
      {
	const char* name = child->GetAttributeValue ("name");
	iCameraPosition* campos = Engine->GetCameraPositions ()->
	  	NewCameraPosition (name ? name : "Start");
	AddToCollection (ldr_context, campos->QueryObject ());
	if (!ParseStart (child, campos))
	  return false;
        break;
      }
      case XMLTOKEN_KEY:
      {
        if (!ParseKey (child, Engine->QueryObject()))
          return false;
        break;
      }
      case XMLTOKEN_SHADERS:
	shader_given = true;
        ParseShaderList (ldr_context, child);
        break;
      default:
	SyntaxService->ReportBadToken (child);
	return false;
    }
  }

  // Sequences and triggers are parsed at the end because
  // all sectors and other objects need to be present.
  if (sequences)
    if (!LoadSequences (ldr_context, sequences))
      return false;
  if (triggers)
    if (!LoadTriggers (ldr_context, triggers))
      return false;

  // Go through the list of proxy textures and load those needed.
  if(ldr_context->GetKeepFlags() == KEEP_USED)
  {
    if(!LoadProxyTextures())
      return false;
  }

  return true;
}

bool csLoader::LoadLibraryFromNode (iLoaderContext* ldr_context,
	iDocumentNode* child, iStreamSource* ssource,
	iMissingLoaderData* missingdata, bool loadProxyTex)
{
  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);
  const char* name = child->GetAttributeValue ("checkdupes");
  bool dupes = ldr_context->CheckDupes ();
  if (name)
  {
    if (!strcasecmp (name, "true") || !strcasecmp (name, "yes") ||
	    	!strcasecmp (name, "1") || !strcasecmp (name, "on"))
      dupes = true;
    else
      dupes = false;
  }
  
  const char* file = child->GetAttributeValue ("file");
  if (file)
  {
    const char* path = child->GetAttributeValue ("path");
    if (path)
    {
      vfs->PushDir ();
      vfs->ChDir (path);
    }
    
    if (Engine->GetSaveableFlag ())
    {
      csRef<iLibraryReference> libraryRef;
      libraryRef.AttachNew (new csLibraryReference (file, path, dupes));
      AddToCollection (ldr_context, libraryRef->QueryObject ());
    }
    
    bool rc;

    rc = LoadMapLibraryFile (file, ldr_context->GetCollection (),
        ldr_context->CurrentCollectionOnly (), dupes, ssource, missingdata,
        ldr_context->GetKeepFlags(), loadProxyTex);

    if (path)
    {
      vfs->PopDir ();
    }
    if (!rc)
      return false;
  }
  else
  {
    if (Engine->GetSaveableFlag ())
    {
      csRef<iLibraryReference> libraryRef;
      libraryRef.AttachNew (new csLibraryReference (
          child->GetContentsValue (), 0, dupes));
      AddToCollection (ldr_context, libraryRef->QueryObject ());
    }
    
    return LoadMapLibraryFile (child->GetContentsValue (), ldr_context->GetCollection (),
        ldr_context->CurrentCollectionOnly (), ldr_context->CheckDupes (), ssource, missingdata, 
        ldr_context->GetKeepFlags(), loadProxyTex);
  }
  return true;
}

bool csLoader::LoadLibrary (iLoaderContext* ldr_context, iDocumentNode* libnode,
	iStreamSource* ssource, iMissingLoaderData* missingdata, bool loadProxyTex)
{
  if (!Engine)
  {
    SyntaxService->ReportError (
	  "crystalspace.maploader.parse.noengine",
	  libnode, "No engine present while in LoadLibrary!");
    return false;
  }
  
  csRef<iDocumentNode> sequences;
  csRef<iDocumentNode> triggers;

  csRef<iDocumentNodeIterator> it = libnode->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_SHADEREXPRESSIONS:
	if (!LoadShaderExpressions (ldr_context, child))
	  return false;
        break;
      case XMLTOKEN_LIBRARY:
      {
	if (!LoadLibraryFromNode (ldr_context, child, ssource, missingdata))
	  return false;
	break;
      }
      case XMLTOKEN_ADDON:
	if (!LoadAddOn (ldr_context, child, (iEngine*)Engine, false, ssource))
	  return false;
      	break;
      case XMLTOKEN_META:
	if (!LoadAddOn (ldr_context, child, (iEngine*)Engine, true, ssource))
	  return false;
      	break;
      case XMLTOKEN_SEQUENCES:
	// Defer sequence parsing to later.
	sequences = child;
	break;
      case XMLTOKEN_TRIGGERS:
	// Defer trigger parsing to later.
	triggers = child;
	break;
      case XMLTOKEN_TEXTURES:
        // Append textures to engine.
        if (!ParseTextureList (ldr_context, child))
          return false;
        break;
      case XMLTOKEN_MATERIALS:
        if (!ParseMaterialList (ldr_context, child))
          return false;
        break;
      case XMLTOKEN_SHADERS:
        if (!ParseShaderList (ldr_context, child))
	  return false;
        break;
      case  XMLTOKEN_VARIABLES:
	if (!ParseVariableList (ldr_context, child))
	  return false;
	break;
      case XMLTOKEN_SOUNDS:
        if (!LoadSounds (child))
          return false;
        break;
      case XMLTOKEN_MESHREF:
        {
          csRef<iMeshWrapper> mesh = LoadMeshObjectFromFactory (ldr_context,
	  	child, ssource);
          if (!mesh)
	  {
	    // Error is already reported.
	    return false;
	  }
	  mesh->QueryObject ()->SetName (child->GetAttributeValue ("name"));
	  Engine->AddMeshAndChildren (mesh);
	  //mesh->DecRef ();
        }
        break;
      case XMLTOKEN_MESHOBJ:
        {
	  csRef<iMeshWrapper> mesh = Engine->CreateMeshWrapper (
			    child->GetAttributeValue ("name"));
          if (!LoadMeshObject (ldr_context, mesh, 0, child, ssource))
	  {
	    // Error is already reported.
	    return false;
	  }
	  else
	  {
	    AddToCollection (ldr_context, mesh->QueryObject ());
	  }
        }
        break;
      case XMLTOKEN_MESHFACT:
        {
          const char* meshfactname = child->GetAttributeValue ("name");
          if(ldr_context->CheckDupes ())
          {
            iMeshFactoryWrapper* mfw = Engine->FindMeshFactory (meshfactname);
            if(mfw)
            {
              AddToCollection (ldr_context, mfw->QueryObject ());
              break;
            }
          }

          csRef<iMeshFactoryWrapper> t = Engine->CreateMeshFactory (meshfactname);
          if (t)
          {
            if (!LoadMeshObjectFactory (ldr_context, t, 0, child, 0, ssource))
            {
              // Error is already reported.
              return false;
            }
            else
            {
              AddToCollection (ldr_context, t->QueryObject ());
            }
          }
        }
        break;
      case XMLTOKEN_PLUGINS:
	if (!LoadPlugins (child))
	  return false;
        break;
      default:
	SyntaxService->ReportBadToken (child);
        return false;
    }
  }

  // Sequences and triggers are parsed at the end because
  // all sectors and other objects need to be present.
  if (sequences)
    if (!LoadSequences (ldr_context, sequences))
      return false;
  if (triggers)
    if (!LoadTriggers (ldr_context, triggers))
      return false;

  if(ldr_context->GetKeepFlags() == KEEP_USED && loadProxyTex)
  {
      if(!LoadProxyTextures())
          return false;
  }

  return true;
}

bool csLoader::LoadPlugins (iDocumentNode* node)
{
  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_PLUGIN:
        {
          const char* plugin_name = child->GetAttributeValue ("name");
          loaded_plugins.NewPlugin (plugin_name, child);
          if (Engine->GetSaveableFlag ())
          {
            const char* plugin_id = loaded_plugins.FindPluginClassID (
	    	plugin_name);
            if (plugin_id)
            {
              csRef<iPluginReference> pluginref;
              pluginref.AttachNew (new csPluginReference (plugin_name,
	      	plugin_id));
              object_reg->Register (pluginref,
	      	csString ("_plugref_") + plugin_id);
            }
          }
        }
        break;
      
      default:
	SyntaxService->ReportBadToken (child);
	return false;
    }
  }

  return true;
}

bool csLoader::LoadSounds (iDocumentNode* node)
{
  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_SOUND:
        {
          const char* name = child->GetAttributeValue ("name");
          const char* filename = name;
          csRef<iDocumentNode> filenode = child->GetNode ("file");
          if (filenode)
          {
            filename = filenode->GetContentsValue ();
          }
	  else
	  {
	    const char* fname = child->GetAttributeValue ("file");
	    if (fname) filename = fname;
	  }
	  int mode3d = -1;
	  csRef<iDocumentAttribute> at = child->GetAttribute ("mode3d");
	  if (at)
	  {
	    ReportWarning (
	        "crystalspace.maploader.parse.sound",
                child, "The 'mode3d' attribute is deprecated! Specify 2d/3d mode when playing sound.");
	    const char* v = at->GetValue ();
	    if (!strcasecmp ("disable", v))
	      mode3d = CS_SND3D_DISABLE;
	    else if (!strcasecmp ("relative", v))
	      mode3d = CS_SND3D_RELATIVE;
	    else if (!strcasecmp ("absolute", v))
	      mode3d = CS_SND3D_ABSOLUTE;
	    else
	    {
	      SyntaxService->ReportError (
	        "crystalspace.maploader.parse.sound", child,
	        "Use either 'disable', 'relative', or 'absolute' for mode3d attribute!");
	      return false;
	    }
	  }

	  // New sound system.
	  if (!SndSysLoader || !SndSysManager)
	  {
	    //SyntaxService->ReportError (
	        //"crystalspace.maploader.parse.sound", child,
	        //"New sound loader not loaded!");
	    return true;
	  }
          iSndSysWrapper* snd = SndSysManager->FindSoundByName (name);
          if (!snd)
	  {	    
            snd = LoadSoundWrapper (name, filename);
	  }
          if (snd)
          {
            csRef<iDocumentNodeIterator> it2 (child->GetNodes ());
            while (it2->HasNext ())
            {
              csRef<iDocumentNode> child2 = it2->Next ();
              if (child2->GetType () != CS_NODE_ELEMENT) continue;
              switch (xmltokens.Request (child2->GetValue ()))
              {
                case XMLTOKEN_KEY:
                  {
                    if (!ParseKey (child2, snd->QueryObject()))
                      return false;
                  }
                  break;
              }
            }
          }
        }
        break;
      default:
        SyntaxService->ReportBadToken (child);
        return false;
    }
  }

  return true;
}

bool csLoader::LoadLodControl (iLODControl* lodctrl, iDocumentNode* node)
{
  lodctrl->SetLOD (0, 1);
  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_DISTANCE:
	{
	  csRef<iDocumentAttribute> at = child->GetAttribute ("varm");
	  if (at)
	  {
	    // We use variables.
	    iSharedVariable *varm = Engine->GetVariableList()->FindByName (
	    	child->GetAttributeValue ("varm"));
	    iSharedVariable *vara = Engine->GetVariableList()->FindByName (
	    	child->GetAttributeValue ("vara"));
	    lodctrl->SetLOD (varm, vara);
	    break;
	  }

	  at = child->GetAttribute ("m");
	  if (at)
	  {
	    float lodm = child->GetAttributeValueAsFloat ("m");
	    float loda = child->GetAttributeValueAsFloat ("a");
	    lodctrl->SetLOD (lodm, loda);
	  }
	  else
	  {
	    float d0 = child->GetAttributeValueAsFloat ("d0");
	    float d1 = child->GetAttributeValueAsFloat ("d1");
	    float lodm = 1.0 / (d1-d0);
	    float loda = -lodm * d0;
	    lodctrl->SetLOD (lodm, loda);
	  }
	}
        break;
      case XMLTOKEN_FADE:
	{
	  csRef<iDocumentAttribute> at = child->GetAttribute ("varf");
	  if (at)
	  {
	    // We use variables.
	    iSharedVariable *varf = Engine->GetVariableList()->FindByName (
	    	child->GetAttributeValue ("varf"));
	    lodctrl->SetLODFade (varf);
	    break;
	  }

	  at = child->GetAttribute ("f");
	  if (at)
	  {
	    float lodf = child->GetAttributeValueAsFloat ("f");
	    lodctrl->SetLODFade (lodf);
	  }
	  else
	  {
	    float d = child->GetAttributeValueAsFloat ("d");
	    float lodm, loda;
	    lodctrl->GetLOD (lodm, loda);
	    float d0 = loda/-lodm;
	    float d1 = 1.0/lodm + d0;
	    float lodf = (d1-d0)/(2*d);
	    lodctrl->SetLODFade (lodf);
	  }
	}
        break;
      default:
	SyntaxService->ReportBadToken (child);
        return false;
    }
  }

  return true;
}

//--------------------------------------------------------------------

bool csLoader::ParseTriMeshChildBox (iDocumentNode* child,
	csRef<iTriangleMesh>& trimesh)
{
  csBox3 b;
  if (!SyntaxService->ParseBox (child, b))
    return false;
  trimesh = csPtr<iTriangleMesh> (new csTriangleMeshBox (b));
  return true;
}

bool csLoader::ParseTriMeshChildMesh (iDocumentNode* child,
	csRef<iTriangleMesh>& trimesh)
{
  int num_vt = 0;
  int num_tri = 0;
  csRef<iDocumentNodeIterator> child_it = child->GetNodes ();
  while (child_it->HasNext ())
  {
    csRef<iDocumentNode> child_child = child_it->Next ();
    if (child_child->GetType () != CS_NODE_ELEMENT) continue;
    const char* child_value = child_child->GetValue ();
    csStringID child_id = xmltokens.Request (child_value);
    switch (child_id)
    {
      case XMLTOKEN_V: num_vt++; break;
      case XMLTOKEN_T: num_tri++; break;
      default:
	SyntaxService->ReportBadToken (child_child);
	return false;
    }
  }

  csTriangleMesh* cstrimesh = new csTriangleMesh ();
  trimesh.AttachNew (cstrimesh);
  
  num_vt = 0;
  num_tri = 0;

  child_it = child->GetNodes ();
  while (child_it->HasNext ())
  {
    csRef<iDocumentNode> child_child = child_it->Next ();
    if (child_child->GetType () != CS_NODE_ELEMENT) continue;
    const char* child_value = child_child->GetValue ();
    csStringID child_id = xmltokens.Request (child_value);
    switch (child_id)
    {
      case XMLTOKEN_V:
	{
	  csVector3 vt;
	  if (!SyntaxService->ParseVector (child_child, vt))
	    return false;
	  cstrimesh->AddVertex (vt);
	  num_vt++;
	}
	break;
      case XMLTOKEN_T:
	{
	  int a = child_child->GetAttributeValueAsInt ("v1");
	  int b = child_child->GetAttributeValueAsInt ("v2");
	  int c = child_child->GetAttributeValueAsInt ("v3");
	  cstrimesh->AddTriangle (a, b, c);
	  num_tri++;
	}
	break;
      default:
	SyntaxService->ReportBadToken (child_child);
	return false;
    }
  }
  return true;
}

#undef CS_CLO
#undef CS_CON
#define CS_CLO (CS_TRIMESH_CLOSED|CS_TRIMESH_NOTCLOSED)
#define CS_CON (CS_TRIMESH_CONVEX|CS_TRIMESH_NOTCONVEX)
static void SetTriMeshFlags (csFlags& flags, bool closed, bool notclosed,
    bool convex, bool notconvex)
{
  if (closed) flags.Set (CS_CLO, CS_TRIMESH_CLOSED);
  if (notclosed) flags.Set (CS_CLO, CS_TRIMESH_NOTCLOSED);
  if (convex) flags.Set (CS_CON, CS_TRIMESH_CONVEX);
  if (notconvex) flags.Set (CS_CON, CS_TRIMESH_NOTCONVEX);
}
#undef CS_CLO
#undef CS_CON

bool csLoader::ParseTriMesh (iDocumentNode* node, iObjectModel* objmodel)
{
  csRef<iTriangleMesh> trimesh;
  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  bool convex = false;
  bool notconvex = false;
  bool closed = false;
  bool notclosed = false;
  bool use_default_mesh = false;
  csArray<csStringID> ids;
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_DEFAULT:
        if (trimesh)
	{
	  SyntaxService->ReportError (
	    "crystalspace.maploader.parse.trimesh", child,
	    "Use either <default>, <box>, or <mesh>!");
	  return false;
	}
	use_default_mesh = true;
        break;
      case XMLTOKEN_BOX:
        if (trimesh || use_default_mesh)
	{
	  SyntaxService->ReportError (
	    "crystalspace.maploader.parse.trimesh", child,
	    "Use either <default>, <box>, or <mesh>!");
	  return false;
	}
        if (!ParseTriMeshChildBox (child, trimesh))
	  return false;
        break;
      case XMLTOKEN_MESH:
        if (trimesh || use_default_mesh)
	{
	  SyntaxService->ReportError (
	    "crystalspace.maploader.parse.trimesh", child,
	    "Use either <default>, <box>, or <mesh>!");
	  return false;
	}
        if (!ParseTriMeshChildMesh (child, trimesh))
	  return false;
        break;
      case XMLTOKEN_CLOSED:
        closed = true;
	notclosed = false;
        break;
      case XMLTOKEN_NOTCLOSED:
        closed = false;
        notclosed = true;
        break;
      case XMLTOKEN_CONVEX:
        convex = true;
	notconvex = false;
        break;
      case XMLTOKEN_NOTCONVEX:
        convex = false;
        notconvex = true;
        break;
      case XMLTOKEN_COLLDET:
	ReportWarning (
	        "crystalspace.maploader.parse.trimesh",
                child, "<colldet> is deprecated. Use <id>colldet</id> instead.");
	ids.Push (stringSet->Request ("colldet"));
	break;
      case XMLTOKEN_VISCULL:
	ReportWarning (
	        "crystalspace.maploader.parse.trimesh",
                child, "<viscull> is deprecated. Use <id>viscull</id> instead.");
	ids.Push (stringSet->Request ("viscull"));
	break;
      case XMLTOKEN_SHADOWS:
	ReportWarning (
	        "crystalspace.maploader.parse.trimesh",
                child, "<shadows> is deprecated. Use <id>shadows</id> instead.");
	ids.Push (stringSet->Request ("shadows"));
	break;
      case XMLTOKEN_ID:
	ids.Push (stringSet->Request (child->GetContentsValue ()));
	break;
      default:
	SyntaxService->ReportBadToken (child);
        return false;
    }
  }
  if (ids.GetSize () == 0)
  {
    SyntaxService->ReportError (
	"crystalspace.maploader.parse.trimesh",
	node, "No id's for this triangle mesh!");
    return false;
  }
  size_t i;
  if (use_default_mesh)
  {
    for (i = 0 ; i < ids.GetSize () ; i++)
    {
      if (objmodel->GetTriangleData (ids[i]))
      {
        csFlags& flags = objmodel->GetTriangleData (ids[i])->GetFlags ();
        SetTriMeshFlags (flags, closed, notclosed, convex, notconvex);
      }
    }
  }
  else
  {
    if (trimesh)
    {
      csFlags& flags = trimesh->GetFlags ();
      SetTriMeshFlags (flags, closed, notclosed, convex, notconvex);
    }

    for (i = 0 ; i < ids.GetSize () ; i++)
      objmodel->SetTriangleData (ids[i], trimesh);
  }

  return true;
}

bool csLoader::LoadMeshObjectFactory (iLoaderContext* ldr_context,
	iMeshFactoryWrapper* stemp, iMeshFactoryWrapper* parent,
	iDocumentNode* node, csReversibleTransform* transf,
	iStreamSource* ssource)
{
  iLoaderPlugin* plug = 0;
  iBinaryLoaderPlugin* binplug = 0;
  iMaterialWrapper *mat = 0;
  bool staticshape = false;
  csRef<iDocumentNodeIterator> prev_it;
  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (true)
  {
    if (!it->HasNext ())
    {
      // Iterator has finished. Check if we still have to continue
      // with the normal iterator first (non-defaults).
      if (!prev_it) break;
      it = prev_it;
      prev_it = 0;
      continue;
    }

    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_LOD:
        {
	  if (!stemp->GetMeshObjectFactory ())
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.meshfactory",
              child, "Please use 'params' before specifying LOD!");
	    return false;
	  }
          csRef<iLODControl> lodctrl (scfQueryInterface<iLODControl> (
	    	stemp->GetMeshObjectFactory ()));
	  if (!lodctrl)
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.meshfactory",
              child, "This mesh factory doesn't implement LOD control!");
	    return false;
	  }
	  if (!LoadLodControl (lodctrl, child))
	    return false;
	}
        break;
      case XMLTOKEN_KEY:
        {
          if (!ParseKey (child, stemp->QueryObject()))
            return false;
        }
        break;
      case XMLTOKEN_ADDON:
	if (!LoadAddOn (ldr_context, child, stemp, false, ssource))
	  return false;
      	break;
      case XMLTOKEN_META:
	if (!LoadAddOn (ldr_context, child, stemp, true, ssource))
	  return false;
      	break;
      case XMLTOKEN_LODLEVEL:
        {
          if (!parent)
	  {
	    SyntaxService->ReportError (
	  	    "crystalspace.maploader.load.meshfactory", child,
		    "Factory must be part of a hierarchy for <lodlevel>!");
	    return false;
	  }
	  parent->AddFactoryToStaticLOD (child->GetContentsValueAsInt (),
	  	stemp);
        }
        break;
      case XMLTOKEN_STATICLOD:
        {
	  iLODControl* lodctrl = stemp->CreateStaticLOD ();
	  if (!LoadLodControl (lodctrl, child))
	    return false;
        }
        break;
      case XMLTOKEN_STATICSHAPE:
        if (!SyntaxService->ParseBool (child, staticshape, true))
	  return false;
        break;
      case XMLTOKEN_NULLMESH:
        {
	  if (plug)
	  {
            SyntaxService->ReportError (
	        "crystalspace.maploader.load.plugin",
                child, "Don't specify the plugin if you use <nullmesh>!");
	    return false;
	  }
	  if (stemp->GetMeshObjectFactory ())
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.meshfactory", child,
	      "Please don't use <params> in combination with <nullmesh>!");
	    return false;
	  }
	  csRef<iMeshObjectType> type = csLoadPluginCheck<iMeshObjectType> (
		object_reg, "crystalspace.mesh.object.null", false);
	  if (!type)
	  {
            SyntaxService->ReportError (
	        "crystalspace.maploader.load.plugin",
                child, "Could not find the nullmesh plugin!");
	    return false;
	  }
	  csRef<iMeshObjectFactory> fact = type->NewFactory ();
	  stemp->SetMeshObjectFactory (fact);
	  fact->SetMeshFactoryWrapper (stemp);
	  csBox3 b;
	  if (!SyntaxService->ParseBox (child, b))
	    return false;
          csRef<iNullFactoryState> nullmesh = 
            scfQueryInterface<iNullFactoryState> (fact);
	  nullmesh->SetBoundingBox (b);
	}
        break;
      case XMLTOKEN_PARAMS:
	if (!plug)
	{
          SyntaxService->ReportError (
	      "crystalspace.maploader.load.plugin",
              child, "Could not load plugin!");
	  return false;
	}
	else
	{
	  // We give here the iMeshObjectFactory as the context. If this
	  // is a new factory this will be 0. Otherwise it is possible
	  // to append information to the already loaded factory.
	  csRef<iBase> mof = plug->Parse (child, ssource, ldr_context,
	  	stemp->GetMeshObjectFactory ());
	  if (!mof)
	  {
	    // Error is reported by plug->Parse().
	    return false;
	  }
	  else
	  {
	    csRef<iMeshObjectFactory> mof2 (
	    	scfQueryInterface<iMeshObjectFactory> (mof));
	    if (!mof2)
	    {
              SyntaxService->ReportError (
	        "crystalspace.maploader.parse.meshfactory",
		child,
		"Returned object does not implement iMeshObjectFactory!");
	      return false;
	    }
	    stemp->SetMeshObjectFactory (mof2);
	    mof2->SetMeshFactoryWrapper (stemp);
	  }
	}
        break;
      case XMLTOKEN_PARAMSFILE:
	if (!plug && !binplug)
	{
          SyntaxService->ReportError (
	      "crystalspace.maploader.load.plugin",
              child, "Could not load plugin!");
	  return false;
	}
	else
        {
          csRef<iFile> buf (VFS->Open (child->GetContentsValue (),
	  	VFS_FILE_READ));
	  if (!buf)
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.loadingfile",
	      child, "Error opening file '%s'!", child->GetContentsValue ());
	    return false;
	  }
	  // We give here the iMeshObjectFactory as the context. If this
	  // is a new factory this will be 0. Otherwise it is possible
	  // to append information to the already loaded factory.
	  csRef<iBase> mof;
	  if (plug)
	    mof = LoadStructuredMap (ldr_context,
	    	plug, buf, stemp->GetMeshObjectFactory (),
	    	child->GetContentsValue (), ssource);
	  else
	  {
	    csRef<iDataBuffer> dbuf = VFS->ReadFile (
	    	child->GetContentsValue ());
	    mof = binplug->Parse (dbuf,
	  	ssource, ldr_context, stemp->GetMeshObjectFactory ());
	  }
	  if (!mof)
	  {
	    // Error is reported by plug->Parse().
	    return false;
	  }
	  else
	  {
	    csRef<iMeshObjectFactory> mof2 (
	    	scfQueryInterface<iMeshObjectFactory> (mof));
	    if (!mof2)
	    {
              SyntaxService->ReportError (
	        "crystalspace.maploader.parse.meshfactory",
		child,
		"Returned object does not implement iMeshObjectFactory!");
	      return false;
	    }
	    stemp->SetMeshObjectFactory (mof2);
	    mof2->SetMeshFactoryWrapper (stemp);
	  }
        }
        break;

      case XMLTOKEN_TRIMESH:
        {
	  if (!stemp->GetMeshObjectFactory ())
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.meshfactory",
              child, "Please use 'params' before specifying 'trimesh'!");
	    return false;
	  }
	  iObjectModel* objmodel = stemp->GetMeshObjectFactory ()
	  	->GetObjectModel ();
	  if (!objmodel)
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.meshfactory", child,
	      "This factory doesn't support setting of other 'trimesh'!");
	    return false;
	  }
	  if (!ParseTriMesh (child, objmodel))
	  {
	    // Error already reported.
	    return false;
	  }
	}
	break;

      case XMLTOKEN_CLOSED:
	if (!stemp->GetMeshObjectFactory ())
	{
          SyntaxService->ReportError (
	    "crystalspace.maploader.parse.meshfactory",
            child, "Please use 'params' before specifying 'closed'!");
	  return false;
	}
	else
        {
	  iObjectModel* objmodel = stemp->GetMeshObjectFactory ()
	  	->GetObjectModel ();
	  csRef<iTriangleMeshIterator> it = objmodel->GetTriangleDataIterator ();
	  while (it->HasNext ())
	  {
	    csStringID id;
	    iTriangleMesh* trimesh = it->Next (id);
	    if (trimesh) trimesh->GetFlags ().Set (
	      CS_TRIMESH_CLOSED | CS_TRIMESH_NOTCLOSED, CS_TRIMESH_CLOSED);
	  }
        }
        break;
      case XMLTOKEN_CONVEX:
	if (!stemp->GetMeshObjectFactory ())
	{
          SyntaxService->ReportError (
	    "crystalspace.maploader.parse.meshfactory",
            child, "Please use 'params' before specifying 'convex'!");
	  return false;
	}
	else
        {
	  iObjectModel* objmodel = stemp->GetMeshObjectFactory ()
	  	->GetObjectModel ();
	  csRef<iTriangleMeshIterator> it = objmodel->GetTriangleDataIterator ();
	  while (it->HasNext ())
	  {
	    csStringID id;
	    iTriangleMesh* trimesh = it->Next (id);
	    if (trimesh) trimesh->GetFlags ().Set (
	      CS_TRIMESH_CONVEX | CS_TRIMESH_NOTCONVEX, CS_TRIMESH_CONVEX);
	  }
        }
        break;
      case XMLTOKEN_MATERIAL:
        {
	  if (!stemp->GetMeshObjectFactory ())
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.meshfactory",
              child, "Please use 'params' before specifying 'material'!");
	    return false;
	  }
	  const char* matname = child->GetContentsValue ();
          mat = ldr_context->FindMaterial (matname);
          if (mat)
	  {
	    if (!stemp->GetMeshObjectFactory ()->SetMaterialWrapper (mat))
	    {
              SyntaxService->ReportError (
	        "crystalspace.maploader.parse.meshfactory",
                child, "This factory doesn't support setting materials this way!");
	      return false;
	    }
	  }
          else
          {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.unknownmaterial",
              child, "Material '%s' not found!", matname);
	    return false;
          }
        }
        break;

      case XMLTOKEN_FILE:
        {
	  ReportWarning (
	        "crystalspace.maploader.parse.meshfact",
                child, "<file> is now obsolete. Use <meshfact> in the world!");
#if 0
	  const char* filename = child->GetContentsValue ();
	  csRef<iDocumentAttribute> attr_name = child->GetAttribute ("name");
	  iBase* result;
	  if (!Load (filename, result, ldr_context->GetRegion (),
	  	ldr_context->CurrentRegionOnly (),
		ldr_context->CheckDupes (),
		ssource, attr_name ? attr_name->GetValue () : 0))
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.loadingmodel",
	      child, "Error opening file model '%s'!", filename);
	    return false;
	  }
#endif
        }
        break;

      case XMLTOKEN_PLUGIN:
        {
	  if (prev_it || plug || binplug)
	  {
	    SyntaxService->ReportError (
 	        "crystalspace.maploader.parse.meshfact",
	        child, "Please specify only one plugin!");
	    return false;
	  }

	  iDocumentNode* defaults = 0;
	  if (!loaded_plugins.FindPlugin (child->GetContentsValue (),
		plug, binplug, defaults))
	  {
	    SyntaxService->ReportError (
 	        "crystalspace.maploader.parse.meshfact",
	        child, "Error loading plugin '%s'!",
		child->GetContentsValue ());
	    return false;
	  }
	  if (defaults)
	  {
	    // Set aside current iterator and start a new one.
	    prev_it = it;
	    it = defaults->GetNodes ();
	  }
	}
        break;

      case XMLTOKEN_MESHFACT:
        {
          csRef<iMeshFactoryWrapper> t = Engine->CreateMeshFactory (
	  	child->GetAttributeValue ("name"));
	  csReversibleTransform child_transf;
          if (!LoadMeshObjectFactory (ldr_context, t, stemp, child,
	  	&child_transf, ssource))
	  {
	    // Error is already reported above.
	    return false;
	  }
	  else
	  {
	    AddToCollection (ldr_context, t->QueryObject ());
	  }
	  stemp->GetChildren ()->Add (t);
	  t->SetTransform (child_transf);
        }
	break;

      case XMLTOKEN_MOVE:
        {
	  if (!transf)
	  {
	    SyntaxService->ReportError (
	    	"crystalspace.maploader.load.meshfactory",
		child,
		"'move' is only useful for hierarchical transformations!");
	    return false;
	  }
	  csRef<iDocumentNode> matrix_node = child->GetNode ("matrix");
	  if (matrix_node)
	  {
	    csMatrix3 m;
	    if (!SyntaxService->ParseMatrix (matrix_node, m))
	      return false;
            transf->SetO2T (m);
	  }
	  csRef<iDocumentNode> vector_node = child->GetNode ("v");
	  if (vector_node)
	  {
	    csVector3 v;
	    if (!SyntaxService->ParseVector (vector_node, v))
	      return false;
            transf->SetO2TTranslation (v);
	  }
        }
        break;
      case XMLTOKEN_HARDMOVE:
        {
	  if (!stemp->GetMeshObjectFactory ())
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.meshfactory",
              child, "Please use 'params' before specifying 'hardmove'!");
	    return false;
	  }
	  if (!stemp->GetMeshObjectFactory ()->SupportsHardTransform ())
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.meshfactory",
              child, "This factory doesn't support 'hardmove'!");
	    return false;
	  }
	  csReversibleTransform tr;
	  csRef<iDocumentNode> matrix_node = child->GetNode ("matrix");
	  if (matrix_node)
	  {
	    csMatrix3 m;
	    if (!SyntaxService->ParseMatrix (matrix_node, m))
	      return false;
            tr.SetT2O (m);
	  }
	  csRef<iDocumentNode> vector_node = child->GetNode ("v");
	  if (vector_node)
	  {
	    csVector3 v;
	    if (!SyntaxService->ParseVector (vector_node, v))
	      return false;
            tr.SetOrigin (v);
	  }
	  stemp->HardTransform (tr);
        }
        break;

      case XMLTOKEN_ZUSE:
        stemp->SetZBufMode (CS_ZBUF_USE);
        break;
      case XMLTOKEN_ZFILL:
        stemp->SetZBufMode (CS_ZBUF_FILL);
        break;
      case XMLTOKEN_ZNONE:
        stemp->SetZBufMode (CS_ZBUF_NONE);
        break;
      case XMLTOKEN_ZTEST:
        stemp->SetZBufMode (CS_ZBUF_TEST);
        break;
      case XMLTOKEN_PRIORITY:
	{
	  const char* priname = child->GetContentsValue ();
	  long pri = Engine->GetRenderPriority (priname);
	  if (pri == 0)
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.meshfactory",
              child, "Unknown render priority '%s'!", priname);
	    return false;
	  }
          stemp->SetRenderPriority (pri);
	}
        break;
      case XMLTOKEN_SHADERVAR:
	{
	  if (!stemp->GetMeshObjectFactory ())
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.meshfactory",
              child, "Please use 'params' before specifying 'shadervar'!");
	    return false;
	  }
	  csRef<iShaderVariableContext> svc = stemp->GetSVContext();
	  CS_ASSERT (svc.IsValid());
	  //create a new variable
	  csRef<csShaderVariable> var;
	  var.AttachNew (new csShaderVariable);
	  if (!SyntaxService->ParseShaderVar (ldr_context, child, *var))
	  {
	    break;
	  }
	  svc->AddVariable (var);
	}
	break;
      case XMLTOKEN_NOLIGHTING:
        stemp->GetFlags ().Set (CS_ENTITY_NOLIGHTING, CS_ENTITY_NOLIGHTING);
        break;
      case XMLTOKEN_NOSHADOWS:
        stemp->GetFlags ().Set (CS_ENTITY_NOSHADOWS, CS_ENTITY_NOSHADOWS);
        break;
      case XMLTOKEN_NOSHADOWCAST:
        stemp->GetFlags ().Set (CS_ENTITY_NOSHADOWCAST, CS_ENTITY_NOSHADOWCAST);
        break;
      case XMLTOKEN_NOSHADOWRECEIVE:
        stemp->GetFlags ().Set (CS_ENTITY_NOSHADOWRECEIVE, CS_ENTITY_NOSHADOWRECEIVE);
        break;
      case XMLTOKEN_NOCLIP:
        stemp->GetFlags ().Set (CS_ENTITY_NOCLIP, CS_ENTITY_NOCLIP);
        break;
      case XMLTOKEN_NOHITBEAM:
        stemp->GetFlags ().Set (CS_ENTITY_NOHITBEAM, CS_ENTITY_NOHITBEAM);
        break;
      case XMLTOKEN_INVISIBLEMESH:
        stemp->GetFlags ().Set (CS_ENTITY_INVISIBLEMESH,
		  CS_ENTITY_INVISIBLEMESH);
        break;
      case XMLTOKEN_INVISIBLE:
        stemp->GetFlags ().Set (CS_ENTITY_INVISIBLE, CS_ENTITY_INVISIBLE);
        break;
      case XMLTOKEN_DETAIL:
        stemp->GetFlags ().Set (CS_ENTITY_DETAIL, CS_ENTITY_DETAIL);
        break;
      case XMLTOKEN_STATICLIT:
        stemp->GetFlags ().Set (CS_ENTITY_STATICLIT, CS_ENTITY_STATICLIT);
        break;
      case XMLTOKEN_LIMITEDSHADOWCAST:
        stemp->GetFlags ().Set (CS_ENTITY_LIMITEDSHADOWCAST,
          CS_ENTITY_LIMITEDSHADOWCAST);
        break;
      case XMLTOKEN_IMPOSTER:
        {
          csRef<iImposter> imposter = scfQueryInterface<iImposter> (stemp);
          if (!imposter)
          {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.meshfactory",
	      node, "This factory doesn't implement impostering!");
            return false;
          }
          if (!ParseImposterSettings (imposter, child))
            return false;
        }
        break;
      default:
	SyntaxService->ReportBadToken (child);
        return false;
    }
  }

  stemp->GetMeshObjectFactory ()->GetFlags ().SetBool (CS_FACTORY_STATICSHAPE,
  	staticshape);

  return true;
}

// Return true if the matrix does not scale.
static bool TestOrthoMatrix (csMatrix3& m)
{
  // Test if the matrix does not scale. Scaling meshes is illegal
  // in CS (must be done through hardmove).
  csVector3 v = m * csVector3 (1, 1, 1);
  float norm = v.Norm ();
  float desired_norm = 1.7320508f;
  return ABS (norm-desired_norm) < 0.01f;
}

bool csLoader::HandleMeshParameter (iLoaderContext* ldr_context,
	iMeshWrapper* mesh, iMeshWrapper* parent, iDocumentNode* child,
	csStringID id, bool& handled, csString& priority,
	bool do_portal_container, bool& staticpos, bool& staticshape,
	bool& zmodeChanged, bool& prioChanged,
	bool recursive, iStreamSource* ssource)
{
#undef TEST_MISSING_MESH
#define TEST_MISSING_MESH \
  if (!mesh) \
  { \
    SyntaxService->ReportError ( \
	"crystalspace.maploader.load.meshobject", \
	child, do_portal_container ? "Specify at least one portal first!" : \
	"First specify the parent factory with 'factory'!"); \
    return false; \
  }

  handled = true;
  switch (id)
  {
    case XMLTOKEN_STATICPOS:
      if (!SyntaxService->ParseBool (child, staticpos, true))
	return false;
      break;
    case XMLTOKEN_STATICSHAPE:
      if (!SyntaxService->ParseBool (child, staticshape, true))
	return false;
      break;
    case XMLTOKEN_MINRENDERDIST:
      {
	TEST_MISSING_MESH
	csRef<iDocumentAttribute> attr;
	if (attr = child->GetAttribute ("value"))
	{
	  float dist = attr->GetValueAsFloat ();
	  mesh->SetMinimumRenderDistance (dist);
	}
	else if (attr = child->GetAttribute ("var"))
	{
	  csString varname = attr->GetValue ();
	  iSharedVariable *var = Engine->GetVariableList()->FindByName (
	  	varname);
	  if (!var)
	  {
	    SyntaxService->ReportError (
		"crystalspace.maploader.parse.meshobject",
		child, "Variable '%s' doesn't exist!", varname.GetData ());
	    return false;
	  }
	  mesh->SetMinimumRenderDistanceVar (var);
	}
	else
	{
	  SyntaxService->ReportError (
		"crystalspace.maploader.parse.meshobject",
		child, "'value' or 'var' should be specified!");
	  return false;
	}
      }
      break;
    case XMLTOKEN_MAXRENDERDIST:
      {
	TEST_MISSING_MESH
	csRef<iDocumentAttribute> attr;
	if (attr = child->GetAttribute ("value"))
	{
	  float dist = attr->GetValueAsFloat ();
	  mesh->SetMaximumRenderDistance (dist);
	}
	else if (attr = child->GetAttribute ("var"))
	{
	  csString varname = attr->GetValue ();
	  iSharedVariable *var = Engine->GetVariableList()->FindByName (
	  	varname);
	  if (!var)
	  {
	    SyntaxService->ReportError (
		"crystalspace.maploader.parse.meshobject",
		child, "Variable '%s' doesn't exist!", varname.GetData ());
	    return false;
	  }
	  mesh->SetMaximumRenderDistanceVar (var);
	}
	else
	{
	  SyntaxService->ReportError (
		"crystalspace.maploader.parse.meshobject",
		child, "'value' or 'var' should be specified!");
	  return false;
	}
      }
      break;
    case XMLTOKEN_STATICLOD:
      {
	TEST_MISSING_MESH
	iLODControl* lodctrl = mesh->CreateStaticLOD ();
	if (!LoadLodControl (lodctrl, child))
	  return false;
      }
      break;
    case XMLTOKEN_LODLEVEL:
      {
	TEST_MISSING_MESH
        if (!parent)
	{
	  SyntaxService->ReportError (
	  	  "crystalspace.maploader.load.meshobject", child,
		  "Mesh must be part of a hierarchical mesh for <lodlevel>!");
	  return false;
	}
        if (!parent->GetStaticLOD ())
	{
	  SyntaxService->ReportError (
	  	  "crystalspace.maploader.load.meshobject", child,
		  "Parent mesh must use <staticlod>!");
	  return false;
	}
	parent->AddMeshToStaticLOD (child->GetContentsValueAsInt (), mesh);
      }
      break;
    case XMLTOKEN_LOD:
      {
	TEST_MISSING_MESH
	if (!mesh->GetMeshObject ())
	{
          SyntaxService->ReportError (
	      "crystalspace.maploader.parse.meshobject",
              child, "Mesh object is missing!");
	  return false;
	}
        csRef<iLODControl> lodctrl (scfQueryInterface<iLODControl> (
	    	mesh->GetMeshObject ()));
	if (!lodctrl)
	{
          SyntaxService->ReportError (
	      "crystalspace.maploader.parse.meshobject",
              child, "This mesh doesn't implement LOD control!");
	  return false;
	}
	if (!LoadLodControl (lodctrl, child))
	  return false;
      }
      break;
    case XMLTOKEN_PRIORITY:
      TEST_MISSING_MESH
      {
        priority = child->GetContentsValue ();
	long pri = Engine->GetRenderPriority (priority);
	if (pri == 0)
	{
          SyntaxService->ReportError (
	      "crystalspace.maploader.parse.meshobject",
              child, "Unknown render priority '%s'!", (const char*)priority);
	  return false;
	}
        if (recursive)
          mesh->SetRenderPriorityRecursive (pri);
        else
          mesh->SetRenderPriority (pri);
        prioChanged = true;
      }
      break;
    case XMLTOKEN_ADDON:
      TEST_MISSING_MESH
      if (!LoadAddOn (ldr_context, child, mesh, false, ssource))
	return false;
      break;
    case XMLTOKEN_META:
      TEST_MISSING_MESH
      if (!LoadAddOn (ldr_context, child, mesh, true, ssource))
	return false;
      break;
    case XMLTOKEN_NOLIGHTING:
      TEST_MISSING_MESH
      if (recursive)
        mesh->SetFlagsRecursive (CS_ENTITY_NOLIGHTING, CS_ENTITY_NOLIGHTING);
      else
        mesh->GetFlags ().Set (CS_ENTITY_NOLIGHTING, CS_ENTITY_NOLIGHTING);
      break;
    case XMLTOKEN_NOSHADOWS:
      TEST_MISSING_MESH
      if (recursive)
        mesh->SetFlagsRecursive (CS_ENTITY_NOSHADOWS, CS_ENTITY_NOSHADOWS);
      else
        mesh->GetFlags ().Set (CS_ENTITY_NOSHADOWS, CS_ENTITY_NOSHADOWS);
      break;
    case XMLTOKEN_NOSHADOWCAST:
      TEST_MISSING_MESH
      if (recursive)
        mesh->SetFlagsRecursive (CS_ENTITY_NOSHADOWCAST, CS_ENTITY_NOSHADOWCAST);
      else
        mesh->GetFlags ().Set (CS_ENTITY_NOSHADOWCAST, CS_ENTITY_NOSHADOWCAST);
      break;
    case XMLTOKEN_NOSHADOWRECEIVE:
      TEST_MISSING_MESH
      if (recursive)
        mesh->SetFlagsRecursive (CS_ENTITY_NOSHADOWRECEIVE, CS_ENTITY_NOSHADOWRECEIVE);
      else
        mesh->GetFlags ().Set (CS_ENTITY_NOSHADOWRECEIVE, CS_ENTITY_NOSHADOWRECEIVE);
      break;
    case XMLTOKEN_NOCLIP:
      TEST_MISSING_MESH
      if (recursive)
        mesh->SetFlagsRecursive (CS_ENTITY_NOCLIP, CS_ENTITY_NOCLIP);
      else
        mesh->GetFlags ().Set (CS_ENTITY_NOCLIP, CS_ENTITY_NOCLIP);
      break;
    case XMLTOKEN_NOHITBEAM:
      TEST_MISSING_MESH
      if (recursive)
        mesh->SetFlagsRecursive (CS_ENTITY_NOHITBEAM, CS_ENTITY_NOHITBEAM);
      else
        mesh->GetFlags ().Set (CS_ENTITY_NOHITBEAM, CS_ENTITY_NOHITBEAM);
      break;
    case XMLTOKEN_INVISIBLEMESH:
      TEST_MISSING_MESH
      if (recursive)
        mesh->SetFlagsRecursive (CS_ENTITY_INVISIBLEMESH,
		CS_ENTITY_INVISIBLEMESH);
      else
        mesh->GetFlags ().Set (CS_ENTITY_INVISIBLEMESH,
		CS_ENTITY_INVISIBLEMESH);
      break;
    case XMLTOKEN_INVISIBLE:
      TEST_MISSING_MESH
      if (recursive)
        mesh->SetFlagsRecursive (CS_ENTITY_INVISIBLE, CS_ENTITY_INVISIBLE);
      else
        mesh->GetFlags ().Set (CS_ENTITY_INVISIBLE, CS_ENTITY_INVISIBLE);
      break;
    case XMLTOKEN_DETAIL:
      TEST_MISSING_MESH
      if (recursive)
        mesh->SetFlagsRecursive (CS_ENTITY_DETAIL, CS_ENTITY_DETAIL);
      else
        mesh->GetFlags ().Set (CS_ENTITY_DETAIL, CS_ENTITY_DETAIL);
      break;
    case XMLTOKEN_STATICLIT:
      TEST_MISSING_MESH
      if (recursive)
        mesh->SetFlagsRecursive (CS_ENTITY_STATICLIT, CS_ENTITY_STATICLIT);
      else
        mesh->GetFlags ().Set (CS_ENTITY_STATICLIT, CS_ENTITY_STATICLIT);
      break;
    case XMLTOKEN_LIMITEDSHADOWCAST:
      TEST_MISSING_MESH
      if (recursive)
        mesh->SetFlagsRecursive (CS_ENTITY_LIMITEDSHADOWCAST,
          CS_ENTITY_LIMITEDSHADOWCAST);
      else
        mesh->GetFlags ().Set (CS_ENTITY_LIMITEDSHADOWCAST,
          CS_ENTITY_LIMITEDSHADOWCAST);
      break;
    case XMLTOKEN_ZFILL:
      TEST_MISSING_MESH
      if (priority.IsEmpty ()) priority = "wall";
      if (recursive)
      {
        mesh->SetRenderPriorityRecursive (
		Engine->GetRenderPriority (priority));
        mesh->SetZBufModeRecursive (CS_ZBUF_FILL);
      }
      else
      {
        mesh->SetRenderPriority (Engine->GetRenderPriority (priority));
        mesh->SetZBufMode (CS_ZBUF_FILL);
      }
      zmodeChanged = true;
      break;
    case XMLTOKEN_ZUSE:
      TEST_MISSING_MESH
      if (priority.IsEmpty ()) priority = "object";
      if (recursive)
      {
        mesh->SetRenderPriorityRecursive (Engine->GetRenderPriority (priority));
        mesh->SetZBufModeRecursive (CS_ZBUF_USE);
      }
      else
      {
        mesh->SetRenderPriority (Engine->GetRenderPriority (priority));
        mesh->SetZBufMode (CS_ZBUF_USE);
      }
      zmodeChanged = true;
      break;
    case XMLTOKEN_ZNONE:
      TEST_MISSING_MESH
      if (priority.IsEmpty ()) priority = "sky";
      if (recursive)
      {
        mesh->SetRenderPriorityRecursive (Engine->GetRenderPriority (priority));
        mesh->SetZBufModeRecursive (CS_ZBUF_NONE);
      }
      else
      {
        mesh->SetRenderPriority (Engine->GetRenderPriority (priority));
        mesh->SetZBufMode (CS_ZBUF_NONE);
      }
      zmodeChanged = true;
      break;
    case XMLTOKEN_ZTEST:
      TEST_MISSING_MESH
      if (priority.IsEmpty ()) priority = "alpha";
      if (recursive)
      {
        mesh->SetRenderPriorityRecursive (Engine->GetRenderPriority (priority));
        mesh->SetZBufModeRecursive (CS_ZBUF_TEST);
      }
      else
      {
        mesh->SetRenderPriority (Engine->GetRenderPriority (priority));
        mesh->SetZBufMode (CS_ZBUF_TEST);
      }
      zmodeChanged = true;
      break;
    case XMLTOKEN_CAMERA:
      TEST_MISSING_MESH
      if (priority.IsEmpty ()) priority = "sky";
      if (recursive)
      {
        mesh->SetRenderPriorityRecursive (Engine->GetRenderPriority (priority));
        mesh->SetFlagsRecursive (CS_ENTITY_CAMERA, CS_ENTITY_CAMERA);
      }
      else
      {
        mesh->SetRenderPriority (Engine->GetRenderPriority (priority));
        mesh->GetFlags ().Set (CS_ENTITY_CAMERA, CS_ENTITY_CAMERA);
      }
      break;
    case XMLTOKEN_BADOCCLUDER:
      TEST_MISSING_MESH
      else
      {
	//Apply the flag CS_CULLER_HINT_BADOCCLUDER to all the meshes in 
        //the meshes' hierarchy, starting from the 'mesh' mesh object.
        csRefArray<iMeshWrapper> meshesArray;
        CollectAllChildren (mesh, meshesArray);
        size_t i, count = meshesArray.GetSize ();
        for (i = 0; i < count; i++)
        {
          csRef<iVisibilityObject> visobj = 
            scfQueryInterface<iVisibilityObject> (meshesArray[i]);
          if (visobj)
            visobj->GetCullerFlags ().Set (CS_CULLER_HINT_BADOCCLUDER);
        }
      }
      break;
    case XMLTOKEN_GOODOCCLUDER:
      TEST_MISSING_MESH
      else
      {
	//Apply the flag CS_CULLER_HINT_GOODOCCLUDER to all the meshes in 
        //the meshes' hierarchy, starting from the 'mesh' mesh object.
        csRefArray<iMeshWrapper> meshesArray;
        CollectAllChildren (mesh, meshesArray);
        size_t i, count = meshesArray.GetSize ();
        for (i = 0; i < count; i++)
        {
          csRef<iVisibilityObject> visobj = 
            scfQueryInterface<iVisibilityObject> (meshesArray[i]);
          if (visobj)
            visobj->GetCullerFlags ().Set (CS_CULLER_HINT_GOODOCCLUDER);
        }
      }
      break;
    case XMLTOKEN_CLOSED:
      TEST_MISSING_MESH
      else
      {
        if (recursive)//Test if recursion on children has been specified.
        {
          csRefArray<iMeshWrapper> meshesArray;
          CollectAllChildren (mesh, meshesArray);
          size_t i, count = meshesArray.GetSize ();
          for (i = 0; i < count; i++)
          {
            ClosedFlags (meshesArray[i]);
          }
        }//if
        else
          ClosedFlags (mesh);
      }
      break;
    case XMLTOKEN_CONVEX:
      TEST_MISSING_MESH
      else
      {
	//Test if recursion on children has been specified.
        if (recursive)
        {
          csRefArray<iMeshWrapper> meshesArray;
          CollectAllChildren (mesh, meshesArray);
          size_t i, count = meshesArray.GetSize ();
          for (i = 0; i < count; i++)
          {
            ConvexFlags (meshesArray[i]);
          }
        }
        else
          ConvexFlags (mesh);
      }
      break;
    case XMLTOKEN_KEY:
      TEST_MISSING_MESH
      else
      {
        if (!ParseKey (child, mesh->QueryObject()))
          return false;
      }
      break;
    case XMLTOKEN_HARDMOVE:
      TEST_MISSING_MESH
      if (!mesh->GetMeshObject())
      {
	SyntaxService->ReportError (
		"crystalspace.maploader.load.meshobject",
		child, "Please specify the params of the meshobject first!");
	return false;
      }
      else if (!mesh->GetMeshObject ()->SupportsHardTransform ())
      {
        SyntaxService->ReportError (
	    "crystalspace.maploader.parse.meshobject",
            child, "This mesh object doesn't support 'hardmove'!");
	return false;
      }
      else
      {
	csReversibleTransform tr;
	csRef<iDocumentNode> matrix_node = child->GetNode ("matrix");
	if (matrix_node)
	{
	  csMatrix3 m;
	  if (!SyntaxService->ParseMatrix (matrix_node, m))
	    return false;
          tr.SetT2O (m);
	}
	csRef<iDocumentNode> vector_node = child->GetNode ("v");
	if (vector_node)
	{
	  csVector3 v;
	  if (!SyntaxService->ParseVector (vector_node, v))
	    return false;
          tr.SetOrigin (v);
	}
	mesh->HardTransform (tr);
      }
      break;
    case XMLTOKEN_MOVE:
      TEST_MISSING_MESH
      else
      {
        mesh->GetMovable ()->SetTransform (csMatrix3 ());     // Identity
        mesh->GetMovable ()->SetPosition (csVector3 (0));
	csRef<iDocumentNode> matrix_node = child->GetNode ("matrix");
	if (matrix_node)
	{
	  csMatrix3 m;
	  if (!SyntaxService->ParseMatrix (matrix_node, m))
	    return false;
	  if (!TestOrthoMatrix (m))
	  {
	    ReportWarning (
	        "crystalspace.maploader.load.mesh",
                child, "Scaling of mesh objects is not allowed in CS!");
	  }
          mesh->GetMovable ()->SetTransform (m);
	}
	csRef<iDocumentNode> vector_node = child->GetNode ("v");
	if (vector_node)
	{
	  csVector3 v;
	  if (!SyntaxService->ParseVector (vector_node, v))
	    return false;
          mesh->GetMovable ()->SetPosition (v);
	}
	mesh->GetMovable ()->UpdateMove ();
      }
      break;
    case XMLTOKEN_BOX:
      TEST_MISSING_MESH
      else
      {
	csBox3 b;
	if (!SyntaxService->ParseBox (child, b))
	  return false;
        mesh->GetMeshObject ()->GetObjectModel ()->SetObjectBoundingBox (b);
      }
      break;
    case XMLTOKEN_SHADERVAR:
      TEST_MISSING_MESH
      else
      {
	csRef<iShaderVariableContext> svc = 
		scfQueryInterface<iShaderVariableContext> (mesh);
	CS_ASSERT (svc.IsValid());
        //create a new variable
        const char* varname = child->GetAttributeValue ("name");
	csRef<csShaderVariable> var;
	var.AttachNew (new csShaderVariable (stringSetSvName->Request (varname)));
	if (!SyntaxService->ParseShaderVar (ldr_context, child, *var))
        {
	  SyntaxService->ReportError (
	    "crystalspace.maploader.load.meshobject", child,
	    "Error loading shader variable '%s' in mesh '%s'.", 
	    varname, mesh->QueryObject()->GetName());
          break;
        }
	svc->AddVariable (var);
      }
      break;
    default:
      handled = false;
      return true;
  }
  return true;
#undef TEST_MISSING_MESH
}

csRef<iMeshWrapper> csLoader::LoadMeshObjectFromFactory (
	iLoaderContext* ldr_context, iDocumentNode* node,
	iStreamSource* ssource)
{
  if (!Engine) return 0;

  csString priority;

  csRef<iMeshWrapper> mesh;
  bool staticpos = false;
  bool staticshape = false;
  bool zbufSet = false;
  bool prioSet = false;

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    bool handled;
    if (!HandleMeshParameter (ldr_context, mesh, 0, child, id,
    	handled, priority, false, staticpos, staticshape, zbufSet,
	prioSet, true, ssource))
      return 0;
    if (!handled) switch (id)
    {
      case XMLTOKEN_FACTORY:
        if (mesh)
	{
	  SyntaxService->ReportError (
	  	"crystalspace.maploader.load.meshobject",
	  	child, "There is already a factory for this mesh!");
	  return 0;
	}
	else
	{
	  iMeshFactoryWrapper* t = ldr_context->FindMeshFactory (
	  	child->GetContentsValue ());
          if (!t)
	  {
	    SyntaxService->ReportError (
	  	"crystalspace.maploader.load.meshobject",
	  	child, "Can't find factory '%s'!", child->GetContentsValue ());
	    return 0;
	  }
	  mesh = t->CreateMeshWrapper ();
	  if (mesh)
	  {
	    AddToCollection (ldr_context, mesh->QueryObject ());
	    // Now also add the child mesh objects to the region.
            const csRef<iSceneNodeArray> children = 
              mesh->QuerySceneNode ()->GetChildrenArray ();
	    AddChildrenToCollection (ldr_context, children);
	  }
	}
        break;
      default:
	SyntaxService->ReportBadToken (child);
	return 0;
    }
  }

  if (!mesh)
  {
    SyntaxService->ReportError (
	  	"crystalspace.maploader.load.meshobject",
	  	node, "There is no 'factory' for this mesh!");
    return 0;
  }
  if (priority.IsEmpty ()) priority = "object";
  mesh->SetRenderPriorityRecursive (Engine->GetRenderPriority (priority));
  
  //I had to put these ugly curly brackets. It's due to the uglier label
  //below! 'children' and 'set' need an initialization indeed. Luca
  {
    csRefArray<iMeshWrapper> meshesArray;
    CollectAllChildren (mesh, meshesArray);
    size_t i, count = meshesArray.GetSize ();
    for (i = 0; i < count; i++)
    {
      iMeshWrapper* mesh = meshesArray[i];
      mesh->GetMeshObject ()->GetFlags ().SetBool (
        CS_MESH_STATICPOS, staticpos);
      mesh->GetMeshObject ()->GetFlags ().SetBool (
        CS_MESH_STATICSHAPE, staticshape);
    }
  }

  return mesh;
}

bool csLoader::LoadTriMeshInSector (iLoaderContext* ldr_context,
	iMeshWrapper* mesh, iDocumentNode* node, iStreamSource* ssource)
{
  iObjectModel* objmodel = mesh->GetMeshObject ()->GetObjectModel ();
  csRef<iTriangleMesh> trimesh;
  csArray<csStringID> ids;

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_ADDON:
        if (!LoadAddOn (ldr_context, child, mesh, false, ssource))
	  return false;
        break;
      case XMLTOKEN_META:
        if (!LoadAddOn (ldr_context, child, mesh, true, ssource))
	  return false;
        break;
      case XMLTOKEN_MOVE:
      {
        mesh->GetMovable ()->SetTransform (csMatrix3 ());     // Identity
        mesh->GetMovable ()->SetPosition (csVector3 (0));
	csRef<iDocumentNode> matrix_node = child->GetNode ("matrix");
	if (matrix_node)
	{
	  csMatrix3 m;
	  if (!SyntaxService->ParseMatrix (matrix_node, m))
	    return false;
	  if (!TestOrthoMatrix (m))
	  {
	    ReportWarning (
	        "crystalspace.maploader.load.mesh",
                child, "Scaling of mesh objects is not allowed in CS!");
	  }
          mesh->GetMovable ()->SetTransform (m);
	}
	csRef<iDocumentNode> vector_node = child->GetNode ("v");
	if (vector_node)
	{
	  csVector3 v;
	  if (!SyntaxService->ParseVector (vector_node, v))
	    return false;
          mesh->GetMovable ()->SetPosition (v);
	}
	mesh->GetMovable ()->UpdateMove ();
        break;
      }
      case XMLTOKEN_BOX:
        if (!ParseTriMeshChildBox (child, trimesh))
	  return false;
        break;
      case XMLTOKEN_MESH:
        if (!ParseTriMeshChildMesh (child, trimesh))
	  return false;
        break;
      case XMLTOKEN_COLLDET:
	ReportWarning (
	        "crystalspace.maploader.parse.trimesh",
                child, "<colldet> is deprecated. Use <id>colldet</id> instead.");
	ids.Push (stringSet->Request ("colldet"));
	break;
      case XMLTOKEN_VISCULL:
	ReportWarning (
	        "crystalspace.maploader.parse.trimesh",
                child, "<viscull> is deprecated. Use <id>viscull</id> instead.");
	ids.Push (stringSet->Request ("viscull"));
	break;
      case XMLTOKEN_SHADOWS:
	ReportWarning (
	        "crystalspace.maploader.parse.trimesh",
                child, "<shadows> is deprecated. Use <id>shadows</id> instead.");
	ids.Push (stringSet->Request ("shadows"));
	break;
      case XMLTOKEN_ID:
	ids.Push (stringSet->Request (child->GetContentsValue ()));
	break;
      default:
	SyntaxService->ReportBadToken (child);
        return false;
    }
  }

  if (ids.GetSize () == 0)
  {
    SyntaxService->ReportError (
	"crystalspace.maploader.parse.trimesh",
	node, "No id's for this triangle mesh!");
    return false;
  }
  if (!trimesh)
  {
    SyntaxService->ReportError (
	"crystalspace.maploader.parse.sector.trimesh",
	node, "Please specify either <mesh/> or <box/>!");
    return false;
  }

  size_t i;
  for (i = 0 ; i < ids.GetSize () ; i++)
    objmodel->SetTriangleData (ids[i], trimesh);

  csRef<iNullMeshState> nullmesh = scfQueryInterface<iNullMeshState> (
    	mesh->GetMeshObject ());
  CS_ASSERT (nullmesh != 0);
  csBox3 bbox;
  csVector3* vt = trimesh->GetVertices ();
  bbox.StartBoundingBox (vt[0]);
  for (i = 1 ; i < trimesh->GetVertexCount () ; i++)
  {
    bbox.AddBoundingVertexSmart (vt[i]);
  }
  nullmesh->SetBoundingBox (bbox);

  return true;
}

bool csLoader::HandleMeshObjectPluginResult (iBase* mo, iDocumentNode* child,
	iMeshWrapper* mesh, bool keepZbuf, bool keepPrio)
{
  csRef<iMeshObject> mo2 = scfQueryInterface<iMeshObject> (mo);
  if (!mo2)
  {
    SyntaxService->ReportError (
      "crystalspace.maploader.parse.mesh",
      child, "Returned object does not implement iMeshObject!");
    return false;
  }
  mesh->SetMeshObject (mo2);
  mo2->SetMeshWrapper (mesh);
  if (mo2->GetFactory () && mo2->GetFactory ()->GetMeshFactoryWrapper ())
  {
    iMeshFactoryWrapper* mfw = mo2->GetFactory ()->GetMeshFactoryWrapper ();
    if (mfw)
    {
      mesh->SetFactory (mfw);
      if (!keepZbuf) mesh->SetZBufMode (mfw->GetZBufMode ());
      if (!keepPrio) mesh->SetRenderPriority (mfw->GetRenderPriority ());
      mesh->GetFlags ().Set (mfw->GetFlags ().Get (),
      	mfw->GetFlags ().Get ());
    }
  }
  return true;
}

bool csLoader::LoadMeshObject (iLoaderContext* ldr_context,
	iMeshWrapper* mesh, iMeshWrapper* parent, iDocumentNode* node,
	iStreamSource* ssource)
{
  if (!Engine) return false;

  csString priority;

  iLoaderPlugin* plug = 0;
  iBinaryLoaderPlugin* binplug = 0;
  bool staticpos = false;
  bool staticshape = false;
  bool zbufSet = false;
  bool prioSet = false;

  csRef<iDocumentNodeIterator> prev_it;
  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (true)
  {
    if (!it->HasNext ())
    {
      // Iterator has finished. Check if we still have to continue
      // with the normal iterator first (non-defaults).
      if (!prev_it) break;
      it = prev_it;
      prev_it = 0;
      continue;
    }

    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    bool handled;
    if (!HandleMeshParameter (ldr_context, mesh, parent, child, id,
        handled, priority, false, staticpos, staticshape, zbufSet, prioSet,
        false, ssource))
      return false;
    if (!handled) switch (id)
    {
      case XMLTOKEN_PORTAL:
        {
	  iMeshWrapper* container_mesh = 0;
          if (!ParsePortal (ldr_context, child, 0, 0, container_mesh, mesh))
	    return 0;
	}
        break;
      case XMLTOKEN_PORTALS:
        if (!ParsePortals (ldr_context, child, 0, mesh, ssource))
	  return 0;
        break;
      case XMLTOKEN_MESHREF:
        {
          csRef<iMeshWrapper> sp = LoadMeshObjectFromFactory (ldr_context,
	      	child, ssource);
          if (!sp)
	  {
	    // Error is already reported.
	    return false;
	  }
	  sp->QueryObject ()->SetName (child->GetAttributeValue ("name"));
	  sp->QuerySceneNode ()->SetParent (mesh->QuerySceneNode ());
        }
        break;
      case XMLTOKEN_MESHOBJ:
        {
	  csRef<iMeshWrapper> sp = Engine->CreateMeshWrapper (
			  child->GetAttributeValue ("name"));
          if (!LoadMeshObject (ldr_context, sp, mesh, child, ssource))
	  {
	    // Error is already reported.
	    return false;
	  }
	  else
	  {
	    AddToCollection (ldr_context, sp->QueryObject ());
	  }
	  sp->QuerySceneNode ()->SetParent (mesh->QuerySceneNode ());
        }
        break;
      case XMLTOKEN_LIGHT:
        {
          iLight * light = ParseStatlight (ldr_context, child);
          if (light)
          {
            light->QuerySceneNode ()->SetParent (mesh->QuerySceneNode ());
          }
          else
          {
            return false;
          }
        }
	break;
      case XMLTOKEN_NULLMESH:
        {
	  if (plug)
	  {
            SyntaxService->ReportError (
	        "crystalspace.maploader.load.plugin",
                child, "Don't specify the plugin if you use <nullmesh>!");
	    return false;
	  }
	  if (mesh->GetMeshObject ())
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.mesh", child,
	      "Please don't use <params> in combination with <nullmesh>!");
	    return false;
	  }
	  csRef<iMeshObjectType> type = csLoadPluginCheck<iMeshObjectType> (
		object_reg, "crystalspace.mesh.object.null", false);
	  if (!type)
	  {
            SyntaxService->ReportError (
	        "crystalspace.maploader.load.plugin",
                child, "Could not find the nullmesh plugin!");
	    return false;
	  }
	  csRef<iMeshObjectFactory> fact = type->NewFactory ();
	  csRef<iMeshObject> mo = fact->NewInstance ();
	  mesh->SetMeshObject (mo);
	  mo->SetMeshWrapper (mesh);
	  csBox3 b;
	  if (!SyntaxService->ParseBox (child, b))
	    return false;
          csRef<iNullMeshState> nullmesh = 
            scfQueryInterface<iNullMeshState> (mo);
	  if (nullmesh)
	    nullmesh->SetBoundingBox (b);
	}
        break;

      case XMLTOKEN_PARAMS:
	if (!plug)
	{
          SyntaxService->ReportError (
	      "crystalspace.maploader.load.plugin",
              child, "Could not load plugin!");
	  return false;
	}
	else
	{
	  csRef<iBase> mo = plug->Parse (child, ssource, ldr_context,
	  	mesh);
          if (!mo || !HandleMeshObjectPluginResult (mo, child, mesh, zbufSet, 
	    prioSet))
	    return false;	// Error already reported.
	}
        break;
      case XMLTOKEN_FILE:
        {
	  const char* fname = child->GetContentsValue ();
	  if (!fname)
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.loadingfile",
	      child, "Specify a VFS filename with 'file'!");
	    return false;
	  }
          csRef<iFile> buf = VFS->Open (fname, VFS_FILE_READ);
	  if (!buf)
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.loadingfile",
	      child, "Error opening file '%s'!", fname);
	    return false;
	  }
	  csRef<iDocument> doc;
	  bool er = LoadStructuredDoc (fname, buf, doc);
	  if (!er)
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.loadingfile",
	      child, "'%s' is not an XML file!", fname);
	    return false;
	  }
	  csRef<iDocumentNode> paramsnode = doc->GetRoot ()->GetNode ("params");
	  if (paramsnode)
	  {
	    if (!plug && !binplug)
	    {
	      SyntaxService->ReportError (
		"crystalspace.maploader.load.plugin",
		child, "Could not load plugin for mesh '%s'!",
		mesh->QueryObject ()->GetName ());
	      return false;
	    }
	    csRef<iBase> mo;
	    if (plug)
	      mo = plug->Parse (paramsnode, ssource, ldr_context, mesh);
	    else
	    {
	      csRef<iDataBuffer> dbuf = VFS->ReadFile (fname);
	      mo = binplug->Parse (dbuf,
	  	  ssource, ldr_context, mesh);
	    }
            if (!mo || !HandleMeshObjectPluginResult (mo, child, mesh,
	      zbufSet, prioSet))
	      return false;	// Error already reported.
	    break;
	  }
	  csRef<iDocumentNode> meshobjnode = doc->GetRoot ()->GetNode (
	  	"meshobj");
	  if (meshobjnode)
	  {
	    if (!LoadMeshObject (ldr_context, mesh, parent, meshobjnode,
	    	ssource))
	      return false;
	    break;
	  }
	  csRef<iDocumentNode> meshfactnode = doc->GetRoot ()->GetNode (
	  	"meshfact");
	  if (meshfactnode)
	  {
	    const char* meshfactname = meshfactnode->GetAttributeValue ("name");
	    // @@@ Handle regions correctly here???
            csRef<iMeshFactoryWrapper> t = Engine->GetMeshFactories ()
	  	->FindByName (meshfactname);
	    if (!t)
	    {
              t = Engine->CreateMeshFactory (meshfactname);
	      if (!t || !LoadMeshObjectFactory (ldr_context, t, 0,
	      	meshfactnode, 0, ssource))
	      {
	        // Error is already reported.
	        return false;
	      }
	      else
	      {
	        AddToCollection (ldr_context, t->QueryObject ());
	      }
	    }
	    break;
	  }
	  SyntaxService->ReportError (
		"crystalspace.maploader.load.plugin", child,
		"File '%s' doesn't contain <params>, <meshobj>, nor <meshfact>!",
		fname);
	  return false;
	}
	break;
      case XMLTOKEN_PARAMSFILE:
	if (!plug && !binplug)
	{
          SyntaxService->ReportError (
	      "crystalspace.maploader.load.plugin",
              child, "Could not load plugin for mesh '%s'!",
	      mesh->QueryObject ()->GetName ());
	  return false;
	}
	else
        {
	  const char* fname = child->GetContentsValue ();
	  if (!fname)
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.loadingfile",
	      child, "Specify a VFS filename with 'paramsfile'!");
	    return false;
	  }
          csRef<iFile> buf (VFS->Open (fname, VFS_FILE_READ));
	  if (!buf)
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.loadingfile",
	      child, "Error opening file '%s'!", fname);
	    return false;
	  }
	  csRef<iBase> mo;
	  if (plug)
	    mo = LoadStructuredMap (ldr_context, plug, buf, mesh, fname,
	    	ssource);
	  else
	  {
	    csRef<iDataBuffer> dbuf = VFS->ReadFile (fname);
	    mo = binplug->Parse (dbuf,
	  	ssource, ldr_context, mesh);
	  }
          if (!mo || !HandleMeshObjectPluginResult (mo, child, mesh,
	      zbufSet, prioSet))
	    return false;	// Error already reported.
	}
        break;

      case XMLTOKEN_TRIMESH:
        {
	  if (!mesh->GetMeshObject ())
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.mesh",
              child, "Please use 'params' before specifying 'trimesh'!");
	    return false;
	  }
	  iObjectModel* objmodel = mesh->GetMeshObject ()->GetObjectModel ();
	  if (!objmodel)
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.mesh", child,
	      "This mesh doesn't support setting of other 'trimesh'!");
	    return false;
	  }
	  if (!ParseTriMesh (child, objmodel))
	  {
	    // Error already reported.
	    return false;
	  }
	}
	break;

      case XMLTOKEN_PLUGIN:
	{
	  if (prev_it || plug || binplug)
	  {
	    SyntaxService->ReportError (
 	        "crystalspace.maploader.parse.plugin",
	        child, "Please specify only one plugin!");
	    return false;
	  }

	  const char* plugname = child->GetContentsValue ();
	  if (!plugname)
	  {
            SyntaxService->ReportError (
	      "crystalspace.maploader.parse.plugin",
	      child, "Specify a plugin name with 'plugin'!");
	    return false;
	  }
	  iDocumentNode* defaults = 0;
	  if (!loaded_plugins.FindPlugin (plugname, plug, binplug, defaults))
	  {
	    SyntaxService->ReportError (
 	        "crystalspace.maploader.parse.meshobj",
	        child, "Error loading plugin '%s'!", plugname);
	    return false;
	  }
	  if (defaults)
	  {
	    // Set aside current iterator and start a new one.
	    prev_it = it;
	    it = defaults->GetNodes ();
	  }
	}
        break;
      default:
	SyntaxService->ReportBadToken (child);
	return false;
    }
  }

  if (!priority.IsEmpty ())
    mesh->SetRenderPriority (Engine->GetRenderPriority (priority));
  mesh->GetMeshObject ()->GetFlags ().SetBool (CS_MESH_STATICPOS, staticpos);
  mesh->GetMeshObject ()->GetFlags ().SetBool (CS_MESH_STATICSHAPE, staticshape);

  return true;
}

bool csLoader::ParseImposterSettings (iImposter* imposter,
    iDocumentNode *node)
{
  const char *s = node->GetAttributeValue ("active");
  if (s && !strcmp (s, "no"))
    imposter->SetImposterActive (false);
  else
    imposter->SetImposterActive (true);

  iSharedVariable *var = 0;

  s = node->GetAttributeValue ("range");
  if (s)
    var = Engine->GetVariableList()->FindByName (s);
  if (!s || !var)
  {
    SyntaxService->ReportError (
	    "crystalspace.maploader.parse.meshobject",
	    node, "Imposter range variable (%s) doesn't exist!", s);
    return false;
  }
  imposter->SetMinDistance (var);

  s = node->GetAttributeValue ("tolerance");
  if (s)
    var = Engine->GetVariableList ()->FindByName (s);
  if (!s || !var)
  {
    SyntaxService->ReportError (
	    "crystalspace.maploader.parse.meshobject", node,
	    "Imposter rotation tolerance variable (%s) doesn't exist!",
	    s);
    return false;
  }
  imposter->SetRotationTolerance (var);

  s = node->GetAttributeValue ("camera_tolerance");
  if (s)
    var = Engine->GetVariableList ()->FindByName (s);
  if (!s || !var)
  {
    SyntaxService->ReportError (
	    "crystalspace.maploader.parse.meshobject", node,
	    "Imposter camera rotation tolerance variable (%s) doesn't exist!",
	    s);
    return false;
  }
  imposter->SetCameraRotationTolerance (var);
  return true;
}

bool csLoader::LoadMeshGenGeometry (iLoaderContext* ldr_context,
                                    iDocumentNode* node,
				    iMeshGenerator* meshgen)
{
  iMeshGeneratorGeometry* geom = meshgen->CreateGeometry ();

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
        const char* factname = child->GetAttributeValue ("name");
        float maxdist = child->GetAttributeValueAsFloat ("maxdist");
        iMeshFactoryWrapper* fact = ldr_context->FindMeshFactory (factname);
        if (!fact)
        {
          SyntaxService->ReportError (
            "crystalspace.maploader.parse.meshgen",
            child, "Can't find mesh factory '%s' for mesh generator!",
            factname);
          return false;
        }
        geom->AddFactory (fact, maxdist);
      }
      break;
    case XMLTOKEN_POSITIONMAP:
      {
        const char* map_name = child->GetAttributeValue ("mapname");
        csRef<iTerraFormer> map = csQueryRegistryTagInterface<iTerraFormer> 
          (object_reg, map_name);
        if (!map)
        {
          SyntaxService->ReportError (
            "crystalspace.maploader.parse.meshgen",
            child, "Can't find map position map terraformer '%s'!", map_name);
          return false;
        }
        
        csVector2 min, max;
        csRef<iDocumentNode> region_node = child->GetNode ("region");

        SyntaxService->ParseVector (region_node->GetNode ("min"), min);
        SyntaxService->ParseVector (region_node->GetNode ("max"), max);

        float value = child->GetAttributeValueAsFloat ("value");

        uint resx = child->GetAttributeValueAsInt ("resx");
        uint resy = child->GetAttributeValueAsInt ("resy");
        
        csRef<iStringSet> strings = csQueryRegistryTagInterface<iStringSet> (
          object_reg, "crystalspace.shared.stringset");

        geom->AddPositionsFromMap (map, csBox2 (min.x, min.y, max.x, max.y), 
          resx, resy, value, strings->Request ("heights"));
      }
      break;
    case XMLTOKEN_DENSITYMAP:
      {
        const char* map_name = child->GetContentsValue ();
        csRef<iTerraFormer> map_tf = csQueryRegistryTagInterface<iTerraFormer> 
          (object_reg, map_name);
        if (!map_tf)
        {
          SyntaxService->ReportError (
            "crystalspace.maploader.parse.meshgen",
            child, "Can't find map density map terraformer '%s'!", map_name);
          return false;
        }
        float factor = child->GetAttributeValueAsFloat ("factor");
        csRef<iStringSet> strings = csQueryRegistryTagInterface<iStringSet> (
        object_reg, "crystalspace.shared.stringset");
        geom->SetDensityMap (map_tf, factor, strings->Request ("densitymap"));
      }
      break;
    case XMLTOKEN_MATERIALFACTOR:
      {
        const char* matname = child->GetAttributeValue ("material");
        if (!matname)
        {
          SyntaxService->ReportError (
            "crystalspace.maploader.parse.meshgen",
            child, "'material' attribute is missing!");
          return false;
        }
        iMaterialWrapper* mat = ldr_context->FindMaterial (matname);
        if (!mat)
        {
          SyntaxService->ReportError (
            "crystalspace.maploader.parse.meshgen",
            child, "Can't find material '%s'!", matname);
          return false;
        }
        float factor = child->GetAttributeValueAsFloat ("factor");
        geom->AddDensityMaterialFactor (mat, factor);
      }
      break;
    case XMLTOKEN_DEFAULTMATERIALFACTOR:
      {
        float factor = child->GetContentsValueAsFloat ();
        geom->SetDefaultDensityMaterialFactor (factor);
      }
      break;
    case XMLTOKEN_RADIUS:
      geom->SetRadius (child->GetContentsValueAsFloat ());
      break;
    case XMLTOKEN_DENSITY:
      geom->SetDensity (child->GetContentsValueAsFloat ());
      break;
    default:
      SyntaxService->ReportBadToken (child);
      return false;
    }
  }
  return true;
}

bool csLoader::LoadMeshGen (iLoaderContext* ldr_context,
                            iDocumentNode* node, iSector* sector)
{
  const char* name = node->GetAttributeValue ("name");
  iMeshGenerator* meshgen = sector->CreateMeshGenerator (name);
  AddToCollection (ldr_context, meshgen->QueryObject ());

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
    case XMLTOKEN_GEOMETRY:
      if (!LoadMeshGenGeometry (ldr_context, child, meshgen))
        return false;
      break;
    case XMLTOKEN_MESHOBJ:
      {
        const char* meshname = child->GetContentsValue ();
	iMeshList* meshes = sector->GetMeshes ();
        iMeshWrapper* mesh = meshes->FindByName (meshname);
        //iMeshWrapper* mesh = ldr_context->FindMeshObject (meshname);
        if (!mesh)
        {
          SyntaxService->ReportError (
            "crystalspace.maploader.parse.meshgen",
            child, "Can't find mesh object '%s' for mesh generator!",
            meshname);
          return false;
        }
        meshgen->AddMesh (mesh);
      }
      break;
    case XMLTOKEN_DENSITYSCALE:
      {
        float mindist = child->GetAttributeValueAsFloat ("mindist");
        float maxdist = child->GetAttributeValueAsFloat ("maxdist");
        float maxfactor = child->GetAttributeValueAsFloat ("maxfactor");
        meshgen->SetDensityScale (mindist, maxdist, maxfactor);
      }
      break;
    case XMLTOKEN_ALPHASCALE:
      {
        float mindist = child->GetAttributeValueAsFloat ("mindist");
        float maxdist = child->GetAttributeValueAsFloat ("maxdist");
        meshgen->SetAlphaScale (mindist, maxdist);
      }
      break;
    case XMLTOKEN_NUMBLOCKS:
      meshgen->SetBlockCount (child->GetContentsValueAsInt ());
      break;
    case XMLTOKEN_CELLDIM:
      meshgen->SetCellCount (child->GetContentsValueAsInt ());
      break;
    case XMLTOKEN_SAMPLEBOX:
      {
        csBox3 b;
        if (!SyntaxService->ParseBox (child, b))
          return false;
        meshgen->SetSampleBox (b);
      }
      break;
    default:
      SyntaxService->ReportBadToken (child);
      return false;
    }
  }
  return true;
}

bool csLoader::LoadAddOn (iLoaderContext* ldr_context,
	iDocumentNode* node, iBase* context, bool is_meta,
	iStreamSource* ssource)
{
  iLoaderPlugin* plug = 0;
  iBinaryLoaderPlugin* binplug = 0;

  const char* plugin_name = node->GetAttributeValue ("plugin");
  if (plugin_name != 0)
  {
    // Short-hand notation: <addon plugin="bla"> ... </addon>
    iDocumentNode* defaults = 0;
    if (!loaded_plugins.FindPlugin (plugin_name, plug, binplug, defaults))
    {
      if (!is_meta)
        ReportWarning (
 	        "crystalspace.maploader.parse.addon",
	        node, "Couldn't find or load addon plugin '%s'!",
		plugin_name);
      return true;
    }
    if (!plug)
    {
      if (!is_meta)
        ReportWarning (
	        "crystalspace.maploader.load.plugin",
                node, "Could not find or load addon plugin!");
      return true;
    }
    if (defaults != 0)
    {
      ReportWarning (
	        "crystalspace.maploader.load.plugin",
                node, "'defaults' section is ignored for addons!");
    }
    csRef<iBase> rc = plug->Parse (node, ssource, ldr_context, context);
    if (!rc) return false;
    
    if (Engine->GetSaveableFlag ())
    {
      csRef<iAddonReference> addon;
      addon.AttachNew (new csAddonReference (plugin_name, 0, rc));
      object_reg->Register (addon);
      AddToCollection (ldr_context, addon->QueryObject ());
    }
    
    return true;
  }
  else
  {
    // Long notation: <addon> <plugin>bla</plugin> <params>...</params> </addon>
    csRef<iDocumentNodeIterator> it = node->GetNodes ();
    while (it->HasNext ())
    {
      csRef<iDocumentNode> child = it->Next ();
      if (child->GetType () != CS_NODE_ELEMENT) continue;
      const char* value = child->GetValue ();
      csStringID id = xmltokens.Request (value);
      switch (id)
      {
        case XMLTOKEN_PARAMS:
	  if (!plug)
	  {
	    if (!is_meta)
              ReportWarning (
	        "crystalspace.maploader.load.plugin",
                child, "Could not find or load plugin!");
	    return true;
	  }
	  else
	  {
	    csRef<iBase> rc = plug->Parse (child, ssource, ldr_context,
	    	context);
      
	    if (!rc) return false;
      
	    if (Engine->GetSaveableFlag ())
	    {
	      csRef<iAddonReference> addon;
	      addon.AttachNew (new csAddonReference (plugin_name, 0, rc));
	      object_reg->Register (addon);
	      AddToCollection (ldr_context, addon->QueryObject ());
	    }
	  }
          break;

        case XMLTOKEN_PARAMSFILE:
	  if (!plug && !binplug)
	  {
	    if (!is_meta)
              ReportWarning (
	        "crystalspace.maploader.load.plugin",
                child, "Could not find or load plugin!");
	    return true;
	  }
	  else
	  {
	    const char* fname = child->GetContentsValue ();
	    if (!fname)
	    {
              SyntaxService->ReportError (
	        "crystalspace.maploader.parse.loadingfile",
	        child, "Specify a VFS filename with 'paramsfile'!");
	      return false;
	    }
            csRef<iFile> buf (VFS->Open (fname, VFS_FILE_READ));
	    if (!buf)
	    {
              SyntaxService->ReportError (
	        "crystalspace.maploader.parse.loadingfile",
	        child, "Error opening file '%s'!", fname);
	      return false;
	    }
	    bool rc;
	    csRef<iBase> ret;
	    if (plug)
	    {
	      ret = LoadStructuredMap (ldr_context,
	    	  plug, buf, 0, fname, ssource);
	      rc = (ret != 0);
	    }
	    else
	    {
	      csRef<iDataBuffer> dbuf = VFS->ReadFile (fname);
	      ret = binplug->Parse (dbuf,
	  	  ssource, ldr_context, 0);
	      rc = (ret != 0);
	    }
      
	    if (!rc)
	      return false;
      
	    if (Engine->GetSaveableFlag ())
	    {
	      csRef<iAddonReference> addon;
	      addon.AttachNew (new csAddonReference (plugin_name,
		  fname, ret));
	      object_reg->Register (addon);
	      AddToCollection (ldr_context, addon->QueryObject ());
	    }
	  }
          break;

        case XMLTOKEN_PLUGIN:
	  {
	    iDocumentNode* defaults = 0;
	    plugin_name = child->GetContentsValue ();
	    if (!loaded_plugins.FindPlugin (plugin_name,
		  plug, binplug, defaults))
	    {
	      if (!is_meta)
	        ReportWarning (
 	          "crystalspace.maploader.parse.addon",
	          child, "Could not find or load plugin '%s'!",
		  child->GetContentsValue ());
	      return true;
	    }
	    if (defaults != 0)
	    {
	      ReportWarning (
	        "crystalspace.maploader.parse.addon",
                child, "'defaults' section is ignored for addons!");
	    }
	  }
          break;
        default:
	  SyntaxService->ReportBadToken (child);
	  return false;
      }
    }
  }
  return true;
}

iRenderLoop* csLoader::ParseRenderLoop (iDocumentNode* node, bool& set)
{
  set = true;
  const char* varname = node->GetAttributeValue ("variable");
  if (varname)
  {
    iSharedVariableList* vl = Engine->GetVariableList ();
    iSharedVariable* var = vl->FindByName (varname);
    csRef<iDocumentNode> default_node;
    csRef<iDocumentNodeIterator> it = node->GetNodes ();
    iRenderLoop* loop = 0;
    while (it->HasNext ())
    {
      csRef<iDocumentNode> child = it->Next ();
      if (child->GetType () != CS_NODE_ELEMENT) continue;
      const char* value = child->GetValue ();
      csStringID id = xmltokens.Request (value);
      switch (id)
      {
        case XMLTOKEN_CONDITION:
	  if (var && var->GetString ())
	  {
	    csString value = child->GetAttributeValue ("value");
	    if (value == var->GetString ())
	    {
	      loop = Engine->GetRenderLoopManager ()->Retrieve (
	      	child->GetContentsValue ());
	    }
	  }
	  break;
        case XMLTOKEN_DEFAULT:
	  default_node = child;
	  break;
        default:
	  SyntaxService->ReportBadToken (child);
          return 0;
      }
    }
    if (!loop && default_node)
    {
      loop = Engine->GetRenderLoopManager ()->Retrieve (
      	default_node->GetContentsValue ());
      if (!loop)
      {
        SyntaxService->Report ("crystalspace.maploader.parse.settings",
		CS_REPORTER_SEVERITY_ERROR,
		node, "No suitable renderloop found!");
        return 0;
      }
    }
    if (!loop)
    {
      loop = Engine->GetCurrentDefaultRenderloop ();
      set = false;
    }

    return loop;
  }
  else
  {
    const char* loopName = node->GetContentsValue ();
    if (loopName)
    {
      iRenderLoop* loop = Engine->GetRenderLoopManager()->Retrieve (loopName);
      if (!loop)
      {
        SyntaxService->Report ("crystalspace.maploader.parse.settings",
		CS_REPORTER_SEVERITY_ERROR,
		node, "Render loop '%s' not found",
		loopName);
        return 0;
      }
      return loop;
    }
    else
    {
      SyntaxService->Report (
	      "crystalspace.maploader.parse.settings",
	      CS_REPORTER_SEVERITY_ERROR,
	      node, "Expected render loop name: %s",
	      loopName);
      return 0;
    }
  }
  return 0;
}

bool csLoader::LoadSettings (iDocumentNode* node)
{
  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_FASTMESH:
	ReportWarning (
	    "crystalspace.maploader.parse.xml",
	    "<fastmesh> is no longer needed.");
	break;
      case XMLTOKEN_CLEARZBUF:
        {
	  bool yesno;
	  if (!SyntaxService->ParseBool (child, yesno, true))
	    return false;
	  Engine->SetClearZBuf (yesno);
        }
        break;
      case XMLTOKEN_CLEARSCREEN:
        {
	  bool yesno;
	  if (!SyntaxService->ParseBool (child, yesno, true))
	    return false;
	  Engine->SetClearScreen (yesno);
        }
        break;
      case XMLTOKEN_LIGHTMAPCELLSIZE:
	ReportWarning (
	        "crystalspace.maploader.parse.settings",
                child, "The 'lightmapcellsize' attribute is deprecated!");
	break;
      case XMLTOKEN_MAXLIGHTMAPSIZE:
	ReportWarning (
	        "crystalspace.maploader.parse.settings",
                child, "The 'maxlightmapsize' attribute is deprecated!");
	break;
      case XMLTOKEN_AMBIENT:
        {
	  csColor c;
	  if (!SyntaxService->ParseColor (child, c))
	    return false;
	  Engine->SetAmbientLight (c);
        }
	break;
      case XMLTOKEN_RENDERLOOP:
	{
	  bool set;
	  iRenderLoop* loop = ParseRenderLoop (child, set);
	  if (!loop) return false;
	  if (set)
	    Engine->SetCurrentDefaultRenderloop (loop);
        }
	break;
      default:
	SyntaxService->ReportBadToken (child);
        return false;
    }
  }

  return true;
}

bool csLoader::ParseStart (iDocumentNode* node, iCameraPosition* campos)
{
  csString start_sector = "room";
  csVector3 pos (0, 0, 0);
  csVector3 up (0, 1, 0);
  csVector3 forward (0, 0, 1);

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_SECTOR:
	start_sector = child->GetContentsValue ();
	break;
      case XMLTOKEN_POSITION:
	if (!SyntaxService->ParseVector (child, pos))
	  return false;
	break;
      case XMLTOKEN_UP:
	if (!SyntaxService->ParseVector (child, up))
	  return false;
	break;
      case XMLTOKEN_FORWARD:
	if (!SyntaxService->ParseVector (child, forward))
	  return false;
	break;
      case XMLTOKEN_FARPLANE:
        {
	  csPlane3 p;
	  p.A () = child->GetAttributeValueAsFloat ("a");
	  p.B () = child->GetAttributeValueAsFloat ("b");
	  p.C () = child->GetAttributeValueAsFloat ("c");
	  p.D () = child->GetAttributeValueAsFloat ("d");
	  campos->SetFarPlane (&p);
        }
	break;
      default:
	SyntaxService->ReportBadToken (child);
	return false;
    }
  }

  campos->Set (start_sector, pos, forward, up);
  return true;
}

iLight* csLoader::ParseStatlight (iLoaderContext* ldr_context,
	iDocumentNode* node)
{
  const char* lightname = node->GetAttributeValue ("name");

  csVector3 pos;

  csVector3 attenvec (0, 0, 0);
  float spotfalloffInner = 1, spotfalloffOuter = 0;
  csLightType type = CS_LIGHT_POINTLIGHT;
  csFlags lightFlags;

  bool use_light_transf = false;
  bool use_light_transf_vector = false;
  csReversibleTransform light_transf;

  float distbright = 1;

  float influenceRadius = 0;
  bool influenceOverride = false;

  csLightAttenuationMode attenuation = CS_ATTN_LINEAR;
  float dist = 0;

  csColor color;
  csColor specular (0, 0, 0);
  bool userSpecular = false;
  csLightDynamicType dyn;
  csRefArray<csShaderVariable> shader_variables;
  struct csHaloDef
  {
    int type;
    union
    {
      struct
      {
        float Intensity;
        float Cross;
      } cross;
      struct
      {
        int Seed;
        int NumSpokes;
        float Roundness;
      } nova;
      struct
      {
        iMaterialWrapper* mat_center;
        iMaterialWrapper* mat_spark1;
        iMaterialWrapper* mat_spark2;
        iMaterialWrapper* mat_spark3;
        iMaterialWrapper* mat_spark4;
        iMaterialWrapper* mat_spark5;
      } flare;
    };
  } halo;

  // This csObject will contain all key-value pairs as children
  csObject Keys;

  memset (&halo, 0, sizeof (halo));

  // New format.
  pos.x = pos.y = pos.z = 0;
  color.red = color.green = color.blue = 1;
  dyn = CS_LIGHT_DYNAMICTYPE_STATIC;

  dist = 1;


  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_RADIUS:
	{
	  dist = child->GetContentsValueAsFloat ();
	  csRef<iDocumentAttribute> attr;
	  if (attr = child->GetAttribute ("brightness"))
	  {
	    distbright = attr->GetValueAsFloat();
	  }
	}
        break;
      case XMLTOKEN_CENTER:
	if (!SyntaxService->ParseVector (child, pos))
	  return 0;
        break;
      case XMLTOKEN_COLOR:
	if (!SyntaxService->ParseColor (child, color))
	  return 0;
        break;
      case XMLTOKEN_SPECULAR:
	if (!SyntaxService->ParseColor (child, specular))
	  return 0;
	userSpecular = true;
        break;
      case XMLTOKEN_DYNAMIC:
        {
	  bool d;
	  if (!SyntaxService->ParseBool (child, d, true))
	    return 0;
	  if (d)
	    dyn = CS_LIGHT_DYNAMICTYPE_PSEUDO;
	  else
	    dyn = CS_LIGHT_DYNAMICTYPE_STATIC;
	}
        break;
      case XMLTOKEN_KEY:
        if (!ParseKey (child, &Keys))
          return false;
        break;
      case XMLTOKEN_HALO:
	{
	  const char* type;
	  csRef<iDocumentNode> typenode = child->GetNode ("type");
	  if (!typenode)
	  {
	    // Default halo, type 'cross' assumed.
	    type = "cross";
	  }
	  else
	  {
	    type = typenode->GetContentsValue ();
	  }

	  if (!strcasecmp (type, "cross"))
	  {
	    halo.type = 1;
            halo.cross.Intensity = 2.0f;
	    halo.cross.Cross = 0.45f;
	    csRef<iDocumentNode> intnode = child->GetNode ("intensity");
	    if (intnode)
	    {
	      halo.cross.Intensity = intnode->GetContentsValueAsFloat ();
	    }
	    csRef<iDocumentNode> crossnode = child->GetNode ("cross");
	    if (crossnode)
	    {
	      halo.cross.Cross = crossnode->GetContentsValueAsFloat ();
	    }
	  }
	  else if (!strcasecmp (type, "nova"))
	  {
            halo.type = 2;
            halo.nova.Seed = 0;
	    halo.nova.NumSpokes = 100;
	    halo.nova.Roundness = 0.5;
	    csRef<iDocumentNode> seednode = child->GetNode ("seed");
	    if (seednode)
	    {
	      halo.nova.Seed = seednode->GetContentsValueAsInt ();
	    }
	    csRef<iDocumentNode> spokesnode = child->GetNode ("numspokes");
	    if (spokesnode)
	    {
	      halo.nova.NumSpokes = spokesnode->GetContentsValueAsInt ();
	    }
	    csRef<iDocumentNode> roundnode = child->GetNode ("roundness");
	    if (roundnode)
	    {
	      halo.nova.Roundness = roundnode->GetContentsValueAsFloat ();
	    }
	  }
	  else if (!strcasecmp (type, "flare"))
	  {
            halo.type = 3;
	    iLoaderContext* lc = ldr_context;
	    csRef<iDocumentNode> matnode;
	    
	    matnode = child->GetNode ("centermaterial");
	    halo.flare.mat_center = matnode ? lc->FindMaterial (
	    	matnode->GetContentsValue ()) : 0;
	    if (!halo.flare.mat_center)
	    {
	      SyntaxService->ReportError (
		  "crystalspace.maploader.parse.light",
    	          child, "Can't find material for flare!");
	      return 0;
	    }
	    matnode = child->GetNode ("spark1material");
	    halo.flare.mat_spark1 = matnode ? lc->FindMaterial (
	    	matnode->GetContentsValue ()) : 0;
	    if (!halo.flare.mat_spark1)
	    {
	      SyntaxService->ReportError (
		  "crystalspace.maploader.parse.light",
    	          child, "Can't find material for flare!");
	      return 0;
	    }
	    matnode = child->GetNode ("spark2material");
	    halo.flare.mat_spark2 = matnode ? lc->FindMaterial (
	    	matnode->GetContentsValue ()) : 0;
	    if (!halo.flare.mat_spark2)
	    {
	      SyntaxService->ReportError (
		  "crystalspace.maploader.parse.light",
    	          child, "Can't find material for flare!");
	      return 0;
	    }
	    matnode = child->GetNode ("spark3material");
	    halo.flare.mat_spark3 = matnode ? lc->FindMaterial (
	    	matnode->GetContentsValue ()) : 0;
	    if (!halo.flare.mat_spark3)
	    {
	      SyntaxService->ReportError (
		  "crystalspace.maploader.parse.light",
    	          child, "Can't find material for flare!");
	      return 0;
	    }
	    matnode = child->GetNode ("spark4material");
	    halo.flare.mat_spark4 = matnode ? lc->FindMaterial (
	    	matnode->GetContentsValue ()) : 0;
	    if (!halo.flare.mat_spark4)
	    {
	      SyntaxService->ReportError (
		  "crystalspace.maploader.parse.light",
    	          child, "Can't find material for flare!");
	      return 0;
	    }
	    matnode = child->GetNode ("spark5material");
	    halo.flare.mat_spark5 = matnode ? lc->FindMaterial (
	    	matnode->GetContentsValue ()) : 0;
	    if (!halo.flare.mat_spark5)
	    {
	      SyntaxService->ReportError (
		  "crystalspace.maploader.parse.light",
    	          child, "Can't find material for flare!");
	      return 0;
	    }
	  }
	  else
	  {
	    SyntaxService->ReportError (
		"crystalspace.maploader.parse.light",
    	        child,
		"Unknown halo type '%s'. Use 'cross', 'nova' or 'flare'!",
		type);
	    return 0;
	  }
        }
        break;
      case XMLTOKEN_ATTENUATION:
	{
	  const char* att = child->GetContentsValue();
	  if (att)
	  {
	    if (!strcasecmp (att, "none"))
	      attenuation = CS_ATTN_NONE;
	    else if (!strcasecmp (att, "linear"))
	      attenuation = CS_ATTN_LINEAR;
	    else if (!strcasecmp (att, "inverse"))
	      attenuation = CS_ATTN_INVERSE;
	    else if (!strcasecmp (att, "realistic"))
	      attenuation = CS_ATTN_REALISTIC;
	    else if (!strcasecmp (att, "clq"))
	      attenuation = CS_ATTN_CLQ;
	    else
	    {
	      SyntaxService->ReportBadToken (child);
	      return 0;
	    }
	  }
	  else
	  {
	    attenuation = CS_ATTN_CLQ;
	  }

	  attenvec.x = child->GetAttributeValueAsFloat ("c");
	  attenvec.y = child->GetAttributeValueAsFloat ("l");
	  attenvec.z = child->GetAttributeValueAsFloat ("q");
	}
	break;
      case XMLTOKEN_INFLUENCERADIUS:
	{
	  influenceRadius = child->GetContentsValueAsFloat();
	  influenceOverride = true;
	}
	break;
      case XMLTOKEN_ATTENUATIONVECTOR:
        {
	  //@@@ should be scrapped in favor of specification via
	  // "attenuation".
          if (!SyntaxService->ParseVector (child, attenvec))
	    return 0;
	  attenuation = CS_ATTN_CLQ;
        }
        break;
      case XMLTOKEN_TYPE:
        {
          const char* t = child->GetContentsValue ();
          if (t)
          {
            if (!strcasecmp (t, "point") || !strcasecmp (t, "pointlight"))
              type = CS_LIGHT_POINTLIGHT;
            else if (!strcasecmp (t, "directional"))
              type = CS_LIGHT_DIRECTIONAL;
            else if (!strcasecmp (t, "spot") || !strcasecmp (t, "spotlight"))
              type = CS_LIGHT_SPOTLIGHT;
            else
            {
              SyntaxService->ReportBadToken (child);
              return 0;
            }
          }
        }
        break;
      case XMLTOKEN_MOVE:
        {
	  use_light_transf = true;
	  csRef<iDocumentNode> matrix_node = child->GetNode ("matrix");
	  if (matrix_node)
	  {
	    csMatrix3 m;
	    if (!SyntaxService->ParseMatrix (matrix_node, m))
	      return false;
            light_transf.SetO2T (m);
	  }
	  csRef<iDocumentNode> vector_node = child->GetNode ("v");
	  if (vector_node)
	  {
	    csVector3 v;
	    if (!SyntaxService->ParseVector (vector_node, v))
	      return false;
	    use_light_transf_vector = true;
            light_transf.SetO2TTranslation (v);
	  }
        }
        break;
      case XMLTOKEN_DIRECTION:
        SyntaxService->ReportError ("crystalspace.maploader.light", child,
          "'direction' is no longer support for lights. Use 'move'!");
        return 0;
      case XMLTOKEN_SPOTLIGHTFALLOFF:
        {
          spotfalloffInner = child->GetAttributeValueAsFloat ("inner");
          spotfalloffInner *= (PI/180);
          spotfalloffInner = cosf(spotfalloffInner);
          spotfalloffOuter = child->GetAttributeValueAsFloat ("outer");
          spotfalloffOuter *= (PI/180);
          spotfalloffOuter = cosf(spotfalloffOuter);
        }
        break;

      case XMLTOKEN_SHADERVAR:
	{
	  const char* varname = child->GetAttributeValue ("name");
	  csRef<csShaderVariable> var;
	  var.AttachNew (new csShaderVariable (stringSetSvName->Request (varname)));
	  if (!SyntaxService->ParseShaderVar (ldr_context, child, *var))
	  {
	    SyntaxService->ReportError (
	      "crystalspace.maploader.load.meshobject", child,
	      "Error loading shader variable '%s' in light '%s'.", 
	      varname, lightname);
	    break;
	  }
	  //svc->AddVariable (var);
	  shader_variables.Push(var);
	}
	break;
      case XMLTOKEN_NOSHADOWS:
	{
	  bool flag;
	  if (!SyntaxService->ParseBool (child, flag, true))
	    return false;
	  lightFlags.SetBool (CS_LIGHT_NOSHADOWS, flag);
	}
	break;
    default:
	SyntaxService->ReportBadToken (child);
	return 0;
    }
  }

  // implicit radius
  if (dist == 0)
  {
    if (color.red > color.green && color.red > color.blue) dist = color.red;
    else if (color.green > color.blue) dist = color.green;
    else dist = color.blue;
  }

  csRef<iLight> l = Engine->CreateLight (lightname, pos,
  	dist, color, dyn);
  AddToCollection (ldr_context, l->QueryObject ());
  l->SetType (type);
  l->GetFlags() = lightFlags;
  l->SetSpotLightFalloff (spotfalloffInner, spotfalloffOuter);

  for (size_t i = 0; i < shader_variables.GetSize (); i++)
  {
     l->GetSVContext()->AddVariable(shader_variables[i]);
  }

  if (userSpecular) l->SetSpecularColor (specular);

  if (use_light_transf)
  {
    if (!use_light_transf_vector)
      light_transf.SetOrigin (pos);
    l->GetMovable ()->SetTransform (light_transf);
    l->GetMovable ()->UpdateMove ();
  }

  switch (halo.type)
  {
    case 1:
      l->CreateCrossHalo (halo.cross.Intensity,
      	halo.cross.Cross);
      break;
    case 2:
      l->CreateNovaHalo (halo.nova.Seed, halo.nova.NumSpokes,
      	halo.nova.Roundness);
      break;
    case 3:
      {
	iMaterialWrapper* ifmc = halo.flare.mat_center;
	iMaterialWrapper* ifm1 = halo.flare.mat_spark1;
	iMaterialWrapper* ifm2 = halo.flare.mat_spark2;
	iMaterialWrapper* ifm3 = halo.flare.mat_spark3;
	iMaterialWrapper* ifm4 = halo.flare.mat_spark4;
	iMaterialWrapper* ifm5 = halo.flare.mat_spark5;
        iFlareHalo* flare = l->CreateFlareHalo ();
	flare->AddComponent (0.0f, 1.2f, 1.2f, CS_FX_ADD, ifmc);
	flare->AddComponent (0.3f, 0.1f, 0.1f, CS_FX_ADD, ifm3);
	flare->AddComponent (0.6f, 0.4f, 0.4f, CS_FX_ADD, ifm4);
	flare->AddComponent (0.8f, 0.05f, 0.05f, CS_FX_ADD, ifm5);
	flare->AddComponent (1.0f, 0.7f, 0.7f, CS_FX_ADD, ifm1);
	flare->AddComponent (1.3f, 0.1f, 0.1f, CS_FX_ADD, ifm3);
	flare->AddComponent (1.5f, 0.3f, 0.3f, CS_FX_ADD, ifm4);
	flare->AddComponent (1.8f, 0.1f, 0.1f, CS_FX_ADD, ifm5);
	flare->AddComponent (2.0f, 0.5f, 0.5f, CS_FX_ADD, ifm2);
	flare->AddComponent (2.1f, 0.15f, 0.15f, CS_FX_ADD, ifm3);
	flare->AddComponent (2.5f, 0.2f, 0.2f, CS_FX_ADD, ifm3);
	flare->AddComponent (2.8f, 0.4f, 0.4f, CS_FX_ADD, ifm4);
	flare->AddComponent (3.0f, 3.0f, 3.0f, CS_FX_ADD, ifm1);
	flare->AddComponent (3.1f, 0.05f, 0.05f, CS_FX_ADD, ifm5);
	flare->AddComponent (3.3f, 0.15f, 0.15f, CS_FX_ADD, ifm2);
      }
      break;
  }
  l->SetAttenuationMode (attenuation);
  if (attenuation == CS_ATTN_CLQ)
  {
    if (attenvec.IsZero())
    {
      //@@TODO:
    }
    else
    {
      l->SetAttenuationConstants (csVector4 (attenvec, 0));
    }
  }

  if (influenceOverride) l->SetCutoffDistance (influenceRadius);
  else l->SetCutoffDistance (dist);

  // Move the key-value pairs from 'Keys' to the light object
  l->QueryObject ()->ObjAddChildren (&Keys);
  Keys.ObjRemoveAll ();

  l->IncRef ();	// To make sure smart pointer doesn't release.
  return l;
}

iMapNode* csLoader::ParseNode (iDocumentNode* node, iSector* sec)
{
  iMapNode* pNode = (iMapNode*)(new csMapNode (
  	node->GetAttributeValue ("name")));
  pNode->SetSector (sec);

  csVector3 pos, v;

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_ADDON:
	SyntaxService->ReportError (
		"crystalspace.maploader.parse.node",
        	child, "'addon' not yet supported in node!");
	return 0;
      case XMLTOKEN_META:
	SyntaxService->ReportError (
		"crystalspace.maploader.parse.node",
        	child, "'meta' not yet supported in node!");
	return 0;
      case XMLTOKEN_KEY:
        if (!ParseKey (child, pNode->QueryObject()))
          return false;
        break;
      case XMLTOKEN_POSITION:
	if (!SyntaxService->ParseVector (child, pos))
	  return 0;
        break;
      case XMLTOKEN_XVECTOR:
	if (!SyntaxService->ParseVector (child, v))
	  return 0;
	pNode->SetXVector (v);
        break;
      case XMLTOKEN_YVECTOR:
	if (!SyntaxService->ParseVector (child, v))
	  return 0;
	pNode->SetYVector (v);
        break;
      case XMLTOKEN_ZVECTOR:
	if (!SyntaxService->ParseVector (child, v))
	  return 0;
	pNode->SetZVector (v);
        break;
      default:
	SyntaxService->ReportBadToken (child);
	return 0;
    }
  }

  pNode->SetPosition (pos);

  return pNode;
}

class csMissingSectorCallback : 
  public scfImplementation1<csMissingSectorCallback, 
                            iPortalCallback>
{
public:
  csRef<iLoaderContext> ldr_context;
  csString sectorname;
  bool autoresolve;

  csMissingSectorCallback (iLoaderContext* ldr_context, const char* sector,
    bool autoresolve) : scfImplementationType (this), ldr_context (ldr_context), 
    sectorname (sector), autoresolve (autoresolve)
  {
  }
  virtual ~csMissingSectorCallback ()
  {
  }
  
  virtual bool Traverse (iPortal* portal, iBase* /*context*/)
  {
    iSector* sector = ldr_context->FindSector (sectorname);
    if (!sector) return false;
    portal->SetSector (sector);
    // For efficiency reasons we deallocate the name here.
    if (!autoresolve)
    {
      sectorname.Empty ();
      portal->RemoveMissingSectorCallback (this);
    }
    return true;
  }
};

bool csLoader::ParsePortal (iLoaderContext* ldr_context,
	iDocumentNode* node, iSector* sourceSector, const char* container_name,
	iMeshWrapper*& container_mesh, iMeshWrapper* parent)
{
  const char* name = node->GetAttributeValue ("name");
  iSector* destSector = 0;
  csPoly3D poly;

  CS::Utility::PortalParameters params;
  csRef<csRefCount> parseState;
  scfString destSectorName;
  params.destSector = &destSectorName;

  // Array of keys we need to parse later.
  csRefArray<iDocumentNode> key_nodes;

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    bool handled;
    if (!SyntaxService->HandlePortalParameter (child, ldr_context,
        parseState, params, handled))
    {
      return false;
    }
    if (!handled)
    {
      const char* value = child->GetValue ();
      csStringID id = xmltokens.Request (value);
      switch (id)
      {
	case XMLTOKEN_KEY:
	  key_nodes.Push (child);
	  break;
        case XMLTOKEN_V:
          {
            csVector3 vec;
            if (!SyntaxService->ParseVector (child, vec))
	      return false;
	    poly.AddVertex (vec);
	  }
          break;
        default:
	  SyntaxService->ReportBadToken (child);
	  return false;
      }
    }
  }

  iPortal* portal;
  // If autoresolve is true we clear the sector since we want the callback
  // to be used.
  if (params.autoresolve)
    destSector = 0;
  else
    destSector = ldr_context->FindSector (destSectorName.GetData ());
  csRef<iMeshWrapper> mesh;
  if (container_mesh)
  {
    mesh = container_mesh;
    csRef<iPortalContainer> pc = 
    	scfQueryInterface<iPortalContainer> (mesh->GetMeshObject ());
    CS_ASSERT (pc != 0);
    portal = pc->CreatePortal (poly.GetVertices (),
    	(int)poly.GetVertexCount ());
    portal->SetSector (destSector);
  }
  else if (parent)
  {
    CS_ASSERT (sourceSector == 0);
    mesh = Engine->CreatePortal (
  	  container_name ? container_name : name,
  	  parent, destSector,
  	  poly.GetVertices (), (int)poly.GetVertexCount (), portal);
    AddToCollection (ldr_context, mesh->QueryObject ());
  }
  else
  {
    mesh = Engine->CreatePortal (
  	  container_name ? container_name : name,
  	  sourceSector, csVector3 (0), destSector,
  	  poly.GetVertices (), (int)poly.GetVertexCount (), portal);
    AddToCollection (ldr_context, mesh->QueryObject ());
  }
  container_mesh = mesh;
  if (name)
    portal->SetName (name);
  if (!destSector)
  {
    // Create a callback to find the sector at runtime when the
    // portal is first used.
    csRef<csMissingSectorCallback> missing_cb;
    missing_cb.AttachNew (new csMissingSectorCallback (
      ldr_context, destSectorName.GetData (), params.autoresolve));
    portal->SetMissingSectorCallback (missing_cb);
  }

  portal->GetFlags ().Set (params.flags);
  if (params.mirror)
  {
    csPlane3 p = poly.ComputePlane ();
    portal->SetWarp (csTransform::GetReflect (p));
  }
  else if (params.warp)
  {
    portal->SetWarp (params.m, params.before, params.after);
  }
  if (params.msv != -1)
  {
    portal->SetMaximumSectorVisit (params.msv);
  }

  size_t i;
  for (i = 0 ; i < key_nodes.GetSize () ; i++)
  {
    if (!ParseKey (key_nodes[i], container_mesh->QueryObject()))
      return false;
  }

  return true;
}

bool csLoader::ParsePortals (iLoaderContext* ldr_context,
	iDocumentNode* node, iSector* sourceSector,
	iMeshWrapper* parent, iStreamSource* ssource)
{
  const char* container_name = node->GetAttributeValue ("name");
  iMeshWrapper* container_mesh = 0;
  csString priority;
  bool staticpos = false;
  bool staticshape = false;
  bool zbufSet = false;
  bool prioSet = false;
  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    bool handled;
    if (!HandleMeshParameter (ldr_context, container_mesh, parent, child, id,
    	handled, priority, true, staticpos, staticshape, zbufSet, prioSet,
	false, ssource))
      return false;
    if (!handled) switch (id)
    {
      case XMLTOKEN_PORTAL:
        if (!ParsePortal (ldr_context, child, sourceSector,
		container_name, container_mesh, parent))
	  return false;
        break;
      default:
	SyntaxService->ReportBadToken (child);
	return false;
    }
  }

  if (!priority.IsEmpty ())
    container_mesh->SetRenderPriority (Engine->GetRenderPriority (priority));
  container_mesh->GetMeshObject ()->GetFlags ().SetBool (CS_MESH_STATICPOS,
  	staticpos);
  container_mesh->GetMeshObject ()->GetFlags ().SetBool (CS_MESH_STATICSHAPE,
  	staticshape);

  return true;
}

iSector* csLoader::ParseSector (iLoaderContext* ldr_context,
	iDocumentNode* node, iStreamSource* ssource)
{
  const char* secname = node->GetAttributeValue ("name");

  bool do_culler = false;
  csString culplugname;

  iSector* sector = ldr_context->FindSector (secname);
  if (sector == 0)
  {
    sector = Engine->CreateSector (secname);
    AddToCollection (ldr_context, sector->QueryObject ());
  }
  
  csRef<iDocumentNode> culler_params;

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_RENDERLOOP:
	{
	  bool set;
	  iRenderLoop* loop = ParseRenderLoop (child, set);
	  if (!loop) return false;
	  if (set)
	    sector->SetRenderLoop (loop);
        }
	break;
      case XMLTOKEN_AMBIENT:
	{
	  csColor c;
	  if (!SyntaxService->ParseColor (child, c))
	    return false;
	  sector->SetDynamicAmbientLight (c);
	}
	break;
      case XMLTOKEN_MESHGEN:
	if (!LoadMeshGen (ldr_context, child, sector))
	  return 0;
	break;
      case XMLTOKEN_ADDON:
	if (!LoadAddOn (ldr_context, child, sector, false, ssource))
	  return 0;
      	break;
      case XMLTOKEN_META:
	if (!LoadAddOn (ldr_context, child, sector, true, ssource))
	  return 0;
      	break;
      case XMLTOKEN_PORTAL:
	{
	  iMeshWrapper* container_mesh = 0;
          if (!ParsePortal (ldr_context, child, sector, 0, container_mesh, 0))
	    return 0;
	}
        break;
      case XMLTOKEN_PORTALS:
        if (!ParsePortals (ldr_context, child, sector, 0, ssource))
	  return 0;
        break;
      case XMLTOKEN_CULLER:
	SyntaxService->ReportError (
	  "crystalspace.maploader.parse.sector",
	  child, "<culler> no longer supported! Convert your level to Dynavis using 'levtool'!");
	csPrintf ("<culler> no longer supported! Convert your level to Dynavis using 'levtool'!");
	return 0;
      case XMLTOKEN_CULLERP:
        {
	  const char* pluginname = child->GetAttributeValue ("plugin");
	  if (pluginname)
	  {
	    // New way to write cullerp.
	    culplugname = pluginname;
	    culler_params = child;	// Remember for later.
	  }
	  else
	  {
	    // Old way.
	    culplugname = child->GetContentsValue ();
	    culler_params = 0;
	  }
	  if (culplugname.IsEmpty ())
	  {
	    SyntaxService->ReportError (
		  "crystalspace.maploader.parse.sector",
	  	  child,
		  "CULLERP expects the name of a visibility culling plugin!");
	    return 0;
	  }
	  else
	  {
            do_culler = true;
	  }
	}
        break;
      case XMLTOKEN_MESHREF:
        {
	  const char* meshname = child->GetAttributeValue ("name");
	  if (!meshname)
	  {
      	    SyntaxService->ReportError (
	      	"crystalspace.maploader.load.meshobject",
		child, "'meshref' requires a name in sector '%s'!",
		secname ? secname : "<noname>");
	    return 0;
	  }
          csRef<iMeshWrapper> mesh = LoadMeshObjectFromFactory (ldr_context,
	      child, ssource);
          if (!mesh)
	  {
	    // Error is already reported.
	    return 0;
	  }
	  mesh->QueryObject ()->SetName (meshname);
          mesh->GetMovable ()->SetSector (sector);
	  mesh->GetMovable ()->UpdateMove ();
	  Engine->AddMeshAndChildren (mesh);
        }
        break;
      case XMLTOKEN_TRIMESH:
        {
	  const char* meshname = child->GetAttributeValue ("name");
	  if (!meshname)
	  {
      	    SyntaxService->ReportError (
	      	"crystalspace.maploader.load.trimesh",
		child, "'trimesh' requires a name in sector '%s'!",
		secname ? secname : "<noname>");
	    return 0;
	  }
	  csRef<iMeshWrapper> mesh = Engine->CreateMeshWrapper (
	  	"crystalspace.mesh.object.null", meshname);
          if (!LoadTriMeshInSector (ldr_context, mesh, child, ssource))
	  {
	    // Error is already reported.
	    return 0;
	  }
	  else
	  {
	    AddToCollection (ldr_context, mesh->QueryObject ());
	  }
          mesh->GetMovable ()->SetSector (sector);
	  mesh->GetMovable ()->UpdateMove ();
        }
        break;
      case XMLTOKEN_MESHOBJ:
        {
	  const char* meshname = child->GetAttributeValue ("name");
	  if (!meshname)
	  {
      	    SyntaxService->ReportError (
	      	"crystalspace.maploader.load.meshobject",
		child, "'meshobj' requires a name in sector '%s'!",
		secname ? secname : "<noname>");
	    return 0;
	  }
	  csRef<iMeshWrapper> mesh = Engine->CreateMeshWrapper (meshname);
          if (!LoadMeshObject (ldr_context, mesh, 0, child, ssource))
	  {
	    // Error is already reported.
	    return 0;
	  }
	  else
	  {
	    AddToCollection (ldr_context, mesh->QueryObject ());
	  }
          mesh->GetMovable ()->SetSector (sector);
	  mesh->GetMovable ()->UpdateMove ();
        }
        break;
      case XMLTOKEN_MESHLIB:
        {
	  const char* meshname = child->GetAttributeValue ("name");
	  if (!meshname)
	  {
      	    SyntaxService->ReportError (
	      	"crystalspace.maploader.load.meshobject",
		child, "'meshlib' requires a name (sector '%s')!",
		secname ? secname : "<noname>");
	    return 0;
	  }
	  iMeshWrapper* mesh = Engine->GetMeshes ()->FindByName (meshname);
	  if (!mesh)
	  {
      	    SyntaxService->ReportError (
	      	"crystalspace.maploader.load.meshobject",
		child,
		"Could not find mesh object '%s' (sector '%s') for MESHLIB!",
		meshname, secname ? secname : "<noname>");
	    return 0;
	  }
          if (!LoadMeshObject (ldr_context, mesh, 0, child, ssource))
	  {
	    // Error is already reported.
	    return 0;
	  }
          mesh->GetMovable ()->GetSectors ()->Add (sector);
	  mesh->GetMovable ()->UpdateMove ();
        }
        break;
      case XMLTOKEN_LIGHT:
        {
	  iLight* sl = ParseStatlight (ldr_context, child);
	  if (!sl) return 0;
          sector->GetLights ()->Add (sl);
	  sl->DecRef ();
	}
        break;
      case XMLTOKEN_NODE:
        {
          iMapNode *n = ParseNode (child, sector);
	  if (n)
	  {
	    n->DecRef ();
	  }
	  else
	  {
	    return 0;
	  }
	}
        break;
      case XMLTOKEN_FOG:
        {
          csFog f;
	  f.color.red = child->GetAttributeValueAsFloat ("red");
	  f.color.green = child->GetAttributeValueAsFloat ("green");
	  f.color.blue = child->GetAttributeValueAsFloat ("blue");
	  f.density = child->GetAttributeValueAsFloat ("density");
	  f.start = child->GetAttributeValueAsFloat ("start");
	  f.end = child->GetAttributeValueAsFloat ("end");
	  csRef<iDocumentAttribute> mode_attr = child->GetAttribute ("mode");
	  if (mode_attr)
	  {
	    const char* str_mode = mode_attr->GetValue();
	    if (!strcmp(str_mode, "linear"))
	      f.mode = CS_FOG_MODE_LINEAR;
	    else if (!strcmp(str_mode, "exp"))
	      f.mode = CS_FOG_MODE_EXP;
	    else if (!strcmp(str_mode, "exp2"))
	      f.mode = CS_FOG_MODE_EXP2;
	    else
	      f.mode = CS_FOG_MODE_NONE;
	  }
	  else
            f.mode = CS_FOG_MODE_CRYSTALSPACE;
          sector->SetFog (f);
        }
        break;
      case XMLTOKEN_KEY:
        if (!ParseKey (child, sector->QueryObject()))
          return 0;
        break;
      default:
	SyntaxService->ReportBadToken (child);
	return 0;
    }
  }
  if (do_culler)
  {
    bool rc = sector->SetVisibilityCullerPlugin (culplugname, culler_params);
    if (!rc)
    {
      SyntaxService->ReportError (
	      	"crystalspace.maploader.load.sector",
		node, "Could not load visibility culler for sector '%s'!",
		secname ? secname : "<noname>");
      return 0;
    }
  }

  return sector;
}

iEngineSequenceManager* csLoader::GetEngineSequenceManager ()
{
  if (!eseqmgr)
  {
    eseqmgr = csQueryRegistryOrLoad<iEngineSequenceManager> (object_reg,
    	"crystalspace.utilities.sequence.engine");
    if (!eseqmgr) return 0;
  }
  return eseqmgr;
}

bool csLoader::ParseVariableList (iLoaderContext* ldr_context,
	iDocumentNode* node)
{
  if (!Engine) return false;

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_VARIABLE:
	if (!ParseSharedVariable (ldr_context, child))
	  return false;
        break;
      default:
        SyntaxService->ReportBadToken (child);
	return false;
    }
  }

  return true;
}

bool csLoader::ParseSharedVariable (iLoaderContext* ldr_context,
	iDocumentNode* node)
{
  csRef<iSharedVariable> v = Engine->GetVariableList()->New ();
  AddToCollection (ldr_context, v->QueryObject ());

  v->SetName (node->GetAttributeValue ("name"));

  if (v->GetName ())
  {
    csRef<iDocumentNode> colornode = node->GetNode ("color");
    csRef<iDocumentNode> vectornode = node->GetNode ("v");
    csRef<iDocumentAttribute> stringattr = node->GetAttribute ("string");
    if (colornode)
    {
      csColor c;
      if (!SyntaxService->ParseColor (colornode, c))
	return false;
      v->SetColor (c);
    }
    else if (vectornode)
    {
      csVector3 vec;
      if (!SyntaxService->ParseVector (vectornode, vec))
	return false;
      v->SetVector (vec);
    }
    else if (stringattr)
    {
      v->SetString (stringattr->GetValue ());
    }
    else
    {
      v->Set (node->GetAttributeValueAsFloat ("value"));
    }
    Engine->GetVariableList ()->Add (v);
  }
  else
  {
    SyntaxService->ReportError ("crystalspace.maploader",
    	node, "Variable tag does not have 'name' attribute.");
    return false;
  }
  return true;
}

//========================================================================
//========================================================================

bool csLoader::LoadShaderExpressions (iLoaderContext* ldr_context,
	iDocumentNode* node)
{
  csRef<iShaderManager> shaderMgr (
    csQueryRegistry<iShaderManager> (csLoader::object_reg));

  if(!shaderMgr)
  {
    ReportNotify ("iShaderManager not found, ignoring shader expressions!");
    return true;
  }

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_SHADEREXPRESSION:
      {
	csRef<iShaderVariableAccessor> ac = SyntaxService->
		ParseShaderVarExpr (child);
        if (!ac) return false;
	const char* name = child->GetAttributeValue ("name");
	csRef<iObject> obj = scfQueryInterface<iObject> (ac);
	if (obj)
	{
	  obj->SetName (name);
	  AddToCollection (ldr_context, obj);
	}
	shaderMgr->RegisterShaderVariableAccessor (name, ac);
      }
      break;
    }
  }
  return true;
}

bool csLoader::ParseShaderList (iLoaderContext* ldr_context,
	iDocumentNode* node)
{
  csRef<iShaderManager> shaderMgr (
    csQueryRegistry<iShaderManager> (csLoader::object_reg));

  if(!shaderMgr)
  {
    ReportNotify ("iShaderManager not found, ignoring shaders!");
    return true;
  }

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_SHADER:
      {
	ParseShader (ldr_context, child, shaderMgr);
      }
      break;
    }
  }
  return true;
}

csRef<iShader> csLoader::LoadShader (const char* filename, bool registerShader)
{
  csRef<iShaderManager> shaderMgr = csQueryRegistry<iShaderManager> (
  	object_reg);
  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);

  csVfsDirectoryChanger dirChanger (vfs);

  csRef<iFile> shaderFile = vfs->Open (filename, VFS_FILE_READ);
  if (!shaderFile)
  {
    ReportError ("crystalspace.maploader",
	"Unable to open shader file '%s'!", filename);
    return 0;
  }

  csRef<iDocumentSystem> docsys = csQueryRegistry<iDocumentSystem> (
  	object_reg);
  if (docsys == 0)
    docsys.AttachNew (new csTinyDocumentSystem ());

  csRef<iDocument> shaderDoc = docsys->CreateDocument ();
  const char* err = shaderDoc->Parse (shaderFile, false);
  if (err != 0)
  {
    ReportError ("crystalspace.maploader",
	"Could not parse shader file '%s': %s",
	filename, err);
    return 0;
  }
  csRef<iDocumentNode> shaderNode = shaderDoc->GetRoot ()->GetNode ("shader");
  if (!shaderNode)
  {
    ReportError ("crystalspace.maploader",
        "Shader file '%s' is not a valid shader XML file!", filename);
    return 0;
  }

  dirChanger.ChangeTo (filename);

  const char* type = shaderNode->GetAttributeValue ("compiler");
  if (type == 0)
    type = shaderNode->GetAttributeValue ("type");
  if (type == 0)
    type = "xmlshader";
  csRef<iShaderCompiler> shcom = shaderMgr->GetCompiler (type);

  csRef<iLoaderContext> ldr_context = csPtr<iLoaderContext> (
	new StdLoaderContext (Engine, 0, true, this, false, 0, KEEP_ALL));

  csRef<iShader> shader = shcom->CompileShader (ldr_context, shaderNode);
  if (shader)
  {
    shader->SetFileName (filename);
    if (registerShader)
      shaderMgr->RegisterShader (shader);
    return shader;
  }
  else 
  {
    return 0;
  }
}

bool csLoader::ParseShader (iLoaderContext* ldr_context,
			    iDocumentNode* node,
			    iShaderManager* shaderMgr)
{
  // @@@ FIXME: unify with csTextSyntaxService::ParseShaderRef()?

  /*csRef<iShader> shader (shaderMgr->CreateShader ());
  //test if we have a childnode named file, if so load from file, else
  //use inline loading
  csRef<iDocumentNode> fileChild = child->GetNode ("file");
  if (fileChild)
  {
    csRef<iDataBuffer> db = VFS->ReadFile (fileChild->GetContentsValue ());
    shader->Load (db);
  }
  else
  {
    shader->Load (child);
  }*/

  csRef<iDocumentNode> shaderNode;
  csRef<iDocumentNode> fileChild = node->GetNode ("file");

  csRef<iVFS> vfs;
  vfs = csQueryRegistry<iVFS> (object_reg);
  csVfsDirectoryChanger dirChanger (vfs);

  if (fileChild)
  {
    csString filename (fileChild->GetContentsValue ());
    csRef<iFile> shaderFile = vfs->Open (filename, VFS_FILE_READ);

    if(!shaderFile)
    {
      ReportWarning ("crystalspace.maploader",
	"Unable to open shader file '%s'!", filename.GetData());
      return false;
    }

    csRef<iDocumentSystem> docsys =
      csQueryRegistry<iDocumentSystem> (object_reg);
    if (docsys == 0)
      docsys.AttachNew (new csTinyDocumentSystem ());
    csRef<iDocument> shaderDoc = docsys->CreateDocument ();
    const char* err = shaderDoc->Parse (shaderFile, false);
    if (err != 0)
    {
      ReportWarning ("crystalspace.maploader",
	"Could not parse shader file '%s': %s",
	filename.GetData(), err);
      return false;
    }
    shaderNode = shaderDoc->GetRoot ()->GetNode ("shader");
    if (!shaderNode)
    {
      SyntaxService->ReportError ("crystalspace.maploader", node,
        "Shader file '%s' is not a valid shader XML file!",
	filename.GetData ());
      return false;
    }

    dirChanger.ChangeTo (filename);
  }
  else
  {
    shaderNode = node->GetNode ("shader");
    if (!shaderNode)
    {
      SyntaxService->ReportError ("crystalspace.maploader", node,
        "'shader' or 'file' node is missing!");
      return false;
    }
  }
  
  csRef<iShader> shader = SyntaxService->ParseShader (ldr_context, shaderNode);
  if (shader.IsValid())
  {
    AddToCollection (ldr_context, shader->QueryObject ());
  }
  return shader.IsValid();
}

void csLoader::CollectAllChildren (iMeshWrapper* meshWrapper,
	csRefArray<iMeshWrapper>& meshesArray)
{  
  size_t lastMeshVisited = 0;
  meshesArray.Push (meshWrapper);
    
  while (lastMeshVisited < meshesArray.GetSize ())
  {
    // Get the children of the current mesh (ie 'mesh').
    const csRef<iSceneNodeArray> ml = 
      meshesArray[lastMeshVisited++]->QuerySceneNode ()->GetChildrenArray ();
    size_t i;
    for (i = 0; i < ml->GetSize(); i++)
    {
      iMeshWrapper* m = ml->Get(i)->QueryMesh ();
      if (m)
        meshesArray.Push (m);
    }
  }

  return;
}

void csLoader::ClosedFlags (iMeshWrapper* mesh)
{
  iObjectModel* objmodel = mesh->GetMeshObject ()->GetObjectModel ();
  csRef<iTriangleMeshIterator> it = objmodel->GetTriangleDataIterator ();
  while (it->HasNext ())
  {
    csStringID id;
    iTriangleMesh* trimesh = it->Next (id);
    if (trimesh) trimesh->GetFlags ().Set (
      CS_TRIMESH_CLOSED | CS_TRIMESH_NOTCLOSED, CS_TRIMESH_CLOSED);
  }
}

void csLoader::ConvexFlags (iMeshWrapper* mesh)
{
  iObjectModel* objmodel = mesh->GetMeshObject ()->GetObjectModel ();
  csRef<iTriangleMeshIterator> it = objmodel->GetTriangleDataIterator ();
  while (it->HasNext ())
  {
    csStringID id;
    iTriangleMesh* trimesh = it->Next (id);
    if (trimesh) trimesh->GetFlags ().Set (
      CS_TRIMESH_CONVEX | CS_TRIMESH_NOTCONVEX, CS_TRIMESH_CONVEX);
  }
}

bool csLoader::ParseKey (iDocumentNode* node, iObject* obj)
{
  csRef<iKeyValuePair> kvp = SyntaxService->ParseKey (node);
  if (!kvp.IsValid())
    return false;

  bool editoronly = node->GetAttributeValueAsBool ("editoronly");
  if (!editoronly || !Engine || Engine->GetSaveableFlag())
    obj->ObjAdd (kvp->QueryObject ());

  return true;
}

}
CS_PLUGIN_NAMESPACE_END(csparser)
