/*
    Crystal Space Windowing System: window class
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
#include "csws/cswindow.h"
#include "csws/csttlbar.h"
#include "csws/csmenu.h"
#include "csws/csapp.h"
#include "csws/csdialog.h"
#include "csws/cswsutil.h"

#define TITLEBAR_TEXTURE_NAME	"csws::TitlebarButtons"

#define ADD_SYSMENU_ENTRIES(menu)				\
  if (iWindowStyle & CSWS_BUTMAXIMIZE)				\
    (void) new csMenuItem (menu, "~Maximize", cscmdMaximize);	\
  if (iWindowStyle & CSWS_BUTHIDE)				\
    (void) new csMenuItem (menu, "~Hide",     cscmdHide);	\
  if (iWindowStyle & CSWS_BUTCLOSE)				\
  {								\
    if (iWindowStyle & (CSWS_BUTMAXIMIZE | CSWS_BUTHIDE))	\
      (void) new csMenuItem (menu);				\
    (void) new csMenuItem (menu, "~Close",    cscmdClose);	\
  }

// Private menu class for system menu
class csSysMenu : public csMenu
{
  csButton *SysButton;

public:
  /// Create menu object
  csSysMenu (csComponent *iParent, csMenuFrameStyle iFrameStyle, int iMenuStyle,
    csButton *iSysButton) : csMenu (iParent, iFrameStyle, iMenuStyle)
  { SysButton = iSysButton; app->InsertClipChild (this); }

  /// Hide the menu
  virtual void Hide ();
};

void csSysMenu::Hide ()
{
  csComponent::Hide ();
  SysButton->Pressed = false;
  SysButton->Invalidate ();
}

csWindow::csWindow (csComponent *iParent, char *iTitle, int iWindowStyle,
  csWindowFrameStyle iFrameStyle) : csComponent (iParent)
{
  name="csWindow";
  state |= CSS_SELECTABLE | CSS_TOPSELECT;
  DragStyle |= CS_DRAG_SIZEABLE;
  FrameStyle = iFrameStyle;
  WindowStyle = iWindowStyle;
  csThemeWindow * thwin = (csThemeWindow *)GetTheme();
  CS_ASSERT(thwin != NULL)
  thwin->GetBorderSize(*this,FrameStyle,BorderWidth,BorderHeight);
  TitlebarHeight = thwin->GetTitleBarHeight();
  MenuHeight = thwin->GetMenuHeight();
  SetPalette (CSPAL_WINDOW);

  // Attach required handles & gadgets to window
  if (iWindowStyle & CSWS_BUTSYSMENU)
  {
    csButton *bt = new csButton (this, cscmdWindowSysMenu, 0, csbfsNone);
    SetButtBitmap (bt, "SYSMNUN", "SYSMNUP");
    bt->id = CSWID_BUTSYSMENU;

    csMenu *mn = new csSysMenu (this, csmfs3D, CSMS_DEFAULTVALUE, bt);
    ADD_SYSMENU_ENTRIES (mn);
//  mn->PlaceItems ();
    mn->Hide ();
    mn->id = CSWID_SYSMENU;
  } /* endif */
  if (iWindowStyle & CSWS_BUTCLOSE)
  {
    csComponent *bt = thwin->GetCloseButton(this);
    bt->id = CSWID_BUTCLOSE;
  } /* endif */
  if (iWindowStyle & CSWS_BUTHIDE)
  {
    csComponent *bt = thwin->GetHideButton(this);
    bt->id = CSWID_BUTHIDE;
  } /* endif */
  if (iWindowStyle & CSWS_BUTMAXIMIZE)
  {
    csComponent *bt = thwin->GetMaximizeButton(this);
    bt->id = CSWID_BUTMAXIMIZE;
  } /* endif */
  if (iWindowStyle & CSWS_TITLEBAR)
  {
    csComponent *tb = thwin->GetTitleBar(this,iTitle);
    tb->id = CSWID_TITLEBAR;
  }
  else
    SetText (iTitle);
  if (iWindowStyle & CSWS_MENUBAR)
  {
    csComponent *mn = new csMenu (this, csmfsBar, 0);
    mn->id = CSWID_MENUBAR;
  } /* endif */
  if (iWindowStyle & CSWS_TOOLBAR)
  {
    bool htb = ((iWindowStyle & CSWS_TBPOS_MASK) == CSWS_TBPOS_TOP)
            || ((iWindowStyle & CSWS_TBPOS_MASK) == CSWS_TBPOS_BOTTOM);
    csDialog *tb = new csDialog (this, htb ? csdfsHorizontal : csdfsVertical);
    tb->id = CSWID_TOOLBAR;
    tb->SetAutoGrid (2, 1, false);
    tb->SetState (CSS_SELECTABLE, false);
  } /* endif */
}

