/*
    Copyright (C) 2011-2012 by Jorrit Tyberghein, Jelle Hellemans, Christian Van Brussel

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

#define CS_IMPLEMENT_PLATFORM_APPLICATION
/* This is needed due the WX headers using free() inline, but the opposing
 * malloc() is in the WX libs. */
#define CS_NO_MALLOC_OVERRIDE

#include "cssysdef.h"

#include "cstool/initapp.h"
#include "cstool/wxappargconvert.h"
#include "ieditor/editor.h"
#include "ieditor/perspective.h"
#include "ieditor/space.h"
#include "iutil/event.h"
#include "iutil/eventq.h"
#include "iutil/objreg.h"
#include "context.h"

#include "csutil/custom_new_disable.h"
#include <wx/wx.h>
#include "csutil/custom_new_enable.h"

#include "cstool/wxappargconvert.h"

CS_IMPLEMENT_APPLICATION

#if defined (CS_PLATFORM_WIN32)

#ifndef SW_SHOWNORMAL
#define SW_SHOWNORMAL 1
#endif

/*
  WX provides WinMain(), but not main(), which is required for console apps.
 */
int main (int argc, const char* const argv[])
{
  return WinMain (GetModuleHandle (0), 0, GetCommandLineA (), SW_SHOWNORMAL);
}

#endif // defined(CS_PLATFORM_WIN32)

using namespace CS::EditorApp;

// Define a new application type
class EditorApplication : public wxApp
{
  bool ReportError (const char* description, ...)
  {
    va_list arg;
    va_start (arg, description);
    wxLogError (wxString::FromUTF8 (csString ().FormatV (description, arg)));
    va_end (arg);
    return false;
  }

  bool ReportWarning (const char* description, ...)
  {
    va_list arg;
    va_start (arg, description);
    wxLogWarning (wxString::FromUTF8 (csString ().FormatV (description, arg)));
    va_end (arg);
    return false;
  }

  bool ReportInfo (const char* description, ...)
  {
    va_list arg;
    va_start (arg, description);
    wxLogMessage (wxString::FromUTF8 (csString ().FormatV (description, arg)));
    va_end (arg);
    return false;
  }

public:
  iObjectRegistry* object_reg;
  csRef<iEditorManager> editorManager;

  virtual bool OnInit (void);
  virtual int OnExit (void);
};

IMPLEMENT_APP (EditorApplication);

/*---------------------------------------------------------------------*
 * Main function
 *---------------------------------------------------------------------*/

bool EditorApplication::OnInit (void)
{
  wxInitAllImageHandlers ();

  // Create the Crystal Space environment
  CS::WX::AppArgConvert args (argc, argv);
  object_reg = csInitializer::CreateEnvironment (args.csArgc(), args.csArgv());

  // Load the iEditorManager plugin
  csRef<iPluginManager> pluginManager = 
    csQueryRegistry<iPluginManager> (object_reg);
  if (!pluginManager) return ReportError ("Could not load the plugin manager!");

  editorManager = csLoadPlugin<iEditorManager>
    (pluginManager, "crystalspace.editor.core");
  if (!editorManager) return ReportError ("Could not load the cseditor plugin!");

  // Start the Crystal Space engine
  if (!editorManager->StartEngine ())
    return ReportError ("Could not start the Crystal Space engine!");

  // Create the context of the editor
  csRef<iContext> context;
  context.AttachNew (new Context (object_reg));

  // Create an instance of the editor
  iEditor* editor = editorManager->CreateEditor ("cseditor", "Crystal Space Editor", context);

  // Load the specific plugins for the Crystal Space editor
  iComponentManager* componentManager = editor->GetComponentManager ();

  if (!componentManager->RegisterComponent ("crystalspace.editor.component.logger")) return false;
  if (!componentManager->RegisterComponent ("crystalspace.editor.component.engine")) return false;
  if (!componentManager->RegisterComponent ("crystalspace.editor.component.undoredo")) return false;
  if (!componentManager->RegisterComponent ("crystalspace.editor.component.perspectives")) return false;
  if (!componentManager->RegisterComponent ("crystalspace.editor.component.maploader")) return false;
  if (!componentManager->RegisterComponent ("crystalspace.editor.component.scenemanager")) return false;
  if (!componentManager->RegisterComponent ("crystalspace.editor.component.2dinfo")) return false;
  if (!componentManager->RegisterComponent ("crystalspace.editor.component.exit")) return false;

  if (!componentManager->RegisterSpace ("crystalspace.editor.space.3dview")) return false;
  if (!componentManager->RegisterSpace ("crystalspace.editor.space.scenetree")) return false;
  if (!componentManager->RegisterSpace ("crystalspace.editor.space.partedit")) return false; 
  if (!componentManager->RegisterSpace ("crystalspace.editor.space.textlog")) return false;
  if (!componentManager->RegisterSpace ("crystalspace.editor.space.properties")) return false;

  if (!componentManager->RegisterPanel ("crystalspace.editor.panel.camera")) return false;
  if (!componentManager->RegisterHeader ("crystalspace.editor.header.3dheader")) return false;

  // Create a 'Default' pespective
  iPerspectiveManager* perspectiveManager = editor->GetPerspectiveManager ();

  iPerspective* perspective = perspectiveManager->CreatePerspective ("Default");
  iPerspectiveWindow* window = perspective->GetRootWindow ();
  window->SetSplitMode (SPLIT_VERTICAL);
  window->SetSplitPosition (220);

  iPerspectiveWindow* window1 = window->GetChild1 ();
  window1->SetSpace ("crystalspace.editor.space.scenetree");

  iPerspectiveWindow* window2 = window->GetChild2 ();
  window2->SetSplitMode (SPLIT_VERTICAL);
  window2->SetSplitPosition (550);

  window2->GetChild1 ()->SetSpace ("crystalspace.editor.space.3dview");
  window2->GetChild2 ()->SetSpace ("crystalspace.editor.space.partedit");

  // Create a 'Terrain' pespective
  perspective = perspectiveManager->CreatePerspective ("Terrain");
  window = perspective->GetRootWindow ();
  //window->SetSpace ("crystalspace.editor.space.3dview");
  window->SetSpace ("crystalspace.editor.space.scenetree");

  // Start the application
  if (!editorManager->StartApplication ()) return false;

  return true;
}

int EditorApplication::OnExit ()
{
  // Send the general Crystal Space 'quit' event
  csRef<iEventQueue> queue (csQueryRegistry<iEventQueue> (object_reg));
  if (queue) queue->GetEventOutlet ()->Broadcast (csevQuit (object_reg));

  // Invalidate the editor manager component since it will be destroyed by
  // csInitializer::DestroyApplication ()
  editorManager.Invalidate ();

  // Close the Crystal Space environment
  csInitializer::DestroyApplication (object_reg);

  return 0;
}
