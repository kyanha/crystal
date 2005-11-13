/*
    Copyright (C) 2003 by Keith Fulton

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

#ifndef __CS_SPRCAL3D_H__
#define __CS_SPRCAL3D_H__

#include "csgeom/box.h"
#include "csgeom/math2d.h"
#include "csgeom/math3d.h"
#include "csgeom/objmodel.h"
#include "csgeom/poly2d.h"
#include "csgeom/poly3d.h"
#include "cstool/rendermeshholder.h"
#include "csutil/cscolor.h"
#include "csutil/dirtyaccessarray.h"
#include "csutil/flags.h"
#include "csutil/hash.h"
#include "csutil/hashr.h"
#include "csutil/leakguard.h"
#include "csutil/parray.h"
#include "csutil/refarr.h"
#include "csutil/sysfunc.h"
#include "csutil/weakref.h"
#include "iengine/light.h"
#include "iengine/lightmgr.h"
#include "iengine/lod.h"
#include "iengine/material.h"
#include "iengine/mesh.h"
#include "imesh/lighting.h"
#include "imesh/object.h"
#include "imesh/spritecal3d.h"
#include "iutil/comp.h"
#include "iutil/eventh.h"
#include "iutil/virtclk.h"
#include "ivideo/graph3d.h"
#include "ivideo/material.h"
#include "ivideo/rendermesh.h"
#include "ivideo/rndbuf.h"

// Hack: work around problems caused by #defining 'new'
#if defined(CS_EXTENSIVE_MEMDEBUG) || defined(CS_MEMORY_TRACKER)
# undef new
#endif
#include <new>

#include <cal3d/cal3d.h>

struct iObjectRegistry;
struct iEngine;
struct iMaterialWrapper;
class csSpriteCal3DMeshObjectType;

#define ALL_LOD_FEATURES (CS_LOD_TRIANGLE_REDUCTION|CS_LOD_DISTANCE_REDUCTION)

struct csCal3DAnimation
{
  int      index;
  csString name;
  int	   type;
  float    base_velocity;
  float    min_velocity;
  float    max_velocity;
  int      min_interval;
  int      max_interval;
  int      idle_pct;
  bool     lock;
};

struct csCal3DMesh
{
  CS_LEAKGUARD_DECLARE (csCal3DMesh);
  int	      index;
  csString    name;
  bool	      attach_by_default;
  csRef<iMaterialWrapper> default_material;
  csArray<csString> morph_target_name;
};

/**
 * A socket for specifying where sprites can plug into
 * other sprites.
 */
class csSpriteCal3DSocket : public iSpriteCal3DSocket
{
private:
  char* name;
  int triangle_index;
  int submesh_index;
  int mesh_index;

  // following are for the primary attached mesh
  iMeshWrapper *attached_mesh;
  csReversibleTransform attached_mesh_trans;

  // secondary meshes
  struct csSpriteCal3DSocketMesh
  {
    csSpriteCal3DSocketMesh(iMeshWrapper * m, csReversibleTransform t)
        : mesh(m), trans(t)
    {
    }

    csRef<iMeshWrapper> mesh;
    csReversibleTransform trans;
  };
  csArray<csSpriteCal3DSocketMesh> secondary_meshes;

public:

  /// Default Constructor
  csSpriteCal3DSocket();

  virtual ~csSpriteCal3DSocket ();

  /// Set the name.
  virtual void SetName (char const*);
  /// Get the name.
  virtual char const* GetName () const { return name; }

  /// Set the attached primary mesh.
  virtual void SetMeshWrapper (iMeshWrapper* mesh);
  /// Get the attached primary mesh.
  virtual iMeshWrapper* GetMeshWrapper () const {return attached_mesh;}

  /// Set the index of the triangle for the socket.
  virtual void SetTriangleIndex (int tri_index)
  { triangle_index = tri_index; }
  /// Get the index of the triangle for the socket.
  virtual int GetTriangleIndex () const { return triangle_index; }

  /// Set the index of the submesh for the socket.
  virtual void SetSubmeshIndex (int subm_index)
  { submesh_index = subm_index;}
  /// Get the index of the submesh for the socket.
  virtual int GetSubmeshIndex () const { return submesh_index;}

  /// Set the index of the mesh for the socket.
  virtual void SetMeshIndex (int m_index) { mesh_index = m_index;}
  /// Get the index of the mesh for the socket.
  virtual int GetMeshIndex () const {return mesh_index;}

  /// Set the transform of the primary mesh
  virtual void SetTransform (const csReversibleTransform & trans)
  { attached_mesh_trans = trans; }
  /// Get the transform of the primary mesh
  virtual csReversibleTransform GetTransform () const
  { return attached_mesh_trans; }

  /**
   * Get a count of the secondary attached meshes (this doesn't include the
   * primary mesh)
   */
  virtual size_t GetSecondaryCount () const
  { return (int)secondary_meshes.Length(); }
  /// Get the attached secondary mesh at the given index
  virtual iMeshWrapper * GetSecondaryMesh (size_t index)
  { return secondary_meshes[index].mesh; }
  /// Get the transform of the attached secondary mesh at the given index
  virtual csReversibleTransform GetSecondaryTransform (size_t index)
  { return secondary_meshes[index].trans; }
  /// Set the transform of the attached secondary mesh at the given index
  virtual void SetSecondaryTransform (size_t index,
    csReversibleTransform trans) { secondary_meshes[index].trans = trans; }
  /// Attach a secondary mesh
  virtual size_t AttachSecondary (iMeshWrapper* mesh,
    csReversibleTransform trans);
  /// Detach a secondary mesh
  virtual void DetachSecondary (const char* mesh_name);
  virtual void DetachSecondary (size_t index);
  /// Searches for the index of the given attached secondary mesh
  virtual size_t FindSecondary (const char* mesh_name);

