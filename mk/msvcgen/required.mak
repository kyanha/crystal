#==============================================================================
#
#    Automatic MSVC-compliant DSW and DSP generation component
#    Copyright (C) 2000 by Eric Sunshine <sunshine@sunshineco.com>
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Library General Public
#    License as published by the Free Software Foundation; either
#    version 2 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Library General Public License for more details.
#
#    You should have received a copy of the GNU Library General Public
#    License along with this library; if not, write to the Free
#    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#==============================================================================
#------------------------------------------------------------------------------
# required.mak
#
#	This makefile component sets the value of the MSVC.PLUGINS.REQUIRED
#	variable.  MSVC.PLUGINS.REQUIRED supplements the list of plug-in
#	modules defined by its brother variable PLUGINS (see CS/mk/user.mak).
#	It also sets MSVC.MAKE.FLAGS which specifies additional flags to pass
#	to the child "make" invocation.  This is useful when one needs to set
#	additional make variables specially for the synthesis process.
#
#	Like the PLUGINS variable, MSVC.PLUGINS.REQUIRED defines a list of
#	plug-in modules.  Unlike PLUGINS, however, MSVC.PLUGINS.REQUIRED
#	defines the list of plug-in modules for which Visual-C++ DSP project
#	files must be generated, unconditionally, regardless of the currently
#	configured makefile environment.
#
#	Because MSVC.PLUGINS.REQUIRED supplements PLUGINS, the Visual-C++
#	project file generation mechanism is able to generate DSP files for
#	Windows-specific plug-in modules even when the makefiles are
#	configured for a different platform, such as Unix, which would not
#	normally process Windows-specific modules.
#
#------------------------------------------------------------------------------
MSVC.MAKE.FLAGS = \
  HAS_CAL3D=yes \
  3DS.AVAILABLE=yes

MSVC.PLUGINS.REQUIRED =         \
  apps/import/caltocs		\
  cscript/cspython              \
  documentsystem/xml            \
  filesys/vfs			\
  font/server/freefnt2          \
  mesh/impexp/3ds               \
  net/driver/socket		\
  net/driver/ensocket		\
  physics/odedynam              \
  sound/driver/waveoutsd        \
  sound/loader/mod              \
  sound/loader/ogg              \
  sound/renderer/ds3d           \
  sound/renderer/openal         \
  sound/renderer/software       \
  video/canvas/ddraw            \
  video/canvas/openglwin        \
  video/format/avi              \
  video/format/codecs/rle       \
  video/loader/png		\
  video/loader/jpg		\
  video/loader/jng              \
  video/renderer/opengl
