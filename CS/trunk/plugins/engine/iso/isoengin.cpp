/*
    Copyright (C) 2001 by W.C.A. Wijngaards

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
#include "cssys/sysfunc.h"
#include "csutil/util.h"
#include "igraphic/imageio.h"
#include "igraphic/image.h"
#include "imesh/object.h"
#include "isoengin.h"
#include "isolight.h"
#include "isomesh.h"
#include "isospr.h"
#include "isoview.h"
#include "isoworld.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "iutil/plugin.h"
#include "iutil/vfs.h"
#include "iutil/cfgmgr.h"
#include "iutil/evdefs.h"
#include "iutil/event.h"
#include "iutil/eventq.h"
#include "iutil/object.h"
#include "iutil/objreg.h"
#include "ivaria/reporter.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/txtmgr.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE (csIsoEngine)
  SCF_IMPLEMENTS_INTERFACE (iIsoEngine)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_FACTORY (csIsoEngine)

SCF_IMPLEMENT_EMBEDDED_IBASE (csIsoEngine::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csIsoEngine::EventHandler)
  SCF_IMPLEMENTS_INTERFACE (iEventHandler)
SCF_IMPLEMENT_IBASE_END

SCF_EXPORT_CLASS_TABLE (iso)
  SCF_EXPORT_CLASS_DEP (csIsoEngine, "crystalspace.engine.iso",
    "Crystal Space Isometric Engine",
    "crystalspace.kernel., crystalspace.graphics3d., crystalspace.graphics2d.")
SCF_EXPORT_CLASS_TABLE_END

void csIsoEngine::Report (int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csRef<iReporter> rep (CS_QUERY_REGISTRY (object_reg, iReporter));
  if (rep)
    rep->ReportV (severity, "crystalspace.engine.iso", msg, arg);
  else
  {
    csPrintfV (msg, arg);
    csPrintf ("\n");
  }
  va_end (arg);
}

csIsoEngine::csIsoEngine (iBase *iParent)
{
  SCF_CONSTRUCT_IBASE (iParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  scfiEventHandler = 0;
  object_reg = 0;
  txtmgr = 0;
  world = 0;
}

csIsoEngine::~csIsoEngine ()
{
  if (scfiEventHandler)
  {
    csRef<iEventQueue> q (CS_QUERY_REGISTRY(object_reg, iEventQueue));
    if (q != 0)
      q->RemoveListener (scfiEventHandler);
    scfiEventHandler->DecRef ();
  }
  materials.scfiMaterialList.RemoveAll ();
  meshfactories.scfiMeshFactoryList.RemoveAll();
}

bool csIsoEngine::Initialize (iObjectRegistry* p)
{
  object_reg = p;
  // Tell system driver that we want to handle broadcast events
  if (!scfiEventHandler)
    scfiEventHandler = new EventHandler (this);
  csRef<iEventQueue> q (CS_QUERY_REGISTRY(object_reg, iEventQueue));
  if (q != 0)
    q->RegisterListener (scfiEventHandler, CSMASK_Broadcast);
  return true;
}

bool csIsoEngine::HandleEvent (iEvent& Event)
{
  // Handle some system-driver broadcasts
  if (Event.Type == csevBroadcast)
    switch (Event.Command.Code)
    {
      case cscmdSystemOpen:
      {
        // system is open we can get ptrs now
        g3d = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
        if (!g3d)
        {
          Report (CS_REPORTER_SEVERITY_ERROR, "IsoEngine: could not get G3D.");
          return false;
        }
        g2d = g3d->GetDriver2D ();
        if (!g2d)
        {
          Report (CS_REPORTER_SEVERITY_ERROR, "IsoEngine: could not get G2D.");
          return false;
        }
        txtmgr = g3d->GetTextureManager();
        if (!txtmgr)
        {
          Report (CS_REPORTER_SEVERITY_ERROR,
            "IsoEngine: could not get TextureManager.");
          return false;
        }
        return true;
      }
      case cscmdSystemClose:
      {
        // We must free all material and texture handles since after
        // G3D->Close() they all become invalid, no matter whenever
        // we did or didn't an IncRef on them.
	materials.scfiMaterialList.RemoveAll ();
        return true;
      }
      case cscmdContextResize:
      {
        return false;
      }
      case cscmdContextClose:
      {
        return false;
      }
    } /* endswitch */

  return false;
}

iIsoWorld* csIsoEngine::CreateWorld()
{
  return new csIsoWorld(this);
}

iIsoView* csIsoEngine::CreateView(iIsoWorld *world)
{
  return new csIsoView(this, this, world);
}

iIsoSprite* csIsoEngine::CreateSprite()
{
  return new csIsoSprite(this);
}

iIsoMeshSprite* csIsoEngine::CreateMeshSprite()
{
  return new csIsoMeshSprite(this);
}

int csIsoEngine::GetBeginDrawFlags () const
{
  return CSDRAW_CLEARZBUFFER;
}

iIsoSprite* csIsoEngine::CreateFloorSprite(const csVector3& pos, float w,
      float h)
{
  iIsoSprite *spr = new csIsoSprite(this);
  spr->AddVertex(csVector3(0,0,0), 0, 0);
  spr->AddVertex(csVector3(0, 0, w), 1, 0);
  spr->AddVertex(csVector3(h, 0, w), 1, 1);
  spr->AddVertex(csVector3(h, 0, 0), 0, 1);
  spr->SetPosition(pos);
  return spr;
}

iIsoSprite* csIsoEngine::CreateFrontSprite(const csVector3& pos, float w,
      float h)
{
  iIsoSprite *spr = new csIsoSprite(this);
  float hw = w * 0.25;
  spr->AddVertex(csVector3(-hw, 0,-hw), 0, 0);
  spr->AddVertex(csVector3(-hw, h, -hw), 1, 0);
  spr->AddVertex(csVector3(+hw, h, +hw), 1, 1);
  spr->AddVertex(csVector3(+hw, 0, +hw), 0, 1);
  spr->SetPosition(pos);
  return spr;
}

