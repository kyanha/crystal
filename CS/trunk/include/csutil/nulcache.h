/*
    Copyright (C) 2002 by Frank Richter

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

#ifndef __CS_UTIL_NULCACHE_H__
#define __CS_UTIL_NULCACHE_H__

/**\file
 * 'Null' Cache Manager
 */
#include "csextern.h"
#include "iutil/cache.h"

/**
 * This is a general cache that doesn't cache anything at all.
 */
class CS_CSUTIL_EXPORT csNullCacheManager : public iCacheManager
{
public:
  /**
   * Does nothing.
   */
  csNullCacheManager ();

  virtual ~csNullCacheManager ();

  SCF_DECLARE_IBASE;

  /**
   * Does nothing.
   */
  virtual void SetCurrentType (const char* type);
  /**
   * Always returns 0.
   */
  virtual const char* GetCurrentType () const { return 0; }
  /**
   * Does nothing. 
   */
  virtual void SetCurrentScope (const char* scope);
  /**
   * Always returns 0.
   */
  virtual const char* GetCurrentScope () const { return 0; }
  /**
   * Always returns false.
   */
  virtual bool CacheData (void* data, uint32 size,
  	const char* type, const char* scope, uint32 id);
  /**
   * Always returns 0.
   */
  virtual csPtr<iDataBuffer> ReadCache (const char* type, const char* scope,
  	uint32 id);
  /**
   * Does nothing. 
   */
  virtual bool ClearCache (const char* type = 0, const char* scope = 0,
  	const uint32* id = 0);
};

#endif // __CS_UTIL_NULCACHE_H__

