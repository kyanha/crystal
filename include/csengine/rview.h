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

#ifndef __CS_RVIEW_H__
#define __CS_RVIEW_H__

#include "csgeom/math3d.h"
#include "csgeom/frustum.h"
#include "csengine/camera.h"

class csClipper;
class csMatrix3;
class csVector3;
class csLight;
class csPolygon3D;
class csRenderView;
class csFrustumView;
struct csFog;
struct iGraphics3D;
struct iGraphics2D;

/// A callback function for csWorld::DrawFunc().
typedef void (csDrawFunc) (csRenderView* rview, int type, void* entity);

/// A callback function for csLight::LightingFunc().
typedef void (csLightingFunc) (csFrustumView* lview, int type, void* entity);

/**
 * Flags for the callbacks called via csWorld::DrawFunc() or
 * csLight::LightingFunc().
 * (type csDrawFunc or csLightingFunc).
 */
#define CALLBACK_POLYGON 1
#define CALLBACK_POLYGON2D 2
#define CALLBACK_POLYGONQ 3
#define CALLBACK_SECTOR 4
#define CALLBACK_SECTOREXIT 5
#define CALLBACK_THING 6
#define CALLBACK_THINGEXIT 7
#define CALLBACK_MESH 8

/**
 * Information for vertex based fog. There is an instance of this
 * structure in the csRenderView struct for every fogged sector that
 * we encounter. It contains information which allows us to calculate
 * the thickness of the fog for any given ray through the incoming
 * and outgoing portals of the sector.
 */
class csFogInfo
{
public:
  /// Next in list (back in recursion time).
  csFogInfo* next;

  /// The incoming plane (plane of the portal).
  csPlane3 incoming_plane;
  /// The outgoing plane (also of a portal).
  csPlane3 outgoing_plane;
  /**
   * If this is false then there is no incoming plane (the current sector has
   * fog and is not being drawn through a portal).
   */
  bool has_incoming_plane;

  /**
   * If this is false there is no outgoing plane.  The 'outgoing plane
   * distance' is then calculated by straight distance to a vertex instead of
   * projecting throught the outgoing plane
   */
  bool has_outgoing_plane;

  /// The structure describing the fog.
  csFog* fog;
};

/**
 * This structure represents all information needed for drawing
 * a scene. It is closely related to the csCamera class and modified
 * while rendering according to portals/warping portals and such.
 */
class csRenderView : public csCamera
{
public:
  /// A pointer to the world.
  csWorld* world;

  /// The 2D polygon describing how everything drawn inside should be clipped.
  csClipper* view;

  /// The 3D graphics subsystem used for drawing.
  iGraphics3D* g3d;
  /// The 2D graphics subsystem used for drawing.
  iGraphics2D* g2d;

  /// The view frustum as defined at z=1.
  float leftx, rightx, topy, boty;

  /**
   * The portal polygon (or NULL if the first sector).
   */
  csPolygon3D* portal_polygon;

  /**
   * The previous sector (or NULL if the first sector).
   */
  csSector* previous_sector;

  /**
   * This sector.
   */
  csSector* this_sector;

  /**
   * This variable holds the plane of the portal through which the camera
   * is looking.
   */
  csPlane3 clip_plane;

  /**
   * If true then we clip all objects to 'clip_plane'. In principle
   * one should always clip to 'clip_plane'. However, in many cases
   * this is not required because portals mostly arrive in at the
   * boundaries of a sector so there can actually be no objects
   * after the portal plane. But it is possible that portals arive
   * somewhere in the middle of a sector (for example with BSP sectors
   * or with Things containing portals). In that case this variable
   * will be set to true and clipping to 'clip_plane' is required.
   */
  bool do_clip_plane;

  /**
   * If true then we have to clip all objects to the portal frustum
   * (either in 2D or 3D). Normally this is not needed but some portals
   * require this. If do_clip_plane is true then the value of this
   * field is also implied to be true. The top-level portal should
   * set do_clip_frustum to true in order for all geometry to be
   * correctly clipped to screen boundaries.
   */
  bool do_clip_frustum;

  /**
   * A callback function. If this is set then no drawing is done.
   * Instead the callback function is called.
   */
  csDrawFunc* callback;

