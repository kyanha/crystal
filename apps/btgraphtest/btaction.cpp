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

#include "graphedit/wxsf/wx_pch.h"
#include "graphedit/wxsf/CommonFcn.h"

#include "btaction.h"

BTAction::BTAction (): GraphNode (){}
BTAction::BTAction (GraphNodeFactory* factory)
  : GraphNode (factory)
{
   initialize ();
   pen = wxPen (*wxBLACK_PEN);
   brush = wxBrush (*wxWHITE_BRUSH);
   nodeSize = wxRealPoint (75, 75);

   MarkSerializableDataMembers();
}

BTAction ::~BTAction()
{
}


void BTAction::SetParameters ()
{
 GraphNode::SetParameters ();
 initialize ();
}

void BTAction::initialize ()
{
 GetParameter (0)->SetString ("unknown");
}

void BTAction::MarkSerializableDataMembers()
{
	XS_SERIALIZE_EX(nodeSize, wxT("size"), wxRealPoint (100, 50));
	XS_SERIALIZE_EX(pen, wxT("border"), wxPen (*wxBLACK_PEN));
	XS_SERIALIZE_EX(brush, wxT("fill"), wxBrush (*wxWHITE_BRUSH));
}

wxRect BTAction::GetBoundingBox()
{
    wxRealPoint apos = this->GetAbsolutePosition();
    return wxRect(wxPoint((int)apos.x, (int)apos.y), wxSize((int)nodeSize.x, (int)nodeSize.y ));
}

void BTAction::Scale(double x, double y, bool children)
{
	// HINT: overload it for custom actions...

	if((x > 0) && (y > 0))
	{
	    double s = 1;

        if(x == 1) s = y;
        else if (y == 1) s = x;
	    else if(x >= y) s = x;
	    else
            s = y;

		SetRectSize(nodeSize.x * s, nodeSize.y * s);

        // call default function implementation (needed for scaling of shape's children)
		wxSFShapeBase::Scale(x, y, children);
	}
}

void BTAction::UpdateParameter (size_t index, csVariant* oldValue, csVariant* newValue)
{
  parameters[index] = *newValue;
  printf ("BTAction::UpdateParameter %li\n", index);
}

void BTAction::CreateHandles()
{
	// HINT: overload it for custom actions...

	AddHandle(wxSFShapeHandle::hndLEFTTOP);
	AddHandle(wxSFShapeHandle::hndTOP);
	AddHandle(wxSFShapeHandle::hndRIGHTTOP);
	AddHandle(wxSFShapeHandle::hndRIGHT);
	AddHandle(wxSFShapeHandle::hndRIGHTBOTTOM);
	AddHandle(wxSFShapeHandle::hndBOTTOM);
	AddHandle(wxSFShapeHandle::hndLEFTBOTTOM);
	AddHandle(wxSFShapeHandle::hndLEFT);
	AddHandle(wxSFShapeHandle::hndLEFTTOP);
}


bool BTAction::Contains(const wxPoint& pos)
{
    // HINT: overload it for custom actions...

    double m, n, a, b;
    wxRealPoint apos;

    a = GetRectSize().x/2;
    b = GetRectSize().y/2;
    apos = GetAbsolutePosition();

    m = apos.x + a;
    n = apos.y + b;

    if((((pos.x - m)*(pos.x - m))/(a*a) + ((pos.y - n)*(pos.y - n))/(b*b)) < 1)return true;
    else
        return false;
}


