/*
    Copyright (C) 2000 by Michael Dale Long

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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "cssysdef.h"
#include "iconsole.h"
#include "igraph2d.h"
#include "igraph3d.h"
#include "isystem.h"
#include "itxtmgr.h"
#include "cssys/csevent.h"
#include "csutil/csrect.h"
#include "csutil/scf.h"
#include "csutil/csstring.h"
#include "cscon.h"
#include "conbuffr.h"

IMPLEMENT_IBASE(csConsole)
  IMPLEMENTS_INTERFACE(iConsole)
  IMPLEMENTS_INTERFACE(iPlugIn)
IMPLEMENT_IBASE_END

IMPLEMENT_FACTORY (csConsole)
DECLARE_FACTORY (funConsole)

EXPORT_CLASS_TABLE (cscon)
  EXPORT_CLASS (csConsole, "crystalspace.console.output.standard",
		"Standard Console Plugin")
  EXPORT_CLASS (funConsole, "crystalspace.console.output.funky",
		"Funky Console Plugin")
EXPORT_CLASS_TABLE_END

csConsole::csConsole (iBase *base)
{
  CONSTRUCT_IBASE (base);
  fg_rgb.Set (255, 255, 255);	// Foreground defaults to white
  bg_rgb.Set (0, 0, 0);		// Background defaults to black
  transparent = false;		// Default to no transparency
  do_snap = true;		// Default to snapping
  // Initialize the cursor state variables
  cursor = csConNoCursor;
  cx = cy = 0;
  flash_interval = 500;
  cursor_visible = true;
  clear_input = false;
  auto_update = true;
  system_ready = false;
  visible = true;
  Client = NULL;
  G3D = NULL;
  G2D = NULL;
  System = NULL;
}

csConsole::~csConsole ()
{
  if (System)
    System->DecRef ();
  if (G2D)
    G2D->DecRef ();
  if (G3D)
    G3D->DecRef ();    
  delete buffer;
}

bool csConsole::Initialize(iSystem *system)
{
  (System = system)->IncRef ();
  G3D = QUERY_PLUGIN_ID (System, CS_FUNCID_VIDEO, iGraphics3D);
  if (!G3D) return false;
  G2D = G3D->GetDriver2D ();
  G2D->IncRef ();
  
  // Initialize the display rectangle to the entire display
  size.Set (0, 0, G2D->GetWidth () - 1, G2D->GetHeight () - 1);
  invalid.Set (size); // Invalidate the entire console
  font = G2D->GetFontID ();
  // Create the backbuffer (4096 lines max)
  buffer = new csConsoleBuffer (4096, (size.Height() / (G2D->GetTextHeight (font) + 2)));
  // Initialize flash_time for flashing cursors
  flash_time = System->GetTime ();

  // We want to see broadcast events
  System->CallOnEvents (this, CSMASK_Broadcast);

  return true;
}

void csConsole::Clear (bool wipe)
{
  if (wipe)
    // Clear the buffer 
    buffer->Clear ();
  else
    // Put the top of the buffer on the current line
    buffer->SetTopLine (buffer->GetCurLine ());

  // Redraw the (now blank) console
  invalid.Set (size);
  // Reset the cursor position
  cx = cy = 0;
  clear_input = false;
}

void csConsole::SetBufferSize (int lines)
{
  buffer->SetLength (lines);
}

void csConsole::PutText (int /*iMode*/, const char *text)
{
  int i;
  csString *curline = NULL;

  // Scan the string for escape characters
  for (i = 0; text && text [i] != 0; i++)
  {
    if (clear_input)
    {
      curline = buffer->WriteLine ();
      curline->Clear ();
      clear_input = false;
      cx = 0;
    }

    switch (text [i])
    {
      case '\r':
        clear_input = true;
        break;
      case '\n':
        buffer->NewLine (do_snap);
        // Update the cursor Y position
        cx = 0;
        if (do_snap)
          cy = buffer->GetCurLine () - buffer->GetTopLine ();
        else
          ++cy < buffer->GetPageSize () ? cy : cy--;
        // Make sure we don't change the X position below
        curline = NULL;
        break;
      case '\b':
        if (cx > 0)
        {
          if (cx == 1)
          {
            cx = 0;
            buffer->DeleteLine (cy);
            curline = NULL;
          }
          else
          {
            // Delete the character before the cursor, and move the cursor back
            curline = buffer->WriteLine ();
            curline->DeleteAt (--cx);
          }
        }
        else if (cy > 0)
        {
          // Backup a line
          cy--;
          buffer->SetCurLine (cy);
          curline = buffer->WriteLine ();
        }
        break;
      case '\t':
        // Print 4-space tabs
        curline = buffer->WriteLine ();
        curline->Append ("    ");
        cx += 4;
        break;
      default:
        curline = buffer->WriteLine ();
        if (cx == (int)curline->Length ())
          curline->Append (text [i]);
        else
          curline->Insert (cx, text [i]);
        cx++;
        break;
      }
  }

  if (auto_update && system_ready && G2D->BeginDraw ())
  {
    csRect rect;
    G2D->Clear (bg);
    Draw2D (&rect);
    G2D->FinishDraw ();
    G2D->Print (&rect);
  }
}