  SCF_DECLARE_IBASE;
};

class csSpriteCal3DMeshObject;

/**
 * A Cal3D sprite based on a triangle mesh with a single texture.  Animation is
 * done with frames.  This class represents a template from which a
 * csSpriteCal3D class can be made.
 */
class csSpriteCal3DMeshObjectFactory : public iMeshObjectFactory
{
private:
  friend class csSpriteCal3DMeshObject;

  /// Material handle as returned by iTextureManager.
  iMeshFactoryWrapper* logparent;
  
  csSpriteCal3DMeshObjectType* sprcal3d_type;

  /// If true then this factory has been initialized.
  bool initialized;

  /// This is the factory equivalent class in cal3d.
  CalCoreModel calCoreModel;
  csPDelArray<csCal3DAnimation> anims;
  csPDelArray<csCal3DMesh> meshes;
  csArray<csString> morph_animation_names;

  csString basePath;
  csPDelArray<csSpriteCal3DSocket> sockets;
  csFlags flags;

  struct MeshBuffers
  {
    csRef<iRenderBuffer> indexBuffer;
    csRef<iRenderBuffer> texcoordBuffer;
  };
  csHash<MeshBuffers, int> meshBuffers;
public:
  CS_LEAKGUARD_DECLARE (csSpriteCal3DMeshObjectFactory);

  iObjectRegistry* object_reg;
  iVirtualClock* vc;

  csWeakRef<iGraphics3D> g3d;
  csRef<iLightManager> light_mgr;

  /**
   * Reference to the engine (optional because sprites can also be
   * used for the isometric engine).
   */
  csWeakRef<iEngine> engine;
  
  /// Create the sprite template.
  csSpriteCal3DMeshObjectFactory (iMeshObjectType* pParent,
  	csSpriteCal3DMeshObjectType* type, iObjectRegistry* object_reg);
  /// Destroy the template.
  virtual ~csSpriteCal3DMeshObjectFactory ();

  void Report (int severity, const char* msg, ...);

  //------------------------iSpriteCal3dFactoryState implementation --------
  /// Create a new core object.
  bool Create(const char *name);
  void ReportLastError ();
  void SetLoadFlags(int flags);
  void SetBasePath(const char *path);
  void RescaleFactory(float factor);
  void CalculateAllBoneBoundingBoxes();

  bool LoadCoreSkeleton(iVFS *vfs,const char *filename);
  int  LoadCoreAnimation(iVFS *vfs,const char *filename,const char *name,
  	int type,float base_vel, float min_vel,float max_vel,int min_interval,
	int max_interval,int idle_pct, bool lock);
  int LoadCoreMesh(iVFS *vfs,const char *filename,const char *name,
  	bool attach,iMaterialWrapper *defmat);
  int LoadCoreMorphTarget(iVFS *vfs,int mesh_index,const char *filename,
  	const char *name);
  int AddMorphAnimation(const char *name);
  bool AddMorphTarget(int morphanimation_index,const char *mesh_name,
  	const char *morphtarget_name);
  bool AddCoreMaterial(iMaterialWrapper *mat);
  void BindMaterials();
  bool RegisterAnimCallback(const char *anim, CalAnimationCallback *callback,
    float min_interval);
  bool RemoveAnimCallback(const char *anim, CalAnimationCallback *callback);

  int GetMeshCount() { return (int)meshes.Length(); }
  int GetMorphAnimationCount() { return (int)morph_animation_names.Length(); }
  int GetMorphTargetCount(int mesh_id);
  const char *GetMeshName(int idx);
  int  FindMeshName(const char *meshName);  
  const char* GetDefaultMaterial( const char* meshName );  
  const char *GetMorphAnimationName(int idx);
  int  FindMorphAnimationName(const char *meshName);
  bool IsMeshDefault(int idx);

  void DefaultGetBuffer (int mesh, csRenderBufferHolder*, csRenderBufferName);

  /// Create and add a new socket to the sprite.
  csSpriteCal3DSocket* AddSocket ();
  /// find a named socket into the sprite.
  csSpriteCal3DSocket* FindSocket (const char * name) const;
  /// find a socked based on the sprite attached to it
  csSpriteCal3DSocket* FindSocket (iMeshWrapper *mesh) const;
  /// Query the number of sockets
  int GetSocketCount () const { return (int)sockets.Length (); }
  /// Query the socket number f
  csSpriteCal3DSocket* GetSocket (int f) const
  {
    return ((size_t)f < sockets.Length()) ?
      (csSpriteCal3DSocket*)sockets [f] : (csSpriteCal3DSocket*)0;
  }


  //------------------------ iMeshObjectFactory implementation --------------
  SCF_DECLARE_IBASE;

  virtual csFlags& GetFlags () { return flags; }
  virtual csPtr<iMeshObject> NewInstance ();
  virtual csPtr<iMeshObjectFactory> Clone () { return 0; }
  virtual void HardTransform (const csReversibleTransform& t);
  virtual bool SupportsHardTransform () const { return true; }
  virtual void SetMeshFactoryWrapper (iMeshFactoryWrapper* lp)
  { logparent = lp; }
  virtual iMeshFactoryWrapper* GetMeshFactoryWrapper () const
  { return logparent; }
  virtual iMeshObjectType* GetMeshObjectType () const 
  { return (iMeshObjectType*)sprcal3d_type; }

