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
#include "qint.h"
#include "qsqrt.h"
#include "csgeom/box.h"
#include "csgeom/math3d.h"
#include "covbuf.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/txtmgr.h"

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csCoverageBuffer)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iDebugHelper)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csCoverageBuffer::DebugHelper)
  SCF_IMPLEMENTS_INTERFACE(iDebugHelper)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csCoverageBuffer::csCoverageBuffer (int w, int h)
{
  SCF_CONSTRUCT_IBASE (NULL);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiDebugHelper);

  buffer = NULL;
  scr_buffer = NULL;
  partcols = NULL;
  depth_buffer = NULL;

  Setup (w, h);
}

csCoverageBuffer::~csCoverageBuffer ()
{
  delete[] buffer;
  delete[] scr_buffer;
  delete[] partcols;
  delete[] depth_buffer;
}

void csCoverageBuffer::Setup (int w, int h)
{
  delete[] buffer;
  delete[] scr_buffer;
  delete[] partcols;
  delete[] depth_buffer;

  width = w;
  height = h;
  numrows = (h+31)/32;

  width_po2 = 1;
  w_shift = 0;
  while (width_po2 < width)
  {
    width_po2 <<= 1;
    w_shift++;
  }

  bufsize = width_po2 * numrows;

  buffer = new uint32[bufsize];
  scr_buffer = new uint32[bufsize];
  partcols = new int[numrows];

  depth_buffer_size = (width_po2 >> 3) * ((height+7) >> 3);
  depth_buffer = new float[depth_buffer_size];
}

void csCoverageBuffer::InitializePolygonBuffer ()
{
  if (width_po2 == width)
    memset (buffer, 0, bufsize << 2);
  else
  {
    int i;
    for (i = 0 ; i < numrows ; i++)
    {
      memset (buffer+(i<<w_shift), 0, width << 2);
    }
  }
}

void csCoverageBuffer::InitializePolygonBuffer (const csBox2Int& bbox)
{
  // @@@ Make a special case for po2 width? (1024 for example)

  int startcol = bbox.minx-40;
  if (startcol < 0) startcol = 0;
  int endcol = bbox.maxx+40;
  if (endcol >= width) endcol = width-1;
  int horsize = endcol-startcol+1;

  int startrow = bbox.miny >> 5;
  if (startrow < 0) startrow = 0;
  int endrow = bbox.maxy >> 5;
  if (endrow >= numrows) endrow = numrows-1;

  int i;
  for (i = startrow ; i <= endrow ; i++)
  {
    memset (buffer+(i<<w_shift)+startcol, 0, horsize << 2);
  }
}

void csCoverageBuffer::Initialize ()
{
  memset (scr_buffer, 0, bufsize << 2);
  int i;
  for (i = 0 ; i < numrows ; i++)
  {
    partcols[i] = width;
  }
  memset (depth_buffer, 0, depth_buffer_size << 2);
  CS_ASSERT (*depth_buffer == 0.0);
}

bool csCoverageBuffer::IsFull ()
{
// @@@ This function is maybe not completely ok now we are using
// a depth buffer too... Needs investigation.
  int i;
  for (i = 0 ; i < numrows ; i++)
  {
    if (partcols[i])
      return false;
  }
  return true;
}

