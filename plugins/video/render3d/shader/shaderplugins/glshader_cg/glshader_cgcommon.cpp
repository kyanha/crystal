/*
  Copyright (C) 2002-2005 by Marten Svanfeldt
			     Anders Stenberg
			     Frank Richter

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

#include "csplugincommon/opengl/glextmanager.h"
#include "ivaria/reporter.h"

#include "glshader_cg.h"
#include "glshader_cgcommon.h"

CS_PLUGIN_NAMESPACE_BEGIN(GLShaderCg)
{

CS_LEAKGUARD_IMPLEMENT (csShaderGLCGCommon);

csShaderGLCGCommon::csShaderGLCGCommon (csGLShader_CG* shaderPlug, 
					ProgramType type) :
  scfImplementationType (this, shaderPlug->object_reg), programType (type),
  assumedConstParams (0)
{
  validProgram = true;
  this->shaderPlug = shaderPlug;
  program = 0;

  InitTokenTable (xmltokens);
}

csShaderGLCGCommon::~csShaderGLCGCommon ()
{
  if (program)
    cgDestroyProgram (program);
    
  if (assumedConstParams != 0)
  {
    for(size_t i = 0; i < assumedConstParams->GetSize (); ++i)
    {
      VariableMapEntry& mapping = assumedConstParams->Get (i);
      
      ShaderParameter* param =
	reinterpret_cast<ShaderParameter*> (mapping.userVal);
      
      FreeShaderParam (param);
    }
    delete assumedConstParams;
  }

  for(size_t i = 0; i < variablemap.GetSize (); ++i)
  {
    VariableMapEntry& mapping = variablemap[i];
    
    ShaderParameter* param =
      reinterpret_cast<ShaderParameter*> (mapping.userVal);
    
    FreeShaderParam (param);
  }
}

void csShaderGLCGCommon::Activate()
{
  cgGLEnableProfile (programProfile);
  if (!cgGLIsProgramLoaded (program))
  {
    cgGLLoadProgram (program);
    PostCompileVmapProcess ();
  }
  cgGLBindProgram (program);
}

void csShaderGLCGCommon::Deactivate()
{
  cgGLDisableProfile (programProfile);
}

void csShaderGLCGCommon::SetupState (const CS::Graphics::RenderMesh* /*mesh*/,
                                     CS::Graphics::RenderMeshModes& /*modes*/,
                                     const csShaderVariableStack& stack)
{
  if ((clips.GetSize() > 0) && (programType == progVP))
  {
    uint clipFlags = 0;
    shaderPlug->clipPlanes.SetShaderVars (stack);
    
    csVector4 packDist[2];
    clipPackedDists[0]->GetValue (packDist[0]);
    clipPackedDists[1]->GetValue (packDist[1]);
    for (size_t c = 0; c < clips.GetSize(); c++)
    {
      const Clip& clip = clips[c];
	
      if (!clip.distance.IsConstant())
      {
	csVector4 v (GetParamVectorVal (stack, clip.distance,
	  csVector4 (0, 0, 0, 0)));
	float distVal = v[clip.distComp];
	if (clip.distNeg) distVal = -distVal;
	packDist[c/4][c%4] = distVal;
      }
      
      bool doClipping = false;
      /* Enable a clip plane if:
         - both plane and distance are constant
         - one is not constant and the SV value is present
         - both are not constant and the SV values are present
       */
      csVector4 v;
      bool constPlane = clip.plane.IsConstant();
      bool hasPlaneSV = !constPlane
        && GetParamVectorVal (stack, clip.plane, &v);
      bool constDist = clip.distance.IsConstant();
      bool hasDistSV = !constDist
        && GetParamVectorVal (stack, clip.distance, &v);
      doClipping = (constPlane && constDist)
        || (constPlane && hasDistSV)
        || (hasPlaneSV && constDist)
        || (hasPlaneSV && hasDistSV);
      if (doClipping) clipFlags |= 1 << c;
    }
    clipPackedDists[0]->SetValue (packDist[0]);
    clipPackedDists[1]->SetValue (packDist[1]);
  
    if ((programProfile == CG_PROFILE_VP30)
        || (programProfile == CG_PROFILE_VP40)
        || (programProfile == CG_PROFILE_GPU_VP))
    {
      for (size_t c = 0; c < clips.GetSize(); c++)
      {
	if (clipFlags & (1 << c))
          shaderPlug->clipPlanes.EnableClipPlane (c);
      }
    }
    else if (programProfile == CG_PROFILE_ARBVP1)
    {
      for (size_t c = 0; c < clips.GetSize(); c++)
      {
	const Clip& clip = clips[c];
	
	if (clipFlags & (1 << c))
	{
	  csVector4 v (
	   GetParamVectorVal (stack, clip.plane, csVector4 (0, 0, 0, 0)));
	  csPlane3 plane (v.x, v.y, v.z, v.w);
	  float distVal = packDist[c/4][c%4];
	  plane.DD -= distVal / plane.Normal().Norm();
	  shaderPlug->clipPlanes.AddClipPlane (v, clip.space);
	}
      }
    }
  }
  
  ApplyVariableMapArrays (stack);
}

