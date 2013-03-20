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

#define CS_IMPLEMENT_PLATFORM_APPLICATION
/* This is needed due the WX headers using free() inline, but the opposing
 * malloc() is in the WX libs. */
#define CS_NO_MALLOC_OVERRIDE

#include "cssysdef.h"

#include "wxgraph.h"

#include "graphedit/graphNode.h"
#include "graphedit/graphNodeFactory.h"
#include "graphedit/graph.h"

#include "graphedit/wxsf/wxShapeFramework.h"
#include "graphedit/wxsf/DiagramManager.h"

#include "myGraph.h"
#include "ids.h"

/* Fun fact: should occur after csutil/event.h, otherwise, gcc may report
 * missing csMouseEventHelper symbols. */
#include <wx/wx.h>

CS_IMPLEMENT_APPLICATION

#if defined(CS_PLATFORM_WIN32)

#ifndef SW_SHOWNORMAL
#define SW_SHOWNORMAL 1
#endif

/*
  WX provides WinMain(), but not main(), which is required for console apps.
 */
int main (int argc, const char* const argv[])
{
  return WinMain (GetModuleHandle (0), 0, GetCommandLineA (), SW_SHOWNORMAL);
}

#endif

//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE (MainFrame, wxFrame)
	EVT_TOOL_RANGE (IDT_FIRST_TOOLMARKER, IDT_LAST_TOOLMARKER, MainFrame::OnTool)
	EVT_UPDATE_UI_RANGE (IDT_FIRST_TOOLMARKER, IDT_LAST_TOOLMARKER, MainFrame::OnUpdateTool)
  EVT_PG_CHANGED ( PAGEID, MainFrame::OnPropertyGridChanging)
  EVT_TREE_ITEM_ACTIVATED (ID_ACTIVATED, MainFrame::OnTreeItemActivated)
 // EVT_COMMAND_TREE_SEL_CHANGED (ID_SEL_CHANGED, MainFrame::OnTreeSelChanged)
END_EVENT_TABLE ()

// The global pointer to frame
MainFrame* frame = 0;

