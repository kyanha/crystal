/*
Copyright (C) 2003 by Keith Fulton

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
#include "sprcal3d.h"
#include "csgeom/polyclip.h"
#include "csgeom/quaterni.h"
#include "csgeom/sphere.h"
#include "csutil/garray.h"
#include "csutil/randomgen.h"
#include "ivideo/graph3d.h"
#include "ivideo/vbufmgr.h"
#include "iengine/camera.h"
#include "iengine/rview.h"
#include "iengine/movable.h"
#include "iengine/light.h"
#include "iengine/engine.h"
#include "iengine/sector.h"
#include "ivaria/reporter.h"
#include "iutil/objreg.h"
#include "iutil/cache.h"
#include "iutil/object.h"
#include "iutil/vfs.h"
#include "csutil/memfile.h"
#include "csutil/csmd5.h"
#include "iengine/mesh.h"
#include "csutil/csendian.h"
#include "qsqrt.h"

// STL include required by cal3d
#include <string>

CS_IMPLEMENT_PLUGIN

static void ReportCalError (iObjectRegistry* objreg, const char* msgId, 
			    const char* msg)
{
  csString text;

  if (msg && (*msg != 0))
    text << msg << " [";
  text << "Cal3d: " << CalError::getLastErrorDescription().data();

  if (CalError::getLastErrorText ().size () > 0)
  {
    text << " '" << CalError::getLastErrorText().data() << "'";
  }

  text << " in " << CalError::getLastErrorFile().data() << "(" << 
    CalError::getLastErrorLine ();
  if (msg && (*msg != 0))
    text << "]";

  csReport (objreg, CS_REPORTER_SEVERITY_ERROR, msgId,
    text);
}

//--------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csSpriteCal3DSocket)
  SCF_IMPLEMENTS_INTERFACE (iSpriteCal3DSocket)
SCF_IMPLEMENT_IBASE_END

csSpriteCal3DSocket::csSpriteCal3DSocket()
{
  SCF_CONSTRUCT_IBASE (0);
  triangle_index = 0;
  submesh_index = 0;
  mesh_index = 0;
  name = 0;
  attached_mesh = 0;
}

csSpriteCal3DSocket::~csSpriteCal3DSocket ()
{
  delete [] name;
  SCF_DESTRUCT_IBASE ();
}

void csSpriteCal3DSocket::SetName (char const* n)
{
  delete [] name;
  if (n)
  {
    name = new char [strlen (n)+1];
    strcpy (name, n);
  }
  else
    name = 0;
}


//--------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csSpriteCal3DMeshObjectFactory)
SCF_IMPLEMENTS_INTERFACE (iMeshObjectFactory)
SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iSpriteCal3DFactoryState)
SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iLODControl)
SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObjectModel)
{
  static scfInterfaceID iPolygonMesh_scfID = (scfInterfaceID)-1;		
  if (iPolygonMesh_scfID == (scfInterfaceID)-1)				
    iPolygonMesh_scfID = iSCF::SCF->GetInterfaceID ("iPolygonMesh");		
  if (iInterfaceID == iPolygonMesh_scfID &&				
    scfCompatibleVersion(iVersion, scfInterface<iPolygonMesh>::GetVersion()))
  {
    printf ("Deprecated feature use: iPolygonMesh queried from Sprite3d "
      "factory; use iObjectModel->GetPolygonMeshColldet() instead.\n");
    iPolygonMesh* Object = scfiObjectModel.GetPolygonMeshColldet();
    (Object)->IncRef ();						
    return CS_STATIC_CAST(iPolygonMesh*, Object);				
  }
}
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSpriteCal3DMeshObjectFactory::SpriteCal3DFactoryState)
SCF_IMPLEMENTS_INTERFACE (iSpriteCal3DFactoryState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSpriteCal3DMeshObjectFactory::LODControl)
SCF_IMPLEMENTS_INTERFACE (iLODControl)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSpriteCal3DMeshObjectFactory::ObjectModel)
SCF_IMPLEMENTS_INTERFACE (iObjectModel)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csStringID csSpriteCal3DMeshObjectFactory::vertex_name = csInvalidStringID;
csStringID csSpriteCal3DMeshObjectFactory::texel_name = csInvalidStringID;
csStringID csSpriteCal3DMeshObjectFactory::normal_name = csInvalidStringID;
csStringID csSpriteCal3DMeshObjectFactory::color_name = csInvalidStringID;
csStringID csSpriteCal3DMeshObjectFactory::index_name = csInvalidStringID;

void csSpriteCal3DMeshObjectFactory::Report (int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csRef<iReporter> rep (CS_QUERY_REGISTRY (object_reg, iReporter));
  if (rep)
    rep->ReportV (severity, "crystalspace.mesh.sprite.cal3d", msg, arg);
  else
  {
    csPrintfV (msg, arg);
    csPrintf ("\n");
  }
  va_end (arg);
}

csSpriteCal3DMeshObjectFactory::csSpriteCal3DMeshObjectFactory (
  iBase *pParent, iObjectRegistry* object_reg)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSpriteCal3DFactoryState);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiLODControl);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObjectModel);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiPolygonMesh);

  scfiPolygonMesh.SetFactory (this);
  scfiObjectModel.SetPolygonMeshBase (&scfiPolygonMesh);
  scfiObjectModel.SetPolygonMeshColldet (&scfiPolygonMesh);
  scfiObjectModel.SetPolygonMeshViscull (0);
  scfiObjectModel.SetPolygonMeshShadows (0);

  csSpriteCal3DMeshObjectFactory::object_reg = object_reg;

  if ((vertex_name == csInvalidStringID) ||
    (texel_name == csInvalidStringID) ||
    (normal_name == csInvalidStringID) ||
    (color_name == csInvalidStringID) ||
    (index_name == csInvalidStringID))
  {
    csRef<iStringSet> strings = 
      CS_QUERY_REGISTRY_TAG_INTERFACE (object_reg, 
      "crystalspace.shared.stringset", iStringSet);
    vertex_name = strings->Request ("vertices");
    texel_name = strings->Request ("texture coordinates");
    normal_name = strings->Request ("normals");
    color_name = strings->Request ("colors");
    index_name = strings->Request ("indices");
  }

  light_mgr = CS_QUERY_REGISTRY (object_reg, iLightManager);
}

csSpriteCal3DMeshObjectFactory::~csSpriteCal3DMeshObjectFactory ()
{
  // Now remove ugly CS material hack from model so cal dtor will work
  //    for (int i=0; i<calCoreModel.getCoreMaterialCount(); i++)
  //    {
  //	CalCoreMaterial *mat = calCoreModel.getCoreMaterial(i);
  //	std::vector< CalCoreMaterial::Map > maps = mat->getVectorMap();
  //	maps.clear();
  //    }

  calCoreModel.destroy();

  SCF_DESTRUCT_EMBEDDED_IBASE (scfiPolygonMesh);
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiObjectModel);
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiLODControl);
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiSpriteCal3DFactoryState);
  SCF_DESTRUCT_IBASE ();
}

bool csSpriteCal3DMeshObjectFactory::Create(const char *name)
{  
  return calCoreModel.create(name);
}

void csSpriteCal3DMeshObjectFactory::ReportLastError ()
{
  ReportCalError (object_reg, "crystalspace.mesh.sprite.cal3d", 0);
}

void csSpriteCal3DMeshObjectFactory::SetLoadFlags(int flags)
{
  CalLoader::setLoadingMode(flags);
}

void csSpriteCal3DMeshObjectFactory::SetBasePath(const char *path)
{
  basePath = path;
}

void csSpriteCal3DMeshObjectFactory::RescaleFactory(float factor)
{
  calCoreModel.scale(factor);
  calCoreModel.getCoreSkeleton()->calculateBoundingBoxes(&calCoreModel);
}

bool csSpriteCal3DMeshObjectFactory::LoadCoreSkeleton (iVFS *vfs,
	const char *filename)
{
  csString path(basePath);
  path.Append(filename);
  csRef<iDataBuffer> file = vfs->ReadFile (path);
  if (file)
  {
    CalCoreSkeleton *skel = CalLoader::loadCoreSkeleton (
    	(void *)file->GetData() );
    if (skel)
    {
      calCoreModel.setCoreSkeleton (skel);
      return true;
    }
    else
      return false;
  }
  else
    return false;
}

int csSpriteCal3DMeshObjectFactory::LoadCoreAnimation (
	iVFS *vfs,const char *filename,
	const char *name,
	int type,
	float base_vel, float min_vel, float max_vel,
	int min_interval, int max_interval,
	int idle_pct, bool lock)
{
  csString path(basePath);
  path.Append(filename);
  csRef<iDataBuffer> file = vfs->ReadFile (path);
  if (file)
  {
    CalCoreAnimation *anim = CalLoader::loadCoreAnimation (
    	(void*)file->GetData(), calCoreModel.getCoreSkeleton() );
    if (anim)
    {
      int id = calCoreModel.addCoreAnimation(anim);
      if (id != -1)
      {
        csCal3DAnimation *an = new csCal3DAnimation;
        an->name          = name;
        an->type          = type;
        an->base_velocity = base_vel;
        an->min_velocity  = min_vel;
        an->max_velocity  = max_vel;
        an->min_interval  = min_interval;
        an->max_interval  = max_interval;
        an->idle_pct      = idle_pct;
        an->lock          = lock;

        an->index = anims.Push(an);

        std::string str(name);
        calCoreModel.addAnimHelper(str,id);
      }
      return id;
    }
    return -1;
  }
  return -1;
}

int csSpriteCal3DMeshObjectFactory::LoadCoreMesh (
	iVFS *vfs,const char *filename,
	const char *name,
	bool attach,
	iMaterialWrapper *defmat)
{
  csString path(basePath);
  path.Append(filename);
  csRef<iDataBuffer> file = vfs->ReadFile (path);
  if (file)
  {
    csCal3DMesh *mesh = new csCal3DMesh;
    CalCoreMesh *coremesh = CalLoader::loadCoreMesh((void*)file->GetData() );
    if (coremesh)
    {
      mesh->index = calCoreModel.addCoreMesh(coremesh);
      if (mesh->index == -1)
      {
        delete mesh;
        return false;
      }
      mesh->name              = name;
      mesh->attach_by_default = attach;
      mesh->default_material  = defmat;

      submeshes.Push(mesh);

      calCoreModel.getCoreSkeleton()->calculateBoundingBoxes(&calCoreModel);

      return mesh->index;
    }
    else
      return -1;
  }
  else
    return -1;
}

int csSpriteCal3DMeshObjectFactory::LoadCoreMorphTarget (
	iVFS *vfs,int mesh_index,
	const char *filename,
	const char *name)
{
  if(mesh_index < 0|| submeshes.Length() <= mesh_index)
  {
    return -1;
  }

  csString path(basePath);
  path.Append(filename);
  csRef<iDataBuffer> file = vfs->ReadFile (path);
  if (file)
  {
    CalCoreMesh *core_mesh = CalLoader::loadCoreMesh((void *)file->GetData() );
    if(core_mesh == 0)
      return -1;
    
    int morph_index = calCoreModel.getCoreMesh(mesh_index)->
    	addAsMorphTarget(core_mesh);
    if(morph_index == -1)
    {
      return -1;
    }
    submeshes[mesh_index]->morph_target_name.Push(name);
    return morph_index;
  }
  return -1;
}

int csSpriteCal3DMeshObjectFactory::AddMorphAnimation(const char *name)
{
  int id = calCoreModel.addCoreMorphAnimation(new CalCoreMorphAnimation());
  morph_animation_names.Push(name);
  return id;
}

bool csSpriteCal3DMeshObjectFactory::AddMorphTarget(
	int morphanimation_index,
	const char *mesh_name, const char *morphtarget_name)
{
  int mesh_index = FindMeshName(mesh_name);
  if(mesh_index == -1)
  {
     return false;
  }
  csArray<csString>& morph_target = submeshes[mesh_index]->morph_target_name;
  int i;
  for (i=0; i<morph_target.Length(); i++)
  {
    if (morph_target[i] == morphtarget_name)
      break;
  }
  if(i==morph_target.Length())
  {
    return false;
  }
  CalCoreMorphAnimation* morph_animation = calCoreModel.getCoreMorphAnimation (
  	morphanimation_index);
  return morph_animation->addMorphTarget(mesh_index,i);
}

int csSpriteCal3DMeshObjectFactory::GetMorphTargetCount(int mesh_id)
{
  if(mesh_id < 0|| submeshes.Length() <= mesh_id)
  {
    return -1;
  }
  return submeshes[mesh_id]->morph_target_name.Length();
}

const char *csSpriteCal3DMeshObjectFactory::GetMeshName(int idx)
{
  if (idx >= submeshes.Length())
    return 0;

  return submeshes[idx]->name;
}

bool csSpriteCal3DMeshObjectFactory::IsMeshDefault(int idx)
{
  if (idx >= submeshes.Length())
    return 0;

  return submeshes[idx]->attach_by_default;
}

csSpriteCal3DSocket* csSpriteCal3DMeshObjectFactory::AddSocket ()
{
  csSpriteCal3DSocket* socket = new csSpriteCal3DSocket();
  sockets.Push (socket);
  return socket;
}

csSpriteCal3DSocket* csSpriteCal3DMeshObjectFactory::FindSocket (
	const char *n) const
{
  int i;
  for (i = GetSocketCount () - 1; i >= 0; i--)
    if (strcmp (GetSocket (i)->GetName (), n) == 0)
      return GetSocket (i);

  return 0;
}

csSpriteCal3DSocket* csSpriteCal3DMeshObjectFactory::FindSocket (
	iMeshWrapper *mesh) const
{
  int i;
  for (i = GetSocketCount () - 1; i >= 0; i--)
    if (GetSocket (i)->GetMeshWrapper() == mesh)
      return GetSocket (i);

  return 0;
}

int csSpriteCal3DMeshObjectFactory::FindMeshName (const char *meshName)
{
  for (int i=0; i<submeshes.Length(); i++)
  {
    if (submeshes[i]->name == meshName)
      return i;
  }
  return -1;
}

const char* csSpriteCal3DMeshObjectFactory::GetDefaultMaterial (
	const char* meshName)
{
  int meshIndex = FindMeshName (meshName);
  if ( meshIndex != -1 )
  {
    if ( submeshes[meshIndex]->default_material )
    {
      return submeshes[meshIndex]->default_material->QueryObject()->GetName();
    }
  }
    
  return 0;                        
}

const char *csSpriteCal3DMeshObjectFactory::GetMorphAnimationName(int idx)
{
  if (idx >= morph_animation_names.Length())
    return 0;

  return morph_animation_names[idx];
}

int csSpriteCal3DMeshObjectFactory::FindMorphAnimationName (
	const char *meshName)
{
  for (int i=0; i<morph_animation_names.Length(); i++)
  {
    if (morph_animation_names[i] == meshName)
      return i;
  }
  return -1;
}


bool csSpriteCal3DMeshObjectFactory::AddCoreMaterial(iMaterialWrapper *mat)
{
  CalCoreMaterial *newmat = new CalCoreMaterial;
  CalCoreMaterial::Map newmap;
  newmap.userData = mat;

  newmat->create();
  newmat->reserve(1);
  newmat->setMap(0,newmap);  // sticking iMaterialWrapper into 2 places
  newmat->setUserData(mat);  // jam CS iMaterialWrapper into cal3d material holder

  calCoreModel.addCoreMaterial(newmat);
  return true;
}

void csSpriteCal3DMeshObjectFactory::BindMaterials()
{
  int materialId;

  // make one material thread for each material
  // NOTE: this is not the right way to do it, but this viewer can't do the
  // right mapping without further information on the model etc.
  for (materialId = 0 ; materialId < calCoreModel.getCoreMaterialCount()
  	; materialId++)
  {
    // create the a material thread
    calCoreModel.createCoreMaterialThread (materialId);

    // initialize the material thread
    calCoreModel.setCoreMaterialId (materialId, 0, materialId);
  }
}


csPtr<iMeshObject> csSpriteCal3DMeshObjectFactory::NewInstance ()
{
  csSpriteCal3DMeshObject* spr = new csSpriteCal3DMeshObject (0, 
    object_reg, calCoreModel);
  spr->SetFactory (this);

  csRef<iMeshObject> im (SCF_QUERY_INTERFACE (spr, iMeshObject));
  spr->DecRef ();
  return csPtr<iMeshObject> (im);
}

void csSpriteCal3DMeshObjectFactory::HardTransform (
	const csReversibleTransform& t)
{
  csQuaternion quat (t.GetO2T ());
  CalQuaternion quatrot(quat.x,quat.y,quat.z,quat.r);
  csVector3 trans (t.GetOrigin () );
  CalVector translation (trans.x,trans.y,trans.z);

  // First we transform the skeleton, then we do the same to each animation.

  // get core skeleton
  CalCoreSkeleton *pCoreSkeleton;
  pCoreSkeleton = calCoreModel.getCoreSkeleton();
	
  // get core bone vector
  std::vector<CalCoreBone *>& vectorCoreBone = pCoreSkeleton
  	->getVectorCoreBone();

  // loop through all root core bones
  std::list<int>::iterator iteratorRootCoreBoneId;
  for (iteratorRootCoreBoneId = pCoreSkeleton->getListRootCoreBoneId().begin()
  	; iteratorRootCoreBoneId != pCoreSkeleton->getListRootCoreBoneId().end()
	; ++iteratorRootCoreBoneId)
  {
    CalCoreBone *bone = vectorCoreBone[*iteratorRootCoreBoneId];
    CalQuaternion bonerot = bone->getRotation();
    CalVector bonevec = bone->getTranslation();
    bonerot *= quatrot;
    bonevec *= quatrot;
    bonevec += translation;
    bone->setRotation(bonerot);
    bone->setTranslation(bonevec);
  }

  int i,count = calCoreModel.getCoreAnimationCount();
  for (i = 0; i < count; i++)
  {
    CalCoreAnimation *anim = calCoreModel.getCoreAnimation(i);
    if (!anim) continue;
    // loop through all root core bones
    std::list<int>::iterator iteratorRootCoreBoneId;
    for (iteratorRootCoreBoneId = pCoreSkeleton->getListRootCoreBoneId().begin()
    	; iteratorRootCoreBoneId != pCoreSkeleton->getListRootCoreBoneId().end()
	; ++iteratorRootCoreBoneId)
    {
      CalCoreTrack *track = anim->getCoreTrack(*iteratorRootCoreBoneId);
      if (!track) continue;
      for (int j=0; j<track->getCoreKeyframeCount(); j++)
      {
	CalCoreKeyframe *frame = track->getCoreKeyframe(j);
	CalQuaternion bonerot = frame->getRotation();
	CalVector bonevec = frame->getTranslation();
	bonerot *= quatrot;
	bonevec *= quatrot;
	bonevec += translation;
	frame->setRotation(bonerot);
	frame->setTranslation(bonevec);
      }
    }
  }
  calCoreModel.getCoreSkeleton()->calculateBoundingBoxes(&calCoreModel);
}

//=============================================================================

SCF_IMPLEMENT_IBASE (csSpriteCal3DMeshObjectFactory::PolyMesh)
SCF_IMPLEMENTS_INTERFACE (iPolygonMesh)
SCF_IMPLEMENT_IBASE_END

//=============================================================================

SCF_IMPLEMENT_IBASE (csSpriteCal3DMeshObject)
  SCF_IMPLEMENTS_INTERFACE (iMeshObject)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iLightingInfo)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iSpriteCal3DState)
  #ifndef CS_USE_NEW_RENDERER
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iVertexBufferManagerClient)
  #else
  #endif // CS_USE_NEW_RENDERER
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iLODControl)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObjectModel)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSpriteCal3DMeshObject::LightingInfo)
  SCF_IMPLEMENTS_INTERFACE (iLightingInfo)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSpriteCal3DMeshObject::SpriteCal3DState)
  SCF_IMPLEMENTS_INTERFACE (iSpriteCal3DState)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSpriteCal3DMeshObject::PolyMesh)
  SCF_IMPLEMENTS_INTERFACE (iPolygonMesh)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

#ifndef CS_USE_NEW_RENDERER
SCF_IMPLEMENT_EMBEDDED_IBASE (csSpriteCal3DMeshObject::eiVertexBufferManagerClient)
SCF_IMPLEMENTS_INTERFACE (iVertexBufferManagerClient)
SCF_IMPLEMENT_EMBEDDED_IBASE_END
#else
#endif // CS_USE_NEW_RENDERER

SCF_IMPLEMENT_EMBEDDED_IBASE (csSpriteCal3DMeshObject::LODControl)
  SCF_IMPLEMENTS_INTERFACE (iLODControl)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSpriteCal3DMeshObject::ObjectModel)
  SCF_IMPLEMENTS_INTERFACE (iObjectModel)
SCF_IMPLEMENT_EMBEDDED_IBASE_END



csSpriteCal3DMeshObject::csSpriteCal3DMeshObject (iBase *pParent,
						  iObjectRegistry* object_reg,
						  CalCoreModel& calCoreModel)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiLightingInfo);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSpriteCal3DState);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiLODControl);
#ifndef CS_USE_NEW_RENDERER
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiVertexBufferManagerClient);
#endif
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiPolygonMesh);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObjectModel);

  csSpriteCal3DMeshObject::object_reg = object_reg;

  //  scfiPolygonMesh.SetFactory (this);
  //  scfiObjectModel.SetPolygonMeshBase (&scfiPolygonMesh);
  //  scfiObjectModel.SetPolygonMeshColldet (&scfiPolygonMesh);
  //  scfiObjectModel.SetPolygonMeshViscull (0);
  //  scfiObjectModel.SetPolygonMeshShadows (0);

  // create the model instance from the loaded core model
  if(!calModel.create (&calCoreModel))
  {
    ReportCalError (object_reg, "crystalspace.mesh.sprite.cal3d", 
      "Error creating model instance");
    return;
  }

#ifdef CS_USE_NEW_RENDERER
  strings =  CS_QUERY_REGISTRY_TAG_INTERFACE (object_reg, 
    "crystalspace.shared.stringset", iStringSet);
  G3D = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
#endif

  // set the material set of the whole model
  vis_cb = 0;
  arrays_initialized = false;
  is_idling = false;
  meshes_colors = 0;
  is_initialized = 0;
#ifdef CS_USE_NEW_RENDERER
  rmeshesSetup = false;
#endif
  meshVersion = 0;
  bboxVersion = (uint)-1;
}

csSpriteCal3DMeshObject::~csSpriteCal3DMeshObject ()
{
  calModel.destroy();

  if (arrays_initialized)
  {
#ifndef CS_USE_NEW_RENDERER
    delete [] meshes;
#endif
    delete [] is_initialized;
    delete [] meshes_colors;
  }

  SCF_DESTRUCT_EMBEDDED_IBASE (scfiObjectModel);
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiPolygonMesh);
#ifndef CS_USE_NEW_RENDERER
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiVertexBufferManagerClient);
#endif
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiLODControl);
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiSpriteCal3DState);
  SCF_DESTRUCT_IBASE ();
}


void csSpriteCal3DMeshObject::SetFactory (csSpriteCal3DMeshObjectFactory* tmpl)
{
  factory = tmpl;

  CalSkeleton *skeleton;
  CalBone *bone;
  skeleton = calModel.getSkeleton();
  std::vector < CalBone *> &bones = skeleton->getVectorBone();
  int i;
  for (i=0; i < (int)bones.size(); i++)
  {
    bone = bones[i];
    bone->calculateState ();
  }
  skeleton->calculateState ();

  // attach all default meshes to the model
  int meshId;
  for(meshId = 0; meshId < factory->GetMeshCount(); meshId++)
  {
    if (factory->submeshes[meshId]->attach_by_default)
    {
      AttachCoreMesh(factory->submeshes[meshId]->index,(int)factory->submeshes[
      	meshId]->default_material);
    }
  }
  //  calModel.setMaterialSet(0);
  calModel.update(0);
  last_update_time = csGetTicks();

  RecalcBoundingBox(object_bbox);

  // Copy the sockets list down to the mesh
  iSpriteCal3DSocket *factory_socket,*new_socket;
  for (i=0; i<tmpl->GetSocketCount(); i++)
  {
    factory_socket = tmpl->GetSocket(i);
    new_socket = AddSocket();  // mesh now
    new_socket->SetName (factory_socket->GetName() );
    new_socket->SetTriangleIndex (factory_socket->GetTriangleIndex() );
    new_socket->SetSubmeshIndex (factory_socket->GetSubmeshIndex() );
    new_socket->SetMeshIndex (factory_socket->GetMeshIndex() );
    new_socket->SetMeshWrapper (0);
  }
}

#ifndef CS_USE_NEW_RENDERER
void csSpriteCal3DMeshObject::SetupVertexBuffer (int mesh,
	int submesh, int num_vertices,
	int num_triangles, csTriangle *triangles)
{
  if (!meshes[mesh][submesh].buffers[0])
  {
    // @@@ priority should be a parameter.
    if (!vbuf)
    {
      iObjectRegistry* object_reg = ((csSpriteCal3DMeshObjectFactory*)factory)
	->object_reg;
      csRef<iGraphics3D> g3d (CS_QUERY_REGISTRY (object_reg, iGraphics3D));
      vbufmgr = g3d->GetVertexBufferManager ();
      vbuf = vbufmgr->CreateBuffer (0);
      vbufmgr->AddClient (&scfiVertexBufferManagerClient);
    }
    meshes[mesh][submesh].buffers[0] = vbuf;

    meshes[mesh][submesh].vertex_fog = new G3DFogInfo[num_vertices];
    meshes[mesh][submesh].num_triangles = num_triangles;
    meshes[mesh][submesh].triangles = new csTriangle[num_triangles];
    memcpy (meshes[mesh][submesh].triangles, triangles,
    	sizeof(csTriangle)*num_triangles);
  }
}
#endif

void csSpriteCal3DMeshObject::GetRadius (csVector3& rad, csVector3& cent)
{
  cent.Set (object_bbox.GetCenter());
  csVector3 maxbox, minbox;

  RecalcBoundingBox (object_bbox);
  maxbox = object_bbox.Max();
  minbox = object_bbox.Min();
  float r1 = (maxbox.x-minbox.x)/2;
  float r2 = (maxbox.y-minbox.y)/2;
  float r3 = (maxbox.z-minbox.z)/2;
  rad.Set(r1,r2,r3);
}

#define CAL3D_EXACT_BOXES true 

void csSpriteCal3DMeshObject::RecalcBoundingBox (csBox3& bbox)
{
  if (bboxVersion == meshVersion)
    return;

  CalBoundingBox &calBoundingBox  = calModel.getBoundingBox(CAL3D_EXACT_BOXES);
  CalVector p[8];
  calBoundingBox.computePoints(p);

  bbox.Set(p[0].x, p[0].y, p[0].z, p[0].x, p[0].y, p[0].z);
  for (int i=1; i<8; i++)
  {
    bbox.AddBoundingVertexSmart(p[i].x,p[i].y,p[i].z);
  }

  bboxVersion = meshVersion;
//  printf("Bbox Width:%1.2f Height:%1.2f Depth:%1.2f\n",bbox.Max().x - bbox.Min().x,bbox.Max().y - bbox.Min().y,bbox.Max().z - bbox.Min().z);
}


void csSpriteCal3DMeshObject::GetObjectBoundingBox (csBox3& bbox, int type)
{
  RecalcBoundingBox (object_bbox);
  bbox = object_bbox;
}

void csSpriteCal3DMeshObjectFactory::GetRadius (csVector3& rad, csVector3& cent)
{
  cent.Set(0,0,0);
  rad.Set(1,1,1);
}

void csSpriteCal3DMeshObjectFactory::GetObjectBoundingBox (csBox3& bbox,
	int type)
{
  CalCoreSkeleton *skel = calCoreModel.getCoreSkeleton ();
  skel->calculateBoundingBoxes(&calCoreModel);
  std::vector<CalCoreBone*> &vectorCoreBone = skel->getVectorCoreBone();
  CalBoundingBox &calBoundingBox  = vectorCoreBone[0]->getBoundingBox();
  CalVector p[8];
  calBoundingBox.computePoints(p);

  bbox.Set (p[0].x, p[0].y, p[0].z, p[0].x, p[0].y, p[0].z);
  for (int i=1; i<8; i++)
  {
    bbox.AddBoundingVertexSmart(p[i].x, p[i].y, p[i].z);
  }
}

void csSpriteCal3DMeshObject::GetObjectBoundingBox (csBox3& bbox,
	int type, csVector3 *verts,int vertCount)
{
  if (object_bbox.Empty())
    RecalcBoundingBox(object_bbox);
  bbox = object_bbox;
}

void csSpriteCal3DMeshObject::LightChanged (iLight*)
{
  lighting_dirty = true;
}

void csSpriteCal3DMeshObject::LightDisconnect (iLight* light)
{
  lighting_dirty = true;
}

void csSpriteCal3DMeshObject::UpdateLighting (const csArray<iLight*>& lights,
					      iMovable* movable)
{
  CalRenderer *pCalRenderer;
  pCalRenderer = calModel.getRenderer();
  
  // begin the rendering loop
  if(!pCalRenderer->beginRendering())
  {
    return;
  }
  
  int meshCount;
  meshCount = pCalRenderer->getMeshCount();
  
  // loop through all meshes of the model
  int meshId;
  for(meshId = 0; meshId < meshCount; meshId++)
  {
    // get the number of submeshes
    int submeshCount;
    submeshCount = pCalRenderer->getSubmeshCount(meshId);
    
    // loop through all submeshes of the mesh
    int submeshId;
    for(submeshId = 0; submeshId < submeshCount; submeshId++)
    {
      // select mesh and submesh for further data access
      if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
      {
        UpdateLightingSubmesh (lights, movable, pCalRenderer,
            meshId, submeshId);
      }
    }
  }
  
  pCalRenderer->endRendering();
}


void csSpriteCal3DMeshObject::UpdateLightingSubmesh (const csArray<iLight*>& lights, 
						     iMovable* movable,
						     CalRenderer *pCalRenderer,
						     int mesh, int submesh,float *have_normals)
{
  int vertCount;
  vertCount = pCalRenderer->getVertexCount();

  int i;

  // get the transformed normals of the submesh
  static float localNormals[60000];
  float *meshNormals;
  if (!have_normals)
  {
    pCalRenderer->getNormals(localNormals);
    meshNormals = localNormals;
  }
  else
  {
    meshNormals = have_normals;
  }

  // Do the lighting.
  csReversibleTransform trans = movable->GetFullTransform ();
  // the object center in world coordinates. "0" because the object
  // center in object space is obviously at (0,0,0).
  csColor color;

  int num_lights = lights.Length ();

  // Make sure colors array exists and set all to ambient
  InitSubmeshLighting (mesh, submesh, pCalRenderer, movable);
  csColor* colors = meshes_colors[mesh][submesh];

  // Update Lighting for all relevant lights
  for (int l = 0; l < num_lights; l++)
  {
    iLight* li = lights[l];
    // Compute light position in object coordinates
    // @@@ Can be optimized a bit. E.g. store obj_light_pos so it can be
    //  reused by submesh lighting.
    csVector3 wor_light_pos = li->GetCenter ();
    csVector3 obj_light_pos = trans.Other2This (wor_light_pos);
    float obj_sq_dist = csSquaredDist::PointPoint (obj_light_pos, 0);
    if (obj_sq_dist >= li->GetInfluenceRadiusSq ()) return;
    float in_obj_dist = (obj_sq_dist >= SMALL_EPSILON)?qisqrt (obj_sq_dist):1.0f;

    csColor light_color = li->GetColor () * (256.0f / CS_NORMAL_LIGHT_LEVEL)
      * li->GetBrightnessAtDistance (qsqrt (obj_sq_dist));

    int normal_index=0;
    for (i = 0; i < vertCount; i++)
    {
      csVector3 normal(meshNormals[normal_index],meshNormals[normal_index+1],meshNormals[normal_index+2]);
      normal_index+=3;
      float cosinus;
      if (obj_sq_dist < SMALL_EPSILON) cosinus = 1;
      else cosinus = obj_light_pos * normal; 
      // because the vector from the object center to the light center
      // in object space is equal to the position of the light

      if (cosinus > 0)
      {
	color = light_color;
	if (obj_sq_dist >= SMALL_EPSILON) cosinus *= in_obj_dist;
	if (cosinus < 1.0f) color *= cosinus;
	colors[i] += color;
      }
    }
  }

  // Clamp all vertex colors to 2.
  for (i = 0 ; i < vertCount; i++)
    colors[i].Clamp (2.0f, 2.0f, 2.0f);
}

void csSpriteCal3DMeshObject::InitSubmeshLighting (int mesh, int submesh,
						   CalRenderer *pCalRenderer,
						   iMovable* movable)
{
  csColor* colors = meshes_colors[mesh][submesh];

  int vertCount = pCalRenderer->getVertexCount();
  if (!colors)
  {
    meshes_colors[mesh][submesh] = new csColor[vertCount];
    colors = meshes_colors[mesh][submesh];
  }

  // Set all colors to ambient light.
  csColor col;
  if (((csSpriteCal3DMeshObjectFactory*)factory)->engine)
  {
    ((csSpriteCal3DMeshObjectFactory*)factory)->engine->GetAmbientLight (col);
    //    col += color;  // no inherent color in cal3d sprites
    iSector* sect = movable->GetSectors ()->Get (0);
    if (sect)
      col += sect->GetDynamicAmbientLight ();
  }
  else
  {
    //    col = color;
  }
  for (int i = 0 ; i < vertCount ; i++)
    colors[i] = col;
}

void csSpriteCal3DMeshObject::UpdateLighting (iMovable* movable, 
					      CalRenderer *pCalRenderer)
{
  if (!lighting_dirty)
    return;
  
  int meshCount;
  meshCount = pCalRenderer->getMeshCount();
  
  if (factory->light_mgr)
  {
    const csArray<iLight*>& relevant_lights = factory->light_mgr
      ->GetRelevantLights (logparent, -1, false);
    int num_lights = relevant_lights.Length ();
    
    // loop through all meshes of the model 
    int meshId;
    for(meshId = 0; meshId < meshCount; meshId++)
    {
      // get the number of submeshes
      int submeshCount;
      submeshCount = pCalRenderer->getSubmeshCount(meshId);
      
      // loop through all submeshes of the mesh
      int submeshId;
      for(submeshId = 0; submeshId < submeshCount; submeshId++)
      {
        // select mesh and submesh for further data access
        if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
        {
            UpdateLightingSubmesh (relevant_lights,
              movable, pCalRenderer, meshId, submeshId);
        }
      }
    }
  }
  
  lighting_dirty = false;
}

void csSpriteCal3DMeshObject::SetupObjectSubmesh(int index)
{
  if (!arrays_initialized)
    return;  // First draw call to SetupObject will take care of most of this.

  if (!calModel.getMesh(index))
    return;

  int submeshes = calModel.getMesh(index)->getSubmeshCount();
#ifdef CS_USE_NEW_RENDERER
  renderMeshes[index].SetLength (submeshes);
#endif
  for (int j=0; j<submeshes; j++)
  {
    bool flag = false;
    is_initialized[index].Push(flag);
#ifndef CS_USE_NEW_RENDERER
    G3DTriangleMesh sample;
    sample.num_vertices_pool = 1;  // no blending
    sample.buffers[0]  = 0; // NULL
    meshes[index].Push(sample);
#endif
    meshes_colors[index].Push(0);
  }
}

void csSpriteCal3DMeshObject::SetupObject()
{
  int meshCount;
  meshCount = calModel.getRenderer()->getMeshCount();

  if (!arrays_initialized)
  {
    arrays_initialized = true;
#ifndef CS_USE_NEW_RENDERER
    meshes = new csArray<G3DTriangleMesh> [ meshCount ];
#endif
    is_initialized = new csArray<bool> [ meshCount ];
    meshes_colors  = new csArray<csColor*> [ meshCount ];
#ifdef CS_USE_NEW_RENDERER
    renderMeshes.SetLength (meshCount);
#endif
    
    for (int index=0; index<meshCount; index++)
    {
      SetupObjectSubmesh (index);
    }
  }
  for (int index=0; index<meshCount; index++)
  {
    CalMesh *mesh = calModel.getMesh(index);
    if (!mesh)
      continue;
    int submeshes = mesh->getSubmeshCount();

    for (int j=0; j<submeshes; j++)
    {
      if (!is_initialized[index][j])
      {
	is_initialized[index][j] = true;

	//	  delete[] meshes[index][j].triangles;
	//	  delete[] meshes[index][j].vertex_fog;
#ifndef CS_USE_NEW_RENDERER
	meshes[index][j].triangles = 0;
	meshes[index][j].vertex_fog = 0;
#endif

//	factory->GetObjectBoundingBox(object_bbox);  // initialize object_bbox here

#ifndef CS_USE_NEW_RENDERER
	meshes[index][j].morph_factor = 0.0f;
	meshes[index][j].num_vertices_pool = 1;
	meshes[index][j].do_morph_texels = false;
	meshes[index][j].do_morph_colors = false;
	meshes[index][j].vertex_mode = G3DTriangleMesh::VM_WORLDSPACE;
#endif
      }
    }
  }
  RecalcBoundingBox(object_bbox);
}

bool csSpriteCal3DMeshObject::HitBeamOutline (const csVector3& start,
	const csVector3& end, csVector3& isect, float* pr)
{
  csSegment3 seg (start, end);
  float dist, temp, max;
  temp = dist = max = csSquaredDist::PointPoint (start, end);
  csVector3 tsect;

  std::vector<CalMesh *>& vectorMesh = calModel.getVectorMesh ();
  std::vector<CalMesh *>::iterator iteratorMesh = vectorMesh.begin();
  while (iteratorMesh != vectorMesh.end())
  {
    std::vector<CalSubmesh * >& vectorSubmesh = (*iteratorMesh)
    	->getVectorSubmesh ();
    std::vector<CalSubmesh *>::iterator iteratorSubmesh = vectorSubmesh.begin();
    while (iteratorSubmesh != vectorSubmesh.end())
    {
      csVector3* pVertexBuffer = new csVector3[(*iteratorSubmesh)
      	->getVertexCount ()];
      calModel.getPhysique()->calculateVertices(*iteratorSubmesh,
      	(float*)pVertexBuffer);

      std::vector<CalCoreSubmesh::Face>& vectorFace = (*iteratorSubmesh)
      	->getCoreSubmesh()->getVectorFace();
      std::vector<CalCoreSubmesh::Face>::iterator iteratorFace
      	= vectorFace.begin();
      while(iteratorFace != vectorFace.end())
      {
	if (csIntersect3::IntersectTriangle (
		pVertexBuffer[(*iteratorFace).vertexId[0]],
		pVertexBuffer[(*iteratorFace).vertexId[1]],
		pVertexBuffer[(*iteratorFace).vertexId[2]], seg, tsect))
	{
	  isect = tsect;
	  if (pr) *pr = qsqrt (csSquaredDist::PointPoint (start, isect) /
	      csSquaredDist::PointPoint (start, end));
	  return true;
	}
	++iteratorFace;
      }
      ++iteratorSubmesh;
    }
    ++iteratorMesh;
  }
  return false;
}


bool csSpriteCal3DMeshObject::HitBeamObject (const csVector3& start,
    const csVector3& end, csVector3& isect, float* pr, int* polygon_idx)
{
  // This routine is slow, but it is intended to be accurate.
  if (polygon_idx) *polygon_idx = -1;

  csSegment3 seg (start, end);
  float dist, temp, max;
  temp = dist = max = csSquaredDist::PointPoint (start, end);
  csVector3 tsect;
  
  std::vector<CalMesh *>& vectorMesh = calModel.getVectorMesh ();
  std::vector<CalMesh *>::iterator iteratorMesh = vectorMesh.begin();
  while (iteratorMesh != vectorMesh.end())
  {
    std::vector<CalSubmesh * >& vectorSubmesh = (*iteratorMesh)
    	->getVectorSubmesh ();
    std::vector<CalSubmesh *>::iterator iteratorSubmesh = vectorSubmesh.begin();
    while (iteratorSubmesh != vectorSubmesh.end())
    {
      csVector3* pVertexBuffer = new csVector3[(*iteratorSubmesh)
      	->getVertexCount ()];
      calModel.getPhysique()->calculateVertices(*iteratorSubmesh,
      	(float*)pVertexBuffer);

      std::vector<CalCoreSubmesh::Face>& vectorFace = (*iteratorSubmesh)
      	->getCoreSubmesh()->getVectorFace();
      std::vector<CalCoreSubmesh::Face>::iterator iteratorFace
      	= vectorFace.begin();
      while(iteratorFace != vectorFace.end())
      {
	if (csIntersect3::IntersectTriangle (
		pVertexBuffer[(*iteratorFace).vertexId[0]], 
		pVertexBuffer[(*iteratorFace).vertexId[1]],
		pVertexBuffer[(*iteratorFace).vertexId[2]], seg, tsect))
	{
	  temp = csSquaredDist::PointPoint (start, tsect);
	  if (temp < dist)
	  {
	    dist = temp;
	    isect = tsect;
	  }
	}
	++iteratorFace;
      }
      ++iteratorSubmesh;
    }
    ++iteratorMesh;
  }
  if (pr) *pr = qsqrt (dist / max);
  if (dist >= max) return false;
  return true;
}

void csSpriteCal3DMeshObject::PositionChild (iMeshObject* child,
	csTicks current_time)
{
  iSpriteCal3DSocket* socket = 0;
  int i;
  for(i=0;i<sockets.Length();i++)
  {
    if(sockets[i]->GetMeshWrapper())
    {
      if (sockets[i]->GetMeshWrapper()->GetMeshObject() == child)
      {
	socket = sockets[i];
	break;
      }
    }
  }
  if(socket)
  {
    iMovable* movable = socket->GetMeshWrapper()->GetMovable();
    Advance(current_time);
    // Get the submesh
    CalSubmesh* submesh = calModel.getMesh(socket->GetMeshIndex())
    	->getSubmesh(socket->GetSubmeshIndex());
    // Get the core submesh
    CalCoreSubmesh* coresubmesh = calModel.getCoreModel()->
      getCoreMesh(socket->GetMeshIndex())->getCoreSubmesh(
      	socket->GetSubmeshIndex());
    // get the sub morph target vector from the core sub mesh
    std::vector<CalCoreSubMorphTarget*>& vectorSubMorphTarget =
      coresubmesh->getVectorCoreSubMorphTarget();
    // calculate the base weight
    float baseWeight = submesh->getBaseWeight();
    // get the number of morph targets
    int morphTargetCount = submesh->getMorphTargetWeightCount();
    // get the triangle
    CalCoreSubmesh::Face face = coresubmesh->getVectorFace()[
    	socket->GetTriangleIndex()];
    // get the vertices
    CalCoreSubmesh::Vertex vertex[3];
    vertex[0] = coresubmesh->getVectorVertex()[face.vertexId[0]];
    vertex[1] = coresubmesh->getVectorVertex()[face.vertexId[1]];
    vertex[2] = coresubmesh->getVectorVertex()[face.vertexId[2]];
    CalVector vector[3];
    unsigned int i;
    unsigned int j;
    for(i=0;i<3;i++)
    {
      CalVector position(0,0,0);
      if(baseWeight == 1.0f)
      {
	position.x = vertex[i].position.x;
	position.y = vertex[i].position.y;
	position.z = vertex[i].position.z;
      }
      else
      {
	position.x = baseWeight*vertex[i].position.x;
	position.y = baseWeight*vertex[i].position.y;
	position.z = baseWeight*vertex[i].position.z;
	int morphTargetId;
	for(morphTargetId=0; morphTargetId < morphTargetCount;++morphTargetId)
	{
	  CalCoreSubMorphTarget::BlendVertex& blendVertex =
	    vectorSubMorphTarget[morphTargetId]->getVectorBlendVertex()[
	    face.vertexId[i]];
	  float currentWeight = submesh->getMorphTargetWeight(morphTargetId);
	  position.x += currentWeight*blendVertex.position.x;
	  position.y += currentWeight*blendVertex.position.y;
	  position.z += currentWeight*blendVertex.position.z;
	}
      }
      for(j=0;j<vertex[i].vectorInfluence.size();j++)
      {
	CalBone* bone = calModel.getSkeleton()->getBone(
		vertex[i].vectorInfluence[j].boneId);
	CalVector v(position);
	v *= bone->getTransformMatrix();
	v += bone->getTranslationBoneSpace();
	vector[i] += vertex[i].vectorInfluence[j].weight*v;
      }
    }
    csVector3 vert1(vector[0].x,vector[0].y,vector[0].z);
    csVector3 vert2(vector[1].x,vector[1].y,vector[1].z);
    csVector3 vert3(vector[2].x,vector[2].y,vector[2].z);

    csVector3 center= (vert1+vert2+vert3)/3;
    

    csVector3 bc = vert3 - vert2;
        
    csVector3 up = vert1-center;
    up.Normalize();
    
    csVector3 normal = bc % up;
    normal.Normalize();

    csReversibleTransform trans = movable->GetFullTransform();
    trans.SetOrigin(center);
    trans.LookAt(normal, up);
    movable->SetTransform(trans);
    movable->UpdateMove();

  }
}

bool csSpriteCal3DMeshObject::DrawTest (iRenderView* rview, iMovable* movable,
	uint32 frustum_mask)
{
  SetupObject ();

  iGraphics3D* g3d = rview->GetGraphics3D ();
  iCamera* camera = rview->GetCamera ();

  // First create the transformation from object to camera space directly:
  //   W = Mow * O - Vow;
  //   C = Mwc * (W - Vwc)
  // ->
  //   C = Mwc * (Mow * O - Vow - Vwc)
  //   C = Mwc * Mow * O - Mwc * (Vow + Vwc)
  csReversibleTransform tr_o2c;
  tr_o2c = camera->GetTransform ();
  if (!movable->IsFullTransformIdentity ())
    tr_o2c /= movable->GetFullTransform ();

  //  float scale = factory->GetRenderScale();
  //  csMatrix3 scale_mat(scale,0,0,0,scale,0,0,0,scale);

  //  tr_o2c *= scale_mat;

  int clip_portal, clip_plane, clip_z_plane;
  rview->CalculateClipSettings (frustum_mask, clip_portal, clip_plane,
      clip_z_plane);

#ifndef CS_USE_NEW_RENDERER
  g3d->SetObjectToCamera (&tr_o2c);
  int meshCount;
  meshCount = calModel.getRenderer()->getMeshCount();
  for (int i=0; i<meshCount; i++)
  {
    if (!calModel.getMesh(i))
      continue;
    for (int j=0; j<calModel.getMesh(i)->getSubmeshCount(); j++)
    {
      meshes[i][j].clip_portal = clip_portal;
      meshes[i][j].clip_plane = clip_plane;
      meshes[i][j].clip_z_plane = clip_z_plane;
      meshes[i][j].do_mirror = camera->IsMirrored ();
    }
  }
#endif

// This is now done at rendertime each frame.
//  if (factory->light_mgr)
//  {
//    const csArray<iLight*>& relevant_lights = factory->light_mgr
//    	->GetRelevantLights (logparent, -1, false);
//    UpdateLighting (relevant_lights, movable);
//  }

  return true;
}

csRenderMesh** csSpriteCal3DMeshObject::GetRenderMeshes (int &n, 
	iRenderView* rview,
	iMovable* movable, uint32 frustum_mask)
{
#ifndef CS_USE_NEW_RENDERER
  n = 0;
  return 0;
#else
  SetupObject ();

  iGraphics3D* g3d = rview->GetGraphics3D ();
  iCamera* camera = rview->GetCamera ();

  // First create the transformation from object to camera space directly:
  //   W = Mow * O - Vow;
  //   C = Mwc * (W - Vwc)
  // ->
  //   C = Mwc * (Mow * O - Vow - Vwc)
  //   C = Mwc * Mow * O - Mwc * (Vow + Vwc)
  csReversibleTransform tr_o2c;
  tr_o2c = camera->GetTransform ();
  if (!movable->IsFullTransformIdentity ())
    tr_o2c /= movable->GetFullTransform ();

  //  float scale = factory->GetRenderScale();
  //  csMatrix3 scale_mat(scale,0,0,0,scale,0,0,0,scale);

  //  tr_o2c *= scale_mat;

  int clip_portal, clip_plane, clip_z_plane;
  rview->CalculateClipSettings (frustum_mask, clip_portal, clip_plane,
  	clip_z_plane);
  csVector3 camera_origin = tr_o2c.GetT2OTranslation ();

  SetupRenderMeshes ();
  csDirtyAccessArray<csRenderMesh*>& meshes = 
    rmHolder.GetUnusedMeshes (rview->GetCurrentFrameNumber ());

  for (int m = 0; m < allRenderMeshes.Length(); m++)
  {
    csRenderMesh* rm;
    if (m >= meshes.Length())
    {
      rm = new csRenderMesh (*allRenderMeshes[m]);
      meshes.Push (rm);
    }
    else
    {
      rm = meshes[m];
      *rm = *allRenderMeshes[m];
    }

    rm->clip_portal = clip_portal;
    rm->clip_plane = clip_plane;
    rm->clip_z_plane = clip_z_plane;
    rm->do_mirror = camera->IsMirrored ();
    rm->object2camera = tr_o2c;
    rm->camera_origin = camera_origin;
    rm->geometryInstance = this;
  }
  currentMovable = movable;
  // @@@ One movable for all meshes... not good.

  n = meshes.Length();
  return meshes.GetArray();

  /*n = allRenderMeshes.Length();
  return allRenderMeshes.GetArray();*/
