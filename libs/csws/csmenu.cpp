/*
    Crystal Space Windowing System: menu class
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

#include <string.h>

#define SYSDEF_CASE
#include "cssysdef.h"
#include "csws/cscomp.h"
#include "csws/csmenu.h"
#include "csws/csapp.h"
#include "csws/cswsaux.h"
#include "cssys/csinput.h"

// Menu texture name
#define MENU_TEXTURE_NAME       "csws::Menu"
// Amount of space at left and at right of each menu item
#define MENUITEM_XSPACE         (9+2)
// Amount of space at top and at bottom of each menu item
#define MENUITEM_YSPACE         2
// Amount of space for \t character in menu items
#define MENUITEM_TABSPACE       20

csPixmap *csMenuItem::sprchecked = NULL;
csPixmap *csMenuItem::sprsubmenu = NULL;
static int menuitemref = 0;

csMenuItem::csMenuItem (csComponent *iParent, const char *iText,
  int iCommandCode, int iStyle) : csComponent (iParent)
{
  Init ();
  SetText (iText);
  Style = iStyle;
  id = CommandCode = iCommandCode;
  state |= CSS_SELECTABLE;
}

csMenuItem::csMenuItem (csComponent *iParent, int iStyle)
  : csComponent (iParent)
{
  Init ();
  Style = iStyle | CSMIS_SEPARATOR;
  CommandCode = cscmdNothing;
}

csMenuItem::csMenuItem (csComponent *iParent, const char *iText,
  csMenu *iSubMenu, int iStyle) : csComponent (iParent)
{
  Init ();
  SetText (iText);
  Style = iStyle;
  CommandCode = cscmdNothing;
  Insert (iSubMenu);
  parent->GetClipParent ()->InsertClipChild (iSubMenu);
  iSubMenu->Hide ();
  state |= CSS_SELECTABLE;
}

void csMenuItem::Init ()
{
  SetPalette (CSPAL_MENUITEM);
  text = NULL;
  info = NULL;
  underline_pos = -1;

  menuitemref++;
  if (app)
  {
    // If  'checked' and 'submenu' images are not loaded, load them
    if (!sprchecked)
      sprchecked = new csSimplePixmap (app->GetTexture (
        MENU_TEXTURE_NAME), 0, 0, 9, 9);
    if (!sprsubmenu)
      sprsubmenu = new csSimplePixmap (app->GetTexture (
        MENU_TEXTURE_NAME), 9, 0, 9, 9);
  } /* endif */
}

csMenuItem::~csMenuItem ()
{
  if (--menuitemref == 0)
  {
    delete sprchecked; sprchecked = NULL;
    delete sprsubmenu; sprsubmenu = NULL;
  }
  delete [] info;
}

void csMenuItem::SetText (const char *iText)
{
  delete [] text;
  delete [] info;

  text = NULL;
  info = NULL;
  underline_pos = -1;

  if (!iText)
    return;

  int sl = strlen (iText);
  int cc = 0;
  int i;

  for (i = 0; i < sl; i++)
    if (iText [i] == '\t')
      break;
    else if (iText [i] != '~')
      cc++;
  text = new char [cc + 1];
  if (i < sl)
    info = csStrNew (iText + i + 1);

  cc = 0;
  for (i = 0; i < sl; i++)
    if (iText [i] == '\t')
      break;
    else if (iText [i] != '~')
      text[cc++] = iText[i];
    else
      underline_pos = cc;
  text [cc] = 0;
  Invalidate ();
}

void csMenuItem::SuggestSize (int &w, int &h)
{
  w = 0; h = 0;

  if (text && parent)
  {
    int fh, fw = GetTextSize (text, &fh);
    w += fw;
    h += fh;
    if (info)
      w += MENUITEM_TABSPACE + GetTextSize (info);
  } /* endif */

  // Leave a bit of space at left, right, top and bottom
  w += MENUITEM_XSPACE * 2;
  h += MENUITEM_YSPACE * 2;

  if (Style & CSMIS_SEPARATOR)
  {
    if (((csMenu *)parent)->IsMenuBar ())
    {
      h = parent->bound.Height () - MENUITEM_YSPACE * 2;
      w = 4;
    } else
    {
      w = parent->bound.Width () - MENUITEM_XSPACE * 2;
      h = 4;
    } /* endif */
  } /* endif */
}

