/*
    Copyright (C) 1998 by Jorrit Tyberghein
    Texture manager for software renderer

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

#ifndef __CS_NULL_TXT_H__
#define __CS_NULL_TXT_H__

#include "csplugincommon/render3d/txtmgr.h"
#include "igraphic/image.h"

class csTextureManagerNull;

/**
 * csTextureHandleNull represents a texture and all its mipmapped
 * variants.
 */
class csTextureHandleNull : public csTextureHandle
{
protected:
  csString imageName;

  /// Is already prepared.
  bool prepared;

  /// The texture manager
  csTextureManagerNull *texman;

  int w, h, d;
  int orig_w, orig_h, orig_d;
public:
  /// Create the mipmapped texture object
  csTextureHandleNull (csTextureManagerNull *txtmgr, iImage *image, int flags);
  /// Destroy the object and free all associated storage
  virtual ~csTextureHandleNull ();

  /**
   * Query if the texture has an alpha channel.<p>
   * This depends both on whenever the original image had an alpha channel
   * and of the fact whenever the renderer supports alpha maps at all.
   */
  bool GetAlphaMap ()
  { return false; }

  csAlphaMode::AlphaType GetAlphaType () { return csAlphaMode::alphaNone; }

  void Precache () {}
  bool GetRendererDimensions (int &mw, int &mh)
  { mw = w; mh = h; return true; }
  bool GetRendererDimensions (int &mw, int &mh, int &md)
  { mw = w; mh = h; md = d; return true; }
  void GetOriginalDimensions (int& mw, int& mh, int& md)
  { mw = orig_w; mh = orig_h; md = orig_d; }
  void GetOriginalDimensions (int& mw, int& mh)
  { mw = orig_w; mh = orig_h; }
  void SetTextureTarget (int /*target*/) { }
  int GetTextureTarget () const { return iTextureHandle::CS_TEX_IMG_2D; }
  const char* GetImageName () const { return imageName; }
  virtual void Blit (int, int, int, int, unsigned char const*, 
    TextureBlitDataFormat) {}
};

/**
 * Software version of the texture manager. This instance of the
 * texture manager is probably the most involved of all 3D rasterizer
 * specific texture manager implementations because it needs to do
 * a lot of work regarding palette management and the creation
 * of lots of lookup tables.
 */
class csTextureManagerNull : public csTextureManager
{
private:
  /// We need a pointer to the 2D driver
  iGraphics2D *G2D;

public:
  ///
  csTextureManagerNull (iObjectRegistry *object_reg,
  	iGraphics2D *iG2D, iConfigFile *config);
  ///
  virtual ~csTextureManagerNull ();

  /// Called from G3D::Open ()
  void SetPixelFormat (csPixelFormat &PixelFormat);

  int GetTextureFormat ()
  { return CS_IMGFMT_TRUECOLOR | CS_IMGFMT_ALPHA; }

  ///
  virtual csPtr<iTextureHandle> RegisterTexture (iImage* image, int flags);
  ///
  virtual void UnregisterTexture (csTextureHandleNull* handle);

  virtual csPtr<iSuperLightmap> CreateSuperLightmap(int w, int h);
  
  virtual void GetMaxTextureSize (int& w, int& h, int& aspect);
};

#endif // __CS_NULL_TXT_H__
