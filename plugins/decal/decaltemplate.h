/*
    Copyright (C) 2006 by Andrew Robberts

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

#ifndef __CS_DECAL_TEMPLATE_H__
#define __CS_DECAL_TEMPLATE_H__

#include "igeom/decal.h"
#include "csutil/scf_implementation.h"
#include "csgeom/vector2.h"

#define CS_DECAL_DEFAULT_TIME_TO_LIVE               -1.0f
#define CS_DECAL_DEFAULT_RENDER_PRIORITY            0
#define CS_DECAL_DEFAULT_NORMAL_THRESHOLD           0.01f
#define CS_DECAL_DEFAULT_OFFSET                     0.05f
#define CS_DECAL_DEFAULT_TOP_CLIP_ON                true
#define CS_DECAL_DEFAULT_TOP_CLIP_SCALE             0.5f
#define CS_DECAL_DEFAULT_BOTTOM_CLIP_ON             true
#define CS_DECAL_DEFAULT_BOTTOM_CLIP_SCALE          0.5f
#define CS_DECAL_DEFAULT_PERPENDICULAR_THRESHOLD    0.05
#define CS_DECAL_DEFAULT_PERPENDICULAR_OFFSET       0.01

class csDecalTemplate : public scfImplementation1<csDecalTemplate,
                                                 iDecalTemplate>
{
private:
  float                 timeToLive;
  iMaterialWrapper*     material;
  long                  renderPriority;
  csZBufMode            zBufMode;
  float                 polygonNormalThreshold;
  float                 decalOffset;
  bool                  hasTopClip;
  float                 topClipScale;
  bool                  hasBottomClip;
  float                 bottomClipScale;
  csVector2             minTexCoord;
  csVector2             maxTexCoord;
  uint                  mixMode;
  float                 perpendicularFaceThreshold;
  float                 perpendicularFaceOffset;

public:

  csDecalTemplate();
  csDecalTemplate(iBase* parent);
  virtual ~csDecalTemplate();

  virtual float GetTimeToLive() const;
  virtual iMaterialWrapper* GetMaterialWrapper();
  virtual long GetRenderPriority() const;
  virtual csZBufMode GetZBufMode() const;
  virtual float GetPolygonNormalThreshold() const;
  virtual float GetDecalOffset() const;
  virtual bool HasTopClipping() const;
  virtual float GetTopClippingScale() const;
  virtual bool HasBottomClipping() const;
  virtual bool GetBottomClippingScale() const;
  virtual const csVector2 & GetMinTexCoord() const;
  virtual const csVector2 & GetMaxTexCoord() const;
  virtual const uint GetMixMode() const;
  virtual float GetPerpendicularFaceThreshold() const;
  virtual float GetPerpendicularFaceOffset() const;

  virtual void SetTimeToLive(float timeToLive);
  virtual void SetMaterialWrapper(iMaterialWrapper* material);
  virtual void SetRenderPriority(long renderPriority);
  virtual void SetZBufMode(csZBufMode mode);
  virtual void SetPolygonNormalThreshold(float polygonNormalThreshold);
  virtual void SetDecalOffset(float decalOffset);
  virtual void SetTopClipping(bool enabled, float topPlaneScale);
  virtual void SetBottomClipping(bool enabled, float bottomPlaneScale);
  virtual void SetTexCoords(const csVector2 & min, const csVector2 & max);
  virtual void SetMixMode(uint mixMode);
  virtual void SetPerpendicularFaceThreshold(float threshold);
  virtual void SetPerpendicularFaceOffset(float offset);
};

#endif // __CS_DECAL_TEMPLATE_H__