const char *csConsole::GetLine (int line) const
{
  return buffer->GetLine ((line == -1) ?
    (buffer->GetCurLine () - buffer->GetTopLine ()) : line)->GetData ();
}

void csConsole::DeleteText (int start, int end)
{
  csString *text = buffer->WriteLine ();
  int length = text->Length ();

  // Avoid invalid start points
  if (start > length)
    return;

  // Make sure we don't go past the end of the string
  if ((end == -1) || (end >= length))
  {
    text->DeleteAt (start, length - start);
    cx = text->Length ();
  }
  else
  {
    text->DeleteAt (start, end - start);
    cx -= end - start;
  }
}

void csConsole::Draw2D (csRect *area)
{
  if (!visible) return;

  int i, height, oldfont;
  csRect line, oldrgn;
  const csString *text;
  bool dirty;

  invalid.Union (size);

  // Save old clipping region
  G2D->GetClipRect (oldrgn.xmin, oldrgn.ymin, oldrgn.xmax, oldrgn.ymax);
  G2D->SetClipRect (invalid.xmin, invalid.ymin, invalid.xmax, invalid.ymax);

  // Save the old font and set it to the requested font
  oldfont = G2D->GetFontID ();
  G2D->SetFontID (font);

  // Calculate the height of the text
  height = G2D->GetTextHeight (font) + 2;

  // Draw the background
  if (!transparent)
    G2D->DrawBox (size.xmin, size.ymin, size.xmax, size.ymax, bg);

  // Make sure we redraw everything we need to
  if (area)
    area->Union (invalid);

  // Print all lines on the current page
  for (i = 0; i < buffer->GetPageSize (); i++)
  {
    // Retrieve the line from the buffer and it's dirty flag
    text = buffer->GetLine (i, &dirty);

    // A NULL line indicates it's the last printed line on the page
    if (text == NULL)
      break;

    // Calculate the rectangle of this line
    line.Set (size.xmin, (i * height) + size.ymin,
      size.xmax, (i * height) + size.ymin + height);

    // See if the line changed or if the line intersects with the invalid area
    if (area && (dirty || line.Intersects (invalid)))
      area->Union (line);

    // Write the line
    G2D->Write (1 + size.xmin, (i * height) + size.ymin, fg, -1, text->GetData ());
  }

  // Test for a change in the flash state
  if (flash_interval > 0)
  {
    cs_time cur_time = System->GetTime ();
    if (cur_time > flash_time + flash_interval || cur_time < flash_time)
    {
      cursor_visible = !cursor_visible;
      flash_time = cur_time;
    }
  }
  else
    cursor_visible = true;

  // See if we draw a cursor
  if ((cursor != csConNoCursor) && cursor_visible && (cy != -1))
  {
    int cx_pix, cy_pix;

    // Get the line of text that the cursor is on
    text = buffer->GetLine (cy);

    if (text == NULL)
    {
#ifdef CS_DEBUG
      if (cx != 0)
	System->Printf (MSG_WARNING, "csConsole:  Current line is empty but cursor x != 0!\n");
#endif // CS_DEBUG
      cx_pix = 1;
    }
    else
    {
      // Make a copy of the text
      csString curText (*text);
      curText.SetCapacity (cx);
      curText.SetAt (cx, '\0');
      cx_pix = G2D->GetTextWidth (font, curText.GetData ());
    }
    cy_pix = (cy * height) + size.ymin;
    cx_pix += size.xmin;

    line.Set (cx_pix, cy_pix, cx_pix + G2D->GetTextWidth(font, "_"), cy_pix + height);

    // Draw the appropriate cursor
    switch (cursor)
    {
      case csConInsertCursor:
        G2D->DrawLine (cx_pix + 1, cy_pix + (height-3), line.xmax, cy_pix + (height-3), fg);
        break;
      case csConNormalCursor:
        G2D->DrawBox (cx_pix + 1, cy_pix + 1, line.xmax - 1, cy_pix + (height - 1), fg);
        break;
#ifdef CS_DEBUG
      default:
        System->Printf(MSG_WARNING, "csConsole:  Invalid cursor setting!\n");
#endif // CS_DEBUG
    }
  }

  // Restore the original clipping region
  G2D->SetClipRect (oldrgn.xmin, oldrgn.ymin, oldrgn.xmax, oldrgn.ymax);

  // No more invalid area
  invalid.MakeEmpty ();

  // Restore the original font
  G2D->SetFontID (oldfont);
}

