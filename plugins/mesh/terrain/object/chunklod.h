/*
    Copyright (C) 2003 by Jorrit Tyberghein, Daniel Duhprey

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

#ifndef __CS_CHUNKLOD_H__
#define __CS_CHUNKLOD_H__

#include "csutil/cscolor.h"
#include "csutil/garray.h"
#include "csutil/weakref.h"

#include "csgeom/transfrm.h"
#include "csgeom/objmodel.h"

#include "imesh/object.h"
#include "imesh/terrain.h"
#include "imesh/lighting.h"

#include "iutil/comp.h"

#include "ivideo/rendermesh.h"

#include "iengine/lightmgr.h"
#include "iengine/shadcast.h"

#include "csgfx/shadervarcontext.h"

#include "cstool/rendermeshholder.h"

struct iMaterialWrapper;
struct iImage;
struct iGraphics3D;

class csChunkLodTerrainType;
class csChunkLodTerrainFactory;
class csChunkLodTerrainObject;

/**
 * ChunkLod terrain type, instantiates factories which create meshes
 */
class csChunkLodTerrainType : public iMeshObjectType
{
private:
  iObjectRegistry *object_reg;
  iBase* parent;

public:
  SCF_DECLARE_IBASE;	

  csChunkLodTerrainType (iBase* p);
  virtual ~csChunkLodTerrainType ();

  csPtr<iMeshObjectFactory> NewFactory();

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE (csChunkLodTerrainType);
    bool Initialize (iObjectRegistry* p)
    { scfParent->object_reg = p; return true; }
  } scfiComponent;
  friend struct eiComponent;
};


/**
 * The factory will keep track of the precomputed quad-tree hierarchy
 * each individual instance of the terrain (if there are more than one)
 * will determine which nodes in the tree are meant to be rendered 
 * (that determination is made during drawtest)
 */
class csChunkLodTerrainFactory : public iMeshObjectFactory
{
  friend class csChunkLodTerrainObject;
public:
  iBase* parent;
  iObjectRegistry *object_reg;
  csFlags flags;
  csRef<iEngine> engine;
  csRef<iLightManager> light_mgr;

  csVector3 scale;
  struct Data
  {
    csVector3 pos;
    csVector3 norm;
    csVector3 tan;
    csVector3 bin;
    csVector2 tex;
    int col;
    float error;
    Data ()
    {
      pos.Set (0);
      norm.Set (0);
      tan.Set (0);
      bin.Set (0);
      tex.Set (0, 0);
      col = 0;
      error = 0.0;
    }
  };
  csArray<Data> datamap;
  int hm_x, hm_y;

  csStringID vertex_name, compressed_vertex_name; 
  csStringID normal_name, compressed_normal_name;
  csStringID tangent_name, compressed_tangent_name;
  csStringID binormal_name, compressed_binormal_name;
  csStringID texcors_name, compressed_texcors_name, texcoords_norm_name;
  csStringID color_name, compressed_color_name;
  csStringID index_name;
  csRef<iGraphics3D> r3d;
  csRef<iShaderManager> shmgr;

  class MeshTreeNode
  {
  private:
    csChunkLodTerrainFactory *pFactory;
    MeshTreeNode *children[4];

    csVector3 center;
    csBox3 box;
    float radius;
    float error;

    csDirtyAccessArray<csVector3> vertices;
    csRef<iRenderBuffer> vertex_buffer;
    csRef<iRenderBuffer> compressed_vertex_buffer;
    csDirtyAccessArray<csVector3> normals;
    csRef<iRenderBuffer> normal_buffer;
    csRef<iRenderBuffer> compressed_normal_buffer;
    csArray<csVector3> tangents;
    csRef<iRenderBuffer> tangent_buffer;
    csRef<iRenderBuffer> compressed_tangent_buffer;
    csArray<csVector3> binormals;
    csRef<iRenderBuffer> binormal_buffer;
    csRef<iRenderBuffer> compressed_binormal_buffer;
    csDirtyAccessArray<csVector2> texcors;
    csRef<iRenderBuffer> texcors_buffer;
    csRef<iRenderBuffer> compressed_texcors_buffer;
    csRef<iRenderBuffer> texcoords_norm_buffer;
    csRef<iRenderBuffer> color_buffer;
    csRef<iRenderBuffer> compressed_color_buffer;
    csRef<iRenderBuffer> index_buffer;
    
