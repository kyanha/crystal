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
#include "csgfx/inv_cmap.h"
#include "csgfx/quantize.h"
#include "csutil/scanstr.h"
#include "csutil/debug.h"
#include "iutil/cfgfile.h"
#include "iutil/event.h"
#include "iutil/eventq.h"
#include "igraphic/image.h"
#include "ivaria/reporter.h"
#include "qint.h"
#include "csgfx/memimage.h"
#include "csgfx/xorpat.h"
#include "soft_g3d.h"

//----------------------------------------------- csSoftwareTextureHandle ---//

csSoftwareTextureHandle::csSoftwareTextureHandle (
	csSoftwareTextureManager *texman, iImage *image, int flags)
	: csTextureHandle (image, flags)
{
  pal2glob = 0;
  if (flags & CS_TEXTURE_3D)
    AdjustSizePo2 ();
  (this->texman = texman)->IncRef ();
  use_332_palette = false;
  update_number = ~0;
  is_palette_init = false;
}

csSoftwareTextureHandle::~csSoftwareTextureHandle ()
{
  texman->UnregisterTexture (this);
  texman->DecRef ();
  delete [] (uint8 *)pal2glob;
}

void csSoftwareTextureHandle::Setup332Palette ()
{
  if (use_332_palette) return;

  use_332_palette = true;
  // First remap the textures to use standard 3:3:2 palette.
  int i;
  for (i = 0 ; i < 4 ; i++)
  {
    if (tex [i])
    {
      csSoftwareTexture *t = (csSoftwareTexture *)tex [i];
      if (!t->bitmap) break;
      int size = t->get_width () * t->get_height ();
      uint8* bm = t->bitmap;
      while (size > 0)
      {
        const csRGBpixel& p = palette[*bm];
	*bm++ = ((p.red >> 5) << 5) |
	        ((p.green >> 5) << 2) |
	        (p.green >> 6);
        size--;
      }
    }
  }

  palette_size = 256;
  delete [] (uint8 *)pal2glob;

  // Remap the pal2glob array.
  if (texman->pfmt.PixelBytes == 2)
  {
    pal2glob = new uint8 [palette_size * sizeof (uint16)];
    uint16* p2g = (uint16*)pal2glob;
    for (i = 0 ; i < 256 ; i++)
    {
      int r = (i>>5) << 5;
      int g = ((i>>2) & 0x7) << 5;
      int b = (i & 0x3) << 6;
      *p2g++ = texman->encode_rgb (r, g, b);
    }
  }
  else
  {
    pal2glob = new uint8 [palette_size * sizeof (uint32)];
    uint32* p2g = (uint32*)pal2glob;
    for (i = 0 ; i < 256 ; i++)
    {
      int r = (i>>5) << 5;
      int g = ((i>>2) & 0x7) << 5;
      int b = (i & 0x3) << 6;
      *p2g++ = texman->encode_rgb (r, g, b);
    }
  }
  // Remap the palette itself.
  for (i = 0 ; i < 256 ; i++)
  {
    int r = (i>>5) << 5;
    int g = ((i>>2) & 0x7) << 5;
    int b = (i & 0x3) << 6;
    palette[i].red = r;
    palette[i].green = g;
    palette[i].blue = b;
  }
}

csTexture *csSoftwareTextureHandle::NewTexture (iImage *newImage,
	bool ismipmap)
{
  csRef<iImage> Image;
  if (ismipmap && texman->sharpen_mipmaps)
  { 
    csRGBpixel *tc = transp ? &transp_color : (csRGBpixel *)0;
    Image = newImage->Sharpen (tc, texman->sharpen_mipmaps);
  }
  else
    Image = newImage;

  return new csSoftwareTexture (this, Image);
}

