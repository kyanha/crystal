/*
  Crystal Space Windowing System - Event subscription internals
  Copyright (C) 2005 by Adam D. Bradley <artdodge@cs.bu.edu>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "cssysdef.h"
#include "csutil/csevent.h"
#include "csutil/cssubscription.h"
#include "csutil/eventnames.h"
#include "iutil/eventh.h"
#include "csutil/cseventq.h"
#include "csutil/partialorder.h"
#include "ivaria/reporter.h"

#ifdef ADB_DEBUG /* debugging output... */
#include <iostream>
#endif


csEventTree::csEventTree (csRef<iEventHandlerRegistry> &h_reg,
			  csRef<iEventNameRegistry> &n_reg, 
			  csEventID name, csEventTree *_parent, csEventQueue *q) :
  csTreeNode (_parent), 
  handler_reg (h_reg), name_reg (n_reg), 
  self (name), queue (q)
{
  CS_ASSERT(name != CS_EVENT_INVALID);
  if (parent) 
  {
    fatRecord = ((csEventTree *)parent)->fatRecord; // I wish csEventTree was polymorphic.
    fatNode = false;
  } 
  else 
  {
    /* this is the root node.
     * create the root PO and Queue so everyone else can COW it. */
    fatRecord = new FatRecordObject(handler_reg, name_reg, 
				    new csPartialOrder<csHandlerID>, 0);
    fatNode = true;
  }
  queue->EventHash.PutUnique(name, this);
}

csEventTree::~csEventTree() 
{
  queue->EventHash.DeleteAll(self);
}

/**
 * Use head-recursion to find the path.
 */
csEventTree *csEventTree::FindNode(csEventID name, csEventQueue *q)
{
  CS_ASSERT(name != CS_EVENT_INVALID);
  csEventTree *res;
  if ((res=q->EventHash.Get(name, NULL))!=NULL) 
  { /* shortcut */
    return res;
  }
  if (!q->EventTree) 
  {
    csEventID root = csevAllEvents(name_reg);
    q->EventTree = new csEventTree(handler_reg, name_reg, root, NULL, q);
    q->EventHash.PutUnique(root, q->EventTree);
  }
  return q->EventTree->FindNodeInternal(name, q);
}

csEventTree *csEventTree::FindNodeInternal(csEventID &name, csEventQueue *q)
{
  CS_ASSERT(name != CS_EVENT_INVALID);
  if (name == self)
    return this;
  else 
  {
    csEventID parentID = name_reg->GetParentID(name);
    CS_ASSERT(parentID != CS_EVENT_INVALID);
    csEventTree *wrk_parent = FindNodeInternal(parentID, q);
    for (size_t iter=0 ; iter<wrk_parent->children.Length() ; iter++) 
    {
      if (((csEventTree *)wrk_parent->children[iter])->self == name)
	return (csEventTree *) wrk_parent->children[iter];
    }
    csEventTree *added = new csEventTree(handler_reg, name_reg, name, wrk_parent, q);
    return added;
  }
}

void csEventTree::PushFatCopy (FatRecordObject *r)
{
  if (!fatNode) 
  {
    fatRecord = r;
    for (size_t i=0 ; i<children.Length() ; i++) 
    {
      ((csEventTree *)children[i])->PushFatCopy(r);
    }
  }
}

void csEventTree::ForceFatCopy ()
{
  if (!fatNode)
  {
#ifdef ADB_DEBUG
    std::cerr << "  ... forcing fat copy here ..." << std::endl;
#endif

    FatRecordObject *newFatRecord = new FatRecordObject
      (handler_reg, name_reg,
       new csPartialOrder<csHandlerID>(fatRecord->SubscriberGraph),
       (fatRecord->SubscriberQueue?
	new csList<iEventHandler *>(*fatRecord->SubscriberQueue):
	0));
    
    // If there's a deletedList, our SQ will always need regeneration,
    // no matter which of the above cases happens.
    newFatRecord->StaleSubscriberQueue = fatRecord->StaleSubscriberQueue || (fatRecord->deletedList!=0);

    if ((fatRecord->iterator != 0) && 
	name_reg->IsKindOf(fatRecord->iterator->baseevent, self)) {
      // The iterator actually should belong to us and not to the parent fatRecord.
      CS_ASSERT(false);
      newFatRecord->iterator = fatRecord->iterator; // DOME : this has to be translated !
      fatRecord->iterator = 0;
      newFatRecord->deletedList = fatRecord->deletedList; // DOME : ditto
      fatRecord->deletedList = 0;

      // If the deletedList belongs to us, we need to regenerate the SQ we swiped it from.
      // DOME ...

    } else {
      // There is no iterator, or it belongs to the parent.  Ignore it.
      newFatRecord->iterator = 0;
      newFatRecord->deletedList = 0;
    }

    PushFatCopy(newFatRecord);
    fatNode = true;
  }
}

