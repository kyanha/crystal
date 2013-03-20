/*
    Copyright (C) 2011-2012 by Jelle Hellemans, Christian Van Brussel
    Copyright (C) 2007 by Seth Yastrov

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
#include "cstool/initapp.h"
#include "csutil/scf.h"
#include "csutil/stringquote.h"
#include "iutil/virtclk.h"

#include "actionmanager.h"
#include "editor.h"
#include "menubar.h"
#include "operatormanager.h"
#include "perspective.h"
#include "spacemanager.h"
#include "statusbar.h"
#include "window.h"

#include <wx/app.h>
#include <wx/log.h>

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

//------------------------------------  EditorManager  ------------------------------------

SCF_IMPLEMENT_FACTORY (EditorManager)

EditorManager::EditorManager (iBase* parent)
  : scfImplementationType (this, parent), pump (nullptr)
{
}

EditorManager::~EditorManager ()
{
  delete pump;

  // The deletion of the iEditor instances is managed automatically
  // by wxWidgets
}

bool EditorManager::ReportError (const char* description, ...) const
{
  va_list arg;
  va_start (arg, description);
  csReportV (object_reg, CS_REPORTER_SEVERITY_ERROR,
	     "crystalspace.editor.core",
	     description, arg);
  va_end (arg);
  return false;
}

bool EditorManager::ReportWarning (const char* description, ...) const
{
  va_list arg;
  va_start (arg, description);
  csReportV (object_reg, CS_REPORTER_SEVERITY_WARNING,
	     "crystalspace.editor.core",
	     description, arg);
  va_end (arg);
  return false;
}

bool EditorManager::Initialize (iObjectRegistry* reg)
{
  object_reg = reg;
  //StartEngine ();

  // Find references to the virtual clock and the main event queue
  vc = csQueryRegistry<iVirtualClock> (object_reg);
  eventQueue = csQueryRegistry<iEventQueue> (object_reg);

  return true;
}

bool EditorManager::StartEngine ()
{
  // Request every standard plugin of Crystal Space
  if (!csInitializer::RequestPlugins (object_reg,
        CS_REQUEST_VFS,
        CS_REQUEST_FONTSERVER,
        CS_REQUEST_REPORTER,
        CS_REQUEST_END))
  {
    wxLogError (wxT ("Can't initialize standard Crystal Space plugins!"));
    return false;
  }

  return true;
}

bool EditorManager::StartApplication ()
{
  // Open the main system. This will open all the previously loaded plug-ins.
  if (!csInitializer::OpenApplication (object_reg))
    return ReportError ("Error opening system!");

  // TODO: remove this hack?
  for (size_t i = 0; i < editors.GetSize (); i++)
    editors[i]->Init ();

  // Initialize the pump (the refresh rate is in millisecond)
  // This pump will update the editors and the manager regularly
  pump = new Pump (this);
  pump->Start (20);

  // Connect to the Idle event
  wxTheApp->Connect (wxEVT_IDLE, wxIdleEventHandler (EditorManager::OnIdle), 0, this);

  return true;
}

iEditor* EditorManager::CreateEditor (const char* name, const char* title, iContext* context)
{
  if (!context)
  {
    ReportError ("No context provided when creating an editor instance!");
    return nullptr;
  }

  csRef<Editor> editor;
  editor.AttachNew (new Editor (this, name, title, context));
  editors.Push (editor);

  return editor;
}

void EditorManager::DeleteEditor (iEditor* editor)
{
  Editor* cseditor = static_cast<Editor*> (editor);
  editors.Delete (cseditor);
}

iEditor* EditorManager::FindEditor (const char* name)
{
  for (size_t i = 0; i < editors.GetSize (); i++)
    if (editors[i]->name == name)
      return editors[i];
  return nullptr;
}

iEditor* EditorManager::GetEditor (size_t index)
{
  return editors[index];
}

size_t EditorManager::GetEditorCount () const
{
  return editors.GetSize ();
}

void EditorManager::Update ()
{
  // Update each editor instance
  for (size_t i = 0; i < editors.GetSize (); i++)
    editors[i]->Update ();

  // Ensure idle events are regularly generated
  wxWakeUpIdle ();
}

void EditorManager::OnIdle (wxIdleEvent& event)
{
  // Update the virtual clock and the global event queue
  vc->Advance ();
  eventQueue->Process ();

  // Ask for more idle events
  // TODO: add an option defining whether or not this is suitable
  //event.RequestMore();
}

//------------------------------------  Editor  ------------------------------------

Editor::Editor (EditorManager* manager, const char* name, const char* title,
		iContext* context)
  // TODO: use size from CS config
  : scfImplementationType (this), name (name), title (title), manager (manager),
    context (context), statusBar (nullptr)
{
  // Create the main objects and managers
  actionManager.AttachNew (new ActionManager (manager->object_reg, this));
  menuManager.AttachNew (new MenuManager (this));
  operatorManager.AttachNew (new OperatorManager (manager->object_reg, this));
  perspectiveManager.AttachNew (new PerspectiveManager (manager->object_reg, this));
  componentManager.AttachNew (new ComponentManager (this));

  // Create the default frame
  CreateFrame (title);
}

Editor::~Editor ()
{
}

iContext* Editor::GetContext () const
{
  return context;
}

iActionManager* Editor::GetActionManager () const
{
  return actionManager;
}

iEditorManager* Editor::GetEditorManager () const
{
  return manager;
}

iMenuManager* Editor::GetMenuManager () const
{
  return menuManager;
}

iOperatorManager* Editor::GetOperatorManager () const
{
  return operatorManager;
}

iPerspectiveManager* Editor::GetPerspectiveManager () const
{
  return perspectiveManager;
}

iComponentManager* Editor::GetComponentManager () const
{
  return componentManager;
}

csPtr<iProgressMeter> Editor::CreateProgressMeter () const
{
  csRef<iProgressMeter> meter;
  meter.AttachNew (new StatusBarProgressMeter (statusBar));
  return csPtr<iProgressMeter> (meter);
}

void Editor::ReportStatus (const char* text)
{
  if (frames.GetSize ())
    frames[0]->SetStatusText (wxString::FromUTF8 (text));
  csReport (manager->object_reg, CS_REPORTER_SEVERITY_NOTIFY,
	    "status", text, nullptr);
}

iEditorFrame* Editor::CreateFrame (const char* title, iPerspective* perspective)
{
  csString cstitle = title;
  if (!title) cstitle = this->title;

  EditorFrame* frame = new EditorFrame (this, cstitle, perspective);
  frames.Push (frame);
  return frame;
}

void Editor::DeleteFrame (iEditorFrame* frame)
{
  frame->GetwxFrame ()->Close (false);
}

size_t Editor::GetFrameCount () const
{
  return frames.GetSize ();
}

iEditorFrame* Editor::GetFrame (size_t index) const
{
  return frames[index];
}

void Editor::Save (iDocumentNode* node) const
{
}

bool Editor::Load (iDocumentNode* node)
{
  return false;
}

void Editor::Init ()
{
  // Create a default perspective if none have been defined
  if (!perspectiveManager->GetPerspectiveCount ())
    perspectiveManager->CreatePerspective ("Default");

  for (size_t i = 0; i < frames.GetSize (); i++)
    frames[i]->Init ();

  ReportStatus ("Ready");
}

void Editor::Update ()
{
  componentManager->Update ();
}

void Editor::Close ()
{
  componentManager.Invalidate ();
  perspectiveManager.Invalidate ();
  operatorManager.Invalidate ();
  menuManager.Invalidate ();
  actionManager.Invalidate ();
}

//------------------------------------  EditorFrame  ------------------------------------

EditorFrame::EditorFrame (Editor* editor, const char* title, iPerspective* perspective)
  : wxFrame (nullptr, -1, wxString::FromAscii (title), wxDefaultPosition,
	     wxSize (1024, 768)),
    scfImplementationType (this), editor (editor)
{
  if (perspective)
    this->perspective = static_cast<Perspective*> (perspective);

  // If this is the first frame created, then add the status and menu bars to it
  if (!editor->frames.GetSize ())
  {
    // Setup the menu
    editor->menuManager->SetFrame (this);

    // Create the status bar
    editor->statusBar = new StatusBar (this);
    editor->statusBar->Show ();
    SetStatusBar (editor->statusBar);
    //PositionStatusBar ();
  }    

  // Make this frame visible
  Show (true);
}

EditorFrame::~EditorFrame ()
{
  // Remove this frame from the list maintained by the editor
  editor->frames.Delete (this);

  // If this was the last frame, then close the editor managers
  if (!editor->frames.GetSize ())
    editor->Close ();

  // TODO: transmit the ownership of the menu and status bar
}

void EditorFrame::Init ()
{
  // Setup the perspective of the window
  if (!perspective)
    perspective = editor->perspectiveManager->perspectives[0];
  perspective->CreateWindow (this);

  // Reset the window size
  //SetSize (wxSize (1024, 768));
  PositionStatusBar ();

  Show (true);
  Layout ();
}

wxFrame* EditorFrame::GetwxFrame ()
{
  return this;
}

bool EditorFrame::SetPerspective (iPerspective* perspective)
{
  if (this->perspective == perspective)
    return true;

  // Remove the previous window
  // TODO: Or keep a cache of the active perspectives instead
  GetSizer ()->Clear (true);
  SetSizer (nullptr);

  // Setup the perspective
  if (perspective)
  {
    this->perspective = static_cast<Perspective*> (perspective);
    this->perspective->CreateWindow (this);
  }
  else
    this->perspective = nullptr;

  Layout ();

  return true;
}

iPerspective* EditorFrame::GetPerspective () const
{
  return perspective;
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)
