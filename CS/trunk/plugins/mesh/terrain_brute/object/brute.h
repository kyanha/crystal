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

#include "csgeom/objmodel.h"
#include "csgeom/transfrm.h"
#include "csgeom/vector3.h"
#include "cstool/rendermeshholder.h"
#include "csutil/bitarray.h"
#include "csutil/cscolor.h"
#include "csutil/refarr.h"
#include "csutil/thread.h"
#include "csutil/list.h"
#include "csutil/sysfunc.h"
#include "csutil/weakref.h"
#include "iengine/mesh.h"
#include "imesh/object.h"
#include "imesh/terrain.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "igeom/objmodel.h"
#include "ivaria/terraform.h"
#include "ivideo/rndbuf.h"

struct iEngine;
struct iMaterialWrapper;
struct iObjectRegistry;
class csTerrainQuad;
class csTerrainObject;
class csSegment3;

/**
* This is one block in the terrain.
*/
class csTerrBlock : public csRefCount
{
public:
  csRef<iRenderBuffer> mesh_vertices;
  csVector3 *vertex_data;
  csRef<iRenderBuffer> mesh_morphvertices;
  csVector3 *morphvertex_data;
  csRef<iRenderBuffer> mesh_normals;
  csVector3 *normal_data;
  csRef<iRenderBuffer> mesh_morphnormals;
  csVector3 *morphnormal_data;
  csRef<iRenderBuffer> mesh_texcoords;
  csVector2 *texcoord_data;
  csRef<iRenderBuffer> mesh_colors;
  csVector3 *color_data;

  int num_mesh_vertices;
  csArray<csRenderMesh> meshes;
  csRef<iMaterialWrapper> material;
  csVector3 center;
  float size;
  int res;

  bool built;

  csRef<iTerraSampler> terrasampler;

  csTerrainObject *terr;

  //          0        
  //      ---------
  //      | 0 | 1 |
  //    2 |-------| 1
  //      | 2 | 3 |
  //      ---------
  //          3

  csTerrBlock* parent;
  csRef<csTerrBlock> children[4];
  csTerrBlock* neighbours[4];
  int child;

  csRef<csShaderVariableContext> svcontext;

  static int tris;

  csBox3 bbox;

  csBitArray materialsChecked;
  csBitArray materialsUsed;
public:
  csTerrBlock (csTerrainObject *terr);
  ~csTerrBlock ();

  /// Generate mesh
  void SetupMesh ();

  /// Detach the node from the tree
  void Detach ();

  /// Split block in 4 children
  void Split ();

  /// Merge block
  void Merge ();

  /// Checks if something needs to be merged or split
  void CalcLOD (iRenderView *rview);

  /// Returns true if this node is a leaf
  bool IsLeaf ()  const
  { return children[0] == 0; }

  void DrawTest (iGraphics3D* g3d, iRenderView *rview, uint32 frustum_mask, 
                 csReversibleTransform &transform);

  bool detach;

  bool IsMaterialUsed (int index);
};


class csBlockBuilder : public csRunnable
{
private:
  csTerrainObject* terr;
  csList< csRef<csTerrBlock> > pendingbuilds;
  int buildcount;
  int refcount;
  bool run;
  csRef<csCondition> processlock;
  csRef<csMutex> listlock;

  void Process ()
  {
    while (true)
    {
      csRef<csTerrBlock> block;
      if (listlock->LockWait ())
      {
        if (pendingbuilds.IsEmpty ())
          break;
        
        block = pendingbuilds.Last ();
        pendingbuilds.PopBack ();
        listlock->Release ();
      }
      
      if (block->GetRefCount ()>1)
        block->SetupMesh ();
    }
  }

public:
  
  csBlockBuilder::csBlockBuilder (csTerrainObject *t)
  {
    processlock = csCondition::Create ();
    listlock = csMutex::Create ();
    run = true;
    terr = t;
    refcount = 1;
    buildcount = 0;
  }

  virtual csBlockBuilder::~csBlockBuilder ()
  {
  }

  virtual void Run ()
  {
    listlock->LockWait ();
    while (run)
    {
      processlock->Wait (listlock);
      listlock->Release ();
      if (!run)
        break;
      Process ();
    }
  }

  void BuildBlock (csTerrBlock *block)
  {
    if (listlock->LockWait ())
    {
      pendingbuilds.PushFront (block);
      listlock->Release ();
      processlock->Signal ();
    }
  }

  void Stop ()
  {
    run = false;
    processlock->Signal ();
  }

