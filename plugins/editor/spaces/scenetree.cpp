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
#include <cssysdef.h>
#include "csutil/scf.h"

#include <iengine/campos.h>
#include <iengine/collection.h>
#include <iengine/light.h>
#include <iengine/material.h>
#include <iengine/mesh.h>
#include <iengine/portalcontainer.h>
#include <iengine/sector.h>
#include <iengine/texture.h>
#include "iutil/event.h"
#include "iutil/eventh.h"
#include <iutil/object.h>
#include <iutil/objreg.h>
#include <iutil/plugin.h>
#include <ivaria/engseq.h>
#include <ivideo/shader/shader.h>

#include <wx/wx.h>
#include <wx/treectrl.h>

#include "scenetree.h"

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

#include "data/editor/images/sceneIcon.xpm"
  
SCF_IMPLEMENT_FACTORY (SceneTree)

BEGIN_EVENT_TABLE (SceneTree, wxPanel)
  EVT_SIZE (SceneTree::OnSize)
END_EVENT_TABLE ()

BEGIN_EVENT_TABLE (SceneTreeCtrl, wxTreeCtrl)
  EVT_ENTER_WINDOW (SceneTreeCtrl::OnEnterWindow)
  EVT_TREE_ITEM_ACTIVATED (SceneTree_Ctrl, SceneTreeCtrl::OnItemActivated)
  EVT_TREE_SEL_CHANGED (SceneTree_Ctrl, SceneTreeCtrl::OnSelChanged)
END_EVENT_TABLE ()

// ----------------------------------------------------------------------------

SceneTree::SceneTree (iBase* parent)
  : scfImplementationType (this, parent), treectrl (nullptr),  enabled (true)
{
}

SceneTree::~SceneTree ()
{
}

bool SceneTree::Initialize (iObjectRegistry* obj_reg, iEditor* editor,
			    iSpaceFactory* factory, wxWindow* parent)
{
  object_reg = obj_reg;
  this->factory = factory;
  
  // Create this panel
  Create (parent, -1, wxDefaultPosition, wxDefaultSize);

  // Create the tree control
  treectrl = new SceneTreeCtrl (object_reg, editor, this, SceneTree_Ctrl,
				wxDefaultPosition, wxDefaultSize);
  
  return true;
}

wxWindow* SceneTree::GetwxWindow ()
{
  return this;
}

void SceneTree::Update ()
{
}

void SceneTree::OnSize (wxSizeEvent& event)
{
  // Resize the tree control
  if (treectrl) {
    treectrl->SetSize (event.GetSize ());
  }

  event.Skip ();
}

// ----------------------------------------------------------------------------

SceneTreeCtrl::SceneTreeCtrl (iObjectRegistry* object_reg, iEditor* editor,
			      wxWindow *parent, const wxWindowID id,
			      const wxPoint& pos, const wxSize& size)
  : wxTreeCtrl (parent, id, pos, size,
		wxTR_MULTIPLE | wxTR_EXTENDED | wxTR_FULL_ROW_HIGHLIGHT// | wxTR_EDIT_LABELS
		| wxTR_HIDE_ROOT | wxTR_NO_LINES),
    editor (editor), selecting (false)
{
  imageList = new wxImageList (16, 16);
  AssignImageList (imageList);

  wxBitmap sceneBmp (sceneIcon_xpm);
  rootIconIdx = imageList->Add (sceneBmp);

  // Register the event handler
  iEventNameRegistry* registry =
    csEventNameRegistry::GetRegistry (object_reg);
  eventSetCollection =
    registry->GetID ("crystalspace.editor.context.fileloader.setcollection");
  eventSetSelectedObjects =
    registry->GetID ("crystalspace.editor.context.selection.setselectedobjects");
  eventAddSelectedObject =
    registry->GetID ("crystalspace.editor.context.selection.addselectedobject");
  eventDeleteSelectedObject =
    registry->GetID ("crystalspace.editor.context.selection.deleteselectedobject");
  eventClearSelectedObjects =
    registry->GetID ("crystalspace.editor.context.selection.clearselectedobjects");

  csEventID events[] = {
    eventSetCollection,
    eventSetSelectedObjects,
    eventAddSelectedObject,
    eventDeleteSelectedObject,
    eventClearSelectedObjects,
    CS_EVENTLIST_END
  };

  RegisterQueue (editor->GetContext ()->GetEventQueue (), events);

  // Initialize the categories
  categories.Put (CAMERA_POSITION, "Camera positions");
  categories.Put (LIGHT, "Lights");
  categories.Put (LIGHT_FACTORY, "Light factories");
  categories.Put (MATERIAL, "Materials");
  categories.Put (MESH, "Meshes");
  categories.Put (MESH_FACTORY, "Mesh factories");
  categories.Put (PORTAL, "Portals");
  categories.Put (SECTOR, "Sectors");
  categories.Put (SEQUENCE, "Sequences");
  categories.Put (SHADER, "Shaders");
  categories.Put (TRIGGER, "Triggers");
  categories.Put (TEXTURE, "Textures");
  categories.Put (UNKNOWN, "Unknown type");

  UpdateTree ();
}

