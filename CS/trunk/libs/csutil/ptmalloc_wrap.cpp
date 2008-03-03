/*
    Copyright (C) 2006 by Frank Richter

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
#include "csutil/array.h"
#include "csutil/callstack.h"
#include "csutil/csendian.h"
#include "csutil/ref.h"
#include "csutil/scopedlock.h"
#include "csutil/threading/atomicops.h"
#include "csutil/threading/mutex.h"

// ptmalloc functions
namespace CS
{
  namespace ptmalloc_
  {
    extern "C"
    {
      extern void* ptmalloc (size_t n);
      extern void ptfree (void* p);
      extern void* ptrealloc (void* p, size_t n);
      extern void* ptcalloc (size_t n, size_t s);
    }
  }
}

using namespace CS;

void* ptmalloc (size_t n)
{
  return ptmalloc_::ptmalloc (n); 
}
void ptfree (void* P)
{ 
  ptmalloc_::ptfree (P); 
}
void* ptrealloc (void* P, size_t n)
{ 
  return ptmalloc_::ptrealloc (P, n); 
}
void* ptcalloc (size_t n, size_t s)
{ 
  return ptmalloc_::ptcalloc (n, s); 
}

namespace
{
  /* For sentinel allocations, a small "cookie" is placed in front and after 
   * the memory returned by the allocators in order to detect corruption, and,
   * since the cookie value is different per module, freeing memory across 
   * modules. */
  typedef uint32 CookieType;
  static CookieType cookie;
  CS_FORCEINLINE static CookieType CookieSwap (CookieType x)
  {
    return csSwapBytes::UInt32 (x);
  }
  CS_FORCEINLINE static CookieType GetCookie (void* p)
  {
    return CookieType (intptr_t (&cookie) ^ intptr_t (p));
  }
  static const size_t cookieOverhead = 
    sizeof (size_t) + 2*sizeof (CookieType);
  // Maximum allocatable size, to avoid wraparound when the cookies are added
  static const size_t maxRequest = (~(size_t)0) - cookieOverhead;
  
  static const uint32 indicator = 0x58585858;

  class AllocatorMallocPlatform
  {
  public:
    /// Allocate a block of memory of size \p n.
    CS_ATTRIBUTE_MALLOC void* Alloc (const size_t n)
    {
      return malloc (n);
    }
    /// Free the block \p p.
    void Free (void* p)
    {
      free (p);
    }
    /// Resize the allocated block \p p to size \p newSize.
    void* Realloc (void* p, size_t newSize)
    {
      return realloc (p, newSize);
    }
    /// Set the information used for memory tracking.
    void SetMemTrackerInfo (const char* info)
    {
      (void)info;
    }
  };
  
  struct AllocatedBlock
  {
    void* address;
    size_t size;
    csRef<csCallStack> stack;
    
    bool operator<(const AllocatedBlock& other) const
    {
      return address < other.address;
    }
  };
  
  static csArray<AllocatedBlock, csArrayElementHandler<AllocatedBlock>, 
    AllocatorMallocPlatform> allocatedPointers;
  static CS::Threading::RecursiveMutex allocatedPointersMutex;
  
  static int AllocatedBlockKeyCompare (const AllocatedBlock& block, 
                                       void* const & key)
  {
    if (block.address < key)
      return -1;
    else if (block.address > key)
      return 1;
    else
      return 0;
  }
  
  static const AllocatedBlock* FindAllocatedBlock (void* P)
  {
    size_t index = allocatedPointers.FindSortedKey (
      csArrayCmp<AllocatedBlock, void*> (P, AllocatedBlockKeyCompare));
    if (index == csArrayItemNotFound) return 0;
    return &(allocatedPointers[index]);
  }
  
  static void DumpAllocatedBlocks (FILE* f)
  {
    for (size_t i = 0; i < allocatedPointers.GetSize(); i++)
    {
      const AllocatedBlock& block = allocatedPointers[i];
      
      fprintf (f, ">>> %p %lu\n", 
        block.address, (unsigned long)block.size);
      block.stack->Print (f);
      fflush (f);
    }
  }

  template<bool keepLocation>
  static bool mem_check_real (const char* msg, bool condition,
                              const char* exprString,
                              csCallStack* stack, 
                              const char* file, int line)
  {
    if (!condition)
    {
      if (keepLocation && (stack != 0))
      {
	fprintf (stderr, 
	  "Memory error:     %s\n", exprString);
	fprintf (stderr, 
	  "Message:          %s\n", msg);
	fflush (stderr);
	
	FILE* out = fopen ("allocations.txt", "w");
	if (out != 0)
	{
	  DumpAllocatedBlocks (out);
	  fclose (out);
	}
      
	fprintf (stderr, "Call stack @ %p:\n", stack);
	fflush (stderr);
	stack->Print (stderr);
	fflush (stderr);
  
        CS::Debug::DebugBreak();
      }
      else
      {
	/* Note: Though this function can be called inside AssertMessage
	 * through VerifyAllMemory, it will not re-enter AssertMessage in
	 * that case since a stack is provided.
	 */
	FILE* out = fopen ("allocations.txt", "w");
	if (out != 0)
	{
	  DumpAllocatedBlocks (out);
	  fclose (out);
	}
	
        CS::Debug::AssertMessage (exprString, file, line, msg);
      }
      return false;
    }
    return true;
  }
  #define mem_check(keepLocation, msg, condition, stack, file, line) \
    mem_check_real<keepLocation> (msg, condition, #condition, stack, file, line)
  
  static bool VerifyMemoryCookie (const AllocatedBlock& block)
  {
    bool ret = true;
    
    // Compute original allocated address
    uint8* p = (uint8*)block.address;
    p -= sizeof(CookieType);
    const CookieType startCookie = GetCookie (p
      - (sizeof(size_t) + sizeof (indicator)));
    const CookieType endCookie = CookieSwap (startCookie);
    
    CookieType theCookie = *(CookieType*)p;
    p -= sizeof(size_t);
    size_t n = *((size_t*)p);
    p -= sizeof(indicator);
    
    // Verify cookies
    ret &= mem_check (true,
      "Memory block has wrong cookie "
      "(was probably allocated in another module)",
      theCookie == startCookie, block.stack, __FILE__, __LINE__);
    ret &= mem_check (true,
      "Memory block has wrong cookie "
      "(probably corrupted by an overflow)",
      *(CookieType*)((uint8*)block.address + n) == endCookie, block.stack, 
      __FILE__, __LINE__);
    
    return ret;
  }
}

