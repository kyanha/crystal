/*
    Copyright (C) 2003 by Jorrit Tyberghein
	      (C) 2003 by Frank Richter

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

#include "iutil/document.h"
#include "iutil/objreg.h"
#include "iengine/engine.h"
#include "iengine/renderloop.h"
#include "imap/services.h"
#include "ivaria/reporter.h"

#include "rlloader.h"

// Plugin stuff

SCF_IMPLEMENT_IBASE(csRenderLoopLoader);
  SCF_IMPLEMENTS_INTERFACE(iLoaderPlugin);
  SCF_IMPLEMENTS_INTERFACE(iComponent);
SCF_IMPLEMENT_IBASE_END;

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY(csRenderLoopLoader);

SCF_EXPORT_CLASS_TABLE (rlloader)
  SCF_EXPORT_CLASS_DEP (csRenderLoopLoader, 
    "crystalspace.renderloop.loop.loader",
    "", "")
SCF_EXPORT_CLASS_TABLE_END

//---------------------------------------------------------------------------

csRenderLoopLoader::csRenderLoopLoader (iBase *p)
{
  SCF_CONSTRUCT_IBASE (p);

  init_token_table (tokens);
}

csRenderLoopLoader::~csRenderLoopLoader ()
{
}

bool csRenderLoopLoader::Initialize(iObjectRegistry *object_reg)
{
  csRenderLoopLoader::object_reg = object_reg;
  
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  rsp.Initialize (object_reg);

  return true;
}

bool csRenderLoopLoader::ParseRenderSteps (iRenderLoop* loop, 
					   iDocumentNode* node)
{
  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    csStringID id = tokens.Request (child->GetValue ());
    switch (id)
    {
      case XMLTOKEN_STEP:
	{
	  csRef<iRenderStep> step = rsp.Parse (object_reg, child);
	  if (!step)
	  {
	    return false;
	  }
	  loop->AddStep (step);
	}
	break;
      default:
	if (synldr) synldr->ReportBadToken (child);
	return false;
    }
  }

  return true;
}

csPtr<iBase> csRenderLoopLoader::Parse (iDocumentNode* node, 
  iLoaderContext* ldr_context, iBase* context)
{
  csRef<iEngine> engine =
    CS_QUERY_REGISTRY (object_reg, iEngine);
  if (!engine) return 0;

  csRef<iRenderLoopManager> loopmgr =
    engine->GetRenderLoopManager ();
  if (!loopmgr) return 0;

  csRef<iRenderLoop> loop = loopmgr->Create ();

  const char* loopName = 0;

  if (node)
  {
    csRef<iDocumentNodeIterator> it = node->GetNodes ();
    while (it->HasNext ())
    {
      csRef<iDocumentNode> child = it->Next ();
      if (child->GetType () != CS_NODE_ELEMENT) continue;
      csStringID id = tokens.Request (child->GetValue ());
      switch (id)
      {
	case XMLTOKEN_NAME:
	  {
	    loopName = child->GetContentsValue ();
	  }
	  break;
	case XMLTOKEN_STEPS:
	  {
	    if (!ParseRenderSteps (loop, child))
	    {
	      return 0;
	    }
	  }
	  break;
	default:
	  if (synldr) synldr->ReportBadToken (child);
	  return 0;
      }
    }
  }

  if (loopName)
  {
    if (!loopmgr->Register (loopName, loop))
    {
      if (synldr) 
      {
	synldr->Report (
	  "crystalspace.renderloop.loop.loader",
	  CS_REPORTER_SEVERITY_WARNING,
	  node,
	  "Couldn't register render loop '%s'. Maybe a loop of the same "
	  "name already exists?",
	  loopName);
      }
    }
  }
  else
  {
    if (synldr) 
    {
      synldr->Report (
	"crystalspace.renderloop.loop.loader",
	CS_REPORTER_SEVERITY_WARNING,
	node,
	"Render loop has no name and is therefore inaccessible. "
	"This may not be what you want.");
    }
  }

  if (loop->GetStepCount () == 0)
  {
    if (synldr) 
    {
      synldr->Report (
	"crystalspace.renderloop.loop.loader",
	CS_REPORTER_SEVERITY_WARNING,
	node,
	"Render loop has no steps. "
	"This may not be what you want.");
    }
  }

  return csPtr<iBase> (loop);
}


