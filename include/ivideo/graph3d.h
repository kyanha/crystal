/*
    Copyright (C) 1998-2001 by Jorrit Tyberghein
                       2004 by Marten Svanfeldt
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

#ifndef __CS_IVIDEO_GRAPH3D_H__
#define __CS_IVIDEO_GRAPH3D_H__

/**\file
 * 3D graphics interface
 */

/**
 * \addtogroup gfx3d
 * @{ */
 
#include "csutil/refarr.h"
#include "csutil/scf.h"
#include "csgeom/plane3.h"
#include "csgeom/vector2.h"
#include "csgeom/transfrm.h"
#include "csgeom/tri.h"
#include "csutil/cscolor.h"
#include "ivideo/rndbuf.h"
#include "ivideo/polyrender.h"

struct csCoreRenderMesh;
struct csRenderMeshModes;
class csMatrix3;
class csVector4;
class csVector3;
class csVector2;
class csPlane3;
class csRect;
class csReversibleTransform;
class csShaderVariable;

struct iGraphics2D;
struct iPolygonBuffer;
struct iPolygonRenderer;
struct iTextureManager;
struct iTextureHandle;
struct iMaterialHandle;
struct iMaterial;
struct iClipper2D;
struct iHalo;
struct iRendererLightmap;
struct csRGBpixel;
struct csPixelFormat;
struct csPolyTextureMapping;
struct iRenderBuffer;
struct iRenderBufferManager;
struct iLightingManager;
struct iShader;
struct iShaderVariableContext;
struct csRenderBufferHolder;

/**\name iGraphics3D::BeginDraw() flags
 * @{ */
/// We're going to draw 2D graphics
#define CSDRAW_2DGRAPHICS   0x00000001
/// We're going to draw 3D graphics
#define CSDRAW_3DGRAPHICS   0x00000002
/// Clear Z-buffer ?
#define CSDRAW_CLEARZBUFFER 0x00000010
/// Clear frame buffer ?
#define CSDRAW_CLEARSCREEN  0x00000020
/** @} */

/**\name Type of clipper (for iGraphics3D::SetClipper())
 * @{ */
/**
 * There is no clipper.
 */
#define CS_CLIPPER_NONE -1
/**
 * Clipper is optional.
 */
#define CS_CLIPPER_OPTIONAL 0
/**
 * Clipper is top-level.
 */
#define CS_CLIPPER_TOPLEVEL 1
/**
 * Clipper is required.
 */
#define CS_CLIPPER_REQUIRED 2
/** @} */

/**\name Clipping requirement for DrawTriangleMesh
 * @{ */
/**
 * No clipping required.
 * (setting for clip_portal, clip_plane, or clip_z_plane).
 */
#define CS_CLIP_NOT 0
/**
 * Clipping may be needed. Depending on the type of the clipper
 * (one of the CS_CLIPPER_??? flags) the renderer has to clip or
 * not. (setting for clip_portal, clip_plane, or clip_z_plane).
 */
#define CS_CLIP_NEEDED 1
/** @} */

/**
 * Fog structure.
 */
struct csFog
{
  /// If true then fog is enabled.
  bool enabled;
  /// Density (0 is off).
  float density;
  /// Color (red).
  float red;
  /// Color (green).
  float green;
  /// Color (blue).
  float blue;
};

/// Z-buffer modes
enum csZBufMode
{
  // values below are sometimes used as bit masks, so don't change them!
  /// Don't test/write
  CS_ZBUF_NONE     = 0x00000000,
  /// write
  CS_ZBUF_FILL     = 0x00000001,
  /// test
  CS_ZBUF_TEST     = 0x00000002,
  /// write/test
  CS_ZBUF_USE      = 0x00000003,
  /// only write
  CS_ZBUF_FILLONLY = 0x00000004,
  /// test if equal
  CS_ZBUF_EQUAL    = 0x00000005,
  /// inverted test
  CS_ZBUF_INVERT   = 0x00000006,
  /// use the z mode of the mesh (NOTE: NOT VALID AS MESH ZMODE)
  CS_ZBUF_MESH     = 0x00000007,
  /**
   * Use a z mode depending on the mesh zmode.
   * The mesh zmode is used a to choose a zmode that makes sure only pixels
   * that are changed by the mesh zmode can be touched, e.g. if the mesh has a
   * zmode of "zuse", zmesh2 will resolve to "ztest". This is useful for multi-
   * pass stuff.
   * (NOTE: NOT VALID AS MESH ZMODE)
   */
  CS_ZBUF_MESH2    = 0x00000008
};