  /// Userdata belonging to the callback.
  void* callback_data;

  /**
   * Every fogged sector we encountered results in an extra structure in the
   * following list. This is only used if we are doing vertex based fog.
   */
  csFogInfo* fog_info;

  /**
   * If the following variable is true then a fog_info was added in this
   * recursion level.
   */
  bool added_fog_info;

  ///
  csRenderView ();
  ///
  csRenderView (const csCamera& c);
  ///
  csRenderView (const csCamera& c, csClipper* v, iGraphics3D* ig3d,
    iGraphics2D* ig2d);

  ///
  void SetView (csClipper* v) { view = v; }
  ///
  void SetClipPlane (csPlane3& p) { clip_plane = p; }
  /// Set the view frustum at z=1.
  void SetFrustum (float lx, float rx, float ty, float by)
  {
    leftx = lx;
    rightx = rx;
    topy = ty;
    boty = by;
  }
};

/**
 * This class is a csFrustum especially used for the lighting calculations.
 * It represents a shadow. It extends csFrustum by adding 'next' and 'prev' for
 * living in a linked list and it adds the 'polygon' member so that we can find
 * for which polygon this frustum was generated.
 */
class csShadowFrustum : public csFrustum
{
public:
  /// Linked list.
  csShadowFrustum* next, * prev;

  /**
   * Polygon which generated this shadow.
   */
  csPolygon3D* polygon;

  /**
   * Current sector when adding this frustum. This is useful to find
   * all shadow frustums added in the same recursion level for one sector
   * (together with draw_busy below).
   */
  csSector* sector;

  /**
   * draw_busy value of the sector when adding this
   * frustum. This is useful to find all shadow frustums added
   * in the same recursion level for one sector.
   */
  int draw_busy;

  /**
   * If true then this frustum is relevant. This is
   * a temporary variable which is used during the lighting
   * calculation process. It may change value several times during
   * the life time of a shadow frustum.
   */
  bool relevant;

public:
  /// Create empty frustum.
  csShadowFrustum (csVector3& origin) :
    csFrustum (origin), next (NULL), prev (NULL), polygon (NULL) { }
};

/**
 * A list of frustums.
 */
class csFrustumList
{
private:
  csShadowFrustum* first, * last;

public:
  /// Create an empty list.
  csFrustumList () : first (NULL), last (NULL) { }

  /// Destroy the list but do not destroy the individual elements!
  virtual ~csFrustumList () { }

  /// Destroy all frustums in the list.
  void DeleteFrustums ()
  {
    csShadowFrustum* sf;
    while (first)
    {
      sf = first->next;
      first->DecRef ();
      first = sf;
    }
    last = NULL;
  }

  /// Clear the list (make empty but don't delete elements).
  void Clear () { first = last = NULL; }

  /// Get the first element in this list (or NULL if empty).
  csShadowFrustum* GetFirst () { return first; }

  /// Get the last element in this list (or NULL if empty).
  csShadowFrustum* GetLast () { return last; }

  /**
   * Append a list to this one. Note that you
   * should not do any modifications on the other list
   * after this is done.
   */
  void AppendList (csFrustumList* list)
  {
    if (last)
    {
      last->next = list->GetFirst ();
      if (list->GetFirst ()) list->GetFirst ()->prev = last;
      if (list->GetLast ()) last = list->GetLast ();
    }
    else
    {
      first = list->GetFirst ();
      last = list->GetLast ();
    }
  }

  /**
   * Set the last element in this list. This basicly has
   * the effect of truncating the list to some specific element.
   * Note that this function only works if the frustum is actually
   * part of the list. No checking is done. The elements which
   * are clipped of the list are unchanged (not deleted). You
   * can relink or delete them if you want. If the given frustum
   * is NULL then this function has the same effect as making
   * the list empty.
   */
  void SetLast (csShadowFrustum* frust)
  {
    if (frust)
    {
      frust->next = NULL;
      last = frust;
    }
    else { first = last = NULL; }
  }

  /// Add a new frustum to the front of the list.
  void AddFirst (csShadowFrustum* fr)
  {
    fr->prev = NULL;
    fr->next = first;
    if (first) first->prev = fr;
    first = fr;
    if (!last) last = fr;
  }

