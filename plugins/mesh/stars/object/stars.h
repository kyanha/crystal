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

#ifndef _STARS_H_
#define _STARS_H_

#include "csgeom/vector3.h"
#include "csgeom/transfrm.h"
#include "csutil/cscolor.h"
#include "imesh/object.h"
#include "imesh/stars.h"
#include "ivideo/graph3d.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "igeom/objmodel.h"

struct iMaterialWrapper;
class csStarsMeshObjectFactory;

/**
 * Stars version of mesh object.
 */
class csStarsMeshObject : public iMeshObject
{
private:
  iMeshObjectFactory* factory;
  iBase* logparent;
  csBox3 box;
  iMeshObjectDrawCallback* vis_cb;
  csColor color;
  csColor max_color;
  bool use_max_color;
  float density;
  float max_dist;

  int seed;

  bool initialized;
  csVector3 max_radius;
  long shapenr;
  float current_lod;
  uint32 current_features;

  /**
   * Setup this object. This function will check if setup is needed.
   */
  void SetupObject ();

  /// get a random number between 0.0 and max
  float GetRandom(float max);

  /// draw a box of stars
  void DrawStarBox (iRenderView* rview,
    const csReversibleTransform &tr_o2c, csZBufMode zbufmode,
      csBox3& starbox, const csVector3& origin);

  //; Draw a star on the screen, given screencoordinates/depth and color
  void DrawPoint(iRenderView *rview, const csVector3& pos, const csColor& col,
    csZBufMode zbufmode);

public:
  /// Constructor.
  csStarsMeshObject (iMeshObjectFactory* factory);

  /// Destructor.
  virtual ~csStarsMeshObject ();

  /// Set total box.
  void SetBox (const csBox3& b)
  {
    initialized = false;
    box = b;
    shapenr++;
  }
  void GetBox (csBox3& b) const { b = box; }

  /// Set density.
  void SetDensity (float d) { initialized = false; density = d; }
  /// Get density.
  float GetDensity () const { return density; }

  /// Set max distance at which stars are visible.
  void SetMaxDistance (float maxdist) { max_dist = maxdist; }
  /// Get max distance at which stars are visible.
  float GetMaxDistance () const { return max_dist; }

  /// Set the color to use. Will be added to the lighting values.
  void SetColor (const csColor& col) { color = col; }
  /// Get the color.
  csColor GetColor () const { return color; }

  /**
   * Set the color used in the distance.
   * If this is used then stars at max distance will have
   * this color (fading is used).
   */
  void SetMaxColor (const csColor& col)
  {
    max_color = col;
    use_max_color = true;
  }
  /// Get the max color.
  csColor GetMaxColor () const { return max_color; }
  /// Return true if max color is used.
  bool IsMaxColorUsed () const { return use_max_color; }

  void GetObjectBoundingBox (csBox3& bbox, int type = CS_BBOX_NORMAL);
  void GetRadius (csVector3& rad, csVector3& cent)
  {
    rad = max_radius;
    cent = box.GetCenter();
  }

  ///---------------------- iMeshObject implementation ------------------------
  SCF_DECLARE_IBASE;

  virtual iMeshObjectFactory* GetFactory () const { return factory; }
  virtual bool DrawTest (iRenderView* rview, iMovable* movable);
  virtual void UpdateLighting (iLight** lights, int num_lights,
      	iMovable* movable);
  virtual bool Draw (iRenderView* rview, iMovable* movable, csZBufMode mode);
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
  virtual void NextFrame (csTicks /*current_time*/) { }
  virtual bool WantToDie () const { return false; }
  virtual void HardTransform (const csReversibleTransform&) { }
  virtual bool SupportsHardTransform () const { return false; }
  virtual bool HitBeamOutline (const csVector3&, const csVector3&,
        csVector3&, float*)
  { return false; }
  virtual bool HitBeamObject (const csVector3&, const csVector3&,
  	csVector3&, float*) { return false; }
  virtual void SetLogicalParent (iBase* lp) { logparent = lp; }
  virtual iBase* GetLogicalParent () const { return logparent; }

  //------------------------- iObjectModel implementation ----------------
  class ObjectModel : public iObjectModel
  {
    SCF_DECLARE_EMBEDDED_IBASE (csStarsMeshObject);
    virtual long GetShapeNumber () const { return scfParent->shapenr; }
    virtual iPolygonMesh* GetPolygonMesh () { return NULL; }
    virtual iPolygonMesh* GetSmallerPolygonMesh () { return NULL; }
    virtual iPolygonMesh* CreateLowerDetailPolygonMesh (float) { return NULL; }
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

  //------------------------- iStarsState implementation ----------------
  class StarsState : public iStarsState
  {
    SCF_DECLARE_EMBEDDED_IBASE (csStarsMeshObject);
    virtual void SetBox (const csBox3& b)
    {
      scfParent->SetBox (b);
    }
    virtual void GetBox (csBox3& b) const
    {
      scfParent->GetBox (b);
    }

    virtual void SetColor (const csColor& col)
    {
      scfParent->SetColor (col);
    }
    virtual csColor GetColor () const
    {
      return scfParent->GetColor ();
    }
    virtual void SetMaxColor (const csColor& col)
    {
      scfParent->SetMaxColor (col);
    }
    virtual csColor GetMaxColor () const
    {
      return scfParent->GetMaxColor ();
    }
    virtual bool IsMaxColorUsed () const
    {
      return scfParent->IsMaxColorUsed ();
    }

    virtual void SetDensity (float d)
    {
      scfParent->SetDensity (d);
    }
    virtual float GetDensity () const
    {
      return scfParent->GetDensity ();
    }
    virtual void SetMaxDistance (float maxdist)
    {
      scfParent->SetMaxDistance (maxdist);
    }
    virtual float GetMaxDistance () const
    {
      return scfParent->GetMaxDistance ();
    }
  } scfiStarsState;
  friend class StarsState;
};

/**
 * Factory for balls.
 */
class csStarsMeshObjectFactory : public iMeshObjectFactory
{
private:
  iBase* logparent;

public:
  /// Constructor.
  csStarsMeshObjectFactory (iBase *pParent);

  /// Destructor.
  virtual ~csStarsMeshObjectFactory ();

  //------------------------ iMeshObjectFactory implementation --------------
  SCF_DECLARE_IBASE;

  virtual iMeshObject* NewInstance ();
  virtual void HardTransform (const csReversibleTransform&) { }
  virtual bool SupportsHardTransform () const { return false; }
  virtual void SetLogicalParent (iBase* lp) { logparent = lp; }
  virtual iBase* GetLogicalParent () const { return logparent; }
};

/**
 * Stars type. This is the plugin you have to use to create instances
 * of csStarsMeshObjectFactory.
 */
class csStarsMeshObjectType : public iMeshObjectType
{
public:
  SCF_DECLARE_IBASE;

  /// Constructor.
  csStarsMeshObjectType (iBase*);
  /// Destructor.
  virtual ~csStarsMeshObjectType ();

  /// New factory.
  virtual iMeshObjectFactory* NewFactory ();

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csStarsMeshObjectType);
    virtual bool Initialize (iObjectRegistry*) { return true; }
  } scfiComponent;
};

#endif // _STARS_H_