namespace CS
{
  namespace Debug
  {
    bool VerifyAllMemory ()
    {
      bool ret = true;
      CS::Threading::ScopedLock<CS::Threading::RecursiveMutex> lock (
        allocatedPointersMutex);
      for (size_t i = 0; i < allocatedPointers.GetSize(); i++)
      {
        ret &= VerifyMemoryCookie (allocatedPointers[i]);
      }
      return ret;
    }
    
    void DumpAllocateMemoryBlocks ()
    {
      FILE* out = fopen ("allocations.txt", "w");
      if (out != 0)
      {
	DumpAllocatedBlocks (out);
	fclose (out);
      }
    }
  } // Debug
} // namespace CS

namespace
{
  static const int32 verifyFreq = 4000;
  static int32 remainingActions = verifyFreq;

  template<bool keepLocation, bool doCheck>
  static void* ptmalloc_debug (size_t n)
  {
    if (doCheck)
    {
      if (CS::Threading::AtomicOperations::Decrement (&remainingActions) == 0)
      {
        CS::Debug::VerifyAllMemory ();
        CS::Threading::AtomicOperations::Set (&remainingActions, verifyFreq);
      }
    }
    
    if (n > maxRequest)
    {
      errno = ENOMEM;
      return 0;
    }
    size_t extraData = sizeof (size_t) + 2*sizeof (CookieType);
    if (keepLocation) extraData += sizeof (indicator);
    uint8* p = (uint8*)ptmalloc_::ptmalloc (n + extraData);
    const CookieType startCookie = GetCookie (p);
    const CookieType endCookie = CookieSwap (startCookie);
    // Write location
    if (keepLocation)
    {
      *((uint32*)p) = indicator;
      p += sizeof (indicator);
    }
    // Write allocated size(needed for checks in free()) and cookies.
    *((size_t*)p) = n;
    p += sizeof (size_t);
    *((CookieType*)p) = startCookie;
    p += sizeof (startCookie);
    *((CookieType*)(p + n)) = endCookie;
    // Pepper.
    memset (p, 0xca, n);
    if (doCheck)
    {
      AllocatedBlock newBlock;
      newBlock.address = p;
      newBlock.size = n;
      newBlock.stack.AttachNew (csCallStackHelper::CreateCallStack (0, true));
      
      CS::Threading::ScopedLock<CS::Threading::RecursiveMutex> lock (
        allocatedPointersMutex);
      allocatedPointers.InsertSorted (newBlock);
    }
    return p;
  }
  
