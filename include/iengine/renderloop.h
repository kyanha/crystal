/*
    Crystal Space 3D engine
    Copyright (C) 2003 by Jorrit Tyberghein
	      (C) 2003 by Frank Richter

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

#ifndef __CS_IENGINE_RENDERLOOP_H__
#define __CS_IENGINE_RENDERLOOP_H__

/**\file
 * Render loop interfaces.
 */
/**
 * \addtogroup engine3d
 * @{ */

#include "csutil/scf.h"

struct iCamera;
struct iClipper2D;
struct iSector;
class csRenderView;

/**
 * Name of the default render loop created by the engine.
 */
#define CS_DEFAULT_RENDERLOOP_NAME	"*default"

SCF_VERSION (iRenderStep, 0, 0, 1);

struct iRenderStep : public iBase
{
//  virtual void Perform (csRenderView* rview, csRenderMeshList* meshes) = 0;
  virtual void Perform (csRenderView* rview, iSector* sector) = 0;
};
 
SCF_VERSION (iRenderLoop, 0, 0, 2);

struct iRenderLoop : public iBase
{
  virtual void Draw (iCamera* c, iClipper2D* clipper) = 0;
};

SCF_VERSION (iRenderLoopManager, 0, 0, 1);

/**
 * Render loop manager.
 * Use to create new loops and manage loop names.
 * \remark It's not recommended to unregister the loop with the name of
 * #CS_DEFAULT_RENDERLOOP_NAME.
 */
struct iRenderLoopManager : public iBase
{
  /**
   * Create a new render loop.
   * \remark This render loop is "unnamed". To name it, use Register().
   */
  virtual iRenderLoop* Create () = 0;
  
  /**
   * Associate a name with a renderloop.
   * One name is associated with one render loop. If you try to register
   * a loop with a name that is already used, Register() will fail.
   * \param name Name the render loop is registered with.
   * \param loop The render loop.
   * \return Whether the loop could be registered with the name. Fails if
   *  either the name is already used or the loop is already registered.
   */
  virtual bool Register (const char* name, iRenderLoop* loop) = 0;
  /**
   * Get the render loop associated with the name.
   * \param name Name for which the renderloop is to be retrieved.
   * \return The render loop associated with the name, or 0 when no loop
   *  is registered with that name.
   */
  virtual iRenderLoop* Retrieve (const char* name) = 0;
  /**
   * Get the name asociated to the render loop.
   * \param loop Render loop which associated name is to be retrieved.
   * \return Name of the loop. 0 if the loop isn't registered.
   */
  virtual const char* GetName (iRenderLoop* loop) = 0;
  /**
   * Remove an association between a name and a render loop.
   * \param loop Render loop which associated name should be removed.
   * \return Whether the association was successfully removed.
   */
  virtual bool Unregister (iRenderLoop* loop) = 0;
};
 
/**@}*/

#endif
