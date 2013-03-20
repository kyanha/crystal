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

#include "cssysdef.h"
#include "csutil/objreg.h"
#include "csutil/scf.h"
#include "csutil/stringquote.h"
#include "ivaria/reporter.h"

#include "editor.h"
#include "perspective.h"
#include "spacemanager.h"
#include "window.h"

#include <wx/artprov.h>
#include <wx/dc.h>
#include <wx/dcmemory.h>
#include <wx/sizer.h>

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{
  
BEGIN_EVENT_TABLE (Window, wxSplitterWindow)
  EVT_SPLITTER_DCLICK (wxID_ANY, Window::OnDClick)
  EVT_SPLITTER_UNSPLIT (wxID_ANY, Window::OnUnsplitEvent)
  EVT_SIZE (Window::OnSize)
END_EVENT_TABLE ()

Window::Window (iObjectRegistry* object_reg, iEditor* editor, wxWindow* parent,
		bool hor)
  : wxSplitterWindow (parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		      wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN), 
  object_reg (object_reg), editor (editor), horizontal (hor)
{
  ViewControl* control = new ViewControl (object_reg, editor, this);
  Initialize (control);
  SetSashGravity (0.5);
}

Window::Window (iObjectRegistry* object_reg, iEditor* editor, wxWindow* parent,
		ViewControl* control, bool hor)
  : wxSplitterWindow (parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		      wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN), 
  object_reg (object_reg), editor (editor), horizontal (hor)
{
  control->Reparent (this);
  Initialize (control);
  SetSashGravity (0.5);
}

Window::~Window ()
{
}

bool Window::Split ()
{
  if (IsSplit ()) return false;
  ViewControl* c = (ViewControl*) GetWindow1 ();
  Window* w1 = new Window (object_reg, editor, this, c, !horizontal);
  Window* w2 = new Window (object_reg, editor, this, !horizontal);
  
  if (horizontal) SplitHorizontally (w1, w2);
  else SplitVertically (w1, w2);

  return true;
}

void Window::Realize (const char* pluginName)
{
  if (IsSplit ()) return;

  ViewControl* control = (ViewControl*) GetWindow1 ();
  control->Realize (pluginName);
}

void Window::OnDClick (wxSplitterEvent& event)
{
  event.Veto ();
  wxWindow* w1 = GetWindow1 ();
  wxWindow* w2 = GetWindow2 ();
  Unsplit ();
  if (GetSplitMode () == wxSPLIT_VERTICAL)
    SplitHorizontally (w1, w2);
  else
    SplitVertically (w1, w2);
}

void Window::OnUnsplitEvent (wxSplitterEvent& event)
{
  wxWindow* w = event.GetWindowBeingRemoved ();
  if (w) w->Destroy ();
}

void Window::OnSize (wxSizeEvent& event)
{
  Layout ();
  event.Skip ();
}

// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE (ViewControl, wxPanel)
  EVT_SIZE (ViewControl::OnSize)
END_EVENT_TABLE ()

ViewControl::ViewControl (iObjectRegistry* object_reg, iEditor* editor, wxWindow* parent)
: wxPanel (parent, wxID_ANY, wxDefaultPosition, wxDefaultSize), object_reg (object_reg),
  editor (editor)
{
  box = new wxBoxSizer (wxVERTICAL);
  
  wxPanel* menuBar = new wxPanel (this, wxID_ANY, wxDefaultPosition, wxSize (-1, -1));
  wxBoxSizer* bar = new wxBoxSizer (wxHORIZONTAL);
  wxToolBar* tb = new wxToolBar (menuBar, wxID_ANY);
      
  // Create the space combo box
  comboBox = new SpaceComboBox (object_reg, editor, tb, this);
  tb->AddControl (comboBox);

  tb->Realize ();
  bar->Add (tb, 0, /*wxEXPAND |*/ wxALIGN_LEFT, 0);

  toolbar = new wxPanel (menuBar, wxID_ANY);
  bar->Add (toolbar, 1, wxEXPAND | wxALL, 0);

  {
    wxToolBar* tb = new wxToolBar (menuBar, wxID_ANY);
    tb->AddTool (1, wxT ("Split"), wxArtProvider::GetBitmap (wxART_ADD_BOOKMARK, wxART_TOOLBAR, wxSize (16, 16)));
    tb->Connect (1, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (ViewControl::OnClicked), 0, this);
    tb->AddTool (2, wxT ("Duplicate"), wxArtProvider::GetBitmap (wxART_GO_TO_PARENT, wxART_TOOLBAR, wxSize (16, 16)));
    tb->Connect (2, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (ViewControl::OnClicked), 0, this);
/*
    tb->AddSeparator ();
    tb->AddTool (2, wxT ("Test"), wxArtProvider::GetBitmap (wxART_NEW_DIR, wxART_TOOLBAR, wxSize (16, 16)));
    tb->AddTool (3, wxT ("Test"), wxArtProvider::GetBitmap (wxART_GO_DIR_UP, wxART_TOOLBAR, wxSize (16, 16)));
    tb->AddTool (4, wxT ("Test"), wxArtProvider::GetBitmap (wxART_GO_HOME, wxART_TOOLBAR, wxSize (16, 16)));
    tb->AddTool (5, wxT ("Test"), wxArtProvider::GetBitmap (wxART_FILE_OPEN, wxART_TOOLBAR, wxSize (16, 16)));
*/
    tb->Realize ();
    bar->Add (tb, 0, /*wxEXPAND |*/ wxALIGN_RIGHT, 0);
  }

  menuBar->SetSizer (bar);

  box->Add (menuBar, 0, wxEXPAND);
  SetSizer (box);
}

ViewControl::~ViewControl ()
{
}

void ViewControl::Realize (const char* pluginName)
{
  iComponentManager* imgr = editor->GetComponentManager ();
  ComponentManager* mgr = static_cast<ComponentManager*> (imgr);

  // Look for any space provided
  if (pluginName)
  {
    size_t index = 0;
    iSpaceFactory* factory = nullptr;
    for (csRefArray<SpaceFactory>::ConstIterator it =
	   mgr->GetSpaceFactories ().GetIterator (); it.HasNext (); index++)
    {
      iSpaceFactory* n = it.Next ();
      SpaceFactory* f = static_cast<SpaceFactory*> (n);
      if (strcmp (f->GetIdentifier (), pluginName) == 0)
      {
	factory = f;
	break;
      }
    }

    if (!factory)
    {
      csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.editor.core.window",
		"Unvalid space factory %s in the definition of the perspective",
		CS::Quote::Single (pluginName));
      return;
    }

    if ((factory->GetMultipleAllowed () || factory->GetEnabledCount () == 0))
    {
      CreateSpace (factory, index);
      return;
    }
  }

  // Otherwise search for the least represented space
  size_t i = 0;
  size_t smallest = 1000;
  size_t smallestIndex = 0;
  iSpaceFactory* smallestFactory = nullptr;

  for (csRefArray<SpaceFactory>::ConstIterator it =
	 mgr->GetSpaceFactories ().GetIterator (); it.HasNext (); )
  {
    iSpaceFactory* f = it.Next ();

    // Check for the least represented space
    if (smallest > f->GetEnabledCount ()
	&& (f->GetMultipleAllowed () || f->GetEnabledCount () == 0))
    {
      smallest = f->GetEnabledCount ();
      smallestIndex = i;
      smallestFactory = f;
    }

    i++;
  }

  // Create the space
  if (smallestFactory) CreateSpace (smallestFactory, smallestIndex);
}