    int parity;
    int max_levels;

    void InitBuffer (const Data &d, int p);
    void AddVertex (const Data &d, int p);
    void EndBuffer (const Data &d, int p);
    void AddEdgeVertex (const Data& d, const Data& mod);
    void AddSkirtVertex (const Data& d, const Data& mod);
    void ProcessMap (int l, int i, int j, int k);
    void ProcessEdge (int start, int end, int move, const Data& mod);
    
  public:
    csArray<int> colors;
  
    MeshTreeNode (csChunkLodTerrainFactory* p, int x, int y, int w, int h,
    	float error);
    ~MeshTreeNode ();

    MeshTreeNode *GetChild (int i) 
    { CS_ASSERT (i >= 0 && i < 4); return (error > 0) ? children[i] : 0; }

    iRenderBuffer *GetRenderBuffer (csStringID name);

    const csVector3 &Center () { return center; }
    const csBox3 &BBox () { return box; }
    float Radius () { return radius; }
    float Error () { return error; }

    int Count () { return vertices.Length(); }

    const csVector3* GetVertices() const { return vertices.GetArray(); }
    const csVector3* GetNormals() const { return normals.GetArray(); }
  } *root;
  friend class MeshTreeNode;

  void ComputeError (int i, int j, int di, int dj, int n, int w);

public:
  SCF_DECLARE_IBASE;

  csChunkLodTerrainFactory (csChunkLodTerrainType* p, iObjectRegistry* objreg);
  virtual ~csChunkLodTerrainFactory ();

  virtual csFlags& GetFlags () { return flags; }
  virtual csPtr<iMeshObject> NewInstance ();
  virtual void HardTransform (const csReversibleTransform&) { }
  virtual bool SupportsHardTransform () const { return false; }
  virtual void SetLogicalParent (iBase* lp) { parent = lp; }
  virtual iBase* GetLogicalParent () const { return parent; }
  virtual iObjectModel* GetObjectModel () { return &scfiObjectModel; }

  void GetObjectBoundingBox (csBox3& bbox, int type = CS_BBOX_NORMAL);
  void GetRadius (csVector3& rad, csVector3& c);

  struct eiObjectModel : public csObjectModel
  {
    SCF_DECLARE_EMBEDDED_IBASE (csChunkLodTerrainFactory);
    virtual void GetObjectBoundingBox (csBox3& b, int t = CS_BBOX_NORMAL)
    { scfParent->GetObjectBoundingBox (b, t); }
    virtual void GetRadius (csVector3& r, csVector3& c)
    { scfParent->GetRadius (r, c); }
  } scfiObjectModel;
  friend struct eiObjectModel;

  void SetScale (const csVector3& scale);
  const csVector3& GetScale () const { return scale; }
  bool SetHeightMap (const csArray<float>& data, int x, int y);
  bool SetHeightMap (iImage* map);
  bool SaveState (const char *filename);
  bool RestoreState (const char *filename);

  struct eiTerrainFactoryState : public iTerrainFactoryState
  {
    SCF_DECLARE_EMBEDDED_IBASE (csChunkLodTerrainFactory);
    virtual void SetScale (const csVector3& scale)
    { scfParent->SetScale (scale); }
    virtual const csVector3& GetScale () const
    { return scfParent->GetScale (); }
    virtual bool SetHeightMap (const csArray<float>& data, int x, int y)
    { return scfParent->SetHeightMap (data, x, y); }
    virtual bool SetHeightMap (iImage* map)
    { return scfParent->SetHeightMap (map); }
    virtual bool SaveState (const char *filename)
    { return scfParent->SaveState (filename); }
    virtual bool RestoreState (const char *filename) 
    { return scfParent->RestoreState (filename); }
  } scfiTerrainFactoryState;
  friend struct eiTerrainFactoryState;

