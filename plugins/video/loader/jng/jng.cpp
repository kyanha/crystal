/*
    JNG image file format support for CrystalSpace 3D library
    Copyright (C) 2002 by Jorrit Tyberghein
    Copyright (C) 2002 by Frank Richter <resqu@gmx.ch>

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

// note: the order is not quite CS standard... However, if changed you'll
// get a couple of compile errors, mostly due to conflicts in some #defines 
// and __declspec()s
#include "cssysdef.h"
#include "csutil/scf.h"
#include "cssys/sysfunc.h"
#include "csgfx/rgbpixel.h"
#include "csutil/databuf.h"
#include "ivaria/reporter.h"

extern "C"
{
#define jpeg_boolean boolean
/*#if defined (OS_WIN32)
#if !defined (COMP_GCC) // Avoid defining "boolean" in libjpeg headers
#  define HAVE_BOOLEAN	// we need int booleans, not Windows unsigned char bools
#  define boolean int
#  undef jpeg_boolean 
#  define jpeg_boolean int
#endif
#endif*/
#define JDCT_DEFAULT JDCT_FLOAT	// use floating-point for decompression
#include <jpeglib.h>
#include <jerror.h>
#if defined (OS_WIN32)
#undef boolean
#endif
}

#if !defined(__CYGWIN__)
#define MNG_DLL
#endif
#define MNG_NO_CMS
#define MNG_SUPPORT_FULL
#define MNG_ACCESS_CHUNKS
#define MNG_WRITE_CHUNKS
#define MNG_SUPPORT_READ
#define MNG_SUPPORT_WRITE
#define MNG_SUPPORT_DISPLAY
#include <libmng.h>

#include "jng.h"
#include <zlib.h>

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE (csJNGImageIO)
  SCF_IMPLEMENTS_INTERFACE (iImageIO)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csJNGImageIO::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csJNGImageIO);

SCF_EXPORT_CLASS_TABLE (csjngimg)
  SCF_EXPORT_CLASS (csJNGImageIO, "crystalspace.graphic.image.io.jng",
		"CrystalSpace JNG image format I/O plugin")
SCF_EXPORT_CLASS_TABLE_END

#define JNG_MIME "image/x-jng" 
#define MNG_MIME "image/x-mng"

static iImageIO::FileFormatDescription formatlist[5] =
{
  {JNG_MIME, "RGBA", CS_IMAGEIO_LOAD|CS_IMAGEIO_SAVE},
  {MNG_MIME, "RGBA", CS_IMAGEIO_LOAD}
};

/// report something
void Report (iObjectRegistry *object_reg, int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csRef<iReporter> rep = CS_QUERY_REGISTRY (object_reg, iReporter);
  if (rep)
    rep->ReportV (severity, "crystalspace.graphic.image.io.jng", 
      msg, arg);
  else
  {
    csPrintf ("crystalspace.graphic.image.io.jng: ");
    csPrintfV (msg, arg);
    csPrintf ("\n");
  }
  va_end (arg);
}

/// report libmng error information 
void ReportLibmngError (iObjectRegistry *object_reg, mng_handle hMNG, char* msg)
{
  mng_int8     severity;
  mng_chunkid  chunkname;
  mng_uint32   chunkseq;
  mng_int32    extra1;
  mng_int32    extra2;
  mng_pchar    errortext;

  mng_getlasterror (hMNG, &severity, &chunkname, &chunkseq, &extra1, 
    &extra2, &errortext);

  Report (object_reg, CS_REPORTER_SEVERITY_WARNING,
    "%s: %s (severity %d, chunkname %.8x, chunkseq %d, %.8x, %.8x)",
    msg, errortext, severity, chunkname, chunkseq, extra1, extra2);
}

csJNGImageIO::csJNGImageIO (iBase *pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  formats.Push (&formatlist[0]);
  formats.Push (&formatlist[1]);
}

const csVector& csJNGImageIO::GetDescription ()
{
  return formats;
}

