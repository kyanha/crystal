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
#include "voslight.h"
#include "iengine/movable.h"
#include "ivaria/ode.h"
#include "csutil/cscolor.h"
#include "ivaria/pmeter.h"

#include <vos/metaobjects/a3dl/a3dl.hh>
#include <vos/metaobjects/misc/search.hh>

#include "vospolygonmesh.h"

using namespace VUtil;
using namespace VOS;

SCF_IMPLEMENT_IBASE (csVosSector)
  SCF_IMPLEMENTS_INTERFACE (iVosSector)
  SCF_IMPLEMENTS_INTERFACE (iVosApi)
SCF_IMPLEMENT_IBASE_END


/// Set ambient lighting task ///
class SetAmbientTask : public Task
{
  csColor color;
public:
  csVosA3DL* vosa3dl;
  csRef<csVosSector> sector;

  SetAmbientTask(csVosA3DL* va, csVosSector* vs, csColor c);
  virtual ~SetAmbientTask() { }
  virtual void doTask();
};

SetAmbientTask::SetAmbientTask(csVosA3DL* va, csVosSector* vs, csColor c)
  : color(c), vosa3dl(va), sector(vs)
{
}

void SetAmbientTask::doTask()
{
  iObjectRegistry *objreg = vosa3dl->GetObjectRegistry();
  csRef<iEngine> engine = CS_QUERY_REGISTRY(objreg, iEngine);
  engine->SetAmbientLight(color);
  // XXX bug must relight static Things. simply calling
  // vosa3dl->incrementRelightCounter() does nothing. Do we
  // need to add a RilghtTask to the main thread task list? 
}

/// Sets CS progress meter ///
class ProgressTask : public VUtil::Task
{
private:
  csRef<iProgressMeter> meter;
  std::string action;
  unsigned int step;
  unsigned int total;

public:
  ProgressTask(iProgressMeter* m, const std::string& a, unsigned int t)
    : meter(m), action(a), total(t)
    {
    }

  ProgressTask(iProgressMeter* m, unsigned int s)
    : meter(m), step(s)
    {
    }

  virtual void doTask()
    {
      if (action != "") {
        meter->SetProgressDescription("crystalspace.network.vos.a3dl.progress", action.c_str());
        meter->SetTotal(total);
        meter->SetGranularity(1);
      }
      else
      {
        meter->Step(step);
      }
    }
};

/// Simple class used to catch VOS progress events  ///
class SimpleProgress : public VOS::ProgressMeterCallback
{
private:
  csRef<iProgressMeter> meter;
  csVosA3DL* vosa3dl;
  unsigned int prevProg;

public:
  SimpleProgress(iProgressMeter* m, csVosA3DL* v)
    : meter(m), vosa3dl(v), prevProg(0)
    {
    }

  virtual ~SimpleProgress() { }

  virtual void notifyTaskStart(const std::string& task, unsigned int total)
    {
      //printf("foo start!\n");
      prevProg = 0;
      vosa3dl->mainThreadTasks.push(new ProgressTask(meter, task, total));
    }

  virtual void notifyProgress(unsigned int progress)
    {
      //printf("foo progress %i!\n", progress);
      vosa3dl->mainThreadTasks.push(new ProgressTask(meter, progress - prevProg));
      prevProg = progress;
    }
};


/// Task for setting up the object from within a non-CS thread///
class SetupObjectTask : public Task
{
public:
  csVosA3DL* vosa3dl;
  vRef<csMetaObject3D> obj3d;
  csRef<csVosSector> sector;

  SetupObjectTask(csVosA3DL* va, csMetaObject3D *o, csVosSector* vs);
  virtual ~SetupObjectTask();
  virtual void doTask();
};

SetupObjectTask::SetupObjectTask (csVosA3DL *va, csMetaObject3D *o, csVosSector *vs)
    : vosa3dl(va), obj3d(o, true), sector(vs)
{
}