void csConsole::CacheColors ()
{
  // Update the colors from the texture manager
  iTextureManager *txtmgr = G3D->GetTextureManager ();
  fg = txtmgr->FindRGB (fg_rgb.red, fg_rgb.green, fg_rgb.blue);
  bg = txtmgr->FindRGB (bg_rgb.red, bg_rgb.green, bg_rgb.blue);
}

void csConsole::GetPosition(int &x, int &y, int &width, int &height) const
{
  x = size.xmin;
  y = size.ymin;
  width = size.Width();
  height = size.Height();
}

void csConsole::SetPosition(int x, int y, int width, int height)
{
  if (x >= 0)
    size.xmin = x;
  if (y >= 0)
    size.ymin = y;
  if (width >= 0)
    size.xmax = size.xmin + width;
  if (height >= 0)
    size.ymax = size.ymin + height;

  // Make sure we don't go off the current screen
  if (size.xmax >= G2D->GetWidth ())
    size.xmax = G2D->GetWidth () - 1;
  if (size.ymax >= G2D->GetHeight ())
    size.ymax = G2D->GetHeight () - 1;
  
  // Calculate the number of lines on the console
  buffer->SetPageSize (size.Height () / (G2D->GetTextHeight (font) + 2));

  // Invalidate the entire new area of the console
  invalid.Set (size); 

  // Update cursor coordinates
  cy = MIN (cy, buffer->GetPageSize ());
  // now check how many chars do fit in the current width
  const csString *text = buffer->GetLine (cy);
  if (!text)
    cx = 0;
  else
  {
    csString curText (*text);
    curText.SetCapacity (cx);
    curText.SetAt (cx, '\0');
    while (cx && G2D->GetTextWidth (font, curText.GetData ()) > size.Width ())
    {
      curText.SetCapacity (--cx);
      curText.SetAt (cx, '\0');
    }
  }
}

void csConsole::Invalidate (csRect &area)
{
  // Make sure we only update within our rectangle, otherwise 2D driver may crash!
  csRect console (size);
  console.Intersect (area);
  if (!console.IsEmpty ())
    invalid.Union (console);
}

