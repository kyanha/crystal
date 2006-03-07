/*
	Copyright (C) 2004 by Andrew Mann

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


#ifndef SNDSYS_RENDERER_SOFTWARE_RENDERER_H
#define SNDSYS_RENDERER_SOFTWARE_RENDERER_H

#include "csutil/cfgacc.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "csgeom/vector3.h"

#include "isndsys/ss_structs.h"
#include "isndsys/ss_renderer.h"
#include "isndsys/ss_eventrecorder.h"

#include "../../utility/queue.h"


//#include "csutil/scf.h"
//#include "csutil/array.h"
//#include "csutil/cfgacc.h"
//#include "csutil/thread.h"
//#include "iSndSys/data2.h"
//#include "iSndSys/renderer2.h"

#define MAX_CHANNELS 18

// see http://www.microsoft.com/whdc/device/audio/multichaud.mspx

#define SPEAKER_FRONT_LEFT                0x00000001
#define SPEAKER_FRONT_RIGHT               0x00000002
#define SPEAKER_FRONT_CENTER              0x00000004
#define SPEAKER_LOW_FREQUENCY             0x00000008
#define SPEAKER_BACK_LEFT                 0x00000010
#define SPEAKER_BACK_RIGHT                0x00000020
#define SPEAKER_FRONT_LEFT_OF_CENTER      0x00000040
#define SPEAKER_FRONT_RIGHT_OF_CENTER     0x00000080
#define SPEAKER_BACK_CENTER               0x00000100
#define SPEAKER_SIDE_LEFT                 0x00000200
#define SPEAKER_SIDE_RIGHT                0x00000400
#define SPEAKER_TOP_CENTER                0x00000800
#define SPEAKER_TOP_FRONT_LEFT            0x00001000
#define SPEAKER_TOP_FRONT_CENTER          0x00002000
#define SPEAKER_TOP_FRONT_RIGHT           0x00004000
#define SPEAKER_TOP_BACK_LEFT             0x00008000
#define SPEAKER_TOP_BACK_CENTER           0x00010000
#define SPEAKER_TOP_BACK_RIGHT            0x00020000


struct st_speaker_properties
{
  csVector3 RelativePosition;
  csVector3 Direction;
  float min_factor,max_factor;
};

struct iSndSysSoftwareDriver;
struct iConfigFile;
class SndSysListenerSoftware;
class SndSysSourceSoftware;
struct iSndSysSourceSoftware;
struct iReporter;

class csSndSysRendererSoftware : public iSndSysRenderer
{
public:
  SCF_DECLARE_IBASE;

  csSndSysRendererSoftware(iBase *piBase);
  virtual ~csSndSysRendererSoftware();

  // Called when the renderer plugin is opened
  bool Open ();

  // Called when the renderer plugin is closed
  void Close ();


  /// Set Volume (range from 0.0 to 1.0)
  virtual void SetVolume (float vol);

  /// Get Volume (range from 0.0 to 1.0)
  virtual float GetVolume ();


  /**
   * Uses the provided iSndSysData to create a sound stream with the given
   * 3D rendering mode
   */
  virtual csPtr<iSndSysStream> CreateStream(iSndSysData* data, int mode3d);

  /// Creates a source when provided with a Sound Stream
  virtual csPtr<iSndSysSource> CreateSource(iSndSysStream* stream);

  /// Remove a stream from the sound renderer's list of streams
  virtual bool RemoveStream(iSndSysStream* stream);

  /// Remove a source from the sound renderer's list of sources
  virtual bool RemoveSource(iSndSysSource* source);

  /// Get the global Listener object
  virtual csRef<iSndSysListener> GetListener ();


  /// Send a message to the console reporter
  void Report (int severity, const char* msg, ...);

  /// Called by the driver thread to request sound data
  virtual size_t FillDriverBuffer(void *buf1, size_t buf1_len,
    void *buf2, size_t buf2_len);

  /// Send a message to the sound system event recorder
  //   This version may be called by other components that have a reference to the renderer
  //   so that they dont need to hold a EventRecorder reference themselves (sources for example)
  void RecordEvent(SndSysEventCategory Category, SndSysEventLevel Severity, const char* msg, ...);



  // The object registry
  iObjectRegistry *object_reg;

  // the global listener object
  csRef<SndSysListenerSoftware> Listener;

  /// The sample format used by the software renderer
  csSndSysSoundFormat render_format;

  /// Set to true if the driver expects little endian data
  bool driver_little_endian;

  // TODO: Move to listener
  struct st_speaker_properties Speakers[MAX_CHANNELS];

