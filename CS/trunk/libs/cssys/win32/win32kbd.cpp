/*
    Copyright (C) 2003 by Jorrit Tyberghein
	      (C) 2003 by Frank Richter

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
#include "cssys/csuctransform.h"
#include "iutil/event.h"
#include <windows.h>

#include "win32kbd.h"
#include "cssys/win32/wintools.h"

static void ModifiersToKeyState (const csKeyModifiers& modifiersState,
				 BYTE* keystate)
{
  if (modifiersState.modifiers[csKeyModifierTypeShift] & 
    (1 << csKeyModifierNumLeft))
    keystate[VK_LSHIFT] |= 0x80;
  if (modifiersState.modifiers[csKeyModifierTypeShift] & 
    (1 << csKeyModifierNumRight))
    keystate[VK_RSHIFT] |= 0x80;
  if (modifiersState.modifiers[csKeyModifierTypeShift] != 0)
    keystate[VK_SHIFT] |= 0x80;

  if (modifiersState.modifiers[csKeyModifierTypeCtrl] & 
    (1 << csKeyModifierNumLeft))
    keystate[VK_LCONTROL] |= 0x80;
  if (modifiersState.modifiers[csKeyModifierTypeCtrl] & 
    (1 << csKeyModifierNumRight))
    keystate[VK_RCONTROL] |= 0x80;
  if (modifiersState.modifiers[csKeyModifierTypeCtrl] != 0)
    keystate[VK_CONTROL] |= 0x80;

  if (modifiersState.modifiers[csKeyModifierTypeAlt] & 
    (1 << csKeyModifierNumLeft))
    keystate[VK_LMENU] |= 0x80;
  if (modifiersState.modifiers[csKeyModifierTypeAlt] & 
    (1 << csKeyModifierNumRight))
    keystate[VK_RMENU] |= 0x80;
  if (modifiersState.modifiers[csKeyModifierTypeAlt] != 0)
    keystate[VK_MENU] |= 0x80;

  if (modifiersState.modifiers[csKeyModifierTypeCapsLock] != 0)
    keystate[VK_CAPITAL] |= 0x01;
  if (modifiersState.modifiers[csKeyModifierTypeNumLock] != 0)
    keystate[VK_NUMLOCK] |= 0x01;
  if (modifiersState.modifiers[csKeyModifierTypeScrollLock] != 0)
    keystate[VK_SCROLL] |= 0x01;
}

static bool CookedCodeToVKey (utf32_char codeCooked, SHORT& vKey,
			      BYTE* keystate)
{
  if (codeCooked >= 0x10000)
    return false;

  if (cswinIsWinNT ())
  {
    vKey = VkKeyScanW (codeCooked);
  }
  else
  {
    utf16_char wCh[2];
    CHAR ch;
    int wSize = csUnicodeTransform::EncodeUTF16 (codeCooked, 
      wCh, sizeof (wCh) / sizeof (utf16_char));
    if (WideCharToMultiByte (CP_ACP, 0, (WCHAR*)wCh, wSize, &ch, 1, 0, 0) == 0)
      return false;

    vKey = VkKeyScanA (ch);
  }

  if (vKey == -1) 
    return false;

  // Set up the appropriate key state
  memset (keystate, 0, 256 * sizeof (BYTE));
  if (vKey & 0x0100) keystate[VK_SHIFT] |= 0x80;
  if (vKey & 0x0200) keystate[VK_CONTROL] |= 0x80;
  if (vKey & 0x0400) keystate[VK_MENU] |= 0x80;
  vKey &= 0xff;

  return true;
}

//-----------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csWin32KeyComposer)
  SCF_IMPLEMENTS_INTERFACE(iKeyComposer)
SCF_IMPLEMENT_IBASE_END

csWin32KeyComposer::csWin32KeyComposer ()
{
  SCF_CONSTRUCT_IBASE(0);
  lastDead = 0;
  lastDeadVk = 0;
}

csWin32KeyComposer::~csWin32KeyComposer ()
{
}

csKeyComposeResult csWin32KeyComposer::HandleKey (
  const csKeyEventData& keyEventData, utf32_char* buf, size_t bufChars, 
  int* resultChars)
{
#define RETURN(ret, bs)			\
  {					\
    if (resultChars) *resultChars = bs;	\
    return ret;				\
  }

#define RETURN0(ret)	      RETURN(ret, 0)

#define RETURN1(ret, a)	      \
  {			      \
    if (bufChars >= 1)	      \
    {			      \
      buf[0] = a;	      \
      RETURN(ret, 1);	      \
    }			      \
    else		      \
      RETURN0(ret);	      \
  }

#define RETURN2(ret, a, b)    \
  {			      \
    if (bufChars >= 2)	      \
    {			      \
      buf[0] = a;	      \
      buf[1] = b;	      \
      RETURN(ret, 2);	      \
    }			      \
    else		      \
      RETURN1(ret, b);	      \
  }

  if (CSKEY_IS_SPECIAL (keyEventData.codeRaw))
    RETURN0(csComposeNoChar)

  if (lastDeadVk != 0)
  {
    /*
      Urg. Windows saves the dead key state in a variable somewhere
      when ToAscii() or ToUnicode() is called with one.
      So to get a properly composed char, we need to first call
      ToAscii() with the dead key, so the 
      */
    char outCh[2];
    int ret = ToAscii (lastDeadVk, MapVirtualKey (lastDeadVk, 0), 
      lastDeadKbdState, (PWORD)&outCh, 0);

    lastDeadVk = 0;

    WCHAR wCh[2];

    SHORT vKey;
    BYTE keystate[256];

    if (!CookedCodeToVKey (keyEventData.codeCooked, vKey, keystate))
      RETURN0(csComposeNoChar)

    // Now, do the actual composing.
    if (cswinIsWinNT ())
    {
      ret = ToUnicode (vKey, 0, keystate, wCh, 2, 0);
    }
    else
    {
      char outCh[2];
      ret = ToAscii (vKey, 0, keystate, (PWORD)&outCh, 0);
      if (ret != 0)
      {
	MultiByteToWideChar (CP_ACP, MB_PRECOMPOSED, outCh, 
	  (ret > 0) ? ret : 1, wCh,  sizeof (wCh) / sizeof (WCHAR));
      }
    }
    if (ret == 2)
    {
      RETURN2(csComposeUncomposeable, wCh[0], wCh[1])
    } 
    else if (ret == 1)
    {
      RETURN1(csComposeComposedChar, wCh[0])
    }
    else
    {
      RETURN0(csComposeNoChar)
    }
  }
  else
  {
    /*
      Not much to do else... the keyboard driver already does some
      translation...
     */
    if (keyEventData.charType == csKeyCharTypeDead)
    {
      if (!CookedCodeToVKey (keyEventData.codeCooked, lastDeadVk, 
	lastDeadKbdState))
	lastDeadVk = 0;

      RETURN0(csComposeNoChar)
    }
    else
    {
      RETURN1(csComposeNormalChar, keyEventData.codeCooked);
    }
  }
}

