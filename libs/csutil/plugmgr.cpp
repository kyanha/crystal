/*
    Copyright (C) 1998-2003 by Jorrit Tyberghein

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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "csutil/array.h"
#include "csutil/plugmgr.h"
#include "csutil/scf_implementation.h"
#include "csutil/stringconv.h"
#include "csutil/util.h"

#include "iutil/comp.h"
#include "iutil/objreg.h"
#include "iutil/cmdline.h"
#include "iutil/cfgmgr.h"
#include "ivaria/reporter.h"

//------------------------------------------------------ csPlugin class -----//

csPluginManager::csPlugin::csPlugin (iComponent *obj, const char *classID)
{
  Plugin = obj;
  ClassID = CS::StrDup (classID);
}

csPluginManager::csPlugin::~csPlugin ()
{
  //csPrintf ("DecRef %08p/'%s' ref=%d\n", Plugin, ClassID, Plugin->GetRefCount ()); fflush (stdout);
  cs_free (ClassID);
}

//------------------------------------------------------------------------
/**
 * Implementation of iPluginIterator.
 */
class csPluginIterator : public scfImplementation1<csPluginIterator,
                                                   iPluginIterator>
{
public:
  csArray<iBase*> pointers;
  size_t idx;

public:
  csPluginIterator ()
    : scfImplementationType (this), idx (0)
  {
  }
  virtual ~csPluginIterator ()
  {
  }


  virtual bool HasNext ()
  {
    return idx < pointers.GetSize ();
  }
  virtual iBase* Next ()
  {
    iBase* p = pointers[idx];
    idx++;
    return p;
  }
};


//------------------------------------------------------------------------


csPluginManager::csPluginManager (iObjectRegistry* object_reg) 
  : scfImplementationType (this), object_reg (object_reg),
  Plugins (8), OptionList (16)
{
}

csPluginManager::~csPluginManager ()
{
  Clear ();
}

void csPluginManager::Clear ()
{
  CS::Threading::RecursiveMutexScopedLock lock (mutex);
 
  OptionList.DeleteAll ();

  // Free all plugins.
  for (size_t i = Plugins.GetSize () ; i > 0 ; i--)
    UnloadPlugin ((iComponent *)Plugins.Get(i - 1)->Plugin);
}

void csPluginManager::QueryOptions (iComponent *obj)
{
  csRef<iCommandLineParser> CommandLine (
  	csQueryRegistry<iCommandLineParser> (object_reg));

  csRef<iPluginConfig> Config (scfQueryInterface<iPluginConfig> (obj));
  if (Config)
  {
    size_t on = OptionList.GetSize ();
    for (int i = 0 ; ; i++)
    {
      csOptionDescription option;
      if (!Config->GetOptionDescription (i, &option))
        break;
      OptionList.Push (new csPluginOption (option.name, option.type, option.id,
        (option.type == CSVAR_BOOL) || (option.type == CSVAR_CMD), Config));
      if (option.type == CSVAR_BOOL)
      {
        char buf[100];
        strcpy (buf, "no");
        strcpy (buf + 2, option.name);
        OptionList.Push (new csPluginOption (buf, option.type, option.id,
          false, Config));
      }
    }

    // Parse the command line for plugin options and pass them to plugin
    for (; on < OptionList.GetSize (); on++)
    {
      csPluginOption *pio = (csPluginOption *)OptionList.Get (on);
      const char *val = CommandLine->GetOption (pio->Name);

      if (val)
      {
        csVariant optval;
        switch (pio->Type)
        {
          case CSVAR_CMD:
	    optval.SetCommand ();
	    break;
          case CSVAR_BOOL:
            optval.SetBool (pio->Value);
            break;
          case CSVAR_LONG:
            if (!val) continue;
            optval.SetLong (atol (val));
            break;
          case CSVAR_FLOAT:
            if (!val) continue;
            optval.SetFloat (CS::Utility::strtof (val));
            break;
	  case CSVAR_STRING:
	    if (!val) continue;
	    optval.SetString (val);
	    break;
        }
        pio->Config->SetOption (pio->ID, &optval);
      }
    }
  }
}

