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
#include "iutil/eventq.h"
#include "iutil/plugin.h"

#include "editor.h"
#include "layouts.h"

#include <wx/artprov.h>
#include <wx/button.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/toolbar.h>

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

BaseLayout::BaseLayout (iObjectRegistry* obj_reg, Editor* editor)
  : scfImplementationType (this), object_reg (obj_reg), editor (editor)
{
}

void BaseLayout::OnOperator (wxCommandEvent& event)
{
  printf ("HeaderLayout::OnOperator %d\n", event.GetId ());
  csRef<iOperator> op = operators.Get (event.GetId (), csRef<iOperator>());
  if (op)
    editor->GetOperatorManager ()->Execute (op);
}

void BaseLayout::OnMenu (wxCommandEvent& event)
{
  printf ("HeaderLayout::OnMenu %d\n", event.GetId ());
  
  csRef<iMenu> menu = menus.Get (event.GetId (), csRef<iMenu>());
  if (menu)
  {
    iContext* context = editor->GetContext ();
    if (menu->Poll (context))
    {
      wxMenu wx;
      csRef<iLayout> layout;
      layout.AttachNew
	(new MenuLayout (object_reg, editor, GetwxWindow (), &wx));
      menu->Draw (context, layout);
      GetwxWindow ()->PopupMenu (&wx);
    }
  }
}

iOperator* BaseLayout::GetOperator (const char* id)
{
  csRef<iOperator> op =
    editor->GetOperatorManager ()->CreateOperator (id);
  return op;
}

iMenu* BaseLayout::GetMenu (const char* pluginName)
{
  csRef<iMenu>* menu =
    registeredMenus.GetElementPointer (pluginName);
  if (menu) return *menu;

  csRef<iBase> base = iSCF::SCF->CreateInstance (pluginName);
  if (!base)
  {
    if (!iSCF::SCF->ClassRegistered  (pluginName))
      csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.editor.core.layout",
		"The editor menu %s is not registered",
		CS::Quote::Single (pluginName));

    else csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		   "crystalspace.editor.core.layout",
		   "Failed to instantiate editor menu %s",
		   CS::Quote::Single (pluginName));

    return nullptr;
  }

  csRef<iMenu> ref = scfQueryInterface<iMenu> (base);
  if (!ref)
  {
    csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.core.layout",
	      "The instanciation of the editor menu %s is not of type iMenu",
	      CS::Quote::Single (pluginName));
    return nullptr;
  }

  base->DecRef ();

  registeredMenus.PutUnique (pluginName, ref);
  return ref;
}

//----------------------------------------------------------------------

HeaderLayout::HeaderLayout (iObjectRegistry* obj_reg, Editor* editor,
			    wxWindow* parent)
  : BaseLayout (obj_reg, editor)
{
  box = new wxBoxSizer (wxVERTICAL);
  tb = new wxToolBar (parent, wxID_ANY);
  tb->Realize ();
  box->Add (tb, 1, wxEXPAND);
  parent->SetSizer (box, true);
}

HeaderLayout::~HeaderLayout ()
{
  //tb->Destroy ();
  box->Clear (true);
}

iOperator* HeaderLayout::AppendOperator
(const char* id, const char* label, const char* icon) 
{ 
  wxString l (label, wxConvUTF8);
  wxToolBarToolBase* item =
    tb->AddTool (wxID_ANY, l, wxArtProvider::GetBitmap (wxART_ERROR));
  tb->Realize ();
  csRef<iOperator> op = GetOperator (id);
  operators.PutUnique (item->GetId (), op);
  tb->Connect (item->GetId (), wxEVT_COMMAND_MENU_SELECTED,
	       wxCommandEventHandler (HeaderLayout::OnOperator),
	       0, this);

  return op; 
}

iMenu* HeaderLayout::AppendMenu (const char* id, const char* label)
{
  wxString l (label, wxConvUTF8);
  wxButton* entry =
    new wxButton (tb, wxID_ANY, l, wxDefaultPosition, wxDefaultSize,
		  wxNO_BORDER /*| wxBU_EXACTFIT*/);
  tb->AddControl (entry);
  tb->Realize ();

  csRef<iMenu> menu = GetMenu (id);

  menus.PutUnique (entry->GetId (), menu);
  tb->Connect (entry->GetId (), wxEVT_COMMAND_BUTTON_CLICKED,
	       wxCommandEventHandler (HeaderLayout::OnMenu), 0, this);

  return menu;
}

