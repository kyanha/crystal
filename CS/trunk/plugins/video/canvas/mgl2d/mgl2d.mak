# This is a subinclude file used to define the rules needed
# to build the MGL 2D driver -- mgl2d

# Driver description
DESCRIPTION.mgl2d = Crystal Space MGL 2D driver

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Driver-specific help commands
DRIVERHELP += \
  $(NEWLINE)echo $"  make mgl2d        Make the $(DESCRIPTION.mgl2d)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: mgl2d mgl2dclean

all plugins drivers drivers2d: mgl2d

mgl2d:
	$(MAKE_TARGET) MAKE_DLL=yes
mgl2dclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

# We need also the MGL libs
ifdef MGL_PATH
  CFLAGS.MGL2D += -I$(MGL_PATH)/include
  LIBS.MGL2D += -L$(MGL_PATH)/lib
endif

LIBS.MGL2D += -lgm -lmgl -lpm

ifeq ($(USE_SHARED_PLUGINS),yes)
  MGL2D = $(OUTDLL)mgl2d$(DLL)
  LIBS.LOCAL.MGL2D = $(LIBS.MGL2D)
  DEP.MGL2D = $(CSUTIL.LIB)
else
  MGL2D = $(OUT)$(LIB_PREFIX)mgl2d$(LIB)
  DEP.EXE += $(MGL2D)
  LIBS.EXE += $(LIBS.MGL2D)
  CFLAGS.STATIC_SCF += $(CFLAGS.D)SCL_MGL2D
endif
DESCRIPTION.$(MGL2D) = $(DESCRIPTION.mgl2d)
SRC.MGL2D = $(wildcard plugins/video/canvas/mgl2d/*.cpp $(SRC.COMMON.DRV2D))
OBJ.MGL2D = $(addprefix $(OUT),$(notdir $(SRC.MGL2D:.cpp=$O)))

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: mgl2d mgl2dclean

# Chain rules
clean: mgl2dclean

mgl2d: $(OUTDIRS) $(MGL2D)

$(OUT)%$O: plugins/video/canvas/mgl2d/%.cpp
	$(DO.COMPILE.CPP) $(CFLAGS.MGL2D)
 
$(MGL2D): $(OBJ.MGL2D) $(DEP.MGL2D)
	$(DO.PLUGIN) $(LIBS.LOCAL.MGL2D)

mgl2dclean:
	$(RM) $(MGL2D) $(OBJ.MGL2D) $(OUTOS)mgl2d.dep

ifdef DO_DEPEND
dep: $(OUTOS)mgl2d.dep
$(OUTOS)mgl2d.dep: $(SRC.MGL2D)
	$(DO.DEP1) $(CFLAGS.MGL2D) $(DO.DEP2)
else
-include $(OUTOS)mgl2d.dep
endif

endif # ifeq ($(MAKESECTION),targets)