  //------------------ iPolygonMesh interface implementation ----------------//

  int GetVertexCount() { return 0; }
  csVector3* GetVertices () { return 0; }
  int GetTriangleCount () { return 0; }
  csMeshedPolygon* GetPolygons () { return 0; }
  csTriangle* GetTriangles () { return 0; }

  struct PolyMesh : public iPolygonMesh
  {
  private:
    csSpriteCal3DMeshObjectFactory* factory;
    csFlags flags;

  public:
    SCF_DECLARE_IBASE;

    void SetFactory (csSpriteCal3DMeshObjectFactory* Factory)
    {
      factory = Factory;
    }

    virtual int GetVertexCount ()
    {
      return factory->GetVertexCount ();
    }
    virtual csVector3* GetVertices ()
    {
      return factory->GetVertices ();
    }
    virtual int GetPolygonCount ()
    {
      return factory->GetTriangleCount ();
    }
    virtual csMeshedPolygon* GetPolygons ()
    {
      return factory->GetPolygons();
    }
    virtual int GetTriangleCount ()
    {
      return factory->GetTriangleCount ();
    }
    virtual csTriangle* GetTriangles ()
    {
      return factory->GetTriangles();
    }

    virtual void Lock () {}
    virtual void Unlock () {}

    virtual csFlags& GetFlags () { return flags; }
    virtual uint32 GetChangeNumber() const { return 0; }

    PolyMesh () : polygons (0)
    {
      SCF_CONSTRUCT_IBASE (0);
      flags.Set (CS_POLYMESH_TRIANGLEMESH);
    }
    virtual ~PolyMesh ()
    {
      Cleanup ();
      SCF_DESTRUCT_IBASE ();
    }
    void Cleanup () {} //  delete[] polygons; polygons = 0; }

    csMeshedPolygon* polygons;
  } scfiPolygonMesh;
  friend struct PolyMesh;

  //------------------------- iObjectModel implementation --------------------
  void GetObjectBoundingBox (csBox3& bbox, csVector3 *verts,int vertCount);
  void GetObjectBoundingBox (csBox3& bbox);
  void SetObjectBoundingBox (const csBox3& bbox);
  void GetRadius (csVector3& rad, csVector3& cent);

  class ObjectModel : public csObjectModel
  {
    SCF_DECLARE_EMBEDDED_IBASE (csSpriteCal3DMeshObjectFactory);
    virtual void GetObjectBoundingBox (csBox3& bbox)
    {
      scfParent->GetObjectBoundingBox (bbox);
    }
    virtual void SetObjectBoundingBox (const csBox3& bbox)
    {
      scfParent->SetObjectBoundingBox (bbox);
    }
    virtual void GetRadius (csVector3& rad, csVector3& cent)
    {
      scfParent->GetRadius (rad, cent);
    }
  } scfiObjectModel;
  friend class ObjectModel;

  virtual iObjectModel* GetObjectModel () { return &scfiObjectModel; }

  //------------------- iSpriteCal3DFactoryState implementation --------------
  struct SpriteCal3DFactoryState : public iSpriteCal3DFactoryState
  {
    SCF_DECLARE_EMBEDDED_IBASE (csSpriteCal3DMeshObjectFactory);

    virtual bool Create(const char *name)
    { return scfParent->Create(name); }

    virtual void SetLoadFlags(int flags)
    {
      scfParent->SetLoadFlags(flags);
    }

    virtual void ReportLastError ()
    { scfParent->ReportLastError(); }

    virtual void SetBasePath(const char *path)
    { scfParent->SetBasePath(path); }

    virtual void RescaleFactory(float factor)
    { scfParent->RescaleFactory(factor); }

    virtual bool LoadCoreSkeleton(iVFS *vfs, const char *filename)
    { return scfParent->LoadCoreSkeleton(vfs,filename); }

    virtual int LoadCoreAnimation(iVFS *vfs, const char *filename,
      const char *name, int type, float base_vel, float min_vel, float max_vel,
      int min_interval,int max_interval,int idle_pct, bool lock)
    { return scfParent->LoadCoreAnimation(vfs, filename, name, type, base_vel,
        min_vel, max_vel, min_interval, max_interval, idle_pct, lock); }

    virtual int LoadCoreMesh(iVFS *vfs, const char *filename, const char *name,
      bool attach,iMaterialWrapper *defmat)
    { return scfParent->LoadCoreMesh(vfs, filename, name, attach, defmat); }

    virtual int AddMorphAnimation(const char *name)
    { return scfParent->AddMorphAnimation(name); }
    
    virtual bool AddMorphTarget(int morphanimation_index,
      const char *mesh_name, const char *morphtarget_name)
    { return scfParent->AddMorphTarget(morphanimation_index, mesh_name,
        morphtarget_name); }
 
    virtual int LoadCoreMorphTarget(iVFS *vfs, int mesh_index,
      const char *filename,const char *name)
    { return scfParent->LoadCoreMorphTarget(vfs, mesh_index, filename,name); }
	    
    virtual bool AddCoreMaterial(iMaterialWrapper *mat)
    { return scfParent->AddCoreMaterial(mat); }

    virtual void CalculateAllBoneBoundingBoxes()
    { scfParent->CalculateAllBoneBoundingBoxes(); }

    virtual void BindMaterials()
    { scfParent->BindMaterials(); }

    virtual int  GetMeshCount()
    { return scfParent->GetMeshCount(); }

    virtual int GetMorphAnimationCount()
    { return scfParent->GetMorphAnimationCount();}

