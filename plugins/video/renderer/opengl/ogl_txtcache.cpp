/*
    Copyright (C) 1998-2001 by Jorrit Tyberghein
    Copyright (C) 1998 by Dan Ogles.

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
#include "csgeom/subrec.h"

#include "ogl_txtcache.h"
#include "ogl_txtmgr.h"
#include "ogl_g3dcom.h"
#include "imesh/thing/lightmap.h" //@@@!!!
#include "imesh/thing/polygon.h"  //@@@!!!
#include "ivideo/graph3d.h"
#include "ivaria/reporter.h"
#include "video/canvas/openglcommon/glstates.h"


// need definitions of R24(), G24(), and B24()
#ifndef CS_NORMAL_LIGHT_LEVEL
#define CS_NORMAL_LIGHT_LEVEL 128
#endif

//------------------------------------------------------------------------//
void csSLMCacheData::Alloc (csTrianglesPerSuperLightmap* s)
{
  source = s;
  source->cacheData = this;
  isUnlit = source->isUnlit;
}

void csSLMCacheData::Clear()
{
  source->cacheData = NULL;
}


//---------------------------------------------------------------------------//

/// Unload a texture cache element (common for both caches)
void OpenGLTextureCache::Unload (csTxtCacheData *d)
{
  if (d->next)
    d->next->prev = d->prev;
  else
    tail = d->prev;

  if (d->prev)
    d->prev->next = d->next;
  else
    head = d->next;

  glDeleteTextures (1, &d->Handle);
  d->Handle = 0;

  num--;
  total_size -= d->Size;

  iTextureHandle *texh = (iTextureHandle *)d->Source;
  if (texh) texh->SetCacheData (NULL);

  delete d;
}

//----------------------------------------------------------------------------//

OpenGLTextureCache::OpenGLTextureCache (int max_size,
  csGraphics3DOGLCommon* g3d)
{
  cache_size = max_size;
  num = 0;
  head = tail = NULL;
  total_size = 0;
  peak_total_size = 0;
  OpenGLTextureCache::g3d = g3d;
}

OpenGLTextureCache::~OpenGLTextureCache ()
{
  Clear ();
}

void OpenGLTextureCache::Cache (iTextureHandle *txt_handle)
{
  csTxtCacheData *cached_texture = (csTxtCacheData *)
    txt_handle->GetCacheData ();

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
    csTextureHandleOpenGL *txt_mm = (csTextureHandleOpenGL *)
      txt_handle->GetPrivateObject ();

    // unit is not in memory. load it into the cache
    while (cache_size && (total_size + txt_mm->size >= cache_size))
      // out of memory. remove units from bottom of list.
      Unload (tail);

    // now load the unit.
    num++;
    total_size += txt_mm->size;
    peak_total_size = MAX(peak_total_size, total_size);

    cached_texture = new csTxtCacheData;

    cached_texture->next = head;
    cached_texture->prev = NULL;
    if (head)
      head->prev = cached_texture;
    else
      tail = cached_texture;
    head = cached_texture;
    cached_texture->Source = txt_handle;
    cached_texture->Size = txt_mm->size;

    txt_handle->SetCacheData (cached_texture);
    Load (cached_texture);              // load it.
  }
}

void OpenGLTextureCache::Clear ()
{
  while (head)
    Unload (head);

  CS_ASSERT (!head);
  CS_ASSERT (!tail);
  CS_ASSERT (!total_size);
  CS_ASSERT (!num);
}

void OpenGLTextureCache::Uncache (iTextureHandle *texh)
{
  csTxtCacheData *cached_texture = (csTxtCacheData *)texh->GetCacheData ();
  if (cached_texture)
    Unload (cached_texture);
}

void OpenGLTextureCache::Load (csTxtCacheData *d, bool reload)
{
  iTextureHandle *txt_handle = (iTextureHandle *)d->Source;
  csTextureHandleOpenGL *txt_mm = (csTextureHandleOpenGL *)
    txt_handle->GetPrivateObject ();

  if (reload)
  {
    csGraphics3DOGLCommon::statecache->SetTexture (GL_TEXTURE_2D, d->Handle);
  }
  else
  {
    GLuint texturehandle;

    glGenTextures (1, &texturehandle);
    d->Handle = texturehandle;
    csGraphics3DOGLCommon::statecache->SetTexture (
      GL_TEXTURE_2D, texturehandle);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
    rstate_bilinearmap ? GL_LINEAR : GL_NEAREST);
  if (((txt_mm->GetFlags () & (CS_TEXTURE_3D | CS_TEXTURE_NOMIPMAPS))
  	== CS_TEXTURE_3D))
  {
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
      rstate_bilinearmap ? GL_LINEAR_MIPMAP_LINEAR
                         : GL_NEAREST_MIPMAP_NEAREST);
  }
  else
  {
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
      rstate_bilinearmap ? GL_LINEAR : GL_NEAREST);
  }
  if (g3d->EXT_texture_filter_anisotropic)
  {
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
      g3d->txtmgr->texture_filter_anisotropy);
  }

  glGetError ();

  for (int i=0; i < txt_mm->vTex.Length (); i++)
  {
    csTextureOpenGL *togl = txt_mm->vTex[i];
    if (togl->compressed == GL_FALSE)
    {
      glTexImage2D (GL_TEXTURE_2D, i, txt_mm->TargetFormat (),
        togl->get_width (), togl->get_height (),
	0, txt_mm->SourceFormat (), txt_mm->SourceType (), togl->image_data);
      g3d->CheckGLError ("glTexImage2D()");
    }
    else
    {
      csGraphics3DOGLCommon::glCompressedTexImage2DARB (
        GL_TEXTURE_2D, i, (GLenum)togl->internalFormat,
	togl->get_width (), togl->get_height (), 0,
	togl->size, togl->image_data);
      g3d->CheckGLError ("glCompressedTexImage2DARB()");
    }
  }
}

//----------------------------------------------------------------------------//

csSuperLightMap::csSuperLightMap ()
{
  cacheData = NULL;
  Handle = 0;
}

csSuperLightMap::~csSuperLightMap ()
{
  Clear ();
  glDeleteTextures (1, &Handle);
}

void csSuperLightMap::Alloc (csTrianglesPerSuperLightmap* s)
{
  cacheData->Alloc (s);
}

void csSuperLightMap::Clear ()
{
  if (cacheData)
  {
    cacheData->Clear ();
    delete cacheData;
    cacheData = NULL;
  }
}

//----------------------------------------------------------------------------//

int OpenGLLightmapCache::super_lm_num[4];
int OpenGLLightmapCache::super_lm_size = DEFAULT_SUPER_LM_SIZE;

OpenGLLightmapCache::OpenGLLightmapCache (csGraphics3DOGLCommon* g3d)
{
  int i;
  for (i = 0 ; i < 4 ; i++)
  {
    if (super_lm_num[i])
      suplm[i] = new csSuperLightMap [super_lm_num[i]];
    else
      suplm[i] = NULL;
  }
  initialized = false;
  OpenGLLightmapCache::g3d = g3d;
  global_timestamp = 0;
  stats_hit[0] = 0;
  stats_hit[1] = 0;
  stats_hit[2] = 0;
  stats_hit[3] = 0;
  stats_fail[0] = 0;
  stats_fail[1] = 0;
  stats_fail[2] = 0;
  stats_fail[3] = 0;
}

OpenGLLightmapCache::~OpenGLLightmapCache ()
{
  Clear ();
  delete[] suplm[0];
  delete[] suplm[1];
  delete[] suplm[2];
  delete[] suplm[3];

  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  // Delete the temporary handle
  glDeleteTextures (1, &TempHandle0);
  glDeleteTextures (1, &TempHandle1);
  glDeleteTextures (1, &TempHandle2);
  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
}

void OpenGLLightmapCache::Setup ()
{
  if (initialized) return;
  initialized = true;
  int size = super_lm_size;
  int q;
  for (q = 0 ; q < 4 ; q++)
  {
    int i;
    for (i = 0 ; i < super_lm_num[q] ; i++)
    {
      GLuint lightmaphandle;
      glGenTextures (1, &lightmaphandle);
      suplm[q][i].Handle = lightmaphandle;
      csGraphics3DOGLCommon::statecache->SetTexture (
        GL_TEXTURE_2D, lightmaphandle);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      // Normally OpenGL specs say that the last parameter to glTexImage2D
      // can be a NULL pointer. Unfortunatelly not all drivers seem to
      // support that. So I give a dummy texture here.
      char* buf = new char [size*size*4];
      memset (buf, 0, 4*size*size);
      glTexImage2D (GL_TEXTURE_2D, 0, 3, size, size,
          0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
      delete[] buf;
    }
    size >>= 1;
  }

  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  // Initialize the temporary handle
  char* buf;
  glGenTextures (1, &TempHandle0);
  csGraphics3DOGLCommon::statecache->SetTexture (
      GL_TEXTURE_2D, TempHandle0);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // Normally OpenGL specs say that the last parameter to glTexImage2D
  // can be a NULL pointer. Unfortunatelly not all drivers seem to
  // support that. So I give a dummy texture here.
  buf = new char [super_lm_size*super_lm_size*4];
  memset (buf, 0, 4*super_lm_size*super_lm_size);
  glTexImage2D (GL_TEXTURE_2D, 0, 3, super_lm_size, super_lm_size,
      0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
  delete[] buf;

  glGenTextures (1, &TempHandle1);
  csGraphics3DOGLCommon::statecache->SetTexture (
      GL_TEXTURE_2D, TempHandle1);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // Normally OpenGL specs say that the last parameter to glTexImage2D
  // can be a NULL pointer. Unfortunatelly not all drivers seem to
  // support that. So I give a dummy texture here.
  buf = new char [64*64*4];
  memset (buf, 0, 4*64*64);
  glTexImage2D (GL_TEXTURE_2D, 0, 3, 64, 64,
      0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
  delete[] buf;

  glGenTextures (1, &TempHandle2);
  csGraphics3DOGLCommon::statecache->SetTexture (
      GL_TEXTURE_2D, TempHandle2);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // Normally OpenGL specs say that the last parameter to glTexImage2D
  // can be a NULL pointer. Unfortunatelly not all drivers seem to
  // support that. So I give a dummy texture here.
  buf = new char [16*16*4];
  memset (buf, 0, 4*16*16);
  glTexImage2D (GL_TEXTURE_2D, 0, 3, 16, 16,
      0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
  delete[] buf;
  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
}

GLuint OpenGLLightmapCache::GetTempHandle (int lmwidth, int lmheight,
	float& txtsize)
{
  Setup ();
  int lms = MAX (lmwidth, lmheight);
  if (lms > 64)
  {
    txtsize = float (super_lm_size);
    return TempHandle0;
  }
  else if (lms > 16)
  {
    txtsize = 64.0;
    return TempHandle1;
  }
  else
  {
    txtsize = 16.0;
    return TempHandle2;
  }
}

void OpenGLLightmapCache::Clear ()
{
  int q;
  for (q = 0 ; q < 4 ; q++)
  {
    int i;
    for (i = 0 ; i < super_lm_num[q] ; i++)
    {
      suplm[q][i].Clear ();
    }
  }
}

int OpenGLLightmapCache::FindFreeSuperLightmap (int q)
{
  int i;

  for (i = 0 ; i < super_lm_num[q] ; i++)
    if (suplm[q][i].cacheData == NULL) return i;
  return -1;
}

/*
 * Caches a whole precalculated superlighmap
 * s stores:
 * Distribution of the lightmaps in the superlightmap
 * Triangles
 * uv's for each triangles' vertex
 * Basically we have to blit all the lightmaps in a free super
 * lightmap
 */
