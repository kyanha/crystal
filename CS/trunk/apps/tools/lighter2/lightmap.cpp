/*
  Copyright (C) 2005 by Marten Svanfeldt

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

#include "crystalspace.h"
#include "common.h"

#include "lightmap.h"
#include "lighter.h"

namespace lighter
{
  void Lightmap::AddAmbientTerm (const csColor amb)
  {
    for (uint i = 0; i < data.GetSize (); i++)
    {
      data[i] += amb;
    }
  }

  void Lightmap::ApplyExposureFunction (float expConstant, float expMax)
  {
    for (uint i = 0; i < data.GetSize (); i++)
    {
      csColor &c = data[i];
      c.red = expMax * (1 - expf (-c.red * expConstant));
      c.green = expMax * (1 - expf (-c.green * expConstant));
      c.blue = expMax * (1 - expf (-c.blue * expConstant));
    }
  }

  void Lightmap::SaveLightmap (const csString& fname)
  {
    filename = fname;
    //write it out

#ifndef DUMP_NORMALS
    // 0.5 to account for the fact that the shader does *2
    ApplyExposureFunction (1.8f, 0.5f); 
#endif

    // first we downsample to LDR csRGBpixel RGBA
    csRGBpixel *pixelData = new csRGBpixel[width*height];
    for (uint i = 0; i < data.GetSize (); i++)
    {
      csColor &c = data[i];
      c.Clamp (1.0f,1.0f,1.0f); //just make sure we don't oversaturate below
      pixelData[i].red = (uint) (c.red * 255.0f);
      pixelData[i].green = (uint) (c.green * 255.0f);
      pixelData[i].blue = (uint) (c.blue * 255.0f);
    }

    // make an image
    csRef<iImage> img;
    img.AttachNew (new csImageMemory (width, height, pixelData));
    csRef<iDataBuffer> imgData = globalLighter->imageIO->Save (img, "image/png");
    csRef<iFile> file = globalLighter->vfs->Open (fname, VFS_FILE_WRITE);
    if (file)
    {
      file->Write (imgData->GetData (), imgData->GetSize ());
      file->Flush ();
    }
    delete[] pixelData;
  }

  iTextureWrapper* Lightmap::GetTexture()
  {
    if (texture == 0)
    {
      texture = globalLighter->engine->CreateBlackTexture (
        GetTextureName(), 1, 1, 0, CS_TEXTURE_3D);
    }
    return texture;
  }

  bool Lightmap::IsNull () const
  {
    for (uint i = 0; i < data.GetSize (); i++)
      if ((data[i].red != 0) || (data[i].green != 0) || (data[i].blue != 0))
        return false;
    return true;
  }

  csString Lightmap::GetTextureNameFromFilename (const csString& file)
  {
    csString out (file);
    out.ReplaceAll ("\\", "_"); //replace bad characters
    out.ReplaceAll ("/", "_"); 
    out.ReplaceAll (" ", "_"); 
    out.ReplaceAll (".", "_"); 
    return out;
  }
}
