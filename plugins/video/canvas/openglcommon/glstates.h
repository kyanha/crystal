/*
    Copyright (C) 2002 by Anders Stenberg
    Written by Anders Stenberg

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

#ifndef __CS_GLSTATES_H__
#define __CS_GLSTATES_H__

#if defined(CS_OPENGL_PATH)
#include CS_HEADER_GLOBAL(CS_OPENGL_PATH,gl.h)
#else
#include <GL/gl.h>
#endif

#include "video/canvas/openglcommon/glextmanager.h"

#define IMPLEMENT_CACHED_BOOL(name) \
  bool enabled_##name; \
  void Enable_##name () \
  { \
    if (!enabled_##name) \
    { \
      enabled_##name = true;  \
      glEnable (name); \
    } \
  } \
  void Disable_##name () \
  { \
    if (enabled_##name) { \
      enabled_##name = false;  \
      glDisable (name); \
    } \
  } \
  bool IsEnabled_##name () const \
  { \
    return enabled_##name; \
  }

#define CS_GL_MAX_LAYER 16

#define IMPLEMENT_CACHED_BOOL_LAYER(name) \
  bool enabled_##name[CS_GL_MAX_LAYER]; \
  void Enable_##name (int l = 0) \
  { \
    if (!enabled_##name[l]) \
    { \
      enabled_##name[l] = true;  \
      glEnable (name); \
    } \
  } \
  void Disable_##name (int l = 0) \
  { \
    if (enabled_##name[l]) \
    { \
      enabled_##name[l] = false;  \
      glDisable (name); \
    } \
  } \
  bool IsEnabled_##name (int l = 0) const \
  { \
    return enabled_##name[l]; \
  }

#define IMPLEMENT_CACHED_BOOL_CURRENTLAYER(name) \
  bool enabled_##name[CS_GL_MAX_LAYER]; \
  void Enable_##name () \
  { \
    if (!enabled_##name[currentUnit]) \
    { \
      enabled_##name[currentUnit] = true;  \
      glEnable (name); \
    } \
  } \
  void Disable_##name () \
  { \
    if (enabled_##name[currentUnit]) \
    { \
      enabled_##name[currentUnit] = false;  \
      glDisable (name); \
    } \
  } \
  bool IsEnabled_##name () const \
  { \
    return enabled_##name[currentUnit]; \
  }

#define IMPLEMENT_CACHED_PARAMETER_1(func, name, type1, param1) \
  type1 parameter_##param1; \
  void Set##name (type1 param1, bool forced = false) \
  { \
    if (forced || (param1 != parameter_##param1)) \
    { \
      parameter_##param1 = param1;  \
      func (param1); \
    } \
  } \
  void Get##name (type1 & param1) \
  { \
    param1 = parameter_##param1;  \
  }

#define IMPLEMENT_CACHED_PARAMETER_2(func, name, type1, param1, type2, param2) \
  type1 parameter_##param1; \
  type2 parameter_##param2; \
  void Set##name (type1 param1, type2 param2, bool forced = false) \
  { \
    if (forced || (param1 != parameter_##param1) || (param2 != parameter_##param2)) \
    { \
      parameter_##param1 = param1;  \
      parameter_##param2 = param2;  \
      func (param1, param2); \
    } \
  } \
  void Get##name (type1 & param1, type2 & param2) \
  { \
    param1 = parameter_##param1;  \
    param2 = parameter_##param2;  \
  }

#define IMPLEMENT_CACHED_PARAMETER_3(func, name, type1, param1, type2, param2, type3, param3) \
  type1 parameter_##param1; \
  type2 parameter_##param2; \
  type3 parameter_##param3; \
  void Set##name (type1 param1, type2 param2, type3 param3, bool forced = false) \
  { \
    if (forced || (param1 != parameter_##param1) || (param2 != parameter_##param2) || (param3 != parameter_##param3)) \
    { \
      parameter_##param1 = param1;  \
      parameter_##param2 = param2;  \
      parameter_##param3 = param3;  \
      func (param1, param2, param3); \
    } \
  } \
  void Get##name (type1 &param1, type2 & param2, type3 & param3) \
  { \
    param1 = parameter_##param1;  \
    param2 = parameter_##param2;  \
    param3 = parameter_##param3;  \
  }


/**
 * Since this class is passed directly between plugins the
 * code in this class cannot do memory allocations or
 * deallocations. The functions in this class will only
 * manipulate member variables.
 */