void csWin32KeyComposer::ResetState ()
{
  lastDead = 0;
  lastDeadVk = 0;
}

//-----------------------------------------------------------------------------

csWin32KeyboardDriver::csWin32KeyboardDriver (iObjectRegistry* r) :
  csKeyboardDriver (r)
{
}

void csWin32KeyboardDriver::Reset ()
{
  csKeyboardDriver::Reset ();
}

bool csWin32KeyboardDriver::HandleKeyMessage (HWND hWnd, UINT message, 
					      WPARAM wParam, LPARAM lParam)
{
  csRef<iEvent> ev;
  switch (message)
  {
    case WM_CHAR:
    case WM_UNICHAR:
      if (wParam != UNICODE_NOCHAR)
      {
	return true;
      }
      return false;
    case WM_KEYUP:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
      {
	utf32_char raw, cooked;
	bool down = (message == WM_KEYDOWN) || 
	    (message == WM_SYSKEYDOWN);
	bool autoRep = ((lParam & 0x40000000) != 0);
	csKeyCharType type;
	if (Win32KeyToCSKey (wParam, lParam, raw, cooked, type))
	{
  	  DoKey (raw, cooked, down, autoRep, type);
	}
      }
      return !(wParam == VK_F4 && 
	(modifiersState.modifiers[csKeyModifierTypeAlt] != 0));   // so Alt+F4 still works
    case WM_DEADCHAR:
    case WM_SYSDEADCHAR:
    default:
      return false;
  }
}

