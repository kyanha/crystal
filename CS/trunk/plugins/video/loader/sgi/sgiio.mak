# Plug-in description
DESCRIPTION.cssgiimg = Crystal Space sgi image loader

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Plug-in-specific help commands
PLUGINHELP += \
  $(NEWLINE)echo $"  make cssgiimg     Make the $(DESCRIPTION.cssgiimg)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: cssgiimg cssgiimgclean
all plugins imgplexall: cssgiimg

cssgiimg:
	$(MAKE_TARGET) MAKE_DLL=yes
cssgiimgclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp $(SRCDIR)/plugins/video/loader/sgi

ifeq ($(USE_PLUGINS),yes)
  CSSGIIMG = $(OUTDLL)/cssgiimg$(DLL)
  LIB.CSSGIIMG = $(foreach d,$(DEP.CSSGIIMG),$($d.LIB))
  TO_INSTALL.DYNAMIC_LIBS += $(CSSGIIMG)
else
  CSSGIIMG = $(OUT)/$(LIB_PREFIX)cssgiimg$(LIB)
  DEP.EXE += $(CSSGIIMG)
  SCF.STATIC += cssgiimg
  TO_INSTALL.STATIC_LIBS += $(CSSGIIMG)
endif

INF.CSSGIIMG = $(SRCDIR)/plugins/video/loader/sgi/cssgiimg.csplugin
INC.CSSGIIMG = $(wildcard $(addprefix $(SRCDIR)/,plugins/video/loader/sgi/*.h))
SRC.CSSGIIMG = $(wildcard $(addprefix $(SRCDIR)/,plugins/video/loader/sgi/*.cpp))

OBJ.CSSGIIMG = $(addprefix $(OUT)/,$(notdir $(SRC.CSSGIIMG:.cpp=$O)))
DEP.CSSGIIMG = CSUTIL CSGFX CSUTIL

MSVC.DSP += CSSGIIMG
DSP.CSSGIIMG.NAME = cssgiimg
DSP.CSSGIIMG.TYPE = plugin

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: cssgiimg cssgiimgclean

cssgiimg: $(OUTDIRS) $(CSSGIIMG)

$(CSSGIIMG): $(OBJ.CSSGIIMG) $(LIB.CSSGIIMG)
	$(DO.PLUGIN)

clean: cssgiimgclean
cssgiimgclean:
	-$(RMDIR) $(CSSGIIMG) $(OBJ.CSSGIIMG) $(OUTDLL)/$(notdir $(INF.CSSGIIMG))

ifdef DO_DEPEND
dep: $(OUTOS)/sgiimg.dep
$(OUTOS)/sgiimg.dep: $(SRC.CSSGIIMG)
	$(DO.DEP)
else
-include $(OUTOS)/sgiimg.dep
endif

endif # ifeq ($(MAKESECTION),targets)

