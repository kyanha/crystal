/*
    Crystal Space Windowing System: Polygon drawing support
    Copyright (C) 2000 by Andrew Zabolotny <bit@eltech.ru>

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

#define CS_SYSDEF_PROVIDE_ALLOCA
#include "cssysdef.h"
#include "csws/cscomp.h"
#include "csws/csapp.h"
#include "csgeom/polyclip.h"
#include "qint.h"

/*
 * Since polygon drawing in windowing system is kind of complicated, it is
 * separated into this file, to clobber less cscomp.cpp. Polygon clipping
 * is implemented as usual (e.g. for lines, boxes etc) - the only difference
 * is that we should adjust per-vertex data (r/g/b/u/v etc) after each
 * clipping. Also since this is the only place in CSWS where we use polygon
 * clipper from libcsgeom if you don't use this function, you won't get
 * the clipper linked into your application.
 */

#define INTERPOLATE1(component) \
  poly.vertices [j].component = orig_poly [vt].component + \
    t * (orig_poly [vt2].component - orig_poly [vt].component);

#define INTERPOLATE(component) \
{ \
  float v1 = orig_poly [edge_from [0]].component + \
    t1 * (orig_poly [edge_to [0]].component - orig_poly [edge_from [0]].component); \
  float v2 = orig_poly [edge_from [1]].component + \
    t2 * (orig_poly [edge_to [1]].component - orig_poly [edge_from [1]].component); \
  poly.vertices [j].component = v1 + t * (v2 - v1); \
}

void csComponent::Polygon3D (G3DPolygonDPFX &poly, UInt mode)
{
 /* Do clipping as follows: create a minimal rectangle which fits the polygon,
  * clip the rectangle against children & parents, then clip the poly against
  * all resulting rectangles.
  */
  cswsRectVector rect (8, 4);
  int i, x = QInt (poly.vertices[0].x), y = QInt (poly.vertices[0].y);
  csRect *lb = new csRect (x, y, x, y);
  for (i = 1; i < poly.num; i++)
    lb->Extend (QInt (poly.vertices[i].x), QInt (poly.vertices[i].y));

  lb->xmax++;
  lb->ymax++;
  if (!clip.IsEmpty ())
    lb->Intersect (clip);
  rect.Push (lb);
  FastClip (rect);

  int dx = 0; int dy = 0;
  LocalToGlobal (dx, dy);

  // Store the original polygon aside (for clipping)
  int orig_num_vert = poly.num;
  ALLOC_STACK_ARRAY (orig_vert, csVector2, orig_num_vert);
  ALLOC_STACK_ARRAY (orig_poly, G3DTexturedVertex, orig_num_vert);
  for (i = 0; i < orig_num_vert; i++)
  {
    orig_vert [i].x = dx + poly.vertices [i].x;
    orig_vert [i].y = dy + poly.vertices [i].y;
    orig_poly [i] = poly.vertices [i];
  }

  for (i = rect.Length () - 1; i >= 0; i--)
  {
    csRect *cur = (csRect *)rect[i];

    // Create the clipper object
    csBoxClipper clipper (cur->xmin, cur->ymin, cur->xmax, cur->ymax);
    // The resulting clipped polygon
    csVector2 clipped_vert [MAX_OUTPUT_VERTICES];
    // Status of every output vertex
    csVertexStatus vstats [MAX_OUTPUT_VERTICES];
    // Number of vertices in clipped polygon
    int clipped_num_vert;
    // Okay, now do the actual clipping
    int rc = clipper.Clip (orig_vert, orig_num_vert, clipped_vert,
      clipped_num_vert, vstats);
    if (rc == CS_CLIP_OUTSIDE) continue;

    // Now process every polygon vertex and create the accompanying
    // values (r/g/b/u/v) according to status of every vertex.
    poly.num = clipped_num_vert;
    int j, vt, vt2;
    float t;
    for (j = 0; j < clipped_num_vert; j++)
    {
      poly.vertices [j].x = clipped_vert [j].x;
      // The Y coordinate is oriented upside
      poly.vertices [j].y = app->ScreenHeight - clipped_vert [j].y;
      if (rc != CS_CLIP_INSIDE)
        switch (vstats [j].Type)
        {
          case CS_VERTEX_ORIGINAL:
            vt = vstats  [j].Vertex;
            poly.vertices [j].z = orig_poly [vt].z;
            poly.vertices [j].u = orig_poly [vt].u;
            poly.vertices [j].v = orig_poly [vt].v;
            if (mode & CS_FX_GOURAUD)
            {
              poly.vertices [j].r = orig_poly [vt].r;
              poly.vertices [j].g = orig_poly [vt].g;
              poly.vertices [j].b = orig_poly [vt].b;
            }
            break;
          case CS_VERTEX_ONEDGE:
            vt = vstats [j].Vertex;
            vt2 = vt + 1; if (vt2 >= orig_num_vert) vt2 = 0;
            t = vstats [j].Pos;
            INTERPOLATE1 (z);
            INTERPOLATE1 (u);
            INTERPOLATE1 (v);
            if (mode & CS_FX_GOURAUD)
            {
              INTERPOLATE1 (r);
              INTERPOLATE1 (g);
              INTERPOLATE1 (b);
            }
            break;
          case CS_VERTEX_INSIDE:
            float x = clipped_vert [j].x;
            float y = clipped_vert [j].y;
            int edge_from [2], edge_to [2];
            int edge = 0;
            int et, ef;
            ef = orig_num_vert - 1;
            for (et = 0; et < orig_num_vert; et++)
            {
              if ((y >= orig_vert [et].y && y <= orig_vert [ef].y) ||
                  (y <= orig_vert [et].y && y >= orig_vert [ef].y))
              {
                edge_from [edge] = et;
                edge_to [edge] = ef;
                edge++;
                if (edge >= 2) break;
              }
              ef = et;
            }
            if (edge == 1)
            {
              // Safety if we only found one edge.
              edge_from [1] = edge_from [0];
              edge_to [1] = edge_to [0];
            }
            csVector2& A = orig_vert [edge_from [0]];
            csVector2& B = orig_vert [edge_to [0]];
            csVector2& C = orig_vert [edge_from [1]];
            csVector2& D = orig_vert [edge_to [1]];
            float t1 = (y - A.y) / (B.y - A.y);
            float t2 = (y - C.y) / (D.y - C.y);
            float x1 = A.x + t1 * (B.x - A.x);
            float x2 = C.x + t2 * (D.x - C.x);
            t = (x - x1) / (x2 - x1);
            INTERPOLATE (z);
            INTERPOLATE (u);
            INTERPOLATE (v);
            if (mode & CS_FX_GOURAUD)
            {
              INTERPOLATE (r);
              INTERPOLATE (g);
              INTERPOLATE (b);
            }
            break;
        }
    }

    app->pplPolygon3D (poly, mode);
  }
}

void csComponent::ClearZbuffer (int x1, int y1, int x2, int y2)
{
  cswsRectVector rect (8, 4);
  csRect *lb = new csRect (x1, y1, x2, y2);
  if (!clip.IsEmpty ())
    lb->Intersect (clip);
  rect.Push (lb);
  FastClip (rect);

  int i;
  for (i = rect.Length () - 1; i >= 0; i--)
  {
    csRect *cur = (csRect *)rect[i];
    app->pplClearZbuffer (cur->xmin, cur->ymin, cur->xmax, cur->ymax);
  }
}
