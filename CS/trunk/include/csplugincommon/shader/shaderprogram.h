/*
    Copyright (C) 2004 by Jorrit Tyberghein
	      (C) 2004 by Frank Richter

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

#ifndef __CS_SHADERPLUGINS_COMMON_SHADERPROGRAM_H__
#define __CS_SHADERPLUGINS_COMMON_SHADERPROGRAM_H__

#include "csextern.h"
#include "csutil/array.h"
#include "csutil/csstring.h"
#include "csutil/ref.h"
#include "csutil/strhash.h"
#include "csgfx/shadervar.h"
#include "csgfx/shadervarcontext.h"

#include "csutil/leakguard.h"
#include "iutil/databuff.h"
#include "iutil/document.h"
#include "iutil/objreg.h"
#include "iutil/strset.h"
#include "iutil/vfs.h"
#include "imap/services.h"

#include "shaderplugin.h"

struct iDataBuffer;

class CS_CRYSTALSPACE_EXPORT csShaderProgram : public iShaderProgram,
						 public iShaderTUResolver
{
protected:
  csStringHash commonTokens;
#define CS_INIT_TOKEN_TABLE_NAME InitCommonTokens
#define CS_TOKEN_ITEM_FILE \
  "csplugincommon/shader/shaderprogram.tok"
#include "cstool/tokenlist.h"
#undef CS_TOKEN_ITEM_FILE
#undef CS_INIT_TOKEN_TABLE_NAME

protected:
  iObjectRegistry* objectReg;
  csRef<iSyntaxService> synsrv;
  csRef<iStringSet> strings;

  enum ProgramParamType
  {
    ParamInvalid    = 0,
    ParamFloat	    = 0x01,
    ParamVector2    = 0x02,
    ParamVector3    = 0x04,
    ParamVector4    = 0x08,
    ParamMatrix	    = 0x10
  };
  struct ProgramParam
  {
    bool valid;
    csVector4 vectorValue;
    csMatrix3 matrixValue;
    csStringID name;
    csRef<csShaderVariable> var;
    ProgramParamType type;

    ProgramParam() : valid (false), vectorValue (0.0f), 
      name(csInvalidStringID), type(ParamInvalid) { }
    ProgramParam(const csVector4& def) : valid (false), 
      vectorValue (def), name(csInvalidStringID), type(ParamInvalid) { }
  };
  friend ProgramParam; // Give MSVC6 access to ParamInvalid.
  void ResolveParamStatic (ProgramParam& param,
    csArray<iShaderVariableContext*> &staticContexts);
  bool ParseProgramParam (iDocumentNode* node,
    ProgramParam& param, uint types = ~0);
  bool RetrieveParamValue (ProgramParam& param, 
    const csShaderVarStack& stacks);

  struct VariableMapEntry : public csShaderVarMapping
  {
    ProgramParam mappingParam;
    intptr_t userVal;

    VariableMapEntry (csStringID s, const char* d) : 
      csShaderVarMapping (s, d)
    { 
      userVal = 0;
      mappingParam.name = s;
      mappingParam.valid = true;
    }
    VariableMapEntry (const csShaderVarMapping& other) :
      csShaderVarMapping (other.name, other.destination)
    {
      userVal = 0;
      mappingParam.name = other.name;
      mappingParam.valid = true;
    }
  };
  csArray<VariableMapEntry> variablemap;

  char* description;
  csRef<iDocumentNode> programNode;
  csRef<iFile> programFile;
  csString programFileName;
  /**
   * Whether the shader program should report additional information during
   * runtime.
   */
  bool doVerbose;

  csShaderVariableContext svcontext;

  bool ParseCommon (iDocumentNode* child);
  iDocumentNode* GetProgramNode ();
  csPtr<iDataBuffer> GetProgramData ();
  void ResolveStaticVars (csArray<iShaderVariableContext*> &staticContexts);
  void DumpProgramInfo (csString& output);
  void DumpVariableMappings (csString& output);
public:
  SCF_DECLARE_IBASE;

  CS_LEAKGUARD_DECLARE (csShaderProgram);

  csShaderProgram (iObjectRegistry* objectReg);
  virtual ~csShaderProgram ();

  virtual int ResolveTextureBinding (const char* binding)
  { return -1; }
};

#endif // __CS_SHADERPLUGINS_COMMON_SHADERPROGRAM_H__
