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

#include "ia3dapi.h"

#include "sndrdr.h"
#include "sndbuf.h"
#include "sndsrc.h"
#include "isystem.h"

IMPLEMENT_FACTORY(csSoundBufferA3D)

IMPLEMENT_IBASE(csSoundBufferA3D)
	IMPLEMENTS_INTERFACE(iSoundBuffer)
IMPLEMENT_IBASE_END;

csSoundBufferA3D::csSoundBufferA3D(iBase *piBase)
{
	CONSTRUCT_IBASE(piBase);
	m_p3DSource = NULL;
	m_p3DAudioRenderer = NULL;
}

csSoundBufferA3D::~csSoundBufferA3D()
{
}

iSoundSource *csSoundBufferA3D::CreateSource()
{
	csSoundSourceA3D* pNew = new csSoundSourceA3D(NULL);
	if (!pNew) return NULL;
	
	pNew->pSoundBuffer = this;
	pNew->m_p3DSource = m_p3DSource;
	pNew->m_p3DSource->SetRenderMode(A3DSOURCE_RENDERMODE_A3D |
        A3DSOURCE_RENDERMODE_OCCLUSIONS |
        A3DSOURCE_RENDERMODE_1ST_REFLECTIONS);
	pNew->m_p3DAudioRenderer = m_p3DAudioRenderer;
	
	return QUERY_INTERFACE(pNew, iSoundSource);
}

int csSoundBufferA3D::CreateSoundBuffer(iSoundRender * render, csSoundData * sound)
{
	HRESULT           hr;
	csSoundRenderA3D *renderA3D;
	VOID             *pbWrite1 = NULL, *pbWrite2 = NULL;
	DWORD             cbLen1, cbLen2;
	
	if (!render) return E_FAIL;
	if (!sound)  return E_FAIL;
	
	renderA3D = (csSoundRenderA3D *)render;
	
	m_p3DAudioRenderer = renderA3D->m_p3DAudioRenderer;
	
	if (!m_p3DAudioRenderer)
		return(E_FAIL);
	
	if ((hr = m_p3DAudioRenderer->NewSource( A3DSOURCE_INITIAL_RENDERMODE_NATIVE , &m_p3DSource)) != S_OK)
	{
		if (m_p3DSource)
			m_p3DSource = NULL;
		
		return(hr);
	}
	
	WAVEFORMATEX    wfxFormat;
	wfxFormat.wFormatTag                    = WAVE_FORMAT_PCM;
	wfxFormat.nChannels                     = (sound->isStereo())?2:1;
	wfxFormat.nSamplesPerSec        = sound->getFrequency();
	wfxFormat.wBitsPerSample        = (sound->is16Bits())?16:8;
	wfxFormat.nBlockAlign           = (wfxFormat.wBitsPerSample*wfxFormat.nChannels)/8;
	wfxFormat.nAvgBytesPerSec       = wfxFormat.nBlockAlign*wfxFormat.nSamplesPerSec;
	wfxFormat.cbSize                                = 0;
	
	if ((hr = m_p3DSource->SetWaveFormat((void*)&wfxFormat)) != S_OK)
	{
		if (m_p3DSource != NULL)
		{
			m_p3DSource->Release();
			m_p3DSource = NULL;
		}
		
		return(hr);
	}
	
	if ((hr = m_p3DSource->AllocateWaveData(sound->getSize())) != S_OK)
	{
		if (m_p3DSource != NULL)
		{
			m_p3DSource->Release();
			m_p3DSource = NULL;
		}
		
		return(hr);
	}
	
	if ((hr = m_p3DSource->Lock(0, sound->getSize(), &pbWrite1, &cbLen1, &pbWrite2, &cbLen2, A3D_ENTIREBUFFER)) != S_OK)
	{
		if (m_p3DSource != NULL)
		{
			m_p3DSource->Release();
			m_p3DSource = NULL;
		}
		
		return(hr);
	}
	
	CopyMemory(pbWrite1, sound->getData(), sound->getSize());
    
	if ((hr = m_p3DSource->Unlock(pbWrite1, sound->getSize(), pbWrite2, 0)) != S_OK)
	{
		if (m_p3DSource != NULL)
		{
			m_p3DSource->Release();
			m_p3DSource = NULL;
		}
		
		return(hr);
	}
	
	return S_OK;
}

int csSoundBufferA3D::DestroySoundBuffer()
{
	HRESULT hr;
	
	if (m_p3DSource)
	{
		if ((hr = m_p3DSource->Stop()) < S_OK)
			return(hr);
		
		if ((hr = m_p3DSource->FreeWaveData()) != S_OK)
			return(hr);
		
		if ((hr = m_p3DSource->Release()) < S_OK)
			return(hr);
		
		m_p3DSource = NULL;
	}
	
	return S_OK;
}

void csSoundBufferA3D::Play(SoundBufferPlayMethod playMethod)
{
	if (m_p3DSource)
	{
		switch(playMethod)
		{
		case SoundBufferPlay_Normal:
			m_p3DSource->Stop();
			m_p3DSource->Play(A3D_SINGLE);
			break;
			
		case SoundBufferPlay_InLoop:
			m_p3DSource->Stop();
			m_p3DSource->Play(A3D_LOOPED);
			break;
			
		case SoundBufferPlay_RestartInLoop:
			m_p3DSource->Stop();
			m_p3DSource->Rewind();
			m_p3DSource->Play(A3D_LOOPED);
			break;
			
		case SoundBufferPlay_Restart:
			m_p3DSource->Stop();
			m_p3DSource->Rewind();
			m_p3DSource->Play(A3D_SINGLE);
			break;
			
		case SoundBufferPlay_DestroyAtEnd:
			break;
			
		default:
			break;
		}
	}
}

void csSoundBufferA3D::Stop()
{
	if (m_p3DSource)
		m_p3DSource->Stop();
}

void csSoundBufferA3D::SetVolume(float vol)
{
	fVolume = vol;
	
	if (m_p3DSource)
		m_p3DSource->SetGain(vol);
}

float csSoundBufferA3D::GetVolume()
{
	float vol = fVolume;
	
	if (m_p3DSource)
		m_p3DSource->GetGain(&vol);
	
	return vol;
}

void csSoundBufferA3D::SetFrequencyFactor(float factor)
{
	fFrequencyFactor = factor;
	
	if (m_p3DSource)
		m_p3DSource->SetPitch(factor);
}

float csSoundBufferA3D::GetFrequencyFactor()
{
	float factor = fFrequencyFactor;
	
	if (m_p3DSource)
		m_p3DSource->GetPitch(&factor);
	
	return factor;
}

