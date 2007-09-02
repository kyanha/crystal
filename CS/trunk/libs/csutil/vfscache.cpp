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

#include "cssysdef.h"
#include "csutil/util.h"
#include "csutil/vfscache.h"
#include "iutil/databuff.h"
#include "iutil/objreg.h"
#include "iutil/vfs.h"
#include "ivaria/reporter.h"

//------------------------------------------------------------------------

csVfsCacheManager::csVfsCacheManager (iObjectRegistry* object_reg,
	const char* vfsdir)
  : scfImplementationType (this), object_reg (object_reg),
  vfsdir (CS::StrDup (vfsdir)),
  vfs (0), readonly (false)
{
}

csVfsCacheManager::~csVfsCacheManager ()
{
  cs_free (vfsdir);
}

iVFS* csVfsCacheManager::GetVFS ()
{
  if (!vfs)
    vfs = csQueryRegistry<iVFS> (object_reg);
  return vfs;
}

void csVfsCacheManager::CacheName (csStringFast<512>& buf, const char* type,
	const char* scope, uint32 id)
{
  if (id == (uint32)~0)
  {
    if (scope == 0)
      buf.Format ("%s", type);
    else
      buf.Format ("%s/%s", type, scope);
  }
  else
    buf.Format ("%s/%s/%" PRIu32 , type, scope, id);
}

void csVfsCacheManager::SetCurrentType (const char* type)
{
  current_type = type;
}

void csVfsCacheManager::SetCurrentScope (const char* scope)
{
  current_scope = scope;
}

bool csVfsCacheManager::CacheData (const void* data, size_t size,
  	const char* type, const char* scope, uint32 id)
{
  if (readonly) return true;
  csStringFast<512> buf;
  GetVFS ()->PushDir ();
  GetVFS ()->ChDir (vfsdir);
  CacheName (buf, type ? type : current_type,
  	scope ? scope : current_scope, id);
  csRef<iFile> cf = GetVFS ()->Open (buf, VFS_FILE_WRITE);
  GetVFS ()->PopDir ();

  if (!cf)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.vfscachemgr.createfile",
	"Could not create file '%s' in VFS dir '%s'\n", buf.GetData(), vfsdir);
    return false;
  }

  size_t ws = cf->Write ((const char*)data, size);
  if (ws != size)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.vfscachemgr.writefile",
	"Could not write file '%s' in VFS dir '%s'\n", buf.GetData(), vfsdir);
    return false;
  }

  return true;
}

csPtr<iDataBuffer> csVfsCacheManager::ReadCache (
  	const char* type, const char* scope, uint32 id)
{
  csStringFast<512> buf;
  GetVFS ()->PushDir ();
  GetVFS ()->ChDir (vfsdir);
  CacheName (buf, type ? type : current_type,
  	scope ? scope : current_scope, id);
  csRef<iDataBuffer> data (GetVFS ()->ReadFile (buf, false));
  GetVFS ()->PopDir ();

  if (!data)
  {
    // This is not an error. It is possible that the item
    // simply hasn't been cached.
    return 0;
  }

  return csPtr<iDataBuffer> (data);
}

bool csVfsCacheManager::ClearCache (const char* type, const char* scope,
  	const uint32* id)
{
  // @@@ Not implemented yet.
  (void)type;
  (void)scope;
  (void)id;
  return false;
}

void csVfsCacheManager::Flush ()
{
  if (!readonly)
    GetVFS ()->Sync ();
}

