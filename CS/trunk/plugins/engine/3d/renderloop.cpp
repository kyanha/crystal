/*
    Copyright (C) 2003 by Jorrit Tyberghein
	      (C) 2003 by Frank Richter
              (C) 2003 by Anders Stenberg
              (C) 2004 by Marten Svanfeldt

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
#include "plugins/engine/3d/renderloop.h"

#include "csutil/sysfunc.h"

#include "iutil/objreg.h"
#include "iutil/plugin.h"
#include "iutil/document.h"
#include "iutil/vfs.h"
#include "iengine/material.h"
#include "iengine/rendersteps/irsfact.h"
#include "iengine/rendersteps/igeneric.h"
#include "imap/reader.h"

#include "csgfx/rgbpixel.h"
#include "plugins/engine/3d/engine.h"
#include "plugins/engine/3d/rview.h"
#include "csutil/xmltiny.h"

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csRenderLoop)
  SCF_IMPLEMENTS_INTERFACE(iRenderLoop)
  SCF_IMPLEMENTS_INTERFACE(iRenderStepContainer)
SCF_IMPLEMENT_IBASE_END

csRenderLoop::csRenderLoop (csEngine* engine)
{
  SCF_CONSTRUCT_IBASE (0);
  csRenderLoop::engine = engine;
}

csRenderLoop::~csRenderLoop ()
{
  SCF_DESTRUCT_IBASE ();
}

void csRenderLoop::Draw (iRenderView *rview, iSector *s)
{
  if (!shadermanager)
    shadermanager = CS_QUERY_REGISTRY (engine->object_reg, iShaderManager);

  if (s)
  {
    ((csRenderView*)rview)->SetupClipPlanes ();

    csShaderVarStack &varStack = shadermanager->GetShaderVariableStack ();
    shadermanager->PushVariables (varStack);
    
    s->IncRecLevel ();
    s->PrepareDraw (rview);

    int i;
    for (i = 0; i < steps.Length(); i++)
    {
      steps[i]->Perform (rview, s, varStack);
    }
    s->DecRecLevel ();
    shadermanager->PopVariables (varStack);

    // @@@ See above note about halos.
    iLightList* lights = s->GetLights();
    for (i = lights->GetCount () - 1; i >= 0; i--)
      // Tell the engine to try to add this light into the halo queue
      engine->AddHalo (rview->GetCamera(), 
        lights->Get (i)->GetPrivateObject ());
  }
}

int csRenderLoop::AddStep (iRenderStep* step)
{
  return steps.Push (step);
}

int csRenderLoop::GetStepCount ()
{
  return steps.Length();
}

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csRenderLoopManager)
  SCF_IMPLEMENTS_INTERFACE(iRenderLoopManager)
SCF_IMPLEMENT_IBASE_END

csRenderLoopManager::csRenderLoopManager(csEngine* engine)
{
  SCF_CONSTRUCT_IBASE (0);

  csRenderLoopManager::engine = engine;
}

csRenderLoopManager::~csRenderLoopManager()
{
// @@@ ???
#if 0
  csGlobalHashIteratorReversible it (&loops);
  while (it.HasNext())
  {
    iRenderLoop* loop = (iRenderLoop*)it.Next();
    loop->DecRef ();
  }
#endif
  SCF_DESTRUCT_IBASE ();
}

csPtr<iRenderLoop> csRenderLoopManager::Create ()
{
  csRenderLoop* loop = new csRenderLoop (engine);
  return csPtr<iRenderLoop> (loop);
}
  
bool csRenderLoopManager::Register (const char* name, iRenderLoop* loop)
{
  const char* myName = strings.Request (strings.Request (name));
  if (loops.In (myName)) return false;
  loops.Put (myName, loop);
  return true;
}
 
iRenderLoop* csRenderLoopManager::Retrieve (const char* name)
{
  return (loops.Get (name, 0));
}

const char* csRenderLoopManager::GetName (iRenderLoop* loop)
{
  return loops.GetKey (loop, 0);
}

bool csRenderLoopManager::Unregister (iRenderLoop* loop)
{
  const char* key;
  if ((key = loops.GetKey (loop, 0)) == 0) return false;
  loops.Delete (key, loop);
  return true;
}

csPtr<iRenderLoop> csRenderLoopManager::Load (const char* fileName)
{
  csRef<iPluginManager> plugin_mgr (
  	CS_QUERY_REGISTRY (engine->object_reg, iPluginManager));

  csRef<iLoaderPlugin> rlLoader =
    CS_LOAD_PLUGIN (plugin_mgr,
      "crystalspace.renderloop.loop.loader",
      iLoaderPlugin);

  if (rlLoader == 0)
  {
    engine->Error ("Error loading '%s': could not retrieve render loop loader",
      fileName);
    return 0;
  }

  csRef<iFile> file = engine->VFS->Open (fileName, VFS_FILE_READ);
  if (file == 0)
  {
    engine->Error ("Error loading '%s': could open file on VFS", fileName);
    return 0;
  }

  csRef<iDocumentSystem> xml/* (CS_QUERY_REGISTRY (engine->object_reg, 
    iDocumentSystem))*/;  
      /* @@@ Eeek. The iDocumentSystem may not be initialized. */
  if (!xml) xml.AttachNew (new csTinyDocumentSystem ());
  csRef<iDocument> doc = xml->CreateDocument ();

  const char* error = doc->Parse (file);
  if (error != 0)
  {
    engine->Error ("Error parsing '%s': %s", fileName, error);
    return 0;
  }

  csRef<iDocumentNode> rlNode = doc->GetRoot ()->GetNode ("params");
  if (rlNode == 0)
  {
    engine->Error ("Error loading '%s': no <params> node", fileName);
    return 0;
  }

  csRef<iBase> b = rlLoader->Parse (rlNode, 0, 0);
  if (!b)
  {
    // Error already reported.
    return 0;
  }
  csRef<iRenderLoop> rl = SCF_QUERY_INTERFACE (b, iRenderLoop);
  if (rl == 0)
  {
    engine->ReportBug (
      "Error loading '%s': returned object doesn't implement iRenderLoop", 
      fileName);
  }
  return (csPtr<iRenderLoop> (rl));
}