bool SceneTreeCtrl::HandleEvent (iEvent &event)
{
  if (event.Name == eventSetCollection)
    UpdateTree ();

  else if (!selecting)
  {
    if (event.Name == eventSetSelectedObjects)
      OnSetSelectedObjects ();
    else if (event.Name == eventAddSelectedObject)
      OnAddSelectedObject (event);
    else if (event.Name == eventDeleteSelectedObject)
      OnDeleteSelectedObject (event);
    else if (event.Name == eventClearSelectedObjects)
      OnClearSelectedObjects ();
  }

  return false;
}

void SceneTreeCtrl::UpdateTree ()
{
  // Clear the tree and rebuild it
  DeleteAllItems ();
  AddRoot (wxString ("", *wxConvCurrent));

  // Re-initialize the object categories
  rootIDs.Truncate (0);
  rootIDs.SetSize (TYPE_COUNT, wxTreeItemId ());

  // Add all objects in the context's collection to the tree
  csRef<iContextFileLoader> fileLoaderContext =
    scfQueryInterface<iContextFileLoader> (editor->GetContext ());
  iCollection* collection = fileLoaderContext->GetCollection ();
  if (!collection)
  {
    InsertItem (GetRootItem (), 0,
		wxString::FromUTF8 ("No objects available"),
		-1, -1, nullptr); 
    return;
  }

  csRef<iObjectIterator> it = collection->QueryObject ()->GetIterator ();
  if (!it->HasNext ())
  {
    InsertItem (GetRootItem (), 0,
		wxString::FromUTF8 ("No objects available"),
		-1, -1, nullptr); 
    return;
  }

  for ( ; it->HasNext (); )
  {
    iObject* object = it->Next ();
   
    // Search the type of the object
    csRef<iMeshWrapper> mesh =
      scfQueryInterface<iMeshWrapper> (object);
    if (mesh)
    {
      csRef<iPortalContainer> portalContainer =
	scfQueryInterface<iPortalContainer> (mesh->GetMeshObject ());
      if (portalContainer)
      {
	AppendObject (object, PORTAL);
	continue;
      }
    
      AppendObject (object, MESH);
      continue;
    }
    
    csRef<iMeshFactoryWrapper> meshFactory =
      scfQueryInterface<iMeshFactoryWrapper> (object);
    if (meshFactory)
    {
      AppendObject (object, MESH_FACTORY);
      continue;
    }
    
    csRef<iTextureWrapper> texture =
      scfQueryInterface<iTextureWrapper> (object);
    if (texture)
    {
      AppendObject (object, TEXTURE);
      continue;
    }
    
    csRef<iMaterialWrapper> material =
      scfQueryInterface<iMaterialWrapper> (object);
    if (material)
    {
      AppendObject (object, MATERIAL);
      continue;
    }
    
    csRef<iShader> shader =
      scfQueryInterface<iShader> (object);
    if (shader)
    {
      AppendObject (object, SHADER);
      continue;
    }
    
    csRef<iLightFactory> lightFactory =
      scfQueryInterface<iLightFactory> (object);
    if (lightFactory)
    {
      AppendObject (object, LIGHT_FACTORY);
      continue;
    }
    
    csRef<iLight> light =
      scfQueryInterface<iLight> (object);
    if (light)
    {
      AppendObject (object, LIGHT);
      continue;
    }
    
    csRef<iSector> sector =
      scfQueryInterface<iSector> (object);
    if (sector)
    {
      AppendObject (object, SECTOR);
      continue;
    }
    
    csRef<iSequenceWrapper> sequenceWrapper =
      scfQueryInterface<iSequenceWrapper> (object);
    if (sequenceWrapper)
    {
      AppendObject (object, SEQUENCE);
      continue;
    }
    
    csRef<iSequenceTrigger> sequenceTrigger =
      scfQueryInterface<iSequenceTrigger> (object);
    if (sequenceTrigger)
    {
      AppendObject (object, TRIGGER);
      continue;
    }
    
    csRef<iCameraPosition> cameraPosition =
      scfQueryInterface<iCameraPosition> (object);
    if (cameraPosition)
    {
      AppendObject (object, CAMERA_POSITION);
      continue;
    }

    // Hide unknown objects
    //AppendObject (object, UNKNOWN);
  }

  // Post-process the categories
  for (int i = 0; i < TYPE_COUNT; i++)
    if (rootIDs[i].IsOk ())
    {
      // Rename the category in order to make appear the count of children objects
      unsigned int count = GetChildrenCount (rootIDs[i], false);
      csString text;
      text.Format ("%s (%u)", categories[i]->GetData (), count);
      SetItemText (rootIDs[i], wxString (text.GetData (), *wxConvCurrent));

      // Sort the child list
      SortChildren (rootIDs[i]);
    }
}

