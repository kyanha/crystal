
/*
    Copyright (C) 1998-2004 by Jorrit Tyberghein
	      (C) 2003 by Philip Aumayr
	      (C) 2004 by Frank Richter

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
#include "csutil/array.h"
#include "csutil/databuf.h"
#include "csutil/debug.h"
#include "csutil/util.h"
#include "csgfx/csimgvec.h"
#include "csgfx/imagemanipulate.h"
#include "csgfx/memimage.h"
#include "csgfx/xorpat.h"
#include "cstool/debugimagewriter.h"
#include "iutil/cfgfile.h"
#include "igraphic/image.h"
#include "ivaria/reporter.h"
#include "../common/txtmgr.h"

#include "plugins/video/canvas/openglcommon/glextmanager.h"

#include "gl_txtmgr.h"

CS_LEAKGUARD_IMPLEMENT(csGLTextureHandle);
CS_LEAKGUARD_IMPLEMENT(csGLMaterialHandle);
CS_LEAKGUARD_IMPLEMENT(csGLRendererLightmap);
CS_LEAKGUARD_IMPLEMENT(csGLSuperLightmap);
CS_LEAKGUARD_IMPLEMENT(csGLTextureManager);

//---------------------------------------------------------------------------

class csRLMAlloc : public csBlockAllocator<csGLRendererLightmap>
{
public:
  csRLMAlloc () : csBlockAllocator<csGLRendererLightmap> (512) { }
};

CS_IMPLEMENT_STATIC_VAR (GetRLMAlloc, csRLMAlloc, ());

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csGLTextureHandle)
  SCF_IMPLEMENTS_INTERFACE(iTextureHandle)
SCF_IMPLEMENT_IBASE_END

csGLTextureHandle::csGLTextureHandle (iImage* image, int flags, int target,
				      csGLGraphics3D *iG3D) : uploadData(0)
{
  SCF_CONSTRUCT_IBASE(0);
  this->target = target;
  G3D = iG3D;
  txtmgr = G3D->txtmgr;
  //has_alpha = false;
  Handle = 0;
  textureClass = txtmgr->GetTextureClassID ("default");

  images = csPtr<iImageVector> (new csImageVector());
  images->AddImage(image);

  texFlags.Set (flagsPublicMask, flags);
  transp_color.red = transp_color.green = transp_color.blue = 0;
  if (image->GetFormat () & CS_IMGFMT_ALPHA)
    alphaType = csAlphaMode::alphaSmooth;
  else if (image->HasKeyColor ())
    alphaType = csAlphaMode::alphaBinary;
  else
    alphaType = csAlphaMode::alphaNone;

  //mean_color.red = mean_color.green = mean_color.blue = 0;
  if (image->HasKeyColor ())
  {
    int r,g,b;
    image->GetKeyColor (r,g,b);
    SetKeyColor (r, g, b);
  }
  cachedata = 0;
}

csGLTextureHandle::csGLTextureHandle (iImageVector* image,
				      int flags, int target,
				      csGLGraphics3D *iG3D) : uploadData(0)
{
  SCF_CONSTRUCT_IBASE(0);
  this->target = target;
  G3D = iG3D;
  txtmgr = G3D->txtmgr;
  //has_alpha = false;
  Handle = 0;
  textureClass = txtmgr->GetTextureClassID ("default");

  images = image;

  texFlags.Set (flagsPublicMask, flags);
  transp_color.red = transp_color.green = transp_color.blue = 0;
  if (images->GetImage (0)->GetFormat () & CS_IMGFMT_ALPHA)
    alphaType = csAlphaMode::alphaSmooth;
  else if (images->GetImage (0)->HasKeyColor ())
    alphaType = csAlphaMode::alphaBinary;
  else
    alphaType = csAlphaMode::alphaNone;

  if (images->GetImage (0)->HasKeyColor ())
  {
    int r,g,b;
    images->GetImage (0)->GetKeyColor (r,g,b);
    SetKeyColor (r, g, b);
  }
  cachedata = 0;
}

csGLTextureHandle::csGLTextureHandle (int target, GLuint Handle, 
				      csGLGraphics3D *iG3D) : uploadData(0)
{
  SCF_CONSTRUCT_IBASE(0);
  G3D = iG3D;
  txtmgr = G3D->txtmgr;
  this->target = target;
  csGLTextureHandle::Handle = Handle;
  alphaType = csAlphaMode::alphaNone;
  SetForeignHandle (true);
}

csGLTextureHandle::~csGLTextureHandle()
{
  Clear ();
  txtmgr->UnregisterTexture (this);
  SCF_DESTRUCT_IBASE()
}

void csGLTextureHandle::Clear()
{
  if (uploadData != 0)
  {
    delete uploadData;
    uploadData = 0;
  }
  Unload ();
}

void csGLTextureHandle::FreeImage ()
{
  origNames.SetLength (images->Length ());
  for (size_t i=0; i<images->Length (); i++)
  {
    origNames.Put (i, images->GetImage (i)->GetName ());
  }
  images = 0;
}

int csGLTextureHandle::GetFlags () const
{
  return texFlags.Get() & flagsPublicMask;
}

void csGLTextureHandle::SetKeyColor (bool Enable)
{
  SetTransp (Enable);
  SetTexupdateNeeded (true);
}

void csGLTextureHandle::SetKeyColor (uint8 red, uint8 green, uint8 blue)
{
  transp_color.red = red;
  transp_color.green = green;
  transp_color.blue = blue;
  SetTransp (true);
  SetTexupdateNeeded (true);
}

bool csGLTextureHandle::GetKeyColor () const
{
  return IsTransp();
}

void csGLTextureHandle::GetKeyColor (uint8 &red, uint8 &green, uint8 &blue) const
{
  red = transp_color.red;
  green = transp_color.green;
  blue = transp_color.blue;
}

bool csGLTextureHandle::GetRendererDimensions (int &mw, int &mh)
{
  PrepareInt ();
  mw = actual_width; mh = actual_height;
  return true;
}

void csGLTextureHandle::GetOriginalDimensions (int& mw, int& mh)
{
  PrepareInt ();
  mw = orig_width;
  mh = orig_height;
}

// Check the two below for correctness
bool csGLTextureHandle::GetRendererDimensions (int &mw, int &mh, int &md)
{
  PrepareInt ();
  mw = actual_width;
  mh = actual_height;
  md = actual_d;
  return true;
}

void csGLTextureHandle::GetOriginalDimensions (int& mw, int& mh, int &md)
{
  PrepareInt ();
  mw = orig_width;
  mh = orig_height;
  md = orig_d;
}

void csGLTextureHandle::SetTextureTarget (int target)
{
  this->target = target;
}

const char* csGLTextureHandle::GetImageName (int depth) const
{
  if (images.IsValid () && images->GetImage (depth).IsValid ())
  {
    return images->GetImage (depth)->GetName ();
  }
  else if (origNames.Length () > (size_t)depth)
  {
    return origNames.Get (depth);
  }
  return 0;
}

void csGLTextureHandle::GetMeanColor (uint8 &red, uint8 &green, uint8 &blue)
  const
{
  red = green = blue = 0;
}

void *csGLTextureHandle::GetCacheData ()
{
  return cachedata;
}

void csGLTextureHandle::SetCacheData (void *d)
{
  cachedata = d;
}

void *csGLTextureHandle::GetPrivateObject ()
{
  return (csGLTextureHandle *)this;
}

bool csGLTextureHandle::GetAlphaMap () const
{
  return (alphaType != csAlphaMode::alphaNone);
}

void csGLTextureHandle::PrepareInt ()
{
  //@@@ Images may be lost if preparing twice. Some better way of solving it?
  if (!images) return;
  if (IsPrepared ()) return;
  SetPrepared (true);

  // In opengl all textures, even non-mipmapped textures are required
  // to be powers of 2.
  AdjustSizePo2 ();

  // Set the alpha of keycolored images to 0.
  size_t i;
  for(i = 0; i < images->Length(); i++)
  {
    csAlphaMode::AlphaType newAlphaType = csAlphaMode::alphaNone;
    if (IsTransp())
      PrepareKeycolor (images->GetImage (i), transp_color, newAlphaType);
#if 0
    // Avoid accessing the image data until really needed
    else
      /*
        Check all alpha values for the actual alpha type.
       */
      CheckAlpha  (images->GetImage (i)->GetWidth(), 
	images->GetImage (i)->GetHeight(), 
	(csRGBpixel*)images->GetImage (i)->GetImageData (),
	0, newAlphaType);
