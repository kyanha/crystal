/*
    Copyright (C) 2000-2001 by Jorrit Tyberghein

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
#include "walktest.h"
#include "bot.h"
#include "infmaze.h"
#include "wentity.h"
#include "command.h"
#include "ivaria/view.h"
#include "csutil/scanstr.h"
#include "csutil/csstring.h"
#include "csutil/util.h"
#include "csutil/dataobj.h"
#include "csgeom/math3d.h"
#include "cssys/csendian.h"
#include "cstool/keyval.h"
#include "cstool/collider.h"
#include "cstool/cspixmap.h"
#include "cstool/mdltool.h"
#include "qint.h"
#include "isound/handle.h"
#include "isound/source.h"
#include "isound/listener.h"
#include "isound/renderer.h"
#include "isound/wrapper.h"
#include "imesh/skeleton.h"
#include "imesh/mdlconv.h"
#include "imesh/mdldata.h"
#include "imesh/crossbld.h"
#include "iengine/skelbone.h"
#include "iengine/material.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "iutil/vfs.h"
#include "iengine/motion.h"
#include "iengine/light.h"
#include "ivaria/perfstat.h"
#include "imesh/sprite3d.h"
#include "imesh/thing/thing.h"
#include "imesh/thing/polygon.h"
#include "iengine/statlght.h"
#include "ivaria/reporter.h"
#include "igeom/clip2d.h"


#include "csengine/radiosty.h"
#include "csengine/light.h"
#include "imap/parser.h"

#include "ivaria/mapnode.h"

extern WalkTest* Sys;

char* LookForKeyValue(iObjectIterator* it,const char* key);
double ParseScaleFactor(iObjectIterator* it);


// Use a view's clipping rect to calculate a bounding box
void BoundingBoxForView(iView *view, csBox2 *box)
{
    int vertexCount = view->GetClipper()->GetVertexCount();
    csVector2 *clip = view->GetClipper()->GetClipPoly();
    for (int i = 0; i < vertexCount; i++)
        box->AddBoundingVertex(clip[i]);
};


/// Save recording
void SaveRecording (iVFS* vfs, const char* fName)
{
  csRef<iFile> cf;
  cf = vfs->Open (fName, VFS_FILE_WRITE);
  int32 l = Sys->recording.Length();
  l = convert_endian (l);
  cf->Write ((char*)&l, sizeof (l));
  int i;
  csRecordedCameraFile camint;
  iSector* prev_sector = 0;
  for (i = 0 ; i < Sys->recording.Length () ; i++)
  {
    csRecordedCamera* reccam = (csRecordedCamera*)Sys->recording[i];
    camint.m11 = convert_endian (float2long (reccam->mat.m11));
    camint.m12 = convert_endian (float2long (reccam->mat.m12));
    camint.m13 = convert_endian (float2long (reccam->mat.m13));
    camint.m21 = convert_endian (float2long (reccam->mat.m21));
    camint.m22 = convert_endian (float2long (reccam->mat.m22));
    camint.m23 = convert_endian (float2long (reccam->mat.m23));
    camint.m31 = convert_endian (float2long (reccam->mat.m31));
    camint.m32 = convert_endian (float2long (reccam->mat.m32));
    camint.m33 = convert_endian (float2long (reccam->mat.m33));
    camint.x = convert_endian (float2long (reccam->vec.x));
    camint.y = convert_endian (float2long (reccam->vec.y));
    camint.z = convert_endian (float2long (reccam->vec.z));
    camint.mirror = reccam->mirror;
    cf->Write ((char*)&camint, sizeof (camint));
    unsigned char len;
    if (prev_sector == reccam->sector)
    {
      len = 255;
      cf->Write ((char*)&len, 1);
    }
    else
    {
      len = strlen (reccam->sector->QueryObject ()->GetName ());
      cf->Write ((char*)&len, 1);
      cf->Write (reccam->sector->QueryObject ()->GetName (),
      	1+strlen (reccam->sector->QueryObject ()->GetName ()));
    }
    prev_sector = reccam->sector;
    if (reccam->cmd)
    {
      len = strlen (reccam->cmd);
      cf->Write ((char*)&len, 1);
      cf->Write (reccam->cmd, 1+strlen(reccam->cmd));
    }
    else
    {
      len = 100;
      cf->Write ((char*)&len, 1);
    }
    if (reccam->arg)
    {
      len = strlen (reccam->arg);
      cf->Write ((char*)&len, 1);
      cf->Write (reccam->arg, 1+strlen(reccam->arg));
    }
    else
    {
      len = 100;
      cf->Write ((char*)&len, 1);
    }
  }
}

/// Load recording
void LoadRecording (iVFS* vfs, const char* fName)
{
  csRef<iFile> cf;
  cf = vfs->Open (fName, VFS_FILE_READ);
  if (!cf) return;
  Sys->recording.DeleteAll ();
  Sys->recording.SetLength (0);
  int32 l;
  cf->Read ((char*)&l, sizeof (l));
  l = convert_endian (l);
  csRecordedCameraFile camint;
  iSector* prev_sector = 0;
  int i;
  for (i = 0 ; i < l ; i++)
  {
    csRecordedCamera* reccam = new csRecordedCamera ();
    cf->Read ((char*)&camint, sizeof (camint));
    reccam->mat.m11 = long2float (convert_endian (camint.m11));
    reccam->mat.m12 = long2float (convert_endian (camint.m12));
    reccam->mat.m13 = long2float (convert_endian (camint.m13));
    reccam->mat.m21 = long2float (convert_endian (camint.m21));
    reccam->mat.m22 = long2float (convert_endian (camint.m22));
    reccam->mat.m23 = long2float (convert_endian (camint.m23));
    reccam->mat.m31 = long2float (convert_endian (camint.m31));
    reccam->mat.m32 = long2float (convert_endian (camint.m32));
    reccam->mat.m33 = long2float (convert_endian (camint.m33));
    reccam->vec.x = long2float (convert_endian (camint.x));
    reccam->vec.y = long2float (convert_endian (camint.y));
    reccam->vec.z = long2float (convert_endian (camint.z));
    reccam->mirror = camint.mirror;
    unsigned char len;
    cf->Read ((char*)&len, 1);
    iSector* s;
    if (len == 255)
    {
      s = prev_sector;
    }
    else
    {
      char buf[100];
      cf->Read (buf, 1+len);
      s = Sys->Engine->GetSectors ()->FindByName (buf);
    }
    reccam->sector = s;
    prev_sector = s;

    cf->Read ((char*)&len, 1);
    if (len == 100)
    {
      reccam->cmd = 0;
    }
    else
    {
      reccam->cmd = new char[len+1];
      cf->Read (reccam->cmd, 1+len);
    }
    cf->Read ((char*)&len, 1);
    if (len == 100)
    {
      reccam->arg = 0;
    }
    else
    {
      reccam->arg = new char[len+1];
      cf->Read (reccam->arg, 1+len);
    }
    Sys->recording.Push (reccam);
  }
}

/// Save/load camera functions
void SaveCamera (iVFS* vfs, const char *fName)
{
  if (!Sys->view) return;
  iCamera *c = Sys->view->GetCamera ();
  csOrthoTransform& camtrans = c->GetTransform ();
  if (!c) return;
  const csMatrix3& m_o2t = camtrans.GetO2T ();
  const csVector3& v_o2t = camtrans.GetOrigin ();
  csString s;
  s << v_o2t.x << ' ' << v_o2t.y << ' ' << v_o2t.z << '\n'
    << m_o2t.m11 << ' ' << m_o2t.m12 << ' ' << m_o2t.m13 << '\n'
    << m_o2t.m21 << ' ' << m_o2t.m22 << ' ' << m_o2t.m23 << '\n'
    << m_o2t.m31 << ' ' << m_o2t.m32 << ' ' << m_o2t.m33 << '\n'
    << '"' << c->GetSector ()->QueryObject ()->GetName () << "\"\n"
    << c->IsMirrored () << '\n';
  vfs->WriteFile (fName, s.GetData(), s.Length());
}

bool LoadCamera (iVFS* vfs, const char *fName)
{
  bool ok = true;
#define IFFAIL(x) if (ok && !(ok = (x)))
  IFFAIL (vfs->Exists (fName))
    Sys->Report (CS_REPORTER_SEVERITY_ERROR,
		 "Could not open camera file '%s'!", fName);
  csRef<iDataBuffer> data;
  IFFAIL ((data = vfs->ReadFile(fName)) != 0)
    Sys->Report (CS_REPORTER_SEVERITY_ERROR,
		 "Could not read camera file '%s'!", fName);
  csMatrix3 m;
  csVector3 v;
  int imirror = false;
  char* sector_name = 0;
  if (ok)
    sector_name = new char [data->GetSize ()];

  IFFAIL (14 == csScanStr (**data,
			   "%f %f %f\n"
			   "%f %f %f\n"
			   "%f %f %f\n"
			   "%f %f %f\n"
			   "%S\n"
			   "%d",
			   &v.x, &v.y, &v.z,
			   &m.m11, &m.m12, &m.m13,
			   &m.m21, &m.m22, &m.m23,
			   &m.m31, &m.m32, &m.m33,
			   sector_name,
			   &imirror))
    Sys->Report (CS_REPORTER_SEVERITY_ERROR,
		 "Wrong format for camera file '%s'", fName);
  iSector* s = 0;
  IFFAIL ((s = Sys->Engine->GetSectors ()->FindByName (sector_name)) != 0)
    Sys->Report (CS_REPORTER_SEVERITY_ERROR,
		 "Sector `%s' in coordinate file does not "
		 "exist in this map!", sector_name);
  if (ok)
  {
    iCamera *c = Sys->view->GetCamera ();
    c->SetSector (s);
    c->SetMirrored ((bool)imirror);
    c->GetTransform ().SetO2T (m);
    c->GetTransform ().SetOrigin (v);
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Camera loaded");
  }
  delete[] sector_name;
  return true;
#undef IFFAIL
}

void move_mesh (iMeshWrapper* sprite, iSector* where, csVector3 const& pos)
{
  sprite->GetMovable ()->SetPosition (pos);
  sprite->GetMovable ()->SetSector (where);
  sprite->GetMovable ()->UpdateMove ();
}

// Load a mesh object factory from a general format (3DS, MD2, ...)
// This creates a mesh object factory which you can then add using add_meshobj ().
void load_meshobj (char *filename, char *templatename, char* txtname)
{
  // First check if the texture exists.
  if (!Sys->view->GetEngine ()->GetMaterialList ()->FindByName (txtname))
  {
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
    	"Can't find material '%s' in memory!", txtname);
    return;
  }

  // read in the model file
  csRef<iDataBuffer> buf (Sys->myVFS->ReadFile (filename));
  if (!buf)
  {
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
  	"There was an error reading the data!");
    return;
  }

  csRef<iModelData> Model (Sys->ModelConverter->Load (buf->GetUint8 (),
  	buf->GetSize ()));
  if (!Model)
  {
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
  	"There was an error reading the data!");
    return;
  }

  csModelDataTools::SplitObjectsByMaterial (Model);
  csModelDataTools::MergeObjects (Model, false);
  iMeshFactoryWrapper *wrap =
    Sys->CrossBuilder->BuildSpriteFactoryHierarchy (Model, Sys->Engine,
    Sys->Engine->GetMaterialList ()->FindByName (txtname));
  wrap->QueryObject ()->SetName (templatename);
}


void GenerateThing (iObjectRegistry* object_reg,
	iObjectIterator * it, iMaterialWrapper* mat,
	char* spriteName, iSector* sector, csVector3 position, float size)
{
  csRef<iPluginManager> plugin_mgr = CS_QUERY_REGISTRY (object_reg,
  	iPluginManager);
  csRef<iMeshObjectType> thingType = CS_QUERY_PLUGIN_CLASS (
      plugin_mgr, "crystalspace.mesh.object.thing",
      iMeshObjectType);

  it->Reset();
  while(it->HasNext())
  {
    csRef<iModelDataObject> mdo (SCF_QUERY_INTERFACE(it->Next(),
		                    iModelDataObject));
    if(!mdo)
    {
      continue;
    }
    csRef<iMeshObjectFactory> thingFactory (thingType->NewFactory());
    csRef<iThingState> thingState =
    	SCF_QUERY_INTERFACE(thingFactory, iThingState);
    csRef<iThingFactoryState> thingFactoryState = thingState->GetFactory ();
    if(!thingFactoryState)
    {
      Sys->Report(CS_REPORTER_SEVERITY_NOTIFY,
		  "Can't get iThingState Interface!");
      return;
    }
    if(!Sys->CrossBuilder->BuildThing(mdo,thingFactoryState,mat))
    {
      Sys->Report(CS_REPORTER_SEVERITY_NOTIFY,"Can't Build Thing!");
      return;
    }
    //Now we have loaded the thing, let's do the mesh
    csRef<iMeshObject> thingObj (
    	SCF_QUERY_INTERFACE(thingFactory,iMeshObject));
    if(!thingObj)
    {
      Sys->Report(CS_REPORTER_SEVERITY_NOTIFY,"Can't Get iMeshObj Interface!");
      return;
    }
    csRef<iMeshWrapper> thingWrapper (Sys->Engine->CreateMeshWrapper(
    	thingObj,spriteName));

    if(!thingWrapper)
    {
      Sys->Report(CS_REPORTER_SEVERITY_NOTIFY,"Can't Get iMeshObj Interface!");
      return;
    }
    csMatrix3 m; m.Identity(); m = m*(1./size);
    csReversibleTransform t = csReversibleTransform(m,csVector3(0,0,0));
    csReversibleTransform t2;
    t2.SetOrigin(position);
    thingWrapper->HardTransform(t);
    thingWrapper->HardTransform(t2);
    thingWrapper->GetMovable()->SetSector(sector);
    thingWrapper->GetMovable()->UpdateMove();
    thingWrapper->DeferUpdateLighting (CS_NLIGHT_STATIC|CS_NLIGHT_DYNAMIC, 10);
  }
}

void load_thing (iObjectRegistry* object_reg,
	iSector* sector, csVector3 position,
	iObjectIterator* it, iEngine* Engine)
{
  /*
   * First we check for mesh material:
   * It must be in memory, since we've registered needed materials before
   * object creation.
   * I'll use only one material til i know how to register more materials
   */

  char* defaultMat = LookForKeyValue(it,"factorymaterial");
  char* fileName = LookForKeyValue(it,"factoryfile");
  char* spriteName = LookForKeyValue(it,"cs_name");
  iMaterialWrapper *mat;

  mat = Engine->GetMaterialList()->FindByName(defaultMat);
  if(!mat)
  {
    Sys->Report(CS_REPORTER_SEVERITY_NOTIFY,"Can't find material %s \
	            for thing creation in memory!!",defaultMat);
    return;
  }
  csRef<iDataBuffer> buffer (Sys->myVFS->ReadFile(fileName));
  if(!buffer)
  {
    Sys->Report(CS_REPORTER_SEVERITY_NOTIFY,"There was an error loading \
	            model %s for thing creation!", fileName);
    return;
  }

  csRef<iModelData> Model (Sys->ModelConverter->Load (buffer->GetUint8(),
  	buffer->GetSize()));
  if(!Model)
  {
    Sys->Report(CS_REPORTER_SEVERITY_NOTIFY,
    	"There was an error reading the data!");
    return;
  }

  csRef<iObjectIterator> it2 (Model->QueryObject()->GetIterator());
  GenerateThing(object_reg, it2,mat,spriteName,sector,
  	position, ParseScaleFactor(it));
}


