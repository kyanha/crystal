/*
Copyright (C) 2002 by Marten Svanfeldt
                      Anders Stenberg

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "cssysdef.h"

#include "csutil/hashmap.h"
#include "csutil/objreg.h"
#include "csutil/ref.h"
#include "csutil/scf.h"
#include "csutil/scfstr.h"
#include "csutil/csmd5.h"
#include "csgeom/vector3.h"
#include "csutil/xmltiny.h"

#include "iutil/document.h"
#include "iutil/string.h"
#include "iutil/strset.h"
#include "iutil/vfs.h"
#include "ivaria/reporter.h"
#include "ivideo/graph3d.h"
#include "ivideo/rndbuf.h"
#include "ivideo/shader/shader.h"

#include "glshader_cg.h"
#include "glshader_cgcommon.h"

csShaderGLCGCommon::csShaderGLCGCommon (csGLShader_CG* shaderPlug) :
  csShaderProgram (shaderPlug->object_reg)
{
  validProgram = true;
  this->shaderPlug = shaderPlug;
  program = 0;
  cg_profile = 0;
  entrypoint = 0;

  init_token_table (xmltokens);
}

csShaderGLCGCommon::~csShaderGLCGCommon ()
{
  if (program)
    cgDestroyProgram (program);
  delete[] cg_profile;
  delete[] entrypoint;
}

void csShaderGLCGCommon::Activate()
{
  cgGLEnableProfile (cgGetProgramProfile (program));
  cgGLBindProgram (program);
}

void csShaderGLCGCommon::Deactivate()
{
  cgGLDisableProfile (cgGetProgramProfile (program));
}

void csShaderGLCGCommon::SetupState (csRenderMesh* mesh,
  const csShaderVarStack &stacks)
{
  int i;

  // set variables
  for(i = 0; i < variablemap.Length(); ++i)
  {
    // Check if it's statically linked
    csRef<csShaderVariable> lvar = variablemap[i].statlink;
    // If not, we check the stack
    if (!lvar && variablemap[i].name < (csStringID)stacks.Length ()
        && stacks[variablemap[i].name].Length () > 0)
      lvar = stacks[variablemap[i].name].Top ();

    if(lvar)
    {
      CGparameter param = (CGparameter)variablemap[i].userPtr;

      switch(lvar->GetType())
      {
        case csShaderVariable::INT:
          {
            int intval;
            if(lvar->GetValue(intval))
              cgGLSetParameter1f (param, (float)intval);
          }
          break;
        case csShaderVariable::FLOAT:
          {
            float fval;
            if(lvar->GetValue(fval))
              cgGLSetParameter1f (param, fval);
          }
          break;
        case csShaderVariable::VECTOR3:
          {
            csVector3 v3;
            if(lvar->GetValue(v3))
              cgGLSetParameter3f(param, v3.x, v3.y, v3.z);
          }
          break;
        case csShaderVariable::VECTOR4:
          {
            csVector4 v4;
            if(lvar->GetValue(v4))
              cgGLSetParameter4f(param, v4.x, v4.y, v4.z, v4.w);
          }
          break;
        default:
	  break;
      }
    }
  }
}

void csShaderGLCGCommon::ResetState()
{
}

bool csShaderGLCGCommon::DefaultLoadProgram (const char* programStr, 
  CGGLenum type, csArray<iShaderVariableContext*> &staticContexts)
{
  csShaderVariable *var;
  int i,j;

  CGprofile profile = CG_PROFILE_UNKNOWN;

  if (cg_profile != 0)
    profile = cgGetProfile (cg_profile);

  if(profile == CG_PROFILE_UNKNOWN)
    profile = cgGLGetLatestProfile (type);

  program = cgCreateProgram (shaderPlug->context, CG_SOURCE,
    programStr, profile, entrypoint ? entrypoint : "main", 0);

  if (!program)
    return false;

  cgGLLoadProgram (program);

  i = 0;
  while (i < variablemap.Length ())
  {
    // Get the Cg parameter
    CGparameter param = cgGetNamedParameter (program, 
      variablemap[i].destination);
    // Check if it's found, and just skip it if not.
    if (!param)
    {
      i++;
      continue;
    }

    if (!cgIsParameterReferenced (param))
    {
      variablemap.DeleteIndex (i);
      continue;
    }
    variablemap[i].userPtr = param;

    // Check if we've got it locally
    var = svcontext.GetVariable(variablemap[i].name);
    if (!var)
    {
      // If not, check the static contexts
      for (j=0;j<staticContexts.Length();j++)
      {
        var = staticContexts[j]->GetVariable (variablemap[i].name);
        if (var) break;
      }
    }
    if (var)
    {
      // We found it, so we add it as a static mapping
      variablemap[i].statlink = var;
    }
    i++;
  }

  variablemap.ShrinkBestFit();

  return true;
}

bool csShaderGLCGCommon::Load(iDocumentNode* program)
{
  if(!program)
    return false;

  csRef<iShaderManager> shadermgr = CS_QUERY_REGISTRY(
  	shaderPlug->object_reg, iShaderManager);

  csRef<iDocumentNode> variablesnode = program->GetNode("cgvp");
  if(variablesnode)
  {
    csRef<iDocumentNodeIterator> it = variablesnode->GetNodes ();
    while(it->HasNext())
    {
      csRef<iDocumentNode> child = it->Next();
      if(child->GetType() != CS_NODE_ELEMENT) continue;
      const char* value = child->GetValue ();
      csStringID id = xmltokens.Request (value);
      switch(id)
      {
        case XMLTOKEN_PROFILE:
	  delete[] cg_profile;
          cg_profile = csStrNew (child->GetContentsValue ());
          break;
        case XMLTOKEN_ENTRY:
	  delete[] entrypoint;
          entrypoint = csStrNew (child->GetContentsValue ());
          break;
        default:
	  if (!ParseCommon (child))
	    return false;
      }
    }
  }

  return true;
}
