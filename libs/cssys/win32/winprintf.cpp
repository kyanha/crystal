/*
    Copyright (C) 2003 by Frank Richter

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

#include "cssysdef.h"
#include "cssys/csunicode.h"
#include "csutil/util.h"
#include "csutil/snprintf.h"
#include "cssys/sysfunc.h"

#include <windows.h>
#include "cssys/win32/wintools.h"

static int _cs_fputs (const char* string, FILE* stream)
{
  int rc;
  if ((stream == stdout) || (stream == stderr))
  {
    UINT cp;
    if ((cp = GetConsoleOutputCP ()) != CP_UTF8)
    {
      // The UTF-8 text has to be converted to the output codepage.
    
      rc = fputs (cswinCtoA (string, cp), stream);
    }
    else
    {
      // Not much to do - the text can be dumped to the console,
      // Windows will care about proper output.
      rc = fputs (string, stream);
    }
  }
  else
  {
    rc = fputs (string, stream);
  }
  return rc;
}

static int _cs_vfprintf (FILE* stream, const char* format, va_list args)
{
  int rc;

  int bufsize, newsize = 128;
  char* str = 0;
  
  do
  {
    delete[] str;
    bufsize = newsize + 1;
    str = new char[bufsize];
    // use cs_vsnprintf() for consistency across all platforms.
    newsize = cs_vsnprintf (str, bufsize, format, args);
  }
  while (bufsize < (newsize + 1));

  // _cs_fputs() will do codepage convertion, if necessary
  rc = _cs_fputs (str, stream);
  delete[] str;
  // On success fputs() returns a value >= 0, but
  // printf() should return the number of chars written.
  return ((rc >= 0) ? (newsize - 1) : -1);
}

// Replacement for printf(); exact same prototype/functionality as printf()
int csPrintf(char const* str, ...)
{
  va_list args;
  va_start(args, str);
  int const rc = _cs_vfprintf (stdout, str, args);
  va_end(args);
  return rc;
}

// Replacement for vprintf()
int csPrintfV(char const* str, va_list args)
{
  return _cs_vfprintf (stdout, str, args);
}

int csFPutErr (const char* str)
{
  return _cs_fputs (str, stderr);
}