iMeshWrapper* GenerateSprite(const char* tname, char* sname, iSector* where,
	csVector3 const& pos)
{
  iMeshFactoryWrapper* tmpl = Sys->Engine->GetMeshFactories ()
  	->FindByName (tname);
  if (!tmpl)
  {
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
    	"Unknown mesh factory '%s'!", tname);
    return 0;
  }
  csRef<iMeshWrapper> spr (Sys->Engine->CreateMeshWrapper (tmpl, sname));
  spr->GetMovable()->SetPosition(where, pos);
  spr->GetMovable ()->UpdateMove ();
  spr->DeferUpdateLighting (CS_NLIGHT_STATIC|CS_NLIGHT_DYNAMIC, 10);
  return spr;
}


iMeshWrapper* add_meshobj (const char* tname, char* sname, iSector* where,
	csVector3 const& pos, float size)
{
  iMeshFactoryWrapper* tmpl = Sys->Engine->GetMeshFactories ()
  	->FindByName (tname);
  if (!tmpl)
  {
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
    	"Unknown mesh factory '%s'!", tname);
    return 0;
  }
  csRef<iMeshWrapper> spr (
  	Sys->Engine->CreateMeshWrapper (tmpl, sname, where, pos));
  csMatrix3 m; m.Identity (); m = m * size;
  spr->GetMovable ()->SetTransform (m);
  spr->GetMovable ()->UpdateMove ();

  spr->DeferUpdateLighting (CS_NLIGHT_STATIC|CS_NLIGHT_DYNAMIC, 10);
  return spr;
}


void list_meshes (void)
{
  int num_meshes;
  const char* mesh_name;
  iMeshWrapper* mesh;

  iMeshList* meshes = Sys->Engine->GetMeshes ();
  num_meshes = meshes->GetCount ();

  int i;
  for(i = 0; i < num_meshes; i++)
  {
    mesh = meshes->Get (i);
    mesh_name = mesh->QueryObject ()->GetName();

    if (mesh_name)
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "%s", mesh_name);
    else
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "A mesh with no name.");
  }
  Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "There are:%d meshes",
	       Sys->Engine->GetMeshes ()->GetCount ());
}

//===========================================================================

void SetConfigOption (iBase* plugin, const char* optName, const char* optValue)
{
  csRef<iConfig> config (SCF_QUERY_INTERFACE (plugin, iConfig));
  if (!config)
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
    	"No config interface for this plugin.");
  else
  {
    int i;
    for (i = 0 ; ; i++)
    {
      csOptionDescription odesc;
      if (!config->GetOptionDescription (i, &odesc)) break;
      if (strcmp (odesc.name, optName) == 0)
      {
	Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Set option %s to %s",
		odesc.name, optValue);
	csVariant var;
	switch (odesc.type)
	{
	  case CSVAR_LONG:
	  {
	    long l;
	    csScanStr (optValue, "%ld", &l);
	    var.SetLong (l);
	    break;
	  }
	  case CSVAR_BOOL:
	  {
	    bool b;
	    csScanStr (optValue, "%b", &b);
	    var.SetBool (b);
	    break;
	  }
	  case CSVAR_FLOAT:
	  {
	    float f;
	    csScanStr (optValue, "%f", &f);
	    var.SetFloat (f);
	    break;
	  }
	  case CSVAR_STRING: var.SetString (optValue); break;
	  default: break;
	}
	config->SetOption (i, &var);
	return;
      }
    }
  }
}

void SetConfigOption (iBase* plugin, const char* optName, csVariant& optValue)
{
  csRef<iConfig> config (SCF_QUERY_INTERFACE (plugin, iConfig));
  if (!config)
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
    	"No config interface for this plugin.");
  else
  {
    int i;
    for (i = 0 ; ; i++)
    {
      csOptionDescription odesc;
      if (!config->GetOptionDescription (i, &odesc)) break;
      if (strcmp (odesc.name, optName) == 0)
      {
	config->SetOption (i, &optValue);
	return;
      }
    }
  }
}

bool GetConfigOption (iBase* plugin, const char* optName, csVariant& optValue)
{
  csRef<iConfig> config (SCF_QUERY_INTERFACE (plugin, iConfig));
  if (!config)
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
    	"No config interface for this plugin.");
  else
  {
    int i;
    for (i = 0 ; ; i++)
    {
      csOptionDescription odesc;
      if (!config->GetOptionDescription (i, &odesc)) break;
      if (strcmp (odesc.name, optName) == 0)
      {
	config->GetOption (i, &optValue);
	return true;
      }
    }
  }
  return false;
}



/// Looks for a key and returns its value (VERY INNEFICIENT!)

char* LookForKeyValue (iObjectIterator* it,const char* key)
{
  char* value = new char[1024];
  it->Reset();
  while (it->HasNext())
  {
    csRef<iKeyValuePair> kp = SCF_QUERY_INTERFACE(it->Next(),iKeyValuePair);
    if(!kp)
    {
      continue;
    }
    if(!strcmp(key,kp->GetKey()))
    {
      strcpy(value ,kp->GetValue());
      return value;
    }
  }
  delete[] value;
  return 0;
}

