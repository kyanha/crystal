/*
    Crystal Space Windowing System: notebook class
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

#define SYSDEF_ALLOCA
#define SYSDEF_CASE
#include "cssysdef.h"
#include "csws/csapp.h"
#include "csws/csnotebk.h"
#include "csws/cswsutil.h"

//------------------------------------------------------------------------------

// Private button class for tab scroller buttons
class cspNotebookButton : public csButton
{
public:
  // Button border type (one of CSNBS_TABPOS_XXX)
  int border;

  // constructor
  cspNotebookButton (csComponent *iParent, int iCommand,
    csPixmap *iNormal, csPixmap *iPressed, csButtonFrameStyle iFrameStyle,
    int iButtonStyle);
  // Draw the button
  virtual void Draw ();
};

cspNotebookButton::cspNotebookButton (csComponent *iParent, int iCommand,
  csPixmap *iNormal, csPixmap *iPressed, csButtonFrameStyle iFrameStyle,
  int iButtonStyle) : csButton (iParent, iCommand, iButtonStyle, iFrameStyle)
{
  SetBitmap (iNormal, iPressed, false);
}

void cspNotebookButton::Draw ()
{
  csButton::Draw ();
  int lc, dc;
  if (Pressed)
    lc = CSPAL_BUTTON_DARK3D, dc = CSPAL_BUTTON_LIGHT3D;
  else
    lc = CSPAL_BUTTON_LIGHT3D, dc = CSPAL_BUTTON_DARK3D;

  if (border == CSNBS_TABPOS_BOTTOM)
    Line (1, 0, bound.Width () - 1, 0, CSPAL_BUTTON_BACKGROUND);
  else
    Line (0, 0, bound.Width (), 0, lc);

  if (border == CSNBS_TABPOS_RIGHT)
    Line (0, 1, 0, bound.Height () - 1, CSPAL_BUTTON_BACKGROUND);
  else
    Line (0, 0, 0, bound.Height (), lc);

  if (border == CSNBS_TABPOS_TOP)
    Line (1, bound.Height () - 1, bound.Width () - 1,
      bound.Height () - 1, CSPAL_BUTTON_BACKGROUND);
  else
    Line (int (border != CSNBS_TABPOS_RIGHT), bound.Height () - 1,
      bound.Width () - 1, bound.Height () - 1, dc);

  if (border == CSNBS_TABPOS_LEFT)
    Line (bound.Width () - 1, 1, bound.Width () - 1,
      bound.Height () - 2, CSPAL_BUTTON_BACKGROUND);
  else
    Line (bound.Width () - 1, int (border != CSNBS_TABPOS_BOTTOM),
      bound.Width () - 1, bound.Height () - 1, dc);
}

//------------------------------------------------------------------------------

// This is the common numeric ID of all components that are notebook pages
#define ID_NOTEBOOK_PAGE	0x0000c600
// Flag: this page is a primary page
#define NOTEBOOK_PAGE_PRIMARY	0x20
// Flag: the page has a pixmap attached rather than a text title
#define NOTEBOOK_PAGE_PIXMAP	0x40
// Flag: destroy pixmap on termination
#define NOTEBOOK_PAGE_DELPIXMAP	0x80

// Texture name with miscelaneous gadgets for notebook
#define NOTEBOOK_TEXTURE_NAME	"csws::NoteBook"

// Skip a bit from the left/top margin when painting the firstmost tab
#define TABPOS_SKIP		4

#define IS_PRIMARY_PAGE(page)	(pages.Get (page)->flags & NOTEBOOK_PAGE_PRIMARY)

//------------------------

csNotebook::cspPageData::cspPageData (csComponent *ipage, unsigned char iflags)
{
  page = ipage;
  flags = iflags;
  zorder = -1;
  image = NULL;
  text = NULL;
}

csNotebook::cspPageData::~cspPageData ()
{
  if (flags & NOTEBOOK_PAGE_PIXMAP
   && flags & NOTEBOOK_PAGE_DELPIXMAP)
    delete image;
  delete [] text;
  delete page;
}

bool csNotebook::cspPageData::IsHotKey (int iKey)
{
  if (!text || underline_pos < 0 || iKey > 255)
    return false;

  return UPPERCASE (iKey) == UPPERCASE (text [underline_pos]);
}

//------------------------

// Count all references to notebook sprites
static int notebkref = 0;

// The sprites
csPixmap *csNotebook::sprites [12] =
{ NULL };

csNotebook::csNotebook (csComponent *iParent, int iStyle) : csComponent (iParent)
{
  SetPalette (CSPAL_NOTEBOOK);
  state |= CSS_SELECTABLE;
  style = iStyle;
  fReposition = true;
  firsttab = activetab = 0;

  notebkref++;
  if (app && !sprites [0])
  {
    // Load images
    iTextureHandle *tex = app->GetTexture (NOTEBOOK_TEXTURE_NAME);
    for (int i = 0; i < 12; i++)
      sprites [i] = new csPixmap (tex, i * 9, 0, 9, 9);
  } /* endif */

  // Create the four buttons: two for subpage switching and two for tab scrolling
  taback = new cspNotebookButton (this, cscmdNotebookScrollTabsBackward,
    sprites [0], sprites [2], csbfsNone,
    CSBS_NOMOUSEFOCUS | CSBS_NODEFAULTBORDER);
  tabfor = new cspNotebookButton (this, cscmdNotebookScrollTabsForward,
    sprites [1], sprites [3], csbfsNone,
    CSBS_NOMOUSEFOCUS | CSBS_NODEFAULTBORDER);
  nextpage = csNewToolbarButton (this, cscmdNotebookNextPage,
    sprites [8], sprites [10], csbfsNone,
    CSBS_NOMOUSEFOCUS | CSBS_NODEFAULTBORDER, false);
  prevpage = csNewToolbarButton (this, cscmdNotebookPrevPage,
    sprites [9], sprites [11], csbfsNone,
    CSBS_NOMOUSEFOCUS | CSBS_NODEFAULTBORDER, false);

  nextpage->SetState (CSS_TRANSPARENT, true);
  prevpage->SetState (CSS_TRANSPARENT, true);
}

