/*
    Copyright (C) 2001 by Norman Kr�mer

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

#ifndef __CS_CSAVI_H__
#define __CS_CSAVI_H__

#include "ivideo/codec.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "iutil/vfs.h"
#include "csutil/csvector.h"
#include "cssys/csendian.h"

struct iObjectRegistry;
class csAVIStreamVideo;
class csAVIStreamAudio;

class csAVIFormat : public iStreamFormat
{
 protected:
  class streamiterator : public iStreamIterator
  {
    friend class csAVIFormat;
  protected:
    streamiterator (iBase *pBase);
    csAVIFormat *pAVI;
    int pos;
  public:
    SCF_DECLARE_IBASE;
    virtual ~streamiterator ();
    bool HasNext ();
    iStream *Next ();
  };

  // @@@TODO: handle optional chunklists in AVI files
  class ChunkList
  {
    enum {
      LIST     = 0x00000001, // the chunk is a list
      KEYFRAME = 0x00000010, // frame is a keyframe
      NOTIME   = 0x00000100, // dont use this frame for time calculations - it takes no time
                             // (palette change for example)
      COMPUSE  = 0x0fff0000  // masks the bits used for compression
    };

    struct indexentry
    {
      uint32 id;
      uint32 flags;
      uint32 offset;
      uint32 length;
      void Endian ()
      {
		id = little_endian_long (id);
		flags = little_endian_long (flags);
		offset = little_endian_long (offset);
		length = little_endian_long (length);
      }
    };

    class StreamIdx : public csVector
    {
    public:
      uint32 id;
    public:
      StreamIdx (uint32 id) : csVector (8,8) {this->id = id;}
      virtual ~StreamIdx (){}
      virtual indexentry *Get (int idx)const {return (indexentry*)csVector::Get(idx); }
    };

    class StreamList : public csVector
    {
    public:
      StreamList () : csVector (8, 8){}
      virtual ~StreamList () {}
      virtual int Compare (void* Item1, void* Item2, int) const
      {
	StreamIdx *i1 = (StreamIdx*)Item1, *i2 = (StreamIdx*)Item2;
        return (i1->id < i2->id ? -1 : i1->id > i2->id ? 1 : 0);
      }
      virtual int CompareKey (void* Item1, const void* Item2, int) const
      {
	StreamIdx *i1 = (StreamIdx*)Item1;
	uint32 id = (uint32)Item2;
        return (i1->id < id ? -1 : i1->id > id ? 1 : 0);
      }
      virtual StreamIdx *Get (int idx)const {return (StreamIdx*)csVector::Get(idx); }
      virtual bool FreeItem (void* Item) { delete (StreamIdx*)Item; return true; }
    };

    StreamList streamlist;
    char *start;
  public:
    ChunkList (char *start) {this->start = start;}
    void LoadList (uint8 *data, uint32 length);
    bool HasChunk (uint32 id, uint32 idx);
    bool GetPos (uint32 id, uint32 idx, char *&pos, uint32 &size);
  };

  friend class streamiterator;
  friend class csAVIStreamVideo;
  friend class csAVIStreamAudio;

  struct AVIDataChunk
  {
    char id[5];
    int32 currentframe;
    char *currentframepos;
    void *data;
    uint32 length; // in byte
  };

  // this is used to read in the id and length of an AVI fileheader,
  // list or chunk
  struct hcl
  {
    char id[4];
    uint32 size;
    void Endian (){ size = little_endian_long (size); }
    bool Is (const char *theID, const char* theType, const char* p)
      {return !strncmp (id, theID, 4) && !strncmp (p, theType, 4);}
    bool Is (const char* theID)
      {return !strncmp (id, theID, 4);}
  };

  const int len_hcl;
  const int len_id;
  const char *RIFF_ID;
  const char *LIST_ID;

  // recognized RIFF types
  const char *RIFF_AVI;
  // recognized LIST types
  const char *LIST_HDRL;
  const char *LIST_STRL;
  const char *LIST_MOVI;
  const char *LIST_REC;
  // recognized chunk types
  const char *CHUNK_AVIH;
  const char *CHUNK_STRH;
  const char *CHUNK_STRF;
  const char *CHUNK_STRD;
  const char *CHUNK_STRN;
  const char *CHUNK_IDX1;

  struct AVIHeader
  {
    enum {
      HASINDEX       = 0x00000010,
      MUSTUSEINDEX   = 0x00000020,
      ISINTERLEAVED  = 0x00000100,
      TRUSTCKTYPE    = 0x00000800,
      WASCAPTUREFILE = 0x00010000,
      COPYRIGHTED    = 0x00020000
    };
    uint32 msecperframe; // milliseconds per frame
    uint32 maxbytespersec; // max. transfer rate in bytes/sec
    uint32 padsize;
    uint32 flags;
    uint32 framecount;
    uint32 initialframecount;
    uint32 streamcount;
    uint32 suggestedbuffersize;
    uint32 width;
    uint32 height;
    uint32 reserved[4];
    void Endian ()
    {
      msecperframe = little_endian_long (msecperframe);
      maxbytespersec = little_endian_long (maxbytespersec);
      padsize = little_endian_long (padsize);
      flags = little_endian_long (flags);
      framecount = little_endian_long (framecount);
      initialframecount = little_endian_long (initialframecount);
      streamcount = little_endian_long (streamcount);
      suggestedbuffersize = little_endian_long (suggestedbuffersize);
      width = little_endian_long (width);
      height = little_endian_long (height);
    }
  };
  struct StreamHeader
  {
    char type[4];
    char handler[4];
    uint32 flags;
    uint16 priority;
    uint16 language;
    uint32 initialframecount;
    uint32 scale;
    uint32 rate;
    uint32 start;
    uint32 length;
    uint32 suggestedbuffersize;
    uint32 quality;
    uint32 samplesize;
    int32 left, top, right, bottom;
    void Endian ()
    {
      flags = little_endian_long (flags);
      priority = little_endian_short (priority);
      language = little_endian_short (language);
      initialframecount = little_endian_long (initialframecount);
      scale = little_endian_long (scale);
      rate = little_endian_long (rate);
      start = little_endian_long (start);
      length = little_endian_long (length);
      suggestedbuffersize = little_endian_long (suggestedbuffersize);
      quality = little_endian_long (quality);
      samplesize = little_endian_long (samplesize);
      top = little_endian_long (top);
      left = little_endian_long (left);
      right = little_endian_long (right);
      bottom = little_endian_long (bottom);
    }
  };

  struct VideoStreamFormat
  {
    uint32 size;
    uint32 width;
    uint32 height;
    uint16 planes;
    uint16 bitcount;
    uint32 compression;
    uint32 sizeimage;
    uint32 xpelspermeter;
    uint32 ypelspermeter;
    uint32 colorsused;
    uint32 colorsimportant;
    void Endian ()
    {
      size = little_endian_long (size);
      width = little_endian_long (width);
      height = little_endian_long (height);
      planes = little_endian_short (planes);
      bitcount = little_endian_short (bitcount);
      compression = little_endian_long (compression);
      sizeimage = little_endian_long (sizeimage);
      xpelspermeter = little_endian_long (xpelspermeter);
      ypelspermeter = little_endian_long (ypelspermeter);
      colorsused = little_endian_long (colorsused);
      colorsimportant = little_endian_long (colorsimportant);
    }
  };

  struct AudioStreamFormat
  {
    uint16 formattag;
    uint16 channels;
    uint32 samplespersecond;
    uint32 avgbytespersecond;
    uint16 blockalign;
    uint16 bitspersample;
    uint16 extra;
    void Endian ()
    {
      formattag = little_endian_short (formattag);
      channels = little_endian_short (channels);
      samplespersecond = little_endian_long (samplespersecond);
      avgbytespersecond = little_endian_long (avgbytespersecond);
      blockalign = little_endian_short (blockalign);
      bitspersample = little_endian_short (bitspersample);
      extra = little_endian_short (extra);
    }
  };
 protected:
  size_t datalen;

  iObjectRegistry *object_reg;
  iFile *pFile;

  char *pData, *p;
  uint16 nAudio, nVideo; // streamcounter
  uint32 maxframe; // max. frame visited to date
  char *maxframepos;
  char *pmovi; // right before movi LIST
  char *moviendpos; // right behind last LIST within movi LIST
  char *startframepos; // right before 1st LIST within movi LIST
  bool no_recl;

  hcl fileheader;
  hcl hdrl, strl;
  hcl avih, strh, avichunk;
  ChunkList *pChunkList;

  AVIHeader aviheader;
  StreamHeader streamheader;
  AudioStreamFormat audsf;
  VideoStreamFormat vidsf;

  csVector vStream;
  iAudioStream *pAudio;
  iVideoStream *pVideo;

  bool InitVideoData ();
  bool ValidateStreams ();
  uint32 CreateStream (StreamHeader *streamheader);
  bool HasChunk (uint32 id, uint32 frameindex);
  bool GetChunk (uint32 frameindex, AVIDataChunk *pChunk);
  uint16 stream_number (const char c1, const char c2);

 public:
  SCF_DECLARE_IBASE;

  csAVIFormat (iBase *pParent);
  virtual ~csAVIFormat ();

  virtual bool Initialize (iObjectRegistry *object_reg);

  void Report (int severity, const char* msg, ...);

  virtual void GetCaps (csStreamFormatCap &caps);
  virtual iStreamIterator* GetStreamIterator ();
  virtual void Select (iAudioStream *pAudio, iVideoStream *pVideo);
  virtual void NextFrame ();
  virtual bool Load (iFile *pVideoData);
  virtual void Unload ();

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csAVIFormat);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize(p); }
  } scfiComponent;
};
#endif // __CS_CSAVI_H__
