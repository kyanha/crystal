/*
    Copyright (C) 2011-2012 by Jorrit Tyberghein, Jelle Hellemans, Christian Van Brussel
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
#ifndef __IEDITOR_EDITOR_H__
#define __IEDITOR_EDITOR_H__

#include "csutil/scf.h"
#include "csutil/scf_implementation.h"

class wxFrame;

struct iDocumentNode;
struct iProgressMeter;

namespace CS {
namespace EditorApp {

struct iActionManager;
struct iComponentManager;
struct iContext;
struct iEditor;
struct iEditorFrame;
struct iMenuManager;
struct iOperatorManager;
struct iPerspective;
struct iPerspectiveManager;

// TODO namespace CS::Editor::Core

struct iEditorManager : public virtual iBase
{
  SCF_INTERFACE (iEditorManager, 1, 0, 0);

  virtual bool StartEngine () = 0;
  virtual bool StartApplication () = 0;

  virtual iEditor* CreateEditor (const char* name, const char* title,
				 iContext* context) = 0;
  virtual void DeleteEditor (iEditor* editor) = 0;
  virtual iEditor* FindEditor (const char* name) = 0;
  virtual iEditor* GetEditor (size_t index) = 0;
  virtual size_t GetEditorCount () const = 0;
};

struct iEditor : public virtual iBase
{
  SCF_INTERFACE (iEditor, 1, 0, 0);

  virtual iContext* GetContext () const = 0;

  virtual iActionManager* GetActionManager () const = 0;
  virtual iEditorManager* GetEditorManager () const = 0;
  virtual iMenuManager* GetMenuManager () const = 0;
  virtual iOperatorManager* GetOperatorManager () const = 0;
  virtual iPerspectiveManager* GetPerspectiveManager () const = 0;
  virtual iComponentManager* GetComponentManager () const = 0;
  // TODO: icon/image manager
  // TODO: translation manager

  virtual csPtr<iProgressMeter> CreateProgressMeter () const = 0;
  virtual void ReportStatus (const char* text) = 0;

  //virtual void Save (iDocumentNode* node) const = 0;
  //virtual bool Load (iDocumentNode* node) = 0;

  virtual iEditorFrame* CreateFrame (const char* title,
				     iPerspective* perspective = nullptr) = 0;
  virtual void DeleteFrame (iEditorFrame* frame) = 0;
  virtual size_t GetFrameCount () const = 0;
  virtual iEditorFrame* GetFrame (size_t index = 0) const = 0;
};

struct iEditorFrame : public virtual iBase
{
  SCF_INTERFACE (iEditorFrame, 1, 0, 0);

  virtual wxFrame* GetwxFrame () = 0;

  virtual bool SetPerspective (iPerspective* perspective) = 0;
  virtual iPerspective* GetPerspective () const = 0;
};

} // namespace EditorApp
} // namespace CS

#endif
