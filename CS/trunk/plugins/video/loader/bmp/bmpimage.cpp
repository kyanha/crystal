/*
    BMPImage class
    Copyright (C) 1998 by Olivier Langlois <olanglois@sympatic.ca>

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

#include <math.h>

#include "cssysdef.h"
#include "cstypes.h"
#include "csgeom/math3d.h"
#include "bmpimage.h"
#include "cssys/csendian.h"
#include "csgfx/rgbpixel.h"
#include "csutil/databuf.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE (csBMPImageIO)
  SCF_IMPLEMENTS_INTERFACE (iImageIO)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csBMPImageIO::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csBMPImageIO);

SCF_EXPORT_CLASS_TABLE (csbmpimg)
  SCF_EXPORT_CLASS (csBMPImageIO, "crystalspace.graphic.image.io.bmp", "CrystalSpace BMP image format I/O plugin")
SCF_EXPORT_CLASS_TABLE_END

#define BMP_MIME "image/bmp"
#define BMP_EXT	 "bmp"

static iImageIO::FileFormatDescription formatlist[] =
{
  { BMP_MIME, "8 bit, palettized, RGB", CS_IMAGEIO_LOAD|CS_IMAGEIO_SAVE},
  { BMP_MIME, "8 bit, palettized, RLE8", CS_IMAGEIO_LOAD},
  { BMP_MIME, "24 bit, RGB", CS_IMAGEIO_LOAD|CS_IMAGEIO_SAVE}
};

//-----------------------------------------------------------------------------
// Some platforms require strict-alignment, which means that values of
// primitive types must be accessed at memory locations which are multiples
// of the size of those types.  For instance, a 'long' can only be accessed
// at a memory location which is a multiple of four.  Consequently, the
// following endian-conversion functions first copy the raw data into a
// variable of the proper data type using memcpy() prior to attempting to
// access it as the given type.
//-----------------------------------------------------------------------------

static inline uint16 us_endian (const uint8* ptr)
{
  uint16 n;
  memcpy(&n, ptr, sizeof(n));
  return convert_endian(n);
}

static inline uint32 ul_endian (const uint8* ptr)
{
  uint32 n;
  memcpy(&n, ptr, sizeof(n));
  return convert_endian(n);
}

static inline long l_endian (const uint8* ptr)
{
  long n;
  memcpy(&n, ptr, sizeof(n));
  return convert_endian(n);
}

#define BFTYPE(x)    us_endian((x) +  0)
#define BFSIZE(x)    ul_endian((x) +  2)
#define BFOFFBITS(x) ul_endian((x) + 10)
#define BISIZE(x)    ul_endian((x) + 14)
#define BIWIDTH(x)   l_endian ((x) + 18)
#define BIHEIGHT(x)  l_endian ((x) + 22)
#define BITCOUNT(x)  us_endian((x) + 28)
#define BICOMP(x)    ul_endian((x) + 30)
#define IMAGESIZE(x) ul_endian((x) + 34)
#define BICLRUSED(x) ul_endian((x) + 46)
#define BICLRIMP(x)  ul_endian((x) + 50)
#define BIPALETTE(x) ((x) + 54)

// Type ID
#define BM "BM" // Windows 3.1x, 95, NT, ...
#define BA "BA" // OS/2 Bitmap Array
#define CI "CI" // OS/2 Color Icon
#define CP "CP" // OS/2 Color Pointer
#define IC "IC" // OS/2 Icon
#define PT "PT" // OS/2 Pointer

// Possible values for the header size
#define WinHSize   0x28
#define OS21xHSize 0x0C
#define OS22xHSize 0xF0

// Possible values for the BPP setting
#define Mono          1  // Monochrome bitmap
#define _16Color      4  // 16 color bitmap
#define _256Color     8  // 256 color bitmap
#define HIGHCOLOR    16  // 16bit (high color) bitmap
#define TRUECOLOR24  24  // 24bit (true color) bitmap
#define TRUECOLOR32  32  // 32bit (true color) bitmap

// Compression Types
#ifndef BI_RGB
#define BI_RGB        0  // none
#define BI_RLE8       1  // RLE 8-bit / pixel
#define BI_RLE4       2  // RLE 4-bit / pixel
#define BI_BITFIELDS  3  // Bitfields
#endif

struct bmpHeader
{
  uint16 pad;
  uint8 bfTypeLo;
  uint8 bfTypeHi;
  uint32 bfSize;
  uint16 bfRes1;
  uint16 bfRes2;
  uint32 bfOffBits;
  uint32 biSize;
  uint32 biWidth;
  uint32 biHeight;
  uint16 biPlanes;
  uint16 biBitCount;
  uint32 biCompression;
  uint32 biSizeImage;
  uint32 biXPelsPerMeter;
  uint32 biYPelsPerMeter;
  uint32 biClrUsed;
  uint32 biClrImportant;
};

//---------------------------------------------------------------------------


csBMPImageIO::csBMPImageIO (iBase *pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  formats.Push (&formatlist[0]);
  formats.Push (&formatlist[1]);
  formats.Push (&formatlist[2]);
}

const csVector& csBMPImageIO::GetDescription ()
{
  return formats;
}

csPtr<iImage> csBMPImageIO::Load (uint8* iBuffer, uint32 iSize, int iFormat)
{
  ImageBMPFile* i = new ImageBMPFile (iFormat);
  if (i && !i->Load (iBuffer, iSize))
  {
    delete i;
    return NULL;
  }
  return csPtr<iImage> (i);
}

void csBMPImageIO::SetDithering (bool)
{
}

csPtr<iDataBuffer> csBMPImageIO::Save (iImage *Image, iImageIO::FileFormatDescription *,
  const char* extraoptions)
{
  extraoptions = NULL;

  if (!Image || !Image->GetImageData ())
    return NULL;

  // check if we have a format we support saving
  int format = Image->GetFormat ();
  bool palette = false;

  switch (format & CS_IMGFMT_MASK)
  {
  case CS_IMGFMT_PALETTED8:
    palette = true;
  case CS_IMGFMT_TRUECOLOR:
    break;
  default:
      // unknown format
      return NULL;
  } /* endswitch */

  // alpha channel?
  bool hasAlpha = format & CS_IMGFMT_ALPHA;
  // number of bytes per pixel in truecolor mode
  int bytesPerPixel = hasAlpha?4:3;

  if (palette && !Image->GetPalette ())
    return NULL;

  // calc size
  int w = Image->GetWidth ();
  int h = Image->GetHeight ();
  size_t len = sizeof (bmpHeader)-2 + w*h*(palette?1:bytesPerPixel) + 256*(palette?4:0);
  bmpHeader hdr;
  hdr.bfTypeLo = 'B';
  hdr.bfTypeHi = 'M';
  hdr.bfSize = little_endian_long (len);
  hdr.bfRes1 = 0;
  hdr.bfRes2 = 0;
  hdr.bfOffBits = little_endian_long (sizeof (bmpHeader)-2 + 256*(palette?4:0));
  hdr.biSize = little_endian_long (40);
  hdr.biWidth = little_endian_long (w);
  hdr.biHeight = little_endian_long (h);
  hdr.biPlanes = little_endian_short (1);
  hdr.biBitCount = little_endian_short ((palette?1:bytesPerPixel)*8);
  hdr.biCompression = little_endian_long (0);
  hdr.biSizeImage = little_endian_long (0);
  hdr.biXPelsPerMeter = little_endian_long (0);
  hdr.biYPelsPerMeter = little_endian_long (0);
  hdr.biClrUsed = little_endian_long (0);
  hdr.biClrImportant = little_endian_long (0);


  csDataBuffer *db = new csDataBuffer (len);
  unsigned char *p = (unsigned char *)db->GetData ();
  memcpy (p, &hdr.bfTypeLo, sizeof (bmpHeader)-2);

  p += sizeof (bmpHeader)-2;

  int x, y, i;
  if (palette)
  {
    csRGBpixel *pal = Image->GetPalette ();
    for (i= 0; i < 256; i++)
    {
      *p++ = pal[i].blue;
      *p++ = pal[i].green;
      *p++ = pal[i].red;
      p++;
    }

    unsigned char *data = (unsigned char *)Image->GetImageData ();
    for (y=h-1; y >= 0; y--)
      for (x=0; x < w; x++)
	*p++ = data[y*w+x];
  }
  else
  {
    csRGBpixel *pixel = (csRGBpixel *)Image->GetImageData ();
    for (y=h-1; y >= 0; y--)
    {
      csRGBpixel *line = &pixel[y*w];
      for (x=0; x < w; x++)
      {
	*p++ = line->blue;
	*p++ = line->green;
	*p++ = line->red;
	if (hasAlpha) *p++ = line->alpha;
	line++;
      }
    }
  }

  return csPtr<iDataBuffer> (db);
}

