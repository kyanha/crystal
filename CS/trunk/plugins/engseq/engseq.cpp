/*
    Copyright (C) 2002 by Jorrit Tyberghein

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
#include <string.h>

#include "cssysdef.h"
#include "cssys/sysfunc.h"
#include "csgeom/matrix3.h"
#include "csgeom/box.h"
#include "csgeom/sphere.h"
#include "csgeom/math3d.h"
#include "csutil/scf.h"
#include "csutil/cscolor.h"
#include "iutil/objreg.h"
#include "iutil/event.h"
#include "iutil/eventq.h"
#include "iutil/vfs.h"
#include "iutil/plugin.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "ivaria/sequence.h"
#include "ivaria/reporter.h"
#include "iengine/light.h"
#include "iengine/sector.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "iengine/camera.h"
#include "iengine/rview.h"
#include "iengine/material.h"
#include "imesh/object.h"
#include "imesh/ball.h"
#include "imesh/genmesh.h"
#include "imesh/sprite3d.h"
#include "imesh/clothmesh.h"
#include "imesh/partsys.h"
#include "imesh/stars.h"
#include "imesh/terrfunc.h"
#include "imesh/thing/polygon.h"
#include "engseq.h"

CS_IMPLEMENT_PLUGIN

//---------------------------------------------------------------------------

/**
 * The superclass of all sequence operations.
 */
class OpStandard : public iSequenceOperation
{
protected:
  virtual ~OpStandard () { }

public:
  OpStandard () { SCF_CONSTRUCT_IBASE (NULL); }
  SCF_DECLARE_IBASE;
};

SCF_IMPLEMENT_IBASE (OpStandard)
  SCF_IMPLEMENTS_INTERFACE (iSequenceOperation)
SCF_IMPLEMENT_IBASE_END

/**
 * The superclass of all sequence conditions.
 */
class CondStandard : public iSequenceCondition
{
protected:
  virtual ~CondStandard () { }

public:
  CondStandard () { SCF_CONSTRUCT_IBASE (NULL); }
  SCF_DECLARE_IBASE;
};

SCF_IMPLEMENT_IBASE (CondStandard)
  SCF_IMPLEMENTS_INTERFACE (iSequenceCondition)
SCF_IMPLEMENT_IBASE_END

//---------------------------------------------------------------------------

/**
 * Set fog operation.
 */
class OpSetFog : public OpStandard
{
private:
  csRef<iSector> sector;
  csColor color;
  float density;

public:
  OpSetFog (iSector* sector, const csColor& color, float density)
  {
    OpSetFog::sector = sector;
    OpSetFog::color = color;
    OpSetFog::density = density;
  }

  virtual void Do (csTicks /*dt*/, iBase*)
  {
    if (density < 0.001)
      sector->DisableFog ();
    else
      sector->SetFog (density, color);
  }
};

/**
 * Fade fog operation.
 */
class OpFadeFog : public OpStandard
{
private:
  csRef<iSector> sector;
  csColor start_col, end_col;
  float start_density, end_density;
  csTicks duration;
  iEngineSequenceManager* eseqmgr;

public:
  OpFadeFog (iSector* sector, const csColor& color, float density,
  	csTicks duration, iEngineSequenceManager* eseqmgr)
  {
    SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSequenceTimedOperation);
    OpFadeFog::sector = sector;
    OpFadeFog::end_col = color;
    OpFadeFog::end_density = density;
    OpFadeFog::duration = duration;
    OpFadeFog::eseqmgr = eseqmgr;
  }

  virtual void Do (csTicks dt, iBase* params)
  {
    csFog* fog = sector->GetFog ();
    start_col.red = fog->red;
    start_col.green = fog->green;
    start_col.blue = fog->blue;
    start_density = fog->density;
    eseqmgr->FireTimedOperation (dt, duration, &scfiSequenceTimedOperation,
    	params);
  }

  void DoTimed (float time, iBase*)
  {
    float density = (1-time) * start_density + time * end_density;
    if (density < 0.001)
      sector->DisableFog ();
    else
    {
      csColor color;
      color.red = (1-time) * start_col.red + time * end_col.red;
      color.green = (1-time) * start_col.green + time * end_col.green;
      color.blue = (1-time) * start_col.blue + time * end_col.blue;
      sector->SetFog (density, color);
    }
  }

  SCF_DECLARE_IBASE_EXT (OpStandard);

  struct SequenceTimedOperation : public iSequenceTimedOperation
  {
    SCF_DECLARE_EMBEDDED_IBASE (OpFadeFog);
    virtual void Do (float time, iBase* params)
    {
      scfParent->DoTimed (time, params);
    }
  } scfiSequenceTimedOperation;
  friend struct SequenceTimedOperation;
};

SCF_IMPLEMENT_IBASE_EXT (OpFadeFog)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iSequenceTimedOperation)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (OpFadeFog::SequenceTimedOperation)
  SCF_IMPLEMENTS_INTERFACE (iSequenceTimedOperation)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

//---------------------------------------------------------------------------

#define SETMATERIALTYPE_GENMESH 0
#define SETMATERIALTYPE_BALL 1
#define SETMATERIALTYPE_SPRITE3D 2
#define SETMATERIALTYPE_PARTSYS 3
#define SETMATERIALTYPE_CLOTH 4
#define SETMATERIALTYPE_UNKNOWN 5

#define SETMATERIALTYPE_POLYGON 6

/**
 * Set material operation.
 */
