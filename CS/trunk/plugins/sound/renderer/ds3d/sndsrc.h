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

#if !defined(__CSSOUNDSOURCEDS3D_H__)
#define __CSSOUNDSOURCEDS3D_H__

#include "cssfxldr/common/snddata.h"
#include "isndrdr.h"
#include "isndsrc.h"

class csSoundBufferDS3D;

class csSoundSourceDS3D : public ISoundSource
{
public:
	csSoundSourceDS3D();
	virtual ~csSoundSourceDS3D();

	STDMETHODIMP SetPosition(float x, float y, float z);
  STDMETHODIMP SetVelocity(float x, float y, float z);

	STDMETHODIMP GetPosition(float &x, float &y, float &z);
	STDMETHODIMP GetVelocity(float &x, float &y, float &z);

  STDMETHODIMP GetSoundBuffer(ISoundBuffer **sound_buffer);

 	DECLARE_IUNKNOWN()
	DECLARE_INTERFACE_TABLE(csSoundSourceDS3D)

public:
  /// Position of sound object
  float fPosX, fPosY, fPosZ;
  /// Velocity of sound object
  float fVelX, fVelY, fVelZ;

  // SoundBuffer
  csSoundBufferDS3D *pSoundBuffer;

	int DestroySource();
	int CreateSource();

  LPDIRECTSOUND3DBUFFER	m_pDS3DBuffer3D;
  LPDIRECTSOUNDBUFFER	m_pDS3DBuffer2D;
  LPDIRECTSOUND		m_p3DAudioRenderer;
};

#endif // __CSSOUNDSOURCEDS3D_H__
