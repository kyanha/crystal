/*
  Copyright (C) 2011 Christian Van Brussel, Eutyche Mukuama, Dodzi de Souza
      Institute of Information
      and Communication Technologies, Electronics and Applied Mathematics
      at Universite catholique de Louvain, Belgium
      http://www.uclouvain.be/en-icteam.html

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __MAINFRAME_H__
#define __MAINFRAME_H__

#define PAGEID 1000

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "csutil/ref.h"
#include <string.h>

#include "graphedit/pluginconfig.h"

#include "iutil/eventq.h"
#include "iutil/event.h"
#include "iutil/objreg.h"
#include "iutil/csinput.h"
#include "iutil/virtclk.h"
#include "iengine/sector.h"
#include "iengine/engine.h"
#include "iengine/camera.h"
#include "iengine/light.h"
#include "iengine/texture.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "iengine/material.h"
#include "imesh/object.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "ivideo/texture.h"
#include "ivideo/material.h"
#include "ivideo/fontserv.h"
#include "ivideo/natwin.h"
#include "ivideo/wxwin.h"
#include "igraphic/imageio.h"
#include "imap/loader.h"
#include "ivaria/reporter.h"
#include "ivaria/stdrep.h"
#include "csutil/cmdhelp.h"
#include "csutil/event.h"
#include "csutil/sysfunc.h"
#include "csutil/event.h"
#include "csutil/common_handlers.h"
#include "csutil/cfgfile.h"
#include "csutil/cfgmgr.h"
#include "iutil/vfs.h"
#include "csutil/cscolor.h"
#include "cstool/csview.h"
#include "cstool/initapp.h"
#include "cstool/genmeshbuilder.h"
#include "cstool/simplestaticlighter.h"

#include "csutil/custom_new_disable.h"
#include <wx/wx.h>
#include <wx/treectrl.h>

#include "csutil/custom_new_enable.h"

// Main propertygrid header.
#include "wx/propgrid/propgrid.h"

// Needed for implementing custom properties.
#include "wx/propgrid/propdev.h"
// Extra property classes.
#include "wx/propgrid/advprops.h"

// This defines wxPropertyGridManager.
#include "wx/propgrid/manager.h"

#include "propgrid.h"
#include "graphedit/graphEditor.h"
#include "myGraph.h"

#include "frameCanvas.h"
#include "art.h"
#include "ids.h"



struct iEngine;
struct iLoader;
struct iGraphics3D;
struct iKeyboardDriver;
struct iVirtualClock;
struct iObjectRegistry;
struct iEvent;
struct iSector;
struct iView;

class FrameCanvas;
class MainFrame : public wxFrame 
{

public:

    enum MODE
	{
		modeDESIGN,
        modeTREE,
        modeDEFAULTSEL,
        modeSEQUENCESEL,
        modeRANDOMSEL,
        modeLOOPDEC,
        modeNEGRETURNDEC,
        modeEXECLIMITDEC,
        modeCHKCONDITION,
        modeMESSAGETRG,
        modeTRIGGERTRG,
        modeSEQUENCETRG,
        modePROPCHANGETRG,
        modeINVENTORYTRG,
        modeMESHSELECTTRG,
        modeENTERSECTORTRG,
        modeTIMEOUTTRG,
        modeOPERATIONTRG,
        modeWATCHTRG,
		modeACTION,
		modeTEXT,
		modeEDITTEXT,
		//modeBITMAP,
		modeLINK,
		modeGRID,

	};

	// public data members
	bool m_fShowShadows;

  MainFrame (iObjectRegistry* object_reg);
  ~MainFrame ();

	// public data accessors
  MODE GetToolMode (){return m_nToolMode;}
  void SetToolMode (MODE m){m_nToolMode = m;}
  GraphEditor* GetGraphEditor () {return graphEdit;}

  bool Initialize ();
  void PushFrame ();
  void OnClose(wxCloseEvent& event);
  void OnIconize(wxIconizeEvent& event);
  void OnShow(wxShowEvent& event);
  void OnMouseDown ();
  void OnTreeItemActivated (wxTreeEvent& event);
  void OnTreeSelChanged (wxTreeEvent& event);
  void OnTreeKeyDown (wxTreeEvent& event);
  void OnTreeItemCollapsed (wxTreeEvent& event);
  void OnTreeItemExpanded (wxTreeEvent& event);
  void OnPropertyGridChanging (wxPropertyGridEvent& event);

  DECLARE_EVENT_TABLE()

private:
  iObjectRegistry* object_reg;
  csRef<iEngine> engine;
  csRef<iLoader> loader;
  csRef<iGraphics3D> g3d;
  csRef<iKeyboardDriver> kbd;
  csRef<iVirtualClock> vc;
  csRef<iView> view;
  iSector* room;
  //the property grid
  PropGridManager* propgrid;
  //the canvas where graphs are drawn
  FrameCanvas* m_pShapeCanvas;
 
  wxPanel *panel;

  GraphEditor* graphEdit;
  MyGraph myGraph;


  static bool SimpleEventHandler (iEvent& ev);
  bool HandleEvent (iEvent& ev);
  void SetupFrame ();
  void CreateFactories ();


/*  CS_DECLARE_EVENT_SHORTCUTS;
  csEventID MouseDown;
  csEventID MouseUp;
  csEventID MouseMove;
  csEventID KeyboardDown;*/

  wxTreeCtrl* m_pTreeCtrl;
  wxMenuBar* m_pMenuBar;
  wxMenu* m_pFileMenu;
  wxMenu* m_pEditMenu;
  wxMenu* m_pAutoLayoutMenu;
  wxMenu* m_pHelpMenu;
  wxToolBar* m_pToolBar;
  wxStatusBar* m_pStatusBar;
  wxSlider* m_pZoomSlider;

  bool m_fShowGrid;
  MODE m_nToolMode;

  // protected function
  void CleanUp();
  void OnTool(wxCommandEvent& event);
  void OnUpdateTool(wxUpdateUIEvent& event);
                                                                                                                                                                    
};

/* There are two ways to drive the CS event loop, from
  a Wx timer or from idle events.  This test app demonstrates
  either method depending on which #define is set below.  Using
  a timer seems to produce better results (a smoother framerate
  and better CPU utilization).
*/

//#define USE_IDLE
#define USE_TIMER

#ifdef USE_TIMER
class Pump : public wxTimer
{
public:
  MainFrame* f;
  Pump() { };
  virtual void Notify()
    {
    f->PushFrame();
    }
};
#endif


// Define a new application type
class MyApp: public wxApp
{
public:
  iObjectRegistry* object_reg;

  virtual bool OnInit(void);
  virtual int OnExit(void);

#ifdef USE_IDLE
  virtual void OnIdle();
  DECLARE_EVENT_TABLE();
#endif
};

// wxwindows app
DECLARE_APP( MyApp )

#endif // __MAINFRAME_H__