bool csMenuItem::SetRect (int xmin, int ymin, int xmax, int ymax)
{
  bool ret = csComponent::SetRect (xmin, ymin, xmax, ymax);
  if (focused)
  {
    csRect dest;
    if (((csMenu *)parent)->IsMenuBar ())
    {
      dest.Set (0, bound.Height (), focused->bound.Width (),
        bound.Height () + focused->bound.Height ());
      int dX = bound.xmin + focused->bound.Width () - parent->bound.Width ();
      if (dX > 0)
        dest.Move (-dX, 0);
    } else
      dest.Set (bound.Width (), 0, bound.Width () + focused->bound.Width (),
        focused->bound.Height ());
    focused->SetRect (dest);
  } /* endif */
  return ret;
}

bool csMenuItem::HandleEvent (iEvent &Event)
{
  // If we have a child menu and it is visible, pass mouse & keyboard events
  if (focused
   && focused->GetState (CSS_VISIBLE))
  {
    if (IS_MOUSE_EVENT (Event))
    {
      int dX = focused->bound.xmin, dY = focused->bound.ymin;

      Event.Mouse.x -= dX;
      Event.Mouse.y -= dY;
      bool ret = focused->HandleEvent (Event);
      Event.Mouse.x += dX;
      Event.Mouse.y += dY;
      if (ret)
        return true;
    } else if (IS_KEYBOARD_EVENT (Event))
    {
      if (focused->HandleEvent (Event))
        return true;
    } /* endif */
  } /* endif */

  switch (Event.Type)
  {
    case csevKeyDown:
      // Check hot key
      if ((underline_pos >= 0)
       && ((Event.Key.Modifiers & CSMASK_CTRL) == 0)
       && ((UPPERCASE (Event.Key.Char) == UPPERCASE (text [underline_pos]))
        || (UPPERCASE (Event.Key.Code) == UPPERCASE (text [underline_pos]))))
      {
        Press ();
        return true;
      }
      switch (Event.Key.Code)
      {
        case CSKEY_LEFT:
          if ((focused)
           && (focused->GetState (CSS_VISIBLE))
           && (!((csMenu *)parent)->IsMenuBar ()))
          {
            parent->SendCommand (cscmdMenuSetDropFlag, (void *)false);
            return true;
          };
          return false;
        case CSKEY_RIGHT:
          if ((focused)
           && (!focused->GetState (CSS_VISIBLE))
           && (!((csMenu *)parent)->IsMenuBar ()))
          {
            parent->SendCommand (cscmdMenuSetDropFlag, (void *)true);
            return true;
          };
          return false;
      } /* endswitch */
      break;
    case csevMouseDown:
      if (Event.Mouse.Button == 2)
        parent->SendCommand (cscmdDeactivateMenu, (void *)cscmdCancel);
      break;
    case csevMouseUp:
      if ((Event.Mouse.Button == 1)
       && bound.Contains (Event.Mouse.x, Event.Mouse.y))
        Press ();
      break;
    case csevCommand:
      switch (Event.Command.Code)
      {
        case cscmdMenuSetDropFlag:
        case cscmdDeactivateMenu:
          if (parent)
            // pass command to parent
            Event.Command.Info = parent->SendCommand (Event.Command.Code,
              Event.Command.Info);
          return true;
        case cscmdMenuPlaceItems:
          if (focused)
            focused->SendCommand (cscmdMenuPlaceItems);
          return true;
        case cscmdMenuCaptureMouse:
          parent->SendCommand (cscmdMenuCaptureMouse, this);
          Event.Command.Info = NULL;
          return true;
        case cscmdActivate:
          Press ();
          return true;
        case cscmdMenuItemCheck:
          if (Event.Command.Info)
            Style |= CSMIS_CHECKED;
          else
            Style &= ~CSMIS_CHECKED;
          Invalidate ();
          return true;
        case cscmdMenuItemGetStyle:
          Event.Command.Info = (void *)Style;
          return true;
        case cscmdMenuItemFindId:
          if (focused)
            Event.Command.Info = ((csMenu *)focused)->GetItem ((int)Event.Command.Info);
          else
            if (CommandCode == (int)Event.Command.Info)
              Event.Command.Info = this;
            else
              Event.Command.Info = NULL;
          return true;
        default:
          // resend unknown commands to parent
          if (parent)
            parent->HandleEvent (Event);
          return true;
      } /* endswitch */
      break;
  } /* endswitch */

  return csComponent::HandleEvent (Event);
}

