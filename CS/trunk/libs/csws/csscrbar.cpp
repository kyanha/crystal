/*
    Crystal Space Windowing System: scroll bar class
    Copyright (C) 1998 by Jorrit Tyberghein
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

#include "sysdef.h"
#include "csws/csscrbar.h"
#include "csws/csmouse.h"
#include "csws/csapp.h"
#include "csengine/texture.h"

// Minimal scroll button size
#define CSSB_MINIMAL_KNOBSIZE   (3+3+8)
/// Minimal scroll bar size
#define CSSB_MINIMAL_SIZE       (2+2+7)

// The scroll button IDs
#define BUTID_TOPLEFT           1001
#define BUTID_BOTRIGHT          1002
// pseudo-button IDs
#define BUTID_SCROLLTOPLEFT     1003
#define BUTID_SCROLLBOTRIGHT    1004

// Period of time to wait before scroll autorepeat
#define SCROLL_START_INTERVAL   500
// Scroll time interval in milliseconds
#define SCROLL_REPEAT_INTERVAL  100

#define SCROLLBAR_TEXTURE_NAME  "tex/scrollbar.png"

csSprite2D *csScrollBar::sprarrows[12] =
{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

csSprite2D *csScrollBar::sprscroller[2] =
{ NULL, NULL };

static int scrbarref = 0;

csScrollBar::csScrollBar (csComponent *iParent, csScrollBarFrameStyle iFrameStyle)
  : csComponent (iParent)
{
  active_button = status.value = status.maxvalue = status.size = status.maxsize
    = status.step = status.pagestep = 0;

  scrbarref++;
  if (app)
  {
    // Load arrow and scroller images
    if (!sprscroller[0])
    {
      csTextureHandle *scrolltex = app->GetTexture (SCROLLBAR_TEXTURE_NAME);
      for (int i = 0; i < 12; i++)
        CHKB (sprarrows [i] = new csSprite2D (scrolltex, i * 9, 0, 9, 9));
      CHK (sprscroller [0] = new csSprite2D (scrolltex, 12 * 9 + 0, 0, 7, 8));
      CHK (sprscroller [1] = new csSprite2D (scrolltex, 12 * 9 + 7, 0, 8, 7));
    } /* endif */
  } /* endif */

  TrackScroller = false;
  FrameStyle = iFrameStyle;
  SetPalette (CSPAL_SCROLLBAR);
  // create both scroll buttons
  csButtonFrameStyle bfs = (FrameStyle == cssfsThickRect ? csbfsThickRect : csbfsThinRect);
  CHK (topleft  = new csButton (this, cscmdNothing, CSBS_NOMOUSEFOCUS, bfs));
  topleft->id = BUTID_TOPLEFT;
  CHK (botright = new csButton (this, cscmdNothing, CSBS_NOMOUSEFOCUS, bfs));
  botright->id = BUTID_BOTRIGHT;
  CHK (scroller = new csButton (this, cscmdNothing, CSBS_NOMOUSEFOCUS, bfs));
  // create repeat timer
  CHK (timer = new csTimer (this, SCROLL_REPEAT_INTERVAL));
}

csScrollBar::~csScrollBar ()
{
  if (--scrbarref == 0)
  {
    for (int i = 0; i < 12; i++)
    {
      CHK (delete sprarrows [i]);
      sprarrows [i] = NULL;
    } /* endfor */
    CHK (delete sprscroller [0]); sprscroller[0] = NULL;
    CHK (delete sprscroller [1]); sprscroller[1] = NULL;
  } /* endif */
}

void csScrollBar::Draw ()
{
  int dx, dy;
  switch (FrameStyle)
  {
    case cssfsThickRect:
      Rect3D (0, 0, bound.Width (), bound.Height (),
        CSPAL_SCROLLBAR_LIGHT3D, CSPAL_SCROLLBAR_DARK3D);
      Rect3D (1, 1, bound.Width () - 1, bound.Height () - 1,
        CSPAL_SCROLLBAR_LIGHT3D, CSPAL_SCROLLBAR_DARK3D);
      dx = dy = 2;
      break;
    case cssfsThinRect:
      Rect3D (0, 0, bound.Width (), bound.Height (),
        CSPAL_SCROLLBAR_LIGHT3D, CSPAL_SCROLLBAR_DARK3D);
      dx = dy = 1;
      break;
    default:
      return;
  } /* endswitch */

  int c1 = CSPAL_SCROLLBAR_BACKGROUND;
  int c2 = CSPAL_SCROLLBAR_BACKGROUND;

  if (active_button == BUTID_SCROLLTOPLEFT)
    c1 = CSPAL_SCROLLBAR_SELBACKGROUND;
  else if (active_button == BUTID_SCROLLBOTRIGHT)
    c2 = CSPAL_SCROLLBAR_SELBACKGROUND;

  if (IsHorizontal)
  {
    Box (dx, dy, scroller->bound.xmin, bound.Height () - dy, c1);
    Box (scroller->bound.xmax, dy, bound.Width () - 1, bound.Height () - dy, c2);
  } else
  {
    Box (dx, dy, bound.Width () - dx, scroller->bound.ymin, c1);
    Box (dx, scroller->bound.ymax, bound.Width () - dx, bound.Height () - 1, c2);
  } /* endif */

  csComponent::Draw ();
  return;
}

