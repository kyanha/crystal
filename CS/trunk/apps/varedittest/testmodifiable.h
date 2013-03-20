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
#ifndef __TEST_MODIFIABLE_H__
#define __TEST_MODIFIABLE_H__

#include "cstool/modifiableimpl.h"
#include "csutil/refarr.h"

using namespace CS::Utility;

// Test modifiable object for the GUI generation.
// This first version is a full implementation
class RawModifiable
: public scfImplementation1<RawModifiable, CS::Utility::iModifiable>
{
public:
  RawModifiable (const char* name, const char* job, long itemCount);
  
  //-- CS::Utility::iModifiable
  //virtual const csStringID GetID () const;
  virtual csPtr<iModifiableDescription> GetDescription (iObjectRegistry* object_reg) const;

  virtual void GetParameterValue (size_t parameterIndex, csVariant& value) const;
  virtual bool SetParameterValue (size_t parameterIndex, const csVariant& value);

  virtual size_t GetTotalParameterCount () const;

  virtual void AddListener (iModifiableListener* listener);
  virtual void RemoveListener (iModifiableListener* listener);

private:
  csString name;
  csString job;
  long itemCount;
  bool awesome;
  float floatThingy;
  csVector2 position;
  csColor color;
  csString vfsFile;
  csString vfsDir;
  csString vfsPath;

  csRefArray<iModifiableListener> listeners;
};

// Some custom test enum
enum CustomType
{
  VALUE1 = 0,
  VALUE2,
  VALUE3,
  VALUE4,
};

// This second version of the modifiable object is using the dedicated macro's
class MacroModifiable
: public scfImplementation1<MacroModifiable, CS::Utility::iModifiable>
{
public:
  MacroModifiable (const char* name, const char* job, long itemCount)
    : scfImplementationType (this),
    name (name),
    job (job),
    itemCount (itemCount),
    awesome (false),
    floatThingy (123.55F),
    position (10, 12),
    color (0.5f, 0.2f, 0.2f),
    vfsFile (""), vfsDir (""), vfsPath (""),
    customType (VALUE2)
  {}
  
  //-- CS::Utility::iModifiable
  MODIF_DECLARE (11);

  MODIF_GETDESCRIPTION_BEGIN ("STATS_PLAYER", "Player stats");
  MODIF_GETDESCRIPTION (STRING, "NAME", "Name", "The dude's name");
  MODIF_GETDESCRIPTION (STRING, "JOB", "Job", "The dude's jawb");
  MODIF_GETDESCRIPTION (LONG, "ITEM", "Item count", "How many items this guy has");
  MODIF_GETDESCRIPTION (BOOL, "AWESOME", "Awesome", "Am I awesome, or what?");
  MODIF_GETDESCRIPTION_CHILD_BEGIN ("STATS_OTHER", "Other stats");
    MODIF_GETDESCRIPTION_C (FLOAT, "FLOATY", "FloatThingy", "Some float",
			    csConstraintBounded (csVariant (-100.0f), csVariant (500.0f)));
    MODIF_GETDESCRIPTION (VECTOR2, "POSITION", "Position", "Spatial position of the unit");
    MODIF_GETDESCRIPTION (COLOR, "COLOR", "Color", "My color");
    MODIF_GETDESCRIPTION_CHILD_BEGIN ("STATS_SUBOTHER", "Sub-other stats");
      MODIF_GETDESCRIPTION_C (STRING, "FILE", "VFS file", "A VFS file name", csConstraintVfsFile);
      MODIF_GETDESCRIPTION_C (STRING, "DIR", "VFS dir", "A VFS dir name", csConstraintVfsDir);
      MODIF_GETDESCRIPTION_C (STRING, "PATH", "VFS path", "A VFS path name", csConstraintVfsPath);
    MODIF_GETDESCRIPTION_CHILD_END ();
  MODIF_GETDESCRIPTION_CHILD_END ();
  MODIF_GETDESCRIPTION_CHILD_BEGIN ("STATS_OTHER2", "Other stats (cont'd)");
    MODIF_GETDESCRIPTION_CENUM_DECLARE ();
    MODIF_GETDESCRIPTION_CENUM_PUSH (VALUE1, "Value 1");
    MODIF_GETDESCRIPTION_CENUM_PUSH (VALUE2, "Value 2");
    MODIF_GETDESCRIPTION_CENUM_PUSH (VALUE3, "Value 3");
    MODIF_GETDESCRIPTION_CENUM_PUSH (VALUE4, "Value 4");
    MODIF_GETDESCRIPTION_CENUM (LONG, "ENUM", "Enumeration", "An enumeration of values of type 'int'");
  MODIF_GETDESCRIPTION_CHILD_END ();
  MODIF_GETDESCRIPTION_RESOURCE ("rawtest");
  MODIF_GETDESCRIPTION_END ();

  MODIF_GETPARAMETERVALUE_BEGIN ();
  MODIF_GETPARAMETERVALUE (0, String, name);
  MODIF_GETPARAMETERVALUE (1, String, job);
  MODIF_GETPARAMETERVALUE (2, Long, itemCount);
  MODIF_GETPARAMETERVALUE (3, Bool, awesome);
  MODIF_GETPARAMETERVALUE (4, Float, floatThingy);
  MODIF_GETPARAMETERVALUE (5, Vector2, position);
  MODIF_GETPARAMETERVALUE (6, Color, color);
  MODIF_GETPARAMETERVALUE (7, String, vfsFile);
  MODIF_GETPARAMETERVALUE (8, String, vfsDir);
  MODIF_GETPARAMETERVALUE (9, String, vfsPath);
  MODIF_GETPARAMETERVALUE (10, Long, customType);
  MODIF_GETPARAMETERVALUE_END ();

  MODIF_SETPARAMETERVALUE_BEGIN ();
  MODIF_SETPARAMETERVALUE (0, String, name);
  MODIF_SETPARAMETERVALUE (1, String, job);
  MODIF_SETPARAMETERVALUE (2, Long, itemCount);
  MODIF_SETPARAMETERVALUE (3, Bool, awesome);
  MODIF_SETPARAMETERVALUE (4, Float, floatThingy);
  MODIF_SETPARAMETERVALUE (5, Vector2, position);
  MODIF_SETPARAMETERVALUE (6, Color, color);
  MODIF_SETPARAMETERVALUE (7, String, vfsFile);
  MODIF_SETPARAMETERVALUE (8, String, vfsDir);
  MODIF_SETPARAMETERVALUE (9, String, vfsPath);
  MODIF_SETPARAMETERVALUE_ENUM (10, Long, customType, CustomType);
  MODIF_SETPARAMETERVALUE_END ();

private:
  csString name;
  csString job;
  long itemCount;
  bool awesome;
  float floatThingy;
  csVector2 position;
  csColor color;
  csString vfsFile;
  csString vfsDir;
  csString vfsPath;
  CustomType customType;
};

