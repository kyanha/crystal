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
#include "csgfx/shadervarcontext.h"

CS_LEAKGUARD_IMPLEMENT (csShaderVariableContext)

SCF_IMPLEMENT_IBASE(csShaderVariableContext)
  SCF_IMPLEMENTS_INTERFACE(iShaderVariableContext)
SCF_IMPLEMENT_IBASE_END

csShaderVariableContext::csShaderVariableContext ()
{
  SCF_CONSTRUCT_IBASE(0);
}

csShaderVariableContext::~csShaderVariableContext ()
{
  SCF_DESTRUCT_IBASE ();
}

void csShaderVariableContext::AddVariable 
  (csShaderVariable *variable) 
{
  csShaderVariable* var = GetVariable(variable->Name);
  if (var == 0)
    variables.Push (variable);
  else
    *var = *variable;
}

csShaderVariable* csShaderVariableContext::GetVariable 
  (csStringID name) const 
{
  for (size_t i=0; i<variables.Length (); ++i)
  {
    if (variables[i]->GetName () == name)
      return variables[i];
  }
  return 0;
}

void csShaderVariableContext::PushVariables 
  (csShaderVarStack &stacks) const
{
  for (size_t i=0; i<variables.Length (); ++i)
  {
    csStringID name = variables[i]->GetName ();
    if (stacks.Length () <= (size_t)name)
      stacks.SetLength (name+1);
    stacks[name].Push (variables[i]);
  }
}

void csShaderVariableContext::PopVariables 
  (csShaderVarStack &stacks) const
{
  for (size_t i=0; i<variables.Length (); ++i)
  {
    csStringID name = variables[i]->GetName ();
    if ((stacks.Length () > (size_t)name) && // @@@ Why is this needed?
      (stacks[name].Length () > 0))
      stacks[name].Pop ();
  }
}
