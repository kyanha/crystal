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
#include "cstool/initapp.h"
#include "csutil/scf.h"
#include "ieditor/context.h"
#include "ieditor/operator.h"
#include "iengine/engine.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/wxwin.h"

#include "cs3dengine.h"

CS_PLUGIN_NAMESPACE_BEGIN(CSEditor)
{

SCF_IMPLEMENT_FACTORY (CS3DEngine)

CS3DEngine::CS3DEngine (iBase* parent)
  : scfImplementationType (this, parent)
{

}

CS3DEngine::~CS3DEngine()
{

}

bool CS3DEngine::Initialize (iEditor* editor)
{
  object_reg = editor->GetContext()->GetObjectRegistry ();
  this->editor = editor;

  // Request the WX 2D canvas along with the 3D renderer and the engine plugins
  if (!csInitializer::RequestPlugins (object_reg,
	CS_REQUEST_IMAGELOADER,
	CS_REQUEST_PLUGIN ("crystalspace.graphics2d.wxgl", iGraphics2D),
	CS_REQUEST_OPENGL3D,
        CS_REQUEST_ENGINE,
        CS_REQUEST_END))
    return ReportError ("Can't initialize standard Crystal Space plugins!");

  // Setup the iWxWindow canvas
  csRef<iGraphics3D> g3d = csQueryRegistry<iGraphics3D> (object_reg);
  if (!g3d)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
              "crystalspace.application.editor",
              "Failed to locate iGraphics3d!");
    return false;
  }
  
  csRef<iWxWindow> wxwin = scfQueryInterface<iWxWindow> (g3d->GetDriver2D ());
  if (!wxwin)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
              "crystalspace.application.editor",
              "The drawing canvas is not a iWxWindow plugin!");
    return false;
  }
  wxwin->SetParent (editor->GetFrame (0)->GetwxFrame ());

  // Setup the default menus
  fileMenu = editor->GetMenuManager ()->GetSubMenu ("&File");
  editMenu = editor->GetMenuManager ()->GetSubMenu ("&Edit");

  return true;
}

void CS3DEngine::Update ()
{
}

void CS3DEngine::Save (iDocumentNode* node) const
{

}

bool CS3DEngine::Load (iDocumentNode* node)
{
  return false;
}

}
CS_PLUGIN_NAMESPACE_END(CSEditor)