protected:
  /// Interface to the Configuration file
  csConfigAccess Config;

  /// Interface to the low level sound driver
  csRef<iSndSysSoftwareDriver> SoundDriver;

  /// Global volume setting
  float Volume;

  Queue<iSndSysSourceSoftware> source_add_queue;
  Queue<iSndSysSource> source_remove_queue,source_clear_queue;
  csArray<iSndSysSourceSoftware *> sources;
  Queue<iSndSysStream> stream_add_queue,stream_remove_queue,stream_clear_queue;
  csArray<iSndSysStream *> streams;

  csTicks last_ticks;

  // Pointer to a buffer of sound samples used to mix data prior to
  // sending to the driver
  csSoundSample *sample_buffer;
  size_t sample_buffer_samples;

  uint32 last_intensity_multiplier;

  /// ID of the 'Open' event fired on system startup
  csEventID evSystemOpen;

  /// ID of the 'Close' event fired on system shutdown
  csEventID evSystemClose;

  /// ID of the 'Frame' event fired once each frame
  csEventID evFrame;

  /// The last time (in csTicks) that the source/stream garbage collection process was run
  csTicks LastGarbageCollectionTicks;

#ifdef CS_DEBUG
  /// The last time status information was reported
  csTicks LastStatusReport;
#endif

  /// The event recorder interface, if active
  csRef<iSndSysEventRecorder> EventRecorder;

protected:
  /// Performs cleanup operations on removed sources and streams
  //      !!WARNING!!  DO NOT CALL THIS FROM THE BACKGROUND THREAD
  void GarbageCollection();

  /// Shutdown procedure.  This removes all sources from the renderer's lists.
  //      !!WARNING!!  DO NOT CALL THIS FROM THE BACKGROUND THREAD
  void RemoveAllSources();

  /// Shutdown procedure.  This removes all streams from the renderer's lists.
  //      !!WARNING!!  DO NOT CALL THIS FROM THE BACKGROUND THREAD
  void RemoveAllStreams();

  /// Send a message to the sound system event recorder as the renderer
  void RecordEvent(SndSysEventLevel Severity, const char* msg, ...);

  /// Report current renderer status via RecordEvent
  //   This function only has a body in debug mode
  void StatusReport(csTicks CurrentTime);

  size_t CalculateMaxSamples(size_t bytes);
  void CalculateMaxBuffers(size_t samples, size_t *buf1_len, size_t *buf2_len);
  void ProcessPendingSources();
  void ProcessPendingStreams();
  void NormalizeSampleBuffer(size_t used_samples);
  void CopySampleBufferToDriverBuffer(void *drvbuf1,size_t drvbuf1_len,
    void *drvbuf2, size_t drvbuf2_len, size_t samples_per_channel);
  /// This copies to a single buffer, called up to twice
  csSoundSample *CopySampleBufferToDriverBuffer(void *drvbuf, 
    size_t drvbuf_len, csSoundSample *src, size_t samples_per_channel);
  
public:
  ////
  //
  // Interface implementation
  //
  ////

  // iComponent
  virtual bool Initialize (iObjectRegistry *obj_reg);

  // iEventHandler
  virtual bool HandleEvent (iEvent &e);

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csSndSysRendererSoftware);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize(p); }
  } scfiComponent;


  struct EventHandler : public iEventHandler
  {
  private:
    csSndSysRendererSoftware* parent;
  public:
    SCF_DECLARE_IBASE;
    EventHandler (csSndSysRendererSoftware* parent)
    {
      SCF_CONSTRUCT_IBASE (0);
      EventHandler::parent = parent;
    }
    virtual ~EventHandler ()
    {
      SCF_DESTRUCT_IBASE();
    }
    virtual bool HandleEvent (iEvent& e) { return parent->HandleEvent(e); }
    CS_EVENTHANDLER_NAMES("crystalspace.sndsys.renderer")
    CS_EVENTHANDLER_NIL_CONSTRAINTS
  } * scfiEventHandler;
};

#endif // #ifndef SNDSYS_RENDERER_SOFTWARE_RENDERER_H



