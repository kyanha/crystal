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
#include "cssysdef.h"

#include <wx/wxprec.h>
#include <wx/app.h>

#include <wx/textdlg.h>
#include <wx/filename.h>

#include "btnode.h"
#include "btaction.h"
#include "selectors.h"
#include "decorators.h"
#include "triggers.h"
#include "ids.h"

#include "frameCanvas.h"


FrameCanvas::FrameCanvas (MyGraph* graph,PropGridManager* propgrid, wxWindow* parent, wxWindowID id)
                             : wxSFShapeCanvas(graph, parent, id, wxDefaultPosition, wxSize (600,700), wxHSCROLL | wxVSCROLL | wxSTATIC_BORDER)
{
    // get pointer to main application propgrid
	m_pParentFrame = (MainFrame*)wxGetApp().GetTopWindow();
	//set pointer to the graph manager
	this->graph = graph;
        //Set pointer to the the propgid propgrid
        this->propgrid = propgrid;
	// initialize grid
	AddStyle(sfsGRID_USE);
	AddStyle(sfsGRID_SHOW);
	// distances between grid lines can be modified via following function:
	SetGridLineMult(10);
	// grid line style can be set as follows:
	SetGridStyle(wxSHORT_DASH);
	
	// the canvas background can be filled with a solid colour ...
	//RemoveStyle(sfsGRADIENT_BACKGROUND);
	//SetBackgroundColour(sfdvSHAPECANVAS_BACKGROUNDCOLOR);
	// ... or by a gradient fill
	AddStyle(sfsGRADIENT_BACKGROUND);
	SetGradientFrom(sfdvSHAPECANVAS_GRADIENT_FROM);
	SetGradientTo(sfdvSHAPECANVAS_GRADIENT_TO);

	// set accepted shapes
	graph->ClearAcceptedShapes();
	graph->AcceptShape(wxT("All"));

	// you can set also the canvas history manager working mode:
	// 1) Requires implementation of xsSerializable::Clone() virtual function
	// and copy constructor in all classes derived from the xsSerializable class
	// and shape manager.
	// GetHistoryManager().SetMode(wxSFCanvasHistory::histUSE_CLONING);
	// 2) No other programming overhead is required (except implementation
	// of standard serialization functionality). This working mode is default.
	GetHistoryManager().SetMode(wxSFCanvasHistory::histUSE_SERIALIZATION);

	// change of the canvas history manager working mode clears the stored states
	// so we should save initial canvas state manually
	SaveCanvasState();
}

