/*
  Copyright (C) 2002 by Marten Svanfeldt
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

#include "csgfx/csimgvec.h"
#include "csgfx/memimage.h"

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
#include "ivideo/graph3d.h"
#include "ivideo/rendermesh.h"
#include "ivideo/halo.h"

#include "ivideo/shader/shader.h"

#include "gl_render3d.h"
#include "gl_renderbuffer.h"
#include "gl_txtcache.h"
#include "gl_txtmgr.h"
#include "gl_polyrender.h"

#include "video/canvas/openglcommon/glextmanager.h"

#include "../common/txtmgr.h"

#define BYTE_TO_FLOAT(x) ((x) * (1.0 / 255.0))

csGLStateCache* csGLGraphics3D::statecache = 0;
csGLExtensionManager* csGLGraphics3D::ext = 0;

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (csGLGraphics3D)

SCF_IMPLEMENT_IBASE(csGLGraphics3D)
  SCF_IMPLEMENTS_INTERFACE(iGraphics3D)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iComponent)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iShaderRenderInterface)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iDebugHelper)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csGLGraphics3D::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csGLGraphics3D::eiShaderRenderInterface)
  SCF_IMPLEMENTS_INTERFACE (iShaderRenderInterface)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csGLGraphics3D::EventHandler)
  SCF_IMPLEMENTS_INTERFACE (iEventHandler)
SCF_IMPLEMENT_IBASE_END


csGLGraphics3D::csGLGraphics3D (iBase *parent)
{
  SCF_CONSTRUCT_IBASE (parent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiShaderRenderInterface);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiDebugHelper);

  verbose = false;
  frustum_valid = false;

  do_near_plane = false;
  viewwidth = 100;
  viewheight = 100;

  stencilclipnum = 0;
  clip_planes_enabled = false;

  render_target = 0;

  color_red_enabled = false;
  color_green_enabled = false;
  color_blue_enabled = false;
  alpha_enabled = false;
  current_shadow_state = 0;

  use_hw_render_buffers = false;
  stencil_threshold = 500;
  broken_stencil = false;

  int i;
  for (i=0; i<16; i++)
  {
    vertattrib[i] = 0;
    vertattribenabled[i] = false;
    vertattribenabled100[i] = false;
    texunittarget[i] = 0;
    texunitenabled[i] = false;
  }
//  lastUsedShaderpass = 0;

  scrapIndicesSize = 0;
  scrapVerticesSize = 0;

  shadow_stencil_enabled = false;
  clipping_stencil_enabled = false;
  clipportal_dirty = true;

  current_drawflags = 0;
}

csGLGraphics3D::~csGLGraphics3D()
{
  csRef<iEventQueue> q (CS_QUERY_REGISTRY(object_reg, iEventQueue));
  if (q)
    q->RemoveListener (scfiEventHandler);

  SCF_DESTRUCT_EMBEDDED_IBASE(scfiDebugHelper);
  SCF_DESTRUCT_EMBEDDED_IBASE(scfiShaderRenderInterface);
  SCF_DESTRUCT_EMBEDDED_IBASE(scfiComponent);
  SCF_DESTRUCT_IBASE();
}

////////////////////////////////////////////////////////////////////
// Private helpers
////////////////////////////////////////////////////////////////////


void csGLGraphics3D::Report (int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csRef<iReporter> rep (CS_QUERY_REGISTRY (object_reg, iReporter));
  if (rep)
    rep->ReportV (severity, "crystalspace.graphics3d.opengl", msg, arg);
  else
  {
    csPrintfV (msg, arg);
    csPrintf ("\n");
  }
  va_end (arg);
}

void csGLGraphics3D::EnableStencilShadow ()
{
  shadow_stencil_enabled = true;
  statecache->Enable_GL_STENCIL_TEST ();
}

void csGLGraphics3D::DisableStencilShadow ()
{
  shadow_stencil_enabled = false;
  if (clipping_stencil_enabled) return;
  statecache->Disable_GL_STENCIL_TEST ();
}

void csGLGraphics3D::EnableStencilClipping ()
{
  clipping_stencil_enabled = true;
  statecache->Enable_GL_STENCIL_TEST ();
}

void csGLGraphics3D::DisableStencilClipping ()
{
  clipping_stencil_enabled = false;
  if (shadow_stencil_enabled) return;
  statecache->Disable_GL_STENCIL_TEST ();
}

void csGLGraphics3D::SetGlOrtho (bool inverted)
{
  /// @@@ Why was this here in the first place? /Anders Stenberg
  /*if (render_target)
  {
    if (inverted)
      glOrtho (0., (GLdouble) viewwidth,
      (GLdouble) (viewheight), 0., -1.0, 10.0);
    else
      glOrtho (0., (GLdouble) viewwidth, 0.,
      (GLdouble) (viewheight), -1.0, 10.0);
  }
  else
  {*/
    if (inverted)
      glOrtho (0., (GLdouble) viewwidth, 
      (GLdouble) viewheight, 0., -1.0, 10.0);
    else
      glOrtho (0., (GLdouble) viewwidth, 0.,
      (GLdouble) viewheight, -1.0, 10.0);
  //}
}

csZBufMode csGLGraphics3D::GetZModePass2 (csZBufMode mode)
{
  switch (mode)
  {
    case CS_ZBUF_NONE:
    case CS_ZBUF_TEST:
    case CS_ZBUF_EQUAL:
      return mode;
    case CS_ZBUF_FILL:
    case CS_ZBUF_FILLONLY:
    case CS_ZBUF_USE:
      return CS_ZBUF_EQUAL;
    default:
      return CS_ZBUF_NONE;
  }
}

void csGLGraphics3D::SetZMode (csZBufMode mode, bool internal)
{
  if (!internal)
    current_zmode = mode;

  switch (mode)
  {
    case CS_ZBUF_NONE:
      statecache->Disable_GL_DEPTH_TEST ();
      break;
    case CS_ZBUF_FILL:
    case CS_ZBUF_FILLONLY:
      statecache->Enable_GL_DEPTH_TEST ();
      statecache->SetDepthFunc (GL_ALWAYS);
      statecache->SetDepthMask (GL_TRUE);
      break;
    case CS_ZBUF_EQUAL:
      statecache->Enable_GL_DEPTH_TEST ();
      statecache->SetDepthFunc (GL_EQUAL);
      statecache->SetDepthMask (GL_FALSE);
      break;
    case CS_ZBUF_TEST:
      statecache->Enable_GL_DEPTH_TEST ();
      statecache->SetDepthFunc (GL_GREATER);
      statecache->SetDepthMask (GL_FALSE);
      break;
    case CS_ZBUF_INVERT:
      statecache->Enable_GL_DEPTH_TEST ();
      statecache->SetDepthFunc (GL_LESS);
      statecache->SetDepthMask (GL_FALSE);
      break;
    case CS_ZBUF_USE:
      statecache->Enable_GL_DEPTH_TEST ();
      statecache->SetDepthFunc (GL_GEQUAL);
      statecache->SetDepthMask (GL_TRUE);
      break;
    default:
      break;
  }
}

void csGLGraphics3D::SetMixMode (uint mode)
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
      statecache->SetBlendFunc (GL_ZERO, GL_SRC_COLOR);
      break;
    case CS_FX_MULTIPLY2:
      // Color = SRC * DEST + DEST * SRC = 2 * DEST * SRC
      statecache->SetBlendFunc (GL_DST_COLOR, GL_SRC_COLOR);
      break;
    case CS_FX_ADD:
      // Color = 1 * DEST + 1 * SRC = DEST + SRC
      statecache->SetBlendFunc (GL_ONE, GL_ONE);
      break;
    case CS_FX_DESTALPHAADD:
      // Color = DEST + DestAlpha * SRC
      statecache->SetBlendFunc (GL_DST_ALPHA, GL_ONE);
      break;
    case CS_FX_SRCALPHAADD:
      // Color = DEST + SrcAlpha * SRC
      statecache->SetBlendFunc (GL_SRC_ALPHA, GL_ONE);
      break;
    case CS_FX_ALPHA:
      // Color = Alpha * SRC + (1-Alpha) * DEST
      statecache->SetBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      break;
    case CS_FX_TRANSPARENT:
      // Color = 1 * DEST + 0 * SRC
      statecache->SetBlendFunc (GL_ZERO, GL_ONE);
      break;
    case CS_FX_COPY:
    default:
      enable_blending = false;
      break;
  }

  if (enable_blending)
    statecache->Enable_GL_BLEND ();
  else
    statecache->Disable_GL_BLEND ();
}

void csGLGraphics3D::SetAlphaType (csAlphaMode::AlphaType alphaType)
{
  switch (alphaType)
  {
    default:
    case csAlphaMode::alphaNone:
      statecache->Disable_GL_BLEND ();
      statecache->Disable_GL_ALPHA_TEST ();
      break;
    case csAlphaMode::alphaBinary:
      statecache->Disable_GL_BLEND ();
      statecache->Enable_GL_ALPHA_TEST ();
      statecache->SetAlphaFunc (GL_GEQUAL, 0.5f);
      break;
    case csAlphaMode::alphaSmooth:
      statecache->Enable_GL_BLEND ();
      statecache->Disable_GL_ALPHA_TEST ();
      statecache->SetBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      break;
  }
}

void csGLGraphics3D::SetMirrorMode (bool mirror)
{
  if (mirror)
    statecache->SetCullFace (GL_BACK);
  else
    statecache->SetCullFace (GL_FRONT);
}

void csGLGraphics3D::CalculateFrustum ()
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

void csGLGraphics3D::SetupStencil ()
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
    EnableStencilClipping ();

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

    statecache->SetShadeModel (GL_FLAT);

    bool oldz = statecache->IsEnabled_GL_DEPTH_TEST ();
    if (oldz) statecache->Disable_GL_DEPTH_TEST ();
    bool tex2d = statecache->IsEnabled_GL_TEXTURE_2D ();
    if (tex2d) statecache->Disable_GL_TEXTURE_2D ();

    if (color_red_enabled || color_green_enabled || color_blue_enabled ||
        alpha_enabled)
      glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    statecache->SetStencilMask (128);
    statecache->SetStencilFunc (GL_ALWAYS, 128, 128);
    statecache->SetStencilOp (GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glBegin (GL_TRIANGLE_FAN);
      glVertex2f ( 1, -1);
      glVertex2f (-1, -1);
      glVertex2f (-1,  1);
      glVertex2f ( 1,  1);
    glEnd ();

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

    statecache->SetStencilMask (127);

    glPopMatrix ();
    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    if (oldz) statecache->Enable_GL_DEPTH_TEST ();
    if (tex2d) statecache->Enable_GL_TEXTURE_2D ();
  }
}

int csGLGraphics3D::SetupClipPlanes (bool add_clipper,
                                   bool add_near_clip,
                                   bool add_z_clip)
{
  if (!(add_clipper || add_near_clip || add_z_clip)) return 0;

  glMatrixMode (GL_MODELVIEW);
  glPushMatrix ();
  glLoadIdentity ();

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

    int maxfrustplanes = 6;
    if (add_near_clip) maxfrustplanes--;
    if (add_z_clip) maxfrustplanes--;
    int numfrustplanes = frustum.GetVertexCount ();
    // Correct for broken stencil implementation.
    if (numfrustplanes > maxfrustplanes)
      numfrustplanes = maxfrustplanes;

    for (i = 0 ; i < numfrustplanes ; i++)
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

void csGLGraphics3D::SetupClipper (int clip_portal,
                                 int clip_plane,
                                 int clip_z_plane,
				 int tri_count)
{
  if (cache_clip_portal == clip_portal &&
      cache_clip_plane == clip_plane &&
      cache_clip_z_plane == clip_z_plane)
    return;
  cache_clip_portal = clip_portal;
  cache_clip_plane = clip_plane;
  cache_clip_z_plane = clip_z_plane;

  clip_planes_enabled = false;

  //===========
  // First we are going to find out what kind of clipping (if any)
  // we need. This depends on various factors including what the engine
  // says about the mesh (the clip_portal and clip_plane flags in the
  // mesh), what the current clipper is (the current cliptype),
  // and what the prefered clipper (stencil or glClipPlane).
  //===========

  // If the following flag becomes true in this routine then this means
  // that for portal clipping we will use stencil.
  bool clip_with_stencil = false;
  // If the following flag becomes true in this routine then this means
  // that for portal clipping we will use glClipPlane. This flag does
  // not say anything about z-plane and near plane clipping.
  bool clip_with_planes = false;
  // If one of the following flags is true then this means
  // that we will have to do plane clipping using glClipPlane for the near
  // or z=0 plane.
  bool do_plane_clipping = (do_near_plane && clip_plane != CS_CLIP_NOT);
  bool do_z_plane_clipping = (clip_z_plane != CS_CLIP_NOT);

  bool m_prefer_stencil;
  if (tri_count > stencil_threshold) m_prefer_stencil = true;
  else m_prefer_stencil = false;

  // First we see how many additional planes we might need because of
  // z-plane clipping and/or near-plane clipping. These additional planes
  // will not be usable for portal clipping (if we're using OpenGL plane
  // clipping).
  int reserved_planes = int (do_plane_clipping) + int (do_z_plane_clipping);

  if (clip_portal != CS_CLIP_NOT)//@@@??? && cliptype != CS_CLIPPER_OPTIONAL)
  {
    // Some clipping may be required.
    if (m_prefer_stencil)
      clip_with_stencil = true;
    else if (clipper->GetVertexCount () > 6-reserved_planes)
    {
      if (broken_stencil)
      {
        // If the stencil is broken we will clip with planes
	// even if we don't have enough planes. We will just
	// ignore the other planes then.
        clip_with_stencil = false;
        clip_with_planes = true;
      }
      else
      {
        clip_with_stencil = true;
      }
    }
    else
      clip_with_planes = true;
  }

  //===========
  // First setup the clipper that we need.
  //===========
  if (clip_with_stencil)
  {
    SetupStencil ();
    // Use the stencil area.
    EnableStencilClipping ();
  }
  else
  {
    DisableStencilClipping ();
  }

  int planes = SetupClipPlanes (clip_with_planes, do_plane_clipping,
  	do_z_plane_clipping);
  if (planes > 0)
  {
    clip_planes_enabled = true;
    for (int i = 0; i < planes; i++)
      glEnable ((GLenum)(GL_CLIP_PLANE0+i));
  }
  // @@@ Hard coded max number of planes (6). Maybe not so good.
  for (int i = planes ; i < 6 ; i++)
    glDisable ((GLenum)(GL_CLIP_PLANE0+i));
}

void csGLGraphics3D::ApplyObjectToCamera ()
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

  matrixholder[12] = 0.0f;
  matrixholder[13] = 0.0f;
  matrixholder[14] = 0.0f;
  matrixholder[15] = 1.0f;

  glMatrixMode (GL_MODELVIEW);
  glLoadMatrixf (matrixholder);
  glTranslatef (-translation.x, -translation.y, -translation.z);
}

void csGLGraphics3D::SetupProjection ()
{
  if (!needProjectionUpdate) return;

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  SetGlOrtho (render_target);
  if (render_target)
  {
    int txt_w, txt_h;
    render_target->GetMipMapDimensions (0, txt_w, txt_h);

    /*
      Need a different translation for PTs, they are in the upper left.
      @@@ Not very nice to have that here,
      @@@ furthermore, the perspective center Y coordinate is effectively
          hardcoded
     */

    glTranslatef (asp_center_x, (txt_h / 2), 0);
  }
  else
    glTranslatef (asp_center_x, asp_center_y, 0);

  GLfloat matrixholder[16];
  for (int i = 0 ; i < 16 ; i++) matrixholder[i] = 0.0;
  matrixholder[0] = matrixholder[5] = 1.0;
  matrixholder[11] = 1.0/aspect;
  matrixholder[14] = -matrixholder[11];
  glMultMatrixf (matrixholder);

  glMatrixMode (GL_MODELVIEW);
  needProjectionUpdate = false;
}

////////////////////////////////////////////////////////////////////
// iGraphics3D
////////////////////////////////////////////////////////////////////


static void FillNormalizationMapSide (unsigned char *normdata, int size, 
                          int xx, int xy, int xo,
                          int yx, int yy, int yo,
                          int zx, int zy, int zo)
{
  const float halfSize = size / 2;
  for (int y=0; y < size; y++)
  {
    float yv = (y + 0.5) / halfSize - 1.0f;
    for (int x=0; x < size; x++)
    {
      float xv = (x + 0.5) / halfSize - 1.0f;
      csVector3 norm = csVector3 (
        xo + xv*xx + yv*xy, 
        yo + xv*yx + yv*yy, 
        zo + xv*zx + yv*zy);
      norm.Normalize ();
      *normdata++ = (unsigned char)(127.5f + norm.x*127.5f);
      *normdata++ = (unsigned char)(127.5f + norm.y*127.5f);
      *normdata++ = (unsigned char)(127.5f + norm.z*127.5f);
      *normdata++ = 0;
    }
  }
}


bool csGLGraphics3D::Open ()
{
  csRef<iPluginManager> plugin_mgr = CS_QUERY_REGISTRY (
  	object_reg, iPluginManager);
  csRef<iCommandLineParser> cmdline = CS_QUERY_REGISTRY (
  	object_reg, iCommandLineParser);

  verbose = cmdline->GetOption ("verbose") != 0;
  if (!verbose) bugplug = 0;

  textureLodBias = config->GetFloat ("Video.OpenGL.TextureLODBias",
    -0.3f);
  if (verbose)
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Texture LOD bias %g", textureLodBias);
 
  if (!G2D->Open ())
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Error opening Graphics2D context.");
    return false;
  }

  SetPerspectiveAspect (G2D->GetHeight ());
  SetPerspectiveCenter (G2D->GetWidth ()/2, G2D->GetHeight ()/2);
  
  object_reg->Register( G2D, "iGraphics2D");

  G2D->PerformExtension ("getstatecache", &statecache);
  G2D->PerformExtension	("getextmanager", &ext);

  int w = G2D->GetWidth ();
  int h = G2D->GetHeight ();
  SetDimensions (w, h);
  asp_center_x = w/2;
  asp_center_y = h/2;

  // The extension manager requires to initialize all used extensions with
  // a call to Init<ext> first.
  ext->InitGL_ARB_multitexture ();
  ext->InitGL_EXT_texture3D ();
  //ext->InitGL_EXT_texture_compression_s3tc (); // not used atm
  ext->InitGL_ARB_vertex_buffer_object ();
  ext->InitGL_SGIS_generate_mipmap ();
  ext->InitGL_EXT_texture_filter_anisotropic ();
  ext->InitGL_EXT_texture_lod_bias ();
  ext->InitGL_EXT_stencil_wrap ();
  ext->InitGL_EXT_stencil_two_side ();
  ext->InitGL_ARB_point_parameters ();
  ext->InitGL_ARB_point_sprite ();
  /*
    Check whether to init NVidia-only exts.
    Note: NV extensions supported by multiple vendors
     should always be inited.
   */
  if (config->GetBool ("Video.OpenGL.UseNVidiaExt", true))
  {
  }
  /*
    Check whether to init ATI-only exts.
    Note: ATI extensions supported by multiple vendors
     should always be inited.
   */
  if (config->GetBool ("Video.OpenGL.UseATIExt", true))
  {
    ext->InitGL_ATI_separate_stencil ();
  }

  rendercaps.minTexHeight = 2;
  rendercaps.minTexWidth = 2;
  GLint mts = config->GetInt ("Video.OpenGL.Caps.MaxTextureSize", -1);
  if (mts == -1)
  {
    glGetIntegerv (GL_MAX_TEXTURE_SIZE, &mts);
    if (mts == 0)
    {
      // There appears to be a bug in some OpenGL drivers where
      // getting the maximum texture size simply doesn't work. In that
      // case we will issue a warning about this and assume 256x256.
      mts = 256;
      Report (CS_REPORTER_SEVERITY_WARNING, "Detecting maximum texture size fails! 256x256 is assumed.\nEdit Video.OpenGL.Caps.MaxTextureSize if you want to change.");
    }
  }
  if (verbose)
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Maximum texture size is %dx%d", mts, mts);
  rendercaps.maxTexHeight = mts;
  rendercaps.maxTexWidth = mts;

  rendercaps.SupportsPointSprites = ext->CS_GL_ARB_point_parameters &&
    ext->CS_GL_ARB_point_sprite;
  if (verbose)
    if (rendercaps.SupportsPointSprites)
      Report (CS_REPORTER_SEVERITY_NOTIFY, "Point sprites are supported.");
    else
      Report (CS_REPORTER_SEVERITY_NOTIFY, "Point sprites are NOT supported.");

  // check for support of VBO
  use_hw_render_buffers = ext->CS_GL_ARB_vertex_buffer_object;
  if (verbose)
    if (use_hw_render_buffers)
      Report (CS_REPORTER_SEVERITY_NOTIFY, "VBO is supported.");
    else
      Report (CS_REPORTER_SEVERITY_NOTIFY, "VBO is NOT supported.");

  stencil_threshold = config->GetInt ("Video.OpenGL.StencilThreshold", 500);
  broken_stencil = false;
  if (config->GetBool ("Video.OpenGL.BrokenStencil", false))
  {
    broken_stencil = true;
    stencil_threshold = 1000000000;
  }
  if (verbose)
    if (broken_stencil)
      Report (CS_REPORTER_SEVERITY_NOTIFY, "Stencil clipping is broken!");
    else
    {
      Report (CS_REPORTER_SEVERITY_NOTIFY, "Stencil clipping is used for objects >= %d triangles.", stencil_threshold);
    }

  shadermgr = CS_QUERY_REGISTRY (object_reg, iShaderManager);
  if (!shadermgr)
  {
    shadermgr = csPtr<iShaderManager> (CS_LOAD_PLUGIN (
    	plugin_mgr, "crystalspace.graphics3d.shadermanager", iShaderManager));
    object_reg->Register (shadermgr, "iShaderManager");
  }

  int texCacheSize = 1024*1024*64;
  const char* texCacheSizeStr = config->GetStr (
    "Video.OpenGL.MaxTextureCache", "64m");
  const char* end = texCacheSizeStr + strspn (texCacheSizeStr,
    "0123456789");
  int texSizeFactor = 1;
  if ((*end == 'k') || (*end == 'K')) 
    texSizeFactor = 1024;
  else if ((*end == 'm') || (*end == 'M')) 
    texSizeFactor = 1024*1024;
  else 
  {
    Report (CS_REPORTER_SEVERITY_WARNING, 
      "Unknown suffix '%s' in maximum texture cache size.", end);
    texSizeFactor = 0;
  }
  if (texSizeFactor != 0)
  {
    if (sscanf (texCacheSizeStr, "%d", &texCacheSize) != 0)
      texCacheSize *= texSizeFactor;
    else
      Report (CS_REPORTER_SEVERITY_WARNING, 
	"Invalid texture cache size '%s'.", texCacheSizeStr);
  }

  txtcache = csPtr<csGLTextureCache> (new csGLTextureCache (
  	texCacheSize, this)); 

  const char* filterModeStr = config->GetStr (
    "Video.OpenGL.TextureFilter", "trilinear");
  int filterMode = 2;
  if (strcmp (filterModeStr, "none") == 0)
    filterMode = 0;
  else if (strcmp (filterModeStr, "nearest") == 0)
    filterMode = 0;
  else if (strcmp (filterModeStr, "bilinear") == 0)
    filterMode = 1;
  else if (strcmp (filterModeStr, "trilinear") == 0)
    filterMode = 2;
  else
  {
    Report (CS_REPORTER_SEVERITY_WARNING, 
      "Invalid texture filter mode '%s'.", filterModeStr);
  }
  txtcache->SetBilinearMapping (filterMode);

  txtmgr.AttachNew (new csGLTextureManager (
  	object_reg, GetDriver2D (), config, this, txtcache));

  glClearDepth (0.0);
  statecache->Enable_GL_CULL_FACE ();
  statecache->SetCullFace (GL_FRONT);

  statecache->SetStencilMask (127);

  // Set up texture LOD bias.
  if (ext->CS_GL_EXT_texture_lod_bias)
  {
    if (ext->CS_GL_ARB_multitexture)
    {
      int texUnits;
      glGetIntegerv (GL_MAX_TEXTURE_UNITS_ARB, &texUnits);
      for (int u = texUnits - 1; u >= 0; u--)
      {
	statecache->SetActiveTU (u);
	glTexEnvf (GL_TEXTURE_FILTER_CONTROL_EXT, 
	  GL_TEXTURE_LOD_BIAS_EXT, textureLodBias); 
      }
    }
    else
    {
      glTexEnvf (GL_TEXTURE_FILTER_CONTROL_EXT, 
	GL_TEXTURE_LOD_BIAS_EXT, textureLodBias); 
    }
  }

  string_vertices = strings->Request ("vertices");
  string_texture_coordinates = strings->Request ("texture coordinates");
  string_normals = strings->Request ("normals");
  string_colors = strings->Request ("colors");
  string_indices = strings->Request ("indices");
  string_point_radius = strings->Request ("point radius");
  string_point_scale = strings->Request ("point scale");
  string_texture_diffuse = strings->Request (CS_MATERIAL_TEXTURE_DIFFUSE);


  // @@@ These shouldn't be here, I guess.
  #define CS_FOGTABLE_SIZE 64
  // Each texel in the fog table holds the fog alpha value at a certain
  // (distance*density).  The median distance parameter determines the
  // (distance*density) value represented by the texel at the center of
  // the fog table.  The fog calculation is:
  // alpha = 1.0 - exp( -(density*distance) / CS_FOGTABLE_MEDIANDISTANCE)
  #define CS_FOGTABLE_MEDIANDISTANCE 10.0f
  #define CS_FOGTABLE_MAXDISTANCE (CS_FOGTABLE_MEDIANDISTANCE * 2.0f)
  #define CS_FOGTABLE_DISTANCESCALE (1.0f / CS_FOGTABLE_MAXDISTANCE)

  unsigned char *transientfogdata = new unsigned char[CS_FOGTABLE_SIZE * 4];
  for (unsigned int fogindex = 0; fogindex < CS_FOGTABLE_SIZE; fogindex++)
  {
    transientfogdata[fogindex * 4 + 0] = (unsigned char) 255;
    transientfogdata[fogindex * 4 + 1] = (unsigned char) 255;
    transientfogdata[fogindex * 4 + 2] = (unsigned char) 255;
    double fogalpha = (256 * (1.0 - exp (-float (fogindex)
      * CS_FOGTABLE_MAXDISTANCE / CS_FOGTABLE_SIZE)));
    transientfogdata[fogindex * 4 + 3] = (unsigned char) fogalpha;
  }
  transientfogdata[(CS_FOGTABLE_SIZE - 1) * 4 + 3] = 0;

  csRef<iImage> img = csPtr<iImage> (new csImageMemory (
    CS_FOGTABLE_SIZE, 1, transientfogdata, true, 
    CS_IMGFMT_TRUECOLOR | CS_IMGFMT_ALPHA));
  csRef<iImageVector> imgvec = csPtr<iImageVector> (new csImageVector ());
  imgvec->AddImage (img);
  csRef<iTextureHandle> fogtex = txtmgr->RegisterTexture (
    imgvec, CS_TEXTURE_3D | CS_TEXTURE_CLAMP | CS_TEXTURE_NOMIPMAPS, 
    iTextureHandle::CS_TEX_IMG_2D);

  csRef<csShaderVariable> fogvar = csPtr<csShaderVariable> (
  	new csShaderVariable (strings->Request ("standardtex fog")));
  fogvar->SetValue (fogtex);
  shadermgr->AddVariable(fogvar);

  const int normalizeCubeSize = config->GetInt (
    "Video.OpenGL.NormalizeCubeSize", 256);

  imgvec = csPtr<iImageVector> (new csImageVector ());

  // Positive X
  unsigned char *normdata = 
    new unsigned char[normalizeCubeSize*normalizeCubeSize*4];
  FillNormalizationMapSide (normdata, normalizeCubeSize,  0,  0,  1,
                                                          0, -1,  0,
                                                         -1,  0,  0);
  img = csPtr<iImage> (new csImageMemory (
    normalizeCubeSize, normalizeCubeSize, normdata, true,
    CS_IMGFMT_TRUECOLOR));
  imgvec->AddImage (img);

  // Negative X
  normdata = new unsigned char[normalizeCubeSize*normalizeCubeSize*4];
  FillNormalizationMapSide (normdata, normalizeCubeSize,  0,  0, -1,
                                                          0, -1,  0,
                                                          1,  0,  0);
  img = csPtr<iImage> (new csImageMemory (
    normalizeCubeSize, normalizeCubeSize, normdata, true, 
    CS_IMGFMT_TRUECOLOR));
  imgvec->AddImage (img);

  // Positive Y
  normdata = new unsigned char[normalizeCubeSize*normalizeCubeSize*4];
  FillNormalizationMapSide (normdata, normalizeCubeSize,  1,  0,  0,
                                                          0,  0,  1,
                                                          0,  1,  0);
  img = csPtr<iImage> (new csImageMemory (
    normalizeCubeSize, normalizeCubeSize, normdata, true, 
    CS_IMGFMT_TRUECOLOR));
  imgvec->AddImage (img);

  // Negative Y
  normdata = new unsigned char[normalizeCubeSize*normalizeCubeSize*4];
  FillNormalizationMapSide (normdata, normalizeCubeSize,  1,  0,  0,
                                                          0,  0, -1,
                                                          0, -1,  0);
  img = csPtr<iImage> (new csImageMemory (
    normalizeCubeSize, normalizeCubeSize, normdata, true, 
    CS_IMGFMT_TRUECOLOR));
  imgvec->AddImage (img);

  // Positive Z
  normdata = new unsigned char[normalizeCubeSize*normalizeCubeSize*4];
  FillNormalizationMapSide (normdata, normalizeCubeSize,  1,  0,  0,
                                                          0, -1,  0,
                                                          0,  0,  1);
  img = csPtr<iImage> (new csImageMemory (
    normalizeCubeSize, normalizeCubeSize, normdata, true, 
    CS_IMGFMT_TRUECOLOR));
  imgvec->AddImage (img);

  // Negative Z
  normdata = new unsigned char[normalizeCubeSize*normalizeCubeSize*4];
  FillNormalizationMapSide (normdata, normalizeCubeSize, -1,  0,  0,
                                                          0, -1,  0,
                                                          0,  0, -1);
  img = csPtr<iImage> (new csImageMemory (
    normalizeCubeSize, normalizeCubeSize, normdata, true, 
    CS_IMGFMT_TRUECOLOR));
  imgvec->AddImage (img);

  csRef<iTextureHandle> normtex = txtmgr->RegisterTexture (
    imgvec, CS_TEXTURE_3D | CS_TEXTURE_CLAMP | CS_TEXTURE_NOMIPMAPS, 
    iTextureHandle::CS_TEX_IMG_CUBEMAP);

  csRef<csShaderVariable> normvar = csPtr<csShaderVariable> (
  	new csShaderVariable (
		strings->Request ("standardtex normalization map")));
  normvar->SetValue (normtex);
  shadermgr->AddVariable(normvar);

  cache_clip_portal = -1;
  cache_clip_plane = -1;
  cache_clip_z_plane = -1;

  return true;
}

void csGLGraphics3D::Close ()
{
  glFinish ();

  if (txtmgr)
  {
    txtmgr->Clear ();
    //delete txtmgr; txtmgr = 0;
  }
  txtmgr = 0;
  if (txtcache)
  {
    txtcache->Clear ();
    // txtcache = 0;
  }
  shadermgr = 0;

  if (G2D)
    G2D->Close ();
}

bool csGLGraphics3D::BeginDraw (int drawflags)
{
/*  if (lastUsedShaderpass)
  {
    lastUsedShaderpass->ResetState ();
    lastUsedShaderpass->Deactivate ();
  }
*/
  SetWriteMask (true, true, true, true);

  int i = 0;
  for (i = 15; i >= 0; i--)
    DeactivateTexture (i);

  // if 2D graphics is not locked, lock it
  if ((drawflags & (CSDRAW_2DGRAPHICS | CSDRAW_3DGRAPHICS))
   && (!(current_drawflags & (CSDRAW_2DGRAPHICS | CSDRAW_3DGRAPHICS))))
  {
    if (!G2D->BeginDraw ())
      return false;
  }
  current_drawflags = drawflags;

  if (render_target)
  {
    int txt_w, txt_h;
    render_target->GetMipMapDimensions (0, txt_w, txt_h);
    if (!rt_cliprectset)
    {
      G2D->GetClipRect (rt_old_minx, rt_old_miny, rt_old_maxx, rt_old_maxy);
      G2D->SetClipRect (-1, -1, txt_w+1, txt_h+1);
      rt_cliprectset = true;
    }

    if (!rt_onscreen)
    {
      glMatrixMode (GL_PROJECTION);
      glLoadIdentity ();
      SetGlOrtho (false);
      glViewport (0, 0, viewwidth, viewheight);
      glMatrixMode (GL_MODELVIEW);
      glLoadIdentity ();

      statecache->SetShadeModel (GL_FLAT);
      glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
      ActivateTexture (render_target);
      statecache->Disable_GL_BLEND ();
      SetZMode (CS_ZBUF_NONE);

      glBegin (GL_QUADS);
      glTexCoord2f (0, 0); glVertex2i (0, viewheight-txt_h);
      glTexCoord2f (0, 1); glVertex2i (0, viewheight);
      glTexCoord2f (1, 1); glVertex2i (txt_w, viewheight);
      glTexCoord2f (1, 0); glVertex2i (txt_w, viewheight-txt_h);
      glEnd ();
      rt_onscreen = true;
    }
  }

  if (drawflags & CSDRAW_CLEARZBUFFER)
  {
    statecache->SetDepthMask (GL_TRUE);
    if (drawflags & CSDRAW_CLEARSCREEN)
      glClear (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT
      	| GL_COLOR_BUFFER_BIT);
    else
      glClear (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }
  else if (drawflags & CSDRAW_CLEARSCREEN)
    glClear (GL_COLOR_BUFFER_BIT);

  if (drawflags & CSDRAW_3DGRAPHICS)
  {
    glViewport (0, 0, viewwidth, viewheight);
    needProjectionUpdate = true;

    glCullFace (render_target ? GL_BACK : GL_FRONT);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    object2camera = csReversibleTransform();
    return true;
  }
  else if (drawflags & CSDRAW_2DGRAPHICS)
  {
    /*
      Turn off some stuff that isn't needed for 2d (or even can
      cause visual glitches.)
     */
    if (use_hw_render_buffers)
    {
      ext->glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0);
      ext->glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    }
    statecache->Disable_GL_ALPHA_TEST ();
    if (ext->CS_GL_ARB_multitexture)
      statecache->SetActiveTU (0);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    if (render_target)
    {
      int txt_w, txt_h;
      render_target->GetMipMapDimensions (0, txt_w, txt_h);
      /*
	Render target: draw everything in top-left corner, but flipped.
      */
      glOrtho (0., (GLdouble) viewwidth, (GLdouble) (2 * viewheight - txt_h), 
	(GLdouble) (viewheight - txt_h), -1.0, 10.0);
      glCullFace (GL_BACK);
    }
    else
      SetGlOrtho (false);
    //glViewport (0, 0, viewwidth, viewheight);
    glViewport (0, 0, viewwidth, viewheight);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

    SetZMode (CS_ZBUF_NONE);
    
    SetMixMode (CS_FX_COPY);
    glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
    return true;//G2D->BeginDraw ();
  }

  current_drawflags = 0;
  return false;
}

void csGLGraphics3D::FinishDraw ()
{
  SetMirrorMode (false);

  if (current_drawflags & (CSDRAW_2DGRAPHICS | CSDRAW_3DGRAPHICS))
    G2D->FinishDraw ();

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

    if ((current_drawflags & (CSDRAW_2DGRAPHICS | CSDRAW_3DGRAPHICS)) == 
      CSDRAW_2DGRAPHICS)
    {
      glCullFace (GL_FRONT);
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
      if (!tex_data)
      {
        // Make sure the texture is in the cache before updating it.
	txtcache->Cache (render_target);
        tex_data = (csTxtCacheData*)render_target->GetCacheData();
      }
      //statecache->SetTexture (GL_TEXTURE_2D, tex_data->Handle);
      // Texture is in tha cache, update texture directly.
      ActivateTexture (render_target);
      /*
        Texture has a keycolor - so we need to deal specially with it
	to make sure the keycolor gets transparent.
       */
      if (tex_mm->GetKeyColor ())
      {
	tex_mm->was_render_target = true;
	csRef<iImageVector>& images = tex_mm->GetImages();
	if (!images.IsValid()) images.AttachNew (new csImageVector ());
	csRef<iImage> image;
	if (images->Length() > 0)
	  image = images->GetImage (0);
	else
	  images->AddImage (image = 0);
	if (image == 0) // @@@ How to deal with cubemaps?
	{
	  image.AttachNew (new csImageMemory (
	    txt_w, txt_h, CS_IMGFMT_TRUECOLOR | CS_IMGFMT_ALPHA));
	  images->SetImage (0, image);
	}

	void* imgdata = image->GetImageData ();
	glReadPixels (1, viewheight - txt_h, txt_w, txt_h, GL_RGBA, GL_UNSIGNED_BYTE, 
	  imgdata);

	/*
	  @@@ Optimize a bit. E.g. the texture shouldn't be uncached and cached again
	  every time.
	 */
	tex_mm->UpdateTexture ();
	//tex_mm->InitTexture (txtmgr, G2D->GetPixelFormat ());
	tex_mm->Unprepare ();
	tex_mm->Prepare();
	txtcache->Cache (tex_mm);

      }
      else
      {
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
        glCopyTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, 0, viewheight-txt_h,
          txt_w, txt_h, 0);
      }
    }
  }
  render_target = 0;
  current_drawflags = 0;
}

void csGLGraphics3D::Print (csRect const* area)
{
  //glFinish ();
  if (bugplug)
    bugplug->ResetCounter ("Triangle Count");
  G2D->Print (area);
}

const csReversibleTransform& csGLGraphics3D::GetObjectToCamera()
{
  return object2camera;
}

void csGLGraphics3D::SetObjectToCameraInternal (
	const csReversibleTransform& object2cam)
{
  const csMatrix3 &orientation1 = object2camera.GetO2T();
  const csVector3 &translation1 = object2camera.GetO2TTranslation();
  const csMatrix3 &orientation2 = object2cam.GetO2T();
  const csVector3 &translation2 = object2cam.GetO2TTranslation();
  if (translation1 == translation2 &&
      orientation1.Col1 () == orientation2.Col1 () &&
      orientation1.Col2 () == orientation2.Col2 () &&
      orientation1.Col3 () == orientation2.Col3 ())
    return;
  object2camera = object2cam;
  ApplyObjectToCamera ();
}

void csGLGraphics3D::DrawLine (const csVector3 & v1, const csVector3 & v2,
	float fov, int color)
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

