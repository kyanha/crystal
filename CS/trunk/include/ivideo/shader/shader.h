/*
    Copyright (C) 2002 by M�rten Svanfeldt
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
#include "csutil/csvector.h"
#include "ivideo/render3d.h"
#include "ivideo/rendermesh.h"
#include "csgfx/symtable.h"

struct iString;
struct iDataBuffer;
struct iDocumentNode;
struct iMaterial;

struct iShaderManager;
struct iShaderRenderInterface;
struct iShader;
struct iShaderWrapper;
struct iShaderVariable;
struct iShaderTechnique;
struct iShaderPass;
struct iShaderProgram;
struct iShaderProgramPlugin;
struct iShaderRenderInterface;

/**
 * A variable stored in any of the shader-related classes.
 */
SCF_VERSION (iShaderVariable, 0, 1, 0);
struct iShaderVariable : public iBase
{
  /// Types of variables
  enum VariableType
  {
    INT = 1,
    STRING,
    FLOAT,
    VECTOR3,
    VECTOR4
  };

  /// Set the variable's type
  virtual void SetType(VariableType) = 0;
  /// Get the variable's type
  virtual VariableType GetType() const = 0;

  /// Get the variable's name
  virtual csStringID GetName() const = 0;

  /// Get an int value
  virtual bool GetValue(int& value) const = 0;
  /// Get a float value
  virtual bool GetValue(float& value) const = 0;
  /// Get a string value
  virtual bool GetValue(iString*& value) const = 0;
  /// Get a 3-vector value
  virtual bool GetValue(csVector3& value) const = 0;
  /// Get a 4-vector value
  virtual bool GetValue(csVector4& value) const = 0;

  /// Set an int value
  virtual bool SetValue(int value) = 0;
  /// Set a float value
  virtual bool SetValue(float value) = 0;
  /// Set a string value
  virtual bool SetValue(iString* value) = 0;
  /// Set a 3-vector value
  virtual bool SetValue(const csVector3 &value) = 0;
  /// Set a 4-vector value
  virtual bool SetValue(const csVector4 &value) = 0;
};

SCF_VERSION (iShaderBranch, 0, 0, 1);
/**
 * Any class that wants in on the shader variable system
 * must implement this interface.
 */
struct iShaderBranch : public iBase
{
  /// Add a child to this branch
  virtual void AddChild(iShaderBranch *child) = 0;

  /// Add a variable to this context
  virtual void AddVariable(iShaderVariable* variable) = 0;

  /// Get variable
  virtual iShaderVariable* GetVariable(csStringID) = 0;

  /// Get the symbol table (used by the implementation to store the variables)
  virtual csSymbolTable* GetSymbolTable() = 0;

  /// Get a symbol table from the array of symbol tables.
  /// If there is only one symbol table, return that one.
  virtual csSymbolTable* GetSymbolTable(int index) = 0;

  /// Select the current symbol table from the array of symbol tables.
  /// If there is only one symbol table, this is a no-op.
  virtual void SelectSymbolTable(int index) = 0;
};

SCF_VERSION (iShaderManager, 0, 1, 0);
/**
 * A manager for all shaders. Will only be one at a given time
 */
struct iShaderManager : iShaderBranch
{
  /// Create an empty shader
  virtual csPtr<iShader> CreateShader() = 0;
  /// Get a shader by name
  virtual iShaderWrapper* GetShader(const char* name) = 0;
  /// Create a wrapper for a new shader
  virtual csPtr<iShaderWrapper> CreateWrapper(iShader* shader) = 0;
  /// Returns all shaders that have been created
  virtual const csRefArray<iShaderWrapper> &GetShaders () = 0;

  /// Create variable
  virtual csPtr<iShaderVariable> CreateVariable(csStringID name) const = 0;

  /// Create a shaderprogram
  virtual csPtr<iShaderProgram> CreateShaderProgram(const char* type) = 0;
};

SCF_VERSION (iShaderRenderInterface, 0,0,1);
struct iShaderRenderInterface : iBase
{
  /// Get a implementationspecific object
  virtual void* GetPrivateObject(const char* name) = 0;
};

SCF_VERSION (iShader, 0,0,1);
/**
 * Specific shader. Can/will be either render-specific or general
 */
struct iShader : iShaderBranch
{
  /// Set this shader's name
  virtual void SetName(const char* name) = 0;
  /// Retrieve name of shader
  virtual const char* GetName() = 0;

  /// Create a new technique
  virtual csPtr<iShaderTechnique> CreateTechnique() = 0;
  /// Get number of techniques
  virtual int GetTechniqueCount() const = 0;
  /// Retrieve a technique
  virtual iShaderTechnique* GetTechnique(int technique) = 0;
  /// Retrieve the best technique in this shader
  virtual iShaderTechnique* GetBestTechnique() = 0;

  /// Check if valid (normaly a shader is valid if there is at least one valid technique)
  virtual bool IsValid() const = 0;

  /// Loads a shader from buffer
  virtual bool Load(iDataBuffer* program) = 0;

  /// Loads from a document-node
  virtual bool Load(iDocumentNode* node) = 0;