// @@@ Keep in sync with values below
// @@@ Document me better!
#define CS_VATTRIB_SPECIFIC_FIRST    0
#define CS_VATTRIB_SPECIFIC_LAST    15
#define CS_VATTRIB_GENERIC_FIRST   100
#define CS_VATTRIB_GENERIC_LAST    (CS_VATTRIB_GENERIC_FIRST + 15)

#define CS_VATTRIB_IS_GENERIC(va)   \
  ((va >= CS_VATTRIB_GENERIC_FIRST) && (va <=CS_VATTRIB_GENERIC_LAST))
#define CS_VATTRIB_IS_SPECIFIC(va)   \
  ((va >= CS_VATTRIB_SPECIFIC_FIRST) && (va <=CS_VATTRIB_SPECIFIC_LAST))

/**
 * For NR:
 * Vertex attributes.
 */
enum csVertexAttrib
{
  /// Position vertex attribute
  CS_VATTRIB_POSITION	      = CS_VATTRIB_SPECIFIC_FIRST + 0,
  /// Vertex weight attribute
  CS_VATTRIB_WEIGHT	      = CS_VATTRIB_SPECIFIC_FIRST + 1,
  /// Normal attribute
  CS_VATTRIB_NORMAL	      = CS_VATTRIB_SPECIFIC_FIRST + 2,
  /// Primary color attribute
  CS_VATTRIB_COLOR	      = CS_VATTRIB_SPECIFIC_FIRST + 3,
  /// Primary color attribute
  CS_VATTRIB_PRIMARY_COLOR    = CS_VATTRIB_SPECIFIC_FIRST + 3,
  /// Secondary color attribute
  CS_VATTRIB_SECONDARY_COLOR  = CS_VATTRIB_SPECIFIC_FIRST + 4,
  /// Fog coordinate attribute
  CS_VATTRIB_FOGCOORD	      = CS_VATTRIB_SPECIFIC_FIRST + 5,
  /// TU 0 texture coordinates
  CS_VATTRIB_TEXCOORD	      = CS_VATTRIB_SPECIFIC_FIRST + 8,
  /// TU 0 texture coordinates
  CS_VATTRIB_TEXCOORD0	      = CS_VATTRIB_SPECIFIC_FIRST + 8,
  /// TU 1 texture coordinates
  CS_VATTRIB_TEXCOORD1	      = CS_VATTRIB_SPECIFIC_FIRST + 9,
  /// TU 2 texture coordinates
  CS_VATTRIB_TEXCOORD2	      = CS_VATTRIB_SPECIFIC_FIRST + 10,
  /// TU 3 texture coordinates
  CS_VATTRIB_TEXCOORD3	      = CS_VATTRIB_SPECIFIC_FIRST + 11,
  /// TU 4 texture coordinates
  CS_VATTRIB_TEXCOORD4	      = CS_VATTRIB_SPECIFIC_FIRST + 12,
  /// TU 5 texture coordinates
  CS_VATTRIB_TEXCOORD5	      = CS_VATTRIB_SPECIFIC_FIRST + 13,
  /// TU 6 texture coordinates
  CS_VATTRIB_TEXCOORD6	      = CS_VATTRIB_SPECIFIC_FIRST + 14,
  /// TU 7 texture coordinates
  CS_VATTRIB_TEXCOORD7	      = CS_VATTRIB_SPECIFIC_FIRST + 15,
  //@{
  /**
  * General vertex attribute
  */
  CS_VATTRIB_0	= CS_VATTRIB_GENERIC_FIRST + 0,
  CS_VATTRIB_1	= CS_VATTRIB_GENERIC_FIRST + 1,
  CS_VATTRIB_2	= CS_VATTRIB_GENERIC_FIRST + 2,
  CS_VATTRIB_3	= CS_VATTRIB_GENERIC_FIRST + 3,
  CS_VATTRIB_4	= CS_VATTRIB_GENERIC_FIRST + 4,
  CS_VATTRIB_5	= CS_VATTRIB_GENERIC_FIRST + 5,
  CS_VATTRIB_6	= CS_VATTRIB_GENERIC_FIRST + 6,
  CS_VATTRIB_7	= CS_VATTRIB_GENERIC_FIRST + 7,
  CS_VATTRIB_8	= CS_VATTRIB_GENERIC_FIRST + 8,
  CS_VATTRIB_9	= CS_VATTRIB_GENERIC_FIRST + 9,
  CS_VATTRIB_10 = CS_VATTRIB_GENERIC_FIRST + 10,
  CS_VATTRIB_11 = CS_VATTRIB_GENERIC_FIRST + 11,
  CS_VATTRIB_12 = CS_VATTRIB_GENERIC_FIRST + 12,
  CS_VATTRIB_13 = CS_VATTRIB_GENERIC_FIRST + 13,
  CS_VATTRIB_14 = CS_VATTRIB_GENERIC_FIRST + 14,
  CS_VATTRIB_15 = CS_VATTRIB_GENERIC_FIRST + 15
  //@}
};

