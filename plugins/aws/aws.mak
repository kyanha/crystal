#------------------------------------------------------------------------------
# Alternate Window Manager plugin submakefile
#------------------------------------------------------------------------------
DESCRIPTION.aws = Alternate Windowing System plug-in

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

PLUGINHELP += \
  $(NEWLINE)echo $"  make aws          Make the $(DESCRIPTION.aws)$"

endif # ifeq ($(MAKESECTION),rootdefines)
#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: aws awsclean
all plugins: aws

aws:
	$(MAKE_TARGET) MAKE_DLL=yes
awsclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)
#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp plugins/aws

ifeq ($(USE_PLUGINS),yes)
  AWS = $(OUTDLL)/aws$(DLL)
  LIB.AWS = $(foreach d,$(DEP.AWS),$($d.LIB))
  TO_INSTALL.DYNAMIC_LIBS += $(AWS)
else
  AWS = $(OUT)/$(LIB_PREFIX)aws$(LIB)
  DEP.EXE += $(AWS)
  SCF.STATIC += aws
  TO_INSTALL.STATIC_LIBS += $(AWS)
endif

INC.AWS = $(wildcard plugins/aws/*.h) $(wildcard include/iaws/*.h)
SRC.AWS = $(filter-out plugins/aws/sllex.cpp plugins/aws/slp.cpp, \
  $(wildcard plugins/aws/*.cpp)) plugins/aws/sllex.cpp plugins/aws/slp.cpp
OBJ.AWS = $(addprefix $(OUT)/,$(notdir $(SRC.AWS:.cpp=$O)))
DEP.AWS = CSUTIL CSSYS CSUTIL CSGEOM CSTOOL CSGFX

TO_INSTALL.DATA += data/awsdef.zip

MSVC.DSP += AWS
DSP.AWS.NAME = aws
DSP.AWS.TYPE = plugin
DSP.AWS.CFLAGS = /D "YY_NEVER_INTERACTIVE"

endif # ifeq ($(MAKESECTION),postdefines)
#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: aws awsclean
aws: $(OUTDIRS) $(AWS)

$(AWS): $(OBJ.AWS) $(LIB.AWS)
	$(DO.PLUGIN)

ifneq (,$(FLEXBIN))
ifneq (,$(BISONBIN))

plugins/aws/sllex.cpp: \
plugins/aws/slp.hpp plugins/aws/skinlang.flx plugins/aws/slp.cpp
	flex -L -Splugins/aws/flex.skl -t plugins/aws/skinlang.flx > \
	plugins/aws/sllex.cpp

plugins/aws/slp.cpp: plugins/aws/skinlang.bsn
	bison --no-lines -d -p aws -o plugins/aws/slp.cpp \
	plugins/aws/skinlang.bsn

plugins/aws/slp.hpp: plugins/aws/slp.cpp
	@if [ -f "plugins/aws/slp.cpp.h" ]; then \
	$(MV) plugins/aws/slp.cpp.h plugins/aws/slp.hpp; \
	fi
endif
endif
clean: awsclean
awsclean:
	-$(RM) $(AWS) $(OBJ.AWS)

ifdef DO_DEPEND
dep: $(OUTOS)/aws.dep
$(OUTOS)/aws.dep: $(SRC.AWS)
	$(DO.DEP)
else
-include $(OUTOS)/aws.dep
endif

endif # ifeq ($(MAKESECTION),targets)
