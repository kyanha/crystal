/*
    Crystal Space input library
    Copyright (C) 1998,2000 by Jorrit Tyberghein
    Written by Andrew Zabolotny <bit@eltech.ru>

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

#ifndef __CS_CSINPUT_H__
#define __CS_CSINPUT_H__

/*
 * These are the low-level implementations of generic classes of input devices
 * like keyboard, mouse, and joystick.
 */

#include "csextern.h"
#include "scf.h"
#include "array.h"
#include "hash.h"
#include "iutil/csinput.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"

struct iEvent;
struct iEventQueue;
struct iObjectRegistry;

/**
 * Superclass of all the generic input drivers.
 */
class CS_CSUTIL_EXPORT csInputDriver
{
private:
  bool Registered;
protected:
  iObjectRegistry* Registry;
  iEventHandler* Listener;
  csInputDriver(iObjectRegistry*);
  virtual ~csInputDriver();
  csPtr<iEventQueue> GetEventQueue();
  virtual void GainFocus() = 0;
  virtual void LostFocus() = 0;
  virtual void Post(iEvent*);
  virtual bool HandleEvent(iEvent&);
  friend struct FocusListener;
  void StartListening();
  void StopListening();
};

class CS_CSUTIL_EXPORT csKeyComposer : public iKeyComposer
{
protected:
  utf32_char lastDead;

public:
  SCF_DECLARE_IBASE;

  csKeyComposer ();
  virtual ~csKeyComposer ();

  virtual csKeyComposeResult HandleKey (const csKeyEventData& keyEventData,
    utf32_char* buf, size_t bufChars, int* resultChars = 0);
  virtual void ResetState ();
};

/**
 * Generic Keyboard Driver.<p>
 * Keyboard driver should generate events and put them into
 * an event queue. Also it tracks the current state of all keys.
 */
class CS_CSUTIL_EXPORT csKeyboardDriver : public csInputDriver,
  public iKeyboardDriver
{
protected:
  /// Key state array.
  csHash<bool, utf32_char> keyStates;
  csKeyModifiers modifiersState;

  /**
   * Set key state. For example SetKey (CSKEY_UP, true). Called
   * automatically by do_press and do_release.
   */
  virtual void SetKeyState (utf32_char codeRaw, bool iDown,
    bool autoRepeat);
  /**
   * Generates a 'cooked' key code for a 'raw' key code from some simple
   * rules.
   */
  virtual void SynthesizeCooked (utf32_char codeRaw,
    const csKeyModifiers& modifiers, utf32_char& codeCooked);
public:
  SCF_DECLARE_IBASE;

  /// Initialize keyboard interface.
  csKeyboardDriver (iObjectRegistry*);
  /// Destructor.
  virtual ~csKeyboardDriver ();

  /// Call to release all key down flags.
  virtual void Reset ();
  /// Call to get the key down flags in sync with the actual pressed keys.
  virtual void RestoreKeys ();

  /**
   * Call this routine to add a key down/up event to queue.
   * \param codeRaw 'Raw' code of the pressed key.
   * \param codeCooked 'Cooked' code of the pressed key.
   * \param iDown Whether the key is up or down.
   * \param autoRepeat Auto-repeat flag for the key event. Typically only
   *  used by the platform-specific keyboard agents.
   * \param charType When the cooked code is a character, it determines
   *  whether it is a normal, dead or composed character.
   */
  virtual void DoKey (utf32_char codeRaw, utf32_char codeCooked, bool iDown,
    bool autoRepeat = false, csKeyCharType charType = csKeyCharTypeNormal);

  /**
   * Query the state of a key. All key codes in range 0..255,
   * CSKEY_FIRST..CSKEY_LAST are supported. Returns true if
   * the key is pressed, false if not.
   */
  virtual bool GetKeyState (utf32_char codeRaw);

  /**
   * Query the state of a modifier key.
   * Returns a bit field, where the nth bit is set if the nth modifier of a
   * type is pressed. If a specific modifier is requested, e.g. 
   * #CSKEY_SHIFT_LEFT, only the according bit is set. Otherwise, for a generic
   * modifier (e.g. #CSKEY_SHIFT), all distinct modifier keys of that type are 
   * represented.<p>
   * Example: Test if any Alt key is pressed:
   * \code
   *   bool pressed = (KeyboardDriver->GetModifierState (CSKEY_ALT) != 0);
   * \endcode
   * Example: Test if the right Ctrl key is pressed:
   * \code
   *   bool pressed = (KeyboardDriver->GetModifierState (CSKEY_CTRL_RIGHT) != 0);
   * \endcode
   * \param codeRaw Raw code of the modifier key.
   * \return Bit mask with the pressed modifiers.
   */
  virtual uint32 GetModifierState (utf32_char codeRaw);

  virtual csPtr<iKeyComposer> CreateKeyComposer ();

  /// Application lost focus.
  virtual void LostFocus() { Reset(); }
  virtual void GainFocus() { RestoreKeys(); }

  /// iEventHandler implementation
  struct CS_CSUTIL_EXPORT eiEventHandler : public iEventHandler
  {
    SCF_DECLARE_EMBEDDED_IBASE(csKeyboardDriver);
    virtual bool HandleEvent(iEvent& e) { return scfParent->HandleEvent(e); }
  } scfiEventHandler;
  friend struct eiEventHandler;
};

