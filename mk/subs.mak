# This submakefile dynamically computes the name for all plugin, library, and
# application submakefiles and includes them.

ifneq ($(TARGET_MAKEFILE),)
  include $(TARGET_MAKEFILE)
endif

PLUGINS += video/renderer video/canvas # Special defines.
ifeq ($(USE_PLUGINS),yes)
  PLUGINS += $(PLUGINS.DYNAMIC)
endif

# SUBMAKEFILES is cached along with other makefile elements.  If it is still
# empty after including cache.mak, then assume that nothing was cached and
# load the submakefiles manually.
SUBMAKEFILES = $(EMPTY)

include mk/cache.mak

ifeq ($(strip $(SUBMAKEFILES)),)
  SUBMAKEFILES = $(sort \
    docs/docs.mak mk/install.mak mk/msvcgen/msvcgen.mak mk/static.mak \
    $(wildcard $(addsuffix /*.mak,$(addprefix plugins/,$(sort $(PLUGINS)))) \
    scripts/cs-config/csconf.mak \
    apps/*/*.mak \
    apps/tests/*/*.mak \
    apps/tutorial/*/*.mak \
    apps/examples/*/*.mak \
    apps/import/*/*.mak \
    apps/tools/*/*.mak \
    libs/*/*.mak))
  include $(SUBMAKEFILES)
endif

# Include a local makefile where you can add additional targets or whatever.
# This comes in handy, if you don't want to alter the original CS makefiles.

-include mk/local.mak
