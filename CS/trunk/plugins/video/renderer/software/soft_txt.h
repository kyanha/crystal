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

#ifndef __SOFT_TXT_H__
#define __SOFT_TXT_H__

#include "video/renderer/common/txtmgr.h"
#include "iimage.h"
#include "igraph2d.h"

class csGraphics3DSoftwareCommon;
class csTextureManagerSoftware;
class csTextureMMSoftware;
class csSoftProcTexture3D;
/**
 * In 8-bit modes we build a 32K inverse colormap for converting
 * RGB values into palette indices. The following macros defines
 * the number of bits to use for encoding R, G and B values.
 */
#define RGB2PAL_BITS_R	5
#define RGB2PAL_BITS_G	5
#define RGB2PAL_BITS_B	5

/**
 * Lookup table for alpha mapping. Converts two palette entries
 * to a new palette entry (alpha blended).
 * (used for display output of 8-bit).
 */
struct csAlphaTables
{
  /// Alpha table for 50%
  UByte alpha_map50 [256*256];
  /// Alpha table for 25% and 75%
  UByte alpha_map25 [256*256];
};

/// The prefered distances to use for the color matching.
#define PREFERED_DIST	2000000

/**
 * A class containing a colormap. A object of this class is used
 * for the global colormap in 8-bit modes.
 */
class csColorMap
{
public:
  /**
   * Array with RGB values for every color of the palette.
   * An RGB value has four bytes: R, G, B, unused.
   * So there are 256*4 bytes in this array.
   */
  RGBPixel palette [256];

  /// Colors which are allocated.
  bool alloc [256];

  /// Constructor
  csColorMap ()
  { memset (alloc, sizeof (alloc), 0); }

  /// Find a value in the colormap and return the color index.
  int find_rgb (int r, int g, int b, int *d = NULL);

  /// Allocate a new RGB color in the colormap.
  int alloc_rgb (int r, int g, int b, int dist);

  /// Get a palette entry
  inline RGBPixel &operator [] (int idx)
  { return palette [idx]; }

  /// Compute number of free palette entries
  int FreeEntries ();
};

/**
 * csTextureSoftware is a class derived from csTexture that implements
 * all the additional functionality required by the software renderer.
 * Every csTextureSoftware is a 8-bit paletted image with a private
 * colormap. The private colormap is common for all mipmapped variants.
 * The colormap is stored inside the parent csTextureMM object.
 */
class csTextureSoftware : public csTexture
{
public:
  /// The bitmap
  uint8 *bitmap;
  /// The alpha map (NULL if no alphamap)
  uint8 *alphamap;
  /// The image (temporary storage)
  iImage *image;

  /// Create a csTexture object
  csTextureSoftware (csTextureMM *Parent, iImage *Image) : csTexture (Parent)
  {
    bitmap = NULL;
    alphamap = NULL;
    image = Image;
    w = Image->GetWidth ();
    h = Image->GetHeight ();
    compute_masks ();
  }
  /// Destroy the texture
  virtual ~csTextureSoftware ()
  { delete [] bitmap; if (image) image->DecRef (); }

  /// Return a pointer to texture data
  uint8 *get_bitmap ()
  { return bitmap; }
  /// Return a pointer to alpha map data
  uint8 *get_alphamap ()
  { return alphamap; }
};

/**
 * csTextureSoftwareProc is a class derived from csTextureSoftware that
 * implements the additional functionality to allow acess to the texture
 * memory via iGraphics2D/3D interfaces. Internally iGraphics2D/3D are 
 * initialised in 8bit mode and use the palette as calculated by 
 * csTextureMMSoftware, so you can render to them as usual without
 * requiring recalculation of palettes each frame. 
 */
class csTextureSoftwareProc : public csTextureSoftware
{
public:
  csSoftProcTexture3D *texG3D;

  csTextureSoftwareProc (csTextureMM *Parent, iImage *Image)
    : csTextureSoftware (Parent, Image), texG3D(NULL)
  {};
  /// Destroy the texture
  virtual ~csTextureSoftwareProc ();

};


/**
 * csTextureMMSoftware represents a texture and all its mipmapped
 * variants.
 */
class csTextureMMSoftware : public csTextureMM
{
protected:
  /**
   * Private colormap -> global colormap table
   * For 16- and 32-bit modes this array contains a 256-element array
   * of either shorts or longs to convert any image pixel from 8-bit
   * paletted format to the native pixel format.
   */
  void *pal2glob;

  /// Same but for 8-bit modes (8bit to 16bit values)
  uint16 *pal2glob8;

  /// The private palette for this texture (compressed a little)
  UByte *orig_palette;

  /// The private palette (with gamma applied)
  RGBPixel palette [256];

  /// Number of used colors in palette
  int palette_size;

  /// Create a new texture object
  virtual csTexture *NewTexture (iImage *Image);

  /// Compute the mean color for the just-created texture
  virtual void ComputeMeanColor ();

  /// the texture manager
  csTextureManagerSoftware *texman;

public:
  /// Create the mipmapped texture object
  csTextureMMSoftware (csTextureManagerSoftware *texman, iImage *image, 
		       int flags);
  /// Destroy the object and free all associated storage
  virtual ~csTextureMMSoftware ();

  /**
   * Create the [Private colormap] -> global colormap table.
   * In 256-color modes we find the correspondense between private texture
   * colormap and the global colormap; in truecolor modes we just build
   * a [color index] -> [truecolor value] conversion table.
   */
  void remap_texture ();

