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
// csGraphics3DDirect3DDx6 Class Declaration
// Written by dan
// Some modifications by Nathaniel

// Ported to COM by Dan Ogles on 8.26.98
// modiefied by TristanMcLure  09/08/1999

#ifndef G3D_D3D_H
#define G3D_D3D_H

#include <windows.h>
#include "ddraw.h"
#include "d3d.h"
#include "d3dcaps.h"

#include "d3d_txtcache.h"
#include "d3d_txtmgr.h"
#include "d3d_states.h"
#include "d3d_vertcache.h"
#include "cs3d/common/dtmesh.h"
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
class csGraphics3DDirect3DDx6 : public iGraphics3D
{
  /// Pointer to DirectDraw class
  IDirectDraw4* m_lpDD4;
  /// Primary Surface
  IDirectDrawSurface4* m_lpddPrimary;
  /// Offscreen Surface
  IDirectDrawSurface4* m_lpddDevice;
  
  /// Direct3D class
  IDirect3D3* m_lpD3D;
  /// ZBuffer surface
  IDirectDrawSurface4* m_lpddZBuffer;
  /// D3D Device
  IDirect3DDevice2* m_lpd3dDevice;
  IDirect3DDevice3* m_lpd3dDevice2;
  /// D3d Viewport
  IDirect3DViewport3* m_lpd3dViewport;
  /// D3d Background material.
  IDirect3DMaterial3* m_lpd3dBackMat;
  /// D3D handle to the background material.
  D3DMATERIALHANDLE m_hd3dBackMat;
  
  /// whether this is a hardware device.
  bool m_bIsHardware;
  /// the bit-depth of this device.
  DWORD m_dwDeviceBitDepth;
  /// the globally unique identifier for this device.
  GUID m_Guid;
  
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
  
  /// The current read/write settings for the Z-buffer.
  G3DZBufMode m_ZBufMode;
  
  /// The current drawing mode (2D/3D)
  int m_nDrawMode;

  /// Option variables: mipmap distances
  float zdist_mipmap1, zdist_mipmap2, zdist_mipmap3;

  /**
   * DAN: render-states
   * these override any other variable settings.
   */
  bool rstate_dither;
  bool rstate_specular;
  bool rstate_bilinearmap;
  bool rstate_trilinearmap;
  bool rstate_gouraud;
  bool rstate_flat;
  bool rstate_alphablend;
  int  rstate_mipmap;

  bool  m_gouraud;
  UInt  m_mixmode;
  float m_alpha;

  /// Should DrawPolygonFX use texture?
  bool  m_textured;

  /// Capabilities of the renderer.
  G3D_CAPS m_Caps;

  //Maximum possible aspect ratio for the renderer.
  int m_MaxAspectRatio;

  /// Current transformation from world to camera.
  csTransform m_o2c;
  /// Current 2D clipper.
  csClipper* m_pClipper;
  /// Current aspect ratio for perspective correction.
  float m_Aspect;
  /// Current inverse aspect ratio for perspective correction.
  float m_InvAspect;
  
  /// the 2d graphics driver.
  iGraphics2D* m_piG2D;
  
  /// The directdraw device description
  IDirectDetectionInternal* m_pDirectDevice;
  
  /// The system driver
  iSystem* m_piSystem;

  /// Verbose mode
  bool m_bVerbose;

  /// The private config file
  csIniFile *config;

  csStateCacheDirect3DDx6 m_States;
  csVertexCacheDirect3D m_VertexCache;

  /// do we batch up polygons for PolygonFX calls?
  bool m_bBatchPolygonFX;
  /// do we render as translucent when requested?
  bool m_bRenderTransparent;
  /// do we render lightmaps?
  bool m_bRenderLightmap;
  /// can we use multitexturing?
  bool m_bMultiTexture;
  /// type of texture blending for lightmaps in multitexture mode
  D3DTEXTUREOP m_LightmapTextureOp;
  /// type of texture blending for lightmaps in multipass mode
  D3DBLEND m_LightmapSrcBlend, m_LightmapDstBlend;
  /// type of transparency blending
  D3DBLEND m_TransSrcBlend, m_TransDstBlend;

public:
  DECLARE_IBASE;

  /// The texture manager
  csTextureManagerDirect3D* txtmgr;

  static DDSURFACEDESC2 m_ddsdPrimarySurfDesc;
  static DDSURFACEDESC2 m_ddsdLightmapSurfDesc;
  static DDSURFACEDESC2 m_ddsdTextureSurfDesc;
  static DDSURFACEDESC2 m_ddsdHaloSurfDesc;

  /// Shift counters for converting R8G8B8 to internal texture format
  int ScreenRsr,  ScreenRsl,  ScreenGsr,  ScreenGsl,  ScreenBsr,  ScreenBsl;
  int TextureRsr, TextureRsl, TextureGsr, TextureGsl, TextureBsr, TextureBsl;
  
  /// The constructor. It is passed an interface to the system using it.
  csGraphics3DDirect3DDx6 (iBase*);
  /// the destructor.
  ~csGraphics3DDirect3DDx6 ();
  
  ///
  virtual bool Initialize (iSystem *iSys);

  /// opens Direct3D.
  virtual bool Open(const char* Title);
  /// closes Direct3D.
  virtual void Close ();
  
  /// Change the dimensions of the display.
  virtual void SetDimensions (int width, int height);
  
  /// Start a new frame.
  virtual bool BeginDraw (int DrawFlags);
  
  /// End the frame and do a page swap.
  virtual void FinishDraw ();
  
  /// Set the mode for the Z buffer (functionality also exists in SetRenderState).
  virtual void SetZBufMode (G3DZBufMode mode);
  
  /// Draw the projected polygon with light and texture.
  virtual void DrawPolygon (G3DPolygonDP& poly);
  /// Draw debug poly
  virtual void DrawPolygonDebug(G3DPolygonDP& /*poly*/) { }
  
  /// Draw a Line.
  virtual void DrawLine (csVector3& v1, csVector3& v2, float fov, int color);
  
  /// Start a series of DrawPolygonFX
  virtual void StartPolygonFX (iTextureHandle* handle, UInt mode);

  /// Finish a series of DrawPolygonFX
  virtual void FinishPolygonFX ();

  /// Draw a polygon wit special effects.
  virtual void DrawPolygonFX (G3DPolygonDPFX& poly);

  /// Give a texture to Graphics3D to cache it.
  void CacheTexture (iPolygonTexture *handle);

  /// Remove a texture from cache (perhaps because it's being unregistered)
  void UncacheTexture (iTextureHandle *texture);
  
  /// Get the capabilities of this driver: NOT IMPLEMENTED.
  virtual void GetCaps(G3D_CAPS *caps);
  
  /// Dump the texture cache.
  virtual void DumpCache(void);
  
  /// Clear the texture cache.
  virtual void ClearCache(void);

  /// Remove some polygon from the cache.
  virtual void RemoveFromCache (iPolygonTexture* /*poly_texture*/) { }

  /// Print the screen.
  virtual void Print (csRect* rect) { m_piG2D->Print(rect); }
  
  /// Set a render state
  virtual bool SetRenderState (G3D_RENDERSTATEOPTION, long);
  /// Get a render state
  virtual long GetRenderState (G3D_RENDERSTATEOPTION);
  
  /// Get a z-buffer point
  virtual unsigned long *GetZBufPoint(int, int) { return NULL; }
  
  /// Get the width
  virtual int GetWidth () { return m_nWidth; }
  ///
  virtual int GetHeight () { return m_nHeight; }

  /// Set center of projection.
  virtual void SetPerspectiveCenter(int x, int y) 
  { 
    m_nHalfWidth  = x;
    m_nHalfHeight = y;
  }
  /// Set perspective aspect. 
  virtual void SetPerspectiveAspect (float aspect)
  {
    m_Aspect    = aspect;
    m_InvAspect = 1.0/aspect;
  }
  /// Set world to camera transformation. 
  virtual void SetObjectToCamera (csTransform* o2c) 
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
 
  /** Adjust the given texture size to an optimal size. This will take into
   *  consideration maximum sizes, limitations to power of two, max aspect
   *  ratio and so on.
   */
  virtual void AdjustToOptimalTextureSize(int& w, int& h);

  /// Returns true if the driver needs PO2 lightmaps and texture maps
  virtual bool NeedsPO2Maps(void) { return true; }
  ///
  virtual int GetMaximumAspectRatio () { return m_MaxAspectRatio; }
  
  /// Get the fog mode.
  virtual G3D_FOGMETHOD GetFogMode () { return G3DFOGMETHOD_VERTEX; }

  /// Get the fog mode.
  virtual bool SetFogMode (G3D_FOGMETHOD fogm)
  { return (fogm == G3DFOGMETHOD_VERTEX); }

  /// Get the ITextureManager.
  virtual iTextureManager *GetTextureManager ()
  { return txtmgr; }

  /// Get the texture cache.
  D3DTextureCache*  GetTextureCache()  {return m_pTextureCache;}

  /// Get the lightmap cache.
  D3DLightMapCache* GetLightmapCache() {return m_pLightmapCache;}

  /// 
  virtual iGraphics2D *GetDriver2D () 
  { return m_piG2D; }

  /// Get Z-buffer value at given X,Y position
  virtual float GetZbuffValue (int /*x*/, int /*y*/)
  { return 0; }

  /// Create a halo of the specified color and return a handle.
  virtual iHalo *CreateHalo (float /*iR*/, float /*iG*/, float /*iB*/,
    unsigned char* /*iAlpha*/, int /*iWidth*/, int /*iHeight*/)
  { return NULL; }

  virtual void OpenFogObject (CS_ID id, csFog* fog);
  virtual void AddFogPolygon (CS_ID id, G3DPolygonAFP& poly, int fogtype);
  virtual void CloseFogObject (CS_ID id);

private:
  
  // Z-Buffer enumeration callback  
  static HRESULT CALLBACK EnumZBufferFormatsCallback(LPDDPIXELFORMAT lpddpf, LPVOID lpUserArg);

  // Microsoft changed their API for DX6, now it is not DDSURFACEDESC2 but DDPIXELFORMAT ... stupid
  static HRESULT CALLBACK EnumPixelFormatsCallback(LPDDPIXELFORMAT lpddpf, LPVOID lpUserArg);
  /// used to get the appropriate device.
  bool EnumDevices(void);
  /// whether the device is locked or not.
  bool m_bIsLocked;
  
  /// used to set up polygon geometry before rasterization.
  inline void SetupPolygon( G3DPolygonDP& poly, float& J1, float& J2, float& J3, 
    float& K1, float& K2, float& K3,
    float& M,  float& N,  float& O  );

  void ConfigureRendering();

  void MultitextureDrawPolygon(G3DPolygonDP & poly);

  void BatchStartPolygonFX(iTextureHandle* handle, UInt mode);
  void BatchFinishPolygonFX();
  void BatchDrawPolygonFX(G3DPolygonDPFX& poly);

};

#endif // G3D_D3D_H