void csShaderGLCGCommon::ResetState()
{
  if ((clips.GetSize() > 0) && (programType == progVP))
  {
    shaderPlug->clipPlanes.DisableClipPlanes();
  }
}
  
void csShaderGLCGCommon::EnsureDumpFile()
{
  if (debugFN.IsEmpty())
  {
    static int programCounter = 0;
    
    const char* progTypeStr ="";
    switch (programType)
    {
      case progVP: progTypeStr = "cgvp"; break;
      case progFP: progTypeStr = "cgfp"; break;
    }
    
    csRef<iVFS> vfs = csQueryRegistry<iVFS> (objectReg);
    csString filename;
    filename << shaderPlug->dumpDir << (programCounter++) << 
      progTypeStr << ".txt";
    debugFN = filename;
    vfs->DeleteFile (debugFN);
  }
}

bool csShaderGLCGCommon::DefaultLoadProgram (
  iShaderDestinationResolverCG* cgResolve,
  const char* programStr, CGGLenum type, CGprofile maxProfile, 
  uint flags)
{
  if (!programStr || !*programStr) return false;

  size_t i;
  csString augmentedProgramStr;
  const csSet<csString>* unusedParams = &this->unusedParams;
  if (cgResolve != 0)
  {
    unusedParams = &cgResolve->GetUnusedParameters ();
  }
  csSet<csString>::GlobalIterator unusedIt (unusedParams->GetIterator());
  while (unusedIt.HasNext())
  {
    const csString& param = unusedIt.Next ();
    augmentedProgramStr.AppendFmt ("#define %s\n",
      param.GetData());
  }
  
  OutputClipPreamble (augmentedProgramStr);
  WriteClipApplications (augmentedProgramStr);
  
  augmentedProgramStr.Append (programStr);
  programStr = augmentedProgramStr;
  CGprofile profile = CG_PROFILE_UNKNOWN;

  if (!cg_profile.IsEmpty())
    profile = cgGetProfile (cg_profile);

  if(profile == CG_PROFILE_UNKNOWN)
    profile = cgGLGetLatestProfile (type);

  if (maxProfile != CG_PROFILE_UNKNOWN)
    profile = csMin (profile, maxProfile);

  if (shaderPlug->doVerbose)
  {
    shaderPlug->Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Cg %s program '%s': using profile %s[%d]", GetProgramType(),
      description.GetData (), cgGetProfileString (profile), profile);
  }

  ArgumentArray args;
  shaderPlug->GetProfileCompilerArgs (GetProgramType(), profile, args);
  for (i = 0; i < compilerArgs.GetSize(); i++) 
    args.Push (compilerArgs[i]);
  /* Work around Cg 2.0 bug: it emits "OPTION ARB_position_invariant;"
     AND computes result.position in the VP - doing both is verboten.
     Remedy: remove -posinv argument 
     (cgc version 2.0.0010)
   */
  if (profile == CG_PROFILE_GPU_VP)
  {
    for (i = 0; i < args.GetSize(); ) 
    {
      if (strcmp (args[i], "-posinv") == 0)
	args.DeleteIndex (i);
      else
	i++;
    }
  }
  args.Push (0);
 
  if (program)
  {
    cgDestroyProgram (program);
  }
  shaderPlug->SetCompiledSource (programStr);
  program = cgCreateProgram (shaderPlug->context, 
    (flags & loadPrecompiled) ? CG_OBJECT : CG_SOURCE, programStr, 
    profile, !entrypoint.IsEmpty() ? entrypoint : "main", args.GetArray());
  
  if (!(flags & loadIgnoreErrors)) shaderPlug->PrintAnyListing();

  if (!program)
  {
    shaderPlug->SetCompiledSource (0);
    /*if (shaderPlug->debugDump)
    {
      EnsureDumpFile();
      WriteAdditionalDumpInfo ("Failed program source", programStr);
    }*/
    return false;
  }
  programProfile = cgGetProgramProfile (program);

  if (flags & loadApplyVmap)
  {
    i = 0;
    while (i < variablemap.GetSize ())
    {
      // Get the Cg parameter
      CGparameter param = cgGetNamedParameter (program, 
	variablemap[i].destination);
      ShaderParameter* sparam =
	reinterpret_cast<ShaderParameter*> (variablemap[i].userVal);
  
      if (!param /*||
	  (cgGetParameterType (param) != CG_ARRAY && !cgIsParameterReferenced (param))*/)
      {
	variablemap.DeleteIndex (i);
	FreeShaderParam (sparam);
	continue;
      }
      FillShaderParam (sparam, param);
      bool assumeConst = sparam->assumeConstant;
      if (assumeConst)
      {
        if (assumedConstParams == 0)
          assumedConstParams = new csSafeCopyArray<VariableMapEntry>;
	assumedConstParams->Push (variablemap[i]);
	variablemap.DeleteIndex (i);
	continue;
      }
      // Mark constants as to be folded in
      if (variablemap[i].mappingParam.IsConstant())
      {
	csShaderVariable* var = variablemap[i].mappingParam.var;
	if (var != 0)
	  SetParameterValueCg (sparam, var);
	cgSetParameterVariability (param, CG_LITERAL);
	variablemap.DeleteIndex (i);
	FreeShaderParam (sparam);
	continue;
      }
      i++;
    }
  
    variablemap.ShrinkBestFit();
  }

  if (flags & loadIgnoreErrors) shaderPlug->SetIgnoreErrors (true);
  cgCompileProgram (program);
  if (flags & loadIgnoreErrors)
    shaderPlug->SetIgnoreErrors (false);
  else
    shaderPlug->PrintAnyListing();
  
  if (flags & loadApplyVmap)
    PostCompileVmapProcess ();

  if (flags & loadLoadToGL)
  {
    cgGetError(); // Clear error
    cgGLLoadProgram (program);
    if (!(flags & loadIgnoreErrors)) shaderPlug->PrintAnyListing();
    if ((cgGetError() != CG_NO_ERROR)
      || !cgGLIsProgramLoaded (program)) 
    {
      //if (shaderPlug->debugDump)
	//DoDebugDump();

      if (shaderPlug->doVerbose
	  && ((type == CG_GL_VERTEX) && (profile >= CG_PROFILE_ARBVP1))
	    || ((type == CG_GL_FRAGMENT) && (profile >= CG_PROFILE_ARBFP1)))
      {
	const char* err = (char*)glGetString (GL_PROGRAM_ERROR_STRING_ARB);
	shaderPlug->Report (CS_REPORTER_SEVERITY_WARNING,
	  "OpenGL error string: %s", err);
      }

      shaderPlug->SetCompiledSource (0);
      return false;
    }
  }

  if (shaderPlug->debugDump)
    DoDebugDump();
  
  shaderPlug->SetCompiledSource (0);

  bool result = true;
  if (programType == progFP)
  {
    int numVaryings = 0;
    CGparameter param = cgGetFirstLeafParameter (program, CG_PROGRAM);
    while (param)
    {
      if (cgIsParameterUsed (param, program)
	&& cgIsParameterReferenced (param))
      {
	const CGenum var = cgGetParameterVariability (param);
	if (var == CG_VARYING)
	  numVaryings++;
      }
  
      param = cgGetNextLeafParameter (param);
    }
    
    /* WORKAROUND: Even NVs G80 doesn't support passing more than 16 attribs
       into an FP, yet Cg happily generates code that uses more (and GL falls 
       back to SW).
       So manually check the number of varying inputs and reject more than 16.
       
       @@@ This should be at least configurable
     */
    result = numVaryings <= 16;
  }
  if (!result && !debugFN.IsEmpty())
  {
    csRef<iVFS> vfs = csQueryRegistry<iVFS> (objectReg);
    vfs->DeleteFile (debugFN);
  }
  return result;
}

