/*
    Copyright (C) 1998-2001 by Jorrit Tyberghein
    Written by Andrew Zabolotny <bit@eltech.ru>

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
#include "csgeom/math.h"
#include "csutil/event.h"
#include "csutil/eventnames.h"
#include <stdarg.h>
#include <stdlib.h>
#include "csplugincommon/canvas/fontcache.h"
#include "csplugincommon/canvas/graph2d.h"
#include "csqint.h"
#include "iutil/cmdline.h"
#include "iutil/plugin.h"
#include "ivideo/graph3d.h"
#include "ivideo/texture.h"
#include "ivideo/txtmgr.h"
#include "iengine/texture.h"
#include "iutil/event.h"
#include "iutil/eventq.h"
#include "iutil/objreg.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "ivaria/reporter.h"

csGraphics2D::csGraphics2D (iBase* parent) : 
  scfImplementationType (this, parent), fontCache (0)
{
  is_open = false;
  object_reg = 0;
  weakEventHandler = 0;

  fontCache = 0;
}

csGraphics2D::~csGraphics2D ()
{
  if (weakEventHandler != 0)
  {
    csRef<iEventQueue> q (csQueryRegistry<iEventQueue> (object_reg));
    if (q != 0)
      CS::RemoveWeakListener (q, weakEventHandler);
  }
  Close ();
}

bool csGraphics2D::Initialize (iObjectRegistry* r)
{
  CS_ASSERT (r != 0);
  object_reg = r;
  plugin_mgr = csQueryRegistry<iPluginManager> (object_reg);
  // Get the system parameters
  config.AddConfig (object_reg, "/config/video.cfg");
  ReadConfig (object_reg, config);
  vpWidth = fbWidth;
  vpHeight = fbHeight;

  // Get the font server: A missing font server is NOT an error
  if (!FontServer)
  {
    FontServer = csQueryRegistry<iFontServer> (object_reg);
  }
#ifdef CS_DEBUG
  if (!FontServer)
  {
    csReport (r, CS_REPORTER_SEVERITY_WARNING,
      "crystalspace.graphics2d.common",
      "Canvas driver couldn't find a font server plugin!  "
      "This is normal if you don't want one (warning displays only in "
      "debug mode)");
  }
#endif

  csRef<iEventQueue> q (csQueryRegistry<iEventQueue> (object_reg));
  if (q != 0)
  {
    csEventID events[3] = { csevSystemOpen (object_reg), 
			    csevSystemClose (object_reg), 
			    CS_EVENTLIST_END };
    CS::RegisterWeakListener (q, this, events, weakEventHandler);
  }
  return true;
}

bool csGraphics2D::HandleEvent (iEvent& Event)
{
  if (Event.Name == csevSystemOpen (object_reg))
  {
    Open ();
    return true;
  }
  else if (Event.Name == csevSystemClose (object_reg))
  {
    Close ();
    return true;
  }
  else
  {
    return false;
  }
}

bool csGraphics2D::Open ()
{
  if (is_open) return true;
  is_open = true;

  if (!CanvasOpen()) return false;

  vpLeft = 0;
  vpTop = 0;
  
  FrameBufferLocked = 0;

  SetClipRect (0, 0, fbWidth, fbHeight);

  return true;
}

void csGraphics2D::Close ()
{
  if (!is_open) return;
  is_open = false;
  CanvasClose();
  delete fontCache;
  fontCache = 0;
}

bool csGraphics2D::BeginDraw ()
{
  FrameBufferLocked++;
  return true;
}

void csGraphics2D::FinishDraw ()
{
  if (FrameBufferLocked)
    FrameBufferLocked--;
}

void csGraphics2D::Clear(int color)
{
  DrawBox (0, 0, vpWidth, vpHeight, color);
}

void csGraphics2D::ClearAll (int color)
{
  if (!BeginDraw ())
    return;
  Clear (color);
  FinishDraw ();
  Print ();
}

void csGraphics2D::SetClipRect (int xmin, int ymin, int xmax, int ymax)
{
  if (xmin < 0) xmin = 0;
  else if (xmin > vpWidth) xmin = vpWidth;
  if (xmax < 0) xmax = 0;
  else if (xmax > vpWidth) xmax = vpWidth;
  if (ymin < 0) ymin = 0;
  else if (ymin > vpHeight) ymin = vpHeight;
  if (ymax < 0) ymax = 0;
  else if (ymax > vpHeight) ymax = vpHeight;
  ClipX1 = xmin; ClipX2 = xmax;
  ClipY1 = ymin; ClipY2 = ymax;
  
  if (fontCache)
    fontCache->SetClipRect (ClipX1, ClipY1, ClipX2, ClipY2);
}

void csGraphics2D::GetClipRect (int &xmin, int &ymin, int &xmax, int &ymax)
{
  xmin = ClipX1; xmax = ClipX2;
  ymin = ClipY1; ymax = ClipY2;
}

/* helper function for ClipLine below */
bool csGraphics2D::CLIPt(float denom, float num, float& tE, float& tL)
{
    float t;

    if(denom > 0)
    {
        t = num / denom;
        if(t > tL) return false;
        else if(t > tE) tE = t;
    }
    else if(denom < 0)
    {
        t = num / denom;
        if(t < tE) return false;
        else if(t < tL) tL = t; // note: there is a mistake on this line in the C edition of the book!
    }
    else 
      if(num > 0) return false;
    return true;
}

