/*
    Crystal Space .INI file management
    Copyright (C) 1998,1999 by Andrew Zabolotny <bit@eltech.ru>

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

#include <string.h>
#include <malloc.h>
#include "sysdef.h"
#include "types.h"
#include "csutil/inifile.h"
#include "csutil/vfs.h"

csIniFile::csIniFile (csVFS *vfs, const char *fName, char iCommentChar)
{
  CommentChar = iCommentChar;
  Dirty = true;
  if (vfs)
  {
    size_t size;
    char *data = vfs->ReadFile (fName, size);
    if (data)
    {
      Load (data, size);
      CHK (delete [] data);
    } /* endif */
  } /* endif */
}