/**
 * Contructs a models in format of SEED_MESH_OBJ
 * Basically is a SPRITE3D contruction.
 * If factory is already loaded it uses that, if doesn't creates a new one and
 * register both.
 * Same for materials and textures.
 * Finally it scales and moves the 3D sprite, not the factory
 */
/*
 Schematical explanation how how this works:

  First of all, let's see if the factory specified in this node is already created
    If yes, let's see if materials needed by sprite are already registered
       If yes create the sprite
	   If not register all the materials in node list that aren't already registered
	If not register all the materials in node list that aren't already registered
	   Create the factory
	   Create the sprite
*/


/**
 * Returns the Scale Factor value of the scalefactor key
 * Basically transforms the string to a csVector3
 */
double ParseScaleFactor(iObjectIterator* it)
{
  double sf;
  char* scaleValue = LookForKeyValue(it,"scalefactor");

  sf = atof(scaleValue);
  return sf;
};


/// Extracts the material name of the defmaterial key value
char* LookForMaterialName(const char* value)
{
  /*
   * value has the format mat_name, text_name. We must extract the material
   * name.
   */

  char* matName = new char[strlen(value)];
  int i;

  for (i = 0; i < (int)strlen(value) ; i++)
  {
    if(value[i]!= ',') matName[i] = value[i];
    else
    {
      matName[i] = 0;
      return matName;
    }
  }
  return matName;
}

/// Extracts the texture file name of the defmaterial key value
char* LookForTextureFileName(const char* value)
{
  // value has the format mat_name, text_name. We must extract
  // the texture file name.
  int i = 0;
  int j;
  char* textFileName = new char[strlen(value)];
  while (i < (int)strlen(value) && value[i] != ',') i++;
  j = i+1;
  int spaces = 0;
  while (j < (int)strlen(value))
  {
    if (value[j] ==' ') spaces++;
    else textFileName[j-spaces-i-1] = value[j];
    j++;
  }
  textFileName[j-spaces-i-1] = 0;
  return textFileName;
}


void RegisterMaterials(iObjectIterator* it,iEngine* Engine,
					   iGraphics3D* /*MyG3D*/, iLoader* loader,
					   iObjectRegistry* /*objReg*/)
{
  iMaterialList* matList = Engine->GetMaterialList();
  //used to check if a material is already registered
  char * matName;
  char * textFileName;
  csRef<iKeyValuePair> kp;
  it->Reset();

  while(it->HasNext())
  {
    kp = SCF_QUERY_INTERFACE(it->Next(),iKeyValuePair);
    if(!kp)
    {
      continue;
    }
    if(strcmp("defmaterial",kp->GetKey()))
    {
      continue;
    }
    matName = LookForMaterialName(kp->GetValue());

    //Let's see if Material is registered
    if (matList->FindByName(matName))
    {
      //Is registered, let's go to another material
      delete [] matName;
    }
    else
    {
      //Is not registered. We have to do it.
      textFileName = LookForTextureFileName(kp->GetValue());
      if(!loader->LoadTexture(matName,textFileName))
      {
        printf("Error loading %s texture!!",textFileName);
      }
      //Material registered, let's go to another one
      delete [] matName;
      delete [] textFileName;
    }
  }
}

/**
 * Build the factory for the sprite specified in a SEED_MESH_OBJ
 */
void BuildFactory(iObjectIterator* it, char* factoryName, iEngine* Engine)
{
  char* modelFilename = LookForKeyValue(it,"factoryfile");
  char* materialName = LookForKeyValue(it,"factorymaterial");
  double scaleFactor = ParseScaleFactor(it);

  //We had the material needed by the factory and the model file

  load_meshobj(modelFilename,factoryName,materialName);

  csRef<iSprite3DFactoryState> state (SCF_QUERY_INTERFACE(
	  Engine->GetMeshFactories()->FindByName(factoryName)\
	  ->GetMeshObjectFactory(),
	  iSprite3DFactoryState));

  iMeshFactoryWrapper* factWrapper =
	                   Engine->GetMeshFactories()->FindByName(factoryName);

  csMatrix3 m; m.Identity(); m = m*(1.0/scaleFactor);
  csReversibleTransform t = csReversibleTransform(m,csVector3(0,0,0));
  factWrapper->HardTransform(t);
}

/**
 * Builds the sprite3D
 */
void BuildSprite(iSector * sector, iObjectIterator* it, csVector3 position)
{
  /*
   * Basically we will call add_mesh
   * We need the sprite name, the factory name and the sprite scale factor
   */
  char* sprName		= LookForKeyValue(it,"cs_name");
  char* factName		= LookForKeyValue(it,"factory");
  //double scaleFactor	= ParseScaleFactor(it);

  iMeshWrapper* sprite = GenerateSprite(factName,sprName,sector,position);

  csRef<iSprite3DState> state (SCF_QUERY_INTERFACE(sprite->GetMeshObject(),
                          iSprite3DState));
  state->SetAction("default");
};



void BuildThing(iObjectRegistry* object_reg,
	iSector* sector, csVector3 position,iObjectIterator* it,
	iEngine* Engine)
{
  load_thing(object_reg, sector,position,it,Engine);
}

void BuildObject(iObjectRegistry* object_reg, iSector * sector,
	iObjectIterator* it, iEngine* Engine,
	csVector3 position, iGraphics3D* MyG3D, iLoader* loader,
	iObjectRegistry* objReg)
{
  char* factoryName;
  if(strcmp(LookForKeyValue(it,"classname"),"SEED_MESH_OBJ")) return;
  //Now we know this objects iterator belongs to a SEED_MESH_OBJECT
  //Proceeding to contruct the object

  RegisterMaterials(it,Engine,MyG3D,loader,objReg);
  if (!strcmp(LookForKeyValue(it,"staticflag"),"static"))
  {
    BuildThing(object_reg, sector,position,it,Engine);
  }
  else
  {
    factoryName = LookForKeyValue(it,"factory");
    if(!Engine->GetMeshFactories()->FindByName(factoryName))
	  BuildFactory(it, factoryName, Engine);

    BuildSprite(sector, it, position);
  }
}

//===========================================================================

void WalkTest::ParseKeyNodes(iObject* src)
{
  csRef<iObjectIterator> it (src->GetIterator());
  csRef<iSector> sector (SCF_QUERY_INTERFACE(src,iSector));

  while(it->HasNext())
  {
    iObject* node_obj = it->Next ();
    csRef<iMapNode> node (SCF_QUERY_INTERFACE(node_obj, iMapNode));
    if(!node)
    {
      continue;
    }
    csRef<iObjectIterator> it2 (node_obj->GetIterator());

    BuildObject(object_reg, sector,it2, Engine, node->GetPosition(), myG3D,
		LevelLoader, object_reg);
  }
}

void WalkTest::ParseKeyCmds (iObject* src)
{
  csRef<iObjectIterator> it (src->GetIterator ());
  while (it->HasNext ())
  {
    csRef<iKeyValuePair> kp (
    	SCF_QUERY_INTERFACE (it->Next (), iKeyValuePair));
    if (!kp)
    {
      continue;
    }
    if (!strcmp (kp->GetKey (), "cmd_AnimateSky"))
    {
      csRef<iSector> Sector (SCF_QUERY_INTERFACE (src, iSector));
      if (Sector)
      {
        char name[100], rot[100];
        csScanStr (kp->GetValue (), "%s,%s,%f", name, rot, &anim_sky_speed);
        if (rot[0] == 'x') anim_sky_rot = 0;
        else if (rot[0] == 'y') anim_sky_rot = 1;
        else anim_sky_rot = 2;
        anim_sky = Sector->GetMeshes ()->FindByName (name);
      }
    }
    else if (!strcmp (kp->GetKey (), "cmd_AnimateDirLight"))
    {
      csRef<iMeshWrapper> wrap (SCF_QUERY_INTERFACE (src, iMeshWrapper));
      if (wrap)
        anim_dirlight = wrap;	// @@@ anim_dirlight should be csRef
    }
    else if (!strcmp (kp->GetKey (), "entity_WavePortal"))
    {
      csRef<iMeshWrapper> wrap (SCF_QUERY_INTERFACE (src, iMeshWrapper));
      if (wrap)
      {
        csRef<iThingState> thing_state (SCF_QUERY_INTERFACE (
		wrap->GetMeshObject (), iThingState));
	if (thing_state)
	{
	  iThingFactoryState* thing = thing_state->GetFactory ();
	  char polyname[255];
	  int xyz;
	  float max_angle, speed;
	  csScanStr (kp->GetValue (), "%s,%d,%f,%f",
	  	polyname, &xyz, &max_angle, &speed);

	  iPolygon3DStatic* p = thing->GetPolygon (polyname);
	  if (!p)
	  {
	    Sys->Report (CS_REPORTER_SEVERITY_WARNING,
	    	"Cannot find a polygon named '%s'!", polyname);
	  }
	  else
	  {
	    iPortal* po = p->GetPortal ();
	    if (!po)
	    {
	      Sys->Report (CS_REPORTER_SEVERITY_WARNING,
	    	"This polygon '%s' has no portal!", polyname);
	    }
	    else
	    {
	      csAnimatedPortal* anportal = new csAnimatedPortal (po,
	      	xyz, max_angle, speed);
	      src->ObjAdd (anportal);
	      Sys->busy_entities.Push (anportal);
              anportal->DecRef ();
	    }
	  }
	}
	else
	{
	  Sys->Report (CS_REPORTER_SEVERITY_WARNING,
	  	"This mesh object is not a thing!");
	}
      }
    }
    else
    {
      // Unknown command. Ignore for the moment.
    }
  }

  csRef<iMeshWrapper> mesh (SCF_QUERY_INTERFACE (src, iMeshWrapper));
  if (mesh)
  {
    int k;
    iMeshList* ml = mesh->GetChildren ();
    for (k = 0 ; k < ml->GetCount () ; k++)
    {
      iMeshWrapper* spr = ml->Get (k);
      ParseKeyCmds (spr->QueryObject ());
    }
  }

}