csPtr<iImage> csJNGImageIO::Load (uint8* iBuffer, uint32 iSize, int iFormat)
{
  ImageJngFile* i = new ImageJngFile (iFormat, object_reg);
  if (i && !i->Load (iBuffer, iSize))
  {
    delete i;
    return NULL;
  }
  return csPtr<iImage> (i);
}

void csJNGImageIO::SetDithering (bool)
{
}

// a couple of callbacks used by both reading & writing

/// memory alloc callback for libmng
mng_ptr MNG_DECL cb_alloc (mng_size_t iLen) 
{
  mng_ptr res = (mng_ptr)new uint8[iLen];
  if (res)                 
    memset (res, 0, iLen);
  return res;
}

/// memory free callback for libmng
void MNG_DECL cb_free (mng_ptr iPtr, mng_size_t iLen)
{
  delete[] (uint8*)iPtr;
  (void)iLen;     // Kill compiler warning
}

/// stream open callback for libmng
mng_bool MNG_DECL cb_openstream (mng_handle hMNG)
{
  (void)hMNG;
  return MNG_TRUE;                     
}

/// stream close callback for libmng
mng_bool MNG_DECL cb_closestream (mng_handle hMNG)
{
  (void)hMNG;
  return MNG_TRUE;                     
}

mng_bool csJNGImageIO::cb_writedata (mng_handle hHandle, mng_ptr pBuf,
                                     mng_uint32 iBuflen, mng_uint32p pWritten)
{
  csJNGImageIO *this_;

  this_ = (csJNGImageIO *)mng_get_userdata (hHandle);

  *pWritten = this_->outfile->Write ((char*)pBuf, iBuflen);

  return MNG_TRUE;
}

mng_ptr csJNGImageIO::cb_getcanvasline(mng_handle hHandle, mng_uint32 iLinenr)
{
  csJNGImageIO *this_;

  this_ = (csJNGImageIO *)mng_get_userdata (hHandle);

  return (csRGBpixel*)this_->imgRGBA->GetImageData()+(iLinenr*this_->imgRGBA->GetWidth());
}

/*
 * Write a JNG file.
 * If you want to understand what's going on here check the JNG format spec 
 * (http://www.libpng.org/pub/mng/spec/jng.html) and the PNG format spec 
 * (http://www.libpng.org/pub/png/spec/)
 */

// JPEG stuff

/* ==== Error mgmnt ==== */

struct my_error_mgr
{
  struct jpeg_error_mgr pub;	/* "public" fields */
  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr *my_error_ptr;

static void my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

/* ==== Destination mgmnt ==== */

struct jpg_datastore{
  unsigned char *data;
  size_t len;

  jpg_datastore () { data = NULL; len = 0; }
  ~jpg_datastore () { free (data); }
};


struct my_dst_mgr
{
  static size_t buf_len;
  struct jpeg_destination_mgr pub;	/* public fields */
  JOCTET *buffer;		/* start of buffer */

