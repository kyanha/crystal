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

#ifdef CS_COMPILER_GCC
  #warning csutil/subrec2.h is deprecated; use csutil/subrec.h instead
#endif
#ifdef CS_COMPILER_MSVC
  #pragma message ("csutil/subrec2.h is deprecated; use csutil/subrec.h instead")
#endif

#define csSubRect2		csSubRect
#define csSubRectangles2	csSubRectangles

#include "subrec.h"