MainFrame::MainFrame (iObjectRegistry* object_reg)
  : wxFrame (0, wxID_ANY, wxT("Crystal Space behaviourtree graph editor test"), 
    wxDefaultPosition, wxSize ( 1024,800 ), wxCLOSE_BOX|wxDEFAULT_FRAME_STYLE|
                                     wxBORDER_STATIC|wxVSCROLL|wxHSCROLL|wxRESIZE_BORDER|wxTAB_TRAVERSAL)
{
  MainFrame::object_reg = object_reg;

  panel = new wxPanel(this, wxID_ANY);

  wxBoxSizer *mainbox = new wxBoxSizer (wxHORIZONTAL);

  wxFlexGridSizer *fgs = new wxFlexGridSizer (1, 2, 0, 0);  
  wxBoxSizer *vbox = new wxBoxSizer (wxVERTICAL);

  m_pTreeCtrl = new wxTreeCtrl (this,wxID_ANY, wxDefaultPosition, wxSize (300, 350), wxTR_DEFAULT_STYLE | 
                                                     wxBORDER_SUNKEN, wxDefaultValidator, wxT("graph items"));
  wxTreeItemId rootItem;
  rootItem = m_pTreeCtrl->AddRoot (wxT("graph items"));
  wxTreeItemId btnode = m_pTreeCtrl->AppendItem (rootItem, wxT("btnode")); 
  wxTreeItemId selectors = m_pTreeCtrl->AppendItem (rootItem, wxT("selectors")); 
  wxTreeItemId triggers = m_pTreeCtrl->AppendItem (rootItem, wxT("triggers")); 
  wxTreeItemId decorators = m_pTreeCtrl->AppendItem (rootItem, wxT("decorators")); 
  wxTreeItemId action = m_pTreeCtrl->AppendItem (rootItem, wxT("action")); 
  wxTreeItemId chk_cond = m_pTreeCtrl->AppendItem (rootItem, wxT("check condition")); 

  wxTreeItemId default_sel = m_pTreeCtrl->AppendItem (selectors, wxT("default")); 
  wxTreeItemId random_sel = m_pTreeCtrl->AppendItem (selectors, wxT("random")); 
  wxTreeItemId seq_sel = m_pTreeCtrl->AppendItem (selectors, wxT("sequential")); 

  wxTreeItemId limit_dec = m_pTreeCtrl->AppendItem (decorators, wxT("execution limit")); 
  wxTreeItemId loop_dec = m_pTreeCtrl->AppendItem (decorators, wxT("loop")); 
  wxTreeItemId neg_dec = m_pTreeCtrl->AppendItem (decorators, wxT("negative return"));

  wxTreeItemId inv_trig = m_pTreeCtrl->AppendItem (triggers, wxT("inventory trigger")); 
  wxTreeItemId mesh_trig = m_pTreeCtrl->AppendItem (triggers, wxT("meshselect trigger"));
  wxTreeItemId msg_trig = m_pTreeCtrl->AppendItem (triggers, wxT("message trigger"));
  wxTreeItemId trig_trig = m_pTreeCtrl->AppendItem (triggers, wxT("trigger trigger"));
  wxTreeItemId seq_trig = m_pTreeCtrl->AppendItem (triggers, wxT("sequence trigger"));
  wxTreeItemId propchg_trig = m_pTreeCtrl->AppendItem (triggers, wxT("property change trigger"));
  wxTreeItemId intersec_trig = m_pTreeCtrl->AppendItem (triggers, wxT("inter sector trigger"));
  wxTreeItemId operation_trig = m_pTreeCtrl->AppendItem (triggers, wxT("operation trigger"));
  wxTreeItemId watch_trig = m_pTreeCtrl->AppendItem (triggers, wxT("watch trigger"));
  wxTreeItemId timout_trig = m_pTreeCtrl->AppendItem (triggers, wxT("timeout trigger"));

 this->Connect (ID_SEL_CHANGED, wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(MainFrame::OnTreeSelChanged));

  vbox->Add (m_pTreeCtrl, 1, wxEXPAND);

  propgrid = new  PropGridManager (this, PAGEID);
  vbox->Add (propgrid->GetManager (), 1, wxEXPAND | wxBOTTOM);

  m_pShapeCanvas = new FrameCanvas (&myGraph, propgrid, this, wxID_ANY);

  fgs->Add (vbox);
  fgs->Add (m_pShapeCanvas, 1, wxEXPAND);

	fgs->AddGrowableCol (1, 1);
	fgs->SetFlexibleDirection( wxHORIZONTAL);
	fgs->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );


  mainbox->Add (fgs, 1, wxALL | wxEXPAND, 10);

  this->SetSizer (mainbox);
  this->Layout ();

  m_pMenuBar = new wxMenuBar( 0 );
	m_pFileMenu = new wxMenu();
	m_pMenuBar->Append( m_pFileMenu, wxT("&File") ); 
	
	m_pEditMenu = new wxMenu();
	m_pMenuBar->Append( m_pEditMenu, wxT("&Edit") ); 
	
	m_pAutoLayoutMenu = new wxMenu();
	m_pMenuBar->Append( m_pAutoLayoutMenu, wxT("&AutoLayout") ); 
	
	m_pHelpMenu = new wxMenu();
	m_pMenuBar->Append( m_pHelpMenu, wxT("&Help") ); 
	
	this->SetMenuBar( m_pMenuBar );
	
	m_pToolBar = this->CreateToolBar( wxTB_HORIZONTAL, wxID_ANY ); 
	m_pToolBar->Realize();

	// add m_pToolBar tools
	m_pToolBar->SetToolBitmapSize(wxSize(16, 15));
	m_pToolBar->AddSeparator();
	m_pToolBar->AddCheckTool(IDT_GRID, wxT("Grid"), wxBitmap(Grid_xpm), wxNullBitmap, wxT("Show/hide grid"));
	m_pToolBar->AddCheckTool(IDT_SHADOW, wxT("Shadows"), wxBitmap(Shadow_xpm), wxNullBitmap, wxT("Show/hide shadows"));

	m_pToolBar->AddRadioTool(IDT_TOOL, wxT("Tool"), wxBitmap(Tool_xpm), wxNullBitmap, wxT("Design tool"));
	m_pToolBar->AddRadioTool(IDT_ACTIONSHP, wxT("BTAction"), wxBitmap(FixedRect_xpm), wxNullBitmap, wxT("BTAction node"));
	m_pToolBar->AddRadioTool(IDT_DECORATORSHP, wxT("Decorators"), wxBitmap(RoundRect_xpm), wxNullBitmap, wxT("Decorators nodes"));
	m_pToolBar->AddRadioTool(IDT_WATCHTRGSHP, wxT("watch Trigger"), wxBitmap(Rect_xpm), wxNullBitmap, wxT("Watch Trigger node"));
	m_pToolBar->AddRadioTool(IDT_ROOTNODESHP, wxT("RootNode"), wxBitmap(Circle_xpm), wxNullBitmap, wxT("root node"));
	m_pToolBar->AddRadioTool(IDT_LOOPDECSHP, wxT("Loop decorator"), wxBitmap(icon4_xpm), wxNullBitmap, wxT("Loop decorator"));
	m_pToolBar->AddRadioTool(IDT_DEFAULTSELSHP, wxT("Default selector"), wxBitmap(Ellipse_xpm), wxNullBitmap, wxT("Default selector"));
  m_pToolBar->AddRadioTool(IDT_CURVESHP, wxT("Link"), wxBitmap(Curve_xpm), wxNullBitmap, wxT("Curve connection"));
	m_pToolBar->AddRadioTool(IDT_TEXTSHP, wxT("Text"), wxBitmap(Text_xpm), wxNullBitmap, wxT("Text"));
	m_pToolBar->AddRadioTool(IDT_EDITTEXTSHP, wxT("Editable text"), wxBitmap(EditText_xpm), wxNullBitmap, wxT("Editable text"));

	
	m_pToolBar->Realize();

	// initialize data members
	m_nToolMode = modeDESIGN;
	m_fShowGrid = false;
	m_fShowShadows = true;

	Centre(); 
}