bool csGLGraphics3D::ActivateBuffer (csVertexAttrib attrib,
	iRenderBuffer* buffer)
{
  bool bind = true;
  int att;
  if (attrib < 100)
  {
    if (vertattrib[attrib] == buffer)
    {
      if (vertattribenabled[attrib])
        return true;
      //bind = false; //@@@[res]: causes graphical errors in some cases
    }
    att = attrib;
  }
  else
  {
    att = attrib-100;
    if (vertattrib[att] == buffer)
    {
      if (vertattribenabled100[att])
        return true;
      //bind = false; //@@@[res]: causes graphical errors in some cases
    }
  }
  if (bind && vertattrib[att])
  {
    vertattrib[att]->Release ();
    vertattrib[att] = 0;
  }

  void* data = ((csGLRenderBuffer*)buffer)->RenderLock (
  	CS_GLBUF_RENDERLOCK_ARRAY); //buffer->Lock (CS_BUF_LOCK_RENDER);
  int stride = buffer->GetStride ();
  if (data != (void*)-1)
  {
    if (ext->glEnableVertexAttribArrayARB && attrib < CS_VATTRIB_SPECIFIC_FIRST)
    {
      ext->glEnableVertexAttribArrayARB (att);
      if (bind)
      {
        ext->glVertexAttribPointerARB(attrib, buffer->GetComponentCount (),
          ((csGLRenderBuffer*)buffer)->compGLType, true, stride, data);
      }
    }
    else
    {
      switch (attrib)
      {
        case CS_VATTRIB_POSITION:
          glVertexPointer (buffer->GetComponentCount (),
            ((csGLRenderBuffer*)buffer)->compGLType, stride, data);
          glEnableClientState (GL_VERTEX_ARRAY);
          break;
        case CS_VATTRIB_NORMAL:
          glNormalPointer (((csGLRenderBuffer*)buffer)->compGLType, stride, data);
          glEnableClientState (GL_NORMAL_ARRAY);
          break;
        case CS_VATTRIB_PRIMARY_COLOR:
          glColorPointer (buffer->GetComponentCount (),
            ((csGLRenderBuffer*)buffer)->compGLType, stride, data);
          glEnableClientState (GL_COLOR_ARRAY);
	  break;
        default:
	  if ((attrib >= CS_VATTRIB_TEXCOORD0) && 
	    (attrib <= CS_VATTRIB_TEXCOORD7))
	  {
	    int unit = attrib - CS_VATTRIB_TEXCOORD0;
	    if (ext->CS_GL_ARB_multitexture)
	    {
	      statecache->SetActiveTU (unit);
	    }
	    else if (unit != 0) return false;
	    glTexCoordPointer (buffer->GetComponentCount (), 
	      ((csGLRenderBuffer*)buffer)->compGLType, stride, data);
	    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
	  }
	  break;
      }
    }
    vertattrib[att] = buffer;
    if (attrib < CS_VATTRIB_SPECIFIC_FIRST)
      vertattribenabled[att] = true;
    else
      vertattribenabled100[att] = true;
  }
  return true;
}

void csGLGraphics3D::DeactivateBuffer (csVertexAttrib attrib)
{
  int att;
  if (attrib < CS_VATTRIB_SPECIFIC_FIRST)
  {
    if (vertattribenabled[attrib] == false) return;
    att = attrib;
  }
  else
  {
    att = attrib - CS_VATTRIB_SPECIFIC_FIRST;
    if (vertattribenabled100[att] == false) return;
  }
  if (ext->glDisableVertexAttribArrayARB && attrib<100) 
  {
    ext->glDisableVertexAttribArrayARB (attrib);
    /*if (use_hw_render_buffers)
      ext->glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0);

    ext->glVertexAttribPointerARB(attrib, 1, GL_FLOAT, true, 0, 0);*/
  }
  else
  {
    switch (attrib)
    {
      case CS_VATTRIB_POSITION:
        glDisableClientState (GL_VERTEX_ARRAY);
        break;
      case CS_VATTRIB_NORMAL:
        glDisableClientState (GL_NORMAL_ARRAY);
        break;
      case CS_VATTRIB_PRIMARY_COLOR:
        glDisableClientState (GL_COLOR_ARRAY);
        break;
      default:
	if ((attrib >= CS_VATTRIB_TEXCOORD0) && 
	  (attrib <= CS_VATTRIB_TEXCOORD7))
	{
	  int unit = attrib - CS_VATTRIB_TEXCOORD0;
	  if (ext->CS_GL_ARB_multitexture)
	  {
	    statecache->SetActiveTU (unit);
	  }
	  else if (unit != 0) break;
	  glDisableClientState (GL_TEXTURE_COORD_ARRAY);
	}
	break;
    }
  }
  if (vertattrib[att])
  {
    vertattrib[att]->Release ();
    vertattrib[att] = 0;
    if (attrib < CS_VATTRIB_SPECIFIC_FIRST)
      vertattribenabled[att] = false;
    else
      vertattribenabled100[att] = false;
  }
}

void csGLGraphics3D::SetBufferState (csVertexAttrib* attribs,
	iRenderBuffer** buffers, int count)
{
  int i;
  for (i = 0 ; i < count ; i++)
  {
    csVertexAttrib attrib = attribs[i];
    iRenderBuffer* buf = buffers[i];
    if (buf)
      ActivateBuffer (attrib, buf);
    else
      DeactivateBuffer (attrib);
  }
}

bool csGLGraphics3D::ActivateTexture (iTextureHandle *txthandle, int unit)
{
  /*bool bind = true;
  if (texunit[unit] == txthandle)
  {
    if (texunitenabled[unit])
      return true;
    // @@@ Doesn't bind sometimes when it should if the following line
    // is uncommented.
    //bind = false;
  }

  if (bind && texunit[unit])
    DeactivateTexture (unit);*/

  if (ext->CS_GL_ARB_multitexture)
  {
    statecache->SetActiveTU (unit);
  }
  else if (unit != 0) return false;

  csGLTextureHandle *gltxthandle = (csGLTextureHandle *)
    txthandle->GetPrivateObject ();
  GLuint texHandle = gltxthandle->GetHandle ();

  switch (gltxthandle->target)
  {
    case iTextureHandle::CS_TEX_IMG_1D:
      statecache->Enable_GL_TEXTURE_1D ();
      statecache->SetTexture (GL_TEXTURE_1D, texHandle);
      break;
    case iTextureHandle::CS_TEX_IMG_2D:
      statecache->Enable_GL_TEXTURE_2D ();
      statecache->SetTexture (GL_TEXTURE_2D, texHandle);
      break;
    case iTextureHandle::CS_TEX_IMG_3D:
      statecache->Enable_GL_TEXTURE_3D ();
      statecache->SetTexture (GL_TEXTURE_3D, texHandle);
      break;
    case iTextureHandle::CS_TEX_IMG_CUBEMAP:
      statecache->Enable_GL_TEXTURE_CUBE_MAP ();
      statecache->SetTexture (GL_TEXTURE_CUBE_MAP, texHandle);
      break;
    default:
      DeactivateTexture (unit);
      return false;
  }
  texunitenabled[unit] = true;
  texunittarget[unit] = gltxthandle->target;
  return true;
}

void csGLGraphics3D::DeactivateTexture (int unit)
{
  if (!texunitenabled[unit])
    return;

  if (ext->CS_GL_ARB_multitexture)
  {
    statecache->SetActiveTU (unit);
  }
  else if (unit != 0) return;

  switch (texunittarget[unit])
  {
    case iTextureHandle::CS_TEX_IMG_1D:
      statecache->Disable_GL_TEXTURE_1D ();
      break;
    case iTextureHandle::CS_TEX_IMG_2D:
      statecache->Disable_GL_TEXTURE_2D ();
      break;
    case iTextureHandle::CS_TEX_IMG_3D:
      statecache->Disable_GL_TEXTURE_3D ();
      break;
    case iTextureHandle::CS_TEX_IMG_CUBEMAP:
      statecache->Disable_GL_TEXTURE_CUBE_MAP ();
      break;
  }
  texunitenabled[unit] = false;
}

void csGLGraphics3D::SetTextureState (int* units, iTextureHandle** textures,
	int count)
{
  int i;
  int unit = 0;
  for (i = 0 ; i < count ; i++)
  {
    unit = units[i];
    iTextureHandle* txt = textures[i];
    if (txt)
      ActivateTexture (txt, unit);
    else
      DeactivateTexture (unit);
  }
}

