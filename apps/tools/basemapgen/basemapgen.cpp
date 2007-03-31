/*
    Copyright (C) 2007 by Jelle Hellemans aka sueastside

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

#include "basemapgen.h"

CS_IMPLEMENT_APPLICATION

//-----------------------------------------------------------------------------

// The global pointer to basemapgen
BaseMapGen *basemapgen;

BaseMapGen::BaseMapGen (iObjectRegistry* object_reg)
{
  BaseMapGen::object_reg = object_reg;
}

BaseMapGen::~BaseMapGen ()
{
}

bool BaseMapGen::Initialize ()
{
  if (0 != 0)
  {
    Report("Not enough chickens sacrificed for cthulhu!");
    return false;
  }

  if (!csInitializer::RequestPlugins (object_reg,
  	CS_REQUEST_VFS,
	CS_REQUEST_FONTSERVER,
	CS_REQUEST_IMAGELOADER,
	CS_REQUEST_REPORTER,
	CS_REQUEST_REPORTERLISTENER,
	CS_REQUEST_END))
  {
    Report("Can't initialize plugins!");
    return false;
  }

  // Check for commandline help.
  if (csCommandLineHelper::CheckHelp (object_reg))
  {
    OnCommandLineHelp();
    return false;
  }

  cmdline = csQueryRegistry<iCommandLineParser> (object_reg);
  if (!cmdline)
  {
    Report("No iCommandLineParser plugin!");
    return false;
  }

  // Open the main system. This will open all the previously loaded plug-ins.
  if (!csInitializer::OpenApplication (object_reg))
  {
    Report("Error opening system!");
    return false;
  }

  return true;
}

void BaseMapGen::Report(const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csReportV(object_reg, CS_REPORTER_SEVERITY_ERROR, "crystalspace.application.basemapgen", msg, arg);
  va_end (arg);
}

void BaseMapGen::OnCommandLineHelp()
{
  printf("\n");
  printf("Usage: basemapgen path world [OPTIONS]\n");
  printf(" Example: basemapgen /this/data/terrain world -terrainname=Terrain\n");
  printf("\n");
  printf("<path>                    The path to the worldfile        (default /this)\n");
  printf("<world>                   Name of the world file           (default 'world')\n");
  printf("-terrainname=<name>       Name of the terrain mesh object  (default 'Terrain')\n");
  printf("-terraformername=<name>   Name of the terraformer          (default 'simple')\n");
  printf("-resolution=<pixels>      The resolution for the basemap   (default basemap resolution)\n");
  printf("-factor=<value>           Up/down scales the basemap       (default 1)\n");
}

bool BaseMapGen::LoadMap ()
{
  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);
  csRef<iConfigManager> cfgmgr = csQueryRegistry<iConfigManager> (object_reg);

  csRef<iConfigFile> cfgfile = cfgmgr->GetDynamicDomain ();
  csPrintf("Loading custom VFS mounts from:\n%s\n", cfgfile->GetFileName ());
  vfs->LoadMountsFromFile(cfgfile);
  
  csString path = cmdline->GetName(0);
  csString world = cmdline->GetName(1);

  if (path.IsEmpty()) path = "/this";
  if (world.IsEmpty()) world = "world";

  csStringArray paths;
  paths.Push ("/lev/");
  if (!vfs->ChDirAuto(path.GetData(), &paths, 0, "world"))
  {
    Report("Error setting directory '%s'!", path.GetData ());
    return false;
  }

  csRef<iFile> buf = vfs->Open(world.GetData(), VFS_FILE_READ);
  if (!buf)
  {
    Report("Failed to open file '%s'!", world.GetData());
    return false;
  }

  csRef<iDocument> doc;
  csRef<iDocumentSystem> docsys;

  if (!docsys) docsys = csPtr<iDocumentSystem> (new csTinyDocumentSystem ());
  doc = docsys->CreateDocument ();
  const char* error = doc->Parse (buf, true);
  if (error != 0)
    return false;

  if (doc)
  {
    rootnode = doc->GetRoot()->GetNode("world");
    if (!rootnode)
      return false;
  }

  return true;
}

void BaseMapGen::AddMaterialLayer (csArray<MaterialLayer>& mat_layers, iDocumentNode* materialnode)
{
  MaterialLayer tl;
  csRef<iDocumentNodeIterator> it;

  // Set the texturename.
  csRef<iDocumentNode> texnode = materialnode->GetNode("texture");
  if(!texnode) return;
  tl.texture_name = texnode->GetContentsValue();

  // Set the texture scale.
  it = materialnode->GetNodes("shadervar");
  csVector2 texscale(32,32);
  while (it->HasNext())
  {
    csRef<iDocumentNode> current = it->Next();
    csString name = current->GetAttributeValue("name");
    if ( name.Compare("texture scale"))
    {
      csString scalestr = current->GetContentsValue();
      size_t pos = scalestr.FindFirst(",",0);
      csString firststr = scalestr.Slice(0,pos);
      csString secondstr = scalestr.Slice(pos+1, scalestr.Length());
      int first = atoi(firststr.GetData());
      int second = atoi(secondstr.GetData());
      texscale = csVector2(first, second);
      break;
    }
  } // while
  tl.texture_scale = texscale;

  // Set the texturefile.
  tl.texture_file = GetTextureFile(tl.texture_name);

  // Set the image.
  tl.image = LoadImage(tl.texture_file, CS_IMGFMT_TRUECOLOR);
   
  // Push it on the array.
  mat_layers.Push(tl);
}

csRef<iDocumentNode> BaseMapGen::GetTerrainNode ()
{
  csRef<iDocumentNode> terrainnode;

  csString terrainname = cmdline->GetOption("terrainname");
  if (terrainname.IsEmpty()) terrainname = "Terrain";

  csPrintf("Trying to load terrain '%s' ...\n", terrainname.GetData());fflush (stdout);

   // Get the terrain node
  csRef<iDocumentNodeIterator> sectors = rootnode->GetNodes("sector");
  while (sectors->HasNext())
  {
    csRef<iDocumentNode> sector = sectors->Next();
    csRef<iDocumentNodeIterator> it = sector->GetNodes("meshobj");
    while (it->HasNext())
    {
      csRef<iDocumentNode> current = it->Next();
      csString name = current->GetAttributeValue("name");
      if (name.Compare(terrainname))
      {
        terrainnode = current;
        break;
      }
    } // while meshobj
  } // while sector

  if (!terrainnode.IsValid())
  {
    Report("Couldn't find terrain node!");
  }

  return terrainnode;
}

csString BaseMapGen::GetTextureFile (const csString& texturename)
{
  csRef<iDocumentNode> textures = rootnode->GetNode("textures");
  csRef<iDocumentNodeIterator> it = textures->GetNodes("texture");
  while (it->HasNext())
  {
    csRef<iDocumentNode> current = it->Next();
    csString name = current->GetAttributeValue("name");
    if (name.Compare(texturename))
    {
      csRef<iDocumentNode> file = current->GetNode("file");
      csString filename = file->GetContentsValue();
      return filename;
    }
  } // while


  Report("Texture '%s' not found!", texturename.GetData());

  return "";
}

csRef<iDocumentNode> BaseMapGen::GetMaterialNode (const csString& materialname)
{
  csRef<iDocumentNode> material;
  csRef<iDocumentNode> mats = rootnode->GetNode ("materials");
  if (!mats) return material;

  csRef<iDocumentNodeIterator> it_mats = mats->GetNodes("material");
  while (it_mats->HasNext())
  {
    csRef<iDocumentNode> mat = it_mats->Next();
    csString matname = mat->GetAttributeValue("name");
    if (materialname.Compare(matname))
    {
      material = mat;
      return material;
    }
  } // while

  return material;
}

csRefArray<iDocumentNode> BaseMapGen::GetMaterialNodes ()
{
  csRefArray<iDocumentNode> materials;
  csRef<iDocumentNode> terrainnode = GetTerrainNode();
  if (!terrainnode) return materials;

  // Get the materialpalette.
  csRef<iDocumentNode> params = terrainnode->GetNode ("params");
  if (!params) return materials;
  csRef<iDocumentNode> materialpalette = params->GetNode ("materialpalette");
  if (!materialpalette) return materials;
  csRef<iDocumentNode> mats = rootnode->GetNode ("materials");
  if (!materialpalette) return materials;

  // Get the materials.
  csRef<iDocumentNodeIterator> it = materialpalette->GetNodes("material");
  while (it->HasNext())
  {
    csRef<iDocumentNode> pal = it->Next();
    csString matname = pal->GetContentsValue();
    csRef<iDocumentNode> matnode = GetMaterialNode(matname);
    if (matnode.IsValid())
      materials.Push(matnode);
  } // while

  csPrintf ("Found %d materials...\n", materials.GetSize()); fflush (stdout);

  return materials;
}

BaseMapGen::ImageMap BaseMapGen::GetMaterialMap ()
{
  ImageMap imagemap;
  csString terrname = cmdline->GetOption("terraformername");
  if (terrname.IsEmpty()) terrname = "simple";

  csPrintf("Trying to load terraformer '%s' ...\n", terrname.GetData());fflush (stdout);

  // Get the terraformer node
  csRef<iDocumentNodeIterator> it = rootnode->GetNodes("addon");
  while (it->HasNext())
  {
    csRef<iDocumentNode> current = it->Next();
    csRef<iDocumentNode> params = current->GetNode ("params");
    if (!params) params = current;
    csRef<iDocumentNode> matmap = params->GetNode ("materialmap");
    if (!matmap) continue;
    csRef<iDocumentNode> namenode = params->GetNode("name");
    if (!namenode) continue;
    csString name = namenode->GetContentsValue();
    if ( name.Compare(terrname))
    {
      // Set the image.
      imagemap.texture_file = matmap->GetAttributeValue("image");
      imagemap.image = LoadImage(imagemap.texture_file, CS_IMGFMT_PALETTED8);
      return imagemap;
    }
  } // while


  Report("Couldn't find terraformer node for '%s'!", terrname.GetData());


  return imagemap;
}

BaseMapGen::ImageMap BaseMapGen::GetBaseMap ()
{
  ImageMap imagemap;

  csRef<iDocumentNode> terrainnode = GetTerrainNode();
  if (!terrainnode) return imagemap;


  csRef<iDocumentNode> params = terrainnode->GetNode ("params");
  if (!params) return imagemap;
  csRef<iDocumentNode> material = params->GetNode ("material");
  if (!material) return imagemap;

  // Get texture file name.
  csString matname = material->GetContentsValue();
  csRef<iDocumentNode> matnode = GetMaterialNode(matname);
  if (!matnode) return imagemap;
  csRef<iDocumentNode> tex = matnode->GetNode ("texture");
  if (!tex) return imagemap;
  csString texname = tex->GetContentsValue();
  imagemap.texture_file = GetTextureFile(texname);

  // Load image.
  imagemap.image = LoadImage(imagemap.texture_file, CS_IMGFMT_TRUECOLOR);

  return imagemap;
}

csRef<iImage> BaseMapGen::LoadImage (const csString& filename, int format)
{
  csPrintf("Trying to load '%s'... \t", filename.GetData());fflush (stdout);
  csRef<iImage> image;
  csRef<iImageIO> imageio = csQueryRegistry<iImageIO> (object_reg);
  csRef<iVFS> VFS = csQueryRegistry<iVFS> (object_reg);

  csRef<iDataBuffer> buf = VFS->ReadFile (filename.GetData(), false);
  if (!buf)
  {
    Report("Failed to load image file '%s'!", filename.GetData());
    return image;
  }
  image = imageio->Load (buf, format);
  image.IsValid() ? printf("SUCCESS\n") : printf("FAILED\n");

  return image;
}

void BaseMapGen::SaveImage (BaseMapGen::ImageMap image)
{
  csRef<iImageIO> imageio = csQueryRegistry<iImageIO> (object_reg);
  csRef<iVFS> VFS = csQueryRegistry<iVFS> (object_reg);

  csRef<iDataBuffer> db = imageio->Save (image.image, "image/png", "progressive");
  if (db)
  {
    if (!VFS->WriteFile (image.texture_file.GetData (), (const char*)db->GetData (), db->GetSize ()))
    {
      Report("Failed to write file '%s'!", image.texture_file.GetData ());
      return;
    }
  }
  else
  {
    Report("Failed to save png image for basemap!");
    return;
  }
}

void BaseMapGen::DrawProgress (int percent)
{
  uint numDone = (65*percent) / 100;
  uint numToDo = (65*(100-percent)) / 100;

  csPrintf(CS_ANSI_CLEAR_LINE);
  csPrintf(CS_ANSI_CURSOR_BWD(70));
  csPrintf(CS_ANSI_CURSOR_UP(1));

  printf("[");
  for (int x = 0 ; x < numDone ; x++)
    printf("=");

  for (int x = 0 ; x < numToDo ; x++)
    printf(" ");

  csPrintf("] %d% \n", percent);

  fflush (stdout);

}


csRGBpixel BaseMapGen::GetPixel (MaterialLayer material, float coord_x, float coord_y)
{
  int matcoord_x, matcoord_y;

  // Calculate the material coordinates.
  matcoord_x =  (int) (coord_x * material.image->GetWidth());
  matcoord_y =  (int) (coord_y * material.image->GetHeight());

  // Scale the texture coordinates.
  matcoord_x *= (int) (material.texture_scale.x);
  matcoord_y *= (int) (material.texture_scale.y);

  // Wrap around the texture coordinates.
  matcoord_x = matcoord_x % material.image->GetWidth();
  matcoord_y = matcoord_y % material.image->GetHeight();

  // Get the material pixel.
  csRGBpixel* mat = (csRGBpixel*)material.image->GetImageData();
  csRGBpixel* mat_dst = mat + matcoord_x + (matcoord_y * material.image->GetHeight());


  return csRGBpixel(mat_dst->red, mat_dst->green, mat_dst->blue);
}

void BaseMapGen::CreateBasemap (int basemap_res, 
                                ImageMap basetexture, 
                                int matmap_res, 
                                ImageMap materialmap, 
                                const csArray<MaterialLayer>& mat_layers)
{
  printf("Creating base texturemap... \n\n");fflush (stdout);
  {//to measure time
    CS::MeasureTime lTimeMeasurer ("Time taken");

    // Get image data.
    csRGBpixel* bm_dst = (csRGBpixel*)(basetexture.image->GetImageData());
    uint8* matmap_dst = (uint8*)(materialmap.image->GetImageData ());

    int layercoord_x, layercoord_y;
    float coord_x, coord_y;

    float inv_basemap_res = 1.0f / basemap_res;

    for (int y = 0 ; y < basemap_res ; y++)
    {
      // Draw progress.
      uint percent = (y*100)/ basemap_res;
      DrawProgress(percent);

      for (int x = 0 ; x < basemap_res ; x++)
      {
        // Get the layer index.
        layercoord_x =  (int) (x * inv_basemap_res * matmap_res);
        layercoord_y =  (int) (y * inv_basemap_res * matmap_res);
        unsigned int layer = *(matmap_dst + layercoord_x + (layercoord_y * matmap_res));
        if (layer > mat_layers.GetSize())
        {
          bm_dst->Set (255, 0, 0);
          continue;
        }

        if (!mat_layers[layer].image.IsValid())
        {
          bm_dst->Set (255, 0, 0);
          continue;
        }

        // Calculate the destination coordinates.
        coord_x    = x * inv_basemap_res;
        coord_y    = y * inv_basemap_res;

        // Get the material pixel.
        csRGBpixel mat_dst = GetPixel(mat_layers[layer], coord_x, coord_y);

        // Set the basemap pixel.
        bm_dst->Set (mat_dst.red, mat_dst.green, mat_dst.blue);

        // Increase the pointer.
        bm_dst++;

      } // for y
    } // for x

  } // Block to measure time

}

void BaseMapGen::Start ()
{
  // Load the world.
  if (!LoadMap ())
  {
    Report("Error opening world file!");
    return;
  }

  // Iterate the materials on the terrain.
  csArray<MaterialLayer> mat_layers;
  csRefArray<iDocumentNode> materials = GetMaterialNodes();
  for (unsigned int i = 0 ; i < materials.GetSize() ; i++)
  {
    AddMaterialLayer (mat_layers, materials.Get(i));
  }

  // Get the materialmap.
  ImageMap materialmap = GetMaterialMap();
  if (!materialmap.image)
  {
    Report("Failed to load Material Map!");
    return;
  }

  // Get the basematerial.
  ImageMap basetexture = GetBaseMap();
  if (!basetexture.image)
  {
    Report("Failed to load Base Map!");
    return;
  }

  // Get the resolution.
  int matmap_res = materialmap.image->GetHeight();
  int basemap_res = basetexture.image->GetHeight();

  // Get the resolution from the commandline.
  csString res = cmdline->GetOption("resolution");
  if (!res.IsEmpty())
    basemap_res = csFindNearestPowerOf2(atoi(res));

  // Get the upscale factor from the commandline.
  csString factorstr = cmdline->GetOption("factor");
  int factor = 1;
  if (!factorstr.IsEmpty())
  {
    factor = atoi(factorstr);
    factor = (factor <= 1) ? 1 : csFindNearestPowerOf2(factor);
  }

  printf("Using resolution: %d, factor: %d\n", basemap_res, factor);

  // Make it bigger.
  basemap_res *= factor;
  if (basemap_res != basetexture.image->GetHeight())
  {
    basetexture.image.Invalidate();
    basetexture.image.AttachNew (new csImageMemory (basemap_res, basemap_res));
  }
  if (factor > 1)
    printf("Upscaling image to:\t %d x %d\n", basetexture.image->GetWidth(), basetexture.image->GetHeight());

  // Scale up the materials to the size of the basemap.
  for (unsigned int i = 0 ; i < mat_layers.GetSize() ; i++)
  {
    mat_layers[i].image = csImageManipulate::Rescale(mat_layers[i].image, basemap_res, basemap_res, 24);
  }

  // Create the basemap.
  CreateBasemap (basemap_res, basetexture, matmap_res, materialmap, mat_layers);

  // Make it smaller.
  if (factor > 1)
  {
    int step = (int)(log((float)factor) / log(2.0f));
    basetexture.image = csImageManipulate::Mipmap(basetexture.image, step);
    printf("Downscaling image to:\t %d x %d\n", basetexture.image->GetWidth(), basetexture.image->GetHeight());
  }

  // Save the basemap.
  printf("Saving %d KB of data.\n", csImageTools::ComputeDataSize(basetexture.image)/1024);
  SaveImage (basetexture);
  
 
}

/*---------------------------------------------------------------------*
 * Main function
 *---------------------------------------------------------------------*/
int main (int argc, char* argv[])
{
  iObjectRegistry* object_reg = csInitializer::CreateEnvironment (argc, argv);
  if (!object_reg) return -1;

  basemapgen = new BaseMapGen (object_reg);

  if (basemapgen->Initialize ())
    basemapgen->Start ();

  delete basemapgen;

  csInitializer::DestroyApplication (object_reg);
  return 0;
}