void csWindow::SetButtBitmap (csButton *button, char *id_n, char *id_p)
{
  if (!button)
    return;

  int tx,ty,tw,th;
  FindCFGBitmap (app->System, *(app->titlebardefs), id_n, &tx, &ty, &tw, &th);
  csPixmap *bmpn = new csPixmap (app->GetTexture (
    TITLEBAR_TEXTURE_NAME), tx, ty, tw, th);
  FindCFGBitmap (app->System, *(app->titlebardefs), id_p, &tx, &ty, &tw, &th);
  csPixmap *bmpp = new csPixmap (app->GetTexture (
    TITLEBAR_TEXTURE_NAME), tx, ty, tw, th);
  button->SetBitmap (bmpn, bmpp);
}

bool csWindow::SetRect (int xmin, int ymin, int xmax, int ymax)
{
  bool ret = csComponent::SetRect (xmin, ymin, xmax, ymax);

  int d_xmin = BorderWidth, d_xmax = bound.Width () - BorderWidth;
  int d_ymin = BorderHeight, d_ymax = BorderHeight + TitlebarHeight;
  int butsize = TitlebarHeight;

  // Place system menu button
  csComponent *c = GetChild (CSWID_BUTSYSMENU);
  if (c)
  {
    c->SetRect (d_xmin, d_ymin, d_xmin + butsize, d_ymax);
    d_xmin += butsize;
  } /* endif */
  // Place close button
  c = GetChild (CSWID_BUTCLOSE);
  if (c)
  {
    c->SetRect (d_xmax - butsize, d_ymin, d_xmax, d_ymax);
    d_xmax -= butsize;
  } /* endif */
  // Place maximize button
  c = GetChild (CSWID_BUTMAXIMIZE);
  if (c)
  {
    c->SetRect (d_xmax - butsize, d_ymin, d_xmax, d_ymax);
    d_xmax -= butsize;
  } /* endif */
  // Place hide button
  c = GetChild (CSWID_BUTHIDE);
  if (c)
  {
    c->SetRect (d_xmax - butsize, d_ymin, d_xmax, d_ymax);
    d_xmax -= butsize;
  } /* endif */
  // Place title bar
  c = GetChild (CSWID_TITLEBAR);
  if (c) c->SetRect (d_xmin, d_ymin, d_xmax, d_ymax);

  // Now distribute the client rectangle
  d_xmin = BorderWidth;
  d_xmax = bound.Width () - BorderWidth;
  if (WindowStyle & (CSWS_TITLEBAR | CSWID_BUTCLOSE | CSWID_BUTMAXIMIZE | CSWID_BUTHIDE))
    d_ymin = d_ymax;
  else
    d_ymin = BorderHeight;
  d_ymax = bound.Height () - BorderHeight;

  // Place system menu (hidden by default)
  c = GetChild (CSWID_SYSMENU);
  if (c)
    c->SetRect (d_xmin, d_ymin, d_xmin + c->bound.Width (),
      d_ymin + c->bound.Height ());

  // Place the menu bar
  c = GetChild (CSWID_MENUBAR);
  if (c)
  {
    int y = d_ymin + MenuHeight;
    c->SetRect (d_xmin, d_ymin, d_xmax, y);
    d_ymin = y;
  } /* endif */

  // Place the tool bar
  c = GetChild (CSWID_TOOLBAR);
  if (c)
  {
    if ((WindowStyle & CSWS_TBPOS_MASK) == CSWS_TBPOS_TOP
     || (WindowStyle & CSWS_TBPOS_MASK) == CSWS_TBPOS_BOTTOM)
      c->SetSize (d_xmax - d_xmin, 0);
    else
      c->SetSize (0, d_ymax - d_ymin);
    int ToolbarW, ToolbarH;
    c->SuggestSize (ToolbarW, ToolbarH);
    switch (WindowStyle & CSWS_TBPOS_MASK)
    {
      case CSWS_TBPOS_TOP:
      {
        int tmp = d_ymin + ToolbarH;
        c->SetRect (d_xmin, d_ymin, d_xmax, tmp);
        d_ymin = tmp;
        break;
      }
      case CSWS_TBPOS_BOTTOM:
      {
        int tmp = d_ymax - ToolbarH;
        c->SetRect (d_xmin, tmp, d_xmax, d_ymax);
        d_ymax = tmp;
        break;
      }
      case CSWS_TBPOS_LEFT:
      {
        int tmp = d_xmin + ToolbarW;
        c->SetRect (d_xmin, d_ymin, tmp, d_ymax);
        d_xmin = tmp;
        break;
      }
      case CSWS_TBPOS_RIGHT:
      {
        int tmp = d_xmax - ToolbarW;
        c->SetRect (tmp, d_ymin, d_xmax, d_ymax);
        d_xmax = tmp;
        break;
      }
    } /* endswitch */
  } /* endif */

  // Place the client window
  c = GetChild (CSWID_CLIENT);
  if (c)
  {
    if (WindowStyle & CSWS_CLIENTBORDER)
    {
      d_xmin++; d_xmax--;
      d_ymin++; d_ymax--;
    } /* endif */
    c->SetRect (d_xmin, d_ymin, d_xmax, d_ymax);
  } /* endif */
  return ret;
}