/// 
enum G3D_FOGMETHOD
{
  G3DFOGMETHOD_NONE = 0x00,
  G3DFOGMETHOD_ZBUFFER = 0x01,
  G3DFOGMETHOD_VERTEX = 0x02
};

/**\name Mix modes for DrawPolygonFX ()
 * The constants can be ORed together if they belong to different masks.
 * @{ */
/// SRC/DST mixing mode mask
#define CS_FX_MASK_MIXMODE 0xF0000000 
/// =SRC
#define CS_FX_COPY         0x00000000 
/// =SRC*DST
#define CS_FX_MULTIPLY     0x10000000 
/// =2*SRC*DST
#define CS_FX_MULTIPLY2    0x20000000 
/// =SRC+DST
#define CS_FX_ADD          0x30000000 
/// =(1-alpha)*SRC + alpha*DST
#define CS_FX_ALPHA        0x40000000 
/// =DST
#define CS_FX_TRANSPARENT  0x50000000 
/// =(dstalpha)*SRC + DST
#define CS_FX_DESTALPHAADD 0x60000000 
/// =(srcalpha)*SRC + DST
#define CS_FX_SRCALPHAADD  0x70000000
/// =SRC + DST*(1-srcalpha)
#define CS_FX_PREMULTALPHA 0x80000000
/**
  * Use the mix mode of the mesh zmode.
  * (NOTE: NOT VALID AS MESH ZMODE - only for shader pass mixmodes)
  */
#define CS_FX_MESH	   0xf0000000
/// color 0 is transparent
#define CS_FX_KEYCOLOR     0x08000000 
/// Flat shading
#define CS_FX_FLAT         0x04000000 
/// Tiling
#define CS_FX_TILING       0x02000000 
/// alpha = 0..FF (opaque..transparent)
#define CS_FX_MASK_ALPHA   0x000000FF 

/// Macro for setting of alpha bits into mixmode (alpha between 0 and 1).
#define CS_FX_SETALPHA(alpha) \
  (CS_FX_ALPHA | uint (alpha * CS_FX_MASK_ALPHA))
/// Macro for setting of alpha bits into mixmode (alpha between 0 and 255).
#define CS_FX_SETALPHA_INT(alpha) \
  (CS_FX_ALPHA | uint (alpha & CS_FX_MASK_ALPHA))
/** @} */

/**
 * Describes how to deal with alpha values in textures.
 */
struct csAlphaMode
{
  /// How to handle alpha
  enum AlphaType
  {
    /// Ignore alpha
    alphaNone = 1,
    /// Binary alpha (pixels with alpha >0.5 are drawn, all others not)
    alphaBinary,
    /// 'Smooth' alpha (colors are mixed based on a pixel's alpha value)
    alphaSmooth
  };
  /// Whether 'automatic alpha mode' should be used.
  bool autoAlphaMode;
  union
  {
    /// Alpha mode to use when autoAlphaMode is \p false
    AlphaType alphaType;
    /// Texture to retrieve the alpha mode from when autoAlphaMode is \p true
    csStringID autoModeTexture;
  };
};

/**\name Light parameters
 * @{ */
/// Position of the light.
#define CS_LIGHTPARAM_POSITION 0
/// Diffuse color of the light.
#define CS_LIGHTPARAM_DIFFUSE 1
/// Specular color of the light.
#define CS_LIGHTPARAM_SPECULAR 2
/// Attenuation of the light.
#define CS_LIGHTPARAM_ATTENUATION 3
/** @} */

