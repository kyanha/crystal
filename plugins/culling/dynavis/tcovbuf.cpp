/* 
    Copyright (C) 2002 by Jorrit Tyberghein

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
#include "cssys/sysfunc.h"
#include "csutil/scfstr.h"
#include "iutil/string.h"
#include "ivaria/bugplug.h"
#include "qint.h"
#include "qsqrt.h"
#include "csgeom/box.h"
#include "csgeom/math3d.h"
#include "csgeom/csrect.h"
#include "tcovbuf.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/txtmgr.h"
#include "cssys/csprocessorcap.h"

//---------------------------------------------------------------------------

csBits64 csCoverageTile::coverage_cache[32];
csBits64 csCoverageTile::precalc_end_lines[64];
csBits64 csCoverageTile::precalc_start_lines[64];
bool csCoverageTile::precalc_init = false;

void csCoverageTile::MakePrecalcTables ()
{
  if (precalc_init) return;
  precalc_init = true;
  int i, j;
  for (i = 0 ; i < 64 ; i++)
  {
    precalc_start_lines[i].Empty ();
    for (j = 0 ; j <= i ; j++)
      precalc_start_lines[i].XorBit (j);
    precalc_end_lines[i].Empty ();
    for (j = i ; j <= 63 ; j++)
      precalc_end_lines[i].XorBit (j);
  }
}

csLineOperation& csCoverageTile::AddOperation ()
{
  CS_ASSERT (num_operations <= max_operations);
  if (num_operations >= max_operations)
  {
    if (max_operations < 100)
      max_operations += max_operations;
    else
      max_operations += 100;
    csLineOperation* new_op = new csLineOperation [max_operations];
    if (num_operations > 0)
      memcpy (new_op, operations, sizeof (csLineOperation)*num_operations);
    delete[] operations;
    operations = new_op;
  }
  num_operations++;
  return operations[num_operations-1];
}

void csCoverageTile::PushLine (int x1, int y1, int x2, int y2, int dx)
{
  CS_ASSERT (x1 >= 0);
  CS_ASSERT (x1 < (32<<16));
  CS_ASSERT (x2 >= 0);
  CS_ASSERT (x2 < (32<<16));
  CS_ASSERT (y1 >= 0);
  CS_ASSERT (y1 < 64);
  CS_ASSERT (y2 >= 0);
  CS_ASSERT (y2 < 64);
  CS_ASSERT (x1+ABS (y2-y1)*dx >= 0);
  CS_ASSERT (x1+ABS (y2-y1)*dx < (32<<16));
  CS_ASSERT (x2-ABS (y1-y2)*dx >= 0); //@@@ VERY SUSPICIOUS! These asserts are triggerred in some cases!
  CS_ASSERT (x2-ABS (y1-y2)*dx < (32<<16));
  csLineOperation& op = AddOperation ();
  op.op = OP_LINE;
  op.x1 = x1;
  op.y1 = y1;
  op.x2 = x2;
  op.y2 = y2;
  op.dx = dx;
//printf ("        LINE %d,%d - %d,%d (dx=%d)\n", x1>>16, y1, x2>>16, y2, dx);
}

void csCoverageTile::PushVLine (int x, int y1, int y2)
{
  CS_ASSERT (x >= 0);
  CS_ASSERT (x < (32<<16));
  CS_ASSERT (y1 >= 0);
  CS_ASSERT (y1 < 64);
  CS_ASSERT (y2 >= 0);
  CS_ASSERT (y2 < 64);
  csLineOperation& op = AddOperation ();
  op.op = OP_VLINE;
  op.x1 = x;
  op.y1 = y1;
  op.y2 = y2;
//printf ("        VLINE %d    %d - %d\n", x>>16, y1, y2);
}

void csCoverageTile::PushFullVLine (int x)
{
  CS_ASSERT (x >= 0);
  CS_ASSERT (x < (32<<16));
  csLineOperation& op = AddOperation ();
  op.op = OP_FULLVLINE;
  op.x1 = x;
//printf ("        FULLVLINE %d\n", x>>16);
}

void csCoverageTile::FlushOperations ()
{
  int i;

  memset (coverage_cache, 0, sizeof (csBits64)*32);

  // First draw all lines.
  for (i = 0 ; i < num_operations ; i++)
  {
    csLineOperation& op = operations[i];
    if (op.op == OP_FULLVLINE)
    {
      CS_ASSERT (op.x1 >= 0 && op.x1 <= (32<<16));
      coverage_cache[op.x1 >> 16].Invert ();
    }
    else if (op.op == OP_VLINE)
    {
      CS_ASSERT (op.x1 >= 0 && op.x1 <= (32<<16));
      CS_ASSERT (op.y1 >= 0);
      CS_ASSERT (op.y1 <= 63);
      CS_ASSERT (op.y2 >= 0);
      CS_ASSERT (op.y2 <= 63);
      int y1, y2;
      if (op.y1 < op.y2) { y1 = op.y1; y2 = op.y2; }
      else { y1 = op.y2; y2 = op.y1; }
      const csBits64& start = precalc_start_lines[y2];
      const csBits64& end = precalc_end_lines[y1];
      // Xor the line with the coverage cache. This happens in three stages:
      csBits64& cc = coverage_cache[op.x1 >> 16];
      cc ^= start;
      cc ^= end;
      cc.Invert ();
    }
    else // OP_LINE
    {
      CS_ASSERT (op.x1 >= 0 && op.x1 <= (32<<16));
      CS_ASSERT (op.x2 >= 0 && op.x2 <= (32<<16));
      CS_ASSERT (op.y1 >= 0);
      CS_ASSERT (op.y1 <= 63);
      CS_ASSERT (op.y2 >= 0);
      CS_ASSERT (op.y2 <= 63);
      int x1, y1, x2, y2;
      if (op.y1 < op.y2) { x1 = op.x1; y1 = op.y1; x2 = op.x2; y2 = op.y2; }
      else { x1 = op.x2; y1 = op.y2; x2 = op.x1; y2 = op.y1; }
      int dy = y2-y1;
      int x = x1;
      int y = y1;
      int dx = op.dx;
      while (dy >= 0)
      {
	CS_ASSERT ((x>>16) >= 0);
	CS_ASSERT ((x>>16) < 32);
	csBits64& cc = coverage_cache[x >> 16];
	cc.XorBit (y);
        x += dx;
        y++;
        dy--;
      }
    }
  }

  // Clear all operations.
  num_operations = 0;
}

void csCoverageTile::Flush (csBits64& fvalue, float maxdepth)
{
  int i;

  if (queue_tile_empty)
    MakeEmpty ();

  if (tile_full)
  {
    // Special case. Only update the fvalue since the tile itself
    // is full.
    for (i = 0 ; i < num_operations ; i++)
    {
      csLineOperation& op = operations[i];
      if (op.op == OP_FULLVLINE)
      {
        // We have a full line (from top to bottom). In this case
	// we simply invert the fvalue.
	fvalue.Invert ();
      }
      else
      {
        // We can ignore the x value of the line here. So VLINE and
	// LINE are equivalent in this case.
	CS_ASSERT (op.y1 >= 0);
	CS_ASSERT (op.y1 <= 63);
	CS_ASSERT (op.y2 >= 0);
	CS_ASSERT (op.y2 <= 63);
	int y1, y2;
	if (op.y1 < op.y2) { y1 = op.y1; y2 = op.y2; }
	else { y1 = op.y2; y2 = op.y1; }
	const csBits64& start = precalc_start_lines[y2];
	const csBits64& end = precalc_end_lines[y1];
	// Xor the line with the fvalue. This happens in three stages:
	fvalue ^= start;
	fvalue ^= end;
	fvalue.Invert ();
      }
    }
    num_operations = 0;
  }
  else
  {
    FlushOperations ();

    if (tile_min_depth < INIT_MIN_DEPTH_CMP && maxdepth <= tile_min_depth)
    {
      // If our new depth is smaller than the minimum depth
      // of this tile then we can do a more optimal routine since
      // we don't have to check the depth buffer.

      // Now perform the XOR sweep and OR with main coverage buffer.
      // fvalue will be the modified from left to right and will be
      // OR-ed with the main buffer.
      tile_full = true;	// Assume full for now.
      csBits64* cc = coverage_cache;
      csBits64* c = coverage;
#ifdef PROC_X86
      if (csTiledCoverageBuffer::use_mmx)
      {

        csBits64 allOnes; allOnes.Full ();
        csBits64 temp;
#ifdef COMP_VC
        __asm
        {
          // save state
          push eax
          push ecx
          push esi
          push edi

          mov esi, [cc]
          mov edi, [c]

          mov ecx, 32

          movq mm0, allOnes
          mov eax, [fvalue]
          movq mm1, [eax]
          mov edx, ~0

fillCol:

          movq mm2, [edi] //1

          movq mm3, mm2   //1
          pxor mm1, [esi] // fvalue ^= *cc   //1
          por mm3, mm1 // *c |= fvalue //1        
          movq [edi], mm3 //1

          pcmpeqd mm2, mm0 //1


          movd eax, mm2 //1

          psrlq mm2, 32 //1
          and edx, eax //1

          movd eax, mm2 //1
          and edx, eax //1


          add edi, 8
          add esi, 8

          loop fillCol

          mov eax, [fvalue]
          movq [eax], mm1
          mov eax, [this+tile_full]
          mov [eax], dl

          // restore state
          pop edi
          pop esi
          pop ecx
          pop eax
          emms
        }

#else if COMP_GCC
        csBits64 *fvalueTemp = &fvalue;
        __asm__  (
          "pushl %%eax                 \n"
          "pushl %%ecx                 \n"
          "pushl %%esi                 \n"
          "pushl %%edi                 \n"
          "movl %0, %%esi              \n"
          "movl %1, %%edi              \n"
          "movl $32, %%ecx             \n"

          "movq %4, %%mm0              \n"
          "movl %2, %%eax              \n"
          "movq 0(%%eax), %%mm1        \n"
          //"movq 0(%2), %%mm1           \n"
          "movl $~0, %%edx             \n"
          "fillCol:                    \n"
          "movq 0(%%edi), %%mm2        \n" //1

          "movq %%mm2,%%mm3            \n"   //1
          "pxor 0(%%esi), %%mm1        \n" // fvalue ^= *cc   //1
          
          "por %%mm1, %%mm3            \n" // *c |= fvalue //1        
          "movq %%mm3, 0(%%edi)        \n" //1
          
          "pcmpeqd %%mm0, %%mm2        \n" //1

          "movd %%mm2, %%eax           \n" //1
          "psrlq $32, %%mm2            \n" //1
          "andl %%eax, %%edx           \n" //1
          "movd %%mm2, %%eax           \n" //1
          "andl %%eax, %%edx           \n" //1

          "addl $8, %%edi              \n"
          "addl $8, %%esi              \n"
          "loop fillCol               \n"
          "movl %2, %%eax              \n"
          "movq %%mm1, 0(%%eax)        \n"
          //"movq %%mm1, 0(%2)           \n"
          "movb %%dl, %3               \n"

          // restore state

          "popl %%edi                  \n"
          "popl %%esi                  \n"
          "popl %%ecx                  \n"
          "popl %%eax                  \n"
          "emms                        \n"
          : /* outputs */
          : "g" (cc), "g" (c), "g" (fvalueTemp), "g" (tile_full), "g" (allOnes)
          : "eax", "ecx", "esi", "edi", "edx");
        //fvalue = fvalueTemp;