void SceneTreeCtrl::AppendObject (iObject* object, ObjectType type)
{
  // Create the object's category if not yet made
  if (!rootIDs[type].IsOk ())
  {
    // Search the index where the category must be placed
    wxTreeItemId previous;
    for (int i = type - 1; i >= 0; i--)
      if (rootIDs[i].IsOk ())
      {
	previous = rootIDs[i];
	break;
      }

    CS_ASSERT (categories[type]);

    if (previous.IsOk ())
      rootIDs[type] = InsertItem (GetRootItem (), previous,
				  wxString (categories[type]->GetData (), *wxConvCurrent),
				  rootIconIdx, -1, nullptr);
    else
      rootIDs[type] = InsertItem (GetRootItem (), 0,
				  wxString (categories[type]->GetData (), *wxConvCurrent),
				  rootIconIdx, -1, nullptr); 
  }

  // Add the object entry in the tree
  csString name = object->GetName ();
  if (name.Trim ().IsEmpty ())
    name = "Unnamed object";

  wxTreeItemId id = AppendItem (rootIDs[type], wxString (name, *wxConvCurrent),
				/*imageIdx*/ -1, -1, new SceneTreeItemData (object));

  objects.Put (object, id);
}

void SceneTreeCtrl::OnItemActivated (wxTreeEvent& event)
{
  wxTreeItemData* itemData = GetItemData (event.GetItem ());
  if (!itemData)
  {
    Toggle (event.GetItem ());
    return;
  }

  SceneTreeItemData* data =
    static_cast<SceneTreeItemData*> (itemData);

  csRef<iContextObjectSelection> objectSelectionContext =
    scfQueryInterface<iContextObjectSelection> (editor->GetContext ());
  objectSelectionContext->SetActiveObject (data->GetObject ());
}

void SceneTreeCtrl::OnSelChanged (wxTreeEvent& event)
{
  if (selecting) return;

  csRef<iContextObjectSelection> objectSelectionContext =
    scfQueryInterface<iContextObjectSelection> (editor->GetContext ());

  wxArrayTreeItemIds selectionIds;
  unsigned int selSize = GetSelections (selectionIds);
  
  selecting = true;

  if (objectSelectionContext->GetSelectedObjects ().GetSize ())
    objectSelectionContext->ClearSelectedObjects ();
  
  // TODO: use iContext::SetSelectedObjects
  for (unsigned int i = 0; i < selSize; i++)
  {
    wxTreeItemId itemId = selectionIds[i];
    
    SceneTreeItemData* data =
      static_cast<SceneTreeItemData*> (GetItemData (itemId));
    if (!data) continue;

    iObject* object = data->GetObject ();
    if (!object) continue;

    objectSelectionContext->AddSelectedObject (object);
  }

  selecting = false;
}

void SceneTreeCtrl::OnSetSelectedObjects ()
{
  csRef<iContextObjectSelection> objectSelectionContext =
    scfQueryInterface<iContextObjectSelection> (editor->GetContext ());

  selecting = true;

  UnselectAll ();
  for (csWeakRefArray<iObject>::ConstIterator it =
	 objectSelectionContext->GetSelectedObjects ().GetIterator ();
       it.HasNext (); )
  {
    iObject* object = it.Next ();
    if (!object) continue;

    wxTreeItemId* id = objects.GetElementPointer (object);
    if (id && id->IsOk ()) SelectItem (*id);
  }

  selecting = false;
}

void SceneTreeCtrl::OnAddSelectedObject (iEvent& event)
{
  csRef<iBase> base;
  event.Retrieve ("object", base);
  csRef<iObject> object = scfQueryInterface<iObject> (base);

  wxTreeItemId* id = objects.GetElementPointer (object);
  if (id && id->IsOk ())
  {
    selecting = true;
    SelectItem (*id);
    selecting = false;
  }
}

void SceneTreeCtrl::OnDeleteSelectedObject (iEvent& event)
{
  csRef<iBase> base;
  event.Retrieve ("object", base);
  csRef<iObject> object = scfQueryInterface<iObject> (base);

  wxTreeItemId* id = objects.GetElementPointer (object);
  if (id && id->IsOk ())
  {
    selecting = true;
    UnselectItem (*id);
    selecting = false;
  }
}

void SceneTreeCtrl::OnClearSelectedObjects ()
{
  selecting = true;
  UnselectAll ();
  selecting = false;
}

void SceneTreeCtrl::OnEnterWindow (wxMouseEvent& event)
{
  // Put back the focus on this window
  SetFocus ();
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)
