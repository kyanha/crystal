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

#include "cssysdef.h"
#include "cssys/sysfunc.h"
#include "csavi.h"
#include "avistrv.h"
#include "avistra.h"
#include "iutil/objreg.h"
#include "ivaria/reporter.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE (csAVIFormat)
  SCF_IMPLEMENTS_INTERFACE (iStreamFormat)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csAVIFormat::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csAVIFormat)
SCF_EXPORT_CLASS_TABLE (csavi)
  SCF_EXPORT_CLASS (csAVIFormat, "crystalspace.video.format.avi", 
		"CrystalSpace AVI format interface")
SCF_EXPORT_CLASS_TABLE_END

#define AVI_EVEN(x) ((x)&1?(x)+1:(x))

csAVIFormat::csAVIFormat (iBase *pParent) : 
  len_hcl(8), 
  len_id(4),
  RIFF_ID("RIFF"),
  LIST_ID("LIST"),
  RIFF_AVI("AVI "),
  LIST_HDRL("hdrl"),
  LIST_STRL("strl"),
  LIST_MOVI("movi"),
  LIST_REC( "rec "),
  CHUNK_AVIH("avih"),
  CHUNK_STRH("strh"),
  CHUNK_STRF("strf"),
  CHUNK_STRD("strd"),
  CHUNK_STRN("strn"),
  CHUNK_IDX1("idx1")
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csAVIFormat::~csAVIFormat ()
{
  if (pFile)
  {
    Unload ();
    pFile->DecRef ();
    delete [] pData;
    if (pChunkList) delete pChunkList;
  }
}

void csAVIFormat::Report (int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  iReporter* rep = CS_QUERY_REGISTRY (object_reg, iReporter);
  if (rep)
    rep->ReportV (severity, "crystalspace.video.avi", msg, arg);
  else
  {
    csPrintfV (msg, arg);
    csPrintf ("\n");
  }
  va_end (arg);
}

bool csAVIFormat::Initialize (iObjectRegistry *object_reg)
{
  datalen = 0;

  csAVIFormat::object_reg = object_reg;
  pFile = NULL;
  pData = NULL;
  pChunkList = NULL;

  maxframe = 0; // max. frame visited to date
  maxframepos = NULL;
  pmovi = NULL; // right before movi LIST
  moviendpos = NULL; // right behind last LIST within movi LIST
  startframepos = NULL; // right before 1st LIST within movi LIST
  no_recl = false;

  return true;
}

void csAVIFormat::GetCaps (csStreamFormatCap &caps)
{
  caps = CS_POS_BY_FRAME; // @@@TODO: BY_TIME should be relatively easy to implement
}

bool csAVIFormat::Load (iFile *pVideoData)
{
  if (pFile)
  {
    pFile->DecRef ();
    if (pData) 
      delete [] pData;
    if (pChunkList) 
      delete pChunkList;
    pChunkList = NULL;
    pData = NULL;
    pFile = NULL;
  }
  
  (pFile = pVideoData)->IncRef ();
  size_t r=0;
  datalen = pFile->GetSize ();
  pData = new char[datalen];
  if (pData)
    r = pFile->Read (pData, datalen);

  if (datalen==r)
    return InitVideoData ();

  return false;
}

