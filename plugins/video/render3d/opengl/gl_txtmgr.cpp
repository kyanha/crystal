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
#include "csgfx/bakekeycolor.h"
#include "csgfx/imagecubemapmaker.h"
#include "csgfx/imagemanipulate.h"
#include "csgfx/imagememory.h"
#include "csgfx/packrgb.h"
#include "csgfx/xorpat.h"
#include "cstool/debugimagewriter.h"
#include "iutil/cfgfile.h"
#include "igraphic/image.h"
#include "ivaria/reporter.h"
#include "csplugincommon/render3d/txtmgr.h"

#include "csplugincommon/opengl/glextmanager.h"

#include "gl_render3d.h"
#include "gl_txtmgr.h"

CS_LEAKGUARD_IMPLEMENT(csGLBasicTextureHandle);
CS_LEAKGUARD_IMPLEMENT(csGLRendererLightmap);
CS_LEAKGUARD_IMPLEMENT(csGLSuperLightmap);
CS_LEAKGUARD_IMPLEMENT(csGLTextureManager);

//---------------------------------------------------------------------------

class csRLMAlloc : public csBlockAllocator<csGLRendererLightmap>
{
public:
  csRLMAlloc () : csBlockAllocator<csGLRendererLightmap> (512) { }
};

CS_IMPLEMENT_STATIC_VAR (GetRLMAlloc, csRLMAlloc, ())

//---------------------------------------------------------------------------

csGLBasicTextureHandle::csGLBasicTextureHandle (int flags, 
				      csGLGraphics3D *iG3D) : 
  scfImplementationType (this), uploadData(0), 
  texFormat((TextureBlitDataFormat)-1)
{
  G3D = iG3D;
  txtmgr = G3D->txtmgr;
  Handle = 0;
  textureClass = txtmgr->GetTextureClassID ("default");

  texFlags.Set (flagsPublicMask, flags);

  transp_color.red = transp_color.green = transp_color.blue = 0;
}

csGLBasicTextureHandle::~csGLBasicTextureHandle()
{
  Clear ();
  txtmgr->UnregisterTexture (this);
}

void csGLBasicTextureHandle::Clear()
{
  if (uploadData != 0)
  {
    delete uploadData;
    uploadData = 0;
  }
  Unload ();
}

int csGLBasicTextureHandle::GetFlags () const
{
  return texFlags.Get() & flagsPublicMask;
}

void csGLBasicTextureHandle::SetKeyColor (bool Enable)
{
  SetTransp (Enable);
  SetTexupdateNeeded (true);
  if (Enable && alphaType == csAlphaMode::alphaNone)
    alphaType = csAlphaMode::alphaBinary;
  else if (!Enable && alphaType == csAlphaMode::alphaBinary)
    alphaType = csAlphaMode::alphaNone;
}

void csGLBasicTextureHandle::SetKeyColor (uint8 red, uint8 green, uint8 blue)
{
  transp_color.red = red;
  transp_color.green = green;
  transp_color.blue = blue;
  if (alphaType == csAlphaMode::alphaNone)
    alphaType = csAlphaMode::alphaBinary;
  texFlags.Set (flagTransp | flagTranspSet | flagTexupdateNeeded);
}

bool csGLBasicTextureHandle::GetKeyColor () const
{
  return IsTransp();
}

void csGLBasicTextureHandle::GetKeyColor (uint8 &red,
	uint8 &green, uint8 &blue) const
{
  red = transp_color.red;
  green = transp_color.green;
  blue = transp_color.blue;
}

bool csGLBasicTextureHandle::GetRendererDimensions (int &mw, int &mh)
{
  AdjustSizePo2 ();
  mw = actual_width; mh = actual_height;
  return true;
}

// Check the two below for correctness
bool csGLBasicTextureHandle::GetRendererDimensions (int &mw, int &mh, int &md)
{
  AdjustSizePo2 ();
  mw = actual_width;
  mh = actual_height;
  md = actual_d;
  return true;
}

void *csGLBasicTextureHandle::GetPrivateObject ()
{
  return (csGLBasicTextureHandle *)this;
}

bool csGLBasicTextureHandle::GetAlphaMap () 
{
  return (alphaType != csAlphaMode::alphaNone);
}

static void ComputeNewPo2ImageSize (int texFlags,
				    int orig_width, int orig_height,
				    int orig_depth,
				    int& newwidth, int& newheight,
				    int& newdepth,
				    int max_tex_size)
{
  csTextureHandle::CalculateNextBestPo2Size (texFlags, orig_width, newwidth);
  csTextureHandle::CalculateNextBestPo2Size (texFlags, orig_height, newheight);
  csTextureHandle::CalculateNextBestPo2Size (texFlags, orig_depth, newdepth);

  // If necessary rescale if bigger than maximum texture size,
  // but only if a dimension has changed. For textures that are
  // already PO2, a lower mipmap will be selected in CreateMipMaps()
  if ((newwidth != orig_width) && (newwidth > max_tex_size)) 
    newwidth = max_tex_size;
  if ((newheight != orig_width) && (newheight > max_tex_size)) 
    newheight = max_tex_size;
  if ((newdepth != orig_depth) && (newdepth > max_tex_size)) 
    newdepth = max_tex_size;
}

