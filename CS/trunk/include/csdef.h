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
#if defined(CS_HAS_CMATH_H)
#include <cmath>
#else
#include <math.h>
#endif
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

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

#define CS_STATIC_CAST(T,V)      CS_CAST(static_cast,T,V)
#define CS_DYNAMIC_CAST(T,V)     CS_CAST(dynamic_cast,T,V)
#define CS_REINTERPRET_CAST(T,V) CS_CAST(reinterpret_cast,T,V)
#define CS_CONST_CAST(T,V)       CS_CAST(const_cast,T,V)

// DEPRECATED use the CS_ prefix versions instead.
#define STATIC_CAST(T,V)      CS_STATIC_CAST(T,V)
#define DYNAMIC_CAST(T,V)     CS_DYNAMIC_CAST(T,V)
#define REINTERPRET_CAST(T,V) CS_REINTERPRET_CAST(T,V)
#define CONST_CAST(T,V)       CS_CONST_CAST(T,V)

// Platforms which do not have floating-point variations of the standard math.h
// cos(), sin(), tan(), sqrt(), etc. functions should define
// CS_USE_FAKE_MATH_H_FLOAT_FUNCS.
#if defined(CS_USE_FAKE_MATH_H_FLOAT_FUNCS)
  #define acosf(X)  CS_STATIC_CAST(float,acos(X))
  #define asinf(X)  CS_STATIC_CAST(float,asin(X))
  #define atan2f(X) CS_STATIC_CAST(float,atan2(X))
  #define atanf(X)  CS_STATIC_CAST(float,atan(X))
  #define cosf(X)   CS_STATIC_CAST(float,cos(X))
  #define exp2f(X)  CS_STATIC_CAST(float,exp2(X))
  #define expf(X)   CS_STATIC_CAST(float,exp(X))
  #define log10f(X) CS_STATIC_CAST(float,log10(X))
  #define log2f(X)  CS_STATIC_CAST(float,log2(X))
  #define logf(X)   CS_STATIC_CAST(float,log(X))
  #define powf(X)   CS_STATIC_CAST(float,pow(X))
  #define sinf(X)   CS_STATIC_CAST(float,sin(X))
  #define sqrtf(X)  CS_STATIC_CAST(float,sqrt(X))
  #define tanf(X)   CS_STATIC_CAST(float,tan(X))
  #define floorf(X) CS_STATIC_CAST(float,floor(X))
  #define ceilf(X)  CS_STATIC_CAST(float,ceil(X))
#endif

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
