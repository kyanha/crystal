/*
    Copyright (C) 2003 by Martin Geisse <mgeisse@gmx.net>

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

#ifndef __CS_MESHOBJ_H__
#define __CS_MESHOBJ_H__

#include "csutil/refarr.h"
#include "iengine/material.h"
#include "iengine/engine.h"
#include "iengine/mesh.h"
#include "imesh/object.h"
#include "csgeom/objmodel.h"

/// Declare a simple mesh factory class
#define CS_DECLARE_SIMPLE_MESH_FACTORY(name,meshclass)                      \
  class name : public csMeshFactory {                                       \
  public:                                                                   \
    name (iEngine *e, iObjectRegistry* reg) : csMeshFactory (e, reg) {}     \
    virtual csPtr<iMeshObject> NewInstance ()                               \
    { return new meshclass (Engine, this); }                                \
  };

/// Declare a simple mesh type plugin
#define CS_DECLARE_SIMPLE_MESH_PLUGIN(name,factclass)                       \
  class name : public csMeshType {                                          \
  public:                                                                   \
    name (iBase *p) : csMeshType (p) {}                                     \
    virtual csPtr<iMeshObjectFactory> NewFactory ()                         \
    { return new factclass (Engine, object_reg); }                          \
  };

/**
 * This is an abstract implementation of iMeshObject. It can be used to
 * write custom mesh object implementations more easily. Currently it
 * supports the following common functions of mesh objects:
 * <ul>
 * <li> Implementation of iMeshObject
 * <li> Implementation of iObjectModel
 * <li> Storing a "visible callback"
 * <li> Storing a logical parent
 * <li> Storing object model properties
 * <li> Default implementation of most methods
 * </ul>
 */
class csMeshObject : public iMeshObject
{
protected:
  /// the drawing callback
  csRef<iMeshObjectDrawCallback> VisCallback;

  /// logical parent (usually the wrapper object from the engine)
  iBase *LogParent;

  /// pointer to the engine if available (@@@ temporary)
  iEngine *Engine;

  /// Tell the engine that this object wants to be deleted
  void WantToDie ();

public:
  SCF_DECLARE_IBASE;

  /// Constructor
  csMeshObject (iEngine *engine);

  /// Destructor
  virtual ~csMeshObject ();

  /**
   * See imesh/object.h for specification. There is no default
   * implementation for this method.
   */
  virtual iMeshObjectFactory* GetFactory () const = 0;

  /**
   * See imesh/object.h for sepcification. The default implementation
   * does nothing and always returns true.
   */
  virtual bool DrawTest (iRenderView* rview, iMovable* movable);

  /**
   * See imesh/object.h for sepcification. The default implementation
   * does nothing and always returns 0.
   * @@@ Note: in future it would be better that the default implementation
   * does nothing as this function has to be implemented by mesh objects.
   */
  virtual csRenderMesh** GetRenderMeshes (int& num)
  {
    return 0;
  }

  /**
   * See imesh/object.h for sepcification. The default implementation
   * does nothing.
   */
  virtual void UpdateLighting (iLight** lights, int num_lights,
      	iMovable* movable);

  /**
   * See imesh/object.h for sepcification. There is no default
   * implementation for this method.
   */
  virtual bool Draw (iRenderView* rview, iMovable* movable,
  	csZBufMode zbufMode) = 0;

  /**
   * See imesh/object.h for sepcification. This function is handled
   * completely in csMeshObject. The actual implementation just has
   * to use the VisCallback variable to perform the callback.
   */
  virtual void SetVisibleCallback (iMeshObjectDrawCallback* cb);

  /**
   * See imesh/object.h for sepcification. This function is handled
   * completely in csMeshObject.
   */
  virtual iMeshObjectDrawCallback* GetVisibleCallback () const;

  /**
   * See imesh/object.h for sepcification. The default implementation
   * does nothing.
   */
  virtual void NextFrame (csTicks current_time,const csVector3& pos);

  /**
   * See imesh/object.h for sepcification. The default implementation
   * does nothing.
   */
  virtual void HardTransform (const csReversibleTransform& t);

  /**
   * See imesh/object.h for sepcification. The default implementation
   * returns false.
   */
  virtual bool SupportsHardTransform () const;

  /**
   * See imesh/object.h for sepcification. The default implementation
   * will always return a miss.
   */
  virtual bool HitBeamOutline (const csVector3& start,
  	const csVector3& end, csVector3& isect, float* pr);

  /**
   * See imesh/object.h for sepcification. The default implementation
   * will always return a miss.
   */
  virtual bool HitBeamObject (const csVector3& start, const csVector3& end,
  	csVector3& isect, float* pr);

