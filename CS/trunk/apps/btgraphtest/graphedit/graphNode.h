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
#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <stdarg.h>

#include "csgeom/vector2.h"
#include "csutil/csstring.h"
//#include "iutil/pluginconfig.h"
#include "pluginconfig.h"

#include <wx/dc.h>

#include "wxsf/ShapeBase.h"

class csVariant;
class Graph;
class GraphAnchor;
class GraphNodeFactory;

class GraphNode : public wxSFShapeBase
{
 public:

  GraphNode ();
  GraphNode (GraphNodeFactory* factory);
		
  void SetName (const char*name);
  const char* GetName () const;

  void SetFactory (GraphNodeFactory* factory);		
  GraphNodeFactory* GetFactory () const ;
         
  //size_t GetAnchorCount () const;
  //GraphAnchor* AddAnchor () ;
  // TODO: remettre celle d'en dessous
  GraphAnchor* AddAnchor (const csVector2& relpos);
  //GraphAnchor const* GetAnchor (size_t index) const;
       
  virtual void SetParameters ();

  csVariant* GetParameter (size_t index);
     
  virtual void UpdateParameter (size_t index, csVariant* oldValue, csVariant* newValue) = 0;
		
 protected:
// protected virtual functions to overload for custom actions...
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

  csString name;
  // the anchor list
  //csArray<GraphAnchor> anchorList;
		
  csArray<csVariant> parameters;
		
  GraphNodeFactory* factory; 

  wxPen pen;
  wxBrush brush;

};

#endif
