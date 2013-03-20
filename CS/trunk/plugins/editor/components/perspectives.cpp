/*
    Copyright (C) 2012 by Christian Van Brussel

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
#include "ieditor/context.h"

#include "perspectives.h"

#include <wx/textdlg.h>

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

SCF_IMPLEMENT_FACTORY (Perspectives)

Perspectives::Perspectives (iBase* parent)
  : scfImplementationType (this, parent)
{
}

Perspectives::~Perspectives()
{
}

bool Perspectives::Initialize (iEditor* editor)
{
  this->editor = editor;

  // Setup the basic menu
  viewMenu = editor->GetMenuManager ()->GetSubMenu ("&View");
  createItem = viewMenu->AppendItem ("&Create Perspective", "createperspective");

  // Register to the event list
  iEventNameRegistry* registry =
    csEventNameRegistry::GetRegistry (editor->GetContext ()->GetObjectRegistry ());
  eventUpdatePerspective =
    registry->GetID ("crystalspace.editor.context.base.updateperspective");

  csEventID events[] = { createItem->GetEventID (),
			 eventUpdatePerspective,
			 CS_EVENTLIST_END };
  RegisterQueue (editor->GetContext ()->GetEventQueue (), events);

  return true;
}

void Perspectives::Update ()
{
}

void Perspectives::Save (iDocumentNode* node) const
{
}

bool Perspectives::Load (iDocumentNode* node)
{
  return false;
}

bool Perspectives::HandleEvent (iEvent &event)
{
  // Update of perspective
  if (event.GetName () == eventUpdatePerspective)
  {
    // Check if it is a new perspective
    size_t index;
    event.Retrieve ("index", index);
    if (index + 1 > perspectives.GetSize ())
      AddPerspective (editor->GetPerspectiveManager ()->GetPerspective (index));

    // TODO: else rename the menu
  }

  // Creation of a new perspective
  else if (event.GetName () == createItem->GetEventID ())
  {
    wxTextEntryDialog dialog (editor->GetFrame (0)->GetwxFrame (),
			      wxT ("Enter the name of the new perspective:"),
			      wxT ("Create a new perspective"));
    dialog.SetValue (wxString::Format (wxT ("Perspective %zu"),
				       editor->GetPerspectiveManager ()->GetPerspectiveCount () + 1));
    dialog.CentreOnParent ();
    if (dialog.ShowModal () != wxID_OK)
      return true;

    editor->GetPerspectiveManager ()->CreatePerspective
      (dialog.GetValue ().mb_str (), editor->GetFrame (0)->GetPerspective ());
  }

  // Selection of a perspective
  else
  {
    for (size_t i = 0; i < perspectives.GetSize (); i++)
      if (event.GetName () == perspectives[i]->GetEventID ())
      {
	iPerspective* perspective = editor->GetPerspectiveManager ()->GetPerspective (i);
	editor->GetFrame (0)->SetPerspective (perspective);
	break;
      }
  }

  return true;
}

void Perspectives::AddPerspective (iPerspective* perspective)
{
  // Create a separator it not yet made
  if (!perspectives.GetSize ())
    separator = viewMenu->AppendSeparator ();

  // Create the menu item
  csRef<iMenuItem> item =
    viewMenu->AppendItem (perspective->GetName (), perspective->GetName ());
  perspectives.Push (item);

  // Update the list of events we are listening to
  csEventID events[perspectives.GetSize () + 2];
  events[0] = createItem->GetEventID ();
  events[1] = eventUpdatePerspective;
  for (size_t i = 0; i < perspectives.GetSize (); i++)
    events[i + 2] = perspectives[i]->GetEventID ();
  events[perspectives.GetSize () + 2] = CS_EVENTLIST_END;
  RegisterQueue (editor->GetContext ()->GetEventQueue (), events);
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