csRef<iImage> csGLBasicTextureHandle::PrepareIntImage (
    int actual_width, int actual_height, int actual_depth, iImage* srcimage,
    csAlphaMode::AlphaType newAlphaType)
{
  csRef<iImage> newImage;
  if (actual_width != srcimage->GetWidth () || actual_height != srcimage->GetHeight () 
      || actual_depth != srcimage->GetDepth ())
  {
    newImage = csImageManipulate::Rescale (srcimage, actual_width, 
	actual_height, actual_depth);
  }
  if (IsTransp())
  {
    if (!newImage.IsValid()) 
      newImage.AttachNew (new csImageMemory (srcimage));
    // Set the alpha of keycolored images to 0.
    PrepareKeycolor (newImage, transp_color, newAlphaType);
  }
#if 0
  // Avoid accessing the image data until really needed
  else
    /* Check all alpha values for the actual alpha type.  */
    CheckAlpha  (image->GetWidth(), image->GetHeight(), 
	(csRGBpixel*)image->GetImageData (), 0, newAlphaType);
#endif
  if (newImage.IsValid()) return newImage;
  return 0;
}

void csGLBasicTextureHandle::PrepareInt ()
{
}

GLenum csGLBasicTextureHandle::DetermineTargetFormat (GLenum defFormat, 
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
      if (strcmp (rawFormat, "*dxt1") == 0)
      {
	targetFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
	compressedFormat = true;
      }
      else if (strcmp (rawFormat, "*dxt1a") == 0)
      {
	targetFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
	compressedFormat = true;
      }
      else if (strcmp (rawFormat, "*dxt3") == 0)
      {
	targetFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
	compressedFormat = true;
      }
      else if (strcmp (rawFormat, "*dxt5") == 0)
      {
	targetFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	compressedFormat = true;
      }
    }
  }
  return targetFormat;
}

CS::StructuredTextureFormat csGLBasicTextureHandle::fmt_r8g8b8_i
	= CS::TextureFormatStrings::ConvertStructured ("r8g8b8_i");
CS::StructuredTextureFormat csGLBasicTextureHandle::fmt_b8g8r8_i
	= CS::TextureFormatStrings::ConvertStructured ("b8g8r8_i");
CS::StructuredTextureFormat csGLBasicTextureHandle::fmt_r5g6b5_i
	= CS::TextureFormatStrings::ConvertStructured ("r5g6b5_i");
CS::StructuredTextureFormat csGLBasicTextureHandle::fmt_a8r8g8b8_i
	= CS::TextureFormatStrings::ConvertStructured ("a8r8g8b8_i");
CS::StructuredTextureFormat csGLBasicTextureHandle::fmt_l8_i
	= CS::TextureFormatStrings::ConvertStructured ("l8_i");
CS::StructuredTextureFormat csGLBasicTextureHandle::fmt_dxt1
	= CS::TextureFormatStrings::ConvertStructured ("*dxt1");
CS::StructuredTextureFormat csGLBasicTextureHandle::fmt_dxt1a
	= CS::TextureFormatStrings::ConvertStructured ("*dxt1a");
CS::StructuredTextureFormat csGLBasicTextureHandle::fmt_dxt3
	= CS::TextureFormatStrings::ConvertStructured ("*dxt3");
CS::StructuredTextureFormat csGLBasicTextureHandle::fmt_dxt5
	= CS::TextureFormatStrings::ConvertStructured ("*dxt5");

bool csGLBasicTextureHandle::ConvertFormat2GL (const char* format,
	csGLSource& src, GLenum& targetFormat, bool allowCompressed,
	bool& compressed)
{
  CS::StructuredTextureFormat fmt = CS::TextureFormatStrings
  	::ConvertStructured (format);
  if (fmt == fmt_b8g8r8_i)
  {
    src.format = GL_RGB;
    src.type = GL_UNSIGNED_BYTE;
    return true;
  }
  if (fmt == fmt_l8_i)
  {
    src.format = GL_LUMINANCE;
    src.type = GL_UNSIGNED_BYTE;
    targetFormat = GL_LUMINANCE;
    return true;
  }
  if (G3D->ext->CS_GL_version_1_2)
  {
    if (fmt == fmt_r8g8b8_i)
    {
      src.format = GL_BGR;
      src.type = GL_UNSIGNED_BYTE;
      return true;
    }
    else if (fmt == fmt_r5g6b5_i)
    {
      src.format = GL_RGB;
      src.type = GL_UNSIGNED_SHORT_5_6_5;
      return true;
    }
    else if (fmt == fmt_a8r8g8b8_i)
    {
      src.format = GL_BGRA;
      src.type = GL_UNSIGNED_BYTE;
      return true;
    }
  }
  if (allowCompressed && G3D->ext->CS_GL_EXT_texture_compression_s3tc)
  {
   if (fmt == fmt_dxt1)
    {
      targetFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
      compressed = true;
      return true;
    }
    else if (fmt == fmt_dxt1a)
    {
      targetFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
      compressed = true;
      return true;
    }
    else if (fmt == fmt_dxt3)
    {
      targetFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
      compressed = true;
      return true;
    }
    else if (fmt == fmt_dxt3)
    {
      targetFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
      compressed = true;
      return true;
    }
  }
  return false;
}