#endif //COMP_
      }
      else
#endif //_x86_
      {
        for (i = 0 ; i < 32 ; i++)
        {
          fvalue ^= *cc;
          *c |= fvalue;
          if (tile_full && !c->IsFull ())
            tile_full = false;
          cc++;
          c++;
        }
      }
    }
    else
    {
      // The general case. It is possible we have to update the
      // depth buffer.

      // Now perform the XOR sweep and OR with main coverage buffer.
      // fvalue will be the modified from left to right and will be
      // OR-ed with the main buffer. In the mean time the coverage_cache
      // buffer contents will be modified to be true wherever the
      // coverage_cache actually modified the coverage buffer.
      tile_full = true;	// Assume full for now.
      csBits64* cc = coverage_cache;
      csBits64* c = coverage;      

#ifdef PROC_X86
      if (csTiledCoverageBuffer::use_mmx)
      {
        
        csBits64 allOnes; allOnes.Full ();
        csBits64 temp;
    #ifdef COMP_VC
        __asm
        {
          // save state
          push eax
          push ecx
          push esi
          push edi

          mov esi, [cc]
          mov edi, [c]

          mov ecx, 32

          movq mm0, allOnes
          mov eax, [fvalue]
          movq mm1, [eax]
          mov edx, ~0

fillCol2:

          movq mm2, [edi] //1

          movq mm3, mm2   //1
          pxor mm1, [esi] // fvalue ^= *cc   //1
          pandn mm2, mm1 // *cc = ~*c & fvalue //1
          por mm3, mm1 // *c |= fvalue //1        
          movq [edi], mm3 //1
          movq [esi], mm2 //1

          pcmpeqd mm2, mm0 //1


          movd eax, mm2 //1

          psrlq mm2, 32 //1
          and edx, eax //1

          movd eax, mm2 //1
          and edx, eax //1


          add edi, 8
          add esi, 8

          loop fillCol2

          mov eax, [fvalue]
          movq [eax], mm1
          mov eax, [this+tile_full]
          mov [eax], dl

          // restore state
          pop edi
          pop esi
          pop ecx
          pop eax
          emms
        }

  #else if COMP_GCC
        csBits64 *fvalueTemp = &fvalue;
        __asm__  (
          "pushl %%eax                 \n"
          "pushl %%ecx                 \n"
          "pushl %%esi                 \n"
          "pushl %%edi                 \n"
          "movl %0, %%esi              \n"
          "movl %1, %%edi              \n"
          "movl $32, %%ecx             \n"

          "movq %4, %%mm0              \n"
          "movl %2, %%eax              \n"
          "movq 0(%%eax), %%mm1        \n"          
          //"movq %2, %%mm1           \n"
          "movl $~0, %%edx             \n"
          "fillCol2:                   \n"
          "movq 0(%%edi), %%mm2        \n" //1

          "movq %%mm2,%%mm3            \n"   //1
          "pxor 0(%%esi), %%mm1        \n" // fvalue ^= *cc   //1
          "pandn %%mm1, %%mm2          \n" // *cc = ~*c & fvalue //1
          "por %%mm1, %%mm3            \n" // *c |= fvalue //1        
          "movq %%mm3, 0(%%edi)        \n" //1
          "movq %%mm2, 0(%%esi)        \n" //1
          "pcmpeqd %%mm0, %%mm2        \n" //1

          "movd %%mm2, %%eax           \n" //1
          "psrlq $32, %%mm2            \n" //1
          "andl %%eax, %%edx           \n" //1
          "movd %%mm2, %%eax           \n" //1
          "andl %%eax, %%edx           \n" //1

          "addl $8, %%edi              \n"
          "addl $8, %%esi              \n"
          "loop fillCol2               \n"
          "movl %2, %%eax              \n"
          "movq %%mm1, 0(%%eax)        \n"
          //"movq %%mm1, 0(%2)           \n"
          "movb %%dl, %3               \n"

          // restore state

          "popl %%edi                  \n"
          "popl %%esi                  \n"
          "popl %%ecx                  \n"
          "popl %%eax                  \n"
          "emms                        \n"
          : /* outputs */
          : "g" (cc), "g" (c), "g" (fvalueTemp), "g" (tile_full), "g" (allOnes)
          : "eax", "ecx", "esi", "edi", "edx");
          //fvalue = fvalueTemp;
    #endif //COMP_
      }
      else
#endif //_x86_
      {  
        for (i = 0 ; i < 32 ; i++)
        {
          fvalue ^= *cc;
          *cc = fvalue;
          cc->AndInverted (*c);
          *c |= fvalue;
          if (tile_full && !c->IsFull ())
            tile_full = false;
          cc++;
          c++;
        }
      }
      // Now do the depth update. Here we will use the coverage_cache
      // to see where we need to update the depth buffer. The coverage_cache
      // will now contain true wherever the coverage buffer was modified.
      bool recheck_min_depth = false;
      for (i = 0 ; i < 4 ; i++)
      {
        float* ldepth = &depth[i];
        int idx = i << 3;
        int j = 1;
        csBits64 mods = coverage_cache[idx];
        while (j < 8)
        {
          mods |= coverage_cache[idx++];
	  j++;
        }
        if (!mods.IsEmpty ())
        {
          if (mods.CheckByte0 ())
          {
            float& d = ldepth[0];
	    if (maxdepth > d)
	    {
	      d = maxdepth;
	      if (d > tile_max_depth) tile_max_depth = d;
	      recheck_min_depth = true;
	    }
          }
          if (mods.CheckByte1 ())
          {
            float& d = ldepth[4];
	    if (maxdepth > d)
	    {
	      d = maxdepth;
	      if (d > tile_max_depth) tile_max_depth = d;
	      recheck_min_depth = true;
	    }
          }
          if (mods.CheckByte2 ())
          {
            float& d = ldepth[8];
	    if (maxdepth > d)
	    {
	      d = maxdepth;
	      if (d > tile_max_depth) tile_max_depth = d;
	      recheck_min_depth = true;
	    }
          }
          if (mods.CheckByte3 ())
          {
            float& d = ldepth[12];
	    if (maxdepth > d)
	    {
	      d = maxdepth;
	      if (d > tile_max_depth) tile_max_depth = d;
	      recheck_min_depth = true;
	    }
          }
          if (mods.CheckByte4 ())
          {
            float& d = ldepth[16];
	    if (maxdepth > d)
	    {
	      d = maxdepth;
	      if (d > tile_max_depth) tile_max_depth = d;
	      recheck_min_depth = true;
	    }
          }
          if (mods.CheckByte5 ())
          {
            float& d = ldepth[20];
	    if (maxdepth > d)
	    {
	      d = maxdepth;
	      if (d > tile_max_depth) tile_max_depth = d;
	      recheck_min_depth = true;
	    }
          }
          if (mods.CheckByte6 ())
          {
            float& d = ldepth[24];
	    if (maxdepth > d)
	    {
	      d = maxdepth;
	      if (d > tile_max_depth) tile_max_depth = d;
	      recheck_min_depth = true;
	    }
          }
          if (mods.CheckByte7 ())
          {
            float& d = ldepth[28];
	    if (maxdepth > d)
	    {
	      d = maxdepth;
	      if (d > tile_max_depth) tile_max_depth = d;
	      recheck_min_depth = true;
	    }
          }
        }
      }
      if (recheck_min_depth)
      {
        tile_min_depth = depth[0];
        for (i = 1 ; i < 32 ; i++)
          if (depth[i] < tile_min_depth) tile_min_depth = depth[i];
      }
    }
  }
}

