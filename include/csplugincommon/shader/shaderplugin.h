/*
  Copyright (C) 2003 by Marten Svanfeldt

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

#ifndef __CS_SHADERPLUGIN_H__
#define __CS_SHADERPLUGIN_H__

/**\file
 * Shader plugin related interfaces.
 */

#include "csutil/scf.h"
#include "csutil/strhash.h"

#include "ivideo/shader/shader.h"

/**\addtogroup plugincommon
 * @{ */

/// Mapping of a shader variable to some shader-specific destination
struct csShaderVarMapping
{
  /// Shader variable name
  csStringID name;
  /// Destination
  csString destination;
  csShaderVarMapping (csStringID n, const char* d)
    : name(n), destination(d) {}
};


/**
 * Interface to allow resolution of friendly destination names. Passed when a 
 * shader program is loaded, used to resolve unknown texture unit names etc.
 */
struct iShaderDestinationResolver : public virtual iBase
{
  SCF_INTERFACE(iShaderDestinationResolver, 0,0,1);
  /**
   * When the destination of a texture binding wasn't recognized, the FP
   * is asked whether it can provide a TU number for it.
   */
  virtual int ResolveTU (const char* binding) = 0;

  virtual csVertexAttrib ResolveBufferDestination (const char* binding) = 0;
};

/**
 * A helper for shaders that which to use the general plugins.
 * This is the main program plugin interface
 */
struct iShaderProgram : public virtual iBase
{
  SCF_INTERFACE(iShaderProgram, 2, 1, 0);
  /// Sets this program to be the one used when rendering
  virtual void Activate() = 0;

  /// Deactivate program so that it's not used in next rendering
  virtual void Deactivate() = 0;

  /// Setup states needed for proper operation of the shaderprogram
  virtual void SetupState (const CS::RenderMesh* mesh, 
                           CS::RenderMeshModes& modes,
                           const iShaderVarStack* stacks) = 0;

  /// Reset states to original
  virtual void ResetState () = 0;

  /// Loads from a document-node
  virtual bool Load (iShaderDestinationResolver* resolve, 
    iDocumentNode* node) = 0;

  /// Loads from raw text
  virtual bool Load (iShaderDestinationResolver* resolve, const char* program, 
    csArray<csShaderVarMapping>& mappings) = 0;

  /// Compile a program
  virtual bool Compile () = 0;
};

/**
 * Plugins which provide iShaderProgram should implement this as a factory
 * for iShaderProgram
 */
struct iShaderProgramPlugin : public virtual iBase
{
  SCF_INTERFACE(iShaderProgramPlugin,2,0,0);
  virtual csPtr<iShaderProgram> CreateProgram(const char* type) = 0;
  virtual bool SupportType(const char* type) = 0;
};

/** @} */

#endif

