/*
    Copyright (C) 2001 by Jorrit Tyberghein

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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "cssysdef.h"
#include "csutil/util.h"
#include "csutil/objreg.h"
#include "csutil/refarr.h"

class csObjectRegistryIterator : public iObjectRegistryIterator
{
private:
  csRefArray<iBase> objects;
  csStringArray tags;
  int cur_idx;

public:
  csObjectRegistryIterator ();
  virtual ~csObjectRegistryIterator ();

  void Add (iBase* obj, char const* tag);

  SCF_DECLARE_IBASE;
  virtual bool Reset ();
  virtual const char* GetCurrentTag ();
  virtual bool HasNext ();
  virtual iBase* Next ();
};

SCF_IMPLEMENT_IBASE (csObjectRegistryIterator)
  SCF_IMPLEMENTS_INTERFACE (iObjectRegistryIterator)
SCF_IMPLEMENT_IBASE_END

csObjectRegistryIterator::csObjectRegistryIterator ()
{
  SCF_CONSTRUCT_IBASE (0);
  cur_idx = 0;
}

csObjectRegistryIterator::~csObjectRegistryIterator ()
{
  for (int i = objects.Length() - 1; i >= 0; i--)
  {
    // Take special care to ensure that this object is no longer on the list
    // before calling DecRef(), since we don't want some other object asking
    // for it during its own destruction.
    objects.DeleteIndex (i);
    tags.DeleteIndex (i);
  }
  SCF_DESTRUCT_IBASE ();
}

bool csObjectRegistryIterator::Reset ()
{
  cur_idx = 0;
  if (objects.Length () <= 0) return false;
  return true;
}

const char* csObjectRegistryIterator::GetCurrentTag ()
{
  if (cur_idx >= objects.Length ()) return 0;
  return tags[cur_idx];
}

bool csObjectRegistryIterator::HasNext ()
{
  if (cur_idx >= objects.Length ()) return false;
  return true;
}

iBase* csObjectRegistryIterator::Next ()
{
  cur_idx++;
  if (cur_idx >= objects.Length ()) return 0;
  return objects[cur_idx];
}

void csObjectRegistryIterator::Add (iBase* obj, char const* tag)
{
  objects.Push (obj);
  tags.Push ((char*)tag);
}

//-------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csObjectRegistry)
  SCF_IMPLEMENTS_INTERFACE (iObjectRegistry)
SCF_IMPLEMENT_IBASE_END

csObjectRegistry::csObjectRegistry () : clearing (false)
{
  SCF_CONSTRUCT_IBASE (0);
  // We need a recursive mutex.
  mutex = csMutex::Create (true);
}

csObjectRegistry::~csObjectRegistry ()
{
  CS_ASSERT (registry.Length () == 0);
  CS_ASSERT (tags.Length () == 0);
  CS_ASSERT (clearing == false);
  SCF_DESTRUCT_IBASE ();
}

void csObjectRegistry::Clear ()
{
  csScopedMutexLock lock (mutex);

  clearing = true;
  int i;
  for (i = registry.Length() - 1; i >= 0; i--)
  {
    // Take special care to ensure that this object is no longer on the list
    // before calling DecRef(), since we don't want some other object asking
    // for it during its own destruction.
    iBase* b = registry[i];
//printf ("Unregister %08lx/'%s' ref=%d\n", b, t, b->GetRefCount ()); fflush (stdout);
    registry.DeleteIndex (i); // Remove from list before DecRef().
    tags.DeleteIndex (i);
    b->DecRef ();
  }
  clearing = false;
}

bool csObjectRegistry::Register (iBase* obj, char const* tag)
{
  csScopedMutexLock lock (mutex);

  CS_ASSERT (registry.Length () == tags.Length ());
  if (!clearing)
  {
    // Don't allow adding an object with an already existing tag.
    if (tag)
    {
      iBase* o = Get (tag);
      if (o)
      {
        // DecRef() o because Get() already increffed it.
        o->DecRef ();
        return false;
      }
    }

    obj->IncRef ();
    registry.Push (obj);
    tags.Push ((char*)tag);
    return true;
  }
  return false;
}

void csObjectRegistry::Unregister (iBase* obj, char const* tag)
{
  csScopedMutexLock lock (mutex);

  CS_ASSERT (registry.Length () == tags.Length ());
  if (!clearing)
  {
    int i;
    for (i = registry.Length() - 1; i >= 0; i--)
    {
      iBase* b = registry[i];
      if (b == obj)
      {
        const char* t = tags[i];
        if ((t == 0 && tag == 0) || (t != 0 && tag != 0 && !strcmp (tag, t)))
        {
	  registry.DeleteIndex (i);
	  tags.DeleteIndex (i);
          b->DecRef ();
	  if (tag != 0) // For a tagged object, we're done.
	    break;
        }
      }
    }
  }
}

iBase* csObjectRegistry::Get (char const* tag)
{
  csScopedMutexLock lock (mutex);

  CS_ASSERT (registry.Length () == tags.Length ());
  int i;
  for (i = registry.Length() - 1; i >= 0; i--)
  {
    const char* t = tags[i];
    if (t && !strcmp (tag, t))
    {
      iBase* b = registry[i];
      b->IncRef ();
      return b;
    }
  }
  return 0;
}

iBase* csObjectRegistry::Get (char const* tag, scfInterfaceID id, int version)
{
  csScopedMutexLock lock (mutex);

  CS_ASSERT (registry.Length () == tags.Length ());
  int i;
  for (i = registry.Length() - 1; i >= 0; i--)
  {
    const char* t = tags[i];
    if (t && !strcmp (tag, t))
    {
      iBase* b = registry[i];
      iBase* interf = (iBase*)(b->QueryInterface (id, version));
      if (!interf)
      {
        printf ("WARNING! Suspicious: object with tag '%s' doesn't implement interface '%s'!\n", t, t);
	fflush (stdout);
	return 0;
      }
      return interf;
    }
  }
  return 0;
}

csPtr<iObjectRegistryIterator> csObjectRegistry::Get (
	scfInterfaceID id, int version)
{
  csObjectRegistryIterator* iterator = new csObjectRegistryIterator ();
  int i;
  csScopedMutexLock lock (mutex);
  for (i = registry.Length() - 1; i >= 0; i--)
  {
    iBase* b = registry[i];
    iBase* interf = (iBase*)(b->QueryInterface (id, version));
    if (interf)
    {
      const char* t = tags[i];
      iterator->Add (interf, t);
      interf->DecRef ();
    }
  }
  return csPtr<iObjectRegistryIterator> (iterator);
}

csPtr<iObjectRegistryIterator> csObjectRegistry::Get ()
{
  csObjectRegistryIterator* iterator = new csObjectRegistryIterator ();
  int i;
  csScopedMutexLock lock (mutex);
  for (i = registry.Length() - 1; i >= 0; i--)
  {
    iBase* b = registry[i];
    const char* t = tags[i];
    iterator->Add (b, t);
  }
  return csPtr<iObjectRegistryIterator> (iterator);
}