csNotebook::~csNotebook ()
{
  int i;

  if (!--notebkref && sprites [0])
    for (i = 0; i < 12; i++)
    {
      delete sprites [i];
      sprites [i] = NULL;
    }
}

void csNotebook::Draw ()
{
  csComponent::Draw ();

  csRect rect;
  GetClientRect (rect);

  if (focused && focused->id == ID_NOTEBOOK_PAGE)
    focused->SetRect (rect);

  /*   __________________ tabw _________________
   *  /                                         \
   *      +----------------+   +--------------    -\  <- taby
   *      |+---------------+\  |+-------------     |
   *      ||                \\ ||                  |
   *      ||                 \\||                   > tabh
   *      ||                  \\                   |
   *      ||                   \\                  |
   *  =====+                    +============     -/
   *                                              -\
   *    info line                                  |
   *                                                > infoh
   *  =======================================      |
   *  +-------------------------------------+     -/
   *  |                                     |
   *  |               rect                  |
   *  |                                     |
   *  +-------------------------------------+
   */

  // Compute the position for tabs
  int infoh = InfoHeight ();
  int taby, tabh;
  if ((style & CSNBS_TABPOS_MASK) == CSNBS_TABPOS_BOTTOM)
  {
    taby = rect.ymax + infoh;
    tabh = bound.Height () - taby;
  }
  else
  {
    taby = 0;
    if ((style & CSNBS_TABPOS_MASK) == CSNBS_TABPOS_TOP)
      tabh = rect.ymin - infoh;
    else
      tabh = bound.Height ();
  }

  int tabx, tabw;
  if ((style & CSNBS_TABPOS_MASK) < CSNBS_TABPOS_LEFT)
  {
    tabx = 0;
    tabw = bound.Width ();
  }
  else if ((style & CSNBS_TABPOS_MASK) == CSNBS_TABPOS_RIGHT)
  {
    tabx = rect.xmax;
    tabw = bound.Width () - rect.xmax;
  }
  else
  {
    tabx = 0;
    tabw = rect.xmin;
  }

  // Compute the position of all buttons
  int bx = 0, by = 0, fx = 0, fy =0, ww, hh;

  ww = sprites [0]->Width () + 6;
  if ((style & CSNBS_TABPOS_MASK) < CSNBS_TABPOS_LEFT)
    hh = tabh;
  else
    hh = sprites [0]->Height () + 6;

  switch (style & CSNBS_TABPOS_MASK)
  {
    case CSNBS_TABPOS_TOP:
      bx = 0; fx = bound.Width () - ww;
      by = fy = 0;
      break;
    case CSNBS_TABPOS_BOTTOM:
      bx = 0; fx = bound.Width () - ww;
      by = fy = bound.Height () - hh;
      break;
    case CSNBS_TABPOS_LEFT:
      bx = fx = rect.xmin - ww;
      by = 0; fy = bound.Height () - hh;
      break;
    case CSNBS_TABPOS_RIGHT:
      bx = fx = rect.xmax;
      by = 0; fy = bound.Height () - hh;
      break;
  }

  if (fReposition)
  {
    fReposition = false;

    taback->border = tabfor->border = style & CSNBS_TABPOS_MASK;
    taback->SetRect (bx, by, bx + ww, by + hh);
    tabfor->SetRect (fx, fy, fx + ww, fy + hh);

    int px = (style & CSNBS_TABPOS_MASK) < CSNBS_TABPOS_LEFT ? 4 : 0;
    taback->SetBitmap (sprites [px + 0], sprites [px + 2], false);
    tabfor->SetBitmap (sprites [px + 1], sprites [px + 3], false);

    if (!(style & CSNBS_PAGEINFO))
      infoh = nextpage->bound.Height () + 4;
    int xx = rect.xmax - nextpage->bound.Width () - 2;
    int yy = taby + (infoh - nextpage->bound.Height ()) / 2;
    if ((style & CSNBS_TABPOS_MASK) == CSNBS_TABPOS_TOP)
      yy += tabh;
    else if ((style & CSNBS_TABPOS_MASK) == CSNBS_TABPOS_BOTTOM)
      yy -= infoh - 1;
    nextpage->SetPos (xx, yy);
    xx -= prevpage->bound.Width () + 2;
    prevpage->SetPos (xx, yy);
  }

  // If current page is a secondary one, find the corresponding primary
  int primary = activetab;
  while (primary && !IS_PRIMARY_PAGE (primary))
    primary--;

  // Count forward starting from first tab, which is the last visible tab
  int lasttab, curpos = TABPOS_SKIP;
  int pos = ((style & CSNBS_TABPOS_MASK) < CSNBS_TABPOS_LEFT) ?
    bound.Width () : bound.Height ();
  for (lasttab = firsttab; lasttab < pages.Length (); lasttab++)
    if (IS_PRIMARY_PAGE (lasttab))
    {
      int w, h;
      GetTabSize (lasttab, w, h);
      curpos += ((style & CSNBS_TABPOS_MASK) < CSNBS_TABPOS_LEFT) ?
        w - tabh / 2 : h + 3;
      if (curpos >= pos)
        break;
    }

  // Hide the tab scrolling knobs if needed
  tabfor->SetState (CSS_VISIBLE, lasttab < pages.Length ());
  taback->SetState (CSS_VISIBLE, firsttab > 0);

  // lasttab can exceed the number of pages
  if (lasttab >= pages.Length ())
    lasttab = pages.Length () - 1;

  // Draw the background
  Clear (CSPAL_NOTEBOOK_BACKGROUND);

  // Display a thin 3D frame if required
  if (style & CSNBS_PAGEFRAME)
    Rect3D (rect.xmin - 1, rect.ymin - 1, rect.xmax + 1, rect.ymax + 1,
      CSPAL_NOTEBOOK_DARK3D, CSPAL_NOTEBOOK_LIGHT3D);

  // Display current page information
  if (style & CSNBS_PAGEINFO)
  {
    int yy = ((style & CSNBS_TABPOS_MASK) == CSNBS_TABPOS_TOP) ? taby + tabh :
      ((style & CSNBS_TABPOS_MASK) == CSNBS_TABPOS_BOTTOM) ? taby - infoh + 1 :
      taby + 1;
    int hh = infoh;
    if (style & CSNBS_PAGEFRAME)
    {
      hh--;
      if ((style & CSNBS_TABPOS_MASK) == CSNBS_TABPOS_BOTTOM)
        yy++;
    }
    else if ((style & CSNBS_TABPOS_MASK) >= CSNBS_TABPOS_LEFT)
      hh--;
    Rect3D (rect.xmin - 1, yy - 1, rect.xmax + 1, yy + hh,
      CSPAL_NOTEBOOK_DARK3D, CSPAL_NOTEBOOK_LIGHT3D);
    SetClipRect (rect.xmin, yy, rect.xmax, yy + hh - 2);
    Text (rect.xmin + 4, yy + (hh - TextHeight ()) / 2,
      CSPAL_NOTEBOOK_INFO_TEXT, -1, focused->GetText ());
    SetClipRect ();
  }
  else
    switch (style & CSNBS_TABPOS_MASK)
    {
      case CSNBS_TABPOS_TOP:
        Line (0, rect.ymin - 1, bound.Width (), rect.ymin - 1, CSPAL_NOTEBOOK_LIGHT3D);
        break;
      case CSNBS_TABPOS_BOTTOM:
        Line (0, rect.ymax, bound.Width (), rect.ymax, CSPAL_NOTEBOOK_DARK3D);
        break;
    }

  int delta, color;
  switch (style & CSNBS_TABPOS_MASK)
  {
    case CSNBS_TABPOS_LEFT:
      if (style & CSNBS_PAGEFRAME)
        color = CSPAL_NOTEBOOK_2LIGHT3D, delta = 2;
      else
        color = CSPAL_NOTEBOOK_LIGHT3D, delta = 1;
      Line (rect.xmin - delta, 0, rect.xmin - delta, bound.Height (), color);
      break;
    case CSNBS_TABPOS_RIGHT:
      if (style & CSNBS_PAGEFRAME)
        color = CSPAL_NOTEBOOK_2DARK3D, delta = 1;
      else
        color = CSPAL_NOTEBOOK_DARK3D, delta = 0;
      Line (rect.xmax + delta, 0, rect.xmax + delta, bound.Height (), color);
      break;
  }

  // Reset position for all tabs
  for (delta = 0; delta < pages.Length (); delta++)
    pages.Get (delta)->zorder = -1;

  // Draw the tabs in two passes: first unselected and then selected
  int zorder = 0;
  for (int selected = 0; selected < 2; selected++)
  {
    pos = (((style & CSNBS_TABPOS_MASK) < CSNBS_TABPOS_LEFT) ? tabx : taby) + curpos;
    for (int tab = lasttab; tab >= firsttab; tab--)
      if (IS_PRIMARY_PAGE (tab))
      {
        int w, h;
        GetTabSize (tab, w, h);
        pos -= ((style & CSNBS_TABPOS_MASK) < CSNBS_TABPOS_LEFT) ?
          w - tabh / 2 : h + 3;
        // Shift the color cluster by 5 for selected tab
        int i, col = selected * 5;
        bool thin = !!(style & CSNBS_THINTABS);
        int bw = thin ? 1 : 2;
        cspPageData *data = pages.Get (tab);

        if (selected == int (tab == primary))
        {
          data->zorder = zorder++;
          switch (style & CSNBS_TABPOS_MASK)
          {
            case CSNBS_TABPOS_LEFT:
              w = tabw;
              Box (tabx + bw, pos + bw, tabx + w - 1 + selected,
                pos + h - bw, col + CSPAL_NOTEBOOK_TAB_UNSEL);
              Line (tabx, pos, tabx + w - 2 + selected, pos,
                col + (thin ? CSPAL_NOTEBOOK_TAB_UNSELB1 : CSPAL_NOTEBOOK_TAB_UNSELB2));
              Line (tabx, pos + 1, tabx, pos + h - 1,
                col + (thin ? CSPAL_NOTEBOOK_TAB_UNSELB1 : CSPAL_NOTEBOOK_TAB_UNSELB2));
              Line (tabx + 1, pos + h - 1, tabx + w - 2 + selected, pos + h - 1,
                col + CSPAL_NOTEBOOK_TAB_UNSELB1);
              if (!thin)
              {
                Line (tabx + 1, pos + 1, tabx + w - 2 + selected, pos + 1,
                  col + CSPAL_NOTEBOOK_TAB_UNSELB3);
                Line (tabx + 1, pos + 2, tabx + 1, pos + h - 2,
                  col + CSPAL_NOTEBOOK_TAB_UNSELB3);
                Line (tabx + 2, pos + h - 2, tabx + w - 2 + selected, pos + h - 2,
                  col + CSPAL_NOTEBOOK_TAB_UNSELB2);
              }
              if (!selected)
                Line (tabx + w - 2, pos, tabx + w - 2, pos + h - 1,
                  CSPAL_NOTEBOOK_TAB_UNSELB2);
              break;
            case CSNBS_TABPOS_RIGHT:
              w = tabw;
              Box (tabx + 1 - selected, pos + bw, tabx + w - bw,
                pos + h - bw, col + CSPAL_NOTEBOOK_TAB_UNSEL);
              Line (tabx + 1 - selected, pos, tabx + w - 1, pos,
                col + (thin ? CSPAL_NOTEBOOK_TAB_UNSELB1 : CSPAL_NOTEBOOK_TAB_UNSELB2));
              Line (tabx + w - 1, pos + 1, tabx + w - 1, pos + h - 1,
                col + CSPAL_NOTEBOOK_TAB_UNSELB1);
              Line (tabx + 1 - selected, pos + h - 1, tabx + w - 2, pos + h - 1,
                col + CSPAL_NOTEBOOK_TAB_UNSELB1);
              if (!thin)
              {
                Line (tabx + 1 - selected, pos + 1, tabx + w - 2, pos + 1,
                  col + CSPAL_NOTEBOOK_TAB_UNSELB3);
                Line (tabx + w - 2, pos + 2, tabx + w - 2, pos + h - 2,
                  col + CSPAL_NOTEBOOK_TAB_UNSELB3);
                Line (tabx + 1 - selected, pos + h - 2, tabx + w - 2, pos + h - 2,
                  col + CSPAL_NOTEBOOK_TAB_UNSELB2);
              }
              if (!selected && (style & CSNBS_PAGEFRAME))
                Line (tabx + 1, pos, tabx + 1, pos + h - 1,
                  CSPAL_NOTEBOOK_TAB_UNSELB1);
              break;
            case CSNBS_TABPOS_TOP:
              h = tabh;
              Box (pos + bw, taby + bw, pos + w - h + 3 - bw,
                taby + h - 1 + selected, col + CSPAL_NOTEBOOK_TAB_UNSEL);
              for (i = 2; i < h - 1 + selected; i++)
                Line (pos + w - h + 1, taby + i,
                  pos + w - h + i + 1 - bw, taby + i,
                  col + CSPAL_NOTEBOOK_TAB_UNSEL);
              Line (pos, taby, pos, taby + h - 2,
                col + (thin ? CSPAL_NOTEBOOK_TAB_UNSELB1 : CSPAL_NOTEBOOK_TAB_UNSELB2));
              Line (pos + 1, taby, pos + w - h + 1, taby,
                col + (thin ? CSPAL_NOTEBOOK_TAB_UNSELB1 : CSPAL_NOTEBOOK_TAB_UNSELB2));
              Line (pos + w - h + 2, taby + 1, pos + w - 1, taby + h - 2,
                col + CSPAL_NOTEBOOK_TAB_UNSELB1);
              if (!thin)
              {
                Line (pos + 1, taby + 1, pos + 1, taby + h - 2, col + CSPAL_NOTEBOOK_TAB_UNSELB3);
                Line (pos + 2, taby + 1, pos + w - h + 1, taby + 1, col + CSPAL_NOTEBOOK_TAB_UNSELB3);
                Line (pos + w - h + 2, taby + 2, pos + w - 2, taby + h - 2,
                  col + CSPAL_NOTEBOOK_TAB_UNSELB2);
              }
              break;
            case CSNBS_TABPOS_BOTTOM:
              h = tabh;
              Box (pos + bw, taby + 1 - selected, pos + w - h + 3 - bw,
                taby + h - bw, col + CSPAL_NOTEBOOK_TAB_UNSEL);
              for (i = 2; i < h - 1 + selected; i++)
                Line (pos + w - h + 1, taby + h - 1 - i,
                  pos + w - h + i + 1 - bw, taby + h - 1 - i,
                  col + CSPAL_NOTEBOOK_TAB_UNSEL);
              Line (pos, taby + h - 1, pos, taby + 1,
                col + (thin ? CSPAL_NOTEBOOK_TAB_UNSELB1 : CSPAL_NOTEBOOK_TAB_UNSELB2));
              Line (pos + 1, taby + h - 1, pos + w - h + 1, taby + h - 1,
                col + (thin ? CSPAL_NOTEBOOK_TAB_UNSELB1 : CSPAL_NOTEBOOK_TAB_UNSELB1));
              Line (pos + w - h + 2, taby + h - 2, pos + w - 1, taby + 1,
                col + CSPAL_NOTEBOOK_TAB_UNSELB1);
              if (!thin)
              {
                Line (pos + 1, taby + h - 2, pos + 1, taby + 1, col + CSPAL_NOTEBOOK_TAB_UNSELB3);
                Line (pos + 2, taby + h - 2, pos + w - h + 1, taby + h - 2, col + CSPAL_NOTEBOOK_TAB_UNSELB2);
                Line (pos + w - h + 2, taby + h - 3, pos + w - 2, taby + 1,
                  col + CSPAL_NOTEBOOK_TAB_UNSELB2);
              }
              break;
          }

          // Now display the text/image on the tab
          if ((style & CSNBS_TABPOS_MASK) < CSNBS_TABPOS_LEFT)
            data->bound.Set (pos, taby, pos + w, taby + h);
          else
            data->bound.Set (tabx, pos, tabx + w, pos + h);
          csRect &rect = data->bound;
          w = rect.Width ();
          h = rect.Height ();
          if ((style & CSNBS_TABPOS_MASK) < CSNBS_TABPOS_LEFT)
            w -= h * 2 / 3;

          if (data->flags & NOTEBOOK_PAGE_PIXMAP)
            Sprite2D (data->image, rect.xmin + (w - data->image->Width ()) / 2,
              rect.ymin + (h - data->image->Height ()) / 2,
              data->image->Width (), data->image->Height ());
          else
          {
            int tx = rect.xmin + (w - TextWidth (data->text)) / 2;
            int ty = rect.ymin + (h - TextHeight ()) / 2;
            Text (tx, ty, col + CSPAL_NOTEBOOK_UNSEL_TEXT, -1, data->text);
            DrawUnderline (tx, ty, data->text, data->underline_pos,
              col + CSPAL_NOTEBOOK_UNSEL_TEXT);
          }
        }

        bool done = ((style & CSNBS_TABPOS_MASK) < CSNBS_TABPOS_LEFT) ?
          (pos <= tabx + TABPOS_SKIP) : (pos <= taby + TABPOS_SKIP);
        if (done) break;
      }
  }
}