void csMenuItem::Draw ()
{
  bool selected = (((csMenu *)parent)->current == this);
  if (!parent->GetState (CSS_FOCUSED) || !GetState (CSS_SELECTABLE))
    selected = false;

  // If this item has a submenu, show it or hide it depending on status
  if (focused && !GetState (CSS_DISABLED))
  {
    bool vis = (parent->SendCommand (cscmdMenuQueryDropFlag) != 0);
    if (((!vis) || (!selected)) && (focused->GetState (CSS_VISIBLE)))
    {
      focused->Hide ();
      focused->SendCommand (cscmdMenuSetItem, NULL);
    }
    else if ((vis) && (selected) && (focused->GetState (CSS_VISIBLE) == 0))
    {
      focused->SendCommand (cscmdMenuSetDropFlag, (void *)false);
      focused->Show (true);
      // If no menu has captured the mouse, select last selected item
      if (!((csMenu *)focused)->current && app->MouseOwner == NULL)
        focused->SendCommand (cscmdMenuSetLastItem);
    } /* endif */
  } /* endif */

  // Draw the background
  Clear (selected ? CSPAL_MENUITEM_SELECTION : CSPAL_MENUITEM_BACKGROUND);
  // Draw the item text
  if (text)
  {
    int txtcol = GetState (CSS_SELECTABLE) ?
      (selected ? CSPAL_MENUITEM_STEXT : CSPAL_MENUITEM_UTEXT) :
      CSPAL_MENUITEM_DTEXT;
    Text (MENUITEM_XSPACE, MENUITEM_YSPACE, txtcol, -1, text);
    DrawUnderline (MENUITEM_XSPACE, MENUITEM_YSPACE, text, underline_pos, txtcol);
    if (info)
      Text (bound.Width () - MENUITEM_XSPACE - GetTextSize (info), MENUITEM_YSPACE,
        txtcol, -1, info);
  } /* endif */

  if (Style & CSMIS_SEPARATOR)
  {
    if (((csMenu *)parent)->IsMenuBar ())
    {
      int x = bound.Width () / 2;
      Line (x, 0, x, bound.Height (), CSPAL_MENUITEM_DARK3D);
      x++;
      Line (x, 0, x, bound.Height (), CSPAL_MENUITEM_LIGHT3D);
    } else
    {
      int y = bound.Height () / 2;
      Line (0, y, bound.Width (), y, CSPAL_MENUITEM_DARK3D);
      y++;
      Line (0, y, bound.Width (), y, CSPAL_MENUITEM_LIGHT3D);
    } /* endif */
  } /* endif */

  if (Style & CSMIS_CHECKED)
    Pixmap (sprchecked, 1, (bound.Height () - 8) / 2);
  if (focused && !((csMenu *)parent)->IsMenuBar ())
    Pixmap (sprsubmenu, bound.Width () - 9, (bound.Height () - 8) / 2);
}

void csMenuItem::Press ()
{
  // Drop the submenu if there is one
  if (focused)
    parent->SendCommand (cscmdMenuSetDropFlag, (void *)true);
  // Set parent's current item to this
  parent->SendCommand (cscmdMenuSetItem, this);
  // If parent is focused, deactivate it
  if (!focused
   && !(Style & CSMIS_NOCLOSE)
   && parent->GetState (CSS_FOCUSED))
    parent->SendCommand (cscmdDeactivateMenu,
      (void *)(CommandCode ? CommandCode : (int)cscmdCancel));
  // Send the command to parent
  if (CommandCode != cscmdNothing)
    parent->SendCommand (CommandCode, this);
}