SetupObjectTask::~SetupObjectTask ()
{
}

void SetupObjectTask::doTask()
{
  LOG("SetupObjectTask", 3, "Setting up object3D")
  obj3d->Setup(vosa3dl, sector);
}

/** Task for loading or removing an object from the sector.  Will kick off
 *  the SetupObjectTask if the object has not been setup yet **/
class LoadObjectTask : public Task
{
public:
  csVosA3DL* vosa3dl;
  vRef<csMetaObject3D> obj3d;
  csRef<csVosSector> sector;
  csRef<iProgressMeter> meter;
  bool toRemove;

  LoadObjectTask(csVosA3DL* va, csMetaObject3D *o, csVosSector* vs,
                 bool rem, iProgressMeter* meter = 0);
  virtual ~LoadObjectTask();
  virtual void doTask();
};

LoadObjectTask::LoadObjectTask (csVosA3DL *va, csMetaObject3D *o, csVosSector *vs,
                                bool rem, iProgressMeter* m)
  : vosa3dl(va), obj3d(o, true), sector(vs), meter(m), toRemove (rem)
{
}

LoadObjectTask::~LoadObjectTask ()
{
}

void LoadObjectTask::doTask()
{
  csRef<iMeshWrapper> wrapper = obj3d->GetCSinterface()->GetMeshWrapper();
  if (wrapper)
  {
    if (toRemove)
    {
      LOG("LoadObjectTask", 3, "Removing object from sector");
      sector->removeObject3D (obj3d->GetCSinterface());
      sector->GetSector()->GetMeshes()->Remove (wrapper);
      //wrapper->GetMovable()->GetSectors()->Remove (sector->GetSector());
      //wrapper->GetMovable()->UpdateMove();
    }
    else
    {
      sector->addObject3D (obj3d->GetCSinterface());
      if (wrapper->GetMovable()->GetSectors()->Find (sector->GetSector()))
      {
        LOG("LoadObjectTask", 3, "Object already setup and in sector");
      }
      else
      {
        LOG("LoadObjectTask", 3, "Object already setup, setting sector");
        if (wrapper->GetMovable()->GetSectors()->Find(sector->GetSector()) < 0) {
          wrapper->GetMovable()->GetSectors()->Add(sector->GetSector());
          wrapper->GetMovable()->UpdateMove();
        }
      }
    }
  }
  else
  {
    if (toRemove)
    {
      sector->removeObject3D (obj3d->GetCSinterface());
      LOG("LoadObjectTask", 2, "Attempting to remove empty meshwrapper!");
    }
    else
    {
      LOG("LoadObjectTask", 3, "Setting up object3D")
        if (obj3d->GetCSinterface() == 0)
          LOG("NEIL-LOG", 1, "Uh oh");

      sector->addObject3D (obj3d->GetCSinterface());
      TaskQueue::defaultTQ().addTask(new SetupObjectTask(vosa3dl, obj3d, sector));
    }
  }

  if (meter.IsValid())
  {
    vosa3dl->mainThreadTasks.push(new ProgressTask(meter, 1));
  }
}


/// Task for setting up the object from within a non-CS thread///
class LoadSectorTask : public Task
{
public:
  csVosA3DL* vosa3dl;
  csRef<csVosSector> sector;
  csRef<iProgressMeter> meter;

  LoadSectorTask(csVosA3DL* va, csVosSector* vs, iProgressMeter* meter);
  virtual ~LoadSectorTask();
  virtual void doTask();
};

LoadSectorTask::LoadSectorTask (csVosA3DL *va, csVosSector *vs, iProgressMeter* m)
  : vosa3dl(va), sector(vs), meter(m)
{
}

LoadSectorTask::~LoadSectorTask ()
{
}

