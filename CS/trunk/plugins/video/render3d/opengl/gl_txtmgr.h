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

#ifndef __CS_GL_NEWTXTMGR_H__
#define __CS_GL_NEWTXTMGR_H__

#define CS_MATERIAL_MAX_TEXTURE_LAYERS 4

#define __CS_USE_NEW_RENDERER_NEW_TEXTURE_MANAGER 1

#include "csgfx/rgbpixel.h"
#include "ivideo/texture.h"
#include "ivideo/material.h"
#include "ivideo/txtmgr.h"
#include "iengine/texture.h"
#include "igraphic/imgvec.h"
#include "csutil/scf.h"
#include "csutil/refarr.h"
#include "igraphic/image.h"


#include "gl_render3d.h"
#include "video/canvas/openglcommon/glextmanager.h"

class csGLTextureHandle;
class csGLTextureManager;

class csGLTexture
{
public:
  csGLTexture(csGLTextureHandle *p, iImage* Image);
  virtual ~csGLTexture();

  csGLTextureHandle *Parent;

  uint8 *image_data;
  GLint compressed;
  GLint internalFormat;
  GLint size;
  int w, h, d, components;
   ///
  int get_width () const { return w; }
  ///
  int get_height () const { return h; }

  int get_depth () const { return d; }

  int get_components () const { return components; } 

  uint8 *&get_image_data ()
  { return image_data; }

  csGLTextureHandle *get_parent () { return Parent; }

};

class csGLTextureHandle : public iTextureHandle
{
private:
  /// texturemanager handle
  csGLTextureManager *txtmgr;

  
  GLenum sourceFormat, targetFormat;
  GLenum sourceType; // what size does each fragment have? e.g. GL_UNSIGNED_BYTE

  /// 2D Canvas
  csRef<iGraphics2D> canvas;
  /// The transparent color
  csRGBpixel transp_color;
  /// Mean color used when texture mapping is disabled.
  csRGBpixel mean_color;
  
  /// used until Prepare() is called
  csRef<iImageVector> images;

  /// Does color 0 mean "transparent" for this texture?
  bool transp;

  bool has_alpha;
  bool texupdate_needed;

  void *cachedata;

  bool transform (iImageVector *ImageVec, csGLTexture *tex);

  csGLTexture* NewTexture (iImage *Image, bool ismipmap);

public:
  int bpp;
  int formatidx;
  int orig_width, orig_height;
  csArray<csGLTexture*> vTex;
  csGLRender3D *R3D;
  long size;
  int flags;
  int target;
  bool was_render_target;

  csGLTextureHandle (iImage* image, int flags, int target, int bpp,
    GLenum sourceFormat, csGLRender3D *iR3D);

  csGLTextureHandle (csRef<iImageVector> image, int flags, int target, int bpp,
    GLenum sourceFormat, csGLRender3D *iR3D);

  virtual ~csGLTextureHandle ();

  GLenum SourceType () const { return sourceType; }
  GLenum SourceFormat () const { return sourceFormat; }
  GLenum TargetFormat () const { return targetFormat; }

  void Clear();

  void AdjustSizePo2 ();
  void CreateMipMaps();
  bool FindFormatType();


  SCF_DECLARE_IBASE;

  /// Retrieve the flags set for this texture
  virtual int GetFlags ();

  /// Enable key color
  virtual void SetKeyColor (bool Enable);

  /// Set the key color.
  virtual void SetKeyColor (uint8 red, uint8 green, uint8 blue);

  /// Get the key color status (false if disabled, true if enabled).
  virtual bool GetKeyColor ();

  /// Get the key color
  virtual void GetKeyColor (uint8 &red, uint8 &green, uint8 &blue);

  /// Release the original image (iImage) as given by the engine.
  void FreeImage ();
  /**
   * Get the dimensions for a given mipmap level (0 to 3).
   * If the texture was registered just for 2D usage, mipmap levels above
   * 0 will return false. Note that the result of this function will
   * be the size that the renderer uses for this texture. In most cases
   * this corresponds to the size that was used to create this texture
   * but some renderers have texture size limitations (like power of two)
   * and in that case the size returned here will be the corrected size.
   * You can get the original image size with GetOriginalDimensions().
   */
  virtual bool GetMipMapDimensions (int mipmap, int &mw, int &mh);