  template<bool keepLocation, bool doCheck>
  static void ptfree_debug (void* P)
  { 
    if (P == 0) return;
    size_t locationSize = 0;
    if (keepLocation) locationSize = sizeof (indicator);
    
    const AllocatedBlock* block = 0;
    if (keepLocation) block = FindAllocatedBlock (P);
    
    // Compute original allocated address
    uint8* p = (uint8*)P;
    p -= sizeof(CookieType);
    const CookieType startCookie = GetCookie (p
      - (sizeof(size_t) + locationSize));
    const CookieType endCookie = CookieSwap (startCookie);
    // Verify cookies
    mem_check (keepLocation,
      "Memory block has wrong cookie "
      "(was probably allocated in another module)",
      *(CookieType*)p == startCookie, block ? block->stack : 0,
      __FILE__, __LINE__);
    p -= sizeof(size_t);
    size_t n = *((size_t*)p);
    if (keepLocation)
    {
      p -= sizeof (indicator);
    }
    mem_check (keepLocation,
      "Memory block has wrong cookie "
      "(probably corrupted by an overflow)",
      *(CookieType*)((uint8*)P + n) == endCookie, 
      block ? block->stack : 0, __FILE__, __LINE__);
    // Salt.
    size_t extraData = sizeof (size_t) + 2*sizeof (CookieType);
    memset (p + locationSize, 0xcf, n + extraData);
    ptmalloc_::ptfree (p);
    if (keepLocation)
    {
      CS::Threading::ScopedLock<CS::Threading::RecursiveMutex> lock (
        allocatedPointersMutex);
      size_t index = allocatedPointers.FindSortedKey (
        csArrayCmp<AllocatedBlock, void*> (P, AllocatedBlockKeyCompare));
      if (index == csArrayItemNotFound)
      {
        fprintf (stderr, 
          "MALLOC ISSUE: pointer 0x%p not allocated with ptmalloc_located\n",
          P);
        fflush (stderr);
      }
      else
        allocatedPointers.DeleteIndex (index);
    }
      
    if (doCheck)
    {
      if (CS::Threading::AtomicOperations::Decrement (&remainingActions) == 0)
      {
        CS::Debug::VerifyAllMemory ();
        CS::Threading::AtomicOperations::Set (&remainingActions, verifyFreq);
      }
    }
  }
  
  template<bool keepLocation, bool doCheck>
  static void* ptrealloc_debug (void* P, size_t n)
  { 
    if (P == 0) return ptmalloc_debug<keepLocation, doCheck> (n);
    if (n > maxRequest)
    {
      errno = ENOMEM;
      return 0;
    }
    if (doCheck)
    {
      if (--remainingActions == 0)
      {
        CS::Debug::VerifyAllMemory ();
        remainingActions = verifyFreq;
      }
    }
    
    size_t locationSize = 0;
    if (keepLocation) locationSize = sizeof (indicator);
    
    const AllocatedBlock* oldBlock = 0;
    if (keepLocation) oldBlock = FindAllocatedBlock (P);
    
    // Compute original allocated address
    uint8* p = (uint8*)P;
    p -= sizeof(CookieType);
    // Verify cookies
    const CookieType startCookie = GetCookie (p
      - (sizeof(size_t) + locationSize));
    const CookieType endCookie = CookieSwap (startCookie);
    mem_check (keepLocation,
      "Memory block has wrong cookie "
      "(was probably allocated in another module)",
      *(CookieType*)p == startCookie, 
      oldBlock ? oldBlock->stack : 0, __FILE__, __LINE__);
    p -= sizeof(size_t);
    size_t nOld = *((size_t*)p);
    if (keepLocation)
    {
      p -= sizeof (indicator);
    }
    mem_check (keepLocation,
      "Memory block has wrong cookie "
      "(probably corrupted by an overflow)",
      *(CookieType*)((uint8*)P + nOld) == endCookie, 
      oldBlock ? oldBlock->stack : 0, __FILE__, __LINE__);
  
    size_t extraData = sizeof (size_t) + 2*sizeof (CookieType) + locationSize;
    uint8* np = (uint8*)ptmalloc_::ptrealloc (p, 
      n + extraData); 
    // Cookie may have changed since the memory address may have changed,
    // update
    const CookieType newStartCookie = GetCookie (np);
    const CookieType newEndCookie = CookieSwap (newStartCookie);
    if (keepLocation)
    {
      np += sizeof (indicator);
    }
    *((size_t*)np) = n;
    np += sizeof (size_t);
    *((CookieType*)np) = newStartCookie;
    np += sizeof (cookie);
    *((CookieType*)(np + n)) = newEndCookie;
    // Spice the enlarged area
    if (n > nOld)
      memset (np + nOld, 0xca, n-nOld);
    if (keepLocation)
    {
      CS::Threading::ScopedLock<CS::Threading::RecursiveMutex> lock (
        allocatedPointersMutex);
      size_t index = allocatedPointers.FindSortedKey (
        csArrayCmp<AllocatedBlock, void*> (P, AllocatedBlockKeyCompare));
      if (index == csArrayItemNotFound)
      {
        fprintf (stderr, 
          "MALLOC ISSUE: pointer %p not allocated with ptmalloc_located\n",
          P);
        fflush (stderr);
      }
      else
      {
        allocatedPointers.DeleteIndex (index);
      }
      AllocatedBlock newBlock;
      newBlock.address = np;
      newBlock.size = n;
      newBlock.stack.AttachNew (csCallStackHelper::CreateCallStack (0, true));
      
      allocatedPointers.InsertSorted (newBlock);
    }
    return np;
  }
  
