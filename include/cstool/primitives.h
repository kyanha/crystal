/*
    Copyright (C) 2005 by Jorrit Tyberghein

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

#ifndef __CS_PRIMITIVES_H__
#define __CS_PRIMITIVES_H__

/**\file
 * Primitive Mesh Generator
 */

#include "csextern.h"

#include "csgeom/vector3.h"
#include "csgeom/vector2.h"
#include "csutil/cscolor.h"
#include "csutil/dirtyaccessarray.h"

class csBox3;
class csSphere;

/**
 * A primitive mesh generator.
 */
class CS_CRYSTALSPACE_EXPORT csPrimitives
{
public:
  /**
   * Generate a box with 24 vertices and 12 triangles so that
   * the normals of every face point outwards (the normals of the
   * vertices belonging to a face will point with the correct
   * normal of the face).
   */
  static void GenerateBox (
      const csBox3& box,
      csDirtyAccessArray<csVector3>& mesh_vertices,
      csDirtyAccessArray<csVector2>& mesh_texels,
      csDirtyAccessArray<csVector3>& mesh_normals,
      csDirtyAccessArray<csTriangle>& mesh_triangles);

  /**
   * Generate a sphere with 'num' vertices on the rim.
   */
  static void GenerateSphere (const csSphere& sphere, int num,
      csDirtyAccessArray<csVector3>& mesh_vertices,
      csDirtyAccessArray<csVector2>& mesh_texels,
      csDirtyAccessArray<csVector3>& mesh_normals,
      csDirtyAccessArray<csTriangle>& mesh_triangles);
};

/** @} */

#endif // __CS_PRIMITIVES_H__