/**
 * Generic Mouse Driver.<p>
 * Mouse driver should generate events and put them into the event queue.  Also
 * it is responsible for generating double-click events.
 */
class CS_CSUTIL_EXPORT csMouseDriver : public csInputDriver, public iMouseDriver
{
private:
  // Generic keyboard driver (for checking modifier key states).
  csRef<iKeyboardDriver> Keyboard;

protected:
  /// Last "mouse down" event time
  csTicks LastClickTime;
  /// Last "mouse down" event button
  int LastClickButton;
  /// Last "mouse down" event position
  int LastClickX, LastClickY;
  /// Last mouse position
  int LastX, LastY;
  /// Mouse buttons state
  bool Button [CS_MAX_MOUSE_BUTTONS];
  /// Mouse double click max interval in 1/1000 seconds
  csTicks DoubleClickTime;
  /// Mouse double click max distance
  size_t DoubleClickDist;
  /// Get the generic keyboard driver (for checking modifier states).
  iKeyboardDriver* GetKeyboardDriver();

public:
  SCF_DECLARE_IBASE;

  /// Initialize mouse interface.
  csMouseDriver (iObjectRegistry*);
  /// Destructor.
  virtual ~csMouseDriver ();

  /// Set double-click mouse parameters
  virtual void SetDoubleClickTime (int iTime, size_t iDist);

  /// Call to release all mouse buttons.
  virtual void Reset ();

  /// Query last mouse X position
  virtual int GetLastX () { return LastX; }
  /// Query last mouse Y position
  virtual int GetLastY () { return LastY; }
  /// Query the last known mouse button state
  virtual bool GetLastButton (int button)
  {
    return (button > 0 && button <= CS_MAX_MOUSE_BUTTONS) ?
      Button [button - 1] : false;
  }

  /// Call this to add a 'mouse button down/up' event to queue
  virtual void DoButton (int button, bool down, int x, int y);
  /// Call this to add a 'mouse moved' event to queue
  virtual void DoMotion (int x, int y);

  /// Application lost focus.
  virtual void LostFocus() { Reset(); }
  virtual void GainFocus() { }

  /// iEventHandler implementation
  struct CS_CSUTIL_EXPORT eiEventHandler : public iEventHandler
  {
    SCF_DECLARE_EMBEDDED_IBASE(csMouseDriver);
    virtual bool HandleEvent(iEvent& e) { return scfParent->HandleEvent(e); }
  } scfiEventHandler;
  friend struct eiEventHandler;
};

/**
 * Generic Joystick driver.<p>
 * The joystick driver is responsible for tracking current
 * joystick state and also for generating joystick events.
 */
class CS_CSUTIL_EXPORT csJoystickDriver : public csInputDriver,
  public iJoystickDriver
{
private:
  // Generic keyboard driver (for checking modifier key states).
  csRef<iKeyboardDriver> Keyboard;
protected:
  /// Joystick button states
  bool Button [CS_MAX_JOYSTICK_COUNT][CS_MAX_JOYSTICK_BUTTONS];
  /// Joystick axis positions
  int LastX [CS_MAX_JOYSTICK_COUNT], LastY [CS_MAX_JOYSTICK_COUNT];
  /// Get the generic keyboard driver (for checking modifier states).
  iKeyboardDriver* GetKeyboardDriver();

public:
  SCF_DECLARE_IBASE;

  /// Initialize joystick interface.
  csJoystickDriver (iObjectRegistry*);
  /// Destructor.
  virtual ~csJoystickDriver ();

  /// Call to release all joystick buttons.
  virtual void Reset ();

  /// Query last joystick X position
  virtual int GetLastX (int number) { return LastX [number]; }
  /// Query last joystick Y position
  virtual int GetLastY (int number) { return LastY [number]; }
  /// Query the last known joystick button state
  virtual bool GetLastButton (int number, int button)
  {
    return (number > 0 && number <= CS_MAX_JOYSTICK_COUNT
         && button > 0 && button <= CS_MAX_JOYSTICK_BUTTONS) ?
            Button [number - 1] [button - 1] : false;
  }

  /// Call this to add a 'joystick button down/up' event to queue
  virtual void DoButton (int number, int button, bool down, int x, int y);
  /// Call this to add a 'joystick moved' event to queue
  virtual void DoMotion (int number, int x, int y);

  /// Application lost focus.
  virtual void LostFocus() { Reset(); }
  virtual void GainFocus() { }

  /// iEventHandler implementation
  struct CS_CSUTIL_EXPORT eiEventHandler : public iEventHandler
  {
    SCF_DECLARE_EMBEDDED_IBASE (csJoystickDriver);
    virtual bool HandleEvent(iEvent& e) { return scfParent->HandleEvent(e); }
  } scfiEventHandler;
  friend struct eiEventHandler;
};

#endif // __CS_CSINPUT_H__
