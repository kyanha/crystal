/*
    Copyright (C) 2002 by Marten Svanfeldt
                          Anders Stenberg

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#ifndef __CS_IVIDEO_SHADER_H__
#define __CS_IVIDEO_SHADER_H__

#include "csgeom/vector4.h"
#include "csutil/ref.h"
#include "csutil/refarr.h"
#include "csutil/scf.h"
#include "csutil/strhash.h"
#include "ivideo/graph3d.h"
#include "csgfx/shadervar.h"

struct iString;
struct iDataBuffer;
struct iDocumentNode;
struct iMaterial;

struct iShaderManager;
struct iShaderRenderInterface;
struct iShader;
struct iShaderCompiler;

struct csRenderMesh;



SCF_VERSION (iShaderVariableContext, 0, 0, 1);

/**
 * This is a baseclass for all interfaces which provides shadervariables
 * both dynamically and static
 */
struct iShaderVariableContext : public iBase
{
  /// Add a variable to this context
  virtual void AddVariable (csShaderVariable *variable) = 0;
  
  /// Get a named variable from this context
  virtual csShaderVariable* GetVariable (csStringID name) const = 0;

  /// Get a named variable from this context, and any context above/outer
  virtual csShaderVariable* GetVariableRecursive (csStringID name) const = 0;

  /// Fill a csShaderVariableList. Return number of variables filled
  virtual unsigned int FillVariableList (csShaderVariableProxyList *list) const = 0;

  /**
   * Like GetVariable(), but it also adds it if doesn't exist already.
   */
  csShaderVariable* GetVariableAdd (csStringID name)
  {
    csShaderVariable* sv;
    sv = GetVariable (name);
    if (sv == 0)
    {
      sv = new csShaderVariable (name);
      AddVariable (sv);
    }
    return sv;
  }

  /**
   * Like GetVariable(), but it also adds it if doesn't exist already.
   */
  csShaderVariable* GetVariableRecursiveAdd (csStringID name)
  {
    csShaderVariable* sv;
    sv = GetVariableRecursive (name);
    if (sv == 0)
    {
      sv = new csShaderVariable (name);
      AddVariable (sv);
    }
    return sv;
  }
};

SCF_VERSION (iShaderManager, 0, 1, 0);

/**
 * A manager for all shaders. Will only be one at a given time
 */
struct iShaderManager : public iShaderVariableContext
{
  /// Register a shader to the shadermanager. Compiler should register all shaders
  virtual void RegisterShader(iShader* shader) = 0;
  /// Get a shader by name
  virtual iShader* GetShader(const char* name) = 0;
  /// Returns all shaders that have been created
  virtual const csRefArray<iShader> &GetShaders ()  = 0;

  /// Register a compiler to the manager
  virtual void RegisterCompiler (iShaderCompiler* compiler) = 0;
  /// Get a shadercompiler by name
  virtual iShaderCompiler* GetCompiler(const char* name) = 0;
};

SCF_VERSION (iShaderRenderInterface, 0,0,1);

/// Document me!@@@
struct iShaderRenderInterface : public iBase
{
  /// Get a implementationspecific object
  virtual void* GetPrivateObject(const char* name) = 0;
};

SCF_VERSION (iShader, 0,0,2);

/**
 * Specific shader. Can/will be either render-specific or general
 * The shader in this form is "compiled" and cannot be modified
 */
struct iShader : public iShaderVariableContext
{
  /// Retrieve name of shader
  virtual const char* GetName() = 0;

  /// Get number of passes this shader have
  virtual int GetNumberOfPasses() = 0;

  /// Activate a pass for rendering
  virtual bool ActivatePass(unsigned int number) = 0;

  /// Setup a pass.
  virtual bool SetupPass(csRenderMesh *mesh,
    csArray<iShaderVariableContext*> &dynamicDomains) = 0;

  /**
   * Tear down current state, and prepare for a new mesh 
   * (for which SetupPass is called)
   */
  virtual bool TeardownPass() = 0;

  /// Completly deactivate a pass
  virtual bool DeactivatePass() = 0;
};


SCF_VERSION (iShaderCompiler, 0,0,1);
/**
 * Compiler of shaders. Compile from a description of the shader to a 
 * compiled shader. The exact schema for input is specific to each shader-
 * compiler.
 */
struct iShaderCompiler : iBase
{
  /// Get a name identifying this compiler
  virtual const char* GetName() = 0;

  /// Compile a template into a shader. Will return 0 if it fails
  virtual csPtr<iShader> CompileShader (iDocumentNode *templ) = 0;

  /// Validate if a template is a valid shader to this compiler
  virtual bool ValidateTemplate (iDocumentNode *templ) = 0;

  /// Check if template is parsable by this compiler
  virtual bool IsTemplateToCompiler (iDocumentNode *templ) = 0;
};

#endif // __CS_IVIDEO_SHADER_H__
