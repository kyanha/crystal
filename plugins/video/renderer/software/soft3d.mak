# This is a subinclude file used to define the rules needed
# to build the 3D software rendering driver.

# Driver description
DESCRIPTION.soft3d = Crystal Space software 3D renderer

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Driver-specific help commands
DRIVERHELP += \
  $(NEWLINE)echo $"  make soft3d       Make the $(DESCRIPTION.soft3d)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: soft3d
all plugins drivers drivers3d: soft3d

soft3d:
	$(MAKE_TARGET) MAKE_DLL=yes
soft3dclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp $(SRCDIR)/plugins/video/renderer/common

ifeq ($(USE_PLUGINS),yes)
  SOFT3D = $(OUTDLL)/soft3d$(DLL)
  LIB.SOFT3D = $(foreach d,$(DEP.SOFT3D),$($d.LIB))
  TO_INSTALL.DYNAMIC_LIBS += $(SOFT3D)
else
  SOFT3D = $(OUT)/$(LIB_PREFIX)soft3d$(LIB)
  DEP.EXE += $(SOFT3D)
  SCF.STATIC += soft3d
  TO_INSTALL.STATIC_LIBS += $(SOFT3D)
endif

INF.SOFT3D = $(SRCDIR)/plugins/video/renderer/software/soft3d.csplugin
INC.SOFT3D = $(wildcard $(addprefix $(SRCDIR)/, \
  plugins/video/renderer/software/*.h \
  plugins/video/renderer/common/txtmgr.h \
  plugins/video/renderer/common/dtmesh.h \
  plugins/video/renderer/common/dpmesh.h \
  plugins/video/renderer/common/vbufmgr.h \
  plugins/video/renderer/common/polybuf.h))
SRC.SOFT3D = $(wildcard $(addprefix $(SRCDIR)/, \
  plugins/video/renderer/software/*.cpp \
  plugins/video/renderer/common/txtmgr.cpp \
  plugins/video/renderer/common/dtmesh.cpp \
  plugins/video/renderer/common/dpmesh.cpp \
  plugins/video/renderer/common/vbufmgr.cpp \
  plugins/video/renderer/common/polybuf.cpp))
ifeq ($(NASM.AVAILABLE),yes)
  SRC.SOFT3D += \
    $(wildcard $(SRCDIR)/plugins/video/renderer/software/i386/*.asm)
endif
OBJ.SOFT3D = \
  $(addprefix $(OUT)/,$(notdir $(subst .asm,$O,$(SRC.SOFT3D:.cpp=$O))))
DEP.SOFT3D = CSGEOM CSGFX CSUTIL CSUTIL
CFG.SOFT3D = $(SRCDIR)/data/config/soft3d.cfg

NASMFLAGS.SOFT3D = -i$(SRCDIR)/plugins/video/renderer/software/i386/

TO_INSTALL.CONFIG += $(CFG.SOFT3D)

MSVC.DSP += SOFT3D
DSP.SOFT3D.NAME = soft3d
DSP.SOFT3D.TYPE = plugin
DSP.SOFT3D.RESOURCES = \
  $(wildcard $(SRCDIR)/plugins/video/renderer/software/*.inc)

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: soft3d soft3dclean

# Chain rules
all: $(SOFT3D)
clean: soft3dclean

soft3d: $(OUTDIRS) $(SOFT3D)

# Extra dependencies not generated by "make depend"
$(OUT)/scan16a$O: $(SRCDIR)/plugins/video/renderer/software/i386/cs.ash \
  $(SRCDIR)/plugins/video/renderer/software/i386/scan.ash
$(OUT)/scan32a$O: $(SRCDIR)/plugins/video/renderer/software/i386/cs.ash \
  $(SRCDIR)/plugins/video/renderer/software/i386/scan.ash
$(OUT)/cpuid$O: $(SRCDIR)/plugins/video/renderer/software/i386/cs.ash

$(OUT)/%$O: $(SRCDIR)/plugins/video/renderer/software/i386/%.asm
	$(DO.COMPILE.ASM) $(NASMFLAGS.SOFT3D)

$(OUT)/%$O: $(SRCDIR)/plugins/video/renderer/software/%.cpp
	$(DO.COMPILE.CPP) $(CFLAGS.PIXEL_LAYOUT)

$(SOFT3D): $(OBJ.SOFT3D) $(LIB.SOFT3D)
	$(DO.PLUGIN)

soft3dclean:
	-$(RMDIR) $(SOFT3D) $(OBJ.SOFT3D) $(OUTDLL)/$(notdir $(INF.SOFT3D))

ifdef DO_DEPEND
dep: $(OUTOS)/soft3d.dep
$(OUTOS)/soft3d.dep: $(SRC.SOFT3D)
	$(DO.DEP1) $(CFLAGS.PIXEL_LAYOUT) $(DO.DEP2)
else
-include $(OUTOS)/soft3d.dep
endif

endif # ifeq ($(MAKESECTION),targets)
