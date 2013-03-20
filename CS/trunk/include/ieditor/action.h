/*
    Copyright (C) 2007 by Seth Yastrov
    Copyright (C) 2011 by Jelle Hellemans

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

#ifndef __IEDITOR_ACTION_H__
#define __IEDITOR_ACTION_H__

#include <csutil/scf.h>
#include <csutil/scf_implementation.h>

#include <wx/string.h>

namespace CS {
namespace EditorApp {

struct iContext;

/**
 * An undoable action. All reversible operations should implement this
 * interface so that they can be undone.
 */

struct iAction : public virtual iBase
{
  SCF_INTERFACE (iAction, 0, 0, 1);

  /**
   * Does the action.
   */
  virtual bool Do (iContext* context ) = 0;
  
  /**
   * Undoes the action.
   */
  virtual bool Undo (iContext* context) = 0;

  /// Get a user-friendly description of the action.
  virtual const char* GetDescription () const = 0;
};

/**
 * Manages all undoable actions.
 */
struct iActionManager : public virtual iBase
{
  SCF_INTERFACE (iActionManager, 0, 0, 1);

  
  /**
   * Perform an action and record it in the undo history,
   * erasing the redo history in the process.
   * \return true if the action can be undone, false otherwise
   */
  virtual bool Do (iAction* action) = 0;

  /**
   * Undo the last action.
   * \return true if an action was undone, false if there were
   *         no actions to undo.
   */
  virtual bool Undo () = 0;

  /**
   * Redo the last undone action.
   * \return true if an action was redone, false if there were
   *         no actions to redo.
   */
  virtual bool Redo () = 0;

  /// Get the last undone action or 0 if none.
  virtual const iAction* PeekUndo () const = 0;

  /// Get the last redone action or 0 if none.
  virtual const iAction* PeekRedo () const = 0;

  /**
   * Set the maximum size for the stack of actions that can be undone. A value of 0 means
   * that there are no limit to the stack (are you really sure to want to do that?). The
   * default value is 100.
   */
  virtual void SetMaximumStackSize (size_t size) = 0;

  /// Get the maximum size for the stack of actions that can be undone.
  virtual size_t GetMaximumStackSize () const = 0;
};

} // namespace EditorApp
} // namespace CS

#endif
