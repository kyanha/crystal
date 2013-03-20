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

#ifndef __COMPONENTS_EXIT_H__
#define __COMPONENTS_EXIT_H__

#include "csutil/csbaseeventh.h"
#include "csutil/scf_implementation.h"
#include "ieditor/editor.h"
#include "ieditor/menu.h"
#include "ieditor/perspective.h"
#include "ieditor/space.h"
#include "iutil/comp.h"
#include "ivaria/reporter.h"

#include <wx/frame.h>

using namespace CS::EditorApp;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

class Perspectives
  : public wxEvtHandler,
    public scfImplementation1<Perspectives, iEditorComponent>,
    public csBaseEventHandler
{
public:
  Perspectives (iBase* parent);
  virtual ~Perspectives();

  //-- iEditorComponent
  virtual bool Initialize (iEditor* editor);
  virtual void Update ();
  virtual void Save (iDocumentNode* node) const;
  virtual bool Load (iDocumentNode* node);
  
  //-- iEventHandler
  virtual bool HandleEvent (iEvent &event);

private:
  void AddPerspective (iPerspective* perspective);

private:
  iEditor* editor;

  csRef<iSubMenu> viewMenu;
  csRef<iMenuItem> createItem;
  csRef<iMenuItem> separator;
  csRefArray<iMenuItem> perspectives;
  csEventID eventUpdatePerspective;
};

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif // __COMPONENTS_EXIT_H__
