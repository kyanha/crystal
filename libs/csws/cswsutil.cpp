/*
    Crystal Space Windowing System: Miscelaneous CSWS utilites
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
#include "csws/cswsutil.h"
#include "csws/csapp.h"
#include "csws/csdialog.h"
#include "csws/cslistbx.h"
#include "csws/cswsaux.h"
#include "csutil/scanstr.h"
#include "csutil/csstring.h"
#include "isys/system.h"
#include "ivaria/reporter.h"

//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--/ Window list --//--

enum
{
  cscmdWindowListShow = 0x7ffffff0,
  cscmdWindowListMaximize,
  cscmdWindowListClose
};

csWindowList::csWindowList (csComponent *iParent) : csWindow (iParent,
  "Window List", CSWS_BUTCLOSE | CSWS_BUTMAXIMIZE | CSWS_TITLEBAR, cswfs3D)
{
  shouldclose = false;
  dialog = new csDialog (this);
  list = new csListBox (dialog, CSLBS_MULTIPLESEL | CSLBS_VSCROLL, cslfsThinRect);
  butshow = new csButton (dialog, cscmdWindowListShow, CSBS_SHIFT
    | CSBS_SELECTABLE | CSBS_NOMOUSEFOCUS | CSBS_NOKEYBOARDFOCUS, csbfsThinRect);
  butshow->SetText ("~Show");
  butmaximize = new csButton (dialog, cscmdWindowListMaximize, CSBS_SHIFT
    | CSBS_SELECTABLE | CSBS_NOMOUSEFOCUS | CSBS_NOKEYBOARDFOCUS, csbfsThinRect);
  butmaximize->SetText ("~Maximize");
  butclose = new csButton (dialog, cscmdWindowListClose, CSBS_SHIFT
    | CSBS_SELECTABLE | CSBS_NOMOUSEFOCUS | CSBS_NOKEYBOARDFOCUS, csbfsThinRect);
  butclose->SetText ("~Close");
  focusedwindow = app->focused;
}

bool csWindowList::SetRect (int xmin, int ymin, int xmax, int ymax)
{
  if (csWindow::SetRect (xmin, ymin, xmax, ymax))
  {
    app->SetWindowListSize (bound.Width (), bound.Height ());
    csRect rect (dialog->bound);
    rect.ymax -= TitlebarHeight;
    rect.Move (-dialog->bound.xmin, -dialog->bound.ymin);
    ((csComponent *)list)->SetRect (rect);
    rect.ymin = rect.ymax + 1;
    rect.ymax += TitlebarHeight;
    int x1 = rect.xmin + rect.Width () / 3;
    int x2 = rect.xmin + (rect.Width () * 2) / 3;
    butshow->SetRect (rect.xmin, rect.ymin, x1, rect.ymax);
    butmaximize->SetRect (x1 + 1, rect.ymin, x2, rect.ymax);
    butclose->SetRect (x2 + 1, rect.ymin, rect.xmax, rect.ymax);
    return true;
  } /* endif */
  return false;
}

void csWindowList::SetState (int mask, bool enable)
{
  csComponent::SetState (mask, enable);
  if ((mask & CSS_FOCUSED) && !enable)
    shouldclose = true;
}

static bool do_sendcommand (csComponent *child, void *param)
{
  ((csComponent *)(child->id))->SendCommand ((int)param, NULL);
  return false;
}

static bool do_select (csComponent *child, void *param)
{
  (void)param;
  ((csComponent *)(child->id))->Select ();
  return true;
}

bool csWindowList::HandleEvent (iEvent &Event)
{
  switch (Event.Type)
  {
    case csevBroadcast:
      if (Event.Command.Code == cscmdPostProcess)
      {
        if (shouldclose)
	{
	  Close ();
	  return true;
	}
        if (app->WindowListChanged)
        {
          app->WindowListChanged = false;
          RebuildList ();
        } /* endif */
      } /* endif */
      break;
    case csevKeyDown:
      if (Event.Key.Code == CSKEY_ESC)
      {
        Close ();
        return true;
      } /* endif */
      break;
    case csevCommand:
      switch (Event.Command.Code)
      {
        case cscmdWindowListShow:
          list->ForEachItem (do_sendcommand, (void *)cscmdHide);
          return true;
        case cscmdWindowListMaximize:
          list->ForEachItem (do_sendcommand, (void *)cscmdMaximize);
          return true;
        case cscmdWindowListClose:
          list->ForEachItem (do_sendcommand, (void *)cscmdClose);
          return true;
        case cscmdListBoxItemDoubleClicked:
          list->ForEachItem (do_select);
          return true;
      } /* endswitch */
      break;
  } /* endswitch */

  return csWindow::HandleEvent (Event);
}