#endif
}

#ifndef CS_USE_NEW_RENDERER
bool csSpriteCal3DMeshObject::DrawSubmesh (iGraphics3D* g3d,
					   iRenderView* rview,
					   CalRenderer *pCalRenderer,
					   int mesh, 
					   int submesh,
					   iMaterialWrapper *material)
{
  // get the transformed vertices of the submesh
  static float meshVertices[30000][3];
  int vertexCount;
  vertexCount = pCalRenderer->getVertices(&meshVertices[0][0]);

  GetObjectBoundingBox (object_bbox, 0, (csVector3 *)meshVertices,vertexCount);

  // get the texture coordinates of the submesh
  // (only for the first map as example, others can be accessed in the
  // same way though)
  static float meshTextureCoordinates[30000][2];
  int textureCoordinateCount;
  textureCoordinateCount = pCalRenderer->getTextureCoordinates(
  	0, &meshTextureCoordinates[0][0]);

  // get the stored texture identifier
  // (only for the first map as example, others can be accessed in the same
  // way though)
  //    iMaterialWrapper *material;
  //    material = (iMaterialWrapper *)pCalRenderer->getMapUserData(0);

  // get the faces of the submesh
  static int meshFaces[50000][3];
  int faceCount;
  faceCount = pCalRenderer->getFaces(&meshFaces[0][0]);

  SetupVertexBuffer (mesh, submesh, vertexCount, faceCount,
  	(csTriangle *)meshFaces);
  if (material) material->Visit ();

  meshes[mesh][submesh].mat_handle = (material)?material->GetMaterialHandle():0;
  meshes[mesh][submesh].use_vertex_color = true;
  meshes[mesh][submesh].mixmode = /* MixMode  | */ CS_FX_COPY;
  meshes[mesh][submesh].vertex_mode = G3DTriangleMesh::VM_WORLDSPACE;

  CS_ASSERT (!vbuf->IsLocked ());

  vbufmgr->LockBuffer (vbuf,
    (csVector3 *)meshVertices,
    (csVector2 *)meshTextureCoordinates,
    meshes_colors[mesh][submesh],
    vertexCount,
    0, object_bbox);

  rview->CalculateFogMesh (g3d->GetObjectToCamera (), meshes[mesh][submesh]);
  g3d->DrawTriangleMesh (meshes[mesh][submesh]);

  vbufmgr->UnlockBuffer (vbuf);
  return true;
}
#endif