bool csGLBasicTextureHandle::transform (bool allowCompressed, GLenum targetFormat, 
				   iImage* Image, int mipNum, int imageNum)
{
  csGLUploadData& uploadData = this->uploadData->GetExtend (
    this->uploadData->Length());
  const char* rawFormat = Image->GetRawFormat();
  if (rawFormat)
  {
    csRef<iDataBuffer> imageRaw = Image->GetRawData();
    uploadData.dataRef = imageRaw;
    if (ConvertFormat2GL (rawFormat, uploadData.source, targetFormat,
	allowCompressed, uploadData.isCompressed))
    {
      uploadData.image_data = imageRaw->GetUint8();
      if (uploadData.isCompressed)
	uploadData.compressed.size = imageRaw->GetSize();
    }
  }

  if (!uploadData.image_data)
  {
    if (csPackRGBA::IsRGBpixelSane())
    {
      uploadData.image_data = (uint8*)Image->GetImageData ();
      uploadData.dataRef = Image;
    }
    else
    {
      const size_t numPix = 
	Image->GetWidth() * Image->GetHeight() * Image->GetDepth();
      csRef<csDataBuffer> newDataBuf;
      newDataBuf.AttachNew (new csDataBuffer (numPix));
      csPackRGBA::PackRGBpixelToRGBA (newDataBuf->GetUint8(),
	(csRGBpixel*)Image->GetImageData(), numPix);
      uploadData.image_data = newDataBuf->GetUint8();
      uploadData.dataRef = newDataBuf;
    }
    //uploadData->size = n * 4;
    uploadData.source.format = GL_RGBA;
    uploadData.source.type = GL_UNSIGNED_BYTE;
  }
  uploadData.targetFormat = targetFormat;
  uploadData.w = Image->GetWidth();
  uploadData.h = Image->GetHeight();
  uploadData.d = Image->GetDepth();
  uploadData.mip = mipNum;
  uploadData.imageNum = imageNum;
  
  //size += uploadData->size * d;
  return true;
}

void csGLBasicTextureHandle::Blit (int x, int y, int width,
    int height, unsigned char const* data, TextureBlitDataFormat format)
{
  // @@@ Keycolor not yet supported here!
  
  GLenum textarget = GetGLTextureTarget();
  if ((textarget != GL_TEXTURE_2D) && (textarget != GL_TEXTURE_RECTANGLE_ARB))
    return;

  // Activate the texture.
  Precache ();
  G3D->ActivateTexture (this);
  GLuint textureFormat = (format == RGBA8888) ? GL_RGBA : GL_BGRA;
  // Make sure mipmapping is ok.
  if (!IsWasRenderTarget() || (texFormat != format))
  {
    texFormat = format;
    bool isWholeImage = (x == 0) && (y == 0) && (width == actual_width)
      && (height == actual_height);

    // Pull texture data and set as RGBA/BGRA again, to prevent compression 
    // (slooow) on subsequent glTexSubImage() calls.
    if (!isWholeImage)
    {
      uint8* pixels = new uint8[actual_width * actual_height * 4];
      glGetTexImage (textarget, 0, textureFormat, GL_UNSIGNED_BYTE, 
	pixels);

      if (!IsWasRenderTarget())
      {
	SetWasRenderTarget (true);
	SetupAutoMipping();
      }

      glTexImage2D (textarget, 0, textureFormat, actual_width, 
	actual_height, 0, textureFormat, GL_UNSIGNED_BYTE, pixels);
      delete[] pixels;
    }
    else
    {
      if (!IsWasRenderTarget())
      {
	SetWasRenderTarget (true);
	SetupAutoMipping();
      }

      glTexImage2D (textarget, 0, textureFormat, actual_width, 
	actual_height, 0, textureFormat, GL_UNSIGNED_BYTE, data);
      return;
    }
  }
  // Do the copy.
  glTexSubImage2D (textarget, 0, x, y, 
      width, height,
      textureFormat, GL_UNSIGNED_BYTE, data);
  //SetNeedMips (true);
}

