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

#ifndef __CS_CSTOOL_RBUFLOCK_H__
#define __CS_CSTOOL_RBUFLOCK_H__

/**\file
 * Helper class for convenient locking/unlocking of an iRenderBuffer.
 */

#include "csutil/ref.h"
#include "ivideo/rndbuf.h"

/**
 * Helper class for convenient locking/unlocking of an iRenderBuffer.
 * The contents can be accessed either directly or array-style in typed
 * way.
 */
template <class T>
class csRenderBufferLock
{
  csRef<iRenderBuffer> buffer;
  csRenderBufferLockType lockType;
  bool isLocked;
  T* lockBuf;
  
  csRenderBufferLock() {}
public:
  /**
   * Construct the helper.
   */
  csRenderBufferLock (iRenderBuffer* buf, 
    csRenderBufferLockType lock = CS_BUF_LOCK_NORMAL) : buffer(buf),
    lockType(lock), isLocked(false)
  {
  }
  
  /**
   * Destruct the helper. Automatically unlocks the buffer if it was locked.
   */
  ~csRenderBufferLock()
  {
    Unlock();
  }
  
  /// Lock the renderbuffer. Returns a pointer to the contained data.
  T* Lock ()
  {
    if (!isLocked)
    {
      lockBuf = (T*)buffer->Lock (lockType);
      isLocked = true;
    }
    return lockBuf;
  }
  
  /// Unlock the renderbuffer.
  void Unlock ()
  {
    if (isLocked)
    {
      buffer->Release();
      isLocked = false;
    }
  }
  
  /// Retrieve a pointer to the contained data.
  operator T* ()
  {
    return Lock();
  }

  /// Retrieve an item in the render buffer.
  T& operator [] (int n)
  {
    return *(Lock() + n);
  }

  /// Retrieve an item in the render buffer.
  T& Get (int n)
  {
    return *(Lock() + n);
  }
};

#endif // __CS_CSTOOL_RBUFLOCK_H__