#ifdef CS_USE_NEW_RENDERER
void csSpriteCal3DMeshObject::SetupRenderMeshes ()
{
  if (rmeshesSetup)
    return;

  for (int m = 0; m < renderMeshes.Length(); m++)
  {
    for (int s = 0; s < renderMeshes[m].Length(); s++)
    {
      CalSubmesh* submesh = calModel.getMesh (m)->getSubmesh (s);
      csRenderMesh& rm = renderMeshes[m][s];
      rm.indexstart = 0;
      rm.indexend = submesh->getFaceCount () * 3;
      rm.meshtype = CS_MESHTYPE_TRIANGLES;
      rm.material = (iMaterialWrapper *)submesh->getCoreMaterialId ();
      rm.variablecontext.AttachNew (new csShaderVariableContext ());

      csShaderVariable* sv;
      csRef<iShaderVariableAccessor> accessor (
	csPtr<iShaderVariableAccessor> (new BaseAccessor (this, m, s)));
      sv = rm.variablecontext->GetVariableAdd (
        csSpriteCal3DMeshObjectFactory::index_name);
      sv->SetAccessor (accessor);
      sv = rm.variablecontext->GetVariableAdd (
        csSpriteCal3DMeshObjectFactory::texel_name);
      sv->SetAccessor (accessor);
      sv = rm.variablecontext->GetVariableAdd (
        csSpriteCal3DMeshObjectFactory::normal_name);
      sv->SetAccessor (accessor);
      sv = rm.variablecontext->GetVariableAdd (
        csSpriteCal3DMeshObjectFactory::color_name);
      sv->SetAccessor (accessor);
      sv = rm.variablecontext->GetVariableAdd (
        csSpriteCal3DMeshObjectFactory::vertex_name);
      sv->SetAccessor (accessor);

      allRenderMeshes.Push (&rm);
    }
  }

  rmeshesSetup = true;
}
#endif

