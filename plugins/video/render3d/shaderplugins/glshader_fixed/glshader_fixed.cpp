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
#include "csutil/csmd5.h"
#include "csutil/scfstr.h"
#include "csgeom/vector3.h"

#include "iutil/comp.h"
#include "iutil/plugin.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/shader/shader.h"

#include "video/canvas/openglcommon/glextmanager.h"
#include "video/canvas/openglcommon/glstates.h"

#include "glshader_ffp.h"
#include "glshader_fvp.h"
#include "glshader_fixed.h"


CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (csGLShader_FIXED)

SCF_IMPLEMENT_IBASE(csGLShader_FIXED)
SCF_IMPLEMENTS_INTERFACE(iShaderProgramPlugin)
SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csGLShader_FIXED::eiComponent)
SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END


csGLShader_FIXED::csGLShader_FIXED(iBase* parent)
{
  SCF_CONSTRUCT_IBASE (parent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);

  enable = false;
}

csGLShader_FIXED::~csGLShader_FIXED()
{

}

////////////////////////////////////////////////////////////////////
//                      iShaderProgramPlugin
////////////////////////////////////////////////////////////////////
bool csGLShader_FIXED::SupportType(const char* type)
{
  if (!enable)
    return false;
  if( strcasecmp(type, "gl_fixed_fp") == 0)
    return true;
  else if( strcasecmp(type, "gl_fixed_vp") == 0)
    return true;
  return false;
}

csPtr<iShaderProgram> csGLShader_FIXED::CreateProgram(const char* type)
{
  if( strcasecmp(type, "gl_fixed_fp") == 0)
    return csPtr<iShaderProgram>(new csGLShaderFFP(object_reg, ext));
  else if( strcasecmp(type, "gl_fixed_vp") == 0)
    return csPtr<iShaderProgram>(new csGLShaderFVP(object_reg, ext));
  else
    return 0;
}

void csGLShader_FIXED::Open()
{
  if(!object_reg)
    return;

  csRef<iGraphics3D> r = CS_QUERY_REGISTRY(object_reg, iGraphics3D);
  csRef<iShaderRenderInterface> sri = SCF_QUERY_INTERFACE(r, iShaderRenderInterface);

  r->GetDriver2D()->PerformExtension ("getextmanager", &ext);

  csRef<iFactory> f = SCF_QUERY_INTERFACE (r, iFactory);
  if (f != 0 && strcmp ("crystalspace.graphics3d.opengl", 
    f->QueryClassID ()) == 0)
    enable = true;
}

csPtr<iString> csGLShader_FIXED::GetProgramID(const char* programstring)
{
  csMD5::Digest d = csMD5::Encode(programstring);
  scfString* str = new scfString();
  str->Append((const char*)d.data[0], 16);
  return csPtr<iString>(str);
}

////////////////////////////////////////////////////////////////////
//                          iComponent
////////////////////////////////////////////////////////////////////
bool csGLShader_FIXED::Initialize(iObjectRegistry* reg)
{
  object_reg = reg;
  return true;
}

