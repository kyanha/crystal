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
#include "iengine/movable.h"
#include "csgeom/vector3.h"
#include "csgeom/matrix3.h"
#include "csgeom/quaterni.h"

#include "vosobject3d.h"
#include <vos/metaobjects/a3dl/a3dl.hh>

using namespace VOS;

SCF_IMPLEMENT_IBASE (csVosObject3D)
  SCF_IMPLEMENTS_INTERFACE (iVosObject3D)
  SCF_IMPLEMENTS_INTERFACE (iVosApi)
SCF_IMPLEMENT_IBASE_END

/// csVosObject3D ///

csVosObject3D::csVosObject3D(A3DL::Object3D* obj3d)
  : object3d(obj3d, true)
{
  SCF_CONSTRUCT_IBASE (0);
}

csVosObject3D::~csVosObject3D()
{
  SCF_DESTRUCT_IBASE();
}

csRef<iMeshWrapper> csVosObject3D::GetMeshWrapper()
{
  return meshwrapper;
}

void csVosObject3D::SetMeshWrapper(iMeshWrapper* mw)
{
  meshwrapper = mw;
}

VOS::vRef<VOS::Vobject> csVosObject3D::GetVobject()
{
  return object3d;
}

/// Construct an object3d

class ConstructObject3DTask : public Task
{
public:
  iObjectRegistry *object_reg;
  vRef<csMetaObject3D> obj;
  csVector3 pos;
  csMatrix3 ori;
  csVector3 hardpos;
  csMatrix3 hardtrans;

  ConstructObject3DTask(iObjectRegistry *objreg, csMetaObject3D* obj,
                        const csVector3& pos, const csMatrix3& ori,
                        const csVector3& posht, const csMatrix3& htrans);
  virtual ~ConstructObject3DTask();
  virtual void doTask();
};

ConstructObject3DTask::ConstructObject3DTask(iObjectRegistry *objreg,
                                             csMetaObject3D* ob,
                                             const csVector3& p,
                                             const csMatrix3& o,
                                             const csVector3& hpos,
                                             const csMatrix3& htrans)
  : object_reg(objreg), obj(ob, true),
    pos(p), ori(o),
    hardpos(hpos),
    hardtrans(htrans)
{
}

ConstructObject3DTask::~ConstructObject3DTask()
{
}

void ConstructObject3DTask::doTask()
{
  csRef<iMeshWrapper> mw = obj->GetCSinterface()->GetMeshWrapper();
  LOG("vosobject3d", 2, "setting position of " << obj->getURLstr() << " to " << pos.x << " " << pos.y << " " << pos.z);
  if(mw.IsValid())
  {
    csReversibleTransform ht(hardtrans, hardpos);
    mw->GetFactory()->HardTransform(ht);

    mw->GetMovable()->SetPosition(pos);
    mw->GetMovable()->SetTransform(ori);
    mw->GetMovable()->UpdateMove();
  }
}

// Set position task

class PositionTask : public Task
{
  vRef<csMetaObject3D> obj;
  csVector3 pos;

public:
  PositionTask (csMetaObject3D *o, const csVector3 &p) : obj(o, true), pos(p) {}
  ~PositionTask () {}

  void doTask() { obj->changePosition(pos); }
};

// Set orientation task

class OrientateTask : public Task
{
  vRef<csMetaObject3D> obj;
  csMatrix3 ori;

public:
  OrientateTask (csMetaObject3D *o, const csMatrix3 &m)
    : obj(o, true), ori(m) {}
  ~OrientateTask () {}

  void doTask() { obj->changeTransform (ori); }
};



/// csMetaObject3D ///

csMetaObject3D::csMetaObject3D(VobjectBase* superobject)
    : A3DL::Object3D(superobject)
{
  csvobj3d = new csVosObject3D(this);
  csvobj3d->IncRef();
}

csMetaObject3D::~csMetaObject3D()
{
  delete csvobj3d;
}

MetaObject* csMetaObject3D::new_csMetaObject3D(VobjectBase* superobject,
                                               const std::string& type)
{
  return new csMetaObject3D(superobject);
}

