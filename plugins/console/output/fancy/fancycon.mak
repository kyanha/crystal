DESCRIPTION.fancycon = Crystal Space fancy output console

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

PLUGINHELP += \
  $(NEWLINE)echo $"  make fancycon     Make the $(DESCRIPTION.fancycon)$"

endif # ifeq ($(MAKESECTION),rootdefines)
#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: fancycon fancyconclean
plugins all: fancycon

fancycon:
	$(MAKE_TARGET) MAKE_DLL=yes
fancyconclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)
#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp $(SRCDIR)/plugins/console/output/fancy

ifeq ($(USE_PLUGINS),yes)
  FANCYCON = $(OUTDLL)/fancycon$(DLL)
  LIB.FANCYCON = $(foreach d,$(DEP.FANCYCON),$($d.LIB))
  TO_INSTALL.DYNAMIC_LIBS += $(FANCYCON)
else
  FANCYCON = $(OUT)/$(LIB_PREFIX)fancycon$(LIB)
  DEP.EXE += $(FANCYCON)
  SCF.STATIC += fancycon
  TO_INSTALL.STATIC_LIBS += $(FANCYCON)
endif

INF.FANCYCON = $(SRCDIR)/plugins/console/output/fancy/fancycon.csplugin
INC.FANCYCON = $(wildcard $(addprefix $(SRCDIR)/,plugins/console/output/fancy/*.h))
SRC.FANCYCON = $(wildcard $(addprefix $(SRCDIR)/,plugins/console/output/fancy/*.cpp))
OBJ.FANCYCON = $(addprefix $(OUT)/,$(notdir $(SRC.FANCYCON:.cpp=$O)))
DEP.FANCYCON = CSUTIL CSGEOM CSSYS CSUTIL
CFG.FANCYCON = $(SRCDIR)/data/config/fancycon.cfg

TO_INSTALL.CONFIG += $(CFG.FANCYCON)
TO_INSTALL.DATA += $(SRCDIR)/data/fancycon.zip

MSVC.DSP += FANCYCON
DSP.FANCYCON.NAME = fancycon
DSP.FANCYCON.TYPE = plugin

endif # ifeq ($(MAKESECTION),postdefines)
#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: fancycon fancyconclean
fancycon: $(OUTDIRS) $(FANCYCON)

$(FANCYCON): $(OBJ.FANCYCON) $(LIB.FANCYCON)
	$(DO.PLUGIN)

clean: fancyconclean
fancyconclean:
	-$(RMDIR) $(FANCYCON) $(OBJ.FANCYCON) $(OUTDLL)/$(notdir $(INF.FANCYCON))

ifdef DO_DEPEND
dep: $(OUTOS)/fancycon.dep
$(OUTOS)/fancycon.dep: $(SRC.FANCYCON)
	$(DO.DEP)
else
-include $(OUTOS)/fancycon.dep
endif

endif # ifeq ($(MAKESECTION),targets)
