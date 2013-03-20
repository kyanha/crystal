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

#ifndef __CSEDITOR_COMPONENT_2DINFO_H__
#define __CSEDITOR_COMPONENT_2DINFO_H__

#include "csutil/csbaseeventh.h"
#include "csutil/eventhandlers.h"
#include "csutil/scf_implementation.h"
#include "iutil/eventq.h"
#include "ivaria/reporter.h"
#include "ivideo/fontserv.h"

#include "ieditor/editor.h"
#include "ieditor/space.h"

using namespace CS::EditorApp;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{
class CS2DInfo
  : public scfImplementation1<CS2DInfo, iEditorComponent>,
    public csBaseEventHandler
{
public:
  CS2DInfo (iBase* parent);
  virtual ~CS2DInfo ();
  
  //-- iEditorComponent
  virtual bool Initialize (iEditor* editor);
  virtual void Update ();
  virtual void Save (iDocumentNode* node) const;
  virtual bool Load (iDocumentNode* node);

  //-- iEventHandler
  bool HandleEvent (iEvent& event);

private:
  void UpdateFrame ();
  void WriteShadow (int x, int y, int fg, const char *str, ...) const;
  void Write (int x, int y, int fg, int bg, const char *str, ...) const;

  bool ReportError (const char* description, ...) const
  {
    va_list arg;
    va_start (arg, description);
    csReportV (object_reg, CS_REPORTER_SEVERITY_ERROR,
	       "crystalspace.editor.component.2dinfo",
	       description, arg);
    va_end (arg);
    return false;
  }

private:
  iObjectRegistry* object_reg;
  iEditor* editor;
  csRef<iGraphics3D> g3d;
  csRef<iFont> font;

  class FrameListener : public scfImplementation1<FrameListener, iEventHandler>
  {
  public:
    FrameListener (CS2DInfo* info);

    //-- iEventHandler
    bool HandleEvent (iEvent &event);

  private:
    CS2DInfo* info;

    // Declare this event handler as listening to the '2D' frame phase
    CS_EVENTHANDLER_PHASE_2D ("crystalspace.editor.component.2dinfo");
  };
  csRef<FrameListener> frameListener;
};

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif // __CSEDITOR_COMPONENT_2DINFO_H__