void LoadSectorTask::doTask()
{
  if (vosa3dl->GetDynamics())
  {
    sector->dynsys = vosa3dl->GetDynamics()->CreateSystem();
    //sector->dynsys->SetGravity(csVector3(0, -9, 0));
    //sector->dynsys->SetRollingDampener(.995);
    //sector->dynsys->SetLinearDampener(.995);
    sector->dynsys->EnableAutoDisable (1);
    sector->dynsys->SetAutoDisableParams(.1f, .1f, 6, 0.0f);

    csRef<iODEDynamicSystemState> osys;
    osys = SCF_QUERY_INTERFACE (sector->dynsys, iODEDynamicSystemState);
    osys->SetContactMaxCorrectingVel (.1);
    osys->SetContactSurfaceLayer (.0001);
    //osys->EnableStepFast (1);
    //osys->EnableQuickStep (1);
  }

  CS_QUERY_REGISTRY_PLUGIN(sector->colsys, sector->objreg, "crystalspace.collisiondetection.opcode",
                           iCollideSystem);

  LOG("csVosSector", 2, "Starting sector load");

  SimpleProgress* sp = 0;
  if (meter != 0) sp = new SimpleProgress(meter, vosa3dl);

  if (meter) sp->notifyTaskStart("Sending world download request...", 1);

  vRef<RemoteSearch> rs = meta_cast<RemoteSearch>(sector->GetVobject()->getSite());
  if(rs.isValid()) {

    {
        vRef<MessageBlock> mb(new MessageBlock(), false);
        vRef<Message> m(new Message(), false);
        m->setType("message");
        m->setMethod("core:start-listening");
        m->insertField(-1, "listen", "parents");
        mb->insertMessage(-1, m);
        mb->setName("parent-listen");
        rs->sendMessage(mb);
    }
    {
        vRef<MessageBlock> mb(new MessageBlock(), false);
        vRef<Message> m(new Message(), false);
        m->setType("message");
        m->setMethod("core:start-listening");
        m->insertField(-1, "listen", "children");
        mb->insertMessage(-1, m);
        mb->setName("children-listen");
        rs->sendMessage(mb);
    }
    {
        vRef<MessageBlock> mb(new MessageBlock(), false);
        vRef<Message> m(new Message(), false);
        m->setType("message");
        m->setMethod("property:start-listening");
        m->insertField(-1, "listen", "property");
        mb->insertMessage(-1, m);
        mb->setName("property-listen");
        rs->sendMessage(mb);
    }

  LOG("csVosSector", 2, "Starting search");


  // TODO: augment sector rule to also listen to properties with cname
  // begininnig with a3dl.  ALso allow application to add rules?
  rs->search(sector->GetVobject(), "sector", 0,
                       "rule sector\n"
                       "do acquire and parent-listen and children-listen to this object\n"
                       "select children with type a3dl:object3D* or with type a3dl:light* or with type a3dl:viewpoint* and apply rule 3Dobject\n"
                       "\n"
                       "rule 3Dobject\n"
                       "do acquire and parent-listen and children-listen to this object\n"
                       "select children with type a3dl:material and apply rule material\n"
                       "select children with type a3dl:portal and apply rule portal\n"
                       "select children with type property:property and apply rule property\n"
                       "select children with type property:property.extrapolated and apply rule extrap-property\n"
                       "\n"
                       "rule material\n"
                       "do acquire and parent-listen and children-listen to this object\n"
                       "select children with type property:property and apply rule property\n"
                       "select children with type a3dl:texture and apply rule texture\n"
                       "\n"
                       "rule texture\n"
                       "do acquire and parent-listen to this object\n"
                       "select children with type property:property and apply rule property\n"
                       "\n"
                       "rule portal\n"
                       "do acquire and parent-listen to this object\n"
                       "select children with type property:property and apply rule property\n"
                       "\n"
                       "rule property\n"
                       "do acquire and parent-listen and property-listen to this object \n"
                       "\n"
                       "rule extrap-property\n"
                       "do acquire and parent-listen and extrap-property-listen to this object\n",
             sp);

    if (sp) delete sp;

    LOG("csVosSector", 2, "Search completed");
  }

  if (meter)
  {
    unsigned int tot = 0;

    for(ChildListIterator cli = sector->GetVobject()->getChildren(); cli.hasMore(); cli++)
    {
      if ((meta_cast<A3DL::Object3D>((*cli)->getChild())).isValid()) tot++;
    }

    vosa3dl->mainThreadTasks.push(new ProgressTask(meter, "Initializing 3D scene", tot));
  }

  sector->GetVobject()->addChildListener (sector);
}


