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

#include <soundserver.h>
#include "cssysdef.h"
#include "artsrend.h"
#include "iutil/objreg.h"
#include "ivaria/reporter.h"

#define ARTS_SIMPLESOUNDSERVER "global:Arts_SimpleSoundServer"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE (csArtsRenderer)
  SCF_IMPLEMENTS_INTERFACE (iSoundListener)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iSoundRender)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csArtsRenderer::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csArtsRenderer::eiSoundRender)
  SCF_IMPLEMENTS_INTERFACE (iSoundRender)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csArtsRenderer);


csArtsRenderer::csArtsRenderer (iBase *pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiSoundRender);
  dispatcher = 0;
  bInit = false;
  SetVolume (1.0f);
  SetEnvironment (ENVIRONMENT_GENERIC);
  SetPosition (csVector3 (0.0f, 0.0f, 1.0f));
  SetDirection (csVector3 (0.0f, 1.0f, 0.0f), csVector3 (0.0f, 0.0f, 1.0f));
  SetVelocity (csVector3 (1.0f, 1.0f, 1.0f));
  SetHeadSize (1.0f);
}

csArtsRenderer::~csArtsRenderer ()
{
  vObject.DeleteAll ();
  if (dispatcher) delete dispatcher;
}

bool csArtsRenderer::Initialize (iObjectRegistry *object_reg)
{
  // ok, we try to get a remote soundserver object
  dispatcher = new Arts::Dispatcher;
  server = Arts::Reference (ARTS_SIMPLESOUNDSERVER);
  if (server.isNull ())
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_WARNING,
    	"crystalspace.sound.arts",
        "Couldn't get a reference to the soundserver !\n"
        "Check whether you have the Arts server running "
        "(usually called \"artsd\")");
  }
  else
    bInit = true;
  return bInit;
}

Arts::csSoundModule *csArtsRenderer::CreateArtsModule ()
{
  Arts::csSoundModule *sm = new Arts::csSoundModule;
  *sm = Arts::DynamicCast (server.createObject ("Arts::csSoundModule"));
  if (sm->isNull ())
  {
    delete sm;
    return 0;
  }
  return sm;
}

void csArtsRenderer::SetVolume (float vol)
{
  if (bInit)
  {
    // Skip through all the soundobjects we handed out and set the volume for
    // each.  Note that we could just insert a stereovolume control on the
    // servers effect stack.  The upside would be easier handling here, the
    // downside is we would modify the volume of other sounds too, like the
    // napstered mp3 playing in the background.
	int i;
    for (i=0; i<vObject.Length (); i++)
      vObject.Get(i)->SetVolume (vol);
  }
  volume = vol;
}

float csArtsRenderer::GetVolume ()
{
  return volume;
}

csPtr<iSoundHandle> csArtsRenderer::RegisterSound(iSoundData *sd)
{
  csArtsHandle *sh = new csArtsHandle (this);
  if (sh->UseData (sd))
  {
    vObject.InsertSorted (sh);
    sh->SetVolume (volume);
    return csPtr<iSoundHandle> (sh);
  }
  else
    delete sh;
  return 0;
}

void csArtsRenderer::UnregisterSound(iSoundHandle *sh)
{
  int idx = vObject.FindSortedKey ((const void*)sh);
  if (idx != -1)
    vObject.Delete (idx);

}

csPtr<iSoundSource> csArtsRenderer::CreateSource (csArtsHandle *pHandle, int Mode3D)
{
  csArtsHandle *sh = new csArtsHandle (this);
  if (sh->UseData (pHandle->sd))
  {
    vObject.InsertSorted (sh);
    sh->SetVolume (volume);
    sh->SetMode3D (Mode3D);
    return csPtr<iSoundSource> (sh);
  }
  else
    delete sh;
  return 0;
}

iSoundListener *csArtsRenderer::GetListener ()
{
  return this;
}

void csArtsRenderer::SetDirection(const csVector3 &Front, const csVector3 &Top)
{
  if (bInit)
  {
	int i;
    for (i=0; i<vObject.Length (); i++)
      vObject.Get(i)->SetDirection (Front, Top);

  }
  front = Front;
  top = Top;
}

void csArtsRenderer::SetPosition (const csVector3 &pos)
{
  if (bInit)
  {
	int i;
    for (i=0; i<vObject.Length (); i++)
      vObject.Get(i)->SetPosition (pos);

  }
  this->pos = pos;
}

void csArtsRenderer::SetVelocity (const csVector3 &v)
{
  if (bInit)
  {
	int i;
    for (i=0; i<vObject.Length (); i++)
      vObject.Get(i)->SetVelocity (v);

  }
  velocity = v;
}

void csArtsRenderer::SetDistanceFactor (float factor)
{
  if (bInit)
  {
	int i;
    for (i=0; i<vObject.Length (); i++)
      vObject.Get(i)->SetDistanceFactor (factor);
  }
  distanceFactor = factor;
}

void csArtsRenderer::SetRollOffFactor (float factor)
{
  if (bInit)
  {
	int i;
    for (i=0; i<vObject.Length (); i++)
      vObject.Get(i)->SetRollOffFactor (factor);
  }
  rolloffFactor = factor;
}

void csArtsRenderer::SetDopplerFactor (float factor)
{
  if (bInit)
  {
	int i;
    for (i=0; i<vObject.Length (); i++)
      vObject.Get(i)->SetDopplerFactor (factor);
  }
  dopplerFactor = factor;
}

void csArtsRenderer::SetHeadSize (float size)
{
  if (bInit)
  {
	int i;
    for (i=0; i<vObject.Length (); i++)
      vObject.Get(i)->SetHeadSize (size);
  }
  headSize = size;
}

void csArtsRenderer::SetEnvironment (csSoundEnvironment env)
{
  int n = (int)env;
  static struct {
    float distFact;
    float roffFact;
    float doppFact;
  } enviro[] = {
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_GENERIC
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_PADDEDCELL
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_ROOM
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_BATHROOM
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_LIVINGROOM
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_STONEROOM
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_AUDITORIUM
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_CONCERTHALL
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_CAVE
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_ARENA
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_CARPETEDHALLWAY
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_HALLWAY
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_STONECORRIDOR
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_ALLEY
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_FOREST
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_CITY
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_MOUNTAINS
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_QUARRY
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_PLAIN
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_PARKINGLOT
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_SEWERPIPE
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_UNDERWATER
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_DRUGGED
    {1.0f, 1.0f, 1.0f}, // ENVIRONMENT_DIZZY
    {1.0f, 1.0f, 1.0f} // ENVIRONMENT_PSYCHOTIC
  };

  SetDistanceFactor (enviro[n].distFact);
  SetRollOffFactor (enviro[n].roffFact);
  SetDopplerFactor (enviro[n].doppFact);
  environment = env;
}
