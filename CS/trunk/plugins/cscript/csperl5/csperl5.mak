DESCRIPTION.csperl5 = Crystal Space Perl5 scripting plugin
DESCRIPTION.csperl5dist = $(DESCRIPTION.csperl5) interim files
DESCRIPTION.swigperl5gen = $(DESCRIPTION.csperl5) (forcibly)

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION), rootdefines)

PLUGINHELP += \
  $(NEWLINE)echo $"  make csperl5      Make the $(DESCRIPTION.csperl5)$"

endif

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION), roottargets)

.PHONY: csperl5 swigperl5gen csperl5clean csperl5distclean

all plugins: csperl5

csperl5:
	$(MAKE_TARGET) MAKE_DLL=yes
swigperl5gen:
	$(MAKE_TARGET)
csperl5clean:
	$(MAKE_CLEAN)
csperl5distclean:
	$(MAKE_CLEAN)

endif

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION), postdefines)

ifeq ($(USE_PLUGINS),yes)
  CSPERL5 = $(OUTDLL)/csperl5$(DLL)
  LIB.CSPERL5 = $(foreach d,$(DEP.CSPERL5),$($d.LIB))
  TO_INSTALL.DYNAMIC_LIBS += $(CSPERL5)
else
  CSPERL5 = $(OUT)/$(LIBPREFIX)csperl5$(LIB)
  DEP.EXE += $(CSPERL5)
  SCF.STATIC += csperl5
  TO_INSTALL.STATIC_LIBS += $(CSPERL5)
endif