/**\name Shadow states
 * @{ */
/// Clear stencil.
#define CS_SHADOW_VOLUME_BEGIN 1
/// Setup for pass 1.
#define CS_SHADOW_VOLUME_PASS1 2
/// Setup for pass 2.
#define CS_SHADOW_VOLUME_PASS2 3
/// Setup for carmack's reverse pass 1.
#define CS_SHADOW_VOLUME_FAIL1 4
/// Setup for carmack's reverse pass 2.
#define CS_SHADOW_VOLUME_FAIL2 5
/// Setup for shadow masking.
#define CS_SHADOW_VOLUME_USE 6
/// Restore states.
#define CS_SHADOW_VOLUME_FINISH 7
/** @} */

/// Graphics3D render state options
enum G3D_RENDERSTATEOPTION
{
  /// Set Z-buffer fill/test/use mode (parameter is a csZBufMode)
  G3DRENDERSTATE_ZBUFFERMODE,
  /// Enable/disable dithering (parameter is a bool)
  G3DRENDERSTATE_DITHERENABLE,
  /// Enable/disable bi-linear mapping (parameter is a bool)
  G3DRENDERSTATE_BILINEARMAPPINGENABLE,
  /// Enable/disable tri-linear mapping (parameter is a bool)
  G3DRENDERSTATE_TRILINEARMAPPINGENABLE,
  /// Enable/disable transparent textures (parameter is a bool)
  G3DRENDERSTATE_TRANSPARENCYENABLE,
  /// Enable/disable mip-mapping (parameter is a bool)
  G3DRENDERSTATE_MIPMAPENABLE,
  /// Enable/disable textures (parameter is a bool)
  G3DRENDERSTATE_TEXTUREMAPPINGENABLE,
  /// Enable/disable lighting (parameter is a bool)
  G3DRENDERSTATE_LIGHTINGENABLE,
  /// Enable/disable interlacing (parameter is a bool)
  G3DRENDERSTATE_INTERLACINGENABLE,
  /// Enable/disable MMX instructions usage (parameter is a bool)
  G3DRENDERSTATE_MMXENABLE,
  /// Set perspective-correction interpolation step (parameter is a int)
  G3DRENDERSTATE_INTERPOLATIONSTEP,
  /// Set maximal number of polygons per frame to draw (parameter is a int)
  G3DRENDERSTATE_MAXPOLYGONSTODRAW,
  /// Enable/disable Gouraud shading (parameter is a bool)
  G3DRENDERSTATE_GOURAUDENABLE,
  /// Enable/disable edge drawing (debugging) (parameter is a bool)
  G3DRENDERSTATE_EDGES
};

/// Information about 3d renderer capabilities.
struct csGraphics3DCaps
{
  bool CanClip;
  int minTexHeight, minTexWidth;
  int maxTexHeight, maxTexWidth;
  G3D_FOGMETHOD fog;
  bool NeedsPO2Maps;
  int MaxAspectRatio;

  // The following caps are only used by NR

  /**
   * Whether point sprites are supported. If \a true, geometry of the type
   * CS_MESHTYPE_POINT_SPRITES can be drawn.
   */
  bool SupportsPointSprites;
  /**
   * Mixmodes utilizing destination alpha are properly supported.
   */
  bool DestinationAlpha;
  /**
   * Enough stencil bits for stencil shadows are available.
   */
  bool StencilShadows;
};

