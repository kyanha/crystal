# This is a subinclude file used to define the rules needed
# to build the X-windows 2D driver -- x2d

# Driver description
DESCRIPTION.x2d = Crystal Space XLib 2D driver

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Driver-specific help commands
DRIVERHELP += \
  $(NEWLINE)echo $"  make x2d          Make the $(DESCRIPTION.x2d)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: x2d x2dclean
all softcanvas plugins drivers drivers2d: x2d

x2d:
	$(MAKE_TARGET) MAKE_DLL=yes
x2dclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

CFLAGS.X2D += $(X_CFLAGS)
LIB.X2D.SYSTEM += $(X_PRE_LIBS) $(X_LIBS) -lX11 $(X_EXTRA_LIBS)

ifeq ($(USE_PLUGINS),yes)
  X2D = $(OUTDLL)/x2d$(DLL)
  LIB.X2D = $(foreach d,$(DEP.X2D),$($d.LIB))
  LIB.X2D.SPECIAL = $(LIB.X2D.SYSTEM)
  TO_INSTALL.DYNAMIC_LIBS += $(X2D)
else
  X2D = $(OUT)/$(LIB_PREFIX)x2d$(LIB)
  DEP.EXE += $(X2D)
  LIBS.EXE += $(LIB.X2D.SYSTEM)
  SCF.STATIC += x2d
  TO_INSTALL.STATIC_LIBS += $(X2D)
endif

INC.X2D = $(wildcard plugins/video/canvas/softx/*.h $(INC.COMMON.DRV2D))
SRC.X2D = $(wildcard plugins/video/canvas/softx/*.cpp $(SRC.COMMON.DRV2D))
OBJ.X2D = $(addprefix $(OUT)/,$(notdir $(SRC.X2D:.cpp=$O)))
DEP.X2D = CSUTIL CSSYS CSGEOM CSUTIL

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: x2d x2dclean

x2d: $(OUTDIRS) $(X2D)

$(OUT)/%$O: plugins/video/canvas/softx/%.cpp
	$(DO.COMPILE.CPP) $(CFLAGS.X2D)

$(X2D): $(OBJ.X2D) $(LIB.X2D)
	$(DO.PLUGIN) $(LIB.X2D.SPECIAL)

clean: x2dclean
x2dclean:
	$(RM) $(X2D) $(OBJ.X2D)

ifdef DO_DEPEND
dep: $(OUTOS)/x2d.dep
$(OUTOS)/x2d.dep: $(SRC.X2D)
	$(DO.DEP1) $(CFLAGS.X2D) $(DO.DEP2)
else
-include $(OUTOS)/x2d.dep
endif

endif # ifeq ($(MAKESECTION),targets)
