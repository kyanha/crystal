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
#ifndef __PERSPECTIVE_H__
#define __PERSPECTIVE_H__

#include "ieditor/perspective.h"

using namespace CS::EditorApp;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

class Editor;
class Perspective;
class PerspectiveWindow;
class Window;

class PerspectiveManager
  : public scfImplementation1<PerspectiveManager, iPerspectiveManager>
{
public:
  PerspectiveManager (iObjectRegistry* object_reg, Editor* editor);
  virtual ~PerspectiveManager ();
  
  //-- iPerspectiveManager
  virtual iPerspective* CreatePerspective
    (const char* name, iPerspective* other = nullptr);
  virtual size_t GetPerspectiveCount () const;
  virtual iPerspective* GetPerspective (size_t index) const;
  virtual void DeletePerspective (size_t index);

  //Window* CreateWindow (iPerspective* perspective, wxFrame* frame);

public:
  iObjectRegistry* object_reg;
  Editor* editor;
  csRefArray<Perspective> perspectives;
};

class Perspective
  : public scfImplementation1<Perspective, iPerspective>
{
public:
  Perspective (PerspectiveManager* manager, const char* name);
  Perspective (PerspectiveManager* manager, const char* name, Perspective* other);
  ~Perspective ();

  Window* CreateWindow (wxFrame* frame);

  //-- iPerspective
  virtual void SetName (const char* name);
  virtual const char* GetName () const;

  virtual iPerspectiveWindow* GetRootWindow () const;

public:
  PerspectiveManager* manager;
  csString name;
  csRef<PerspectiveWindow> root;
};

class PerspectiveWindow
  : public scfImplementation1<PerspectiveWindow, iPerspectiveWindow>
{
public:
  PerspectiveWindow ();
  PerspectiveWindow (PerspectiveWindow* other);
  ~PerspectiveWindow ();

  //-- iPerspectiveWindow
  virtual void SetSpace (const char* pluginName);
  virtual void SetSplitMode (SplitMode mode);
  virtual void SetSplitPosition (int position);
  virtual iPerspectiveWindow* GetChild1 () const;
  virtual iPerspectiveWindow* GetChild2 () const;

  void SetupWindow (Window* window);

public:
  SplitMode mode;
  csString pluginName;
  int position;
  csRef<PerspectiveWindow> child1;
  csRef<PerspectiveWindow> child2;
};

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif
