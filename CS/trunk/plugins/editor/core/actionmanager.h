/*
    Copyright (C) 2007 by Seth Yastrov
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

#ifndef __ACTIONMANAGER_H__
#define __ACTIONMANAGER_H__

#include "csutil/refarr.h"
#include "ieditor/action.h"
#include "ieditor/editor.h"
#include "iutil/event.h"

struct iObjectRegistry;

using namespace CS::EditorApp;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

class ActionManager : public scfImplementation1<ActionManager, iActionManager>
{
public:
  ActionManager (iObjectRegistry* obj_reg, iEditor* editor);
  virtual ~ActionManager ();

  virtual bool Do (iAction* action);

  virtual bool Undo ();
  virtual bool Redo ();

  virtual const iAction* PeekUndo () const;
  virtual const iAction* PeekRedo () const;

  virtual void SetMaximumStackSize (size_t size);
  virtual size_t GetMaximumStackSize () const;

private:
  void NotifyListeners (iAction* action);
  
  iObjectRegistry* object_reg;
  iEditor* editor;

  csRefArray<iAction> undoStack, redoStack;
  size_t maximumStackSize;

  csEventID eventID;
};

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif
