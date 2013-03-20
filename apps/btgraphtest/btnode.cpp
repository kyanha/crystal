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

#include "btnode.h"

BTNode::BTNode (): GraphNode (){}
BTNode::BTNode (GraphNodeFactory* factory)
  : GraphNode (factory)
{
   initialize ();
   pen = wxPen (*wxBLACK_PEN);
   brush = wxBrush (*wxGREEN_BRUSH);
   nodeSize = wxRealPoint (75, 75);

   MarkSerializableDataMembers();
}

/*BTNode::BTNode (const GraphNode* obj, GraphNodeFactory* factory)
                      : GraphNode (obj, factory)
{
 initialize ();
 pen = wxPen (*wxBLACK_PEN);
 brush = wxBrush (*wxWHITE_BRUSH); 
}*/

BTNode ::~BTNode()
{
}


void BTNode::SetParameters ()
{
 GraphNode::SetParameters ();
 initialize ();
}

void BTNode::initialize ()
{
 GetParameter (0)->SetString ("unknown");
}

void BTNode::MarkSerializableDataMembers()
{
	XS_SERIALIZE_EX(nodeSize, wxT("size"), wxRealPoint (100, 50));
	XS_SERIALIZE_EX(pen, wxT("border"), wxPen (*wxBLACK_PEN));
	XS_SERIALIZE_EX(brush, wxT("fill"), wxBrush (*wxWHITE_BRUSH));
}

wxRect BTNode::GetBoundingBox()
{
    wxRealPoint apos = this->GetAbsolutePosition();
    return wxRect(wxPoint((int)apos.x, (int)apos.y), wxSize((int)nodeSize.x, (int)nodeSize.y ));
}

void BTNode::Scale(double x, double y, bool children)
{
	// HINT: overload it for custom actions...

	if((x > 0) && (y > 0))
	{
                double s = 1;
                if (x == 1) s = y;
                else if (y == 1) s = x;
                else if (x >= y) s = x;
                else s = y;

		SetRectSize(nodeSize.x * x, nodeSize.y * y);

        // call default function implementation (needed for scaling of shape's children)
		wxSFShapeBase::Scale(x, y, children);
	}
}

void BTNode::UpdateParameter (size_t index, csVariant* oldValue, csVariant* newValue)
{
  parameters[index] = *newValue;
  printf ("BTNode::UpdateParameter %li\n", index);
}

void BTNode::FitToChildren()
{
    // HINT: overload it for custom actions...

    wxSFShapeBase* pChild;

    // get bounding box of the shape and children set be inside it	
	wxRect chBB = this->GetBoundingBox();
	wxRect shpBB = chBB;

    SerializableList::compatibility_iterator node = GetFirstChildNode();
    while(node)
    {
        pChild = (wxSFShapeBase*)node->GetData();

        if( pChild->ContainsStyle(sfsALWAYS_INSIDE) )
        {
            pChild->GetCompleteBoundingBox(chBB, bbSELF | bbCHILDREN);
        }
        node = node->GetNext();
    }

	if(!chBB.IsEmpty())
	{
		//wxRect shpBB = this->GetBoundingBox();

		if(!shpBB.Contains(chBB))
		{
			double dx = chBB.GetLeft() - shpBB.GetLeft();
			double dy = chBB.GetTop() - shpBB.GetTop();

			// resize parent shape
			shpBB.Union(chBB);
			MoveTo(shpBB.GetPosition().x, shpBB.GetPosition().y);
			nodeSize = wxRealPoint(shpBB.GetSize().x, shpBB.GetSize().y);

			// move its "1st level" children if neccessary
			if((dx < 0) || (dy < 0))
			{
				node = GetFirstChildNode();
				while(node)
				{
					pChild = (wxSFShapeBase*)node->GetData();
					if(dx < 0)pChild->MoveBy(abs((int)dx), 0);
					if(dy < 0)pChild->MoveBy(0, abs((int)dy));

					node = node->GetNext();
				}
			}
		}
	}
}


void BTNode::CreateHandles()
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


bool BTNode::Contains(const wxPoint& pos)
{
    // HINT: overload it for custom actions...

    wxRealPoint center = GetAbsolutePosition() + wxRealPoint(nodeSize.x/2, nodeSize.y/2);

    if(wxSFCommonFcn::Distance(center, wxRealPoint(pos.x, pos.y)) <= nodeSize.x/2) return true;
    else
        return false;
}