  csVector3 CollisionDetect (const csVector3 &p);
};


/**
 * An array giving shadow information for a pseudo-dynamic light.
 */
class csShadowArray
{
public:
  iLight* light;
  // For every vertex of the mesh a value.
  float* shadowmap;

  csShadowArray () : shadowmap (0) { }
  ~csShadowArray ()
  {
    delete[] shadowmap;
  }
};

/**
 * Instance of an implementation of Thatcher Ulritch's Chunked LOD algorithm
 * for terrain rendering.  http://www.tulrich.com/geekstuff/chunklod.html
 * The factory is responsible for the preprocessing step on the quad-tree
 * hierarchy and each instance (this class) is responsible for determining
 * which of those nodes in the tree should be rendered (draw_test) and render
 * them (GetRenderMeshes)
 */
class csChunkLodTerrainObject : public iMeshObject
{
private:
  iBase* logparent;
  csChunkLodTerrainFactory* pFactory;
  iMeshObjectDrawCallback* vis_cb;

  csColor basecolor;
  csRef<iMaterialWrapper> matwrap;
  csArray<iMaterialWrapper*> palette;
  csRefArray<iImage> alphas;

  csDirtyAccessArray<csColor> staticLights;
  uint colorVersion;
  csColor dynamic_ambient;
  // If we are using the iLightingInfo lighting system then this
  // is an array of lights that affect us right now.
  csSet<iLight*> affecting_lights;
  csHash<csShadowArray*, iLight*> pseudoDynInfo;

  float error_tolerance;
  float lod_distance;

  csRenderMeshHolderSingle rmHolder;
  csRenderMeshHolderMultiple returnMeshesHolder;
  csDirtyAccessArray<csRenderMesh*>* returnMeshes;
  csReversibleTransform tr_o2c;

  // Use for clipping during rendering.
  csPlane3 planes[10];

  int tricount;
  csFlags flags;
  bool staticLighting;

  class MeshTreeNodeWrapper : public iBase
  {
    csRef<MeshTreeNodeWrapper> children[4];
  public:
    csWeakRef<csChunkLodTerrainObject> obj;
    csChunkLodTerrainFactory::MeshTreeNode* factoryNode;
    csRef<csShaderVariableContext> svcontext;

    SCF_DECLARE_IBASE;

    MeshTreeNodeWrapper (csChunkLodTerrainObject* obj,
      csChunkLodTerrainFactory::MeshTreeNode* node);
    virtual ~MeshTreeNodeWrapper();
    MeshTreeNodeWrapper* GetChild (int n);
  };
  class MeshTreeNodeSVA : public iShaderVariableAccessor
  {
    csWeakRef<MeshTreeNodeWrapper> wrapper;
    csRef<iRenderBuffer> colorBuffer;
    uint colorVersion;
  public:
    SCF_DECLARE_IBASE;

    MeshTreeNodeSVA (MeshTreeNodeWrapper* wrapper);
    virtual ~MeshTreeNodeSVA ();

    virtual void PreGetValue (csShaderVariable *variable);
  };
  
  csRef<MeshTreeNodeWrapper> rootNode;
  iMovable* light_movable; 
  void UpdateColors (const csArray<int>& colors, const csVector3* vertices, 
    const csVector3* normals, const csBox3& box, csColor* staticColors);
public: 
  SCF_DECLARE_IBASE;

  csChunkLodTerrainObject (csChunkLodTerrainFactory* f);
  virtual ~csChunkLodTerrainObject ();

  /// Returns a point to the factory that made this
  iMeshObjectFactory* GetFactory () const
  {
    return (iMeshObjectFactory*)pFactory;
  }

  virtual csFlags& GetFlags () { return flags; }

