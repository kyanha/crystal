/*
    Copyright (C) 1998-2004 by Jorrit Tyberghein
	      (C) 2003 by Philip Aumayr
	      (C) 2004-2007 by Frank Richter

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

#include "igraphic/imageio.h"
#include "iutil/vfs.h"

#include "gl_render3d.h"
#include "gl_txtmgr.h"
#include "gl_txtmgr_imagetex.h"
#include "gl_txtmgr_lightmap.h"

CS_PLUGIN_NAMESPACE_BEGIN(gl3d)
{

CS_LEAKGUARD_IMPLEMENT(csGLTextureManager);

static const csGLTextureClassSettings defaultSettings = 
  {GL_RGB, GL_RGBA, false, false, true, true};

csGLTextureManager::csGLTextureManager (iObjectRegistry* object_reg,
        iGraphics2D* iG2D, iConfigFile *config,
        csGLGraphics3D *iG3D) : 
  scfImplementationType (this), textures (16, 16), compactTextures (false)
{
  csGLTextureManager::object_reg = object_reg;
  
  G3D = iG3D;
  max_tex_size = G3D->GetMaxTextureSize ();

  G3D->ext->InitGL_ARB_texture_compression ();
  if (G3D->ext->CS_GL_ARB_texture_compression)
    G3D->ext->InitGL_EXT_texture_compression_s3tc (); 
  G3D->ext->InitGL_ARB_texture_float();
  /*if (G3D->ext->CS_GL_ARB_texture_float)
    G3D->ext->InitGL_ARB_half_float_pixel();*/

  G3D->ext->InitGL_ARB_pixel_buffer_object();
  hasPBO = G3D->ext->CS_GL_ARB_pixel_buffer_object;

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
#undef CS_GL_TEXTURE_FORMAT_EXT
#undef CS_GL_TEXTURE_FORMAT

  textureClasses.Put (textureClassIDs.Request ("default"), defaultSettings);

  read_config (config);
  InitFormats ();
}

csGLTextureManager::~csGLTextureManager()
{
  Clear ();
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
  for (i=0; i < textures.GetSize (); i++)
  {
    csGLBasicTextureHandle* tex = textures[i];
    if (tex != 0) tex->Clear ();
  }
  textures.DeleteAll ();
  for (i = 0; i < superLMs.GetSize (); i++)
  {
    superLMs[i]->DeleteTexture();
  }
  superLMs.DeleteAll ();
}

void csGLTextureManager::SetupPixelStore()
{
  G3D->statecache->SetPixelUnpackAlignment (1);
#ifndef CS_LITTLE_ENDIAN
  // The texture format stuff generally assumes little endian
  G3D->statecache->SetPixelSwapBytes (true);
#endif
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
  CompactTextures ();
  textures.Push(txt);
  return csPtr<iTextureHandle> (txt);
}

csPtr<iTextureHandle> csGLTextureManager::CreateTexture (int w, int h,
      csImageType imagetype, const char* format, int flags,
      iString* fail_reason)
{
  CS::StructuredTextureFormat texFormat (
    CS::TextureFormatStrings::ConvertStructured (format));
  if (!texFormat.IsValid()) 
  {
    if (fail_reason) fail_reason->Replace ("invalid texture format");
    return 0;
  }

  /* @@@ TODO: Does it make sense to "optimize" formats?
         For example, NV is fastest when GL_BGR(A) is used. When a color format
         is specified, an "argb" format (CS notation for GL_BGR(A)) could be
         substituted instead. Or does the driver handle that automatically?
   */

  csGLBasicTextureHandle *txt = new csGLBasicTextureHandle (
      w, h, 1, imagetype, flags, G3D);

  if (!txt->SynthesizeUploadData (texFormat, fail_reason))
  {
    delete txt;
    return 0;
  }

  CompactTextures ();
  textures.Push(txt);
  return csPtr<iTextureHandle> (txt);
}

void csGLTextureManager::CompactTextures ()
{
  if (!compactTextures) return;
    
  size_t i = 0;
  while (i < textures.GetSize ())
  {
    if (textures[i] == 0)
      textures.DeleteIndexFast (i);
    else
      i++;
  }
  
  compactTextures = false;
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
  for (size_t i = 0; i < superLMs.GetSize (); i++)
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

}
CS_PLUGIN_NAMESPACE_END(gl3d)
