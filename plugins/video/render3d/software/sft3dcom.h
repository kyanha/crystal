/*
    Copyright (C) 1998-2000 by Jorrit Tyberghein
    Copyright (C) 2003 by Anders Stenberg

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

#ifndef __CS_SFTR3DCOM_H__
#define __CS_SFTR3DCOM_H__

#include "csutil/scf.h"
#include "csgeom/transfrm.h"
#include "soft_txt.h"
#include "csutil/cfgacc.h"
#include "scan.h"
#include "ivideo/halo.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "iutil/strset.h"
#include "ivideo/graph2d.h"
#include "ivideo/render3d.h"
#include "ivideo/shader/shader.h"

class csSoftwareTextureCache;
class csPolArrayVertexBufferManager;
struct iConfigFile;

// Maximum size of a single lightmap, in pixels
#define MAX_LIGHTMAP_SIZE	1500000

/// This structure is used to hold references to all current fog objects.
/*struct FogBuffer
{
  FogBuffer *next, *prev;
  CS_ID id;
  float density;
  float red, green, blue;
};*/

/// Extra information for vertex fogging.
class G3DFogInfo
{
public:
  /// Color.
  float r, g, b;
  /**
  * Intensity (== density * thickness).
  * The second intensity value is always 0 and is put there
  * to make it easier for 3D implementations to just use the
  * two values below as a coordinate in a texture of 64*1.
  */
  float intensity;
  float intensity2;
};

/// Information about a texture plane.
class G3DTexturePlane
{
public:
  /// Transformation from camera space to texture space.
  csMatrix3* m_cam2tex;
  /// Transformation from camera space to texture space.
  csVector3* v_cam2tex;
};

/// Structure containing all info needed by DrawFogPolygon (DFP)
struct G3DPolygonDFP
{
  /// Current number of vertices.
  int num;
  /// Vertices that form the polygon.
  csVector2 vertices[100];

  /// The plane equation in camera space of this polygon.
  csPlane3 normal;
};

/// Structure containing all info needed by DrawPolygon (DP)
struct G3DPolygonDPF : public G3DPolygonDFP
{
  /// Extra optional fog information.
  G3DFogInfo fog_info[100];
  /// Use fog info?
  bool use_fog;

  /// The material handle as returned by iTextureManager.
  iMaterialHandle* mat_handle;

  /// Transformation matrices for the texture. @@@ BAD NAME
  G3DTexturePlane plane;

  /// Handle to lighted textures (texture + lightmap)
  iPolygonTexture* poly_texture;

  /// Draw fullbright?
  bool do_fullbright;

  /// Mixmode to use. If CS_FX_COPY then no mixmode is used.
  uint mixmode;

  /// Z value (in camera space) of vertex[0].
  float z_value;
};

/// Structure containing all info needed by DrawPolygonFX (DPFX)
struct G3DPolygonDPFX
{
  /// Current number of vertices.
  int num;
  /// Vertices that form the polygon.
  csVector2 vertices[100];
  /// 1/z for every vertex.
  float z[100];
  /// Texels per vertex.
  csVector2 texels[100];
  /// Lighting info per vertex.
  csColor colors[100];

  /// Extra optional fog information.
  G3DFogInfo fog_info[100];
  /// Use fog info?
  bool use_fog;

  /// The material handle as returned by iTextureManager.
  iMaterialHandle *mat_handle;
  /// Mixmode to use. If CS_FX_COPY then no mixmode is used.
  uint mixmode;

  /// Use this color for drawing (if txt_handle == 0) instead of a material.
  uint8 flat_color_r;
  uint8 flat_color_g;
  uint8 flat_color_b;

  // A dummy constructor to appease NextStep compiler which otherwise
  // complains that it is unable to create this object.  This happens when
  // a subcomponent such as csVector2 has a constructor.
  G3DPolygonDPFX() {}
};