/// csVosSector ///

csVosSector::csVosSector(iObjectRegistry *o, csVosA3DL* va, csMetaSector* sec)
{
  SCF_CONSTRUCT_IBASE(0);
  didLoad = false;
  objreg = o;
  sectorvobj.assign(sec, true);
  sec->SetCsVosSector(this);
  vosa3dl = va;
  engine = CS_QUERY_REGISTRY (objreg, iEngine);
  sector = engine->CreateSector(sec->getURLstr().c_str());
  isLit = false;
  waitingForChildren = 0;
  gravity = 0;
  collisionDetection = false;
}

csVosSector::~csVosSector()
{
  SCF_DESTRUCT_IBASE();
}

const csSet< csPtrKey<iVosObject3D> > &csVosSector::GetObject3Ds()
{
  return loadedObjects;
}

void csVosSector::addObject3D (iVosObject3D *obj)
{
  loadedObjects.Add (obj);
}

void csVosSector::removeObject3D (iVosObject3D *obj)
{
  loadedObjects.Delete (obj);
}

void csVosSector::Load(iProgressMeter* progress)
{
  meter = progress;
  vosa3dl->setProgressMeter(progress);

  if(! didLoad) {
    didLoad = true;
    waitingForChildren = sectorvobj->numChildren();
    TaskQueue::defaultTQ().addTask(new LoadSectorTask(vosa3dl, this, progress));
  }
}

void csVosSector::notifyChildInserted (VobjectEvent &event)
{
  LOG("csVosSector", 3, "notifyChildInserted");
  try
  {
    if (   event.getContextualName() == "a3dl:gravity"
        || event.getContextualName() == "a3dl:collision-detection"
        || event.getContextualName() == "a3dl:ambient-light"
       )
    {
      vRef<Property> p = meta_cast<Property> (event.getChild());
      if(p.isValid())
      {
        p->addParentListener (&DoNothingListener::static_);
        p->addPropertyListener (this);
      }
    }
    else
    {
      vRef<csMetaObject3D> obj3d = meta_cast<csMetaObject3D>(event.getChild());
      LOG("SectorChildInserted", 3, "Looking at " << event.getChild()->getURLstr()
          << " " << obj3d.isValid())

        if(obj3d.isValid())
        {
          //obj3d->Setup(vosa3dl, this);
          vosa3dl->mainThreadTasks.push(new LoadObjectTask( vosa3dl, obj3d, this,
                                                            false, meter));
          if (obj3d->getTypes().hasItem ("a3dl:object3D.polygonmesh")
              && obj3d->getTypes().hasItem ("a3dl:static"))
          {
            vosa3dl->incrementRelightCounter();
          }
        }
        else
        {
          vRef<csMetaLight> light = meta_cast<csMetaLight>(event.getChild());
          if(light.isValid())
          {
            light->Setup(vosa3dl, this);
            vosa3dl->incrementRelightCounter();
          }
        }
    }
  }
  catch(std::runtime_error e)
  {
    LOG("csVosSector", 2, "caught runtime error setting up "
        << event.getChild()->getURLstr() << ": " << e.what());
  }

  LOG("csVosSector", 3, "leaving notifyChildInserted " << waitingForChildren);

  //waitingForChildren--;
  //if(waitingForChildren <= 0)
  //{
  //  vosa3dl->mainThreadTasks.push(new RelightTask(vosa3dl, this));
  //}
}

