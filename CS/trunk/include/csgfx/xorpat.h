/*
    Copyright (C) 2000 by Jorrit Tyberghein
	      (C) 2001 by F.Richter

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

/**\file
 */
 
/**\addtogroup gfx
 * @{ 
 */

#ifndef __CS_XORPAT_H__
#define __CS_XORPAT_H__

/**
 * Create an iImage with a nice XOR pattern with 2^\p recdepth
 * shades of from white to black.
 * \param width Width of the image to create
 * \param height Height of the image to create
 * \param recdepth "recursion depth", clamped to range 1-8.
 */
extern csPtr<iImage> csCreateXORPatternImage(int width, int height,
					     int recdepth);

/** @} */

#endif // __CS_XORPAT_H__

