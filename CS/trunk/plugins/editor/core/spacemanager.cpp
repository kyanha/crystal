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
#include "csutil/weakrefarr.h"
#include "ieditor/context.h"
#include "ieditor/header.h"
#include "ieditor/layout.h"
#include "ieditor/menu.h"
#include "ieditor/operator.h"
#include "ieditor/panel.h"
#include "iutil/document.h"
#include "iutil/plugin.h"
#include "ivaria/reporter.h"

#include "editor.h"
#include "layouts.h"
#include "spacemanager.h"
#include "window.h"

#include <wx/button.h>
#include <wx/colordlg.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>

#include "data/editor/images/sceneIcon.xpm"  

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

SpaceFactory::SpaceFactory (Editor* editor)
  : scfImplementationType (this), editor (editor), icon (sceneIcon_xpm), allowMultiple (true)
{
}

const char* SpaceFactory::GetIdentifier () const
{
  return identifier;
}

const char* SpaceFactory::GetLabel () const
{
  return label;
}

const wxBitmap& SpaceFactory::GetIcon () const
{
  return icon;
}

bool SpaceFactory::GetMultipleAllowed () const
{
  return allowMultiple;
}

csPtr<iSpace> SpaceFactory::CreateInstance (wxWindow* parent)
{
  csRef<iBase> base = iSCF::SCF->CreateInstance (identifier);
  if (!base)
  {
    if (!iSCF::SCF->ClassRegistered  (identifier))
      csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.editor.core.spacefactory",
		"The space component %s is not registered",
		CS::Quote::Single (identifier));

    else csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		   "crystalspace.editor.core.spacefactory",
		   "Failed to instantiate space factory %s",
		   CS::Quote::Single (identifier.GetData ()));

    return csPtr<iSpace> (nullptr);
  }

  csRef<iSpace> ref = scfQueryInterface<iSpace> (base);
  if (!ref)
  {
    csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.core.spacefactory",
	      "The instanciation of the space factory %s is not of type iSpace",
	      CS::Quote::Single (identifier.GetData ()));
    return csPtr<iSpace> (nullptr);
  }

  base->DecRef ();
  ref->Initialize (editor->manager->object_reg, editor, this, parent);
  spaces.Push (ref); 

  return csPtr<iSpace> (ref);
}

size_t SpaceFactory::GetCount ()
{
  spaces.Compact ();
  return spaces.GetSize ();
}

size_t SpaceFactory::GetEnabledCount ()
{
  spaces.Compact ();
  size_t count = 0;
  for (size_t i = 0; i < spaces.GetSize (); i++)
    if (spaces[i] && spaces[i]->GetEnabled ())
      count++;
  return count;
}

//----------------------------------------------------------------------

ComponentManager::ComponentManager (Editor* editor)
  : scfImplementationType (this), editor (editor)
{
  csRef<iEventNameRegistry> registry =
    csQueryRegistry<iEventNameRegistry> (editor->manager->object_reg);
  RegisterQueue (editor->context->GetEventQueue (),
		 registry->GetID ("crystalspace.editor.context"));
}

ComponentManager::~ComponentManager ()
{
}

bool ComponentManager::RegisterComponent (const char* pluginName)
{
  // Check if this component is already registered
  if (components.Contains (pluginName))
  {
    csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_WARNING,
	      "crystalspace.editor.core.spacemanager",
	      "The editor component %s is already registered",
	      CS::Quote::Single (pluginName));
    return true;
  }

  // Create the editor component
  csRef<iBase> base = iSCF::SCF->CreateInstance (pluginName);
  if (!base)
  {
    if (!iSCF::SCF->ClassRegistered  (pluginName))
      csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.editor.core.spacemanager",
		"The editor component %s is not registered",
		CS::Quote::Single (pluginName));

    else csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		   "crystalspace.editor.core.spacemanager",
		   "Failed to instantiate editor component %s",
		   CS::Quote::Single (pluginName));

    return false;
  }

  csRef<iEditorComponent> ref = scfQueryInterface<iEditorComponent> (base);
  if (!ref)
  {
    csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.core.spacemanager",
	      "The instanciation of the editor component %s is not of type iEditorComponent",
	      CS::Quote::Single (pluginName));
    return false;
  }

  base->DecRef ();
  if (!ref->Initialize (editor))
    return false;

  components.PutUnique (pluginName, ref);
  // TODO: notify component loaded. In debug mode only? No for user error reports
  return true;
}