bool csScrollBar::HandleEvent (csEvent &Event)
{
  switch (Event.Type)
  {
    case csevMouseMove:
      // track mouse motion when scroller button is pressed
      if (TrackScroller)
      {
        int x = Event.Mouse.x - scroller->bound.xmin;
        int y = Event.Mouse.y - scroller->bound.ymin;
        if ((x != scrollerdx)
         || (y != scrollerdy))
        {
          int delta = (IsHorizontal ? x - scrollerdx : y - scrollerdy)
             * status.maxvalue / activepixlen;
          SetValue (status.value + delta);
        } /* endif */
        return true;
      } /* endif */
      if (app->MouseOwner == this)
      {
        if (!bound.ContainsRel (Event.Mouse.x, Event.Mouse.y))
        {
          active_button = 0;
          Invalidate ();
        } else if (active_button == 0)
          goto pagescroll;
        return true;
      } /* endif */
      break;
    case csevMouseDown:
    case csevMouseDoubleClick:
      if (csComponent::HandleEvent (Event))
      {
        // Switch mouse owner to us if scroller captured it
        if (app->MouseOwner == scroller)
        {
          app->CaptureMouse (this);
          TrackScroller = true;
        } /* endif */
        return true;
      } /* endif */
      if ((Event.Mouse.Button == 1)
       && bound.ContainsRel (Event.Mouse.x, Event.Mouse.y)
       && (status.maxvalue > 0))
      {
pagescroll:
        int cmp;

        if (scroller->bound.IsEmpty ())
          if (IsHorizontal)
            cmp = bound.Width () / 2;
          else
            cmp = bound.Height () / 2;
        else
          if (IsHorizontal)
            cmp = scroller->bound.xmin;
          else
            cmp = scroller->bound.ymin;

        if ((IsHorizontal ? Event.Mouse.x : Event.Mouse.y) <= cmp)
          active_button = BUTID_SCROLLTOPLEFT;
        else
          active_button = BUTID_SCROLLBOTRIGHT;
        Invalidate ();

        if (app->MouseOwner != this)
          app->CaptureMouse (this);
        // Emulate timeout
        timer->Pause (SCROLL_START_INTERVAL);
        SendCommand (cscmdTimerPulse, timer);
        return true;
      } else
      {
        active_button = 0;
        Invalidate ();
      } /* endif */
      return true;
    case csevMouseUp:
      if (Event.Mouse.Button == 1)
      {
        if (TrackScroller)
        {
          app->CaptureMouse (scroller);
          TrackScroller = false;
          return scroller->HandleEvent (Event);
        } /* endif */
        if (app->MouseOwner == this)
        {
          app->CaptureMouse (NULL);
          active_button = 0;
          Invalidate ();
        } /* endif */
      } /* endif */
      break;
    case csevCommand:
      switch (Event.Command.Code)
      {
        case cscmdButtonUp:
          active_button = 0;
          Invalidate ();
          return true;
        case cscmdButtonDown:
          if (Event.Command.Info == scroller)
          {
            app->GetMouse ()->GetPosition (scrollerdx, scrollerdy);
            scroller->GlobalToLocal (scrollerdx, scrollerdy);
            return true;
          } /* endif */
          active_button = ((csComponent *)Event.Command.Info)->id;
          timer->Pause (SCROLL_START_INTERVAL);
          // fallback to timer pulse
        case cscmdTimerPulse:
          if (active_button == BUTID_TOPLEFT)
            SetValue (status.value - status.step);
          else if (active_button == BUTID_BOTRIGHT)
            SetValue (status.value + status.step);
          else if (active_button == BUTID_SCROLLTOPLEFT)
            SetValue (status.value - status.pagestep);
          else if (active_button == BUTID_SCROLLBOTRIGHT)
            SetValue (status.value + status.pagestep);
          return true;
        case cscmdScrollBarSet:
        {
          status = *((csScrollBarStatus *)Event.Command.Info);
          if (status.size > status.maxsize)
            status.size = status.maxsize;
          int oldvalue = status.value;
          status.value = -1;
          SetValue (oldvalue);
          Event.Command.Info = NULL;
          return true;
        }
        case cscmdScrollBarGetStatus:
          *((csScrollBarStatus *)Event.Command.Info) = status;
          Event.Command.Info = NULL;
          return true;
      } /* endswitch */
      break;
  } /* endswitch */
  return csComponent::HandleEvent (Event);
}

bool csScrollBar::SetRect (int xmin, int ymin, int xmax, int ymax)
{
  if (csComponent::SetRect (xmin, ymin, xmax, ymax))
  {
    int w = bound.Width ();
    int h = bound.Height ();
    if (w > h)
    {
      // horizontal scroll bar
      IsHorizontal = true;

      if ((w / 2 < h) || (h < CSSB_MINIMAL_SIZE))
      {
        // make buttons invisible
        if (topleft)
          topleft->SetRect (0, 0, -1, h);
        if (botright)
          botright->SetRect (w, 0, w, h);
      } else
      {
        if (topleft)
          topleft->SetRect (0, 0, h, h);
        if (botright)
          botright->SetRect (w - h, 0, w, h);
      } /* endif */
    } else
    {
      // vertical scroll bar
      IsHorizontal = false;
      if ((h / 2 < w) || (w < CSSB_MINIMAL_SIZE))
      {
        // make buttons invisible
        if (topleft)
          topleft->SetRect (0, 0, w, -1);
        if (botright)
          botright->SetRect (0, h, w, h);
      } else
      {
        if (topleft)
          topleft->SetRect (0, 0, w, w);
        if (botright)
          botright->SetRect (0, h - w, w, h);
      } /* endif */
    } /* endif */
    SetValue (status.value);
    return true;
  } else
    return false;
}

void csScrollBar::SetValue (int iValue)
{
  if (iValue < 0)
    iValue = 0;
  if (iValue > status.maxvalue)
    iValue = status.maxvalue;
  if (status.value == iValue)
    return;
  status.value = iValue;

  if (status.maxvalue <= 0)
  {
noscrollbut:
    if (IsHorizontal)
      scroller->SetRect (bound.Width () / 2, 0, bound.Width () / 2, -1);
    else
      scroller->SetRect (0, bound.Height () / 2, -1, bound.Height () / 2);
  } else
  {
    int pixmin, pixmax;

    if (IsHorizontal)
    {
      pixmin = topleft->bound.xmax;
      pixmax = botright->bound.xmin;
    } else
    {
      pixmin = topleft->bound.ymax;
      pixmax = botright->bound.ymin;
    } /* endif */

    activepixlen = pixmax - pixmin;
    if ((activepixlen + 2 < CSSB_MINIMAL_KNOBSIZE) || (status.maxsize <= 0))
      goto noscrollbut;

    int pixsize = (status.size * activepixlen) / status.maxsize;
    if (pixsize < CSSB_MINIMAL_KNOBSIZE)
      pixsize = CSSB_MINIMAL_KNOBSIZE;
    if (pixsize >= activepixlen)
      goto noscrollbut;

    pixmin += pixsize / 2;
    pixmax -= ((pixsize + 1) / 2);
    activepixlen = pixmax - pixmin;
    int spix = pixmin + (status.value * activepixlen) / status.maxvalue
      - pixsize / 2;
    int epix = spix + pixsize;
    if (IsHorizontal)
      scroller->SetRect (spix, 0, epix, bound.Height ());
    else
      scroller->SetRect (0, spix, bound.Width (), epix);
  } /* endif */

  // Set up arrows on scroll buttons
  if (IsHorizontal)
  {
    scroller->SetBitmap (sprscroller[1], NULL, false);
    if ((status.maxvalue <= 0) || (status.value <= 0))
    {
      topleft->SetBitmap (sprarrows[8], NULL, false);
      topleft->SetState (CSS_DISABLED, true);
    } else
    {
      topleft->SetBitmap (sprarrows[6], sprarrows[10], false);
      topleft->SetState (CSS_DISABLED, false);
    } /* endif */
    if ((status.maxvalue <= 0) || (status.value >= status.maxvalue))
    {
      botright->SetBitmap (sprarrows[9], NULL, false);
      botright->SetState (CSS_DISABLED, true);
    } else
    {
      botright->SetBitmap (sprarrows[7], sprarrows[11], false);
      botright->SetState (CSS_DISABLED, false);
    } /* endif */
  } else
  {
    scroller->SetBitmap (sprscroller[0], NULL, false);
    if ((status.maxvalue <= 0) || (status.value <= 0))
    {
      topleft->SetBitmap (sprarrows[2], NULL, false);
      topleft->SetState (CSS_DISABLED, true);
    } else
    {
      topleft->SetBitmap (sprarrows[0], sprarrows[4], false);
      topleft->SetState (CSS_DISABLED, false);
    } /* endif */
    if ((status.maxvalue <= 0) || (status.value >= status.maxvalue))
    {
      botright->SetBitmap (sprarrows[3], NULL, false);
      botright->SetState (CSS_DISABLED, true);
    } else
    {
      botright->SetBitmap (sprarrows[1], sprarrows[5], false);
      botright->SetState (CSS_DISABLED, false);
    } /* endif */
  } /* endif */

  if (parent)
    parent->SendCommand (cscmdScrollBarValueChanged, (void *)this);
}