void csShaderGLCGCommon::DoDebugDump ()
{
  csString output;
  DumpProgramInfo (output);
  output << "CG program type: " << programType << "\n";
  output << "CG profile: " << cgGetProgramString (program, 
    CG_PROGRAM_PROFILE) << "\n";
  output << "CG entry point: " << (entrypoint ? entrypoint : "main") << 
    "\n";
  output << "CG program valid: " << validProgram << "\n";
  output << "\n";

  output << "Variable mappings:\n";
  for (size_t v = 0; v < variablemap.GetSize (); v++)
  {
    const VariableMapEntry& vme = variablemap[v];
    ShaderParameter* sparam =
      reinterpret_cast<ShaderParameter*> (vme.userVal);

    output << stringsSvName->Request (vme.name);
    output << '(' << vme.name << ") -> ";
    output << vme.destination << ' ';
    if (sparam == 0)
    {
      output << "(null)";
    }
    else
    {
      if (sparam->paramType != 0) output << cgGetTypeString (sparam->paramType) << ' ';
      if (sparam->param != 0) output << cgGetParameterName (sparam->param) << "  ";
      output << "baseslot " << sparam->baseSlot;
      if (sparam->assumeConstant) output << "  assumed constant";
    }
    output << '\n'; 
  }
  output << "\n";

  CGparameter param = cgGetFirstLeafParameter (program, CG_PROGRAM);
  while (param)
  {
    output << "Parameter: " << cgGetParameterName (param) << "\n";
    output << " Type: " << 
      cgGetTypeString (cgGetParameterNamedType (param)) << "\n";
    output << " Direction: " <<
      cgGetEnumString (cgGetParameterDirection (param)) << "\n";
    output << " Semantic: " << cgGetParameterSemantic (param) << "\n";
    const CGenum var = cgGetParameterVariability (param);
    output << " Variability: " << cgGetEnumString (var) << "\n";
    output << " Resource: " <<
      cgGetResourceString (cgGetParameterResource (param)) << "\n";
    output << " Resource index: " <<
      cgGetParameterResourceIndex (param) << "\n";
    // Cg 2.0 seems to not like CG_DEFAULT for uniforms
    if (/*(var == CG_UNIFORM) || */(var == CG_CONSTANT))
    {
      int nValues;
      const double* values = cgGetParameterValues (param, 
	(var == CG_UNIFORM) ? CG_DEFAULT : CG_CONSTANT, &nValues);
      if (nValues != 0)
      {
	output << " Values:";
	for (int v = 0; v < nValues; v++)
	{
	  output << ' ' << values[v];
	}
	output << "\n";
      }
    }
    if (!cgIsParameterUsed (param, program)) output << "  not used\n";
    if (!cgIsParameterReferenced (param)) output << "  not referenced\n";

    param = cgGetNextLeafParameter (param);
  }
  output << "\n";

  output << "Program source:\n";
  output << cgGetProgramString (program, CG_PROGRAM_SOURCE);
  output << "\n";

  output << "Compiled program:\n";
  output << cgGetProgramString (program, CG_COMPILED_PROGRAM);
  output << "\n";

  csRef<iVFS> vfs = csQueryRegistry<iVFS> (objectReg);
  EnsureDumpFile();

  csRef<iFile> debugFile = vfs->Open (debugFN, VFS_FILE_APPEND);
  if (!debugFile)
  {
    csReport (objectReg, CS_REPORTER_SEVERITY_WARNING, 
      "crystalspace.graphics3d.shader.glcg",
      "Could not write '%s'", debugFN.GetData());
  }
  else
  {
    debugFile->Write (output.GetData(), output.Length ());
    csReport (objectReg, CS_REPORTER_SEVERITY_NOTIFY, 
      "crystalspace.graphics3d.shader.glcg",
      "Dumped Cg program info to '%s'", debugFN.GetData());
  }
}

void csShaderGLCGCommon::WriteAdditionalDumpInfo (const char* description, 
						  const char* content)
{
  if (!shaderPlug->debugDump || !debugFN) return;

  csRef<iVFS> vfs = csQueryRegistry<iVFS> (objectReg);
  csRef<iDataBuffer> oldDump = vfs->ReadFile (debugFN, true);

  csString output (oldDump ? (char*)oldDump->GetData() : 0);
  output << description << ":\n";
  output << content;
  output << "\n";
  if (!vfs->WriteFile (debugFN, output.GetData(), output.Length ()))
  {
    csReport (objectReg, CS_REPORTER_SEVERITY_WARNING, 
      "crystalspace.graphics3d.shader.glcg",
      "Could not augment '%s'", debugFN.GetData());
  }
}

}
CS_PLUGIN_NAMESPACE_END(GLShaderCg)
