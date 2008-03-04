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

/**\file
 * OpenGL state cache.
 */

#if defined(CS_OPENGL_PATH)
#include CS_HEADER_GLOBAL(CS_OPENGL_PATH,gl.h)
#else
#include <GL/gl.h>
#endif

#include "csextern_gl.h"
#include "csgeom/math.h"
#include "glextmanager.h"

/**\addtogroup plugincommon
 * @{ */

// Set to 'true' to force state changing commands. For debugging.
#define FORCE_STATE_CHANGE			  false/*true*/

#define DECLARE_CACHED_BOOL(name) \
  bool enabled_##name;

#define IMPLEMENT_CACHED_BOOL(name) \
  void Enable_##name () \
  { \
    if (!currentContext->enabled_##name || FORCE_STATE_CHANGE) \
    { \
      currentContext->enabled_##name = true;  \
      glEnable (name); \
    } \
  } \
  void Disable_##name () \
  { \
    if (currentContext->enabled_##name || FORCE_STATE_CHANGE) { \
      currentContext->enabled_##name = false;  \
      glDisable (name); \
    } \
  } \
  bool IsEnabled_##name () const \
  { \
    return currentContext->enabled_##name; \
  }

#define DECLARE_CACHED_BOOL_CURRENTLAYER(name) \
  AutoArray<bool> enabled_##name;

#define IMPLEMENT_CACHED_BOOL_CURRENTLAYER(name)                              \
  void Enable_##name ()                                                       \
  {                                                                           \
    const int currentUnit = currentContext->currentUnit;                      \
    if (!currentContext->enabled_##name[currentUnit] || FORCE_STATE_CHANGE)   \
    {                                                                         \
      ActivateTU (activateTexEnable);                                         \
      currentContext->enabled_##name[currentUnit] = true;                     \
      glEnable (name);                                                        \
    }                                                                         \
  }                                                                           \
  void Disable_##name ()                                                      \
  {                                                                           \
    const int currentUnit = currentContext->currentUnit;                      \
    if (currentContext->enabled_##name[currentUnit] || FORCE_STATE_CHANGE)    \
    {                                                                         \
      ActivateTU (activateTexEnable);                                         \
      currentContext->enabled_##name[currentUnit] = false;                    \
      glDisable (name);                                                       \
    }                                                                         \
  }                                                                           \
  bool IsEnabled_##name () const                                              \
  {                                                                           \
    const int currentUnit = currentContext->currentUnit;                      \
    return currentContext->enabled_##name[currentUnit];                       \
  }

#define DECLARE_CACHED_PARAMETER_1(func, name, type1, param1) \
  type1 parameter_##param1;

#define IMPLEMENT_CACHED_PARAMETER_1(func, name, type1, param1) \
  void Set##name (type1 param1, bool forced = false) \
  { \
    if (forced || (param1 != currentContext->parameter_##param1) || FORCE_STATE_CHANGE) \
    { \
      currentContext->parameter_##param1 = param1;  \
      func (param1); \
    } \
  } \
  void Get##name (type1 & param1) const\
  { \
    param1 = currentContext->parameter_##param1;  \
  }

#define DECLARE_CACHED_PARAMETER_2(func, name, type1, param1, type2, param2) \
  type1 parameter_##param1; \
  type2 parameter_##param2;

#define IMPLEMENT_CACHED_PARAMETER_2(func, name, type1, param1, type2, param2) \
  void Set##name (type1 param1, type2 param2, bool forced = false) \
  { \
    if (forced || (param1 != currentContext->parameter_##param1) || \
        (param2 != currentContext->parameter_##param2) || FORCE_STATE_CHANGE) \
    { \
      currentContext->parameter_##param1 = param1;  \
      currentContext->parameter_##param2 = param2;  \
      func (param1, param2); \
    } \
  } \
  void Get##name (type1 & param1, type2 & param2) const\
  { \
    param1 = currentContext->parameter_##param1;  \
    param2 = currentContext->parameter_##param2;  \
  }

#define DECLARE_CACHED_PARAMETER_3(func, name, type1, param1, type2, param2, type3, param3) \
  type1 parameter_##param1; \
  type2 parameter_##param2; \
  type3 parameter_##param3;

#define IMPLEMENT_CACHED_PARAMETER_3(func, name, type1, param1, type2, param2, type3, param3) \
  void Set##name (type1 param1, type2 param2, type3 param3, bool forced = false) \
  { \
    if (forced || (param1 != currentContext->parameter_##param1) \
        || (param2 != currentContext->parameter_##param2) \
        || (param3 != currentContext->parameter_##param3) || FORCE_STATE_CHANGE) \
    { \
      currentContext->parameter_##param1 = param1;  \
      currentContext->parameter_##param2 = param2;  \
      currentContext->parameter_##param3 = param3;  \
      func (param1, param2, param3); \
    } \
  } \
  void Get##name (type1 &param1, type2 & param2, type3 & param3) const\
  { \
    param1 = currentContext->parameter_##param1;  \
    param2 = currentContext->parameter_##param2;  \
    param3 = currentContext->parameter_##param3;  \
  }

#define DECLARE_CACHED_PARAMETER_4(func, name, type1, param1, \
  type2, param2, type3, param3, type4, param4) \
  type1 parameter_##param1; \
  type2 parameter_##param2; \
  type3 parameter_##param3; \
  type4 parameter_##param4;

#define IMPLEMENT_CACHED_PARAMETER_4(func, name, type1, param1, \
    type2, param2, type3, param3, type4, param4) \
  void Set##name (type1 param1, type2 param2, type3 param3, type4 param4, \
    bool forced = false) \
  { \
    if (forced || (param1 != currentContext->parameter_##param1) || \
      (param2 != currentContext->parameter_##param2) || \
      (param3 != currentContext->parameter_##param3) || \
      (param4 != currentContext->parameter_##param4) || FORCE_STATE_CHANGE) \
    { \
      currentContext->parameter_##param1 = param1;  \
      currentContext->parameter_##param2 = param2;  \
      currentContext->parameter_##param3 = param3;  \
      currentContext->parameter_##param4 = param4;  \
      func (param1, param2, param3, param4); \
    } \
  } \
  void Get##name (type1 &param1, type2 & param2, type3 & param3, type4& param4) const\
  { \
    param1 = currentContext->parameter_##param1;  \
    param2 = currentContext->parameter_##param2;  \
    param3 = currentContext->parameter_##param3;  \
    param4 = currentContext->parameter_##param4;  \
  }

#define DECLARE_CACHED_CLIENT_STATE(name)	      \
  bool enabled_##name;

#define IMPLEMENT_CACHED_CLIENT_STATE(name)	      \
  void Enable_##name () \
  { \
    if (!currentContext->enabled_##name || FORCE_STATE_CHANGE) \
    { \
      currentContext->enabled_##name = true;  \
      glEnableClientState (name); \
    } \
  } \
  void Disable_##name () \
  { \
    if (currentContext->enabled_##name || FORCE_STATE_CHANGE) { \
      currentContext->enabled_##name = false;  \
      glDisableClientState (name); \
    } \
  } \
  bool IsEnabled_##name () const \
  { \
    return currentContext->enabled_##name; \
  }

#define DECLARE_CACHED_CLIENT_STATE_LAYER(name)	                              \
  AutoArray<bool> enabled_##name;

#define IMPLEMENT_CACHED_CLIENT_STATE_LAYER(name)	                      \
  void Enable_##name ()                                                       \
  {                                                                           \
    const int currentUnit = currentContext->currentUnit;                      \
    if (!currentContext->enabled_##name[currentUnit] || FORCE_STATE_CHANGE)   \
    {                                                                         \
      ActivateTU (activateTexCoord);                                          \
      currentContext->enabled_##name[currentUnit] = true;                     \
      glEnableClientState (name);                                             \
    }                                                                         \
  }                                                                           \
  void Disable_##name ()                                                      \
  {                                                                           \
    const int currentUnit = currentContext->currentUnit;                      \
    if (currentContext->enabled_##name[currentUnit] || FORCE_STATE_CHANGE)    \
    {                                                                         \
      ActivateTU (activateTexCoord);                                          \
      currentContext->enabled_##name[currentUnit] = false;                    \
      glDisableClientState (name);                                            \
    }                                                                         \
  }                                                                           \
  bool IsEnabled_##name () const                                              \
  {                                                                           \
    const int currentUnit = currentContext->currentUnit;                      \
    return currentContext->enabled_##name[currentUnit];                       \
  }

#define DECLARE_CACHED_PARAMETER_1_LAYER(func, name, type1, param1) \
  AutoArray<type1> parameter_##param1;

#define IMPLEMENT_CACHED_PARAMETER_1_LAYER(func, name, type1, param1) \
  void Set##name (type1 param1, bool forced = false) \
  { \
    if (forced || \
        (param1 != currentContext->parameter_##param1[currentContext->currentUnit] \
        || FORCE_STATE_CHANGE)) \
    { \
      ActivateTU (); \
      currentContext->parameter_##param1[currentContext->currentUnit] = param1;  \
      func (param1); \
    } \
  } \
  void Get##name (type1 &param1) const\
  { \
    param1 = currentContext->parameter_##param1[currentContext->currentUnit];  \
  }

#define DECLARE_CACHED_PARAMETER_2_LAYER(func, name, type1, param1, \
  type2, param2) \
  AutoArray<type1> parameter_##param1; \
  AutoArray<type2> parameter_##param2;

#define IMPLEMENT_CACHED_PARAMETER_2_LAYER(func, name, type1, param1, \
  type2, param2) \
  void Set##name (type1 param1, type2 param2, bool forced = false) \
  { \
    if (forced || (param1 != currentContext->parameter_##param1[ \
                    currentContext->currentUnit]) || \
                  (param2 != currentContext->parameter_##param2[ \
                    currentContext->currentUnit]) \
               || FORCE_STATE_CHANGE) \
    { \
      ActivateTU (); \
      currentContext->parameter_##param1[currentContext->currentUnit] = param1;  \
      currentContext->parameter_##param2[currentContext->currentUnit] = param2;  \
      func (param1, param2); \
    } \
  } \
  void Get##name (type1 &param1, type2 & param2) const\
  { \
    param1 = currentContext->parameter_##param1[currentContext->currentUnit];  \
    param2 = currentContext->parameter_##param2[currentContext->currentUnit];  \
  }

#define DECLARE_CACHED_PARAMETER_3_LAYER(func, name, type1, param1, \
  type2, param2, type3, param3) \
  AutoArray<type1> parameter_##param1; \
  AutoArray<type2> parameter_##param2; \
  AutoArray<type3> parameter_##param3;


#define IMPLEMENT_CACHED_PARAMETER_3_LAYER(func, name, type1, param1, \
  type2, param2, type3, param3) \
  void Set##name (type1 param1, type2 param2, type3 param3,\
    bool forced = false) \
  { \
    if (forced || (param1 != currentContext->parameter_##param1[ \
                    currentContext->currentUnit]) || \
                  (param2 != currentContext->parameter_##param2[ \
                    currentContext->currentUnit]) || \
                  (param3 != currentContext->parameter_##param3[ \
                    currentContext->currentUnit]) \
               || FORCE_STATE_CHANGE) \
    { \
      ActivateTU (); \
      currentContext->parameter_##param1[currentContext->currentUnit] = param1;  \
      currentContext->parameter_##param2[currentContext->currentUnit] = param2;  \
      currentContext->parameter_##param3[currentContext->currentUnit] = param3;  \
      func (param1, param2, param3); \
    } \
  } \
  void Get##name (type1 &param1, type2 & param2, type3 & param3) const\
  { \
    param1 = currentContext->parameter_##param1[currentContext->currentUnit];  \
    param2 = currentContext->parameter_##param2[currentContext->currentUnit];  \
    param3 = currentContext->parameter_##param3[currentContext->currentUnit];  \
  }


#define DECLARE_CACHED_PARAMETER_4_LAYER(func, name, type1, param1,           \
  type2, param2, type3, param3, type4, param4)                                \
  AutoArray<type1> parameter_##param1;                                        \
  AutoArray<type2> parameter_##param2;                                        \
  AutoArray<type3> parameter_##param3;                                        \
  AutoArray<type4> parameter_##param4;

#define IMPLEMENT_CACHED_PARAMETER_4_LAYER(func, name, type1, param1,         \
    type2, param2, type3, param3, type4, param4)                              \
  void Set##name (type1 param1, type2 param2, type3 param3, type4 param4,     \
    bool forced = false)                                                      \
  {                                                                           \
    const int currentUnit = currentContext->currentUnit;                      \
    if (forced                                                                \
      || (param1 != currentContext->parameter_##param1[currentUnit])          \
      || (param2 != currentContext->parameter_##param2[currentUnit])          \
      || (param3 != currentContext->parameter_##param3[currentUnit])          \
      || (param4 != currentContext->parameter_##param4[currentUnit])          \
      || FORCE_STATE_CHANGE)                                                  \
    {                                                                         \
      ActivateTU (activateTexCoord);                                          \
      currentContext->parameter_##param1[currentUnit] = param1;               \
      currentContext->parameter_##param2[currentUnit] = param2;               \
      currentContext->parameter_##param3[currentUnit] = param3;               \
      currentContext->parameter_##param4[currentUnit] = param4;               \
      func (param1, param2, param3, param4);                                  \
    }                                                                         \
  }                                                                           \
  void Get##name (type1 &param1, type2 & param2, type3 & param3,              \
    type4& param4) const                                                      \
  {                                                                           \
    const int currentUnit = currentContext->currentUnit;                      \
    param1 = currentContext->parameter_##param1[currentUnit];                 \
    param2 = currentContext->parameter_##param2[currentUnit];                 \
    param3 = currentContext->parameter_##param3[currentUnit];                 \
    param4 = currentContext->parameter_##param4[currentUnit];                 \
  }


class csGLStateCacheContext
{
  template<typename T>
  struct AutoArray
  {
    T* p;
    
    AutoArray() : p (0) {}
    ~AutoArray()
    {
      delete[] p;
    }
    void Setup (size_t n)
    {
      CS_ASSERT (p == 0);
      p = new T[n];
    }
    
    T& operator[] (size_t idx)
    {
      return p[idx];
    }
  };
public:
  csGLExtensionManager* extmgr;

  // Special caches
  AutoArray<GLuint> boundtexture;
  GLint numImageUnits;
  GLint numTexCoords;
  int currentUnit;
  int activeUnit[2];
  enum
  {
    boElementArray = 0, boIndexArray, boPixelPack, boPixelUnpack,
    
    boCount
  };
  GLuint currentBufferID[boCount];
  static int GLBufferTargetToCacheIndex (GLenum target)
  {
    switch (target)
    {
    case GL_ELEMENT_ARRAY_BUFFER_ARB: return boElementArray;
    case GL_ARRAY_BUFFER_ARB:         return boIndexArray;
    case GL_PIXEL_PACK_BUFFER_ARB:    return boPixelPack;
    case GL_PIXEL_UNPACK_BUFFER_ARB:  return boPixelUnpack;
    default: return -1;      
    }
  }

  // BlendFunc/BlendFuncSeparate
  GLenum blend_sourceRGB;
  GLenum blend_destinationRGB;
  GLenum blend_sourceA;
  GLenum blend_destinationA;
  
  // Pixel storage
  GLint pixelUnpackAlignment;
  bool pixelUnpackSwapBytes;

  // Standardized caches
  DECLARE_CACHED_BOOL (GL_DEPTH_TEST)
  DECLARE_CACHED_BOOL (GL_BLEND)
  DECLARE_CACHED_BOOL (GL_DITHER)
  DECLARE_CACHED_BOOL (GL_STENCIL_TEST)
  DECLARE_CACHED_BOOL (GL_CULL_FACE)
  DECLARE_CACHED_BOOL (GL_POLYGON_OFFSET_FILL)
  DECLARE_CACHED_BOOL (GL_LIGHTING)
  DECLARE_CACHED_BOOL (GL_ALPHA_TEST)
  DECLARE_CACHED_BOOL (GL_SCISSOR_TEST)
  DECLARE_CACHED_BOOL (GL_TEXTURE_GEN_S)
  DECLARE_CACHED_BOOL (GL_TEXTURE_GEN_T)
  DECLARE_CACHED_BOOL (GL_TEXTURE_GEN_R)
  DECLARE_CACHED_BOOL (GL_TEXTURE_GEN_Q)
  DECLARE_CACHED_BOOL (GL_FOG)
  DECLARE_CACHED_BOOL (GL_COLOR_SUM_EXT)
  DECLARE_CACHED_BOOL_CURRENTLAYER (GL_TEXTURE_1D)
  DECLARE_CACHED_BOOL_CURRENTLAYER (GL_TEXTURE_2D)
  DECLARE_CACHED_BOOL_CURRENTLAYER (GL_TEXTURE_3D)
  DECLARE_CACHED_BOOL_CURRENTLAYER (GL_TEXTURE_CUBE_MAP)
  DECLARE_CACHED_BOOL_CURRENTLAYER (GL_TEXTURE_RECTANGLE_ARB)
  DECLARE_CACHED_PARAMETER_2 (glAlphaFunc, AlphaFunc, GLenum, alpha_func, GLclampf, alpha_ref)
  DECLARE_CACHED_PARAMETER_1 (glCullFace, CullFace, GLenum, cull_mode)
  DECLARE_CACHED_PARAMETER_1 (glDepthFunc, DepthFunc, GLenum, depth_func)
  DECLARE_CACHED_PARAMETER_1 (glDepthMask, DepthMask, GLboolean, depth_mask)
  DECLARE_CACHED_PARAMETER_1 (glShadeModel, ShadeModel, GLenum, shade_model)
  DECLARE_CACHED_PARAMETER_3 (glStencilFunc, StencilFunc, GLenum, stencil_func, GLint, stencil_ref, GLuint, stencil_mask)
  DECLARE_CACHED_PARAMETER_3 (glStencilOp, StencilOp, GLenum, stencil_fail, GLenum, stencil_zfail, GLenum, stencil_zpass)
  DECLARE_CACHED_PARAMETER_1 (glStencilMask, StencilMask, GLuint, maskl)
  DECLARE_CACHED_PARAMETER_4 (glColorMask, ColorMask, GLboolean, wmRed, \
    GLboolean, wmGreen, GLboolean, wmBlue, GLboolean, wmAlpha)

  DECLARE_CACHED_CLIENT_STATE (GL_VERTEX_ARRAY)
  DECLARE_CACHED_CLIENT_STATE (GL_COLOR_ARRAY)
  DECLARE_CACHED_CLIENT_STATE (GL_SECONDARY_COLOR_ARRAY_EXT)
  DECLARE_CACHED_CLIENT_STATE (GL_NORMAL_ARRAY)
  DECLARE_CACHED_CLIENT_STATE_LAYER (GL_TEXTURE_COORD_ARRAY)

  DECLARE_CACHED_PARAMETER_1 (glMatrixMode, MatrixMode, GLenum, matrixMode)
  
  DECLARE_CACHED_PARAMETER_4 (glVertexPointer, VertexPointer, GLint, vsize,
    GLenum, vtype, GLsizei, vstride, GLvoid*, vpointer)
  DECLARE_CACHED_PARAMETER_3 (glNormalPointer, NormalPointer, GLenum, ntype,
    GLsizei, nstride, GLvoid*, npointer)
  DECLARE_CACHED_PARAMETER_4 (glColorPointer, ColorPointer, GLint, csize,
    GLenum, ctype, GLsizei, cstride, GLvoid*, cpointer)
  DECLARE_CACHED_PARAMETER_4 (extmgr->glSecondaryColorPointerEXT, 
    SecondaryColorPointerEXT, GLint, scsize, GLenum, sctype, GLsizei, scstride, 
    GLvoid*, scpointer);
  DECLARE_CACHED_PARAMETER_4_LAYER (glTexCoordPointer, TexCoordPointer, GLint, tsize,
    GLenum, ttype, GLsizei, tstride, GLvoid*, tpointer)
  
  csGLStateCacheContext (csGLExtensionManager* extmgr)
  {
    csGLStateCacheContext::extmgr = extmgr;
  
    // Need to init exts here b/c we need the image units count
    extmgr->InitGL_ARB_multitexture ();
    extmgr->InitGL_ARB_fragment_program ();
    
    if (extmgr->CS_GL_ARB_fragment_program)
    {
      glGetIntegerv (GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &numImageUnits);
      glGetIntegerv (GL_MAX_TEXTURE_COORDS_ARB, &numTexCoords);
    }
    else if (extmgr->CS_GL_ARB_multitexture)
    {
      glGetIntegerv (GL_MAX_TEXTURE_UNITS_ARB, &numImageUnits);
      numTexCoords = numImageUnits;
    }
    else
    {
      numTexCoords = numImageUnits = 1;
    }
    
    boundtexture.Setup (numImageUnits);
    enabled_GL_TEXTURE_1D.Setup (numImageUnits);
    enabled_GL_TEXTURE_2D.Setup (numImageUnits);
    enabled_GL_TEXTURE_3D.Setup (numImageUnits);
    enabled_GL_TEXTURE_CUBE_MAP.Setup (numImageUnits);
    enabled_GL_TEXTURE_RECTANGLE_ARB.Setup (numImageUnits);
    
    enabled_GL_TEXTURE_COORD_ARRAY.Setup (numTexCoords);
    parameter_tsize.Setup (numTexCoords);
    parameter_ttype.Setup (numTexCoords);
    parameter_tstride.Setup (numTexCoords);
    parameter_tpointer.Setup (numTexCoords);
  }

  /** 
   * Init cache. Does both retrieval of current GL state as well as setting
   * some states to known values.
   */
  void InitCache()
  {
    int i;
    glGetIntegerv (GL_ALPHA_TEST_FUNC, (GLint*)&parameter_alpha_func);
    glGetFloatv (GL_ALPHA_TEST_REF, &parameter_alpha_ref);
    if (extmgr->CS_GL_EXT_blend_func_separate)
    {
      glGetIntegerv (GL_BLEND_SRC_RGB_EXT, (GLint*)&blend_sourceRGB);
      glGetIntegerv (GL_BLEND_SRC_ALPHA_EXT, (GLint*)&blend_sourceA);
      glGetIntegerv (GL_BLEND_DST_RGB_EXT, (GLint*)&blend_destinationRGB);
      glGetIntegerv (GL_BLEND_DST_ALPHA_EXT, (GLint*)&blend_destinationA);
    }
    else
    {
      glGetIntegerv (GL_BLEND_SRC, (GLint*)&blend_sourceRGB);
      blend_sourceA = blend_sourceRGB;
      glGetIntegerv (GL_BLEND_DST, (GLint*)&blend_destinationRGB);
      blend_destinationA = blend_destinationRGB;
    }
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
    glGetIntegerv (GL_MATRIX_MODE, (GLint*)&parameter_matrixMode);
    GLboolean writemask[4];
    glGetBooleanv (GL_COLOR_WRITEMASK, writemask);
    parameter_wmRed = writemask[0];
    parameter_wmGreen = writemask[1];
    parameter_wmBlue = writemask[2];
    parameter_wmAlpha = writemask[3];
    enabled_GL_DEPTH_TEST = (glIsEnabled (GL_DEPTH_TEST) == GL_TRUE);
    enabled_GL_BLEND = (glIsEnabled (GL_BLEND) == GL_TRUE);
    enabled_GL_DITHER = (glIsEnabled (GL_DITHER) == GL_TRUE);
    enabled_GL_STENCIL_TEST = (glIsEnabled (GL_STENCIL_TEST) == GL_TRUE);
    enabled_GL_CULL_FACE = (glIsEnabled (GL_CULL_FACE) == GL_TRUE);
    enabled_GL_POLYGON_OFFSET_FILL = (glIsEnabled (GL_POLYGON_OFFSET_FILL) == GL_TRUE);
    enabled_GL_LIGHTING = (glIsEnabled (GL_LIGHTING) == GL_TRUE);
    enabled_GL_ALPHA_TEST = (glIsEnabled (GL_ALPHA_TEST) == GL_TRUE);
    enabled_GL_TEXTURE_GEN_S = (glIsEnabled (GL_TEXTURE_GEN_S) == GL_TRUE);
    enabled_GL_TEXTURE_GEN_T = (glIsEnabled (GL_TEXTURE_GEN_T) == GL_TRUE);
    enabled_GL_TEXTURE_GEN_R = (glIsEnabled (GL_TEXTURE_GEN_R) == GL_TRUE);
    enabled_GL_TEXTURE_GEN_Q = (glIsEnabled (GL_TEXTURE_GEN_Q) == GL_TRUE);
    enabled_GL_FOG = (glIsEnabled (GL_FOG) == GL_TRUE);

    memset (boundtexture.p, 0, numImageUnits * sizeof (GLuint));
    currentUnit = 0;
    memset (activeUnit, 0, sizeof (activeUnit));
    if (extmgr->CS_GL_ARB_multitexture)
    {
      for (i = numImageUnits; i-- > 0; )
      {
        extmgr->glActiveTextureARB (GL_TEXTURE0_ARB + i);
        enabled_GL_TEXTURE_1D[i] = (glIsEnabled (GL_TEXTURE_1D) == GL_TRUE);
        enabled_GL_TEXTURE_2D[i] = (glIsEnabled (GL_TEXTURE_2D) == GL_TRUE);
        enabled_GL_TEXTURE_3D[i] = (glIsEnabled (GL_TEXTURE_3D) == GL_TRUE);
        enabled_GL_TEXTURE_CUBE_MAP[i] = (glIsEnabled (GL_TEXTURE_CUBE_MAP) == GL_TRUE);
	if (extmgr->CS_GL_ARB_texture_rectangle
	  || extmgr->CS_GL_EXT_texture_rectangle
	  || extmgr->CS_GL_NV_texture_rectangle)
	  enabled_GL_TEXTURE_RECTANGLE_ARB[i] = (glIsEnabled (GL_TEXTURE_RECTANGLE_ARB) == GL_TRUE);
	else
	  enabled_GL_TEXTURE_RECTANGLE_ARB[i] = false;
      }
      for (i = numTexCoords; i-- > 0; )
      {
        extmgr->glClientActiveTextureARB (GL_TEXTURE0_ARB + i);
        enabled_GL_TEXTURE_COORD_ARRAY[i] = (glIsEnabled (GL_TEXTURE_COORD_ARRAY) == GL_TRUE);
        glGetIntegerv (GL_TEXTURE_COORD_ARRAY_SIZE, (GLint*)&parameter_tsize[i]);
        glGetIntegerv (GL_TEXTURE_COORD_ARRAY_STRIDE, (GLint*)&parameter_tstride[i]);
        glGetIntegerv (GL_TEXTURE_COORD_ARRAY_TYPE, (GLint*)&parameter_ttype[i]);
        glGetPointerv (GL_TEXTURE_COORD_ARRAY_POINTER, &parameter_tpointer[i]);
      }
    } 
    else 
    {
      enabled_GL_TEXTURE_1D[0] = (glIsEnabled (GL_TEXTURE_1D) == GL_TRUE);
      enabled_GL_TEXTURE_2D[0] = (glIsEnabled (GL_TEXTURE_2D) == GL_TRUE);
      enabled_GL_TEXTURE_3D[0] = (glIsEnabled (GL_TEXTURE_3D) == GL_TRUE);
      enabled_GL_TEXTURE_CUBE_MAP[0] = (glIsEnabled (GL_TEXTURE_CUBE_MAP) == GL_TRUE);
      enabled_GL_TEXTURE_COORD_ARRAY[0] = (glIsEnabled (GL_TEXTURE_COORD_ARRAY) == GL_TRUE);
	if (extmgr->CS_GL_ARB_texture_rectangle
	  || extmgr->CS_GL_EXT_texture_rectangle
	  || extmgr->CS_GL_NV_texture_rectangle)
	  enabled_GL_TEXTURE_RECTANGLE_ARB[0] = (glIsEnabled (GL_TEXTURE_RECTANGLE_ARB) == GL_TRUE);
	else
	  enabled_GL_TEXTURE_RECTANGLE_ARB[0] = false;
      glGetIntegerv (GL_TEXTURE_COORD_ARRAY_SIZE, (GLint*)&parameter_tsize[0]);
      glGetIntegerv (GL_TEXTURE_COORD_ARRAY_STRIDE, (GLint*)&parameter_tstride[0]);
      glGetIntegerv (GL_TEXTURE_COORD_ARRAY_TYPE, (GLint*)&parameter_ttype[0]);
      glGetPointerv (GL_TEXTURE_COORD_ARRAY_POINTER, &parameter_tpointer[0]);
      for (i = 1 ; i < numImageUnits; i++)
      {
        enabled_GL_TEXTURE_1D[i] = enabled_GL_TEXTURE_1D[0];
        enabled_GL_TEXTURE_2D[i] = enabled_GL_TEXTURE_2D[0];
        enabled_GL_TEXTURE_3D[i] = enabled_GL_TEXTURE_3D[0];
        enabled_GL_TEXTURE_CUBE_MAP[i] = enabled_GL_TEXTURE_CUBE_MAP[0];
        enabled_GL_TEXTURE_COORD_ARRAY[i] = enabled_GL_TEXTURE_COORD_ARRAY[0];
	enabled_GL_TEXTURE_RECTANGLE_ARB[i] = enabled_GL_TEXTURE_RECTANGLE_ARB[0];
	parameter_tsize[i] = parameter_tsize[0];
	parameter_tstride[i] = parameter_tstride[0];
	parameter_ttype[i] = parameter_ttype[0];
	parameter_tpointer[i] = parameter_tpointer[0];
      }
    }
    enabled_GL_SCISSOR_TEST = (glIsEnabled (GL_SCISSOR_TEST) == GL_TRUE);
    enabled_GL_VERTEX_ARRAY = (glIsEnabled (GL_VERTEX_ARRAY) == GL_TRUE);
    enabled_GL_COLOR_ARRAY = (glIsEnabled (GL_COLOR_ARRAY) == GL_TRUE);
    if (extmgr->CS_GL_EXT_secondary_color)
      enabled_GL_SECONDARY_COLOR_ARRAY_EXT = 
        (glIsEnabled (GL_SECONDARY_COLOR_ARRAY_EXT) == GL_TRUE);
    else
      enabled_GL_SECONDARY_COLOR_ARRAY_EXT = false;
    enabled_GL_NORMAL_ARRAY = (glIsEnabled (GL_NORMAL_ARRAY) == GL_TRUE);

    memset (currentBufferID, 0, sizeof (currentBufferID));
    {
      static const GLenum localIndexToGLBufferBinding[boCount] =
      { GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB, GL_ARRAY_BUFFER_BINDING_ARB, 
	GL_PIXEL_PACK_BUFFER_BINDING_ARB, GL_PIXEL_UNPACK_BUFFER_BINDING_ARB };

      enum { extVBO = 1, extPBO = 2 };
      static const GLenum requiredExt[boCount] =
      { extVBO, extVBO, extPBO, extPBO };
      
      int boExt = 0;
      if (extmgr->CS_GL_ARB_vertex_buffer_object) boExt |= extVBO;
      if (extmgr->CS_GL_ARB_pixel_buffer_object) boExt |= extPBO;
      for (int b = 0; b < boCount; b++)
      {
	if (requiredExt[b] & boExt)
	{
	  glGetIntegerv (localIndexToGLBufferBinding[b], 
	    (GLint*)&currentBufferID[b]);
	}
      }
    }

    glGetIntegerv (GL_VERTEX_ARRAY_SIZE, (GLint*)&parameter_vsize);
    glGetIntegerv (GL_VERTEX_ARRAY_STRIDE, (GLint*)&parameter_vstride);
    glGetIntegerv (GL_VERTEX_ARRAY_TYPE, (GLint*)&parameter_vtype);
    glGetPointerv (GL_VERTEX_ARRAY_POINTER, &parameter_vpointer);

    glGetIntegerv (GL_NORMAL_ARRAY_STRIDE, (GLint*)&parameter_nstride);
    glGetIntegerv (GL_NORMAL_ARRAY_TYPE, (GLint*)&parameter_ntype);
    glGetPointerv (GL_NORMAL_ARRAY_POINTER, &parameter_npointer);

    glGetIntegerv (GL_COLOR_ARRAY_SIZE, (GLint*)&parameter_csize);
    glGetIntegerv (GL_COLOR_ARRAY_STRIDE, (GLint*)&parameter_cstride);
    glGetIntegerv (GL_COLOR_ARRAY_TYPE, (GLint*)&parameter_ctype);
    glGetPointerv (GL_COLOR_ARRAY_POINTER, &parameter_cpointer);
    
    if (extmgr->CS_GL_EXT_secondary_color)
    {
      glGetIntegerv (GL_SECONDARY_COLOR_ARRAY_SIZE_EXT, 
        (GLint*)&parameter_scsize);
      glGetIntegerv (GL_SECONDARY_COLOR_ARRAY_STRIDE_EXT, 
        (GLint*)&parameter_scstride);
      glGetIntegerv (GL_SECONDARY_COLOR_ARRAY_TYPE_EXT, 
        (GLint*)&parameter_sctype);
      glGetPointerv (GL_SECONDARY_COLOR_ARRAY_POINTER_EXT, 
        &parameter_scpointer);
      enabled_GL_COLOR_SUM_EXT = glIsEnabled (GL_COLOR_SUM_EXT) != GL_FALSE;
    }
    else
    {
      parameter_scsize = 0;
      parameter_scstride = 0;
      parameter_sctype = 0;
      parameter_scpointer = 0;
      enabled_GL_COLOR_SUM_EXT = false;
    }
    
    glGetIntegerv (GL_UNPACK_ALIGNMENT, &pixelUnpackAlignment);
    GLint v;
    glGetIntegerv (GL_UNPACK_SWAP_BYTES, &v);
    pixelUnpackSwapBytes = v != 0;
  }
};


/**
 * OpenGL state cache.
 * All state changes that are made often (possibly with the same value, ie
 * actually no change) or across plugins should be done through the cache.
 * \remarks
 * Since this class is passed directly between plugins the
 * code in this class cannot do memory allocations or
 * deallocations. The functions in this class will only
 * manipulate member variables.
 */
class csGLStateCache
{
  enum
  {
    texServer = 0,
    texClient = 1
  };
public:
  csGLExtensionManager* extmgr;
  csGLStateCacheContext* currentContext;

  csGLStateCache (csGLExtensionManager* extmgr)
  {
    csGLStateCache::extmgr = extmgr;
    currentContext = 0;
  }

  void SetContext (csGLStateCacheContext *context)
  {
    currentContext = context;
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
  IMPLEMENT_CACHED_BOOL (GL_TEXTURE_GEN_Q)
  IMPLEMENT_CACHED_BOOL (GL_FOG)
  IMPLEMENT_CACHED_BOOL (GL_COLOR_SUM_EXT)
  IMPLEMENT_CACHED_BOOL_CURRENTLAYER (GL_TEXTURE_1D)
  IMPLEMENT_CACHED_BOOL_CURRENTLAYER (GL_TEXTURE_2D)
  IMPLEMENT_CACHED_BOOL_CURRENTLAYER (GL_TEXTURE_3D)
  IMPLEMENT_CACHED_BOOL_CURRENTLAYER (GL_TEXTURE_CUBE_MAP)
  IMPLEMENT_CACHED_BOOL_CURRENTLAYER (GL_TEXTURE_RECTANGLE_ARB)
  IMPLEMENT_CACHED_PARAMETER_2 (glAlphaFunc, AlphaFunc, GLenum, alpha_func, GLclampf, alpha_ref)
  IMPLEMENT_CACHED_PARAMETER_1 (glCullFace, CullFace, GLenum, cull_mode)
  IMPLEMENT_CACHED_PARAMETER_1 (glDepthFunc, DepthFunc, GLenum, depth_func)
  IMPLEMENT_CACHED_PARAMETER_1 (glDepthMask, DepthMask, GLboolean, depth_mask)
  IMPLEMENT_CACHED_PARAMETER_1 (glShadeModel, ShadeModel, GLenum, shade_model)
  IMPLEMENT_CACHED_PARAMETER_3 (glStencilFunc, StencilFunc, GLenum, stencil_func, GLint, stencil_ref, GLuint, stencil_mask)
  IMPLEMENT_CACHED_PARAMETER_3 (glStencilOp, StencilOp, GLenum, stencil_fail, GLenum, stencil_zfail, GLenum, stencil_zpass)
  IMPLEMENT_CACHED_PARAMETER_1 (glStencilMask, StencilMask, GLuint, maskl)
  IMPLEMENT_CACHED_PARAMETER_4 (glColorMask, ColorMask, GLboolean, wmRed, \
    GLboolean, wmGreen, GLboolean, wmBlue, GLboolean, wmAlpha)

  IMPLEMENT_CACHED_CLIENT_STATE (GL_VERTEX_ARRAY)
  IMPLEMENT_CACHED_CLIENT_STATE (GL_COLOR_ARRAY)
  IMPLEMENT_CACHED_CLIENT_STATE (GL_SECONDARY_COLOR_ARRAY_EXT)
  IMPLEMENT_CACHED_CLIENT_STATE (GL_NORMAL_ARRAY)
  IMPLEMENT_CACHED_CLIENT_STATE_LAYER (GL_TEXTURE_COORD_ARRAY)

  IMPLEMENT_CACHED_PARAMETER_1 (glMatrixMode, MatrixMode, GLenum, matrixMode)
  
  IMPLEMENT_CACHED_PARAMETER_4 (glVertexPointer, VertexPointer, GLint, vsize,
    GLenum, vtype, GLsizei, vstride, GLvoid*, vpointer);
  IMPLEMENT_CACHED_PARAMETER_3 (glNormalPointer, NormalPointer, GLenum, ntype,
    GLsizei, nstride, GLvoid*, npointer);
  IMPLEMENT_CACHED_PARAMETER_4 (glColorPointer, ColorPointer, GLint, csize,
    GLenum, ctype, GLsizei, cstride, GLvoid*, cpointer);
  IMPLEMENT_CACHED_PARAMETER_4 (extmgr->glSecondaryColorPointerEXT, 
    SecondaryColorPointerExt, GLint, scsize, GLenum, sctype, GLsizei, scstride, 
    GLvoid*, scpointer);
  IMPLEMENT_CACHED_PARAMETER_4_LAYER (glTexCoordPointer, TexCoordPointer, GLint, tsize,
    GLenum, ttype, GLsizei, tstride, GLvoid*, tpointer);
  
  // Special caches
  void SetTexture (GLenum target, GLuint texture)
  {
    const int currentUnit = currentContext->currentUnit;
    if (texture != currentContext->boundtexture[currentUnit])
    {
      ActivateTU (activateImage);
      currentContext->boundtexture[currentUnit] = texture;
      glBindTexture (target, texture);
    }
  }
  GLuint GetTexture (GLenum /*target*/)
  {
    const int currentUnit = currentContext->currentUnit;
    return currentContext->boundtexture[currentUnit];
  }
  GLuint GetTexture (GLenum /*target*/, int unit)
  {
    return currentContext->boundtexture[unit];
  }
  /**
   * Select the currently active texture unit. 
   * \remarks Doesn't check whether the  multitexture extension is actually 
   * supported, this must be done in calling code.
   */
  void SetCurrentTU (int unit)
  {
    currentContext->currentUnit = unit;   
  }
  /// Query active texture unit.
  int GetCurrentTU ()
  {
    return currentContext->currentUnit;
  }
  /// Flag that the active TU should be used for setting texture coords
  static const int activateTexCoord = 1 << texClient;
  /// Flag that the active TU should be used for binding textures
  static const int activateImage = 1 << texServer;
  /// Flag that the active TU should be used when changing the texture matrix
  static const int activateMatrix = 1 << texServer;
  /// Flag that the active TU should be used when changing the texture environment
  static const int activateTexEnv = 1 << texServer;
  /**
   * Flag that the active TU should be used when changing the texture coord
   * generation parameters.
   */
  static const int activateTexGen = 1 << texServer;
  /// Flag that the active TU should be used when enabling/disabling texturing
  static const int activateTexEnable = 1 << texServer;
  /**
   * Activate the currently selected TU. 
   * \a usage is a combination of UnitTexCoord and UnitImage and specifies,
   * for what the current unit is to be used.
   * \remarks Doesn't check whether the  multitexture extension is actually 
   *   supported, this must be done in calling code.
   */
  void ActivateTU (uint usage)
  {
    int currentUnit = currentContext->currentUnit;
    for (int i = 0; i < 2; i++)
    {
      if (currentContext->activeUnit[i] != currentUnit)
      {
        GLuint tu = GL_TEXTURE0_ARB + currentUnit;
        if (usage & (1 << i))
        {
          if (i == texClient)
            extmgr->glClientActiveTextureARB (tu);
          else
            extmgr->glActiveTextureARB (tu);
          currentContext->activeUnit[i] = currentUnit;
        }
      }
    }
  }

  /**
   * Bind a given VBO/PBO buffer.
   * \remarks Doesn't check whether the relevant buffer object extension is 
   *   actually supported, this must be done in calling code.
   */
  void SetBufferARB (GLenum target, GLuint id)
  {
    int index = csGLStateCacheContext::GLBufferTargetToCacheIndex (target);
    CS_ASSERT (index >= 0);
    if (id != currentContext->currentBufferID[index])
    {
      extmgr->glBindBufferARB (target, id);
      currentContext->currentBufferID[index] = id;

      if (target == GL_ARRAY_BUFFER_ARB)
      {
        //invalidate vertex pointers
        currentContext->parameter_vpointer = (GLvoid*)~0; 
        currentContext->parameter_npointer = (GLvoid*)~0;
        currentContext->parameter_cpointer = (GLvoid*)~0;
        memset(currentContext->parameter_tpointer.p, ~0,
          sizeof(GLvoid*)*currentContext->numImageUnits);
      }
    }
  }

  /**
   * Get the currently bound VBO/PBO buffer.
   * \remarks Doesn't check whether the relevant buffer object extension is 
   *   actually supported, this must be done in calling code.
   */
  GLuint GetBufferARB (GLenum target)
  {
    int index = csGLStateCacheContext::GLBufferTargetToCacheIndex (target);
    CS_ASSERT (index >= 0);
    return currentContext->currentBufferID[index];
  }

  /**\name Blend functions
   * @{ */
  void SetBlendFunc (GLenum blend_source, GLenum blend_destination, 
		      bool forced = false)
  {
    if (forced 
      || (blend_source != currentContext->blend_sourceRGB)
      || (blend_source != currentContext->blend_sourceA)
      || (blend_destination != currentContext->blend_destinationRGB) 
      || (blend_destination != currentContext->blend_destinationA) 
      || FORCE_STATE_CHANGE)
    {
      currentContext->blend_sourceRGB = blend_source;
      currentContext->blend_sourceA = blend_source;
      currentContext->blend_destinationRGB = blend_destination;
      currentContext->blend_destinationA = blend_destination;
      glBlendFunc (blend_source, blend_destination);
    }
  }
  void GetBlendFunc (GLenum& blend_source, GLenum& blend_destination) const
  {
    blend_source = currentContext->blend_sourceRGB;
    blend_destination = currentContext->blend_destinationRGB;
  }
  void SetBlendFuncSeparate (GLenum blend_sourceRGB, 
			      GLenum blend_destinationRGB, 
			      GLenum blend_sourceA, 
			      GLenum blend_destinationA, 
			      bool forced = false)
  {
    if (forced 
      || (blend_sourceRGB != currentContext->blend_sourceRGB)
      || (blend_sourceA != currentContext->blend_sourceA)
      || (blend_destinationRGB != currentContext->blend_destinationRGB) 
      || (blend_destinationA != currentContext->blend_destinationA) 
      || FORCE_STATE_CHANGE)
    {
      currentContext->blend_sourceRGB = blend_sourceRGB;
      currentContext->blend_sourceA = blend_sourceA;
      currentContext->blend_destinationRGB = blend_destinationRGB;
      currentContext->blend_destinationA = blend_destinationA;
      extmgr->glBlendFuncSeparateEXT (blend_sourceRGB, blend_destinationRGB, 
	blend_sourceA, blend_destinationA);
    }
  }
  void GetBlendFuncSeparate (GLenum& blend_sourceRGB, 
			      GLenum& blend_destinationRGB,
			      GLenum& blend_sourceA, 
			      GLenum& blend_destinationA) const
  {
    blend_sourceRGB = currentContext->blend_sourceRGB;
    blend_destinationRGB = currentContext->blend_destinationRGB;
    blend_sourceA = currentContext->blend_sourceA;
    blend_destinationA = currentContext->blend_destinationA;
  }
  /** @} */
  
  /**\name Pixel storage
   * @{ */
  GLint GetPixelUnpackAlignment ()
  { return currentContext->pixelUnpackAlignment; }
  void SetPixelUnpackAlignment (GLint alignment)
  {
    if (alignment != currentContext->pixelUnpackAlignment)
    {
      glPixelStorei (GL_UNPACK_ALIGNMENT, alignment);
      currentContext->pixelUnpackAlignment = alignment;
    }
  }
  bool GetPixelUnpackSwapBytes ()
  { return currentContext->pixelUnpackSwapBytes; }
  void SetPixelUnpackSwapBytes (GLint swap)
  {
    if (swap != currentContext->pixelUnpackSwapBytes)
    {
      glPixelStorei (GL_UNPACK_SWAP_BYTES, (GLint)swap);
      currentContext->pixelUnpackSwapBytes = swap;
    }
  }
  /** @} */
  
  /// Query the number of texture image units supported by OpenGL
  GLint GetNumImageUnits() const { return currentContext->numImageUnits; }
  /// Query the number of texture coordinate sets supported by OpenGL
  GLint GetNumTexCoords() const { return currentContext->numTexCoords; }
};

#undef IMPLEMENT_CACHED_BOOL
#undef IMPLEMENT_CACHED_BOOL_CURRENTLAYER
#undef IMPLEMENT_CACHED_PARAMETER_1
#undef IMPLEMENT_CACHED_PARAMETER_2
#undef IMPLEMENT_CACHED_PARAMETER_3
#undef IMPLEMENT_CACHED_PARAMETER_4
#undef IMPLEMENT_CACHED_PARAMETER_1_LAYER
#undef IMPLEMENT_CACHED_PARAMETER_2_LAYER
#undef IMPLEMENT_CACHED_PARAMETER_3_LAYER
#undef IMPLEMENT_CACHED_PARAMETER_4_LAYER
#undef IMPLEMENT_CACHED_CLIENT_STATE
#undef IMPLEMENT_CACHED_CLIENT_STATE_LAYER

#undef DECLARE_CACHED_BOOL
#undef DECLARE_CACHED_BOOL_CURRENTLAYER
#undef DECLARE_CACHED_PARAMETER_1
#undef DECLARE_CACHED_PARAMETER_2
#undef DECLARE_CACHED_PARAMETER_3
#undef DECLARE_CACHED_PARAMETER_4
#undef DECLARE_CACHED_PARAMETER_1_LAYER
#undef DECLARE_CACHED_PARAMETER_2_LAYER
#undef DECLARE_CACHED_PARAMETER_3_LAYER
#undef DECLARE_CACHED_PARAMETER_4_LAYER
#undef DECLARE_CACHED_CLIENT_STATE
#undef DECLARE_CACHED_CLIENT_STATE_LAYER

#undef FORCE_STATE_CHANGE

/** @} */

#endif // __CS_GLSTATES_H__
