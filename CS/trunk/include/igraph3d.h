/*
    Copyright (C) 1998 by Jorrit Tyberghein
    Written by Jorrit Tyberghein, Dan Ogles, and Gary Clark.

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

#ifndef __IGRAPH3D_H__
#define __IGRAPH3D_H__

#include "cscom/com.h"

//@@@BAD!!!
#include "csengine/fog.h"

class csVector2;
class csMatrix3;
class csVector3;
class csRect;

interface IPolygon3D;
interface IPolygonTexture;
interface IGraphics2D;
interface ISystem;
interface ITextureManager;
interface ITextureHandle;

#define CS_FOG_FRONT		0
#define CS_FOG_BACK		1
#define CS_FOG_VIEW		2

/**
 * Mix modes for DrawPolygonFX ()
 * The constants below can be ORed together if they belong to different masks.
 */ 
#define CS_FX_MASK_MIXMODE	0xF0000000	// SRC/DST mixing mode mask
#define CS_FX_COPY		0x00000000	// =SRC
#define CS_FX_MULTIPLY		0x10000000	// =SRC*DST
#define CS_FX_MULTIPLY2		0x20000000	// =2*SRC*DST
#define CS_FX_ADD		0x30000000	// =SRC+DST
#define CS_FX_ALPHA		0x40000000	// =(1-alpha)*SRC + alpha*DST
#define CS_FX_TRANSPARENT	0x50000000	// =DST
#define CS_FX_KEYCOLOR		0x08000000	// color 0 is transparent
#define CS_FX_GOURAUD		0x04000000	// Gouraud shading
#define CS_FX_MASK_ALPHA	0x000000FF	// alpha = 0..FF (opaque..transparent)

/// Macro for easier setting of alpha bits into mixmode
#define CS_FX_SETALPHA(alpha)	(CS_FX_ALPHA | UInt (alpha * CS_FX_MASK_ALPHA))

/// Vertex Structure for use with G3DPolygonDP and G3DPolygonAFP
class G3DVertex
{
public:
  /// Screen x space.
  float sx;
  /// Screen y space.
  float sy;
};

/// Vertex Structure for use with G3DPolygonDPQ
class G3DTexturedVertex : public G3DVertex
{
public:
  /// z value
  float z;
  
  //Texture coordinates
  float u, v; 

  //Lighting info (Used only with Gouroud shading (between 0 and 1))
  float r, g, b;
};

/// Extra information for vertex fogging.
class G3DFogInfo
{
public:
  /// Color.
  float r, g, b;
  /// Intensity (== density * thickness).
  float intensity;
};

///
class G3DTexturePlane
{
public:
  /// Transformation from camera space to texture space.
  csMatrix3* m_cam2tex;
  /// Transformation from camera space to texture space.
  csVector3* v_cam2tex;
};

///
class G3DPolyNormal
{
public:
  ///
  float A;
  ///
  float B;
  ///
  float C;
  ///
  float D;
};


///Structure containing all info needed by DrawPolygonFX (DPFX)
struct G3DPolygonDPFX
{
  /// Current number of vertices.
  int num;
  /// Vertices that form the polygon.
  G3DTexturedVertex vertices[100];
  /// Extra optional fog information.
  G3DFogInfo fog_info[100];
  /// Use fog info?
  bool use_fog;

  /// Invert aspect ratio that was used to perspective project the vertices (1/fov)
  float inv_aspect;

  /// The texture handle as returned by ITextureManager.
  ITextureHandle* txt_handle;

  /// Use this color for drawing (if txt_handle == NULL) instead of a texture.
  float flat_color_r;
  float flat_color_g;
  float flat_color_b;
};

///Structure containing all info needed by DrawPolygon (DP)
struct G3DPolygonDP
{
  /// Current number of vertices.
  int num;
  /// Vertices that form the polygon.
  G3DVertex vertices[100];
  /// Extra optional fog information.
  G3DFogInfo fog_info[100];
  /// Use fog info?
  bool use_fog;

  /// Invert aspect ratio that was used to perspective project the vertices (1/fov)
  float inv_aspect;

  /// The texture handle as returned by ITextureManager.
  ITextureHandle* txt_handle;

  /// Transformation matrices for the texture. @@@ BAD NAME
  G3DTexturePlane plane;
  /// The plane equation in camera space of this polygon. @@@ BAD NAME
  G3DPolyNormal normal;

  /// Use this color for drawing (if txt_handle == NULL) instead of a texture.
  float flat_color_r;
  float flat_color_g;
  float flat_color_b;

