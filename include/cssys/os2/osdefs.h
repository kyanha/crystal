/*
    Copyright (C) 1998 by Jorrit Tyberghein
    Written by Andrew Zabolotny <bit@eltech.ru>

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

#ifndef __OSDEFS_H__
#define __OSDEFS_H__

//--//--//--//--//--//--//--//--//--//--//--/ Unconditional definitions --//--//
#define strcasecmp stricmp
#define strncasecmp strnicmp

// The 2D graphics driver used by software renderer on this platform
#define SOFTWARE_2D_DRIVER "crystalspace.graphics2d.dive"

// The 2D graphics driver used by OpenGL renderer
#define OPENGL_2D_DRIVER "crystalspace.graphics2d.glos2"

#if defined (SYSDEF_PATH) || defined (SYSDEF_DIR)
#  include <sys/types.h>
#  if defined (__EMX__)
#    include <dirent.h>
#  else
#    include <direct.h>
#  endif
#endif

#ifdef SYSDEF_TEMP
#  include <process.h>
#  include <stdlib.h>
#  if defined (COMP_WCC)
   static inline int _gettid ()
   {
     int tid;
     __asm	mov eax,fs:[12]	/* ptib2 */
     __asm	mov eax,[eax]	/* TID */
     __asm	mov tid,eax
     return tid;
   }
#  endif
#  define TEMP_FILE "zip%d.%d", getpid(), _gettid()
#  define TEMP_DIR os2_tempdir()
   // This is the function called by TEMP_DIR macro
   static inline char *os2_tempdir()
   {
     char *tmp;
     if ((tmp = getenv ("TMP")) != NULL)
       return tmp;
     if ((tmp = getenv ("TEMP")) != NULL)
       return tmp;
     return "";
   }
#endif

#ifdef SYSDEF_GETCWD
#  if defined (__EMX__)
#    include <stdlib.h>
#    define getcwd os2_getcwd
     static inline char *os2_getcwd (char *buffer, size_t size)
     {
       char *ret = _getcwd2 (buffer, size);
       char *cur = ret;
       while (*cur)
       {
         if (*cur == '/')
           *cur = '\\';
         cur++;
       } /* endif */
       return ret;
     }
#  elif defined (COMP_WCC)
#    include <direct.h>
#  endif
#  undef SYSDEF_GETCWD
#endif

#ifdef SYSDEF_DIR
#  if defined (_A_SUBDIR) && !defined (A_DIR)
#    define A_DIR _A_SUBDIR
#  endif
   static inline bool isdir (const char *path, dirent *de)
   {
     (void)path;
     return !!(de->d_attr & A_DIR);
   }
#endif

#ifdef SYSDEF_MKDIR
#  if defined (__EMX__)
#    define MKDIR(path) mkdir (path, 0644)
#  else
#    define MKDIR(path) _mkdir (path)
#  endif
#  undef SYSDEF_MKDIR
#endif // SYSDEF_MKDIR

#ifdef SYSDEF_SOCKETS
#  include <sys/ioctl.h>
#  include <sys/so_ioctl.h>
typedef int socklen_t;
#endif

#ifdef SYSDEF_ACCESS
#  include <io.h>
#endif

#endif // __OSDEFS_H__