/**
* Structure containing all info needed by DrawTriangeMesh.
* This function is capable of:<br>
* <ul>
* <li>Object2camera transformation and perspective.
* <li>Linear interpolation between two sets of vertices.
* <li>Clipping.
* <li>Whatever else DrawPolygonFX can do.
* </ul>
* To disable the use of one of the components, set it to 0.
*/
struct G3DTriangleMesh
{
  enum
  {
    /// Maximum number of vertex pool, used for vertex weighting/morphing.
    MAX_VERTEXPOOL = 2
  };

  /// Number of vertex sets, if > 1, morphing will be applied.
  int num_vertices_pool;

  /// Number of triangles.
  int num_triangles;
  /// Pointer to array of triangles.
  csTriangle* triangles;

  /// Clip to portal? One of CS_CLIP_???.
  int clip_portal;
  /// Clip to near plane? One of CS_CLIP_???.
  int clip_plane;
  /// Clip to z plane? One of CS_CLIP_???.
  int clip_z_plane;

  /// Use precalculated vertex color?
  bool use_vertex_color;

  /// Apply fogging?
  bool do_fog;
  /// Consider triangle vertices in anti-clockwise order if true.
  bool do_mirror;
  /// If morphing is applied then morph texels too if true.
  bool do_morph_texels;
  /// If morphing is applied then morph vertex colors too if true.
  bool do_morph_colors;

  /// Types of vertices supplied.
  enum VertexMode
  {
    /// Must apply transformation and perspective.
    VM_WORLDSPACE,
    /// Must apply perspective.
    VM_VIEWSPACE
  };

  /// Type of vertices supplied.
  VertexMode vertex_mode;

  /// DrawPolygonFX flag.
  uint mixmode;
  float morph_factor;
  /**
  * Vertex buffers. Note that all vertex buffers used here MUST
  * have the same number of vertices.
  */
  iVertexBuffer* buffers[MAX_VERTEXPOOL];
  iMaterialHandle* mat_handle;
  /// Information for fogging the vertices.
  G3DFogInfo* vertex_fog;

  // TODO : store information required for lighting calculation
};



/**
 * The basic software renderer class.
 * This class is the parent for both "normal" software renderer
 * as well as for procedural texture class.
 */
class csSoftwareRender3DCommon : public iRender3D
{
protected:
  //friend class csSoftHalo;

  /// Driver this driver is sharing info with (if any)
  csSoftwareRender3DCommon *partner;

  /// if this is a procedural texture manager
  bool is_for_procedural_textures;

  /// Current render target.
  csRef<iTextureHandle> render_target;
  /// If true then the current render target has been put on screen.
  bool rt_onscreen;
  /// If true then we have set the old clip rect.
  bool rt_cliprectset;
  /// Old clip rect to restore after rendering on a proc texture.
  int rt_old_minx, rt_old_miny, rt_old_maxx, rt_old_maxy;

  /// Z buffer.
  uint32* z_buffer;
  /// Size of Z buffer.
  long z_buf_size;

  /**
   * Addresses of all lines for this frame. This table is used to avoid
   * calls to GetPixelAt in the main renderer loop. It is filled every frame
   * (by BeginDraw()) because double buffering or other stuff can change
   * the addresses.
   */
  uint8** line_table;

  /// If true then really rendering with a smaller size inside a larger window.
  bool do_smaller_rendering;

  /// Buffer for smaller rendering.
  unsigned char* smaller_buffer;

  /**
   * Number of bytes for every pixel (expressed in shifts). Also used in
   * combination with line_table to calculate the in-screen offset.
   */
  int pixel_shift;

  /// For debugging: the maximum number of polygons to draw in a frame.
  long dbg_max_polygons_to_draw;
  /// For debugging: the current polygon number.
  long dbg_current_polygon;

  /// Z Buffer mode to use while rendering next polygon.
  csZBufMode z_buf_mode;

