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

#ifndef __CS_GL_RENDER3D_H__
#define __CS_GL_RENDER3D_H__


#include "csgeom/csrect.h"
#include "csgeom/poly3d.h"
#include "csgeom/transfrm.h"
#include "csgeom/vector2.h"
#include "csgeom/vector3.h"

#include "csutil/cfgacc.h"
#include "csutil/cscolor.h"
#include "csutil/csstring.h"
#include "csutil/scf.h"
#include "csutil/scfstrset.h"
#include "iutil/strset.h"

#include "iutil/comp.h"
#include "iutil/event.h"
#include "iutil/eventh.h"
#include "ivideo/graph2d.h"
#include "ivideo/render3d.h"

#include "ivideo/shader/shader.h"
#include "video/canvas/openglcommon/glstates.h"

#include "ivaria/bugplug.h"

#include "video/canvas/openglcommon/glextmanager.h"
#include "gl_sysbufmgr.h"
#include "gl_varbufmgr.h"
#include "gl_vaobufmgr.h"
#include "gl_helper.h"

class csGLTextureCache;
class csGLTextureHandle;
class csGLTextureManager;

struct iClipper2D;
struct iObjectRegistry;
struct iTextureManager;
struct iRenderBufferManager;
struct iLightingManager;

struct iEvent;


#define CS_GL_CLIP_AUTO           'a' // Used for auto-detection.
#define CS_GL_CLIP_NONE           'n'
#define CS_GL_CLIP_ZBUF           'z'
#define CS_GL_CLIP_STENCIL        's'
#define CS_GL_CLIP_PLANES         'p'
#define CS_GL_CLIP_SOFTWARE       '0'
#define CS_GL_CLIP_LAZY_NONE      'N'
#define CS_GL_CLIP_LAZY_ZBUF      'Z'
#define CS_GL_CLIP_LAZY_STENCIL   'S'
#define CS_GL_CLIP_LAZY_PLANES    'P'

class csGLRender3D : public iRender3D
{
private:
  //friend declarations
  friend class csVARRenderBufferManager;
  friend class csSysRenderBufferManager;
  friend class csVARRenderBuffer;
  friend class csVaoRenderBuffer;
  friend class csGLTextureHandle;
  friend class csGLTextureCache;
  friend class csGLTextureManager;
  friend class eiShaderRenderInterface;
  friend class csShaderGLMTEX;

  csRef<iObjectRegistry> object_reg;
  csRef<iGraphics2D> G2D;
  csRef<iRenderBufferManager> buffermgr;
  csRef<iLightingManager> lightmgr;
  csRef<iShaderManager> shadermgr;

  csRef<iBugPlug> bugplug;

  static csGLStateCache* statecache;

  csGLExtensionManager *ext;
  csGLVertexArrayHelper varr;
  csGLTextureCache *txtcache;
  //csGLTextureManager *txtmgr;
  csRef<csGLTextureManager> txtmgr;

  bool color_red_enabled, color_green_enabled, color_blue_enabled, 
    alpha_enabled;
  int current_drawflags;
  int current_shadow_state;
  csZBufMode current_zmode;

  int asp_center_x, asp_center_y;
  float aspect;
  float fov;
  int viewwidth, viewheight;
  csPoly3D frustum;
  bool frustum_valid;

  csReversibleTransform object2camera;

  csRender3dCaps rendercaps;

  csRef<iStringSet> strings;

  csStringID string_vertices;
  csStringID string_texture_coordinates;
  csStringID string_normals;
  csStringID string_colors;
  csStringID string_indices;

  csConfigAccess config;

  float textureLodBias;

  /// Light standard variables
  csRef<iShaderVariable> shvar_light_0_pos;
  csRef<iShaderVariable> shvar_light_0_diffuse;
  csRef<iShaderVariable> shvar_light_0_specular;
  csRef<iShaderVariable> shvar_light_0_attenuation;


  bool do_near_plane;
  csPlane3 near_plane;

  /// Prefered clipping modes to use for optional portals.
  char clip_optional[3];
  /// Prefered clipping modes to use for required portals.
  char clip_required[3];
  /// Prefered clipping modes to use for outer portal.

  int stencilclipnum;
  bool stencil_enabled;
  bool clip_planes_enabled;
  char clip_outer[3];
  csRef<iClipper2D> clipper;
  int cliptype;
  bool stencil_initialized;
  bool clipplane_initialized;

  /// Current render target.
  csRef<iTextureHandle> render_target;
  /// If true then the current render target has been put on screen.
  bool rt_onscreen;
  /// If true then we have set the old clip rect.
  bool rt_cliprectset;
  /// Old clip rect to restore after rendering on a proc texture.
  int rt_old_minx, rt_old_miny, rt_old_maxx, rt_old_maxy;


