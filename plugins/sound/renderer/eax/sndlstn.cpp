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
#include "isystem.h"

#include "dsound.h"
#include "eax.h"

#include "sndrdr.h"
#include "sndlstn.h"

IMPLEMENT_FACTORY(csSoundListenerEAX)

IMPLEMENT_IBASE(csSoundListenerEAX)
IMPLEMENTS_INTERFACE(iSoundListener)
IMPLEMENT_IBASE_END;

csSoundListenerEAX::csSoundListenerEAX(iBase *piBase)
{
	CONSTRUCT_IBASE(piBase);
	fPosX = fPosY = fPosZ = 0.0;
	fDirTopX = fDirTopY = fDirTopZ = 0.0;
	fDirFrontX = fDirFrontY = fDirFrontZ = 0.0;
	fVelX = fVelY = fVelZ = 0.0;
	fDoppler = 1.0;
	fDistance = 1.0;
	fRollOff = 1.0;
	
	m_bEaxSupported = false;
	m_p3DAudioRenderer = NULL;
	m_pDS3DPrimaryBuffer = NULL;
	m_pDS3DListener = NULL;
	m_pKsPropertiesSet = NULL;
}

csSoundListenerEAX::~csSoundListenerEAX()
{
	DestroyListener();
}

int csSoundListenerEAX::CreateListener(iSoundRender * render)
{
	HRESULT hr;
	csSoundRenderEAX *renderEAX;
	
	if(!render) return E_FAIL;
	renderEAX = (csSoundRenderEAX *)render;
	
	m_p3DAudioRenderer = renderEAX->m_p3DAudioRenderer;
	
	if (!m_p3DAudioRenderer)
		return(E_FAIL);
	
	DSBUFFERDESC    dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D;
	
	if ((hr = m_p3DAudioRenderer->CreateSoundBuffer(&dsbd, &m_pDS3DPrimaryBuffer, NULL)) != DS_OK)
	{
		return E_FAIL;
	}
	
	if ((hr = m_pDS3DPrimaryBuffer->QueryInterface(IID_IDirectSound3DListener, (void **) &m_pDS3DListener)) != DS_OK)
	{
		return E_FAIL;
	}
	
	if ((hr = m_pDS3DListener->SetDopplerFactor(fDoppler, DS3D_IMMEDIATE)) != DS_OK)
	{
		return E_FAIL;
	}
	
	if ((hr = m_pDS3DListener->SetDistanceFactor(fDistance, DS3D_IMMEDIATE)) != DS_OK)
	{
		return E_FAIL;
	}
	
	if ((hr = m_pDS3DListener->SetRolloffFactor(fRollOff, DS3D_IMMEDIATE)) != DS_OK)
	{
		return E_FAIL;
	}
	
	hr = m_pDS3DPrimaryBuffer->QueryInterface(IID_IKsPropertySet, (void**) &m_pKsPropertiesSet);
	ULONG support = 0;
	if(SUCCEEDED(hr) && m_pKsPropertiesSet)
	{    
		hr = m_pKsPropertiesSet->QuerySupport(DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ALLPARAMETERS, &support);
		if(SUCCEEDED(hr))
		{
			if( (support&(KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET))
				== (KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET))
			{
				m_bEaxSupported = true;
			}
		}
		if(!m_bEaxSupported)
		{
			m_piSystem->Printf(MSG_INITIALIZATION, "WARNING : this device don't support EAX 2.0\n");
		}
	}
	else
		m_piSystem->Printf(MSG_INITIALIZATION, "WARNING : cannot get properties, this device may not support EAX\n");
	
	return S_OK;
}

int csSoundListenerEAX::DestroyListener()
{
	HRESULT hr;
	
	if(m_pKsPropertiesSet)
	{
		if ((hr = m_pKsPropertiesSet->Release()) < DS_OK)
			return(hr);
		
		m_pKsPropertiesSet = NULL;
	}
	
	if (m_pDS3DListener)
	{
		if ((hr = m_pDS3DListener->Release()) < DS_OK)
			return(hr);
		
		m_pDS3DListener = NULL;
	}
	
	if (m_pDS3DPrimaryBuffer)
	{
		if ((hr = m_pDS3DPrimaryBuffer->Stop()) < DS_OK)
			return(hr);
		
		if ((hr = m_pDS3DPrimaryBuffer->Release()) < DS_OK)
			return(hr);
		
		m_pDS3DPrimaryBuffer = NULL;
	}
	
	fDoppler = 1.0f;
	fDistance = 1.0f;
	fRollOff = 1.0f;
	
	return S_OK;
}

