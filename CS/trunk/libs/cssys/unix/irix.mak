# This is an include file for all the makefiles which describes system specific
# settings. Also have a look at mk/user.mak.

# Friendly names for building environment
DESCRIPTION.irix = IRIX

# Choose which drivers you want to build/use
PLUGINS+=cs2d/softx cs3d/software cssndrdr/software

# uncomment the following to build GGI 2D driver
#PLUGINS+=cs2d/ggi
# uncomment the following to build OpenGL stuff
PLUGINS+=cs3d/opengl cs2d/openglx

#---------------------------------------------------- rootdefines & defines ---#
ifneq (,$(findstring defines,$(MAKESECTION)))

# Processor is autodetected and written to config.mak
#PROC=MIPS

# Operating system. Can be one of: SOLARIS, LINUX, IRIX, BSD, UNIX, DOS, MACOS, AMIGAOS, WIN32, OS2, BE
OS=IRIX

# Compiler. Can be one of: GCC, WCC (Watcom C++), MPWERKS, VC (Visual C++), UNKNOWN
COMP=GCC

endif # ifneq (,$(findstring defines,$(MAKESECTION)))

#------------------------------------------------------------------ defines ---#
ifeq ($(MAKESECTION),defines)

include mk/unix.mak

# Does this system require libsocket.a?
NEED_SOCKET_LIB=yes

# Extra libraries needed on this system.
LIBS.EXE=-ldl -lm

# Where can the Zlib library be found on this system?
Z_LIBS=-Llibs/zlib -lz

# Where can the PNG library be found on this system?
PNG_LIBS=-Llibs/libpng -lpng

# Where can the JPG library be found on this system?
JPG_LIBS=-Llibs/libjpeg -ljpeg

# Where can the optional sound libraries be found on this system?
SOUND_LIBS=

# Indicate where special include files can be found.
CFLAGS.INCLUDE=-Ilibs/zlib -Ilibs/libpng -Ilibs/libjpeg

# General flags for the compiler which are used in any case.
CFLAGS.GENERAL=-Wall $(CFLAGS.SYSTEM)

# Flags for the compiler which are used when optimizing.
CFLAGS.optimize=-O -fomit-frame-pointer

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
#LFLAGS.DLL=-Wl,-G -nostdlib -lgcc -lc
LFLAGS.DLL=-shared

# System dependent source files included into CSSYS library
SRC.SYS_CSSYS=libs/cssys/unix/unix.cpp libs/cssys/unix/loadlib.cpp \
  libs/cssys/general/printf.cpp libs/cssys/unix/utiming.cpp \
  support/gnu/getopt.c support/gnu/getopt1.c

# The C compiler.
CC=gcc -c

# The C++ compiler.
CXX=gcc -c

# The linker.
LINK=gcc

endif # ifeq ($(MAKESECTION),defines)

#--------------------------------------------------------------- confighelp ---#
ifeq ($(MAKESECTION),confighelp)

SYSHELP += \
  $(NEWLINE)echo $"  make irix         Prepare for building under and for $(DESCRIPTION.irix)$"

endif # ifeq ($(MAKESECTION),confighelp)

#---------------------------------------------------------------- configure ---#
ifeq ($(ROOTCONFIG),config)

define SYSCONFIG
  bin/unixconf.sh irix >>config.tmp
  echo "Don't forget to set USE_MESA=0 in user.mak"
endef

endif # ifeq ($(ROOTCONFIG),config)