void csGLGraphics3D::DrawMesh (csRenderMesh* mymesh,
  const csShaderVarStack &stacks)
{
  SetupProjection ();

  SetupClipPortals ();
  int num_tri = (mymesh->indexend-mymesh->indexstart)/3;
  SetupClipper (mymesh->clip_portal, 
                mymesh->clip_plane, 
                mymesh->clip_z_plane,
		num_tri);

  SetObjectToCameraInternal (mymesh->object2camera);
  
  CS_ASSERT (string_indices<(csStringID)stacks.Length ()
      && stacks[string_indices].Length () > 0);
  csShaderVariable* indexBufSV = stacks[string_indices].Top ();
  iRenderBuffer* iIndexbuf = 0;
  indexBufSV->GetValue (iIndexbuf);
  CS_ASSERT(iIndexbuf);
  csGLRenderBuffer* indexbuf = (csGLRenderBuffer*)iIndexbuf;
  
  const size_t indexCompsBytes = indexbuf->compcount * indexbuf->compSize;
  CS_ASSERT((indexCompsBytes * mymesh->indexstart) <= indexbuf->size);
  CS_ASSERT((indexCompsBytes * mymesh->indexend) <= indexbuf->size);

  GLenum primitivetype = GL_TRIANGLES;
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
    case CS_MESHTYPE_POINT_SPRITES:
    {
      if(!(ext->CS_GL_ARB_point_sprite && ext->CS_GL_ARB_point_parameters))
      {
        break;
      }
      float radius, scale;
      CS_ASSERT (string_point_radius<(csStringID)stacks.Length ()
          && stacks[string_point_radius].Length () > 0);
      csShaderVariable* radiusSV = stacks[string_point_radius].Top ();
      radiusSV->GetValue (radius);

      CS_ASSERT (string_point_scale < (csStringID)stacks.Length ()
          && stacks[string_point_scale].Length () > 0);
      csShaderVariable* scaleSV = stacks[string_point_scale].Top ();
      scaleSV->GetValue (scale);

      glPointSize (1.0f);
      GLfloat atten[3] = {0.0f, 0.0f, scale * scale};
      ext->glPointParameterfvARB (GL_POINT_DISTANCE_ATTENUATION_ARB, atten);
      ext->glPointParameterfARB (GL_POINT_SIZE_MAX_ARB, 9999.0f);
      ext->glPointParameterfARB (GL_POINT_SIZE_MIN_ARB, 0.0f);
      ext->glPointParameterfARB (GL_POINT_FADE_THRESHOLD_SIZE_ARB, 1.0f);

      glEnable (GL_POINT_SPRITE_ARB);
      primitivetype = GL_POINTS;
      statecache->SetActiveTU (0);
      glTexEnvi (GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

      break;
    }
    case CS_MESHTYPE_LINES:
      primitivetype = GL_LINES;
      break;
    case CS_MESHTYPE_LINESTRIP:
      primitivetype = GL_LINE_STRIP;
      break;
    case CS_MESHTYPE_POLYGON:
    case CS_MESHTYPE_TRIANGLES:
    default:
      primitivetype = GL_TRIANGLES;
      break;
  }

  switch (current_shadow_state)
  {
    case CS_SHADOW_VOLUME_PASS1:
      statecache->SetStencilOp (GL_KEEP, GL_KEEP, GL_INCR);
      statecache->SetStencilFunc (GL_ALWAYS, 0,
        (clipping_stencil_enabled?128:0));
      break;
    case CS_SHADOW_VOLUME_FAIL1:
      statecache->SetStencilOp (GL_KEEP, GL_INCR, GL_KEEP);
      statecache->SetStencilFunc (GL_ALWAYS, 0,
        (clipping_stencil_enabled?128:0));
      break;
    case CS_SHADOW_VOLUME_PASS2:
      statecache->SetStencilOp (GL_KEEP, GL_KEEP, GL_DECR);
      statecache->SetStencilFunc (GL_ALWAYS, 0,
        (clipping_stencil_enabled?128:0));
      break;
    case CS_SHADOW_VOLUME_FAIL2:
      statecache->SetStencilOp (GL_KEEP, GL_DECR, GL_KEEP);
      statecache->SetStencilFunc (GL_ALWAYS, 0,
        (clipping_stencil_enabled?128:0));
      break;
    case CS_SHADOW_VOLUME_USE:
      statecache->SetStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
      statecache->SetStencilFunc (GL_EQUAL, 0, 127 |
        (clipping_stencil_enabled?128:0));
      break;
    default:
      if (clipping_stencil_enabled)
      {
        statecache->SetStencilFunc (GL_EQUAL, 0, 128);
        statecache->SetStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
      }
  }

  if (current_shadow_state == CS_SHADOW_VOLUME_PASS2 ||
      current_shadow_state == CS_SHADOW_VOLUME_FAIL1)
    SetMirrorMode (!mymesh->do_mirror);
  else
    SetMirrorMode (mymesh->do_mirror);


  const uint mixmode = mymesh->mixmode;
  statecache->SetShadeModel ((mixmode & CS_FX_FLAT) ? GL_FLAT : GL_SMOOTH);
  statecache->SetShadeModel (GL_SMOOTH);

  void* bufData = indexbuf->RenderLock (CS_GLBUF_RENDERLOCK_ELEMENTS);
  if (bufData != (void*)-1)
  {
    if ((mixmode & CS_FX_MASK_MIXMODE) != CS_FX_COPY)
      SetMixMode (mixmode);
    else
      SetAlphaType (mymesh->alphaType);

    if (bugplug)
    {
      bugplug->AddCounter ("Triangle Count", num_tri);
      bugplug->AddCounter ("Mesh Count", 1);
    }

    if ((current_zmode == CS_ZBUF_MESH) || (current_zmode == CS_ZBUF_MESH2))
    {
      CS_ASSERT ((mymesh->z_buf_mode != CS_ZBUF_MESH) &&
	(mymesh->z_buf_mode != CS_ZBUF_MESH2));
      SetZMode ((current_zmode == CS_ZBUF_MESH2) ? 
	GetZModePass2 (mymesh->z_buf_mode) : mymesh->z_buf_mode, true);
/*      if (mymesh->z_buf_mode != CS_ZBUF_MESH)
      {
        SetZMode (mymesh->z_buf_mode, true);
      } else {
        // Should do some real reporting here. :)
        csPrintf ("Bwaaaah! Meshes can't have zmesh zmode. You deserve some spanking!\n");
        return;
      }*/
    }

    float alpha = 1.0f;
    if ((mixmode & CS_FX_MASK_MIXMODE) == CS_FX_ALPHA)
      alpha = (float)(mixmode & CS_FX_MASK_ALPHA) / 255.0f;
    glColor4f (1.0f, 1.0f, 1.0f, alpha);
    glDrawElements (
      primitivetype,
      mymesh->indexend - mymesh->indexstart,
      indexbuf->compGLType,
      ((uint8*)bufData) +
	(indexCompsBytes * mymesh->indexstart));

    indexbuf->Release();
  }

  if (mymesh->meshtype == CS_MESHTYPE_POINT_SPRITES) 
  {
    glTexEnvi (GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_FALSE);
    glDisable(GL_POINT_SPRITE_ARB);
  }

  SetMirrorMode (false);
}

void csGLGraphics3D::DrawPixmap (iTextureHandle *hTex,
  int sx, int sy, int sw, int sh, 
  int tx, int ty, int tw, int th, uint8 Alpha)
{
  // If original dimensions are different from current dimensions (because
  // image has been scaled to conform to OpenGL texture size restrictions)
  // we correct the input coordinates here.
  int bitmapwidth = 0, bitmapheight = 0;
  hTex->GetMipMapDimensions (0, bitmapwidth, bitmapheight);
  csGLTextureHandle *txt_mm = (csGLTextureHandle *)
    hTex->GetPrivateObject ();
  int owidth = txt_mm->orig_width;
  int oheight = txt_mm->orig_height;
  if (owidth != bitmapwidth || oheight != bitmapheight)
  {
    tx = (int)(tx * (float)bitmapwidth  / (float)owidth );
    ty = (int)(ty * (float)bitmapheight / (float)oheight);
    tw = (int)(tw * (float)bitmapwidth  / (float)owidth );
    th = (int)(th * (float)bitmapheight / (float)oheight);
  }

  // cache the texture if we haven't already.
  txtcache->Cache (hTex);

  // as we are drawing in 2D, we disable some of the commonly used features
  // for fancy 3D drawing
  statecache->SetShadeModel (GL_FLAT);
  SetZMode (CS_ZBUF_NONE);
  //@@@???statecache->SetDepthMask (GL_FALSE);

  // if the texture has transparent bits, we have to tweak the
  // OpenGL blend mode so that it handles the transparent pixels correctly
  if (hTex->GetKeyColor () || hTex->GetAlphaMap () || Alpha)
    SetMixMode (CS_FX_ALPHA);
  else
    SetMixMode (CS_FX_COPY);

  statecache->Enable_GL_TEXTURE_2D ();
  glColor4f (1.0, 1.0, 1.0, Alpha ? (1.0 - BYTE_TO_FLOAT (Alpha)) : 1.0);
  ActivateTexture (hTex);

  // convert texture coords given above to normalized (0-1.0) texture
  // coordinates
  float ntx1,nty1,ntx2,nty2;
  ntx1 = ((float)tx            ) / bitmapwidth;
  ntx2 = ((float)tx + (float)tw) / bitmapwidth;
  nty1 = ((float)ty            ) / bitmapheight;
  nty2 = ((float)ty + (float)th) / bitmapheight;

  // draw the bitmap
  glBegin (GL_QUADS);
  //    glTexCoord2f (ntx1, nty1);
  //    glVertex2i (sx, height - sy - 1);
  //    glTexCoord2f (ntx2, nty1);
  //    glVertex2i (sx + sw, height - sy - 1);
  //    glTexCoord2f (ntx2, nty2);
  //    glVertex2i (sx + sw, height - sy - sh - 1);
  //    glTexCoord2f (ntx1, nty2);
  //    glVertex2i (sx, height - sy - sh - 1);

  // smgh: This works in software opengl and with cswstest
  // wouter: removed height-sy-1 to be height-sy.
  //    this is because on opengl y=0.0 is off screen, as is y=height.
  //    using height-sy gives output on screen which is identical to 
  //    using the software canvas.
  glTexCoord2f (ntx1, nty1);
  glVertex2i (sx, viewheight - sy);
  glTexCoord2f (ntx2, nty1);
  glVertex2i (sx + sw, viewheight - sy);
  glTexCoord2f (ntx2, nty2);
  glVertex2i (sx + sw, viewheight - (sy + sh));
  glTexCoord2f (ntx1, nty2);
  glVertex2i (sx, viewheight - (sy + sh));
  glEnd ();
}

void csGLGraphics3D::SetShadowState (int state)
{
  switch (state)
  {
    case CS_SHADOW_VOLUME_BEGIN:
      current_shadow_state = CS_SHADOW_VOLUME_BEGIN;
      stencil_initialized = false;
      glClearStencil (0);
      glClear (GL_STENCIL_BUFFER_BIT);
      EnableStencilShadow ();
      //statecache->SetStencilFunc (GL_ALWAYS, 0, 127);
      //statecache->SetStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
      // @@@ Jorrit: to avoid flickering I had to increase the
      // values below and multiply them with 3.
      //glPolygonOffset (-0.1f, -4.0f); 
      glPolygonOffset (-0.3f, -12.0f); 
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
      DisableStencilShadow ();
      break;
  }
}

void csGLGraphics3D::OpenPortal (size_t numVertices, 
				 const csVector2* vertices,
				 const csPlane3& normal)
{
  csClipPortal* cp = new csClipPortal ();
  cp->poly = new csVector2[numVertices];
  memcpy (cp->poly, vertices, numVertices * sizeof (csVector2));
  cp->num_poly = numVertices;
  cp->normal = normal;
  clipportal_stack.Push (cp);
  clipportal_dirty = true;
}

void csGLGraphics3D::ClosePortal ()
{
  if (clipportal_stack.Length () <= 0) return;
  csClipPortal* cp = clipportal_stack.Pop ();
  delete cp;
  clipportal_dirty = true;
}

