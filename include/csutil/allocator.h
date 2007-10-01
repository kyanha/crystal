/*
    Copyright (C) 2006-2007 by Frank Richter

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

#ifndef __CS_CSUTIL_ALLOCATOR_H__
#define __CS_CSUTIL_ALLOCATOR_H__

/**\file
 * Basic allocator classes.
 */

#include "csutil/alignedalloc.h"
#include "csutil/memdebug.h"

/**\addtogroup util_memory
 * @{ */

namespace CS
{
  namespace Memory
  {
    /**\page Allocators Memory allocators
     * Several Crystal Space utility classes take optional memory allocators
     * (e.g. csArray<>). This allows clients to customize the memory 
     * allocation strategy used in a particular case. 
    
     * For example, if you know that an array has a size below a certain 
     * threshold most of the time, you could utilize a LocalBufferAllocator<>
     * which return memory from a fast local buffer, if possible - this is 
     * faster than using the heap.
     *
     * As another example, when memory allocations follow a pattern where
     * long-lived allocations are mixed with very short-lived allocations,
     * heap fragmentation can occur when the short-lived allocations are 
     * freed. In that case it's beneficial to use a separate heap for the
     * short-lived allocations; specific objects can use that heap via the
     * allocator customization.
     *
     * Allocators must obey the following methods and their semantics:
     * - <tt>void* Alloc(const size_t n)</tt> allocates \a n bytes of memory.
     *   Returns 0 if allocation fails.
     * - <tt>void Free (void* p)</tt> frees the pointer \a p. For debugging
     *   purposes it's usually a good idea to throw an assertion when \a p is
     *   invalid for an allocator.
     * - <tt>void* Realloc (void* p, size_t newSize)</tt> "resizes" the block
     *   at \a p to have a size of \a newSize bytes. If \a p is 0 then the call
     *   should have the same effect as an Alloc() with the specified size.
     *   Be aware that the returned pointer may be different from \a p! Also, 
     *   if the reallocation fails, 0 is returned - however, the original 
     *   memory block is still valid!
     * - <tt>void SetMemTrackerInfo (const char* info)</tt> is a debugging 
     *   aid. If the allocator does memory tracking, some users will call
     *   this method to set a description of themselves which is displayed
     *   to allow easy identification of what object allocated how much
     *   memory. Implementation of this method is entirely optional and can be
     *   safely stubbed out.
     */
    
    /**
     * A default memory allocator that allocates with cs_malloc().
     */
    class AllocatorMalloc
    {
    #ifdef CS_MEMORY_TRACKER
      /// Memory tracking info
      csMemTrackerInfo* mti;
    #endif
    public:
    #ifdef CS_MEMORY_TRACKER
      AllocatorMalloc() : mti (0) {}
    #endif
      /// Allocate a block of memory of size \p n.
      CS_ATTRIBUTE_MALLOC void* Alloc (const size_t n)
      {
      #ifdef CS_MEMORY_TRACKER
	size_t* p = (size_t*)cs_malloc (n + sizeof (size_t));
	*p = n;
	p++;
	if (mti) mtiUpdateAmount (mti, 1, int (n));
	return p;
      #else
	return cs_malloc (n);
      #endif
      }
      /// Free the block \p p.
      void Free (void* p)
      {
      #ifdef CS_MEMORY_TRACKER
	size_t* x = (size_t*)p;
	x--;
	size_t allocSize = *x;
	cs_free (x);
	if (mti) mtiUpdateAmount (mti, -1, -int (allocSize));
      #else
	cs_free (p);
      #endif
      }
      /// Resize the allocated block \p p to size \p newSize.
      void* Realloc (void* p, size_t newSize)
      {
      #ifdef CS_MEMORY_TRACKER
        if (p == 0) return Alloc (newSize);
	size_t* x = (size_t*)p;
	x--;
	if (mti) mtiUpdateAmount (mti, -1, -int (*x));
	size_t* np = (size_t*)cs_realloc (x, newSize + sizeof (size_t));
	*np = newSize;
	np++;
	if (mti) mtiUpdateAmount (mti, 1, int (newSize));
	return np;
      #else
	return cs_realloc (p, newSize);
      #endif
      }
      /// Set the information used for memory tracking.
      void SetMemTrackerInfo (const char* info)
      {
      #ifdef CS_MEMORY_TRACKER
	mti = mtiRegister (info);
      #else
	(void)info;
      #endif
      }
    };