bool csCoverageTile::TestFullRect (float testdepth)
{
  if (tile_full)
  {
    // Tile is full so check for depth.
    int i;
    for (i = 0 ; i < 32 ; i++)
      if (testdepth <= depth[i])
        return true;
    return false;
  }
  else
  {
    // Tile is not full which means the rectangle is automatically
    // visible.
    return true;
  }
}

bool csCoverageTile::TestRect (int start, int end, float testdepth)
{
  // Tile is still empty.
  if (queue_tile_empty) return true;

  // If the depth of this rectangle is smaller than the minimum depth
  // of this tile then this rectangle is automatically visible.
  if (testdepth <= tile_min_depth) return true;

  int i;

  //@@@@@@@@@@@ This routine can be optimized considerably!

  if (!tile_full)
  {
    // Tile is not full which means we test coverage first.
    csBits64* c = coverage+start;
    for (i = start ; i <= end ; i++)
    {
      if (!c->IsFull ())
        return true;
      c++;
    }
  }

  // If the depth of this rectangle is greater than the maximum depth
  // of this tile then this rectangle cannot be visible.
  if (testdepth > tile_max_depth) return false;

  // Initialize the coverage cache.
  for (i = 0 ; i < start ; i++) coverage_cache[i].Empty ();
  for (i = start ; i <= end ; i++) coverage_cache[i].Full ();
  for (i = end ; i < 32 ; i++) coverage_cache[i].Empty ();

  // Test depth where appropriate.
  for (i = (start>>3) ; i <= (end>>3) ; i++)
  {
    float* ld = &depth[i];
    // Here we create a mask which will be set to eight 1 bits wherever
    // the corresponding depth value is relevant (i.e. testdepth < the
    // depth in the depth value). Only where the mask is true do we have
    // to check the coverage_cache.
    uint32 b1 = 0, b2 = 0;
    if (testdepth < ld[0]) b1 |= 0xff;
    if (testdepth < ld[4]) b1 |= 0xff00;
    if (testdepth < ld[8]) b1 |= 0xff0000;
    if (testdepth < ld[12]) b1 |= 0xff000000;
    if (testdepth < ld[16]) b2 |= 0xff;
    if (testdepth < ld[20]) b2 |= 0xff00;
    if (testdepth < ld[24]) b2 |= 0xff0000;
    if (testdepth < ld[28]) b2 |= 0xff000000;
    if (!b1 && !b2) continue;	// Nothing to do.
    csBits64 testmask;
    testmask.Set (b1, b2);

    int idx = i << 3;
    int j = 0;
    csBits64* cc = coverage_cache+idx;
    while (j < 8)
    {
      if (testmask.TestMask (*cc))
        return true;
      cc++;
      j++;
    }
  }
  return false;
}

bool csCoverageTile::TestRect (const csBits64& vermask, int start, int end,
  	float testdepth)
{
  // Tile is still empty.
  if (queue_tile_empty) return true;

  // If the depth of this rectangle is smaller than the minimum depth
  // of this tile then this rectangle is automatically visible.
  if (testdepth <= tile_min_depth) return true;

  int i;

  //@@@@@@@@@@@ This routine can be optimized considerably!

  if (!tile_full)
  {
    // Tile is not full which means we first test coverage.
    csBits64* c = coverage+start;
    for (i = start ; i <= end ; i++)
    {
      if (vermask.TestInvertedMask (*c))
        return true;
      c++;
    }
  }

  // If the depth of this rectangle is greater than the maximum depth
  // of this tile then this rectangle cannot be visible.
  if (testdepth > tile_max_depth) return false;

  // Initialize the coverage cache.
  for (i = 0 ; i < start ; i++) coverage_cache[i].Empty ();
  for (i = start ; i <= end ; i++) coverage_cache[i] = vermask;
  for (i = end ; i < 32 ; i++) coverage_cache[i].Empty ();

  // Test depth where appropriate.
  for (i = (start>>3) ; i <= (end>>3) ; i++)
  {
    float* ld = &depth[i];
    // Here we create a mask which will be set to eight 1 bits wherever
    // the corresponding depth value is relevant (i.e. testdepth < the
    // depth in the depth value). Only where the mask is true do we have
    // to check the coverage_cache.
    uint32 b1 = 0, b2 = 0;
    if (testdepth < ld[0]) b1 |= 0xff;
    if (testdepth < ld[4]) b1 |= 0xff00;
    if (testdepth < ld[8]) b1 |= 0xff0000;
    if (testdepth < ld[12]) b1 |= 0xff000000;
    if (testdepth < ld[16]) b2 |= 0xff;
    if (testdepth < ld[20]) b2 |= 0xff00;
    if (testdepth < ld[24]) b2 |= 0xff0000;
    if (testdepth < ld[28]) b2 |= 0xff000000;
    if (!b1 && !b2) continue;	// Nothing to do.
    csBits64 testmask;
    testmask.Set (b1, b2);

    int idx = i << 3;
    int j = 0;
    csBits64* cc = coverage_cache+idx;
    while (j < 8)
    {
      if (testmask.TestMask (*cc))
        return true;
      cc++;
      j++;
    }
  }
  return false;
}

