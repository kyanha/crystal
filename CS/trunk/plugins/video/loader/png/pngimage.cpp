/*
    PNG image file format support for CrystalSpace 3D library
    Copyright (C) 1998,2000 by Andrew Zabolotny <bit@eltech.ru>

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
#include "csgfx/rgbpixel.h"
#include "csutil/databuf.h"
#include "cssys/csendian.h"
#include <math.h>

extern "C"
{
#define Byte z_Byte     /* Kludge to avoid conflicting typedef in zconf.h */
#include <zlib.h>
#undef Byte
#include <png.h>
}

#include "pngimage.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE (csPNGImageIO)
  SCF_IMPLEMENTS_INTERFACE (iImageIO)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csPNGImageIO::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csPNGImageIO);

SCF_EXPORT_CLASS_TABLE (cspngimg)
  SCF_EXPORT_CLASS (csPNGImageIO, "crystalspace.graphic.image.io.png",
		"CrystalSpace PNG image format I/O plugin")
SCF_EXPORT_CLASS_TABLE_END

#define PNG_MIME "image/png"

struct datastore{
  unsigned char *data;
  long pos;
  long length;

  datastore () { data = NULL; pos = 0; length = 0; }
  ~datastore () { free (data); }
};

static void png_write (png_structp png, png_bytep data, png_size_t length)
{
  datastore *ds = (datastore *)png->io_ptr;
  if (ds->pos + (long)length > ds->length)
  {
    ds->data = (unsigned char*)realloc (ds->data, ds->pos + (long)length);
    if (!ds->data)
      png_error (png, "memory allocation error");
    else
      ds->length = ds->pos + length;
  }
  memcpy (ds->data + ds->pos, data, length);
  ds->pos += length;
}

void png_flush (png_structp)
{
}

static iImageIO::FileFormatDescription formatlist[5] =
{
  {PNG_MIME, "Gray", CS_IMAGEIO_LOAD},
  {PNG_MIME, "GrayAlpha", CS_IMAGEIO_LOAD},
  {PNG_MIME, "Palette", CS_IMAGEIO_LOAD|CS_IMAGEIO_SAVE},
  {PNG_MIME, "RGB", CS_IMAGEIO_LOAD|CS_IMAGEIO_SAVE},
  {PNG_MIME, "RGBA", CS_IMAGEIO_LOAD|CS_IMAGEIO_SAVE}
};

csPNGImageIO::csPNGImageIO (iBase *pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  formats.Push (&formatlist[0]);
  formats.Push (&formatlist[1]);
  formats.Push (&formatlist[2]);
  formats.Push (&formatlist[3]);
  formats.Push (&formatlist[4]);
}

const csVector& csPNGImageIO::GetDescription ()
{
  return formats;
}

iImage *csPNGImageIO::Load (uint8* iBuffer, uint32 iSize, int iFormat)
{
  ImagePngFile* i = new ImagePngFile (iFormat);
  if (i && !i->Load (iBuffer, iSize))
  {
    delete i;
    return NULL;
  }
  return i;
}

void csPNGImageIO::SetDithering (bool)
{
}

static inline unsigned sqr (int x)
{
  return (x * x);
}

int closest_index (iImage *Image, csRGBpixel *iColor)
{
  csRGBpixel *Palette = Image->GetPalette();

  if (!Palette)
    return -1;

  int closest_idx = -1;
  unsigned closest_dst = (unsigned)-1;

  int idx;
  for (idx = 0; idx < 256; idx++)
  {
    unsigned dst = sqr (iColor->red   - Palette [idx].red)   * R_COEF_SQ +
                   sqr (iColor->green - Palette [idx].green) * G_COEF_SQ +
                   sqr (iColor->blue  - Palette [idx].blue)  * B_COEF_SQ;
    if (dst == 0)
      return idx;
    if (dst < closest_dst)
    {
      closest_dst = dst;
      closest_idx = idx;
    } /* endif */
  }
  return closest_idx;
}

