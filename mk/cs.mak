#==============================================================================
# These are the dependencies for Crystal Space.  This is an include file for
# the makefiles.  Don't use this directly as a makefile.
#==============================================================================

# The following two symbols are intended to be used in "echo" commands.
# config.mak can override them depending on configured platform's requirements.
"='
|=|

# The following include file should define all variables needed for this
# makefile to work.  Please do not change the include order unless you know
# what you are doing!
include config.mak
include $(TARGET_MAKEFILE)
include mk/user.mak
include mk/common.mak

.PHONY: depend clean cleanlib cleandep distclean maintainerclean show

# Remove all standard suffixes to speed up make lookups
.SUFFIXES:

#-------------- Several definitions to make this file compiler-independent ---#
# Flags for C compiler to direct output to the rule target
CFLAGS.@ = -o $@
# Flags for linker to direct output to the rule target
LFLAGS.@ = -o $@
# Flags for indicating to linker an additional library path
LFLAGS.L = -L
# Flags for indicating an additional library to linker
LFLAGS.l = -l
# Command-line flag to define a macro from command-line
CFLAGS.D = -D
# Command-line flag to set include directories
CFLAGS.I = -I
# Flags for librarian to direct output to the rule target
ARFLAGS.@=$@
# Object file extension
O=.o

ifeq ($(USE_PLUGINS),no)
  override MAKE_DLL=no
endif

# The following include should re/define system-dependent variables
MAKESECTION=defines
include mk/subs.mak
include mk/nasm.mak

ifeq ($(USE_SHARED_LIBS),yes)
  DO.LIBRARY = $(DO.SHARED.LIBRARY)
  LIB_SUFFIX = $(DLL)
else
  DO.LIBRARY = $(DO.STATIC.LIBRARY)
  LIB_SUFFIX = $(LIB)
endif

.SUFFIXES: $O $(EXE) $(LIB) $(DLL) .S .c .cpp .h .asm .ash .y

# Define paths automatically searched for source files
vpath %.cpp

# Directory for generated files (those removed at "make clean/distclean" time).
OUTBASE=out

# Directory for object files.
OUTOS=$(OUTBASE)/$(OS)
OUTPROC=$(OUTOS)/$(PROC)
OUT=$(OUTPROC)/$(MODE)$(OUTSUFX.$(MAKE_DLL))

# Directory for derived (transient) source files.
OUTDERIVED=$(OUTPROC)/derived

# Directory for plugin modules.
ifeq (,$(OUTDLL))
OUTDLL=.
endif

# Collective output directory list.
OUTDIRS = $(OUTBASE) $(OUTOS) $(OUTPROC) $(OUT) $(OUTDERIVED)
ifneq (.,$(OUTDLL))
OUTDIRS += $(OUTDLL)
endif

#==============================================================================

CFLAGS.INCLUDE+=$(CFLAGS.I). $(CFLAGS.I)./include $(CFLAGS.I)./libs \
  $(CFLAGS.I)./plugins $(CFLAGS.I)./apps
  
CFLAGS=$(CFLAGS.D)__CRYSTAL_SPACE__ $(CFLAGS.GENERAL) $(CFLAGS.$(MODE))
LFLAGS=$(LFLAGS.GENERAL) $(LFLAGS.$(MODE)) $(LFLAGS.L)$(OUT)
LIBS=$(LIBS.EXE)

ifeq ($(MAKE_DLL),yes)
  CFLAGS+=$(CFLAGS.DLL)
endif
ifeq ($(DO_ASM),no)
  CFLAGS+=$(CFLAGS.D)NO_ASSEMBLER
endif
ifeq ($(MODE),debug)
  CFLAGS+=$(CFLAGS.D)CS_DEBUG
endif

# Use $(^^) instead of $^ when you need all dependencies except libraries
^^=$(filter-out %$(LIB_SUFFIX),$^)
# Use $(<<) instead of $< to allow system-dependent makefiles to override
<<=$<
# Use $(L^) to link with all libraries specified as dependencies
L^=$(addprefix $(LFLAGS.l),$(subst $(SPACE)$(LIB_PREFIX),$(SPACE),\
  $(basename $(notdir $(filter %$(LIB_SUFFIX),$+)))))

# How to compile a .c source
DO.COMPILE.C = $(CC) $(CFLAGS.@) $(<<) $(CFLAGS) $(CFLAGS.INCLUDE)
# How to compile a .cpp source
DO.COMPILE.CPP = $(CXX) $(CFLAGS.@) $(<<) $(CFLAGS) $(CFLAGS.INCLUDE)
# How to compile a GAS source
DO.COMPILE.S = \
  $(CC) $(CFLAGS.@) -x assembler-with-cpp $(<<) $(CFLAGS) $(CFLAGS.INCLUDE)
# How to compile a NASM source
DO.COMPILE.ASM = $(NASM.BIN) $(NASM.@) $(NASMFLAGS) $(<<)
# How to make a static library
DO.STATIC.LIBRARY = $(AR) $(ARFLAGS) $(ARFLAGS.@) $(^^)
# How to make a shared/dynamic library
DO.SHARED.LIBRARY = \
  $(LINK) $(LFLAGS.DLL) $(LFLAGS.@) $(^^) $(L^) $(LIBS) $(LFLAGS)
