/*
    Copyright (C) 2001 by Jorrit Tyberghein
  
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

#ifndef __CS_TERRFUNC_H__
#define __CS_TERRFUNC_H__

#include "csgeom/transfrm.h"
#include "csgeom/vector3.h"
#include "csobject/csobject.h"
#include "csutil/cscolor.h"
#include "iterrain/object.h"
#include "iengine/terrain.h"
#include "iterrain/terrfunc.h"

struct iEngine;
struct iMaterialWrapper;
struct iSystem;

#define LOD_LEVELS 4

/**
 * This is one block in the terrain.
 */
class csTerrBlock
{
public:
  G3DTriangleMesh mesh[LOD_LEVELS];	// Mesh with four LOD levels.
  csVector3* normals[LOD_LEVELS];	// Array of normals for the LOD levels.
  iMaterialWrapper* material;		// Material for this block.
  csVector3 center;			// Center for LOD.
  // Numbers of all meshes. Here we can see if we need to update the lighting
  // for a mesh and LOD.
  int dirlight_numbers[LOD_LEVELS];
  csBox3 bbox;				// Bounding box in 3D of this block.

public:
  csTerrBlock ();
  ~csTerrBlock ();
};

class csTerrFuncObject : public iTerrainObject
{
public:
  csTerrainHeightFunction* height_func;
  csTerrainNormalFunction* normal_func;
  void* height_func_data;
  void* normal_func_data;
  int blockx, blocky;
  int gridx, gridy;
  csVector3 topleft;
  csVector3 scale;
  csTerrBlock* blocks;
  bool block_dim_invalid;

private:
  iSystem* pSystem;
  iTerrainObjectFactory* pFactory;
  csColor base_color;
  // Squared distances at which to change LOD (0..2).
  float lod_sqdist[LOD_LEVELS-1];
  // Maximum cost for every lod transition.
  float max_cost[LOD_LEVELS-1];

  // For directional lighting.
  bool do_dirlight;
  csVector3 dirlight;
  csColor dirlight_color;
  // This number is increased whenever there is a lighting change
  long dirlight_number;

  bool initialized;

  /**
   * Clear a mesh and initialize it for new usage (call before
   * SetupBaseMesh() or ComputeLODLevel() (as dest)).
   */
  void InitMesh (G3DTriangleMesh& mesh);

  /**
   * Setup the base mesh (lod level 0). This will basically
   * initialize the mesh by sampling the height function at regular
   * intervals (gridx/gridy resolution).
   */
  void SetupBaseMesh (G3DTriangleMesh& mesh, int bx, int by);

  /**
   * Compute a destination mesh from a given source mesh
   * by reducing triangles.
   */
  void ComputeLODLevel (
	const G3DTriangleMesh& source, G3DTriangleMesh& dest,
	float maxcost, int& del_tri, int& tot_tri);

  /**
   * Compute the bounding box of a triangle mesh.
   */
  void ComputeBBox (const G3DTriangleMesh& mesh, csBox3& bbox);

  /**
   * Compute all bounding boxes.
   */
  void ComputeBBoxes ();
  
  /**
   * Allocate normal array and compute normals for one mesh and put
   * result in pNormals.
   */
  void ComputeNormals (const G3DTriangleMesh& mesh, csVector3** pNormals);

  /**
   * Compute all normal arrays (all lod levels and all blocks).
   */
  void ComputeNormals ();

  /**
   * Do the setup of the entire terrain. This will compute the base
   * mesh, the LOD meshes, normals, ...
   */
  void SetupObject ();

  /**
   * Recompute lighting for one block.
   */
  void RecomputeLighting (int lod, int bx, int by);

  /**
   * Test if this bounding box is visible in the given clipper.
   * If visible 'do_clip' will be set to true if clipping is needed.
   */
  bool BBoxVisible (const csBox3& bbox, iCamera* camera,
	iClipper2D* clipper, bool& do_clip);

public:
  /// Constructor.
  csTerrFuncObject (iSystem* pSys, iTerrainObjectFactory* factory);
  virtual ~csTerrFuncObject ();

