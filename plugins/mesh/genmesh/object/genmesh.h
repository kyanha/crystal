/*
    Copyright (C) 2002 by Jorrit Tyberghein

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

#ifndef __CS_GENMESH_H__
#define __CS_GENMESH_H__

#include "csgeom/vector3.h"
#include "csgeom/vector4.h"
#include "csgeom/transfrm.h"
#include "csutil/cscolor.h"
#include "csutil/refarr.h"
#include "csutil/hashmap.h"
#include "csutil/hash.h"
#include "csutil/garray.h"
#include "imesh/object.h"
#include "imesh/genmesh.h"
#include "imesh/lighting.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "ivideo/graph3d.h"
#include "ivideo/vbufmgr.h"
#include "csgeom/objmodel.h"
#include "igeom/polymesh.h"
#include "iengine/shadcast.h"
#include "iengine/light.h"
#include "iengine/lightmgr.h"
#include "ivideo/rndbuf.h"
#include "ivideo/rendermesh.h"
#include "cstool/anonrndbuf.h"
#include "csgfx/shadervar.h"
#include "csgfx/shadervarcontext.h"

struct iMaterialWrapper;
struct iObjectRegistry;
struct iMovable;
struct iShadowBlockList;
struct iCacheManager;
struct iEngine;
class csGenmeshMeshObjectFactory;
class csGenmeshMeshObjectType;
class csGenmeshMeshObject;
class csColor;
class csPolygonMesh;
class G3DFogInfo;

/**
 * An array giving shadow information for a pseudo-dynamic light.
 */
class csShadowArray
{
public:
  iLight* light;
  csShadowArray* next;
  // For every vertex of the mesh a value.
  float* shadowmap;

  csShadowArray () : shadowmap (0) { }
  ~csShadowArray ()
  {
    delete[] shadowmap;
  }
};

/**
 * Genmesh version of mesh object.
 */
class csGenmeshMeshObject : public iMeshObject
{
private:
#ifdef CS_USE_NEW_RENDERER
  csRenderMesh mesh;
  csRenderMesh* meshPtr;
  csShaderVariableContext* svcontext;
  csRef<iGraphics3D> g3d;
  bool mesh_colors_dirty_flag;

  uint buffers_version;
  csRef<iRenderBuffer> color_buffer;
  iMovable* lighting_movable;
#endif
  csGenmeshMeshObjectFactory* factory;
  iBase* logparent;
  iMaterialWrapper* material;
  bool material_needs_visit;
  uint MixMode;
  iMeshObjectDrawCallback* vis_cb;
  bool do_lighting;
  bool do_manual_colors;
  csColor color;
  float current_lod;
  uint32 current_features;
  csFlags flags;

  bool do_shadows;
  bool do_shadow_rec;

  csColor* lit_mesh_colors;
  int num_lit_mesh_colors;	// Should be equal to factory number.
  csColor* static_mesh_colors;
  /// Dynamic ambient light assigned to this genmesh.
  csColor dynamic_ambient;
  uint32 ambient_version;
  csHash<csShadowArray*, iLight*> pseudoDynInfo;

  // If we are using the iLightingInfo lighting system then this
  // is an array of lights that affect us right now.
  csSet<iLight*> affecting_lights;
  // If the following flag is dirty then some of the affecting lights
  // has changed and we need to recalculate.
  bool lighting_dirty;

  // choose whether to draw shadow caps or not
  bool shadow_caps;

  bool initialized;

  /**
   * Camera space bounding box is cached here.
   * GetCameraBoundingBox() will check the current camera number from the
   * camera to see if it needs to recalculate this.
   */
  csBox3 camera_bbox;
  csBox3 world_bbox;

  /// Current camera number.
  long cur_cameranr;
  /// Current movable number.
  long cur_movablenr;

  /**
   * Clears out the pseudoDynInfo hash and frees the memory allocated by the
   * shadow maps.
   */
  void ClearPseudoDynLights ();

