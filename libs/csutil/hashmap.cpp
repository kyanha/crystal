/*
    Hash Map and hash support functions.
    Copyright (C) 2000 by Jorrit Tyberghein

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

#include <stdio.h>
#include <string.h>
#include "cssysdef.h"
#include "csutil/hashmap.h"

//-----------------------------------------------------------------------------

inline static csHashKey rotate_bits_right_3(csHashKey h)
{
  return (h >> 3) | (h << 29);
}

csHashKey csHashCompute(char const* s, int n)
{
  csHashKey h = 0;
  char const* slim = s + n;
  while (s < slim)
    h = rotate_bits_right_3(h) + *s++;
  return h;
}

csHashKey csHashCompute(char const* s)
{
  csHashKey h = 0;
  while (*s != 0)
    h = rotate_bits_right_3(h) + *s++;
  return h;
}

//-----------------------------------------------------------------------------

csGlobalHashIterator::csGlobalHashIterator (csHashMap *hm)
{
  hash = hm;
  bucket = 0;
  bucket_len = 0;
  element_index = 0;
  bucket_index = (uint32)-1;
  nbuckets = (uint32)hash->Buckets.Length();
  GotoNextElement ();
}

bool csGlobalHashIterator::HasNext ()
{
  return bucket != 0;
}

void csGlobalHashIterator::GotoNextElement ()
{
  element_index++;
  if (element_index >= (int)bucket_len)
  {
    // Next bucket.
    bucket_index++;
    while (bucket_index < nbuckets)
    {
      bucket = &hash->Buckets[bucket_index];
      bucket_len = bucket->Length ();
      if (bucket_len != 0)
      {
        element_index = 0;
	return;
      }
      bucket_index++;
    }
    bucket = 0;
  }
}

csHashObject csGlobalHashIterator::Next ()
{
  csHashObject obj = ((*bucket)[element_index]).object;
  GotoNextElement ();
  return obj;
}

void csGlobalHashIterator::DeleteNext ()
{
  // @@@ Not yet implemented.
}

//-----------------------------------------------------------------------------

csHashIterator::csHashIterator (csHashMap *hm, csHashKey hkey)
{
  uint32 idx = hkey % hm->NumBuckets;

  hash = hm;
  bucket = &(hm->Buckets[idx]);
  element_index = -1;
  current_index = -1;
  bucket_index = idx;
  key = hkey;
  GotoNextSameKey ();
}

bool csHashIterator::HasNext ()
{
  return bucket && bucket->Length () > 0;
}

void csHashIterator::GotoNextSameKey ()
{
  if (!bucket) return;
  element_index++;
  while (element_index < bucket->Length () &&
  	bucket->Get(element_index).key != key)
  {
    element_index++;
  }
  if (element_index >= bucket->Length ()) bucket = 0;
}

csHashObject csHashIterator::Next ()
{
  csHashObject obj = ((*bucket)[element_index]).object;
  current_index = element_index;
  GotoNextSameKey ();
  return obj;
}

void csHashIterator::DeleteNext ()
{
  // @@@ Not yet implemented.
}

//-----------------------------------------------------------------------------

uint32 csHashMap::prime_table[] =
{
  53,         97,         193,       389,       769, 
  1543,       3079,       6151,      12289,     24593,
  49157,      98317,      196613,    393241,    786433,
  1572869,    3145739,    6291469,   12582917,  25165843,
  50331653,   100663319,  201326611, 402653189, 805306457,
  1610612741, 0
};

csHashMap::csHashMap (uint32 size)
{
  NumBuckets = size;
  Buckets.SetLength (size, csHashBucket ());
  hash_elements = 0;
}

csHashMap::~csHashMap ()
{
  DeleteAll ();
}

uint32 csHashMap::FindLargerPrime (uint32 num)
{
  int i = 0;
  uint32 p = prime_table[i];
  while (p)
  {
    if (p > num) return p;
    i++;
    p = prime_table[i];
  }
  return 0;
}

void csHashMap::ChangeBuckets (uint32 newsize)
{
//printf ("Extend from %d to %d (hash_elements=%d)\n", NumBuckets, newsize, hash_elements);
  Buckets.SetLength (newsize, csHashBucket ());
  uint32 i;
  // Only go up to old size.
  uint32 old_NumBuckets = NumBuckets;
  NumBuckets = newsize;
  for (i = 0 ; i < old_NumBuckets ; i++)
  {
    csHashBucket& bucket = Buckets[i];
    if (bucket.Length () == 0) continue;
    csHashBucket b;
    bucket.TransferTo (b);
    int bucket_len = b.Length ();
    int j;
    for (j = 0 ; j < bucket_len ; j++)
    {
      csHashElement& el = b[j];
      uint32 new_idx =  el.key % NumBuckets;
      PutInternal (new_idx, el.key, el.object);
    }
  }
}

void csHashMap::PutInternal (uint32 idx, csHashKey key, csHashObject object)
{
  csHashBucket& bucket = Buckets[idx];
  int i = bucket.Push (csHashElement ());
  bucket[i].key = key;
  bucket[i].object = object;
}

void csHashMap::Put (csHashKey key, csHashObject object)
{
  uint32 idx = key % NumBuckets;
  PutInternal (idx, key, object);
  hash_elements++;
  if (NumBuckets < 20000UL && hash_elements > (int)(NumBuckets*4))
    ChangeBuckets (FindLargerPrime (NumBuckets*4));
}

csHashObject csHashMap::Get (csHashKey key) const
{
  uint32 idx = key % NumBuckets;
  const csHashBucket& bucket = Buckets[idx];
  int i;
  int len = bucket.Length ();
  for (i = 0 ; i < len ; i++)
  {
    const csHashElement& element = bucket[i];
    if (element.key == key) return element.object;
  }
  return 0;
}

void csHashMap::Delete (csHashKey key, csHashObject object)
{
  uint32 idx = key % NumBuckets;
  csHashBucket& bucket = Buckets[idx];
  int i;
  for (i = bucket.Length ()-1 ; i >= 0 ; i--)
  {
    csHashElement& element = bucket.Get (i);
    if (element.key == key && element.object == object)
    {
      bucket.DeleteIndex (i);
      hash_elements--;
      break;
    }
  }
}

void csHashMap::DeleteAll (csHashKey key)
{
  uint32 idx = key % NumBuckets;
  csHashBucket& bucket = Buckets[idx];
  uint32 i;
  for (i = bucket.Length () ; i-- > 0 ; )
  {
    csHashElement& element = bucket.Get (i);
    if (element.key == key)
    {
      bucket.DeleteIndex (i);
      hash_elements--;
    }
  }
}

void csHashMap::DeleteAll ()
{
  uint32 b;
  for (b = Buckets.Length () ; b-- > 0 ; )
  {
    Buckets[b].DeleteAll ();
  }
  hash_elements = 0;
}

void csHashMap::DumpStats ()
{
  uint32 i;
  int count_null = 0;
  int count_empty_but_not_null = 0;
  int count_elements = 0;
  int max_elements = 0;
  for (i = 0 ; i < NumBuckets ; i++)
  {
    csHashBucket& bucket = Buckets[i];
    if (bucket.Length () == 0)
    {
      count_empty_but_not_null++;
    }
    else
    {
      count_elements += bucket.Length ();
      if (bucket.Length () > max_elements)
	max_elements = bucket.Length ();
    }
  }
  printf ("buckets=%d null=%d empty=%d el=%d/%d max_el=%d avg_el=%g\n",
    NumBuckets, count_null, count_empty_but_not_null,
    count_elements, hash_elements, max_elements,
    float (count_elements) / float (NumBuckets));
}

//-----------------------------------------------------------------------------

csHashSet::csHashSet (uint32 size) : map (size)
{
}

void csHashSet::Add (csHashObject object)
{
  if (In (object)) return;
  AddNoTest (object);
}

void csHashSet::AddNoTest (csHashObject object)
{
  csHashKey key = (csHashKey)object;
  map.Put (key, object);
}

bool csHashSet::In (csHashObject object)
{
  csHashKey key = (csHashKey)object;
  csHashIterator it (&map, key);
  while (it.HasNext ())
  {
    csHashObject obj = it.Next ();
    if (obj == object)
      return true;
  }
  return false;
}

void csHashSet::DeleteAll ()
{
  map.DeleteAll ();
}

void csHashSet::Delete (csHashObject object)
{
  csHashKey key = (csHashKey)object;
  map.Delete (key, object);
}

