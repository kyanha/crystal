/*
    Copyright (C) 1998,2000 by Jorrit Tyberghein

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

#ifndef SKEL3D_H
#define SKEL3D_H

#include "csgeom/math3d.h"
#include "csgeom/math2d.h"
#include "csgeom/box.h"
#include "csgeom/transfrm.h"
#include "plugins/mesh/spr3d/object/spr3d.h"
#include "imesh/skeleton.h"
#include "iengine/skelbone.h"

class csSkelLimbState;
class csPoly3D;

/**
 * A limb in a skeletal system. The Crystal Space skeletal system
 * works by controlling the vertices in a frame. So we combine
 * frame based sprites and skeletal sprites.
 */
class csSkelLimb : public iSkeletonLimb
{
private:
  /// Next in the list.
  csSkelLimb* next;

  /// The vertices from the parent sprite that it controls.
  int* vertices;

  /// The number of vertices to control.
  int num_vertices;

  /// Children of this limb.
  csSkelLimb* children;

  /// Bounding box in object space for this limb.
  csBox3 box;
  /// Radius in object space for this limb.
  csVector3 radius;

  /// The name of this Limb.
  char* name;

protected:
  /// Update state information.
  void UpdateState (csSkelLimbState* limb);

public:
  /// Create an empty limb.
  csSkelLimb ();

  /// Destructor.
  virtual ~csSkelLimb ();

  /// Add a vertex index.
  virtual void AddVertex (int v);

  /// Get the vertex index array.
  virtual int* GetVertices () const { return vertices; }

  /// Get the number of vertices.
  virtual int GetVertexCount () const { return num_vertices; }

  /// Get the bounding box.
  void GetBoundingBox (csBox3& b) const { b = box; }

  /// Get the radius.
  void GetRadius (csVector3& r) const { r = radius; }

  /// Add a child limb.
  void AddChild (csSkelLimb* child);

  /// Get first child.
  iSkeletonLimb* GetChildren () const { return (iSkeletonLimb*)children; }

  /// Linked list.
  void SetNext (csSkelLimb* n) { next = n; }
  /// Linked list.
  csSkelLimb* GetNext () const { return next; }
  /// Linked list.
  virtual iSkeletonLimb* GetNextSibling () const { return (iSkeletonLimb*)next; }

  /// Create a csSkelState from this csSkel.
  virtual csSkelLimbState* CreateState ();

  /// For LOD. Remap vertices.
  void RemapVertices (int* mapping);

  /**
   * Compute the object space bounding box and radius for this limb.
   */
  void ComputeBoundingBox (csPoly3D* source);

  /// Set the name for this limb.
  virtual void SetName (const char* name);
  /// Get the name for this limb.
  virtual const char* GetName () const { return name; }
  /// Create a connection.
  virtual iSkeletonConnection* CreateConnection ();

  SCF_DECLARE_IBASE;
};

/**
 * A connection. This is basically a transformation with a limb that
 * is controlled by the transformation.
 */
class csSkelConnection : public csSkelLimb
{
private:
  /// A transformation with respect to the parent limb.
  csTransform trans;

public:
  /// Create an empty limb with an identity transformation.
  csSkelConnection ();

  /// Create an empty limb with the given transformation.
  csSkelConnection (csTransform& tr) : trans (tr) { }

  /// Destructor.
  virtual ~csSkelConnection () { }

  /// Set the transformation.
  virtual void SetTransformation (const csTransform& tr) { trans = tr; }

  /// Get the transformation.
  virtual csTransform& GetTransformation () { return trans; }

  /// Create a csSkelState from this csSkel.
  virtual csSkelLimbState* CreateState ();

  SCF_DECLARE_IBASE_EXT (csSkelLimb);

  struct SkeletonConnection : public iSkeletonConnection
  {
    SCF_DECLARE_EMBEDDED_IBASE (csSkelConnection);
    virtual void SetTransformation (const csTransform& t)
      { scfParent->SetTransformation(t); }
    virtual csTransform& GetTransformation ()
      { return scfParent->GetTransformation(); }
  } scfiSkeletonConnection;
};

