/*
Copyright (C) 2002 by M�rten Svanfeldt
                      Anders Stenberg

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "cssysdef.h"

#include <ctype.h>

#include "qint.h"

#include "csgeom/polyclip.h"
#include "csgeom/transfrm.h"
#include "csgeom/vector4.h"

#include "csutil/objreg.h"
#include "csutil/ref.h"
#include "csutil/scf.h"
#include "csutil/strset.h"

#include "igeom/clip2d.h"

#include "iutil/cmdline.h"
#include "iutil/comp.h"
#include "iutil/plugin.h"
#include "iutil/eventq.h"

#include "ivaria/reporter.h"

#include "ivideo/lighting.h"
#include "ivideo/txtmgr.h"
#include "ivideo/render3d.h"
#include "ivideo/rndbuf.h"

#include "ivideo/shader/shader.h"

#include "gl_render3d.h"
#include "gl_sysbufmgr.h"
#include "gl_txtcache.h"
#include "gl_txtmgr.h"
#include "video/canvas/openglcommon/glextmanager.h"

#include "../common/txtmgr.h"

#define BYTE_TO_FLOAT(x) ((x) * (1.0 / 255.0))


csGLStateCache* csGLRender3D::statecache;


CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (csGLRender3D)

SCF_EXPORT_CLASS_TABLE (glrender3d)
SCF_EXPORT_CLASS_DEP (csGLRender3D, "crystalspace.render3d.opengl",
                      "OpenGL Render3D graphics driver for Crystal Space",
                      "crystalspace.font.server.")
SCF_EXPORT_CLASS_TABLE_END


SCF_IMPLEMENT_IBASE(csGLRender3D)
SCF_IMPLEMENTS_INTERFACE(iRender3D)
SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iComponent)
SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iShaderRenderInterface)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csGLRender3D::eiComponent)
SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csGLRender3D::eiShaderRenderInterface)
SCF_IMPLEMENTS_INTERFACE (iShaderRenderInterface)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csGLRender3D::EventHandler)
SCF_IMPLEMENTS_INTERFACE (iEventHandler)
SCF_IMPLEMENT_IBASE_END


csGLRender3D::csGLRender3D (iBase *parent)
{
  SCF_CONSTRUCT_IBASE (parent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiShaderRenderInterface);

  scfiEventHandler = NULL;


  frustum_valid = false;

  do_near_plane = false;
  viewwidth = 100;
  viewheight = 100;

  stencilclipnum = 0;
  stencil_enabled = false;
  clip_planes_enabled = false;

  render_target = NULL;

  color_red_enabled = false;
  color_green_enabled = false;
  color_blue_enabled = false;
  alpha_enabled = false;
  current_shadow_state = 0;

  //@@@ Test default. Will have to be autodetected later.
  clip_optional[0] = CS_GL_CLIP_PLANES;
  clip_optional[1] = CS_GL_CLIP_STENCIL;
  clip_optional[2] = CS_GL_CLIP_NONE;
  clip_required[0] = CS_GL_CLIP_PLANES;
  clip_required[1] = CS_GL_CLIP_STENCIL;
  clip_required[2] = CS_GL_CLIP_NONE;
  clip_outer[0] = CS_GL_CLIP_PLANES;
  clip_outer[1] = CS_GL_CLIP_STENCIL;
  clip_outer[2] = CS_GL_CLIP_NONE;

  int i;
  for (i=0; i<16; i++)
  {
    vertattrib[i] = NULL;
    vertattribenabled[i] = false;
    texunit[i] = NULL;
    texunitenabled[i] = false;
  }
  lastUsedShaderpass = NULL;
}

csGLRender3D::~csGLRender3D()
{
  delete buffermgr;
  delete txtcache;
  delete txtmgr;

  scfiEventHandler->DecRef();
}




////////////////////////////////////////////////////////////////////
// Private helpers
////////////////////////////////////////////////////////////////////


void csGLRender3D::Report (int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csRef<iReporter> rep (CS_QUERY_REGISTRY (object_reg, iReporter));
  if (rep)
    rep->ReportV (severity, "crystalspace.render3d.opengl", msg, arg);
  else
  {
    csPrintfV (msg, arg);
    csPrintf ("\n");
  }
  va_end (arg);
}

int csGLRender3D::GetMaxTextureSize ()
{
  GLint max;
  glGetIntegerv (GL_MAX_TEXTURE_SIZE, &max);
  return max;
}

void csGLRender3D::SetGlOrtho (bool inverted)
{
  if (render_target)
  {
    if (inverted)
      glOrtho (0., (GLdouble) (viewwidth+1),
      (GLdouble) (viewheight+1), 0., -1.0, 10.0);
    else
      glOrtho (0., (GLdouble) (viewwidth+1), 0.,
      (GLdouble) (viewheight+1), -1.0, 10.0);
  }
  else
  {
    if (inverted)
      glOrtho (0., (GLdouble) viewwidth,
      (GLdouble) viewheight, 0., -1.0, 10.0);
    else
      glOrtho (0., (GLdouble) viewwidth, 0.,
      (GLdouble) viewheight, -1.0, 10.0);
  }
}

void csGLRender3D::SetZMode (csZBufMode mode)
{
  switch (mode)
  {
  case CS_ZBUF_NONE:
    current_zmode = mode;
    statecache->Disable_GL_DEPTH_TEST ();
    break;
  case CS_ZBUF_FILL:
  case CS_ZBUF_FILLONLY:
    current_zmode = mode;
    statecache->Enable_GL_DEPTH_TEST ();
    statecache->SetDepthFunc (GL_ALWAYS);
    statecache->SetDepthMask (GL_TRUE);
    break;
  case CS_ZBUF_EQUAL:
    current_zmode = mode;
    statecache->Enable_GL_DEPTH_TEST ();
    statecache->SetDepthFunc (GL_EQUAL);
    statecache->SetDepthMask (GL_FALSE);
    break;
  case CS_ZBUF_TEST:
    current_zmode = mode;
    statecache->Enable_GL_DEPTH_TEST ();
    statecache->SetDepthFunc (GL_GREATER);
    statecache->SetDepthMask (GL_FALSE);
    break;
  case CS_ZBUF_INVERT:
    current_zmode = mode;
    statecache->Enable_GL_DEPTH_TEST ();
    statecache->SetDepthFunc (GL_LESS);
    statecache->SetDepthMask (GL_FALSE);
    break;
  case CS_ZBUF_USE:
    current_zmode = mode;
    statecache->Enable_GL_DEPTH_TEST ();
    statecache->SetDepthFunc (GL_GEQUAL);
    statecache->SetDepthMask (GL_TRUE);
    break;
  default:
    break;
  }
}

csZBufMode csGLRender3D::GetZModePass2 (csZBufMode mode)
{
  switch (mode)
  {
  case CS_ZBUF_NONE:
  case CS_ZBUF_TEST:
  case CS_ZBUF_EQUAL:
    return mode;
    break;
  case CS_ZBUF_FILL:
  case CS_ZBUF_FILLONLY:
    return CS_ZBUF_EQUAL;
    break;
  case CS_ZBUF_USE:
    return CS_ZBUF_EQUAL;
    break;
  default:
    break;
  }
  return mode;
}

float csGLRender3D::SetMixMode (uint mode, float m_alpha, bool txt_alpha)
{

  // Note: In all explanations of Mixing:
  // Color: resulting color
  // SRC:   Color of the texel (content of the texture to be drawn)
  // DEST:  Color of the pixel on screen
  // Alpha: Alpha value of the polygon
  bool enable_blending = true;
  switch (mode & CS_FX_MASK_MIXMODE)
  {
    case CS_FX_MULTIPLY:
      // Color = SRC * DEST +   0 * SRC = DEST * SRC
      m_alpha = 1.0f;
      statecache->SetBlendFunc (GL_ZERO, GL_SRC_COLOR);
      break;
    case CS_FX_MULTIPLY2:
      // Color = SRC * DEST + DEST * SRC = 2 * DEST * SRC
      m_alpha = 1.0f;
      statecache->SetBlendFunc (GL_DST_COLOR, GL_SRC_COLOR);
      break;
    case CS_FX_ADD:
      // Color = 1 * DEST + 1 * SRC = DEST + SRC
      m_alpha = 1.0f;
      statecache->SetBlendFunc (GL_ONE, GL_ONE);
      break;
    case CS_FX_DESTALPHAADD:
      // Color = DEST + DestAlpha * SRC
      m_alpha = 1.0f;
      statecache->SetBlendFunc (GL_DST_ALPHA, GL_ONE);
      break;
    case CS_FX_ALPHA:
      // Color = Alpha * DEST + (1-Alpha) * SRC
      statecache->SetBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      break;
    case CS_FX_TRANSPARENT:
      // Color = 1 * DEST + 0 * SRC
      m_alpha = 0.0f;
      statecache->SetBlendFunc (GL_ZERO, GL_ONE);
      break;
    case CS_FX_COPY:
    default:
      enable_blending = txt_alpha;
      if (txt_alpha)
      {
        // Color = 0 * DEST + 1 * SRC = SRC
        m_alpha = 1.0f;
        statecache->SetBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      }
      else
      {
	m_alpha = 0;
      }
      break;
  }

  if (enable_blending)
    statecache->Enable_GL_BLEND ();
  else
    statecache->Disable_GL_BLEND ();

  return m_alpha;
}

void csGLRender3D::SetMirrorMode (bool mirror)
{
  if (mirror)
    statecache->SetCullFace (GL_BACK);
  else
    statecache->SetCullFace (GL_FRONT);
}

void csGLRender3D::CalculateFrustum ()
{
  if (frustum_valid) return;
  frustum_valid = true;
  if (clipper)
  {
    frustum.MakeEmpty ();
    int nv = clipper->GetVertexCount ();
    csVector3 v3;
    v3.z = 1;
    csVector2* v = clipper->GetClipPoly ();
    int i;
    for (i = 0 ; i < nv ; i++)
    {
      v3.x = (v[i].x - asp_center_x) * (1.0/aspect);
      v3.y = (v[i].y - asp_center_y) * (1.0/aspect);
      frustum.AddVertex (v3);
    }
  }
}

void csGLRender3D::SetupStencil ()
{
  if (stencil_initialized)
    return;
  
  stencil_initialized = true;

  if (clipper)
  {
    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glLoadIdentity ();
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();
    glLoadIdentity ();
    // First set up the stencil area.
    statecache->Enable_GL_STENCIL_TEST ();
    
    //stencilclipnum++;
    //if (stencilclipnum>255)
    {
      /*glStencilMask (128);
      glClearStencil (128);
      glClear (GL_STENCIL_BUFFER_BIT);*/
      stencilclipnum = 1;
    }
    int nv = clipper->GetVertexCount ();
    csVector2* v = clipper->GetClipPoly ();
    glColor4f (1, 0, 0, 0);
    statecache->SetShadeModel (GL_FLAT);
    SetZMode (CS_ZBUF_NONE);

    statecache->Disable_GL_TEXTURE_2D ();
    if (color_red_enabled || color_green_enabled || color_blue_enabled ||
      alpha_enabled)
      glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    statecache->SetStencilFunc (GL_ALWAYS, 128, 128);
    statecache->SetStencilOp (GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glBegin (GL_TRIANGLE_FAN);
      glVertex2f ( 1, -1);
      glVertex2f (-1, -1);
      glVertex2f (-1,  1);
      glVertex2f ( 1,  1);
    glEnd ();
    glColor4f (0, 1, 0, 0);
    statecache->SetStencilFunc (GL_ALWAYS, 0, 128);

    glBegin (GL_TRIANGLE_FAN);
    int i;
    for (i = 0 ; i < nv ; i++)
      glVertex2f (2.0*v[i].x/(float)viewwidth-1.0,
                  2.0*v[i].y/(float)viewheight-1.0);
    glEnd ();

    if (color_red_enabled || color_green_enabled || color_blue_enabled ||
      alpha_enabled)
      glColorMask (color_red_enabled, color_green_enabled, color_blue_enabled,
        alpha_enabled);

    //statecache->Disable_GL_STENCIL_TEST ();

    glPopMatrix ();
    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
  }
}

