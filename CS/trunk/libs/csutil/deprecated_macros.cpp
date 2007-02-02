/*
    Copyright (C) 2007 by Frank Richter

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

/* Implementations for functions declared for the "inline function" trick
 * to have deprecated macros emit a message.
 */

#include "cssysdef.h"
#undef CS_TEMP_DIR
#undef CS_TEMP_FILE
#include "csutil/syspath.h"

namespace CS
{
  namespace Macros
  {
    csString CS_TEMP_DIR ()
    {
      return Platform::GetTempDirectory ();
    }
    csString CS_TEMP_FILE ()
    {
      return Platform::GetTempFilename (0);
    }
  }
}

