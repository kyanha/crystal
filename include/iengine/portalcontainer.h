/*
    Copyright (C) 2003 by Jorrit Tyberghein
              (C) 2004 by Marten Svanfeldt

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

#ifndef __CS_IENGINE_PORTALCONTAINER_H__
#define __CS_IENGINE_PORTALCONTAINER_H__

/**\file
 */

#include "csutil/scf.h"

/**
 * \addtogroup engine3d
 * @{ */

struct iPortal;
struct iRenderView;

SCF_VERSION (iPortalContainer, 0, 0, 1);

/**
 * A container for portals.
 * <p>
 * Main creators of instances implementing this interface:
 *   <ul>
 *   <li>iEngine::CreatePortalContainer()
 *   <li>iEngine::CreatePortal()
 *   </ul>
 * Main ways to get pointers to this interface:
 *   <ul>
 *   <li>SCF_QUERY_INTERFACE() on iMeshObject from a portal container mesh.
 *   </ul>
 * Main users of this interface:
 *   <ul>
 *   <li>iEngine
 *   </ul>
 */
struct iPortalContainer : public iBase
{
  /// Get the number of portals in this contain.
  virtual int GetPortalCount () const = 0;

  /// Get a specific portal.
  virtual iPortal* GetPortal (int idx) const = 0;

  /// Create a new portal.
  virtual iPortal* CreatePortal (csVector3* vertices, int num) = 0;

  /// Remove a portal.
  virtual void RemovePortal (iPortal* portal) = 0;

  /// Render the portal container
  virtual void Draw (iRenderView* rview) = 0;
};

/** @} */

#endif // __CS_IENGINE_PORTALCONTAINER_H__