bool csSpriteCal3DMeshObject::Draw (iRenderView* rview, iMovable* movable,
				    csZBufMode mode)
{
#ifndef CS_USE_NEW_RENDERER
  if (vis_cb) if (!vis_cb->BeforeDrawing (this, rview)) return false;

  iGraphics3D* g3d = rview->GetGraphics3D ();

  CalRenderer *pCalRenderer;
  pCalRenderer = calModel.getRenderer();

  // begin the rendering loop
  if(!pCalRenderer->beginRendering())
  {
    return false;
  }

  UpdateLighting (movable, pCalRenderer);

  // Prepare for rendering.
  g3d->SetRenderState (G3DRENDERSTATE_ZBUFFERMODE, mode);


  int meshCount;
  meshCount = pCalRenderer->getMeshCount();

  // loop through all meshes of the model
  int meshId;
  for(meshId = 0; meshId < meshCount ; meshId++)
  {
    // get the number of submeshes
    int submeshCount;
    submeshCount = pCalRenderer->getSubmeshCount(meshId);

    // loop through all submeshes of the mesh
    int submeshId;
    for(submeshId = 0; submeshId < submeshCount; submeshId++)
    {
      // select mesh and submesh for further data access
      if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
      {
	iMaterialWrapper *mat = (iMaterialWrapper *)calModel.getVectorMesh()[
		meshId]->getSubmesh(submeshId)->getCoreMaterialId();
	DrawSubmesh(g3d,rview,pCalRenderer,meshId,submeshId,mat);
      }
    }
  }

  pCalRenderer->endRendering();

  return true;
#else
  return false;
#endif
}