csPtr<iKeyComposer> csWin32KeyboardDriver::CreateKeyComposer ()
{
  return csPtr<iKeyComposer> (new csWin32KeyComposer ());
}

bool csWin32KeyboardDriver::Win32KeyToCSKey (LONG vKey, LONG keyFlags, 
					     utf32_char& rawCode, 
					     utf32_char& cookedCode,
					     csKeyCharType& charType)
{
  charType = csKeyCharTypeNormal;

#define DISTINGUISH_EXTENDED(nonExt, ext, cooked)	\
      rawCode = (keyFlags & 0x01000000) ? ext : nonExt;	\
      cookedCode = cooked;

  switch (vKey)
  {
    case VK_MENU:
      DISTINGUISH_EXTENDED (CSKEY_ALT_LEFT, CSKEY_ALT_RIGHT, CSKEY_ALT);
      return true;
    case VK_CONTROL:  
      DISTINGUISH_EXTENDED (CSKEY_CTRL_LEFT, CSKEY_CTRL_RIGHT, CSKEY_CTRL);
      return true;
    case VK_SHIFT:
      DISTINGUISH_EXTENDED (CSKEY_SHIFT_LEFT, CSKEY_SHIFT_RIGHT, CSKEY_SHIFT);
      return true;
    case VK_UP:       
      /*
        Arrow keys weirdness: if the 'extended' flag is set, they are the block
	left to the keypad. If it's not, then they are KP keys w/ numlock off.
       */
      DISTINGUISH_EXTENDED (CSKEY_PAD8, CSKEY_UP, CSKEY_UP);
      return true;
    case VK_DOWN:     
      DISTINGUISH_EXTENDED (CSKEY_PAD2, CSKEY_DOWN, CSKEY_DOWN);
      return true;
    case VK_LEFT:     
      DISTINGUISH_EXTENDED (CSKEY_PAD4, CSKEY_LEFT, CSKEY_LEFT);
      return true;
    case VK_RIGHT:    
      DISTINGUISH_EXTENDED (CSKEY_PAD6, CSKEY_RIGHT, CSKEY_RIGHT);
      return true;
    case VK_CLEAR:    
      rawCode = CSKEY_PAD5; 
      cookedCode = CSKEY_CENTER; 
      return true;
    case VK_INSERT:   
      DISTINGUISH_EXTENDED (CSKEY_PAD0, CSKEY_INS, CSKEY_INS);
      return true;
    case VK_DELETE:   
      DISTINGUISH_EXTENDED (CSKEY_PADDECIMAL, CSKEY_DEL, CSKEY_DEL);
      return true;
    case VK_PRIOR:    
      DISTINGUISH_EXTENDED (CSKEY_PAD9, CSKEY_PGUP, CSKEY_PGUP);
      return true;
    case VK_NEXT:     
      DISTINGUISH_EXTENDED (CSKEY_PAD3, CSKEY_PGDN, CSKEY_PGDN);
      return true;
    case VK_HOME:     
      DISTINGUISH_EXTENDED (CSKEY_PAD7, CSKEY_HOME, CSKEY_HOME);
      return true;
    case VK_END:
      DISTINGUISH_EXTENDED (CSKEY_PAD1, CSKEY_END, CSKEY_END);
      return true;
    case VK_RETURN:   
      DISTINGUISH_EXTENDED (CSKEY_ENTER, CSKEY_PADENTER, CSKEY_ENTER);
      return true;
    case VK_NUMPAD0:
      rawCode = CSKEY_PAD0;
      cookedCode = '0';
      return true;
    case VK_NUMPAD1:
      rawCode = CSKEY_PAD1;
      cookedCode = '1';
      return true;
    case VK_NUMPAD2:
      rawCode = CSKEY_PAD2;
      cookedCode = '2';
      return true;
    case VK_NUMPAD3:
      rawCode = CSKEY_PAD3;
      cookedCode = '3';
      return true;
    case VK_NUMPAD4:
      rawCode = CSKEY_PAD4;
      cookedCode = '4';
      return true;
    case VK_NUMPAD5:
      rawCode = CSKEY_PAD5;
      cookedCode = '5';
      return true;
    case VK_NUMPAD6:
      rawCode = CSKEY_PAD6;
      cookedCode = '6';
      return true;
    case VK_NUMPAD7:
      rawCode = CSKEY_PAD7;
      cookedCode = '7';
      return true;
    case VK_NUMPAD8:
      rawCode = CSKEY_PAD8;
      cookedCode = '8';
      return true;
    case VK_NUMPAD9:
      rawCode = CSKEY_PAD9;
      cookedCode = '9';
      return true;
    case VK_BACK:     
      rawCode = CSKEY_BACKSPACE;
      cookedCode = CSKEY_BACKSPACE;
      return true;
    case VK_TAB:
      rawCode = CSKEY_TAB;
      cookedCode = CSKEY_TAB;
      return true;
    case VK_ESCAPE:
      rawCode = CSKEY_ESC;
      cookedCode = CSKEY_ESC;
      return true;
    case VK_F1:
      rawCode = CSKEY_F1;
      cookedCode = CSKEY_F1;
      return true;
    case VK_F2:
      rawCode = CSKEY_F2;
      cookedCode = CSKEY_F2;
      return true;
    case VK_F3:
      rawCode = CSKEY_F3;
      cookedCode = CSKEY_F3;
      return true;
    case VK_F4:
      rawCode = CSKEY_F4;
      cookedCode = CSKEY_F4;
      return true;
    case VK_F5:
      rawCode = CSKEY_F5;
      cookedCode = CSKEY_F5;
      return true;
    case VK_F6:
      rawCode = CSKEY_F6;
      cookedCode = CSKEY_F6;
      return true;
    case VK_F7:
      rawCode = CSKEY_F7;
      cookedCode = CSKEY_F7;
      return true;
    case VK_F8:
      rawCode = CSKEY_F8;
      cookedCode = CSKEY_F8;
      return true;
    case VK_F9:
      rawCode = CSKEY_F9;
      cookedCode = CSKEY_F9;
      return true;
    case VK_F10:
      rawCode = CSKEY_F10;
      cookedCode = CSKEY_F10;
      return true;
    case VK_F11:
      rawCode = CSKEY_F11;
      cookedCode = CSKEY_F11;
      return true;
    case VK_F12:
      rawCode = CSKEY_F12;
      cookedCode = CSKEY_F12;
      return true;
    case VK_ADD:
      DISTINGUISH_EXTENDED ('+', CSKEY_PADPLUS, '+');
      return true;
    case VK_SUBTRACT:
      DISTINGUISH_EXTENDED ('-', CSKEY_PADMINUS, '-');
      return true;
    case VK_MULTIPLY: 
      DISTINGUISH_EXTENDED ('*', CSKEY_PADMULT, '*');
      return true;
    case VK_DIVIDE:
      DISTINGUISH_EXTENDED ('/', CSKEY_PADDIV, '/');
      return true;
    case VK_NUMLOCK:
      rawCode = CSKEY_PADNUM;
      cookedCode = CSKEY_PADNUM;
      return true;
    case VK_PAUSE:
      rawCode = CSKEY_PAUSE;
      cookedCode = CSKEY_PAUSE;
      return true;
    case VK_APPS:
      rawCode = CSKEY_CONTEXT;
      cookedCode = CSKEY_CONTEXT;
      return true;
    case VK_CAPITAL:
      rawCode = CSKEY_CAPSLOCK;
      cookedCode = CSKEY_CAPSLOCK;
      return true;
    case VK_SNAPSHOT:
      rawCode = CSKEY_PRINTSCREEN;
      cookedCode = CSKEY_PRINTSCREEN;
      return true;
    case VK_SCROLL:
      rawCode = CSKEY_SCROLLLOCK;
      cookedCode = CSKEY_SCROLLLOCK;
      return true;
    default:
      {
	WCHAR wCh[2];
	int ret;
	uint8 keystate[256];

	// First, calculate the raw code. This is done by retrieving the key's
	// character without any pressed modifiers.
	memset (&keystate, 0, sizeof (keystate));
	if (cswinIsWinNT ())
	{
	  ret = ToUnicode (vKey, keyFlags, keystate, wCh, 2, 0);
	}
	else
	{
	  char outCh[2];
	  ret = ToAscii (vKey, keyFlags, keystate, (PWORD)&outCh, 0);
	  if (ret != 0)
	  {
	    MultiByteToWideChar (CP_ACP, MB_PRECOMPOSED, outCh, 
	      (ret > 0) ? ret : 0, wCh,  sizeof (wCh) / sizeof (WCHAR));
	  }
	}
	/*
	  In case 2 chars were emitted (couldn't compose from dead key),
	  put the last one into 'cooked'.
	*/
	if (ret > 0)
	{
	  rawCode = wCh[(ret == 1) ? 0 : 1];
	}
	else if (ret < 0)
	{
	  // Dead char
	  rawCode = wCh[0];
	}
	else
	{
	  // Couldn't get a code for that key
	  rawCode = cookedCode = 0;
	  return false;
	}

	// Now, the cooked code

	// Set up modifiers
	ModifiersToKeyState (modifiersState, keystate);

	if (cswinIsWinNT ())
	{
	  ret = ToUnicode (vKey, keyFlags, keystate, wCh, 2, 0);
	}
	else
	{
	  char outCh[2];
	  ret = ToAscii (vKey, keyFlags, keystate, (PWORD)&outCh, 0);
	  if (ret != 0)
	  {
	    MultiByteToWideChar (CP_ACP, MB_PRECOMPOSED, outCh, 
	      (ret > 0) ? ret : 0, wCh,  sizeof (wCh) / sizeof (WCHAR));
	  }
	}
	if (ret > 0)
	{
	  cookedCode = wCh[(ret == 1) ? 0 : 1];
	  // Composed char
	  /*if ((lastDeadVk != 0) && (ret == 1)) charType = csKeyCharTypeComposed;*/
	}
  	else if (ret < 0)
	{
	  cookedCode = wCh[0];
	  // Dead char
	  charType = csKeyCharTypeDead;
	}
	else
	{
	  cookedCode = 0;
	}

	return true;
      }
  }
#undef DISTINGUISH_EXTENDED

  return false;
}

