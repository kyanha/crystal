/*
  Copyright (C) 2010 Christian Van Brussel, Communications and Remote
      Sensing Laboratory of the School of Engineering at the 
      Universite catholique de Louvain, Belgium
      http://www.tele.ucl.ac.be

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#ifndef __CSDEMOAPPLICATION_H__
#define __CSDEMOAPPLICATION_H__

/**\file 
 * Crystal Space demo application framework class and tools
 */

/**
 * \addtogroup appframe
 * @{ */

// All these files commonly needed are already included, so that the
// applications only need to include the files relevant for them.
#include "cstool/csapplicationframework.h"
#include "cstool/simplestaticlighter.h"
#include "csutil/cmdhelp.h"
#include "csutil/common_handlers.h"
#include "csutil/csbaseeventh.h"
#include "csutil/event.h"
#include "csutil/hash.h"
#include "csutil/stringarray.h"
#include "iengine/camera.h"
#include "iengine/scenenode.h"
#include "iengine/sector.h"
#include "iengine/movable.h"
#include "imesh/object.h"
#include "iutil/cmdline.h"
#include "iutil/csinput.h"
#include "iutil/virtclk.h"
#include "iutil/visualdebug.h"
#include "ivideo/graph2d.h"
#include "ivaria/view.h"

class csPixmap;

namespace CS {
namespace Demo {

// ------------------------ CommandLineHelper ------------------------

/**
 * Identifier for a command line block (see CS::Demo::CommandLineHelper::AddCommandLineBlock()).
 */
typedef size_t CommandLineBlockID;

/**
 * A generic tool to print the command line help when the '-help' option is used
 * on a demo application.
 * To use this tool, you should define the applicationCommand,
 * applicationCommandUsage and applicationDescription variables (to be defined in
 * the constructor CommandLineHelper()), and add the needed command line
 * options through AddCommandLineOption(). Separate blocks of options can be defined
 * through AddCommandLineBlock().
 * 
 * When the WriteHelp() method is called, this tool will write to the standard
 * output a help text of the following structure:
 * \verbatim
<applicationDescription>

Usage: <applicationCommandUsage>

Available options:

Specific options for <applicationCommand>:
<list of options>

<block name>-specific options:
<list of options>

General options:
<list of CS general options>
\endverbatim
\sa csCommandLineHelper
 */
class CS_CRYSTALSPACE_EXPORT CommandLineHelper
{
 public:
  /**
   * Constructor.
   * \param applicationCommand Name of the executable (eg "myapp").
   * \param applicationCommandUsage Syntax to use the executable (eg "myapp
   * <OPTIONS> filename"). Additional lines of text with examples of use can also
   * be added.
   * \param applicationDescription User friendly description of the application.
   */
  CommandLineHelper (const char* applicationCommand,
		     const char* applicationCommandUsage,
		     const char* applicationDescription);

  /**
   * Add a new block of options
   */
  CommandLineBlockID AddCommandLineBlock (const char* name);

  /**
   * Add a command option to be described in the help text.
   * \param option The name of the option, eg 'enable-debug'.
   * \param description A user friendly description of the option, eg 'Enable
   * output of debug information'.
   * \param option Name of the option
   * \param description Description of the option
   * \param block The block of the option. A value of 0
   * means the default 'application' block.
   */
  void AddCommandLineOption (const char* option, const char* description,
			     CommandLineBlockID block = 0);

  /**
   * Print to standard output all command options and usages of this executable.
   */
  void WriteHelp (iObjectRegistry* registry) const;

 private:
  struct CommandOption
  {
    // Constructor
    CommandOption (const char* option, const char* description)
    : option (option), description (description) {}

    // Name of the option
    csString option;
    // User friendly description of the option
    csString description;
  };

  struct CommandBlock
  {
    // Constructor
    CommandBlock (const char* name)
    : name (name) {}

    // Name of the block
    csString name;
    // Array of options
    csArray<CommandOption> commandOptions;
  };

  // Command line help
  csString applicationCommand;
  csString applicationCommandUsage;
  csString applicationDescription;
  // Array of command line blocks
  csArray<CommandBlock> commandBlocks;
  CommandLineBlockID blockCount;
};

// ------------------------ HUDHelper ------------------------

/**
 * A generic tool managing the display of a HUD, eg for applications
 * implementing DemoApplication.
 * The HUD consists of the Crystal Space logo, the list of available keys
 * that can be used to interact with the demo, and a list of strings
 * describing the current state of the application.
 * \warning SetContext() must be called before any other operation.
 */
class CS_CRYSTALSPACE_EXPORT HUDHelper
{
 public:
  /**
   * Constructor.
   */
  HUDHelper ();
  /**
   * Destructor.
   */
  ~HUDHelper ();

