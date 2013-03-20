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

#include "cssysdef.h"
#include "csutil/cseventq.h"
#include "csutil/objreg.h"
#include "csutil/scf.h"
#include "iengine/camera.h"
#include "iengine/collection.h"
#include "iutil/event.h"
#include "iutil/eventq.h"
#include "iutil/object.h"
#include "ivaria/reporter.h"

#include "context.h"

namespace CS {
namespace EditorApp {

Context::Context (iObjectRegistry* obj_reg)
  : scfImplementationType (this), object_reg (obj_reg)
{
  // Create a new event queue (and hack around the fact that a new
  // csEventQueue instance will be registered to the object registry,
  // hence overwriting the global event queue).
  csRef<iEventQueue> mainEventQueue =
    csQueryRegistry<iEventQueue> (object_reg);
  eventQueue.AttachNew (new csEventQueue (object_reg));
  object_reg->Register (mainEventQueue, "contextQueue");

  // Initialize the event ID's
  csRef<iEventNameRegistry> registry =
    csQueryRegistry<iEventNameRegistry> (object_reg);
  eventUpdatePerspective =
    registry->GetID ("crystalspace.editor.context.base.updateperspective");
  eventDeletePerspective =
    registry->GetID ("crystalspace.editor.context.base.deleteperspective");
  eventSetActiveObject =
    registry->GetID ("crystalspace.editor.context.selection.setactiveobject");
  eventSetSelectedObjects =
    registry->GetID ("crystalspace.editor.context.selection.setselectedobjects");
  eventAddSelectedObject =
    registry->GetID ("crystalspace.editor.context.selection.addselectedobject");
  eventDeleteSelectedObject =
    registry->GetID ("crystalspace.editor.context.selection.deleteselectedobject");
  eventClearSelectedObjects =
    registry->GetID ("crystalspace.editor.context.selection.clearselectedobjects");
  eventSetCamera =
    registry->GetID ("crystalspace.editor.context.camera.setcamera");
  eventSetCollection =
    registry->GetID ("crystalspace.editor.context.fileloader.setcollection");
}

Context::~Context ()
{
}

iObjectRegistry* Context::GetObjectRegistry ()
{
  return object_reg;
}

iEventQueue* Context::GetEventQueue ()
{
  return eventQueue;
}

void Context::RegisterData (csStringID id, iBase* data)
{
  ContextData cdata;
  cdata.data = data;
  cdata.eventHandler = new wxEvtHandler ();
  contextData.PutUnique (id, cdata);
}

void Context::SetData (csStringID id, iBase* data)
{
  ContextData* cdata = contextData.GetElementPointer (id);

#ifdef CS_DEBUG
  if (!cdata)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.context.base",
	      "Invalid context data ID");
    return;
  }
#endif // CS_DEBUG

  cdata->data = data;
  // TODO: callbacks
}

iBase* Context::GetData (csStringID id)
{
  ContextData* cdata = contextData.GetElementPointer (id);

#ifdef CS_DEBUG
  if (!cdata)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.context.base",
	      "Invalid context data ID");
    return nullptr;
  }
#endif // CS_DEBUG

  return cdata->data;
}

void Context::UpdatePerspective (size_t index)
{
  csRef<iEvent> event = eventQueue->CreateEvent (eventUpdatePerspective);
  event->Add ("index", index);
  eventQueue->Post (event);
  eventQueue->Process ();
}

void Context::DeletePerspective (size_t index)
{
  csRef<iEvent> event = eventQueue->CreateEvent (eventDeletePerspective);
  event->Add ("index", index);
  eventQueue->Post (event);
  eventQueue->Process ();
}

void Context::PostEvent (csEventID eventID)
{
  csRef<iEvent> event = eventQueue->CreateEvent (eventID);
  eventQueue->Post (event);
  eventQueue->Process ();
}

void Context::SetActiveObject (iObject* object)
{
  active = object;
  PostEvent (eventSetActiveObject);
}

iObject* Context::GetActiveObject () const
{
  return active;
}

void Context::SetSelectedObjects
    (const csWeakRefArray<iObject>& objects)
{
  selection = objects;
  PostEvent (eventSetSelectedObjects);
}

const csWeakRefArray<iObject>& Context::GetSelectedObjects () const
{
  return selection; 
}

void Context::AddSelectedObject (iObject* object)
{
  if (!object || ContainsSelectedObject (object))
    return;
  selection.Push (object);

  // Post the event
  csRef<iEvent> event = eventQueue->CreateEvent (eventAddSelectedObject);
  event->Add ("object", object);
  eventQueue->Post (event);
  eventQueue->Process ();
}

void Context::DeleteSelectedObject (iObject* object)
{
  if (!object || !ContainsSelectedObject (object))
    return;
  selection.Delete (object);

  // Post the event
  csRef<iEvent> event = eventQueue->CreateEvent (eventDeleteSelectedObject);
  event->Add ("object", object);
  eventQueue->Post (event);
  eventQueue->Process ();
}

void Context::ClearSelectedObjects ()
{
  selection.DeleteAll ();
  PostEvent (eventClearSelectedObjects);
}

bool Context::ContainsSelectedObject (iObject* object) const
{
  for (csWeakRefArray<iObject>::ConstIterator it = selection.GetIterator (); it.HasNext (); )
    if (it.Next () == object)
      return true;
  return false;
}

void Context::SetCamera (iCamera* cam)
{
  camera = cam;
  PostEvent (eventSetCamera);
}

iCamera* Context::GetCamera ()
{
  return camera;
}

void Context::SetPath (const char* path)
{
  this->path = path;
}

const char* Context::GetPath ()
{
  return path;
}

void Context::SetFilename (const char* filename)
{
  this->filename = filename;
}

const char* Context::GetFilename ()
{
  return filename;
}

void Context::SetCollection (iCollection* collection)
{
  this->collection = collection;
  PostEvent (eventSetCollection);
}

iCollection* Context::GetCollection () const
{
  return collection;
}

} // namespace EditorApp
} // namespace CS