bool csSpriteCal3DMeshObject::Advance (csTicks current_time)
{
  // update anim frames, etc. here
  float delta = ((float)current_time - last_update_time)/1000.0F;
  if (!current_time)
    delta = 0;
  calModel.update(delta);
  if (current_time)
    last_update_time = current_time;

  if (is_idling) // check for override and play if time
  {
    idle_override_interval -= delta;
    if (idle_override_interval <= 0)
    {
      SetAnimAction(factory->anims[idle_action]->name,.25,.25);
      idle_override_interval = 20;
    }
  }
  meshVersion++;
  lighting_dirty = true;
  return true;
}

//--------------------------------------------------------------------------

csMeshedPolygon* csSpriteCal3DMeshObject::PolyMesh::GetPolygons ()
{
  return 0;
}

#ifndef CS_USE_NEW_RENDERER
void csSpriteCal3DMeshObject::eiVertexBufferManagerClient::ManagerClosing ()
{
  if (scfParent->vbuf)
  {
    scfParent->vbuf = 0;
    scfParent->vbufmgr = 0;
  }
}
#endif

int csSpriteCal3DMeshObject::GetAnimCount()
{
  return calModel.getCoreModel()->getCoreAnimationCount();
}

const char *csSpriteCal3DMeshObject::GetAnimName(int idx)
{
  if (idx >= GetAnimCount())
    return 0;

  return factory->anims[idx]->name;
}