csMenu::csMenu (csComponent *iParent, csMenuFrameStyle iFrameStyle,
  int iMenuStyle) : csComponent (iParent)
{
  FrameStyle = iFrameStyle;
  SetPalette (CSPAL_MENU);
  MenuStyle = iMenuStyle;
  oldparentfocus = NULL;
  SubMenuOpened = false;
  fPlaceItems = false;
  switch (iFrameStyle)
  {
    case csmfsNone:
      BorderWidth = BorderHeight = 0;
      break;
    case csmfsThin:
      BorderWidth = BorderHeight = 1;
      break;
    case csmfsBar:
      BorderWidth = 0;
      BorderHeight = 2;
      break;
    case csmfs3D:
      BorderWidth = BorderHeight = 2;
      break;
  } /* endswitch */
  state |= CSS_SELECTABLE;
  if (!IsMenuBar ())
    state |= CSS_TOPSELECT;
  current = NULL;
  first = NULL;
  last = NULL;
}

void csMenu::Draw ()
{
  if (fPlaceItems)
    PlaceItems ();

  switch (FrameStyle)
  {
    case csmfsNone:
      break;
    case csmfsThin:
      Rect3D (0, 0, bound.Width (), bound.Height (), CSPAL_MENU_2DARK3D,
        CSPAL_MENU_2DARK3D);
      break;
    case csmfsBar:
      Line (0, 0, bound.Width (), 0, CSPAL_MENU_2LIGHT3D);
      Line (0, 1, bound.Width (), 1, CSPAL_MENU_LIGHT3D);
      Line (0, bound.Height () - 2, bound.Width (), bound.Height () - 2,
        CSPAL_MENU_DARK3D);
      Line (0, bound.Height () - 1, bound.Width (), bound.Height () - 1,
        CSPAL_MENU_2DARK3D);
      break;
    case csmfs3D:
      Rect3D (0, 0, bound.Width (), bound.Height (),
        CSPAL_MENU_2DARK3D, CSPAL_MENU_2LIGHT3D);
      Rect3D (1, 1, bound.Width () - 1, bound.Height () - 1,
        CSPAL_MENU_DARK3D, CSPAL_MENU_LIGHT3D);
      break;
  } /* endswitch */
  Box (BorderWidth, BorderHeight, bound.Width () - BorderWidth,
    bound.Height () - BorderHeight, CSPAL_MENU_BACKGROUND);

  // Draw separators between columns, if any
  if (!IsMenuBar ())
    for (csComponent *cur = focused; cur; cur = cur->next)
    {
      if (int (cur->SendCommand (cscmdMenuItemGetStyle)) & CSMIS_NEWCOLUMN)
      {
        Line (cur->bound.xmin - 2, BorderHeight,
          cur->bound.xmin - 2, bound.Height () - BorderHeight,
          CSPAL_MENU_LIGHT3D);
        Line (cur->bound.xmin - 1, BorderHeight,
          cur->bound.xmin - 1, bound.Height () - BorderHeight,
          CSPAL_MENU_DARK3D);
      } /* endif */
      if (cur->next == focused)
        break;
    } /* endfor */

  csComponent::Draw ();
}

bool csMenu::SetCurrent (csComponent *newCurrent, bool DropSubmenu)
{
  if (newCurrent && newCurrent->parent != this)
    return false;

  if (newCurrent != current)
  {
    if (current)
    {
      current->SetState (CSS_FOCUSED, false);
      current->Invalidate ();
    }
    current = newCurrent;
    if (current)
    {
      current->SetState (CSS_FOCUSED, true);
      current->Invalidate ();
    }
  } /* endif */
  if (current)
    last = current;
  if (DropSubmenu && (!SubMenuOpened))
  {
    SubMenuOpened = true;
    if (current)
      current->Invalidate ();
  }
  return true;
}