bool csNotebook::SetRect (int xmin, int ymin, int xmax, int ymax)
{
  bool rc = csComponent::SetRect (xmin, ymin, xmax, ymax);
  fReposition |= rc;
  return rc;
}

bool csNotebook::InsideTab (int iIndex, int x, int y)
{
  if (iIndex < 0 || iIndex >= pages.Length ())
    return false;

  cspPageData *data = pages.Get (iIndex);
  if (data->zorder < 0
   || !data->bound.Contains (x, y))
    return false;

  if ((style & CSNBS_TABPOS_MASK) == CSNBS_TABPOS_TOP)
    return (data->bound.xmax - x) >= (data->bound.ymax - y);
  else if ((style & CSNBS_TABPOS_MASK) == CSNBS_TABPOS_BOTTOM)
    return (data->bound.xmax - x) >= (y - data->bound.ymin);
  else
    return true;
}

bool csNotebook::HandleEvent (csEvent &Event)
{
  switch (Event.Type)
  {
    case csevMouseDown:
      if (csComponent::HandleEvent (Event))
        return true;
      if (Event.Mouse.Button == 1)
      {
        // Find the "topmost" tab under the mouse
        int zmax = -1, pageno = -1;
        for (int i = pages.Length () - 1; i >= 0; i--)
          if (InsideTab (i, Event.Mouse.x, Event.Mouse.y))
          {
            cspPageData *data = pages.Get (i);
            if (data->zorder > zmax)
            { pageno = i; zmax = data->zorder; }
          }
        if (pageno >= 0)
        {
          SelectTab (pageno);
          return true;
        }
      }
      return false;
    case csevKeyDown:
      switch (Event.Key.Code)
      {
        case CSKEY_PGDN:
          if ((Event.Key.Modifiers & CSMASK_ALLSHIFTS) == CSMASK_ALT)
          {
            SelectTab (activetab + 1);
            return true;
          }
          break;
        case CSKEY_PGUP:
          if ((Event.Key.Modifiers & CSMASK_ALLSHIFTS) == CSMASK_ALT)
          {
            SelectTab (activetab - 1);
            return true;
          }
          break;
        default:
          if ((app->KeyboardOwner == NULL)
           && ((Event.Key.Modifiers & CSMASK_CTRL) == 0)
           && (Event.Key.Modifiers & CSMASK_FIRST))
          for (int i = 0; i < pages.Length (); i++)
            if (pages.Get (i)->IsHotKey (Event.Key.Code))
            {
              SelectTab (i);
              return true;
            }
          break;
      }
      break;
    case csevCommand:
      switch (Event.Command.Code)
      {
        case cscmdNotebookNextPage:
          SelectTab (activetab + 1);
          return true;
        case cscmdNotebookPrevPage:
          SelectTab (activetab - 1);
          return true;
        case cscmdNotebookScrollTabsForward:
        {
          int np = firsttab + 1;
          while (np < pages.Length ()
              && !(pages.Get (np)->flags & NOTEBOOK_PAGE_PRIMARY))
            np++;
          if ((np > firsttab) && (np < pages.Length ())
           && (pages.Get (np)->flags & NOTEBOOK_PAGE_PRIMARY))
          {
            firsttab = np;
            Invalidate ();
          }
          return true;
        }
        case cscmdNotebookScrollTabsBackward:
        {
          int np = firsttab - 1;
          while (np > 0
              && !(pages.Get (np)->flags & NOTEBOOK_PAGE_PRIMARY))
            np--;
          if ((np < firsttab) && (np >= 0)
           && (pages.Get (np)->flags & NOTEBOOK_PAGE_PRIMARY))
          {
            firsttab = np;
            Invalidate ();
          }
          return true;
        }
      }
  }
  return csComponent::HandleEvent (Event);
}