    virtual int GetMorphTargetCount(int mesh_id)
    { return scfParent->GetMorphTargetCount(mesh_id);}

    virtual const char *GetMeshName(int idx)
    { return scfParent->GetMeshName(idx); }

    virtual int  FindMeshName(const char *meshName)
    { return scfParent->FindMeshName(meshName); }
    
    virtual const char* GetDefaultMaterial( const char* meshName )
    { return scfParent->GetDefaultMaterial( meshName ); }
    
    virtual const char *GetMorphAnimationName(int idx)
    { return scfParent->GetMorphAnimationName(idx); }

    virtual int  FindMorphAnimationName(const char *meshName)
    { return scfParent->FindMorphAnimationName(meshName); }

    virtual bool IsMeshDefault(int idx)
    { return scfParent->IsMeshDefault(idx); }

    virtual iSpriteCal3DSocket* AddSocket ()
    {
      csRef<iSpriteCal3DSocket> ifr (
	  SCF_QUERY_INTERFACE_SAFE (scfParent->AddSocket (),
	    iSpriteCal3DSocket));
      return ifr;       // DecRef is ok here.
    }
    virtual iSpriteCal3DSocket* FindSocket (const char* name) const
    {
      csRef<iSpriteCal3DSocket> ifr (SCF_QUERY_INTERFACE_SAFE (
	    scfParent->FindSocket (name), iSpriteCal3DSocket));
      return ifr;       // DecRef is ok here.
    }
    virtual iSpriteCal3DSocket* FindSocket (iMeshWrapper* mesh) const
    {
      csRef<iSpriteCal3DSocket> ifr (SCF_QUERY_INTERFACE_SAFE (
	    scfParent->FindSocket (mesh), iSpriteCal3DSocket));
      return ifr;       // DecRef is ok here.
    }
    virtual int GetSocketCount () const
    {
      return scfParent->GetSocketCount ();
    }
    virtual iSpriteCal3DSocket* GetSocket (int f) const
    {
      csRef<iSpriteCal3DSocket> ifr (
	  SCF_QUERY_INTERFACE_SAFE (scfParent->GetSocket (f),
	    iSpriteCal3DSocket));
      return ifr;       // DecRef is ok here.
    }

    virtual CalCoreModel *GetCal3DCoreModel()
    {
      return &scfParent->calCoreModel;
    }
    
    virtual bool RegisterAnimCallback(const char *anim,
      CalAnimationCallback *callback,float min_interval)
    {
      return scfParent->RegisterAnimCallback(anim,callback,min_interval);
    }

    virtual bool RemoveAnimCallback(const char *anim,
      CalAnimationCallback *callback)
    {
      return scfParent->RemoveAnimCallback(anim,callback);
    }
  
  } scfiSpriteCal3DFactoryState;
  friend struct SpriteCal3DFactoryState;

  struct LODControl : public iLODControl
  {
    SCF_DECLARE_EMBEDDED_IBASE (csSpriteCal3DMeshObjectFactory);
    virtual void SetLOD (float, float)
    {
    }
    virtual void GetLOD (float& m, float& a) const
    {
      m = 0;
      a = 1;
    }
    virtual void SetLOD (iSharedVariable*, iSharedVariable*)
    {
    }
    virtual void GetLOD (iSharedVariable*& varm, iSharedVariable* &vara)
    {
      varm = 0;
      vara = 0;
    }
    virtual int GetLODPolygonCount (float /*lod*/) const
    {
      return 0;
    }
    virtual void GetLOD(iSharedVariable*& first,iSharedVariable*& second) const
    {
      first=0; second=0;
    }
  } scfiLODControl;
  friend struct LODControl;

};

/**
 * A Cal3D sprite based on a triangle mesh with a single texture.
 * Animation is done with frames (a frame may be controlled by
 * a skeleton).
 */
class csSpriteCal3DMeshObject : public iMeshObject
{
private:
  struct ActiveAnim
  {
    csCal3DAnimation* anim;
    float weight;
  };

  iObjectRegistry* object_reg;
  iMeshObjectDrawCallback* vis_cb;
  uint32 current_features;  // LOD Control thing
  iMeshWrapper* logparent;
  CalModel calModel;
  float last_update_time;
  csArray<ActiveAnim> active_anims;
  bool is_idling;
  int  default_idle_anim,last_locked_anim;
  float idle_override_interval;
  int   idle_action;

  // Optimization: only update animation when true.
  int do_update;	// If 0 we update, else we decrease.

  
  // User defined position update things  
  csRef<iAnimTimeUpdateHandler> anim_time_handler;

  csFlags flags;

  /**
   * Each mesh must have its own individual socket assignments,
   * but the vector must be copied down from the factory at create time.
   */
  csPDelArray<csSpriteCal3DSocket> sockets;

  class MeshAccessor : public iRenderBufferAccessor
  {
  protected:
    csSpriteCal3DMeshObject* meshobj;
    int mesh;
    uint colorVersion, normalVersion;
    int vertexCount;

    csRef<iRenderBuffer> texel_buffer;
    csRef<iRenderBuffer> normal_buffer;
    csRef<iRenderBuffer> color_buffer;
    csRef<iRenderBuffer> index_buffer;

    void UpdateNormals (CalRenderer* render,
      CalMesh* calMesh, size_t vertexCount);
  public:
    iMovable* movable;

    SCF_DECLARE_IBASE;

