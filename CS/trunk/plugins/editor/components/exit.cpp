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

#include "exit.h"

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

SCF_IMPLEMENT_FACTORY (Exit)

Exit::Exit (iBase* parent)
  : scfImplementationType (this, parent)
{
}

Exit::~Exit()
{
}

bool Exit::Initialize (iEditor* editor)
{
  this->editor = editor;

  // Setup the 'Quit' menu
  fileMenu = editor->GetMenuManager ()->GetSubMenu ("&File");
  separator = fileMenu->AppendSeparator ();
  quitItem = fileMenu->AppendItem ("&Quit\tCTRL-Q", "quit");

  csEventID events[] = { quitItem->GetEventID (), CS_EVENTLIST_END };
  RegisterQueue (editor->GetContext ()->GetEventQueue (), events);

  // Connect to the closing window event
  editor->GetFrame (0)->GetwxFrame ()->Connect
    (wxEVT_CLOSE_WINDOW, wxCommandEventHandler (Exit::OnQuit), 0, this);

  return true;
}

void Exit::Update ()
{
}

void Exit::Save (iDocumentNode* node) const
{
}

bool Exit::Load (iDocumentNode* node)
{
  return false;
}

bool Exit::HandleEvent (iEvent &event)
{
  // Forward the event to the closing of the window
  editor->GetFrame (0)->GetwxFrame ()->Close ();

  return true;
}

void Exit::OnQuit (wxCommandEvent& event)
{
  editor->ReportStatus ("Quit");

  // Destroy this top-level window
  editor->GetFrame (0)->GetwxFrame ()->Destroy ();
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

