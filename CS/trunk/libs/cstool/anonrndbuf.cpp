/*
    Copyright (C) 2003 by Philipp Aumayr

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

#ifdef CS_USE_NEW_RENDERER

#include "cstool/anonrndbuf.h"
#include "ivideo/graph3d.h"
#include "ivideo/rndbuf.h"

csAnonRenderBufferManager::csAnonRenderBufferManager(iObjectRegistry *object_reg)
{
  this->object_reg = object_reg;
}

csAnonRenderBufferManager::~csAnonRenderBufferManager()
{
  for(int i = 0; i < anon_buffers.Length(); i++)
  {
    anon_buffers[i]->buf->Release();
    delete anon_buffers[i];
  }
}

iRenderBuffer * csAnonRenderBufferManager::GetRenderBuffer(csStringID name)
{
  for(int i = 0; i < anon_buffers.Length(); i++)
  {
    if(name == anon_buffers[i]->name)
      return anon_buffers[i]->buf;
  }
  return 0;
}

bool csAnonRenderBufferManager::AddRenderBuffer (const char *name, csRenderBufferComponentType component_type, int component_size, int numverts)
{
  csRef<iGraphics3D> g3d = CS_QUERY_REGISTRY (object_reg, iGraphics3D);

  csRef<iStringSet> strings = 
    CS_QUERY_REGISTRY_TAG_INTERFACE (object_reg, 
    "crystalspace.shared.stringset", iStringSet);
  
  anonbuffer *newrb = new anonbuffer();

  int size = 0;
  switch (component_type)
  {
  case CS_BUFCOMP_BYTE:
    size = sizeof(char);
    break;
  case CS_BUFCOMP_UNSIGNED_BYTE:
    size = sizeof(unsigned char);
    break;
  case CS_BUFCOMP_SHORT:
    size = sizeof(short);
    break;
  case CS_BUFCOMP_UNSIGNED_SHORT:
    size = sizeof(unsigned short);
    break;
  case CS_BUFCOMP_INT:
    size = sizeof(int);
    break;
  case CS_BUFCOMP_UNSIGNED_INT:
    size = sizeof(unsigned int);
    break;
  case CS_BUFCOMP_FLOAT:
    size = sizeof(float);
    break;
  case CS_BUFCOMP_DOUBLE:
    size = sizeof(double);
    break;
  }
  newrb->buf = g3d->CreateRenderBuffer (
        size*component_size*(numverts), CS_BUF_STATIC,
        component_type, component_size, false);
 
  newrb->name = strings->Request (name);
  newrb->size = component_size;
  anon_buffers.Push(newrb);
  return true;
}

bool csAnonRenderBufferManager::SetRenderBufferComponent (const char *name, int index, int component, float value)
{
  csRef<iStringSet> strings = 
    CS_QUERY_REGISTRY_TAG_INTERFACE (object_reg, 
    "crystalspace.shared.stringset", iStringSet);
  csStringID nameid = strings->Request (name);
  int i;
  for (i = 0; i < anon_buffers.Length(); i ++)
  {
    if (nameid == anon_buffers[i]->name) break;
  }
  if (i == anon_buffers.Length()) return false;

  float *buf = (float *)anon_buffers[i]->buf->Lock(CS_BUF_LOCK_NORMAL);
  buf[index * anon_buffers[i]->size + component] = value;
  anon_buffers[i]->buf->Release ();
  return true;
}

bool csAnonRenderBufferManager::SetRenderBufferComponent (const char *name, int index, int component, int value)
{
  csRef<iStringSet> strings = 
    CS_QUERY_REGISTRY_TAG_INTERFACE (object_reg, 
    "crystalspace.shared.stringset", iStringSet);
  csStringID nameid = strings->Request (name);
  int i;
  for (i = 0; i < anon_buffers.Length(); i ++)
  {
    if (nameid == anon_buffers[i]->name) break;
  }
  if (i == anon_buffers.Length()) return false;

  int *buf = (int *)anon_buffers[i]->buf->Lock(CS_BUF_LOCK_NORMAL);
  buf[index * anon_buffers[i]->size + component] = value;
  anon_buffers[i]->buf->Release ();
  return true;
}

bool csAnonRenderBufferManager::SetRenderBuffer (const char *name, float *value, int numverts)
{
  csRef<iStringSet> strings = 
    CS_QUERY_REGISTRY_TAG_INTERFACE (object_reg, 
    "crystalspace.shared.stringset", iStringSet);

  csStringID nameid = strings->Request (name);
  int i;
  for (i = 0; i < anon_buffers.Length(); i ++)
  {
    if (nameid == anon_buffers[i]->name) break;
  }
  if (i == anon_buffers.Length()) return false;

  anon_buffers[i]->buf->CopyToBuffer (value, sizeof (float) * anon_buffers[i]->size * numverts);
  return true;
}

bool csAnonRenderBufferManager::SetRenderBuffer (const char *name, int *value, int numverts)
{
  csRef<iStringSet> strings = 
    CS_QUERY_REGISTRY_TAG_INTERFACE (object_reg, 
    "crystalspace.shared.stringset", iStringSet);

  csStringID nameid = strings->Request (name);
  int i;
  for (i = 0; i < anon_buffers.Length(); i ++)
  {
    if (nameid == anon_buffers[i]->name) break;
  }
  if (i == anon_buffers.Length()) return false;

  anon_buffers[i]->buf->CopyToBuffer (value, sizeof (int) * anon_buffers[i]->size * numverts);
  return true;
}


#endif // CS_USE_NEW_RENDERER