#endif

    if (newAlphaType > alphaType) alphaType = newAlphaType;
  }
  CreateMipMaps ();
  FreeImage ();
}

void csGLTextureHandle::AdjustSizePo2 ()
{
  size_t i;
  actual_d = orig_d = images->Length();
  for(i = 0; i < images->Length(); i++)
  {
    orig_width  = images->GetImage (i)->GetWidth();
    orig_height = images->GetImage (i)->GetHeight();

    int newwidth, newheight;

    csTextureHandle::CalculateNextBestPo2Size (
      orig_width, orig_height, newwidth, newheight);

    actual_width = newwidth;
    actual_height = newheight;

    // If necessary rescale if bigger than maximum texture size,
    // but only if a dimension has changed. For textures that are
    // already PO2, a lower mipmap will be selected in CreateMipMaps()
    if ((newwidth != orig_width) && (newwidth > txtmgr->max_tex_size)) 
      newwidth = txtmgr->max_tex_size;
    if ((newheight != orig_width) && (newheight > txtmgr->max_tex_size)) 
      newheight = txtmgr->max_tex_size;

    if (newwidth != orig_width || newheight != orig_height)
    {
      images->SetImage (i, csImageManipulate::Rescale (
	images->GetImage (i), newwidth, newheight));
    }
  }
}

//#define MIPMAP_DEBUG

void csGLTextureHandle::CreateMipMaps()
{
  csRGBpixel *tc = IsTransp() ? &transp_color : (csRGBpixel *)0;

  const csGLTextureClassSettings* textureSettings = 
    txtmgr->GetTextureClassSettings (textureClass);
  /* Determine internal format of the texture. You can't mix glTexImage and 
   * glCompressedTexImage for different mip levels unless the internal format
   * is exactly the same. The target formats of the lower mip levels are later
   * checked against the target format of the first mip.
   */
  bool compressedTarget;
  GLenum targetFormat = (alphaType != csAlphaMode::alphaNone) ? 
    textureSettings->formatRGBA : textureSettings->formatRGB;
  targetFormat = DetermineTargetFormat (targetFormat, 
    !textureSettings->forceDecompress, images->GetImage (0)->GetRawFormat(), 
    compressedTarget);

  // Determine if and how many mipmaps we skip.
  const bool doReduce = !texFlags.Check (CS_TEXTURE_2D | CS_TEXTURE_NOMIPMAPS)
    && textureSettings->allowDownsample;
  int mipskip = doReduce ? txtmgr->texture_downsample : 0;
  while (((actual_width >> mipskip) > txtmgr->max_tex_size)
    || ((actual_height >> mipskip) > txtmgr->max_tex_size))
    mipskip++;

  // Delete existing mipmaps, if any
  size_t i;
  if (uploadData != 0)
    uploadData->DeleteAll();
  else
    uploadData = new csArray<csGLUploadData>;

  csRef<iImageVector> thisImages =
      csPtr<iImageVector> (new csImageVector()); 
  for (i=0; i < images->Length(); i++)
  {
    thisImages->AddImage (images->GetImage (i));
#ifdef MIPMAP_DEBUG
    csDebugImageWriter::DebugImageWrite (images->GetImage (i),
      "/tmp/mipdebug/%.8x_%d_0.png", this, i);
#endif
  }
  if (texFlags.Check (CS_TEXTURE_NOMIPMAPS))
  {
    transform (!textureSettings->forceDecompress, targetFormat, 
      thisImages, 0);
  }
  else
  {
    // Create each new level by creating a level 2 mipmap from previous level
    // we do this down to 1x1 as opengl defines it
    csArray<int> nMipmaps;

    int w, h;
    int nTex = 0;
    int nMip = 0;

    for (i=0; i < images->Length(); i++)
    {
      nMipmaps.Push (images->GetImage (i)->HasMipmaps());
    }
    
    do
    {
      w = thisImages->GetImage (0)->GetWidth ();
      h = thisImages->GetImage (0)->GetHeight ();

      if ((mipskip == 0) || ((w == 1) && (h == 1)))
	transform (!textureSettings->forceDecompress, targetFormat, 
	thisImages, nTex++);

      if ((w == 1) && (h == 1)) break;

      nMip++;
      for (i=0; i<thisImages->Length();i++)
      {
	csRef<iImage> cimg;
	bool precompMip = false;
	if (nMipmaps[i] != 0)
	{
	  cimg = images->GetImage (i)->GetMipmap (nMip);
	  nMipmaps[i]--;
	  precompMip = true;
	}
	else
	{
	  cimg = csImageManipulate::Mipmap (thisImages->GetImage (i), 1, tc);
	}
	if (txtmgr->sharpen_mipmaps 
	  && (mipskip == 0) // don't sharpen when doing skip...
	  && textureSettings->allowMipSharpen
	  && (!precompMip || textureSettings->sharpenPrecomputedMipmaps))
	{
	  cimg = csImageManipulate::Sharpen (cimg, txtmgr->sharpen_mipmaps, 
	    tc);
	}
  #ifdef MIPMAP_DEBUG
	csDebugImageWriter::DebugImageWrite (cimg,
	  "/tmp/mipdebug/%.8x_%d_%d.png", this, i, nMip);
  #endif
	thisImages->SetImage (i, cimg);
      }
      if (mipskip != 0) mipskip--;
    }
    while (true);
  }
}

