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

#ifndef __LIGHTMESH_H__
#define __LIGHTMESH_H__

#include "csgeom/math.h"
#include "csgeom/transfrm.h"
#include "csgeom/vector2.h"
#include "csgeom/vector3.h"

#include "csutil/array.h"
#include "csutil/cscolor.h"
#include "csutil/csstring.h"
#include "csutil/parray.h"
#include "csutil/refcount.h"
#include "csutil/ref.h"

class litMeshFace;
class litLightingMesh;

struct iExtractorInformation;
struct iMeshObject;
struct iMeshObjectFactory;
struct iMeshWrapper;

/**
 * Material used during light computation.
 */
class litLightingMaterial : public csRefCount
{
public:
};

/**
 * Factory for light meshes. Corresponds to CS mesh factories.
 */
class litLightingMeshFactory : public csRefCount
{
public:
  litLightingMeshFactory ()
    : moFact (0)
  {
  }

  // Identification of CS mesh factory
  iMeshObjectFactory *moFact;

  // Original (untransformed) mesh information
  csArray<csVector3> vertices;
  
  // Information needed to create a face..
  struct litMeshFactoryFace
  {
    // Vertex indices
    csArray<uint> indices;

    // Lightmap texture mapping for this face
    csArray<csVector2> textureCoordinates;

    // Material
    csRef<litLightingMaterial> material;
  };

  // Original set of faces
  csArray<litMeshFactoryFace> faces;

  // Create a new mesh from factory
  csPtr<litLightingMesh> CreateMesh (iMeshWrapper *meshwrapper);
};

/**
 * A single mesh used for light computation.
 */
class litLightingMesh : public csRefCount
{
public:
  litLightingMesh ()
    : mo (0), factory (0)
  {
  }
  // Identification of the real mesh
  iMeshObject *mo;

  // The faces that makes up the mesh
  csPDelArray<litMeshFace> faces;

  /// Extractor/saver specific information
  iExtractorInformation *extractorInfo;

  //Vertex positions
  csArray<csVector3> vertexList;
  //Vertex colors (for shared colors)
  csArray<csColor> colorList;

  //O2w-transform
  csReversibleTransform transform;

  // Factory the object is created from
  litLightingMeshFactory *factory;
};

class litMeshPatch;
class litMeshFace
{
public:
  //Properties
  //Geometric normal
  csVector3 geoNormal;

  //Initial emmitance
  csColor emmitance;

  //Toplevel patches for this face
  csArray<litMeshPatch*> patches;

  //Mesh
  litLightingMesh* mesh;

  //Material
  litLightingMaterial *material;

  //Methods
  //Add new vertex, return index to the newly added one
  uint AddVertex (const csVector3 &vertex)
  {
    mesh->vertexList.Push (vertex);
    return (uint)(mesh->vertexList.Length ()-1);
  }

  //Add new shared color, return index to the newly added one
  uint AddColor (const csColor &color)
  {
    mesh->colorList.Push (color);
    return (uint)(mesh->colorList.Length ()-1);
  }
};

/**
 * Raytracing acceleration structure.
 * The structure is ment to be cache optimized (we do need to fix it for
 * 64 bit platforms, but for now ignore them :) so please take care if 
 * you ever find any reason for changing it (visual cleanup is not an
 * allowed reason for change ;)
 * 
 * See [Wald04], page 97 for more information.
 */
struct litMeshPatchAccStruct
{
  //Half cacheline
  //Plane
  float normal_u; //normal.u / normal.k
  float normal_v; //normal.v / normal.k
  float normal_d; //d in normal equation
  int32 k;        //projection axis, 0x, 1y, 2z

  //Half cacheline
  //Line ab
  float b_nu;
  float b_nv;
  float b_d;
  int32 pad; //pad rest of cacheline

  //Half cacheline
  //Line ac
  float c_nu;
  float c_nv;
  float c_d;

  //pointer to patch
  litMeshPatch *patch; //hope pointers are 32-bit, otherwise we are screwed.. ;)
};

/**
 * Helperfunction to populate a litMeshPatchAccStruct 
 */
void litSetupAccStruct (litMeshPatchAccStruct &acc, const csVector3 &a, const csVector3 &b,
                       const csVector3 &c, const csVector3 &normal);

