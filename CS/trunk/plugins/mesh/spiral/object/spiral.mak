DESCRIPTION.spiral = Spiral mesh object plug-in

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

PLUGINHELP += \
  $(NEWLINE)echo $"  make spiral       Make the $(DESCRIPTION.spiral)$"

endif # ifeq ($(MAKESECTION),rootdefines)
#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: spiral spiralclean
plugins meshes all: spiral

spiralclean:
	$(MAKE_CLEAN)
spiral:
	$(MAKE_TARGET) MAKE_DLL=yes

endif # ifeq ($(MAKESECTION),roottargets)
#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp $(SRCDIR)/plugins/mesh/spiral/object $(SRCDIR)/plugins/mesh/partgen

ifeq ($(USE_PLUGINS),yes)
  SPIRAL = $(OUTDLL)/spiral$(DLL)
  LIB.SPIRAL = $(foreach d,$(DEP.SPIRAL),$($d.LIB))
  TO_INSTALL.DYNAMIC_LIBS += $(SPIRAL)
else
  SPIRAL = $(OUT)/$(LIB_PREFIX)spiral$(LIB)
  DEP.EXE += $(SPIRAL)
  SCF.STATIC += spiral
  TO_INSTALL.STATIC_LIBS += $(SPIRAL)
endif

INF.SPIRAL = $(SRCDIR)/plugins/mesh/spiral/object/spiral.csplugin
INC.SPIRAL = $(wildcard $(addprefix $(SRCDIR)/,plugins/mesh/spiral/object/*.h plugins/mesh/partgen/*.h))
SRC.SPIRAL = $(wildcard $(addprefix $(SRCDIR)/,plugins/mesh/spiral/object/*.cpp plugins/mesh/partgen/*.cpp))
OBJ.SPIRAL = $(addprefix $(OUT)/,$(notdir $(SRC.SPIRAL:.cpp=$O)))
DEP.SPIRAL = CSTOOL CSGEOM CSUTIL CSSYS CSUTIL

MSVC.DSP += SPIRAL
DSP.SPIRAL.NAME = spiral
DSP.SPIRAL.TYPE = plugin

endif # ifeq ($(MAKESECTION),postdefines)
#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: spiral spiralclean
spiral: $(OUTDIRS) $(SPIRAL)

$(SPIRAL): $(OBJ.SPIRAL) $(LIB.SPIRAL)
	$(DO.PLUGIN)

clean: spiralclean
spiralclean:
	-$(RMDIR) $(SPIRAL) $(OBJ.SPIRAL) $(OUTDLL)/$(notdir $(INF.SPIRAL))

ifdef DO_DEPEND
dep: $(OUTOS)/spiral.dep
$(OUTOS)/spiral.dep: $(SRC.SPIRAL)
	$(DO.DEP)
else
-include $(OUTOS)/spiral.dep
endif

endif # ifeq ($(MAKESECTION),targets)
