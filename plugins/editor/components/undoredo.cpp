/*
    Copyright (C) 2012 by Christian Van Brussel
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
#include "csutil/scf.h"
#include "ieditor/action.h"
#include "ieditor/context.h"

#include "undoredo.h"

CS_PLUGIN_NAMESPACE_BEGIN(CSEditor)
{

SCF_IMPLEMENT_FACTORY (UndoRedo)

UndoRedo::UndoRedo (iBase* parent)
  : scfImplementationType (this, parent)
{
}

UndoRedo::~UndoRedo()
{
}

bool UndoRedo::Initialize (iEditor* editor)
{
  this->editor = editor;

  // Setup the menus
  editMenu = editor->GetMenuManager ()->GetSubMenu ("&Edit");
  undoItem = editMenu->AppendItem ("&Undo\tCtrl+Z", "undo");
  redoItem = editMenu->AppendItem ("&Redo\tCtrl+Y", "redo");

  csRef<iEventNameRegistry> registry = csQueryRegistry<iEventNameRegistry>
    (editor->GetContext ()->GetObjectRegistry ());

  csEventID events[] = {
    registry->GetID ("crystalspace.editor.action.actiondone"),
    undoItem->GetEventID (),
    redoItem->GetEventID (),
    CS_EVENTLIST_END
  };

  RegisterQueue (editor->GetContext ()->GetEventQueue (), events);

  UpdateMenus ();

  return true;
}

void UndoRedo::Update ()
{
}

void UndoRedo::Save (iDocumentNode* node) const
{
}

bool UndoRedo::Load (iDocumentNode* node)
{
  return false;
}

bool UndoRedo::HandleEvent (iEvent &event)
{
  if (event.Name == undoItem->GetEventID ())
    editor->GetActionManager ()->Undo ();

  else if (event.Name == redoItem->GetEventID ())
    editor->GetActionManager ()->Redo ();

  else UpdateMenus ();

  return true;
}

void UndoRedo::UpdateMenus ()
{
  const iAction* undo = editor->GetActionManager ()->PeekUndo ();
  const iAction* redo = editor->GetActionManager ()->PeekRedo ();

  undoItem->GetwxMenuItem ()->Enable (undo != nullptr);
  redoItem->GetwxMenuItem ()->Enable (redo != nullptr);

  if (undo)
    undoItem->GetwxMenuItem ()->SetItemLabel
      (wxT("Undo \"") + wxString(undo->GetDescription (), wxConvUTF8) + wxT("\"\tCtrl+Z"));
  
  if (redo)
    redoItem->GetwxMenuItem ()->SetItemLabel
      (wxT("Redo \"") + wxString(redo->GetDescription (), wxConvUTF8) + wxT("\"\tCtrl+Y"));
}

}
CS_PLUGIN_NAMESPACE_END(CSEditor)

