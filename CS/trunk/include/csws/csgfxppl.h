/*
    Crystal Space Windowing System: Graphics Pipeline class
    Copyright (C) 1998,1999 by Andrew Zabolotny <bit@eltech.ru>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the 9License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __CSGFXPPL_H__
#define __CSGFXPPL_H__

#include "csutil/csbase.h"
#include "csutil/csrect.h"
#include "csengine/cspixmap.h"
#include "igraph3d.h"

struct iGraphics2D;
struct iSystem;

// Maximal number of primitives which can be drawn IN ONE FRAME
#define MAX_CSWS_PIPELINE_LENGTH 16384
// Maximal number of video pages to sync image
#define MAX_SYNC_PAGES 8

/**
 * Graphics System pipeline class<p>
 * This class implements all drawing operations (which are then passed to
 * System->G2D object). All drawing operations are put on hold until
 * Flush() is called. This routine is usually called once per frame.<p>
 * All methods and variables of this class are private. Only csApp should
 * have access to its internals, all graphics pipeline management is done
 * through main application object.
 */
class csGraphicsPipeline : public csBase
{
private:
  /// Only csApp can manipulate the graphics pipeline
  friend class csApp;
  /// Graphics pipeline operations
  enum
  {
    pipeopNOP,
    pipeopBOX,
    pipeopLINE,
    pipeopPIXEL,
    pipeopTEXT,
    pipeopSPR2D,
    pipeopSAVAREA,
    pipeopRESAREA,
    pipeopSETCLIP,
    pipeopRESCLIP,
    pipeopPOLY3D
  };
  /**
   * Graphics pipeline entry.
   * WARNING! Keep the size of this structure AS LOW AS POSSIBLE.
   * The gfx pipeline is an static array that contains VERY many
   * elements of this type.
   */
  typedef struct
  {
    unsigned char Op;
    unsigned char font;	// this is a part of Text struct (below)
    union
    {
      struct
      {
        int xmin,ymin,xmax,ymax;
        int color;
      } Box;
      struct
      {
        float x1,y1,x2,y2;
        int color;
      } Line;
      struct
      {
        int x,y,color;
      } Pixel;
      struct
      {
        int x,y,fg,bg;
        char *string;
      } Text;
      struct
      {
        csPixmap *s2d;
        int x, y, w, h;
      } Spr2D;
      struct
      {
        csImageArea **Area;
        int x, y, w, h;
      } SavArea;
      struct
      {
        csImageArea *Area;
        bool Free;
      } ResArea;
      struct
      {
        int xmin, ymin, xmax, ymax;
      } ClipRect;
      struct
      {
        void *poly;	/* A pointer to packed G3DPolygonDPFX structure */
        int mode;
      } Poly3D;
    };
  } csPipeEntry;

  int pipelen;
  csPipeEntry pipeline [MAX_CSWS_PIPELINE_LENGTH];
  // Used to propagate changes to all pages
  csImageArea *SyncArea [MAX_SYNC_PAGES];
  csRect SyncRect [MAX_SYNC_PAGES];
  csRect RefreshRect;
  int MaxPage;
  int CurPage;

  // Frame width and height
  int FrameWidth, FrameHeight;

  // The 2D graphics driver
  iGraphics2D *G2D;
  // The 3D graphics driver
  iGraphics3D *G3D;

  // Used to clear screen
  int ClearPage,ClearColor;

  /// Initialize pipeline
  csGraphicsPipeline (iSystem *System);
  /// Deinitialize pipeline
  virtual ~csGraphicsPipeline ();

  /// Synchronise image on this page with previous pages
  void Sync (int CurPage, int &xmin, int &ymin, int &xmax, int &ymax);

  /// Drop all synchronization rectangles
  void Desync ();

  /// Flush graphics pipeline
  void Flush (int iCurPage);

  /// Draw a box
  void Box (int xmin, int ymin, int xmax, int ymax, int color);

  /// Draw a line
  void Line (float x1, float y1, float x2, float y2, int color);

  /// Draw a pixel
  void Pixel (int x, int y, int color);

  /// Draw a text string: if bg < 0 background is not drawn
  void Text (int x, int y, int fg, int bg, int font, const char *s);

  /// Draw a 2D sprite
  void Sprite2D (csPixmap *s2d, int x, int y, int w, int h);

  /// Save a part of screen
  void SaveArea (csImageArea **Area, int x, int y, int w, int h);

  /// Restore a part of screen
  void RestoreArea (csImageArea *Area, bool Free);

  /// Free buffer used to keep an area of screen
  void FreeArea (csImageArea *Area);

  /// Clear screen with specified color
  void Clear (int color);

  /// Set clipping rectangle: SHOULD CALL RestoreClipRect() AFTER DRAWING!
  void SetClipRect (int xmin, int ymin, int xmax, int ymax);

  /// Restore clipping rectangle to (0, 0, ScreenW, ScreenH);
  void RestoreClipRect();

  /// Draw a 3D polygon using DrawPolygonFX
  void Polygon3D (G3DPolygonDPFX &poly, UInt mode);

  /// Allocate a new slot in graphics pipeline
  csPipeEntry *AllocOp(int pipeOp)
  {
    if (pipelen < MAX_CSWS_PIPELINE_LENGTH)
    {
      pipeline[pipelen].Op = pipeOp;
      return &pipeline[pipelen++];
    }
    else
      return NULL;
  }

  /// Clip a line against a rectangle and return true if its clipped out
  bool ClipLine (float &x1, float &y1, float &x2, float &y2,
    int ClipX1, int ClipY1, int ClipX2, int ClipY2);

  /// Get R,G,B at given screen location
  void GetPixel (int x, int y, UByte &oR, UByte &oG, UByte &oB);

  /// Change system mouse cursor and return success status
  bool SwitchMouseCursor (csMouseCursorID Shape);

  /// Return the width of given text using selected font
  int TextWidth (const char *text, int Font);

  /// Return the height of selected font
  int TextHeight (int Font);

  /// Begin painting
  bool BeginDraw ();

  /// Finish painting
  void FinishDraw ();
};

#endif // __CSGFXPPL_H__
