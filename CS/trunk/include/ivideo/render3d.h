/*
    Copyright (C) 2002 by M�rten Svanfeldt
                          Anders Stenberg

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

#ifndef __CS_IVIDEO_RENDER3D_H__
#define __CS_IVIDEO_RENDER3D_H__


#include "csutil/scf.h"
/** \file
 * New 3D graphics interface
 */

/**
 * \addtogroup gfx3d
 * @{ */

class csRect;
class csReversibleTransform;
class csStringSet;
class csPlane3;
class csVector3;

class csRenderMesh;

struct iClipper2D;
struct iGraphics2D;
struct iMaterialHandle;
struct iTextureManager;
struct iTextureHandle;
struct iRenderBuffer;
struct iRenderBufferManager;
struct iLightingManager;


/**\name iRender3D::BeginDraw() flags
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
/**
 * Clipping is not needed for the current clipper but it might
 * be needed for the toplevel clipper. (setting for clip_portal,
 * will never be used for clip_plane or clip_z_plane).
 */
#define CS_CLIP_TOPLEVEL 2
/** @} */

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
};

/// Vertex attributes
enum csVertexAttrib
{
  CS_VATTRIB_0 = 0,
  CS_VATTRIB_1 = 1,
  CS_VATTRIB_2 = 2,
  CS_VATTRIB_3 = 3,
  CS_VATTRIB_4 = 4,
  CS_VATTRIB_5 = 5,
  CS_VATTRIB_6 = 6,
  CS_VATTRIB_7 = 7,
  CS_VATTRIB_8 = 8,
  CS_VATTRIB_9 = 9,
  CS_VATTRIB_10 = 10,
  CS_VATTRIB_11 = 11,
  CS_VATTRIB_12 = 12,
  CS_VATTRIB_13 = 13,
  CS_VATTRIB_14 = 14,
  CS_VATTRIB_15 = 15,
  CS_VATTRIB_POSITION = 0,
  CS_VATTRIB_WEIGHT = 1,
  CS_VATTRIB_NORMAL = 2,
  CS_VATTRIB_COLOR = 3,
  CS_VATTRIB_PRIMARY_COLOR = 3,
  CS_VATTRIB_SECONDARY_COLOR = 4,
  CS_VATTRIB_FOGCOORD = 5,
  CS_VATTRIB_TEXCOORD = 8,
  CS_VATTRIB_TEXCOORD0 = 8,
  CS_VATTRIB_TEXCOORD1 = 9,
  CS_VATTRIB_TEXCOORD2 = 10,
  CS_VATTRIB_TEXCOORD3 = 11
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
/// color 0 is transparent
#define CS_FX_KEYCOLOR     0x08000000 
/// Gouraud shading
#define CS_FX_GOURAUD      0x04000000 
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

/**\name Light parameters
 * @{ */

/**
 * Position of the light
 */
#define CS_LIGHTPARAM_POSITION 0

/**
 * Diffuse color of the light
 */
#define CS_LIGHTPARAM_DIFFUSE 1

/**
 * Specular color of the light
 */
#define CS_LIGHTPARAM_SPECULAR 2

/**
 * Attenuation of the light
 */
#define CS_LIGHTPARAM_ATTENUATION 3

/** @} */

/**\name Shadow states
 * @{ */


/**
 * Clear stencil
 */
#define CS_SHADOW_VOLUME_BEGIN 1

/**
 * Setup for pass 1
 */
#define CS_SHADOW_VOLUME_PASS1 2

/**
 * Setup for pass 2
 */
#define CS_SHADOW_VOLUME_PASS2 3

/**
 * Setup for carmack's reverse pass 1
 */
#define CS_SHADOW_VOLUME_FAIL1 4

/**
 * Setup for carmack's reverse pass 2
 */
#define CS_SHADOW_VOLUME_FAIL2 5

/**
 * Setup for shadow masking
 */
#define CS_SHADOW_VOLUME_USE 6

/**
 * Restore states
 */
#define CS_SHADOW_VOLUME_FINISH 7

/** @} */



/// Graphics3D render state options
enum R3D_RENDERSTATEOPTION
{
};

/**
 * A triangle. Note that this structure is only valid if used
 * in combination with a vertex or edge table. 'a', 'b', and 'c' are then
 * indices in that table (either vertices or edges).
 */
/*struct csTriangle
{
  int a, b, c;

  /// Empty default constructor
  csTriangle() {}

  /// Convenience constructor, builds a triangle with initializers
  csTriangle(int _a, int _b, int _c):a(_a), b(_b), c(_c) {}
};
*/

class csRender3dCaps
{
};

SCF_VERSION (iRender3D, 0, 0, 1);


/**
 * New 3D Interface. Work in progress!
 */

struct iRender3D : public iBase
{
  /// Open 3d renderer.
  virtual bool Open () = 0;

  /// Close renderer and release all resources used
  virtual void Close () = 0;

