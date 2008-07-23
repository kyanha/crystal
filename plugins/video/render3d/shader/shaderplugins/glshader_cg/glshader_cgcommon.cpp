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
#include "csplugincommon/opengl/glstates.h"
#include "csplugincommon/shader/shadercachehelper.h"
#include "csutil/csendian.h"
#include "csutil/documenthelper.h"
#include "csutil/memfile.h"
#include "csutil/scfstr.h"
#include "iutil/hiercache.h"
#include "iutil/stringarray.h"
#include "ivaria/reporter.h"

#include "glshader_cg.h"
#include "glshader_cgcommon.h"
#include "profile_limits.h"

// Enabling this preprocesses Cg sources with boost::Wave
//#define PREPROCESS_WITH_WAVE

#ifdef PREPROCESS_WITH_WAVE
#include <string>
#include <vector>

#include <boost/wave.hpp>

#include <boost/wave/cpplexer/cpp_lex_token.hpp>
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp>
#endif

CS_PLUGIN_NAMESPACE_BEGIN(GLShaderCg)
{

CS_LEAKGUARD_IMPLEMENT (csShaderGLCGCommon);

csShaderGLCGCommon::csShaderGLCGCommon (csGLShader_CG* shaderPlug, 
					ProgramType type) :
  scfImplementationType (this, shaderPlug->object_reg), programType (type)
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
  #if 0
  if (!cgGLIsProgramLoaded (program))
  {
    cgGLLoadProgram (program);
    PostCompileVmapProcess ();
  }
  #endif
  cgGLBindProgram (program);
  
  if (shaderPlug->ext->CS_GL_ARB_color_buffer_float)
  {
    shaderPlug->statecache->SetClampColor (GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);
  }
}