int csNotebook::FindPage (csComponent *iComponent)
{
  for (int idx = 0; idx < pages.Length (); idx++)
    if (pages.Get (idx)->page == iComponent)
      return idx;
  return -1;
}

bool csNotebook::SelectTab (int iIndex)
{
  if (iIndex < 0 || iIndex >= pages.Length ())
    return false;

  if (activetab != iIndex)
  {
    pages.Get (activetab)->page->SetState (CSS_VISIBLE, false);
    activetab = iIndex;
    pages.Get (activetab)->page->Select ();
    Invalidate ();
  }
  return true;
}

bool csNotebook::SelectTab (csComponent *iComponent)
{
  return SelectTab (FindPage (iComponent));
}

bool csNotebook::DeleteTab (csComponent *iComponent)
{
  return pages.Delete (FindPage (iComponent));
}

bool csNotebook::DeleteTab (int iIndex)
{
  return pages.Delete (iIndex);
}

bool csNotebook::AddTab (cspPageData *iPageData, const char *iInfo,
  csComponent *iBefore)
{
  // Set page title
  if (iInfo) iPageData->page->SetText (iInfo);

  // Mark the component as being a notebook page
  iPageData->page->id = ID_NOTEBOOK_PAGE;

  // Tell component to stay below any controls we may overlay above
  iPageData->page->SetState (CSS_TOPSELECT, false);
  SetZorder (iPageData->page, NULL);

  // Select this page if no other is
  if (!focused || focused->id != ID_NOTEBOOK_PAGE)
    iPageData->page->Select ();

  // Find the page to insert before
  int idx = iBefore ? FindPage (iBefore) : -1;
  if (idx < 0)
    pages.Push (iPageData);
  else
    pages.Insert (idx, iPageData);

  // And of course repaint
  Invalidate ();

  return true;
}

