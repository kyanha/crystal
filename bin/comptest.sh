#!/bin/sh
#==============================================================================
# A compiler capability testing script.
#
# This script tries to identify the compiler first, it looks if CC or CXX are
# set, otherwise it tries to use one of the common gcc names. After that it
# does some tests for compiler characteristics and known compiler bugs.
#
# The output of this script (a makefile fragment) is configuration information
# needed for building Crystal Space.  It is piped to stdout, and errors are
# piped to stderr.
#==============================================================================

#------------------------------------------------------------------------------
# Try to find compiler.
#------------------------------------------------------------------------------
#Find a c++ compiler
if [ -z "${CXX}" ]; then
  [ -z "${CXX}" ] && CXX=`checkprog g++`
  [ -z "${CXX}" ] && CXX=`checkprog gcc`
  [ -z "${CXX}" ] && CXX=`checkprog egcs`
  [ -z "${CXX}" ] && CXX=`checkprog c++`
  [ -z "${CXX}" ] && CXX=`checkprog cc`
  if [ -z "${CXX}" ]; then
    echo "$0: Cannot find an installed C++ compiler!" >&2
    exit 1
  fi
  CXX=`basename ${CXX}`
fi

#Find a C compiler
if [ -z "${CC}" ]; then
  [ -z "${CC}" ] && CC=`checkprog gcc`
  [ -z "${CC}" ] && CC=`checkprog egcs`
  [ -z "${CC}" ] && CC=`checkprog cc`
  if [ -z "${CC}" ]; then
    echo "$0: Cannot find an installed C compiler!" >&2
    exit 1
  fi
  CC=`basename ${CC}`
fi

LINK="${CXX}"

echo "CC = ${CC} -c"
echo "CXX = ${CXX} -c"
echo "LINK = ${CXX}"


#-----------------------------------------------------------------------------
# Check for optimisation flags
#-----------------------------------------------------------------------------
# Create a dummy C++ program
echo "int main() { return 0; }" > comptest.cpp

if test -n "$ARCH"; then
    ARCH="-march=$ARCH";
fi
# Check for machine-specific C compiler flags
(echo "$CPU" | grep -s 686 >/dev/null && ${CXX} -c -mcpu=pentiumpro ${ARCH-"-march=i686"} comptest.cpp && echo "CFLAGS.SYSTEM += -mcpu=pentiumpro ${ARCH-"-march=i686"}") || \
(echo "$CPU" | grep -s 686 >/dev/null && ${CXX} -c -mpentiumpro ${ARCH-"-march=i686"} comptest.cpp && echo "CFLAGS.SYSTEM += -mpentiumpro ${ARCH-"-march=i686"}") || \
(echo "$CPU" | grep -s [5-6]86 >/dev/null && ${CXX} -c -mcpu=pentium ${ARCH-"-march=i586"} comptest.cpp && echo "CFLAGS.SYSTEM += -mcpu=pentium ${ARCH-"-march=i586"}") || \
(echo "$CPU" | grep -s [5-6]86 >/dev/null && ${CXX} -c -mpentium ${ARCH-"-march=i586"} comptest.cpp && echo "CFLAGS.SYSTEM += -mpentium ${ARCH-"-march=i586"}") || \
(echo "$CPU" | grep -s [3-9]86 >/dev/null && ${CXX} -c -mcpu=i486 comptest.cpp && echo "CFLAGS.SYSTEM += -mcpu=i486") || \
(echo "$CPU" | grep -s [3-9]86 >/dev/null && ${CXX} -c -m486 comptest.cpp && echo "CFLAGS.SYSTEM += -m486") || \
(echo "$MACHINE" | grep -s alpha >/dev/null && ${CXX} -c -mieee comptest.cpp && echo "CFLAGS.SYSTEM += -mieee")

# Check for GCC-version-specific command-line options
${CXX} -c -fno-exceptions comptest.cpp 2>/dev/null && echo "CFLAGS.SYSTEM += -fno-exceptions"


#------------------------------------------------------------------------------
# Check if C++ compiler has a built-in 'bool' type.
#------------------------------------------------------------------------------
echo "int func() { bool b = true; return (int)b; }" > comptest.cpp
${CXX} -c comptest.cpp 2>/dev/null || echo "CS_USE_FAKE_BOOL_TYPE = yes"


#------------------------------------------------------------------------------
# Check if C++ compiler understands new-style C++ casting syntax.
# For example: `static_cast<int>(foo)' vs. `(int)foo'
#
# Specifically check for all four new casting operators since some botched
# compilers have been known to implement only a partial set.  (The OpenStep
# Objective-C++ compiler is one such botched implementation.  It fails to
# recognize reinterpret_cast even though it recognizes the others.)
#------------------------------------------------------------------------------
cat << TEST > comptest.cpp
int func1() { long n = 1; return static_cast<int>(n); }
char* func2() { static char const* s = "const"; return const_cast<char*>(s); }
struct A {}; A* func3(A* a) { return dynamic_cast<A*>(a); }
A* func4(void* p) { return reinterpret_cast<A*>(p); }
TEST
${CXX} -c comptest.cpp 2>/dev/null || echo "CS_USE_OLD_STYLE_CASTS = yes"


#------------------------------------------------------------------------------
# Check if C++ compiler understands new C++ `explicit' keyword.
#------------------------------------------------------------------------------
echo "class A { public: explicit A(int); };" > comptest.cpp
${CXX} -c comptest.cpp 2>/dev/null || echo "CS_USE_FAKE_EXPLICIT_KEYWORD = yes"


#------------------------------------------------------------------------------
# If processor type was specified, check if compiler is able to understand
# CS/include/qsqrt.h.  This test should not only catch compilers which do not
# understand the x86 assembly in that file, but should also catch compilers
# which fail with an internal error on this file (such as the RedHat 7 GCC).
# If the processor type was not specified, then no test is made, nor are any
# makefile variables set.  In this case, it is up to the platform-specific
# configuration mechanism to enable the CS_NO_QSQRT flag if necessary.
#------------------------------------------------------------------------------
if [ -n "${PROC}" ]; then
cat << TEST > comptest.cpp
#define PROC_${PROC}
#define COMP_GCC
#include "include/qsqrt.h"
float func() { float n = 1; return qsqrt(n); }
TEST
${CXX} -c comptest.cpp 2>/dev/null || echo "CS_NO_QSQRT = yes"
fi


#------------------------------------------------------------------------------
# Attempt to detect a compiler bug discovered in gcc 2.96 (redhat and other
# unstable versions) and gcc 3.0.1 Note: This fails for cross-compiling because
# you can't execute the result.  (Oh well.)
# -----------------------------------------------------------------------------
cat << TEST > comptest.cpp
static inline long double2int(double val)
{
  long* l;
  val += 68719476736.0;
  l = (long*)((char*)&val + 2);
  return *l;
}
int main() { return (double2int(255.99) != 255 ? 1 : 0); }
TEST
${CXX} -O2 comptest.cpp -o comptest 
./comptest 2>/dev/null || echo "CS_QINT_WORKAROUND = yes"


#------------------------------------------------------------------------------
# Clean up.
#------------------------------------------------------------------------------
rm -f comptest.cpp comptest.o comptest.obj comptest.exe comptest