void csVosSector::notifyChildRemoved (VobjectEvent &event)
{
  LOG("csVosSector", 3, "notifyChildRemoved");

  vRef<csMetaObject3D> obj3d = meta_cast<csMetaObject3D>(event.getChild());
  if(obj3d.isValid())
  {
    vosa3dl->mainThreadTasks.push(new LoadObjectTask( vosa3dl, obj3d,
                                                        this, true));
  }
  else
  {
  /// TODO: Add code to remove a light dynamically! ///
  }
  LOG("csVosSector", 3, "leaving notifyChildRemoved");
}

void csVosSector::notifyChildReplaced (VobjectEvent &event)
{
  if((*event.getNewChild()) == (*event.getOldChild())) return;
  LOG("csVosSector", 3, "notifyChildReplaced");
  notifyChildRemoved(event);
  notifyChildInserted(event);
}

csRef<iSector> csVosSector::GetSector()
{
  return sector;
}

vRef<VOS::Vobject> csVosSector::GetVobject()
{
  return sectorvobj;
}

void csVosSector::CacheLightmaps()
{
  for(ChildListIterator cli = sectorvobj->getChildren(); cli.hasMore(); cli++)
  {
    vRef<csMetaPolygonMesh> mpm = meta_cast<csMetaPolygonMesh>((*cli)->getChild());
    if(mpm.isValid())
    {
      mpm->WriteLightmapCache();
    }
  }
}

class SetGravityAndColDetTask : public Task
{
public:
  double grav;
  bool coldet;
  vRef<csMetaSector> sectorvobj;

  virtual void doTask()
    {
        for(ChildListIterator ci = sectorvobj->getChildren(); ci.hasMore(); ci++) {
          vRef<csMetaObject3D> obj3d = meta_cast<csMetaObject3D>((*ci)->getChild());
          if(obj3d.isValid()) {
            obj3d->setupCollider(coldet, grav);
          }
        }
    };
};

void csVosSector::notifyPropertyChange(const VOS::PropertyEvent &event)
{
  try
  {
      vRef<ParentChildRelation> pcr = event.getProperty()->findParent (sectorvobj);
      LOG("vosobject3d", 5, "found parent");

      bool setActor = false;

      if(pcr->getContextualName() == "a3dl:gravity")
      {
        gravity = sectorvobj->getGravity();
        setActor = true;
      }
      else if(pcr->getContextualName() == "a3dl:collision-detection")
      {
        collisionDetection = sectorvobj->getCollisionDetection();
        setActor = true;
      }
      else if(pcr->getContextualName() == "a3dl:ambient-light")
      {
          float r, g, b;
          sectorvobj->getAmbientLightColor(r, g, b);
          vosa3dl->mainThreadTasks.push(new SetAmbientTask(vosa3dl, this, csColor(r, g, b)));
      }

      if(setActor)
      {
        SetGravityAndColDetTask* t = new SetGravityAndColDetTask();
        t->sectorvobj = sectorvobj;
        t->grav = gravity;
        t->coldet = collisionDetection;

        LOG("vossector", 1, "setting sectorvobj to " << sectorvobj
            << " and grav to " << t->grav
            << " and coldet to " << t->coldet);

        vosa3dl->mainThreadTasks.push (t);
      }
  }
  catch(std::runtime_error e)
  {
    LOG("csVosSector", 2, "caught runtime error on property change "
        << event.getProperty()->getURLstr() << ": " << e.what());
  }

}

float csVosSector::getGravity()
{
  return gravity;
}

bool csVosSector::getCollisionDetection()
{
  return collisionDetection;
}

/// csMetaSector ///

csMetaSector::csMetaSector(VOS::VobjectBase* superobject)
  : A3DL::Sector(superobject)
{
}

VOS::MetaObject* csMetaSector::new_csMetaSector(VOS::VobjectBase* superobject,
                    const std::string& type)
{
  return new csMetaSector(superobject);
}
