# Plug-in module only valid if module is listed in PLUGINS.
ifneq (,$(findstring cspython,$(PLUGINS)))

# Plugin description
DESCRIPTION.cspython = Crystal Script Python plug-in
DESCRIPTION.pythmod = Crystal Space Python module
DESCRIPTION.swigpythgen = SWIG Python files (forcibly)
DESCRIPTION.swigpythinst = Install SWIG-generated Python files

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Plugin-specific help commands
PLUGINHELP += \
  $(NEWLINE)echo $"  make cspython     Make the $(DESCRIPTION.cspython)$"
ifneq ($(MAKE_PYTHON_MODULE),no)
PLUGINHELP += \
  $(NEWLINE)echo $"  make pythmod      Make the $(DESCRIPTION.pythmod)$"
endif

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: cspython pythmod cspythonclean swigpythinst swigpythgen
ifneq ($(MAKE_PYTHON_MODULE),no)
all: cspython pythmod
plugins: cspython
else
all plugins: cspython
endif

cspython:
	$(MAKE_TARGET) MAKE_DLL=yes
cspythonclean:
	$(MAKE_CLEAN)
ifneq ($(MAKE_PYTHON_MODULE),no)
pythmod:
	$(MAKE_TARGET)
pythmodclean:
	$(MAKE_CLEAN)
endif
ifneq (,$(SWIGBIN))
swigpythgen:
	$(MAKE_TARGET)
swigpythinst:
	$(MAKE_TARGET) DO_SWIGPYTHINST=yes
endif

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

PYTHON.CFLAGS += -DSWIG_GLOBAL

ifeq ($(USE_PLUGINS),yes)
  CSPYTHON = $(OUTDLL)/cspython$(DLL)
  LIB.CSPYTHON = $(foreach d,$(DEP.CSPYTHON),$($d.LIB))
  LIB.CSPYTHON.LOCAL = $(PYTHON.LFLAGS)
  TO_INSTALL.DYNAMIC_LIBS += $(CSPYTHON)
else
  CSPYTHON = $(OUT)/$(LIB_PREFIX)cspython$(LIB)
  DEP.EXE += $(CSPYTHON)
  LIBS.EXE += $(PYTHON.LFLAGS)
  SCF.STATIC += cspython
  TO_INSTALL.STATIC_LIBS += $(CSPYTHON)
endif

PYTHMOD.BUILDBASE=$(OUT)/python
PYTHMOD.INSTALLDIR=$(OUTPROC)/python
PYTHMOD = $(PYTHMOD.INSTALLDIR)/_cspace$(DLL)
LIB.PYTHMOD = $(LIB.CSPYTHON)
LIB.PYTHMOD.LOCAL = $(LIB.CSPYTHON.LOCAL)

TO_INSTALL.EXE += python.cex

SWIG.OUTDIR = $(OUTDERIVED)
SWIG.INTERFACE = $(SRCDIR)/include/ivaria/cspace.i
SWIG.CSPYTHON = $(SWIG.OUTDIR)/cs_pyth.cpp
SWIG.CSPYTHON.CVS = $(SRCDIR)/plugins/cscript/cspython/cs_pyth.cpp
SWIG.CSPYTHON.OBJ = $(addprefix $(OUT)/,$(notdir $(SWIG.CSPYTHON:.cpp=$O)))

TRASH.CSPYTHON = $(wildcard $(addprefix $(SRCDIR)/scripts/python/,*.pyc *.pyo))

