# Application description
DESCRIPTION.picview = Crystal Space Picture Viewer

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Application-specific help commands
APPHELP += $(NEWLINE)echo $"  make picview      Make the $(DESCRIPTION.picview)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: picview picviewclean

all apps: picview
picview:
	$(MAKE_APP)
picviewclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)


PICVIEW.EXE = picview$(EXE)
DIR.PICVIEW = apps/tools/picview
OUT.PICVIEW = $(OUT)/$(DIR.PICVIEW)
INC.PICVIEW = $(wildcard $(DIR.PICVIEW)/*.h )
SRC.PICVIEW = $(wildcard $(DIR.PICVIEW)/*.cpp )
OBJ.PICVIEW = $(addprefix $(OUT.PICVIEW)/,$(notdir $(SRC.PICVIEW:.cpp=$O)))
DEP.PICVIEW = \
  CSWS CSTOOL CSTOOL CSGFX CSUTIL CSSYS CSGEOM CSUTIL
LIB.PICVIEW = $(foreach d,$(DEP.PICVIEW),$($d.LIB))

#TO_INSTALL.EXE    += $(picview.EXE)
#TO_INSTALL.CONFIG += $(CFG.PICVIEW)

MSVC.DSP += PICVIEW
DSP.PICVIEW.NAME = picview
DSP.PICVIEW.TYPE = appcon

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: build.picview picviewclean picviewcleandep

all: $(PICVIEW.EXE)
build.picview: $(OUT.PICVIEW) $(PICVIEW.EXE)
clean: picviewclean

$(OUT.PICVIEW)/%$O: $(DIR.PICVIEW)/%.cpp
	$(DO.COMPILE.CPP)

$(PICVIEW.EXE): $(DEP.EXE) $(OBJ.PICVIEW) $(LIB.PICVIEW)
	$(DO.LINK.EXE)

$(OUT.PICVIEW):
	$(MKDIRS)

picviewclean:
	-$(RMDIR) $(PICVIEW.EXE) $(OBJ.PICVIEW)

cleandep: picviewcleandep
picviewcleandep:
	-$(RM) $(OUT.PICVIEW)/picview.dep

ifdef DO_DEPEND
dep: $(OUT.PICVIEW) $(OUT.PICVIEW)/picview.dep
$(OUT.PICVIEW)/picview.dep: $(SRC.PICVIEW)
	$(DO.DEPEND)
else
-include $(OUT.PICVIEW)/picview.dep
endif

endif # ifeq ($(MAKESECTION),targets)