  /**
   * (Re-)set the various objects of the context of this HUDHelper.
   */
  bool SetContext (csApplicationFramework* applicationFramework,
		   iGraphics3D* g3d, iGraphics2D* g2d, iLoader* loader,
		   iVirtualClock* vc);

  /**
   * Update the current state of the HUD depending on the keyboard events.
   * This should be called automatically by DemoApplication.
   */
  bool OnKeyboard (iEvent &event);

  /**
   * Array of string describing the available user keys (eg 'd: toggle debug mode').
   * By default the array is filled with the keys available to move the camera. You
   * can call csStringArray::DeleteAll() if you don't want these keys to be displayed.
   */
  csStringArray keyDescriptions;
  /**
   * Array of string describing the state of the application (eg 'Debug mode enabled').
   */
  csStringArray stateDescriptions;

  /**
   * Display of a 2D text with a shadow. Additional parameters can be defined,
   * they will be formated into the text string by using the cs_snprintf()-style
   * formatting directives.
   */
  void WriteShadow (int x, int y, int color, const char *str,...);
  /**
   * Display of a 2D text. Additional parameters can be defined,
   * they will be formated into the text string by using the cs_snprintf()-style
   * formatting directives.
   */
  void Write (int x, int y, int fg, int color, const char *str,...);

  /**
   * Display the HUD. This has to be called on each frame, after 3D drawings have
   * been done. This should be done automatically within DemoApplication::Frame().
   */
  void DisplayHUD ();

 private:
  // Reference to the application framework
  csApplicationFramework* applicationFramework;
  // Reference to the 3D graphics
  csRef<iGraphics3D> g3d;
  // Reference to the 2D graphics
  csRef<iGraphics2D> g2d;
  // Reference to the virtual clock
  csRef<iVirtualClock> vc;

  // Reference to the font used to display information
  csRef<iFont> font;
  // Crystal Space logo
  csPixmap* cslogo;

  // Computing of frames per second
  uint frameCount;
  int frameTime;
  float currentFPS;

  // Current page of available keys to be displayed
  uint currentKeyPage;
  uint maxKeys;
};

// ------------------------ CameraHelper ------------------------

/**
 * Various camera modes which can be used with CameraHelper.
 */
enum CameraMode
{
  CSDEMO_CAMERA_NONE = 0,         /*!< The application will manage the camera by itself */
  CSDEMO_CAMERA_MOVE_FREE,        /*!< The camera is free to move */
  CSDEMO_CAMERA_MOVE_LOOKAT,      /*!< The camera is free to move but keeps looking at the target */
  CSDEMO_CAMERA_ROTATE            /*!< The camera rotates around the target */
};

/**
 * An abstract class to be implemented in order to control the movement of
 * the camera managed by a CS::Demo::CameraHelper.
 */
class CS_CRYSTALSPACE_EXPORT CameraManager
{
 public:
  /**
   * Return the start position of the camera. This is used at start or
   * when CameraHelper::ResetCamera() is called.
   */
  virtual csVector3 GetCameraStart ()
  { return csVector3 (0.0f, 0.0f, -3.0f); }

  /**
   * Return the closest distance there can be between the camera and its
   * target. This is relevant only for the CSDEMO_CAMERA_MOVE_LOOKAT and
   * CSDEMO_CAMERA_ROTATE camera modes.
   */
  virtual float GetCameraMinimumDistance ()
  { return 0.1f; }

  /**
   * Return the camera target, ie what it is looking at. This is relevant
   * only for the CSDEMO_CAMERA_MOVE_LOOKAT and CSDEMO_CAMERA_ROTATE camera
   * modes.
   */
  virtual csVector3 GetCameraTarget ()
  { return csVector3 (0.0f); }

  virtual ~CameraManager () {}
};

/**
 * A generic tool managing the movements of the camera with the keyboard
 * and/or the mouse.
 * \warning SetContext() must be called before any other operation.
 */
class CS_CRYSTALSPACE_EXPORT CameraHelper
{
 public:
  /**
   * Constructor.
   */
  CameraHelper ();