/**
 * The base skeleton class (or the 'body').
 */
class csSkel : public csSkelLimb
{
public:
  /// Create an empty skeleton.
  csSkel ();

  /// Destructor.
  virtual ~csSkel () { }

  /// Create a csSkelState from this csSkel.
  virtual csSkelLimbState* CreateState ();

  SCF_DECLARE_IBASE_EXT (csSkelLimb);

  struct Skeleton : public iSkeleton
  {
    SCF_DECLARE_EMBEDDED_IBASE (csSkel);
  } scfiSkeleton;
};

/**
 * A limb in a skeletal system (state version, see csSkelState for
 * more information).
 */
class csSkelLimbState : public iSkeletonLimbState
{
  friend class csSkelLimb;

private:
  /// Next in the list.
  csSkelLimbState* next;

  /// Pointer to original skeleton node.
  csSkelLimb* tmpl;

  /// The vertices from the parent sprite that it controls.
  int* vertices;

  /// The number of vertices to control.
  int num_vertices;

  /// Children of this limb.
  csSkelLimbState* children;

  /// The name of this Limb.
  char* name;

  /// Userdata.
  void* data;

  /// Add a child limb.
  void AddChild (csSkelLimbState* child);

  /// Linked list.
  void SetNext (csSkelLimbState* n) { next = n; }

protected:
  /// Create an empty limb (protected constructor).
  csSkelLimbState ();

public:
  /// Destructor.
  virtual ~csSkelLimbState ();

  /**
   * Transform the vertices in the given frame to the destination frame.
   * This is a recursive function which traverses all limbs and creates
   * a combined transformation along the way.
   */
  virtual void Transform (const csTransform& tr, csVector3* source,
  	csVector3* dest);

  /**
   * Calculate the real bounding box for the given state.
   */
  virtual void ComputeBoundingBox (const csTransform& tr, csBox3& box,
  	csPoly3D* source);

  /**
   * Calculate the real squared radius for the given state.
   */
  virtual void ComputeSqRadius (const csTransform& tr,
  	csVector3& max_sq_radius, csPoly3D* source);

  /// Get first child.
  csSkelLimbState* GetFirstChild () { return children; }
  /// Get first child.
  iSkeletonLimbState* GetChildren () const
  { return (iSkeletonLimbState*)children; }

  /// Get next sibling in list.
  csSkelLimbState* GetNext () { return next; }
  /// Get next sibling in list.
  iSkeletonLimbState* GetNextSibling () const
  { return (iSkeletonLimbState*)next; }

  /// Set the name for this limb.
  virtual void SetName (const char* name);
  /// Get the name for this limb.
  virtual const char* GetName () const { return name; }
  /// Set userdata object.
  virtual void SetUserData (void* data) { csSkelLimbState::data = data; }
  /// Get userdata object.
  virtual void* GetUserData () const { return data; }


  SCF_DECLARE_IBASE;
};

/**
 * A connection (state version, see csSkelState for more information).
 * This is basicly a transformation with a limb that
 * is controlled by the transformation.
 */
class csSkelConnectionState : public csSkelLimbState
{
  friend class csSkelConnection;

private:
  /// A transformation with respect to the parent limb.
  csTransform trans;

protected:
  /// Create an empty limb with an identity transformation (protected).
  csSkelConnectionState ();

public:
  /// Destructor.
  virtual ~csSkelConnectionState () { }

  /// Transform the vertices in the given frame to the destination frame.
  virtual void Transform (const csTransform& tr, csVector3* source,
  	csVector3* dest);

  /// Calculate the real bounding box for the given state.
  virtual void ComputeBoundingBox (const csTransform& tr, csBox3& box,
  	csPoly3D* source);

  /**
   * Calculate the real squared radius for the given state.
   */
  virtual void ComputeSqRadius (const csTransform& tr,
  	csVector3& max_sq_radius, csPoly3D* source);

  /// Set the transformation.
  void SetTransformation (const csTransform& tr) { trans = tr; }

  /// Get the transformation.
  csTransform& GetTransformation () { return trans; }