// This test class is to be used for the different enum types
class EnumModifiable
: public scfImplementation1<EnumModifiable, CS::Utility::iModifiable>
{
public:
  EnumModifiable ()
    : scfImplementationType (this),
    customType (VALUE2)
  {}
  
  //-- CS::Utility::iModifiable
  MODIF_DECLARE (1);

  MODIF_GETDESCRIPTION_BEGIN ("STATS", "Player stats");
  MODIF_GETDESCRIPTION_CENUM_DECLARE ();
  MODIF_GETDESCRIPTION_CENUM_PUSH (VALUE1, "Value 1");
  MODIF_GETDESCRIPTION_CENUM_PUSH (VALUE2, "Value 2");
  MODIF_GETDESCRIPTION_CENUM_PUSH (VALUE3, "Value 3");
  MODIF_GETDESCRIPTION_CENUM_PUSH (VALUE4, "Value 4");
  MODIF_GETDESCRIPTION_CENUM (LONG, "ENUM", "Enumeration", "An enumeration of values of type 'int'");
  MODIF_GETDESCRIPTION_END ();

  MODIF_GETPARAMETERVALUE_BEGIN ();
  MODIF_GETPARAMETERVALUE (0, Long, customType);
  MODIF_GETPARAMETERVALUE_END ();

  MODIF_SETPARAMETERVALUE_BEGIN ();
  MODIF_SETPARAMETERVALUE_ENUM (0, Long, customType, CustomType);
  MODIF_SETPARAMETERVALUE_END ();

private:
  CustomType customType;
};

