/*
    Copyright (C) 2003 by Martin Geisse <mgeisse@gmx.net>

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
#include "cstool/meshobjtmpl.h"
#include "csutil/ref.h"
#include "iutil/objreg.h"

SCF_IMPLEMENT_IBASE (csMeshObject)
  SCF_IMPLEMENTS_INTERFACE (iMeshObject)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObjectModel)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csMeshObject::eiObjectModel)
  SCF_IMPLEMENTS_INTERFACE (iObjectModel)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csMeshObject::csMeshObject (iEngine *eng)
  : VisCallback (0), LogParent (0), Engine (eng)
{
  SCF_CONSTRUCT_IBASE (0);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObjectModel);
}

csMeshObject::~csMeshObject ()
{
}

void csMeshObject::WantToDie ()
{
  // @@@ Ugly!
  if (Engine)
  {
    csRef<iMeshWrapper> m = SCF_QUERY_INTERFACE (LogParent, iMeshWrapper);
    if (m) Engine->WantToDie (m);
  }
}

bool csMeshObject::DrawTest (iRenderView*, iMovable*)
{
  return true;
}

void csMeshObject::UpdateLighting (iLight**, int, iMovable*)
{
}


void csMeshObject::SetVisibleCallback (iMeshObjectDrawCallback* cb)
{
  VisCallback = cb;
}

iMeshObjectDrawCallback* csMeshObject::GetVisibleCallback () const
{
  return VisCallback;
}

void csMeshObject::NextFrame (csTicks current_time,const csVector3& pos)
{
}

void csMeshObject::HardTransform (const csReversibleTransform&)
{
}

bool csMeshObject::SupportsHardTransform () const
{
  return false;
}

bool csMeshObject::HitBeamOutline (const csVector3& start,
  	const csVector3& end, csVector3& isect, float* pr)
{
  return false;
}

bool csMeshObject::HitBeamObject (const csVector3& start, const csVector3& end,
  	csVector3& isect, float* pr)
{
  return false;
}

void csMeshObject::SetLogicalParent (iBase* p)
{
  LogParent = p;
}

iBase* csMeshObject::GetLogicalParent () const
{
  return LogParent;
}

bool csMeshObject::SetColor (const csColor& color)
{
  return false;
}

bool csMeshObject::GetColor (csColor& color) const
{
  return false;
}

bool csMeshObject::SetMaterialWrapper (iMaterialWrapper* material)
{
  return false;
}

iMaterialWrapper* csMeshObject::GetMaterialWrapper () const
{
  return 0;
}

void csMeshObject::GetObjectBoundingBox (csBox3& bbox, int type)
{
  bbox.SetCenter (csVector3 (0, 0, 0));
  bbox.SetSize (csVector3 (CS_BOUNDINGBOX_MAXVALUE,
    CS_BOUNDINGBOX_MAXVALUE, CS_BOUNDINGBOX_MAXVALUE));
}

void csMeshObject::GetRadius (csVector3& radius, csVector3& center)
{
  csBox3 b;
  GetObjectBoundingBox (b, CS_BBOX_NORMAL);
  radius = (b.Max () - b.Min ()) * 0.5f;
  center = b.GetCenter ();
}

// ------------------

SCF_IMPLEMENT_IBASE (csMeshFactory)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectFactory)
SCF_IMPLEMENT_IBASE_END

csMeshFactory::csMeshFactory (iEngine *eng, iObjectRegistry* reg)
	: Engine (eng), object_reg (reg)
{
  SCF_CONSTRUCT_IBASE (0);
}

csMeshFactory::~csMeshFactory ()
{
}

void csMeshFactory::HardTransform (const csReversibleTransform& t)
{
}

bool csMeshFactory::SupportsHardTransform () const
{
  return false;
}

void csMeshFactory::SetLogicalParent (iBase* p)
{
  LogParent = p;
}

iBase* csMeshFactory::GetLogicalParent () const
{
  return LogParent;
}

// ---------------------

SCF_IMPLEMENT_IBASE (csMeshType)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectType)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csMeshType::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csMeshType::csMeshType (iBase *p)
{
  SCF_CONSTRUCT_IBASE (p);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent);
  Engine = 0;
}

csMeshType::~csMeshType ()
{
}

bool csMeshType::Initialize (iObjectRegistry* reg)
{
  csRef<iEngine> e = CS_QUERY_REGISTRY (reg, iEngine);
  Engine = e;
  object_reg = reg;
  return true;
}
