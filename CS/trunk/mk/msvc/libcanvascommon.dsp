# Microsoft Developer Studio Project File - Name="libcanvascommon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libcanvascommon - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libcanvascommon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libcanvascommon.mak" CFG="libcanvascommon - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libcanvascommon - Win32 ExtensiveMemDebug_NR" (based on "Win32 (x86) Static Library")
!MESSAGE "libcanvascommon - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcanvascommon - Win32 Debug_NR" (based on "Win32 (x86) Static Library")
!MESSAGE "libcanvascommon - Win32 Release_NR" (based on "Win32 (x86) Static Library")
!MESSAGE "libcanvascommon - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcanvascommon - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libcanvascommon - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\libcanvascommon\"
# PROP Intermediate_Dir "..\..\out\release\build\libcanvascommon\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_LIB" /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_LIB" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\libcanvascommon\libcanvascommon.res" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\release\libs\libcanvascommon.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /subsystem:windows /machine:I386 /OPT:NOREF  /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcanvascommon - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcanvascommon\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcanvascommon\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcanvascommon\libcanvascommon.res" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcanvascommon_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /subsystem:windows /machine:I386 /debug /pdbtype:sept  /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcanvascommon - Win32 Release_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\libcanvascommon\"
# PROP Intermediate_Dir "..\..\out\release\build\libcanvascommon\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_LIB" /D "_MT" /D "_MBCS" /D "NDEBUG" /D "CS_USE_NEW_RENDERER" /D "_LIB" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "CS_USE_NEW_RENDERER" /D "_LIB" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\libcanvascommon\libcanvascommon.res" /D "NDEBUG" /D "CS_USE_NEW_RENDERER" /D "_LIB" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\release\libs\libcanvascommon.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /subsystem:windows /machine:I386 /OPT:NOREF  /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcanvascommon - Win32 Debug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcanvascommon\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcanvascommon\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "_LIB" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "_LIB" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcanvascommon\libcanvascommon.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "_LIB" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcanvascommon_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /subsystem:windows /machine:I386 /debug /pdbtype:sept  /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcanvascommon - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 
# PROP BASE Output_Dir "memdebug"
# PROP BASE Intermediate_Dir "memdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 
# PROP Output_Dir "..\..\out\memdebug\build\libcanvascommon\"
# PROP Intermediate_Dir "..\..\out\memdebug\build\libcanvascommon\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\memdebug\build\libcanvascommon\libcanvascommon.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /subsystem:windows /machine:I386   /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcanvascommon - Win32 ExtensiveMemDebug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 
# PROP BASE Output_Dir "memdebug"
# PROP BASE Intermediate_Dir "memdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 
# PROP Output_Dir "..\..\out\memdebug\build\libcanvascommon\"
# PROP Intermediate_Dir "..\..\out\memdebug\build\libcanvascommon\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\memdebug\build\libcanvascommon\libcanvascommon.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /subsystem:windows /machine:I386   /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libcanvascommon - Win32 Release"
# Name "libcanvascommon - Win32 Debug"
# Name "libcanvascommon - Win32 Release_NR"
# Name "libcanvascommon - Win32 Debug_NR"
# Name "libcanvascommon - Win32 ExtensiveMemDebug"
# Name "libcanvascommon - Win32 ExtensiveMemDebug_NR"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\video\canvas\common\graph2d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\canvas\common\scrshot.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\canvas\common\x11-keys.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\canvas\common\fontcache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\canvas\common\softfontcache.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\video\canvas\common\graph2d.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\canvas\common\scancode.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\canvas\common\scrshot.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\canvas\common\fontcache.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\canvas\common\softfontcache.h
# End Source File
# End Group
# End Target
# End Project
