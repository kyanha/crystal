/*
    Copyright (C) 2001 by Jorrit Tyberghein

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

#define CS_SYSDEF_PROVIDE_DIR
#include "cssysdef.h"
#include "cssys/csshlib.h"
#include "cssys/sysdriv.h"
#include "cssys/sysfunc.h"
#include "cssys/system.h"
#include "cstool/initapp.h"
#include "csutil/cfgacc.h"
#include "csutil/cfgfile.h"
#include "csutil/cfgmgr.h"
#include "csutil/cmdline.h"
#include "csutil/cseventq.h"
#include "csutil/csinput.h"
#include "csutil/objreg.h"
#include "csutil/physfile.h"
#include "csutil/plugldr.h"
#include "csutil/plugmgr.h"
#include "csutil/prfxcfg.h"
#include "csutil/virtclk.h"
#include "csutil/xmltiny.h"
#include "iengine/engine.h"
#include "iengine/motion.h"
#include "igraphic/imageio.h"
#include "imap/parser.h"
#include "imesh/crossbld.h"
#include "imesh/mdlconv.h"
#include "inetwork/driver.h"
#include "isound/loader.h"
#include "isound/renderer.h"
#include "iutil/cfgmgr.h"
#include "iutil/cmdline.h"
#include "iutil/comp.h"
#include "iutil/evdefs.h"
#include "iutil/eventh.h"
#include "iutil/eventq.h"
#include "iutil/objreg.h"
#include "iutil/plugin.h"
#include "iutil/vfs.h"
#include "iutil/virtclk.h"
#include "ivaria/conin.h"
#include "ivaria/conout.h"
#include "ivaria/iso.h"
#include "ivaria/perfstat.h"
#include "ivaria/reporter.h"
#include "ivaria/stdrep.h"
#include "ivideo/fontserv.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"

#ifdef CS_DEBUG
#define CS_LOAD_LIB_VERBOSE true
#else
#define CS_LOAD_LIB_VERBOSE false
#endif

static SysSystemDriver* global_sys = 0;
static bool config_done = false;
static bool sys_init_done = false;
static iEventHandler* installed_event_handler = 0;

iObjectRegistry* csInitializer::CreateEnvironment (
  int argc, char const* const argv[])
{
  CS_INITIALIZE_PLATFORM_APPLICATION;

  iObjectRegistry* reg = 0;
  if (InitializeSCF())
  {
    iObjectRegistry* r = CreateObjectRegistry();
    if (r != 0)
    {
      if (CreatePluginManager(r) &&
          CreateEventQueue(r) &&
          CreateVirtualClock(r) &&
          CreateCommandLineParser(r, argc, argv) &&
          CreateConfigManager(r) &&
          CreateInputDrivers(r) &&
          csPlatformStartup(r))
        reg = r;
      else
        r->DecRef();
    }
  }
  return reg;
}

// Scan a directory for .csplugin files
static void ScanScfDir (const char* dir)
{
  struct dirent* de;
  DIR* dh = opendir(dir);
  if (dh != 0)
  {
    while ((de = readdir(dh)) != 0)
    {
      if (!isdir(dir, de))
      {
        int const n = strlen(de->d_name);
        if (n >= 9 && strcasecmp(de->d_name + n - 9, ".csplugin") == 0)
        {
	  csString scffilepath;
	  scffilepath << dir << PATH_SEPARATOR << de->d_name;
	  csPhysicalFile file(scffilepath, "rb");
	  csTinyDocumentSystem docsys;
	  csRef<iDocument> doc = docsys.CreateDocument();
	  char const* errmsg = doc->Parse(&file);
	  if (errmsg == 0)
	    scfInitialize(doc);
	  else
	    fprintf(stderr, "csInitializer::InitializeSCF: "
	      "Error parsing %s: %s\n", scffilepath.GetData(), errmsg);
        }
      }
    }
    closedir(dh);
  }
}

bool csInitializer::InitializeSCF ()
{
  scfInitialize();

#ifndef CS_STATIC_LINKED
  // Search plugins in pluginpaths
  char** pluginpaths = csGetPluginPaths ();
  for (int i=0; pluginpaths[i]!=0; i++)
  {
    csString temp = pluginpaths[i];
    temp += PATH_SEPARATOR;
    csAddLibraryPath(temp);
    ScanScfDir (pluginpaths[i]);
    delete[] pluginpaths[i];
  }
  delete[] pluginpaths;
#endif

  return true;
}

iObjectRegistry* csInitializer::CreateObjectRegistry ()
{
  csObjectRegistry* r = new csObjectRegistry ();
  global_sys = new SysSystemDriver (r);
  return r;
}

iPluginManager* csInitializer::CreatePluginManager (iObjectRegistry* r)
{
  csRef<csPluginManager> plugmgr = csPtr<csPluginManager> (
  new csPluginManager (r));
  r->Register (plugmgr, "iPluginManager");
  return plugmgr;
}

iEventQueue* csInitializer::CreateEventQueue (iObjectRegistry* r)
{
  // Register the shared event queue.
  csRef<iEventQueue> q = csPtr<iEventQueue> (new csEventQueue (r));
  r->Register (q, "iEventQueue");
  return q;
}

bool csInitializer::CreateInputDrivers (iObjectRegistry* r)
{
  // Register some generic pseudo-plugins.  (Some day these should probably
  // become real plugins.)
  iKeyboardDriver* k = new csKeyboardDriver (r);
  iMouseDriver*    m = new csMouseDriver    (r);
  iJoystickDriver* j = new csJoystickDriver (r);
  r->Register (k, "iKeyboardDriver");
  r->Register (m, "iMouseDriver");
  r->Register (j, "iJoystickDriver");
  j->DecRef();
  m->DecRef();
  k->DecRef();
  return true;
}

iVirtualClock* csInitializer::CreateVirtualClock (iObjectRegistry* r)
{
  csRef<csVirtualClock> vc = csPtr<csVirtualClock> (new csVirtualClock ());
  r->Register (vc, "iVirtualClock");
  return vc;
}

iCommandLineParser* csInitializer::CreateCommandLineParser(
  iObjectRegistry* r, int argc, char const* const argv[])
{
  csRef<iCommandLineParser> c = csPtr<iCommandLineParser> (
      new csCommandLineParser (argc, argv));
  r->Register (c, "iCommandLineParser");
  return c;
}

iConfigManager* csInitializer::CreateConfigManager (iObjectRegistry* r)
{
  csRef<iConfigFile> cfg = csPtr<iConfigFile> (new csConfigFile ());
  csRef<iConfigManager> Config = csPtr<iConfigManager> (
    new csConfigManager (cfg, true));
  r->Register (Config, "iConfigManager");
  return Config;
}

static void SetupPluginLoadErrVerbosity(iObjectRegistry* r)
{
  csRef<iCommandLineParser> cmdline(CS_QUERY_REGISTRY(r, iCommandLineParser));
  if (cmdline.IsValid())
  {
    bool verbose = CS_LOAD_LIB_VERBOSE;
    char const* const s = cmdline->GetOption("verbose");
    if (s != 0)
      verbose = true;
    csSetLoadLibraryVerbose(verbose);
  }
}

bool csInitializer::SetupConfigManager (
  iObjectRegistry* r, char const* configName, char const* AppID)
{
  SetupPluginLoadErrVerbosity(r);

  if (config_done) return true;

  // @@@ This is ugly.  We need a better, more generalized way of doing this.
  // Hard-coding the name of the VFS plugin (crytalspace.kernel.vfs) is bad.
  // Then later ensuring that we skip over this same plugin when requested
  // by the client is even uglier.  The reason that the VFS plugin is required
  // this early is that both the application configuration file and the
  // configuration file for other plugins may (and almost always do) reside on
  // a VFS volume.

  // We first create an empty application config file, so we can create the
  // config manager at all. Then we load the VFS. After that, all config files
  // can be loaded. At the end, we make the user-and-application-specific
  // config file the dynamic one.

  csRef<iVFS> VFS (CS_QUERY_REGISTRY (r, iVFS));
  if (!VFS)
  {
    csRef<iPluginManager> plugin_mgr (CS_QUERY_REGISTRY (r, iPluginManager));
    VFS = csPtr<iVFS> ((iVFS*)(plugin_mgr->QueryPlugin ("iVFS", iVFS_VERSION)));
  }
  if (!VFS)
  {
    csRef<iPluginManager> plugin_mgr (CS_QUERY_REGISTRY (r, iPluginManager));
    VFS = CS_LOAD_PLUGIN (plugin_mgr, "crystalspace.kernel.vfs", iVFS);
    if (!VFS)
    {
      fprintf (stderr, "Couldn't load vfs plugin!\n");
      return false;
    }
    r->Register (VFS, "iVFS");
  }

  csRef<iConfigManager> Config (CS_QUERY_REGISTRY (r, iConfigManager));
  csRef<iConfigFile> cfg = Config->GetDynamicDomain ();
  Config->SetDomainPriority (cfg, iConfigManager::ConfigPriorityApplication);

  // Initialize application configuration file
  if (configName)
    if (!cfg->Load (configName, VFS))
      return false;

  // Check if the user-specific config domain should be used.
  {
    csConfigAccess cfgacc (r, "/config/system.cfg");
    if (cfgacc->GetBool ("System.UserConfig", true))
    {
      // Open the user-specific, application-neutral config domain.
      cfg = csGetPlatformConfig ("CrystalSpace.Global");
      Config->AddDomain (cfg, iConfigManager::ConfigPriorityUserGlobal);

      // Open the user-and-application-specific config domain.
      const char* appid = cfgacc->GetStr ("System.ApplicationID", AppID);
      cfg = csGetPlatformConfig (appid);
      Config->AddDomain (cfg, iConfigManager::ConfigPriorityUserApp);
      Config->SetDynamicDomain (cfg);
    }
  }

  config_done = true;
  return true;
}

bool csInitializer::RequestPlugins (iObjectRegistry* r, ...)
{
  SetupConfigManager (r, 0);
  SetupPluginLoadErrVerbosity(r);

  csPluginLoader* plugldr = new csPluginLoader (r);

  va_list arg;
  va_start (arg, r);
  char* plugName = va_arg (arg, char*);
  while (plugName != 0)
  {
    char* intName = va_arg (arg, char*);
    int scfId = va_arg (arg, scfInterfaceID);
    int version = va_arg (arg, int);
    // scfId and version are unused for now.
    (void)scfId; (void)version;
    char* colon = strchr (plugName, ':');
    if (colon)
    {
      // We have a special tag name.
      char newPlugName[512];
      strcpy (newPlugName, plugName);
      *strchr (newPlugName, ':') = 0;
      plugldr->RequestPlugin (newPlugName, colon+1);
    }
    else
    {
      plugldr->RequestPlugin (plugName, intName);
    }
    plugName = va_arg (arg, char*);
  }
  va_end (arg);

  bool rc = plugldr->LoadPlugins ();
  delete plugldr;
  return rc;
}

bool csInitializer::SetupEventHandler (
  iObjectRegistry* r, iEventHandler* evhdlr, unsigned int eventmask)
{
  CS_ASSERT(installed_event_handler == 0);
  csRef<iEventQueue> q (CS_QUERY_REGISTRY (r, iEventQueue));
  if (q)
  {
    q->RegisterListener (evhdlr, eventmask);
    installed_event_handler = evhdlr;
    return true;
  }
  return false;
}

class csAppEventHandler : public iEventHandler
{
private:
  csEventHandlerFunc evhdlr;
public:
  SCF_DECLARE_IBASE;
  csAppEventHandler (csEventHandlerFunc h) : evhdlr(h)
  { SCF_CONSTRUCT_IBASE (0); }
  virtual bool HandleEvent (iEvent& e) { return evhdlr (e); }
};

SCF_IMPLEMENT_IBASE (csAppEventHandler)
  SCF_IMPLEMENTS_INTERFACE (iEventHandler)
SCF_IMPLEMENT_IBASE_END

bool csInitializer::SetupEventHandler (
  iObjectRegistry* r, csEventHandlerFunc evhdlr_func, unsigned int eventmask)
{
  csAppEventHandler* evhdlr = new csAppEventHandler (evhdlr_func);
  bool rc = SetupEventHandler (r, evhdlr, eventmask);
  evhdlr->DecRef ();
  return rc;
}

bool csInitializer::OpenApplication (iObjectRegistry* r)
{
  SetupConfigManager (r, 0);

  // @@@ Temporary.
  if (!sys_init_done)
  {
    if (!global_sys->Initialize ()) return false;
    sys_init_done = true;
  }

  // Pass the open event to all interested listeners.
  csEvent Event (csGetTicks (), csevBroadcast, cscmdSystemOpen);
  csRef<iEventQueue> EventQueue (CS_QUERY_REGISTRY (r, iEventQueue));
  CS_ASSERT (EventQueue != 0);
  EventQueue->Dispatch (Event);
  return true;
}

void csInitializer::CloseApplication (iObjectRegistry* r)
{
  // Notify all interested listeners that the system is going down
  csRef<iEventQueue> EventQueue (CS_QUERY_REGISTRY (r, iEventQueue));
  if (EventQueue)
  {
    csEvent Event (csGetTicks (), csevBroadcast, cscmdSystemClose);
    EventQueue->Dispatch (Event);
  }
}

void csInitializer::DestroyApplication (iObjectRegistry* r)
{
  CloseApplication (r);
  csPlatformShutdown (r);

  // This will remove the installed_event_handler (if used) as well as free
  // all other event handlers which are registered through plug-ins or
  // other sources.
  {
    csRef<iEventQueue> q (CS_QUERY_REGISTRY (r, iEventQueue));
    if (q)
      q->RemoveAllListeners ();
  }

  // @@@ A throwback to an earlier time.  Remove this once we eliminate the
  // old csSystemDriver cruft entirely.
  delete global_sys;
  global_sys = 0;

  // Explicitly unload all plugins from the plugin manager because
  // some plugins hold references to the plugin manager so the plugin
  // manager will never get destructed if there are still plugins in memory.
  {
    csRef<iPluginManager> plugin_mgr (CS_QUERY_REGISTRY (r, iPluginManager));
    if (plugin_mgr)
      plugin_mgr->Clear ();
    // Force cleanup here.
  }

  // Explicitly clear the object registry before its destruction since some
  // objects being cleared from it may need to query it for other objects, and
  // such queries can fail (depending upon the compiler) if they are made while
  // the registry itself it being destroyed.  Furthermore, such objects may
  // make SCF queries as they are destroyed, so this must occur before SCF is
  // finalized (see below).
  r->Clear ();
  r->DecRef (); // @@@ Why is this being done? !!!

  // Destroy all static variables created by CS_IMPLEMENT_STATIC_VAR() or one
  // of its cousins.
  CS_DECLARE_STATIC_VARIABLE_CLEANUP
  CS_STATIC_VARIABLE_CLEANUP

  iSCF::SCF->Finish();
}