bool ComponentManager::RegisterSpace (const char* pluginName)
{
  // Check if this space is already registered
  for (size_t i = 0; i < spaceFactories.GetSize (); i++)
    if (spaceFactories[i]->identifier == pluginName)
    {
      csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_WARNING,
		"crystalspace.editor.core.spacemanager",
		"The space %s is already registered",
		CS::Quote::Single (pluginName));
      return true;
    }

  // Create the space factory
  csRef<iDocumentNode> klass = iSCF::SCF->GetPluginMetadataNode (pluginName);
  if (klass)
  {
    csRef<SpaceFactory> f;
    f.AttachNew (new SpaceFactory (editor));
    f->identifier = pluginName;
    csRef<iDocumentNode> m = klass->GetNode ("allowMultiple");
    if (m) f->allowMultiple = strcmp (m->GetContentsValue (), "true") == 0;
    csRef<iDocumentNode> label = klass->GetNode ("description");
    if (label) f->label = label->GetContentsValue ();
      
    spaceFactories.Push (f);
    return true;
  }

  // Report for the missing plugin metadata node
  if (!iSCF::SCF->ClassRegistered  (pluginName))
    csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.core.spacemanager",
	      "The space component %s is not registered to SCF",
	      CS::Quote::Single (pluginName));

  else csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		 "crystalspace.editor.core.spacemanager",
		 "No SCF metadata found for the space component %s",
		 CS::Quote::Single (pluginName));

  return false;
}

bool ComponentManager::RegisterHeader (const char* pluginName)
{
  csRef<iBase> base = iSCF::SCF->CreateInstance (pluginName);
  if (!base)
  {
    if (!iSCF::SCF->ClassRegistered  (pluginName))
      csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.editor.core.spacemanager",
		"The header component %s is not registered",
		CS::Quote::Single (pluginName));

    else csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		   "crystalspace.editor.core.spacemanager",
		   "Failed to instantiate header component %s",
		   CS::Quote::Single (pluginName));

    return false;
  }

  csRef<iHeader> header = scfQueryInterface<iHeader> (base);
  if (!header)
  {
    csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.core.spacemanager",
	      "The instanciation of the header component %s is not of type iHeader",
	      CS::Quote::Single (pluginName));
    return false;
  }

  base->DecRef ();

  csRef<iDocumentNode> klass = iSCF::SCF->GetPluginMetadataNode (pluginName);
  if (klass)
  {
    csRef<iDocumentNode> space = klass->GetNode ("space");
    if (!space)
    {
      csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.editor.core.spacemanager",
		"No space defined in the plugin metadata of the header %s",
		CS::Quote::Single (pluginName));
      return false;
    }

    // Check that the space is valid
    bool spaceValid = false;
    for (size_t i = 0; i < spaceFactories.GetSize (); i++)
      if (spaceFactories[i]->identifier == space->GetContentsValue ())
      {
	spaceValid = true;
	break;
      }

    if (!spaceValid)
    {
      csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.editor.core.spacemanager",
		"The space defined in the plugin metadata of the header %s is not valid (%s)",
		CS::Quote::Single (pluginName), CS::Quote::Single (space->GetContentsValue ()));
      return false;
    }

    // Register the header
    headers.Put (space->GetContentsValue (), header);
    return true;
  }

  csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	    "crystalspace.editor.core.spacemanager",
	    "Failed to register header");
  return false;
}

bool ComponentManager::RegisterPanel (const char* pluginName)
{
  csRef<iBase> base = iSCF::SCF->CreateInstance (pluginName);
  if (!base)
  {
    if (!iSCF::SCF->ClassRegistered  (pluginName))
      csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.editor.core.spacemanager",
		"The panel component %s is not registered",
		CS::Quote::Single (pluginName));

    else csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		   "crystalspace.editor.core.spacemanager",
		   "Failed to instantiate panel component %s",
		   CS::Quote::Single (pluginName));

    return false;
  }

  csRef<iPanel> panel = scfQueryInterface<iPanel> (base);
  if (!panel)
  {
    csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.core.spacemanager",
	      "The instanciation of the panel component %s is not of type iPanel",
	      CS::Quote::Single (pluginName));
    return false;
  }

  base->DecRef ();

  csRef<iDocumentNode> klass = iSCF::SCF->GetPluginMetadataNode (pluginName);
  if (klass)
  {
    csRef<iDocumentNode> space = klass->GetNode ("space");
    if (!space)
    {
      csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.editor.core.spacemanager",
		"No space defined in the plugin metadata of the panel %s",
		CS::Quote::Single (pluginName));
      return false;
    }

    // TODO: check space valid
    panels.Put (space->GetContentsValue (), panel);
    return true;
  }

  csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	    "crystalspace.editor.core.spacemanager",
	    "Failed to register panel");

  return false;
}