  /// Add a new frustum to the back of the list.
  void AddLast (csShadowFrustum* fr)
  {
    fr->next = NULL;
    fr->prev = last;
    if (last) last->next = fr;
    last = fr;
    if (!first) first = fr;
  }

  /// Unlink a shadow frustum from the list.
  void Unlink (csShadowFrustum* sf)
  {
    if (sf->next) sf->next->prev = sf->prev;
    else last = sf->prev;
    if (sf->prev) sf->prev->next = sf->next;
    else first = sf->next;
  }

  /**
   * Apply a transformation to all frustums in this list.
   */
  void Transform (csTransform* trans)
  {
    csShadowFrustum* sf = first;
    while (sf)
    {
      sf->Transform (trans);
      sf = sf->next;
    }
  }
};

class csFrustumView;
class csObject;
class csOctreeNode;
typedef void (csFrustumViewFunc)(csObject* obj, csFrustumView* lview);
typedef void (csFrustumViewNodeFunc)(csOctreeNode* node, csFrustumView* lview);

/**
 * The structure for registering cleanup actions.  You can register with any
 * frustumlist object any number of cleanup routines.  For this you create
 * such a structure and pass it to RegisterCleanup () method of csFrsutumList.
 * You can derive a subclass from csFrustrumViewCleanup and keep all
 * additional data there.
 */
struct csFrustrumViewCleanup
{
  // Pointer to next cleanup action in chain
  csFrustrumViewCleanup *next;
  // The routine that is called for cleanup
  void (*action) (csFrustumView *, csFrustrumViewCleanup *);
};

/**
 * This structure represents all information needed for the frustum
 * visibility calculator.
 * @@@ This structure needs some cleanup. It contains too many
 * fields that are lighting related. These should probably go to
 * the 'userdata'.
 */
class csFrustumView
{
public:
  /// The head of cleanup actions
  csFrustrumViewCleanup *cleanup;

  /// Data for the functions below.
  void* userdata;
  /// A function that is called for every node that is visited.
  csFrustumViewNodeFunc* node_func;
  /// A function that is called for every polygon that is hit.
  csFrustumViewFunc* poly_func;
  /// A function that is called for every curve that is hit.
  csFrustumViewFunc* curve_func;

  /**
   * The current color of the light. Initially this is the same as the
   * light in csStatLight but portals may change this.
   */
  float r, g, b;

  /// Radius we want to check.
  float radius;

  /// Squared radius.
  float sq_radius;

  /// If true the we process shadows for things.
  bool things_shadow;

  /// If space is mirrored.
  bool mirror;

  /**
   * If this structure is used for dynamic light frustum calculation
   * then this flag is true.
   */
  bool dynamic;

  /**
   * If only gouraud shading should be updated then this flag is true.
   */
  bool gouraud_only;

  /**
   * If 'true' then the gouraud vertices need to be initialized (set to
   * black) first. Only the parent PolygonSet of a polygon can know this
   * because it is calculated using the current_light_frame_number.
   */
  bool gouraud_color_reset;

  /**
   * The frustum for the light. Everthing that falls in this frustum
   * is lit unless it also is in a shadow frustum.
   */
  csFrustum* light_frustum;

  /**
   * The list of shadow frustums. Note that this list will be
   * expanded with every traversal through a portal but it needs
   * to be restored to original state again before returning.
   */
  csFrustumList shadows;

  /**
   * A callback function. If this is set then no actual
   * lighting is done.
   * Instead the callback function is called.
   */
  csLightingFunc* callback;

  /// Userdata belonging to the callback.
  void* callback_data;

public:
  /// Constructor. frustum_id is generated each time a new object is created.
  csFrustumView ();
  /// Copy constructor. Everything is copied except the frustum ID
  csFrustumView (const csFrustumView &iCopy);

  /// Destroy the object
  ~csFrustumView ();

  /// Register a cleanup action to be called from destructor
  void RegisterCleanup (csFrustrumViewCleanup *action)
  { action->next = cleanup; cleanup = action; }
  /// Deregister a cleanup action
  bool DeregisterCleanup (csFrustrumViewCleanup *action);
};

#endif // __CS_RVIEW_H__
