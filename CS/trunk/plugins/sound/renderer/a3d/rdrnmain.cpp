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

#include <objbase.h>
#include <initguid.h>

#include "ia3dapi.h"

#include "cssysdef.h"
#include "csutil/scf.h"
#include "sndrdr.h"
#include "isndrdr.h"

static unsigned int gRefCount = 0;
static DllRegisterData gRegData =
{
  &CLSID_A3DSoundRender,
  "crystalspace.sound.render.a3d",
  "Crystal Space A3D Sound Render"
};

#ifdef CS_STATIC_LINKED

void SoundRenderA3DRegister ()
{
  static csSoundRenderA3DFactory gFactory;
  gRegData.pClass = &gFactory;
  csRegisterServer (&gRegData);
}

void SoundRenderA3DUnregister ()
{
  csUnregisterServer (&gRegData);
}

#else

// This is the name of the DLL. Make sure to change this if you change the DLL name!
// DAN: this might have to be changed for each OS, cuz each OS has a different extension for DLLs.
#ifdef OS_WIN32
#define DLL_NAME "SoundRenderA3D.dll"
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
  static csSoundRenderA3DFactory gFactory;
  if (rclsid == CLSID_A3DSoundRender)
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

// Implementation of csSoundRenderA3DFactory

IMPLEMENT_UNKNOWN_NODELETE (csSoundRenderA3DFactory)

BEGIN_INTERFACE_TABLE (csSoundRenderA3DFactory)
  IMPLEMENTS_INTERFACE (ISoundRenderFactory)
END_INTERFACE_TABLE ()

STDMETHODIMP csSoundRenderA3DFactory::CreateInstance (REFIID riid, iSystem* piSystem, void** ppv)
{
  if (!piSystem)
  {
    *ppv = 0;
    return E_UNEXPECTED;
  }

  csSoundRenderA3D* pNew = new csSoundRenderA3D (piSystem);
  if (!pNew)
  {
    *ppv = 0;
    return E_OUTOFMEMORY;
  }

  return pNew->QueryInterface (riid, ppv);
}

STDMETHODIMP csSoundRenderA3DFactory::LockServer (COMBOOL bLock)
{
  if (bLock)
    gRefCount++;
  else
    gRefCount--;

  return S_OK;
}
