/*
    Crystal Space Windowing System: color wheel class
    Copyright (C) 1998,1999 by Andrew Zabolotny <bit@eltech.ru>

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
#include "csws/cscwheel.h"
#include "csws/csapp.h"

#define COLORWHEEL_TEXTURE_NAME	"csws::ColorWheel"

static int cwref = 0;
static csPixmap *cwspr = NULL;

csColorWheel::csColorWheel (csComponent *iParent) : csStatic (iParent, csscsBitmap)
{
  h = s = 0;
  trackmouse = false;
  cwref++;
  if (app)
  {
    // If color wheel image is not loaded, load it
    if (!cwspr)
      cwspr = new csPixmap (app->GetTexture (
        COLORWHEEL_TEXTURE_NAME), 0, 0, 129, 129);
  } /* endif */
  Bitmap = cwspr;
  SetSuggestedSize (0, 0);
}

csColorWheel::~csColorWheel ()
{
  if (--cwref == 0)
  {
    delete cwspr;
    cwspr = NULL;
  }
  Bitmap = NULL;
}

bool csColorWheel::HandleEvent (csEvent &Event)
{
  switch (Event.Type)
  {
    case csevMouseDown:
    case csevMouseMove:
      if (trackmouse || (Event.Type == csevMouseDown))
      {
        if (!trackmouse)
        {
          app->CaptureMouse (this);
          trackmouse = true;
        }
        int xc = bound.Width () / 2;
        int yc = bound.Height () / 2;
        float ns = sqrt (fSquare (Event.Mouse.x - xc) + fSquare (Event.Mouse.y - yc)) / xc;
        if (ns > 1) ns = 1;
        float nh = atan2 (yc - Event.Mouse.y, Event.Mouse.x - xc) / (2 * M_PI);
        if (nh < 0) nh += 1;
        if ((ns != s) || (nh != h))
        {
          s = ns; h = nh;
          parent->SendCommand (cscmdColorWheelChanged, this);
        }
        return true;
      }
    case csevMouseUp:
      if (trackmouse)
      {
        app->CaptureMouse (NULL);
        trackmouse = false;
      }
      return true;
  }
  return csStatic::HandleEvent (Event);
}

void csColorWheel::SetHS (float iH, float iS)
{
  h = iH; s = iS;
  Invalidate ();
}

void csColorWheel::Draw ()
{
  csStatic::Draw ();
  int xc = bound.Width () / 2;
  int yc = bound.Height () / 2;
  int x = int (xc * (1 + s * cos (h * 2 * M_PI)));
  int y = int (yc * (1 - s * sin (h * 2 * M_PI)));
  Line (x, 0, x, bound.Height (), CSPAL_STATIC_LIGHT3D);
  Line (0, y, bound.Width (), y, CSPAL_STATIC_LIGHT3D);
}