bool csWindowList::do_addtowindowlist (csComponent *child, void *param)
{
  csWindowList *windowlist = (csWindowList *)param;
  if (windowlist != child)
  {
    const char *title = child->GetText ();
    if (title && title[0])
      (void)new csListBoxItem (windowlist->list, title, (int)child,
        child == windowlist->focusedwindow ? cslisEmphasized : cslisNormal);
  } /* endif */
  return false;
}

void csWindowList::RebuildList ()
{
  list->SendCommand (cscmdListBoxClear, NULL);
  app->ForEach (do_addtowindowlist, (void *)this, true);
}

//--//--//--//--//--//--//--//--//--//--//--//--//--//- Utility functions --//--

csButton *csNewToolbarButton (csComponent *iToolbar, int iCommand, char *iText,
  csButtonFrameStyle iFrameStyle, int iButtonStyle)
{
  csButton *but = new csButton (iToolbar, iCommand, iButtonStyle,
    iFrameStyle);
  but->SetText (iText);
  int w, h;
  but->SuggestSize (w, h);
  but->SetSize (w, h);
  return but;
}

csButton *csNewToolbarButton (csComponent *iToolbar, int iCommand,
  csPixmap *bmpup, csPixmap *bmpdn, csButtonFrameStyle iFrameStyle,
  int iButtonStyle, bool iDeletePixmaps)
{
  csButton *but = new csButton (iToolbar, iCommand, iButtonStyle,
    iFrameStyle);
  but->SetBitmap (bmpup, bmpdn, iDeletePixmaps);
  int w, h;
  but->SuggestSize (w, h);
  but->SetSize (w, h);
  return but;
}

csPixmap *NewBitmap (csApp *app, char *texturename, int tx, int ty,
  int tw, int th)
{
  iTextureHandle *tex = app->GetTexture (texturename);
  csPixmap *spr;
  if (tex)
    spr = new csSimplePixmap (tex, tx, ty, tw, th);
  else
    spr = NULL;

  return spr;
}

struct RectUnionRec
{
  cswsRectVector *rect;
  csRect *result;
};

static bool doRectUnion (int *vector, int count, void *arg)
{
  RectUnionRec *ru = (RectUnionRec *)arg;
  csRect tmp (*(csRect *)(*ru->rect) [vector [0]]);
  int i;
  for (i = 0; i < count; i++)
    tmp.AddAdjanced (*(csRect *)(*ru->rect) [vector [i]]);
  for (i = count; i < ru->rect->Length (); i++)
    tmp.AddAdjanced (*(csRect *)(*ru->rect) [i]);
  if (tmp.Area () > ru->result->Area ())
    ru->result->Set (tmp);
  return false;
}

void RectUnion (cswsRectVector &rect, csRect &result)
{
  // Sort rectangles by area so that we can compute much less variants
  for (int i = 0; i < rect.Length (); i++)
    for (int j = rect.Length () - 1; j > i; j--)
      if (((csRect *)rect [i])->Area () < ((csRect *)rect [j])->Area ())
      {
        csRect *tmp = (csRect *)rect [i];
        rect [i] = rect [j];
        rect [j] = tmp;
      } /* endif */

  RectUnionRec ru;
  ru.rect = &rect;
  ru.result = &result;
  result.MakeEmpty ();
  int n = rect.Length ();
  // Save user from endless wait
  if (n > 8)
    n = 8;
  if (rect.Length ())
    csCombinations (n, n, doRectUnion, &ru);
}

