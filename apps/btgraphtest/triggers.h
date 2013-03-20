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
#ifndef __TRIGGERS_H__
#define __TRIGGERS_H__

#include <wx/wx.h>
#include <wx/dc.h>

#include "graphedit/wxsf/wxShapeFramework.h"

#include "graphedit/graphNode.h"
#include "graphedit/graphNodeFactory.h"

#include "ids.h"

class TriggerNode : public GraphNode
{
 public:

 TriggerNode ();

 TriggerNode (GraphNodeFactory* factory);

 ~TriggerNode ();

// public virtual functions
virtual void SetParameters ();
void UpdateParameter (size_t index, csVariant* oldValue, csVariant* newValue);

 virtual bool Contains(const wxPoint& pos);

 virtual wxRect GetBoundingBox();

 virtual wxRealPoint GetBorderPoint(const wxRealPoint& start, const wxRealPoint& end);

 virtual void CreateHandles();

 virtual void OnHandle(wxSFShapeHandle& handle);

 virtual void OnBeginHandle(wxSFShapeHandle& handle);

 virtual void FitToChildren();

 virtual void Scale(double x, double y, bool children = sfWITHCHILDREN);

 void SetFill(const wxBrush& brush){this->brush  = brush;}

 wxBrush GetFill() const {return brush;}

 void SetBorder(const wxPen& pen){this->pen = pen;}

 wxPen GetBorder() const {return pen;}

 void SetRectSize(const wxRealPoint& size){nodeSize = size;}

 void SetRectSize(double x, double y){nodeSize.x = x; nodeSize.y = y;}

 wxRealPoint GetRectSize() const {return nodeSize;}

 protected:

  virtual void DrawNormal(wxDC& dc);

  virtual void DrawHover(wxDC& dc);

  virtual void DrawHighlighted(wxDC& dc);

  virtual void DrawShadow(wxDC& dc);

  virtual void OnRightHandle(wxSFShapeHandle& handle);

  virtual void OnLeftHandle(wxSFShapeHandle& handle);

  virtual void OnTopHandle(wxSFShapeHandle& handle);

  virtual void OnBottomHandle(wxSFShapeHandle& handle);

 private:
  void initialize ();

  void MarkSerializableDataMembers();

  wxRealPoint nodeSize;
  /*! \brief Auxiliary data member. */
  wxRealPoint m_nPrevSize;
  /*! \brief Auxiliary data member. */
  wxRealPoint m_nPrevPosition;
};

#endif// TRIGGERS_H