  /**
   * Return the original dimensions of the image used to create this texture.
   * This is most often equal to GetMipMapDimensions (0, mw, mh) but in
   * some cases the texture will have been resized in order to accomodate
   * hardware restrictions (like power of two and maximum texture size).
   * This function returns the uncorrected coordinates.
   */
  virtual void GetOriginalDimensions (int& mw, int& mh);


  //enum { CS_TEX_IMG_1D = 0, CS_TEX_IMG_2D, CS_TEX_IMG_3D, CS_TEX_IMG_CUBEMAP };
  /**
   * Texture Depth Indices are used for Cubemap interface
   */
  //enum { CS_TEXTURE_CUBE_POS_X = 0, CS_TEXTURE_CUBE_NEG_X, 
  //       CS_TEXTURE_CUBE_POS_Y, CS_TEXTURE_CUBE_NEG_Y,
  //       CS_TEXTURE_CUBE_POS_Z, CS_TEXTURE_CUBE_NEG_Z };

  /**
   * Get the dimensions for a given mipmap level (0 to 3).
   * If the texture was registered just for 2D usage, mipmap levels above
   * 0 will return false. Note that the result of this function will
   * be the size that the renderer uses for this texture. In most cases
   * this corresponds to the size that was used to create this texture
   * but some renderers have texture size limitations (like power of two)
   * and in that case the size returned here will be the corrected size.
   * You can get the original image size with GetOriginalDimensions().
   */
  virtual bool GetMipMapDimensions (int mipmap, int &mw, int &mh, int &md);

  /**
   * Return the original dimensions of the image used to create this texture.
   * This is most often equal to GetMipMapDimensions (0, mw, mh, md) but in
   * some cases the texture will have been resized in order to accomodate
   * hardware restrictions (like power of two and maximum texture size).
   * This function returns the uncorrected coordinates.
   */
  virtual void GetOriginalDimensions (int& mw, int& mh, int &md);

  /**
   *Sets Texture Target. 
   *This can either be CS_TEXTURE_1D, CS_TEXTURE_2D, CS_TEXTURE_3D, 
   *CS_TEXTURE_CUBEMAP etc.
   *With CS_TEXTURE_CUBEMAP, the depth index specifies the side 
   *of the cubemap (CS_TEXTURE_CUBE_POS_X, CS_TEXTURE_CUBE_NEG_X,
   *CS_TEXTURE_CUBE_POS_Y, etc.
   */
  virtual void SetTextureTarget(int target);


  /// Get the mean color.
  virtual void GetMeanColor (uint8 &red, uint8 &green, uint8 &blue);

  /// Get data associated internally with this texture by texture cache
  virtual void *GetCacheData ();

  /// Set data associated internally with this texture by texture cache
  virtual void SetCacheData (void *d);

  /**
   * Query the private object associated with this handle.
   * For internal usage by the 3D driver.
   */
  virtual void *GetPrivateObject ();

  /**
   * Query if the texture has an alpha channel.<p>
   * This depends both on whenever the original image had an alpha channel
   * and of the fact whenever the renderer supports alpha maps at all.
   */
  virtual bool GetAlphaMap ();

  /**
   * Merge this texture into current palette, compute mipmaps and so on.
   * You should call either Prepare() or iTextureManager::PrepareTextures()
   * before using any texture.
   */
  virtual void Prepare ();

  /**
   * Get a canvas instance which is suitable for rendering on this
   * texture. Note that it is not allowed to change the palette of
   * the returned canvas.
   */
  virtual iGraphics2D* GetCanvas ();
};

