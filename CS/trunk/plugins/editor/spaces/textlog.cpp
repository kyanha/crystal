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
#include "ieditor/editor.h"

#include "textlog.h"

#include <wx/wx.h>

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

SCF_IMPLEMENT_FACTORY (TextLog)

TextLog::TextLog (iBase* parent)
 : scfImplementationType (this, parent), enabled (true)
{  
}

bool TextLog::Initialize (iObjectRegistry* obj_reg, iEditor* editor,
			  iSpaceFactory* fact, wxWindow* parent)
{
  object_reg = obj_reg;
  factory = fact;

  // Find the logger component and 
  iEditorComponent* component = editor->GetComponentManager ()->FindComponent
    ("crystalspace.editor.component.logger");
  if (!component)
    return ReportError ("The logger component is not registered to the space manager");

  csRef<CS::EditorApp::Component::iLogger> logger =
    scfQueryInterface<CS::EditorApp::Component::iLogger> (component);
  if (!logger)
    return ReportError ("The logger component does not implement the iLogger interface");

  // Create the log text control
  window = logger->CreateTextCtrl (parent);

  return true;
}

TextLog::~TextLog ()
{
  window->Destroy ();
}

wxWindow* TextLog::GetwxWindow ()
{
  return window;
}

void TextLog::Update ()
{
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