  /**
   * (Re-)set the various objects of the context of this CameraHelper.
   */
  bool SetContext (CameraManager* manager, iKeyboardDriver* kbd, iVirtualClock* vc, iCamera* camera);

  /// Update the position of the camera. The behavior depends on the current camera mode.
  void Frame ();
  /// Update the position of the camera. The behavior depends on what was used for SetMouseMoveEnabled().
  bool OnMouseDown (iEvent &event);
  /// Update the position of the camera. The behavior depends on what was used for SetMouseMoveEnabled().
  bool OnMouseUp (iEvent &event);
  /// Update the position of the camera. The behavior depends on what was used for SetMouseMoveEnabled().
  bool OnMouseMove (iEvent &event);

  /// Set the camera mode to be used. Default value is CSDEMO_CAMERA_MOVE_NORMAL.
  void SetCameraMode (CameraMode cameraMode);
  /// Return the current camera mode.
  CameraMode GetCameraMode ();

  /**
   * Set whether the camera can be moved or not through the mouse.
   * Default value is true.
   * If enabled, then the camera will be moved when the user drags
   * the mouse while holding one of the following button:
   * - left button: the camera is moved sideways
   * - right button: the camera is rotated around the target returned
   * by CameraManager::GetCameraTarget().
   * - middle button: the camera is moved forward and backward. The camera
   * cannot get closer than CameraManager::GetCameraMinimumDistance().
   */
  void SetMouseMoveEnabled (bool enabled);
  /**
   * Return whether the camera can be moved or not through the mouse.
   */
  bool GetMouseMoveEnabled ();

  /**
   * Reset the camera position to the position returned by
   * CameraManager::GetCameraStart().
   */
  void ResetCamera ();

  /**
   * Set the speed of the camera's motion, in unit per second. The default value is 5.
   * Note that the camera moves ten times faster when the CTRL key is pressed. 
   */
  void SetMotionSpeed (float speed);
  /**
   * Get the speed of the camera's motion, in unit per second.
   */
  float GetMotionSpeed ();

  /**
   * Set the rotation speed of the camera, in radian per second. The default value is 2.
   * Note that the camera rotates five times faster when the CTRL key is pressed. 
   */
  void SetRotationSpeed (float speed);
  /**
   * Get the speed of the camera's motion, in unit per second.
   */
  float GetRotationSpeed ();

 private:
  void UpdateCamera ();
  void UpdateCameraOrigin (const csVector3& desiredOrigin);

  // Reference to the camera manager
  CameraManager* cameraManager;
  // Reference to the keyboard driver
  csRef<iKeyboardDriver> kbd;
  // Reference to the virtual clock
  csRef<iVirtualClock> vc;
  // Reference to the camera
  csRef<iCamera> camera;

  CameraMode cameraMode;
  bool mouseMoveEnabled;
  csVector3 panCameraTarget;
  float cameraDist;
  float cameraYaw;
  float cameraPitch;

  bool cameraModePan;
  bool cameraModeRotate;
  bool cameraModeZoom;

  float motionSpeed;
  float rotationSpeed;