class OpSetMaterial : public OpStandard
{
private:
  csRef<iMeshWrapper> mesh;

  csRef<iGeneralMeshState> genmesh;
  csRef<iBallState> ball;
  csRef<iSprite3DState> sprite3d;
  csRef<iParticleState> partsys;
  csRef<iClothMeshState> cloth;
  int type;
  csRef<iPolygon3D> polygon;

  csRef<iMaterialWrapper> material;

public:
  OpSetMaterial (iMeshWrapper* mesh, iMaterialWrapper* material)
  {
    OpSetMaterial::mesh = mesh;
    OpSetMaterial::material = material;

    type = SETMATERIALTYPE_GENMESH;
    genmesh = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iGeneralMeshState);
    if (genmesh) return; type++;
    ball = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iBallState);
    if (ball) return; type++;
    sprite3d = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iSprite3DState);
    if (sprite3d) return; type++;
    partsys = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iParticleState);
    if (partsys) return; type++;
    cloth = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iClothMeshState);
    if (cloth) return; type++;
  }
  OpSetMaterial (iPolygon3D* polygon, iMaterialWrapper* material)
  {
    OpSetMaterial::polygon = polygon;
    OpSetMaterial::material = material;
    type = SETMATERIALTYPE_POLYGON;
  }

  virtual void Do (csTicks /*dt*/, iBase*)
  {
    switch (type)
    {
      case SETMATERIALTYPE_GENMESH: genmesh->SetMaterialWrapper (material); break;
      case SETMATERIALTYPE_BALL: ball->SetMaterialWrapper (material); break;
      case SETMATERIALTYPE_SPRITE3D: sprite3d->SetMaterialWrapper (material); break;
      case SETMATERIALTYPE_PARTSYS: partsys->SetMaterialWrapper (material); break;
      case SETMATERIALTYPE_CLOTH: cloth->SetMaterialWrapper (material); break;
      case SETMATERIALTYPE_POLYGON: polygon->SetMaterial (material); break;
      case SETMATERIALTYPE_UNKNOWN: break;
    }
  }
};

//---------------------------------------------------------------------------

/**
 * Set light operation.
 */
class OpSetLight : public OpStandard
{
private:
  csRef<iLight> light;
  csColor color;

public:
  OpSetLight (iLight* light, const csColor& color)
  {
    OpSetLight::light = light;
    OpSetLight::color = color;
  }

  virtual void Do (csTicks /*dt*/, iBase*)
  {
    light->SetColor (color);
  }
};

/**
 * Fade light operation.
 */
class OpFadeLight : public OpStandard
{
private:
  csRef<iLight> light;
  csColor start_col, end_col;
  csTicks duration;
  iEngineSequenceManager* eseqmgr;

public:
  OpFadeLight (iLight* light, const csColor& color,
  	csTicks duration, iEngineSequenceManager* eseqmgr)
  {
    SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSequenceTimedOperation);
    OpFadeLight::light = light;
    OpFadeLight::end_col = color;
    OpFadeLight::duration = duration;
    OpFadeLight::eseqmgr = eseqmgr;
  }

  virtual void Do (csTicks dt, iBase* params)
  {
    start_col = light->GetColor ();
    eseqmgr->FireTimedOperation (dt, duration, &scfiSequenceTimedOperation,
    	params);
  }

  void DoTimed (float time, iBase*)
  {
    csColor color;
    color.red = (1-time) * start_col.red + time * end_col.red;
    color.green = (1-time) * start_col.green + time * end_col.green;
    color.blue = (1-time) * start_col.blue + time * end_col.blue;
    light->SetColor (color);
  }

  SCF_DECLARE_IBASE_EXT (OpStandard);

  struct SequenceTimedOperation : public iSequenceTimedOperation
  {
    SCF_DECLARE_EMBEDDED_IBASE (OpFadeLight);
    virtual void Do (float time, iBase* params)
    {
      scfParent->DoTimed (time, params);
    }
  } scfiSequenceTimedOperation;
  friend struct SequenceTimedOperation;
};

SCF_IMPLEMENT_IBASE_EXT (OpFadeLight)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iSequenceTimedOperation)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (OpFadeLight::SequenceTimedOperation)
  SCF_IMPLEMENTS_INTERFACE (iSequenceTimedOperation)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

//---------------------------------------------------------------------------

#define SETCOLORTYPE_GENMESH 0
#define SETCOLORTYPE_BALL 1
#define SETCOLORTYPE_SPRITE3D 2
#define SETCOLORTYPE_PARTSYS 3
#define SETCOLORTYPE_CLOTH 4
#define SETCOLORTYPE_TERRFUNC 5
#define SETCOLORTYPE_STARS 6
#define SETCOLORTYPE_UNKNOWN 7

/**
 * Set mesh color operation.
 */
class OpSetMeshColor : public OpStandard
{
private:
  csRef<iMeshWrapper> mesh;

  csRef<iGeneralMeshState> genmesh;
  csRef<iBallState> ball;
  csRef<iSprite3DState> sprite3d;
  csRef<iParticleState> partsys;
  csRef<iClothMeshState> cloth;
  csRef<iTerrFuncState> terrfunc;
  csRef<iStarsState> stars;
  int type;