    /**
     * An allocator with a small local buffer.
     * If the data fits into the local buffer (which is set up to holds 
     * \c N elements of type \c T), a memory allocation from the heap is saved.
     * Thus, if you have lots of arrays with a relatively small, well known size
     * you can gain some performance by using this allocator.
     * \c SingleAllocation specifies whether no more than a single block is
     * allocated from the allocator at any time. Using that option saves 
     * (albeit a miniscule amount of) memory, but obviously is only safe when
     * it's know that the single allocation constraint is satisfied (such as 
     * allocators for csArray<>s).
     *
     * \warning The pointer returned may point into the instance data; be 
     *  careful when moving that around - an earlier allocated pointer may
     *  suddenly become invalid!
     * \remark This means that if you use this allocator with an array, and you 
     *  nest that into another array, you MUST use 
     *  csSafeCopyArrayElementHandler for  the nesting array!
     */
    template<typename T, size_t N, class ExcessAllocator = AllocatorMalloc,
      bool SingleAllocation = false>
    class LocalBufferAllocator : public ExcessAllocator
    {
      static const size_t localSize = N * sizeof (T);
      static const uint8 freePattern = 0xfa;
      static const uint8 newlyAllocatedSalt = 0xac;
      uint8 localBuf[localSize + (SingleAllocation ? 0 : 1)];
    public:
      LocalBufferAllocator ()
      {
        if (SingleAllocation) 
        {
      #ifdef CS_DEBUG
          memset (localBuf, freePattern, localSize);
      #endif
        }
        else
          localBuf[localSize] = 0;
      }
      LocalBufferAllocator (const ExcessAllocator& xalloc) : 
        ExcessAllocator (xalloc)
      {
        if (SingleAllocation) 
        {
      #ifdef CS_DEBUG
          memset (localBuf, freePattern, localSize);
      #endif
        }
        else
          localBuf[localSize] = 0;
      }
      T* Alloc (size_t allocSize)
      {
        if (SingleAllocation)
        {
      #ifdef CS_DEBUG
          /* Verify that the local buffer consists entirely of the "local
             buffer unallocated" pattern. (Not 100% safe since a valid 
             allocated buffer may be coincidentally filled with just that
             pattern, but let's just assume that it's unlikely.) */
          bool validPattern = true;
          for (size_t n = 0; n < localSize; n++)
          {
            if (localBuf[n] != freePattern)
            {
              validPattern = false;
              break;
            }
          }
          CS_ASSERT_MSG("This LocalBufferAllocator only allows one allocation "
            "a time!", validPattern);
          memset (localBuf, newlyAllocatedSalt, localSize);
      #endif
	  if (allocSize <= localSize)
	    return (T*)localBuf;
	  else
          {
            void* p = ExcessAllocator::Alloc (allocSize);
	    return (T*)p;
          }
        }
        else
        {
          void* p;
	  if ((allocSize <= localSize) && !localBuf[localSize])
          {
            localBuf[localSize] = 1;
	    p = localBuf;
        #ifdef CS_DEBUG
            memset (p, newlyAllocatedSalt, allocSize);
        #endif
          }
	  else
          {
            p = ExcessAllocator::Alloc (allocSize);
          }
          return (T*)p;
        }
      }
    
      void Free (T* mem)
      {
        if (SingleAllocation)
        {
          if (mem != (T*)localBuf)
	    ExcessAllocator::Free (mem);
	  else
	  {
      #ifdef CS_DEBUG
	    /* Verify that the local buffer does entirely consist of the "local
	       buffer unallocated" pattern. (Not 100% safe since a valid 
	       allocated buffer may be coincidentally filled with just that
	       pattern, but let's just assume that it's unlikely.) */
	    bool validPattern = true;
	    for (size_t n = 0; n < localSize; n++)
	    {
	      if (localBuf[n] != freePattern)
	      {
		validPattern = false;
		break;
	      }
	    }
	    CS_ASSERT_MSG("Free() without prior allocation", !validPattern);
      #endif
	  }
      #ifdef CS_DEBUG
	  memset (localBuf, freePattern, localSize);
      #endif
        }
        else
        {
          if (mem != (T*)localBuf) 
            ExcessAllocator::Free (mem);
          else
          {
            localBuf[localSize] = 0;
          }
        }
      }
    
      // The 'relevantcount' parameter should be the number of items
      // in the old array that are initialized.
      void* Realloc (void* p, size_t newSize)
      {
        if (p == 0) return Alloc (newSize);
        if (p == localBuf)
        {
          if (newSize <= localSize)
            return p;
          else
          {
	    p = ExcessAllocator::Alloc (newSize);
	    memcpy (p, localBuf, localSize);
        #ifdef CS_DEBUG
            memset (localBuf, freePattern, localSize);
        #endif
            if (!SingleAllocation) localBuf[localSize] = 0;
	    return p;
          }
        }
        else
        {
          if ((newSize <= localSize) && !localBuf[localSize])
	  {
	    memcpy (localBuf, p, newSize);
	    ExcessAllocator::Free (p);
            if (!SingleAllocation) localBuf[localSize] = 1;
	    return localBuf;
	  }
	  else
	    return ExcessAllocator::Realloc (p, newSize);
        }
      }

      using ExcessAllocator::SetMemTrackerInfo;
    };
    
    /**
     * This class implements an allocator policy which aligns the first
     * element on given byte boundary.  It has a per-block overhead of
     * `sizeof(void*)+alignment' bytes.
     */
    template <size_t A = 1>
    class AllocatorAlign
    {
    public:
      /**
       * Allocate a raw block of given size. 
       */
      static inline CS_ATTRIBUTE_MALLOC void* Alloc (size_t size) 
      {
        return AlignedMalloc (size, A);
      }