void csWindow::Draw ()
{
  int bw = 0, bh = 0;
  int li,di,back;

  csThemeWindow * thwin = (csThemeWindow *) GetTheme();

  li = thwin->GetBorderLightColor();
  di = thwin->GetBorderDarkColor();
  back = thwin->GetBackgroundColor();
  switch (FrameStyle)
  {
    case cswfsNone:
      break;
    case cswfsThin:
      thwin->GetBorderSize(*this,csthfsThin,bw,bh);
      thwin->DrawBorder(*this,csthfsThin,bw,bh,di,di);
      break;
    case cswfs3D:
    {
      if ((BorderWidth > 0) && (BorderHeight > 0))
      {
        thwin->GetBorderSize(*this,csthfsThin,bw,bh);
        thwin->DrawBorder(*this,csthfsThin,bw,bh,li,di);
      } /* endif */
      if ((BorderWidth > 1) && (BorderHeight > 1))
      {
        thwin->GetBorderSize(*this,csthfsThinRect,bw,bh);
        thwin->DrawBorder(*this,csthfsThinRect,bw,bh,li,di);
      } /* endif */
      break;
    }
  } /* endswitch */

  Box (bw, bw, bound.Width () - bw, bound.Height () - bh,back);

  if (WindowStyle & CSWS_CLIENTBORDER)
  {
    csComponent *c = GetChild (CSWID_CLIENT);
    if (c)
      Rect3D (c->bound.xmin - 1, c->bound.ymin - 1, c->bound.xmax + 1,
        c->bound.ymax + 1, li, di);
  } /* endif */
  csComponent::Draw ();
}

void csWindow::SetState (int mask, bool enable)
{
  int oldstate = state;
  csComponent::SetState (mask, enable);
  if ((state ^ oldstate) & CSS_FOCUSED)
  {
    csComponent *c = GetChild (CSWID_TITLEBAR);
    if (c)
      c->Invalidate ();
  } /* endif */
}

void csWindow::SetBorderSize (int w, int h)
{
  BorderWidth = w;
  BorderHeight = h;
  csComponent::SetRect (bound);
}

bool csWindow::HandleEvent (iEvent &Event)
{
  switch (Event.Type)
  {
    case csevMouseDown:
    case csevMouseMove:
      if (csComponent::HandleEvent (Event))
        return true;
      if (HandleDragEvent (Event, BorderWidth, BorderHeight))
        return true;
      return false;
    case csevCommand:
      switch (Event.Command.Code)
      {
        case cscmdButtonDown:
          if (Event.Command.Info != (void *)GetChild (CSWID_BUTSYSMENU))
	  {
	    Select ();
            return false;
	  }
          // fallback to system menu popup
        case cscmdWindowSysMenu:
        {
          csComponent *c = GetChild (CSWID_SYSMENU);
          if (c)
            if (c->GetState (CSS_FOCUSED))
              c->Hide ();
            else
            {
              // Emulate 'mouse down' event for menu
              csEvent me (0, csevMouseDown, -1, -1, 1, 0);
              c->HandleEvent (me);
            } /* endif */
          return true;
        }
        case cscmdWindowSetClient:
          if (Event.Command.Info)
          {
            csComponent *client = (csComponent *)Event.Command.Info;
            if (client->parent != this)
              return true;
            client->id = CSWID_CLIENT;
            SetRect (bound.xmin, bound.ymin, bound.xmax, bound.ymax);
            SetFocused (client);
            Event.Command.Info = NULL;
          } /* endif */
          return true;
      } /* endswitch */
      break;
  } /* endswitch */
  return csComponent::HandleEvent (Event);
}

