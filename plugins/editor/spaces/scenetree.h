/*
    Copyright (C) 2012 by Christian Van Brussel
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
#ifndef __SPACE_SCENETREE_H__
#define __SPACE_SCENETREE_H__

#include "csutil/csbaseeventh.h"
#include "ieditor/context.h"
#include "ieditor/editor.h"
#include "ieditor/space.h"

#include <csutil/scf_implementation.h>
#include <csutil/hash.h>
#include <iutil/comp.h>
#include <iengine/scenenode.h>

#include <wx/event.h>
#include <wx/panel.h>
#include <wx/treectrl.h>
#include <wx/imaglist.h>

using namespace CS::EditorApp;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

class SceneTreeCtrl;

class SceneTree : public wxPanel, public scfImplementation1<SceneTree, iSpace>
{
public:
  SceneTree (iBase* parent);
  virtual ~SceneTree ();

  // iSpace
  virtual bool Initialize (iObjectRegistry* obj_reg, iEditor* editor,
			   iSpaceFactory* factory, wxWindow* parent);
  virtual iSpaceFactory* GetFactory () const { return factory; }
  virtual wxWindow* GetwxWindow ();
  virtual void SetEnabled (bool enabled) { this->enabled = enabled; }
  virtual bool GetEnabled () const { return enabled; }
  virtual void Update ();

private:
  void OnSize (wxSizeEvent& event);
  
private:
  iObjectRegistry* object_reg;
  csRef<iSpaceFactory> factory;

  SceneTreeCtrl* treectrl;
  wxBoxSizer* sizer;
  bool enabled;

  DECLARE_EVENT_TABLE ();
};

class SceneTreeItemData : public wxTreeItemData
{
public:
  SceneTreeItemData (iObject* obj) : object (obj) {}
  SceneTreeItemData (SceneTreeItemData* data) : object (data->object) {}

  iObject* GetObject () { return object; }
  
private:
  csRef<iObject> object;
};

enum ObjectType
{
  TEXTURE = 0,
  MATERIAL,
  SHADER,
  MESH_FACTORY,
  LIGHT_FACTORY,
  CAMERA_POSITION,
  SECTOR,
  PORTAL,
  MESH,
  LIGHT,
  TRIGGER,
  SEQUENCE,
  UNKNOWN,
  TYPE_COUNT
};

class SceneTreeCtrl : public wxTreeCtrl, public csBaseEventHandler
{
public:
  SceneTreeCtrl (iObjectRegistry* obj_reg, iEditor* editor, wxWindow *parent,
             const wxWindowID id, const wxPoint& pos, const wxSize& size);
  virtual ~SceneTreeCtrl () { };

  //-- iEventHandler
  bool HandleEvent (iEvent &event);

private:
  void UpdateTree ();
  void AppendObject (iObject* object, ObjectType type);

  void OnItemActivated (wxTreeEvent& event);
  void OnSelChanged (wxTreeEvent& event);

  void OnSetSelectedObjects ();
  void OnAddSelectedObject (iEvent& event);
  void OnDeleteSelectedObject (iEvent& event);
  void OnClearSelectedObjects ();

  void OnEnterWindow (wxMouseEvent& event);

private:
  iEditor* editor;

  wxImageList* imageList;

  int rootIconIdx;
  csArray<wxTreeItemId> rootIDs;
  csHash<csString, int> categories;
  csHash<wxTreeItemId, iObject*> objects;

  csEventID eventSetCollection;
  csEventID eventSetSelectedObjects;
  csEventID eventAddSelectedObject;
  csEventID eventDeleteSelectedObject;
  csEventID eventClearSelectedObjects;

  bool selecting;

  DECLARE_EVENT_TABLE ();
};

// Menu and control ids
enum
{
  SceneTree_Ctrl = wxID_HIGHEST
};

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif // __SPACE_SCENETREE_H__
