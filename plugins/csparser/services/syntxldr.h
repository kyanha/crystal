/*
    Copyright (C) 2001 by Norman Kraemer

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

#ifndef __CS_SYNTAX_SERVICE_H__
#define __CS_SYNTAX_SERVICE_H__


#include "imap/services.h"
#include "iutil/comp.h"
#include "iutil/dbghelp.h"
#include "csutil/csstring.h"
#include "csutil/strhash.h"

struct iObjectRegistry;
struct iThingFactoryState;
struct iEngine;
struct iSector;
struct iMaterialWrapper;
struct iReporter;
struct iLoader;
struct iMeshObjectType;
struct csGradientShade;

CS_PLUGIN_NAMESPACE_BEGIN(SyntaxService)
{

/**
 * This component provides services for other loaders to easily parse
 * properties of standard CS world syntax. This implementation will parse
 * the textual representation.
 */
class csTextSyntaxService : 
  public scfImplementation3<csTextSyntaxService,
                            iSyntaxService,
                            iComponent,
                            iDebugHelper>
{
protected:
  iObjectRegistry* object_reg;
  csRef<iReporter> reporter;
  csStringHash xmltokens;
#define CS_TOKEN_ITEM_FILE "plugins/csparser/services/syntxldr.tok"
#include "cstool/tokenlist.h"
  csRef<iStringSet> strings;

  void ReportV (const char* msgid, int severity, 
	iDocumentNode* errornode, const char* msg, va_list arg);
  bool ParseGradientShade (iDocumentNode* node, csGradientShade& shade);
  bool WriteGradientShade (iDocumentNode* node, const csGradientShade& shade);

public:
  csTextSyntaxService (iBase *parent);
  virtual ~csTextSyntaxService ();

  bool Initialize (iObjectRegistry* object_reg);

  virtual bool ParseBool (iDocumentNode* node, bool& result, bool def_result);
  virtual bool ParseBoolAttribute (iDocumentNode* node, const char* attrname,
  	bool& result, bool def_result, bool required);
  virtual bool WriteBool (iDocumentNode* node, const char* name, bool value);
  virtual bool ParseMatrix (iDocumentNode* node, csMatrix3 &m);
  virtual bool WriteMatrix (iDocumentNode* node, const csMatrix3& m);
  virtual bool ParsePlane (iDocumentNode* node, csPlane3 &p);
  virtual bool WritePlane (iDocumentNode* node, const csPlane3 &p);
  virtual bool ParseVector (iDocumentNode* node, csVector3 &v);
  virtual bool WriteVector (iDocumentNode* node, const csVector3& v);
  virtual bool ParseVector (iDocumentNode* node, csVector2 &v);
  virtual bool WriteVector (iDocumentNode* node, const csVector2& v);
  virtual bool ParseBox (iDocumentNode* node, csBox3 &v);
  virtual bool WriteBox (iDocumentNode* node, const csBox3& v);
  virtual bool ParseColor (iDocumentNode* node, csColor &c);
  virtual bool WriteColor (iDocumentNode* node, const csColor& c);
  virtual bool ParseColor (iDocumentNode* node, csColor4 &c);
  virtual bool WriteColor (iDocumentNode* node, const csColor4& c);
  virtual bool ParseMixmode (iDocumentNode* node, uint &mixmode,
    bool allowFxMesh = false);
  virtual bool WriteMixmode (iDocumentNode* node, uint mixmode,
  	bool allowFxMesh);
  virtual bool HandlePortalParameter (
	iDocumentNode* child, iLoaderContext* ldr_context,
	uint32 &flags, bool &mirror, bool &warp, int& msv,
	csMatrix3 &m, csVector3 &before, csVector3 &after,
	iString* destSector, bool& handled, bool& autoresolve);
  virtual bool ParseGradient (iDocumentNode* node, iGradient* gradient);
  virtual bool WriteGradient (iDocumentNode* node, iGradient* gradient);
  virtual bool ParseShaderVar (iDocumentNode* node, csShaderVariable& var);
  virtual csRef<iShaderVariableAccessor> ParseShaderVarExpr (
    iDocumentNode* node);
  virtual bool WriteShaderVar (iDocumentNode* node, csShaderVariable& var);
  virtual bool ParseAlphaMode (iDocumentNode* node, iStringSet* strings,
    csAlphaMode& alphaMode, bool allowAutoMode);
  virtual bool WriteAlphaMode (iDocumentNode* node, iStringSet* strings,
    const csAlphaMode& alphaMode);
  virtual bool ParseZMode (iDocumentNode* node, csZBufMode& zmode,
    bool allowZmesh);
  virtual bool WriteZMode (iDocumentNode* node, csZBufMode zmode,
    bool allowZmesh);
  virtual bool ParseKey (iDocumentNode* node, iKeyValuePair* &keyvalue);
  virtual bool WriteKey (iDocumentNode* node, iKeyValuePair* keyvalue);

  virtual csRef<iRenderBuffer> ParseRenderBuffer (iDocumentNode* node);
  virtual bool WriteRenderBuffer (iDocumentNode* node, iRenderBuffer* buffer);

  virtual csRef<iShader> ParseShaderRef (iDocumentNode* node);

  virtual void ReportError (const char* msgid, iDocumentNode* errornode,
	const char* msg, ...);
  virtual void ReportBadToken (iDocumentNode* badtokennode);
  virtual void Report (const char* msgid, int severity, 
	iDocumentNode* errornode, const char* msg, ...);

public:
  // Debugging functions.
  csPtr<iString> UnitTest ();

  virtual int GetSupportedTests () const
  { return CS_DBGHELP_UNITTEST; }
  virtual csPtr<iString> StateTest ()
  { return 0; }
  virtual csTicks Benchmark (int /*num_iterations*/)
  { return 0; }
  virtual csPtr<iString> Dump ()
  { return 0; }
  virtual void Dump (iGraphics3D* /*g3d*/)
  { }
  virtual bool DebugCommand (const char* /*cmd*/)
  { return false; }
};

}
CS_PLUGIN_NAMESPACE_END(SyntaxService)

#endif // __CS_SYNTAX_SERVICE_H__