  /// Values to check if we have to reinit StartPolygonFX.
  bool dpfx_valid;
  bool dpfx_use_fog;
  iMaterialHandle* dpfx_mat_handle;
  uint dpfx_mixmode;
  csZBufMode dpfx_z_buf_mode;

  /// Alpha mask used for 16-bit mode.
  uint16 alpha_mask;

  /// Fog buffers.
  //FogBuffer* fog_buffers;

  /// Width of display.
  int display_width;
  /// Height of display.
  int display_height;

  /// pseudo width of display.
  int width;
  /// pseudo height of display.
  int height;
  /// Opt: width divided by 2.
  int width2;
  /// Opt: height divided by 2.
  int height2;
  /// The pixel format of display
  csPixelFormat pfmt;
#if defined (DO_MMX)
  /// True if CPU has MMX instructions.
  bool cpu_mmx;
  /// True if 3D rendering should use MMX if available.
  bool do_mmx;
#endif
  /// Current transformation from world to camera.
  csReversibleTransform o2c;

  /// Current 2D clipper.
  iClipper2D* clipper;
  /// Clipper type.
  int cliptype;
  /// Current near plane.
  csPlane3 near_plane;
  /// Is near plane used?
  bool do_near_plane;

  /// Current aspect ratio for perspective correction.
  float aspect;
  /// Current inverse aspect ratio for perspective correction.
  float inv_aspect;

  /// Mipmap selection coefficient (normal == 1.0)
  float mipmap_coef;

  /// Do we want mipmaps?
  int rstate_mipmap;

  /// DrawFlags on last BeginDraw ()
  int DrawMode;

  /// scan_xxx routines
  csDrawScanline* ScanProc [0x28];
  /// scan_pi_xxx routines
  csDrawPIScanline* ScanProcPI [0x2c];
  /// scan_pi_xxx routines
  csDrawPIScanlineGouraud* ScanProcPIG [0x28];

  /// The routine for getting the address of needed scanline_xxx_alpha
  csDrawScanline* (*ScanProc_Alpha) (csSoftwareRender3DCommon*, int alpha, bool keycolor, bool alphamap);

  /// ScanProc_Alpha for 8 bpp modes
  static csDrawScanline* ScanProc_8_Alpha (csSoftwareRender3DCommon*,
    int alpha, bool keycolor, bool alphamap);
  /// ScanProc_Alpha for 16 bpp modes
  static csDrawScanline* ScanProc_16_Alpha (csSoftwareRender3DCommon*,
    int alpha, bool keycolor, bool alphamap);
  /// ScanProc_Alpha for 32 bpp modes
  static csDrawScanline* ScanProc_32_Alpha (csSoftwareRender3DCommon*,
    int alpha, bool keycolor, bool alphamap);

  /// Look for a given fog buffer
  //FogBuffer* find_fog_buffer (CS_ID id);

  /**
   * Same as DrawPolygon but no texture mapping.
   * (Flat drawing).
   */
  void DrawPolygonFlat (G3DPolygonDPF& poly);

  /// Start a series of DrawPolygonFX
  void RealStartPolygonFX (iMaterialHandle* handle, uint mode,
  	bool use_fog);


  csRef<iStringSet> strings;
  csRef<iShaderManager> shadermgr;

  iRenderBuffer* activebuffers[16];

public:
  SCF_DECLARE_IBASE;

  /// Report
  void Report (int severity, const char* msg, ...);

  /**
   * Low-level 2D graphics layer.
   * csSoftwareRender3DCommon is in charge of creating and managing this.
   */
  csRef<iGraphics2D> G2D;

  /// The configuration file
  csConfigAccess config;

  /// The texture manager.
  csSoftwareTextureManager* texman;
  /// The vertex buffer manager.
  //csPolArrayVertexBufferManager* vbufmgr;

  /// The texture cache.
  csSoftwareTextureCache *tcache;