void ViewControl::CreateSpace (iSpaceFactory* factory, size_t index)
{
  iComponentManager* imgr = editor->GetComponentManager ();
  ComponentManager* mgr = static_cast<ComponentManager*> (imgr);

  space = factory->CreateInstance (this);
  spaces.Put (index, space);
  comboBox->SetSelectedIndex (index);

  box->Insert (0, space->GetwxWindow (), 1, wxEXPAND, 0);
  mgr->ReDraw (space);
  box->Layout ();
}

void ViewControl::OnClicked (wxCommandEvent& event)
{
  if (event.GetId () == 1)
  {
    Window* window = (Window*) this->GetParent ();
    window->Split ();
    ((Window*) window->GetWindow2 ())->Realize ();

    // Disable temporarily the current space. This is needed for the 3D view
    // because its OpenGL canvas can get temporarily invalid.
    space->SetEnabled (false);
    space->SetEnabled (true);
  }
  else
  {
    // Create a new perspective for this view
    iPerspectiveManager* perspectiveManager = editor->GetPerspectiveManager ();
    iPerspective* perspective = perspectiveManager->CreatePerspective
      (csString ().Format ("Perspective %zu",
			   perspectiveManager->GetPerspectiveCount () +1));
    iPerspectiveWindow* window = perspective->GetRootWindow ();
    window->SetSpace (space->GetFactory ()->GetIdentifier ());

    // Create the editor frame
    iEditorFrame* frame = editor->CreateFrame (nullptr, perspective);
    EditorFrame* csframe = static_cast<EditorFrame*> (frame);
    csframe->Init ();
  }
}

