DESCRIPTION.spirldr = Spiral mesh object loader

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

PLUGINHELP += \
  $(NEWLINE)echo $"  make spirldr      Make the $(DESCRIPTION.spirldr)$"

endif # ifeq ($(MAKESECTION),rootdefines)
#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: spirldr spirldrclean
plugins meshes all: spirldr

spirldrclean:
	$(MAKE_CLEAN)
spirldr:
	$(MAKE_TARGET) MAKE_DLL=yes

endif # ifeq ($(MAKESECTION),roottargets)
#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp $(SRCDIR)/plugins/mesh/spiral/persist/standard

ifeq ($(USE_PLUGINS),yes)
  SPIRLDR = $(OUTDLL)/spirldr$(DLL)
  LIB.SPIRLDR = $(foreach d,$(DEP.SPIRLDR),$($d.LIB))
  TO_INSTALL.DYNAMIC_LIBS += $(SPIRLDR)
else
  SPIRLDR = $(OUT)/$(LIB_PREFIX)spirldr$(LIB)
  DEP.EXE += $(SPIRLDR)
  SCF.STATIC += spirldr
  TO_INSTALL.STATIC_LIBS += $(SPIRLDR)
endif

INF.SPIRLDR = $(SRCDIR)/plugins/mesh/spiral/persist/standard/spirldr.csplugin
INC.SPIRLDR = $(wildcard $(addprefix $(SRCDIR)/,plugins/mesh/spiral/persist/standard/*.h))
SRC.SPIRLDR = $(wildcard $(addprefix $(SRCDIR)/,plugins/mesh/spiral/persist/standard/*.cpp))
OBJ.SPIRLDR = $(addprefix $(OUT)/,$(notdir $(SRC.SPIRLDR:.cpp=$O)))
DEP.SPIRLDR = CSGEOM CSUTIL CSSYS CSUTIL

MSVC.DSP += SPIRLDR
DSP.SPIRLDR.NAME = spirldr
DSP.SPIRLDR.TYPE = plugin

endif # ifeq ($(MAKESECTION),postdefines)
#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: spirldr spirldrclean
spirldr: $(OUTDIRS) $(SPIRLDR)

$(SPIRLDR): $(OBJ.SPIRLDR) $(LIB.SPIRLDR)
	$(DO.PLUGIN)

clean: spirldrclean
spirldrclean:
	-$(RMDIR) $(SPIRLDR) $(OBJ.SPIRLDR) $(OUTDLL)/$(notdir $(INF.SPIRLDR))

ifdef DO_DEPEND
dep: $(OUTOS)/spirldr.dep
$(OUTOS)/spirldr.dep: $(SRC.SPIRLDR)
	$(DO.DEP)
else
-include $(OUTOS)/spirldr.dep
endif

endif # ifeq ($(MAKESECTION),targets)
