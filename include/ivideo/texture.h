/*
    Copyright (C) 1998, 2000 by Jorrit Tyberghein
                        2004 by Marten Svanfeldt

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

#ifndef __CS_IVIDEO_TEXTURE_H__
#define __CS_IVIDEO_TEXTURE_H__

/**\file
 * Texture handle interface
 */

/**
 * \addtogroup gfx3d
 * @{ */

#include "csutil/scf_interface.h"
#include "cstypes.h"
#include "ivideo/graph3d.h"

struct iGraphics2D;
struct iGraphics3D;


/**
 * A texture handle as returned by iTextureManager.
 *
 * Main creators of instances implementing this interface:
 * - iTextureManager::RegisterTexture()
 *
 * Main ways to get pointers to this interface:
 * - iTextureWrapper::GetTextureHandle()
 *
 * Main users of this interface:
 * - 3D renderer implementations (iGraphics3D).
 */
struct iTextureHandle : public virtual iBase
{
  SCF_INTERFACE(iTextureHandle, 4,0,3);
  /// Retrieve the flags set for this texture
  virtual int GetFlags () const = 0;

  /// Enable key color
  virtual void SetKeyColor (bool Enable) = 0;

  /// Set the key color.
  virtual void SetKeyColor (uint8 red, uint8 green, uint8 blue) = 0;

  /// Get the key color status (false if disabled, true if enabled).
  virtual bool GetKeyColor () const = 0;

  /// Get the key color
  virtual void GetKeyColor (uint8 &red, uint8 &green, uint8 &blue) const = 0;

  /**
   * Get the dimensions the renderer uses for this texture.
   * In most cases this corresponds to the size that was used to create this
   * texture, but some renderers have texture size limitations (like power
   * of two) and in that case the size returned here will be the corrected
   * size.
   * You can get the original image size with GetOriginalDimensions().
   * \return Whether the renderer-used dimensions could be determined.
   */
  virtual bool GetRendererDimensions (int &mw, int &mh) = 0;

  /**
   * Return the original dimensions of the image used to create this texture.
   * This is most often equal to GetMipMapDimensions (0, mw, mh) but in
   * some cases the texture will have been resized in order to accomodate
   * hardware restrictions (like power of two and maximum texture size).
   * This function returns the uncorrected coordinates.
   */
  virtual void GetOriginalDimensions (int& mw, int& mh) = 0;

  // CHANGED TO ADD SUPPORT FOR CUBEMAPS AND 3D TEXTURES
  // done by Phil Aumayr (phil@rarebyte.com)
  enum //CS_DEPRECATED_TYPE
  {
    CS_TEX_IMG_1D = 0,
    CS_TEX_IMG_2D,
    CS_TEX_IMG_3D,
    CS_TEX_IMG_CUBEMAP,
    CS_TEX_IMG_RECT
  };
  /**
   * Texture Depth Indices are used for Cubemap interface
   */
  enum { CS_TEXTURE_CUBE_POS_X = 0, CS_TEXTURE_CUBE_NEG_X,
         CS_TEXTURE_CUBE_POS_Y, CS_TEXTURE_CUBE_NEG_Y,
         CS_TEXTURE_CUBE_POS_Z, CS_TEXTURE_CUBE_NEG_Z };


  /**
   * Get the dimensions the renderer uses for this texture.
   * In most cases this corresponds to the size that was used to create this
   * texture, but some renderers have texture size limitations (like power
   * of two) and in that case the size returned here will be the corrected
   * size.
   * You can get the original image size with GetOriginalDimensions().
   * \return Whether the renderer-used dimensions could be determined.
   */
  virtual bool GetRendererDimensions (int &mw, int &mh, int &md) = 0;

  /**
   * Return the original dimensions of the image used to create this texture.
   * This is most often equal to GetMipMapDimensions (0, mw, mh, md) but in
   * some cases the texture will have been resized in order to accomodate
   * hardware restrictions (like power of two and maximum texture size).
   * This function returns the uncorrected coordinates.
   */
  virtual void GetOriginalDimensions (int& mw, int& mh, int &md) = 0;

  /**
   * Get the texture target. Note the texture target is determined by the
   * image from which the texture was created and possibly the texture flags.
   * \deprecated Deprecated in 1.3. Use GetTextureType() instead
   */
  CS_DEPRECATED_METHOD_MSG("Use GetTextureType() instead")
  virtual int GetTextureTarget () const = 0;

  /// Format of the pixel data that is passed to iTextureHandle->Blit()
  enum TextureBlitDataFormat
  {
    /// RGBA, 8 bits per pixel
    RGBA8888 = 0,
    /// BGRA, 8 bits per pixel
    BGRA8888
  };

