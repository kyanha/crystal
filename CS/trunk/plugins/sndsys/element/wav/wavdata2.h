/*
    Copyright (C) 2005 by Andrew Mann
    Based in part on work by Norman Kraemer

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef SNDSYS_DATA_WAV_H
#define SNDSYS_DATA_WAV_H

/**
 * iSndSysData implementation for waveform PCM audio
 */

#include "iutil/databuff.h"
#include "isndsys/ss_structs.h"
#include "isndsys/ss_data.h"


// header of the RIFF-chunk
struct _RIFFchk
{
  char riff_id[4]; // for RIFF-files always "RIFF"
  uint32 len; // length of chunk after this 8 bytes of header
  char wave_id[4]; // for wav-files always "WAVE"
};

// header of the wav-format-chunk
struct _FMTchk
{
  char chunk_id[4]; // for the format-chunk of wav-files always "fmt "
  uint32 len; // length of this chunk after this 8 bytes of header
  uint16 fmt_tag;
  uint16 channel;
  uint32 samples_per_sec;
  uint32 avg_bytes_per_sec;
  uint16 blk_align;
  uint16 bits_per_sample;
};

// header of the wav-data-chunk
struct _WAVchk
{
  char chunk_id[4]; // for wav-data-chunk this is always "data"
  uint32 len; // length of chunk after this 8 bytes of header
};


struct WavDataStore
{
  csRef<iDataBuffer> buf;
  unsigned char *data;
  size_t length;

  WavDataStore (iDataBuffer* buf) : buf(buf)
  {
    data = buf->GetUint8();
    length = buf->GetSize();
  }
};


struct WavStreamData
{
  WavDataStore *datastore;
  int32 position;
};


class SndSysWavSoundData : public iSndSysData
{
 public:
  SCF_DECLARE_IBASE;

  SndSysWavSoundData (iBase *parent, iDataBuffer* data);
  virtual ~SndSysWavSoundData();


  /// Get the format of the sound data.
  virtual const csSndSysSoundFormat *GetFormat();

  /// Get size of this sound in samples.
  virtual size_t GetSampleCount();

  /** 
   * Return the size of the data stored in bytes.  This is informational only 
   * and is not guaranteed to be a number usable for sound calculations.
   * For example, an audio file compressed with variable rate compression may 
   * result in a situation where FILE_SIZE is not equal to 
   * SAMPLE_COUNT * SAMPLE_SIZE since SAMPLE_SIZE may vary throughout the 
   * audio data.
   */
  virtual size_t GetDataSize();

  /**
   * Creates a stream associated with this sound data positioned at the
   * beginning of the sound data and initially paused if possible.
   */
  virtual iSndSysStream *CreateStream(csSndSysSoundFormat *renderformat,
  	int mode3d);

  void Initialize();

  static bool IsWav (iDataBuffer* Buffer);

  static bool ReadHeaders(void *Buffer, size_t len, _RIFFchk *p_riffchk, 
    _FMTchk *p_fmtchk, _WAVchk *p_wavchk, void **data_start, 
    size_t *data_len);

 protected:
  WavDataStore *ds;
  bool data_ready;
  csSndSysSoundFormat fmt;
  size_t sample_count;

  _RIFFchk riffhdr;
  _FMTchk fmthdr;
  _WAVchk wavhdr;
  void *wavedata;
  size_t wavedata_len;

};

#endif // #ifndef SNDSYS_DATA_WAV_H


