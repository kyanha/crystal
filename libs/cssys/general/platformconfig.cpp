/*
    Copyright (C) 2003 by Matthias Braun

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

/** This file implements a platform sepcific user configration which tries to
 * create the config files in $HOME/.crystal which is apporpriate for most
 * posixish systems.
 */
#include <sys/stat.h>

#include "cssysdef.h"
#include "csutil/cfgfile.h"
#include "csutil/csstring.h"
#include "cssys/sysfunc.h"

#ifndef S_IRUSR
#define	S_IRUSR 0000400
#endif
#ifndef S_IWUSR
#define	S_IWUSR 0000200
#endif
#ifndef S_IXUSR
#define	S_IXUSR 0000100
#endif
#ifndef S_IRGRP
#define S_IRGRP	0000040
#endif
#ifndef S_IWGRP
#define S_IWGRP	0000020
#endif
#ifndef S_IXGRP
#define S_IXGRP	0000010
#endif
#ifndef S_IROTH
#define S_IROTH	0000004
#endif
#ifndef S_IWOTH
#define S_IWOTH	0000002
#endif
#ifndef S_IXOTH
#define S_IXOTH	0000001
#endif

csPtr<iConfigFile> csGetPlatformConfig(const char* key)
{
  // Is $HOME set? otherwise fallback to standard mode
  const char* home = getenv("HOME");
  if (home == 0)
    return 0;
  
  // Construct directory and filename of the config file
  csString dir, fname;
  dir << home << PATH_SEPARATOR << ".crystal";
  fname << dir << PATH_SEPARATOR << key << ".cfg";

  // Try to create the directory (we assume that $HOME is already created)
  struct stat stats;
  if (stat(dir, &stats) != 0)
  {
    mode_t const m =
      S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH;
    if (mkdir(dir, m) != 0)
    {
      fprintf(stderr,
  	  "Failed to create `%s' for configuration files (errno %d).\n",
	  dir, errno);
      return 0;
    }
  }

  return new csConfigFile(fname);
}
