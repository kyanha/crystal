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
#include "csutil/csmd5.h"
#include "csutil/scfstr.h"
#include "csgeom/vector3.h"

#include "iutil/comp.h"
#include "iutil/plugin.h"
#include "ivideo/graph2d.h"
#include "ivideo/render3d.h"
#include "ivideo/shader/shader.h"

#include "video/canvas/openglcommon/glextmanager.h"
#include "../../opengl/gl_helper.h"

#include "glshader_avp.h"

#include "glshader_arb.h"


CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (csGLShader_ARB)

SCF_IMPLEMENT_IBASE(csGLShader_ARB)
SCF_IMPLEMENTS_INTERFACE(iShaderProgramPlugin)
SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csGLShader_ARB::eiComponent)
SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END


csGLShader_ARB::csGLShader_ARB(iBase* parent)
{
  SCF_CONSTRUCT_IBASE (parent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csGLShader_ARB::~csGLShader_ARB()
{

}

////////////////////////////////////////////////////////////////////
//                      iShaderProgramPlugin
////////////////////////////////////////////////////////////////////
bool csGLShader_ARB::SupportType(const char* type)
{
  if( strcasecmp(type, "gl_arb_vp") == 0)
    return true;
  return false;
}

csPtr<iShaderProgram> csGLShader_ARB::CreateProgram(const char* type)
{
  if( strcasecmp(type, "gl_arb_vp") == 0)
    return csPtr<iShaderProgram>(new csShaderGLAVP(object_reg, ext, varr));
  else
    return 0;
}

void csGLShader_ARB::Open()
{
  if(!object_reg)
    return;

  csRef<iRender3D> r = CS_QUERY_REGISTRY(object_reg,iRender3D);
  csRef<iShaderRenderInterface> sri = SCF_QUERY_INTERFACE(r, iShaderRenderInterface);

  r->GetDriver2D()->PerformExtension ("getextmanager", &ext);
  ext->InitGL_ARB_vertex_program ();
  varr = (csGLVertexArrayHelper*) sri->GetPrivateObject ("varr");
}

csPtr<iString> csGLShader_ARB::GetProgramID(const char* programstring)
{
  csMD5::Digest d = csMD5::Encode(programstring);
  scfString* str = new scfString();
  str->Append((const char*)d.data[0], 16);
  return csPtr<iString>(str);
}

////////////////////////////////////////////////////////////////////
//                          iComponent
////////////////////////////////////////////////////////////////////
bool csGLShader_ARB::Initialize(iObjectRegistry* reg)
{
  object_reg = reg;
  return true;
}

