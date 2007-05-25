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

#include "csgeom/math3d.h"
#include "csgfx/imagemanipulate.h"
#include "csqint.h"
#include "cstool/collider.h"
#include "cstool/cspixmap.h"
#include "cstool/csview.h"
#include "cstool/keyval.h"
#include "csutil/callstack.h"
#include "csutil/csendian.h"
#include "csutil/cspmeter.h" 
#include "csutil/csstring.h"
#include "csutil/flags.h"
#include "csutil/scanstr.h"
#include "csutil/util.h"
#include "iengine/camera.h"
#include "iengine/light.h"
#include "iengine/light.h"
#include "iengine/material.h"
#include "iengine/movable.h"
#include "iengine/portalcontainer.h"
#include "iengine/sector.h"
#include "iengine/sharevar.h"
#include "iengine/scenenode.h"
#include "iengine/campos.h"
#include "igeom/clip2d.h"
#include "igraphic/imageio.h"
#include "imap/loader.h"
#include "imap/saver.h"
#include "imesh/object.h"
#include "imesh/sprite3d.h"
#include "imesh/thing.h"
#include "isndsys.h"
#include "iutil/databuff.h"
#include "iutil/pluginconfig.h"
#include "iutil/vfs.h"
#include "ivaria/bugplug.h"
#include "ivaria/mapnode.h"
#include "ivaria/pmeter.h" 
#include "ivaria/reporter.h"
#include "ivaria/view.h"
#include "ivaria/engseq.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/material.h"

#include "bot.h"
#include "command.h"
#include "infmaze.h"
#include "walktest.h"
#include "wentity.h"

extern WalkTest* Sys;

csString LookForKeyValue(iObjectIterator* it,const char* key);
double ParseScaleFactor(iObjectIterator* it);

// Use a view's clipping rect to calculate a bounding box
void BoundingBoxForView(iView *view, csBox2 *box)
{
    size_t vertexCount = view->GetClipper()->GetVertexCount();
    csVector2 *clip = view->GetClipper()->GetClipPoly();
    for (size_t i = 0; i < vertexCount; i++)
        box->AddBoundingVertex(clip[i]);
}