iSpaceFactory* ComponentManager::FindSpaceFactory (const char* pluginName, size_t& index) const
{
  index = 0;
  for (csRefArray<SpaceFactory>::ConstIterator it =
	 spaceFactories.GetIterator (); it.HasNext (); index++)
  {
    iSpaceFactory* n = it.Next ();
    SpaceFactory* factory = static_cast<SpaceFactory*> (n);
    if (factory->identifier == pluginName) return factory;
  }

  return nullptr;
}

iEditorComponent* ComponentManager::FindComponent (const char* pluginName) const
{
  return *components.GetElementPointer (pluginName);
}

bool ComponentManager::HandleEvent (iEvent &event)
{
  for (csRefArray<SpaceFactory>::Iterator it =
	 spaceFactories.GetIterator (); it.HasNext (); )
  {
    iSpaceFactory* n = it.Next ();
    SpaceFactory* f = static_cast<SpaceFactory*> (n);
    if (!f) continue;

    for (csWeakRefArray<iSpace>::Iterator spaces =
	   f->spaces.GetIterator (); spaces.HasNext (); )
    {
      iSpace* space = spaces.Next ();
      if (space && space->GetEnabled ()) ReDraw (space);
    }
  }

  return false;
}

const csRefArray<SpaceFactory>& ComponentManager::GetSpaceFactories ()
{
  return spaceFactories;
}


void ComponentManager::ReDraw (iSpace* space)
{
  if (!space || !space->GetFactory ()) return;
  const char* id = space->GetFactory ()->GetIdentifier ();

  // Draw header
  csRef<iHeader> header = headers.Get (id, csRef<iHeader>());
  if (header)
  {
    wxWindow* win = space->GetwxWindow ();
    if (win && win->GetParent ())
    {
      ViewControl* ctrl = static_cast<ViewControl*>(win->GetParent ());
      if (ctrl)
      {
        ctrl->SetLayout (0);
        csRef<iLayout> layout;
        layout.AttachNew (new HeaderLayout (editor->manager->object_reg,
					    editor, ctrl->GetRegion ()));
        ctrl->SetLayout (layout);
        header->Draw (editor->context, layout);
        ctrl->GetRegion ()->GetParent ()->Layout ();
      }
    }
  }
  
  // Draw panels
  wxWindow* win = space->GetwxWindow ();
  if (win)
  {
    csHash<csRef<iPanel>, csString>::Iterator panelsit = panels.GetIterator (id);

    if (panelsit.HasNext ())
    {
      wxSizer* sz = new wxBoxSizer (wxVERTICAL);
      if (win->GetSizer ())
        win->GetSizer ()->Clear (true);
      
      while (panelsit.HasNext ())
      {
        iPanel* panel = panelsit.Next ();
        if (panel && panel->Poll (editor->context))
        {
          csRef<iLayout> layout;
          
          csRef<iFactory> fact = scfQueryInterface<iFactory> (panel);
          
          CollapsiblePane* collpane = new CollapsiblePane
	    (editor->manager->object_reg, win, fact->QueryDescription ());

          layout.AttachNew (new PanelLayout (editor->manager->object_reg,
					     editor, collpane->GetPane ()));
          collpane->SetLayout (layout);
          panel->Draw (editor->context, layout);

	  collpane->Expand ();
          sz->Add (collpane, 0, wxGROW|wxALL, 10);
        }
      }

      win->SetSizer (sz, true);
      sz->Layout ();
    }
  }
}

void ComponentManager::Update ()
{
  // Update the editor components
  for (csHash<csRef<iEditorComponent>, csString>::GlobalIterator it =
	 components.GetIterator (); it.HasNext (); )
  {
    iEditorComponent* component = it.Next ();
    component->Update ();
  }

  // Update the spaces
  for (csRefArray<SpaceFactory>::Iterator it =
	 spaceFactories.GetIterator (); it.HasNext (); )
  {
    iSpaceFactory* n = it.Next ();
    SpaceFactory* f = static_cast<SpaceFactory*> (n);
    if (!f) continue;

    for (csWeakRefArray<iSpace>::Iterator spaces =
	   f->spaces.GetIterator (); spaces.HasNext (); )
    {
      iSpace* space = spaces.Next ();
      if (space) space->Update ();
    }
  }
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)
