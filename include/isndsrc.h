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

#ifndef __ISNDSRC_H__
#define __ISNDSRC_H__

#include "csutil/scf.h"
#include "csgeom/vector3.h"

// these are flags and can be or'd
#define SOUND_RESTART   1
#define SOUND_LOOP      2

SCF_VERSION (iSoundSource, 0, 0, 1);

/**
 * The sound source is used to play a sound stream. It can be a non-3d source,
 * in which case it plays the sound as it was recorded, or a 3d source, in
 * which case it represents an object in 3d space and adjusts L/R volume for
 * 3d sound.
 */
struct iSoundSource : public iBase
{
  /// Play the sound
  virtual void Play (unsigned long playMethod = 0) = 0;
  /// Stop the sound
  virtual void Stop () = 0;
  /// Set volume
  virtual void SetVolume (float volume) = 0;
  /// Get volume
  virtual float GetVolume () = 0;
  /// Set frequency factor : 1 = normal, >1 faster, 0-1 slower
  virtual void SetFrequencyFactor (float factor) = 0;
  /// Get frequency factor
  virtual float GetFrequencyFactor () = 0;

  /// is this a 3d source?
  virtual bool Is3d() = 0;
  /// set position of this source. Only used in 3d sources
  virtual void SetPosition(csVector3 pos) = 0;
  /// get position of this source. Only used in 3d sources
  virtual csVector3 GetPosition() = 0;
  /// set velocity of this source. Only used in 3d sources
  virtual void SetVelocity(csVector3 spd) = 0;
  /// get velocity of this source. Only used in 3d sources
  virtual csVector3 GetVelocity() = 0;
};

#endif
