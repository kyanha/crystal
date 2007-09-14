/*
Copyright (C) 2002 by Anders Stenberg
                      Marten Svanfeldt

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

#ifndef __GLSHADER_CGFP_H__
#define __GLSHADER_CGFP_H__

#include "csplugincommon/shader/shaderplugin.h"
#include "csgfx/shadervarcontext.h"
#include "ivideo/shader/shader.h"
#include "csutil/strhash.h"
#include "csutil/leakguard.h"

#include "glshader_cg.h"
#include "glshader_cgcommon.h"

class csGLStateCache;

CS_PLUGIN_NAMESPACE_BEGIN(GLShaderCg)
{

class csShaderGLCGFP : public csShaderGLCGCommon
{
private:
  csGLStateCache* statecache;

  csRef<iShaderProgram> pswrap;
public:
  CS_LEAKGUARD_DECLARE (csShaderGLCGFP);

  csShaderGLCGFP (csGLShader_CG* shaderPlug) : 
    csShaderGLCGCommon (shaderPlug, "cgfp") { }

  /// Sets this program to be the one used when rendering
  virtual void Activate ();

  /// Deactivate program so that it's not used in next rendering
  virtual void Deactivate();

  /// Setup states needed for proper operation of the shader
  virtual void SetupState (const CS::Graphics::RenderMesh* mesh,
    CS::Graphics::RenderMeshModes& modes,
    const iShaderVarStack* stacks);

  /// Reset states to original
  virtual void ResetState ();

  /// Compile a program
  virtual bool Compile();

  virtual int ResolveTU (const char* binding);

  virtual const char* GetProgramType() { return "fragment"; }
};

}
CS_PLUGIN_NAMESPACE_END(GLShaderCg)

#endif //__GLSHADER_CGFP_H__
