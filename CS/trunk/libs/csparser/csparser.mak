# Library description
DESCRIPTION.csparser = Crystal Space world/model parser library

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Library-specific help commands
LIBHELP += \
  $(NEWLINE)echo $"  make csparser     Make the $(DESCRIPTION.csparser)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: csparser

all libs: csparser
csparser:
	$(MAKE_TARGET)
csparserclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp libs/csparser libs/csparser/impexp

CSPARSER.LIB = $(OUT)$(LIB_PREFIX)csparser$(LIB_SUFFIX)
INC.CSPARSER = $(wildcard libs/csparser/*.h include/csparser/*.h)
SRC.CSPARSER = $(wildcard libs/csparser/*.cpp libs/csparser/impexp/*.cpp)
OBJ.CSPARSER = $(addprefix $(OUT),$(notdir $(SRC.CSPARSER:.cpp=$O)))
CFLAGS.CSPARSER = -Ilibs/csterr

TO_INSTALL.STATIC_LIBS += $(CSPARSER.LIB)

MSVC.DSP += CSPARSER
DSP.CSPARSER.NAME = csparser
DSP.CSPARSER.TYPE = library

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: csparser csparserclean

all: $(CSPARSER.LIB)
csparser: $(OUTDIRS) $(CSPARSER.LIB)
clean: csparserclean

$(OUT)csloader$O: libs/csparser/csloader.cpp
	$(DO.COMPILE.CPP) $(CFLAGS.CSPARSER)

$(CSPARSER.LIB): $(OBJ.CSPARSER)
	$(DO.LIBRARY)

csparserclean:
	-$(RM) $(CSPARSER.LIB) $(OBJ.CSPARSER) $(OUTOS)csparser.dep ivcon.log

ifdef DO_DEPEND
dep: $(OUTOS)csparser.dep
$(OUTOS)csparser.dep: $(SRC.CSPARSER)
	$(DO.DEP1) $(CFLAGS.CSPARSER) $(DO.DEP2)
else
-include $(OUTOS)csparser.dep
endif

endif # ifeq ($(MAKESECTION),targets)
