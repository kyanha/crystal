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

#include <math.h>
#include <stdarg.h>

#include "cssysdef.h"
#include "null_txt.h"
#include "csgfxldr/inv_cmap.h"
#include "csgfxldr/quantize.h"
#include "csutil/scanstr.h"
#include "csutil/inifile.h"
#include "isystem.h"
#include "iimage.h"

#define SysPrintf System->Printf

#define RESERVED_COLOR(c) ((c == 0) || (c == 255))

#define CLIP_RGB \
  if (r < 0) r = 0; else if (r > 255) r = 255; \
  if (g < 0) g = 0; else if (g > 255) g = 255; \
  if (b < 0) b = 0; else if (b > 255) b = 255;

/**
 * A nice observation about the properties of human eye:
 * Let's call the largest R or G or B component of a color "main".
 * If some other color component is much smaller than the main component,
 * we can change it in a large range without noting any change in
 * the color itself. Examples:
 * (128, 128, 128) - we note a change in color if we change any component
 * by 4 or more.
 * (192, 128, 128) - we should change of G or B components by 8 to note any
 * change in color.
 * (255, 128, 128) - we should change of G or B components by 16 to note any
 * change in color.
 * (255, 0, 0) - we can change any of G or B components by 32 and we
 * won't note any change.
 * Thus, we use this observation to create a palette that contains more
 * useful colors. We implement here a function to evaluate the "distance"
 * between two colors. tR,tG,tB define the color we are looking for (target);
 * sR, sG, sB define the color we're examining (source).
 */
static inline int rgb_dist (int tR, int tG, int tB, int sR, int sG, int sB)
{
  register int max = MAX (tR, tG);
  max = MAX (max, tB);

  sR -= tR; sG -= tG; sB -= tB;

  return R_COEF_SQ * sR * sR * (32 - ((max - tR) >> 3)) +
         G_COEF_SQ * sG * sG * (32 - ((max - tG) >> 3)) +
         B_COEF_SQ * sB * sB * (32 - ((max - tB) >> 3));
}

//------------------------------------------------------------- csColorMapLine ---//

int csColorMapLine::find_rgb (int r, int g, int b, int *d)
{
  CLIP_RGB;

  int i, min, mindist;
  mindist = 0x7fffffff;
  min = -1;
  // Color 0 is reserved for transparency
  for (i = 1; i < 256; i++)
    if (alloc [i])
    {
      register int dist = rgb_dist (r, g, b,
        palette [i].red, palette [i].green, palette [i].blue);
      if (dist < mindist) { mindist = dist; min = i; if (!dist) break; }
    }
  if (d) *d = mindist;
  return min;
}

int csColorMapLine::alloc_rgb (int r, int g, int b, int dist)
{
  CLIP_RGB;

  int d, i = find_rgb (r, g, b, &d);
  if (i == -1 || d > dist)
  {
    for (int j = 0; j < 256; j++)
      if (!alloc [j])
      {
        alloc[j] = true;
        palette [j].red = r;
        palette [j].green = g;
        palette [j].blue = b;
        return j;
      }
    return i; // We couldn't allocate a new color, return best fit
  }
  else
    return i;
}

int csColorMapLine::FreeEntries ()
{
  int colors = 0;
  for (int i = 0; i < 256; i++)
    if (!alloc [i])
      colors++;
  return colors;
}

//-------------------------------------------------------- csTextureMMNull ---//

csTextureMMNull::csTextureMMNull (iImage *image, int flags) :
  csTextureMM (image, flags)
{
  pal2glob = NULL;
}

csTextureMMNull::~csTextureMMNull ()
{
}

csTexture *csTextureMMNull::NewTexture (iImage *Image)
{
  return new csTextureNull (this, Image);
}

void csTextureMMNull::ComputeMeanColor ()
{
  int i;

  // Compute a common palette for all three mipmaps
  csQuantizeBegin ();

  RGBPixel *tc = transp ? &transp_color : NULL;

  for (i = 0; i < 4; i++)
    if (tex [i])
    {
      csTextureNull *t = (csTextureNull *)tex [i];
      if (!t->image) break;
      csQuantizeCount ((RGBPixel *)t->image->GetImageData (),
        t->get_size (), tc);
    }

  RGBPixel *pal = palette;
  palette_size = 256;
  csQuantizePalette (pal, palette_size, tc);

  for (i = 0; i < 4; i++)
    if (tex [i])
    {
      csTextureNull *t = (csTextureNull *)tex [i];
      if (!t->image) break;

      csQuantizeRemap ((RGBPixel *)t->image->GetImageData (),
        t->get_size (), t->bitmap, tc);

      // Very well. Now we don'tex need the iImage anymore, so free it
      t->image->DecRef ();
      t->image = NULL;
    }

  csQuantizeEnd ();

  // Compute the mean color from the palette
  RGBPixel *src = palette;
  unsigned r = 0, g = 0, b = 0;
  for (i = 0; i < palette_size; i++)
  {
    RGBPixel pix = *src++;
    r += pix.red;
    g += pix.green;
    b += pix.blue;
  }
  mean_color.red   = r / palette_size;
  mean_color.green = g / palette_size;
  mean_color.blue  = b / palette_size;
}