MainFrame::~MainFrame ()
{
}

void MainFrame::SetupFrame ()
{

  if (!g3d->BeginDraw (CSDRAW_3DGRAPHICS))
    return;

  // Tell the camera to render into the frame buffer.
  view->Draw ();
}

bool MainFrame::HandleEvent (iEvent& ev)
{
 if (CS_IS_KEYBOARD_EVENT(object_reg, ev))
  {
    csPrintf("Got key %" PRIu32 " / %" PRIu32 "\n",
           csKeyEventHelper::GetCookedCode(&ev),
           csKeyEventHelper::GetRawCode(&ev));
    if((ev.Name == csevKeyboardDown (object_reg)) &&
       (csKeyEventHelper::GetCookedCode (&ev) == CSKEY_ESC))
    {
      /* Close the main window, which will trigger an application exit.
         CS-specific cleanup happens in OnClose(). */
      Close();
      return true;
    }
  }

  return false;
}

bool MainFrame::SimpleEventHandler (iEvent& ev)
{
  return frame ? frame->HandleEvent (ev) : false;
}

bool MainFrame::Initialize ()
{
  if (!csInitializer::RequestPlugins (object_reg,
                                      CS_REQUEST_VFS,
                                      CS_REQUEST_PLUGIN( "crystalspace.graphics2d.wxgl", iGraphics2D ),
                                      CS_REQUEST_OPENGL3D,
                                      CS_REQUEST_ENGINE,
                                      CS_REQUEST_FONTSERVER,
                                      CS_REQUEST_IMAGELOADER,
                                      CS_REQUEST_LEVELLOADER,
                                      CS_REQUEST_REPORTER,
                                      CS_REQUEST_REPORTERLISTENER,
                                      CS_REQUEST_END))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
              "crystalspace.application.wxtest",
              "Can't initialize plugins!");
    return false;
  }

  //csEventNameRegistry::Register (object_reg);
  if (!csInitializer::SetupEventHandler (object_reg, SimpleEventHandler))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
              "crystalspace.application.wxtest",
              "Can't initialize event handler!");
    return false;
  }

  // Check for commandline help. void OnPropertyGridChanging (wxPropertyGridEvent& event)
  if (csCommandLineHelper::CheckHelp (object_reg))
  {
    csCommandLineHelper::Help (object_reg);
    return false;
  }

  // The virtual clock.
  vc = csQueryRegistry<iVirtualClock> (object_reg);
  if (vc == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
              "crystalspace.application.wxtest",
              "Can't find the virtual clock!");
    return false;
  }

  // Find the pointer to engine plugin
  engine = csQueryRegistry<iEngine> (object_reg);
  if (engine == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
              "crystalspace.application.wxtest",
              "No iEngine plugin!");
    return false;
  }

  loader = csQueryRegistry<iLoader> (object_reg);
  if (loader == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
              "crystalspace.application.wxtest",
              "No iLoader plugin!");
    return false;
  }

  g3d = csQueryRegistry<iGraphics3D> (object_reg);
  if (g3d == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
              "crystalspace.application.wxtest",
              "No iGraphics3D plugin!");
    return false;
  }

  kbd = csQueryRegistry<iKeyboardDriver> (object_reg);
  if (kbd == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
              "crystalspace.application.wxtest",
              "No iKeyboardDriver plugin!");
    return false;
  }

  CreateFactories ();

  new wxPanel(this, wxID_ANY, wxPoint(0, 0), wxSize(0, 0));
  Show(true);
  iGraphics2D* g2d = g3d->GetDriver2D ();
  csRef<iWxWindow> wxwin = scfQueryInterface<iWxWindow> (g2d);
  if( !wxwin )
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
              "crystalspace.application.wxtest",
              "Canvas is no iWxWindow plugin!");
    return false;
  }
  wxwin->SetParent (panel); 

  // Open the main system. This will open all the previously loaded plug-ins.
  if (!csInitializer::OpenApplication (object_reg))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
              "crystalspace.application.wxtest",
              "Error opening system!");
    return false;
  }

  /* Manually focus the GL canvas.
     This is so it receives keyboard events (and conveniently translate these
     into CS keyboard events/update the CS keyboard state).
   */
  wxwin->GetWindow()->SetFocus ();

  engine->Prepare ();
  view = csPtr<iView> (new csView (engine, g3d));

  return true;
}

