# Microsoft Developer Studio Project File - Name="libcsplugincommon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libcsplugincommon - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libcsplugincommon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libcsplugincommon.mak" CFG="libcsplugincommon - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libcsplugincommon - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsplugincommon - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsplugincommon - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libcsplugincommon - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\libcsplugincommon\"
# PROP Intermediate_Dir "..\..\out\release\build\libcsplugincommon\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_LIB" /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_LIB" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\libcsplugincommon\libcsplugincommon.res" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\release\libs\libcsplugincommon.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /OPT:NOREF /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsplugincommon - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcsplugincommon\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcsplugincommon\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcsplugincommon\libcsplugincommon.res" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcsplugincommon_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsplugincommon - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcsplugincommon\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcsplugincommon\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcsplugincommon\libcsplugincommon.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcsplugincommon_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libcsplugincommon - Win32 Release"
# Name "libcsplugincommon - Win32 Debug"
# Name "libcsplugincommon - Win32 ExtensiveMemDebug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\csplugincommon\canvas\cursorconvert.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\canvas\fontcache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\canvas\graph2d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\canvas\scrshot.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\canvas\softfontcache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\canvas\x11-keys.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\imageloader\commonimagefile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\particlesys\partgen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\particlesys\particle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\render3d\normalizationcube.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\render3d\txtmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\renderstep\basesteploader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\renderstep\basesteptype.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\renderstep\parserenderstep.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\shader\shaderprogram.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\soundloader\sndload.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\soundloader\soundraw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\soundrenderer\shdl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\soundrenderer\slstn.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\canvas\cursorconvert.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\canvas\draw_box.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\canvas\draw_common.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\canvas\draw_line.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\canvas\draw_text.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\canvas\fontcache.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\canvas\graph2d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\canvas\scancode.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\canvas\scrshot.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\canvas\softfontcache.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\canvas\softfontcacheimpl.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\imageloader\commonimagefile.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\particlesys\partgen.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\particlesys\particle.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\render3d\normalizationcube.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\render3d\pixfmt.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\render3d\txtmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\renderstep\basesteploader.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\renderstep\basesteptype.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\renderstep\parserenderstep.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\shader\shaderplugin.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\shader\shaderprogram.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\soundloader\sndload.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\soundloader\soundraw.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\soundrenderer\shdl.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\..\..\include\csplugincommon\soundrenderer\slstn.h
# End Source File
# End Group
# End Target
# End Project