void csShaderGLCGCommon::Deactivate()
{
  cgGLDisableProfile (programProfile);

  if (shaderPlug->ext->CS_GL_ARB_color_buffer_float)
  {
    shaderPlug->statecache->SetClampColor (GL_CLAMP_VERTEX_COLOR_ARB, GL_TRUE);
  }
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

void csShaderGLCGCommon::PrecacheClear()
{
  if (program != 0)
  {
    cgDestroyProgram (program);
    program = 0;
  }
  objectCode.Empty();
  objectCodeCachePath.Empty();
}

bool csShaderGLCGCommon::DefaultLoadProgram (iShaderProgramCG* cgResolve,
  const char* programStr, CGGLenum type, CGprofile maxProfile, 
  uint flags, const ProfileLimits* customLimits)
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
  
  if (flags & loadFlagUnusedV2FForInit)
    augmentedProgramStr.Append ("#define _INITIALIZE_UNUSED_V2F\n");
  
  OutputClipPreamble (augmentedProgramStr);
  WriteClipApplications (augmentedProgramStr);
  
  augmentedProgramStr.Append (programStr);
  programStr = augmentedProgramStr;
  CGprofile profile = CG_PROFILE_UNKNOWN;
  CS::PluginCommon::ShaderProgramPluginGL::HardwareVendor vendor;

  if (customLimits != 0)
  {
    profile = customLimits->profile;
    vendor = customLimits->vendor;
  }
  else
  {
    if (!cg_profile.IsEmpty())
      profile = cgGetProfile (cg_profile);
  
    if(profile == CG_PROFILE_UNKNOWN)
      profile = cgGLGetLatestProfile (type);
  
    if (maxProfile != CG_PROFILE_UNKNOWN)
      profile = csMin (profile, maxProfile);
      
    vendor = shaderPlug->vendor;
  }

  if (shaderPlug->doVerbose || shaderPlug->doVerbosePrecache)
  {
    shaderPlug->Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Cg %s program '%s': using profile %s[%d]", GetProgramType(),
      description.GetData (), cgGetProfileString (profile), profile);
  }

  ArgumentArray args;
  shaderPlug->GetProfileCompilerArgs (GetProgramType(), profile, 
    vendor, flags & loadIgnoreConfigProgramOpts, args);
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
  if (customLimits != 0) customLimits->ToCgOptions (args);
  args.Push (0);
 
  if (program)
  {
    cgDestroyProgram (program);
  }
  shaderPlug->SetCompiledSource (programStr);
  program = cgCreateProgram (shaderPlug->context, 
    CG_SOURCE, programStr, 
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
    GetParamsFromVmap();

  if (flags & loadIgnoreErrors) shaderPlug->SetIgnoreErrors (true);
  cgCompileProgram (program);
  if (flags & loadIgnoreErrors)
    shaderPlug->SetIgnoreErrors (false);
  else
    shaderPlug->PrintAnyListing();
  
  if (flags & loadApplyVmap)
    GetPostCompileParamProps ();

  if (flags & loadLoadToGL)
  {
    cgGetError(); // Clear error
    cgGLLoadProgram (program);
    if (!(flags & loadIgnoreErrors)) shaderPlug->PrintAnyListing();
    if ((cgGetError() != CG_NO_ERROR)
      || !cgGLIsProgramLoaded (program)) 
    {
      if (shaderPlug->debugDump)
	DoDebugDump();

      if (shaderPlug->doVerbose
	  && ((type == CG_GL_VERTEX) && (profile >= CG_PROFILE_ARBVP1))
	    || ((type == CG_GL_FRAGMENT) && (profile >= CG_PROFILE_ARBFP1)))
      {
	csString err = (char*)glGetString (GL_PROGRAM_ERROR_STRING_ARB);
	if (!err.IsEmpty())
	  shaderPlug->Report (CS_REPORTER_SEVERITY_WARNING,
	    "OpenGL error string: %s", err.GetData());
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

csString csShaderGLCGCommon::GetPreprocessedProgram (const char* programStr, 
  const ArgumentArray& args)
{
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
  
  csString result;
  
  csStringArray macros;
  for (size_t i = 0; i < args.GetSize(); i++)
  {
    const char* s = args[i];
    if (s && (strlen (s) > 2) && (s[0] == '-') && (s[1] == 'D'))
      macros.Push (s + 2);
  }
  
#ifdef PREPROCESS_WITH_WAVE
  /* This code is mostly stolen^Wadapted from the boost::Wave quickstart
      example */

  // current file position is saved for exception handling
  boost::wave::util::file_position_type current_position;

  try
  {
      //  The template boost::wave::cpplexer::lex_token<> is the token type to be 
      //  used by the Wave library.
      typedef boost::wave::cpplexer::lex_token<> token_type;
    
      //  The template boost::wave::cpplexer::lex_iterator<> is the lexer type to
      //  be used by the Wave library.
      typedef boost::wave::cpplexer::lex_iterator<token_type> lex_iterator_type;
	
      //  This is the resulting context type to use. The first template parameter
      //  should match the iterator type to be used during construction of the
      //  corresponding context object (see below).
      typedef boost::wave::context<std::string::iterator, lex_iterator_type>
	  context_type;
	  
      // To work around Wave warning w/ missing newline at EOF
      std::string instring (augmentedProgramStr);
      instring.append ("\n");

      // The preprocessor iterator shouldn't be constructed directly. It is 
      // to be generated through a wave::context<> object. This wave:context<> 
      // object is to be used additionally to initialize and define different 
      // parameters of the actual preprocessing.
      //
      // The preprocessing of the input stream is done on the fly behind the 
      // scenes during iteration over the context_type::iterator_type stream.
      context_type ctx (instring.begin(), instring.end(), "*buffer*");
      
      for (size_t i = 0; i < macros.GetSize(); i++)
      {
        ctx.add_macro_definition (macros[i], false);
      }

      // analyze the input file
      context_type::iterator_type first = ctx.begin();
      context_type::iterator_type last = ctx.end();
	
      while (first != last)
      {
	  current_position = (*first).get_position();
	  result << (*first).get_value().c_str();
	  ++first;
      }
  }
  catch (boost::wave::cpp_exception const& e)
  {
      // some preprocessing error
      std::cerr 
	  << e.file_name() << "(" << e.line_no() << "): "
	  << e.description() << std::endl;
      if (e.get_severity() >= boost::wave::util::severity_error)
	result = (char*)0;
  }
  catch (std::exception const& e)
  {
      // use last recognized token to retrieve the error position
      std::cerr 
	  << current_position.get_file() 
	  << "(" << current_position.get_line() << "): "
	  << "exception caught: " << e.what()
	  << std::endl;
      result = (char*)0;
  }
  catch (...)
  {
      // use last recognized token to retrieve the error position
      std::cerr 
	  << current_position.get_file() 
	  << "(" << current_position.get_line() << "): "
	  << "unexpected exception caught." << std::endl;
      result = (char*)0;
  }
#endif
  
  if (result.IsEmpty())
  {
    /* "Fake" preprocessing: since the output is only used for hashing,
       return a result that is different for each source/macros combination. */
       
    for (size_t i = 0; i < macros.GetSize(); i++)
    {
      result.AppendFmt ("// @@MACRO %s\n", macros[i]);
    }
    result += augmentedProgramStr;
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

/* Magic value for cg program files.
 * The most significant byte serves as a "version", increase when the
 * cache file format changes. */
static const uint32 cacheFileMagic = 0x04706763;

enum
{
  cpsValid = 0x6b726f77,
  cpsInvalid = 0x6b723062
};

bool csShaderGLCGCommon::WriteToCache (iHierarchicalCache* cache,
                                       const ProfileLimits& limits,
                                       const char* tag)
{
  if (!cache) return false;

  csMemFile cacheFile;
  
  uint32 diskMagic = csLittleEndian::UInt32 (cacheFileMagic);
  if (cacheFile.Write ((char*)&diskMagic, sizeof (diskMagic))
      != sizeof (diskMagic)) return false;
  
  csString objectCode (this->objectCode);
  if ((program != 0) && objectCode.IsEmpty())
    objectCode = cgGetProgramString (program, CG_COMPILED_PROGRAM);
  /*const char* object;
  if ((program == 0)
    || ((object = cgGetProgramString (program, CG_COMPILED_PROGRAM)) == 0)
    || (*object == 0))*/
  if ((program == 0) || objectCode.IsEmpty())
  {
    uint32 diskState = csLittleEndian::UInt32 (cpsInvalid);
    if (cacheFile.Write ((char*)&diskState, sizeof (diskState))
	!= sizeof (diskState)) return false;
  }
  else
  {
    {
      uint32 diskState = csLittleEndian::UInt32 (cpsValid);
      if (cacheFile.Write ((char*)&diskState, sizeof (diskState))
	  != sizeof (diskState)) return false;
    }
    
    CS::PluginCommon::ShaderCacheHelper::WriteString (&cacheFile, description);
    
    csRef<iDocumentSystem> docsys = shaderPlug->binDocSys;
    if (!docsys.IsValid()) docsys = shaderPlug->xmlDocSys;
    csRef<iDocument> doc = docsys->CreateDocument();
    csRef<iDocumentNode> root = doc->CreateRoot();
    for (size_t i = 0; i < cacheKeepNodes.GetSize(); i++)
    {
      csRef<iDocumentNode> newNode = root->CreateNodeBefore (
	cacheKeepNodes[i]->GetType());
      CS::DocSystem::CloneNode (cacheKeepNodes[i], newNode);
    }
    
    {
      csMemFile docFile;
      const char* err = doc->Write (&docFile);
      if (err != 0)
      {
	csReport (objectReg, CS_REPORTER_SEVERITY_WARNING, 
	  "crystalspace.graphics3d.shader.glcg",
	  "Error writing document: %s", err);
      }
      csRef<iDataBuffer> docBuf = docFile.GetAllData (false);
      if (!CS::PluginCommon::ShaderCacheHelper::WriteDataBuffer (&cacheFile,
	  docBuf))
	return false;
    }
    
    if (objectCodeCachePath.IsEmpty())
    {
      if (!WriteToCompileCache (limits, cache))
        return false;
    }
    
    CS_ASSERT(!objectCodeCachePath.IsEmpty());
    CS::PluginCommon::ShaderCacheHelper::WriteString (&cacheFile,
      objectCodeCachePath);
  }
  
  csString cacheName ("/");
  cacheName += tag;
  return cache->CacheData (cacheFile.GetData(), cacheFile.GetSize(),
    cacheName);
}

struct CachedShaderWrapper
{
  csString name;
  csRef<iFile> cacheFile;
  ProfileLimitsPair limits;
  
  CachedShaderWrapper (iFile* file) : cacheFile (file) {}
    
  bool operator< (const CachedShaderWrapper& other) const
  { return limits < other.limits; }
};

iShaderProgram::CacheLoadResult csShaderGLCGCommon::LoadFromCache (
  iHierarchicalCache* cache, csRef<iString>* failReason,
  csRef<iString>* tag)
{
  if (!cache) return iShaderProgram::loadFail;

  csRef<iStringArray> allCachedPrograms = cache->GetSubItems ("/");
  if (!allCachedPrograms.IsValid())
  {
    if (failReason) failReason->AttachNew (
      new scfString ("no cached programs found"));
    return iShaderProgram::loadFail;
  }
  
  csArray<CachedShaderWrapper> cachedProgWrappers;
  for (size_t i = 0; i < allCachedPrograms->GetSize(); i++)
  {
    csString cachePath ("/");
    cachePath.Append (allCachedPrograms->Get (i));
    csRef<iDataBuffer> cacheBuf = cache->ReadCache (cachePath);
    if (!cacheBuf.IsValid()) continue;
    
    csRef<iFile> cacheFile;
    cacheFile.AttachNew (new csMemFile (cacheBuf, true));
  
    uint32 diskMagic;
    if (cacheFile->Read ((char*)&diskMagic, sizeof (diskMagic))
	!= sizeof (diskMagic)) continue;
    if (csLittleEndian::UInt32 (diskMagic) != cacheFileMagic)
      continue;
      
    CachedShaderWrapper wrapper (cacheFile);
    
    wrapper.name = allCachedPrograms->Get (i);
    if (!wrapper.limits.FromString (wrapper.name)) continue;
    
    cachedProgWrappers.Push (wrapper);
  }
  
  if (cachedProgWrappers.GetSize() == 0)
  {
    if (failReason) failReason->AttachNew (
      new scfString ("all cached programs failed to read"));
    return iShaderProgram::loadFail;
  }
  
  cachedProgWrappers.Sort ();

  csString allReasons;
  bool oneReadCorrectly = false;
  for (size_t i = cachedProgWrappers.GetSize(); i-- > 0;)
  {
    const CachedShaderWrapper& wrapper = cachedProgWrappers[i];
    const ProfileLimits& limits =
      (programType == progVP) ? wrapper.limits.vp : wrapper.limits.fp;
  
    bool profileSupported =
      (shaderPlug->ProfileNeedsRouting (limits.profile)
        && shaderPlug->IsRoutedProfileSupported (limits.profile))
      || cgGLIsProfileSupported (limits.profile);
    if (!profileSupported)
    {
      allReasons += wrapper.name;
      allReasons += ": Profile unsupported; ";
      continue;
    }
    
    ProfileLimits currentLimits (shaderPlug->vendor,
      limits.profile);
    currentLimits.GetCurrentLimits (shaderPlug->ext);
    
    if ((limits.vendor != currentLimits.vendor)
        && (limits.vendor != CS::PluginCommon::ShaderProgramPluginGL::Other))
    {
      allReasons += wrapper.name;
      allReasons += ": vendor mismatch; ";
      continue;
    }
    
    bool limitsSupported = currentLimits >= limits;
    if (!limitsSupported)
    {
      allReasons += wrapper.name;
      allReasons += ": Limits exceeded; ";
      continue;
    }
    
    iFile* cacheFile = wrapper.cacheFile;
    
    {
      uint32 diskState;
      if (cacheFile->Read ((char*)&diskState, sizeof (diskState))
	  != sizeof (diskState)) continue;
      if (csLittleEndian::UInt32 (diskState) != cpsValid)
      {
        oneReadCorrectly = true;
        continue;
      }
    }
  
    
    description = CS::PluginCommon::ShaderCacheHelper::ReadString (cacheFile);
    
    csRef<iDataBuffer> docBuf =
      CS::PluginCommon::ShaderCacheHelper::ReadDataBuffer (cacheFile);
    if (!docBuf.IsValid()) continue;
    
    csRef<iDocument> doc;
    if (shaderPlug->binDocSys.IsValid())
    {
      doc = shaderPlug->binDocSys->CreateDocument ();
      const char* err = doc->Parse (docBuf);
      if (err != 0)
      {
	csReport (objectReg, CS_REPORTER_SEVERITY_WARNING, 
	  "crystalspace.graphics3d.shader.glcg",
	  "Error reading document: %s", err);
      }
    }
    if (!doc.IsValid() && shaderPlug->xmlDocSys.IsValid())
    {
      doc = shaderPlug->xmlDocSys->CreateDocument ();
      const char* err = doc->Parse (docBuf);
      if (err != 0)
      {
	csReport (objectReg, CS_REPORTER_SEVERITY_WARNING, 
	  "crystalspace.graphics3d.shader.glcg",
	  "Error reading document: %s", err);
      }
    }
    if (!doc.IsValid()) continue;
    
    csRef<iDocumentNode> root = doc->GetRoot();
    if (!root.IsValid()) continue;
    
    bool breakFail = false;
    csRef<iDocumentNodeIterator> nodes = root->GetNodes();
    while(nodes->HasNext() && !breakFail)
    {
      csRef<iDocumentNode> child = nodes->Next();
      if(child->GetType() != CS_NODE_ELEMENT) continue;
      const char* value = child->GetValue ();
      csStringID id = xmltokens.Request (value);
      switch(id)
      {
	case XMLTOKEN_VARIABLEMAP:
	  if (!ParseVmap (child))
	    breakFail = true;
	  break;
	case XMLTOKEN_CLIP:
	  if (!ParseClip (child))
	    breakFail = true;
	  break;
	default:
	  breakFail = true;
      }
    }
    if (breakFail) continue;
    
    objectCodeCachePath =
      CS::PluginCommon::ShaderCacheHelper::ReadString (cacheFile);
    if (objectCodeCachePath.IsEmpty()) continue;
    
    if (!LoadObjectCodeFromCompileCache (limits, cache))
      continue;
    
    oneReadCorrectly = true;
    if (program)
    {
      cgDestroyProgram (program);
      program = 0;
    }
    
    if (objectCode.IsEmpty()) continue;
    
    cgGetError(); // Clear error
    program = cgCreateProgram (shaderPlug->context, 
      CG_OBJECT, objectCode, limits.profile, 0, 0);
    if (!program) continue;
    CGerror err = cgGetError();
    if (err != CG_NO_ERROR)
    {
      const char* errStr = cgGetErrorString (err);
      shaderPlug->Report (CS_REPORTER_SEVERITY_WARNING,
	"Cg error %s", errStr);
      continue;
    }
    programProfile = limits.profile;
    
    ClipsToVmap();
    GetParamsFromVmap();
    
    cgGetError(); // Clear error
    cgGLLoadProgram (program);
    shaderPlug->PrintAnyListing();
    err = cgGetError();
    if ((err != CG_NO_ERROR)
      || !cgGLIsProgramLoaded (program)) 
    {
      //if (shaderPlug->debugDump)
	//DoDebugDump();
	
      const char* errStr = cgGetErrorString (err);
      shaderPlug->Report (CS_REPORTER_SEVERITY_WARNING,
	"Cg error %s", errStr);
  
      if (shaderPlug->doVerbose
	  && ((programType == progVP) && (programProfile >= CG_PROFILE_ARBVP1))
	    || ((programType == progFP) && (programProfile >= CG_PROFILE_ARBFP1)))
      {
	const char* err = (char*)glGetString (GL_PROGRAM_ERROR_STRING_ARB);
	shaderPlug->Report (CS_REPORTER_SEVERITY_WARNING,
	  "OpenGL error string: %s", err);
      }
  
      shaderPlug->SetCompiledSource (0);
      continue;
    }
    
    GetPostCompileParamProps ();
    
    if (shaderPlug->debugDump)
      DoDebugDump();
      
    tag->AttachNew (new scfString (wrapper.limits.ToString()));
    
    return iShaderProgram::loadSuccessShaderValid;
  }
  
  if (failReason) failReason->AttachNew (
    new scfString (allReasons));
  return oneReadCorrectly ? iShaderProgram::loadSuccessShaderInvalid : iShaderProgram::loadFail;
}

static const uint32 cacheFileMagicCC = 0x02637063;

bool csShaderGLCGCommon::TryLoadFromCompileCache (const char* source, 
                                                  const ProfileLimits& limits,
                                                  iHierarchicalCache* cache)
{
  csString objectCodeCachePath;
  
  iHierarchicalCache* rootCache = cache->GetTopCache();
  csMD5::Digest sourceMD5 = csMD5::Encode (source);
  csString cachePath;
  cachePath.Format ("/CgProgCache/%s/%s",
    sourceMD5.HexString().GetData(), limits.ToString().GetData());
  csRef<iHierarchicalCache> progCache = rootCache->GetRootedCache (cachePath);
  
  csRef<iFile> foundFile;
  csRef<iStringArray> cachedVersions = progCache->GetSubItems ("/");
  for (size_t i = 0; i < cachedVersions->GetSize(); i++)
  {
    csString cachePath2 ("/");
    cachePath2.Append (cachedVersions->Get (i));
    csRef<iDataBuffer> cacheBuf = progCache->ReadCache (cachePath2);
    if (!cacheBuf.IsValid()) continue;
    
    csRef<iFile> cacheFile;
    cacheFile.AttachNew (new csMemFile (cacheBuf, true));
  
    uint32 diskMagic;
    if (cacheFile->Read ((char*)&diskMagic, sizeof (diskMagic))
	!= sizeof (diskMagic)) continue;
    if (csLittleEndian::UInt32 (diskMagic) != cacheFileMagicCC)
      continue;
    
    csString cachedSource =
      CS::PluginCommon::ShaderCacheHelper::ReadString (cacheFile);
      
    if (cachedSource == source)
    {
      foundFile = cacheFile;
      objectCodeCachePath.Format ("/%s/%s/%s",
        sourceMD5.HexString().GetData(), limits.ToString().GetData(),
        cachedVersions->Get (i));
      break;
    }
  }
  
  if (!foundFile.IsValid()) return false;
  
  {
    uint32 diskState;
    if (foundFile->Read ((char*)&diskState, sizeof (diskState))
	!= sizeof (diskState)) return false;
    if (csLittleEndian::UInt32 (diskState) != cpsValid)
    {
      if (program) cgDestroyProgram (program);
      program = 0;
      return true;
    }
  }
  
  objectCode =
    CS::PluginCommon::ShaderCacheHelper::ReadString (foundFile);
  if (objectCode.IsEmpty()) return false;
  
  unusedParams.DeleteAll();
  {
    csString p;
    while (!(p = CS::PluginCommon::ShaderCacheHelper::ReadString (foundFile))
      .IsEmpty())
    {
      unusedParams.Add (p);
    }
  }

  if (program)
  {
    cgDestroyProgram (program);
  }
  cgGetError(); // Clear error
  program = cgCreateProgram (shaderPlug->context, 
    CG_OBJECT, objectCode, limits.profile, 0, 0);
  if (!program) return false;
  CGerror err = cgGetError();
  if (err != CG_NO_ERROR)
  {
    const char* errStr = cgGetErrorString (err);
    shaderPlug->Report (CS_REPORTER_SEVERITY_WARNING,
      "Cg error %s", errStr);
    return false;
  }
  
  this->objectCodeCachePath = objectCodeCachePath;
  
  return true;
}

bool csShaderGLCGCommon::LoadObjectCodeFromCompileCache (
  const ProfileLimits& limits, iHierarchicalCache* cache)
{
  CS_ASSERT(!objectCodeCachePath.IsEmpty());

  iHierarchicalCache* rootCache = cache->GetTopCache();
  csRef<iHierarchicalCache> progCache =
    rootCache->GetRootedCache ("/CgProgCache");
  
  csRef<iDataBuffer> cacheBuf = progCache->ReadCache (objectCodeCachePath);
  if (!cacheBuf.IsValid()) return false;
    
  csRef<iFile> cacheFile;
  cacheFile.AttachNew (new csMemFile (cacheBuf, true));
  
  uint32 diskMagic;
  if (cacheFile->Read ((char*)&diskMagic, sizeof (diskMagic))
      != sizeof (diskMagic)) return false;
  if (csLittleEndian::UInt32 (diskMagic) != cacheFileMagicCC)
    return false;
    
  CS::PluginCommon::ShaderCacheHelper::ReadString (cacheFile);
  
  if (!cacheFile.IsValid()) return false;
  
  {
    uint32 diskState;
    if (cacheFile->Read ((char*)&diskState, sizeof (diskState))
	!= sizeof (diskState)) return false;
    if (csLittleEndian::UInt32 (diskState) != cpsValid)
    {
      objectCode.Empty();
      return true;
    }
  }
  
  objectCode =
    CS::PluginCommon::ShaderCacheHelper::ReadString (cacheFile);
    
  unusedParams.DeleteAll();
  {
    csString p;
    while (!(p = CS::PluginCommon::ShaderCacheHelper::ReadString (cacheFile))
      .IsEmpty())
    {
      unusedParams.Add (p);
    }
  }
  
  return true;
}

bool csShaderGLCGCommon::WriteToCompileCache (const char* source,
                                              const ProfileLimits& limits,
                                              iHierarchicalCache* cache)
{
  CS_ASSERT(objectCodeCachePath.IsEmpty());

  iHierarchicalCache* rootCache = cache->GetTopCache();
  csMD5::Digest sourceMD5 = csMD5::Encode (source);
  csString cachePath;
  cachePath.Format ("/CgProgCache/%s/%s",
    sourceMD5.HexString().GetData(), limits.ToString().GetData());
  csRef<iHierarchicalCache> progCache = rootCache->GetRootedCache (cachePath);
  
  csString subItem;
  csRef<iStringArray> cachedVersions = progCache->GetSubItems ("/");
  for (size_t i = 0; i < cachedVersions->GetSize(); i++)
  {
    csString cachePath2 ("/");
    cachePath2.Append (cachedVersions->Get (i));
    csRef<iDataBuffer> cacheBuf = progCache->ReadCache (cachePath2);
    if (!cacheBuf.IsValid()) continue;
    
    csRef<iFile> cacheFile;
    cacheFile.AttachNew (new csMemFile (cacheBuf, true));
  
    uint32 diskMagic;
    if (cacheFile->Read ((char*)&diskMagic, sizeof (diskMagic))
	!= sizeof (diskMagic)) continue;
    if (csLittleEndian::UInt32 (diskMagic) != cacheFileMagicCC)
      continue;
    
    csString cachedSource =
      CS::PluginCommon::ShaderCacheHelper::ReadString (cacheFile);
      
    if (cachedSource == source)
    {
      subItem = cachePath2;
      break;
    }
  }
  
  if (subItem.IsEmpty())
  {
    uint n = 0;
    csRef<iDataBuffer> item;
    do
    {
      subItem.Format ("/%u", n++);
      item = progCache->ReadCache (subItem);
    }
    while (item.IsValid());
  }
  
  csMemFile cacheFile;
  
  uint32 diskMagic = csLittleEndian::UInt32 (cacheFileMagicCC);
  if (cacheFile.Write ((char*)&diskMagic, sizeof (diskMagic))
      != sizeof (diskMagic)) return false;
  
  if (!CS::PluginCommon::ShaderCacheHelper::WriteString (&cacheFile, source))
    return false;
  
  const char* object;
  if ((program == 0)
    || ((object = cgGetProgramString (program, CG_COMPILED_PROGRAM)) == 0)
    || (*object == 0))
  {
    uint32 diskState = csLittleEndian::UInt32 (cpsInvalid);
    if (cacheFile.Write ((char*)&diskState, sizeof (diskState))
	!= sizeof (diskState)) return false;
  }
  else
  {
    {
      uint32 diskState = csLittleEndian::UInt32 (cpsValid);
      if (cacheFile.Write ((char*)&diskState, sizeof (diskState))
	  != sizeof (diskState)) return false;
    }
    
    if (!CS::PluginCommon::ShaderCacheHelper::WriteString (&cacheFile, object))
      return false;
    
    csSet<csString>::GlobalIterator iter (unusedParams.GetIterator());
    while (iter.HasNext())
    {
      const csString& s = iter.Next();
      if (!CS::PluginCommon::ShaderCacheHelper::WriteString (&cacheFile, s))
	return false;
    }

    if (!CS::PluginCommon::ShaderCacheHelper::WriteString (&cacheFile, 0))
      return false;
  }
    
  if (!progCache->CacheData (cacheFile.GetData(), cacheFile.GetSize(),
      subItem))
    return false;
    
  objectCodeCachePath.Format ("/%s/%s%s",
    sourceMD5.HexString().GetData(), limits.ToString().GetData(),
    subItem.GetData());
  
  return true;
}
    
bool csShaderGLCGCommon::WriteToCompileCache (const ProfileLimits& limits,
                                              iHierarchicalCache* cache)
{
  CS_ASSERT(objectCodeCachePath.IsEmpty());
  csString source;

  ArgumentArray args;
  shaderPlug->GetProfileCompilerArgs (GetProgramType(),
    limits.profile, limits.vendor, false, args);
  for (size_t i = 0; i < compilerArgs.GetSize(); i++) 
    args.Push (compilerArgs[i]);

  // Get preprocessed result of pristine source
  source = GetPreprocessedProgram (
    cgGetProgramString (program, CG_PROGRAM_SOURCE), args);
      
  iHierarchicalCache* rootCache = cache->GetTopCache();
  csRef<iHierarchicalCache> progCache = rootCache->GetRootedCache ("/CgProgCache");
  csMD5::Digest sourceMD5 = csMD5::Encode (source);
  
  csString cachePath;
  uint n = 0;
  csRef<iDataBuffer> item;
  do
  {
    cachePath.Format ("/%s/%s/%u",
      sourceMD5.HexString().GetData(), limits.ToString().GetData(),
      n++);
    item = progCache->ReadCache (cachePath);
  }
  while (item.IsValid());
  
  csMemFile cacheFile;
  
  uint32 diskMagic = csLittleEndian::UInt32 (cacheFileMagicCC);
  if (cacheFile.Write ((char*)&diskMagic, sizeof (diskMagic))
      != sizeof (diskMagic)) return false;
  
  if (!CS::PluginCommon::ShaderCacheHelper::WriteString (&cacheFile, source))
    return false;
  
  const char* object;
  if ((program == 0)
    || ((object = cgGetProgramString (program, CG_COMPILED_PROGRAM)) == 0)
    || (*object == 0))
  {
    uint32 diskState = csLittleEndian::UInt32 (cpsInvalid);
    if (cacheFile.Write ((char*)&diskState, sizeof (diskState))
	!= sizeof (diskState)) return false;
  }
  else
  {
    {
      uint32 diskState = csLittleEndian::UInt32 (cpsValid);
      if (cacheFile.Write ((char*)&diskState, sizeof (diskState))
	  != sizeof (diskState)) return false;
    }
    
    if (!CS::PluginCommon::ShaderCacheHelper::WriteString (&cacheFile, object))
      return false;
  
    csSet<csString>::GlobalIterator iter (unusedParams.GetIterator());
    while (iter.HasNext())
    {
      const csString& s = iter.Next();
      if (!CS::PluginCommon::ShaderCacheHelper::WriteString (&cacheFile, s))
	return false;
    }

    if (!CS::PluginCommon::ShaderCacheHelper::WriteString (&cacheFile, 0))
      return false;
  }
    
  if (!progCache->CacheData (cacheFile.GetData(), cacheFile.GetSize(),
      cachePath))
    return false;
    
  objectCodeCachePath = cachePath;
  
  return true;
}
    
}
CS_PLUGIN_NAMESPACE_END(GLShaderCg)
