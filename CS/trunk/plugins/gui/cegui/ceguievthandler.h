/*
    Copyright (C) 2005 Dan Hardfeldt and Seth Yastrov

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

#ifndef _CS_CEGUI_EVENTHANDLER_H_
#define _CS_CEGUI_EVENTHANDLER_H_

/**\file 
*/
/**
* \addtogroup CEGUI
* @{ */

#include "iutil/csinput.h"
#include "csutil/csbaseeventh.h"
#include "csutil/set.h"

struct iObjectRegistry;

class csCEGUIRenderer;

/// Handles resize events and injects mouse and keyboard into CEGUI.
class csCEGUIEventHandler : public csBaseEventHandler
{
public:
  /// Constructor.
  csCEGUIEventHandler (iObjectRegistry*, csCEGUIRenderer*);

  /// Destructor.
  ~csCEGUIEventHandler ();

  /// Initialize the event handler. This sets up the event queue.
  bool Initialize ();

  /// Handle unhandled events (like broadcast messages).
  bool OnUnhandledEvent (iEvent &event);

  /// Handle mouse down events.
  bool OnMouseDown (iEvent &event);

  /// Handle mouse move events.
  bool OnMouseMove (iEvent &event);

  /// Handle mouse up events.
  bool OnMouseUp (iEvent &event);

  /// Handle keyboard events.
  bool OnKeyboard (iEvent &event);

private:
  iObjectRegistry *obj_reg;
  csCEGUIRenderer* renderer;
  csRef<iKeyComposer> compose;
  csSet<utf32_char> caughtCharKeys;

  static CEGUI::MouseButton CSMBtoCEMB (uint button);
};
#endif // _CS_CEGUI_EVENTHANDLER_H_