  jpg_datastore *ds;
};

size_t my_dst_mgr::buf_len = 4096;

static void init_destination (j_compress_ptr cinfo)
{
  my_dst_mgr *dest = (my_dst_mgr*)cinfo->dest;
  dest->buffer = (JOCTET*)(*cinfo->mem->alloc_small)((j_common_ptr)cinfo,
    JPOOL_IMAGE, sizeof(JOCTET) * my_dst_mgr::buf_len);
  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = my_dst_mgr::buf_len;
}

static jpeg_boolean empty_output_buffer (j_compress_ptr cinfo)
{
  my_dst_mgr *dest = (my_dst_mgr*)cinfo->dest;

  dest->ds->data = (unsigned char*)realloc (dest->ds->data,
    dest->ds->len + sizeof(JOCTET) * my_dst_mgr::buf_len);
  if (!dest->ds->data)
    ERREXITS(cinfo,JERR_OUT_OF_MEMORY, "Could not reallocate enough memory");
  memcpy (dest->ds->data + dest->ds->len, dest->buffer,
    sizeof(JOCTET) * my_dst_mgr::buf_len);
  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = my_dst_mgr::buf_len;
  dest->ds->len += sizeof(JOCTET) * my_dst_mgr::buf_len;
  return true;
}

static void term_destination (j_compress_ptr cinfo)
{
  my_dst_mgr *dest = (my_dst_mgr*)cinfo->dest;
  size_t len = my_dst_mgr::buf_len - dest->pub.free_in_buffer;

  if (len > 0)
  {
    dest->ds->data = (unsigned char*)realloc (dest->ds->data,
      dest->ds->len + sizeof(JOCTET) * len);
    if (!dest->ds->data)
      ERREXITS(cinfo,JERR_OUT_OF_MEMORY, "Could not reallocate enough memory");
    memcpy(dest->ds->data + dest->ds->len, dest->buffer, sizeof(JOCTET) * len);
    dest->ds->len += sizeof(JOCTET) * len;
  }
}

static void jpeg_buffer_dest (j_compress_ptr cinfo, jpg_datastore *ds)
{
  my_dst_mgr *dest;

  if (cinfo->dest == NULL)
    cinfo->dest = (struct jpeg_destination_mgr*)(*cinfo->mem->alloc_small)
      ((j_common_ptr)cinfo, JPOOL_PERMANENT, sizeof(my_dst_mgr));

  dest = (my_dst_mgr*)cinfo->dest;
  dest->pub.init_destination = init_destination;
  dest->pub.empty_output_buffer = empty_output_buffer;
  dest->pub.term_destination = term_destination;
  dest->ds = ds;
}

csPtr<iDataBuffer> csJNGImageIO::Save (iImage *Image,
	iImageIO::FileFormatDescription *, const char* extraoptions)
{
  // we need to get a RGB/RGBA version of the image.
  switch (Image->GetFormat() & CS_IMGFMT_MASK)
  {
    case CS_IMGFMT_PALETTED8:
      imgRGBA = Image->Clone ();
      imgRGBA->SetFormat (CS_IMGFMT_TRUECOLOR | (Image->GetFormat() & CS_IMGFMT_ALPHA));
      break;
    case CS_IMGFMT_TRUECOLOR:
      imgRGBA = csRef<iImage>(Image);
      break;
    default:
      // unknown format
      return NULL;
  } 

  // compression options
  int quality = 80;
  bool progressive = false;
  bool alpha_jpeg = false;
  int alpha_png_compress = 6;
  int alpha_jpeg_quality = -1;

  /*
     parse output options.
     options are a comma-separated list and can be either
     'option' or 'option=value'.

     supported options:
       compress=#	  image color compression, 0..100 higher values give smaller files
			  but uglier results.
       progressive	  progressive encoding.
       jng_lossy_alpha	  use lossy JPEG compression for alpha channel (instead of
			  default lossles PNG)
       jng_alpha_compress alpha channel compression, 0..100
			  Impact of higher value depends on alpha channel type.
			  JPEG - smaller files, uglier results.
			  PNG - smaller files, longer time to encode.
			  Note: defaults to value for image color compression if lossy alpha
			  is selected.
	    

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
      {
	quality = 100 - atoi (opt_value);
        if (alpha_jpeg_quality == -1) alpha_jpeg_quality = quality;
      }
    }
    else if (!strcmp (opt_key, "progressive"))
    {
      progressive = true;
    }
    else if (!strcmp (opt_key, "jng_lossy_alpha"))
    {
      alpha_jpeg = true;
      if (alpha_jpeg_quality == -1) alpha_jpeg_quality = quality;
    }
    else if (!strcmp (opt_key, "jng_alpha_compress"))
    {
      if (opt_value)
      {
	alpha_jpeg_quality = 100 - atoi (opt_value);
	alpha_png_compress = atoi (opt_value) / 10;
      }
    }

    delete opt_key;
  }

  if (quality < 0) quality = 0;
  if (quality > 100) quality = 100;
  if (alpha_jpeg_quality < 0) alpha_jpeg_quality = 0;
  if (alpha_jpeg_quality > 100) alpha_jpeg_quality = 100;
  if (alpha_png_compress < 0) alpha_png_compress = 0;
  if (alpha_png_compress > 9) alpha_png_compress = 9;

  mng_handle handle = mng_initialize ( mng_ptr(this), cb_alloc, 
                                      cb_free, MNG_NULL);
  if (!handle)
  {
    Report (object_reg, CS_REPORTER_SEVERITY_WARNING,
      "failed to initialize libmng");
    return NULL;
  }

  if ((mng_setcb_openstream (handle, cb_openstream) != MNG_NOERROR) ||
      (mng_setcb_closestream (handle, cb_closestream) != MNG_NOERROR) ||
      (mng_setcb_writedata (handle, cb_writedata) != MNG_NOERROR))
  {
    ReportLibmngError (object_reg, handle, "failed to set libmng callbacks");
    mng_cleanup (&handle);
    return NULL;
  }

  outfile = new csMemFile ();

  if (mng_create (handle) != MNG_NOERROR)
  {
    ReportLibmngError (object_reg, handle, "failed to create new jng");
    mng_cleanup (&handle);
    delete outfile;
    imgRGBA = 0;
    return NULL;
  }

  bool has_alpha = imgRGBA->GetFormat() & CS_IMGFMT_ALPHA;

  if (mng_putchunk_jhdr (handle, imgRGBA->GetWidth(), imgRGBA->GetHeight(),
			 has_alpha ? MNG_COLORTYPE_JPEGCOLORA 
				   : MNG_COLORTYPE_JPEGCOLOR,
			 MNG_BITDEPTH_JPEG8, MNG_COMPRESSION_BASELINEJPEG,
			 progressive ? MNG_INTERLACE_PROGRESSIVE
				     : MNG_INTERLACE_SEQUENTIAL,
				     has_alpha?8:0, has_alpha?(alpha_jpeg?8:0):0, 
			 0, 0) != MNG_NOERROR)
  {
    ReportLibmngError (object_reg, handle, "failed to put JHDR chunk");
    mng_cleanup (&handle);
    delete outfile;
    imgRGBA = 0;
    return NULL;
  }

  // @@@ chunk data generation.
  // lots of stuff needs to be done manually.
  // should be changed as libmng evolves.

  // write out alpha channel
  if (has_alpha)
  {
    // extract the alpha channel from the image
    int pixels = imgRGBA->GetWidth() * imgRGBA->GetHeight();
    uint8 *alpha = new uint8 [pixels];
    uint8 *alphaptr = alpha;
    csRGBpixel *imgdata = (csRGBpixel*)imgRGBA->GetImageData();

    while (pixels>0)
    {
      *alphaptr++ = (imgdata++)->alpha;
      pixels--;
    }

    if (alpha_jpeg)
    {
      // compress the alpha data as JPEG and write it out.
      uint8* volatile row = NULL;
      struct jpg_datastore ds;
      struct jpeg_compress_struct cinfo;
      struct my_error_mgr jerr;
      cinfo.err = jpeg_std_error (&jerr.pub);
      jerr.pub.error_exit = my_error_exit;

      if (setjmp (jerr.setjmp_buffer))
      {
	Report (object_reg, CS_REPORTER_SEVERITY_WARNING,
		"failed to JPEG compress alpha data");
	mng_cleanup (&handle);
	delete outfile;
	delete [] row;
	delete[] alpha;
	jpeg_destroy_compress (&cinfo);
	imgRGBA = 0;
	return NULL;
      }

      jpeg_create_compress (&cinfo);
      jpeg_buffer_dest (&cinfo, &ds);

      cinfo.image_width = imgRGBA->GetWidth ();
      cinfo.image_height = imgRGBA->GetHeight ();
      cinfo.input_components = 1;
      cinfo.in_color_space = JCS_GRAYSCALE;

      row = new uint8[cinfo.image_width];
      jpeg_set_defaults (&cinfo);
      jpeg_set_quality (&cinfo, alpha_jpeg_quality, true);
      jpeg_start_compress (&cinfo, true);

      JSAMPROW row_pointer[1];
      uint8 *image = alpha;
      row_pointer[0] = (JSAMPLE*)&row[0];

      while (cinfo.next_scanline < cinfo.image_height)
      {
	for (size_t i=0; i < cinfo.image_width; i++)
	  row[i] = image[cinfo.next_scanline * cinfo.image_width + i];
	jpeg_write_scanlines (&cinfo, row_pointer, 1);
      }

      jpeg_finish_compress (&cinfo);
      jpeg_destroy_compress (&cinfo);

      delete [] row;

      // funny, mng_putchunk_jdaa is missing from libmng
      //if (mng_putchunk_jdaa (handle, ds.len, ds.data) != MNG_NOERROR)
      if (mng_putchunk_unknown (handle, MNG_UINT_JDAA, ds.len, ds.data) != MNG_NOERROR)
      {
	ReportLibmngError (object_reg, handle, "failed to put JDAA chunk");
	mng_cleanup (&handle);
	delete outfile;
	delete[] alpha;
	imgRGBA = 0;
	return NULL;
      }
    }
    else
    {
      // generate the IDAT chunk data
      // we use the "Up" filter. 
      uint8* chunkdata = new uint8[(imgRGBA->GetWidth() + 1) * imgRGBA->GetHeight()];
      uint8* lastline = new uint8[imgRGBA->GetWidth()];
      uint8* chunkptr = chunkdata;
      alphaptr = alpha;

      memset (lastline, 0, imgRGBA->GetWidth());
      int lines = imgRGBA->GetHeight();
      while (lines > 0) 
      {
	*chunkptr++ = 2;
	int pix = 0;
	while (pix<imgRGBA->GetWidth())
	{
	  *chunkptr++ = *alphaptr - lastline[pix];
	  lastline[pix] = *alphaptr++;
	  pix++;
	}
	lines--;
      }
      delete[] lastline;

      // now compress the data
      z_stream zs;

      zs.zalloc = (alloc_func) NULL;
      zs.zfree = (free_func) NULL;
      zs.next_in = (Byte *) chunkdata;
      zs.avail_in = (imgRGBA->GetWidth() + 1) * imgRGBA->GetHeight();

      if (deflateInit (&zs, alpha_png_compress) != Z_OK)
      {
	Report (object_reg, CS_REPORTER_SEVERITY_WARNING,
	  "deflateInit() failed");
	mng_cleanup (&handle);
	delete outfile;
	delete[] chunkdata;
	delete[] alpha;
	imgRGBA = 0;
	return NULL;
      }

      char buff[0x8000];

      while (1)
      {
        zs.next_out = (Byte *)buff;
        zs.avail_out = sizeof (buff);

        int rc = deflate (&zs, Z_FINISH);   /* Do actual compression */
        size_t size = sizeof (buff) - zs.avail_out;

	// create a chuk w/compressed data.
	if (mng_putchunk_idat (handle, size, &buff) != MNG_NOERROR)
	{
	  ReportLibmngError (object_reg, handle, "failed to put IDAT chunk");
          deflateEnd (&zs);
	  mng_cleanup (&handle);
	  delete outfile;
	  delete[] chunkdata;
	  delete[] alpha;
	  imgRGBA = 0;
	  return NULL;
	}
        if (rc == Z_STREAM_END)
          break;            /* finished */
      } 
      deflateEnd (&zs);