int csSpriteCal3DMeshObject::GetAnimType(int idx)
{
  if (idx >= GetAnimCount())
    return 0;

  return factory->anims[idx]->type;
}

int csSpriteCal3DMeshObject::FindAnim(const char *name)
{
  int count = GetAnimCount();

  for (int i=0; i<count; i++)
  {
    if (factory->anims[i]->name == name)
      return i;
  }
  return -1;
}

void csSpriteCal3DMeshObject::ClearAllAnims()
{
  while (active_anims.Length())
  {
    csCal3DAnimation *pop = active_anims.Pop();
    ClearAnimCycle(pop->index,0);
    // do not delete pop because ptr is shared with factory
    active_weights.Pop();
  }
}

bool csSpriteCal3DMeshObject::SetAnimCycle(const char *name, float weight)
{
  ClearAllAnims();
  return AddAnimCycle(name, weight, 0);
}

bool csSpriteCal3DMeshObject::AddAnimCycle(const char *name, float weight,
	float delay)
{
  int idx = FindAnim(name);
  if (idx == -1)
    return false;

  AddAnimCycle(idx,weight,delay);

  Advance(0);
  return true;
}

bool csSpriteCal3DMeshObject::AddAnimCycle(int idx, float weight, float delay)
{
  calModel.getMixer()->blendCycle(idx,weight,delay);

  active_anims.Push(factory->anims[idx]);
  active_weights.Push(weight);
  return true;
}

