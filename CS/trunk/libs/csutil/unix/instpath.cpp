/*
    Copyright (C) 2001 by Wouter Wijngaards

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cssysdef.h"
#include "csutil/util.h"
#include "csutil/sysfunc.h"
#include "csutil/syspath.h"

// @@@ Re-enable recursive scanning after we rethink it.  Presently, many
// developers run applications from within the source tree, and recursion
// causes a lot of needless scanning. For now it is disabled.
#define DO_SCAN_RECURSION false

// These defines should be set by the configure script
#ifndef CS_CONFIGDIR
#ifdef COMP_GCC
#warning CS_CONFIGDIR not set
#endif
#define CS_CONFIGDIR "/usr/local/crystal"
#endif
#ifndef CS_PLUGINDIR
#ifdef COMP_GCC
#warning CS_PLUGINDIR not set
#endif
#define CS_PLUGINDIR "/usr/local/crystal/lib"
#endif

csString csGetConfigPath ()
{
  const char* crystalconfig = getenv("CRYSTAL_CONFIG");
  if (crystalconfig)
    return crystalconfig;
  
  const char* crystal = getenv ("CRYSTAL");
  if (crystal)
  {
    csString path, file;

    path = crystal;
    path << "/etc/crystal";
    file = path;
    file << "/vfs.cfg";
    if (!access(file, F_OK))
      return path;

    path = crystal;
    path << "/etc";
    file = path;
    file << "/vfs.cfg";
    if (!access(file, F_OK))
      return path;

    path = crystal;
    file = path;
    file << "/vfs.cfg";
    if (!access(file, F_OK))
      return path;

    fprintf (stderr,
        "Failed to find vfs.cfg in '%s' (defined by CRYSTAL var).\n", crystal);
    return "";
  }

  // No environment variable.
  // Is the current dir a possible install?
  if (!access("vfs.cfg", F_OK))
    return ".";

  // Fallback to default location.
  return CS_CONFIGDIR;
}

csPluginPaths* csGetPluginPaths (const char* argv0)
{
  csPluginPaths* paths = new csPluginPaths;

  csString resPath = csGetResourceDir (argv0);
  if (!resPath.IsEmpty())
    paths->AddOnce (resPath, DO_SCAN_RECURSION, "app");

  csString appPath = csGetAppDir (argv0);
  if (!appPath.IsEmpty())
    paths->AddOnce (appPath, DO_SCAN_RECURSION, "app");

  const char* crystal = getenv("CRYSTAL");
  if (crystal)
  {
    csString libpath1, libpath2;
    libpath1 << crystal << "/lib";
    libpath2 << libpath1 << "/crystal";
    paths->AddOnce(libpath2, DO_SCAN_RECURSION, "plugins");
    paths->AddOnce(libpath1, DO_SCAN_RECURSION, "plugins");
    paths->AddOnce(crystal,  DO_SCAN_RECURSION, "plugins");
  }

  const char* crystal_plugin = getenv("CRYSTAL_PLUGIN");
  if (crystal_plugin)
    paths->AddOnce(crystal_plugin, DO_SCAN_RECURSION, "plugins");

  if (!crystal && !crystal_plugin)
    paths->AddOnce(CS_PLUGINDIR, DO_SCAN_RECURSION, "plugins");
    
  return paths;
}