void csTextureMMNull::remap_texture (csTextureManager *texman)
{
  int i;
  csTextureManagerLine *txm = (csTextureManagerLine *)texman;
  switch (texman->pfmt.PixelBytes)
  {
    case 1:
      delete [] (UByte *)pal2glob;
      pal2glob = new UByte [palette_size];
      for (i = 0; i < palette_size; i++)
        ((UByte *)pal2glob) [i] = txm->cmap.find_rgb (palette [i].red,
          palette [i].green, palette [i].blue);
      break;
    case 2:
      delete [] (UShort *)pal2glob;
      pal2glob = new UShort [palette_size];
      for (i = 0; i < palette_size; i++)
        ((UShort *)pal2glob) [i] = txm->encode_rgb (palette [i].red,
          palette [i].green, palette [i].blue);
      break;
    case 4:
      delete [] (ULong *)pal2glob;
      pal2glob = new ULong [palette_size];
      for (i = 0; i < palette_size; i++)
        ((ULong *)pal2glob) [i] = txm->encode_rgb (palette [i].red,
          palette [i].green, palette [i].blue);
      break;
  }
}

//----------------------------------------------- csTextureManagerLine ---//

csTextureManagerLine::csTextureManagerLine (iSystem *iSys,
  iGraphics2D *iG2D, csIniFile *config) : csTextureManager (iSys, iG2D)
{
  ResetPalette ();
  read_config (config);
  G2D = iG2D;
}

void csTextureManagerLine::SetPixelFormat (csPixelFormat &PixelFormat)
{
  pfmt = PixelFormat;

  truecolor = (pfmt.PalEntries == 0);
}

void csTextureManagerLine::read_config (csIniFile *config)
{
  csTextureManager::read_config (config);
  prefered_dist = config->GetInt ("TextureManager", "RGB_DIST", PREFERED_DIST);
  uniform_bias = config->GetInt ("TextureManager", "UNIFORM_BIAS", 75);
  if (uniform_bias > 100) uniform_bias = 100;
}

csTextureManagerLine::~csTextureManagerLine ()
{
  Clear ();
}

void csTextureManagerLine::Clear ()
{
  csTextureManager::Clear ();
}

ULong csTextureManagerLine::encode_rgb (int r, int g, int b)
{
  return
    ((r >> (8 - pfmt.RedBits))   << pfmt.RedShift) |
    ((g >> (8 - pfmt.GreenBits)) << pfmt.GreenShift) |
    ((b >> (8 - pfmt.BlueBits))  << pfmt.BlueShift);
}

int csTextureManagerLine::find_rgb (int r, int g, int b)
{
  CLIP_RGB;
  return inv_cmap [encode_rgb (r, g, b)];
}

int csTextureManagerLine::FindRGB (int r, int g, int b)
{
  CLIP_RGB;
  if (truecolor)
    return encode_rgb (r, g, b);
  else
    return inv_cmap [encode_rgb (r, g, b)];
}

void csTextureManagerLine::create_inv_cmap ()
{
  // We create a inverse colormap for finding fast the nearest palette index
  // given any R,G,B value. Usually this is done by scanning the entire palette
  // which is way too slow for realtime. Because of this we use an table that
  // that takes on input an 5:6:5 encoded value (like in 16-bit truecolor modes)
  // and on output we get the palette index.

  if (pfmt.PixelBytes != 1)
    return;

  if (verbose)
    SysPrintf (MSG_INITIALIZATION, "  Computing inverse colormap...\n");

  // Greg Ewing, 12 Oct 1998
  delete inv_cmap;
  inv_cmap = NULL; // let the routine allocate the array itself
  csInverseColormap (256, &cmap [0], RGB2PAL_BITS_R, RGB2PAL_BITS_G,
    RGB2PAL_BITS_B, inv_cmap);

  // Color number 0 is reserved for transparency
  inv_cmap [encode_rgb (cmap [0].red, cmap [0].green, cmap [0].blue)] =
    cmap.find_rgb (cmap [0].red, cmap [0].green, cmap [0].blue);
}