  /// The System interface.
  iObjectRegistry* object_reg;

  /// Option variable: do texture lighting?
  bool do_lighting;
  /// Option variable: render alpha-transparent textures?
  bool do_alpha;
  /// Option variable: render textures?
  bool do_textured;
  /// Option variable: do very expensive bilinear filtering? (0/1/2)
  unsigned char bilinear_filter;
  /// Do we want Gouraud Shaded polygons?
  bool do_gouraud;

  /// Do interlacing? (-1 - no, 0/1 - yes)
  int do_interlaced;
  /**
   * For interlacing.  Temporary set to true if we moved quickly.  This will
   * decrease the bluriness a little.
   */
  bool ilace_fastmove;

  /// Render capabilities
  //csGraphics3DCaps Caps;

  // An experimental filter feature.
  static int filter_bf;

  /// Setup scanline drawing routines according to current bpp and setup flags
  csSoftwareRender3DCommon (iBase* parent);
  /// Destructor.
  virtual ~csSoftwareRender3DCommon ();

  /**
   * Initialization method required by iComponent interface.
   * Sets System pointer.
   */
  virtual bool Initialize (iObjectRegistry*);

  /**
   * Open or close our interface.
   */
  bool HandleEvent (iEvent& ev);

  /// Initialize new state from config file
  void NewInitialize ();

  /// Open a canvas.
  virtual bool Open ();

  /// Gathers all that has to be done when opening from scratch.
  bool NewOpen ();

  /**
   * Used when multiple contexts are in system, opens sharing information from
   * other driver.
   */
  bool SharedOpen ();

  /// Scan setup.
  void ScanSetup ();
  /// Close.
  virtual void Close ();

  /// Change the dimensions of the display.
  virtual void SetDimensions (int width, int height);

  /// Start a new frame (see CSDRAW_XXX bit flags)
  virtual bool BeginDraw (int DrawFlags);

  virtual void Print (csRect *area);
  /// End the frame and do a page swap.
  virtual void FinishDraw ();
  /// Draw the projected polygon with light and texture.
  //virtual void DrawPolygon (G3DPolygonDP& poly);

  /**
   * Draw the projected polygon with light and texture.  Debugging version.
   * This one does not actually draw anything but it just prints debug
   * information about what it would have done.
   */
  //virtual void DrawPolygonDebug (G3DPolygonDP& poly);

  /**
   * Initiate a volumetric fog object.  This function will be called before
   * front-facing and back-facing fog polygons are added to the object.  The
   * fog object will be convex but not necesarily closed.  The given CS_ID can
   * be used to identify multiple fog objects when multiple objects are
   * started.
   */
  //virtual void OpenFogObject (CS_ID id, csFog* fog);

  /**
   * Add a front or back-facing fog polygon in the current fog object.  Note
   * that it is guaranteed that all back-facing fog polygons will have been
   * added before the first front-facing polygon.  fogtype can be:
   * <ul>
   *    <li>CS_FOG_FRONT:       a front-facing polygon
   *    <li>CS_FOG_BACK:        a back-facing polygon
   *    <li>CS_FOG_VIEW:        the view-plane
   * </ul>
   */
  //virtual void DrawFogPolygon (CS_ID id, G3DPolygonDFP& poly, int fogtype);

  /**
   * Close a volumetric fog object.  After the volumetric object is closed it
   * should be rendered on screen (whether you do it here or in
   * DrawFrontFog/DrawBackFog() is not important).
   */
  //virtual void CloseFogObject (CS_ID id);

  /// Open a new clipped portal.
  //virtual void OpenPortal (G3DPolygonDFP* poly);

  /// Close a portal previously opened with OpenPortal().
  //virtual void ClosePortal ();

  /// Draw a line in camera space.
  virtual void DrawLine (const csVector3& v1, const csVector3& v2,
    float fov, int color);