void ParseConfigBitmap (csApp *app, const char *prefix, const char *section,
  const char *id, int &x, int &y, int &w, int &h)
{
  const char *butdef = NULL;

  if (prefix) {
    csString Keyname;
    Keyname << "CSWS." << prefix << '.' << section << '.' << id;
    butdef = app->config->GetStr (Keyname, NULL);
  }

  if (!butdef) {
    csString Keyname;
    Keyname << "CSWS." << section << '.' << id;
    butdef = app->config->GetStr (Keyname, NULL);
  }

  if (!butdef)
  {
    app->Printf (CS_REPORTER_SEVERITY_ERROR, "Cannot find bitmap definition %s.%s\n",
      section, id);
    exit (-1);	// @@@ Please replace with return value!!!
  }

  if (csScanStr (butdef, "%d,%d,%d,%d", &x, &y, &w, &h) != 4)
  {
    app->Printf (CS_REPORTER_SEVERITY_ERROR, "%s.%s): parse error in string: %s\n",
      section, id, butdef);
    exit (-1);	// @@@ Please replace with return value!!!
  }
}

// Multiply with this constant to convert from range -1/3..+1/3 to -PI...+PI
#define CONST_F2A	(M_PI * 3.0)

// HLS -> RGB
void csHLS2RGB (float h, float l, float s, float &r, float &g, float &b)
{
  float hr = (h > 2.0 / 3.0) ? h - 1.0 : h;

  r = (hr < 1.0 / 3.0) ?
    1 + s * cos ((hr           ) * CONST_F2A) :
    1 - s;
  g = (h < 2.0 / 3.0) ?
    1 + s * cos ((h - 1.0 / 3.0) * CONST_F2A) :
    1 - s;
  b = (h > 1.0 / 3.0) ?
    1 + s * cos ((h - 2.0 / 3.0) * CONST_F2A) :
    1 - s;

  r *= l; if (r > 1.0) r = 1.0;
  g *= l; if (g > 1.0) g = 1.0;
  b *= l; if (b > 1.0) b = 1.0;
}

// RGB -> HLS
// Phew! It took me two days until I figured how to do
// the reverse transformation :-) -- A.Z.
void csRGB2HLS (float r, float g, float b, float &h, float &l, float &s)
{
  float max, mid, min, sign, delta;
  if (r > g)
    if (r > b)
    {
      delta = 0;
      max = r;
      if (g > b)
        mid = g, min = b, sign = 1;
      else
        mid = b, min = g, sign = -1;
    }
    else
    {
      delta = 2.0 / 3.0;
      max = b;
      mid = r;
      min = g;
      sign = 1;
    }
  else
    if (g > b)
    {
      delta = 1.0 / 3.0;
      max = g;
      if (r > b)
        mid = r, min = b, sign = -1;
      else
        mid = b, min = r, sign = 1;
    }
    else
    {
      delta = 2.0 / 3.0;
      max = b;
      mid = g;
      min = r;
      sign = -1;
    }
  l = max;

  if (l == 0)
    s = 0;
  else
    s = 1 - min / l;

  if ((r == g) && (g == b))
    h = 0;
  else
    h = delta + sign * (acos ((max - mid) / (max - min))) / CONST_F2A;
  if (h < 0) 
    h += 1;
}

#undef CONST_F2A

void csGetRGB (int iColor, csApp *iApp, float &r, float &g, float &b)
{
  iColor = iApp->pplColor (iColor);
  csPixelFormat *pfmt = iApp->GetG2D ()->GetPixelFormat ();
  csRGBpixel *palette = iApp->GetG2D ()->GetPalette ();
  if (pfmt->PalEntries)
  {
    r = palette [iColor].red   / 255.;
    g = palette [iColor].green / 255.;
    b = palette [iColor].blue  / 255.;
  }
  else
  {
    int _r = ((iColor & pfmt->RedMask  ) >> pfmt->RedShift  ) << (8 - pfmt->RedBits  );
    int _g = ((iColor & pfmt->GreenMask) >> pfmt->GreenShift) << (8 - pfmt->GreenBits);
    int _b = ((iColor & pfmt->BlueMask ) >> pfmt->BlueShift ) << (8 - pfmt->BlueBits );
    r = _r ? float (_r + (255 >> pfmt->RedBits  )) / 255. : 0.;
    g = _g ? float (_g + (255 >> pfmt->GreenBits)) / 255. : 0.;
    b = _b ? float (_b + (255 >> pfmt->BlueBits )) / 255. : 0.;
  }
}
