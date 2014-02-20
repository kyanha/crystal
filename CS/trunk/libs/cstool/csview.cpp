/*
    CrystalSpace 3D renderer view
    Copyright (C) 1998 by Jorrit Tyberghein

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
#include "csqint.h"
#include "csgeom/polyclip.h"
#include "csgeom/poly2d.h"
#include "ivideo/graph3d.h"
#include "cstool/csview.h"
#include "cstool/enginetools.h"
#include "iengine/camera.h"
#include "iengine/engine.h"
#include "iengine/rendermanager.h"


csView::csView (iEngine *e, iGraphics3D* ig3d) 
  : scfImplementationType (this),
    Engine (e), G3D (ig3d), RectView (nullptr), PolyView (nullptr),
    normalizedRectView (nullptr), normalizedPolyView (nullptr),
    AutoResize (true)
{
  csRef<iPerspectiveCamera> pcam = e->CreatePerspectiveCamera ();
  SetPerspectiveCamera(pcam);

  viewWidth = G3D->GetWidth ();
  viewHeight = G3D->GetHeight ();
}

csView::~csView ()
{
  delete RectView;
  delete PolyView;
  delete normalizedRectView;
  delete normalizedPolyView;
}

iEngine* csView::GetEngine ()
{
  return Engine;
}

void csView::SetEngine (iEngine* e)
{
  Engine = e;
}

iCamera *csView::GetCamera ()
{
  return Camera;
}

void csView::SetCamera (iCamera* c)
{
  CS_ASSERT_MSG("Null camera not allowed.", c != NULL); 
  Camera = c;
}

iPerspectiveCamera *csView::GetPerspectiveCamera ()
{
  csRef<iPerspectiveCamera> pcam = scfQueryInterfaceSafe<iPerspectiveCamera>(Camera);
  return pcam;
}

void csView::SetPerspectiveCamera (iPerspectiveCamera* c)
{
  CS_ASSERT_MSG("Null camera not allowed.", c != NULL); 
  Camera = c->GetCamera();
}

iCustomMatrixCamera* csView::GetCustomMatrixCamera ()
{
  csRef<iCustomMatrixCamera> cmcam = scfQueryInterfaceSafe<iCustomMatrixCamera>(Camera);
  return cmcam;
}

void csView::SetCustomMatrixCamera (iCustomMatrixCamera* c)
{
  CS_ASSERT_MSG("Null camera not allowed.", c != nullptr); 
  Camera = c->GetCamera();
}

iGraphics3D* csView::GetContext ()
{
  return G3D;
}

void csView::SetContext (iGraphics3D *ig3d)
{
  G3D = ig3d;
}

void csView::SetRectangle (int x, int y, int w, int h, bool restrictToScreen)
{
  delete PolyView; PolyView = nullptr;
  delete normalizedPolyView; normalizedPolyView = nullptr;
  Clipper = nullptr;

  if (restrictToScreen)
  {
    // Do not allow the rectangle to go out of the screen
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > viewWidth) { w = viewWidth - x; }
    if (y + h > viewHeight) { h = viewHeight - y; }
  }

  if (RectView)
  {
    RectView->Set (x, y, x + w, y + h);
    normalizedRectView->Set (((float) x) / ((float) viewWidth),
			     ((float) y) / ((float) viewHeight),
			     ((float) x + w) / ((float) viewWidth),
			     ((float) y + h) / ((float) viewHeight));
  }
  else
  {
    RectView = new csBox2 (x, y, x + w, y + h);
    normalizedRectView = new csBox2 (((float) x) / ((float) viewWidth),
				     ((float) y) / ((float) viewHeight),
				     ((float) x + w) / ((float) viewWidth),
				     ((float) y + h) / ((float) viewHeight));
  }
}

void csView::ClearView ()
{
  Clipper = nullptr;
  delete RectView; RectView = nullptr;
  delete normalizedRectView; normalizedRectView = nullptr;

  if (PolyView)
  {
    PolyView->MakeEmpty ();
    normalizedPolyView->MakeEmpty ();
  }
}

void csView::AddViewVertex (int x, int y)
{
  if (!PolyView)
  {
    PolyView = new csPoly2D ();
    normalizedPolyView = new csPoly2D ();
  }

  PolyView->AddVertex (x, y);
  normalizedPolyView->AddVertex (((float) x) / ((float) viewWidth),
				 ((float) y) / ((float) viewHeight));

  Clipper = nullptr;
  delete RectView; RectView = nullptr;
  delete normalizedRectView; normalizedRectView = nullptr;
}

void csView::UpdateView ()
{
  if (viewWidth == G3D->GetWidth () && viewHeight == G3D->GetHeight ())
    return;

  viewWidth = G3D->GetWidth ();
  viewHeight = G3D->GetHeight ();

  iPerspectiveCamera* pcamera = GetPerspectiveCamera ();
  if (pcamera)
    pcamera->SetAspectRatio ((float) viewWidth / (float) viewHeight);

  if (PolyView)
  {
    size_t i;
    csVector2 *pverts = PolyView->GetVertices ();
    csVector2 *npverts = normalizedPolyView->GetVertices ();
    size_t InCount = PolyView->GetVertexCount ();

    // Scale the poly
    for (i = 0; i < InCount; i++)
    {
      pverts[i].x = npverts[i].x * viewWidth;
      pverts[i].y = npverts[i].y * viewHeight;
    }
  }
  else if (RectView)
  {
    RectView->Set (csQround (normalizedRectView->MinX () * viewWidth),
		   csQround (normalizedRectView->MinY () * viewHeight),
		   csQround (normalizedRectView->MaxX () * viewWidth),
		   csQround (normalizedRectView->MaxY () * viewHeight));
  }

  Clipper = nullptr;
}

void csView::Draw (iMeshWrapper* mesh)
{
  Engine->GetRenderManager()->RenderView (this);
}

void csView::UpdateClipper ()
{
  if (AutoResize) UpdateView ();

  if (!Clipper)
  {
    if (PolyView)
      Clipper.AttachNew (new csPolygonClipper (PolyView));
    else
    {
      if (!RectView)
      {
        RectView = new csBox2 (0, 0, viewWidth, viewHeight);
        normalizedRectView = new csBox2 (0.f, 0.f, 1.f, 1.f);
      }
      Clipper.AttachNew (new csBoxClipper (*RectView));
    }
  }
}

void csView::RestrictClipperToScreen ()
{
  // rectangular views are automatically restricted to screen borders,
  // so we only have to update polygon-based views
  if (PolyView)
  {
    // Clip the normalized polygon
    size_t InCount = normalizedPolyView->GetVertexCount (), OutCount;
    csBoxClipper bc (0.f, 0.f, 1.f, 1.f);
    csVector2 *TempPoly = new csVector2[InCount + 5];
    uint8 rc = bc.Clip (normalizedPolyView->GetVertices (), InCount , TempPoly, OutCount);
    if (rc != CS_CLIP_OUTSIDE)
    {
      normalizedPolyView->MakeRoom (OutCount);
      normalizedPolyView->SetVertices (TempPoly, OutCount);
      //@@@normalizedPolyView->UpdateBoundingBox ();
    }
    delete [] TempPoly;

    // Copy and scale into the actual polygon
    InCount = normalizedPolyView->GetVertexCount ();
    PolyView->SetVertexCount (InCount);
    csVector2 *pverts = PolyView->GetVertices ();
    csVector2 *npverts = normalizedPolyView->GetVertices ();
     for (size_t i = 0; i < InCount; i++)
    {
      pverts[i].x = npverts[i].x * viewWidth;
      pverts[i].y = npverts[i].y * viewHeight;
    }
  }
}

iClipper2D* csView::GetClipper ()
{
  UpdateClipper ();
  return Clipper;
}

csVector2 csView::Project (const csVector3& v) const
{
  return csEngineTools::NormalizedToScreen
    (Camera->Project (v), viewWidth, viewHeight);
}

csVector3 csView::InvProject (const csVector2& p, float z) const
{
  return Camera->InvProject
    (csEngineTools::ScreenToNormalized (p, viewWidth, viewHeight), z);
}
