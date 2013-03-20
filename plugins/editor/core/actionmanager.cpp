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

#include "cssysdef.h"
#include "csutil/objreg.h"
#include "csutil/scf.h"
#include "ieditor/context.h"
#include "iutil/eventq.h"

#include "actionmanager.h"

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

ActionManager::ActionManager (iObjectRegistry* obj_reg, iEditor* editor)
  : scfImplementationType (this), object_reg (obj_reg), editor (editor), maximumStackSize (100)
{
  csRef<iEventNameRegistry> registry = csQueryRegistry<iEventNameRegistry>
    (editor->GetContext ()->GetObjectRegistry ());
  eventID = registry->GetID ("crystalspace.editor.action.actiondone");
}

ActionManager::~ActionManager ()
{
}

bool ActionManager::Do (iAction* action)
{
  if (!action->Do (editor->GetContext ()))
    return false;
  
  redoStack.Empty ();
  undoStack.Push (action);

  // Limit the size of the 'Undo' stack
  if (undoStack.GetSize () > maximumStackSize)
    undoStack.DeleteIndex (0);

  NotifyListeners (action);
  
  return true;
}

bool ActionManager::Undo ()
{
  if (undoStack.IsEmpty ())
    return false;

  csRef<iAction> action (undoStack.Pop ());

  // Store redo action
  if (action->Undo (editor->GetContext ()))
    redoStack.Push (action);

  NotifyListeners (action);

  return true;
}

bool ActionManager::Redo ()
{
  if (redoStack.IsEmpty ())
    return false;
  
  csRef<iAction> action (redoStack.Pop ());
  
  if (action->Do (editor->GetContext ()))
    undoStack.Push (action);

  NotifyListeners (action);

  return true;
}

const iAction* ActionManager::PeekUndo () const
{
  if (undoStack.IsEmpty ())
    return 0;
  
  return undoStack.Get (undoStack.GetSize () - 1);
}

const iAction* ActionManager::PeekRedo () const
{
  if (redoStack.IsEmpty ())
    return 0;
  
  return redoStack.Get (redoStack.GetSize () - 1);
}

void ActionManager::SetMaximumStackSize (size_t size)
{
  maximumStackSize = size;
}

size_t ActionManager::GetMaximumStackSize () const
{
  return  maximumStackSize;
}

void ActionManager::NotifyListeners (iAction* action)
{
  iEventQueue* queue = editor->GetContext ()->GetEventQueue ();
  csRef<iEvent> event = queue->CreateEvent (eventID);
  event->Add ("action", action);
  queue->Post (event);
  queue->Process ();
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)