  virtual void IncRef() {refcount++;}
  virtual void DecRef() {refcount--;}
  virtual int GetRefCount() {return refcount;}
};

class csTerrainObject : public iMeshObject
{
private:

  friend class csTerrBlock;

  csBox2 region;
  csRef<csTerrBlock> rootblock;
  bool block_dim_invalid;
  csBox3 global_bbox;

  csFlags flags;

  csRefArray<iObjectModelListener> listeners;

  csRef<iRenderBuffer> mesh_indices[16];
  int numindices[16];

  csRef<iMaterialWrapper> matwrap;
  csArray<iMaterialWrapper*> palette;	// TODO@@@ Use csRefArray!!!
  csRefArray<iImage> alphas;

  float error_tolerance;
  float lod_distance;

  iObjectRegistry* object_reg;
  csWeakRef<iGraphics3D> g3d;
  iBase* logparent;
  csRef<iMeshObjectFactory> pFactory;
  csRef<iMeshObjectDrawCallback> vis_cb;
  iVertexBufferManager *vbufmgr;
  float lod_lcoeff;
  float lod_qcoeff;
  float block_maxsize;
  float block_minsize;
  int block_res;

  csBlockBuilder *builder;
  csRef<csThread> buildthread;

  csRef<iTerraFormer> terraformer;

  csArray<char> materialMap;
  int materialMapW, materialMapH;

  csDirtyAccessArray<csRenderMesh*>* returnMeshes;
  csRenderMeshHolderSingle rmHolder;
  csRenderMeshHolderMultiple returnMeshesHolder;
  csReversibleTransform tr_o2c;

  // Use for clipping during rendering.
  csPlane3 planes[10];

  csStringID vertices_name, morphvertices_name,
    normals_name, morphnormals_name,
    texcoords_name, indices_name,
    morphval_name, colors_name;

  bool initialized;

  bool staticlighting;
  bool castshadows;

  // Data for the colldet polygon mesh.
  bool polymesh_valid;
  csVector3* polymesh_vertices;
  int polymesh_vertex_count;
  csTriangle* polymesh_triangles;
  int polymesh_tri_count;
  csMeshedPolygon* polymesh_polygons;
  void SetupPolyMeshData ();
  int cd_resolution;

  /**
  * Do the setup of the entire terrain. This will compute the base
  * mesh, the LOD meshes, normals, ...
  */
  void SetupObject ();

public:
  /// Constructor.
  csTerrainObject (iObjectRegistry* object_reg, iMeshObjectFactory* factory);
  virtual ~csTerrainObject ();

  bool SetColor (const csColor &color) { return false; }
  bool GetColor (csColor &color) const { return false; }
  bool SetMaterialWrapper (iMaterialWrapper *material) 
  { 
    matwrap = material;
    return true; 
  }
  iMaterialWrapper *GetMaterialWrapper () const { return matwrap; }

  void SetBlockMaxSize (float size)
  {
    block_maxsize = size;
    initialized = false;
  }

  float GetBlockMaxSize () { return block_maxsize; }

  void SetBlockMinSize (float size)
  {
    block_minsize = size;
  }

  float GetBlockMinSize () { return block_minsize; }

  void SetBlockResolution (int res)
  {
    block_res = res;
    initialized = false;
  }

  int GetBlockResolution ()
  {
    return block_res;
  }

  void SetLODCoeffs (float linear, float quadratic)
  {
    lod_lcoeff = linear;
    lod_qcoeff = quadratic;
  }

  /**
  * Test visibility from a given position.
  * This will call MarkVisible() for all quad nodes that are visible.
  */
  //void TestVisibility (iRenderView* rview);

  int CollisionDetect (iMovable *m, csTransform *p);

  void GetObjectBoundingBox (csBox3& bbox, int type = CS_BBOX_NORMAL);
  void GetRadius (csVector3& rad, csVector3& cent);

  SCF_DECLARE_IBASE;
  ///--------------------- iMeshObject implementation ---------------------

  virtual bool Draw (iRenderView* rview, iMovable* movable, csZBufMode mode)
    { return false; }

  virtual csFlags& GetFlags () { return flags; }
  virtual csPtr<iMeshObject> Clone () { return 0; }

  virtual iMeshObjectFactory* GetFactory () const { return pFactory; }

  virtual bool DrawTest (iRenderView* rview, iMovable* movable, 
    uint32 frustum_mask);

  virtual csRenderMesh** GetRenderMeshes (int &n, iRenderView* rview,
    iMovable* movable, uint32 frustum_mask);

