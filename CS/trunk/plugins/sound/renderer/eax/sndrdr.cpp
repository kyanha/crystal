/*
    Copyright (C) 1998, 1999 by Nathaniel 'NooTe' Saint Martin
    Copyright (C) 1998, 1999 by Jorrit Tyberghein
    Written by Nathaniel 'NooTe' Saint Martin

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
#include "csutil/sysfunc.h"
#include <stdio.h>

#include "csutil/scf.h"
#include "iutil/cfgfile.h"
#include "iutil/event.h"
#include "iutil/eventq.h"
#include "iutil/objreg.h"
#include "iutil/virtclk.h"
#include "ivaria/reporter.h"

#define INITGUID
#include "sndrdr.h"
#include "sndlstn.h"
#include "sndsrc.h"
#include "sndhdl.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY(csSoundRenderEAX);


SCF_IMPLEMENT_IBASE(csSoundRenderEAX)
  SCF_IMPLEMENTS_INTERFACE(iSoundRender)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iComponent)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iEventHandler)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSoundRenderEAX::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSoundRenderEAX::eiEventHandler)
  SCF_IMPLEMENTS_INTERFACE (iEventHandler)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csSoundRenderEAX::csSoundRenderEAX(iBase *piBase)
{
  SCF_CONSTRUCT_IBASE(piBase);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiEventHandler);
  Listener = 0;
  AudioRenderer = 0;
  object_reg = 0;
}

bool csSoundRenderEAX::Initialize(iObjectRegistry *r)
{
  object_reg = r;
  csRef<iEventQueue> q = CS_QUERY_REGISTRY(object_reg, iEventQueue);
  if (q != 0)
    q->RegisterListener (&scfiEventHandler,
      CSMASK_Command | CSMASK_Broadcast | CSMASK_Nothing);
  LoadFormat.Bits = -1;
  LoadFormat.Freq = -1;
  LoadFormat.Channels = -1;
  Config.AddConfig(object_reg, "/config/sound.cfg");
  return true;
}

csSoundRenderEAX::~csSoundRenderEAX()
{
  csRef<iEventQueue> q = CS_QUERY_REGISTRY(object_reg, iEventQueue);
  if (q != 0)
    q->RemoveListener (&scfiEventHandler);
  Close();
  SCF_DESTRUCT_EMBEDDED_IBASE(scfiEventHandler);
  SCF_DESTRUCT_EMBEDDED_IBASE(scfiComponent);
  SCF_DESTRUCT_IBASE();
}

bool csSoundRenderEAX::Open()
{
  csRef<iReporter> reporter = CS_QUERY_REGISTRY (object_reg, iReporter);
  if (reporter)
    reporter->Report (CS_REPORTER_SEVERITY_NOTIFY,
  	"crystalspace.sound.eax",
	"SoundRender EAX selected");

  HRESULT r;
  if (!AudioRenderer)
  {
    r = DirectSoundCreate(0, &AudioRenderer, 0);
    if (r != DS_OK)
    {
      if (reporter)
        reporter->Report (CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.sound.eax",
		"Error : Cannot Initialize DirectSound3D (%s).", GetError(r));
      else
        csPrintf (
		"Error : Cannot Initialize DirectSound3D (%s).\n", GetError(r));
      Close();
      return false;
    }

    DWORD dwLevel = DSSCL_EXCLUSIVE;
    r = AudioRenderer->SetCooperativeLevel(GetForegroundWindow(), dwLevel);
    if (r != DS_OK)
    {
      if (reporter)
        reporter->Report (CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.sound.eax",
		"Error : Cannot Set Cooperative Level (%s).", GetError(r));
      else
	csPrintf (
		"Error : Cannot Set Cooperative Level (%s).\n", GetError(r));
      Close();
      return false;
    }
  }

  if (!Listener)
  {
    Listener = new csSoundListenerEAX(this);
    if (!Listener->Initialize(this))
    {
      return false;
    }
  }

  float vol = Config->GetFloat("Sound.Volume",-1);
  if (vol>=0) SetVolume(vol);
  if (reporter)
    reporter->Report (CS_REPORTER_SEVERITY_NOTIFY,
	"crystalspace.sound.eax",
    	"  Volume: %g\n", GetVolume());

  csTicks et, ct;
  csRef<iVirtualClock> vc = CS_QUERY_REGISTRY (object_reg, iVirtualClock);
  et = vc->GetElapsedTicks ();
  ct = vc->GetCurrentTicks ();
  LastTime = ct;

  return true;
}

void csSoundRenderEAX::Close()
{
  while (ActiveSources.Length()>0)
    ActiveSources.Get(0)->Stop();

  while (SoundHandles.Length()>0)
  {
    csSoundHandleEAX *hdl = SoundHandles.Pop();
    hdl->Unregister();
    hdl->DecRef();
  }

  if (Listener) Listener->DecRef();
  Listener = 0;

  if (AudioRenderer) AudioRenderer->Release();
  AudioRenderer = 0;
}

void csSoundRenderEAX::SetVolume(float vol)
{
  if (!Listener) return;
  long dsvol = DSBVOLUME_MIN + (DSBVOLUME_MAX-DSBVOLUME_MIN)*vol;
  int a = Listener->PrimaryBuffer->SetVolume(dsvol);
}

float csSoundRenderEAX::GetVolume()
{
  if (!Listener) return 0;
  long dsvol;
  int a = Listener->PrimaryBuffer->GetVolume(&dsvol);
  return (float)(dsvol-DSBVOLUME_MIN)/(float)(DSBVOLUME_MAX-DSBVOLUME_MIN);
}


csPtr<iSoundHandle> csSoundRenderEAX::RegisterSound(iSoundData *snd)
{
  if (!snd) return 0;
  if (!snd->Initialize(&LoadFormat)) return 0;
  csSoundHandleEAX *hdl = new csSoundHandleEAX(this, snd);
  SoundHandles.Push(hdl);
  return csPtr<iSoundHandle> (hdl);
}

void csSoundRenderEAX::UnregisterSound(iSoundHandle *snd)
{
  int n = SoundHandles.Find((csSoundHandleEAX*)snd);
  if (n != -1)
  {
    csSoundHandleEAX *hdl = (csSoundHandleEAX *)snd;
    SoundHandles.DeleteIndex (n);
    hdl->Unregister();
    hdl->DecRef();
  }
}


iSoundListener *csSoundRenderEAX::GetListener()
{
  return Listener;
}

void csSoundRenderEAX::Update()
{
  int i;
  csTicks et, ct;
  csRef<iVirtualClock> vc = CS_QUERY_REGISTRY (object_reg, iVirtualClock);
  et = vc->GetElapsedTicks ();
  ct = vc->GetCurrentTicks ();
  csTicks ETime = ct - LastTime;
  LastTime = ct;

  Listener->Prepare();

  for (i=0;i<SoundHandles.Length();i++)
  {
    csSoundHandleEAX *hdl = SoundHandles.Get(i);
    hdl->Update_Time(ETime);
  }

  for (i=0;i<ActiveSources.Length();i++)
  {
    csSoundSourceEAX *src = ActiveSources.Get(i);
    if (!src->IsPlaying())
    {
      ActiveSources.DeleteIndex (i);
      i--;
    }
  }
}


void csSoundRenderEAX::MixingFunction()
{
}

void csSoundRenderEAX::SetDirty()
{
  Listener->Dirty = true;
}

void csSoundRenderEAX::AddSource(csSoundSourceEAX *src)
{
  if (ActiveSources.Find(src)==-1)
  {
    src->IncRef();
    ActiveSources.Push(src);
  }
}

void csSoundRenderEAX::RemoveSource(csSoundSourceEAX *src)
{
  int n=ActiveSources.Find(src);
  if (n!=-1)
  {
    ActiveSources.DeleteIndex(n);
    src->DecRef();
  }
}

const char *csSoundRenderEAX::GetError(HRESULT r)
{
  switch (r)
  {
    case DS_OK: return "success";
    case DSERR_ALLOCATED: return "the resource is already allocated";
    case DSERR_ALREADYINITIALIZED: return "the object is already initialized";
    case DSERR_BADFORMAT: return "the specified wave format is not supported";
    case DSERR_BUFFERLOST: return "the buffer memory has been lost";
    case DSERR_CONTROLUNAVAIL: return "the requested control "
      "(volume, pan, ...) is not available";
    case DSERR_INVALIDCALL:
      return "this function is not valid for the current state of this object";
    case DSERR_INVALIDPARAM: return "an invalid parameter was passed to the "
      "returning function";
    case DSERR_NOAGGREGATION: return "the object does not support aggregation";
    case DSERR_NODRIVER: return "no sound driver is available for use";
    case DSERR_OTHERAPPHASPRIO: return "OTHERAPPHASPRIO";
    case DSERR_OUTOFMEMORY: return "out of memory";
    case DSERR_PRIOLEVELNEEDED: return "the application does not have the "
      "required priority level";
    case DSERR_UNINITIALIZED: return "IDirectSound not initialized";
    case DSERR_UNSUPPORTED: return "function is not supported at this time";

    case DSERR_GENERIC: default: return "unknown DirectSound error";
  }
}

bool csSoundRenderEAX::HandleEvent (iEvent &e)
{
  if (e.Type == csevCommand || e.Type == csevBroadcast)
  {
    switch (e.Command.Code)
    {
    case cscmdPreProcess:
      Update();
      break;
    case cscmdSystemOpen:
      Open();
      break;
    case cscmdSystemClose:
      Close();
      break;
    }
  }
  return false;
}
