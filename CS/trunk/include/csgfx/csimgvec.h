/*
    Copyright (C) 2003 by Philipp Aumayr

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

#ifndef __CS_CSGRAPHIC_IMGLIST_H__
#define __CS_CSGRAPHIC_IMGLIST_H__

#include "csutil/scf.h"
#include "csutil/refarr.h"
#include "igraphic/image.h"
#include "igraphic/imgvec.h"

SCF_VERSION (csImageVector, 0, 0, 1);

struct csImageVector : public iImageVector
{
private:
  csRefArray<iImage> image;
public:

  csImageVector();
  virtual ~csImageVector();

  SCF_DECLARE_IBASE;
  /**
  * Add an Image to the Vector
  */
  virtual void AddImage (iImage* img);
  /**
  * Insert an Image into the Vector at specified index
  */
  virtual void InsertImage (iImage* img, int index);
  /**
  * Add an Image to the End of the Vector
  */
  virtual void operator += (iImage* img);
  /**
  * Get Image at specified index
  */
  virtual csRef<iImage> GetImage(int index);
  /**
  * Get Image operator[]
  */
  virtual void SetImage (int index, iImage* img);
  /**
  * Get Image Count
  */
  virtual int Length();
  /**
   * Remove a specific index
   */
  virtual void DeleteIndex(int index);
  /**
  * Remove All Images
  */
  virtual void RemoveAll();

};


#endif // __CS_IGRAPHIC_IMGLIST_H__
