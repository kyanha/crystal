#==============================================================================
# This is a MacOS/X 2D graphics driver makefile that uses openGL to
# do most of it's work
# Copyright (C) 2001 by Matt Reda <mreda@mac.com>
#==============================================================================

DESCRIPTION.glosx2d = Crystal Space MacOS/X OpenGL 2D driver

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Driver-specific help commands
DRIVERHELP += \
  $(NEWLINE)echo $"  make glosx2d      Make the $(DESCRIPTION.glosx2d)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: glosx2d glosx2dclean
all openglcanvas plugins drivers drivers2d: glosx2d

glosx2d:
	$(MAKE_TARGET) MAKE_DLL=yes DO_GLOSX2D=yes
glosx2dclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#----------------------------------------------------------------- defines ---#
ifeq ($(MAKESECTION),defines)

MACOSX.SOURCE_GLOSX2D_PATHS = $(addprefix $(SRCDIR)/, \
  plugins/video/canvas/macosx/opengl plugins/video/canvas/macosx/common)
MACOSX.HEADER_GLOSX2D_PATHS = \
  $(addprefix $(CFLAGS.I),$(MACOSX.SOURCE_GLOSX2D_PATHS))

# Only add header search paths if actually building this plug-in or if
# USE_PLUGINS=no, in which case this module might be built as the dependency
# of some other module (rather than being built explicitly by the `glosx2d'
# target).
ifeq ($(USE_PLUGINS),no)
  CFLAGS.INCLUDE += $(MACOSX.HEADER_GLOSX2D_PATHS)
else
ifeq ($(DO_GLOSX2D),yes)
  CFLAGS.INCLUDE += $(MACOSX.HEADER_GLOSX2D_PATHS)
endif
endif

endif # ifeq ($(MAKESECTION),defines)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

ifeq ($(USE_PLUGINS),yes)
  GLOSX2D = $(OUTDLL)/glosx2d$(DLL)
  LIB.GLOSX2D = $(foreach d,$(DEP.GLOSX2D),$($d.LIB))
  TO_INSTALL.DYNAMIC_LIBS += $(GLOSX2D)
else
  GLOSX2D = $(OUT)/$(LIB_PREFIX)glosx2d$(LIB)
  DEP.EXE += $(GLOSX2D)
  SCF.STATIC += glosx2d
  TO_INSTALL.STATIC_LIBS += $(GLOSX2D)
endif

INC.GLOSX2D = $(wildcard $(addprefix $(SRCDIR)/,$(INC.COMMON.DRV2D) $(INC.COMMON.DRV2D.OPENGL) \
  $(addsuffix /*.h,$(MACOSX.SOURCE_GLOSX2D_PATHS))))
SRC.GLOSX2D = $(wildcard $(addprefix $(SRCDIR)/,$(SRC.COMMON.DRV2D) $(SRC.COMMON.DRV2D.OPENGL) \
  $(addsuffix /*.cpp,$(MACOSX.SOURCE_GLOSX2D_PATHS)) \
  $(addsuffix /*.m,$(MACOSX.SOURCE_GLOSX2D_PATHS))))
OBJ.GLOSX2D = $(addprefix $(OUT)/, \
  $(notdir $(subst .cpp,$O,$(SRC.GLOSX2D:.m=$O))))
DEP.GLOSX2D = CSSYS CSUTIL

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: glosx2d glosx2dclean

glosx2d: $(OUTDIRS) $(GLOSX2D)

# Rule to make common OpenGL source
$(OUT)/%$O: $(SRCDIR)/plugins/video/canvas/openglcommon/%.cpp
	$(DO.COMPILE.CPP) $(GL.CFLAGS)

# Rules to make OSX sources
$(OUT)/%$O: $(SRCDIR)/plugins/video/canvas/macosx/opengl/%.cpp
	$(DO.COMPILE.CPP) $(GL.CFLAGS)

$(OUT)/%$O: $(SRCDIR)/plugins/video/canvas/macosx/opengl/%.m
	$(DO.COMPILE.C) $(GL.CFLAGS)

$(OUT)/%$O: $(SRCDIR)/plugins/video/canvas/macosx/opengl/%.mm
	$(DO.COMPILE.MM) $(GL.CFLAGS)

$(GLOSX2D): $(OBJ.GLOSX2D) $(LIB.GLOSX2D)
	$(DO.PLUGIN.PREAMBLE) \
	$(DO.PLUGIN.CORE) $(GL.LFLAGS) \
	$(DO.PLUGIN.POSTAMBLE)

clean: glosx2dclean
glosx2dclean:
	$(RM) $(GLOSX2D) $(OBJ.GLOSX2D)

ifdef DO_DEPEND
dep: $(OUTOS)/glosx2d.dep
$(OUTOS)/glosx2d.dep: $(SRC.GLOSX2D)
	$(DO.DEP1) $(GL.CFLAGS) $(MACOSX.HEADER_GLOSX2D_PATHS) $(DO.DEP2)
else
-include $(OUTOS)/glosx2d.dep
endif

endif # ifeq ($(MAKESECTION),targets)
