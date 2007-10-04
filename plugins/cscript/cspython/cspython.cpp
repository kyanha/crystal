/*
    Copyright (C) 1999 by Brandon Ehle <azverkan@yahoo.com>

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

#if !defined(DEBUG_PYTHON) && defined(_DEBUG)
#undef _DEBUG
#endif

#include <Python.h>
#include <stdio.h>

#include "cssysdef.h"
#include "csutil/sysfunc.h"
#include "csutil/syspath.h"
#include "csutil/csstring.h"
#include "csutil/util.h"
#include "iutil/cmdline.h"
#include "iutil/eventq.h"
#include "csutil/event.h"
#include "csutil/eventhandlers.h"
#include "iutil/objreg.h"
#include "iutil/vfs.h"
#include "ivaria/reporter.h"

#include "cspython.h"
#include "pytocs.h"

extern "C"
{
  #include "swigpyruntime.h"
}

CS_IMPLEMENT_PLUGIN

CS_PLUGIN_NAMESPACE_BEGIN(cspython)
{

SCF_IMPLEMENT_FACTORY(csPython)

csPython* csPython::shared_instance = 0;

csPython::csPython(iBase *iParent) : scfImplementationType (this, iParent),
  object_reg(0), Mode(CS_REPORTER_SEVERITY_NOTIFY), use_debugger(false)
{
  shared_instance = this;
}

csPython::~csPython()
{
  csRef<iEventQueue> queue = csQueryRegistry<iEventQueue> (object_reg);
  if (queue.IsValid())
    queue->RemoveListener (eventHandler);
  Mode = CS_REPORTER_SEVERITY_BUG;
  Py_Finalize();
  object_reg = 0;
}

PyObject* csWrapTypedObject(void* objectptr, const char *typetag,
  int own)
{
  swig_type_info *ti = SWIG_TypeQuery (typetag);
  PyObject *obj = SWIG_NewPointerObj (objectptr, ti, own);
  return obj;
}

static csString const& path_append(csString& path, char const* component)
{
  char c;
  size_t n = path.Length();
  if (n > 0 && (c = path[n - 1]) != '/' && c != '\\')
    path << '/';
  path << component;
  return path;
}

bool csPython::Initialize(iObjectRegistry* object_reg)
{
  csPython::object_reg = object_reg;

  csRef<iCommandLineParser> cmdline(
    csQueryRegistry<iCommandLineParser> (object_reg));
  bool const reporter = cmdline->GetOption("python-enable-reporter") != 0;
  use_debugger = cmdline->GetOption("python-enable-debugger") != 0;
 
  // Add compile location to pythonpath 
#ifdef DEFAULT_PYTHMOD_PATH
  csString pythonpath (getenv ("PYTHONPATH"));
  pythonpath.Insert (0, CS_PATH_DELIMITER);
  pythonpath.Insert (0, DEFAULT_PYTHMOD_PATH);
  setenv ("PYTHONPATH", pythonpath, true);
#endif

  Py_SetProgramName("Crystal Space -- Python");
  Py_Initialize();
  InitPytocs();

  if (!LoadModule ("sys")) return false;

  // Add /scripts vfs path to pythonpath
  csString cmd;
  csRef<iVFS> vfs(csQueryRegistry<iVFS> (object_reg));
  if (vfs.IsValid())
  {
    csRef<iStringArray> paths(vfs->GetRealMountPaths("/scripts"));
    for (size_t i = 0, n = paths->GetSize(); i < n; i++)
    {
      csString path = paths->Get(i);
      cmd << "sys.path.append('" << path_append(path, "python") << "')\n";
    }
  }

  // Add cs python scripts folder to pythonpath
  csString cfg(csGetConfigPath());
  cmd << "sys.path.append('" << path_append(cfg, "scripts/python") << "')\n";
  if (!RunText (cmd)) return false;

  if (reporter && !LoadModule ("cshelper")) return false;
  if (use_debugger && !LoadModule ("pdb")) return false;
  if (!LoadModule ("cspace")) return false;

  Mode = CS_REPORTER_SEVERITY_NOTIFY;

  // Inject iSCF::SCF
  // @@@ Ugly... shouldn't have to Store/assign from py, just Store.
  Store("cspace.__corecvar_iSCF_SCF", iSCF::SCF, (void*)"iSCF *");
  RunText("cspace.SetSCFPointer(cspace.__corecvar_iSCF_SCF)");
  // Store the object registry pointer in 'cspace.object_reg'.
  Store("cspace.object_reg", object_reg, (void *) "iObjectRegistry *");

  eventHandler.AttachNew (new EventHandler (this));
  csRef<iEventQueue> queue = csQueryRegistry<iEventQueue> (object_reg);
  if (queue.IsValid())
    queue->RegisterListener(eventHandler, csevCommandLineHelp(object_reg));
  return true;
}

bool csPython::HandleEvent(iEvent& e)
{
  bool handled = false;
  if (e.Name == csevCommandLineHelp(object_reg))
  {
#undef indent
#define indent "                     "
    csPrintf("Options for csPython plugin:\n"
	   "  -python-enable-reporter\n"
	   indent "Redirect sys.stdout and sys.stderr to iReporter\n"
	   "  -python-enable-debugger\n"
	   indent "When Python exception is thrown, launch Python debugger\n");
#undef indent
    handled = true;
  }
  return handled;
}

void csPython::ShowError()
{
  if(PyErr_Occurred())
  {
    PyErr_Print();
    Print(true, "ERROR!\n");
  }
}

bool csPython::RunText(const char* Text)
{
  // Apparently, some Python installations have bad prototype, so const_cast<>.
  bool ok = !PyRun_SimpleString(const_cast<char*> (Text));
  if(!ok && use_debugger)
    PyRun_SimpleString(const_cast<char*> ("pdb.pm()"));
  ShowError();
  return ok;
}

bool csPython::Store(const char* name, void* data, void* tag)
{
  bool ok = false;
  PyObject *obj = csWrapTypedObject(data, (const char*)tag, 0);
  char *mod_name = csStrNew(name);
  char *var_name = strchr(mod_name, '.');
  if (var_name)
  {
    *var_name = '\0';
    ++var_name;
    PyObject * module = PyImport_ImportModule(mod_name);
    PyModule_AddObject(module, var_name, obj);
    ok = true;
  }
  delete[] mod_name;
  return ok;
}

bool csPython::LoadModule(const char* name)
{
  csString s;
  s << "import " << name;
  return RunText(s);
}

bool csPython::LoadModule (const char *path, const char *name)
{
  csRef<iVFS> vfs(csQueryRegistry<iVFS> (object_reg));
  if (!vfs.IsValid())
    return false;
  csRef<iDataBuffer> rpath = vfs->GetRealPath (path);

  return LoadModuleNative (rpath->GetData (), name);
}

bool csPython::LoadModuleNative (const char *path, const char *name)
{
  // Alternative from `embedding' in py c api docs:
  //   Must provide custom implementation of
  //  Py_GetPath(), Py_GetPrefix(), Py_GetExecPrefix(),
  //  and Py_GetProgramFullPath()
  csString import;
  import << "import sys\n"
         << "paths = sys.path\n"
         << "sys.path = ['" << path << "']\n"
         << "import " << name << "\n"
         << "sys.path = paths\n";
  return RunText(import);
}

void csPython::Print(bool Error, const char *msg)
{
  csReport (object_reg, Error ? CS_REPORTER_SEVERITY_ERROR : Mode,
    "crystalspace.script.python", "%s", msg);
}

}
CS_PLUGIN_NAMESPACE_END(cspython)
