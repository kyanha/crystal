/*
    Copyright (C) 2003 by Frank Richter

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

#include "cssysdef.h"
#include "csutil/sysfunc.h"
#include "csutil/syspath.h"
#include "csutil/csstring.h"
#include "csutil/util.h"

#include <stdio.h>
#include <unistd.h>
#include <limits.h>

csString csGetAppPath (const char* argv0)
{
  if (argv0 == 0 || *argv0 == '\0')
    return (const char*) 0;

  if (*argv0 == PATH_SEPARATOR)	// Absolute path; tells exact location of app.
  {
    return argv0;  
  } 
  else if (strchr (argv0, PATH_SEPARATOR) != 0) // Relative path containing / ?
  {
    char dir[CS_MAXPATHLEN];		// Yes, getcwd()+relpath gives location
    if (getcwd(dir, sizeof(dir)) == 0)	// of app.
      return (const char*) 0;

    csString path(dir);
    path << PATH_SEPARATOR << argv0;
    return path;
  }
  else 
  {
    // A bare name. Search PATH.
    char* envPATH = csStrNew (getenv ("PATH"));
    char* currentPart;
    char* nextPart = envPATH;

    do
    {
      currentPart = nextPart;
      nextPart = strchr (currentPart, PATH_DELIMITER);
      if (nextPart)
	*nextPart++ = 0;

      csString apppath;
      apppath << currentPart << PATH_SEPARATOR << argv0;
      if (access (apppath, F_OK) == 0)
      {
        delete[] envPATH;
        return apppath;
      }
    }
    while (nextPart != 0);
  }
  
  return (const char*) 0;
}
