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

#ifndef __CS_SEQUENCE_H__
#define __CS_SEQUENCE_H__

#include "csutil/util.h"
#include "csutil/csvector.h"
#include "ivaria/sequence.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"

struct iObjectRegistry;

struct csSequenceOp
{
  csSequenceOp* next, * prev;
  csTicks time;
  csRef<iBase> params;
  csRef<iSequenceOperation> operation;

  csSequenceOp () { }
  ~csSequenceOp () { }
};

class csSequence : public iSequence
{
private:
  iSequenceManager* seqmgr;
  // A double linked list of all sequence operations.
  csSequenceOp* first;
  csSequenceOp* last;

public:
  //=====
  // Standard operation.
  //=====
  class StandardOperation : public iSequenceOperation
  {
  protected:
    iSequenceManager* seqmgr;
    virtual ~StandardOperation () { }
  public:
    SCF_DECLARE_IBASE;
    StandardOperation (iSequenceManager* sm) : seqmgr (sm)
    {
      SCF_CONSTRUCT_IBASE (NULL);
    }
  };

  //=====
  // An operation to run a sequence.
  //=====
  class RunSequenceOp : public StandardOperation
  {
  private:
    csRef<iSequence> sequence;
  protected:
    virtual ~RunSequenceOp () { }
  public:
    RunSequenceOp (iSequenceManager* sm, iSequence* seq) :
    	StandardOperation (sm)
    {
      sequence = seq;
    }
    virtual void Do (csTicks dt, iBase* params);
  };

  //=====
  // An operation for a condition.
  //=====
  class RunCondition : public StandardOperation
  {
  private:
    csRef<iSequenceCondition> condition;
    csRef<iSequence> trueSequence;
    csRef<iSequence> falseSequence;
  protected:
    virtual ~RunCondition () { }
  public:
    RunCondition (iSequenceManager* sm, iSequenceCondition* cond,
    	iSequence* trueSeq, iSequence* falseSeq) : StandardOperation (sm)
    {
      trueSequence = trueSeq;
      falseSequence = falseSeq;
      condition = cond;
    }
    virtual void Do (csTicks dt, iBase* params);
  };

  //=====
  // An operation for a loop.
  //=====
  class RunLoop : public StandardOperation
  {
  private:
    csRef<iSequenceCondition> condition;
    csRef<iSequence> sequence;
  protected:
    virtual ~RunLoop () { }
  public:
    RunLoop (iSequenceManager* sm, iSequenceCondition* cond, iSequence* seq) :
    	StandardOperation (sm)
    {
      sequence = seq;
      condition = cond;
    }
    virtual void Do (csTicks dt, iBase* params);
  };

public:
  SCF_DECLARE_IBASE;

  csSequence (iSequenceManager* seqmgr);
  virtual ~csSequence ();

  csSequenceOp* GetFirstSequence () { return first; }
  void DeleteFirstSequence ();

  virtual void AddOperation (csTicks time, iSequenceOperation* operation,
  	iBase* params = NULL);
  virtual void AddRunSequence (csTicks time, iSequence* sequence,
  	iBase* params = NULL);
  virtual void AddCondition (csTicks time, iSequenceCondition* condition,
  	iSequence* trueSequence, iSequence* falseSequence,
	iBase* params = NULL);
  virtual void AddLoop (csTicks time, iSequenceCondition* condition,
  	iSequence* sequence, iBase* params = NULL);
  virtual void Clear ();
  virtual bool IsEmpty () { return first == NULL; }
};

class csSequenceManager : public iSequenceManager
{
private:
  iObjectRegistry *object_reg;

  // The sequence manager uses one big sequence to keep all queued
  // sequence operations. New sequences will be merged with this one.
  csSequence* main_sequence;

  // The previous time.
  csTicks previous_time;
  bool previous_time_valid;

  // This is the current time for the sequence manager.
  // This is the main ticker that keeps the entire system running.
  csTicks main_time;

  // If true the sequence manager is suspended
  bool suspended;

public:
  SCF_DECLARE_IBASE;

  csSequenceManager (iBase *iParent);
  virtual ~csSequenceManager ();
  virtual bool Initialize (iObjectRegistry *object_reg);

  /// This is set to receive the once per frame nothing event.
  virtual bool HandleEvent (iEvent &event);

  virtual void Clear ();
  virtual bool IsEmpty () { return main_sequence->IsEmpty (); }
  virtual void Suspend ();
  virtual bool IsSuspended () { return suspended; }
  virtual void Resume ();
  virtual void TimeWarp (csTicks time, bool skip);
  virtual csTicks GetMainTime () { return main_time; }
  virtual iSequence* NewSequence ();
  virtual void RunSequence (csTicks time, iSequence* sequence,
  	iBase* params = NULL);

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csSequenceManager);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize(p); }
  } scfiComponent;
  struct EventHandler : public iEventHandler
  {
  private:
    csSequenceManager* parent;
  public:
    EventHandler (csSequenceManager* parent)
    {
      SCF_CONSTRUCT_IBASE (NULL);
      EventHandler::parent = parent;
    }
    SCF_DECLARE_IBASE;
    virtual bool HandleEvent (iEvent& e) { return parent->HandleEvent(e); }
  } * scfiEventHandler;
};

#endif // __CS_SEQUENCE_H__

