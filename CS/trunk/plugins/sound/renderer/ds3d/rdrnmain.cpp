/*
    Sound Render interface DLL

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

#include <stdlib.h>

#include "dsound.h"

#include "sysdef.h"
#include "csutil/scf.h"
#include "sndrdr.h"
#include "isndrdr.h"

static unsigned int gRefCount = 0;
static DllRegisterData gRegData =
{
  &CLSID_DS3DSoundRender,
  "crystalspace.sound.render.ds3d",
  "Crystal Space DirectSound3D Sound Render"
};

#ifdef CS_STATIC_LINKED

void SoundRenderDS3DRegister ()
{
  static csSoundRenderDS3DFactory gFactory;
  gRegData.pClass = &gFactory;
  csRegisterServer (&gRegData);
}

void SoundRenderDS3DUnregister ()
{
  csUnregisterServer (&gRegData);
}

#else

// This is the name of the DLL. Make sure to change this if you change the DLL name!
// DAN: this might have to be changed for each OS, cuz each OS has a different extension for DLLs.
#ifdef OS_WIN32
#define DLL_NAME "SoundRenderDS3D.dll"
#endif

// our main entry point...should be called when we're loaded.
STDAPI DllInitialize ()
{
  csCoInitialize (0);
  gRegData.szInProcServer = DLL_NAME;
  return TRUE;
}

EXTERN_C void STDAPICALLTYPE ModuleRelease ()
{
  gRefCount--;
}

EXTERN_C void STDAPICALLTYPE ModuleAddRef ()
{
  gRefCount++;
}

// return S_OK if it's ok to unload us now.
STDAPI DllCanUnloadNow ()
{
  return gRefCount ? S_FALSE : S_OK;
}

// used to get a COM class object from us.
STDAPI DllGetClassObject (REFCLSID rclsid, REFIID riid, void** ppv)
{
  static csSoundRenderDS3DFactory gFactory;
  if (rclsid == CLSID_DS3DSoundRender)
    return gFactory.QueryInterface(riid, ppv);

  //  if we get here, rclsid is a class we don't implement
  *ppv = NULL;
  return CLASS_E_CLASSNOTAVAILABLE;
}

// Called by RegSvr32.exe
STDAPI DllRegisterServer ()
{
  return csRegisterServer (&gRegData);
}

// Called by RegSvr32.exe
STDAPI DllUnregisterServer ()
{
  return csUnregisterServer(&gRegData);
}

#endif

// Implementation of csSoundRenderDS3DFactory

IMPLEMENT_UNKNOWN_NODELETE (csSoundRenderDS3DFactory)

BEGIN_INTERFACE_TABLE (csSoundRenderDS3DFactory)
  IMPLEMENTS_INTERFACE (ISoundRenderFactory)
END_INTERFACE_TABLE ()

STDMETHODIMP csSoundRenderDS3DFactory::CreateInstance (REFIID riid, iSystem* piSystem, void** ppv)
{
  if (!piSystem)
  {
    *ppv = 0;
    return E_UNEXPECTED;
  }

  csSoundRenderDS3D* pNew = new csSoundRenderDS3D (piSystem);
  if (!pNew)
  {
    *ppv = 0;
    return E_OUTOFMEMORY;
  }

  return pNew->QueryInterface (riid, ppv);
}

STDMETHODIMP csSoundRenderDS3DFactory::LockServer (COMBOOL bLock)
{
  if (bLock)
    gRefCount++;
  else
    gRefCount--;

  return S_OK;
}