      delete[] chunkdata;
    }

    delete[] alpha;
  }

  // compress the color data as JPEG and write it out.
  csRGBcolor* volatile row = NULL;
  struct jpg_datastore ds;
  struct jpeg_compress_struct cinfo;
  struct my_error_mgr jerr;
  cinfo.err = jpeg_std_error (&jerr.pub);
  jerr.pub.error_exit = my_error_exit;

  if (setjmp (jerr.setjmp_buffer))
  {
    Report (object_reg, CS_REPORTER_SEVERITY_WARNING,
	    "failed to JPEG compress color data");
    mng_cleanup (&handle);
    delete outfile;
    delete [] row;
    jpeg_destroy_compress (&cinfo);
    imgRGBA = 0;
    return NULL;
  }

  jpeg_create_compress (&cinfo);
  jpeg_buffer_dest (&cinfo, &ds);

  cinfo.image_width = imgRGBA->GetWidth ();
  cinfo.image_height = imgRGBA->GetHeight ();
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;

  row = new csRGBcolor[cinfo.image_width];
  jpeg_set_defaults (&cinfo);
  jpeg_set_quality (&cinfo, quality, true);
  if (progressive) jpeg_simple_progression (&cinfo);
  jpeg_start_compress (&cinfo, true);

  JSAMPROW row_pointer[1];
  csRGBpixel *image = (csRGBpixel*)imgRGBA->GetImageData ();
  row_pointer[0] = (JSAMPLE*)&row[0];

  while (cinfo.next_scanline < cinfo.image_height)
  {
    for (size_t i=0; i < cinfo.image_width; i++)
      row[i] = image[cinfo.next_scanline * cinfo.image_width + i];
    jpeg_write_scanlines (&cinfo, row_pointer, 1);
  }

  jpeg_finish_compress (&cinfo);
  jpeg_destroy_compress (&cinfo);

  delete [] row;

  if (mng_putchunk_jdat (handle, ds.len, ds.data) != MNG_NOERROR)
  {
    ReportLibmngError (object_reg, handle, "failed to put JDAT chunk");
    mng_cleanup (&handle);
    delete outfile;
    imgRGBA = 0;
    return NULL;
  }
  
  imgRGBA = 0;

  if (mng_putchunk_iend (handle) != MNG_NOERROR)
  {
    ReportLibmngError (object_reg, handle, "failed to put IEND chunk");
    mng_cleanup (&handle);
    delete outfile;
    return NULL;
  }

  if (mng_write (handle) != MNG_NOERROR)
  {
    ReportLibmngError (object_reg, handle, "failed to write out JNG data");
    mng_cleanup (&handle);
    delete outfile;
    return NULL;
  }

  mng_cleanup (&handle);

  csRef<iDataBuffer> db (outfile->GetAllData ());

  delete outfile;

  return csPtr<iDataBuffer> (db);
}