void csSoftwareTextureHandle::ComputeMeanColor ()
{
  int i;

  bool destroy_image = true;

  // Compute a common palette for all three mipmaps
  csColorQuantizer quant;
  quant.Begin ();

  csRGBpixel *tc = transp ? &transp_color : 0;

  for (i = 0; i < 4; i++)
    if (tex [i])
    {
      csSoftwareTexture *t = (csSoftwareTexture *)tex [i];
      if (!t->image) break;
      quant.Count ((csRGBpixel *)t->image->GetImageData (),
        t->get_size (), tc);
    }
#if 0
  // This code makes sure the palette also has sufficient
  // information to encode a standard 3:3:2 palette.
  // This is useful when the texture is used as a procedural
  // texture.
  csRGBpixel bias[256];
  for (i = 0 ; i < 256 ; i++)
  {
    int r = ((i & 0xe0) >> 5) << (8-3);
    int g = ((i & 0x1c) >> 2) << (8-3);
    int b = ((i & 0x03) >> 0) << (8-2);
    bias[i].red = r;
    bias[i].green = g;
    bias[i].blue = b;
  }
  quant.Count (bias, 256, 0);
#endif

  csRGBpixel *pal = palette;
  palette_size = 256;
  quant.Palette (pal, palette_size, tc);

  for (i = 0; i < 4; i++)
    if (tex [i])
    {
      csSoftwareTexture *t = (csSoftwareTexture *)tex [i];
      if (!t->image) break;
      uint8* bmap = t->bitmap; // Temp assignment to pacify BeOS compiler.
      if (texman->dither_textures || (flags & CS_TEXTURE_DITHER))
        quant.RemapDither ((csRGBpixel *)t->image->GetImageData (),
          t->get_size (), t->get_width (), pal, palette_size, bmap, tc);
      else
        quant.Remap ((csRGBpixel *)t->image->GetImageData (),
          t->get_size (), bmap, tc);
      t->bitmap = bmap;

      // Get the alpha map for the texture, if present
      if (t->image->GetFormat () & CS_IMGFMT_ALPHA)
      {
        csRGBpixel *srcimg = (csRGBpixel *)t->image->GetImageData ();
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
	DG_UNLINK (t, t->image);
	t->image = 0;
      }
    }

  quant.End ();

  SetupFromPalette ();
}

void csSoftwareTextureHandle::SetupFromPalette ()
{
  int i;
  // Compute the mean color from the palette
  csRGBpixel *src = palette;
  unsigned r = 0, g = 0, b = 0;
  for (i = palette_size; i > 0; i--)
  {
    csRGBpixel pix = *src++;
    r += pix.red;
    g += pix.green;
    b += pix.blue;
  }
  mean_color.red   = r / palette_size;
  mean_color.green = g / palette_size;
  mean_color.blue  = b / palette_size;
}

void csSoftwareTextureHandle::remap_texture ()
{
  int i;
  CS_ASSERT (texman);
  switch (texman->pfmt.PixelBytes)
  {
    case 2:
      delete [] (uint16 *)pal2glob;
      pal2glob = new uint8 [palette_size * sizeof (uint16)];
      for (i = 0; i < palette_size; i++)
        ((uint16 *)pal2glob) [i] = texman->encode_rgb (palette [i].red,
          palette [i].green, palette [i].blue);
      break;
    case 4:
      delete [] (uint32 *)pal2glob;
      pal2glob = new uint8 [palette_size * sizeof (uint32)];
      for (i = 0; i < palette_size; i++)
      {
        ((uint32 *)pal2glob) [i] = texman->encode_rgb (palette [i].red,
          palette [i].green, palette [i].blue);
      }
      break;
  }
}

void csSoftwareTextureHandle::Prepare ()
{
  CreateMipmaps ();
  remap_texture ();
}

class csOFSCbSoftware : public iOffscreenCanvasCallback
{
private:
  csSoftwareTextureHandle* txt;

public:
  csOFSCbSoftware (csSoftwareTextureHandle* txt)
  {
    SCF_CONSTRUCT_IBASE (0);
    csOFSCbSoftware::txt = txt;
  }
  virtual ~csOFSCbSoftware ()
  {
  }
  SCF_DECLARE_IBASE;
  virtual void FinishDraw (iGraphics2D*)
  {
    txt->UpdateTexture ();
  }
  virtual void SetRGB (iGraphics2D*, int idx, int r, int g, int b)
  {
    txt->ChangePaletteEntry (idx, r, g, b);
  }
};


SCF_IMPLEMENT_IBASE(csOFSCbSoftware)
  SCF_IMPLEMENTS_INTERFACE(iOffscreenCanvasCallback)
SCF_IMPLEMENT_IBASE_END

iGraphics2D* csSoftwareTextureHandle::GetCanvas ()
{
  if (!canvas)
  {
    csOFSCbSoftware* ofscb = new csOFSCbSoftware (this);
    csSoftwareTexture *t = (csSoftwareTexture *)tex [0];
    canvas = texman->G3D->GetDriver2D ()->CreateOffscreenCanvas (
  	t->bitmap, t->get_width (), t->get_height (), 8,
	ofscb);
    ofscb->DecRef ();
    int i;
    is_palette_init = true;
    for (i = 0 ; i < palette_size ; i++)
    {
      canvas->SetRGB (i, palette[i].red, palette[i].green,
      	palette[i].blue);
    }
    is_palette_init = false;
  }
  return canvas;
}

