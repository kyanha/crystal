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

#ifndef __CS_UTIL_H__
#define __CS_UTIL_H__

#include <stdio.h>
#include "cssys/csunicode.h"

/**\file
 */

/**\addtogroup util
 * @{ */
 
/**
 * Allocate a new char [] and copy the string into the newly allocated 
 * storage.
 * This is a handy method for copying strings, in fact it is the C++ analogue
 * of the strdup() function from string.h (strdup() is not present on some
 * platforms). To free the pointer the caller should call delete[].
 */
extern char *csStrNew (const char *s);
/**
 * Allocate a new char [] and copy an UTF-8 version of the string into 
 * the newly allocated storage.
 */
extern char *csStrNew (const wchar_t *s);
/**
 * Allocate a new widechar [] and the string into the newly allocated storage.
 */
extern wchar_t* csStrNewW (const wchar_t *s);
/**
 * Allocate a new widechar [] and copy the string converted from UTF-8 into 
 * the newly allocated storage.
 */
extern wchar_t* csStrNewW (const char *s);

/**
 * Helper class to convert widechar* to char*(UTF-8) strings for use
 * as function parameters.
 * Use of this helper class is more convenient than an a 
 * csStrNewW() / delete[], but essentially does the same.
 * \code
 *   widechar* wstr = L"Hello World";
 *    ... 
 *   iNativeWindow* natwin =  ... ;
 *     natwin->SetTitle (csWtoC (wstr));
 * \endcode
 */
struct csWtoC
{
private:
  char* s;
public:
  /**
   * Constructor.
   * Stores an UTF-8 converted version of \p ws internally.   
   */
  csWtoC (const wchar_t* ws)
  { s = csStrNew (ws); }
  /**
   * Deletes the internally stored string.
   */
  ~csWtoC ()
  { delete[] s; }
  /**
   * Retrieve the converted string.
   */
  operator const char* () const
  { return s; }
};

/**
 * Helper class to convert (UTF-8)widechar* to char* strings for use
 * as function parameters.
 * Use of this helper class is more convenient than an a 
 * csStrNewW() / delete[], but essentially does the same.
 */
struct csCtoW
{
private:
  wchar_t* ws;
public:
  /**
   * Constructor.
   * Stores an UTF-16 converted version of \p s internally.   
   */
  csCtoW (const char* s)
  { ws = csStrNewW (s); }
  /**
   * Deletes the internally stored string.
   */
  ~csCtoW ()
  { delete[] ws; }
  /**
   * Retrieve the converted string.
   */
  operator const wchar_t* () const
  { return ws; }
};

/**
 * Expand a filename if it contains shortcuts.
 * Currently the following macros are recognised and expanded:
 * <pre>
 * '.', '~', '..', 'drive:' (on DOS/Win32/OS2)
 * </pre>
 * The returned filename is always absolute, i.e. it always starts
 * from root. Return a string allocated with csStrNew().
 */
extern char *csExpandName (const char *iName);

/**
 * Split a pathname into separate path and name. Path delimiters are either
 * '/', PATH_SEPARATOR and, for OS/2, MS-DOS and Win32 targets, ':'.
 */
extern void csSplitPath (const char *iPathName, char *oPath, size_t iPathSize,
  char *oName, size_t iNameSize);

/**
 * This is a really simple function that does very nice
 * "filename against filemask" comparisons. It understands
 * even such things like "*a*.txt" or "*a?b*" or even "*"
 * (wish I DOS could do it ...). No "[]" wildcards though :-)
 * <p>
 * \remark If you want case-insensitive comparison, upcase (or lowercase,
 *   depends on personal taste) strings first.
 */
extern bool csGlobMatches (const char *fName, const char *fMask);

/**
 * Finds the smallest number that is a power of two and is larger or
 * equal to n.
 */
int csFindNearestPowerOf2 (int n);

/// returns true if n is a power of two
bool csIsPowerOf2 (int n);

/// Find the log2 of argument
static inline int csLog2 (int n)
{
  int l = 31; unsigned x = 0x80000000;
  while (x && !(n & x))
    l--, x >>= 1;
  return l;
}

/**
 * given src and dest, which are already allocated, copy source to dest.
 * But, do not copy 'search', instead replace that with 'replace' string.
 * max is size of dest.
 */
void csFindReplace (char *dest, const char *src, const char *search,
  const char *replace, int max);

/** @} */
  
#endif // __CS_UTIL_H__
