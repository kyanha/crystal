/*
    Copyright (C) 1998,1999,2000 by Jorrit Tyberghein

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

#ifndef __CS_ISYSTEM_H__
#define __CS_ISYSTEM_H__

#include "csutil/scf.h"

/**
 * Several types of messages.
 * The type of message is indicated when you call iSystem::Printf().
 * Depending on message type, the message will be put into debug log,
 * will be output to the console and so on.
 */
/// Internal error, this is a serious bug in CS
#define MSG_INTERNAL_ERROR	1
/// Error which prevents proper further execution
#define MSG_FATAL_ERROR		2
/// Error which doesn't prevent proper execution
#define MSG_WARNING		3
/// Initialization message
#define MSG_INITIALIZATION	4
/// Message intended for display on the console
#define MSG_CONSOLE		5
/// Message intended for display on stdout
#define MSG_STDOUT		6
/// Show message if debug level is 0, 1, or 2 (no debug)
#define MSG_DEBUG_0		7
/// Show message if debug level is 1 or 2 (debug mode)
#define MSG_DEBUG_1		8
/// Show message if debug level 2 (verbose mode)
#define MSG_DEBUG_2		9
/// Show message if debug level is 0, 1, or 2 (no debug) and flush
#define MSG_DEBUG_0F		10
/// Show message if debug level is 1 or 2 (debug mode) and flush
#define MSG_DEBUG_1F		11
/// Show message if debug level 2 (verbose mode) and flush
#define MSG_DEBUG_2F		12

/**
 * Plugins have an additional characteristic called "functionality ID".
 * This identifier is used by other plugins/engine/system driver to
 * find some plugin that user assigned to perform some function.
 * For example, the "VideoDriver" functionality identifier is used to
 * separate the main 3D graphics driver from other possibly loaded
 * driver that also implements the iGraphics3D interface (it could be
 * the secondary video driver for example).
 *<p>
 * The functionality ID is given in the system config file as left
 * side of assignment in "PlugIns" section. For example, in the
 * following line:
 * <pre>
 * [PlugIns]
 * VideoDriver = crystal.graphics3d.software
 * </pre>
 * "VideoDriver" is functionality identifier, and "crystal.graphics3d.software"
 * is SCF class ID. No two plugins can have same functionality ID. When you
 * load a plugin with System::RequestPlugin() you can define functionality ID
 * after a double colon:
 * <pre>
 * System->RequestPlugin ("crystalspace.kernel.vfs:VFS");
 * </pre>
 * If you load a plugin via the ::LoadPlugIn method you just specify the
 * functionality ID as one of arguments.
 */

/// VFS plugin functionality identifier
#define CS_FUNCID_VFS		"VFS"
/// Functionality ID for the video driver
#define CS_FUNCID_VIDEO		"VideoDriver"
/// Sound renderer
#define CS_FUNCID_SOUND		"SoundRender"
/// Font renderer
#define CS_FUNCID_FONT		"FontRender"
/// Network driver
#define CS_FUNCID_NETDRV	"NetDriver"
/// Network manager
#define CS_FUNCID_NETMAN	"NetManager"
/// Network Protocol layer
#define CS_FUNCID_PROTOCOL      "Protocol"
/// Authentation Plugin
#define CS_FUNCID_AUTH          "Auth"
/// Network Cmd Manager layer 
#define CS_FUNCID_CMDMGR        "CmdManager"
/// Console
#define CS_FUNCID_CONSOLE	"Console"
/// 3D engine
#define CS_FUNCID_ENGINE	"Engine"

/**
 * Query a pointer to some plugin through the System interface.
 * `Object' is a object that implements iSystem interface.
 * `Interface' is a interface name (iGraphics2D, iVFS and so on).
 */