void csEventTree::KillFatCopy()
{
  CS_ASSERT(fatRecord->iterator == 0);
  CS_ASSERT(fatRecord->deletedList == 0);
  if (fatNode)
  {
    delete fatRecord;
    PushFatCopy(((csEventTree*)parent)->fatRecord);
    fatNode = false;
  }
}

void csEventTree::FatRecordObject::RebuildQueue()
{
  csList<const csHandlerID> hlist;
  SubscriberGraph->Solve(hlist);
  csList<const csHandlerID>::Iterator it(hlist);
  if (SubscriberQueue)
    SubscriberQueue->DeleteAll();
  else
    SubscriberQueue = new csList<iEventHandler*>;
  while (it.HasNext()) 
  {
    csHandlerID h = it.Next();
    if (handler_reg->IsInstance(h))
      SubscriberQueue->PushBack(handler_reg->GetHandler(h));
  }
  StaleSubscriberQueue = false;
}

csPartialOrder<csHandlerID> *csEventTree::FatRecordObject::SubscribeInternal(csHandlerID id, csEventID baseevent)
{
  csPartialOrder<csHandlerID> *NewSubscriberGraph;
  iEventHandler *handler = handler_reg->GetHandler(id);
  /* 
   * Build a copy of the graph and see if the new subscription "fits".
   * If it does, and if this also succeeds for all of the children,
   * we will replace the current graph with the new one below.
   * If not, we simply discard the new graph instead of trying to
   * mess with "backing out" our changes.
   *
   * We will solve the PO below once we know our children have
   * succeeded.  This conserves work - no delivery_queue is rebuilt
   * until we know that the subscription succeeds completely. 
   *
   * Also, if the current event name is an in-delivery event name,
   * we switch the event queue iterator into graph-solver mode,
   * and we do not regenerate the subscriber queue until the iterator
   * has finished its pass.
   */
  NewSubscriberGraph = new csPartialOrder<csHandlerID>(SubscriberGraph);
  
  CS_ASSERT(id != CS_HANDLER_INVALID);
  NewSubscriberGraph->Add (id); /* ensure this node is present */
  
  /* id is an actual event handler, so we must sandwich it between
   * the abstract handler name's magic ":pre" and ":post" instances
   * to make sure abstract ordering works. */
  
  csHandlerID preBound = handler_reg->GetGenericPreBoundID(id);
  csHandlerID postBound = handler_reg->GetGenericPostBoundID(id);

  CS_ASSERT(preBound != CS_HANDLER_INVALID);
  NewSubscriberGraph->Add (preBound);
  if (!NewSubscriberGraph->AddOrder (preBound, id)) 
    goto fail; /* This edge introduced a cycle... possible if generic(id) 
		* is already ruled out by conflicting generic constraints. */

#ifdef ADB_DEBUG
  NewSubscriberGraph->Dump ();
#endif
  
  CS_ASSERT(postBound != CS_HANDLER_INVALID);
  NewSubscriberGraph->Add(postBound);
  if (!NewSubscriberGraph->AddOrder(id, postBound))
    goto fail; /* Ditto */

#ifdef ADB_DEBUG  
  NewSubscriberGraph->Dump ();
#endif

  /* If this is an in-delivery event name, then if the event is
     still eligible to execute (subject to all constraints) then
     we will let it, otherwise we mark it, so it will not get 
     executed until a new instance of the event gets dispatched. 
     Note that this is not deterministic unless your order is
     specifically with request to the currentl in-service handler,
     since PO solutions are not fully deterministic. */
  if ((iterator) && (NewSubscriberGraph->IsMarked(postBound)))
    NewSubscriberGraph->Mark(id);

  do {
    const csHandlerID *precs = handler->InstancePrec(handler_reg, name_reg, baseevent);
    if (precs != 0)
    {
      for (size_t i=0 ; precs[i]!=CS_HANDLERLIST_END ; i++)
      {
	csHandlerID prec = precs[i];
	/* This is a little subtle.  If the predecessor ID is a generic
	 * handler name, we actually want to mark that ID's ":post" 
	 * magic instance as our predecessor; since all instances 
	 * will be predecessors of this (see above), we get the desired 
	 * effect. */
	if (!handler_reg->IsInstance(prec))
	{
	  prec = handler_reg->GetGenericPostBoundID(prec);
	}
	NewSubscriberGraph->Add (prec);
	if (!NewSubscriberGraph->AddOrder (prec, id))
	  goto fail; /* This edge introduced a cycle */
	
#ifdef ADB_DEBUG      
	NewSubscriberGraph->Dump ();
#endif
      }
    }
  } while (0);
  
  do {
    const csHandlerID *succs = handler->InstanceSucc(handler_reg, name_reg, baseevent);
    if (succs != 0)
    {
      for (size_t i=0 ; succs[i]!=CS_HANDLERLIST_END ; i++)
      {
	csHandlerID succ = succs[i];
	/* Same rationale as above. */
	if (!handler_reg->IsInstance(succ))
	  succ = handler_reg->GetGenericPreBoundID(succ);
	if (iterator && (NewSubscriberGraph->IsMarked(succ))) 
	{
	  /* See the above comment about in-delivery event names */
	  NewSubscriberGraph->Mark(id);
	  break;
	}
	NewSubscriberGraph->Add(succ);
	if (!NewSubscriberGraph->AddOrder(id, succ))
	  goto fail; /* This edge introduced a cycle */
	
#ifdef ADB_DEBUG
	NewSubscriberGraph->Dump ();
#endif      
      }
    }
  } while (0);
  
  return NewSubscriberGraph;
  
 fail:
  if (NewSubscriberGraph)
    delete NewSubscriberGraph;
  return 0;
}

