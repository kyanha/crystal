/*
    Copyright (C) 2001 by W.C.A. Wijngaards

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
#include "cstool/csfxscr.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/txtmgr.h"
#include "csutil/cscolor.h"

void csfxInterference(iGraphics2D *g2d, iTextureManager *txtmgr,
  float amount, float anim, float length)
{
#define SEMIRAND anim = (anim + 0.137564f) - int(anim + 0.137564f);
  if (amount == 0) amount = 0.000001f;
  float skipwidth = length * (1.0f / amount);
  float xpos = 0.0f;
  float ypos = 0.0f;
  int width = g2d->GetWidth();
  int height = g2d->GetHeight();
  while(ypos < height)
  {
    float skipnow = skipwidth * anim; SEMIRAND;
    float donow = length * anim; SEMIRAND;
    int col = 255 - int(255.0f * anim); SEMIRAND;
    int colour = g2d->FindRGB(col, col, col);
    while(donow+xpos >= width)
    {
      g2d->DrawLine(xpos, ypos, width-1, ypos, colour);
      ypos++;
      if(ypos >= height) return;
      donow -= width-xpos;
      xpos = 0.0f;
    }
    g2d->DrawLine(xpos, ypos, xpos+donow, ypos, colour);
    xpos += donow;
    xpos += skipnow;
    ypos += int(xpos) / width;
    xpos += (-int(xpos) + int(xpos)%width);
  }
#undef SEMIRAND
}

void csfxFadeToColor(iGraphics3D *g3d, float fadevalue, const csColor& color)
{
  uint8 red = int(255*color.red);
  uint8 green = int(255*color.green);
  uint8 blue = int(255*color.blue);
  float fade = 1.0 - fadevalue;
#ifndef CS_USE_NEW_RENDERER
  csfxScreenDPFX(g3d, NULL, CS_FX_SETALPHA(fade), red, green, blue);
#endif // CS_USE_NEW_RENDERER
}

void csfxFadeOut(iGraphics3D *g3d, float fadevalue)
{
  uint8 multval = 255 - int(255.*fadevalue);
#ifndef CS_USE_NEW_RENDERER
  csfxScreenDPFX(g3d, NULL, CS_FX_MULTIPLY, multval, multval, multval);
#endif // CS_USE_NEW_RENDERER
}

void csfxFadeTo(iGraphics3D *g3d, iMaterialHandle *mat, float fadevalue)
{
  float fade = 1.0f - fadevalue;
#ifndef CS_USE_NEW_RENDERER
  csfxScreenDPFX(g3d, mat, CS_FX_SETALPHA(fade), 0, 0, 0);
#endif // CS_USE_NEW_RENDERER
}

void csfxGreenScreen(iGraphics3D *g3d, float fadevalue)
{
  uint8 multval = 255 - int(255.*fadevalue);
#ifndef CS_USE_NEW_RENDERER
  csfxScreenDPFX(g3d, NULL, CS_FX_MULTIPLY, multval, 255, multval);
#endif // CS_USE_NEW_RENDERER
}

void csfxRedScreen(iGraphics3D *g3d, float fadevalue)
{
  uint8 multval = 255 - int(255.*fadevalue);
#ifndef CS_USE_NEW_RENDERER
  csfxScreenDPFX(g3d, NULL, CS_FX_MULTIPLY, 255, multval, multval);
#endif // CS_USE_NEW_RENDERER
}

void csfxBlueScreen(iGraphics3D *g3d, float fadevalue)
{
  uint8 multval = 255 - int(255.*fadevalue);
#ifndef CS_USE_NEW_RENDERER
  csfxScreenDPFX(g3d, NULL, CS_FX_MULTIPLY, multval, multval, 255);
#endif // CS_USE_NEW_RENDERER
}


void csfxWhiteOut(iGraphics3D *g3d, float fadevalue)
{
  uint8 multval = int(255.*fadevalue);
#ifndef CS_USE_NEW_RENDERER
  csfxScreenDPFX(g3d, NULL, CS_FX_ADD, multval, multval, multval);
#endif // CS_USE_NEW_RENDERER
}

void csfxShadeVert(iGraphics3D *g3d, const csColor& topcolor,
  const csColor& bottomcolor, uint mixmode)
{
#ifndef CS_USE_NEW_RENDERER
  G3DPolygonDPFX dpfx;
  dpfx.num = 4;
  dpfx.use_fog = false;
  /// sy=0 is bottom of screen
  dpfx.vertices[0].x = 0.;
  dpfx.vertices[0].y = 0.;
  dpfx.z[0] = 0.;
  dpfx.texels[0].x = 0.;
  dpfx.texels[0].y = 1.;
  dpfx.colors[0].red = bottomcolor.red;
  dpfx.colors[0].green = bottomcolor.green;
  dpfx.colors[0].blue = bottomcolor.blue;
  dpfx.vertices[1].x = 0.;
  dpfx.vertices[1].y = g3d->GetHeight();
  dpfx.z[1] = 0.;
  dpfx.texels[1].x = 0.;
  dpfx.texels[1].y = 0.;
  dpfx.colors[1].red = topcolor.red;
  dpfx.colors[1].green = topcolor.green;
  dpfx.colors[1].blue = topcolor.blue;
  dpfx.vertices[2].x = g3d->GetWidth();
  dpfx.vertices[2].y = g3d->GetHeight();
  dpfx.z[2] = 0.;
  dpfx.texels[2].x = 1.;
  dpfx.texels[2].y = 0.;
  dpfx.colors[2].red = topcolor.red;
  dpfx.colors[2].green = topcolor.green;
  dpfx.colors[2].blue = topcolor.blue;
  dpfx.vertices[3].x = g3d->GetWidth();
  dpfx.vertices[3].y = 0.;
  dpfx.z[3] = 0.;
  dpfx.texels[3].x = 1.;
  dpfx.texels[3].y = 1.;
  dpfx.colors[3].red = bottomcolor.red;
  dpfx.colors[3].green = bottomcolor.green;
  dpfx.colors[3].blue = bottomcolor.blue;
  dpfx.mat_handle = NULL;
  dpfx.mixmode = mixmode | CS_FX_GOURAUD;
  dpfx.flat_color_r = 255;
  dpfx.flat_color_g = 255;
  dpfx.flat_color_b = 255;
  int oldzbufmode = g3d->GetRenderState(G3DRENDERSTATE_ZBUFFERMODE);
  g3d->SetRenderState(G3DRENDERSTATE_ZBUFFERMODE, CS_ZBUF_NONE);
  g3d->DrawPolygonFX(dpfx);
  g3d->SetRenderState(G3DRENDERSTATE_ZBUFFERMODE, oldzbufmode);
#endif // CS_USE_NEW_RENDERER
}


void csfxScreenDPFX(iGraphics3D *g3d, iMaterialHandle *mat, uint mixmode,
  uint8 r, uint8 g, uint8 b)
{
#ifndef CS_USE_NEW_RENDERER
  G3DPolygonDPFX dpfx;
  dpfx.num = 4;
  dpfx.use_fog = false;
  dpfx.vertices[0].x = 0.;
  dpfx.vertices[0].y = 0.;
  dpfx.z[0] = 0.;
  dpfx.texels[0].x = 0.;
  dpfx.texels[0].y = 1.;
  dpfx.colors[0].red = 1.;
  dpfx.colors[0].green = 1.;
  dpfx.colors[0].blue = 1.;
  dpfx.vertices[1].x = 0.;
  dpfx.vertices[1].y = g3d->GetHeight();
  dpfx.z[1] = 0.;
  dpfx.texels[1].x = 0.;
  dpfx.texels[1].y = 0.;
  dpfx.colors[1].red = 1.;
  dpfx.colors[1].green = 1.;
  dpfx.colors[1].blue = 1.;
  dpfx.vertices[2].x = g3d->GetWidth();
  dpfx.vertices[2].y = g3d->GetHeight();
  dpfx.z[2] = 0.;
  dpfx.texels[2].x = 1.;
  dpfx.texels[2].y = 0.;
  dpfx.colors[2].red = 1.;
  dpfx.colors[2].green = 1.;
  dpfx.colors[2].blue = 1.;
  dpfx.vertices[3].x = g3d->GetWidth();
  dpfx.vertices[3].y = 0.;
  dpfx.z[3] = 0.;
  dpfx.texels[3].x = 1.;
  dpfx.texels[3].y = 1.;
  dpfx.colors[3].red = 1.;
  dpfx.colors[3].green = 1.;
  dpfx.colors[3].blue = 1.;
  dpfx.mat_handle = mat;
  dpfx.mixmode = mixmode;
  dpfx.flat_color_r = r;
  dpfx.flat_color_g = g;
  dpfx.flat_color_b = b;
  int oldzbufmode = g3d->GetRenderState (G3DRENDERSTATE_ZBUFFERMODE);
  g3d->SetRenderState (G3DRENDERSTATE_ZBUFFERMODE, CS_ZBUF_NONE);
  g3d->DrawPolygonFX (dpfx);
  g3d->SetRenderState (G3DRENDERSTATE_ZBUFFERMODE, oldzbufmode);
#endif // CS_USE_NEW_RENDERER
}