  SCF_DECLARE_IBASE_EXT (csSkelLimbState);

  struct SkeletonBone : public iSkeletonBone
  {
    SCF_DECLARE_EMBEDDED_IBASE (csSkelConnectionState);

    virtual iSkeletonBone* GetNext ()
    {
      csSkelLimbState* ls=scfParent->GetNext();
      if (!ls) return NULL;
      csRef<iSkeletonBone> ibone (SCF_QUERY_INTERFACE(ls, iSkeletonBone));
      return ibone;	// DecRef is ok here.
    }
    virtual iSkeletonBone* GetChildren ()
    {
      csSkelLimbState* ls=scfParent->GetFirstChild ();
      if (!ls) return NULL;
      csRef<iSkeletonBone> ibone (SCF_QUERY_INTERFACE(ls, iSkeletonBone));
      return ibone;	// DecRef is ok here.
    }
    virtual const char* GetName ()
    {
      return scfParent->GetName();
    }
    virtual void SetTransformation (const csTransform& tr)
    {
      scfParent->SetTransformation (tr);
    }
    virtual csTransform& GetTransformation ()
    {
      return scfParent->GetTransformation ();
    }
  } scfiSkeletonBone;
  friend struct SkeletonBone;

  struct SkeletonConnectionState : iSkeletonConnectionState
  {
    SCF_DECLARE_EMBEDDED_IBASE (csSkelConnectionState);
    virtual void SetTransformation (const csTransform& tr)
    {
      scfParent->SetTransformation (tr);
    }
    virtual csTransform& GetTransformation ()
    {
      return scfParent->GetTransformation ();
    }
  } scfiSkeletonConnectionState;
};

/**
 * Skeleton state. This class is the runtime version of a skeleton.
 * The csSkel classes are used in a template. They contain a
 * static description of the skeleton code. There is at most one csSkel
 * for every csSpriteTemplate. This structure is the run-time
 * version. There is one csSkelState for every csSprite3D which
 * uses a skeleton.
 */
class csSkelState : public csSkelLimbState
{
  friend class csSkel;

protected:
  /**
   * Create an empty skeleton.
   * The constructor is protected because it is csSkel which
   * creates instances of this class.
   */
  csSkelState ();

public:
  /// Destructor.
  virtual ~csSkelState () { }

  /// Calculate the real bounding box for the given state.
  virtual void ComputeBoundingBox (const csTransform& tr, csBox3& box,
  	csPoly3D* source);

  /**
   * Calculate the real radius for the given state.
   */
  virtual void ComputeSqRadius (const csTransform& tr,
  	csVector3& max_sq_radius, csPoly3D* source);

  SCF_DECLARE_IBASE_EXT (csSkelLimbState);

  struct SkeletonBone : public iSkeletonBone
  {
    SCF_DECLARE_EMBEDDED_IBASE (csSkelState);

    virtual iSkeletonBone* GetNext ()
    {
      csSkelLimbState* ls=scfParent->GetNext();
      if(!ls) return NULL;
      csRef<iSkeletonBone> ibone (SCF_QUERY_INTERFACE(ls, iSkeletonBone));
      return ibone;	// DecRef is ok here.
    }
    virtual iSkeletonBone* GetChildren ()
    {
      csSkelLimbState* ls=scfParent->GetFirstChild();
      if(!ls) return NULL;
      csRef<iSkeletonBone> ibone (SCF_QUERY_INTERFACE(ls, iSkeletonBone));
      return ibone;	// DecRef is ok here.
    }
    virtual const char* GetName ()
    {
      return scfParent->GetName();
    }
    // @@@This is ugly. Skeleton doesn't support transformations!
    virtual void SetTransformation (const csTransform&) {}
    csTransform dummy;
    virtual csTransform& GetTransformation () { return dummy; }

  } scfiSkeletonBone;
  friend struct SkeletonBone;

  struct SkeletonState : public iSkeletonState
  {
    SCF_DECLARE_EMBEDDED_IBASE (csSkelState);
  } scfiSkeletonState;
};

#endif /*SKEL3D_H*/