class csGLStateCache
{
public:
  csGLExtensionManager* extmgr;

  csGLStateCache (csGLExtensionManager* extmgr)
  {
    csGLStateCache::extmgr = extmgr;
  }
  
  /// Init cache
  void InitCache()
  {
    int i;
    glGetIntegerv (GL_ALPHA_TEST_FUNC, (GLint*)&parameter_alpha_func);
    glGetFloatv (GL_ALPHA_TEST_REF, &parameter_alpha_ref);
    glGetIntegerv (GL_BLEND_SRC, (GLint*)&parameter_blend_source);
    glGetIntegerv (GL_BLEND_DST, (GLint*)&parameter_blend_destination);
    glGetIntegerv (GL_CULL_FACE_MODE, (GLint*)&parameter_cull_mode);
    glGetIntegerv (GL_DEPTH_FUNC, (GLint*)&parameter_depth_func);
    glGetBooleanv (GL_DEPTH_WRITEMASK, &parameter_depth_mask);
    glGetIntegerv (GL_SHADE_MODEL, (GLint*)&parameter_shade_model);
    glGetIntegerv (GL_STENCIL_BITS, (GLint*)&parameter_maskl);
    glGetIntegerv (GL_STENCIL_FUNC, (GLint*)&parameter_stencil_func);
    glGetIntegerv (GL_STENCIL_VALUE_MASK, (GLint*)&parameter_stencil_mask);
    glGetIntegerv (GL_STENCIL_REF, &parameter_stencil_ref);
    glGetIntegerv (GL_STENCIL_FAIL, (GLint*)&parameter_stencil_fail);
    glGetIntegerv (GL_STENCIL_PASS_DEPTH_FAIL, (GLint*)&parameter_stencil_zfail);
    glGetIntegerv (GL_STENCIL_PASS_DEPTH_PASS, (GLint*)&parameter_stencil_zpass);
    enabled_GL_DEPTH_TEST = glIsEnabled (GL_DEPTH_TEST);
    enabled_GL_BLEND = glIsEnabled (GL_BLEND);
    enabled_GL_DITHER = glIsEnabled (GL_DITHER);
    enabled_GL_STENCIL_TEST = glIsEnabled (GL_STENCIL_TEST);
    enabled_GL_CULL_FACE = glIsEnabled (GL_CULL_FACE);
    enabled_GL_POLYGON_OFFSET_FILL = glIsEnabled (GL_POLYGON_OFFSET_FILL);
    enabled_GL_LIGHTING = glIsEnabled (GL_LIGHTING);
    enabled_GL_ALPHA_TEST = glIsEnabled (GL_ALPHA_TEST);
    enabled_GL_TEXTURE_GEN_S = glIsEnabled (GL_TEXTURE_GEN_S);
    enabled_GL_TEXTURE_GEN_T = glIsEnabled (GL_TEXTURE_GEN_T);
    enabled_GL_TEXTURE_GEN_R = glIsEnabled (GL_TEXTURE_GEN_R);
    enabled_GL_FOG = glIsEnabled (GL_FOG);
    enabled_GL_TEXTURE_1D[0] = glIsEnabled (GL_TEXTURE_1D);
    enabled_GL_TEXTURE_2D[0] = glIsEnabled (GL_TEXTURE_2D);
    enabled_GL_TEXTURE_3D[0] = glIsEnabled (GL_TEXTURE_3D);
    enabled_GL_TEXTURE_CUBE_MAP[0] = glIsEnabled (GL_TEXTURE_CUBE_MAP);
    for (i = 1 ; i < CS_GL_MAX_LAYER; i++)
    {
      enabled_GL_TEXTURE_1D[i] = enabled_GL_TEXTURE_1D[0];
      enabled_GL_TEXTURE_2D[i] = enabled_GL_TEXTURE_2D[0];
      enabled_GL_TEXTURE_3D[i] = enabled_GL_TEXTURE_3D[0];
      enabled_GL_TEXTURE_CUBE_MAP[i] = enabled_GL_TEXTURE_CUBE_MAP[0];
    }
    enabled_GL_SCISSOR_TEST = glIsEnabled (GL_SCISSOR_TEST);

    memset (boundtexture, 0, CS_GL_MAX_LAYER * sizeof (GLuint));
    currentUnit = 0;
  }