bool csNotebook::AddPrimaryTab (csComponent *iComponent, const char *iText,
  const char *iInfo, csComponent *iBefore)
{
  if (!iComponent || iComponent->parent != this || !iText)
    return false;

  // Prepare the private page data
  cspPageData *data = new cspPageData (iComponent, NOTEBOOK_PAGE_PRIMARY);
  PrepareLabel (iText, data->text, data->underline_pos);

  return AddTab (data, iInfo, iBefore);
}

bool csNotebook::AddSecondaryTab (csComponent *iComponent, const char *iInfo,
  csComponent *iBefore)
{
  if (!iComponent || iComponent->parent != this)
    return false;

  // Prepare the private page data
  cspPageData *data = new cspPageData (iComponent, 0);

  return AddTab (data, iInfo, iBefore);
}

bool csNotebook::AddPrimaryTab (csComponent *iComponent, csPixmap *iPixmap,
  bool iDeletePixmap, const char *iInfo, csComponent *iBefore)
{
  if (!iComponent || iComponent->parent != this || !iPixmap)
    return false;

  // Prepare the private page data
  cspPageData *data = new cspPageData (iComponent, NOTEBOOK_PAGE_PRIMARY |
    NOTEBOOK_PAGE_PIXMAP | (iDeletePixmap ? NOTEBOOK_PAGE_DELPIXMAP : 0));
  data->image = iPixmap;

  return AddTab (data, iInfo, iBefore);
}

