/*
  Copyright (C) 2011 by Antony Martin

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
#include "csplugincommon/opengl/glhelper.h"

#include "csutil/scanstr.h"
#include "csutil/scfstr.h"

#include "glshader_glsl.h"
#include "glshader_glslshader.h"
#include "glshader_glslprogram.h"

CS_PLUGIN_NAMESPACE_BEGIN(GLShaderGLSL)
{
  CS_LEAKGUARD_IMPLEMENT (csShaderGLSLProgram);

  csShaderGLSLProgram::csShaderGLSLProgram (csGLShader_GLSL* shaderPlug) :
    scfImplementationType (this, shaderPlug->object_reg),
    shaderPlug (shaderPlug),
    program_id (0),
    validProgram (false),
    useTessellation (false)
  {
    InitTokenTable (xmltokens);
  }

  csShaderGLSLProgram::~csShaderGLSLProgram ()
  {
    if (program_id != 0)
      shaderPlug->ext->glDeleteObjectARB (program_id);

    for (size_t i = 0; i < variablemap.GetSize (); ++i)
    {
      VariableMapEntry& mapping = variablemap[i];
      shaderPlug->uniformAlloc.Free (reinterpret_cast<ProgramUniform*> (mapping.userVal));
    }
  }

  void csShaderGLSLProgram::Activate ()
  {
    shaderPlug->ext->glUseProgramObjectARB (program_id);
  }

  void csShaderGLSLProgram::Deactivate()
  {
    // may be overkill: shader deactivation seems to be performed each time
    // an object is rendered, but this deactivation does represent an useless
    // overhead if the next object to render also uses shaders
    shaderPlug->ext->glUseProgramObjectARB (0);
  }

  void csShaderGLSLProgram::SetupState (const CS::Graphics::RenderMesh* /*mesh*/,
    CS::Graphics::RenderMeshModes& modes,
    const csShaderVariableStack& stack)
  {
    size_t i;
    csRef<csShaderVariable> var;

    if (useTessellation)
      shaderPlug->graph->SetTessellation (true);

    // set variables
    for (i = 0; i < variablemap.GetSize (); ++i)
    {
      VariableMapEntry& mapping = variablemap[i];

      var = csGetShaderVariableFromStack (stack, mapping.name);
      if (!var.IsValid ())
        var = mapping.mappingParam.var;

      // If var is null now we have no const nor any passed value, ignore it
      if (!var.IsValid ())
        continue;
      
      const ProgramUniform* uniform (reinterpret_cast<const ProgramUniform*> (mapping.userVal));
      SetUniformValue (*uniform, var);
    }
  }
  
  template<typename Base, unsigned N>
  struct Vec
  {
    Base data[N];
    
    bool CopyFrom (csShaderVariable* var)
    {
      csVector4 v;
      if (!var->GetValue (v)) return false;
      for (uint i = 0; i < N; i++)
        data[i] = v[i];
      return true;
    }
  };
  
  template<>
  struct Vec<GLint, 1>
  {
    GLint data[1];

    bool CopyFrom (csShaderVariable* var)
    {
      int i;
      if (!var->GetValue (i)) return false;
      data[0] = i;
      return true;
    }
  };
  
  template<>
  struct Vec<GLfloat, 1>
  {
    GLfloat data[1];

    bool CopyFrom (csShaderVariable* var)
    {
      float f;
      if (!var->GetValue (f)) return false;
      data[0] = f;
      return true;
    }
  };
  
  template<typename Base, unsigned Cols, unsigned Rows>
  struct Mat
  {
    Base data[Cols*Rows];
    
    bool CopyFrom (csShaderVariable* var)
    {
      float matrix[16];
      switch (var->GetType())
      {
      case csShaderVariable::TRANSFORM:
        {
          csReversibleTransform t;
          if (!var->GetValue (t)) return false;
          makeGLMatrix (t, matrix);
        }
        break;
      case csShaderVariable::MATRIX3X3:
        {
          csMatrix3 m;
          if (!var->GetValue (m)) return false;
          makeGLMatrix (m, matrix);
        }
        break;
      case csShaderVariable::MATRIX4X4:
        {
          CS::Math::Matrix4 m;
          if (!var->GetValue (m)) return false;
          CS::PluginCommon::MakeGLMatrix4x4 (m, matrix);
        }
        break;
      default:
        return false;
      }
      
      for (uint c = 0; c < Cols; c++)
      {
        for (uint r = 0; r < Rows; r++)
        {
          data[r + c * Rows] = matrix[r + c*4];
        }
      }
      return true;
    }
  };
  
  template<typename WrappedFunction>
  struct glUniformMatrixWrapperType
  {
    WrappedFunction func;
    
    glUniformMatrixWrapperType (WrappedFunction func) : func (func) {}
    
    void operator() (GLint location, GLsizei count, GLfloat* value)
    {
      func (location, count, GL_FALSE /* column major order */, value);
    }
  };
  
  template<typename WrappedFunction>
  glUniformMatrixWrapperType<WrappedFunction> glUniformMatrixWrapper (WrappedFunction func)
  {
    return glUniformMatrixWrapperType<WrappedFunction> (func);
  }

  void csShaderGLSLProgram::SetUniformValue (const ProgramUniform& uniform,
                                             csShaderVariable* var)
  {
      const csGLExtensionManager* ext = shaderPlug->ext;
      switch (uniform.type)
      {
      case GL_FLOAT:
      case GL_BOOL_ARB:
        SetUniformValue<Vec<GLfloat, 1> > (uniform, var, ext->glUniform1fvARB);
        break;    
      case GL_FLOAT_VEC2_ARB:
      case GL_BOOL_VEC2_ARB:
        SetUniformValue<Vec<GLfloat, 2> > (uniform, var, ext->glUniform2fvARB);
        break;    
      case GL_FLOAT_VEC3_ARB:
      case GL_BOOL_VEC3_ARB:
        SetUniformValue<Vec<GLfloat, 3> > (uniform, var, ext->glUniform3fvARB);
        break;    
      case GL_FLOAT_VEC4_ARB:
      case GL_BOOL_VEC4_ARB:
        SetUniformValue<Vec<GLfloat, 4> > (uniform, var, ext->glUniform4fvARB);
        break;    
      case GL_INT:
        SetUniformValue<Vec<GLint, 1> > (uniform, var, ext->glUniform1ivARB);
        break;    
      case GL_INT_VEC2_ARB:
        SetUniformValue<Vec<GLint, 2> > (uniform, var, ext->glUniform2ivARB);
        break;    
      case GL_INT_VEC3_ARB:
        SetUniformValue<Vec<GLint, 3> > (uniform, var, ext->glUniform3ivARB);
        break;    
      case GL_INT_VEC4_ARB:
        SetUniformValue<Vec<GLint, 4> > (uniform, var, ext->glUniform4ivARB);
        break;
      case GL_FLOAT_MAT2_ARB:
        SetUniformValue<Mat<GLfloat, 2, 2> > (uniform, var,
                                              glUniformMatrixWrapper (ext->glUniformMatrix2fvARB));
        break;
      case GL_FLOAT_MAT3_ARB:
        SetUniformValue<Mat<GLfloat, 3, 3> > (uniform, var,
                                              glUniformMatrixWrapper (ext->glUniformMatrix3fvARB));
        break;
      case GL_FLOAT_MAT4_ARB:
        SetUniformValue<Mat<GLfloat, 4, 4> > (uniform, var,
                                              glUniformMatrixWrapper (ext->glUniformMatrix4fvARB));
        break;
      default:
        break;
      }
  }

  template<typename ElementType, typename Setter>
  void csShaderGLSLProgram::SetUniformValue (const ProgramUniform& uniform,
                                             csShaderVariable* var,
                                             Setter setter)
  {
    if (var->GetType() == csShaderVariable::ARRAY)
    {
      // Array: collect all values into a single flat array...
      size_t numElements (uniform.size);
      numElements = csMin (numElements, var->GetArraySize());
      if (numElements == 0) return;
      CS_ALLOC_STACK_ARRAY(ElementType, values, numElements);
      memset (values, 0, sizeof (ElementType) * numElements);
      for (size_t i = 0; i < numElements; i++)
        values[i].CopyFrom (var->GetArrayElement (i));
      // ... and set.
      setter (uniform.location, numElements, values[0].data);
    }
    else
    {
      // “Simple” value: set directly
      ElementType val;
      if (val.CopyFrom (var))
        setter (uniform.location, 1, val.data);
    }
  }

  void csShaderGLSLProgram::ResetState ()
  {
    if (useTessellation)
      shaderPlug->graph->SetTessellation (false);
  }

  csRef<iDataBuffer> csShaderGLSLProgram::LoadSource (iDocumentNode* node)
  {
    ProgramSource program;
    if (!ParseProgramNode (node, program)) return (iDataBuffer*)nullptr;
    return GetProgramData (program);
  }

  bool csShaderGLSLProgram::Load (iShaderDestinationResolver*,
    iDocumentNode* node)
  {
    if (!node)
      return false;

    csRef<iDocumentNodeIterator> it = node->GetNodes ();
    while(it->HasNext())
    {
      csRef<iDocumentNode> child = it->Next();
      if(child->GetType() != CS_NODE_ELEMENT) continue;
      const char* value = child->GetValue ();
      csStringID id = xmltokens.Request (value);
      switch(id)
      {
        case XMLTOKEN_VP:
          vpSource = LoadSource (child);
          if (!vpSource)
            return false;
          break;
        case XMLTOKEN_FP:
          fpSource = LoadSource (child);
          if (!fpSource)
            return false;
          break;
        case XMLTOKEN_GP:
          gpSource = LoadSource (child);
          if (!gpSource)
            return false;
          break;
        case XMLTOKEN_EP:
          epSource = LoadSource (child);
          if (!epSource)
            return false;
          break;
        case XMLTOKEN_CP:
          cpSource = LoadSource (child);
          if (!cpSource)
            return false;
          break;

        case XMLTOKEN_VARIABLEMAP:
        case XMLTOKEN_DESCRIPTION:
          if (!ParseCommon (child))
            return false;
          break;
        default:
          synsrv->ReportBadToken (child);
          return false;
      }
    }

    return true;
  }

  bool csShaderGLSLProgram::Load (iShaderDestinationResolver*, const char* program, 
    csArray<csShaderVarMapping> &mappings)
  {
    // makes no sense for an "unified" shader to be loaded from one single source
    return false;
  }

  void csShaderGLSLProgram::SetupVmap ()
  {
    const csGLExtensionManager* ext = shaderPlug->ext;
    csRef<csShaderVariable> var;
    
    csHash<ProgramUniform, csString> uniforms;
    GLint activeUniforms (0);
    ext->glGetObjectParameterivARB (program_id, GL_OBJECT_ACTIVE_UNIFORMS_ARB,
                                    &activeUniforms);
    GLint maxUniformNameLen (0);
    ext->glGetObjectParameterivARB (program_id, GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB,
                                    &maxUniformNameLen);
    CS_ALLOC_STACK_ARRAY(char, uniformName, maxUniformNameLen+1);
    for (GLint i = 0; i < activeUniforms; i++)
    {
      ProgramUniform uniform;
      ext->glGetActiveUniformARB (program_id, GLuint (i),
                                  maxUniformNameLen, nullptr, &uniform.size,
                                  &uniform.type, reinterpret_cast<GLcharARB*> (uniformName));
      uniform.location = ext->glGetUniformLocationARB (program_id,
        uniformName);
      uniforms.Put (uniformName, uniform);
      
      // Distribute TUs to active samplers right away
      if ((uniform.type == GL_SAMPLER_1D_ARB)
        || (uniform.type == GL_SAMPLER_2D_ARB)
        || (uniform.type == GL_SAMPLER_3D_ARB)
        || (uniform.type == GL_SAMPLER_CUBE_ARB)
        || (uniform.type == GL_SAMPLER_1D_SHADOW_ARB)
        || (uniform.type == GL_SAMPLER_2D_SHADOW_ARB)
        || (uniform.type == GL_SAMPLER_2D_RECT_ARB)
        || (uniform.type == GL_SAMPLER_2D_RECT_SHADOW_ARB))
      {
        int tu = int (tuBindings.GetSize());
        tuBindings.Put (uniformName, tu);
        ext->glUniform1iARB (uniform.location, tu);
      }
    }

    for (size_t i = 0; i < variablemap.GetSize (); ++i)
    {
      VariableMapEntry& mapping = variablemap[i];
      
      ProgramUniform* uniform (uniforms.GetElementPointer (mapping.destination));
      if (!uniform) 
      {
        // the uniform variable doesnt exist!
        variablemap.DeleteIndex (i--);
        continue;
      }
      
      ProgramUniform* allocatedUniform (shaderPlug->uniformAlloc.Alloc (*uniform));
      mapping.userVal = reinterpret_cast<uintptr_t> (allocatedUniform);
    }
  }

  csPtr<csShaderGLSLShader> csShaderGLSLProgram::CreateVP () const
  {
    return csPtr<csShaderGLSLShader> (
      new csShaderGLSLShader (shaderPlug, "vertex", GL_VERTEX_SHADER_ARB));
  }
  csPtr<csShaderGLSLShader> csShaderGLSLProgram::CreateFP () const
  {
    return csPtr<csShaderGLSLShader> (
      new csShaderGLSLShader (shaderPlug, "fragment", GL_FRAGMENT_SHADER_ARB));
  }
  csPtr<csShaderGLSLShader> csShaderGLSLProgram::CreateGP () const
  {
    return csPtr<csShaderGLSLShader> (
      new csShaderGLSLShader (shaderPlug, "geometry", GL_GEOMETRY_SHADER_EXT));
  }
  csPtr<csShaderGLSLShader> csShaderGLSLProgram::CreateEP () const
  {
    return csPtr<csShaderGLSLShader> (
      new csShaderGLSLShader (shaderPlug, "tessellation evaluation",
      GL_TESS_EVALUATION_SHADER_ARB));
  }
  csPtr<csShaderGLSLShader> csShaderGLSLProgram::CreateCP () const
  {
    return csPtr<csShaderGLSLShader> (
      new csShaderGLSLShader (shaderPlug, "tessellation control",
      GL_TESS_CONTROL_SHADER_ARB));
  }

  bool csShaderGLSLProgram::Compile (iHierarchicalCache*, csRef<iString>* tag)
  {
    GLint status;                   // link status

    shaderPlug->Open ();

    const csGLExtensionManager* ext = shaderPlug->ext;
    if (!ext)
      return false;

    program_id = ext->glCreateProgramObjectARB ();

    if (vpSource)
    {
      if (!ext->CS_GL_ARB_vertex_shader)
        return false;

      vp = CreateVP ();
      if (!vp->Compile (vpSource->GetData ()))
        return false;

      ext->glAttachObjectARB (program_id, vp->GetID ());
    }
    if (fpSource)
    {
      if (!ext->CS_GL_ARB_fragment_shader)
        return false;

      fp = CreateFP ();
      if (!fp->Compile (fpSource->GetData ()))
        return false;

      ext->glAttachObjectARB (program_id, fp->GetID ());
    }
    if (gpSource)
    {
      if (!ext->CS_GL_EXT_geometry_shader4)
        return false;

      gp = CreateGP ();
      if (!gp->Compile (gpSource->GetData ()))
        return false;

      ext->glAttachObjectARB (program_id, gp->GetID ());
    }
    if (epSource)
    {
      if (!ext->CS_GL_ARB_tessellation_shader)
        return false;

      ep = CreateEP ();
      if (!ep->Compile (epSource->GetData ()))
        return false;

      ext->glAttachObjectARB (program_id, ep->GetID ());
      useTessellation = true;
    }
    if (cpSource)
    {
      if (!ext->CS_GL_ARB_tessellation_shader)
        return false;

      cp = CreateCP ();
      if (!cp->Compile (cpSource->GetData ()))
        return false;

      ext->glAttachObjectARB (program_id, cp->GetID ());
      useTessellation = true;
    }

    ext->glLinkProgramARB (program_id);
    ext->glGetObjectParameterivARB (program_id, GL_OBJECT_LINK_STATUS_ARB,
      &status);
    if (!status)
    {
      GLint size;

      ext->glGetObjectParameterivARB (program_id, GL_OBJECT_INFO_LOG_LENGTH_ARB,
        &size);
      csString logs((size_t)(size + 1));
      // cast hax
      ext->glGetInfoLogARB (program_id, size, NULL, (GLcharARB*)logs.GetData ());

      shaderPlug->Report (CS_REPORTER_SEVERITY_WARNING,
        "Couldn't link %s", description.GetData ());
      shaderPlug->Report (CS_REPORTER_SEVERITY_WARNING, "Error string: %s", 
        CS::Quote::Single (logs.GetData ()));
      return false;
    }

    // glValidateProgram() ?

    SetupVmap ();

    tag->AttachNew (new scfString ("default"));

    return true;
  }


  csVertexAttrib csShaderGLSLProgram::ResolveBufferDestination (const char* binding)
  {
    csVertexAttrib dest = CS_VATTRIB_INVALID;
    const csGLExtensionManager* ext = shaderPlug->ext;

    if (ext)
    {
      int unit;
      if (csScanStr (binding, "gl_MultiTexCoord%d", &unit) == 1)
      {
        // TODO: unit might be too big for the current implementation
        dest = (csVertexAttrib)(CS_VATTRIB_TEXCOORD0 + unit);
      }
      else if (strcasecmp (binding, "gl_Vertex") == 0)
      {
        dest = CS_VATTRIB_POSITION;
      }
      else if (strcasecmp (binding, "gl_Color") == 0)
      {
        dest = CS_VATTRIB_PRIMARY_COLOR;
      }
      else if (strcasecmp (binding, "gl_SecondaryColor") == 0)
      {
        dest = CS_VATTRIB_SECONDARY_COLOR;
      }
      else if (strcasecmp (binding, "gl_Normal") == 0)
      {
        dest = CS_VATTRIB_NORMAL;
      }
      else if (strcasecmp (binding, "gl_FogCoord") == 0)
      {
        dest = CS_VATTRIB_FOGCOORD;
      }
      else
      {
        GLint loc = ext->glGetAttribLocationARB (program_id, binding);
        if (loc != -1)
        {
          dest = (csVertexAttrib)(CS_VATTRIB_0 + loc);
        }
        else
        {
          dest = CS_VATTRIB_UNUSED;
        }
      }
    }

    return dest;
  }

  int csShaderGLSLProgram::ResolveTU (const char* binding)
  {
    return tuBindings.Get (binding, -1);
  }
}
CS_PLUGIN_NAMESPACE_END(GLShaderGLSL)
