/*
    Copyright (C) 1998 by Jorrit Tyberghein
    Largely rewritten by Ivan Avramovic <ivan@avramovic.com>

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

#ifndef __CS_FASTSQRT_H__
#define __CS_FASTSQRT_H__

/**\file 
 */
/**
 * \addtogroup geom_utils
 * @{ */

#include "csextern.h"


#ifdef CS_COMPILER_GCC
  #warning FastSqrt is deprecated. Use sqrtf instead.
#endif
#ifdef CS_COMPILER_MSVC
  #pragma message ("FastSqrt is deprecated. Use sqrtf instead.")
#endif

/// declare table of square roots
CS_CRYSTALSPACE_EXPORT void BuildSqrtTable ()
{}
/// fast square root, looks up table build by BuildSqrtTable().
CS_CRYSTALSPACE_EXPORT float FastSqrt (float n)
{
  return sqrtf(n);
}

/** @} */

#endif // __CS_FASTSQRT_H__

