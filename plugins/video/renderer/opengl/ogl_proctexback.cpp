/*
    Copyright (C) 2000  by Samuel Humphreys
    Based on the Glide implementation by Norman Kr�mer

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
    Software Foundation, Inc.0f, 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include "cssysdef.h"
#include "ogl_txtcache.h"
#include "ogl_txtmgr.h"
#include "ogl_proctexback.h"
#include "ogl_g3dcom.h"
#include "csgeom/polyclip.h"
#include "iutil/event.h"
#include "iutil/eventq.h"
#include "iutil/objreg.h"
#include "ivaria/reporter.h"
#include "video/canvas/openglcommon/glstates.h"

#if defined(CS_OPENGL_PATH)
#include CS_HEADER_GLOBAL(CS_OPENGL_PATH,gl.h)
#else
#include <GL/gl.h>
#endif


csOpenGLProcBackBuffer::csOpenGLProcBackBuffer (iBase *parent) :
  csGraphics3DOGLCommon (parent)
{
  tex_mm = NULL;
  g3d = NULL;
  rstate_bilinearmap = false;
  buffer = NULL;
}

csOpenGLProcBackBuffer::~csOpenGLProcBackBuffer ()
{
  // Our dummy 2d driver will get destroyed lower down in the class hierachy
  // NULL out those members which are shared with the frame buffer interfaces
  // so they don't get deleted.
  vbufmgr = NULL;
  txtmgr = NULL;
  texture_cache = NULL;
  lightmap_cache = NULL;
  m_fogtexturehandle = 0;
  delete [] buffer;
  if (object_reg != 0)
  {
    csRef<iEventQueue> q (CS_QUERY_REGISTRY(object_reg, iEventQueue));
    if (q != 0)
      q->GetEventOutlet ()->Broadcast (cscmdContextClose, (void*)G2D);
  }
}

void csOpenGLProcBackBuffer::Close ()
{
  m_fogtexturehandle = 0;
}

void csOpenGLProcBackBuffer::Prepare (csGraphics3DOGLCommon *g3d,
  csTextureHandleOpenGL *tex_mm, csPixelFormat *ipfmt, bool bpersistent)
{
  memcpy (&pfmt, ipfmt, sizeof(csPixelFormat));
  persistent = bpersistent;
  this->g3d = g3d;
  this->tex_mm = tex_mm;
  object_reg = g3d->object_reg;
  CS_ASSERT (object_reg != NULL);
  g2d = g3d->GetDriver2D ();

  tex_mm->GetMipMapDimensions(0, width, height);

  G2D = new csOpenGLProcBackBuffer2D (g2d, width, height, ipfmt);
  pixel_bytes = g2d->GetPixelBytes ();
  frame_height = g2d->GetHeight ();
  frame_width = g2d->GetWidth ();

  tex_0 = (csTextureProcOpenGL*) tex_mm->vTex[0];

  SharedInitialize (g3d);
  SharedOpen (g3d);

  rstate_bilinearmap = g3d->GetRenderState(
  	G3DRENDERSTATE_BILINEARMAPPINGENABLE);

  // We are going to use an inverted orthographic projection matrix
  inverted = true;

  // Set up a temporary buffer for glReadPixel conversions when the texture is
  // not in the cache.

  buffer = new char[tex_0->size];

  backbuffercopy = new char[width*height*3];
#ifdef CS_DEBUG
  Report (CS_REPORTER_SEVERITY_NOTIFY,
    "%dx%d GL %sbackbuffer procedural texture", 
    width, height, persistent?"persistent ":"");
#endif
}

bool csOpenGLProcBackBuffer::BeginDraw (int DrawFlags)
{
  bool do_quad = false;

  // if 2D graphics is not locked, lock it
  if ((DrawFlags & (CSDRAW_2DGRAPHICS | CSDRAW_3DGRAPHICS))
      && (!(DrawMode & (CSDRAW_2DGRAPHICS | CSDRAW_3DGRAPHICS))))
 {
   if (!G2D->BeginDraw ())
     return false;

   g3d->FlushDrawPolygon ();
   g3d->lightmap_cache->Flush ();

/*    GLenum format, type;
    switch (pixel_bytes)
    {
      case 1:
	format = GL_COLOR_INDEX;
	type = GL_UNSIGNED_BYTE;
	break;
#ifdef GL_VERSION_1_2
      case 2:
	format = GL_RGB;
	type = GL_UNSIGNED_SHORT_5_6_5;
	break;
#endif
      case 4:
	format = GL_RGBA;
	type = GL_UNSIGNED_BYTE;
	break;
      default:
	return false; // invalid format
    }
    glReadPixels (0, 0, width, height,
      format, type, backbuffercopy);  */

    g2d->GetClipRect(oldminx, oldminy, oldmaxx, oldmaxy);
    g2d->SetClipRect(0, 0, frame_width, frame_height);

    glViewport(0,0,width,height);

    glMatrixMode (GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity ();
    // We set this transform to be the size of the procedural texture and to be
    // inverted so that all the action takes place on the lower left hand
    // corner of the screen and upside down.
    glOrtho (0.0f, (GLdouble) width, (GLdouble) height, 0.0f, -1.0f, 10.0f);
    // but this flips front/back of walls...
    statecache->SetCullFace (GL_BACK);

    glMatrixMode (GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity ();
    glColor3f (1.0f, 1.0f, 1.0f);
    glClearColor (0.0f, 0.0f, 0.0f, 0.0f);

    if (persistent)
    {
      // cache the texture if we haven't already.
      texture_cache->Cache (tex_mm);

      // Get texture handle
      GLuint texturehandle = ((csTxtCacheData *)tex_mm->GetCacheData ())
      	->Handle;
      statecache->SetShadeModel (GL_FLAT);
      statecache->EnableState (GL_TEXTURE_2D);
      glColor4f (1.0f,1.0f,1.0f,1.0f);
      statecache->SetTexture (GL_TEXTURE_2D, texturehandle);
      do_quad = true;
      csGraphics3DOGLCommon::SetupBlend (CS_FX_COPY, 0, false);
    }
  }

  if ((DrawFlags & CSDRAW_CLEARSCREEN) && !do_quad)
  {
    statecache->DisableState (GL_TEXTURE_2D);
    glColor3f (0,0,0);
    do_quad = true;
  }

  if (!(DrawFlags & CSDRAW_CLEARZBUFFER))
  {
    if (do_quad)
    {
      csGraphics3DOGLCommon::SetGLZBufferFlags (CS_ZBUF_NONE);
      csGraphics3DOGLCommon::SetupBlend (CS_FX_COPY, 0, false);
    }
  }
  else
  {
    csGraphics3DOGLCommon::SetGLZBufferFlags (CS_ZBUF_FILL);
    if (!do_quad)
    {
      statecache->DisableState (GL_TEXTURE_2D);
      csGraphics3DOGLCommon::SetupBlend (CS_FX_TRANSPARENT, 0, false);
      statecache->SetShadeModel (GL_FLAT);
      glColor4f (0.0f,0.0f,0.0f,0.0f);
    }
    do_quad = true;
  }

  if (do_quad)
  {
    glBegin (GL_QUADS);
    glTexCoord2f (0, 1);
    glVertex3i (0, 0, 0);

    glTexCoord2f (0, 0);
    glVertex3i (0, height, 0);

    glTexCoord2f (1, 0);
    glVertex3i (width, height, 0);

    glTexCoord2f (1, 1);
    glVertex3i (width, 0, 0);
    glEnd ();
  }
  else
  {
    csGraphics3DOGLCommon::SetGLZBufferFlags (CS_ZBUF_NONE);
    csGraphics3DOGLCommon::SetupBlend (CS_FX_COPY, 0, false);
  }

  DrawMode = DrawFlags;

  return true;
}

void csOpenGLProcBackBuffer::FinishDraw ()
{
  if ((!(DrawMode & (CSDRAW_2DGRAPHICS | CSDRAW_3DGRAPHICS))))
    return;
  FinishDraw ();
  statecache->SetCullFace (GL_FRONT);
  glMatrixMode (GL_PROJECTION);
  glPopMatrix();
  glMatrixMode (GL_MODELVIEW);
  glPopMatrix();
  glViewport (0,0,frame_width, frame_height);
  g2d->SetClipRect(oldminx, oldminy, oldmaxx, oldmaxy);
}

void csOpenGLProcBackBuffer::Print (csRect *area)
{
  // Optimise: copy over area only
  (void)area;
  statecache->EnableState (GL_TEXTURE_2D);
  csGraphics3DOGLCommon::SetGLZBufferFlags (CS_ZBUF_NONE);
  csGraphics3DOGLCommon::SetupBlend (CS_FX_COPY, 0, false);
  statecache->DisableState (GL_ALPHA_TEST);

  csTxtCacheData *tex_data = (csTxtCacheData*) tex_mm->GetCacheData();

  if (tex_data)
  {
    //printf ("cached \n");
    // Currently Mesa3.x has bugs which affects at least the voodoo2.
    // Copying the framebuffer to make a new texture will work with the voodoo2
    // as of Mesa CVS 6/6/2000. Unfortunately this change has broken the
    // glCopyTexSubImage command, so I cant test this with just updating
    // the rectangle as it is intended to be.
    /*glDeleteTextures (1, &tex_data->Handle);
    tex_data->Handle = 0;
    glGenTextures (1, &tex_data->Handle);*/
    // Texture is in tha cache, update texture directly.
    statecache->SetTexture (GL_TEXTURE_2D, tex_data->Handle);
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

/*    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
         rstate_bilinearmap ? GL_LINEAR : GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
         rstate_bilinearmap ? GL_LINEAR : GL_NEAREST);*/

    glCopyTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, 0,0,width,height,0);
  }
  else
  {
    // Not in cache.
    //printf ("not cached \n");
#ifdef GL_VERSION_1_2x
    if (pfmt.PixelBytes == 2)
    {

      glReadPixels (0,0, width, height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
        buffer);

      csRGBpixel *dst = tex_0->get_image_data();
      uint16 bb = 8 - pfmt.BlueBits;
      uint16 gb = 8 - pfmt.GreenBits;
      uint16 rb = 8 - pfmt.RedBits;
      uint16 *src = (uint16*) buffer;
      int i;
      for (i = 0; i < width*height; i++, src++, dst++)
      {
	dst->red = ((*src & pfmt.RedMask) >> pfmt.RedShift) << rb;
	dst->green = ((*src & pfmt.GreenMask) >> pfmt.GreenShift) << gb;
	dst->blue = ((*src & pfmt.BlueMask) >> pfmt.BlueShift) << bb;
      }
    }
    else
      glReadPixels (0, 0, width, height,
        GL_RGBA, GL_UNSIGNED_BYTE, tex_0->get_image_data());
#else
    glReadPixels (0,0, width, height, tex_mm->SourceFormat (),
      tex_mm->SourceType (), tex_0->get_image_data ());
#endif
  }

