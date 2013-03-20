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

#include "decorators.h"
#include "ids.h"

DecoratorNode::DecoratorNode (): GraphNode (){}
DecoratorNode::DecoratorNode (GraphNodeFactory* factory)
  : GraphNode (factory)
{
   initialize ();
   pen = wxPen (*wxBLACK_PEN);
   brush = wxBrush (*wxWHITE_BRUSH);

   m_nRadius = sfdvROUNDRECTSHAPE_RADIUS;
   nodeSize = wxRealPoint (100, 50);

   MarkSerializableDataMembers();
}

DecoratorNode ::~DecoratorNode()
{
}


void DecoratorNode::SetParameters ()
{
 GraphNode::SetParameters ();
 initialize ();
}

void DecoratorNode::initialize ()
{
 switch (factory->GetId ())
 {
   case IDT_LOOP: 
   case IDT_EXE_LIMIT:
        GetParameter (0)->SetString ("unknown");
        GetParameter (1)->SetLong (0);
        break;
   case IDT_NEGATERETURN: 
        GetParameter (0)->SetString ("unknown");
        break;
 }
}

void DecoratorNode::MarkSerializableDataMembers()
{
	XS_SERIALIZE_EX(nodeSize, wxT("size"), wxRealPoint (100, 50));
	XS_SERIALIZE_EX(pen, wxT("border"), wxPen (*wxBLACK_PEN));
	XS_SERIALIZE_EX(brush, wxT("fill"), wxBrush (*wxWHITE_BRUSH));
	XS_SERIALIZE_EX(m_nRadius, wxT("radius"), sfdvROUNDRECTSHAPE_RADIUS);
}

wxRect DecoratorNode::GetBoundingBox()
{
    wxRealPoint apos = this->GetAbsolutePosition();
    return wxRect(wxPoint((int)apos.x, (int)apos.y), wxSize((int)nodeSize.x, (int)nodeSize.y ));
}

void DecoratorNode::Scale(double x, double y, bool children)
{
	// HINT: overload it for custom actions...

	if((x > 0) && (y > 0))
	{
		SetRectSize(nodeSize.x * x, nodeSize.y * y);

        // call default function implementation (needed for scaling of shape's children)
		wxSFShapeBase::Scale(x, y, children);
	}
}

void DecoratorNode::UpdateParameter (size_t index, csVariant* oldValue, csVariant* newValue)
{
  parameters[index] = *newValue;
  printf ("DecoratorNode::UpdateParameter %li\n", index);
}

void DecoratorNode::CreateHandles()
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


bool DecoratorNode::Contains(const wxPoint& pos)
{
    // HINT: overload it for custom actions...

    // get original bounding box
    wxRect shpBB = GetBoundingBox();

    // calculate modified boxes
    wxRect hr(shpBB);
    hr.Deflate(0, (int)m_nRadius);
    wxRect vr(shpBB);
    vr.Deflate((int)m_nRadius, 0);

    // test whether given position is inside body rect or rounded corners
    if(hr.Contains(pos))return true;
    else if(vr.Contains(pos))return true;
    else if(IsInCircle(pos, shpBB.GetTopLeft() + wxPoint((int)m_nRadius, (int)m_nRadius)))return true;
    else if(IsInCircle(pos, shpBB.GetBottomLeft() + wxPoint((int)m_nRadius, (int)-m_nRadius)))return true;
    else if(IsInCircle(pos, shpBB.GetTopRight() + wxPoint((int)-m_nRadius, (int)m_nRadius)))return true;
    else if(IsInCircle(pos, shpBB.GetBottomRight() + wxPoint((int)-m_nRadius, (int)-m_nRadius)))return true;

    return false;
}

void DecoratorNode::FitToChildren()
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


wxRealPoint DecoratorNode::GetBorderPoint(const wxRealPoint& start, const wxRealPoint& end)
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