INC.CSPYTHON = $(wildcard $(addprefix $(SRCDIR)/,plugins/cscript/cspython/*.h))
SRC.CSPYTHON = $(filter-out plugins/cscript/cspython/pythmod.cpp, \
  $(sort $(wildcard plugins/cscript/cspython/*.cpp) $(SWIG.CSPYTHON)))
OBJ.CSPYTHON = $(addprefix $(OUT)/, $(notdir $(SRC.CSPYTHON:.cpp=$O)))
DEP.CSPYTHON = CSTOOL CSGFX CSGEOM CSSYS CSUTIL CSSYS CSUTIL

INC.PYTHMOD =
SRC.PYTHMOD = $(SRCDIR)/plugins/cscript/cspython/pythmod.cpp $(SWIG.CSPYTHON)
OBJ.PYTHMOD = $(addprefix $(OUT)/, $(notdir $(SRC.PYTHMOD:.cpp=$O)))
DEP.PYTHMOD = $(DEP.CSPYTHON)

MSVC.DSP += CSPYTHON
DSP.CSPYTHON.NAME = cspython
DSP.CSPYTHON.TYPE = plugin
DSP.CSPYTHON.RESOURCES = $(SWIG.INTERFACE)
DSP.CSPYTHON.CFLAGS = /D "SWIG_GLOBAL"

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: cspython pythmod cspythonclean swigpythinst swigpythgen

ifneq ($(MAKE_PYTHON_MODULE),no)
all: $(CSPYTHON.LIB) $(PYTHMOD.LIB)
else
all: $(CSPYTHON.LIB)
endif
cspython: $(OUTDIRS) $(CSPYTHON) python.cex
ifneq ($(MAKE_PYTHON_MODULE),no)
pythmod: $(OUTDIRS) $(PYTHMOD)
endif
clean: cspythonclean pythmodclean

$(SWIG.CSPYTHON.OBJ): $(SWIG.CSPYTHON)
	$(filter-out -W -Wunused -Wall -Wmost,$(DO.COMPILE.CPP) $(PYTHON.CFLAGS))

$(OUT)/%$O: $(SRCDIR)/plugins/cscript/cspython/%.cpp
	$(DO.COMPILE.CPP) $(PYTHON.CFLAGS)

$(OUT)/%$O: $(SRCDIR)/plugins/cscript/cspython/%.c
	$(DO.COMPILE.C) $(PYTHON.CFLAGS)

SWIGFLAGS=-python -c++ -shadow -Iinclude/
SWIG.CSPYTHON.DEPS=\
	$(SRCDIR)/include/ivaria/pythpre.i \
	$(SRCDIR)/include/ivaria/pythpost.i

ifeq ($(DO_SWIGPYTHINST),yes)
$(SWIG.CSPYTHON): $(SWIG.INTERFACE) $(SWIG.CSPYTHON.DEPS)
	$(SWIGBIN) $(SWIGFLAGS) -o $(SWIG.CSPYTHON) $(SWIG.INTERFACE)
else
ifneq (,$(SWIGBIN))
$(SWIG.CSPYTHON): $(SWIG.INTERFACE) $(SWIG.CSPYTHON.DEPS)
	$(SWIGBIN) $(SWIGFLAGS) -o $(SWIG.CSPYTHON) $(SWIG.INTERFACE)
else
$(SWIG.CSPYTHON): $(SWIG.CSPYTHON.CVS)
	-$(RM) $(SWIG.CSPYTHON)
	$(CP) $(SWIG.CSPYTHON.CVS) $(SWIG.CSPYTHON)
endif
endif

python.cex: $(SRCDIR)/plugins/cscript/cspython/python.cin
	@echo Generate python cs-config extension...
	@echo $"#!/bin/sh$" > python.cex
	@echo $"# WARNING: This file is generated automatically by cspython.mak$" >> python.cex
	@echo $"PYTH_LIBS="$(LIB.CSPYTHON.LOCAL)"$" >> python.cex
	@echo $"PYTH_CFLAGS="$(PYTHON.CFLAGS)"$"    >> python.cex
	@echo $"PYTH_CXXFLAGS="$(PYTHON.CFLAGS)"$"  >> python.cex
	@echo $"PYTH_DEPS=""$"		            >> python.cex
	@cat $(SRCDIR)/plugins/cscript/cspython/python.cin >> python.cex

$(CSPYTHON): $(OBJ.CSPYTHON) $(LIB.CSPYTHON)
	$(DO.PLUGIN.PREAMBLE) \
	$(DO.PLUGIN.CORE) $(LIB.CSPYTHON.LOCAL) \
	$(DO.PLUGIN.POSTAMBLE)

$(PYTHMOD.INSTALLDIR):
	-$(MKDIRS)

ifeq ($(PYTHON.DISTUTILS),yes)
$(PYTHMOD): $(PYTHMOD.INSTALLDIR) $(SRC.PYTHMOD) $(LIB.PYTHMOD)
	$(PYTHON) $(SRCDIR)/plugins/cscript/cspython/pythmod_setup.py $(SWIG.OUTDIR) $(SRCDIR) $(SRCDIR)/include ./include $(OUT) build -q --build-base=$(PYTHMOD.BUILDBASE) install -q --install-lib=$(PYTHMOD.INSTALLDIR)
else
$(PYTHMOD):
	@echo $(DESCRIPTION.pythmod)" not supported: distutils not available!"
endif

pythmodclean:
	-$(RMDIR) $(SWIG.CSPYTHON) $(PYTHMOD) $(PYTHMOD.BUILDBASE)


ifeq ($(DO_SWIGHPYTHINST),yes)
swigpythinst: $(SWIG.CSPYTHON.CVS) $(SRCDIR)/scripts/python/cspace.py

$(SWIG.CSPYTHON.CVS): $(SWIG.OUTDIR)/cs_pyth.cpp
	-$(RM) $@
	$(CP) $(SWIG.OUTDIR)/cs_pyth.cpp $@

$(SRCDIR)/scripts/python/cspace.py: $(SWIG.OUTDIR)/cspace.py
	-$(RM) $@
	$(CP) $(SWIG.OUTDIR)/cspace.py $@
endif

cspythonclean:
	-$(RM) $(CSPYTHON) $(SWIG.CSPYTHON) $(OBJ.CSPYTHON) $(TRASH.CSPYTHON) python.cex

swigpythgen: swigpythclean cspython

swigpythclean:
	-$(RM) $(CSPYTHON) $(PYTHMOD) $(SWIG.CSPYTHON)

ifdef DO_DEPEND
dep: $(OUTOS)/cspython.dep
$(OUTOS)/cspython.dep: $(SRC.CSPYTHON)
	$(DO.DEP1) $(PYTHON.CFLAGS) $(DO.DEP2)
ifneq ($(MAKE_PYTHON_MODULE),no)
dep: $(OUTOS)/pythmod.dep
$(OUTOS)/pythmod.dep: $(SRC.PYTHMOD)
	$(DO.DEP1) $(PYTHON.CFLAGS) $(DO.DEP2)
endif
else # ifdef DO_DEPEND
-include $(OUTOS)/cspython.dep
ifneq ($(MAKE_PYTHON_MODULE),no)
-include $(OUTOS)/pythmod.dep
endif
endif # ifdef/else DO_DEPEND

endif # ifeq ($(MAKESECTION),targets)
endif # ifneq (,$(findstring cspython,$(PLUGINS)))