csPtr<iDataBuffer> csJNGImageIO::Save (iImage *Image, const char *mime,
  const char* extraoptions)
{
  if (!strcasecmp (mime, JNG_MIME))
    return Save (Image, (iImageIO::FileFormatDescription *)NULL,
      extraoptions);
  return NULL;
}

//---------------------------------------------------------------------------

/*
 * A small wor don how everything works:
 * Basically, libmng is made to read & display animated stuff and
 * requires to have some timing stuff set up, thus the getticks and
 * settimer callbacks, although they seem to be a bit weird when loading
 * still images. 
 *
 * We just want one frame. To achieve this, mng_display() is called once. 
 * With the images we are most interested in here (JNGs) this is probably 
 * successful and libmng fills our buffer. However this does _not_ work as 
 * well with MNGs. It actually depends on the file; with some the first frame 
 * is displayed right, with others just partially. Download the MNGsuite
 * (http://www.libmng.com/MNGsuite) and see for yourself.
 */

mng_bool ImageJngFile::cb_readdata (mng_handle hHandle, mng_ptr pBuf,
					      mng_uint32 iBuflen, mng_uint32 *pRead)
{
  ImageJngFile *this_;

  this_ = (ImageJngFile *)mng_get_userdata (hHandle);

  // determine amount of data to read; 0 if EOF
  *pRead = MIN (iBuflen, this_->bufferSize - (this_->bufptr - this_->buffer));
  if (*pRead > 0)
  {
    memcpy (pBuf, this_->bufptr, *pRead);
    this_->bufptr += *pRead;
  }
  else
    *pRead = 0;

  return MNG_TRUE;
}

