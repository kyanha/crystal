/*
    Copyright (C) 2000 by W.C.A. Wijngaards

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
#include "csgfx/bumpmap.h"
#include "csgfx/rgbpixel.h"
#include "igraphic/image.h"

csBumpMap::csBumpMap (iImage* src, int fmt)
{
  bumpmap = NULL;
  width = src->GetWidth ();
  height = src->GetHeight ();
  format = fmt;

  /// Get the truecolor image.
  csRef<iImage> rgbimage = src;
  if (src->GetFormat () != CS_IMGFMT_TRUECOLOR)
  {
    rgbimage = src->Clone ();
    rgbimage->SetFormat (CS_IMGFMT_TRUECOLOR);
  }

  /// Now create the height bumpmap using the grayscale data of the image.
  csRGBpixel *rgbdata = (csRGBpixel *)rgbimage->GetImageData ();
  uint8 *heightdata = new uint8 [width * height];

  int u;
  for (u = 0; u < width * height; u++)
    heightdata [u] = rgbdata [u].Intensity ();

  /// now convert the height data to the desired format.
  if (format == CS_BUMPFMT_HEIGHT_8)
  {
    bumpmap = new uint8 [width * height];
    uint8 *map = (uint8 *)bumpmap;
    for (u = 0; u < width * height; u++)
      map [u] = heightdata [u];
  }
  else if (format == CS_BUMPFMT_SLOPE_44)
  {
    bumpmap = new uint8 [width * height];
    uint8 *map = (uint8 *)bumpmap;

    int y;
    for (y = 0; y < height; y++)
    {
      int x;
      for (x = 0; x < width; x++)
      {
	int dx = heightdata [y * width + (x + 1) % width] -
	  heightdata [y * width + (x - 1) % width];
	int dy = heightdata [((y + 1) % height) * width + x] -
	  heightdata [((y - 1) % height) * width + x];

	/// now dx,dy are -255 ... 255, but must fit in -8..+7
	dx >>= 5;
	dy >>= 5;
	map [y * width + x] = ((dx << 4) & 0xF0) | (dy & 0xF);
      }
    }
  }
  
  delete [] heightdata;
}

csBumpMap::~csBumpMap ()
{
  delete [] bumpmap;
}

