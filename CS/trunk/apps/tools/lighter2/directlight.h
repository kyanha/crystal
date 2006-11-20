/*
  Copyright (C) 2006 by Marten Svanfeldt

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

#ifndef __DIRECTLIGHT_H__
#define __DIRECTLIGHT_H__

#include "primitive.h"

namespace lighter
{
  class Sector;
  class Raytracer;
  class Primitive;
  class Light;
  
  // Class to calculate direct lighting
  class DirectLighting
  {
  public:

    // Shade by using all primitives within range
    static void ShootDirectLighting (Sector* sector, float progressStep);

  private:
    //Make sure we don't instance it
    DirectLighting ();
    DirectLighting (const DirectLighting& o);
  };
}

#endif
