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
#ifndef _FRAMECANVAS_H
#define _FRAMECANVAS_H

#include <wx/wx.h>
#include "graphedit/wxsf/wxShapeFramework.h"
#include "myGraph.h"
#include "propgrid.h"
#include "wxgraph.h"


class MainFrame;
class MyGraph;
class PropGridManager;
class GraphNode;

class FrameCanvas : public wxSFShapeCanvas

{
public:
    FrameCanvas(){;}
	FrameCanvas(MyGraph* graph,PropGridManager* propgrid , wxWindow* parent, wxWindowID id);
	virtual ~FrameCanvas(void);

	// public virtual functions
	virtual void OnLeftDown(wxMouseEvent& event);
	//virtual void OnRightDown(wxMouseEvent& event);
	virtual void OnKeyDown(wxKeyEvent& event);
	virtual void OnLeftDoubleClick (wxMouseEvent& event);
	virtual void OnConnectionFinished(wxSFLineShape* connection);

protected:
	// protected data members
	MainFrame *m_pParentFrame;
	MyGraph* graph;
        PropGridManager* propgrid;
};

#endif //_FRAMECANVAS_H