bool csAVIFormat::InitVideoData ()
{
  bool bSucc;

  // first remove all previous streams
  Unload ();
  p = pData;
  memcpy (&fileheader, p, len_hcl);
  p += len_hcl;
  fileheader.Endian ();
  bSucc = fileheader.Is (RIFF_ID, RIFF_AVI, p);
  if (bSucc)
  {
    if (fileheader.size > datalen)
      Report (CS_REPORTER_SEVERITY_WARNING, 
		       "AVI: RIFF header claims to be longer than the whole file is !");
    bSucc = false;
    p += len_id;
    memcpy (&hdrl, p, len_hcl);
    hdrl.Endian ();
    p += len_hcl;
    if (hdrl.Is (LIST_ID, LIST_HDRL, p))
    {
      pmovi = p + AVI_EVEN(hdrl.size); // audio/video data begins here
      p += len_id;
      // read the AVI header chunk
      memcpy (&avichunk, p, len_hcl);
      avichunk.Endian ();
      if (avichunk.Is (CHUNK_AVIH))
      {
	// read the avi header data
	p += len_hcl;
	memcpy (&aviheader, p, sizeof (AVIHeader));
	aviheader.Endian ();
	p += AVI_EVEN(avichunk.size);
	// now read all streamlists
	for (ULong nStream=0; nStream < aviheader.streamcount; nStream++)
	{
	  memcpy (&strl, p, len_hcl);
	  strl.Endian ();
	  p += len_hcl;
	  char *pListEnd = p+AVI_EVEN(strl.size);
	  if (strl.Is (LIST_ID, LIST_STRL, p))
	  {
	    p += len_id;
	    // read all chunks in the list
	    while (p < pListEnd)
	    {
	      memcpy (&strh, p, len_hcl);
	      strh.Endian ();
	      if (strh.Is (CHUNK_STRH))
	      {
		nStream++;
		p += len_hcl;
		memcpy (&streamheader, p, sizeof (StreamHeader));
		streamheader.Endian ();
		p += AVI_EVEN(strh.size);
		ULong nread = CreateStream (&streamheader);
		if (nread == 0)
		{
		  // error ! we skip the remainder of the stream LIST
		  p = pListEnd;
		}
	      }
	      else
	      {
		p += AVI_EVEN(strh.size) + len_hcl;
	      }
	    }
	  }
	  else
	  {
	    Report (CS_REPORTER_SEVERITY_DEBUG, "unrecognized LIST type \"%4c\" .. skipping it !", strl.id);
	  }
	  p = pListEnd;
	}
	p = pmovi;
	// now skip through all lists until we find the <movi> list
	hcl movi;
	while (p < pData+fileheader.size)
	{
	  memcpy (&movi, p, len_hcl);
	  movi.Endian ();
	  p += len_hcl;
	  if (!movi.Is (LIST_ID, LIST_MOVI, p))
	    p += AVI_EVEN(movi.size);
	  else
	    break;
	}
	// here is the <movi> LIST expected
	if (movi.Is (LIST_ID, LIST_MOVI, p))
	{
	  startframepos = p + len_id;
	  moviendpos = p + AVI_EVEN(movi.size);
	  maxframe = 0;
	  maxframepos = startframepos;
	  // some AVI with only one stream do not enclose the chunks in a <rec> LIST but
	  // sequentially write <##??> chunks
	  p += len_id;
	  memcpy (&movi, p, sizeof (hcl));
	  p += len_hcl;
	  if (!movi.Is (LIST_ID, LIST_REC, p))
	    // we have found such a beast (or the AVI is corrupt :)
	    no_recl = true;
	  else
	    no_recl = false;

	  // optionally we may find an idx1 list here
	  if ((aviheader.flags & AVIHeader::HASINDEX) && pData + datalen > moviendpos+len_hcl)
	  {
	    memcpy (&movi, moviendpos, len_hcl);
	    movi.Endian ();
	    if (movi.Is (CHUNK_IDX1))
	    {
	      pChunkList = new ChunkList (startframepos - len_id);
	      pChunkList->LoadList ((UByte*)(moviendpos + len_hcl), movi.size);
	    }
	  }
	}
      }
      else
	Report (CS_REPORTER_SEVERITY_WARNING, "No <avih> chunk found !");
    }
    else
      Report (CS_REPORTER_SEVERITY_WARNING, "No <hdrl> LIST found !");
  }
  else
    Report (CS_REPORTER_SEVERITY_WARNING, "No RIFF header found !");

  // did we find a video stream ?
  // first check the validity of the streams found
  if (!ValidateStreams ())
    Report (CS_REPORTER_SEVERITY_WARNING, "No valid videostream found !");
  return vStream.Length () > 0;
}

