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

#ifndef __SPACE_3DVIEW_H__
#define __SPACE_3DVIEW_H__

#include "csutil/csbaseeventh.h"
#include "csutil/eventnames.h"
#include "csutil/scf_implementation.h"
#include "ieditor/editor.h"
#include "ieditor/space.h"
#include "iutil/event.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"

class wxWindow;

using namespace CS::EditorApp;

CS_PLUGIN_NAMESPACE_BEGIN(CSEditor)
{

class CS3DSpace
  : public wxEvtHandler,
    public scfImplementation1<CS3DSpace, iSpace>
{
public:
  CS3DSpace (iBase* parent);
  virtual ~CS3DSpace();

  //-- iSpace
  virtual bool Initialize (iObjectRegistry* obj_reg, iEditor* editor,
			   iSpaceFactory* factory, wxWindow* parent);
  virtual iSpaceFactory* GetFactory () const { return factory; }
  virtual wxWindow* GetwxWindow ();
  virtual void SetEnabled (bool enabled);
  virtual bool GetEnabled () const;
  virtual void Update ();

private:
  virtual void OnFrameBegin ();
  virtual void OnFramePrint ();
  
  void OnSize (wxSizeEvent& event);
  void OnEnterWindow (wxMouseEvent& event);

private:
  iObjectRegistry* object_reg;
  iEditor* editor;
  csRef<iSpaceFactory> factory;

  wxWindow* window;
  bool enabled;
  size_t updateCount;
  
  csRef<iGraphics3D> g3d;
  csRef<iView> view;

  class FrameBegin3DDraw : public scfImplementation1<FrameBegin3DDraw, iEventHandler>
  {
  public:
    FrameBegin3DDraw (CS3DSpace* space);
    ~FrameBegin3DDraw ();
    bool HandleEvent (iEvent &event);

  private:
    CS3DSpace* space;
    CS_EVENTHANDLER_PHASE_3D ("crystalspace.editor.space.3dview.frame_begin");
  };
  FrameBegin3DDraw* frameBegin3DDraw;

  class FramePrinter : public scfImplementation1<FramePrinter, iEventHandler>
  {
  public:
    FramePrinter (CS3DSpace* space);
    ~FramePrinter ();
    bool HandleEvent (iEvent &event);

  private:
    CS3DSpace* space;
    CS_EVENTHANDLER_PHASE_FRAME ("crystalspace.editor.space.3dview.frame_print");
  };
  FramePrinter* framePrinter;

  class Space : public wxPanel
  {
    public:
      Space (CS3DSpace* p, wxWindow* parent, wxWindowID id = wxID_ANY,
	    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize)
      : wxPanel (parent, id, pos, size), space (p)
      {}
    
      //-- wxPanel
      virtual void OnSize (wxSizeEvent& ev)
      { if (space) space->OnSize (ev); }

    private:
      CS3DSpace* space;

      DECLARE_EVENT_TABLE ();
  };
};

}
CS_PLUGIN_NAMESPACE_END(CSEditor)

#endif // __SPACE_3DVIEW_H__