void csSpriteCal3DMeshObject::ClearAnimCycle(int idx, float delay)
{
  calModel.getMixer()->clearCycle(idx,delay);
}

bool csSpriteCal3DMeshObject::ClearAnimCycle(const char *name, float delay)
{
  int idx = FindAnim(name);
  if (idx == -1)
    return false;

  ClearAnimCycle(idx,delay);

  return true;
}

int csSpriteCal3DMeshObject::GetActiveAnimCount()
{
  return active_anims.Length();
}

int csSpriteCal3DMeshObject::GetActiveAnims(char *buffer,int max_length)
{
  int count=0,i;

  for (i=0; i<active_anims.Length() && count<max_length-1; i++)
  {
    buffer[count++] = active_anims[i]->index;
    // will not work for % weights, but only for weights >= 1.
    buffer[count++] = (char)active_weights[i];
  }
  return i==active_anims.Length();  // true if successful
}

void csSpriteCal3DMeshObject::SetActiveAnims(const char *buffer,int anim_count)
{
  ClearAllAnims();

  for (int i=0; i<anim_count; i++)
  {
    AddAnimCycle(buffer[i*2],buffer[i*2+1],0);
  }
}

bool csSpriteCal3DMeshObject::SetAnimAction(const char *name, float delayIn,
	float delayOut)
{
  int idx = FindAnim(name);
  if (idx == -1)
    return false;

  calModel.getMixer()->executeAction(idx,delayIn,delayOut,
  	1 /* ,factory->anims[idx]->lock */ );

  return true;
}

void csSpriteCal3DMeshObject::SetIdleOverrides(csRandomGen *rng,int which)
{
  csCal3DAnimation *anim = factory->anims[which];

  is_idling = true;

  // Determine interval till next override.
  idle_override_interval = rng->Get(anim->max_interval - anim->min_interval)
  	+ anim->min_interval;

  // Determine which idle override will be played.
  int odds = rng->Get(100);
  idle_action = 0;
  for (int i=0; i<GetAnimCount(); i++)
  {
    if (factory->anims[i]->idle_pct > odds)
    {
      idle_action = i;
      return;
    }
    else
    {
      odds -= factory->anims[i]->idle_pct;
    }
  }
}

bool csSpriteCal3DMeshObject::SetVelocity(float vel,csRandomGen *rng)
{
  int count = GetAnimCount();
  int i;
  ClearAllAnims();
  if (!vel)
  {
    for (i=0; i<count; i++)
    {
      if (factory->anims[i]->type == iSpriteCal3DState::C3D_ANIM_TYPE_IDLE)
      {
        AddAnimCycle(i,1,0);
	if (rng)
	  SetIdleOverrides(rng,i);
        return true;
      }
    }
  }

  bool reversed = (vel<0);
  if (reversed)
    vel = -vel;

  is_idling = false;
  for (i=0; i<count; i++)
  {
    if (factory->anims[i]->type == iSpriteCal3DState::C3D_ANIM_TYPE_TRAVEL)
    {
      if (vel < factory->anims[i]->min_velocity ||
          vel > factory->anims[i]->max_velocity)
    	continue;

      float pct,vel_diff;
      if (vel == factory->anims[i]->base_velocity)
	      pct = 1;
      else if (vel < factory->anims[i]->base_velocity)
      {
    	vel_diff = factory->anims[i]->base_velocity
		- factory->anims[i]->min_velocity;
	pct = 1 - (vel - factory->anims[i]->min_velocity) / vel_diff;
      }
      else
      {
    	vel_diff = factory->anims[i]->max_velocity
		- factory->anims[i]->base_velocity;
	pct = (factory->anims[i]->max_velocity - vel) / vel_diff;
      }
      AddAnimCycle(i,pct,0);
//      printf("  Adding %s weight=%1.2f\n",factory->anims[i]->name.GetData(),pct);

      if (pct == 1)
    	break;
    }
  }
  if (reversed)
    SetTimeFactor(-1);
  else
    SetTimeFactor(1);

  return true;    
}

void csSpriteCal3DMeshObject::SetLOD(float lod)
{
  calModel.setLodLevel(lod);
}

bool csSpriteCal3DMeshObject::AttachCoreMesh(const char *meshname)
{
  int idx = factory->FindMeshName(meshname);
  if (idx == -1)
    return false;

  return AttachCoreMesh(factory->submeshes[idx]->index,
  	(int)factory->submeshes[idx]->default_material);
}

bool csSpriteCal3DMeshObject::AttachCoreMesh(int mesh_id,int iMatWrapID)
{  
  if ( attached_ids.Find( mesh_id ) != -1 )
    return true;
    
  if (!calModel.attachMesh(mesh_id))
    return false;

  // Since all our vertex buffers and G3DTriangles are in arrays
  // which line up with the attached submesh list on the calModel,
  // we need to track these id's in the identical way so that
  // we can remove the right G3DTriangles when the submesh is
  // removed.

  attached_ids.Push(mesh_id);
  SetupObjectSubmesh(attached_ids.Length()-1);
  CalMesh *mesh = calModel.getMesh(mesh_id);
  for (int i=0; i<mesh->getSubmeshCount(); i++)
  {
    mesh->getSubmesh(i)->setCoreMaterialId(iMatWrapID);
  }
  //    mesh->setMaterialSet(0);
  return true;
}

bool csSpriteCal3DMeshObject::DetachCoreMesh(const char *meshname)
{
  int idx = factory->FindMeshName(meshname);
  if (idx == -1)
    return false;

  return DetachCoreMesh(factory->submeshes[idx]->index);
}

bool csSpriteCal3DMeshObject::DetachCoreMesh (int mesh_id)
{
#ifndef CS_USE_NEW_RENDERER
  if (!calModel.detachMesh(mesh_id))
    return false;

  // Now that the submesh is removed from the model, we must
  // remove all the CS rendering structures as well.
  int i;
  for (i=0; i<attached_ids.Length(); i++)
  {
    if (attached_ids[i] == mesh_id)
    {
      meshes[i].DeleteAll();
      is_initialized[i].DeleteAll();
      meshes_colors[i].DeleteAll();

      int j;
      for (j=i+1; j<attached_ids.Length(); j++)
      {
        meshes[j-1] = meshes[j];
        is_initialized[j-1] = is_initialized[j];
        meshes_colors[j-1]  = meshes_colors[j];
      }
      meshes[j-1].DeleteAll();
      is_initialized[j-1].DeleteAll();
      meshes_colors[j-1].DeleteAll();

      attached_ids.DeleteIndex(i);
      break;
    }
  }
#endif
  return true;
}