  csColor color;

public:
  OpSetMeshColor (iMeshWrapper* mesh, const csColor& color)
  {
    OpSetMeshColor::mesh = mesh;
    OpSetMeshColor::color = color;
    type = SETCOLORTYPE_GENMESH;
    genmesh = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iGeneralMeshState);
    if (genmesh) return; type++;
    ball = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iBallState);
    if (ball) return; type++;
    sprite3d = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iSprite3DState);
    if (sprite3d) return; type++;
    partsys = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iParticleState);
    if (partsys) return; type++;
    cloth = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iClothMeshState);
    if (cloth) return; type++;
    terrfunc = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iTerrFuncState);
    if (terrfunc) return; type++;
    stars = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iStarsState);
    if (stars) return; type++;
  }

  virtual void Do (csTicks /*dt*/, iBase*)
  {
    switch (type)
    {
      case SETCOLORTYPE_GENMESH: genmesh->SetColor (color); break;
      case SETCOLORTYPE_BALL: ball->SetColor (color); break;
      case SETCOLORTYPE_SPRITE3D: sprite3d->SetBaseColor (color); break;
      case SETCOLORTYPE_PARTSYS: partsys->SetColor (color); break;
      case SETCOLORTYPE_CLOTH: cloth->SetColor (color); break;
      case SETCOLORTYPE_TERRFUNC: terrfunc->SetColor (color); break;
      case SETCOLORTYPE_STARS: stars->SetColor (color); break;
      case SETCOLORTYPE_UNKNOWN: break;
    }
  }
};

/**
 * Fade mesh operation.
 */
class OpFadeMeshColor : public OpStandard
{
private:
  csRef<iMeshWrapper> mesh;

  csRef<iGeneralMeshState> genmesh;
  csRef<iBallState> ball;
  csRef<iSprite3DState> sprite3d;
  csRef<iParticleState> partsys;
  csRef<iClothMeshState> cloth;
  csRef<iTerrFuncState> terrfunc;
  csRef<iStarsState> stars;
  int type;

  csColor start_col, end_col;
  csTicks duration;
  iEngineSequenceManager* eseqmgr;

public:
  OpFadeMeshColor (iMeshWrapper* mesh, const csColor& color,
  	csTicks duration, iEngineSequenceManager* eseqmgr)
  {
    SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSequenceTimedOperation);
    OpFadeMeshColor::mesh = mesh;
    OpFadeMeshColor::end_col = color;
    OpFadeMeshColor::duration = duration;
    OpFadeMeshColor::eseqmgr = eseqmgr;
    type = SETCOLORTYPE_GENMESH;
    genmesh = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iGeneralMeshState);
    if (genmesh) return; type++;
    ball = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iBallState);
    if (ball) return; type++;
    sprite3d = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iSprite3DState);
    if (sprite3d) return; type++;
    partsys = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iParticleState);
    if (partsys) return; type++;
    cloth = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iClothMeshState);
    if (cloth) return; type++;
    terrfunc = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iTerrFuncState);
    if (terrfunc) return; type++;
    stars = SCF_QUERY_INTERFACE (mesh->GetMeshObject (), iStarsState);
    if (stars) return; type++;
  }

  virtual void Do (csTicks dt, iBase* params)
  {
    switch (type)
    {
      case SETCOLORTYPE_GENMESH: start_col = genmesh->GetColor (); break;
      case SETCOLORTYPE_BALL: start_col = ball->GetColor (); break;
      case SETCOLORTYPE_SPRITE3D: sprite3d->GetBaseColor (start_col); break;
      case SETCOLORTYPE_PARTSYS: start_col = partsys->GetColor (); break;
      case SETCOLORTYPE_CLOTH: start_col = cloth->GetColor (); break;
      case SETCOLORTYPE_TERRFUNC: start_col = terrfunc->GetColor (); break;
      case SETCOLORTYPE_STARS: start_col = stars->GetColor (); break;
      case SETCOLORTYPE_UNKNOWN: break;
    }
    eseqmgr->FireTimedOperation (dt, duration, &scfiSequenceTimedOperation,
    	params);
  }

  void DoTimed (float time, iBase*)
  {
    csColor color;
    color.red = (1-time) * start_col.red + time * end_col.red;
    color.green = (1-time) * start_col.green + time * end_col.green;
    color.blue = (1-time) * start_col.blue + time * end_col.blue;
    switch (type)
    {
      case SETCOLORTYPE_GENMESH: genmesh->SetColor (color); break;
      case SETCOLORTYPE_BALL: ball->SetColor (color); break;
      case SETCOLORTYPE_SPRITE3D: sprite3d->SetBaseColor (color); break;
      case SETCOLORTYPE_PARTSYS: partsys->SetColor (color); break;
      case SETCOLORTYPE_CLOTH: cloth->SetColor (color); break;
      case SETCOLORTYPE_TERRFUNC: terrfunc->SetColor (color); break;
      case SETCOLORTYPE_STARS: stars->SetColor (color); break;
      case SETCOLORTYPE_UNKNOWN: break;
    }
  }

  SCF_DECLARE_IBASE_EXT (OpStandard);

  struct SequenceTimedOperation : public iSequenceTimedOperation
  {
    SCF_DECLARE_EMBEDDED_IBASE (OpFadeMeshColor);
    virtual void Do (float time, iBase* params)
    {
      scfParent->DoTimed (time, params);
    }
  } scfiSequenceTimedOperation;
  friend struct SequenceTimedOperation;
};

SCF_IMPLEMENT_IBASE_EXT (OpFadeMeshColor)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iSequenceTimedOperation)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (OpFadeMeshColor::SequenceTimedOperation)
  SCF_IMPLEMENTS_INTERFACE (iSequenceTimedOperation)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

