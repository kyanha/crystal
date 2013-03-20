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
#ifndef GRAPHEDITOR_H__
#define GRAPHEDITOR_H__

#include <stdarg.h>
#include "graph.h"
#include "graphNodeFactory.h"

class Graph;
class GraphNodeFactory;

class GraphEditor
{
 protected:
  Graph* graph;
  csArray<GraphNodeFactory*> nodeFactoryList;

 public:
  GraphEditor ();
  // virtual ~GraphEditor();

  void AddNodeFactory (GraphNodeFactory* factory);
  size_t GetNodeFactoryCount () const;
  GraphNodeFactory* GetNodeFactory (size_t index) const;
        
  void SetGraph (Graph* graph);
  Graph* GetGraph () const;
};

#endif       
