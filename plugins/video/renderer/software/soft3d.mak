# This is a subinclude file used to define the rules needed
# to build the 3D software rendering driver.

# Driver description
DESCRIPTION.soft = Crystal Space software 3D renderer

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Driver-specific help commands
DRIVERHELP += \
  $(NEWLINE)echo $"  make soft         Make the $(DESCRIPTION.soft)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: soft
all plugins drivers drivers3d: soft

soft:
	$(MAKE_TARGET) MAKE_DLL=yes
softclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp plugins/video/renderer/software plugins/video/renderer/common

ifeq ($(USE_PLUGINS),yes)
  SOFT3D = $(OUTDLL)soft3d$(DLL)
  LIB.SOFT3D = $(foreach d,$(DEP.SOFT3D),$($d.LIB))
  TO_INSTALL.DYNAMIC_LIBS += $(SOFT3D)
else
  SOFT3D = $(OUT)$(LIB_PREFIX)soft3d$(LIB)
  DEP.EXE += $(SOFT3D)
  CFLAGS.STATIC_SCF += $(CFLAGS.D)SCL_SOFT3D
  TO_INSTALL.STATIC_LIBS += $(SOFT3D)
endif

INC.SOFT3D = $(wildcard plugins/video/renderer/software/*.h) \
  plugins/video/renderer/common/txtmgr.h \
  plugins/video/renderer/common/dtmesh.h \
  plugins/video/renderer/common/dpmesh.h
SRC.SOFT3D = $(wildcard plugins/video/renderer/software/*.cpp) \
  plugins/video/renderer/common/txtmgr.cpp \
  plugins/video/renderer/common/dtmesh.cpp \
  plugins/video/renderer/common/dpmesh.cpp
ifeq ($(NASM.INSTALLED),yes)
  SRC.SOFT3D += $(wildcard plugins/video/renderer/software/i386/*.asm)
endif
OBJ.SOFT3D = \
  $(addprefix $(OUT),$(notdir $(subst .asm,$O,$(SRC.SOFT3D:.cpp=$O))))
DEP.SOFT3D = CSGEOM CSGFXLDR CSUTIL CSSYS
CFG.SOFT3D = data/config/soft3d.cfg

NASMFLAGS.SOFT3D = -i./plugins/video/renderer/software/i386/

TO_INSTALL.CONFIG += $(CFG.SOFT3D)

MSVC.DSP += SOFT3D
DSP.SOFT3D.NAME = soft3d
DSP.SOFT3D.TYPE = plugin
DSP.SOFT3D.RESOURCES = $(wildcard plugins/video/renderer/software/*.inc)

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: soft softclean

# Chain rules
all: $(SOFT3D)
clean: softclean

soft: $(OUTDIRS) $(SOFT3D)

# Extra dependencies not generated by "make depend"
$(OUT)scan8a$O: plugins/video/renderer/software/i386/cs.ash  \
  plugins/video/renderer/software/i386/scan.ash
$(OUT)scan16a$O: plugins/video/renderer/software/i386/cs.ash \
  plugins/video/renderer/software/i386/scan.ash
$(OUT)scan32a$O: plugins/video/renderer/software/i386/cs.ash \
  plugins/video/renderer/software/i386/scan.ash
$(OUT)cpuid$O: plugins/video/renderer/software/i386/cs.ash

$(OUT)%$O: plugins/video/renderer/software/i386/%.asm
	$(DO.COMPILE.ASM) $(NASMFLAGS.SOFT3D)

$(SOFT3D): $(OBJ.SOFT3D) $(LIB.SOFT3D)
	$(DO.PLUGIN)

softclean:
	$(RM) $(SOFT3D) $(OBJ.SOFT3D) $(OUTOS)soft3d.dep

ifdef DO_DEPEND
dep: $(OUTOS)soft3d.dep
$(OUTOS)soft3d.dep: $(SRC.SOFT3D)
	$(DO.DEP)
else
-include $(OUTOS)soft3d.dep
endif

endif # ifeq ($(MAKESECTION),targets)