iBase* csPluginManager::LoadPlugin(const char *classID, bool init, bool report)
{
  csRef<iComponent> p (scfCreateInstance<iComponent> (classID));

  if (!p)
  {
    if (report)
      csReport (object_reg, CS_REPORTER_SEVERITY_WARNING,
      "crystalspace.pluginmgr.loadplugin",
      "could not load plugin '%s'", classID);
  }
  else
  {
    CS::Threading::RecursiveMutexScopedLock lock (mutex);
    size_t index = csArrayItemNotFound;
    // See if the plugin is already in our plugin list.
    for (size_t i = 0 ; i < Plugins.GetSize () ; i++)
    {
      csPlugin* pl = Plugins.Get (i);
      if (pl->ClassID)
        if (pl->ClassID == classID || !strcmp (pl->ClassID, classID))
        {
          index = i;
          break;
        }
    }

    if (index == csArrayItemNotFound)
    {
      // The plugin wasn't in our plugin list yet. Add it here.
      index = Plugins.Push (new csPlugin (p, classID));
    }

    if ((!init) || p->Initialize (object_reg))
    {
      p->IncRef();
      if (init) QueryOptions (p);
      return p;
    }
    if (report)
      csReport (object_reg, CS_REPORTER_SEVERITY_WARNING,
      "crystalspace.pluginmgr.loadplugin",
      "failed to initialize plugin '%s'", classID);
    // If we added this plugin in this call then we remove it here as well.
    if (index != csArrayItemNotFound)
      Plugins.DeleteIndex (index);
  }
  return NULL;
}

bool csPluginManager::RegisterPlugin (const char *classID,
  iComponent *obj)
{
  CS::Threading::RecursiveMutexScopedLock lock (mutex);
  size_t index = Plugins.Push (new csPlugin (obj, classID));
  if (obj->Initialize (object_reg))
  {
    QueryOptions (obj);
    return true;
  }
  else
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_WARNING,
    	"crystalspace.pluginmgr.registerplugin",
    	"failed to initialize plugin '%s'", classID);
    Plugins.DeleteIndex (index);
    return false;
  }
}

csPtr<iPluginIterator> csPluginManager::GetPlugins ()
{
  CS::Threading::RecursiveMutexScopedLock lock (mutex);
  csPluginIterator* it = new csPluginIterator ();
  size_t i;
  for (i = 0 ; i < Plugins.GetSize () ; i++)
  {
    it->pointers.Push (Plugins.Get (i)->Plugin);
  }
  return csPtr<iPluginIterator> (it);
}

iBase *csPluginManager::QueryPlugin (const char *iInterface, int iVersion)
{
  scfInterfaceID ifID = iSCF::SCF->GetInterfaceID (iInterface);
  CS::Threading::RecursiveMutexScopedLock lock (mutex);
  for (size_t i = 0; i < Plugins.GetSize (); i++)
  {
    iBase *ret = Plugins.Get (i)->Plugin;
    if (ret->QueryInterface (ifID, iVersion))
      return ret;
  }
  return 0;
}

iBase *csPluginManager::QueryPlugin (const char* classID,
				     const char *iInterface, 
                                     int iVersion)
{
  scfInterfaceID ifID = iSCF::SCF->GetInterfaceID (iInterface);
  CS::Threading::RecursiveMutexScopedLock lock (mutex);
  for (size_t i = 0 ; i < Plugins.GetSize () ; i++)
  {
    csPlugin* pl = Plugins.Get (i);
    if (pl->ClassID)
      if (pl->ClassID == classID || !strcmp (pl->ClassID, classID))
      {
        iBase *p = Plugins.Get(i)->Plugin;
        if (p->QueryInterface(ifID,iVersion))
          return p;
      }
  }
  return 0;
}

bool csPluginManager::UnloadPlugin (iComponent* obj)
{
  CS::Threading::RecursiveMutexScopedLock lock (mutex);
  size_t idx = Plugins.FindKey (
    csArrayCmp<csPlugin*,iComponent*>(obj, csPluginsVector::CompareAddress));
  if (idx == csArrayItemNotFound)
    return false;

  csRef<iPluginConfig> config (scfQueryInterface<iPluginConfig> (obj));
  if (config)
  {
    for (size_t i = OptionList.GetSize (); i > 0; i--)
    {
      csPluginOption *pio = (csPluginOption *)OptionList.Get (i - 1);
      if (pio->Config == config)
        OptionList.DeleteIndex (i - 1);
    }
  }

  object_reg->Unregister (obj, 0);
  return Plugins.DeleteIndex (idx);
}