// A parent class that will be inherited by a child one
class ParentModifiable
: public scfImplementation1<ParentModifiable, CS::Utility::iModifiable>
{
public:
  ParentModifiable ()
    : scfImplementationType (this),
    someValue (6),
    otherValue ("This is some text")
  {}
  
  //-- CS::Utility::iModifiable
  MODIF_DECLARE (2);

  MODIF_GETDESCRIPTION_BEGIN ("PARENT_PROPS", "Parent properties");
  MODIF_GETDESCRIPTION (LONG, "SOMEVALUE", "Some Value", "This is a parent property");
  MODIF_GETDESCRIPTION (STRING, "OTHERVALUE", "Other Value", "This is a parent property");
  MODIF_GETDESCRIPTION_RESOURCE ("parenttest");
  MODIF_GETDESCRIPTION_END ();

  MODIF_GETPARAMETERVALUE_BEGIN ();
  MODIF_GETPARAMETERVALUE (0, Long, someValue);
  MODIF_GETPARAMETERVALUE (1, String, otherValue);
  MODIF_GETPARAMETERVALUE_END ();

  MODIF_SETPARAMETERVALUE_BEGIN ();
  MODIF_SETPARAMETERVALUE (0, Long, someValue);
  MODIF_SETPARAMETERVALUE (1, String, otherValue);
  MODIF_SETPARAMETERVALUE_END ();

private:
  int someValue;
  csString otherValue;
};

// A second parent class to be inherited by a child one
class ParentModifiable2
: public scfImplementation1<ParentModifiable2, CS::Utility::iModifiable>
{
public:
  ParentModifiable2 ()
    : scfImplementationType (this),
    someBool (true)
  {}
  
  //-- CS::Utility::iModifiable
  MODIF_DECLARE (1);

  MODIF_GETDESCRIPTION_BEGIN ("PARENT2_PROPS", "Second parent properties");
  MODIF_GETDESCRIPTION (BOOL, "SOMEBOOL", "Some bool", "This is a parent property");
  MODIF_GETDESCRIPTION_END ();

  MODIF_GETPARAMETERVALUE_BEGIN ();
  MODIF_GETPARAMETERVALUE (0, Bool, someBool);
  MODIF_GETPARAMETERVALUE_END ();

  MODIF_SETPARAMETERVALUE_BEGIN ();
  MODIF_SETPARAMETERVALUE (0, Bool, someBool);
  MODIF_SETPARAMETERVALUE_END ();

private:
  bool someBool;
};

// A child class inheriting from parent modifiables
class ChildModifiable
//: public scfImplementationExt1<ChildModifiable, ParentModifiable, ParentModifiable2>
: public scfImplementationExt0<ChildModifiable, ParentModifiable>
{
public:
  ChildModifiable ()
    : scfImplementationType (this),
    childValue (2.5f),
    color (0.1f, 0.1f, 0.9f)
  {}
  
  //-- CS::Utility::iModifiable
  MODIF_DECLARE (2);

  MODIF_GETDESCRIPTION_BEGIN ("CHILD_PROPS", "Child properties");
  MODIF_GETDESCRIPTION (FLOAT, "CHILDVALUE", "Child Value", "This is a child property");
  MODIF_GETDESCRIPTION_CHILD_BEGIN ("CHILD_PROPS_OTHER", "Other child properties");
    MODIF_GETDESCRIPTION (COLOR, "COLOR", "Color", "This is a child property");
  MODIF_GETDESCRIPTION_CHILD_END ();
  MODIF_GETDESCRIPTION_PARENT (ParentModifiable);
  //MODIF_GETDESCRIPTION_PARENT (ParentModifiable2);
  MODIF_GETDESCRIPTION_RESOURCE ("childtest");
  MODIF_GETDESCRIPTION_END ();

  MODIF_GETPARAMETERVALUE_BEGIN ();
  MODIF_GETPARAMETERVALUE (0, Float, childValue);
  MODIF_GETPARAMETERVALUE (1, Color, color);
  MODIF_GETPARAMETERVALUE_PARENT_BEGIN ();
  MODIF_GETPARAMETERVALUE_PARENT (ParentModifiable);
  //MODIF_GETPARAMETERVALUE_PARENT (ParentModifiable2);
  MODIF_GETPARAMETERVALUE_PARENT_END ();

  MODIF_SETPARAMETERVALUE_BEGIN ();
  MODIF_SETPARAMETERVALUE (0, Float, childValue);
  MODIF_SETPARAMETERVALUE (1, Color, color);
  MODIF_SETPARAMETERVALUE_PARENT_BEGIN ();
  MODIF_SETPARAMETERVALUE_PARENT (ParentModifiable);
  //MODIF_SETPARAMETERVALUE_PARENT (ParentModifiable2);
  MODIF_SETPARAMETERVALUE_PARENT_END ();

private:
  float childValue;
  csColor color;
};

#endif // __TEST_MODIFIABLE_H__