  /// Draw a polygon with special effects.
  virtual void DrawPolygonFX (G3DPolygonDPFX& poly);

  /// Set a renderstate boolean.
  //virtual bool SetRenderState (G3D_RENDERSTATEOPTION op, long val);

  /// Get a renderstate value.
  //virtual long GetRenderState (G3D_RENDERSTATEOPTION op);

  /**
   * Get the current driver's capabilities.  Each driver implements their own
   * function.
   */
  /*virtual csGraphics3DCaps *GetCaps ()
  { return &Caps; }*/

  /// Get address of Z-buffer at specific point
  virtual uint32 *GetZBuffAt (int x, int y)
  { return z_buffer + x + y*width; }

  /// Dump the texture cache.
  virtual void DumpCache ();

  /// Clear the texture cache.
  virtual void ClearCache ();

  /// Remove texture from cache.
  virtual void RemoveFromCache (iPolygonTexture* poly_texture);

  /// Get drawing buffer width
  virtual int GetWidth ()
  { return width; }
  /// Get drawing buffer height
  virtual int GetHeight ()
  { return height; }

  /// Set center of projection.
  virtual void SetPerspectiveCenter (int x, int y)
  {
    width2 = x;
    height2 = y;
  }
  /// Get center of projection.
  virtual void GetPerspectiveCenter (int& x, int& y) const
  {
    x = width2;
    y = height2;
  }
  /// Set perspective aspect.
  virtual void SetPerspectiveAspect (float aspect)
  {
    this->aspect = aspect;
    inv_aspect = 1./aspect;
  }
  /// Get perspective aspect.
  virtual float GetPerspectiveAspect () const
  {
    return aspect;
  }
  /// Set world to camera transformation.
  virtual void SetObjectToCamera (csReversibleTransform* o2c)
  {
    this->o2c = *o2c;
  }
  /// Get world to camera transformation.
  virtual const csReversibleTransform& GetObjectToCamera ()
  {
    return o2c;
  }
  /// Set optional clipper.
  virtual void SetClipper (iClipper2D* clipper, int cliptype);
  /// Get clipper.
  virtual iClipper2D* GetClipper ()
  {
    return clipper;
  }
  /// Get cliptype.
  virtual int GetClipType () const { return cliptype; }

  /// Set near clip plane.
  virtual void SetNearPlane (const csPlane3& pl)
  {
    do_near_plane = true;
    near_plane = pl;
  }

  /// Reset near clip plane (i.e. disable it).
  virtual void ResetNearPlane () { do_near_plane = false; }

  /// Get near clip plane.
  virtual const csPlane3& GetNearPlane () const { return near_plane; }

  /// Return true if we have near plane.
  virtual bool HasNearPlane () const { return do_near_plane; }


  /// Draw a triangle mesh.
  //virtual void DrawTriangleMesh (G3DTriangleMesh& mesh);

  /// Draw a polygon mesh.
  //virtual void DrawPolygonMesh (G3DPolygonMesh& mesh);

  /// Get the iGraphics2D driver.
  virtual iGraphics2D *GetDriver2D ()
  { return G2D; }

  /// Get the ITextureManager.
  virtual iTextureManager *GetTextureManager ()
  { return texman; }

  /// Get the vertex buffer manager.
  /*virtual iVertexBufferManager* GetVertexBufferManager ()
  { return vbufmgr; }*/

  /// Check if lightmap is not too large
  //virtual bool IsLightmapOK (iPolygonTexture* poly_texture);

  virtual void SetRenderTarget (iTextureHandle* handle, bool persistent);
  virtual iTextureHandle* GetRenderTarget () { return render_target; }

  /// Get Z-buffer value at given X,Y position
  virtual float GetZBuffValue (int x, int y);

  /// Create a halo of the specified color and return a handle.
  /*virtual iHalo *CreateHalo (float iR, float iG, float iB,
    unsigned char *iAlpha, int iWidth, int iHeight);*/