  /// Prepares the shader for usage. Must be called before the shader is assigned to a material
  virtual bool Prepare() = 0;
};

SCF_VERSION(iShaderWrapper, 0, 0, 1);
/**
 * A thin wrapper over iShader to do dynamic selection of which iMaterial
 * the shader is acting on.
 */
struct iShaderWrapper : iShaderBranch
{
  /// Get the wrapped shader.
  virtual iShader* GetShader() = 0;

  /// Select the material we are about to act on.
  virtual void SelectMaterial(iMaterial *mat) = 0;
};

SCF_VERSION (iShaderTechnique, 0,0,1);
/**
 * One specific technique used by shader.
 */
struct iShaderTechnique : iShaderBranch
{
  /**
   * Get technique priority. If there are several valid techniques
   * use the one with highest priority
   */
  virtual int GetPriority() const = 0;

  /// Set technique priority.
  virtual void SetPriority(int priority) = 0;

  /// Create a pass
  virtual csPtr<iShaderPass> CreatePass() = 0;
  /// Get number of passes
  virtual int GetPassCount() const = 0;
  /// Retrieve a pass
  virtual iShaderPass* GetPass( int pass ) = 0;

  /// Check if valid
  virtual bool IsValid() const = 0;

  /// Loads a technique from buffer
  virtual bool Load(iDataBuffer* program) = 0;

  /// Loads from a document-node
  virtual bool Load(iDocumentNode* node) = 0;

  /// Prepares the technique for usage.
  virtual bool Prepare() = 0;
};

SCF_VERSION (iShaderPass, 0,0,1);
/**
 * Description of a single pass in  a shader
 */
struct iShaderPass : iShaderBranch
{
  /// Add a stream mapping
  virtual void AddStreamMapping (csStringID name, csVertexAttrib attribute) = 0;
  /// Get stream mapping for a certain attribute
  virtual csStringID GetStreamMapping (csVertexAttrib attribute) const = 0;

  /// Add a texture mapping by name
  virtual void AddTextureMapping (const char* name, int unit) = 0;
  /// Add a texture mapping by material layer
  virtual void AddTextureMapping (int layer, int unit) = 0;
  /// Get texture mapping for a certain unit as a layer index
  virtual int GetTextureMappingAsLayer (int unit) const = 0;
  /// Get texture mapping for a certain unit as a texture name
  virtual iTextureHandle* GetTextureMappingAsDirect (int unit) = 0;

  /// Get mixmode override
  virtual uint GetMixmodeOverride () const = 0;

  /// Get vertex-program
  virtual iShaderProgram* GetVertexProgram() = 0;

  /// Set vertex-program
  virtual void SetVertexProgram(iShaderProgram* program) = 0;

  /// Get fragment-program
  virtual iShaderProgram* GetFragmentProgram() = 0;

  /// Set fragment-program
  virtual void SetFragmentProgram(iShaderProgram* program) = 0;

  /// Check if valid
  virtual bool IsValid() const = 0;

  /// Activate the whole pass for the indicated mesh (which might be 0)
  virtual void Activate(csRenderMesh* mesh) = 0;

  /// Deactivate the whole pass
  virtual void Deactivate() = 0;

  /// Setup states needed for proper operation of the shader
  virtual void SetupState (csRenderMesh* mesh) = 0;

  /// Reset states to original
  virtual void ResetState () = 0;

  /// Loads pass from buffer
  virtual bool Load(iDataBuffer* program) = 0;

  /// Loads from a document-node
  virtual bool Load(iDocumentNode* node) = 0;

  /// Prepares the pass for usage.
  virtual bool Prepare() = 0;
};

SCF_VERSION (iShaderProgram, 0,0,1);
/** 
 * A shader-program is either a vertexprogram, fragmentprogram or any
 * other type of "program" utilizied by shader.
 */
struct iShaderProgram : iShaderBranch
{
  /// Get a programid for the current program
  virtual csPtr<iString> GetProgramID() = 0;

  /// Sets this program to be the one used when rendering
  virtual void Activate(iShaderPass* current, csRenderMesh* mesh) = 0;

  /// Deactivate program so that it's not used in next rendering
  virtual void Deactivate(iShaderPass* current) = 0;

  /// Setup states needed for proper operation of the shader
  virtual void SetupState (iShaderPass* current, csRenderMesh* mesh) = 0;

  /// Reset states to original
  virtual void ResetState () = 0;

  /// Check if valid
  virtual bool IsValid() = 0;

  /// Loads shaderprogram from buffer
  virtual bool Load(iDataBuffer* program) = 0;

  /// Loads from a document-node
  virtual bool Load(iDocumentNode* node) = 0;

  /// Prepares the shaderprogram for usage.
  /// Must be called before the shader is assigned to a material.
  virtual bool Prepare() = 0;
};

SCF_VERSION(iShaderProgramPlugin, 0,0,1);
struct iShaderProgramPlugin : iBase
{
  virtual csPtr<iShaderProgram> CreateProgram(const char* type) = 0;
  virtual bool SupportType(const char* type) = 0;
  virtual void Open() = 0;
};

#endif // __CS_IVIDEO_SHADER_H__