void WalkTest::ParseKeyCmds ()
{
  int i;
  for (i = 0 ; i < Engine->GetSectors ()->GetCount () ; i++)
  {
    iSector* sector = Engine->GetSectors ()->Get (i);
    ParseKeyCmds (sector->QueryObject ());
    ParseKeyNodes(sector->QueryObject());

    int j;
    iMeshList* ml = sector->GetMeshes ();
    for (j = 0 ; j < ml->GetCount () ; j++)
    {
      iMeshWrapper* sprite = ml->Get (j);
      ParseKeyCmds (sprite->QueryObject ());
    }
    for (j = 0 ; j < sector->GetLights ()->GetCount () ; j++)
    {
      iLight* l = sector->GetLights ()->Get (j);
      ParseKeyCmds (l->QueryObject ());
    }
  }
}

void WalkTest::ActivateObject (iObject* src)
{
  csRef<iObjectIterator> it (src->GetIterator ());
  while (it->HasNext ())
  {
    csRef<csWalkEntity> wentity (SCF_QUERY_INTERFACE (it->Next (),
    	csWalkEntity));
    if (wentity)
      wentity->Activate ();
  }
}

//===========================================================================

float safe_atof (const char* arg)
{
  if (arg) return atof (arg);
  else return 1;
}

//--//--//--//--//--//--//--//--//--//--//-- Handle our additional commands --//

// Command recording
#define RECORD_ARGS(CMD, ARG) \
if (Sys->cfg_recording >= 0)                        \
{                                                   \
  Sys->recorded_cmd = new char[strlen(CMD)+1];      \
  strcpy (Sys->recorded_cmd, CMD);                  \
  if (ARG)                                          \
  {                                                 \
    Sys->recorded_arg = new char[strlen(ARG)+1];    \
    strcpy (Sys->recorded_arg, ARG);                \
  }                                                 \
}
#define RECORD_CMD(CMD) \
if (Sys->cfg_recording >= 0)                        \
{                                                   \
  Sys->recorded_cmd = new char[strlen(CMD)+1];      \
  strcpy (Sys->recorded_cmd, CMD);                  \
}

