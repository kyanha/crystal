/*
    Copyright (C) 2011-2012 by Jelle Hellemans, Christian Van Brussel

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

#ifndef __CSEDITOR_CONTEXT_H__
#define __CSEDITOR_CONTEXT_H__

#include "csutil/hash.h"
#include "csutil/refarr.h"
#include "ieditor/context.h"

#include <wx/event.h>

struct iObjectRegistry;

namespace CS {
namespace EditorApp {

class Context
  : public scfImplementation4<Context,
    iContext,
    iContextObjectSelection,
    iContextCamera,
    iContextFileLoader>
{
public:
  Context (iObjectRegistry* obj_reg);
  virtual ~Context ();
  
  //-- iContext
  virtual iObjectRegistry* GetObjectRegistry ();
  virtual iEventQueue* GetEventQueue ();

  virtual void RegisterData (csStringID id, iBase* data);
  virtual void SetData (csStringID id, iBase* data);
  virtual iBase* GetData (csStringID id);

  virtual void UpdatePerspective (size_t index);
  virtual void DeletePerspective (size_t index);

  //-- iContextObjectSelection
  virtual void SetActiveObject (iObject* object);
  virtual iObject* GetActiveObject () const;
  virtual void SetSelectedObjects
    (const csWeakRefArray<iObject>& objects);
  virtual const csWeakRefArray<iObject>& GetSelectedObjects () const;
  virtual void AddSelectedObject (iObject*);
  virtual void DeleteSelectedObject (iObject*);
  virtual void ClearSelectedObjects ();
  virtual bool ContainsSelectedObject (iObject* object) const;

  //-- iContextCamera
  virtual void SetCamera (iCamera*);
  virtual iCamera* GetCamera ();
  
  //-- iContextFileLoader
  virtual void SetPath (const char* path);
  virtual const char* GetPath ();
  virtual void SetFilename (const char* filename);
  virtual const char* GetFilename ();
  virtual void SetCollection (iCollection* collection);
  virtual iCollection* GetCollection () const;

private:
  void PostEvent (csEventID eventID);

private:
  //-- iContext
  iObjectRegistry* object_reg;
  csRef<iEventQueue> eventQueue;

  struct ContextData
  {
    // TODO
    iBase* data;
    wxEvtHandler* eventHandler;

    ~ContextData ()
      { delete eventHandler; }
  };

  csHash<ContextData, csStringID> contextData;

  csEventID eventUpdatePerspective;
  csEventID eventDeletePerspective;

  //-- iContextObjectSelection
  csWeakRef<iObject> active;
  csWeakRefArray<iObject> selection;

  csEventID eventSetActiveObject;
  csEventID eventSetSelectedObjects;
  csEventID eventAddSelectedObject;
  csEventID eventDeleteSelectedObject;
  csEventID eventClearSelectedObjects;

  //-- iContextCamera
  csWeakRef<iCamera> camera;

  csEventID eventSetCamera;

  //-- iContextFileLoader
  csString path;
  csString filename;
  csRef<iCollection> collection;

  csEventID eventSetCollection;
};

} // namespace EditorApp
} // namespace CS

#endif
