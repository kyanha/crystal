################################################################################
#                        This is the makefile for OS2/EMX
#                       *** (tested only with pgcc-1.0) ***
################################################################################

#---------------------------------------------------
# NOTE: OS/2 makefiles for libjpeg, libpng and zlib
# can be found in libs/cssys/os2 subdirectory
#---------------------------------------------------

# Friendly names for building environment
DESCRIPTION.os2gcc = OS/2 with GCC/EMX

# Choose which drivers you want to build/use
PLUGINS += video/canvas/csdive video/renderer/software

#---------------------------------------------------- rootdefines & defines ---#
ifneq (,$(findstring defines,$(MAKESECTION)))

.SUFFIXES: .exe .dll .lib .res .rc

# The path for system-specific sources
vpath %.cpp libs/cssys/os2

# Processor type. Most OS/2 systems runs on Intel's, I'm not sure IBM
# will ever ressurect the PowerPC version ... :-(
PROC=INTEL

# Operating system
OS=OS2

# Compiler
COMP=GCC

SYSMODIFIERS=USE_OMF=$(USE_OMF) USE_CRTDLL=$(USE_CRTDLL)

# The command to update target
UPD=cmd /c bin\\os2upd.cmd $@ DEST

endif # ifneq (,$(findstring defines,$(MAKESECTION)))

#------------------------------------------------------------------ defines ---#
ifeq ($(MAKESECTION),defines)

include mk/dos.mak

# Extra libraries needed on this system (beside drivers)
LIBS.EXE=

# Where can the Zlib library be found on this system?
Z_LIBS=-lz

# Where can the PNG library be found on this system?
PNG_LIBS=-lpng

# Where can the JPG library be found on this system?
JPG_LIBS=-ljpeg

# Where can the optional sound libraries be found on this system?
SOUND_LIBS=

# Does this system require libsocket.a?
NEED_SOCKET_LIB=yes

# Indicate where special include files can be found.
CFLAGS.INCLUDE=-Ilibs/zlib -Ilibs/libpng -Ilibs/libjpeg

# General flags for the compiler which are used in any case.
CFLAGS.GENERAL=-Wall -Zmt $(CFLAGS.SYSTEM)

# Flags for the compiler which are used when optimizing.
CFLAGS.optimize=-s -O6 -fomit-frame-pointer -ffast-math

# Flags for the compiler which are used when debugging.
CFLAGS.debug=-g

# Flags for the compiler which are used when profiling.
CFLAGS.profile=-pg -O -g

# Flags for the compiler which are used when building a shared library.
CFLAGS.DLL=

# General flags for the linker which are used in any case.
LFLAGS.GENERAL=-Zmt

# Flags for the linker which are used when optimizing/debugging/profiling.
LFLAGS.optimize=-s
LFLAGS.debug=-g
LFLAGS.profile=-pg
ifeq ($(USE_OMF)/$(USE_CRTDLL),yes/no)
  LFLAGS.optimize+=-Zsmall-conv -Zsys
  LFLAGS.debug+=-Zsmall-conv -Zsys
  LFLAGS.profile+=-Zsmall-conv -Zsys
else
  LFLAGS.optimize+=-Zcrtdll
  LFLAGS.debug+=-Zcrtdll
  LFLAGS.profile+=-Zcrtdll
endif

# Flags for linker when building a GUI executable
LFLAGS.EXE+=-Zstack 512

# Flags for the linker which are used when building a shared library.
LFLAGS.DLL=-Zdll

# Typical extension for objects and static libraries
ifeq ($(USE_OMF),yes)
  O=.obj
  LIB=.lib
  define AR
	@rm -f $@
	emxomfar
  endef
  ARFLAGS=-p32 cr
  CFLAGS.GENERAL += -Zomf
  LFLAGS.GENERAL += -Zomf
  NASMFLAGS.SYSTEM=-f obj
else
  O=.o
  LIB=.a
  define AR
	@rm -f $@
	ar
  endef
  ARFLAGS=cr
  NASMFLAGS.SYSTEM=-f aout -DEXTERNC_UNDERSCORE
endif

# System dependent source files included into CSSYS library
SRC.SYS_CSSYS = libs/cssys/general/printf.cpp libs/cssys/general/timing.cpp \
  libs/cssys/os2/csos2.cpp libs/cssys/os2/loadlib.cpp \
  libs/cssys/os2/scancode.cpp support/gnu/getopt.c support/gnu/getopt1.c