/// Primitive type of a mesh
enum csRenderMeshType
{
  /// Triangles.
  CS_MESHTYPE_TRIANGLES,
  /// Quads.
  CS_MESHTYPE_QUADS,
  /**
   * Triangle strip.
   * The OpenGL spec describes it pretty well:
   * "A triangle strip is a series of triangles connected along shared edges. 
   * A triangle strip is specified by giving a series of defining vertices 
   * [...]. In this case, the first three vertices define the first triangle 
   * [...]. Each subsequent  vertex defines a new triangle using that point 
   * along with two vertices from the previous triangle."
   */
  CS_MESHTYPE_TRIANGLESTRIP,
  /**
   * Triangle fan.
   * Similar to a triangle strip, however, a triangle is always defined with
   * the first, previously added and the recently added vertex.
   */
  CS_MESHTYPE_TRIANGLEFAN,
  /**
   * Points.
   */
  CS_MESHTYPE_POINTS,
  /**
   * Point sprites. 
   * Note: only supported if the \a SupportsPointSprites member of the 
   * \a csGraphics3DCaps structure for this renderer is true.
   */
  CS_MESHTYPE_POINT_SPRITES,
  /**
   * Lines.
   */
  CS_MESHTYPE_LINES,
  /**
   * Line strip.
   * A line is defined from the prebviously and recently added vertex.
   */
  CS_MESHTYPE_LINESTRIP,
  /**
   * Render polygons. Note that you <b>*must*</b> supply geometry with the
   * help of an iPolygonRenderer. In the common case, if you want to draw a
   * polygon, you probably want to use CS_MESHTYPE_TRIANGLES or 
   * CS_MESHTYPE_TRIANGLEFAN and triangulate the poly yourself. 
   */
  CS_MESHTYPE_POLYGON
};

/**
 * Flags to influence the behaviour of DrawSimpleMesh().
 */
enum csSimpleMeshFlags
{
  /**
   * Ignore the object2camera transform in the csSimpleRenderMesh struct and
   * replace it with a transformation that effectively lets you specify the
   * vertices in screen space. The Z components of the mesh vertices should be
   * set to 0 when this flag is specified.
   */
  csSimpleMeshScreenspace = 0x01
};

/**
 * A simple render mesh.
 */
struct csSimpleRenderMesh
{
  /// Type of the geometry to draw.
  csRenderMeshType meshtype;

  /// Number of vertex indices
  uint indexCount;
  /// Vertex indices
  const uint* indices;

  /// Number of vertices
  uint vertexCount;
  /**
   * Vertices. Note: you can omit vertices or texcoords, however this 
   * will likely only give useable results if you provide a shader and 
   * shader var context (and transfer vertices and/or texcoords with SVs.)
   */
  const csVector3* vertices;
  /// (Optional) Texture coordinates.
  const csVector2* texcoords;
  /**
   * (Optional) Colors. 
   * Leaving this 0 has the same effect as having all vertex colors set to
   * white.
   */
  const csVector4* colors;
  /**
   * (Optional) Handle to the texture to select. 
   * Leaving this 0 has the same effect as using a white texture.
   */
  iTextureHandle* texture;

  /// (Optional) Shader to use.
  iShader* shader;
  /// (Optional) Shader variable context.
  iShaderVariableContext* dynDomain;
  /// (Optional) Alpha mode. Defaults to "autodetect".
  csAlphaMode alphaType;
  /// (Optional) Z buffer mode. Defaults to CS_ZBUF_NONE.
  csZBufMode z_buf_mode;
  /// (Optional) Mix mode. Defaults to CS_FX_COPY.
  uint mixmode;
  /**
   * (Optional) Transform to apply to the mesh.
   * \remark This transform is initialized to an identity transform.
   *  This effectively means that geometry is drawn in eye space.
   *  To draw in screen space, supply the \a csSimpleMeshScreenspace
   *  flag to DrawSimpleMesh(). For anything else supply an appropriate
   *  transformation.
   */
  csReversibleTransform object2camera;

  csSimpleRenderMesh () : colors(0), texture (0), shader (0), dynDomain (0), 
    z_buf_mode (CS_ZBUF_NONE), mixmode (CS_FX_COPY)
  {  
    alphaType.autoAlphaMode = true;
    alphaType.autoModeTexture = csInvalidStringID;
  };
};

SCF_VERSION (iGraphics3D, 5, 5, 1);

/**
 * This is the standard 3D graphics interface.
 * All 3D graphics rasterizer servers for Crystal Space should implement this
 * interface, as well as the iGraphics2D interface.  The standard
 * implementation is csGraphics3DSoftware.
 * <p>
 * Main creators of instances implementing this interface:
 *   <ul>
 *   <li>OpenGL Renderer plugin (crystalspace.graphics3d.opengl)
 *   <li>Software Renderer plugin (crystalspace.graphics3d.software)
 *   <li>Null 3D Renderer plugin (crystalspace.graphics3d.null)
 *   </ul>
 * Main ways to get pointers to this interface:
 *   <ul>
 *   <li>CS_QUERY_REGISTRY()
 *   </ul>
 */
