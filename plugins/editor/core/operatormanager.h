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

#ifndef __OPERATORMANAGER_H__
#define __OPERATORMANAGER_H__

#include "csutil/refarr.h"
#include "csutil/hash.h"

#include "ieditor/operator.h"
#include "iutil/event.h"
#include "iutil/eventh.h"
#include "iutil/eventq.h"

using namespace CS::EditorApp;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{
  
struct OperatorMeta : public CS::Utility::AtomicRefCount
{
  csString label;
  csString description;
};

class Editor;

class OperatorManager : public scfImplementation2<OperatorManager, iOperatorManager, iEventHandler>
{
public:
  OperatorManager (iObjectRegistry* obj_reg, Editor* editor);
  virtual ~OperatorManager ();
  
  //-- iOperatorManager
  virtual csPtr<iOperator> CreateOperator (const char* identifier);

  virtual iOperator* Execute (iOperator* op);
  virtual iOperator* Invoke (iOperator* op, iEvent* event);
  
private:  
  //-- iEventHandler
  virtual bool HandleEvent (iEvent& ev);
  csRef<iOperator> modalOperator; //TODO: needs to become a stack?
  
  /// The event name registry, used to convert event names to IDs and back.
  csRef<iEventNameRegistry> nameRegistry;

  CS_EVENTHANDLER_NAMES ("crystalspace.editor.core.operator")
  CS_EVENTHANDLER_NIL_CONSTRAINTS

private:
  iObjectRegistry* object_reg;
  Editor* editor;
  csHash<csRef<OperatorMeta>, csString> operatorMeta;
};

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif
