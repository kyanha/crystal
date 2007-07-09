/*
    Copyright (C) 2005 by Jorrit Tyberghein
              (C) 2005 by Frank Richter

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

#include "cssysdef.h"

#include "csgfx/bakekeycolor.h"
#include "csgfx/imagememory.h"
#include "csgfx/packrgb.h"

#include "gl_render3d.h"
#include "gl_txtmgr.h"
#include "gl_r2t_framebuf.h"

CS_PLUGIN_NAMESPACE_BEGIN(gl3d)
{

void csGLRender2TextureFramebuf::SetRenderTarget (iTextureHandle* handle, 
						  bool persistent,
						  int subtexture)
{
  render_target = handle;
  rt_onscreen = !persistent;
  sub_texture_id = subtexture;

  if (handle)
  {
    render_target->GetRendererDimensions (txt_w, txt_h);
    G3D->GetDriver2D()->PerformExtension ("vp_set", txt_w, txt_h);

    G3D->GetDriver2D()->GetClipRect (rt_old_minx, rt_old_miny, 
      rt_old_maxx, rt_old_maxy);
    if ((rt_old_minx != 0) || (rt_old_miny != 0)
      || (rt_old_maxx != txt_w) || (rt_old_maxy != txt_h))
    {
      G3D->GetDriver2D()->SetClipRect (0, 0, txt_w, txt_h);
    }
  }
  else
  {
    G3D->GetDriver2D()->PerformExtension ("vp_reset");
    G3D->GetDriver2D()->SetClipRect (rt_old_minx, rt_old_miny, 
      rt_old_maxx, rt_old_maxy);
  }
}

void csGLRender2TextureFramebuf::BeginDraw (int drawflags)
{
  GLRENDER3D_OUTPUT_STRING_MARKER((" "));

  /* Note: the renderer relies on this function to setup
   * matrices etc. So be careful when changing stuff. */

  G3D->GetDriver2D()->PerformExtension ("glflushtext");
  if (drawflags & CSDRAW_3DGRAPHICS)
  {
  }
  else if (drawflags & CSDRAW_2DGRAPHICS)
  {
    /*
      Render target: draw everything flipped.
    */
    G3D->statecache->SetMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    G3D->SetGlOrtho (true);
  }
  if (!rt_onscreen)
  {
    G3D->statecache->SetShadeModel (GL_FLAT);
    glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
    G3D->ActivateTexture (render_target);
    G3D->statecache->Disable_GL_BLEND ();
    G3D->SetZMode (CS_ZBUF_NONE);

    csGLBasicTextureHandle* tex_mm = 
      static_cast<csGLBasicTextureHandle*> (render_target->GetPrivateObject ());
    GLenum textarget = tex_mm->GetGLTextureTarget();

    GLint oldMagFilt, oldMinFilt;
    glGetTexParameteriv (textarget, GL_TEXTURE_MAG_FILTER, &oldMagFilt);
    glGetTexParameteriv (textarget, GL_TEXTURE_MIN_FILTER, &oldMinFilt);
    glTexParameteri (textarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (textarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBegin (GL_QUADS);
    glTexCoord2f (0, 0); glVertex2i (0, txt_h);
    glTexCoord2f (0, 1); glVertex2i (0, 0);
    glTexCoord2f (1, 1); glVertex2i (txt_w, 0);
    glTexCoord2f (1, 0); glVertex2i (txt_w, txt_h);
    glEnd ();
    rt_onscreen = true;
    glTexParameteri (textarget, GL_TEXTURE_MAG_FILTER, oldMagFilt);
    glTexParameteri (textarget, GL_TEXTURE_MIN_FILTER, oldMinFilt);
  }
  G3D->statecache->SetCullFace (GL_BACK);
}

void csGLRender2TextureFramebuf::SetupProjection ()
{
  GLRENDER3D_OUTPUT_LOCATION_MARKER;
  G3D->SetGlOrtho (true);
}

void csGLRender2TextureFramebuf::FinishDraw ()
{
  GLRENDER3D_OUTPUT_LOCATION_MARKER;

  G3D->statecache->SetCullFace (GL_FRONT);

  if (rt_onscreen)
  {
    rt_onscreen = false;
    csGLBasicTextureHandle* tex_mm = 
      static_cast<csGLBasicTextureHandle*> (render_target->GetPrivateObject ());
    tex_mm->Precache ();
    // Texture is in tha cache, update texture directly.
    G3D->ActivateTexture (tex_mm);
    /* Texture has a keycolor - so we need to deal specially with it
     * to make sure the keycolor gets transparent. */
    if (tex_mm->GetKeyColor ())
    {
      // @@@ Processing sucks, but how else to handle keycolor?
      const size_t numPix = txt_w * txt_h;
      pixelScratch.SetSize (numPix * 4);
      glReadPixels (0, 0, txt_w, txt_h, GL_RGBA, 
	GL_UNSIGNED_BYTE, pixelScratch.GetArray());
      csRGBpixel key;
      tex_mm->GetKeyColor (key.red, key.green, key.blue);
      csBakeKeyColor::RGBA2D (pixelScratch.GetArray(), 
	pixelScratch.GetArray(), txt_w, txt_h, key);
      tex_mm->Blit (0, 0, txt_w, txt_h, pixelScratch.GetArray());
    }
    else
    {
      GLenum textarget = tex_mm->GetGLTextureTarget();
      if ((textarget != GL_TEXTURE_2D) && (textarget != GL_TEXTURE_RECTANGLE_ARB) 
		  && (textarget != GL_TEXTURE_CUBE_MAP))
        return;

	  bool handle_subtexture = (textarget == GL_TEXTURE_CUBE_MAP);
      /* Reportedly, some drivers crash if using CopyTexImage on a texture
       * size larger than the framebuffer. Use CopyTexSubImage then. */
      bool needSubImage = (txt_w > G3D->GetWidth()) 
	|| (txt_h > G3D->GetHeight());
      // Texture was not used as a render target before.
      // Make some necessary adjustments.
      if (!tex_mm->IsWasRenderTarget())
      {
	tex_mm->SetupAutoMipping();
	tex_mm->SetWasRenderTarget (true);
	tex_mm->texFormat = iTextureHandle::RGBA8888;
	if (needSubImage)
	{
	  // Gah. Turn target texture to RGBA storage.
	  uint8* blub = new uint8[txt_w * txt_h * 4];
	  glTexImage2D (textarget, 0, GL_RGBA, txt_w, txt_h, 
	    0, GL_RGBA, GL_UNSIGNED_BYTE, blub);
	  delete[] blub;
	}
      }
      if (needSubImage)
		{
		  if (handle_subtexture)
			glCopyTexSubImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + sub_texture_id, 0, 0, 0, 0, 0, 
			  G3D->GetWidth(), G3D->GetHeight());
		  else
		   glCopyTexSubImage2D (textarget, 0, 0, 0, 0, 0, 
		    G3D->GetWidth(), G3D->GetHeight());
		}
      else
		{
		  if (handle_subtexture)
		    glCopyTexSubImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + sub_texture_id, 
			  0, 0, 0, 0, 0, txt_w, txt_h);
		  else
		    glCopyTexImage2D (textarget, 0, GL_RGBA, 0, 0, txt_w, txt_h, 0);
		      tex_mm->SetNeedMips (true);
		}
    }
  }
}

void csGLRender2TextureFramebuf::SetClipRect (const csRect& clipRect)
{
  GLRENDER3D_OUTPUT_LOCATION_MARKER;
  glScissor (clipRect.xmin, txt_h - clipRect.ymax, clipRect.Width(),
    clipRect.Height());
}

void csGLRender2TextureFramebuf::SetupClipPortalDrawing ()
{
  G3D->statecache->SetMatrixMode (GL_MODELVIEW);
  glScalef (1, -1, 1);
  //G3D->statecache->SetCullFace (GL_BACK);
}

bool csGLRender2TextureFramebuf::HasStencil()
{
  GLint sbits;
  glGetIntegerv (GL_STENCIL_BITS, &sbits);
  return sbits > 0;
}

}
CS_PLUGIN_NAMESPACE_END(gl3d)