void csCoverageBuffer::DrawLeftLine (int x1, int y1, int x2, int y2,
	int yfurther)
{
  y2 += yfurther;

  if (y2 < 0 || y1 >= height)
  {
    //------
    // Totally outside screen vertically.
    //------
    return;
  }

  if (x1 < 0 && x2 < 0)
  {
    //------
    // Totally on the left side. Just clamp.
    //------

    // First we need to clip vertically. This is easy to do
    // in this particular case since x=0 all the time.
    if (y1 < 0) y1 = 0;
    if (y2 >= height) y2 = height-1;

    int dy = y2-y1;
    int y = y1;
    while (dy > 0)
    {
      // @@@ Optimize!
      uint32* buf = &buffer[(y>>5) << w_shift];
      buf[0] ^= 1 << (y & 0x1f);
      y++;
      dy--;
    }
    return;
  }
  else if (x1 >= width && x2 >= width)
  {
    //------
    // Lines on the far right can just be dropped since they
    // will have no effect on the coverage buffer.
    //------
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

    int dy = y2-y1;
    int y = y1;
    while (dy > 0)
    {
      // @@@ Optimize!
      uint32* buf = &buffer[(y>>5) << w_shift];
      buf[x1] ^= 1 << (y & 0x1f);
      y++;
      dy--;
    }
    return;
  }

  //------
  // We don't have any of the trivial horizontal cases.
  // So we must clip vertically first.
  //------
  if (y1 < 0)
  {
    x1 = x1 + ( (0-y1) * (x2-x1) ) / (y2-yfurther-y1);
    y1 = 0;
  }
  if (y2 >= height)
  {
    x2 = x1 + ( (height-1-y1) * (x2-x1) ) / (y2-yfurther-y1);
    y2 = height-1;
    yfurther = 0;
  }

  if (x1 >= 0 && x2 >= 0 && x1 < width && x2 < width)
  {
    //------
    // The normal case, no clipping needed at all.
    //------
    int dy = (y2-y1)+1;
    int x = x1<<16;
    int y = y1;
    int dx = ((x2-x1)<<16) / (dy-yfurther);
    dy--;
    while (dy > 0)
    {
      uint32* buf = &buffer[(y>>5) << w_shift];
      buf[x>>16] ^= 1 << (y & 0x1f);
      x += dx;
      y++;
      dy--;
    }
  }
  else
  {
    //------
    // In this case we need to clip horizontally.
    //------
    int dy = (y2-y1)+1;
    int x = x1<<16;
    int y = y1;
    int dx = ((x2-x1)<<16) / (dy-yfurther);
    dy--;
    while (dy > 0)
    {
      uint32* buf = &buffer[(y>>5) << w_shift];
      if (x < 0)
      {
        buf[0] ^= 1 << (y & 0x1f);
      }
      else
      {
        int xn = x>>16;
	if (xn < width)
          buf[xn] ^= 1 << (y & 0x1f);
        else
	{
	  if (dx > 0) break;	// We can stop here.
	}
      }
      x += dx;
      y++;
      dy--;
    }
  }
}

void csCoverageBuffer::DrawRightLine (int x1, int y1, int x2, int y2,
	int yfurther)
{
  y2 += yfurther;
  if (y2 < 0 || y1 >= height)
  {
    //------
    // Totally outside screen vertically.
    //------
    return;
  }

  if (x1 < 0 && x2 < 0)
  {
    //------
    // Totally on the left side. Just clamp.
    //------

    // First we need to clip vertically. This is easy to do
    // in this particular case since x=0 all the time.
    if (y1 < 0) y1 = 0;
    if (y2 >= height) y2 = height-1;

    int dy = y2-y1;
    int y = y1;
    while (dy > 0)
    {
      // @@@ Optimize!
      uint32* buf = &buffer[(y>>5) << w_shift];
      buf[0] ^= 1 << (y & 0x1f);
      y++;
      dy--;
    }
    return;
  }
  else if (x1 >= width && x2 >= width)
  {
    //------
    // Lines on the far right can just be dropped since they
    // will have no effect on the coverage buffer.
    //------
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

    int dy = y2-y1;
    int y = y1;
    while (dy > 0)
    {
      // @@@ Optimize!
      uint32* buf = &buffer[(y>>5) << w_shift];
      buf[x1] ^= 1 << (y & 0x1f);
      y++;
      dy--;
    }
    return;
  }

  //------
  // We don't have any of the trivial horizontal cases.
  // So we must clip vertically first.
  //------
  if (y1 < 0)
  {
    x1 = x1 + ( (0-y1) * (x2-x1) ) / (y2-yfurther-y1);
    y1 = 0;
  }
  if (y2 >= height)
  {
    x2 = x1 + ( (height-1-y1) * (x2-x1) ) / (y2-yfurther-y1);
    y2 = height-1;
    yfurther = 0;
  }

  if (x1 >= 0 && x2 >= 0 && x1 < width && x2 < width)
  {
    //------
    // The normal case, no clipping needed at all.
    //------
    int dy = (y2-y1)+1;
    int x = x1<<16;
    int y = y1;
    int dx = ((x2-x1)<<16) / (dy-yfurther);
    int dx2 = dx>>1;
    x += dx2;
    dy--;
    while (dy > 0)
    {
      uint32* buf = &buffer[(y>>5) << w_shift];
      buf[x>>16] ^= 1 << (y & 0x1f);
      x += dx;
      y++;
      dy--;
    }
  }
  else
  {
    //------
    // In this case we need to clip horizontally.
    //------
    int dy = (y2-y1)+1;
    int x = x1<<16;
    int y = y1;
    int dx = ((x2-x1)<<16) / (dy-yfurther);
    int dx2 = dx>>1;
    x += dx2;
    dy--;
    while (dy > 0)
    {
      uint32* buf = &buffer[(y>>5) << w_shift];
      if (x < 0)
      {
        buf[0] ^= 1 << (y & 0x1f);
      }
      else
      {
        int xn = x>>16;
	if (xn < width)
          buf[xn] ^= 1 << (y & 0x1f);
        else
	{
	  if (dx > 0) break;	// We can stop here.
	}
      }
      x += dx;
      y++;
      dy--;
    }
  }
}

