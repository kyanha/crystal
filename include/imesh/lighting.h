/*
    Copyright (C) 2001-2002 by Jorrit Tyberghein

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

#ifndef __CS_IMESH_LIGHTING_H__
#define __CS_IMESH_LIGHTING_H__

/**\file
 * Mesh object lighting
 */ 

#include "csutil/scf.h"

/**\addtogroup meshplugins
 * @{ */

struct iLight;

class csColor;

/**
 * This interface is implemented by mesh objects that have some kind
 * of lighting system. It has features to initialize lighting and
 * keep track of lights.
 * 
 * Main creators of instances implementing this interface:
 * - Several mesh objects implement this.
 *   
 * Main ways to get pointers to this interface:
 * - scfQueryInterface() on iMeshObject.
 *   
 * Main users of this interface:
 * - The 3D engine plugin (crystalspace.engine.3d).
 */
struct iLightingInfo : public virtual iBase
{
  SCF_INTERFACE (iLightingInfo, 3, 0, 0);

  /**
   * Initialize the lighting information to some default (mostly black).
   * If clear is true then the lighting is really cleared to black. Otherwise
   * the lighting system is just warned that lighting information is
   * going to be added. This is useful in case a single light is added.
   * Thus the first call to this function should use a clear of true.
   */
  virtual void InitializeDefault (bool clear) = 0;

  /**
   * Finally prepare the lighting for use. This function must be called
   * last.
   */
  virtual void PrepareLighting () = 0;

  /**
   * Indicate that some light has changed. This function will
   * be called by the lighting system whenever a light that
   * affects this mesh is changed in some way.
   */
  virtual void LightChanged (iLight* light) = 0;

  /**
   * Indicate that some light no longer affects this mesh.
   */
  virtual void LightDisconnect (iLight* light) = 0;

  /**
   * Disconnect all lights from this mesh.
   */
  virtual void DisconnectAllLights () = 0;
};

/** @} */

#endif // __CS_IMESH_LIGHTING_H__