//---------------------------------------------------------------------------

/**
 * Rotate operation.
 */
class OpRotate : public OpStandard
{
private:
  csRef<iMeshWrapper> mesh;
  int axis1, axis2, axis3;
  float tot_angle1, tot_angle2, tot_angle3;
  csVector3 offset;
  csReversibleTransform start_transform;
  csTicks duration;
  iEngineSequenceManager* eseqmgr;

public:
  OpRotate (iMeshWrapper* mesh,
  	int axis1, float tot_angle1,
  	int axis2, float tot_angle2,
  	int axis3, float tot_angle3,
	const csVector3& offset,
  	csTicks duration, iEngineSequenceManager* eseqmgr)
  {
    SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSequenceTimedOperation);
    OpRotate::mesh = mesh;
    OpRotate::axis1 = axis1;
    OpRotate::tot_angle1 = tot_angle1;
    OpRotate::axis2 = axis2;
    OpRotate::tot_angle2 = tot_angle2;
    OpRotate::axis3 = axis3;
    OpRotate::tot_angle3 = tot_angle3;
    OpRotate::offset = offset;
    OpRotate::duration = duration;
    OpRotate::eseqmgr = eseqmgr;
  }

  virtual void Do (csTicks dt, iBase* params)
  {
    iMovable* movable = mesh->GetMovable ();
    start_transform = movable->GetTransform ();
    eseqmgr->FireTimedOperation (dt, duration, &scfiSequenceTimedOperation,
    	params);
  }

  void DoTimed (float time, iBase*)
  {
    csReversibleTransform trans = start_transform;
    trans.Translate (-offset);
    csVector3 o (0);
    switch (axis1)
    {
      case -1:
        break;
      case 0:
        trans = trans * csTransform (csXRotMatrix3 (tot_angle1*time), o);
	break;
      case 1:
        trans = trans * csTransform (csYRotMatrix3 (tot_angle1*time), o);
	break;
      case 2:
        trans = trans * csTransform (csZRotMatrix3 (tot_angle1*time), o);
	break;
    }
    switch (axis2)
    {
      case -1:
        break;
      case 0:
        trans = trans * csTransform (csXRotMatrix3 (tot_angle2*time), o);
	break;
      case 1:
        trans = trans * csTransform (csYRotMatrix3 (tot_angle2*time), o);
	break;
      case 2:
        trans = trans * csTransform (csZRotMatrix3 (tot_angle2*time), o);
	break;
    }
    switch (axis3)
    {
      case -1:
        break;
      case 0:
        trans = trans * csTransform (csXRotMatrix3 (tot_angle3*time), o);
	break;
      case 1:
        trans = trans * csTransform (csYRotMatrix3 (tot_angle3*time), o);
	break;
      case 2:
        trans = trans * csTransform (csZRotMatrix3 (tot_angle3*time), o);
	break;
    }
    trans.Translate (offset);
    mesh->GetMovable ()->SetTransform (trans);
    mesh->GetMovable ()->UpdateMove ();
    mesh->DeferUpdateLighting (CS_NLIGHT_STATIC | CS_NLIGHT_DYNAMIC, 10);
  }

  SCF_DECLARE_IBASE_EXT (OpStandard);

  struct SequenceTimedOperation : public iSequenceTimedOperation
  {
    SCF_DECLARE_EMBEDDED_IBASE (OpRotate);
    virtual void Do (float time, iBase* params)
    {
      scfParent->DoTimed (time, params);
    }
  } scfiSequenceTimedOperation;
  friend struct SequenceTimedOperation;
};

SCF_IMPLEMENT_IBASE_EXT (OpRotate)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iSequenceTimedOperation)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (OpRotate::SequenceTimedOperation)
  SCF_IMPLEMENTS_INTERFACE (iSequenceTimedOperation)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

//---------------------------------------------------------------------------

/**
 * Move operation.
 */
class OpMove : public OpStandard
{
private:
  csRef<iMeshWrapper> mesh;
  csVector3 offset;
  csVector3 start_pos;
  csTicks duration;
  iEngineSequenceManager* eseqmgr;

public:
  OpMove (iMeshWrapper* mesh,
	const csVector3& offset,
  	csTicks duration, iEngineSequenceManager* eseqmgr)
  {
    SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSequenceTimedOperation);
    OpMove::mesh = mesh;
    OpMove::offset = offset;
    OpMove::duration = duration;
    OpMove::eseqmgr = eseqmgr;
  }

  virtual void Do (csTicks dt, iBase* params)
  {
    iMovable* movable = mesh->GetMovable ();
    start_pos = movable->GetTransform ().GetOrigin ();
    eseqmgr->FireTimedOperation (dt, duration, &scfiSequenceTimedOperation,
    	params);
  }

  void DoTimed (float time, iBase*)
  {
    csVector3 new_pos = start_pos + time * offset;
    mesh->GetMovable ()->GetTransform ().SetOrigin (new_pos);
    mesh->GetMovable ()->UpdateMove ();
    mesh->DeferUpdateLighting (CS_NLIGHT_STATIC | CS_NLIGHT_DYNAMIC, 10);
  }

  SCF_DECLARE_IBASE_EXT (OpStandard);

  struct SequenceTimedOperation : public iSequenceTimedOperation
  {
    SCF_DECLARE_EMBEDDED_IBASE (OpMove);
    virtual void Do (float time, iBase* params)
    {
      scfParent->DoTimed (time, params);
    }
  } scfiSequenceTimedOperation;
  friend struct SequenceTimedOperation;
};

