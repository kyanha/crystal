/*
    The Crystal Space geometry loader interface
    Copyright (C) 2000 by Andrew Zabolotny <bit@eltech.ru>

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

#ifndef __CS_IMAP_PARSER_H__
#define __CS_IMAP_PARSER_H__

/**\file 
 * Geometry loader interface
 */
/**\addtogroup loadsave
 * @{ */
#include "csutil/scf.h"
#include "ivideo/txtmgr.h"
#include "igraphic/image.h"

struct iTextureHandle;
struct iTextureWrapper;
struct iMaterialWrapper;
struct iSector;
struct iSoundData;
struct iSoundHandle;
struct iMeshWrapper;
struct iMeshFactoryWrapper;
struct iSoundWrapper;

SCF_VERSION (iLoader, 0, 0, 8);

/**
 * This interface represents the map loader.
 */
struct iLoader : public iBase
{
  /**
   * Load an image file. The image will be loaded in the format requested by
   * the engine. If no engine exists, the format is taken from the video
   * renderer. If no video renderer exists, this function fails. You may also
   * request an alternate format to override the above sequence.
   */
  virtual csPtr<iImage> LoadImage (const char* Filename,
    int Format = CS_IMGFMT_INVALID) = 0;
  /**
   * Load an image as with LoadImage() and create a texture handle from it.
   * The 'Flags' parameter accepts the flags described in ivideo/txtmgr.h.
   * The texture manager determines the format, so choosing an alternate format
   * doesn't make sense here. Instead you may choose an alternate texture
   * manager.
   */
  virtual csPtr<iTextureHandle> LoadTexture (const char* Filename,
	int Flags = CS_TEXTURE_3D, iTextureManager *tm = NULL, iImage **image=NULL) = 0;
  /**
   * Load a texture as with LoadTexture() above and register it with the
   * engine. 'Name' is the name that the engine will use for the wrapper.
   * If 'create_material' is true then this function also creates a
   * material for the texture.<br>
   * If 'register' is true then the texture and material will be registered
   * to the texture manager. Set 'register' to false if you plan on calling
   * 'engine->Prepare()' later as that function will take care of registering
   * too.
   */
  virtual iTextureWrapper* LoadTexture (const char *Name,
  	const char *FileName,
	int Flags = CS_TEXTURE_3D, iTextureManager *tm = NULL,
	bool reg = false, bool create_material = true) = 0;

  /// Load a sound file and return an iSoundData object
  virtual csPtr<iSoundData> LoadSoundData (const char *fname) = 0;
  /// Load a sound file and register the sound
  virtual csPtr<iSoundHandle> LoadSound (const char *fname) = 0;
  /// Load a sound file, register the sound and create a wrapper object for it
  virtual csPtr<iSoundWrapper> LoadSound (const char *name,
  	const char *fname) = 0;

  /**
   * Load a map file. If 'clearEngine' is true then the current contents
   * of the engine will be deleted before loading. If 'resolveOnlyRegion'
   * is true then portals will only connect to the sectors in the current
   * region, things will only use thing templates defined in the current
   * region and meshes will only use mesh factories defined in the current
   * region.
   * <p>
   * If you use 'checkDupes' == true then materials, textures,
   * and mesh factories will only be loaded if they don't already exist
   * in the entire engine (ignoring regions). By default this is false because
   * it is very legel for different world files to have different objects
   * with the same name. Only use checkDupes == true if you know that your
   * objects have unique names accross all world files.
   */
  virtual bool LoadMapFile (const char* filename, bool clearEngine = true,
	bool resolveOnlyRegion = true, bool checkDupes = false) = 0;
  /// Load library from a VFS file
  virtual bool LoadLibraryFile (const char* filename) = 0;

  /// Load a Mesh Object Factory from the map file.
  virtual csPtr<iMeshFactoryWrapper> LoadMeshObjectFactory (
  	const char* fname) = 0;
  /**
   * Load a mesh object from a file.
   * The mesh object is not automatically added to the engine and sector.
   */
  virtual csPtr<iMeshWrapper> LoadMeshObject (const char* fname) = 0;
  
  /// Load and parse a effect from file
  virtual bool LoadEffectFile (const char* filename) = 0;
};

/** } */

#endif // __CS_IMAP_PARSER_H__

