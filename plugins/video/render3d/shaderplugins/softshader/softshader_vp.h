/*
Copyright (C) 2002 by Anders Stenberg
                      M�rten Svanfeldt

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

#ifndef __SOFTSHADER_VP_H__
#define __SOFTSHADER_VP_H__

#include "ivideo/shader/shader.h"
#include "csutil/strhash.h"

class csSoftShader_VP : public iShaderProgram
{
private:
  enum
  {
    XMLTOKEN_SOFTVP = 1
  };

  csRef<iObjectRegistry> object_reg;

  csStringHash xmltokens;

  void BuildTokenHash();

  bool validProgram;

  csArray<csSymbolTable> symtabs;
  csSymbolTable *symtab;

public:
  SCF_DECLARE_IBASE;

  csSoftShader_VP(iObjectRegistry* objreg)
  {
    validProgram = true;
    SCF_CONSTRUCT_IBASE (0);
    this->object_reg = objreg;
  }
  virtual ~csSoftShader_VP ()
  {
  }

  void SetValid(bool val) { validProgram = val; }

  ////////////////////////////////////////////////////////////////////
  //                      iShaderProgram
  ////////////////////////////////////////////////////////////////////

  virtual csPtr<iString> GetProgramID();

  /// Sets this program to be the one used when rendering
  virtual void Activate(iShaderPass* current, csRenderMesh* mesh);

  /// Deactivate program so that it's not used in next rendering
  virtual void Deactivate(iShaderPass* current);

  virtual void SetupState (iShaderPass *current, csRenderMesh *mesh) {}

  virtual void ResetState () {}

  virtual void AddChild(iShaderBranch *b) {}
  virtual void AddVariable(iShaderVariable* variable) {}
  virtual iShaderVariable* GetVariable(csStringID s)
    { return (iShaderVariable *) symtab->GetSymbol(s); }
  virtual csSymbolTable* GetSymbolTable() { return symtab; }
  virtual void SelectSymbolTable(int i) {
    if (symtabs.Length () <= i) symtabs.SetLength (i + 1, csSymbolTable ());
    symtab = & symtabs[i];
  }

  /// Check if valid
  virtual bool IsValid() { return validProgram;} 

    /// Loads shaderprogram from buffer
  virtual bool Load(iDataBuffer* program);

  /// Loads from a document-node
  virtual bool Load(iDocumentNode* node);

  /// Prepares the shaderprogram for usage. Must be called before the shader is assigned to a material
  virtual bool Prepare();
};


#endif //__SOFTSHADER_VP_H__

