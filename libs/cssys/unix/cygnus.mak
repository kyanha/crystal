# This is an include file for all the makefiles which describes system specific
# settings. Also have a look at mk/user.mak.

# Friendly names for building environment
DESCRIPTION.cygnus = Windows with Cygnus

# Choose which drivers you want to build/use
PLUGINS+=video/canvas/softx video/renderer/software \
  sound/driver/oss sound/renderer/software

# uncomment the following to build SVGALIB 2D driver
PLUGINS+=video/canvas/svgalib
# uncomment the following to build GGI 2D driver
#PLUGINS+=video/canvas/ggi 
# uncomment the following to build OpenGL stuff
PLUGINS+=video/renderer/opengl video/canvas/openglx
# uncomment the following to build Glide stuff
#PLUGINS+=video/renderer/glide2 video/canvas/unxglide2

#---------------------------------------------------- rootdefines & defines ---#
ifneq (,$(findstring defines,$(MAKESECTION)))

# Processor is autodetected and written to config.mak
#PROC=

# Operating system. Can be one of: SOLARIS, LINUX, IRIX, BSD, UNIX, DOS, MACOS, AMIGAOS, WIN32, OS2, BE
OS=WIN32

# Compiler. Can be one of: GCC, WCC (Watcom C++), MPWERKS, VC (Visual C++), UNKNOWN
COMP=GCC

endif # ifneq (,$(findstring defines,$(MAKESECTION)))

#------------------------------------------------------------------ defines ---#
ifeq ($(MAKESECTION),defines)

#include mk/dos.mak

# Does this system require libsocket.a?
NEED_SOCKET_LIB=no

# Extra libraries needed on this system.
LIBS.EXE+=-ldl -lm  

# Where can the Zlib library be found on this system?
Z_LIBS=-lz

# Where can the PNG library be found on this system?
PNG_LIBS=-lpng

# Where can the JPG library be found on this system?
JPG_LIBS=-ljpeg

# Where can the optional sound libraries be found on this system?
SOUND_LIBS=

# Indicate where special include files can be found.
CFLAGS.INCLUDE=

# General flags for the compiler which are used in any case.
# -fno-exceptions and -fno-rtti have effect only for gcc >= 2.8.x
CFLAGS.GENERAL=-Wall $(CFLAGS.SYSTEM)

# Flags for the compiler which are used when optimizing.
ifeq ($(PROC),INTEL)
  CFLAGS.optimize=-O6 -fomit-frame-pointer -ffast-math
else
  CFLAGS.optimize=-O -fomit-frame-pointer
endif

# Flags for the compiler which are used when debugging.
CFLAGS.debug=-g3 -gstabs

# Flags for the compiler which are used when profiling.
CFLAGS.profile=-pg -O -g

# Flags for the compiler which are used when building a shared library.
CFLAGS.DLL=

# General flags for the linker which are used in any case.
LFLAGS.GENERAL=

# Flags for the linker which are used when debugging.
LFLAGS.debug=-g3

# Flags for the linker which are used when profiling.
LFLAGS.profile=-pg

# Flags for the linker which are used when building a shared library.
#LFLAGS.DLL=-Wl,-shared -nostdlib -lgcc
LFLAGS.DLL=-Wl,-shared

# System-dependent flags to pass to NASM
NASMFLAGS.SYSTEM=-f elf

# System dependent source files included into CSSYS library
SRC.SYS_CSSYS = libs/cssys/unix/unix.cpp libs/cssys/unix/utiming.cpp \
  libs/cssys/unix/loadlib.cpp libs/cssys/general/findlib.cpp \
  libs/cssys/general/printf.cpp
SRC.SYS_CSSYS_DLL = libs/cssys/unix/dummy.cpp

# The C compiler.
CC=g++ -c

# The C++ compiler.
CXX=g++ -c

# The linker.
LINK=g++

# The library (archive) manager
AR=ar
ARFLAGS=cr

# Command sequence for creating a directory.
# Note that directories will have forward slashes. Please
# make sure that this command accepts that (or use 'subst' first).
MKDIR=mkdir $(@:/=)

# Extra parameters for 'sed' which are used for doing 'make depend'.
SYS_SED_DEPEND=

# If we don't use -fpic we don't need separate output directories
ifeq ($(CFLAGS.DLL),)
OUTSUFX.yes=
endif

#==================================================
# Extra operation system dependent options.
#==================================================

# Include support for the XSHM extension in Crystal Space.
# Note that you don't need to disable this if you don't have XSHM
# support in your X server because Crystal Space will autodetect
# the availability of XSHM.
DO_SHM=yes

endif # ifeq ($(MAKESECTION),defines)

#--------------------------------------------------------------- confighelp ---#
ifeq ($(MAKESECTION),confighelp)

SYSHELP += \
  $(NEWLINE)echo $"  make cygnus       Prepare for building on $(DESCRIPTION.cygnus)$"

endif # ifeq ($(MAKESECTION),confighelp)

#---------------------------------------------------------------- configure ---#
ifeq ($(ROOTCONFIG),config)

SYSCONFIG=bin/unixconf.sh cygnus >>config.tmp

endif # ifeq ($(ROOTCONFIG),config)