    MeshAccessor (csSpriteCal3DMeshObject* meshobj, int mesh)
    {
      SCF_CONSTRUCT_IBASE (0);
      MeshAccessor::meshobj = meshobj;
      MeshAccessor::mesh = mesh;
      colorVersion = normalVersion = (uint)-1;
      vertexCount = meshobj->ComputeVertexCount (mesh);
    }
    virtual ~MeshAccessor() 
    {
      SCF_DESTRUCT_IBASE ();
    }
    virtual void PreGetBuffer (csRenderBufferHolder*, csRenderBufferName);
    
  };
  friend class MeshAccessor;


  /**
   * Default animation time update handler (simply invokes CalModel::update()).
   */
  struct DefaultAnimTimeUpdateHandler : public iAnimTimeUpdateHandler
  {
    SCF_DECLARE_IBASE;
    DefaultAnimTimeUpdateHandler() { SCF_CONSTRUCT_IBASE(0); }
    virtual ~DefaultAnimTimeUpdateHandler() { SCF_DESTRUCT_IBASE(); }
    void UpdatePosition (float delta, CalModel*);
  };

  csRef<iStringSet> strings;
  csWeakRef<iGraphics3D> G3D;

  /* The deal with meshes, submeshes and attached meshes:
   * - A cal3d model consists of multiple meshes.
   * - A cal3d mesh consists of multiple submeshes.
   * - A cal3d submesh has one material(* Although in some places CS allows
   *   only a material per mesh)
   * - Of the meshes, several can be 'attached' at a time - that is,
   *   actually used and rendered. Every mesh cam at most be only attached 
   *   once.
   * For CS rendering, since we limit the material to one per mesh, we also
   * use one rendermesh per cal mesh. 
   * The total number of RMs may vary with instances and their attached meshes.
   */
  uint meshVersion;
  csBox3 object_bbox;
  uint bboxVersion;
  bool lighting_dirty;

  struct Mesh
  {
    /// Cal3d ID of the mesh
    int calCoreMeshID;
    /// Shader variable context for this mesh
    csRef<csShaderVariableContext> svc;

    csRef<iRenderBuffer> vertex_buffer;
    uint vertexVersion;

    Mesh() : vertexVersion((uint)~0) {}
  };
  csArray<Mesh> meshes;
  csArray<csRenderMesh> renderMeshTemplates;
  static int CompareMeshIndexKey (const Mesh& m, int const& id);
  static int CompareMeshMesh (const Mesh& m1, const Mesh& m2);
  void SetupRenderMeshTemplate (CalMesh* calMesh, size_t index);
  // Vertices are handled differently since they're also needed by HitBeam*().
  iRenderBuffer* GetVertexBuffer (size_t index, CalRenderer *pCalRenderer);
  int ComputeVertexCount (int mesh);

  void SetIdleOverrides(csRandomGen *rng,int which);
  void RecalcBoundingBox(csBox3& bbox);

  int FindAnimCyclePos(int idx) const;
  int FindAnimCycleNamePos(char const*) const;
  void ClearAnimCyclePos(int pos, float delay);

  void InitSubmeshLighting (int mesh, int submesh, CalRenderer *pCalRenderer,
    iMovable* movable, csColor* colors);
  void UpdateLightingSubmesh (const csArray<iLightSectorInfluence*>& lights,
      iMovable*, CalRenderer*, int mesh, int submesh, float* have_normals, 
      csColor* colors);

public:
  float updateanim_sqdistance1;
  int updateanim_skip1;		// 0 is normal, > 0 is skip
  float updateanim_sqdistance2;
  int updateanim_skip2;
  float updateanim_sqdistance3;
  int updateanim_skip3;

public:
  SCF_DECLARE_IBASE;
  CS_LEAKGUARD_DECLARE (csSpriteCal3DMeshObject);

  /// The parent.
  csSpriteCal3DMeshObjectFactory* factory;

  /// Constructor.
  csSpriteCal3DMeshObject (iBase *pParent, iObjectRegistry* object_reg,
    CalCoreModel& calCoreModel);
  /// Destructor.
  virtual ~csSpriteCal3DMeshObject ();

  /// Set the factory.
  void SetFactory (csSpriteCal3DMeshObjectFactory* factory);

  /// Get the factory.
  csSpriteCal3DMeshObjectFactory* GetFactory3D () const { return factory; }

  // ------------------------------- Lighting --------------------------------
  void LightChanged (iLight* light);
  void LightDisconnect (iLight* light);

