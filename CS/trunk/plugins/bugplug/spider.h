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

#ifndef __CS_SPIDER_H__
#define __CS_SPIDER_H__

#include "imesh/object.h"
#include "csgeom/box.h"
#include "igeom/objmodel.h"

struct iEngine;
struct iCamera;
struct iMeshWrapper;

/**
 * Beware! In the dark recesses of BugPlug there lives a creature so awful
 * and fierce that even the mightiest heroes don't dare to look at it! When
 * the sun shines over the mountains and fields, everything is well and
 * Spider remains hidden in its dark cave. However, when the moonlight is
 * dim or shaded, and nasty bugs crawl over the country, Spider will weave
 * its web of deceit over the infected land! Unaware of the great danger, an
 * unsuspecting wanderer might look over the country. Here Spider will
 * strike! It will put a curse on the wanderer which will reveal his
 * whereabouts to Spider at any time in the future! The only remedy to this
 * curse is death!
 * <p>
 * For the more technically minded:
 * <ul>
 * <li>Spider: mesh object
 * <li>Mountains and fields: sectors
 * <li>Bugs: bugs :-)
 * <li>Wanderer: camera
 * </ul>
 */
class csSpider : public iMeshObject
{
private:
  iBase* logparent;
  iCamera* camera;
  iMeshWrapper* wrap;

public:
  csSpider ();
  virtual ~csSpider ();

  /// Get the camera that Spider catched.
  iCamera* GetCamera () { return camera; }

  /// Clear the camera so that Spider can catch a new one.
  void ClearCamera () { camera = 0; }

  /**
   * Weave the web: i.e. distribute Spider over all engine sectors.
   * Will return false if this could not be done (i.e. no sectors)
   */
  bool WeaveWeb (iEngine* engine);

  /// Unweave the web.
  void UnweaveWeb (iEngine* engine);

  void GetObjectBoundingBox (csBox3& bbox, int type = CS_BBOX_NORMAL)
  {
    (void)type;
    bbox.Set (-100000, -100000, -100000, 100000, 100000, 100000);
    return;
  }
  void GetRadius (csVector3& rad, csVector3& cent)
  {
    rad.Set (200000, 200000, 200000);
    cent.Set (0,0,0);
  }

  SCF_DECLARE_IBASE;

  virtual iMeshObjectFactory* GetFactory () const { return 0; }
  virtual bool DrawTest (iRenderView* rview, iMovable* movable);
  virtual void UpdateLighting (iLight**, int, iMovable*) { }
  virtual bool Draw (iRenderView*, iMovable*, csZBufMode) { return false; }
  virtual csRenderMesh** GetRenderMeshes (int&) { return 0; }
#ifdef CS_USE_NEW_RENDERER
  virtual bool DrawZ (iRenderView*, iMovable*, csZBufMode) { return false; }
  virtual bool DrawShadow (iRenderView*, iMovable*, csZBufMode, iLight *light) { return false; }
  virtual bool DrawLight (iRenderView*, iMovable*, csZBufMode, iLight *) { return false; }
  /// The following enable/disable shadow caps for stencil shadow rendering
  virtual void EnableShadowCaps (){;}
  virtual void DisableShadowCaps (){;}

#endif // CS_USE_NEW_RENDERER
  virtual void SetVisibleCallback (iMeshObjectDrawCallback*) { }
  virtual iMeshObjectDrawCallback* GetVisibleCallback () const { return 0; }
  virtual void NextFrame (csTicks, const csVector3& /*pos*/) { }
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
    SCF_DECLARE_EMBEDDED_IBASE (csSpider);
    virtual long GetShapeNumber () const { return 1; }
    virtual iPolygonMesh* GetPolygonMeshBase () { return 0; }
    virtual iPolygonMesh* GetPolygonMeshColldet () { return 0; }
    virtual void SetPolygonMeshColldet (iPolygonMesh*) { }
    virtual iPolygonMesh* GetPolygonMeshViscull () { return 0; }
    virtual void SetPolygonMeshViscull (iPolygonMesh*) { }
    virtual iPolygonMesh* GetPolygonMeshShadows () { return 0; }
    virtual void SetPolygonMeshShadows (iPolygonMesh*) { }
    virtual csPtr<iPolygonMesh> CreateLowerDetailPolygonMesh (float)
    { return 0; }
    virtual void GetObjectBoundingBox (csBox3& bbox, int type = CS_BBOX_NORMAL)
    {
      scfParent->GetObjectBoundingBox (bbox, type);
    }
    virtual void GetRadius (csVector3& rad, csVector3& cent)
    {
      scfParent->GetRadius (rad, cent);
    }
    virtual void AddListener (iObjectModelListener*)
    {
    }
    virtual void RemoveListener (iObjectModelListener*)
    {
    }
  } scfiObjectModel;
  friend class ObjectModel;

  virtual iObjectModel* GetObjectModel () { return &scfiObjectModel; }
  virtual bool SetColor (const csColor&) { return false; }
  virtual bool GetColor (csColor&) const { return false; }
  virtual bool SetMaterialWrapper (iMaterialWrapper*) { return false; }
  virtual iMaterialWrapper* GetMaterialWrapper () const { return 0; }
  virtual int GetPortalCount () const { return 0; }
  virtual iPortal* GetPortal (int) const { return 0; }
};

#endif // __CS_SPIDER_H__