  /**
   * Setup this object. This function will check if setup is needed.
   */
  void SetupObject ();

  /**
   * Make sure the 'lit_mesh_colors' array has the right size.
   * Also clears the pseudo-dynamic light hash if the vertex count
   * changed!
   */
  void CheckLitColors ();

  /**
   * Process one light and add the values to the genmesh light table.
   * The given transform is the full movable transform.
   */
  void UpdateLightingOne (const csReversibleTransform& trans, iLight* light);

  /**
   * Update lighting using the iLightingInfo system.
   */
  void UpdateLighting2 (iMovable* movable);

  void UpdateLighting (const csArray<iLight*>& lights, iMovable* movable);
public:
  /// Constructor.
  csGenmeshMeshObject (csGenmeshMeshObjectFactory* factory);

  /// Destructor.
  virtual ~csGenmeshMeshObject ();

  uint GetMixMode () const { return MixMode; }
  void SetLighting (bool l) { do_lighting = l; }
  bool IsLighting () const { return do_lighting; }
  csColor GetColor () const { return color; }
  void SetManualColors (bool m) { do_manual_colors = m; }
  bool IsManualColors () const { return do_manual_colors; }
  void GetObjectBoundingBox (csBox3& bbox, int type = CS_BBOX_NORMAL);
  void GetRadius (csVector3& rad, csVector3& cent);

  //----------------------- Shadow and lighting system ----------------------
  char* GenerateCacheName ();
  void InitializeDefault (bool clear);
  bool ReadFromCache (iCacheManager* cache_mgr);
  bool WriteToCache (iCacheManager* cache_mgr);
  void PrepareLighting ();

  void SetDynamicAmbientLight (const csColor& color)
  {
    dynamic_ambient = color;
    lighting_dirty = true;
    ambient_version++;
  }

  void AppendShadows (iMovable* movable, iShadowBlockList* shadows,
    	const csVector3& origin);
  void CastShadows (iMovable* movable, iFrustumView* fview);
  void FinalizeLighting (iMovable* movable, iLight* light,
    const csArray<bool>& influences);
  void LightChanged (iLight* light);
  void LightDisconnect (iLight* light);

  //----------------------- iMeshObject implementation ----------------------
  SCF_DECLARE_IBASE;

  virtual iMeshObjectFactory* GetFactory () const
  {
    return (iMeshObjectFactory*)factory;
  }
  virtual csFlags& GetFlags () { return flags; }
  virtual bool DrawTest (iRenderView* rview, iMovable* movable);
  virtual bool Draw (iRenderView* rview, iMovable* movable, csZBufMode mode);
  virtual csRenderMesh** GetRenderMeshes (int &n);
  virtual void SetVisibleCallback (iMeshObjectDrawCallback* cb)
  {
    if (cb) cb->IncRef ();
    if (vis_cb) vis_cb->DecRef ();
    vis_cb = cb;
  }
  virtual iMeshObjectDrawCallback* GetVisibleCallback () const
  {
    return vis_cb;
  }
  virtual void NextFrame (csTicks /*current_time*/,
  	const csVector3& /*pos*/) { }
  virtual void HardTransform (const csReversibleTransform&) { }
  virtual bool SupportsHardTransform () const { return false; }
  virtual bool HitBeamOutline (const csVector3& start, const csVector3& end,
    csVector3& isect, float *pr);
  virtual bool HitBeamObject (const csVector3& start, const csVector3& end,
  	csVector3& isect, float* pr, int* polygon_idx = 0);
  virtual void SetLogicalParent (iBase* lp) { logparent = lp; }
  virtual iBase* GetLogicalParent () const { return logparent; }


  virtual iObjectModel* GetObjectModel ();
  virtual bool SetColor (const csColor& col) { color = col; return true; }
  virtual bool GetColor (csColor& col) const { col = color; return true; }
  virtual bool SetMaterialWrapper (iMaterialWrapper* mat);
  virtual iMaterialWrapper* GetMaterialWrapper () const { return material; }
  virtual void InvalidateMaterialHandles () { }
  /**
   * see imesh/object.h for specification. The default implementation
   * does nothing.
   */
  virtual void PositionChild (iMeshObject* child,csTicks current_time) { }