SCF_IMPLEMENT_IBASE_EXT (OpMove)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iSequenceTimedOperation)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (OpMove::SequenceTimedOperation)
  SCF_IMPLEMENTS_INTERFACE (iSequenceTimedOperation)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

//---------------------------------------------------------------------------

/**
 * Set trigger state.
 */
class OpTriggerState : public OpStandard
{
private:
  csRef<iSequenceTrigger> trigger;
  bool en;

public:
  OpTriggerState (iSequenceTrigger* trigger, bool en)
  {
    OpTriggerState::trigger = trigger;
    OpTriggerState::en = en;
  }

  virtual void Do (csTicks /*dt*/, iBase*)
  {
    trigger->SetEnabled (en);
  }
};

//---------------------------------------------------------------------------

/**
 * Check trigger.
 */
class OpCheckTrigger : public OpStandard
{
private:
  csRef<iSequenceTrigger> trigger;
  csTicks delay;

public:
  OpCheckTrigger (iSequenceTrigger* trigger, csTicks delay)
  {
    OpCheckTrigger::trigger = trigger;
    OpCheckTrigger::delay = delay;
  }

  virtual void Do (csTicks /*dt*/, iBase*)
  {
    trigger->TestConditions (delay);
  }
};

//---------------------------------------------------------------------------

/**
 * Condition to test a trigger.
 */
class CondTestTrigger : public CondStandard
{
private:
  iSequenceTrigger* trigger;

public:
  CondTestTrigger (iSequenceTrigger* trigger)
  {
    CondTestTrigger::trigger = trigger;
  }

  virtual bool Condition (csTicks /*dt*/, iBase*)
  {
    return trigger->CheckState ();
  }
};

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE_EXT(csSequenceWrapper)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iSequenceWrapper)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSequenceWrapper::SequenceWrapper)
  SCF_IMPLEMENTS_INTERFACE(iSequenceWrapper)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csSequenceWrapper::csSequenceWrapper (csEngineSequenceManager* eseqmgr,
	iSequence* sequence)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSequenceWrapper);
  csSequenceWrapper::eseqmgr = eseqmgr;
  csSequenceWrapper::sequence = sequence;
}

csSequenceWrapper::~csSequenceWrapper ()
{
}

void csSequenceWrapper::AddOperationSetMaterial (csTicks time,
	iPolygon3D* polygon, iMaterialWrapper* material)
{
  OpSetMaterial* op = new OpSetMaterial (polygon, material);
  sequence->AddOperation (time, op);
  op->DecRef ();
}

void csSequenceWrapper::AddOperationSetMaterial (csTicks time,
	iMeshWrapper* mesh, iMaterialWrapper* material)
{
  OpSetMaterial* op = new OpSetMaterial (mesh, material);
  sequence->AddOperation (time, op);
  op->DecRef ();
}

void csSequenceWrapper::AddOperationSetLight (csTicks time,
	iLight* light, const csColor& color)
{
  OpSetLight* op = new OpSetLight (light, color);
  sequence->AddOperation (time, op);
  op->DecRef ();
}

void csSequenceWrapper::AddOperationFadeLight (csTicks time,
	iLight* light, const csColor& color, csTicks duration)
{
  OpFadeLight* op = new OpFadeLight (light, color, duration, eseqmgr);
  sequence->AddOperation (time, op);
  op->DecRef ();
}

void csSequenceWrapper::AddOperationSetMeshColor (csTicks time,
	iMeshWrapper* mesh, const csColor& color)
{
  OpSetMeshColor* op = new OpSetMeshColor (mesh, color);
  sequence->AddOperation (time, op);
  op->DecRef ();
}

void csSequenceWrapper::AddOperationFadeMeshColor (csTicks time,
	iMeshWrapper* mesh, const csColor& color, csTicks duration)
{
  OpFadeMeshColor* op = new OpFadeMeshColor (mesh, color, duration, eseqmgr);
  sequence->AddOperation (time, op);
  op->DecRef ();
}

void csSequenceWrapper::AddOperationSetFog (csTicks time,
	iSector* sector, const csColor& color, float density)
{
  OpSetFog* op = new OpSetFog (sector, color, density);
  sequence->AddOperation (time, op);
  op->DecRef ();
}

void csSequenceWrapper::AddOperationFadeFog (csTicks time,
	iSector* sector, const csColor& color, float density,
	csTicks duration)
{
  OpFadeFog* op = new OpFadeFog (sector, color, density, duration, eseqmgr);
  sequence->AddOperation (time, op);
  op->DecRef ();
}

void csSequenceWrapper::AddOperationRotateDuration (csTicks time,
	iMeshWrapper* mesh,
	int axis1, float tot_angle1,
	int axis2, float tot_angle2,
	int axis3, float tot_angle3,
	const csVector3& offset, csTicks duration)
{
  OpRotate* op = new OpRotate (mesh,
  	axis1, tot_angle1, axis2, tot_angle2, axis3, tot_angle3,
	offset, duration, eseqmgr);
  sequence->AddOperation (time, op);
  op->DecRef ();
}

void csSequenceWrapper::AddOperationMoveDuration (csTicks time,
	iMeshWrapper* mesh, const csVector3& offset,
	csTicks duration)
{
  OpMove* op = new OpMove (mesh, offset,
	duration, eseqmgr);
  sequence->AddOperation (time, op);
  op->DecRef ();
}