void csGLBasicTextureHandle::SetupAutoMipping()
{
  // Set up mipmap generation
  if ((!(texFlags.Get() & CS_TEXTURE_NOMIPMAPS))
    /*&& (!G3D->ext->CS_GL_EXT_framebuffer_object)*/)
  {
    if (G3D->ext->CS_GL_SGIS_generate_mipmap)
      glTexParameteri (GetGLTextureTarget(), GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    else
      glTexParameteri  (GetGLTextureTarget(), GL_TEXTURE_MIN_FILTER,
	txtmgr->rstate_bilinearmap ? GL_LINEAR : GL_NEAREST);
  }
}

void csGLBasicTextureHandle::Load ()
{
  if (Handle != 0) return;

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
      if (uploadData.isCompressed)
      {
	G3D->ext->glCompressedTexImage2DARB (GL_TEXTURE_2D, uploadData.mip, 
	  uploadData.targetFormat, uploadData.w, uploadData.h, 
	  0, (GLsizei)uploadData.compressed.size, uploadData.image_data);
      }
      else
      {
	glTexImage2D (GL_TEXTURE_2D, uploadData.mip, 
	  uploadData.targetFormat, 
	  uploadData.w, uploadData.h, 0, uploadData.source.format, 
	  uploadData.source.type, uploadData.image_data);
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
    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER,
      (texFlags.Check (CS_TEXTURE_NOMIPMAPS)) ? magFilter : minFilter);

    if (G3D->ext->CS_GL_EXT_texture_filter_anisotropic)
    {
      glTexParameterf (GL_TEXTURE_3D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
        txtmgr->texture_filter_anisotropy);
    }

    size_t i;
    for (i = 0; i < uploadData->Length(); i++)
    {
      const csGLUploadData& uploadData = this->uploadData->Get (i);
      if (uploadData.isCompressed)
      {
	G3D->ext->glCompressedTexImage3DARB (GL_TEXTURE_3D, uploadData.mip, 
	  uploadData.targetFormat, uploadData.w, uploadData.h, 
	  uploadData.d, 0, (GLsizei)uploadData.compressed.size, 
	  uploadData.image_data);
      }
      else
      {
	G3D->ext->glTexImage3DEXT (GL_TEXTURE_3D, uploadData.mip, 
	  uploadData.targetFormat, uploadData.w, uploadData.h, uploadData.d,
	  0, uploadData.source.format, uploadData.source.type, 
	  uploadData.image_data);
      }
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

      if (uploadData.isCompressed)
      {
	G3D->ext->glCompressedTexImage2DARB (
	  GL_TEXTURE_CUBE_MAP_POSITIVE_X + uploadData.imageNum, 
	  uploadData.mip, 
	  uploadData.targetFormat, uploadData.w, uploadData.h, 
	  0, (GLsizei)uploadData.compressed.size, uploadData.image_data);
      }
      else
      {
	glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X + uploadData.imageNum, 
	  uploadData.mip, uploadData.targetFormat, 
	  uploadData.w, uploadData.h,
	  0, uploadData.source.format, uploadData.source.type,	
	  uploadData.image_data);
      }
    }
  }
  else if (target == CS_TEX_IMG_RECT)
  {
    G3D->statecache->SetTexture (GL_TEXTURE_RECTANGLE_ARB, Handle);
    glTexParameteri (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, wrapMode);

    glTexParameteri (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER,
      (texFlags.Check (CS_TEXTURE_NOMIPMAPS)) ? magFilter : minFilter);

    if (G3D->ext->CS_GL_EXT_texture_filter_anisotropic)
    {
      glTexParameterf (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAX_ANISOTROPY_EXT,
        txtmgr->texture_filter_anisotropy);
    }

    size_t i;
    for (i = 0; i < uploadData->Length(); i++)
    {
      const csGLUploadData& uploadData = this->uploadData->Get (i);
      if (uploadData.isCompressed)
      {
	G3D->ext->glCompressedTexImage2DARB (GL_TEXTURE_RECTANGLE_ARB, 
          uploadData.mip, uploadData.targetFormat, uploadData.w, uploadData.h, 
	  0, (GLsizei)uploadData.compressed.size, uploadData.image_data);
      }
      else
      {
	glTexImage2D (GL_TEXTURE_RECTANGLE_ARB, uploadData.mip, 
	  uploadData.targetFormat, 
	  uploadData.w, uploadData.h, 0, uploadData.source.format, 
	  uploadData.source.type, uploadData.image_data);
      }
    }
  }
  delete uploadData; uploadData = 0;
}

void csGLBasicTextureHandle::Unload ()
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
  else if (target == CS_TEX_IMG_RECT)
    csGLTextureManager::UnsetTexture (GL_TEXTURE_RECTANGLE_ARB, Handle);
  glDeleteTextures (1, &Handle);
  Handle = 0;
}

void csGLBasicTextureHandle::Precache ()
{
  PrepareInt ();
  Load ();
}

void csGLBasicTextureHandle::SetTextureClass (const char* className)
{
  textureClass = txtmgr->GetTextureClassID (className ? className : "default");
}

const char* csGLBasicTextureHandle::GetTextureClass ()
{
  return txtmgr->GetTextureClassName (textureClass);
}

void csGLBasicTextureHandle::UpdateTexture ()
{
  Unload ();
}

GLuint csGLBasicTextureHandle::GetHandle ()
{
  Precache ();
  if ((!(texFlags.Get() & CS_TEXTURE_NOMIPMAPS))
    && (G3D->ext->CS_GL_EXT_framebuffer_object)
    && IsNeedMips())
  {
    G3D->statecache->SetTexture (GL_TEXTURE_2D, Handle);
    G3D->ext->glGenerateMipmapEXT (GL_TEXTURE_2D);
    SetNeedMips (false);
  }
  return Handle;
}

