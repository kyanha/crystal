/*
    Copyright (C) 2005 by Jorrit Tyberghein
	      (C) 2005 by Frank Richter

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

#ifndef __CS_CSUTIL_FORMATTER_H__
#define __CS_CSUTIL_FORMATTER_H__

/**\file
 */

#include "csutil/csuctransform.h"
#include "csutil/dirtyaccessarray.h"
#include "csgeom/math.h"

/**\addtogroup csutil
 * @{ */

/**
 * Standard format string source for csPrintfFormatter.
 * \remarks Uses csUnicodeTransform::Decode(), so it supports
 *  utd8_char, utf16_char, utf32_char and wchar_t strings.
 */
template <class T>
class csFmtDefaultReader
{
  const T* str;
  const T* const startStr;
  size_t len;
  const size_t startLen;
public:
  /// Init with a string and a length
  csFmtDefaultReader (const T* string, size_t length) : startStr (string), 
    startLen (length) { Reset(); }
  /// Get next character. Returns false if end of input was reached.
  bool GetNext (utf32_char& ch) 
  {
    int n = csUnicodeTransform::Decode (str, len, ch);
    if (n == 0) return false;
    str += (size_t)n;
    len -= (size_t)n;
    return true;
  }
  /// Rewind
  void Reset() { str = startStr; len = startLen; }
  /// Get position relative to beginning
  size_t GetPosition() const { return str - startStr; }
};


/**
 * Standard formatted string destination for csPrintfFormatter.
 * \remarks Uses csUnicodeTransform::Decode(), so it supports
 *  utd8_char, utf16_char, utf32_char and wchar_t strings.
 */
template <class T>
class csFmtDefaultWriter
{
  T* dest;
  size_t size;
  size_t total;
public:
  /// Init with a string and a maximum length
  csFmtDefaultWriter (T* dest, size_t size) : dest (dest), size (size), 
    total (0) {}
  /// Write a char.
  void Put (utf32_char ch) 
  { 
    size_t n = (size_t)csUnicodeTransform::Encode (ch, dest, size);
    total += n;
    n = csMin (size, n);
    dest += n;
    size -= n;
  }
  /**
   * Get number of chars that have been sent to the write (all, not only 
   * those that did fit into the buffer).
   */
  size_t GetTotal() const { return total; }
};

/**
 * Templated class for printf()-style string formatting.
 * Treader provides the format string, Twriter receives the output.
 */
template <class Twriter, class Treader>
class csPrintfFormatter
{
  class Scratch : public csDirtyAccessArray<utf32_char>
  {
  public:
    void WriteTo (Twriter& writer, size_t offset = 0, size_t len = (size_t)~0)
    {
      const size_t n = MIN (len, Length());
      for (size_t i = offset; i < n; i++) writer.Put (Get (i));
    }
  };
  Scratch scratch;

  /// Parameter pulled from stack
  struct FmtParam
  {
    union
    {
      int vInt;
      void* vPtr;
      long vLong;
      longlong vLL;
      double vDbl;
      long double vLongDbl;
      size_t vSzT;
      ptrdiff_t vPDT;
      intmax_t vIMT;
    };
  };
  enum Conversion
  {
    convBogus = 0,
    convNone,
    convInt,
    convOctal,
    convUint,
    convHex,
    convFloatFix,
    convFloatExp,
    convFloatGeneral,
    convFloatHex,
    convChar,
    convStr,
    convPtr,
    convGetNum,
    convErrno
  };
  enum Type
  {
    typeNone = 0,
    typeChar,
    typeShort,
    typeIntmax,
    typeLong,
    typeLongLong,
    typePtrDiffT,
    typeSizeT
  };
  /// Parsed format specifier
  struct FormatSpec
  {
    size_t copyRun;
    size_t fmtSkip;

    int paramIdx;
    bool leftJustify;
    bool plusSign;
    bool spacePrefix;
    bool basePrefix;
    //bool grouping;
    bool padZero;
    int width;
    int precision;
    Conversion conversion;
    bool uppercase;
    Type type;

    FormatSpec() { Reset(); }
    void Reset () 
    { 
      memset (this, 0, sizeof (*this)); 
      precision = -1;
    }
  };
  csArray<FormatSpec> formatSpecs;
  csArray<FmtParam> params;
  Treader& reader;