bool CommandHandler (const char *cmd, const char *arg)
{
  if (!strcasecmp (cmd, "help"))
  {
    csCommandProcessor::perform (cmd, arg);
#   undef CONPRI
#   define CONPRI(m) Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, m);
    CONPRI("-*- Additional commands -*-");
    CONPRI("Visibility:");
    CONPRI("  db_frustum farplane");
    CONPRI("Lights:");
    CONPRI("  addlight dellight dellights addstlight delstlight");
    CONPRI("  clrlights setlight");
    CONPRI("Views:");
    CONPRI("  split_view unsplit_view toggle_view");
    CONPRI("Movement:");
    CONPRI("  step_forward step_backward strafe_left strafe_right");
    CONPRI("  look_up look_down rotate_left rotate_right jump move3d");
    CONPRI("  i_forward i_backward i_left i_right i_up i_down i_rotleftc");
    CONPRI("  i_rotleftw i_rotrightc i_rotrightw i_rotleftx i_rotleftz");
    CONPRI("  i_rotrightx i_rotrightz do_gravity colldet freelook");
    CONPRI("Statistics:");
    CONPRI("  stats perftest coordshow");
    CONPRI("Special effects:");
    CONPRI("  addbot delbot addskel addghost fire explosion spiral frain rain");
    CONPRI("  snow fountain flame portal fs_inter fs_fadeout fs_fadecol");
    CONPRI("  fs_fadetxt fs_red fs_green fs_blue fs_whiteout fs_shadevert");
    CONPRI("Debugging:");
    CONPRI("  hi zbuf debug0 debug1 debug2 palette bugplug");
    CONPRI("  db_boxshow db_boxcam1 db_boxcam2 db_boxsize1 db_boxsize2");
    CONPRI("  db_radstep db_radhi db_radtodo");
    CONPRI("Meshes:");
    CONPRI("  loadmesh addmesh delmesh listmeshes");
    CONPRI("  listactions setaction setmotion");
    CONPRI("Various:");
    CONPRI("  coordsave coordload bind p_alpha s_fog");
    CONPRI("  snd_play snd_volume record play clrrec saverec");
    CONPRI("  loadrec action plugins conflist confset do_logo");
    CONPRI("  varlist var setvar setvarv setvarc");

#   undef CONPRI
  }
  else if (!strcasecmp (cmd, "coordsave"))
  {
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
    	"Saved camera location in /temp/walktest.cam");
    SaveCamera (Sys->myVFS, "/temp/walktest.cam");
  }
  else if (!strcasecmp (cmd, "coordload"))
  {
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
    	"Loading camera location from /temp/walktest.cam");
    LoadCamera (Sys->myVFS, "/temp/walktest.cam");
  }
  else if (!strcasecmp (cmd, "plugins"))
  {
    int i = 0;
    csRef<iPluginIterator> it = Sys->plugin_mgr->GetPlugins ();
    while (it->HasNext ())
    {
      iBase* plugin = it->Next ();
      csRef<iFactory> fact (SCF_QUERY_INTERFACE (plugin, iFactory));
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"%d: %s", i, fact->QueryDescription ());
      i++;
    }
  }
  else if (!strcasecmp (cmd, "setvarc"))
  {
    if (!arg)
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	  	"Please give a name of a variable and a color.");
      return false;
    }
    char name[256];
    csColor c;
    csScanStr (arg, "%s,%f,%f,%f", name, &c.red, &c.green, &c.blue);

    iSharedVariableList* vl = Sys->view->GetEngine ()->GetVariableList ();
    iSharedVariable* v = vl->FindByName (name);
    if (!v)
    {
      csRef<iSharedVariable> nv = vl->New ();
      v = nv;
      v->SetName (name);
      vl->Add (nv);
    }
    v->SetColor (c);
  }
  else if (!strcasecmp (cmd, "setvarv"))
  {
    if (!arg)
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	  	"Please give a name of a variable and a vector.");
      return false;
    }
    char name[256];
    csVector3 w;
    csScanStr (arg, "%s,%f,%f,%f", name, &w.x, &w.y, &w.z);

    iSharedVariableList* vl = Sys->view->GetEngine ()->GetVariableList ();
    iSharedVariable* v = vl->FindByName (name);
    if (!v)
    {
      csRef<iSharedVariable> nv = vl->New ();
      v = nv;
      v->SetName (name);
      vl->Add (nv);
    }
    v->SetVector (w);
  }
  else if (!strcasecmp (cmd, "setvar"))
  {
    if (!arg)
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	  	"Please give a name of a variable and a value.");
      return false;
    }
    char name[256];
    float value;
    csScanStr (arg, "%s,%f", name, &value);

    iSharedVariableList* vl = Sys->view->GetEngine ()->GetVariableList ();
    iSharedVariable* v = vl->FindByName (name);
    if (!v)
    {
      csRef<iSharedVariable> nv = vl->New ();
      v = nv;
      v->SetName (name);
      vl->Add (nv);
    }
    v->Set (value);
  }
  else if (!strcasecmp (cmd, "var"))
  {
    if (!arg)
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	  	"Please give a name of a variable to show.");
      return false;
    }
    char name[256];
    csScanStr (arg, "%s", name);

    iSharedVariableList* vl = Sys->view->GetEngine ()->GetVariableList ();
    iSharedVariable* v = vl->FindByName (name);
    if (!v)
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	  	"Couldn't find variable '%s'!", name);
      return false;
    }
    int t = v->GetType ();
    switch (t)
    {
      case iSharedVariable::SV_FLOAT:
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "  float '%s'=%g",
	  	v->GetName (), v->Get ());
	break;
      case iSharedVariable::SV_COLOR:
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "  color '%s'=%g,%g,%g",
	  	v->GetName (), v->GetColor ().red, v->GetColor ().green,
		v->GetColor ().blue);
	break;
      case iSharedVariable::SV_VECTOR:
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "  vector '%s'=%g,%g,%g",
	  	v->GetName (), v->GetVector ().x, v->GetVector ().y,
		v->GetVector ().z);
        break;
      default:
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "  unknown '%s'=?",
	  	v->GetName ());
        break;
    }
  }
  else if (!strcasecmp (cmd, "varlist"))
  {
    iSharedVariableList* vl = Sys->view->GetEngine ()->GetVariableList ();
    int i;
    for (i = 0 ; i < vl->GetCount () ; i++)
    {
      iSharedVariable* v = vl->Get (i);
      int t = v->GetType ();
      switch (t)
      {
        case iSharedVariable::SV_FLOAT:
          Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "  float '%s'=%g",
	  	v->GetName (), v->Get ());
	  break;
        case iSharedVariable::SV_COLOR:
          Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "  color '%s'=%g,%g,%g",
	  	v->GetName (), v->GetColor ().red, v->GetColor ().green,
		v->GetColor ().blue);
	  break;
        case iSharedVariable::SV_VECTOR:
          Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "  vector '%s'=%g,%g,%g",
	  	v->GetName (), v->GetVector ().x, v->GetVector ().y,
		v->GetVector ().z);
          break;
        default:
          Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "  unknown '%s'=?",
	  	v->GetName ());
          break;
      }
    }
  }
  else if (!strcasecmp (cmd, "conflist"))
  {
    if (arg)
    {
      int idx;
      sscanf (arg, "%d", &idx);
      int i = 0;
      iBase* plugin = 0;
      csRef<iPluginIterator> it = Sys->plugin_mgr->GetPlugins ();
      while (it->HasNext ())
      {
        plugin = it->Next ();
        if (idx == i) break;
        plugin = 0;
        i++;
      }

      if (!plugin)
	Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
		"Bad value for plugin (see 'plugins' command)!");
      else
      {
        csRef<iConfig> config (SCF_QUERY_INTERFACE (plugin, iConfig));
	if (!config)
	  Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	  	"No config interface for this plugin.");
	else
	{
          int i;
          for (i = 0 ; ; i++)
          {
	    csOptionDescription odesc;
	    if (!config->GetOptionDescription (i, &odesc)) break;
	    char buf[256];
	    sprintf (buf, "Option %s (%s) ", odesc.name, odesc.description);
	    csVariant var;
	    config->GetOption (i, &var);
	    switch (odesc.type)
	    {
	      case CSVAR_LONG: Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	      	"%sLONG=%ld", buf, var.GetLong ());
		break;
	      case CSVAR_BOOL: Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	      	"%sBOOL=%d", buf, var.GetBool ());
	        break;
	      case CSVAR_CMD: Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	      	"%sCMD", buf);
		break;
	      case CSVAR_FLOAT: Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	      	"%sFLOAT=%g", buf, var.GetFloat ());
		break;
	      case CSVAR_STRING: Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	      	"%sSTRING=%s", buf, var.GetString ());
		break;
	      default: Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	      	"%s<unknown type>", buf);
		break;
	    }
          }
	}
      }
    }
    else
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Expected index to plugin (from 'plugins' command)!");
  }
  else if (!strcasecmp (cmd, "confset"))
  {
    if (arg)
    {
      char name[50];
      char val[256];
      int idx;
      csScanStr (arg, "%d,%s,%s", &idx, name, val);
      int i = 0;
      iBase* plugin = 0;
      csRef<iPluginIterator> it = Sys->plugin_mgr->GetPlugins ();
      while (it->HasNext ())
      {
        plugin = it->Next ();
        if (idx == i) break;
        plugin = 0;
        i++;
      }

      if (!plugin)
      {
	Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
		"Bad value for plugin (see 'plugins' command)!");
      }
      else
      {
	SetConfigOption (plugin, name, val);
      }
    }
    else
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Expected index to plugin (from 'plugins' command)!");
  }
  else if (!strcasecmp (cmd, "action"))
  {
    csVector3 where = Sys->view->GetCamera ()->GetTransform ().This2Other (
    	3.0f*CS_VEC_FORWARD);
    int pidx;
    csVector3 isect;
    iMeshWrapper* mesh = 
      Sys->view->GetCamera ()->GetSector ()->HitBeamPortals (
      Sys->view->GetCamera ()->GetTransform ().GetOrigin (), where, isect, &pidx);

    if (mesh && pidx != -1)
    {
      //Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	//"Action polygon '%s'", p->GetStaticData ()->GetName ());
      printf ("ACTION\n");
      //Sys->ActivateObject ((csObject*)(ob->QueryObject ()));
    }
  }
  else if (!strcasecmp (cmd, "saverec"))
  {
    if (arg)
    {
      char buf[255];
      sprintf (buf, "/this/%s.rec", arg);
      SaveRecording (Sys->myVFS, buf);
    }
    else
      SaveRecording (Sys->myVFS, "/this/record");
  }
  else if (!strcasecmp (cmd, "loadrec"))
  {
    if (Sys->perf_stats && Sys->recorded_perf_stats)
      Sys->perf_stats->FinishSubsection ();
    Sys->recorded_perf_stats = 0;
    delete [] Sys->recorded_perf_stats_name;
    Sys->recorded_perf_stats_name = 0;
    if (arg)
    {
      char buf[255];
      sprintf (buf, "/this/%s.rec", arg);
      LoadRecording (Sys->myVFS, buf);
      Sys->recorded_perf_stats_name = csStrNew (arg);
    }
    else
      LoadRecording (Sys->myVFS, "/this/record");
  }
  else if (!strcasecmp (cmd, "clrrec"))
  {
    Sys->recording.DeleteAll ();
    Sys->recording.SetLength (0);
  }
  else if (!strcasecmp (cmd, "record"))
  {
    if (Sys->cfg_recording == -1)
    {
      Sys->cfg_playrecording = -1;
      Sys->cfg_recording = 0;
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Start recording camera movement...");
      if (Sys->perf_stats)
        Sys->recorded_perf_stats = Sys->perf_stats->StartNewSubsection (0);
    }
    else
    {
      Sys->cfg_recording = -1;
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Stop recording.");
      if (Sys->perf_stats)
        Sys->perf_stats->FinishSubsection ();
      Sys->recorded_perf_stats = 0;
    }
  }
  else if (!strcasecmp (cmd, "play"))
  {
    if (Sys->cfg_playrecording == -1)
    {
      Sys->cfg_recording = -1;
      Sys->cfg_playrecording = 0;
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Start playing back camera movement...");
      Sys->cfg_playloop = true;
      if (arg)
      {
	bool summary = true;
	char name[50], option[50];
	int resolution = 0;
	csScanStr (arg, "%s,%d,%s", option, &resolution, name);
	if (Sys->perf_stats)
	  Sys->recorded_perf_stats = Sys->perf_stats->StartNewSubsection (name);
	if (!strcasecmp (option, "res") && (resolution >= 1))
	{
	  if (Sys->perf_stats)
	    Sys->perf_stats->SetResolution (resolution);
	  summary = false;
	}
	else if (!strcasecmp (option, "break") && (resolution >= 1))
	{
	  Sys->recorded_perf_stats->DebugSetBreak (resolution);
	  return true;
	}
	else
	{
	  strcpy(name, option);
	}
	char buf[255];
	sprintf (buf, "/this/%s.rps", name);
	Sys->recorded_perf_stats->SetOutputFile (buf, summary);
	if (Sys->recorded_perf_stats_name)
	  Sys->recorded_perf_stats->SetName (Sys->recorded_perf_stats_name);
	Sys->recorded_perf_stats->DebugSetBreak (-1);
	Sys->cfg_playloop = false;
      }
    }
    else
    {
      Sys->cfg_playrecording = -1;
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Stop playback.");
      if (Sys->perf_stats)
        Sys->perf_stats->FinishSubsection ();
      Sys->recorded_perf_stats = 0;
    }
  }
  else if (!strcasecmp (cmd, "recsubperf"))
  {
    RECORD_ARGS (cmd, arg);
    if (Sys->recorded_perf_stats)
    {
      if (!Sys->recorded_perf_stats->IsSubsection ())
      {
	if (!arg)
	  Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	  	"Error: Expecting name of subsection.");
	else
	{
	  Sys->recorded_perf_stats->StartNewSubsection (arg);
	  Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	  	"Performance subsection '%s'",arg);
	}
      }
      else
      {
	Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
		"Finished performance subsection.");
	Sys->recorded_perf_stats->
		PrintSubsectionStats (CS_REPORTER_SEVERITY_NOTIFY);
	Sys->recorded_perf_stats->FinishSubsection ();
      }
    }
    else
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Error: Option valid only when recording.");
  }
  else if (!strcasecmp (cmd, "bind"))
  {
    extern void bind_key (const char* arg);
    bind_key (arg);
  }
  else if (!strcasecmp (cmd, "bugplug"))
  {
    csRef<iBase> plug = CS_LOAD_PLUGIN_ALWAYS (Sys->plugin_mgr,
    	"crystalspace.utilities.bugplug");
    plug->IncRef ();	// Avoid smart pointer release (@@@)
  }
  else if (!strcasecmp (cmd, "do_logo"))
    csCommandProcessor::change_boolean (arg, &Sys->do_logo, "do_logo");
  else if (!strcasecmp (cmd, "do_gravity"))
    csCommandProcessor::change_boolean (arg, &Sys->do_gravity, "do_gravity");
  else if (!strcasecmp (cmd, "inverse_mouse"))
    csCommandProcessor::change_boolean (arg, &Sys->inverse_mouse, "inverse_mouse");
  else if (!strcasecmp (cmd, "colldet"))
    csCommandProcessor::change_boolean (arg, &Sys->do_cd, "colldet");
  else if (!strcasecmp (cmd, "zbuf"))
    csCommandProcessor::change_boolean (arg, &Sys->do_show_z, "zbuf");
  else if (!strcasecmp (cmd, "db_boxshow"))
    csCommandProcessor::change_boolean (arg, &Sys->do_show_debug_boxes, "show debug boxes");
  else if (!strcasecmp (cmd, "db_boxcam1"))
    Sys->debug_box1.SetCenter (Sys->view->GetCamera ()->GetTransform ().GetOrigin ());
  else if (!strcasecmp (cmd, "db_boxcam2"))
    Sys->debug_box2.SetCenter (Sys->view->GetCamera ()->GetTransform ().GetOrigin ());
  else if (!strcasecmp (cmd, "db_boxsize1"))
  {
    float size = Sys->debug_box1.MaxX ()-Sys->debug_box1.MinX ();
    csCommandProcessor::change_float (arg, &size, "box1 size", 0.01, 1000);
    Sys->debug_box1.SetSize (csVector3 (size, size, size));
  }
  else if (!strcasecmp (cmd, "db_boxsize2"))
  {
    float size = Sys->debug_box2.MaxX ()-Sys->debug_box2.MinX ();
    csCommandProcessor::change_float (arg, &size, "box2 size", 0.01, 1000);
    Sys->debug_box2.SetSize (csVector3 (size, size, size));
  }
  else if (!strcasecmp (cmd, "db_frustum"))
    csCommandProcessor::change_int (arg, &Sys->cfg_debug_check_frustum, "debug check frustum", 0, 2000000000);
  else if (!strcasecmp (cmd, "db_radstep"))
  {
#if 0
    csRadiosity* rad;
    csEngine* engine = (csEngine*)(iEngine*)(Sys->Engine);
    if ((rad = engine->GetRadiosity ()) != 0)
    {
      int steps;
      if (arg)
        sscanf (arg, "%d", &steps);
      else
        steps = 1;
      if (steps < 1) steps = 1;
      rad->DoRadiosityStep (steps);
      engine->InvalidateLightmaps ();
    }
#endif
  }
  else if (!strcasecmp (cmd, "db_radtodo"))
  {
#if 0
    csRadiosity* rad;
    csEngine* engine = (csEngine*)(iEngine*)(Sys->Engine);
    if ((rad = engine->GetRadiosity ()) != 0)
    {
      rad->ToggleShowDeltaMaps ();
      engine->InvalidateLightmaps ();
    }
#endif
  }
  else if (!strcasecmp (cmd, "db_radhi"))
  {
#if 0
    csRadiosity* rad;
    csEngine* engine = (csEngine*)(iEngine*)(Sys->Engine);
    if ((rad = engine->GetRadiosity ()) != 0)
    {
      csRef<iPolygon3D> p (SCF_QUERY_INTERFACE (rad->GetNextPolygon (),
      	iPolygon3D));
      Sys->selected_polygon = p;
    }
#endif
  }
  else if (!strcasecmp (cmd, "palette"))
    csCommandProcessor::change_boolean (arg, &Sys->do_show_palette, "palette");
  else if (!strcasecmp (cmd, "move3d"))
    csCommandProcessor::change_boolean (arg, &Sys->move_3d, "move3d");
  else if (!strcasecmp (cmd, "freelook"))
  {
    csCommandProcessor::change_boolean (arg, &Sys->do_freelook, "freelook");
    if (Sys->do_freelook)
      Sys->myG2D->SetMousePosition (FRAME_WIDTH / 2, FRAME_HEIGHT / 2);
    Sys->myG2D->SetMouseCursor (Sys->do_freelook?csmcNone:csmcArrow);
  }
  else if (!strcasecmp (cmd, "stats"))
  {
    csCommandProcessor::change_boolean (arg, &Sys->do_stats, "stats");
    if (Sys->do_stats) Sys->do_show_coord = false;
  }
  else if (!strcasecmp (cmd, "coordshow"))
  {
    csCommandProcessor::change_boolean (arg, &Sys->do_show_coord, "coordshow");
    if (Sys->do_show_coord) Sys->do_stats = false;
  }
  else if (!strcasecmp (cmd, "hi"))
  {
#if 0
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    csPolygon3D* hi = arg ? Sys->view->GetCamera ()->GetSector ()->GetPolygon3D (arg) : (csPolygon3D*)0;
    if (hi) Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
    	"Hilighting polygon: '%s'", arg);
    else Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Disabled hilighting.");
    Sys->selected_polygon = hi;
