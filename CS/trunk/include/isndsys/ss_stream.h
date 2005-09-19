/*
    Copyright (C) 2004 by Andrew Mann
    Copyright (C) 1998-2000 by Jorrit Tyberghein

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

#ifndef SNDSYS_STREAM_H
#define SNDSYS_STREAM_H

#include "csutil/scf.h"


struct SndSysSoundFormat;
struct iSndSysData;

SCF_VERSION (iSndSysStream, 0, 1, 0);

#define ISNDSYS_STREAM_UNKNOWN_LENGTH -1

#define ISNDSYS_STREAM_PAUSED     0
#define ISNDSYS_STREAM_UNPAUSED   1

#define ISNDSYS_STREAM_DONTLOOP   0
#define ISNDSYS_STREAM_LOOP       1

/// Every sound stream must be created with one of these 3d modes.
enum
{
  /// Disable 3d effect.
  SND3D_DISABLE=0,
  /// Position of the sound is relative to the listener.
  SND3D_RELATIVE,
  /// Position of the sound is absolute.
  SND3D_ABSOLUTE
};

/**
 * 
 *
 *
 */
struct iSndSysStream : public iBase
{
  /// Get the format of the rendered sound data.  This is for informational purposes only.
  virtual const SndSysSoundFormat *GetRenderedFormat() = 0;

  /// Retrieve the 3D Mode the sound stream was created for
  virtual int Get3dMode() = 0;

  /** Get length of this stream in rendered samples.
   *  May return ISOUND_STREAM_UNKNOWN_LENGTH if the stream is of unknown length.
   *  For example, sound data being streamed from a remote host may not have a pre-determinable length.
   */
  virtual long GetSampleCount() = 0;

  /** Returns the current position of this sound in rendered samples. 
   *  This should return a valid value even if GetSampleCount() returns ISOUND_STREAM_UNKNOWN_LENGTH since an object implementing
   *   this interface should know its position relative to the begining of the data.  In the case where the begining may be ambiguous
   *   it should be considered to be at the point where the stream first started.  In other words, where there is doubt, the position
   *   should start at 0 and advance as the position advances.
   */
  virtual long GetPosition() = 0;

  /** Resets the position of the stream to the begining if possible.
   *  FALSE may be returned if a reset operation is not permitted or not possible.
   */
  virtual bool ResetPosition() = 0;

  /** Sets the position of the stream to a particular sample.
   *  FALSE may be returned if a set position operation is not permitted or not possible.
   */
  virtual bool SetPosition(long newposition) = 0;

  /** Pauses the stream at the current position.  
   *    Data will not be provided through the GetData() call beyond the point of pausing.
   *    AdvancePosition() will NOT adjust the position of the stream.
   *
   *    If either of the above conditions cannot be met, this call fails and returns FALSE.  The sound element should continue
   *     operation as though this call were not made.
   *    For example, a stream coming from a remote host may not be able to be stopped or advance-buffered properly, in this case pausing the stream
   *      is not possible, and this function should return FALSE.
   */
  virtual bool Pause() = 0;


  /** Unpauses the stream and resumes providing data at the current position.
   *  If the stream is not currently paused this function returns FALSE.
   */
  virtual bool Unpause() = 0;

  /** Returns the PAUSE state of the stream:
   *  ISNDSYS_STREAM_PAUSED - The stream is paused.
   *  ISNDSYS_STREAM_UNPAUSED - The stream is not paused.  AdvancePosition is moving the stream position.
   */
  virtual int GetPauseState() = 0;

  /** Sets the loop state of the stream. Current acceptable values are ISNDSYS_STREAM_DONTLOOP and ISNDSYS_STREAM_LOOP
   *  May return FALSE if looping is not supported
   */
  virtual bool SetLoopState(int loopstate) = 0;

  /// Retrieves the loop state of the stream.  Current possible returns are ISNDSYS_STREAM_DONTLOOP and ISNDSYS_STREAM_LOOP
  virtual int GetLoopState() = 0;

  /// Set the playback rate adjustment factor in percent.  100 = 100% (normal speed)
  virtual void SetPlayRatePercent(int percent) = 0;

  /// Retrieves the playback rate adjustment factor in percent.  100 = 100% (normal speed)
  virtual int GetPlayRatePercent() = 0;

  /// If AutoUnregister is set, when the stream is paused it, and all sources attached to it are removed from the sound engine
  virtual void SetAutoUnregister(bool autounreg) = 0;

  /// If AutoUnregister is set, when the stream is paused it, and all sources attached to it are removed from the sound engine
  virtual bool GetAutoUnregister() = 0;

  /// Used by the sound renderer to determine if this stream needs to be unregistered
  virtual bool GetAutoUnregisterRequested() = 0;


  /** NOT AN APPLICATION CALLABLE FUNCTION!   This function advances the stream position based on the provided time value which is considered as a "current time".
   *   A Sound Element will usually store the last advance time internally.  When this function is called it will compare the last time with the time presented
   *   and retrieve more data from the associated iSound2Data container as needed.  It will then update its internal last advance time value.
   *  
   *   This function is not necessarily thread safe and must be called ONLY from the Sound System's main processing thread.
   *
   */
  virtual void AdvancePosition(csTicks current_time) = 0;

  /** NOT AN APPLICATION CALLABLE FUNCTION!  This function is used to retrieve pointers to properly formatted sound data.  
   *
   *  Since a Stream may be attached to multiple Sources, it will be most optimal to perform any decoded-data buffering at the stream level.  The parameters
   *   passed to GetDataPointers() should allow for proper interface to a cyclic buffering method to store this decoded-data.
   *
   *  Since the data in the Stream buffer(s) and data related to how the buffers are accessed may change when AdvancePosition() is called, this function is not
   *   safe to call while AdvancePosition() is in operation.  For this reason it is only safe to be called from the same thread that calls AdvancePosition() - specifically
   *   the Sound System main processing thread.
   *
   *   @param position_marker Should point to a long initially filled by the Sound System internally when a Source is created - through a call to InitializeSourcePositionMarker().
   *   @param max_requested_length Should contain the maximum number of bytes the calling source is interested in receiving.  On return, *buffer1_length + *buffer2_length must not exceed this value.
   *   @param buffer1 should point to a (void *) that will be filled with a pointer to the first chunk of data on return or NULL (0) if no data is available
   *   @param buffer1_length should point to a long that will be filled with the length of valid data in the buffer pointed to by *buffer1 on return.
   *   @param buffer2 should point to a (void *) that will be filled with a pointer to the second chunk of data on return, or NULL (0) if no second chunk is needed.
   *   @param buffer2_length should point to a long that will be filled with the length of valid data in the buffer pointed to by *buffer1 on return.
   *
   */
  virtual void GetDataPointers(long *position_marker,long max_requested_length,void **buffer1,long *buffer1_length,void **buffer2,long *buffer2_length) = 0;



  /// NOT AN APPLICATION CALLABLE FUNCTION!  This function fills a long value that will be used to track a Source's position through calls to GetDataPointers().
  virtual void InitializeSourcePositionMarker(long *position_marker) = 0;

  /// Retrieve a direct pointer to this object
  virtual iSndSysStream *GetPtr() = 0;
};

#endif // #ifndef SNDSYS_STREAM_H