void csNotebook::SetStyle (int iStyle)
{
  style = iStyle;
  fReposition = true;
  Invalidate ();
}

void csNotebook::GetClientRect (csRect &oRect)
{
  struct { int w, h; } maxsize;
  maxsize.w = maxsize.h = 0;
  for (int i = 0; i < pages.Length (); i++)
  {
    cspPageData *data = pages.Get (i);
    if ((data->flags & NOTEBOOK_PAGE_PIXMAP)
     || (data->flags & NOTEBOOK_PAGE_PRIMARY))
    {
      int w, h;
      GetTabSize (i, w, h);
      if (maxsize.w < w) maxsize.w = w;
      if (maxsize.h < h) maxsize.h = h;
    }
  }

  oRect.Set (0, 0, bound.Width (), bound.Height ());

  // Skip the space for tabs
  switch (style & CSNBS_TABPOS_MASK)
  {
    case CSNBS_TABPOS_TOP:    oRect.ymin += maxsize.h + 1; break;
    case CSNBS_TABPOS_BOTTOM: oRect.ymax -= maxsize.h + 1; break;
    case CSNBS_TABPOS_LEFT:   oRect.xmin += maxsize.w + 1; break;
    case CSNBS_TABPOS_RIGHT:  oRect.xmax -= maxsize.w + 1; break;
  }

  // On top or bottom we can have page information
  if (style & CSNBS_PAGEINFO)
  {
    int h = InfoHeight ();
    if ((style & CSNBS_TABPOS_MASK) == CSNBS_TABPOS_BOTTOM)
      oRect.ymax -= h;
    else
      oRect.ymin += h;
  }

  if (style & CSNBS_PAGEFRAME)
  {
    oRect.xmin++; oRect.xmax--;
    if ((style & CSNBS_TABPOS_MASK) == CSNBS_TABPOS_TOP)
      oRect.ymax--;
    else if ((style & CSNBS_TABPOS_MASK) == CSNBS_TABPOS_BOTTOM)
      oRect.ymin++;
    else
      oRect.ymin++, oRect.ymax--;
  }
}