bool csCoverageBuffer::DrawPolygon (csVector2* verts, int num_verts,
	csBox2Int& bbox, int shift)
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
  xa[0] = QInt (verts[0].x);
  ya[0] = QInt (verts[0].y);
  bbox.minx = bbox.maxx = xa[0];
  bbox.miny = bbox.maxy = ya[0];
  for (i = 1 ; i < num_verts ; i++)
  {
    xa[i] = QInt (verts[i].x);
    ya[i] = QInt (verts[i].y);

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

  InitializePolygonBuffer (bbox);

  //---------
  // First find out in which direction the 'right' lines go.
  //---------
  //@@@@ THIS IS NOT VERY OPTIMAL!
  int top_vt_right = (top_vt+1)%num_verts;
  int top_vt_left = (top_vt-1+num_verts)%num_verts;
  int dxdyR = ((xa[top_vt_right]-xa[top_vt])<<16)
  	/ (ya[top_vt_right]-ya[top_vt]+1);
  int dxdyL = ((xa[top_vt_left]-xa[top_vt])<<16)
  	/ (ya[top_vt_left]-ya[top_vt]+1);
  int dir_right = (dxdyR > dxdyL) ? 1 : -1;

  //---------
  // Draw all right lines.
  //---------
  int dir = dir_right;
  i = top_vt;
  j = (i+num_verts+dir)%num_verts;

  while (i != bot_vt)
  {
    if (ya[i] != ya[j])
    {
      DrawRightLine (xa[i]+shift, ya[i], xa[j]+shift, ya[j],
      	ya[j] == bbox.maxy ? 1 : 0);
    }
    i = j;
    j = (j+num_verts+dir)%num_verts;
  }

  //---------
  // Draw all left lines.
  //---------
  dir = -dir_right;
  i = top_vt;
  j = (i+num_verts+dir)%num_verts;

  while (i != bot_vt)
  {
    if (ya[i] != ya[j])
    {
      DrawLeftLine (xa[i]-shift, ya[i], xa[j]-shift, ya[j],
      	ya[j] == bbox.maxy ? 1 : 0);
    }
    i = j;
    j = (j+num_verts+dir)%num_verts;
  }
  return true;
}

void csCoverageBuffer::XORSweep ()
{
  int i, x;
  uint32* buf;
  for (i = 0 ; i < numrows ; i++)
  {
    buf = &buffer[i<<w_shift];
    uint32 first = *buf++;
    for (x = 1 ; x < width ; x++)
    {
      first ^= *buf;
      *buf++ = first;
    }
  }
}