SCF.CSPERL5 = $(SRCDIR)/plugins/cscript/csperl5/csperl5.scf
INC.CSPERL5 = $(wildcard $(addprefix $(SRCDIR)/,plugins/cscript/csperl5/*.h))
SRC.CSPERL5 = $(wildcard $(addprefix $(SRCDIR)/,plugins/cscript/csperl5/*.cpp))
OBJ.CSPERL5 = $(addprefix $(OUT)/,$(notdir $(SRC.CSPERL5:.cpp=$O)))
DEP.CSPERL5 = CSTOOL CSGEOM CSSYS CSUTIL CSSYS CSUTIL

PERLXSI.C = $(OUTDERIVED)/csperlxs.c
PERLXSI.O = $(OUT)/$(notdir $(PERLXSI.C:.c=$O))

SWIG.I = $(SRCDIR)/include/ivaria/cspace.i
SWIG.MOD = cspace
SWIG.PERL5.PM = $(SRCDIR)/scripts/perl5/$(SWIG.MOD).pm
SWIG.PERL5.C = $(SRCDIR)/plugins/cscript/csperl5/cswigpl5.inc
SWIG.PERL5.CPP = $(SRCDIR)/plugins/cscript/csperl5/cswigpl5.cpp
SWIG.PERL5.O = $(OUT)/$(notdir $(SWIG.PERL5.CPP:.cpp=$O))
SWIG.PERL5.DLL = $(SRCDIR)/scripts/perl5/$(SWIG.MOD)$(DLL)
SWIG.PERL5.DOC = $(SRCDIR)/scripts/perl5/cs_wrap.doc

CEX.CSPERL5 = perl5.cex
CIN.CSPERL5 = plugins/cscript/csperl5/perl5.cin

MSVC.DSP += MSCSPERL5
DSP.MSCSPERL5.NAME = csperl5
DSP.MSCSPERL5.TYPE = plugin
DSP.MSCSPERL5.CFLAGS = \
  /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "NO_STRICT" \
  /D "HAVE_DES_FCRYPT" /D "PERL_IMPLICIT_CONTEXT" \
  /D "PERL_IMPLICIT_SYS" /D "USE_PERLIO" \
  /D "PERL_MSVCRT_READFIX" \
MSCSPERL5 = $(CSPERL5)
LIB.MSCSPERL5 = $(LIB.CSPERL5) $(LIBPREFIX)perl58$(LIB)
INC.MSCSPERL5 = $(INC.CSPERL5)
SRC.MSCSPERL5 = $(SRC.CSPERL5) $(PERLXSI.C)
OBJ.MSCSPERL5 = $(OBJ.CSPERL5) $(PERLXSI.O)
DEP.MSCSPERL5 = $(DEP.CSPERL5)

MSVC.DSP += MSCSPERL5SWIG
DSP.MSCSPERL5SWIG.NAME = csperl5s
DSP.MSCSPERL5SWIG.TYPE = plugin
DSP.MSCSPERL5SWIG.CFLAGS = $(DSP.MSCSPERL5.CFLAGS) /D "PERL_POLLUTE"
MSCSPERL5SWIG = $(SWIG.PERL5.DLL)
LIB.MSCSPERL5SWIG = $(LIB.MSCSPERL5)
SRC.MSCSPERL5SWIG = $(SWIG.PERL5.C)
OBJ.MSCSPERL5SWIG = $(SWIG.PERL5.O)
DEP.MSCSPERL5SWIG = $(DEP.CSPERL5)

endif

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION), targets)

.PHONY: csperl5 swigperl5gen csperl5clean csperl5distclean

csperl5: $(OUTDIRS) $(CSPERL5) $(CSPERL5.PM) $(CEX.CSPERL5)

$(CSPERL5): $(OBJ.CSPERL5) $(LIB.CSPERL5) $(PERLXSI.O) $(SWIG.PERL5.DLL)
	$(DO.PLUGIN.PREAMBLE) \
	$(DO.PLUGIN.CORE) $(PERL5.LFLAGS) \
	$(DO.PLUGIN.POSTAMBLE)

$(OUT)/%$O: $(SRCDIR)/plugins/cscript/csperl5/%.cpp
	$(DO.COMPILE.CPP) $(PERL5.CFLAGS)

ifeq ($(PERL5.EXTUTILS.EMBED.AVAILABLE),yes)
  ifeq ($(PERL5.DYNALOADER.AVAILABLE),yes)
    PERLXSI.MK = $(CMD.PERL5) -MExtUtils::Embed -e xsinit -- \
    -o $(PERLXSI.C) -std DynaLoader cspace
  else
    PERLXSI.MK = $(CMD.PERL5) -MExtUtils::Embed -e xsinit -- \
    -o $(PERLXSI.C) -std cspace
  endif
else
  PERLXSI.MK = echo "\#include \"cssys/csperlxs_fallback.inc\"" > $(PERLXSI.C)
endif

$(PERLXSI.C):
	$(PERLXSI.MK)

$(PERLXSI.O): $(PERLXSI.C)
	$(DO.COMPILE.C) $(PERL5.CFLAGS)

ifeq (,$(SWIGBIN))
$(SWIG.PERL5.PM) $(SWIG.PERL5.C):
	@echo $"ERROR: Swig 1.3.14 or newer, not detected$" >&2
else
$(SWIG.PERL5.PM) $(SWIG.PERL5.C):
	-$(SWIGBIN) -perl5 -c++ -v -shadow -Iinclude \
	-module $(SWIG.MOD) -o $(SWIG.PERL5.C) $(SWIG.I)
	$(MV) $(SRCDIR)/plugins/cscript/csperl5/$(SWIG.MOD).pm $(SWIG.PERL5.PM)
endif

$(SWIG.PERL5.CPP): $(SWIG.PERL5.C)

$(SWIG.PERL5.O): $(SWIG.PERL5.CPP)
	$(filter-out -W -Wunused -Wall -Wmost,$(DO.COMPILE.CPP) \
	$(PERL5.CFLAGS) -DPERL_POLLUTE)

$(SWIG.PERL5.DLL): $(SWIG.PERL5.O) $(LIB.CSPERL5)
	$(DO.PLUGIN.PREAMBLE) \
	$(DO.PLUGIN.CORE) $(PERL5.LFLAGS) \
	$(DO.PLUGIN.POSTAMBLE)

$(CEX.CSPERL5): $(CIN.CSPERL5)
	@echo Generating perl5 cs-config extension...
	$(PERL5) $(CIN.CSPERL5) \
	$"CFLAGS=$(PERL5.CFLAGS)$" $"LFLAGS=$(PERL5.LFLAGS)$" > $@

swigperl5gen: swigperl5clean $(SWIG.PERL5.C) $(SWIG.PERL5.PM)

clean: csperl5clean
distclean: csperl5distclean

csperl5clean:
	-$(RMDIR) $(CSPERL5) $(OBJ.CSPERL5) $(OUTDLL)/$(notdir $(SCF.CSPERL5)) \
$(PERLXSI.O) $(PERLXSI.C) $(CSPERL5.PM) $(SWIG.PERL5.O) $(SWIG.PERL5.DLL)

swigperl5clean:
	$(RM) $(SWIG.PERL5.DOC) $(SWIG.PERL5.C) $(SWIG.PERL5.PM)

csperl5distclean: csperl5clean swigperl5clean

ifdef DO_DEPEND
dep: $(OUTOS)/csperl5.dep
$(OUTOS)/csperl5.dep: $(SRC.CSPERL5)
	$(DO.DEP1) $(PERL5.CFLAGS) $(DO.DEP2)
else
-include $(OUTOS)/csperl5.dep
endif

endif
