/*
    Copyright (C) 2000 by Andrew Zabolotny, <bit@eltech.ru>

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

#ifndef __BITSET_H__
#define __BITSET_H__

#include <stdlib.h>
#include <string.h>

/// How many bits there are in one byte (usually 8 :-)
#define BITS_PER_BYTE	8
/// Max value for a byte
#define MAX_BYTE_VALUE	0xff

// Processor-dependent macros
#if defined (PROC_INTEL)
#  define IMPLEMENT_BIT_SET \
   asm ("bts %1,%0" : : "o" (*bits), "r" (index));
#  define IMPLEMENT_BIT_RESET \
   asm ("btr %1,%0" : : "o" (*bits), "r" (index));
#  define IMPLEMENT_BIT_GET \
   bool result; \
   asm ("bt %2,%1\nsetc %%al" : "=a" (result) : "o" (*bits), "r" (index)); \
   return result;
#else
#  define IMPLEMENT_BIT_SET \
   bits [index / BITS_PER_BYTE] |= (1 << (index % BITS_PER_BYTE));
#  define IMPLEMENT_BIT_RESET \
   bits [index / BITS_PER_BYTE] &= ~(1 << (index % BITS_PER_BYTE));
#  define IMPLEMENT_BIT_GET \
   return !!(bits [index / BITS_PER_BYTE] & (1 << (index % BITS_PER_BYTE)));
#endif

/**
 * A BitSet is an array of bits. The csBitSet class allow you to
 * allocate, resize and manipulate such an array.
 *<p>
 * The bit set is a highly effective way to store an array of booleans.
 * The implementation uses assembly whenever possible, and most methods
 * are inline, thus it is highly recommended to use it whenever possible.
 */
class csBitSet
{
  unsigned size;
  unsigned char *bits;
public:
  /// Create bit set of given size
  csBitSet (unsigned iSize)
  {
    size = (iSize + BITS_PER_BYTE - 1) / BITS_PER_BYTE;
    bits = (unsigned char *)malloc (size);
  }

  /// Destroy the bit array
  ~csBitSet ()
  { free (bits); }

  /// Query bit set size
  unsigned GetSize ()
  { return size; }

  /// Resize the array
  void Resize (unsigned iSize)
  {
    size = (iSize + BITS_PER_BYTE - 1) / BITS_PER_BYTE;
    bits = (unsigned char *)realloc (bits, size);
  }

  /// Get a pointer to entire array for custom manipulations
  unsigned char *GetBits ()
  { return bits; }

  /// Clear the entire array
  inline void Reset ()
  { memset (bits, 0, size); }

  /// Set all the bits to one
  inline void Set ()
  { memset (bits, MAX_BYTE_VALUE, size); }

  /// Set a bit in the array
  inline void Set (unsigned index)
  { IMPLEMENT_BIT_SET }

  /// Set a number of bits in the array, starting with given index
  inline void Set (unsigned index, unsigned count)
  {
    unsigned char *start = bits + index / BITS_PER_BYTE;
    unsigned char smask = MAX_BYTE_VALUE << (index % BITS_PER_BYTE);
    unsigned char *end = bits + (index + count) / BITS_PER_BYTE;
    unsigned char emask = MAX_BYTE_VALUE >> (8 - (index + count) % BITS_PER_BYTE);
    if (start == end)
      *start |= smask & emask;
    else
    {
      *start++ |= smask;
      while (start < end)
        *start++ = MAX_BYTE_VALUE;
      if (emask) *start |= emask;
    }
  }

  /// Reset a bit in the array
  inline void Reset (unsigned index)
  { IMPLEMENT_BIT_RESET }

  /// Set a number of bits in the array, starting with given index
  inline void Reset (unsigned index, unsigned count)
  {
    unsigned char *start = bits + index / BITS_PER_BYTE;
    unsigned char smask = MAX_BYTE_VALUE >> (8 - index % BITS_PER_BYTE);
    unsigned char *end = bits + (index + count) / BITS_PER_BYTE;
    unsigned char emask = MAX_BYTE_VALUE << ((index + count) % BITS_PER_BYTE);
    if (start == end)
      *start &= smask | emask;
    else
    {
      *start++ &= smask;
      while (start < end)
        *start++ = 0;
      if (emask != MAX_BYTE_VALUE) *start &= emask;
    }
  }

  /// Get the value of a bit in the array
  inline bool Get (unsigned index)
  { IMPLEMENT_BIT_GET }

  /// OR two bit sets together
  inline csBitSet &operator |= (csBitSet &bs)
  {
    unsigned sz = MIN (size, bs.size);
    ULong *src = (ULong *)bits;
    ULong *dst = (ULong *)bs.bits;
    while (sz >= sizeof (ULong))
    {
      *src++ |= *dst++;
      sz -= sizeof (ULong);
    }
    while (sz--)
      *((UByte *)src)++ |= *((UByte *)dst)++;
    return *this;
  }

  /// AND two bit sets together
  inline csBitSet &operator &= (csBitSet &bs)
  {
    unsigned sz = MIN (size, bs.size);
    ULong *src = (ULong *)bits;
    ULong *dst = (ULong *)bs.bits;
    while (sz >= sizeof (ULong))
    {
      *src++ &= *dst++;
      sz -= sizeof (ULong);
    }
    while (sz--)
      *((UByte *)src)++ &= *((UByte *)dst)++;
    return *this;
  }
};

#endif // __BITSET_H__
