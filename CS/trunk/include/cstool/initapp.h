/*
    Copyright (C) 1998-2001 by Jorrit Tyberghein

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

#ifndef __CS_INITAPP_H__
#define __CS_INITAPP_H__

/**\file
 * Application initialization helper class
 */
 
/**
 * \addtogroup appframe
 * @{ */

#include "cstypes.h"
#include "iutil/evdefs.h"

struct iObjectRegistry;
struct iEvent;
struct iEventHandler;
struct iEventQueue;
struct iPluginManager;
struct iVirtualClock;
struct iCommandLineParser;
struct iConfigManager;

/**\name Plugin request macros
 * Utility macros to select what plugins you want to have loaded.
 * @{ */
/// Request a plugin.
#define CS_REQUEST_PLUGIN(Name,Interface) \
  Name, #Interface, iSCF::SCF->GetInterfaceID(#Interface), Interface##_VERSION
/// Marker for the end of the requested plugins list.
#define CS_REQUEST_END \
  0
/// Request VFS plugin.
#define CS_REQUEST_VFS \
  CS_REQUEST_PLUGIN("crystalspace.kernel.vfs", iVFS)
/// Request default font server.
#define CS_REQUEST_FONTSERVER \
  CS_REQUEST_PLUGIN("crystalspace.font.server.default", iFontServer)
/// Request default image loader.
#define CS_REQUEST_IMAGELOADER \
  CS_REQUEST_PLUGIN("crystalspace.graphic.image.io.multiplex", iImageIO)
/// Request null 3D renderer
#define CS_REQUEST_NULL3D \
  CS_REQUEST_PLUGIN("crystalspace.graphics3d.null",iGraphics3D)
/// Request software 3D renderer.
#define CS_REQUEST_SOFTWARE3D \
  CS_REQUEST_PLUGIN("crystalspace.graphics3d.software",iGraphics3D)
/// Request OpenGL 3D renderer
#define CS_REQUEST_OPENGL3D \
  CS_REQUEST_PLUGIN("crystalspace.graphics3d.opengl", iGraphics3D)
/// Request 3D engine.
#define CS_REQUEST_ENGINE \
  CS_REQUEST_PLUGIN("crystalspace.engine.3d", iEngine)
/// Request map loader.
#define CS_REQUEST_LEVELLOADER \
  CS_REQUEST_PLUGIN("crystalspace.level.loader", iLoader)
/// Request map writer.
#define CS_REQUEST_LEVELSAVER \
  CS_REQUEST_PLUGIN("crystalspace.level.saver", iSaver)
/// Request reporter.
#define CS_REQUEST_REPORTER \
  CS_REQUEST_PLUGIN("crystalspace.utilities.reporter", iReporter)
/// Request default reporter listener.
#define CS_REQUEST_REPORTERLISTENER \
  CS_REQUEST_PLUGIN("crystalspace.utilities.stdrep", iStandardReporterListener)
/// Request simple console output.
#define CS_REQUEST_CONSOLEOUT \
  CS_REQUEST_PLUGIN("crystalspace.console.output.simple", iConsoleOutput)
/** @} */

/**
 * Function to handle events for apps.
 */
typedef bool (*csEventHandlerFunc) (iEvent&);

/**
 * This class contains several static member functions that can help
 * setup an application. It is possible to do all the setup on your own
 * but using the functions below will help considerably.
 */
class csInitializer
{
public:
  /**
   * Create everything needed to get a CS application operational.
   * This function is completely equivalent to calling:
   * <ul>
   * <li>#CS_INITIALIZE_PLATFORM_APPLICATION macro</li>
   * <li>InitializeSCF()</li>
   * <li>CreateObjectRegistry()</li>
   * <li>CreatePluginManager()</li>
   * <li>CreateEventQueue()</li>
   * <li>CreateVirtualClock()</li>
   * <li>CreateCommandLineParser()</li>
   * <li>CreateConfigManager()</li>
   * <li>CreateInputDrivers()</li>
   * <li>csPlatformStartup()</li>
   * </ul>
   * You may want to call all or some of those manually if you don't wish
   * to use this function. It is suggested that 
   * #CS_INITIALIZE_PLATFORM_APPLICATION, InitializeSCF() and 
   * csPlatformStartup() are always invoked; the other calls can be replaced 
   * with manual creation of the respective objects. However, the order above
   * should be retained when doing so.
   * \param argc argc argument from main().
   * \param argv argv argument from main().
   * \return This function will return the pointer to the object registry where
   * all the created objects will be registered.
   */
  static iObjectRegistry* CreateEnvironment(int argc, char const* const argv[]);

