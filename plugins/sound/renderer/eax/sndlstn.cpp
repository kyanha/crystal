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
#include "csutil/scf.h"
#include "ivaria/reporter.h"
#include "iutil/objreg.h"

#include "dsound.h"
#include "eax.h"

#include "sndrdr.h"
#include "sndlstn.h"

SCF_IMPLEMENT_IBASE(csSoundListenerEAX)
	SCF_IMPLEMENTS_INTERFACE(iSoundListener)
SCF_IMPLEMENT_IBASE_END;

csSoundListenerEAX::csSoundListenerEAX(iBase *piBase) {
  SCF_CONSTRUCT_IBASE(piBase);
  PrimaryBuffer = 0;
  Listener = 0;
  Renderer = 0;

	EaxKsPropertiesSet = 0;
}

csSoundListenerEAX::~csSoundListenerEAX() {

	if(EaxKsPropertiesSet)
	{
		EaxKsPropertiesSet->Release();
		EaxKsPropertiesSet = 0;
	}

  if (Renderer) Renderer->DecRef();
  if (Listener) Listener->Release();
  if (PrimaryBuffer) {
    PrimaryBuffer->Stop();
    PrimaryBuffer->Release();
  }
}

bool csSoundListenerEAX::Initialize(csSoundRenderEAX *srdr) {
  srdr->IncRef();
  Renderer = srdr;

  DSBUFFERDESC dsbd;
  dsbd.dwSize = sizeof(DSBUFFERDESC);
  dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME;
  dsbd.dwReserved = 0;
  dsbd.dwBufferBytes = 0;
  dsbd.lpwfxFormat = 0;

  csRef<iReporter> reporter = CS_QUERY_REGISTRY (Renderer->object_reg, iReporter); ;

  HRESULT r;
  r = Renderer->AudioRenderer->CreateSoundBuffer(&dsbd, &PrimaryBuffer, 0);
  if (r != DS_OK) {
    if (reporter)
      reporter->Report (CS_REPORTER_SEVERITY_WARNING,
        "crystalspace.sound.eax", "EAX listener: "
        "Cannot create primary sound buffer (%s).\n", Renderer->GetError(r));
    return false;
  }

  r = PrimaryBuffer->QueryInterface(IID_IDirectSound3DListener, (void **) &Listener);
  if (r != DS_OK) {
    if (reporter)
      reporter->Report (CS_REPORTER_SEVERITY_WARNING,
        "crystalspace.sound.eax", "EAX listener: Cannot query listener"
        " interface from primary sound buffer (%s).\n", Renderer->GetError(r));
    return false;
  }

	r = PrimaryBuffer->QueryInterface(IID_IKsPropertySet, (void**) &EaxKsPropertiesSet);
	ULONG support = 0;
	if(SUCCEEDED(r) && EaxKsPropertiesSet)
	{
		r = EaxKsPropertiesSet->QuerySupport(DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ALLPARAMETERS, &support);
		if(!SUCCEEDED(r) || (support&(KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET))
				!= (KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET))
		{
			if (reporter)
                          reporter->Report (CS_REPORTER_SEVERITY_WARNING,
                            "crystalspace.sound.eax",
                            "EAX listener : this device don't support EAX 2.0\nSo only DirectSound3D will be used.");
			EaxKsPropertiesSet->Release();
			EaxKsPropertiesSet = 0;
		}
	}
	else {
		if (reporter)
                   reporter->Report (CS_REPORTER_SEVERITY_WARNING,
                     "crystalspace.sound.eax",
                     "EAX listener : cannot get properties, this device may not support EAX\nSo only DirectSound3D will be used.");
             }

  SetPosition(csVector3(0,0,0));
  SetVelocity(csVector3(0,0,0));
  SetDirection(csVector3(0,0,1), csVector3(0,1,0));
  SetDistanceFactor(1.0);
  SetDopplerFactor(1.0);
  SetDistanceFactor(1.0);
  SetRollOffFactor(1.0);
  SetEnvironment(ENVIRONMENT_GENERIC);
  Prepare();

  return true;
}

void csSoundListenerEAX::SetPosition(const csVector3 &v) {
  Dirty = true;
  csSoundListener::SetPosition(v);
  Listener->SetPosition( v.x, v.y, v.z, DS3D_DEFERRED);
}

void csSoundListenerEAX::SetDirection(const csVector3 &f, const csVector3 &t) {
  Dirty = true;
  csSoundListener::SetDirection(f, t);
  Listener->SetOrientation(f.x, f.y, f.z,t.x, t.y, t.z,DS3D_DEFERRED);
}

void csSoundListenerEAX::SetHeadSize(float size) {
//  Dirty = true;
  csSoundListener::SetHeadSize(size);
// @@@
}

