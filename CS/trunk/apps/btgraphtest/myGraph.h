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
#ifndef BTGRAPH_H
#define BTGRAPH_H


#include "cssysdef.h"

/*#include <crystalspace.h>
#include <csgeom/vector2.h>*/

//#include "iutil/pluginconfig.h"
#include "graphedit/pluginconfig.h"

#include <wx/wxprec.h>
#include <wx/wx.h>

#include <wx/textdlg.h>
#include <wx/filename.h>
#include "graphedit/graphNode.h"
#include "graphedit/graphNodeFactory.h"
#include "graphedit/graphLink.h"
#include "graphedit/graphAnchor.h"
#include "graphedit/graph.h"

#include "graphedit/wxsf/wxShapeFramework.h"
#include "graphedit/wxsf/DiagramManager.h"


#include "btnode.h"
#include "btaction.h"
#include "selectors.h"
#include "triggers.h"
#include "decorators.h"

class MyGraph : public Graph
{
 public:
  //MyGraph () {}
	
  virtual void RemoveNode (GraphNode* node)
  {
    //this->RemoveShape (node);
    nodeList.Delete (node);
  }

  //virtual void ReparentNode (GraphNode* node, GraphNode* oldParent, GraphNode* newParent) {}

  virtual GraphLink* CreateLink ()
  {
    GraphLink* link = new GraphLink ();
    if (link)
      linkList.Push (link);
    return link;
  }

  virtual GraphLink* CreateLink (GraphAnchor* source, GraphAnchor* target)
  {
    GraphLink* link = new GraphLink (source,target, this);
    if (link)
      linkList.Push (link);
    return link;
  }
	
  virtual void RemoveLink (GraphLink* link)
  {
   // this->RemoveShape (link);
    linkList.Delete (link);
  }

 virtual GraphNode* CreateNode (GraphNodeFactory* factory, GraphNode* parent = nullptr)
  { 
   GraphNode *node;
   switch (factory->GetId ())
   {
     case IDT_ROOT: 
              node = new BTNode (factory);  
              break;
     case  IDT_DEFAULT:
     case  IDT_RANDOM: 
     case  IDT_SEQUENTIAL: 
              node = new SelectorNode (factory);  
              break; 
     case IDT_LOOP : 
     case IDT_NEGATERETURN:  
     case IDT_EXE_LIMIT: 
              node = new DecoratorNode (factory);  
              break; 
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
              node = new TriggerNode (factory);  
              break; 
     case IDT_ACTION:
             node = new BTAction (factory);
   }
     node->SetParent ((xsSerializable*)parent);
     node->SetId (nodeList.GetSize ());
     nodeList.Push (node);
     return node;
  }
	
  GraphNode* GetNode (size_t index) const
  {
    return nodeList.Get (index);
  }
	
  GraphLink* GetLink (size_t index) const
  {
    return linkList.Get (index);
  }
	
  size_t  GetNodeCount () const
  {
    return nodeList.GetSize ();
  }
	
  size_t  GetLinkCount () const
  {
    return linkList.GetSize ();
  }

};

#endif// BTGRAPH_H
