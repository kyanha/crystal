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

#ifndef __CS_OBJPOOL_H__
#define __CS_OBJPOOL_H__

/**
 * This macro defines a 'pool' class for the given type. The pool class 
 * (descending from csObjectPool) can be used to create objects of the given 
 * type, but it will re-use older objects if possible to save time. For this 
 * reason, unused objects of the given type should not be deleted but given 
 * to the pool.
 */
#define CS_DECLARE_OBJECT_POOL(name,type)				\
class name : protected csObjectPool {					\
private:								\
  virtual void* CreateItem ()						\
  { return new type (); }						\
  virtual void FreeItem (void* o)					\
  { type *obj = (type*)o; delete obj; }					\
public:									\
  virtual ~name ()							\
  { for (int i=0; i<Num; i++) FreeItem (Objects[i]); }			\
  type *Alloc ()							\
  { return (type*)csObjectPool::Alloc (); }				\
  void Free (type *o)							\
  { csObjectPool::Free (o); }						\
};

/// Helper class for #DECLARE_OBJECT_POOL
class csObjectPool
{
protected:
  void** Objects;
  int Num, Max;

public:
  /// Create a new item
  virtual void* CreateItem () = 0;
  /// Delete an item
  virtual void FreeItem (void*) = 0;

  csObjectPool ()
  {
    Objects = new void* [16];
    Num = 0;
    Max = 16;
  }
  virtual ~csObjectPool ()
  {
    delete[] Objects;
  }
  /// Get an object from the pool
  void *Alloc ()
  {
    if (Num > 0) {
      Num--;
      return Objects [Num];
    } else {
      return CreateItem ();
    }
  }
  /// Give an object back to the pool
  void Free (void* o) {
    if (Num == Max) {
      void** old = Objects;
      Objects = new void* [Max + 16];
      memcpy (Objects, old, sizeof (void*) * Max);
      delete[] old;
      Max += 16;
    }
    Objects [Num] = o;
    Num++;
  }
};

#endif // __CS_OBJPOOL_H__
