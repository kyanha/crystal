# Application description
DESCRIPTION.mdl2spr = Quake model MDL/MD2 conversion tool

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Application-specific help commands
APPHELP += \
  $(NEWLINE)echo $"  make mdl2spr      Make the $(DESCRIPTION.mdl2spr)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: mdl2spr mdl2sprclean

all apps: mdl2spr
mdl2spr:
	$(MAKE_APP)
mdl2sprclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp apps/import/mdl2spr

MDL2SPR.EXE = mdl2spr$(EXE)
INC.MDL2SPR = $(wildcard apps/import/mdl2spr/*.h)
SRC.MDL2SPR = $(wildcard apps/import/mdl2spr/*.cpp)
OBJ.MDL2SPR = $(addprefix $(OUT)/,$(notdir $(SRC.MDL2SPR:.cpp=$O)))
DEP.MDL2SPR = CSGFX CSUTIL CSSYS CSUTIL CSGEOM
LIB.MDL2SPR = $(foreach d,$(DEP.MDL2SPR),$($d.LIB))

TO_INSTALL.EXE += $(MDL2SPR.EXE)

MSVC.DSP += MDL2SPR
DSP.MDL2SPR.NAME = mdl2spr
DSP.MDL2SPR.TYPE = appcon

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: build.mdl2spr mdl2sprclean

all: $(MDL2SPR.EXE)
build.mdl2spr: $(OUTDIRS) $(MDL2SPR.EXE)
clean: mdl2sprclean

$(MDL2SPR.EXE): $(OBJ.MDL2SPR) $(LIB.MDL2SPR)
	$(DO.LINK.CONSOLE.EXE)

mdl2sprclean:
	-$(RM) $(MDL2SPR.EXE) $(OBJ.MDL2SPR)

ifdef DO_DEPEND
dep: $(OUTOS)/mdl2spr.dep
$(OUTOS)/mdl2spr.dep: $(SRC.MDL2SPR)
	$(DO.DEP)
else
-include $(OUTOS)/mdl2spr.dep
endif

endif # ifeq ($(MAKESECTION),targets)