mng_bool ImageJngFile::cb_processheader (mng_handle hHandle, 
					  mng_uint32 iWidth, mng_uint32 iHeight)
{
  ImageJngFile *this_;
  
  this_ = (ImageJngFile *)mng_get_userdata (hHandle);

  if (mng_set_canvasstyle (hHandle, MNG_CANVAS_RGBA8))
  {
    ReportLibmngError (this_->object_reg, hHandle, "failed to set canvas style");
    return MNG_FALSE;
  }
   
  this_->Width = iWidth;
  this_->Height = iHeight;
  this_->NewImage = new csRGBpixel [iWidth * iHeight];

  return MNG_TRUE;
}

mng_ptr ImageJngFile::cb_getcanvasline (mng_handle hHandle, mng_uint32 iLineNr)
{
  ImageJngFile *this_;

  this_ = (ImageJngFile *)mng_get_userdata (hHandle);

  return this_->NewImage + (iLineNr * this_->Width);
}

mng_bool ImageJngFile::cb_imagerefresh (mng_handle hHandle,
					 mng_uint32 iX, mng_uint32 iY, 
					 mng_uint32 iWidth, mng_uint32 iHeight)
{
  return MNG_TRUE;
}

mng_uint32 ImageJngFile::cb_gettickcount (mng_handle hHandle)
{
  return csGetTicks();      
}