  struct SpecParseState
  {
    utf32_char ch;
    FormatSpec currentFormat;
    size_t charRun;
    int paramIdx;
    size_t fmtBegin;

    SpecParseState() : paramIdx(0) {}
    void Reset()
    {
      charRun = 0;
      currentFormat.Reset();
    }
  };

  bool ParseFlag (SpecParseState& state)
  {
    switch (state.ch)
    {
      case '-':
	{
	  state.currentFormat.leftJustify = true;
	  return true;
	}
      case '+':
	{
	  state.currentFormat.plusSign = true;
	  return true;
	}
      case ' ':
	{
	  state.currentFormat.spacePrefix = true;
	  return true;
	}
      case '#':
	{
	  state.currentFormat.basePrefix = true;
	  return true;
	}
      case '0':
	{
	  state.currentFormat.padZero = true;
	  return true;
	}
      case '\'':
	{
	  return true;
	}
    }
    return false;
  }
  
  bool ParseType (SpecParseState& state)
  {
    switch (state.ch)
    {
      case 'h':
	{
	  if (state.currentFormat.type == typeNone)
	    state.currentFormat.type = typeShort;
	  else if (state.currentFormat.type == typeShort)
	    state.currentFormat.type = typeChar;
	  else
	    return false;
	  return true;
	}
      case 'j':
	{
	  if (state.currentFormat.type == typeNone)
	    state.currentFormat.type = typeIntmax;
	  else
	    return false;
	  return true;
	}
      case 'l':
	{
	  if (state.currentFormat.type == typeNone)
	    state.currentFormat.type = typeLong;
	  else if (state.currentFormat.type == typeLong)
	    state.currentFormat.type = typeLongLong;
	  else
	    return false;
	  return true;
	}
      case 'L':
      case 'q':
	{
	  if (state.currentFormat.type == typeNone)
	    state.currentFormat.type = typeLongLong;
	  else
	    return false;
	  return true;
	}
      case 't':
	{
	  if (state.currentFormat.type == typeNone)
	    state.currentFormat.type = typePtrDiffT;
	  else
	    return false;
	  return true;
	}
      case 'z':
	{
	  if (state.currentFormat.type == typeNone)
	    state.currentFormat.type = typeSizeT;
	  else
	    return false;
	  return true;
	}
    }
    return false;
  }

  bool ParseConversion (SpecParseState& state)
  {
    switch (state.ch)
    {
      case '%':
	{
	  const size_t fmtLen = (reader.GetPosition() - 1) - state.fmtBegin;
	  if (fmtLen == 1)
	  {
	    state.currentFormat.conversion = convNone;
	    state.fmtBegin++;
	    state.currentFormat.copyRun++;
	    return true;
	  }
	  break;
	}
      case 'd':
      case 'i':
	{
	  state.currentFormat.conversion = convInt;
	  return true;
	}
      case 'o':
	{
	  state.currentFormat.conversion = convOctal;
	  return true;
	}
      case 'u':
	{
	  state.currentFormat.conversion = convUint;
	  return true;
	}
      case 'x':
      case 'X':
	{
	  state.currentFormat.conversion = convHex;
	  state.currentFormat.uppercase = (state.ch == 'X');
	  return true;
	}
      case 'f':
	{
	  state.currentFormat.conversion = convFloatFix;
	  return true;
	}
      case 'e':
      case 'E':
	{
	  state.currentFormat.conversion = convFloatExp;
	  state.currentFormat.uppercase = (state.ch == 'E');
	  return true;
	}
      case 'g':
      case 'G':
	{
	  state.currentFormat.conversion = convFloatGeneral;
	  state.currentFormat.uppercase = (state.ch == 'G');
	  return true;
	}
      case 'a':
      case 'A':
	{
	  state.currentFormat.conversion = convFloatHex;
	  state.currentFormat.uppercase = (state.ch == 'A');
	  return true;
	}
      case 'c':
	{
	  state.currentFormat.conversion = convChar;
	  return true;
	}
      case 'C':
	{
	  state.currentFormat.conversion = convChar;
	  state.currentFormat.type = typeLong;
	  return true;
	}
      case 's':
	{
	  state.currentFormat.conversion = convStr;
	  return true;
	}
      case 'S':
	{
	  state.currentFormat.conversion = convStr;
	  state.currentFormat.type = typeLong;
	  return true;
	}
      case 'p':
	{
	  state.currentFormat.conversion = convPtr;
	  return true;
	}
      case 'n':
	{
	  state.currentFormat.conversion = convGetNum;
	  return true;
	}
      case 'm':
	{
	  state.currentFormat.conversion = convErrno;
	  return true;
	}
    }
    return false;
  }

