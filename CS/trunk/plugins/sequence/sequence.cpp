/*
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

#include <string.h>
#define SYSDEF_PATH
#include "cssysdef.h"
#include "version.h"
#include "csutil/scf.h"
#include "sequence.h"
#include "isys/system.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "isys/vfs.h"
#include "isys/event.h"

//---------------------------------------------------------------------------

IMPLEMENT_IBASE (csSequence)
  IMPLEMENTS_INTERFACE (iSequence)
IMPLEMENT_IBASE_END

csSequence::csSequence (iSequenceManager* seqmgr) : first (NULL), last (NULL)
{
  CONSTRUCT_IBASE (NULL);
  csSequence::seqmgr = seqmgr;
}

csSequence::~csSequence ()
{
  Clear ();
}

void csSequence::Clear ()
{
  while (first)
  {
    csSequenceOp* n = first->next;
    delete first;
    first = n;
  }
  last = NULL;
}

void csSequence::DeleteFirstSequence ()
{
  if (first)
  {
    csSequenceOp* n = first->next;
    delete first;
    first = n;
    if (!first) last = NULL;
    else first->prev = NULL;
  }
}

void csSequence::AddOperation (cs_time time, iSequenceOperation* operation)
{
  csSequenceOp* op = new csSequenceOp ();
  op->time = time;
  op->operation = operation;
  op->operation->IncRef ();
  // Insert this operation at the right time.
  if (first)
  {
    csSequenceOp* o = first;
    while (o)
    {
      if (time <= o->time)
      {
        op->next = o;
	op->prev = o->prev;
	if (o->prev) o->prev->next = op;
	else first = op;
	o->prev = op;
	break;
      }
      o = o->next;
    }
    if (!o)
    {
      // Put it last.
      op->next = NULL;
      op->prev = last;
      last->next = op;
      last = op;
    }
  }
  else
  {
    // The very first operation.
    first = last = op;
    op->prev = op->next = NULL;
  }
}

void csSequence::AddRunSequence (cs_time time, iSequence* sequence)
{
  RunSequenceOp* op = new RunSequenceOp (seqmgr, sequence);
  AddOperation (time, op);
  op->DecRef ();
}

void csSequence::RunSequenceOp::Do (cs_time dt)
{
  seqmgr->RunSequence (-(signed)dt, sequence);
}

void csSequence::AddCondition (cs_time time, iSequenceCondition* condition,
  	iSequence* trueSequence, iSequence* falseSequence)
{
  RunCondition* op = new RunCondition (seqmgr, condition, trueSequence,
  	falseSequence);
  AddOperation (time, op);
  op->DecRef ();
}

void csSequence::RunCondition::Do (cs_time dt)
{
  if (condition->Condition (dt))
    seqmgr->RunSequence (-(signed)dt, trueSequence);
  else
    seqmgr->RunSequence (-(signed)dt, falseSequence);
}

void csSequence::AddLoop (cs_time time, iSequenceCondition* condition,
  	iSequence* sequence)
{
  RunLoop* op = new RunLoop (seqmgr, condition, sequence);
  AddOperation (time, op);
  op->DecRef ();
}

void csSequence::RunLoop::Do (cs_time dt)
{
  while (condition->Condition (dt))
    seqmgr->RunSequence (-(signed)dt, sequence);
}

//---------------------------------------------------------------------------

IMPLEMENT_FACTORY (csSequenceManager)

EXPORT_CLASS_TABLE (sequence)
  EXPORT_CLASS (csSequenceManager, "crystalspace.utilities.sequence",
    "Sequence Manager")
EXPORT_CLASS_TABLE_END

IMPLEMENT_IBASE (csSequenceManager)
  IMPLEMENTS_INTERFACE (iSequenceManager)
  IMPLEMENTS_EMBEDDED_INTERFACE (iPlugIn)
IMPLEMENT_IBASE_END

IMPLEMENT_EMBEDDED_IBASE (csSequenceManager::eiPlugIn)
  IMPLEMENTS_INTERFACE (iPlugIn)
IMPLEMENT_EMBEDDED_IBASE_END

csSequenceManager::csSequenceManager (iBase *iParent)
{
  CONSTRUCT_IBASE (iParent);
  CONSTRUCT_EMBEDDED_IBASE(scfiPlugIn);
  System = NULL;
  main_sequence = new csSequence (this);
  previous_time_valid = false;
  main_time = 0;
  suspended = true;
}

csSequenceManager::~csSequenceManager ()
{
  main_sequence->DecRef ();
}

bool csSequenceManager::Initialize (iSystem *system)
{
  System = system;
  if (!System->CallOnEvents (&scfiPlugIn, CSMASK_Nothing))
    return false;
  return true;
}

bool csSequenceManager::HandleEvent (iEvent &event)
{
  if (event.Type != csevBroadcast
   || event.Command.Code != cscmdPostProcess)
    return false;

  if (!suspended)
  {
    cs_time current_time = System->GetTime ();
    if (!previous_time_valid)
    {
      previous_time = current_time;
      previous_time_valid = true;
    }
    TimeWarp (current_time-previous_time, false);
    previous_time = current_time;
  }
  return true;
}

void csSequenceManager::Clear ()
{
  main_sequence->Clear ();
  main_time = 0;
  previous_time_valid = false;
}

void csSequenceManager::Suspend ()
{
  suspended = true;
}

void csSequenceManager::Resume ()
{
  if (suspended)
  {
    suspended = false;
    previous_time_valid = false;
  }
}

void csSequenceManager::TimeWarp (cs_time time, bool skip)
{
  main_time += time;
  csSequenceOp* seqOp = main_sequence->GetFirstSequence ();
  while (seqOp && seqOp->time <= main_time)
  {
    // Because an operation can itself modify the main sequence
    // queue we take care to first unlink this sequence operation
    // before performing it. Because DeleteFirstSequence() does a
    // DecRef() we first IncRef() it.
    iSequenceOperation* op = seqOp->operation;
    cs_time opt = seqOp->time;
    op->IncRef ();
    main_sequence->DeleteFirstSequence ();

    if (!skip)
    {
      op->Do (main_time - opt);
    }
    // Now really delete the operation.
    op->DecRef ();
    // And fetch the next one.
    seqOp = main_sequence->GetFirstSequence ();
  }
}

iSequence* csSequenceManager::NewSequence ()
{
  csSequence* n = new csSequence (this);
  return n;
}

void csSequenceManager::RunSequence (cs_time time, iSequence* sequence)
{
  csSequence* seq = (csSequence*)sequence;
  csSequenceOp* op = seq->GetFirstSequence ();
  while (op)
  {
    main_sequence->AddOperation (main_time + time + op->time, op->operation);
    op = op->next;
  }
}

//---------------------------------------------------------------------------

