DESCRIPTION.starldr = Star mesh object loader

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

PLUGINHELP += \
  $(NEWLINE)echo $"  make starldr      Make the $(DESCRIPTION.starldr)$"

endif # ifeq ($(MAKESECTION),rootdefines)
#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: starldr starldrclean
plugins meshes all: starldr

starldrclean:
	$(MAKE_CLEAN)
starldr:
	$(MAKE_TARGET) MAKE_DLL=yes

endif # ifeq ($(MAKESECTION),roottargets)
#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp $(SRCDIR)/plugins/mesh/stars/persist/standard

ifeq ($(USE_PLUGINS),yes)
  STARLDR = $(OUTDLL)/starldr$(DLL)
  LIB.STARLDR = $(foreach d,$(DEP.STARLDR),$($d.LIB))
  TO_INSTALL.DYNAMIC_LIBS += $(STARLDR)
else
  STARLDR = $(OUT)/$(LIB_PREFIX)starldr$(LIB)
  DEP.EXE += $(STARLDR)
  SCF.STATIC += starldr
  TO_INSTALL.STATIC_LIBS += $(STARLDR)
endif

INF.STARLDR = $(SRCDIR)/plugins/mesh/stars/persist/standard/starldr.csplugin
INC.STARLDR = $(wildcard $(addprefix $(SRCDIR)/,plugins/mesh/stars/persist/standard/*.h))
SRC.STARLDR = $(wildcard $(addprefix $(SRCDIR)/,plugins/mesh/stars/persist/standard/*.cpp))
OBJ.STARLDR = $(addprefix $(OUT)/,$(notdir $(SRC.STARLDR:.cpp=$O)))
DEP.STARLDR = CSGEOM CSUTIL CSUTIL

MSVC.DSP += STARLDR
DSP.STARLDR.NAME = starldr
DSP.STARLDR.TYPE = plugin

endif # ifeq ($(MAKESECTION),postdefines)
#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: starldr starldrclean
starldr: $(OUTDIRS) $(STARLDR)

$(STARLDR): $(OBJ.STARLDR) $(LIB.STARLDR)
	$(DO.PLUGIN)

clean: starldrclean
starldrclean:
	-$(RMDIR) $(STARLDR) $(OBJ.STARLDR) $(OUTDLL)/$(notdir $(INF.STARLDR))

ifdef DO_DEPEND
dep: $(OUTOS)/starldr.dep
$(OUTOS)/starldr.dep: $(SRC.STARLDR)
	$(DO.DEP)
else
-include $(OUTOS)/starldr.dep
endif

endif # ifeq ($(MAKESECTION),targets)
