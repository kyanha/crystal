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
#ifndef  GRAPH_H__
#define GRAPH_H__
#include <stdarg.h>

#include "wxsf/DiagramManager.h"

#include "csutil/array.h"

class GraphNodeFactory  ;
class GraphNode;
class GraphLink;
class GraphAnchor;
class GraphEditor;

class Graph : public wxSFDiagramManager
{
 public:
  virtual GraphNode* CreateNode (GraphNodeFactory* factory , GraphNode* parent = nullptr) = 0;
  virtual void RemoveNode (GraphNode* node) = 0;
  //virtual void ReparentNode (GraphNode* node, GraphNode* oldParent, GraphNode* newParent) = 0;
  virtual GraphLink* CreateLink () = 0;
  virtual GraphLink* CreateLink (GraphAnchor* source, GraphAnchor* target) = 0;
  virtual void RemoveLink (GraphLink* link) = 0;
    
 protected:
  GraphEditor* graphEditor;
  csArray<GraphNode *> nodeList; 
  csArray<GraphLink *> linkList; 
};
#endif
