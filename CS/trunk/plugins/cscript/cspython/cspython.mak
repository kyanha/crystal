# Plug-in module only valid if module is listed in PLUGINS.
ifneq (,$(findstring cspython,$(PLUGINS)))

# Plugin description
DESCRIPTION.cspython = Crystal Script Python plug-in
DESCRIPTION.pythmod = Crystal Space Python module
DESCRIPTION.cspythonswig = Crystal Script Python SWIG interface

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

.PHONY: cspython pythmod cspythonclean cspythonswig
ifneq ($(MAKE_PYTHON_MODULE),no)
all plugins: cspython pythmod
else
all plugins: cspython
endif

cspython:
	$(MAKE_TARGET) MAKE_DLL=yes
ifneq ($(MAKE_PYTHON_MODULE),no)
pythmod:
	$(MAKE_TARGET) MAKE_DLL=yes
endif
cspythonclean:
	$(MAKE_CLEAN)
cspythonswig:
	$(MAKE_TARGET) MAKE_DLL=yes

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

PYTHMOD = scripts/python/_cspace$(DLL)
LIB.PYTHMOD = $(LIB.CSPYTHON)
LIB.PYTHMOD.LOCAL = $(LIB.CSPYTHON.LOCAL)

TO_INSTALL.EXE += python.cex

SWIG.INTERFACE = include/ivaria/cspace.i
SWIG.CSPYTHON = plugins/cscript/cspython/cs_pyth.cpp
SWIG.CSPYTHON.OBJ = $(addprefix $(OUT)/,$(notdir $(SWIG.CSPYTHON:.cpp=$O)))

TRASH.CSPYTHON = $(wildcard $(addprefix scripts/python/,*.pyc *.pyo))

INC.CSPYTHON = $(wildcard plugins/cscript/cspython/*.h)
SRC.CSPYTHON = \
  $(filter-out plugins/cscript/cspython/pythmod.cpp, $(sort $(wildcard plugins/cscript/cspython/*.cpp) $(SWIG.CSPYTHON)))
OBJ.CSPYTHON = $(addprefix $(OUT)/, $(notdir $(SRC.CSPYTHON:.cpp=$O)))
DEP.CSPYTHON = CSTOOL CSGFX CSGEOM CSSYS CSUTIL CSSYS CSUTIL

INC.PYTHMOD =
SRC.PYTHMOD = plugins/cscript/cspython/pythmod.cpp $(SWIG.CSPYTHON)
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

.PHONY: cspython pythmod cspythonclean cspythonswig csjavaswigclean

ifneq ($(MAKE_PYTHON_MODULE),no)
all: $(CSPYTHON.LIB) $(PYTHMOD.LIB)
else
all: $(CSPYTHON.LIB)
endif
cspython: $(OUTDIRS) $(CSPYTHON) python.cex
ifneq ($(MAKE_PYTHON_MODULE),no)
pythmod: $(OUTDIRS) $(PYTHMOD) python.cex
clean: cspythonclean pythmodclean
else
clean: cspythonclean
endif

$(SWIG.CSPYTHON.OBJ): $(SWIG.CSPYTHON)
	$(filter-out -W -Wunused -Wall -Wmost,$(DO.COMPILE.CPP) $(PYTHON.CFLAGS))

$(OUT)/%$O: plugins/cscript/cspython/%.cpp
	$(DO.COMPILE.CPP) $(PYTHON.CFLAGS)

$(OUT)/%$O: plugins/cscript/cspython/%.c
	$(DO.COMPILE.C) $(PYTHON.CFLAGS)

ifeq (,$(SWIGBIN))
  SWIGBIN = swig
endif

SWIGFLAGS=-python -c++ -shadow -Iinclude/
SWIG.CSPYTHON.DEPS=\
	include/ivaria/pythpre.i \
	include/ivaria/pythpost.i

$(SWIG.CSPYTHON): $(SWIG.INTERFACE) $(SWIG.CSPYTHON.DEPS)
	$(SWIGBIN) $(SWIGFLAGS) -o $(SWIG.CSPYTHON) $(SWIG.INTERFACE)
	$(CP) plugins/cscript/cspython/cspace.py scripts/python/

python.cex: plugins/cscript/cspython/python.cin
	@echo Generate python cs-config extension...
	@echo $"#!/bin/sh$" > python.cex
	@echo $"# WARNING: This file is generated automatically by cspython.mak$" >> python.cex
	@echo $"PYTH_LIBS="$(LIB.CSPYTHON.LOCAL)"$" >> python.cex
	@echo $"PYTH_CFLAGS="$(PYTHON.CFLAGS)"$"    >> python.cex
	@echo $"PYTH_CXXFLAGS="$(PYTHON.CFLAGS)"$"  >> python.cex
	@echo $"PYTH_DEPS=""$"		            >> python.cex
	@cat plugins/cscript/cspython/python.cin    >> python.cex

$(CSPYTHON): $(OBJ.CSPYTHON) $(LIB.CSPYTHON)
	$(DO.PLUGIN.PREAMBLE) \
	$(DO.PLUGIN.CORE) $(LIB.CSPYTHON.LOCAL) \
	$(DO.PLUGIN.POSTAMBLE)

$(PYTHMOD): $(OBJ.PYTHMOD) $(LIB.PYTHMOD)
	cd plugins/cscript/cspython ; $(PYTHON) pythmod_setup.py pythmod_install ../../../$(OUT)

pythmodclean:
	cd plugins/cscript/cspython ; $(PYTHON) pythmod_setup.py pythmod_clean
	-$(RM) -fr plugins/cscript/cspython/build/

cspythonclean:
	-$(RM) $(CSPYTHON) $(OBJ.CSPYTHON) $(TRASH.CSPYTHON) python.cex

cspythonswig: cspythonswigclean cspython

cspythonswigclean:
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
