/*
    Common routines for X-Windows drivers
    Copyright (C) 1998 by Jorrit Tyberghein

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

#ifndef __X11COMM_H__
#define __X11COMM_H__

#include "isys/system.h"

/**
 * Query user preferences through iObjectRegistry interface:
 * (o) oHardwareCursor will be set if user desires hardware (system)
 *     mouse cursor (vs software - drawn by CS engine)
 * (o) oUseSHM will be set if user wants to use X11 shared memory
 *     extension (it is not always available).
 * (o) oSimDepth is the screen depth to simulate.
 */
extern void GetX11Settings (iObjectRegistry *object_reg,
	int &oSimDepth, bool &oUseSHM, bool &oHardwareCursor);

#endif // __X11COMM_H__