/**
 * A single patch in light processing.
 *
 * The structure is ment to be cache optimized (we do need to fix it for
 * 64 bit platforms, but for now ignore them :) so please take care if 
 * you ever find any reason for changing it (visual cleanup is not an
 * allowed reason for change ;)
 *
 * Geometric layout:
 * A patch can be either quad or triangle. In both cases vertices,
 * neighbours and children are numbered counter clockwise.
 * 
 * Triangle
 *
 *             0 
 *             /\
 *            /  \
 *     0     / 0  \     2
 *          /______\
 *         /\      /\
 *        /  \ 3  /  \
 *       / 1  \  / 2  \
 *      /______\/______\
 *     1                2
 *             1
 *
 * Quad
 *            0
 *    0               3
 *     +------+------+
 *     |      |      |
 *     |  0   |   3  |
 *     |      |      |
 *  1  +------+------+  3
 *     |      |      |
 *     |  1   |   2  |
 *     |      |      |
 *     +------+------+
 *    1               2
 *            2
 */
class litMeshPatch
{
public:
  //Properties
  // Indices to vertex and color info
  // vertexIndex[3] will be -1 for a triangle patch
  int vertexIndex[4];           //4*4=16 bytes

  // Pointer to common info for all patches from a single polygon
  litMeshFace *parentFace;       //4 bytes

  // Color of patch
  csColor color;                //24 bytes

  // Area
  float area;                   //4 bytes

  // Neighbour patches
  litMeshPatch *neighbour[4];    //4*4=16 bytes

  // Child patches
  litMeshPatch *child[4];     //4*4=16 bytes

  // Parent patch
  litMeshPatch *parent;          //4 bytes

  // Centerpoint
  csVector3 center;             //24 bytes

  //Acceleration structure used during raycasting for patches
  //If this is a quad, two acceleration structures after eachother
  //is used.
  litMeshPatchAccStruct* accStruct;    //4 bytes

  /*TOTAL SIZE:                 //128 bytes.. 0 bytes free until we occupy
  more than 4 cachelines (4*32 = 128 bytes).*/

  //Methods

  // Constructor
  litMeshPatch ();

  // Destructor
  ~litMeshPatch ();

  // Is patch a quad
  const bool IsQuad () const { return vertexIndex[3] >= 0; }
  // Is patch a triangle
  const bool IsTriangle () const { return vertexIndex[3] < 0; }
  // Do patch have any children
  const bool HasChildren () const { return neighbour[0] != 0; }
    
  // Get the geometric normal of patch
  const csVector3& GetGeoNormal () const { return parentFace->geoNormal; }

  // Helper to get vertex by index
  const csVector3& GetVertex (uint i) const { return parentFace->mesh->vertexList[vertexIndex[i]];}
  // Helper to get color by index
  const csColor& GetColor (uint i) const { return parentFace->mesh->colorList[vertexIndex[i]];}
  
  // Helper to set vertex indices
  void SetVertexIdx (uint i1, uint i2, uint i3, uint i4 = (uint)~0)
  {
    vertexIndex[0] = i1; vertexIndex[1] = i2; vertexIndex[2] = i3;
    vertexIndex[3] = i4;
  }

  // Setup neighbour connections for our children
  void SetupNeighbours ();

  // Subdivide patch one step
  void Subdivide ();

  // Construct the acceleration structures
  void ConstructAccelerationStruct ();

  // Destruct the acceleration structures. Make sure they are not used outside this class
  // after destruction
  void FreeAccelerationStructs ();

  // Return minimal and maximal coordinate along given axis
  void GetExtent (int axis, float &coord1, float &coord2)
  {
    coord1 = FLT_MAX;
    coord2 = -FLT_MAX;
    
    coord1 = csMin (coord1, GetVertex (0)[axis]);
    coord1 = csMin (coord1, GetVertex (1)[axis]);
    coord1 = csMin (coord1, GetVertex (2)[axis]);

    coord2 = csMax (coord2, GetVertex (0)[axis]);
    coord2 = csMax (coord2, GetVertex (1)[axis]);
    coord2 = csMax (coord2, GetVertex (2)[axis]);

    if (IsQuad ())
    {
      coord1 = csMin (coord1, GetVertex (3)[axis]);
      coord2 = csMax (coord2, GetVertex (3)[axis]);
    }
  }
};

#endif 