void OpenGLLightmapCache::Cache (csTrianglesPerSuperLightmap* s, bool dirty,
  bool* modified)
{
  // Mark current super lightmap with timestamp.
  s->timestamp = global_timestamp;

  Setup ();
  *modified = false;
  // First: Try to find a free superlightmap
  // Check if the superLightmap is already in the cache.

  csRect* rectangleArray = s->rectangles.GetArray();
  const csRefArray<iPolygonTexture>& lmArray = s->lightmaps;
  const csGrowingArray<csRGBpixel*>& lmInfo = s->lm_info;
  int q = s->queue_num;

  int i;
  int numLightmaps = s->lightmaps.Length ();
  if (s->cacheData)
  {
    stats_hit[q]++;
    // The data is already in cache, let's see
    // if we need to recalculate the lightmaps
    // due the effect of dynamic lights.

    if (dirty || !s->initialized)
    {
      GLuint SLMHandle = s->cacheData->Handle;
      csGraphics3DOGLCommon::statecache->SetTexture (GL_TEXTURE_2D, SLMHandle);

      for (i = 0; i < numLightmaps; i++)
      {
        if (lmArray[i]->RecalculateDynamicLights ())
        {
          csRGBpixel* lm_data = lmInfo[i];
          csRect& r = rectangleArray[i];
          glTexSubImage2D (GL_TEXTURE_2D, 0, r.xmin, r.ymin,
            r.xmax-r.xmin, r.ymax-r.ymin, GL_RGBA, GL_UNSIGNED_BYTE, lm_data);
        }
      }
      s->initialized = true;
    }

    return;
  }

  // The superlightmap isn't in the cache, so we have to cache it.
  stats_fail[q]++;
  int index = FindFreeSuperLightmap (q);
  if (index < 0)
  {
    // Clear one lightmap.
    // We look for the super lightmap slot with the lowest cost in
    // combination with how long ago it was used.
    int best_cost_idx = -1;
    int best_cost = 1000000000;
    int i;
    for (i = 0 ; i < super_lm_num[q] ; i++)
    {
      csTrianglesPerSuperLightmap* slm = suplm[q][i].cacheData->source;
      int cost = slm->CalculateCost ();

      // Last time we used this super lightmap (in number of frames).
      // If it was 500 frames ago that we used this super lightmap
      // then we will decrease the cost a lot.
      uint32 dt = global_timestamp-slm->timestamp;
      if (dt > 500) cost >>= 2;
      else if (dt > 25) cost >>= 1;

      if (cost < best_cost)
      {
        best_cost = cost;
	best_cost_idx = i;
      }
    }
    suplm[q][best_cost_idx].Clear ();
    index = best_cost_idx;
  }
  //Fill the superLightmap
  suplm[q][index].cacheData = new csSLMCacheData ();
  //We're going to fill the whole super lightmap, so we don't give
  //width and height
  suplm[q][index].Alloc (s);
  csSLMCacheData* superLMData = (csSLMCacheData*) suplm[q][index].cacheData;
  GLuint SLMHandle;
  superLMData->Handle = SLMHandle = suplm[q][index].Handle;
  csGraphics3DOGLCommon::statecache->SetTexture (GL_TEXTURE_2D, SLMHandle);
  for (i = 0; i < numLightmaps; i++)
  {
    if (dirty) lmArray[i]->RecalculateDynamicLights();
    csRGBpixel* lm_data = lmInfo[i];
    csRect& r = rectangleArray[i];
    glTexSubImage2D (GL_TEXTURE_2D, 0, r.xmin, r.ymin,
      r.xmax-r.xmin, r.ymax-r.ymin, GL_RGBA, GL_UNSIGNED_BYTE, lm_data);
  }
  s->initialized = true;
}

bool OpenGLLightmapCache::IsLightmapOK (iPolygonTexture *polytex)
{
  return (polytex->GetLightMap () &&
    (polytex->GetLightMap ()->GetWidth () <= super_lm_size) &&
    (polytex->GetLightMap ()->GetHeight () <= super_lm_size));
}

