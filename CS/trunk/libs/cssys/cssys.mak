#
# This submakefile requires the following variables defined in
# system-dependent makefile:
#
# SRC.SYS_CSSYS
#   - all system-dependent source files that should be included in cssys
#     library
#

# Library description
DESCRIPTION.cssys = Crystal Space system library

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Library-specific help commands
LIBHELP += \
  $(NEWLINE)echo $"  make cssys        Make the $(DESCRIPTION.cssys)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: cssys

all libs: cssys
cssys:
	$(MAKE_TARGET)
cssysclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp libs/cssys $(filter-out libs/cssys/general/, $(sort $(dir $(SRC.SYS_CSSYS)))) libs/cssys/general
vpath %.c   libs/cssys $(filter-out libs/cssys/general/, $(sort $(dir $(SRC.SYS_CSSYS)))) libs/cssys/general

INC.CSSYS = $(wildcard include/cssys/*.h)
SRC.CSSYS = $(wildcard libs/cssys/*.cpp $(SRC.SYS_CSSYS))
ifeq ($(MAKE_DLL),yes)
  CSSYS.LIB = $(OUT)/$(LIB_PREFIX)cssys_D$(LIB_SUFFIX)
else
  ifneq ($(OS),WIN32)
    CSSYS.LIB = $(OUT)/$(LIB_PREFIX)cssys$(LIB_SUFFIX)
  else
    CSSYS.LIB = $(OUT)/$(LIB_PREFIX)cssys$(LIB)
    DEP.EXE  += $(CSSYS.LIB)
    LIBS.EXE += $(LIBS.DXINPUT) $(LIBS.DXGUID)
  endif
endif
ifeq (,$(strip $(OBJ.CSSYS)))
OBJ.CSSYS = $(addprefix $(OUT)/,$(notdir \
  $(subst .s,$O,$(subst .c,$O,$(SRC.CSSYS:.cpp=$O)))))
endif

TO_INSTALL.STATIC_LIBS += $(CSSYS.LIB)

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: cssys cssysclean

all: $(CSSYS.LIB)
cssys: $(OUTDIRS) $(CSSYS.LIB)
clean: cssysclean

$(CSSYS.LIB): $(OBJ.CSSYS)
	$(DO.LIBRARY)

cssysclean:
	-$(RM) $(CSSYS.LIB) $(OBJ.CSSYS)

ifdef DO_DEPEND
dep: $(OUTOS)/cssys.dep
$(OUTOS)/cssys.dep: $(SRC.CSSYS)
	$(DO.DEP)
else
-include $(OUTOS)/cssys.dep
endif

endif # ifeq ($(MAKESECTION),targets)