/// Save recording
void SaveRecording (iVFS* vfs, const char* fName)
{
  csRef<iFile> cf;
  cf = vfs->Open (fName, VFS_FILE_WRITE);
  uint32 l = (int32)Sys->recording.GetSize ();
  l = csLittleEndian::Convert (l);
  cf->Write ((char*)&l, sizeof (l));
  size_t i;
  csRecordedCameraFile camint;
  iSector* prev_sector = 0;
  for (i = 0 ; i < Sys->recording.GetSize () ; i++)
  {
    csRecordedCamera* reccam = (csRecordedCamera*)Sys->recording[i];
    camint.m11 = csLittleEndian::Convert (csFloatToLong (reccam->mat.m11));
    camint.m12 = csLittleEndian::Convert (csFloatToLong (reccam->mat.m12));
    camint.m13 = csLittleEndian::Convert (csFloatToLong (reccam->mat.m13));
    camint.m21 = csLittleEndian::Convert (csFloatToLong (reccam->mat.m21));
    camint.m22 = csLittleEndian::Convert (csFloatToLong (reccam->mat.m22));
    camint.m23 = csLittleEndian::Convert (csFloatToLong (reccam->mat.m23));
    camint.m31 = csLittleEndian::Convert (csFloatToLong (reccam->mat.m31));
    camint.m32 = csLittleEndian::Convert (csFloatToLong (reccam->mat.m32));
    camint.m33 = csLittleEndian::Convert (csFloatToLong (reccam->mat.m33));
    camint.x = csLittleEndian::Convert (csFloatToLong (reccam->vec.x));
    camint.y = csLittleEndian::Convert (csFloatToLong (reccam->vec.y));
    camint.z = csLittleEndian::Convert (csFloatToLong (reccam->vec.z));
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
      size_t _len = strlen (reccam->sector->QueryObject ()->GetName ());
      len = (_len > 255) ? 255 : (unsigned char)len;
      cf->Write ((char*)&len, 1);
      cf->Write (reccam->sector->QueryObject ()->GetName (),
      	1+len);
    }
    prev_sector = reccam->sector;
    if (reccam->cmd)
    {
      size_t _len = strlen (reccam->cmd);
      len = (_len > 255) ? 255 : (unsigned char)len;
      cf->Write ((char*)&len, 1);
      cf->Write (reccam->cmd, 1+len);
    }
    else
    {
      len = 100;
      cf->Write ((char*)&len, 1);
    }
    if (reccam->arg)
    {
      size_t _len = strlen (reccam->arg);
      len = (_len > 255) ? 255 : (unsigned char)len;
      cf->Write ((char*)&len, 1);
      cf->Write (reccam->arg, 1+len);
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
  Sys->recording.SetSize (0);
  int32 l;
  cf->Read ((char*)&l, sizeof (l));
  l = csLittleEndian::Convert (l);
  csRecordedCameraFile camint;
  iSector* prev_sector = 0;
  int i;
  for (i = 0 ; i < l ; i++)
  {
    csRecordedCamera* reccam = new csRecordedCamera ();
    cf->Read ((char*)&camint, sizeof (camint));
    reccam->mat.m11 = csLongToFloat (csLittleEndian::Convert (camint.m11));
    reccam->mat.m12 = csLongToFloat (csLittleEndian::Convert (camint.m12));
    reccam->mat.m13 = csLongToFloat (csLittleEndian::Convert (camint.m13));
    reccam->mat.m21 = csLongToFloat (csLittleEndian::Convert (camint.m21));
    reccam->mat.m22 = csLongToFloat (csLittleEndian::Convert (camint.m22));
    reccam->mat.m23 = csLongToFloat (csLittleEndian::Convert (camint.m23));
    reccam->mat.m31 = csLongToFloat (csLittleEndian::Convert (camint.m31));
    reccam->mat.m32 = csLongToFloat (csLittleEndian::Convert (camint.m32));
    reccam->mat.m33 = csLongToFloat (csLittleEndian::Convert (camint.m33));
    reccam->vec.x = csLongToFloat (csLittleEndian::Convert (camint.x));
    reccam->vec.y = csLongToFloat (csLittleEndian::Convert (camint.y));
    reccam->vec.z = csLongToFloat (csLittleEndian::Convert (camint.z));
    reccam->mirror = (camint.mirror != 0);
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
void WalkTest::SaveCamera (const char *fName)
{
  if (!view) return;
  iCamera *c = view->GetCamera ();
  csOrthoTransform& camtrans = c->GetTransform ();
  if (!c) return;
  const csMatrix3& m_o2t = camtrans.GetO2T ();
  const csVector3& v_o2t = camtrans.GetOrigin ();
  csString s;
  s.Format ("%.10f %.10f %.10f\n"
    "%.10f %.10f %.10f\n"
    "%.10f %.10f %.10f\n"
    "%.10f %.10f %.10f\n"
    "\"%s\"\n"
    "%d\n",
    v_o2t.x, v_o2t.y, v_o2t.z,
    m_o2t.m11, m_o2t.m12, m_o2t.m13, 
    m_o2t.m21, m_o2t.m22, m_o2t.m23, 
    m_o2t.m31, m_o2t.m32, m_o2t.m33, 
    c->GetSector ()->QueryObject ()->GetName (), 
    int(c->IsMirrored ()));

  myVFS->WriteFile (fName, s.GetData(), s.Length ());
}

bool WalkTest::LoadCamera (const char *fName)
{
  bool ok = true;
#define IFFAIL(x) if (ok && !(ok = (x)))
  IFFAIL (myVFS->Exists (fName))
    Report (CS_REPORTER_SEVERITY_ERROR,
	    "Could not open camera file '%s'!", fName);
  csRef<iDataBuffer> data;
  IFFAIL ((data = myVFS->ReadFile(fName)) != 0)
    Report (CS_REPORTER_SEVERITY_ERROR,
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
    Report (CS_REPORTER_SEVERITY_ERROR,
	    "Wrong format for camera file '%s'", fName);
  iSector* s = 0;
  IFFAIL ((s = Engine->GetSectors ()->FindByName (sector_name)) != 0)
    Report (CS_REPORTER_SEVERITY_ERROR,
	    "Sector `%s' in coordinate file does not "
	    "exist in this map!", sector_name);
  if (ok)
  {
    iCamera *c = view->GetCamera ();
    c->SetSector (s);
    c->SetMirrored (imirror != 0);
    c->GetTransform ().SetO2T (m);
    c->GetTransform ().SetOrigin (v);
    collider_actor.SetCamera (c);
    Report (CS_REPORTER_SEVERITY_NOTIFY, "Camera loaded");
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

  iBase* result;
  if (!Sys->LevelLoader->Load (filename, result))
  {
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
  	"There was an error reading model '%s'!", filename);
    return;
  }

  csRef<iMeshFactoryWrapper> wrap = scfQueryInterface<iMeshFactoryWrapper> (
  	result);
  if (wrap)
    wrap->QueryObject ()->SetName (templatename);
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
  csRef<iPluginConfig> config (scfQueryInterface<iPluginConfig> (plugin));
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
  csRef<iPluginConfig> config (scfQueryInterface<iPluginConfig> (plugin));
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
  csRef<iPluginConfig> config (scfQueryInterface<iPluginConfig> (plugin));
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



/// Looks for a key and returns its value.
csString LookForKeyValue (iObjectIterator* it,const char* key)
{
  it->Reset();
  while (it->HasNext())
  {
    csRef<iKeyValuePair> kp = scfQueryInterface<iKeyValuePair> (it->Next());
    if(!kp)
      continue;
    if(!strcmp(key,kp->GetKey()))
      return kp->GetValue ();
  }
  return csString ("");
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
  csString scaleValue = LookForKeyValue(it,"scalefactor");

  sf = atof(scaleValue);
  return sf;
}


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
    kp = scfQueryInterface<iKeyValuePair> (it->Next());
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
        csPrintf("Error loading %s texture!!",textFileName);
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
  csString modelFilename = LookForKeyValue(it,"factoryfile");
  csString materialName = LookForKeyValue(it,"factorymaterial");
  double scaleFactor = ParseScaleFactor(it);

  //We had the material needed by the factory and the model file

  load_meshobj((char*)(const char*)modelFilename,
  	(char*)(const char*)factoryName,(char*)(const char*)materialName);

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
  csString sprName		= LookForKeyValue(it,"cs_name");
  csString factName		= LookForKeyValue(it,"factory");
  //double scaleFactor	= ParseScaleFactor(it);

  iMeshWrapper* sprite = GenerateSprite((char*)(const char*)factName,
  	(char*)(const char*)sprName,sector,position);

  csRef<iSprite3DState> state (
                          scfQueryInterface<iSprite3DState> (sprite->GetMeshObject()));
  state->SetAction("default");
}



void BuildObject(iSector * sector,
	iObjectIterator* it, iEngine* Engine,
	csVector3 position, iGraphics3D* MyG3D, iLoader* loader,
	iObjectRegistry* objReg)
{
  csString factoryName;
  if(strcmp(LookForKeyValue(it,"classname"),"SEED_MESH_OBJ")) return;
  //Now we know this objects iterator belongs to a SEED_MESH_OBJECT
  //Proceeding to contruct the object

  RegisterMaterials(it,Engine,MyG3D,loader,objReg);
  factoryName = LookForKeyValue(it,"factory");
  if(!Engine->GetMeshFactories()->FindByName(factoryName))
	  BuildFactory(it, (char*)(const char*)factoryName, Engine);

  BuildSprite(sector, it, position);
}

//===========================================================================

void WalkTest::ParseKeyNodes(iObject* src)
{
  csRef<iObjectIterator> it (src->GetIterator());
  csRef<iSector> sector (scfQueryInterface<iSector> (src));

  while(it->HasNext())
  {
    iObject* node_obj = it->Next ();
    csRef<iMapNode> node (scfQueryInterface<iMapNode> (node_obj));
    if(!node)
    {
      continue;
    }
    csRef<iObjectIterator> it2 (node_obj->GetIterator());

    BuildObject(sector, it2, Engine, node->GetPosition(), myG3D,
		LevelLoader, object_reg);
  }
}

void WalkTest::ParseKeyCmds (iObject* src)
{
  csRef<iObjectIterator> it (src->GetIterator ());
  while (it->HasNext ())
  {
    csRef<iKeyValuePair> kp (
    	scfQueryInterface<iKeyValuePair> (it->Next ()));
    if (!kp)
    {
      continue;
    }
    if (!strcmp (kp->GetKey (), "cmd_AnimateSky"))
    {
      csRef<iSector> Sector (scfQueryInterface<iSector> (src));
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
      csRef<iMeshWrapper> wrap = scfQueryInterface<iMeshWrapper> (src);
      if (wrap)
        anim_dirlight = wrap;	// @@@ anim_dirlight should be csRef
    }
    else if (!strcmp (kp->GetKey (), "entity_WavePortal"))
    {
      csRef<iMeshWrapper> wrap = scfQueryInterface<iMeshWrapper> (src);
      if (wrap)
      {
        const csRef<iSceneNodeArray> ml = 
          wrap->QuerySceneNode ()->GetChildrenArray ();
	size_t i;
	iMeshWrapper* pcmesh = 0;
        for (i = 0 ; i < ml->GetSize() ; i++)
          if ((pcmesh = ml->Get(i)->QueryMesh ()))
	  {
	    break;
	  }
	if (pcmesh)
	{
	  iPortalContainer* pc = pcmesh->GetPortalContainer ();
	  if (pc)
	  {
	    char polyname[255];
	    int xyz;
	    float max_angle, speed;
	    csScanStr (kp->GetValue (), "%s,%d,%f,%f",
	  	  polyname, &xyz, &max_angle, &speed);

	    iPortal* portal = 0;
	    for (int i = 0 ; i < pc->GetPortalCount () ; i++)
	    {
	      iPortal* test = pc->GetPortal (i);
	      if (!strcmp (polyname, test->GetName ()))
	      {
		portal = test;
	        break;
	      }
	    }
	    if (portal == 0)
	    {
	      Sys->Report (CS_REPORTER_SEVERITY_WARNING,
	    	  "Cannot find a portal named '%s'!", polyname);
	    }
	    else
	    {
	      csAnimatedPortal* anportal = new csAnimatedPortal (portal,
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

  csRef<iMeshWrapper> mesh (scfQueryInterface<iMeshWrapper> (src));
  if (mesh)
  {
    size_t k;
    const csRef<iSceneNodeArray> ml = 
      mesh->QuerySceneNode ()->GetChildrenArray ();
    for (k = 0 ; k < ml->GetSize() ; k++)
    {
      iMeshWrapper* spr = ml->Get(k)->QueryMesh ();
      // @@@ Also for others?
      if (spr)
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
    csRef<csWalkEntity> wentity (
    	scfQueryInterface<csWalkEntity> (it->Next ()));
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
  if (!csStrCaseCmp (cmd, "help"))
  {
    csCommandProcessor::perform (cmd, arg);
#   undef CONPRI
#   define CONPRI(m) Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, m);
    CONPRI("-*- Additional commands -*-");
    CONPRI("Visibility:");
    CONPRI("  farplane");
    CONPRI("Lights:");
    CONPRI("  addlight dellight dellights addstlight delstlight");
    CONPRI("  clrlights setlight relight");
    CONPRI("Views:");
    CONPRI("  split_view unsplit_view toggle_view");
    CONPRI("Movement:");
    CONPRI("  step_forward step_backward strafe_left strafe_right speed");
    CONPRI("  look_up look_down rotate_left rotate_right jump move3d");
    CONPRI("  i_forward i_backward i_left i_right i_up i_down i_rotleftc");
    CONPRI("  i_rotleftw i_rotrightc i_rotrightw i_rotleftx i_rotleftz");
    CONPRI("  i_rotrightx i_rotrightz do_gravity colldet freelook");
    CONPRI("Statistics:");
    CONPRI("  perftest coordshow");
    CONPRI("Special effects:");
    CONPRI("  addmbot delmbot addbot delbot fire explosion frain decal_test");
    CONPRI("  rain snow fountain flame portal fs_inter fs_fadeout fs_fadecol");
    CONPRI("  fs_fadetxt fs_red fs_green fs_blue fs_whiteout fs_shadevert");
    CONPRI("Debugging:");
    CONPRI("  zbuf debug0 debug1 debug2 palette bugplug");
    CONPRI("  db_boxshow db_boxcam1 db_boxcam2 db_boxsize1 db_boxsize2");
    CONPRI("Meshes:");
    CONPRI("  loadmesh addmesh delmesh listmeshes listactions setaction");
    CONPRI("  objectdump objectmove objectmovex objectmovez");
    CONPRI("Various:");
    CONPRI("  coordsave coordload bind s_fog");
    CONPRI("  snd_play snd_volume record play playonce clrrec saverec");
    CONPRI("  loadrec plugins conflist confset do_logo");
    CONPRI("  varlist var setvar setvarv setvarc loadmap saveworld");

#   undef CONPRI
  }
  else if (!csStrCaseCmp (cmd, "coordsave"))
  {
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
    	"Saving camera to /tmp/walktest.cam");
    Sys->SaveCamera ("/tmp/walktest.cam");
  }
  else if (!csStrCaseCmp (cmd, "coordload"))
  {
    char const* s = "/tmp/walktest.cam"; // User-writable location.
    if (!Sys->myVFS->Exists(s))
      s = "/varia/walktest.cam";         // Potentially read-only.
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Loading camera from %s", s);
    Sys->LoadCamera (s);
  }
  else if (!csStrCaseCmp (cmd, "plugins"))
  {
    int i = 0;
    csRef<iPluginIterator> it = Sys->plugin_mgr->GetPlugins ();
    while (it->HasNext ())
    {
      iBase* plugin = it->Next ();
      csRef<iFactory> fact (scfQueryInterface<iFactory> (plugin));
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"%d: %s", i, fact->QueryDescription ());
      i++;
    }
  }
  else if (!csStrCaseCmp (cmd, "setvarc"))
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
  else if (!csStrCaseCmp (cmd, "setvarv"))
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
  else if (!csStrCaseCmp (cmd, "setvar"))
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
  else if (!csStrCaseCmp (cmd, "var"))
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
  else if (!csStrCaseCmp (cmd, "varlist"))
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
  else if (!csStrCaseCmp (cmd, "conflist"))
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
        csRef<iPluginConfig> config (scfQueryInterface<iPluginConfig> (plugin));
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
	    csString buf;
	    buf.Format ("Option %s (%s) ", odesc.name, odesc.description);
	    csVariant var;
	    config->GetOption (i, &var);
	    switch (odesc.type)
	    {
	      case CSVAR_LONG: Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	      	"%sLONG=%ld", buf.GetData(), var.GetLong ());
		break;
	      case CSVAR_BOOL: Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	      	"%sBOOL=%d", buf.GetData(), (int)var.GetBool ());
	        break;
	      case CSVAR_CMD: Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	      	"%sCMD", buf.GetData());
		break;
	      case CSVAR_FLOAT: Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	      	"%sFLOAT=%g", buf.GetData(), var.GetFloat ());
		break;
	      case CSVAR_STRING: Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	      	"%sSTRING=%s", buf.GetData(), var.GetString ());
		break;
	      default: Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	      	"%s<unknown type>", buf.GetData());
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
  else if (!csStrCaseCmp (cmd, "confset"))
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
  else if (!csStrCaseCmp (cmd, "saverec"))
  {
    if (arg)
    {
      csString buf;
      buf.Format ("/tmp/%s.rec", arg);
      SaveRecording (Sys->myVFS, buf);
    }
    else
      SaveRecording (Sys->myVFS, "/tmp/record");
  }
  else if (!csStrCaseCmp (cmd, "loadrec"))
  {
    if (arg)
    {
      csString buf;
      buf.Format ("/tmp/%s.rec", arg);
      LoadRecording (Sys->myVFS, buf);
    }
    else
      LoadRecording (Sys->myVFS, "/tmp/record");
  }
  else if (!csStrCaseCmp (cmd, "clrrec"))
  {
    Sys->recording.DeleteAll ();
    Sys->recording.SetSize (0);
  }
  else if (!csStrCaseCmp (cmd, "record"))
  {
    if (Sys->cfg_recording == -1)
    {
      Sys->cfg_playrecording = -1;
      Sys->cfg_recording = 0;
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Start recording camera movement...");
    }
    else
    {
      Sys->cfg_recording = -1;
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Stop recording.");
    }
  }
  else if (!csStrCaseCmp (cmd, "play"))
  {
    if (Sys->cfg_playrecording == -1)
    {
      Sys->cfg_recording = -1;
      Sys->cfg_playrecording = 0;
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Start playing back camera movement...");
      Sys->cfg_playloop = true;
      Sys->record_start_time = csGetTicks ();
      Sys->record_frame_count = 0;
    }
    else
    {
      Sys->cfg_playrecording = -1;
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Stop playback.");
    }
  }
  else if (!csStrCaseCmp (cmd, "playonce"))
  {
    if (Sys->cfg_playrecording == -1)
    {
      Sys->cfg_recording = -1;
      Sys->cfg_playrecording = 0;
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Start playing back camera movement once...");
      Sys->cfg_playloop = false;
      Sys->record_start_time = csGetTicks ();
      Sys->record_frame_count = 0;
    }
    else
    {
      Sys->cfg_playrecording = -1;
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Stop playback.");
    }
  }
  else if (!csStrCaseCmp (cmd, "bind"))
  {
    extern void bind_key (const char* arg);
    bind_key (arg);
  }
  else if (!csStrCaseCmp (cmd, "bugplug"))
  {
    csRef<iBase> plug = CS_LOAD_PLUGIN_ALWAYS (Sys->plugin_mgr,
    	"crystalspace.utilities.bugplug");
    plug->IncRef ();	// Avoid smart pointer release (@@@)
  }
  else if (!csStrCaseCmp (cmd, "do_logo"))
    csCommandProcessor::change_boolean (arg, &Sys->do_logo, "do_logo");
  else if (!csStrCaseCmp (cmd, "do_gravity"))
  {
    bool do_gravity = Sys->collider_actor.GetGravity () > 0.1;
    csCommandProcessor::change_boolean (arg, &do_gravity, "do_gravity");
    if (do_gravity)
    {
      Sys->collider_actor.SetGravity (9.806f);
    }
    else
    {
      Sys->collider_actor.SetGravity (0.0f);
      Sys->desired_velocity.y = 0;
      Sys->velocity.y = 0;
    }
  }
  else if (!csStrCaseCmp (cmd, "inverse_mouse"))
    csCommandProcessor::change_boolean (arg, &Sys->inverse_mouse, "inverse_mouse");
  else if (!csStrCaseCmp (cmd, "colldet"))
  {
    bool do_cd = Sys->collider_actor.HasCD ();
    csCommandProcessor::change_boolean (arg, &do_cd, "colldet");
    Sys->collider_actor.SetCD (do_cd);
  }
  else if (!csStrCaseCmp (cmd, "zbuf"))
    csCommandProcessor::change_boolean (arg, &Sys->do_show_z, "zbuf");
  else if (!csStrCaseCmp (cmd, "db_boxshow"))
    csCommandProcessor::change_boolean (arg, &Sys->do_show_debug_boxes, "show debug boxes");
  else if (!csStrCaseCmp (cmd, "db_boxcam1"))
    Sys->debug_box1.SetCenter (Sys->view->GetCamera ()->GetTransform ().GetOrigin ());
  else if (!csStrCaseCmp (cmd, "db_boxcam2"))
    Sys->debug_box2.SetCenter (Sys->view->GetCamera ()->GetTransform ().GetOrigin ());
  else if (!csStrCaseCmp (cmd, "db_boxsize1"))
  {
    float size = Sys->debug_box1.MaxX ()-Sys->debug_box1.MinX ();
    csCommandProcessor::change_float (arg, &size, "box1 size", 0.01f, 1000);
    Sys->debug_box1.SetSize (csVector3 (size, size, size));
  }
  else if (!csStrCaseCmp (cmd, "db_boxsize2"))
  {
    float size = Sys->debug_box2.MaxX ()-Sys->debug_box2.MinX ();
    csCommandProcessor::change_float (arg, &size, "box2 size", 0.01f, 1000);
    Sys->debug_box2.SetSize (csVector3 (size, size, size));
  }
  else if (!csStrCaseCmp (cmd, "palette"))
    csCommandProcessor::change_boolean (arg, &Sys->do_show_palette, "palette");
  else if (!csStrCaseCmp (cmd, "move3d"))
    csCommandProcessor::change_boolean (arg, &Sys->move_3d, "move3d");
  else if (!csStrCaseCmp (cmd, "speed"))
  {
    bool speed = Sys->cfg_walk_maxspeed_multreal > 1.1f;
    csCommandProcessor::change_boolean (arg, &speed, "speed");
    if (speed)
      Sys->cfg_walk_maxspeed_multreal = Sys->cfg_walk_maxspeed_mult;
    else
      Sys->cfg_walk_maxspeed_multreal = 1.0f;
  }
  else if (!csStrCaseCmp (cmd, "freelook"))
  {
    csCommandProcessor::change_boolean (arg, &Sys->do_freelook, "freelook");
    if (Sys->do_freelook)
      Sys->myG2D->SetMousePosition (FRAME_WIDTH / 2, FRAME_HEIGHT / 2);
    Sys->myG2D->SetMouseCursor (Sys->do_freelook?csmcNone:csmcArrow);
  }
  else if (!csStrCaseCmp (cmd, "coordshow"))
  {
    csCommandProcessor::change_boolean (arg, &Sys->do_show_coord, "coordshow");
  }
  else if (!csStrCaseCmp (cmd, "objectdump"))
  {
    if (Sys->closestMesh)
    {
      csReversibleTransform& tr = Sys->closestMesh->GetMovable ()
	  ->GetTransform ();
      printf ("mesh: %s\n", Sys->closestMesh->QueryObject ()->GetName ());
      printf ("pos: %g,%g,%g\n", tr.GetOrigin ().x, tr.GetOrigin ().y,
	  tr.GetOrigin ().z);
      printf ("matrix: %g,%g,%g\n",
	  tr.GetO2T ().m11, tr.GetO2T ().m12, tr.GetO2T ().m13);
      printf ("        %g,%g,%g\n",
	  tr.GetO2T ().m21, tr.GetO2T ().m22, tr.GetO2T ().m23);
      printf ("        %g,%g,%g\n",
	  tr.GetO2T ().m31, tr.GetO2T ().m32, tr.GetO2T ().m33);
      fflush (stdout);
    }
    else
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"No object selected!");
    }
  }
  else if (!csStrCaseCmp (cmd, "objectmove"))
  {
    csCommandProcessor::change_boolean (arg, &Sys->do_object_move,
	"objectmove");
  }
  else if (!csStrCaseCmp (cmd, "objectmovex"))
  {
    if (Sys->do_object_move)
    {
      float f = safe_atof (arg);
      if (f && Sys->closestMesh)
      {
	csReversibleTransform& tr = Sys->closestMesh->GetMovable ()
	  ->GetTransform ();
	csVector3 v (-f, 0, 0);
	tr.Translate (
	    Sys->view->GetCamera ()->GetTransform ().This2OtherRelative (v));
	Sys->closestMesh->GetMovable ()->UpdateMove ();
      }
    }
  }
  else if (!csStrCaseCmp (cmd, "objectmovez"))
  {
    if (Sys->do_object_move)
    {
      float f = safe_atof (arg);
      if (f && Sys->closestMesh)
      {
	csReversibleTransform& tr = Sys->closestMesh->GetMovable ()
	  ->GetTransform ();
	csVector3 v (0, 0, f);
	tr.Translate (
	    Sys->view->GetCamera ()->GetTransform ().This2OtherRelative (v));
	Sys->closestMesh->GetMovable ()->UpdateMove ();
      }
    }
  }
  else if (!csStrCaseCmp (cmd, "s_fog"))
  {
    if (!arg)
    {
      const csFog& f = Sys->view->GetCamera ()->GetSector ()->GetFog ();
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Fog in current sector (%f,%f,%f) density=%f",
      	f.color.red, f.color.green, f.color.blue, f.density);
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
      csFog f;
      f.density = dens;
      f.color.red = r;
      f.color.green = g;
      f.color.blue = b;
      f.mode = CS_FOG_MODE_CRYSTALSPACE;
      Sys->view->GetCamera ()->GetSector ()->SetFog (f);
    }
  }
  else if (!csStrCaseCmp (cmd, "loadmap"))
  {
    if (arg)
    {
      char level[300];
      csScanStr (arg, "%s", level);
      if (!Sys->SetMapDir (level))
      {
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	  "Couldn't open level '%s'!", level);
	return false;
      }
      csRef<iEngineSequenceManager> engseq = csQueryRegistry<
	iEngineSequenceManager> (Sys->object_reg);
      if (engseq)
      {
	engseq->RemoveTriggers ();
	engseq->RemoveSequences ();
      }
      Sys->Engine->DeleteAll ();
      Sys->Engine->SetVFSCacheManager ();
      if (!Sys->LevelLoader->LoadMapFile ("world"))
      {
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	  "Couldn't load level '%s'!", level);
	return false;
      }
      Sys->Engine->Prepare ();
      // Look for the start sector in this map.
      bool camok = false;
      if (!camok && Sys->Engine->GetCameraPositions ()->GetCount () > 0)
      {
        iCameraPosition *cp = Sys->Engine->GetCameraPositions ()->Get (0);
        if (cp->Load(Sys->views[0]->GetCamera (), Sys->Engine) &&
	    cp->Load(Sys->views[1]->GetCamera (), Sys->Engine))
	  camok = true;
      }
      if (!camok)
      {
        iSector* room = Sys->Engine->GetSectors ()->FindByName ("room");
        if (room)
        {
	  Sys->views[0]->GetCamera ()->SetSector (room);
	  Sys->views[1]->GetCamera ()->SetSector (room);
	  Sys->views[0]->GetCamera ()->GetTransform ().SetOrigin (
	      csVector3 (0, 0, 0));
	  Sys->views[1]->GetCamera ()->GetTransform ().SetOrigin (
	      csVector3 (0, 0, 0));
	  camok = true;
        }
      }
      if (!camok)
      {
        Sys->Report (CS_REPORTER_SEVERITY_ERROR,
          "Map does not contain a valid starting point!\n"
          "Try adding a room called 'room' or a START keyword");
        return false;
      }
      Sys->InitCollDet (Sys->Engine, 0);
    }
  }
  else if (!csStrCaseCmp (cmd, "portal"))
  {
    if (arg)
    {
      char level[300];
      csScanStr (arg, "%s", level);
      void OpenPortal (iLoader*, iView* view, char* lev);
      OpenPortal (Sys->LevelLoader, Sys->view, level);
    }
    else
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Expected parameter 'level'!");
  }
  else if (!csStrCaseCmp (cmd, "fs_inter"))
  {
    Sys->do_fs_inter = !Sys->do_fs_inter;
    if (Sys->do_fs_inter)
    {
      Sys->fs_inter_amount = 0.3f;
      Sys->fs_inter_length = 30;
      Sys->fs_inter_anim = 0;
      if (arg)
        csScanStr (arg, "%f,%f", &Sys->fs_inter_amount, &Sys->fs_inter_length);
    }
  }
  else if (!csStrCaseCmp (cmd, "fs_fadeout"))
  {
    Sys->do_fs_fadeout = !Sys->do_fs_fadeout;
    if (Sys->do_fs_fadeout)
    {
      Sys->fs_fadeout_fade = 0;
      Sys->fs_fadeout_dir = true;
    }
  }
  else if (!csStrCaseCmp (cmd, "fs_fadecol"))
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
  else if (!csStrCaseCmp (cmd, "fs_fadetxt"))
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
        Sys->fs_fadetxt_txt = mat->GetMaterial()->GetTexture ();
      }
      else
      {
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
		"Can't find material!");
	Sys->do_fs_fadetxt = false;
      }
    }
  }
  else if (!csStrCaseCmp (cmd, "fs_red"))
  {
    Sys->do_fs_red = !Sys->do_fs_red;
    if (Sys->do_fs_red)
    {
      Sys->fs_red_fade = 0;
      Sys->fs_red_dir = true;
    }
  }
  else if (!csStrCaseCmp (cmd, "fs_green"))
  {
    Sys->do_fs_green = !Sys->do_fs_green;
    if (Sys->do_fs_green)
    {
      Sys->fs_green_fade = 0;
      Sys->fs_green_dir = true;
    }
  }
  else if (!csStrCaseCmp (cmd, "fs_blue"))
  {
    Sys->do_fs_blue = !Sys->do_fs_blue;
    if (Sys->do_fs_blue)
    {
      Sys->fs_blue_fade = 0;
      Sys->fs_blue_dir = true;
    }
  }
  else if (!csStrCaseCmp (cmd, "fs_whiteout"))
  {
    Sys->do_fs_whiteout = !Sys->do_fs_whiteout;
    if (Sys->do_fs_whiteout)
    {
      Sys->fs_whiteout_fade = 0;
      Sys->fs_whiteout_dir = true;
    }
  }
  else if (!csStrCaseCmp (cmd, "fs_shadevert"))
  {
    Sys->do_fs_shadevert = !Sys->do_fs_shadevert;
    if (Sys->do_fs_shadevert)
    {
      float tr = 1, tg = 0, tb = 0, br = 0, bg = 0, bb = 1;
      if (arg) csScanStr (arg, "%f,%f,%f,%f,%f,%f",
      	&tr, &tg, &tb, &br, &bg, &bb);
      Sys->fs_shadevert_topcol.Set (tr, tg, tb);
      Sys->fs_shadevert_botcol.Set (br, bg, bb);
    }
  }
  else if (!csStrCaseCmp (cmd, "perftest"))
  {
    int num = 200;
    if (arg)
    {
      csScanStr (arg, "%d", &num);
      RECORD_ARGS (cmd, arg);
    }
    else
    {
      RECORD_CMD (cmd);
    }
    perf_test (num);
  }
  else if (!csStrCaseCmp (cmd, "debug0"))
  {
  }
  else if (!csStrCaseCmp (cmd, "debug1"))
  {
  }
  else if (!csStrCaseCmp (cmd, "debug2"))
  {
  }
  else if (!csStrCaseCmp (cmd, "strafe_left"))
  {
    float f = safe_atof (arg);
    if (Sys->move_3d) { if (f) Sys->imm_left (0.1f, false, false); }
    else Sys->Strafe (-1*f);
  }
  else if (!csStrCaseCmp (cmd, "strafe_right"))
  {
    float f = safe_atof (arg);
    if (Sys->move_3d) { if (f) Sys->imm_right (0.1f, false, false); }
    else Sys->Strafe (1*f);
  }
  else if (!csStrCaseCmp (cmd, "step_forward"))
  {
    float f = safe_atof (arg);
    if (Sys->do_object_move)
    {
      if (f && Sys->closestMesh)
      {
	csReversibleTransform& tr = Sys->closestMesh->GetMovable ()
	  ->GetTransform ();
	tr.Translate (csVector3 (0, Sys->object_move_speed, 0));
	Sys->closestMesh->GetMovable ()->UpdateMove ();
      }
    }
    else
    {
      if (Sys->move_3d) { if (f) Sys->imm_forward (0.1f, false, false); }
      else Sys->Step (1*f);
    }
  }
  else if (!csStrCaseCmp (cmd, "step_backward"))
  {
    float f = safe_atof (arg);
    if (Sys->do_object_move)
    {
      if (f && Sys->closestMesh)
      {
	csReversibleTransform& tr = Sys->closestMesh->GetMovable ()
	  ->GetTransform ();
	tr.Translate (csVector3 (0, -Sys->object_move_speed, 0));
	Sys->closestMesh->GetMovable ()->UpdateMove ();
      }
    }
    else
    {
      if (Sys->move_3d) { if (f) Sys->imm_backward (0.1f, false, false); }
      else Sys->Step (-1*f);
    }
  }
  else if (!csStrCaseCmp (cmd, "rotate_left"))
  {
    float f = safe_atof (arg);
    if (Sys->do_object_move)
    {
      if (f && Sys->closestMesh)
      {
	csReversibleTransform& tr = Sys->closestMesh->GetMovable ()
	  ->GetTransform ();
	tr.RotateThis (csVector3 (0, 1, 0), -Sys->object_move_speed);
	Sys->closestMesh->GetMovable ()->UpdateMove ();
      }
    }
    else
    {
      if (Sys->move_3d)
      { if (f) Sys->imm_rot_left_camera (0.1f, false, false); }
      else Sys->Rotate (-1*f);
    }
  }
  else if (!csStrCaseCmp (cmd, "rotate_right"))
  {
    float f = safe_atof (arg);
    if (Sys->do_object_move)
    {
      if (f && Sys->closestMesh)
      {
	csReversibleTransform& tr = Sys->closestMesh->GetMovable ()
	  ->GetTransform ();
	tr.RotateThis (csVector3 (0, 1, 0), Sys->object_move_speed);
	Sys->closestMesh->GetMovable ()->UpdateMove ();
      }
    }
    else
    {
      if (Sys->move_3d)
      { if (f) Sys->imm_rot_right_camera (0.1f, false, false); }
      else Sys->Rotate (1*f);
    }
  }
  else if (!csStrCaseCmp (cmd, "look_up"))
  {
    float f = safe_atof (arg);
    if (Sys->move_3d) { if (f) Sys->imm_rot_right_xaxis (0.1f, false, false); }
    else Sys->Look (1*f);
  }
  else if (!csStrCaseCmp (cmd, "look_down"))
  {
    float f = safe_atof (arg);
    if (Sys->move_3d) { if (f) Sys->imm_rot_left_xaxis (0.1f, false, false); }
    else Sys->Look (-1*f);
  }
  else if (!csStrCaseCmp (cmd, "jump"))
  {
    Sys->Jump ();
  }
  else if (!csStrCaseCmp (cmd, "i_forward"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_forward (0.1f, (slow != 0), (fast != 0));
  }
  else if (!csStrCaseCmp (cmd, "i_backward"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_backward (0.1f, (slow != 0), (fast != 0));
  }
  else if (!csStrCaseCmp (cmd, "i_left"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_left (0.1f, (slow != 0), (fast != 0));
  }
  else if (!csStrCaseCmp (cmd, "i_right"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_right (0.1f, (slow != 0), (fast != 0));
  }
  else if (!csStrCaseCmp (cmd, "i_up"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_up (0.1f, (slow != 0), (fast != 0));
  }
  else if (!csStrCaseCmp (cmd, "i_down"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_down (0.1f, (slow != 0), (fast != 0));
  }
  else if (!csStrCaseCmp (cmd, "i_rotleftc"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_rot_left_camera (0.1f, (slow != 0), (fast != 0));
  }
  else if (!csStrCaseCmp (cmd, "i_rotleftw"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_rot_left_world (0.1f, (slow != 0), (fast != 0));
  }
  else if (!csStrCaseCmp (cmd, "i_rotrightc"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_rot_right_camera (0.1f, (slow != 0), (fast != 0));
  }
  else if (!csStrCaseCmp (cmd, "i_rotrightw"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_rot_right_world (0.1f, (slow != 0), (fast != 0));
  }
  else if (!csStrCaseCmp (cmd, "i_rotleftx"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_rot_left_xaxis (0.1f, (slow != 0), (fast != 0));
  }
  else if (!csStrCaseCmp (cmd, "i_rotleftz"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_rot_left_zaxis (0.1f, (slow != 0), (fast != 0));
  }
  else if (!csStrCaseCmp (cmd, "i_rotrightx"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_rot_right_xaxis (0.1f, (slow != 0), (fast != 0));
  }
  else if (!csStrCaseCmp (cmd, "i_rotrightz"))
  {
    int slow = 0, fast = 0; if (arg) csScanStr (arg, "%d,%d", &slow, &fast);
    Sys->imm_rot_right_zaxis (0.1f, (slow != 0), (fast != 0));
  }
  else if (!csStrCaseCmp (cmd, "fire"))
  {
    RECORD_CMD (cmd);
    extern void fire_missile ();
    fire_missile ();
  }
  else if (!csStrCaseCmp (cmd, "decal_test"))
  {
    extern void test_decal();
    test_decal ();
  }
  else if (!csStrCaseCmp (cmd, "lightning"))
  {
    extern void show_lightning ();
    show_lightning ();
  }
  else if (!csStrCaseCmp (cmd, "rain"))
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
  else if (!csStrCaseCmp (cmd, "frain"))
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
  else if (!csStrCaseCmp (cmd, "snow"))
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
    if (cnt <= 2) speed = 0.3f;
    if (cnt <= 1) num = 500;
    if (cnt <= 0) strcpy (txtname, "snow");
    add_particles_snow (Sys->view->GetCamera ()->GetSector (),
    	txtname, num, speed);
  }
  else if (!csStrCaseCmp (cmd, "flame"))
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
  else if (!csStrCaseCmp (cmd, "fountain"))
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
  else if (!csStrCaseCmp (cmd, "explosion"))
  {
    char txtname[100];
    int cnt = 0;
    if (arg) cnt = csScanStr (arg, "%s", txtname);
    extern void add_particles_explosion (iSector* sector,
    	iEngine* engine, const csVector3& center,
    	char* txtname);
    if (cnt != 1)
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
      	"Expected parameter 'texture'!");
    }
    else
      add_particles_explosion (Sys->view->GetCamera ()->GetSector (),
    	Sys->Engine,
	Sys->view->GetCamera ()->GetTransform ().GetOrigin (), txtname);
  }
  else if (!csStrCaseCmp (cmd, "loadmesh"))
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
  else if (!csStrCaseCmp (cmd, "addmesh"))
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
  else if (!csStrCaseCmp (cmd, "delmesh"))
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
  else if (!csStrCaseCmp (cmd, "listmeshes"))
  {
    list_meshes ();
  }
  else if(!csStrCaseCmp(cmd, "listactions"))
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
        csRef<iSprite3DFactoryState> fstate (
          scfQueryInterface<iSprite3DFactoryState> (
            wrap->GetMeshObject ()->GetFactory ()));
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
  else if (!csStrCaseCmp (cmd, "setaction"))
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
		
		scfQueryInterface<iSprite3DState> (wrap->GetMeshObject ()));
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
  else if (!csStrCaseCmp (cmd, "addmbot"))
  {
    RECORD_ARGS (cmd, arg);
    float radius = 0;
    if (arg) csScanStr (arg, "%f", &radius);
    Sys->add_bot (2, Sys->view->GetCamera ()->GetSector (),
    	Sys->view->GetCamera ()->GetTransform ().GetOrigin (), radius,
	true);
  }
  else if (!csStrCaseCmp (cmd, "addbot"))
  {
    RECORD_ARGS (cmd, arg);
    float radius = 0;
    if (arg) csScanStr (arg, "%f", &radius);
    Sys->add_bot (2, Sys->view->GetCamera ()->GetSector (),
    	Sys->view->GetCamera ()->GetTransform ().GetOrigin (), radius);
  }
  else if (!csStrCaseCmp (cmd, "delbot"))
  {
    Sys->del_bot ();
  }
  else if (!csStrCaseCmp (cmd, "delmbot"))
  {
    Sys->del_bot (true);
  }
  else if (!csStrCaseCmp (cmd, "clrlights"))
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
  else if (!csStrCaseCmp (cmd, "setlight"))
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
  else if (!csStrCaseCmp (cmd, "addlight"))
  {
    RECORD_ARGS (cmd, arg);
    csVector3 dir (0,0,0);
    csVector3 pos = Sys->view->GetCamera ()->GetTransform ().This2Other (dir);
    csRef<iLight> dyn;

    bool rnd;
    float r, g, b, radius;
    if (arg && csScanStr (arg, "%f,%f,%f,%f", &r, &g, &b, &radius) == 4)
    {
      dyn = Sys->view->GetEngine ()->CreateLight ("", pos,
      	radius, csColor (r, g, b), CS_LIGHT_DYNAMICTYPE_DYNAMIC);
      rnd = false;
    }
    else
    {
      dyn = Sys->view->GetEngine ()->CreateLight ("", pos,
      	6, csColor (1, 1, 1), CS_LIGHT_DYNAMICTYPE_DYNAMIC);
      rnd = true;
    }
    iLightList* ll = Sys->view->GetCamera ()->GetSector ()->GetLights ();
    ll->Add (dyn);
    dyn->Setup ();
    Sys->dynamic_lights.Push (dyn);
    // @@@ BUG: for some reason it is needed to call Setup() twice!!!!
    dyn->Setup ();
    extern void AttachRandomLight (iLight* light);
    if (rnd)
      AttachRandomLight (dyn);
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Dynamic light added.");
  }
  else if (!csStrCaseCmp (cmd, "delstlight"))
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
      Sys->view->GetEngine ()->RemoveLight (l);
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Static light removed.");
    }
  }
  else if (!csStrCaseCmp (cmd, "addstlight"))
  {
    RECORD_ARGS (cmd, arg);
    csVector3 dir (0,0,0);
    csVector3 pos = Sys->view->GetCamera ()->GetTransform ().This2Other (dir);
    csRef<iLight> light;

    float r, g, b, radius;
    char name[255];
    if (arg && csScanStr (arg, "%s,%f,%f,%f,%f", name, &r, &g, &b,
    	&radius) == 5)
    {
      light = Sys->view->GetEngine ()->CreateLight (name,
        pos, radius, csColor (r, g, b), CS_LIGHT_DYNAMICTYPE_PSEUDO);
    }
    else
    {
      light = Sys->view->GetEngine ()->CreateLight ("deflight",
        pos, 12, csColor (0, 0, 1), CS_LIGHT_DYNAMICTYPE_PSEUDO);
    }
    iLightList* ll = Sys->view->GetCamera ()->GetSector ()->GetLights ();
    ll->Add (light);
    Sys->view->GetEngine ()->ForceRelight (light);
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Static light added.");
  }
  else if (!csStrCaseCmp (cmd, "dellight"))
  {
    RECORD_CMD (cmd);
    iLightList* ll = Sys->view->GetCamera ()->GetSector ()->GetLights ();
    int i;
    for (i = 0 ; i < ll->GetCount () ; i++)
    {
      iLight* l = ll->Get (i);
      if (l->GetDynamicType () == CS_LIGHT_DYNAMICTYPE_DYNAMIC)
      {
        ll->Remove (l);
	size_t j;
	for (j = 0 ; j < Sys->dynamic_lights.GetSize () ; j++)
	{
	  if (Sys->dynamic_lights[j] == l)
	  {
	    Sys->dynamic_lights.DeleteIndex (j);
	    break;
	  }
	}
	Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Dynamic light removed.");
        break;
      }
    }
  }
  else if (!csStrCaseCmp (cmd, "dellights"))
  {
    RECORD_CMD (cmd);
    iLightList* ll = Sys->view->GetCamera ()->GetSector ()->GetLights ();
    int i;
    for (i = 0 ; i < ll->GetCount () ; i++)
    {
      iLight* l = ll->Get (i);
      if (l->GetDynamicType () == CS_LIGHT_DYNAMICTYPE_DYNAMIC)
      {
        ll->Remove (l);
	size_t j;
	for (j = 0 ; j < Sys->dynamic_lights.GetSize () ; j++)
	{
	  if (Sys->dynamic_lights[j] == l)
	  {
	    Sys->dynamic_lights.DeleteIndex (j);
	    break;
	  }
	}
	i--;
      }
    }
    Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "All dynamic lights deleted.");
  }
  else if (!csStrCaseCmp (cmd, "relight"))
  {
    csRef<iConsoleOutput> console = csQueryRegistry<iConsoleOutput> (Sys->object_reg);
    if(console.IsValid())
    {
      csTextProgressMeter* meter = new csTextProgressMeter(console);
      Sys->Engine->ForceRelight (0, meter);
      delete meter;
    }
  }
  else if (!csStrCaseCmp (cmd, "snd_play"))
  {
    if (Sys->mySound)
    {
      csRef<iSndSysManager> mgr = csQueryRegistry<iSndSysManager> (
	Sys->object_reg);
      iSndSysWrapper* sb = mgr->FindSoundByName (arg);
      if (sb)
      {
	csRef<iSndSysStream> sndstream = Sys->mySound->CreateStream (
	    sb->GetData (), CS_SND3D_ABSOLUTE);
	csRef<iSndSysSource> sndsource = Sys->mySound->CreateSource (
	    sndstream);
	sndsource->SetVolume (1.0f);
        sndstream->SetLoopState (CS_SNDSYS_STREAM_DONTLOOP);
        sndstream->Unpause ();
      }
      else
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
		"Sound '%s' not found!", arg);
    }
  }
  else if (!csStrCaseCmp (cmd, "snd_volume"))
  {
    if (Sys->mySound)
    {
      float vol = Sys->mySound->GetVolume ();
      csCommandProcessor::change_float (arg, &vol, "snd_volume", 0.0, 1.0);
      Sys->mySound->SetVolume (vol);
    }
  }
  else if (!csStrCaseCmp (cmd, "fullscreen"))
    Sys->myG2D->PerformExtension("fullscreen");
  else if (!csStrCaseCmp(cmd, "split_view"))
  {
    if (Sys->split == -1)
    {	
        csBox2 bbox;
        BoundingBoxForView(Sys->view, &bbox);
        
        int width = csQint(bbox.MaxX() - bbox.MinX());
        int height = csQint(bbox.MaxY() - bbox.MinY());
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
  else if (!csStrCaseCmp(cmd, "unsplit_view"))
  {
    if (Sys->split != -1)
    {
        csBox2 bbox1, bbox2;
        BoundingBoxForView(Sys->views[0], &bbox1);
        BoundingBoxForView(Sys->views[1], &bbox2);

        int width = csQint(bbox2.MaxX() - bbox1.MinX());
        int height = csQint(bbox1.MaxY() - bbox1.MinY());
        Sys->view->SetRectangle((int)bbox1.MinX(), (int)bbox1.MinY(), width, height);
        Sys->view->GetCamera()->SetPerspectiveCenter(bbox1.MinX() + (width / 2), 
                                                    bbox2.MinY() + (height / 2));
        Sys->split = -1;
	Sys->collider_actor.SetCamera (Sys->view->GetCamera ());
        Sys->Report(CS_REPORTER_SEVERITY_NOTIFY, "Unsplitting view");
    }
  }
  else if (!csStrCaseCmp(cmd, "toggle_view"))
  {
    if (Sys->split != -1)
    {
        Sys->split = (Sys->split + 1) % 2;
        Sys->view = Sys->views[Sys->split];
	Sys->collider_actor.SetCamera (Sys->view->GetCamera ());
        Sys->Report(CS_REPORTER_SEVERITY_NOTIFY, "Switching to view %d", Sys->split);
    }
  }
  else if (!csStrCaseCmp(cmd, "farplane"))
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
  else if (!csStrCaseCmp (cmd, "saveworld"))
  {
    if (!arg)
    {
      Sys->Report(CS_REPORTER_SEVERITY_NOTIFY, "saveworld: Excepted VFS file path.");
      return true;
    }
    if (Sys->myVFS->Exists (arg))
    {
      Sys->Report(CS_REPORTER_SEVERITY_NOTIFY,
        "saveworld: Specified file `%s' already exists.", arg);
      return true;
    }
    csRef<iSaver> saver = csQueryRegistry<iSaver> (Sys->object_reg);
    if (!saver.IsValid ())
    {
      saver = CS_LOAD_PLUGIN(Sys->plugin_mgr, "crystalspace.level.saver", iSaver);
      if (!saver.IsValid ())
      {
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
          "saveworld: Unable to load crystalspace.level.saver plugin!");
        return true;
      }
      Sys->object_reg->Register (saver, "iSaver");
    }
    if (saver->SaveMapFile (arg))
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "saveworld: Saved world to `%s'", arg);
    else
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "saveworld: Error saving map file!");
  }
  else if (!csStrCaseCmp (cmd, "cubemapshots"))
  {
    csRef<iImageIO> iio = csQueryRegistry<iImageIO> (Sys->object_reg);
    int dim = MIN (Sys->myG3D->GetWidth (), Sys->myG3D->GetHeight ());
    dim = csFindNearestPowerOf2 (dim + 1) >> 1;
    int g2dh = Sys->myG3D->GetHeight ();

    csRef<iView> sideView;
    sideView.AttachNew (new csView (Sys->Engine, Sys->myG3D));
    sideView->GetCamera()->SetSector (Sys->view->GetCamera()->GetSector());
    sideView->GetCamera()->SetTransform (Sys->view->GetCamera()->GetTransform());
    sideView->GetCamera()->SetFOVAngle (90, dim);
    sideView->GetCamera()->SetPerspectiveCenter (dim / 2, dim / 2);
    
    sideView->SetRectangle (0, 0, dim, dim);
    int cMinX, cMinY, cMaxX, xMaxY;
    Sys->myG2D->GetClipRect (cMinX, cMinY, cMaxX, xMaxY);
    Sys->myG2D->SetClipRect (0, 0, Sys->myG3D->GetWidth(), g2dh);

    static const csVector3 lookAtVecs[12] = {
      csVector3 ( 0.0f,  0.0f,  1.0f), csVector3 (0.0f, 1.0f,  0.0f), 
      csVector3 ( 0.0f,  0.0f, -1.0f), csVector3 (0.0f, 1.0f,  0.0f), 
      csVector3 ( 1.0f,  0.0f,  0.0f), csVector3 (0.0f, 1.0f,  0.0f), 
      csVector3 (-1.0f,  0.0f,  0.0f), csVector3 (0.0f, 1.0f,  0.0f), 
      csVector3 ( 0.0f,  1.0f,  0.0f), csVector3 (0.0f, 0.0f, -1.0f), 
      csVector3 ( 0.0f, -1.0f,  0.0f), csVector3 (0.0f, 0.0f, -1.0f)  
    };
    static const char* fnSuffix[] = {"pz", "nz", "px", "nx", "py", "ny"};

    for (int i = 0; i < 6; i++)
    {
      if (!Sys->myG3D->BeginDraw (Sys->Engine->GetBeginDrawFlags () 
        | CSDRAW_3DGRAPHICS | CSDRAW_CLEARZBUFFER))
      {
	Sys->myG2D->SetClipRect (cMinX, cMinY, cMaxX, xMaxY);
	return true;
      }

      csOrthoTransform& camTF = sideView->GetCamera()->GetTransform();
      camTF.LookAt (lookAtVecs[i*2], lookAtVecs[i*2 + 1]);
      sideView->Draw ();

      Sys->myG3D->FinishDraw ();
      Sys->myG3D->Print (0);

      csString fn;
      fn.Format ("/tmp/cube_%s.png", fnSuffix[i]);
      csRef<iImage> shot = Sys->myG2D->ScreenShot();
      shot = csImageManipulate::Crop (shot, 0, g2dh - dim, dim, dim);
      csRef<iDataBuffer> data = iio->Save (shot, "image/png");
      if (Sys->myVFS->WriteFile (fn, data->GetData(), data->GetSize()))
	Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, "Written '%s'...",
	  fn.GetData());
    }
    
    Sys->myG2D->SetClipRect (cMinX, cMinY, cMaxX, xMaxY);
  }
  else if (!csStrCaseCmp (cmd, "callstack"))
  {
    csCallStack* stack = csCallStackHelper::CreateCallStack();
    if (stack == 0)
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
        "callstack not available");
    }
    else
    {
      for (size_t i = 0; i < stack->GetEntryCount(); i++)
      {
	Sys->Report (CS_REPORTER_SEVERITY_NOTIFY,
	  "%s", stack->GetEntryAll (i).GetData());
      }
      stack->Free();
    }
  }
  else if (!csStrCaseCmp (cmd, "bugplugcmd"))
  {
    csRef<iBugPlug> bugplug = csQueryRegistry<iBugPlug> (Sys->object_reg);
    if (!bugplug.IsValid())
    {
      csRef<iPluginManager> plugmgr = 
        csQueryRegistry<iPluginManager> (Sys->object_reg);
      bugplug = csQueryPluginClass<iBugPlug> (plugmgr, 
        "crystalspace.utilities.bugplug");
    }
    if (!bugplug.IsValid())
    {
      Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, 
        "Could not get bugplug interface");
    }
    else
    {
      bool success = bugplug->ExecCommand (arg);
      if (!success)
        Sys->Report (CS_REPORTER_SEVERITY_NOTIFY, 
        "bugplug failed to execute '%s'", arg);
    }
  }
  else
    return false;

  return true;
}

#undef RECORD_CMD
#undef RECORD_ARGS