  void LoadMaterialGroup (iEngine* engine, const char *pName,
    int iStart, int iEnd);
  /// Set the base color.
  void SetColor (const csColor& col) { base_color = col; dirlight_number++; }
  /// Get the base color.
  csColor GetColor () const { return base_color; }
  /// Set the function to use for the terrain.
  void SetHeightFunction (csTerrainHeightFunction* func, void* d)
  { height_func = func; height_func_data = d; initialized = false; }
  /// Set the normal function to use for the terrain.
  void SetNormalFunction (csTerrainNormalFunction* func, void* d)
  { normal_func = func; normal_func_data = d; initialized = false; }

  ///--------------------- iTerrainObject implementation ---------------------
  DECLARE_IBASE;

  virtual void SetDirLight (const csVector3& pos, const csColor& col)
  {
    csVector3 dp = dirlight - pos;
    if (do_dirlight &&
    	ABS (dp.x) < .00001 &&
	ABS (dp.y) < .00001 &&
	ABS (dp.z) < .00001 &&
        ABS (col.red-dirlight_color.red) < .0001 &&
        ABS (col.green-dirlight_color.green) < .0001 &&
	ABS (col.blue-dirlight_color.blue) < .0001)
      return;	// Nothing changed.
    do_dirlight = true;
    dirlight = pos;
    dirlight.Normalize ();
    dirlight_color = col;
    dirlight_number++;
  }
  virtual csVector3 GetDirLightPosition () const { return dirlight; }
  virtual csColor GetDirLightColor () const { return dirlight_color; }
  virtual void DisableDirLight () { do_dirlight = false; }
  virtual bool IsDirLightEnabled () const { return do_dirlight; }
  virtual void Draw (iRenderView *rview, bool use_z_buf = true);
  virtual void SetMaterial (int i, iMaterialWrapper* mat)
  {
    if (!blocks || block_dim_invalid)
    {
      blocks = new csTerrBlock [blockx*blocky];
      block_dim_invalid = false;
    }
    blocks[i].material = mat;
  }
  virtual int GetNumMaterials () { return blockx*blocky; }
  virtual void SetLOD (unsigned int) { }

  virtual int CollisionDetect (csTransform *p);

  /// Setup the number of blocks in the terrain.
  void SetResolution (int x, int y)
  {
    block_dim_invalid = blockx != x || blocky != y;
    blockx = x;
    blocky = y;
    initialized = false;
  }

  /// Get the x resolution.
  int GetXResolution () { return blockx; }
  /// Get the y resolution.
  int GetYResolution () { return blocky; }
  /**
   * Setup the number of grid points in every block for the base mesh.
   */
  void SetGridResolution (int x, int y)
  {
    gridx = x;
    gridy = y;
    initialized = false;
  }
  /// Get the x resolution for a block.
  int GetXGridResolution () { return gridx; }
  /// Get the y resolution for a block.
  int GetYGridResolution () { return gridy; }

  /// Set the top-left corner of the terrain.
  virtual void SetTopLeftCorner (const csVector3& topleft)
  {
    csTerrFuncObject::topleft = topleft;
    initialized = false;
  }
  // Get the top-left corner.
  virtual csVector3 GetTopLeftCorner ()
  {
    return topleft;
  }
  /// Set the scale of the terrain.
  virtual void SetScale (const csVector3& scale)
  {
    csTerrFuncObject::scale = scale;
    initialized = false;
  }
  /// Get the scale of the terrain.
  virtual csVector3 GetScale ()
  {
    return scale;
  }
  /**
   * Set the distance at which to switch to the given lod level
   * (lod from 1 to 3).
   */
  void SetLODDistance (int lod, float dist)
  {
    lod_sqdist[lod-1] = dist*dist;
  }
  /**
   * Get the distance at which lod will switch to that level.
   */
  float GetLODDistance (int lod)
  {
    return sqrt (lod_sqdist[lod-1]);
  }
  /**
   * Set the maximum cost for LOD level (1..3).
   */
  void SetMaximumLODCost (int lod, float maxcost)
  {
    max_cost[lod-1] = maxcost;
    initialized = false;
  }
  /**
   * Get the maximum cost for LOD level (1..3).
   */
  float GetMaximumLODCost (int lod)
  {
    return max_cost[lod-1];
  }

