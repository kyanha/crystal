/*
    DSS image file format support for CrystalSpace 3D library
    Copyright (C) 2003 by Matze Braun <matze@braunis.de>

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
#include "cssysdef.h"

#include "dds.h"
#include "ddsloader.h"

#define DDS_MIME "image/dds"

struct iImageIO::FileFormatDescription formatlist[] =
{
  {DDS_MIME, "RGBA", CS_IMAGEIO_LOAD}
};

csDDSImageIO::csDDSImageIO (iBase* parent)
{
  SCF_CONSTRUCT_IBASE(parent);

  int formatcount =
    sizeof(formatlist)/sizeof(iImageIO::FileFormatDescription);
  for (int i=0; i < formatcount; i++)
    formats.Push (&formatlist[i]);
}

csDDSImageIO::~csDDSImageIO ()
{
}

bool csDDSImageIO::Initialize (iObjectRegistry* )
{
  return true;
}

const csVector& csDDSImageIO::GetDescription ()
{
  return formats;
}

void csDDSImageIO::SetDithering (bool)
{
}

csPtr<iImage> csDDSImageIO::Load (uint8* buffer, uint32 size, int format)
{
  dds::Loader* loader = new dds::Loader;

  loader->SetSource (buffer, (size_t) size);
  if (!loader->IsDDS())                       
  {
    delete loader;
    return NULL;
  }

  csDDSImageFile* image = new csDDSImageFile(format);
  if (!image->Load (loader))
  {
    delete loader;
    return NULL;
  }

  delete loader;
  return csPtr<iImage> (image);
}

csPtr<iDataBuffer> csDDSImageIO::Save (iImage* image,
    iImageIO::FileFormatDescription* format, const char* options)
{
  return NULL;
}

csPtr<iDataBuffer> csDDSImageIO::Save (iImage* image, const char* mime,
				       const char* options)
{
  return NULL;
}

//---------------------------------------------------------------------------

csDDSImageFile::csDDSImageFile (int format)
  : csImageFile (format), mipmaps(0), mipmapcount(0)
{
}

csDDSImageFile::~csDDSImageFile ()
{
}

bool csDDSImageFile::Load (dds::Loader* loader)
{
  set_dimensions (loader->GetWidth(), loader->GetHeight());
  if (loader->GetBytesPerPixel() != 4)
  {
    printf ("WARNING: DDS loader only supports 32 bit images at the moment.\n");
    return false;
  }
  uint8* img = loader->LoadImage ();
  if (!img)
    return false;
  convert_rgba ((csRGBpixel*) img);

  mipmapcount = loader->GetMipmapCount ();
  for (int i=0;i<mipmapcount;i++)
  {
    uint8* img = loader->LoadMipmap(i);
    if (!img)
      return false;
    csDDSImageFile* image = new csDDSImageFile(Format);
    image->set_dimensions (loader->GetWidth() >> (i+1),
			   loader->GetHeight() >> (i+1));
    image->convert_rgba((csRGBpixel*) img);
    mipmaps.Push (image);
  }

  return true;
}

csPtr<iImage> csDDSImageFile::MipMap (int step, csRGBpixel* transp)
{
  if (step==0 || step>mipmapcount || transp)
    return csImageFile::MipMap (step, transp);

  return csPtr<iImage> (mipmaps[step-1]);
}

int csDDSImageFile::HasMipmaps ()
{
  return mipmapcount;
}

//---------------------------------------------------------------------------
// The usual plugin stuff

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE (csDDSImageIO)
  SCF_IMPLEMENTS_INTERFACE (iImageIO)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_FACTORY(csDDSImageIO);

SCF_EXPORT_CLASS_TABLE (csddsimg)
  SCF_EXPORT_CLASS (csDDSImageIO, "crystalspace.graphic.image.io.dds",
      "DDS image format I/O plugin")
SCF_EXPORT_CLASS_TABLE_END

