# Application description
DESCRIPTION.cswstest = Crystal Space Windowing System test

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Application-specific help commands
APPHELP+=$(NEWLINE)echo $"  make cswstest     Make the $(DESCRIPTION.cswstest)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: cswstest cswstestclean

all apps: cswstest
cswstest:
	$(MAKE_APP)
cswstestclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

CSWSTEST.EXE = cswstest$(EXE)
DIR.CSWSTEST = apps/cswstest
OUT.CSWSTEST = $(OUT)/$(DIR.CSWSTEST)
INC.CSWSTEST = $(wildcard $(addprefix $(SRCDIR)/,$(DIR.CSWSTEST)/*.h))
SRC.CSWSTEST = $(wildcard $(addprefix $(SRCDIR)/,$(DIR.CSWSTEST)/*.cpp))
OBJ.CSWSTEST = $(addprefix $(OUT.CSWSTEST)/,$(notdir $(SRC.CSWSTEST:.cpp=$O)))
DEP.CSWSTEST = CSWS CSGFX CSGEOM CSSYS CSGEOM CSUTIL CSTOOL CSUTIL CSSYS CSUTIL
LIB.CSWSTEST = $(foreach d,$(DEP.CSWSTEST),$($d.LIB))
CFG.CSWSTEST = $(SRCDIR)/data/config/cswstest.cfg

OUTDIRS += $(OUT.CSWSTEST)

#TO_INSTALL.EXE    += $(CSWSTEST.EXE)
#TO_INSTALL.CONFIG += $(CFG.CSWSTEST)

MSVC.DSP += CSWSTEST
DSP.CSWSTEST.NAME = cswstest
DSP.CSWSTEST.TYPE = appgui

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: build.cswstest cswstestclean cswstestcleandep

all: $(CSWSTEST.EXE)
build.cswstest: $(OUTDIRS) $(CSWSTEST.EXE)
clean: cswstestclean

$(OUT.CSWSTEST)/%$O: $(SRCDIR)/$(DIR.CSWSTEST)/%.cpp
	$(DO.COMPILE.CPP)

$(CSWSTEST.EXE): $(DEP.EXE) $(OBJ.CSWSTEST) $(LIB.CSWSTEST)
	$(DO.LINK.EXE)

cswstestclean:
	-$(RM) cswstest.txt
	-$(RMDIR) $(CSWSTEST.EXE) $(OBJ.CSWSTEST)

cleandep: cswstestcleandep
cswstestcleandep:
	-$(RM) $(OUT.CSWSTEST)/cswstest.dep

ifdef DO_DEPEND
dep: $(OUT.CSWSTEST) $(OUT.CSWSTEST)/cswstest.dep
$(OUT.CSWSTEST)/cswstest.dep: $(SRC.CSWSTEST)
	$(DO.DEPEND)
else
-include $(OUT.CSWSTEST)/cswstest.dep
endif

endif # ifeq ($(MAKESECTION),targets)
