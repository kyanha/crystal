DESCRIPTION.ie3ds = Model Import/Export 3ds plug-in

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

PLUGINHELP += \
  $(NEWLINE)echo $"  make ie3ds        Make the $(DESCRIPTION.ie3ds)$"

endif # ifeq ($(MAKESECTION),rootdefines)
#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: ie3ds ie3dsclean
plugins meshes all: ie3ds

ie3dsclean:
	$(MAKE_CLEAN)
ie3ds:
	$(MAKE_TARGET) MAKE_DLL=yes

endif # ifeq ($(MAKESECTION),roottargets)
#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

ifeq ($(USE_PLUGINS),yes)
  IE3DS = $(OUTDLL)/ie3ds$(DLL)
  LIB.IE3DS = $(foreach d,$(DEP.IE3DS),$($d.LIB))
  TO_INSTALL.DYNAMIC_LIBS += $(IE3DS)
else
  IE3DS = $(OUT)/$(LIB_PREFIX)ie3ds$(LIB)
  DEP.EXE += $(IE3DS)
  SCF.STATIC += ie3ds
  TO_INSTALL.STATIC_LIBS += $(IE3DS)
endif

INF.IE3DS = $(SRCDIR)/plugins/mesh/impexp/3ds/ie3ds.csplugin
INC.IE3DS = $(wildcard $(addprefix $(SRCDIR)/,plugins/mesh/impexp/3ds/*.h))
SRC.IE3DS = $(wildcard $(addprefix $(SRCDIR)/,plugins/mesh/impexp/3ds/*.cpp))
OBJ.IE3DS = $(addprefix $(OUT)/,$(notdir $(SRC.IE3DS:.cpp=$O)))
DEP.IE3DS = CSUTIL CSTOOL CSUTIL CSGEOM

MSVC.DSP += IE3DS
DSP.IE3DS.NAME = ie3ds
DSP.IE3DS.TYPE = plugin
DSP.IE3DS.LIBS = lib3ds-120

endif # ifeq ($(MAKESECTION),postdefines)
#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: ie3ds ie3dsclean
ie3ds: $(OUTDIRS) $(IE3DS)

$(IE3DS): $(OBJ.IE3DS) $(LIB.IE3DS)
	$(DO.PLUGIN.PREAMBLE) \
	$(DO.PLUGIN.CORE) $(3DS.LFLAGS) \
	$(DO.PLUGIN.POSTAMBLE)

$(OUT)/%$O: $(SRCDIR)/plugins/mesh/impexp/3ds/%.cpp
	$(DO.COMPILE.CPP) $(3DS.CFLAGS)

clean: ie3dsclean
ie3dsclean:
	-$(RMDIR) $(IE3DS) $(OBJ.IE3DS) $(OUTDLL)/$(notdir $(INF.IE3DS))

ifdef DO_DEPEND
dep: $(OUTOS)/ie3ds.dep
$(OUTOS)/ie3ds.dep: $(SRC.IE3DS)
	$(DO.DEP1) \
	$(3DS.CFLAGS) \
	$(DO.DEP2)
else
-include $(OUTOS)/ie3ds.dep
endif

endif # ifeq ($(MAKESECTION),targets)