/*
*
* new MaterialHandle Implementation
* by Phil Aumayr (phil@rarebyte.com)
*
*/
class csGLMaterialHandle : public iMaterialHandle
{
protected:
  /// Original material.
  csRef<iMaterial> material;
  /// Parent texture manager
  //csRef<csGLTextureManager> texman;
  csGLTextureManager* texman;

public:
  ///
  csGLMaterialHandle (iMaterial* material, csGLTextureManager *parent);
  ///
  csGLMaterialHandle (iTextureHandle* texture, csGLTextureManager *parent);
  ///
  virtual ~csGLMaterialHandle ();

  /// Release the original material (iMaterial).
  void FreeMaterial ();

  /// Get the material.
  iMaterial* GetMaterial () { return material; }

  //--------------------- iMaterialHandle implementation ----------------------
  SCF_DECLARE_IBASE;

  /**
   * Get associated shader
   */
  virtual iShaderWrapper* GetShader (csStringID type) 
  { return material->GetShader (type); }

  /**
   * Get a texture from the material.
   */
  virtual iTextureHandle *GetTexture ();

  /**
   * Get the flat color. If the material has a texture assigned, this
   * will return the mean texture color.
   */
  virtual void GetFlatColor (csRGBpixel &oColor) 
  { 
    material->GetFlatColor (oColor);
  }

  /**
   * Get light reflection parameters for this material.
   */
  virtual void GetReflection (float &oDiffuse, float &oAmbient,
    float &oReflection)
  { 
    material->GetReflection (oDiffuse, oAmbient, oReflection);
  }

  /**
   * Prepare this material. The material wrapper (remembered during
   * RegisterMaterial()) is queried again for material parameters
   * and a new material descriptor (internal to the texture manager)
   * is associated with given material handle.
   */
  virtual void Prepare ();

  virtual iShaderBranch* QueryShaderBranch ()
  { return (iShaderBranch*)this; } // @@@ Evil cast?
};

/*
*
* New Texture Manager... done by Phil Aumayr (phil@rarebyte.com)
*
*/
class csGLTextureManager : public iTextureManager
{
  struct formatDescription
  {
    GLenum targetFormat;
    char *name;
    GLenum sourceFormat;
    int components; // number of components in texture
    GLint compressedFormat;
    GLenum forcedFormat;
    int texelbytes;
  };

  csRef<csGLRender3D> R3D;

  typedef csRefArray<csGLTextureHandle> csTexVector;
  /// List of textures.
  csTexVector textures;
  // Private class used to keep a list of objects derived from csMaterialHandle
  typedef csRefArray<csGLMaterialHandle> csMatVector;
  /// List of materials.
  csMatVector materials;

  csPixelFormat pfmt;
  
  bool verbose;

  void AlterTargetFormat (const char *oldTarget, const char *newTarget);

  iObjectRegistry *object_reg;
public:

  int max_tex_size;
  /// Sharpen mipmaps?
  int sharpen_mipmaps;
  /// downsample textures?
  int texture_downsample;
  /// texture filtering anisotropy
  float texture_filter_anisotropy;
  /// what bpp should textures have?
  int texture_bits;

  csStringID nameDiffuseTexture;

  static formatDescription glformats [];

  csGLTextureManager (iObjectRegistry* object_reg,
        iGraphics2D* iG2D, iConfigFile *config,
        csGLRender3D *R3D);

  virtual ~csGLTextureManager () { }

  /// Read configuration values from config file.
  void read_config (iConfigFile *config);
  void Clear ();
  void DetermineStorageSizes();
  void SetPixelFormat (csPixelFormat &PixelFormat);


  /**
   * Called from csMaterialHandle destructor to notify parent texture
   * manager that a material is going to be destroyed.
   */
  void UnregisterMaterial (csGLMaterialHandle* handle);