  /**
   * See imesh/object.h for sepcification. This function is handled
   * completely in csMeshObject.
   */
  virtual void SetLogicalParent (iBase* logparent);

  /**
   * See imesh/object.h for sepcification. This function is handled
   * completely in csMeshObject.
   */
  virtual iBase* GetLogicalParent () const;

  /**
   * See imesh/object.h for specification.
   */
  virtual iObjectModel* GetObjectModel () { return &scfiObjectModel; }

  /**
   * See imesh/object.h for sepcification. The default implementation
   * does not support a base color.
   */
  virtual bool SetColor (const csColor& color);

  /**
   * See imesh/object.h for sepcification. The default implementation
   * does not support a base color.
   */
  virtual bool GetColor (csColor& color) const;

  /**
   * See imesh/object.h for sepcification. The default implementation
   * does not support a material.
   */
  virtual bool SetMaterialWrapper (iMaterialWrapper* material);

  /**
   * See imesh/object.h for sepcification. The default implementation
   * does not support a material.
   */
  virtual iMaterialWrapper* GetMaterialWrapper () const;

  /**
   * See imesh/object.h for sepcification. The default implementation
   * does not support portals.
   */
  virtual int GetPortalCount () const { return 0; }

  /**
   * See imesh/object.h for sepcification. The default implementation
   * does not support portals.
   */
  virtual iPortal* GetPortal (int) const { return 0; }

  /**
   * See igeom/objmodel.h for specification. The default implementation
   * returns an infinite bounding box.
   */
  virtual void GetObjectBoundingBox (csBox3& bbox, int type);

  /**
   * See igeom/objmodel.h for specification. The default implementation
   * returns an infinite radius.
   */
  virtual void GetRadius (csVector3& radius, csVector3& center);

  // implementation of iObjectModel
  struct eiObjectModel : public csObjectModel
  {
    SCF_DECLARE_EMBEDDED_IBASE (csMeshObject);
    virtual void GetObjectBoundingBox (csBox3& bbox, int type)
    {
      scfParent->GetObjectBoundingBox (bbox, type);
    }
    virtual void GetRadius (csVector3& radius, csVector3& center)
    {
      scfParent->GetRadius (radius, center);
    }
  } scfiObjectModel;
  friend struct eiObjectModel;
};

/**
 * This is the abstract implementation of iMeshObjectFactory. Like
 * csMeshObject, it stores a pointer to the "logical parent".
 */
class csMeshFactory : public iMeshObjectFactory
{
protected:
  /// Logical parent (usually the wrapper object from the engine)
  iBase *LogParent;

  /// Pointer to the engine if available (@@@ temporary)
  iEngine *Engine;

  /// Object registry.
  iObjectRegistry* object_reg;

public:
  SCF_DECLARE_IBASE;

  /// Constructor
  csMeshFactory (iEngine *engine, iObjectRegistry* object_reg);

  /// destructor
  virtual ~csMeshFactory ();

  /**
   * See imesh/object.h for sepcification. There is no default
   * implementation for this method.
   */
  virtual csPtr<iMeshObject> NewInstance () = 0;

  /**
   * See imesh/object.h for sepcification. The default implementation
   * does nothing.
   */
  virtual void HardTransform (const csReversibleTransform& t);

  /**
   * See imesh/object.h for sepcification. The default implementation
   * returns false.
   */
  virtual bool SupportsHardTransform () const;

  /**
   * See imesh/object.h for sepcification. This function is handled
   * completely in csMeshObject.
   */
  virtual void SetLogicalParent (iBase* logparent);

  /**
   * See imesh/object.h for sepcification. This function is handled
   * completely in csMeshObject.
   */
  virtual iBase* GetLogicalParent () const;

  /**
   * See imesh/object.h for specification.
   */
  virtual iObjectModel* GetObjectModel () { return 0; }

};

/**
 * This is the abstract implementation of iMeshObjectType.
 */
class csMeshType : public iMeshObjectType
{
protected:
  /// pointer to the engine if available (@@@ temporary)
  iEngine *Engine;

  /// Object registry.
  iObjectRegistry* object_reg;

public:
  SCF_DECLARE_IBASE;

  /// constructor
  csMeshType (iBase *p);

  /// destructor
  virtual ~csMeshType ();

  /**
   * Initialize this plugin.
   */
  bool Initialize (iObjectRegistry* reg);

  /**
   * See imesh/object.h for sepcification. There is no default
   * implementation for this method.
   */
  virtual csPtr<iMeshObjectFactory> NewFactory () = 0;

  /**
   * iComponent implementation.
   */
  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE (csMeshType);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize (p); }
  } scfiComponent;
};

#endif // __CS_MESHOBJ_H__