GLenum csGLTextureHandle::DetermineTargetFormat (GLenum defFormat, 
						 bool allowCompress,
						 const char* rawFormat, 
						 bool& compressedFormat)
{
  GLenum targetFormat = defFormat;
  compressedFormat = false;

  if (rawFormat)
  {
    if (G3D->ext->CS_GL_EXT_texture_compression_s3tc 
      && allowCompress)
    {
      if (strcmp (rawFormat, "dxt1") == 0)
      {
	targetFormat = (alphaType != csAlphaMode::alphaNone) ?
	  GL_COMPRESSED_RGBA_S3TC_DXT1_EXT : GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
	compressedFormat = true;
      }
      else if (strcmp (rawFormat, "dxt3") == 0)
      {
	targetFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
	compressedFormat = true;
      }
      else if (strcmp (rawFormat, "dxt5") == 0)
      {
	targetFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	compressedFormat = true;
      }
    }
  }
  return targetFormat;
}

bool csGLTextureHandle::transform (bool allowCompressed, GLenum targetFormat, 
				   iImageVector *ImageVector, int mipNum)
{
  uint8 *h;

  csRef<iImage> Image = ImageVector->GetImage (0);
  int n = Image->GetWidth ()*Image->GetHeight ();
  int d = ImageVector->Length();
  if (d > 1)
  {
    if (target == CS_TEX_IMG_CUBEMAP)
    {
      for (int j=0; j<d; j++)
      {
	csGLUploadData& uploadData = this->uploadData->GetExtend (
	  this->uploadData->Length());

	csRef<iImage> Image = ImageVector->GetImage (j);
	uploadData.dataRef = Image;
	uploadData.image_data = (uint8*)Image->GetImageData (); // @@@ use pixel packing!
	uploadData.imageNum = j;
	uploadData.w = Image->GetWidth();
	uploadData.h = Image->GetHeight();
	uploadData.d = 1;
	uploadData.targetFormat = targetFormat;
	uploadData.sourceFormat = GL_RGBA;
	uploadData.sourceType = GL_UNSIGNED_BYTE;
	uploadData.mip = mipNum;
      }
    }
    else
    {
      csGLUploadData& uploadData = this->uploadData->GetExtend (
	this->uploadData->Length());
      uploadData.w = Image->GetWidth();
      uploadData.h = Image->GetHeight();
      uploadData.d = d;
      uploadData.targetFormat = targetFormat;
      uploadData.sourceFormat = GL_RGBA;
      uploadData.sourceType = GL_UNSIGNED_BYTE;
      uploadData.mip = mipNum;

      const void* data = 0;
      int j;

      csRef<csDataBuffer> volumeData;
      volumeData.AttachNew (new csDataBuffer (n * d * 4));
      uploadData.dataRef = volumeData;
      uploadData.image_data = h = volumeData->GetUint8();
      for (j=0; j<d; j++)
      {
	Image = ImageVector->GetImage (j);
	if (n == (Image->GetWidth() * Image->GetHeight()))
	{
	  data = Image->GetImageData();
	  memcpy (h, data, n * 4);
	}
	else
	{
	  memset (h, 0, n * 4);
	  /* or well, something else? */
	}
	h += (n * 4);
      }
    }
    //uploadData->size = n * 4;
  }
  else
  {
    csGLUploadData& uploadData = this->uploadData->GetExtend (
      this->uploadData->Length());

    csRef<iImage> Image = ImageVector->GetImage (0);
    const char* rawFormat = Image->GetRawFormat();
    if (rawFormat)
    {
      csRef<iDataBuffer> imageRaw = Image->GetRawData();
      uploadData.dataRef = imageRaw;
      if (strcmp (rawFormat, "r8g8b8") == 0)
      {
	uploadData.image_data = imageRaw->GetUint8();
	uploadData.sourceFormat = GL_RGB;
	uploadData.sourceType = GL_UNSIGNED_BYTE;
      }
      else if (strcmp (rawFormat, "r5g6b5") == 0)
      {
	uploadData.image_data = imageRaw->GetUint8();
	uploadData.sourceFormat = GL_RGB;
	uploadData.sourceType = GL_UNSIGNED_SHORT_5_6_5;
      }
      else if (strcmp (rawFormat, "a8r8g8b8") == 0)
      {
	uploadData.image_data = imageRaw->GetUint8();
	uploadData.sourceFormat = GL_BGRA;
	uploadData.sourceType = GL_UNSIGNED_INT_8_8_8_8_REV;
      }
      else 
      {
	bool isCompressedTarget;
	/* Only use glCompressedTexImage if the target format matches
	 * exactly the one of mip 0. */
	if ((DetermineTargetFormat (targetFormat, allowCompressed,
	  rawFormat, isCompressedTarget) == targetFormat) 
	  && isCompressedTarget)
	{
	  uploadData.image_data = imageRaw->GetUint8();
	  uploadData.compressed = true;
	  uploadData.compressedSize = imageRaw->GetSize();
	}
      }
    }

    if (!uploadData.image_data)
    {
      uploadData.image_data = (uint8*)Image->GetImageData (); // @@@ use pixel packing!
      uploadData.dataRef = Image;
      //uploadData->size = n * 4;
      uploadData.sourceFormat = GL_RGBA;
      uploadData.sourceType = GL_UNSIGNED_BYTE;
    }
    uploadData.targetFormat = targetFormat;
    uploadData.w = Image->GetWidth();
    uploadData.h = Image->GetHeight();
    uploadData.d = 1;
    uploadData.mip = mipNum;
  }
  
  //size += uploadData->size * d;
  return true;
}

