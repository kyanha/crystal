# Application description
DESCRIPTION.r3dtest = Test of new renderer

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Application-specific help commands
APPHELP += \
  $(NEWLINE)echo $"  make r3dtest      Make the $(DESCRIPTION.r3dtest)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: r3dtest r3dtestclean

all apps: r3dtest
r3dtest:
	$(MAKE_APP)
r3dtestclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)


R3DTEST.EXE = r3dtest$(EXE)
DIR.R3DTEST = apps/tests/r3dtest
OUT.R3DTEST = $(OUT)/$(DIR.R3DTEST)
INC.R3DTEST = $(wildcard $(DIR.R3DTEST)/*.h )
SRC.R3DTEST = $(wildcard $(DIR.R3DTEST)/*.cpp )
OBJ.R3DTEST = $(addprefix $(OUT.R3DTEST)/,$(notdir $(SRC.R3DTEST:.cpp=$O)))
DEP.R3DTEST = CSTOOL CSGFX CSUTIL CSSYS CSGEOM CSUTIL CSSYS
LIB.R3DTEST = $(foreach d,$(DEP.R3DTEST),$($d.LIB))

#TO_INSTALL.EXE += $(R3DTEST.EXE)

MSVC.DSP += R3DTEST
DSP.R3DTEST.NAME = r3dtest
DSP.R3DTEST.TYPE = appcon

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: build.r3dtest r3dtestclean r3dtestcleandep

all: $(R3DTEST.EXE)
build.r3dtest: $(OUT.R3DTEST) $(R3DTEST.EXE)
clean: r3dtestclean

$(OUT.R3DTEST)/%$O: $(DIR.R3DTEST)/%.cpp
	$(DO.COMPILE.CPP)

$(R3DTEST.EXE): $(DEP.EXE) $(OBJ.R3DTEST) $(LIB.R3DTEST)
	$(DO.LINK.EXE)

$(OUT.R3DTEST):
	$(MKDIRS)

r3dtestclean:
	-$(RM) r3dtest.txt
	-$(RMDIR) $(R3DTEST.EXE) $(OBJ.R3DTEST)

cleandep: r3dtestcleandep
r3dtestcleandep:
	-$(RM) $(OUT.R3DTEST)/r3dtest.dep

ifdef DO_DEPEND
dep: $(OUT.R3DTEST) $(OUT.R3DTEST)/r3dtest.dep
$(OUT.R3DTEST)/r3dtest.dep: $(SRC.R3DTEST)
	$(DO.DEPEND)
else
-include $(OUT.R3DTEST)/r3dtest.dep
endif

endif # ifeq ($(MAKESECTION),targets)
