/*
    Copyright (C) 2012 by Christian Van Brussel

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
#include "ieditor/context.h"
#include "iengine/camera.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "iengine/sector.h"
#include "imesh/objmodel.h"
#include "iutil/object.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"

#include "2dinfo.h"

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{
  
SCF_IMPLEMENT_FACTORY (CS2DInfo)

CS2DInfo::CS2DInfo (iBase* parent)
  : scfImplementationType (this, parent)
{
}

CS2DInfo::~CS2DInfo ()
{
  if (frameListener)
  {
    csRef<iEventQueue> eventQueue = csQueryRegistry<iEventQueue> (object_reg);
    eventQueue->RemoveListener (frameListener);
  }
}

bool CS2DInfo::Initialize (iEditor* editor)
{
  this->editor = editor;
  object_reg = editor->GetContext ()->GetObjectRegistry ();

  g3d = csQueryRegistry<iGraphics3D> (object_reg);

  // Load the font
  csRef<iFontServer> fontServer = g3d->GetDriver2D ()->GetFontServer ();
  if (!fontServer) return ReportError ("Failed to locate font server!");

  font = fontServer->LoadFont (CSFONT_COURIER);
  if (!font) return ReportError ("Failed to load font!");

  // Register a frame listener to the global event queue
  frameListener.AttachNew (new CS2DInfo::FrameListener (this));

  return true;
}

void CS2DInfo::Update ()
{
}

void CS2DInfo::Save (iDocumentNode* node) const
{
}

bool CS2DInfo::Load (iDocumentNode* node)
{
  return false;
}

bool CS2DInfo::HandleEvent (iEvent& event)
{
  return false;
}

void CS2DInfo::UpdateFrame ()
{
  csRef<iContextCamera> cameraContext = scfQueryInterface<iContextCamera> (editor->GetContext ());
  iCamera* camera = cameraContext->GetCamera ();
  if (!camera || !camera->GetSector ())
    return;

  csRef<iContextObjectSelection> objectSelectionContext =
    scfQueryInterface<iContextObjectSelection> (editor->GetContext ());
  const csWeakRefArray<iObject>& selection = objectSelectionContext->GetSelectedObjects ();

  iGraphics2D* g2d = g3d->GetDriver2D ();

  // TODO: BeginDraw () don't work for the wx canvas?
  //if (!g2d->BeginDraw ()) return;
  if (!g3d->BeginDraw (CSDRAW_2DGRAPHICS)) return;

  // Display the orientation of the camera
  float axisSize = 45.f;
  float centerX = axisSize + 10.f;
  float centerY = g2d->GetHeight () - axisSize - 10.f;
  float textOffset = 5.f;
  int lineSize = 18;

  csVector3 end = camera->GetTransform ().GetO2T () * csVector3 (axisSize, 0.f, 0.f);
  int color = g2d->FindRGB (255, 0, 0);
  g2d->DrawLine (centerX, centerY, centerX + end[0], centerY + end[1], color);
  g2d->Write (font, centerX + end[0] + textOffset, centerY + end[1] - textOffset - lineSize, color, -1, "x");

  end = camera->GetTransform ().GetO2T () * csVector3 (0.f, -axisSize, 0.f);
  color = g2d->FindRGB (0, 255, 0);
  g2d->DrawLine (centerX, centerY, centerX + end[0], centerY + end[1], color);
  g2d->Write (font, centerX + end[0] + textOffset, centerY + end[1] - textOffset - lineSize, color, -1, "y");

  end = camera->GetTransform ().GetO2T () * csVector3 (0.f, 0.f, axisSize);
  color = g2d->FindRGB (0, 0, 255);
  g2d->DrawLine (centerX, centerY, centerX + end[0], centerY + end[1], color);
  g2d->Write (font, centerX + end[0] + textOffset, centerY + end[1] - textOffset - lineSize, color, -1, "z");

  // Display the textual information
  int x = 2.f * (centerX + textOffset);
  int y = g2d->GetHeight () - 2 * lineSize;
  int fontColor = g2d->FindRGB (255, 255, 255);

  csString sectorName = camera->GetSector ()->QueryObject ()->GetName ();
  if (sectorName != "viewmesh")
  {
    WriteShadow (x, y, fontColor, "Sector: %s", sectorName.GetData ());
    y -= lineSize;
  }

  csVector3 origin = camera->GetTransform ().GetOrigin ();
  WriteShadow (x, y, fontColor, "Position: %.3f, %.3f, %.3f", origin[0], origin[1], origin[2]);
  y -= lineSize;

  if (sectorName == "viewmesh")
  {
    csString name = camera->GetSector ()->GetMeshes ()->Get (0)->GetFactory ()->QueryObject ()->GetName ();
    if (!name) name = "Unnamed object";
    WriteShadow (x, y, fontColor, "%s", name.GetData ());
    y -= lineSize;
  }

  else if (selection.GetSize ())
  {
    if (selection.GetSize () == 1)
    {
      csString name = selection[0]->GetName ();
      if (!name) name = "Unnamed object";
      WriteShadow (x, y, fontColor, "%s", name.GetData ());
    }
    else
      WriteShadow (x, y, fontColor, "%i objects selected", selection.GetSize ());
    y -= lineSize;
  }

  // Display the bounding box of the selected objects
  int bbox_color = g2d->FindRGB (128, 255, 128);
  for (size_t i = 0; i < selection.GetSize (); i++)
  {
    iObject* object = selection[i];
    if (!object) continue;

    csRef<iMeshWrapper> mesh = scfQueryInterface<iMeshWrapper> (object);
    if (!mesh) continue;

    g2d->DrawBoxProjected (mesh->GetMeshObject ()->GetObjectModel ()->GetObjectBoundingBox (),
			   camera->GetTransform () * mesh->GetMovable ()->GetFullTransform ().GetInverse (),
			   camera->GetProjectionMatrix (), bbox_color);
  }

  //g2d->FinishDraw ();
  g3d->FinishDraw ();
}

void CS2DInfo::WriteShadow (int x, int y, int fg, const char *str,...) const
{
  csString buf;
  va_list arg;

  va_start (arg, str);
  buf.FormatV (str, arg);
  va_end (arg);

  Write (x + 1, y - 1, 0, -1, "%s", buf.GetData ());
  Write (x, y, fg, -1, "%s", buf.GetData ());
}

void CS2DInfo::Write (int x, int y, int fg, int bg, const char *str,...) const
{
  csString buf;
  va_list arg;

  va_start (arg,str);
  buf.FormatV (str, arg);
  va_end (arg);

  g3d->GetDriver2D ()->Write (font, x, y, fg, bg, buf);
}

// -------------------------------------------------------------------------------------

CS2DInfo::FrameListener::FrameListener (CS2DInfo* info)
  : scfImplementationType (this), info (info)
{
  csRef<iEventQueue> eventQueue =
    csQueryRegistry<iEventQueue> (info->object_reg);
  eventQueue->RegisterListener (this, csevFrame (info->object_reg));
}

bool CS2DInfo::FrameListener::HandleEvent (iEvent &event)
{
  info->UpdateFrame ();
  return false;
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)
