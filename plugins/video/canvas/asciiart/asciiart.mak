# This is a subinclude file used to define the rules needed
# to build the Ascii Art 2D driver using libaa

# Driver description
DESCRIPTION.asciiart = Crystal Space Ascii Art driver

#-------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Driver-specific help commands
DRVHELP += $(NEWLINE)echo $"  make asciiart     Make the $(DESCRIPTION.asciiart)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#-------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: asciiart

all drivers drivers2d: asciiart

asciiart:
	$(MAKE_TARGET) MAKE_DLL=yes

endif # ifeq ($(MAKESECTION),roottargets)

#-------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

# The AsciiArt library
LIBS.ASCIIART+=$(LFLAGS.l)aa

# The 2D Ascii Art driver
ifeq ($(USE_DLL),yes)
  ASCIIART=asciiart$(DLL)
  DEP.ASCIIART=$(CSCOM.LIB) $(CSUTIL.LIB) $(CSSYS.LIB)
  LIBS.LOCAL.ASCIIART=$(LIBS.ASCIIART)
else
  ASCIIART=$(OUT)$(LIB_PREFIX)asciiart$(LIB)
  DEP.EXE+=$(ASCIIART) $(LIBS.ASCIIART)
  CFLAGS.STATIC_COM+=$(CFLAGS.D)SCL_ASCII2D
endif
DESCRIPTION.$(ASCIIART)=$(DESCRIPTION.asciiart)
SRC.ASCIIART = $(wildcard libs/cs2d/asciiart/*.cpp $(SRC.COMMON.DRV2D))
OBJ.ASCIIART = $(addprefix $(OUT),$(notdir $(SRC.ASCIIART:.cpp=$O)))

endif # ifeq ($(MAKESECTION),postdefines)

#------------------------------------------------------------------ targets ---#
ifeq ($(MAKESECTION),targets)

vpath %.cpp libs/cs2d/asciiart

.PHONY: asciiart asciiartclean asciiartcleanlib

# Chain rules
clean: asciiartclean
cleanlib: asciiartcleanlib

asciiart: $(OUTDIRS) $(ASCIIART)

$(ASCIIART): $(OBJ.ASCIIART) $(DEP.ASCIIART)
	$(DO.LIBRARY) $(LIBS.LOCAL.ASCIIART)

asciiartclean:
	$(RM) $(ASCIIART)

asciiartcleanlib:
	$(RM) $(OBJ.ASCIIART) $(ASCIIART)

ifdef DO_DEPEND
depend: $(OUTOS)asciiart.dep
$(OUTOS)asciiart.dep: $(SRC.ASCIIART)
	$(DO.DEP)
else
-include $(OUTOS)asciiart.dep
endif

endif # ifeq ($(MAKESECTION),targets)
