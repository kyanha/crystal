/*
    Copyright (C) 2005 Dan Hardfeldt and Seth Yastrov

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

#ifndef _CS_CEGUITEXTURE_H_
#define _CS_CEGUITEXTURE_H_

/**\file 
*/
/**
* \addtogroup CEGUI
* @{ */

// hack: work around problems caused by #defining 'new'
#if defined(CS_EXTENSIVE_MEMDEBUG) || defined(CS_MEMORY_TRACKER)
# undef new
#endif
#include <new>
#include <CEGUI.h>
#if defined(CS_EXTENSIVE_MEMDEBUG) || defined(CS_MEMORY_TRACKER)
# define new CS_EXTENSIVE_MEMDEBUG_NEW
#endif

#include "csutil/ref.h"
#include "ivideo/texture.h"

#include "ceguirenderer.h"

struct iObjectRegistry;

/// CS implementation of CEGUI::Texture.
class csCEGUITexture : public CEGUI::Texture
{
  friend class csCEGUIRenderer;
public:
  /// Constructor.
  csCEGUITexture (CEGUI::Renderer*, iObjectRegistry*);

  /// Destructor.
  virtual ~csCEGUITexture ();

  /// Get the width of the texture.
  virtual CEGUI::ushort getWidth (void) const;

  /// Get the height of the texture.
  virtual CEGUI::ushort getHeight (void) const;

  /// Load a texture from a file.
  virtual void loadFromFile (const CEGUI::String &filename, 
    const CEGUI::String &resourceGroup);

  /**
   * Load a texture directly from memory. This is called from CEGUI,
   * for example, when a font should be loaded.
   */
  virtual void loadFromMemory (const void *buffPtr,
    CEGUI::uint buffWidth, CEGUI::uint buffHeight, CEGUI::Texture::PixelFormat pixFmt);

  /// Get a handle to the texture.
  iTextureHandle* GetTexHandle() const;

private:
  /// Returns the renderer
  CEGUI::Renderer* getRenderer (void) const;
  CEGUI::Renderer* renderer;
  csRef<iTextureHandle> hTxt;
  iObjectRegistry* obj_reg;

};

#endif 