  //------------------------- iLightingInfo interface -------------------------
  struct LightingInfo : public iLightingInfo
  {
    SCF_DECLARE_EMBEDDED_IBASE (csGenmeshMeshObject);
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

  //-------------------- iShadowCaster interface implementation ----------
  struct ShadowCaster : public iShadowCaster
  {
    SCF_DECLARE_EMBEDDED_IBASE (csGenmeshMeshObject);
    virtual void AppendShadows (iMovable* movable, iShadowBlockList* shadows,
    	const csVector3& origin)
    {
      scfParent->AppendShadows (movable, shadows, origin);
    }
  } scfiShadowCaster;
  friend struct ShadowCaster;

  //-------------------- iShadowReceiver interface implementation ----------
  struct ShadowReceiver : public iShadowReceiver
  {
    SCF_DECLARE_EMBEDDED_IBASE (csGenmeshMeshObject);
    virtual void CastShadows (iMovable* movable, iFrustumView* fview)
    {
      scfParent->CastShadows (movable, fview);
    }
  } scfiShadowReceiver;
  friend struct ShadowReceiver;

  //------------------------- iGeneralMeshState implementation ----------------
  class GeneralMeshState : public iGeneralMeshState
  {
    SCF_DECLARE_EMBEDDED_IBASE (csGenmeshMeshObject);
    virtual void SetMaterialWrapper (iMaterialWrapper* material)
    {
      scfParent->material = material;
    }
    virtual iMaterialWrapper* GetMaterialWrapper () const
    { return scfParent->material; }
    virtual void SetMixMode (uint mode) { scfParent->MixMode = mode; }
    virtual uint GetMixMode () const { return scfParent->MixMode; }
    virtual void SetLighting (bool l) { scfParent->SetLighting (l); }
    virtual bool IsLighting () const { return scfParent->IsLighting (); }
    virtual void SetColor (const csColor& col) { scfParent->SetColor (col); }
    virtual csColor GetColor () const { return scfParent->GetColor (); }
    virtual void SetManualColors (bool m)
    {
      scfParent->SetManualColors (m);
    }
    virtual bool IsManualColors () const
    {
      return scfParent->IsManualColors ();
    }
    virtual void SetShadowCasting (bool m)
    {
      scfParent->do_shadows = m;
    }
    virtual bool IsShadowCasting () const
    {
      return scfParent->do_shadows;
    }
    virtual void SetShadowReceiving (bool m)
    {
      scfParent->do_shadow_rec = m;
    }
    virtual bool IsShadowReceiving () const
    {
      return scfParent->do_shadow_rec;
    }
  } scfiGeneralMeshState;
  friend class GeneralMeshState;

  //------------------ iPolygonMesh interface implementation ----------------//
  struct PolyMesh : public iPolygonMesh
  {
  private:
    csFlags flags;

  public:
    SCF_DECLARE_EMBEDDED_IBASE (csGenmeshMeshObject);

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
    {
      flags.Set (CS_POLYMESH_TRIANGLEMESH);
    }
    virtual ~PolyMesh () { }
  } scfiPolygonMesh;
  friend struct PolyMesh;

  //------------------ iShaderVariableAccessor implementation ------------
#ifdef CS_USE_NEW_RENDERER
  class eiShaderVariableAccessor : public iShaderVariableAccessor
  {
  public:
    SCF_DECLARE_EMBEDDED_IBASE (csGenmeshMeshObject);
    virtual void PreGetValue (csShaderVariable* variable)
    {
      scfParent->PreGetShaderVariableValue (variable);
    }
  } scfiShaderVariableAccessor;
  friend class eiShaderVariableAccessor;

