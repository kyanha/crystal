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

#include "csutil/databuf.h"
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

#include "csplugincommon/opengl/glextmanager.h"

#include "glshader_arb.h"
#include "glshader_avp.h"

CS_LEAKGUARD_IMPLEMENT (csShaderGLAVP);

SCF_IMPLEMENT_IBASE_EXT(csShaderGLAVP)
SCF_IMPLEMENT_IBASE_EXT_END

void csShaderGLAVP::Report (int severity, const char* msg, ...)
{
  va_list args;
  va_start (args, msg);
  csReportV (shaderPlug->object_reg, severity, 
    "crystalspace.graphics3d.shader.glarb", msg, args);
  va_end (args);
}

void csShaderGLAVP::Activate ()
{
  //enable it
  glEnable(GL_VERTEX_PROGRAM_ARB);
  shaderPlug->ext->glBindProgramARB (GL_VERTEX_PROGRAM_ARB, program_num);
}

void csShaderGLAVP::Deactivate()
{
  glDisable (GL_VERTEX_PROGRAM_ARB);
}

void csShaderGLAVP::SetupState (const csRenderMesh *mesh, 
	const csShaderVarStack &stacks)
{
  size_t i;
  const csGLExtensionManager* ext = shaderPlug->ext;

  // set variables
  for(i = 0; i < variablemap.Length(); ++i)
  {
    const VariableMapEntry& mapping = variablemap[i];
    // Check if it's statically linked
    csRef<csShaderVariable> lvar = mapping.statlink;
    // If not, we check the stack
    if (!lvar && mapping.name < (csStringID)stacks.Length ()
        && stacks[mapping.name].Length () > 0)
      lvar = stacks[mapping.name].Top ();

    if(lvar)
    {
      csVector4 v4;
      if (lvar->GetValue (v4))
      {
        ext->glProgramLocalParameter4fvARB (GL_VERTEX_PROGRAM_ARB, 
	  mapping.userInt, &v4.x);
      }
    }
  }
}

void csShaderGLAVP::ResetState ()
{
}

bool csShaderGLAVP::LoadProgramStringToGL ()
{
  if (!shaderPlug->ext)
    return false;

  const csGLExtensionManager* ext = shaderPlug->ext;

  if(!ext->CS_GL_ARB_vertex_program)
    return false;

  csRef<iDataBuffer> data = programBuffer;
  if (!data)
    data = GetProgramData();
  if (!data)
    return false;

  //step to first !!
  const char* programstring = (char*)data->GetData ();
  size_t stringlen = data->GetSize ();

  size_t i=0;
  while (*programstring != '!' && i<stringlen)
  {
    ++programstring;
    ++i;
  }
  stringlen -= i;

  ext->glGenProgramsARB(1, &program_num);
  ext->glBindProgramARB(GL_VERTEX_PROGRAM_ARB, program_num);
  
  ext->glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, 
    (GLsizei)stringlen, (void*) programstring);

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
      "Couldn't load vertex program \"%s\"", description);
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
	"Warning for vertex program \"%s\": '%s'", description, 
	programErrorString);
    }
  }

  return true;
}

bool csShaderGLAVP::Load (iDocumentNode* program)
{
  if(!program)
    return false;

  csRef<iStringSet> strings = CS_QUERY_REGISTRY_TAG_INTERFACE (
    shaderPlug->object_reg, "crystalspace.shared.stringset", iStringSet);

  csRef<iDocumentNode> variablesnode = program->GetNode("arbvp");
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

bool csShaderGLAVP::Load (const char* program, 
			  csArray<csShaderVarMapping> &mappings)
{
  programBuffer.AttachNew (new csDataBuffer (csStrNew (program),
    strlen (program)));

  for (size_t i = 0; i < mappings.Length(); i++)
  {
    variablemap.Push (VariableMapEntry (mappings[i]));
  }

  return true;
}

bool csShaderGLAVP::Compile(csArray<iShaderVariableContext*> &staticContexts)
{
  shaderPlug->Open ();

  ResolveStaticVars (staticContexts);

  for (size_t i = 0; i < variablemap.Length ();)
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
    i++;
  }

  variablemap.ShrinkBestFit();

  return LoadProgramStringToGL ();
}