void csGLGraphics3D::SetupClipPortals ()
{
  if (clipportal_dirty)
  {
    clipportal_dirty = false;
    //if (GLCaps.use_stencil)
    {
      if (clipportal_stack.Length () <= 0)
      {
        statecache->Disable_GL_STENCIL_TEST ();
      }
      else
      {
        csClipPortal* cp = clipportal_stack.Top ();

        // First set up the stencil area.
        statecache->Enable_GL_STENCIL_TEST ();
        glClearStencil (0);
        glClear (GL_STENCIL_BUFFER_BIT);
        statecache->SetStencilFunc (GL_ALWAYS, 1, 1);
        statecache->SetStencilOp (GL_KEEP, GL_ZERO, GL_REPLACE);

	CS_ALLOC_STACK_ARRAY(csVector3, vertices3, cp->num_poly);
	CS_ALLOC_STACK_ARRAY(uint, indices, cp->num_poly);
	for (int v = 0; v < cp->num_poly; v++)
	{
	  vertices3[v].Set (cp->poly[v].x, cp->poly[v].y, 0.0f);
	  indices[v] = v;
	}
	csSimpleRenderMesh simpleRM;
	simpleRM.meshtype = CS_MESHTYPE_TRIANGLEFAN;
	simpleRM.indexCount = cp->num_poly;
	simpleRM.indices = indices;
	simpleRM.vertexCount = cp->num_poly;
	simpleRM.vertices = vertices3;
	simpleRM.texcoords = 0;
        // USE OR FILL@@@?
	simpleRM.z_buf_mode = CS_ZBUF_USE;

	DrawSimpleMesh (simpleRM);

	//DrawPolygonZFill (cp->poly, cp->num_poly, cp->normal);

        // Use the stencil area.
        statecache->SetStencilFunc (GL_EQUAL, 1, 1);
        statecache->SetStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);

	// First clear the z-buffer here.
	SetZMode (CS_ZBUF_FILLONLY);
	glColorMask (false, false, false, false);

	glBegin (GL_QUADS);
	glVertex4f (0.0*100000.0, 0.0*100000.0, -1.0, 100000.0);
	glVertex4f (float (viewwidth-1)*100000.0, 0.0*100000.0, -1.0, 
	  100000.0);
	glVertex4f (float (viewwidth-1)*100000.0, 
	  float (viewheight-1)*100000.0, -1.0, 100000.0);
	glVertex4f (0.0*100000.0, float (viewheight-1)*100000.0, -1.0, 
	  100000.0);
	glEnd ();
	glColorMask (color_red_enabled, color_green_enabled, 
	  color_blue_enabled, alpha_enabled);
      }
    }
  }
}

void csGLGraphics3D::SetClipper (iClipper2D* clipper, int cliptype)
{
  //clipper = new csBoxClipper (10, 10, 200, 200);
  csGLGraphics3D::clipper = clipper;
  if (!clipper) cliptype = CS_CLIPPER_NONE;
  csGLGraphics3D::cliptype = cliptype;
  stencil_initialized = false;
  frustum_valid = false;
  int i;
  for (i = 0; i<6; i++)
    glDisable ((GLenum)(GL_CLIP_PLANE0+i));
  DisableStencilClipping ();
  cache_clip_portal = -1;
  cache_clip_plane = -1;
  cache_clip_z_plane = -1;
  /*if (cliptype != CS_CLIPPER_NONE)
  {
    csVector2 *clippoly = clipper->GetClipPoly ();
    csBox2 scissorbox;
    scissorbox.AddBoundingVertex (clippoly[0]);
    for (i=1; i<clipper->GetVertexCount (); i++)
      scissorbox.AddBoundingVertexSmart (clippoly[i]);

    glScissor (scissorbox.MinX (), scissorbox.MinY (), 
      scissorbox.MaxX ()-scissorbox.MinX (),
      scissorbox.MaxY ()-scissorbox.MinY ());
    glEnable (GL_SCISSOR_TEST);
  } else {
    glDisable (GL_SCISSOR_TEST);
  }*/
}

// @@@ doesn't serve any purpose for now, but might in the future.
// left in for now.
bool csGLGraphics3D::SetRenderState (G3D_RENDERSTATEOPTION op, long val)
{
  return false;
}

long csGLGraphics3D::GetRenderState (G3D_RENDERSTATEOPTION op) const
{
  return 0;
}

csPtr<iPolygonRenderer> csGLGraphics3D::CreatePolygonRenderer ()
{
  return csPtr<iPolygonRenderer> (new csGLPolygonRenderer (this));
}

void csGLGraphics3D::DrawSimpleMesh (const csSimpleRenderMesh& mesh)
{
  if (scrapIndicesSize < mesh.indexCount)
  {
    scrapIndices = CreateRenderBuffer (mesh.indexCount * sizeof (uint),
      CS_BUF_DYNAMIC, CS_BUFCOMP_UNSIGNED_INT, 1, true);
    scrapIndicesSize = mesh.indexCount;
  }
  if (scrapVerticesSize < mesh.vertexCount)
  {
    scrapVertices = CreateRenderBuffer (
      mesh.vertexCount * sizeof (float) * 3,
      CS_BUF_DYNAMIC, CS_BUFCOMP_FLOAT, 3, false);
    scrapTexcoords = CreateRenderBuffer (
      mesh.vertexCount * sizeof (float) * 2,
      CS_BUF_DYNAMIC, CS_BUFCOMP_FLOAT, 2, false);
    scrapColors = CreateRenderBuffer (
      mesh.vertexCount * sizeof (float) * 4,
      CS_BUF_DYNAMIC, CS_BUFCOMP_FLOAT, 4, false);

    scrapVerticesSize = mesh.vertexCount;
  }

  bool useShader = (mesh.shader != 0);

  csShaderVariable* sv;
  sv = scrapContext.GetVariableAdd (string_indices);
  if (mesh.indices)
  {
    scrapIndices->CopyToBuffer (mesh.indices, 
      mesh.indexCount * sizeof (uint));
    sv->SetValue (scrapIndices);
  }
  else
  {
    sv->SetValue (0);
  }
  sv = scrapContext.GetVariableAdd (string_vertices);
  if (mesh.vertices)
  {
    scrapVertices->CopyToBuffer (mesh.vertices, 
      mesh.vertexCount * sizeof (csVector3));
    if (useShader)
      sv->SetValue (scrapVertices);
    else
      ActivateBuffer (CS_VATTRIB_POSITION, scrapVertices);
  }
  else
  {
    if (useShader)
      sv->SetValue (0);
    else
      DeactivateBuffer (CS_VATTRIB_POSITION);
  }
  sv = scrapContext.GetVariableAdd (string_texture_coordinates);
  if (mesh.texcoords)
  {
    scrapTexcoords->CopyToBuffer (mesh.texcoords, 
      mesh.vertexCount * sizeof (csVector2));
    if (useShader)
      sv->SetValue (scrapTexcoords);
    else
      ActivateBuffer (CS_VATTRIB_TEXCOORD, scrapTexcoords);
  }
  else
  {
    if (useShader)
      sv->SetValue (0);
    else
      DeactivateBuffer (CS_VATTRIB_TEXCOORD);
  }
  sv = scrapContext.GetVariableAdd (string_colors);
  if (mesh.colors)
  {
    scrapColors->CopyToBuffer (mesh.colors, 
      mesh.vertexCount * sizeof (csVector4));
    if (useShader)
      sv->SetValue (scrapColors);
    else
      ActivateBuffer (CS_VATTRIB_COLOR, scrapColors);
  }
  else
  {
    if (useShader)
      sv->SetValue (0);
    else
      DeactivateBuffer (CS_VATTRIB_COLOR);
  }
  if (useShader)
  {
    sv = scrapContext.GetVariableAdd (string_texture_diffuse);
    sv->SetValue (mesh.texture);
  }
  else
  {
    if (ext->CS_GL_ARB_multitexture)
      statecache->SetActiveTU (0);
    if (mesh.texture)
      ActivateTexture (mesh.texture);
    else
      DeactivateTexture ();
  }

  int passCount = 1;
  if (mesh.shader != 0)
  {
    passCount = mesh.shader->GetNumberOfPasses ();
  }
  
  csRenderMesh rmesh;
  //rmesh.z_buf_mode = mesh.z_buf_mode;
  rmesh.mixmode = mesh.mixmode;
  rmesh.clip_portal = 0;
  rmesh.clip_plane = 0;
  rmesh.clip_z_plane = 0;
  rmesh.do_mirror = false;
  rmesh.meshtype = mesh.meshtype;
  rmesh.indexstart = 0;
  rmesh.indexend = mesh.indexCount;
  rmesh.variablecontext = &scrapContext;

  csShaderVarStack stacks;
  shadermgr->PushVariables (stacks);
  scrapContext.PushVariables (stacks);

  if (mesh.alphaType.autoAlphaMode)
  {
    csAlphaMode::AlphaType autoMode = csAlphaMode::alphaNone;

    iTextureHandle* tex = 0;
    if (mesh.alphaType.autoModeTexture != csInvalidStringID
        && mesh.alphaType.autoModeTexture < (csStringID)stacks.Length ()
        && stacks[mesh.alphaType.autoModeTexture].Length () > 0)
    {
      csShaderVariable* texVar = 
        stacks[mesh.alphaType.autoModeTexture].Top ();
      if (texVar)
	texVar->GetValue (tex);
    }
    if (tex == 0)
      tex = mesh.texture;
    if (tex != 0)
      autoMode = tex->GetAlphaType ();

    rmesh.alphaType = autoMode;
  }
  else
  {
    rmesh.alphaType = mesh.alphaType.alphaType;
  }

  SetZMode (mesh.z_buf_mode);
  for (int p = 0; p < passCount; p++)
  {
    if (mesh.shader != 0)
    {
      mesh.shader->ActivatePass (p);
      mesh.shader->SetupPass (&rmesh, stacks);
    }
    DrawMesh (&rmesh, stacks);
    if (mesh.shader != 0)
    {
      mesh.shader->TeardownPass ();
      mesh.shader->DeactivatePass ();
    }
  }

  if (!useShader)
  {
    DeactivateBuffer (CS_VATTRIB_POSITION);
    DeactivateBuffer (CS_VATTRIB_TEXCOORD);
    DeactivateBuffer (CS_VATTRIB_COLOR);
    if (mesh.texture)
      DeactivateTexture ();
  }
}

class csOpenGLHalo : public iHalo
{
  /// The halo color
  float R, G, B;
  /// The width and height
  int Width, Height;
  /// Width and height factor
  float Wfact, Hfact;
  /// Blending method
  uint dstblend;
  /// Our OpenGL texture handle
  GLuint halohandle;
  /// The OpenGL 3D driver
  csGLGraphics3D* G3D;

public:
  SCF_DECLARE_IBASE;

  csOpenGLHalo (float iR, float iG, float iB, unsigned char *iAlpha,
    int iWidth, int iHeight, csGLGraphics3D* iG3D);

  virtual ~csOpenGLHalo ();

  virtual int GetWidth () { return Width; }
  virtual int GetHeight () { return Height; }

  virtual void SetColor (float &iR, float &iG, float &iB)
  { R = iR; G = iG; B = iB; }

  virtual void GetColor (float &oR, float &oG, float &oB)
  { oR = R; oG = G; oB = B; }

  virtual void Draw (float x, float y, float w, float h, float iIntensity,
    csVector2 *iVertices, int iVertCount);
};

SCF_IMPLEMENT_IBASE (csOpenGLHalo)
  SCF_IMPLEMENTS_INTERFACE (iHalo)