  ///Handle to lighted textures (texture+lightmap) (for all mipmap levels)
  IPolygonTexture* poly_texture[4];

  /** 
    * AlphaValue of the polygon. Ranges from 0 to 100. 0 means opaque, 100 is 
    * comletely transparent.
    */
  int alpha;

  ///true, if it is ok, to use mipmaps
  bool uses_mipmaps;

  ///z value (in camera space) of vertex[0]
  float z_value;
};

///Structure containing all info needed by DrawPolygonFlat (DPF)
typedef G3DPolygonDP G3DPolygonDPF;

///Structure containing all info needed by AddFogPolygon (AFP)
struct G3DPolygonAFP
{
  /// Current number of vertices.
  int num;
  /// Vertices that form the polygon.
  G3DVertex vertices[100];

  /// Invert aspect ratio that was used to perspective project the vertices (1/fov)
  float inv_aspect;

  /// The plane equation in camera space of this polygon. @@@ BAD NAME
  G3DPolyNormal normal;
};

///
class G3DFltLight
{
public:
  // all points are screen-projections.
  // this is a 2d circle (it's incorrect, but works well as an approx)
  float sx, sy, sz;
  float srad;
  float sqdist;       // camera space: radius squared.
  float r, g, b;
};

/// Don't test/write, write, test, and write/test, respectively.
enum G3DZBufMode
{
  CS_ZBUF_NONE = 0,
  CS_ZBUF_FILL = 1,
  CS_ZBUF_TEST = 2,
  CS_ZBUF_USE  = 3
};

///
enum G3D_RENDERSTATEOPTION
{
  G3DRENDERSTATE_NOTHING,
  G3DRENDERSTATE_ZBUFFERTESTENABLE,
  G3DRENDERSTATE_ZBUFFERFILLENABLE,
  G3DRENDERSTATE_DITHERENABLE,
  G3DRENDERSTATE_SPECULARENABLE,
  G3DRENDERSTATE_BILINEARMAPPINGENABLE,
  G3DRENDERSTATE_TRILINEARMAPPINGENABLE,
  G3DRENDERSTATE_TRANSPARENCYENABLE,
  G3DRENDERSTATE_MIPMAPENABLE,
  G3DRENDERSTATE_DEBUGENABLE,
  G3DRENDERSTATE_TEXTUREMAPPINGENABLE,
  G3DRENDERSTATE_FILTERINGENABLE,
  G3DRENDERSTATE_PERFECTMAPPINGENABLE,	// OBSOLETE
  G3DRENDERSTATE_LIGHTINGENABLE,
  G3DRENDERSTATE_INTERLACINGENABLE,
  G3DRENDERSTATE_MMXENABLE,
  G3DRENDERSTATE_INTERPOLATIONSTEP,
  G3DRENDERSTATE_MAXPOLYGONSTODRAW,
  G3DRENDERSTATE_GOURAUDENABLE
};

/// Bit flags for IGraphics3D::BeginDraw ()
/// We're going to draw 2D graphics
#define CSDRAW_2DGRAPHICS   0x00000001
/// We're going to draw 3D graphics
#define CSDRAW_3DGRAPHICS   0x00000002
/// Clear Z-buffer ?
#define CSDRAW_CLEARZBUFFER 0x00000010
/// Clear frame buffer ?
#define CSDRAW_CLEARSCREEN  0x00000020

///
enum G3D_COLORMODEL
{
  G3DCOLORMODEL_MONO,
  G3DCOLORMODEL_RGB
};

///
enum G3D_RASTERCAPS
{
  G3DRASTERCAPS_DITHER = 0x01,
  G3DRASTERCAPS_SUBPIXEL = 0x02,
  G3DRASTERCAPS_ZBUFFERLESSHSR = 0x04
};

///
enum G3D_SHADECAPS
{
  G3DRASTERCAPS_FLAT = 1,
  G3DRASTERCAPS_GOURAUD = 2,
  G3DRASTERCAPS_PHONG = 4,
  G3DRASTERCAPS_LIGHTMAP = 8
};

///
enum G3D_FILTERCAPS
{
  G3DFILTERCAPS_LINEAR = 0x01,
  G3DFILTERCAPS_LINEARMIPLINEAR = 0x02,
  G3DFILTERCAPS_LINEARMIPNEAREST = 0x04,
  G3DFILTERCAPS_MIPLINEAR = 0x08,
  G3DFILTERCAPS_MIPNEAREST = 0x10,
  G3DFILTERCAPS_NEAREST = 0x20
};

///
enum G3D_FOGMETHOD
{
  G3DFOGMETHOD_NONE = 0x00,
  G3DFOGMETHOD_ZBUFFER = 0x01,
  G3DFOGMETHOD_VERTEX = 0x02
};

///
struct G3D_PRIMCAPS
{
  G3D_RASTERCAPS RasterCaps;
  bool canBlend;
  G3D_SHADECAPS ShadeCaps;
  bool PerspectiveCorrects;
  G3D_FILTERCAPS FilterCaps;
};

///
struct G3D_CAPS
{
  G3D_COLORMODEL ColorModel;
  bool CanClip;
  bool SupportsArbitraryMipMapping;
  int BitDepth;
  int ZBufBitDepth;
  int minTexHeight, minTexWidth;
  int maxTexHeight, maxTexWidth;
  G3D_PRIMCAPS PrimaryCaps;
  G3D_FOGMETHOD fog;
};

enum G3D_COLORMAPFORMAT
{
  G3DCOLORFORMAT_ANY,             // can use either private color maps or global color maps
  G3DCOLORFORMAT_PRIVATE,         // only uses private color maps.
  G3DCOLORFORMAT_GLOBAL,          // only uses global color maps.
  G3DCOLORFORMAT_24BIT            // only uses 24 bit textures
};

extern const GUID IID_IGraphics3D;

/**
 * This is the standard 3D graphics interface.
 * All 3D graphics rasterizer servers for Crystal Space
 * should implement this interface, as well as the 
 * IGraphics2D and IGraphicsInfo interface.
 * The standard implementation is csGraphics3DSoftware.
 */
interface IGraphics3D : public IUnknown
{
public:
  /// Initialize the 3D graphics system.
  STDMETHOD (Initialize) () PURE;

  /// Open the 3D graphics display.
  STDMETHOD (Open) (char *Title) PURE;
  /// Close the 3D graphics display.
  STDMETHOD (Close) () PURE;

  /// Change the dimensions of the display.
  STDMETHOD (SetDimensions) (int width, int height) PURE;

  /// Start a new frame (see CSDRAW_XXX bit flags)
  STDMETHOD (BeginDraw) (int DrawFlags) PURE;

  /// End the frame and do a page swap.
  STDMETHOD (FinishDraw) () PURE;

  /// Print the image in backbuffer
  STDMETHOD (Print) (csRect *area) PURE;

  /// Set the mode for the Z buffer used for drawing the next polygon.
  STDMETHOD (SetZBufMode) (G3DZBufMode mode) PURE;

  /// Draw the projected polygon with light and texture.
  STDMETHOD (DrawPolygon) (G3DPolygonDP& poly) PURE;

  /**
   * Draw the projected polygon with light and texture.
   * Debugging version. This one does not actually draw anything
   * but it just prints debug information about what it would have
   * done.
   */
  STDMETHOD (DrawPolygonDebug) ( G3DPolygonDP& poly) PURE;

  /// Draw a line in camera space.
  STDMETHOD (DrawLine) (csVector3& v1, csVector3& v2, float fov, int color) PURE;

  /**
   * Prepare for drawing a series of Polygon FX which all use
   * the same settings. You must call this function before calling a
   * series of DrawPolygonFX(). After calling the series you should
   * call FinishPolygonFX().<p>
   *
   * Warning! After calling this function you are not allowed to do
   * any calls to the 3D rasterizer other than DrawPolygonFX() and
   * FinishPolygonFX().
   *
   * Warning! Do not rely on this method to handle Color keying under
   * all circumstances. Color Keying will only work reliable in Mixmodes
   * FX_Copy, FX_Add and FX_Transparent. When using FX_Multiply
   * and FX_Multiply2, it depends very much on the driver if it works or
   * not. For example the RivaTNT Detonator 0.48 driver can display 
   * Multiply with color keying, while newer versions can't. They will 
   * then not display anything at all. It is always safer to use a texture
   * where transparent sections are white or 50% gray if you want to achieve
   * transparent sections in Multiply, Multiply2.
   * There are also some drivers (which I would consider buggy...), that won't
   * display FX_Alpha correctly with Color Keying. I can't provide a valid 
   * workaround for that except using FX_Multiplay and FX_Add, to manually
   * create the image, but that would be very expensive.
   * 
   * parameters:
   * handle:  The texture handle as returned by ITextureManager.
   * mode:    How shall the new polygon be combined with the current 
   *          screen content. This is any legal combination of CS_FX_XXX
   *          flags including alpha value (if CS_FX_ALPHA flag is set)
   */
  STDMETHOD (StartPolygonFX) (ITextureHandle* handle, UInt mode) PURE;

  /**
   * Finish drawing a series of Polygon FX.
   */
  STDMETHOD (FinishPolygonFX) () PURE;

  /**
   * Draw a polygon with special effects. This is the most rich and slowest
   * variant of DrawPolygonXxx. (If you use these features) 
   */
  STDMETHOD (DrawPolygonFX) (G3DPolygonDPFX& poly) PURE;

  /// Get the current fog mode (G3D_FOGMETHOD).
  COM_METHOD_DECL GetFogMode (G3D_FOGMETHOD& fogMethod) = 0;

  /// Set the current fog mode as supported by this 3D rasterizer.
  COM_METHOD_DECL SetFogMode (G3D_FOGMETHOD fogMethod) = 0;

  /**
   * Initiate a volumetric fog object. This function will be called
   * before front-facing and back-facing fog polygons are added to
   * the object. The fog object will be convex but not necesarily closed.
   * The given CS_ID can be used to identify multiple fog objects when
   * multiple objects are started.
   */
  STDMETHOD (OpenFogObject) (CS_ID id, csFog* fog) PURE;

  /**
   * Add a front or back-facing fog polygon in the current fog object.
   * Note that it is guaranteed that all back-facing fog polygons
   * will have been added before the first front-facing polygon.
   * fogtype can be:<br>
   * <ul>
   *	<li>CS_FOG_FRONT:	a front-facing polygon
   *	<li>CS_FOG_BACK:	a back-facing polygon
   *	<li>CS_FOG_VIEW:	the view-plane
   * </ul>
   */
  STDMETHOD (AddFogPolygon) (CS_ID id, G3DPolygonAFP& poly, int fogtype) PURE;

  /**
   * Close a volumetric fog object. After the volumetric object is
   * closed it should be rendered on screen (whether you do it here
   * or in AddFogPolygon is not important).
   */
  STDMETHOD (CloseFogObject) (CS_ID id) PURE;

  /// Give a texture to csGraphics3D to cache it.
  STDMETHOD (CacheTexture) (IPolygonTexture* texture) PURE;

  /// Release a texture from the cache.
  STDMETHOD (UncacheTexture) (IPolygonTexture* texture) PURE;

  /// Set a renderstate value.
  STDMETHOD (SetRenderState) (G3D_RENDERSTATEOPTION op, long val) PURE;

  /// Get a renderstate value.
  STDMETHOD (GetRenderState) (G3D_RENDERSTATEOPTION op, long& val) PURE;

  /**
   * Get the current driver's capabilities. Each driver implements their
   * own function.
   */
  STDMETHOD (GetCaps) (G3D_CAPS* caps) PURE;

  /// Debugging: get the value of the Z buffer at some location.
  STDMETHOD (GetZBufPoint) (int x, int y, unsigned long** nResult) PURE;

  /// Set the texture cache size.
  STDMETHOD (SetCacheSize) (long size) PURE;

  /// Dump the texture cache.
  STDMETHOD (DumpCache) (void) PURE;

  /// Clear the texture cache.
  STDMETHOD (ClearCache) (void) PURE;

  /// Get drawing buffer width
  STDMETHOD (GetWidth) (int& nResult) PURE;
  /// Get drawing buffer height
  STDMETHOD (GetHeight) (int& nResult) PURE;

  /// Get the texture representation scheme.
  STDMETHOD (GetColormapFormat) (G3D_COLORMAPFORMAT& g3dFormat) PURE;

  /// Returns S_OK if the driver needs PO2 lightmaps and texture maps, S_FALSE otherwise.
  STDMETHOD (NeedsPO2Maps) () PURE;

  /// Get the maximum aspect ratio of texture maps.
  STDMETHOD (GetMaximumAspectRatio) (int& retval) PURE;

  /// Get the 2D Driver
  STDMETHOD (Get2dDriver) (IGraphics2D** piG2D) PURE;

  /// Get the texture manager.
  STDMETHOD (GetTextureManager) (ITextureManager** piTxtMgr) PURE;
};

extern const IID IID_IGraphicsContextFactory;
/// 
interface IGraphicsContextFactory : public IUnknown
{
  /// Create the graphics context
  STDMETHOD (CreateInstance) (REFIID riid, ISystem* piSystem, void** ppv) PURE;

  /// Lock or unlock from memory.
  STDMETHOD (LockServer) (COMBOOL bLock) PURE;
};

#endif // __IGRAPH3D_H__
