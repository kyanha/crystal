/*
    Copyright (C) 2000 by Jorrit Tyberghein
    Copyright (C) 2001 by W.C.A. Wijngaards

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

#ifndef _SPIRALLDR_H_
#define _SPIRALLDR_H_

#include "imap/reader.h"
#include "imap/writer.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"

struct iEngine;
struct iPluginManager;
struct iObjectRegistry;

/**
 * Spiral factory loader.
 */
class csSpiralFactoryLoader : public iLoaderPlugin
{
private:
  iPluginManager* plugin_mgr;
  iObjectRegistry* object_reg;

public:
  SCF_DECLARE_IBASE;

  /// Constructor.
  csSpiralFactoryLoader (iBase*);

  /// Destructor.
  virtual ~csSpiralFactoryLoader ();

  bool Initialize (iObjectRegistry* p);

  /// Parse a given string and return a new object for it.
  virtual iBase* Parse (const char* string, iLoaderContext* ldr_context,
  	iBase* context);

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csSpiralFactoryLoader);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize (p); }
  } scfiComponent;
  friend struct eiComponent;
};

/**
 * Spiral factory saver.
 */
class csSpiralFactorySaver : public iSaverPlugin
{
private:
  iPluginManager* plugin_mgr;
  iObjectRegistry* object_reg;

public:
  SCF_DECLARE_IBASE;

  /// Constructor.
  csSpiralFactorySaver (iBase*);

  /// Destructor.
  virtual ~csSpiralFactorySaver ();

  bool Initialize (iObjectRegistry* p);

  /// Write down given object and add to string vector.
  virtual void WriteDown (iBase *obj, iFile *file);

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csSpiralFactorySaver);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize (p); }
  } scfiComponent;
  friend struct eiComponent;
};

/**
 * Spiral loader.
 */
class csSpiralLoader : public iLoaderPlugin
{
private:
  iPluginManager* plugin_mgr;
  iObjectRegistry* object_reg;

public:
  SCF_DECLARE_IBASE;

  /// Constructor.
  csSpiralLoader (iBase*);

  /// Destructor.
  virtual ~csSpiralLoader ();

  bool Initialize (iObjectRegistry* p);

  /// Parse a given string and return a new object for it.
  virtual iBase* Parse (const char* string, iLoaderContext* ldr_context,
  	iBase* context);

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csSpiralLoader);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize (p); }
  } scfiComponent;
  friend struct eiComponent;
};

/**
 * Spiral saver.
 */
class csSpiralSaver : public iSaverPlugin
{
private:
  iPluginManager* plugin_mgr;
  iObjectRegistry* object_reg;

public:
  SCF_DECLARE_IBASE;

  /// Constructor.
  csSpiralSaver (iBase*);

  /// Destructor.
  virtual ~csSpiralSaver ();

  bool Initialize (iObjectRegistry* p);

  /// Write down given object and add to string vector.
  virtual void WriteDown (iBase *obj, iFile *file);

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csSpiralSaver);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize (p); }
  } scfiComponent;
  friend struct eiComponent;
};

#endif // _SPIRALLDR_H_
