# Application description
DESCRIPTION.scftut = Crystal Space SCF tutorial application
DESCRIPTION.scftutdlls = \
  Crystal Space SCF tutorial application shared libraries

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Application-specific help commands
APPHELP += \
  $(NEWLINE)echo $"  make scftut       Make the $(DESCRIPTION.scftut)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: scftut scftutclean

all apps: scftut
scftut: scftutdlls
	$(MAKE_TARGET)
scftutdlls:
	$(MAKE_TARGET) MAKE_DLL=yes
scftutclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp apps/scftutor

ZOO.EXE = zoo$(EXE)
SRC.ZOO = apps/scftutor/zoo.cpp apps/scftutor/frog.cpp
OBJ.ZOO = $(addprefix $(OUT),$(notdir $(SRC.ZOO:.cpp=$O)))
SRC.DOG = apps/scftutor/dog.cpp
OBJ.DOG = $(addprefix $(OUT),$(notdir $(SRC.DOG:.cpp=$O)))
SRC.WORM = apps/scftutor/worm.cpp
OBJ.WORM = $(addprefix $(OUT),$(notdir $(SRC.WORM:.cpp=$O)))
DESCRIPTION.$(ZOO.EXE) = $(DESCRIPTION.scftut)
DESCRIPTION.$(DOG.DLL) = Sample Shared Dog class
DESCRIPTION.$(WORM.DLL) = Sample Shared Worm class
#TO_INSTALL.EXE+=$(ZOO.EXE)

ifeq ($(USE_SHARED_PLUGINS),yes)
  DOG.DLL = $(OUTDLL)Dog$(DLL)
  WORM.DLL = $(OUTDLL)Worm$(DLL)
  #TO_INSTALL.DYNAMIC_LIBS+=$(DOG.DLL) $(WORM.DLL)
else
  SRC.ZOO += $(SRC.DOG) $(SRC.WORM)
endif

LIB.SCFTUTOR = $(CSUTIL.LIB) $(CSSYS.LIB) $(CSUTIL.LIB)

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: scftut scftutclean

all: $(ZOO.EXE) $(DOG.DLL) $(WORM.DLL)
scftut: $(OUTDIRS) $(ZOO.EXE)
scftutdlls: $(OUTDIRS) $(DOG.DLL) $(WORM.DLL)
clean: scftutclean

$(ZOO.EXE): $(DEP.EXE) $(OBJ.ZOO) $(LIB.SCFTUTOR)
	$(DO.LINK.CONSOLE.EXE)
$(DOG.DLL): $(DEP.EXE) $(OBJ.DOG) $(LIB.SCFTUTOR)
	$(DO.SHARED.PLUGIN)
$(WORM.DLL): $(DEP.EXE) $(OBJ.WORM) $(LIB.SCFTUTOR)
	$(DO.SHARED.PLUGIN)

scftutclean:
	-$(RM) $(ZOO.EXE) $(DOG.DLL) $(WORM.DLL) $(OBJ.ZOO) $(OBJ.DOG) \
	$(OBJ.WORM) $(OUTOS)scftut.dep

ifdef DO_DEPEND
dep: $(OUTOS)scftut.dep
$(OUTOS)scftut.dep: $(SRC.ZOO) $(SRC.DOG) $(SRC.WORM)
	$(DO.DEP)
else
-include $(OUTOS)scftut.dep
endif

endif # ifeq ($(MAKESECTION),targets)
