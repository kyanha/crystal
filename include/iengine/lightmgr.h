/*
    Copyright (C) 2004 by Jorrit Tyberghein

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

#ifndef __CS_IENGINE_LIGHTMGR_H__
#define __CS_IENGINE_LIGHTMGR_H__

/**\file
 */
/**
 * \addtogroup engine3d_light
 * @{ */
 
#include "csutil/scf.h"
#include "csutil/array.h"

struct iLight;

SCF_VERSION (iLightManager, 0, 0, 1);

/**
 * An engine (3D or iso) can implement this interface for the benefit
 * of mesh objects so that they can request lighting information from
 * the engine. The 'logObject' parameter given to these functions is
 * the logical parent that is set in the mesh objects.
 * The engine must attempt to give this information as efficiently as
 * possible. That means only recalculating this lighting information
 * when needed (i.e. light moves, object moves, ...).
 * <p>
 * The engine registers an implementation of this object in the object
 * registry with the "iLightManager" name.
 */
struct iLightManager : public iBase
{
  /**
   * Return all 'relevant' lights that hit this object.
   * Depending on implementation in the engine this can simply
   * mean a list of all lights that affect the object or
   * it can be a list of the N most relevant lights (with N a
   * parameter set by the user on that object).
   */
  virtual const csArray<iLight*>& GetRelevantLights (iBase* logObject) = 0;
};

/** @} */

#endif // __CS_IENGINE_LIGHTMGR_H__

