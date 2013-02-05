/*
Copyright (C) 2011 by Alin Baciu

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
#include "iutil/objreg.h"
#include "iutil/plugin.h"
#include "iutil/vfs.h"

// Theora headers
#include "csutil/custom_new_disable.h"
#include <theora/theoradec.h>
#include <vorbis/codec.h>
#include "csutil/custom_new_enable.h"

#include "theoraloader.h"

SCF_IMPLEMENT_FACTORY (csThOggLoader)

csThOggLoader::csThOggLoader (iBase* parent) :
scfImplementationType (this, parent),
_object_reg (0)
{
}

csThOggLoader::~csThOggLoader ()
{
}

bool csThOggLoader::Initialize (iObjectRegistry* r)
{
  _object_reg = r;
  _infile = nullptr;

  return true;
}

csPtr<iMediaContainer> csThOggLoader::LoadMedia (const char* pFileName,
                                                 const char *pDescription)
{
  _path = pFileName;

#ifdef CS_DEBUG
  printf ("\nLoading media file '%s'\n", pFileName);
#endif

  _g3d=csQueryRegistry<iGraphics3D> (_object_reg);
  _texManager = _g3d->GetTextureManager ();

  // Get an iMediaParser from the object registry
  csRef<iVFS> vfs = csQueryRegistry<iVFS> (_object_reg);

  // Get the path for the video
  csRef<iDataBuffer> vidPath = vfs->GetRealPath (pFileName);
  _infile = fopen (vidPath->GetData (),"rb");

  // Check if the file exists
  if (!_infile)
  {
    csReport (_object_reg, CS_REPORTER_SEVERITY_WARNING, QUALIFIED_PLUGIN_NAME,
      "Unable to open '%s' for playback.\n", pFileName);
    return csPtr<iMediaContainer> (nullptr);
  }
  else
  {
    // Create a new container
    csRef<TheoraMediaContainer> container;
    container.AttachNew (new TheoraMediaContainer ((iBase*) this));
    container->Initialize (_object_reg);

    if (!StartParsing (container))
      return csPtr<iMediaContainer> (nullptr);

    container->SetDescription (pDescription);
    return csPtr<iMediaContainer> (container);
  }
}

int csThOggLoader::BufferData (ogg_sync_state *oy)
{
  char *buffer=ogg_sync_buffer (oy,4096);
  int bytes=fread (buffer,1,4096,_infile);
  ogg_sync_wrote (oy,bytes);
  return (bytes);
}

bool csThOggLoader::StartParsing (TheoraMediaContainer* container)
{
  // Start up Ogg stream synchronization layer
  ogg_sync_init (&_oy);

  // Init supporting Vorbis structures needed in header parsing
  vorbis_info_init (&_vi);
  vorbis_comment_init (&_vc);

  // Init supporting Theora structures needed in header parsing
  th_comment_init (&tc);
  th_info_init (&ti);

  // Copy the file pointer in the container
  container->_infile=_infile;

  return ParseHeaders (container);
}

bool csThOggLoader::ParseHeaders (TheoraMediaContainer* container)
{
  fseek (_infile,0,SEEK_END);
  unsigned long mSize = ftell (_infile);
  fseek (_infile,0,SEEK_SET);
  container->SetFileSize (mSize);

  ogg_packet op;
  th_setup_info *ts = 0;
  int stateflag = 0;
//  int audioCnt = 0;

  // Since we're loading a Theora video, we expect video to be present
  // otherwise, everything will be dropped.
  bool foundVideo = false;

  // Parse the headers
  // Only interested in Vorbis/Theora streams
  while (!stateflag)
  {
    int ret = BufferData (&_oy);
    if (ret == 0)
      break;

    while (ogg_sync_pageout (&_oy,&_og) > 0)
    {
      ogg_stream_state test;

      // Is this a mandated initial header? If not, stop parsing
      if (!ogg_page_bos (&_og))
      {
        // Don't leak the page; get it into the appropriate stream
        container->QueuePage (&_og);
        stateflag = 1;
        break;
      }

      ogg_stream_init (&test,ogg_page_serialno (&_og));
      ogg_stream_pagein (&test,&_og);
      ogg_stream_packetout (&test,&op);

      // Identify the codec
      if (th_decode_headerin (&ti,&tc,&ts,&op) >= 0)
      {
        // It is Theora
        if (foundVideo)
          continue;

        csString name (_path);
        name.Append ("_videostream");
        csRef<csTheoraVideoMedia> videoStream;
        videoStream.AttachNew ( new csTheoraVideoMedia ((iBase*)this) );
        videoStream->InitializeStream (name.GetData (), test, ti, tc, ts, _infile, _texManager);
        container->AddMedia (videoStream);
        foundVideo = true;

        // Reinitialize Theora structures for next stream, if there is one
        th_comment_init (&tc);
        th_info_init (&ti);
      }
      else
        if (vorbis_synthesis_headerin (&_vi,&_vc,&op) == 0)
        {
          // It is Vorbis

          // TODO: support for Vorbis audio stream
          // Debug this part of code that triggers a segfault
          /*
          audioCnt++;
          csString name (_path);
          name.Append ("_audiostream_").Append (audioCnt);

          csRef<csTheoraAudioMedia> audioStream;
          audioStream.AttachNew ( new csTheoraAudioMedia ((iBase*)this) );
          audioStream->InitializeStream (name.GetData (), test, _vi, _vc, _infile);
          container->AddMedia (audioStream);

          // Reinitialize Vorbis structures for next stream, if there is one
          vorbis_info_init (&_vi);
          vorbis_comment_init (&_vc);
          */
        }
        else
        {
          // Whatever it is, we don't care about it
          ogg_stream_clear (&test);
        }
    }

  }

  // If there isn't a Theora video stream in the file, we don't care anymore
  if (!foundVideo)
  {
    // Clear the sync state
    ogg_sync_clear (&_oy);

    // Post a warning
    csReport (_object_reg, CS_REPORTER_SEVERITY_WARNING, QUALIFIED_PLUGIN_NAME,
      "Unable to find a Theora video stream.\n");

    return false;
  }

  // Next, parse secondary headers.
  size_t times=0;
  while (times!=container->GetMediaCount ())
  {
    int ret;
    for (uint i=0;i<container->GetMediaCount ();i++)
    {
      // Read extra headers for video
      if (strcmp (container->GetMedia (i)->GetType (), "TheoraVideo")==0)
      {
        csRef<iVideoMedia> media = scfQueryInterface<iVideoMedia> (container->GetMedia (i));
        if (media.IsValid ())
        {
          csRef<csTheoraVideoMedia> buff = static_cast<csTheoraVideoMedia*> ( (iVideoMedia*)media);

          while (buff->Theora_p () && (buff->Theora_p ()<3) && (ret=ogg_stream_packetpeek (buff->StreamState (),&op)))
          {
            if (ret<0)
            {
              csReport (_object_reg, CS_REPORTER_SEVERITY_ERROR, QUALIFIED_PLUGIN_NAME,
                "Error parsing Theora stream headers; corrupt stream?\n");
              return false;
            }
            int res=th_decode_headerin (buff->StreamInfo (), buff->StreamComments (),buff->SetupInfo (),&op);
            if (res>0)
            {
              ogg_stream_packetout (buff->StreamState (),&op);
            }
            else
            {
              csReport (_object_reg, CS_REPORTER_SEVERITY_ERROR, QUALIFIED_PLUGIN_NAME,
                "Error parsing Theora stream headers; corrupt stream?\n");
              return false;
            }
            buff->Theora_p ()++;
            if (buff->Theora_p ()==3)
            {

              th_comment_clear (&tc);
              th_info_clear (&ti);
              times++;
              break;
            }
          }

        }
      }

      // Read extra headers for audio
      if (strcmp (container->GetMedia (i)->GetType (), "TheoraAudio") == 0)
      {
	csMedia* media = dynamic_cast<csMedia*> (container->GetMedia (i));
	csTheoraAudioMedia* buff = static_cast<csTheoraAudioMedia*> (media);

	while (buff->Vorbis_p () && (buff->Vorbis_p ()<3) && (ret=ogg_stream_packetout (buff->StreamState (),&op)))
	{
	  if (ret<0)
	  {
	    csReport (_object_reg, CS_REPORTER_SEVERITY_ERROR, QUALIFIED_PLUGIN_NAME,
		      "Error parsing Vorbis stream headers; corrupt stream?\n");
	    return false;
	  }
	  if (vorbis_synthesis_headerin (buff->StreamInfo (),buff->StreamComments (),&op))
	  {
	    csReport (_object_reg, CS_REPORTER_SEVERITY_ERROR, QUALIFIED_PLUGIN_NAME,
		      "Error parsing Vorbis stream headers; corrupt stream?\n");
	    return false;
	  }
	  buff->Vorbis_p ()++;
	  if (buff->Vorbis_p ()==3)
	  {

	    vorbis_comment_clear (&_vc);
	    vorbis_info_clear (&_vi);
	    times++;
	    break;
	  }
	}
      }
    }

    if (ogg_sync_pageout (&_oy,&_og)>0)
    {
      container->QueuePage (&_og); // demux into the appropriate stream
    }
    else
    {
      int ret=BufferData (&_oy);
      if (ret==0)
      {
        csReport (_object_reg, CS_REPORTER_SEVERITY_ERROR, QUALIFIED_PLUGIN_NAME,
          "End of file while searching for codec headers.\n");
        return false;
      }
    }
  }

  // Compute the length in seconds and frame count (where applicable)
  // of the streams
  ComputeStreamLength (container);

  // In order to get everything back in order, we need to reparse
  // the headers. Streams aren't recreated, but we need to get
  // the sync state in the proper position
  stateflag=0;
  ts=0;

  // Parse the headers
  while (!stateflag)
  {
    int ret=BufferData (&_oy);
    if (ret==0)
      break;

    while (ogg_sync_pageout (&_oy,&_og)>0)
    {
      ogg_stream_state test;

      // Is this a mandated initial header? If not, stop parsing
      if (!ogg_page_bos (&_og))
      {
        // Don't leak the page; get it into the appropriate stream
        container->QueuePage (&_og);

        stateflag=1;
        break;
      }

      ogg_stream_init (&test,ogg_page_serialno (&_og));
      ogg_stream_pagein (&test,&_og);
      ogg_stream_packetout (&test,&op);

      ogg_stream_clear (&test);
    }
  }

  // Next, parse secondary headers.
  times=0;
  while (times!=container->GetMediaCount ())
  {
    for (uint i=0;i<container->GetMediaCount ();i++)
    {

      times++;
      if (ogg_sync_pageout (&_oy,&_og)>0)
      {
        container->QueuePage (&_og); /* demux into the appropriate stream */
      }
      else
      {
        int ret=BufferData (&_oy);
        if (ret==0)
        {
          csReport (_object_reg, CS_REPORTER_SEVERITY_ERROR, QUALIFIED_PLUGIN_NAME,
            "End of file while searching for codec headers.\n");
          return false;
        }
      }
    }

  }

  // Copy the ogg sync state to the container
  memcpy (&container->_syncState,&_oy,sizeof (_oy));

  return true;
}