void csSoundListenerEAX::SetPosition(float x, float y, float z)
{
	fPosX = x; fPosY = y; fPosZ = z;
	m_pDS3DListener->SetPosition( fPosX, fPosY, fPosZ, DS3D_IMMEDIATE );
}

void csSoundListenerEAX::SetDirection(float fx, float fy, float fz, float tx, float ty, float tz)
{
	fDirFrontX = fx; fDirFrontY = fy; fDirFrontZ = fz;
	fDirTopX = tx; fDirTopY = ty; fDirTopZ = tz;
	
	m_pDS3DListener->SetOrientation(
		fDirFrontX, fDirFrontY, fDirFrontZ,
		fDirTopX, fDirTopY, fDirTopZ,
		DS3D_IMMEDIATE);
}

void csSoundListenerEAX::SetHeadSize(float size)
{
	fHeadSize = size;
}

void csSoundListenerEAX::SetVelocity(float x, float y, float z)
{
	fVelX = x; fVelY = y; fVelZ = z;
	
	if(!m_pDS3DListener) return;
	
	m_pDS3DListener->SetVelocity(
		fVelX, fVelY, fVelZ,
		DS3D_IMMEDIATE);
}

void csSoundListenerEAX::SetDopplerFactor(float factor)
{
	fDoppler = factor;
	
	if(!m_pDS3DListener) return;
	
	m_pDS3DListener->SetDopplerFactor(fDoppler, DS3D_IMMEDIATE);
}

void csSoundListenerEAX::SetDistanceFactor(float factor)
{
	fDistance = factor;
	
	if(!m_pDS3DListener) return;
	
	m_pDS3DListener->SetDistanceFactor(fDistance, DS3D_IMMEDIATE);
}

void csSoundListenerEAX::SetRollOffFactor(float factor)
{
	fRollOff = factor;
	
	if(!m_pDS3DListener) return;
	
	m_pDS3DListener->SetRolloffFactor(fRollOff, DS3D_IMMEDIATE);
}

#define MAX_s2eaxEnv 25
struct s2eaxEnv_type
{
	SoundEnvironment senv;
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

void csSoundListenerEAX::SetEnvironment(SoundEnvironment env)
{
	Environment = env;
	
	if(m_bEaxSupported)
	{
		if(m_pKsPropertiesSet)
		{
			DWORD preset={EAX_ENVIRONMENT_GENERIC};
			
			for(int i=0; i<MAX_s2eaxEnv; i++)
			{
				if(s2eaxEnv[i].senv==env)
				{
					preset = s2eaxEnv[i].eaxenv;
					break;
				}
			}
			
			m_pKsPropertiesSet->Set(DSPROPSETID_EAX_ListenerProperties,
				DSPROPERTY_EAXLISTENER_ENVIRONMENT,
				NULL,
				0,
				&preset,
				sizeof(DWORD));
		}
	}
}

void csSoundListenerEAX::GetPosition(float &x, float &y, float &z)
{
	x = fPosX; y = fPosY; z = fPosZ;
}

void csSoundListenerEAX::GetDirection(float &fx, float &fy, float &fz, float &tx, float &ty, float &tz)
{
	fx = fDirFrontX; fy = fDirFrontY; fz = fDirFrontZ;
	tx = fDirTopX; ty = fDirTopY; tz = fDirTopZ;
}

float csSoundListenerEAX::GetHeadSize()
{
	return fHeadSize;
}

void csSoundListenerEAX::GetVelocity(float &x, float &y, float &z)
{
	x = fVelX; y = fVelY; z = fVelZ;
}

float csSoundListenerEAX::GetDopplerFactor()
{
	return fDoppler;
}

float csSoundListenerEAX::GetDistanceFactor()
{
	return fDistance;
}

float csSoundListenerEAX::GetRollOffFactor()
{
	return fRollOff;
}

SoundEnvironment csSoundListenerEAX::GetEnvironment()
{
	return Environment;
}