void MainFrame::CreateFactories () 
{
  // Create a cel.behaviourtree node factory
  GraphNodeFactory* factory1 = new GraphNodeFactory ();
  factory1->SetName ("cel.behaviourtree");
  factory1->SetId (IDT_ROOT);

  // Add a parameter description
  csOptionDescription description;
  description.name = "Name";
  description.description = "set the name of this node (type: string)";
  description.type = CSVAR_STRING;
  factory1->AddParameter (description);
//------------------------------------------------------------------------------------------------
  // Create a defaul selector node factory
  GraphNodeFactory* factory2 = new GraphNodeFactory ();
  factory2->SetName ("cel.selectors.default");
  factory2->SetId (IDT_DEFAULT);

  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node (type: string)";
  description.type = CSVAR_STRING;
  factory2->AddParameter (description);
//--------------------------------------------------------------------------------------------------
  // Create a parameter check condition node factory
  GraphNodeFactory* factory3 = new GraphNodeFactory ();
  factory3->SetName ("cel.behaviourtree.parametercheck");
  factory3->SetId (IDT_PARAM_CHCK);


  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node(type: string)";
  description.type = CSVAR_STRING;
  factory3->AddParameter (description);
  
  // Add a parameter description 
	description.name = "Parameter";
	description.description = "Set the parameter to be checked by this node (type: String)";
	description.type = CSVAR_STRING;
	factory3->AddParameter (description);

	// Add a parameter description 
	description.name = "Value";
	description.description = "Set the value that will be checked against the parameter (type: String)";
	description.type = CSVAR_STRING;
	factory3->AddParameter (description);
  //-----------------------------------------------------------------------------------------
  // Create an execution limit node factory
  GraphNodeFactory* factory4 = new GraphNodeFactory ();
  factory4->SetName ("cel.decorators.executionlimit");
  factory4->SetId (IDT_EXE_LIMIT);

  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node(type: string)";
  description.type = CSVAR_STRING;
  factory4->AddParameter (description);

  // Add a parameter description
  description.name = "execution limit";
  description.description = "Set the number of times to execute child(type: long)";
  description.type = CSVAR_LONG;
  factory4->AddParameter (description);
//---------------------------------------------------------------------
  // Create a loop decorator node factory
  GraphNodeFactory* factory5 = new GraphNodeFactory ();
  factory5->SetName ("cel.decorators.loop");
  factory5->SetId (IDT_LOOP);

  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node(type: string)";
  description.type = CSVAR_STRING;
  factory5->AddParameter (description);

  // Add a parameter description
  description.name = "loop limit";
  description.description = "set the number of loops (type: long)";
  description.type = CSVAR_LONG;
  factory5->AddParameter (description);
//-----------------------------------------------------------
  // Create a negate return decorator node factory
  GraphNodeFactory* factory6 = new GraphNodeFactory ();
  factory6->SetName ("cel.decorators.negatereturn");
  factory6->SetId (IDT_NEGATERETURN);

  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node (type: string)";
  description.type = CSVAR_STRING;
  factory6->AddParameter (description);
//-------------------------------------------------------------------------
  // Create a random selector node factory
  GraphNodeFactory* factory7 = new GraphNodeFactory ();
  factory7->SetName ("cel.selectors.random");
  factory7->SetId (IDT_RANDOM);

  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node (type: string)";
  description.type = CSVAR_STRING;
  factory7->AddParameter (description);
 //---------------------------------------------------------------------

  // Create a sequential selector node factory
  GraphNodeFactory* factory8 = new GraphNodeFactory ();
  factory8->SetName ("cel.selectors.sequential");
  factory8->SetId (IDT_SEQUENTIAL);
  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node (type: string)";
  description.type = CSVAR_STRING;
  factory8->AddParameter (description);
 //-------------------------------------------------------------------------
  // Create a inventory trigger  node factory
  GraphNodeFactory* factory9 = new GraphNodeFactory ();
  factory9->SetName ("cel.triggers.inventory");
  factory9->SetId (IDT_INVENTORY);
  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node (type: string)";
  description.type = CSVAR_STRING;
  factory9->AddParameter (description);

  // Add a parameter description
  description.name = "entity";
  description.description = "Set the name of the entity that contains the pcinventory property on which this tigger will fire(type: string)";
  description.type = CSVAR_STRING;
  factory9->AddParameter (description);

  // Add a parameter description
  description.name = "child";  
  description.description = "Set the name of the entity to watch for (type: string)";
  description.type = CSVAR_STRING;
  factory9->AddParameter (description);
//------------------------------------------------------------------------------------------
  // Create a message trigger  node factory
  GraphNodeFactory* factory10 = new GraphNodeFactory ();
  factory10->SetName ("cel.triggers.message");
  factory10->SetId (IDT_MESSAGE);

  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node (type: string)";
  description.type = CSVAR_STRING;
  factory10->AddParameter (description);

  // Add a parameter description
  description.name = "Entity";
  description.description = "Set the name of the entity from which we want to listen to messages(type: string)";
  description.type = CSVAR_STRING;
  factory10->AddParameter (description);

  // Add a parameter description
  description.name = "message mask ";
  description.description = "Set the message mask that we want to receive message for (type: string)";
  description.type = CSVAR_STRING;
  factory10->AddParameter (description);
//-----------------------------------------------------------------------------------------------------
  // Create a mesh selected trigger  node factory
  GraphNodeFactory* factory11 = new GraphNodeFactory ();
  factory11->SetName ("cel.triggers.meshselect");
  factory11->SetId (IDT_MESH_SELECT);

  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node (type: string)";
  description.type = CSVAR_STRING;
  factory11->AddParameter (description);

  // Add a parameter description
  description.name = "Entity";
  description.description = "Set the name of the entity containing the pcmeshselect property class on which this trigger will fire(type: string)";
  description.type = CSVAR_STRING;
  factory11->AddParameter (description);
//-----------------------------------------------------------------------------------------------------
  // Create a operation trigger  node factory
  GraphNodeFactory* factory12 = new GraphNodeFactory ();
  factory12->SetName ("cel.triggers.operation");
  factory12->SetId (IDT_OPERATION);

  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node (type: string)";
  description.type = CSVAR_STRING;
  factory12->AddParameter (description);

  // Add a parameter description
  description.name = "operation";
  description.description = "Set the operation this trigger will use to combine child triggers. the logical operation to perform can be one of 'or', 'and' or 'xor'(type: string)";
  description.type = CSVAR_STRING;
  factory12->AddParameter (description);
//-----------------------------------------------------------------------------------------------------
  // Create a property change trigger node factory
  GraphNodeFactory* factory13 = new GraphNodeFactory ();
  factory13->SetName ("cel.triggers.propertychange");
  factory12->SetId (IDT_PROP_CHANGE);

  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node (type: string)";
  description.type = CSVAR_STRING;
  factory13->AddParameter (description);

  // Add a parameter description
  description.name = "entity";
  description.description = "Set the name of the entity containing the pcproperties property class on which this trigger will fire (type: string)";
  description.type = CSVAR_STRING;
  factory13->AddParameter (description);

  // Add a parameter description
  description.name = "On change only";
  description.description = "Set whether the trigger will fire only on actual change, or whenever the condition is met(even initially) (type: bool)";
  description.type = CSVAR_BOOL;
  factory13->AddParameter (description);

  // Add a parameter description
  description.name = "operation";
  description.description = "Set the operation this trigger will test with. Valid operations are: 'eq' (equals), 'gt' (greater than), 'lt' (lower than), 'ne' (not equal), 'le' (lower and equal) 'ge' (greater or equal')(type: string)";
  description.type = CSVAR_STRING;

  factory13->AddParameter (description);
  // Add a parameter description
  description.name = "property";
  description.description = "Set the name of the property on which this trigger will fire (type: string)";
  description.type = CSVAR_STRING;
  factory13->AddParameter (description);

  factory13->AddParameter (description);
  // Add a parameter description
  description.name = "value";
  description.description = "Set the value of the property on which this trigger will fire (type: string)";
  description.type = CSVAR_STRING;
  factory13->AddParameter (description);
//------------------------------------------------------------------------------------------------------------------------------------------------------- 
  // Create a sequence finished trigger node factory
  GraphNodeFactory* factory14 = new GraphNodeFactory ();
  factory14->SetName ("cel.triggers.sequencefinish");
  factory14->SetId (IDT_SEQ_FINISH);

  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node (type: string)";
  description.type = CSVAR_STRING;
  factory14->AddParameter (description);

  // Add a parameter description
  description.name = "Entity";
  description.description = "Set the name of the entity containing the pcquest property class on which this trigger will fire(type: string)";
  description.type = CSVAR_STRING;
  factory14->AddParameter (description);

  // Add a parameter description
  description.name = "sequence name";
  description.description = "Set the name of the sequence(type: string)";
  description.type = CSVAR_STRING;
  factory14->AddParameter (description);
//---------------------------------------------------------------------------------------------------------------------------------------------------------
  // Create a timeout trigger node factory
  GraphNodeFactory* factory15 = new GraphNodeFactory ();
  factory15->SetName ("cel.triggers.timeout");
  factory15->SetId (IDT_TIMEOUT);

  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node (type: string)";
  description.type = CSVAR_STRING;
  factory15->AddParameter (description);

  // Add a parameter description
  description.name = "Name";
  description.description = "Set the timeout on which on which this trigger will fire (type: float)";
  description.type = CSVAR_FLOAT;
  factory15->AddParameter (description);
//------------------------------------------------------------------------------------------------------------------------------
  // Create a trigger trigger node factory
  GraphNodeFactory* factory16 = new GraphNodeFactory ();
  factory16->SetName ("cel.triggers.trigger");
  factory16->SetId (IDT_TRIGGER);

  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node (type: string)";
  description.type = CSVAR_STRING;
  factory16->AddParameter (description);

  // Add a parameter description
  description.name = "entity";
  description.description = "Set the name of the entity containing the pccamera or pcmesh on which this trigger will fire (type: string)";
  description.type = CSVAR_STRING;
  factory16->AddParameter (description);
//------------------------------------------------------------------------------------------------------------------------------------------------------
  // Create a watch trigger node factory
  GraphNodeFactory* factory17 = new GraphNodeFactory ();
  factory17->SetName ("cel.triggers.watch");
  factory17->SetId (IDT_WATCH);

  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node (type: string)";
  description.type = CSVAR_STRING;
  factory17->AddParameter (description);

  // Add a parameter description
  description.name = "entity";
  description.description = "Set the name of the source entity containing the  pcmesh property class on which this trigger will fire (type: string)";
  description.type = CSVAR_STRING;
  factory17->AddParameter (description);

  // Add a parameter description
  description.name = "target entity";
  description.description = "Set the name of the target entity containing the  pcmesh property class on which this trigger will fire (type: string)";
  description.type = CSVAR_STRING;
  factory17->AddParameter (description);

  // Add a parameter description
  description.name = "checktime";
  description.description = "Set the interval (in milliseconds) after which this trigger will check visibility again (type: float)";
  description.type = CSVAR_FLOAT;
  factory17->AddParameter (description);

  // Add a parameter description
  description.name = "radius";
  description.description = "Set the maximum radius for visibility checking (type: float)";
  description.type = CSVAR_FLOAT;
  factory17->AddParameter (description);

  // Add a parameter description
  description.name = "offset";
  description.description = "optional offset parameter (type: csVector3)";
  description.type = CSVAR_VECTOR3;
  factory17->AddParameter (description);
//------------------------------------------------------------------------------------------------------------------------------------------------------
   // Create a enter sector trigger node factory
  GraphNodeFactory* factory18 = new GraphNodeFactory ();
  factory18->SetName ("cel.triggers.entersector");
  factory18->SetId (IDT_ENTER_SECTOR);

  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node (type: string)";
  description.type = CSVAR_STRING;
  factory18->AddParameter (description);

  // Add a parameter description
  description.name = "sector";
  description.description = "Set the name of the sector on which this trigger will fire (type: string)";
  description.type = CSVAR_STRING;
  factory18->AddParameter (description);
//--------------------------------------------------------------------------------------------------------------------------------------------------------
 //Create a graph editor   
  // Create a btaction node factory
  GraphNodeFactory* factory19 = new GraphNodeFactory ();
  factory19->SetName ("cel.behaviourtree.action");
  factory19->SetId (IDT_ACTION);

  // Add a parameter description
  description.name = "Name";
  description.description = "Set the name of this node (type: string)";
  description.type = CSVAR_STRING;
  factory19->AddParameter (description);
//--------------------------------------------------------------------------------------------------------------------------------------------------------
 //Create a graph editor
  graphEdit = new GraphEditor();
  
  //Add node factories to this graph editor
  graphEdit->AddNodeFactory (factory1);
  graphEdit->AddNodeFactory (factory2);
  graphEdit->AddNodeFactory (factory3);
  graphEdit->AddNodeFactory (factory4);  
  graphEdit->AddNodeFactory (factory5);
  graphEdit->AddNodeFactory (factory6);
  graphEdit->AddNodeFactory (factory7);
  graphEdit->AddNodeFactory (factory8);
  graphEdit->AddNodeFactory (factory9);
  graphEdit->AddNodeFactory (factory10);
  graphEdit->AddNodeFactory (factory11);
  graphEdit->AddNodeFactory (factory12);
  graphEdit->AddNodeFactory (factory13);
  graphEdit->AddNodeFactory (factory14);
  graphEdit->AddNodeFactory (factory15);
  graphEdit->AddNodeFactory (factory16);
  graphEdit->AddNodeFactory (factory17);
  graphEdit->AddNodeFactory (factory18);
  graphEdit->AddNodeFactory (factory19);
 
 // set the graph
  graphEdit->SetGraph (&myGraph);

}

