# This is an include file for all the makefiles which describes system specific
# settings. Also have a look at mk/user.mak.

# Amiga port, using GCC
DESCRIPTION.amiga = Amiga with GCC

# Choose which drivers you want to build/use
DRIVERS+=cs3d/software cs2d/x2d csnetdrv/null csnetdrv/sockets \
  csnetman/null csnetman/simple cssnddrv/null cssndrdr/null \
  cssndrdr/software

#---------------------------------------------------- rootdefines & defines ---#
ifneq (,$(findstring defines,$(MAKESECTION)))

# Processor. Can be one of: INTEL, SPARC, POWERPC, M68K, UNKNOWN
# Currently, only M68K is supported in the Amiga port.
# POWERPC will be supported in the (hopefully near) future.
CPU=M68K
PROC=M68K

# Operating system. Can be one of: SOLARIS, LINUX, IRIX, BSD, UNIX, DOS, MACOS, AMIGAOS, WIN32, OS2, BE
OS=AMIGAOS

# Compiler. Can be one of: GCC, WCC (Watcom C++), MPWERKS, VC (Visual C++), UNKNOWN
COMP=GCC

# Processor version.  Can be one of -m68020, -m68040, -m68060, or blank (68000).
# The flags telling the compiler to optimize for a specific processor
CPUVER=

# Math coprocessor version.  Can be either -m68881 or blank (for none)
# The flag telling the compiler to optimize for FPU or not.
FPUVER=

endif # ifneq (,$(findstring defines,$(MAKESECTION)))

#------------------------------------------------------------------ defines ---#
ifeq ($(MAKESECTION),defines)

include mk/unix.mak

# Does this system require libsocket.a?
NEED_SOCKET_LIB=no

# Extra libraries needed on this system.
LIBS.EXE+=-ldl -lm  

# Where can the Zlib library be found on this system?
Z_LIBS=-Llibs/zlib -lz

# Where can the PNG library be found on this system?
PNG_LIBS=-Llibs/libpng -lpng

# Where can the JPG library be found on this system?
# TODO:  Manage to compile libjpeg.  (:
JPG_LIBS=-Llibs/libjpeg -ljpeg

# Where can the optional sound libraries be found on this system?
# TODO
SOUND_LIBS=

# Indicate where special include files can be found.
CFLAGS.INCLUDE=-I GG:os-include/amiga -I libs/zlib -I libs/libpng -I libs/libjpeg

# General flags for the compiler which are used in any case.
# -fno-exceptions and -fno-rtti have effect only for gcc >= 2.8.x
#CFLAGS.GENERAL=-Wall -fno-exceptions -fno-rtti
CFLAGS.GENERAL=-Wall $(CFLAGS.SYSTEM)

# Flags for the compiler which are used when optimizing.
CFLAGS.optimize=-O3 -fomit-frame-pointer -noixemul $(CPUVER) $(FPUVER)

# Flags for the compiler which are used when debugging.
CFLAGS.debug=-g3 -gstabs -Wall -O

# Flags for the compiler which are used when profiling.
CFLAGS.profile=-pg -O -g

# Flags for the compiler which are used when building a shared library.
# TODO
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

# System dependent source files included into CSSYS library
# TODO: system/amiga/loadlib.cpp  and  system/amiga/graphics2d.cpp
SRC.SYS_CSSYS=libs/cssys/amiga/Amiga.cpp libs/cssys/general/printf.cpp \
  libs/cssys/amiga/loadlib.cpp
# libs/cssys/amiga/Scanm68k.S

# The C compiler.
CC=gcc -c

# The C++ compiler.
CXX=g++ -c

# The linker.
LINK=g++

endif # ifeq ($(MAKESECTION),defines)

#--------------------------------------------------------------- confighelp ---#
ifeq ($(MAKESECTION),confighelp)

SYSHELP += \
  $(NEWLINE)echo $"  make amiga        Prepare for building under and for $(DESCRIPTION.amiga)$"

endif # ifeq ($(MAKESECTION),confighelp)

#---------------------------------------------------------------- configure ---#
ifeq ($(ROOTCONFIG),config)

# Currently this port does not support dynamic libraries
override USE_SHARED_PLUGINS = no

endif # ifeq ($(ROOTCONFIG),config)