bool csCoverageTile::TestPoint (int x, int y, float testdepth)
{
  CS_ASSERT (x >= 0 && x < 32);
  CS_ASSERT (y >= 0 && y < 64);

  // If tile is still empty we are visible.
  if (queue_tile_empty) return true;

  // First check for depth.
  int xd = x >> 3;	// Depth x coordinate.
  int yd = y >> 3;	// Depth y coordinate.
  float d = depth[(yd << 2) + xd];
  if (testdepth <= d)
  {
    // Visible regardless of coverage.
    return true;
  }

  if (tile_full)
  {
    // If tile is full we know we are not visible because depth
    // has already been checked.
    return false;
  }

  const csBits64& c = coverage[x];
  return !c.TestBit (y);
}

csPtr<iString> csCoverageTile::Debug_Dump ()
{
  scfString* rc = new scfString ();
  csString& str = rc->GetCsString ();

  csString ss;
  ss.Format ("full=%d queue_empty=%d blocks_full=%08x blocks_part=%08x\n",
  	tile_full, queue_tile_empty, blocks_full, blocks_partial);
  str.Append (ss);
  ss.Format ("  d %g,%g,%g,%g\n", depth[0], depth[1], depth[2], depth[3]);
  str.Append (ss);
  ss.Format ("  d %g,%g,%g,%g\n", depth[4], depth[5], depth[6], depth[7]);
  str.Append (ss);
  ss.Format ("  d %g,%g,%g,%g\n", depth[8], depth[9], depth[10], depth[11]);
  str.Append (ss);
  ss.Format ("  d %g,%g,%g,%g\n", depth[12], depth[13], depth[14], depth[15]);
  str.Append (ss);
  ss.Format ("  d %g,%g,%g,%g\n", depth[16], depth[17], depth[18], depth[19]);
  str.Append (ss);
  ss.Format ("  d %g,%g,%g,%g\n", depth[20], depth[21], depth[22], depth[23]);
  str.Append (ss);
  ss.Format ("  d %g,%g,%g,%g\n", depth[24], depth[25], depth[26], depth[27]);
  str.Append (ss);
  ss.Format ("  d %g,%g,%g,%g\n", depth[28], depth[29], depth[30], depth[31]);
  str.Append (ss);
  int i;
  for (i = 0 ; i < num_operations ; i++)
  {
    ss.Format ("  op %d ", i);
    str.Append (ss);
    csLineOperation& op = operations[i];
    switch (op.op)
    {
      case OP_LINE: ss.Format ("LINE %d,%d - %d,%d   dx=%d\n",
      	op.x1>>16, op.y1, op.x2>>16, op.y2, op.dx);
	str.Append (ss);
	break;
      case OP_VLINE: ss.Format ("VLINE x=%d y1=%d y2=%d\n",
      	op.x1>>16, op.y1, op.y2);
	str.Append (ss);
        break;
      case OP_FULLVLINE: ss.Format ("FULLVLINE x=%d\n", op.x1>>16);
        str.Append (ss);
        break;
      default: str.Append ("???\n");
        break;
    }
  }
  str.Append ("          1    1    2    2    3  \n");
  str.Append ("0    5    0    5    0    5    0  \n");
  for (i = 0 ; i < 64 ; i++)
  {
    int j;
    for (j = 0 ; j < 32 ; j++)
    {
      const csBits64& c = coverage[j];
      str.Append (c.TestBit (i) ? "#" : ".");
    }
    ss.Format (" %d\n", i);
    str.Append (ss);
  }

  return csPtr<iString> (rc);
}

csPtr<iString> csCoverageTile::Debug_Dump_Cache ()
{
  scfString* rc = new scfString ();
  csString& str = rc->GetCsString ();
  csString ss;

  int i;
  str.Append ("          1    1    2    2    3  \n");
  str.Append ("0    5    0    5    0    5    0  \n");
  for (i = 0 ; i < 64 ; i++)
  {
    int j;
    for (j = 0 ; j < 32 ; j++)
    {
      const csBits64& c = coverage_cache[j];
      str.Append (c.TestBit (i) ? "#" : ".");
    }
    ss.Format (" %d\n", i);
    str.Append (ss);
  }

  return csPtr<iString> (rc);
}

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csTiledCoverageBuffer)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iDebugHelper)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csTiledCoverageBuffer::DebugHelper)
  SCF_IMPLEMENTS_INTERFACE(iDebugHelper)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

bool csTiledCoverageBuffer::use_mmx = false;

csTiledCoverageBuffer::csTiledCoverageBuffer (int w, int h)
{
  SCF_CONSTRUCT_IBASE (0);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiDebugHelper);

  tiles = 0;
  dirty_left = 0;
  dirty_right = 0;
  bugplug = 0;

  Setup (w, h);
#ifdef PROC_X86
  use_mmx = csProcessorCapability::HasMMX ();
  //printf ("use_mmx=%d\n", use_mmx); fflush (stdout);
#endif
}

csTiledCoverageBuffer::~csTiledCoverageBuffer ()
{
  delete[] tiles;
  delete[] dirty_left;
  delete[] dirty_right;
}

void csTiledCoverageBuffer::Setup (int w, int h)
{
  delete[] tiles;
  delete[] dirty_left;
  delete[] dirty_right;

  width = w;
  height = h;
  num_tile_rows = (h+63)/64;
  height_64 = num_tile_rows * 64;
  CS_ASSERT (height_64 >= height);

  width_po2 = 1;
  w_shift = 0;
  while (width_po2 < width)
  {
    width_po2 <<= 1;
    w_shift++;
  }
  w_shift -= 5;
  CS_ASSERT (w_shift >= 0);

  num_tiles = (width_po2 / 32) * num_tile_rows;

  tiles = new csCoverageTile[num_tiles];
  dirty_left = new int[num_tile_rows];
  dirty_right = new int[num_tile_rows];
}

void csTiledCoverageBuffer::Initialize ()
{
  int i;
  for (i = 0 ; i < num_tiles ; i++)
  {
    tiles[i].MarkEmpty ();
    tiles[i].ClearOperations ();
  }
}