wxRealPoint BTAction::GetBorderPoint(const wxRealPoint& start, const wxRealPoint& end)
{
    // HINT: override it for custom actions ...

    // the function calculates intesection of line leading from the shape center to
    // given point with the shape's bounding box;

    wxRealPoint intersection;
    wxRect bbRct = this->GetBoundingBox();


    if(wxSFCommonFcn::LinesIntersection(wxRealPoint(bbRct.GetTopLeft().x, bbRct.GetTopLeft().y),
                             wxRealPoint(bbRct.GetTopRight().x + 1, bbRct.GetTopRight().y), start, end, intersection)) return intersection;

    if(wxSFCommonFcn::LinesIntersection(wxRealPoint(bbRct.GetTopRight().x + 1, bbRct.GetTopRight().y),
                                  wxRealPoint(bbRct.GetBottomRight().x + 1, bbRct.GetBottomRight().y + 1), start, end, intersection)) return intersection;

	if(wxSFCommonFcn::LinesIntersection(wxRealPoint(bbRct.GetBottomRight().x + 1, bbRct.GetBottomRight().y + 1),
                                  wxRealPoint(bbRct.GetBottomLeft().x, bbRct.GetBottomLeft().y + 1), start, end, intersection)) return intersection;

    if(wxSFCommonFcn::LinesIntersection(wxRealPoint(bbRct.GetBottomLeft().x, bbRct.GetBottomLeft().y + 1),
                                  wxRealPoint(bbRct.GetTopLeft().x, bbRct.GetTopLeft().y), start, end, intersection)) return intersection;
	
    return GetCenter();
}

void BTAction::OnRightHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...

	//nodeSize.x = handle.GetPosition().x - GetAbsolutePosition().x;
	nodeSize.x += handle.GetDelta().x;
}

void BTAction::OnLeftHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...

    wxSFShapeBase *pChild;

	//double dx = (double)handle.GetPosition().x - GetAbsolutePosition().x;
	double dx = (double)handle.GetDelta().x;

	// update position of children
	if( !ContainsStyle(sfsLOCK_CHILDREN) )
	{	
		SerializableList::compatibility_iterator node = GetFirstChildNode();
		while(node)
		{
			pChild = (wxSFShapeBase*)node->GetData();
			if( pChild->GetHAlign() == halignNONE )
			{
				pChild->MoveBy(-dx, 0);
			}
			node = node->GetNext();
		}
	}
	// update position and size of the shape
	nodeSize.x -= dx;
	m_nRelativePosition.x += dx;
}

void BTAction::OnTopHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...

    wxSFShapeBase *pChild;

	//double dy = (double)handle.GetPosition().y - GetAbsolutePosition().y;
	double dy = (double)handle.GetDelta().y;

	// update position of children
	if( !ContainsStyle( sfsLOCK_CHILDREN ) )
	{
		SerializableList::compatibility_iterator node = GetFirstChildNode();
		while(node)
		{
			pChild = (wxSFShapeBase*)node->GetData();
			if( pChild->GetVAlign() == valignNONE )
			{
				pChild->MoveBy(0, -dy);
			}
			node = node->GetNext();
		}
	}
	// update position and size of the shape
	nodeSize.y -= dy;
	m_nRelativePosition.y += dy;
}

void BTAction::OnBottomHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...

	//nodeSize.y = handle.GetPosition().y - GetAbsolutePosition().y;
	nodeSize.y += handle.GetDelta().y;
}
  

void BTAction::DrawNormal (wxDC& dc)
{
// HINT: overload it for custom actions...
 wxRealPoint pos = GetAbsolutePosition();
 wxRealPoint center = pos + wxRealPoint(nodeSize.y/6, nodeSize.y/2);
 dc.SetPen(pen);
 dc.SetBrush(brush);
 dc.SetFont (*wxNORMAL_FONT);
 dc.SetTextForeground (*wxBLACK);
 dc.SetTextBackground (*wxWHITE);
 dc.DrawRectangle(wxSFCommonFcn::Conv2Point(GetAbsolutePosition()), wxSize (75, 75));
 dc.DrawText(wxT("Action"), wxSFCommonFcn::Conv2Point(center));

}

void BTAction::DrawHover (wxDC& dc)
{
// HINT: overload it for custom actions...
 wxRealPoint pos = GetAbsolutePosition();
 wxRealPoint center = pos + wxRealPoint(nodeSize.y/6, nodeSize.y/2);
 dc.SetPen(wxPen(m_nHoverColor, 1));
 dc.SetBrush(brush);
 dc.SetFont (*wxNORMAL_FONT);
 dc.SetTextForeground (*wxBLACK);
 dc.SetTextBackground (*wxWHITE);
 dc.DrawRectangle(wxSFCommonFcn::Conv2Point(GetAbsolutePosition()), wxSize (75, 75));
 dc.DrawText(wxT("Action"), wxSFCommonFcn::Conv2Point(center));
}