  //------------------------ iLightingInfo interface -------------------------
  struct LightingInfo : public iLightingInfo
  {
    SCF_DECLARE_EMBEDDED_IBASE (csSpriteCal3DMeshObject);
    virtual void InitializeDefault (bool /*clear*/)
    {
    }
    virtual bool ReadFromCache (iCacheManager* /*cache_mgr*/)
    {
      return true;
    }
    virtual bool WriteToCache (iCacheManager* /*cache_mgr*/)
    {
      return true;
    }
    virtual void PrepareLighting ()
    {
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

  //----------------------- iMeshObject implementation -----------------------
  virtual bool HitBeamOutline (const csVector3& start, const csVector3& end,
      csVector3& intersect, float* pr);
  virtual bool HitBeamObject (const csVector3& start, const csVector3& end,
      csVector3& intersect, float* pr, int* = 0);

  virtual bool SetColor (const csColor& /*col*/)
  {
    //SetBaseColor (col);
    return true;
  }
  virtual bool GetColor (csColor& /*col*/) const
  {
    //GetBaseColor (col);
    return true;
  }
  virtual bool SetMaterialWrapper (iMaterialWrapper* /*mat*/)
  {
    //SetMaterial (mat);
    return true;
  }
  virtual iMaterialWrapper* GetMaterialWrapper () const
  {
    return 0; //GetMaterial ();
  }
  virtual void InvalidateMaterialHandles () { }
  /**
   * See imesh/object.h for specification. The default implementation
   * does nothing.
   */
  virtual void PositionChild (iMeshObject* child,csTicks current_time);

  virtual iMeshObjectFactory* GetFactory () const
  {
    csRef<iMeshObjectFactory> ifact (SCF_QUERY_INTERFACE (factory,
    	iMeshObjectFactory));
    return ifact;	// DecRef is ok here.
  }
  virtual csFlags& GetFlags () { return flags; }
  virtual csPtr<iMeshObject> Clone () { return 0; }
  virtual csRenderMesh **GetRenderMeshes (int &n, iRenderView* rview,
    iMovable* movable, uint32 frustum_mask);
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
  virtual bool Advance (csTicks current_time);

  virtual void NextFrame (csTicks current_time, const csVector3& /*new_pos*/)
  {   
    Advance (current_time);
  }
  virtual void HardTransform (const csReversibleTransform&) { }
  virtual bool SupportsHardTransform () const { return false; }
  virtual void SetMeshWrapper (iMeshWrapper* lp) { logparent = lp; }
  virtual iMeshWrapper* GetMeshWrapper () const { return logparent; }
  void SetUserData(void *data);

  //------------------ iPolygonMesh interface implementation -----------------
  struct PolyMesh : public iPolygonMesh
  {
  private:
    csFlags flags;

  public:
    SCF_DECLARE_EMBEDDED_IBASE (csSpriteCal3DMeshObject);

    /// Get the number of vertices for this mesh.
    virtual int GetVertexCount ()
    {
      csSpriteCal3DMeshObjectFactory* fact = scfParent->GetFactory3D ();
      return fact->GetVertexCount ();
    }
    /// Get the pointer to the array of vertices.
    virtual csVector3* GetVertices ()
    {
      csSpriteCal3DMeshObjectFactory* fact = scfParent->GetFactory3D ();
      return fact->GetVertices ();
    }
    /// Get the number of polygons for this mesh.
    virtual int GetPolygonCount ()
    {
      csSpriteCal3DMeshObjectFactory* fact = scfParent->GetFactory3D ();
      return fact->GetTriangleCount ();
    }
    virtual int GetTriangleCount ()
    {
      csSpriteCal3DMeshObjectFactory* fact = scfParent->GetFactory3D ();
      return fact->GetTriangleCount ();
    }
    virtual csTriangle* GetTriangles ()
    {
      csSpriteCal3DMeshObjectFactory* fact = scfParent->GetFactory3D ();
      return fact->GetTriangles();
    }

    /// Get the pointer to the array of polygons.
    virtual csMeshedPolygon* GetPolygons ();

    virtual void Lock () { }
    virtual void Unlock () { }
    
    virtual csFlags& GetFlags () { return flags;  }
    virtual uint32 GetChangeNumber() const { return 0; }

    PolyMesh () : polygons (0)
    {
      flags.Set (CS_POLYMESH_TRIANGLEMESH);
    }
    virtual ~PolyMesh () { Cleanup (); }
    void Cleanup () { delete[] polygons; polygons = 0; }

    csMeshedPolygon* polygons;
  } scfiPolygonMesh;
  friend struct PolyMesh;

  virtual iObjectModel* GetObjectModel () { return &scfiObjectModel; }

  //--------------------- iSpriteCal3DState implementation -------------------
  int GetAnimCount();
  const char *GetAnimName(int idx);
  int  GetAnimType(int idx);
  int  FindAnim(const char *name);
  void ClearAllAnims();
  bool SetAnimCycle(const char *name, float weight);
  bool SetAnimCycle(int idx, float weight);
  bool AddAnimCycle(const char *name, float weight, float delay);
  bool AddAnimCycle(int idx, float weight, float delay);
  bool ClearAnimCycle (int idx, float delay);
  bool ClearAnimCycle (const char *name, float delay);
  size_t GetActiveAnimCount();
  bool GetActiveAnims(csSpriteCal3DActiveAnim* buffer, size_t max_length);
  void SetActiveAnims(const csSpriteCal3DActiveAnim* buffer, size_t anim_count);
  bool SetAnimAction(const char *name, float delayIn, float delayOut);
  bool SetAnimAction(int idx, float delayIn, float delayOut);
  bool SetVelocity(float vel,csRandomGen *rng=0);
  void SetDefaultIdleAnim(const char *name);
  void SetLOD(float lod);
  void SetTimeFactor(float timeFactor);
  float GetTimeFactor();

  bool AttachCoreMesh(const char *meshname);
  bool AttachCoreMesh(int mesh_id,iMaterialWrapper* iMatWrapID);
  bool DetachCoreMesh(const char *meshname);
  bool DetachCoreMesh(int mesh_id);

  bool BlendMorphTarget(int morph_animation_id, float weight, float delay);
  bool ClearMorphTarget(int morph_animation_id, float delay);

  /// Create and add a new socket to the sprite.
  csSpriteCal3DSocket* AddSocket ();
  /// find a named socket into the sprite.
  csSpriteCal3DSocket* FindSocket (const char * name) const;
  /// find a socked based on the sprite attached to it
  csSpriteCal3DSocket* FindSocket (iMeshWrapper *mesh) const;
  /// Query the number of sockets
  int GetSocketCount () const { return (int)sockets.Length (); }
  /// Query the socket number f
  csSpriteCal3DSocket* GetSocket (int f) const
  {
    return ((size_t)f < sockets.Length()) ?
      (csSpriteCal3DSocket*)sockets[f] : (csSpriteCal3DSocket*)0;
  }

  bool SetMaterial(const char *mesh_name,iMaterialWrapper *mat);

  float GetAnimationTime()
  {
    return calModel.getMixer()->getAnimationTime();
  }
  float GetAnimationDuration()
  {
    return calModel.getMixer()->getAnimationDuration();
  }
  void SetAnimationTime(float animationTime)
  {
    calModel.getMixer()->setAnimationTime(animationTime);
  }

  void SetAnimTimeUpdateHandler (iAnimTimeUpdateHandler*);

  virtual iShaderVariableContext* GetCoreMeshShaderVarContext (
    const char* meshName);

  struct SpriteCal3DState : public iSpriteCal3DState
  {
    SCF_DECLARE_EMBEDDED_IBASE(csSpriteCal3DMeshObject);

    virtual int GetAnimCount()
    {
      return scfParent->GetAnimCount();
    }
    virtual const char *GetAnimName(int idx)
    {
      return scfParent->GetAnimName(idx);
    }
    virtual int GetAnimType(int idx)
    {
      return scfParent->GetAnimType(idx);
    }
    virtual int FindAnim(const char* name)
    {
      return scfParent->FindAnim(name);
    }
    virtual void ClearAllAnims()
    {
      scfParent->ClearAllAnims();
    }
    virtual bool SetAnimCycle(const char *name, float weight)
    {
      return scfParent->SetAnimCycle(name,weight);
    }
    virtual bool SetAnimCycle(int idx, float weight)
    {
      return scfParent->SetAnimCycle(idx,weight);
    }
    virtual bool AddAnimCycle(const char *name, float weight, float delay)
    {
      return scfParent->AddAnimCycle(name,weight,delay);
    }
    bool AddAnimCycle (int idx, float weight, float delay)
    {
      return scfParent->AddAnimCycle (idx, weight, delay);
    }
    virtual bool ClearAnimCycle (int idx, float delay)
    {
      return scfParent->ClearAnimCycle (idx, delay);
    }
    virtual bool ClearAnimCycle(const char *name, float delay)
    {
      return scfParent->ClearAnimCycle(name,delay);
    }
    virtual size_t GetActiveAnimCount()
    {
      return scfParent->GetActiveAnimCount();
    }
    virtual bool GetActiveAnims(csSpriteCal3DActiveAnim* buffer, 
      size_t max_length)
    {
      return scfParent->GetActiveAnims(buffer,max_length);
    }
    virtual void SetActiveAnims(const csSpriteCal3DActiveAnim* buffer,
    	size_t anim_count)
    {
      scfParent->SetActiveAnims(buffer,anim_count);
    }
    virtual bool SetAnimAction(const char *name, float delayIn, float delayOut)
    {
      return scfParent->SetAnimAction(name,delayIn,delayOut);
    }
    virtual bool SetAnimAction(int idx, float delayIn, float delayOut)
    {
      return scfParent->SetAnimAction(idx,delayIn,delayOut);
    }

    virtual bool SetVelocity(float vel,csRandomGen *rng=0)
    {
      return scfParent->SetVelocity(vel,rng);
    }
    virtual void SetDefaultIdleAnim(const char *name)
    {
      scfParent->SetDefaultIdleAnim(name);
    }

    virtual void SetLOD(float lod)
    {
      scfParent->SetLOD(lod);
    }
    virtual bool AttachCoreMesh(const char *meshname)
    { return scfParent->AttachCoreMesh(meshname); }

    virtual bool AttachCoreMesh(int mesh_id,iMaterialWrapper* iMatWrapID)
    { return scfParent->AttachCoreMesh(mesh_id,iMatWrapID); }

    virtual bool DetachCoreMesh(const char *meshname)
    { return scfParent->DetachCoreMesh(meshname); }

    virtual bool DetachCoreMesh(int mesh_id)
    { return scfParent->DetachCoreMesh(mesh_id); }

    virtual bool BlendMorphTarget(int morph_animation_id, float weight,
      float delay)
    { return scfParent->BlendMorphTarget(morph_animation_id, weight, delay); }

    virtual bool ClearMorphTarget(int morph_animation_id, float delay)
    { return scfParent->ClearMorphTarget(morph_animation_id, delay); }
    virtual iSpriteCal3DSocket* AddSocket ()
    {
      csRef<iSpriteCal3DSocket> ifr (
	  SCF_QUERY_INTERFACE_SAFE (scfParent->AddSocket (),
	    iSpriteCal3DSocket));
      return ifr;       // DecRef is ok here.
    }
    virtual iSpriteCal3DSocket* FindSocket (const char* name) const
    {
      csRef<iSpriteCal3DSocket> ifr (SCF_QUERY_INTERFACE_SAFE (
	    scfParent->FindSocket (name), iSpriteCal3DSocket));
      return ifr;       // DecRef is ok here.
    }
    virtual iSpriteCal3DSocket* FindSocket (iMeshWrapper* mesh) const
    {
      csRef<iSpriteCal3DSocket> ifr (SCF_QUERY_INTERFACE_SAFE (
	    scfParent->FindSocket (mesh), iSpriteCal3DSocket));
      return ifr;       // DecRef is ok here.
    }
    virtual int GetSocketCount () const
    {
      return scfParent->GetSocketCount ();
    }
    virtual iSpriteCal3DSocket* GetSocket (int f) const
    {
      csRef<iSpriteCal3DSocket> ifr (
	  SCF_QUERY_INTERFACE_SAFE (scfParent->GetSocket (f),
	    iSpriteCal3DSocket));
      return ifr;       // DecRef is ok here.
    }
    virtual bool SetMaterial(const char *mesh_name,iMaterialWrapper *mat)
    {
      return scfParent->SetMaterial(mesh_name,mat);
    }
    virtual void SetTimeFactor(float timeFactor)
    {
      scfParent->SetTimeFactor(timeFactor);
    }
    virtual float GetTimeFactor()
    {
      return scfParent->GetTimeFactor();
    }
    virtual float GetAnimationTime()
    {
      return scfParent->GetAnimationTime();
    }
    virtual float GetAnimationDuration()
    {
      return scfParent->GetAnimationDuration();
    }
    virtual void SetAnimationTime(float animationTime)
    {
      scfParent->SetAnimationTime(animationTime);
    }
    virtual CalModel *GetCal3DModel()
    {
      return &scfParent->calModel;
    }
    virtual void SetUserData(void *data)
    {
      scfParent->SetUserData(data);
    }
    virtual void SetAnimTimeUpdateHandler(iAnimTimeUpdateHandler* p)
    {
      scfParent->SetAnimTimeUpdateHandler(p);
    }
    virtual iShaderVariableContext* GetCoreMeshShaderVarContext (
      const char* meshName)
    { return scfParent->GetCoreMeshShaderVarContext (meshName); }
  } scfiSpriteCal3DState;
  friend struct SpriteCal3DState;

  //--------------------- iLODControl implementation -------------------------
  int GetLODPolygonCount (float /*lod*/)
  { return 0; }

  struct LODControl : public iLODControl
  {
    SCF_DECLARE_EMBEDDED_IBASE (csSpriteCal3DMeshObject);
    virtual void SetLOD (float, float) {  }
    virtual void GetLOD (float& m, float& a) const { m = 0; a = 1; }
    virtual void SetLOD (iSharedVariable*, iSharedVariable*) {  }
    virtual void GetLOD (iSharedVariable*& varm, iSharedVariable*& vara)
    {
      varm = 0;
      vara = 0;
    }
    virtual int GetLODPolygonCount (float lod) const
    {
      return scfParent->GetLODPolygonCount (lod);
    }
    virtual void GetLOD(iSharedVariable*& first,iSharedVariable*& second) const
    {
      first=0; second=0;
    }
  } scfiLODControl;
  friend struct LODControl;

  void GetObjectBoundingBox (csBox3& bbox, csVector3 *verts, int vertCount);
  void GetObjectBoundingBox (csBox3& bbox);
  void SetObjectBoundingBox (const csBox3& bbox);
  void GetRadius (csVector3& rad, csVector3& cent);

  class ObjectModel : public csObjectModel
  {
    SCF_DECLARE_EMBEDDED_IBASE (csSpriteCal3DMeshObject);
    virtual void GetObjectBoundingBox (csBox3& bbox)
    {
      scfParent->GetObjectBoundingBox (bbox);
    }
    virtual void SetObjectBoundingBox (const csBox3& bbox)
    {
      scfParent->SetObjectBoundingBox (bbox);
    }
    virtual void GetRadius (csVector3& rad, csVector3& cent)
    {
      scfParent->GetRadius (rad, cent);
    }
  } scfiObjectModel;
  friend class ObjectModel;
};

/**
 * Sprite Cal3D type. This is the plugin you have to use to create instances
 * of csSpriteCal3DMeshObjectFactory.
 */
class csSpriteCal3DMeshObjectType : public iMeshObjectType
{
private:
  iObjectRegistry* object_reg;
  csRef<iVirtualClock> vc;
  csWeakRef<iEngine> engine;

public:
  float updateanim_sqdistance1;
  int updateanim_skip1;		// 0 is normal, > 0 is skip
  float updateanim_sqdistance2;
  int updateanim_skip2;
  float updateanim_sqdistance3;
  int updateanim_skip3;

