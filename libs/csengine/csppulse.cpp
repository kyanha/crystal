/*
    Copyright (C) 1999 by Eric Sunshine <sunshine@sunshineco.com>
    Writen by Eric Sunshine <sunshine@sunshineco.com>

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

#include "sysdef.h"
#include "csengine/world.h"
#include "csengine/csppulse.h"

static char const ANIMATION[] = "-\\|/";
int const ANIMATION_COUNT = sizeof(ANIMATION) / sizeof(ANIMATION[0]) - 1;

csProgressPulse::csProgressPulse() :
    type(MSG_INITIALIZATION), state(0), drawn(false)
{
}

csProgressPulse::~csProgressPulse()
{
  Erase();
}

void csProgressPulse::Erase()
{
  if (drawn)
  {
    CsPrintf (type, "\b \b");
    drawn = false;
  }
}

void csProgressPulse::Reset()
{
  Erase();
  state = 0;
}

void csProgressPulse::Step()
{
  char const* prefix = (drawn ? "\b" : "");
  drawn = true;
  CsPrintf (type, "%s%c", prefix, ANIMATION[state]);
  if (++state >= ANIMATION_COUNT)
    state = 0;
}