void csTiledCoverageBuffer::DrawLine (int x1, int y1, int x2, int y2,
	int yfurther)
{
  y2 += yfurther;

  if (y2 <= 0 || y1 >= height)
  {
    //------
    // Totally outside screen vertically.
    //------
    return;
  }

  if (x1 <= 0 && x2 <= 0)
  {
    //------
    // Totally on the left side. Just clamp.
    //------

    // First we need to clip vertically. This is easy to do
    // in this particular case since x=0 all the time.
    if (y1 < 0) y1 = 0;
    if (y2 >= height) y2 = height-1;

    // First calculate tile coordinates of x1,y1 and x2,y2.
    int tile_y1 = y1 >> 6;
    int tile_y2 = (y2-1) >> 6;
    csCoverageTile* tile = GetTile (0, tile_y1);

    if (tile_y1 == tile_y2)
    {
      //------
      // All is contained in one tile.
      //------
      tile->PushVLine (0, y1 & 63, (y2-1) & 63);
      MarkTileDirty (0, tile_y1);
    }
    else
    {
      //------
      // Multiple tiles. First do first tile, then intermediate tiles,
      // and finally the last tile.
      //------
      tile->PushVLine (0, y1 & 63, 63);
      MarkTileDirty (0, tile_y1);
      int t;
      for (t = tile_y1+1 ; t < tile_y2 ; t++)
      {
        tile += width_po2 >> 5;
        tile->PushFullVLine (0);
	MarkTileDirty (0, t);
      }
      tile += width_po2 >> 5;
      tile->PushVLine (0, 0, (y2-1) & 63);
      MarkTileDirty (0, tile_y2);
    }
    return;
  }
  else if (x1 >= width && x2 >= width)
  {
    //------
    // Lines on the far right can just be dropped since they
    // will have no effect on the coverage buffer.
    // However we must mark the right most tiles as dirty.
    //------

    // First we need to clip vertically for marking the vertical
    // row of tiles right at the end of the screen.
    if (y1 < 0) y1 = 0;
    if (y2 >= height) y2 = height-1;

    // First calculate tile coordinates of x1,y1 and x2,y2.
    int tile_y1 = y1 >> 6;
    int tile_y2 = (y2-1) >> 6;
    int t;
    for (t = tile_y1 ; t <= tile_y2 ; t++)
      MarkTileDirty (width_po2 >> 5, t);
    return;
  }
  else if (x1 == x2)
  {
    //------
    // If line is fully vertical we also have a special case that
    // is easier to resolve.
    //------
    // First we need to clip vertically. This is easy to do
    // in this particular case since x=0 all the time.
    if (y1 < 0) y1 = 0;
    if (y2 >= height) y2 = height-1;

    // First calculate tile coordinates of x1,y1 and x2,y2.
    int tile_x = x1 >> 5;	// tile_x1 == tile_x2
    int tile_y1 = y1 >> 6;
    int tile_y2 = (y2-1) >> 6;
    x1 &= 31;
    x1 <<= 16;

    csCoverageTile* tile = GetTile (tile_x, tile_y1);
    if (tile_y1 == tile_y2)
    {
      //------
      // All is contained in one tile.
      //------
      tile->PushVLine (x1, y1 & 63, (y2-1) & 63);
      MarkTileDirty (tile_x, tile_y1);
    }
    else
    {
      //------
      // Multiple tiles. First do first tile, then intermediate tiles,
      // and finally the last tile.
      //------
      tile->PushVLine (x1, y1 & 63, 63);
      MarkTileDirty (tile_x, tile_y1);
      int t;
      for (t = tile_y1+1 ; t < tile_y2 ; t++)
      {
        tile += width_po2 >> 5;
	tile->PushFullVLine (x1);
	MarkTileDirty (tile_x, t);
      }
      tile += width_po2 >> 5;
      tile->PushVLine (x1, 0, (y2-1) & 63);
      MarkTileDirty (tile_x, tile_y2);
    }
    return;
  }

  //------
  // We don't have any of the trivial vertical cases.
  // So we must clip first.
  //------
  int old_x1 = x1;
  int old_x2 = x2;
  int old_y1 = y1;
  int old_y2 = y2;
  csRect r (0, 0, width-1, height-1-yfurther);
  y2 -= yfurther;
  bool outside = !r.ClipLineSafe (x1, y1, x2, y2);

  // @@@ Is the below good?
  if (!outside && y1 == y2) return;  // Return if clipping results in one pixel.
  y2 += yfurther;

  if (outside)
  {
    // We know the line is fully outside. Now we have to discover if the
    // line is on the left or the right of the screen.
    // First restore old coordinates because ClipLineSafe may have
    // partially modified them.
    bool right_side;
    x1 = old_x1;
    x2 = old_x2;
    y1 = old_y1;
    y2 = old_y2;
    if (x1 < width && x2 < width)
    {
      // We're sure the line is fully on the left side.
      right_side = false;
    }
    else if (x1 >= 0 && x2 >= 0)
    {
      // We're sure the line is fully on the right side.
      right_side = true;
    }
    else
    {
      // Most general case. We have to check a bit better.
      // We will check the x value where y=0 unless y=0 is outside
      // the line vertically in which case we will use y=y1.
      int y = 0;
      if (y1 > y) y = y1;
      int x = QInt (x1 + float (y-y1) * float (x2 - x1)
      		/ float (y2-y1));
      right_side = (x > 0);
    }

    // Make sure the y coordinates are clipped.
    if (y1 < 0) y1 = 0;
    if (y2 >= height) y2 = height-1;

    // First calculate tile coordinates.
    int tile_y1 = y1 >> 6;
    int tile_y2 = (y2-1) >> 6;

    if (right_side)
    {
      //------
      // Lines on the far right can just be dropped since they
      // will have no effect on the coverage buffer.
      // However we must mark the right-most tiles as dirty.
      //------
      int t;
      for (t = tile_y1 ; t <= tile_y2 ; t++)
        MarkTileDirty (width_po2 >> 5, t);
      return;
    }
    else
    {
      //------
      // Totally on the left side. Just clamp.
      //------
      csCoverageTile* tile = GetTile (0, tile_y1);
      if (tile_y1 == tile_y2)
      {
        //------
        // All is contained in one tile.
        //------
        tile->PushVLine (0, y1 & 63, (y2-1) & 63);
        MarkTileDirty (0, tile_y1);
      }
      else
      {
        //------
        // Multiple tiles. First do first tile, then intermediate tiles,
        // and finally the last tile.
        //------
        tile->PushVLine (0, y1 & 63, 63);
        MarkTileDirty (0, tile_y1);
        int t;
        for (t = tile_y1+1 ; t < tile_y2 ; t++)
        {
          tile += width_po2 >> 5;
          tile->PushFullVLine (0);
	  MarkTileDirty (0, t);
        }
        tile += width_po2 >> 5;
        tile->PushVLine (0, 0, (y2-1) & 63);
        MarkTileDirty (0, tile_y2);
      }
      return;
    }
  }

  //------
  // We know that at least part of the line is on screen.
  // We cannot ignore the part of the line that has been clipped away but 
  // is still between the top and bottom of the screen. The reason is that
  // on the left side this must be clamped to 0 and on the right side we
  // need to mark the right-most tiles as dirty. We will handle this
  // here before we draw the remainder of the line.
  //------

  // First clip the old y coordinates to the screen vertically.
  if (old_y1 < 0) old_y1 = 0;
  if (old_y2 >= height) old_y2 = height-1;

  if (old_y1 < y1)
  {
    if (x1 <= 0)
    {
      // We have an initial part that needs to be clamped.
      int tile_y1 = old_y1 >> 6;
      int tile_y2 = (y1-1) >> 6;
      csCoverageTile* tile = GetTile (0, tile_y1);
      if (tile_y1 == tile_y2)
      {
        tile->PushVLine (0, old_y1 & 63, (y1-1) & 63);
        MarkTileDirty (0, tile_y1);
      }
      else
      {
        tile->PushVLine (0, old_y1 & 63, 63);
        MarkTileDirty (0, tile_y1);
        int t;
        for (t = tile_y1+1 ; t < tile_y2 ; t++)
        {
          tile += width_po2 >> 5;
	  tile->PushFullVLine (0);
	  MarkTileDirty (0, t);
        }
        tile += width_po2 >> 5;
        tile->PushVLine (0, 0, (y1-1) & 63);
        MarkTileDirty (0, tile_y2);
      }
    }
    else if (x1 >= width-1)
    {
      // We have an initial part for which we need to mark tiles dirty.
      int tile_y1 = old_y1 >> 6;
      int tile_y2 = (y1-1) >> 6;
      int t;
      for (t = tile_y1 ; t <= tile_y2 ; t++)
        MarkTileDirty (width_po2 >> 5, t);
    }
  }
  if (old_y2 > y2)
  {
    if (x2 <= 0)
    {
      // We have a final part that needs to be clamped.
      int tile_y1 = y2 >> 6;
      int tile_y2 = (old_y2-1) >> 6;
      csCoverageTile* tile = GetTile (0, tile_y1);
      if (tile_y1 == tile_y2)
      {
        tile->PushVLine (0, y2 & 63, (old_y2-1) & 63);
        MarkTileDirty (0, tile_y1);
      }
      else
      {
        tile->PushVLine (0, y2 & 63, 63);
        MarkTileDirty (0, tile_y1);
        int t;
        for (t = tile_y1+1 ; t < tile_y2 ; t++)
        {
          tile += width_po2 >> 5;
	  tile->PushFullVLine (0);
	  MarkTileDirty (0, t);
        }
        tile += width_po2 >> 5;
        tile->PushVLine (0, 0, (old_y2-1) & 63);
        MarkTileDirty (0, tile_y2);
      }
    }
    else if (x2 >= width-1)
    {
      // We have a final part for which we need to mark tiles dirty.
      int tile_y1 = y2 >> 6;
      int tile_y2 = (old_y2-1) >> 6;
      int t;
      for (t = tile_y1 ; t <= tile_y2 ; t++)
        MarkTileDirty (width_po2 >> 5, t);
    }
  }

  //------
  // First calculate tile coordinates of x1,y1 and x2,y2.
  // Now we know that this line segment is fully on screen.
  //------
  int tile_x1 = x1 >> 5;
  int tile_y1 = y1 >> 6;
  int tile_x2 = x2 >> 5;
  int tile_y2 = (y2-1) >> 6;

# define xmask ((32<<16)-1)
# define ymask ((64<<16)-1)

  if (tile_x1 == tile_x2 && tile_y1 == tile_y2)
  {
    //------
    // Easy case. The line segment is fully inside one tile.
    //------
    int dy = y2-y1;
    int dx = ((x2-x1)<<16) / (dy-yfurther);
    csCoverageTile* tile = GetTile (tile_x1, tile_y1);
// @@@@@@@ DOUBLE CHECK THE THING BELOW TO SEE IF IT IS REALLY OK!!!
    tile->PushLine ((x1 & 31) << 16, y1 & 63, ((x2 & 31) << 16)
    	-(yfurther ? 0 : dx),
    	(y2-1) & 63, dx);
    MarkTileDirty (tile_x1, tile_y1);
    return;
  }
  else if (tile_x1 == tile_x2)
  {
    //------
    // Line is nearly vertical. This means we will stay in the same
    // column of tiles.
    //------
    int dy = y2-y1;
    int dx = ((x2-x1)<<16) / (dy-yfurther);
    x1 <<= 16;
    x2 <<= 16;
    int x = x1 + dx * (63 - (y1 & 63));
    csCoverageTile* tile = GetTile (tile_x1, tile_y1);
    tile->PushLine (x1 & xmask, y1 & 63, x & xmask, 63, dx);
    MarkTileDirty (tile_x1, tile_y1);
    x += dx;
    int t;
    for (t = tile_y1+1 ; t < tile_y2 ; t++)
    {
      tile += width_po2 >> 5;
      int xt = x + (dx << 6) - dx;
      tile->PushLine (x & xmask, 0, xt & xmask, 63, dx);
      MarkTileDirty (tile_x1, t);
      x = xt+dx;
    }
    tile += width_po2 >> 5;
    tile->PushLine (x & xmask, 0, (x2 & xmask) - dx, (y2-1) & 63, dx);
    MarkTileDirty (tile_x1, tile_y2);
    return;
  }

  //------
  // This is the most general case.
  // @@@ The loops below can be done more efficiently.
  //------
  int dy = y2-y1;
  int dx = ((x2-x1)<<16) / (dy-yfurther);

  if (tile_y1 == tile_y2)
  {
    //------
    // Line is nearly horizontal. This means we will stay in the same
    // row of tiles.
    //------
    MarkTileDirty (tile_x1, tile_y1);
    MarkTileDirty (tile_x2, tile_y1);
    // Calculate the slope of the line.
    int x = x1<<16;
    int y = y1;

    //------
    // Here is the remainder of the line until we go out screen again.
    //------
    bool need_to_finish = dy > 0;
    int last_x = x;
    int last_y = y;
    int cur_tile_x = tile_x1;
    csCoverageTile* stile = GetTile (tile_x1, tile_y1);
    while (dy > 0)
    {
      int tile_x = x >> (16+5);
      if (cur_tile_x != tile_x)
      {
        csCoverageTile* tile = stile + (cur_tile_x-tile_x1);
        tile->PushLine (last_x & xmask, last_y & 63, (x-dx) & xmask,
      	  (y-1) & 63, dx);
        cur_tile_x = tile_x;
        last_x = x;
        last_y = y;
      }

      x += dx;
      y++;
      dy--;
    }

    if (need_to_finish)
    {
      csCoverageTile* tile = stile + (cur_tile_x-tile_x1);
      tile->PushLine (last_x & xmask, last_y & 63, (x-dx) & xmask,
    	  (y-1) & 63, dx);
    }
    return;
  }

  // Calculate the slope of the line.
  int x = x1<<16;
  int y = y1;

  //------
  // Here is the remainder of the line until we go out screen again.
  //------
  bool need_to_finish = dy > 0;
  int last_x = x;
  int last_y = y;
  int cur_tile_x = tile_x1;
  int cur_tile_y = tile_y1;
  while (dy > 0)
  {
    int tile_x = x >> (16+5);
    int tile_y = y >> 6;
    if (cur_tile_x != tile_x || cur_tile_y != tile_y)
    {
      csCoverageTile* tile = GetTile (cur_tile_x, cur_tile_y);
      tile->PushLine (last_x & xmask, last_y & 63, (x-dx) & xmask,
      	(y-1) & 63, dx);
      MarkTileDirty (cur_tile_x, cur_tile_y);
      cur_tile_x = tile_x;
      cur_tile_y = tile_y;
      last_x = x;
      last_y = y;
    }

    x += dx;
    y++;
    dy--;
  }

  if (need_to_finish)
  {
    csCoverageTile* tile = GetTile (cur_tile_x, cur_tile_y);
    tile->PushLine (last_x & xmask, last_y & 63, (x-dx) & xmask,
    	(y-1) & 63, dx);
    MarkTileDirty (cur_tile_x, cur_tile_y);
  }
}