  int lastMouseX, lastMouseY;
};

// ------------------------ DemoApplication ------------------------

/**
 * Crystal Space demo application framework class. This class and its companions
 * provide the basic functionalities for Crystal Space's demo and test
 * applications. Most demos, tests and tutorials would want to use this class
 * in order to simplify their code, make it more pertinent for the user, and
 * unify the graphical and general layout between all applications.
 *
 * The basic functionalities provided by this class are:
 * - creation of the main objects of the engine
 * - default creation of the scene
 * - management of the camera (class CS::Demo::CameraHelper).
 * - display of the available keys, Crystal Space logo and other informations
 * (class CS::Demo::HUDHelper).
 * - management of the command line's help (class CS::Demo::CommandLineHelper).
 *
 * Here is an example for the most simple use of this class:
 * \code
 * //--------------------------
 * // example.h
 *
 * #include "cstool/csdemoapplication.h"
 *
 * class MyApp : public CS::Demo::DemoApplication
 * {
 * public:
 *   MyApp();
 *   bool Application ();
 * };
 *
 * //--------------------------
 * // example.cpp
 *
 * #include "cssysdef.h"
 * #include "example.h"
 *
 * MyApp::MyApp ()
 *   : DemoApplication ("CrystalSpace.MyApp", "myapplication", "myapplication",
 *		          "Description of 'myapplication'.")
 * {
 *   // Configure the options for DemoApplication
 * }
 *
 * bool MyApp::Application ()
 * {
 *   // Default behavior from DemoApplication
 *   if (!DemoApplication::Application ())
 *     return false;
 *
 *   // Initialize here your application
 *
 *   // Default behavior from DemoApplication for the creation of the scene
 *   if (!CreateRoom ())
 *     return false;
 *
 *   // Create here the main objects of the scene
 *
 *   // Run the application
 *   Run();
 *
 *   return true;
 * }
 *
 * //--------------------------
 * // main.cpp
 *
 * #include "example.h"
 *
 * CS_IMPLEMENT_APPLICATION
 * 
 * int main (int argc, char* argv[]) 
 * {
 *   return MyApp ().Main (argc, argv);
 * }
 * \endcode
 */
class CS_CRYSTALSPACE_EXPORT DemoApplication : public csApplicationFramework,
  public csBaseEventHandler, public CameraManager
{
  friend class HUDHelper;
  friend class CameraHelper;

 protected:
  /// Reference to the engine
  csRef<iEngine> engine;
  /// Reference to the loader
  csRef<iLoader> loader;
  /// Reference to the 3D graphics
  csRef<iGraphics3D> g3d;
  /// Reference to the 2D graphics
  csRef<iGraphics2D> g2d;
  /// Reference to the keyboard driver
  csRef<iKeyboardDriver> kbd;
  /// Reference to the virtual clock
  csRef<iVirtualClock> vc;
  /// Reference to the view
  csRef<iView> view;
  /// Reference to the frame printer
  csRef<FramePrinter> printer;
  /// Reference to the virtual file system
  csRef<iVFS> vfs;

  /// Reference to the main sector
  csRef<iSector> room;

  /// Current mouse cursor X's position
  int mouseX;
  /// Current mouse cursor Y's position
  int mouseY;

  /// Visual debugger
  csRef<CS::Debug::iVisualDebugger> visualDebugger;

  /// Base implementation of the method inherited from csBaseEventHandler
  virtual void Frame ();
  /// Base implementation of the method inherited from csBaseEventHandler
  virtual bool OnKeyboard (iEvent &event);
  /// Base implementation of the method inherited from csBaseEventHandler
  virtual bool OnMouseDown (iEvent &event);
  /// Base implementation of the method inherited from csBaseEventHandler
  virtual bool OnMouseUp (iEvent &event);
  /// Base implementation of the method inherited from csBaseEventHandler
  virtual bool OnMouseMove (iEvent &event);

  /**
   * Default initialization for the creation of the main sector (DemoApplication::room). It
   * creates a grey uniform background far away, initializes the camera, and adds a few lights.
   */
  virtual bool CreateRoom ();

  /// Command line helper
  CommandLineHelper commandLineHelper;

  /// HUD helper
  HUDHelper hudHelper;

  /// Camera helper
  CameraHelper cameraHelper;

  /**
   * Set whether or not the HUD must be displayed. Default value is true.
   */
  void SetHUDDisplayed (bool displayed);
  /// Return whether or not the HUD is currently displayed.
  bool GetHUDDisplayed ();

 public:
  /**
   * Constructor. The parameters of this constructor are used mainly to display
   * information when the '-help' option is used.
   * \param applicationName Name of the application, used to set
   * csApplicationFramework::SetApplicationName().
   * \param applicationCommand Name of the executable (eg "myapp").
   * \param applicationCommandUsage Syntax to use the executable (eg "myapp
   * <OPTIONS> filename"). Additional lines of text with examples of use can also
   * be added.
   * \param applicationDescription User friendly description of the application.
   */
  DemoApplication (const char* applicationName, const char* applicationCommand,
		   const char* applicationCommandUsage,
		   const char* applicationDescription);

  /// Base implementation of the method inherited from csApplicationFramework
  virtual void OnExit ();
  /// Base implementation of the method inherited from csApplicationFramework
  virtual bool OnInitialize (int argc, char* argv[]);
  /// Base implementation of the method inherited from csApplicationFramework
  virtual bool Application ();

 private:
  // Whether the HUD should be displayed or not
  bool hudDisplayed;
};

} // namespace Demo
} // namespace CS

/** @} */

#endif // __CSDEMOAPPLICATION_H__