  //------------------------- iTerrFuncState implementation ----------------
  class TerrFuncState : public iTerrFuncState
  {
    DECLARE_EMBEDDED_IBASE (csTerrFuncObject);
    virtual void LoadMaterialGroup (iEngine* engine, const char *pName,
      int iStart, int iEnd)
    {
      scfParent->LoadMaterialGroup (engine, pName, iStart, iEnd);
    }
    virtual void SetTopLeftCorner (const csVector3& topleft)
    {
      scfParent->SetTopLeftCorner (topleft);
    }
    virtual csVector3 GetTopLeftCorner ()
    {
      return scfParent->GetTopLeftCorner ();
    }
    virtual void SetScale (const csVector3& scale)
    {
      scfParent->SetScale (scale);
    }
    virtual csVector3 GetScale ()
    {
      return scfParent->GetScale ();
    }
    virtual void SetResolution (int x, int y)
    {
      scfParent->SetResolution (x, y);
    }
    virtual int GetXResolution ()
    {
      return scfParent->GetXResolution ();
    }
    virtual int GetYResolution ()
    {
      return scfParent->GetYResolution ();
    }
    virtual void SetGridResolution (int x, int y)
    {
      scfParent->SetGridResolution (x, y);
    }
    virtual int GetXGridResolution ()
    {
      return scfParent->GetXGridResolution ();
    }
    virtual int GetYGridResolution ()
    {
      return scfParent->GetYGridResolution ();
    }
    virtual void SetColor (const csColor& col)
    {
      scfParent->SetColor (col);
    }
    virtual csColor GetColor () const
    {
      return scfParent->GetColor ();
    }
    virtual void SetHeightFunction (csTerrainHeightFunction* func,
    	void* d)
    {
      scfParent->SetHeightFunction (func, d);
    }
    virtual void SetNormalFunction (csTerrainNormalFunction* func,
    	void* d)
    {
      scfParent->SetNormalFunction (func, d);
    }
    virtual void SetLODDistance (int lod, float dist)
    {
      scfParent->SetLODDistance (lod, dist);
    }
    virtual float GetLODDistance (int lod)
    {
      return scfParent->GetLODDistance (lod);
    }
    virtual void SetMaximumLODCost (int lod, float maxcost)
    {
      scfParent->SetMaximumLODCost (lod, maxcost);
    }
    virtual float GetMaximumLODCost (int lod)
    {
      return scfParent->GetMaximumLODCost (lod);
    }
  } scfiTerrFuncState;
  friend class TerrFuncState;
};

/**
 * Factory for terrain. 
 */
class csTerrFuncObjectFactory : public iTerrainObjectFactory
{
private:
  iSystem *pSystem;

public:
  /// Constructor.
  csTerrFuncObjectFactory (iSystem* pSys);

  /// Destructor.
  virtual ~csTerrFuncObjectFactory ();

  DECLARE_IBASE;

  virtual iTerrainObject* NewInstance ();
};

/**
 * TerrFunc type. This is the plugin you have to use to create instances
 * of csTerrFuncObjectFactory.
 */
class csTerrFuncObjectType : public iTerrainObjectType
{
private:
  iSystem *pSystem;

public:
  /// Constructor.
  csTerrFuncObjectType (iBase*);

  /// Destructor.
  virtual ~csTerrFuncObjectType ();

  /// Register plugin with the system driver
  virtual bool Initialize (iSystem *pSys);

  //---------------------- iTerrainObjectType implementation --------------
  DECLARE_IBASE;

  /// Draw.
  virtual iTerrainObjectFactory* NewFactory ();
};

#endif // __CS_TERRFUNC_H__