void MainFrame::PushFrame ()
{
  csRef<iEventQueue> q (csQueryRegistry<iEventQueue> (object_reg));
  if (!q)
    return ;
  csRef<iVirtualClock> vc (csQueryRegistry<iVirtualClock> (object_reg));

  if (vc)
    vc->Advance();
  q->Process();
}

void MainFrame::OnClose(wxCloseEvent& event)
{
  csPrintf("got close event\n");
  
  // Tell CS we're going down
  csRef<iEventQueue> q (csQueryRegistry<iEventQueue> (object_reg));
  if (q) q->GetEventOutlet()->Broadcast (csevQuit(object_reg));
  
  // WX will destroy the 'MainFrame' instance
  frame = 0;
}

void MainFrame::OnTreeItemActivated (wxTreeEvent& event)
{
  printf("hello\n");
}

void MainFrame::OnTreeSelChanged (wxTreeEvent& event)
{
  printf ("hello\n");
  wxTreeItemId item = event.GetItem ();
  m_pTreeCtrl->GetItemText (item);
//research function
}

void MainFrame::OnTool(wxCommandEvent& event)
{
    if(m_pShapeCanvas->GetMode() == FrameCanvas::modeCREATECONNECTION)m_pShapeCanvas->AbortInteractiveConnection();

    switch(event.GetId())
    {
      case IDT_GRID:
        	m_fShowGrid = !m_fShowGrid;
			    if( m_fShowGrid )
			    {
				    m_pShapeCanvas->AddStyle(wxSFShapeCanvas::sfsGRID_SHOW);
				    m_pShapeCanvas->AddStyle(wxSFShapeCanvas::sfsGRID_USE);
			    }
			    else
			    {
				    m_pShapeCanvas->RemoveStyle(wxSFShapeCanvas::sfsGRID_SHOW);
				    m_pShapeCanvas->RemoveStyle(wxSFShapeCanvas::sfsGRID_USE);
			    }

           m_pShapeCanvas->Refresh(false);
           break;

        case IDT_SHADOW:
        	m_fShowShadows = !m_fShowShadows;

        	m_pShapeCanvas->ShowShadows(m_fShowShadows, wxSFShapeCanvas::shadowALL);
        	// also shadows for topmost shapes only are allowed:
        	//m_pShapeCanvas->ShowShadows(m_fShowShadows, wxSFShapeCanvas::shadowTOPMOST);

          m_pShapeCanvas->Refresh(false);
          break;

        case IDT_WATCHTRGSHP:
              m_nToolMode = modeWATCHTRG;
           	break;


        case IDT_DEFAULTSELSHP:
              m_nToolMode = modeDEFAULTSEL;
           	break;

        case IDT_LOOPDECSHP:
              m_nToolMode = modeLOOPDEC;
           	break;

        case IDT_CURVESHP:
            m_nToolMode = modeLINK;
            break;

        case IDT_EDITTEXTSHP:
            m_nToolMode = modeEDITTEXT;
            break;

        case IDT_ROOTNODESHP:
            m_nToolMode = modeTREE;
            break;

        case IDT_ACTIONSHP:
            m_nToolMode = modeACTION;
            break;
        case IDT_TEXTSHP:
            m_nToolMode = modeTEXT;
            break;

        case IDT_TOOL:
            m_nToolMode = modeDESIGN;
            break;
      
        default:
            event.Skip();
            break;
    }
}

