# This is a subinclude file used to define the rules needed
# to build DOS Allegro driver alleg2d

# Driver description
DESCRIPTION.alleg2d = Crystal Space Allegro driver

#-------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Driver-specific help commands
DRVHELP += $(NEWLINE)echo $"  make alleg2d      Make the $(DESCRIPTION.alleg2d)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#-------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: alleg2d

all drivers drivers2d: alleg2d

alleg2d:
	$(MAKE_TARGET) MAKE_DLL=yes

endif # ifeq ($(MAKESECTION),roottargets)

#-------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

# The Allegro 2D DOS driver
ALLEG2D=$(OUT)$(LIB_PREFIX)alleg2d$(LIB)
DEP.EXE+=$(ALLEG2D)
DESCRIPTION.$(ALLEG2D) = $(DESCRIPTION.alleg2d)
SRC.ALLEG2D=$(wildcard libs/cs2d/dosalleg/*.cpp $(SRC.COMMON.DRV2D))
OBJ.ALLEG2D = $(addprefix $(OUT),$(notdir $(SRC.ALLEG2D:.cpp=$O)))
CFLAGS.STATIC_COM+=$(CFLAGS.D)SCL_ALLEG2D

endif # ifeq ($(MAKESECTION),postdefines)

#------------------------------------------------------------------ targets ---#
ifeq ($(MAKESECTION),targets)

vpath %.cpp libs/cs2d/dosalleg

.PHONY: alleg2dclean alleg2dcleanlib alleg2d

# Chain rules
clean: alleg2dclean
cleanlib: alleg2dcleanlib
alleg2d: $(OUTDIRS) $(ALLEG2D)

$(ALLEG2D): $(OBJ.ALLEG2D)
	$(DO.LIBRARY)

alleg2dclean:
	$(RM) $(ALLEG2D)

alleg2dcleanlib:
	$(RM) $(OBJ.ALLEG2D) $(ALLEG2D)

ifdef DO_DEPEND
depend: $(OUTOS)alleg2d.dep
$(OUTOS)alleg2d.dep: $(SRC.ALLEG2D)
	$(DO.DEP)
else
-include $(OUTOS)alleg2d.dep
endif

endif # ifeq ($(MAKESECTION),targets)