  void ParseSpec ()
  {
    enum {
      scanFormat,
      formatParamFlagsWidthPrecTypeConversion,
      formatFlagsWidthPrecTypeConversion,
      formatParamWidth,
      formatPrecTypeConversion,
      formatTypeConversion
    } parseState = scanFormat;

    // Collect positions of state specifiers from format string
    SpecParseState state;
    state.Reset();
    while (reader.GetNext (state.ch))
    {
      switch (parseState)
      {
	case scanFormat:
	  {
	    // Check for a % sign
	    if (state.ch == '%')
	    {
	      parseState = formatParamFlagsWidthPrecTypeConversion;
	      state.fmtBegin = reader.GetPosition() - 1;
	      state.currentFormat.copyRun = state.charRun;
	    }
	    else
	      state.charRun++;
	  }
	  break;
	case formatParamFlagsWidthPrecTypeConversion:
	  // Check for start of width or param index
	  if ((state.ch >= '1') && (state.ch <= '9'))
	  {
	    state.currentFormat.width = state.ch - '0';
	    parseState = formatParamWidth;
	    break;
	  }
	  // Param delimiter
	  else if (state.ch == '$')
	  {
	    // @@@ Hmm. Empty param...
	    parseState = formatFlagsWidthPrecTypeConversion;
	    break;
	  }
	case formatParamWidth:
	  if (parseState == formatParamWidth) // != can occur due fallthrough
	  {
	    // Subsequent digits width or param index
	    if ((state.ch >= '0') && (state.ch <= '9'))
	    {
	      state.currentFormat.width *= 10;
	      state.currentFormat.width += state.ch - '0';
	      break;
	    }
	    // Param delimiter
	    else if (state.ch == '$')
	    {
	      state.paramIdx = state.currentFormat.width - 1;
	      state.currentFormat.width = 0;
	      parseState = formatFlagsWidthPrecTypeConversion;
	      break;
	    }
	  }
	case formatFlagsWidthPrecTypeConversion:
	  // Check for start of width
	  if ((state.ch >= '1') && (state.ch <= '9'))
	  {
	    state.currentFormat.width *= 10;
	    state.currentFormat.width += state.ch - '0';
	    parseState = formatParamWidth;
	    break;
	  }
	  // Check for flags (0, -, ...)
	  else if (ParseFlag (state))
	  {
	    parseState = formatFlagsWidthPrecTypeConversion;
	    break;
	  }
	  // Check for precision delimiter
	  else if (state.ch == '.')
	  {
	    parseState = formatPrecTypeConversion;
	    state.currentFormat.precision = 0;
	    break;
	  }
	case formatPrecTypeConversion:
	  // Precision digits
	  if ((state.ch >= '0') && (state.ch <= '9'))
	  {
	    state.currentFormat.precision *= 10;
	    state.currentFormat.precision += state.ch - '0';
	    break;
	  }
	  // Check for param type modifier (l, h, ...)
	  else if (ParseType (state))
	  {
	    parseState = formatPrecTypeConversion;
	    break;
	  }
	case formatTypeConversion:
	  // Check actual conversion (s, d, ...)
	  if (ParseConversion (state))
	  {
	    state.currentFormat.fmtSkip = reader.GetPosition() - state.fmtBegin;
	    if (state.currentFormat.conversion != convNone)
	      state.currentFormat.paramIdx = state.paramIdx++;
	    formatSpecs.Push (state.currentFormat);

	    state.Reset();
	  }
	  else
	  {
	    state.charRun += reader.GetPosition() - state.fmtBegin;
	    state.currentFormat.Reset();
	  }
	  parseState = scanFormat;
	  break;
       }
     }
   }