void DecoratorNode::OnRightHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...

	//nodeSize.x = handle.GetPosition().x - GetAbsolutePosition().x;
	nodeSize.x += handle.GetDelta().x;
}

void DecoratorNode::OnLeftHandle(wxSFShapeHandle& handle)
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

void DecoratorNode::OnTopHandle(wxSFShapeHandle& handle)
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

void DecoratorNode::OnBottomHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...

	//nodeSize.y = handle.GetPosition().y - GetAbsolutePosition().y;
	nodeSize.y += handle.GetDelta().y;
}
  

void DecoratorNode::DrawNormal (wxDC& dc)
{
// HINT: overload it for custom actions...
 wxRealPoint pos = GetAbsolutePosition();
 wxRealPoint center = pos + wxRealPoint(nodeSize.y/4, nodeSize.y/4);
 dc.SetPen(pen);
 dc.SetBrush(brush);
 dc.SetFont (*wxNORMAL_FONT);
 dc.SetTextForeground (*wxBLACK);
 dc.SetTextBackground (*wxWHITE);
 switch (factory->GetId ())
 {
   case IDT_LOOP: 
   dc.DrawRoundedRectangle (wxSFCommonFcn::Conv2Point(GetAbsolutePosition()), wxSize (100, 50), m_nRadius);
   dc.DrawText(wxT("Loop"), wxSFCommonFcn::Conv2Point(center));
   break;
   case IDT_NEGATERETURN: 
   dc.DrawRoundedRectangle (wxSFCommonFcn::Conv2Point(GetAbsolutePosition()), wxSize (100, 50), m_nRadius);
   dc.DrawText(wxT("negatereturn"), wxSFCommonFcn::Conv2Point(center));
   break;
   case IDT_EXE_LIMIT:
   dc.DrawRoundedRectangle (wxSFCommonFcn::Conv2Point(GetAbsolutePosition()), wxSize (100, 50), m_nRadius); 
   dc.DrawText(wxT("execlimit"), wxSFCommonFcn::Conv2Point(center)); 
   break;
 
 }
 //dc.DrawEllipse(wxSFCommonFcn::Conv2Point(GetAbsolutePosition()), wxSize (100, 50));
 dc.SetBrush(wxNullBrush);
 dc.SetPen(wxNullPen);
}

void DecoratorNode::DrawHover (wxDC& dc)
{
// HINT: overload it for custom actions...
 wxRealPoint pos = GetAbsolutePosition();
 wxRealPoint center = pos + wxRealPoint(nodeSize.y/4, nodeSize.y/4);
 dc.SetPen(wxPen(m_nHoverColor, 1));
 dc.SetBrush(brush);
 dc.SetFont (*wxNORMAL_FONT);
 dc.SetTextForeground (*wxBLACK);
 dc.SetTextBackground (*wxWHITE);
 switch (factory->GetId ())
 {
   case IDT_LOOP: 
   dc.DrawRoundedRectangle (wxSFCommonFcn::Conv2Point(GetAbsolutePosition()), wxSize (100, 50), m_nRadius);
   dc.DrawText(wxT("Loop"), wxSFCommonFcn::Conv2Point(center));
   break;
   case IDT_NEGATERETURN: 
   dc.DrawRoundedRectangle (wxSFCommonFcn::Conv2Point(GetAbsolutePosition()), wxSize (100, 50), m_nRadius);
   break;
   case IDT_EXE_LIMIT:
   dc.DrawRoundedRectangle (wxSFCommonFcn::Conv2Point(GetAbsolutePosition()), wxSize (100, 50), m_nRadius);  
   break;
 }
 dc.SetBrush(wxNullBrush);
 dc.SetPen(wxNullPen);
}

