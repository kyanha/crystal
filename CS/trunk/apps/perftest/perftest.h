/*
    Copyright (C) 2000 by Jorrit Tyberghein

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

#ifndef PERF_H
#define PERF_H

#include "cssys/sysdriv.h"

class PerfTest;
struct iTextureHandle;

class Tester
{
protected:
  int draw;

public:
  virtual void Setup (iGraphics3D* g3d, PerfTest* perftest) = 0;
  virtual void Draw (iGraphics3D* g3d) = 0;
  virtual void Description (char* dst) = 0;
  virtual Tester* NextTester () = 0;
  int GetCount () { return draw; }
};


class PerfTest : public SysSystemDriver
{
  typedef SysSystemDriver superclass;

private:
  bool draw_3d;
  bool draw_2d;
  bool needs_setup;
  Tester* current_tester;
  iTextureHandle* texture[4];
  // Load a texture.
  iTextureHandle* LoadTexture (char* file);

public:
  PerfTest ();
  virtual ~PerfTest ();

  virtual bool Initialize (int argc, const char* const argv[],
    const char *iConfigName);
  virtual void NextFrame (time_t elapsed_time, time_t current_time);
  virtual bool HandleEvent (csEvent &Event);
  
  iTextureHandle* GetTexture (int idx)
  {
    return texture[idx];
  }
};

#endif // PERF_H