bool csEventTree::SubscribeInternal (csHandlerID id, csEventID baseevent)
{
  CS_ASSERT(baseevent != CS_EVENT_INVALID);
  CS_ASSERT(id != CS_HANDLER_INVALID);
  csPartialOrder<csHandlerID> *NewSubscriberGraph = 
    fatNode ? fatRecord->SubscribeInternal(id, baseevent) : 0;

  /**
   * Descend the event name tree, looking for fat copies needing subscriptions.
   * Note that, in the common case, this will amount to a no-op true.
   * The scheme only becomes expensive when there are subscribers at a lot of
   * subtrees of the graft point (i.e., a lot of fat copies).
   */
  for (size_t i=0 ; i<children.Length() ; i++) 
  {
    if (!((csEventTree *)children[i])->SubscribeInternal (id, baseevent))
      goto fail;
  }

  // Success!  Push the changes into the live event tree.
  if (NewSubscriberGraph) 
  {
    // Install new partial order
    *fatRecord->SubscriberGraph = *NewSubscriberGraph;
    delete NewSubscriberGraph;

#ifdef ADB_DEBUG
    fatRecord->SubscriberGraph->Dump();
#endif

    fatRecord->StaleSubscriberQueue = true;

    if (fatRecord->iterator) 
    {
      fatRecord->iterator->GraphMode();
    } 

    /* We do not re-build the queue now.  Instead, we do it on-demand,
     * in the hopes of "batching up" subscribes and unsubscribes. */
  }

  return true;

 fail: 
  /* We either failed locally or on a subtree.  
   * Don't change the live tree. */
  if (NewSubscriberGraph)
    delete NewSubscriberGraph;
  return false;
}

void csEventTree::FatRecordObject::UnsubscribeInternal(csHandlerID id, csEventID baseevent)
{
  iEventHandler *h = handler_reg->GetHandler(id);
  /* It is possible we've been called for a "universal unsubscribe"
   * (baseevent==CS_EVENT_INVALID), so it could be there's nothing
   * to remove from this event node because the handler didn't
   * subscribe here. */
  if (SubscriberGraph->Contains(id)) 
  {
    /* Automagically removes all of the edges for us.
     * Dangling edges are a non-issue, since they go away
     * and their deleted endpoint will never appear again
     * (instance IDs are non-repeating). */
    SubscriberGraph->Delete(id);
    
    /* Remove from the delivery queue without unnecessary SubscriberGraph->Solve() */
    if ((SubscriberQueue != 0) && (iterator == 0)) {
      csList<iEventHandler *>::Iterator it(*SubscriberQueue);
      while (it.HasNext()) 
      {
	if (it.Next() == h) 
	{
	  SubscriberQueue->Delete(it);
	  break;
	}
      }
    }
    else
    {
      /* It's not safe to delete out from under a live iterator. */
      StaleSubscriberQueue = true;
      
      /* Don't regenerate right away, there may be more to come.
       * We do lazy re-evaluation, i.e., only when the next
       * event is dispatched to this target (see Dispatch()). */
      if (!deletedList)
	deletedList = new csList<csHandlerID>;
      deletedList->PushBack(id);
    }
  }
}

/*
 * This has to be "reentrant", by which I simply mean it must be safe for
 * an event handler to un-subscribe itself while it is being delivered to.
 * If we are in graph mode, this is done automagically for us; the PO
 * implementation doesn't use pointer references, so simply removing a node
 * (whether marked or not) doesn't endanger us.  In the SQ case, however,
 * we have to be careful, since we can't delete nodes out from under the 
 * SQ iterator.  Instead, we delete them from the graph (as usual), mark 
 * them in deletedList (so the iterator can make sure not to hit them),
 * and signal a regenerate for when the iteration is done.
 */
void csEventTree::UnsubscribeInternal(csHandlerID id, csEventID baseevent)
{
  CS_ASSERT(id != CS_HANDLER_INVALID);
  if (fatNode)
  {
    fatRecord->UnsubscribeInternal(id, baseevent);
  }

  /* Since we manipulate fatRecord in place, we only really need
   * to propagate to (fatNode) children.  
   * Descend recursively and look for them. */

  for (size_t i=0 ; i<children.Length() ; i++) 
  {
    ((csEventTree *)children[i])->UnsubscribeInternal(id, baseevent);
  }
}

bool csEventTree::Subscribe (csHandlerID id, csEventID event, csEventQueue *q)
{
#ifdef ADB_DEBUG
  std::cerr << __func__ << " : " 
	    << handler_reg->GetString(id)
	    << "/" << name_reg->GetString(event) << std::endl;
#endif

  CS_ASSERT(id != CS_HANDLER_INVALID);
  CS_ASSERT(event != CS_EVENT_INVALID);
  csEventTree *w = FindNode(event, q);
  bool wasFat = w->fatNode;
  w->ForceFatCopy ();
  if (!w->SubscribeInternal (id, event)) 
  {
    if (!wasFat) {
#ifdef ADB_DEBUG
      std::cerr << __func__ << " (killing fat copy)" << std::endl;
#endif
      w->KillFatCopy();
    }
#ifdef ADB_DEBUG
    std::cerr << __func__ << " FAILED" << std::endl;
#endif
    return false;
  } 
  else 
  {
#ifdef ADB_DEBUG
    std::cerr << __func__ << " SUCCEEDED" << std::endl;
#endif
    return true;
  }
}

void csEventTree::Unsubscribe(csHandlerID id, csEventID event, csEventQueue *q)
{
  CS_ASSERT(id != CS_HANDLER_INVALID);
  if (event == CS_EVENT_INVALID)
    q->EventTree->UnsubscribeInternal(id, event);
  else 
  {
    csEventTree *w = FindNode(event, q);
    w->ForceFatCopy ();
    w->UnsubscribeInternal (id, event);
    // TODO : test if UnsubscribeInternal fails (i.e., is a no-op); 
    // if it turns out we created a fat copy unnecessarily, kill it.
  }
}

void csEventTree::Notify ()
{
  csRef<iEvent> e(queue->CreateEvent(self));
  e->Broadcast = true;
  Dispatch(*e);
}

void csEventTree::Dispatch (iEvent &e)
{
  CS_ASSERT(e.Name == self);

  if (fatRecord->StaleSubscriberQueue)
    fatRecord->RebuildQueue();

  /* automatic variable will be destroyed for us... */
  SubscriberIterator it(handler_reg, this, e.Name); 

#ifdef ADB_DEBUG
  std::cerr << "DISPATCH [" 
	    << name_reg->GetString(e.Name) 
	    << "] on event node "
	    << name_reg->GetString(self)
	    << std::endl;
#endif

  while (it.HasNext()) 
  {
    iEventHandler *h = it.Next();
    if (fatRecord->deletedList) {
      csList<csHandlerID>::Iterator dit(*fatRecord->deletedList);
      while (dit.HasNext()) {
	if (dit.Next() == handler_reg->GetID(h))
	  continue;
      }
    }
    CS_ASSERT(h != 0);
#ifdef ADB_DEBUG
    std::cerr << " -- dispatching to "
	      << h->GenericName()
	      << std::endl;
#endif
    if (h->HandleEvent(e) && (!e.Broadcast))
      break;
  }
#ifdef ADB_DEBUG
  if (it.HasNext()) {
    std::cerr << "  SKIPPING:" << std::endl;
    do {
      std::cerr << "    " << it.Next()->GenericName() << std::endl;
    } while (it.HasNext());
  }
  std::cerr << "END DISPATCH"
	    << std::endl;
#endif
}






void csEventTree::SubscriberIterator::GraphMode()
{
  if (mode ==SI_GRAPH)
    return;
  CS_ASSERT(mode==SI_LIST);

  record->SubscriberGraph->ClearMark();

  csList<iEventHandler *>::Iterator zit(*record->SubscriberQueue);
  while (zit.HasNext()) 
  {
    /**
     * This loop marks every node in the graph that the queue iterator (qit)
     * has already visited.  From here forward, we do everything on the graph
     * itself and do not look at the SubscriberQueue.
     */
    iEventHandler *h = zit.Next();
    csHandlerID hid = handler_reg->GetID(h);
    record->SubscriberGraph->Mark(hid);
    if (h == qit.FetchCurrent())
      break;
  }

  mode = SI_GRAPH;
}






#ifdef ADB_DEBUG
void Indent (int n)
{
  for (int i=0 ; i<n ; i++)
  {
    std::cerr << "  ";
  }
}

void csEventTree::Dump ()
{
  std::cerr << "-------------------------------------------------------------" 
	    << std::endl << "Event tree:" << std::endl;
  queue->EventTree->Dump (0);
  std::cerr << "-------------------------------------------------------------"
	    << std::endl;
}

void csEventTree::Dump (int depth)
{
  Indent(depth);
  std::cerr << "Node: [" << name_reg->GetString(self) << "]" << std::endl;
  if ((fatRecord->iterator) && (fatRecord->iterator->mode != SubscriberIterator::SI_LIST)) 
  {
    Indent(depth+3);
    std::cerr << "NOTE - Node is in graph iterator mode, this is probably wrong" << std::endl;
  }

  if (fatRecord->SubscriberGraph && (fatRecord->SubscriberGraph->Size()>0)) {
    Indent(depth+1);
    std::cerr << "Subscribers:" << std::endl;
    for (size_t i=0 ; i<fatRecord->SubscriberGraph->Size() ; i++) {
      csHandlerID hid = fatRecord->SubscriberGraph->GetByIndex(i);
      iEventHandler *h = handler_reg->IsInstance(hid) ?
	handler_reg->GetHandler(hid) : 0;
      std::cerr << "[" << handler_reg->GetString(hid) << "] <"
		<< std::hex << ((unsigned long) h) << std::dec << ">"
		<< std::endl;
    }
  } else {
    Indent(depth+1);
    std::cerr << "No subscribers" << std::endl;
  }

  for (size_t i=0 ; i<children.Length() ; i++) 
  {
    ((csEventTree *) children[i])->Dump(depth+1);
  }
}
#endif

