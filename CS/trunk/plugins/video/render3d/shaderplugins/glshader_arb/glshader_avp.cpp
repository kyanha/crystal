/*
Copyright (C) 2002 by M�rten Svanfeldt
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

#include "csutil/csvector.h"
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
#include "iutil/vfs.h"
#include "ivaria/reporter.h"
#include "ivideo/render3d.h"
#include "ivideo/rndbuf.h"
#include "ivideo/shader/shader.h"
//#include "ivideo/shader/shadervar.h"

#include "video/canvas/openglcommon/glextmanager.h"
#include "../../opengl/gl_helper.h"

#include "glshader_avp.h"

SCF_IMPLEMENT_IBASE(csShaderGLAVP)
SCF_IMPLEMENTS_INTERFACE(iShaderProgram)
SCF_IMPLEMENT_IBASE_END

void csShaderGLAVP::Activate(iShaderPass* current, csRenderMesh* mesh)
{
  //enable it
  glEnable(GL_VERTEX_PROGRAM_ARB);
  ext->glBindProgramARB(GL_VERTEX_PROGRAM_ARB, program_num);
}

void csShaderGLAVP::Deactivate(iShaderPass* current)
{
  glDisable (GL_VERTEX_PROGRAM_ARB);
}

void csShaderGLAVP::SetupState (iShaderPass *current, csRenderMesh *mesh)
{
    // set variables
  int i;
  for(i = 0; i < variablemap.Length(); ++i)
  {
    variablemapentry* e = (variablemapentry*)variablemap.Get(i);
    iShaderVariable* lvar = GetVariable(e->namehash );
    if(!lvar)
      lvar = current->GetVariable(e->namehash);

    if(lvar)
    {
      switch(lvar->GetType())
      {
      case iShaderVariable::INT:
        {
          int intval;
          if(lvar->GetValue(intval))
            ext->glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, e->registernum,
                                              (float)intval, (float)intval, (float)intval, (float)intval);
        }
        break;
      case iShaderVariable::FLOAT:
        {
          float fval;
          if(lvar->GetValue(fval))
            ext->glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, e->registernum,
                                              fval,fval,fval,fval);
        }
        break;
      case iShaderVariable::VECTOR3:
        {
          csVector3 v3;
          if(lvar->GetValue(v3))
            ext->glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, e->registernum,
                                              v3.x ,v3.y,v3.z,1);
        }
        break;
      case iShaderVariable::VECTOR4:
        {
          csVector4 v4;
          if(lvar->GetValue(v4))
            ext->glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, e->registernum,
                                              v4.x ,v4.y,v4.z,v4.w);
        }
        break;
      }
    }
  }
}

void csShaderGLAVP::ResetState ()
{
}

bool csShaderGLAVP::LoadProgramStringToGL(const char* programstring)
{
  if(!programstring)
    return false;
  //step to first !!
  int stringlen = strlen(programstring);
  int i=0;
  while (*programstring != '!' && i<stringlen)
  {
    ++programstring;
    ++i;
  }

  if(!ext)
    return false;

  if(!ext->CS_GL_ARB_vertex_program)
    return false;

  ext->glGenProgramsARB(1, &program_num);
  ext->glBindProgramARB(GL_VERTEX_PROGRAM_ARB, program_num);
  
  ext->glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, 
    strlen(programstring), (void*) programstring);

  const GLubyte * programErrorString = glGetString(GL_PROGRAM_ERROR_STRING_ARB);

  int errorpos;
  glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errorpos);
  if(errorpos != -1)
  {
    char* end = strchr (programstring+errorpos, '\n');
    if (end)
      *(end-1) = 0;
    const char* start = strrchr (programstring, '\n');
    if (!start)
      start = programstring+errorpos;
    else
      start++;

    csReport ( object_reg, CS_REPORTER_SEVERITY_WARNING,"crystalspace.render3d.shader.glarb",
      "Couldn't load vertexprogram", 0);
    csReport ( object_reg, CS_REPORTER_SEVERITY_WARNING,"crystalspace.render3d.shader.glarb",
      "Programerror at: \"%s\"", start);
    csReport ( object_reg, CS_REPORTER_SEVERITY_WARNING,"crystalspace.render3d.shader.glarb",
      "Errorstring %s", programErrorString);
    return false;
  }

  return true;
}

void csShaderGLAVP::BuildTokenHash()
{
  xmltokens.Register("ARBVP",XMLTOKEN_ARBVP);
  xmltokens.Register("declare",XMLTOKEN_DECLARE);
  xmltokens.Register("variablemap",XMLTOKEN_VARIABLEMAP);
  xmltokens.Register("program", XMLTOKEN_PROGRAM);

  xmltokens.Register("integer", 100+iShaderVariable::INT);
  xmltokens.Register("float", 100+iShaderVariable::FLOAT);
  xmltokens.Register("string", 100+iShaderVariable::STRING);
  xmltokens.Register("vector3", 100+iShaderVariable::VECTOR3);
}

bool csShaderGLAVP::Load(iDataBuffer* program)
{
  csRef<iDocumentSystem> xml (CS_QUERY_REGISTRY (object_reg, iDocumentSystem));
  if (!xml) xml = csPtr<iDocumentSystem> (new csTinyDocumentSystem ());
  csRef<iDocument> doc = xml->CreateDocument ();
  const char* error = doc->Parse (program);
  if (error != 0)
  { 
    csReport( object_reg, CS_REPORTER_SEVERITY_ERROR, "crystalspace.render3d.shader.glarb",
      "XML error '%s'!", error);
    return false;
  }
  return Load(doc->GetRoot());
}

bool csShaderGLAVP::Load(iDocumentNode* program)
{
  if(!program)
    return false;

  BuildTokenHash();

  csRef<iRender3D> r3d = CS_QUERY_REGISTRY (object_reg, iRender3D);
  csRef<iShaderManager> shadermgr = CS_QUERY_REGISTRY(object_reg, iShaderManager);

  csRef<iDocumentNode> variablesnode = program->GetNode("arbvp");
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
      case XMLTOKEN_PROGRAM:
        {
          //save for later loading
          programstring = new char[strlen(child->GetContentsValue())+1];
          strcpy(programstring, child->GetContentsValue());
        }
          break;
      case XMLTOKEN_DECLARE:
        {
          //create a new variable
          csRef<iShaderVariable> var = 
            shadermgr->CreateVariable (child->GetAttributeValue ("name"));
          csStringID idtype = xmltokens.Request( child->GetAttributeValue("type") );
          idtype -= 100;
          var->SetType( (iShaderVariable::VariableType) idtype);
          switch(idtype)
          {
          case iShaderVariable::INT:
            var->SetValue( child->GetAttributeValueAsInt("default") );
            break;
          case iShaderVariable::FLOAT:
            var->SetValue( child->GetAttributeValueAsFloat("default") );
            break;
          case iShaderVariable::STRING:
            var->SetValue(new scfString( child->GetAttributeValue("default")) );
            break;
          case iShaderVariable::VECTOR3:
            const char* def = child->GetAttributeValue("default");
            csVector3 v;
            sscanf(def, "%f,%f,%f", &v.x, &v.y, &v.z);
            var->SetValue( v );
            break;
          }
          // @@@ I'll blame Matze if this is bad :) /Anders Stenberg
          var->IncRef (); 
          variables.Put( csHashCompute(var->GetName()), var);
        }
        break;
      case XMLTOKEN_VARIABLEMAP:
        {
          //create a varable<->register mapping
          variablemapentry * map = new variablemapentry();
          const char* varname = child->GetAttributeValue("variable");
          map->name = new char[strlen(varname)+1];
          memset(map->name, 0, strlen(varname)+1); 
          memcpy(map->name, varname, strlen(varname));

          map->registernum = child->GetAttributeValueAsInt("register");
          //compute the namehash
          map->namehash = csHashCompute (varname);

          //save it for later
          variablemap.Push( map );
        }
        break;
      default:
        return false;
      }
    }
  }

  return true;
}

  
bool csShaderGLAVP::Prepare()
{
  return LoadProgramStringToGL(programstring);
}


csBasicVector csShaderGLAVP::GetAllVariableNames()
{
  csBasicVector res;

  csGlobalHashIterator c( &variables);
  while(c.HasNext())
  {
    res.PushSmart( (void*)((iShaderVariable*)c.Next())->GetName());
  }
  return res;
}

csSymbolTable* csShaderGLAVP::GetSymbolTable()
{
  return 0;
}

iShaderVariable* csShaderGLAVP::GetVariable(int namehash)
{
  csHashIterator c(&variables, namehash);

  if(c.HasNext())
  {
    return (iShaderVariable*)c.Next();
  }

  return 0;
}

csPtr<iString> csShaderGLAVP::GetProgramID()
{
  csMD5::Digest d = csMD5::Encode(programstring);
  scfString* str = new scfString();
  str->Append((const char*)d.data[0], 16);
  return csPtr<iString>(str);
}
