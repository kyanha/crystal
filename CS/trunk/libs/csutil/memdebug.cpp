/*
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

// Include platform.h here. Otherwise we don't get the CS_EXTENSIVE_MEMDEBUG
// or CS_MEMORY_TRACKER definitions.
#include "platform.h"

#ifdef CS_EXTENSIVE_MEMDEBUG
  // in cssysdef.h is a "#define new" which affects the operator
  // implementations as well
#  define CS_EXTENSIVE_MEMDEBUG_IMPLEMENT
#  undef CS_EXTENSIVE_MEMDEBUG
#endif

#ifdef CS_MEMORY_TRACKER
#  define CS_MEMORY_TRACKER_IMPLEMENT
#  undef CS_MEMORY_TRACKER
#endif

// it's important that cssysdef.h is included AFTER the above #ifdef
#include "cssysdef.h"

#ifdef CS_EXTENSIVE_MEMDEBUG_IMPLEMENT
// Select the type of memory debugger to use:
//#  define MEMDEBUG_EXTENSIVE
//#  define MEMDEBUG_CHECKALLOC
//#  define MEMDEBUG_DUMPALLOC
#else
#  define MEMDEBUG_MEMORY_TRACKER
#endif

#if defined(COMP_VC)
//========================================================================
// Branch: For VC
//========================================================================

// The VC runtime has it's own memory debugging facility, always
// enabled when using the debug runtime.
// So not much to do here.

#ifdef CS_EXTENSIVE_MEMDEBUG_IMPLEMENT
// use the CRT's built-in memory debugging aids
#include <crtdbg.h>

void* operator new (size_t s, void* filename, int line)
{
  return (void*)_malloc_dbg (s, _NORMAL_BLOCK, (char*)filename, line);
}

void* operator new[] (size_t s, void* filename, int line)
{
  return (void*)_malloc_dbg (s, _NORMAL_BLOCK, (char*)filename, line);
}

void operator delete (void* p)
{
  if (p) _free_dbg (p, _NORMAL_BLOCK);
}
void operator delete[] (void* p)
{
  if (p) _free_dbg (p, _NORMAL_BLOCK);
}
#endif	// CS_EXTENSIVE_MEMDEBUG_IMPLEMENT

#elif defined(MEMDEBUG_EXTENSIVE) // COMP_VC
//========================================================================
// Extensive memory debugger.
//========================================================================

// Configuration:

// If the following define is 1 we will use a table and then we're able
// to do much more extensive (but lots slower) memory debugging.
#define DETECT_USE_TABLE 0

// If this define is 1 then freed memory will not be freed but
// instead kept for some time (depending on age). This is of course very
// memory expensive.
#define DETECT_KEEP_FREE_MEMORY 0
//========================================================================

// Size of the wall in front and after all memory allocations.
#define DETECT_WALL 20
#define DETECT_WALL_SAME 8

#define DETECT      "ABCDabcd01234567890+"
#define DETECTAR    "ABCDabcd+09876543210"	// 8 first bytes same as DETECT
#define DETECTFREE  "FreeFreeFreeFreeFree"
#define DETECT_NEW  0xda
#define DETECT_FREE 0x9d

struct MemEntry
{
  char* start;		// 0 if not used or else pointer to memory.
  size_t size;
  bool freed;		// If true then this memory entry is freed.
  unsigned long age;	// Time when 'free' was done.
  char* alloc_file;	// Filename at allocation time.
  int alloc_line;	// Line number.
};

// This is the size of the table with all memory allocations
// that are kept in memory.
#define DETECT_TABLE_SIZE 100000
// When the table is full use the following define to delete older
// entries. The define is the number of age cycles to go back in time.
#define DETECT_TABLE_OLDER 10000

// This define indicates how often we will check memory.
#define DETECT_CHECK_MEMORY 1000

// If this define is 1 then new allocated memory will be filled
// with DETECT_NEW bytes.
#define DETECT_GARBLE_NEW 1

// If this define is 1 then freed memory will be filled
// with DETECT_FREE bytes.
#define DETECT_GARBLE_FREE 1

#if DETECT_USE_TABLE
static int first_free_idx = -1;
static unsigned long global_age = 0;
static MemEntry mem_table[DETECT_TABLE_SIZE];

//=======================================================
// If the table is not initialized, initialize it here.
//=======================================================
static void InitFreeMemEntries ()
{
  if (first_free_idx >= 0) return;
  int i;
  for (i = 0 ; i < DETECT_TABLE_SIZE ; i++)
  {
    mem_table[i].start = 0;
    mem_table[i].size = 0;
    mem_table[i].freed = true;
  }
  first_free_idx = 0;
}

//=======================================================
// Compact the table by removing all entries that are 0
// and shifting the others upwards.
//=======================================================
static void CompactMemEntries ()
{
  printf ("Basic compact of memory table!\n"); fflush (stdout);
  InitFreeMemEntries ();
  // Compact the table.
  int i, j;
  for (i = j = 0 ; i < DETECT_TABLE_SIZE ; i++)
  {
    if (mem_table[i].start != 0)
    {
      if (j != i) mem_table[j] = mem_table[i];
      j++;
    }
  }
  for (i = j ; i < DETECT_TABLE_SIZE ; i++)
    mem_table[i].start = 0;
  first_free_idx = j;
}

//=======================================================
// Compact the table by removing all freed memory entries
// that are older that the given older_age.
//=======================================================
static void CompactMemEntries (unsigned long older_age)
{
  printf ("Extended compact of memory table!\n"); fflush (stdout);
  InitFreeMemEntries ();
  // Compact the table.
  int i, j;
  for (i = j = 0 ; i < DETECT_TABLE_SIZE ; i++)
  {
    if (mem_table[i].start != 0)
    {
      if (mem_table[i].freed && mem_table[i].age < older_age)
      {
        free (mem_table[i].start-DETECT_WALL-4);
      }
      else
      {
        if (j != i) mem_table[j] = mem_table[i];
        j++;
      }
    }
  }
  for (i = j ; i < DETECT_TABLE_SIZE ; i++)
    mem_table[i].start = 0;
  first_free_idx = j;
}

//=======================================================
// Find a free memory entry.
// If needed the table will be compacted.
//=======================================================
static MemEntry& FindFreeMemEntry ()
{
  InitFreeMemEntries ();
  if (first_free_idx >= DETECT_TABLE_SIZE)
  {
    CompactMemEntries ();
    if (first_free_idx >= DETECT_TABLE_SIZE-20)
    {
      // There is still too little space left so we
      // compact again.
      unsigned long older_age = global_age;
      if (older_age >= DETECT_TABLE_OLDER)
        older_age -= DETECT_TABLE_OLDER;
      else older_age = 0;
      for (;;)
      {
        CompactMemEntries (older_age);
	if (first_free_idx >= DETECT_TABLE_SIZE-5)
	{
	  older_age += DETECT_TABLE_OLDER/10;
	  if (older_age > global_age-10)
	  {
	    printf ("Increase DETECT_TABLE_SIZE!\n");
	    fflush (stdout);
	    exit (0);
	  }
	}
	else
	{
	  break;
	}
      }
    }
  }
  first_free_idx++;
  return mem_table[first_free_idx-1];
}

//=======================================================
// Find the memory entry for the given memory.
//=======================================================
static MemEntry* FindMemEntry (char* mem)
{
  int i;
  for (i = 0 ; i < first_free_idx ; i++)
  {
    if (mem == mem_table[i].start) return mem_table+i;
  }
  return 0;
}

//=======================================================
// Show block info when a crash occurs.
//=======================================================
static void ShowBlockInfo (MemEntry& me)
{
  printf ("BLOCK: start=%08lx size=%d freed=%d\n", (long)me.start,
  	me.size, me.freed);
# ifdef CS_EXTENSIVE_MEMDEBUG_IMPLEMENT
  printf ("       alloced at '%s' %d\n", me.alloc_file, me.alloc_line);
# endif
}

//=======================================================
// Do a sanity check on all entries in the memory table.
// Check if freed memory is still containing the contents
// we put there. Check if the walls are intact.
//=======================================================
static void MemoryCheck ()
{
  printf ("Checking memory (age=%ld)...\n", global_age); fflush (stdout);
  int i;
  for (i = 0 ; i < first_free_idx ; i++)
  {
    MemEntry& me = mem_table[i];
    if (me.start != 0)
    {
      char* rc = me.start - DETECT_WALL - 4;
      size_t s;
      memcpy (&s, rc+DETECT_WALL, 4);
      if (s != me.size)
      {
	ShowBlockInfo (me);
        printf ("CHK: Size in table doesn't correspond with size in block!\n");
	fflush (stdout);
	DEBUG_BREAK;
      }
      if (me.freed)
      {
	if (strncmp (rc, DETECTFREE, DETECT_WALL) != 0)
	{
	  ShowBlockInfo (me);
	  printf ("CHK: Bad start of block for freed block!\n");
	  fflush (stdout);
	  DEBUG_BREAK;
	}
	if (strncmp (rc+4+DETECT_WALL+s, DETECTFREE, DETECT_WALL) != 0)
	{
	  ShowBlockInfo (me);
	  printf ("CHK: Bad end of block for freed block!\n");
	  fflush (stdout);
	  DEBUG_BREAK;
	}
#       if DETECT_KEEP_FREE_MEMORY
        unsigned int j;
	for (j = 0 ; j < s ; j++)
	{
	  if (me.start[j] != (char)DETECT_FREE)
	  {
	    ShowBlockInfo (me);
	    printf ("CHK: Freed memory is used at offset (%d)!\n", j);
	    fflush (stdout);
	    DEBUG_BREAK;
	  }
	}
#       endif
      }
      else
      {
	if (strncmp (rc, DETECT, DETECT_WALL_SAME) != 0)
	{
	  ShowBlockInfo (me);
	  printf ("CHK: Bad start of block!\n");
	  fflush (stdout);
	  DEBUG_BREAK;
	}
	if (strncmp (rc+4+DETECT_WALL+s, DETECT, DETECT_WALL_SAME) != 0)
	{
	  ShowBlockInfo (me);
	  printf ("CHK: Bad end of block!\n");
	  fflush (stdout);
	  DEBUG_BREAK;
	}
      }
    }
  }
}
#endif // DETECT_USE_TABLE

//=======================================================
// Dump error
//=======================================================
static void DumpError (const char* msg, int info, char* rc)
{
  bool do_crash = true;
#if DETECT_USE_TABLE
  if (rc)
  {
    MemEntry* me = FindMemEntry (rc);
    if (me)
      ShowBlockInfo (*me);
    else
    {
      printf ("Memory block not allocated in this module!\n");
      do_crash = false;
    }
  }
  else
  {
    printf ("No block info!\n");
  }
#endif
  printf (msg, info);
  fflush (stdout);
  if (do_crash)
  {
    DEBUG_BREAK;
  }
}

#ifdef CS_EXTENSIVE_MEMDEBUG_IMPLEMENT
#undef new
void* operator new (size_t s, void* filename, int line)
#else
void* operator new (size_t s)
#endif
{
#if DETECT_USE_TABLE
  global_age++;
  if (global_age % DETECT_CHECK_MEMORY == 0) MemoryCheck ();
#endif
  if (s <= 0) DumpError ("BAD SIZE in new %d\n", s, 0);
  char* rc = (char*)malloc (s+4+DETECT_WALL+DETECT_WALL);
  memcpy (rc, DETECT, DETECT_WALL);
  memcpy (rc+DETECT_WALL, &s, 4);
  memcpy (rc+DETECT_WALL+4+s, DETECT, DETECT_WALL);
#if DETECT_GARBLE_NEW
  memset ((void*)(rc+4+DETECT_WALL), DETECT_NEW, s);
#endif
#if DETECT_USE_TABLE
  MemEntry& me = FindFreeMemEntry ();
  me.start = rc+4+DETECT_WALL;
  me.size = s;
  me.freed = false;
# ifdef CS_EXTENSIVE_MEMDEBUG_IMPLEMENT
  me.alloc_file = (char*)filename;
  me.alloc_line = line;
# endif
#endif
  return (void*)(rc+4+DETECT_WALL);
}

#ifdef CS_EXTENSIVE_MEMDEBUG_IMPLEMENT
void* operator new[] (size_t s, void* filename, int line)
#else
void* operator new[] (size_t s)
#endif
{
#if DETECT_USE_TABLE
  global_age++;
  if (global_age % DETECT_CHECK_MEMORY == 0) MemoryCheck ();
#endif
  if (s <= 0) DumpError ("BAD SIZE in new[] %d\n", s, 0);
  char* rc = (char*)malloc (s+4+DETECT_WALL+DETECT_WALL);
  memcpy (rc, DETECTAR, DETECT_WALL);
  memcpy (rc+DETECT_WALL, &s, 4);
  memcpy (rc+DETECT_WALL+4+s, DETECTAR, DETECT_WALL);
#if DETECT_GARBLE_NEW
  memset ((void*)(rc+4+DETECT_WALL), DETECT_NEW, s);
#endif
#if DETECT_USE_TABLE
  MemEntry& me = FindFreeMemEntry ();
  me.start = rc+4+DETECT_WALL;
  me.size = s;
  me.freed = false;
# ifdef CS_EXTENSIVE_MEMDEBUG_IMPLEMENT
  me.alloc_file = (char*)filename;
  me.alloc_line = line;
# endif
#endif
  return (void*)(rc+4+DETECT_WALL);
}

void operator delete (void* p)
{
  if (!p) return;
#if DETECT_USE_TABLE
  global_age++;
  if (global_age % DETECT_CHECK_MEMORY == 0) MemoryCheck ();
#endif
  char* rc = (char*)p;
  rc -= 4+DETECT_WALL;
  size_t s;
  memcpy (&s, rc+DETECT_WALL, 4);
  if (strncmp (rc, DETECT, DETECT_WALL) != 0)
    DumpError ("operator delete: BAD START!\n", 0, (char*)p);
  if (strncmp (rc+4+DETECT_WALL+s, DETECT, DETECT_WALL) != 0)
    DumpError ("operator delete: BAD END!\n", 0, (char*)p);
  memcpy (rc, DETECTFREE, DETECT_WALL);
  memcpy (rc+4+s+DETECT_WALL, DETECTFREE, DETECT_WALL);

#if DETECT_GARBLE_FREE
  memset ((void*)(rc+4+DETECT_WALL), DETECT_FREE, s);
#endif

#if DETECT_USE_TABLE
  MemEntry* me = FindMemEntry (rc+4+DETECT_WALL);
  if (!me)
    DumpError ("ERROR! Can't find memory entry for this block!\n", 0,
      (char*)p);
  if (me->size != s)
    DumpError ("ERROR! Size in table does not correspond with size in block!\n",
    	0, (char*)p);
  if (me->freed)
    DumpError ("ERROR! According to table memory is already freed!\n", 0,
      (char*)p);
# if DETECT_KEEP_FREE_MEMORY
  me->freed = true;
  me->age = global_age;
# else
  me->start = 0;
  free (rc);
# endif
#else
  free (rc);
#endif
}

void operator delete[] (void* p)
{
  if (!p) return;
#if DETECT_USE_TABLE
  global_age++;
  if (global_age % DETECT_CHECK_MEMORY == 0) MemoryCheck ();
#endif
  char* rc = (char*)p;
  rc -= 4+DETECT_WALL;
  size_t s;
  memcpy (&s, rc+DETECT_WALL, 4);
  if (strncmp (rc, DETECTAR, DETECT_WALL) != 0)
    DumpError ("operator delete[]: BAD START!\n", 0, (char*)p);
  if (strncmp (rc+4+DETECT_WALL+s, DETECTAR, DETECT_WALL) != 0)
    DumpError ("operator delete[]: BAD END!\n", 0, (char*)p);
  memcpy (rc, DETECTFREE, DETECT_WALL);
  memcpy (rc+4+s+DETECT_WALL, DETECTFREE, DETECT_WALL);
#if DETECT_GARBLE_FREE
  memset ((void*)(rc+4+DETECT_WALL), DETECT_FREE, s);
#endif

#if DETECT_USE_TABLE
  MemEntry* me = FindMemEntry (rc+4+DETECT_WALL);
  if (!me)
    DumpError ("ERROR! Can't find memory entry for this block!\n", 0,
      (char*)p);
  if (me->size != s)
    DumpError ("ERROR! Size in table does not correspond with size in block!\n",
    	0, (char*)p);
  if (me->freed)
    DumpError ("ERROR! According to table memory is already freed!\n", 0,
    	(char*)p);
# if DETECT_KEEP_FREE_MEMORY
  me->freed = true;
  me->age = global_age;
# else
  me->start = 0;
  free (rc);
# endif
#else
  free (rc);
#endif
}

#elif defined(MEMDEBUG_CHECKALLOC)	// COMP_VC
//========================================================================
// This alternative branch allows for checking allocated memory amounts.
//========================================================================

#ifdef CS_EXTENSIVE_MEMDEBUG_IMPLEMENT
#undef new
static size_t alloc_total = 0;
static size_t alloc_cnt = 0;
void* operator new (size_t s, void* filename, int line)
{
  alloc_total += s;
  alloc_cnt++;
  uint32* rc = (uint32*)malloc (s+8);
  *rc++ = 0xdeadbeef;
  *rc++ = s;
  printf ("+ %p %d %d %s\n", &alloc_total, alloc_total, alloc_cnt, filename);
  fflush (stdout);
  return (void*)rc;
}
void* operator new[] (size_t s, void* filename, int line)
{
  alloc_total += s;
  alloc_cnt++;
  uint32* rc = (uint32*)malloc (s+8);
  *rc++ = 0xdeadbeef;
  *rc++ = s;
  printf ("+ %p %d %d %s\n", &alloc_total, alloc_total, alloc_cnt, filename);
  fflush (stdout);
  return (void*)rc;
}
void operator delete (void* p)
{
  if (p)
  {
    uint32* rc = ((uint32*)p)-2;
    CS_ASSERT (*rc == 0xdeadbeef);
    alloc_total -= rc[1];
    alloc_cnt--;
    free ((void*)rc);
    printf ("- %p %d %d\n", &alloc_total, alloc_total, alloc_cnt);
  }
}
void operator delete[] (void* p)
{
  if (p)
  {
    uint32* rc = ((uint32*)p)-2;
    CS_ASSERT (*rc == 0xdeadbeef);
    alloc_total -= rc[1];
    alloc_cnt--;
    free ((void*)rc);
    printf ("- %p %d %d\n", &alloc_total, alloc_total, alloc_cnt);
  }
}
#endif	// CS_EXTENSIVE_MEMDEBUG_IMPLEMENT

#elif defined(MEMDEBUG_DUMPALLOC)	// COMP_VC
//========================================================================
// This alternative branch allows for dumping all memory allocations.
//========================================================================

#ifdef CS_EXTENSIVE_MEMDEBUG_IMPLEMENT
#undef new
static size_t alloc_total = 0;
static size_t alloc_cnt = 0;
void* operator new (size_t s, void* filename, int line)
{
  alloc_total += s;
  alloc_cnt++;
  if (s > 1000) { printf ("new s=%d tot=%d/%d file=%s line=%d\n",
  	s, alloc_total, alloc_cnt, filename, line); fflush (stdout); }
  return (void*)malloc (s);
}
void* operator new[] (size_t s, void* filename, int line)
{
  alloc_total += s;
  alloc_cnt++;
  if (s > 1000) { printf ("new[] s=%d tot=%d/%d file=%s line=%d\n",
  	s, alloc_total, alloc_cnt, filename, line); fflush (stdout); }
  return (void*)malloc (s);
}
void operator delete (void* p)
{
  if (p) free (p);
}
void operator delete[] (void* p)
{
  if (p) free (p);
}
#endif	// CS_EXTENSIVE_MEMDEBUG_IMPLEMENT

#elif defined(MEMDEBUG_MEMORY_TRACKER)

#undef new

#include "csutil/scf.h"
#include "csutil/ref.h"
#include "iutil/objreg.h"
#include "iutil/memdebug.h"

// This structure is used per file to keep track of allocations.
// ModuleMemTracker maintains an array of them per module.
struct MemTrackerInfo
{
  char* file;
  size_t max_alloc;
  size_t current_alloc;
  int max_count;
  int current_count;
  void Init (char* filename)
  {
    file = filename;
    max_alloc = 0;
    current_alloc = 0;
    max_count = 0;
    current_count = 0;
  }
};

// This class is the memory tracker per module or application.
// MemTrackerRegistry maintains a list of them.
class MemTrackerModule
{
public:
  char* Class;		// Name of class or 0 for application level.
  MemTrackerInfo* mti_table[2000];
  int mti_table_count;

  MemTrackerModule ()
  {
    mti_table_count = 0;
  }

  void InsertBefore (int idx, char* filename)
  {
    int tomove = mti_table_count - idx;
    if (tomove > 0)
      memmove (mti_table+idx+1, mti_table+idx,
      	  sizeof (MemTrackerInfo*) * tomove);
    mti_table_count++;
    mti_table[idx] = (MemTrackerInfo*)malloc (sizeof (MemTrackerInfo));
    mti_table[idx]->Init (filename);
  }

  MemTrackerInfo* FindInsertMtiTableEntry (
	char* filename, int start, int end)
  {
    // Binary search.
    if (mti_table_count <= 0)
    {
      mti_table_count++;
      mti_table[0] = (MemTrackerInfo*)malloc (sizeof (MemTrackerInfo));
      mti_table[0]->Init (filename);
      return mti_table[0];
    }

    if (start == end)
    {
    //@@@@@@@@@@@ IS STRCMP NEEDED?
      int rc = strcmp (filename, mti_table[start]->file);
      if (rc == 0) return mti_table[start];
      if (rc < 0)
      {
        InsertBefore (start, filename);
        return mti_table[start];
      }
      else
      {
        InsertBefore (start+1, filename);
        return mti_table[start+1];
      }
    }
    else if (start+1 == end)
    {
      int rc1 = strcmp (filename, mti_table[start]->file);
      if (rc1 == 0) return mti_table[start];
      if (rc1 < 0)
      {
        InsertBefore (start, filename);
        return mti_table[start];
      }

      int rc2 = strcmp (filename, mti_table[end]->file);
      if (rc2 == 0) return mti_table[end];
      if (rc2 > 0)
      {
        InsertBefore (end+1, filename);
        return mti_table[end+1];
      }
      InsertBefore (start+1, filename);
      return mti_table[start+1];
    }
    else
    {
      int mid = (start+end)/2;
      int rc = strcmp (filename, mti_table[mid]->file);
      if (rc == 0) return mti_table[mid];
      if (rc < 0) return FindInsertMtiTableEntry (filename, start, mid-1);
      return FindInsertMtiTableEntry (filename, mid+1, end);
    }
  }

  MemTrackerInfo* FindInsertMtiTableEntry (char* filename)
  {
    return FindInsertMtiTableEntry (filename, 0, mti_table_count-1);
  }

  void Dump ()
  {
    int i;
    printf ("-----------------------------------------------------\n");
    printf ("Module: %s\n", Class);
    printf ("current  max      current# max#     file\n");
    for (i = 0 ; i < mti_table_count ; i++)
    {
      MemTrackerInfo* mti = mti_table[0];
      printf ("%8d %8d %8d %8d %s\n", mti->current_alloc,
    	  mti->max_alloc, mti->current_count, mti->max_count,
	  mti->file);
    }
    fflush (stdout);
  }
};


// The following machinery is needed to try to keep track of memory
// allocations in different plugins.
class MemTrackerRegistry : public iMemoryTracker
{
public:
  MemTrackerModule* modules[500];	// @@@ Hardcoded!
  int num_modules;

  SCF_DECLARE_IBASE;
  MemTrackerRegistry ()
  {
    SCF_CONSTRUCT_IBASE (0);
    num_modules = 0;
  }
  virtual ~MemTrackerRegistry ()
  {
    SCF_DESTRUCT_IBASE ();
  }

  MemTrackerModule* NewMemTrackerModule (char* Class)
  {
    MemTrackerModule* mod = new MemTrackerModule ();
    mod->Class = Class;
    modules[num_modules++] = mod;
    return mod;
  }

  virtual void Dump ()
  {
    int i;
    for (i = 0 ; i < num_modules ; i++)
    {
      modules[i]->Dump ();
    }
  }
};

SCF_IMPLEMENT_IBASE (MemTrackerRegistry)
  SCF_IMPLEMENTS_INTERFACE (iMemoryTracker)
SCF_IMPLEMENT_IBASE_END

static MemTrackerModule* mti_this_module = 0;

void RegisterMemoryTrackerModule (char* Class)
{
  if (!iSCF::SCF)
  {
    printf ("iSCF::SCF not set yet!\n");
    return;
  }

  if (iSCF::SCF->object_reg)
  {
    csRef<iMemoryTracker> mtiTR = CS_QUERY_REGISTRY_TAG_INTERFACE (
    	iSCF::SCF->object_reg, "crystalspace.utilities.memorytracker",
	iMemoryTracker);
    if (!mtiTR)
    {
      mtiTR = csPtr<iMemoryTracker> ((iMemoryTracker*)new MemTrackerRegistry);
      iSCF::SCF->object_reg->Register (mtiTR,
      	"crystalspace.utilities.memorytracker");
    }
    mti_this_module = ((MemTrackerRegistry*)(iMemoryTracker*)mtiTR)
    	->NewMemTrackerModule (Class);
  }
  else
  {
    printf ("Object Reg not set for %s!!!\n", Class);
    fflush (stdout);
  }
}

MemTrackerInfo* mtiRegisterAlloc (size_t s, void* filename)
{
  if (mti_this_module == 0)
    return 0;	// Don't track this alloc yet.

  MemTrackerInfo* mti = mti_this_module->FindInsertMtiTableEntry (
  	(char*)filename);
  mti->current_count++;
  mti->current_alloc += s;
  if (mti->current_count > mti->max_count)
    mti->max_count = mti->current_count;
  if (mti->current_alloc > mti->max_alloc)
    mti->max_alloc = mti->current_alloc;
  return mti;
}

void mtiRegisterFree (MemTrackerInfo* mti, size_t s)
{
  if (mti)
  {
    mti->current_count--;
    mti->current_alloc -= s;
  }
}

void* operator new (size_t s, void* filename, int /*line*/)
{
  uint32* rc = (uint32*)malloc (s+16);
  *rc++ = s;
  *rc++ = 0xbeebbeeb;
  *rc++ = (uint32)mtiRegisterAlloc (s, filename);
  *rc++ = 0xdeadbeef;
  return (void*)rc;
}
void* operator new[] (size_t s, void* filename, int /*line*/)
{
  uint32* rc = (uint32*)malloc (s+16);
  *rc++ = s;
  *rc++ = 0xfeedbeef;
  *rc++ = (uint32)mtiRegisterAlloc (s, filename);
  *rc++ = 0xdeadbeef;
  return (void*)rc;
}
void operator delete (void* p)
{
  if (p)
  {
    uint32* rc = ((uint32*)p)-4;
    if (rc[3] != 0xdeadbeef) { free (p); return; }
    size_t s = rc[0];
    MemTrackerInfo* mti = (MemTrackerInfo*)rc[2];
    free ((void*)rc);
    mtiRegisterFree (mti, s);
  }
}
void operator delete[] (void* p)
{
  if (p)
  {
    uint32* rc = ((uint32*)p)-4;
    if (rc[3] != 0xdeadbeef) { free (p); return; }
    size_t s = rc[0];
    MemTrackerInfo* mti = (MemTrackerInfo*)rc[2];
    free ((void*)rc);
    mtiRegisterFree (mti, s);
  }
}

#else	// COMP_VC
//========================================================================
// If CS_EXTENSIVE_MEMDEBUG is defined we still have to provide
// the correct overloaded operators even if we don't do debugging.
//========================================================================

#ifdef CS_EXTENSIVE_MEMDEBUG_IMPLEMENT
#undef new
void* operator new (size_t s, void*, int)
{
  return (void*)malloc (s);
}
void* operator new[] (size_t s, void*, int)
{
  return (void*)malloc (s);
}
void operator delete (void* p)
{
  if (p) free (p);
}
void operator delete[] (void* p)
{
  if (p) free (p);
}
#endif	// CS_EXTENSIVE_MEMDEBUG_IMPLEMENT

#endif	// COMP_VC

