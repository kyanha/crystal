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
#include "csgeom/vector2.h"
#include "cstool/enginetools.h"
#include "csutil/scf.h"
#include "csutil/sysfunc.h"

#include "ieditor/action.h"
#include "ieditor/context.h"
#include "ieditor/editor.h"
#include "iengine/camera.h"
#include "iengine/mesh.h"
#include "iutil/csinput.h"
#include "csutil/event.h"
#include "csutil/eventnames.h"
#include "iutil/object.h"
#include "iutil/objreg.h"
#include "iutil/plugin.h"

#include "selectoperator.h"

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{
  
SCF_IMPLEMENT_FACTORY (SelectOperator)

SelectOperator::SelectOperator (iBase* parent)
 : scfImplementationType (this, parent)
{
}

bool SelectOperator::Initialize (iObjectRegistry* obj_reg, iEditor* editor,
				 const char* identifier, const char* label, const char* desc)
{
  this->editor = editor;
  this->object_reg = obj_reg;
  this->identifier = identifier;
  this->label = label;
  this->desc = desc;
  return true;
}

const char* SelectOperator::GetIdentifier () { return identifier; }
const char* SelectOperator::GetLabel () { return label; }
const char* SelectOperator::GetDescription () { return desc; }

SelectOperator::~SelectOperator ()
{
}

bool SelectOperator::Poll (iContext* context)
{
  csRef<iContextCamera> cameraContext = scfQueryInterface<iContextCamera> (context);
  return cameraContext->GetCamera ();
}

OperatorState SelectOperator::Execute (iContext* context)
{
  return OperatorFinished;
}

OperatorState SelectOperator::Invoke (iContext* context, iEvent* event)
{
  csRef<iContextCamera> cameraContext = scfQueryInterface<iContextCamera> (context);
  if (cameraContext->GetCamera ()->GetSector ())
  {
    int mouse_x = csMouseEventHelper::GetX (event);
    int mouse_y = csMouseEventHelper::GetY (event);
    csScreenTargetResult result =
      csEngineTools::FindScreenTarget (csVector2 (mouse_x, mouse_y),
				       100000.0f, cameraContext->GetCamera ());
    if (result.mesh)
    {
      csRef<iKeyboardDriver> kbd =
	csQueryRegistry<iKeyboardDriver> (object_reg);
      csRef<iAction> action;
      action.AttachNew (new SelectObjectAction
			(object_reg, result.mesh->QueryObject (),
			 kbd->GetKeyState (CSKEY_SHIFT)));
      editor->GetActionManager ()->Do (action);
    }
  }
  return OperatorFinished;
}

OperatorState SelectOperator::Modal (iContext*, iEvent* ev)
{
  return OperatorFinished;
}


SelectObjectAction::SelectObjectAction (iObjectRegistry* object_reg,
					iObject* obj, bool multi) 
  : scfImplementationType (this), object_reg (object_reg), object (obj),
  multiple (multi)
{
}

bool SelectObjectAction::Do (iContext* context)
{
  if (object)
  {
    csRef<iContextObjectSelection> objectSelectionContext =
      scfQueryInterface<iContextObjectSelection> (context);

    if (!multiple)
    {
      selection = objectSelectionContext->GetSelectedObjects ();
      objectSelectionContext->ClearSelectedObjects ();
      objectSelectionContext->AddSelectedObject (object);
    }

    else
    {
      if (objectSelectionContext->ContainsSelectedObject (object))
	objectSelectionContext->DeleteSelectedObject (object);
      else
	objectSelectionContext->AddSelectedObject (object);
    }

    return true; 
  }
  return false; 
}

bool SelectObjectAction::Undo (iContext* context)
{
  if (object)
  {
    csRef<iContextObjectSelection> objectSelectionContext =
      scfQueryInterface<iContextObjectSelection> (context);

    if (!multiple)
      objectSelectionContext->SetSelectedObjects (selection);

    else
    {
      if (objectSelectionContext->ContainsSelectedObject (object))
	objectSelectionContext->DeleteSelectedObject (object);
      else
	objectSelectionContext->AddSelectedObject (object);
    }

    return true; 
  }
  return false; 
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)
