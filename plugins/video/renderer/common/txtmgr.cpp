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
#include "txtmgr.h"
#include "csutil/util.h"
#include "csutil/debug.h"
#include "qint.h"
#include "igraphic/image.h"
#include "ivideo/material.h"
#include "iengine/material.h"
#include "ivideo/graph2d.h"

csTexture::csTexture (csTextureHandle *Parent)
{
  parent = Parent;
  DG_ADD (this, 0);
  DG_TYPE (this, "csTexture");
}

csTexture::~csTexture ()
{
  DG_REM (this);
}

//----------------------------------------------------- csTextureHandle -----//

SCF_IMPLEMENT_IBASE (csTextureHandle)
  SCF_IMPLEMENTS_INTERFACE (iTextureHandle)
SCF_IMPLEMENT_IBASE_END

csTextureHandle::csTextureHandle (iImage* Image, int Flags)
{
  SCF_CONSTRUCT_IBASE (0);
  DG_ADDI (this, 0);
  DG_TYPE (this, "csTextureHandle");

  image = Image;
  DG_LINK (this, image);
  flags = Flags;

  tex [0] = tex [1] = tex [2] = tex [3] = 0;

  transp = false;
  transp_color.red = transp_color.green = transp_color.blue = 0;

  if (image->HasKeycolor ())
  {
    int r,g,b;
    image->GetKeycolor (r,g,b);
    SetKeyColor (r, g, b);
  }
  cachedata = 0;
}

csTextureHandle::~csTextureHandle ()
{
  int i;
  for (i = 0; i < 4; i++)
  {
    if (tex[i])
    {
      DG_UNLINK (this, tex[i]);
      delete tex [i];
    }
  }
  DG_REM (this);
  FreeImage ();
}

void csTextureHandle::FreeImage ()
{
  if (!image) return;
  DG_UNLINK (this, image);
  image = 0;
}

void csTextureHandle::CreateMipmaps ()
{
  if (!image) return;

  csRGBpixel *tc = transp ? &transp_color : (csRGBpixel *)0;

  // Delete existing mipmaps, if any
  int i;
  for (i = 0; i < 4; i++)
  {
    if (tex[i])
    {
      DG_UNLINK (this, tex[i]);
      delete tex[i];
      tex[i] = 0;
    }
  }

  tex [0] = NewTexture (image);
  DG_LINK (this, tex[0]);

  // 2D textures uses just the top-level mipmap
  if ((flags & (CS_TEXTURE_3D | CS_TEXTURE_NOMIPMAPS)) == CS_TEXTURE_3D)
  {
    // Create each new level by creating a level 1 mipmap from previous level
    int nMipmaps = image->HasMipmaps();
    csRef<iImage> i1 = image->MipMap (1, tc);
    csRef<iImage> i2 =
      (nMipmaps >= 2) ? image->MipMap (2, tc) : i1->MipMap (1, tc);
    csRef<iImage> i3 =
      (nMipmaps >= 3) ? image->MipMap (3, tc) : i2->MipMap (1, tc);

    tex [1] = NewTexture (i1, true);
    DG_LINK (this, tex[1]);
    tex [2] = NewTexture (i2, true);
    DG_LINK (this, tex[2]);
    tex [3] = NewTexture (i3, true);
    DG_LINK (this, tex[3]);
  }

  ComputeMeanColor ();
}

void csTextureHandle::SetKeyColor (bool Enable)
{
  transp = Enable;
}

// This function must be called BEFORE calling TextureManager::Update().
void csTextureHandle::SetKeyColor (uint8 red, uint8 green, uint8 blue)
{
  transp_color.red = red;
  transp_color.green = green;
  transp_color.blue = blue;
  transp = true;
}

/// Get the transparent color
void csTextureHandle::GetKeyColor (uint8 &r, uint8 &g, uint8 &b)
{
  r = transp_color.red;
  g = transp_color.green;
  b = transp_color.blue;
}

bool csTextureHandle::GetKeyColor ()
{
  return transp;
}

void csTextureHandle::GetMeanColor (uint8 &r, uint8 &g, uint8 &b)
{
  r = mean_color.red;
  g = mean_color.green;
  b = mean_color.blue;
}

bool csTextureHandle::GetMipMapDimensions (int mipmap, int& w, int& h)
{
  csTexture *txt = get_texture (mipmap);
  if (txt)
  {
    w = txt->get_width ();
    h = txt->get_height ();
    return true;
  }
  return false;
}

void csTextureHandle::CalculateNextBestPo2Size (const int width, 
						const int height, 
						int& newWidth, 
						int& newHeigth)
{
  newWidth = csFindNearestPowerOf2 (width);
  if (newWidth != width)
  {
    int dU = newWidth - width;
    int dD = width - (newWidth >> 1);
    if (dD < dU)
      newWidth >>= 1;
  }
  newHeigth = csFindNearestPowerOf2 (height);
  if (newHeigth != height)
  {
    int dU = newHeigth - height;
    int dD = height - (newHeigth >> 1);
    if (dD < dU)
      newHeigth >>= 1;
  }
}