  /**
   * This very important function initializes the SCF sub-system.
   * Without this you can do almost nothing in CS.
   */
  static bool InitializeSCF ();

  /**
   * This function should be called second. It will create the object
   * registry and return a pointer to it. If there is a problem it will
   * return 0.
   */
  static iObjectRegistry* CreateObjectRegistry ();

  /**
   * You will almost certainly want to call this function. It will
   * create the plugin manager which is essential for nearly everything.
   * The created plugin manager will be registered with the object registry
   * as the default plugin manager (using 0 tag).
   */
  static iPluginManager* CreatePluginManager (iObjectRegistry*);

  /**
   * This essential function creates the event queue which is the main
   * driving force between the event-driven CS model. In addition this function
   * will register the created event queue with the object registry as
   * the default event queue (using 0 tag).
   */
  static iEventQueue* CreateEventQueue (iObjectRegistry*);

  /**
   * Create the virtual clock. This clock is responsible for keeping
   * track of virtual time in the game system. This function will
   * register the created virtual clock with the object registry as the
   * default virtual clock (using 0 tag).
   */
  static iVirtualClock* CreateVirtualClock (iObjectRegistry*);

  /**
   * Create the commandline parser. This function will register the created
   * commandline parser with the object registry as the default
   * parser (using 0 tag).
   */
  static iCommandLineParser* CreateCommandLineParser (
    iObjectRegistry*, int argc, char const* const argv[]);

  /**
   * Create the config manager. This function will register the created
   * config manager with the object registry as the default config manager
   * (using 0 tag).
   */
  static iConfigManager* CreateConfigManager (iObjectRegistry*);

  /**
   * This function will create the three common input drivers
   * (csKeyboardDriver, csMouseDriver, and csJoystickDriver) and register
   * them with the object registry. Note that this function must be
   * called after creating the config manager (CreateConfigManager()).
   */
  static bool CreateInputDrivers (iObjectRegistry*);

  /**
   * Setup the config manager. If you have no config file then you can still
   * call this routine using a 0 parameter. If you don't call this then
   * either RequestPlugins() or Initialize() will call this routine with
   * 0 parameter. The 'ApplicationID' parameter is used to determine the
   * correct user-specific domain. It is possibly overriden by the application
   * config file option "System.ApplicationID".
   */
  static bool SetupConfigManager (iObjectRegistry*, const char* configName,
    const char *ApplicationID = "Noname");

  /**
   * Request a few widely used standard plugins and also read
   * the standard config file and command line for potential other plugins.
   * This routine must be called before Initialize().
   * <p>
   * The variable arguments should contain three entries for every
   * plugin you want to load: name, scfID, and version. To make this
   * easier it is recommended you use one of the CS_REQUEST_xxx macros
   * above. <b>WARNING</b> Make sure to end the list with #CS_REQUEST_END!
   */
  static bool RequestPlugins (iObjectRegistry*, ...);

  /**
   * Send the cscmdSystemOpen command to all loaded plugins.
   * This should be done after initializing them (Initialize()).
   */
  static bool OpenApplication (iObjectRegistry*);

  /**
   * Send the cscmdSystemClose command to all loaded plugins.
   */
  static void CloseApplication (iObjectRegistry*);

  /**
   * Initialize an event handler for the application. This is the
   * most general routine. This event handler will receive all events
   * that are sent through the event manager. Use this function to know
   * about keyboard, mouse and other events. Note that you also have to
   * use this function to be able to render something as rendering
   * happens as a result of one event (cscmdProcess).
   */
  static bool SetupEventHandler (
    iObjectRegistry*, iEventHandler*, unsigned int eventmask);

  /**
   * Initialize an event handler function. This is an easier version
   * of SetupEventHandler() that takes a function and will register an
   * event handler to call that function for all relevant events if `eventmask'
   * is not specified, or for the requested event types if it is specified.
   */
  static bool SetupEventHandler (
    iObjectRegistry*, csEventHandlerFunc, unsigned int eventmask =
      CSMASK_Nothing          |
      CSMASK_Broadcast        |
      CSMASK_MouseUp          |
      CSMASK_MouseDown        |
      CSMASK_MouseMove        |
      CSMASK_KeyDown          |
      CSMASK_KeyUp            |
      CSMASK_MouseClick       |
      CSMASK_MouseDoubleClick |
      CSMASK_JoystickMove     |
      CSMASK_JoystickDown     |
      CSMASK_JoystickUp);

  /**
   * Destroy the application.<p>
   * Undo all of the initialization done by CreateEnvironment() or any of the
   * other setup functions.
   */
  static void DestroyApplication (iObjectRegistry*);
};

/** @} */

#endif // __CS_INITAPP_H__
