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

#ifndef __CS_CSGFX_SHADERVARCONTEXT_H__
#define __CS_CSGFX_SHADERVARCONTEXT_H__

#include "csextern.h"

#include "ivideo/shader/shader.h"
#include "shadervar.h"

/**\file
 * Simple implementation for iShaderVariableContext.
 */

class CS_CSGFX_EXPORT csShaderVariableContext : public iShaderVariableContext
{
  csRefArray<csShaderVariable> variables;
public:
  SCF_DECLARE_IBASE;

  csShaderVariableContext ();

  virtual ~csShaderVariableContext ();

  /// Add a variable to this context
  virtual void AddVariable (csShaderVariable *variable);

  /// Get a named variable from this context
  virtual csShaderVariable* GetVariable (csStringID name) const;

  /**
   * Push the variables of this context onto the variable stacks
   * supplied in the "stacks" argument
   */
  virtual void PushVariables 
    (csShaderVarStack &stacks) const;

  /**
  * Pop the variables of this context off the variable stacks
  * supplied in the "stacks" argument
  */
  virtual void PopVariables 
    (csShaderVarStack &stacks) const;
};

#endif // __CS_CSGFX_SHADERVARCONTEXT_H__