# How to make a static plugin
DO.STATIC.PLUGIN.PREAMBLE =
DO.STATIC.PLUGIN.CORE = $(AR) $(ARFLAGS) $(ARFLAGS.@) $(^^)
DO.STATIC.PLUGIN.POSTAMBLE =
# How to make a shared plugin
DO.SHARED.PLUGIN.PREAMBLE =
DO.SHARED.PLUGIN.CORE = \
  $(LINK) $(LFLAGS.DLL) $(LFLAGS.@) $(^^) $(L^) $(LIBS) $(LFLAGS)
DO.SHARED.PLUGIN.POSTAMBLE =
# How to link a console executable
DO.LINK.CONSOLE.EXE = $(LINK) $(LFLAGS) $(LFLAGS.CONSOLE.EXE) $(LFLAGS.@) \
  $(^^) $(L^) $(LIBS) $(LIBS.EXE.PLATFORM)
# How to link a graphical executable
DO.LINK.EXE = $(LINK) $(LFLAGS) $(LFLAGS.EXE) $(LFLAGS.@) \
  $(^^) $(L^) $(LIBS) $(LIBS.EXE.PLATFORM)

# How to do either a dynamic or static library (depending on MAKE_DLL)
ifeq ($(MAKE_DLL),yes)
  DO.PLUGIN.PREAMBLE = $(DO.SHARED.PLUGIN.PREAMBLE)
  DO.PLUGIN.CORE = $(DO.SHARED.PLUGIN.CORE)
  DO.PLUGIN.POSTAMBLE = $(DO.SHARED.PLUGIN.POSTAMBLE)
else
  DO.PLUGIN.PREAMBLE = $(DO.STATIC.PLUGIN.PREAMBLE)
  DO.PLUGIN.CORE = $(DO.STATIC.PLUGIN.CORE)
  DO.PLUGIN.POSTAMBLE = $(DO.STATIC.PLUGIN.POSTAMBLE)
endif
DO.PLUGIN = $(DO.PLUGIN.PREAMBLE) $(DO.PLUGIN.CORE) $(DO.PLUGIN.POSTAMBLE)

# The sed script used to build dependencies
SED_DEPEND=-e "s:^\([^ \#].*\):$(BUCK)\(OUT\)/\1:" $(SYS_SED_DEPEND)
# How to build a source dependency file
ifndef DO.DEP
  ifeq ($(DEPEND_TOOL),cc)
    DO.DEP1 = $(CC) -MM $(CFLAGS) $(CFLAGS.INCLUDE)
    DO.DEP2 = $(filter-out %.asm,$^) | sed $(SED_DEPEND) >$@
    DO.DEP = $(DO.DEP1) $(DO.DEP2)
  else
    ifeq ($(DEPEND_TOOL),mkdep)
      # If mkdep is already installed, don't build it
      ifneq ($(MAKEDEP.AVAILABLE),yes)
dep: build.makedep
        MAKEDEP := ./makedep
      else
        ifneq (,$(CMD.MAKEDEP))
          MAKEDEP := $(CMD.MAKEDEP)
        else
          MAKEDEP := makedep
        endif
      endif
      DO.DEP1 = $(MAKEDEP) $(subst $(CFLAGS.I),-I,$(CFLAGS.INCLUDE) )
      DO.DEP2 = \
        $(filter-out %.asm,$^) -o $(BUCK)O -p $(BUCK)\(OUT\)/ -r -c -f $@
      DO.DEP = $(DO.DEP1) $(DO.DEP2)
    else
      DO.DEP = echo Building dependencies is not supported on this platform
    endif
  endif
endif

# The following include should make additional defines using above variables
MAKESECTION = postdefines
include mk/subs.mak

# Now remove duplicate include dirs and duplicate libraries
CFLAGS.INCLUDE := $(sort $(CFLAGS.INCLUDE))
LIBS.EXE := $(sort $(LIBS.EXE))

# If some libraries should always come last in a specific order, define the
# LIBS.SORT variable to equal these libraries in the order they are neede.
# For example, if LIBS.SORT = -ldl -lm -lsomething these libraries will be
# made the last ones (in the exact sequence) but only if they are already
# present in LIBS.EXE
ifdef LIBS.SORT
  LIBS.EXE := $(filter-out $(LIBS.SORT),$(LIBS.EXE)) \
    $(foreach LIB,$(LIBS.SORT),$(filter $(LIB),$(LIBS.EXE)))
endif

all: $(OUTDIRS)

dep: $(OUTBASE) $(OUTOS)

maintainerclean: distclean
	-$(RMDIR) configure autom4te.cache

distclean: clean
	-$(RM) Makefile config.mak config.cache config.status \
	include/volatile.h

clean:
	-$(RM) config.log
	-$(RMDIR) $(OUTBASE)
ifneq (.,$(OUTDLL))
	-$(RMDIR) $(OUTDLL)
endif

cleanlib:

cleandep: $(OUTBASE) $(OUTOS)
	-$(RM) $(OUTOS)/*.dep

$(OUT)/%$O: %.cpp
	$(DO.COMPILE.CPP)

$(OUT)/%$O: %.c
	$(DO.COMPILE.C)

$(OUT)/%$O: %.S
	$(DO.COMPILE.S)

$(OUT)/%$O: %.s
	$(DO.COMPILE.S)

$(OUT)/%$O: %.asm
	$(DO.COMPILE.ASM)

$(OUTDIRS):
	$(MKDIR)

# A makefile debugging facility.  Prints out the expansion of the named
# variable.  For instance, to print the expansion of SRC.SOFT3D, invoke
# "make show V=SRC.SOFT3D" from the command-line.
show:
	@echo $"$V=$($V)$"

# The following include should define system-dependent targets
MAKESECTION=targets
include mk/subs.mak
