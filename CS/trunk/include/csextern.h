/*
    Copyright (C) 2004 by Jorrit Tyberghein
    Written by John Harger <jharger AT users.sourceforge.net>

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

#ifndef __CS_CSEXTERN_H__
#define __CS_CSEXTERN_H__

#ifdef CS_CSUTIL_LIB
  #define CS_CSUTIL_EXPORT 	CS_EXPORT_SYM
#else
  #define CS_CSUTIL_EXPORT 	CS_IMPORT_SYM
#endif

#ifdef CS_CSTOOL_LIB
  #define CS_CSTOOL_EXPORT CS_EXPORT_SYM
#else
  #define CS_CSTOOL_EXPORT CS_IMPORT_SYM
#endif

#ifdef CS_CSGFX_LIB
  #define CS_CSGFX_EXPORT CS_EXPORT_SYM
#else
  #define CS_CSGFX_EXPORT CS_IMPORT_SYM
#endif

#ifdef CS_CSGEOM_LIB
  #define CS_CSGEOM_EXPORT CS_EXPORT_SYM
#else
  #define CS_CSGEOM_EXPORT CS_IMPORT_SYM
#endif

#ifdef CS_CSWS_LIB
  #define CS_CSWS_EXPORT CS_EXPORT_SYM
#else
  #define CS_CSWS_EXPORT CS_IMPORT_SYM
#endif

#ifdef CS_CSPLUGINCOMMON_LIB
  #define CS_CSPLUGINCOMMON_EXPORT CS_EXPORT_SYM
#else
  #define CS_CSPLUGINCOMMON_EXPORT CS_IMPORT_SYM
#endif

#ifdef CS_CSPLUGINCOMMON_W32_LIB
  #define CS_CSPLUGINCOMMON_W32_EXPORT CS_EXPORT_SYM
#else
  #define CS_CSPLUGINCOMMON_W32_EXPORT CS_IMPORT_SYM
#endif

#ifdef CS_CSPLUGINCOMMON_OGL_LIB
  #define CS_CSPLUGINCOMMON_OGL_EXPORT CS_EXPORT_SYM
#else
  #define CS_CSPLUGINCOMMON_OGL_EXPORT CS_IMPORT_SYM
#endif

#ifdef CS_CSPLUGINCOMMON_DX_LIB
  #define CS_CSPLUGINCOMMON_DX_EXPORT CS_EXPORT_SYM
#else
  #define CS_CSPLUGINCOMMON_DX_EXPORT CS_IMPORT_SYM
#endif

#endif // __CS_EXTERN_H__
