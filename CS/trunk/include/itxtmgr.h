/*
    Copyright (C) 1998 by Jorrit Tyberghein
    Written by Jorrit Tyberghein.

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

#ifndef __ITXTMGR_H__
#define __ITXTMGR_H__

#include "csutil/scf.h"

class Vector2;
class csMatrix3;
class csVector3;
class csRect;

struct iImage;
struct iTextureHandle;

/**
 * Texture registration flags. During texture registration you should tell
 * the manager which way you're going to use the texture: whenever you're
 * going to use it for 2D (DrawSprite ()), for 3D (DrawPolygon ()), whenever
 * the texture will be dynamically modified.
 */
/// You're going to yse the texture for 2D drawing
#define CS_TEXTURE_2D		0x00000001
/// You're going to yse the texture for 3D drawing
#define CS_TEXTURE_3D		0x00000002
/**
 * The texture manager should be able to hand you a pointer to some buffer
 * containing texture image and a format description. You can change the
 * contents of that buffer and then hand it back to the texture manager.
 * Then texture manager recompute that texture from the image buffer.
 */
#define CS_TEXTURE_DYNAMIC	0x00000004

SCF_VERSION (iTextureManager, 1, 0, 0);

/**
 * This is the standard texture manager interface.
 * A 3D rasterizer will have to implement a subclass of this one and
 * return a pointer to it in Graphics3D.
 * This class is responsible for receiving all textures
 * from the 3D engine, converting them to an internal format if
 * needed, calculating a palette if needed, and calculating all
 * lookup tables related to the textures. Mipmap creation is
 * also done in this class.
 */
struct iTextureManager : public iBase
{
  /**
   * Register a texture. The given input image is IncRef'd and DecRef'ed
   * later when FreeImages () is called. If you want to keep the input image
   * make sure you have called IncRef yourselves.
   *<p>
   * The texture is not converted immediately. Instead, you can make
   * intermediate calls to iTextureHandle::SetTransparent (). Finally,
   * if you want to merge the texture into the current environment, you
   * should call PrepareTexture (). Alternatively you can call the
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
   */
  virtual iTextureHandle *RegisterTexture (iImage *image, int flags) = 0;

  /**
   * Unregister a texture. Note that this will have no effect on the
   * possible palette and lookup tables until after PrepareTextures ()
   * is called again. Note that the texture will be deleted only when
   * you will remove all references to this texture (i.e. you will do
   * DecRef () as much times as you did IncRef () plus one)
   */
  virtual void UnregisterTexture (iTextureHandle *handle) = 0;

  /**
   * Merge this texture into current palette, compute mipmaps and so on.
   * You should call either PrepareTexture() or PrepareTextures() before
   * using any texture.
   */
  virtual void PrepareTexture (iTextureHandle *handle) = 0;

  /**
   * After all textures have been added, this function does all
   * needed calculations (palette, lookup tables, mipmaps, ...).
   * PrepareTextures () must be able to handle being called twice
   * or more without ill effects.
   */
  virtual void PrepareTextures () = 0;

  /**
   * Call this function if you want to release all iImage's as
   * given to this texture manager. After FreeImages() has been called
   * it is no longer allowed to call Prepare() again. So the advantage
   * of calling FreeImages() is that you gain memory (may be a lot)
   * but the disadvantage is that when you want to add textures later
   * you have to reload them all and start all over.
   */
  virtual void FreeImages () = 0;

  /**
   * Reset all reserved colors in palette. This function should be called
   * if you want to reverse the effect of all ReserveColor() calls.
   * The function will have effect on next call to PrepareTextures ().
   */
  virtual void ResetPalette () = 0;

  /**
   * Reserve RGB. Call this function to reserve a color
   * from the palette (if any). This function only takes effect after
   * the next call to Prepare (). Note that black (0) and white (255)
   * are already preallocated colors.
   */
  virtual void ReserveColor (int r, int g, int b) = 0;

  /**
   * Return a color. Find the color in palette and return the palette
   * index that contains the nearest color. For 15-, 16- and 32-bit modes
   * this returns a encoded RGB color as needed by both 2D and 3D drivers.
   */
  virtual int FindRGB (int r, int g, int b) = 0;

  /**
   * Switch to the new palette. This function should be called
   * after you called the PrepareTextures() method which will compute
   * a optimal palette for all textures. Of course, it is not neccessarily
   * to call it directly after PrepareTextures() but you should call it before
   * using any texture, otherwise they will look garbled.
   */
  virtual void SetPalette () = 0;

  /**
   * Set verbose mode on/off. In verbose mode, texture manager will
   * Printf() through the system driver during all initialization and
   * preparation operations.
   */
  virtual void SetVerbose (bool vb) = 0;

  /**
   * Query the basic format of textures that can be registered with this
   * texture manager. It is very likely that the texture manager will
   * reject the texture if it is in an improper format. The alpha channel
   * is optional; the texture can have it and can not have it. Only the
   * bits that fit the CS_IMGFMT_MASK mask matters.
   */
  virtual int GetTextureFormat () = 0;
};

#endif // __ITXTMGR_H__
