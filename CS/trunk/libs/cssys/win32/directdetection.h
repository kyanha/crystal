// DirectDetection.h: interface for the DirectDetection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTDETECTION_H__E850B940_FD15_11D1_933A_0000B43D8331__INCLUDED_)
#define AFX_DIRECTDETECTION_H__E850B940_FD15_11D1_933A_0000B43D8331__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if defined(OS_WIN32)

#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <d3d.h>
#include <d3dcaps.h>

#include "def.h"
#include "iddetect.h"

/// Description of DirectDraw device
class DirectDetection2D
{
public:
  DirectDetection2D()
  {
    Mipmap = false; Texture = false; Can3D = false; Windowed = false;
    IsPrimary2D = true; DeviceName2D = NULL; DeviceDescription2D = NULL;
    ZeroMemory(&Guid2D, sizeof(GUID));
  }
  virtual ~DirectDetection2D() {}

  bool Mipmap;                // Can enable ddraw surface as mipmap ?
  bool Texture;               // Can enable ddraw surface as texture ?
  GUID Guid2D;                // Guid for DirectDraw device
  bool Can3D;                 // Have a support for d3d device ?
  bool Windowed;              // Can enable windowed mode for graphics ?
  bool IsPrimary2D;           // Is a primary ddraw device ?
  char * DeviceName2D;        // Name of device
  char * DeviceDescription2D; // Description of device
};

/// Description of Direct3D device
class DirectDetection3D : public DirectDetection2D
{
public:
  // construct a device from ddraw (2d) device
  DirectDetection3D(DirectDetection2D * dd2d)
  {
    Hardware = false; AlphaBlendType = 0; AlphaBlend = false, AlphaBlendHalo = false;
    Perspective = false; HighColor = false; ZBuffer = false;
    ZeroMemory(&Desc3D, sizeof(D3DDEVICEDESC));
    ZeroMemory(&Guid3D, sizeof(GUID));
    DeviceName3D = NULL; DeviceDescription3D = NULL;
    IsPrimary3D = true;

    memcpy((DirectDetection2D *)this, dd2d, sizeof(DirectDetection2D));
  }
  DirectDetection3D():DirectDetection2D()
  {
    Hardware = false; AlphaBlendType = 0; AlphaBlend = false, AlphaBlendHalo = false;
    Perspective = false; HighColor = false; ZBuffer = false;
    ZeroMemory(&Desc3D, sizeof(D3DDEVICEDESC));
    ZeroMemory(&Guid3D, sizeof(GUID));
    DeviceName3D = NULL; DeviceDescription3D = NULL;
    IsPrimary3D = true;
  } 
  virtual ~DirectDetection3D() {}

  bool Hardware;              // Is hardware device ?
  bool AlphaBlend;            // Can enable AlphaBlend for lightmap ?
  bool AlphaBlendHalo;            // Can enable AlphaBlend for halo ?
  int AlphaBlendType;         // type of AphaBlend supported
  bool Perspective;           // Can enable perspective ?
  bool VideoMemoryTexture;    // Can push texture in video memory ?
  bool HighColor;             // Can enable 16 bit mode
  bool ZBuffer;               // Have a ZBuffer ?
  D3DDEVICEDESC Desc3D;       // D3DDEVICEDESC for complete information 
  GUID Guid3D;                // Guid for Direct3D device
  bool IsPrimary3D;           // Is a primary d3d device ?
  char * DeviceName3D;        // Device name
  char * DeviceDescription3D; // Device description
};

/// generic device (2d device + 3d device) which is a item list
class DirectDetectionDevice : public DirectDetection3D, public IDirectDetectionInternal
{
public:
  DirectDetectionDevice():DirectDetection3D() {next = NULL; Only2D = true;}
  virtual ~DirectDetectionDevice() {}

  bool Only2D; // Is only a 2d device ?
  DirectDetectionDevice * next;
  
  ///
  STDMETHODIMP_(LPD3DDEVICEDESC) GetDesc3D()  { return &Desc3D; }
  ///
  STDMETHODIMP_(LPGUID) GetGuid3D() { return &Guid3D; }
  ///
  STDMETHODIMP_(bool) GetMipmap() { return Mipmap; }
  ///
  STDMETHODIMP_(bool) GetAlphaBlend() { return AlphaBlend; }
  ///
  STDMETHODIMP_(int) GetAlphaBlendType() { return AlphaBlendType; }
  ///
  STDMETHODIMP_(bool) GetAlphaBlendHalo() { return AlphaBlendHalo; }
  ///
  STDMETHODIMP_(bool) GetHardware() { return Hardware; }
};

// master class of the device detection of direct3d and directdraw
class DirectDetection  
{
public:
  bool Have2DDevice();
  bool Have3DDevice();
  DirectDetection();
  virtual ~DirectDetection();
  DirectDetectionDevice * findBestDevice3D(bool fscreen);
  DirectDetectionDevice * findBestDevice2D();
  int addDevice(DirectDetection2D * dd2d);
  int addDevice(DirectDetection3D * dd3d);
  bool checkDevices();
  bool checkDevices2D();
  bool checkDevices3D();

  DirectDetectionDevice * Devices; // list of devices
};

#endif
#endif // !defined(AFX_DIRECTDETECTION_H__E850B940_FD15_11D1_933A_0000B43D8331__INCLUDED_)
