# Application description
DESCRIPTION.walk = Crystal Space WalkTest demo executable

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Application-specific help commands
APPHELP += $(NEWLINE)echo $"  make walk         Make the $(DESCRIPTION.walk)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: walk walkclean

walkall: walk vfs soft csfont simpcon perfstat rapid sndload
all apps: walk
walk:
	$(MAKE_TARGET)
walkclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp apps/walktest apps/support

WALKTEST.EXE = walktest$(EXE)
INC.WALKTEST = $(wildcard apps/walktest/*.h) apps/support/command.h
SRC.WALKTEST = $(wildcard apps/walktest/*.cpp) \
  apps/support/static.cpp apps/support/command.cpp
OBJ.WALKTEST = $(addprefix $(OUT),$(notdir $(SRC.WALKTEST:.cpp=$O)))
DEP.WALKTEST = CSPARSER CSENGINE CSTERR CSGEOM CSGFXLDR CSSYS CSUTIL CSOBJECT
LIB.WALKTEST = $(foreach d,$(DEP.WALKTEST),$($d.LIB))
CFG.WALKTEST = data/config/cryst.cfg

TO_INSTALL.EXE    += $(WALKTEST.EXE)
TO_INSTALL.CONFIG += $(CFG.WALKTEST)

MSVC.DSP += WALKTEST
DSP.WALKTEST.NAME = walktest
DSP.WALKTEST.TYPE = appgui

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: walk walkclean

all: $(WALKTEST.EXE)
walk: $(OUTDIRS) $(WALKTEST.EXE)
clean: walkclean

$(WALKTEST.EXE): $(DEP.EXE) $(OBJ.WALKTEST) $(LIB.WALKTEST)
	$(DO.LINK.EXE)

walkclean:
	-$(RM) $(WALKTEST.EXE) $(OBJ.WALKTEST) $(OUTOS)walktest.dep

ifdef DO_DEPEND
dep: $(OUTOS)walktest.dep
$(OUTOS)walktest.dep: $(SRC.WALKTEST)
	$(DO.DEP)
else
-include $(OUTOS)walktest.dep
endif

endif # ifeq ($(MAKESECTION),targets)
