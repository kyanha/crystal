/*
    Crystal Space String interface
    Copyright (C) 1999 by Brandon Ehle (Azverkan)

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
#include "csutil/scfstr.h"

SCF_IMPLEMENT_IBASE (scfString)
  SCF_IMPLEMENTS_INTERFACE (iString)
SCF_IMPLEMENT_IBASE_END

void scfString::SetCapacity (size_t NewSize)
{ s.SetCapacity (NewSize); }

size_t scfString::GetCapacity() const
{ return s.GetCapacity(); }

void scfString::SetGrowsBy(size_t n)
{ s.SetGrowsBy(n); }

size_t scfString::GetGrowsBy() const
{ return s.GetGrowsBy(); }

void scfString::SetGrowsExponentially(bool b)
{ s.SetGrowsExponentially(b); }

bool scfString::GetGrowsExponentially() const
{ return s.GetGrowsExponentially(); }

void scfString::Truncate (size_t iPos)
{ s.Truncate (iPos); }

void scfString::Reclaim ()
{ s.Reclaim (); }

iString *scfString::Clone () const
{ return new scfString (*this); }

char *scfString::GetData () const
{ return s.GetData (); }

size_t scfString::Length () const
{ return s.Length (); }

char& scfString::operator [] (size_t iPos)
{ return s[iPos]; }

void scfString::SetAt (size_t iPos, char iChar)
{ s.SetAt (iPos, iChar); }

char scfString::GetAt (size_t iPos) const
{ return s.GetAt (iPos); }

void scfString::Insert (size_t iPos, iString *iStr)
{ s.Insert (iPos, iStr->GetData ()); }

void scfString::Overwrite (size_t iPos, iString *iStr)
{ s.Overwrite (iPos, iStr->GetData ()); }

iString &scfString::Append (const char *iStr, size_t iCount)
{
  s.Append (iStr, iCount);
  return *this;
}

iString &scfString::Append (const iString *iStr, size_t iCount)
{
  s.Append (iStr->GetData (), iCount);
  return *this;
}

void scfString::SubString (iString * sub, size_t start, size_t len) 
{
  csString tmp(sub->GetData());
  s.SubString(tmp, start, len);
  sub->Clear();
  sub->Append(tmp.GetData());
}

size_t scfString::FindFirst (const char c, size_t p) 
{
  return s.FindFirst(c, p);
}

size_t scfString::FindLast (const char c, size_t p)
{
  return s.FindLast(c, p);
}

void scfString::Format (const char* format, ...)
{
  va_list args;
  va_start (args, format);

  FormatV (format, args);

  va_end (args);
}

void scfString::FormatV (const char* format, va_list args)
{
  s.FormatV (format, args);
}

void scfString::Replace (const iString *iStr, size_t iCount)
{ s.Replace (iStr->GetData (), iCount); }

bool scfString::Compare (const iString *iStr) const
{ return s.Compare (iStr->GetData ()); }

bool scfString::CompareNoCase (const iString *iStr) const
{ return s.CompareNoCase (iStr->GetData ()); }
