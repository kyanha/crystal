/*
  Copyright (C) 2005-2006 by Marten Svanfeldt

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

#ifndef __LIGHTER_H__
#define __LIGHTER_H__

#include "csutil/refcount.h"

#include "raydebug.h"
#include "statistics.h"

namespace lighter
{
  class Scene;
  class Sector;
  class Light_old;
  class Primitive;
  class Raytracer;
  class SwapManager;
  
  class ElementAreasAlloc;

  class Lighter : public csRefCount
  {
  public:
    Lighter (iObjectRegistry *objectRegistry);
    ~Lighter ();

    int Run ();

    // Initialize and load plugins we want
    bool Initialize ();

    // Do the fancy lighting
    bool LightEmUp ();

    // Report an error/warning, always returns false
    bool Report (const char* msg, ...);

    // Public members
    csRef<iDocumentSystem> docSystem;
    csRef<iEngine> engine;
    csRef<iImageIO> imageIO;
    csRef<iLoader> loader;
    csRef<iPluginManager> pluginManager;
    csRef<iReporter> reporter;
    csRef<iVFS> vfs;
    csRef<iCommandLineParser> cmdLine;
    csRef<iConfigManager> configMgr;
    iObjectRegistry *objectRegistry;
    csRef<iStringSet> strings;

    SwapManager* swapManager;
    RayDebugHelper rayDebug;
  protected:
    // Cleanup and prepare for shutdown
    void CleanUp ();

    // Parse the commandline and load any files specified
    bool LoadFiles (Statistics::Progress& progress);

    void LoadConfiguration ();

    void CommandLineHelp () const;

    Scene *scene;

    Statistics::Progress progStartup;
    Statistics::Progress progLoadFiles;
    Statistics::Progress progLightmapLayout;
    Statistics::Progress progSaveFactories;
    Statistics::Progress progInitializeMain;
    Statistics::Progress progInitialize;
    Statistics::Progress progInitializeLightmaps;
    Statistics::Progress progPrepareLighting;
    Statistics::Progress progPrepareLightingUVL;
    Statistics::Progress progPrepareLightingSector;
    Statistics::Progress progSaveMeshesMain;
    Statistics::Progress progSaveMeshes;
    Statistics::Progress progSaveFinish;
    Statistics::Progress progBuildKDTree;
    Statistics::Progress progDirectLighting;
    Statistics::Progress progPostproc;
    Statistics::Progress progPostprocSector;
    Statistics::Progress progPostprocLM;
    Statistics::Progress progSaveResult;
    Statistics::Progress progSaveMeshesPostLight;
    Statistics::Progress progApplyWorldChanges;
    Statistics::Progress progCleanup;
    Statistics::Progress progFinished;
  };

  // Global lighter
  extern Lighter* globalLighter;
}

#endif