  /// Query the private texture colormap
  RGBPixel *GetColorMap () { return palette; }
  /// Query the number of colors in the colormap
  int GetColorMapSize () { return palette_size; }
  /// Get original colormap (packed)
  void GetOriginalColormap (RGBPixel *oPalette, int &oCount);

  /// Query palette -> native format table
  void *GetPaletteToGlobal () { return pal2glob; }

  /// Query palette -> 16-bit values table for 8-bit modes
  uint16 *GetPaletteToGlobal8 () { return pal2glob8; }

  /// Apply gamma correction to private palette
  void ApplyGamma (UByte *GammaTable);

  /// Used in 8bit when more than one procedural texture 
  void MapToGlobalPalette (RGBPixel *pal_8bit);

  /// Return the interfaces to the procedural texture buffer
  virtual iGraphics3D *GetProcTextureInterface ();

  /// Called to update pal2glob with palette information created elsewhere
  virtual void ProcTextureSync ();

  /// Called when the procedural texture shares texture manager with parent 
  /// context and in either 16 or 32bit. The 8bit version doesn't require 
  /// repreparing.
  void RePrepareProcTexture ();

  /**
   * Query if the texture has an alpha channel.<p>
   * This depends both on whenever the original image had an alpha channel
   * and of the fact whenever the renderer supports alpha maps at all.
   */
  virtual bool GetAlphaMap ()
  { return !!((csTextureSoftware *)get_texture (0))->get_alphamap (); }
};


/**
 * Software version of the texture manager. This instance of the
 * texture manager is probably the most involved of all 3D rasterizer
 * specific texture manager implementations because it needs to do
 * a lot of work regarding palette management and the creation
 * of lots of lookup tables.
 */
class csTextureManagerSoftware : public csTextureManager
{
private:
  /// Structure to help manage multiple 8bit canvases
  struct canvas8bit
  {
    canvas8bit (iGraphics2D *new_canvas)
      : g2d(new_canvas), next(NULL) {};
    canvas8bit (iGraphics2D *new_canvas, canvas8bit *head_canvas)
      : g2d(new_canvas), next(head_canvas) {};

    iGraphics2D *g2d;
    canvas8bit *next;
  };
  /// Head of a linked list of 8bit canvases sharing this texture manager.
  canvas8bit *mG2D;
  /// How strong texture manager should push 128 colors towards 
  /// a uniform palette
  int uniform_bias;

  /// Which colors are locked in the global colormap
  bool locked [256];

  /// true if palette has already been computed
  bool palette_ok;

  /// True if truecolor mode/false if paletted mode
  bool truecolor;

  /// Configuration values for color matching.
  int prefered_dist;

  /// We need a pointer to the 2D driver
  iGraphics2D *G2D;

public:

  /// We need a pointer to the 3D driver
  csGraphics3DSoftwareCommon *G3D;

  csSoftProcTexture3D *alone_proc_tex;
  /// Apply dithering to textures while reducing from 24-bit to 8-bit paletted?
  bool dither_textures;

  /// The global colormap (used in 256-color modes)
  csColorMap cmap;

  /// Lookup table (8-bit modes)
  csAlphaTables *alpha_tables;

  /// The multiplication tables used for lightmapping
  UByte *lightmap_tables [3];

  /// The inverse colormap (for 8-bit modes)
  UByte *inv_cmap;

  /// The translation table for applying gamma
  UByte GammaTable [256];

  /// Texture gamma
  float Gamma;

  ///
  csTextureManagerSoftware (iSystem *iSys, csGraphics3DSoftwareCommon *iG3D, 
			    csIniFile *config);
  ///
  virtual ~csTextureManagerSoftware ();

  /// Called from G3D::Open ()
  void SetPixelFormat (csPixelFormat &PixelFormat);

  /// Encode RGB values to a 16-bit word (for 16-bit mode).
  ULong encode_rgb (int r, int g, int b);

  /**
   * Create the inverse colormap. The forward colormap
   * must be created before this can be used.
   */
  void create_inv_cmap ();

  /// Create the alpha tables.
  void create_alpha_tables ();

  /// Find an rgb value using the faster lookup tables.
  int find_rgb (int r, int g, int b);

  /**
   * Compute the 'best' palette for all loaded textures.
   * This function will exactly behave the same in 16-bit mode
   * since we still need the common 256-color palette (although
   * it will not be installed on the display).
   */
  void compute_palette ();

  /// Set gamma correction value.
  void SetGamma (float iGamma);

  /// Maintain a linked list of 8bit canvases for the procedural textures
  void Register8BitCanvas (iGraphics2D *g2d8bit);
  void UnRegister8BitCanvas (iGraphics2D *g2d8bit);

  /// Read configuration values from config file.
  virtual void read_config (csIniFile *config);

  ///
  virtual void Clear ();

  /**
   * Return the index for some color. This works in 8-bit
   * (returns an index in the 256-color table), in 15/16-bit
   * (returns a 15/16-bit encoded RGB value) and in 32-bit
   * modes as well.
   */
  virtual int FindRGB (int r, int g, int b);

  ///
  virtual void PrepareTextures ();
  ///
  virtual iTextureHandle *RegisterTexture (iImage* image, int flags);
  ///
  virtual void PrepareTexture (iTextureHandle *handle);
  ///
  virtual void UnregisterTexture (iTextureHandle* handle);
  /// Clear the palette (including all reserved colors)
  virtual void ResetPalette ();
  /// Reserve a color in palette (if any)
  virtual void ReserveColor (int r, int g, int b);
  /// Really allocate the palette on the system.
  virtual void SetPalette ();
};

#endif // __SOFT_TXT_H__
