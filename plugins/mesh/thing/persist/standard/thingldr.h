/*
    Copyright (C) 2001 by Jorrit Tyberghein

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

#ifndef __CS_THINGLDR_H__
#define __CS_THINGLDR_H__

#include "imap/reader.h"
#include "imap/writer.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "csutil/strhash.h"

struct iEngine;
struct iPluginManager;
struct iObjectRegistry;
struct iSyntaxService;
struct iReporter;

/**
 * Private information during the loading process of a thing.
 */
class ThingLoadInfo
{
public:
  iMaterialWrapper* default_material;
  float default_texlen;
  bool use_mat_set;
  char* mat_set_name;

  ThingLoadInfo () : default_material (NULL),
    default_texlen (1),
    use_mat_set (false), mat_set_name (NULL)
    {}

  void SetTextureSet (const char* name)
  {
    delete [] mat_set_name;
    mat_set_name = new char [strlen (name) + 1];
    strcpy (mat_set_name, name);
  }
};

/**
 * Thing loader.
 */
class csThingLoader : public iLoaderPlugin
{
private:
  iObjectRegistry* object_reg;
  csRef<iSyntaxService> synldr;
  csRef<iReporter> reporter;
  csStringHash xmltokens;

  bool ParseCurve (iCurve* curve, iLoaderContext* ldr_context,
  	iDocumentNode* node);
  bool LoadThingPart (iThingEnvironment* te,
  	iDocumentNode* node, iLoaderContext* ldr_context,
	iObjectRegistry* object_reg, iReporter* reporter,
	iSyntaxService *synldr, ThingLoadInfo& info,
	iEngine* engine, iThingState* thing_state,
	iThingFactoryState* thing_fact_state,
	int vt_offset, bool isParent);

public:
  SCF_DECLARE_IBASE;

  /// Constructor.
  csThingLoader (iBase*);
  /// Destructor.
  virtual ~csThingLoader ();

  bool Initialize (iObjectRegistry* p);

  /// Parse a given node and return a new object for it.
  virtual csPtr<iBase> Parse (iDocumentNode* node,
    iLoaderContext* ldr_context, iBase* context);

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csThingLoader);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize (p); }
  } scfiComponent;
  friend struct eiComponent;
};

/**
 * Thing saver.
 */
class csThingSaver : public iSaverPlugin
{
private:
  iObjectRegistry* object_reg;
  csRef<iReporter> reporter;

public:
  SCF_DECLARE_IBASE;

  /// Constructor.
  csThingSaver (iBase*);
  /// Destructor.
  virtual ~csThingSaver ();

  bool Initialize (iObjectRegistry* p);

  /// Write down given object and add to string vector.
  virtual void WriteDown (iBase *obj, iFile *file);

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csThingSaver);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize (p); }
  } scfiComponent;
  friend struct eiComponent;
};

#endif // __CS_THINGLDR_H__