  virtual void UpdateLighting (iLight** lights, int num_lights,
    iMovable* movable);

  virtual void SetVisibleCallback (iMeshObjectDrawCallback* cb)
  {
    vis_cb = cb;
  }
  virtual iMeshObjectDrawCallback* GetVisibleCallback () const
  {
    return vis_cb;
  }

  virtual void NextFrame (csTicks, const csVector3& /*pos*/) { }

  virtual void HardTransform (const csReversibleTransform&) { }
  virtual bool SupportsHardTransform () const { return false; }
  virtual void SetLogicalParent (iBase* lp) { logparent = lp; }
  virtual iBase* GetLogicalParent () const { return logparent; }

  bool HitBeam (csTerrBlock* block,
	const csSegment3& seg,
	csVector3& isect, float* pr);
  virtual bool HitBeamOutline (const csVector3& start, const csVector3& end,
    csVector3& isect, float* pr);
  virtual bool HitBeamObject (const csVector3& start, const csVector3& end,
    csVector3& isect, float* pr, int* polygon_idx = 0);

  virtual void InvalidateMaterialHandles () { }
  virtual void PositionChild (iMeshObject* child,csTicks current_time) { }

  void FireListeners ();
  void AddListener (iObjectModelListener* listener);
  void RemoveListener (iObjectModelListener* listener);

  //------------------ iPolygonMesh interface implementation ----------------//
  struct PolyMesh : public iPolygonMesh
  {
  private:
    csTerrainObject* terrain;
    csFlags flags;
  public:
    SCF_DECLARE_IBASE;

    void SetTerrain (csTerrainObject* t)
    {
      terrain = t;
      flags.SetAll (CS_POLYMESH_TRIANGLEMESH);
    }
    void Cleanup ();

    virtual int GetVertexCount ();
    virtual csVector3* GetVertices ();
    virtual int GetPolygonCount ();
    virtual csMeshedPolygon* GetPolygons ();
    virtual int GetTriangleCount ();
    virtual csTriangle* GetTriangles ();
    virtual void Lock () { }
    virtual void Unlock () { }

    virtual csFlags& GetFlags () { return flags;  }
    virtual uint32 GetChangeNumber() const { return 0; }

    PolyMesh ()
    { SCF_CONSTRUCT_IBASE (0); }
    virtual ~PolyMesh ()
    { SCF_DESTRUCT_IBASE (); }
  } scfiPolygonMesh;
  friend class PolyMesh;

  //------------------------- iObjectModel implementation ----------------
  class eiObjectModel : public csObjectModel
  {
    SCF_DECLARE_EMBEDDED_IBASE (csTerrainObject);
    virtual void GetObjectBoundingBox (csBox3& bbox, int type = CS_BBOX_NORMAL)
    {
      scfParent->GetObjectBoundingBox (bbox, type);
    }
    virtual void GetRadius (csVector3& rad, csVector3& cent)
    {
      scfParent->GetRadius (rad, cent);
    }
  } scfiObjectModel;
  friend class ObjectModel;

  virtual iObjectModel* GetObjectModel () { return &scfiObjectModel; }

  //------------------ iTerrainObjectState implementation ----------------

  bool SetMaterialPalette (const csArray<iMaterialWrapper*>& pal);
  csArray<iMaterialWrapper*> GetMaterialPalette ();
  bool SetMaterialMap (const csArray<char>& data, int x, int y);
  bool SetMaterialMap (iImage* map);
  csArray<char> GetMaterialMap ();
  bool SetLODValue (const char* parameter, float value);
  float GetLODValue (const char* parameter);

  class eiTerrainObjectState : public iTerrainObjectState
  {
    SCF_DECLARE_EMBEDDED_IBASE (csTerrainObject);
    virtual bool SetMaterialPalette (const csArray<iMaterialWrapper*>& pal)
    {
      return scfParent->SetMaterialPalette (pal);
    }

    virtual const csArray<iMaterialWrapper*> &GetMaterialPalette () const
    {
      return scfParent->palette;
    }

    virtual bool SetMaterialMap (const csArray<char> &data, int x, int y)
    {
      return scfParent->SetMaterialMap (data, x, y);
    }
    
    virtual bool SetMaterialMap (iImage* map)
    {
      return scfParent->SetMaterialMap (map);
    }
    
    virtual csArray<char> GetMaterialMap ()
    {
      return scfParent->GetMaterialMap ();
    }

    virtual bool SetLODValue (const char* parameter, float value)
    {
      return scfParent->SetLODValue (parameter, value);
    }