void csMetaObject3D::Setup(csVosA3DL* vosa3dl, csVosSector* sect)
{
  LOG("csMetaObject3D", 3, "now in csMetaObject3D::setup");

  this->vosa3dl = vosa3dl;

  double x, y, z;
  try
  {
    getPosition(x, y, z);
  }
  catch(...)
  {
    x = y = z = 0;
  }

  LOG("csMetaObject3D", 3, "got position");

  double a, b, c, d;
  try
  {
    getOrientation(a, b, c, d);
  }
  catch(...)
  {
    a = b = d = 0;
    c = 1;
  }

  LOG("csMetaObject3D", 3, "got orientation ");

  double xht, yht, zht;
  try
  {
    getPositionHT(xht, yht, zht);
    LOG("csMetaObject3D", 3, "got hard position");
  }
  catch(...)
  {
    xht = yht = zht = 0;
  }

  double aht, bht, cht, dht;
  try
  {
    getOrientationHT(aht, bht, cht, dht);
    LOG("csMetaObject3D", 3, "got hard orientation ");
  }
  catch(...)
  {
    aht = bht = dht = 0;
    cht = 1;
  }

  double sxht, syht, szht;
  try
  {
    getScalingHT(sxht, syht, szht);
    LOG("csMetaObject3D", 3, "got hard scaling");
  }
  catch(...)
  {
    sxht = syht = szht = 1.0;
  }

  csQuaternion q;
  q.SetWithAxisAngle(csVector3(a, b, c), d * M_PI / 180.0);

  csQuaternion qht;
  qht.SetWithAxisAngle(csVector3(aht, bht, cht), dht * M_PI/180.0);

  LOG("vosobject3d", 3, "setting position " << x << " " << y << " " << z);

  vosa3dl->mainThreadTasks.push (new ConstructObject3DTask(
                            vosa3dl->GetObjectRegistry(), this,
                            csVector3(x, y, z),                    // pos
                            csMatrix3(q),                          // ori
                            csVector3(xht, yht, zht),              // hardpos
                            csMatrix3(qht) * csMatrix3(sxht, 0, 0, // hardtrans
                                                       0, syht, 0,
                                                       0, 0, szht)));
  addChildListener (this);
}

void csMetaObject3D::notifyChildInserted (VobjectEvent &event)

{
  LOG ("vosobject3d", 2, "notifyChildInserted " << event.getContextualName());
  if (event.getContextualName() == "a3dl:position" ||
      event.getContextualName() == "a3dl:orientation")
  {
    try
    {
      LOG("vosobject3d", 2, "adding property listener");
      meta_cast<Property> (event.getChild())->addPropertyListener (this);
    }
    catch (...)
    {
    }
  }
}

void csMetaObject3D::notifyChildRemoved (VobjectEvent &event)
{
  LOG ("vosobject3d", 2, "notifyChildRemoved " << event.getContextualName());
  if (event.getContextualName() == "a3dl:position" ||
      event.getContextualName() == "a3dl:orientation")
  {
    try
    {
      meta_cast<Property> (event.getChild())->removePropertyListener (this);
    }
    catch (...)
    {
    }
  }
}

void csMetaObject3D::notifyChildReplaced (VobjectEvent &event)
{
  LOG ("vosobject3d", 2, "notifyChildReplaced " << event.getContextualName());
  if (event.getContextualName() == "a3dl:position" ||
      event.getContextualName() == "a3dl:orientation")
  {
    try
    {
      vRef<Property> prop = meta_cast<Property> (event.getOldChild());
    if (prop.isValid()) prop->removePropertyListener (this);
    }
    catch (...)
    {
    }

    try
    {
    meta_cast<Property> (event.getNewChild())->addPropertyListener (this);
    }
    catch (...)
    {
    }
  }
}

void csMetaObject3D::notifyPropertyChange(const PropertyEvent &event)
{
  try
  {
    if (event.getEvent() != PropertyEvent::PropertyRead)
    {
      vRef<ParentChildRelation> pcr = event.getProperty()->findParent (*this);
      if (pcr->getContextualName() == "a3dl:position")
      {
        double x, y, z;
        getPosition (x,y,z);
        vosa3dl->mainThreadTasks.push (new PositionTask(this,csVector3(x,y,z)));
      }
      else if (pcr->getContextualName() == "a3dl:orientation")
      {
        double x, y, z, angle;
        getOrientation (x,y,z,angle);
        csQuaternion q;
        q.SetWithAxisAngle (csVector3(x,y,z), angle * M_PI/180.0);
        vosa3dl->mainThreadTasks.push (new OrientateTask(this,csMatrix3(q)));
      }
    }
  }
  catch (...)
  {}
}

void csMetaObject3D::changePosition (const csVector3 &pos)
{
  csRef<iMeshWrapper> mw = GetCSinterface()->GetMeshWrapper();
  LOG("vosobject3d", 2, "setting position of " << getURLstr() <<
      " to " << pos.x << " " << pos.y << " " << pos.z);
  if(mw.IsValid())
  {
    mw->GetMovable()->SetPosition(pos);
    mw->GetMovable()->UpdateMove();
  }
}

void csMetaObject3D::changeTransform (const csMatrix3 &trans)
{
  csRef<iMeshWrapper> mw = GetCSinterface()->GetMeshWrapper();
  if(mw.IsValid())
  {
    mw->GetMovable()->SetTransform(trans);
    mw->GetMovable()->UpdateMove();
  }
}

csRef<csVosObject3D> csMetaObject3D::GetCSinterface()
{
  return csvobj3d;
}
