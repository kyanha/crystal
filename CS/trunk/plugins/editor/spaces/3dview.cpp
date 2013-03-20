/*
    Copyright (C) 2012 by Christian Van Brussel
    Copyright (C) 2007 by Seth Yastrov

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
#include "cstool/csview.h"
#include "cstool/initapp.h"
#include "csutil/scf.h"
#include "ieditor/context.h"
#include "iengine/camera.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/wxwin.h"

#include "3dview.h"

#include <wx/wx.h>

CS_PLUGIN_NAMESPACE_BEGIN(CSEditor)
{

BEGIN_EVENT_TABLE(CS3DSpace::Space, wxPanel)
  EVT_SIZE(CS3DSpace::Space::OnSize)
END_EVENT_TABLE()

SCF_IMPLEMENT_FACTORY (CS3DSpace)

CS3DSpace::CS3DSpace (iBase* parent)
  : scfImplementationType (this, parent), object_reg (0),
  enabled (true), updateCount (0), frameBegin3DDraw (nullptr),
  framePrinter (nullptr)
{
}

CS3DSpace::~CS3DSpace()
{
  delete frameBegin3DDraw;
  delete framePrinter;
}

bool CS3DSpace::Initialize (iObjectRegistry* obj_reg, iEditor* editor,
			    iSpaceFactory* fact, wxWindow* parent)
{
  // Initialize the main pointers
  object_reg = obj_reg;
  this->editor = editor;
  factory = fact;

  g3d = csQueryRegistry<iGraphics3D> (object_reg);
  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);

  // Setup the 2D canvas
  iGraphics2D* g2d = g3d->GetDriver2D ();
  g2d->AllowResize (true);

  csRef<iWxWindow> wxwin = scfQueryInterface<iWxWindow> (g2d);
  if (!wxwin)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
              "crystalspace.editor.space.3dview",
              "The 2D canvas is not a iWxWindow interface!");
    return false;
  }

  // Create the main wx window
  window = new CS3DSpace::Space
    (this, parent, -1, wxPoint (0,0), wxSize (-1,-1));
  wxwin->SetParent (window);

  // Listen to the 'enter window' event in order to put back the focus on this window
  wxwin->GetWindow ()->Connect
    (wxEVT_ENTER_WINDOW, wxMouseEventHandler (CS3DSpace::OnEnterWindow), 0, this);
  
  // Setup the view and the camera context
  view = csPtr<iView> (new csView (engine, g3d));
  view->SetAutoResize (false);
  view->SetRectangle (0, 0, g2d->GetWidth (), g2d->GetHeight (), false);

  csRef<iContextCamera> cameraContext =
    scfQueryInterface<iContextCamera> (editor->GetContext ());
  cameraContext->SetCamera (view->GetCamera ());

  // Register frame listeners to the global event queue
  frameBegin3DDraw = new CS3DSpace::FrameBegin3DDraw (this);
  framePrinter = new CS3DSpace::FramePrinter (this);

  return true;
}

wxWindow* CS3DSpace::GetwxWindow ()
{
  return window;
}

void CS3DSpace::SetEnabled (bool enabled)
{
  this->enabled = enabled;
  if (enabled)
  {
    updateCount = 0;

    // Force the reparenting of the wxGL canvas
    csRef<iWxWindow> wxwin =
      scfQueryInterface<iWxWindow> (g3d->GetDriver2D ());
    wxwin->SetParent (window);
  }
}

bool CS3DSpace::GetEnabled () const
{
  return enabled;
}

void CS3DSpace::OnFrameBegin ()
{
  // Hack around an impossibility before wxWidgets 2.9 to know whether
  // or not the wx OpenGL context has been correctly initialized. The
  // workaround here is to simply wait some time before drawing anything
  // in the hope that the context has got the time to be initialized.
  if (!enabled || updateCount < 5) return;

  // Tell the 3D driver we're going to display 3D things.
  if (!g3d->BeginDraw (CSDRAW_3DGRAPHICS))
    return;

  // Render the view into the frame buffer.
  view->Draw ();
}

void CS3DSpace::OnFramePrint ()
{
  if (!enabled || updateCount < 5) return;

  // Finish the drawing
  g3d->FinishDraw ();
  g3d->Print (0);
}

void CS3DSpace::Update ()
{
  updateCount++;
}

void CS3DSpace::OnSize (wxSizeEvent& event)
{
  if (!g3d.IsValid () || !g3d->GetDriver2D () || !view.IsValid ())
    return;
  
  wxSize size = event.GetSize ();
  iGraphics2D* g2d = g3d->GetDriver2D ();
  
  // Also resize the wxWindow
  csRef<iWxWindow> wxwin = scfQueryInterface<iWxWindow> (g2d);
  if (!wxwin->GetWindow ())
  {
    g2d->Resize (size.x, size.y);
    return;
  }
  
  wxwin->GetWindow()->SetSize (size);
  
  // Update the view ratio
  if (view->GetPerspectiveCamera ())
    view->GetPerspectiveCamera ()->SetFOV
      ((float) (size.y) / (float) (size.x), 1.0f);
  view->SetRectangle (0, 0, size.x, size.y, false);

  event.Skip ();
}

void CS3DSpace::OnEnterWindow (wxMouseEvent& event)
{
  csRef<iWxWindow> wxwin = scfQueryInterface<iWxWindow> (g3d->GetDriver2D ());
  wxwin->GetWindow ()->SetFocus ();
}

//---------------------------------------------------------------

CS3DSpace::FrameBegin3DDraw::FrameBegin3DDraw (CS3DSpace* space)
  : scfImplementationType (this), space (space)
{
  csRef<iEventQueue> queue =
    csQueryRegistry<iEventQueue> (space->object_reg);
  queue->RegisterListener (this, csevFrame (space->object_reg));
}

CS3DSpace::FrameBegin3DDraw::~FrameBegin3DDraw ()
{
  csRef<iEventQueue> queue =
    csQueryRegistry<iEventQueue> (space->object_reg);
  queue->RemoveListener (this);
}

bool CS3DSpace::FrameBegin3DDraw::HandleEvent (iEvent &event)
{
  space->OnFrameBegin ();
  return false;
}

//---------------------------------------------------------------

CS3DSpace::FramePrinter::FramePrinter (CS3DSpace* space)
  : scfImplementationType (this), space (space)
{
  csRef<iEventQueue> queue =
    csQueryRegistry<iEventQueue> (space->object_reg);
  queue->RegisterListener (this, csevFrame (space->object_reg));
}

CS3DSpace::FramePrinter::~FramePrinter ()
{
  csRef<iEventQueue> queue =
    csQueryRegistry<iEventQueue> (space->object_reg);
  queue->RemoveListener (this);
}

bool CS3DSpace::FramePrinter::HandleEvent (iEvent &event)
{
  space->OnFramePrint ();
  return false;
}

}
CS_PLUGIN_NAMESPACE_END(CSEditor)