int csGLRender3D::SetupClipPlanes (bool add_clipper,
                                   bool add_near_clip,
                                   bool add_z_clip)
{
  /*add_near_clip = false;
  add_z_clip = false;*/
  /*if (clipplane_initialized)
    return;
  
  clipplane_initialized = true;*/

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  glPushMatrix ();

  int i = 0;
  GLdouble plane_eq[4];

  // This routine assumes the hardware planes can handle the
  // required number of planes from the clipper.
  if (clipper && add_clipper)
  {
    CalculateFrustum ();
    csPlane3 pl;
    int i1;
    i1 = frustum.GetVertexCount ()-1;

    for (i = 0 ; i < frustum.GetVertexCount () ; i++)
    {
      pl.Set (csVector3 (0), frustum[i], frustum[i1]);
      plane_eq[0] = pl.A ();
      plane_eq[1] = pl.B ();
      plane_eq[2] = pl.C ();
      plane_eq[3] = pl.D ();
      glClipPlane ((GLenum)(GL_CLIP_PLANE0+i), plane_eq);
      i1 = i;
    }
    if (add_near_clip)
    {
      plane_eq[0] = -near_plane.A ();
      plane_eq[1] = -near_plane.B ();
      plane_eq[2] = -near_plane.C ();
      plane_eq[3] = -near_plane.D ();
      glClipPlane ((GLenum)(GL_CLIP_PLANE0+i), plane_eq);
      i++;
    }
    if (add_z_clip)
    {
      plane_eq[0] = 0;
      plane_eq[1] = 0;
      plane_eq[2] = 1;
      plane_eq[3] = -.001;
      glClipPlane ((GLenum)(GL_CLIP_PLANE0+i), plane_eq);
      i++;
    }
  }

  glPopMatrix ();
  return i;
}

