/*
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

#ifndef __IEDITOR_CONTEXT_H__
#define __IEDITOR_CONTEXT_H__

#include "csutil/scf.h"
#include "csutil/scf_implementation.h"
#include "csutil/weakrefarr.h"
#include "iutil/strset.h"

struct iEventQueue;
struct iCamera;
struct iCollection;
struct iObject;
struct iObjectRegistry;

class wxEvtHandler;

namespace CS {
namespace EditorApp {

/**
 * 
 */
struct iContext : public virtual iBase
{
  SCF_INTERFACE (iContext, 1, 0, 0);
  
  virtual iObjectRegistry* GetObjectRegistry () = 0;

  virtual iEventQueue* GetEventQueue () = 0;

  /// To be called by the plugins at registration time. data is the default value.
  /// It has no effect if the ID is already registered
  virtual void RegisterData (csStringID id, iBase* data) = 0;

  /// Will fire an event for context change
  virtual void SetData (csStringID id, iBase* data) = 0;

  /// Get a specific context data
  virtual iBase* GetData (csStringID id) = 0;

  virtual void UpdatePerspective (size_t index) = 0;
  virtual void DeletePerspective (size_t index) = 0;
};

// TODO: in cseditor
/**
 * 
 */
struct iContextObjectSelection : public virtual iBase
{
  SCF_INTERFACE (iContextObjectSelection, 1, 0, 0);
  
  virtual void SetActiveObject (iObject* object) = 0;
  virtual iObject* GetActiveObject () const = 0;

  virtual void SetSelectedObjects
    (const csWeakRefArray<iObject>& objects) = 0;
  virtual const csWeakRefArray<iObject>& GetSelectedObjects () const = 0;

  virtual void AddSelectedObject (iObject* object) = 0;
  virtual void DeleteSelectedObject (iObject* object) = 0;
  virtual void ClearSelectedObjects () = 0;

  virtual bool ContainsSelectedObject (iObject* object) const = 0;
};

/**
 * 
 */
struct iContextCamera : public virtual iBase
{
  SCF_INTERFACE (iContextCamera, 1, 0, 0);
  
  virtual void SetCamera (iCamera* camera) = 0;
  virtual iCamera* GetCamera () = 0; // const?
};

/**
 * 
 */
struct iContextFileLoader : public virtual iBase
{
  SCF_INTERFACE (iContextFileLoader, 1, 0, 0);
  
  virtual void SetPath (const char* path) = 0;
  virtual const char* GetPath () = 0;
  virtual void SetFilename (const char* filename) = 0;
  virtual const char* GetFilename () = 0;
  virtual void SetCollection (iCollection* collection) = 0;
  virtual iCollection* GetCollection () const = 0;
};

} // namespace EditorApp
} // namespace CS

#endif