  SCF_DECLARE_IBASE;
  /**
   * Register a texture. The given input image is IncRef'd and DecRef'ed
   * later when FreeImages () is called. If you want to keep the input image
   * make sure you have called IncRef yourselves.
   *<p>
   * The texture is not converted immediately. Instead, you can make
   * intermediate calls to iTextureHandle::SetKeyColor (). Finally,
   * if you want to merge the texture into the current environment, you
   * should call txt->Prepare(). Alternatively you can call the
   * PrepareTextures () method to compute a optimal palette and convert
   * all the textures into the internal format.
   *<p>
   * This function returns a handle which should be given
   * to the 3D rasterizer or 2D driver when drawing or otherwise using
   * the texture.
   *<p>
   * The `flags' contains one or several of CS_TEXTURE_XXX flags OR'ed
   * together. They define the mode texture is going to be used in.
   *<p>
   * The texture manager will reject the texture if it is an inappropiate
   * format (see GetTextureFormat () method).
   *<p>
   * The texture is unregistered at destruction, i.e. as soon as the last
   * reference to the texture handle is released.
   *<p>
   * If CS_USE_NEW_RENDERER is enabled param target specifies the texture target.
   * Defines for that can be found in ivideo/texture.h
   *
   */
  virtual csPtr<iTextureHandle> RegisterTexture (iImage *image, int flags);

  virtual csPtr<iTextureHandle> RegisterTexture (iImageVector *image, int flags, int target);

  /**
   * After all textures have been added, this function does all
   * needed calculations (palette, lookup tables, mipmaps, ...).
   * PrepareTextures () must be able to handle being called twice
   * or more without ill effects.
   */
  virtual void PrepareTextures ();

  /**
   * Call this function if you want to release all iImage's as
   * given to this texture manager. After FreeImages() has been called
   * it is no longer allowed to call Prepare() again. So the advantage
   * of calling FreeImages() is that you gain memory (may be a lot)
   * but the disadvantage is that when you want to add textures later
   * you have to reload them all and start all over.
   */
  virtual void FreeImages ();

  /**
   * Register a material. The input material is IncRef'd and DecRef'ed
   * later when FreeMaterials () is called or the material handle is destroyed
   * by calling DecRef on it enough times. If you want to keep the input
   * material make sure you have called IncRef yourselves. <p>
   *
   * The material is unregistered at destruction, i.e. as soon as the last
   * reference to the material handle is released.
   */
  virtual csPtr<iMaterialHandle> RegisterMaterial (iMaterial* material);

  /**
   * Register a material based on a texture handle. This is a short-cut
   * to quickly make materials based on a single texture.
   *
   * The material is unregistered at destruction, i.e. as soon as the last
   * reference to the material handle is released.
   */
  virtual csPtr<iMaterialHandle> RegisterMaterial (
  	iTextureHandle* txthandle);

  /**
   * Prepare all materials.
   */
  virtual void PrepareMaterials ();

  /**
   * Call this function if you want to release all iMaterial's as
   * given to this texture manager.
   */
  virtual void FreeMaterials ();

  /**
   * Set verbose mode on/off. In verbose mode, texture manager will
   * Printf() through the system driver during all initialization and
   * preparation operations.
   */
  virtual void SetVerbose (bool vb);

  /**
   * Query the basic format of textures that can be registered with this
   * texture manager. It is very likely that the texture manager will
   * reject the texture if it is in an improper format. The alpha channel
   * is optional; the texture can have it and can not have it. Only the
   * bits that fit the CS_IMGFMT_MASK mask matters.
   */
  virtual int GetTextureFormat ();

  virtual csPtr<iSuperLightmap> CreateSuperLightmap(int, int);

  virtual void GetMaxTextureSize (int& w, int& h, int& aspect);

  virtual int GetMaterialIndex (iMaterialHandle* mat)
  {
    return materials.Find ((csGLMaterialHandle*)mat); // @@@ Evil cast?
  }
};

#define CS_GL_FORMAT_TABLE(var) \
csGLTextureManager::formatDescription var[] = {

#define CS_GL_FORMAT(dsttype, srctype, size, texelsize) \
{dsttype, #dsttype, srctype, size, 0, (GLenum)0, texelsize},

#define CS_GL_FORMAT_TABLE_END \
{(GLenum)0, 0, (GLenum)0, 0, 0, (GLenum)0, 0}};


#endif // __CS_GL_TXTMGR_H__

