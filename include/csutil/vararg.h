/*
  Copyright (C)2012 by Eric Sunshine <sunshine@sunshineco.com>

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

#ifndef __CSUTIL_VARARG_H__
#define __CSUTIL_VARARG_H__

#include <cssysdef.h>
#include <csutil/ref.h>

/**\file
 * Simplify invocation of methods accepting va_list.
 *
 * CS::va_call() and CS::va_callv() provide a convenient mechanism by which to
 * invoke a method or function accepting va_list when a vararg (...) variation
 * of the same method is unavilable. va_call() is used with methods returning
 * an argument, whereas va_callv() is for methods declared void. For example,
 * assume the following interface:
 *
 * \code
 * struct iFoo {
 *   void bar(char const* fmt, ...) = 0;
 *   void barv(char const* fmt, va_list) = 0;
 * };
 * \endcode
 *
 * To invoke iFoo::bar(), one writes:
 *
 * \code
 * iFoo* foo = ...;
 * foo->bar("%d %s", 42, "forty-two");
 * \endcode
 *
 * Assume, however, for some reason, that iFoo publishes method barv(), but not
 * bar(). CS::va_callv() provides a relatively convenient method for invoking
 * barv() without having to manage a va_list manually.
 *
 * \code
 * iFoo* foo = ...;
 * CS::va_callv(&iFoo::barv, foo, "%d %s", 42, "forty-two");
 * \endcode
 *
 * \remarks
 * The llvm-gcc compiler shipped with Apple's Xcode is incompletely
 * implemented.  In particular, when it encounters a vararg virtual method in a
 * virtual base class, it aborts with error "generic thunk code fails for
 * method foo::bar() which uses '...'". A potential work-around is to #ifdef
 * out the problematic vararg methods and instead invoke their va_list
 * counterparts. CS_va_call() and CS::va_callv() simplify this process by
 * providing a relatively convenient way to invoke the va_list counterparts
 * without writing custom code to manage a va_list.
 */

namespace CS
{
  /// \copydoc vararg.h
  template <typename T, typename U>
  void va_callv(void(T::*f)(U, va_list), T* o, U x, ...) {
    va_list v;
    va_start(v, x);
    (o->*f)(x, v);
    va_end(v);
  }

  /// \copydoc vararg.h
  template <typename T, typename U>
  void va_callv(void(T::*f)(U, va_list), csRef<T> o, U x, ...) {
    va_list v;
    va_start(v, x);
    ((*o).*f)(x, v);
    va_end(v);
  }

  /// \copydoc vararg.h
  template <typename R, typename T, typename U>
  R va_call(R(T::*f)(U, va_list), T* o, U x, ...) {
    va_list v;
    va_start(v, x);
    R r = (o->*f)(x, v);
    va_end(v);
    return r;
  }

  /// \copydoc vararg.h
  template <typename R, typename T, typename U>
  R va_call(R(T::*f)(U, va_list), csRef<T> o, U x, ...) {
    va_list v;
    va_start(v, x);
    R r = ((*o).*f)(x, v);
    va_end(v);
    return r;
  }

  /// \copydoc vararg.h
  template <typename T, typename U1, typename U2>
  void va_callv(void(T::*f)(U1, U2, va_list), T* o, U1 x1, U2 x2, ...) {
    va_list v;
    va_start(v, x2);
    (o->*f)(x1, x2, v);
    va_end(v);
  }

  /// \copydoc vararg.h
  template <typename T, typename U1, typename U2>
  void va_callv(void(T::*f)(U1, U2, va_list), csRef<T> o, U1 x1, U2 x2, ...) {
    va_list v;
    va_start(v, x2);
    ((*o).*f)(x1, x2, v);
    va_end(v);
  }

  /// \copydoc vararg.h
  template <typename R, typename T, typename U1, typename U2>
  R va_call(R(T::*f)(U1, U2, va_list), T* o, U1 x1, U2 x2, ...) {
    va_list v;
    va_start(v, x2);
    R r = (o->*f)(x1, x2, v);
    va_end(v);
    return r;
  }

  /// \copydoc vararg.h
  template <typename R, typename T, typename U1, typename U2>
  R va_call(R(T::*f)(U1, U2, va_list), csRef<T> o, U1 x1, U2 x2, ...) {
    va_list v;
    va_start(v, x2);
    R r = ((*o).*f)(x1, x2, v);
    va_end(v);
    return r;
  }

  /// \copydoc vararg.h
  template <typename T, typename U1, typename U2, typename U3>
  void va_callv(void(T::*f)(U1, U2, U3, va_list), T* o, U1 x1, U2 x2, U3 x3, ...) {
    va_list v;
    va_start(v, x3);
    (o->*f)(x1, x2, x3, v);
    va_end(v);
  }

  /// \copydoc vararg.h
  template <typename T, typename U1, typename U2, typename U3>
  void va_callv(void(T::*f)(U1, U2, U3, va_list), csRef<T> o, U1 x1, U2 x2, U3 x3, ...) {
    va_list v;
    va_start(v, x3);
    ((*o).*f)(x1, x2, x3, v);
    va_end(v);
  }

  /// \copydoc vararg.h
  template <typename R, typename T, typename U1, typename U2, typename U3>
  R va_call(R(T::*f)(U1, U2, U3, va_list), T* o, U1 x1, U2 x2, U3 x3, ...) {
    va_list v;
    va_start(v, x3);
    R r = (o->*f)(x1, x2, x3, v);
    va_end(v);
    return r;
  }

  /// \copydoc vararg.h
  template <typename R, typename T, typename U1, typename U2, typename U3>
  R va_call(R(T::*f)(U1, U2, U3, va_list), csRef<T> o, U1 x1, U2 x2, U3 x3, ...) {
    va_list v;
    va_start(v, x3);
    R r = ((*o).*f)(x1, x2, x3, v);
    va_end(v);
    return r;
  }

  /// \copydoc vararg.h
  template <typename T, typename U1, typename U2, typename U3, typename U4>
  void va_callv(void(T::*f)(U1, U2, U3, U4, va_list), T* o, U1 x1, U2 x2, U3 x3, U4 x4, ...) {
    va_list v;
    va_start(v, x4);
    (o->*f)(x1, x2, x3, x4, v);
    va_end(v);
  }

  /// \copydoc vararg.h
  template <typename T, typename U1, typename U2, typename U3, typename U4>
  void va_callv(void(T::*f)(U1, U2, U3, U4, va_list), csRef<T> o, U1 x1, U2 x2, U3 x3, U4 x4, ...) {
    va_list v;
    va_start(v, x4);
    ((*o).*f)(x1, x2, x3, x4, v);
    va_end(v);
  }

  /// \copydoc vararg.h
  template <typename R, typename T, typename U1, typename U2, typename U3, typename U4>
  R va_call(R(T::*f)(U1, U2, U3, U4, va_list), T* o, U1 x1, U2 x2, U3 x3, U4 x4, ...) {
    va_list v;
    va_start(v, x4);
    R r = (o->*f)(x1, x2, x3, x4, v);
    va_end(v);
    return r;
  }

  /// \copydoc vararg.h
  template <typename R, typename T, typename U1, typename U2, typename U3, typename U4>
  R va_call(R(T::*f)(U1, U2, U3, U4, va_list), csRef<T> o, U1 x1, U2 x2, U3 x3, U4 x4, ...) {
    va_list v;
    va_start(v, x4);
    R r = ((*o).*f)(x1, x2, x3, x4, v);
    va_end(v);
    return r;
  }

} // namespace CS

#endif // __CSUTIL_VARARG_H__