  /**
   * Draw a sprite (possibly rescaled to given width (sw) and height (sh))
   * using given rectangle from given texture clipped with G2D's clipper.
   */
  virtual void DrawPixmap (iTextureHandle *hTex, int sx, int sy, int sw,
    int sh, int tx, int ty, int tw, int th, uint8 Alpha);











  /// Create a renderbuffer
  virtual csPtr<iRenderBuffer> CreateRenderBuffer (int size, 
    csRenderBufferType type, csRenderBufferComponentType componentType, 
    int componentCount, bool index);

  /// Activate a vertex buffer
  virtual bool ActivateBuffer (csVertexAttrib attrib, iRenderBuffer* buffer)
  {
    activebuffers[attrib] = buffer;
    return true;
  }

  /// Deactivate a vertex buffer
  virtual void DeactivateBuffer (csVertexAttrib attrib)
  {
    activebuffers[attrib] = 0;
  }

  /// Activate a texture
  virtual bool ActivateTexture (iTextureHandle *txthandle, int unit = 0)
  {
    return false;
  }

  /// Activate a texture (Should probably handled some better way)
  virtual bool ActivateTexture (iMaterialHandle *mathandle, int layer, int unit = 0)
  {
    return false;
  }

  /// Deactivate a texture
  virtual void DeactivateTexture (int unit = 0)
  {
  }

  /// Get width of window
  int GetWidth () const
  { return display_width; }

  /// Get height of window
  int GetHeight () const
  { return display_width; }

  /// Capabilities of the driver
  const csRender3dCaps* GetCaps() const
  { return 0; }

  /// Set the z buffer write/test mode
  virtual void SetZMode (csZBufMode mode) 
  {
  }

  // Enables offsetting of Z values
  void EnableZOffset ()
  { 
  }

  // Disables offsetting of Z values
  void DisableZOffset ()
  {
  }

  csReversibleTransform* GetWVMatrix ()
  {
    return &o2c;
  }

  /// Set the masking of color and/or alpha values to framebuffer
  virtual void SetWriteMask (bool red, bool green, bool blue, bool alpha)
  { 
  }

  virtual void GetWriteMask (bool &red, bool &green, bool &blue, bool &alpha) const
  {
  }

  /// Drawroutine. Only way to draw stuff
  void DrawMesh (csRenderMesh* mymesh);

  /// Controls shadow drawing
  virtual void SetShadowState (int state)
  {
  }

  /// Get maximum number of simultaneous vertex lights supported
  virtual int GetMaxLights () const
  {
    return 0;
  }

  /// Sets a parameter for light i
  virtual void SetLightParameter (int i, int param, csVector3 value)
  {
  }

  /// Enables light i
  virtual void EnableLight (int i)
  {
  }

  /// Disables light i
  virtual void DisableLight (int i)
  {
  }

  /// Enable vertex lighting
  virtual void EnablePVL ()
  {
  }

  /// Disable vertex lighting
  virtual void DisablePVL ()
  {
  }

  virtual bool SetRenderState (R3D_RENDERSTATEOPTION op, long val)
  {
    return 0;
  }

  virtual long GetRenderState (R3D_RENDERSTATEOPTION op) const
  {
    return 0;
  }


  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csSoftwareRender3DCommon);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize(p); }
  } scfiComponent;
  struct EventHandler : public iEventHandler
  {
  private:
    csSoftwareRender3DCommon* parent;
  public:
    EventHandler (csSoftwareRender3DCommon* parent)
    {
      SCF_CONSTRUCT_IBASE (0);
      EventHandler::parent = parent;
    }
    SCF_DECLARE_IBASE;
    virtual bool HandleEvent (iEvent& ev)
    {
      return parent->HandleEvent (ev);
    }
  } * scfiEventHandler;
};

#endif // __CS_SFTR3DCOM_H__
