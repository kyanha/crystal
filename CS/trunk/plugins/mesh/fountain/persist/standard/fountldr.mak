DESCRIPTION.fountldr = Fountain mesh object loader

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

PLUGINHELP += \
  $(NEWLINE)echo $"  make fountldr     Make the $(DESCRIPTION.fountldr)$"

endif # ifeq ($(MAKESECTION),rootdefines)
#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: fountldr fountldrclean
plugins meshes all: fountldr

fountldrclean:
	$(MAKE_CLEAN)
fountldr:
	$(MAKE_TARGET) MAKE_DLL=yes

endif # ifeq ($(MAKESECTION),roottargets)
#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp $(SRCDIR)/plugins/mesh/fountain/persist/standard

ifeq ($(USE_PLUGINS),yes)
  FOUNTLDR = $(OUTDLL)/fountldr$(DLL)
  LIB.FOUNTLDR = $(foreach d,$(DEP.FOUNTLDR),$($d.LIB))
  TO_INSTALL.DYNAMIC_LIBS += $(FOUNTLDR)
else
  FOUNTLDR = $(OUT)/$(LIB_PREFIX)fountldr$(LIB)
  DEP.EXE += $(FOUNTLDR)
  SCF.STATIC += fountldr
  TO_INSTALL.STATIC_LIBS += $(FOUNTLDR)
endif

INF.FOUNTLDR = $(SRCDIR)/plugins/mesh/fountain/persist/standard/fountldr.csplugin
INC.FOUNTLDR = $(wildcard $(addprefix $(SRCDIR)/,plugins/mesh/fountain/persist/standard/*.h))
SRC.FOUNTLDR = $(wildcard $(addprefix $(SRCDIR)/,plugins/mesh/fountain/persist/standard/*.cpp))
OBJ.FOUNTLDR = $(addprefix $(OUT)/,$(notdir $(SRC.FOUNTLDR:.cpp=$O)))
DEP.FOUNTLDR = CSGEOM CSUTIL CSSYS CSUTIL

MSVC.DSP += FOUNTLDR
DSP.FOUNTLDR.NAME = fountldr
DSP.FOUNTLDR.TYPE = plugin

endif # ifeq ($(MAKESECTION),postdefines)
#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: fountldr fountldrclean
fountldr: $(OUTDIRS) $(FOUNTLDR)

$(FOUNTLDR): $(OBJ.FOUNTLDR) $(LIB.FOUNTLDR)
	$(DO.PLUGIN)

clean: fountldrclean
fountldrclean:
	-$(RMDIR) $(FOUNTLDR) $(OBJ.FOUNTLDR) $(OUTDLL)/$(notdir $(INF.FOUNTLDR))

ifdef DO_DEPEND
dep: $(OUTOS)/fountldr.dep
$(OUTOS)/fountldr.dep: $(SRC.FOUNTLDR)
	$(DO.DEP)
else
-include $(OUTOS)/fountldr.dep
endif

endif # ifeq ($(MAKESECTION),targets)