struct iGraphics3D : public iBase
{
  /// Open the 3D graphics display.
  virtual bool Open () = 0;
  /// Close the 3D graphics display.
  virtual void Close () = 0;

  /**
   * Retrieve the associated canvas.
   * \remarks This will return a valid canvas only after
   *   csInitializer::OpenApplication() has been invoked (and if the canvas
   *   plugin loaded and initialized successfully); otherwise it will return
   *   null.
   */
  virtual iGraphics2D *GetDriver2D () = 0;

  /**
   * Retrieve the texture manager.
   * \remarks This will return a valid texture manager only after
   *   csInitializer::OpenApplication() has been invoked; otherwise it will
   *   return null.
   */
  virtual iTextureManager *GetTextureManager () = 0;

  /// Change the dimensions of the display.
  virtual void SetDimensions (int width, int height) = 0;
  /// Get drawing buffer width.
  virtual int GetWidth () const = 0;
  /// Get drawing buffer height.
  virtual int GetHeight () const = 0;

  /**
   * Get the current driver's capabilities. Each driver implements their
   * own function.
   */
  virtual const csGraphics3DCaps *GetCaps () const = 0;

  /**
   * Set center of projection for perspective projection.
   * Center is set in screen space coordinates.
   */
  virtual void SetPerspectiveCenter (int x, int y) = 0;

  /// Get perspective center.
  virtual void GetPerspectiveCenter (int& x, int& y) const = 0;

  /**
   * Set aspect ratio for perspective projection.
   */
  virtual void SetPerspectiveAspect (float aspect) = 0;

  /// Get aspect ratio.
  virtual float GetPerspectiveAspect () const = 0;

  /**
   * Set object to camera transformation (currently only used by
   * DrawTriangleMesh and DrawPolygonMesh).
   */
  virtual void SetObjectToCamera (csReversibleTransform* o2c) = 0;

  /**
   * Get object to camera transformation.
   */
  virtual const csReversibleTransform& GetObjectToCamera () = 0;
  
  /**
   * Set the target of rendering. If this is 0 then the target will
   * be the main screen. Otherwise it is a texture. After calling
   * g3d->FinishDraw() the target will automatically be reset to 0 (main
   * screen). Note that on some implementions rendering on a texture
   * will overwrite the screen. So you should only do this BEFORE you
   * start rendering your frame.
   * <p>
   * If 'persistent' is true then the current contents of the texture
   * will be copied on screen before drawing occurs (in the first
   * call to BeginDraw). Otherwise it is assumed that you fully render
   * the texture.
   */
  virtual void SetRenderTarget (iTextureHandle* handle,
  	bool persistent = false) = 0;

  /**
   * Get the current render target (0 for screen).
   */
  virtual iTextureHandle* GetRenderTarget () const = 0;

  /// Start a new frame (see CSDRAW_XXX bit flags)
  virtual bool BeginDraw (int DrawFlags) = 0;

  /// End the frame and do a page swap.
  virtual void FinishDraw () = 0;

  /**
   * Print the image in backbuffer. The area parameter is only a hint to the
   * renderer. Changes outside the rectangle may or may not be printed as
   * well.
   */
  virtual void Print (csRect const* area) = 0;

  /**
   * Draw a pixmap using a rectangle from given texture.
   * The sx,sy(sw,sh) rectangle defines the screen rectangle within
   * which the drawing is performed (clipping rectangle is also taken
   * into account). The tx,ty(tw,th) rectangle defines a subrectangle
   * from texture which should be painted. If the subrectangle exceeds
   * the actual texture size, texture coordinates are wrapped around
   * (e.g. the texture is tiled). The Alpha parameter defines the
   * transparency of the drawing operation, 0 means opaque, 255 means
   * fully transparent.<p>
   * <b>WARNING: Tiling works only with textures that have power-of-two
   * sizes!</b> That is, both width and height should be a power-of-two,
   * although not neccessarily equal.
   */
  virtual void DrawPixmap (iTextureHandle *hTex, int sx, int sy,
    int sw, int sh, int tx, int ty, int tw, int th, uint8 Alpha = 0) = 0;