void csGLTextureHandle::Blit (int x, int y, int width,
    int height, unsigned char const* data)
{
  // @@@ Keycolor not yet supported here!
  
  // Activate the texture.
  Precache ();
  G3D->ActivateTexture (this);
  // Make sure mipmapping is ok.
  G3D->PrepareAsRenderTarget (this);
  // Do the copy.
  glTexSubImage2D (GL_TEXTURE_2D, 0, x, y, 
      width, height,
      GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void csGLTextureHandle::Load ()
{
  if (Handle != 0) return;

  const csGLTextureClassSettings* textureSettings = 
    txtmgr->GetTextureClassSettings (textureClass);

  static const GLint textureMinFilters[3] = {GL_NEAREST_MIPMAP_NEAREST, 
    GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR};
  static const GLint textureMagFilters[3] = {GL_NEAREST, GL_LINEAR, 
    GL_LINEAR};

  glGenTextures (1, &Handle);

  const int texFilter = texFlags.Check (CS_TEXTURE_NOFILTER) ? 0 : 
    txtmgr->rstate_bilinearmap;
  const GLint magFilter = textureMagFilters[texFilter];
  const GLint minFilter = textureMinFilters[texFilter];
  const GLint wrapMode = 
    (texFlags.Check (CS_TEXTURE_CLAMP)) ? GL_CLAMP_TO_EDGE : GL_REPEAT;

  if (target == CS_TEX_IMG_1D)
  {
    G3D->statecache->SetTexture (GL_TEXTURE_1D, Handle);
    glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, minFilter);

    if (G3D->ext->CS_GL_EXT_texture_filter_anisotropic)
    {
      glTexParameterf (GL_TEXTURE_1D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
        txtmgr->texture_filter_anisotropy);
    }

    // @@@ Implement upload!
  }
  else if (target == CS_TEX_IMG_2D)
  {
    G3D->statecache->SetTexture (GL_TEXTURE_2D, Handle);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
      (texFlags.Check (CS_TEXTURE_NOMIPMAPS)) ? magFilter : minFilter);

    if (G3D->ext->CS_GL_EXT_texture_filter_anisotropic)
    {
      glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
        txtmgr->texture_filter_anisotropy);
    }

    size_t i;
    for (i = 0; i < uploadData->Length(); i++)
    {
      const csGLUploadData& uploadData = this->uploadData->Get (i);
      if (uploadData.compressed)
      {
	G3D->ext->glCompressedTexImage2DARB (GL_TEXTURE_2D, uploadData.mip, 
	  uploadData.targetFormat, uploadData.w, uploadData.h, 
	  0, uploadData.compressedSize, uploadData.image_data);
      }
      else
      {
	glTexImage2D (GL_TEXTURE_2D, uploadData.mip, 
	  uploadData.targetFormat, 
	  uploadData.w, uploadData.h, 0, uploadData.sourceFormat, 
	  uploadData.sourceType, uploadData.image_data);
      }
    }
  }
  else if (target == CS_TEX_IMG_3D)
  {
    G3D->statecache->Enable_GL_TEXTURE_3D ();
    G3D->statecache->SetTexture (GL_TEXTURE_3D, Handle);
    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrapMode);
    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrapMode);

    // @@@ Not sure if the following makes sense with 3D textures.
    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, magFilter);

    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, minFilter);

    if (G3D->ext->CS_GL_EXT_texture_filter_anisotropic)
    {
      glTexParameterf (GL_TEXTURE_3D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
        txtmgr->texture_filter_anisotropy);
    }

    size_t i;
    for (i = 0; i < uploadData->Length(); i++)
    {
      const csGLUploadData& uploadData = this->uploadData->Get (i);
      G3D->ext->glTexImage3DEXT (GL_TEXTURE_3D, uploadData.mip, 
	uploadData.targetFormat, uploadData.w, uploadData.h, uploadData.d,
	0, uploadData.sourceFormat, uploadData.sourceType, 
	uploadData.image_data);
    }
  }
  else if (target == CS_TEX_IMG_CUBEMAP)
  {
    G3D->statecache->SetTexture (GL_TEXTURE_CUBE_MAP, Handle);
    // @@@ Temporarily force clamp, although I don't know if REPEAT
    // makes sense with cubemaps.
    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, 
      GL_CLAMP_TO_EDGE/*wrapMode*/);
    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, 
      GL_CLAMP_TO_EDGE/*wrapMode*/);
    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, 
      GL_CLAMP_TO_EDGE/*wrapMode*/);

    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter);

    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
      (texFlags.Check (CS_TEXTURE_NOMIPMAPS)) ? magFilter : minFilter);

    if (G3D->ext->CS_GL_EXT_texture_filter_anisotropic)
    {
      glTexParameterf (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT,
        txtmgr->texture_filter_anisotropy);
    }

    size_t i;
    for (i = 0; i < uploadData->Length(); i++)
    {
      const csGLUploadData& uploadData = this->uploadData->Get (i);

      glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X + uploadData.imageNum, 
	uploadData.mip, uploadData.targetFormat, 
	uploadData.w, uploadData.h,
        0, uploadData.sourceFormat, uploadData.sourceType,	
	uploadData.image_data);
    }
  }
  delete uploadData; uploadData = 0;
}

void csGLTextureHandle::Unload ()
{
  if ((Handle == 0) || IsForeignHandle()) return;
  if (target == CS_TEX_IMG_1D)
    csGLTextureManager::UnsetTexture (GL_TEXTURE_1D, Handle);
  else if (target == CS_TEX_IMG_2D)
    csGLTextureManager::UnsetTexture (GL_TEXTURE_2D, Handle);
  else if (target == CS_TEX_IMG_3D)
    csGLTextureManager::UnsetTexture (GL_TEXTURE_3D, Handle);
  else if (target == CS_TEX_IMG_CUBEMAP)
    csGLTextureManager::UnsetTexture (GL_TEXTURE_CUBE_MAP, Handle);
  glDeleteTextures (1, &Handle);
  Handle = 0;
}

void csGLTextureHandle::Precache ()
{
  PrepareInt ();
  Load ();
}

void csGLTextureHandle::SetTextureClass (const char* className)
{
  textureClass = txtmgr->GetTextureClassID (className);
}

const char* csGLTextureHandle::GetTextureClass ()
{
  return txtmgr->GetTextureClassName (textureClass);
}

void csGLTextureHandle::UpdateTexture ()
{
  Unload ();
}

GLuint csGLTextureHandle::GetHandle ()
{
  Precache ();
  return Handle;
}

void csGLTextureHandle::ComputeMeanColor (int w, int h, csRGBpixel *src,
					  const csRGBpixel* transp_color,
					  csRGBpixel& mean_color)
{
  int pixels = w * h;
  unsigned r = 0, g = 0, b = 0;
  CS_ASSERT (pixels > 0);
  int count = pixels;
  pixels = 0;
  while (count--)
  {
    const csRGBpixel &pix = *src++;
    if (!transp_color || !transp_color->eq (pix) || pix.alpha)
    {
      r += pix.red;
      g += pix.green;
      b += pix.blue;
      pixels++;
    }
  }
  if (pixels)
  {
    mean_color.red   = r / pixels;
    mean_color.green = g / pixels;
    mean_color.blue  = b / pixels;
  }
  else
    mean_color.red = mean_color.green = mean_color.blue = 0;
}

void csGLTextureHandle::CheckAlpha (int w, int h, csRGBpixel *src, 
				    const csRGBpixel* transp_color, 
				    csAlphaMode::AlphaType& alphaType)
{
  int pixels = w * h;
  CS_ASSERT (pixels > 0);
  int count = pixels;
  pixels = 0;
  while (count--)
  {
    const csRGBpixel &pix = *src++;
    if (!transp_color || !transp_color->eq (pix) || pix.alpha)
    {
      if ((pix.alpha < 255) && (alphaType != csAlphaMode::alphaSmooth))
	alphaType = csAlphaMode::alphaSmooth;
      pixels++;
    }
    else
    {
      if (alphaType == csAlphaMode::alphaNone)
	alphaType = transp_color ? csAlphaMode::alphaBinary : 
          csAlphaMode::alphaSmooth;
    }
  }
}


