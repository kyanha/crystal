/*
    Copyright (C) 1998 by Jorrit Tyberghein and Dan Ogles

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

// g3d_d3d.h
// csGraphics3DDirect3DDx5 Class Declaration
// Written by dan
// Some modifications by Nathaniel

// Ported to COM by Dan Ogles on 8.26.98

#ifndef G3D_D3D_H
#define G3D_D3D_H

#include <windows.h>
#include "ddraw.h"
#include "d3d.h"
#include "d3dcaps.h"

#include "video/renderer/direct3d5/d3d_txtcache.h"
#include "video/renderer/direct3d5/d3d_txtmgr.h"
#include "video/renderer/common/dtmesh.h"
#include "video/renderer/common/dpmesh.h"
#include "csutil/scf.h"
#include "csgeom/transfrm.h"
#include "cssys/win32/IDDetect.h"
#include "igraph3d.h"
#include "ihalo.h"
#include "iplugin.h"
#include "ipolygon.h"

//DIRECT3D DRIVER HRESULTS/////////////////////////////

#define CSD3DERR_ERRORSTARTINGDIRECTDRAW    MAKE_CSHRESULT(0x0000)
#define CSD3DERR_WRONGVERSION               MAKE_CSHRESULT(0x5001)
#define CSD3DERR_NOPRIMARYSURFACE           MAKE_CSHRESULT(0x5002)
#define CSD3DERR_NOZBUFFER                  MAKE_CSHRESULT(0x5003)
#define CSD3DERR_CANNOTATTACH               MAKE_CSHRESULT(0x5004)
#define CSD3DERR_NOCOMPATIBLESURFACEDESC    MAKE_CSHRESULT(0x5005)
#define CSD3DERR_ERRORCREATINGMATERIAL      MAKE_CSHRESULT(0x5006)
#define CSD3DERR_ERRORCREATINGVIEWPORT      MAKE_CSHRESULT(0x5007)
#define CSD3DERR_ERRORCLEARINGBUFFER        MAKE_CSHRESULT(0x5008)
#define CSD3DERR_ERRORBEGINSCENE            MAKE_CSHRESULT(0x5009)
#define CSD3DERR_ERRORENDSCENE              MAKE_CSHRESULT(0x500A)
#define CSD3DERR_INTERNALERROR              MAKE_CSHRESULT(0x000B)

//DIRECT3D SPECIFIC STRUCTS///////////////////////////

class D3DTextureCache;
class D3DLightMapCache;
class csClipper;

/// the Direct3D implementation of the Graphics3D class.
class csGraphics3DDirect3DDx5 : public iGraphics3D
{
  /// Pointer to DirectDraw class
  IDirectDraw* m_lpDD;
  /// Primary Surface
  IDirectDrawSurface* m_lpddPrimary;
  /// Offscreen Surface
  IDirectDrawSurface* m_lpddDevice;
  
  /// Direct3D class
  IDirect3D2* m_lpD3D;
  /// ZBuffer surface
  IDirectDrawSurface* m_lpddZBuffer;
  /// D3D Device
  IDirect3DDevice2* m_lpd3dDevice;
  
  /// D3d Viewport
  IDirect3DViewport2* m_lpd3dViewport;
  /// D3d Background material.
  IDirect3DMaterial2* m_lpd3dBackMat;
  /// D3D handle to the background material.
  D3DMATERIALHANDLE m_hd3dBackMat;
  
  /// whether this is a hardware device.
  bool m_bIsHardware;
  /// the bit-depth of this device.
  DWORD m_dwDeviceBitDepth;
  /// the globally unique identifier for this device.
  GUID m_Guid;
  
  /// The texture manager
  csTextureManagerDirect3D* txtmgr;

  /// the texture cache.
  D3DTextureCache* m_pTextureCache;
  /// the lightmap cache.
  D3DLightMapCache* m_pLightmapCache;
  
  /// supported lightmap type
  int m_iTypeLightmap;
  /// support halo.
  bool m_bHaloEffect;
  
  /// Dimensions of viewport.
  int m_nWidth,  m_nHeight;
  
  /// Half-dimensions of viewport.
  int m_nHalfWidth,  m_nHalfHeight;

  /// Current transformation from world to camera.
  csReversibleTransform m_o2c;
  /// Current 2D clipper.
  csClipper* m_pClipper;
  /// Current aspect ratio for perspective correction.
  float m_Aspect;
  /// Current inverse aspect ratio for perspective correction.
  float m_InvAspect;
  
  /// The current read/write settings for the Z-buffer.
  csZBufMode m_ZBufMode;
  
  /// The current drawing mode (2D/3D)
  int m_nDrawMode;

  /// Option variables: mipmap distances
  float zdist_mipmap1, zdist_mipmap2, zdist_mipmap3;

  /**
   * DAN: render-states
   * these override any other variable settings.
   */
  bool rstate_dither;
  bool rstate_bilinearmap;
  bool rstate_trilinearmap;
  bool rstate_gouraud;
  bool rstate_flat;
  bool rstate_alphablend;
  int  rstate_mipmap;

  bool  m_gouraud;
  UInt  m_mixmode;
  float m_alpha;

  /// Capabilities of the renderer.
  csGraphics3DCaps m_Caps;

  /// the 2d graphics driver.
  iGraphics2D* m_piG2D;
  
  /// The directdraw device description
  IDirectDetectionInternal* m_pDirectDevice;
  
  /// The system driver
  iSystem* m_piSystem;

  /// Verbose mode
  bool m_bVerbose;

  /// The configuration file
  csIniFile *config;

