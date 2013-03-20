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

#include "cssysdef.h"
#include "csutil/cscolor.h"
#include "csutil/event.h"
#include "csutil/ref.h"
#include "cstool/initapp.h"
#include "iutil/eventq.h"
#include "iutil/object.h"

#include "testmodifiable.h"

// -------------------------------- RawModifiable --------------------------------

RawModifiable::RawModifiable
(const char* name, const char* job, long itemCount) :
  scfImplementationType (this),
  name (name),
  job (job),
  itemCount (itemCount),
  awesome (false),
  floatThingy (123.55F),
  position (10, 12),
  color (0.5f, 0.2f, 0.2f),
  vfsFile (""), vfsDir (""), vfsPath ("")
{
}

/*
const csStringID RawModifiable::GetID () const
{
  csRef<iStringSet> strings =
    csQueryRegistryTagInterface<iStringSet> (object_reg, "crystalspace.shared.stringset");
  return strings->Request ("RawModifiable");
}
*/

csPtr<iModifiableDescription> RawModifiable::GetDescription (iObjectRegistry* object_reg) const 
{
  csBasicModifiableDescription* description = new csBasicModifiableDescription ("STATS_PLAYER", "Player stats");
  csRef<csBasicModifiableParameter> parameter;
  csRef<iModifiableConstraint> constraint;
  csRef<iStringSet> strings =
    csQueryRegistryTagInterface<iStringSet> (object_reg, "crystalspace.shared.stringset");
  csStringID id;
  csString label;

  label = "NAME";
  id = strings->Request (label);
  parameter.AttachNew (new csBasicModifiableParameter (CSVAR_STRING, id, label, "Name", "The dude's name"));
  description->Push (parameter);

  label = "JOB";
  id = strings->Request (label);
  parameter.AttachNew (new csBasicModifiableParameter (CSVAR_STRING, id, label, "Job", "The dude's jawb"));
  description->Push (parameter);

  label = "ITEMS";
  id = strings->Request (label);
  parameter.AttachNew (new csBasicModifiableParameter (CSVAR_LONG, id, label, "Item count", "How many items this guy has"));
  description->Push (parameter);

  label = "AWESOME";
  id = strings->Request (label);
  parameter.AttachNew (new csBasicModifiableParameter (CSVAR_BOOL, id, label, "Awesome", "Am I awesome, or what?"));
  description->Push (parameter);

  csRef<csBasicModifiableDescription> child;
  child.AttachNew (new csBasicModifiableDescription ("STATS_OTHER", "Other stats"));
  description->Push (child);

  label = "FLOATY";
  id = strings->Request (label);
  constraint.AttachNew (new csConstraintBounded (csVariant (-100.0f), csVariant (500.0f)));
  parameter.AttachNew (new csBasicModifiableParameter (CSVAR_FLOAT, id, label, "FloatThingy", "Some float", constraint));
  child->Push (parameter);

  label = "POSITION";
  id = strings->Request (label);
  parameter.AttachNew (new csBasicModifiableParameter (CSVAR_VECTOR2, id, label, "Position", "Spatial position of the guy"));
  child->Push (parameter);

  label = "COLOR";
  id = strings->Request (label);
  parameter.AttachNew (new csBasicModifiableParameter (CSVAR_COLOR, id, label, "Color", "My color"));
  child->Push (parameter);

  csRef<csBasicModifiableDescription> subChild;
  subChild.AttachNew (new csBasicModifiableDescription ("STATS_SUBOTHER", "Sub-other stats"));
  child->Push (subChild);

  label = "FILE";
  id = strings->Request (label);
  constraint.AttachNew (new csConstraintVfsFile);
  parameter.AttachNew (new csBasicModifiableParameter (CSVAR_STRING, id, label, "VFS file", "A VFS file name", constraint));
  subChild->Push (parameter);

  label = "DIR";
  id = strings->Request (label);
  constraint.AttachNew (new csConstraintVfsDir);
  parameter.AttachNew (new csBasicModifiableParameter (CSVAR_STRING, id, label, "VFS dir", "A VFS dir name", constraint));
  subChild->Push (parameter);

  label = "PATH";
  id = strings->Request (label);
  constraint.AttachNew (new csConstraintVfsPath);
  parameter.AttachNew (new csBasicModifiableParameter (CSVAR_STRING, id, label, "VFS path", "A VFS path name", constraint));
  subChild->Push (parameter);

  description->Push ("rawtest");

  return description;
}

void RawModifiable::GetParameterValue (size_t index, csVariant& value) const
{
  switch (index)
  {
  case 0:
    value.SetString (name);
    break;
  case 1:
    value.SetString (job);
    break;
  case 2:
    value.SetLong (itemCount);
    break;
  case 3:
    value.SetBool (awesome);
    break;
  case 4:
    value.SetFloat (floatThingy);
    break;
  case 5:
    value.SetVector2 (position);
    break;
  case 6:
    value.SetColor (color);
    break;
  case 7:
    value.SetString (vfsFile);
    break;
  case 8:
    value.SetString (vfsDir);
    break;
  case 9:
    value.SetString (vfsPath);
    break;
  default:
    //TODO: error message? return false?
    break;
  }
}

bool RawModifiable::SetParameterValue (size_t index, const csVariant& value)
{
  switch (index)
  {
  case 0:
    name = value.GetString ();
    break;
  case 1:
    job = value.GetString ();
    break;
  case 2:
    itemCount = value.GetLong ();
    break;
  case 3:
    awesome = value.GetBool ();
    break;
  case 4:
    floatThingy = value.GetFloat ();
    break;
  case 5:
    position = value.GetVector2 ();
    break;
  case 6:
    color = value.GetColor ();
    break;
  case 7:
    vfsFile = value.GetString ();
    break;
  case 8:
    vfsDir = value.GetString ();
    break;
  case 9:
    vfsPath = value.GetString ();
    break;
  default:
    //TODO: error message?
    return false;
  }

  // Broadcast a variant set event
  for (size_t i = 0; i < listeners.GetSize (); i++)
    listeners[i]->ValueChanged (this, index);

  return true;
}

size_t RawModifiable::GetTotalParameterCount () const
{
  return 10;
}

void RawModifiable::AddListener (iModifiableListener* listener)
{
  listeners.Push (listener);
}

void RawModifiable::RemoveListener (iModifiableListener* listener)
{
  listeners.Delete (listener);
}
