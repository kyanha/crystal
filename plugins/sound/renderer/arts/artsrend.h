/*
    Copyright (C) 2001 by Norman Kr�mer

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

#ifndef __CS_ARTS_SOUNDRENDERER_H__
#define __CS_ARTS_SOUNDRENDERER_H__

#include "isound/renderer.h"
#include "isound/listener.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "artshndl.h"
#include "csutil/parray.h"

class csSoundModule;

class csArtsRenderer : public iSoundListener
{
 protected:
  friend class csArtsHandle;
  class soVector : public csPDelArray<csArtsHandle>
  {
  public:
    static int Compare (csArtsHandle const* h1, csArtsHandle const* h2)
    {
      return (h1 < h2 ? -1 : h1 > h2 ? 1 : 0);
    }
    static int CompareKey (csArtsHandle* h1, void* Key)
    {
      csArtsHandle *h2 = (csArtsHandle *)Key;
      return (h1 < h2 ? -1 : h1 > h2 ? 1 : 0);
    }
  };

  Arts::SimpleSoundServer server;
  Arts::Dispatcher *dispatcher;

  csVector3 front, top, pos, velocity;
  float distanceFactor, rolloffFactor, dopplerFactor, headSize, volume;
  csSoundEnvironment environment;
  soVector vObject;
  bool bInit;

  csPtr<iSoundSource> CreateSource (csArtsHandle *pHandle, int Mode3D);
  Arts::csSoundModule *CreateArtsModule ();

public:
  SCF_DECLARE_IBASE;
  csArtsRenderer (iBase *pParent);
  virtual ~csArtsRenderer ();

  bool Initialize (iObjectRegistry *object_reg);

  // *********** Renderer Interface ****************
  /// Set Volume [0, 1]
  virtual void SetVolume (float vol);
  /// Get Volume [0, 1]
  virtual float GetVolume ();

  /// Register a sound
  virtual csPtr<iSoundHandle> RegisterSound(iSoundData *);
  /// Unregister a sound
  virtual void UnregisterSound(iSoundHandle *);

  /// Get the global Listener object
  virtual iSoundListener *GetListener ();

  virtual void MixingFunction (){}

  // *********** Listener Interface *****************

  /// Set direction of listener (front and top 3d vectors)
  virtual void SetDirection (const csVector3 &Front, const csVector3 &Top);
  /// Set position of listener
  virtual void SetPosition (const csVector3 &pos);
  /// Set velocity of listener
  virtual void SetVelocity (const csVector3 &v);
  /// Set a distance attenuator
  virtual void SetDistanceFactor (float factor);
  /// Set a RollOff factor
  virtual void SetRollOffFactor (float factor);
  /// Set the Doppler attenuator
  virtual void SetDopplerFactor (float factor);
  /// Set distance between the 2 'ears' of listener
  virtual void SetHeadSize (float size);
  /// set type of environment where 'live' listener
  virtual void SetEnvironment (csSoundEnvironment env);

  /// Get direction of listener (front and top 3d vectors)
  virtual void GetDirection (csVector3 &Front, csVector3 &Top)
  {Front = front; Top = top;}
  /// Get position of listener
  virtual const csVector3 &GetPosition ()
  {return pos;}
  /// Get velocity of listener
  virtual const csVector3 &GetVelocity ()
  {return velocity;}
  /// Get a distance attenuator
  virtual float GetDistanceFactor ()
  {return distanceFactor;}
  /// Get a RollOff factor
  virtual float GetRollOffFactor ()
  {return rolloffFactor;}
  /// Get the Doppler attenuator
  virtual float GetDopplerFactor ()
  {return dopplerFactor;}
  /// Get distance between the 2 'ears' of listener
  virtual float GetHeadSize ()
  {return headSize;}
  /// Get type of environment where 'live' listener
  virtual csSoundEnvironment GetEnvironment ()
  {return environment;}

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csArtsRenderer);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize(p); }
  } scfiComponent;

  struct eiSoundRender : public iSoundRender
  {
    SCF_DECLARE_EMBEDDED_IBASE(csArtsRenderer);
    virtual void SetVolume (float vol){scfParent->SetVolume (vol);}
    virtual float GetVolume (){return scfParent->GetVolume ();}
    virtual csPtr<iSoundHandle> RegisterSound(iSoundData *snd){return scfParent->RegisterSound (snd);}
    virtual void UnregisterSound(iSoundHandle *hdl){scfParent->UnregisterSound (hdl);}
    virtual iSoundListener *GetListener (){return scfParent->GetListener ();}
    virtual void MixingFunction (){scfParent->MixingFunction ();}
  } scfiSoundRender;

};

#endif // __CS_ARTS_SOUNDRENDERER_H__