bool csMenu::ExecuteKey (int key)
{
  if (!current)
  {
    SetCurrent (last);
    if (!current)
      SetCurrent (focused);
    if (!current || current->GetState (CSS_SELECTABLE))
      return true;
  } /* endif */

  int closedist = 9999, oppdist = 0;
  csComponent *cur = current;
  csComponent *closest = NULL;
  csComponent *opposite = NULL;

  do
  {
    if (cur->GetState (CSS_SELECTABLE) && (cur != current))
      switch (key)
      {
        case CSKEY_UP:
          if (current->bound.xmin == cur->bound.xmin)
          {
            if ((current->bound.ymin > cur->bound.ymin)
             && (closedist > ABS (current->bound.ymin - cur->bound.ymin)))
            {
              closedist = ABS (current->bound.ymin - cur->bound.ymin);
              closest = cur;
            } /* endif */
            if ((current->bound.ymin < cur->bound.ymin)
             && (oppdist < ABS (current->bound.ymin - cur->bound.ymin)))
            {
              oppdist = ABS (current->bound.ymin - cur->bound.ymin);
              opposite = cur;
            } /* endif */
          } /* endif */
          break;
        case CSKEY_DOWN:
          if (current->bound.xmin == cur->bound.xmin)
          {
            if ((current->bound.ymin < cur->bound.ymin)
             && (closedist > ABS (current->bound.ymin - cur->bound.ymin)))
            {
              closedist = ABS (current->bound.ymin - cur->bound.ymin);
              closest = cur;
            } /* endif */
            if ((current->bound.ymin > cur->bound.ymin)
             && (oppdist < ABS (current->bound.ymin - cur->bound.ymin)))
            {
              oppdist = ABS (current->bound.ymin - cur->bound.ymin);
              opposite = cur;
            } /* endif */
          } /* endif */
          break;
        case CSKEY_LEFT:
          if (current->bound.ymin == cur->bound.ymin)
          {
            if ((current->bound.xmin > cur->bound.xmin)
             && (closedist > ABS (current->bound.xmin - cur->bound.xmin)))
            {
              closedist = ABS (current->bound.xmin - cur->bound.xmin);
              closest = cur;
            } /* endif */
            if ((current->bound.xmin < cur->bound.xmin)
             && (oppdist < ABS (current->bound.xmin - cur->bound.xmin)))
            {
              oppdist = ABS (current->bound.xmin - cur->bound.xmin);
              opposite = cur;
            } /* endif */
          } /* endif */
          break;
        case CSKEY_RIGHT:
          if (current->bound.ymin == cur->bound.ymin)
          {
            if ((current->bound.xmin < cur->bound.xmin)
             && (closedist > ABS (current->bound.xmin - cur->bound.xmin)))
            {
              closedist = ABS (current->bound.xmin - cur->bound.xmin);
              closest = cur;
            } /* endif */
            if ((current->bound.xmin > cur->bound.xmin)
             && (oppdist < ABS (current->bound.xmin - cur->bound.xmin)))
            {
              oppdist = ABS (current->bound.xmin - cur->bound.xmin);
              opposite = cur;
            } /* endif */
          } /* endif */
          break;
      } /* endswitch */
    cur = cur->next;
  } while (cur != current); /* enddo */
  if (closest)
    SetCurrent (closest);
  else if (opposite)
    SetCurrent (opposite);
  else
    return false;
  return true;
}

bool csMenu::CurrentHandleEvent (iEvent &Event)
{
  bool ret = false;
  if (current)
  {
    int dX = current->bound.xmin, dY = current->bound.ymin;
    if (IS_MOUSE_EVENT (Event))
    {
      Event.Mouse.x -= dX;
      Event.Mouse.y -= dY;
    } /* endif */
    ret = current->HandleEvent (Event);
    if (IS_MOUSE_EVENT (Event))
    {
      Event.Mouse.x += dX;
      Event.Mouse.y += dY;
    } /* endif */
  } /* endif */
  return ret;
}

