/*
    Copyright (C) 2000 by Eric Sunshine <sunshine@sunshineco.com>

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
#include "csutil/memfile.h"
#include <stdlib.h>

IMPLEMENT_IBASE (csMemFile)
  IMPLEMENTS_INTERFACE (iFile)
IMPLEMENT_IBASE_END

const char* csMemFile::GetName() { return "#csMemFile"; }
const char* csMemFile::GetData() const { return buffer; }
size_t csMemFile::GetSize() { return size; }
int csMemFile::GetStatus() { return VFS_STATUS_OK; }
bool csMemFile::AtEOF() { return (cursor >= size); }
size_t csMemFile::GetPos() { return cursor; }
void csMemFile::SetPos(size_t p) { cursor = p < size ? p : size; }

csMemFile::csMemFile() :
  disposition(DISPOSITION_DELETE), buffer(0), capacity(0), size(0), cursor(0)
  { CONSTRUCT_IBASE(0); }

csMemFile::csMemFile(const char* p, size_t s) :
  disposition(DISPOSITION_IGNORE), buffer((char*)p), capacity(s), size(s),
  cursor(0) { CONSTRUCT_IBASE(0); }

csMemFile::csMemFile(char* p, size_t s, Disposition d) :
  disposition(d), buffer(p), capacity(s), size(s), cursor(0)
  { CONSTRUCT_IBASE(0); }

csMemFile::~csMemFile()
{
  FreeBuffer();
}

void csMemFile::FreeBuffer()
{
  if (buffer != 0)
  {
    if (disposition == DISPOSITION_DELETE)
      delete[] buffer;
    else if (disposition == DISPOSITION_FREE)
      free(buffer);
  }
}

size_t csMemFile::Read(char* Data, size_t DataSize)
{
  const size_t remaining = cursor < size ? size - cursor : 0;
  const size_t nbytes = DataSize < remaining ? DataSize : remaining;
  if (nbytes != 0)
    memcpy(Data, buffer + cursor, nbytes);
  return nbytes;
}

size_t csMemFile::Write(const char* Data, size_t DataSize)
{
  if (DataSize != 0 && Data != 0)
  {
    size_t new_cursor = cursor + DataSize;
    if (capacity < new_cursor)
    {
      if (capacity == 0)
        capacity = 1024;
      while (capacity < new_cursor)
        capacity <<= 1;
      char* new_buffer = new char[capacity];
      if (buffer != 0)
        memcpy(new_buffer, buffer, size);
      FreeBuffer();
      buffer = new_buffer;
      disposition = DISPOSITION_DELETE;
    }
    memcpy(buffer + cursor, Data, DataSize);
    cursor = new_cursor;
    if (new_cursor > size)
      size = new_cursor;
    return DataSize;
  } else return 0;
}

char* csMemFile::GetAllData()
{
  char* data = buffer;
  if (buffer != 0 && disposition == DISPOSITION_FREE)
  {
    data = new char[size];
    memcpy(data, buffer, size);
    free(buffer);
  }
  disposition = DISPOSITION_DELETE;
  buffer = 0;
  capacity = 0;
  size = 0;
  cursor = 0;
  return data;
}
