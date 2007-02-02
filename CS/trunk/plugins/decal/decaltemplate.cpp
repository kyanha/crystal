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

#include "cssysdef.h"
#include "decaltemplate.h"

SCF_IMPLEMENT_FACTORY(csDecalTemplate)

csDecalTemplate::csDecalTemplate()
    : scfImplementationType(this),
      timeToLive(CS_DECAL_DEFAULT_TIME_TO_LIVE),
      material(0),
      renderPriority(CS_DECAL_DEFAULT_RENDER_PRIORITY),
      zBufMode(CS_ZBUF_TEST),
      polygonNormalThreshold(CS_DECAL_DEFAULT_NORMAL_THRESHOLD),
      decalOffset(CS_DECAL_DEFAULT_OFFSET),
      hasNearFarClipping(CS_DECAL_DEFAULT_CLIP_NEAR_FAR),
      nearFarClippingDist(CS_DECAL_DEFAULT_NEAR_FAR_DIST),
      minTexCoord(0,0),
      maxTexCoord(1,1),
      mixMode(CS_FX_COPY)
{
}

csDecalTemplate::csDecalTemplate(iBase* parent)
    : scfImplementationType(this, parent)
{
}

csDecalTemplate::~csDecalTemplate()
{
}

float csDecalTemplate::GetTimeToLive() const
{
    return timeToLive;
}

iMaterialWrapper* csDecalTemplate::GetMaterialWrapper()
{
    return material;
}

long csDecalTemplate::GetRenderPriority() const
{
    return renderPriority;
}

csZBufMode csDecalTemplate::GetZBufMode() const
{
    return zBufMode;
}

float csDecalTemplate::GetPolygonNormalThreshold() const
{
    return polygonNormalThreshold;
}

float csDecalTemplate::GetDecalOffset() const
{
    return decalOffset;
}

bool csDecalTemplate::HasNearFarClipping() const
{
    return hasNearFarClipping;
}

float csDecalTemplate::GetNearFarClippingDist() const
{
    return nearFarClippingDist;
}

const csVector2 & csDecalTemplate::GetMinTexCoord() const
{
    return minTexCoord;
}

const csVector2 & csDecalTemplate::GetMaxTexCoord() const
{
    return maxTexCoord;
}

const uint csDecalTemplate::GetMixMode() const
{
  return mixMode;
}

void csDecalTemplate::SetTimeToLive(float timeToLive)
{
    this->timeToLive = timeToLive;
}

void csDecalTemplate::SetMaterialWrapper(iMaterialWrapper* material)
{
    this->material = material;
}

void csDecalTemplate::SetRenderPriority(long renderPriority)
{
    this->renderPriority = renderPriority;
}

void csDecalTemplate::SetZBufMode(csZBufMode mode)
{
    this->zBufMode = mode;
}

void csDecalTemplate::SetPolygonNormalThreshold(float polygonNormalThreshold)
{
    this->polygonNormalThreshold = polygonNormalThreshold;
}

void csDecalTemplate::SetDecalOffset(float decalOffset)
{
    this->decalOffset = decalOffset;
}

void csDecalTemplate::SetNearFarClipping(bool enabled)
{
    this->hasNearFarClipping = enabled;
}

void csDecalTemplate::SetNearFarClippingDist(float dist)
{
    this->nearFarClippingDist = dist;
}

void csDecalTemplate::SetTexCoords(const csVector2 & min, 
    const csVector2 & max)
{
    this->minTexCoord = min;
    this->maxTexCoord = max;
}

void csDecalTemplate::SetMixMode(uint mixMode)
{
  this->mixMode = mixMode;
}