  csRenderMeshHolder rmHolder;
  csFrameDataHolder<csDirtyAccessArray<csRenderMesh*> > rmArrayHolder;

public:
  /// Constructor.
  csSpriteCal3DMeshObjectType (iBase*);

  /// Destructor.
  virtual ~csSpriteCal3DMeshObjectType ();

  bool Initialize (iObjectRegistry* p);

  //------------------------ iMeshObjectType implementation ------------------
  SCF_DECLARE_IBASE;

  /// New Factory.
  virtual csPtr<iMeshObjectFactory> NewFactory ();

  //--------------------- iComponent interface implementation ----------------
  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csSpriteCal3DMeshObjectType);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize (p); }
  } scfiComponent;
  friend struct eiComponent;

  //--------------------- iLODControl implementation -------------------------
  struct LODControl : public iLODControl
  {
    SCF_DECLARE_EMBEDDED_IBASE (csSpriteCal3DMeshObjectType);
    virtual void SetLOD (float, float)
    {
    }
    virtual void GetLOD (float& m, float& a) const
    {
      m = 0;
      a = 1;
    }
    virtual void SetLOD (iSharedVariable*, iSharedVariable*)
    {
    }
    virtual void GetLOD (iSharedVariable*& varm, iSharedVariable*& vara)
    {
      varm = 0;
      vara = 0;
    }
    virtual int GetLODPolygonCount (float /*lod*/) const
    {
      return 0;
    }
    virtual void GetLOD(iSharedVariable*& first,iSharedVariable*& second) const
    {
      first=0; second=0;
    }
  } scfiLODControl;
  friend struct LODControl;
};

#endif // __CS_SPRCAL3D_H__
