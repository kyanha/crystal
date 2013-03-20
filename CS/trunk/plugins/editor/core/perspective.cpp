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
#include "csutil/stringquote.h"
#include "ieditor/context.h"
#include "ivaria/reporter.h"

#include "editor.h"
#include "perspective.h"
#include "window.h"

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

//------------------------------------  PerspectiveManager  ------------------------------------

PerspectiveManager::PerspectiveManager (iObjectRegistry* object_reg, Editor* editor)
  : scfImplementationType (this), object_reg (object_reg), editor (editor)
{
}

PerspectiveManager::~PerspectiveManager ()
{
}

iPerspective* PerspectiveManager::CreatePerspective (const char* name, iPerspective* iother)
{
  csRef<Perspective> perspective;
  if (iother)
  {
    Perspective* other = static_cast<Perspective*> (iother);
    perspective.AttachNew (new Perspective (this, name, other));
  }
  else perspective.AttachNew (new Perspective (this, name));
  perspectives.Push (perspective);
  // TODO: ensure no name conflict

  editor->context->UpdatePerspective (perspectives.GetSize () - 1);

  return perspective;
}

size_t PerspectiveManager::GetPerspectiveCount () const
{
  return perspectives.GetSize ();
}

iPerspective* PerspectiveManager::GetPerspective (size_t index) const
{
  return perspectives[index];
}

void PerspectiveManager::DeletePerspective (size_t index)
{
  editor->context->DeletePerspective (index);
  perspectives.DeleteIndex (index);
}

//------------------------------------  Perspective  ------------------------------------

Perspective::Perspective (PerspectiveManager* manager, const char* name)
  : scfImplementationType (this), manager (manager), name (name)
{
  root.AttachNew (new PerspectiveWindow ());
}

Perspective::Perspective (PerspectiveManager* manager, const char* name,
			  Perspective* other)
  : scfImplementationType (this), manager (manager), name (name)
{
  root.AttachNew (new PerspectiveWindow (other->root));
}

Perspective::~Perspective ()
{
}

Window* Perspective::CreateWindow (wxFrame* frame)
{
  Window* window = new Window (manager->object_reg, manager->editor, frame);

  wxBoxSizer* box = new wxBoxSizer (wxHORIZONTAL);
  box->Add (window, 1, wxEXPAND | wxALL, 0);
  frame->SetSizer (box);
  box->Layout ();

  root->SetupWindow (window);

  return window;
}

void Perspective::SetName (const char* name)
{
  this->name = name;
  // TODO: ensure no name conflict
  // TODO: context event UpdatePerspective
}

const char* Perspective::GetName () const
{
  return name;
}

iPerspectiveWindow* Perspective::GetRootWindow () const
{
  return root;
}

//------------------------------------  PerspectiveWindow  ------------------------------------

PerspectiveWindow::PerspectiveWindow ()
  : scfImplementationType (this), mode (SPLIT_NONE), position (0)
{
}

PerspectiveWindow::PerspectiveWindow (PerspectiveWindow* other)
  : scfImplementationType (this), mode (other->mode), pluginName (other->pluginName),
  position (other->position)
{
  if (mode != SPLIT_NONE)
  {
    child1.AttachNew (new PerspectiveWindow (other->child1));
    child2.AttachNew (new PerspectiveWindow (other->child2));
  }
}

PerspectiveWindow::~PerspectiveWindow ()
{
}

void PerspectiveWindow::SetSpace (const char* pluginName)
{
  this->pluginName = pluginName;
}

void PerspectiveWindow::SetSplitMode (SplitMode mode)
{
  this->mode = mode;

  if (mode == SPLIT_NONE)
  {
    child1.Invalidate ();
    child2.Invalidate ();
  }

  else if (!child1)
  {
    child1.AttachNew (new PerspectiveWindow ());
    child2.AttachNew (new PerspectiveWindow ());
  }
}

void PerspectiveWindow::SetSplitPosition (int position)
{
  this->position = position;
}

iPerspectiveWindow* PerspectiveWindow::GetChild1 () const
{
  return child1;
}

iPerspectiveWindow* PerspectiveWindow::GetChild2 () const
{
  return child2;
}

void PerspectiveWindow::SetupWindow (Window* window)
{
  if (mode == SPLIT_NONE)
  {
    window->Realize (pluginName);
  }

  else
  {
    window->Split ();
    window->SetSplitMode (mode == SPLIT_HORIZONTAL ? wxSPLIT_HORIZONTAL : wxSPLIT_VERTICAL);
    if (position) window->SetSashPosition (position, true);

    Window* childWindow1 = (Window*) window->GetWindow1 ();
    child1->SetupWindow (childWindow1);
    Window* childWindow2 = (Window*) window->GetWindow2 ();
    child2->SetupWindow (childWindow2);

    window->Realize ();
  }
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)
