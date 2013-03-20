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

#include "triggers.h"

TriggerNode::TriggerNode (): GraphNode (){}
TriggerNode::TriggerNode (GraphNodeFactory* factory)
  : GraphNode (factory)
{
   initialize ();
   pen = wxPen (*wxBLACK_PEN);
   brush = wxBrush (*wxWHITE_BRUSH);
   nodeSize = wxRealPoint (125, 75);

   MarkSerializableDataMembers();
}

TriggerNode ::~TriggerNode()
{
}

void TriggerNode::SetParameters ()
{
 GraphNode::SetParameters ();
 initialize ();
}

void TriggerNode::initialize ()
{
 switch (factory->GetId ())
 {
   case IDT_INVENTORY:
   case IDT_MESSAGE:
   case IDT_SEQ_FINISH: 
         GetParameter (0)->SetString ("unknown");
         GetParameter (1)->SetString ("unknown");
         GetParameter (2)->SetString ("unknown");
         break;
   case IDT_MESH_SELECT:
   case IDT_OPERATION:
   case IDT_TRIGGER:
   case IDT_ENTER_SECTOR:
         GetParameter (0)->SetString ("unknown");
         GetParameter (1)->SetString ("unknown");
         break;

   case IDT_PROP_CHANGE:
         GetParameter (0)->SetString ("unknown");
         GetParameter (1)->SetString ("unknown");
         GetParameter (2)->SetBool (true);
         GetParameter (3)->SetString ("eq");
         GetParameter (4)->SetString ("unknown");
         GetParameter (5)->SetString ("unknown");
         break;

   case IDT_TIMEOUT: 
         GetParameter (0)->SetString ("unknown");
         GetParameter (1)->SetFloat (0.000f);
         break;
   case IDT_WATCH:
         GetParameter (0)->SetString ("unknown");
         GetParameter (1)->SetString ("unknown");
         GetParameter (2)->SetString ("unknown");
         GetParameter (3)->SetFloat (0.000f);
         GetParameter (4)->SetFloat (0.000f);
         GetParameter (5)->SetVector3 (csVector3  (0.0f,0.0f,0.0f));
         break;
 }
}

void TriggerNode::UpdateParameter (size_t index, csVariant* oldValue, csVariant* newValue)
{
  parameters[index] = *newValue;
  printf ("TriggerNode::UpdateParameter %li\n", index);
}

void TriggerNode::MarkSerializableDataMembers()
{
	XS_SERIALIZE_EX(nodeSize, wxT("size"), wxRealPoint (125, 75));
	XS_SERIALIZE_EX(pen, wxT("border"), wxPen (*wxBLACK_PEN));
	XS_SERIALIZE_EX(brush, wxT("fill"), wxBrush (*wxWHITE_BRUSH));
}

wxRect TriggerNode::GetBoundingBox()
{
    wxRealPoint apos = this->GetAbsolutePosition();
    return wxRect(wxPoint((int)apos.x, (int)apos.y), wxSize((int)nodeSize.x, (int)nodeSize.y ));
}

void TriggerNode::CreateHandles()
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


bool TriggerNode::Contains(const wxPoint& pos)
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

void TriggerNode::Scale(double x, double y, bool children)
{
	// HINT: overload it for custom actions...

	if((x > 0) && (y > 0))
	{
		SetRectSize(nodeSize.x * x, nodeSize.y * y);

        // call default function implementation (needed for scaling of shape's children)
		wxSFShapeBase::Scale(x, y, children);
	}
}

void TriggerNode::FitToChildren()
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


wxRealPoint TriggerNode::GetBorderPoint(const wxRealPoint& start, const wxRealPoint& end)
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

void TriggerNode::OnRightHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...

	//nodeSize.x = handle.GetPosition().x - GetAbsolutePosition().x;
	nodeSize.x += handle.GetDelta().x;
}

void TriggerNode::OnLeftHandle(wxSFShapeHandle& handle)
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

void TriggerNode::OnTopHandle(wxSFShapeHandle& handle)
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

void TriggerNode::OnBottomHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...

	//nodeSize.y = handle.GetPosition().y - GetAbsolutePosition().y;
	nodeSize.y += handle.GetDelta().y;
}
  