  ////////////////////////////////////////////////////////////////////
  //                         Private helpers
  ////////////////////////////////////////////////////////////////////
	
  void Report (int severity, const char* msg, ...);

  int GetMaxTextureSize ();

  void SetGlOrtho (bool inverted);

  void SetZMode (csZBufMode mode);

  float SetMixMode (uint mode, float m_alpha, bool txt_alpha);
  
  csZBufMode GetZModePass2 (csZBufMode mode);

  void SetMirrorMode (bool mirror);

  void CalculateFrustum ();

  void SetupStencil ();

  int SetupClipPlanes (bool add_clipper, 
    bool add_near_clip, 
    bool add_z_clip);

  void SetupClipper (int clip_portal, int clip_plane, int clip_z_plane);

  void ApplyObjectToCamera ();

  void CacheTexture(iTextureHandle* itex);


  iRenderBuffer* vertattrib[16]; // @@@ Hardcoded max number of attributes
  bool vertattribenabled[16]; // @@@ Hardcoded max number of attributes
  iTextureHandle* texunit[16]; // @@@ Hardcoded max number of units
  bool texunitenabled[16]; // @@@ Hardcoded max number of units

  iShaderPass* lastUsedShaderpass;
public:
  SCF_DECLARE_IBASE;

  csGLRender3D (iBase *parent);
  virtual ~csGLRender3D ();

  ////////////////////////////////////////////////////////////////////
  //                            iRender3d
  ////////////////////////////////////////////////////////////////////

  /// Open 3d renderer.
  bool Open ();

  /// Close renderer and release all resources used
  void Close ();

  /// Get a pointer to our 2d canvas driver. NOTE: It's not increfed,
  /// and therefore it shouldn't be decref-ed by caller.
  iGraphics2D* GetDriver2D () 
    { return G2D; }

  /// Get a pointer to our texture manager
  iTextureManager* GetTextureManager () 
    { return (iTextureManager*)((csGLTextureManager*)txtmgr); }

  /**
   * Get a pointer to the VB-manager
   * Always use the manager referenced here to get VBs
   */
  iRenderBufferManager* GetBufferManager () 
    { return buffermgr; }

  /// Get a pointer to lighting manager
  iLightingManager* GetLightingManager () 
    { return lightmgr; }

  /// Activate a vertex buffer
  virtual bool ActivateBuffer (csVertexAttrib attrib, iRenderBuffer* buffer);

  /// Deactivate a vertex buffer
  virtual void DeactivateBuffer (csVertexAttrib attrib);

  /// Activate a texture
  virtual bool ActivateTexture (iTextureHandle *txthandle, int unit = 0);

  /// Activate a texture (Should probably handled some better way)
  virtual bool ActivateTexture (iMaterialHandle *mathandle, int layer, int unit = 0);

  /// Deactivate a texture
  virtual void DeactivateTexture (int unit = 0);

  /// Set dimensions of window
  void SetDimensions (int width, int height)
    { viewwidth = width; viewheight = height; }
  
  /// Get width of window
  int GetWidth () 
    { return viewwidth; }
  
  /// Get height of window
  int GetHeight () 
    { return viewheight; }

  /// Capabilities of the driver
  csRender3dCaps* GetCaps() 
    { return &rendercaps; }

  /// Set center of projection.
  virtual void SetPerspectiveCenter (int x, int y)
  {
    asp_center_x = x;
    asp_center_y = y;
    frustum_valid = false;
  }
  
  /// Get center of projection.
  virtual void GetPerspectiveCenter (int& x, int& y)
  {
    x = asp_center_x;
    y = asp_center_y;
  }
  
  /// Set perspective aspect.
  virtual void SetPerspectiveAspect (float aspect)
  {
    csGLRender3D::aspect = aspect;
    frustum_valid = false;
  }

  /// Get perspective aspect.
  virtual float GetPerspectiveAspect ()
  {
    return aspect;
  }

  /// Set world to view transform
  void SetObjectToCamera (csReversibleTransform* wvmatrix);
  csReversibleTransform* GetWVMatrix ();

  /// Set the current render target (0 for screen).
  virtual void SetRenderTarget (iTextureHandle* handle,
	  bool persistent = false)
  {
    render_target = handle;
    rt_onscreen = !persistent;
    rt_cliprectset = false;
  }

  /// Get the current render target (0 for screen).
  virtual iTextureHandle* GetRenderTarget ()
  {
    return render_target;
  }

  /// Begin drawing in the renderer
  bool BeginDraw (int drawflags);

  /// Indicate that drawing is finished
  void FinishDraw ();

  /// Do backbuffer printing
  void Print (csRect* area);