void csSoftwareTextureHandle::ChangePaletteEntry (int idx, int r, int g, int b)
{
  if (is_palette_init) return;
  if (idx >= palette_size)
  {
    void* p2g;
    if (texman->pfmt.PixelBytes == 2)
    {
      p2g = new uint8 [256 * sizeof (uint16)];
      memcpy (p2g, pal2glob, sizeof (uint16)*palette_size);
    }
    else
    {
      p2g = new uint8 [256 * sizeof (uint32)];
      memcpy (p2g, pal2glob, sizeof (uint32)*palette_size);
    }
    pal2glob = p2g;
    palette_size = 256;
  }

  palette[idx].red = r;
  palette[idx].green = g;
  palette[idx].blue = b;
  if (texman->pfmt.PixelBytes == 2)
  {
    ((uint16*)pal2glob)[idx] = texman->encode_rgb (r, g, b);
  }
  else
  {
    ((uint32*)pal2glob)[idx] = texman->encode_rgb (r, g, b);
  }
}

//----------------------------------------------------------------------------//

SCF_IMPLEMENT_IBASE_INCREF(csSoftRendererLightmap)					
SCF_IMPLEMENT_IBASE_GETREFCOUNT(csSoftRendererLightmap)				
SCF_IMPLEMENT_IBASE_REFOWNER(csSoftRendererLightmap)				
SCF_IMPLEMENT_IBASE_QUERY(csSoftRendererLightmap)
SCF_IMPLEMENTS_INTERFACE(iRendererLightmap)
SCF_IMPLEMENT_IBASE_END

void csSoftRendererLightmap::DecRef ()
{
  if (scfRefCount == 1)							
  {									
    CS_ASSERT (slm != 0);
    slm->FreeRLM (this);
    return;								
  }									
  scfRefCount--;							
}

csSoftRendererLightmap::csSoftRendererLightmap ()
{
  SCF_CONSTRUCT_IBASE (0);

  data = 0;
  dirty = false;

  lightCellSize = 0;
  memset (cacheData, 0, sizeof (cacheData));
}

csSoftRendererLightmap::~csSoftRendererLightmap ()
{
}

void csSoftRendererLightmap::GetRendererCoords (float& lm_u1, float& lm_v1, 
                                                float &lm_u2, float& lm_v2)
{
  lm_u1 = u1;
  lm_v1 = v1;
  lm_u2 = u2;
  lm_v2 = v2;
}

void csSoftRendererLightmap::GetSLMCoords (int& left, int& top, 
                                           int& width, int& height)
{
  left = rect.xmin; top  = rect.xmin;
  width = rect.Width (); height = rect.Height ();
}

void csSoftRendererLightmap::SetData (csRGBpixel* data)
{
  csSoftRendererLightmap::data = data;
  dirty = true;
}

void csSoftRendererLightmap::SetLightCellSize (int size)
{
  lightCellSize = size;
  lightCellShift = csLog2 (lightCellSize);
}

int csSoftRendererLightmap::GetLightCellSize ()
{
  return lightCellSize;
}

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csSoftSuperLightmap)
SCF_IMPLEMENTS_INTERFACE(iSuperLightmap)
SCF_IMPLEMENT_IBASE_END

csSoftSuperLightmap::csSoftSuperLightmap (int width, int height) : RLMs(32)
{
  SCF_CONSTRUCT_IBASE (0);

  w = width; h = height;
}

csSoftSuperLightmap::~csSoftSuperLightmap ()
{
}

void csSoftSuperLightmap::FreeRLM (csSoftRendererLightmap* rlm)
{
  // IncRef() ourselves manually.
  // Otherwise freeing the RLM could trigger our own destruction -
  // causing an assertion in block allocator (due to how BA frees items and
  // the safety assertions on BA destruction.)
  scfRefCount++;
  RLMs.Free (rlm);
  DecRef ();
}

csPtr<iRendererLightmap> csSoftSuperLightmap::RegisterLightmap (int left, int top, 
                                                                int width, int height)
{
  csSoftRendererLightmap* rlm = RLMs.Alloc ();
  rlm->slm = this;
  rlm->rect.Set (left, top, left + width, top + height);

  rlm->u1 = left;
  rlm->v1 = top;
  rlm->u2 = left + width;
  rlm->v2 = top  + height;

  return csPtr<iRendererLightmap> (rlm);
}