void csGLRender3D::SetupClipper (int clip_portal,
                                 int clip_plane,
                                 int clip_z_plane)
{
  stencil_enabled = false;
  clip_planes_enabled = false;

  //===========
  // First we are going to find out what kind of clipping (if any)
  // we need. This depends on various factors including what the engine
  // says about the mesh (the clip_portal and clip_plane flags in the
  // mesh), what the current clipper is (the current cliptype), what
  // the current z-buf render mode is, and what the settings are to use
  // for the clipper on the current type of hardware (the clip_... arrays).
  //===========
  char how_clip = CS_GL_CLIP_NONE;
  bool use_lazy_clipping = false;
  bool do_plane_clipping = false;
  bool do_z_plane_clipping = false;

  // First we see how many additional planes we might need because of
  // z-plane clipping and/or near-plane clipping. These additional planes
  // will not be usable for portal clipping (if we're using OpenGL plane
  // clipping).
  int reserved_planes =
    int (do_near_plane && clip_plane != CS_CLIP_NOT) +
    int (clip_z_plane != CS_CLIP_NOT);

  if (clip_portal != CS_CLIP_NOT)
  {
    // Some clipping may be required.

    // In some z-buf modes we cannot use clipping modes that depend on
    // zbuffer ('n','N', 'z', or 'Z').
    bool no_zbuf_clipping = (
      current_zmode == CS_ZBUF_NONE || current_zmode == CS_ZBUF_FILL ||
      current_zmode == CS_ZBUF_FILLONLY);

    // Select the right clipping mode variable depending on the
    // type of clipper.
    int ct = cliptype;
    // If clip_portal in the mesh indicates that we might need toplevel
    // clipping then we do as if the current clipper type is toplevel.
    if (clip_portal == CS_CLIP_TOPLEVEL) ct = CS_CLIPPER_TOPLEVEL;
    char* clip_modes;
    switch (ct)
    {
    case CS_CLIPPER_OPTIONAL: clip_modes = clip_optional; break;
    case CS_CLIPPER_REQUIRED: clip_modes = clip_required; break;
    case CS_CLIPPER_TOPLEVEL: clip_modes = clip_outer; break;
    default: clip_modes = clip_optional;
    }
    // Go through all the modes and select the first one that is appropriate.
    for (int i = 0 ; i < 3 ; i++)
    {
      char c = clip_modes[i];
      // We cannot use n,N,z, or Z if no_zbuf_clipping is true.
      if ((c == 'n' || c == 'N' || c == 'z' || c == 'Z') && no_zbuf_clipping)
        continue;
      // We cannot use p or P if the clipper has more vertices than the
      // number of hardware planes minus one (for the view plane).
      if ((c == 'p' || c == 'P') &&
        clipper->GetVertexCount ()
        > 6-reserved_planes)
        continue;
      how_clip = c;
      break;
    }
    if (how_clip != '0' && toupper (how_clip) == how_clip)
    {
      use_lazy_clipping = true;
      how_clip = tolower (how_clip);
    }
  }

  // Check for the near-plane.
  if (do_near_plane && clip_plane != CS_CLIP_NOT)
  {
    do_plane_clipping = true;
    // If we must do clipping to the near plane then we cannot use
    // lazy clipping.
    use_lazy_clipping = false;
    // If we are doing plane clipping already then we don't have
    // to do additional software plane clipping as the OpenGL plane
    // clipper can do this too.
    if (how_clip == 'p')
    {
      do_plane_clipping = false;
    }
  }

  // Check for the z-plane.
  if (clip_z_plane != CS_CLIP_NOT)
  {
    do_z_plane_clipping = true;
    // If hardware requires clipping to the z-plane (because it
    // crashes otherwise) we have to disable lazy clipping.
    // @@@
    if (true)
    {
      use_lazy_clipping = false;
    }
    else
    {
      // If we are doing plane clipping already then we don't have
      // to do additional software plane clipping as the OpenGL plane
      // clipper can do this too.
      if (how_clip == 'p')
      {
        do_z_plane_clipping = false;
      }
    }
  }

  //===========
  // First setup the clipper that we need.
  //===========
  if (how_clip == 's')
  {
    SetupStencil ();
    stencil_enabled = true;
    // Use the stencil area.
    statecache->Enable_GL_STENCIL_TEST ();
    //statecache->SetStencilFunc (GL_EQUAL, stencilclipnum, (unsigned)-1);
    statecache->SetStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
  }

  /*int planes = SetupClipPlanes (how_clip == 'p', 
    do_near_plane && clip_plane != CS_CLIP_NOT,
    clip_z_plane != CS_CLIP_NOT);
  if (planes>0)
  {
    clip_planes_enabled = true;
    for (int i = 0; i < planes; i++)
      glEnable ((GLenum)(GL_CLIP_PLANE0+i));
  }*/
}

void csGLRender3D::ApplyObjectToCamera ()
{
  GLfloat matrixholder[16];
  const csMatrix3 &orientation = object2camera.GetO2T();
  const csVector3 &translation = object2camera.GetO2TTranslation();

  matrixholder[0] = orientation.m11;
  matrixholder[1] = orientation.m21;
  matrixholder[2] = orientation.m31;
  matrixholder[3] = 0.0f;

  matrixholder[4] = orientation.m12;
  matrixholder[5] = orientation.m22;
  matrixholder[6] = orientation.m32;
  matrixholder[7] = 0.0f;

  matrixholder[8] = orientation.m13;
  matrixholder[9] = orientation.m23;
  matrixholder[10] = orientation.m33;
  matrixholder[11] = 0.0f;

  matrixholder[12] = orientation.m11*-translation.x + orientation.m12*-translation.y + orientation.m13*-translation.z;
  matrixholder[13] = orientation.m21*-translation.x + orientation.m22*-translation.y + orientation.m23*-translation.z;
  matrixholder[14] = orientation.m31*-translation.x + orientation.m32*-translation.y + orientation.m33*-translation.z;
  matrixholder[15] = 1.0f;

  
  glMatrixMode (GL_MODELVIEW);
  glLoadMatrixf (matrixholder);
  //glTranslatef (-translation.x, -translation.y, -translation.z);
}




////////////////////////////////////////////////////////////////////
// iRender3D
////////////////////////////////////////////////////////////////////

