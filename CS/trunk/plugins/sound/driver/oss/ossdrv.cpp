/*
	Copyright (C) 1998, 1999 by Nathaniel 'NooTe' Saint Martin
	Copyright (C) 1998, 1999 by Jorrit Tyberghein
	Written by Nathaniel 'NooTe' Saint Martin
      Linux sound driver by Gert Steenssens <gsteenss@eps.agfa.be>

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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <signal.h>

#include "cssysdef.h"
#if defined(__NetBSD__)
#  include <soundcard.h>
#elif defined(OS_BSD)
#  include <machine/soundcard.h>
#else
#  include <sys/soundcard.h>
#endif
#include "csutil/scf.h"
#include "ossdrv.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "isys/system.h"
#include "isound/listener.h"
#include "isound/source.h"
#include "isound/renderer.h"
#include "ivaria/reporter.h"
#include "iutil/objreg.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY(csSoundDriverOSS)

SCF_EXPORT_CLASS_TABLE(ossdrv)
  SCF_EXPORT_CLASS(csSoundDriverOSS,
    "crystalspace.sound.driver.oss",
    "OSS Sounddriver for Crystal Space")
SCF_EXPORT_CLASS_TABLE_END;

SCF_IMPLEMENT_IBASE(csSoundDriverOSS)
  SCF_IMPLEMENTS_INTERFACE(iSoundDriver)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iComponent)
SCF_IMPLEMENT_IBASE_END;

SCF_IMPLEMENT_EMBEDDED_IBASE (csSoundDriverOSS::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

int lasterr=0;
int gaudio;
bool inUse=false;
char *err[]=
{
  "no error", "get semaphore", "dec semaphore", "inc semaphore",
  "malloc shmptr", "shmget soundmem", "mark shared mem as removable",
  "mapping shared mem", "detaching shared mem", "msgsnd",
  "semget", "semctl", "get audio queue", "opening audio device",
  "setting samplesize", "setting mono/stereo",
  "setting frequence", "reading blocksize",
  "setting signalhandler", "setting timer", "set fragment size",
  "alloc soundbuffer"
};

#define err_alloc_soundbuffer	21
#define err_set_fragment_size 20
#define err_set_timer 19
#define err_set_handler 18
#define err_get_blocksize 17
#define err_set_frequence 16
#define err_set_stereo 15
#define err_set_samplesize 14
#define err_open_dsp 13
#define err_create_queue 12
#define err_semctl 11
#define err_semget 10
#define err_msgsnd 9
#define err_shm_unmap 8
#define err_shm_map 7
#define err_shmctl_remove 6
#define err_shmget_soundmem 5
#define err_malloc_shmptr 4
#define err_inc_sem 3
#define err_dec_sem 2
#define err_get_sem 1
#define err_no_err 0

bool AudioDeviceBlocked()
{
  // are there free fragments to fill in the soundbuffer ?
  audio_buf_info info;
  ioctl(gaudio, SNDCTL_DSP_GETOSPACE, &info);
  /* Thats what the OSS-specification has to say:
     Number of full fragments that can be read or written without
     blocking.Note that this field is reliable only when the
     application reads/writes full fragments at time.
  */
  return info.fragments == 0;
}

AudioDevice::AudioDevice()
{
  audio = -1;
}

bool AudioDevice::Open(int& frequency, bool& bit16, bool& stereo,
  int& fragments, int& block_size)
{
  int dsp_sample,dsp_stereo,dsp_speed;
  bool succ;

  dsp_speed  = frequency;
  dsp_stereo = stereo;
  dsp_sample = (bit16 ? 16 : 8);

  lasterr=err_open_dsp;
  audio = open(SOUND_DEVICE, O_WRONLY, 0);
  succ = audio != -1;

  if (succ) lasterr=err_set_samplesize;
  succ = succ && ioctl(audio, SNDCTL_DSP_SAMPLESIZE, &dsp_sample) != -1;

  if (succ) lasterr=err_set_stereo;
  succ = succ && ioctl (audio, SNDCTL_DSP_STEREO, &dsp_stereo) != -1;

  if (succ) lasterr=err_set_frequence;
  succ = succ && ioctl (audio, SNDCTL_DSP_SPEED, &dsp_speed) != -1;

  if (succ)
  {
    unsigned int bytes_per_second;
    int frag_size; // fragment size
    int num_frag=0;  // number of fragments
    int hlp;

    // ok, now we know what our soundcard is capable of :) we close it, reopen
    // it and setting the blocksize appropriate for our needs
    close(audio);
    lasterr=err_open_dsp;
    audio = open(SOUND_DEVICE, O_WRONLY, 0);
    succ = audio != -1;
    gaudio = audio;
    // this amount of bytes we pump per second through the audio-device
    bytes_per_second = dsp_speed * (dsp_sample/8) * (dsp_stereo ? 2 : 1);

    // the OSS-specification adivises to use relativly small fragment-sizes
    // (the smallest possible is 256 byte)
    // lets use a fragmentsize of 8192 bytes
    frag_size = 13; // (2^13=8192);
    hlp = bytes_per_second >> frag_size;
    while (hlp)
    {
       num_frag++;
       hlp = hlp >> 1;
    }
//  num_frag++;
    fragments = 1 << num_frag;

    hlp = (num_frag << 16) | frag_size;
    if (succ) lasterr=err_set_fragment_size;
    succ = succ && ioctl (audio, SNDCTL_DSP_SETFRAGMENT, &hlp) != -1;

    if (succ) lasterr=err_set_samplesize;
    succ = succ && ioctl(audio, SNDCTL_DSP_SAMPLESIZE, &dsp_sample) != -1;

    if (succ) lasterr=err_set_stereo;
    succ = succ && ioctl (audio, SNDCTL_DSP_STEREO, &dsp_stereo) != -1;

    if (succ) lasterr=err_set_frequence;
    succ = succ && ioctl (audio, SNDCTL_DSP_SPEED, &dsp_speed) != -1;

    if (succ)
    {
      block_size=8192;
      fprintf(stderr, "block_size: %d\n", block_size);
      frequency = dsp_speed;
      stereo = dsp_stereo;
      bit16 = (dsp_sample == 16 ? true : false);
    }
  }
  return succ;
}