int csConsole::GetFontID () const
{
  return font;
}

void csConsole::SetFontID (int FontID)
{
  font = FontID;

  // Calculate the number of lines on the console with the new font
  buffer->SetPageSize (size.Height () / (G2D->GetTextHeight (font) + 2));
}

int csConsole::GetTopLine () const
{
  return buffer->GetTopLine ();
}

void csConsole::ScrollTo(int top, bool snap)
{
  switch (top)
  {
    case csConPageUp:
      buffer->SetTopLine (MAX (0, buffer->GetTopLine () - buffer->GetPageSize ()));
      break;
    case csConPageDown:
      buffer->SetTopLine (buffer->GetTopLine () + buffer->GetPageSize ());
      break;
    case csConVeryTop:
      buffer->SetTopLine (0);
      break;
    case csConVeryBottom:
      buffer->SetTopLine (buffer->GetCurLine () - buffer->GetPageSize () + 1);
      break;
    default:
      buffer->SetTopLine (top);
      break;
  }

  if ((buffer->GetCurLine () >= buffer->GetTopLine ())
   && (buffer->GetCurLine () <= buffer->GetTopLine () + buffer->GetPageSize ()))
    cy = MAX (buffer->GetCurLine () - buffer->GetTopLine (), 0);
  else
    cy = -1;
  do_snap = snap;
}

void csConsole::GetCursorPos(int &x, int &y) const
{
  x = cx;
  y = cy;
}

void csConsole::SetCursorPos(int x, int y)
{
  int max_x, max_y = buffer->GetPageSize ();
  const csString *curline = buffer->GetLine (cy);

  if (curline)
    max_x = curline->Length ();
  else
    max_x = 0;

  // Keep the cursor from going past the end of the line
  if (x > max_x)
    cx = max_x - 1;
  else
    cx = x;
  
  // Keep it from going off the bottom of the display
  if (y > max_y)
    cy = max_y - 1;
  else
    cy = y;
}

void csConsole::SetCursorPos (int iCharNo)
{
  if (cy>-1)
  {
    int max_x;
    const csString *curline = buffer->GetLine (cy);
 
    max_x = curline ? curline->Length () : 0;
    cx = (iCharNo > max_x) ? max_x : (iCharNo <= 0) ? 0 : iCharNo;
  }    
}

void csConsole::SetVisible (bool iShow)
{
  visible = iShow;
  if (Client)
  {
    csEvent e (System->GetTime (), csevBroadcast, cscmdConsoleStatusChange, this);
    Client->HandleEvent (e);
  }
  invalid.Set (size);
}

bool csConsole::ConsoleExtension (const char *iCommand, ...)
{
  va_list args;
  va_start (args, iCommand);

  bool rc = true;
  if (!strcmp (iCommand, "FlashTime"))
    flash_interval = va_arg (args, cs_time);
  else if (!strcmp (iCommand, "GetPos"))
  {
    int *x = va_arg (args, int *);
    int *y = va_arg (args, int *);
    int *w = va_arg (args, int *);
    int *h = va_arg (args, int *);
    GetPosition (*x, *y, *w, *h);
  }
  else if (!strcmp (iCommand, "SetPos"))
  {
    int x = va_arg (args, int);
    int y = va_arg (args, int);
    int w = va_arg (args, int);
    int h = va_arg (args, int);
    SetPosition (x, y, w, h);
  }
  else
    rc = false;

  va_end (args);
  return rc;
}

bool csConsole::HandleEvent (iEvent &Event)
{
  switch (Event.Type)
  {
    case csevBroadcast:
      switch (Event.Command.Code)
      {
        case cscmdSystemOpen:
          system_ready = true;
          CacheColors ();
          return true;
        case cscmdSystemClose:
          system_ready = false;
          return true;
        case cscmdPaletteChanged:
          CacheColors ();
          break;
      }
      break;
  }
  return false;
}
