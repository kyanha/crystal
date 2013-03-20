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
#ifndef __SPACEMANAGER_H__
#define __SPACEMANAGER_H__

#include "csutil/csbaseeventh.h"
#include "csutil/hash.h"
#include "csutil/refarr.h"

#include "ieditor/space.h"
#include "ieditor/context.h"

#include <wx/bitmap.h>

using namespace CS::EditorApp;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

class Editor;
class EditorManager;

class SpaceFactory
  : public scfImplementation1<SpaceFactory, iSpaceFactory>
{
public:
  SpaceFactory (Editor* editor);

  virtual csPtr<iSpace> CreateInstance (wxWindow* parent);
  virtual const char* GetIdentifier () const;
  virtual const char* GetLabel () const;
  virtual const wxBitmap& GetIcon () const;
  virtual bool GetMultipleAllowed () const;
  virtual size_t GetCount ();
  virtual size_t GetEnabledCount ();

private:
  Editor* editor;
  csString identifier;
  csString label;
  wxBitmap icon;
  bool allowMultiple;
  csWeakRefArray<iSpace> spaces;

  friend class ComponentManager;
};

class ComponentManager
  : public scfImplementation1<ComponentManager, iComponentManager>, public csBaseEventHandler
{
public:
  ComponentManager (Editor* editor);
  virtual ~ComponentManager ();
  
  //-- iComponentManager
  virtual bool RegisterComponent (const char* pluginName);
  virtual bool RegisterSpace (const char* pluginName);
  virtual bool RegisterHeader (const char* pluginName);
  virtual bool RegisterPanel (const char* pluginName);

  virtual iEditorComponent* FindComponent (const char* pluginName) const;
  virtual iSpaceFactory* FindSpaceFactory (const char* pluginName, size_t& index) const;
  
  //-- iEventHandler
  bool HandleEvent (iEvent &event);

  const csRefArray<SpaceFactory>& GetSpaceFactories ();
  void ReDraw (iSpace* space);
  void Update ();

private:
  Editor* editor;
  csHash<csRef<iEditorComponent>, csString> components;
  csRefArray<SpaceFactory> spaceFactories;
  csHash<csRef<iHeader>, csString> headers;
  csHash<csRef<iPanel>, csString> panels;
};

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif
