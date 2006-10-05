/*
    Copyright (C) 1998 by Jorrit Tyberghein
    Written by Brandon Ehle

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

#ifndef __CS_CSPYTHON_H__
#define __CS_CSPYTHON_H__

#include "ivaria/script.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "csutil/csinput.h"

CS_PLUGIN_NAMESPACE_BEGIN(cspython)
{

class csPython : public scfImplementation2<csPython, 
                                           iScript,
                                           iComponent> 
{
public:
  csPython(iBase *iParent);
  virtual ~csPython();

  static csPython* shared_instance;
  iObjectRegistry* object_reg;
  int Mode;
  bool use_debugger;

  virtual bool Initialize(iObjectRegistry* object_reg);
  virtual bool HandleEvent(iEvent&);
  virtual bool RunText(const char *Text);
  virtual bool LoadModule(const char *Text);
  virtual bool LoadModule (const char *path, const char *name);
  virtual bool Store(const char* name, void* data, void* tag);

  /*
    @@@ New functions not yet implemented
  */
  virtual bool Call(const char *name, const char *fmt, ...)
    { return false; }
  virtual bool Call(const char *name, int &ret, const char *fmt, ...)
    { return false; }
  virtual bool Call(const char *name, float &ret, const char *fmt, ...)
    { return false; }
  virtual bool Call(const char *name, double &ret, const char *fmt, ...)
    { return false; }
  virtual bool Call(const char *name, csRef<iString> &ref,
    const char *fmt, ...)
    { return false; }
  virtual bool Call(const char *name, csRef<iScriptObject> &ref,
    const char *fmt, ...)
    { return false; }
  virtual csRef<iScriptObject> NewObject(const char *type,
    const char *fmt, ...)
    { return 0; }
  virtual bool Store(const char *name, int data)
    { return false; }
  virtual bool Store(const char *name, float data)
    { return false; }
  virtual bool Store(const char *name, double data)
    { return false; }
  virtual bool Store(const char *name, char const *data)
    { return false; }
  virtual bool Store(const char *name, iScriptObject *data)
    { return false; }
  virtual bool SetTruth(const char *name, bool data)
    { return false; }
  virtual bool Retrieve(const char *name, int &data) const
    { return false; }
  virtual bool Retrieve(const char *name, float &data) const
    { return false; }
  virtual bool Retrieve(const char *name, double &data) const
    { return false; }
  virtual bool Retrieve(const char *name, csRef<iString> &data) const
    { return false; }
  virtual bool Retrieve(const char *name, csRef<iScriptObject> &data) const
    { return false; }
  virtual bool GetTruth(const char *name, bool &data) const
    { return false; }
  virtual bool Remove(const char *name)
    { return false; }

  void ShowError();
  void Print(bool Error, const char *msg);

  // Implement iEventHandler interface.
  struct EventHandler : public scfImplementation1<EventHandler, iEventHandler>
  {
    csPython* parent;

    EventHandler (csPython* parent) : scfImplementationType (this), 
      parent (parent) {}
    virtual ~EventHandler() {}
    virtual bool HandleEvent (iEvent& e) { return parent->HandleEvent(e); }

    CS_EVENTHANDLER_NAMES("crystalspace.cspython")
    CS_EVENTHANDLER_NIL_CONSTRAINTS
  };
  csRef<EventHandler> eventHandler;
};

}
CS_PLUGIN_NAMESPACE_END(cspython)

#endif // __CS_CSPYTHON_H__