  void PreGetShaderVariableValue (csShaderVariable* variable);
#endif // CS_USE_NEW_RENDERER
};

/**
 * Factory for general meshes.
 */
class csGenmeshMeshObjectFactory : public iMeshObjectFactory
{
private:
#ifndef CS_USE_NEW_RENDERER
  csRef<iVertexBuffer> vbuf;
  iVertexBufferManager* vbufmgr;
  G3DTriangleMesh top_mesh;
#endif
  iMaterialWrapper* material;
  csVector3* mesh_vertices;
  csVector2* mesh_texels;
  csVector3* mesh_normals;
  csColor* mesh_colors;
  int num_mesh_vertices;
  csVector3* mesh_tri_normals;

#ifdef CS_USE_NEW_RENDERER
  csTriangle* mesh_triangles;
  int num_mesh_triangles;

  bool mesh_vertices_dirty_flag;
  bool mesh_texels_dirty_flag;
  bool mesh_normals_dirty_flag;
  bool mesh_colors_dirty_flag;
  bool mesh_triangle_dirty_flag;

  bool autonormals;

  csRef<iGraphics3D> g3d;
  csRef<iStringSet> strings;

  csRef<iRenderBuffer> vertex_buffer;
  csRef<iRenderBuffer> texel_buffer;
  csRef<iRenderBuffer> normal_buffer;
  csRef<iRenderBuffer> color_buffer;
  csRef<iRenderBuffer> index_buffer;
  
  csAnonRenderBufferManager anon_buffers;
  csArray<csStringID> anon_buffer_names;
#endif

  csVector3 radius;
  csBox3 object_bbox;
  bool object_bbox_valid;
  bool initialized;

  csMeshedPolygon* polygons;

  /// Calculate bounding box and radius.
  void CalculateBBoxRadius ();

#ifndef CS_USE_NEW_RENDERER
  /// Retrieve a vertexbuffer from the manager if not done already.
  void SetupVertexBuffer ();
#endif

  /**
   * Compress vertices. This is for CalculateNormals().
   */
  bool CompressVertices (
	csVector3* orig_verts, int orig_num_vts,
	csVector3*& new_verts, int& new_num_vts,
	csTriangle* orig_tris, int num_tris,
	csTriangle*& new_tris,
	int*& mapping);

  /**
   * Setup this factory. This function will check if setup is needed.
   */
  void SetupFactory ();

#ifndef CS_USE_NEW_RENDERER
  /// interface to receive state of vertexbuffermanager
  struct eiVertexBufferManagerClient : public iVertexBufferManagerClient
  {
    SCF_DECLARE_EMBEDDED_IBASE (csGenmeshMeshObjectFactory);
    virtual void ManagerClosing ();
  }scfiVertexBufferManagerClient;
  friend struct eiVertexBufferManagerClient;
#endif

public:
  static csStringID vertex_name, texel_name, normal_name, color_name, 
    index_name;
  uint buffers_version;

  iObjectRegistry* object_reg;
  iBase* logparent;
  csGenmeshMeshObjectType* genmesh_type;
  csRef<iLightManager> light_mgr;
  csFlags flags;

  iEngine* engine;

  /// Constructor.
  csGenmeshMeshObjectFactory (iBase *pParent, iObjectRegistry* object_reg);

  /// Destructor.
  virtual ~csGenmeshMeshObjectFactory ();

  void SetMaterialWrapper (iMaterialWrapper* material)
  {
    csGenmeshMeshObjectFactory::material = material;
  }
  iMaterialWrapper* GetMaterialWrapper () const { return material; }
  void SetVertexCount (int n);
  int GetVertexCount () const { return num_mesh_vertices; }
  csVector3* GetVertices () { SetupFactory (); return mesh_vertices; }
  csVector2* GetTexels () { SetupFactory (); return mesh_texels; }
  csVector3* GetNormals () { SetupFactory (); return mesh_normals; }
  csColor* GetColors () { SetupFactory (); return mesh_colors; }
  void SetTriangleCount (int n);
#ifndef CS_USE_NEW_RENDERER
  int GetTriangleCount () const { return top_mesh.num_triangles; }
  csTriangle* GetTriangles () { SetupFactory (); return top_mesh.triangles; }
#else
  int GetTriangleCount () const { return num_mesh_triangles; }
  csTriangle* GetTriangles () { SetupFactory (); return mesh_triangles; }
#endif
  void Invalidate ();
  void CalculateNormals ();
  void GenerateBox (const csBox3& box);