bool csTiledCoverageBuffer::DrawPolygon (csVector2* verts, int num_verts,
	csBox2Int& bbox)
{
  int i, j;

  //---------
  // First we copy the vertices to xa/ya. In the mean time
  // we convert to integer and also search for the top vertex (lowest
  // y coordinate) and bottom vertex.
  //@@@ TODO: pre-shift x with 16
  //---------
  int xa[128], ya[128];
  int top_vt = 0;
  int bot_vt = 0;
  xa[0] = QRound (verts[0].x);
  ya[0] = QRound (verts[0].y);
  bbox.minx = bbox.maxx = xa[0];
  bbox.miny = bbox.maxy = ya[0];
  for (i = 1 ; i < num_verts ; i++)
  {
    xa[i] = QRound (verts[i].x);
    ya[i] = QRound (verts[i].y);

    if (xa[i] < bbox.minx) bbox.minx = xa[i];
    else if (xa[i] > bbox.maxx) bbox.maxx = xa[i];

    if (ya[i] < bbox.miny)
    {
      bbox.miny = ya[i];
      top_vt = i;
    }
    else if (ya[i] > bbox.maxy)
    {
      bbox.maxy = ya[i];
      bot_vt = i;
    }
  }

  if (bbox.maxx <= 0) return false;
  if (bbox.maxy <= 0) return false;
  if (bbox.minx >= width) return false;
  if (bbox.miny >= height) return false;

  //---------
  // First initialize dirty_left and dirty_right for every row.
  //---------
  for (i = 0 ; i < num_tile_rows ; i++)
  {
    dirty_left[i] = 1000;
    dirty_right[i] = -1;
  }

  //---------
  // Draw all lines.
  //---------
  j = num_verts-1;
  for (i = 0 ; i < num_verts ; i++)
  {
    if (ya[i] != ya[j])
    {
      int xa1, xa2, ya1, ya2;
      if (ya[i] < ya[j])
      {
	xa1 = xa[i];
        xa2 = xa[j];
	ya1 = ya[i];
	ya2 = ya[j];
      }
      else
      {
	xa1 = xa[j];
        xa2 = xa[i];
	ya1 = ya[j];
	ya2 = ya[i];
      }
      DrawLine (xa1, ya1, xa2, ya2, ya2 == bbox.maxy ? 1 : 0);
    }
    j = i;
  }

  return true;
}