iDataBuffer *csPNGImageIO::Save (iImage *Image, iImageIO::FileFormatDescription *,
  const char* extraoptions)
{
  if (!Image)
    return NULL;

  int compress = 6;
  bool interlace = false;
  /*
     parse output options.
     options are a comma-separated list and can be either
     'option' or 'option=value'.

     supported options:
       compress=#   image compression, 0..100 higher values give smaller files,
		    but take longer to encode.
       progressive  interlaced output.
   
     examples:
       compress=50
       progressive,compress=30
   */
  const char *current_opt = extraoptions;
  while (current_opt && *current_opt)
  {
    if (*current_opt == ',') current_opt++;

    const char *opt_end = strchr (current_opt, ',');
    if (!opt_end) opt_end = strchr (current_opt, 0);

    char *opt_key = new char[opt_end - current_opt + 1];
    strncpy (opt_key, current_opt, opt_end - current_opt);
    opt_key[opt_end - current_opt] = 0;
    current_opt = opt_end;

    char *opt_value = strchr (opt_key, '=');
    if (opt_value) *opt_value++ = 0;

    if (!strcmp (opt_key, "compress"))
    {
      if (opt_value)
	compress = atoi (opt_value) / 10;
    }
    else if (!strcmp (opt_key, "progressive"))
    {
      interlace = true;
    }

    delete opt_key;
  }

  if (compress < 0) compress = 0;
  if (compress > 9) compress = 9;

  datastore ds;

  png_structp png = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!png)
  {
error1:
    return NULL;
  }

  png_set_compression_level (png, compress);

  /* Allocate/initialize the image information data. */
  png_infop info = png_create_info_struct (png);
  if (info == NULL)
  {
    png_destroy_write_struct (&png, (png_infopp)NULL);
error2:
    goto error1;
  }

  /* Catch processing errors */
  if (setjmp(png->jmpbuf))
  {
    /* If we get here, we had a problem reading the file */
    png_destroy_write_struct (&png, &info);
    goto error2;
  }

  /* Set up the output function. We could use standard file output
   * routines but for some (strange) reason if we write the file inside
   * the shared PNG library this causes problems at least on OS/2...
   */
  png_set_write_fn (png, (png_voidp)&ds, png_write, png_flush);

  /* Set the image information here.  Width and height are up to 2^31,
   * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
   * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
   * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
   * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
   * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
   * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
   */
  int format = Image->GetFormat ();
  int colortype, rowlen;
  int width = Image->GetWidth (), height = Image->GetHeight ();
  switch (format & CS_IMGFMT_MASK)
  {
    case CS_IMGFMT_NONE:
      // plain alphamaps not supported
      goto error2;
    case CS_IMGFMT_PALETTED8:
      colortype = PNG_COLOR_TYPE_PALETTE;
      rowlen = Image->GetWidth ();
      break;
    case CS_IMGFMT_TRUECOLOR:
      colortype = (format & CS_IMGFMT_ALPHA) ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB;
      rowlen = Image->GetWidth () * sizeof (csRGBpixel);
      break;
    default:
      // unknown format
      goto error2;
  } /* endswitch */
  png_set_IHDR (png, info, width, height, 8, colortype,
    interlace?PNG_INTERLACE_ADAM7:PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, 
    PNG_FILTER_TYPE_BASE);

  png_colorp palette = NULL;
  /* set the palette if there is one. */
  if (colortype & PNG_COLOR_MASK_PALETTE)
  {
    csRGBpixel *pal = Image->GetPalette ();
    
    palette = (png_colorp)malloc (256 * sizeof (png_color));
    int i;
    for (i = 0; i < 256; i++)
    {
      palette [i].red   = pal [i].red;
      palette [i].green = pal [i].green;
      palette [i].blue  = pal [i].blue;
    } /* endfor */
    int max_color = 0;
    // seek maximum color index used in the image
    int n = Image->GetWidth() * Image->GetHeight();
    uint8 *imagedata = (uint8*)Image->GetImageData();
    while (n > 0)
    {
      max_color = MAX(max_color, *imagedata);
      imagedata++;
      n--;
    }
    png_set_PLTE (png, info, palette, max_color+1);

    if (Image->HasKeycolor())
    {
      // Get the keycolor palette index and write the appropriate
      // tRNS chunk.
      int key_r, key_g, key_b;
      Image->GetKeycolor (key_r, key_g, key_b);
      csRGBpixel key (key_r, key_g, key_b);
      int key_index = closest_index (Image, &key);
      png_bytep trans = new png_byte[key_index + 1];
      memset (trans, 0xff, key_index);
      trans[key_index] = 0;
      png_set_tRNS (png, info, trans, key_index+1, NULL);
      delete[] trans;
    }
  }
  else
  {
    //Write tRNS chunk with keycolor.
    if (Image->HasKeycolor())
    {
      int key_r, key_g, key_b;
      Image->GetKeycolor (key_r, key_g, key_b);
      png_color_16 trans;
      memset (&trans, 0, sizeof(trans));
      trans.red = big_endian_short (key_r << 8);
      trans.green = big_endian_short (key_g << 8);
      trans.blue = big_endian_short (key_b << 8);
      png_set_tRNS (png, info, NULL, NULL, &trans);
    }
  }

  /* otherwise, if we are dealing with a color image then */
  png_color_8 sig_bit;
  memset (&sig_bit, 0, sizeof (sig_bit));
  sig_bit.red = 8;
  sig_bit.green = 8;
  sig_bit.blue = 8;
  /* if the image has an alpha channel then */
  if (format & CS_IMGFMT_ALPHA)
    sig_bit.alpha = 8;
  png_set_sBIT (png, info, &sig_bit);

  /* Write the file header information. */
  png_write_info (png, info);

  /* Get rid of filler (OR ALPHA) bytes, pack XRGB/RGBX/ARGB/RGBA into
   * RGB (4 channels -> 3 channels). The second parameter is not used.
   */
  if (((format & CS_IMGFMT_MASK) == CS_IMGFMT_TRUECOLOR)
   && !(format & CS_IMGFMT_ALPHA))
    png_set_filler (png, 0xff, PNG_FILLER_AFTER);

  /* Writing the image. Interlacing is handled automagically. */
  png_bytep *row_pointers = new png_bytep [height];
  uint8 *ImageData = (uint8 *)Image->GetImageData ();
  int i;
  for (i = 0; i < height; i++)
    row_pointers [i] = ImageData + i * rowlen;
  png_write_image (png, row_pointers);

  /* It is REQUIRED to call this to finish writing the rest of the file */
  png_write_end (png, info);

  /* clean up after the write, and free any memory allocated */
  png_destroy_write_struct (&png, &info);
  if (palette)
    free(palette);

  /* Free the row pointers */
  delete [] row_pointers;

  /* make the iDataBuffer to return */
  csDataBuffer *db = new csDataBuffer (ds.pos);
  memcpy (db->GetData (), ds.data, ds.pos);

  /* that's it */
  return db;
}

