DESCRIPTION.fountain = Fountain mesh object plug-in

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

PLUGINHELP += \
  $(NEWLINE)echo $"  make fountain     Make the $(DESCRIPTION.fountain)$"

endif # ifeq ($(MAKESECTION),rootdefines)
#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: fountain fountainclean
plugins meshes all: fountain

fountainclean:
	$(MAKE_CLEAN)
fountain:
	$(MAKE_TARGET) MAKE_DLL=yes

endif # ifeq ($(MAKESECTION),roottargets)
#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp $(SRCDIR)/plugins/mesh/fountain/object $(SRCDIR)/plugins/mesh/partgen

ifeq ($(USE_PLUGINS),yes)
  FOUNTAIN = $(OUTDLL)/fountain$(DLL)
  LIB.FOUNTAIN = $(foreach d,$(DEP.FOUNTAIN),$($d.LIB))
  TO_INSTALL.DYNAMIC_LIBS += $(FOUNTAIN)
else
  FOUNTAIN = $(OUT)/$(LIB_PREFIX)fountain$(LIB)
  DEP.EXE += $(FOUNTAIN)
  SCF.STATIC += fountain
  TO_INSTALL.STATIC_LIBS += $(FOUNTAIN)
endif

INF.FOUNTAIN = $(SRCDIR)/plugins/mesh/fountain/object/fountain.csplugin
INC.FOUNTAIN = $(wildcard $(addprefix $(SRCDIR)/,plugins/mesh/fountain/object/*.h plugins/mesh/partgen/*.h))
SRC.FOUNTAIN = $(wildcard $(addprefix $(SRCDIR)/,plugins/mesh/fountain/object/*.cpp plugins/mesh/partgen/*.cpp))
OBJ.FOUNTAIN = $(addprefix $(OUT)/,$(notdir $(SRC.FOUNTAIN:.cpp=$O)))
DEP.FOUNTAIN = CSTOOL CSGEOM CSUTIL CSUTIL CSGFX

MSVC.DSP += FOUNTAIN
DSP.FOUNTAIN.NAME = fountain
DSP.FOUNTAIN.TYPE = plugin

endif # ifeq ($(MAKESECTION),postdefines)
#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: fountain fountainclean
fountain: $(OUTDIRS) $(FOUNTAIN)

$(FOUNTAIN): $(OBJ.FOUNTAIN) $(LIB.FOUNTAIN)
	$(DO.PLUGIN)

clean: fountainclean
fountainclean:
	-$(RMDIR) $(FOUNTAIN) $(OBJ.FOUNTAIN) $(OUTDLL)/$(notdir $(INF.FOUNTAIN))

ifdef DO_DEPEND
dep: $(OUTOS)/fount.dep
$(OUTOS)/fount.dep: $(SRC.FOUNTAIN)
	$(DO.DEP)
else
-include $(OUTOS)/fount.dep
endif

endif # ifeq ($(MAKESECTION),targets)