/*  GLenum format, type;
  switch (pixel_bytes)
  {
    case 1:
      format = GL_COLOR_INDEX;
      type = GL_UNSIGNED_BYTE;
      break;
#ifdef GL_VERSION_1_2
    case 2:
      format = GL_RGB;
      type = GL_UNSIGNED_SHORT_5_6_5;
      break;
#endif
    case 4:
      format = GL_RGBA;
      type = GL_UNSIGNED_BYTE;
      break;
    default:
      return; // invalid format
  }
  glRasterPos2i(0, 0);
  glDrawPixels(width, height, format, type, backbuffercopy);*/
}

float csOpenGLProcBackBuffer::GetZBuffValue (int x, int y)
{
  GLfloat zvalue;
  glReadPixels (x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zvalue);
  // 0.090909=1/11, that is 1 divided by total depth delta set by
  // glOrtho. Where 0.090834 comes from, I don't know
  return (0.090834 / (zvalue - (0.090909)));
}

void csOpenGLProcBackBuffer::ClearCache ()
{
  // since we share everything, the main OpenGL renderer
  // should clean up
}

//---------------------------------------------------------------------------
// Dummy iGraphics2D
//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csOpenGLProcBackBuffer2D)
  SCF_IMPLEMENTS_INTERFACE (iGraphics2D)
