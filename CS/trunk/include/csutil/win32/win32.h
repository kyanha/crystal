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

#ifndef __CS_WIN32_H__
#define __CS_WIN32_H__

/**\file
 * Win32-specific interfaces
 */

#ifndef __CS_CSOSDEFS_H__
#  error csosdefs.h should be included before "csutil/win32.h"
#endif

#include "csutil/scf.h"

/// CrystalSpace window class
#define CS_WIN32_WINDOW_CLASS_NAME  "CrystalSpaceWin32"
/// CrystalSpace window class (Unicode version)
#define CS_WIN32_WINDOW_CLASS_NAMEW L"CrystalSpaceWin32"

SCF_VERSION (iWin32ExceptionHandler, 0, 0, 1);

/**
 * Exception handler.
 */
struct iWin32ExceptionHandler : public iBase
{
  /// What to do after the user-defined handler finished
  enum ExceptAction
  {
    /**
     * Call the default exception handler (usually causes termination,
     * but may also pop up some messages)
     */
    CallDefaultHandler,
    /**
     * Call the old exception handler (usually causes a message to pop
     * up, or Dr. Watson to be called)
     */
    CallOldHandler,
    /// Just terminate the process.
    JustTerminate
  };
  /**
   * User-defined exception handler.
   */
  virtual ExceptAction HandleException (struct _EXCEPTION_POINTERS* info) = 0;
};

SCF_VERSION (iWin32Assistant, 0, 0, 5);

/**
 * This interface describes actions specific to the Windows platform.
 * An instance of this object will be registered to the object registry
 * with tag `iWin32Assistant'.
 */
struct iWin32Assistant : public iBase
{
  /// Returns the HINSTANCE of the program
  virtual HINSTANCE GetInstance () const = 0;
  /// Returns true if the program is 'active', false otherwise.
  virtual bool GetIsActive () const = 0;
  /// Gets the nCmdShow of the WinMain().
  virtual int GetCmdShow () const = 0;
  /// Set the mouse shape.
  virtual bool SetCursor (int cursor) = 0;
  /// Disable the console window (if present)
  virtual void DisableConsole () = 0;
  /// Display a nice message box
  virtual void AlertV (HWND window, int type, const char* title, 
    const char* okMsg, const char* msg, va_list args) = 0;
  /**
   * Returns the handle to the main application window.
   */
  virtual HWND GetApplicationWindow() = 0;

  /**
   * Sets wether CS should get Messages on it's own
   */
  virtual void UseOwnMessageLoop (bool ownmsgloop) = 0;
  /**
   * Gets whether CS should get Messages on it's own
   */
  virtual bool HasOwnMessageLoop () = 0;

  /**
   * Set the handler for unfiltered exceptions.
   */
  virtual void SetExceptionHandler (iWin32ExceptionHandler* handler) = 0;
};

#endif // __CS_WIN32_H__