SCF_IMPLEMENT_IBASE_END

csOpenGLHalo::csOpenGLHalo (float iR, float iG, float iB, unsigned char *iAlpha,
  int iWidth, int iHeight, csGLGraphics3D* iG3D)
{
  SCF_CONSTRUCT_IBASE (0);

  // Initialization
  R = iR; G = iG; B = iB;
  // OpenGL can only use 2^n sized textures
  Width = csFindNearestPowerOf2 (iWidth);
  Height = csFindNearestPowerOf2 (iHeight);

  uint8 *Alpha = iAlpha;
  if ((Width != iWidth) || (Height != iHeight))
  {
    // Allocate our copy of the scanline which is power-of-two
    Alpha = new uint8 [Width * Height];
    int i;
    for (i = 0; i < iHeight; i++)
    {
      // Copy a scanline from the supplied alphamap
      memcpy (Alpha + (i * Width), iAlpha + (i * iWidth), iWidth);
      // Clear the tail of the scanline
      memset (Alpha + (i * Width) + iWidth, 0, Width - iWidth);
    }
    memset (Alpha + iHeight * Width, 0, (Height - iHeight) * Width);
  }

  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
  // Create handle
  glGenTextures (1, &halohandle);
  // Activate handle
  csGLGraphics3D::statecache->SetTexture (GL_TEXTURE_2D, halohandle);

  // Jaddajaddajadda
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D (GL_TEXTURE_2D, 0, GL_ALPHA, Width, Height, 0, GL_ALPHA,
    GL_UNSIGNED_BYTE, Alpha);

  if (Alpha != iAlpha)
    delete [] Alpha;
  (G3D = iG3D)->IncRef ();

  Wfact = float (iWidth) / Width;
  Hfact = float (iHeight) / Height;

  Width = iWidth;
  Height = iHeight;

  if (R > 1.0 || G > 1.0 || B > 1.0)
  {
    dstblend = CS_FX_SRCALPHAADD;
    R /= 2; G /= 2; B /= 2;
  }
  else
    dstblend = CS_FX_ALPHA;
}

csOpenGLHalo::~csOpenGLHalo ()
{
  // Kill, crush and destroy
  // Delete generated OpenGL handle
  glDeleteTextures (1, &halohandle);
  G3D->DecRef ();
  SCF_DESTRUCT_IBASE();
}

// Draw the halo. Wasn't that a suprise
void csOpenGLHalo::Draw (float x, float y, float w, float h, float iIntensity,
  csVector2 *iVertices, int iVertCount)
{
  int swidth = G3D->GetWidth ();
  int sheight = G3D->GetHeight ();
  int i;

  if (w < 0) w = Width;
  if (h < 0) h = Height;

  csVector2 HaloPoly [4];
  if (!iVertices)
  {
    iVertCount = 4;
    iVertices = HaloPoly;

    float x1 = x, y1 = y, x2 = x + w, y2 = y + h;
    if (x1 < 0) x1 = 0; if (x2 > swidth ) x2 = swidth ;
    if (y1 < 0) y1 = 0; if (y2 > sheight) y2 = sheight;
    if ((x1 >= x2) || (y1 >= y2))
      return;

    HaloPoly [0].Set (x1, y1);
    HaloPoly [1].Set (x1, y2);
    HaloPoly [2].Set (x2, y2);
    HaloPoly [3].Set (x2, y1);
  };

  /// The inverse width and height of the halo
  float inv_W = Wfact / w, inv_H = Hfact / h;
  float aspect = (float)G3D->GetWidth() / G3D->GetAspect();
  float hw = (float)G3D->GetWidth() * 0.5f;

  //???@@@glMatrixMode (GL_MODELVIEW);
  glPushMatrix ();
  glLoadIdentity();
  G3D->SetGlOrtho (false);
  //glTranslatef (0, 0, 0);

  //csGLGraphics3D::SetGLZBufferFlags (CS_ZBUF_NONE);
  G3D->SetZMode (CS_ZBUF_NONE);
  csGLGraphics3D::statecache->Enable_GL_TEXTURE_2D ();

  csGLGraphics3D::statecache->SetShadeModel (GL_FLAT);
  csGLGraphics3D::statecache->SetTexture (GL_TEXTURE_2D, halohandle);
  G3D->SetAlphaType (csAlphaMode::alphaSmooth);

  G3D->SetMixMode (dstblend);
  glColor4f (R, G, B, iIntensity);

  glBegin (GL_POLYGON);
  for (i = iVertCount - 1; i >= 0; i--)
  {
    float vx = iVertices [i].x, vy = iVertices [i].y;
    glTexCoord2f ((vx - x) * inv_W, (vy - y) * inv_H);
    glVertex3f ((vx - hw) * aspect + hw, sheight - vy, -14.0f);
  }
  glEnd ();

  glPopMatrix ();
}

iHalo *csGLGraphics3D::CreateHalo (float iR, float iG, float iB,
  unsigned char *iAlpha, int iWidth, int iHeight)
{
  return new csOpenGLHalo (iR, iG, iB, iAlpha, iWidth, iHeight, this);
}

float csGLGraphics3D::GetZBuffValue (int x, int y)
{
  GLfloat zvalue;
  glReadPixels (x, viewheight - y - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, 
    &zvalue);
  if (zvalue < .000001) return 1000000000.;
  // 0.090909=1/11, that is 1 divided by total depth delta set by
  // glOrtho. Where 0.090834 comes from, I don't know
  //return (0.090834 / (zvalue - (0.090909)));
  // @@@ Jorrit: I have absolutely no idea what they are trying to do
  // but changing the above formula to the one below at least appears
  // to give more accurate results.
  return (0.090728 / (zvalue - (0.090909)));
}

////////////////////////////////////////////////////////////////////
// iComponent
////////////////////////////////////////////////////////////////////

bool csGLGraphics3D::Initialize (iObjectRegistry* p)
{
  bool ok = true;
  object_reg = p;

  if (!scfiEventHandler)
    scfiEventHandler = csPtr<EventHandler> (new EventHandler (this));

  csRef<iEventQueue> q = CS_QUERY_REGISTRY(object_reg, iEventQueue);
  if (q)
    q->RegisterListener (scfiEventHandler, CSMASK_Broadcast);

  scfiShaderRenderInterface.Initialize(p);

  bugplug = CS_QUERY_REGISTRY (object_reg, iBugPlug);

  strings = CS_QUERY_REGISTRY_TAG_INTERFACE (
    object_reg, "crystalspace.shared.stringset", iStringSet);

  csRef<iPluginManager> plugin_mgr = CS_QUERY_REGISTRY (
  	object_reg, iPluginManager);
  csRef<iCommandLineParser> cmdline = CS_QUERY_REGISTRY (
  	object_reg, iCommandLineParser);

  config.AddConfig(object_reg, "/config/r3dopengl.cfg");

  const char *driver = cmdline->GetOption ("canvas");
  if (!driver)
    driver = config->GetStr ("Video.OpenGL.Canvas", CS_OPENGL_2D_DRIVER);

  G2D = CS_LOAD_PLUGIN (plugin_mgr, driver, iGraphics2D);
  if (G2D != 0)
    object_reg->Register(G2D, "iGraphics2D");
  else
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "Error loading Graphics2D plugin.");
    ok = false;
  }

  return ok;
}




////////////////////////////////////////////////////////////////////
// iEventHandler
////////////////////////////////////////////////////////////////////




bool csGLGraphics3D::HandleEvent (iEvent& Event)
{
  if (Event.Type == csevBroadcast)
    switch (Event.Command.Code)
    {
      case cscmdSystemOpen:
        Open ();
        return true;
      case cscmdSystemClose:
        Close ();
        return true;
      case cscmdContextResize:
	{
	  int w = G2D->GetWidth ();
	  int h = G2D->GetHeight ();
	  SetDimensions (w, h);
	  asp_center_x = w/2;
	  asp_center_y = h/2;
	}
	return true;
    }
  return false;
}


////////////////////////////////////////////////////////////////////
//                    iShaderRenderInterface
////////////////////////////////////////////////////////////////////

csGLGraphics3D::eiShaderRenderInterface::eiShaderRenderInterface()
{
}

csGLGraphics3D::eiShaderRenderInterface::~eiShaderRenderInterface()
{

}

void* csGLGraphics3D::eiShaderRenderInterface::GetPrivateObject (
	const char* name)
{
  if (strcasecmp(name, "ext") == 0)
    return (void*) (scfParent->ext);
  if (strcasecmp(name, "txtcache") == 0)
    return (void*) (scfParent->txtcache);
  return 0;
}

void csGLGraphics3D::eiShaderRenderInterface::Initialize (iObjectRegistry *reg)
{
  object_reg = reg;
}

////////////////////////////////////////////////////////////////////
//                          iDebugHelper
////////////////////////////////////////////////////////////////////

SCF_IMPLEMENT_EMBEDDED_IBASE (csGLGraphics3D::eiDebugHelper)
  SCF_IMPLEMENTS_INTERFACE (iDebugHelper)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

bool csGLGraphics3D::DebugCommand (const char* cmdstr)
{
  CS_ALLOC_STACK_ARRAY(char, cmd, strlen (cmdstr) + 1);
  strcpy (cmd, cmdstr);
  char* param = 0;
  char* space = strchr (cmdstr, ' ');
  if (space)
  {
    param = space + 1;
    *space = 0;
  }

  if (strcasecmp (cmd, "dump_slms") == 0)
  {
    csRef<iImageIO> imgsaver = CS_QUERY_REGISTRY (object_reg, iImageIO);
    if (!imgsaver)
    {
      Report (CS_REPORTER_SEVERITY_WARNING,
        "Could not get image saver.");
      return false;
    }

    csRef<iVFS> vfs = CS_QUERY_REGISTRY (object_reg, iVFS);
    if (!vfs)
    {
      Report (CS_REPORTER_SEVERITY_WARNING, 
	"Could not get VFS.");
      return false;
    }

    if (txtmgr)
    {
      const char* dir = 
	((param != 0) && (*param != 0)) ? param : "/tmp/slmdump/";
      txtmgr->DumpSuperLightmaps (vfs, imgsaver, dir);
    }

    return true;
  }
  return false;
}

