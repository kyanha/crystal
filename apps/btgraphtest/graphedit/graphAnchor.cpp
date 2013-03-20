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
#include "graphAnchor.h"
#include "graphNode.h"
#include "graphLink.h"

GraphAnchor::GraphAnchor () : wxSFConnectionPoint ()
{
  node = NULL;
  link = NULL;
}

GraphAnchor::GraphAnchor (GraphNode *node, GraphLink *link)
  : wxSFConnectionPoint (), node(node), link(link)
{
}
GraphAnchor::GraphAnchor (GraphNode *node, GraphLink *link, const wxRealPoint relpos , long id)
  : wxSFConnectionPoint (node, relpos, id ), node(node), link(link)
{
}

GraphNode* GraphAnchor::GetParentNode () const
{
  return node;
}
         
void GraphAnchor::SetParentNode (GraphNode* node)
{
  this->SetParentShape (node);
}

bool GraphAnchor::IsSource () const
{ 
  GraphAnchor *Anchor = link->GetSourceAnchor ();
  if (Anchor == this) 
    return true;
  else 
    return false;
}

bool GraphAnchor::IsTarget () const
{
  GraphAnchor *anchor = link->GetTargetAnchor ();
  if (anchor == this) 
    return true;
  else 
    return false;
}

GraphLink* GraphAnchor::GetLink () const
{
  return link;
}
    
void GraphAnchor::SetLink (GraphLink* link)
{
  this->link = link;
}

GraphAnchor* GraphAnchor::GetLinkedAnchor () const
{ 
  if(IsSource ()) 
    return link->GetTargetAnchor ();
  else 
    return link->GetSourceAnchor ();
}
