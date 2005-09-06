/*
    Copyright (C) 2005 Seth Yastrov

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

#ifndef _CS_CEGUIRESOURCEPROVIDER_H_
#define _CS_CEGUIRESOURCEPROVIDER_H_

/**\file 
*/
/**
* \addtogroup CEGUI
* @{ */

#include "csutil/ref.h"
#include "iutil/vfs.h"

#include "CEGUI.h"

struct iObjectRegistry;

/**
 * This is a VFS implementation of the CEGUI::ResourceProvider.
 */
class csCEGUIResourceProvider : public CEGUI::ResourceProvider
{
public:
  /// Constructor.
  csCEGUIResourceProvider (iObjectRegistry*);

  /// Destructor.
  virtual ~csCEGUIResourceProvider ();

  /**
   * Load raw data container using VFS.
   * @param filename VFS path.
   * @param output Raw data container where file will be loaded into.
   * @param resourceGroup Group the loaded data will belong to.
   */
  virtual void loadRawDataContainer (const CEGUI::String& filename,
    CEGUI::RawDataContainer& output, const CEGUI::String& resourceGroup);

  /// Delete the raw data container.
  virtual void unloadRawDataContainer (CEGUI::RawDataContainer& data);

protected:
  iObjectRegistry* obj_reg;
  csRef<iVFS> vfs;
};

#endif
