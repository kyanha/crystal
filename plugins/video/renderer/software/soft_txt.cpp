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
#include "soft_txt.h"
#include "csgfxldr/inv_cmap.h"
#include "csgfxldr/quantize.h"
#include "csutil/scanstr.h"
#include "csutil/inifile.h"
#include "isystem.h"
#include "iimage.h"
#include "qint.h"
#include "protex3d.h"

#define SysPrintf System->Printf

#define RESERVED_COLOR(c) ((c == 0) || (c == 255))

#define CLIP_RGB \
  if (r < 0) r = 0; else if (r > 255) r = 255; \
  if (g < 0) g = 0; else if (g > 255) g = 255; \
  if (b < 0) b = 0; else if (b > 255) b = 255;

/**
 * A nice observation about the properties of the human eye:
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

//------------------------------------------------------------- csColorMap ---//

int csColorMap::find_rgb (int r, int g, int b, int *d)
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

int csColorMap::alloc_rgb (int r, int g, int b, int dist)
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

int csColorMap::FreeEntries ()
{
  int colors = 0;
  for (int i = 0; i < 256; i++)
    if (!alloc [i])
      colors++;
  return colors;
}

//---------------------------------------------------- csTextureSoftProc -----//

csTextureSoftwareProc::~csTextureSoftwareProc ()
{ 
  if (texG3D) texG3D->DecRef ();
  if (image) image->DecRef ();
}

//---------------------------------------------------- csTextureMMSoftware ---//

csTextureMMSoftware::csTextureMMSoftware (csTextureManagerSoftware *texman, 
  iImage *image, int flags) : csTextureMM (image, flags)
{
  pal2glob = NULL;
  pal2glob8 = NULL;
  if (flags & CS_TEXTURE_3D)
    AdjustSizePo2 ();
  orig_palette = NULL;
  this->texman = texman;
}

csTextureMMSoftware::~csTextureMMSoftware ()
{
  delete [] (UByte *)pal2glob;
  delete [] pal2glob8;
  delete [] orig_palette;
}

csTexture *csTextureMMSoftware::NewTexture (iImage *Image)
{
  if ((flags & CS_TEXTURE_PROC) == CS_TEXTURE_PROC)

    return new csTextureSoftwareProc (this, Image);
  else
    return new csTextureSoftware (this, Image);
}

void csTextureMMSoftware::ApplyGamma (UByte *GammaTable)
{
  if (!orig_palette)
    return;

  UByte *src = orig_palette;
  RGBPixel *dst = palette;
  for (int i = palette_size; i > 0; i--)
  {
    dst->red   = GammaTable [*src++];
    dst->green = GammaTable [*src++];
    dst->blue  = GammaTable [*src++];
    dst++;
  }
} 

void csTextureMMSoftware::ComputeMeanColor ()
{
  int i;

  // If the procedural textures are not running fully at 8bit and so do not have 
  // their own texture manager the procedural textures update the iImage, so we 
  // avoid destroying them here.
  bool destroy_image = ((flags & CS_TEXTURE_PROC) != CS_TEXTURE_PROC);

  // Compute a common palette for all three mipmaps
  csQuantizeBegin ();

  RGBPixel *tc = transp ? &transp_color : NULL;

  for (i = 0; i < 4; i++)
    if (tex [i])
    {
      csTextureSoftware *t = (csTextureSoftware *)tex [i];
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
      csTextureSoftware *t = (csTextureSoftware *)tex [i];
      if (!t->image) break;
      if (texman->dither_textures || (flags & CS_TEXTURE_DITHER))
        csQuantizeRemapDither ((RGBPixel *)t->image->GetImageData (),
          t->get_size (), t->get_width (), pal, palette_size, t->bitmap, tc);
      else
        csQuantizeRemap ((RGBPixel *)t->image->GetImageData (),
          t->get_size (), t->bitmap, tc);

      // Get the alpha map for the texture, if present
      if (t->image->GetFormat () & CS_IMGFMT_ALPHA)
      {
        RGBPixel *srcimg = (RGBPixel *)t->image->GetImageData ();
        size_t imgsize = t->get_size ();
        uint8 *dstalpha = t->alphamap = new uint8 [imgsize];
        // In 8- and 16-bit modes we limit the alpha to 5 bits (32 values)
        // This is related to the internal implementation of alphamap
        // routine and is quite enough for 5-5-5 and 5-6-5 modes.
        if (texman->pfmt.PixelBytes != 4)
          while (imgsize--)
            *dstalpha++ = srcimg++->alpha >> 3;
        else
          while (imgsize--)
            *dstalpha++ = srcimg++->alpha;
      }

      if (destroy_image)
      {
	// Very well, we don't need the iImage anymore, so free it
	t->image->DecRef ();
	t->image = NULL;
      }
    }

  csQuantizeEnd ();

  SetupFromPalette ();
}

void csTextureMMSoftware::SetupFromPalette ()
{
  int i;
  // Compute the mean color from the palette
  RGBPixel *src = palette;
  unsigned r = 0, g = 0, b = 0;
  for (i = palette_size; i > 0; i--)
  {
    RGBPixel pix = *src++;
    r += pix.red;
    g += pix.green;
    b += pix.blue;
  }
  mean_color.red   = r / palette_size;
  mean_color.green = g / palette_size;
  mean_color.blue  = b / palette_size;

  // Now allocate the orig palette and compress the texture palette there
  src = palette;
  delete [] orig_palette;
  orig_palette = new UByte [palette_size * 3];
  UByte *dst = orig_palette;
  for (i = palette_size; i > 0; i--)
  {
    *dst++ = src->red;
    *dst++ = src->green;
    *dst++ = src->blue;
    src++;
  }
}

void csTextureMMSoftware::GetOriginalColormap (RGBPixel *oPalette, int &oCount)
{
  oCount = palette_size;
  UByte *src = orig_palette;
  RGBPixel *dst = oPalette;
  for (int i = palette_size; i > 0; i--)
  {
    dst->red   = *src++;
    dst->green = *src++;
    dst->blue  = *src++;
    dst++;
  }
}

void csTextureMMSoftware::remap_texture ()
{
  int i;
  ApplyGamma (texman->GammaTable);
  switch (texman->pfmt.PixelBytes)
  {
    case 1:
      delete [] (UByte *)pal2glob;
      pal2glob = new UByte [palette_size * sizeof (UByte)];
      delete [] pal2glob8;
      pal2glob8 = new uint16 [palette_size];
      for (i = 0; i < palette_size; i++)
      {
        ((UByte *)pal2glob) [i] = texman->cmap.find_rgb (palette [i].red,
          palette [i].green, palette [i].blue);
        pal2glob8 [i] = texman->encode_rgb (palette [i].red,
          palette [i].green, palette [i].blue);
      }
      break;
    case 2:
      delete [] (UShort *)pal2glob;
      pal2glob = new UByte [palette_size * sizeof (UShort)];
      for (i = 0; i < palette_size; i++)
        ((UShort *)pal2glob) [i] = texman->encode_rgb (palette [i].red,
          palette [i].green, palette [i].blue);
      break;
    case 4:
      delete [] (ULong *)pal2glob;
      pal2glob = new UByte [palette_size * sizeof (ULong)];
      for (i = 0; i < palette_size; i++)
        ((ULong *)pal2glob) [i] = texman->encode_rgb (palette [i].red,
          palette [i].green, palette [i].blue);
      break;
  }
}

void csTextureMMSoftware::RemapProcToGlobalPalette (csTextureManagerSoftware *txtmgr)
{
  csTextureSoftwareProc* t = (csTextureSoftwareProc*)tex[0];
  if (!txtmgr->inv_cmap || t->proc_ok)
    return;

  // This texture manager is fully 8bit and is either managing all the textures in
  // the system or just a set of proc textures.
  RGBPixel *src = (RGBPixel *)t->image->GetImageData ();
  uint8 *dst = t->bitmap;
  // Copy the global palette to the texture palette
  memcpy (palette, &txtmgr->cmap.palette[0], sizeof(RGBPixel)*256);
  // Remap image according to new palette if persistent
  if ((flags & CS_TEXTURE_PROC_PERSISTENT) == CS_TEXTURE_PROC_PERSISTENT) 
    for (int i = 0; i < t->get_size (); i++, dst++, src++)
      *dst = txtmgr->inv_cmap[txtmgr->encode_rgb(src->red, src->green, src->blue)];
  // we have really finished with the image now
  t->image->DecRef ();
  t->image = NULL;

  // If a proc texture was initialised without an image the palette size will be
  // very small, therefore force it to the max, so the whole new palette is 
  // recognised by at least DrawPixmap
  palette_size = 256;
  SetupFromPalette ();
  // Update tables with global palette
  remap_texture ();
  t->proc_ok = true;
}

void csTextureMMSoftware::ProcTextureSync ()
{

}

void csTextureMMSoftware::ReprepareProcTexture ()
{
  ComputeMeanColor ();
  remap_texture ();
}

iGraphics3D *csTextureMMSoftware::GetProcTextureInterface ()
{
  if ((flags & CS_TEXTURE_PROC) != CS_TEXTURE_PROC)
    return NULL;

  if (!((csTextureSoftwareProc*)tex[0])->texG3D)
  {
    bool success = false;
    bool alone_hint = (flags & CS_TEXTURE_PROC_ALONE_HINT) == 
						CS_TEXTURE_PROC_ALONE_HINT;

    csSoftProcTexture3D *stex = new csSoftProcTexture3D (NULL);

    // stipulate max palette size
    if (texman->pfmt.PixelBytes == 1)
    {
      // We are at 8bit display depth so we can share the main texture manager
      // as well as render fully at 8bit, thereby also avoiding recalculation of 
      // palettes :)
      // NB: the alone_hint has no effect in fully 8bit
      success = stex->Prepare (this, texman->G3D, NULL, NULL,
                               ((csTextureSoftware*) tex[0])->bitmap, 
			       &texman->pfmt, &texman->cmap.palette[0], false);
    }
    else if (alone_hint)
    {
      // We can work internally at 8bit, but there maybe proc textures in the
      // system running at 16/32bit also. :)
      // This palette is now managed by the fully 8bit texture manager. 
      success = stex->Prepare (this, texman->G3D, texman->first_8bit_proc_tex, 
			     ((csTextureSoftware*) tex[0])->image->GetImageData (),
			     ((csTextureSoftware*) tex[0])->bitmap, 
			      &texman->pfmt, palette, alone_hint);
      if (!texman->first_8bit_proc_tex)
      {
	texman->first_8bit_proc_tex = stex;
	stex->IncRef ();
      }
    }
    else
      // We work at display depth i.e. 16/32bit, this requires  frame buffer 
      // conversions and palette recalculations each update :(
      success = stex->Prepare  (this, texman->G3D, NULL, 
                            ((csTextureSoftware*) tex[0])->image->GetImageData (), 
		             NULL, &texman->pfmt, palette, alone_hint);

    if (!success)
      delete stex;
    else
      ((csTextureSoftwareProc*)tex[0])->texG3D = stex;
  }
  return ((csTextureSoftwareProc*)tex[0])->texG3D;
}

//----------------------------------------------- csTextureManagerSoftware ---//

static UByte *GenLightmapTable (int bits)
{
  UByte *table = new UByte [64 * 256];
  UByte *dst = table;
  UByte maxv = (1 << bits) - 1;
  int rshf = (13 - bits);
  for (int i = 0; i < 64; i++)
    for (int j = 0; j < 256; j++)
    {
      int x = (i * j) >> rshf;
      *dst++ = (x > maxv) ? maxv : x;
    }
  return table;
}

csTextureManagerSoftware::csTextureManagerSoftware (iSystem *iSys,
  csGraphics3DSoftwareCommon *iG3D, csIniFile *config) 
  : csTextureManager (iSys, iG3D->GetDriver2D())
{
  alpha_tables = NULL;
  ResetPalette ();
  read_config (config);
  G3D = iG3D;
  inv_cmap = NULL;
  Scan.GlobalCMap = NULL;

  first_8bit_proc_tex = NULL;
  main_txtmgr = NULL;
  proc_txtmgr = NULL;
}

void csTextureManagerSoftware::SetGamma (float iGamma)
{
  Gamma = iGamma;
  if (Gamma < EPSILON)
    Gamma = EPSILON;

  int i;
  float inv_Gamma = 1 / Gamma;
  for (i = 0; i < 256; i++)
    GammaTable [i] = QRound (255 * pow (i / 255.0, inv_Gamma));
  // Remap all textures according to the new colormap.
  if (truecolor)
    for (i = 0; i < textures.Length (); i++)
      ((csTextureMMSoftware*)textures.Get (i))->remap_texture ();
  else if (textures.Length ())
    SetPalette ();
}

void csTextureManagerSoftware::SetPixelFormat (csPixelFormat &PixelFormat)
{
  pfmt = PixelFormat;

  truecolor = (pfmt.PalEntries == 0);

  // Create multiplication tables
  lightmap_tables [0] = GenLightmapTable (pfmt.RedBits);

  if (pfmt.GreenBits == pfmt.RedBits)
    lightmap_tables [1] = lightmap_tables [0];
  else
    lightmap_tables [1] = GenLightmapTable (pfmt.GreenBits);

  if (pfmt.BlueBits == pfmt.RedBits)
    lightmap_tables [2] = lightmap_tables [0];
  else if (pfmt.BlueBits == pfmt.GreenBits)
    lightmap_tables [2] = lightmap_tables [1];
  else
    lightmap_tables [2] = GenLightmapTable (pfmt.BlueBits);

  if (pfmt.PixelBytes == 1)
    Scan.GlobalCMap = new uint16 [256];
}

void csTextureManagerSoftware::read_config (csIniFile *config)
{
  csTextureManager::read_config (config);
  prefered_dist = config->GetInt ("TextureManager", "RGB_DIST", PREFERED_DIST);
  uniform_bias = config->GetInt ("TextureManager", "UNIFORM_BIAS", 75);
  if (uniform_bias > 100) uniform_bias = 100;
  dither_textures = config->GetYesNo ("TextureManager", "DITHER_TEXTURES", true);
}

csTextureManagerSoftware::~csTextureManagerSoftware ()
{
  if (first_8bit_proc_tex) 
    first_8bit_proc_tex->DecRef ();

  delete [] inv_cmap;
  delete [] lightmap_tables [0];
  if (lightmap_tables [1] != lightmap_tables [0])
    delete [] lightmap_tables [1];
  if (lightmap_tables [2] != lightmap_tables [1]
   && lightmap_tables [2] != lightmap_tables [0])
    delete [] lightmap_tables [2];
  Clear ();
}

void csTextureManagerSoftware::Clear ()
{
  csTextureManager::Clear ();
  delete alpha_tables; alpha_tables = NULL;
}

ULong csTextureManagerSoftware::encode_rgb (int r, int g, int b)
{
  return
    ((r >> (8 - pfmt.RedBits))   << pfmt.RedShift) |
    ((g >> (8 - pfmt.GreenBits)) << pfmt.GreenShift) |
    ((b >> (8 - pfmt.BlueBits))  << pfmt.BlueShift);
}

int csTextureManagerSoftware::find_rgb (int r, int g, int b)
{
  CLIP_RGB;
  return inv_cmap [encode_rgb (r, g, b)];
}

int csTextureManagerSoftware::FindRGB (int r, int g, int b)
{
  CLIP_RGB;
  if (truecolor)
    return encode_rgb (r, g, b);
  else
    return inv_cmap [encode_rgb (r, g, b)];
}

void csTextureManagerSoftware::create_inv_cmap ()
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
  delete [] inv_cmap;
  inv_cmap = NULL; // let the routine allocate the array itself
  csInverseColormap (256, &cmap [0], RGB2PAL_BITS_R, RGB2PAL_BITS_G,
    RGB2PAL_BITS_B, inv_cmap);

  // Color number 0 is reserved for transparency
  inv_cmap [encode_rgb (cmap [0].red, cmap [0].green, cmap [0].blue)] =
    cmap.find_rgb (cmap [0].red, cmap [0].green, cmap [0].blue);

  // Now we'll encode the palette into 16-bit values
  for (int i = 0; i < 256; i++)
    Scan.GlobalCMap [i] = encode_rgb (cmap [i].red, cmap [i].green, cmap [i].blue);
}

void csTextureManagerSoftware::create_alpha_tables ()
{
  if (pfmt.PixelBytes != 1)
    return;

  if (verbose)
    SysPrintf (MSG_INITIALIZATION, "  Computing alpha tables...\n");

  if (!alpha_tables)
    alpha_tables = new csAlphaTables ();

  UByte *map50 = alpha_tables->alpha_map50;
  UByte *map25 = alpha_tables->alpha_map25;

  for (int i = 0 ; i < 256 ; i++)
    for (int j = 0 ; j < 256 ; j++)
    {
      int r, g, b;
      r = (cmap [i].red   + cmap [j].red  ) / 2;
      g = (cmap [i].green + cmap [j].green) / 2;
      b = (cmap [i].blue  + cmap [j].blue ) / 2;
      *map50++ = find_rgb (r, g, b);

      r = (cmap [i].red   + cmap [j].red   * 3) / 4;
      g = (cmap [i].green + cmap [j].green * 3) / 4;
      b = (cmap [i].blue  + cmap [j].blue  * 3) / 4;
      *map25++ = find_rgb (r, g, b);
    }
}

void csTextureManagerSoftware::compute_palette ()
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
    csTextureMMSoftware *txt = (csTextureMMSoftware *)textures [t];
    if ((txt->GetFlags() & CS_TEXTURE_PROC) == CS_TEXTURE_PROC)
      continue;
    RGBPixel colormap [256];
    int colormapsize;
    txt->GetOriginalColormap (colormap, colormapsize);
    RGBPixel *cmap = colormap;
    if (txt->GetKeyColor ())
      cmap++, colormapsize--;
    csQuantizeCount (cmap, colormapsize);
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

  // Also we need the alpha tables
  create_alpha_tables ();

  palette_ok = true;
}

void csTextureManagerSoftware::PrepareTextures ()
{
  if (verbose)
    SysPrintf (MSG_INITIALIZATION, "Preparing textures (%s dithering)...\n",
      dither_textures ? "with" : "no");

  // Drop all "color allocated" flags to locked colors.
  // We won't clear the palette as we don't care about unused colors anyway.
  // The locked colors will stay the same.
  memcpy (cmap.alloc, locked, sizeof(locked));

  if (verbose) SysPrintf (MSG_INITIALIZATION, "  Creating texture mipmaps...\n");

  int i;

  // Create mipmaps for all textures
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
  {
    csTextureMMSoftware* txt = (csTextureMMSoftware*)textures.Get (i);
    if ((pfmt.PixelBytes == 1) && 
       ((txt->GetFlags() & CS_TEXTURE_PROC) == CS_TEXTURE_PROC))
      ((csTextureMMSoftware*)txt)->RemapProcToGlobalPalette (this);
    else
      txt->remap_texture ();
  }

  // If we are the dedicated procedural texture manager in the system, we notify
  // the main texture manager to reprepare the procedural textures created with
  // the ALONE_HINT
  if (main_txtmgr)
    main_txtmgr->ReprepareAloneProcs ();

  delete [] Scan.GlobalCMap;
}

iTextureHandle *csTextureManagerSoftware::RegisterTexture (iImage* image,
  int flags)
{
  if (!image) return NULL;
  csTextureMMSoftware *txt = new csTextureMMSoftware (this, image, flags);
  textures.Push (txt);
  return txt;
}

void csTextureManagerSoftware::PrepareTexture (iTextureHandle *handle)
{
  if (!handle) return;

  csTextureMMSoftware *txt = (csTextureMMSoftware *)handle->GetPrivateObject ();
  txt->CreateMipmaps ();
  if ((pfmt.PixelBytes == 1) && 
      ((txt->GetFlags() & CS_TEXTURE_PROC) == CS_TEXTURE_PROC))
    ((csTextureMMSoftware*)txt)->RemapProcToGlobalPalette (this);
  else
    txt->remap_texture ();

  if (main_txtmgr)
    main_txtmgr->ReprepareAloneProcs ();
}

void csTextureManagerSoftware::UnregisterTexture (iTextureHandle* handle)
{
  csTextureMMSoftware *tex_mm = (csTextureMMSoftware *)handle->GetPrivateObject ();
  int idx = textures.Find (tex_mm);
  if (idx >= 0) textures.Delete (idx);
}

void csTextureManagerSoftware::ResetPalette ()
{
  memset (&locked, 0, sizeof (locked));
  locked [0] = true;
  locked [255] = true;
  cmap [0] = RGBcolor (0, 0, 0);
  cmap [255] = RGBcolor (255, 255, 255);
  memcpy (cmap.alloc, locked, sizeof(locked));
  palette_ok = false;
}

void csTextureManagerSoftware::ReserveColor (int r, int g, int b)
{
  if (!pfmt.PalEntries) return;
  int color = cmap.alloc_rgb (r, g, b, 0);
  locked [color] = true;
}

void csTextureManagerSoftware::SetPalette ()
{
  int i;
  if (truecolor)
    return;
  if (!palette_ok)
    compute_palette ();

  iGraphics2D *G2D = G3D->GetDriver2D ();
  for (i = 0; i < 256; i++)
  {
    int r = GammaTable [cmap [i].red];
    int g = GammaTable [cmap [i].green];
    int b = GammaTable [cmap [i].blue];
    G2D->SetRGB (i, r, g, b);
  }
//    if (proc_only_txtmgr)
//      ((csSoftProcTexture3D*)G3D)->SyncProcPalettes ();
}

void csTextureManagerSoftware::ReprepareAloneProcs ()
{
  // Remap all textures according to the new colormap.
  for (int i = 0; i < textures.Length (); i++)
  {
    csTextureMMSoftware* txt = (csTextureMMSoftware*)textures.Get (i);
    if ((txt->GetFlags() & (CS_TEXTURE_PROC | CS_TEXTURE_PROC_ALONE_HINT)) 
	== (CS_TEXTURE_PROC | CS_TEXTURE_PROC_ALONE_HINT))
      ((csTextureMMSoftware*)txt)->RemapProcToGlobalPalette (proc_txtmgr);
  }
}