void MainFrame::OnUpdateTool(wxUpdateUIEvent& event)
{
    switch(event.GetId())
    {
        case IDT_GRID:
        	event.Check(m_fShowGrid);
            break;

        case IDT_WATCHTRGSHP:
              event.Check(m_nToolMode == modeWATCHTRG);
           	break;

        case IDT_DEFAULTSELSHP:
              event.Check(m_nToolMode == modeDEFAULTSEL);
           	break;

        case IDT_LOOPDECSHP:
              event.Check(m_nToolMode == modeLOOPDEC);
           	break;

        case IDT_CURVESHP:
            event.Check(m_nToolMode == modeLINK);
            break;

        case IDT_EDITTEXTSHP:
            event.Check(m_nToolMode == modeEDITTEXT);
            break;

        case IDT_ROOTNODESHP:
            event.Check(m_nToolMode == modeTREE);
            break;

        case IDT_TEXTSHP:
            event.Check(m_nToolMode == modeTEXT);
            break;

        case IDT_TOOL:
            event.Check(m_nToolMode == modeDESIGN);
            break;

        default:
            event.Skip();
            break;
    }
}

 void MainFrame::OnPropertyGridChanging (wxPropertyGridEvent& event)
{
 propgrid->OnPropertyGridChanging (event); 
}