void HeaderLayout::AppendLabel (const char* label) {}
void HeaderLayout::AppendSeparator () 
{
  tb->AddSeparator ();
  tb->Realize ();
}
iLayout* HeaderLayout::Row () { return nullptr; }
iLayout* HeaderLayout::Column () { return nullptr; }


wxWindow* HeaderLayout::GetwxWindow () 
{
  return tb;
}

//----------------------------------------------------------------------

MenuLayout::MenuLayout (iObjectRegistry* obj_reg, Editor* editor,
			wxWindow* parent, wxMenu* menu)
  : BaseLayout (obj_reg, editor),  parent (parent), menu (menu)
{
}

MenuLayout::~MenuLayout ()
{
}

iOperator* MenuLayout::AppendOperator
(const char* id, const char* label, const char* icon) 
{ 
  wxString l (label, wxConvUTF8);
  wxMenuItem* item = menu->Append (wxID_ANY, l);
  csRef<iOperator> op = GetOperator (id);
  parent->Connect (item->GetId (), wxEVT_COMMAND_MENU_SELECTED,
		   wxCommandEventHandler (MenuLayout::OnOperator),
		   0, this);
  operators.PutUnique (item->GetId (), op);

  return op; 
}

iMenu* MenuLayout::AppendMenu (const char* id, const char* label)
{
  return nullptr;
}

void MenuLayout::AppendLabel (const char* label) {}
void MenuLayout::AppendSeparator () 
{
  menu->AppendSeparator ();
}
iLayout* MenuLayout::Row () { return nullptr; }
iLayout* MenuLayout::Column () { return nullptr; }

wxWindow* MenuLayout::GetwxWindow () 
{
  return parent;
}

//----------------------------------------------------------------------

PanelLayout::PanelLayout
(iObjectRegistry* obj_reg, Editor* editor, wxWindow* parent)
  : BaseLayout (obj_reg, editor),  parent (parent)
{
  paneSz = new wxBoxSizer (wxVERTICAL);
  parent->SetSizer (paneSz);
}

PanelLayout::~PanelLayout ()
{
  paneSz->Clear (true);
}

iOperator* PanelLayout::AppendOperator
(const char* id, const char* label, const char* icon) 
{ 
  wxString l (label, wxConvUTF8);
  wxButton* item =
    new wxButton (parent, wxID_ANY, l, wxDefaultPosition,
		  wxDefaultSize/*, wxNO_BORDER | wxBU_EXACTFIT*/);
  csRef<iOperator> op = GetOperator (id);
  parent->Connect (item->GetId (), wxEVT_COMMAND_BUTTON_CLICKED,
		   wxCommandEventHandler (MenuLayout::OnOperator),
		   0, this);
  operators.PutUnique (item->GetId (), op);
  
  paneSz->Add (item, 0, wxEXPAND, 0);
  item->Show ();
  
  return op; 
}

iMenu* PanelLayout::AppendMenu (const char* id, const char* label)
{
  return nullptr;
}

void PanelLayout::AppendLabel (const char* label) 
{
  wxString l (label, wxConvUTF8);
  paneSz->Add (new wxStaticText (parent, wxID_ANY, l), 1, wxEXPAND, 0);
}

void PanelLayout::AppendSeparator () {}
iLayout* PanelLayout::Row () { return nullptr; }
iLayout* PanelLayout::Column () { return nullptr; }

wxWindow* PanelLayout::GetwxWindow () 
{
  return parent;
}

//----------------------------------------------------------------------

BEGIN_EVENT_TABLE (CollapsiblePane, wxCollapsiblePane)
  EVT_SIZE (CollapsiblePane::OnSize)
END_EVENT_TABLE ()

CollapsiblePane::CollapsiblePane
 (iObjectRegistry* obj_reg, wxWindow* parent, const char* label)
  : wxCollapsiblePane (parent, wxID_ANY, wxString (label, wxConvUTF8),
		       wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE),
  object_reg (obj_reg)
{
}

CollapsiblePane::~CollapsiblePane ()
{
}

void CollapsiblePane::OnSize (wxSizeEvent& event)
{
  Layout ();
  event.Skip ();
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)