csPtr<iImage> csSoftSuperLightmap::Dump ()
{
  return 0;
}


//----------------------------------------------- csSoftwareTextureManager ---//

static uint8 *GenLightmapTable (int bits)
{
  uint8 *table = new uint8 [64 * 256];
  uint8 *dst = table;
  uint8 maxv = (1 << bits) - 1;
  int rshf = (13 - bits);
  int i, j;
  for (i = 0; i < 64; i++)
  {
    *dst++ = 0;
    for (j = 1; j < 256; j++)
    {
      int x = (i * j) >> rshf;
      *dst++ = (x > maxv) ? maxv : (x?x:1) ;
    }
  }
  return table;
}

csSoftwareTextureManager::csSoftwareTextureManager (
  iObjectRegistry *object_reg,
  csSoftwareGraphics3DCommon *iG3D, iConfigFile *config)
  : csTextureManager (object_reg, iG3D->GetDriver2D())
{
  read_config (config);
  G3D = iG3D;
}

void csSoftwareTextureManager::SetPixelFormat (csPixelFormat &PixelFormat)
{
  pfmt = PixelFormat;

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
}

void csSoftwareTextureManager::read_config (iConfigFile *config)
{
  csTextureManager::read_config (config);
  dither_textures = config->GetBool
        ("Video.Software.TextureManager.DitherTextures", true);
  sharpen_mipmaps = config->GetInt
        ("Video.Software.TextureManager.SharpenMipmaps", 0);
}

csSoftwareTextureManager::~csSoftwareTextureManager ()
{
  delete [] lightmap_tables [0];
  if (lightmap_tables [1] != lightmap_tables [0])
    delete [] lightmap_tables [1];
  if (lightmap_tables [2] != lightmap_tables [1]
   && lightmap_tables [2] != lightmap_tables [0])
    delete [] lightmap_tables [2];
  Clear ();
}

void csSoftwareTextureManager::Clear ()
{
  csTextureManager::Clear();
}

uint32 csSoftwareTextureManager::encode_rgb (int r, int g, int b)
{
  return
    ((r >> (8 - pfmt.RedBits))   << pfmt.RedShift) |
    ((g >> (8 - pfmt.GreenBits)) << pfmt.GreenShift) |
    ((b >> (8 - pfmt.BlueBits))  << pfmt.BlueShift);
}

void csSoftwareTextureManager::PrepareTextures ()
{
  if (verbose)
    G3D->Report (CS_REPORTER_SEVERITY_NOTIFY,
    	"Preparing textures (%s dithering)...",
	dither_textures ? "with" : "no");

  if (verbose)
    G3D->Report (CS_REPORTER_SEVERITY_NOTIFY, "  Creating texture mipmaps...");

  int i;

  // Create mipmaps for all textures
  for (i = 0; i < textures.Length (); i++)
  {
    csTextureHandle *txt = textures.Get (i);
    txt->CreateMipmaps ();
  }

  // Remap all textures according to the new colormap.
  for (i = 0; i < textures.Length (); i++)
  {
    csSoftwareTextureHandle* txt = (csSoftwareTextureHandle*)textures.Get (i);
    txt->remap_texture ();
  }
}

csPtr<iTextureHandle> csSoftwareTextureManager::RegisterTexture (iImage* image,
  int flags)
{
  if (!image)
  {
    G3D->Report(CS_REPORTER_SEVERITY_BUG,
      "BAAAD!!! csSoftwareTextureManager::RegisterTexture with 0 image!");

    csRef<iImage> im (csCreateXORPatternImage(32, 32, 5));
    image = im;
    im->IncRef ();	// Avoid smart pointer cleanup. @@@ UGLY
  }

  csSoftwareTextureHandle *txt = new csSoftwareTextureHandle (
  	this, image, flags);
  textures.Push (txt);
  return csPtr<iTextureHandle> (txt);
}

void csSoftwareTextureManager::UnregisterTexture (
		csSoftwareTextureHandle* handle)
{
  int idx = textures.Find (handle);
  if (idx >= 0) textures.DeleteIndex (idx);
}

csPtr<iSuperLightmap> csSoftwareTextureManager::CreateSuperLightmap (
  int width, int height)
{
  return csPtr<iSuperLightmap> (new csSoftSuperLightmap (width, height));
}

void csSoftwareTextureManager::GetMaxTextureSize (int& w, int& h, int& aspect)
{
  w = h = 2048;
  aspect = 32768;
}

