/*
    Copyright (C) 2001 by Martin Geisse <mgeisse@gmx.net>

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

#ifndef __PRFXCFG_H__
#define __PRFXCFG_H__

#include "csutil/cfgfile.h"

/**
 * This is an implementation of iConfigFile that stores an ID string
 * at construction, which it then prepends to all config keys on read/write
 * operations.
 */
class csPrefixConfig : public csConfigFile
{
public:
  /// constructor
  csPrefixConfig(const char *fName, iVFS *vfs, const char *pref);
  /// destructor
  ~csPrefixConfig();

private:
  virtual bool LoadNow(const char *Filename, iVFS *vfs, bool overwrite);
  virtual bool SaveNow(const char *Filename, iVFS *vfs) const;

  char *Prefix;
  long PrefixLength;
};

#endif
