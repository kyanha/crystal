/*
    Copyright (C) 2002 by Manjunath Sripadarao

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
#include "bufio.h"

DataBuffer::DataBuffer(char *d, int sz) 
{
  data = new char[sz + 1];
  memcpy(data, d, sz);
  data[sz + 1] = '\0';
  buffSize = sz;
  position = 0;
  maxline = 0;
  while (position < buffSize) {
    if((strchr((data + position), '\n') - (data + position)) > maxline)
      maxline = strchr((data + position), '\n') - (data + position);
    position++;
  }
  position = 0;
}

DataBuffer::~DataBuffer()
{
}

bool DataBuffer::GetData (char *d, int len) 
{
  if(eof())
    return false;

  if (position + len >= buffSize) {
    position = buffSize;
    memcpy(d, (data + position), len);
    return true;
  } else {
    memcpy(d, (data + position), len);
    position += len;
    return false;
  }
}

bool DataBuffer::GetLine(char *line) 
{
  int length = 0;
  char *ptr;
  if(!eof()) {
    if((ptr = strchr((data + position), '\n')) != NULL) {
      length = ptr - (data + position);
    } else {
      length = buffSize - position;
    }
    memcpy(line, (data + position), length);
    line[length] ='\0';
    position += length + 1;
    return true;
  } else {
    line = NULL;
    return false;
  }
}