void csTextureManagerLine::compute_palette ()
{
  if (truecolor) return;

  if (verbose) SysPrintf (MSG_INITIALIZATION, "  Computing palette...\n");

  // Allocate first 6*6*4=144 colors in a uniformly-distributed fashion
  // since we'll get lighted/dimmed/colored textures more often
  // than original pixel values, thus we should be prepared for this.
  for (int _r = 0; _r < 6; _r++)
    for (int _g = 0; _g < 6; _g++)
      for (int _b = 0; _b < 4; _b++)
        cmap.alloc_rgb (20 + _r * 42, 20 + _g * 42, 30 + _b * 50, prefered_dist);

  // Compute a common color histogram for all textures
  csQuantizeBegin ();

  for (int t = textures.Length () - 1; t >= 0; t--)
  {
    csTextureMMNull *txt = (csTextureMMNull *)textures [t];
    RGBPixel *colormap = txt->GetColorMap ();
    int colormapsize = txt->GetColorMapSize ();
    if (txt->GetTransparent ())
      colormap++, colormapsize--;
    csQuantizeCount (colormap, colormapsize);
  }

  // Introduce the uniform colormap bias into the histogram
  RGBPixel new_cmap [256];
  int ci, colors = 0;
  for (ci = 0; ci < 256; ci++)
    if (!locked [ci] && cmap.alloc [ci])
      new_cmap [colors++] = cmap [ci];

  csQuantizeBias (new_cmap, colors, uniform_bias);

  // Now compute the actual colormap
  colors = 0;
  for (ci = 0; ci < 256; ci++)
    if (!locked [ci]) colors++;
  RGBPixel *cmap_p = new_cmap;
  csQuantizePalette (cmap_p, colors);

  // Finally, put the computed colors back into the colormap
  int outci = 0;
  for (ci = 0; ci < colors; ci++)
  {
    while (locked [outci]) outci++;
    cmap [outci++] = new_cmap [ci];
  }

  csQuantizeEnd ();

  // Now create the inverse colormap
  create_inv_cmap ();

  palette_ok = true;
}

void csTextureManagerLine::PrepareTextures ()
{
  if (verbose) SysPrintf (MSG_INITIALIZATION, "Preparing textures...\n");

  // Drop all "color allocated" flags to locked colors.
  // We won't Clear the palette as we don't care about unused colors anyway.
  // The locked colors will stay the same.
  cmap.alloc = locked;

  if (verbose) SysPrintf (MSG_INITIALIZATION, "  Creating texture mipmaps...\n");

  // Create mipmaps for all textures
  int i;
  for (i = 0; i < textures.Length (); i++)
  {
    csTextureMM *txt = textures.Get (i);
    txt->CreateMipmaps ();
  }

  // The only thing left to do is to compute the palette
  // Everything other has been done during textures registration
  compute_palette ();

  // Remap all textures according to the new colormap.
  for (i = 0; i < textures.Length (); i++)
    ((csTextureMMNull*)textures[i])->remap_texture (this);
}

iTextureHandle *csTextureManagerLine::RegisterTexture (iImage* image,
  int flags)
{
  if (!image) return NULL;

  csTextureMMNull *txt = new csTextureMMNull (image, flags);
  textures.Push (txt);
  return txt;
}

void csTextureManagerLine::PrepareTexture (iTextureHandle *handle)
{
  if (!handle) return;

  csTextureMMNull *txt = (csTextureMMNull *)handle->GetPrivateObject ();
  txt->CreateMipmaps ();
  txt->remap_texture (this);
}

void csTextureManagerLine::UnregisterTexture (iTextureHandle* handle)
{
  csTextureMMNull *tex_mm = (csTextureMMNull *)handle->GetPrivateObject ();
  int idx = textures.Find (tex_mm);
  if (idx >= 0) textures.Delete (idx);
}

void csTextureManagerLine::ResetPalette ()
{
  memset (&locked, 0, sizeof (locked));
  locked [0] = true;
  locked [255] = true;
  cmap [0] = RGBcolor (0, 0, 0);
  cmap [255] = RGBcolor (255, 255, 255);
  cmap.alloc = locked;
  palette_ok = false;
}

void csTextureManagerLine::ReserveColor (int r, int g, int b)
{
  if (!pfmt.PalEntries) return;
  int color = cmap.alloc_rgb (r, g, b, 0);
  locked [color] = true;
}

void csTextureManagerLine::SetPalette ()
{
  if (!truecolor && !palette_ok)
    compute_palette ();

  for (int i = 0; i < 256; i++)
    G2D->SetRGB (i, cmap [i].red, cmap [i].green, cmap [i].blue);
}
