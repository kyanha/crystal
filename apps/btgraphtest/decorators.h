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
#ifndef __DECORATORNODE_H__
#define __DECORATORNODE_H__

#include <wx/wx.h>
#include <wx/dc.h>

#include "graphedit/wxsf/wxShapeFramework.h"

#include "graphedit/graphNode.h"
#include "graphedit/graphNodeFactory.h"

class DecoratorNode : public GraphNode
{
 public:
 DecoratorNode ();
 DecoratorNode (GraphNodeFactory* factory);
 ~DecoratorNode ();

// public virtual functions
virtual void SetParameters ();
void UpdateParameter (size_t index, csVariant* oldValue, csVariant* newValue);

/*!
 * \brief Test whether the given point is inside the shape. The function
* can be overrided if neccessary.
* \param pos Examined point
* \return TRUE if the point is inside the shape area, otherwise FALSE
*/
 virtual bool Contains(const wxPoint& pos);
/*!
 * \brief Get intersection point of the shape border and a line leading from
 * 'start' point to 'end' point. The function can be overrided if neccessary.
 * \param start Starting point of the virtual intersection line
* \param end Ending point of the virtual intersection line
 * \return Intersection point
 */

 virtual wxRect GetBoundingBox();
/*!
 * \brief Get intersection point of the shape border and a line leading from
 * 'start' point to 'end' point. The function can be overrided if neccessary.
 * \param start Starting point of the virtual intersection line
* \param end Ending point of the virtual intersection line
 * \return Intersection point
 */
 virtual wxRealPoint GetBorderPoint(const wxRealPoint& start, const wxRealPoint& end);

/*!
 * \brief Function called by the framework responsible for creation of shape handles
* at the creation time. The function can be overrided if neccesary.
*/
 virtual void CreateHandles();
/*!
 * \brief Event handler called during dragging of the shape handle.
 * The function can be overrided if necessary.
 *
 * The function is called by the framework (by the shape canvas).
 * \param handle Reference to dragged handle
 */
 virtual void OnHandle(wxSFShapeHandle& handle);
/*!
 * \brief Event handler called when the user started to drag the shape handle.
 * The function can be overrided if necessary.
 *
 * The function is called by the framework (by the shape canvas).
 * \param handle Reference to dragged handle
 */
 virtual void OnBeginHandle(wxSFShapeHandle& handle);

/*! \brief Resize the shape to bound all child shapes. The function can be overrided if neccessary. */
 virtual void FitToChildren();
/*!
 * \brief Scale the shape size by in both directions. The function can be overrided if necessary
* (new implementation should call default one ore scale shape's children manualy if neccesary).
* \param x Horizontal scale factor
* \param y Vertical scale factor
* \param children TRUE if the shape's children shoould be scaled as well, otherwise the shape will be updated after scaling via Update() function.
*/
 virtual void Scale(double x, double y, bool children = sfWITHCHILDREN);

// public data accessors
/*!
 * \brief Set rectangle's fill style.
 * \param brush Refernce to a brush object
 */
void SetFill(const wxBrush& brush){this->brush  = brush;}
/*!
 * \brief Get current fill style.
 * \return Current brush
 */
wxBrush GetFill() const {return brush;}
/*!
 * \brief Set rectangle's border style.
 * \param pen Reference to a pen object
 */
 void SetBorder(const wxPen& pen){this->pen = pen;}
/*!
 * \brief Get current border style.
 * \return Current pen
 */
wxPen GetBorder() const {return pen;}
/*!
 * \brief Set the rectangle size.
 * \param size New size
 */
 void SetRectSize(const wxRealPoint& size){nodeSize = size;}
/*!
 * \brief Set the rectangle size.
 * \param x Horizontal size
 * \param y Verical size
 */
 void SetRectSize(double x, double y){nodeSize.x = x; nodeSize.y = y;}
/*!
 * \brief Get the rectangle size.
 * \return Current size
 */
 wxRealPoint GetRectSize() const {return nodeSize;}
	// public member data accessors
	/*!
	 * \brief Set corner radius.
	 * \param radius New corner radius
	 */
 void SetRadius(double radius){m_nRadius = radius;}
	/*!
	 * \brief Get current corner radius.
	 * \return Current corner radius
	 */
 double GetRadius(){return m_nRadius;}


    protected:
    // protected virtual functions
/*!
 * \brief Draw the shape in the normal way. The function can be overrided if neccessary.
 * \param dc Reference to device context where the shape will be drawn to
 */
   virtual void DrawNormal(wxDC& dc);
/*!
 * \brief Draw the shape in the hower mode (the mouse cursor is above the shape).
 * The function can be overrided if neccessary.
 * \param dc Reference to device context where the shape will be drawn to
 */
   virtual void DrawHover(wxDC& dc);
/*!
 * \brief Draw the shape in the highlighted mode (another shape is dragged over this
 * shape and this shape will accept the dragged one if it will be dropped on it).
 * The function can be overrided if neccessary.
 * \param dc Reference to device context where the shape will be drawn to
 */
  virtual void DrawHighlighted(wxDC& dc);
/*!
 * \brief Draw shadow under the shape. The function can be overrided if neccessary.
 * \param dc Reference to device context where the shadow will be drawn to
 */
  virtual void DrawShadow(wxDC& dc);
/*!
* \brief Event handler called during dragging of the right shape handle.
 * The function can be overrided if neccessary.
 * \param handle Reference to dragged shape handle
 */
 virtual void OnRightHandle(wxSFShapeHandle& handle);
/*!
* \brief Event handler called during dragging of the left shape handle.
 * The function can be overrided if neccessary.
 * \param handle Reference to dragged shape handle
 */
 virtual void OnLeftHandle(wxSFShapeHandle& handle);
/*!
* \brief Event handler called during dragging of the top shape handle.
 * The function can be overrided if neccessary.
 * \param handle Reference to dragged shape handle
 */
 virtual void OnTopHandle(wxSFShapeHandle& handle);
/*!
* \brief Event handler called during dragging of the bottom shape handle.
 * The function can be overrided if neccessary.
 * \param handle Reference to dragged shape handle
 */
 virtual void OnBottomHandle(wxSFShapeHandle& handle);
	// protected functions
	/*!
     * \brief Auxiliary function. Checks whether the point is inside a circle with given center. The circle's radius
	 * is the roundrect corner radius.
	 * \param pos Examined point
	 * \param center Circle center
	 */
	bool IsInCircle(const wxPoint& pos, const wxPoint& center);

	// protected data members
	/*! \brief Corner radius. */
	double m_nRadius;

 private:
  void initialize ();
  void MarkSerializableDataMembers();
  
  wxRealPoint nodeSize;
  /*! \brief Auxiliary data member. */
  wxRealPoint m_nPrevSize;
  /*! \brief Auxiliary data member. */
  wxRealPoint m_nPrevPosition;
};

#endif// SELECTORNODE_H
