# Application description
DESCRIPTION.g2dtest = Crystal Space canvas plugin test

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Application-specific help commands
APPHELP+=$(NEWLINE)echo $"  make g2dtest      Make the $(DESCRIPTION.g2dtest)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: g2dtest g2dtstclean

all apps: g2dtest
g2dtest:
	$(MAKE_APP)
g2dtstclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp apps/tests/g2dtest

G2DTEST.EXE = g2dtest$(EXE)
INC.G2DTEST =
SRC.G2DTEST = apps/tests/g2dtest/g2dtest.cpp
OBJ.G2DTEST = $(addprefix $(OUT)/,$(notdir $(SRC.G2DTEST:.cpp=$O)))
DEP.G2DTEST = CSTOOL CSGFX CSSYS CSUTIL CSGEOM
LIB.G2DTEST = $(foreach d,$(DEP.G2DTEST),$($d.LIB))

TO_INSTALL.EXE += $(G2DTEST.EXE)

MSVC.DSP += G2DTEST
DSP.G2DTEST.NAME = g2dtest
DSP.G2DTEST.TYPE = appcon

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: build.g2dtest g2dtstclean

build.g2dtest: $(OUTDIRS) $(G2DTEST.EXE)
clean: g2dtstclean

$(G2DTEST.EXE): $(DEP.EXE) $(OBJ.G2DTEST) $(LIB.G2DTEST)
	$(DO.LINK.EXE)

g2dtstclean:
	-$(RM) $(G2DTEST.EXE) $(OBJ.G2DTEST)

ifdef DO_DEPEND
dep: $(OUTOS)/g2dtest.dep
$(OUTOS)/g2dtest.dep: $(SRC.G2DTEST)
	$(DO.DEP)
else
-include $(OUTOS)/g2dtest.dep
endif

endif # ifeq ($(MAKESECTION),targets)