  /**
   * Get a pointer to our 2d canvas driver. NOTE: It's not increfed,
   * and therefore it shouldn't be decref-ed by caller.
   */
  virtual iGraphics2D* GetDriver2D () = 0;

  /// Get a pointer to our texture manager
  virtual iTextureManager* GetTextureManager () = 0;

  /**
   * Get a pointer to the VB-manager
   * Always use the manager referenced here to get VBs
   */
  virtual iRenderBufferManager* GetBufferManager () = 0;

  /// Get a pointer to lighting manager
  virtual iLightingManager* GetLightingManager () = 0;

  /// Activate a vertex buffer
  virtual bool ActivateBuffer (csVertexAttrib attrib, iRenderBuffer* buffer) = 0;

  /// Deactivate a vertex buffer
  virtual void DeactivateBuffer (csVertexAttrib attrib) = 0;

  /// Activate a texture
  virtual bool ActivateTexture (iTextureHandle *txthandle, int unit = 0) = 0;

  /// Activate a texture (Should probably handled some better way)
  virtual bool ActivateTexture (iMaterialHandle *matwrapper, int layer, int unit = 0) = 0;

  /// Deactivate a texture
  virtual void DeactivateTexture (int unit = 0) = 0;

  /// Set dimensions of window
  virtual void SetDimensions (int width, int height) = 0;
  /// Get width of window
  virtual int GetWidth () = 0;
  /// Get height of window
  virtual int GetHeight () = 0;

  /// Capabilities of the driver
  virtual csRender3dCaps* GetCaps () = 0;

  /// Set center of projection.
  virtual void SetPerspectiveCenter (int x, int y) = 0;
  
  /// Get center of projection.
  virtual void GetPerspectiveCenter (int& x, int& y) = 0;
  
  /// Set perspective aspect.
  virtual void SetPerspectiveAspect (float aspect) = 0;

  /// Get perspective aspect.
  virtual float GetPerspectiveAspect () = 0;

  /// Set world to view transform
  virtual void SetObjectToCamera (csReversibleTransform* wvmatrix) = 0;
  virtual csReversibleTransform* GetWVMatrix () = 0;

  /**
   * Set the target of rendering. If this is NULL then the target will
   * be the main screen. Otherwise it is a texture. After calling
   * g3d->FinishDraw() the target will automatically be reset to NULL (main
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

  /// Get the current render target (NULL for screen).
  virtual iTextureHandle* GetRenderTarget () = 0;

  /// Begin drawing in the renderer
  virtual bool BeginDraw (int drawflags) = 0;

  /// Indicate that drawing is finished
  virtual void FinishDraw () = 0;

  /// Do backbuffer printing
  virtual void Print (csRect* area) = 0;

  /// Drawroutine. Only way to draw stuff
  virtual void DrawMesh (csRenderMesh* mymesh) = 0;

  /// Set the masking of color and/or alpha values to framebuffer
  virtual void SetWriteMask (bool red, bool green, bool blue, bool alpha) = 0;

  /// Get the masking of color and/or alpha values to framebuffer
  virtual void GetWriteMask (bool &red, bool &green, bool &blue, bool &alpha) = 0;

  /// Enables offsetting of Z values
  virtual void EnableZOffset () = 0;

  /// Disables offsetting of Z values
  virtual void DisableZOffset () = 0;

  /// Controls shadow drawing
  virtual void SetShadowState (int state) = 0;

  /// Draw a line
  virtual void DrawLine(const csVector3 & v1,
  const csVector3 & v2, float fov, int color) = 0;

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
  virtual int GetClipType () = 0;

  /// Set near clip plane.
  virtual void SetNearPlane (const csPlane3& pl) = 0;

  /// Reset near clip plane (i.e. disable it).
  virtual void ResetNearPlane () = 0;

  /// Get near clip plane.
  virtual const csPlane3& GetNearPlane () = 0;

  /// Return true if we have near plane.
  virtual bool HasNearPlane () = 0;

  /// Get maximum number of simultaneous HW lights supported
  virtual int GetMaxLights () = 0;

  /// Sets a parameter for light i
  virtual void SetLightParameter (int i, int param, csVector3 value) = 0;

  /// Enables light i
  virtual void EnableLight (int i) = 0;

  /// Disables light i
  virtual void DisableLight (int i) = 0;

  /// Enable vertex lighting
  virtual void EnablePVL () = 0;

  /// Disable vertex lighting
  virtual void DisablePVL () = 0;

  /// Get a stringhash to be used by our streamsources etc.
  virtual csStringSet* GetStringContainer () = 0;

  /// Set a renderstate value.
  virtual bool SetRenderState (R3D_RENDERSTATEOPTION op, long val) = 0;

  /// Get a renderstate value.
  virtual long GetRenderState (R3D_RENDERSTATEOPTION op) = 0;
};

/** @} */

#endif // __CS_IVIDEO_RENDER3D_H__