#ifdef USE_IDLE
BEGIN_EVENT_TABLE(MyApp, wxApp)
  EVT_IDLE(MyApp::OnIdle)
END_EVENT_TABLE()
#endif


IMPLEMENT_APP(MyApp)

/*---------------------------------------------------------------------*
 * Main function
 *---------------------------------------------------------------------*/
  bool MyApp::OnInit(void)
{
#if defined(wxUSE_UNICODE) && wxUSE_UNICODE
  char** csargv;
  csargv = (char**)cs_malloc(sizeof(char*) * argc);
  for(int i = 0; i < argc; i++) 
  {
    csargv[i] = strdup (wxString(argv[i]).mb_str().data());
  }
  object_reg = csInitializer::CreateEnvironment (argc, csargv);
#else
  object_reg = csInitializer::CreateEnvironment (argc, argv);
#endif

  frame = new MainFrame (object_reg);
  SetTopWindow (frame);
  frame->Initialize ();

#ifdef USE_TIMER
  /* This triggers a timer event every 20 milliseconds, which will yield
   a maximum framerate of 1000/20 = 50 FPS.  Obviously if it takes longer
   than 20 ms to render a frame the framerate will be lower :-)
   You may wish to tweak this for your own application.  Note that
   this also lets you throttle the CPU usage of your app, because
   the application will yield the CPU and wait for events in the
   time between when it completes rendering the current frame and
   the timer triggers the next frame.
  */

  Pump* p = new Pump();
  p->f = frame;
  p->Start(20);
#endif

  return true;
}

#ifdef USE_IDLE
void MyApp::OnIdle() {
  frame->PushFrame();
}
#endif

int MyApp::OnExit()
{
  csInitializer::DestroyApplication (object_reg);
  return 0;
}