void csWindow::FixSize (int &newW, int &newH)
{
  int minw = BorderWidth * 2 + TitlebarHeight * 6;
  int minh = BorderHeight * 2 + TitlebarHeight;
  csComponent *c = GetChild (CSWID_MENUBAR);
  if (c) minh = c->bound.ymax + BorderHeight + 1;
  c = GetChild (CSWID_TOOLBAR);
  if (c)
  {
    if ((WindowStyle & CSWS_TBPOS_MASK) == CSWS_TBPOS_TOP
     || (WindowStyle & CSWS_TBPOS_MASK) == CSWS_TBPOS_BOTTOM)
      minh += c->bound.Height ();
    else
    {
      int mw = BorderWidth * 2 + c->bound.Width ();
      if (mw > minw) minw = mw;
    } /* endif */
  } /* endif */

  c = GetChild (CSWID_CLIENT);
  if (c)
  {
    WindowToClient (minw, minh);
    c->FixSize (minw, minh);
    ClientToWindow (minw, minh);
  }
  // Don't allow too small windows
  if (newW < minw) newW = minw;
  if (newH < minh) newH = minh;
}

bool csWindow::Maximize ()
{
  if (!Maximized && (DragStyle & CS_DRAG_SIZEABLE) && parent)
  {
    OrgBound.Set (bound);
    csRect newbound (- BorderWidth, - BorderHeight,
      parent->bound.Width () + BorderWidth,
      parent->bound.Height () + BorderHeight);
    // give a chance to parent window to limit "maximize" bounds
    parent->SendCommand (cscmdLimitMaximize, (void *)&newbound);
    csComponent::SetRect (newbound);
    Maximized = true;

    csButton *bt = (csButton *)GetChild (CSWID_BUTMAXIMIZE);
    SetButtBitmap (bt, "MAXOFFN", "MAXOFFP");
    return true;
  } /* endif */
  return false;
}

bool csWindow::Restore ()
{
  if (csComponent::Restore ())
  {
    csButton *bt = (csButton *)GetChild (CSWID_BUTMAXIMIZE);
    SetButtBitmap (bt, "MAXN", "MAXP");
    return true;
  } /* endif */
  return false;
}

void csWindow::SetTitleHeight (int iHeight)
{
  TitlebarHeight = iHeight;
  csComponent::SetRect (bound);
}

void csWindow::SetMenuBarHeight (int iHeight)
{
  MenuHeight = iHeight;
  csComponent::SetRect (bound);
}

void csWindow::SetText (const char *iText)
{
  csComponent *c = GetChild (CSWID_TITLEBAR);
  if (c)
    c->SetText (iText);
  else
    csComponent::SetText (iText);
}

void csWindow::GetText (char *oText, int iTextSize) const
{
  csComponent *c = GetChild (CSWID_TITLEBAR);
  if (c)
    c->GetText (oText, iTextSize);
  else if (iTextSize)
    *oText = 0;
}

const char *csWindow::GetText () const
{
  csComponent *c = GetChild (CSWID_TITLEBAR);
  if (c)
    return c->GetText ();
  else
    return text;
}

void csWindow::ClientToWindow (int &ClientW, int &ClientH)
{
  ClientW += BorderWidth * 2;
  ClientH += BorderHeight * 2;
  if (WindowStyle & (CSWS_TITLEBAR | CSWID_BUTCLOSE | CSWID_BUTMAXIMIZE | CSWID_BUTHIDE))
    ClientH += TitlebarHeight;
  csComponent *c = GetChild (CSWID_MENUBAR);
  if (c)
    ClientH += c->bound.Height ();
  c = GetChild (CSWID_TOOLBAR);
  if (c)
    ClientH += c->bound.Height ();
  if (WindowStyle & CSWS_CLIENTBORDER)
  {
    ClientW += 2;
    ClientH += 2;
  }
}

void csWindow::WindowToClient (int &ClientW, int &ClientH)
{
  // determine the amount of space the window takes to draw its borders and stuff
  int w=0, h=0;
  ClientToWindow (w, h);
  ClientW -= w;
  ClientH -= h;
}
