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

#ifndef __CS_DYNAVIS_H__
#define __CS_DYNAVIS_H__

#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "iutil/dbghelp.h"
#include "csutil/csvector.h"
#include "csutil/scf.h"
#include "csgeom/plane3.h"
#include "iengine/viscull.h"
#include "dmodel.h"

class csKDTree;
class csKDTreeChild;
class csCoverageBuffer;
struct iPolygonMesh;
struct iMovable;
struct iMeshWrapper;

enum csVisReason
{
  INVISIBLE_PARENT = 0,	// Invisible because some parent node is invisible.
  INVISIBLE_FRUSTUM,	// Invisible because object outside frustum.
  INVISIBLE_TESTRECT,	// Invisible because covbuf->TestRectangle() failed.
  VISIBLE,		// Just visible.
  VISIBLE_INSIDE,	// Visible because camera is inside bbox.
  VISIBLE_HISTORY,	// Visible because it was visible last frame.
  LAST_REASON
};

#define VIEWMODE_STATS 0
#define VIEWMODE_STATSOVERLAY 1
#define VIEWMODE_CLEARSTATSOVERLAY 2
#define VIEWMODE_OUTLINES 3

#define VIEWMODE_FIRST 0
#define VIEWMODE_LAST 3

#define COVERAGE_NONE 0
#define COVERAGE_POLYGON 1
#define COVERAGE_OUTLINE 2

struct VisTest_Front2BackData;

/**
 * This object is a wrapper for an iVisibilityObject from the engine.
 */
class csVisibilityObjectWrapper
{
public:
  iVisibilityObject* visobj;
  csKDTreeChild* child;
  long update_number;	// Last used update_number from movable.
  long shape_number;	// Last used shape_number from model.
  csVisReason reason;	// Reason object is visible/invisible.
  csObjectModel* model;

  // If the folloging counter > 0 then the object will be assumed visible
  // automatically. The counter will be decremented then.
  int vis_cnt;

  csVisibilityObjectWrapper ()
  {
    vis_cnt = 0;
  }

  void MarkVisible (csVisReason reason, int cnt)
  {
    visobj->MarkVisible ();
    csVisibilityObjectWrapper::reason = reason;
    vis_cnt = cnt;
  }
};

/**
 * A dynamic visisibility culling system.
 */
class csDynaVis : public iVisibilityCuller
{
private:
  iObjectRegistry *object_reg;
  csKDTree* kdtree;
  csCoverageBuffer* covbuf;
  csVector visobj_vector;
  csObjectModelManager* model_mgr;

  // For Debug_Dump(g3d): keep the last original camera.
  iCamera* debug_camera;

  // For statistics. Count the number of times VisTest() was called.
  int stats_cnt_vistest;
  // For statistics. Count the total time we spend in VisTest().
  csTicks stats_total_vistest_time;
  // For statistics. Count the total time we do NOT spend in VisTest().
  csTicks stats_total_notvistest_time;

  // Various flags to enable/disable parts of the culling algorithm.
  bool do_cull_frustum;
  int do_cull_coverage;
  bool do_cull_history;

  // View mode for debugging (one of VIEWMODE_... constants).
  int cfg_view_mode;

  // If this flag is true we will do an extensive dump of the current
  // visibility culling proceedings during the next frame. This flag
  // will immediatelly be cleared after that dump.
  bool do_state_dump;

  // Scan all objects, mark them as invisible and check if they
  // have moved since last frame (and update them in the kdtree then).
  void UpdateObjects ();

  // Fill the bounding box with the current object status.
  void CalculateVisObjBBox (iVisibilityObject* visobj, csBox3& bbox);

  // Given an occluder, update it in the coverage buffer.
  void UpdateCoverageBuffer (iCamera* camera, iVisibilityObject* visobj,
  	csObjectModel* model);

  // Given an occluder, update it in the coverage buffer. Using the outline.
  void UpdateCoverageBufferOutline (iCamera* camera, iVisibilityObject* visobj,
  	csObjectModel* model);

public:
  SCF_DECLARE_IBASE;

  csDynaVis (iBase *iParent);
  virtual ~csDynaVis ();
  virtual bool Initialize (iObjectRegistry *object_reg);

  // Test visibility for the given node. Returns true if visible.
  // This function will also modify the frustum_mask in 'data'. So
  // take care to restore this later if you recurse down.
  bool TestNodeVisibility (csKDTree* treenode, VisTest_Front2BackData* data);

  // Test visibility for the given object. Returns true if visible.
  bool TestObjectVisibility (csVisibilityObjectWrapper* obj,
  	VisTest_Front2BackData* data);

  virtual void Setup (const char* name);
  virtual void RegisterVisObject (iVisibilityObject* visobj);
  virtual void UnregisterVisObject (iVisibilityObject* visobj);
  virtual bool VisTest (iRenderView* rview);
  virtual iPolygon3D* IntersectSegment (const csVector3& start,
    const csVector3& end, csVector3& isect, float* pr = NULL,
    iMeshWrapper** p_mesh = NULL) { return NULL; }
  virtual bool SupportsShadowCasting () { return false; }
  virtual void CastShadows (iFrustumView* /*fview*/) { }

  virtual void RegisterShadowReceiver (iShadowReceiver* /*receiver*/) { }
  virtual void UnregisterShadowReceiver (iShadowReceiver* /*receiver*/) { }

  // Debugging functions.
  iString* Debug_UnitTest ();
  iString* Debug_StateTest ();
  iString* Debug_Dump ();
  void Debug_Dump (iGraphics3D* g3d);
  csTicks Debug_Benchmark (int num_iterations);
  bool Debug_DebugCommand (const char* cmd);
  csKDTree* GetKDTree () { return kdtree; }

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE (csDynaVis);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize (p); }
  } scfiComponent;

  struct DebugHelper : public iDebugHelper
  {
    SCF_DECLARE_EMBEDDED_IBASE (csDynaVis);
    virtual int GetSupportedTests () const
    {
      return CS_DBGHELP_UNITTEST | CS_DBGHELP_TXTDUMP |
      	CS_DBGHELP_STATETEST | CS_DBGHELP_BENCHMARK |
	CS_DBGHELP_GFXDUMP;
    }
    virtual iString* UnitTest ()
    {
      return scfParent->Debug_UnitTest ();
    }
    virtual iString* StateTest ()
    {
      return scfParent->Debug_StateTest ();
    }
    virtual csTicks Benchmark (int num_iterations)
    {
      return scfParent->Debug_Benchmark (num_iterations);
    }
    virtual iString* Dump ()
    {
      return scfParent->Debug_Dump ();
    }
    virtual void Dump (iGraphics3D* g3d)
    {
      scfParent->Debug_Dump (g3d);
    }
    virtual bool DebugCommand (const char* cmd)
    {
      return scfParent->Debug_DebugCommand (cmd);
    }
  } scfiDebugHelper;
};

#endif // __CS_DYNAVIS_H__