void csSequenceWrapper::AddOperationTriggerState (csTicks time,
	iSequenceTrigger* trigger, bool en)
{
  OpTriggerState* op = new OpTriggerState (trigger, en);
  sequence->AddOperation (time, op);
  op->DecRef ();
}

void csSequenceWrapper::AddOperationCheckTrigger (csTicks time,
  		  iSequenceTrigger* trigger, csTicks delay)
{
  OpCheckTrigger* op = new OpCheckTrigger (trigger, delay);
  sequence->AddOperation (time, op);
  op->DecRef ();
}

void csSequenceWrapper::AddOperationTestTrigger (csTicks time,
  		  iSequenceTrigger* trigger,
		  iSequence* trueSequence,
		  iSequence* falseSequence)
{
  CondTestTrigger* cond = new CondTestTrigger (trigger);
  sequence->AddCondition (time, cond, trueSequence, falseSequence);
  cond->DecRef ();
}

//---------------------------------------------------------------------------

/**
 * Callback that will activate trigger when sector is visible.
 */
class csTriggerSectorCallback : public iSectorCallback
{
private:
  csSequenceTrigger* trigger;
  bool insideonly;
  bool do_box;
  csBox3 box;
  bool do_sphere;
  csSphere sphere;
  uint32 framenr;

public:
  csTriggerSectorCallback (csSequenceTrigger* trigger,
	bool insideonly, const csBox3* box, const csSphere* sphere)
  {
    SCF_CONSTRUCT_IBASE (NULL);
    csTriggerSectorCallback::trigger = trigger;
    csTriggerSectorCallback::insideonly = insideonly;
    if (box)
    {
      do_box = true;
      csTriggerSectorCallback::box = *box;
    }
    else
    {
      do_box = false;
    }
    if (sphere)
    {
      do_sphere = true;
      csTriggerSectorCallback::sphere = *sphere;
    }
    else
    {
      do_sphere = false;
    }
    framenr = 0;
  }
  virtual ~csTriggerSectorCallback () { }

  SCF_DECLARE_IBASE;

  virtual void Traverse (iSector* /*sector*/, iBase* context)
  {
    csRef<iRenderView> rview (SCF_QUERY_INTERFACE (context, iRenderView));
    if (rview)
    {
      uint32 global_framenr = trigger->GetEngineSequenceManager ()
      	->GetGlobalFrameNr ();
      if (framenr != global_framenr)
      {
	// It is potentially useful to fire. So we try to see if
	// all conditions are met.
	if (insideonly && rview->GetPreviousSector () != NULL)
	  return;
	if (do_sphere)
	{
	  const csVector3& pos = rview->GetCamera ()
		  ->GetTransform ().GetOrigin ();
	  float sqd = csSquaredDist::PointPoint (pos, sphere.GetCenter ());
	  if (sqd > sphere.GetRadius () * sphere.GetRadius ())
	    return;
	}
	if (do_box)
	{
	  const csVector3& pos = rview->GetCamera ()
		  ->GetTransform ().GetOrigin ();
	  if (!box.In (pos))
	    return;
	}

        framenr = global_framenr;
	trigger->Fire ();
      }
    }
  }
};

SCF_IMPLEMENT_IBASE (csTriggerSectorCallback)
  SCF_IMPLEMENTS_INTERFACE (iSectorCallback)
SCF_IMPLEMENT_IBASE_END

//---------------------------------------------------------------------------

/**
 * Cleanup a sector callback.
 */
class csConditionCleanupSectorCB : public csConditionCleanup
{
private:
  csRef<iSector> sector;
  csRef<iSectorCallback> cb;

public:
  csConditionCleanupSectorCB (iSector* sect, iSectorCallback* cb)
  {
    sector = sect;
    csConditionCleanupSectorCB::cb = cb;
  }
  virtual void Cleanup ()
  {
    if (sector && cb)
    {
      sector->RemoveSectorCallback (cb);
    }
  }
};

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE_EXT (csSequenceTrigger)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iSequenceTrigger)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSequenceTrigger::SequenceTrigger)
  SCF_IMPLEMENTS_INTERFACE (iSequenceTrigger)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csSequenceTrigger::csSequenceTrigger (csEngineSequenceManager* eseqmgr)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiSequenceTrigger);
  enabled = true;
  enable_onetest = false;
  onetest_framenr = 0;
  fire_delay = 0;
  csSequenceTrigger::eseqmgr = eseqmgr;
  framenr = 0;
  total_conditions = 0;
  last_trigger_state = false;
  condtest_delay = 0;
}

csSequenceTrigger::~csSequenceTrigger ()
{
  ClearConditions ();
}

void csSequenceTrigger::AddConditionInSector (iSector* sector,
	bool insideonly, const csBox3* box, const csSphere* sphere)
{
  csTriggerSectorCallback* trig = new csTriggerSectorCallback (this,
		  insideonly, box, sphere);
  sector->SetSectorCallback (trig);

  csConditionCleanupSectorCB* cleanup = new csConditionCleanupSectorCB (
  	sector, trig);
  condition_cleanups.Push (cleanup);
 
  cleanup->DecRef ();
  trig->DecRef ();

  total_conditions++;
}

void csSequenceTrigger::AddConditionMeshClick (iMeshWrapper* mesh)
{
  eseqmgr->RegisterMeshTrigger (this);
  click_mesh = mesh;
  total_conditions++;
}

void csSequenceTrigger::AddConditionManual ()
{
}