public:
  DECLARE_IBASE;
  
  static DDSURFACEDESC m_ddsdLightmapSurfDesc;
  static DDSURFACEDESC m_ddsdTextureSurfDesc;
  static DDSURFACEDESC m_ddsdHaloSurfDesc;
  
  /// The constructor. It is passed an interface to the system using it.
  csGraphics3DDirect3DDx5 (iBase*);
  /// the destructor.
  ~csGraphics3DDirect3DDx5 ();
  
  ///
  virtual bool Initialize (iSystem *iSys);

  /// opens Direct3D.
  virtual bool Open(const char* Title);
  /// closes Direct3D.
  virtual void Close();
  
  /// Change the dimensions of the display.
  virtual void SetDimensions (int width, int height);
  
  /// Start a new frame.
  virtual bool BeginDraw (int DrawFlags);
  
  /// End the frame and do a page swap.
  virtual void FinishDraw ();
  
  /// Draw the projected polygon with light and texture.
  virtual void DrawPolygon (G3DPolygonDP& poly);
  /// Draw debug poly
  virtual void DrawPolygonDebug(G3DPolygonDP& /*poly*/) { }
  
  /// Draw a Line.
  virtual void DrawLine (const csVector3& v1, const csVector3& v2, float fov, int color);
  
  /// Start a series of DrawPolygonFX
  virtual void StartPolygonFX (iTextureHandle* handle, UInt mode);

  /// Finish a series of DrawPolygonFX
  virtual void FinishPolygonFX ();

  /// Draw a polygon wit special effects.
  virtual void DrawPolygonFX (G3DPolygonDPFX& poly);

  /// Give a texture to Graphics3D to cache it.
  void CacheTexture (iPolygonTexture* texture);
  
  /// Get the capabilities of this driver: NOT IMPLEMENTED.
  virtual csGraphics3DCaps *GetCaps ()
  { return &m_Caps; }
  
  /// Dump the texture cache.
  virtual void DumpCache ();
  
  /// Clear the texture cache.
  virtual void ClearCache ();

  /// Remove some polygon from the cache.
  virtual void RemoveFromCache (iPolygonTexture* /*poly_texture*/) { }

  /// Print the screen.
  virtual void Print (csRect* rect)
  { m_piG2D->Print (rect); }
  
  /// Set a render state
  virtual bool SetRenderState(G3D_RENDERSTATEOPTION, long);
  /// Get a render state
  virtual long GetRenderState(G3D_RENDERSTATEOPTION);
  
  /// Get a z-buffer point
  virtual unsigned long *GetZBuffAt (int /*x*/, int /*y*/)
  { return NULL; }
  
  /// Get the width
  virtual int GetWidth ()
  { return m_nWidth; }
  ///
  virtual int GetHeight ()
  { return m_nHeight; }

  /// Set center of projection.
  virtual void SetPerspectiveCenter(int x, int y);
  /// Set perspective aspect. 
  virtual void SetPerspectiveAspect (float aspect)
  {
    m_Aspect    = aspect;
    m_InvAspect = 1.0/aspect;
  }
  /// Set world to camera transformation. 
  virtual void SetObjectToCamera (csReversibleTransform* o2c) 
  {
    m_o2c = *o2c;
  }
  /// Set optional clipper. 
  virtual void SetClipper (csVector2* vertices, int num_vertices);
  /// Draw a triangle mesh. 
  virtual void DrawTriangleMesh (G3DTriangleMesh& mesh) 
  {
    DefaultDrawTriangleMesh (mesh, this, m_o2c, m_pClipper, m_Aspect, m_nHalfWidth, m_nHalfHeight);
  }

  /// Draw a polygon mesh.
  virtual void DrawPolygonMesh (G3DPolygonMesh& mesh)
  {
    DefaultDrawPolygonMesh (mesh, this, o2c, clipper, aspect, inv_aspect, width2, height2);
  }

  /// Get the ITextureManager.
  virtual iTextureManager *GetTextureManager ()
  { return txtmgr; }

  /// 
  virtual iGraphics2D *GetDriver2D () 
  { return m_piG2D; }

  /// Get Z-buffer value at given X,Y position
  virtual float GetZBuffValue (int /*x*/, int /*y*/)
  { return 0; }

  /// Create a halo of the specified color and return a handle.
  virtual iHalo *CreateHalo (float /*iR*/, float /*iG*/, float /*iB*/,
    unsigned char* /*iAlpha*/, int /*iWidth*/, int /*iHeight*/)
  { return NULL; }

  virtual void OpenFogObject (CS_ID id, csFog* fog);
  virtual void DrawFogPolygon (CS_ID id, G3DPolygonDFP& poly, int fogtype);
  virtual void CloseFogObject (CS_ID id);

  /// Create an off screen canvas
  virtual iGraphics3D *CreateOffScreenRenderer (int /*width*/, int /*height*/, 
     csPixelFormat */*pfmt*/, void */*buffer*/, RGBPixel */*palette*/, int /*pal_size*/)
  { return NULL; }
private:
  
  // texture format enumeration callback function :: static member function.
  static HRESULT CALLBACK EnumTextFormatsCallback(LPDDSURFACEDESC lpddsd, LPVOID lpUserArg);
  
  /// used to get the appropriate device.
  bool EnumDevices(void);
  /// whether the device is locked or not.
  bool m_bIsLocked;
  
  /// used to set up polygon geometry before rasterization.
  inline void SetupPolygon( G3DPolygonDP& poly, float& J1, float& J2, float& J3, 
    float& K1, float& K2, float& K3,
    float& M,  float& N,  float& O  );
};

#endif // G3D_D3D_H