  // Standardized caches
  IMPLEMENT_CACHED_BOOL (GL_DEPTH_TEST)
  IMPLEMENT_CACHED_BOOL (GL_BLEND)
  IMPLEMENT_CACHED_BOOL (GL_DITHER)
  IMPLEMENT_CACHED_BOOL (GL_STENCIL_TEST)
  IMPLEMENT_CACHED_BOOL (GL_CULL_FACE)
  IMPLEMENT_CACHED_BOOL (GL_POLYGON_OFFSET_FILL)
  IMPLEMENT_CACHED_BOOL (GL_LIGHTING)
  IMPLEMENT_CACHED_BOOL (GL_ALPHA_TEST)
  IMPLEMENT_CACHED_BOOL (GL_SCISSOR_TEST)
  IMPLEMENT_CACHED_BOOL (GL_TEXTURE_GEN_S)
  IMPLEMENT_CACHED_BOOL (GL_TEXTURE_GEN_T)
  IMPLEMENT_CACHED_BOOL (GL_TEXTURE_GEN_R)
  IMPLEMENT_CACHED_BOOL (GL_FOG)
  IMPLEMENT_CACHED_BOOL_CURRENTLAYER (GL_TEXTURE_1D)
  IMPLEMENT_CACHED_BOOL_CURRENTLAYER (GL_TEXTURE_2D)
  IMPLEMENT_CACHED_BOOL_CURRENTLAYER (GL_TEXTURE_3D)
  IMPLEMENT_CACHED_BOOL_CURRENTLAYER (GL_TEXTURE_CUBE_MAP)
  IMPLEMENT_CACHED_PARAMETER_2 (glAlphaFunc, AlphaFunc, GLenum, alpha_func, GLclampf, alpha_ref)
  IMPLEMENT_CACHED_PARAMETER_2 (glBlendFunc, BlendFunc, GLenum, blend_source, GLenum, blend_destination)
  IMPLEMENT_CACHED_PARAMETER_1 (glCullFace, CullFace, GLenum, cull_mode)
  IMPLEMENT_CACHED_PARAMETER_1 (glDepthFunc, DepthFunc, GLenum, depth_func)
  IMPLEMENT_CACHED_PARAMETER_1 (glDepthMask, DepthMask, GLboolean, depth_mask)
  IMPLEMENT_CACHED_PARAMETER_1 (glShadeModel, ShadeModel, GLenum, shade_model)
  IMPLEMENT_CACHED_PARAMETER_3 (glStencilFunc, StencilFunc, GLenum, stencil_func, GLint, stencil_ref, GLuint, stencil_mask)
  IMPLEMENT_CACHED_PARAMETER_3 (glStencilOp, StencilOp, GLenum, stencil_fail, GLenum, stencil_zfail, GLenum, stencil_zpass)
  IMPLEMENT_CACHED_PARAMETER_1 (glStencilMask, StencilMask, GLuint, maskl)

  // Special caches
  GLuint boundtexture[CS_GL_MAX_LAYER]; // 32 max texture layers
  int currentUnit;
  void SetTexture (GLenum target, GLuint texture)
  {
    if (texture != boundtexture[currentUnit])
    {
      boundtexture[currentUnit] = texture;
      glBindTexture (target, texture);
    }
  }
  GLuint GetTexture (GLenum target)
  {
    return boundtexture[currentUnit];
  }
  GLuint GetTexture (GLenum target, int unit)
  {
    return boundtexture[unit];
  }
  /**
   * Set active texture unit. Doesn't check whether the multitexture ext is
   * actually supported, this must be done in calling code.
   */
  void SetActiveTU (int unit)
  {
    if (currentUnit != unit)
    {
      extmgr->glActiveTextureARB (GL_TEXTURE0_ARB + unit);
      extmgr->glClientActiveTextureARB (GL_TEXTURE0_ARB + unit);
      currentUnit = unit;
    }
  }
  int GetActiveTU ()
  {
    return currentUnit;
  }
};

#undef IMPLEMENT_CACHED_BOOL
#undef IMPLEMENT_CACHED_BOOL_LAYER
#undef IMPLEMENT_CACHED_BOOL_CURRENTLAYER
#undef IMPLEMENT_CACHED_PARAMETER_1
#undef IMPLEMENT_CACHED_PARAMETER_2
#undef IMPLEMENT_CACHED_PARAMETER_3

#endif // __CS_GLSTATES_H__
