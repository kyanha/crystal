/*
    Copyright (C) 2002 by Norman Kramer
  
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
#include "csver.h"
#include "csjoylin.h"
#include "ivaria/reporter.h"
#include "csutil/csstring.h"
#include "csutil/array.h"

#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>

#define CS_LINUX_JOYSTICK_CFG "/config/joystick.cfg"
#define CS_LINUX_JOYSTICK_KEY "Device.Joystick."

#define CS_LINUX_JOYSTICK_OLD_EVENTS // the values of the first two axis are sent only

CS_IMPLEMENT_PLUGIN;

SCF_IMPLEMENT_IBASE (csLinuxJoystick)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iEventPlug)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iEventHandler)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csLinuxJoystick::eiEventPlug)
  SCF_IMPLEMENTS_INTERFACE (iEventPlug)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csLinuxJoystick::eiEventHandler)
  SCF_IMPLEMENTS_INTERFACE (iEventHandler)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csLinuxJoystick::csLinuxJoystick (iBase *parent):
  object_reg(0),
  joystick(0),
  nJoy(0),
  bHooked(false),
  EventOutlet(0)
{
  SCF_CONSTRUCT_IBASE(parent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiEventPlug);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiEventHandler);
}

csLinuxJoystick::~csLinuxJoystick ()
{
  Close ();
}

bool csLinuxJoystick::Initialize (iObjectRegistry *oreg)
{
  object_reg = oreg;
  return Init ();
}

bool csLinuxJoystick::HandleEvent (iEvent &)
{
  struct js_event js;

  for (int i=0; i < nJoy; i++)
  {
    while (read (joystick [i].fd, &js, sizeof(struct js_event)) == sizeof(struct js_event)) 
    {
      switch (js.type & ~JS_EVENT_INIT) 
      {
      case JS_EVENT_BUTTON:
        //        DEBUG_BREAK;
        joystick[i].button[js.number] = js.value;
        EventOutlet->Joystick (i, js.number+1, js.value, -1, 0);
        break;
      case JS_EVENT_AXIS:
        joystick[i].axis[js.number] = js.value;
#ifdef CS_LINUX_JOYSTICK_OLD_EVENTS
        if (js.number < 2)
          EventOutlet->Joystick (i, 0, 0, joystick[i].axis[0], (joystick[i].nAxes > 1 ? joystick[i].axis[1] : 0));
#else
        EventOutlet->Joystick (i, 0, 0, js.number, js.value);
#endif
        break;
      }
    }
  }
  return false;
}

bool csLinuxJoystick::Init ()
{
  // read list of devices
  config.AddConfig (object_reg, CS_LINUX_JOYSTICK_CFG);

  // create the key
  csString key(CS_LINUX_JOYSTICK_KEY);
  key += CS_PLATFORM_NAME;
  key += ".";
  key += CS_COMPILER_NAME;
  key += ".";

  csRef<iConfigIterator> it (config->Enumerate (CS_LINUX_JOYSTICK_KEY));

  csArray<int> fds;
  int fd;

  nJoy=0;
  bHooked = false;
  EventOutlet = 0;

  while (it->Next ())
  {
    if ((fd = open (it->GetStr (), O_RDONLY)) < 0) 
    {
      Report (CS_REPORTER_SEVERITY_NOTIFY, 
              "Failed to open joystick device %s - error: %s\n",
              it->GetStr (),
              strerror (errno));
    }
    else
      nJoy++;
    fds.Push(fd);
  }

  if (nJoy)
  {
    delete [] joystick;
    joystick = new joydata[nJoy];
    
    csArray<int>::Iterator i = fds.GetIterator();
    int n = 0;
    while (i.HasNext())
    {
      fd = i.Next();
      if (fd > -1)
      {
	unsigned char axes = 2;
	unsigned char buttons = 2;
	int version = 0x000800;
	char name[128] = "Unknown";

	ioctl (fd, JSIOCGVERSION, &version);
	ioctl (fd, JSIOCGAXES, &axes);
	ioctl (fd, JSIOCGBUTTONS, &buttons);
	ioctl (fd, JSIOCGNAME(128), name);

	Report (CS_REPORTER_SEVERITY_NOTIFY,
                "Joystick number %d (%s) has %d axes and %d buttons. Driver version is %d.%d.%d.\n",
		n, name, axes, buttons, version >> 16, (version >> 8) & 0xff, version & 0xff);
        joystick[n].number = n;
        joystick[n].fd = fd;
        joystick[n].nButtons = buttons;
        joystick[n].nAxes = axes;
        joystick[n].axis = new int16[axes];
        joystick[n].button = new int16[buttons];

        // make device non blocking
        fcntl(fd, F_SETFL, O_NONBLOCK);
        n++;
      }
    }

    // hook into eventqueue
    csRef<iEventQueue> eq (CS_QUERY_REGISTRY(object_reg, iEventQueue));
    if (eq != 0)
    {
      eq->RegisterListener (&scfiEventHandler, CSMASK_Nothing);
      EventOutlet = eq->CreateEventOutlet (&scfiEventPlug);
      bHooked = true;
    }
  }
  else
  {
    Report (CS_REPORTER_SEVERITY_NOTIFY,
            "No operable joystick found\n");
  }

  return EventOutlet.IsValid();
}

bool csLinuxJoystick::Close ()
{
  if (bHooked)
  {
    csRef<iEventQueue> eq (CS_QUERY_REGISTRY(object_reg, iEventQueue));
    if (eq != 0)
      eq->RemoveListener (&scfiEventHandler);
    bHooked = false;
  }

  EventOutlet = 0;

  for (int i=0; i<nJoy; i++)
    close (joystick[i].fd);

  delete [] joystick;
  joystick = 0;
  nJoy = 0;

  return true;
}

void csLinuxJoystick::Report (int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csRef<iReporter> rep (CS_QUERY_REGISTRY (object_reg, iReporter));
  if (rep)
    rep->ReportV (severity, "crystalspace.device.joystick.linux", msg, arg);
  else
  {
    csPrintfV (msg, arg);
    csPrintf ("\n");
  }
  va_end (arg);
}

SCF_IMPLEMENT_FACTORY (csLinuxJoystick);

