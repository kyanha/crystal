/*
    Copyright (C) 2000 by Jorrit Tyberghein
    Copyright (C) 2000 by W.C.A. Wijngaards

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
#include "ivideo/txtmgr.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "csutil/cscolor.h"
#include "csutil/util.h"
#include "iutil/objreg.h"

#include "cstool/prplasma.h"

/// the implementation of the plasma texture
csProcPlasma::csProcPlasma () : csProcTexture()
{
  palsize = 0;
  palette = NULL;
  costable = 0;
  mat_w = 64;
  mat_h = 64;

  texFlags = CS_TEXTURE_3D | CS_TEXTURE_NOMIPMAPS;
}

csProcPlasma::~csProcPlasma ()
{
  delete[] palette;
  delete[] costable;
}

bool csProcPlasma::PrepareAnim ()
{
  if (anim_prepared) return true;
  if (!csProcTexture::PrepareAnim ()) return false;
  costable = new uint8[256];
  memset (costable, 0, sizeof (uint8)*256);
  MakeTable ();
  MakePalette (256);
  anims0= 0;
  anims1= 0;
  anims2= 0;
  anims3= 0;
  offsets0=0;
  offsets1=1;

  lineincr0 = +3;
  lineincr1 = +4;
  lineincr2 = +1;
  lineincr3 = +2;
  frameincr0 = -4;
  frameincr1 = +3;
  frameincr2 = -2;
  frameincr3 = +1;
  offsetincr0 = +2;
  offsetincr1 = -3;

  if (mat_w<256)
  {
    lineincr0 = lineincr0*256/mat_w;
    lineincr1 = lineincr1*256/mat_w;
    lineincr2 = lineincr2*256/mat_w;
    lineincr3 = lineincr3*256/mat_w;
  }
  return true;
}

void csProcPlasma::MakeTable ()
{
  int i;
  for (i=0 ; i<256 ; i++)
  {
    /// scale i so that 0-256 is 360 degrees.
    /// then convert that to radians.
    double angle = double(i)/256.0*360.0*PI/180.0;
    /// costable has results from [-1..1] mapped to [0..63].
    costable[i] = (uint8) ( cos(angle)*32+32 );
    /// make sure no 64 in table; 4*64 = 256; which will be out of bounds
    if(costable[i]==64) costable[i]=63;
  }
}

void csProcPlasma::MakePalette (int max)
{
  int i;
  if (palette) delete[] palette;
  palsize = max;
  palette = new int[palsize];
  iTextureManager* ptTxtMgr = g3d->GetTextureManager ();
  palette[0] = ptTxtMgr->FindRGB (0,0,0);
  for (i = 0; i < palsize; i++)
    palette[i] = palette[0];
  /// fill the palette
  int maxcolours = palsize;
  csColor col;
  int r,g,b;
  for (i = 0; i < maxcolours; i++)
  {
    col.Set(0.9f, 0.9f, 1.0f);
    float val;// = float(GetCos( (i*4)%256))/float(64);
    val = float(i) / float(maxcolours);
    float I = val * val;
    col *= I;
    col *= 255.0f;
    r = (int) col.red;
    g = (int) col.green;
    b = (int) col.blue;
    palette[i] = ptTxtMgr->FindRGB(r, g, b);
  }
}

void csProcPlasma::Animate (csTicks current_time)
{
  (void)current_time;
  int x, y;
  /// draw palette
  //for(int i=0; i<palsize; i++)
  //{
  //  g2d->DrawBox(i*(g2d->GetWidth()/palsize), 1,
  //    (g2d->GetWidth()/palsize)+5, 10, palette[i]);
  //}

  g3d->SetRenderTarget (tex->GetTextureHandle ());
  if (!g3d->BeginDraw (CSDRAW_2DGRAPHICS)) return;

  uint8 curanim0, curanim1, curanim2, curanim3;
  /// draw texture
  curanim2 = anims2;
  curanim3 = anims3;
  for(y=0; y<mat_h; y++)
  {
    curanim0 = anims0;
    curanim1 = anims1;
    int thex_cst = GetCos(y*256/mat_h+ offsets0)/(4*64/mat_w);
    int col_cst = GetCos(curanim2) + GetCos(curanim3);
    for(x=0; x<mat_w;x++)
    {
      int col= GetCos(curanim0) + GetCos(curanim1) + col_cst;
      int thex = x+thex_cst;
      int they = y+GetCos(thex*256/mat_w+ offsets1)/(4*64/mat_h);
      thex %= mat_w;
      they %= mat_h;

      g2d->DrawPixel (thex, they, palette[col*palsize/256] );

      curanim0 += lineincr0;
      curanim1 += lineincr1;
    }
    curanim2 += lineincr2;
    curanim3 += lineincr3;
  }

  g3d->FinishDraw ();

  anims0 += frameincr0;
  anims1 += frameincr1;
  anims2 += frameincr2;
  anims3 += frameincr3;
  offsets0 += offsetincr0;
  offsets1 += offsetincr1;
}

