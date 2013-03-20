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
#ifndef GRAPHANCHOR_H
#define GRAPHANCHOR_H

#include <stdarg.h>
#include <wx/wx.h>

#include "wxsf/ShapeDockpoint.h"

class GraphLink;
class GraphNode;

class GraphAnchor : public wxSFConnectionPoint 
{
 public:
  GraphAnchor ();
  GraphAnchor (GraphNode *node, GraphLink *link);
  GraphAnchor (GraphNode *node, GraphLink *link, const wxRealPoint relpos , long id);
  //virtual ~GraphAnchor();

  GraphNode* GetParentNode () const;
  void SetParentNode (GraphNode* node);

  bool IsSource () const;
  bool IsTarget () const;

  GraphLink* GetLink () const;
  void SetLink (GraphLink* link);

  GraphAnchor* GetLinkedAnchor () const;

 protected:
	
  GraphNode *node;  
  GraphLink *link;
};

#endif
