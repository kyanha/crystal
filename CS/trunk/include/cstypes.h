/*
    Copyright (C) 1998 by Jorrit Tyberghein
  
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

#ifndef __CS_CSTYPES_H__
#define __CS_CSTYPES_H__

/**\file
 */
/**\addtogroup util
 * @{
 */
#ifndef __CS_CSSYSDEFS_H__
#error "cssysdef.h must be included in EVERY source file!"
#endif

#include "platform.h"
#include <float.h>

//-----------------------------------------------------------------------------
// If your compiler complains about 'true', 'false', and 'bool' it may be an
// older C++ compiler which doesn't understand these constructs.  In that case,
// set CS_USE_FAKE_BOOL_TYPE to 'yes' in the makefile, or define the C-macro
// CS_USE_FAKE_BOOL_TYPE in your project file.
//-----------------------------------------------------------------------------
#ifdef CS_USE_FAKE_BOOL_TYPE
typedef int bool;
#undef  true
#define true 1
#undef  false
#define false 0
#endif

/**\name Specific sized types.
 * The following types should be used whenever you need a variable of
 * a specific size (in bits).  If these types are already defined by system
 * headers for a particular platform, then define CS_BUILTIN_SIZED_TYPES to
 * avoid duplicate type definition here.
 * @{ */
#if !defined(CS_BUILTIN_SIZED_TYPES)


#ifndef CS_HAS_STDINT_H
/// unsigned 8-bit integer (0..255)
typedef unsigned char uint8;
/// signed 8-bit integer (-128..127)
typedef char int8;
/// unsigned 16-bit integer (0..65 535)
typedef unsigned short uint16;
/// signed 16-bit integer (-32 768..32 767)
typedef short int16;
/// unsigned 32-bit integer (0..4 294 967 295)
typedef unsigned int uint32;
/// signed 32-bit integer (-2 147 483 648..2 147 483 647)
typedef int int32;
#ifdef COMP_GCC
/// unsigned 64 bit integer
typedef unsigned long long uint64;
/// signed 64 bit integer
typedef long long int64;
/// specify 64 bit integer constant
#define CONST_INT64(x) x ## LL
/// specify 64 bit unsigned integer constant
#define CONST_UINT64(x) x ## ULL
#else
# ifdef COMP_VC
/// unsigned 64 bit integer
typedef unsigned __int64 uint64;
/// signed 64 bit integer
typedef __int64 int64;
/// specify 64 bit integer constant
#define CONST_INT64(x) x##i64
/// specify 64 bit unsigned integer constant
#define CONST_UINT64(x) x##ui64
# else
#  warning NO definition for 64 bit integers defined for your compiler
# endif
#endif // end of #ifdef COMP_GCC

#else
// we're happy and can simply use stdint.h
#define __STDC_CONSTANT_MACROS
#define __STDC_LIMIT_MACROS
#include <stdint.h>
typedef uint8_t uint8;
typedef int8_t int8;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint32_t uint32;
typedef int32_t int32;
typedef uint64_t uint64;
typedef int64_t int64;
#define CONST_INT64(x) INT64_C(x)
#define CONST_UINT64(x) UINT64_C(x)
#endif

#endif // end of #if !defined(CS_BUILTIN_SIZED_TYPES)
/** @} */

/**\name Pointer-sized integer types.
 * These are signed and unsigned integer types with the same size as a
 * platform pointer. Use them in case you need to cast a pointer to an int
 * (although it's better to avoid such casts.)
 * @{ */
#ifdef CS_PLATFORM_IS_64BITS
/// Signed integer with the size of a pointer
typedef int64 ptr_int;
/// Unsigned integer with the size of a pointer
typedef uint64 ptr_uint;
#else
#if defined(COMP_VC) && _MSC_VER >= 1300
/*
  Without that __w64 thingie, MSVC >= 7 will report a C4311 warning when
  using ptr_(u)int as intended.
 */
/// Signed integer with the size of a pointer
typedef __w64 int32 ptr_int;
/// Unsigned integer with the size of a pointer
typedef __w64 uint32 ptr_uint;
#else
/// Signed integer with the size of a pointer
typedef int32 ptr_int;
/// Unsigned integer with the size of a pointer
typedef uint32 ptr_uint;
#endif
#endif
/** @} */

/// Used for uniquely generated id numbers XXX: remove this sometime
typedef uint32 CS_ID;

/// Time in milli (1/1000) seconds XXX: should be moved to sysfunc.h
typedef unsigned int csTicks;

/**\name Shortcuts for normal C types
 * @{ */
/// Default unsigned int.
typedef unsigned int uint;
/** @} */

/** @} */

#endif // __CS_CSTYPES_H__
