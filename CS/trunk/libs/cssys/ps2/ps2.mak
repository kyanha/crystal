# This is an include file for all the makefiles which describes system specific
# settings. Also have a look at mk/user.mak.

# Friendly names for building environment
DESCRIPTION.ps2 = Playstation 2

# Choose which 2D/3D driver combinations you want to build/use
#PLUGINS+=video/canvas/softx video/renderer/software
#PLUGINS+=video/renderer/opengl video/canvas/openglx
#PLUGINS+=video/canvas/linex

# uncomment the following to build SVGALIB and/or GGI 2D drivers
#PLUGINS+=video/canvas/svgalib
#PLUGINS+=video/canvas/ggi 

# uncomment the following to build sound drivers
#PLUGINS+=sound/driver/oss sound/renderer/software

# uncomment some of the following if you have a special MESA version that uses some 
# of the following hardware/software renderers
# Also set the entry Driver in section Display of opengl.cfg
#PLUGINS+=video/canvas/openglx/glide
#PLUGINS+=video/canvas/openglx/svga
#PLUGINS+=video/canvas/openglx/empty

# uncomment the following to build Glide stuff
#GLIDE_VERSIONS=2 3
ifneq ($(strip $(GLIDE_VERSIONS)),)
PLUGINS+=video/canvas/unxglide
PLUGINS+=video/renderer/glide
endif
GLIDE2_PATH=-I/usr/include/glide2
GLIDE3_PATH=-I/usr/include/glide3
#---------------------------------------------------- rootdefines & defines ---#
ifneq (,$(findstring defines,$(MAKESECTION)))

# Processor is autodetected and written to config.mak
PROC=EE

# Operating system. Can be one of: SOLARIS, LINUX, IRIX, BSD, UNIX, DOS, MACOS, AMIGAOS, WIN32, OS2, BE
OS=PS2

# Compiler. Can be one of: GCC, WCC (Watcom C++), MPWERKS, VC (Visual C++), UNKNOWN
COMP=GCC

endif # ifneq (,$(findstring defines,$(MAKESECTION)))

#------------------------------------------------------------------ defines ---#
ifeq ($(MAKESECTION),defines)

include mk/unix.mak

# Does this system require libsocket.a?
NEED_SOCKET_LIB=no

# Extra libraries needed on this system.
LIBS.EXE+=-lgcc -lstdc++ -lm  

# Where can the Zlib library be found on this system?
Z_LIBS=

# Where can the PNG library be found on this system?
PNG_LIBS=

# Where can the JPG library be found on this system?
JPG_LIBS=

# Where can the optional sound libraries be found on this system?
SOUND_LIBS=

# Indicate where special include files can be found.
CFLAGS.INCLUDE=

# General flags for the compiler which are used in any case.
# -fno-exceptions and -fno-rtti have effect only for gcc >= 2.8.x
CFLAGS.GENERAL=-Wall -Wunused -W $(CFLAGS.SYSTEM)

# Flags for the compiler which are used when optimizing.
ifeq ($(PROC),INTEL)
  CFLAGS.optimize=-O6 -fomit-frame-pointer -ffast-math
else
  CFLAGS.optimize=-O -fomit-frame-pointer
endif

# Flags for the compiler which are used when debugging.
CFLAGS.debug=-g3 -gstabs

CFLAGS.memdebug=-g3 -gstabs

# Flags for the compiler which are used when profiling.
CFLAGS.profile=-pg -O -g

# Flags for the compiler which are used when building a shared library.
CFLAGS.DLL=

# General flags for the linker which are used in any case.
LFLAGS.GENERAL=

# Flags for the linker which are used when debugging.
LFLAGS.debug=-g3

LFLAGS.memdebug=-g3

# Flags for the linker which are used when profiling.
LFLAGS.profile=-pg

# Flags for the linker which are used when building a shared library.
#LFLAGS.DLL=-Wl,-shared -nostdlib -lgcc
LFLAGS.DLL=-Wl,-shared

# System-dependent flags to pass to NASM
NASMFLAGS.SYSTEM=-f elf

# System dependent source files included into CSSYS library
SRC.SYS_CSSYS = \
  libs/cssys/general/findlib.cpp \
  libs/cssys/general/printf.cpp libs/cssys/general/getopt.cpp
SRC.SYS_CSSYS_DLL = libs/cssys/unix/dummy.cpp

# The C compiler.
CC=ee-g++ -c

# The C++ compiler.
CXX=ee-g++ -c

# The linker.
LINK=ee-g++

endif # ifeq ($(MAKESECTION),defines)

#--------------------------------------------------------------- confighelp ---#
ifeq ($(MAKESECTION),confighelp)

SYSHELP += \
  $(NEWLINE)echo $"  make ps2          Prepare for building on $(DESCRIPTION.ps2)$"

endif # ifeq ($(MAKESECTION),confighelp)

#---------------------------------------------------------------- configure ---#
ifeq ($(ROOTCONFIG),config)

SYSCONFIG=echo PROC=ee >>config.tmp; echo OS=PS2 >>config.tmp; echo COMP=GCC >>config.tmp; echo CS_LITTLE_ENDIAN=1 >>config.tmp

endif # ifeq ($(ROOTCONFIG),config)
