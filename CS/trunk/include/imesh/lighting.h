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

#include "csutil/scf.h"

struct iCacheManager;

SCF_VERSION (iLightingInfo, 0, 3, 0);

/**
 * This interface is implemented by mesh objects that have some kind
 * of lighting system. It has features to initialize lighting, to read
 * it from a cache, ...
 */
struct iLightingInfo : public iBase
{
  /**
   * Initialize the lighting information to some default (mostly black).
   * If clear is true then the lighting is really cleared to black. Otherwise
   * the lighting system is just warned that lighting information is
   * going to be added. This is useful in case a single light is added.
   * Thus the first call to this function should use a clear of true.
   */
  virtual void InitializeDefault (bool clear) = 0;

  /**
   * Read the lighting information from the cache. Call this instead
   * of InitializeDefault(). Returns false if there was a problem.
   * This function will read the data from the current VFS dir.
   */
  virtual bool ReadFromCache (iCacheManager* cache_mgr) = 0;

  /**
   * Write the lighting information to the cache. Returns false if there
   * was a problem. This function will write the data to the current VFS
   * dir.
   */
  virtual bool WriteToCache (iCacheManager* cache_mgr) = 0;

  /**
   * Finally prepare the lighting for use. This function must be called
   * last.
   */
  virtual void PrepareLighting () = 0;

  /**
   * Sets dynamic ambient light for this object.
   */
  virtual void SetDynamicAmbientLight (const csColor& color) = 0;

  /**
   * Get dynamic ambient light.
   */
  virtual const csColor& GetDynamicAmbientLight () = 0;

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
};

#endif // __CS_IMESH_LIGHTING_H__

