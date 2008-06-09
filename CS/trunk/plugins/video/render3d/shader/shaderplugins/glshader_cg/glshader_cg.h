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

#ifndef __GLSHADER_CG_H__
#define __GLSHADER_CG_H__

#include "cg_common.h"

#include "csplugincommon/shader/shaderplugin.h"
#include "csutil/blockallocator.h"
#include "csutil/leakguard.h"

#include "iutil/comp.h"
#include "ivideo/shader/shader.h"

#include "glshader_cgcommon.h"

struct csGLExtensionManager;

CS_PLUGIN_NAMESPACE_BEGIN(GLShaderCg)
{

class csGLShader_CG : public scfImplementation2<csGLShader_CG, 
						iShaderProgramPlugin,
					        iComponent>
{
private:
  static void ErrorHandler (CGcontext context, CGerror err, void* appdata);
  static void ErrorHandlerObjReg (CGcontext context, CGerror err, void* appdata);

  bool enable;
  bool isOpen;
  const char* compiledProgram;
  bool doIgnoreErrors;
public:
  CS_LEAKGUARD_DECLARE (csGLShader_CG);

  iObjectRegistry* object_reg;
  CGcontext context;
  csRef<iShaderProgramPlugin> psplg;
  CGprofile psProfile;
  csGLExtensionManager* ext;
  bool debugDump;
  char* dumpDir;
  bool doVerbose;

  bool enableVP, enableFP;
  CGprofile maxProfileVertex;
  CGprofile maxProfileFragment;
  
  csBlockAllocator<ShaderParameter> paramAlloc;

  csGLShader_CG (iBase *parent);
  virtual ~csGLShader_CG ();

  void Report (int severity, const char* msg, ...) CS_GNUC_PRINTF(3, 4);
  
  /**\name iShaderProgramPlugin implementation
   * @{ */
  virtual csPtr<iShaderProgram> CreateProgram(const char* type) ;

  virtual bool SupportType(const char* type);

  bool Open();
  /** @} */

  /**\name iComponent implementation
   * @{ */
  bool Initialize (iObjectRegistry* reg);
  /** @} */

  void SplitArgsString (const char* str, ArgumentArray& args);
  void GetProfileCompilerArgs (const char* type, CGprofile profile, 
    ArgumentArray& args);
  static bool ProfileNeedsRouting (CGprofile profile)
  {
    return (profile >= CG_PROFILE_PS_1_1) && (profile <= CG_PROFILE_PS_1_3);
  }
  bool IsRoutedProfileSupported (CGprofile profile);
  
  void SetCompiledSource (const char* prog)
  { compiledProgram = prog; }
  void SetIgnoreErrors (bool doIgnore) { doIgnoreErrors = doIgnore; }
  void PrintCgListing (const char* listing);
  void PrintAnyListing ()
  {
    const char* listing = cgGetLastListing (context);
    if (listing && *listing && doVerbose)
    {
      PrintCgListing (listing);
    }
  }
};

}
CS_PLUGIN_NAMESPACE_END(GLShaderCg)

#endif //__GLSHADER_CG_H__