      /**
       * Free a block.
       * \remarks Does not check that the block pointer is valid.
       */
      static inline void Free (void* p)
      {
        AlignedFree (p);
      }

      void* Realloc (void* p, size_t newSize)
      {
        return AlignedRealloc (p, newSize, A);
      }

      void SetMemTrackerInfo (const char* info)
      {
	(void)info;
      }
    };

    /**
     * A default memory allocator that allocates using new char[].
     * \c Reallocatable specifies whether Realloc() should be supported.
     * (This support incurs an overhead as the allocated size has to be stored.)
     */
    template<bool Reallocatable = true>
    class AllocatorNewChar
    {
    #ifdef CS_MEMORY_TRACKER
      /// Memory tracking info
      csMemTrackerInfo* mti;
    #endif
    public:
    #ifdef CS_MEMORY_TRACKER
      AllocatorNewChar() : mti (0) {}
    #endif
      /// Allocate a block of memory of size \p n.
      CS_ATTRIBUTE_MALLOC void* Alloc (const size_t n)
      {
        // Note that with memtracking we store the alloc'ed size anyway.
      #ifndef CS_MEMORY_TRACKER
        if (!Reallocatable)
        {
          return new char[n];
        }
      #endif
	size_t* p = (size_t*)new char[n + sizeof (size_t)];
	*p = n;
	p++;
      #ifdef CS_MEMORY_TRACKER
	if (mti) mtiUpdateAmount (mti, 1, int (n));
      #endif
	return p;
      }
      /// Free the block \p p.
      void Free (void* p)
      {
      #ifndef CS_MEMORY_TRACKER
        if (!Reallocatable)
        {
          delete[] (char*)p;
          return;
        }
      #endif
	size_t* x = (size_t*)p;
	x--;
	size_t allocSize = *x;
	delete[] (char*)x;
      #ifdef CS_MEMORY_TRACKER
	if (mti) mtiUpdateAmount (mti, -1, -int (allocSize));
      #else
        (void)allocSize;
      #endif
      }
      /// Resize the allocated block \p p to size \p newSize.
      void* Realloc (void* p, size_t newSize)
      {
        if (!Reallocatable) return 0;
	  
        if (p == 0) return Alloc (newSize);
	size_t* x = (size_t*)p;
	x--;
        size_t oldSize = *x;
      #ifdef CS_MEMORY_TRACKER
	if (mti) mtiUpdateAmount (mti, -1, -int (oldSize));
      #endif
	size_t* np = (size_t*)Alloc (newSize);
        if (newSize < oldSize)
          memcpy (np, p, newSize);
        else
          memcpy (np, p, oldSize);
        Free (p);
      #ifdef CS_MEMORY_TRACKER
	if (mti) mtiUpdateAmount (mti, 1, int (newSize));
      #endif
	return np;
      }
      /// Set the information used for memory tracking.
      void SetMemTrackerInfo (const char* info)
      {
      #ifdef CS_MEMORY_TRACKER
	mti = mtiRegister (info);
      #else
	(void)info;
      #endif
      }
    };

    /**
     * A default memory allocator that allocates using new T[].
     * \warning Using this allocator is somewhat dangerous, as an array
     *   of Ts to fit the requested size is allocated, and the array is
     *   casted to a void*. Using anything but POD types is irresponsible.
     *   Don't use this allocator unless you really, \em really have to - for
     *   example, when memory is passed from/to code which uses 
     *   new[]/delete[] of T for allocation/deallocation.
     * \remarks Reallocatability is inherently unsupported.
     */
    template<typename T>
    class AllocatorNew
    {
    public:
      /// Allocate a block of memory of size \p n.
      CS_ATTRIBUTE_MALLOC void* Alloc (const size_t n)
      {
        return new T[((n + sizeof(T) - 1) / sizeof(T)) * sizeof(T)];
      }
      /// Free the block \p p.
      void Free (void* p)
      {
        delete[] (T*)p;
      }
      /// Resize the allocated block \p p to size \p newSize.
      void* Realloc (void* p, size_t newSize)
      {
        CS_ASSERT_MSG("Realloc() called on AllocatorNew", false);
	return 0;
      }
      /// Set the information used for memory tracking.
      void SetMemTrackerInfo (const char* /*info*/)
      {
      }
    };

    
    /**
     * Class to store a pointer that is allocated from Allocator,
     * to eliminate overhead from a possibly empty Allocator.
     * See http://www.cantrip.org/emptyopt.html for details.
     */
    template<typename T, typename Allocator>
    struct AllocatorPointerWrapper : public Allocator
    {
      /// The allocated pointer.
      T* p;

      AllocatorPointerWrapper () {}
      AllocatorPointerWrapper (const Allocator& alloc) : 
        Allocator (alloc) {}
      AllocatorPointerWrapper (T* p) : p (p) {}
      AllocatorPointerWrapper (const Allocator& alloc, T* p) : 
        Allocator (alloc), p (p) {}
    };

  } // namespace Memory
} // namespace CS

/** @} */

#endif // __CS_CSUTIL_ALLOCATOR_H__