bool csCoverageBuffer::InsertPolygon (csVector2* verts, int num_verts,
	float max_depth, bool negative)
{
  csBox2Int bbox;
  if (!DrawPolygon (verts, num_verts, bbox, 1))
    return false;

  // In this routine we render the polygon to the polygon buffer
  // and then we simulate (but don't actually perform for optimization
  // purposes) a XOR sweep on that polygon buffer.

  int i, x;
  uint32* buf;
  uint32* scr_buf;
  float* depth_buf;
  bool mod = false;
  uint32 init;

  int startrow, endrow;
  int startcol, endcol;
  if (negative)
  {
    init = ~0;
    // If we are working in negative mode we need
    // to do entire box and not only what the bounding box says.
    startrow = 0;
    endrow = numrows-1;
    startcol = 0;
    endcol = width-1;
  }
  else
  {
    init = 0;
    startrow = bbox.miny >> 5;
    if (startrow < 0) startrow = 0;
    endrow = bbox.maxy >> 5;
    if (endrow >= numrows) endrow = numrows-1;
    startcol = bbox.minx-1;
    if (startcol < 0) startcol = 0;
    endcol = bbox.maxx;
    if (endcol >= width) endcol = width-1;
  }

  for (i = startrow ; i <= endrow ; i++)
  {
    int pc = partcols[i];
    if (!pc) continue;

    int buf_idx = (i<<w_shift) + startcol;
    buf = &buffer[buf_idx];
    scr_buf = &scr_buffer[buf_idx];

    uint32 first = init;
    for (x = startcol ; x <= endcol ; x++)
    {
      first ^= *buf++;
      uint32 sb = *scr_buf;
      if ((~sb) & first)
      {
        mod = true;
	sb |= first;
	*scr_buf++ = sb;
	if (!~sb) pc--;
	//@@@ Optimize
	depth_buf = &depth_buffer[(i<<(w_shift-2)) + (x>>2)];
	if (max_depth > *depth_buf)
	  *depth_buf = max_depth;
      }
      else
      {
        scr_buf++;
      }
    }
    partcols[i] = pc;
  }
  return mod;
}

bool csCoverageBuffer::TestPolygon (csVector2* verts, int num_verts,
	float min_depth)
{
  csBox2Int bbox;
  if (!DrawPolygon (verts, num_verts, bbox, 0))
    return false;

  int i, x;
  uint32* buf;
  uint32* scr_buf;
  float* depth_buf;

  int startrow = bbox.miny >> 5;
  if (startrow < 0) startrow = 0;
  int endrow = bbox.maxy >> 5;
  if (endrow >= numrows) endrow = numrows-1;
  int startcol = bbox.minx-1;
  if (startcol < 0) startcol = 0;
  int endcol = bbox.maxx;
  if (endcol >= width) endcol = width-1;
  
  for (i = startrow ; i <= endrow ; i++)
  {
    if (!partcols[i])
      continue;

    int buf_idx = (i<<w_shift) + startcol;
    buf = &buffer[buf_idx];
    scr_buf = &scr_buffer[buf_idx];

    uint32 first = 0;
    for (x = startcol ; x <= endcol ; x++)
    {
      first ^= *buf++;
      if ((~*scr_buf) & first)
      {
        return true;
      }
      else
      {
	//@@@ Optimize
	depth_buf = &depth_buffer[(i<<(w_shift-2)) + (x>>2)];
	if (min_depth <= *depth_buf)
	  return true;
      }
      scr_buf++;
    }
  }
  return false;
}