void csSoundListenerEAX::SetVelocity(const csVector3 &v) {
  Dirty = true;
  csSoundListener::SetVelocity(v);
  Listener->SetVelocity(v.x, v.y, v.z, DS3D_DEFERRED);
}

void csSoundListenerEAX::SetDopplerFactor(float factor) {
  Dirty = true;
  csSoundListener::SetDopplerFactor(factor);
  Listener->SetDopplerFactor(factor, DS3D_DEFERRED);
}

void csSoundListenerEAX::SetDistanceFactor(float factor) {
  Dirty = true;
  csSoundListener::SetDistanceFactor(factor);
  Listener->SetDistanceFactor(factor, DS3D_DEFERRED);
}

void csSoundListenerEAX::SetRollOffFactor(float factor) {
  Dirty = true;
  csSoundListener::SetRollOffFactor(factor);
  Listener->SetRolloffFactor(factor, DS3D_DEFERRED);
}

#define MAX_s2eaxEnv 25
struct s2eaxEnv_type
{
	csSoundEnvironment senv;
	DWORD eaxenv;
} s2eaxEnv[MAX_s2eaxEnv]=
{
	{ENVIRONMENT_GENERIC,         {EAX_ENVIRONMENT_GENERIC}},
	{ENVIRONMENT_PADDEDCELL,      {EAX_ENVIRONMENT_PADDEDCELL}},
	{ENVIRONMENT_ROOM,            {EAX_ENVIRONMENT_ROOM}},
	{ENVIRONMENT_BATHROOM,        {EAX_ENVIRONMENT_BATHROOM}},
	{ENVIRONMENT_LIVINGROOM,      {EAX_ENVIRONMENT_LIVINGROOM}},
	{ENVIRONMENT_STONEROOM,       {EAX_ENVIRONMENT_STONEROOM}},
	{ENVIRONMENT_AUDITORIUM,      {EAX_ENVIRONMENT_AUDITORIUM}},
	{ENVIRONMENT_CONCERTHALL,     {EAX_ENVIRONMENT_CONCERTHALL}},
	{ENVIRONMENT_CAVE,            {EAX_ENVIRONMENT_CAVE}},
	{ENVIRONMENT_ARENA,           {EAX_ENVIRONMENT_ARENA}},
	{ENVIRONMENT_CARPETEDHALLWAY, {EAX_ENVIRONMENT_CARPETEDHALLWAY}},
	{ENVIRONMENT_HALLWAY,         {EAX_ENVIRONMENT_HALLWAY}},
	{ENVIRONMENT_STONECORRIDOR,   {EAX_ENVIRONMENT_STONECORRIDOR}},
	{ENVIRONMENT_ALLEY,           {EAX_ENVIRONMENT_ALLEY}},
	{ENVIRONMENT_FOREST,          {EAX_ENVIRONMENT_FOREST}},
	{ENVIRONMENT_CITY,            {EAX_ENVIRONMENT_CITY}},
	{ENVIRONMENT_MOUNTAINS,       {EAX_ENVIRONMENT_MOUNTAINS}},
	{ENVIRONMENT_QUARRY,          {EAX_ENVIRONMENT_QUARRY}},
	{ENVIRONMENT_PLAIN,           {EAX_ENVIRONMENT_PLAIN}},
	{ENVIRONMENT_PARKINGLOT,      {EAX_ENVIRONMENT_PARKINGLOT}},
	{ENVIRONMENT_SEWERPIPE,       {EAX_ENVIRONMENT_SEWERPIPE}},
	{ENVIRONMENT_UNDERWATER,      {EAX_ENVIRONMENT_UNDERWATER}},
	{ENVIRONMENT_DRUGGED,         {EAX_ENVIRONMENT_DRUGGED}},
	{ENVIRONMENT_DIZZY,           {EAX_ENVIRONMENT_DIZZY}},
	{ENVIRONMENT_PSYCHOTIC,       {EAX_ENVIRONMENT_PSYCHOTIC}}
};

void csSoundListenerEAX::SetEnvironment(csSoundEnvironment env) {

  Environment = env;

	if(EaxKsPropertiesSet)
	{
		DWORD preset={EAX_ENVIRONMENT_GENERIC};

		int i;
		for(i=0; i<MAX_s2eaxEnv; i++)
		{
			if(s2eaxEnv[i].senv==env)
			{
				preset = s2eaxEnv[i].eaxenv;
				break;
			}
		}

		EaxKsPropertiesSet->Set(DSPROPSETID_EAX_ListenerProperties,
			DSPROPERTY_EAXLISTENER_ENVIRONMENT,
			0,
			0,
			&preset,
			sizeof(DWORD));
	}
}




void csSoundListenerEAX::Prepare() {
  if (!Dirty) return;
  Listener->CommitDeferredSettings();
  Dirty = false;
}