bool csGLRender3D::Open ()
{
  csRef<iPluginManager> plugin_mgr (
    CS_QUERY_REGISTRY (object_reg, iPluginManager));

  csRef<iCommandLineParser> cmdline (CS_QUERY_REGISTRY (object_reg,
    iCommandLineParser));

  config.AddConfig(object_reg, "/config/r3dopengl.cfg");

  const char *driver = cmdline->GetOption ("canvas");
  if (!driver)
    driver = config->GetStr ("Video.OpenGL.Canvas", CS_OPENGL_2D_DRIVER);

  textureLodBias = config->GetFloat ("Video.OpenGL.TextureLODBias",
    -0.5);

  // @@@ Should check what canvas to load
  G2D = CS_LOAD_PLUGIN (plugin_mgr, driver, iGraphics2D);
  object_reg->Register( G2D, "iGraphics2D");
  if (!G2D)
    return false;

  if (!G2D->Open ())
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Error opening Graphics2D context.");
    return false;
  }
  
  object_reg->Register( G2D, "iGraphics2D");

  G2D->PerformExtension ("getstatecache", &statecache);
  G2D->PerformExtension	("getextmanager", &ext);

  int w = G2D->GetWidth ();
  int h = G2D->GetHeight ();
  SetDimensions (w, h);
  asp_center_x = w/2;
  asp_center_y = h/2;

  /*effectserver = CS_QUERY_REGISTRY(object_reg, iEffectServer);
  if( !effectserver )
  {
  effectserver = CS_LOAD_PLUGIN (plugin_mgr,
  "crystalspace.video.effects.stdserver", iEffectServer);
  object_reg->Register (effectserver, "iEffectServer");
  }*/

  // The extension manager requires to initialize all used extensions with
  // a call to Init<ext> first.
  ext->InitGL_ARB_multitexture ();
  ext->InitGL_ARB_texture_compression ();
  ext->InitGL_SGIS_generate_mipmap ();
  ext->InitGL_EXT_texture_filter_anisotropic ();
  ext->InitGL_EXT_texture_lod_bias ();
  /*
    Check whether to init NVidia-only exts.
    Note: NV extensions supported by multiple vendors
     should always be inited.
   */
  if (config->GetBool ("Video.OpenGL.UseNVidiaExt", true))
  {
    ext->InitGL_NV_vertex_array_range ();
    ext->InitGL_NV_fence ();
  }
  /*
    Check whether to init ATI-only exts.
    Note: ATI extensions supported by multiple vendors
     should always be inited.
   */
  if (config->GetBool ("Video.OpenGL.UseATIExt", true))
  {
    ext->InitGL_ATI_vertex_array_object ();
    ext->InitGL_ATI_vertex_attrib_array_object ();
  }
  varr.ext = ext;

  shadermgr = CS_QUERY_REGISTRY(object_reg, iShaderManager);
  if( !shadermgr )
  {
    shadermgr = csPtr<iShaderManager>
      (CS_LOAD_PLUGIN(plugin_mgr, "crystalspace.render3d.shadermanager", iShaderManager));
    object_reg->Register( shadermgr, "iShaderManager");
  }

  // setup or standard-variables for lighting
  //position
  shvar_light_0_pos = shadermgr->CreateVariable("STANDARD_LIGHT_0_POSITION");
  shvar_light_0_pos->SetType(iShaderVariable::VECTOR4);
  shadermgr->AddVariable(shvar_light_0_pos);
  
  shvar_light_0_diffuse = shadermgr->CreateVariable("STANDARD_LIGHT_0_DIFFUSE");
  shvar_light_0_diffuse->SetType(iShaderVariable::VECTOR4);
  shadermgr->AddVariable(shvar_light_0_diffuse);
  
  shvar_light_0_specular = shadermgr->CreateVariable("STANDARD_LIGHT_0_SPECULAR");
  shvar_light_0_specular->SetType(iShaderVariable::VECTOR4);
  shadermgr->AddVariable(shvar_light_0_specular);

  shvar_light_0_attenuation = shadermgr->CreateVariable("STANDARD_LIGHT_0_ATTENUATION");
  shvar_light_0_attenuation->SetType(iShaderVariable::VECTOR4);
  shadermgr->AddVariable(shvar_light_0_attenuation);

  if (ext->CS_GL_NV_vertex_array_range && ext->CS_GL_NV_fence)
  {
    csVARRenderBufferManager * bm = new csVARRenderBufferManager();
    bm->Initialize(this);
    buffermgr = bm;
  } 
  else if ( ext->CS_GL_ATI_vertex_array_object && 
    ext->CS_GL_ATI_vertex_attrib_array_object)
  {
    csVaoRenderBufferManager* bm = new csVaoRenderBufferManager();
    bm->Initialize(this);
    buffermgr = bm;
    varr.IsATI = true;
  }
  else
  {
    buffermgr.AttachNew (new csSysRenderBufferManager());
  }

  txtcache = new csGLTextureCache (1024*1024*32, this);
  txtmgr.AttachNew (new csGLTextureManager (object_reg, GetDriver2D (), config, this));

  glClearDepth (0.0);
  statecache->Enable_GL_CULL_FACE ();
  statecache->SetCullFace (GL_FRONT);


  string_vertices = strings->Request ("vertices");
  string_texture_coordinates = strings->Request ("texture coordinates");
  string_normals = strings->Request ("normals");
  string_colors = strings->Request ("colors");
  string_indices = strings->Request ("indices");

  return true;
}

void csGLRender3D::Close ()
{
  glFinish ();

  if (txtmgr)
  {
    txtmgr->Clear ();
    //delete txtmgr; txtmgr = NULL;
  }
  txtmgr = NULL;
  if (txtcache)
  {
    txtcache->Clear ();
    //delete txtcache; txtcache = NULL;
  }
  buffermgr = NULL;
  shadermgr = NULL;

  if (G2D)
    G2D->Close ();
}

