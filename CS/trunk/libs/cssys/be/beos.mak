# This is an include file for all the makefiles which describes system
# specific settings. Also have a look at mk/user.mak.

# Friendly names for building environment
DESCRIPTION.beos = BeOS

# Choose which drivers you want to build/use
PLUGINS += video/renderer/software video/canvas/be \
  video/canvas/openglbe video/renderer/opengl

# Automatically detect presence of Glide packages.
GLIDE_VERSIONS =

# Glide 2 package.
GLIDE2_INCLUDE = /boot/develop/headers/3dfx/glide2
ifneq ($(wildcard $(GLIDE2_INCLUDE)),)
GLIDE_VERSIONS += 2
PLUGINS += video/canvas/beglide2
endif

# Glide 3D renderer.
ifneq ($(strip $(GLIDE_VERSIONS)),)
PLUGINS += video/renderer/glide
endif

# Be compiler does not currently grok CS assembly
override DO_ASM=no

#--------------------------------------------------- rootdefines & defines ---#
ifneq (,$(findstring defines,$(MAKESECTION)))

# Processor. Can be one of: INTEL, SPARC, POWERPC, M68K, UNKNOWN
PROC=INTEL

# Operating system. Can be one of: SOLARIS, LINUX, IRIX, BSD, UNIX, DOS,
# MACOS, AMIGAOS, WIN32, OS2, BE
OS=BE

# Compiler. Can be one of: GCC, WCC (Watcom C++), MPWERKS, VC (Visual C++),
# UNKNOWN
COMP=GCC

endif # ifneq (,$(findstring defines,$(MAKESECTION)))

#----------------------------------------------------------------- defines ---#
ifeq ($(MAKESECTION),defines)

include mk/unix.mak

# Typical extension for dynamic libraries on this system.
DLL=.plugin

# Does this system require libsocket.a?
NEED_SOCKET_LIB=no

# Extra libraries needed on this system.
LIBS.EXE+=-lroot -lbe -lgame -ltextencoding

# Where can the Zlib library be found on this system?
Z_LIBS=-lz

# Where can the PNG library be found on this system?
PNG_LIBS=-lpng

# Where can the JPG library be found on this system?
JPG_LIBS=-Llibs/libjpeg -ljpeg

# Where can the optional sound libraries be found on this system?
SOUND_LIBS=

# Indicate where special include files can be found.
CFLAGS.INCLUDE=$(CFLAGS.I). $(CFLAGS.I)include $(CFLAGS.I)libs \
 $(CFLAGS.I)apps $(CFLAGS.I)support $(CFLAGS.I)libs/libjpeg

# General flags for the compiler which are used in any case.
CFLAGS.GENERAL=-Wall -Wno-multichar -Wno-ctor-dtor-privacy 

# Flags for the compiler which are used when optimizing.
# *NOTE* Must disable schedule-insns2 optimization since it causes QInt() to
# failin some cases; for instance, csGraphics2D::DrawLine().
CFLAGS.optimize=-O3 -fno-schedule-insns2

# Flags for the compiler which are used when debugging.
CFLAGS.debug=-g -O0

# Flags for the compiler which are used when profiling.
CFLAGS.profile=-pg -O -g

# Flags for the compiler which are used when building a shared library.
CFLAGS.DLL=

# General flags for the linker which are used in any case.
LFLAGS.GENERAL=

# Flags for the linker which are used when debugging.
LFLAGS.debug= -gdwarf-2

# Flags for the linker which are used when profiling.
LFLAGS.profile=-pg

# Flags for the linker which are used when building a shared library.
LFLAGS.DLL= -nostart

# System dependent source files included into CSSYS library
SRC.SYS_CSSYS=$(wildcard libs/cssys/be/*.cpp) \
  libs/cssys/general/findlib.cpp \
  libs/cssys/general/printf.cpp \
  libs/cssys/general/getopt.cpp

# Where to put dynamic libraries on this system?
OUTDLL=add-ons/

# The C compiler.
CC=gcc -c

# The C++ compiler.
CXX=gcc -c

# The linker.
LINK=gcc

# Defines for OpenGL 3D driver
OPENGL.LIBS.DEFINED=1
CFLAGS.GL3D+=$(CFLAGS.I)/boot/develop/headers/be/opengl 
LIBS.LOCAL.GL3D+=$(LFLAGS.l)GL

# Defines for Glide2 driver.
ifneq ($(findstring 2,$(GLIDE_VERSIONS)),)
GLIDE2_PATH=-I$(GLIDE2_INCLUDE)
# Should be libglide2x.so for linker flag '-l' to work, but Be goofed name.
GLIDE2_LIB=/system/lib/glide2x.so
endif

endif # ifeq ($(MAKESECTION),defines)

#-------------------------------------------------------------- confighelp ---#
ifeq ($(MAKESECTION),confighelp)

SYSHELP += \
  $(NEWLINE)echo $"  make beos         Prepare for building on $(DESCRIPTION.beos)$"

endif # ifeq ($(MAKESECTION),confighelp)

#--------------------------------------------------------------- configure ---#
ifeq ($(MAKESECTION)/$(ROOTCONFIG),rootdefines/config)

SYSCONFIG += $(NEWLINE)bin/haspythn.sh >>config.tmp
SYSCONFIG += $(NEWLINE)echo override DO_ASM = $(DO_ASM)>>config.tmp
SYSCONFIG += $(NEWLINE)echo CS_BUILTIN_SIZED_TYPES = yes>>config.tmp

endif # rootdefines & config
