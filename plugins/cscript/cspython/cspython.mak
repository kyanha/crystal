# Plug-in module only valid if module is listed in PLUGINS.
ifneq (,$(findstring cspython,$(PLUGINS)))

# Plugin description
DESCRIPTION.cspython = Crystal Script Python plug-in
DESCRIPTION.cspythonswig = Crystal Script Python SWIG interface

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Plugin-specific help commands
PLUGINHELP += \
  $(NEWLINE)echo $"  make cspython     Make the $(DESCRIPTION.cspython)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: cspython cspythonclean cspythonswig
all plugins: cspython

cspython:
	$(MAKE_TARGET) MAKE_DLL=yes
cspythonclean:
	$(MAKE_CLEAN)
cspythonswig: 
	$(MAKE_TARGET) MAKE_DLL=yes	

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

#TCLTK=-ltk8.0 -ltcl8.0 -L/usr/X11R6/lib -lX11
PTHREAD=-lpthread

CFLAGS.PYTHON += $(CFLAGS.I)$(PYTHON_INC)
LIB.CSPYTHON.SYSTEM += $(LFLAGS.l)$(notdir $(PYTHON_LIB)) \
  $(LFLAGS.L)$(PYTHON_LIB)/config $(TCLTK) $(PTHREAD)

ifeq ($(USE_PLUGINS),yes)
  CSPYTHON = $(OUTDLL)cspython$(DLL)
  LIB.CSPYTHON = $(foreach d,$(DEP.CSPYTHON),$($d.LIB))
  LIB.CSPYTHON.LOCAL = $(LIB.CSPYTHON.SYSTEM)
# TO_INSTALL.DYNAMIC_LIBS += $(CSPYTHON)
else
  CSPYTHON = $(OUT)$(LIB_PREFIX)cspy$(LIB)
  DEP.EXE += $(CSPYTHON)
  LIBS.EXE += $(LIB.CSPYTHON.SYSTEM)
  CFLAGS.STATIC_SCF += $(CFLAGS.D)SCL_PYTHON
# TO_INSTALL.STATIC_LIBS += $(CSPYTHON)
endif

SWIG.CSPYTHON = plugins/cscript/cspython/cs_pyth.cpp
SWIG.INTERFACE = plugins/cscript/common/cs.i

INC.CSPYTHON = $(wildcard plugins/cscript/cspython/*.h)
SRC.CSPYTHON = $(wildcard plugins/cscript/cspython/*.cpp) $(SWIG.CSPYTHON)
OBJ.CSPYTHON = $(addprefix $(OUT),$(notdir $(SRC.CSPYTHON:.cpp=$O)))
DEP.CSPYTHON = CSGEOM CSSYS CSUTIL CSSYS

#MSVC.DSP += CSPYTHON
#DSP.CSPYTHON.NAME = cspython
#DSP.CSPYTHON.TYPE = plugin
#DSP.CSPYTHON.RESOURCES = $(SWIG.INTERFACE)

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: cspython cspythonclean cspythonswig

all: $(CSPYTHON.LIB)
cspython: $(OUTDIRS) $(CSPYTHON)
clean: cspythonclean

$(OUT)%$O: plugins/cscript/cspython/%.cpp
	$(DO.COMPILE.CPP) $(CFLAGS.PYTHON)

$(OUT)%$O: plugins/cscript/cspython/%.c
	$(DO.COMPILE.C) $(CFLAGS.PYTHON)

$(SWIG.CSPYTHON): $(SWIG.INTERFACE)
	swig -python -c++ -shadow -o $(SWIG.CSPYTHON) $(SWIG.INTERFACE)
	mv plugins/cscript/cspython/cspace.py scripts/python/

$(CSPYTHON): $(OBJ.CSPYTHON) $(LIB.CSPYTHON)
	$(DO.PLUGIN) $(LIB.CSPYTHON.LOCAL)

cspythonclean:
	-$(RM) $(CSPYTHON) $(OBJ.CSPYTHON) $(OUTOS)cspython.dep

cspythonswig: cspythonswigclean cspython

cspythonswigclean:
	-$(RM) $(CSPYTHON) $(SWIG.CSPYTHON) $(OUT)cs_pyth.cpp \
	$(OUTOS)cspython.dep

ifdef DO_DEPEND
dep: $(OUTOS)cspython.dep
$(OUTOS)cspython.dep: $(SRC.CSPYTHON)
	$(DO.DEP1) $(CFLAGS.PYTHON) $(DO.DEP2)
else
-include $(OUTOS)cspython.dep
endif

endif # ifeq ($(MAKESECTION),targets)
endif # ifneq (,$(findstring cspython,$(PLUGINS)))