bool csCoverageBuffer::TestRectangle (const csBox2& rect, float min_depth)
{
  csBox2Int bbox;
  bbox.minx = QInt (rect.MinX ());
  bbox.miny = QInt (rect.MinY ());
  bbox.maxx = QInt (rect.MaxX ());
  bbox.maxy = QInt (rect.MaxY ());

  if (bbox.maxx < 0) return false;
  if (bbox.maxy < 0) return false;
  if (bbox.minx >= width) return false;
  if (bbox.miny >= height) return false;

  int i, x;
  uint32* scr_buf;
  float* depth_buf;

  int startrow = bbox.miny >> 5;
  if (startrow < 0) startrow = 0;
  int endrow = bbox.maxy >> 5;
  if (endrow >= numrows) endrow = numrows-1;
  int startcol = bbox.minx;
  if (startcol < 0) startcol = 0;
  int endcol = bbox.maxx;
  if (endcol >= width) endcol = width-1;
  
  for (i = startrow ; i <= endrow ; i++)
  {
    if (!partcols[i])
      continue;

    int buf_idx = (i<<w_shift) + startcol;
    scr_buf = &scr_buffer[buf_idx];

    uint32 first = ~0;
    if (i == startcol)
    {
      //@@@ MASK
    }
    if (i == endrow)
    {
      //@@@ MASK
    }

    for (x = startcol ; x <= endcol ; x++)
    {
      if ((~*scr_buf) & first)
      {
        return true;
      }
      else
      {
	//@@@ Optimize
	depth_buf = &depth_buffer[(i<<(w_shift-2)) + (x>>2)];
	if (min_depth <= *depth_buf)
	  return true;
      }
      scr_buf++;
    }
  }
  return false;
}

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

void csCoverageBuffer::Debug_Dump (iGraphics3D* g3d, int zoom)
{ 
  iGraphics2D* g2d = g3d->GetDriver2D ();
  iTextureManager *txtmgr = g3d->GetTextureManager ();
  int col = txtmgr->FindRGB (200, 200, 200);
  int x, y, i;
  for (i = 0 ; i < numrows ; i++)
  {
    uint32* row = &scr_buffer[i<<w_shift];
    for (x = 0 ; x < width ; x++)
    {
      uint32 c = *row++;
      int yy = i*32;
      for (y = 0 ; y < 32 ; y++)
      {
        DrawZoomedPixel (g2d, x, yy, (c & 1) ? col : 0, zoom);
	c = c >> 1;
	yy++;
      }
    }
  }
}

static float rnd (int totrange, int leftpad, int rightpad)
{
  return float (((rand () >> 4) % (totrange-leftpad-rightpad)) + leftpad);
}

bool csCoverageBuffer::Debug_ExtensiveTest (int num_iterations,
	csVector2* verts, int& num_verts)
{
  int i;
  for (i = 0 ; i < num_iterations ; i++)
  {
    num_verts = ((rand () >> 4) % 1)+3;
    switch (num_verts)
    {
      case 3:
        verts[0].Set (rnd (width, -100, 20), rnd (height, -100, -100));
        verts[1].Set (rnd (width, -100, 20), rnd (height, -100, -100));
        verts[2].Set (rnd (width, -100, 20), rnd (height, -100, -100));
        break;
      case 4:
        // @@@ TODO!
	break;
    }
    csBox2Int bbox;
    InitializePolygonBuffer ();
    DrawPolygon (verts, num_verts, bbox);
    XORSweep ();

    int r;
    for (r = 0 ; r < numrows ; r++)
    {
      uint32* row = &buffer[(r<<w_shift)+width-2];
      if (*row) return false;
    }
  }
  return true;
}