bool csTiledCoverageBuffer::DrawOutline (csVector2* verts, int num_verts,
	bool* used_verts,
	int* edges, int num_edges,
	csBox2Int& bbox)
{
  int i;

  //---------
  // First we copy the vertices to xa/ya. In the mean time
  // we convert to integer and also search for the top vertex (lowest
  // y coordinate) and bottom vertex.
  //@@@ TODO: pre-shift x with 16
  //---------
  static int* xa = 0, * ya = 0;
  static int num_xa = 0;
  if (num_verts > num_xa)
  {
    delete[] xa;
    delete[] ya;
    num_xa = num_verts;
    // @@@ MEMORY LEAK!!!
    xa = new int[num_xa];
    ya = new int[num_xa];
  }
  int top_vt = -1;
  int bot_vt = -1;
  bbox.minx = 1000000;
  bbox.maxx = -1000000;
  bbox.miny = 1000000;
  bbox.maxy = -1000000;
  for (i = 0 ; i < num_verts ; i++)
  {
    if (used_verts[i])
    {
      xa[i] = QRound (verts[i].x);
      ya[i] = QRound (verts[i].y);

      if (xa[i] < bbox.minx) bbox.minx = xa[i];
      if (xa[i] > bbox.maxx) bbox.maxx = xa[i];

      if (ya[i] < bbox.miny)
      {
        bbox.miny = ya[i];
        top_vt = i;
      }
      if (ya[i] > bbox.maxy)
      {
        bbox.maxy = ya[i];
        bot_vt = i;
      }
    }
  }

  if (bbox.maxx <= 0) return false;
  if (bbox.maxy <= 0) return false;
  if (bbox.minx >= width) return false;
  if (bbox.miny >= height) return false;

  //---------
  // First initialize dirty_left and dirty_right for every row.
  //---------
  for (i = 0 ; i < num_tile_rows ; i++)
  {
    dirty_left[i] = 1000;
    dirty_right[i] = -1;
  }

  //---------
  // Draw all edges.
  //---------
  for (i = 0 ; i < num_edges ; i++)
  {
    int vt1 = *edges++;
    int vt2 = *edges++;
    int ya1 = ya[vt1];
    int ya2 = ya[vt2];
    if (ya1 != ya2)
    {
      int xa1, xa2;
      if (ya1 < ya2)
      {
	xa1 = xa[vt1];
        xa2 = xa[vt2];
      }
      else
      {
        int y = ya1;
	ya1 = ya2;
	ya2 = y;
	xa1 = xa[vt2];
        xa2 = xa[vt1];
      }
      DrawLine (xa1, ya1, xa2, ya2, 0);	// @@@ last parm 1 on last row?
    }
  }

  return true;
}

void csTiledCoverageBuffer::InsertPolygon (csVector2* verts, int num_verts,
	float max_depth)
{
  csBox2Int bbox;
  if (!DrawPolygon (verts, num_verts, bbox))
    return;

  int tx, ty;
  int startrow, endrow;
  startrow = bbox.miny >> 6;
  if (startrow < 0) startrow = 0;
  endrow = bbox.maxy >> 6;
  if (endrow >= num_tile_rows) endrow = num_tile_rows-1;

  for (ty = startrow ; ty <= endrow ; ty++)
  {
    csBits64 fvalue;
    fvalue.Empty ();
    csCoverageTile* tile = GetTile (dirty_left[ty], ty);
//printf ("ty=%d dirty_left=%d dirty_right=%d\n", ty, dirty_left[ty], dirty_right[ty]); fflush (stdout);
    int dr = dirty_right[ty];
    if (dr >= (width_po2 >> 5)) dr = (width_po2 >> 5)-1;
    for (tx = dirty_left[ty] ; tx <= dr ; tx++)
    {
      tile->Flush (fvalue, max_depth);
      tile++;
    }
  }
}

void csTiledCoverageBuffer::InsertOutline (csVector2* verts, int num_verts,
	bool* used_verts,
	int* edges, int num_edges, float max_depth)
{
  csBox2Int bbox;
  if (!DrawOutline (verts, num_verts, used_verts, edges, num_edges, bbox))
    return;

  int tx, ty;
  int startrow, endrow;
  startrow = bbox.miny >> 6;
  if (startrow < 0) startrow = 0;
  endrow = bbox.maxy >> 6;
  if (endrow >= num_tile_rows) endrow = num_tile_rows-1;

  for (ty = startrow ; ty <= endrow ; ty++)
  {
    csBits64 fvalue;
    fvalue.Empty ();
    csCoverageTile* tile = GetTile (dirty_left[ty], ty);
    int dr = dirty_right[ty];
    if (dr >= (width_po2 >> 5)) dr = (width_po2 >> 5)-1;
    for (tx = dirty_left[ty] ; tx <= dr ; tx++)
    {
      tile->Flush (fvalue, max_depth);
      tile++;
    }
  }
}

