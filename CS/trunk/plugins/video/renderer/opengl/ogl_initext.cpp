/*
 * Copyright (C) 1998 by Jorrit Tyberghein
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

// THIS FILE IS AUTOMATICALLY GENERATED
/*
  if you want to add more GL extensions, add them first to the list in 
  ogl_suppext.h, then run make_ogl_initext.sh. If some custom extension
  initialization code is required add it to ogl_initext.h. Look there for
  examples.
*/

#include "cssysdef.h"
#include "ogl_g3dcom.h"
#include "ivaria/reporter.h"
#include "ogl_initext.h"

#ifndef CSGL_EXT_STATIC_ASSERTION
# define CSGL_FUNCTION(fType,fName) \
fName = (fType) G2DGL->GetProcAddress ( #fName ); \
allFound = allFound && fName != NULL;
#else
# define CSGL_FUNCTION(fType,fName) \
csGraphics3DOGLCommon::fName = (fType) ::fName; \
allFound = allFound && fName != NULL;
#include "extstat.h"
#endif

// -----------------------

#ifndef INITMORE_ARB_multitexture
#define INITMORE_ARB_multitexture
#endif

void csGraphics3DOGLCommon::Init_ARB_multitexture(iOpenGLInterface *G2DGL) 
{ 
  #if !defined(CSGL_EXT_STATIC_ASSERTION) || defined(CSGL_EXT_STATIC_ASSERTION_ARB_multitexture) 
  bool &allFound = ARB_multitexture;	  	  
  allFound = true;			  
  #define _CSGLEXT_
  #define CSGL_ARB_multitexture
  #include "csglext.h"
  #undef CSGL_ARB_multitexture
  if (!allFound)
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Could not get all function addresses for %s", "GL_ARB_multitexture");
  else
  {
    INITMORE_ARB_multitexture
  }
  #endif
}

#undef INITMORE_ARB_multitexture

// -----------------------

#ifndef INITMORE_ARB_texture_compression
#define INITMORE_ARB_texture_compression
#endif

void csGraphics3DOGLCommon::Init_ARB_texture_compression(iOpenGLInterface *G2DGL) 
{ 
  #if !defined(CSGL_EXT_STATIC_ASSERTION) || defined(CSGL_EXT_STATIC_ASSERTION_ARB_texture_compression) 
  bool &allFound = ARB_texture_compression;	  	  
  allFound = true;			  
  #define _CSGLEXT_
  #define CSGL_ARB_texture_compression
  #include "csglext.h"
  #undef CSGL_ARB_texture_compression
  if (!allFound)
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Could not get all function addresses for %s", "GL_ARB_texture_compression");
  else
  {
    INITMORE_ARB_texture_compression
  }
  #endif
}

#undef INITMORE_ARB_texture_compression

// -----------------------

#ifndef INITMORE_ARB_texture_env_combine
#define INITMORE_ARB_texture_env_combine
#endif

void csGraphics3DOGLCommon::Init_ARB_texture_env_combine(iOpenGLInterface*) 
{ 
  #if !defined(CSGL_EXT_STATIC_ASSERTION) || defined(CSGL_EXT_STATIC_ASSERTION_ARB_texture_env_combine) 
  bool &allFound = ARB_texture_env_combine;	  	  
  allFound = true;			  
  #define _CSGLEXT_
  #define CSGL_ARB_texture_env_combine
  #include "csglext.h"
  #undef CSGL_ARB_texture_env_combine
  if (!allFound)
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Could not get all function addresses for %s", "GL_ARB_texture_env_combine");
  else
  {
    INITMORE_ARB_texture_env_combine
  }
  #endif
}

#undef INITMORE_ARB_texture_env_combine

// -----------------------

#ifndef INITMORE_ARB_texture_env_dot3
#define INITMORE_ARB_texture_env_dot3
#endif

void csGraphics3DOGLCommon::Init_ARB_texture_env_dot3(iOpenGLInterface*) 
{ 
  #if !defined(CSGL_EXT_STATIC_ASSERTION) || defined(CSGL_EXT_STATIC_ASSERTION_ARB_texture_env_dot3) 
  bool &allFound = ARB_texture_env_dot3;	  	  
  allFound = true;			  
  #define _CSGLEXT_
  #define CSGL_ARB_texture_env_dot3
  #include "csglext.h"
  #undef CSGL_ARB_texture_env_dot3
  if (!allFound)
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Could not get all function addresses for %s", "GL_ARB_texture_env_dot3");
  else
  {
    INITMORE_ARB_texture_env_dot3
  }
  #endif
}

#undef INITMORE_ARB_texture_env_dot3

// -----------------------
#ifndef INITMORE_ARB_vertex_program
#define INITMORE_ARB_vertex_program
#endif

void csGraphics3DOGLCommon::Init_ARB_vertex_program(iOpenGLInterface *G2DGL) 
{ 
  #if !defined(CSGL_EXT_STATIC_ASSERTION) || defined(CSGL_EXT_STATIC_ASSERTION_ARB_vertex_program) 
  bool &allFound = ARB_vertex_program;	  	  
  allFound = true;			  
  #define _CSGLEXT_
  #define CSGL_ARB_vertex_program
  #include "csglext.h"
  #undef CSGL_ARB_vertex_program
  if (!allFound)
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Could not get all function addresses for %s", "GL_NV_vertex_program");
  else
  {
    INITMORE_ARB_vertex_program
  }
  #endif
}

#undef INITMORE_ARB_vertex_program

// -----------------------

#ifndef INITMORE_EXT_texture_env_combine
#define INITMORE_EXT_texture_env_combine
#endif

void csGraphics3DOGLCommon::Init_EXT_texture_env_combine(iOpenGLInterface*) 
{ 
  #if !defined(CSGL_EXT_STATIC_ASSERTION) || defined(CSGL_EXT_STATIC_ASSERTION_EXT_texture_env_combine) 
  bool &allFound = EXT_texture_env_combine;	  	  
  allFound = true;			  
  #define _CSGLEXT_
  #define CSGL_EXT_texture_env_combine
  #include "csglext.h"
  #undef CSGL_EXT_texture_env_combine
  if (!allFound)
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Could not get all function addresses for %s", "GL_EXT_texture_env_combine");
  else
  {
    INITMORE_EXT_texture_env_combine
  }
  #endif
}

#undef INITMORE_EXT_texture_env_combine

// -----------------------

#ifndef INITMORE_EXT_texture_env_dot3
#define INITMORE_EXT_texture_env_dot3
#endif

void csGraphics3DOGLCommon::Init_EXT_texture_env_dot3(iOpenGLInterface*) 
{ 
  #if !defined(CSGL_EXT_STATIC_ASSERTION) || defined(CSGL_EXT_STATIC_ASSERTION_EXT_texture_env_dot3) 
  bool &allFound = EXT_texture_env_dot3;	  	  
  allFound = true;			  
  #define _CSGLEXT_
  #define CSGL_EXT_texture_env_dot3
  #include "csglext.h"
  #undef CSGL_EXT_texture_env_dot3
  if (!allFound)
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Could not get all function addresses for %s", "GL_EXT_texture_env_dot3");
  else
  {
    INITMORE_EXT_texture_env_dot3
  }
  #endif
}

#undef INITMORE_EXT_texture_env_dot3

// -----------------------

#ifndef INITMORE_NV_vertex_array_range
#define INITMORE_NV_vertex_array_range
#endif

void csGraphics3DOGLCommon::Init_NV_vertex_array_range(iOpenGLInterface *G2DGL) 
{ 
  #if !defined(CSGL_EXT_STATIC_ASSERTION) || defined(CSGL_EXT_STATIC_ASSERTION_NV_vertex_array_range) 
  bool &allFound = NV_vertex_array_range;	  	  
  allFound = true;			  
  #define _CSGLEXT_
  #define CSGL_NV_vertex_array_range
  #include "csglext.h"
  #undef CSGL_NV_vertex_array_range
  if (!allFound)
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Could not get all function addresses for %s", "GL_NV_vertex_array_range");
  else
  {
    INITMORE_NV_vertex_array_range
  }
  #endif
}

#undef INITMORE_NV_vertex_array_range

// -----------------------

#ifndef INITMORE_NV_vertex_program
#define INITMORE_NV_vertex_program
#endif

void csGraphics3DOGLCommon::Init_NV_vertex_program(iOpenGLInterface *G2DGL) 
{ 
  #if !defined(CSGL_EXT_STATIC_ASSERTION) || defined(CSGL_EXT_STATIC_ASSERTION_NV_vertex_program) 
  bool &allFound = NV_vertex_program;	  	  
  allFound = true;			  
  #define _CSGLEXT_
  #define CSGL_NV_vertex_program
  #include "csglext.h"
  #undef CSGL_NV_vertex_program
  if (!allFound)
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Could not get all function addresses for %s", "GL_NV_vertex_program");
  else
  {
    INITMORE_NV_vertex_program
  }
  #endif
}

#undef INITMORE_NV_vertex_program

// -----------------------

#ifndef INITMORE_SGIS_generate_mipmap
#define INITMORE_SGIS_generate_mipmap
#endif

void csGraphics3DOGLCommon::Init_SGIS_generate_mipmap(iOpenGLInterface*)
{ 
  #if !defined(CSGL_EXT_STATIC_ASSERTION) || defined(CSGL_EXT_STATIC_ASSERTION_SGIS_generate_mipmap) 
  bool &allFound = SGIS_generate_mipmap;	  	  
  allFound = true;			  
  #define _CSGLEXT_
  #define CSGL_SGIS_generate_mipmap
  #include "csglext.h"
  #undef CSGL_SGIS_generate_mipmap
  if (!allFound)
    Report (CS_REPORTER_SEVERITY_NOTIFY,
      "Could not get all function addresses for %s", "GL_SGIS_generate_mipmap");
  else
  {
    INITMORE_SGIS_generate_mipmap
  }
  #endif
}

#undef INITMORE_SGIS_generate_mipmap

#undef CSGL_FUNCTION