/* This function and the next one were taken
   from _Computer Graphics: Principals and Practice_ (2nd ed)
   by Foley et al
   This implements the Liang-Barsky efficient parametric
   line-clipping algorithm
*/
bool csGraphics2D::ClipLine (float &x0, float &y0, float &x1, float &y1,
                             int xmin, int ymin, int xmax, int ymax)
{
    // exclude the left/bottom edges (the Liang-Barsky algorithm will
    // clip to those edges exactly, whereas the documentation for
    // ClipLine specifies that the lower/bottom edges are excluded)
    xmax--;
    ymax--;

    float dx = x1 - x0;
    float dy = y1 - y0;
    bool visible = false;

    if(dx == 0 && dy == 0 && x0 >= xmin && y0 >= ymin && x0 < xmax && y0 < ymax) 
    {
        visible = true;
    }
    else
    {
        float tE = 0.0;
        float tL = 1.0;
        if(CLIPt(dx, xmin - x0, tE, tL))
            if(CLIPt(-dx, x0 - xmax, tE, tL))
                if(CLIPt(dy, ymin - y0, tE, tL))
                    if(CLIPt(-dy, y0 - ymax, tE, tL))
                    {
                        visible = true;
                        if(tL < 1.0)
			{
                            x1 = x0 + tL * dx;
                            y1 = y0 + tL * dy;
                        }
                        if(tE > 0)
			{
                            x0 += tE * dx;
                            y0 += tE * dy;
                        }
                    }
    }
    return !visible;
}

void csGraphics2D::Write (iFont *font, int x, int y, int fg, int bg, 
			  const char *text, uint flags) 
{ 
  if (!text || !*text) return;
  fontCache->WriteString (font, x, y, fg, bg, text, false, flags);
}

void csGraphics2D::Write (iFont *font, int x, int y, int fg, int bg, 
			  const wchar_t*text, uint flags) 
{ 
  if (!text || !*text) return;
  fontCache->WriteString (font, x, y, fg, bg, text, true, flags);
}

bool csGraphics2D::PerformExtensionV (char const* command, va_list args)
{
  return false;
}

bool csGraphics2D::Resize (int w, int h)
{
  int old_width (fbWidth), old_height (fbHeight);
  if (!CanvasResize (w, h))
    return false;

  if (!is_open)
  {
    // Still in Initialization phase, configuring size of canvas
    vpWidth = fbWidth;
    vpHeight = fbHeight;
    return true;
  }

  if (old_width != w || old_height != h)
  {
    if ((vpLeft == 0) && (vpTop == 0)
        && (vpWidth == old_width) && (vpHeight == old_height))
    {
      vpWidth = w;
      vpHeight = h;
    }
  }
  return true;
}

void csGraphics2D::SetViewport (int left, int top, int width, int height)
{ 
  vpLeft = left; vpTop = top; vpWidth = width; vpHeight = height;
  fontCache->SetViewportOfs (left, top);
}

bool csGraphics2D::DebugCommand (const char* /*cmd*/)
{
  return false;
}
