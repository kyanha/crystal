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
#include "graphEditor.h"

GraphEditor::GraphEditor ()
{
}

void GraphEditor::AddNodeFactory (GraphNodeFactory* factory)
{
	nodeFactoryList.Push (factory);
}

size_t GraphEditor::GetNodeFactoryCount () const
{
	return nodeFactoryList.GetSize ();
}

GraphNodeFactory* GraphEditor::GetNodeFactory (size_t index) const
{
	return nodeFactoryList.Get (index);
}

Graph* GraphEditor::GetGraph () const
{
  return graph;
}

void GraphEditor::SetGraph (Graph* graph) 
{
   this->graph = graph;
}