  /// Drawroutine. Only way to draw stuff
  void DrawMesh (csRenderMesh* mymesh);

  /// Set the masking of color and/or alpha values to framebuffer
  virtual void SetWriteMask (bool red, bool green, bool blue, bool alpha)
  { 
    color_red_enabled = red;
    color_green_enabled = green;
    color_blue_enabled = blue;
    alpha_enabled = alpha;
    glColorMask (red, green, blue, alpha); 
  }

  virtual void GetWriteMask (bool &red, bool &green, bool &blue, bool &alpha)
  {
    red = color_red_enabled;
    green = color_green_enabled;
    blue = color_blue_enabled;
    alpha = alpha_enabled;
  }


  /// Enables offsetting of Z values
  virtual void EnableZOffset ()
    { glPolygonOffset (-0.05, -2); 
      statecache->Enable_GL_POLYGON_OFFSET_FILL (); }

  /// Disables offsetting of Z values
  virtual void DisableZOffset ()
    { statecache->Disable_GL_POLYGON_OFFSET_FILL (); }

  /// Controls shadow drawing
  virtual void SetShadowState (int state);

  /// Draw a line
  virtual void DrawLine(const csVector3 & v1,
    const csVector3 & v2, float fov, int color);

  /**
   * Set optional clipper to use. If clipper == null
   * then there is no clipper.
   * Currently only used by DrawTriangleMesh.
   */
  void SetClipper (iClipper2D* clipper, int cliptype);

  /// Get clipper that was used.
  iClipper2D* GetClipper ()
    { return clipper; }

  /// Return type of clipper.
  int GetClipType ()
    { return cliptype; }

  /// Set near clip plane.
  virtual void SetNearPlane (const csPlane3& pl)
  {
    do_near_plane = true;
    near_plane = pl;
  }

  /// Reset near clip plane (i.e. disable it).
  virtual void ResetNearPlane () 
    { do_near_plane = false; }

  /// Get near clip plane.
  virtual const csPlane3& GetNearPlane () 
    { return near_plane; }

  /// Return true if we have near plane.
  virtual bool HasNearPlane () 
    { return do_near_plane; }

  /// Get maximum number of simultaneous vertex lights supported
  virtual int GetMaxLights ()
    { return GL_MAX_LIGHTS; }

  /// Sets a parameter for light i
  virtual void SetLightParameter (int i, int param, csVector3 value);

  /// Enables light i
  virtual void EnableLight (int i)
    { glEnable (GL_LIGHT0+i); }

  /// Disables light i
  virtual void DisableLight (int i)
    { glDisable (GL_LIGHT0+i); }

  /// Enable vertex lighting
  virtual void EnablePVL ()
    { statecache->Enable_GL_LIGHTING (); }
    
  /// Disable vertex lighting
  virtual void DisablePVL ()
    { statecache->Disable_GL_LIGHTING (); }

  virtual bool SetRenderState (R3D_RENDERSTATEOPTION op, long val);
  virtual long GetRenderState (R3D_RENDERSTATEOPTION op);

  ////////////////////////////////////////////////////////////////////
  //                         iShaderRenderInterface
  ////////////////////////////////////////////////////////////////////

  class eiShaderRenderInterface : public iShaderRenderInterface
  {
  private:
    csRef<iObjectRegistry> object_reg;
    csBasicVector pluginlist;
  public:
    SCF_DECLARE_EMBEDDED_IBASE(csGLRender3D);
    eiShaderRenderInterface();
    virtual ~eiShaderRenderInterface();

    virtual void Initialize( iObjectRegistry *reg);


    virtual void* GetPrivateObject(const char* name);
  } scfiShaderRenderInterface;
  friend class eiShaderRenderInterface;

  ////////////////////////////////////////////////////////////////////
  //                          iComponent
  ////////////////////////////////////////////////////////////////////

  bool Initialize (iObjectRegistry* reg);

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csGLRender3D);
    virtual bool Initialize (iObjectRegistry* reg)
      { return scfParent->Initialize (reg); }
  } scfiComponent;
	
  ////////////////////////////////////////////////////////////////////
  //                         iEventHandler
  ////////////////////////////////////////////////////////////////////
  
  bool HandleEvent (iEvent& Event);

  struct EventHandler : public iEventHandler
  {
  private:
    csGLRender3D* parent;
  public:
    EventHandler (csGLRender3D* parent)
    {
      SCF_CONSTRUCT_IBASE (0);
      EventHandler::parent = parent;
    }
    
    SCF_DECLARE_IBASE;
    virtual bool HandleEvent (iEvent& ev) 
      { return parent->HandleEvent (ev); }
  } * scfiEventHandler;



};

#endif // __CS_GL_RENDER3D_H__

