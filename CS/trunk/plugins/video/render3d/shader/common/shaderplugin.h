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

#include "csutil/scf.h"
#include "csgfx/shadervar.h"

struct varmapping
{
  csStringID source;
  csString destination;
  varmapping (csStringID s, char* d)
    : source(s), destination(d) {}
};

SCF_VERSION(iShaderProgram, 0,2,0);
/**
 * A helper for shaders that which to use the general plugins.
 * This is the main program plugin interface
 */
struct iShaderProgram : iBase
{
  /// Sets this program to be the one used when rendering
  virtual void Activate() = 0;

  /// Deactivate program so that it's not used in next rendering
  virtual void Deactivate() = 0;

  /// Setup states needed for proper operation of the shaderprogram
  virtual void SetupState (csRenderMesh* mesh, 
    const CS_SHADERVAR_STACK &stacks) = 0;

  /// Reset states to original
  virtual void ResetState () = 0;

  /// Loads from a document-node
  virtual bool Load (iDocumentNode* node) = 0;

  /// Loads from raw text
  virtual bool Load (const char* program, csArray<varmapping> &mappings) = 0;

  /// Compile a program
  virtual bool Compile (csArray<iShaderVariableContext*> &staticDomains) = 0;
};

SCF_VERSION(iShaderProgramPlugin,0,1,0);
/**
 * Plugins which provide iShaderProgram should implement this as a factory
 * for iShaderProgram
 */
struct iShaderProgramPlugin : public iBase
{
  virtual csPtr<iShaderProgram> CreateProgram(const char* type) = 0;
  virtual bool SupportType(const char* type) = 0;
};

#endif

