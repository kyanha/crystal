###############################################################################
#                       This is the makefile for DOS/DJGPP
###############################################################################

# Friendly names for building environment
DESCRIPTION.djgpp = DOS/DJGPP
DESCRIPTION.OS.djgpp = DOS

ifeq ($(USE_ALLEGRO),yes)
  PLUGINS += video/canvas/allegro
else
  PLUGINS += video/canvas/dosraw
endif

#--------------------------------------------------- rootdefines & defines ---#
ifneq (,$(findstring defines,$(MAKESECTION)))

.SUFFIXES: .exe

# Processor type
PROC=X86

# "Operating system", if it can be called so :-/
OS=DOS

# Compiler
COMP=GCC

# The command to update target
UPD=libs/cssys/djgpp/dosupd.bat $(subst /,\,$@ DEST)

endif # ifneq (,$(findstring defines,$(MAKESECTION)))

#---------------------------------------------------------------- defines ---#
ifeq ($(MAKESECTION),defines)

include mk/dos.mak

vpath %.s libs/cssys/djgpp

# Typical object file extension
O=.o

# Typical extension for dynamic libraries on this system.
DLL=.dxe

# Typical extension for static libraries
LIB=.a
AR=ar
ARFLAGS=cr

# Typical prefix for library filenames
LIB_PREFIX=lib

# Extra libraries needed on this system.
LIBS.EXE = -lm
LIBS.SORT = -lm
ifeq ($(USE_PLUGINS),yes)
LIBS.EXE += -ldl
LIBS.SORT += -ldl
endif

# Where can the Zlib library be found on this system?
Z_LIBS=-lz$(ZLIB.SUFFIX)

# Where can the PNG library be found on this system?
PNG_LIBS=-lpng$(LIBPNG.SUFFIX)

# Where can the JPG library be found on this system?
JPG_LIBS=-ljpeg$(LIBJPEG.SUFFIX)

# Where can the optional sound libraries be found on this system?
SOUND_LIBS=

# Indicate where special include files can be found.
CFLAGS.INCLUDE=-Ilibs/cssys/djgpp/libs/libpng -Ilibs/cssys/djgpp/libs/libjpeg -Ilibs/cssys/djgpp/libs/zlib

# General flags for the compiler which are used in any case.
CFLAGS.GENERAL=-Wall $(CFLAGS.SYSTEM)

# Flags for the compiler which are used when optimizing.
CFLAGS.optimize=-s -O6 -fomit-frame-pointer -ffast-math

# Flags for the compiler which are used when debugging.
CFLAGS.debug=-g3

# Flags for the compiler which are used when profiling.
CFLAGS.profile=-pg -O -g

# Flags for the compiler which are used when building a shared library.
CFLAGS.DLL=

# General flags for the linker which are used in any case.
LFLAGS.GENERAL=-Llibs/cssys/djgpp/libs/libpng -Llibs/cssys/djgpp/libs/libjpeg -Llibs/cssys/djgpp/libs/zlib

# Flags for the linker which are used when optimizing.
LFLAGS.optimize=-s

# Flags for the linker which are used when debugging.
LFLAGS.debug=-g

# Flags for the linker which are used when profiling.
LFLAGS.profile=-pg

# Flags for the linker which are used when building a shared library.
LFLAGS.DLL=-Llibs/cssys/djgpp/libs/libpng -Llibs/cssys/djgpp/libs/libjpeg -Llibs/cssys/djgpp/libs/zlib

# System-dependent flags to pass to NASM
NASMFLAGS.SYSTEM=-f coff -DEXTERNC_UNDERSCORE

# System dependent source files included into CSSYS library
SRC.SYS_CSSYS = \
  libs/cssys/djgpp/djgpp.cpp \
  libs/cssys/djgpp/printf.cpp \
  libs/cssys/djgpp/djtimer.cpp \
  libs/cssys/djgpp/djmousys.s \
  libs/cssys/djgpp/djkeysys.s \
  libs/cssys/djgpp/sysroot.cpp \
  libs/cssys/general/getopt.cpp \
  libs/cssys/general/instpath.cpp \
  libs/cssys/general/runloop.cpp \
  libs/cssys/general/sysinit.cpp \
  libs/cssys/general/username.cpp
ifeq ($(USE_PLUGINS),yes)
SRC.SYS_CSSYS += libs/cssys/djgpp/loadlib.cpp libs/cssys/general/findlib.cpp
endif

# The C compiler.
CC=gcc -c

# The C++ compiler.
CXX=gcc -c

# The linker.
LINK=gcc

# Extra parameters for 'sed' which are used for doing 'make depend'.
SYS_SED_DEPEND=

# Use makedep to build dependencies
DEPEND_TOOL=mkdep

endif # ifeq ($(MAKESECTION),defines)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

# How to make a shared AKA dynamic library
DO.SHARED.PLUGIN.CORE = dxe2gen -o $@ $(^^) $(L^) -U -E $(patsubst %.dxe,%,$(notdir $@))_ -E djgpp $(LFLAGS.DLL) $(LFLAGS.L)$(OUT)

endif # ifeq ($(MAKESECTION),postdefines)

#-------------------------------------------------------------- confighelp ---#
ifeq ($(MAKESECTION),confighelp)

SYSHELP += \
  $(NEWLINE)echo $"  make djgpp        Prepare for building on $(DESCRIPTION.djgpp)$"

endif # ifeq ($(MAKESECTION),confighelp)

#--------------------------------------------------------------- configure ---#
ifeq ($(MAKESECTION)/$(ROOTCONFIG),rootdefines/config)

#SYSCONFIG=cmd /E:8192 /C libs\cssys\djgpp\dosconf.bat
SYSCONFIG=command /E:8192 /C libs\cssys\djgpp\dosconf.bat
#SYSCONFIG=libs\cssys\djgpp\dosconf.bat
# Check if "echo" executable is not installed (thus using dumb COMMAND.COM's echo)
ifeq ($(shell echo ""),"")
SYSCONFIG += $(NEWLINE)type libs\cssys\djgpp\dosconf.var>>config.tmp
endif

endif # rootdefines & config