iDataBuffer *csPNGImageIO::Save (iImage *Image, const char *mime,
  const char* extraoptions)
{
  if (!strcasecmp (mime, PNG_MIME))
    return Save (Image, (iImageIO::FileFormatDescription *)NULL,
      extraoptions);
  return NULL;
}

//---------------------------------------------------------------------------

struct ImagePngRawData
{
  // The buffer to "read" from
  uint8 *r_data;
  // The buffer size
  size_t r_size;
};

void ImagePngRead (png_structp png, png_bytep data, png_size_t size)
{
  ImagePngRawData *self = (ImagePngRawData *) png->io_ptr;

  if (self->r_size < size)
    png_error (png, "Read Error");
  else
  {
    memcpy (data, self->r_data, size);
    self->r_size -= size;
    self->r_data += size;
  } /* endif */
}

bool ImagePngFile::Load (uint8 *iBuffer, uint32 iSize)
{
  size_t rowbytes, exp_rowbytes;
  png_infop info;

  if (!png_check_sig (iBuffer, iSize))
    return false;

  png_structp png =
    png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!png)
  {
nomem:
    FreeImage ();
    return false;
  }
  info = png_create_info_struct (png);
  if (!info)
  {
nomem2:
    png_destroy_read_struct (&png, (png_infopp) NULL, (png_infopp) NULL);
    goto nomem;
  }

  if (setjmp (png->jmpbuf))
    // If we get here, we had a problem reading the file
    goto nomem2;

  ImagePngRawData raw = { iBuffer, iSize };
  png_set_read_fn (png, &raw, ImagePngRead);

  png_read_info (png, info);

  // Get picture info
  png_uint_32 Width, Height;
  int bit_depth, color_type;

  png_get_IHDR (png, info, &Width, &Height, &bit_depth, &color_type,
    NULL, NULL, NULL);

  if (bit_depth > 8)
  {
    // tell libpng to strip 16 bit/color files down to 8 bits/color
    png_set_strip_16 (png);
    bit_depth = 8;
  }
  else if (bit_depth < 8)
    // Expand pictures with less than 8bpp to 8bpp
    png_set_packing (png);

  volatile enum { imgRGB, imgPAL, imgGrayAlpha } ImageType;
  int keycolor_index = -1;

  switch (color_type)
  {
    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_GRAY_ALPHA:
      if ((Format & CS_IMGFMT_ALPHA) && (color_type & PNG_COLOR_MASK_ALPHA))
      {
	ImageType = imgGrayAlpha;
      }
      else
      {
        // Check if we have keycolor transparency.
	ImageType = imgPAL;
	png_set_strip_alpha (png);
	if (png_get_valid (png, info, PNG_INFO_tRNS))
	{
	  png_color_16p trans_values;
	  png_get_tRNS (png, info, NULL, NULL, &trans_values);
	  has_keycolour = true;
	  keycolour_r = convert_endian (trans_values->gray) & 0xff;
	  keycolour_g = convert_endian (trans_values->gray) & 0xff;
	  keycolour_b = convert_endian (trans_values->gray) & 0xff;
	}
      }
      break;
    case PNG_COLOR_TYPE_PALETTE:
      ImageType = imgPAL;
      // If we need alpha, take it. If we don't, strip it.
      if (Format & CS_IMGFMT_ALPHA)
      {
	if (png_get_valid (png, info, PNG_INFO_tRNS))
	{
	  // tRNS chunk. Every palette entry gets its own alpha value.
	  png_bytep trans;
	  int num_trans;
	  png_get_tRNS (png, info, &trans, &num_trans, NULL);
	  
	  // see if there is a single entry w/ alpha==0 and all other 255.
	  // if yes use keycolor transparency.
	  bool only_binary_trans = true;
	  for (int i = 0; (i < num_trans)&& only_binary_trans; i++)
	  {
	    if (trans[i] != 0xff)
	    {
	      only_binary_trans = only_binary_trans && (trans[i] == 0) 
		&& (keycolor_index == -1);
	      keycolor_index = i;
	    }
	  }
	  if (!only_binary_trans)
	  {
	    keycolor_index = -1;
	    png_set_palette_to_rgb (png);
	    png_set_tRNS_to_alpha (png);
	    ImageType = imgRGB;
	  }
	  else
	    Format &= ~CS_IMGFMT_ALPHA;
	}
      }
      break;
    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_RGB_ALPHA:
      ImageType = imgRGB;
      // If there is no alpha information, fill with 0xff
      if (!(color_type & PNG_COLOR_MASK_ALPHA))
      {
        Format &= ~CS_IMGFMT_ALPHA;
        // Check if we have keycolor transparency.
        if (png_get_valid (png, info, PNG_INFO_tRNS))
	{
	  png_color_16p trans_values;
	  png_get_tRNS (png, info, NULL, NULL, &trans_values);
	  has_keycolour = true;
	  keycolour_r = convert_endian (trans_values->red) & 0xff;
	  keycolour_g = convert_endian (trans_values->green) & 0xff;
	  keycolour_b = convert_endian (trans_values->blue) & 0xff;
	}
        png_set_filler (png, 0xff, PNG_FILLER_AFTER);
      }
      break;
    default:
      goto nomem2;
  }

  // Update structure with the above settings
  png_read_update_info (png, info);

  // Allocate the memory to hold the image
  set_dimensions (Width, Height);
  if (ImageType == imgRGB)
    exp_rowbytes = Width * sizeof (csRGBpixel);
  else if (ImageType == imgGrayAlpha)
    exp_rowbytes = Width * 2;
  else
    exp_rowbytes = Width;

  rowbytes = png_get_rowbytes (png, info);
  if (rowbytes != exp_rowbytes)
    goto nomem2;                        // Yuck! Something went wrong!

  png_bytep * const row_pointers = new png_bytep[Height];

  if (setjmp (png->jmpbuf))             // Set a new exception handler
  {
    delete [] row_pointers;
    goto nomem2;
  }

  void *NewImage = NULL;
  if (ImageType == imgRGB)
    NewImage = new csRGBpixel [Width * Height];
  else if (ImageType == imgGrayAlpha)
    NewImage = new uint8 [Width * Height * 2];
  else
    NewImage = new uint8 [Width * Height];
  if (!NewImage)
    goto nomem2;

  for (png_uint_32 row = 0; row < Height; row++)
    row_pointers [row] = ((png_bytep)NewImage) + row * rowbytes;

  // Read image data
  png_read_image (png, row_pointers);

  // read rest of file, and get additional chunks in info_ptr
  png_read_end (png, (png_infop)NULL);

  if (ImageType == imgRGB)
    convert_rgba ((csRGBpixel *)NewImage);
  else if (ImageType == imgPAL)
  {
    csRGBcolor graypal [256];
    csRGBcolor *palette = NULL;
    int colors;
    if (!png_get_PLTE (png, info, (png_colorp *)&palette, &colors))
    {
      // This is a grayscale image, build a grayscale palette
      palette = graypal;
      int entries = (1 << bit_depth) - 1;
      colors = entries + 1;
	  int i;
      for (i = 0; i <= entries; i++)
        palette [i].red = palette [i].green = palette [i].blue =
          (i * 255) / entries;
    }
    if (keycolor_index != -1)
    {
      has_keycolour = true;
      keycolour_r = palette[keycolor_index].red;
      keycolour_g = palette[keycolor_index].green;
      keycolour_b = palette[keycolor_index].blue;
    }
    convert_pal8 ((uint8 *)NewImage, palette, colors);
  }
  else // grayscale + alpha
  {
    // This is a grayscale image, build a grayscale palette
    csRGBpixel *palette = new csRGBpixel [256];
    int i, entries = (1 << bit_depth) - 1;
    for (i = 0; i <= entries; i++)
      palette [i].red = palette [i].green = palette [i].blue =
        (i * 255) / entries;

    int pixels = Width * Height;
    uint8 *image = new uint8 [pixels];
    Alpha = new uint8 [pixels];
    uint8 *src = (uint8 *)NewImage;
    for (i = 0; i < pixels; i++)
    {
      image [i] = *src++;
      Alpha [i] = *src++;
    }
    delete [] (uint8 *)NewImage;
    convert_pal8 (image, palette);
  }

  // clean up after the read, and free any memory allocated
  png_destroy_read_struct (&png, &info, (png_infopp) NULL);

  // Free the row pointers array that is not needed anymore
  delete [] row_pointers;

  // Check if the alpha channel is valid
  CheckAlpha ();

  return true;
}