iIsoSprite* csIsoEngine::CreateZWallSprite(const csVector3& pos, float w,
  float h)
{
  iIsoSprite *spr = new csIsoSprite(this);
  spr->AddVertex(csVector3(0, 0, 0), 0, 0);
  spr->AddVertex(csVector3(0, h, 0), 1, 0);
  spr->AddVertex(csVector3(0, h, w), 1, 1);
  spr->AddVertex(csVector3(0, 0, w), 0, 1);
  spr->SetPosition(pos);
  return spr;
}

iIsoSprite* csIsoEngine::CreateXWallSprite(const csVector3& pos, float w,
  float h)
{
  iIsoSprite *spr = new csIsoSprite(this);
  spr->AddVertex(csVector3(0, 0, 0), 0, 0);
  spr->AddVertex(csVector3(0, h, 0), 1, 0);
  spr->AddVertex(csVector3(w, h, 0), 1, 1);
  spr->AddVertex(csVector3(w, 0, 0), 0, 1);
  spr->SetPosition(pos);
  return spr;
}

iIsoLight* csIsoEngine::CreateLight()
{
  return new csIsoLight(this);
}

iMaterialWrapper *csIsoEngine::CreateMaterialWrapper(const char *vfsfilename,
	          const char *materialname)
{
  csRef<iImageIO> imgloader;
  csRef<iVFS> VFS;
  csRef<iDataBuffer> buf;
  csRef<iImage> image;
  csRef<iTextureHandle> handle;
  csIsoMaterial *material = 0;
  csRef<iMaterialHandle> math;
  iMaterialWrapper *mat_wrap = 0;

  imgloader = CS_QUERY_REGISTRY (object_reg, iImageIO);
  if(imgloader==0)
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Could not get image loader plugin. "
    	"Failed to load file %s.", vfsfilename);
    return 0;
  }

  VFS = CS_QUERY_REGISTRY (object_reg, iVFS);
  if(VFS==0)
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Could not get VFS plugin. "
    	"Failed to load file %s.", vfsfilename);
    return 0;
  }

  buf = VFS->ReadFile (vfsfilename);
  if(!buf)
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Could not read vfs file %s\n",
      vfsfilename);
    return 0;
  }

  image = imgloader->Load(buf->GetUint8 (), buf->GetSize (),
			  txtmgr->GetTextureFormat ());
  if(!image)
  {
    Report (CS_REPORTER_SEVERITY_ERROR,
      "The imageloader could not load image %s", vfsfilename);
    return 0;
  }

  handle = txtmgr->RegisterTexture (image, CS_TEXTURE_2D | CS_TEXTURE_3D);
  if(!handle)
  {
    Report (CS_REPORTER_SEVERITY_ERROR,
      "Texturemanager could not register texture %s", vfsfilename);
    return 0;
  }

  material = new csIsoMaterial(handle);
  math = txtmgr->RegisterMaterial(material);
  if (math)
  {
    mat_wrap = materials.scfiMaterialList.NewMaterial (math);
    mat_wrap->IncRef ();	// Jorrit: @@@ Not sure why this is needed?
    mat_wrap->QueryObject ()->SetName (materialname);
  }
  else
  {
    Report (CS_REPORTER_SEVERITY_ERROR,
      "Texturemanager could not register material %s", materialname);
    return 0;
  }

  return mat_wrap;
}

iMeshFactoryWrapper *csIsoEngine::CreateMeshFactory(const char* classId,
    const char *name)
{
  csRef<iMeshObjectFactory> mesh_fact;
  csRef<iMeshObjectType> mesh_type;

  if (name)
  {
    iMeshFactoryWrapper* wrap = meshfactories.
    	scfiMeshFactoryList.FindByName (name);
    if (wrap)
      return wrap;
  }

  csRef<iPluginManager> plugin_mgr (
  	CS_QUERY_REGISTRY (object_reg, iPluginManager));
  mesh_type = CS_QUERY_PLUGIN_CLASS (plugin_mgr, classId, iMeshObjectType);

  if(!mesh_type)
    mesh_type = CS_LOAD_PLUGIN (plugin_mgr, classId, iMeshObjectType);
  if(!mesh_type)
    return 0;

  csIsoMeshFactoryWrapper* wrap = 0;
  mesh_fact = mesh_type->NewFactory ();
  if (mesh_fact)
  {
    //AddMeshFactory (mesh_fact, name);
    //mesh_fact->DecRef ();
    wrap = new csIsoMeshFactoryWrapper (mesh_fact);
    csRef<iObject> obj (SCF_QUERY_INTERFACE (wrap, iObject));
    obj->SetName (name);
    meshfactories.scfiMeshFactoryList.Add (&(wrap->scfiMeshFactoryWrapper));
    wrap->DecRef ();
    return &(wrap->scfiMeshFactoryWrapper);
  }
  return 0;
}

iMeshFactoryWrapper *csIsoEngine::CreateMeshFactory (const char *name)
{
  if (name)
  {
    iMeshFactoryWrapper* mfw = meshfactories.
      scfiMeshFactoryList.FindByName (name);
    if (mfw)
      return mfw;
  }

  csIsoMeshFactoryWrapper *mfw = new csIsoMeshFactoryWrapper ();
  if (name) 
    mfw->SetName (name);
  meshfactories.scfiMeshFactoryList.Add (&(mfw->scfiMeshFactoryWrapper));
  mfw->DecRef ();

  return &mfw->scfiMeshFactoryWrapper;
}

