/*
    Copyright (C) 2001 by Norman Kr�mer

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
struct iPolygon3DStatic;
struct iThingFactoryState;
struct iEngine;
struct iSector;
struct iMaterialWrapper;
struct iReporter;
struct iLoader;
struct iMeshObjectType;

/**
 * This component provides services for other loaders to easily parse
 * properties of standard CS world syntax. This implementation will parse
 * the textual representation.
 */
class csTextSyntaxService : public iSyntaxService
{
protected:
  iObjectRegistry* object_reg;
  csRef<iMeshObjectType> thing_type;
  csRef<iReporter> reporter;
  csStringHash xmltokens;

  void OptimizePolygon (iPolygon3DStatic *p);

  void ReportV (const char* msgid, int severity, 
	iDocumentNode* errornode, const char* msg, va_list arg);
public:

  SCF_DECLARE_IBASE;
  csTextSyntaxService (iBase *parent);
  virtual ~csTextSyntaxService ();

  bool Initialize (iObjectRegistry* object_reg);

  virtual bool ParseBool (iDocumentNode* node, bool& result, bool def_result);
  virtual bool ParseMatrix (iDocumentNode* node, csMatrix3 &m);
  virtual bool ParseVector (iDocumentNode* node, csVector3 &v);
  virtual bool ParseBox (iDocumentNode* node, csBox3 &v);
  virtual bool ParseColor (iDocumentNode* node, csColor &c);
  virtual bool ParseMixmode (iDocumentNode* node, uint &mixmode);
  virtual bool ParseTextureMapping (iDocumentNode* node,
  			     const csVector3* vref, uint &texspec,
			     csVector3 &tx_orig, csVector3 &tx1,
			     csVector3 &tx2, csVector3 &len,
			     csMatrix3 &tx_m, csVector3 &tx_v,
			     csVector2 &uv_shift,
			     int &idx1, csVector2 &uv1,
			     int &idx2, csVector2 &uv2,
			     int &idx3, csVector2 &uv3,
			     const char *polyname);
  virtual  bool ParsePortal (iDocumentNode* node, iLoaderContext* ldr_context,
		  	   iPolygon3DStatic* poly_3d,
		  	   csVector &flags, bool &mirror,
  			   bool& warp, int& msv,
			   csMatrix3 &m, csVector3 &before,
			   csVector3 &after);
  virtual bool ParsePoly3d (iDocumentNode* node,
   			    iLoaderContext* ldr_context,
  			    iEngine* engine, iPolygon3DStatic* poly3d,
			    float default_texlen,
			    iThingFactoryState* thing_fact_state,
			    int vt_offset);

  virtual void ReportError (const char* msgid, iDocumentNode* errornode,
	const char* msg, ...);
  virtual void ReportBadToken (iDocumentNode* badtokennode);
  virtual void Report (const char* msgid, int severity, 
	iDocumentNode* errornode, const char* msg, ...);

private:
  /// make it plugable
  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE (csTextSyntaxService);
    virtual bool Initialize (iObjectRegistry* object_reg)
    { return scfParent->Initialize (object_reg); }
  }scfiComponent;
  friend struct eiComponent;

public:
  // Debugging functions.
  csPtr<iString> Debug_UnitTest ();

  struct DebugHelper : public iDebugHelper
  {
    SCF_DECLARE_EMBEDDED_IBASE (csTextSyntaxService);
    virtual int GetSupportedTests () const
    {
      return CS_DBGHELP_UNITTEST;
    }
    virtual csPtr<iString> UnitTest ()
    {
      return scfParent->Debug_UnitTest ();
    }
    virtual csPtr<iString> StateTest ()
    {
      return NULL;
    }
    virtual csTicks Benchmark (int /*num_iterations*/)
    {
      return 0;
    }
    virtual csPtr<iString> Dump ()
    {
      return NULL;
    }
    virtual void Dump (iGraphics3D* /*g3d*/)
    {
    }
    virtual bool DebugCommand (const char* /*cmd*/)
    {
      return false;
    }
  } scfiDebugHelper;
};

#endif // __CS_SYNTAX_SERVICE_H__