GLenum csGLBasicTextureHandle::GetGLTextureTarget() const
{
  switch (target)
  {
    case CS_TEX_IMG_1D:
      return GL_TEXTURE_1D;
    case CS_TEX_IMG_2D:
      return GL_TEXTURE_2D;
    case CS_TEX_IMG_3D:
      return GL_TEXTURE_3D;
    case CS_TEX_IMG_CUBEMAP:
      return GL_TEXTURE_CUBE_MAP;
    case CS_TEX_IMG_RECT:
      return GL_TEXTURE_RECTANGLE_ARB;
    default:
      return 0;
  }
}

void csGLBasicTextureHandle::CheckAlpha (int w, int h, int d, csRGBpixel *src, 
				    const csRGBpixel* transp_color, 
				    csAlphaMode::AlphaType& alphaType)
{
  int count = w * h * d;
  CS_ASSERT (count > 0);
  while (count--)
  {
    const csRGBpixel &pix = *src++;
    if ((!transp_color || !transp_color->eq (pix)) && pix.alpha)
    {
      if ((pix.alpha < 255) && (alphaType != csAlphaMode::alphaSmooth))
	alphaType = csAlphaMode::alphaSmooth;
    }
    else
    {
      if (alphaType == csAlphaMode::alphaNone)
	alphaType = transp_color ? csAlphaMode::alphaBinary : 
          csAlphaMode::alphaSmooth;
    }
  }
}


void csGLBasicTextureHandle::PrepareKeycolor (csRef<iImage>& image,
					 const csRGBpixel& transp_color,
					 csAlphaMode::AlphaType& alphaType)
{
  int w = image->GetWidth();
  int h = image->GetHeight ();
  int d = image->GetDepth ();
  csRGBpixel *_src = (csRGBpixel *)image->GetImageData ();
  CheckAlpha (w, h, d, _src, &transp_color, alphaType);
  if (alphaType == csAlphaMode::alphaNone) return; // Nothing to fix up
  image = csBakeKeyColor::Image (image, transp_color);
}

//---------------------------------------------------------------------------

csGLTextureHandle::csGLTextureHandle (iImage* image, int flags, 
				      csGLGraphics3D *iG3D) : 
  csGLBasicTextureHandle (flags, iG3D),
  origName(0)
{
  this->image = image;
  switch (image->GetImageType())
  {
    case csimgCube:
      target = CS_TEX_IMG_CUBEMAP;
      break;
    case csimg3D:
      target = CS_TEX_IMG_3D;
      break;
    default:
      target = CS_TEX_IMG_2D;
      break;
  }
  const uint npotsNeededFlags = (CS_TEXTURE_NOMIPMAPS | CS_TEXTURE_CLAMP);
  if (flags & CS_TEXTURE_NPOTS)
  {
    // For NPOTS we need...
    bool npotsValid = (
      // The extension
      (G3D->ext->CS_GL_ARB_texture_rectangle
      || G3D->ext->CS_GL_EXT_texture_rectangle
      || G3D->ext->CS_GL_NV_texture_rectangle
      || txtmgr->enableNonPowerOfTwo2DTextures
      || G3D->ext->CS_GL_ARB_texture_non_power_of_two)
      // Certain additional texture flags, unless we have ARB_tnpot
      && (((flags & npotsNeededFlags) == npotsNeededFlags) 
        || G3D->ext->CS_GL_ARB_texture_non_power_of_two))
      // A 2D image, unless we have ARB_tnpot
      && ((image->GetImageType() == csimg2D)
        || G3D->ext->CS_GL_ARB_texture_non_power_of_two);
    if (!npotsValid)
    {
      flags &= ~CS_TEXTURE_NPOTS;
    }
    else if (!txtmgr->enableNonPowerOfTwo2DTextures
      && !G3D->ext->CS_GL_ARB_texture_non_power_of_two)
      /* Note that 'enableNonPowerOfTwo2DTextures' is the flag for ATI's
       * support of non-POT _2D_ textures; that is, the textures, being
       * NPOTS, need to go to the 2D target, not RECT. 
       * Same when ARB_tnpot is available. */
      target = CS_TEX_IMG_RECT;
  }
  texFlags.Set (flagsPublicMask, flags);

  if (image->GetFormat () & CS_IMGFMT_ALPHA)
    alphaType = csAlphaMode::alphaSmooth;
  else if (image->HasKeyColor ())
    alphaType = csAlphaMode::alphaBinary;
  else
    alphaType = csAlphaMode::alphaNone;

  if (image->HasKeyColor())
    SetTransp (true);
}

csGLTextureHandle::csGLTextureHandle (int target, GLuint Handle, 
				      csGLGraphics3D *iG3D) : 
  csGLBasicTextureHandle (0, iG3D),
  origName(0)
{
  this->target = target;
  csGLBasicTextureHandle::Handle = Handle;
  alphaType = csAlphaMode::alphaNone;
  SetForeignHandle (true);
}