  /**
   * Does all pre-render calculation.  Determines which LOD children in the 
   * tree should be drawn
   */
  bool DrawTestQuad (iRenderView* rv, MeshTreeNodeWrapper* node
	/*csChunkLodTerrainFactory::MeshTreeNode* node*/, float kappa,
	uint32 frustum_mask);
  bool DrawTest (iRenderView* rview, iMovable* movable);

  bool Draw (iRenderView*, iMovable*, csZBufMode) 
  { /* deprecated */ return false; }

  /// Returns the mesh, ready for rendering
  virtual csRenderMesh** GetRenderMeshes (int &n, iRenderView* rview,
  	iMovable* movable, uint32 frustum_mask);

  virtual void SetVisibleCallback (iMeshObjectDrawCallback* cb) { vis_cb = cb; }
  virtual iMeshObjectDrawCallback* GetVisibleCallback () const
  { return vis_cb; }

  /// For animation ... ha ha
  virtual void NextFrame (csTicks, const csVector3&) { }

  /// Unsupported
  virtual void HardTransform (const csReversibleTransform&) { }

  /// Shows that HardTransform is not supported by this mesh
  virtual bool SupportsHardTransform () const { return false; }

  /// Check if the terrain is hit by the beam
  virtual bool HitBeamOutline (const csVector3& start, const csVector3& end, 
	csVector3& isect, float* pr);
  /// Find exact position of a beam hit
  virtual bool HitBeamObject (const csVector3& start, const csVector3& end, 
	csVector3& isect, float* pr, int* polygon_idx = 0);

  /// Set/Get logical parent
  virtual void SetLogicalParent (iBase* lp) { logparent = lp; }
  virtual iBase* GetLogicalParent () const { return logparent; }

  /// Gets the objects model, not sure what this means yet
  virtual iObjectModel *GetObjectModel () { return &scfiObjectModel; }

  /// Set (Get) the terrain to a constant base color 
  virtual bool SetColor (const csColor& c) { basecolor = c; return true; }
  virtual bool GetColor (csColor &c) const { c = basecolor; return true; }

  /** 
   * Set (Get) the terrain to a single material, useful only with 
   * large textures or small terrains (or terrains in the distance)
   * See TerrainState for better texture settings
   */
  virtual bool SetMaterialWrapper (iMaterialWrapper* m)
  { matwrap = m; return true; }
  virtual iMaterialWrapper* GetMaterialWrapper () const { return matwrap; }
  virtual void InvalidateMaterialHandles () { }
  /**
   * see imesh/object.h for specification. The default implementation
   * does nothing.
   */
  virtual void PositionChild (iMeshObject* child,csTicks current_time) { }

  bool SetMaterialPalette (const csArray<iMaterialWrapper*>& pal);
  const csArray<iMaterialWrapper*>& GetMaterialPalette () const
  {
    return palette;
  }
  bool SetMaterialMap (const csArray<char>& data, int x, int y);
  bool SetMaterialMap (iImage* map);
  void SetLODDistance (float distance) { lod_distance = distance; }
  float GetLODDistance () const { return lod_distance; }
  void SetErrorTolerance (float error) { error_tolerance = error; }
  float GetErrorTolerance () const { return error_tolerance; }
  /// Saves the texture quad-tree into the file specified
  bool SaveState (const char *filename);
  bool RestoreState (const char *filename);
  int CollisionDetect (iMovable *m, csTransform *p);
  void SetStaticLighting (bool enable)
  { staticLighting = enable; }
  bool GetStaticLighting ()
  { return staticLighting; }

  char* GenerateCacheName ();
  void InitializeDefault (bool clear);
  bool ReadFromCache (iCacheManager* cache_mgr);
  bool WriteToCache (iCacheManager* cache_mgr);
  void PrepareLighting ();
  void SetDynamicAmbientLight (const csColor& color);
  const csColor& GetDynamicAmbientLight ();
  void LightChanged (iLight* light);
  void LightDisconnect (iLight* light);
  virtual void CastShadows (iMovable* movable, iFrustumView* fview);