   void FetchParams (va_list args)
   {
    // Determine order of params
    for (size_t i = 0; i < formatSpecs.Length(); i++)
    {
      const FormatSpec& currentFormat = formatSpecs[i];
      FmtParam& param = params.GetExtend (currentFormat.paramIdx);
      switch (currentFormat.conversion)
      {
	case convInt:
	case convOctal:
	case convUint:
	case convHex:
	default:
	  {
	    switch (currentFormat.type)
	    {
	      case typeIntmax:
		param.vIMT = va_arg (args, intmax_t);
		break;
	      case typeLong:
		param.vLong = va_arg (args, long);
		break;
	      case typeLongLong:
		param.vLL = va_arg (args, longlong);
		break;
	      case typePtrDiffT:
		param.vPDT = va_arg (args, ptrdiff_t);
		break;
	      case typeSizeT:
		param.vSzT = va_arg (args, size_t);
		break;
	      case typeShort:
		param.vInt = (short)(va_arg (args, int));
		break;
	      case typeChar:
		param.vInt = (char)(va_arg (args, int));
		break;
	      default:
		param.vInt = va_arg (args, int);
		break;
	    }
	  }
	  break;
	case convErrno:
	  param.vInt = errno;
	  break;
	case convChar:
	  if (currentFormat.type == typeLong)
	  {
	    param.vInt = (wint_t)(va_arg (args, int));
	  }
	  else
	  {
	    param.vInt = (unsigned char)(va_arg (args, int));
	  }
	  break;
	case convFloatFix:
	case convFloatExp:
	case convFloatGeneral:
	case convFloatHex:
	  if (currentFormat.type == typeLong)
	  {
	    param.vLongDbl = va_arg (args, long double);
	  }
	  else
	  {
	    param.vDbl = va_arg (args, double);
	  }
	  break;
	case convStr:
	case convPtr:
	case convGetNum:
	  param.vPtr = va_arg (args, void*);
	  break;
	case convNone:
	  break;
      }
    }
  }

  void Init (va_list args)
  {
    ParseSpec ();
    FetchParams (args);
  }

  /// Write out a string
  template<class T>
  void OutputString (Twriter& writer, const FormatSpec& currentFormat,
    const T* stringPtr)
  {
    if (stringPtr == 0)
    {
      OutputString (writer, currentFormat, (utf8_char*)"(nil)");
      return;
    }

    size_t scratchOffs = scratch.Length();
    size_t len = 0;
    {
      const T* ptr = stringPtr;
      while (*ptr++ != 0) len++;
    }
    while (len > 0)
    {
      utf32_char ch;
      int n = csUnicodeTransform::Decode (stringPtr, len, ch);
      scratch.Push (ch);
      stringPtr += n;
      len -= (size_t)n;
    }
    if (!currentFormat.leftJustify 
      && ((size_t)currentFormat.width > (scratch.Length() - scratchOffs)))
    {
      size_t d = (size_t)currentFormat.width - scratch.Length() + scratchOffs;
      while (d-- > 0) writer.Put (' ');
    }
    scratch.WriteTo (writer, scratchOffs);
    if (currentFormat.leftJustify 
      && ((size_t)currentFormat.width > (scratch.Length() - scratchOffs)))
    {
      size_t d = (size_t)currentFormat.width - scratch.Length() + scratchOffs;
      while (d-- > 0) writer.Put (' ');
    }
    scratch.Truncate (scratchOffs);
  }

  /// Pad scratch buffer with spaces or zeros left resp. right
  void DoPadding (const FormatSpec& currentFormat, const size_t scratchOffs,
    const size_t insert0offs)
  {
    if (currentFormat.leftJustify)
    {
      while ((size_t)currentFormat.width > (scratch.Length() - scratchOffs))
      {
	scratch.Push (' ');
      }
    }
    else
    {
      if (currentFormat.padZero)
      {
	while ((size_t)currentFormat.width > (scratch.Length() - scratchOffs))
	{
	  scratch.Insert (insert0offs, '0');
	}
      }
      else
      {
	while ((size_t)currentFormat.width > (scratch.Length() - scratchOffs))
	{
	  scratch.Insert (scratchOffs, ' ');
	}
      }
    }
  }

