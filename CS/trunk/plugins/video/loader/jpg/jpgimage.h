/*
    Copyright (C) 1998 by Tor Andersson

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

#ifndef JPGIMAGE_H
#define JPGIMAGE_H

#include "csgfx/csimage.h"
#include "igraphic/imageio.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "iutil/databuff.h"
#include "csutil/csvector.h"

/**
 * The JPG image file format loader.
 */
class csJPGImageIO : public iImageIO
{
 protected:
  csVector formats;
  iObjectRegistry* object_reg;

 public:
  SCF_DECLARE_IBASE;

  csJPGImageIO (iBase *pParent);
  virtual ~csJPGImageIO (){}

  virtual const csVector& GetDescription ();
  virtual csPtr<iImage> Load (uint8* iBuffer, uint32 iSize, int iFormat);
  virtual void SetDithering (bool iEnable);
  virtual csPtr<iDataBuffer> Save (iImage *image, const char *mime = NULL,
    const char* extraoptions = NULL);
  virtual csPtr<iDataBuffer> Save (iImage *image, iImageIO::FileFormatDescription *format = NULL,
    const char* extraoptions = NULL);

  bool Initialize (iObjectRegistry* p) 
    { object_reg = p; return true; }

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csJPGImageIO);
    virtual bool Initialize (iObjectRegistry* p) 
      { return scfParent->Initialize(p); }
  } scfiComponent;
};

/**
 * An csImageFile subclass for reading JPG files.<p>
 * This implementation needs libjpeg to read JFIF files.
 */
class ImageJpgFile : public csImageFile
{
  friend class csJPGImageIO;

private:
  iObjectRegistry* object_reg;

  /// Initialize the image object
  ImageJpgFile (int iFormat, iObjectRegistry* p) : csImageFile (iFormat) 
    { object_reg = p; };
  /// Try to read the PNG file from the buffer and return success status
  bool Load (uint8* iBuffer, uint32 iSize);
};

#endif //JPGIMAGE_H