bool csGLRender3D::BeginDraw (int drawflags)
{
  current_drawflags = drawflags;
  
  if (lastUsedShaderpass)
  {
    lastUsedShaderpass->ResetState ();
    lastUsedShaderpass->Deactivate ();
  }

  SetWriteMask (true,true,true,true);

  int i = 0;
  for (i = 0; i < 16; i++)
  {
    texunit[i] = NULL;
    texunitenabled[i] = false;
  }

  if (render_target)
  {
    int txt_w, txt_h;
    render_target->GetMipMapDimensions (0, txt_w, txt_h);
    if (!rt_cliprectset)
    {
      G2D->GetClipRect (rt_old_minx, rt_old_miny, rt_old_maxx, rt_old_maxy);
      G2D->SetClipRect (-1, -1, txt_w+1, txt_h+1);
      rt_cliprectset = true;

      glMatrixMode (GL_PROJECTION);
      glLoadIdentity ();
      SetGlOrtho (false);
      glViewport (1, -1, viewwidth+1, viewheight+1);
    }

    if (!rt_onscreen)
    {
      /*txtcache->Cache (render_target);
      GLuint handle = ((csTxtCacheData *)render_target->GetCacheData ())
        ->Handle;*/
      statecache->SetShadeModel (GL_FLAT);
      glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
      //statecache->Enable_GL_TEXTURE_2D ();
      //statecache->SetTexture (GL_TEXTURE_2D, handle);
      ActivateTexture (render_target);
      statecache->Disable_GL_BLEND ();
      SetZMode (CS_ZBUF_NONE);

      glBegin (GL_QUADS);
      glTexCoord2f (0, 0); glVertex2i (0, viewheight-txt_h+1);
      glTexCoord2f (0, 1); glVertex2i (0, viewheight-0+1);
      glTexCoord2f (1, 1); glVertex2i (txt_w, viewheight-0+1);
      glTexCoord2f (1, 0); glVertex2i (txt_w, viewheight-txt_h+1);
      glEnd ();
      rt_onscreen = true;
    }
  }

  if (drawflags & CSDRAW_CLEARZBUFFER)
  {
    statecache->SetDepthMask (GL_TRUE);
    if (drawflags & CSDRAW_CLEARSCREEN)
      glClear (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    else
      glClear (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }
  else if (drawflags & CSDRAW_CLEARSCREEN)
    glClear (GL_COLOR_BUFFER_BIT);

  if (!render_target && (drawflags & CSDRAW_3DGRAPHICS))
  {
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    SetGlOrtho (false);
    glViewport (1, -1, viewwidth+1, viewheight+1);
    glTranslatef (viewwidth/2, viewheight/2, 0);

    GLfloat matrixholder[16];
    for (int i = 0 ; i < 16 ; i++) matrixholder[i] = 0.0;
    matrixholder[0] = matrixholder[5] = 1.0;
    matrixholder[11] = 1.0/aspect;
    matrixholder[14] = -matrixholder[11];
    glMultMatrixf (matrixholder);
    return true;
  }
  if (drawflags & CSDRAW_2DGRAPHICS)
  {
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity ();
    glMatrixMode (GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity ();
    SetGlOrtho (false);
    glViewport (1, -1, viewwidth+1, viewheight+1);

    SetZMode (CS_ZBUF_NONE);
    return G2D->BeginDraw ();
  }

  current_drawflags = 0;
  return false;
}

void csGLRender3D::FinishDraw ()
{
  SetMirrorMode (false);

  if (current_drawflags & (CSDRAW_2DGRAPHICS | CSDRAW_3DGRAPHICS))
    G2D->FinishDraw ();
  if (current_drawflags & CSDRAW_2DGRAPHICS)
  {
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode (GL_PROJECTION);
    glPopMatrix();
  }
  current_drawflags = 0;

  if (render_target)
  {
    if (rt_cliprectset)
    {
      rt_cliprectset = false;
      G2D->SetClipRect (rt_old_minx, rt_old_miny, rt_old_maxx, rt_old_maxy);
      glMatrixMode (GL_PROJECTION);
      glLoadIdentity ();
      glOrtho (0., viewwidth, 0., viewheight, -1.0, 10.0);
      glViewport (0, 0, viewwidth, viewheight);
    }

    if (rt_onscreen)
    {
      rt_onscreen = false;
      //statecache->Enable_GL_TEXTURE_2D ();
      SetZMode (CS_ZBUF_NONE);
      statecache->Disable_GL_BLEND ();
      statecache->Disable_GL_ALPHA_TEST ();
      int txt_w, txt_h;
      render_target->GetMipMapDimensions (0, txt_w, txt_h);
      csGLTextureHandle* tex_mm = (csGLTextureHandle *)
        render_target->GetPrivateObject ();
      csGLTexture *tex_0 = tex_mm->vTex[0];
      csTxtCacheData *tex_data = (csTxtCacheData*)render_target->GetCacheData();
      if (tex_data)
      {
        // Texture is in tha cache, update texture directly.
        //statecache->SetTexture (GL_TEXTURE_2D, tex_data->Handle);
        ActivateTexture (render_target);
        // Texture was not used as a render target before.
        // Make some necessary adjustments.
        if (!tex_mm->was_render_target)
        {
          if (!(tex_mm->GetFlags() & CS_TEXTURE_NOMIPMAPS))
          {
            if (ext->CS_GL_SGIS_generate_mipmap)
            {
              glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
            }
            else
            {
              glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                txtcache->GetBilinearMapping() ? GL_LINEAR : GL_NEAREST);
            }
          }
          tex_mm->was_render_target = true;
        }
        glCopyTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, 1, viewheight-txt_h,
          txt_w, txt_h, 0);
      }
      else
      {
        // Not in cache.
#ifdef GL_VERSION_1_2x
        if (pfmt.PixelBytes == 2)
        {
          char* buffer = new char[2*txt_w*txt_h]; // @@@ Alloc elsewhere!!!
          glReadPixels (0, height-txt_h, txt_w, txt_h,
            GL_RGB, GL_UNSIGNED_SHORT_5_6_5, buffer);

          csRGBpixel *dst = tex_0->get_image_data ();
          uint16 bb = 8 - pfmt.BlueBits;
          uint16 gb = 8 - pfmt.GreenBits;
          uint16 rb = 8 - pfmt.RedBits;
          uint16 *src = (uint16*) buffer;
          int i;
          for (i = 0 ; i < width*height ; i++, src++, dst++)
          {
            dst->red = ((*src & pfmt.RedMask) >> pfmt.RedShift) << rb;
            dst->green = ((*src & pfmt.GreenMask) >> pfmt.GreenShift) << gb;
            dst->blue = ((*src & pfmt.BlueMask) >> pfmt.BlueShift) << bb;
          }
          delete[] buffer;
        }
        else
          glReadPixels (1, height-txt_h, txt_w, txt_h,
          GL_RGBA, GL_UNSIGNED_BYTE, tex_0->get_image_data());
#else
        glReadPixels (1, viewheight-txt_h, txt_w, txt_h, tex_mm->SourceFormat (),
          tex_mm->SourceType (), tex_0->get_image_data ());
#endif
      }
    }
  }
  render_target = NULL;
}

void csGLRender3D::Print (csRect* area)
{
  //glFinish ();
  if (bugplug)
    bugplug->ResetCounter ("Triangle Count");
  G2D->Print (area);
}

csReversibleTransform* csGLRender3D::GetWVMatrix()
{
  return &object2camera;
}

void csGLRender3D::SetObjectToCamera(csReversibleTransform* wvmatrix)
{
  const csMatrix3 &orientation1 = object2camera.GetO2T();
  const csVector3 &translation1 = object2camera.GetO2TTranslation();
  const csMatrix3 &orientation2 = wvmatrix->GetO2T();
  const csVector3 &translation2 = wvmatrix->GetO2TTranslation();
  if (translation1 == translation2 &&
      orientation1.Col1 () == orientation2.Col1 () &&
      orientation1.Col2 () == orientation2.Col2 () &&
      orientation1.Col3 () == orientation2.Col3 ())
      return;
  object2camera = *wvmatrix;
  ApplyObjectToCamera ();
}

void csGLRender3D::DrawLine(const csVector3 & v1, const csVector3 & v2, float fov, int color)
{
  if (v1.z < SMALL_Z && v2.z < SMALL_Z)
    return;

  float x1 = v1.x, y1 = v1.y, z1 = v1.z;
  float x2 = v2.x, y2 = v2.y, z2 = v2.z;

  if (z1 < SMALL_Z)
  {
    // x = t*(x2-x1)+x1;
    // y = t*(y2-y1)+y1;
    // z = t*(z2-z1)+z1;
    float t = (SMALL_Z - z1) / (z2 - z1);
    x1 = t * (x2 - x1) + x1;
    y1 = t * (y2 - y1) + y1;
    z1 = SMALL_Z;
  }
  else if (z2 < SMALL_Z)
  {
    // x = t*(x2-x1)+x1;
    // y = t*(y2-y1)+y1;
    // z = t*(z2-z1)+z1;
    float t = (SMALL_Z - z1) / (z2 - z1);
    x2 = t * (x2 - x1) + x1;
    y2 = t * (y2 - y1) + y1;
    z2 = SMALL_Z;
  }
  float iz1 = fov / z1;
  int px1 = QInt (x1 * iz1 + (viewwidth / 2));
  int py1 = viewheight - 1 - QInt (y1 * iz1 + (viewheight / 2));
  float iz2 = fov / z2;
  int px2 = QInt (x2 * iz2 + (viewwidth / 2));
  int py2 = viewheight - 1 - QInt (y2 * iz2 + (viewheight / 2));

  G2D->DrawLine (px1, py1, px2, py2, color);
}

bool csGLRender3D::ActivateBuffer (csVertexAttrib attrib, iRenderBuffer* buffer)
{
  bool bind = true;
  if (vertattrib[attrib] == buffer)
  {
    if (vertattribenabled[attrib])
      return true;
    bind = false;
  }
  if (bind && vertattrib[attrib])
  {
    vertattrib[attrib]->Release ();
    vertattrib[attrib] = NULL;
  }
  
  static GLenum type[] = {GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, 
    GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT, GL_FLOAT, GL_DOUBLE };
  void* data = buffer->Lock (CS_BUF_LOCK_RENDER);
  if (data)
  {
    ext->glEnableVertexAttribArrayARB (attrib);
    if (bind)
      varr.VertexAttribPointer (
        attrib, buffer->GetComponentCount (), 
        type[buffer->GetComponentType ()], true, 0, data);
    vertattrib[attrib] = buffer;
    vertattribenabled[attrib] = true;
  }
  return true;
}

void csGLRender3D::DeactivateBuffer (csVertexAttrib attrib)
{
  if (vertattrib[attrib])
  {
    ext->glDisableVertexAttribArrayARB (attrib);
    vertattrib[attrib]->Release ();
    vertattribenabled[attrib] = false;
  }
}

bool csGLRender3D::ActivateTexture (iTextureHandle *txthandle, int unit)
{
  bool bind = true;
  if (texunit[unit] == txthandle)
  {
    if (texunitenabled[unit])
      return true;
    bind = false;
  }

  if (bind && texunit[unit])
    DeactivateTexture (unit);

  if (ext->CS_GL_ARB_multitexture)
  {
    ext->glActiveTextureARB(GL_TEXTURE0_ARB + unit);
    ext->glClientActiveTextureARB(GL_TEXTURE0_ARB + unit);
  } else if (unit != 0) return false;

  txtcache->Cache (txthandle);
  csGLTextureHandle *gltxthandle = (csGLTextureHandle *)
    txthandle->GetPrivateObject ();
  csTxtCacheData *cachedata =
    (csTxtCacheData *)gltxthandle->GetCacheData ();

  switch (gltxthandle->target)
  {
  case iTextureHandle::CS_TEX_IMG_1D:
    statecache->Enable_GL_TEXTURE_1D (unit);
    if (bind)
      glBindTexture (GL_TEXTURE_1D, cachedata->Handle );
    texunit[unit] = txthandle;
    texunitenabled[unit] = true;
    break;
  case iTextureHandle::CS_TEX_IMG_2D:
    statecache->Enable_GL_TEXTURE_2D (unit);
    if (bind)
      glBindTexture (GL_TEXTURE_2D, cachedata->Handle );
    if (ext->CS_GL_EXT_texture_lod_bias)
    {
      glTexEnvi (GL_TEXTURE_FILTER_CONTROL_EXT, 
	GL_TEXTURE_LOD_BIAS_EXT, textureLodBias); //big hack
    }
    texunit[unit] = txthandle;
    texunitenabled[unit] = true;
    break;
  case iTextureHandle::CS_TEX_IMG_3D:
    statecache->Enable_GL_TEXTURE_3D (unit);
    if (bind)
      glBindTexture (GL_TEXTURE_3D, cachedata->Handle );
    texunit[unit] = txthandle;
    texunitenabled[unit] = true;
    break;
  case iTextureHandle::CS_TEX_IMG_CUBEMAP:
    statecache->Enable_GL_TEXTURE_CUBE_MAP (unit);
    if (bind)
      glBindTexture (GL_TEXTURE_CUBE_MAP, cachedata->Handle);
    texunit[unit] = txthandle;
    texunitenabled[unit] = true;
    break;
  default:
    return false;
  }
  return true;
}

bool csGLRender3D::ActivateTexture (iMaterialHandle *mathandle, int layer, int unit)
{
  iTextureHandle* txthandle = NULL;
  csMaterialHandle* mathand = (csMaterialHandle*)mathandle;
  if(layer == 0)
  {
    txthandle = mathand->GetTexture();
    if (!txthandle)
      return false;
  } else if(layer > 0 && layer <= mathand->GetTextureLayerCount() )
  {
    csTextureLayer* matlayer = mathand->GetTextureLayer(layer-1);
    txthandle = matlayer->txt_handle;
    if (!txthandle)
      return false;
  } else return false;

  bool bind = true;
  if (texunit[unit] == txthandle)
  {
    if (texunitenabled[unit])
      return true;
    bind = false;
  }

  if (bind && texunit[unit])
    DeactivateTexture (unit);

  if (ext->CS_GL_ARB_multitexture)
  {
    ext->glActiveTextureARB(GL_TEXTURE0_ARB + unit);
    ext->glClientActiveTextureARB(GL_TEXTURE0_ARB + unit);
  } else if (unit != 0) return false;

  txtcache->Cache (txthandle);
  csGLTextureHandle *gltxthandle = (csGLTextureHandle *)
    txthandle->GetPrivateObject ();
  csTxtCacheData *cachedata =
    (csTxtCacheData *)gltxthandle->GetCacheData ();

  switch (gltxthandle->target)
  {
  case iTextureHandle::CS_TEX_IMG_1D:
    statecache->Enable_GL_TEXTURE_1D (unit);
    if (bind)
      glBindTexture (GL_TEXTURE_1D, cachedata->Handle );
    texunit[unit] = txthandle;
    texunitenabled[unit] = true;
    break;
  case iTextureHandle::CS_TEX_IMG_2D:
    statecache->Enable_GL_TEXTURE_2D (unit);
    if (bind)
      glBindTexture (GL_TEXTURE_2D, cachedata->Handle );
    if (ext->CS_GL_EXT_texture_lod_bias)
    {
      glTexEnvi (GL_TEXTURE_FILTER_CONTROL_EXT, 
	GL_TEXTURE_LOD_BIAS_EXT, textureLodBias); //big hack
    }
    texunit[unit] = txthandle;
    texunitenabled[unit] = true;
    break;
  case iTextureHandle::CS_TEX_IMG_3D:
    statecache->Enable_GL_TEXTURE_3D (unit);
    if (bind)
      glBindTexture (GL_TEXTURE_3D, cachedata->Handle );
    texunit[unit] = txthandle;
    texunitenabled[unit] = true;
    break;
  case iTextureHandle::CS_TEX_IMG_CUBEMAP:
    statecache->Enable_GL_TEXTURE_CUBE_MAP (unit);
    if (bind)
      glBindTexture (GL_TEXTURE_CUBE_MAP, cachedata->Handle);
    texunit[unit] = txthandle;
    texunitenabled[unit] = true;
    break;
  default:
    return false;
  }
  return true;
}


void csGLRender3D::DeactivateTexture (int unit)
{
  if (!texunitenabled[unit])
    return;

  if (ext->CS_GL_ARB_multitexture)
  {
    ext->glActiveTextureARB(GL_TEXTURE0_ARB + unit);
    ext->glClientActiveTextureARB(GL_TEXTURE0_ARB + unit);
  } else if (unit != 0) return;

  csGLTextureHandle *gltxthandle = (csGLTextureHandle *)
    texunit[unit]->GetPrivateObject ();
  switch (gltxthandle->target)
  {
  case iTextureHandle::CS_TEX_IMG_1D:
    statecache->Disable_GL_TEXTURE_1D (unit);
    //glBindTexture (GL_TEXTURE_1D, NULL);
    break;
  case iTextureHandle::CS_TEX_IMG_2D:
    statecache->Disable_GL_TEXTURE_2D (unit);
    //glBindTexture (GL_TEXTURE_2D, NULL);
    break;
  case iTextureHandle::CS_TEX_IMG_3D:
    statecache->Disable_GL_TEXTURE_3D (unit);
    //glBindTexture (GL_TEXTURE_3D, NULL);
    break;
  case iTextureHandle::CS_TEX_IMG_CUBEMAP:
    statecache->Disable_GL_TEXTURE_CUBE_MAP (unit);
    //glBindTexture (GL_TEXTURE_CUBE_MAP, NULL);
    break;
  }
  texunitenabled[unit] = false;
}

void csGLRender3D::DrawMesh(csRenderMesh* mymesh)
{
  /*SetupClipper (mymesh->clip_portal, 
                mymesh->clip_plane, 
                mymesh->clip_z_plane);*/

  SetZMode (mymesh->z_buf_mode);
  // @@@ maybe GL can do this for us? or in the VPs, maybe?
  SetObjectToCamera (mymesh->transform);

  GLenum primitivetype;
  switch (mymesh->meshtype)
  {
  case CS_MESHTYPE_QUADS:
    primitivetype = GL_QUADS;
    break;
  case CS_MESHTYPE_TRIANGLESTRIP:
    primitivetype = GL_TRIANGLE_STRIP;
    break;
  case CS_MESHTYPE_TRIANGLEFAN:
    primitivetype = GL_TRIANGLE_FAN;
    break;
  case CS_MESHTYPE_POINTS:
    primitivetype = GL_POINTS;
    break;
  case CS_MESHTYPE_LINES:
    primitivetype = GL_LINES;
    break;
  case CS_MESHTYPE_LINESTRIP:
    primitivetype = GL_LINE_STRIP;
    break;
  case CS_MESHTYPE_TRIANGLES:
  default:
    primitivetype = GL_TRIANGLES;
    break;
  }

  switch (current_shadow_state)
  {
  case CS_SHADOW_VOLUME_PASS1:
    statecache->SetStencilOp (GL_KEEP, GL_KEEP, GL_INCR);
    statecache->SetStencilFunc (GL_ALWAYS, 0, 255);
    break;
  case CS_SHADOW_VOLUME_FAIL1:
    statecache->SetStencilOp (GL_KEEP, GL_INCR, GL_KEEP);
    statecache->SetStencilFunc (GL_ALWAYS, 0, 255);
    break;
  case CS_SHADOW_VOLUME_PASS2:
    statecache->SetStencilOp (GL_KEEP, GL_KEEP, GL_DECR);
    statecache->SetStencilFunc (GL_ALWAYS, 0, 255);
    break;
  case CS_SHADOW_VOLUME_FAIL2:
    statecache->SetStencilOp (GL_KEEP, GL_DECR, GL_KEEP);
    statecache->SetStencilFunc (GL_ALWAYS, 0, 255);
    break;
  case CS_SHADOW_VOLUME_USE:
    statecache->SetStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
    if (stencil_enabled)
      statecache->SetStencilFunc (GL_EQUAL, 0, 255);
    else
      statecache->SetStencilFunc (GL_EQUAL, 0, 127);
    break;
  }

  if (current_shadow_state == CS_SHADOW_VOLUME_PASS2 ||
      current_shadow_state == CS_SHADOW_VOLUME_FAIL1)
    SetMirrorMode (!mymesh->do_mirror);
  else
    SetMirrorMode (mymesh->do_mirror);

//  csMaterialHandle* mathandle = 
//    (csMaterialHandle*)(mymesh->mathandle);
  csMaterialHandle* mathandle = mymesh->material ?
    ((csMaterialHandle*)(mymesh->material->GetMaterialHandle())) :
    NULL;

  statecache->SetShadeModel (GL_SMOOTH);

  /*csRef<iTextureHandle> txthandle;
  if (mathandle)
    txthandle = mathandle->GetTexture ();

  float red = 1, green = 1, blue = 1, alpha = 1;
  if (txthandle)
  {

    alpha = 1.0f - BYTE_TO_FLOAT (mymesh->mixmode & CS_FX_MASK_ALPHA);
    alpha = SetMixMode (mymesh->mixmode, alpha, 
      txthandle->GetKeyColor () || txthandle->GetAlphaMap ());

  } else {
    statecache->Disable_GL_TEXTURE_2D ();

    csRGBpixel color;
    if (mathandle)
      mathandle->GetFlatColor (color);
    
    red = BYTE_TO_FLOAT (color.red);
    green = BYTE_TO_FLOAT (color.green);
    blue = BYTE_TO_FLOAT (color.blue);
    alpha = SetMixMode (mymesh->mixmode, 1, false);
  }

  glColor4f (red, green, blue, alpha);*/

  csRef<iShader> shader;
  if (mathandle)
     shader = mathandle->GetMaterial()->GetShader();

  glEnableClientState(GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV);
  bool useshader = false;
  if(shader && shader->IsValid())
  {
    useshader = true;
    //iShaderTechnique* tech = shader->GetBestTechnique();

    iStreamSource* source = mymesh->streamsource;
    iRenderBuffer* indexbuf =
      source->GetBuffer (string_indices);
    if (!indexbuf)
      return;

    SetMixMode (mymesh->mixmode, 0, true);

    /*int currp;
    for(currp = 0; currp< tech->GetPassCount(); ++currp)
    {
      iShaderPass *pass = tech->GetPass (currp);
      uint mixmode = pass->GetMixmodeOverride ();
      if (mixmode != 0)
        SetMixMode (mixmode, 0, true);

      if (pass != lastUsedShaderpass)
      {
        if (lastUsedShaderpass)
        {
          lastUsedShaderpass->Deactivate ();
        }

        pass->Activate (mymesh);
      }
      pass->SetupState (mymesh);*/

      if (bugplug)
        bugplug->AddCounter ("Triangle Count", (mymesh->indexend-mymesh->indexstart)/3);

      glDrawElements (
        primitivetype,
        mymesh->indexend-mymesh->indexstart,
        GL_UNSIGNED_INT,
        ((unsigned int*)indexbuf->Lock(CS_BUF_LOCK_RENDER))
        +mymesh->indexstart);

      //pass->Deactivate(mymesh);
      /*pass->ResetState ();
      lastUsedShaderpass = pass;*/
      indexbuf->Release ();
    //}
  } else {
    /*if (lastUsedShaderpass)
    {
      lastUsedShaderpass->ResetState ();
      lastUsedShaderpass->Deactivate ();
      lastUsedShaderpass = NULL;
    }*/

    iStreamSource* source = mymesh->streamsource;
    iRenderBuffer* vertexbuf =
      source->GetBuffer (string_vertices);
    iRenderBuffer* texcoordbuf = NULL;
      //source->GetBuffer (string_texture_coordinates);
    iRenderBuffer* normalbuf = NULL;
      //source->GetBuffer (string_normals);
    iRenderBuffer* colorbuf = NULL;
      //source->GetBuffer (string_colors);
    iRenderBuffer* indexbuf =
      source->GetBuffer (string_indices);

    if (!vertexbuf || vertexbuf->IsDiscarded())
      return;
    if (!indexbuf)
      return;

    varr.VertexPointer (3, GL_FLOAT, 0,
      (float*)vertexbuf->Lock(CS_BUF_LOCK_RENDER));
    glEnableClientState (GL_VERTEX_ARRAY);

    /*if (texcoordbuf && !texcoordbuf->IsDiscarded())
    {
      varr.TexCoordPointer (2, GL_FLOAT, 0, (float*)
        texcoordbuf->Lock(CS_BUF_LOCK_RENDER));
      glEnableClientState (GL_TEXTURE_COORD_ARRAY);
    }
    if (normalbuf && !normalbuf->IsDiscarded())
    {
      varr.NormalPointer (GL_FLOAT, 0, (float*)
        normalbuf->Lock(CS_BUF_LOCK_RENDER));
      glEnableClientState (GL_NORMAL_ARRAY);
    }
    if (colorbuf && !colorbuf->IsDiscarded())
    {
      varr.ColorPointer (4, GL_FLOAT, 0, (float*)
        colorbuf->Lock(CS_BUF_LOCK_RENDER));
      glEnableClientState (GL_COLOR_ARRAY);
    }*/

    if (bugplug)
        bugplug->AddCounter ("Triangle Count", (mymesh->indexend-mymesh->indexstart)/3);

    glColor4f (0, 0, 0, 0);
    glDrawElements (
      primitivetype,
      mymesh->indexend-mymesh->indexstart,
      GL_UNSIGNED_INT,
      ((unsigned int*)indexbuf->Lock(CS_BUF_LOCK_RENDER))
      +mymesh->indexstart);

    /*if (mathandle)
    {
      for (int l=0; l<mathandle->GetTextureLayerCount (); l++)
      {
        if (indexbuf) // must make sure that the indexbuffer is aviable for locking
          indexbuf->Release();

        csTextureLayer* layer = mathandle->GetTextureLayer (l);
        txthandle = layer->txt_handle;
        if (txthandle)
        {
          ActivateTexture (txthandle);
          txtcache->Cache (txthandle);
          csGLTextureHandle *gltxthandle = (csGLTextureHandle *)
            txthandle->GetPrivateObject ();
          csTxtCacheData *cachedata =
            (csTxtCacheData *)gltxthandle->GetCacheData ();

          statecache->SetTexture (GL_TEXTURE_2D, cachedata->Handle);
          statecache->Enable_GL_TEXTURE_2D ();
        } else continue;

        alpha = 1.0f - BYTE_TO_FLOAT (layer->mode & CS_FX_MASK_ALPHA);
        alpha = SetMixMode (layer->mode, alpha, 
          txthandle->GetKeyColor () || txthandle->GetAlphaMap ());
        glColor4f (1, 1, 1, alpha);

        glMatrixMode (GL_TEXTURE);
        glPushMatrix ();
        GLfloat scalematrix[16];
        int i;
        for (i = 0 ; i < 16 ; i++) scalematrix[i] = 0.0;
        scalematrix[0] = layer->uscale;
        scalematrix[5] = layer->vscale;
        scalematrix[10] = 1;
        scalematrix[15] = 1;
        scalematrix[12] = layer->ushift*layer->uscale; 
        scalematrix[13] = layer->vshift*layer->vscale;
        // @@@ Shift is ignored for now.
        glLoadMatrixf (scalematrix);

        if (bugplug)
          bugplug->AddCounter ("Triangle Count", (mymesh->indexend-mymesh->indexstart)/3);

        glDrawElements (
          primitivetype,
          mymesh->indexend-mymesh->indexstart,
          GL_UNSIGNED_INT,
          ((unsigned int*)indexbuf->Lock(CS_BUF_LOCK_RENDER))
          +mymesh->indexstart);

        glPopMatrix ();
      }
    }*/

    vertexbuf->Release();
    glDisableClientState (GL_VERTEX_ARRAY);
    indexbuf->Release();
    /*if (texcoordbuf)
    {
      glDisableClientState (GL_TEXTURE_COORD_ARRAY);
      texcoordbuf->Release();
    }
    if (colorbuf)
    {
      glDisableClientState (GL_COLOR_ARRAY);
      colorbuf->Release();
    }
    if (normalbuf)
    {
      glDisableClientState (GL_NORMAL_ARRAY);
      normalbuf->Release();
    }*/
  }
  glDisableClientState(GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV);

  /*if (clip_planes_enabled)
  {
    clip_planes_enabled = false;
    for (int i = 0; i < 6; i++)
      glDisable ((GLenum)(GL_CLIP_PLANE0+i));
  }
  if (stencil_enabled)
  {
    //stencil_enabled = false;
    //statecache->Disable_GL_STENCIL_TEST ();
  }*/
}

void csGLRender3D::SetShadowState (int state)
{
  switch (state)
  {
  case CS_SHADOW_VOLUME_BEGIN:
    current_shadow_state = CS_SHADOW_VOLUME_BEGIN;
    stencil_initialized = false;
    glClearStencil (0);
    glClear (GL_STENCIL_BUFFER_BIT);
    statecache->Enable_GL_STENCIL_TEST ();
    statecache->SetStencilFunc (GL_ALWAYS, 0, 127);
    //statecache->SetStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
    glPolygonOffset (-0.1, -4); 
    statecache->Enable_GL_POLYGON_OFFSET_FILL ();
    break;
  case CS_SHADOW_VOLUME_PASS1:
    current_shadow_state = CS_SHADOW_VOLUME_PASS1;
    break;
  case CS_SHADOW_VOLUME_FAIL1:
    current_shadow_state = CS_SHADOW_VOLUME_FAIL1;
	break;
  case CS_SHADOW_VOLUME_PASS2:
    current_shadow_state = CS_SHADOW_VOLUME_PASS2;
    break;
  case CS_SHADOW_VOLUME_FAIL2:
    current_shadow_state = CS_SHADOW_VOLUME_FAIL2;
    break;
  case CS_SHADOW_VOLUME_USE:
    current_shadow_state = CS_SHADOW_VOLUME_USE;
    statecache->Disable_GL_POLYGON_OFFSET_FILL ();
    break;
  case CS_SHADOW_VOLUME_FINISH:
    current_shadow_state = 0;
    statecache->Disable_GL_STENCIL_TEST ();
    break;
  }
}


void csGLRender3D::SetClipper (iClipper2D* clipper, int cliptype)
{
  //clipper = new csBoxClipper (10, 10, 200, 200);
  csGLRender3D::clipper = clipper;
  if (!clipper) cliptype = CS_CLIPPER_NONE;
  csGLRender3D::cliptype = cliptype;
  clipplane_initialized = false;
  stencil_initialized = false;
  frustum_valid = false;
}

void csGLRender3D::SetLightParameter (int i, int param, csVector3 value)
{
  if(i != 0) return; //not implemented any other light than first yet
  switch (param)
  {
  case CS_LIGHTPARAM_POSITION:
    value *= object2camera;
    shvar_light_0_pos->SetValue(csVector4(value));
    break;
  case CS_LIGHTPARAM_DIFFUSE:
    shvar_light_0_diffuse->SetValue(csVector4(value));
    break;
  case CS_LIGHTPARAM_SPECULAR:
    shvar_light_0_specular->SetValue(csVector4(value));
    break;
  case CS_LIGHTPARAM_ATTENUATION:
    shvar_light_0_attenuation->SetValue(csVector4(value));
    break;
  }
}

// @@@ doesn't serve any purpose for now, but might in the future.
// left in for now.
bool csGLRender3D::SetRenderState (R3D_RENDERSTATEOPTION op, long val)
{
  return false;
}

long csGLRender3D::GetRenderState (R3D_RENDERSTATEOPTION op)
{
  return 0;
}

////////////////////////////////////////////////////////////////////
// iComponent
////////////////////////////////////////////////////////////////////




bool csGLRender3D::Initialize (iObjectRegistry* p)
{
  object_reg = p;

  if (!scfiEventHandler)
    scfiEventHandler = new EventHandler (this);

  csRef<iEventQueue> q (CS_QUERY_REGISTRY(object_reg, iEventQueue));
  if (q)
    q->RegisterListener (scfiEventHandler, CSMASK_Broadcast);

  scfiShaderRenderInterface.Initialize(p);

  bugplug = CS_QUERY_REGISTRY (object_reg, iBugPlug);

  strings = CS_QUERY_REGISTRY_TAG_INTERFACE (object_reg, "crystalspace.renderer.stringset", iStringSet);
  if (!strings)
  {
    strings = csPtr<iStringSet> (new csScfStringSet ());
    object_reg->Register (strings, "crystalspace.renderer.stringset");
  }

  return true;
}




////////////////////////////////////////////////////////////////////
// iEventHandler
////////////////////////////////////////////////////////////////////




bool csGLRender3D::HandleEvent (iEvent& Event)
{
  if (Event.Type == csevBroadcast)
    switch (Event.Command.Code)
  {
    case cscmdSystemOpen:
      {
        Open ();
        return true;
      }
    case cscmdSystemClose:
      {
        Close ();
        return true;
      }
  }
  return false;
}


////////////////////////////////////////////////////////////////////
//                    iShaderRenderInterface
////////////////////////////////////////////////////////////////////

csGLRender3D::eiShaderRenderInterface::eiShaderRenderInterface()
{
}

csGLRender3D::eiShaderRenderInterface::~eiShaderRenderInterface()
{

}

void* csGLRender3D::eiShaderRenderInterface::GetPrivateObject(const char* name)
{
  if(strcasecmp(name, "ext") == 0)
    return (void*) (scfParent->ext);
  if(strcasecmp(name, "txtcache") == 0)
    return (void*) (scfParent->txtcache);
  if(strcasecmp(name, "varr") == 0)
    return (void*) (&scfParent->varr);
  return NULL;
}

void csGLRender3D::eiShaderRenderInterface::Initialize(iObjectRegistry *reg)
{
  object_reg = reg;
}