  bool AddRenderBuffer (const char *name,
  	csRenderBufferComponentType component_type, int component_size);
  bool SetRenderBufferComponent (const char *name, int index,
  	int component, float value);
  bool SetRenderBufferComponent (const char *name, int index,
  	int component, int value);
  bool SetRenderBuffer (const char *name, float *value);
  bool SetRenderBuffer (const char *name, int *value);
#ifdef CS_USE_NEW_RENDERER
  /**
   * Get the string ID's for the anonymous buffers
   */
  const csArray<csStringID> &GetAnonymousNames ()
  { return anon_buffer_names; }
#endif

  const csBox3& GetObjectBoundingBox ();
  const csVector3& GetRadius ();

  /**
   * Calculate polygons for iPolygonMesh.
   */
  csMeshedPolygon* GetPolygons ();
#ifndef CS_USE_NEW_RENDERER
  iVertexBufferManager* GetVertexBufferManager ()
  {
    SetupFactory ();
    return vbufmgr;
  }
  iVertexBuffer* GetVertexBuffer ()
  {
    SetupFactory ();
    return vbuf;
  }
  G3DTriangleMesh& GetMesh ()
  {
    SetupFactory ();
    return top_mesh;
  }
#endif

  //------------------------ iMeshObjectFactory implementation --------------
  SCF_DECLARE_IBASE;

  virtual csFlags& GetFlags () { return flags; }
  virtual csPtr<iMeshObject> NewInstance ();
  virtual void HardTransform (const csReversibleTransform& t);
  virtual bool SupportsHardTransform () const { return true; }
  virtual void SetLogicalParent (iBase* lp) { logparent = lp; }
  virtual iBase* GetLogicalParent () const { return logparent; }

  //----------------------- iGeneralFactoryState implementation -------------
  class GeneralFactoryState : public iGeneralFactoryState
  {
    SCF_DECLARE_EMBEDDED_IBASE (csGenmeshMeshObjectFactory);
    virtual void SetMaterialWrapper (iMaterialWrapper* material)
    {
      scfParent->SetMaterialWrapper (material);
    }
    virtual iMaterialWrapper* GetMaterialWrapper () const
    {
      return scfParent->GetMaterialWrapper ();
    }
    virtual void SetVertexCount (int n)
    {
      scfParent->SetVertexCount (n);
    }
    virtual int GetVertexCount () const
    {
      return scfParent->GetVertexCount ();
    }
    virtual csVector3* GetVertices ()
    {
      return scfParent->GetVertices ();
    }
    virtual csVector2* GetTexels ()
    {
      return scfParent->GetTexels ();
    }
    virtual csVector3* GetNormals ()
    {
      return scfParent->GetNormals ();
    }
    virtual void SetTriangleCount (int n)
    {
      scfParent->SetTriangleCount (n);
    }
    virtual int GetTriangleCount () const
    {
      return scfParent->GetTriangleCount ();
    }
    virtual csTriangle* GetTriangles ()
    {
      return scfParent->GetTriangles ();
    }
    virtual csColor* GetColors ()
    {
      return scfParent->GetColors ();
    }
    virtual void Invalidate ()
    {
      scfParent->Invalidate ();
    }
    virtual void CalculateNormals ()
    {
      scfParent->CalculateNormals ();
    }
    virtual void GenerateBox (const csBox3& box)
    {
      scfParent->GenerateBox (box);
    }
    virtual bool AddRenderBuffer (const char *name, 
      csRenderBufferComponentType component_type, int component_size)
    {
      return scfParent->AddRenderBuffer (name, component_type, component_size);
    }
    virtual bool SetRenderBufferComponent (const char *name, int index,
      int comp, float value)
    {
      return scfParent->SetRenderBufferComponent (name, index, comp, value);
    }
    virtual bool SetRenderBufferComponent (const char *name, int index,
      int comp, int value)
    {
      return scfParent->SetRenderBufferComponent (name, index, comp, value);
    }
    virtual bool SetRenderBuffer (const char *name, float *value) 
    {
      return scfParent->SetRenderBuffer (name, value);
    }
    virtual bool SetRenderBuffer (const char *name, int *value) 
    {
      return scfParent->SetRenderBuffer (name, value);
    }
  } scfiGeneralFactoryState;
  friend class GeneralFactoryState;