void csSequenceTrigger::ClearConditions ()
{
  total_conditions = 0;
  fired_conditions = 0;
  framenr = 0;
  condition_cleanups.DeleteAll ();
  click_mesh = NULL;
}

void csSequenceTrigger::Trigger ()
{
}

void csSequenceTrigger::FireSequence (csTicks delay, iSequenceWrapper* seq)
{
  fire_sequence = seq;
  fire_delay = delay;
}

void csSequenceTrigger::EnableOneTest ()
{
  if (enable_onetest && onetest_framenr == 0)
  {
    // Since we last enabled the test nothing has happened.
    // That means that we can consider the test to have
    // failed since nothing fired (i.e. Fire() wasn't called).
    last_trigger_state = false;
    return;
  }
  enable_onetest = true;
  onetest_framenr = 0;	// We don't know the frame yet.
}

bool csSequenceTrigger::CheckState ()
{
  return last_trigger_state;
}

void csSequenceTrigger::Fire ()
{
  if (enabled)
  {
    enable_onetest = false;
    uint32 global_framenr = eseqmgr->GetGlobalFrameNr ();
    if (framenr != global_framenr)
    {
      framenr = global_framenr;
      fired_conditions = 0;
    }

    last_trigger_state = false;
    fired_conditions++;
    if (fired_conditions >= total_conditions)
    {
      last_trigger_state = true;
      // Only fire if trigger is enabled. Otherwise we are only
      // doing the test.
      eseqmgr->GetSequenceManager ()->RunSequence (fire_delay,
    	  fire_sequence->GetSequence ());
      enabled = false;
      fired_conditions = 0;
    }
  }
  else if (enable_onetest)
  {
    uint32 global_framenr = eseqmgr->GetGlobalFrameNr ();
    if (framenr != global_framenr)
    {
      // We start a new frame.
      if (onetest_framenr != 0)
      {
        // In this case we already did our test last frame.
	// Since we come here we know the trigger failed.
	enable_onetest = false;
	last_trigger_state = false;
	return;
      }
      framenr = global_framenr;
      onetest_framenr = global_framenr;
      fired_conditions = 0;
    }
    if (onetest_framenr == 0) return;	// Not busy testing yet.

    fired_conditions++;
    if (fired_conditions >= total_conditions)
    {
      last_trigger_state = true;
      fired_conditions = 0;
      enable_onetest = false;
    }
  }
}

/**
 * Condition to loop for TestConditions().
 */
class CondTestConditions : public CondStandard
{
private:
  csSequenceTrigger* trigger;
  csTicks delay;

public:
  CondTestConditions (csSequenceTrigger* trigger, csTicks delay)
  {
    CondTestConditions::trigger = trigger;
    CondTestConditions::delay = delay;
  }

  virtual bool Condition (csTicks /*dt*/, iBase*)
  {
    // The sequence which loops this condition will end
    // automatically when the delay in the trigger is
    // different from this one.
    if (delay != trigger->GetConditionTestDelay ())
      return false;
    trigger->EnableOneTest ();
    return true;
  }
};

void csSequenceTrigger::TestConditions (csTicks delay)
{
  if (condtest_delay == delay) return;

  // By setting the condtest_delay to a different value we will
  // end the condition test sequence that is already running as soon
  // as it fires again.
  condtest_delay = delay;
  if (delay > 0)
  {
    // Here we already start a new sequence with the new delay.
    csRef<iSequence> seq (csPtr<iSequence> (
	eseqmgr->GetSequenceManager ()->NewSequence ()));
    CondTestConditions* cond = new CondTestConditions (this, delay);
    seq->AddCondition (delay, cond, seq, NULL);
    cond->DecRef ();
    eseqmgr->GetSequenceManager ()->RunSequence (0, seq);
  }
}

//---------------------------------------------------------------------------

SCF_IMPLEMENT_FACTORY (csEngineSequenceManager)

SCF_EXPORT_CLASS_TABLE (engseq)
  SCF_EXPORT_CLASS (csEngineSequenceManager,
  	"crystalspace.utilities.sequence.engine", "Engine Sequence Manager")
SCF_EXPORT_CLASS_TABLE_END

SCF_IMPLEMENT_IBASE (csEngineSequenceManager)
  SCF_IMPLEMENTS_INTERFACE (iEngineSequenceManager)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csEngineSequenceManager::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csEngineSequenceManager::EventHandler)
  SCF_IMPLEMENTS_INTERFACE (iEventHandler)
SCF_IMPLEMENT_IBASE_END

csEngineSequenceManager::csEngineSequenceManager (iBase *iParent)
{
  SCF_CONSTRUCT_IBASE (iParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent);
  scfiEventHandler = NULL;
  object_reg = NULL;
  global_framenr = 1;
}

csEngineSequenceManager::~csEngineSequenceManager ()
{
  if (scfiEventHandler)
  {
    csRef<iEventQueue> q (CS_QUERY_REGISTRY(object_reg, iEventQueue));
    if (q != 0)
      q->RemoveListener (scfiEventHandler);
    scfiEventHandler->DecRef ();
  }
}

bool csEngineSequenceManager::Initialize (iObjectRegistry *r)
{
  object_reg = r;
  if (!scfiEventHandler)
    scfiEventHandler = new EventHandler (this);
  csRef<iEventQueue> q (CS_QUERY_REGISTRY(object_reg, iEventQueue));
  if (q != 0)
    q->RegisterListener (scfiEventHandler, CSMASK_Nothing | CSMASK_MouseDown);

  csRef<iPluginManager> plugin_mgr (CS_QUERY_REGISTRY (object_reg,
  	iPluginManager));
  seqmgr = CS_LOAD_PLUGIN (plugin_mgr, "crystalspace.utilities.sequence",
  	iSequenceManager);
  if (!seqmgr)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.utilities.sequence.engine",
	"Couldn't load sequence manager plugin!");
    return false;
  }
  seqmgr->Resume ();

  return true;
}