bool csSpriteCal3DMeshObject::BlendMorphTarget(int morph_animation_id,
	float weight, float delay)
{
  if(morph_animation_id < 0||
  	factory->morph_animation_names.Length() <= morph_animation_id)
  {
    return false;
  }
  return calModel.getMorphTargetMixer()->blend(morph_animation_id,weight,delay);
}

bool csSpriteCal3DMeshObject::ClearMorphTarget(int morph_animation_id,
	float delay)
{
  if(morph_animation_id < 0||
  	factory->morph_animation_names.Length() <= morph_animation_id)
  {
    return false;
  }
  return calModel.getMorphTargetMixer()->clear(morph_animation_id,delay);
}

csSpriteCal3DSocket* csSpriteCal3DMeshObject::AddSocket ()
{
  csSpriteCal3DSocket* socket = new csSpriteCal3DSocket();
  sockets.Push (socket);
  return socket;
}

csSpriteCal3DSocket* csSpriteCal3DMeshObject::FindSocket (const char *n) const
{
  int i;
  for (i = GetSocketCount () - 1; i >= 0; i--)
    if (strcmp (GetSocket (i)->GetName (), n) == 0)
      return GetSocket (i);

  return 0;
}

csSpriteCal3DSocket* csSpriteCal3DMeshObject::FindSocket (
	iMeshWrapper *mesh) const
{
  int i;
  for (i = GetSocketCount () - 1; i >= 0; i--)
    if (GetSocket (i)->GetMeshWrapper() == mesh)
      return GetSocket (i);

  return 0;
}

bool csSpriteCal3DMeshObject::SetMaterial(const char *mesh_name,
	iMaterialWrapper *mat)
{
  int idx = factory->FindMeshName(mesh_name);
  if (idx == -1)
    return false;
          
  int i,j;
  for (i=0; i<attached_ids.Length(); i++)
  {
    if (attached_ids[i] == idx)
    {
      CalMesh *mesh = calModel.getMesh(attached_ids[i]);
      for (j=0; j<mesh->getSubmeshCount(); j++)
      {
        mesh->getSubmesh(j)->setCoreMaterialId((int)mat);
      }
      return true;
    }
  }
  return false;
}

void csSpriteCal3DMeshObject::SetTimeFactor(float timeFactor)
{
  calModel.getMixer()->setTimeFactor(timeFactor);
}

float csSpriteCal3DMeshObject::GetTimeFactor()
{
  return calModel.getMixer()->getTimeFactor();
}


//----------------------------------------------------------------------

#ifdef CS_USE_NEW_RENDERER
SCF_IMPLEMENT_IBASE(csSpriteCal3DMeshObject::BaseAccessor)
SCF_IMPLEMENT_IBASE_END

void csSpriteCal3DMeshObject::BaseAccessor::PreGetValue (
  csShaderVariable *variable)
{
  const csStringID id = variable->Name;
  if ((id == csSpriteCal3DMeshObjectFactory::index_name) || 
    (id == csSpriteCal3DMeshObjectFactory::vertex_name) ||
    (id == csSpriteCal3DMeshObjectFactory::texel_name) ||
    (id == csSpriteCal3DMeshObjectFactory::normal_name) ||
    (id == csSpriteCal3DMeshObjectFactory::color_name))
  {
    if (meshobj->meshVersion != meshVersion)
    {
      CalRenderer* render = meshobj->calModel.getRenderer();
      render->beginRendering();
      render->selectMeshSubmesh (mesh, submesh);

      int indexCount = render->getFaceCount() * 3;
      if ((index_buffer == 0) || (index_size < indexCount))
      {
	// @@@ How often do those change?
	index_buffer = meshobj->G3D->CreateRenderBuffer (
	  sizeof (uint) * indexCount, CS_BUF_DYNAMIC,
	  CS_BUFCOMP_UNSIGNED_INT, 1, true);
	index_size = indexCount;
      }
      uint* indices = (uint*)index_buffer->Lock (CS_BUF_LOCK_NORMAL);
      render->getFaces ((CalIndex*)indices);
      index_buffer->Release ();

      int vertexCount = render->getVertexCount ();
      if ((vertex_buffer == 0) || (vertex_size < vertexCount))
      {
	vertex_buffer = meshobj->G3D->CreateRenderBuffer (
	  sizeof (float) * vertexCount * 3, CS_BUF_DYNAMIC,
	  CS_BUFCOMP_FLOAT, 3, false);
	normal_buffer = meshobj->G3D->CreateRenderBuffer (
	  sizeof (float) * vertexCount * 3, CS_BUF_DYNAMIC,
	  CS_BUFCOMP_FLOAT, 3, false);
	texel_buffer = meshobj->G3D->CreateRenderBuffer (
	  sizeof (float) * vertexCount * 2, CS_BUF_DYNAMIC,
	  CS_BUFCOMP_FLOAT, 2, false);
	color_buffer = meshobj->G3D->CreateRenderBuffer (
	  sizeof (float) * vertexCount * 3, CS_BUF_DYNAMIC,
	  CS_BUFCOMP_FLOAT, 3, false);
	vertex_size = vertexCount;
      }

      float* vertices = (float*)vertex_buffer->Lock (CS_BUF_LOCK_NORMAL);
      render->getVertices (vertices);
      vertex_buffer->Release ();

      CS_ALLOC_STACK_ARRAY(float, normals, vertexCount * 3);
      render->getNormals (normals);

      const csArray<iLight*>& relevant_lights = meshobj->factory->light_mgr
              ->GetRelevantLights (meshobj->logparent, -1, false);

      meshobj->UpdateLightingSubmesh (relevant_lights, 
                                      meshobj->currentMovable,
                                      render,
                                      mesh,
                                      submesh,
                                      normals);

      normal_buffer->CopyToBuffer (normals, sizeof (float) * vertexCount * 3);

      float* texels = (float*)texel_buffer->Lock (CS_BUF_LOCK_NORMAL);
      render->getTextureCoordinates (0, texels);
      texel_buffer->Release ();

      color_buffer->CopyToBuffer (meshobj->meshes_colors[mesh][submesh],
	                                sizeof (float) * vertexCount * 3);

      render->endRendering();

      meshVersion = meshobj->meshVersion;
    }

    if (id == csSpriteCal3DMeshObjectFactory::index_name)
    {
      variable->SetValue (index_buffer);
    }
    else if (id == csSpriteCal3DMeshObjectFactory::vertex_name)
    {
      variable->SetValue (vertex_buffer);
    }
    else if (id == csSpriteCal3DMeshObjectFactory::normal_name)
    {
      variable->SetValue (normal_buffer);
    }
    else if (id == csSpriteCal3DMeshObjectFactory::texel_name)
    {
      variable->SetValue (texel_buffer);
    }
    else if (id == csSpriteCal3DMeshObjectFactory::color_name)
    {
      variable->SetValue (color_buffer);
    }
  }
  //variable->SetValue (meshobj->GetRenderBuffer (mesh, submesh,
  //  variable->Name));
}
#endif

//----------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csSpriteCal3DMeshObjectType)
SCF_IMPLEMENTS_INTERFACE (iMeshObjectType)
SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iConfig)
SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iLODControl)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSpriteCal3DMeshObjectType::eiComponent)
SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSpriteCal3DMeshObjectType::csSpriteCal3DConfig)
SCF_IMPLEMENTS_INTERFACE (iConfig)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSpriteCal3DMeshObjectType::LODControl)
SCF_IMPLEMENTS_INTERFACE (iLODControl)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csSpriteCal3DMeshObjectType)

csSpriteCal3DMeshObjectType::csSpriteCal3DMeshObjectType (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiConfig);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiLODControl);
}

csSpriteCal3DMeshObjectType::~csSpriteCal3DMeshObjectType ()
{
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiLODControl);
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiConfig);
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiComponent);
  SCF_DESTRUCT_IBASE ();
}

bool csSpriteCal3DMeshObjectType::Initialize (iObjectRegistry* object_reg)
{
  csSpriteCal3DMeshObjectType::object_reg = object_reg;
  vc = CS_QUERY_REGISTRY (object_reg, iVirtualClock);
  csRef<iEngine> eng = CS_QUERY_REGISTRY (object_reg, iEngine);
  // We don't want to keep a reference to the engine (circular ref otherwise).
  engine = eng;
  return true;
}

csPtr<iMeshObjectFactory> csSpriteCal3DMeshObjectType::NewFactory ()
{
  csSpriteCal3DMeshObjectFactory* cm = new csSpriteCal3DMeshObjectFactory (
    this, object_reg);
  cm->vc = vc;
  cm->engine = engine;
#ifdef CS_USE_NEW_RENDERER
  cm->g3d = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
  //cm->anon_buffers = new csAnonRenderBufferManager (object_reg);
#endif
  csRef<iMeshObjectFactory> ifact (
    SCF_QUERY_INTERFACE (cm, iMeshObjectFactory));
  cm->DecRef ();
  return csPtr<iMeshObjectFactory> (ifact);
}
