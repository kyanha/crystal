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

// alpha.h: interface for the csAlphaHandle class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ALPHA_H
#define ALPHA_H
#include "types.h"
#include "csobject/csobj.h"
#include "csgfxldr/boxfilt.h"
#include "igraph2d.h"

class csWorld;
class AlphaMapFile;
interface IAlphaMapHandle;

/**
 * csAlphaMapHandle implements IAlphaMapHandle.
 */
class csAlphaMapHandle : public csObject
{
private:
  /// The corresponding ImageFile.
  AlphaMapFile* afile;
  /// The handle
  IAlphaMapHandle* alpha_handle;
  // Transparent color
public:
  /// Construct an alpha-map handle given a image file
  csAlphaMapHandle (AlphaMapFile* alphamap);
  /// Copy contstructor
  csAlphaMapHandle (csAlphaMapHandle &ah);
  /// Release alpha-map handle
  virtual ~csAlphaMapHandle ();

  /// Get the alph-map handle.
  IAlphaMapHandle* GetAlphaHandle () { return alpha_handle; }

  /// Set the alpha-map handle.
  void SetAlphaHandle (IAlphaMapHandle* h);

  /// Get the ImageFile.
  AlphaMapFile* GetAlphaMapFile () { return afile; }

  CSOBJTYPE;
};

#endif /*TEXTURE_H*/
