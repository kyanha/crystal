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

#include "video/canvas/openglcommon/glextmanager.h"

#include "glshader_arb.h"
#include "glshader_afp.h"

SCF_IMPLEMENT_IBASE(csShaderGLAFP)
SCF_IMPLEMENTS_INTERFACE(iShaderProgram)
SCF_IMPLEMENT_IBASE_END

void csShaderGLAFP::Report (int severity, const char* msg, ...)
{
  va_list args;
  va_start (args, msg);
  csReportV (shaderPlug->object_reg, severity, 
    "crystalspace.graphics3d.shader.glarb", msg, args);
  va_end (args);
}

void csShaderGLAFP::Activate ()
{
  //enable it
  glEnable(GL_FRAGMENT_PROGRAM_ARB);
  shaderPlug->ext->glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, program_num);
}

void csShaderGLAFP::Deactivate()
{
  glDisable (GL_FRAGMENT_PROGRAM_ARB);
}

void csShaderGLAFP::SetupState (csRenderMesh *mesh, 
	const csShaderVarStack &stacks)
{
  const csGLExtensionManager* ext = shaderPlug->ext;
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
      csVector4 v4;
      if (lvar->GetValue (v4))
      {
        ext->glProgramLocalParameter4fvARB (GL_FRAGMENT_PROGRAM_ARB, 
	  variablemap[i].userInt, &v4.x);
      }
    }
  }
}

void csShaderGLAFP::ResetState ()
{
}

bool csShaderGLAFP::LoadProgramStringToGL ()
{
  if (!shaderPlug->ext)
    return false;

  const csGLExtensionManager* ext = shaderPlug->ext;

  if(!ext->CS_GL_ARB_fragment_program)
    return false;

  //step to first !!
  csRef<iDataBuffer> data = GetProgramData();
  if (!data)
    return false;

  const char* programstring = (char*)data->GetData ();
  int stringlen = data->GetSize ();

  int i=0;
  while (*programstring != '!' && (i < stringlen))
  {
    ++programstring;
    ++i;
  }
  stringlen -= i;

  ext->glGenProgramsARB(1, &program_num);
  ext->glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, program_num);
  
  ext->glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, 
    stringlen, (void*) programstring);

  const GLubyte * programErrorString = glGetString(GL_PROGRAM_ERROR_STRING_ARB);

  GLint errorpos;
  glGetIntegerv (GL_PROGRAM_ERROR_POSITION_ARB, &errorpos);
  if(errorpos != -1)
  {
    CS_ALLOC_STACK_ARRAY (char, errorStart, strlen (programstring) + 1);
    strcpy (errorStart, programstring);

    char* start = errorStart + errorpos;
    while (start > errorStart)
    {
      if (*(start - 1) == '\n')
      {
	break;
      }
      start--;
    }

    char* end = strchr (start, '\n');
    if (end)
      *(end-1) = 0;

    Report (CS_REPORTER_SEVERITY_WARNING, 
      "Couldn't load fragment program \"%s\"", description.GetData());
    Report (CS_REPORTER_SEVERITY_WARNING, "Program error at: \"%s\"", start);
    Report (CS_REPORTER_SEVERITY_WARNING, "Error string: '%s'", 
      programErrorString);
    return false;
  }
  else
  {
    if ((programErrorString != 0) && (*programErrorString != 0))
    {
      Report (CS_REPORTER_SEVERITY_WARNING, 
	"Warning for fragment program \"%s\": '%s'", description.GetData(), 
	programErrorString);
    }
  }

  return true;
}

bool csShaderGLAFP::Load(iDocumentNode* program)
{
  if(!program)
    return false;

  csRef<iDocumentNode> variablesnode = program->GetNode("arbfp");
  if (variablesnode)
  {
    csRef<iDocumentNodeIterator> it = variablesnode->GetNodes ();
    while (it->HasNext())
    {
      csRef<iDocumentNode> child = it->Next();
      if(child->GetType() != CS_NODE_ELEMENT) continue;
      if (!ParseCommon (child))
	return false;
    }
  }

  return true;
}

bool csShaderGLAFP::Compile(csArray<iShaderVariableContext*> &staticContexts)
{
  shaderPlug->Open ();

  ResolveStaticVars (staticContexts);

  for (int i = 0; i < variablemap.Length (); i++)
  {
    int dest;
    if (sscanf (variablemap[i].destination, "register %d", &dest) != 1)
    {
      Report (CS_REPORTER_SEVERITY_WARNING, 
	"Unknown variable destination %s", 
	variablemap[i].destination.GetData());
      variablemap.DeleteIndex (i);
      continue;
    }

    variablemap[i].userInt = dest;
  }

  variablemap.ShrinkBestFit();

  return LoadProgramStringToGL ();
}
