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
#include "ivideo/graph3d.h"
#include "ivideo/rndbuf.h"
#include "ivideo/shader/shader.h"
//#include "ivideo/shader/shadervar.h"

#include "softshader_vp.h"

SCF_IMPLEMENT_IBASE(csSoftShader_VP)
  SCF_IMPLEMENTS_INTERFACE(iShaderProgram)
SCF_IMPLEMENT_IBASE_END

void csSoftShader_VP::Activate(iShaderPass* current, csRenderMesh* mesh)
{
}

void csSoftShader_VP::Deactivate(iShaderPass* current)
{
}

void csSoftShader_VP::BuildTokenHash()
{
  xmltokens.Register("softvp", XMLTOKEN_SOFTVP);

  xmltokens.Register("integer", 100+csShaderVariable::INT);
  xmltokens.Register("float", 100+csShaderVariable::FLOAT);
  xmltokens.Register("string", 100+csShaderVariable::STRING);
  xmltokens.Register("vector3", 100+csShaderVariable::VECTOR3);
}

bool csSoftShader_VP::Load(iDataBuffer* program)
{
  csRef<iDocumentSystem> xml (
    CS_QUERY_REGISTRY (object_reg, iDocumentSystem));
  if (!xml) xml = csPtr<iDocumentSystem> (new csTinyDocumentSystem ());
  csRef<iDocument> doc = xml->CreateDocument ();
  const char* error = doc->Parse (program);
  if (error != 0)
  { 
    csReport( object_reg, CS_REPORTER_SEVERITY_ERROR, 
      "crystalspace.graphics3d.shader.software", "XML error '%s'!", error);
    return false;
  }
  return Load(doc->GetRoot());
}

bool csSoftShader_VP::Load(iDocumentNode* program)
{
  if(!program)
    return false;

  BuildTokenHash();

  csRef<iGraphics3D> g3d = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
  csRef<iShaderManager> shadermgr = 
    CS_QUERY_REGISTRY(object_reg, iShaderManager);

  csRef<iDocumentNode> variablesnode = program->GetNode("softvp");
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
        default:
          return false;
      }
    }
  }

  return true;
}

  
bool csSoftShader_VP::Prepare()
{
  return true;
}

csPtr<iString> csSoftShader_VP::GetProgramID()
{
  csMD5::Digest d = csMD5::Encode("csSoftShader_VP");
  scfString* str = new scfString();
  str->Append((char const*)&d.data, sizeof(d.data));
  return csPtr<iString>(str);
}