csGLTextureHandle::~csGLTextureHandle()
{
  delete[] origName;
}


void csGLTextureHandle::FreeImage ()
{
  if (image.IsValid()) 
  {
    origName = csStrNew (image->GetName());
    if (IsTransp() && !IsTranspSet())
    {
      int r,g,b;
      image->GetKeyColor (r,g,b);
      SetKeyColor (r, g, b);
    }						 
  }
  image = 0;
}

void csGLTextureHandle::GetOriginalDimensions (int& mw, int& mh)
{
  AdjustSizePo2 ();
  mw = orig_width;
  mh = orig_height;
}

void csGLTextureHandle::GetOriginalDimensions (int& mw, int& mh, int &md)
{
  AdjustSizePo2 ();
  mw = orig_width;
  mh = orig_height;
  md = orig_d;
}

const char* csGLTextureHandle::GetImageName () const
{
  if (image.IsValid()) 
    return image->GetName();
  else
    return origName;
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
  GLenum targetFormat; 
  if ((target == iTextureHandle::CS_TEX_IMG_RECT)
    && (txtmgr->disableRECTTextureCompression))
    /* @@@ Hack: Some ATI drivers can't grok generic compressed formats for 
     * RECT textures, so force an uncompressed format in this case. */
    targetFormat = (alphaType != csAlphaMode::alphaNone) ? 
      GL_RGBA : GL_RGB;
  else
    targetFormat = (alphaType != csAlphaMode::alphaNone) ? 
      textureSettings->formatRGBA : textureSettings->formatRGB;
  targetFormat = DetermineTargetFormat (targetFormat, 
    !textureSettings->forceDecompress, image->GetRawFormat(), 
    compressedTarget);

  // Determine if and how many mipmaps we skip.
  const bool doReduce = !texFlags.Check (CS_TEXTURE_2D | CS_TEXTURE_NOMIPMAPS)
    && textureSettings->allowDownsample;
  int mipskip = doReduce ? txtmgr->texture_downsample : 0;
  while (((actual_width >> mipskip) > txtmgr->max_tex_size)
      || ((actual_height >> mipskip) > txtmgr->max_tex_size)
      || ((actual_d >> mipskip) > txtmgr->max_tex_size))
    mipskip++;

  // Delete existing mipmaps, if any
  size_t i;
  if (uploadData != 0)
    uploadData->DeleteAll();
  else
    uploadData = new csArray<csGLUploadData>;

  size_t subImageCount = image->HasSubImages() + 1;
#ifdef MIPMAP_DEBUG
  for (i=0; i < subImageCount; i++)
  {
    csDebugImageWriter::DebugImageWrite (image->GetSubImage (i),
      "/tmp/mipdebug/%p_%zu_0.png", this, i);
  }
#endif
  if (texFlags.Check (CS_TEXTURE_NOMIPMAPS))
  {
    for (i=0; i < subImageCount; i++)
    {
      transform (!textureSettings->forceDecompress, targetFormat, 
	image->GetSubImage ((uint)i), 0, (int)i);
    }
  }
  else
  {
    for (i=0; i < subImageCount; i++)
    {
      // Create each new level by creating a level 2 mipmap from previous level
      // we do this down to 1x1 as opengl defines it
      int w, h;
      int nTex = 0;
      int nMip = 0;
      csRef<iImage> thisImage = image->GetSubImage ((uint)i); 
      int nMipmaps = thisImage->HasMipmaps();

      do
      {
	w = thisImage->GetWidth ();
	h = thisImage->GetHeight ();

	if ((mipskip == 0) || ((w == 1) && (h == 1)))
	  transform (!textureSettings->forceDecompress, targetFormat, 
	  thisImage, nTex++, (int)i);

	if ((w == 1) && (h == 1)) break;

	nMip++;
	csRef<iImage> cimg;
	bool precompMip = false;
	if (nMipmaps != 0)
	{
	  cimg = image->GetSubImage ((uint)i)->GetMipmap (nMip);
	  nMipmaps--;
	  precompMip = true;
	}
	else
	{
	  cimg = csImageManipulate::Mipmap (thisImage, 1, tc);
	}
	if (txtmgr->sharpen_mipmaps 
	  && (mipskip == 0) // don't sharpen when doing skip...
	  && textureSettings->allowMipSharpen
	  && (cimg->GetDepth() == 1) // @@@ sharpen not "depth-safe"
	  && (!precompMip || textureSettings->sharpenPrecomputedMipmaps))
	{
	  cimg = csImageManipulate::Sharpen (cimg, txtmgr->sharpen_mipmaps, 
	    tc);
	}
  #ifdef MIPMAP_DEBUG
	csDebugImageWriter::DebugImageWrite (cimg,
	  "/tmp/mipdebug/%p_%zu_%d.png", this, i, nMip);
  #endif
	thisImage = cimg;
	if (mipskip != 0) mipskip--;
      }
      while (true);
    }
  }
}