  /**
   * Blit a memory block to this texture. Format of the image is determined
   * by the \a format parameter. Row by row.
   * \remarks If the specified target rectangle exceeds the texture dimensions
   *   the Blit() call may have no effect. It's the responsibility of the
   *   caller to ensure that the rectangle lies completely inside the texture.
   */
  virtual void Blit (int x, int y, int width, int height,
    unsigned char const* data, TextureBlitDataFormat format = RGBA8888) = 0;

  /**
   * Get the original image name.
   */
  virtual const char* GetImageName () const = 0;

  /**
   * Query the private object associated with this handle.
   * For internal usage by the 3D driver.
   */
  virtual void *GetPrivateObject () = 0;

  /**
   * Query if the texture has an alpha channel.<p>
   * This depends both on whenever the original image had an alpha channel
   * and of the fact whenever the renderer supports alpha maps at all.
   */
  virtual bool GetAlphaMap () = 0;

  /// Get the type of alpha associated with the texture.
  virtual csAlphaMode::AlphaType GetAlphaType () const = 0;

  /**
   * Precache this texture. This might free up temporary memory and
   * makes later usage of the texture faster.
   */
  virtual void Precache () = 0;

  /**
   * Check if this texture has already been precached.
   */
  virtual bool IsPrecached () = 0;

  /**
   * Set the "class" of this texture.
   * A texture class is used to set some characteristics on how a texture is
   * handled at runtime. For example, graphics hardware usually offers texture
   * compression, but it can cause a loss of quality and precision and thus
   * may not be desireable for all data. In this case, a class can be set on
   * the texture that instructs the renderer to not apply texture compression.
   * \remarks Not all renderers may support texture classes.
   * \sa GetTextureClass
   */
  virtual void SetTextureClass (const char* className) = 0;
  /**
   * Get the "class" of a texture.
   * \sa SetTextureClass
   */
  virtual const char* GetTextureClass () = 0;

  /**
   * Set the type of alpha associated with the texture.
   * Usually, the alpha mode is auto-detected (alphaSmooth on images with alpha
   * channels, alphaBinary on keycolored images, alphaNone otherwise), but can
   * be overridden with this method.
   */
  virtual void SetAlphaType (csAlphaMode::AlphaType alphaType) = 0;

  /// Possible texture types
  enum TextureType
  {
    /// 1D texture
    texType1D = 0,
    /// 2D texture
    texType2D,
    /// 3D texture
    texType3D,
    /// Cube map
    texTypeCube,
    /// Rectangle texture
    texTypeRect
  };

  /**
   * Get the texture type (2D, 3D, cube map, RECT texture ...). Note that the
   * texture type is determined by the image from which the texture was
   * created and possibly the texture flags.
   */
  virtual TextureType GetTextureType() const = 0;

  enum
  {
    /// The buffer will also be read from, so make sure it's readable
    blitbufReadable = 1,
    /**
     * The returned buffer should mirror the current contents of the requested
     * area on the texture. If this flag is not present the buffer contents
     * will be undefined and should thus be reset entirely. However, setting
     * this flag may incur a performance penalty.
     */
    blitbufRetainArea = 2
  };

  /**
   * Query a buffer for blitting.
   * \par Multiple areas
   * If multiple areas of the texture are to be updated, call
   * QueryBlitBuffer () for a new area before you call ApplyBlitBuffer () for
   * the prior one. That is, the order of calls should be like:
   * \code
   *   uint8* buf1 = texture->QueryBlitBuffer (...);
   *   // ... Change buf1 ...
   *   uint8* buf2 = texture->QueryBlitBuffer (...);
   *   texture->ApplyBlitBuffer (buf1);
   *   // ... Change buf1 ...
   *   // And so on
   * \endcode
   * This "interleaving" will result in greater runtime performance as some
   * resources can be re-used.
   *
   * When multiple queried areas overlap the overlapping areas will contain
   * undefined data on the texture.
   */
  virtual uint8* QueryBlitBuffer (int x, int y, int width, int height,
    size_t& pitch, TextureBlitDataFormat format = RGBA8888,
    uint bufFlags = 0) = 0;
  /// Apply the given blitting buffer.
  virtual void ApplyBlitBuffer (uint8* buf) = 0;
  /// Characteristics of a blit buffer
  enum BlitBufferNature
  {
    /**
     * The returned buffer is an intermediate buffer which is applied only
     * indirectly, possibly with a call to a foreign module.
     * This information is mostly useful in cases where some state has to be
     * set before a foreign call could possibly occur. An examples for such
     * action is code that utilizes MMX which should execute the 'emms'
     * instruction before foreign code is called.
     */
    natureIndirect = 0,
    /**
     * The returned buffer is actually a direct pointer to the texture
     * storage.
     */
    natureDirect = 1
  };
  virtual BlitBufferNature GetBufferNature (uint8* buf) = 0;
};

/** @} */

#endif // __CS_IVIDEO_TEXTURE_H__