static bool do_findmouse (csComponent *child, void *param)
{
  iEvent *Event = (iEvent *)param;
  if (child->GetState (CSS_SELECTABLE) == 0)
    return false;
  return child->bound.Contains (Event->Mouse.x, Event->Mouse.y);
}

static bool do_forall (csComponent *child, void *param)
{
  return child->HandleEvent (*((iEvent *)param));
}

bool csMenu::HandleEvent (iEvent &Event)
{
  switch (Event.Type)
  {
    case csevMouseUp:
    {
      if (Event.Mouse.Button != 1)
        break;

      csComponent *item = ForEach (do_findmouse, (void *)&Event, true);
      if (item)
      {
        item->SendCommand (cscmdActivate);
        if (item->focused)
          Deactivate (cscmdCancel);
      }
      else
        if (!CurrentHandleEvent (Event))
          Deactivate (cscmdCancel);

      return true;
    }
    case csevMouseDown:
      if (Event.Mouse.Button != 1)
        break;
      Select ();
      if (parent->SendCommand (cscmdMenuCaptureMouse, this))
        app->CaptureMouse (this);
      // fallback to MouseMove
    case csevMouseMove:
    {
      if (GetState (CSS_FOCUSED))
      {
        // Find the item under cursor
        csComponent *item = (csComponent *)ForEach (do_findmouse, (void *)&Event, true);
        // If mouse is not captured, don't deselect items
        if (item || app->MouseOwner)
          // If menu item has a child menu, don't deselect it
          if (item
           || !current
           || !current->focused
           || !current->focused->GetState (CSS_VISIBLE))
            SetCurrent (item, true);
      }
      CurrentHandleEvent (Event);
      return true;
    } /* endif */
    case csevKeyDown:
    {
      if (GetState (CSS_FOCUSED) && current
       && CurrentHandleEvent (Event))
        return true;
      switch (Event.Key.Code)
      {
        case CSKEY_ESC:
          if (parent->SendCommand (cscmdMenuSetDropFlag, (void *)false) == NULL)
          {
            Deactivate (cscmdCancel);
            return true;
          }
          return false;
        case CSKEY_ENTER:
          if (current)
          {
            current->SendCommand (cscmdActivate);
            return true;
          }
          return false;
        case CSKEY_LEFT:
          return ExecuteKey (CSKEY_LEFT);
        case CSKEY_RIGHT:
          return ExecuteKey (CSKEY_RIGHT);
        case CSKEY_UP:
          return ExecuteKey (CSKEY_UP);
        case CSKEY_DOWN:
          if (ExecuteKey (CSKEY_DOWN))
            return true;
          else
          {
            if (IsMenuBar () && !SubMenuOpened)
            {
              SendCommand (cscmdMenuSetDropFlag, (void *)true);
              return true;
            } /* endif */
            return false;
          } /* endif */
        default:
          return !!(ForEach (do_forall, &Event));
      } /* endswitch */
      break;
    }
    case csevCommand:
      switch (Event.Command.Code)
      {
        case cscmdDeactivateMenu:
          if (parent->SendCommand (cscmdDeactivateMenu, Event.Command.Info))
            Deactivate ((int)Event.Command.Info);
          Event.Command.Info = NULL;
          return true;
        case cscmdMenuPlaceItems:
          PlaceItems ();
          return true;
        case cscmdMenuCaptureMouse:
          if (parent->SendCommand (cscmdMenuCaptureMouse, this))
            app->CaptureMouse (this);
          Event.Command.Info = NULL;
          return true;
        case cscmdMenuSetItem:
          if (SetCurrent ((csComponent *)Event.Command.Info))
            Event.Command.Info = NULL;
          return true;
        case cscmdMenuSetLastItem:
          if (!current)
            SetCurrent (last);
          if (!current)
            SetCurrent (focused);
          return true;
        case cscmdMenuQueryDropFlag:
          Event.Command.Info = (void *)SubMenuOpened;
          return true;
        case cscmdMenuSetDropFlag:
          if (SubMenuOpened != (bool)Event.Command.Info)
          {
            SubMenuOpened = (bool)Event.Command.Info;
            if (current)
              current->Invalidate ();
            Event.Command.Info = this;
          } /* endif */
          return true;
        default:
          // resend unknown commands to parent
          if (parent)
            parent->HandleEvent (Event);
          return true;
      } /* endswitch */
      break;
  } /* endswitch */

  if (CurrentHandleEvent (Event))
    return true;
  return csComponent::HandleEvent (Event);
}

