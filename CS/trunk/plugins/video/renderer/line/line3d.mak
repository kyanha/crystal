# This is a subinclude file used to define the rules needed
# to build the 3D line rendering driver.

# Driver description
DESCRIPTION.line = Crystal Space line 3D renderer

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Driver-specific help commands
DRIVERHELP += \
  $(NEWLINE)echo $"  make line         Make the $(DESCRIPTION.line)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: line
all plugins drivers drivers3d: line

line:
	$(MAKE_TARGET) MAKE_DLL=yes
lineclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp plugins/video/renderer/line

ifeq ($(USE_PLUGINS),yes)
  LINE3D = $(OUTDLL)line3d$(DLL)
  LIB.LINE3D = $(foreach d,$(DEP.LINE3D),$($d.LIB))
  TO_INSTALL.DYNAMIC_LIBS += $(LINE3D)
else
  LINE3D = $(OUT)$(LIB_PREFIX)line$(LIB)
  DEP.EXE += $(LINE3D)
  CFLAGS.STATIC_SCF += $(CFLAGS.D)SCL_LINE3D
  TO_INSTALL.STATIC_LIBS += $(LINE3D)
endif

INC.LINE3D = $(wildcard plugins/video/renderer/line/*.h) \
  plugins/video/renderer/common/txtmgr.h \
  plugins/video/renderer/common/dtmesh.h \
  plugins/video/renderer/common/dpmesh.h
SRC.LINE3D = $(wildcard plugins/video/renderer/line/*.cpp) \
  plugins/video/renderer/common/txtmgr.cpp \
  plugins/video/renderer/common/dtmesh.cpp \
  plugins/video/renderer/common/dpmesh.cpp
OBJ.LINE3D = $(addprefix $(OUT),$(notdir $(SRC.LINE3D:.cpp=$O)))
DEP.LINE3D = CSGEOM CSGFXLDR CSUTIL CSSYS

MSVC.DSP += LINE3D
DSP.LINE3D.NAME = line3d
DSP.LINE3D.TYPE = plugin

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: line lineclean

# Chain rules
all: $(LINE3D)
clean: lineclean

line: $(OUTDIRS) $(LINE3D)

$(LINE3D): $(OBJ.LINE3D) $(LIB.LINE3D)
	$(DO.PLUGIN)

lineclean:
	$(RM) $(LINE3D) $(OBJ.LINE3D) $(OUTOS)line3d.dep

ifdef DO_DEPEND
dep: $(OUTOS)line3d.dep
$(OUTOS)line3d.dep: $(SRC.LINE3D)
	$(DO.DEP)
else
-include $(OUTOS)line3d.dep
endif

endif # ifeq ($(MAKESECTION),targets)
