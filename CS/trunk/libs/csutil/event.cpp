/*
    Event system related helpers
    Copyright (C) 2003 by Jorrit Tyberghein
	      (C) 2003 by Frank Richter

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "cssysdef.h"
#include "csutil/event.h"

utf32_char csKeyEventHelper::GetRawCode (const iEvent* event)
{
  uint32 code;
  if (!event->Find ("keyCodeRaw", code))
    return 0;
  return code;
}

utf32_char csKeyEventHelper::GetCookedCode (const iEvent* event)
{
  uint32 code;
  if (!event->Find ("keyCodeCooked", code))
    return 0;
  return code;
}

void csKeyEventHelper::GetModifiers (const iEvent* event, 
				     csKeyModifiers& modifiers)
{
  memset (&modifiers, 0, sizeof (modifiers));

  void* mod;
  uint32 modSize;
  if (!event->Find ("keyModifiers", &mod, modSize)) return;
  memcpy (&modifiers, mod, MIN (sizeof (modifiers), modSize));
}

csKeyEventType csKeyEventHelper::GetEventType (const iEvent* event)
{
  uint8 type;
  if (!event->Find ("keyEventType", type))
    return (csKeyEventType)-1;
  return (csKeyEventType)type;
}

bool csKeyEventHelper::GetAutoRepeat (const iEvent* event)
{
  bool autoRep;
  if (!event->Find ("keyAutoRepeat", autoRep)) return false;
  return autoRep;
}
				    
csKeyCharType csKeyEventHelper::GetCharacterType (const iEvent* event)
{
  uint8 type;
  if (!event->Find ("keyCharType", type))
    return (csKeyCharType)-1;
  return (csKeyCharType)type;
}

bool csKeyEventHelper::GetEventData (const iEvent* event, csKeyEventData& data)
{
  if (!CS_IS_KEYBOARD_EVENT (*event)) return false;

  data.autoRepeat = GetAutoRepeat (event);
  data.charType = GetCharacterType (event);
  data.codeCooked = GetCookedCode (event);
  data.codeRaw = GetRawCode (event);
  data.eventType = GetEventType (event);
  GetModifiers (event, data.modifiers);

  return true;
}

uint32 csKeyEventHelper::GetModifiersBits (const iEvent* event)
{
  csKeyModifiers m;
  GetModifiers (event, m);
  return GetModifiersBits (m);
}

uint32 csKeyEventHelper::GetModifiersBits (const csKeyModifiers& m)
{
  uint32 res = 0;

  for (int n = 0; n < csKeyModifierTypeLast; n++)
  {
    if (m.modifiers[n] != 0)
      res |= (1 << n);
  }

  return res;
}