void csTextureHandle::AdjustSizePo2 ()
{
  int newwidth;
  int newheight;

  CalculateNextBestPo2Size (image->GetWidth(), image->GetHeight(),
    newwidth, newheight);

  if (newwidth != image->GetWidth () || newheight != image->GetHeight ())
    image->Rescale (newwidth, newheight);
}

//----------------------------------------------------- csMaterialHandle -----//

SCF_IMPLEMENT_IBASE (csMaterialHandle)
  SCF_IMPLEMENTS_INTERFACE (iMaterialHandle)
SCF_IMPLEMENT_IBASE_END

csMaterialHandle::csMaterialHandle (iMaterial* m, csTextureManager *parent)
{
  SCF_CONSTRUCT_IBASE (0);
  DG_ADDI (this, 0);
  DG_TYPE (this, "csMaterialHandle");
  num_texture_layers = 0;
  material = m;
  if (material != 0)
  {
    texture = material->GetTexture ();
    if (texture)
    {
      DG_LINK (this, texture);
    }
    material->GetReflection (diffuse, ambient, reflection);
    material->GetFlatColor (flat_color);
    num_texture_layers = material->GetTextureLayerCount ();
    if (num_texture_layers > 4) num_texture_layers = 4;
    int i;
    for (i = 0 ; i < num_texture_layers ; i++)
    {
      texture_layers[i] = *(material->GetTextureLayer (i));
      texture_layer_translate[i] =
	texture_layers[i].uscale != 1 ||
	texture_layers[i].vscale != 1 ||
	texture_layers[i].ushift != 0 ||
	texture_layers[i].vshift != 0;
    }
  }
  texman = parent;
}

csMaterialHandle::csMaterialHandle (iTextureHandle* t, csTextureManager *parent)
{
  SCF_CONSTRUCT_IBASE (0);
  DG_ADDI (this, 0);
  DG_TYPE (this, "csMaterialHandle");
  num_texture_layers = 0;
  diffuse = 0.7; ambient = 0; reflection = 0;
  texture = t;
  if (texture != 0)
  {
    DG_LINK (this, texture);
  }
  texman = parent;
}

csMaterialHandle::~csMaterialHandle ()
{
  FreeMaterial ();
  texman->UnregisterMaterial (this);
  DG_REM (this);
}

void csMaterialHandle::FreeMaterial ()
{
  if (texture)
  {
    DG_UNLINK (this, texture);
  }
  material = 0;
}

void csMaterialHandle::Prepare ()
{
  if (material)
  {
    if (texture != material->GetTexture())
    {
      DG_UNLINK (this, texture);
      texture = material->GetTexture ();
      if (texture)
      {
	DG_LINK (this, texture);
      }
    }
    material->GetReflection (diffuse, ambient, reflection);
    material->GetFlatColor (flat_color);
  }
}

//------------------------------------------------------------ csTexture -----//

void csTexture::compute_masks ()
{
  shf_w = csLog2 (w);
  and_w = (1 << shf_w) - 1;
  shf_h = csLog2 (h);
  and_h = (1 << shf_h) - 1;
}

//----------------------------------------------------- csTextureManager -----//

SCF_IMPLEMENT_IBASE (csTextureManager)
  SCF_IMPLEMENTS_INTERFACE (iTextureManager)
SCF_IMPLEMENT_IBASE_END

csTextureManager::csTextureManager (iObjectRegistry* object_reg,
	iGraphics2D *iG2D)
	: textures (16, 16), materials (16, 16)
{
  SCF_CONSTRUCT_IBASE (0);
  csTextureManager::object_reg = object_reg;
  verbose = false;

  pfmt = *iG2D->GetPixelFormat ();
}

csTextureManager::~csTextureManager()
{
  Clear ();
  //printf("Texture manager now going bye byes...\n"); //@@@ Debugging. MHV
}

void csTextureManager::read_config (iConfigFile* /*config*/)
{
}

void csTextureManager::FreeImages ()
{
  int i;
  for (i = 0 ; i < textures.Length () ; i++)
    textures.Get (i)->FreeImage ();
}

int csTextureManager::GetTextureFormat ()
{
  return CS_IMGFMT_TRUECOLOR | CS_IMGFMT_ALPHA;
}

csPtr<iMaterialHandle> csTextureManager::RegisterMaterial (iMaterial* material)
{
  if (!material) return 0;
  csMaterialHandle *mat = new csMaterialHandle (material, this);
  materials.Push (mat);
  return csPtr<iMaterialHandle> (mat);
}

csPtr<iMaterialHandle> csTextureManager::RegisterMaterial (
	iTextureHandle* txthandle)
{
  if (!txthandle) return 0;
  csMaterialHandle *mat = new csMaterialHandle (txthandle, this);
  materials.Push (mat);
  return csPtr<iMaterialHandle> (mat);
}

void csTextureManager::UnregisterMaterial (csMaterialHandle* handle)
{
  int idx = materials.Find (handle);
  if (idx >= 0) materials.DeleteIndex (idx);
}

void csTextureManager::PrepareMaterials ()
{
  int i;
  for (i = 0; i < materials.Length (); i++)
    materials.Get (i)->Prepare ();
}

void csTextureManager::FreeMaterials ()
{
  int i;
  for (i = 0; i < materials.Length (); i++)
    materials.Get (i)->FreeMaterial ();
}
