/*
    Copyright (C) 1998 by Jorrit Tyberghein

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

#ifndef __CS_IMGMULTIPLEX_H__
#define __CS_IMGMULTIPLEX_H__

#include "csgfx/csimage.h"
#include "igraphic/imageio.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "iutil/databuff.h"
#include "iutil/plugin.h"
#include "csutil/csvector.h"
#include "csutil/cfgacc.h"
#include "csutil/refarr.h"

/**
 * Through this plugin you can load/save a set of different formats.
 * It works by loading other plzugins and transfers execution to them.
 */
class csMultiplexImageIO : public iImageIO
{
 protected:
  csRefArray<iImageIO> list;
  csVector formats;
  csConfigAccess config;
  csRef<iStrVector> classlist;
  csRef<iPluginManager> plugin_mgr;
  bool global_dither;

  void StoreDesc (const csVector& format);
  /**
   * load the next plugin in the class list
   * returns true if more plugins are in the list
   */
  bool LoadNextPlugin ();

 public:
  SCF_DECLARE_IBASE;

  csMultiplexImageIO (iBase *pParent);
  virtual ~csMultiplexImageIO ();

  virtual bool Initialize (iObjectRegistry*);
  virtual const csVector& GetDescription ();
  virtual csPtr<iImage> Load (uint8* iBuffer, uint32 iSize, int iFormat);
  virtual void SetDithering (bool iEnable);
  virtual csPtr<iDataBuffer> Save (iImage *image, const char *mime = 0,
    const char* extraoptions = 0);
  virtual csPtr<iDataBuffer> Save (iImage *image, iImageIO::FileFormatDescription *format = 0,
    const char* extraoptions = 0);

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csMultiplexImageIO);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize(p); }
  } scfiComponent;
};

#endif // __CS_IMGMULTIPLEX_H__
