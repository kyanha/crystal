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

#ifndef __SOUND_RENDER_SOFTWARE_H__
#define __SOUND_RENDER_SOFTWARE_H__

// SoundRender.H
// csSoundRenderSoftware class.

#include "csutil/scf.h"
#include "csutil/csvector.h"
#include "isndrdr.h"
#include "iplugin.h"
#include "isnddata.h"
#include "ivfs.h"

struct iSoundDriver;
struct iSoundData;
struct iSoundLoader;
class csSoundListenerSoftware;
class csSoundSourceSoftware;

class csSoundRenderSoftware : public iSoundRender
{
public:
	DECLARE_IBASE;
	csSoundRenderSoftware(iBase *piBase);
	virtual ~csSoundRenderSoftware();
	
	// implementation of iPlugin
	virtual bool Initialize (iSystem *iSys);
	
	// implementation of iSoundRender
	virtual bool Open ();
	virtual void Close ();
	virtual void SetVolume (float vol);
	virtual float GetVolume ();
        virtual void PlayEphemeral (iSoundData *snd, bool Loop);
	virtual iSoundListener *GetListener ();
	virtual iSoundSource *CreateSource (iSoundData *snd, bool Is3d);
	virtual void MixingFunction ();
    virtual void Update();
    virtual const csSoundFormat *GetLoadFormat();

    // add a sound source
    void AddSource(csSoundSourceSoftware *src);
    void RemoveSource(csSoundSourceSoftware *src);

	// is 16 bit mode device
	bool is16Bits();
	// is a stereo device
	bool isStereo();
	// return frequency
	int getFrequency();
	
private:
    // the config file
    csIniFile *Config;

    // all active sound sources
    csVector Sources;

    // the system driver
	iSystem *System;

    // the low-level sound driver
	iSoundDriver *SoundDriver;

    // memory of the sound driver
	void *memory;
	int memorysize;
	
    // the global listener object
	csSoundListenerSoftware *Listener;

    // the vfs
    iVFS *VFS;

    // format for loading sounds
    csSoundFormat LoadFormat;
};

#endif	//__NETWORK_DRIVER_SOFTWARE_H__

