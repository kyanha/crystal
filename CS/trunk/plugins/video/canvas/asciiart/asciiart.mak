# This is a subinclude file used to define the rules needed
# to build the Ascii Art 2D driver using libaa

# Driver description
DESCRIPTION.asciiart = Crystal Space Ascii Art driver

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Driver-specific help commands
DRIVERHELP += \
  $(NEWLINE)echo $"  make asciiart     Make the $(DESCRIPTION.asciiart)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: asciiart asciiartclean

all plugins drivers drivers2d: asciiart

asciiart:
	$(MAKE_TARGET) MAKE_DLL=yes
asciiartclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

# The AsciiArt library
LIBS.ASCIIART+=$(LFLAGS.l)aa
# On Unix we need additional libraries
ifdef X11_PATH
  LIBS.ASCIIART+=-lgpm -lslang -L$(X11_PATH)/lib -lXext -lX11 $(X11_EXTRA_LIBS)
endif

# The 2D Ascii Art driver
ifeq ($(USE_SHARED_PLUGINS),yes)
  ASCIIART=asciiart$(DLL)
  DEP.ASCIIART=$(CSUTIL.LIB) $(CSSYS.LIB)
  LIBS.LOCAL.ASCIIART=$(LIBS.ASCIIART)
  TO_INSTALL.DYNAMIC_LIBS+=$(ASCIIART)
else
  ASCIIART=$(OUT)$(LIB_PREFIX)asciiart$(LIB)
  DEP.EXE+=$(ASCIIART)
  LIBS.EXE+=$(LIBS.ASCIIART)
  CFLAGS.STATIC_SCF+=$(CFLAGS.D)SCL_ASCII2D
  TO_INSTALL.STATIC_LIBS+=$(ASCIIART)
endif
TO_INSTALL.CONFIG += data/config/asciiart.cfg
DESCRIPTION.$(ASCIIART)=$(DESCRIPTION.asciiart)
SRC.ASCIIART = $(wildcard plugins/video/canvas/asciiart/*.cpp \
  $(SRC.COMMON.DRV2D))
OBJ.ASCIIART = $(addprefix $(OUT),$(notdir $(SRC.ASCIIART:.cpp=$O)))

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

vpath %.cpp plugins/video/canvas/asciiart

.PHONY: asciiart asciiartclean

# Chain rules
clean: asciiartclean

asciiart: $(OUTDIRS) $(ASCIIART)

$(ASCIIART): $(OBJ.ASCIIART) $(DEP.ASCIIART)
	$(DO.PLUGIN) $(LIBS.LOCAL.ASCIIART)

asciiartclean:
	$(RM) $(ASCIIART) $(OBJ.ASCIIART) $(OUTOS)asciiart.dep

ifdef DO_DEPEND
dep: $(OUTOS)asciiart.dep
$(OUTOS)asciiart.dep: $(SRC.ASCIIART)
	$(DO.DEP)
else
-include $(OUTOS)asciiart.dep
endif

endif # ifeq ($(MAKESECTION),targets)