  //------------------ iPolygonMesh interface implementation ----------------//
  struct PolyMesh : public iPolygonMesh
  {
  private:
    csGenmeshMeshObjectFactory* factory;
    csFlags flags;
  public:
    //SCF_DECLARE_EMBEDDED_IBASE (csGenmeshMeshObjectFactory);
    SCF_DECLARE_IBASE;

    void SetFactory (csGenmeshMeshObjectFactory* Factory)
    { factory = Factory; }

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
    {
      SCF_CONSTRUCT_IBASE (0);
      flags.Set (CS_POLYMESH_TRIANGLEMESH);
    }
    virtual ~PolyMesh ()
    {
      SCF_DESTRUCT_IBASE ();
    }
  } scfiPolygonMesh;
  friend struct PolyMesh;

  //------------------------- iObjectModel implementation ----------------
  class ObjectModel : public csObjectModel
  {
    SCF_DECLARE_EMBEDDED_IBASE (csGenmeshMeshObjectFactory);
    virtual void GetObjectBoundingBox (csBox3& bbox, int type = CS_BBOX_NORMAL)
    {
      bbox = scfParent->GetObjectBoundingBox ();
    }
    virtual void GetRadius (csVector3& rad, csVector3& cent)
    {
      rad = scfParent->GetRadius ();
      cent.Set (0);
    }
  } scfiObjectModel;
  friend class ObjectModel;

  virtual iObjectModel* GetObjectModel () { return &scfiObjectModel; }

  //------------------ iShaderVariableAccessor implementation ------------
#ifdef CS_USE_NEW_RENDERER
  class eiShaderVariableAccessor : public iShaderVariableAccessor
  {
  public:
    SCF_DECLARE_EMBEDDED_IBASE (csGenmeshMeshObjectFactory);
    virtual void PreGetValue (csShaderVariable* variable)
    {
      scfParent->PreGetShaderVariableValue (variable);
    }
  } scfiShaderVariableAccessor;
  friend class eiShaderVariableAccessor;

  void PreGetShaderVariableValue (csShaderVariable* variable);
#endif // CS_USE_NEW_RENDERER
};

/**
 * Genmesh type. This is the plugin you have to use to create instances
 * of csGenmeshMeshObjectFactory.
 */
class csGenmeshMeshObjectType : public iMeshObjectType
{
public:
  iObjectRegistry* object_reg;
  bool do_verbose;

  SCF_DECLARE_IBASE;

  /// Constructor.
  csGenmeshMeshObjectType (iBase*);
  /// Destructor.
  virtual ~csGenmeshMeshObjectType ();
  /// Draw.
  virtual csPtr<iMeshObjectFactory> NewFactory ();
  /// Initialize.
  bool Initialize (iObjectRegistry* object_reg);

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csGenmeshMeshObjectType);
    virtual bool Initialize (iObjectRegistry* object_reg)
    {
      return scfParent->Initialize (object_reg);
    }
  } scfiComponent;
};

#endif // __CS_GENMESH_H__

