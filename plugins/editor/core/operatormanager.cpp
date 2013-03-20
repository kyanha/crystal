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
#include "csutil/csevent.h"
#include "csutil/objreg.h"
#include "csutil/scf.h"
#include "csutil/stringquote.h"

#include "ieditor/context.h"
#include "iutil/document.h"
#include "ivaria/reporter.h"

#include "editor.h"
#include "operatormanager.h"

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

OperatorManager::OperatorManager (iObjectRegistry* obj_reg, Editor* editor)
  : scfImplementationType (this), object_reg (obj_reg), editor (editor)
{
  nameRegistry = csEventNameRegistry::GetRegistry (object_reg);
  if (!nameRegistry) return;

  csRef<iEventQueue> eventQueue = csQueryRegistry<iEventQueue> (object_reg);
  if (!eventQueue) return;

  eventQueue->RegisterListener (this);

  // Register for the input events, for Handle ().
  eventQueue->RegisterListener (this, nameRegistry->GetID ("crystalspace.input"));
}

OperatorManager::~OperatorManager ()
{
}

bool OperatorManager::HandleEvent (iEvent& ev)
{
  if (modalOperator)
  {
    if (modalOperator->Modal (editor->GetContext (), &ev) != OperatorRunningModal)
    {
      modalOperator.Invalidate ();
    }
    else
      return true;
  }
  return false;
} 

csPtr<iOperator> OperatorManager::CreateOperator (const char* identifier)
{
  csRef<OperatorMeta> meta = operatorMeta.Get (identifier, csRef<OperatorMeta>());
  if (!meta)
  {
    csRef<iDocumentNode> klass = iSCF::SCF->GetPluginMetadataNode (identifier);
    if (klass)
    {
      meta.AttachNew (new OperatorMeta ());
      csRef<iDocumentNode> label = klass->GetNode ("label");
      if (label) meta->label = label->GetContentsValue ();
      csRef<iDocumentNode> description = klass->GetNode ("description");
      if (description) meta->description = description->GetContentsValue ();
      operatorMeta.PutUnique (identifier, meta);
    }
  }

  if (!meta)
  {
    if (!iSCF::SCF->ClassRegistered  (identifier))
      csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.editor.core.operator",
		"The operator %s is not registered",
		CS::Quote::Single (identifier));

    else csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
		   "crystalspace.editor.core.operator",
		   "Failed to load plugin metadata for operator '%s'",
		   CS::Quote::Single (identifier));

    return csPtr<iOperator> (nullptr);
  }

  csRef<iBase> base = iSCF::SCF->CreateInstance (identifier);
  if (!base)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.core.operator",
	      "Failed to instantiate operator '%s'",
	      CS::Quote::Single (identifier));
    return csPtr<iOperator> (nullptr);
  }

  csRef<iOperator> ref = scfQueryInterface<iOperator> (base);
  if (!ref)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.core.operator",
	      "The instanciation of the operator %s is not of type iOperator",
	      CS::Quote::Single (identifier));
    return csPtr<iOperator> (nullptr);
  }

  ref->Initialize (object_reg, editor, identifier, meta->label, meta->description);

  return csPtr<iOperator> (ref);
}

iOperator* OperatorManager::Execute (iOperator* op)
{
  iContext* context = editor->GetContext ();
  if (op->Poll (context))
  {
    if (op->Execute (context) == OperatorRunningModal)
    {
      modalOperator = op;
    }
  }
  else
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR, "crystalspace.editor.core.operator",
	      "Poll failed for '%s'", op->GetIdentifier ());
  }
  return op;
}

iOperator* OperatorManager::Invoke (iOperator* op, iEvent* ev)
{
  iContext* context = editor->GetContext ();
  if (op->Poll (context))
  {
    if (op->Invoke (context, ev) == OperatorRunningModal)
    {
      modalOperator = op;
    }
  }
  else
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR, "crystalspace.editor.core.operator",
	      "Poll failed for '%s'", op->GetIdentifier ());
  }

  return op;
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)