void csGLTextureHandle::PrepareInt ()
{
  //@@@ Images may be lost if preparing twice. Some better way of solving it?
  if (!image.IsValid()) return;
  if (IsPrepared ()) return;
  SetPrepared (true);

  if (IsTransp() && !IsTranspSet())
  {
    int r,g,b;
    image->GetKeyColor (r,g,b);
    SetKeyColor (r, g, b);
  }						 

  // In opengl all textures, even non-mipmapped textures are required
  // to be powers of 2.
  AdjustSizePo2 ();

  csAlphaMode::AlphaType newAlphaType = csAlphaMode::alphaNone;

  // Do any resizing, if needed
  if (image->GetImageType() == csimgCube)
  {
    // Handle cube map faces
    csRef<csImageCubeMapMaker> newCube;
    int faceCount = MIN (image->HasSubImages() + 1, 6);
    for (int i = 0; i < faceCount; i++)
    {
      int newFaceW, newFaceH, newFaceD;
      csRef<iImage> imgFace = image->GetSubImage (i);
      ComputeNewPo2ImageSize (texFlags.Get(), 
	imgFace->GetWidth(), imgFace->GetHeight(), 1,
	newFaceW, newFaceH, newFaceD, txtmgr->max_tex_size);
      if (newFaceW != newFaceH) newFaceH = newFaceW;
      csRef<iImage> newFace = PrepareIntImage (newFaceW, newFaceH,
	imgFace->GetDepth (), imgFace, newAlphaType);
      if (newFace.IsValid())
      {
	// Create a new cube if we needed to resize one face.
	if (!newCube.IsValid()) 
	{
	  newCube.AttachNew (new csImageCubeMapMaker ());
	  newCube->SetName (image->GetName());
	}
	newCube->SetSubImage (i, newFace);
      }
    }
    if (faceCount < 6) // Ensure at least the 6 faces.
    {
      newCube.AttachNew (new csImageCubeMapMaker ());
      newCube->SetName (image->GetName());
    }
    if (newCube.IsValid())
    {
      for (int i = 0; i < faceCount; i++)
      {
	if (!newCube->SubImageSet (i))
	  newCube->SetSubImage (i, image->GetSubImage (i));
      }
      image = newCube;
    }
  }
  else
  {
    csRef<iImage> newImage = PrepareIntImage (actual_width, actual_height,
	actual_d, image, newAlphaType);
    if (newImage.IsValid()) image = newImage;
  }
  if (newAlphaType > alphaType) alphaType = newAlphaType;

  CreateMipMaps ();
  FreeImage ();
}

void csGLTextureHandle::GetKeyColor (uint8 &red,
	uint8 &green, uint8 &blue) const
{
  if (image.IsValid() && image->HasKeyColor() && !IsTranspSet ())
  {
    int r,g,b;
    image->GetKeyColor (r,g,b);
    red = r; green = g; blue = b;
  }
  else
  {
    red = transp_color.red;
    green = transp_color.green;
    blue = transp_color.blue;
  }
}

void csGLTextureHandle::AdjustSizePo2 ()
{
  if (IsSizeAdjusted ()) return;
  SetSizeAdjusted (true);

  //actual_d = orig_d = images->Length();
  orig_width  = image->GetWidth();
  orig_height = image->GetHeight();
  orig_d = image->GetDepth();

  if (texFlags.Check (CS_TEXTURE_NPOTS)) 
  {
    actual_width = MIN(orig_width, G3D->maxNpotsTexSize);
    actual_height = MIN(orig_height, G3D->maxNpotsTexSize);
    actual_d = MIN(orig_d, G3D->maxNpotsTexSize);
    return;
  }

  int newwidth, newheight, newd;

  ComputeNewPo2ImageSize (texFlags.Get(), orig_width, orig_height, orig_d, 
    newwidth, newheight, newd, txtmgr->max_tex_size);

  actual_width = newwidth;
  actual_height = newheight;
  actual_d = newd;
}

//---------------------------------------------------------------------------

static const csGLTextureClassSettings defaultSettings = 
  {GL_RGB, GL_RGBA, false, false, true, true};

csGLTextureManager::csGLTextureManager (iObjectRegistry* object_reg,
        iGraphics2D* iG2D, iConfigFile *config,
        csGLGraphics3D *iG3D) : 
  scfImplementationType (this), textures (16, 16)
{
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
}

void csGLTextureManager::read_config (iConfigFile *config)
{
  sharpen_mipmaps = config->GetInt
    ("Video.OpenGL.SharpenMipmaps", 0);
  texture_downsample = config->GetInt
    ("Video.OpenGL.TextureDownsample", 0);
  texture_filter_anisotropy = config->GetFloat
    ("Video.OpenGL.TextureFilterAnisotropy", 1.0);
  disableRECTTextureCompression = config->GetBool
    ("Video.OpenGL.DisableRECTTextureCompression", false);
  enableNonPowerOfTwo2DTextures = config->GetBool
    ("Video.OpenGL.EnableNonPowerOfTwo2DTextures", false);
  
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
  while (it->HasNext())
  {
    it->Next();
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
    csGLBasicTextureHandle* tex = textures[i];
    if (tex != 0) tex->Clear ();
  }
  for (i = 0; i < superLMs.Length(); i++)
  {
    superLMs[i]->DeleteTexture();
  }
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
          oldTU = statecache->GetCurrentTU ();
        statecache->SetCurrentTU (u);
	statecache->SetTexture (target, 0);
      }
    }
    if (oldTU != -1)
    {
      statecache->SetCurrentTU (oldTU);
      statecache->ActivateTU (csGLStateCache::activateImage);
    }
  }
  else
  {
    if (statecache->GetTexture (target) == texture)
      statecache->SetTexture (target, 0);
  }
}