bool csMenu::PreHandleEvent (iEvent &Event)
{
  if (parent->GetState (CSS_FOCUSED)
   && GetState (CSS_VISIBLE)
   && !GetState (CSS_FOCUSED)
   && (Event.Type == csevKeyDown))
  {
    // if we aren't active, and ALT is not pressed,
    // or Ctrl is pressed, skip this event
    if (((Event.Key.Modifiers & CSMASK_ALT) == 0)
     || (Event.Key.Modifiers & CSMASK_CTRL))
      return false;
    // Cycle through all menu items, checking for hotkey
    if (ForEach (do_forall, &Event))
    {
      // Activate this menu
      Select ();
      return true;
    } /* endif */
  } /* endif */
  return csComponent::PreHandleEvent (Event);
}

void csMenu::Deactivate (int DismissCode)
{
  // If parent didn't eat cscmdDeactivateMenu, we'll deactivate
  if (parent->SendCommand (cscmdDeactivateMenu, (void *)DismissCode))
  {
    // If we're Execute()'ing, dismiss
    if (app->FocusOwner == this)
      app->Dismiss (DismissCode);
    // If we're focused, focus on previously focused component
    if (parent && (parent->focused == this) && oldparentfocus)
      parent->SetFocused (oldparentfocus);
  } /* endif */

  // If we captured the mouse, release it
  if (app->MouseOwner == this)
    app->CaptureMouse (NULL);
}

void csMenu::SetItemWidth (csComponent *start, int count, int width)
{
  if ((count == 0) || IsMenuBar ())
    return;

  csComponent *cur = first;
  bool fill = false;
  while (cur)
  {
    fill = fill || (cur == start);
    if (fill)
    {
      cur->SetRect (cur->bound.xmin, cur->bound.ymin,
        cur->bound.xmin + width, cur->bound.ymax);
      if (--count == 0)
        return;
    } /* endif */

    cur = cur->next;
    if (cur == first)
      return;
  } /* endwhile */
}

static bool do_place_items (csComponent *child, void *param)
{
  (void)param;
  child->SendCommand (cscmdMenuPlaceItems);
  return false;
}

void csMenu::PlaceItems ()
{
  fPlaceItems = false;

  // Call PlaceItems for child chain
  ForEach (do_place_items);

  int curx = BorderWidth, cury = BorderHeight;
  int maxw = 0, maxh = 0;
  int prevh = 0, colw = 0;
  int colcount = 0;

  // Remember first menu item on first invocation
  if (!first)
    first = focused;

  csComponent *cur = (csComponent *)first;
  csComponent *colstart = cur;
  csComponent *lastcol = NULL;

  while (cur)
  {
    // Decide where to place this menu item
    if ((int (cur->SendCommand (cscmdMenuItemGetStyle)) & CSMIS_NEWCOLUMN)
     || IsMenuBar ())
    {
      // Start a new column
      curx += colw;
      if (!IsMenuBar ())
        curx += 2;
      cury = BorderHeight;
      // Set width of preceeding items
      SetItemWidth (colstart, colcount, colw);
      colw = 0; colcount = 0; colstart = cur;
    }
    else
    {
      // Start a new row
      cury += prevh;
    } /* endif */

    // Query current item width and height
    int w, h;
    cur->SuggestSize (w, h);

    // If this is a menu bar, center the item vertically
    if (IsMenuBar ())
      cury = (bound.Height () - h) / 2;

    // Set current item x,y and height
    cur->SetRect (curx, cury, curx + w, cury + h);
    colcount++;

    if (int (cur->SendCommand (cscmdMenuItemGetStyle)) & CSMIS_NEWCOLUMN)
      lastcol = cur;

    if (curx + w > maxw) maxw = curx + w;
    if (cury + h > maxh) maxh = cury + h;
    if (w > colw) colw = w;
    prevh = h;
    cur = (csComponent *)cur->next;
    if (cur == first)
      break;
  } /* endwhile */
  maxw += BorderWidth;
  maxh += BorderHeight;

  // Set width of preceeding items
  SetItemWidth (colstart, colcount, colw);

  if (IsMenuBar ())
  {
    if (lastcol)
    {
      // If this is a menu bar, move items starting from lastcol to right margin
      cur = (csComponent *)first;
      maxw = bound.Width () - BorderWidth;
      do
      {
        cur = (csComponent *)cur->prev;
        cur->SetRect (maxw - (cur->bound.xmax - cur->bound.xmin),
          cur->bound.ymin, maxw, cur->bound.ymax);
        maxw -= cur->bound.Width ();
      } while (cur != lastcol); /* enddo */
    } /* endif */
  } else
    SetRect (bound.xmin, bound.ymin, bound.xmin + maxw, bound.ymin + maxh);
}