void BTAction::DrawHighlighted (wxDC& dc)
{
// HINT: overload it for custom actions...
 wxRealPoint pos = GetAbsolutePosition();
 wxRealPoint center = pos + wxRealPoint(nodeSize.y/6, nodeSize.y/2);
 dc.SetPen(wxPen(m_nHoverColor, 2));
 dc.SetBrush(brush);
 dc.SetFont (*wxNORMAL_FONT);
 dc.SetTextForeground (*wxBLACK);
 dc.SetTextBackground (*wxWHITE);
 dc.DrawRectangle(wxSFCommonFcn::Conv2Point(GetAbsolutePosition()), wxSize (75, 75));
 dc.DrawText(wxT("Action"), wxSFCommonFcn::Conv2Point(center));

}

void BTAction::DrawShadow (wxDC& dc)
{
// HINT: overload it for custom actions...
 if( brush.GetStyle() != wxTRANSPARENT )
 {
  dc.SetPen(*wxTRANSPARENT_PEN);
  dc.SetBrush(GetParentCanvas()->GetShadowFill());
  dc.DrawRectangle(wxSFCommonFcn::Conv2Point(GetAbsolutePosition() + GetParentCanvas()->GetShadowOffset()), wxSize (75, 75));
 }
}

void BTAction::OnHandle(wxSFShapeHandle& handle)
{

    double maxsize = 0, dx = 0, dy = 0;

    wxRealPoint prevSize = nodeSize;

    // perform standard operations
	switch(handle.GetType())
	{
    case wxSFShapeHandle::hndLEFTTOP:
	case wxSFShapeHandle::hndLEFT:
	case wxSFShapeHandle::hndLEFTBOTTOM:
		OnLeftHandle(handle);
		break;

    case wxSFShapeHandle::hndRIGHTTOP:
	case wxSFShapeHandle::hndRIGHT:
	case wxSFShapeHandle::hndRIGHTBOTTOM:
		OnRightHandle(handle);
		break;

	case wxSFShapeHandle::hndTOP:
		OnTopHandle(handle);
		break;

	case wxSFShapeHandle::hndBOTTOM:
		OnBottomHandle(handle);
		break;

    default:
        break;
	}

    // calculate common size and some auxilary values
    if((prevSize.x < nodeSize.x) || (prevSize.y < nodeSize.y))
    {
        if(nodeSize.x >= nodeSize.y)maxsize = nodeSize.x;
        else
            maxsize = nodeSize.y;
    }
    else
    {
        if(nodeSize.x <= nodeSize.y)maxsize = nodeSize.x;
        else
            maxsize = nodeSize.y;
    }

    dx = maxsize - nodeSize.x;
    dy = maxsize - nodeSize.y;

    // normalize rect sizes
    nodeSize.x = nodeSize.y = maxsize;

    // move rect if neccessary
    switch(handle.GetType())
	{
	case wxSFShapeHandle::hndLEFT:
        MoveBy(-dx, -dy/2);
		break;

	case wxSFShapeHandle::hndLEFTTOP:
        MoveBy(-dx, -dy);
		break;

	case wxSFShapeHandle::hndLEFTBOTTOM:
        MoveBy(-dx, 0);
		break;

	case wxSFShapeHandle::hndRIGHT:
        MoveBy(0, -dy/2);
		break;

	case wxSFShapeHandle::hndRIGHTTOP:
        MoveBy(0, -dy);
		break;

	case wxSFShapeHandle::hndTOP:
        MoveBy(-dx/2, -dy);
		break;

	case wxSFShapeHandle::hndBOTTOM:
        MoveBy(-dx/2, 0);
		break;

    default:
        break;
	}
	
	wxSFShapeBase::OnHandle( handle );
}

void BTAction::OnBeginHandle(wxSFShapeHandle& handle)
{
    m_nPrevPosition = m_nRelativePosition;
    m_nPrevSize = nodeSize;
	
	wxSFShapeBase::OnBeginHandle( handle );
}
