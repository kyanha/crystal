/*
    Copyright (C) 2001 by W.C.A. Wijngaards
  
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
#include "csfx/gentrtex.h"
#include "csutil/cscolor.h"
#include "igraphic/image.h"
#include "csgfx/rgbpixel.h"
#include "csgfx/memimage.h"
#include "qint.h"

csGenerateTerrainImage::csGenerateTerrainImage()
{
  baselist = 0;
}

csGenerateTerrainImage::~csGenerateTerrainImage()
{
  csGenerateTerrainImagePart *p = baselist, *np =0;
  while(p)
  {
    np = p->next;
    if(p->image) p->image->DecRef();
    delete p;
    p = np;
  }
}

void csGenerateTerrainImage::AddLayer(float height, iImage *image, 
  const csVector2& scale, const csVector2& offset)
{
  /// find a spot
  csGenerateTerrainImagePart *p = baselist, *prevp = 0;
  while(p && (p->height < height))
  {
    prevp = p;
    p = p->next;
  }
  /// check value
  if(p && (p->height == height))
  {
    printf("csGenerateTerrainImage Error: "
      "encountered duplicate height %g. Ignoring.\n", height);
    return;
  }
  /// create new
  csGenerateTerrainImagePart *part = new csGenerateTerrainImagePart;
  p->next = 0;
  p->height = height;
  p->scale = scale;
  p->offset = offset;
  p->image = image;
  if(image) image->IncRef();
  /// insert in list
  if(p==0)
  {
    // append
    if(prevp) prevp->next = part;
    else baselist = part;
  }
  else if(prevp == 0)
  {
    // prepend
    part->next = baselist;
    baselist = part;
  }
  else
  {
    // insert into linked list
    part->next = p;
    prevp->next = part;
  }
}

void csGenerateTerrainImage::GetImagePixel(iImage *image, int x, int y,
  csRGBpixel& res)
{
  int r, g, b;
  x %= image->GetWidth();
  y %= image->GetHeight();
  if(image->GetFormat() & CS_IMGFMT_PALETTED8)
  {
    int pal = ((unsigned char*)image->GetImageData())[
      y*image->GetWidth() + x];
    r = image->GetPalette()[pal].red;
    g = image->GetPalette()[pal].green;
    b = image->GetPalette()[pal].blue;
  }
  else
  {
    // truecolor image
    r = ((csRGBpixel*)image->GetImageData())
      [y*image->GetWidth() + x].red;
    g = ((csRGBpixel*)image->GetImageData())
      [y*image->GetWidth() + x].green;
    b = ((csRGBpixel*)image->GetImageData())
      [y*image->GetWidth() + x].blue;
  }
  res.Set(r,g,b);
}


csColor csGenerateTerrainImage::ComputeLayerColor(
  csGenerateTerrainImagePart *layer, const csVector2& pos)
{
  csVector2 imagepos = pos - layer->offset;
  imagepos.x *= layer->scale.x;
  imagepos.y *= layer->scale.y;
  imagepos.x *= layer->image->GetWidth();
  imagepos.y *= layer->image->GetHeight();
  /// imagepos is now a pixel, floating valued.
  csRGBpixel pix;
  csColor col1, col2; /// left & right linear interpolation
  int x = QInt(imagepos.x);
  int y = QInt(imagepos.y);
  float blendy = imagepos.y - float(y);
  float invblendy = 1.f - blendy;
  GetImagePixel(layer->image, x, y+1, pix);
  col1.red = float(pix.red) * blendy;
  col1.green = float(pix.green) * blendy;
  col1.blue = float(pix.blue) * blendy;
  GetImagePixel(layer->image, x, y, pix);
  col1.red += float(pix.red) * invblendy;
  col1.green += float(pix.green) * invblendy;
  col1.blue += float(pix.blue) * invblendy;

  GetImagePixel(layer->image, x+1, y+1, pix);
  col2.red = float(pix.red) * blendy;
  col2.green = float(pix.green) * blendy;
  col2.blue = float(pix.blue) * blendy;
  GetImagePixel(layer->image, x+1, y, pix);
  col2.red += float(pix.red) * invblendy;
  col2.green += float(pix.green) * invblendy;
  col2.blue += float(pix.blue) * invblendy;

  /// now linearly interpolate col1 and col2
  csColor col;
  float blendx = imagepos.x - float(x);
  float invblendx = 1.f - blendx;
  col.red = invblendx * col1.red + blendx * col2.red;
  col.green = invblendx * col1.green + blendx * col2.green;
  col.blue = invblendx * col1.blue + blendx * col2.blue;

  // return trilinear interpolated value
  col *= 1./255.;
  return col;
}

csRGBpixel csGenerateTerrainImage::ComputeColor(const csVector2& pos)
{
  /// get height
  float height = heightfunc(userdata, pos.x, pos.y);
  /// find closest layers (below <), (above >=)
  csGenerateTerrainImagePart *below = 0, *above = 0;
  above = baselist;
  while(above && (above->height < height))
  {
    below = above;
    above = above->next;
  }
  /// compute color
  float belowfactor = 0.0;
  float abovefactor = 0.0;
  csColor abovecol, belowcol;
  if(!below && !above) return csRGBpixel(128, 128, 128);
  if(!below)
  {
    abovefactor = 1.0;
    abovecol = ComputeLayerColor(above, pos);
  }
  else if(!above)
  {
    belowfactor = 1.0;
    belowcol = ComputeLayerColor(below, pos);
  }
  else { // both an above and below - blend
    float dist = above->height - below->height;
    belowfactor = (above->height - height) / dist;
    abovefactor = 1.0 - belowfactor;
    abovecol = ComputeLayerColor(above, pos);
    belowcol = ComputeLayerColor(below, pos);
  }

  csColor col(0,0,0);
  col += abovecol * abovefactor;
  col += belowcol * belowfactor;

  return csRGBpixel (QInt(col.red), QInt(col.green), QInt(col.blue));
}


iImage *csGenerateTerrainImage::Generate(int totalw, int totalh, 
  int startx, int starty, int partw, int parth)
{
  csImageMemory *csim = new csImageMemory(partw, parth);
  csim->Clear(csRGBpixel (128, 128, 128));
  iImage *result = csim;
  
  csVector2 pixelsize( 1./float(totalw), 1./float(totalh) );
  csVector2 startpos(float(startx) * pixelsize.x, float(starty)*pixelsize.y);
  csVector2 pos;
  /// memory image is always truecolor
  csRGBpixel *destpix = (csRGBpixel*)result->GetImageData();
  for(int y=0; y< parth; y++)
  {
    pos.y = startpos.y + pixelsize.y * float(y);
    pos.x = startpos.x;
    for(int x=0; x< partw; x++)
    {
      /// compute color
      csRGBpixel col = ComputeColor(pos);
      /// set pixel
      *destpix = col;
      destpix++;
    }
  }

  return result;
}