  template<bool keepLocation, bool doCheck>
  static void* ptcalloc_debug (size_t n, size_t s)
  { 
    // Overflow test lifted from dlmalloc
    const size_t halfSizeT = (~(size_t)0) >> (sizeof (size_t) * 4);
    size_t req = n*s;
    if (((n | s) & ~halfSizeT) && (req / n != s))
    {
      errno = ENOMEM;
      return 0;
    }
    void* p = ptmalloc_debug<keepLocation, doCheck> (n * s);
    if (p != 0) memset (p, 0, n * s);
    return p;
  }

}

void* ptmalloc_sentinel (size_t n)
{
  return ptmalloc_debug<false, false> (n);
}

void ptfree_sentinel (void* P)
{ 
  return ptfree_debug<false, false> (P);
}

void* ptrealloc_sentinel (void* P, size_t n)
{ 
  return ptrealloc_debug<false, false> (P, n);
}

void* ptcalloc_sentinel (size_t n, size_t s)
{ 
  return ptcalloc_debug<false, false> (n, s);
}


void* ptmalloc_located (size_t n)
{
  return ptmalloc_debug<true, false> (n);
}

void ptfree_located (void* P)
{ 
  return ptfree_debug<true, false> (P);
}

void* ptrealloc_located (void* P, size_t n)
{ 
  return ptrealloc_debug<true, false> (P, n);
}

void* ptcalloc_located (size_t n, size_t s)
{ 
  return ptcalloc_debug<true, false> (n, s);
}


void* ptmalloc_checking (size_t n)
{
  return ptmalloc_debug<true, true> (n);
}

void ptfree_checking (void* P)
{ 
  return ptfree_debug<true, true> (P);
}

void* ptrealloc_checking (void* P, size_t n)
{ 
  return ptrealloc_debug<true, true> (P, n);
}

void* ptcalloc_checking (size_t n, size_t s)
{ 
  return ptcalloc_debug<true, true> (n, s);
}

/* Cygwin has funny issues with atexit() that ptmalloc seems to tickle.
 * So within ptmalloc we use our own single-use implementation of atexit()
 * when on Cygwin.  Note that use of a static variable could lead to incorrect
 * cleanup order so we use the GCC "__attribute__ ((init_priority (101))"
 * extention to force atexitHandler to be constructed before other
 * static vars and thus be destructed after all other static vars.
 *
 * With the MSVC runtime (ie MSVC itself and MingW), the catch is that 
 * atexit() functions are called before global static objects are destroyed. 
 *
 * On Linux this seems to be the case as well, so generally tweak the 
 * destruction order here.
 *
 * !!! WARNING !!!
 * This is fragile.  If some other part of the application tries to set a
 * lower init_prority (lower means eariler construction and later destruction)
 * ptmalloc could crash on application exit (typically in ptfree).
 * The bottom line is be sure that no other static variable in
 * the application has a numerically smaller init_priority
 * than atexitHandler.
 */

#if defined(CS_COMPILER_MSVC)
#pragma warning(disable:4073)
#pragma init_seg(lib)
#endif

namespace CS
{
  namespace ptmalloc_
  {
    class AtexitHandler
    {
      void (*func)(void);
    public:
      ~AtexitHandler()
      {
        if (func) func();
      }
      void Set (void(*func)(void))
      {
        CS_ASSERT(this->func == 0);
        this->func = func;
      }
    };
    static AtexitHandler atexitHandler 
#if defined(CS_COMPILER_GCC)
      __attribute__ ((init_priority (101)))
#endif
    ;
  }
}

extern "C" int cs_atexit(void(*func)(void))
{
  CS::ptmalloc_::atexitHandler.Set (func);
  return 0;
}
