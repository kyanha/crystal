/*
    Copyright (C) 2002 by Jorrit Tyberghein

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

#define CS_CSUTIL_LIB
#include "cssysdef.h"
#include "qint.h"
#include "qsqrt.h"
#include "csutil/utiltest.h"
#include "csutil/xmltiny.h"
#include "iutil/document.h"
#include "csutil/scfstr.h"

SCF_IMPLEMENT_IBASE (csUtilDebugHelper)
  SCF_IMPLEMENTS_INTERFACE(iDebugHelper)
SCF_IMPLEMENT_IBASE_END

csUtilDebugHelper::csUtilDebugHelper ()
{
  SCF_CONSTRUCT_IBASE (0);
}

csUtilDebugHelper::~csUtilDebugHelper()
{
  SCF_DESTRUCT_IBASE ();
}

#define UTI_ASSERT(test,msg) \
  if (!(test)) \
  { \
    csString ss; \
    ss.Format ("csUtil failure (%d,%s): %s\n", int(__LINE__), \
    	#msg, #test); \
    str.Append (ss); \
    return csPtr<iString> (rc); \
  }

csPtr<iString> csUtilDebugHelper::UnitTest ()
{
  scfString* rc = new scfString ();
  //csString& str = rc->GetCsString ();

  //==========================================================================
  // Tests for XML parsing.
  //==========================================================================

  rc->DecRef ();
  return 0;
}

