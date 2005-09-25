/*
Copyright (C) 2005 by Andrew Mann

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

#ifndef __CS_SNDSYS_COMMON_STRUCTS_H__
#define __CS_SNDSYS_COMMON_STRUCTS_H__

/**\file
 * Sound system: types
 */

#include "csutil/scf.h"

/**\addtogroup sndsys
 * @{ */

/**
 * The sound format. This keeps information about the frequency, bits and
 * channels of a sound data object.
 */
struct csSndSysSoundFormat
{
  /// Frequency of the sound (hz)
  int Freq;
  /// number of bits per sample (8 or 16)
  int Bits;
  /// number of channels (1 or 2)
  int Channels;
};

// A single sound sample will be stored as a signed int
typedef int32 csSoundSample;

#endif // __CS_SNDSYS_COMMON_STRUCTS_H__