void TriggerNode::DrawNormal (wxDC& dc)
{
// HINT: overload it for custom actions...
 wxRealPoint pos = GetAbsolutePosition();
 wxRealPoint center = pos + wxRealPoint(nodeSize.y/6, nodeSize.y/2);
 dc.SetPen(pen);
 dc.SetBrush(brush);
 switch (factory->GetId ())
 {
   case IDT_INVENTORY: 
   case IDT_MESSAGE: 
   case IDT_MESH_SELECT: 
   case IDT_OPERATION: 
   case IDT_PROP_CHANGE: 
   case IDT_SEQ_FINISH: 
   case IDT_TIMEOUT: 
   case IDT_TRIGGER: 
   case IDT_WATCH: 
       dc.DrawRectangle (wxSFCommonFcn::Conv2Point(center), wxSize (125, 75));
        dc.DrawText(wxT ("watch trigger"), wxSFCommonFcn::Conv2Point(center));
   case IDT_ENTER_SECTOR:
       break;
 }
 dc.SetBrush(wxNullBrush);
 dc.SetPen(wxNullPen);
}

void TriggerNode::DrawHover (wxDC& dc)
{
// HINT: overload it for custom actions...
 wxRealPoint pos = GetAbsolutePosition();
 wxRealPoint center = pos + wxRealPoint(nodeSize.y/6, nodeSize.y/2);
 dc.SetPen(wxPen(m_nHoverColor, 1));
 dc.SetBrush(brush);
 switch (factory->GetId ())
 {
   case IDT_INVENTORY: 
   case IDT_MESSAGE: 
   case IDT_MESH_SELECT: 
   case IDT_OPERATION: 
   case IDT_PROP_CHANGE: 
   case IDT_SEQ_FINISH: 
   case IDT_TIMEOUT: 
   case IDT_TRIGGER: 
   case IDT_WATCH: 
       dc.DrawRectangle (wxSFCommonFcn::Conv2Point(center), wxSize (125, 75));
        dc.DrawText(wxT ("watch trigger"), wxSFCommonFcn::Conv2Point(center));
   case IDT_ENTER_SECTOR:
       break;
 }
 dc.SetBrush(wxNullBrush);
 dc.SetPen(wxNullPen);
}

void TriggerNode::DrawHighlighted (wxDC& dc)
{
// HINT: overload it for custom actions...
 wxRealPoint pos = GetAbsolutePosition();
 wxRealPoint center = pos + wxRealPoint(nodeSize.y/6, nodeSize.y/2);
 dc.SetPen(wxPen(m_nHoverColor, 2));
 dc.SetBrush(brush);
 switch (factory->GetId ())
 {
   case IDT_INVENTORY: 
   case IDT_MESSAGE: 
   case IDT_MESH_SELECT: 
   case IDT_OPERATION: 
   case IDT_PROP_CHANGE: 
   case IDT_SEQ_FINISH: 
   case IDT_TIMEOUT: 
   case IDT_TRIGGER: 
   case IDT_WATCH: 
        dc.DrawRectangle (wxSFCommonFcn::Conv2Point(center), wxSize (125, 75));
        dc.DrawText(wxT ("watch trigger"), wxSFCommonFcn::Conv2Point(center));
   case IDT_ENTER_SECTOR:
       break;
 }
 dc.SetBrush(wxNullBrush);
 dc.SetPen(wxNullPen);
}

void TriggerNode::DrawShadow (wxDC& dc)
{
// HINT: overload it for custom actions...
 if( brush.GetStyle() != wxTRANSPARENT )
 {
  dc.SetPen(*wxTRANSPARENT_PEN);
  dc.SetBrush(GetParentCanvas()->GetShadowFill());
 switch (factory->GetId ())
 {
   case IDT_INVENTORY: 
   case IDT_MESSAGE: 
   case IDT_MESH_SELECT: 
   case IDT_OPERATION: 
   case IDT_PROP_CHANGE: 
   case IDT_SEQ_FINISH: 
   case IDT_TIMEOUT: 
   case IDT_TRIGGER: 
   case IDT_WATCH: 
   case IDT_ENTER_SECTOR:
         if( brush.GetStyle() != wxTRANSPARENT )
         {
          dc.SetPen(*wxTRANSPARENT_PEN);
          dc.SetBrush(GetParentCanvas()->GetShadowFill());
          dc.DrawRectangle(wxSFCommonFcn::Conv2Point(GetAbsolutePosition() + GetParentCanvas()->GetShadowOffset()), wxSize (125, 75));
         }
       break;
 }
  dc.SetBrush(wxNullBrush);
  dc.SetPen(wxNullPen);
 }
}

void TriggerNode::OnHandle(wxSFShapeHandle& handle)
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

void TriggerNode::OnBeginHandle(wxSFShapeHandle& handle)
{
    m_nPrevPosition = m_nRelativePosition;
    m_nPrevSize = nodeSize;
	
	wxSFShapeBase::OnBeginHandle( handle );
}
