# This is the makefile for Cygnus/Win32 compiler

# Friendly names for building environment
DESCRIPTION.cygwin = Win32 with Cygnus GCC

# Choose which drivers you want to build/use
# video/canvas/ddraw6 video/canvas/openglwin video/renderer/direct3d5 
# video/renderer/direct3d6 video/renderer/opengl
#
PLUGINS+=video/canvas/ddraw video/renderer/software sound/renderer/software

# Uncomment the following to get an startup console window
#CONSOLE_FLAGS = -DWIN32_USECONSOLE

#---------------------------------------------------- rootdefines & defines ---#
ifneq (,$(findstring defines,$(MAKESECTION)))

.SUFFIXES: .exe .dll

# Processor type.
PROC=INTEL

# Operating system
OS=WIN32

# Compiler
COMP=GCC

# Command to update a target
#UPD=bin\dosupd.bat $@ DEST

endif # ifneq (,$(findstring defines,$(MAKESECTION)))

#------------------------------------------------------------------ defines ---#
ifeq ($(MAKESECTION),defines)

include mk/dos.mak

# Extra libraries needed on this system (beside drivers)
LIBS.EXE=

# Where can the Zlib library be found on this system?
Z_LIBS=-Llibs/zlib -lz

# Where can the PNG library be found on this system?
PNG_LIBS=-Llibs/libpng -lpng

# Where can the JPG library be found on this system?
JPG_LIBS=-Llibs/libjpeg -ljpeg

# Where can the optional sound libraries be found on this system?
SOUND_LIBS=

# Does this system require libsocket.a?
NEED_SOCKET_LIB=

# Indicate where special include files can be found.
CFLAGS.INCLUDE=-Ilibs/zlib -Ilibs/libpng -Ilibs/libjpeg

# General flags for the compiler which are used in any case.
CFLAGS.GENERAL=-Wall $(CFLAGS.SYSTEM)

# Flags for the compiler which are used when optimizing.
CFLAGS.optimize=-s -O3 -fomit-frame-pointer -ffast-math

# Flags for the compiler which are used when debugging.
CFLAGS.debug=-g

# Flags for the compiler which are used when profiling.
CFLAGS.profile=-pg -O -g

# Flags for the compiler which are used when building a shared library.
CFLAGS.DLL=

# General flags for the linker which are used in any case.
LFLAGS.GENERAL=

# Flags for the linker which are used when optimizing.
LFLAGS.optimize=-s

# Flags for the linker which are used when debugging.
LFLAGS.debug=-g

# Flags for the linker which are used when profiling.
LFLAGS.profile=-pg

# Flags for the linker which are used when building a shared library.
LFLAGS.DLL=

# Typical extension for objects and static libraries
LIB=.a
define AR
  @rm -f $@
  ar
endef
ARFLAGS=cr

# Command sequence for creating a directory.
# Note that directories will have forward slashes. Please
# make sure that this command accepts that (or use 'subst' first).
MKDIR=mkdir $(@:/=)

# System-dependent flags to pass to NASM
NASMFLAGS.SYSTEM=-f win32 -DEXTERNC_UNDERSCORE

# System dependent source files included into CSSYS library
SRC.SYS_CSSYS = libs/cssys/win32/printf.cpp libs/cssys/win32/timing.cpp \
  libs/cssys/win32/dir.cpp libs/cssys/win32/win32.cpp \
  libs/cssys/win32/loadlib.cpp support/gnu/getopt.c support/gnu/getopt1.c
SRC.SYS_CSSYS_EXE=libs/cssys/win32/exeentry.cpp
SRC.SYS_CSSYS_DLL=libs/cssys/win32/dllentry.cpp

# The C compiler
CC=gcc -c

# The C++ compiler
CXX=c++ -c

# The linker.
LINK=gcc

# Command sequence for creating a directory.
# Note that directories will have forward slashes. Please
# make sure that this command accepts that (or use 'subst' first).

# For using sockets we should link with sockets library
NETSOCK_LIBS=

# Extra parameters for 'sed' which are used for doing 'make depend'.
SYS_SED_DEPEND=-e "s/\.ob*j*\:/\$$O:/g"

# Flags for linking a GUI and a console executable
LFLAGS.EXE=-mwindows
LFLAGS.CONSOLE.EXE=

# Use makedep to build dependencies
#DEPEND_TOOL=mkdep
DEPEND_TOOL=cc

endif # ifeq ($(MAKESECTION),defines)

#--------------------------------------------------------------- confighelp ---#
ifeq ($(MAKESECTION),confighelp)

ifneq (,$(findstring command,$(SHELL))$(findstring COMMAND,$(SHELL)))
"=
|=�
endif

SYSHELP += \
  $(NEWLINE)echo $"  make cygwin       Prepare for building under and for $(DESCRIPTION.cygwin)$"

endif # ifeq ($(MAKESECTION),confighelp)

#---------------------------------------------------------------- configure ---#
ifeq ($(ROOTCONFIG),config)

SYSCONFIG=

endif # ifeq ($(ROOTCONFIG),config)
