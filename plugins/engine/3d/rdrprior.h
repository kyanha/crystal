/*
    Copyright (C) 2001 by Martin Geisse <mgeisse@gmx.net>

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

#ifndef __CS_ENGINE_RDRPRIOR_H__
#define __CS_ENGINE_RDRPRIOR_H__

#include "csutil/array.h"
#include "csutil/parray.h"

struct iMeshWrapper;
struct iRenderView;

typedef csArray<iMeshWrapper*> csArrayMeshPtr;
typedef csPDelArray<csArrayMeshPtr> csArrayMeshPtrVector;

/**
 * This class contains a list of rendering queues, each of which is a list
 * of mesh wrappers. The rendering queues are sorted by rendering priority.
 * Note that the mesh objects are not reference-counted!
 */
class csRenderQueueSet
{
private:
  // List of visiblel meshes per render priority. Updating
  // during VisTest() (OR only!)
  csArrayMeshPtrVector visible;

public:
  /// Constructor.
  csRenderQueueSet ();
  /// Destructor.
  ~csRenderQueueSet ();

  /// Clear all visible meshes.
  void ClearVisible ();
  
  /// Register a visible mesh object.
  void AddVisible (iMeshWrapper *mesh);

  /**
   * Sort all priority queues and return a sorted list of all mesh
   * objects for all priorities. This list should be deleted with delete[]
   * later. Returns 0 if there are no visible objects.
   * The number of objects returned in 'tot_num' is the size of the
   * returned array. Note that this function will only add
   * visible objects to the array (i.e. iVisibilityObject)!
   */
  iMeshWrapper** SortAll (iRenderView* rview, int& tot_num,
	uint32 current_visnr);

  /// Sort this queue based on the flags for that queue.
  void Sort (iRenderView* rview, int priority);
};

#endif // __CS_ENGINE_RDRPRIOR_H__

