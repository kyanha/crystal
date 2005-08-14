/*
    Copyright (C) 2000-2002 by Jorrit Tyberghein

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

#ifndef __CS_IMAP_LOADER_H__
#define __CS_IMAP_LOADER_H__

#include "csutil/scf.h"

/**\file
 */
/**\addtogroup loadsave
 * @{ */
struct iLoaderContext;
struct iDocumentNode;

SCF_VERSION (iLoaderPlugin, 0, 2, 1);

/**
 * This is a plugin for the loader based on document tree.
 */
struct iLoaderPlugin : public virtual iBase
{
  /// Parse a given document node and return a new object for it.
  virtual csPtr<iBase> Parse (iDocumentNode* node, iLoaderContext* ldr_context,
  	iBase* context) = 0;
};

SCF_VERSION (iBinaryLoaderPlugin, 0, 0, 1);

/**
 * This is a binary plugin for the loader.
 */
struct iBinaryLoaderPlugin : public iBase
{
  /// Parse given data and return a new object for it.
  virtual csPtr<iBase> Parse (void* data, iLoaderContext* ldr_context,
  	iBase* context) = 0;
};

/** @} */

#endif // __CS_IMAP_LOADER_H__