FrameCanvas::~FrameCanvas(void)
{
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

void FrameCanvas::OnLeftDown(wxMouseEvent& event)
{
    GraphNode* node = NULL;
    wxSFShapeBase* pShape = NULL;
	
   switch(m_pParentFrame->GetToolMode())
   {
	  case MainFrame::modeTEXT:
	
	  case MainFrame::modeEDITTEXT:
		{  
		    wxTextEntryDialog dlg(this, wxT(""), wxT("Enter text"), wxT("enter text"));

		    if(dlg.ShowModal() == wxID_OK)
		    {
                             	if(m_pParentFrame->GetToolMode() == MainFrame::modeTEXT)
					pShape =  graph->AddShape(CLASSINFO(wxSFTextShape), 
                                                                            event.GetPosition(), sfDONT_SAVE_STATE);
				else
				{
					pShape =  graph->AddShape(CLASSINFO(wxSFEditTextShape),
                                                                                event.GetPosition(), sfDONT_SAVE_STATE);
					// editable text shapes can be forced to be multiline at default like this:
					// ((wxSFEditTextShape*)pShape)->ForceMultiline( true );
					
					// also edit control type can be set here like this:
					//((wxSFEditTextShape*)pShape)->SetEditType( wxSFEditTextShape::editDIALOG );
				}

                if(pShape)
                {
		    ((wxSFTextShape*)pShape)->SetText(dlg.GetValue());

                    // set alignment
                    pShape->SetVAlign(wxSFShapeBase::valignTOP);
                    pShape->SetHAlign(wxSFShapeBase::halignCENTER);
                    pShape->SetVBorder(10);

                    // set shapes policy
                    pShape->AcceptConnection(wxT("All"));
                    pShape->AcceptSrcNeighbour(wxT("BTAction"));
                    //pShape->AcceptTrgNeighbour(wxT("wxSFTextShape"));
                    //pShape->AcceptTrgNeighbour(wxT("wxSFEditTextShape"));
                }
		    }
		 
          break;
          }

	 //case MainFrame::modeGRID:
     case MainFrame::modeTREE:
		{        
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (0));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptTrgNeighbour(wxT("All"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }
	break;}
     case MainFrame::MainFrame::modeDEFAULTSEL:
		{        
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (1));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptSrcNeighbour(wxT("All"));
			  node->AcceptTrgNeighbour(wxT("All"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }
		
    break;}

  case MainFrame::modeSEQUENCESEL:
		{        
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (7));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptSrcNeighbour(wxT("All"));
			  node->AcceptTrgNeighbour(wxT("All"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }
		
	break;}
  case MainFrame::modeRANDOMSEL:
		{        
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (6));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptSrcNeighbour(wxT("All"));
			  node->AcceptTrgNeighbour(wxT("All"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }
		
	break;}

	case MainFrame::modeLOOPDEC:
		{        
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (4));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptSrcNeighbour(wxT("All"));
			  node->AcceptTrgNeighbour(wxT("All"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }
		
	break;}
      
	case	MainFrame::modeNEGRETURNDEC:
		{        
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (5));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptSrcNeighbour(wxT("All"));
			  node->AcceptTrgNeighbour(wxT("All"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }
		
	break;}
	case	MainFrame::modeEXECLIMITDEC: 
		{        
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (3));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptSrcNeighbour(wxT("All"));
			  node->AcceptTrgNeighbour(wxT("All"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }
		
	break;}
  case  MainFrame::modeCHKCONDITION: break;
	case	MainFrame::modeMESSAGETRG:
		{        
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (9));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptSrcNeighbour(wxT("All"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }
		
	break;}
  case  MainFrame::modeTRIGGERTRG:
		{        
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (15));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptSrcNeighbour(wxT("All"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }
		
	break;}
  case  MainFrame::modeSEQUENCETRG:
		{        
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (13));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptSrcNeighbour(wxT("All"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }
		
	break;}
  case  MainFrame::modePROPCHANGETRG:
		{        
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (12));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptSrcNeighbour(wxT("All"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }
		
	break;}
  case  MainFrame::modeINVENTORYTRG:
		{        
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (8));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptSrcNeighbour(wxT("All"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }
		
	break; }
  case  MainFrame::modeMESHSELECTTRG:
		{        
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (10));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptSrcNeighbour(wxT("All"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }
		
	break;}
  case  MainFrame::modeENTERSECTORTRG:
		{        
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (17));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptSrcNeighbour(wxT("All"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }	
	break;
   }
  case  MainFrame::modeTIMEOUTTRG:
  {        
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (14));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptSrcNeighbour(wxT("All"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }	
	break;
    }
  case  MainFrame::modeOPERATIONTRG:
		{        
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (11));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptSrcNeighbour(wxT("All"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }	
	break;
   }
  case  MainFrame::modeWATCHTRG:
   {     
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (16));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptSrcNeighbour(wxT("All"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }	
	break;
     }
  case	MainFrame::modeACTION:
		{        
      node = (GraphNode*) graph->CreateNode (m_pParentFrame->GetGraphEditor ()->GetNodeFactory (18));
      if (node)
      {
        node->AcceptChild(wxT("wxSFTextShape"));
			  node->AcceptChild(wxT("wxSFEditTextShape"));
			  node->AcceptConnection(wxT("All"));
			  node->AcceptSrcNeighbour(wxT("All"));
			  node->AcceptTrgNeighbour(wxT("wxSFTextShape"));
			  node->AcceptTrgNeighbour(wxT("wxSFEditTextShape"));
			  node = (GraphNode*) graph->AddShape (node, graph->GetRootItem (),
                                                    wxPoint (event.GetX (),event.GetY ()), sfINITIALIZE, sfDONT_SAVE_STATE );
        if(node->GetParentShape ())
           node->GetParentShape ()->Refresh ();                          
      }
		
	break;
       }
  case MainFrame::modeLINK:
   {
      if(GetMode() == modeREADY)
      {
          //wxPoint lpos = DP2LP (event.GetPosition ());
          //GraphNode* node = graph->GetNode (GetShapeAtPosition(lpos)->GetId ());
          //GraphAnchor* srcAnchor = (GraphAnchor*) node->GetNearestConnectionPoint(wxRealPoint(event.GetX (),event.GetY ())); 

          GraphLink* link = graph->CreateLink ();
          //link->SetSourceAnchor (srcAnchor);

          StartInteractiveConnection(link, event.GetPosition());
         
      }
      else
          wxSFShapeCanvas::OnLeftDown(event);
   
  break;
   }
	
	default:
		// do default actions
		wxSFShapeCanvas::OnLeftDown(event);
	}

	if(node || pShape)
	{
	    ShowShadows(m_pParentFrame->m_fShowShadows, shadowALL);

	    SaveCanvasState();

        if(!event.ControlDown())
        {
            m_pParentFrame->SetToolMode(MainFrame::modeDESIGN);
        }

        node->Refresh ();
        if (pShape)
          pShape->Refresh ();
  }
}

void FrameCanvas::OnLeftDoubleClick (wxMouseEvent& event)
{
  // do default actions
  wxSFShapeCanvas::OnLeftDoubleClick (event);
  wxPoint lpos = DP2LP (event.GetPosition ());
  GraphNode* node = (GraphNode*) GetShapeAtPosition(lpos);
  if(node && !node->IsKindOf(CLASSINFO(wxSFLineShape)))
     propgrid->SetGraphNode (node);
}
void FrameCanvas::OnKeyDown(wxKeyEvent& event)
{
	switch(event.GetKeyCode())
	{
	case WXK_ESCAPE:
		m_pParentFrame->SetToolMode(MainFrame::modeDESIGN);
               // m_pParentFrame->Close ();
		break;
	}

	// do default actions
	wxSFShapeCanvas::OnKeyDown(event);
}

void FrameCanvas::OnConnectionFinished(wxSFLineShape* connection)
{
    if(connection)
    {
		// the line's ending style can be set like this:
        connection->SetTrgArrow(CLASSINFO(wxSFSolidArrow));
        // also wxSFOpenArrow, wxSFDiamondArrow and wxSFCircleArrow styles are available.
        connection->SetSrcArrow(CLASSINFO(wxSFCircleArrow));
		
        connection->AcceptChild(wxT("wxSFTextShape"));
        connection->AcceptChild(wxT("wxSFEditTextShape"));

        connection->AcceptConnection(wxT("All"));
        connection->AcceptSrcNeighbour(wxT("All"));
        connection->AcceptTrgNeighbour(wxT("All"));

        m_pParentFrame->SetToolMode(MainFrame::modeDESIGN);   
    }
}
