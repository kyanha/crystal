#==============================================================================
#
#    Automatic MSVC-compliant workspace and project generation component
#    Copyright (C) 2000-2004 by Eric Sunshine <sunshine@sunshineco.com>
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
#	project file generation mechanism is able to generate project files for
#	Windows-specific plug-in modules even when the makefiles are
#	configured for a different platform, such as Unix, which would not
#	normally process Windows-specific modules.
#
#------------------------------------------------------------------------------
MSVC.MAKE.FLAGS =		\
  3DS.AVAILABLE=yes		\
  DIRECTX.AVAILABLE=yes		\
  FT2.AVAILABLE=yes		\
  GL.AVAILABLE=yes		\
  GLU.AVAILABLE=yes		\
  HAS_CAL3D=yes			\
  JPEG.AVAILABLE=yes		\
  MNG.AVAILABLE=yes		\
  ODE.AVAILABLE=yes		\
  OPENAL.AVAILABLE=yes		\
  PNG.AVAILABLE=yes		\
  PYTHON.AVAILABLE=yes		\
  SOCKET.AVAILABLE=yes		\
  USE_NEW_RENDERER=yes		\
  VORBISFILE.AVAILABLE=yes	\
  ZLIB.AVAILABLE=yes

MSVC.PLUGINS.REQUIRED =         \
  mesh/terrbig			\
  mesh/terrfunc/object		\
  mesh/terrfunc/persist/standard\
  sound/driver/waveoutsd        \
  sound/renderer/ds3d           \
  sound/renderer/software       \
  video/canvas/ddraw            \
  video/canvas/openglwin        \
  video/format/avi              \
  video/format/codecs/rle       \
  video/renderer/opengl		\
  video/renderer/software