wxRealPoint BTNode::GetBorderPoint(const wxRealPoint& start, const wxRealPoint& end)
{
    // HINT: override it for custom actions ...

    // the function calculates intesection of line leading from the shape center to
    // given point with the shape's bounding box;
	double dist = wxSFCommonFcn::Distance(start, end);

	if(dist)
	{
		double srcDx = nodeSize.x/2*(end.x-start.x)/dist;
		double srcDy = nodeSize.y/2*(end.y-start.y)/dist;

		return wxRealPoint(start.x + srcDx, start.y + srcDy);
	}
	else
	return GetAbsolutePosition() + wxRealPoint(nodeSize.x/2, nodeSize.y/2); // center
}

void BTNode::OnRightHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...

	//nodeSize.x = handle.GetPosition().x - GetAbsolutePosition().x;
	nodeSize.x += handle.GetDelta().x;
}

void BTNode::OnLeftHandle(wxSFShapeHandle& handle)
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

void BTNode::OnTopHandle(wxSFShapeHandle& handle)
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

void BTNode::OnBottomHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...

	//nodeSize.y = handle.GetPosition().y - GetAbsolutePosition().y;
	nodeSize.y += handle.GetDelta().y;
}
  

void BTNode::DrawNormal (wxDC& dc)
{
// HINT: overload it for custom actions...
 wxRealPoint pos = GetAbsolutePosition();
 wxRealPoint center = pos + wxRealPoint(nodeSize.y/6, nodeSize.y/2);
 dc.SetPen(pen);
 dc.SetBrush(brush);
 dc.SetFont (*wxNORMAL_FONT);
 dc.SetTextForeground (*wxBLACK);
 dc.SetTextBackground (*wxWHITE);
 dc.DrawCircle (int(pos.x + nodeSize.x/2), int(pos.y + nodeSize.y/2), int(nodeSize.x/2));
 dc.DrawText(wxT("BT node"), wxSFCommonFcn::Conv2Point(center));
}

void BTNode::DrawHover (wxDC& dc)
{
// HINT: overload it for custom actions...
 wxRealPoint pos = GetAbsolutePosition();
 wxRealPoint center = pos + wxRealPoint(nodeSize.y/6, nodeSize.y/4);
 dc.SetPen(wxPen(m_nHoverColor, 1));
 dc.SetBrush(brush);
 dc.SetFont (*wxNORMAL_FONT);
 dc.SetTextForeground (*wxBLACK);
 dc.SetTextBackground (*wxWHITE);
 dc.DrawCircle (int(pos.x + nodeSize.x/2), int(pos.y + nodeSize.y/2), int(nodeSize.x/2));
 dc.DrawText(wxT("BT node"), wxSFCommonFcn::Conv2Point(center));

}

void BTNode::DrawHighlighted (wxDC& dc)
{
// HINT: overload it for custom actions...
 wxRealPoint pos = GetAbsolutePosition();
 wxRealPoint center = pos + wxRealPoint(nodeSize.y/6, nodeSize.y/4);
 dc.SetPen(wxPen(m_nHoverColor, 2));
 dc.SetBrush(brush);
 dc.SetBrush(brush);
 dc.SetFont (*wxNORMAL_FONT);
 dc.SetTextForeground (*wxBLACK);
 dc.SetTextBackground (*wxWHITE);
 dc.DrawCircle (int(pos.x + nodeSize.x/2), int(pos.y + nodeSize.y/2), int(nodeSize.x/2));
 dc.DrawText(wxT("BT node"), wxSFCommonFcn::Conv2Point(center));

}

void BTNode::DrawShadow (wxDC& dc)
{
// HINT: overload it for custom actions...
 wxRealPoint pos = GetAbsolutePosition();
 if( brush.GetStyle() != wxTRANSPARENT )
 {
  dc.SetPen(*wxTRANSPARENT_PEN);
  dc.SetBrush(GetParentCanvas()->GetShadowFill());
  dc.DrawCircle(int(pos.x + nodeSize.x/2 + GetParentCanvas()->GetShadowOffset().x), 
                     int(pos.y + nodeSize.y/2 + GetParentCanvas()->GetShadowOffset().y),  int(nodeSize.x/2));
  dc.SetBrush(wxNullBrush);
  dc.SetPen(wxNullPen);
 }
}

void BTNode::OnHandle(wxSFShapeHandle& handle)
{
    // HINT: overload it for custom actions...
        double maxsize = 0, dx = 0, dy = 0;
        wxRealPoint prevSize = nodeSize;

	switch(handle.GetType())
	{
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
}

void BTNode::OnBeginHandle(wxSFShapeHandle& handle)
{
    m_nPrevPosition = m_nRelativePosition;
    m_nPrevSize = nodeSize;
	
	wxSFShapeBase::OnBeginHandle( handle );
}
