/*
    Copyright (C) 2011-2012 Christian Van Brussel, Institute of Information
      and Communication Technologies, Electronics and Applied Mathematics
      at Universite catholique de Louvain, Belgium
      http://www.uclouvain.be/en-icteam.html
    Copyright (C) 2012 by Dominik Seifert
    Copyright (C) 2011 by Liu Lu

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

#ifndef __CS_BULLET_COMMON2_H__
#define __CS_BULLET_COMMON2_H__

#include "bulletsystem.h"
#include "bulletsector.h"
#include "csgeom/matrix3.h"
#include "csgeom/transfrm.h"
#include "csgeom/vector3.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivaria/view.h"
#include "iengine/camera.h"
#include "csutil/cscolor.h"
#include "btBulletCollisionCommon.h"
#include "motionstates.h"


CS_PLUGIN_NAMESPACE_BEGIN(Bullet2)
{

class csBulletCollisionObject;

//----------------------- Bullet-CS matrices and vectors conversion ----------------------------

static inline csReversibleTransform BulletToCS (const btTransform& trans,
						float inverseInternalScale)
{
  const btVector3& trans_o = trans.getOrigin ();
  csVector3 origin (trans_o.getX () * inverseInternalScale,
		    trans_o.getY () * inverseInternalScale,
		    trans_o.getZ () * inverseInternalScale);
  const btMatrix3x3& trans_m = trans.getBasis ();
  const btVector3& row0 = trans_m.getRow (0);
  const btVector3& row1 = trans_m.getRow (1);
  const btVector3& row2 = trans_m.getRow (2);
  csMatrix3 m (
      row0.getX (), row1.getX (), row2.getX (),
      row0.getY (), row1.getY (), row2.getY (),
      row0.getZ (), row1.getZ (), row2.getZ ());
  return csReversibleTransform (m, origin);
}

static inline btTransform CSToBullet (const csReversibleTransform& tr,
				      float internalScale)
{
  const csVector3& origin = tr.GetOrigin ();
  btVector3 trans_o (origin.x * internalScale,
		     origin.y * internalScale,
		     origin.z * internalScale);
  const csMatrix3& m = tr.GetO2T ();
  btMatrix3x3 trans_m (
      m.m11, m.m21, m.m31,
      m.m12, m.m22, m.m32,
      m.m13, m.m23, m.m33);
  return btTransform (trans_m, trans_o);
}

static inline btMatrix3x3 CSToBullet (const csMatrix3& m)
{
  return btMatrix3x3 (
      m.m11, m.m21, m.m31,
      m.m12, m.m22, m.m32,
      m.m13, m.m23, m.m33);
}

static inline csVector3 BulletToCS (const btVector3& v,
				    float inverseInternalScale)
{
  return csVector3 (v.getX () * inverseInternalScale,
		    v.getY () * inverseInternalScale,
		    v.getZ () * inverseInternalScale);
}

static inline btVector3 CSToBullet (const csVector3& v,
				    float internalScale)
{
  return btVector3 (v.x * internalScale,
		    v.y * internalScale,
		    v.z * internalScale);
}

/// Returns the index'th component of the given vector
static inline btScalar& BulletVectorComponent(btVector3& v, int index)
{
  return v.m_floats[index];
}

/// Returns the index'th component of the given vector
static inline const btScalar& BulletVectorComponent(const btVector3& v, int index)
{
  return v.m_floats[index];
}

/**
 * Computes the index into a 1D bullet array, given a 2D CS index (in x/y)
 */
static inline int CSToBulletIndex2D (int x, int y, int w, int h)
{
  return (h-y-1) * w + x;
}


/*
 * Returns the reflection direction of a ray going 'direction' hitting a surface with normal 'normal'
 *
 * from: http://www-cs-students.stanford.edu/~adityagp/final/node3.html
 */
static inline btVector3 BtVectorComputeReflectionDirection (const btVector3& direction, const btVector3& normal)
{
	return direction - (btScalar(2.0) * direction.dot(normal)) * normal;
}

/*
 * Returns the portion of 'direction' that is parallel to 'normal'
 */
static inline btVector3 BtVectorNormalComponent (const btVector3& direction, const btVector3& normal)
{
  btScalar magnitude = direction.dot(normal);
  return normal * magnitude;
}

/*
 * Returns the portion of 'direction' that is perpendicular to 'normal'
 */
static inline btVector3 BtVectorTangentialComponent(const btVector3& direction, const btVector3& normal)
{
  return direction - BtVectorNormalComponent(direction, normal);
}

//----------------------- DowncastPtr ----------------------------

/**
 * Very ugly and inefficient work-around to easily cast between two known-to-be-compatible types
 */
template<typename T, typename T2>
inline csPtr<T> DowncastPtr(csPtr<T2> ptr)
{
  return csPtr<T>(csRef<T>(csRef<T2>(ptr)));
}

//----------------------- csBulletDebugDraw ----------------------------

struct csBulletDebugLine
{
  csVector3 p1, p2;
  csColor color;
};

