/*
    Generic Console Output Support
    Copyright (C) 1998-2001 by Jorrit Tyberghein

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

#include <stdarg.h>
#include <stdio.h>
#include "cssysdef.h"
#include "csutil/sysfunc.h"

// Replacement for printf(); exact same prototype/functionality as printf()
int csPrintf(char const* str, ...)
{
  va_list args;
  va_start (args, str);
  int const rc = vprintf(str, args);
  va_end (args);
  return rc;
}

// Replacement for vprintf()
int csPrintfV(char const* str, va_list args)
{
  return vprintf(str, args);
}

int csFPutErr (const char* str)
{
  return fputs (str, stderr);
}