void csThOggLoader::ComputeStreamLength (TheoraMediaContainer* container)
{
  for (size_t i=0;i<container->GetMediaCount ();i++)
  {
    if (strcmp (container->GetMedia (i)->GetType (), "TheoraVideo") == 0)
    {
      csRef<iVideoMedia> media = scfQueryInterface<iVideoMedia> (container->GetMedia (i));
      if (media.IsValid ())
      {
        csRef<csTheoraVideoMedia> buff = static_cast<csTheoraVideoMedia*> ( (iVideoMedia*)media);
        buff->Initialize (this->_object_reg);

        float mDuration=0;
        unsigned long mNumFrames=0;
        ogg_sync_reset (&_oy);

        for (int i=1;i<=10;i++)
        {
          ogg_sync_reset (&_oy);
          fseek (_infile,container->GetFileSize () - 4096*i,SEEK_SET);

          char *buffer = ogg_sync_buffer (&_oy, 4096*i);
          int bytesRead = fread (buffer,1,4096*i,_infile);
          ogg_sync_wrote (&_oy, bytesRead );
          ogg_sync_pageseek (&_oy,&_og);

          while (true)
          {
            int ret=ogg_sync_pageout (&_oy, &_og);
            if (ret == 0) break;
            // If page is not a Theora page, skip it
            if (ogg_page_serialno (&_og) != buff->StreamState ()->serialno) continue;

            unsigned long granule= (unsigned long) ogg_page_granulepos (&_og);
            if (granule >= 0)
            {
              mDuration= (float) th_granule_time (buff->DecodeControl (),granule);
              mNumFrames= (unsigned long) th_granule_frame (buff->DecodeControl (),granule)+1;
            }
          }

          if (mDuration > 0) break;
        }

        buff->SetLength (mDuration);
        buff->SetFrameCount (mNumFrames);

        ogg_sync_reset (&_oy);
        fseek (_infile,0,SEEK_SET);
      }
    }

    if (strcmp (container->GetMedia (i)->GetType (),"TheoraAudio")==0 )
    {
      csMedia* media = dynamic_cast<csMedia*> (container->GetMedia (i));
      csTheoraAudioMedia* buff = static_cast<csTheoraAudioMedia*> (media);
      buff->Initialize (this->_object_reg);
      float mDuration=0;

      for (int i=1;i<=10;i++)
      {
	ogg_sync_reset (&_oy);
	fseek (_infile,container->GetFileSize () - 4096*i,SEEK_SET);

	char *buffer = ogg_sync_buffer (&_oy, 4096*i);
	int bytesRead = fread (buffer,1,4096*i,_infile);
	ogg_sync_wrote (&_oy, bytesRead );
	ogg_sync_pageseek (&_oy,&_og);

	while (true)
	{
	  int ret=ogg_sync_pageout ( &_oy, &_og );
	  if (ret == 0) break;

	  // If page is not a Theora page, skip it
	  if (ogg_page_serialno (&_og) != buff->StreamState ()->serialno) continue;

	  unsigned long granule= (unsigned long) ogg_page_granulepos (&_og);
	  if (granule >= 0)
	  {
	    mDuration= (float) vorbis_granule_time (buff->DspState (),granule);
	  }
	}

	if (mDuration > 0) break;
      }

      buff->SetLength (mDuration);

      ogg_sync_reset (&_oy);
      fseek (_infile,0,SEEK_SET);
    }
  }
}
