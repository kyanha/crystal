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

/* rd_tga.c - read a TrueVision Targa file
**
** Partially based on tgatoppm.c from pbmplus (just a big hack :)
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/

#include <math.h>
#include <stdio.h>

#include "cssysdef.h"
#include "cssys/csendian.h"
#include "csgfx/packrgb.h"

#include "walimage.h"
#include "walpal.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE (csWALImageIO)
  SCF_IMPLEMENTS_INTERFACE (iImageIO)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csWALImageIO::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csWALImageIO);

SCF_EXPORT_CLASS_TABLE (cswalimg)
  SCF_EXPORT_CLASS (csWALImageIO, "crystalspace.graphic.image.io.wal", "CrystalSpace WAL image format I/O plugin")
SCF_EXPORT_CLASS_TABLE_END

/* Header definition. */
struct WALHeader
{
  unsigned char Name[32];		// Internal Name
  unsigned int width;			// Width of largest mipmap
  unsigned int height;			// Height of largest mipmap
  unsigned int offsets[4];		// Offset to 4 mipmaps in file
  char nextframe[32];			// Name of next file in animation if any
  unsigned int flags;			// ??
  unsigned int contents;		// ??
  unsigned int value;			// ??
} _WALHeader;


static iImageIO::FileFormatDescription formatlist[1] =
{
  {"image/wal", "8 bit, palettized", CS_IMAGEIO_LOAD}
};


csWALImageIO::csWALImageIO (iBase *pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  formats.Push (&formatlist[0]);
}

const csVector& csWALImageIO::GetDescription ()
{
  return formats;
}

csPtr<iImage> csWALImageIO::Load (uint8* iBuffer, uint32 iSize, int iFormat)
{
  if (iSize < sizeof(WALHeader))
    return 0;
  ImageWALFile* i = new ImageWALFile (iFormat);
  if (i && !i->Load (iBuffer, iSize))
  {
    delete i;
    return 0;
  }
  return csPtr<iImage> (i);
}

void csWALImageIO::SetDithering (bool)
{
}

csPtr<iDataBuffer> csWALImageIO::Save (iImage *image, iImageIO::FileFormatDescription *format,
  const char* extraoptions)
{
  image = 0;
  format = 0;
  extraoptions = 0;

  return 0;
}

csPtr<iDataBuffer> csWALImageIO::Save (iImage *image, const char *mime,
  const char* extraoptions)
{
  image = 0;
  mime = 0;
  extraoptions = 0;

  return 0;
}

//---------------------------------------------------------------------------

bool ImageWALFile::Load (uint8* iBuffer, uint32 iSize)
{
  WALHeader head = *(WALHeader *)iBuffer;
  head.width = little_endian_long (head.width);
  head.height = little_endian_long (head.height);
  head.offsets [0] = little_endian_long (head.offsets [0]);
  head.offsets [3] = little_endian_long (head.offsets [3]);

  Format &= ~CS_IMGFMT_ALPHA;

  // There's no id-tag in .WAL files, so the only way I know to check
  // if it's a wal, is to use this method. Hope it works
  uint32 chkfilesize = head.offsets [3] + ((head.height / 8) * (head.width / 8));
  if (chkfilesize != iSize)
    return false;

  set_dimensions (head.width, head.height);

  // There are 4 mipmaps in a wal-file, but we just use the first and discard the rest
  uint8 *buffer = new uint8 [Width * Height];

  memcpy (buffer, iBuffer + head.offsets[0], Width * Height);
  const csRGBcolor *WALpal = csUnpackRGBtoRGBcolor (WALpalette, 256);
  convert_pal8 (buffer, WALpal);
  csDiscardUnpackedRGBcolor (WALpal);

  return true;
}