void csAVIFormat::Unload ()
{
  int i;
  for (i=0; i < vStream.Length (); i++)
    ((iStream*)vStream.Get (i))->DecRef ();
  vStream.DeleteAll ();

  pAudio = NULL;
  pVideo = NULL;
  nAudio = 0;
  nVideo = 0;
}

bool csAVIFormat::ValidateStreams ()
{
  // a valid avi file contains one video stream and zero or more audiostreams
  csStreamDescription desc;
  iStreamIterator *it = GetStreamIterator ();
  int nAudio = 0, nVideo = 0;
  while (it->HasNext())
  {
    it->GetNext ()->GetStreamDescription (desc);
    if (desc.type == CS_STREAMTYPE_AUDIO) 
      nAudio++;
    else
    if (desc.type == CS_STREAMTYPE_VIDEO) 
      nVideo++;
  }
  it->DecRef ();
  return nAudio >= 0 && nVideo == 1;
}

ULong csAVIFormat::CreateStream (StreamHeader *streamheader)
{
  ULong n=0;
  UByte *pCID=NULL;
  char *pName = NULL;
  ULong nCIDLen = 0;
  UByte *pFormatEx;
  ULong nFormatEx;

  if (!strncmp (streamheader->type, "auds", 4))
  {
    // make an audio stream
    csAVIStreamAudio *pAudioStream = new csAVIStreamAudio (this);
    
    memcpy (&strh, p, len_hcl);
    strh.Endian ();
    if (strh.Is (CHUNK_STRF))
    {
      p += len_hcl;
      memcpy (&audsf, p, sizeof (AudioStreamFormat));
      audsf.Endian ();
      p += AVI_EVEN(strh.size);
      pFormatEx = (UByte*)p;
      nFormatEx = sizeof (audsf) + sizeof (audsf.extra);
      n = AVI_EVEN(strh.size) + len_hcl;
      // optionally follows a "strd" chunk containing codec specific data which is handed
      // over to the codec 'as is'
      memcpy (&strh, p, len_hcl);
      strh.Endian ();
      if (strh.Is (CHUNK_STRD))
      {
	p += len_hcl;
	pCID = (UByte*)p;
	nCIDLen = strh.size;
	p += AVI_EVEN(strh.size);
	n += AVI_EVEN(strh.size) + len_hcl;
	memcpy (&strh, p, len_hcl);
	strh.Endian ();
      }
      // here may come a "strn" chunk containing a zeroterminated string which is the name
      // of the stream
      if (strh.Is (CHUNK_STRN))
      {
	p += len_hcl;
	pName = p;
	p += AVI_EVEN(strh.size);
	n += AVI_EVEN(strh.size) + len_hcl;
      }
      if (pAudioStream->Initialize (&aviheader, streamheader, &audsf, nAudio, 
				    pCID, nCIDLen, pName, pFormatEx, nFormatEx, object_reg))
	vStream.Push (pAudioStream);
      else
	pAudioStream->DecRef ();
    }
    nAudio++;
  }
  else
  if (!strncmp (streamheader->type, "vids", 4))
  {
    // make an video stream
    csAVIStreamVideo *pVideoStream = new csAVIStreamVideo (this);
    
    memcpy (&strh, p, len_hcl);
    strh.Endian ();
    if (strh.Is (CHUNK_STRF))
    {
      p += len_hcl;
      memcpy (&vidsf, p, sizeof (VideoStreamFormat));
      vidsf.Endian ();
      pFormatEx = (UByte*)p;
      nFormatEx = sizeof(vidsf) + vidsf.size;
      p += AVI_EVEN(strh.size);
      n = AVI_EVEN(strh.size) + len_hcl;
      // optionally follows a "strd" chunk containing codec specific data which is handed
      // over to the codec 'as is'
      memcpy (&strh, p, len_hcl);
      strh.Endian ();
      if (strh.Is (CHUNK_STRD))
      {
	p += len_hcl;
	pCID = (UByte*)p;
	nCIDLen = strh.size;
	p += AVI_EVEN(strh.size);
	n += AVI_EVEN(strh.size) + len_hcl;
	memcpy (&strh, p, len_hcl);
	strh.Endian ();
      }
      // here may come a "strn" chunk containing a zeroterminated string which is the name
      // of the stream
      if (strh.Is (CHUNK_STRN))
      {
	p += len_hcl;
	pName = p;
	p += AVI_EVEN(strh.size);
	n += AVI_EVEN(strh.size) + len_hcl;
      }
      if (pVideoStream->Initialize (&aviheader, streamheader, &vidsf, nVideo, 
				    pCID, nCIDLen, pName, pFormatEx, nFormatEx, object_reg))
      vStream.Push (pVideoStream);
      else
	pVideoStream->DecRef ();
    }
    nVideo++;
  }
  else
  {
    // assume this is just a format we dont know, so simply skip it
    memcpy (&strh, p, len_hcl);
    strh.Endian ();
    if (!strh.Is (CHUNK_STRF))
    {
      Report (CS_REPORTER_SEVERITY_WARNING, "Unsupported streamtype \"%4c\" found ... ignoring it !", 
		       strh.id);
      n = AVI_EVEN(strh.size) + len_hcl;
      p += AVI_EVEN(strh.size) + len_hcl;
    }
    else
    {
      // hmm, not only we dont know this stream type, it does not have an format header either.
      // this rather indicates an error. We should skip the remaining of the stream LIST.
      // we force this by returning 0.
    }
  }
  return n;
}

