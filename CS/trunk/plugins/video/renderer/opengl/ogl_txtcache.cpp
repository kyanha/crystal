/*
    Copyright (C) 1998 by Jorrit Tyberghein and Dan Ogles.

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
#include "cssys/sysdriv.h"

#include <GL/gl.h>

#include "ogl_txtcache.h"
#include "ogl_txtmgr.h"
#include "ogl_g3d.h"
#include "isystem.h"
#include "ilghtmap.h"
#include "ipolygon.h"
#include "igraph3d.h"

// need definitions of R24(), G24(), and B24()
#ifndef NORMAL_LIGHT_LEVEL
#define NORMAL_LIGHT_LEVEL 128
#endif

//----------------------------------------------------------------------------//

OpenGLCache::OpenGLCache (int max_size, csCacheType type, int bpp)
{
  OpenGLCache::type = type;
  OpenGLCache::bpp = bpp;
  cache_size = max_size;
  num = 0;
  total_size = 0;
  head = tail = NULL;
}

OpenGLCache::~OpenGLCache ()
{
  Clear ();
}

void OpenGLCache::cache_texture (iTextureHandle *txt_handle)
{
  int size = 0;

  if (type != CS_TEXTURE)
    return;

  for (int c = 0; c < 4; c++)
  {
    int width, height;
    if (txt_handle->GetMipMapDimensions (c, width, height))
      size += width * height;
  }

  size *= bpp / 8;

  csGLCacheData *cached_texture = (csGLCacheData *)txt_handle->GetCacheData ();
  if (cached_texture)
  {
    // move unit to front (MRU)
    if (cached_texture != head)
    {
      if (cached_texture->prev)
        cached_texture->prev->next = cached_texture->next;
      else
        head = cached_texture->next;
      if (cached_texture->next)
        cached_texture->next->prev = cached_texture->prev;
      else
        tail = cached_texture->prev;

      cached_texture->prev = NULL;
      cached_texture->next = head;
      if (head)
        head->prev = cached_texture;
      else
        tail = cached_texture;
      head = cached_texture;
    }
  }
  else
  {
    // unit is not in memory. load it into the cache
    while (total_size + size >= cache_size)
    {
      // out of memory. remove units from bottom of list.
      csGLCacheData *cached_texture = tail;
      iTextureHandle *texh = (iTextureHandle *)cached_texture->Source;
      texh->SetCacheData (NULL);

      tail = tail->prev;
      if (tail)
        tail->next = NULL;
      else
        head = NULL;
      cached_texture->prev = NULL;

      Unload (cached_texture);          // unload it.

      num--;
      total_size -= cached_texture->Size;

      delete cached_texture;
    }

    // now load the unit.
    num++;
    total_size += size;

    cached_texture = new csGLCacheData;

    cached_texture->next = head;
    cached_texture->prev = NULL;
    if (head)
      head->prev = cached_texture;
    else
      tail = cached_texture;
    head = cached_texture;
    cached_texture->Source = txt_handle;
    cached_texture->Size = size;

    txt_handle->SetCacheData (cached_texture);
    Load (cached_texture);              // load it.
  }
}



void OpenGLCache::cache_lightmap (iPolygonTexture *polytex)
{
  csGLCacheData *cached_texture;

  iLightMap *piLM = polytex->GetLightMap ();

  if (type != CS_LIGHTMAP || piLM == NULL)
    return;

  int width = piLM->GetWidth ();
  int height = piLM->GetHeight ();
  int size = width * height * (bpp / 8);

  // if lightmap has changed, uncache it if it was cached so we
  // can reload it
  cached_texture = (csGLCacheData *)piLM->GetCacheData ();
  if (polytex->RecalculateDynamicLights () && cached_texture)
  {
    if (cached_texture->next != NULL)
      cached_texture->next->prev = cached_texture->prev;
    if (cached_texture->prev != NULL)
      cached_texture->prev->next = cached_texture->next;
    piLM->SetCacheData (NULL);

    Unload (cached_texture);                         // unload it.

    num--;
    total_size -= cached_texture->Size;

    if (head == cached_texture) head = NULL;
    if (tail == cached_texture) tail = NULL;

    delete cached_texture;
    cached_texture = NULL;
  }

  if (cached_texture)
  {
    // move unit to front (MRU)
    if (cached_texture != head)
    {
      if (cached_texture->prev)
        cached_texture->prev->next = cached_texture->next;
      else
        head = cached_texture->next;
      if (cached_texture->next)
        cached_texture->next->prev = cached_texture->prev;
      else
        tail = cached_texture->prev;

      cached_texture->prev = NULL;
      cached_texture->next = head;
      if (head)
        head->prev = cached_texture;
      else
        tail = cached_texture;
      head = cached_texture;
    }
  }
  else
  {
    // unit is not in memory. load it into the cache
    while (total_size + size >= cache_size)
    {
      // out of memory. remove units from bottom of list.
      cached_texture = tail;
      tail = tail->prev;
      if (tail)
        tail->next = NULL;
      else
        head = NULL;

      iLightMap *lm = (iLightMap *)cached_texture->Source;
      lm->SetCacheData (NULL);
      cached_texture->prev = NULL;
      Unload (cached_texture);                       // unload it.

      num--;
      total_size -= cached_texture->Size;

      delete cached_texture;
    }

    // now load the unit.
    num++;
    total_size += size;

    cached_texture = new csGLCacheData;

    cached_texture->next = head;
    cached_texture->prev = NULL;
    if (head)
      head->prev = cached_texture;
    else
      tail = cached_texture;
    head = cached_texture;
    cached_texture->Source = piLM;
    cached_texture->Size = size;

    piLM->SetCacheData (cached_texture);
    Load (cached_texture);              // load it.
  }
}

void OpenGLCache::Clear ()
{
  while (head)
  {
    csGLCacheData *n = head->next;

    head->next = head->prev = NULL;

    Unload (head);

    if (type == CS_TEXTURE)
    {
      iTextureHandle *texh = (iTextureHandle *)head->Source;

      if (texh)
        texh->SetCacheData (NULL);
    }
    else if (type == CS_LIGHTMAP)
    {
      iLightMap *lm = (iLightMap *)head->Source;

      if (lm)
        lm->SetCacheData (NULL);
    }

    Unload (head);
    delete head;

    head = n;
  }

  head = tail = NULL;
  total_size = 0;
  num = 0;
}

//----------------------------------------------------------------------------//

OpenGLTextureCache::OpenGLTextureCache(int size, int bitdepth)
  : OpenGLCache (size, CS_TEXTURE, bitdepth)
{
}

OpenGLTextureCache::~OpenGLTextureCache ()
{
  Clear ();
}

void OpenGLTextureCache::Uncache (iTextureHandle *texh)
{
  csGLCacheData *cached_texture = (csGLCacheData *)texh->GetCacheData ();
  if (!cached_texture)
    return;

  texh->SetCacheData (NULL);
  Unload (cached_texture);

  csGLCacheData *n = cached_texture->next;
  if (n && n->next)
    n->next->prev = cached_texture->prev;
  else
    tail = cached_texture->prev;
  if (cached_texture->prev)
    cached_texture->prev->next = n;
  else
    head = n;

  total_size -= cached_texture->Size;
  delete cached_texture;
}

void OpenGLTextureCache::Load (csGLCacheData *d)
{
  iTextureHandle *txt_handle = (iTextureHandle *)d->Source;
  csTextureMM *txt_mm = (csTextureMM *)txt_handle->GetPrivateObject ();

  GLuint texturehandle;
  glGenTextures (1, &texturehandle);
  glBindTexture (GL_TEXTURE_2D, texturehandle);

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, rstate_bilinearmap ? GL_LINEAR : GL_NEAREST);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, rstate_bilinearmap ? GL_LINEAR : GL_NEAREST);
  if (txt_mm->GetFlags () & CS_TEXTURE_3D)
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
      rstate_bilinearmap ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);

  // generate mipmaps
  // bind all mipmap levels required.  In OpenGL this means the original mipmap (level 0)
  // with further levels (1,2,3...) being a reduction of the original mipmap
  // by a factor of 2 in both directions.  You must keep reducing the mipmap size until
  // you get down to the 1x1 size mipmap.
  int mipmaplevel = 0;
  RGBPixel *transp = txt_handle->GetTransparent () ? txt_mm->get_transparent () : NULL;
  iImage *previmg = NULL;
  int twhack = 0, thhack = 0;
  while (true)
  {
    RGBPixel *src;
    iImage *img = NULL;
    int tw, th;

    // the first X mipmap levels are generated by CS
    csTextureOpenGL *txt = (csTextureOpenGL *)txt_mm->get_texture (mipmaplevel);
    if (txt)
    {
      tw = txt->get_width ();
      th = txt->get_height ();
      src = txt->get_image_data ();
    }
    else
    {
      if (!previmg)
        previmg = img = ((csTextureOpenGL *)txt_mm->get_texture
          (mipmaplevel - 1))->get_image ()->MipMap (1, transp);
      else
      {
        // standard CS imaging will not properly mipmap a 1xN or Nx1 bitmap, so we
        // have to make a hack to short-circuit such situations with manual shrinkage
	if ( (previmg->GetWidth() == 1) || (previmg->GetHeight() == 1) )
	{
	  img=previmg;
	  src = (RGBPixel *)previmg->GetImageData();
	  int totalnewpixels = ( previmg->GetWidth() * previmg->GetHeight() ) / 2;
	  for (int skippixel = 0; skippixel < totalnewpixels; skippixel ++)
	    src[skippixel] = src[skippixel*2];
	  
	  // as part of the hack we have to manually track the mipmap size, as the
	  // 'actual' image size is invalid
	  if ( (twhack == 0) || (thhack = 0) )
	  {
	    twhack = previmg->GetWidth();
	    thhack = previmg->GetHeight();
	  }
	  twhack /= 2;
	  thhack /= 2;
	  if (twhack < 1) twhack = 1;
	  if (thhack < 1) thhack = 1;
	}
        else
	{
          img = previmg->MipMap (1, transp);
          previmg->DecRef ();
          previmg = img;
	}
      }
      tw = img->GetWidth ();
      th = img->GetHeight ();
      src = (RGBPixel *)img->GetImageData ();
    }

    if (twhack || thhack)
    {
      tw = twhack;
      th = thhack;
    }

    if (transp)
    {
      int pixels = tw * th;
      RGBPixel *_src = src;
      while (pixels--)
      {
         // By default, every RGBPixel initializes its alpha component to 255.
         // Thus, we should just drop to zero alpha for transparent pixels, if any
         if (transp->eq (*_src))
          _src->alpha = 0;
        _src++;
      }
    }

    // now that the texture has been generated, send it to openGL
    glTexImage2D (GL_TEXTURE_2D, mipmaplevel, 4, tw, th,
      0, GL_RGBA, GL_UNSIGNED_BYTE, src);

    // shrink down to the next mipmap level
    if ((tw <= 1) && (th <= 1)) break;
    mipmaplevel++;

    // if the mipmapping does not properly shrink bitmaps, it will never terminate.
    // this defensive check will terminate anyways
//    if (mipmaplevel > 20) break;
  }
  if (previmg) previmg->DecRef ();

  d->Handle = texturehandle;
}

///
void OpenGLTextureCache::Unload (csGLCacheData *d)
{
  if (d && d->Handle)
  {
    glDeleteTextures(1, &d->Handle);
    d->Handle = 0;
  }
}


//----------------------------------------------------------------------------//
OpenGLLightmapCache::OpenGLLightmapCache(int size, int bitdepth)
  : OpenGLCache(size,CS_LIGHTMAP,bitdepth)
{
}

OpenGLLightmapCache::~OpenGLLightmapCache ()
{
  Clear ();
}


///
void OpenGLLightmapCache::Load(csGLCacheData *d)
{
  iLightMap *lightmap_info = (iLightMap *)d->Source;

  int lmwidth = lightmap_info->GetWidth ();
  int lmheight = lightmap_info->GetHeight ();
  int lmrealwidth = lightmap_info->GetRealWidth ();
  int lmrealheight = lightmap_info->GetRealHeight ();

//lmheight = floor(pow(2.0, ceil(log(lmheight)/log(2.0)) ) );
//lmwidth = floor(pow(2.0, ceil(log(lmwidth)/log(2.0)) ) );

  unsigned char *red_data = lightmap_info->GetMap (0);
  unsigned char *green_data = lightmap_info->GetMap (1);
  unsigned char *blue_data = lightmap_info->GetMap (2);

  // @@@ Note @@@
  // The lightmap data used by Crystal Space is in another format
  // then the lightmap data needed by OpenGL. Maybe we should consider
  // switching Crystal Space to this format as it is more standard?
  // For the software renderer this would be almost no problem and
  // it would make the hardware renderers faster as the following
  // conversion would not be needed anymore.
  unsigned char *lm_data = new unsigned char[lmwidth*lmheight*4];
  unsigned char *walkdata = lm_data;
  unsigned char *last_data = walkdata+(lmwidth*lmheight*4);
  while (walkdata < last_data)
  {
    *walkdata++ = *red_data++;
    *walkdata++ = *green_data++;
    *walkdata++ = *blue_data++;
    *walkdata++ = NORMAL_LIGHT_LEVEL;
  }

  // Because our actual lightmap is a smaller lightmap in a larger
  // po2 texture we can have problems when using GL_LINEAR for
  // the GL_TEXTURE_MAG_FILTER and GL_TEXTURE_MIN_FILTER.
  // We still have to copy the last row and column of the lightmap
  // to just outside the original lightmap boundaries.
  // Otherwise we get a random colored border. Because we use
  // GL_REPEAT the problem is even worse. In that case we also have to
  // copy the first row and column to the last row and column in
  // the po2 lightmap texture.
  int i;
  if (lmwidth != lmrealwidth)
  {
    // Point to last real column.
    unsigned char* lastrealcol = lm_data+(lmrealwidth-1)*4;
    // Copy to next column.
    for (i = 0 ; i < lmheight ; i++)
    {
      lastrealcol[4] = lastrealcol[0];
      lastrealcol[5] = lastrealcol[1];
      lastrealcol[6] = lastrealcol[2];
      lastrealcol += lmwidth*4;
    }

    // Point to last physical column.
    unsigned char* lastcol = lm_data+(lmwidth-1)*4;
    unsigned char* firstcol = lm_data;
    // Copy first column to last.
    for (i = 0 ; i < lmheight ; i++)
    {
      lastcol[0] = firstcol[0];
      lastcol[1] = firstcol[1];
      lastcol[2] = firstcol[2];
      lastcol += lmwidth*4;
      firstcol += lmwidth*4;
    }
  }
  if (lmheight != lmrealheight)
  {
    // Point to last real row.
    unsigned char* lastrealrow = lm_data+lmwidth*(lmrealheight-1)*4;
    // Copy to next row.
    for (i = 0 ; i < lmwidth*4 ; i++) lastrealrow[i+lmwidth*4] = lastrealrow[i];

    // Point to last physical row.
    unsigned char* lastrow = lm_data+lmwidth*(lmheight-1)*4;
    unsigned char* firstrow = lm_data;
    // Copy first row to last.
    for (i = 0 ; i < lmwidth*4 ; i++) lastrow[i] = firstrow[i];
  }

  GLuint lightmaphandle;
  glGenTextures (1, &lightmaphandle);
  glBindTexture (GL_TEXTURE_2D, lightmaphandle);

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexImage2D (GL_TEXTURE_2D, 0, 3, lmwidth, lmheight, 0, GL_RGBA,
    GL_UNSIGNED_BYTE, lm_data);

  GLenum errtest;
  errtest = glGetError();
  if (errtest != GL_NO_ERROR)
  {
    //SysPrintf (MSG_DEBUG_0,"openGL error string: %s\n",gluErrorString(errtest) );
  }

  delete [] lm_data;

/*
    CsPrintf(MSG_DEBUG_0,"caching lightmap in handle %d\n", lightmaphandle);
    CsPrintf(MSG_DEBUG_0,"size (%d,%d)\n",lmwidth,lmheight);
    CsPrintf(MSG_DEBUG_0,"lightmap data location %x\n",lightmap_info);
*/

  d->Handle = lightmaphandle;
}

///
void OpenGLLightmapCache::Unload(csGLCacheData *d)
{
  if (d && d->Handle)
  {
    glDeleteTextures (1, &d->Handle);
    d->Handle = 0;
  }
}
