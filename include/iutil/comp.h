/*
    Copyright (C) 2001 by Jorrit Tyberghein

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

#ifndef __CS_IUTIL_COMP_H__
#define __CS_IUTIL_COMP_H__

#include "csutil/scf.h"
struct iObjectRegistry;

/**\file
 * Generic component interface
 */
/**
 * \addtogroup scf
 * @{ */

SCF_VERSION (iComponent, 0, 0, 1);

/**
 * This interface describes a generic component in Crystal Space.
 * <p>
 * Main creators of instances implementing this interface:
 *   <ul>
 *   <li>All plugins implement this interface.
 *   </ul>
 * Main ways to get pointers to this interface:
 *   <ul>
 *   <li>SCF_QUERY_INTERFACE() from a plugin instance.
 *   </ul>
 * Main users of this interface:
 *   <ul>
 *   <li>csPluginManager
 *   </ul>
 */
struct iComponent : public iBase
{
  /// Initialize the component, and return success status.
  virtual bool Initialize (iObjectRegistry*) = 0;
};

/** @} */

#endif // __CS_IUTIL_COMP_H__