void csGLTextureHandle::PrepareKeycolor (iImage* image,
					 const csRGBpixel& transp_color,
					 csAlphaMode::AlphaType& alphaType)
{
  int pixels = image->GetWidth () * image->GetHeight ();
  csRGBpixel *_src = (csRGBpixel *)image->GetImageData ();

  while (pixels--)
  {
    /*
      Drop the alpha of the transparent pixels to 0, but leave the alpha
      of non-keycolored ones as is. Keycolor only makes transparent, but
      doesn't mean the rest of the image isn't.
     */
     if (transp_color.eq (*_src)) _src->alpha = 0;
    _src++;
  }

  // Now we draw borders inside all keycolored areas.
  // This removes the halos of keycolor when using bilinear filtering
  int h, rows, w, cols;
  h = rows = image->GetHeight ();
  w = image->GetWidth();
  
  _src = (csRGBpixel *)image->GetImageData ();
  csRGBpixel mean_color;
  CheckAlpha (w, h, _src, &transp_color, alphaType);
  ComputeMeanColor (w, h, _src, &transp_color, mean_color);

  while (rows--)
  {
    cols = w;
    while (cols--)
    {
      if (!_src[(rows*w)+cols].alpha)
      {
	int n=0, r=0, g=0, b=0, xl, xr, yt, yb;
	if (!cols)
	{
	  xl = w-1;
	  xr = 1;
	}
	else if (cols==w-1)
	{
	  xl = cols-1;
	  xr = 0;
	}
	else
	{
	  xl = cols-1;
	  xr = cols+1;
	}

	if (!rows)
	{
	  yt = h-1;
	  yb = 1;
	}
	else if (rows==h-1)
	{
	  yt = rows-1;
	  yb = 0;
	}
	else
	{
	  yt = rows-1;
	  yb = rows+1;
	}

#define CHECK_PIXEL(d) \
	{ \
	  if (_src[(d)].alpha) \
	  { \
	    n++; \
	    r += _src[(d)].red; \
	    g += _src[(d)].green; \
	    b += _src[(d)].blue; \
	  } \
	}

	CHECK_PIXEL((yt*w)+xl);
	CHECK_PIXEL((yt*w)+cols);
	CHECK_PIXEL((yt*w)+xr);
	CHECK_PIXEL((rows*w)+xl);
	CHECK_PIXEL((rows*w)+xr);
	CHECK_PIXEL((yb*w)+xl);
	CHECK_PIXEL((yb*w)+cols);
	CHECK_PIXEL((yb*w)+xr);
#undef CHECK_PIXEL
	if (n)
	{
	  _src[(rows*w)+cols].red = r / n;
	  _src[(rows*w)+cols].green = g / n;
	  _src[(rows*w)+cols].blue = b / n;
	}
	else
	{
	  _src[(rows*w)+cols].red = mean_color.red;
	  _src[(rows*w)+cols].green = mean_color.green;
	  _src[(rows*w)+cols].blue = mean_color.blue;
	}
      }
    }
  }
}

SCF_IMPLEMENT_IBASE (csGLMaterialHandle)
  SCF_IMPLEMENTS_INTERFACE (iMaterialHandle)
SCF_IMPLEMENT_IBASE_END

csGLMaterialHandle::csGLMaterialHandle (iMaterial* m,
	csGLTextureManager *parent)
{
  SCF_CONSTRUCT_IBASE (0);
  material = m;
  texman = parent;
}

csGLMaterialHandle::csGLMaterialHandle (iTextureHandle* t,
	csGLTextureManager *parent)
{
  SCF_CONSTRUCT_IBASE (0);
  texman = parent;
  texture = t;
}

csGLMaterialHandle::~csGLMaterialHandle ()
{
  FreeMaterial ();
  texman->UnregisterMaterial (this);
  SCF_DESTRUCT_IBASE()
}

void csGLMaterialHandle::FreeMaterial ()
{
  material = 0;
}

iShader* csGLMaterialHandle::GetShader (csStringID type)
{ 
  return material ? material->GetShader (type) : 0; 
}

iTextureHandle* csGLMaterialHandle::GetTexture ()
{
  if (material)
  {
    return material->GetTexture (texman->nameDiffuseTexture);
  }
  else
  {
    return texture;
  }
}

void csGLMaterialHandle::GetFlatColor (csRGBpixel &oColor)
{ 
  if (material)
  {
    material->GetFlatColor (oColor);
  }
  else
  {
    texture->GetMeanColor (oColor.red, oColor.green, oColor.blue);
  }
}

void csGLMaterialHandle::GetReflection (float &oDiffuse, float &oAmbient,
  float &oReflection)
{ 
  if (material)
  {
    material->GetReflection (oDiffuse, oAmbient, oReflection);
  }
  else
  {
    oDiffuse = CS_DEFMAT_DIFFUSE;
    oAmbient = CS_DEFMAT_AMBIENT;
    oReflection = CS_DEFMAT_REFLECTION;
  }
}

SCF_IMPLEMENT_IBASE(csGLTextureManager)
  SCF_IMPLEMENTS_INTERFACE(iTextureManager)
SCF_IMPLEMENT_IBASE_END

static const csGLTextureClassSettings defaultSettings = 
  {GL_RGB, GL_RGBA, false, false, true, true};