void DecoratorNode::DrawHighlighted (wxDC& dc)
{
// HINT: overload it for custom actions...
 wxRealPoint pos = GetAbsolutePosition();
 wxRealPoint center = pos + wxRealPoint(nodeSize.y/4, nodeSize.y/4);
 dc.SetPen(wxPen(m_nHoverColor, 2));
 dc.SetBrush(brush);
 dc.SetFont (*wxNORMAL_FONT);
 dc.SetTextForeground (*wxBLACK);
 dc.SetTextBackground (*wxWHITE);
 switch (factory->GetId ())
 {
   case IDT_LOOP: 
   dc.DrawRoundedRectangle (wxSFCommonFcn::Conv2Point(GetAbsolutePosition()), wxSize (100, 50), m_nRadius);
   dc.DrawText(wxT("Loop decorator"), wxSFCommonFcn::Conv2Point(center));
   break;
   case IDT_NEGATERETURN: 
   dc.DrawRoundedRectangle (wxSFCommonFcn::Conv2Point(GetAbsolutePosition()), wxSize (100, 50), m_nRadius);
   break;
   case IDT_EXE_LIMIT:
   dc.DrawRoundedRectangle (wxSFCommonFcn::Conv2Point(GetAbsolutePosition()), wxSize (100, 50), m_nRadius);  
   break;
 }

}

void DecoratorNode::DrawShadow (wxDC& dc)
{
// HINT: overload it for custom actions...
 if( brush.GetStyle() != wxTRANSPARENT )
 {
  dc.SetPen(*wxTRANSPARENT_PEN);
  dc.SetBrush(GetParentCanvas()->GetShadowFill());
 switch (factory->GetId ())
 {
   case IDT_LOOP: 
   //dc.DrawRoundedRectangle (wxSFCommonFcn::Conv2Point(GetAbsolutePosition() + GetParentCanvas()->GetShadowOffset()), wxSize (100, 50), m_nRadius);
   break;
   case IDT_NEGATERETURN: 
   dc.DrawRoundedRectangle(wxSFCommonFcn::Conv2Point(GetAbsolutePosition() + GetParentCanvas()->GetShadowOffset()), wxSize (100, 50), m_nRadius);
   break;
   case IDT_EXE_LIMIT:
   dc.DrawRoundedRectangle (wxSFCommonFcn::Conv2Point(GetAbsolutePosition() + GetParentCanvas()->GetShadowOffset()), wxSize (100, 50), m_nRadius);  
   break;
 }
  dc.SetBrush(wxNullBrush);
  dc.SetPen(wxNullPen);
 }
}

void DecoratorNode::OnHandle(wxSFShapeHandle& handle)
{
    // HINT: overload it for custom actions...

	switch(handle.GetType())
	{
	case wxSFShapeHandle::hndLEFT:
		OnLeftHandle(handle);
		break;

	case wxSFShapeHandle::hndLEFTTOP:
		OnLeftHandle(handle);
		OnTopHandle(handle);
		break;

	case wxSFShapeHandle::hndLEFTBOTTOM:
		OnLeftHandle(handle);
		OnBottomHandle(handle);
		break;

	case wxSFShapeHandle::hndRIGHT:
		OnRightHandle(handle);
		break;

	case wxSFShapeHandle::hndRIGHTTOP:
		OnRightHandle(handle);
		OnTopHandle(handle);
		break;

	case wxSFShapeHandle::hndRIGHTBOTTOM:
		OnRightHandle(handle);
		OnBottomHandle(handle);
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
	
	wxSFShapeBase::OnHandle( handle );
}

void DecoratorNode::OnBeginHandle(wxSFShapeHandle& handle)
{
    m_nPrevPosition = m_nRelativePosition;
    m_nPrevSize = nodeSize;
	
	wxSFShapeBase::OnBeginHandle( handle );
}

bool DecoratorNode::IsInCircle(const wxPoint& pos, const wxPoint& center)
{
    return (wxSFCommonFcn::Distance(wxSFCommonFcn::Conv2RealPoint(center), wxSFCommonFcn::Conv2RealPoint(pos)) <= m_nRadius);
}