  /**
   * Draw a line in camera space. Warning! This is a 2D operation
   * and must be called while in BeginDraw(CSDRAW_2DGRAPHICS)!
   */
  virtual void DrawLine (const csVector3& v1, const csVector3& v2,
    float fov, int color) = 0;

  /**
   * Set optional clipper to use. If clipper == null
   * then there is no clipper.
   * Currently only used by DrawTriangleMesh.
   */
  virtual void SetClipper (iClipper2D* clipper, int cliptype) = 0;

  /**
   * Get clipper that was used.
   */
  virtual iClipper2D* GetClipper () = 0;

  /**
   * Return type of clipper.
   */
  virtual int GetClipType () const = 0;

  /**
   * Set near clip plane.
   * Currently only used by DrawTriangleMesh.
   */
  virtual void SetNearPlane (const csPlane3& pl) = 0;

  /**
   * Reset near clip plane (i.e. disable it).
   */
  virtual void ResetNearPlane () = 0;

  /**
   * Get near clip plane.
   */
  virtual const csPlane3& GetNearPlane () const = 0;

  /**
   * Return true if we have a near plane.
   */
  virtual bool HasNearPlane () const = 0;

  /// Set a renderstate value.
  virtual bool SetRenderState (G3D_RENDERSTATEOPTION op, long val) = 0;

  /// Get a renderstate value.
  virtual long GetRenderState (G3D_RENDERSTATEOPTION op) const = 0;

  /**
   * Set a renderer specific option. Returns false if renderer doesn't
   * support that option.
   */
  virtual bool SetOption (const char*, const char*) = 0;

  /**
   * Create a render buffer.
   * \param size Size of the buffer in bytes.
   * \param type Type of buffer; CS_BUF_DYNAMIC, CS_BUF_STATIC or 
   *  CS_BUF_STREAM.
   * \param componentType Components Types; CS_BUFCOMP_FLOAT, CS_BUFCOMP_INT,
   *        etc
   * \param componentCount Number of components per element (e.g. 4 for RGBA)
   * \param copy if true (default) then this buffer will make a copy of the
   *        data. Hardware vertex buffers and interleaved buffers will always
   *        copy data.
   */
  virtual csPtr<iRenderBuffer> CreateRenderBuffer (size_t size, 
    csRenderBufferType type, csRenderBufferComponentType componentType, 
    int componentCount, bool copy = true) = 0;
  /**
   * Create an index buffer.
   * \param size Size of the buffer in bytes.
   * \param type Type of buffer; CS_BUF_DYNAMIC, CS_BUF_STATIC or 
   *  CS_BUF_STREAM.
   * \param componentType Components Types; usually CS_BUFCOMP_UNSIGNED_INT
   * \param rangeStart Minimum index value that is expected to be written to 
   *  the created buffer.
   * \param rangeEnd Maximum index value that is expected to be written to 
   *  the created buffer.
   * \param copy if true (default) then this buffer will make a copy of the
   *        data. Hardware vertex buffers and interleaved buffers will always
   *        copy data.
   */
  virtual csPtr<iRenderBuffer> CreateIndexRenderBuffer (size_t size, 
    csRenderBufferType type, csRenderBufferComponentType componentType,
    size_t rangeStart, size_t rangeEnd, bool copy = true) = 0;

  /**
   * Create an interleaved renderbuffer (You would use this then set stride to
   * determine offset and stride of the interleaved buffer
   * \param size size of the buffer in bytes
   * \param type the type of render buffers you want
   * \param count number of render buffers you want
   * \param buffers an array of render buffer references that can hold
   * at least 'count' render buffers.
   */
  virtual void CreateInterleavedRenderBuffers (size_t size, 
    csRenderBufferType type, int count, csRef<iRenderBuffer>* buffers) = 0;

  /**
   * Activate the buffers in the default buffer holder.
   */
  virtual bool ActivateBuffers (csRenderBufferHolder* holder, 
    csRenderBufferName mapping[CS_VATTRIB_SPECIFIC_LAST+1]) = 0;

  /**
   * Activate all given buffers.
   */
  virtual bool ActivateBuffers (csVertexAttrib *attribs,
    iRenderBuffer **buffers, unsigned int count) = 0;

  /**
   * Deactivate all given buffers.
   * If attribs is 0, all buffers are deactivated;
   */
   virtual void DeactivateBuffers (csVertexAttrib *attribs, unsigned int count) = 0;