#define QUERY_PLUGIN(Object,Interface)					\
  (Interface *)Object->QueryPlugIn (#Interface, VERSION_##Interface)

/**
 * Find a plugin by his functionality ID. First the plugin with requested
 * functionality identifier is found, and after this it is queried for the
 * respective interface; if it does not implement the requested interface,
 * NULL is returned. NULL is also returned if the plugin with respective
 * functionality is simply not found. If you need the plugin with given
 * functionality identifier no matter which interface it implements, ask
 * for some basic interface, say iBase or iPlugIn.
 */
#define QUERY_PLUGIN_ID(Object,FuncID,Interface)			\
  (Interface *)Object->QueryPlugIn (FuncID, #Interface, VERSION_##Interface)

/**
 * Tell system driver to load a plugin.
 * Since SCF kernel is hidden behind the iSystem driver, this is the only
 * way to load a plugin from another plugin.
 * `Object' is a object that implements iSystem interface.
 * `ClassID' is the class ID (`crystalspace.graphics3d.software').
 * `Interface' is a interface name (iGraphics2D, iVFS and so on).
 */
#define LOAD_PLUGIN(Object,ClassID,FuncID,Interface)			\
  (Interface *)(Object)->LoadPlugIn (ClassID, FuncID, #Interface, VERSION_##Interface)

struct iPlugIn;
struct iVFS;

SCF_VERSION (iSystem, 1, 0, 0);

/**
 * This interface serves as a way for plug-ins to query Crystal Space about
 * miscelaneous settings. It also serves as a way for plug-ins to print
 * through Crystal Space's printing interfaces.
 *<p>
 * Notes on plugin support: the list of plugins is queried from the [PlugIns]
 * section in the config file. The plugins are loaded in the order they come
 * in the .cfg file.
 *<p>
 * The plugin can insert itself into the event processing chain and perform
 * some actions depending on events. It also can supply an private independent
 * API but in this case client program should know this in advance.
 */
struct iSystem : public iBase
{
  /// Returns the configuration.
  virtual void GetSettings (int &oWidth, int &oHeight, int &oDepth, bool &oFullScreen) = 0;
  /// Load a plugin and initialize it
  virtual iBase *LoadPlugIn (const char *iClassID, const char *iFuncID,
    const char *iInterface, int iVersion) = 0;
  /// Get first of the loaded plugins that supports given interface ID
  virtual iBase *QueryPlugIn (const char *iInterface, int iVersion) = 0;
  /// Find a plugin given his functionality ID
  virtual iBase *QueryPlugIn (const char *iFuncID, const char *iInterface, int iVersion) = 0;
  /// Remove a plugin from system driver's plugin list
  virtual bool UnloadPlugIn (iPlugIn *iObject) = 0;
  /// Print a string to the specified device.
  virtual void Printf (int mode, const char *format, ...) = 0;
  /// Get the time in milliseconds.
  virtual time_t GetTime () = 0;
  /// Quit the system.
  virtual void StartShutdown () = 0;
  /// Check if system is shutting down
  virtual bool GetShutdown () = 0;
  /// Get a VFS implementation if available
  virtual iVFS* GetVFS () const = 0;
  /// Get a integer configuration value
  virtual int ConfigGetInt (const char *Section, const char *Key, int Default = 0) = 0;
  /// Get a string configuration value
  virtual const char *ConfigGetStr (const char *Section, const char *Key, const char *Default = NULL) = 0;
  /// Get a string configuration value
  virtual bool ConfigGetYesNo (const char *Section, const char *Key, bool Default = false) = 0;
  /// Get a float configuration value
  virtual float ConfigGetFloat (const char *Section, const char *Key, float Default = 0) = 0;
  /// Set an integer configuration value
  virtual bool ConfigSetInt (const char *Section, const char *Key, int Value) = 0;
  /// Set an string configuration value
  virtual bool ConfigSetStr (const char *Section, const char *Key, const char *Value) = 0;
  /// Set an float configuration value
  virtual bool ConfigSetFloat (const char *Section, const char *Key, float Value) = 0;
  /// Save system configuration file
  virtual bool ConfigSave () = 0;
  /// Put a keyboard event into event queue 
  virtual void QueueKeyEvent (int iKeyCode, bool iDown) = 0;
  /// Put an extended keyboard event into event queue 
  virtual void QueueExtendedKeyEvent (int iKeyCode, int iKeyCodeTranslated, bool iDown) = 0;
  /// Put a mouse event into event queue 
  virtual void QueueMouseEvent (int iButton, bool iDown, int x, int y) = 0;
  /// Put a joystick event into event queue
  virtual void QueueJoystickEvent (int iNumber, int iButton, bool iDown, int x, int y) = 0;
  /// Put a focus event into event queue 
  virtual void QueueFocusEvent (bool Enable) = 0;
  /// Register the plugin to receive specific events
  virtual bool CallOnEvents (iPlugIn *iObject, unsigned int iEventMask) = 0;
  /// Query current state for given key
  virtual bool GetKeyState (int key) = 0;
  /// Query current state for given mouse button (0..9)
  virtual bool GetMouseButton (int button) = 0;
  /// Query current (last known) mouse position
  virtual void GetMousePosition (int &x, int &y) = 0;
  /// Query a specific commandline option (you can query second etc such option)
  virtual const char *GetOptionCL (const char *iName, int iIndex = 0) = 0;
  /// Query a filename specified on the commandline (that is, without leading '-')
  virtual const char *GetNameCL (int iIndex = 0) = 0;
  /// Add a command-line option to the command-line option array
  virtual void AddOptionCL (const char *iName, const char *iValue) = 0;
  /// Add a command-line name to the command-line names array
  virtual void AddNameCL (const char *iName) = 0;
  /// A shortcut for requesting to load a plugin (before Initialize())
  inline void RequestPlugin (const char *iPluginName)
  { AddOptionCL ("plugin", iPluginName); }
  /// Called before forced suspend / after resuming suspend
  virtual void SuspendResume (bool iSuspend) = 0;
  /// Toggle console text output (for consoles that share text/graphics mode)
  virtual void EnablePrintf (bool iEnable) = 0;
};

#endif // __CS_ISYSTEM_H__
