# Application description
DESCRIPTION.simpvs = Crystal Space tutorial, video selector

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Application-specific help commands
APPHELP += \
  $(NEWLINE)echo $"  make simpvs       Make the $(DESCRIPTION.simpvs)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: simpvs tutsimpvsclean

all apps: simpvs
simpvs:
	$(MAKE_APP)
tutsimpvsclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp apps/tutorial/simpvs

SIMPVS.EXE = simpvs$(EXE)
INC.SIMPVS = $(wildcard apps/tutorial/simpvs/*.h)
SRC.SIMPVS = $(wildcard apps/tutorial/simpvs/*.cpp)
OBJ.SIMPVS = $(addprefix $(OUT),$(notdir $(SRC.SIMPVS:.cpp=$O)))
DEP.SIMPVS = CSTOOL CSGFX CSUTIL CSSYS CSGEOM CSUTIL CSSYS
LIB.SIMPVS = $(foreach d,$(DEP.SIMPVS),$($d.LIB))

#TO_INSTALL.EXE += $(SIMPVS.EXE)

MSVC.DSP += SIMPVS
DSP.SIMPVS.NAME = simpvs
DSP.SIMPVS.TYPE = appcon

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: build.simpvs tutsimpvsclean

all: $(SIMPVS.EXE)
build.simpvs: $(OUTDIRS) $(SIMPVS.EXE)
clean: tutsimpvsclean

$(SIMPVS.EXE): $(DEP.EXE) $(OBJ.SIMPVS) $(LIB.SIMPVS)
	$(DO.LINK.EXE)

tutsimpvsclean:
	-$(RM) $(SIMPVS.EXE) $(OBJ.SIMPVS)

ifdef DO_DEPEND
dep: $(OUTOS)simpvs.dep
$(OUTOS)simpvs.dep: $(SRC.SIMPVS)
	$(DO.DEP)
else
-include $(OUTOS)simpvs.dep
endif

endif # ifeq ($(MAKESECTION),targets)
