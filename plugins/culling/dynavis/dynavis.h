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

#ifndef __CS_DYNAVIS_H__
#define __CS_DYNAVIS_H__

#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "csutil/scf.h"
#include "iengine/viscull.h"

/**
 * A dynamic visisibility culling system.
 */
class csDynaVis : public iVisibilityCuller
{
private:
  iObjectRegistry *object_reg;

public:
  SCF_DECLARE_IBASE;

  csDynaVis (iBase *iParent);
  virtual ~csDynaVis ();
  virtual bool Initialize (iObjectRegistry *object_reg);

  virtual void Setup (const char* /*name*/) { }
  virtual void RegisterVisObject (iVisibilityObject* /*visobj*/) { }
  virtual void UnregisterVisObject (iVisibilityObject* /*visobj*/) { }
  virtual bool VisTest (iRenderView* /*irview*/) { return false; }
  virtual iPolygon3D* IntersectSegment (const csVector3& start,
    const csVector3& end, csVector3& isect, float* pr = NULL,
    iMeshWrapper** p_mesh = NULL) { return NULL; }
  virtual bool SupportsShadowCasting () { return false; }
  virtual void CastShadows (iFrustumView* /*fview*/) { }

  virtual void RegisterShadowReceiver (iShadowReceiver* /*receiver*/) { }
  virtual void UnregisterShadowReceiver (iShadowReceiver* /*receiver*/) { }

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE (csDynaVis);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize (p); }
  } scfiComponent;
};

#endif // __CS_DYNAVIS_H__