SCF_IMPLEMENT_IBASE_END;

csOpenGLProcBackBuffer2D::csOpenGLProcBackBuffer2D
    (iGraphics2D *ig2d, int iwidth, int iheight, csPixelFormat *ipfmt)
{
  SCF_CONSTRUCT_IBASE (NULL);
  g2d = ig2d;
  width = iwidth;
  height = iheight;
  pfmt = ipfmt;
  frame_height = g2d->GetHeight ();
  g2d->IncRef ();
}

csOpenGLProcBackBuffer2D::~csOpenGLProcBackBuffer2D ()
{
  g2d->DecRef ();
}

void csOpenGLProcBackBuffer2D::Clear (int color)
{
  CS_ASSERT (pfmt->PixelBytes != 1);
  csGraphics3DOGLCommon::statecache->DisableState (GL_TEXTURE_2D);
  csGraphics3DOGLCommon::SetGLZBufferFlags (CS_ZBUF_NONE);
  csGraphics3DOGLCommon::SetupBlend (CS_FX_COPY, 0, false);
  glColor3f (float (color & pfmt->RedMask  ) / pfmt->RedMask,
       float (color & pfmt->GreenMask) / pfmt->GreenMask,
       float (color & pfmt->BlueMask ) / pfmt->BlueMask);

  glBegin (GL_QUADS);
  glVertex2i (0, 0);
  glVertex2i (width, 0);
  glVertex2i (width, height);
  glVertex2i (0, height);
  glEnd ();
}

void csOpenGLProcBackBuffer2D::DrawLine (float x1, float y1,
              float x2, float y2, int color)
{
  g2d->DrawLine (x1, frame_height - height + y1,
     x2, frame_height - height + y2, color);
}

void csOpenGLProcBackBuffer2D::DrawBox (int x, int y, int w, int h,
             int color)
{
  g2d->DrawBox (x, frame_height - height + y, w, h, color);
}

void csOpenGLProcBackBuffer2D::DrawPixel (int x, int y, int color)
{
  g2d->DrawPixel (x, frame_height - height + y, color);
}

unsigned char *csOpenGLProcBackBuffer2D::GetPixelAt (int x, int y)
{
  return g2d->GetPixelAt (x, frame_height - height + y);
}

csImageArea *csOpenGLProcBackBuffer2D::SaveArea (int x, int y, int w, int h)
{
  return g2d->SaveArea (x, frame_height - height + y, w, h);
}

void csOpenGLProcBackBuffer2D::Write (iFont *font, int x, int y,
  int fg, int bg, const char *str)
{
  g2d->Write (font, x, frame_height - height + y, fg, bg, str);
}

void csOpenGLProcBackBuffer2D::WriteBaseline (iFont *font, int x, int y,
  int fg, int bg, const char *str)
{
  g2d->WriteBaseline (font, x, frame_height - height + y, fg, bg, str);
}

int csOpenGLProcBackBuffer2D::GetWidth ()
{
  return width;
}

int csOpenGLProcBackBuffer2D::GetHeight ()
{
  return height;
}

void csOpenGLProcBackBuffer2D::GetPixel (int x, int y,
  uint8 &oR, uint8 &oG, uint8 &oB)
{
  g2d->GetPixel (x, frame_height - height + y, oR, oG, oB);
}

