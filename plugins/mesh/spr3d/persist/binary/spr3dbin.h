/*
    Copyright (C) 2002 by Jorrit Tyberghein

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

#ifndef _SPR3DBINLDR_H_
#define _SPR3DBINLDR_H_

#include "imap/reader.h"
#include "imap/writer.h"
#include "imap/services.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"

struct iEngine;
struct iSkeletonLimb;
struct iReporter;
struct iPluginManager;
struct iObjectRegistry;


/**
 * Sprite 3D factory loader for Binary formatted sprites
 */
class csSprite3DBinFactoryLoader : public iBinaryLoaderPlugin
{
private:
  iReporter* reporter;
  iPluginManager* plugin_mgr;
  iObjectRegistry* object_reg;
  iSyntaxService* synldr;

  // Load a skeleton.
  bool LoadSkeleton (csParser* parser, iReporter* reporter, 
    iSkeletonLimb* limb, char* buf);

public:
  SCF_DECLARE_IBASE;

  /// Constructor.
  csSprite3DBinFactoryLoader (iBase*);

  /// Destructor.
  virtual ~csSprite3DBinFactoryLoader ();

  /// Register plugin with the system driver
  virtual bool Initialize (iObjectRegistry *object_reg);

  /// Parse data  and return a new object for it.
  virtual csPtr<iBase> Parse (void* data, 
    iLoaderContext* ldr_context, iBase* context);

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csSprite3DBinFactoryLoader);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize(p); }
  } scfiComponent;
};

/**
 * Sprite3D factory saver.
 */
class csSprite3DBinFactorySaver : public iSaverPlugin
{
private:
  iReporter* reporter;
  iPluginManager* plugin_mgr;
  iObjectRegistry* object_reg;
  iSyntaxService* synldr;

  // Save a skeleton.
  void SaveSkeleton (iSkeletonLimb* limb, csString& str);

public:
  SCF_DECLARE_IBASE;

  /// Constructor.
  csSprite3DBinFactorySaver (iBase*);

  /// Destructor.
  virtual ~csSprite3DBinFactorySaver ();

  /// Register plugin with the system driver
  virtual bool Initialize (iObjectRegistry *object_reg);

  /// Write down given object and add to string vector.
  virtual void WriteDown (iBase *obj, iFile *file);

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csSprite3DBinFactorySaver);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize(p); }
  } scfiComponent;
};


#endif // _SPR3DLDR_H_
