/*
    Copyright (C) 1999 by Andrew Zabolotny <bit@eltech.ru>

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

#define SYSDEF_PATH
#include "sysdef.h"
#include "cssys/system.h"
#include "csws/csws.h"

#if defined(OS_NEXT)
// This is an ugly solution for an ugly problem.  On NeXT platforms, libraries
// need to be linked into the application to satisfy symbolic linkages required
// by plug-ins which are loaded dynamically, thus we need to force the linker
// to link in symbols required by all plug-ins which this application loads.
// In this case, the software renderer needs operator*() and csPolygonClipper.
#include "csgeom/polyclip.h"
#include "csgeom/transfrm.h"
void* csWsForceLink()
{
  csVector3 v; csTransform t;
  v = v * t; // operator*()
  return new csPolygonClipper(0);
}
#endif

class csWsTest : public csApp
{
public:
  /// Initialize maze editor
  csWsTest (char *AppTitle);

  ///
  virtual bool HandleEvent (csEvent &Event);

  virtual bool InitialSetup (int argc, const char* const argv[],
    const char *ConfigName, const char* DataDir);
};

csWsTest *app;                        // The main Windowing System object

//-----------------------------------------------------------------------------

// Scroll bar class default palette
static int palette_csWsTest[] =
{
  cs_Color_Gray_D,			// Application workspace
  cs_Color_Green_L,			// End points
  cs_Color_Red_L,			// lines
  cs_Color_White			// Start points
};

csWsTest::csWsTest (char *AppTitle) : csApp (AppTitle)
{
  SetPalette (palette_csWsTest, sizeof (palette_csWsTest) / sizeof (int));
}

bool csWsTest::InitialSetup (int argc, const char* const argv[],
  const char *ConfigName, const char* DataDir)
{
  if (!csApp::InitialSetup (argc, argv, ConfigName, DataDir))
    return false;

  printf (MSG_INITIALIZATION, "Crystal Space Windowing System test version %s (%s).\n", VERSION, RELEASE_DATE);
  printf (MSG_INITIALIZATION, "Created by Andrew Zabolotny and others...\n\n");

  // create a window
  csComponent *window = new csWindow (this, "-- Drag me --",
    CSWS_DEFAULTVALUE | CSWS_TOOLBAR | CSWS_CLIENTBORDER);

  csMenu *menu = (csMenu *)window->GetChild (CSWID_MENUBAR);
  if (menu)
  {
    menu->SetFont (csFontCourier);
    csMenu *submenu;

    submenu = new csMenu (NULL);
    (void)new csMenuItem (menu, "~File", submenu);
      (void)new csMenuItem (submenu, "~Open\tCtrl+O", cscmdNothing);
      (void)new csMenuItem (submenu, "~Save\tCtrl+S", cscmdNothing);
      (void)new csMenuItem (submenu, "~Close", cscmdNothing);
      (void)new csMenuItem (submenu);
      (void)new csMenuItem (submenu, "~Quit\tCtrl+Q", cscmdQuit);

    submenu = new csMenu (NULL);
    (void)new csMenuItem (menu, "~Edit", submenu);
      (void)new csMenuItem (submenu, "~Undo\tAlt+BackSpace", cscmdNothing);
      (void)new csMenuItem (submenu);
      (void)new csMenuItem (submenu, "~Copy\tCtrl+Ins", cscmdNothing);
      (void)new csMenuItem (submenu, "Cu~t\tShift+Del", cscmdNothing);
      (void)new csMenuItem (submenu, "~Paste\tShift+Ins", cscmdNothing);
      (void)new csMenuItem (submenu);
      (void)new csMenuItem (submenu, "~Select all\tCtrl+/", cscmdNothing);
      (void)new csMenuItem (submenu, "~Deselect all\tCtrl+\\", cscmdNothing);

    submenu = new csMenu (NULL);
    (void)new csMenuItem (menu, "~Modify", submenu);
     (void)new csMenuItem (submenu, "~Scale", cscmdNothing);
      (void)new csMenuItem (submenu, "~Rotate", cscmdNothing);
      (void)new csMenuItem (submenu, "~Move", cscmdNothing);

    submenu = new csMenu (NULL);
    (void)new csMenuItem (menu, "~Test submenu", submenu);
      (void)new csMenuItem (submenu, "~One", cscmdNothing, CSMIS_DEFAULTVALUE | CSMIS_CHECKED);
      (void)new csMenuItem (submenu, "~Two", cscmdNothing);
      (void)new csMenuItem (submenu, "T~hree", cscmdNothing);
      csMenu *subsubmenu = new csMenu (NULL);
      (void)new csMenuItem (submenu, "~Four", subsubmenu);
        (void)new csMenuItem (subsubmenu, "Four / ~One", cscmdNothing);
        (void)new csMenuItem (subsubmenu, "Four / ~Two", cscmdNothing);
        (void)new csMenuItem (subsubmenu, "Four / T~hree", cscmdNothing);
      subsubmenu = new csMenu (NULL);
      (void)new csMenuItem (submenu, "Fi~ve", subsubmenu, CSMIS_NEWCOLUMN);
        (void)new csMenuItem (subsubmenu, "Five / ~One", cscmdNothing);
        (void)new csMenuItem (subsubmenu, "Five / ~Two", cscmdNothing);
        (void)new csMenuItem (subsubmenu, "Five / T~hree", cscmdNothing);
        (void)new csMenuItem (subsubmenu);
        (void)new csMenuItem (subsubmenu, "~Maximize", cscmdMaximize, CSMIS_DEFAULTVALUE | CSMIS_NOCLOSE);
        (void)new csMenuItem (subsubmenu, "~Hide", cscmdHide);
        (void)new csMenuItem (subsubmenu, "~Close", cscmdClose);
        (void)new csMenuItem (subsubmenu);
        (void)new csMenuItem (subsubmenu, "~Quit\tCtrl+Q", cscmdQuit);
        subsubmenu->GetChild (cscmdHide)->SendCommand (cscmdMenuItemCheck, (void *)true);
        subsubmenu->GetChild (cscmdQuit)->SendCommand (cscmdMenuItemCheck, (void *)true);
      (void)new csMenuItem (submenu, "~Six", cscmdNothing);
      (void)new csMenuItem (submenu, "~Seven", cscmdNothing);

    (void)new csMenuItem (menu, "~NoSubmenu");

    (void)new csMenuItem (menu, CSMIS_NEWCOLUMN);

    submenu = new csMenu (NULL);
    (void)new csMenuItem (menu, "~Help", submenu);
      (void)new csMenuItem (submenu, "~Index", cscmdNothing);
      (void)new csMenuItem (submenu, "~General", cscmdNothing);
      (void)new csMenuItem (submenu, "~Context\tF1", cscmdNothing);
      (void)new csMenuItem (submenu);
      (void)new csMenuItem (submenu, "~About", cscmdNothing);
      submenu->GetChild (cscmdNothing)->SendCommand (cscmdDeactivateMenu, (void *)false);
  }
  csDialog *toolbar = (csDialog *)window->GetChild (CSWID_TOOLBAR);
  if (toolbar)
  {
    csNewToolbarButton (toolbar, cscmdNothing, "hello");
    csNewToolbarButton (toolbar, cscmdNothing, "hello again");
    csNewToolbarButton (toolbar, cscmdNothing, "test");
    csNewToolbarButton (toolbar, cscmdNothing, "another test");
    csNewToolbarButton (toolbar, cscmdNothing, "yet another test");
  }
  window->SetRect (80, 20, 520, 340);

  csComponent *client = new csDialog (window);

  {
    csButton *but = new csButton (client, cscmdQuit);
    but->SetText ("~Quit!"); but->SetRect (20, 20, 90, 40);
    but->SetState (CSS_GROUP, true);
    csStatic *stat = new csStatic (client, but, "Test ~Label", csscsFrameLabel);
    stat->SetRect (10, 10, 420, 110);
    but = new csButton (client, cscmdNothing, CSBS_DEFAULTVALUE | CSBS_DEFAULT);
    but->SetText ("~Another one"); but->SetRect (50, 80, 180, 100);

    but = new csButton (client, cscmdNothing, CSBS_DEFAULTVALUE, csbfsThinRect);
    but->SetText ("hmm~..."); but->SetRect (20, 130, 100, 144);
    but->SetState (CSS_GROUP, true);
    stat = new csStatic (client, but, "Another ~Group", csscsFrameLabel);
    stat->SetRect (10, 110, 420, 270);
    but = new csButton (client, cscmdNothing, CSBS_DEFAULTVALUE, csbfsThickRect);
    but->SetText ("~whoops!"); but->SetRect (120, 130, 200, 144);
    but = new csButton (client, cscmdNothing, CSBS_DEFAULTVALUE, csbfsThinRect);
    but->SetText ("~crash!"); but->SetRect (220, 130, 300, 144);

    csInputLine *il = new csInputLine (client, 40, csifsThinRect);
    il->SetRect (100, 200, 300, 216);
    il->SetText ("input line test: check it out!");
    il = new csInputLine (client, 10, csifsThickRect);
    il->SetRect (100, 220, 300, 236);
    il->SetText ("another input line");
    il->SetFont (csFontCourier); il->SetSelection (0, 999);

    csListBox *lb = new csListBox (client, CSLBS_HSCROLL | CSLBS_VSCROLL, cslfsThinRect);
    lb->SetRect (320, 120, 410, 250);
    lb->SetFont (csFontCourier);
    for (int i = 1; i < 100; i++)
    {
      char tmp[20];
      sprintf (tmp, "item %d - dummy", i);
      (void)new csListBoxItem (lb, tmp, i);
    }
  }

  window = new csWindow (this, "SetState (CSS_TOPSELECT, false)",
    CSWS_DEFAULTVALUE, cswfsThin);
  window->SetState (CSS_TOPSELECT, false);
  window->SetRect (20, 20, 400, 80);

  {
    csButton *but = new csButton (this, 9999);
    but->SetText ("Hello"); but->SetRect (10, 10, 100, 30);
    but->SetFont (csFontTiny);
    but = new csButton (this, 9998);
    but->SetText ("Othello"); but->SetRect (210, 10, 300, 30);
  }

  return true;
}

bool csWsTest::HandleEvent (csEvent &Event)
{
  static csMouseCursorID mousecursors [] =
  {
    csmcNone, csmcArrow, csmcLens, csmcCross, csmcPen, csmcMove,
    csmcSizeNWSE, csmcSizeNESW, csmcSizeNS, csmcSizeEW, csmcStop, csmcWait
  };

  static int mousecursor = 1;
  static int px, py;
  static bool draw = false;

  if (csApp::HandleEvent (Event))
    return true;

  switch (Event.Type)
  {
    case csevCommand:
      switch (Event.Command.Code)
      {
        case 9999:
          {
            csWindow *d = csFileDialog (this, "test file dialog");
            if (d)
            {
              if (Execute (d) == cscmdCancel)
              {
                delete d;
                return true;
              }
              char filename [MAXPATHLEN + 1];
              csQueryFileDialog (d, filename, sizeof (filename));
              delete d;
              csMessageBox (app, "Result", filename);
            }
            return true;
          }
        case 9998:
          {
            csWindow *d = csColorDialog (this, "test color dialog");
            if (d)
            {
              if (Execute (d) == cscmdCancel)
              {
                delete d;
                return true;
              }
              int color;
              csQueryColorDialog (d, color);
              delete d;
              char buff [100];
              sprintf (buff, "color value: %08X\n", color);
              csMessageBox (app, "Result", buff);
            }
            return true;
          }
      }
      break;

    case csevMouseMove:
      SetMouse (mousecursors [mousecursor]);
      if (draw)
      {
        // kludge: set dirty rectangle so that line won't get clipped
        csRect old (dirty);
        dirty.Set (px, py, Event.Mouse.x, Event.Mouse.y);
        dirty.Normalize ();
        dirty.xmax++; dirty.ymax++;
        Line (px, py, Event.Mouse.x, Event.Mouse.y, 2);
        dirty.Set (old);

        px = Event.Mouse.x;
        py = Event.Mouse.y;
      } /* endif */
      return true;

    case csevMouseDown:
    case csevMouseDoubleClick:
      if (Event.Mouse.Button == 1)
      {
        draw = true;
        px = Event.Mouse.x;
        py = Event.Mouse.y;

        // kludge: set dirty rectangle so that line won't get clipped
        csRect old (dirty);
        dirty.Set (px - 1, py - 1, px + 2, py + 2);
        Box (px - 1, py - 1, px + 2, py + 2, 3);
        dirty.Set (old);
      }
      else if (Event.Mouse.Button == 2)
      {
        mousecursor = (mousecursor + 1) % (sizeof (mousecursors) / sizeof (int));
        SetMouse (mousecursors [mousecursor]);
      } /* endif */
      return true;

    case csevMouseUp:
    {
      // kludge: set dirty rectangle so that line won't get clipped
      csRect old (dirty);
      dirty.Set (px - 1, py - 1, px + 2, py + 2);
      Box (px - 1, py - 1, px + 2, py + 2, 1);
      dirty.Set (old);

      draw = false;
      return true;
    }
  } /* endswitch */

  return false;
}

//---------------------------------------------------------------------------

/*
 * Main function
 */
int main (int argc, char* argv[])
{
  app = new csWsTest ("Crystal Space 3D maze editor");

  if (app->InitialSetup (argc, argv, "MazeD.cfg", "/lib/MazeD"))
    app->Loop ();

  delete app;

  return (0);
}