  /**
   * Activate or deactivate all given buffers depending on the value of
   * 'buffers' for that index.
   */
/*  virtual void SetBufferState (csVertexAttrib* attribs,
  	iRenderBuffer** buffers, int count) = 0;*/

  /**
   * Activate or deactivate all given textures depending on the value
   * of 'textures' for that unit (i.e. deactivate if 0).
   */
  virtual void SetTextureState (int* units, iTextureHandle** textures,
  	int count) = 0;

  /// Drawroutine. Only way to draw stuff
  virtual void DrawMesh (const csCoreRenderMesh* mymesh,
    const csRenderMeshModes& modes,
    const csArray< csArray<csShaderVariable*> > &stacks) = 0;

  /// Set the masking of color and/or alpha values to framebuffer
  virtual void SetWriteMask (bool red, bool green, bool blue, bool alpha) = 0;

  /// Get the masking of color and/or alpha values to framebuffer
  virtual void GetWriteMask (bool &red, bool &green, bool &blue,
	bool &alpha) const = 0;

  /// Set the z buffer write/test mode
  virtual void SetZMode (csZBufMode mode) = 0;

  /// Enables offsetting of Z values
  virtual void EnableZOffset () = 0;

  /// Disables offsetting of Z values
  virtual void DisableZOffset () = 0;

  /// Controls shadow drawing
  virtual void SetShadowState (int state) = 0;

   /// Get Z-buffer value at given X,Y position
  virtual float GetZBuffValue (int x, int y) = 0;

  /**
   * Enter a new portal. If 'floating' is true then this routine will restrict
   * all further drawing to the given 2D area and it will also respect
   * the current contents of the Z-buffer so that geometry will only
   * render where the Z-buffer allows it (even if zfill or znone is used).
   * Remember to close a portal later using ClosePortal().
   * Basically this represents a stacked layer of portals. Each subsequent
   * portal must be fully contained in the previous ones.
   */
  virtual void OpenPortal (size_t numVertices, const csVector2* vertices,
    const csPlane3& normal, bool floating) = 0;

  /**
   * Close a portal previously opened with OpenPortal().
   * If 'zfill_portal' then the portal area will be zfilled.
   */
  virtual void ClosePortal (bool zfill_portal) = 0;

  /// Create a halo of the specified color and return a handle.
  virtual iHalo *CreateHalo (float iR, float iG, float iB,
    unsigned char *iAlpha, int iWidth, int iHeight) = 0;

  /**
   * Remove some polygon from the cache.
   * You have to call this function before deleting a polygon
   * (csPolygon3D destructor will do that).
   */
  virtual void RemoveFromCache (iRendererLightmap* rlm) = 0;

  //=========================================================================
  // Here ends the zone of unimplemented methods.
  //=========================================================================

  /**
   * Check if renderer can handle a lightmap.
   * Returns true if it can, false if not.
   */
  virtual bool IsLightmapOK (int lmw, int lmh, 
    int lightCellSize) = 0;
    
  virtual csPtr<iPolygonRenderer> CreatePolygonRenderer () = 0;

  /*
    @@@ Needed for SW poly drawing ATM.
   */
  virtual void SetWorldToCamera (csReversibleTransform* w2c) = 0;

  /**
   * Draw a csSimpleRenderMesh on the screen.
   * Simple render meshes are intended for cases where setting up
   * a render mesh and juggling with render buffers would be too much
   * effort - e.g. when you want to draw a single polygon on the screen.
   * <p>
   * DrawSimpleMesh () hides the complexity of csRenderMesh, it cares
   * about setting up render buffers, activating the texture etc.
   * Note that you can still provide shaders and shader variables, but those
   * are optional.
   * \param mesh The mesh to draw.
   * \param flags Drawing flags, a combination of csSimpleMeshFlags values.
   * \remark This operation can also be called from 2D mode. In this case,
   *  the csSimpleMeshScreenspace flag should be specified. If this is not the
   *  case, you are responsible for the mess that is likely created.
   */
  virtual void DrawSimpleMesh (const csSimpleRenderMesh& mesh,
    uint flags = 0) = 0;

  /// Get the z buffer write/test mode
  virtual csZBufMode GetZMode () = 0;
};

/** @} */

#endif // __CS_IVIDEO_GRAPH3D_H__

