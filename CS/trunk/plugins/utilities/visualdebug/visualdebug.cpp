/*
  Copyright (C) 2010 Christian Van Brussel, Institute of Information
  and Communication Technologies, Electronics and Applied Mathematics
  at Universite catholique de Louvain, Belgium
  http://www.uclouvain.be/en-icteam.html

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
#include "csutil/scf.h"

#include "visualdebug.h"
#include "csqint.h"
#include "iengine/camera.h"
#include "ivaria/view.h"
#include "ivideo/graph2d.h"

CS_PLUGIN_NAMESPACE_BEGIN(VisualDebug)
{

  SCF_IMPLEMENT_FACTORY(VisualDebugger);

  CS_LEAKGUARD_IMPLEMENT(VisualDebugger);

  VisualDebugger::VisualDebugger (iBase* parent)
    : scfImplementationType (this, parent)
  {
  }

  bool VisualDebugger::Initialize (iObjectRegistry* object_reg)
  {
    this->object_reg = object_reg;
    return true;
  }

  void VisualDebugger::DebugTransform (const csReversibleTransform& transform,
				       bool persist, float size)
  {
    TransformData data;
    data.transform = transform;
    data.persist = persist;
    data.size = size;
    transforms.Push (data);
  }

  void VisualDebugger::DebugPosition (const csVector3& position,
				      bool persist,
				      csColor color,
				      size_t size)
  {
    PositionData data;
    data.position = position;
    data.persist = persist;
    data.color = color;
    data.size = size;
    positions.Push (data);
  }

  void VisualDebugger::DebugVector (const csReversibleTransform& transform, const csVector3& vector,
				    bool persist, csColor color)
  {
    VectorData data;
    data.transform = transform;
    data.vector = vector;
    data.persist = persist;
    data.color = color;
    vectors.Push (data);
  }

  void VisualDebugger::Display (iView* view)
  {
    iGraphics3D* g3d = view->GetContext ();
    iGraphics2D* g2d = g3d->GetDriver2D ();
    csTransform tr_w2c = view->GetCamera ()->GetTransform ();
    const CS::Math::Matrix4& projection (g3d->GetProjectionMatrix ());

    if (!g3d->BeginDraw (CSDRAW_2DGRAPHICS))
      return;

    // Display transforms
    size_t index = transforms.GetSize () - 1;
    for (csArray<TransformData>::ReverseIterator it = transforms.GetReverseIterator ();
	 it.HasNext (); )
    {
      TransformData& transform = it.Next ();

      csVector3 origin = transform.transform.GetOrigin ();
      csVector3 end = transform.transform.This2Other (csVector3 (transform.size, 0.0f, 0.0f));
      int color = g2d->FindRGB (255, 0, 0);
      g2d->Draw3DLine (tr_w2c * origin, tr_w2c * end, projection, color);

      end = transform.transform.This2Other (csVector3 (0.0f, transform.size, 0.0f));
      color = g2d->FindRGB (0, 255, 0);
      g2d->Draw3DLine (tr_w2c * origin, tr_w2c * end, projection, color);

      end = transform.transform.This2Other (csVector3 (0.0f, 0.0f, transform.size));
      color = g2d->FindRGB (0, 0, 255);
      g2d->Draw3DLine (tr_w2c * origin, tr_w2c * end, projection, color);

      if (!transform.persist)
	transforms.DeleteIndex (index);

      index--;
    }

    // Display positions
    index = positions.GetSize () - 1;
    for (csArray<PositionData>::ReverseIterator it = positions.GetReverseIterator ();
	 it.HasNext (); )
    {
      PositionData& positionData = it.Next ();
      int color = g2d->FindRGB (255.0f * positionData.color[0],
				255.0f * positionData.color[1],
				255.0f * positionData.color[2]);
      csVector3 position = tr_w2c * positionData.position;

      if (position < SMALL_Z)
	continue;

      csVector4 v1p (projection * csVector4 (position));
      v1p /= v1p.w;

      int px1 = csQint ((v1p.x + 1) * (g2d->GetWidth() / 2));
      int py1 = g2d->GetHeight () - 1 - csQint ((v1p.y + 1) * (g2d->GetHeight () / 2));

      for (size_t i = 0; i < positionData.size; i++)
	for (size_t j = 0; j < positionData.size; j++)
	  g3d->GetDriver2D ()->DrawPixel (px1 - positionData.size / 2 + i,
					  py1 - positionData.size / 2 + j,
					  color);

      if (!positionData.persist)
	positions.DeleteIndex (index);

      index--;
    }

    // Display vectors
    index = vectors.GetSize () - 1;
    for (csArray<VectorData>::ReverseIterator it = vectors.GetReverseIterator ();
	 it.HasNext (); )
    {
      VectorData& vectorData = it.Next ();
      int color = g2d->FindRGB (255.0f * vectorData.color[0],
				255.0f * vectorData.color[1],
				255.0f * vectorData.color[2]);
      csVector3 origin = vectorData.transform.GetOrigin ();
      csVector3 end = origin + vectorData.transform.This2OtherRelative (vectorData.vector);
      g2d->Draw3DLine (tr_w2c * origin, tr_w2c * end, projection, color);

      if (!vectorData.persist)
	vectors.DeleteIndex (index);

      index--;
    }
  }

}
CS_PLUGIN_NAMESPACE_END(VisualDebug)