#define COV_ASSERT(test,msg) \
  if (!(test)) \
  { \
    str.Format ("csCoverageBuffer failure (%d,%s): %s\n", int(__LINE__), \
    	#msg, #test); \
    return false; \
  }

bool csCoverageBuffer::Debug_TestOneIteration (csString& str)
{
  csVector2 poly[6];
  //============================
  // The first part of this test is designed to test the coverage
  // part of the coverage buffer alone. The depth buffer itself is ignored.
  // To do this all polygons are inserted with 0.0 depth value and all
  // polygons are tested with 1.0 depth value.
  //============================
  Initialize ();
  poly[0].Set (5, 5);
  poly[1].Set (635, 5);
  poly[2].Set (635, 475);
  poly[3].Set (5, 475);
  COV_ASSERT (InsertPolygon (poly, 4, 0.0, true) == true, neg);
  poly[0].Set (-100, -100);
  poly[1].Set (7, 7);
  poly[2].Set (-100, -40);
  COV_ASSERT (TestPolygon (poly, 3, 1.0) == true, t1);
  poly[0].Set (1, 1);
  poly[1].Set (3, 1);
  poly[2].Set (2, 3);
  COV_ASSERT (TestPolygon (poly, 3, 1.0) == false, t2);
  poly[0].Set (633, 472);
  poly[1].Set (638, 475);
  poly[2].Set (635, 478);
  COV_ASSERT (TestPolygon (poly, 3, 1.0) == true, t3);
  poly[0].Set (100, 100);
  poly[1].Set (150, 101);
  poly[2].Set (150, 101);
  poly[3].Set (100, 105);
  COV_ASSERT (TestPolygon (poly, 4, 1.0) == true, t4);
  poly[0].Set (100, 0);
  poly[1].Set (150, 1);
  poly[2].Set (150, 1);
  poly[3].Set (100, 4);
  COV_ASSERT (TestPolygon (poly, 4, 1.0) == false, t5);
  poly[0].Set (160, 120);
  poly[1].Set (80, 240);
  poly[2].Set (160, 360);
  poly[3].Set (240, 240);
  COV_ASSERT (InsertPolygon (poly, 4, 0.0) == true, i6);
  poly[0].Set (240, 120);
  poly[1].Set (320, 240);
  poly[2].Set (240, 360);
  poly[3].Set (160, 240);
  COV_ASSERT (InsertPolygon (poly, 4, 0.0) == true, i7);
  poly[0].Set (330, 240);
  poly[1].Set (340, 241);
  poly[2].Set (339, 249);
  poly[3].Set (331, 248);
  COV_ASSERT (TestPolygon (poly, 4, 1.0) == true, t8);
  poly[0].Set (150, 230);
  poly[1].Set (250, 230);
  poly[2].Set (250, 250);
  poly[3].Set (150, 250);
  COV_ASSERT (InsertPolygon (poly, 4, 0.0) == false, i9);
  poly[0].Set (160, 120);
  poly[1].Set (240, 120);
  poly[2].Set (320, 240);
  poly[3].Set (240, 360);
  poly[4].Set (160, 360);
  COV_ASSERT (InsertPolygon (poly, 5, 0.0) == true, i10);
  poly[0].Set (330, 240);
  poly[1].Set (340, 241);
  poly[2].Set (339, 249);
  poly[3].Set (331, 248);
  COV_ASSERT (TestPolygon (poly, 4, 1.0) == true, t11);
  poly[0].Set (400, -200);
  poly[1].Set (410, -200);
  poly[2].Set (410, 1000);
  poly[3].Set (400, 1000);
  COV_ASSERT (InsertPolygon (poly, 4, 0.0) == true, i12);
  poly[0].Set (420, -200);
  poly[1].Set (430, -200);
  poly[2].Set (430, 1000);
  poly[3].Set (420, 1000);
  COV_ASSERT (InsertPolygon (poly, 4, 0.0) == true, i13);
  poly[0].Set (410, -200);
  poly[1].Set (420, -200);
  poly[2].Set (420, 1000);
  poly[3].Set (410, 1000);
  COV_ASSERT (TestPolygon (poly, 4, 1.0) == true, t14);
  poly[0].Set (410, -200);
  poly[1].Set (420, -200);
  poly[2].Set (420, 1000);
  poly[3].Set (410, 1000);
  COV_ASSERT (InsertPolygon (poly, 4, 0.0) == true, i15);
  poly[0].Set (410, -200);
  poly[1].Set (420, -200);
  poly[2].Set (420, 1000);
  poly[3].Set (410, 1000);
  COV_ASSERT (TestPolygon (poly, 4, 1.0) == false, t16);
  COV_ASSERT (IsFull () == false, f17);
  poly[0].Set (-10000, -10000);
  poly[1].Set (10000, -10000);
  poly[2].Set (10000, 10000);
  poly[3].Set (-100000, 10000);
  COV_ASSERT (TestPolygon (poly, 4, 1.0) == true, t17);
  poly[0].Set (0, 0);
  poly[1].Set (640, 0);
  poly[2].Set (640, 120);
  poly[3].Set (0, 120);
  COV_ASSERT (InsertPolygon (poly, 4, 0.0) == true, i18);
  poly[0].Set (0, 120);
  poly[1].Set (640, 120);
  poly[2].Set (640, 240);
  poly[3].Set (0, 240);
  COV_ASSERT (InsertPolygon (poly, 4, 0.0) == true, i19);
  poly[0].Set (0, 240);
  poly[1].Set (640, 240);
  poly[2].Set (640, 360);
  poly[3].Set (0, 360);
  COV_ASSERT (InsertPolygon (poly, 4, 0.0) == true, i20);
  COV_ASSERT (IsFull () == false, f21);
  poly[0].Set (0, 360);
  poly[1].Set (640, 360);
  poly[2].Set (640, 480);
  poly[3].Set (0, 480);
  COV_ASSERT (InsertPolygon (poly, 4, 0.0) == true, i22);
  COV_ASSERT (IsFull () == true, f23);
  poly[0].Set (-10000, -10000);
  poly[1].Set (10000, -10000);
  poly[2].Set (10000, 10000);
  poly[3].Set (-100000, 10000);
  COV_ASSERT (TestPolygon (poly, 4, 1.0) == false, t24);
  poly[0].Set (330, 240);
  poly[1].Set (340, 241);
  poly[2].Set (339, 249);
  poly[3].Set (331, 248);
  COV_ASSERT (TestPolygon (poly, 4, 1.0) == false, t25);

  //============================
  // This part tests the depth buffer part as well.
  //============================
  Initialize ();
  poly[0].Set (160, 120);
  poly[1].Set (320, 120);
  poly[2].Set (320, 240);
  poly[3].Set (160, 240);
  COV_ASSERT (InsertPolygon (poly, 4, 10.0) == true, i26);
  poly[0].Set (240, 180);
  poly[1].Set (380, 180);
  poly[2].Set (380, 300);
  poly[3].Set (240, 300);
  COV_ASSERT (InsertPolygon (poly, 4, 20.0) == true, i27);
  poly[0].Set (170, 170);
  poly[1].Set (180, 170);
  poly[2].Set (180, 180);
  COV_ASSERT (TestPolygon (poly, 3, 9.0) == true, t28);
  COV_ASSERT (TestPolygon (poly, 3, 11.0) == false, t29);
  poly[0].Set (235, 175);
  poly[1].Set (245, 180);
  poly[2].Set (240, 190);
  COV_ASSERT (TestPolygon (poly, 3, 9.0) == true, t30);
  COV_ASSERT (TestPolygon (poly, 3, 15.0) == false, t31);
  COV_ASSERT (TestPolygon (poly, 3, 25.0) == false, t32);
  poly[0].Set (315, 235);
  poly[1].Set (325, 234);
  poly[2].Set (314, 250);
  COV_ASSERT (TestPolygon (poly, 3, 9.0) == true, t33);
  COV_ASSERT (TestPolygon (poly, 3, 15.0) == true, t34);
  COV_ASSERT (TestPolygon (poly, 3, 25.0) == false, t35);

  return true;
}

iString* csCoverageBuffer::Debug_UnitTest ()
{
  Setup (640, 480);

  scfString* rc = new scfString ();
  if (!Debug_TestOneIteration (rc->GetCsString ()))
  {
    return rc;
  }

  csVector2 verts[6];
  int num_verts;
  if (!Debug_ExtensiveTest (10000, verts, num_verts))
  {
    rc->GetCsString ().Append ("csCoverageBuffer failure:\n");
    csString sub;
    int i;
    for (i = 0 ; i < num_verts ; i++)
    {
      sub.Format ("  (%g,%g)\n", verts[i].x, verts[i].y);
      rc->GetCsString ().Append (sub);
    }
    return rc;
  }

  rc->DecRef ();
  return NULL;
}

csTicks csCoverageBuffer::Debug_Benchmark (int num_iterations)
{
  Setup (640, 480);

  csTicks start = csGetTicks ();
  scfString* rc = new scfString ();
  int i;
  for (i = 0 ; i < num_iterations ; i++)
  {
    Debug_TestOneIteration (rc->GetCsString ());
  }
  csTicks end = csGetTicks ();
  rc->DecRef ();
  return end-start;
}