int csNotebook::InfoHeight ()
{
  if (!(style & CSNBS_PAGEINFO))
    return 0;

  int h = TextHeight ();
  if (style & CSNBS_THINTABS)
    h += h / 2;
  else
    h += h;

  // Check if info line is not thinner than next/prev page buttons
  int minh = nextpage->bound.Height () + 2;
  if (h < minh) h = minh;

  // Add one or two pixel for divider line(s)
  if ((style & CSNBS_PAGEFRAME)
   || (style & CSNBS_TABPOS_MASK) >= CSNBS_TABPOS_LEFT)
    h += 2;
  else
    h++;

  return h;
}

bool csNotebook::GetTabSize (int iTab, int &oW, int &oH)
{
  cspPageData *data = pages.Get (iTab);
  if (data->flags & NOTEBOOK_PAGE_PIXMAP)
  {
    oW = data->image->Width ();
    oH = data->image->Height ();

    if (style & CSNBS_THINTABS)
      oW += 2 + 1, oH += 2 + 1;
    else
      oW += 4 + 2, oH += 4 + 2;
  }
  else
  {
    oW = TextWidth (data->text);
    oH = TextHeight ();

    if (style & CSNBS_THINTABS)
    {
      // Leave space for 1/8 left and 1/8 right of text
      oW += oW / 4 + 1;
      // Leave space for 1/4 above and 1/4 below text
      oH += oH / 2 + 1;
    }
    else
    {
      // Leave space for 1/4 left and 1/4 right of text
      oW += oW / 2 + 2;
      // Leave space for 1/2 above and 1/2 below text
      oH += oH + 2;
    }
  }

  // Leave space for right part of tab
  if ((style & CSNBS_TABPOS_MASK) < CSNBS_TABPOS_LEFT)
    oW += oH - 1;

  return true;
}