void AudioDevice::Close()
{
  if (audio != -1)
  {
    // close audio device
    close(audio);
    audio = -1;
  }
}

void AudioDevice::Play(unsigned char *snddata, int len)
{
  write(audio, snddata, len);
}

static void* soundptr;

void isTime(int)
{
  iSoundRender *mysound=(iSoundRender*)soundptr;
  if (!AudioDeviceBlocked())
    mysound->MixingFunction();
}

bool csSoundDriverOSS::SetupTimer(int nTimesPerSecond)
{
  struct itimerval itime;
  struct timeval val;
  struct sigaction act;

  val.tv_usec= 1000 / nTimesPerSecond;
  val.tv_sec=0;

  itime.it_value=val;
  itime.it_interval=val;

  act.sa_handler = isTime;
  sigemptyset(&act.sa_mask);
  sigaddset(&act.sa_mask, SIGVTALRM);
  act.sa_flags = SA_NOCLDSTOP | SA_RESTART;

  // set static ptr because the timer handler needs it...
  soundptr=(void*)m_piSoundRender;

  /*
   * Set signal handling function.
   * LINUX-behavior: Dont use signal coz it resets after execution of
   * handlerfunction.  sigaction is a way to make it BSD-like.  NOTE: its not
   * POSIX setting sa_flags to SA_RESTART (from the man page)
   */
  lasterr=err_set_handler;
  bSignalInstalled = (sigaction (SIGVTALRM, &act, &oldact) == 0);

  if (bSignalInstalled) lasterr=err_set_timer;
  // set timer
  bTimerInstalled =
    bSignalInstalled && setitimer (ITIMER_VIRTUAL,&itime,&otime) != -1;
  return bTimerInstalled;
}

csSoundDriverOSS::csSoundDriverOSS(iBase *piBase)
{
  SCF_CONSTRUCT_IBASE(piBase);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  object_reg = NULL;
  m_piSoundRender = NULL;
  memorysize = 0;
  memory = NULL;
  block_size=0;
  block = 0;
  fragments = 0;
  soundbuffer = NULL;
  bSignalInstalled = false;
  bTimerInstalled = false;
}

csSoundDriverOSS::~csSoundDriverOSS()
{
  // if (memory) delete [] memory;
}

bool csSoundDriverOSS::Open(iSoundRender *render, int frequency, bool bit16,
  bool stereo)
{
  csReport (object_reg, CS_REPORTER_SEVERITY_NOTIFY,
  	"crystalspace.sound.oss",
    	"SoundDriver OSS selected");
  m_piSoundRender = render;
  m_bStereo = stereo;
  m_b16Bits = bit16;
  m_nFrequency = frequency;

  bool Active = device.Open(frequency,bit16,stereo, fragments, block_size);
  if (Active)
  {
    lasterr = err_alloc_soundbuffer;
    soundbuffer = new unsigned char[fragments * block_size];
    Active = (soundbuffer != NULL) && SetupTimer(fragments);
  }

  if (!Active)
  {
    perror(err[lasterr]);
    return false;
  }

  return true;
}

void csSoundDriverOSS::Close()
{
  if (bTimerInstalled) setitimer (ITIMER_VIRTUAL, &otime, NULL);
  if (bSignalInstalled) sigaction (SIGVTALRM, &oldact, NULL);
  if (soundbuffer) { delete soundbuffer; soundbuffer=NULL; }
  device.Close();
  memory=NULL;
  memorysize=0;
}

void csSoundDriverOSS::LockMemory(void **mem, int *memsize)
{
  *mem = &soundbuffer[block * block_size];
  *memsize = block_size;
}

void csSoundDriverOSS::UnlockMemory()
{
  // tell device to play this soundblock
  device.Play(&soundbuffer[block * block_size], block_size);
  block = (block+1) % fragments;
}

bool csSoundDriverOSS::IsHandleVoidSound() { return true; }
bool csSoundDriverOSS::IsBackground() { return true; }
bool csSoundDriverOSS::Is16Bits() { return  m_b16Bits; }
bool csSoundDriverOSS::IsStereo() { return m_bStereo; }
int csSoundDriverOSS::GetFrequency() { return m_nFrequency; }