  struct eiTerrainObjectState : public iTerrainObjectState
  {
    SCF_DECLARE_EMBEDDED_IBASE (csChunkLodTerrainObject);
    virtual bool SetMaterialPalette (const csArray<iMaterialWrapper*>& pal) 
    { return scfParent->SetMaterialPalette (pal); }
    virtual const csArray<iMaterialWrapper*>& GetMaterialPalette () const
    { return scfParent->GetMaterialPalette (); }
    virtual bool SetMaterialMap (const csArray<char>& data, int x, int y)
    { return scfParent->SetMaterialMap (data, x, y); }
    virtual bool SetMaterialMap (iImage* map)
    { return scfParent->SetMaterialMap (map); }
    virtual void SetLODDistance (float distance)
    { scfParent->SetLODDistance (distance); }
    virtual float GetLODDistance () const
    { return scfParent->GetLODDistance (); }
    virtual void SetErrorTolerance (float error)
    { scfParent->SetErrorTolerance (error); }
    virtual float GetErrorTolerance () const
    { return scfParent->GetErrorTolerance (); }
    virtual bool SaveState (const char *filename)
    { return scfParent->SaveState (filename); }
    virtual bool RestoreState (const char *filename) 
    { return scfParent->RestoreState (filename); }
    virtual int CollisionDetect (iMovable *m, csTransform *p)
    { return scfParent->CollisionDetect (m, p); }
    virtual void SetStaticLighting (bool enable)
    { scfParent->SetStaticLighting (enable); }
    virtual bool GetStaticLighting ()
    { return scfParent->GetStaticLighting (); }
  } scfiTerrainObjectState;
  friend struct eiTerrainObjectState;

  struct eiObjectModel : public csObjectModel
  {
    SCF_DECLARE_EMBEDDED_IBASE (csChunkLodTerrainObject);
    virtual void GetObjectBoundingBox (csBox3& b, int t = CS_BBOX_NORMAL)
    { scfParent->pFactory->GetObjectBoundingBox (b, t); }
    virtual void GetRadius (csVector3& r, csVector3& c)
    { scfParent->pFactory->GetRadius (r, c); }
  } scfiObjectModel;
  friend struct eiObjectModel;

  //-------------------- iShadowReceiver interface implementation ----------
  struct ShadowReceiver : public iShadowReceiver
  {
    SCF_DECLARE_EMBEDDED_IBASE (csChunkLodTerrainObject);
    virtual void CastShadows (iMovable* movable, iFrustumView* fview)
    {
      scfParent->CastShadows (movable, fview);
    }
  } scfiShadowReceiver;
  friend struct ShadowReceiver;

  //------------------------- iLightingInfo interface -------------------------
  struct LightingInfo : public iLightingInfo
  {
    SCF_DECLARE_EMBEDDED_IBASE (csChunkLodTerrainObject);
    virtual void InitializeDefault (bool clear)
    {
      scfParent->InitializeDefault (clear);
    }
    virtual bool ReadFromCache (iCacheManager* cache_mgr)
    {
      return scfParent->ReadFromCache (cache_mgr);
    }
    virtual bool WriteToCache (iCacheManager* cache_mgr)
    {
      return scfParent->WriteToCache (cache_mgr);
    }
    virtual void PrepareLighting ()
    {
      scfParent->PrepareLighting ();
    }
    virtual void SetDynamicAmbientLight (const csColor& color)
    {
      scfParent->SetDynamicAmbientLight (color);
    }
    virtual const csColor& GetDynamicAmbientLight ()
    {
      return scfParent->dynamic_ambient;
    }
    virtual void LightChanged (iLight* light)
    {
      scfParent->LightChanged (light);
    }
    virtual void LightDisconnect (iLight* light)
    {
      scfParent->LightDisconnect (light);
    }
  } scfiLightingInfo;
  friend struct LightingInfo;
};

#endif // __CS_CHNKLOD_H__
