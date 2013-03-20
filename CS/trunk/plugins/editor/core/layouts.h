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

#ifndef __CSEDITOR_LAYOUTS_H__
#define __CSEDITOR_LAYOUTS_H__

#include "csutil/hash.h"
#include "csutil/refarr.h"
#include "csutil/scf_implementation.h"
#include "ieditor/context.h"
#include "ieditor/layout.h"
#include "ieditor/menu.h"
#include "ieditor/operator.h"

#include <wx/event.h>
#include <wx/collpane.h>

class wxWindow;
class wxCommandEvent;
class wxMenu;
class wxToolBar;
class wxBoxSizer;
class wxSizer;

using namespace CS::EditorApp;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

class Editor;

struct BaseLayout
  : public wxEvtHandler, public scfImplementation1<BaseLayout, iLayout>
{
  BaseLayout (iObjectRegistry* obj_reg, Editor* editor);
 
  void OnOperator (wxCommandEvent& event);
  
  void OnMenu (wxCommandEvent& event);
  
  iOperator* GetOperator (const char* id);
  iMenu* GetMenu (const char* id);
  virtual wxWindow* GetwxWindow () = 0;

protected:
  iObjectRegistry* object_reg;
  Editor* editor;
  csHash<csRef<iOperator>, int> operators;
  csHash<csRef<iMenu>, int> menus;

  csHash<csRef<iMenu>, csString> registeredMenus;
};

//----------------------------------------------------------------------

struct HeaderLayout : public BaseLayout
{
  HeaderLayout (iObjectRegistry* obj_reg, Editor* editor, wxWindow* parent);
  ~HeaderLayout ();

  virtual iOperator* AppendOperator
    (const char* id, const char* label, const char* icon);
  virtual iMenu* AppendMenu (const char* id, const char* label);
  virtual void AppendLabel (const char* label);
  virtual void AppendSeparator ();

  virtual iLayout* Row ();
  virtual iLayout* Column ();
  
  virtual wxWindow* GetwxWindow ();
  
private:
  wxToolBar* tb;
  wxBoxSizer* box;
};

//----------------------------------------------------------------------

struct MenuLayout : public BaseLayout
{
  MenuLayout (iObjectRegistry* obj_reg, Editor* editor, wxWindow*, wxMenu*);
  ~MenuLayout ();

  virtual iOperator* AppendOperator
    (const char* id, const char* label, const char* icon);
  virtual iMenu* AppendMenu (const char* id, const char* label);
  virtual void AppendLabel (const char* label);
  virtual void AppendSeparator ();

  virtual iLayout* Row ();
  virtual iLayout* Column ();

  virtual wxWindow* GetwxWindow ();

private:
  wxWindow* parent;
  wxMenu* menu;
};

//----------------------------------------------------------------------

struct PanelLayout : public BaseLayout
{
  PanelLayout (iObjectRegistry* obj_reg, Editor* editor, wxWindow*);
  ~PanelLayout ();

  virtual iOperator* AppendOperator
    (const char* id, const char* label, const char* icon);
  virtual iMenu* AppendMenu (const char* id, const char* label);
  virtual void AppendLabel (const char* label);
  virtual void AppendSeparator ();

  virtual iLayout* Row ();
  virtual iLayout* Column ();
  
  virtual wxWindow* GetwxWindow ();
  
private:
  wxWindow* parent;
  wxSizer* paneSz;
};

//----------------------------------------------------------------------

class CollapsiblePane : public wxCollapsiblePane
{
public:
  CollapsiblePane (iObjectRegistry*, wxWindow*, const char*);
  virtual ~CollapsiblePane ();

  void SetLayout (iLayout* l) { layout = l; }

private:
  void OnSize (wxSizeEvent& ev);

  DECLARE_EVENT_TABLE ();

private:
  iObjectRegistry* object_reg;
  csRef<iLayout> layout;
};

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif
