/*
    Copyright (C) 2000 by Jorrit Tyberghein

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

#ifndef __MOTLDR_H__
#define __MOTLDR_H__

#include "imap/reader.h"
#include "imap/writer.h"

struct iEngine;
struct iSystem;
struct iVFS;
struct iMotionManager;
struct iMotion;

/*
 *
 *	Motion Loader.
 *
 */

///
class csMotionLoader : public iLoaderPlugIn
{
private:
  iSystem *sys;
  iVFS *vfs;
  iMotionManager *motman;
  
public:

  iMotion* LoadMotion ( const char *fname );
  bool LoadMotion ( iMotion *mot, char *buf );

  /// Constructor
  csMotionLoader (iBase *);
  
  virtual ~csMotionLoader();
  
  virtual bool Initialize( iSystem *Sys);

  SCF_DECLARE_IBASE;

  virtual iBase* Parse( const char* string, iEngine *engine, iBase *context );
};

class csMotionSaver : public iSaverPlugIn
{
private:
  iSystem *sys;

public:

  csMotionSaver (iBase *);
  
  virtual ~csMotionSaver ();
  
  virtual bool Initialize ( iSystem *system );
  
  SCF_DECLARE_IBASE;
  
  virtual void WriteDown ( iBase *obj, iStrVector *string, iEngine *engine );
  
};

#endif


