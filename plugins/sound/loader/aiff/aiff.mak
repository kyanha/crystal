# Plug-in description
DESCRIPTION.sndaiff = Crystal Space aiff sound loader

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Plug-in-specific help commands
PLUGINHELP += \
  $(NEWLINE)echo $"  make sndaiff      Make the $(DESCRIPTION.sndaiff)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: sndaiff sndaiffclean
all plugins: sndaiff

sndaiff:
	$(MAKE_TARGET) MAKE_DLL=yes
sndaiffclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp $(SRCDIR)/plugins/sound/loader/aiff $(SRCDIR)/plugins/sound/loader/common

ifeq ($(USE_PLUGINS),yes)
  SNDAIFF = $(OUTDLL)/sndaiff$(DLL)
  LIB.SNDAIFF = $(foreach d,$(DEP.SNDAIFF),$($d.LIB))
  TO_INSTALL.DYNAMIC_LIBS += $(SNDAIFF)
else
  SNDAIFF = $(OUT)/$(LIB_PREFIX)sndaiff$(LIB)
  DEP.EXE += $(SNDAIFF)
  SCF.STATIC += sndaiff
  TO_INSTALL.STATIC_LIBS += $(SNDAIFF)
endif

INF.SNDAIFF = $(SRCDIR)/plugins/sound/loader/aiff/sndaiff.csplugin
INC.SNDAIFF = $(wildcard $(addprefix $(SRCDIR)/, \
  plugins/sound/loader/aiff/*.h plugins/sound/loader/common/*.h))
SRC.SNDAIFF = $(wildcard $(addprefix $(SRCDIR)/, \
  plugins/sound/loader/aiff/*.cpp plugins/sound/loader/common/*.cpp))
OBJ.SNDAIFF = $(addprefix $(OUT)/,$(notdir $(SRC.SNDAIFF:.cpp=$O)))
DEP.SNDAIFF = CSUTIL CSSYS CSUTIL

MSVC.DSP += SNDAIFF
DSP.SNDAIFF.NAME = sndaiff
DSP.SNDAIFF.TYPE = plugin

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: sndaiff sndaiffclean

sndaiff: $(OUTDIRS) $(SNDAIFF)

$(SNDAIFF): $(OBJ.SNDAIFF) $(LIB.SNDAIFF)
	$(DO.PLUGIN)

clean: sndaiffclean
sndaiffclean:
	-$(RMDIR) $(SNDAIFF) $(OBJ.SNDAIFF) $(OUTDLL)/$(notdir $(INF.SNDAIFF))

ifdef DO_DEPEND
dep: $(OUTOS)/sndaiff.dep
$(OUTOS)/sndaiff.dep: $(SRC.SNDAIFF)
	$(DO.DEP)
else
-include $(OUTOS)/sndaiff.dep
endif

endif # ifeq ($(MAKESECTION),targets)