#endif
  }
  else if (!strcasecmp (cmd, "p_alpha"))
  {
#if 0
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    csPolygon3D* hi = Sys->selected_polygon;
    if (hi)
    {
      if (hi->GetPortal ())
      {
        int a = hi->GetAlpha ();
        csCommandProcessor::change_int (arg, &a, "portal alpha", 0, 100);
	hi->SetAlpha (a);
      }
      else Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Only for portals!");
    }
    else Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "No polygon selected!");
#endif
  }
  else if (!strcasecmp (cmd, "s_fog"))
  {
    csFog* f = Sys->view->GetCamera ()->GetSector ()->GetFog ();
    if (!arg)
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Fog in current sector (%f,%f,%f) density=%f",
      	f->red, f->green, f->blue, f->density);
    }
    else
    {
      float r, g, b, dens;
      if (csScanStr (arg, "%f,%f,%f,%f", &r, &g, &b, &dens) != 4)
      {
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
		"Expected r,g,b,density. Got something else!");
        return false;
      }
      f->enabled = true;
      f->density = dens;
      f->red = r;
      f->green = g;
      f->blue = b;
    }
  }
  else if (!strcasecmp (cmd, "portal"))
  {
    if (arg)
    {
      char level[100];
      csScanStr (arg, "%s", level);
      void OpenPortal (iLoader*, iView* view, char* lev);
      OpenPortal (Sys->LevelLoader, Sys->view, level);
    }
    else
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Expected parameter 'level'!");
  }
  else if (!strcasecmp (cmd, "fs_inter"))
  {
    Sys->do_fs_inter = !Sys->do_fs_inter;
    if (Sys->do_fs_inter)
    {
      Sys->fs_inter_amount = .3;
      Sys->fs_inter_length = 30;
      Sys->fs_inter_anim = 0;
      if (arg)
        csScanStr (arg, "%f,%f", &Sys->fs_inter_amount, &Sys->fs_inter_length);
    }
  }
  else if (!strcasecmp (cmd, "fs_fadeout"))
  {
    Sys->do_fs_fadeout = !Sys->do_fs_fadeout;
    if (Sys->do_fs_fadeout)
    {
      Sys->fs_fadeout_fade = 0;
      Sys->fs_fadeout_dir = true;
    }
  }
  else if (!strcasecmp (cmd, "fs_fadecol"))
  {
    Sys->do_fs_fadecol = !Sys->do_fs_fadecol;
    if (Sys->do_fs_fadecol)
    {
      Sys->fs_fadecol_fade = 0;
      Sys->fs_fadecol_dir = true;
      float r = 1, g = 0, b = 0;
      if (arg) csScanStr (arg, "%f,%f,%f", &r, &g, &b);
      Sys->fs_fadecol_color.Set (r, g, b);
    }
  }
  else if (!strcasecmp (cmd, "fs_fadetxt"))
  {
    Sys->do_fs_fadetxt = !Sys->do_fs_fadetxt;
    if (Sys->do_fs_fadetxt)
    {
      Sys->fs_fadetxt_fade = 0;
      Sys->fs_fadetxt_dir = true;
      char buf[255];
      *buf = 0;
      if (arg) csScanStr (arg, "%s", buf);
      iMaterialWrapper* mat = Sys->view->GetEngine ()->GetMaterialList ()->FindByName (buf);
      if (mat)
      {
        Sys->fs_fadetxt_mat = mat->GetMaterialHandle ();
      }
      else
      {
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
		"Can't find material!");
	Sys->do_fs_fadetxt = false;
      }
    }
  }
  else if (!strcasecmp (cmd, "fs_red"))
  {
    Sys->do_fs_red = !Sys->do_fs_red;
    if (Sys->do_fs_red)
    {
      Sys->fs_red_fade = 0;
      Sys->fs_red_dir = true;
    }
  }
  else if (!strcasecmp (cmd, "fs_green"))
  {
    Sys->do_fs_green = !Sys->do_fs_green;
    if (Sys->do_fs_green)
    {
      Sys->fs_green_fade = 0;
      Sys->fs_green_dir = true;
    }
  }
  else if (!strcasecmp (cmd, "fs_blue"))
  {
    Sys->do_fs_blue = !Sys->do_fs_blue;
    if (Sys->do_fs_blue)
    {
      Sys->fs_blue_fade = 0;
      Sys->fs_blue_dir = true;
    }
  }
  else if (!strcasecmp (cmd, "fs_whiteout"))
  {
    Sys->do_fs_whiteout = !Sys->do_fs_whiteout;
    if (Sys->do_fs_whiteout)
    {
      Sys->fs_whiteout_fade = 0;
      Sys->fs_whiteout_dir = true;
    }
  }
  else if (!strcasecmp (cmd, "fs_shadevert"))
  {
    Sys->do_fs_shadevert = !Sys->do_fs_shadevert;
    if (Sys->do_fs_shadevert)
    {
      float tr = 1, tg = 0, tb = 0, br = 0, bg = 0, bb = 1;
      if (arg) csScanStr (arg, "%f,%f,%f,%f,%f,%f", &tr, &tg, &tb, &br, &bg, &bb);
      Sys->fs_shadevert_topcol.Set (tr, tg, tb);
      Sys->fs_shadevert_botcol.Set (br, bg, bb);
    }
  }
  else if (!strcasecmp (cmd, "perftest"))
  {
    int num = 100;
    if (arg) csScanStr (arg, "%d", &num);
    perf_test (num);
  }
  else if (!strcasecmp (cmd, "debug0"))
  {
  }
  else if (!strcasecmp (cmd, "debug1"))
  {
  }
  else if (!strcasecmp (cmd, "debug2"))
  {
  }
  else if (!strcasecmp (cmd, "strafe_left"))
  {
    float f = safe_atof (arg);
    if (Sys->move_3d) { if (f) Sys->imm_left (.1, false, false); }
    else Sys->strafe (-1*f,0);
  }
  else if (!strcasecmp (cmd, "strafe_right"))
  {
    float f = safe_atof (arg);
    if (Sys->move_3d) { if (f) Sys->imm_right (.1, false, false); }
    else Sys->strafe (1*f,0);
  }
  else if (!strcasecmp (cmd, "step_forward"))
  {
    float f = safe_atof (arg);
    if (Sys->move_3d) { if (f) Sys->imm_forward (.1, false, false); }
    else Sys->step (1*f,0);
  }
  else if (!strcasecmp (cmd, "step_backward"))
  {
    float f = safe_atof (arg);
    if (Sys->move_3d) { if (f) Sys->imm_backward (.1, false, false); }
    else Sys->step (-1*f,0);
  }
  else if (!strcasecmp (cmd, "rotate_left"))
  {
    float f = safe_atof (arg);
    if (Sys->move_3d) { if (f) Sys->imm_rot_left_camera (.1, false, false); }
    else Sys->rotate (-1*f,0);
  }
  else if (!strcasecmp (cmd, "rotate_right"))
  {
    float f = safe_atof (arg);
    if (Sys->move_3d) { if (f) Sys->imm_rot_right_camera (.1, false, false); }
    else Sys->rotate (1*f,0);
  }
  else if (!strcasecmp (cmd, "look_up"))
  {
    float f = safe_atof (arg);
    if (Sys->move_3d) { if (f) Sys->imm_rot_right_xaxis (.1, false, false); }
    else Sys->look (-1*f,0);
  }
  else if (!strcasecmp (cmd, "look_down"))
  {
    float f = safe_atof (arg);
    if (Sys->move_3d) { if (f) Sys->imm_rot_left_xaxis (.1, false, false); }
    else Sys->look (1*f,0);
  }
  else if (!strcasecmp (cmd, "jump"))
  {
    if (Sys->do_gravity && Sys->on_ground)
      Sys->velocity.y = Sys->cfg_jumpspeed;
  }
  else if (!strcasecmp (cmd, "i_forward"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_forward (.1, (bool)slow, (bool)fast);
  }
  else if (!strcasecmp (cmd, "i_backward"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_backward (.1, (bool)slow, (bool)fast);
  }
  else if (!strcasecmp (cmd, "i_left"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_left (.1, (bool)slow, (bool)fast);
  }
  else if (!strcasecmp (cmd, "i_right"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_right (.1, (bool)slow, (bool)fast);
  }
  else if (!strcasecmp (cmd, "i_up"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_up (.1, (bool)slow, (bool)fast);
  }
  else if (!strcasecmp (cmd, "i_down"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_down (.1, (bool)slow, (bool)fast);
  }
  else if (!strcasecmp (cmd, "i_rotleftc"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_rot_left_camera (.1, (bool)slow, (bool)fast);
  }
  else if (!strcasecmp (cmd, "i_rotleftw"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_rot_left_world (.1, (bool)slow, (bool)fast);
  }
  else if (!strcasecmp (cmd, "i_rotrightc"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_rot_right_camera (.1, (bool)slow, (bool)fast);
  }
  else if (!strcasecmp (cmd, "i_rotrightw"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_rot_right_world (.1, (bool)slow, (bool)fast);
  }
  else if (!strcasecmp (cmd, "i_rotleftx"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_rot_left_xaxis (.1, (bool)slow, (bool)fast);
  }
  else if (!strcasecmp (cmd, "i_rotleftz"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_rot_left_zaxis (.1, (bool)slow, (bool)fast);
  }
  else if (!strcasecmp (cmd, "i_rotrightx"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_rot_right_xaxis (.1, (bool)slow, (bool)fast);
  }
  else if (!strcasecmp (cmd, "i_rotrightz"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_rot_right_zaxis (.1, (bool)slow, (bool)fast);
  }
  else if (!strcasecmp (cmd, "fire"))
  {
    RECORD_CMD (cmd);
    extern void fire_missile ();
    fire_missile ();
  }
  else if (!strcasecmp (cmd, "lightning"))
  {
    extern void show_lightning ();
    show_lightning ();
  }
  else if (!strcasecmp (cmd, "rain"))
  {
    char txtname[100];
    int cnt = 0;
    /* speed and num must be preset to prevent compiler warnings
     * on some systems. */
    int num = 0;
    float speed = 0;
    if (arg) cnt = csScanStr (arg, "%s,%d,%f", txtname, &num, &speed);
    extern void add_particles_rain (iSector* sector, char* txtname,
    	int num, float speed, bool do_camera);
    if (cnt <= 2) speed = 2;
    if (cnt <= 1) num = 500;
    if (cnt <= 0) strcpy (txtname, "raindrop");
    add_particles_rain (Sys->view->GetCamera ()->GetSector (),
    	txtname, num, speed, false);
  }
  else if (!strcasecmp (cmd, "frain"))
  {
    char txtname[100];
    int cnt = 0;
    /* speed and num must be preset to prevent compiler warnings
     * on some systems. */
    int num = 0;
    float speed = 0;
    if (arg) cnt = csScanStr (arg, "%s,%d,%f", txtname, &num, &speed);
    extern void add_particles_rain (iSector* sector, char* txtname,
    	int num, float speed, bool do_camera);
    if (cnt <= 2) speed = 2;
    if (cnt <= 1) num = 500;
    if (cnt <= 0) strcpy (txtname, "raindrop");
    add_particles_rain (Sys->view->GetCamera ()->GetSector (),
    	txtname, num, speed, true);
  }
  else if (!strcasecmp (cmd, "snow"))
  {
    char txtname[100];
    int cnt = 0;
    /* speed and num must be preset to prevent compiler warnings
     * on some systems. */
    int num = 0;
    float speed = 0;
    if (arg) cnt = csScanStr (arg, "%s,%d,%f", txtname, &num, &speed);
    extern void add_particles_snow (iSector* sector, char* txtname,
    	int num, float speed);
    if (cnt <= 2) speed = .3;
    if (cnt <= 1) num = 500;
    if (cnt <= 0) strcpy (txtname, "snow");
    add_particles_snow (Sys->view->GetCamera ()->GetSector (),
    	txtname, num, speed);
  }
  else if (!strcasecmp (cmd, "flame"))
  {
    RECORD_ARGS (cmd, arg);
    char txtname[100];
    int cnt = 0;
    int num = 0;
    if (arg) cnt = csScanStr (arg, "%s,%d", txtname, &num);
    extern void add_particles_fire (iSector* sector, char* txtname,
    	int num, const csVector3& origin);
    if (cnt <= 1) num = 200;
    if (cnt <= 0) strcpy (txtname, "raindrop");
    add_particles_fire (Sys->view->GetCamera ()->GetSector (),
    	txtname, num, Sys->view->GetCamera ()->GetTransform ().GetOrigin ()-
	csVector3 (0, Sys->cfg_body_height, 0));
  }
  else if (!strcasecmp (cmd, "fountain"))
  {
    RECORD_ARGS (cmd, arg);
    char txtname[100];
    int cnt = 0;
    int num = 0;
    if (arg) cnt = csScanStr (arg, "%s,%d", txtname, &num);
    extern void add_particles_fountain (iSector* sector, char* txtname,
    	int num, const csVector3& origin);
    if (cnt <= 1) num = 400;
    if (cnt <= 0) strcpy (txtname, "spark");
    add_particles_fountain (Sys->view->GetCamera ()->GetSector (),
    	txtname, num, Sys->view->GetCamera ()->GetTransform ().GetOrigin ()-
	csVector3 (0, Sys->cfg_body_height, 0));
  }
  else if (!strcasecmp (cmd, "explosion"))
  {
    char txtname[100];
    int cnt = 0;
    if (arg) cnt = csScanStr (arg, "%s", txtname);
    extern void add_particles_explosion (iSector* sector, const csVector3& center,
    	char* txtname);
    if (cnt != 1)
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Expected parameter 'texture'!");
    }
    else
      add_particles_explosion (Sys->view->GetCamera ()->GetSector (),
    	Sys->view->GetCamera ()->GetTransform ().GetOrigin (), txtname);
  }
  else if (!strcasecmp (cmd, "spiral"))
  {
    char txtname[100];
    int cnt = 0;
    if (arg) cnt = csScanStr (arg, "%s", txtname);
    extern void add_particles_spiral (iSector* sector, const csVector3& bottom,
    	char* txtname);
    if (cnt != 1)
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Expected parameter 'texture'!");
    }
    else
      add_particles_spiral (Sys->view->GetCamera ()->GetSector (),
    	Sys->view->GetCamera ()->GetTransform ().GetOrigin (), txtname);
  }
  else if (!strcasecmp (cmd, "loadmesh"))
  {
    char filename[100], tempname[100], txtname[100];
    int cnt = 0;
    if (arg) cnt = csScanStr (arg, "%s,%s,%s", filename, tempname, txtname);
    if (cnt != 3)
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Expected parameters 'file','template','texture'!");
    }
    else load_meshobj (filename, tempname, txtname);
  }
  else if (!strcasecmp (cmd, "addmesh"))
  {
    char tname[100];
    char sname[100];
    float size = 9;
    int cnt = 0;
    if (arg) cnt = csScanStr (arg, "%s,%s,%f", tname, sname, &size);
    if(cnt != 3)
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Expected parameters 'templatename','meshname','size'!");
    }
    else
    {
      add_meshobj (tname, sname, Sys->view->GetCamera ()->GetSector (),
    	          Sys->view->GetCamera ()->GetTransform ().GetOrigin (), size);
    }
  }
  else if (!strcasecmp (cmd, "delmesh"))
  {
    char name[100];
    if (arg)
    {
      csScanStr (arg, "%s", name);
      iMeshList* meshes = Sys->Engine->GetMeshes ();
      iMeshWrapper* mesh = meshes->FindByName (name);
      if (mesh)
        meshes->Remove (mesh);
      else
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
		"Can't find mesh with that name!");
    }
    else
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Missing mesh name!");
  }
  else if (!strcasecmp (cmd, "listmeshes"))
  {
    list_meshes ();
  }
  else if(!strcasecmp(cmd, "listactions"))
  {
    char name[100];
    char action[100];
    int cnt = 0;
    if (arg) cnt = csScanStr (arg, "%s,%s", name, action);
    if(cnt != 1)
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Expected parameters 'meshname'!");
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"To get the names use 'listmeshes'");
    }
    else
    {
      // See if the mesh exists.
      iMeshWrapper *wrap = Sys->Engine->GetMeshes ()->FindByName (name);
      if (wrap)
      {
	csRef<iSprite3DFactoryState> fstate (SCF_QUERY_INTERFACE (
		wrap->GetMeshObject ()->GetFactory (),
		iSprite3DFactoryState));
	iSpriteAction* aspr_act;
	int i;

	for (i = 0; i < (fstate->GetActionCount ()); i ++)
	{
	  aspr_act = fstate->GetAction (i);
	  Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	  	"%s", aspr_act->GetName ());
	}
      }
      else
      {
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
		"Expected parameters 'meshname'!");
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
		"To get the names use 'listmeshes'");
      }
    }
  }
  else if (!strcasecmp (cmd, "setaction"))
  {
    char name[100];
    char action[100];
    int cnt = 0;
    if (arg) cnt = csScanStr (arg, "%s,%s", name, action);
    if(cnt != 2)
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Expected parameters 'meshname,action'!");
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"To get the names use 'listmeshes'");
    }
    else
    {
      // Test to see if the mesh exists.
      iMeshWrapper* wrap = Sys->Engine->GetMeshes ()->FindByName (name);
      if (!wrap)
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "No such mesh!");
      else
      {
        csRef<iSprite3DState> state (
		SCF_QUERY_INTERFACE (wrap->GetMeshObject (),
		iSprite3DState));
        if (state)
        {
          // Test to see if the action exists for that sprite.
          if (!state->SetAction (action))
	  {
            Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
		         "Expected parameters 'meshname,action'!");
	    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
		   "That mesh does not have that action.");
	  }
        }
        else
          Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Mesh is not a 3D sprite!");
      }
    }
  }
  else if (!strcasecmp (cmd, "setmotion"))
  {
    char name[100];
    char motion[100];
    int cnt = 0;
    if (arg) cnt = csScanStr (arg, "%s,%s", name, motion);
    if(cnt != 2)
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Expected parameters 'meshname,motion'!");
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"To get the names use 'listmeshes'");
    }
    else
    {
      // Test to see if the mesh exists.
      iMeshWrapper* wrap = Sys->Engine->GetMeshes ()->FindByName (name);
      if (!wrap)
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "No such mesh!");
      else
      {
	csRef<iSprite3DState> state (
		SCF_QUERY_INTERFACE (wrap->GetMeshObject (),
		iSprite3DState));
        if (state)
        {
	  csRef<iSkeletonBone> sb (
	  	SCF_QUERY_INTERFACE(state->GetSkeletonState(), iSkeletonBone));
	  if (sb)
	  {
	    if (Sys->myMotionMan)
	    {
              iMotionController* mc=Sys->myMotionMan->
	      	FindControllerBySkeleton(sb);
              if(!mc)
	      {
                mc=Sys->myMotionMan->AddController(sb);
              }
              iMotionTemplate* mt=Sys->myMotionMan->FindMotionByName(motion);
	      if (mt)
	      {
                mc->SetMotion(mt);
              }
	      else
	        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
			"That motion does not exist!");
	    }
	    else
	      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	      	"No motion manager exists to animate the skeleton!");
	  }
	  else
	    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	    	"That sprite does not contain a skeleton!");
        }
	else
	{
          Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Mesh is not a 3D sprite!");
	}
      }
    }
  }
  else if (!strcasecmp (cmd, "addskel"))
  {
    RECORD_ARGS (cmd, arg);
    int depth = 0, width = 0;
    if (arg) csScanStr (arg, "%d,%d", &depth, &width);
    if (depth < 1) depth = 3;
    if (width < 1) width = 3;
    extern void add_skeleton_tree (iSector* where, csVector3 const& pos,
    	int depth, int width);
    add_skeleton_tree (Sys->view->GetCamera ()->GetSector (),
    	Sys->view->GetCamera ()->GetTransform ().GetOrigin (), depth, width);
  }
  else if (!strcasecmp (cmd, "addghost"))
  {
    RECORD_ARGS (cmd, arg);
    int depth, width;
    if (arg) csScanStr (arg, "%d,%d", &depth, &width);
    else { depth = 5; width = 8; }
    extern void add_skeleton_ghost (iSector* where, csVector3 const& pos,
    	int maxdepth, int width);
    add_skeleton_ghost (Sys->view->GetCamera ()->GetSector (),
    	Sys->view->GetCamera ()->GetTransform ().GetOrigin (), depth, width);
  }
  else if (!strcasecmp (cmd, "addbot"))
  {
    RECORD_ARGS (cmd, arg);
    float radius = 0;
    if (arg) csScanStr (arg, "%f", &radius);
    extern void add_bot (float size, iSector* where, csVector3 const& pos,
	float dyn_radius);
    add_bot (2, Sys->view->GetCamera ()->GetSector (),
    	Sys->view->GetCamera ()->GetTransform ().GetOrigin (), radius);
  }
  else if (!strcasecmp (cmd, "delbot"))
  {
    extern void del_bot ();
    del_bot ();
  }
  else if (!strcasecmp (cmd, "clrlights"))
  {
    RECORD_CMD (cmd);
    csRef<iLightIterator> lit (Sys->view->GetEngine ()->GetLightIterator ());
    iLight* l;
    while (lit->HasNext ())
    {
      l = lit->Next ();
      l->SetColor (csColor (0, 0, 0));
    }
  }
  else if (!strcasecmp (cmd, "setlight"))
  {
    if (Sys->selected_light)
    {
      float r, g, b;
      if (arg && csScanStr (arg, "%f,%f,%f", &r, &g, &b) == 3)
        Sys->selected_light->SetColor (csColor (r, g, b));
      else
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Arguments missing or invalid!");
    }
    else
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "No light selected!");
  }
  else if (!strcasecmp (cmd, "addlight"))
  {
    RECORD_ARGS (cmd, arg);
    csVector3 dir (0,0,0);
    csVector3 pos = Sys->view->GetCamera ()->GetTransform ().This2Other (dir);
    csRef<iDynLight> dyn;

    bool rnd;
    float r, g, b, radius, thing_shadows;
    if (arg && csScanStr (arg, "%f,%f,%f,%f,%d", &r, &g, &b, &radius,
    	&thing_shadows) == 5)
    {
      dyn = Sys->view->GetEngine ()->CreateDynLight (pos,
      	radius, csColor (r, g, b));
      if (thing_shadows) dyn->QueryLight ()->
      	GetFlags ().Set (CS_LIGHT_THINGSHADOWS, CS_LIGHT_THINGSHADOWS);
      rnd = false;
    }
    else
    {
      dyn = Sys->view->GetEngine ()->CreateDynLight (pos,
      	6, csColor (1, 1, 1));
      rnd = true;
    }
    dyn->QueryLight ()->SetSector (Sys->view->GetCamera ()->GetSector ());
    dyn->Setup ();
    extern void AttachRandomLight (iDynLight* light);
    if (rnd)
      AttachRandomLight (dyn);
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Dynamic light added.");
  }
  else if (!strcasecmp (cmd, "delstlight"))
  {
    RECORD_ARGS (cmd, arg);
    char name[255];
    if (arg && csScanStr (arg, "%s", name))
    {
    }
    else
    {
      strcpy (name, "deflight");
    }
    iLightList* ll = Sys->view->GetCamera ()->GetSector ()->GetLights ();
    iLight* l = ll->FindByName (name);
    if (!l)
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Couldn't find light '%s' in this sector!", name);
    }
    else
    {
      csRef<iStatLight> sl = SCF_QUERY_INTERFACE (l, iStatLight);
      if (sl)
      {
        Sys->view->GetEngine ()->RemoveLight (sl);
	Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Static light removed.");
      }
    }
  }
  else if (!strcasecmp (cmd, "addstlight"))
  {
    RECORD_ARGS (cmd, arg);
    csVector3 dir (0,0,0);
    csVector3 pos = Sys->view->GetCamera ()->GetTransform ().This2Other (dir);
    csRef<iStatLight> light;

    float r, g, b, radius;
    char name[255];
    if (arg && csScanStr (arg, "%s,%f,%f,%f,%f", name, &r, &g, &b,
    	&radius) == 5)
    {
      light = Sys->view->GetEngine ()->CreateLight (name,
        pos, radius, csColor (r, g, b), true);
    }
    else
    {
      light = Sys->view->GetEngine ()->CreateLight ("deflight",
        pos, 12, csColor (0, 0, 1), true);
    }
    iLightList* ll = Sys->view->GetCamera ()->GetSector ()->GetLights ();
    ll->Add (light->QueryLight ());
    Sys->view->GetEngine ()->ForceRelight (light);
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Static light added.");
  }
  else if (!strcasecmp (cmd, "dellight"))
  {
    iDynLight* dyn;
    if ((dyn = Sys->view->GetEngine ()->GetFirstDynLight ()) != 0)
    {
      Sys->view->GetEngine ()->RemoveDynLight (dyn);
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Dynamic light deleted.");
    }
  }
  else if (!strcasecmp (cmd, "dellights"))
  {
    RECORD_CMD (cmd);
    iDynLight* dyn;
    while ((dyn = Sys->view->GetEngine ()->GetFirstDynLight ()) != 0)
    {
      Sys->view->GetEngine ()->RemoveDynLight (dyn);
    }
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "All dynamic lights deleted.");
  }
  else if (!strcasecmp (cmd, "snd_play"))
  {
    if (Sys->mySound)
    {
      csRef<iSoundWrapper> sb (
        CS_GET_NAMED_CHILD_OBJECT (Sys->view->GetEngine ()->
        QueryObject (), iSoundWrapper, arg));
      if (sb)
        sb->GetSound ()->Play();
      else
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
		"Sound '%s' not found!", arg);
    }
  }
  else if (!strcasecmp (cmd, "snd_volume"))
  {
    if (Sys->mySound)
    {
      float vol = Sys->mySound->GetVolume ();
      csCommandProcessor::change_float (arg, &vol, "snd_volume", 0.0, 1.0);
      Sys->mySound->SetVolume (vol);
    }
  }
  else if (!strcasecmp (cmd, "fullscreen"))
    Sys->myG2D->PerformExtension("fullscreen");
  else if (!strcasecmp(cmd, "split_view"))
  {
    if (Sys->split == -1)
    {	
        csBox2 bbox;
        BoundingBoxForView(Sys->view, &bbox);
        
        int width = QInt(bbox.MaxX() - bbox.MinX());
        int height = QInt(bbox.MaxY() - bbox.MinY());
        Sys->views[0]->SetRectangle((int)bbox.MinX(), (int)bbox.MinY(), width / 2, height);
        Sys->views[0]->GetCamera()->SetPerspectiveCenter(bbox.MinX() + (width / 4),
                                                        bbox.MinY() + (height / 2));
        Sys->views[1]->SetRectangle((int)bbox.MinX() + (width / 2), (int)bbox.MinY(), 
                                    width / 2, height);
        Sys->views[1]->GetCamera()->SetPerspectiveCenter(bbox.MinX() + (3 * width / 4),
                                                        bbox.MinY() + (height / 2));
        Sys->split = (Sys->view == Sys->views[0]) ? 0 : 1;
        Sys->Report(CS_REPORTER_SEVERITY_NOTIFY, "Splitting to 2 views");
    };
  }
  else if (!strcasecmp(cmd, "unsplit_view"))
  {
    if (Sys->split != -1)
    {
        csBox2 bbox1, bbox2;
        BoundingBoxForView(Sys->views[0], &bbox1);
        BoundingBoxForView(Sys->views[1], &bbox2);

        int width = QInt(bbox2.MaxX() - bbox1.MinX());
        int height = QInt(bbox1.MaxY() - bbox1.MinY());
        Sys->view->SetRectangle((int)bbox1.MinX(), (int)bbox1.MinY(), width, height);
        Sys->view->GetCamera()->SetPerspectiveCenter(bbox1.MinX() + (width / 2), 
                                                    bbox2.MinY() + (height / 2));
        Sys->split = -1;
        Sys->Report(CS_REPORTER_SEVERITY_NOTIFY, "Unsplitting view");
    }
  }
  else if (!strcasecmp(cmd, "toggle_view"))
  {
    if (Sys->split != -1)
    {
        Sys->split = (Sys->split + 1) % 2;
        Sys->view = Sys->views[Sys->split];
        Sys->Report(CS_REPORTER_SEVERITY_NOTIFY, "Switching to view %d", Sys->split);
    }
  }
  else if (!strcasecmp(cmd, "farplane"))
  {
    if (!arg)
    {
	Sys->Report(CS_REPORTER_SEVERITY_WARNING, "Please specify the"
		"distance of the farplane or 0 to disable it.");
	return true;
    }
    float distance;
    csScanStr (arg, "%f", &distance);
    // disable farplane
    if (distance==0)
    {
	Sys->view->GetCamera()->SetFarPlane(0);
	// we can't disable zclear now... because we can't say for sure that
	// the level didn't need it
	Sys->Report(CS_REPORTER_SEVERITY_NOTIFY, "farplane disabled");
	return true;
    }
    csPlane3 farplane(0,0,-1,distance);
    Sys->view->GetCamera()->SetFarPlane(&farplane);
    // turn on zclear to be sure
    Sys->Engine->SetClearZBuf(true);
  }
  else
    return false;

  return true;
}

#undef RECORD_CMD
#undef RECORD_ARGS
