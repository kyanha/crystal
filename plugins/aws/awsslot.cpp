#include "cssysdef.h"
#include "awsslot.h"
#include "awsadler.h"
#include "iaws/awsdefs.h"
#include <string.h>

#define callRefMemberFunction(object, ptrToMember)  ((object).*(ptrToMember))
#define callPtrMemberFunction(object, ptrToMember)  ((object)->*(ptrToMember))

static unsigned long NameToId (const char *n)
{
  if (n)
  {
    unsigned long id = aws_adler32 (
        aws_adler32 (0, 0, 0),
        (unsigned char *)n,
        strlen (n));

    return id;
  }
  else
    return 0;
}

///////////////////////////////////// Signal Sink Manager ////////////////////////////////////////////////////
awsSinkManager::awsSinkManager (iBase *p)
{
  SCF_CONSTRUCT_IBASE (p);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent);
}

awsSinkManager::~awsSinkManager ()
{
  int i;

  for (i = 0; i < sinks.Length (); ++i)
  {
    SinkMap *sm = (SinkMap *)sinks[i];

    sm->sink->DecRef ();
    delete sm;
  }

  return ;
}

bool awsSinkManager::Initialize (iObjectRegistry *)
{
  return true;
}

void awsSinkManager::RegisterSink (const char *name, iAwsSink *sink)
{
  sink->IncRef ();
  sinks.Push (new SinkMap (NameToId (name), sink));
}

bool awsSinkManager::RemoveSink (iAwsSink* sink)
{
  int rc = sinks.Find (sink);
  if (rc<0)
    return false;

  sinks.Delete (rc);
  sink->DecRef ();
  return true;
}

iAwsSink *awsSinkManager::FindSink (const char *_name)
{
  int i;
  unsigned long name = NameToId (_name);

  for (i = 0; i < sinks.Length (); ++i)
  {
    SinkMap *sm = (SinkMap *)sinks[i];

    if (sm->name == name)
      return sm->sink;
  }

  return 0;
}

iAwsSink *awsSinkManager::CreateSink (void *parm)
{
  return new awsSink (parm);
}

iAwsSlot *awsSinkManager::CreateSlot ()
{
  return new awsSlot ();
}

///////////////////////////////////// Signal Sinks //////////////////////////////////////////////////////////
awsSink::awsSink (void *p) :
parm(p), sink_err(0)
{
  SCF_CONSTRUCT_IBASE (0);
}

awsSink::~awsSink ()
{
  for (int i=0; i<triggers.Length(); i++)
  {
    TriggerMap *tm = (TriggerMap *)triggers[i];
    delete tm;
  }
  triggers.SetLength(0);
}

unsigned long awsSink::GetTriggerID (const char *_name)
{
  unsigned long name = NameToId (_name);
  int i;

  sink_err=0;

  for (i = 0; i < triggers.Length (); ++i)
  {
    TriggerMap *tm = (TriggerMap *)triggers[i];

    if (tm->name == name) return i;
  }

  sink_err = AWS_ERR_SINK_TRIGGER_NOT_FOUND;
  return 0;
}

void awsSink::HandleTrigger (int trigger, iAwsSource *source)
{
  sink_err = 0;

  if (triggers.Length () == 0) 
  {
    sink_err = AWS_ERR_SINK_NO_TRIGGERS;
    return ;
  }

  void (*Trigger) (void *, iAwsSource *) =
    (((TriggerMap *) (triggers[trigger]))->trigger);
  (Trigger) (parm, source);
}

void awsSink::RegisterTrigger (const char *name,
  void (*Trigger) (void *, iAwsSource *))
{
  sink_err = 0;
  triggers.Push (new TriggerMap (NameToId (name), Trigger));
}

///////////////////////////////////// Signal Sources ////////////////////////////////////////////////////////
awsSource::awsSource (iAwsComponent *_owner) :
  owner(_owner)
{
  SCF_CONSTRUCT_IBASE (0);
}

awsSource::~awsSource ()
{
  for (int i = 0; i < slots.Length (); ++i)
  {
    SlotSignalMap *p = (SlotSignalMap*) slots[i];
    p->slot->DecRef ();
    delete p;
  }
}

iAwsComponent *awsSource::GetComponent ()
{
  return owner;
}

bool awsSource::RegisterSlot (iAwsSlot *slot, unsigned long signal)
{
  SlotSignalMap *ssm = new SlotSignalMap;

  ssm->slot = slot;
  ssm->signal = signal;

  slot->IncRef ();
  slots.Push (ssm);

  return true;
}

bool awsSource::UnregisterSlot (iAwsSlot *slot, unsigned long signal)
{
  for (int i = 0; i < slots.Length (); ++i)
  {
    SlotSignalMap *ssm = (SlotSignalMap*) slots[i];

    if (ssm->signal == signal && ssm->slot == slot)
    {
      slot->DecRef ();
      slots.Delete (i);
      delete ssm;

      return true;
    }
  }

  return false;
}

void awsSource::Broadcast (unsigned long signal)
{
  int i;

  for (i = 0; i < slots.Length (); ++i)
  {
    SlotSignalMap *ssm = (SlotSignalMap*) slots[i];

    if (ssm->signal == signal)
	ssm->slot->Emit (*this, signal);
  }
}

///////////////////////////////////// Slots ////////////////////////////////////////////////////////
awsSlot::awsSlot ()
{
  SCF_CONSTRUCT_IBASE (0);
}

awsSlot::~awsSlot ()
{
    for (int i = 0; i < stmap.Length(); i++)
    {
	SignalTriggerMap* sm = (SignalTriggerMap*) stmap[i];
	sm->sink->DecRef ();
	delete sm;
    }
}

void awsSlot::Connect (
  iAwsSource *source,
  unsigned long signal,
  iAwsSink *sink,
  unsigned long trigger)
{
  source->RegisterSlot (this, signal);

  int i;

  for (i = 0; i < stmap.Length (); ++i)
  {
    SignalTriggerMap *stm = (SignalTriggerMap *)stmap[i];

    if (stm->signal == signal && stm->trigger == trigger && stm->sink == sink)
    {
      stm->refs++;
      return ;
    }
  }

  sink->IncRef ();
  stmap.Push (new SignalTriggerMap (signal, sink, trigger, 1));
}

void awsSlot::Disconnect (
  iAwsSource *source,
  unsigned long signal,
  iAwsSink *sink,
  unsigned long trigger)
{
  source->UnregisterSlot (this, signal);

  int i;

  for (i = 0; i < stmap.Length (); ++i)
  {
    SignalTriggerMap *stm = (SignalTriggerMap *)stmap[i];

    if (stm->signal == signal && stm->trigger == trigger && stm->sink == sink)
    {
      stm->refs--;

      if (stm->refs == 0)
      {
	stm->sink->DecRef ();
	stmap.Delete (i);
      }

      return ;
    }
  }
}

void awsSlot::Emit (iAwsSource &source, unsigned long signal)
{
  int i;

  for (i = 0; i < stmap.Length (); ++i)
  {
    SignalTriggerMap *stm = (SignalTriggerMap *)stmap[i];

    if (stm->signal == signal)
      stm->sink->HandleTrigger (stm->trigger, &source);
  }
}