csGLTextureManager::csGLTextureManager (iObjectRegistry* object_reg,
        iGraphics2D* iG2D, iConfigFile *config,
        csGLGraphics3D *iG3D) : 
  textures (16, 16), materials (16, 16)
{
  SCF_CONSTRUCT_IBASE (0);
  csGLTextureManager::object_reg = object_reg;

  nameDiffuseTexture = iG3D->strings->Request (CS_MATERIAL_TEXTURE_DIFFUSE);

  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

  pfmt = *iG2D->GetPixelFormat ();

  G3D = iG3D;
  max_tex_size = G3D->GetMaxTextureSize ();

  G3D->ext->InitGL_ARB_texture_compression ();
  if (G3D->ext->CS_GL_ARB_texture_compression)
    G3D->ext->InitGL_EXT_texture_compression_s3tc (); 

#define CS_GL_TEXTURE_FORMAT(fmt)					    \
  textureFormats.Put (#fmt, TextureFormat (fmt, true));		
#define CS_GL_TEXTURE_FORMAT_EXT(fmt, Ext)				    \
  textureFormats.Put (#fmt, TextureFormat (fmt, G3D->ext->CS_##Ext));		

  CS_GL_TEXTURE_FORMAT(GL_RGB);
  CS_GL_TEXTURE_FORMAT(GL_R3_G3_B2);
  CS_GL_TEXTURE_FORMAT(GL_RGB4);
  CS_GL_TEXTURE_FORMAT(GL_RGB5);
  CS_GL_TEXTURE_FORMAT(GL_RGB8);
  CS_GL_TEXTURE_FORMAT(GL_RGB10);
  CS_GL_TEXTURE_FORMAT(GL_RGB12);
  CS_GL_TEXTURE_FORMAT(GL_RGB16);
  CS_GL_TEXTURE_FORMAT(GL_RGBA);
  CS_GL_TEXTURE_FORMAT(GL_RGBA2);
  CS_GL_TEXTURE_FORMAT(GL_RGBA4);
  CS_GL_TEXTURE_FORMAT(GL_RGB5_A1);
  CS_GL_TEXTURE_FORMAT(GL_RGBA8);
  CS_GL_TEXTURE_FORMAT(GL_RGB10_A2);
  CS_GL_TEXTURE_FORMAT(GL_RGBA12);
  CS_GL_TEXTURE_FORMAT(GL_RGBA16);
  CS_GL_TEXTURE_FORMAT_EXT(GL_COMPRESSED_RGB_ARB, 
    GL_ARB_texture_compression);
  CS_GL_TEXTURE_FORMAT_EXT(GL_COMPRESSED_RGBA_ARB, 
    GL_ARB_texture_compression);
  CS_GL_TEXTURE_FORMAT_EXT(GL_COMPRESSED_RGB_S3TC_DXT1_EXT, 
    GL_EXT_texture_compression_s3tc);
  CS_GL_TEXTURE_FORMAT_EXT(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 
    GL_EXT_texture_compression_s3tc);
  CS_GL_TEXTURE_FORMAT_EXT(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 
    GL_EXT_texture_compression_s3tc);
  CS_GL_TEXTURE_FORMAT_EXT(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 
    GL_EXT_texture_compression_s3tc);
#undef CS_GL_TEXTURE_FORMAT_SUPP
#undef CS_GL_TEXTURE_FORMAT_EXT
#undef CS_GL_TEXTURE_FORMAT

  textureClasses.Put (textureClassIDs.Request ("default"), defaultSettings);

  read_config (config);
  Clear ();

  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
}

csGLTextureManager::~csGLTextureManager()
{
  SCF_DESTRUCT_IBASE()
}

void csGLTextureManager::read_config (iConfigFile *config)
{
  sharpen_mipmaps = config->GetInt
    ("Video.OpenGL.SharpenMipmaps", 0);
  texture_downsample = config->GetInt
    ("Video.OpenGL.TextureDownsample", 0);
  texture_filter_anisotropy = config->GetFloat
    ("Video.OpenGL.TextureFilterAnisotropy", 1.0);	
  int texture_bits = config->GetInt
    ("Video.OpenGL.TextureBits", 32);
  if (!texture_bits) texture_bits = pfmt.PixelBytes*8;
  
  const char* filterModeStr = config->GetStr (
    "Video.OpenGL.TextureFilter", "trilinear");
  rstate_bilinearmap = 2;
  if (strcmp (filterModeStr, "none") == 0)
    rstate_bilinearmap = 0;
  else if (strcmp (filterModeStr, "nearest") == 0)
    rstate_bilinearmap = 0;
  else if (strcmp (filterModeStr, "bilinear") == 0)
    rstate_bilinearmap = 1;
  else if (strcmp (filterModeStr, "trilinear") == 0)
    rstate_bilinearmap = 2;
  else
  {
    G3D->Report (CS_REPORTER_SEVERITY_WARNING, 
      "Invalid texture filter mode '%s'.", filterModeStr);
  }

  ReadTextureClasses (config);
}

const csGLTextureClassSettings* 
csGLTextureManager::GetTextureClassSettings (csStringID texclass)
{
  const csGLTextureClassSettings* ret = 
    textureClasses.GetElementPointer (texclass);
  return ret ? ret : &defaultSettings;
}

GLenum csGLTextureManager::ParseTextureFormat (const char* formatName, 
					       GLenum defaultFormat)
{
  csString extractedFormat;

  while ((formatName != 0) && (*formatName != 0))
  {
    const char* comma = strchr (formatName, ',');
    if (comma != 0)
    {
      extractedFormat.Replace (formatName, comma - formatName);
      formatName = comma + 1;
    }
    else
    {
      extractedFormat.Replace (formatName);
      formatName = 0;
    }
    const TextureFormat* textureFmt = textureFormats.GetElementPointer (
      extractedFormat.GetData());
    if (textureFmt == 0)
    {
      G3D->Report (CS_REPORTER_SEVERITY_ERROR,
	"Unknown texture format name '%s'", extractedFormat.GetData());
    }
    else
    {
      if (textureFmt->supported)
	return textureFmt->format;
      else
      {
	// @@@ Report if verbose?
      }
    }
  }

  return defaultFormat;
}

void csGLTextureManager::ReadTextureClasses (iConfigFile* config)
{
  csString extractedClass;
  csRef<iConfigIterator> it = config->Enumerate (
    "Video.OpenGL.TextureClass.");
  while (it->Next())
  {
    const char* keyName = it->GetKey (true);
    const char* dot = strchr (keyName, '.');
    if (dot != 0)
    {
      extractedClass.Replace (keyName, dot - keyName);

      csStringID classID = textureClassIDs.Request (extractedClass);
      csGLTextureClassSettings* settings = textureClasses.GetElementPointer (
	classID);
      if (settings == 0)
      {
	textureClasses.Put (classID, defaultSettings);
	settings = textureClasses.GetElementPointer (classID);
      }

      const char* optionName = dot + 1;
      if (strcasecmp (optionName, "FormatRGB") == 0)
      {
	settings->formatRGB = ParseTextureFormat (it->GetStr(),
	  GL_RGB);
      } 
      else if (strcasecmp (optionName, "FormatRGBA") == 0)
      {
	settings->formatRGBA = ParseTextureFormat (it->GetStr(),
	  GL_RGBA);
      } 
      else if (strcasecmp (optionName, "SharpenPrecomputedMipmaps") == 0)
      {
	settings->sharpenPrecomputedMipmaps = it->GetBool ();
      } 
      else if (strcasecmp (optionName, "ForceDecompress") == 0)
      {
	settings->forceDecompress = it->GetBool ();
      } 
      else if (strcasecmp (optionName, "AllowDownsample") == 0)
      {
	settings->allowDownsample = it->GetBool ();
      } 
      else if (strcasecmp (optionName, "AllowMipSharpen") == 0)
      {
	settings->allowMipSharpen = it->GetBool ();
      } 
      else
      {
	G3D->Report (CS_REPORTER_SEVERITY_ERROR,
	  "Unknown texture class option '%s' for '%s'", 
	  optionName, extractedClass.GetData());
      }
    }
    else
    {
      // @@@ Report?
    }
  }
}

void csGLTextureManager::Clear()
{
  size_t i;
  for (i=0; i < textures.Length (); i++)
  {
    csGLTextureHandle* tex = textures[i];
    if (tex != 0) tex->Clear ();
  }
  for (i = 0; i < superLMs.Length(); i++)
  {
    superLMs[i]->DeleteTexture();
  }
}

void csGLTextureManager::UnregisterMaterial (csGLMaterialHandle* handle)
{
  size_t const idx = materials.Find (handle);
  if (idx != csArrayItemNotFound) materials.DeleteIndexFast (idx);
}

void csGLTextureManager::UnsetTexture (GLenum target, GLuint texture)
{
  csGLStateCache* statecache = csGLGraphics3D::statecache;

  if (csGLGraphics3D::ext->CS_GL_ARB_multitexture)
  {
    int oldTU = -1;
    for (int u = 0; u < CS_GL_MAX_LAYER; u++)
    {
      if (statecache->GetTexture (target, u) == texture)
      {
	if (oldTU == -1)
	  oldTU = statecache->GetActiveTU ();
	statecache->SetActiveTU (u);
	statecache->SetTexture (target, 0);
      }
    }
    if (oldTU != -1)
    {
      statecache->SetActiveTU (oldTU);
      statecache->ActivateTU ();
    }
  }
  else
  {
    if (statecache->GetTexture (target) == texture)
      statecache->SetTexture (target, 0);
  }
}

csPtr<iTextureHandle> csGLTextureManager::RegisterTexture (iImage *image,
	int flags)
{
  if (!image)
  {
    G3D->Report(CS_REPORTER_SEVERITY_BUG,
      "BAAAAAAAD!!! csGLTextureManager::RegisterTexture with 0 image!");
    return 0;
  }

  csGLTextureHandle *txt = new csGLTextureHandle (image, flags,
    iTextureHandle::CS_TEX_IMG_2D, G3D);
  textures.Push(txt);
  return csPtr<iTextureHandle> (txt);
}

csPtr<iTextureHandle> csGLTextureManager::RegisterTexture (iImageVector *image,
	int flags, int target)
{
  if (!image)
  {
    G3D->Report(CS_REPORTER_SEVERITY_BUG,
      "BAAAAAAAD!!! csGLTextureManager::RegisterTexture with 0 image array!");
    return 0;
  }

  csGLTextureHandle *txt = new csGLTextureHandle (image, flags,
    target, G3D);
  textures.Push(txt);
  return csPtr<iTextureHandle> (txt);
}

void csGLTextureManager::UnregisterTexture (csGLTextureHandle* handle)
{
  size_t const idx = textures.Find (handle);
  if (idx != csArrayItemNotFound) textures.DeleteIndexFast (idx);
}

csPtr<iMaterialHandle> csGLTextureManager::RegisterMaterial (
	iMaterial* material)
{
  if (!material) return 0;
  csGLMaterialHandle *mat = new csGLMaterialHandle (material, this);
  materials.Push (mat);

  // @@@ Dunno if this should be _here_ really.
  csRef<iShaderManager> shadman = 
    CS_QUERY_REGISTRY (object_reg, iShaderManager);
  //shadman->AddChild (material);
  
  return csPtr<iMaterialHandle> (mat);
}

csPtr<iMaterialHandle> csGLTextureManager::RegisterMaterial (
      iTextureHandle* txthandle)
{
  if (!txthandle) return 0;
  csGLMaterialHandle *mat = new csGLMaterialHandle (txthandle, this);
  materials.Push (mat);

  // @@@ Dunno if this should be _here_ really.
  csRef<iShaderManager> shadman = 
    CS_QUERY_REGISTRY (object_reg, iShaderManager);
  //shadman->AddChild (mat->GetMaterial ());

  return csPtr<iMaterialHandle> (mat);
}

void csGLTextureManager::FreeMaterials ()
{
  size_t i;
  for (i = 0; i < materials.Length (); i++)
  {
    csGLMaterialHandle* mat = materials[i];
    if (mat) mat->FreeMaterial ();
  }
}

int csGLTextureManager::GetTextureFormat ()
{
  return CS_IMGFMT_TRUECOLOR | CS_IMGFMT_ALPHA;
}

csPtr<iSuperLightmap> csGLTextureManager::CreateSuperLightmap(int w, int h)
{
  csGLSuperLightmap* slm = new csGLSuperLightmap (this, w, h);
  superLMs.Push (slm);
  return csPtr<iSuperLightmap> (slm);
}

void csGLTextureManager::GetMaxTextureSize (int& w, int& h, int& aspect)
{
  w = max_tex_size;
  h = max_tex_size;
  aspect = max_tex_size;
}

void csGLTextureManager::DumpSuperLightmaps (iVFS* VFS, iImageIO* iio, 
					     const char* dir)
{
  csString outfn;
  for (size_t i = 0; i < superLMs.Length(); i++)
  {
    csRef<iImage> img = superLMs[i]->Dump ();
    if (img)
    {
      csRef<iDataBuffer> buf = iio->Save (img, "image/png");
      if (!buf)
      {
	G3D->Report (CS_REPORTER_SEVERITY_WARNING,
	  "Could not save super lightmap.");
      }
      else
      {
	outfn.Format ("%s%lu.png", dir, (unsigned long)i);
	if (!VFS->WriteFile (outfn, (char*)buf->GetInt8 (), buf->GetSize ()))
	{
	  G3D->Report (CS_REPORTER_SEVERITY_WARNING,
	    "Could not write to %s.", outfn.GetData ());
	}
	else
	{
	  G3D->Report (CS_REPORTER_SEVERITY_NOTIFY,
	    "Dumped %dx%d SLM to %s", superLMs[i]->w, superLMs[i]->h,
	    	outfn.GetData ());
	}
      }
    }
  }
}
  
void csGLTextureManager::GetLightmapRendererCoords (int slmWidth, int slmHeight,
						    int lm_x1, int lm_y1, 
						    int lm_x2, int lm_y2,
						    float& lm_u1, float& lm_v1, 
						    float &lm_u2, float& lm_v2)
{
  float islmW = 1.0f / (float)slmWidth;
  float islmH = 1.0f / (float)slmHeight;
  // Those offsets seem to result in a look similar to the software
  // renderer... but not perfect yet.
  lm_u1 = ((float)lm_x1 + 0.5f) * islmW;
  lm_v1 = ((float)lm_y1 + 0.5f) * islmH;
  lm_u2 = ((float)lm_x2 - 1.0f) * islmW;
  lm_v2 = ((float)lm_y2 - 1.0f) * islmH;
}

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE_INCREF(csGLRendererLightmap)					
SCF_IMPLEMENT_IBASE_GETREFCOUNT(csGLRendererLightmap)				
SCF_IMPLEMENT_IBASE_REFOWNER(csGLRendererLightmap)				
SCF_IMPLEMENT_IBASE_REMOVE_REF_OWNERS(csGLRendererLightmap)
SCF_IMPLEMENT_IBASE_QUERY(csGLRendererLightmap)
  SCF_IMPLEMENTS_INTERFACE(iRendererLightmap)
SCF_IMPLEMENT_IBASE_END

void csGLRendererLightmap::DecRef ()
{
  if (scfRefCount == 1)							
  {									
    CS_ASSERT (slm != 0);
    slm->FreeRLM (this);
    return;								
  }									
  scfRefCount--;							
}

csGLRendererLightmap::csGLRendererLightmap ()
{
  SCF_CONSTRUCT_IBASE (0);
}

csGLRendererLightmap::~csGLRendererLightmap ()
{
#ifdef CS_DEBUG
  if (slm->texHandle != (GLuint)~0)
  {
    csRGBpixel* pat = new csRGBpixel[rect.Width () * rect.Height ()];
    int x, y;
    csRGBpixel* p = pat;
    for (y = 0; y < rect.Height (); y++)
    {
      for (x = 0; x < rect.Width (); x++)
      {
	p->red = ((x ^ y) & 1) * 0xff;
	p++;
      }
    }

    csGLGraphics3D::statecache->SetTexture (
      GL_TEXTURE_2D, slm->texHandle);

    glTexSubImage2D (GL_TEXTURE_2D, 0, rect.xmin, rect.ymin, 
      rect.Width (), rect.Height (),
      GL_RGBA, GL_UNSIGNED_BYTE, pat);

    delete[] pat;
  }
#endif
  SCF_DESTRUCT_IBASE()
}

void csGLRendererLightmap::GetSLMCoords (int& left, int& top, 
    int& width, int& height)
{
  left = rect.xmin; top  = rect.ymin;
  width = rect.Width (); height = rect.Height ();
}
    
void csGLRendererLightmap::SetData (csRGBpixel* data)
{
  slm->CreateTexture ();

  csGLGraphics3D::statecache->SetTexture (
    GL_TEXTURE_2D, slm->texHandle);

  glTexSubImage2D (GL_TEXTURE_2D, 0, rect.xmin, rect.ymin, 
    rect.Width (), rect.Height (),
    GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void csGLRendererLightmap::SetLightCellSize (int size)
{
  (void)size;
}

//---------------------------------------------------------------------------


SCF_IMPLEMENT_IBASE_INCREF(csGLSuperLightmap)					
SCF_IMPLEMENT_IBASE_GETREFCOUNT(csGLSuperLightmap)				
SCF_IMPLEMENT_IBASE_REFOWNER(csGLSuperLightmap)				
SCF_IMPLEMENT_IBASE_REMOVE_REF_OWNERS(csGLSuperLightmap)
SCF_IMPLEMENT_IBASE_QUERY(csGLSuperLightmap)
  SCF_IMPLEMENTS_INTERFACE(iSuperLightmap)
SCF_IMPLEMENT_IBASE_END

void csGLSuperLightmap::DecRef ()
{
  if (scfRefCount == 1)							
  {
    if (txtmgr != 0)
      txtmgr->superLMs.Delete (this);
    delete this;
    return;								
  }									
  scfRefCount--;							
}

csGLSuperLightmap::csGLSuperLightmap (csGLTextureManager* txtmgr, 
				      int width, int height)
{
  SCF_CONSTRUCT_IBASE (0);
  w = width; h = height;
  texHandle = (GLuint)~0;
  numRLMs = 0;
  csGLSuperLightmap::txtmgr = txtmgr;
}

csGLSuperLightmap::~csGLSuperLightmap ()
{
  DeleteTexture ();
  SCF_DESTRUCT_IBASE()
}

void csGLSuperLightmap::CreateTexture ()
{
  if (texHandle == (GLuint)~0)
  {
    glGenTextures (1, &texHandle);

    csGLGraphics3D::statecache->SetTexture (
      GL_TEXTURE_2D, texHandle);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    csRGBpixel* data = new csRGBpixel [w * h];
#ifdef CS_DEBUG
    // Fill the background for debugging purposes (to quickly see what's
    // a lightmap and what's not; esp. useful when LMs are rather dark -
    // would be hardly visible if at all on black)
    // And to have it not that boring, add a neat backdrop.
    static const uint16 debugBG[16] =
      {0x0000, 0x3222, 0x4a36, 0x422a, 0x3222, 0x0a22, 0x4a22, 0x33a2, 
       0x0000, 0x2232, 0x364a, 0x2a42, 0x2232, 0x220a, 0x224a, 0xa233};

    csRGBpixel* p = data;
    int y, x;
    for (y = 0; y < h; y++)
    {
      for (x = 0; x < w; x++)
      {
	const int bitNum = 6;
	int b = (~x) & 0xf;
	int px = (debugBG[y & 0xf] & (1 << b));
	if (b < bitNum)
	{
	  px <<= bitNum - b;
	}
	else
	{
	  px >>= b - bitNum;
	}
	p->blue = ~(1 << bitNum) + px;
	p->alpha = ~((px >> 6) * 0xff);
	p++;
      }
    }
#endif
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, 
      GL_RGBA, GL_UNSIGNED_BYTE, data);
    delete[] data;
  }
}

void csGLSuperLightmap::DeleteTexture ()
{
  if (texHandle != (GLuint)~0)
  {
    /*csGLGraphics3D::statecache->SetTexture (
      GL_TEXTURE_2D, 0);*/
    csGLTextureManager::UnsetTexture (GL_TEXTURE_2D, texHandle);

    glDeleteTextures (1, &texHandle);
    texHandle = (GLuint)~0;
    th = 0;
  }
}

void csGLSuperLightmap::FreeRLM (csGLRendererLightmap* rlm)
{
  if (--numRLMs == 0)
  {
    DeleteTexture ();
  }

  // IncRef() ourselves manually.
  // Otherwise freeing the RLM could trigger our own destruction -
  // causing an assertion in block allocator (due to how BA frees items and
  // the safety assertions on BA destruction.)
  scfRefCount++;
  GetRLMAlloc ()->Free (rlm);
  DecRef ();
}

csPtr<iRendererLightmap> csGLSuperLightmap::RegisterLightmap (int left, int top,
	int width, int height)
{
  csGLRendererLightmap* rlm = GetRLMAlloc ()->Alloc ();
  rlm->slm = this;
  rlm->rect.Set (left, top, left + width, top + height);

  numRLMs++;

  return csPtr<iRendererLightmap> (rlm);
}

csPtr<iImage> csGLSuperLightmap::Dump ()
{
  // @@@ hmm... or just return an empty image?
  if (texHandle == (GLuint)~0) return 0;

  GLint tw, th;
  csGLGraphics3D::statecache->SetTexture (GL_TEXTURE_2D, texHandle);

  glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tw);
  glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &th);

  uint8* data = new uint8[tw * th * 4];
  glGetTexImage (GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  csImageMemory* lmimg = 
    new csImageMemory (tw, th,
    data, true, 
    CS_IMGFMT_TRUECOLOR | CS_IMGFMT_ALPHA);

  return csPtr<iImage> (lmimg);
}

iTextureHandle* csGLSuperLightmap::GetTexture ()
{
  if (th == 0)
  {
    CreateTexture ();
    th.AttachNew (new csGLTextureHandle (iTextureHandle::CS_TEX_IMG_2D, 
      texHandle, txtmgr->G3D));
  }
  return th;
}
