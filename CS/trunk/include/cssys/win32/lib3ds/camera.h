/* -*- c -*- */
#ifndef INCLUDED_LIB3DS_CAMERA_H
#define INCLUDED_LIB3DS_CAMERA_H
/*
 * The 3D Studio File Format Library
 * Copyright (C) 1996-2001 by J.E. Hoffmann <je-h@gmx.net>
 * All rights reserved.
 *
 * This program is  free  software;  you can redistribute it and/or modify it
 * under the terms of the  GNU Lesser General Public License  as published by 
 * the  Free Software Foundation;  either version 2.1 of the License,  or (at 
 * your option) any later version.
 *
 * This  program  is  distributed in  the  hope that it will  be useful,  but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or  FITNESS FOR A  PARTICULAR PURPOSE.  See the  GNU Lesser General Public  
 * License for more details.
 *
 * You should  have received  a copy of the GNU Lesser General Public License
 * along with  this program;  if not, write to the  Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id$
 */

#ifndef INCLUDED_LIB3DS_TYPES_H
#include <lib3ds/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Camera object
 * \ingroup camera
 */
struct _Lib3dsCamera {
    Lib3dsCamera *next;
    char name[64];
    Lib3dsVector position;
    Lib3dsVector target;
    Lib3dsFloat roll;
    Lib3dsFloat fov;
    Lib3dsBool see_cone;
    Lib3dsFloat near_range;
    Lib3dsFloat far_range;
}; 

extern LIB3DSAPI Lib3dsCamera* lib3ds_camera_new(const char *name);
extern LIB3DSAPI void lib3ds_camera_free(Lib3dsCamera *mesh);
extern LIB3DSAPI void lib3ds_camera_dump(Lib3dsCamera *camera);
extern LIB3DSAPI Lib3dsBool lib3ds_camera_read(Lib3dsCamera *camera, FILE *f);
extern LIB3DSAPI Lib3dsBool lib3ds_camera_write(Lib3dsCamera *camera, FILE *f);

#ifdef __cplusplus
};
#endif
#endif

