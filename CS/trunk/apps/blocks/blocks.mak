# Application description
DESCRIPTION.blks = Crystal Space Blocks game

#-------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Application-specific help commands
APPHELP += $(NEWLINE)echo $"  make blks         Make the $(DESCRIPTION.blks)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#-------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: blks

all apps: blks
blks:
	$(MAKE_TARGET)

endif # ifeq ($(MAKESECTION),roottargets)

#-------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp apps/blocks apps/support

BLOCKS.EXE=blocks$(EXE)
SRC.BLOCKS = $(wildcard apps/blocks/*.cpp) \
  apps/support/static.cpp
OBJ.BLOCKS = $(addprefix $(OUT),$(notdir $(SRC.BLOCKS:.cpp=$O)))
DESCRIPTION.$(BLOCKS.EXE) = $(DESCRIPTION.blks)

endif # ifeq ($(MAKESECTION),postdefines)

#------------------------------------------------------------------ targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: blks blksclean

all: $(BLOCKS.EXE)
blks: $(OUTDIRS) $(BLOCKS.EXE)
clean: blksclean

$(BLOCKS.EXE): $(DEP.EXE) $(OBJ.BLOCKS) \
  $(CSPARSER.LIB) $(CSENGINE.LIB) $(CSTERR.LIB) $(CSSCRIPT.LIB)\
  $(CSSFXLDR.LIB) $(CSGFXLDR.LIB) $(CSUTIL.LIB) $(CSSYS.LIB) $(CSGEOM.LIB) \
  $(CSOBJECT.LIB) $(CSUTIL.LIB)
	$(DO.LINK.EXE)

blksclean:
	-$(RM) $(BLOCKS.EXE)

ifdef DO_DEPEND
depend: $(OUTOS)blocks.dep
$(OUTOS)blocks.dep: $(SRC.BLOCKS)
	$(DO.DEP)
else
-include $(OUTOS)blocks.dep
endif

endif # ifeq ($(MAKESECTION),targets)
