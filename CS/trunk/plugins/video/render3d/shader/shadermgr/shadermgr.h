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

#ifndef __SHADERMGR_H__
#define __SHADERMGR_H__

#include "csutil/cfgacc.h"
#include "csutil/csstring.h"
#include "csutil/objreg.h"
#include "csutil/parray.h"
#include "csutil/ref.h"
#include "csutil/refarr.h"
#include "csutil/scf.h"
#include "csutil/util.h"
#include "csutil/weakref.h"

#include "iutil/event.h"
#include "iutil/eventh.h"

#include "iutil/string.h"
#include "iutil/comp.h"

#include "ivideo/graph3d.h"
#include "ivideo/shader/shader.h"
#include "ivideo/material.h"
#include "ivideo/texture.h"
#include "ivideo/txtmgr.h"

#include "csgfx/shadervar.h"
#include "csgfx/shadervarcontext.h"


class csShaderManager : public scfImplementation3<csShaderManager,
						  iShaderManager,
						  iEventHandler,
						  iComponent>
{
private:
  iObjectRegistry* objectreg;
  csRef<iVirtualClock> vc;
  csRef<iTextureManager> txtmgr;
  csRef<iStringSet> strings;

  bool do_verbose;

  csRefArray<iShader> shaders;
  csRefArray<iShaderCompiler> compilers;

  csConfigAccess config;

  int seqnumber;

  // standard variables
  // these are inited and updated by the shadermanager itself
  csRef<csShaderVariable> sv_time;
  void UpdateStandardVariables();

  csShaderVariableContext svcontext;
  csShaderVarStack shaderVarStack;

  csSet<csStringID> neutralTags;
  csSet<csStringID> forbiddenTags;
  csSet<csStringID> requiredTags;
  struct TagInfo
  {
    csShaderTagPresence presence;
    int priority;
  };
  csSet<csStringID>& GetTagSet (csShaderTagPresence presence);
  csHash<TagInfo, csStringID> tagInfo;

  csArray<iLight*> activeLights;

  csEventID PreProcess;
  csEventID SystemOpen;
  csEventID SystemClose;

public:
  csShaderManager(iBase* parent);
  virtual ~csShaderManager();

  void Open ();
  void Close ();

  /**\name iShaderManager implementation
   * @{ */

  /**
   * Register a shader to the shadermanager. Compiler should register all
   * shaders.
   */
  virtual void RegisterShader (iShader* shader);
  virtual void UnregisterShader (iShader* shader);
  /// Get a shader by name
  virtual iShader* GetShader (const char* name);
  /// Returns all shaders that have been created
  virtual const csRefArray<iShader> &GetShaders () { return shaders; }


  /// Register a compiler to the manager
  virtual void RegisterCompiler (iShaderCompiler* compiler);
  /// Get a shadercompiler by name
  virtual iShaderCompiler* GetCompiler (const char* name);

  /// Report a message.
  void Report (int severity, const char* msg, ...);

  /// Get the shadervariablestack used to handle shadervariables on rendering
  virtual csShaderVarStack& GetShaderVariableStack ()
  {
    return shaderVarStack;
  }

  virtual void SetTagOptions (csStringID tag, csShaderTagPresence presence, 
    int priority = 0);
  virtual void GetTagOptions (csStringID tag, csShaderTagPresence& presence, 
    int& priority);

  virtual const csSet<csStringID>& GetTags (csShaderTagPresence presence,
    int& count);

  /**
  * Set the list of active lights.
  * Active lights is lights that the shader should use.
  */
  virtual void SetActiveLights (const csArray<iLight*>& lights);

  /**
  * Get the list of active lights. 
  */
  virtual const csArray<iLight*>& GetActiveLights () const
  {
    return activeLights;
  }
  /** @} */

  /**\name iShaderVariableContext implementation
   * @{ */
  /// Add a variable to this context
  void AddVariable (csShaderVariable *variable)
    { svcontext.AddVariable (variable); }

  /// Get a named variable from this context
  csShaderVariable* GetVariable (csStringID name) const
    { return svcontext.GetVariable (name); }

  /// Get Array of all ShaderVariables
  const csRefArray<csShaderVariable>& GetShaderVariables () const
  { return svcontext.GetShaderVariables(); }

  /**
  * Push the variables of this context onto the variable stacks
  * supplied in the "stacks" argument
  */
  void PushVariables (csShaderVarStack &stacks) const
    { svcontext.PushVariables (stacks); }

  bool IsEmpty () const 
  { return svcontext.IsEmpty(); }

  void ReplaceVariable (csShaderVariable *variable)
  { svcontext.ReplaceVariable(variable); }
  void Clear () { svcontext.Clear(); }
  /** @} */

  /**\name iComponent implementation
   * @{ */
  bool Initialize (iObjectRegistry* objreg);
  /** @} */


  /**\name iEventHandler implementation
   * @{ */
  bool HandleEvent (iEvent& Event);

  CS_EVENTHANDLER_NAMES("crystalspace.graphics3d.shadermgr")
  
  CS_CONST_METHOD virtual const csHandlerID * GenericPrec (
    csRef<iEventHandlerRegistry> &, csRef<iEventNameRegistry> &,
    csEventID) const { return 0; }
  
  csHandlerID eventSucc[2];
  CS_CONST_METHOD virtual const csHandlerID * GenericSucc (
    csRef<iEventHandlerRegistry> &, csRef<iEventNameRegistry> &,
    csEventID) const 
  { 
    return 0;//eventSucc; 
  }
  
  CS_EVENTHANDLER_DEFAULT_INSTANCE_CONSTRAINTS
  /** @} */
};

#endif //__SHADERMGR_H__
