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

#include "csgeom/vector3.h"
#include "csplugincommon/opengl/glextmanager.h"
#include "csutil/objreg.h"
#include "csutil/ref.h"
#include "csutil/scanstr.h"
#include "csutil/scf.h"
#include "csutil/stringreader.h"
#include "iutil/document.h"
#include "iutil/string.h"
#include "ivaria/reporter.h"
#include "ivideo/graph3d.h"
#include "ivideo/shader/shader.h"
#include "iutil/databuff.h"

#include "glshader_cgvp.h"
#include "glshader_cg.h"
#include "profile_limits.h"

CS_PLUGIN_NAMESPACE_BEGIN(GLShaderCg)
{

CS_LEAKGUARD_IMPLEMENT (csShaderGLCGVP);

bool csShaderGLCGVP::Compile (iHierarchicalCache* cache)
{
  if (!shaderPlug->enableVP) return false;

  csRef<iDataBuffer> programBuffer = GetProgramData();
  if (!programBuffer.IsValid())
    return false;
  csString programStr;
  programStr.Append ((char*)programBuffer->GetData(), programBuffer->GetSize());

  CGprofile progProf = CG_PROFILE_UNKNOWN;
  /* @@@ Hack: Make sure at least ARB_v_p is used.
   * This is done because we don't completely support NV_vertex_program based
   * profiles - those require "manual" binding of state matrices via 
   * glTrackMatrixNV() which we don't support right now.
   */
  if (!cg_profile.IsEmpty())
    progProf = cgGetProfile (cg_profile);
  
  if(progProf == CG_PROFILE_UNKNOWN)
    progProf = cgGLGetLatestProfile (CG_GL_VERTEX);
  if (progProf < CG_PROFILE_ARBVP1)
    cg_profile = "arbvp1";
  
  bool ret = DefaultLoadProgram (cgResolve, programStr, CG_GL_VERTEX, 
    shaderPlug->maxProfileVertex);

  ProfileLimits limits (programProfile);
  limits.GetCurrentLimits (shaderPlug->ext);
  WriteToCache (cache, limits);
  cacheKeepNodes.DeleteAll ();

  return ret;
}

bool csShaderGLCGVP::Precache (const ProfileLimits& limits,
                               iHierarchicalCache* cache)
{
  PrecacheClear();

  csRef<iDataBuffer> programBuffer = GetProgramData();
  if (!programBuffer.IsValid())
    return false;
  csString programStr;
  programStr.Append ((char*)programBuffer->GetData(), programBuffer->GetSize());

  bool needBuild = true;
  csString sourcePreproc;
  {
    csString programStr;
    programStr.Append ((char*)programBuffer->GetData(), programBuffer->GetSize());
    
    ArgumentArray args;
    shaderPlug->GetProfileCompilerArgs (GetProgramType(),
      limits.profile, true, args);
    for (size_t i = 0; i < compilerArgs.GetSize(); i++) 
      args.Push (compilerArgs[i]);
  
    // Get preprocessed result of pristine source
    sourcePreproc = GetPreprocessedProgram (programStr, args);
    if (!sourcePreproc.IsEmpty ())
    {
      // Check preprocessed source against cache
      if (TryLoadFromCompileCache (sourcePreproc, limits, cache))
        needBuild = false;
    }
  }
  
  bool ret;
  if (needBuild)
    ret = DefaultLoadProgram (cgResolve, programStr, CG_GL_VERTEX, 
      CG_PROFILE_UNKNOWN,
      loadApplyVmap | loadIgnoreConfigProgramOpts | loadFlagUnusedV2FForInit,
      &limits);
  else
    ret = true;

  // Store program against preprocessed source in cache
  {
    if (needBuild && !sourcePreproc.IsEmpty ())
      WriteToCompileCache (sourcePreproc, limits, cache);
  }

  WriteToCache (cache, limits);

  return ret;
}

csVertexAttrib csShaderGLCGVP::ResolveBufferDestination (const char* binding)
{
  csVertexAttrib dest = CS_VATTRIB_INVALID;
  if (program)
  {
    CGparameter parameter = cgGetNamedParameter (program, binding);
    if (parameter)
    {
      CGresource base = cgGetParameterBaseResource (parameter);
      int index = cgGetParameterResourceIndex (parameter);
      switch (base)
      {
        case CG_UNDEFINED:
          return CS_VATTRIB_UNUSED;
	case CG_TEX0: 
	case CG_TEXUNIT0:
	case CG_TEXCOORD0:
	  if ((index >= 0) && (index < 8))
            dest = (csVertexAttrib)(CS_VATTRIB_TEXCOORD0 + index);
	  break;
	case CG_ATTR0:
	  if ((index >= 0) && (index < 16))
            dest = (csVertexAttrib)(CS_VATTRIB_0 + index);
	  break;
	case CG_COL0:
	case CG_COLOR0:
	  if ((index >= 0) && (index < 2))
            dest = (csVertexAttrib)(CS_VATTRIB_PRIMARY_COLOR + index);
	  break;
	case CG_HPOS:
	case CG_POSITION0:
	  dest = CS_VATTRIB_POSITION;
	  break;
	case CG_BLENDWEIGHT0:
	  dest = CS_VATTRIB_WEIGHT;
	  break;
	case CG_NORMAL0:
	  dest = CS_VATTRIB_NORMAL;
	  break;
	case CG_FOG0:
	  dest = CS_VATTRIB_FOGCOORD;
	  break;
        default:
	  break;
      }
    }
  }

  return dest;
}

}
CS_PLUGIN_NAMESPACE_END(GLShaderCg)