csPtr<iDataBuffer> csBMPImageIO::Save (iImage *Image, const char *mime,
  const char* extraoptions)
{
  if (!strcasecmp (mime, BMP_MIME))
    return Save (Image, (iImageIO::FileFormatDescription *)NULL,
      extraoptions);
  return NULL;
}

bool ImageBMPFile::Load (uint8* iBuffer, uint32 iSize)
{
  if ((memcmp (iBuffer, BM, 2) == 0) && BISIZE(iBuffer) == WinHSize)
    return LoadWindowsBitmap (iBuffer, iSize);
  return false;
}

bool ImageBMPFile::LoadWindowsBitmap (uint8* iBuffer, uint32 iSize)
{
  set_dimensions (BIWIDTH(iBuffer), BIHEIGHT(iBuffer));
  const int bmp_size = Width * Height;

  uint8 *iPtr = iBuffer + BFOFFBITS(iBuffer);

  // No alpha for BMP... at least not always
  Format &= ~CS_IMGFMT_ALPHA;

  // The last scanline in BMP corresponds to the top line in the image
  int  buffer_y = Width * (Height - 1);
  bool blip     = false;

  if (BITCOUNT(iBuffer) == _256Color)
  {
    uint8    *buffer  = new uint8 [bmp_size];
    csRGBpixel *palette = new csRGBpixel [256];
    csRGBpixel *pwork   = palette;
    uint8    *inpal   = BIPALETTE(iBuffer);
    int scanlinewidth = 4 * ((Width+3) / 4);
    int color;
    for (color = 0; color < 256; color++, pwork++)
    {
      // Whacky BMP palette is in BGR order.
      pwork->blue  = *inpal++;
      pwork->green = *inpal++;
      pwork->red   = *inpal++;
      inpal++; // Skip unused byte.
    }

    if (BICOMP(iBuffer) == BI_RGB)
    {
      // Read the pixels from "top" to "bottom"
      while (iPtr < iBuffer + iSize && buffer_y >= 0)
      {
        memcpy (buffer + buffer_y, iPtr, Width);
        iPtr += scanlinewidth;
        buffer_y -= Width;
      } /* endwhile */
    }
    else if (BICOMP(iBuffer) == BI_RLE8)
    {
      // Decompress pixel data
      uint8 rl, rl1, i;			// runlength
      uint8 clridx, clridx1;		// colorindex
      int buffer_x = 0;
      while (iPtr < iBuffer + iSize && buffer_y >= 0)
      {
        rl = rl1 = *iPtr++;
        clridx = clridx1 = *iPtr++;
        if (rl == 0)
           if (clridx == 0)
        {
    	  // new scanline
          if (!blip)
          {
            // if we didnt already jumped to the new line, do it now
            buffer_x  = 0;
            buffer_y -= Width;
          }
          continue;
        }
        else if (clridx == 1)
          // end of bitmap
          break;
        else if (clridx == 2)
        {
          // next 2 bytes mean column- and scanline- offset
          buffer_x += *iPtr++;
          buffer_y -= (Width * (*iPtr++));
          continue;
        }
        else if (clridx > 2)
          rl1 = clridx;

        for ( i = 0; i < rl1; i++ )
        {
          if (!rl) clridx1 = *iPtr++;
          buffer [buffer_y + buffer_x] = clridx1;

          if (++buffer_x >= Width)
          {
            buffer_x  = 0;
            buffer_y -= Width;
            blip = true;
          }
          else
            blip = false;
        }
        // pad in case rl == 0 and clridx in [3..255]
        if (rl == 0 && (clridx & 0x01)) iPtr++;
      }
    }
    // Now transform the image data to target format
    convert_pal8 (buffer, palette);
    return true;
  }
  else if (BITCOUNT(iBuffer) == TRUECOLOR24)
  {
    csRGBpixel *buffer = new csRGBpixel [bmp_size];

    int x;

    while (iPtr < iBuffer + iSize && buffer_y >= 0)
    {
      csRGBpixel *d = buffer + buffer_y;
      for (x = Width; x; x--)
      {
        d->blue    = *iPtr++;
        d->green   = *iPtr++;
        d->red     = *iPtr++;
        d++;
      } /* endfor */

      buffer_y -= Width;
    }
    // Now transform the image data to target format
    convert_rgba (buffer);
    return true;
  }
  else if (BITCOUNT(iBuffer) == TRUECOLOR32)
  {
    Format |= CS_IMGFMT_ALPHA;
    csRGBpixel *buffer = new csRGBpixel [bmp_size];

    int x;

    while (iPtr < iBuffer + iSize && buffer_y >= 0)
    {
      csRGBpixel *d = buffer + buffer_y;
      for (x = Width; x; x--)
      {
        d->blue    = *iPtr++;
        d->green   = *iPtr++;
        d->red     = *iPtr++;
	d->alpha   = *iPtr++;
        d++;
      } /* endfor */

      buffer_y -= Width;
    }
    // Now transform the image data to target format
    convert_rgba (buffer);
    return true;
  }

  return false;
}

