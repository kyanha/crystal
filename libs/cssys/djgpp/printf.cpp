/*
    DOS support for Crystal Space 3D library
    Copyright (C) 1998 by Jorrit Tyberghein
    Written by David N. Arnold <derek_arnold@fuse.net>
    Written by Andrew Zabolotny <bit@eltech.ru>

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

// In DOS we do not want printf() to work in graphics mode
// since it shows garbage on screen

#include <stdarg.h>
#include <stdio.h>

#include "sysdef.h"
#include "cssys/common/system.h"

static bool printf_initialized = false;

// to be called before all pprintf() calls
void csSystemDriver::printf_init(void)
{
  printf_initialized = true;
}

// to be called before shutdown
void csSystemDriver::printf_close(void)
{
  printf_initialized = false;
}

static bool ShutUp = false;

void printf_Enable (bool Enable)
{
  ShutUp = !Enable;
}

// to be called instead of printf (exact same prototype/functionality of printf)
int csSystemDriver::printf (const char *str, ...)
{
  if (ShutUp)
    return 0;

  va_list arg;
  char buf[1024];

  va_start (arg, str);
  vsprintf (buf, str, arg);
  va_end (arg);

  return ::printf("%s",buf);
}
