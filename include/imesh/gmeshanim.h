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

#ifndef __CS_IMESH_GMESHANIM_H__
#define __CS_IMESH_GMESHANIM_H__

#include "csutil/scf.h"

SCF_VERSION (iGenMeshAnimationControlSetup, 0, 0, 3);

/**
 * This interface describes the API for setting up the animation
 * control as implemented by the 'gmeshanim' plugin. The objects
 * implement iGenMeshAnimationControl also implement this interface.
 */
struct iGenMeshAnimationControlSetup : public iBase
{
  // TODO: Add more manual setup methods!
};

#endif // __CS_IMESH_GMESHANIM_H__