void csWin32KeyboardDriver::UpdateModifierState ()
{
  uint8 keystate[256];
  GetKeyboardState (keystate);

  memset (&modifiersState, 0, sizeof (modifiersState));
  if (keystate[VK_LSHIFT] & 0x80)
    modifiersState.modifiers[csKeyModifierTypeShift] |=
    (1 << csKeyModifierNumLeft);
  if (keystate[VK_RSHIFT] & 0x80)
    modifiersState.modifiers[csKeyModifierTypeShift] |=
    (1 << csKeyModifierNumRight);

  if (keystate[VK_LCONTROL] & 0x80)
    modifiersState.modifiers[csKeyModifierTypeCtrl] |=
    (1 << csKeyModifierNumLeft);
  if (keystate[VK_RCONTROL] & 0x80)
    modifiersState.modifiers[csKeyModifierTypeCtrl] |=
    (1 << csKeyModifierNumRight);

  if (keystate[VK_LMENU] & 0x80)
    modifiersState.modifiers[csKeyModifierTypeAlt] |=
    (1 << csKeyModifierNumLeft);
  if (keystate[VK_RMENU] & 0x80)
    modifiersState.modifiers[csKeyModifierTypeAlt] |=
    (1 << csKeyModifierNumRight);

  if (keystate[VK_CAPITAL] & 0x01)
    modifiersState.modifiers[csKeyModifierTypeCapsLock] = 1;
  if (keystate[VK_SCROLL] & 0x01)
    modifiersState.modifiers[csKeyModifierTypeScrollLock] = 1;
  if (keystate[VK_NUMLOCK] & 0x01)
    modifiersState.modifiers[csKeyModifierTypeNumLock] = 1;

  // @@@ Hm, call SetKeyState() for pressed keys?
}

