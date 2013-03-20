/*
  Copyright (C) 2012 Christian Van Brussel, Andrei Barsan

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#define CS_IMPLEMENT_PLATFORM_APPLICATION
/* This is needed due the WX headers using free () inline, but the opposing
 * malloc () is in the WX libs. */
#define CS_NO_MALLOC_OVERRIDE

#include "cssysdef.h"
#include "cstool/wxappargconvert.h"

#include "vareditapp.h"
#include "vareditframe.h"
#include "testmodifiable.h"

CS_IMPLEMENT_APPLICATION

IMPLEMENT_APP (VarEditTestApp);

using namespace std;

bool VarEditTestApp::OnInit ()
{
  wxInitAllImageHandlers ();

  // Create the Crystal Space environment
  CS::WX::AppArgConvert args (argc, argv);
  object_reg = csInitializer::CreateEnvironment (args.csArgc(), args.csArgv());

  // Load the needed plugins; so far, just the translator is needed
  if (!csInitializer::RequestPlugins (object_reg, 
				      CS_REQUEST_PLUGIN ("crystalspace.kernel.vfs", iVFS),
				      CS_REQUEST_END ))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.application.varedittest",
	      "Can't initialize plugins!");
    return false;
  }
  
  // Create the main frame
  ModifiableTestFrame* frame = new ModifiableTestFrame (object_reg);
  
  // Setup the frame's CS stuff
  frame->Initialize ();

  // Create a test modifiable listener
  csRef<CS::Utility::iModifiableListener> listener;
  listener.AttachNew (new ModifiableListener ());

  // Add some test modifiable objects to the varedittest in order to check its functionality. 
  csRef<CS::Utility::iModifiable> modifiable;

  modifiable.AttachNew (new RawModifiable ("Bob", "murderer", 11));
  modifiable->AddListener (listener);
  frame->AddModifiable (modifiable);

  modifiable.AttachNew (new RawModifiable ("Jake", "garbage man", 0));
  modifiable->AddListener (listener);
  frame->AddModifiable (modifiable);

  modifiable.AttachNew (new MacroModifiable ("Frodo", "part-time orc slayer", 2));
  modifiable->AddListener (listener);
  frame->AddModifiable (modifiable);

  modifiable.AttachNew (new EnumModifiable ());
  modifiable->AddListener (listener);
  frame->AddModifiable (modifiable);

  modifiable.AttachNew (new ChildModifiable ());
  modifiable->AddListener (listener);
  frame->AddModifiable (modifiable);

  frame->Show ();

  return true;
}

int VarEditTestApp::OnExit ()
{
  csInitializer::DestroyApplication (object_reg );
  return 0;
}

void ModifiableListener::ValueChanged (CS::Utility::iModifiable* modifiable, size_t parameterIndex)
{
  csVariant value;
  modifiable->GetParameterValue (parameterIndex, value);
  printf ("New value set for parameter %zu: %s\n", parameterIndex, value.Description ().GetData ());
}