void csMenu::SetState (int mask, bool enable)
{
  int oldstate = state;
  csComponent *oldfocused = parent->focused;

  csComponent::SetState (mask, enable);
  if (((oldstate ^ state) & CSS_FOCUSED) && !enable
   && (MenuStyle & CSMS_HIDEINACTIVE))
    Hide ();

  if ((oldstate ^ state) & (CSS_FOCUSED | CSS_VISIBLE))
  {
    if ((mask & (CSS_FOCUSED | CSS_VISIBLE))
     && enable
     && (oldfocused != this))
      oldparentfocus = oldfocused;
    if (current)
      current->Invalidate ();
  } /* endif */
}

bool csMenu::SetRect (int xmin, int ymin, int xmax, int ymax)
{
  // If fPlaceItems is true, adjust our bounds
  if (IsMenuBar ())
    fPlaceItems = true;
  else if (fPlaceItems)
  {
    PlaceItems ();
    if (!IsMenuBar ())
    {
      xmax = xmin + bound.Width ();
      ymax = ymin + bound.Height ();
    }
  }

  return csComponent::SetRect (xmin, ymin, xmax, ymax);
}

struct finditem_rec
{
  int id;
  csComponent *result;
};

static bool do_finditem (csComponent *child, void *param)
{
  csComponent *res = (csComponent *)child->SendCommand (cscmdMenuItemFindId,
    (void *)((finditem_rec *)param)->id);
  if (res)
  {
    ((finditem_rec *)param)->result = res;
    return true;
  }

  return false;
}

csComponent *csMenu::GetItem (int iCommandCode)
{
  finditem_rec fr;
  fr.id = iCommandCode;
  fr.result = NULL;
  ForEach (do_finditem, (void *)&fr);
  return fr.result;
}

void csMenu::SetCheck (int iCommandCode, bool iState)
{
  csComponent *item = GetItem (iCommandCode);
  if (item)
    item->SendCommand (cscmdMenuItemCheck, (void *)iState);
}

void csMenu::Insert (csComponent *comp)
{
  fPlaceItems = true;
  csComponent::Insert (comp);
}

void csMenu::Delete (csComponent *comp)
{
  fPlaceItems = true;
  csComponent::Delete (comp);
}

void csMenu::SuggestSize (int &w, int &h)
{
  if (fPlaceItems)
    PlaceItems ();

  w = h = 0;
  csComponent *cur = first;
  do
  {
    int maxw = cur->bound.xmax;
    int maxh = cur->bound.ymax;
    if (w < maxw) w = maxw;
    if (h < maxh) h = maxh;
    cur = cur->next;
  } while (cur != first);

  switch (FrameStyle)
  {
    case csmfsNone:
      break;
    case csmfsThin:
      w += 1; h += 1;
      break;
    case csmfsBar:
      h += 2;
      break;
    case csmfs3D:
      w += 2; h += 2;
      break;
  } /* endswitch */
}
