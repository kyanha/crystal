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

#ifndef CBUFFER_H
#define CBUFFER_H

#include "csgeom/math2d.h"

/*=================*
 * Coverage Buffer *
 *=================*/

/**
 * A simple span in a CBuffer line.
 * Note that we work with a 'negative' c-buffer. This means
 * that every span indicates an empty region on screen.
 */
struct csCBufferSpan
{
  /// Inclusive range of span.
  int startx, endx;
  /// Next span.
  csCBufferSpan* next;
};

class csCBuffer;

/**
 * A line in the CBuffer.
 * Spans are allocated on an as-needed base. But they are
 * not freed. Instead, unused spans are put in the first_unused
 * list. The size of this list will always be equal to the maximum
 * number of spans used for that line.
 */
class csCBufferLine
{
  friend class csCBuffer;

private:
  // List of all empty spans on this line.
  csCBufferSpan* first_span;
  // Pointer to last span in the list of empty spans.
  csCBufferSpan* last_span;
  // List of all unused spans on this line.
  csCBufferSpan* first_unused;

private:
  /// Initialize a c-buffer line.
  csCBufferLine ();
  /// Destruct a c-buffer line and free all spans (also the unused ones).
  ~csCBufferLine ();

  /**
   * Initialize this line to the given empty span.
   */
  void Initialize (int startx, int endx);

  /// Return true if this line if full (i.e. no empty spans left).
  bool IsFull () { return first_span == NULL; }

  /**
   * Take a full span and test if it would have changed this c-buffer
   * line on insertion. This means that the span is visible.
   */
  bool TestSpan (int startx, int endx);

  /**
   * Take a full span and insert it into the c-buffer line. Return
   * true if the span line was modified (i.e. the span is visible).
   */
  bool InsertSpan (int startx, int endx);
};

/**
 * The CBuffer.
 * Note that all ranges specified in this class are inclusive.
 */
class csCBuffer
{
private:
  // The lines of this c-buffer.
  csCBufferLine* lines;
  // Number of vertical lines.
  int num_lines;
  // A value for every line indicating if it is full or not.
  bool* full;
  // Horizontal start and end (inclusive).
  int startx, endx;
  // Total number of not-full lines.
  int not_full_lines;

public:
  /// Create a new c-buffer with the given dimensions.
  csCBuffer (int sx, int ex, int n_lines);
  /// Destroy the c-buffer.
  ~csCBuffer ();

  /// Initialize the c-buffer to empty.
  void Initialize ();

  /// Return true if the screen (c-buffer) is full.
  bool IsFull () { return not_full_lines <= 0; }

  /**
   * Take a full span and test if it would have changed the c-buffer
   * line on insertion. This means that the span is visible.
   */
  bool TestSpan (int s_spanx, int e_spanx, int y)
  {
    return lines[y].TestSpan (s_spanx, e_spanx);
  }

  /**
   * Take a full span and insert it into the c-buffer line. Return
   * true if the span modified the buffer (i.e. span is visible).
   */
  bool InsertSpan (int s_spanx, int e_spanx, int y);

  /**
   * Take a polygon and test if it would have changed the c-buffer.
   * This means that the polygon is visible. Polygon vertices are
   * converted to integer before comparing.
   */
  bool TestPolygon (csVector2* verts, int num_verts);

  /**
   * Take a polygon and insert all spans in the c-buffer.
   * Returns true if the polygon is visible.
   */
  bool InsertPolygon (csVector2* verts, int num_verts);
};

#endif /*CBUFFER_H*/