  /// Output a signed integer.
  template<class T>
  void OutputInt (Twriter& writer, const FormatSpec& currentFormat, T value)
  {
    const size_t scratchOffs = scratch.Length();
    size_t insertOffs = scratchOffs;

    if (value < 0)
    {
      scratch.Push ('-');
      insertOffs++;
      value = -value;
    }
    else if (currentFormat.plusSign)
    {
      scratch.Push ('+');
      insertOffs++;
    }
    else if (currentFormat.spacePrefix)
    {
      scratch.Push (' ');
      insertOffs++;
    }

    int width = 0;
    int numDigits = currentFormat.precision;
    if (!((value == 0) && (numDigits == 0)))
    {
      do
      {
	int d = value % 10;
	scratch.Insert (insertOffs, d + '0');
	width++;
	value = value / 10;
      }
      while ((value != 0) || (width < numDigits));
    }
    DoPadding (currentFormat, scratchOffs, insertOffs);
    scratch.WriteTo (writer, scratchOffs);
    scratch.Truncate (scratchOffs);
  }

  /// Output an unsigned integer.
  template<class T>
  void OutputUint (Twriter& writer, const FormatSpec& currentFormat,
    T value, uint radix, const char* prefix = 0)
  {
    const utf32_char letterFirst = currentFormat.uppercase ? 'A' : 'a';
    const size_t scratchOffs = scratch.Length();
    size_t insertOffs = scratchOffs;

    if (prefix != 0)
    {
      while (*prefix != 0)
      {
	utf32_char ch = (value != 0) ? *prefix : ' ';
	scratch.Push (ch);
	insertOffs++;
	prefix++;
      }
    }

    int width = 0;
    int numDigits = currentFormat.precision;
    if (!((value == 0) && (numDigits == 0)))
    {
      do
      {
	uint d = value % radix;
	utf32_char ch;
	if (d <= 9)
	  ch = d + '0';
	else
	  ch = d - 10 + letterFirst;
	scratch.Insert (insertOffs, ch);
	width++;
	value = value / radix;
      }
      while ((value != 0) || (width < numDigits));
    }
    DoPadding (currentFormat, scratchOffs, insertOffs);
    scratch.WriteTo (writer, scratchOffs);
    scratch.Truncate (scratchOffs);
  }

  /// Output a float. We're lazy and use the existing CRT routines.
  template<class T>
  void OutputFloat (Twriter& writer, const FormatSpec& currentFormat,
    T value, const char* type)
  {
    char flags[5] = "";
    if (currentFormat.plusSign)
      strcat (flags, "+");
    if (currentFormat.spacePrefix)
      strcat (flags, " ");
    if (currentFormat.basePrefix)
      strcat (flags, "#");
    if (currentFormat.padZero)
      strcat (flags, "0");
    CS_ALLOC_STACK_ARRAY(char, precStr, 
      (sizeof(currentFormat.precision) * 24) / 10 + 3);
    if (currentFormat.precision >= 0)
      sprintf (precStr, ".%d", currentFormat.precision);
    else
      precStr[0] = 0;
    CS_ALLOC_STACK_ARRAY(char, formatStr, 1 + strlen (flags)
      + (sizeof(currentFormat.width) * 24) / 10 + 2 + strlen (precStr) + 2);
    sprintf (formatStr, "%%%s%d%s%s", flags, currentFormat.width, precStr,
      type);
    char formattedStr[32];
    sprintf (formattedStr, formatStr, value);

    char* p = formattedStr;
    while (*p != 0)
      writer.Put (*p++);
  }

  /**
   * Helper to deal with mantissa (which can span multiple uints)
   */
  template<class T, class Tbase>
  struct IEEEFloatMantissa
  {
    Tbase mantissa[sizeof(T)/sizeof(Tbase)];