class csBulletDebugDraw : public btIDebugDraw
{
private:
  csArray<csBulletDebugLine> lines;
  int mode;
  float inverseInternalScale;

public:
  csBulletDebugDraw (float inverseInternalScale)
    : mode (DBG_DrawWireframe | DBG_DrawConstraints | DBG_DrawConstraintLimits),
    inverseInternalScale (inverseInternalScale)
  {
  }

  virtual ~csBulletDebugDraw () { }

  virtual void drawLine (const btVector3& from, const btVector3& to,
    const btVector3& color)
  {
    csBulletDebugLine l;
    l.p1.Set (BulletToCS (from, inverseInternalScale));
    l.p2.Set (BulletToCS (to, inverseInternalScale));
    l.color.Set (color.getX (), color.getY (), color.getZ ());
    lines.Push (l);
  }

  virtual void drawContactPoint (const btVector3 &PointOnB,
    const btVector3 &normalOnB,
    btScalar distance, int lifeTime,
    const btVector3 &color)
  {
    btVector3 to = PointOnB+normalOnB*1;//distance;
    const btVector3& from = PointOnB;
    csBulletDebugLine l;
    l.p1.Set (BulletToCS (from, inverseInternalScale));
    l.p2.Set (BulletToCS (to, inverseInternalScale));
    l.color.Set (color.getX (), color.getY (), color.getZ ());
    lines.Push (l);
  }

  virtual void reportErrorWarning (const char *warningString)
  {
    printf ("%s", warningString);
  }

  virtual void draw3dText (const btVector3 &location, const char *textString)
  {}

  void SetDebugMode (CS::Physics::DebugMode mode)
  {
    this->mode = 0;
    if (mode & CS::Physics::DEBUG_COLLIDERS)
      this->mode |= DBG_DrawWireframe;
    if (mode & CS::Physics::DEBUG_AABB)
      this->mode |= DBG_DrawAabb;
    if (mode & CS::Physics::DEBUG_JOINTS)
      this->mode |= DBG_DrawConstraints | DBG_DrawConstraintLimits;
  }

  CS::Physics::DebugMode GetDebugMode ()
  {
    CS::Physics::DebugMode mode = CS::Physics::DEBUG_NOTHING;
    if (this->mode & DBG_DrawWireframe)
      mode = (CS::Physics::DebugMode) (mode | CS::Physics::DEBUG_COLLIDERS);
    if (this->mode & DBG_DrawAabb)
      mode = (CS::Physics::DebugMode) (mode | CS::Physics::DEBUG_AABB);
    if (this->mode & DBG_DrawConstraints)
      mode = (CS::Physics::DebugMode) (mode | CS::Physics::DEBUG_JOINTS);
    return mode;
  }

  virtual void StartProfile ()
  {
    this->mode |= DBG_ProfileTimings;
  }

  virtual void StopProfile ()
  {
    this->mode &= this->mode & ~DBG_ProfileTimings;
  }

  virtual void setDebugMode (int m)
  {
    mode = m;
  }

  virtual int getDebugMode () const
  {
    return mode;
  }

  void DebugDraw (iGraphics3D* g3d, iCamera* camera)
  {
    iGraphics2D* g2d = g3d->GetDriver2D ();
    csTransform tr_w2c = camera->GetTransform ();
    const CS::Math::Matrix4& projection (camera->GetProjectionMatrix ());

    if (!g3d->BeginDraw (CSDRAW_2DGRAPHICS))
      return;

    for (size_t i = 0 ; i < lines.GetSize () ; i++)
    {
      csBulletDebugLine& l = lines[i];
      int color = g2d->FindRGB (int (l.color.red * 255),
        int (l.color.green * 255),
        int (l.color.blue * 255));
      g2d->DrawLineProjected (tr_w2c * l.p1, tr_w2c * l.p2, projection, color);
    }

    lines.Empty ();
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Some extra Bullet vector operations

///Reflect the vector d around the vector r
inline btVector3 reflect( const btVector3& d, const btVector3& r )
{
  return d - ( btScalar( 2.0 ) * d.dot( r ) ) * r;
}


///Project a vector u on another vector v
inline btVector3 project( const btVector3& u, const btVector3& v )
{
  return v * u.dot( v );
}


///Helper for computing the character sliding
inline btVector3 slide( const btVector3& direction, const btVector3& planeNormal )
{
  return direction - project( direction, planeNormal );
}

inline btVector3 slideOnCollision( const btVector3& fromPosition, const btVector3& toPosition, const btVector3& hitNormal )
{
  btVector3 moveDirection = toPosition - fromPosition;
  btScalar moveLength = moveDirection.length();

  if( moveLength <= btScalar( SIMD_EPSILON ) )
    return toPosition;

  moveDirection.normalize();

  btVector3 reflectDir = reflect( moveDirection, hitNormal );
  reflectDir.normalize();

  return fromPosition + slide( reflectDir, hitNormal ) * moveLength;
}

}
CS_PLUGIN_NAMESPACE_END(Bullet2)

#endif //__CS_BULLET_COMMON2_H__
