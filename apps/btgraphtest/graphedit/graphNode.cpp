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
#define CS_IMPLEMENT_PLATFORM_APPLICATION
/* This is needed due the WX headers using free() inline, but the opposing
 * malloc() is in the WX libs. */
#define CS_NO_MALLOC_OVERRIDE

#include "cssysdef.h"
#include "graph.h"
#include "graphAnchor.h"
#include "graphNode.h"
#include "graphNodeFactory.h"

GraphNode::GraphNode (): wxSFShapeBase () {}

GraphNode::GraphNode (GraphNodeFactory* factory)
  : wxSFShapeBase (), factory (factory)
{
  SetParameters ();
}

void GraphNode::SetFactory (GraphNodeFactory* factory)
{
 this->factory = factory;
}

GraphNodeFactory* GraphNode::GetFactory () const
{
  return factory;
}

void GraphNode::SetName (const char* name)
{
  this->name = name;
}

const char* GraphNode::GetName () const
{
  return name;
}

 /*size_t GraphNode::GetAnchorCount () const
 {
  return anchorList.GetSize ();
 }

GraphAnchor* GraphNode::AddAnchor ()
{
  GraphAnchor* anchor = new GraphAnchor ();
  //anchor->SetNode(this);
  anchorList.Push (*anchor);
  this->AddConnectionPoint (anchor, true);
  return anchor;
}*/


  GraphAnchor* GraphNode::AddAnchor (const csVector2& relpos)
  {
   GraphAnchor* anchor = new GraphAnchor ();
   anchor->SetRelativePosition (wxRealPoint (relpos.x, relpos.y));
   anchor->SetParentNode (this);
   AddConnectionPoint (anchor, true);
   anchor->Refresh ();
   return anchor;
  }

 /* GraphAnchor* GraphNode::GetAnchor (size_t index) const
  { 
   GraphAnchor *anchor = &anchorList.Get(index);
    return anchor;
  }*/

void GraphNode::SetParameters ()
{
  csVariant variant;
  for (size_t i = 0; i < factory->GetParameterCount (); i++)
    parameters.Push (variant);
}

csVariant* GraphNode::GetParameter (size_t index)
{
  csVariant* var  = &parameters.Get (index);
  return var;
}

void GraphNode::DrawNormal (wxDC& dc)
{
 // HINT: overload it for custom actions...
}

void GraphNode::DrawHover (wxDC& dc)
{	
// HINT: overload it for custom actions...
}

void GraphNode::DrawHighlighted (wxDC& dc)
{
// HINT: overload it for custom actions...
}

void GraphNode::DrawShadow (wxDC& dc)
{
// HINT: overload it for custom actions...
}
