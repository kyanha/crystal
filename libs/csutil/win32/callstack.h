/*
    Copyright (C) 2004 by Jorrit Tyberghein
	      (C) 2004 by Frank Richter

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

#ifndef __CS_UTIL_WIN32_CALLSTACK_H__
#define __CS_UTIL_WIN32_CALLSTACK_H__

#include "csutil/array.h"
#include "csutil/blockallocator.h"
#include "csutil/callstack.h"
#include "csutil/strset.h"

#include "csutil/win32/DbgHelpAPI.h"

class cswinCallStack : public csCallStack
{
public:
  static csStringSet strings;

  struct StackEntry
  {
    uint64 instrPtr;

    struct Param
    {
      csStringID name;
      uint32 value;
    };
    bool hasParams;
    csArray<Param> params;
  };

  csArray<StackEntry> entries;
  static BOOL CALLBACK EnumSymCallback (SYMBOL_INFO* pSymInfo, 
    ULONG SymbolSize, PVOID UserContext);

  void AddFrame (const STACKFRAME64& frame);

  virtual int GetEntryCount ();
  virtual bool GetFunctionName (int num, csString& str);
  virtual bool GetLineNumber (int num, csString& str);
  virtual bool GetParameters (int num, csString& str);
};

#endif // __CS_UTIL_WIN32_CALLSTACK_H__