bool csTiledCoverageBuffer::TestRectangle (const csBox2& rect, float min_depth)
{
  csBox2Int bbox;
  if (rect.MaxX () > 10000.0) bbox.maxx = 10000;
  else
  {
    if (rect.MaxX () <= 0) return false;
    bbox.maxx = QRound (rect.MaxX ());
  }
  if (rect.MaxY () > 10000.0) bbox.maxy = 10000;
  else
  {
    if (rect.MaxY () <= 0) return false;
    bbox.maxy = QRound (rect.MaxY ());
  }

  if (rect.MinX () < -10000.0) bbox.minx = -10000;
  else
  {
    if (rect.MinX () > 10000.0) return false;
    bbox.minx = QRound (rect.MinX ());
    if (bbox.minx >= width) return false;
  }
  if (rect.MinY () < -10000.0) bbox.miny = -10000;
  else
  {
    if (rect.MinY () > 10000.0) return false;
    bbox.miny = QRound (rect.MinY ());
    if (bbox.miny >= height) return false;
  }

  int tx, ty;
  if (bbox.miny < 0) bbox.miny = 0;
  int startrow = bbox.miny >> 6;
  if (bbox.maxy >= height) bbox.maxy = height-1;
  int endrow = bbox.maxy >> 6;
  CS_ASSERT (endrow < num_tile_rows);
  if (bbox.minx < 0) bbox.minx = 0;
  int startcol = bbox.minx >> 5;
  if (bbox.maxx >= width) bbox.maxx = width-1;
  CS_ASSERT (bbox.maxx < width);
  int endcol = bbox.maxx >> 5;

  csBits64 vermask;
  int start_x = bbox.minx & 31;
  int end_x = bbox.maxx & 31;

  for (ty = startrow ; ty <= endrow ; ty++)
  {
    bool do_vermask = false;
    vermask.Full ();
    if (ty == startrow && (bbox.miny & 63) != 0)
    {
      vermask = csCoverageTile::precalc_end_lines[bbox.miny & 63];
      do_vermask = true;
    }
    if (ty == endrow && (bbox.maxy & 63) != 63)
    {
      vermask &= csCoverageTile::precalc_start_lines[bbox.maxy & 63];
      do_vermask = true;
    }

    csCoverageTile* tile = GetTile (startcol, ty);
    for (tx = startcol ; tx <= endcol ; tx++)
    {
      int sx = 0, ex = 31;
      bool do_hor = false;
      if (tx == startcol && start_x != 0)
      {
        sx = start_x;
	do_hor = true;
      }
      if (tx == endcol && end_x != 31)
      {
        ex = end_x;
	do_hor = true;
      }

      if (do_hor || do_vermask)
      {
        if (!do_vermask)
	{
          if (tile->TestRect (sx, ex, min_depth))
	    return true;
	}
	else
	{
          if (tile->TestRect (vermask, sx, ex, min_depth))
	    return true;
        }
      }
      else
      {
        if (tile->TestFullRect (min_depth))
          return true;
      }
      tile++;
    }
  }
  return false;
}

bool csTiledCoverageBuffer::TestPoint (const csVector2& point, float min_depth)
{
  int xi, yi;
  xi = QRound (point.x);
  yi = QRound (point.y);

  if (xi < 0) return false;
  if (yi < 0) return false;
  if (xi >= width) return false;
  if (yi >= height) return false;

  int ty = yi >> 6;
  int tx = xi >> 5;

  csCoverageTile* tile = GetTile (tx, ty);
  return tile->TestPoint (xi & 31, yi & 63, min_depth);
}

#if defined(THIS_IS_UNUSED)
static void DrawZoomedPixel (iGraphics2D* g2d, int x, int y, int col, int zoom)
{
  if (zoom == 1)
    g2d->DrawPixel (x, y, col);
  else if (zoom == 2)
  {
    x <<= 1;
    y <<= 1;
    g2d->DrawPixel (x+0, y+0, col);
    g2d->DrawPixel (x+1, y+0, col);
    g2d->DrawPixel (x+0, y+1, col);
    g2d->DrawPixel (x+1, y+1, col);
  }
  else if (zoom == 3)
  {
    x *= 3;
    y *= 3;
    g2d->DrawPixel (x+0, y+0, col);
    g2d->DrawPixel (x+1, y+0, col);
    g2d->DrawPixel (x+2, y+0, col);
    g2d->DrawPixel (x+0, y+1, col);
    g2d->DrawPixel (x+1, y+1, col);
    g2d->DrawPixel (x+2, y+1, col);
    g2d->DrawPixel (x+0, y+2, col);
    g2d->DrawPixel (x+1, y+2, col);
    g2d->DrawPixel (x+2, y+2, col);
  }
}
#endif

csPtr<iString> csTiledCoverageBuffer::Debug_Dump ()
{
  scfString* rc = new scfString ();
  csString& str = rc->GetCsString ();

  int x, y, tx, ty, i, j;
  for (ty = 0 ; ty < num_tile_rows ; ty++)
  {
    for (y = 0 ; y < 8 ; y++)
    {
      for (tx = 0 ; tx < (width_po2 >> 5) ; tx++)
      {
        csCoverageTile* tile = GetTile (tx, ty);
	for (x = 0 ; x < 4 ; x++)
	{
          int cnt = 0;
	  if (!tile->queue_tile_empty)
	    for (i = 0 ; i < 8 ; i++)
	      for (j = 0 ; j < 8 ; j++)
	        cnt += tile->coverage[x*8+i].TestBit (y*8+j);
	  char c;
	  if (cnt == 64) c = '#';
	  else if (cnt > 54) c = '*';
	  else if (cnt == 0) c = ' ';
	  else if (cnt < 10) c = '.';
	  else c = 'x';
	  str.Append (c);
	}
      }
      str.Append ('\n');
    }
  }

  return csPtr<iString> (rc);
}

void csTiledCoverageBuffer::Debug_Dump (iGraphics3D* g3d, int /*zoom*/)
{
  iGraphics2D* g2d = g3d->GetDriver2D ();
  int colpoint = g2d->FindRGB (255, 0, 0);

  int x, y, tx, ty, i, j;
  for (ty = 0 ; ty < num_tile_rows ; ty++)
  {
    for (y = 0 ; y < 8 ; y++)
    {
      for (tx = 0 ; tx < (width_po2 >> 5) ; tx++)
      {
        g2d->DrawPixel (tx*32, ty*64, colpoint);

        csCoverageTile* tile = GetTile (tx, ty);
	for (x = 0 ; x < 4 ; x++)
	{
	  float depth = tile->depth[y*4+x];
	  for (i = 0 ; i < 8 ; i++)
	    for (j = 0 ; j < 8 ; j++)
	    {
	      bool val;
	      if (tile->queue_tile_empty)
	        val = false;
	      else
	        val = tile->coverage[x*8+i].TestBit (y*8+j);
	      if (val)
	      {
	        int c = 255-int (depth);
		if (c < 50) c = 50;
		int col = g2d->FindRGB (c, c, c);
	        g2d->DrawPixel (tx*32+x*8+i, ty*64+y*8+j, col);
	      }
	    }
	}
      }
    }
  }
}

#if 0
static float rnd (int totrange, int leftpad, int rightpad)
{
  return float (((rand () >> 4) % (totrange-leftpad-rightpad)) + leftpad);
}
#endif

#define COV_ASSERT(test,msg) \
  if (!(test)) \
  { \
    str.Format ("csTiledCoverageBuffer failure (%d,%s): %s\n", int(__LINE__), \
    	#msg, #test); \
    return csPtr<iString> (rc); \
  }

csPtr<iString> csTiledCoverageBuffer::Debug_UnitTest ()
{
  Setup (640, 480);

  scfString* rc = new scfString ();
  csString& str = rc->GetCsString ();

  csVector2 poly[4];
  //csCoverageTile* t;
  //iString* s;

  Initialize ();
  COV_ASSERT (TestPoint (csVector2 (100, 100), 5) == true, "tp");
  poly[0].Set (50, 50);
  poly[1].Set (600, 50);
  poly[2].Set (600, 430);
  poly[3].Set (50, 430);
  InsertPolygon (poly, 4, 10.0);
  COV_ASSERT (TestPoint (csVector2 (100, 100), 5) == true, "tp");
  COV_ASSERT (TestPoint (csVector2 (100, 100), 15) == false, "tp");
  COV_ASSERT (TestPoint (csVector2 (599, 100), 5) == true, "tp");
  COV_ASSERT (TestPoint (csVector2 (599, 100), 15) == false, "tp");
  COV_ASSERT (TestPoint (csVector2 (601, 100), 5) == true, "tp");
  COV_ASSERT (TestPoint (csVector2 (601, 100), 15) == true, "tp");

  rc->DecRef ();
  return 0;
}

csTicks csTiledCoverageBuffer::Debug_Benchmark (int /*num_iterations*/)
{
  Setup (640, 480);

  csTicks start = csGetTicks ();
  csTicks end = csGetTicks ();
  return end-start;
}