csPtr<iTextureHandle> csGLTextureManager::RegisterTexture (iImage *image,
	int flags, iString* fail_reason)
{
  if (!image)
  {
    if (fail_reason) fail_reason->Replace (
      "No image given to RegisterTexture!");
    return 0;
  }

  if ((image->GetImageType() == csimgCube)
      && !G3D->ext->CS_GL_ARB_texture_cube_map)
  {
    if (fail_reason) fail_reason->Replace (
      "Cubemap images are not supported!");
    return 0;
  }
  if ((image->GetImageType() == csimg3D)
      && !G3D->ext->CS_GL_EXT_texture3D)
  {
    if (fail_reason) fail_reason->Replace (
      "3D images are not supported!");
    return 0;
  }

  csGLTextureHandle *txt = new csGLTextureHandle (image, flags, G3D);
  textures.Push(txt);
  return csPtr<iTextureHandle> (txt);
}

csPtr<iTextureHandle> csGLTextureManager::CreateTexture (int w, int h,
      csImageType imagetype, const char* format, int flags,
      iString* fail_reason)
{
  (void)w;
  (void)h;
  (void)imagetype;
  (void)format;
  (void)flags;
  if (fail_reason) fail_reason->Replace ("Not implemented yet!");
  return 0;
}

void csGLTextureManager::UnregisterTexture (csGLBasicTextureHandle* handle)
{
  size_t const idx = textures.Find (handle);
  if (idx != csArrayItemNotFound) textures.DeleteIndexFast (idx);
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
	outfn.Format ("%s%zu.png", dir, i);
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
  
//---------------------------------------------------------------------------

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

csGLRendererLightmap::csGLRendererLightmap () : scfImplementationType (this)
{
}

csGLRendererLightmap::~csGLRendererLightmap ()
{
#ifdef CS_DEBUG
  if (slm->texHandle != (GLuint)~0)
  {
    csRGBcolor* pat = new csRGBcolor[rect.Width () * rect.Height ()];
    int x, y;
    csRGBcolor* p = pat;
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
      GL_RGB, GL_UNSIGNED_BYTE, pat);

    delete[] pat;
  }
#endif
}

void csGLRendererLightmap::GetSLMCoords (int& left, int& top, 
    int& width, int& height)
{
  left = rect.xmin; top  = rect.ymin;
  width = rect.Width (); height = rect.Height ();
}
    
void csGLRendererLightmap::SetData (csRGBcolor* data)
{
  slm->CreateTexture ();

  csGLGraphics3D::statecache->SetTexture (
    GL_TEXTURE_2D, slm->texHandle);

  const uint8* packed = csPackRGB::PackRGBcolorToRGB (data,
    rect.Width () * rect.Height ());
  glTexSubImage2D (GL_TEXTURE_2D, 0, rect.xmin, rect.ymin, 
    rect.Width (), rect.Height (),
    GL_RGB, GL_UNSIGNED_BYTE, packed);
  csPackRGB::DiscardPackedRGB (packed);
}

void csGLRendererLightmap::SetLightCellSize (int size)
{
  (void)size;
}

//---------------------------------------------------------------------------


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
				      int width, int height) :
  scfImplementationType (this)
{
  w = width; h = height;
  texHandle = (GLuint)~0;
  numRLMs = 0;
  csGLSuperLightmap::txtmgr = txtmgr;
}

csGLSuperLightmap::~csGLSuperLightmap ()
{
  DeleteTexture ();
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

    csRGBcolor* data = new csRGBcolor [w * h];
#ifdef CS_DEBUG
    // Fill the background for debugging purposes (to quickly see what's
    // a lightmap and what's not; esp. useful when LMs are rather dark -
    // would be hardly visible if at all on black)
    // And to have it not that boring, add a neat backdrop.
    static const uint16 debugBG[16] =
      {0x0000, 0x3222, 0x4a36, 0x422a, 0x3222, 0x0a22, 0x4a22, 0x33a2, 
       0x0000, 0x2232, 0x364a, 0x2a42, 0x2232, 0x220a, 0x224a, 0xa233};

    csRGBcolor* p = data;
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
	p++;
      }
    }
#endif
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, 
      GL_RGB, GL_UNSIGNED_BYTE, data);
    delete[] data;
  }
}

void csGLSuperLightmap::DeleteTexture ()
{
  if (texHandle != (GLuint)~0)
  {
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
