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

#include "iengine/mesh.h"
#include "imesh/sprite3d.h"
#include "imesh/object.h"
#include "imesh/mdlconv.h"
#include "imesh/mdldata.h"
#include "imesh/crossbld.h"
#include "cstool/mdltool.h"
#include "csutil/databuf.h"
#include "igeom/objmodel.h"

#include "csvosa3dl.h"
#include "vosmodel.h"
#include "vosmaterial.h"

using namespace VOS;

class ConstructModelTask : public Task
{
public:
  iObjectRegistry *object_reg;
  vRef<csMetaMaterial> metamat;
  vRef<Property> property;
  csRef<iDataBuffer> databuf;
  vRef<csMetaModel> model;
  std::string name;
  csRef<iSector> sector;

  ConstructModelTask(iObjectRegistry *objreg, vRef<csMetaMaterial> mat,
                     vRef<Property> p, csMetaModel* m, std::string n, iSector *s);
  virtual ~ConstructModelTask();
  virtual void doTask();
};

ConstructModelTask::ConstructModelTask (iObjectRegistry *objreg,
                      vRef<csMetaMaterial> mat, vRef<Property> p,
                      csMetaModel* m, std::string n, iSector *s)
  : object_reg(objreg), metamat(mat), property(p), model(m, true), name(n), sector(s)
{
}

ConstructModelTask::~ConstructModelTask()
{
}

#define _MAX(a, b) ((a > b) ? a : b)
static void NormalizeModel(csRef<iMeshWrapper> wrapper, bool recenter, 
                           bool fixalign, std::string datatype)
{
  csBox3 b;

  if (!wrapper || !wrapper->GetMeshObject() ||
      !wrapper->GetMeshObject()->GetObjectModel())
  {
    LOG ("NormalizeModel", 2, "Bad factory wrapper passed to function");
    return;
  }

  wrapper->GetMeshObject()->GetObjectModel()->GetObjectBoundingBox(b);

  LOG("NormalizeModel", 3, "maxes (" << b.MaxX() << ", " << b.MaxY() << 
      ", " << b.MaxZ() << ")");
  LOG("NormlizeModel", 3, "mins (" << b.MinX() << ", " << b.MinY() << 
      ", " << b.MinZ() << ")");

  float xextent = b.MaxX() - b.MinX();
  float yextent = b.MaxY() - b.MinY();
  float zextent = b.MaxZ() - b.MinZ();

  float scale = 1.0 / _MAX(xextent, _MAX(yextent, zextent));
  LOG("NormalizeModel", 3, "scaling extents of (" << xextent << 
      ", " << yextent << ", " << zextent << ") by " << scale);

  csVector3 newcenter(0,0,0);
  if (recenter)
  {
    newcenter.Set((b.MaxX() + b.MinX())/2,
                  (b.MaxY() + b.MinY())/2,
                  (b.MaxZ() + b.MinZ())/2);
    newcenter *= -scale;
    LOG("NormalizeModel", 4, "setting center to " << newcenter.x << 
        ", " << newcenter.y << ", " << newcenter.z << ")");
  }

  scale = 1.0 / scale;
  csReversibleTransform rt (csZScaleMatrix3 (scale) * csYScaleMatrix3 (scale) *
                            csXScaleMatrix3 (scale), newcenter);

  if (fixalign)
  {
    if(datatype == "model/md2") rt.SetO2T(csYRotMatrix3(-M_PI/2) * rt.GetO2T());
  }

  wrapper->GetFactory()->HardTransform (rt);
}
#undef _MAX

void ConstructModelTask::doTask()
{
  // Create databuffer for model.  Need a new buffer for this as CS will
  // delete[] it
  csRef<iDataBuffer> databuf;
  std::string s;
  property->read(s);
  char *buffer = new char[s.size()];
  for (std::string::size_type i = 0; i < s.size(); i++) buffer[i] = s[i];
  databuf.AttachNew (new csDataBuffer (buffer, s.size()));
  
  // Engine
  csRef<iEngine> engine = CS_QUERY_REGISTRY (object_reg, iEngine);

  // Get the model converter and cross builder
  csRef<iModelConverter> modconv = CS_QUERY_REGISTRY (object_reg, iModelConverter);
  csRef<iCrossBuilder> xbuild = CS_QUERY_REGISTRY (object_reg, iCrossBuilder);

  // Check they were loaded
  if (!modconv || !xbuild)
  {
    LOG ("ConstructModelTask", 2, "Need model converter and cross builder, " <<
         "ignoring model (" << name << ")");
  return;
  }

  LOG ("ConstructModelTask", 2, "Loading into model converter");
  csRef<iModelData> data = modconv->Load (databuf->GetUint8(), databuf->GetSize());
  if (!data)
  {
    LOG ("ConstructModelTask", 2, "Could not load model using converter");
    return;
  }

  LOG ("ConstructModelTask", 2, "Splitting objects");
  csModelDataTools::SplitObjectsByMaterial (data);
  csModelDataTools::MergeObjects (data, false);

  LOG ("ConstructModelTask", 2, "Creating factory");
  csRef<iMeshFactoryWrapper> factory = xbuild->BuildSpriteFactoryHierarchy (
          data, engine, metamat->GetMaterialWrapper());
  if (!factory)
  {
    LOG ("ConstructModelTask", 2, "Could not build factory");
    return;
  }
  else factory->QueryObject()->SetName (name.c_str());

  csRef<iMeshWrapper> meshwrapper = engine->CreateMeshWrapper (
                             factory, name.c_str(), sector, csVector3(0, 0, 0));

  NormalizeModel (meshwrapper, true, true, property->getDataType());

  model->GetCSinterface()->SetMeshWrapper(meshwrapper);
}

/// csMetaModel ///

csMetaModel::csMetaModel(VobjectBase* superobject)
  : A3DL::Object3D(superobject),
    csMetaObject3D(superobject),
    A3DL::Model(superobject)
{
}

MetaObject* csMetaModel::new_csMetaModel(VobjectBase* superobject, const std::string& type)
{
  return new csMetaModel(superobject);
}

void csMetaModel::Setup(csVosA3DL* vosa3dl, csVosSector* sect)
{
  // Get material
  vRef<A3DL::Material> m = getMaterial();
  vRef<csMetaMaterial> mat = meta_cast<csMetaMaterial>(getMaterial());
  LOG("csMetaModel", 2, "getting material " << mat.isValid());
  mat->Setup(vosa3dl);
  LOG("csMetaModel", 2, "setting up model");

  // Create task
  vosa3dl->mainThreadTasks.push(new ConstructModelTask (
                              vosa3dl->GetObjectRegistry(), mat, getModelObj(),
                              this, getURLstr(), sect->GetSector()));

  LOG("csMetaModel", 2, "calling csMetaObject3D::setup");
  csMetaObject3D::Setup(vosa3dl, sect);
}