    virtual float GetLODValue (const char* parameter) const
    {
      return scfParent->GetLODValue (parameter);
    }

    virtual bool SaveState (const char *filename)
    {
      return true;
    }

    virtual bool RestoreState (const char *filename)
    {
      return true;
    }

    virtual int CollisionDetect (iMovable *m, csTransform *p)
    {
      return scfParent->CollisionDetect (m, p);
    }

    virtual void SetStaticLighting (bool enable)
    { 
      scfParent->staticlighting = enable; 
    }
    
    virtual bool GetStaticLighting ()
    { 
      return scfParent->staticlighting; 
    }
    
    virtual void SetCastShadows (bool enable)
    { 
      scfParent->castshadows = enable; 
    }
    
    virtual bool GetCastShadows ()
    { 
      return scfParent->castshadows;
    }

  } scfiTerrainObjectState;
  friend class eiTerrainObjectState;
};

/**
* Factory for terrain.
*/
class csTerrainFactory : public iMeshObjectFactory
{
private:
  iBase* logparent;

  iMeshObjectType* brute_type;

  csFlags flags;

public:
  csRef<iTerraFormer> terraformer;

  csBox2 samplerRegion;
  int resolution;

  iObjectRegistry *object_reg;

  csVector3 scale;

  /// Constructor.
  csTerrainFactory (iObjectRegistry* object_reg, iMeshObjectType* parent);

  /// Destructor.
  virtual ~csTerrainFactory ();

  SCF_DECLARE_IBASE;

  virtual csPtr<iMeshObject> NewInstance ();
  virtual csPtr<iMeshObjectFactory> Clone () { return 0; }
  virtual void HardTransform (const csReversibleTransform&) { }
  virtual bool SupportsHardTransform () const { return false; }
  virtual void SetLogicalParent (iBase* lp) { logparent = lp; }
  virtual iBase* GetLogicalParent () const { return logparent; }
  virtual iMeshObjectType* GetMeshObjectType () const { return brute_type; }

  void SetTerraFormer (iTerraFormer* form);
  iTerraFormer* GetTerraFormer ();
  void SetSamplerRegion (const csBox2& region);
  const csBox2& GetSamplerRegion ();

  virtual csFlags& GetFlags () { return flags; }

  class eiTerrainFactoryState : public iTerrainFactoryState
  {
    SCF_DECLARE_EMBEDDED_IBASE (csTerrainFactory);
    virtual void SetTerraFormer (iTerraFormer* form)
    { 
      scfParent->SetTerraFormer (form); 
    }
    
    virtual iTerraFormer* GetTerraFormer ()
    { 
      return scfParent->GetTerraFormer (); 
    }
    
    virtual void SetSamplerRegion (const csBox2& region)
    { 
      scfParent->SetSamplerRegion (region); 
    }
    
    virtual const csBox2& GetSamplerRegion ()
    { 
      return scfParent->GetSamplerRegion (); 
    }

    virtual bool SaveState (const char *filename)
    {
      return true;
    }
    virtual bool RestoreState (const char *filename)
    {
      return true;
    }
  } scfiTerrainFactoryState;

  friend class eiTerrainFactoryState;

  //------------------------- iObjectModel implementation ----------------
  class eiObjectModel : public csObjectModel
  {
    SCF_DECLARE_EMBEDDED_IBASE (csTerrainFactory);
    virtual void GetObjectBoundingBox (csBox3& bbox, int type = CS_BBOX_NORMAL)
    {
      //scfParent->GetObjectBoundingBox (bbox, type);
    }
    virtual void GetRadius (csVector3& rad, csVector3& cent)
    {
      //scfParent->GetRadius (rad, cent);
    }
  } scfiObjectModel;
  friend class eiObjectModel;

  virtual iObjectModel* GetObjectModel () { return &scfiObjectModel; }
};

/**
* TerrFunc type. This is the plugin you have to use to create instances
* of csTerrainFactory.
*/
class csTerrainObjectType : public iMeshObjectType
{
private:
  iObjectRegistry *object_reg;

public:
  SCF_DECLARE_IBASE;

  /// Constructor.
  csTerrainObjectType (iBase*);
  /// Destructor.
  virtual ~csTerrainObjectType ();

  /// create a new factory.
  virtual csPtr<iMeshObjectFactory> NewFactory ();

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csTerrainObjectType);
    virtual bool Initialize (iObjectRegistry* p)
    { scfParent->object_reg = p; return true; }
  } scfiComponent;
  friend struct eiComponent;
};

#endif // __CS_TERRFUNC_H__
