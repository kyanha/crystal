/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
   $Id$

    This file is part of Crystal Space Virtual Object System Abstract
    3D Layer plugin (csvosa3dl).

    Copyright (C) 2004 Peter Amstutz

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "cssysdef.h"
#include "csvosa3dl.h"
#include "vossector.h"
#include "vosobject3d.h"
#include "voscube.h"
#include "voscone.h"
#include "vosbillboard.h"
#include "vostexture.h"
#include "vosmaterial.h"
#include "vospolygonmesh.h"
#include "voslight.h"

#include <vos/metaobjects/a3dl/a3dl.hh>

using namespace VOS;
using namespace A3DL;

SCF_IMPLEMENT_IBASE (csVosA3DL)
  SCF_IMPLEMENTS_INTERFACE (iVosA3DL)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
  SCF_IMPLEMENTS_INTERFACE (iEventHandler)
SCF_IMPLEMENT_IBASE_END

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (csVosA3DL)

/// csVosA3DL ///

csVosA3DL::csVosA3DL (iBase *parent)
{
  SCF_CONSTRUCT_IBASE (parent);
}

csVosA3DL::~csVosA3DL()
{
  SCF_DESTRUCT_IBASE();
}

csRef<iVosSector> csVosA3DL::GetSector(const char* s)
{
  csRef<iVosSector> r;
  r.AttachNew(new csVosSector(objreg, this, s));
  return r;
}

bool csVosA3DL::Initialize (iObjectRegistry *o)
{
  LOG("csVosA3DL", 2, "Initializing");

  Site::removeRemoteMetaObjectFactory("a3dl:object3D", &A3DL::Object3D::new_Object3D);
  Site::removeRemoteMetaObjectFactory("a3dl:object3D.cube", &A3DL::Cube::new_Cube);
  Site::removeRemoteMetaObjectFactory("a3dl:object3D.cone", &A3DL::Cone::new_Cone);
  Site::removeRemoteMetaObjectFactory("a3dl:object3D.polygonmesh", &A3DL::PolygonMesh::new_PolygonMesh);
  Site::removeRemoteMetaObjectFactory("a3dl:object3D.billboard", &A3DL::Billboard::new_Billboard);
  Site::removeRemoteMetaObjectFactory("a3dl:texture", &A3DL::Texture::new_Texture);
  Site::removeRemoteMetaObjectFactory("a3dl:material", &A3DL::Material::new_Material);
  Site::removeRemoteMetaObjectFactory("a3dl:light", &A3DL::Light::new_Light);

  Site::addRemoteMetaObjectFactory("a3dl:object3D", "a3dl:object3D",
                                   &csMetaObject3D::new_csMetaObject3D);
  Site::addRemoteMetaObjectFactory("a3dl:object3D.cube", "a3dl:object3D.cube",
                                   &csMetaCube::new_csMetaCube);
  Site::addRemoteMetaObjectFactory("a3dl:object3D.cone", "a3dl:object3D.cone",
                                   &csMetaCone::new_csMetaCone);
  Site::addRemoteMetaObjectFactory("a3dl:object3D.polygonmesh", "a3dl:object3D.polygonmesh",
                                   &csMetaPolygonMesh::new_csMetaPolygonMesh);
  Site::addRemoteMetaObjectFactory("a3dl:object3D.billboard", "a3dl:object3D.billboard",
                                   &csMetaBillboard::new_csMetaBillboard);
  Site::addRemoteMetaObjectFactory("a3dl:texture", "a3dl:texture",
                                   &csMetaTexture::new_csMetaTexture);
  Site::addRemoteMetaObjectFactory("a3dl:material", "a3dl:material",
                                   &csMetaMaterial::new_csMetaMaterial);
  Site::addRemoteMetaObjectFactory("a3dl:light", "a3dl:light",
                                   &csMetaLight::new_csMetaLight);

  objreg = o;

  csMetaMaterial::object_reg = objreg;

  eventq = CS_QUERY_REGISTRY (objreg, iEventQueue);
  if (! eventq) return false;
  eventq->RegisterListener (this, CSMASK_FrameProcess);

  localsite.assign(new Site(true), false);
  localsite->addSiteExtension(new LocalSocketSiteExtension());

  return true;
}

bool csVosA3DL::HandleEvent (iEvent &ev)
{
  if (ev.Type == csevBroadcast && ev.Command.Code == cscmdProcess)
  {
    while(! mainThreadTasks.empty())
    {
      Task* t = mainThreadTasks.pop();
      t->doTask();
      delete t;
    }
  }
  return false;
}