    Tbase& operator[] (int index)
    { return mantissa[index]; }
    bool Eq0 ()
    {
      for (uint n = 0; n < sizeof(T)/sizeof(Tbase); n++)
      {
	if (mantissa[n] != 0) return false;
      }
      return true;
    }
    const Tbase operator& (Tbase other) const
    { return mantissa[0] & other; }
    IEEEFloatMantissa& operator<<= (int shift)
    { 
      const int ovShift = sizeof(Tbase) * 8 - shift;
      Tbase overflow = 0;
      for (uint n = 0; n < sizeof(T)/sizeof(Tbase); n++)
      {
	Tbase newOverflow = mantissa[n] >> ovShift;
	mantissa[n] = (mantissa[n] << shift) | overflow;
	overflow = newOverflow;
      }
      return *this;
    }
    Tbase& Leftmost ()
    { return mantissa[sizeof(T)/sizeof(Tbase)-1]; }
  };

  /// Class to pry apart an IEEE floating point number
  template<class T, class Tbase>
  struct IEEEFloatSplitter
  {
    bool sign;
    Tbase exp;

    IEEEFloatMantissa<T, Tbase> mantissa;

    IEEEFloatSplitter (const T& val, const int mantissaBits) 
    {
      const int baseBits = sizeof(Tbase) * 8;
      const int expBits = sizeof(T) * 8 - mantissaBits - 1;

      union
      {
	T v;
	Tbase vB[sizeof(T)/sizeof(Tbase)];
      } toBase;
      toBase.v = val;
  #ifdef CS_LITTLE_ENDIAN
      const int hi = (sizeof (T) / sizeof (Tbase)) - 1;
      const int lo = 0;
      const int d = 1;
  #else
      const int hi = 0;
      const int lo = (sizeof (T) / sizeof (Tbase)) - 1;
      const int d = -1;
  #endif
      sign = toBase.vB[hi] & (1 << (baseBits - 1));
      exp = (toBase.vB[hi] >> (mantissaBits % (baseBits)))
	& ((1 << expBits) - 1);
      for (int n = lo, p = 0; n != hi + d; n += d, p++)
      {
	const int bit = p * baseBits;
	const Tbase mask = ((bit + baseBits) < mantissaBits) ? ~0 
	  : ((1 << mantissaBits) - 1);
	mantissa[p] = toBase.vB[n] & mask;
      }
    }
  };
  /// Output a float in hex format
  template <class T>
  void OutputFloatHex (Twriter& writer, const FormatSpec& currentFormat,
    const T& value, const int mantissaBits, const int bias)
  {
    const utf32_char letterFirst = currentFormat.uppercase ? 'A' : 'a';

#ifdef CS_IEEE_DOUBLE_FORMAT
    IEEEFloatSplitter<T, uint> vSplit (value, mantissaBits);
    const uint expMax = (1 << (sizeof(T) * 8 - mantissaBits - 1)) - 1;

    if ((vSplit.exp == expMax) && vSplit.mantissa.Eq0())
    {
      char infStr[5];
      if (vSplit.sign)
      {
	strcpy (infStr, "-");
      }
      else
      {
	if (currentFormat.plusSign)
	  strcpy (infStr, "+");
	else if (currentFormat.spacePrefix)
	  strcpy (infStr, " ");
	else
	  strcpy (infStr, "");
      }
      strcat (infStr, currentFormat.uppercase ? "INF" : "inf");
      OutputString (writer, currentFormat, 
	(utf8_char*)infStr);
      return;
    }
    else if ((vSplit.exp == expMax) && !vSplit.mantissa.Eq0())
    {
      char nanStr[5];
      if (vSplit.sign)
      {
	strcpy (nanStr, "-");
      }
      else
      {
	if (currentFormat.plusSign)
	  strcpy (nanStr, "+");
	else if (currentFormat.spacePrefix)
	  strcpy (nanStr, " ");
	else
	  strcpy (nanStr, "");
      }
      strcat (nanStr, currentFormat.uppercase ? "NAN" : "nan");
      OutputString (writer, currentFormat, 
	(utf8_char*)nanStr);
      return;
    }

    const size_t scratchOffs = scratch.Length();
    if (vSplit.sign)
    {
      scratch.Push ('-');
    }
    scratch.Push ('0');
    scratch.Push (currentFormat.uppercase ? 'X' : 'x');
    if (exp == 0)
      scratch.Push ('0');
    else
      scratch.Push ('1');
    if ((currentFormat.precision > 0) || (!vSplit.mantissa.Eq0()))
    {
      scratch.Push ('.');
      
      IEEEFloatMantissa<T, uint> m (vSplit.mantissa);
      m <<= sizeof(T)*8 - mantissaBits;
      int w = 0;
      do
      {
	uint d = m.Leftmost() >> ((sizeof(uint)*8)-4);
	utf32_char ch;
	if (d <= 9)
	  ch = d + '0';
	else
	  ch = d - 10 + letterFirst;
	scratch.Push (ch);
	m <<= 4;
	w++;
      }
      while ((w < currentFormat.precision) 
	|| ((currentFormat.precision <= 0) && !m.Eq0()));
    }
    scratch.Push (currentFormat.uppercase ? 'P' : 'p');
    int e;
    if ((vSplit.exp == 0) && vSplit.mantissa.Eq0())
      e = 0;
    else
      e = (int)vSplit.exp + bias;
    if (e < 0)
    {
      scratch.Push ('-');
      e = -e;
    }
    else
      scratch.Push ('+');
    const size_t insertOffs = scratch.Length();;
    do
    {
      uint d = e % 10;
      scratch.Insert (insertOffs, d + '0');
      e = e / 10;
    }
    while (e != 0);

    DoPadding (currentFormat, scratchOffs, 
      vSplit.sign ? scratchOffs + 1 : scratchOffs);
    scratch.WriteTo (writer, scratchOffs);
    scratch.Truncate (scratchOffs);
#else
  #warning Dont know how to hex-format floats for you
#endif
  }
public:
  /// Initialize from a reader and a argument list.
  csPrintfFormatter (Treader* reader, va_list args) : reader (*reader)
  {
    Init (args);
  }
  /// Initialize from a reader and a argument list.
  csPrintfFormatter (Treader* reader, ...) : reader (*reader)
  {
    va_list ap;
    va_start(ap, reader);
    Init (ap);
    va_end(ap);
  }
  /// Do the actual formatting.
  void Format (Twriter& writer)
  {
    reader.Reset();
    size_t i = 0;
    utf32_char ch;
    while (i < formatSpecs.Length())
    {
      const FormatSpec& currentFormat = formatSpecs[i];
      size_t n;
      for (n = 0; n < currentFormat.copyRun; n++)
      {
	if (!reader.GetNext (ch)) break;
	writer.Put (ch);
      }

      switch (currentFormat.conversion)
      {
	case convStr:
	  {
	    if (currentFormat.type == typeLong)
	      OutputString (writer, currentFormat, 
	      (wchar_t*)(params[currentFormat.paramIdx].vPtr));
	    else
	      OutputString (writer, currentFormat, 
	      (utf8_char*)(params[currentFormat.paramIdx].vPtr));
	  }
	  break;
	case convChar:
	  {
	    writer.Put (params[currentFormat.paramIdx].vInt);
	  }
	  break;
	case convInt:
	  {
	    const FmtParam& param = params[currentFormat.paramIdx];
	    switch (currentFormat.type)
	    {
	      case typeIntmax:
		{
		  intmax_t v = param.vIMT;
		  OutputInt (writer, currentFormat, v);
		}
		break;
	      case typeLong:
		{
		  long v = param.vLong;
		  OutputInt (writer, currentFormat, v);
		}
		break;
	      case typeLongLong:
		{
		  longlong v = param.vLL;
		  OutputInt (writer, currentFormat, v);
		}
		break;
	      case typePtrDiffT:
		{
		  ptrdiff_t v = param.vPDT;
		  OutputInt (writer, currentFormat, v);
		}
		break;
	      case typeSizeT:
		{
		  size_t v = param.vSzT;
		  OutputInt (writer, currentFormat, v);
		}
		break;
	      default:
		{
		  int v = param.vInt;
		  OutputInt (writer, currentFormat, v);
		}
		break;
	    }
	  }
	  break;
	case convHex:
	case convUint:
	case convOctal:
	  {
	    uint uiradix;
	    const char* prefix;
	    if (currentFormat.conversion == convHex)
	    {
	      uiradix = 16;
	      prefix = currentFormat.basePrefix 
		? (currentFormat.uppercase ? "0X" : "0x") : 0;
	    }
	    else if (currentFormat.conversion == convOctal)
	    {
	      uiradix = 8;
	      prefix = currentFormat.basePrefix ? "0" : 0;
	    }
	    else
	    {
	      uiradix = 10;
	      prefix = 0;
	    }
	    const FmtParam& param = params[currentFormat.paramIdx];
	    switch (currentFormat.type)
	    {
	      case typeIntmax:
		{
		  intmax_t v = param.vIMT;
		  OutputUint (writer, currentFormat, v, uiradix, prefix);
		}
		break;
	      case typeLong:
		{
		  unsigned long v = param.vLong;
		  OutputUint (writer, currentFormat, v, uiradix, prefix);
		}
		break;
	      case typeLongLong:
		{
		  ulonglong v = param.vLL;
		  OutputUint (writer, currentFormat, v, uiradix, prefix);
		}
		break;
	      case typePtrDiffT:
		{
		  ptrdiff_t v = param.vPDT;
		  OutputUint (writer, currentFormat, v, uiradix, prefix);
		}
		break;
	      case typeSizeT:
		{
		  size_t v = param.vSzT;
		  OutputUint (writer, currentFormat, v, uiradix, prefix);
		}
		break;
	      default:
		{
		  uint v = param.vInt;
		  OutputUint (writer, currentFormat, v, uiradix, prefix);
		}
		break;
	    }
	  }
	  break;
	case convGetNum:
	  *((int*)(params[currentFormat.paramIdx].vPtr)) = writer.GetTotal();
	  break;
	case convErrno:
	  OutputString (writer, currentFormat, 
	    (utf8_char*)strerror (params[currentFormat.paramIdx].vInt));
	  break;
	case convPtr:
	  {
	    FormatSpec fakeFormat;
	    fakeFormat.leftJustify = currentFormat.leftJustify;
	    fakeFormat.precision = sizeof (uintptr_t) * 2;
	    if (params[currentFormat.paramIdx].vPtr == 0)
	    {
	      OutputString (writer, fakeFormat, (utf8_char*)"(null)");
	    }
	    else
	    {
	      OutputUint (writer, fakeFormat, 
		(uintptr_t)params[currentFormat.paramIdx].vPtr,	16, "0x");
	    }
	  }
	  break;
	case convFloatFix:
	  {
	    if (currentFormat.type == typeLongLong)
	      OutputFloat (writer, currentFormat, 
	      params[currentFormat.paramIdx].vLongDbl, "f");
	    else
	      OutputFloat (writer, currentFormat, 
	      params[currentFormat.paramIdx].vDbl, "f");
	  }
	  break;
	case convFloatExp:
	  {
	    if (currentFormat.type == typeLongLong)
	      OutputFloat (writer, currentFormat, 
	      params[currentFormat.paramIdx].vLongDbl, 
	      currentFormat.uppercase ? "E" : "e");
	    else
	      OutputFloat (writer, currentFormat, 
	      params[currentFormat.paramIdx].vDbl, 
	      currentFormat.uppercase ? "E" : "e");
	  }
	  break;
	case convFloatGeneral:
	  {
	    if (currentFormat.type == typeLongLong)
	      OutputFloat (writer, currentFormat, 
	      params[currentFormat.paramIdx].vLongDbl, 
	      currentFormat.uppercase ? "G" : "g");
	    else
	      OutputFloat (writer, currentFormat, 
	      params[currentFormat.paramIdx].vDbl, 
	      currentFormat.uppercase ? "G" : "g");
	  }
	  break;
	case convFloatHex:
	  {
	    if (currentFormat.type == typeLongLong)
	      OutputFloatHex (writer, currentFormat, 
	      params[currentFormat.paramIdx].vLongDbl, LDBL_MANT_DIG-1, 
	      -(LDBL_MAX_EXP - 1));
	    else
	      OutputFloatHex (writer, currentFormat, 
	      params[currentFormat.paramIdx].vDbl, DBL_MANT_DIG-1, 
	      -(DBL_MAX_EXP - 1));
	  }
	  break;
	default:
	  break;
      }

      for (n = 0; n < currentFormat.fmtSkip; n++)
      {
	if (!reader.GetNext (ch)) break;
      }
      i++;
    }
    while (reader.GetNext (ch))
      writer.Put (ch);
    writer.Put (0);
  }
};

/** @} */

#endif // __CS_CSUTIL_FORMATTER_H__