bool csAVIFormat::HasChunk (ULong id, ULong frameindex)
{
  bool bSucc = false;
  if (pChunkList)
    bSucc = pChunkList->HasChunk (id, frameindex);
  else
  {
    // bad, no chunklist. skip through the data and count it down
    if (frameindex < maxframe)
      bSucc = true;
    else
    {
      char *pp = maxframepos;
      hcl ch;
	  ch.size=0; // to stop MSVC moaning
      while (pp < moviendpos && maxframe <= frameindex)
      {
	memcpy (&ch, pp, len_hcl);
	ch.Endian ();
	pp += AVI_EVEN(ch.size) + len_hcl;
	maxframepos = pp;
	maxframe++;
      }
      bSucc = (maxframe-1 == frameindex && pp <= moviendpos);
      if (!bSucc)
      {
	if (pp>moviendpos)
	{
	  maxframepos -= (AVI_EVEN(ch.size) + len_hcl);
	  maxframe--;
	}
      }
    }
  }
  return bSucc;
}
 
bool csAVIFormat::GetChunk (ULong frameindex, AVIDataChunk *pChunk)
{
  char *pp = NULL;
  ULong maxsize=0, n=0;

  if (HasChunk (*(ULong*)pChunk->id, frameindex))
  {
    if (!pChunk->currentframepos)
      pChunk->currentframepos = startframepos;
    if (pChunkList)
    {
      bool succ = pChunkList->GetPos (*(ULong*)pChunk->id, frameindex, 
				      pp, pChunk->length);
      pChunk->data = (csSome)(pp + len_hcl);
      pChunk->currentframe = frameindex;
      return succ;
    }
    else
    {  // no index list
      ULong startfrom = (frameindex < (ULong)pChunk->currentframe ? 0 
			 : frameindex <= maxframe ? pChunk->currentframe : maxframe);
      pp = (frameindex < (ULong)pChunk->currentframe ? startframepos
	    : frameindex <= maxframe ? pChunk->currentframepos : maxframepos);
    
      if (!no_recl)
      {
	hcl ch;
	ch.size=0; // to stop MSVC moaning
	while (startfrom <= frameindex)
	{
	  memcpy (&ch, pp, len_hcl);
	  ch.Endian ();
	  pp += AVI_EVEN(ch.size) + len_hcl;
	  startfrom++;
	}
	pChunk->currentframepos = pp - (AVI_EVEN(ch.size) + len_hcl);
	pChunk->currentframe = frameindex;
	pp = pp - AVI_EVEN(ch.size) + len_id;
	maxsize = AVI_EVEN(ch.size);
      }
      else 
      { // no_recl == true
	hcl ch;
	ch.size=0; // to stop MSVC moaning
	while (startfrom <= frameindex)
	{
	  memcpy (&ch, pp, len_hcl);
	  ch.Endian ();
	  pp += AVI_EVEN(ch.size) + len_hcl;
	  startfrom++;
	}
	pp -= (AVI_EVEN(ch.size) + len_hcl);
	pChunk->currentframepos = pp;
	pChunk->currentframe = frameindex;
	maxsize = AVI_EVEN(ch.size) + len_hcl;
      }
    }

    hcl chunk;

    while (n<=maxsize)
    {
      memcpy (&chunk, pp, len_hcl);
      chunk.Endian ();
      if (!strncmp (pChunk->id, chunk.id, 4))
      {
	pChunk->data = pp + len_hcl;
	pChunk->length = chunk.size;
	return true;
      }
      else
      {
	pp += AVI_EVEN(chunk.size) + len_hcl;
	n  += AVI_EVEN(chunk.size) + len_hcl;
      }
    }
  }
  return false;
}

