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

#ifndef __COMPONENTS_UNDOREDO_H__
#define __COMPONENTS_UNDOREDO_H__

#include "csutil/csbaseeventh.h"
#include "csutil/scf_implementation.h"
#include "ieditor/editor.h"
#include "ieditor/menu.h"
#include "ieditor/space.h"
#include "iutil/comp.h"
#include "ivaria/reporter.h"

#include <wx/event.h>
#include <wx/menu.h>

using namespace CS::EditorApp;

CS_PLUGIN_NAMESPACE_BEGIN(CSEditor)
{

class UndoRedo
  : public wxEvtHandler,
    public scfImplementation1<UndoRedo, iEditorComponent>,
    public csBaseEventHandler
{
public:
  UndoRedo (iBase* parent);
  virtual ~UndoRedo();

  //-- iEditorComponent
  virtual bool Initialize (iEditor* editor);
  virtual void Update ();
  virtual void Save (iDocumentNode* node) const;
  virtual bool Load (iDocumentNode* node);
  
  //-- iEventHandler
  virtual bool HandleEvent (iEvent &event);

private:
  void UpdateMenus ();

private:
  iEditor* editor;

  csRef<iSubMenu> editMenu;
  csRef<iMenuItem> undoItem;
  csRef<iMenuItem> redoItem;
};

}
CS_PLUGIN_NAMESPACE_END(CSEditor)

#endif // __COMPONENTS_UNDOREDO_H__
