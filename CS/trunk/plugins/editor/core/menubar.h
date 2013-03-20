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

#ifndef __MENUBAR_H__
#define __MENUBAR_H__

#include "csutil/refarr.h"
#include "csutil/weakrefarr.h"
#include "csutil/hash.h"
#include "ieditor/menu.h"
#include "ieditor/operator.h"

#include <wx/aui/aui.h>
#include <wx/menu.h>

#include <map>

using namespace CS::EditorApp;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{
  
class Editor;
class MenuManager;

class MenuItem
  : public wxEvtHandler, public scfImplementation1<MenuItem, iMenuItem>
{
public:
  MenuItem (MenuManager* menuManager, wxMenu* menu,
	    wxMenuItem* item, const char* eventName);
  virtual ~MenuItem ();
  
  virtual wxMenuItem* GetwxMenuItem () const;
  virtual const csEventID GetEventID () const;
  
private:
  void OnToggle (wxCommandEvent& event);
  MenuManager* menuManager;
  wxMenu* menu;
  wxMenuItem* item;
  csEventID eventID;
};

//---------------------------------------------------------------

class SeparatorMenuItem
  : public scfImplementation1<SeparatorMenuItem, iMenuItem>
{
public:
  SeparatorMenuItem (wxMenu* menu, wxMenuItem* item);
  virtual ~SeparatorMenuItem ();
  
  virtual wxMenuItem* GetwxMenuItem () const;
  virtual const csEventID GetEventID () const;

private:
  wxMenu* menu;
  wxMenuItem* item;
};

//---------------------------------------------------------------

class SubMenu
  : public wxEvtHandler, public scfImplementation1<SubMenu, iSubMenu>
{
public:
  SubMenu (MenuManager* menuManager, wxMenu* menu, const wxString& title);
  virtual ~SubMenu ();
  
  virtual wxMenu* GetwxMenu () const;
  
  virtual csPtr<iMenuItem> AppendItem (const char* item, const char* eventName);
  virtual csPtr<iMenuItem> AppendSeparator ();
  virtual csPtr<iSubMenu> AppendSubMenu (const char* item);

private:
  MenuManager* menuManager;
  wxMenu* menu;
  wxString title;

  friend class MenuManager;
};

//---------------------------------------------------------------

class MenuManager
  : public wxEvtHandler, public scfImplementation1<MenuManager, iMenuManager>
{
public:
  MenuManager (Editor* editor);
  virtual ~MenuManager ();

  void SetFrame (wxFrame* frame);

  //-- iMenuManager
  virtual wxMenuBar* GetwxMenuBar () const;
  virtual csRef<iSubMenu> GetSubMenu (const char* item);

private:
  Editor* editor;
  wxMenuBar* menuBar;
  csWeakRefArray<SubMenu> subMenus;

  friend class MenuOperatorItem;
  friend class MenuItem;
};

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif
