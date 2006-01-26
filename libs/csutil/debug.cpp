/*
    Debugging tools.
    Copyright (C) 2001 by Jorrit Tyberghein

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
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "csutil/sysfunc.h"
#include "csutil/scf.h"
#include "csutil/util.h"
#include "csutil/debug.h"
#include "csutil/eventnames.h"
#include "iutil/objreg.h"

//-----------------------------------------------------------------------------

struct csDGEL;

// A link element (parent->child or child->parent)
struct csDGELLinkEl
{
  csDGEL* link;		// Child or parent linked too.
  uint32 timestamp;	// Time of creation of link.
};

struct csDGEL
{
  void* object;		// Pointer to the object.
  uint32 timestamp;	// Timestamp of last allocation.
  uint8 scf;		// If true 'object' is an iBase.
  uint8 used;		// If true the object is currently allocated.
  uint8 marker;		// To see what we already dumped.
  uint8 recurse_marker;	// To see what we're dumping in this recursion.
  uint16 num_parents;
  uint16 num_children;
  csDGELLinkEl* parents;
  csDGELLinkEl* children;
  char* description;
  char* type;
  char* file;
  int linenr;

  csDGEL ()
  {
    object = 0;
    scf = false;
    used = false;
    timestamp = 0;
    description = 0;
    type = 0;
    file = 0;
    num_parents = 0;
    parents = 0;
    num_children = 0;
    children = 0;
  }
  void Clear ()
  {
    delete[] description; description = 0;
    delete[] type; type = 0;
    delete[] parents; parents = 0; num_parents = 0;
    delete[] children; children = 0; num_children = 0;
    file = 0;
  }
  ~csDGEL ()
  {
    Clear ();
  }

  void AddChild (csDGEL* child, uint32 timestamp)
  {
    if (!children)
    {
      children = new csDGELLinkEl[1];
    }
    else
    {
      csDGELLinkEl* new_children = new csDGELLinkEl[num_children+1];
      memcpy (new_children, children, sizeof (csDGELLinkEl)*num_children);
      delete[] children; children = new_children;
    }
    children[num_children].link = child;
    children[num_children++].timestamp = timestamp;
  }
  void RemoveChild (csDGEL* child)
  {
    if (!children) return;
    if (num_children == 1)
    {
      if (child == children[0].link)
      {
        delete[] children; children = 0;
	num_children = 0;
      }
      return;
    }
    int i, j = 0;
    for (i = 0 ; i < num_children ; i++)
    {
      if (child != children[i].link) children[j++] = children[i];
    }
    num_children = j;
  }
  void AddParent (csDGEL* parent, uint32 timestamp)
  {
    if (!parents)
    {
      parents = new csDGELLinkEl[1];
    }
    else
    {
      csDGELLinkEl* new_parents = new csDGELLinkEl[num_parents+1];
      memcpy (new_parents, parents, sizeof (csDGELLinkEl)*num_parents);
      delete[] parents; parents = new_parents;
    }
    parents[num_parents].link = parent;
    parents[num_parents++].timestamp = timestamp;
  }
  void RemoveParent (csDGEL* parent)
  {
    if (!parents) return;
    if (num_parents == 1)
    {
      if (parent == parents[0].link)
      {
        delete[] parents; parents = 0;
	num_parents = 0;
      }
      return;
    }
    int i, j = 0;
    for (i = 0 ; i < num_parents ; i++)
    {
      if (parent != parents[i].link) parents[j++] = parents[i];
    }
    num_parents = j;
  }
};

struct iDebugGraph : public virtual iBase
{
  SCF_INTERFACE(iDebugGraph, 0, 0, 1);

  virtual void Clear () = 0;
  virtual csDGEL* FindEl (void* object) = 0;
  virtual csDGEL* AddEl (void* object) = 0;
  virtual int GetNumEls () = 0;
  virtual csDGEL** GetEls () = 0;
  virtual uint32 GetLastTimestamp() = 0;
};

class csDebugGraph : public scfImplementation1<csDebugGraph, iDebugGraph>
{
public:
  int num_els;
  int max_els;
  csDGEL** els;
  uint32 last_timestamp;

  csDebugGraph () : scfImplementationType(this)
  {
    num_els = 0;
    max_els = 100;
    els = new csDGEL* [max_els];
    last_timestamp = 1;
  }
  virtual ~csDebugGraph ()
  {
    Clear ();
    delete[] els;
  }
  void Clear ()
  {
    int i;
    for (i = 0 ; i < num_els ; i++)
    {
      delete els[i];
    }
    delete[] els;
    num_els = 0;
    max_els = 100;
    els = new csDGEL* [max_els];
    last_timestamp = 1;
  }

  csDGEL* AddEl (void* object)
  {
    if (num_els >= max_els)
    {
      max_els += 100;
      csDGEL** new_els = new csDGEL* [max_els];
      memcpy (new_els, els, sizeof (csDGEL*) * num_els);
      delete[] els;
      els = new_els;
    }

    csDGEL* el = new csDGEL ();
    els[num_els++] = el;
    el->used = false;
    el->object = object;
    return el;
  }

  csDGEL* FindEl (void* object)
  {
    int i;
    for (i = 0 ; i < num_els ; i++)
    {
      if (els[i]->object == object) return els[i];
    }
    return 0;
  }

  uint32 GetLastTimestamp() { return last_timestamp++; }
  int GetNumEls () { return num_els; }
  csDGEL** GetEls () { return els; }
};

static csRef<iDebugGraph> SetupDebugGraph (iObjectRegistry* object_reg)
{
  const char graphTag[] = "__Debug_Graph__";
  csRef<iBase> idg (csQueryRegistryTag (object_reg, graphTag));
  if (!idg)
  {
    idg = csPtr<iBase> (new csDebugGraph ());
    if (!object_reg->Register (idg, graphTag))
    {
      // If registering fails this probably means we are in the destruction
      // pass and the object registry doesn't allow new updates anymore.
      return 0;
    }
  }
  //@@FIX
  return scfQueryInterface<iDebugGraph> (idg);
//  return (csDebugGraph*)(iBase*)idg;	// DecRef() but that's ok in this case.
}

//-----------------------------------------------------------------------------

void csDebuggingGraph::SetupGraph (iObjectRegistry* object_reg)
{
  if (!SetupDebugGraph (object_reg)) return;
#ifdef CS_DEBUG
  iSCF::SCF->object_reg = object_reg;
#endif
}

void csDebuggingGraph::AddObject (iObjectRegistry* object_reg,
	void* object, bool scf, char* file, int linenr,
  	char* description, ...)
{
#ifdef CS_DEBUG
  if (!object_reg) object_reg = iSCF::SCF->object_reg;
#endif
  if (!object_reg) return;
  iDebugGraph* dg = SetupDebugGraph (object_reg);
  if (!dg) return;

  csDGEL* el = dg->FindEl (object);
  if (el)
  {
    // The element is already there. This either means that
    // the object was freed first and now a new object happens
    // to be allocated on the same position (this is a valid
    // situation), or else it means that the object is allocated
    // twice! This is not a valid situation because it means
    // that DG_ADD or DG_ADDI is used with a missing DG_REM
    // in between.
    if (el->used)
    {
      csPrintf ("ERROR! Object is added twice to the debug graph!\n");
      csPrintf ("%p %s", el->object, el->description);
      fflush (stdout);
      CS_ASSERT (false);
      return;
    }

    // Reinitialize the element. We will also clear the list of
    // parents and children here since this is a new element and the
    // previous lists are certainly invalid. Note that it is possible
    // that other elements still point to this element from a previous
    // incarnation. That case can be detected with the timestamp: timestamp
    // of this creation will be bigger than the timestamp of the creation
    // of the link to this item. The Dump will show this anomaly.
    el->Clear ();
  }
  else
  {
    // We have a new element.
    el = dg->AddEl (object);
  }

  el->used = true;
  el->timestamp = dg->GetLastTimestamp();
  el->scf = scf;

  if (description)
  {
    csString buf;
    va_list arg;
    va_start (arg, description);
    buf.FormatV (description, arg);
    va_end (arg);
    el->description = csStrNew (buf);
  }
  else el->description = 0;

  el->file = file;
  el->linenr = linenr;
}

void csDebuggingGraph::AttachDescription (iObjectRegistry* object_reg,
  	void* object, char* description, ...)
{
#ifdef CS_DEBUG
  if (!object_reg) object_reg = iSCF::SCF->object_reg;
#endif
  if (!object_reg) return;
  iDebugGraph* dg = SetupDebugGraph (object_reg);
  if (!dg) return;

  csDGEL* el = dg->FindEl (object);
  if (el == 0)
  {
    csPrintf ("ERROR! Cannot find object %p to add description:\n'", object);
    va_list arg;
    va_start (arg, description);
    csPrintfV (description, arg);
    va_end (arg);
    csPrintf ("'\n");
    fflush (stdout);
    CS_ASSERT (false);
    return;
  }

  delete[] el->description;
  if (description)
  {
    csString buf;
    va_list arg;
    va_start (arg, description);
    buf.FormatV (description, arg);
    va_end (arg);
    el->description = csStrNew (buf);
  }
  else el->description = 0;
}

void csDebuggingGraph::AttachType (iObjectRegistry* object_reg,
  	void* object, char* type)
{
#ifdef CS_DEBUG
  if (!object_reg) object_reg = iSCF::SCF->object_reg;
#endif
  if (!object_reg) return;
  iDebugGraph* dg = SetupDebugGraph (object_reg);
  if (!dg) return;

  csDGEL* el = dg->FindEl (object);
  if (el == 0)
  {
    csPrintf ("ERROR! Cannot find object %p to add type '%s'\n", object, type);
    fflush (stdout);
    CS_ASSERT (false);
    return;
  }

  delete[] el->type;
  if (type)
    el->type = csStrNew (type);
  else el->type = 0;
}

void csDebuggingGraph::RemoveObject (iObjectRegistry* object_reg,
	void* object, char* file, int linenr)
{
#ifdef CS_DEBUG
  if (!object_reg) object_reg = iSCF::SCF->object_reg;
#endif
  if (!object_reg) return;
  (void)file;
  (void)linenr;
  iDebugGraph* dg = SetupDebugGraph (object_reg);
  if (!dg) return;

  csDGEL* el = dg->FindEl (object);
  if (!el)
  {
    csPrintf ("ERROR! Cannot find element for object %p!\n", object);
    fflush (stdout);
    CS_ASSERT (false);
    return;
  }
  if (!el->used)
  {
    csPrintf ("ERROR! Element for object %p is not allocated!\n", object);
    fflush (stdout);
    CS_ASSERT (false);
    return;
  }

  el->used = false;
}

void csDebuggingGraph::AddParent (iObjectRegistry* object_reg,
	void* child, void* parent)
{
#ifdef CS_DEBUG
  if (!object_reg) object_reg = iSCF::SCF->object_reg;
#endif
  if (!object_reg) return;
  iDebugGraph* dg = SetupDebugGraph (object_reg);
  if (!dg) return;
  csDGEL* p_el = dg->FindEl (parent);
  // If parent could not be found. Create a dummy place holder for later.
  if (!p_el) p_el = dg->AddEl (parent);
  csDGEL* c_el = dg->FindEl (child);
  // If child could not be found. Create a dummy place holder for later.
  if (!c_el) c_el = dg->AddEl (child);

  c_el->AddParent (p_el, dg->GetLastTimestamp());
}

void csDebuggingGraph::AddChild (iObjectRegistry* object_reg,
	void* parent, void* child)
{
#ifdef CS_DEBUG
  if (!object_reg) object_reg = iSCF::SCF->object_reg;
#endif
  if (!object_reg) return;
  iDebugGraph* dg = SetupDebugGraph (object_reg);
  if (!dg) return;
  csDGEL* p_el = dg->FindEl (parent);
  // If parent could not be found. Create a dummy place holder for later.
  if (!p_el) p_el = dg->AddEl (parent);
  csDGEL* c_el = dg->FindEl (child);
  // If child could not be found. Create a dummy place holder for later.
  if (!c_el) c_el = dg->AddEl (child);

  p_el->AddChild (c_el, dg->GetLastTimestamp());
}

void csDebuggingGraph::RemoveParent (iObjectRegistry* object_reg,
	void* child, void* parent)
{
#ifdef CS_DEBUG
  if (!object_reg) object_reg = iSCF::SCF->object_reg;
#endif
  if (!object_reg) return;
  iDebugGraph* dg = SetupDebugGraph (object_reg);
  if (!dg) return;
  csDGEL* c_el = dg->FindEl (child);
  if (!c_el) return;	// Nothing to do if child is not there.
  csDGEL* p_el = dg->FindEl (parent);
  if (!p_el) return;	// Nothing to do if parent doesn't exist either.

  c_el->RemoveParent (p_el);
}

void csDebuggingGraph::RemoveChild (iObjectRegistry* object_reg,
	void* parent, void* child)
{
#ifdef CS_DEBUG
  if (!object_reg) object_reg = iSCF::SCF->object_reg;
#endif
  if (!object_reg) return;
  iDebugGraph* dg = SetupDebugGraph (object_reg);
  if (!dg) return;
  csDGEL* p_el = dg->FindEl (parent);
  if (!p_el) return;	// Nothing to do.
  csDGEL* c_el = dg->FindEl (child);
  if (!c_el) return;	// Nothing to do.

  p_el->RemoveChild (c_el);
}

void csDebuggingGraph::Clear (iObjectRegistry* object_reg)
{
#ifdef CS_DEBUG
  if (!object_reg) object_reg = iSCF::SCF->object_reg;
#endif
  if (!object_reg) return;
  iDebugGraph* dg = SetupDebugGraph (object_reg);
  if (!dg) return;
  dg->Clear ();
}

void csDebuggingGraph::Dump (iObjectRegistry* object_reg)
{
#ifdef CS_DEBUG
  if (!object_reg) object_reg = iSCF::SCF->object_reg;
#endif
  if (!object_reg) return;
  iDebugGraph* dg = SetupDebugGraph (object_reg);
  if (!dg) return;

  csDGEL** els = dg->GetEls();
  // First mark all elements as unused and count the number
  // of elements we have.
  int i, cnt = 0;
  for (i = 0 ; i < dg->GetNumEls() ; i++)
  {
    if (els[i]->used)
    {
      cnt++;
      els[i]->marker = false;
    }
    else
      els[i]->marker = true;
    els[i]->recurse_marker = false;
  }

  csPrintf ("====================================================\n");
  csPrintf ("Total number of used objects in graph: %d\n", cnt);

  // Find the first unmarked object and dump it.
  i = 0;
  while (i < dg->GetNumEls())
  {
    if (!els[i]->marker)
    {
      Dump (object_reg, els[i]->object, false);
      i = 0;	// Restart scan.
      csPrintf ("----------------------------------------------------\n");
    }
    else i++;
  }
  fflush (stdout);
}

static void DumpSubTree (int indent, const char* type, uint32 link_timestamp,
	csDGEL* el)
{
  // link_timestamp is the timestamp when the link was created.

  char spaces[1000];
  int ind = indent;
  if (ind > 999) ind = 999;
  char* sp = spaces;
  while (ind >= 10)
  {
    strcpy (sp, "          ");
    sp += 10;
    ind -= 10;
  }
  while (ind >= 1)
  {
    *sp++ = ' ';
    ind--;
  }
  *sp = 0;

  if (el->recurse_marker && *type == 'P')
  {
    // We already encountered this object in this recursion. So we just
    // put a short-hand here.
    csPrintf ("%s%s(%" PRIu32 ") %p <-\n", spaces, type, link_timestamp, 
      el->object);
    return;
  }

  // Show the ref count if it is an scf interface. If the object
  // is no longer used then show '?' instead of ref count to avoid
  // calling an invalid pointer.
  csPrintf ("%s%s(%" PRIu32 ") %p(", spaces, type, link_timestamp, el->object);
  if (el->scf)
  {
    if (el->used)
      csPrintf ("r%d,", ((iBase*)(el->object))->GetRefCount ());
    else
      csPrintf ("r?,-");
  }
  else if (!el->used)
    csPrintf ("-");
  if (el->type)
    csPrintf ("t%" PRIu32 ") %s(%s)", el->timestamp, el->type, el->description);
  else
    csPrintf ("t%" PRIu32 ") %s", el->timestamp, el->description);

  // If the object is used but the link to this object was created
  // BEFORE the object (i.e. timestamps) then this is at least very
  // suspicious and is also marked as such.
  if (el->used && link_timestamp > 0 && link_timestamp < el->timestamp)
  {
    csPrintf (" (SUSPICIOUS!)");
  }

  if (el->marker || *type == 'P' || !el->used)
  {
    if (el->used)
    {
      if (el->marker)
        csPrintf (" (REF)\n");
      else
        csPrintf ("\n");
    }
    else
      csPrintf (" (BAD LINK!)\n");

    if (*type != 'P') el->marker = true;
  }
  else
  {
    el->marker = true;
    csPrintf (" (%s,%d) #p=%" PRIu16 " #c=%" PRIu16 "\n",
    	el->file, el->linenr, el->num_parents, el->num_children);
    if (el->num_parents + el->num_children > 0)
    {
      bool use_brackets = true;
      if (el->num_children == 0 && el->num_parents == 1)
      {
        // If we have only one parent and no children we check
	// if we already visited that parent in this recursion.
	// In that case we don't print the brackets.
	if (el->parents[0].link->recurse_marker) use_brackets = false;
      }

      if (use_brackets) csPrintf ("%s{\n", spaces);
      el->recurse_marker = true;
      int i;
      for (i = 0 ; i < el->num_parents ; i++)
      {
        DumpSubTree (indent+2, "P", el->parents[i].timestamp,
		el->parents[i].link);
      }
      for (i = 0 ; i < el->num_children ; i++)
      {
        DumpSubTree (indent+2, "C", el->children[i].timestamp,
		el->children[i].link);
      }
      el->recurse_marker = false;
      if (use_brackets) csPrintf ("%s}\n", spaces);
    }
  }
  fflush (stdout);
}

static int compare_el (const void* vel1, const void* vel2)
{
  csDGEL* el1 = *(csDGEL**)vel1;
  csDGEL* el2 = *(csDGEL**)vel2;
  if (el1->num_parents < el2->num_parents) return -1;
  else if (el1->num_parents > el2->num_parents) return 1;
  else return 0;
}

void csDebuggingGraph::Dump (iObjectRegistry* object_reg, void* object,
	bool reset_mark)
{
#ifdef CS_DEBUG
  if (!object_reg) object_reg = iSCF::SCF->object_reg;
#endif
  if (!object_reg) return;
  iDebugGraph* dg = SetupDebugGraph (object_reg);
  if (!dg) return;
  int i;
  if (reset_mark)
  {
    // First mark all elements as unused.
    csDGEL** els = dg->GetEls();
    for (i = 0 ; i < dg->GetNumEls() ; i++)
    {
      if (els[i]->used) els[i]->marker = false;
      else els[i]->marker = true;
      els[i]->recurse_marker = false;
    }
  }

  csDGEL* el = dg->FindEl (object);
  CS_ASSERT (el != 0);

  // First copy all elements that belong to this sub-graph
  // to a local array.
  csDGEL** local_els = new csDGEL* [dg->GetNumEls()];
  int done = 0, num = 0;
  local_els[num++] = el; el->marker = true;
  while (done < num)
  {
    csDGEL* lel = local_els[done++];
    if (lel->used)
    {
      for (i = 0 ; i < lel->num_parents ; i++)
      {
        if (!lel->parents[i].link->marker)
        {
          local_els[num++] = lel->parents[i].link;
	  lel->parents[i].link->marker = true;
        }
      }
      for (i = 0 ; i < lel->num_children ; i++)
        if (!lel->children[i].link->marker)
        {
          local_els[num++] = lel->children[i].link;
	  lel->children[i].link->marker = true;
        }
    }
  }

  // Now mark all elements as unused again.
  for (i = 0 ; i < num ; i++)
    local_els[i]->marker = false;

  // Sort all elements based on the number of parents.
  // This means that 'root' like elements will come first in the
  // array.
  qsort (local_els, num, sizeof (csDGEL*), compare_el);

  // Now dump all parents here until all are marked.
  for (i = 0 ; i < num ; i++)
  {
    if (!local_els[i]->used)
    {
      local_els[i]->marker = true;
    }
    else if (!local_els[i]->marker)
    {
      DumpSubTree (0, "R", 0, local_els[i]);
    }
  }

  delete[] local_els;
}

//-----------------------------------------------------------------------------

