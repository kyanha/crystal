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
#include "ieditor/context.h"
#include "iutil/event.h"
#include "iutil/eventq.h"

#include "menubar.h"
#include "editor.h"

#include <wx/menu.h>

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

MenuItem::MenuItem (MenuManager* menuManager, wxMenu* menu, wxMenuItem* item, const char* eventName)
  : scfImplementationType (this), menuManager (menuManager), menu (menu), item (item)
{
  menuManager->GetwxMenuBar ()->GetParent ()->Connect
    (item->GetId (), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (MenuItem::OnToggle), 0, this);

  csRef<iEventNameRegistry> registry = csQueryRegistry<iEventNameRegistry>
    (menuManager->editor->GetContext ()->GetObjectRegistry ());
  csString name = "crystalspace.editor.event.menu.";
  name += eventName;
  eventID = registry->GetID (name);
}

MenuItem::~MenuItem ()
{
  menuManager->GetwxMenuBar ()->GetParent ()->Disconnect
    (item->GetId (), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (MenuItem::OnToggle), 0, this); 
  menu->Remove (item);
  delete item;
}

void MenuItem::OnToggle (wxCommandEvent& wxevent)
{
  iEventQueue* queue = menuManager->editor->GetContext ()->GetEventQueue ();
  csRef<iEvent> event = queue->CreateEvent (eventID);
  queue->Post (event);
  queue->Process ();
}

wxMenuItem* MenuItem::GetwxMenuItem () const
{
  return item;
}

const csEventID MenuItem::GetEventID () const
{
  return eventID;
}

//---------------------------------------------------------------

SeparatorMenuItem::SeparatorMenuItem (wxMenu* menu, wxMenuItem* item)
  : scfImplementationType (this), menu (menu), item (item)
{
}

SeparatorMenuItem::~SeparatorMenuItem ()
{
  menu->Remove (item);
  delete item;
}

wxMenuItem* SeparatorMenuItem::GetwxMenuItem () const
{
  return item;
}

const csEventID SeparatorMenuItem::GetEventID () const
{
  return (csEventID) 0;
}

//---------------------------------------------------------------

SubMenu::SubMenu (MenuManager* menuManager, wxMenu* menu, const wxString& title)
  : scfImplementationType (this), menuManager (menuManager), menu (menu), title (title)
{
}

SubMenu::~SubMenu ()
{
  int pos = menuManager->GetwxMenuBar ()->FindMenu (title);
  if (pos != wxNOT_FOUND) menuManager->GetwxMenuBar ()->Remove (pos);
  delete menu;
}

wxMenu* SubMenu::GetwxMenu () const
{
  return menu;
}

csPtr<iMenuItem> SubMenu::AppendItem (const char* item, const char* eventName)
{
  wxString str (item, wxConvUTF8);
  wxMenuItem* i = menu->Append (wxID_ANY, str);

  csRef<iMenuItem> ref;
  ref.AttachNew (new MenuItem (menuManager, menu, i, eventName));

  return csPtr<iMenuItem> (ref);
}

csPtr<iMenuItem> SubMenu::AppendSeparator ()
{
  wxMenuItem* i = menu->AppendSeparator ();

  csRef<iMenuItem> ref;
  ref.AttachNew (new SeparatorMenuItem (menu, i));

  return csPtr<iMenuItem> (ref);
}

csPtr<iSubMenu> SubMenu::AppendSubMenu (const char* item)
{
  wxMenu* m = new wxMenu ();
  wxString str (item, wxConvUTF8);
  menu->AppendSubMenu (m, str);

  csRef<iSubMenu> ref;
  ref.AttachNew (new SubMenu (menuManager, m, str));

  return csPtr<iSubMenu> (ref);
}

//---------------------------------------------------------------

MenuManager::MenuManager (Editor* editor)
  : scfImplementationType (this), editor (editor)
{
  menuBar = new wxMenuBar ();
}

MenuManager::~MenuManager ()
{
  // The 'menuBar' doesn't need to be deleted since this is made
  // automatically by wxWidgets
}

void MenuManager::SetFrame (wxFrame* frame)
{
  frame->SetMenuBar (menuBar);
  menuBar->Reparent (frame);
}

wxMenuBar* MenuManager::GetwxMenuBar () const
{
  return menuBar;
}

csRef<iSubMenu> MenuManager::GetSubMenu (const char* item)
{
  wxString title (item, wxConvUTF8);

  // Check if this submenu already exists
  subMenus.Compact ();
  for (size_t i = 0; i < subMenus.GetSize (); i++)
    if (subMenus[i]->title.CmpNoCase (title) == 0)
      return csRef<iSubMenu> (subMenus[i]);

  // If not found then create a new submenu
  wxMenu* menu = new wxMenu ();
  menuBar->Append (menu, title);

  csRef<SubMenu> ref;
  ref.AttachNew (new SubMenu (this, menu, title));
  subMenus.Push (ref);

  return ref;
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)
