# This is a subinclude file used to define the rules needed
# to build the GLX 2D driver -- glx2d

# Driver description
DESCRIPTION.glx2d = Crystal Space GL/X 2D driver

#-------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Driver-specific help commands
DRIVERHELP += $(NEWLINE)echo $"  make glx2d        Make the $(DESCRIPTION.glx2d)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#-------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: glx2d

all plugins drivers drivers2d: glx2d

glx2d:
	$(MAKE_TARGET) MAKE_DLL=yes
glx2dclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#-------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

# Local CFLAGS and libraries
LIBS._GLX2D+=-L$(X11_PATH)/lib -lXext -lX11

ifeq ($(USE_MESA),1)
  ifdef MESA_PATH
    CFLAGS.GLX2D+=-I$(MESA_PATH)/include
    LIBS._GLX2D+=-L$(MESA_PATH)/lib
  endif
  LIBS._GLX2D+=-lMesaGL -lMesaGLU
else
  LIBS._GLX2D+=-lGL
endif

CFLAGS.GLX2D+=-I$(X11_PATH)/include

# The 2D GLX driver
ifeq ($(USE_SHARED_PLUGINS),yes)
  GLX2D=$(OUTDLL)glx2d$(DLL)
  LIBS.GLX2D=$(LIBS._GLX2D)
  DEP.GLX2D=$(CSGEOM.LIB) $(CSUTIL.LIB) $(CSSYS.LIB)
else
  GLX2D=$(OUT)$(LIB_PREFIX)glx2d$(LIB)
  DEP.EXE+=$(GLX2D)
  LIBS.EXE+=$(LIBS._GLX2D)
  CFLAGS.STATIC_SCF+=$(CFLAGS.D)SCL_GLX2D
endif
DESCRIPTION.$(GLX2D) = $(DESCRIPTION.glx2d)
SRC.GLX2D = $(wildcard libs/cs2d/openglx/*.cpp \
  $(SRC.COMMON.DRV2D.OPENGL) $(SRC.COMMON.DRV2D))
OBJ.GLX2D = $(addprefix $(OUT),$(notdir $(SRC.GLX2D:.cpp=$O)))

endif # ifeq ($(MAKESECTION),postdefines)

#------------------------------------------------------------------ targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: glx2d glxclean

# Chain rules
clean: glxclean

glx2d: $(OUTDIRS) $(GLX2D)

$(OUT)%$O: libs/cs2d/openglx/%.cpp
	$(DO.COMPILE.CPP) $(CFLAGS.GLX2D)
 
$(OUT)%$O: libs/cs2d/openglcommon/%.cpp
	$(DO.COMPILE.CPP) $(CFLAGS.GLX2D)
 
$(GLX2D): $(OBJ.GLX2D) $(DEP.GLX2D)
	$(DO.PLUGIN) $(LIBS.GLX2D)

glx2dclean:
	$(RM) $(GLX2D) $(OBJ.GLX2D)
 
ifdef DO_DEPEND
dep: $(OUTOS)glx2d.dep
$(OUTOS)glx2d.dep: $(SRC.GLX2D)
	$(DO.DEP1) $(CFLAGS.GLX2D) $(DO.DEP2)
else
-include $(OUTOS)glx2d.dep
endif

endif # ifeq ($(MAKESECTION),targets)