void ViewControl::OnSize (wxSizeEvent& event)
{
  Layout ();
  event.Skip ();
}

// ----------------------------------------------------------------------------

SpaceComboBox::SpaceComboBox
  (iObjectRegistry* object_reg, iEditor* editor, wxWindow* parent, ViewControl* ctrl)
  : wxBitmapComboBox (parent, wxID_ANY, wxEmptyString,wxDefaultPosition,
		      wxSize (50, 20),0, NULL, wxCB_READONLY),
    object_reg (object_reg), editor (editor), control (ctrl), lastIndex ((size_t) ~0)
{
  // Build the list of menu entries for all spaces
  iComponentManager* imgr = editor->GetComponentManager ();
  ComponentManager* mgr = static_cast<ComponentManager*> (imgr);
  csRefArray<SpaceFactory>::ConstIterator spaces =
    mgr->GetSpaceFactories ().GetIterator ();

  while (spaces.HasNext ())
  {
    // Add the menu entry for this space
    iSpaceFactory* f = spaces.Next ();
    wxString label (f->GetLabel (), wxConvUTF8);
    Append (label, f->GetIcon ());
  }

  // Listen to the OnSelected event
  Connect (GetId (), wxEVT_COMMAND_COMBOBOX_SELECTED,
	   wxCommandEventHandler (SpaceComboBox::OnSelected), 0, this);  
}

SpaceComboBox::~SpaceComboBox ()
{
}

void SpaceComboBox::SetSelectedIndex (size_t index)
{
  lastIndex = index;
  SetSelection (index);
}

void SpaceComboBox::OnSelected (wxCommandEvent& event)
{
  // Search the space that is being selected
  iComponentManager* imgr = editor->GetComponentManager ();
  ComponentManager* mgr = static_cast<ComponentManager*> (imgr);

  csRefArray<SpaceFactory>::ConstIterator spaces =
    mgr->GetSpaceFactories ().GetIterator ();

  size_t i = 0;
  while (spaces.HasNext ())
  {
    iSpaceFactory* f = spaces.Next ();
    wxString label (f->GetLabel (), wxConvUTF8);

    if (GetValue () == label)
    {
      // Don't do anything if the selection is still the same
      if (lastIndex == i) return;

      // Create an instance of the selected space
      if (f->GetMultipleAllowed () || f->GetEnabledCount () == 0)
      {
	// Invalidate the previous space if any
	if (lastIndex != (size_t) ~0)
	{
	  control->layout.Invalidate ();
	  control->box->Detach (control->space->GetwxWindow ());
	  control->space->SetEnabled (false);
	  control->space->GetwxWindow ()->Hide ();
	}

	// Get or create the selected space
	if (control->spaces.Contains (i))
	{
	  control->space = *control->spaces.GetElementPointer (i);
	  control->space->SetEnabled (true);
	  control->space->GetwxWindow ()->Show ();
	}

        else
	{
	  control->space = f->CreateInstance (control);
	  control->spaces.Put (i, control->space);
	}

	// Update the combo box and the view
        control->box->Insert (0, control->space->GetwxWindow (), 1, wxEXPAND, 0);
        SetSelection (i);
	lastIndex = i;
        mgr->ReDraw (control->space);
	control->box->Layout ();

	return;
      }

      break;
    }

    i++;
  }

  // Put back the previous selection
  if (lastIndex != (size_t) ~0)
    SetSelection (lastIndex);
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)