mng_bool ImageJngFile::cb_settimer (mng_handle hHandle, mng_uint32 iMsecs)
{
  return MNG_TRUE;
}

bool ImageJngFile::Load (uint8 *iBuffer, uint32 iSize)
{
  NewImage = NULL;
  mng_retcode retcode;

  const int magicSize = 8;
  const char magicMNG[] = "\x8aJNG\x0d\x0a\x1a\x0a";
  const char magicJNG[] = "\x8bJNG\x0d\x0a\x1a\x0a";

  // check for magic JNG/MNG bytes. If not correct, we can skip
  // messing around w/ libmng entirely.
  if ((iSize < 8) || 
    ((memcmp ((void*)iBuffer, (void*)&magicMNG, magicSize)) &&
     (memcmp ((void*)iBuffer, (void*)&magicJNG, magicSize))))
  {
    return false;
  }

  mng_handle handle = mng_initialize (mng_ptr(this), cb_alloc, 
                                      cb_free, MNG_NULL);
  if (!handle)
  {
    Report (object_reg, CS_REPORTER_SEVERITY_WARNING,
      "failed to initialize libmng");
    return false;
  }

  buffer = iBuffer;
  bufptr = buffer;
  bufferSize = iSize;

  if ((mng_setcb_openstream (handle, cb_openstream) != MNG_NOERROR) ||
      (mng_setcb_closestream (handle, cb_closestream) != MNG_NOERROR) ||
      (mng_setcb_readdata (handle, cb_readdata) != MNG_NOERROR) ||
      (mng_setcb_processheader(handle, cb_processheader) != MNG_NOERROR) ||
      (mng_setcb_getcanvasline(handle, cb_getcanvasline) != MNG_NOERROR) ||
      (mng_setcb_refresh(handle, cb_imagerefresh) != MNG_NOERROR) ||
      (mng_setcb_gettickcount(handle, cb_gettickcount) != MNG_NOERROR) ||
      (mng_setcb_settimer(handle, cb_settimer) != MNG_NOERROR))
  {
    ReportLibmngError (object_reg, handle, "failed to set libmng callbacks");
    mng_cleanup (&handle);
    return false;
  }

  retcode = mng_read (handle);
  if (retcode != MNG_NOERROR)
  {
    if (retcode != MNG_INVALIDSIG) // maybe its just not an jng/mng...
      ReportLibmngError (object_reg, handle, "failed to read data");
    mng_cleanup (&handle);
    return false;
  }

  // Don't read PNGs 
  if (mng_get_sigtype (handle) == mng_it_png)
  {
    if (NewImage) delete[] NewImage;
    mng_cleanup (&handle);
    return false;
  }

  mng_display (handle);

  if (NewImage)
    convert_rgba (NewImage);

  mng_cleanup (&handle);

  return NewImage;
}
