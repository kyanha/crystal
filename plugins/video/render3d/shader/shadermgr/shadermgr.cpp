/*
    Copyright (C) 2002 by Marten Svanfeldt
                          Anders Stenberg

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
#include <limits.h>
#include "csgeom/vector3.h"
#include "csgeom/vector4.h"
#include "cstypes.h"
#include "csutil/objreg.h"
#include "csutil/event.h"
#include "csutil/eventnames.h"
#include "csutil/ref.h"
#include "csutil/scf.h"
#include "csutil/callstack.h"
#include "iengine/engine.h"
#include "iengine/material.h"
#include "iengine/texture.h"
#include "igeom/clip2d.h"
#include "imap/services.h"
#include "iutil/comp.h"
#include "iutil/document.h"
#include "iutil/eventq.h"
#include "iutil/object.h"
#include "iutil/plugin.h"
#include "iutil/stringarray.h"
#include "iutil/verbositymanager.h"
#include "iutil/virtclk.h"
#include "ivaria/reporter.h"
#include "ivideo/graph3d.h"
#include "ivideo/shader/shader.h"
#include "ivideo/texture.h"

#include "shadermgr.h"
#include "nullshader.h"

// Pluginstuff
CS_IMPLEMENT_PLUGIN

CS_PLUGIN_NAMESPACE_BEGIN(ShaderManager)
{

CS_LEAKGUARD_IMPLEMENT (csNullShader);

SCF_IMPLEMENT_FACTORY (csShaderManager)

void csNullShader::SelfDestruct ()
{
  mgr->UnregisterShader ((iShader*)this);
}

//=================== csShaderManager ================//

// General stuff
csShaderManager::csShaderManager(iBase* parent) : 
  scfImplementationType (this, parent)
{
  shaderVarStack.AttachNew (new scfArray<iShaderVarStack>);
  seqnumber = 0;
  eventSucc[0] = CS_HANDLERLIST_END;
  eventSucc[1] = CS_HANDLERLIST_END;
}

csShaderManager::~csShaderManager()
{
  //clear all shaders
  shaders.DeleteAll ();
  if (weakEventHandler)
  {
    csRef<iEventQueue> q = csQueryRegistry<iEventQueue> (objectreg);
    if (q)
      RemoveWeakListener (q, weakEventHandler);
  }
}

void csShaderManager::Report (int severity, const char* msg, ...)
{
  va_list args;
  va_start (args, msg);
  csReportV (objectreg, severity, "crystalspace.graphics3d.shadermgr", 
    msg, args);
  va_end (args);
}

bool csShaderManager::Initialize(iObjectRegistry *objreg)
{
  objectreg = objreg;
  vc = csQueryRegistry<iVirtualClock> (objectreg);
  txtmgr = csQueryRegistry<iTextureManager> (objectreg);

  csRef<iVerbosityManager> verbosemgr (
    csQueryRegistry<iVerbosityManager> (objectreg));
  if (verbosemgr) 
    do_verbose = verbosemgr->Enabled ("renderer.shader");
  else
    do_verbose = false;

  PreProcess = csevPreProcess(objectreg);
  SystemOpen = csevSystemOpen(objectreg);
  SystemClose = csevSystemClose(objectreg);

  csRef<iEventHandlerRegistry> handlerReg = 
    csQueryRegistry<iEventHandlerRegistry> (objectreg);
  eventSucc[0] = handlerReg->GetGenericID ("crystalspace.graphics3d");

  csRef<iEventQueue> q = csQueryRegistry<iEventQueue> (objectreg);
  if (q)
  {
    csEventID events[] = { PreProcess, SystemOpen, SystemClose, 
			    CS_EVENTLIST_END };
    RegisterWeakListener (q, this, events, weakEventHandler);
  }

  csRef<iPluginManager> plugin_mgr = 
	csQueryRegistry<iPluginManager> (objectreg);

  strings = csQueryRegistryTagInterface<iStringSet> (
    objectreg, "crystalspace.shared.stringset");

  {
    csRef<csNullShader> nullShader;
    nullShader.AttachNew (new csNullShader (this));
    nullShader->SetName ("*null");
    RegisterShader (nullShader);
  }

  config.AddConfig (objectreg, "/config/shadermgr.cfg");

  csRef<iStringArray> classlist =
    iSCF::SCF->QueryClassList("crystalspace.graphics3d.shadercompiler.");
  size_t const nmatches = classlist.IsValid() ? classlist->Length() : 0;
  if (nmatches != 0)
  {
    size_t i;
    for (i = 0; i < nmatches; ++i)
    {
      const char* classname = classlist->Get(i);
      csRef<iShaderCompiler> plugin = 
	CS_LOAD_PLUGIN (plugin_mgr, classname, iShaderCompiler);
      if (plugin)
      {
	if (do_verbose)
	  Report (CS_REPORTER_SEVERITY_NOTIFY,
	    "Loaded compiler plugin %s, compiler: %s", 
	    classname,plugin->GetName());
        RegisterCompiler(plugin);
      }
    }
  }
  else
  {
    Report (CS_REPORTER_SEVERITY_WARNING, "No shader plugins found!");
  }
  
  csString cfgKey;
  const csString keyPrefix ("Video.ShaderManager.Tags.");
  csSet<csString> knownKeys;
  csRef<iConfigIterator> it (config->Enumerate (keyPrefix));
  while (it->HasNext ())
  {
    it->Next();
    const char* key = it->GetKey (true);
    const char* dot = strrchr (key, '.');
    cfgKey.Clear ();
    cfgKey.Append (key, dot - key);

    if (knownKeys.In ((const char*)cfgKey)) continue;
    knownKeys.Add ((const char*)cfgKey);

    const char* tagName = config->GetStr (
      keyPrefix + cfgKey + ".TagName", cfgKey);
    const char* tagPresence = config->GetStr (
      keyPrefix + cfgKey + ".Presence", "Neutral");
    int tagPriority = config->GetInt (
      keyPrefix + cfgKey + ".Priority", 0);

    csShaderTagPresence presence;
    if (strcasecmp (tagPresence, "neutral") == 0)
    {
      presence = TagNeutral;
    }
    else if (strcasecmp (tagPresence, "forbidden") == 0)
    {
      presence = TagForbidden;
    }
    else if (strcasecmp (tagPresence, "required") == 0)
    {
      presence = TagRequired;
    }
    else
    {
      Report (CS_REPORTER_SEVERITY_WARNING, 
	"Unknown shader tag presence '%s' for tag config '%s'",
	tagPresence, (const char*)cfgKey);
      continue;
    }
    csStringID tagID = strings->Request (tagName);
    SetTagOptions (tagID, presence, tagPriority);
  }

  sv_time.AttachNew (new csShaderVariable (strings->Request ("standard time")));
  sv_time->SetValue (0.0f);
  AddVariable (sv_time);

  return true;
}

void csShaderManager::Open ()
{
}

void csShaderManager::Close ()
{
}

void csShaderManager::RegisterShaderVariableAccessor (const char* name,
      iShaderVariableAccessor* accessor)
{
  sva_hash.Put (name, accessor);
}

void csShaderManager::UnregisterShaderVariableAccessor (const char* name,
      iShaderVariableAccessor* accessor)
{
  sva_hash.Delete (name, accessor);
}

iShaderVariableAccessor* csShaderManager::GetShaderVariableAccessor (
      const char* name)
{
  return sva_hash.Get (name, (iShaderVariableAccessor*)0);
}

void csShaderManager::UnregisterShaderVariableAcessors ()
{
  sva_hash.DeleteAll ();
}

bool csShaderManager::HandleEvent(iEvent& event)
{
  if (event.Name == PreProcess)
  {
    UpdateStandardVariables();
    return false;
  }
  else if (event.Name == SystemOpen)
  {
    Open ();
    return true;
  }
  else if (event.Name == SystemClose)
  {
    Close ();
    return true;
  }
  return false;
}

void csShaderManager::UpdateStandardVariables()
{
  sv_time->SetValue ((float)vc->GetCurrentTicks() / 1000.0f);
}

static int ShaderCompare (iShader* const& s1,
			  iShader* const& s2)
{
  return strcasecmp (s1->QueryObject ()->GetName(), 
    s2->QueryObject ()->GetName());
}

void csShaderManager::RegisterShader (iShader* shader)
{
  if (shader != 0)
    shaders.InsertSorted (shader, &ShaderCompare);
}

void csShaderManager::UnregisterShader (iShader* shader)
{
  if (shader != 0)
  {
    size_t index = shaders.FindSortedKey (
      csArrayCmp<iShader*, iShader*> (shader, &ShaderCompare));
    if (index != csArrayItemNotFound) shaders.DeleteIndex (index);
  }
}

void csShaderManager::UnregisterShaders ()
{
  shaders.DeleteAll ();
  csRef<csNullShader> nullShader;
  nullShader.AttachNew (new csNullShader (this));
  nullShader->SetName ("*null");
  RegisterShader (nullShader);
}

static int ShaderCompareName (iShader* const& s1,
			      const char* const& name)
{
  return strcasecmp (s1->QueryObject ()->GetName(), 
    name);
}

iShader* csShaderManager::GetShader(const char* name)
{
  size_t index = shaders.FindSortedKey (
    csArrayCmp<iShader*, const char*> (name, &ShaderCompareName));
  if (index != csArrayItemNotFound) return shaders[index];

  return 0;
}

static int CompilerCompare (iShaderCompiler* const& c1,
			    iShaderCompiler* const& c2)
{
  return strcasecmp (c1->GetName(), c2->GetName());
}

void csShaderManager::RegisterCompiler(iShaderCompiler* compiler)
{
  compilers.InsertSorted (compiler, &CompilerCompare);
}

static int CompilerCompareName (iShaderCompiler* const& c1,
				const char* const& name)
{
  return strcasecmp (c1->GetName(), name);
}

iShaderCompiler* csShaderManager::GetCompiler(const char* name)
{
  size_t index = compilers.FindSortedKey (
    csArrayCmp<iShaderCompiler*, const char*> (name, &CompilerCompareName));
  if (index != csArrayItemNotFound) return compilers[index];

  return 0;
}

csSet<csStringID>& csShaderManager::GetTagSet (csShaderTagPresence presence)
{
  switch (presence)
  {
    case TagNeutral:
      return neutralTags;
    case TagForbidden:
      return forbiddenTags;
    case TagRequired:
      return requiredTags;

    default:
      return neutralTags;
  }
}

void csShaderManager::SetTagOptions (csStringID tag,
	csShaderTagPresence presence, int priority)
{
  TagInfo* info = tagInfo.GetElementPointer (tag);
  if (info != 0)
  {
    if (info->presence != presence)
    {
      GetTagSet (info->presence).Delete (tag);
      if ((presence == TagNeutral) && (priority == 0))
      {
	tagInfo.DeleteAll (tag);
      }
      else
      {
	GetTagSet (presence).Add (tag);
	info->priority = priority;
      }
    }
  }
  else
  {
    if ((presence != TagNeutral) || (priority != 0))
    {
      TagInfo newInfo;
      newInfo.presence = presence;
      newInfo.priority = priority;
      GetTagSet (presence).Add (tag);
      tagInfo.Put (tag, newInfo);
    }
  }
}

void csShaderManager::GetTagOptions (csStringID tag,
	csShaderTagPresence& presence, int& priority)
{
  TagInfo* info = tagInfo.GetElementPointer (tag);
  if (info == 0) 
  {
    presence = TagNeutral;
    priority = 0;
    return;
  }

  presence = info->presence;
  priority = info->priority;
}

const csSet<csStringID>& csShaderManager::GetTags (csShaderTagPresence presence,
    int& count)
{
  csSet<csStringID>& set = GetTagSet (presence);
  count = (int)set.GetSize ();
  return set;
}


void csShaderManager::SetActiveLights (const csArray<iLight*>& lights)
{
  //copy over the list
  activeLights.Empty ();
  activeLights = lights;
}

}
CS_PLUGIN_NAMESPACE_END(ShaderManager)
