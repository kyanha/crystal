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

#ifndef __CS_CSCON_H__
#define __CS_CSCON_H__

#include "iconsole.h"
#include "csutil/scf.h"
#include "csutil/csrect.h"

struct iGraphics2D;
struct iTextureManager;
struct iCursor;
class csConsoleBuffer;

class csConsole : public iConsole
{
public:
  DECLARE_IBASE;
  csConsole(iBase *base);
  virtual ~csConsole();
  virtual bool Initialize(iSystem *);
  /***DEPRECATED***/
  virtual void Show();
  /***DEPRECATED***/
  virtual void Hide();
  /***DEPRECATED***/
  virtual bool IsActive() const;
  virtual void Clear(bool wipe = false);
  virtual void PutText(const char *text);
  virtual const csString *GetText(int line = -1) const;
  virtual void DeleteText(int start = 0, int end = -1);
  virtual void Draw(csRect *rect);
  virtual void SetBufferSize(int lines);
  virtual void CacheColors(iTextureManager *txtmgr);
  virtual void GetForeground(int &red, int &green, int &blue) const;
  virtual void SetForeground(int red, int green, int blue);
  virtual void GetBackground(int &red, int &green, int &blue) const;
  virtual void SetBackground(int red, int green, int blue);
  virtual void GetPosition(int &x, int &y, int &width, int &height) const;
  virtual void SetPosition(int x, int y, int width = -1, int height = -1);
  virtual void Invalidate(csRect &area);
  virtual bool GetTransparency() const;
  virtual void SetTransparency(bool trans);
  virtual int GetFontID() const;
  virtual void SetFontID(int FontID);
  virtual int GetTopLine() const;
  virtual void ScrollTo(int topline, bool snap = true);
  virtual void GetCursorPos(int &x, int &y) const;
  virtual void SetCursorPos(int x, int y);
  virtual int GetCursorStyle(bool &flashing, iCursor **custom = NULL) const;
  virtual void SetCursorStyle(int style, bool flashing = true, iCursor *custom = NULL);

protected:
  csConsoleBuffer *buffer;
  bool transparent, do_snap, flash_cursor, do_flash;
  iGraphics2D *piG2D;
  iSystem *piSystem;
  csRect size, invalid;
  int font, cursor, cx, cy;
  iCursor *custom_cursor;
  int flash_time;

  typedef struct {
    int red;
    int green;
    int blue;
  } Color;

  //  Foreground and background colors
  Color fg_rgb;
  Color bg_rgb;
  // The texture manager codes for the colors
  int fg, bg;

};

#endif // ! __CS_CSCON_H__
