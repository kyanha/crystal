/*
    Copyright (C) 1998-2000 by Jorrit Tyberghein
  
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

#if !defined(CSDEF_FRIEND)
#error You are not allowed to include this file! Use cssysdef.h instead.
#endif

#ifndef __CS_CSDEF_H__
#define __CS_CSDEF_H__

#include "platform.h"
#include "cstypes.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

// DEPRECATED use "true" instead
#ifndef TRUE
  #define TRUE 1
#endif

// DEPRECATED use "false" instead
#ifndef FALSE
  #define FALSE 0
#endif

#ifndef MIN
  #define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
  #define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef ABS
 #define ABS(x) ((x)<0?-(x):(x))
#endif

#ifndef SIGN
  #define SIGN(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0))
#endif

#ifndef PI
 #define PI 3.1415926535897932385f
#endif
#ifndef HALF_PI
  #define HALF_PI (PI / 2.0f)
#endif
#ifndef TWO_PI
  #define TWO_PI (PI * 2.0f)
#endif

#undef EPSILON
#define EPSILON 0.001f			/* Small value */
#undef SMALL_EPSILON
#define SMALL_EPSILON 0.000001f		/* Very small value */
#undef SMALL_EPSILON_D
#define SMALL_EPSILON_D 0.000000000001f	/* Very, very small value */

// Platforms with compilers which only understand old-style C++ casting syntax
// should define CS_USE_OLD_STYLE_CASTS.
#if defined(CS_USE_OLD_STYLE_CASTS)
  #define CS_CAST(C,T,V) ((T)(V))
#else
  #define CS_CAST(C,T,V) (C<T>(V))
#endif

#define STATIC_CAST(T,V)      CS_CAST(static_cast,T,V)
#define DYNAMIC_CAST(T,V)     CS_CAST(dynamic_cast,T,V)
#define REINTERPRET_CAST(T,V) CS_CAST(reinterpret_cast,T,V)
#define CONST_CAST(T,V)       CS_CAST(const_cast,T,V)

// Platforms with compilers which do not understand the new C++ keyword
// `explicit' should define CS_USE_FAKE_EXPLICIT_KEYWORD.
#if defined(CS_USE_FAKE_EXPLICIT_KEYWORD)
  #define explicit /* nothing */
#endif

// Platforms with compilers which do not understand the new C++ keyword
// `typename' should define CS_USE_FAKE_TYPENAME_KEYWORD.
#if defined(CS_USE_FAKE_TYPENAME_KEYWORD)
  #define typename /* nothing */
#endif

// Platforms with compilers which do not undersatnd the new C++ explicit
// template specialization syntax `template<>' should define
// CS_USE_OLD_TEMPLATE_SPECIALIZATION.
#if defined(CS_USE_OLD_TEMPLATE_SPECIALIZATION)
  #define CS_SPECIALIZE_TEMPLATE
#else
  #define CS_SPECIALIZE_TEMPLATE template<>
#endif

// The smallest Z at which 3D clipping occurs
#define SMALL_Z 0.01f

#endif // __CS_CSDEF_H__