bool csEngineSequenceManager::HandleEvent (iEvent &event)
{
  if (event.Type == csevBroadcast
	&& event.Command.Code == cscmdPreProcess)
  {
    global_framenr++;

    csTicks curtime = seqmgr->GetMainTime ();
    int i = timed_operations.Length ()-1;
    while (i >= 0)
    {
      csTimedOperation* op = timed_operations[i];
      if (curtime >= op->end)
      {
        op->op->Do (1.0, op->GetParams ());
        timed_operations.Delete (i);
      }
      else
      {
	float time = float (curtime-op->start) / float (op->end-op->start);
        op->op->Do (time, op->GetParams ());
      }
      i--;
    }

    return true;
  }
  else if (event.Type == csevMouseDown)
  {
    int mouse_x = event.Mouse.x;
    int mouse_y = event.Mouse.y;
    if (camera != NULL && mesh_triggers.Length () > 0)
    {
      csVector3 v;
      // Setup perspective vertex, invert mouse Y axis.
      csVector2 p (mouse_x, camera->GetShiftY() * 2 - mouse_y);

      camera->InvPerspective (p, 1, v);
      csVector3 vw = camera->GetTransform ().This2Other (v);

      iSector* sector = camera->GetSector ();
      csVector3 origin = camera->GetTransform ().GetO2TTranslation ();
      csVector3 isect, end = origin + (vw - origin) * 120;

      iPolygon3D* poly = NULL;
      iMeshWrapper* sel = sector->HitBeam (origin, end, isect, &poly);

      int i;
      for (i = 0 ; i < mesh_triggers.Length () ; i++)
      {
	if (mesh_triggers[i]->GetClickMesh () == sel)
	  mesh_triggers[i]->Fire ();
      }

      //vw = isect;
      //v = camera->GetTransform ().Other2This (vw);
    }
  }
  return false;
}

void csEngineSequenceManager::RegisterMeshTrigger (
	csSequenceTrigger* trigger)
{
  if (mesh_triggers.Find (trigger) == -1)
    mesh_triggers.Push (trigger);
}

void csEngineSequenceManager::UnregisterMeshTrigger (
	csSequenceTrigger* trigger)
{
  mesh_triggers.Delete (trigger);
}

csPtr<iSequenceTrigger> csEngineSequenceManager::CreateTrigger (
	const char* name)
{
  csSequenceTrigger* trig = new csSequenceTrigger (this);
  trig->SetName (name);
  triggers.Push (&(trig->scfiSequenceTrigger));
  return &(trig->scfiSequenceTrigger);
}

void csEngineSequenceManager::RemoveTrigger (iSequenceTrigger* trigger)
{
  triggers.Delete (trigger);
}

void csEngineSequenceManager::RemoveTriggers ()
{
  triggers.DeleteAll ();
}

int csEngineSequenceManager::GetTriggerCount () const
{
  return triggers.Length ();
}

iSequenceTrigger* csEngineSequenceManager::GetTrigger (int idx) const
{
  return triggers[idx];
}

iSequenceTrigger* csEngineSequenceManager::FindTriggerByName (
	const char* name) const
{
  int i;
  for (i = 0 ; i < triggers.Length () ; i++)
  {
    if (!strcmp (name, triggers[i]->QueryObject ()->GetName ()))
      return triggers[i];
  }
  return NULL;
}

csPtr<iSequenceWrapper> csEngineSequenceManager::CreateSequence (
	const char* name)
{
  csRef<iSequence> seq (csPtr<iSequence> (seqmgr->NewSequence ()));
  csSequenceWrapper* seqwrap = new csSequenceWrapper (this, seq);
  seqwrap->SetName (name);
  sequences.Push (&(seqwrap->scfiSequenceWrapper));
  return &(seqwrap->scfiSequenceWrapper);
}

void csEngineSequenceManager::RemoveSequence (iSequenceWrapper* seq)
{
  sequences.Delete (seq);
}

void csEngineSequenceManager::RemoveSequences ()
{
  sequences.DeleteAll ();
}

int csEngineSequenceManager::GetSequenceCount () const
{
  return sequences.Length ();
}

iSequenceWrapper* csEngineSequenceManager::GetSequence (int idx) const
{
  return sequences[idx];
}

iSequenceWrapper* csEngineSequenceManager::FindSequenceByName (
	const char* name) const
{
  int i;
  for (i = 0 ; i < sequences.Length () ; i++)
  {
    if (!strcmp (name, sequences[i]->QueryObject ()->GetName ()))
      return sequences[i];
  }
  return NULL;
}

void csEngineSequenceManager::FireTimedOperation (csTicks delta,
	csTicks duration, iSequenceTimedOperation* op, iBase* params)
{
  csTicks curtime = seqmgr->GetMainTime ();
  if (delta >= duration)
  {
    op->Do (1.0, params);
    return;	// Already done.
  }

  csTimedOperation* top = new csTimedOperation (op, params);
  top->start = curtime-delta;
  top->end = top->start + duration;

  timed_operations.Push (top);
  top->DecRef ();
}