UShort csAVIFormat::stream_number (const char c1, const char c2)
{
#define AVI_DECODE_HEX(c) ((c)<='9'?(c)-'0':((c)<='F'?(c)-'A'+10:(c)-'a'+10))

  UShort num = AVI_DECODE_HEX(c1);
  num <<=4;
  num += AVI_DECODE_HEX(c2);

#undef AVI_DECODE_HEX
  return num;
}

iStreamIterator* csAVIFormat::GetStreamIterator ()
{
  return new streamiterator (this);
}

void csAVIFormat::Select (iAudioStream *pAudio, iVideoStream *pVideo)
{
  this->pAudio = pAudio;
  this->pVideo = pVideo;
}

void csAVIFormat::NextFrame ()
{
  pAudio->NextFrame ();
  pVideo->NextFrame ();
}

SCF_IMPLEMENT_IBASE (csAVIFormat::streamiterator)
  SCF_IMPLEMENTS_INTERFACE (iStreamIterator)
  SCF_IMPLEMENTS_INTERFACE (iBase)
SCF_IMPLEMENT_IBASE_END

csAVIFormat::streamiterator::streamiterator (iBase *pBase)
{
  SCF_CONSTRUCT_IBASE (pBase);
   pAVI = (csAVIFormat*)pBase;
   pos = 0;
}

csAVIFormat::streamiterator::~streamiterator ()
{
	
}

bool csAVIFormat::streamiterator::HasNext ()
{
  return pos < pAVI->vStream.Length ();
}

iStream* csAVIFormat::streamiterator::GetNext ()
{
  if (HasNext ())
    return (iStream*)pAVI->vStream.Get (pos++);
   return NULL;
}


void csAVIFormat::ChunkList::LoadList (UByte *data, ULong length)
{
  ULong i, nEntries = length / sizeof(indexentry);
  indexentry *ie = (indexentry*)data;
  int idx;

  for (i = 0; i < nEntries; i++)
  {
    ie->Endian ();
    idx = streamlist.FindKey ((csSome)ie->id);
    if (idx == -1)
      idx = streamlist.Push (new StreamIdx (ie->id));
    streamlist.Get (idx)->Push (ie);
    ie++;
  }
}

bool csAVIFormat::ChunkList::HasChunk (ULong id, ULong idx)
{
  int i = streamlist.FindKey ((csSome)id);
  return (i == -1 ? false : idx < (ULong)streamlist.Get (i)->Length ());
}

bool csAVIFormat::ChunkList::GetPos (ULong id, ULong idx, char *&pos, ULong &size)
{
  int i = streamlist.FindKey ((csSome)id);
  if (i != -1)
  {
    indexentry *ie = streamlist.Get (i)->Get (idx);
    pos = start + ie->offset;
    size = ie->length;
    return true;
  }
  return false;
}
