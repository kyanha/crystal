# Application description
DESCRIPTION.simple1 = Crystal Space tutorial part one

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Application-specific help commands
APPHELP += \
  $(NEWLINE)echo $"  make simple1      Make the $(DESCRIPTION.simple1)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: simple1 tutsimp1clean

all apps: simple1
simple1:
	$(MAKE_APP)
tutsimp1clean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp apps/tutorial/simple1

SIMPLE1.EXE = simple1$(EXE)
INC.SIMPLE1 = $(wildcard apps/tutorial/simple1/*.h)
SRC.SIMPLE1 = $(wildcard apps/tutorial/simple1/*.cpp)
OBJ.SIMPLE1 = $(addprefix $(OUT),$(notdir $(SRC.SIMPLE1:.cpp=$O)))
DEP.SIMPLE1 = CSTOOL CSGFX CSUTIL CSSYS CSGEOM CSUTIL CSSYS
LIB.SIMPLE1 = $(foreach d,$(DEP.SIMPLE1),$($d.LIB))

#TO_INSTALL.EXE += $(SIMPLE1.EXE)

MSVC.DSP += SIMPLE1
DSP.SIMPLE1.NAME = simple1
DSP.SIMPLE1.TYPE = appcon

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: build.simple1 tutsimp1clean

all: $(SIMPLE1.EXE)
build.simple1: $(OUTDIRS) $(SIMPLE1.EXE)
clean: tutsimp1clean

$(SIMPLE1.EXE): $(DEP.EXE) $(OBJ.SIMPLE1) $(LIB.SIMPLE1)
	$(DO.LINK.EXE)

tutsimp1clean:
	-$(RM) $(SIMPLE1.EXE) $(OBJ.SIMPLE1)

ifdef DO_DEPEND
dep: $(OUTOS)simple1.dep
$(OUTOS)simple1.dep: $(SRC.SIMPLE1)
	$(DO.DEP)
else
-include $(OUTOS)simple1.dep
endif

endif # ifeq ($(MAKESECTION),targets)