SRC.SYS_CSSYS_DLL=libs/cssys/os2/dllentry.cpp

# The C compiler (autodetected)
#CC=gcc -c

# The C++ compiler (autodetected)
#CXX=gcc -c

# The Resource Compiler (autodetected)
#RC=rc
RCFLAGS=-r -I libs $(RCFLAGS.SYSTEM)

# Command sequence for creating a directory.
# Note that directories will have forward slashes. Please
# make sure that this command accepts that (or use 'subst' first).
ifneq (,$(findstring cmd,$(SHELL))$(findstring CMD,$(SHELL)))
  MKDIR = mkdir $(subst /,\,$(@:/=))
else
  MKDIR = mkdir $@
endif

# For using sockets we should link with sockets library
NETSOCK_LIBS=-lsocket

# Override linker with os2link.exe
LINK=@$(OS2LINK) --linker=$(LD) --description="$(DESCRIPTION.$@)" --verbose --out=$(OUT)
LFLAGS.CONSOLE.EXE=--console

# Defineds for OpenGL 3D driver
OPENGL.LIBS.DEFINED=1
CFLAGS.GL3D+=-I/toolkit/h
LIBS.LOCAL.GL3D+=-lopengl

# Extra parameters for 'sed' which are used for doing 'make depend'.
SYS_SED_DEPEND=-e "s/\.ob*j*\:/\$$O:/g"

# Use makedep to build dependencies
DEPEND_TOOL=mkdep

endif # ifeq ($(MAKESECTION),defines)

#--------------------------------------------------------------- confighelp ---#
ifeq ($(MAKESECTION),confighelp)

ifneq (,$(findstring cmd,$(SHELL))$(findstring CMD,$(SHELL)))
"=
|=�
endif

SYSHELP += \
  $(NEWLINE)echo $"  make os2gcc       Prepare for building under and for $(DESCRIPTION.os2gcc)$"

# System-dependent help commands
#  
SYSMODIFIERSHELP += \
  $(NEWLINE)echo $"  USE_OMF=yes$|no (OS/2)$" \
  $(NEWLINE)echo $"      Use OMF object module format (yes) vs a.out format (no)$" \
  $(NEWLINE)echo $"  USE_CRTDLL=yes$|no (OS/2)$" \
  $(NEWLINE)echo $"      Use EMX C runtime DLLs (yes: default) or don`t (no)$"

endif # ifeq ($(MAKESECTION),confighelp)

#---------------------------------------------------------------- configure ---#
ifeq ($(ROOTCONFIG),config)

# Default value for USE_OMF
ifndef USE_OMF
  USE_OMF = yes
endif

# Default value for USE_CRTDLL
ifndef USE_CRTDLL
  USE_CRTDLL = yes
endif

define SYSCONFIG
  @echo USE_OMF = $(USE_OMF)>>config.tmp
  @echo USE_CRTDLL = $(USE_CRTDLL)>>config.tmp
  @cmd /c bin\\os2conf.cmd SHELL $(SHELL)>>config.tmp
endef

endif # ifeq ($(ROOTCONFIG),config)

#------------------------------------------------------------------ targets ---#
ifeq ($(MAKESECTION),targets)

ifndef OS2LINK

# Define a rule to build os2link.exe before any other file,
# if it is not pre-installed
OS2LINK = ./os2link$(EXE)

libs/cssys/os2/os2gcc.mak: $(OS2LINK)

$(OS2LINK): libs/cssys/os2/support/os2link.cpp
	$(LD) $(LFLAGS.@) $(CFLAGS.GENERAL) $(CFLAGS.optimize) $(LFLAGS.optimize) $^ -Zomf -Zlinker /PM:VIO

endif

ifndef STARTFS

# Also we need the full-screen launcher for MGL 2D driver
STARTFS = ./startfs$(EXE)

mgl2d: $(STARTFS)

$(STARTFS): libs/cssys/os2/support/startfs.cpp
	$(LD) $(LFLAGS.@) $(CFLAGS.GENERAL) $(CFLAGS.optimize) $(LFLAGS.optimize) $^ -Zomf -Zlinker /PM:NOVIO

endif

endif # ifeq ($(MAKESECTION),targets)
